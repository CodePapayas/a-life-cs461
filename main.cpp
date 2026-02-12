#include <iostream>
#include "source/Simulation.hpp"

int main() {
    // Create and initialize the simulation
    Simulation sim;
    sim.initialize();
    std::cout << "\nSimulation setup complete with " << sim.get_entity_count() << " entity!" << std::endl;
    sim.testAccess();
    // Display the environment
    sim.display_environment();
    // To do: Pull in perception and movement
    std::vector<float> perception = sim.get_perception();
    std::cout << "Perception values: ";
    for (float val : perception) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    // Pull in resources
    // Handle getting the info from perception, passing it to the brain, and then executing the movement
    // Handle the entity consuming at a resource node.
    return 0;
}
