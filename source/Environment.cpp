/*
 *  Dillon Stickler - Oregon State University - 2026
 */

#include "Environment.h"
#include "MathVector.hpp"
// Environment Class
// Responsible for creating, handling, and accessing the simulation environment data.
// Agents and other simulation entities can access specific data necessary through
// accessing the environment, the desired chunk, and the desired tile in the chunk.

// ------------------------------[TO-DO]------------------------------
// - Implement perlin noise
// - Optimize data storage to be faster
// - More getters and setters, just cause
// - Refactor tiles and chunks to remain ungenerated until accessed
//      - Likely won't be until we get Agent functionality implemented
// - Adjust the global variables chunk_amt and tile_amt to be configurable at runtime
//      - ... and by extent, adjust the "chunks" and "tiles" arrays/vectors/lists to accomodate this
// - Memory management considerations
//      - Mainly pointer cleanup on deconstruction
// - Class method overloads to allow integer parameters instead of Vector2d

// Tile class, holds environment data at coordinate position.
class Tile{
	std::vector<double> values; // currently an arbitrary value for tracking things like noise
	public:
		Tile(std::vector<double> value_list) {
            for(auto value : value_list){
                values.push_back(value); //placeholder random value noise
            }
        };
        std::vector<double> getValues(){return values;};          //
};

// constructor for environment
// currently, generates the whole chunk grid
Environment::Environment(){
    for(int x = 0; x < tile_amt; x++){
        std::vector<Tile*> tile_col;
        for(int y = 0; y < tile_amt; y++){
            int curr_id = tile_map.size();
            tile_map[curr_id] = Vector2d(x,y);
            tile_col.push_back(new Tile({(double)(rand() % 11) / 10.0}));
        }
        tiles.push_back(tile_col);
    }
};

// function that converts and clamps passed position data to chunk, tile coordinates of range [0, chunks * tiles per chunk - 1].
Vector2d Environment::boundCoords(Vector2d pos){
    // Converts absolute position coordinates to the array index system.
    // Takes in the pos value and clamps it to the range of the chunks array and the tiles array inside chunks.
    // Input: Vector2d pos;
    // Ouput: Vector2d tile_pos;
    
    int tile_x = 	pos.x;
    int tile_y = 	pos.y;

    if (tile_x < 0)                {tile_x = 0;}
    else if (tile_x >= tile_amt)  {tile_x = tile_amt - 1;}
    if (tile_y < 0)                {tile_y = 0;}
    else if (tile_y >= tile_amt)  {tile_y = tile_amt - 1;}

    Vector2d tile_pos =		Vector2d(tile_x, tile_y);
    
    return tile_pos;
}

std::vector<double> Environment::getTileValues(Vector2d pos){
    // input: Vector2 position; Desired X,Y coordinate access in environment
    // ouput: float value; the value in the tile

    pos = boundCoords(pos);

    Tile *curr_tile = tiles[pos.x][pos.y];

    std::vector<double> result = curr_tile->getValues();
    return result;
};

Vector2d Environment::getTileFromID(int id){
    // input: int ID; Desired chunk id
    // output: Vector2d chunk_coord; The coordinate position of the chunk.
    Vector2d *chunk_coord = &tile_map[id];
    if(chunk_coord){
        return *chunk_coord;
    }
    return Vector2d(-1, -1);
};