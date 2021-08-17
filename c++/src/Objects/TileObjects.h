#ifndef TILE_OBJECTS_H
#define TILE_OBJECTS_H

#include "Tile.h"
#include "../ID/Tiles.h"
#include "../ID/Items.h"

// @param Type - the type whose class is being declared
// NEW_TILE(Type)
// - make Tile a friend of Type
// - declares a constructor Type()
// - implements a special id for the type

class Air : public Tile { NEW_TILE(Air) };
class Dirt : public Tile { NEW_TILE(Dirt) };
class Stone : public Tile { NEW_TILE(Stone) };

#endif