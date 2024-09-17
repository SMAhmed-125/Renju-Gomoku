#ifndef _BOARD_H_
#define _BOARD_H_
#include <stdbool.h>
#define EMPTY_INTERSECTION 0
#define BLACK_STONE 1
#define WHITE_STONE 2
#define clear() printf("\033[H\033[J")

typedef struct {
    unsigned char size;
    unsigned char* grid;
} board;


/**
 * This function creates a new dynamically allocated board struct,
 * initializes board.size with the parameter size, initializes board.grid with a new dynamically allocated array,
 * initializes all grid intersections with EMPTY_INTERSECTION, finally it returns the struct created.
 * If an invalid size is given, exit with the code BOARD_SIZE_ERR as defined in error-codes.h.
 * @param size The size of the board to be created.
 * @return board* A pointer to the newly created board struct.
 */
board* board_create(unsigned char size);

/**
 * This function frees the memory of a dynamically allocated board struct,
 * it also frees the memory of its dynamically allocated grid field.
 * If the pointer b is NULL, exit with the code NULL_POINTER_ERR as defined in error-codes.h.
 * @param b A pointer to the board struct to be deleted.
 */
void board_delete(board* b);

/**
 * This function prints the board in the format specified in section Printing the Board with Stones.
 * If in_place is true, it clears the terminal first.
 * @param b A pointer to the board struct to be printed.
 * @param in_place A boolean indicating whether to clear the terminal before printing.
 */
void board_print(board* b, bool in_place);

/**
 * This function converts the horizontal coordinate x and vertical coordinate y
 * for a board.grid to a "letter + number" formal coordinate,
 * and stores the result in the buffer formal_coord. Finally it returns SUCCESS.
 * If the x and y coordinates are invalid for board b,
 * return COORDINATE_ERR instead.
 * @param b A pointer to the board struct.
 * @param x The horizontal coordinate.
 * @param y The vertical coordinate.
 * @param formal_coord A buffer to store the resulting formal coordinate.
 * @return unsigned char SUCCESS if the conversion is successful, otherwise COORDINATE_ERR.
 */
unsigned char board_formal_coord(board* b, unsigned char x, unsigned char y, char* formal_coord);

/**
 * This function converts a "letter + number" formal coordinate string formal_coord to the horizontal
 * and vertical coordinates for a board.grid, and stores the results in x and y that are passed by reference.
 * Finally it returns SUCCESS.
 * If formal_coord is invalid for board b, return FORMAL_COORDINATE_ERR instead.
 * Return codes are defined in error-codes.h.
 * @param b A pointer to the board struct.
 * @param formal_coord The formal coordinate string.
 * @param x A pointer to store the horizontal coordinate.
 * @param y A pointer to store the vertical coordinate.
 * @return unsigned char SUCCESS if the conversion is successful, otherwise FORMAL_COORDINATE_ERR.
 */
unsigned char board_coord(board* b, const char* formal_coord, unsigned char* x, unsigned char* y);

/**
 * This function returns the intersection occupation state stored in a board.grid
 * at the given horizontal and vertical coordinate pair x and y.
 * @param b A pointer to the board struct.
 * @param x The horizontal coordinate.
 * @param y The vertical coordinate.
 * @return unsigned char The intersection occupation state.
 */
unsigned char board_get(board* b, unsigned char x, unsigned char y);

/**
 * This function stores the intersection occupation state stone to a board.grid at the given horizontal and vertical coordinate pair x and y.
 * If stone is neither BLACK_STONE nor WHITE_STONE, exit with the code STONE_TYPE_ERR as defined in error-codes.h.
 * @param b A pointer to the board struct.
 * @param x The horizontal coordinate.
 * @param y The vertical coordinate.
 * @param stone The stone type (BLACK_STONE or WHITE_STONE).
 */
void board_set(board* b, unsigned char x, unsigned char y, unsigned char stone);

/**
 * This function returns true if all intersections of a board.grid are occupied by a stone, otherwise it returns false.
 * @param b A pointer to the board struct.
 * @return bool true if the board is full, otherwise false.
 */
bool board_is_full(board* b);
#endif
