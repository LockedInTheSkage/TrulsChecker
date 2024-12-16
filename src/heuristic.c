#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include "Heuristic.h"

#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

int heuristic(ChessBoard *board) {
    int score = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        Piece p = board->squares[i];
        if (GET_COLOR(p) == White) {
            score += pieceScore(p);
        } else{
            score -= pieceScore(p);
        }
    }
    return score;
}

int pieceScore(Piece p) {
    switch (GET_TYPE(p)) {
        case Pawn:
            return 100;
        case Knight:
            return 300;
        case Bishop:
            return 300;
        case Rook:
            return 500;
        case Queen:
            return 900;
        case King:
            return 10000;
        default:
            return 0;
    }
}