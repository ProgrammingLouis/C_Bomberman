#include "sprite.h"

// draw the current frame of the current animation of the animated sprite at the given position
// (Called at each draw loop)
void drawSprite(SDL_Renderer *renderer, struct sprite *sprite, int x, int y, int custom_width) {
    int frame_x = sprite->default_x+sprite->skin_dx*sprite->current_skin;
    int frame_y = sprite->default_y+sprite->skin_dy*sprite->current_skin;
    int width = sprite->width;
    int height = sprite->height;
    if (custom_width >= 0) {
        width = custom_width;
    }
    SDL_Rect src = {frame_x, frame_y, width, height};
    SDL_Rect dst = {x, y, width*sprite->rescale, height*sprite->rescale};
    // printf("%d %d\n", anim.first_frame_x, anim.first_frame_y);
    const SDL_RendererFlip flip = sprite->is_flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (SDL_RenderCopyEx(renderer, sprite->texture, &src, &dst, 0, NULL, flip) != 0) {
        printf("Error while rendering sprite: %s\n", SDL_GetError());
    }
}

// free the memory allocated for the sprite
void DestroySprite(struct sprite *sprite) {
    if(NULL != sprite->texture) SDL_DestroyTexture(sprite->texture);
}
