#ifndef PIECE_I_H
#define PIECE_I_H
#include "tetris_config.h"

#define X 1

static const int PIECE_I[4][4][4] = {
    // rotation 0
    {{X,X,X,X},
     {0,0,0,0},
     {0,0,0,0},
     {0,0,0,0}},
    // rotation 1
    {{0,0,X,0},
     {0,0,X,0},
     {0,0,X,0},
     {0,0,X,0}},
    // rotation 2
    {{0,0,0,0},
     {0,0,0,0},
     {X,X,X,X},
     {0,0,0,0}},
    // rotation 3
    {{0,X,0,0},
     {0,X,0,0},
     {0,X,0,0},
     {0,X,0,0}}
};
#undef X
#endif // PIECE_I_H
