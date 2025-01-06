#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include "Heuristic.h"

#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>


#define OUR(t) (cb->pieces[GET_PIECE(t, cb->turn)])                                     // Bitboard representing our pieces of type t
#define THEIR(t) (cb->pieces[GET_PIECE(t, !cb->turn)])                                  // Bitboard representing their pieces of type t
const int boardPriority[6][64] = {
    // Pawn
    {  0,  0,  0,  0,  0,  0,  0,  0,
      50, 50, 50, 50, 50, 50, 50, 50,
      10, 10, 20, 30, 30, 20, 10, 10,
       5,  5, 10, 25, 25, 10,  5,  5,
       0,  0,  0, 20, 20,  0,  0,  0,
       5, -5,-10,  0,  0,-10, -5,  5,
       5, 10, 10,-20,-20, 10, 10,  5,
       0,  0,  0,  0,  0,  0,  0,  0 },
    // Knight
    {-50,-40,-30,-30,-30,-30,-40,-50,
     -40,-20,  0,  0,  0,  0,-20,-40,
     -30,  0, 10, 15, 15, 10,  0,-30,
     -30,  5, 15, 20, 20, 15,  5,-30,
     -30,  0, 15, 20, 20, 15,  0,-30,
     -30,  5, 10, 15, 15, 10,  5,-30,
     -40,-20,  0,  5,  5,  0,-20,-40,
     -50,-40,-30,-30,-30,-30,-40,-50 },
    // Bishop
    {-20,-10,-10,-10,-10,-10,-10,-20,
     -10,  0,  0,  0,  0,  0,  0,-10,
     -10,  0,  5, 10, 10,  5,  0,-10,
     -10,  5,  5, 10, 10,  5,  5,-10,
     -10,  0, 10, 10, 10, 10,  0,-10,
       0, 10, 10, 10, 10, 10, 10,  0,
     -10,  5,  0,  0,  0,  0,  5,-10,
     -20,-10,-10,-10,-10,-10,-10,-20 },
    // Rook
    {  0,  0,  0,  0,  0,  0,  0,  0,
       5, 10, 10, 10, 10, 10, 10,  5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
      -5,  0,  0,  0,  0,  0,  0, -5,
       0,  0,  0,  5,  5,  0,  0,  0 },
    // Queen
    {-20,-10,-10, -5, -5,-10,-10,-20,
     -10,  0,  0,  0,  0,  0,  0,-10,
     -10,  0,  5,  5,  5,  5,  0,-10,
      -5,  0,  5,  5,  5,  5,  0, -5,
       0,  0,  5,  5,  5,  5,  0, -5,
     -10,  5,  5,  5,  5,  5,  0,-10,
     -10,  0,  5,  0,  0,  0,  0,-10,
     -20,-10,-10, -5, -5,-10,-10,-20 },
    // King (early game)
    {-30,-40,-40,-50,-50,-40,-40,-30,
     -30,-40,-40,-50,-50,-40,-40,-30,
     -30,-40,-40,-50,-50,-40,-40,-30,
     -30,-40,-40,-50,-50,-40,-40,-30,
     -20,-30,-30,-40,-40,-30,-30,-20,
     -10,-20,-20,-20,-20,-20,-20,-10,
      20, 20,  0,  0,  0,  0, 20, 20,
      20, 30, 10,  0,  0, 10, 30, 20 }
};

// Heuristic evaluation function
int evaluateBoard(ChessBoard *cb) {
    int score = 0;

    for (int pieceType = 0; pieceType < 6; pieceType++) {
        BitBoard ourPieces = OUR(pieceType);
        BitBoard theirPieces = THEIR(pieceType);

        while (ourPieces) {
            int square = BitBoardPopLSB(&ourPieces);
            score += boardPriority[pieceType][63-square];
        }

        while (theirPieces) {
            int square = BitBoardPopLSB(&theirPieces);
            score -= boardPriority[pieceType][square];
        }
    }

    return score;
}


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
        score += BitBoardCountBits(pieces) * pieceScore(i) * (2*GET_COLOR(i)-1)*10000;
    }
    
    
    score += evaluateBoard(board);

    return score;
}

int pieceScore(Piece p) {
    switch (GET_TYPE(p)) {
        case Pawn:
            return 1;
        case Knight:
            return 3;
        case Bishop:
            return 3;
        case Rook:
            return 5;
        case Queen:
            return 9;
        default:
            return 0;
    }
}