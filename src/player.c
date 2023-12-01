#include "../include/SDL2/SDL.h"
#include "player.h"
#include "math.h"

void drawPlayer(SDL_Renderer *renderer, struct player *player, SDL_Texture* players_texture, int playAreaTopLeftX, int playAreaTopLeftY) {
    if (player->isDead) return;
    // double PlayerScale = 1.8;
    // SDL_Rect src = {0, 0+73*player->id, 16, 24};
    // SDL_Rect dest = {player->x, player->y, 16*PlayerScale, 24*PlayerScale};
    // SDL_RenderCopy(renderer, players_texture, &src, &dest);
    drawAnimatedSprite(renderer, &player->sprite, player->x+playAreaTopLeftX, player->y+playAreaTopLeftY);
}

// Return the obstacle that the player collsion box is colliding into
struct obstacle getCollidedObstacle(struct player *player1, struct cellsGrid *cellsGridStruct) {
    // Collision with obstacles
    for (int i = 0; i < cellsGridStruct->nbCellsRow; i++) {
        for (int j = 0; j < cellsGridStruct->nbCellsCollumn; j++) {
            cell cell = cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j];
            if(cell.type == UNDESTRUCTIBLE_OBSTACLE||cell.type == DESTRUCTIBLE_OBSTACLE){
                struct obstacle obstacle = cell.obstacle;
                // printf("obstacle %d: x=%d, y=%d, width=%d, height=%d", i, obstacles[i].x, obstacles[i].y, obstacles[i].width, obstacles[i].height);
                // If player rect overlaps with obstacle rect
                if (player1->x + player1->width > obstacle.x && player1->x < obstacle.x + obstacle.width) {
                    if (player1->y + player1->height > obstacle.y && player1->y < obstacle.y + obstacle.height) {
                        return obstacle;
                    }
                }
            }
        }
    }
    return (struct obstacle){-1, -1, -1, -1};
}


void UpdatePlayerPos(double dt, struct player *player, struct cellsGrid *cellsGridStruct, int winWidth, int winHeight, struct controller *controllers) {
    double player_speed = player->speed*dt;
    // Retrieve keyboard keys states
    const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);

    SDL_bool vertical_move = keyboard_states[player->keyboardInputs.up] || keyboard_states[player->keyboardInputs.down];
    SDL_bool horizontal_move = keyboard_states[player->keyboardInputs.left] || keyboard_states[player->keyboardInputs.right];


    if (controllers[player->id].connected) {
        // Retrieve controller left joystick axis values
        // this value is between -32768 and 32767
        // we divide by 32767 to get a value between -1 and 1
        double dx = SDL_GameControllerGetAxis(controllers[player->id].GameController, SDL_CONTROLLER_AXIS_LEFTX)/32767.;
        double dy = SDL_GameControllerGetAxis(controllers[player->id].GameController, SDL_CONTROLLER_AXIS_LEFTY)/32767.;

        // Deadzone
        double deadzone = 0.4;
        if (dx < deadzone && dx > -deadzone) dx = 0;
        if (dy < deadzone && dy > -deadzone) dy = 0;
        if (dx == 0 && dy == 0) return;


        player->x += dx*player_speed;
        struct obstacle collided_obstacle = getCollidedObstacle(player, cellsGridStruct);
        if (collided_obstacle.x != -1) {
            if (dx > 0) player->x = collided_obstacle.x - player->width;
            else player->x = collided_obstacle.x + collided_obstacle.width;
        }

        player->y += dy*player_speed;
        collided_obstacle = getCollidedObstacle(player, cellsGridStruct);
        if (collided_obstacle.x != -1) {
            if (dy > 0) player->y = collided_obstacle.y - player->height;
            else player->y = collided_obstacle.y + collided_obstacle.height;
        }
    
    // If no controller is connected, use keyboard inputs
    } else {
    struct playerKeyboardInputs playerInputs = player->keyboardInputs;

        // Up movement
        if (keyboard_states[playerInputs.up]) {
            if (horizontal_move) player->y -=player_speed/sqrt(2);
            else player->y -=player_speed;
            struct obstacle collided_obstacle = getCollidedObstacle(player, cellsGridStruct);
            // if collided with obstacle, move back to the obstacle side
            if (collided_obstacle.x != -1) {
                player->y = collided_obstacle.y + collided_obstacle.height;
            }
        }
        if (keyboard_states[playerInputs.left]) {
            if (vertical_move) player->x -=player_speed/sqrt(2);
            else player->x -=player_speed;
            struct obstacle collided_obstacle = getCollidedObstacle(player, cellsGridStruct);
            if (collided_obstacle.x != -1) {
                player->x = collided_obstacle.x + collided_obstacle.width;
            }
        }
        if (keyboard_states[playerInputs.down]) {
            if (horizontal_move) player->y +=player_speed/sqrt(2);
            else player->y +=player_speed;
            struct obstacle collided_obstacle = getCollidedObstacle(player, cellsGridStruct);
            if (collided_obstacle.x != -1) {
                player->y = collided_obstacle.y - player->height;
            }
        }
        if (keyboard_states[playerInputs.right]) {
            if (vertical_move) player->x +=player_speed/sqrt(2);
            else player->x +=player_speed;
            struct obstacle collided_obstacle = getCollidedObstacle(player, cellsGridStruct);
            if (collided_obstacle.x != -1) {
                player->x = collided_obstacle.x - player->width;
            }
        }
    }

    // Collision with window borders
    if (player->x < 0) player->x = 0;
    if (player->x + player->width > winWidth) player->x = winWidth - player->width;
    if (player->y < 0) player->y = 0;
    if (player->y + player->height > winHeight) player->y = winHeight - player->height;

}

// To change the player animation based on inputs
void UpdatePlayerAnimation(struct player *player, struct controller *controllers) {
    const Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
    struct playerKeyboardInputs playerInputs = player->keyboardInputs;

    if (controllers[player->id].connected) {
        // Player animations with controller
        // Retrieve controller left joystick axis values
        // this value is between -32768 and 32767
        // we divide by 32767 to get a value between -1 and 1
        double dx = SDL_GameControllerGetAxis(controllers[player->id].GameController, SDL_CONTROLLER_AXIS_LEFTX)/32767.;
        double dy = SDL_GameControllerGetAxis(controllers[player->id].GameController, SDL_CONTROLLER_AXIS_LEFTY)/32767.;
        SDL_bool max_dx = fabs(dx) > fabs(dy);

        // Deadzone
        double deadzone = 0.4;
        if (dx < deadzone && dx > -deadzone) dx = 0;
        if (dy < deadzone && dy > -deadzone) dy = 0;
        if (dx == 0 && dy == 0) {
            if (player->sprite.current_animation == WALK_DOWN) ChangeAnim(&player->sprite, IDLE_DOWN);
            else if (player->sprite.current_animation == WALK_UP) ChangeAnim(&player->sprite, IDLE_UP);
            else if (player->sprite.current_animation == WALK_RIGHT) ChangeAnim(&player->sprite, IDLE_RIGHT);
            return;
        }

        // Change animation based on the joystick values
        if (max_dx) {
            ChangeAnim(&player->sprite, WALK_RIGHT);
            player->sprite.is_flipped = dx < 0;
        } else {
            if (dy > 0) ChangeAnim(&player->sprite, WALK_DOWN);
            else ChangeAnim(&player->sprite, WALK_UP);
            player->sprite.is_flipped = SDL_FALSE;
        }

    // if no controller is connected, use keyboard inputs
    } else {
        if (player->sprite.animations[player->sprite.current_animation].one_time_anim) return;

        if (keyboard_states[playerInputs.right]) {
            ChangeAnim(&player->sprite, WALK_RIGHT);
            player->sprite.is_flipped = SDL_FALSE;
        } else if (keyboard_states[playerInputs.left]) {
            ChangeAnim(&player->sprite, WALK_RIGHT);
            player->sprite.is_flipped = SDL_TRUE;
        } else if (keyboard_states[playerInputs.down]) {
            ChangeAnim(&player->sprite, WALK_DOWN);
            player->sprite.is_flipped = SDL_FALSE;
        } else if (keyboard_states[playerInputs.up]) {
            ChangeAnim(&player->sprite, WALK_UP);
            player->sprite.is_flipped = SDL_FALSE;
        } else {
            // If no key is pressed, change animation to idle
            if (player->sprite.current_animation == WALK_DOWN) ChangeAnim(&player->sprite, IDLE_DOWN);
            else if (player->sprite.current_animation == WALK_UP) ChangeAnim(&player->sprite, IDLE_UP);
            else if (player->sprite.current_animation == WALK_RIGHT) ChangeAnim(&player->sprite, IDLE_RIGHT);
        }
    }
}

// Main function to update the player
void UpdatePlayer(struct player *player, struct cellsGrid *cellsGridStruct, int winWidth, int winHeight, struct controller *controllers, double deltaTime, int *nbPplayersAlive) {
    if (player->isDead) return;
    UpdatePlayerPos(deltaTime, player, cellsGridStruct, winWidth, winHeight, controllers);
    UpdateAnimatedSprite(&player->sprite);
    UpdatePlayerAnimation(player, controllers);
    // give bomb
    if (player->nb_bombs < player->nb_bombs_max && SDL_GetTicks() - player->lastBombGiveTime > player->bomb_cooldown) {
        player->nb_bombs++;
        player->lastBombGiveTime = SDL_GetTicks();
        printf("player %d has %d bombs\n", player->id, player->nb_bombs);
    }

    // check if powerup is picked up
    int playerCenterX = player->x + player->width/2;
    int playerCenterY = player->y + player->height/2;
    int i = playerCenterY/cellsGridStruct->sellSize;
    int j = playerCenterX/cellsGridStruct->sellSize;
    if (cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type == POWERUP) {
        Uint32 powerup_type = cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].powerup.type;
        if (powerup_type == 0) {
            player->bomb_range++;
            cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type = EMPTY;
        } else {
            player->speed *= 1.1;
            cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type = EMPTY;
        }


        // player->bomb_range++;
        // cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type = EMPTY;
    }

    // player is dead when DEATH animation is finished
    if (player->sprite.current_animation == DEATH && player->sprite.current_frame == player->sprite.animations[player->sprite.current_animation].nb_frames - 1) {
        player->isDead = SDL_TRUE;
        (*nbPplayersAlive)--;
    }
}

// Called in main.c to init players
struct animated_sprite* CreatePlayersAnimatedSprites(SDL_Texture* players_texture, int nb_players, double DisplayScale) {
    int nb_animations = 11;

    //#region Player1 animations
    struct sprite_animation player1_idle_DOWN_anim = {
        .first_frame_x = 0,
        .first_frame_y = 0,
        .nb_frames = 1,
        .frame_dx = 0,
        .frame_dy = 0,
        .frame_duration = 0,
        .one_time_anim = SDL_FALSE,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_idle_RIGHT_anim = {
        .first_frame_x = 0,
        .first_frame_y = 24,
        .nb_frames = 1,
        .frame_dx = 0,
        .frame_dy = 0,
        .frame_duration = 0,
        .one_time_anim = SDL_FALSE,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_idle_UP_anim = {
        .first_frame_x = 0,
        .first_frame_y = 48,
        .nb_frames = 1,
        .frame_dx = 0,
        .frame_dy = 0,
        .frame_duration = 0,
        .one_time_anim = SDL_FALSE,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    }; 
    struct sprite_animation player1_walk_RIGHT_anim = {
        .first_frame_x = 17,
        .first_frame_y = 24,
        .nb_frames = 6,
        .frame_dx = 16,
        .frame_dy = 0,
        .frame_duration = 100,
        .one_time_anim = SDL_FALSE,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_walk_DOWN_anim = {
        .first_frame_x = 17,
        .first_frame_y = 0,
        .nb_frames = 6,
        .frame_dx = 16,
        .frame_dy = 0,
        .frame_duration = 80,
        .one_time_anim = SDL_FALSE,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_walk_UP_anim = {
        .first_frame_x = 17,
        .first_frame_y = 48,
        .nb_frames = 6,
        .frame_dx = 16,
        .frame_dy = 0,
        .frame_duration = 80,
        .one_time_anim = SDL_FALSE,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_place_DOWN_anim = {
        .first_frame_x = 114,
        .first_frame_y = 0,
        .nb_frames = 1,
        .frame_dx = 0,
        .frame_dy = 0,
        .frame_duration = 200,
        .one_time_anim = SDL_TRUE,
        .next_anim = IDLE_DOWN,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_place_RIGHT_anim = {
        .first_frame_x = 114,
        .first_frame_y = 24,
        .nb_frames = 1,
        .frame_dx = 0,
        .frame_dy = 0,
        .frame_duration = 200,
        .one_time_anim = SDL_TRUE,
        .next_anim = IDLE_RIGHT,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_place_UP_anim = {
        .first_frame_x = 114,
        .first_frame_y = 48,
        .nb_frames = 1,
        .frame_dx = 0,
        .frame_dy = 0,
        .frame_duration = 200,
        .one_time_anim = SDL_TRUE,
        .next_anim = IDLE_UP,
        .custom_size = SDL_FALSE,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_DEATH_anim = {
        .first_frame_x = 131,
        .first_frame_y = 0,
        .nb_frames = 5,
        .frame_dx = 20,
        .frame_dy = 0,
        .frame_duration = 100,
        .one_time_anim = SDL_TRUE,
        .next_anim = IDLE_UP,
        .custom_size = SDL_TRUE,
        .custom_width = 20,
        .custom_height = 24,
        .both_way_anim = SDL_FALSE,
    };
    struct sprite_animation player1_WIN_anim = {
        .first_frame_x = 131,
        .first_frame_y = 24,
        .nb_frames = 4,
        .frame_dx = 16,
        .frame_dy = 0,
        .frame_duration = 130,
        .one_time_anim = SDL_FALSE,
        .next_anim = IDLE_UP,
        .custom_size = SDL_TRUE,
        .custom_width = 16,
        .custom_height = 37,
        .both_way_anim = SDL_TRUE,
    };
    // struct sprite_animation player1_WIN_anim = default_sprite_animation;
    // player1_WIN_anim.first_frame_x = 131;
    // player1_WIN_anim.first_frame_y = 24;
    // player1_WIN_anim.nb_frames = 4;
    // player1_WIN_anim.frame_dx = 16;
    // player1_WIN_anim.frame_duration = 200;
    // player1_WIN_anim.custom_size = SDL_TRUE;
    // player1_WIN_anim.custom_width = 16;
    // player1_WIN_anim.custom_height = 37;
    // player1_WIN_anim.both_way_anim = SDL_TRUE;
    //#endregion

    struct sprite_animation* player1_animations = malloc(sizeof(struct sprite_animation)*nb_animations);
    //#region Player1 animations list
    player1_animations[IDLE_DOWN] = player1_idle_DOWN_anim;
    player1_animations[IDLE_RIGHT] = player1_idle_RIGHT_anim;
    player1_animations[IDLE_UP] = player1_idle_UP_anim;
    player1_animations[WALK_RIGHT] = player1_walk_RIGHT_anim;
    player1_animations[WALK_UP] = player1_walk_UP_anim;
    player1_animations[WALK_DOWN] = player1_walk_DOWN_anim;
    player1_animations[PLACE_DOWN] = player1_place_DOWN_anim;
    player1_animations[PLACE_RIGHT] = player1_place_RIGHT_anim;
    player1_animations[PLACE_UP] = player1_place_UP_anim;
    player1_animations[DEATH] = player1_DEATH_anim;
    player1_animations[WIN] = player1_WIN_anim;
    //#endregion

    struct animated_sprite player1_animated_sprite = {
        .texture = players_texture,
        .nb_animations = nb_animations,
        .current_animation = IDLE_DOWN,
        .animations = player1_animations,
        .is_flipped = SDL_FALSE,
        .width = 16,
        .height = 24,
        .rescale = 1.8*DisplayScale,
        .current_frame = 0,
        .nb_skins = 8,
        .current_skin = 0,
        .skin_dx = 0,
        .skin_dy = 73,
    };

    struct animated_sprite* players_animated_sprites = malloc(sizeof(struct animated_sprite)*nb_players);
    for (int i = 0; i < nb_players; i++)
    {
        // players_animated_sprites[i] = i%2 == 0 ? player1_animated_sprite : player2_animated_sprite;
        players_animated_sprites[i] = player1_animated_sprite;
        players_animated_sprites[i].current_skin = i;
    }



    return players_animated_sprites;
}