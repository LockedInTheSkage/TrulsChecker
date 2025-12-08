#include "Heuristic.h"
#include "NeuralHeuristic.h"
#include <stdio.h>

// Piece values in centipawns
#define PAWN_VALUE 100
#define KNIGHT_VALUE 320
#define BISHOP_VALUE 330
#define ROOK_VALUE 500
#define QUEEN_VALUE 900
#define KING_VALUE 20000

/**
 * Simple material-based evaluation
 * TODO: Add positional evaluation, king safety, pawn structure, etc.
 */
int evaluate(LookupTable l, ChessBoard *board) {
    int score = 0;
    
    // Material count
    int whitePawns = BitBoardCount(board->types[Pawn] & board->colors[White]);
    int blackPawns = BitBoardCount(board->types[Pawn] & board->colors[Black]);
    score += (whitePawns - blackPawns) * PAWN_VALUE;
    
    int whiteKnights = BitBoardCount(board->types[Knight] & board->colors[White]);
    int blackKnights = BitBoardCount(board->types[Knight] & board->colors[Black]);
    score += (whiteKnights - blackKnights) * KNIGHT_VALUE;
    
    int whiteBishops = BitBoardCount(board->types[Bishop] & board->colors[White]);
    int blackBishops = BitBoardCount(board->types[Bishop] & board->colors[Black]);
    score += (whiteBishops - blackBishops) * BISHOP_VALUE;
    
    int whiteRooks = BitBoardCount(board->types[Rook] & board->colors[White]);
    int blackRooks = BitBoardCount(board->types[Rook] & board->colors[Black]);
    score += (whiteRooks - blackRooks) * ROOK_VALUE;
    
    int whiteQueens = BitBoardCount(board->types[Queen] & board->colors[White]);
    int blackQueens = BitBoardCount(board->types[Queen] & board->colors[Black]);
    score += (whiteQueens - blackQueens) * QUEEN_VALUE;
    
    // TODO: Add positional evaluation
    // - Piece-square tables
    // - King safety
    // - Pawn structure
    // - Mobility
    // - Control of center
    // - Development
    
    return score;
}

/**
 * Main evaluation function - uses neural network if available
 */
int evaluate_position(LookupTable l, ChessBoard *board) {
    if (neural_is_available()) {
        return neural_evaluate(l, board);
    }
    return evaluate(l, board);
}
