/*
  test_fuzzy_vision.cpp
  Unit tests for Simulation::fuzzy_vision and Simulation::get_distance_from_index
*/

#include "../source/simulation/Simulation.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

using namespace std;

static int totalTests = 0, passedTests = 0;

#define TEST(name) totalTests++; cout << "[TEST] " << (name) << "... "; bool ok = true;
#define CHECK(cond) if (!(cond)) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #cond; }
#define CHECK_APPROX(a, b, eps) if (std::abs((a) - (b)) >= (eps)) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #a " ~= " #b " (got " << (a) << ", expected " << (b) << ")"; }
#define END_TEST() if (ok) { cout << "PASS"; passedTests++; } else { cout << " <-- FAILED"; } cout << "\n";

// Simulation can be constructed without initialize() since fuzzy_vision
// and get_distance_from_index are pure functions with no instance state.
static Simulation sim;

// -------------------------------------------------------
// get_distance_from_index
// -------------------------------------------------------

void testDistanceCenter() {
    TEST("get_distance_from_index - center tile (index 12) has distance 0")
    // 5x5 grid, center = index 12 (x=2,y=2)
    double d = sim.get_distance_from_index(12);
    CHECK_APPROX(d, 0.0, 1e-9)
    END_TEST()
}

void testDistanceCorner() {
    TEST("get_distance_from_index - top-left corner (index 0) has distance sqrt(8)")
    double d = sim.get_distance_from_index(0);
    CHECK_APPROX(d, std::sqrt(8.0), 1e-9)
    END_TEST()
}

void testDistanceEdgeMid() {
    TEST("get_distance_from_index - left edge midpoint (index 10) has distance 2")
    // index 10: x = 10%5 = 0, y = 10/5 = 2 -> dx=-2, dy=0 -> distance=2
    double d = sim.get_distance_from_index(10);
    CHECK_APPROX(d, 2.0, 1e-9)
    END_TEST()
}

void testDistanceAdjacentCenter() {
    TEST("get_distance_from_index - tile adjacent to center (index 13) has distance 1")
    // index 13: x=3, y=2 -> dx=1, dy=0 -> distance=1
    double d = sim.get_distance_from_index(13);
    CHECK_APPROX(d, 1.0, 1e-9)
    END_TEST()
}

// -------------------------------------------------------
// fuzzy_vision output size
// -------------------------------------------------------

void testOutputSizeMatchesInput() {
    TEST("fuzzy_vision - output size equals input size")
    vector<double> perception(25, 0.5);
    auto result = sim.fuzzy_vision(perception, 1.0f);
    CHECK(result.size() == perception.size())
    END_TEST()
}

void testEmptyInputReturnsEmpty() {
    TEST("fuzzy_vision - empty input returns empty output")
    vector<double> empty;
    auto result = sim.fuzzy_vision(empty, 1.0f);
    CHECK(result.empty())
    END_TEST()
}

// -------------------------------------------------------
// fuzzy_vision center tile passthrough
// -------------------------------------------------------

void testCenterTilePreservesRawValue() {
    TEST("fuzzy_vision - center tile (index 12) always returns raw perception value")
    // Distance = 0 -> clarity = exp(0) = 1.0 -> filtered = raw
    vector<double> perception(25, 0.0);
    perception[12] = 0.9;
    auto result = sim.fuzzy_vision(perception, 0.1f); // Low vision, but center is unaffected
    CHECK_APPROX(result[12], 0.9, 1e-9)
    END_TEST()
}

void testCenterTileAtNeutral() {
    TEST("fuzzy_vision - center tile at neutral (0.5) stays at 0.5")
    vector<double> perception(25, 0.5);
    auto result = sim.fuzzy_vision(perception, 1.0f);
    CHECK_APPROX(result[12], 0.5, 1e-9)
    END_TEST()
}

// -------------------------------------------------------
// fuzzy_vision neutral point collapses to neutral
// -------------------------------------------------------

void testNeutralInputUnchanged() {
    TEST("fuzzy_vision - all-neutral input (0.5) remains 0.5 at every tile")
    vector<double> perception(25, 0.5);
    auto result = sim.fuzzy_vision(perception, 0.01f); // Extremely low vision
    for (size_t i = 0; i < result.size(); ++i) {
        CHECK_APPROX(result[i], 0.5, 1e-9)
    }
    END_TEST()
}

// -------------------------------------------------------
// fuzzy_vision: higher vision preserves more signal
// -------------------------------------------------------

void testHighVisionPreservesSignalBetter() {
    TEST("fuzzy_vision - higher vision preserves more signal at far tiles")
    // Use a corner tile (index 0, farthest from center) with a strongly non-neutral value
    vector<double> perception(25, 0.5);
    perception[0] = 1.0;

    auto highVision  = sim.fuzzy_vision(perception, 1.0f);
    auto lowVision   = sim.fuzzy_vision(perception, 0.1f);

    // Both should be pulled toward 0.5, but high vision less so
    double highDeviation = std::abs(highVision[0]  - 0.5);
    double lowDeviation  = std::abs(lowVision[0]   - 0.5);

    CHECK(highDeviation > lowDeviation)
    END_TEST()
}

// -------------------------------------------------------
// fuzzy_vision: vision floor clamp
// -------------------------------------------------------

void testVisionFloorClamp() {
    TEST("fuzzy_vision - vision value <= 0 clamps to 0.05 minimum")
    // At vision=0.05 and vision=0.001 the result should be identical
    vector<double> perception(25, 0.8);
    auto resultAtFloor  = sim.fuzzy_vision(perception, 0.05f);
    auto resultBelowFloor = sim.fuzzy_vision(perception, 0.0f);
    for (size_t i = 0; i < resultAtFloor.size(); ++i) {
        CHECK_APPROX(resultAtFloor[i], resultBelowFloor[i], 1e-9)
    }
    END_TEST()
}

// -------------------------------------------------------
// fuzzy_vision: values stay within reasonable range
// -------------------------------------------------------

void testOutputBoundedByInput() {
    TEST("fuzzy_vision - output for extreme input (0.0/1.0) stays within [0, 1]")
    vector<double> perception(25);
    for (int i = 0; i < 25; ++i) perception[i] = (i % 2 == 0) ? 0.0 : 1.0;

    auto result = sim.fuzzy_vision(perception, 0.5f);
    for (size_t i = 0; i < result.size(); ++i) {
        CHECK(result[i] >= 0.0 && result[i] <= 1.0)
    }
    END_TEST()
}

// -------------------------------------------------------
// fuzzy_vision: specific value check for a known tile
// -------------------------------------------------------

void testKnownValueAtAdjacentTile() {
    TEST("fuzzy_vision - manually computed value at index 13 (distance 1)")
    // index 13: distance = 1.0
    // clarity = exp(-1.0 / (0.5 * 2.0)) = exp(-1.0) ~= 0.36788
    // filtered = 0.5 + (0.9 - 0.5) * 0.36788 = 0.5 + 0.14715 = 0.64715
    vector<double> perception(25, 0.5);
    perception[13] = 0.9;
    double expected = 0.5 + (0.9 - 0.5) * std::exp(-1.0 / (0.5 * 2.0));
    auto result = sim.fuzzy_vision(perception, 0.5f);
    CHECK_APPROX(result[13], expected, 1e-6)
    END_TEST()
}

// -------------------------------------------------------

int main() {
    cout << "\n=== fuzzy_vision Tests ===\n" << endl;

    testDistanceCenter();
    testDistanceCorner();
    testDistanceEdgeMid();
    testDistanceAdjacentCenter();

    testOutputSizeMatchesInput();
    testEmptyInputReturnsEmpty();

    testCenterTilePreservesRawValue();
    testCenterTileAtNeutral();

    testNeutralInputUnchanged();
    testHighVisionPreservesSignalBetter();
    testVisionFloorClamp();
    testOutputBoundedByInput();
    testKnownValueAtAdjacentTile();

    cout << "\n" << passedTests << "/" << totalTests << " tests passed." << endl;
    return (passedTests == totalTests) ? 0 : 1;
}
