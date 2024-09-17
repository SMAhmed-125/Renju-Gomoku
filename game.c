/**
* @file game.c
* @author Sadia Ahmed (sahmed23)
* This file creates a game
* Depending on the game format choosen it will replay the game
* or resume it and make sure that all moves are made according to the rule
*/


#include "io.h"
#include "board.h"
#include "game.h"
#include "error-codes.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

game* game_create( unsigned char board_size, unsigned char game_type ) 
{

    game * Game = ( game * ) malloc ( sizeof( game ) );
    Game->board = board_create( board_size );
    Game->type = game_type;
    Game->stone = BLACK_STONE;
    Game->state = GAME_STATE_PLAYING;
    Game->winner = EMPTY_INTERSECTION;
    Game->moves_capacity = GAME_CAPACITY;
    Game->moves = ( move * ) malloc( Game->moves_capacity * sizeof( move ) );
    Game->moves_count = INITIALIZE;
    return Game;

}

void game_delete( game* g )
{
    if ( !g ) {
        exit ( NULL_POINTER_ERR );
    }

    free( g->moves );
    free( g );
}

bool game_update( game* g ) 
{
    if ( g->state != GAME_STATE_PLAYING ) {
        return false;
    }
    
    // Storing the input from terminal
    char input[DEFAULT_STRING_LENGTH];
    
    // Creating x to store the vertical coord
    unsigned char x;

    // Creating y to store the horizontal coord
    unsigned char y;

    while ( true ) {
        printf( "%s stone's turn, please enter a move: \n", g->stone == BLACK_STONE ? "Black" : "White" );


        if ( fgets( input, sizeof( input ), stdin ) == NULL ) {
            if ( feof( stdin ) ) {
                printf( "The game is stopped.\n" );
                g->state = GAME_STATE_STOPPED;
                clearerr( stdin ); // Clear the EOF flag for future use
            } else {
                // Handle input error
                perror( "Error reading input" );
            }
            return false;
        }

        if ( board_coord( g->board, input, &x, &y ) == SUCCESS ) {
            if ( game_place_stone( g, x, y ) ) {
                return true;
            }
        }

        printf( "The coordinate you entered is invalid, please try again.\n" );
    }
}

void game_loop( game* g )
{
    do {
        board_print( g->board, true );
        if ( g->state != GAME_STATE_PLAYING ) {
            break;
        }
    } while ( game_update( g ) );

    if ( g->state == GAME_STATE_FINISHED && g->winner != EMPTY_INTERSECTION ) {
        printf( "Game concluded, %s won.\n", g->stone == BLACK_STONE ? "white" : "black" );
    } else if ( g->state == GAME_STATE_FORBIDDEN ) {
        printf( "Game concluded, black made a forbidden move, white won.\n" );
    } else if ( g->state == GAME_STATE_FINISHED && g->winner == EMPTY_INTERSECTION ) {
        printf( "Game concluded, the board is full, draw.\n" );
    }
}

void game_resume( game* g ) 
{
    if ( g->state != GAME_STATE_STOPPED ) {
        exit( RESUME_ERR );
    }

    g->state = GAME_STATE_PLAYING;

    game_loop( g );
}



void game_replay( game* g ) 
{

    // Creating a string to store the formatted coord
    char formatted_coord[DEFAULT_STRING_LENGTH];

    for ( int i = INITIALIZE; i < g->moves_count; i++ ) {

        // Storing the x coord of current i
        unsigned char x = g->moves[i].x;

        // Storing the y cood of current i
        unsigned char y = g->moves[i].y;

        // Storing the stone type of current i
        unsigned char stone = g->moves[i].stone;

        board_set( g->board, x, y, stone );

        board_print( g->board, true );

        if ( i == g->moves_count - 1 ) {

            if ( g->state == GAME_STATE_FINISHED && g->winner != EMPTY_INTERSECTION ) {
                printf( "Game concluded, %s won.\n", g->stone == BLACK_STONE ? "white" : "black" );
            } else if ( g->state == GAME_STATE_FORBIDDEN ) {
                printf( "Game concluded, black made a forbidden move, white won.\n" );
            } else if ( g->state == GAME_STATE_FINISHED && g->winner == EMPTY_INTERSECTION ) {
                printf( "Game concluded, the board is full, draw.\n" );
            } else {
                printf( "The game is stopped.\n" );
            }

        }

        printf( "Moves:\n" );

        for ( int j = INITIALIZE; j <= i; j++ ) {
            board_formal_coord( g->board, g->moves[j].x, g->moves[j].y, formatted_coord );
            if ( g->moves[j].stone == BLACK_STONE ) {
                printf( "Black: %3s", formatted_coord );
            } else {
                printf( "  White: %3s\n", formatted_coord );
            }
        }

        // sleep( 1 ); // Pause for 1 second before next move
    }

    if ( g->moves[g->moves_count - 1].stone != WHITE_STONE ) {
        printf( "\n" );
    }


}


/**
 * This function checks for winning conditions in freestyle Gomoku.
 * If five or more consecutive stones are found in any direction (horizontal, vertical, main diagonals, secondary diagonals), 
 * it updates the game state and winner and returns true. If the board is full, it updates the game state to finished and returns false. Otherwise, returns true.
 * @param g A pointer to the game struct.
 * @param x The x-coordinate of the last move.
 * @param y The y-coordinate of the last move.
 * @return bool true if the game continues or is won, false if it stops.
 */
static bool game_freestyle_gomoku( game* g, unsigned char x, unsigned char y ) {

    // Casting the board grid
    unsigned char ( *grid )[g->board->size] = ( unsigned char ( * )[g->board->size] ) g->board->grid;

    // Storing the size of the board
    int size = g->board->size;

    // Creating a counter for stones
    int count;

    // Check horizontal
    for ( int i = INITIALIZE; i < size; i++ ) {
        count = INITIALIZE;
        for ( int j = INITIALIZE; j < size; j++ ) {
            if ( grid[i][j] == g->stone ) {
                count++;
                if ( count >= WIN_CONDITION ) {
                    g->state = GAME_STATE_FINISHED;
                    g->winner = g->stone;
                    return true;
                }
            } else {
                count = INITIALIZE;
            }
        }
    }

    // Check vertical
    for ( int j = INITIALIZE; j < size; j++ ) {
        count = INITIALIZE;
        for ( int i = INITIALIZE; i < size; i++ ) {
            if ( grid[i][j] == g->stone ) {
                count++;
                if ( count >= WIN_CONDITION ) {
                    g->state = GAME_STATE_FINISHED;
                    g->winner = g->stone;
                    return true;
                }
            } else {
                count = INITIALIZE;
            }
        }
    }

    // Check main diagonals from top-left to bottom-right
    for ( int i = INITIALIZE; i <= size - WIN_CONDITION; i++ ) {
        for ( int j = INITIALIZE; j <= size - WIN_CONDITION; j++ ) {
            count = INITIALIZE;
            for ( int k = INITIALIZE; k < WIN_CONDITION; k++ ) {
                if ( grid[i + k][j + k] == g->stone ) {
                    count++;
                    if ( count >= WIN_CONDITION ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = g->stone;
                        return true;
                    }
                } else {
                    break;
                }
            }
        }
    }

    // Check secondary diagonals from bottom-left to top-right
    for ( int i = INITIALIZE; i <= size - WIN_CONDITION; i++ ) {
        for ( int j = INITIALIZE; j <= size - WIN_CONDITION; j++ ) {
            count = INITIALIZE;
            for ( int k = INITIALIZE; k < WIN_CONDITION; k++ ) {
                if ( grid[i - k][j + k] == g->stone ) {
                    count++;
                    if ( count >= WIN_CONDITION ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = g->stone;
                        return true;
                    }
                } else {
                    break;
                }
            }
        }
    }

    // Check if the board is full
    if ( board_is_full( g->board ) ) {
        g->state = GAME_STATE_FINISHED;
        return false;
    }

    // No winner
    return true;
}

/**
 * This function helps sort and order the patterns to make it more apparent
 * which patterns are repeats and which may be new instances
 * @param pattern of at most 4 pairs of x and y coordinates to be sorted
 */
static void normalize_pattern( int pattern[4][2] ) {

    // Sort the pattern coordinates to ensure a unique representation
    for ( int i = INITIALIZE; i < 3; i++ ) {

        for ( int j = i + 1; j < OPEN_FOUR; j++ ) {

            if ( pattern[i][INITIALIZE] > pattern[j][INITIALIZE] || 
               ( pattern[i][INITIALIZE] == pattern[j][INITIALIZE] && pattern[i][1] > pattern[j][1] ) ) {

                // Swap the coordinates
                int temp_x = pattern[i][INITIALIZE];
                int temp_y = pattern[i][1];

                pattern[i][INITIALIZE] = pattern[j][INITIALIZE];
                pattern[i][1] = pattern[j][1];

                pattern[j][INITIALIZE] = temp_x;
                pattern[j][1] = temp_y;
            }
        }
    }
}


/**
 * This function checks for winning conditions in renju just for the black player.
 * If five or more consecutive stones are found in any direction (horizontal, vertical, main diagonals, secondary diagonals), 
 * it updates the game state and winner and returns true. If a forbidden move is played, it automatically exits.
 * If the board is full, it updates the game state to finished and returns false. Otherwise, returns true.
 * @param g A pointer to the game struct.
 * @param x The x-coordinate of the last move.
 * @param y The y-coordinate of the last move.
 * @return bool true if the game continues, false if it stops.
 */
static bool game_renju_black( game* g, unsigned char x, unsigned char y ) {

    // Casting the board grid to grid
    unsigned char ( *grid )[g->board->size] = ( unsigned char( * )[g->board->size] ) g->board->grid;

    // Storing the size of the board
    int size = g->board->size;

    // Storing the count of open fours
    int open_four_count = INITIALIZE;

    // Storing the count of half open fours
    int half_open_four_count = INITIALIZE;

    // Storing the count of open threes
    int open_three_count = INITIALIZE;
    
    // Storing the count in the positive direction for a given orientation
    int pos_count;

    // Storing the count in the neg direction for a given orientation
    int neg_count;

    // Array to store up to 3 sets of 4 coordinate pairs
    int pattern_coords[3][4][2];

    // How many patterns of valid open threes, or fours have been stored yet
    int pattern_count = INITIALIZE;

    // Counting inner spaces for open three
    int pattern_spaces = INITIALIZE;

    // Counting inner spaces for the positive traversal
    int pos_spaces = INITIALIZE;

    // Counting inner spaces for the negative traversal
    int neg_spaces = INITIALIZE;

    // Initialize the temporary array
    memset( pattern_coords, BLANK_COORD, sizeof( pattern_coords ) );

    // The move 
    int the_move[2] = { y, x };

    for ( int i = INITIALIZE; i < size; i++ ) {

        for ( int j = INITIALIZE; j < size; j++ ) {

            if ( grid[i][j] != BLACK_STONE ) continue;

            // Check horizontal
            pos_count = INITIALIZE;
            neg_count = INITIALIZE;
            int new_pattern[4][2] = { {BLANK_COORD, BLANK_COORD}, {BLANK_COORD, BLANK_COORD}, {BLANK_COORD, BLANK_COORD}, {BLANK_COORD, BLANK_COORD} };

            // Check forward direction on horizontal
            for ( int k = INITIALIZE; k < OVERLINE; k++ ) {

                if ( j + k >= size ) break;

                if ( grid[i][j + k] == BLACK_STONE || grid[i][j + k] == EMPTY_INTERSECTION ) {
                    
                    if ( grid[i][j + k] == BLACK_STONE ) {

                        new_pattern[pos_count][INITIALIZE] = i;  // Store x coordinate
                        new_pattern[pos_count][1] = j + k;  // Store y coordinate
                        pos_count++;

                    } else if ( grid[i][j + k] == EMPTY_INTERSECTION ) {

                        pos_spaces++;

                        if ( grid[i][j + k + ONE_SPACE_OUT] == BLACK_STONE ) {

                            if ( grid[i][j + k + TWO_SPACE_OUT] == EMPTY_INTERSECTION ) {
                                pattern_spaces++;
                            }
                            
                            continue;

                        } else {
                            break;
                        }
                    }

                    // checking to see if it is a new instance
                    bool is_new_instance = false;
                    for ( int l = INITIALIZE; l < 4; l++ ) {
                        if ( memcmp( new_pattern[l], the_move, sizeof( the_move ) ) == INITIALIZE ) {
                            is_new_instance = true;
                            break;
                        }
                    }

                    // Check for open fours 
                    if ( pos_count == OPEN_FOUR && j - ONE_SPACE_OUT >= INITIALIZE && j + pos_count < size &&
                        grid[i][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION &&  grid[i][j + pos_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_four_count++;

                        if ( open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    }  else if ( pos_count == OPEN_FOUR && j - ONE_SPACE_OUT >= INITIALIZE && j + pos_count < size &&
                        ( ( grid[i][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION ) ^ ( grid[i][j + pos_count] == EMPTY_INTERSECTION && is_new_instance ) ) ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        half_open_four_count++;

                        if ( half_open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( pos_count == OPEN_THREE && j - ONE_SPACE_OUT >= INITIALIZE && j + pos_count < size && pattern_spaces == INITIALIZE &&
                        grid[i][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i][j + pos_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      

                    } else if ( pos_count == OPEN_THREE && j - ONE_SPACE_OUT >= INITIALIZE && j + OPEN_FOUR < size && pattern_spaces == 1 &&
                        grid[i][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i][j + OPEN_FOUR] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      

                    } else if ( pos_count == OVERLINE || ( half_open_four_count + open_four_count >= MAX_VIOLATIONS ) ) {

                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return true;

                    } else if ( pos_count == WIN_CONDITION && pos_spaces == INITIALIZE && grid[i][j + k + ONE_SPACE_OUT] != BLACK_STONE ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = BLACK_STONE;
                        return true;
                    }

                } else {
                    break;
                }
            }

            pattern_spaces = INITIALIZE;
            memset( new_pattern, BLANK_COORD, sizeof( new_pattern ) ); // Reset the pattern array
            
            // Check backward direction on horizontal
            for ( int k = INITIALIZE; k < OVERLINE; k++ ) {

                if ( j - k < INITIALIZE ) break;

                if ( grid[i][j - k] == BLACK_STONE || grid[i][j - k] == EMPTY_INTERSECTION ) {

                    if ( grid[i][j - k] == BLACK_STONE ) {

                        new_pattern[neg_count][INITIALIZE] = i;  // Store x coordinate
                        new_pattern[neg_count][1] = j - k;  // Store y coordinate
                        neg_count++;

                    } else if ( grid[i][j - k] == EMPTY_INTERSECTION ) {
                        
                        neg_spaces++;

                        if ( grid[i][j - k - ONE_SPACE_OUT] == BLACK_STONE ) {

                            if ( grid[i][j - k - TWO_SPACE_OUT] == EMPTY_INTERSECTION ) {
                                pattern_spaces++;
                            }

                            continue;

                        } else {
                            break;
                        }
                    }

                    // checking to see if it is a new instance
                    bool is_new_instance = false;
                    for ( int l = INITIALIZE; l < OPEN_FOUR; l++ ) {
                        if ( memcmp( new_pattern[l], the_move, sizeof( the_move ) ) == INITIALIZE ) {
                            is_new_instance = true;
                            break;
                        }
                    }

                    // Check for open fours 
                    if ( neg_count == OPEN_FOUR && j + ONE_SPACE_OUT < size && j - neg_count >= INITIALIZE &&
                        grid[i][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i][j - neg_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_four_count++;

                        if ( open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    }  else if ( neg_count == OPEN_FOUR && j + ONE_SPACE_OUT < size && j - neg_count >= INITIALIZE &&
                        ( ( grid[i][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION ) ^ ( grid[i][j - neg_count] == EMPTY_INTERSECTION ) ) && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        half_open_four_count++;

                        if ( half_open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( neg_count == OPEN_THREE && j + ONE_SPACE_OUT < size && j - neg_count >= INITIALIZE && pattern_spaces == INITIALIZE &&
                        grid[i][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i][j - neg_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( neg_count == OPEN_THREE && j + ONE_SPACE_OUT < size && j - OPEN_FOUR >= INITIALIZE && pattern_spaces == 1 &&
                        grid[i][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i][j - OPEN_FOUR] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( neg_count == OVERLINE || ( half_open_four_count + open_four_count >= MAX_VIOLATIONS ) ) {

                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return true;

                    } else if ( neg_count == WIN_CONDITION && neg_spaces == INITIALIZE && grid[i][j - k - ONE_SPACE_OUT] != BLACK_STONE ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = BLACK_STONE;
                        return true;
                    }

                } else {
                    break;
                }
            }

            // Check vertical
            pos_count = INITIALIZE;
            neg_count = INITIALIZE;

            pos_spaces = INITIALIZE;
            neg_spaces = INITIALIZE;

            pattern_spaces = INITIALIZE;
            memset( new_pattern, BLANK_COORD, sizeof( new_pattern ) ); // Reset the pattern array

            // Check forward vertical
            for ( int k = INITIALIZE; k < OVERLINE; k++ ) {

                if ( i + k >= size ) break;

                if ( grid[i + k][j] == BLACK_STONE || grid[i + k][j] == EMPTY_INTERSECTION ) {

                    if ( grid[i + k][j] == BLACK_STONE ) {

                        new_pattern[pos_count][INITIALIZE] = i + k;  // Store x coordinate
                        new_pattern[pos_count][1] = j;  // Store y coordinate
                        pos_count++;

                    } else if ( grid[i + k][j] == EMPTY_INTERSECTION ) {
                        
                        pos_spaces++;

                        if ( grid[i + k + ONE_SPACE_OUT][j] == BLACK_STONE ) {

                            if ( grid[i + k + TWO_SPACE_OUT][j] == EMPTY_INTERSECTION ) {
                                pattern_spaces++;
                            }

                            continue;

                        } else {
                            break;
                        }
                    }

                    // checking to see if it is a new instance
                    bool is_new_instance = false;
                    for ( int l = INITIALIZE; l < OPEN_FOUR; l++ ) {
                        if ( memcmp( new_pattern[l], the_move, sizeof( the_move ) ) == INITIALIZE ) {
                            is_new_instance = true;
                            break;
                        }
                    }

                    // Check for open fours before counting
                    if ( pos_count == OPEN_FOUR && i - ONE_SPACE_OUT >= INITIALIZE && i + pos_count < size &&
                        grid[i - ONE_SPACE_OUT][j] == EMPTY_INTERSECTION && grid[i + pos_count][j] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_four_count++;

                        if ( open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    }  else if ( pos_count == OPEN_FOUR && i - ONE_SPACE_OUT >= INITIALIZE && i + pos_count < size &&
                        ( ( grid[i - ONE_SPACE_OUT][j] == EMPTY_INTERSECTION ) ^ ( grid[i + pos_count][j] == EMPTY_INTERSECTION ) ) && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern )  ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        half_open_four_count++;

                        if ( half_open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( pos_count == OPEN_THREE && i - ONE_SPACE_OUT >= INITIALIZE && i + pos_count < size && pattern_spaces == INITIALIZE &&
                        grid[i - ONE_SPACE_OUT][j] == EMPTY_INTERSECTION && grid[i + pos_count][j] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( pos_count == OPEN_THREE && i - ONE_SPACE_OUT >= INITIALIZE && i + OPEN_FOUR < size && pattern_spaces == 1 &&
                        grid[i - ONE_SPACE_OUT][j] == EMPTY_INTERSECTION && grid[i + OPEN_FOUR][j] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( pos_count == OVERLINE || ( half_open_four_count + open_four_count >= MAX_VIOLATIONS ) ) {

                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return true;

                    } else if ( pos_count == WIN_CONDITION && pos_spaces == INITIALIZE && grid[i + k + ONE_SPACE_OUT][j] != BLACK_STONE ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = BLACK_STONE;
                        return true;
                    }

                } else {
                    break;
                }
            }

            pattern_spaces = INITIALIZE;
            memset( new_pattern, BLANK_COORD, sizeof( new_pattern ) ); // Reset the pattern array

            // Check backward vertical
            for ( int k = INITIALIZE; k < OVERLINE; k++ ) {

                if ( i - k < INITIALIZE ) break;

                if ( grid[i - k][j] == BLACK_STONE || grid[i - k][j] == EMPTY_INTERSECTION ) {

                    if ( grid[i - k][j] == BLACK_STONE ) {

                        new_pattern[neg_count][INITIALIZE] = i - k;  // Store x coordinate
                        new_pattern[neg_count][1] = j;  // Store y coordinate
                        neg_count++;

                    } else if ( grid[i - k][j] == EMPTY_INTERSECTION ) {

                        neg_spaces++;

                        if ( grid[i - k - ONE_SPACE_OUT][j] == BLACK_STONE ) {

                            if ( grid[i - k - TWO_SPACE_OUT][j] == EMPTY_INTERSECTION ) {
                                pattern_spaces++;
                            }

                            continue;

                        } else {
                            break;
                        }
                    }

                    // checking to see if it is a new instance
                    bool is_new_instance = false;
                    for ( int l = INITIALIZE; l < OPEN_FOUR; l++ ) {
                        if ( memcmp( new_pattern[l], the_move, sizeof( the_move ) ) == INITIALIZE ) {
                            is_new_instance = true;
                            break;
                        }
                    }

                    // Check for open fours 
                    if ( neg_count == OPEN_FOUR && i + ONE_SPACE_OUT < size && i - neg_count >= INITIALIZE &&
                        grid[i + ONE_SPACE_OUT][j] == EMPTY_INTERSECTION && grid[i - neg_count][j] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_four_count++;

                        if ( open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( neg_count == OPEN_FOUR && i + ONE_SPACE_OUT < size && i - neg_count >= INITIALIZE &&
                        ( ( grid[i + ONE_SPACE_OUT][j] == EMPTY_INTERSECTION ) ^ ( grid[i - neg_count][j] == EMPTY_INTERSECTION ) ) && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        half_open_four_count++;

                        if ( half_open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( neg_count == OPEN_THREE && i + ONE_SPACE_OUT < size && i - neg_count >= INITIALIZE && pattern_spaces == INITIALIZE &&
                        grid[i + ONE_SPACE_OUT][j] == EMPTY_INTERSECTION && grid[i - neg_count][j] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( neg_count == OPEN_THREE && i + 1 < size && i - OPEN_FOUR >= INITIALIZE && pattern_spaces == 1 &&
                        grid[i + ONE_SPACE_OUT][j] == EMPTY_INTERSECTION && grid[i - OPEN_FOUR][j] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( neg_count == OVERLINE || ( half_open_four_count + open_four_count >= MAX_VIOLATIONS ) ) {

                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return true;

                    } else if ( neg_count == WIN_CONDITION && neg_spaces == INITIALIZE && grid[i - k - ONE_SPACE_OUT][j] != BLACK_STONE ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = BLACK_STONE;
                        return true;
                    }

                } else {
                    break;
                }
            }

            // Check main diagonals from bottom-left to top-right
            pos_count = INITIALIZE;
            neg_count = INITIALIZE;

            pos_spaces = INITIALIZE;
            neg_spaces = INITIALIZE;

            pattern_spaces = INITIALIZE;
            memset( new_pattern, BLANK_COORD, sizeof( new_pattern ) ); // Reset the pattern array

            // Check forward traversal of diagonal of type '/'
            for ( int k = INITIALIZE; k < OVERLINE; k++ ) {

                if ( i + k >= size || j + k >= size ) break;

                if ( grid[i + k][j + k] == BLACK_STONE || grid[i + k][j + k] == EMPTY_INTERSECTION ) {

                    if ( grid[i + k][j + k] == BLACK_STONE ) {

                        new_pattern[pos_count][INITIALIZE] = i + k;  // Store x coordinate
                        new_pattern[pos_count][1] = j + k;  // Store y coordinate
                        pos_count++;

                    } else if ( grid[i + k][j + k] == EMPTY_INTERSECTION ) {

                        pos_spaces++;

                        if ( grid[i + k + ONE_SPACE_OUT][j + k + ONE_SPACE_OUT] == BLACK_STONE ) {

                            if ( grid[i + k + TWO_SPACE_OUT][j + k + TWO_SPACE_OUT] == EMPTY_INTERSECTION ) {
                                pattern_spaces++;
                            }

                            continue;

                        } else {
                            break;
                        }
                    }
        
                    // checking to see if it is a new instance
                    bool is_new_instance = false;
                    for ( int l = INITIALIZE; l < OPEN_FOUR; l++ ) {
                        if ( memcmp( new_pattern[l], the_move, sizeof( the_move ) ) == INITIALIZE ) {
                            is_new_instance = true;
                            break;
                        }
                    }

                    // Check for open fours
                    if ( pos_count == OPEN_FOUR && i - ONE_SPACE_OUT >= INITIALIZE && j - ONE_SPACE_OUT >= INITIALIZE && i + pos_count < size && j + pos_count < size &&
                        grid[i - ONE_SPACE_OUT][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i + pos_count][j + pos_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_four_count++;

                        if ( open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( pos_count == OPEN_FOUR && i - ONE_SPACE_OUT >= INITIALIZE && j - ONE_SPACE_OUT >= INITIALIZE && i + pos_count < size && j + pos_count < size &&
                        ( ( grid[i - ONE_SPACE_OUT][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION ) ^ ( grid[i + pos_count][j + pos_count] == EMPTY_INTERSECTION ) ) && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        half_open_four_count++;

                        if ( half_open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( pos_count == OPEN_THREE && i - ONE_SPACE_OUT >= INITIALIZE && j - ONE_SPACE_OUT >= INITIALIZE && i + pos_count < size && j + pos_count < size && pattern_spaces == INITIALIZE &&
                        grid[i - ONE_SPACE_OUT][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i + pos_count][j + pos_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( pos_count == OPEN_THREE && i - ONE_SPACE_OUT >= INITIALIZE && j - ONE_SPACE_OUT >= INITIALIZE && i + OPEN_FOUR < size && j + OPEN_FOUR < size && pattern_spaces == 1
                        && grid[i - ONE_SPACE_OUT][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i + OPEN_FOUR][j + OPEN_FOUR] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( pos_count == OVERLINE || ( half_open_four_count + open_four_count >= MAX_VIOLATIONS ) ) {

                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return true;

                    } else if ( pos_count == WIN_CONDITION && pos_spaces == INITIALIZE && grid[i + k + ONE_SPACE_OUT][j + k + ONE_SPACE_OUT] != BLACK_STONE ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = BLACK_STONE;
                        return true;
                    }

                } else {
                    break;
                }
            }
            
            pattern_spaces = INITIALIZE;
            memset( new_pattern, BLANK_COORD, sizeof( new_pattern ) ); // Reset the pattern array

            // Check backwards traveral of diagonal of type '/'
            for ( int k = INITIALIZE; k < OVERLINE; k++ ) {

                if ( i - k < INITIALIZE || j - k < INITIALIZE ) break;

                if ( grid[i - k][j - k] == BLACK_STONE || grid[i - k][j - k] == EMPTY_INTERSECTION ) {

                    if ( grid[i - k][j - k] == BLACK_STONE ) {

                        new_pattern[neg_count][INITIALIZE] = i - k;  // Store x coordinate
                        new_pattern[neg_count][1] = j - k;  // Store y coordinate
                        neg_count++;

                    } else if ( grid[i - k][j - k] == EMPTY_INTERSECTION ) {

                        neg_spaces++;

                        if ( grid[i - k - ONE_SPACE_OUT][j - k - ONE_SPACE_OUT] == BLACK_STONE ) {

                            if ( grid[i - k - TWO_SPACE_OUT][j - k - TWO_SPACE_OUT] == EMPTY_INTERSECTION ) {
                                pattern_spaces++;
                            }

                            continue;

                        } else {
                            break;
                        }
                    }

                    // checking to see if it is a new instance
                    bool is_new_instance = false;
                    for ( int l = INITIALIZE; l < OPEN_FOUR; l++ ) {
                        if ( memcmp( new_pattern[l], the_move, sizeof( the_move ) ) == INITIALIZE ) {
                            is_new_instance = true;
                            break;
                        }
                    }

                    // Check for open fours
                    if ( neg_count == OPEN_FOUR && i + ONE_SPACE_OUT < size && j + ONE_SPACE_OUT < size && i - neg_count >= INITIALIZE && j - neg_count >= INITIALIZE &&
                        grid[i + ONE_SPACE_OUT][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i - neg_count][j - neg_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_four_count++;
                        
                        if ( open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( neg_count == OPEN_FOUR && i + ONE_SPACE_OUT < size && j + ONE_SPACE_OUT < size && i - neg_count >= INITIALIZE && j - neg_count >= INITIALIZE &&
                        ( ( grid[i + ONE_SPACE_OUT][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION ) ^ ( grid[i - neg_count][j - neg_count] == EMPTY_INTERSECTION ) ) && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        half_open_four_count++;

                        if ( half_open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( neg_count == OPEN_THREE && i + ONE_SPACE_OUT < size && j + ONE_SPACE_OUT < size && i - neg_count >= INITIALIZE && j - neg_count >= INITIALIZE && pattern_spaces == INITIALIZE &&
                        grid[i + ONE_SPACE_OUT][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i - neg_count][j - neg_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( neg_count == OPEN_THREE && i + ONE_SPACE_OUT < size && j + ONE_SPACE_OUT < size && i - OPEN_FOUR >= INITIALIZE && j - OPEN_FOUR >= INITIALIZE && pattern_spaces == 1 &&
                        grid[i + ONE_SPACE_OUT][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i - OPEN_FOUR][j - OPEN_FOUR] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( neg_count == OVERLINE || ( half_open_four_count + open_four_count >= MAX_VIOLATIONS ) ) {

                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return true;

                    } else if ( neg_count == WIN_CONDITION && neg_spaces == INITIALIZE && grid[i - k - ONE_SPACE_OUT][j - k - ONE_SPACE_OUT] != BLACK_STONE ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = BLACK_STONE;
                        return true;
                    }

                } else {
                    break;
                }
            }

            // Check secondary diagonals from top-left to bottom-right
            pos_count = INITIALIZE;
            neg_count = INITIALIZE;

            pos_spaces = INITIALIZE;
            neg_spaces = INITIALIZE;

            pattern_spaces = INITIALIZE;
            memset( new_pattern, BLANK_COORD, sizeof( new_pattern ) ); // Reset the pattern array

            // Check forward traveral of secondary diagonal of type '\'
            for ( int k = INITIALIZE; k < OVERLINE; k++ ) {

                if ( i - k < INITIALIZE || j + k >= size ) break;

                if ( grid[i - k][j + k] == BLACK_STONE || grid[i - k][j + k] == EMPTY_INTERSECTION ) {

                    if ( grid[i - k][j + k] == BLACK_STONE ) {

                        new_pattern[pos_count][INITIALIZE] = i - k;  // Store x coordinate
                        new_pattern[pos_count][1] = j + k;  // Store y coordinate
                        pos_count++;

                    } else if ( grid[i - k][j + k] == EMPTY_INTERSECTION ) {
                        
                        pos_spaces++;

                        if ( grid[i - k - ONE_SPACE_OUT][j + k + ONE_SPACE_OUT] == BLACK_STONE ) {

                            if ( grid[i - k - TWO_SPACE_OUT][j + k + TWO_SPACE_OUT] == EMPTY_INTERSECTION ) {
                                pattern_spaces++;
                            }

                            continue;

                        } else {
                            break;
                        }
                    }

                    // checking to see if it is a new instance
                    bool is_new_instance = false;
                    for ( int l = INITIALIZE; l < OPEN_FOUR; l++ ) {
                        if ( memcmp( new_pattern[l], the_move, sizeof( the_move ) ) == INITIALIZE ) {
                            is_new_instance = true;
                            break;
                        }
                    }

                    // Check for open fours 
                    if ( pos_count == OPEN_FOUR && i + ONE_SPACE_OUT < size && j - ONE_SPACE_OUT >= INITIALIZE && i - pos_count >= INITIALIZE && j + pos_count < size &&
                        grid[i + ONE_SPACE_OUT][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i - pos_count][j + pos_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern(new_pattern);

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_four_count++;
                        
                        if ( open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( pos_count == OPEN_FOUR && i + ONE_SPACE_OUT < size && j - ONE_SPACE_OUT >= INITIALIZE && i - pos_count >= INITIALIZE && j + pos_count < size &&
                        ( ( grid[i + ONE_SPACE_OUT][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION ) ^ ( grid[i - pos_count][j + pos_count] == EMPTY_INTERSECTION ) ) && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        half_open_four_count++;

                        if ( half_open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( pos_count == OPEN_THREE && i + ONE_SPACE_OUT < size && j - ONE_SPACE_OUT >= INITIALIZE && i - pos_count >= INITIALIZE && j + pos_count < size && pattern_spaces == INITIALIZE &&
                        grid[i + ONE_SPACE_OUT][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i - pos_count][j + pos_count] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( pos_count == OPEN_THREE && i + ONE_SPACE_OUT < size && j - ONE_SPACE_OUT >= INITIALIZE && i - OPEN_FOUR >= INITIALIZE && j + OPEN_FOUR < size && pattern_spaces == 1
                        && grid[i + ONE_SPACE_OUT][j - ONE_SPACE_OUT] == EMPTY_INTERSECTION && grid[i - OPEN_FOUR][j + OPEN_FOUR] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( pos_count == OVERLINE || ( half_open_four_count + open_four_count >= MAX_VIOLATIONS ) ) {

                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return true;

                    } else if ( pos_count == WIN_CONDITION && pos_spaces == INITIALIZE && grid[i - k - ONE_SPACE_OUT][j + k + ONE_SPACE_OUT] != BLACK_STONE ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = BLACK_STONE;
                        return true;
                    }

                } else {
                    break;
                }
            }

            pattern_spaces = INITIALIZE;
            memset( new_pattern, BLANK_COORD, sizeof( new_pattern ) ); // Reset the pattern array

            // Check backward traversal of secondary diagonal of type '\'
            for ( int k = INITIALIZE; k < OVERLINE; k++ ) {

                if ( i + k >= size || j - k < INITIALIZE ) break;

                if ( grid[i + k][j - k] == BLACK_STONE || grid[i + k][j - k] == EMPTY_INTERSECTION ) {

                    if ( grid[i + k][j - k] == BLACK_STONE ) {

                        new_pattern[neg_count][INITIALIZE] = i + k;  // Store x coordinate
                        new_pattern[neg_count][1] = j - k;  // Store y coordinate
                        neg_count++;

                    } else if ( grid[i + k][j - k] == EMPTY_INTERSECTION ) {
                        
                        neg_spaces++;

                        if ( grid[i + k + ONE_SPACE_OUT][j - k - ONE_SPACE_OUT] == BLACK_STONE ) {

                            if ( grid[i + k + TWO_SPACE_OUT][j - k - TWO_SPACE_OUT] == EMPTY_INTERSECTION ) {
                                pattern_spaces++;
                            }

                            continue;

                        } else {
                            break;
                        }
                    }

                    // checking to see if it is a new instance
                    bool is_new_instance = false;
                    for ( int l = INITIALIZE; l < OPEN_FOUR; l++ ) {
                        if ( memcmp( new_pattern[l], the_move, sizeof( the_move ) ) == INITIALIZE ) {
                            is_new_instance = true;
                            break;
                        }
                    }

                    // Check for open fours
                    if ( neg_count == OPEN_FOUR && i - ONE_SPACE_OUT >= INITIALIZE && j + ONE_SPACE_OUT < size && i + neg_count < size && j - neg_count >= INITIALIZE &&
                         grid[i + neg_count][j - neg_count] == EMPTY_INTERSECTION && grid[i - ONE_SPACE_OUT][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_four_count++;

                        if ( open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( neg_count == OPEN_FOUR && i - ONE_SPACE_OUT >= INITIALIZE && j + ONE_SPACE_OUT < size && i + neg_count < size && j - neg_count >= INITIALIZE &&
                        ( ( grid[i + neg_count][j - neg_count] == EMPTY_INTERSECTION ) ^ ( grid[i - ONE_SPACE_OUT][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION ) ) && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        half_open_four_count++;

                        if ( half_open_four_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }

                    } else if ( neg_count == OPEN_THREE && i - ONE_SPACE_OUT >= INITIALIZE && j + ONE_SPACE_OUT < size && i + neg_count < size && j - neg_count >= INITIALIZE && pattern_spaces == INITIALIZE &&
                         grid[i + neg_count][j - neg_count] == EMPTY_INTERSECTION && grid[i - ONE_SPACE_OUT][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( neg_count == OPEN_THREE && i - ONE_SPACE_OUT >= INITIALIZE && j + ONE_SPACE_OUT < size && i + OPEN_FOUR < size && j - OPEN_FOUR >= INITIALIZE && pattern_spaces == 1 &&
                         grid[i + OPEN_FOUR][j - OPEN_FOUR] == EMPTY_INTERSECTION && grid[i - ONE_SPACE_OUT][j + ONE_SPACE_OUT] == EMPTY_INTERSECTION && is_new_instance ) {

                        normalize_pattern( new_pattern );

                        bool is_duplicate = false;
                        for ( int p = INITIALIZE; p <= pattern_count; p++ ) {
                            if ( memcmp( pattern_coords[p], new_pattern, sizeof( new_pattern ) ) == INITIALIZE ) {
                                is_duplicate = true;
                                break;
                            }
                        }

                        if ( !is_duplicate ) {
                            memcpy( pattern_coords[pattern_count], new_pattern, sizeof( new_pattern ) );
                            pattern_count++;
                        } else {
                            break;
                        }

                        open_three_count++;

                        if ( open_three_count >= MAX_VIOLATIONS ) {

                            g->state = GAME_STATE_FORBIDDEN;
                            g->winner = WHITE_STONE;
                            return true;

                        }      
                              
                    } else if ( neg_count == OVERLINE || ( half_open_four_count + open_four_count >= MAX_VIOLATIONS ) ) {

                        g->state = GAME_STATE_FORBIDDEN;
                        g->winner = WHITE_STONE;
                        return true;

                    } else if ( neg_count == WIN_CONDITION && neg_spaces == INITIALIZE && grid[i + k + ONE_SPACE_OUT][j - k - ONE_SPACE_OUT] != BLACK_STONE ) {
                        g->state = GAME_STATE_FINISHED;
                        g->winner = BLACK_STONE;
                        return true;
                    }

                } else {
                    break;
                }
            }

        }
    }

    // Check if the board is full
    if ( board_is_full( g->board ) ) {
        g->state = GAME_STATE_FINISHED;
        return false;
    }

    return true;
}


bool game_place_stone( game* g, unsigned char x, unsigned char y ) {
    unsigned char ( *grid )[g->board->size] = ( unsigned char ( * )[g->board->size] ) g->board->grid;

    if ( grid[y][x] != EMPTY_INTERSECTION ) {
        printf( "There is already a stone at the coordinate you entered, please try again.\n" );
        return false;
    }

    if ( g->moves_count >= g->moves_capacity ) {
        g->moves_capacity *= CAPACITY_INCREASE;
        g->moves = ( move * ) realloc( g->moves, g->moves_capacity * sizeof( move ) );
        if ( !g->moves ) {
            exit( NULL_POINTER_ERR );
        }
    }

    g->moves[g->moves_count].x = x;
    g->moves[g->moves_count].y = y;
    g->moves[g->moves_count].stone = g->stone;
    g->moves_count++;

    board_set( g->board, x, y, g->stone );

    bool result = false;
    if ( g->type == GAME_FREESTYLE ) {
        result = game_freestyle_gomoku( g, x, y );
    } else if ( g->type == GAME_RENJU ) {
        if ( g->stone == WHITE_STONE ) {
            result = game_freestyle_gomoku( g, x, y );
        } else {
            result = game_renju_black( g, x, y );
        }
    }

    if ( result ) {
        g->stone = ( g->stone == BLACK_STONE ) ? WHITE_STONE : BLACK_STONE;
    }

    return true;
}



