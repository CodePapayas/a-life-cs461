#pragma once

#include <memory>
#include <vector>
#include "Environment.h"
#include "../decision_center/entity.hpp"
#include "../perception_movement/perception.hpp"

// Forward declarations
class Brain;
class Biology;

/**
 * @class Simulation
 * @brief Main simulation controller that initializes and manages the environment, entities, and brains
 */
class Simulation
{
private:
    std::unique_ptr<Environment> _environment;
    std::vector<std::unique_ptr<Entity>> _entities;
    std::unique_ptr<Perception> _perception;

public:
    /**
     * @brief Constructor that initializes the simulation with default settings
     */
    Simulation();

    /**
     * @brief Destructor for the simulation
     */
    ~Simulation() = default;

    /**
     * @brief Initializes the simulation with environment, entity, and brain. Currently all randos
     */
    void initialize();

    /**
     * @brief Returns the value of the tile located at (x,y)
     * @return the float value.
     */
    float environGetTileValue(int x, int y) const;

    /**
     * @brief Returns the perception object for this simulation
     * @return Pointer to the perception object
     */
    std::vector<double> get_perception() const;

    /**
     * @brief Sends perception plus internal state to the entity brain and returns a decision code
     * @return The decision code, or -1 on error
     */
    int pass_perception_to_brain();

    /**
     * @brief Returns the first entity (primary entity). Initial sims will only have 1, but I want to have this in place for when we expand.
     * @return Pointer to the first entity
     */
    Entity* get_primary_entity() const;

    /**
     * @brief Returns number of entities in the simulation. Surpisingly helpful in diagnosing bugs.
     * @return The count of entities
     */
    size_t get_entity_count() const;

    /**
     * @brief Tests whether the sim can actually access its members
     * Does not currently test for accuracy, just that communication between modules is possible
     * Also outputs to CLI all info accessed for visual confirmation
     */
    void testAccess();

    /**
     * @brief Sets the coordinates of the primary entity. Should be renamed. Doesn't access bio 
     * @param coords The new coordinates to set
     */
    void biologySetCoordinates(Vector2d coords);

    /**
     * @brief Gets the coordinates of the primary entity. Should be renamed. Doesn't access bio
     * @return The current coordinates of the primary entity
     */
    Vector2d biologyGetCoordinates() const;

    /**
     * @brief Displays the environment to the CLI with color-coded tiles
     * Tiles are colored blue (low values) to red (high values)
     * Entities are displayed as white "X"s
     */
    void display_environment() const;

    float get_vision_value() const;
};
