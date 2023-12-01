#ifndef POWERUPS_H
#define POWERUPS_H

#include "../include/SDL2/SDL.h"
#include "sprite.h"

struct sprite* createPowerUpSprites(SDL_Renderer* renderer, double DisplayScale, SDL_Texture* powerups_texture);

#endif