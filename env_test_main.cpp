#include "./source/Environment.h"
#include <iostream>
#include <cassert>      // for testing
#include "./source/MathVector.hpp"
#include "./source/PerlinNoise.hpp"

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
    std::cout << env->getTileValues(Vector2d(-1,-1))[0] << "\t ";
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

void test_SampleNoiseLayered(){
    PerlinNoise2d noise = PerlinNoise2d();
    noise.SetFrequency(1);
    noise.SetAmplitude(200);
    noise.SetOctaves(8);
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            std::cout << noise.SampleNoiseLayered(Vector2d(x,y)) << "\t ";
        }
        std::cout << std::endl;
    }
}

int main(){
    Environment height_map; // generates a 2d array of normalized floats
    test_DisplayEnvironment(&height_map);
    test_GetChunkFromID(&height_map);
    test_GetTileValue(&height_map);
    test_SampleNoiseLayered();
    return 0;
}