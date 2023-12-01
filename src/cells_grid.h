#ifndef CELLS_GRID_H
#define CELLS_GRID_H

#include "../include/SDL2/SDL.h"
#include "cells_grid_structs.h"
#include "bomb.h"
#include "sprite.h"

// Place the obstacles in the cells grid
void SellsGridInit(struct cellsGrid* cellsGridStruct, int *playersSpawnPositions, int nbPlayers);

void drawCellGrid(SDL_Renderer *renderer, struct cellsGrid* cellsGridStruct, int playAreaTopLeftX, int playAreaTopLeftY, struct sprite* powerupsSprite);

// Update the bombs and explosions in the cellsGrid
void updateCellsGrid(struct cellsGrid* cellsGridStruct, struct bomb_animation_utils *bombAnimationUtils, struct player *playersList, int nbPlayers);

// Debug function to print the cells grid types
void printCellsGridTypes(struct cellsGrid* cellsGridStruct);

#endif