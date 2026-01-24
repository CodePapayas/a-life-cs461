#include "./source/Environment.h"
#include <iostream>
#include <cassert>      // for testing

// second usecase of Vector2d, may warrant class construction.
class Vector2d{
	public:
		float x;
		float y;
		Vector2d(){x = 0; y = 0;}
		Vector2d(int n_x, int n_y){x = n_x; y = n_y;}
		float dot(Vector2d other) {return (x * other.x + y * other.y);} 
};

void test_DisplayEnvironment(Environment *env){
    std::cout << "Generating environment grid..." << std::endl;
    int chunks = env->getChunksInEnvironment();
    int tiles = env->getTilesPerChunk();
    int env_size = tiles * chunks;
    std::cout << "Displaying " << env_size << "x" << env_size << " tile environment grid." << std::endl;
    for(int y = 0; y < env_size; y++){
        for(int x = 0; x < env_size; x++){
            std::cout << env->getTileValue(Vector2d(x,y)) << "\t ";
        }
        std::cout << std::endl;
    }
}

void test_GetChunkFromID(Environment *env){
    int get_id = 0;
    assert(env->getChunkFromID(get_id).x == 0 && env->getChunkFromID(get_id).y == 0);
    std::cout << "PASSED: Got chunk from ID" << std::endl;
}

int main(){
    Environment height_map; // generates a 2d array of normalized floats
    test_DisplayEnvironment(&height_map);
    test_GetChunkFromID(&height_map);
    return 0;
}