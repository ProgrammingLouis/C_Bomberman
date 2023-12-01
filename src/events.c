
#include "events.h"
#include "bomb.h"

// handle quit event, place bomb input event and controller added/removed event
void HandleEvents(SDL_bool *quit, struct controller *controllers, struct player *playersList, int nb_players, int maxControllers, struct cellsGrid *cellsGridStruct, struct bomb_animation_utils *bomb_animation_utils, SDL_bool GameOver) {
    SDL_Event event;
    // loop through all events in queue
    while (SDL_PollEvent(&event)) {
        // Handle quit event
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)) {
            *quit = SDL_TRUE;
            return;
        
        }
        if (GameOver) return;
        if (event.type == SDL_KEYDOWN) {
            // printf("key down : %d\n", event.key.keysym.scancode);

            
            for (int i=0; i<nb_players; ++i) {
                if (controllers[i].connected) continue;
                struct player* player = &playersList[i];
                if (event.key.keysym.scancode == player->keyboardInputs.placeBomb) {
                    PlaceBomb(player, cellsGridStruct, bomb_animation_utils);
                    // ChangeAnim(&player->sprite, PLACE_RIGHT);
                    if (player->sprite.current_animation == WALK_RIGHT || player->sprite.current_animation == IDLE_RIGHT) {
                        ChangeAnim(&player->sprite, PLACE_RIGHT);
                    }
                    else if (player->sprite.current_animation == WALK_UP || player->sprite.current_animation == IDLE_UP) {
                        ChangeAnim(&player->sprite, PLACE_UP);
                    }
                    else if (player->sprite.current_animation == WALK_DOWN || player->sprite.current_animation == IDLE_DOWN) {
                        ChangeAnim(&player->sprite, PLACE_DOWN);
                    }
                }
            }
        
        } else if (event.type == SDL_CONTROLLERDEVICEADDED) {
            int deviceID = (int)event.cdevice.which;
            if (deviceID >= maxControllers) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Too many controllers connected (max 8)");
                continue;
            }
            SDL_GameController *controller = SDL_GameControllerOpen(deviceID);
            struct controller new_controller = {deviceID, SDL_TRUE, controller};
            controllers[deviceID] = new_controller;
            printf("controller %d added\n", deviceID);

        } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
            int deviceID = (int)event.cdevice.which;
            //TODO close device
            SDL_GameControllerClose(controllers[deviceID].GameController);
            controllers[deviceID].connected = SDL_FALSE;
            printf("controller %d removed\n", deviceID);

        // } else if (event.type == SDL_CONTROLLERDEVICEREMAPPED) {
        //     printf("controller remapped\n");

        } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
            // printf("controller button down\n");
            int deviceID = (int)event.cbutton.which;
            if (!controllers[deviceID].connected) continue;
            if (deviceID >= 2) continue;
            Uint8 button = event.cbutton.button;
            if (button == SDL_CONTROLLER_BUTTON_A) {
                // printf("controller %d pressed A\n", deviceID);
                if (!controllers[deviceID].connected) continue;
                if (deviceID >= 2) continue;
                struct player* player = &playersList[deviceID];
                PlaceBomb(player, cellsGridStruct, bomb_animation_utils);

                // Animate player placing bomb
                if (player->sprite.current_animation == WALK_RIGHT || player->sprite.current_animation == IDLE_RIGHT) {
                        ChangeAnim(&player->sprite, PLACE_RIGHT);
                    }
                    else if (player->sprite.current_animation == WALK_UP || player->sprite.current_animation == IDLE_UP) {
                        ChangeAnim(&player->sprite, PLACE_UP);
                    }
                    else if (player->sprite.current_animation == WALK_DOWN || player->sprite.current_animation == IDLE_DOWN) {
                        ChangeAnim(&player->sprite, PLACE_DOWN);
                    }
            }

        } else if (event.type == SDL_CONTROLLERBUTTONUP) {
            // printf("controller button up\n");
        } else if (event.type == SDL_CONTROLLERAXISMOTION) {
            // SDL_ControllerAxisEvent axisEvent = event.caxis;
            // SDL_GameControllerAxis axis = axisEvent.axis;
            // if (axis == SDL_CONTROLLER_AXIS_INVALID) continue;
            // int deviceID = (int)axisEvent.which;
            // double value = axisEvent.value/32767.;
            // printf("controller %d axis %d value %f\n", deviceID, axis, value);
        }
    }
}