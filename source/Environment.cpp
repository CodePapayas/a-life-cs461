#include "Environment.h"
// Environment Class
// Responsible for creating, handling, and accessing the simulation environment and it's data.
// Agents and other simulation entities can access specific data necessary

// ------------------------------[TO-DO]------------------------------
// - Implement perlin noise
// - Optimize data storage to be faster
// - More getters and setters, just cause
// - Create a "key : value" system for efficient chunk and tile lookup purposes
// - Refactor tiles and chunks to remain ungenerated until accessed
//      - Likely won't be until we get Agent functionality implemented
// - Stop using so many darn nested loops where possible
// - Adjust the global variables chunk_amt and tile_amt to be configurable at runtime
//      - ... and by extent, adjust the "chunks" and "tiles" arrays/vectors/lists to accomodate this
// - Memory management considerations
//      - Mainly pointer cleanup on deconstruction
// - Class method overloads to allow integer parameters instead of Vector2d

// class for tracking positions, like a <float, float> tuple but with vector math.
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
		int tile_id; // relative to tile index in chunk's Tile array tiles
		int value = 0; // currently an arbitrary value for tracking things like noise
	public:
		Tile() {
            value = rand() % 10; //placeholder for noise
        };
        int getValue(){return value;};
};

// class for holding chunk data
class Chunk{
	int chunk_id; 
    public:
        std::vector<std::vector<Tile*>> tiles; // added vector dep to change up arrays
    	Chunk(){
            for(int x = 0; x < tile_amt; x++){
                std::vector<Tile*> tile_col;
                for(int y = 0; y < tile_amt; y++){
                    tile_col.push_back(new Tile());
                }
                tiles.push_back(tile_col);
            }
        };
};

// constructor for environment
// currently, generates the whole chunk grid
Environment::Environment(){
    for(int x = 0; x < chunk_amt; x++){
        std::vector<Chunk*> chunk_col;
        for(int y = 0; y < chunk_amt; y++){
            chunk_col.push_back(new Chunk());
        }
        chunks.push_back(chunk_col);
    }
};

//(some type for noise) Noise = (some type for noise)(); // get the current noise we are sampling from
std::tuple<Vector2d, Vector2d> Environment::toChunkCoord(Vector2d pos){
    // Converts absolute position coordinates to the array index system.
    // Takes in the pos value and clamps it to the range of the chunks array and the tiles array inside chunks.
    // Input: Vector2d pos;
    // Ouput: Vector2d chunk_pos, Vector2d tile_pos;
    
    int true_x = 	pos.x; //pos.x - x_origin;
    int true_y = 	pos.y; //pos.y - y_origin; // got ambitious here, wanted to account for if (0,0) was the center of the grid and not the top left corner
    
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

int Environment::getTileInfo(Vector2d pos){
    auto[chunk_pos, tile_pos] = this->toChunkCoord(pos);

    Chunk *curr_chunk   = chunks[(int)chunk_pos.x][(int)chunk_pos.y];
    Tile *curr_tile     = curr_chunk->tiles[(int)tile_pos.x][(int)tile_pos.y];

    int result          = curr_tile->getValue();
    return result;
};