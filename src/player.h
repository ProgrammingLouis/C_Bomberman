#ifndef PLAYER_H
#define PLAYER_H

#include "cells_grid_structs.h"
#include "structs.h"
#include "animated_sprite.h"

struct playerKeyboardInputs {
    Uint32 up;
    Uint32 down;
    Uint32 left;
    Uint32 right;
    Uint32 placeBomb;
};

struct player {
    int x;
    int y;
    int width;
    int height;
    int id;
    struct animated_sprite sprite;
    SDL_bool using_controller; // not used yet (player use controller if controller is connected)
    struct playerKeyboardInputs keyboardInputs;
    int nb_bombs_max;
    int nb_bombs;
    int bomb_range;
    Uint32 lastBombGiveTime;
    Uint32 bomb_cooldown;
    double speed;
    SDL_bool isDead;
};


enum playerAnimation {
    IDLE_DOWN,
    IDLE_RIGHT,
    IDLE_UP,
    WALK_RIGHT,
    WALK_DOWN,
    WALK_UP,
    PLACE_DOWN,
    PLACE_RIGHT,
    PLACE_UP,
    DEATH,
    WIN,
};

void drawPlayer(SDL_Renderer *renderer, struct player *player, SDL_Texture* players_texture, int playAreaTopLeftX, int playAreaTopLeftY);

struct obstacle getCollidedObstacle(struct player *player1, struct cellsGrid *cellsGridStruct);

void UpdatePlayerPos(double dt, struct player *player, struct cellsGrid *cellsGridStruct, int winWidth, int winHeight, struct controller *controllers);

// To change the player animation based on inputs
void UpdatePlayerAnimation(struct player *player, struct controller *controllers);

void UpdatePlayer(struct player *player, struct cellsGrid *cellsGridStruct, int winWidth, int winHeight, struct controller *controllers, double deltaTime, int *nbPlayersAlive);

// Called in main.c to init players
struct animated_sprite* CreatePlayersAnimatedSprites(SDL_Texture* players_texture, int nb_players, double DisplayScale);

#endif