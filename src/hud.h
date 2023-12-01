#ifndef HUD_H
#define HUD_H

#include "../include/SDL2/SDL.h"
#include "sprite.h"
#include "player.h"

struct hud {
    struct sprite players_head_sprite;
    struct sprite bomb_sprite;
    struct sprite game_over_sprite;
};

struct hud initHUD(SDL_Texture *texture, double DisplayScale);

void drawHUD(SDL_Renderer *renderer, struct hud* hud, struct player* playersList, int nbPlayers, double DisplayScale);

void drawGameOver(SDL_Renderer *renderer, struct hud* hud, double DisplayScale, int winWidth, int winHeight);
#endif