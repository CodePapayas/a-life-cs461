// simple_simulation/simulation.h
// Author: Kai Lindskog
// Date: November 11, 2025

#pragma once

#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <memory>

using namespace std;

struct Genome {
    vector<float> genes;
    
    Genome(size_t size = 8) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);
        for (size_t i = 0; i < size; ++i) genes.push_back(dis(gen));
    }
    
    void mutate(float rate) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);
        for (auto& gene : genes) {
            if (dis(gen) < rate) {
                float val = gene + (dis(gen) - 0.5f) * 0.3f;
                gene = max(0.0f, min(1.0f, val));
            }
        }
    }
};

struct Food {
    float x, y;
    bool eaten = false;
};

struct Organism {
    int id;
    Genome genome;
    float x, y, energy, r, g, b;
    int age = 0;
    
    Organism(int id, float x, float y) : id(id), x(x), y(y), energy(100.0f) {
        r = genome.genes[0];
        g = genome.genes[1];
        b = genome.genes[2];
    }
    
    void seek(float tx, float ty, float speed) {
        float dx = tx - x, dy = ty - y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > 1.0f) {
            x += (dx / dist) * speed;
            y += (dy / dist) * speed;
        }
    }
};

class Simulation {
public:
    vector<Organism> organisms;
    vector<Food> food;
    int next_id = 0;
    int starting_organisms = 5;
    int starting_food = 40;
    float food_spawn_rate = 0.03f;
    
    Simulation() { reset(); }
    
    void reset() {
        organisms.clear();
        food.clear();
        next_id = 0;
        spawn_food(starting_food);
        for (int i = 0; i < starting_organisms; ++i) spawn_organism();
    }
    
    void spawn_organism() {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dx(50, 1180), dy(50, 670);
        organisms.emplace_back(next_id++, dx(gen), dy(gen));
    }
    
    void spawn_food(int count) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dx(20, 1260), dy(20, 700);
        for (int i = 0; i < count; ++i) food.push_back({dx(gen), dy(gen), false});
    }
    
    void update() {
        for (auto& org : organisms) {
            org.age++;
            org.energy -= 0.15f;
            
            // Find nearest food
            float nearest_dist = 1e9;
            Food* nearest_food = nullptr;
            for (auto& f : food) {
                if (f.eaten) continue;
                float dx = f.x - org.x, dy = f.y - org.y;
                float dist = sqrt(dx * dx + dy * dy);
                if (dist < nearest_dist) {
                    nearest_dist = dist;
                    nearest_food = &f;
                }
            }
            
            // Move toward food
            if (nearest_food) {
                float speed = org.genome.genes[3] * 2.0f + 0.5f;
                org.seek(nearest_food->x, nearest_food->y, speed);
                
                // Eat food if close enough
                if (nearest_dist < 15.0f) {
                    nearest_food->eaten = true;
                    org.energy += 40.0f;
                }
            }
            
            // Boundary wrap
            if (org.x < 0) org.x = 1280;
            if (org.x > 1280) org.x = 0;
            if (org.y < 0) org.y = 720;
            if (org.y > 720) org.y = 0;
        }
        
        // Mating
        for (size_t i = 0; i < organisms.size(); ++i) {
            auto& org1 = organisms[i];
            if (org1.energy < 120.0f || org1.age < 100) continue;
            
            for (size_t j = i + 1; j < organisms.size(); ++j) {
                auto& org2 = organisms[j];
                if (org2.energy < 120.0f || org2.age < 100) continue;
                
                float dx = org1.x - org2.x, dy = org1.y - org2.y;
                float dist = sqrt(dx * dx + dy * dy);
                
                if (dist < 30.0f) {
                    org1.energy -= 50.0f;
                    org2.energy -= 50.0f;
                    
                    Organism child(next_id++, (org1.x + org2.x) / 2, (org1.y + org2.y) / 2);
                    for (size_t k = 0; k < child.genome.genes.size(); ++k) {
                        child.genome.genes[k] = (org1.genome.genes[k] + org2.genome.genes[k]) / 2.0f;
                    }
                    child.genome.mutate(0.15f);
                    child.r = child.genome.genes[0];
                    child.g = child.genome.genes[1];
                    child.b = child.genome.genes[2];
                    organisms.push_back(child);
                    break;
                }
            }
        }
        
        // Remove dead organisms
        organisms.erase(remove_if(organisms.begin(), organisms.end(), 
            [](const Organism& o) { return o.energy <= 0; }), organisms.end());
        
        // Remove eaten food
        food.erase(remove_if(food.begin(), food.end(), 
            [](const Food& f) { return f.eaten; }), food.end());
        
        // Spawn new food at fixed rate
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> chance(0.0, 1.0);
        if (chance(gen) < food_spawn_rate) spawn_food(1);
    }
};
