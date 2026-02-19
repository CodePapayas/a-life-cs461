/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462

  test_auto_save.cpp
  Integration tests for the AutoSave system.

  Prerequisites: same as test_persistence.cpp
*/

#include "../include/db_connector.h"
#include "../include/save_manager.h"
#include "../include/auto_save.h"
#include "../include/simulation_state.h"
#include "../include/fitness_calculator.h"

#include <iostream>
#include <cassert>
#include <ctime>
#include <memory>

using namespace std;

static int totalTests = 0, passedTests = 0;
#define TEST(name) totalTests++; cout << "[TEST] " << (name) << "... "; bool ok = true;
#define CHECK(cond) if (!(cond)) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #cond; }
#define END_TEST() if (ok) { cout << "PASS"; passedTests++; } else { cout << " <-- FAILED"; } cout << "\n";

// -------------------------------------------------------
// Helpers
// -------------------------------------------------------

static auto makeStack() {
    auto db = make_shared<DBConnector>(DBConnectionParams::fromEnv());
    auto sm = make_shared<SaveManager>(db);
    sm->initSchema(ALIFE_PROJECT_ROOT "/db/schema.sql");
    return make_pair(db, sm);
}

static SimulationSavePayload dummyPayload(uint64_t tick) {
    SimulationSavePayload p;
    p.slotName      = "";  // AutoSave sets the name
    p.description   = "auto test";
    p.tick          = tick;
    p.realTimestamp = static_cast<double>(time(nullptr));
    p.worldWidth    = 64;
    p.worldHeight   = 64;
    p.totalEnergy   = 200.0;

    AgentSaveData a;
    a.agentId = 1; a.energy = 50.0; a.maxEnergy = 100.0; a.age = tick;
    a.fitness = FitnessCalculator::calculateFitness(50.0, 100.0, tick, 100.0, 80.0, 1);
    p.agents.push_back(a);

    return p;
}

// -------------------------------------------------------
// Tests
// -------------------------------------------------------

void testAutoSaveConfig() {
    TEST("AutoSave - configure persists to DB and reloads")
    try {
        auto [db, sm] = makeStack();

        AutoSaveConfig cfg;
        cfg.intervalTicks = 50;
        cfg.maxAutoSaves  = 3;
        cfg.slotPrefix    = "testas";
        cfg.enabled       = true;

        {
            AutoSave as(sm, /*loadConfigFromDB=*/false);
            as.configure(cfg);
        }

        // Re-create and load from DB
        AutoSave as2(sm, /*loadConfigFromDB=*/true);
        CHECK(as2.config().intervalTicks == 50);
        CHECK(as2.config().maxAutoSaves  == 3);
        CHECK(as2.config().slotPrefix    == "testas");
        CHECK(as2.isEnabled());

    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

void testAutoSaveFires() {
    TEST("AutoSave - tick() fires after intervalTicks")
    try {
        auto [db, sm] = makeStack();

        AutoSaveConfig cfg;
        cfg.intervalTicks = 10;
        cfg.maxAutoSaves  = 5;
        cfg.slotPrefix    = "fire_test";
        cfg.enabled       = true;

        AutoSave as(sm, false);
        as.configure(cfg);

        // Clean up leftover test slots
        for (uint32_t i = 0; i < cfg.maxAutoSaves; ++i)
            sm->deleteSave("fire_test_" + to_string(i));

        bool fired = false;

        // Ticks 0-9: should NOT fire (< intervalTicks elapsed from last=0)
        for (uint64_t t = 1; t < 10; ++t) {
            bool r = as.tick(t, [&]{ return dummyPayload(t); });
            CHECK(!r);
        }

        // Tick 10: should fire
        fired = as.tick(10, [&]{ return dummyPayload(10); });
        CHECK(fired);
        CHECK(as.stats().totalAutoSavesDone == 1);

        // Tick 11-19: should NOT fire
        for (uint64_t t = 11; t < 20; ++t) {
            bool r = as.tick(t, [&]{ return dummyPayload(t); });
            CHECK(!r);
        }

        // Tick 20: should fire again
        fired = as.tick(20, [&]{ return dummyPayload(20); });
        CHECK(fired);
        CHECK(as.stats().totalAutoSavesDone == 2);

        // Clean up
        as.clearAllAutoSaves();

    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

void testAutoSaveRotation() {
    TEST("AutoSave - slot rotation stays within maxAutoSaves")
    try {
        auto [db, sm] = makeStack();

        AutoSaveConfig cfg;
        cfg.intervalTicks = 1;  // Fire every tick for this test
        cfg.maxAutoSaves  = 3;
        cfg.slotPrefix    = "rot_test";
        cfg.enabled       = true;

        AutoSave as(sm, false);
        as.configure(cfg);
        as.clearAllAutoSaves();

        // Fire 6 saves — pool size is 3, so expects <= 3 in DB
        for (uint64_t t = 1; t <= 6; ++t) {
            as.tick(t, [&]{ return dummyPayload(t); });
        }

        auto slots = as.listAutoSaves();
        CHECK(slots.size() <= 3);

        as.clearAllAutoSaves();

    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

void testAutoSaveLoadLatest() {
    TEST("AutoSave - loadLatest returns most recent save")
    try {
        auto [db, sm] = makeStack();

        AutoSaveConfig cfg;
        cfg.intervalTicks = 1;
        cfg.maxAutoSaves  = 5;
        cfg.slotPrefix    = "latest_test";
        cfg.enabled       = true;

        AutoSave as(sm, false);
        as.configure(cfg);
        as.clearAllAutoSaves();

        // Save three times with distinct ticks
        for (uint64_t t : {10ULL, 20ULL, 30ULL}) {
            auto p = dummyPayload(t);
            as.forceSave(p);
        }

        SimulationSavePayload latest;
        bool found = as.loadLatest(latest);
        CHECK(found);
        // Most recent should be tick=30
        CHECK(latest.tick == 30);

        as.clearAllAutoSaves();
        for (auto* r : latest.resources) delete r;

    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

void testAutoSaveDisable() {
    TEST("AutoSave - disabled: tick() never fires")
    try {
        auto [db, sm] = makeStack();

        AutoSaveConfig cfg;
        cfg.intervalTicks = 1;
        cfg.maxAutoSaves  = 5;
        cfg.slotPrefix    = "disabled_test";
        cfg.enabled       = false;

        AutoSave as(sm, false);
        as.configure(cfg);

        for (uint64_t t = 1; t <= 100; ++t) {
            bool r = as.tick(t, [&]{ return dummyPayload(t); });
            CHECK(!r);
        }
        CHECK(as.stats().totalAutoSavesDone == 0);

    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

// -------------------------------------------------------
// main
// -------------------------------------------------------

int main() {
    cout << "=== AutoSave Integration Tests ===\n\n";

    testAutoSaveConfig();
    testAutoSaveFires();
    testAutoSaveRotation();
    testAutoSaveLoadLatest();
    testAutoSaveDisable();

    cout << "\n" << passedTests << " / " << totalTests << " tests passed.\n";
    return (passedTests == totalTests) ? 0 : 1;
}
