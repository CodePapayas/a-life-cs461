#pragma once

#include <vector>

/**
 * Perception module - handles sensory input from environment
 * TODO: Expand feature extraction based on simulation requirements
 */

class Perception {
public:
    struct SensoryInput {
        std::vector<double> features;
        int entity_x;
        int entity_y;
    };

    // TODO: Implement perception based on difficulty map
    static SensoryInput perceive_local_environment(
        int entity_x,
        int entity_y,
        const std::vector<std::vector<double>>& difficulty_map
    );

private:
    // TODO: Add helper functions for feature extraction
};
