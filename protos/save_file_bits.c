#include <stdio.h>
#include <string.h>
#include "../include/SDL2/SDL.h"
// Local personal includes
#include "../src/player.h"


void savePlayerKeyboardInputs(struct playerKeyboardInputs *inputs, FILE *file) {
    fwrite(inputs, sizeof(struct playerKeyboardInputs), 1, file);
}

struct playerKeyboardInputs loadPlayerKeyboardInputs(FILE *file) {
    struct playerKeyboardInputs inputs;
    fread(&inputs, sizeof(struct playerKeyboardInputs), 1, file);
    return inputs;
}

int main() {
    FILE *fileWrite = fopen("test.data", "wb");
    
    struct playerKeyboardInputs player1KeyboardInputs = {SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_Q};
    struct playerKeyboardInputs player2KeyboardInputs = {SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_SPACE};

    savePlayerKeyboardInputs(&player1KeyboardInputs, fileWrite);
    savePlayerKeyboardInputs(&player2KeyboardInputs, fileWrite);

    fclose(fileWrite);

    FILE *fileRead = fopen("test.data", "rb");

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