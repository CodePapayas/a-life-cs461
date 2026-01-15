#include "./Header Files/Environment.h"

const int chunk_amt = 32;
const int tile_amt = 3;

class Vector2d{
	public:
		float x;
		float y;
		Vector2d(){x = 0; y = 0;}
		Vector2d(int n_x, int n_y){x = n_x; y = n_y;}
		float dot(Vector2d other) {return (x * other.x + y * other.y);} 
};

// class for holding tile data
class Tile{
	private:
		int tile_id;
		int value = 0; // currently an arbitrary value for tracking things like noise
	public:
		Tile();
};


// class for holding chunk data
class Chunk{
	int chunk_id;
	Tile tiles[tile_amt][tile_amt]; 
	// all neighboring chunks
    public:
		Chunk();
};

Environment::Environment(){};

// stores overall environment map for easy ID lookup

//(some type for noise) Noise = (some type for noise)(); // get the current noise we are sampling from

std::tuple<Vector2d, Vector2d> Environment::toChunkCoord(Vector2d pos){
    // Converts absolute position coordinates to the array index system.
    // Takes in the pos value and clamps it to the range of the chunks array and the tiles array inside chunks.
    // Input: Vector2d pos;
    // Ouput: Vector2d chunk_pos, Vector2d tile_pos;
    
    int true_x = 	pos.x - x_origin;
    int true_y = 	pos.y - y_origin;
    
    int chunk_x = 	true_x / tile_amt;
    int chunk_y = 	true_y / tile_amt;
    
    int tile_x = 	true_x % tile_amt;
    int tile_y = 	true_y % tile_amt;
    
    // keep chunk position bound
    if (chunk_x < 0) {chunk_x = 0;}
    else if (chunk_y >= chunk_amt) {chunk_y = chunk_amt - 1;}
    if (chunk_y < 0) {chunk_y = 0;}
    else if (chunk_y >= chunk_amt) {chunk_y = chunk_amt - 1;}
    
    Vector2d chunk_pos = 	Vector2d(chunk_x, chunk_y);
    Vector2d tile_pos =		Vector2d(tile_x, tile_y);
    
    return {chunk_pos, tile_pos};
}