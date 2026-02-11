#include "Simulation.hpp"
#include "Environment.h"
#include "../decision_center/entity.hpp"
#include "../decision_center/brain.hpp"
#include "../decision_center/biology.hpp"
#include <iostream>

Simulation::Simulation()
    : _environment(nullptr)
{
}

void Simulation::initialize()
{
    // Create a new environment
    _environment = std::make_unique<Environment>();
    std::cout << "Environment created successfully!" << std::endl;

    // Create an entity
    auto entity = std::make_unique<Entity>();
    std::cout << "Entity created successfully with ID: " << entity->get_id() << std::endl;
    entity->set_coordinates(Vector2d(5, 5)); // Set initial coordinates for the entity
    // Create a brain with a neural network architecture
    // Architecture: 4 inputs -> 8 hidden -> 8 hidden -> 6 outputs
    std::vector<int> layer_sizes = {4, 8, 8, 6};
    auto brain = std::make_shared<Brain>(layer_sizes);
    std::cout << "Brain created successfully with " << brain->get_layer_count() << " layers!" << std::endl;

    // Create biology
    auto biology = std::make_shared<Biology>(false);  // false for randomized genetics
    std::cout << "Biology created successfully!" << std::endl;

    // Set the brain and biology on the entity
    entity->set_brain(brain);
    entity->set_biology(biology);
    std::cout << "Entity configured with brain and biology!" << std::endl;

    // Add entity to the simulation
    _entities.push_back(std::move(entity));
}

float Simulation::environGetTileValue(int x, int y) const
{
    return _environment->getTileValue(Vector2d(x, y));
}

Entity* Simulation::get_primary_entity() const
{
    if (_entities.empty())
    {
        return nullptr;
    }
    return _entities[0].get();
}

size_t Simulation::get_entity_count() const
{
    return _entities.size();
}

void Simulation::biologySetCoordinates(Vector2d coords)
{
    auto entity = get_primary_entity();
    if (entity)
    {
        entity->set_coordinates(coords);
        std::cout << "Entity location set to (" << coords.x << ", " << coords.y << ")" << std::endl;
    }
    else
    {
        std::cerr << "No primary entity found to set coordinates!" << std::endl;
    }
}

Vector2d Simulation::biologyGetCoordinates() const
{
    auto entity = get_primary_entity();
    if (entity)
    {
        return entity->get_coordinates();
    }
    else
    {
        return Vector2d(0, 0); // Return default coordinates if no entity is found
    }
}

void Simulation::display_environment() const
{
    if (!_environment)
    {
        std::cerr << "Environment not initialized!" << std::endl;
        return;
    }

    int grid_size = chunk_amt * tile_amt;
    Vector2d entity_pos = get_primary_entity() ? get_primary_entity()->get_coordinates() : Vector2d(-1, -1);

    std::cout << "\n=== Environment Display ===\n" << std::endl;
    // Normal 2d traversal of the environment grid
    for (int y = 0; y < grid_size; ++y)
    {
        for (int x = 0; x < grid_size; ++x)
        {
            float tile_value = _environment->getTileValue(Vector2d(x, y));
            // Check if an entity is at this location, probably a better way down the line
            if (entity_pos.x == x && entity_pos.y == y)
            {
                // Display entity as white x
                std::cout << "\033[97m"  // White color
                          << "X "
                          << "\033[0m"; // Reset color
            }
            else
            {
                // Some hacky color coding adopted from another project.
                int r, g, b;
                if (tile_value < 0.5)
                {
                    // Blue to Cyan (0.0 to 0.5)
                    float normalized = tile_value * 2; // 0.0 to 1.0
                    b = 255;
                    g = (int)(normalized * 255);
                    r = 0;
                }
                else
                {
                    // Cyan to Red (0.5 to 1.0 Gets kinda dicey around.5, but it is what it is)
                    float normalized = (tile_value - 0.5) * 2; // 0.0 to 1.0
                    r = (int)(normalized * 255);
                    g = (int)((1 - normalized) * 255);
                    b = (int)((1 - normalized) * 255);
                }
                std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m"
                          << "O "
                          << "\033[0m"; 
            }
        }
        std::cout << std::endl;
    }
    std::cout << "\n=== End Environment Display ===\n" << std::endl;
}

void Simulation::testAccess()
{
    std::cout << "Testing access to environment and entity..." << std::endl;
    float val = environGetTileValue(0, 0);
    if (val)
    {
        std::cout << "Tile at 0,0 has value: " << val << std::endl;
    }
    else
    {
        std::cerr << "Access failed to the environment!" << std::endl;
    }
    auto entity = get_primary_entity();
    if (entity)
    {
        std::cout << "Entity accessed successfully with ID: " << entity->get_id() << std::endl;
        auto metrics = entity->biology_get_metrics();
        for (const auto& pair : metrics){
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
        auto genetics = entity->biology_get_genetics();
        for (const auto& pair : genetics){
            std::cout << pair.first << ": " << pair.second << std::endl;   
        }
        auto brain = entity->brain();
        std::vector<double> inputs = {0.5, 0.2, 0.1, 0.9};
        auto decision = entity->brain_get_decision(inputs);
        std::cout << "Brain decision made successfully: " << std::any_cast<int>(decision) << std::endl;
    }
    else
    {
        std::cerr << "Couldn't find a boy, did you initialize the simulation properly?" << std::endl;
    }
}