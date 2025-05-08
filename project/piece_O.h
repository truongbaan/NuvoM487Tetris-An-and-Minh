#ifndef PIECE_O_H
#define PIECE_O_H
#include "tetris_config.h"

#define X 4

static const int PIECE_O[4][BLOCK_SIZE][BLOCK_SIZE] = {
    // rotation 0
    {{X, X, 0, 0},
     {X, X, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},
    // rotation 1
    {{X, X, 0, 0},
     {X, X, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},
    // rotation 2
    {{X, X, 0, 0},
     {X, X, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},
    // rotation 3
    {{X, X, 0, 0},
     {X, X, 0, 0},
     {0, 0, 0, 0},
     {0, 0, 0, 0}}
};
#undef X
#endif // PIECE_O_H