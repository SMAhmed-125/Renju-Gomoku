/**
* @file io.c
* @author Sadia Ahmed (sahmed23)
* This file imports data from a given file
* and also exports the game data when the game is stopped or finished
*/


#include "io.h"
#include "board.h"
#include "game.h"
#include "error-codes.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

game* game_import( const char* path ) 
{

    // Reading in the file
    FILE* fp = fopen( path, "r" );

    if ( !fp ) {
        exit( FILE_INPUT_ERR );
    }
    
    // Storing in the magic number at beginning to compare
    char magic_num[3] = {};

    if ( fscanf( fp, "%2s", magic_num ) != 1 || strcmp( magic_num, "GA" ) != 0) {
        fclose( fp );
        exit( FILE_INPUT_ERR );
    }

    // Storing the board size from file
    int size = INITIALIZE;

    if ( fscanf( fp, "%d", &size ) != 1 ) {
        fclose( fp );
        exit( FILE_INPUT_ERR );
    }

    // Casting the board size
    unsigned char unsigned_size = ( unsigned char ) size;

    if ( unsigned_size != 15 && unsigned_size != 17 && unsigned_size != 19 ) {
        fclose( fp );
        exit( BOARD_SIZE_ERR );
    }
    
    // Storing the game type from file
    int type = INITIALIZE;

    if ( fscanf( fp, "%d", &type ) != 1 ) {
        fclose( fp );
        exit( FILE_INPUT_ERR );
    }

    // Casting the game type from file
    unsigned char unsigned_type = ( unsigned char ) type;

    if ( unsigned_type != GAME_FREESTYLE && unsigned_type != GAME_RENJU ) {
        fclose( fp );
        exit( FILE_INPUT_ERR );       
    }

    game *Game = game_create( unsigned_size, unsigned_type );
    
    // Storing the game state from file
    int state = INITIALIZE;

    if ( fscanf( fp, "%d", &state) != 1 ) {
        fclose( fp );
        exit( FILE_INPUT_ERR );
    }

    // Casting the game state from file
    unsigned char unsigned_state = ( unsigned char ) state;

    if ( unsigned_state != GAME_STATE_FORBIDDEN && unsigned_state != GAME_STATE_FINISHED && unsigned_state != GAME_STATE_STOPPED ) {
        fclose( fp );
        exit( FILE_INPUT_ERR );       
    } else {
        Game->state = unsigned_state;
    }


    // Storing the winner from file
    int winner = INITIALIZE;

    if ( fscanf( fp, "%d", &winner ) != 1 ) {
        fclose( fp );
        exit( FILE_INPUT_ERR );
    }
    
    // Casting the winner from file
    unsigned char unsigned_winner = ( unsigned char ) winner;
    
    
    if ( unsigned_winner != BLACK_STONE && unsigned_winner != WHITE_STONE && unsigned_winner != 0 ) {
         fclose( fp );
         exit( FILE_INPUT_ERR );       
    } else {
        Game->winner = unsigned_winner;
    }
    
    // Storing the x coord of the move
    char x_coord = INITIALIZE;

    // Storing the y coord of the move
    int y_coord = INITIALIZE;

    // Storing how many moves have been read in so far
    int count = INITIALIZE;

    while ( fscanf(fp, " %c%d", &x_coord, &y_coord ) != EOF ) {
        if ( count >= Game->moves_capacity ) {
            Game->moves_capacity *= CAPACITY_INCREASE;
            Game->moves = realloc( Game->moves, Game->moves_capacity * sizeof( move ) );
            if ( !Game->moves ) {
                fclose( fp );
                exit( FILE_INPUT_ERR );
            }
        }

        Game->moves[count].x = x_coord - 'A';
        Game->moves[count].y = y_coord - 1;
        
        Game->moves[count].stone = ( count % 2 == 0 ) ? BLACK_STONE : WHITE_STONE;

        count++;
    }

    Game->moves_count = count;

    fclose( fp );

    return Game;
}


void game_export( game* g, const char* path ) 
{

    // Creating a pointer to the file to write to
    FILE* fp = fopen( path, "w" );

    if ( fp == NULL ) {
        exit( FILE_OUTPUT_ERR );
    }

    // Other info
    fprintf( fp, "GA\n" );
    fprintf( fp, "%d\n", g->board->size );
    fprintf( fp, "%d\n", g->type );
    fprintf( fp, "%d\n", g->state );
    fprintf( fp, "%d\n", g->winner );

    // Moves
    for ( int i = INITIALIZE; i < g->moves_count; i++ ) {
        fprintf( fp, "%c%d\n", 'A' + g->moves[i].x, g->moves[i].y + 1 );
    }

    fclose( fp );
}


