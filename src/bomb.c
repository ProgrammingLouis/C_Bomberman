#include "bomb.h"
#include "animated_sprite.h"

struct sprite_animation createBombAnimation() {
    struct sprite_animation bomb_animation = {
        .first_frame_x = 0,
        .first_frame_y = 0,
        .nb_frames = 4,
        .frame_dx = 16,
        .frame_dy = 0,
        .frame_duration = 100,
        .one_time_anim = SDL_FALSE,
        .next_anim = 0,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };

    return bomb_animation;
};

struct sprite_animation createExplosionAnimation() {
    struct sprite_animation explosion_animation = {
        .first_frame_x = 0,
        .first_frame_y = 0,
        .nb_frames = 8,
        .frame_dx = 16,
        .frame_dy = 0,
        .frame_duration = 50,
        .one_time_anim = SDL_TRUE,
        .next_anim = 0,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };

    return explosion_animation;
};



// place a bomb in the cell where the player is
void PlaceBomb(struct player *player, struct cellsGrid* cellsGridStruct, struct bomb_animation_utils *bomb_animation_utils) {
    if (player->isDead) return;
    if (player->nb_bombs <= 0) return;
    player->nb_bombs--;
    // printf("player %d has %d bombs\n", player->id, player->nb_bombs);

    // when a bomb is placed, the new bomb cooldown is restarted
    player->lastBombGiveTime = SDL_GetTicks();

    int playerCenterX = player->x + player->width/2;
    int playerCenterY = player->y + player->height/2;
    int i = playerCenterY/cellsGridStruct->sellSize;
    int j = playerCenterX/cellsGridStruct->sellSize;
    int cellIndex = i*cellsGridStruct->nbCellsCollumn + j;
    cellsGridStruct->cells[cellIndex].type = BOMB;
    cellsGridStruct->cells[cellIndex].bomb.PlacedTime = SDL_GetTicks();
    cellsGridStruct->cells[cellIndex].bomb.range = player->bomb_range;
    cellsGridStruct->cells[cellIndex].bomb.owner = player;
    printf("bomb placed at %d %d\n", i, j);
    

    // set the bomb sprite
    struct animated_sprite bomb_animated_sprite = {
        .texture = bomb_animation_utils->bomb_texture,
        .nb_animations = 1,
        .current_animation = 0,
        .animations = &bomb_animation_utils->bomb_animation,
        .is_flipped = SDL_FALSE,
        .width = 16,
        .height = 16,
        .rescale = 2.*bomb_animation_utils->DisplayScale,
        .current_frame = 0,
        .nb_skins = 1,
        .current_skin = 0,
        .skin_dx = 0,
        .skin_dy = 0,
    };
    cellsGridStruct->cells[cellIndex].bomb.sprite = bomb_animated_sprite;
}

// called in explodeBomb function
void placeExplosion(struct cellsGrid* cellsGridStruct, int i, int j, struct bomb_animation_utils *bomb_animation_utils, struct player* playersList, int nbPlayers, SDL_bool exploded_obstacle) {
    int cellIndex = i*cellsGridStruct->nbCellsCollumn + j;
    cellsGridStruct->cells[cellIndex].type = EXPLOSION;
    // printf("explosion placed at %d %d\n", i, j);
    

    // set the explosion sprite
    struct animated_sprite explosion_animated_sprite = {
        .texture = bomb_animation_utils->explosion_texture,
        .nb_animations = 1,
        .current_animation = 0,
        .animations = &bomb_animation_utils->explosion_animation,
        .is_flipped = SDL_FALSE,
        .width = 16,
        .height = 16,
        .rescale = 1.8*bomb_animation_utils->DisplayScale,
        .current_frame = 0,
        .nb_skins = 1,
        .current_skin = 0,
        .skin_dx = 0,
        .skin_dy = 0,
    };
    cellsGridStruct->cells[cellIndex].explosion.sprite = explosion_animated_sprite;
    cellsGridStruct->cells[cellIndex].explosion.PlacedTime = SDL_GetTicks();
    cellsGridStruct->cells[cellIndex].explosion.exploded_obstacle = exploded_obstacle;

    // check if there is a player in the cell
    for (int k = 0; k < nbPlayers; k++) {
        struct player player = playersList[k];
        int playerExplodeX = player.x + player.width*0.5;
        int playerExplodeY = player.y + player.height*0.7;
        // printf("player %d : %d %d %d %d %d %d\n", k, j*cellsGridStruct->sellSize, playerExplodeX, (j+1)*cellsGridStruct->sellSize, i*cellsGridStruct->sellSize, playerExplodeY, (i+1)*cellsGridStruct->sellSize);
        if (playerExplodeX > j*cellsGridStruct->sellSize && playerExplodeX < (j+1)*cellsGridStruct->sellSize && playerExplodeY > i*cellsGridStruct->sellSize && playerExplodeY < (i+1)*cellsGridStruct->sellSize) {
            // playersList[k].is_dead = SDL_TRUE;
            printf("player %d is dead\n", k);
            playersList[k].sprite.current_animation = DEATH;
        }
    }
    // printf("\n");
};

// called in updateCellsGrid function in cellsGrid.c
void explodeBomb(struct cellsGrid* cellsGridStruct, int i, int j, struct bomb_animation_utils *bomb_animation_utils, struct player* playersList, int nbPlayers) {
    int cellIndex = i*cellsGridStruct->nbCellsCollumn + j;
    cellsGridStruct->cells[cellIndex].type = EXPLOSION;

    // explode in all directions
    // explosion up
    for (int k = 1; k <= cellsGridStruct->cells[cellIndex].bomb.range; k++) {
        if (i-k >= 0) {
            int cellIndex = (i-k)*cellsGridStruct->nbCellsCollumn + j;
            if (cellsGridStruct->cells[cellIndex].type == EMPTY) {
                placeExplosion(cellsGridStruct, i-k, j, bomb_animation_utils, playersList, nbPlayers, SDL_FALSE);
                // printf("explosion empty at %d %d\n", i-k, j);
            } else if (cellsGridStruct->cells[cellIndex].type == DESTRUCTIBLE_OBSTACLE) {
                cellsGridStruct->cells[cellIndex].type = EMPTY;
                placeExplosion(cellsGridStruct, i-k, j, bomb_animation_utils, playersList, nbPlayers, SDL_TRUE);
                // printf("explosion destructible at %d %d\n", i-k, j);
                break;
            } else if (cellsGridStruct->cells[cellIndex].type == UNDESTRUCTIBLE_OBSTACLE) {
                // printf("explosion undestructible at %d %d\n", i-k, j);
                break;
            }
        }
    }

    // explosion down
    for (int k = 1; k <= cellsGridStruct->cells[cellIndex].bomb.range; k++) {
        if (i+k < cellsGridStruct->nbCellsRow) {
            int cellIndex = (i+k)*cellsGridStruct->nbCellsCollumn + j;
            if (cellsGridStruct->cells[cellIndex].type == EMPTY) {
                placeExplosion(cellsGridStruct, i+k, j, bomb_animation_utils, playersList, nbPlayers, SDL_FALSE);
                // printf("explosion empty at %d %d\n", i+k, j);
            } else if (cellsGridStruct->cells[cellIndex].type == DESTRUCTIBLE_OBSTACLE) {
                cellsGridStruct->cells[cellIndex].type = EMPTY;
                placeExplosion(cellsGridStruct, i+k, j, bomb_animation_utils, playersList, nbPlayers, SDL_TRUE);
                // printf("explosion destructible at %d %d\n", i+k, j);
                break;
            } else if (cellsGridStruct->cells[cellIndex].type == UNDESTRUCTIBLE_OBSTACLE) {
                // printf("explosion undestructible at %d %d\n", i+k, j);
                break;
            }
        }
    }

    // explosion left
    for (int k = 1; k <= cellsGridStruct->cells[cellIndex].bomb.range; k++) {
        if (j-k >= 0) {
            int cellIndex = i*cellsGridStruct->nbCellsCollumn + j-k;
            if (cellsGridStruct->cells[cellIndex].type == EMPTY) {
                placeExplosion(cellsGridStruct, i, j-k, bomb_animation_utils, playersList, nbPlayers, SDL_FALSE);
                // printf("explosion empty at %d %d\n", i, j-k);
            } else if (cellsGridStruct->cells[cellIndex].type == DESTRUCTIBLE_OBSTACLE) {
                cellsGridStruct->cells[cellIndex].type = EMPTY;
                placeExplosion(cellsGridStruct, i, j-k, bomb_animation_utils, playersList, nbPlayers, SDL_TRUE);
                // printf("explosion destructible at %d %d\n", i, j-k);
                break;
            } else if (cellsGridStruct->cells[cellIndex].type == UNDESTRUCTIBLE_OBSTACLE) {
                // printf("explosion undestructible at %d %d\n", i, j-k);
                break;
            }
        }
    }

    // explosion right
    for (int k = 1; k <= cellsGridStruct->cells[cellIndex].bomb.range; k++) {
        if (j+k < cellsGridStruct->nbCellsCollumn) {
            int cellIndex = i*cellsGridStruct->nbCellsCollumn + j+k;
            if (cellsGridStruct->cells[cellIndex].type == EMPTY) {
                placeExplosion(cellsGridStruct, i, j+k, bomb_animation_utils, playersList, nbPlayers, SDL_FALSE);
                // printf("explosion empty at %d %d\n", i, j+k);
            } else if (cellsGridStruct->cells[cellIndex].type == DESTRUCTIBLE_OBSTACLE) {
                cellsGridStruct->cells[cellIndex].type = EMPTY;
                placeExplosion(cellsGridStruct, i, j+k, bomb_animation_utils, playersList, nbPlayers, SDL_TRUE);
                // printf("explosion destructible at %d %d\n", i, j+k);
                break;
            } else if (cellsGridStruct->cells[cellIndex].type == UNDESTRUCTIBLE_OBSTACLE) {
                // printf("explosion undestructible at %d %d\n", i, j+k);
                break;
            }
        }
    }
    placeExplosion(cellsGridStruct, i, j, bomb_animation_utils, playersList, nbPlayers, SDL_FALSE);
}
