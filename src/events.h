#ifndef EVENTS_H
#define EVENTS_H

#include "SDL2/SDL.h"
#include "structs.h"
#include "cells_grid_structs.h"
#include "player.h"
#include "bomb.h"

// handle quit event, place bomb input event and controller added/removed event
void HandleEvents(SDL_bool *quit, struct controller *controllers, struct player *playersList, int nb_players, int maxControllers, struct cellsGrid *cellsGridStruct, struct bomb_animation_utils *bomb_animation_utils,  SDL_bool GameOver);
#endif