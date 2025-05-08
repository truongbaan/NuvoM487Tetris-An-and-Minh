#ifndef GAME_fUNCTION
#define GAME_fUNCTION
#include "tetris_config.h"
#include<stdbool.h>

//place which hold all pieces type
extern int pieces[7][4][BLOCK_SIZE][BLOCK_SIZE];
void init_pieces(int pieces[7][4][BLOCK_SIZE][BLOCK_SIZE]);

// use to init the board
void createBoard(int board[HEIGHT][WIDTH]); 

// Function to generate a random Tetris piece
int generatePiece();

//use to print board while playing
void printBoard(int board[HEIGHT][WIDTH], int score, int piece, int rotation, int posX, int posY);

//check if movment is possible
int canMove(int board[HEIGHT][WIDTH], int piece, int rotation, int x, int y);

//movement for the piece
void movePieceDown(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation);
void movePieceLeft(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation);
void movePieceRight(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation);
void rotatePiece(int board[HEIGHT][WIDTH], int *piece, int *rotation, int *x, int *y);

//check for completed line
int removeLines(int board[HEIGHT][WIDTH]);

//check if end game
int isGameOver(int board[HEIGHT][WIDTH]);

//config input
bool key_input(int board[HEIGHT][WIDTH], int piece, int *rotation, int *x, int *y);

#endif // gamefunction
