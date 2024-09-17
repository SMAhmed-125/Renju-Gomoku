/**
* @file gomoku.c
* @author Sadia Ahmed (sahmed23)
* This file executes a new or unfinished gomoku game
* options can be input to save the game and input a unfinished game
*/


#include "io.h"
#include "board.h"
#include "game.h"
#include "error-codes.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>



/**
 * Initiates playing the game by importing, taking in user input, etc.
 * The main function of the gomoku game style option.
 * @param argc the number of terminal arguments defined by user
 * @param the array of arguments themselves 
 * @return the exit status or the error status
 */
int main( int argc, char *argv[] ) 
{
    
    // Storing the length of the filename for comparison
    size_t str_length_one = strlen( argv[0] );

    // Storing the part of the file name to compare to
    char src_path[DEFAULT_STRING_LENGTH] = "gomoku";

    // Storing the length of the file name to compare to
    size_t str_length_two = strlen( src_path );

    if ( strncmp( argv[0] + str_length_one - str_length_two , src_path, str_length_two ) != 0 ) {
        exit( FILE_INPUT_ERR );
    }

    game * Game = NULL;

    if ( argc == 1 ) {
        Game = game_create( 15, GAME_FREESTYLE );
        game_loop( Game );
        return EXIT_SUCCESS;
    }
    
    // flag for -r option
    bool r_flag = false;

    // flag for -b option
    bool b_flag = false;

    // flag for -o option
    bool o_flag = false;

    // Storing the output path
    char output_path[LONG_DEFAULT_STRING_LENGTH];

    if ( argc > 1 ) {
        for( int i = 0; i < argc; i++ ) {

            if ( strcmp( "-o", argv[i] ) == 0 ) {
                o_flag = true;
                strcpy( output_path, argv[i + 1] );
                
                // Storing the size of the output path
                size_t str_length_one = strlen( output_path );

                // Creating the string of the file extension to compare to
                char file_ext[4] = "gmk";

                // Storing the size of that file ext
                size_t str_length_two = strlen( file_ext );

                if ( strncmp( output_path + str_length_one - str_length_two , file_ext, str_length_two ) != 0 ) {
                    exit( FILE_OUTPUT_ERR );
                }

            } else if ( strcmp( "-b", argv[i] ) == 0 ) {

                if ( r_flag ) {
                    b_flag = true;
                    break;
                }

                // Converting the board size from string
                int size_param = atoi( argv[i + 1] );

                if ( size_param == 0 ) {
                    printf( "usage: ./%s ", src_path );
                    printf( "[-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n" );
                    printf( "       -r and -b conflicts with each other\n" );
                    exit( ARGUMENT_ERR );
                }

                if ( size_param != 15 && size_param != 17 && size_param != 19 ) {
                    exit( BOARD_SIZE_ERR );
                }
                
                // Casting the size of the board
                unsigned char size = ( unsigned char ) size_param;

                Game = game_create( size, GAME_FREESTYLE );
                game_loop( Game );
                b_flag = true;

            } else if ( strcmp( "-r", argv[i] ) == 0 ) {

                if ( b_flag ) {
                    r_flag = true;
                    break;
                }

                Game = game_import( argv[i + 1] );

                if ( Game->type != GAME_FREESTYLE && Game->type != GAME_RENJU ) {
                    exit( RESUME_ERR );
                } 

                game_resume( ( game * ) Game );
                r_flag = true;
            } 

        }

        if ( r_flag && b_flag ) {
            printf( "usage: .//%s ", src_path );
            printf( "[-r <unfinished-match.gmk>] [-o <saved-match.gmk>] [-b <15|17|19>]\n" );
            printf( "       -r and -b conflicts with each other\n" );

            exit( ARGUMENT_ERR );
        }

        if ( o_flag && !r_flag && !b_flag ) {
            Game = game_create( 15, GAME_FREESTYLE );
            game_loop( Game );
            game_export( ( game * ) Game, output_path );
        } else if ( o_flag && ( r_flag || b_flag ) ) {
            game_export( ( game * ) Game, output_path );
        }


    }

    board_delete( Game->board );
    game_delete( ( game * ) Game );

    return EXIT_SUCCESS;
}