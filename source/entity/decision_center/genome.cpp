#include "genome.hpp"
#include "biology_constants.hpp"
#include <random>
#include <algorithm>
#include <stdexcept>

std::vector<std::string> Genome::trait_order()
{
    // Keep ordering consistent with GetDefaultGeneticValues
    return {
        "Energy Efficiency",
        "Water Efficiency",
        "Mass",
        "Vision",
        "Chem 1",
        "Chem 2",
        "Chem 3",
        "Chem 4",
        "Traversal Efficiency 1",
        "Traversal Efficiency 2",
        "Traversal Efficiency 3"
    };
}

std::vector<uint8_t> Genome::encodeFromTraits(const std::unordered_map<std::string,double>& traits)
{
    auto order = trait_order();
    std::vector<uint8_t> bytes;
    bytes.reserve(order.size());
    for (const auto& key : order) {
        auto it = traits.find(key);
        double val = 0.0;
        if (it != traits.end()) val = it->second;
        if (val < 0.0) val = 0.0; if (val > 1.0) val = 1.0;
        uint8_t b = static_cast<uint8_t>(std::round(val * 255.0));
        bytes.push_back(b);
    }
    return bytes;
}

std::unordered_map<std::string,double> Genome::decodeToTraits(const std::vector<uint8_t>& bytes)
{
    auto order = trait_order();
    std::unordered_map<std::string,double> out;
    size_t n = std::min(order.size(), bytes.size());
    for (size_t i = 0; i < n; ++i) {
        out[order[i]] = static_cast<double>(bytes[i]) / 255.0;
    }
    for (size_t i = n; i < order.size(); ++i) {
        out[order[i]] = 0.5;
    }
    return out;
}

void Genome::mutateBytes(std::vector<uint8_t>& bytes, double rate, double magnitude)
{
    if (bytes.empty()) return;
    static thread_local std::mt19937 rng((std::random_device())());
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_real_distribution<double> change(-1.0, 1.0);
    for (auto& b : bytes) {
        if (prob(rng) < rate) {
            double delta = change(rng) * magnitude * 255.0;
            int v = static_cast<int>(b) + static_cast<int>(std::round(delta));
            if (v < 0) v = 0; if (v > 255) v = 255;
            b = static_cast<uint8_t>(v);
        }
    }
}

std::vector<uint8_t> Genome::crossover(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b)
{
    size_t n = std::min(a.size(), b.size());
    if (n == 0) return {};
    static thread_local std::mt19937 rng((std::random_device())());
    std::uniform_int_distribution<size_t> cut_dist(1, n-1);
    size_t cut = cut_dist(rng);
    std::vector<uint8_t> child;
    child.reserve(n);
    for (size_t i = 0; i < cut; ++i) child.push_back(a[i]);
    for (size_t i = cut; i < n; ++i) child.push_back(b[i]);
    return child;
}
