#ifndef CELLS_GRID_STRUCTS_H
#define CELLS_GRID_STRUCTS_H

#include "../include/SDL2/SDL.h"
#include "animated_sprite.h"


struct bomb {
    Uint32 PlacedTime;
    int range;
    struct animated_sprite sprite;
    struct player* owner;
};

struct obstacle {
    int x;
    int y;
    int width;
    int height;
};

struct explosion {
    Uint32 PlacedTime;
    struct animated_sprite sprite;
    SDL_bool finished;
    SDL_bool exploded_obstacle;
};

struct powerup {
    Uint32 type;
};

typedef enum SELL_TYPE_ENUM {
    EMPTY,
    DESTRUCTIBLE_OBSTACLE,
    UNDESTRUCTIBLE_OBSTACLE,
    BOMB,
    EXPLOSION,
    POWERUP,
} SELL_TYPE_ENUM;

typedef struct cell {
    Uint32 type;
    union {
        struct obstacle obstacle;
        struct bomb bomb;
        struct explosion explosion;
        struct powerup powerup;
    };
} cell;

struct cellsGrid {
    cell *cells;
    int nbCellsRow;
    int nbCellsCollumn;
    int sellSize;
};

#endif