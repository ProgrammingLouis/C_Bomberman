#ifndef BOMB_H
#define BOMB_H

#include "cells_grid_structs.h"
#include "player.h"

struct bomb_animation_utils {
    struct sprite_animation bomb_animation;
    SDL_Texture* bomb_texture;
    struct sprite_animation explosion_animation;
    SDL_Texture* explosion_texture;
    double DisplayScale;
};

struct sprite_animation createBombAnimation();
struct sprite_animation createExplosionAnimation();

// place a bomb in the cell where the player is
void PlaceBomb(struct player *player, struct cellsGrid* cellsGridStruct, struct bomb_animation_utils *bomb_animation_utils);

void placeExplosion(struct cellsGrid* cellsGridStruct, int i, int j, struct bomb_animation_utils *bomb_animation_utils, struct player* playersList, int nbPlayers, SDL_bool exploded_obstacle);

void explodeBomb(struct cellsGrid* cellsGridStruct, int i, int j, struct bomb_animation_utils *bomb_animation_utils, struct player* playersList, int nbPlayers);

#endif