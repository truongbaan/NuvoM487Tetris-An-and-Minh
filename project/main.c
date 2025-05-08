#include "tetris_config.h"
#include "function.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include<stdbool.h>


//sleep modified base on platform
#ifdef _WIN32
  #include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
  #include <unistd.h>
#else
  #error "Unsupported platform for msleep()"
#endif

static void msleep(int milliseconds) {
  #ifdef _WIN32
    Sleep(milliseconds);
  #else
    // usleep takes microseconds, so multiply by 1000
    usleep((useconds_t)milliseconds * 1000);
  #endif
}


int main() {
    srand((unsigned)time(NULL)); 

    int board[HEIGHT][WIDTH];
    init_pieces(pieces);
    createBoard(board);

    int piece    = generatePiece();
    int rotation = 0;
    int x        = WIDTH/2 - 2;
    int y        = 0;
    int score    = 0;
    int gameSpeed    = 150;
    int linesRemoved = 0;

    while (!isGameOver(board)) {
        printBoard(board, score, piece, rotation, x, y);

        // handle input; if ESC, break out and end game
        if (key_input(board, piece, &rotation, &x, &y)) {
            printf("Exiting game...\n");
            return 0;
        }

        // auto moving down
        movePieceDown(board, &x, &y, piece, rotation);
        msleep(gameSpeed);

        // clear lines & update score/speed
        linesRemoved = removeLines(board);
        if (linesRemoved) {
            score += linesRemoved * 100;
            gameSpeed = (gameSpeed > 100) ? gameSpeed - 50 : 100;
        }

        // if landed, spawn new piece
        if (!canMove(board, piece, rotation, x, y)) {
            piece    = generatePiece();
            rotation = 0;
            x        = WIDTH/2 - 2;
            y        = 0;
            if (isGameOver(board)) {
                printBoard(board, score, piece, rotation, x, y);
                printf("Game Over!\n");
                msleep(2000);
                break;
            }
        }
    }

    printf("Final Score: %d\n", score);
    return 0;
}