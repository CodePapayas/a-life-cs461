/*
 *  Dillon Stickler - Oregon State University - 2026
 */

#pragma once

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <tuple>
#include <iostream>
#include <vector>       // added vector dep to change up arrays
#include <unordered_map>

// placeholder classes for functionality, may be extrapolated into their own files later
class Vector2d;
class Tile;
class Chunk;

class Environment
{
private:
    int tile_amt = 9;
    std::vector<std::vector<Tile*>> tiles;    
    std::unordered_map <int, Vector2d> tile_map; // get the X,Y for it for simplicity
public:
    Environment();
    Vector2d boundCoords(Vector2d pos);
    std::vector<double> getTileValues(Vector2d pos);
    void setTileAmount(int a) {tile_amt = a;};
    int getTileAmountSquared() {return tile_amt * tile_amt;};
    Vector2d getTileFromID(int id);
};

#endif