/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462 
  
  Advanced FitnessCalculator Tests
  Coverage: Boundary values, extreme cases, weight configurations, edge conditions
*/

#include "../include/fitness_calculator.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <limits>

using namespace std;

int totalTests = 0, passedTests = 0;

#define TEST(name) totalTests++; cout << "[TEST] " << name << "... "; bool ok = true;
#define CHECK(cond) if (!(cond)) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #cond; }
#define CHECK_APPROX(a, b, eps) if (abs((a) - (b)) >= eps) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #a " ≈ " #b; }
#define END_TEST() if (ok) { cout << "PASS"; passedTests++; } else { cout << endl; } cout << endl;

// Test 1: Energy score boundary conditions
void testEnergyScoreBoundaries() {
    TEST("FitnessCalculator - Energy score boundaries")
    
    // Zero energy
    CHECK_APPROX(FitnessCalculator::energyScore(0.0, 100.0), 0.0, 0.001);
    
    // Full energy
    CHECK_APPROX(FitnessCalculator::energyScore(100.0, 100.0), 1.0, 0.001);
    
    // Over max (should clamp to 1.0)
    CHECK_APPROX(FitnessCalculator::energyScore(150.0, 100.0), 1.0, 0.001);
    
    // Negative energy (should clamp to 0.0)
    CHECK_APPROX(FitnessCalculator::energyScore(-10.0, 100.0), 0.0, 0.001);
    
    // Zero max energy (edge case)
    CHECK_APPROX(FitnessCalculator::energyScore(50.0, 0.0), 0.0, 0.001);
    
    // Very small values
    CHECK_APPROX(FitnessCalculator::energyScore(0.001, 0.01), 0.1, 0.001);
    
    END_TEST()
}

// Test 2: Survival score with extreme ages
void testSurvivalScoreExtremes() {
    TEST("FitnessCalculator - Survival score extreme ages")
    
    // Age 0
    CHECK_APPROX(FitnessCalculator::survivalScore(0), 0.0, 0.001);
    
    // Age 1
    CHECK(FitnessCalculator::survivalScore(1) > 0.0);
    CHECK(FitnessCalculator::survivalScore(1) < 1.0);
    
    // Very old age
    CHECK(FitnessCalculator::survivalScore(10000) > 0.0);
    CHECK_APPROX(FitnessCalculator::survivalScore(10000), 1.0, 0.001);  // Should approach 1.0
    
    // Logarithmic scaling verification: older should be higher
    CHECK(FitnessCalculator::survivalScore(100) > FitnessCalculator::survivalScore(50));
    CHECK(FitnessCalculator::survivalScore(1000) > FitnessCalculator::survivalScore(100));
    
    // But with diminishing returns
    double diff1 = FitnessCalculator::survivalScore(100) - FitnessCalculator::survivalScore(50);
    double diff2 = FitnessCalculator::survivalScore(1000) - FitnessCalculator::survivalScore(950);
    CHECK(diff1 > diff2);  // Earlier gains are larger
    
    END_TEST()
}

// Test 3: Efficiency score edge cases
void testEfficiencyScoreEdgeCases() {
    TEST("FitnessCalculator - Efficiency score edge cases")
    
    // Perfect efficiency: gained more than spent
    CHECK(FitnessCalculator::efficiencyScore(200.0, 100.0) > 0.5);
    
    // Break-even
    CHECK_APPROX(FitnessCalculator::efficiencyScore(100.0, 100.0), 0.5, 0.001);
    
    // Poor efficiency
    CHECK(FitnessCalculator::efficiencyScore(50.0, 100.0) < 0.5);
    
    // Zero spending with gains
    CHECK_APPROX(FitnessCalculator::efficiencyScore(100.0, 0.0), 1.0, 0.001);
    
    // Zero spending, zero gains
    CHECK_APPROX(FitnessCalculator::efficiencyScore(0.0, 0.0), 0.0, 0.001);
    
    // Negative values (shouldn't happen in practice, but test robustness)
    CHECK(FitnessCalculator::efficiencyScore(-10.0, 100.0) >= 0.0);
    
    // Very high efficiency
    CHECK(FitnessCalculator::efficiencyScore(1000.0, 10.0) > 0.9);
    
    END_TEST()
}

// Test 4: Reproduction score scaling
void testReproductionScoreScaling() {
    TEST("FitnessCalculator - Reproduction score scaling")
    
    // No offspring
    CHECK_APPROX(FitnessCalculator::reproductionScore(0), 0.0, 0.001);
    
    // One offspring
    CHECK(FitnessCalculator::reproductionScore(1) > 0.0);
    
    // Multiple offspring
    CHECK(FitnessCalculator::reproductionScore(5) > FitnessCalculator::reproductionScore(1));
    CHECK(FitnessCalculator::reproductionScore(10) > FitnessCalculator::reproductionScore(5));
    
    // Diminishing returns
    double diff1 = FitnessCalculator::reproductionScore(2) - FitnessCalculator::reproductionScore(1);
    double diff2 = FitnessCalculator::reproductionScore(20) - FitnessCalculator::reproductionScore(19);
    CHECK(diff1 > diff2);
    
    // Very high offspring count
    CHECK(FitnessCalculator::reproductionScore(100) <= 1.0);
    
    END_TEST()
}

// Test 5: Custom weight configurations
void testCustomWeights() {
    TEST("FitnessCalculator - Custom weight configurations")
    
    FitnessCalculator::FitnessWeights weights;
    
    // 100% energy weight
    weights.energyWeight = 1.0;
    weights.survivalWeight = 0.0;
    weights.efficiencyWeight = 0.0;
    weights.reproductionWeight = 0.0;
    
    double fitness = FitnessCalculator::calculateFitness(75.0, 100.0, 100, 500.0, 300.0, 5, weights);
    CHECK_APPROX(fitness, 0.75, 0.001);
    
    // 100% survival weight
    weights.energyWeight = 0.0;
    weights.survivalWeight = 1.0;
    
    double survivalOnly = FitnessCalculator::calculateFitness(50.0, 100.0, 100, 500.0, 300.0, 5, weights);
    double expectedSurvival = FitnessCalculator::survivalScore(100);
    CHECK_APPROX(survivalOnly, expectedSurvival, 0.001);
    
    // 100% efficiency weight
    weights.survivalWeight = 0.0;
    weights.efficiencyWeight = 1.0;
    
    double efficiencyOnly = FitnessCalculator::calculateFitness(50.0, 100.0, 100, 500.0, 300.0, 5, weights);
    double expectedEfficiency = FitnessCalculator::efficiencyScore(500.0, 300.0);
    CHECK_APPROX(efficiencyOnly, expectedEfficiency, 0.001);
    
    END_TEST()
}

// Test 6: Extreme parameter combinations
void testExtremeParameterCombinations() {
    TEST("FitnessCalculator - Extreme parameter combinations")
    
    // Maximum everything
    double maxFitness = FitnessCalculator::calculateFitness(1000.0, 1000.0, 100000, 100000.0, 1.0, 1000);
    CHECK(maxFitness <= 1.0);
    CHECK(maxFitness > 0.9);
    
    // Minimum everything
    double minFitness = FitnessCalculator::calculateFitness(0.0, 100.0, 0, 0.0, 100.0, 0);
    CHECK(minFitness >= 0.0);
    CHECK(minFitness < 0.1);
    
    // Mixed: high energy, low survival
    double mixed1 = FitnessCalculator::calculateFitness(100.0, 100.0, 1, 100.0, 10.0, 0);
    
    // Mixed: low energy, high survival
    double mixed2 = FitnessCalculator::calculateFitness(10.0, 100.0, 10000, 100.0, 10.0, 5);
    
    // Both should be positive and bounded
    CHECK(mixed1 > 0.0 && mixed1 <= 1.0);
    CHECK(mixed2 > 0.0 && mixed2 <= 1.0);
    
    END_TEST()
}

// Test 7: Fitness ordering consistency
void testFitnessOrdering() {
    TEST("FitnessCalculator - Fitness ordering consistency")
    
    // Healthy agent
    double healthy = FitnessCalculator::calculateFitness(
        90.0, 100.0,    // High energy
        1000,           // Old age
        1000.0, 500.0,  // Good efficiency
        5               // Multiple offspring
    );
    
    // Struggling agent
    double struggling = FitnessCalculator::calculateFitness(
        20.0, 100.0,    // Low energy
        50,             // Young
        200.0, 300.0,   // Poor efficiency
        0               // No offspring
    );
    
    // Average agent
    double average = FitnessCalculator::calculateFitness(
        50.0, 100.0,    // Medium energy
        200,            // Medium age
        500.0, 500.0,   // Break-even
        2               // Some offspring
    );
    
    // Verify ordering
    CHECK(healthy > average);
    CHECK(average > struggling);
    CHECK(healthy > struggling);
    
    // All should be in valid range
    CHECK(healthy >= 0.0 && healthy <= 1.0);
    CHECK(average >= 0.0 && average <= 1.0);
    CHECK(struggling >= 0.0 && struggling <= 1.0);
    
    END_TEST()
}

// Test 8: Survival thresholds
void testSurvivalThresholds() {
    TEST("FitnessCalculator - Survival thresholds")
    
    // Meets minimum survival
    CHECK(FitnessCalculator::meetsMinimumSurvival(50.0, 10.0));
    CHECK(FitnessCalculator::meetsMinimumSurvival(10.0, 10.0));  // Exact match
    CHECK(!FitnessCalculator::meetsMinimumSurvival(5.0, 10.0));
    
    // Reproduction threshold
    CHECK(FitnessCalculator::canReproduce(100.0, 50.0));
    CHECK(FitnessCalculator::canReproduce(50.0, 50.0));  // Exact match
    CHECK(!FitnessCalculator::canReproduce(40.0, 50.0));
    
    // Edge cases
    CHECK(!FitnessCalculator::meetsMinimumSurvival(0.0, 0.1));
    CHECK(FitnessCalculator::canReproduce(1000.0, 0.0));  // Zero threshold
    
    END_TEST()
}

// Test 9: Zero and negative input handling
void testZeroAndNegativeInputs() {
    TEST("FitnessCalculator - Zero and negative input handling")
    
    // All zeros
    double allZeros = FitnessCalculator::calculateFitness(0.0, 100.0, 0, 0.0, 0.0, 0);
    CHECK(allZeros >= 0.0 && allZeros <= 1.0);
    
    // Negative current energy (shouldn't happen, but be robust)
    double negEnergy = FitnessCalculator::calculateFitness(-10.0, 100.0, 50, 100.0, 50.0, 1);
    CHECK(negEnergy >= 0.0 && negEnergy <= 1.0);
    
    // Zero max energy
    double zeroMax = FitnessCalculator::calculateFitness(50.0, 0.0, 50, 100.0, 50.0, 1);
    CHECK(zeroMax >= 0.0 && zeroMax <= 1.0);
    
    END_TEST()
}

// Test 10: Weight sum validation
void testWeightSumValidation() {
    TEST("FitnessCalculator - Weight sum edge cases")
    
    FitnessCalculator::FitnessWeights weights;
    
    // Weights summing to less than 1.0
    weights.energyWeight = 0.2;
    weights.survivalWeight = 0.2;
    weights.efficiencyWeight = 0.2;
    weights.reproductionWeight = 0.2;
    
    double fitness1 = FitnessCalculator::calculateFitness(75.0, 100.0, 100, 500.0, 300.0, 5, weights);
    CHECK(fitness1 >= 0.0 && fitness1 <= 1.0);
    
    // Weights summing to more than 1.0
    weights.energyWeight = 0.5;
    weights.survivalWeight = 0.5;
    weights.efficiencyWeight = 0.5;
    weights.reproductionWeight = 0.5;
    
    double fitness2 = FitnessCalculator::calculateFitness(75.0, 100.0, 100, 500.0, 300.0, 5, weights);
    CHECK(fitness2 >= 0.0 && fitness2 <= 1.0);  // Should still clamp to [0,1]
    
    END_TEST()
}

// Test 11: Precision and rounding
void testPrecisionAndRounding() {
    TEST("FitnessCalculator - Precision and rounding")
    
    // Very small differences
    double fitness1 = FitnessCalculator::calculateFitness(50.001, 100.0, 100, 500.0, 300.0, 5);
    double fitness2 = FitnessCalculator::calculateFitness(50.002, 100.0, 100, 500.0, 300.0, 5);
    
    // Should be very close but different
    CHECK(abs(fitness1 - fitness2) < 0.01);
    
    // Very large numbers
    double largeFitness = FitnessCalculator::calculateFitness(
        1e6, 1e6,      // Large energy values
        1000000,        // Very old
        1e8, 1e7,      // Large energy transactions
        10000           // Many offspring
    );
    CHECK(largeFitness >= 0.0 && largeFitness <= 1.0);
    
    END_TEST()
}

// Test 12: Realistic agent lifecycle scenarios
void testRealisticLifecycleScenarios() {
    TEST("FitnessCalculator - Realistic agent lifecycle scenarios")
    
    // Newborn agent
    double newborn = FitnessCalculator::calculateFitness(
        50.0, 100.0,   // Starting energy
        0,              // Just born
        0.0, 0.0,      // No activity yet
        0               // No offspring
    );
    
    // Young adult
    double youngAdult = FitnessCalculator::calculateFitness(
        70.0, 100.0,   // Good energy
        100,            // Some age
        300.0, 200.0,  // Positive efficiency
        1               // First offspring
    );
    
    // Mature agent
    double mature = FitnessCalculator::calculateFitness(
        80.0, 100.0,   // High energy
        500,            // Mature
        2000.0, 1000.0, // Very efficient
        8               // Multiple offspring
    );
    
    // Elderly declining agent
    double elderly = FitnessCalculator::calculateFitness(
        30.0, 100.0,   // Declining energy
        2000,           // Very old
        5000.0, 6000.0, // Declining efficiency
        10              // Had many offspring
    );
    
    // Verify lifecycle progression makes sense
    CHECK(youngAdult > newborn);
    CHECK(mature > youngAdult);
    
    // All should be valid
    CHECK(newborn >= 0.0 && newborn <= 1.0);
    CHECK(youngAdult >= 0.0 && youngAdult <= 1.0);
    CHECK(mature >= 0.0 && mature <= 1.0);
    CHECK(elderly >= 0.0 && elderly <= 1.0);
    
    END_TEST()
}

int main() {
    cout << "========================================" << endl;
    cout << "FITNESS CALCULATOR ADVANCED TEST SUITE" << endl;
    cout << "========================================" << endl << endl;
    
    testEnergyScoreBoundaries();
    testSurvivalScoreExtremes();
    testEfficiencyScoreEdgeCases();
    testReproductionScoreScaling();
    testCustomWeights();
    testExtremeParameterCombinations();
    testFitnessOrdering();
    testSurvivalThresholds();
    testZeroAndNegativeInputs();
    testWeightSumValidation();
    testPrecisionAndRounding();
    testRealisticLifecycleScenarios();
    
    cout << endl << "========================================" << endl;
    cout << "Results: " << passedTests << "/" << totalTests << " tests passed";
    if (passedTests == totalTests) {
        cout << " ✓" << endl;
    } else {
        cout << " ✗" << endl;
    }
    cout << "Coverage: Boundary values, extreme cases, weight configs" << endl;
    cout << "========================================" << endl;
    
    return passedTests == totalTests ? 0 : 1;
}
