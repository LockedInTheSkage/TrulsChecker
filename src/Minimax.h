#ifndef MINIMAX_H
#define MINIMAX_H

#include <stdbool.h>
#include <stdint.h>
#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"
#include "Dictionary.h"

int minimax(
    LookupTable l,
    ChessBoard *board,
    Dictionary *dict,
    int alpha,
    int beta,
    int depth,
    bool maximizingPlayer
);

Move findBestMove(
    LookupTable l,
    ChessBoard *board,
    Dictionary *dict,
    int depth,
    bool verbose
);

#endif
