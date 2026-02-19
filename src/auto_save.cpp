/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462
*/

#include "../include/auto_save.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <ctime>

using namespace std;

void AutoSaveConfig::validate() const {
    if (intervalTicks == 0)
        throw invalid_argument("AutoSaveConfig: intervalTicks must be > 0");
    if (maxAutoSaves == 0)
        throw invalid_argument("AutoSaveConfig: maxAutoSaves must be > 0");
    if (slotPrefix.empty())
        throw invalid_argument("AutoSaveConfig: slotPrefix must not be empty");
}

AutoSave::AutoSave(shared_ptr<SaveManager> saveManager, bool loadConfigFromDB)
    : m_sm(move(saveManager))
{
    if (!m_sm) throw invalid_argument("AutoSave: null SaveManager");
    if (loadConfigFromDB) {
        try {
            loadConfig();
        } catch (...) {
            // DB may not have a config row yet — keep defaults
        }
    }
}

void AutoSave::configure(const AutoSaveConfig& cfg) {
    cfg.validate();
    m_cfg = cfg;
    persistConfig();
}

void AutoSave::loadConfig() {
    PGResultGuard r(db().execParams(
        "SELECT interval_ticks, max_auto_saves, enabled, "
        "       slot_prefix, last_auto_save_tick "
        "FROM   auto_save_config ORDER BY id LIMIT 1",
        {}
    ));

    if (r.rows() == 0) return;  // Nothing stored yet, keep defaults

    m_cfg.intervalTicks      = static_cast<uint32_t>(stoul(r.val(0, 0)));
    m_cfg.maxAutoSaves       = static_cast<uint32_t>(stoul(r.val(0, 1)));
    m_cfg.enabled            = (r.val(0, 2) == "t" || r.val(0, 2) == "true");
    m_cfg.slotPrefix         = r.val(0, 3);
    m_stats.lastAutoSaveTick = stoull(r.val(0, 4));
}

void AutoSave::persistConfig() const {
    PGResultGuard r(db().execParams(
        "UPDATE auto_save_config "
        "SET interval_ticks      = $1, "
        "    max_auto_saves      = $2, "
        "    enabled             = $3, "
        "    slot_prefix         = $4, "
        "    last_auto_save_tick = $5, "
        "    updated_at          = NOW() ",
        {
            to_string(m_cfg.intervalTicks),
            to_string(m_cfg.maxAutoSaves),
            m_cfg.enabled ? "true" : "false",
            m_cfg.slotPrefix,
            to_string(m_stats.lastAutoSaveTick)
        }
    ));

    // If the update touched 0 rows the table was empty — insert a fresh row
    if (string(PQcmdTuples(r)) == "0") {
        PGResultGuard ins(db().execParams(
            "INSERT INTO auto_save_config "
            "(interval_ticks, max_auto_saves, enabled, slot_prefix, last_auto_save_tick) "
            "VALUES ($1,$2,$3,$4,$5)",
            {
                to_string(m_cfg.intervalTicks),
                to_string(m_cfg.maxAutoSaves),
                m_cfg.enabled ? "true" : "false",
                m_cfg.slotPrefix,
                to_string(m_stats.lastAutoSaveTick)
            }
        ));
    }
}

void AutoSave::setEnabled(bool enabled) {
    m_cfg.enabled = enabled;
    persistConfig();
}

// Pick the next slot in the rotating pool by wrapping the total-saves count
string AutoSave::nextSlotName() {
    uint32_t idx = m_stats.totalAutoSavesDone % m_cfg.maxAutoSaves;
    return m_cfg.slotPrefix + "_" + to_string(idx);
}

// Called every tick — does nothing unless enough ticks have elapsed
bool AutoSave::tick(uint64_t currentTick,
                    function<SimulationSavePayload()> payloadBuilder) {
    if (!m_cfg.enabled) return false;

    uint64_t elapsed = currentTick - m_stats.lastAutoSaveTick;
    if (elapsed < static_cast<uint64_t>(m_cfg.intervalTicks)) return false;

    // Time to save — build the payload only now (not every tick)
    SimulationSavePayload payload = payloadBuilder();
    payload.tick = currentTick;
    if (payload.realTimestamp == 0.0)
        payload.realTimestamp = static_cast<double>(time(nullptr));

    try {
        doSave(payload);
        m_stats.lastAutoSaveTick  = currentTick;
        m_stats.lastSaveSucceeded = true;
        m_stats.lastError.clear();
        persistConfig();  // Write updated tick count back to DB
    } catch (const exception& e) {
        m_stats.lastSaveSucceeded = false;
        m_stats.lastError = e.what();
        return false;  // Non-fatal — simulation keeps running
    }

    return true;
}

int AutoSave::forceSave(const SimulationSavePayload& payload) {
    return doSave(payload);
}

int AutoSave::doSave(const SimulationSavePayload& payload) {
    SimulationSavePayload p = payload;
    p.slotName    = nextSlotName();
    p.description = "Auto-save @ tick " + to_string(p.tick);
    if (p.realTimestamp == 0.0)
        p.realTimestamp = static_cast<double>(time(nullptr));

    int saveId = m_sm->saveInternal(p, /*isAutoSave=*/true);

    m_stats.totalAutoSavesDone++;
    m_stats.currentSlotIndex = (m_stats.totalAutoSavesDone - 1) % m_cfg.maxAutoSaves;

    pruneOldAutoSaves();  // Keep the pool from growing beyond maxAutoSaves
    return saveId;
}

void AutoSave::pruneOldAutoSaves() {
    // Delete anything beyond the N most-recent auto-saves
    // (rotation reuses slot names so this is mostly a safety net)
    PGResultGuard r(db().execParams(
        "DELETE FROM simulation_saves "
        "WHERE  is_auto_save = TRUE "
        "  AND  id NOT IN ( "
        "    SELECT id FROM simulation_saves "
        "    WHERE  is_auto_save = TRUE "
        "    ORDER  BY created_at DESC "
        "    LIMIT  $1 "
        "  )",
        {to_string(m_cfg.maxAutoSaves)}
    ));
}

void AutoSave::clearAllAutoSaves() {
    PGResultGuard r(db().execParams(
        "DELETE FROM simulation_saves WHERE is_auto_save = TRUE",
        {}
    ));
}

vector<SaveSlotInfo> AutoSave::listAutoSaves() const {
    return m_sm->listAutoSaves();
}

bool AutoSave::loadLatest(SimulationSavePayload& outPayload) const {
    auto slots = m_sm->listAutoSaves();
    if (slots.empty()) return false;
    return m_sm->load(slots.front().slotName, outPayload);
}

bool AutoSave::loadNthLatest(size_t n, SimulationSavePayload& outPayload) const {
    auto slots = m_sm->listAutoSaves();
    if (n >= slots.size()) return false;
    return m_sm->load(slots[n].slotName, outPayload);
}
