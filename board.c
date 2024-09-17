/**
* @file board.c
* @author Sadia Ahmed (sahmed23)
* The file generates the board, prints it, and performs
* other related functions with player move coordinates
*/

#include "io.h"
#include "board.h"
#include "game.h"
#include "error-codes.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

board* board_create( unsigned char size ) 
{

    // Check if size is valid
    if ( size <= INITIALIZE ) {
        exit( BOARD_SIZE_ERR );
    }
    
    // Create the dynamically allocated board
    board *Board = ( board * ) malloc( sizeof( board ) );


    // Assign values to struct fields
    Board->size = size;
    Board->grid = ( unsigned char* ) malloc( size * size * sizeof ( unsigned char ) );

    // Initialize to INITIALIZE
    memset( Board->grid, INITIALIZE, size * size * sizeof( unsigned char ) );

    return Board;
}

void board_delete( board* b ) 
{
    
    // Check if b is null
    if ( !b ) {
        exit (NULL_POINTER_ERR);
    }

    if ( b->grid ) {
        free( b->grid );
        b->grid = NULL;
    }
    free( b );
    b = NULL;
}



void board_print( board* b, bool in_place ) 
{   
    // Casting the board grid
    unsigned char ( *grid )[b->size] = ( unsigned char ( * )[b->size] ) b->grid;

    if ( in_place ) {
        clear( );
    }

    // Print the board with rows and columns
    for ( int i = b->size - 1; i >= INITIALIZE; i-- ) {
        printf( "%2d ", i + 1 );
        for ( int j = INITIALIZE; j < b->size; j++ ) {
            if ( grid[i][j] == WHITE_STONE ) {
                printf( "\u25CB" );
            } else if ( grid[i][j] == BLACK_STONE ) {
                printf( "\u25CF" );
            } else {
                printf( "+" );
            }

            if ( j < b->size - 1 ) {
                printf( "-" );
            }
        }
        printf( "\n" );
    }

    for ( int j = INITIALIZE; j < b->size; j++ ) {

        if ( j == INITIALIZE ) {
            printf( "   %c", 'A' + j );
        // if printing the last letter in the line
        } else if ( j == b->size - 1 ) {
            printf( " %c\n", 'A' + j );
        // if printing another iteration
        } else {
            printf( " %c", 'A' + j );
        }

    }

}

unsigned char board_formal_coord( board* b, unsigned char x, unsigned char y, char* formal_coord ) 
{

    if ( x < INITIALIZE || x > b->size ) {
        exit( COORDINATE_ERR );
    }

    if ( y < INITIALIZE || y > b->size ) {
        exit( COORDINATE_ERR );
    }
    
    x = x + 'A';
    y = y + 1;

    sprintf( formal_coord, "%c%d", x, y );

    
    return SUCCESS;

}


unsigned char board_coord( board* b, const char* formal_coord, unsigned char* x, unsigned char* y )
{
    
    // store the letter coord
    char letter;

    // store the num coord
    int num;

    // Use sscanf to parse the formal_coord string
    if ( sscanf( formal_coord, "%c%d", &letter, &num ) != 2 ) { 
        return FORMAL_COORDINATE_ERR;
    }

    // Convert letter to row index (x)
    *x = letter - 'A';

    // Convert num to column index (y)
    *y = num - 1;

    // Validate x and y coordinates
    if ( *x < INITIALIZE || *x >= b->size || *y < INITIALIZE || *y >= b->size ) {
        return FORMAL_COORDINATE_ERR;
    }

    return SUCCESS;
}


unsigned char board_get( board* b, unsigned char x, unsigned char y ) 
{
    return ( ( unsigned char( * )[ b->size ] ) b->grid )[y][x];
}


void board_set( board* b, unsigned char x, unsigned char y, unsigned char stone ) 
{
    if ( stone != BLACK_STONE && stone != WHITE_STONE ) {
        exit( STONE_TYPE_ERR );
    }

    ( ( unsigned char( * )[ b->size ] ) b->grid )[y][x] = stone;
}


bool board_is_full( board* b )
{
    unsigned char ( *grid )[b->size] = ( unsigned char ( * )[b->size] ) b->grid;

    for ( int i = INITIALIZE; i < b->size; i++ ) {
        for ( int j = INITIALIZE; j < b->size; j++ ) {

            if ( grid[i][j] != WHITE_STONE && grid[i][j] != BLACK_STONE ) {
                return false;
            }
        }
    }

    return true;
}