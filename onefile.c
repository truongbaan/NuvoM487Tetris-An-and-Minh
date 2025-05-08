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

//screen setting: 240 , 320. scale down to 15, 20 (divive 16)
// Game settings
#define WIDTH 15
#define HEIGHT 20
#define BLOCK_SIZE 4 // Each Tetris piece is made of 4 blocks

// Tetris piece definitions
// 7 piece, each piece is 4x4 block big, 4 rotation
//if you want to increase block_size, you would need to redraw all of the piece too
int pieces[7][4][4][4] = { 
    // I-piece : 1
    {{{1, 1, 1, 1},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0}},

     {{0, 0, 0, 0},
      {0, 0, 0, 0},
      {1, 1, 1, 1},
      {0, 0, 0, 0}},

     {{0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 0, 0}}},

    // J-piece : 2
    {{{2, 0, 0, 0},
      {2, 2, 2, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 2, 2, 0},
      {0, 2, 0, 0},
      {0, 2, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {2, 2, 2, 0},
      {0, 0, 2, 0},
      {0, 0, 0, 0}},

     {{0, 2, 0, 0},
      {0, 2, 0, 0},
      {2, 2, 0, 0},
      {0, 0, 0, 0}}},

    // Z-piece : 3
    {{{3, 3, 0, 0},
      {0, 3, 3, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 3, 0},
      {0, 3, 3, 0},
      {0, 3, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {3, 3, 0, 0},
      {0, 3, 3, 0},
      {0, 0, 0, 0}},

     {{0, 3, 0, 0},
      {3, 3, 0, 0},
      {3, 0, 0, 0},
      {0, 0, 0, 0}}},

    // L-piece : 4
    {{{0, 0, 4, 0},
      {4, 4, 4, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 4, 4, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {4, 4, 4, 0},
      {4, 0, 0, 0},
      {0, 0, 0, 0}},

     {{4, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 4, 0, 0},
      {0, 0, 0, 0}}},

    // O-piece : 5
    {{{5, 5, 0, 0},
      {5, 5, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{5, 5, 0, 0},
      {5, 5, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{5, 5, 0, 0},
      {5, 5, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{5, 5, 0, 0},
      {5, 5, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}}},

    // S-piece : 6
    {{{0, 6, 6, 0},
      {6, 6, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 6, 0, 0},
      {0, 6, 6, 0},
      {0, 0, 6, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {0, 6, 6, 0},
      {6, 6, 0, 0},
      {0, 0, 0, 0}},

     {{6, 0, 0, 0},
      {6, 6, 0, 0},
      {0, 6, 0, 0},
      {0, 0, 0, 0}}},

    // T-piece : 7
    {{{0, 7, 0, 0},
      {7, 7, 7, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}},

     {{0, 7, 0, 0},
      {0, 7, 7, 0},
      {0, 7, 0, 0},
      {0, 0, 0, 0}},

     {{0, 0, 0, 0},
      {7, 7, 7, 0},
      {0, 7, 0, 0},
      {0, 0, 0, 0}},

     {{0, 7, 0, 0},
      {7, 7, 0, 0},
      {0, 7, 0, 0},
      {0, 0, 0, 0}}}
    
};

// func to init the board when first start
void createBoard(int board[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            board[i][j] = 0; // 0 == empty cell
        }
    }
}

// func for print board
void printBoard(int board[HEIGHT][WIDTH], int score, int piece, int rotation, int posX, int posY)
{
    system("cls");
    printf("Tetris Game   Score: %d\n\n", score);

    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            if (board[row][col]) {// print out the block that has been occupied
                printf("[%d]", board[row][col]);
            }
            else {
                // Then check if the active piece covers this cell
                int localX = col - posX;
                int localY = row - posY;

                if ( localX >= 0 && localX < BLOCK_SIZE && localY >= 0 && localY < BLOCK_SIZE && pieces[piece][rotation][localY][localX] ) {
                    // draw falling piece
                    printf("[%d]", (piece + 1)); // use number to define color later
                }
                else {
                    // empty cell
                    printf("   "); // 3 width long due to 2 '[' and 1 digit (would be incrase base on the how big the screen and block size be)
                }
            }
        }
        printf("\n");
    }

    // bottom border
    for(int j = 0; j < WIDTH; j++) {
        printf("---");
    }
    printf("\n");
}

// Function to generate a random Tetris piece
int generatePiece() {
    return rand() % 7;
}

// Function to check if a piece can move to a new position
int canMove(int board[HEIGHT][WIDTH], int piece, int rotation, int x, int y) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            if (pieces[piece][rotation][i][j]) {
                int newX = x + j;
                int newY = y + i;

                if (newX < 0 || newX >= WIDTH || newY >= HEIGHT || board[newY][newX])
                    return 0; // Out of bounds or collision
                if(newY < 0) //ignore checking the top of the board
                    continue;
            }
        }
    }
    return 1; // The piece can move
}

// movement functions
void movePieceDown(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    *y = *y + 1;
    if (!canMove(board, piece, rotation, *x, *y)) {
        *y = *y - 1; // Move it back to the original position if it can't move down
        // Lock the piece on the board
        for (int i = 0; i < BLOCK_SIZE; i++) {
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (pieces[piece][rotation][i][j]) {
                    board[*y + i][*x + j] = (piece + 1);
                }
            }
        }
    }
}

void movePieceLeft(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    *x = *x - 1;
    if (!canMove(board, piece, rotation, *x, *y)) {
        *x = *x + 1;
    }
}

void movePieceRight(int board[HEIGHT][WIDTH], int *x, int *y, int piece, int rotation) {
    *x = *x + 1;
    if (!canMove(board, piece, rotation, *x, *y)) {
        *x = *x - 1;
    }
}

void rotatePiece(int board[HEIGHT][WIDTH], int *piece, int *rotation, int *x, int *y) {
    int newRotation = (*rotation + 1) % 4;
    if (canMove(board, *piece, newRotation, *x, *y)) {
        *rotation = newRotation;
    }
}

// Function to check for completed lines and remove them
int removeLines(int board[HEIGHT][WIDTH]) {
    int linesRemoved = 0;
    for (int i = HEIGHT - 1; i >= 0; i--) {
        int lineComplete = 1;
        for (int j = 0; j < WIDTH; j++) {
            if (!board[i][j]) {
                lineComplete = 0;
                break;
            }
        }
        if (lineComplete) {
            linesRemoved++;
            // Shift all rows above down by one row
            for (int k = i - 1; k >= 0; k--) {
                for (int l = 0; l < WIDTH; l++) {
                    board[k + 1][l] = board[k][l];
                }
            }
            // Clear the top row
            for (int l = 0; l < WIDTH; l++) {
                board[0][l] = 0;
            }
        }
    }
    return linesRemoved;
}

// Function to check for game over
int isGameOver(int board[HEIGHT][WIDTH]) {
    for (int j = 0; j < WIDTH; j++) {
        if (board[0][j])
            return 1; // Game over if there is a block in the top row
    }
    return 0;
}

// Returns true if ESC was pressed (signal to exit game loop)
bool key_input(int board[HEIGHT][WIDTH],int piece,int *rotation, int *x, int *y) {
    if (!_kbhit()) 
        return false;

    char key = _getch();
    switch (key) {
        case 'a': case 'A':
            movePieceLeft(board, x, y, piece, *rotation);
            break;
        case 'd': case 'D':
            movePieceRight(board, x, y, piece, *rotation);
            break;
        case 's': case 'S':
            movePieceDown(board, x, y, piece, *rotation);
            break;
        case 'w': case 'W':
            rotatePiece(board, &piece, rotation, x, y);
            break;
        case 27:  // ESC
            return true;
        default:
            break;
    }
    return false;
}

int main() {
    srand((unsigned)time(NULL)); 

    int board[HEIGHT][WIDTH];
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