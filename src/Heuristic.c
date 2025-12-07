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
#define ATTACK_FACTOR 3
#define CASTLING_FACTOR 25


#define BACK_RANK(c) (BitBoard)((c == White) ? SOUTH_EDGE : NORTH_EDGE)                // BitBoard representing the back rank given a color

#define WHITE_PIECE(t) (board->pieces[GET_PIECE(t, White)])                                     // Bitboard representing our pieces of type t
#define BLACK_PIECE(t) (board->pieces[GET_PIECE(t, Black)])                                  // Bitboard representing their pieces of type t

#define WHITE_PIECES (WHITE_PIECE(Pawn) | WHITE_PIECE(Knight) | WHITE_PIECE(Bishop) | WHITE_PIECE(Rook) | WHITE_PIECE(Queen) | WHITE_PIECE(King)) // Bitboard of all our pieces
#define BLACK_PIECES (BLACK_PIECE(Pawn) | BLACK_PIECE(Knight) | BLACK_PIECE(Bishop) | BLACK_PIECE(Rook) | BLACK_PIECE(Queen) | BLACK_PIECE(King)) // Bitboard of all their pieces



/*
    * heuristic: A heuristic function to evaluate the position of a chess board
    * 
    * Parameters:
    * - l: LookupTable containing precomputed attack patterns
    * - board: Pointer to the current chess board
    * - dict: Dictionary for storing and retrieving board positions
    * 
    * Returns:
    * - An integer score representing the evaluation of the position in favor of the black player
    
*/
int heuristic(LookupTable l, ChessBoard *board, Dictionary *dict) {
    int score = 0;
    
    
    // Check if the game is over
    if (WHITE_PIECE(King) == 0) {
        return INT_MAX;
    }
    if (BLACK_PIECE(King) == 0) {
        return INT_MIN;
    }
    

    if (dict->zobrist != NULL) {
        int dictScore = betterDictScore(board, dict);
        if (dictScore) {
            return dictScore;
        }
    }
    
    // Find threats

    BitBoard black_targets = BLACK_PIECES;
    BitBoard white_targets = WHITE_PIECES;

    for (int j = 0; j < 64; j++) {

        Piece piece = board->squares[j];

        // Skip empty squares
        if (piece == EMPTY_PIECE) {
            continue;
        }


        int pieceType = GET_TYPE(piece);
        int pieceColor = GET_COLOR(piece);
        

        score += pieceScore(pieceType) * (2*pieceColor-1) * PIECE_FACTOR;

        BitBoard targets;
        if (pieceColor) {
            targets = white_targets;
        } else {
            targets = black_targets;
        }
        if (pieceType ==Pawn){
            BitBoard attacks;
            if (GET_COLOR(piece)){
                attacks = (BitBoardShiftSW(BitBoardSetBit(EMPTY_BOARD, j)) | BitBoardShiftSE(BitBoardSetBit(EMPTY_BOARD, j))) & targets;
            }else{
                attacks = (BitBoardShiftNW(BitBoardSetBit(EMPTY_BOARD, j)) | BitBoardShiftNE(BitBoardSetBit(EMPTY_BOARD, j))) & targets;
            }
            score += BitBoardCountBits(attacks) * pieceScore(pieceType) * (2*pieceColor-1) * ATTACK_FACTOR;
        }else{
            
            score += BitBoardCountBits(LookupTableAttacks(l, j, pieceType, targets)) * pieceScore(pieceType) * (2*pieceColor-1) * ATTACK_FACTOR;
        }
    }
    
    score -= BitBoardCountBits(board->castling & BACK_RANK(White))*CASTLING_FACTOR;
    score += BitBoardCountBits(board->castling & BACK_RANK(Black))*CASTLING_FACTOR;
    
    
    if (dict->zobrist != NULL) {
        install_board(dict, board, score, 0);
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

int pieceScore(int pieceType) {
    switch (pieceType) {
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

