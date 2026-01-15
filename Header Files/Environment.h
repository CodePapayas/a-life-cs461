#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <tuple>

class Vector2d;
class Tile;
class Chunk;

class Environment
{
private:
    const int chunk_amt = 32;
    const int tile_amt = 3;


    Chunk *chunks; // initially a fully null array

    double x_origin = ((double)(chunk_amt * tile_amt)) / 2.0;
    double y_origin = ((double)(chunk_amt * tile_amt)) / 2.0;
public:
    std::tuple<Vector2d, Vector2d> toChunkCoord(Vector2d pos);
    int getChunkAmt();
    int getTileAmt();
};

#endif