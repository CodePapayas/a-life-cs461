-- ============================================================
-- A-Life Simulation Save/Load System - PostgreSQL Schema
-- Author: Kai Lindskog
-- Oregon State University 
-- CS 462
-- ============================================================

-- Extensions
CREATE EXTENSION IF NOT EXISTS "pgcrypto";  -- For gen_random_uuid if needed

-- ============================================================
-- Core save-slot metadata
-- Each row represents one named save (user or auto-save)
-- ============================================================
CREATE TABLE IF NOT EXISTS simulation_saves (
    id              SERIAL          PRIMARY KEY,
    slot_name       VARCHAR(64)     NOT NULL UNIQUE,
    description     TEXT,
    tick            BIGINT          NOT NULL,
    real_timestamp  DOUBLE PRECISION NOT NULL,   -- seconds since epoch
    agent_count     INTEGER         NOT NULL DEFAULT 0,
    resource_count  INTEGER         NOT NULL DEFAULT 0,
    total_energy    DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    average_fitness DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    is_auto_save    BOOLEAN         NOT NULL DEFAULT FALSE,
    compressed      BOOLEAN         NOT NULL DEFAULT TRUE,
    created_at      TIMESTAMPTZ     NOT NULL DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_saves_slot_name
    ON simulation_saves (slot_name);

CREATE INDEX IF NOT EXISTS idx_saves_auto_created
    ON simulation_saves (is_auto_save, created_at);

-- ============================================================
-- Per-agent state (one row per agent per save)
-- ============================================================
CREATE TABLE IF NOT EXISTS simulation_agent_states (
    id              SERIAL          PRIMARY KEY,
    save_id         INTEGER         NOT NULL
                        REFERENCES simulation_saves(id) ON DELETE CASCADE,
    agent_id        BIGINT          NOT NULL,
    pos_x           INTEGER         NOT NULL,
    pos_y           INTEGER         NOT NULL,
    energy          DOUBLE PRECISION NOT NULL,
    max_energy      DOUBLE PRECISION NOT NULL,
    age             BIGINT          NOT NULL,
    energy_gained   DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    energy_spent    DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    offspring       INTEGER         NOT NULL DEFAULT 0,
    fitness         DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    -- Genome stored as compressed binary (zlib deflate)
    genome_data     BYTEA,
    genome_length   INTEGER         NOT NULL DEFAULT 0   -- uncompressed byte count
);

CREATE INDEX IF NOT EXISTS idx_agent_states_save_id
    ON simulation_agent_states (save_id);

-- ============================================================
-- Per-resource state
-- ============================================================
CREATE TABLE IF NOT EXISTS simulation_resource_states (
    id              SERIAL          PRIMARY KEY,
    save_id         INTEGER         NOT NULL
                        REFERENCES simulation_saves(id) ON DELETE CASCADE,
    resource_id     BIGINT          NOT NULL,
    pos_x           INTEGER         NOT NULL,
    pos_y           INTEGER         NOT NULL,
    resource_type   INTEGER         NOT NULL,  -- maps to ResourceType enum
    current_energy  DOUBLE PRECISION NOT NULL,
    max_energy      DOUBLE PRECISION NOT NULL,
    renewable       BOOLEAN         NOT NULL DEFAULT FALSE,
    regen_rate      DOUBLE PRECISION NOT NULL DEFAULT 0.0
);

CREATE INDEX IF NOT EXISTS idx_resource_states_save_id
    ON simulation_resource_states (save_id);

-- ============================================================
-- Environment / world metadata (one row per save)
-- ============================================================
CREATE TABLE IF NOT EXISTS simulation_environment_state (
    id              SERIAL          PRIMARY KEY,
    save_id         INTEGER         NOT NULL UNIQUE
                        REFERENCES simulation_saves(id) ON DELETE CASCADE,
    world_width     INTEGER         NOT NULL DEFAULT 0,
    world_height    INTEGER         NOT NULL DEFAULT 0,
    total_energy    DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    -- Extensible key-value store for future environment fields
    extra_data      JSONB
);

-- ============================================================
-- History: lightweight circular-buffer snapshots persisted over time
-- Mirrors the in-memory CircularBuffer<SimulationState>
-- ============================================================
CREATE TABLE IF NOT EXISTS simulation_state_history (
    id              SERIAL          PRIMARY KEY,
    save_id         INTEGER         NOT NULL
                        REFERENCES simulation_saves(id) ON DELETE CASCADE,
    tick            BIGINT          NOT NULL,
    real_timestamp  DOUBLE PRECISION NOT NULL,
    agent_count     INTEGER         NOT NULL DEFAULT 0,
    total_energy    DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    total_resources INTEGER         NOT NULL DEFAULT 0,
    avg_agent_energy DOUBLE PRECISION NOT NULL DEFAULT 0.0,
    avg_fitness      DOUBLE PRECISION NOT NULL DEFAULT 0.0
);

CREATE INDEX IF NOT EXISTS idx_state_history_save_tick
    ON simulation_state_history (save_id, tick);

-- ============================================================
-- Auto-save configuration (single-row settings table)
-- ============================================================
CREATE TABLE IF NOT EXISTS auto_save_config (
    id                  SERIAL  PRIMARY KEY,
    interval_ticks      INTEGER NOT NULL DEFAULT 100,   -- ticks between auto-saves
    max_auto_saves      INTEGER NOT NULL DEFAULT 5,     -- oldest auto-saves rotate out
    enabled             BOOLEAN NOT NULL DEFAULT TRUE,
    slot_prefix         VARCHAR(32) NOT NULL DEFAULT 'autosave',
    last_auto_save_tick BIGINT  NOT NULL DEFAULT 0,
    updated_at          TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

-- Seed default config if empty
INSERT INTO auto_save_config (interval_ticks, max_auto_saves, enabled, slot_prefix, last_auto_save_tick)
SELECT 100, 5, TRUE, 'autosave', 0
WHERE NOT EXISTS (SELECT 1 FROM auto_save_config);
