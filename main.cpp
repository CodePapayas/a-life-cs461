#include <iostream>
#include "source/simulation/Simulation.hpp"

int main() {
    // Create and initialize the simulation
    Simulation sim;
    sim.initialize();
    std::cout << "\nSimulation setup complete with " << sim.get_entity_count() << " entity!" << std::endl;

    // Run the simulation for a certain number of ticks
    const int numTicks = 1000;
    int top_sim = -1;
    int most_ticks = -1;
    int sims = 25;
    for (int s = 0; s < sims; s++){
        Simulation sim;
        sim.initialize();
        int i = 0;
        while(true){
            system("cls");
            std::cout << "\n=== Sim " << (s + 1) << " = Tick " << (i + 1) << " ===" << std::endl;
            int result = sim.tick();
            i++;
            if (result == -1) {
                goto exit; // End the simulation if the primary entity has died
            }
        }
        exit:
        if (i > most_ticks) {most_ticks = i; top_sim = s + 1;}
    }

    std::cout << "longest time alive: [Sim #"  << top_sim << "] " << most_ticks << " ticks" << std::endl;

    // Handle getting the resources passed to the brain
    // Handle the request for consumption of a resource node and passing that to the biology
    // Handle the entity consuming at a resource node.
    return 0;
}
