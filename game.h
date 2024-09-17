#ifndef _GAME_H
#define _GAME_H
#include "board.h"
#include <stdbool.h>
#include <stdlib.h>

#define GAME_FREESTYLE 0
#define GAME_RENJU 1
#define GAME_STATE_PLAYING 0
#define GAME_STATE_FORBIDDEN 1
#define GAME_STATE_STOPPED 2
#define GAME_STATE_FINISHED 3

#define GAME_CAPACITY 16
#define DEFAULT_STRING_LENGTH 10
#define INITIALIZE 0
#define WIN_CONDITION 5
#define OPEN_FOUR 4
#define OPEN_THREE 3
#define MAX_VIOLATIONS 2
#define OVERLINE 6
#define BLANK_COORD -1
#define ONE_SPACE_OUT 1
#define TWO_SPACE_OUT 2
#define CAPACITY_INCREASE 2
#define LONG_DEFAULT_STRING_LENGTH 100

typedef struct {
    unsigned char x;
    unsigned char y;
    unsigned char stone;
} move;

typedef struct {
    board* board;
    unsigned char type;
    unsigned char stone;
    unsigned char state;
    unsigned char winner;
    move* moves;
    size_t moves_count;
    size_t moves_capacity;
} game;

/**
 * This function creates and returns a new dynamically allocated game struct of the specified type game_type with all fields initialized.
 * The board should be created with function board_create(); type should be set to game_type;
 * stone should be set to BLACK_STONE; state should be set to GAME_STATE_PLAYING; winner should be set to EMPTY_INTERSECTION;
 * moves should be dynamically allocated, it should have an initial capacity to hold 16 moves, also, initialize moves_count and moves_capacity accordingly.
 * @param board_size The size of the board to be created.
 * @param game_type The type of the game (GAME_FREESTYLE or GAME_RENJU).
 * @return game* A pointer to the newly created game struct.
 */
game* game_create(unsigned char board_size, unsigned char game_type);

/**
 * This function frees the memory of a dynamically allocated game struct.
 * You should also free the memory of its dynamically allocated fields, note that board should be freed by function board_delete().
 * If g is NULL, exit with the code NULL_POINTER_ERR as defined in error-codes.h.
 * @param g A pointer to the game struct to be deleted.
 */
void game_delete(game* g);

/**
 * This function controls what happens in the game at each turn. If the state of game g isn't GAME_STATE_PLAYING,
 * return false immediately. Otherwise, prompt the player to enter a move, if the move entered is badly-formatted or out of board boundary,
 * re-prompt and retry until a valid one is entered; if an EOF is entered in the process, stop the game and prompt accordingly.
 * Once a valid move is input, actuate the move by calling game_place_stone() and return true.
 * @param g A pointer to the game struct.
 * @return bool true if the game continues, false if it stops.
 */
bool game_update(game* g);

/**
 * This function controls the game loop as the name suggests,
 * it prints the current board and calls game_update(), repeat until game_update() returns false.
 * @param g A pointer to the game struct.
 */
void game_loop(game* g);

/**
 * This function restarts the game loop for a saved game.
 * If the state of g is not GAME_STATE_STOPPED, exit with the code RESUME_ERR as defined in error-codes.h.
 * Also, before you call game_resume(), you should check the type of g, if it's against the type of the executable (gomoku or renju),
 * you should exit with RESUME_ERR as well. Otherwise, set state of g to GAME_STATE_PLAYING, and call game_loop().
 * @param g A pointer to the game struct.
 */
void game_resume(game* g);

/**
 * This function replays a saved game. It creates a new game struct, 
 * and re-makes moves saved in g, pausing 1 second before making every move (calls function sleep()).
 * This function should act largely the same as game_loop() except it doesn't read user input, and prints extra information as specified in section Replay Saved Games.
 * @param g A pointer to the game struct.
 */
void game_replay(game* g);

/**
 * This function checks for winning conditions in freestyle Gomoku.
 * If five or more consecutive stones are found in any direction (horizontal, vertical, main diagonals, secondary diagonals), 
 * it updates the game state and winner and returns true. If the board is full, it updates the game state to finished and returns false. Otherwise, returns true.
 * @param g A pointer to the game struct.
 * @param x The x-coordinate of the last move.
 * @param y The y-coordinate of the last move.
 * @return bool true if the game continues, false if it stops.
 */
bool game_place_stone(game* g, unsigned char x, unsigned char y);
#endif
