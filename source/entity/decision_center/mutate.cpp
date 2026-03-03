#include <cmath>
#include <vector>
#include <numeric>
#include <random>
#include <ctime>
#include <algorithm>
#include "mutate.hpp"
#include <unordered_map>

std::vector<double> mutate_vector(const std::vector<double>& original) {
    std::vector<double> mutated = original;
    std::mt19937 gen(std::time(nullptr));
    std::uniform_real_distribution<double> mutation_dist(-0.05, 0.05); // Mutation changes value by up to ±0.05
    std::uniform_real_distribution<double> chance_dist(0.0, 1.0); // For mutation chance

    for (double& val : mutated) {
        if (chance_dist(gen) < MUTATION_CHANCE) {
            if (chance_dist(gen) < MUTATION_CHANCE) {
                val = chance_dist(gen); // .1% chance to completely randomize the value
            }
            else{
                val += mutation_dist(gen);
                if (val > 1.0) val = 1.0;
                if (val < 0.0) val = 0.0;
            }
        }
    }
    return mutated;
}

std::unordered_map<std::string, double> mutate_genetics(const std::unordered_map<std::string, double>& original) {
    std::unordered_map<std::string, double> mutated;
    for (const auto& pair : original) {
        std::vector<double> single_val_vector = { pair.second };
        std::vector<double> mutated_vector = mutate_vector(single_val_vector);
        mutated[pair.first] = mutated_vector[0];
    }
    return mutated;
}