#ifndef PIECE_Z_H
#define PIECE_Z_H
#include "tetris_config.h"

#define X 7

static const int PIECE_Z[4][BLOCK_SIZE][BLOCK_SIZE] = {
    // rotation 0
    {{X, X, 0, 0},
     {0, X, X, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},
    // rotation 1
    {{0, 0, X, 0},
     {0, X, X, 0},
     {0, X, 0, 0},
     {0, 0, 0, 0}},
    // rotation 2
    {{0, 0, 0, 0},
     {X, X, 0, 0},
     {0, X, X, 0},
     {0, 0, 0, 0}},
    // rotation 3
    {{0, X, 0, 0},
     {X, X, 0, 0},
     {X, 0, 0, 0},
     {0, 0, 0, 0}}
};
#undef X
#endif // PIECE_Z_H