#include "animated_sprite.h"

// draw the current frame of the current animation of the animated sprite at the given position
// (Called at each draw loop)
void drawAnimatedSprite(SDL_Renderer *renderer, struct animated_sprite *animated_sprite, int x, int y) {
    struct sprite_animation anim = animated_sprite->animations[animated_sprite->current_animation];
    // int frame_dx = animated_sprite->is_reserved ? anim.frame_dx * (anim.nb_frames-1-animated_sprite->current_frame) : anim.frame_dx * animated_sprite->current_frame;
    int frame_dx = anim.frame_dx * animated_sprite->current_frame;
    int frame_dy = anim.frame_dy * animated_sprite->current_frame;
    // printf("%d %d\n", frame_dx, frame_dy);
    int frame_x = anim.first_frame_x+ frame_dx + animated_sprite->current_skin*animated_sprite->skin_dx;
    // int frame_x = animated_sprite->is_reserved ? anim.first_frame_x+anim.frame_dx*(anim.nb_frames;
    int frame_y = anim.first_frame_y + frame_dy + animated_sprite->current_skin*animated_sprite->skin_dy;
    int width = anim.custom_size ? anim.custom_width : animated_sprite->width;
    int height = anim.custom_size ? anim.custom_height : animated_sprite->height;
    SDL_Rect src = {frame_x, frame_y, width, height};
    SDL_Rect dst = {x, y, width*animated_sprite->rescale, height*animated_sprite->rescale};
    // printf("%d %d\n", anim.first_frame_x, anim.first_frame_y);
    const SDL_RendererFlip flip = animated_sprite->is_flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (SDL_RenderCopyEx(renderer, animated_sprite->texture, &src, &dst, 0, NULL, flip) != 0) {
        printf("Error while rendering animated sprite: %s\n", SDL_GetError());
    }
}

// update the current frame of the current animation of the animated sprite
// this function hudle frames changes
void UpdateAnimatedSprite(struct animated_sprite *animated_sprite) {
    struct sprite_animation anim = animated_sprite->animations[animated_sprite->current_animation];
    Uint32 current_time = SDL_GetTicks();
    if(current_time - animated_sprite->last_frame_time > anim.frame_duration) {
        // printf("next frame\n");
        if (anim.one_time_anim && animated_sprite->current_frame+1 >= anim.nb_frames) {
            // printf("a\n");
            animated_sprite->current_animation = anim.next_anim;
            animated_sprite->current_frame = 0;
            animated_sprite->last_frame_time = SDL_GetTicks();
        } else {
            if (anim.nb_frames <= 0) {
                animated_sprite->current_frame = 0;
                animated_sprite->last_frame_time = current_time;
                printf("Error: animation with <=0 frames\n");
                return;
            }
            // Change to next frame
            if (anim.both_way_anim) {
                if ((!animated_sprite->is_reserved && animated_sprite->current_frame+1 >= anim.nb_frames) || (animated_sprite->is_reserved && animated_sprite->current_frame <= 0)) {
                    animated_sprite->is_reserved = !animated_sprite->is_reserved;
                }
                if (animated_sprite->is_reserved) {
                    animated_sprite->current_frame--;
                } else {
                    animated_sprite->current_frame++;
                }
            } else animated_sprite->current_frame = (animated_sprite->current_frame + 1) % anim.nb_frames;
            animated_sprite->last_frame_time = current_time;
        }
    }
    // printf("updated\n");
}

void ChangeAnim(struct animated_sprite *animated_sprite, int change_anim) {
    struct sprite_animation anim = animated_sprite->animations[animated_sprite->current_animation];
    if (anim.one_time_anim) return;

    if (animated_sprite->current_animation == change_anim) return;

    animated_sprite->current_animation = change_anim;
    animated_sprite->current_frame = 0;
    animated_sprite->last_frame_time = SDL_GetTicks();
}

// free the memory allocated for the animated sprite
// the animation array and the texture are freed
void DestroyAnimatedSprite(struct animated_sprite *animated_sprite) {
    if (NULL != animated_sprite->animations) free(animated_sprite->animations);
    if(NULL != animated_sprite->texture)
        SDL_DestroyTexture(animated_sprite->texture);
}
