#include <iostream>
#include "source/Simulation.hpp"

int main() {
    // Create and initialize the simulation
    Simulation sim;
    sim.initialize();
    std::cout << "\nSimulation setup complete with " << sim.get_entity_count() << " entity!" << std::endl;

    // Verify that the simulation can access the environment and entity data correctly
    sim.testAccess();

    // Display the environment
    sim.display_environment();
    
    // Test perception retrieval
    std::vector<double> perception = sim.get_perception();
    std::cout << "Perception values: ";
    for (double val : perception) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    for (int i = 0; i < 5; i++) {
        sim.execute_movement(i); // Placeholder for movement execution test
        sim.display_environment(); // Display environment after movement Entity should have moved if movement execution is implemented
    }
    /*
    for(int i = 0; i < 10; i++)
    {
        int decision = sim.pass_perception_to_brain();
        int filteredDecision = sim.pass_perception_to_brain();
        std::cout << "Brain Chose decided: " << decision << std::endl; // Deprecated, maybe add in a decider for the unfiltered perception for testing purposes?
        std::cout << "Brain decided with filtered perception: " << filteredDecision << std::endl;
    }
    */
    // Pull in resources
    // Handle getting the info from perception, passing it to the brain, and then executing the movement
    // Handle the entity consuming at a resource node.
    return 0;
}
