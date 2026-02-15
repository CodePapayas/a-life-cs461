#include "./source/Environment.h"
#include <iostream>
#include <cassert>      // for testing
#include "./source/MathVector.hpp"
#include "./source/PerlinNoise.hpp"
#include <iomanip>

void test_DisplayEnvironment(Environment *env){
    std::cout << "Generating environment grid..." << std::endl;
    int tiles = sqrt(env->getTileAmountSquared());
    std::cout << "Displaying " << tiles << "x" << tiles << " tile environment grid." << std::endl;
    for(int y = 0; y < tiles; y++){
        for(int x = 0; x < tiles; x++){
            std::cout << env->getTileValues(Vector2d(x,y))[0] << "\t ";
        }
        std::cout << std::endl;
    }
}

void test_GetTileValue(Environment *env){
    int tiles = env->getTileAmountSquared();
    std::cout << "Testing bounds..." << std::endl;
    std::cout << "Below zero..." << std::endl;
    std::cout << env->getTileValues(Vector2d(0,-1))[0] << "\t ";
    std::cout << env->getTileValues(Vector2d(-1, 0))[0] << "\t ";
    std::cout << env->getTileValues(Vector2d(-1,-1))[0] << "\t\n";
    std::cout << "Above limit..." << std::endl;
    std::cout << env->getTileValues(Vector2d(0, sqrt(tiles)))[0] << "\t ";
    std::cout << env->getTileValues(Vector2d(sqrt(tiles), 0))[0] << "\t ";
    std::cout << env->getTileValues(Vector2d(sqrt(tiles), sqrt(tiles)))[0] << "\t\n";
}


void test_GetChunkFromID(Environment *env){
    std::cout << "Get chunk from id..." << std::endl;
    int get_id = 0;
    for(int x = 0; x <  sqrt(env->getTileAmountSquared()) ; x++){
        for(int y = 0; y <  sqrt(env->getTileAmountSquared()) ; y++){
            Vector2d curr_tile = env->getTileFromID(get_id);
            assert(curr_tile.x == x && curr_tile.y == y);
            get_id++;
        }
    }
}

void test_SampleNormalized(){
    PerlinNoise2d noise = PerlinNoise2d();
    double amp = 200.0;
    noise.SetFrequency(0.05);
    noise.SetAmplitude(amp);
    noise.SetOctaves(8);
    int size = 32;
    for(int y = 0; y < size; y++){
        for(int x = 0; x < size; x++){
            double noise_val = noise.SampleNormalized(Vector2d(x,y));
            noise_val = round(10.0 * noise_val) / 10.0;
            std::cout << std::setprecision(1) << noise_val << " ";
        }
        std::cout << std::endl;
    }
}

void test_SampleNormalized_Seeded(){
    PerlinNoise2d noise = PerlinNoise2d(101);
    double amp = 200.0;
    noise.SetFrequency(0.05);
    noise.SetAmplitude(amp);
    noise.SetOctaves(8);
    int size = 32;
    for(int y = 0; y < size; y++){
        for(int x = 0; x < size; x++){
            double noise_val = noise.SampleNormalized(Vector2d(x,y));
            noise_val = round(10.0 * noise_val) / 10.0;
            std::cout << std::setprecision(1) << noise_val << " ";
        }
        std::cout << std::endl;
    }
}

int main(){
    Environment height_map = Environment(16,16); // generates a 2d array of normalized floats
    test_DisplayEnvironment(&height_map);
    test_GetChunkFromID(&height_map);
    test_GetTileValue(&height_map);
    test_SampleNormalized();
    test_SampleNormalized_Seeded();
    test_SampleNormalized_Seeded();
    return 0;
}