#include "./Header Files/Environment.h"
#include "iostream"

int main(){
    Environment env;
    int chunks = env.getChunksInEnvironment();
    int tiles = env.getTilesPerChunk();
    int env_size = tiles * chunks;
    for(int y = 0; y < env_size; y++){
        for(int x = 0; x < (tiles*tiles); x++){
            std::cout << env.getTileInfo(x,y);
        }
    }
}