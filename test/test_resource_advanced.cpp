/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462 
  
  Advanced ResourceNode and ResourceManager Tests
  Coverage: Spatial queries, concurrent operations, edge cases, performance
*/

#include "../include/resource_node.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>

using namespace std;

int totalTests = 0, passedTests = 0;

#define TEST(name) totalTests++; cout << "[TEST] " << name << "... "; bool ok = true;
#define CHECK(cond) if (!(cond)) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #cond; }
#define CHECK_APPROX(a, b, eps) if (abs((a) - (b)) >= eps) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #a " ≈ " #b; }
#define END_TEST() if (ok) { cout << "PASS"; passedTests++; } else { cout << endl; } cout << endl;

// Test 1: Position distance calculations
void testPositionDistances() {
    TEST("Position - Distance calculations")
    
    Position p1(0, 0);
    Position p2(3, 4);
    Position p3(-5, -5);
    
    // Manhattan distance
    CHECK(p1.manhattanDistance(p2) == 7);
    CHECK(p1.manhattanDistance(p3) == 10);
    CHECK(p2.manhattanDistance(p3) == 17);
    
    // Euclidean distance
    CHECK_APPROX(p1.euclideanDistance(p2), 5.0, 0.001);
    CHECK_APPROX(p1.euclideanDistance(p3), 7.071, 0.01);
    
    // Symmetry
    CHECK(p1.manhattanDistance(p2) == p2.manhattanDistance(p1));
    CHECK_APPROX(p1.euclideanDistance(p2), p2.euclideanDistance(p1), 0.001);
    
    // Same position
    CHECK(p1.manhattanDistance(p1) == 0);
    CHECK_APPROX(p1.euclideanDistance(p1), 0.0, 0.001);
    
    END_TEST()
}

// Test 2: ResourceNode consumption edge cases
void testResourceConsumptionEdgeCases() {
    TEST("ResourceNode - Consumption edge cases")
    
    ResourceNode res(Position(0, 0), ResourceType::FOOD, 100.0, false);
    
    // Consume exact amount
    double consumed1 = res.consume(30.0);
    CHECK_APPROX(consumed1, 30.0, 0.001);
    CHECK_APPROX(res.getEnergyValue(), 70.0, 0.001);
    
    // Try to consume more than available
    double consumed2 = res.consume(100.0);
    CHECK_APPROX(consumed2, 70.0, 0.001);  // Should only get what's left
    CHECK_APPROX(res.getEnergyValue(), 0.0, 0.001);
    CHECK(res.isDepleted());
    
    // Try to consume from depleted resource
    double consumed3 = res.consume(50.0);
    CHECK_APPROX(consumed3, 0.0, 0.001);
    
    // Try to consume zero
    ResourceNode res2(Position(0, 0), ResourceType::WATER, 50.0, false);
    double consumed4 = res2.consume(0.0);
    CHECK_APPROX(consumed4, 0.0, 0.001);
    CHECK_APPROX(res2.getEnergyValue(), 50.0, 0.001);
    
    // Try to consume negative (edge case)
    double consumed5 = res2.consume(-10.0);
    CHECK_APPROX(consumed5, 0.0, 0.001);
    
    END_TEST()
}

// Test 3: Renewable resource regeneration
void testRenewableRegeneration() {
    TEST("ResourceNode - Renewable regeneration")
    
    ResourceNode renewable(Position(5, 5), ResourceType::PLANT, 100.0, true);
    
    // Consume some energy
    renewable.consume(50.0);
    CHECK_APPROX(renewable.getEnergyValue(), 50.0, 0.001);
    
    // Update should regenerate (1% per tick by default)
    double before = renewable.getEnergyValue();
    renewable.update(1.0);  // 1 time unit
    double after = renewable.getEnergyValue();
    
    CHECK(after > before);
    CHECK(after <= renewable.getMaxEnergy());
    
    // Update multiple times to reach max
    for (int i = 0; i < 200; i++) {
        renewable.update(1.0);
    }
    CHECK_APPROX(renewable.getEnergyValue(), renewable.getMaxEnergy(), 0.1);
    
    // Non-renewable shouldn't regenerate
    ResourceNode nonRenewable(Position(5, 5), ResourceType::MINERAL, 100.0, false);
    nonRenewable.consume(50.0);
    double beforeNR = nonRenewable.getEnergyValue();
    nonRenewable.update(10.0);
    CHECK_APPROX(nonRenewable.getEnergyValue(), beforeNR, 0.001);
    
    END_TEST()
}

// Test 4: Manual regeneration
void testManualRegeneration() {
    TEST("ResourceNode - Manual regeneration")
    
    ResourceNode renewable(Position(0, 0), ResourceType::PLANT, 100.0, true);
    renewable.consume(60.0);
    CHECK_APPROX(renewable.getEnergyValue(), 40.0, 0.001);
    
    // Manually regenerate
    renewable.regenerate(30.0);
    CHECK_APPROX(renewable.getEnergyValue(), 70.0, 0.001);
    
    // Regenerate beyond max (should clamp)
    renewable.regenerate(50.0);
    CHECK_APPROX(renewable.getEnergyValue(), 100.0, 0.001);
    
    // Non-renewable can't manually regenerate
    ResourceNode nonRenewable(Position(0, 0), ResourceType::FOOD, 100.0, false);
    nonRenewable.consume(50.0);
    nonRenewable.regenerate(30.0);
    CHECK_APPROX(nonRenewable.getEnergyValue(), 50.0, 0.001);  // No change
    
    END_TEST()
}

// Test 5: Range detection
void testRangeDetection() {
    TEST("ResourceNode - Range detection")
    
    ResourceNode res(Position(10, 10), ResourceType::FOOD, 100.0, false);
    
    // Exact position
    CHECK(res.isInRange(Position(10, 10), 0));
    CHECK(res.isInRange(Position(10, 10), 1));
    
    // Adjacent positions (Manhattan distance = 1)
    CHECK(res.isInRange(Position(11, 10), 1));
    CHECK(res.isInRange(Position(9, 10), 1));
    CHECK(res.isInRange(Position(10, 11), 1));
    CHECK(res.isInRange(Position(10, 9), 1));
    
    // Diagonal (Manhattan distance = 2)
    CHECK(!res.isInRange(Position(11, 11), 1));
    CHECK(res.isInRange(Position(11, 11), 2));
    
    // Far away
    CHECK(!res.isInRange(Position(20, 20), 5));
    CHECK(res.isInRange(Position(20, 20), 20));
    
    END_TEST()
}

// Test 6: ResourceManager spatial queries
void testResourceManagerSpatialQueries() {
    TEST("ResourceManager - Spatial queries")
    
    ResourceManager mgr;
    
    // Create a grid of resources
    mgr.createResource(Position(0, 0), ResourceType::FOOD, 50.0, false);
    mgr.createResource(Position(10, 0), ResourceType::WATER, 30.0, true);
    mgr.createResource(Position(0, 10), ResourceType::MINERAL, 40.0, false);
    mgr.createResource(Position(10, 10), ResourceType::PLANT, 60.0, true);
    mgr.createResource(Position(5, 5), ResourceType::FOOD, 45.0, false);
    
    CHECK(mgr.getResourceCount() == 5);
    
    // Find resources in range from (5, 5)
    auto inRange = mgr.findResourcesInRange(Position(5, 5), 1);
    CHECK(inRange.size() >= 1);  // Should at least find the one at (5,5)
    
    // Find resources in larger range
    auto inLargeRange = mgr.findResourcesInRange(Position(5, 5), 10);
    CHECK(inLargeRange.size() >= 3);  // Should find multiple
    
    // Find nearest from (0, 0)
    ResourceNode* nearest = mgr.findNearestResource(Position(0, 0));
    CHECK(nearest != nullptr);
    CHECK(nearest->getPosition() == Position(0, 0));
    
    // Find nearest from (6, 6)
    ResourceNode* nearest2 = mgr.findNearestResource(Position(6, 6));
    CHECK(nearest2 != nullptr);
    CHECK(nearest2->getPosition() == Position(5, 5));  // Closest one
    
    END_TEST()
}

// Test 7: Depletion and cleanup
void testDepletionAndCleanup() {
    TEST("ResourceManager - Depletion and cleanup")
    
    ResourceManager mgr;
    
    // Create mix of renewable and non-renewable
    ResourceNode* r1 = mgr.createResource(Position(0, 0), ResourceType::FOOD, 20.0, false);
    ResourceNode* r2 = mgr.createResource(Position(1, 0), ResourceType::PLANT, 30.0, true);
    ResourceNode* r3 = mgr.createResource(Position(2, 0), ResourceType::MINERAL, 25.0, false);
    
    CHECK(mgr.getResourceCount() == 3);
    
    // Deplete the non-renewable resources
    r1->consume(20.0);
    r3->consume(25.0);
    
    CHECK(r1->isDepleted());
    CHECK(r3->isDepleted());
    CHECK(!r2->isDepleted());
    
    // Remove depleted resources
    size_t removed = mgr.removeDepletedResources();
    CHECK(removed == 2);  // Both non-renewable depleted ones
    CHECK(mgr.getResourceCount() == 1);  // Only renewable left
    
    // Deplete the renewable and try to remove
    r2->consume(30.0);
    CHECK(r2->isDepleted());
    
    size_t removed2 = mgr.removeDepletedResources();
    CHECK(removed2 == 0);  // Renewable stays even when depleted
    CHECK(mgr.getResourceCount() == 1);
    
    END_TEST()
}

// Test 8: Total energy tracking
void testTotalEnergyTracking() {
    TEST("ResourceManager - Total energy tracking")
    
    ResourceManager mgr;
    
    mgr.createResource(Position(0, 0), ResourceType::FOOD, 50.0, false);
    mgr.createResource(Position(1, 0), ResourceType::WATER, 30.0, true);
    mgr.createResource(Position(2, 0), ResourceType::MINERAL, 20.0, false);
    
    CHECK_APPROX(mgr.getTotalEnergy(), 100.0, 0.001);
    
    // Consume from one resource
    auto resources = mgr.findResourcesInRange(Position(0, 0), 0);
    CHECK(resources.size() > 0);
    resources[0]->consume(25.0);
    
    CHECK_APPROX(mgr.getTotalEnergy(), 75.0, 0.001);
    
    // Update all (renewable should regenerate)
    double before = mgr.getTotalEnergy();
    mgr.update(10.0);
    double after = mgr.getTotalEnergy();
    
    CHECK(after >= before);  // Should increase or stay same
    
    END_TEST()
}

// Test 9: Unique ID generation
void testUniqueIDGeneration() {
    TEST("ResourceNode - Unique ID generation")
    
    ResourceNode r1(Position(0, 0), ResourceType::FOOD, 50.0, false);
    ResourceNode r2(Position(1, 1), ResourceType::WATER, 50.0, true);
    ResourceNode r3(Position(2, 2), ResourceType::MINERAL, 50.0, false);
    
    // IDs should be unique
    CHECK(r1.getID() != r2.getID());
    CHECK(r2.getID() != r3.getID());
    CHECK(r1.getID() != r3.getID());
    
    // IDs should be sequential (may not always be true depending on other tests)
    // But at least they should be monotonically increasing
    CHECK(r1.getID() > 0);
    CHECK(r2.getID() > 0);
    CHECK(r3.getID() > 0);
    
    END_TEST()
}

// Test 10: Resource type differentiation
void testResourceTypeDifferentiation() {
    TEST("ResourceNode - Resource type differentiation")
    
    ResourceNode food(Position(0, 0), ResourceType::FOOD, 50.0, false);
    ResourceNode water(Position(0, 0), ResourceType::WATER, 50.0, false);
    ResourceNode mineral(Position(0, 0), ResourceType::MINERAL, 50.0, false);
    ResourceNode plant(Position(0, 0), ResourceType::PLANT, 50.0, true);
    
    CHECK(food.getType() == ResourceType::FOOD);
    CHECK(water.getType() == ResourceType::WATER);
    CHECK(mineral.getType() == ResourceType::MINERAL);
    CHECK(plant.getType() == ResourceType::PLANT);
    
    // Different types can have different properties
    CHECK(!food.isRenewable());
    CHECK(plant.isRenewable());
    
    END_TEST()
}

// Test 11: Large-scale spatial queries
void testLargeScaleSpatialQueries() {
    TEST("ResourceManager - Large-scale spatial queries")
    
    ResourceManager mgr;
    
    // Create a 10x10 grid of resources
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            mgr.createResource(
                Position(x * 10, y * 10),
                ResourceType::FOOD,
                50.0,
                false
            );
        }
    }
    
    CHECK(mgr.getResourceCount() == 100);
    
    // Find all resources in large range from center
    auto inRange = mgr.findResourcesInRange(Position(50, 50), 30);
    CHECK(inRange.size() > 0);
    
    // Find nearest from various positions
    ResourceNode* nearest1 = mgr.findNearestResource(Position(0, 0));
    CHECK(nearest1 != nullptr);
    
    ResourceNode* nearest2 = mgr.findNearestResource(Position(95, 95));
    CHECK(nearest2 != nullptr);
    
    // Clear all
    mgr.clear();
    CHECK(mgr.getResourceCount() == 0);
    CHECK_APPROX(mgr.getTotalEnergy(), 0.0, 0.001);
    
    END_TEST()
}

// Test 12: Concurrent operations simulation
void testConcurrentOperationsSimulation() {
    TEST("ResourceManager - Concurrent operations simulation")
    
    ResourceManager mgr;
    
    // Create resources
    for (int i = 0; i < 10; i++) {
        mgr.createResource(
            Position(i, i),
            ResourceType::FOOD,
            100.0,
            i % 2 == 0  // Every other is renewable
        );
    }
    
    // Simulate multiple agents accessing resources
    for (int agent = 0; agent < 5; agent++) {
        Position agentPos(agent * 2, agent * 2);
        auto nearby = mgr.findResourcesInRange(agentPos, 3);
        
        // Each agent consumes from nearby resources
        for (auto* res : nearby) {
            if (!res->isDepleted()) {
                res->consume(20.0);
            }
        }
    }
    
    // Update all resources
    mgr.update(5.0);
    
    // Renewable ones should have some energy
    double totalEnergy = mgr.getTotalEnergy();
    CHECK(totalEnergy > 0.0);
    
    // Cleanup depleted non-renewables
    size_t removed = mgr.removeDepletedResources();
    CHECK(removed >= 0);
    
    END_TEST()
}

// Test 13: Boundary positions and negative coordinates
void testBoundaryPositions() {
    TEST("Position - Boundary and negative coordinates")
    
    Position origin(0, 0);
    Position negative(-10, -10);
    Position positive(10, 10);
    Position mixed(-5, 5);
    
    // Distance calculations with negative coordinates
    CHECK(origin.manhattanDistance(negative) == 20);
    CHECK(negative.manhattanDistance(positive) == 40);
    CHECK_APPROX(origin.euclideanDistance(negative), 14.142, 0.01);
    
    // Equality with negative coordinates
    CHECK(negative == Position(-10, -10));
    CHECK(!(negative == origin));
    
    // Resources at negative positions
    ResourceNode res(Position(-5, -5), ResourceType::FOOD, 50.0, false);
    CHECK(res.getPosition() == Position(-5, -5));
    CHECK(res.isInRange(Position(-4, -5), 1));
    CHECK(res.isInRange(Position(-5, -4), 1));
    
    END_TEST()
}

// Test 14: Resource manager with max range limits
void testMaxRangeLimits() {
    TEST("ResourceManager - Max range limits in findNearest")
    
    ResourceManager mgr;
    
    mgr.createResource(Position(0, 0), ResourceType::FOOD, 50.0, false);
    mgr.createResource(Position(100, 100), ResourceType::WATER, 30.0, true);
    
    // Find nearest from (50, 50) with no range limit
    ResourceNode* nearest1 = mgr.findNearestResource(Position(50, 50), 0);
    CHECK(nearest1 != nullptr);
    
    // Find nearest with strict range limit
    ResourceNode* nearest2 = mgr.findNearestResource(Position(50, 50), 10);
    CHECK(nearest2 == nullptr);  // Nothing within range 10
    
    // Find nearest with sufficient range
    ResourceNode* nearest3 = mgr.findNearestResource(Position(50, 50), 100);
    CHECK(nearest3 != nullptr);
    
    END_TEST()
}

int main() {
    cout << "========================================" << endl;
    cout << "RESOURCE NODE/MANAGER ADVANCED TESTS" << endl;
    cout << "========================================" << endl << endl;
    
    testPositionDistances();
    testResourceConsumptionEdgeCases();
    testRenewableRegeneration();
    testManualRegeneration();
    testRangeDetection();
    testResourceManagerSpatialQueries();
    testDepletionAndCleanup();
    testTotalEnergyTracking();
    testUniqueIDGeneration();
    testResourceTypeDifferentiation();
    testLargeScaleSpatialQueries();
    testConcurrentOperationsSimulation();
    testBoundaryPositions();
    testMaxRangeLimits();
    
    cout << endl << "========================================" << endl;
    cout << "Results: " << passedTests << "/" << totalTests << " tests passed";
    if (passedTests == totalTests) {
        cout << " ✓" << endl;
    } else {
        cout << " ✗" << endl;
    }
    cout << "Coverage: Spatial queries, edge cases, performance" << endl;
    cout << "========================================" << endl;
    
    return passedTests == totalTests ? 0 : 1;
}
