#include "cells_grid.h"
#include "bomb.h"

// Place the obstacles in the cells grid
void SellsGridInit(struct cellsGrid* cellsGridStruct, int *playersSpawnPositions, int nbPlayers) {
    // printf("nbCellsRow = %d, nbCellsCollumn = %d\n", cellsGridStruct->nbCellsRow, cellsGridStruct->nbCellsCollumn);
    int cellsSize = cellsGridStruct->sellSize;
    int nbCellsRow = cellsGridStruct->nbCellsRow;
    int nbCellsCollumn = cellsGridStruct->nbCellsCollumn;

    for (int i = 0; i < nbCellsRow; i++) {
        for (int j = 0; j < nbCellsCollumn; j++) {
            int cellIndex = i*nbCellsCollumn+j;
            if((i%2==1)&&(j%2==1)) {
                cellsGridStruct->cells[cellIndex].type = UNDESTRUCTIBLE_OBSTACLE;
                cellsGridStruct->cells[cellIndex].obstacle = (struct obstacle) {j*cellsSize, i*cellsSize, cellsSize, cellsSize};
            } else if (!(i==0 && j==0)) {
                cellsGridStruct->cells[cellIndex].type = DESTRUCTIBLE_OBSTACLE;
                cellsGridStruct->cells[cellIndex].obstacle = (struct obstacle) {j*cellsSize, i*cellsSize, cellsSize, cellsSize};
            } else {
                cellsGridStruct->cells[cellIndex].type = EMPTY;
            }
        } 
    }

    // remove the obstacles around the spawn positions
    for (int i = 0; i<nbPlayers; i++) {
        int cellIndex = playersSpawnPositions[i*2]*nbCellsCollumn+playersSpawnPositions[i*2+1];
        cellsGridStruct->cells[cellIndex].type = EMPTY;
        if (cellIndex+1 < nbCellsRow*nbCellsCollumn && playersSpawnPositions[i*2+1]<nbCellsCollumn-1 && cellsGridStruct->cells[cellIndex+1].type == DESTRUCTIBLE_OBSTACLE) {
            cellsGridStruct->cells[cellIndex+1].type = EMPTY;
        }
        if (cellIndex-1 >= 0 && playersSpawnPositions[i*2+1]>0 && cellsGridStruct->cells[cellIndex-1].type == DESTRUCTIBLE_OBSTACLE) {
            cellsGridStruct->cells[cellIndex-1].type = EMPTY;
        }
        if (cellIndex+nbCellsCollumn < nbCellsRow*nbCellsCollumn && playersSpawnPositions[i*2]<nbCellsRow-1 && cellsGridStruct->cells[cellIndex+nbCellsCollumn].type == DESTRUCTIBLE_OBSTACLE) {
            cellsGridStruct->cells[cellIndex+nbCellsCollumn].type = EMPTY;
        }
        if (cellIndex-nbCellsCollumn >= 0 && playersSpawnPositions[i*2]>0 && cellsGridStruct->cells[cellIndex-nbCellsCollumn].type == DESTRUCTIBLE_OBSTACLE) {
            cellsGridStruct->cells[cellIndex-nbCellsCollumn].type = EMPTY;
        }
    }
}

void drawCellGrid(SDL_Renderer *renderer, struct cellsGrid* cellsGridStruct, int playAreaTopLeftX, int playAreaTopLeftY, struct sprite* powerupsSprites) {
    int CellSize = cellsGridStruct->sellSize;
    // int bombSize = CellSize/2;
     for (int i = 0; i < cellsGridStruct->nbCellsRow; i++) {
        for (int j = 0; j < cellsGridStruct->nbCellsCollumn; j++) {
            Uint32 cell_type = cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type;
            
            // if type is UNDESTRUCTIBLE_OBSTACLE, draw a gray rectangle
            if(cell_type == UNDESTRUCTIBLE_OBSTACLE){
                // printf("drawing obstacle at cell %d %d & screen pos %d% d\n", i, j, j*CellSize, i*CellSize);
                SDL_Rect rect = {j*CellSize+playAreaTopLeftX, i*CellSize+playAreaTopLeftY, CellSize, CellSize};
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
                SDL_RenderFillRect(renderer, &rect);
            
            // if type is BOMB, draw a purple little rectangle
            } else if (cell_type == BOMB) {
                // SDL_Rect rect = {j*CellSize + CellSize/4, i*CellSize + CellSize/4, CellSize/2, CellSize/2};
                // SDL_SetRenderDrawColor(renderer, 78, 23, 166, 255);
                // SDL_RenderFillRect(renderer, &rect);
                // printf("drawing bomb at cell %d %d & screen pos %d% d\n", i, j, j*CellSize, i*CellSize);
                drawAnimatedSprite(renderer, &cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].bomb.sprite, j*CellSize + CellSize/4 + playAreaTopLeftX, i*CellSize + CellSize/4 + playAreaTopLeftY);
                // printf("bomb drawn\n");
            
            // if type is UNDESTRUCTIBLE_OBSTACLE, draw a brown rectangle 
            } else if (cell_type == EXPLOSION) {
                drawAnimatedSprite(renderer, &cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].explosion.sprite, j*CellSize + CellSize/4 + playAreaTopLeftX, i*CellSize + CellSize/4 + playAreaTopLeftY);
            } else if (cell_type == DESTRUCTIBLE_OBSTACLE){
                SDL_Rect rect = {j*CellSize + playAreaTopLeftX, i*CellSize + playAreaTopLeftY, CellSize, CellSize};
                SDL_SetRenderDrawColor(renderer,  34, 16, 0, 255);
                SDL_RenderFillRect(renderer, &rect); 
            } else if (cell_type == POWERUP) {
                Uint32 powerupType = cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].powerup.type;
                drawSprite(renderer, &powerupsSprites[powerupType], j*CellSize + playAreaTopLeftX, i*CellSize + playAreaTopLeftY, -1);
            }
        }
    }
}

// Update the bombs and explosions in the cellsGrid
void updateCellsGrid(struct cellsGrid* cellsGridStruct, struct bomb_animation_utils *bombAnimationUtils, struct player *playersList, int nbPlayers) {
    for (int i = 0; i < cellsGridStruct->nbCellsRow; i++) {
        for (int j = 0; j < cellsGridStruct->nbCellsCollumn; j++) {
            Uint32 cell_type = cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type;
            if(cell_type == BOMB){
                UpdateAnimatedSprite(&cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].bomb.sprite);
                Uint32 bombPlacedTime = cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].bomb.PlacedTime;
                Uint32 currentTime = SDL_GetTicks();
                if(currentTime - bombPlacedTime > cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].bomb.owner->bomb_cooldown){
                    explodeBomb(cellsGridStruct, i, j, bombAnimationUtils, playersList, nbPlayers);
                    // cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type = EXPLOSION;
                }
            } else if (cell_type == EXPLOSION) {
                struct explosion *explosion = &cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].explosion;
                UpdateAnimatedSprite(&explosion->sprite);
                Uint32 explosionPlacedTime = explosion->PlacedTime;
                Uint32 currentTime = SDL_GetTicks();
                // explosion ends after the last frame of the animation
                if(currentTime - explosionPlacedTime > bombAnimationUtils->explosion_animation.nb_frames * bombAnimationUtils->explosion_animation.frame_duration){
                    cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type = EMPTY;
                    if (explosion->exploded_obstacle) {
                        // place a powerup with 1/4 chance
                        if (rand()%3 == 0) {
                            cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type = POWERUP;
                            cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].powerup.type = rand()%2;
                        }
                    }
                }
            }
        }
    }
}

// Debug function to print the cells grid types
void printCellsGridTypes(struct cellsGrid* cellsGridStruct) {
    printf("nbCellsRow = %d, nbCellsCollumn = %d\n", cellsGridStruct->nbCellsRow, cellsGridStruct->nbCellsCollumn);
    for (int i = 0; i < cellsGridStruct->nbCellsRow; i++) {
        for (int j = 0; j < cellsGridStruct->nbCellsCollumn; j++) {
            printf("cell type at %d %d is %d \n", i, j, cellsGridStruct->cells[i*cellsGridStruct->nbCellsCollumn+j].type);
        }
    }
}