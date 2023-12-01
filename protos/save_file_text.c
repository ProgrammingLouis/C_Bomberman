#include <stdio.h>
#include <string.h>
#include "../include/SDL2/SDL.h"
// Local personal includes
#include "../src/player.h"


void savePlayerKeyboardInputs(struct playerKeyboardInputs *inputs, FILE *file) {
    fprintf(file, "up: %d\n", inputs->up);
    fprintf(file, "down: %d\n", inputs->down);
    fprintf(file, "left: %d\n", inputs->left);
    fprintf(file, "right: %d\n", inputs->right);
    fprintf(file, "placeBomb: %d\n", inputs->placeBomb);
}

struct playerKeyboardInputs loadPlayerKeyboardInputs(FILE *file) {
    struct playerKeyboardInputs inputs;
    char line[256];
    while (!feof(file)) {
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
            break;
        }
    }
    return inputs;
}

int main() {
    int nb_players = 2;
    FILE *fileWrite = fopen("test.txt", "w");
    fprintf(fileWrite, "nb_players: %d\n", nb_players);

    struct playerKeyboardInputs player1KeyboardInputs = {SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_Q};
    struct playerKeyboardInputs player2KeyboardInputs = {SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_SPACE};
    fprintf(fileWrite, "# Player 1\n");
    savePlayerKeyboardInputs(&player1KeyboardInputs, fileWrite);
    fprintf(fileWrite, "# Player 2\n");
    savePlayerKeyboardInputs(&player2KeyboardInputs, fileWrite);

    fclose(fileWrite);


    FILE *fileRead = fopen("test.txt", "r");

    struct playerKeyboardInputs loadedPlayer1KeyboardInputs = loadPlayerKeyboardInputs(fileRead);
    printf("loadedPlayer1KeyboardInputs.up: %d\n", loadedPlayer1KeyboardInputs.up);
    printf("loadedPlayer1KeyboardInputs.down: %d\n", loadedPlayer1KeyboardInputs.down);
    printf("loadedPlayer1KeyboardInputs.left: %d\n", loadedPlayer1KeyboardInputs.left);
    printf("loadedPlayer1KeyboardInputs.right: %d\n", loadedPlayer1KeyboardInputs.right);
    printf("loadedPlayer1KeyboardInputs.placeBomb: %d\n", loadedPlayer1KeyboardInputs.placeBomb);
    
    struct playerKeyboardInputs loadedPlayer2KeyboardInputs = loadPlayerKeyboardInputs(fileRead);
    printf("loadedPlayer2KeyboardInputs.up: %d\n", loadedPlayer2KeyboardInputs.up);
    printf("loadedPlayer2KeyboardInputs.down: %d\n", loadedPlayer2KeyboardInputs.down);
    printf("loadedPlayer2KeyboardInputs.left: %d\n", loadedPlayer2KeyboardInputs.left);
    printf("loadedPlayer2KeyboardInputs.right: %d\n", loadedPlayer2KeyboardInputs.right);
    printf("loadedPlayer2KeyboardInputs.placeBomb: %d\n", loadedPlayer2KeyboardInputs.placeBomb);

    fclose(fileRead);

    return 0;
}