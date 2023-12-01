#include "../include/SDL2/SDL.h"
#include <stdio.h>
#include <stdlib.h>

// Local personal includes
#include "structs.h"
#include "events.h"
#include "cells_grid.h"
#include "cells_grid_structs.h"
#include "player.h"
#include "bomb.h"
#include "animated_sprite.h"
#include "datafile.h"
#include "hud.h"
#include "powerups.h"


// Draw the background green terrain grid
void drawTerrain(SDL_Renderer *renderer, int CellSize, int nbCellsRow, int nbCellsCollumn, int playAreaTopLeftX, int playAreaTopLeftY) {
    for (int i = 0; i < nbCellsRow; i++) {
        for (int j = 0; j < nbCellsCollumn; j++) {
            if ((i+j%2)%2 == 0) SDL_SetRenderDrawColor(renderer, 29, 100, 13, 255);
            else SDL_SetRenderDrawColor(renderer, 90, 126, 50, 255);
            SDL_Rect rect = {j*CellSize+playAreaTopLeftX, i*CellSize+playAreaTopLeftY, CellSize, CellSize};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

// Main draw function
void draw(SDL_Renderer *renderer, struct player *playersList, int playAreaTopLeftX, int playAreaTopLeftY, struct cellsGrid* cellsGridStruct, SDL_Texture* players_texture, int nb_players, struct hud* hud, double DisplayScale, SDL_bool GameOver, int winWidth, int winHeight, struct sprite* powerup_sprites) {
    // Clear the screen
    SDL_Color backgroundColor = {41, 68, 74, 255};
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

    drawTerrain(renderer, cellsGridStruct->sellSize, cellsGridStruct->nbCellsRow, cellsGridStruct->nbCellsCollumn, playAreaTopLeftX, playAreaTopLeftY);
    drawCellGrid(renderer, cellsGridStruct, playAreaTopLeftX, playAreaTopLeftY, powerup_sprites);
    if (!GameOver) drawHUD(renderer, hud, playersList, nb_players, DisplayScale);

    // Draw the players
    for (int i = 0; i < nb_players; i++) {
        drawPlayer(renderer, &playersList[i], players_texture, playAreaTopLeftX, playAreaTopLeftY);
    }

    // Draw game over
    if (GameOver) {
        drawGameOver(renderer, hud, DisplayScale, winWidth, winHeight);
    }

    // Update the screen
    SDL_RenderPresent(renderer);
}


SDL_Texture* loadImage(const char path[], SDL_Renderer *renderer) {
    SDL_Surface* tmp = NULL; 
    SDL_Texture* texture = NULL;
    tmp = SDL_LoadBMP(path);
    if(NULL == tmp)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
        return NULL;
    }
    texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
        return NULL;
    }
    return texture;
}



int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    int status = EXIT_FAILURE;

    //#region Game variables
    int MaxFps = 30;
    int maxControllers = 8;

    double DisplayScale = 0.9;

    int nbCellsRow = 9;
    int nbCellsCollumn = 11;
    int cellSize = 60*DisplayScale;
    int playAreaTopLeftX = 70*DisplayScale;
    int playAreaTopLeftY = 70*DisplayScale;
    int winWidth = nbCellsCollumn*cellSize+playAreaTopLeftX*2;
    int winHeight = nbCellsRow*cellSize+playAreaTopLeftY*2;
    SDL_bool GameOver = SDL_FALSE;
    // Create cells matrix
    cell cellsGrid[nbCellsRow*nbCellsCollumn];
    //#endregion

    // iniatilize random seed
    srand(time(NULL));

    // SDL Init
    if(0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER))
    {
        fprintf(stderr, "Erreur SDL_Init : %s", SDL_GetError());
        goto Quit;
    }
    // Display SDL version
    SDL_version nb;
    SDL_VERSION(&nb);
    printf("SDL version %d.%d.%d \n", nb.major, nb.minor, nb.patch);

    //#region Create window and renderer
    window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              winWidth, winHeight, SDL_WINDOW_SHOWN);
    if(NULL == window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
        goto Quit;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(NULL == renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        goto Quit;
    }
    //#endregion
    

    //#region Load textures & animations
    SDL_Texture* players_texture = loadImage("assets/players_sprites.bmp", renderer);
    // if (players_texture == NULL) {
    //     goto Quit;
    // }
    SDL_Texture* bomb_texture = loadImage("assets/bomb_animation.bmp", renderer);
    SDL_Texture* explosion_texture = loadImage("assets/explosion.bmp", renderer);
    SDL_Texture* hud_texture = loadImage("assets/HUD.bmp", renderer);
    SDL_Surface* logo_surf = SDL_LoadBMP("assets/logo.bmp");
    SDL_Texture* powerups_texture = loadImage("assets/powerups.bmp", renderer);

    struct sprite_animation bomb_animation = createBombAnimation();
    struct sprite_animation explosion_animation = createExplosionAnimation();
    struct bomb_animation_utils bomb_animation_utils = {bomb_animation, bomb_texture, explosion_animation, explosion_texture, DisplayScale};
    struct sprite* powerup_sprites = createPowerUpSprites(renderer, DisplayScale, powerups_texture);
    
    //#endregion

    SDL_SetWindowIcon(window, logo_surf);

    //#region Load data file
    FILE* dataFileRead = fopen("data.txt", "r");
    if (dataFileRead == NULL) {
        printf("\"data.txt\" file not found, creating a default one\n");
        createDefaultDataFile(); //TODO hundle file creation error
        dataFileRead = fopen("data.txt", "r");
        if (dataFileRead == NULL) {
            printf("Error opening file!\n");
            goto Quit;
        }
    }
    //#endregion

    // Read data file
    struct save_data saveData = loadSaveData(dataFileRead);
    fclose(dataFileRead);
    printf("Loaded data file\n");

    int nb_players = saveData.nbPlayers;
    int nbPlayersAlive = nb_players;

    int playersSpawnPositions[16] = {0, 0, nbCellsRow-1, nbCellsCollumn-1, 0, nbCellsCollumn-1, nbCellsRow-1, 0, nbCellsRow/2, 0, 0, nbCellsCollumn/2, nbCellsRow/2, nbCellsCollumn-1, nbCellsRow-1, nbCellsCollumn/2};
    //#region Init players

    struct playerKeyboardInputs player1KeyboardInputs = saveData.player1KeyboardInputs;
    struct playerKeyboardInputs player2KeyboardInputs = saveData.player2KeyboardInputs;

    struct animated_sprite* players_animated_sprites = CreatePlayersAnimatedSprites(players_texture, nb_players, DisplayScale);
    
    // TODO cleanly define players size

    struct player* playersList = malloc(sizeof(struct player)*nb_players);
    for (int playerId = 0; playerId < nb_players; playerId++) {
        struct player player = {
            .x = playersSpawnPositions[playerId*2+1]*cellSize,
            .y = playersSpawnPositions[playerId*2]*cellSize,
            .width = 16*1.8*DisplayScale,
            .height = 24*1.8*DisplayScale,
            .id = playerId,
            .sprite = players_animated_sprites[playerId],
            .using_controller = SDL_FALSE,
            .keyboardInputs = playerId%2==1 ? player1KeyboardInputs : player2KeyboardInputs,
            .nb_bombs_max = 3,
            .nb_bombs = 3,
            .bomb_range = 1,
            .lastBombGiveTime = 0,
            .bomb_cooldown = 2000,
            .speed = 0.125*DisplayScale,
            .isDead = SDL_FALSE,
        };
        playersList[playerId] = player;
    }

    free(players_animated_sprites);

    //#endregion

    struct controller *controllers = malloc(sizeof(struct controller)*maxControllers);


    struct cellsGrid sellsGridStruct = {cellsGrid, nbCellsRow, nbCellsCollumn, cellSize};
    SellsGridInit(&sellsGridStruct, playersSpawnPositions, nb_players);
    // printCellsGridTypes(&sellsGridStruct);

    // Init HUD
    struct hud hud = initHUD(hud_texture, DisplayScale);


    // Main loop
    int frameDelay = 1000 / MaxFps;
    SDL_bool quit = SDL_FALSE;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    while (!quit) {
        // Calculate delta time, which is the time elapsed since the last frame in ms
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency());

        HandleEvents(&quit, controllers, playersList, nb_players, maxControllers, &sellsGridStruct, &bomb_animation_utils, GameOver);

        updateCellsGrid(&sellsGridStruct, &bomb_animation_utils, playersList, nb_players);

        if (!GameOver) {

            for (int i = 0; i < nb_players; i++) {
                // we change the window size here cause it needs the play area size and not the real window size for borders collisions
                UpdatePlayer(&playersList[i], &sellsGridStruct, winWidth-2*playAreaTopLeftX, winHeight-2*playAreaTopLeftX, controllers, deltaTime, &nbPlayersAlive);
            }
            if (nbPlayersAlive <= 1) {
                GameOver = SDL_TRUE;
                for (int i = 0; i < nb_players; i++) {
                    if (!playersList[i].isDead) {
                        playersList[i].sprite.current_animation = WIN;
                        break;
                    }
                }
            }
        } else {
            for (int i = 0; i < nb_players; i++) {
                if (!playersList[i].isDead) {
                    UpdateAnimatedSprite(&playersList[i].sprite);
                }
            }
        }

        draw(renderer, playersList, playAreaTopLeftX, playAreaTopLeftY, &sellsGridStruct, players_texture, nb_players, &hud, DisplayScale, GameOver, winWidth, winHeight, powerup_sprites);
        SDL_Delay(frameDelay);
        // printCellsGridTypes(&sellsGridStruct);
    }
    

    status = EXIT_SUCCESS;

    // Free memory and quit
Quit:
    //TODO if error, don't free not allocated memory

    // Close controllers
    for (int i = 0; i <maxControllers; i++)
    {
        if (!controllers[i].connected) SDL_GameControllerClose(controllers[i].GameController);
    }

    free(controllers);

    // Free players animated sprites
    if (NULL != playersList[0].sprite.animations) free(playersList[0].sprite.animations);

    free(playersList);

    // Free all textures
    SDL_DestroyTexture(players_texture);
    SDL_DestroyTexture(bomb_texture);
    SDL_DestroyTexture(explosion_texture);
    SDL_DestroyTexture(hud_texture);
    
    if(NULL != renderer)
        SDL_DestroyRenderer(renderer);
    if(NULL != window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    return status;
}