#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include "../include/SDL2/SDL.h"

struct sprite_animation {
    int first_frame_x; // x position of the first frame in the texture
    int first_frame_y; // y position of the first frame in the texture
    int nb_frames; // number of frames in the animation
    int frame_dx; // x distance between two frames in the texture
    int frame_dy; // y distance between two frames in the texture
    int frame_duration; // duration of each frame in ms
    SDL_bool one_time_anim; // if set to true, this anim will only play once, then change to next_anim
                            // ! a one_time_anim blocks anim changes
                            // (by default if set to false, all anims play indefinitly)
    Uint32 next_anim; // anim to play after this one one finished, used only if one_time_anim is true
    SDL_bool custom_size; // if set to true, the animation src rect will be custom_width*custom_height
    int custom_width; // custom width of the src rect
    int custom_height; // custom height of the src rect
    SDL_bool both_way_anim; // if set to true, the animation will play in reverse after reaching the last frame
};

struct animated_sprite {
    SDL_Texture* texture; // texture containing the sprite sheet, void all the frames
    struct sprite_animation* animations; // array of animations
    int nb_animations; // number of animations
    int current_animation; // index of the current animation
    SDL_bool is_flipped; // is the sprite flipped horizontally
    int width;
    int height;
    double rescale; // rescale factor
    Uint32 last_frame_time; // time of the last frame change
    int current_frame; // index of the current frame in the current animation
    int nb_skins; // number of skins
    int current_skin; // index of the current skin
    int skin_dx; // x offset between two skins in the texture
    int skin_dy; // y offset between two skins in the texture
    SDL_bool is_reserved; // used in both ways anims to know if the animation is playing in reverse
};

// draw the current frame of the current animation of the animated sprite at the given position
// (Called at each draw loop)
void drawAnimatedSprite(SDL_Renderer *renderer, struct animated_sprite *animated_sprite, int x, int y);

// update the current frame of the current animation of the animated sprite
// this function hudle frames changes
void UpdateAnimatedSprite(struct animated_sprite *animated_sprite);

void ChangeAnim(struct animated_sprite *animated_sprite, int change_anim);

// free the memory allocated for the animated sprite
// the animation array and the texture are freed
void DestroyAnimatedSprite(struct animated_sprite *animated_sprite);

#endif