#ifndef SPRITE_H
#define SPRITE_H

#include "../include/SDL2/SDL.h"

struct sprite {
    SDL_Texture* texture; // texture containing the sprite sheet, void all the frames
    int default_x; // the x position of the pixel top left corner in the texture
    int default_y; // the y position of the pixel top left corner in the texture
    int width;
    int height;
    double rescale; // rescale factor
    int nb_skins;
    int current_skin;
    int skin_dx;
    int skin_dy;
    SDL_bool is_flipped;
};

// draw the current frame of the current animation of the animated sprite at the given position
// (Called at each draw loop)
void drawSprite(SDL_Renderer *renderer, struct sprite *sprite, int x, int y, int custom_width);

// free the memory allocated for the animated sprite
// the animation array and the texture are freed
void DestroySprite(struct sprite *sprite);

#endif