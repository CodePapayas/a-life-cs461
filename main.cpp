#include <iostream>
#include "source/Simulation.hpp"

int main() {
    // Create and initialize the simulation
    Simulation sim;
    sim.initialize();
    std::cout << "\nSimulation setup complete with " << sim.get_entity_count() << " entity!" << std::endl;

    // Run the simulation for a certain number of ticks
    const int numTicks = 10;
    for (int i = 0; i < numTicks; ++i) {
        std::cout << "\n=== Tick " << (i + 1) << " ===" << std::endl;
        int result = sim.tick();
        if (result == -1) {
            break; // End the simulation if the primary entity has died
        }
    }

    // Handle getting the resources passed to the brain
    // Handle the request for consumption of a resource node and passing that to the biology
    // Handle the entity consuming at a resource node.
    return 0;
}
