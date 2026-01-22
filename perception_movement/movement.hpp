#pragma once

#include <vector>

/**
 * Movement module - converts decisions to actions
 * TODO: Implement movement execution and action selection
 */

class Movement {
public:
    enum Direction {
        NORTH = 0,
        SOUTH = 1,
        WEST = 2,
        EAST = 3,
        STAY = 4
    };

    struct Action {
        Direction direction;
        int dx;
        int dy;
        double energy_cost;
    };

    // TODO: Implement decision to action conversion
    static Action decide_movement(const std::vector<double>& logits);

    // TODO: Implement movement execution with boundary checking
    static bool execute_movement(
        int& current_x,
        int& current_y,
        const Action& action,
        int env_width,
        int env_height,
        double& energy
    );
};
