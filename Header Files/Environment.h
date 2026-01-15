#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <tuple>

// placeholder classes for functionality, may be extrapolated into their own files
class Vector2d;
class Tile;
class Chunk;

class Environment
{
private:
    Chunk *chunks; // initially a fully null array

    double x_origin = ((double)(chunk_amt * tile_amt)) / 2.0;
    double y_origin = ((double)(chunk_amt * tile_amt)) / 2.0;
public:
    Environment(){};
    std::tuple<Vector2d, Vector2d> toChunkCoord(Vector2d pos);
};

#endif