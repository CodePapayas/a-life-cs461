/*
  Author: Dillon Stickler
  Oregon State University
  CS 463

  test_environment_functions.cpp
  Input checking on all functions pertaining to the environment.
*/

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <cmath>
#include "../source/environment/Environment.h"
#include "../source/environment/PerlinNoise.hpp"
#include "../source/environment/MathVector.hpp"

// Helper to compare doubles with a small epsilon
bool isClose(double a, double b, double epsilon = 0.001) {
    return std::abs(a - b) < epsilon;
}

void test_constructor_and_dimensions() {
    std::cout << "Running: test_constructor_and_dimensions..." << std::endl;
    int x = 10;
    int y = 20;
    Environment env(x, y);

    assert(env.getTileAmountX() == x);
    assert(env.getTileAmountY() == y);
    assert(env.getTileArea() == x * y);
    std::cout << "  Passed!" << std::endl;
}

void test_bound_coords() {
    std::cout << "Running: test_bound_coords..." << std::endl;
    Environment env(10, 10);

    // Happy Path: Inside bounds
    Vector2d inside = env.boundCoords(Vector2d(5, 5));
    assert(inside.x == 5 && inside.y == 5);

    // Edge Case: Zero (Lower Boundary)
    Vector2d zero = env.boundCoords(Vector2d(0, 0));
    assert(zero.x == 0 && zero.y == 0);

    // Edge Case: Negative values (should clamp to 0)
    Vector2d negative = env.boundCoords(Vector2d(-5, -10));
    assert(negative.x == 0 && negative.y == 0);

    // Edge Case: Exact Upper Boundary (should clamp to size - 1)
    Vector2d upper = env.boundCoords(Vector2d(10, 10));
    assert(upper.x == 9 && upper.y == 9);

    // Edge Case: Far out of bounds (should clamp)
    Vector2d far_out = env.boundCoords(Vector2d(100, -100));
    assert(far_out.x == 9 && far_out.y == 0);
    std::cout << "  Passed!" << std::endl;
}

void test_get_set_tile_values() {
    std::cout << "Running: test_get_set_tile_values..." << std::endl;
    Environment env(5, 5);
    Vector2d pos(2, 2);
    std::vector<double> new_values = {0.1, 0.2, 0.3};

    // Happy Path: Set and Get vector
    env.setTileValues(pos, new_values);
    std::vector<double> retrieved = env.getTileValues(pos);
    assert(retrieved.size() == 3);
    assert(isClose(retrieved[0], 0.1));
    assert(isClose(retrieved[1], 0.2));
    assert(isClose(retrieved[2], 0.3));

    // Happy Path: Set and Get single index
    env.setTileValue(pos, 9.9, 1);
    assert(isClose(env.getTileValue(pos, 1), 9.9));

    // Edge Case: Accessing out of bounds (should clamp via getTile)
    // Setting value at (6,6) should actually modify (4,4)
    env.setTileValue(Vector2d(6, 6), 5.5, 0);
    assert(isClose(env.getTileValue(Vector2d(4, 4), 0), 5.5));
    std::cout << "  Passed!" << std::endl;
}

void test_tile_metadata() {
    std::cout << "Running: test_tile_metadata..." << std::endl;
    Environment env(5, 5);
    Vector2d pos(1, 1);

    // Check if terrain type exists and is not empty
    std::string type = env.getTileType(pos);
    assert(!type.empty());
    assert(type.find("Terrain Efficiency") != std::string::npos);

    // Check temperature calculation (should be a valid number)
    double temp = env.calculateTemperature(pos);
    assert(!std::isnan(temp));
    std::cout << "  Passed!" << std::endl;
}

void test_get_tile_from_id() {
    std::cout << "Running: test_get_tile_from_id..." << std::endl;
    Environment env(2, 2); // Creates 4 tiles: IDs 0, 1, 2, 3

    // Happy Path: Valid ID
    Vector2d coord = env.getTileFromID(2);
    // tile_map inserts in order: (0,0)=0, (0,1)=1, (1,0)=2, (1,1)=3
    assert(coord.x == 1 && coord.y == 0);

    // Edge Case: Invalid ID
    Vector2d invalid = env.getTileFromID(99);
    assert(invalid.x == -1 && invalid.y == -1);
    std::cout << "  Passed!" << std::endl;
}

void test_update_tiles() {
    std::cout << "Running: test_update_tiles..." << std::endl;
    Environment env(3, 3);
    Vector2d pos(1, 1);

    double initial_temp = env.getTile(pos)->getTemperature();

    // Update the environment
    env.updateTiles();

    double updated_temp = env.getTile(pos)->getTemperature();

    // Since temperature_movement increments, the noise sampling location shifts,
    // so the temperature at the same coordinate should likely change.
    // This tests that the loop actually executes and modifies state.
    assert(initial_temp != updated_temp);
    std::cout << "  Passed!" << std::endl;
}

int main() {
    std::cout << "Starting Environment Test Suite..." << std::endl;
    std::cout << "----------------------------------" << std::endl;

    try {
        std::cout << "Test 1:" << std::endl;
        test_constructor_and_dimensions();
        std::cout << "Test 2:" << std::endl;
        test_bound_coords();
        std::cout << "Test 3:" << std::endl;
        test_get_set_tile_values();
        std::cout << "Test 4:" << std::endl;
        test_tile_metadata();
        std::cout << "Test 5:" << std::endl;
        test_get_tile_from_id();
        std::cout << "Test 6:" << std::endl;
        test_update_tiles();

        std::cout << "----------------------------------" << std::endl;
        std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "TEST FAILED with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "TEST FAILED with unknown error." << std::endl;
        return 1;
    }

    return 0;
}