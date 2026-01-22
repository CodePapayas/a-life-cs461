#include "perception.hpp"

Perception::SensoryInput Perception::perceive_local_environment(
    int entity_x,
    int entity_y,
    const std::vector<std::vector<double>>& difficulty_map) {

    SensoryInput sensory;
    sensory.entity_x = entity_x;
    sensory.entity_y = entity_y;

    // TODO: Implement actual perception logic
    // Placeholder: return simple features
    sensory.features = {
        0.5,  // Local difficulty placeholder
        0.5,  // Mean neighbor difficulty placeholder
        0.0,  // Slope difficulty placeholder
        0.0,  // Roughness placeholder
        1.0   // Bias term
    };

    return sensory;
}
