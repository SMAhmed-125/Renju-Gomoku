#ifndef _IO_H_
#define _IO_H_
#include "game.h"

/**
 * Imports a saved game from a file and returns the reconstructed game struct.
 * @param path The path to the file to be imported.
 * @return game* A pointer to the reconstructed game struct.
 * If the file doesn't exist, can't be read, or doesn't follow the specified format, the function exits with FILE_INPUT_ERR.
 */
game* game_import(const char* path);

/**
 * Exports the current game state to a file.
 * @param g A pointer to the game struct to be exported.
 * @param path The path to the file where the game state will be exported.
 */
void game_export(game* g, const char* path);
#endif
