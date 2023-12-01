#include "powerups.h"

struct sprite* createPowerUpSprites(SDL_Renderer* renderer, double DisplayScale, SDL_Texture* powerups_texture) {
    struct sprite range_sprite = {
        .texture = powerups_texture,
        .default_x = 39,
        .default_y = 46,
        .width = 32,
        .height = 32,
        .rescale = 1.*DisplayScale,
        .nb_skins = 1,
        .current_skin = 0,
        .skin_dx = 0,
        .skin_dy = 0,
        .is_flipped = SDL_FALSE,
    };
    struct sprite speed_sprite = {
        .texture = powerups_texture,
        .default_x = 75,
        .default_y = 10,
        .width = 32,
        .height = 32,
        .rescale = 1.*DisplayScale,
        .nb_skins = 1,
        .current_skin = 0,
        .skin_dx = 0,
        .skin_dy = 0,
        .is_flipped = SDL_FALSE,
    };
    struct sprite* powerups_sprites = malloc(2*sizeof(struct sprite));
    powerups_sprites[0] = range_sprite;
    powerups_sprites[1] = speed_sprite;
    return powerups_sprites;
}