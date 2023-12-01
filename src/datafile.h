#ifndef DATAFILE_H
#define DATAFILE_H
#include "../src/player.h"

struct save_data {
    int nbPlayers;
    struct playerKeyboardInputs player1KeyboardInputs;
    struct playerKeyboardInputs player2KeyboardInputs;
};

// struct game_data {
//     // struct save_data saveData;
//     // int nbPlayers;
//     int MaxFps;
//     double DisplayScale;
//     int nbCellsRow;
//     int nbCellsCollumn;
//     int cellSize = 60*DisplayScale;
//     int playAreaTopLeftX = 70*DisplayScale;
//     int playAreaTopLeftY = 70*DisplayScale;

// };

void savePlayerKeyboardInputs(struct playerKeyboardInputs *inputs, FILE *file);

struct playerKeyboardInputs loadPlayerKeyboardInputs(FILE *file);

struct save_data loadSaveData(FILE *file);

void createDefaultDataFile();

#endif