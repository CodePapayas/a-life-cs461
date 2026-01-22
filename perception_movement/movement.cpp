#include "movement.hpp"

Movement::Action Movement::decide_movement(const std::vector<double>& logits) {
    // TODO: Implement softmax and argmax logic
    // Placeholder: always stay
    return {STAY, 0, 0, 0.0};
}

bool Movement::execute_movement(
    int& current_x,
    int& current_y,
    const Action& action,
    int env_width,
    int env_height,
    double& energy) {

    // TODO: Implement boundary checking and energy management
    // Placeholder implementation
    energy -= action.energy_cost;
    current_x += action.dx;
    current_y += action.dy;
    
    return true;
}
