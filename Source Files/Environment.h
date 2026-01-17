#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <tuple>
#include <iostream>

const int chunk_amt = 3;
const int tile_amt = 3;

// placeholder classes for functionality, may be extrapolated into their own files later
class Vector2d;
class Tile;
class Chunk;

class Environment
{
private:
    Chunk **chunks; // initially a fully null array

    double x_origin = ((double)(chunk_amt * tile_amt)) / 2.0;
    double y_origin = ((double)(chunk_amt * tile_amt)) / 2.0;
public:
    Environment();
    ~Environment(){delete[] chunks;}
    std::tuple<Vector2d, Vector2d> toChunkCoord(Vector2d pos);
    int getTileInfo(Vector2d pos);
    int getTilesPerChunk()          {return tile_amt;};
    int getChunksInEnvironment()    {return chunk_amt;};
};

#endif