/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462

  test_persistence.cpp
  Integration tests for SaveManager / DBConnector.

  Prerequisites:
    Set ALIFE_DB_HOST / ALIFE_DB_PORT / ALIFE_DB_NAME / ALIFE_DB_USER / ALIFE_DB_PASS
    (or accept defaults: localhost:5432/alife_sim/postgres)

  The test database+schema must exist before running.
  Run:  psql -U postgres -c "CREATE DATABASE alife_sim;"
  Then: psql -U postgres -d alife_sim -f db/schema.sql
*/

#include "../include/db_connector.h"
#include "../include/save_manager.h"
#include "../include/simulation_state.h"
#include "../include/resource_node.h"
#include "../include/circular_buffer.h"
#include "../include/fitness_calculator.h"

#include <iostream>
#include <cassert>
#include <ctime>
#include <memory>

using namespace std;

// -------------------------------------------------------
// Mini test framework (matches style in existing tests)
// -------------------------------------------------------
static int totalTests = 0, passedTests = 0;
#define TEST(name) totalTests++; cout << "[TEST] " << (name) << "... "; bool ok = true;
#define CHECK(cond) if (!(cond)) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #cond; }
#define END_TEST() if (ok) { cout << "PASS"; passedTests++; } else { cout << " <-- FAILED"; } cout << "\n";

// -------------------------------------------------------
// Helpers
// -------------------------------------------------------

static shared_ptr<DBConnector> makeDB() {
    auto params = DBConnectionParams::fromEnv();
    return make_shared<DBConnector>(params);
}

static SimulationSavePayload makeTestPayload(const string& slotName) {
    SimulationSavePayload p;
    p.slotName      = slotName;
    p.description   = "Unit test save";
    p.tick          = 42;
    p.realTimestamp = static_cast<double>(time(nullptr));
    p.worldWidth    = 100;
    p.worldHeight   = 100;
    p.totalEnergy   = 500.0;

    // Two dummy agents
    AgentSaveData a1;
    a1.agentId    = 1; a1.posX = 10; a1.posY = 20;
    a1.energy = 80.0; a1.maxEnergy = 100.0;
    a1.age = 50; a1.energyGained = 200.0; a1.energySpent = 150.0;
    a1.offspring = 2;
    a1.fitness = FitnessCalculator::calculateFitness(
        a1.energy, a1.maxEnergy, a1.age, a1.energyGained, a1.energySpent, a1.offspring);
    // Small dummy genome
    a1.genomeBytes = {0x01, 0x02, 0x03, 0x04, 0x05};

    AgentSaveData a2;
    a2.agentId = 2; a2.posX = 30; a2.posY = 40;
    a2.energy = 50.0; a2.maxEnergy = 100.0;
    a2.age = 10; a2.energyGained = 80.0; a2.energySpent = 60.0;
    a2.offspring = 0;
    a2.fitness = FitnessCalculator::calculateFitness(
        a2.energy, a2.maxEnergy, a2.age, a2.energyGained, a2.energySpent, a2.offspring);

    p.agents = {a1, a2};

    return p;
}

// -------------------------------------------------------
// Tests
// -------------------------------------------------------

void testDBConnection() {
    TEST("DBConnector - connects to PostgreSQL")
    try {
        auto db = makeDB();
        CHECK(db->isConnected());
    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

void testSaveAndLoad() {
    TEST("SaveManager - save then load round-trip")
    try {
        auto db = makeDB();
        SaveManager sm(db);
        sm.initSchema(ALIFE_PROJECT_ROOT "/db/schema.sql");

        // Clean up any previous test slot
        sm.deleteSave("test_slot_roundtrip");

        auto payload = makeTestPayload("test_slot_roundtrip");
        int id = sm.save(payload);
        CHECK(id > 0);

        SimulationSavePayload loaded;
        bool found = sm.load("test_slot_roundtrip", loaded);
        CHECK(found);
        CHECK(loaded.tick == 42);
        CHECK(loaded.worldWidth  == 100);
        CHECK(loaded.worldHeight == 100);
        CHECK(loaded.agents.size() == 2);

        // Verify first agent round-trips correctly
        CHECK(loaded.agents[0].agentId == 1);
        CHECK(loaded.agents[0].posX    == 10);
        CHECK(loaded.agents[0].posY    == 20);
        CHECK(loaded.agents[0].offspring == 2);

        // Genome should survive the round-trip
        CHECK(loaded.agents[0].genomeBytes.size() == 5);
        CHECK(loaded.agents[0].genomeBytes[2]     == 0x03);

        // Clean up
        sm.deleteSave("test_slot_roundtrip");

        // Free heap-allocated resources
        for (auto* r : loaded.resources) delete r;

    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

void testStateHistoryPersisted() {
    TEST("SaveManager - circular buffer history persisted")
    try {
        auto db = makeDB();
        SaveManager sm(db);
        sm.deleteSave("test_slot_history");

        CircularBuffer<SimulationState> buf(5);
        for (int i = 0; i < 5; i++) {
            SimulationState s;
            s.tick         = static_cast<uint64_t>(i);
            s.agentCount   = 10 + i;
            s.totalEnergy  = 100.0 * (i + 1);
            buf.push(s);
        }

        auto payload        = makeTestPayload("test_slot_history");
        payload.stateHistory = &buf;
        sm.save(payload);

        // Verify rows exist in the DB
        PGResultGuard r(db->execParams(
            "SELECT COUNT(*) FROM simulation_state_history h "
            "JOIN simulation_saves s ON h.save_id = s.id "
            "WHERE s.slot_name = $1",
            {"test_slot_history"}
        ));
        int count = stoi(r.val(0, 0));
        CHECK(count == 5);

        sm.deleteSave("test_slot_history");

    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

void testSlotListing() {
    TEST("SaveManager - slot listing")
    try {
        auto db = makeDB();
        SaveManager sm(db);
        sm.deleteSave("test_list_a");
        sm.deleteSave("test_list_b");

        sm.save(makeTestPayload("test_list_a"));
        sm.save(makeTestPayload("test_list_b"));

        auto slots = sm.listSaves();
        bool foundA = false, foundB = false;
        for (const auto& s : slots) {
            if (s.slotName == "test_list_a") foundA = true;
            if (s.slotName == "test_list_b") foundB = true;
        }
        CHECK(foundA && foundB);
        CHECK(sm.slotExists("test_list_a"));
        CHECK(!sm.slotExists("does_not_exist_xyz"));

        sm.deleteSave("test_list_a");
        sm.deleteSave("test_list_b");

    } catch (const exception& e) {
        ok = false;
        cout << "\n  Exception: " << e.what();
    }
    END_TEST()
}

void testOverwriteSlot() {
    TEST("SaveManager - overwrite existing slot")
    try {
        auto db = makeDB();
        SaveManager sm(db);
        sm.deleteSave("test_overwrite");

        auto p1 = makeTestPayload("test_overwrite");
        p1.tick = 10;
        sm.save(p1);

        auto p2 = makeTestPayload("test_overwrite");
        p2.tick = 99;
        sm.save(p2);  // Should replace p1

        SimulationSavePayload loaded;
        sm.load("test_overwrite", loaded);
        CHECK(loaded.tick == 99);  // Old tick gone, new tick present

        sm.deleteSave("test_overwrite");
        for (auto* r : loaded.resources) delete r;

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
    cout << "=== Persistence Integration Tests ===\n\n";

    testDBConnection();
    testSaveAndLoad();
    testStateHistoryPersisted();
    testSlotListing();
    testOverwriteSlot();

    cout << "\n" << passedTests << " / " << totalTests << " tests passed.\n";
    return (passedTests == totalTests) ? 0 : 1;
}
