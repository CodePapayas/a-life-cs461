/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462
*/
#pragma once

#include "save_manager.h"

#include <string>
#include <memory>
#include <cstdint>
#include <functional>

using namespace std;

/** AutoSaveConfig - settings that drive the tick-based auto-save loop */
struct AutoSaveConfig {
    uint32_t intervalTicks = 100;       // Save every N ticks
    uint32_t maxAutoSaves  = 5;         // Rotating pool size
    bool     enabled       = true;
    string   slotPrefix    = "autosave";// Slots named <prefix>_<index>

    void validate() const;  // Throws invalid_argument if something looks wrong
};

/** AutoSaveStats - what happened last time we saved */
struct AutoSaveStats {
    uint64_t lastAutoSaveTick   = 0;
    uint32_t totalAutoSavesDone = 0;
    uint32_t currentSlotIndex   = 0;  // Which slot was written last
    bool     lastSaveSucceeded  = true;
    string   lastError;
};

/** AutoSave - tick-driven save system; wraps SaveManager with a rotating slot pool */
class AutoSave {
public:
    // Loads saved config from DB on construction if loadConfigFromDB is true
    explicit AutoSave(shared_ptr<SaveManager> saveManager,
                      bool loadConfigFromDB = true);

    ~AutoSave() = default;

    AutoSave(const AutoSave&) = delete;
    AutoSave& operator=(const AutoSave&) = delete;

    void configure(const AutoSaveConfig& cfg);  // Applies + persists new config
    void loadConfig();                          // Re-read config from DB
    void persistConfig() const;                 // Write current config to DB
    void setEnabled(bool enabled);              // Toggle without changing other settings

    bool isEnabled() const { return m_cfg.enabled; }
    const AutoSaveConfig& config() const { return m_cfg; }

    // Call once per tick; payloadBuilder only invoked when a save is actually due
    bool tick(uint64_t currentTick,
              function<SimulationSavePayload()> payloadBuilder);

    int forceSave(const SimulationSavePayload& payload);  // Force save now, ignores interval

    const AutoSaveStats& stats() const { return m_stats; }

    vector<SaveSlotInfo> listAutoSaves() const;                           // All auto-saves, newest first
    bool loadLatest(SimulationSavePayload& outPayload) const;             // Most recent
    bool loadNthLatest(size_t n, SimulationSavePayload& outPayload) const;// 0 = newest

    void pruneOldAutoSaves(); // Trim pool to maxAutoSaves; called after each save
    void clearAllAutoSaves(); // Wipe all auto-save rows from DB

private:
    shared_ptr<SaveManager> m_sm;
    AutoSaveConfig          m_cfg;
    AutoSaveStats           m_stats;

    int    doSave(const SimulationSavePayload& payload);  // Actual save logic
    string nextSlotName();                                // Picks the next slot in rotation

    DBConnector& db() const { return *m_sm->m_db; }  // Borrowed from SaveManager
};
