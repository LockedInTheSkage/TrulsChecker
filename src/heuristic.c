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
        if (board->turn == White) {
            score += pieceScore(board->squares[i]);
        } else if (board->turn == Black) {
            score -= pieceScore(board->squares[i]);
        }
    }
    return score;
}

int pieceScore(Piece p) {
    switch (p) {
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