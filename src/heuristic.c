#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include "Heuristic.h"

#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

int heuristic(LookupTable l, ChessBoard *board) {
    int score = 0;

    

    for (int i = 0; i < PIECE_SIZE; i++) {
        BitBoard pieces = board->pieces[i];
        if (GET_TYPE(i) == King && BitBoardCountBits(pieces) == 0) {
            if (GET_COLOR(i) == White) {
                return INT_MAX;
            } else {
                return INT_MIN;
            }
        }
        score += BitBoardCountBits(pieces) * pieceScore(i) * (2*GET_COLOR(i)-1);
    }
    
    
    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, board, branches);
    board->turn = !board->turn;
    Branch otherBranches[BRANCHES_SIZE];
    int otherBranchesSize = BranchFill(l, board, otherBranches);
    board->turn = !board->turn;

    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);
    int otherMovesSize = BranchExtract(otherBranches, otherBranchesSize, moves);
    if (board->turn == White) {
        score += movesSize;
        score -= otherMovesSize;
    } else {
        score -= movesSize;
        score += otherMovesSize;
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
        default:
            return 0;
    }
}