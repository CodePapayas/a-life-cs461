#ifndef GENOME_HPP
#define GENOME_HPP

#include <vector>
#include <unordered_map>
#include <string>

class Genome {
public:
    // Trait order must match Biology's default map order
    static std::vector<std::string> trait_order();

    // Encode trait map (0.0-1.0) into compact bytes
    static std::vector<uint8_t> encodeFromTraits(const std::unordered_map<std::string,double>& traits);

    // Decode bytes into trait map (values in 0.0-1.0)
    static std::unordered_map<std::string,double> decodeToTraits(const std::vector<uint8_t>& bytes);

    // Mutate bytes in-place. rate = probability per locus, magnitude in [0,1] fraction of full range
    static void mutateBytes(std::vector<uint8_t>& bytes, double rate, double magnitude);

    // Single-point crossover: returns child bytes
    static std::vector<uint8_t> crossover(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);
};

#endif // GENOME_HPP
