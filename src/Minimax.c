#include "Minimax.h"
#include "Heuristic.h"
#include "templechess/templechess/src/MoveSet.h"
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

int minimax(
    LookupTable l,
    ChessBoard *board,
    Dictionary *dict,
    int alpha,
    int beta,
    int depth,
    bool maximizingPlayer
) {
    if (dict != NULL && dict->zobrist != NULL) {
        nlist *entry = lookup_board(dict, board);
        if (entry != NULL && entry->depth >= depth) {
            return entry->score;
        }
    }
    
    if (depth == 0) {
        return evaluate_position(l, board);
    }
    
    MoveSet ms = MoveSetNew();
    MoveSetFill(l, board, &ms);
    int moveCount = MoveSetCount(&ms);
    
    if (moveCount == 0) {
        return maximizingPlayer ? INT_MIN : INT_MAX;
    }
    
    int bestScore;
    
    if (maximizingPlayer) {
        bestScore = INT_MIN;
        
        for (int i = 0; i < moveCount; i++) {
            Move move = MoveSetPop(&ms);
            ChessBoard newBoard = *board;
            ChessBoardPlayMove(&newBoard, move);
            
            int score = minimax(l, &newBoard, dict, alpha, beta, depth - 1, false);
            bestScore = (score > bestScore) ? score : bestScore;
            alpha = (alpha > score) ? alpha : score;
            
            if (beta <= alpha) {
                break;
            }
        }
    } else {
        bestScore = INT_MAX;
        
        for (int i = 0; i < moveCount; i++) {
            Move move = MoveSetPop(&ms);
            ChessBoard newBoard = *board;
            ChessBoardPlayMove(&newBoard, move);
            
            int score = minimax(l, &newBoard, dict, alpha, beta, depth - 1, true);
            bestScore = (score < bestScore) ? score : bestScore;
            beta = (beta < score) ? beta : score;
            
            if (beta <= alpha) {
                break;
            }
        }
    }
    
    if (dict != NULL && dict->zobrist != NULL) {
        install_board(dict, board, bestScore, depth);
    }
    
    return bestScore;
}

Move findBestMove(
    LookupTable l,
    ChessBoard *board,
    Dictionary *dict,
    int depth,
    bool verbose
) {
    Move bestMove;
    int bestScore = (board->turn == White) ? INT_MIN : INT_MAX;
    bool maximizing = (board->turn == White);
    
    MoveSet ms = MoveSetNew();
    MoveSetFill(l, board, &ms);
    int moveCount = MoveSetCount(&ms);
    
    if (verbose) {
        printf("Searching %d moves at depth %d\n", moveCount, depth);
    }
    
    for (int i = 0; i < moveCount; i++) {
        Move move = MoveSetPop(&ms);
        ChessBoard newBoard = *board;
        ChessBoardPlayMove(&newBoard, move);
        
        int score = minimax(l, &newBoard, dict, INT_MIN, INT_MAX, depth - 1, !maximizing);
        
        if (verbose) {
            printf("Move %d: ", i + 1);
            ChessBoardPrintMove(move);
            printf(" Score: %d\n", score);
        }
        
        bool isBetter = maximizing ? (score > bestScore) : (score < bestScore);
        if (isBetter || i == 0) {
            bestScore = score;
            bestMove = move;
        }
    }
    
    if (verbose) {
        printf("Best move: ");
        ChessBoardPrintMove(bestMove);
        printf(" Score: %d\n", bestScore);
    }
    
    if (dict != NULL && dict->zobrist != NULL) {
        install_board(dict, board, bestScore, depth);
    }
    
    return bestMove;
}
