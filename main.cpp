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
    std::vector<double> perception = sim.get_perception();
    std::cout << "Perception values: ";
    for (double val : perception) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << "Number of tiles in full view: " << perception.size() << std::endl;
    
    for(int i = 0; i < 10; i++)
    {
        int decision = sim.pass_perception_to_brain();
        std::cout << "Brain Chose decided: " << decision << std::endl;
    }
    
    // Pull in resources
    // Handle getting the info from perception, passing it to the brain, and then executing the movement
    // Handle the entity consuming at a resource node.
    return 0;
}
