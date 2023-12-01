#include <stdio.h>
#include <string.h>
#include "datafile.h"
#include "../include/SDL2/SDL.h"

// unused for now
void savePlayerKeyboardInputs(struct playerKeyboardInputs *inputs, FILE *file) {
    fprintf(file, "up: %d\n", inputs->up);
    fprintf(file, "down: %d\n", inputs->down);
    fprintf(file, "left: %d\n", inputs->left);
    fprintf(file, "right: %d\n", inputs->right);
    fprintf(file, "placeBomb: %d\n", inputs->placeBomb);
}

struct playerKeyboardInputs loadPlayerKeyboardInputs(FILE *file) {
    struct playerKeyboardInputs inputs;
    SDL_bool player_found = SDL_FALSE;
    char line[256];
    while (!feof(file) && !player_found) {
        fgets(line, 256, file);

        if (strstr(line, "#")) {
            continue;
        }
        // printf("line: %s\n", line);

        if (strstr(line, "up: ")) {
            sscanf(line, "up: %d", &inputs.up);
        }
        if (strstr(line, "down: ")) {
            sscanf(line, "down: %d", &inputs.down);
        }
        if (strstr(line, "left: ")) {
            sscanf(line, "left: %d", &inputs.left);
        }
        if (strstr(line, "right: ")) {
            sscanf(line, "right: %d", &inputs.right);
        }
        if (strstr(line, "placeBomb: ")) {
            sscanf(line, "placeBomb: %d", &inputs.placeBomb);
            player_found = SDL_TRUE;
        }
    }
    if (!player_found) {
        return (struct playerKeyboardInputs){SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN};
    }
    return inputs;
}



struct save_data loadSaveData(FILE *file) {
    struct save_data saveData;
    fscanf(file, "nb_players: %d\n", &saveData.nbPlayers);
    saveData.player1KeyboardInputs = loadPlayerKeyboardInputs(file);
    saveData.player2KeyboardInputs = loadPlayerKeyboardInputs(file);
    return saveData;
}

void createDefaultDataFile() {
    FILE *fileWrite = fopen("data.txt", "w");
    fprintf(fileWrite, "nb_players: 2\n");
    fprintf(fileWrite, "# Player 1\n");
    fprintf(fileWrite, "up: %d\n", SDL_SCANCODE_W);
    fprintf(fileWrite, "down: %d\n", SDL_SCANCODE_S);
    fprintf(fileWrite, "left: %d\n", SDL_SCANCODE_A);
    fprintf(fileWrite, "right: %d\n", SDL_SCANCODE_D);
    fprintf(fileWrite, "placeBomb: %d\n", SDL_SCANCODE_Q);
    fprintf(fileWrite, "# Player 2\n");
    fprintf(fileWrite, "up: %d\n", SDL_SCANCODE_UP);
    fprintf(fileWrite, "down: %d\n", SDL_SCANCODE_DOWN);
    fprintf(fileWrite, "left: %d\n", SDL_SCANCODE_LEFT);
    fprintf(fileWrite, "right: %d\n", SDL_SCANCODE_RIGHT);
    fprintf(fileWrite, "placeBomb: %d\n", SDL_SCANCODE_SPACE);
    fclose(fileWrite);
}