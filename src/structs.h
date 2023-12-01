#ifndef STRUCTS_H
#define STRUCTS_H

#include "../include/SDL2/SDL.h"
// #include "cells_grid_structs.h"
// #include "player.h"



struct controller {
    int id;
    SDL_bool connected;
    SDL_GameController *GameController;
};








#endif