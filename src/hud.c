#include "hud.h"

#define HUD_PLAYERS_HEADS_SPACING 100

struct hud initHUD(SDL_Texture *texture, double DisplayScale) {
    struct hud hud;
    struct sprite players_head_sprite = {
        .texture = texture,
        .default_x = 0,
        .default_y = 0,
        .width = 16,
        .height = 12,
        .rescale = 2.8*DisplayScale,
        .nb_skins = 8,
        .current_skin = 0,
        .skin_dx = 0,
        .skin_dy = 12,
        .is_flipped = SDL_FALSE
    };
    hud.players_head_sprite = players_head_sprite;

    struct sprite bomb_sprite = {
        .texture = texture,
        .default_x = 17,
        .default_y = 2,
        .width = 12,
        .height = 14,
        .rescale = 1.5*DisplayScale,
        .nb_skins = 1,
        .current_skin = 0,
        .skin_dx = 0,
        .skin_dy = 0,
        .is_flipped = SDL_FALSE
    };
    hud.bomb_sprite = bomb_sprite;

    struct sprite game_over_sprite = {
        .texture = texture,
        .default_x = 58,
        .default_y = 0,
        .width = 604,
        .height = 83,
        .rescale = 1.6*DisplayScale,
        .nb_skins = 1,
        .current_skin = 0,
        .skin_dx = 0,
        .skin_dy = 0,
        .is_flipped = SDL_FALSE
    };
    hud.game_over_sprite = game_over_sprite;

    return hud;
}

void drawHUD(SDL_Renderer *renderer, struct hud* hud, struct player* playersList, int nbPlayers, double DisplayScale) {
    // int heads_spacing = HUD_PLAYERS_HEADS_SPACING;
    for (int i = 0; i < nbPlayers; i++) {
        hud->players_head_sprite.current_skin = i;
        drawSprite(renderer, &hud->players_head_sprite, i*HUD_PLAYERS_HEADS_SPACING*DisplayScale, 0, -1);
    }

    // draw bomb sprite
    // indicating the number of bombs left for each player
    for (int i = 0; i < nbPlayers; i++) {
        if (playersList[i].isDead) continue;
        for (int j = 0; j < playersList[i].nb_bombs; j++) {
            int x = i*HUD_PLAYERS_HEADS_SPACING + j*(hud->bomb_sprite.width+8);
            int y = 30;
            drawSprite(renderer, &hud->bomb_sprite, x*DisplayScale, y*DisplayScale, -1);
        }

        // draw the last bomb charging
        if (playersList[i].nb_bombs < playersList[i].nb_bombs_max) {
            int lastBombGiveTime = playersList[i].lastBombGiveTime;
            int currentTime = SDL_GetTicks();
            int timeSinceLastBombGive = currentTime - lastBombGiveTime;
            int timeToCharge = playersList[i].bomb_cooldown;
            // printf("timeSinceLastBombGive: %d, timeToCharge: %d\n", timeSinceLastBombGive, timeToCharge);
            double pourcentCharged = (double)timeSinceLastBombGive/timeToCharge;
            int customWidth = hud->bomb_sprite.width*pourcentCharged;
            // printf("pourcentCharged: %f, customWidth: %d\n", pourcentCharged, customWidth);
            int x = i*HUD_PLAYERS_HEADS_SPACING + playersList[i].nb_bombs*(hud->bomb_sprite.width+8);
            int y = 30;
            drawSprite(renderer, &hud->bomb_sprite, x*DisplayScale, y*DisplayScale, customWidth);
        }
    }
}

// TODO verify postion when DisplayScale != 1
void drawGameOver(SDL_Renderer *renderer, struct hud* hud, double DisplayScale, int winWidth, int winHeight) {
    int x = winWidth/2-hud->game_over_sprite.width/2*hud->game_over_sprite.rescale;
    int y = 0.75*winHeight/2-hud->game_over_sprite.height/2*hud->game_over_sprite.rescale;
    drawSprite(renderer, &hud->game_over_sprite, x*DisplayScale, y*DisplayScale, -1);
}