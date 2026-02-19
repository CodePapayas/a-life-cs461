/*
* Author: Kai Lindskog-Coffin
* Oregon State University
* CS 462
*/
#pragma once

#include "db_connector.h"
#include "simulation_state.h"
#include "resource_node.h"
#include "circular_buffer.h"

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <functional>

using namespace std;

// Flat snapshot of a single agent — fill this from Agent class before saving
struct AgentSaveData {
    uint64_t agentId      = 0;
    int32_t  posX         = 0;
    int32_t  posY         = 0;
    double   energy       = 0.0;
    double   maxEnergy    = 0.0;
    uint64_t age          = 0;
    double   energyGained = 0.0;
    double   energySpent  = 0.0;
    uint32_t offspring    = 0;
    double   fitness      = 0.0;

    vector<uint8_t> genomeBytes;    // Raw genome, SaveManager compresses before storing
};

// Everything needed to fully reconstruct a simulation — passed to save() and filled by load()
struct SimulationSavePayload {
    string   slotName;                          // Unique name for this save slot
    string   description;                       // Human-readable label
    uint64_t tick          = 0;
    double   realTimestamp = 0.0;               // Seconds since epoch

    vector<AgentSaveData> agents;
    vector<ResourceNode*> resources;            // Non-owning pointers

    int32_t worldWidth  = 0;
    int32_t worldHeight = 0;
    double  totalEnergy = 0.0;

    // Optionally include the circular buffer so recent tick history gets persisted too
    const CircularBuffer<SimulationState>* stateHistory = nullptr;
};

// What gets returned when you list available saves
struct SaveSlotInfo {
    int      id            = 0;
    string   slotName;
    string   description;
    uint64_t tick          = 0;
    double   realTimestamp = 0.0;
    int      agentCount    = 0;
    int      resourceCount = 0;
    double   totalEnergy   = 0.0;
    double   avgFitness    = 0.0;
    bool     isAutoSave    = false;
    string   createdAt;             // ISO 8601 timestamp string from DB
};

/**
 * SaveManager - Writes and reads full simulation state to/from PostgreSQL
 * Handles agents, resources, environment, and circular buffer history
 * Existing save slots are replaced (delete + re-insert) in a single transaction
 */
class SaveManager {
public:
    // Shared DB connection — SaveManager borrows it, doesn't own it
    explicit SaveManager(shared_ptr<DBConnector> db);
    ~SaveManager() = default;

    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    // Apply db/schema.sql — safe to call repeatedly (all DDL uses IF NOT EXISTS)
    void initSchema(const string& schemaFilePath = "db/schema.sql");

    int  save(const SimulationSavePayload& payload);                // Returns new save_id
    bool load(const string& slotName, SimulationSavePayload& out);  // false if not found
    bool deleteSave(const string& slotName);                        // false if not found

    vector<SaveSlotInfo> listSaves() const;      // All slots, newest first
    vector<SaveSlotInfo> listAutoSaves() const;  // Auto-saves only, newest first
    bool slotExists(const string& slotName) const;

    // Compress/decompress genome bytes with zlib (no-op if zlib not compiled in)
    static vector<uint8_t> compressBytes(const vector<uint8_t>& data);
    static vector<uint8_t> decompressBytes(const vector<uint8_t>& data,
                                            size_t expectedUncompressedSize = 0);
    static bool compressionEnabled();   // True if ALIFE_USE_ZLIB was defined at build

private:
    shared_ptr<DBConnector> m_db;

    int  upsertSaveSlot(const SimulationSavePayload& payload, bool isAutoSave);
    void saveAgents     (int saveId, const vector<AgentSaveData>& agents);
    void saveResources  (int saveId, const vector<ResourceNode*>& resources);
    void saveEnvironment(int saveId, const SimulationSavePayload& payload);
    void saveHistory    (int saveId, const CircularBuffer<SimulationState>* hist);

    void loadAgents   (int saveId, vector<AgentSaveData>& outAgents);
    void loadResources(int saveId, SimulationSavePayload& out);

    static string       resourceTypeToString(ResourceType t);
    static ResourceType resourceTypeFromInt(int v);

    // Same as save() but lets AutoSave mark the slot as an auto-save
    int saveInternal(const SimulationSavePayload& payload, bool isAutoSave);

    friend class AutoSave;  // AutoSave needs access to saveInternal and m_db
};
