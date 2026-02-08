/*
  Author: Kai Lindskog-Coffin
  Oregon State University
  CS 462 
  
  Integration Tests - Multi-Component Interactions
  Coverage: Component interactions, realistic simulation scenarios, end-to-end workflows
*/

#include "../include/circular_buffer.h"
#include "../include/simulation_state.h"
#include "../include/fitness_calculator.h"
#include "../include/resource_node.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

using namespace std;

int totalTests = 0, passedTests = 0;

#define TEST(name) totalTests++; cout << "[TEST] " << name << "... "; bool ok = true;
#define CHECK(cond) if (!(cond)) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #cond; }
#define CHECK_APPROX(a, b, eps) if (abs((a) - (b)) >= eps) { ok = false; cout << "\n  FAIL at line " << __LINE__ << ": " #a " ≈ " #b; }
#define END_TEST() if (ok) { cout << "PASS"; passedTests++; } else { cout << endl; } cout << endl;

// Helper: Simulate an agent in the world
struct Agent {
    Position position;
    double energy;
    double maxEnergy;
    uint64_t age;
    double energyGained;
    double energySpent;
    uint32_t offspring;
    
    Agent(Position pos, double maxE)
        : position(pos), energy(maxE * 0.5), maxEnergy(maxE)
        , age(0), energyGained(0.0), energySpent(0.0), offspring(0)
    {}
    
    double getFitness() const {
        return FitnessCalculator::calculateFitness(
            energy, maxEnergy, age, energyGained, energySpent, offspring
        );
    }
    
    bool isAlive() const {
        return FitnessCalculator::meetsMinimumSurvival(energy, 1.0);
    }
    
    bool canReproduce() const {
        return FitnessCalculator::canReproduce(energy, maxEnergy * 0.8);
    }
};

// Test 1: Agent foraging simulation
void testAgentForagingSimulation() {
    TEST("Integration - Agent foraging simulation")
    
    // Setup environment
    ResourceManager mgr;
    mgr.createResource(Position(5, 5), ResourceType::FOOD, 100.0, true);
    mgr.createResource(Position(10, 10), ResourceType::WATER, 80.0, true);
    mgr.createResource(Position(15, 5), ResourceType::PLANT, 60.0, true);
    
    // Create agent
    Agent agent(Position(5, 5), 100.0);
    CHECK(agent.isAlive());
    
    // Agent forages nearby resources
    auto nearby = mgr.findResourcesInRange(agent.position, 2);
    CHECK(nearby.size() > 0);
    
    for (auto* res : nearby) {
        if (!res->isDepleted()) {
            double consumed = res->consume(20.0);
            agent.energy += consumed;
            agent.energyGained += consumed;
        }
    }
    
    CHECK(agent.energy > 50.0);  // Should have gained energy
    CHECK(agent.energyGained > 0.0);
    
    // Update resources (regeneration)
    mgr.update(1.0);
    
    // Agent survives and ages
    agent.age++;
    double fitness = agent.getFitness();
    CHECK(fitness > 0.0);
    
    END_TEST()
}

// Test 2: Multi-agent competition for resources
void testMultiAgentCompetition() {
    TEST("Integration - Multi-agent resource competition")
    
    // Setup limited resources
    ResourceManager mgr;
    mgr.createResource(Position(10, 10), ResourceType::FOOD, 100.0, false);
    
    // Create multiple agents near the resource
    vector<Agent> agents;
    agents.push_back(Agent(Position(10, 10), 100.0));
    agents.push_back(Agent(Position(11, 10), 100.0));
    agents.push_back(Agent(Position(10, 11), 100.0));
    
    // All agents try to consume
    ResourceNode* resource = mgr.findNearestResource(Position(10, 10));
    CHECK(resource != nullptr);
    
    double initialEnergy = resource->getEnergyValue();
    
    for (auto& agent : agents) {
        if (resource->isInRange(agent.position, 1) && !resource->isDepleted()) {
            double consumed = resource->consume(40.0);
            agent.energy += consumed;
            agent.energyGained += consumed;
        }
    }
    
    // Resource should be partially or fully depleted
    CHECK(resource->getEnergyValue() < initialEnergy);
    
    // Not all agents got equal resources (first-come-first-served)
    CHECK(agents[0].energyGained >= agents[2].energyGained);
    
    END_TEST()
}

// Test 3: Simulation history tracking
void testSimulationHistoryTracking() {
    TEST("Integration - Simulation history tracking with CircularBuffer")
    
    // Create history buffer
    CircularBuffer<SimulationState> history(100);
    
    // Setup simulation
    ResourceManager mgr;
    for (int i = 0; i < 5; i++) {
        mgr.createResource(Position(i * 5, i * 5), ResourceType::FOOD, 50.0, true);
    }
    
    vector<Agent> agents;
    agents.push_back(Agent(Position(0, 0), 100.0));
    agents.push_back(Agent(Position(5, 5), 100.0));
    
    // Run simulation for several ticks
    for (uint64_t tick = 0; tick < 10; tick++) {
        // Create state snapshot
        SimulationState state;
        state.tick = tick;
        state.timestamp = tick * 0.1;
        state.totalEnergy = mgr.getTotalEnergy();
        state.totalResources = mgr.getResourceCount();
        state.agentCount = agents.size();
        
        // Calculate average fitness
        double totalFitness = 0.0;
        double totalEnergy = 0.0;
        for (const auto& agent : agents) {
            totalFitness += agent.getFitness();
            totalEnergy += agent.energy;
        }
        state.averageFitness = totalFitness / agents.size();
        state.averageAgentEnergy = totalEnergy / agents.size();
        
        // Store in history
        history.push(state);
        
        // Simulate tick: agents age and resources regenerate
        for (auto& agent : agents) {
            agent.age++;
            agent.energySpent += 1.0;  // Metabolism
            agent.energy -= 1.0;
        }
        mgr.update(1.0);
    }
    
    // Verify history
    CHECK(history.size() == 10);
    CHECK(history.latest().tick == 9);
    CHECK(history.rewind(9).tick == 0);
    
    // Check that we can rewind and inspect past states
    SimulationState pastState = history.rewind(5);
    CHECK(pastState.tick == 4);
    CHECK(pastState.agentCount == 2);
    
    END_TEST()
}

// Test 4: Agent lifecycle with reproduction
void testAgentLifecycleWithReproduction() {
    TEST("Integration - Agent lifecycle with reproduction")
    
    // Setup rich environment
    ResourceManager mgr;
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            mgr.createResource(Position(x * 3, y * 3), ResourceType::FOOD, 150.0, true);
        }
    }
    
    Agent agent(Position(6, 6), 100.0);
    
    // Simulate many ticks
    int reproductionEvents = 0;
    for (int tick = 0; tick < 50; tick++) {
        // Agent forages
        auto nearby = mgr.findResourcesInRange(agent.position, 3);
        for (auto* res : nearby) {
            if (!res->isDepleted() && agent.energy < agent.maxEnergy) {
                double consumed = res->consume(15.0);
                agent.energy = min(agent.energy + consumed, agent.maxEnergy);
                agent.energyGained += consumed;
            }
        }
        
        // Check reproduction
        if (agent.canReproduce()) {
            agent.offspring++;
            agent.energy -= agent.maxEnergy * 0.5;  // Cost of reproduction
            reproductionEvents++;
        }
        
        // Metabolism
        agent.energy -= 2.0;
        agent.energySpent += 2.0;
        agent.age++;
        
        // Resources regenerate
        mgr.update(1.0);
    }
    
    // Agent should have reproduced at least once in rich environment
    CHECK(reproductionEvents > 0);
    CHECK(agent.offspring > 0);
    CHECK(agent.isAlive());
    
    // Fitness should be reasonable for successful agent
    double fitness = agent.getFitness();
    CHECK(fitness > 0.3);
    
    END_TEST()
}

// Test 5: Resource depletion and recovery
void testResourceDepletionAndRecovery() {
    TEST("Integration - Resource depletion and recovery cycle")
    
    ResourceManager mgr;
    
    // Mix of renewable and non-renewable
    mgr.createResource(Position(0, 0), ResourceType::FOOD, 100.0, false);      // Non-renewable
    mgr.createResource(Position(10, 10), ResourceType::PLANT, 100.0, true);   // Renewable
    
    double initialTotal = mgr.getTotalEnergy();
    
    // Deplete both
    auto resources = mgr.findResourcesInRange(Position(0, 0), 0);
    resources[0]->consume(100.0);
    
    resources = mgr.findResourcesInRange(Position(10, 10), 0);
    resources[0]->consume(100.0);
    
    CHECK(mgr.getTotalEnergy() < 1.0);  // Nearly depleted
    
    // Cleanup non-renewables
    size_t removed = mgr.removeDepletedResources();
    CHECK(removed == 1);  // Only the non-renewable was removed
    CHECK(mgr.getResourceCount() == 1);  // Renewable remains
    
    // Update to regenerate
    for (int i = 0; i < 200; i++) {
        mgr.update(1.0);
    }
    
    // Renewable should have recovered
    CHECK(mgr.getTotalEnergy() > 50.0);
    
    END_TEST()
}

// Test 6: Fitness-based selection simulation
void testFitnessBasedSelection() {
    TEST("Integration - Fitness-based agent selection")
    
    vector<Agent> population;
    
    // Create diverse population
    population.push_back(Agent(Position(0, 0), 100.0));  // Will be weak
    population.push_back(Agent(Position(1, 1), 100.0));  // Will be strong
    population.push_back(Agent(Position(2, 2), 100.0));  // Will be average
    
    // Give them different characteristics
    population[0].energy = 20.0;  // Low energy
    population[0].age = 10;
    population[0].energyGained = 50.0;
    population[0].energySpent = 100.0;  // Inefficient
    
    population[1].energy = 90.0;  // High energy
    population[1].age = 500;        // Old (experienced)
    population[1].energyGained = 500.0;
    population[1].energySpent = 200.0;  // Efficient
    population[1].offspring = 5;
    
    population[2].energy = 50.0;  // Medium
    population[2].age = 100;
    population[2].energyGained = 200.0;
    population[2].energySpent = 200.0;
    population[2].offspring = 2;
    
    // Calculate fitness for all
    double fitness0 = population[0].getFitness();
    double fitness1 = population[1].getFitness();
    double fitness2 = population[2].getFitness();
    
    // Verify fitness ordering matches expectations
    CHECK(fitness1 > fitness2);  // Strong > Average
    CHECK(fitness2 > fitness0);  // Average > Weak
    
    // Simulate selection (keep top 2)
    vector<Agent> survivors;
    for (const auto& agent : population) {
        if (agent.getFitness() > 0.3) {  // Threshold
            survivors.push_back(agent);
        }
    }
    
    CHECK(survivors.size() >= 1);  // At least the strong one survives
    
    END_TEST()
}

// Test 7: Spatial movement and resource finding
void testSpatialMovementAndResourceFinding() {
    TEST("Integration - Spatial movement and resource pathfinding")
    
    ResourceManager mgr;
    
    // Create resources at known locations
    mgr.createResource(Position(20, 20), ResourceType::FOOD, 100.0, false);
    mgr.createResource(Position(50, 50), ResourceType::WATER, 80.0, true);
    
    Agent agent(Position(0, 0), 100.0);
    
    // Find nearest resource
    ResourceNode* nearest = mgr.findNearestResource(agent.position);
    CHECK(nearest != nullptr);
    CHECK(nearest->getPosition() == Position(20, 20));  // Closest one
    
    // Simulate movement toward resource
    while (agent.position.manhattanDistance(nearest->getPosition()) > 1) {
        // Simple movement: reduce distance
        if (agent.position.x < nearest->getPosition().x) agent.position.x++;
        else if (agent.position.x > nearest->getPosition().x) agent.position.x--;
        
        if (agent.position.y < nearest->getPosition().y) agent.position.y++;
        else if (agent.position.y > nearest->getPosition().y) agent.position.y--;
        
        agent.energySpent += 0.5;  // Movement cost
        agent.energy -= 0.5;
        agent.age++;
    }
    
    // Agent should be near the resource now
    CHECK(nearest->isInRange(agent.position, 1));
    
    // Consume the resource
    double consumed = nearest->consume(50.0);
    agent.energy += consumed;
    agent.energyGained += consumed;
    
    CHECK(agent.energy > 50.0);
    CHECK(agent.energyGained > 0.0);
    
    END_TEST()
}

// Test 8: Complete simulation tick
void testCompleteSimulationTick() {
    TEST("Integration - Complete simulation tick workflow")
    
    // Initialize world
    ResourceManager mgr;
    CircularBuffer<SimulationState> history(1000);
    vector<Agent> agents;
    
    // Setup
    for (int i = 0; i < 3; i++) {
        mgr.createResource(Position(i * 10, i * 10), ResourceType::FOOD, 100.0, true);
        agents.push_back(Agent(Position(i * 10, i * 10), 100.0));
    }
    
    // Run one complete tick
    uint64_t tick = 0;
    
    // 1. Agents act
    for (auto& agent : agents) {
        // Find and consume resources
        auto nearby = mgr.findResourcesInRange(agent.position, 2);
        for (auto* res : nearby) {
            if (!res->isDepleted()) {
                double consumed = res->consume(10.0);
                agent.energy += consumed;
                agent.energyGained += consumed;
            }
        }
        
        // Metabolism
        agent.energy -= 1.0;
        agent.energySpent += 1.0;
        agent.age++;
    }
    
    // 2. Update environment
    mgr.update(1.0);
    mgr.removeDepletedResources();
    
    // 3. Record state
    SimulationState state;
    state.tick = tick;
    state.totalEnergy = mgr.getTotalEnergy();
    state.totalResources = mgr.getResourceCount();
    state.agentCount = agents.size();
    
    double totalFitness = 0.0;
    for (const auto& agent : agents) {
        totalFitness += agent.getFitness();
    }
    state.averageFitness = totalFitness / agents.size();
    
    history.push(state);
    
    // Verify state
    CHECK(state.agentCount == 3);
    CHECK(state.totalResources > 0);
    CHECK(state.averageFitness > 0.0);
    CHECK(history.size() == 1);
    
    END_TEST()
}

// Test 9: Long-term simulation stability
void testLongTermSimulationStability() {
    TEST("Integration - Long-term simulation stability (100 ticks)")
    
    ResourceManager mgr;
    CircularBuffer<SimulationState> history(100);
    
    // Create sustainable environment
    for (int i = 0; i < 10; i++) {
        mgr.createResource(Position(i * 5, i * 5), ResourceType::PLANT, 100.0, true);
    }
    
    Agent agent(Position(25, 25), 100.0);
    
    // Run for 100 ticks
    for (uint64_t tick = 0; tick < 100; tick++) {
        // Agent forages
        auto nearby = mgr.findResourcesInRange(agent.position, 10);
        for (auto* res : nearby) {
            if (!res->isDepleted() && agent.energy < agent.maxEnergy * 0.9) {
                double consumed = res->consume(5.0);
                agent.energy += consumed;
                agent.energyGained += consumed;
            }
        }
        
        // Metabolism
        agent.energy -= 1.0;
        agent.energySpent += 1.0;
        agent.age++;
        
        // Environment update
        mgr.update(1.0);
        
        // Record state
        SimulationState state;
        state.tick = tick;
        state.totalEnergy = mgr.getTotalEnergy();
        state.agentCount = agent.isAlive() ? 1 : 0;
        state.averageFitness = agent.getFitness();
        history.push(state);
    }
    
    // Verify stability
    CHECK(agent.isAlive());
    CHECK(history.size() == 100);
    CHECK(mgr.getTotalEnergy() > 0.0);  // Resources should be regenerating
    
    // Check that early and late states are reasonable
    SimulationState earlyState = history.get(10);
    SimulationState lateState = history.latest();
    
    CHECK(earlyState.tick == 10);
    CHECK(lateState.tick == 99);
    CHECK(lateState.agentCount == 1);  // Agent survived
    
    END_TEST()
}

int main() {
    cout << "========================================" << endl;
    cout << "INTEGRATION TESTS - MULTI-COMPONENT" << endl;
    cout << "========================================" << endl << endl;
    
    testAgentForagingSimulation();
    testMultiAgentCompetition();
    testSimulationHistoryTracking();
    testAgentLifecycleWithReproduction();
    testResourceDepletionAndRecovery();
    testFitnessBasedSelection();
    testSpatialMovementAndResourceFinding();
    testCompleteSimulationTick();
    testLongTermSimulationStability();
    
    cout << endl << "========================================" << endl;
    cout << "Results: " << passedTests << "/" << totalTests << " tests passed";
    if (passedTests == totalTests) {
        cout << " ✓" << endl;
    } else {
        cout << " ✗" << endl;
    }
    cout << "Coverage: Component interactions, realistic scenarios" << endl;
    cout << "========================================" << endl;
    
    return passedTests == totalTests ? 0 : 1;
}
