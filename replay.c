/**
* @file replay.c
* @author Sadia Ahmed (sahmed23)
* This file replays finished games
*/

#include "io.h"
#include "board.h"
#include "game.h"
#include "error-codes.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>


/**
 * Initiates replaying the game by importing, executing the moves, etc.
 * The main function of the replay option.
 * @param argc the number of terminal arguments defined by user
 * @param the array of arguments themselves 
 * @return the exit status or the error status
 */
int main( int argc, char *argv[] ) 
{
    
    size_t str_length_one = strlen( argv[0] );

    char src_path[DEFAULT_STRING_LENGTH] = "replay";

    size_t str_length_two = strlen( src_path );

    if ( strncmp( argv[0] + str_length_one - str_length_two , src_path, str_length_two ) != 0 ) {
        exit( FILE_INPUT_ERR );
    }

    if ( argc != 2 ) {
        printf( "usage: %s <saved-match.gmk>\n", argv[0] );
        exit( ARGUMENT_ERR );
    }

    game *Game = game_import( argv[1] );
    if ( !Game ) {
        printf( "Failed to import game from %s\n", argv[1] );
        exit( ARGUMENT_ERR );
    }

    game_replay( Game );
    game_delete( Game );

    return EXIT_SUCCESS;
}