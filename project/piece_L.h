#ifndef PIECE_L_H
#define PIECE_L_H
#include "tetris_config.h"

#define X 3

static const int PIECE_L[4][BLOCK_SIZE][BLOCK_SIZE] = {
    // rotation 0
    {{0, 0, X, 0},
     {X, X, X, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},
    // rotation 1
    {{0, X, 0, 0},
     {0, X, 0, 0},
     {0, X, X, 0},
     {0, 0, 0, 0}},
    // rotation 2
    {{0, 0, 0, 0},
     {X, X, X, 0},
     {X, 0, 0, 0},
     {0, 0, 0, 0}},
    // rotation 3
    {{X, X, 0, 0},
     {0, X, 0, 0},
     {0, X, 0, 0},
     {0, 0, 0, 0}}
};
#undef X
#endif // PIECE_L_H