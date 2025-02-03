#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include "Zobrist.h"
#include "Dictionary.h"
#include "Heuristic.h"


#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

#define PIECE_FACTOR 100
#define ATTACK_FACTOR 2
#define CASTLING_FACTOR 25


#define BACK_RANK(c) (BitBoard)((c == White) ? SOUTH_EDGE : NORTH_EDGE)                // BitBoard representing the back rank given a color

#define WHITE_PIECE(t) (board->pieces[GET_PIECE(t, White)])                                     // Bitboard representing our pieces of type t
#define BLACK_PIECE(t) (board->pieces[GET_PIECE(t, Black)])                                  // Bitboard representing their pieces of type t

#define WHITE_PIECES (WHITE_PIECE(Pawn) | WHITE_PIECE(Knight) | WHITE_PIECE(Bishop) | WHITE_PIECE(Rook) | WHITE_PIECE(Queen) | WHITE_PIECE(King)) // Bitboard of all our pieces
#define BLACK_PIECES (BLACK_PIECE(Pawn) | BLACK_PIECE(Knight) | BLACK_PIECE(Bishop) | BLACK_PIECE(Rook) | BLACK_PIECE(Queen) | BLACK_PIECE(King)) // Bitboard of all their pieces


int heuristic(LookupTable l, ChessBoard *board, Dictionary *dict) {
    int score = 0;
    if (dict->zobrist != NULL) {
        int dictScore = betterDictScore(board, dict);
        if (dictScore) {
            return dictScore;
        }
    }
    

    

    for (int i = 0; i < PIECE_SIZE; i++) {
        BitBoard pieces = board->pieces[i];
        if (GET_TYPE(i) == King && BitBoardCountBits(pieces) == 0) {
            if (GET_COLOR(i) == White) {
                return INT_MAX;
            } else {
                return INT_MIN;
            }
        }
        score += BitBoardCountBits(pieces) * pieceScore(i) * (2*GET_COLOR(i)-1)*PIECE_FACTOR;
        if (GET_TYPE(i) == King) {
            continue;
        }
        for (int j = 0; j < 64; j++) {
            if (pieces & (1ULL << j)) {
                BitBoard targets;
                Type piece_type = GET_TYPE(i);
                if (GET_COLOR(i) == White) {
                    targets = BLACK_PIECES;
                } else {
                    targets = WHITE_PIECES;
                }
                if (GET_TYPE(i)==Pawn){
                    BitBoard attacks;
                    if (GET_COLOR(i)==White){
                        attacks = (BitBoardShiftNW(BitBoardSetBit(EMPTY_BOARD, j)) | BitBoardShiftNE(BitBoardSetBit(EMPTY_BOARD, j))) & targets;
                    }else{
                        attacks = (BitBoardShiftSW(BitBoardSetBit(EMPTY_BOARD, j)) | BitBoardShiftSE(BitBoardSetBit(EMPTY_BOARD, j))) & targets;
                    }
                    score += BitBoardCountBits(attacks) * pieceScore(i) * (2*GET_COLOR(i)-1)*ATTACK_FACTOR;
                }else{
                    score += BitBoardCountBits(LookupTableAttacks(l, j, piece_type, targets)) * pieceScore(i) * (2*GET_COLOR(i)-1)*ATTACK_FACTOR;
                }
                
                
            }
        }
    }
    
    score -= BitBoardCountBits(board->castling & BACK_RANK(White))*CASTLING_FACTOR;
    score += BitBoardCountBits(board->castling & BACK_RANK(Black))*CASTLING_FACTOR;
    
    
    if (dict->zobrist != NULL) {
        install_board(dict, board, score, board->depth);
    }

    return score;
}

int betterDictScore(ChessBoard *board, Dictionary *dict){
    nlist *np = lookup_board(dict, board);
    if (np != NULL && np->depth >= board->depth) {
        return np->score;
    }
    return 0;
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
            return 1;
    }
}

