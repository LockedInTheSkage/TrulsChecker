#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

#define BOARD_SIZE 64
#define PIECE_SIZE 12 // Including empty pieces
#define MAX_DEPTH 10 // Arbitrary large number for max depth
#define TIME_LIMIT 2000 // in milliseconds

int evaluate(ChessBoard *board) {
    // Simple material-based heuristic
    int score = 0;
    for (int i = 0; i < PIECE_SIZE; i++) {
        if (board->pieces[i] > 0) {
            score += 1; // Example, increment for each piece type
        } else if (board->pieces[i] < 0) {
            score -= 1;
        }
    }
    return score;
}

int minimax(ChessBoard *board, int depth, int alpha, int beta, bool maximizingPlayer, clock_t startTime, int timeLimit) {
    // Check for time out
    if (((clock() - startTime) / CLOCKS_PER_SEC) * 1000 >= timeLimit) {
        return INT_MIN; // Indicate timeout
    }

    if (depth == 0) {
        return evaluate(board);
    }

    Branch b;
    BranchFill(NULL, board, &b); // Fill branches with moves

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int i = 0; i < PIECE_SIZE; i++) {
            Move move = {0}; // Dummy move
            // Apply move
            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, board, move);
            int eval = minimax(&newBoard, depth - 1, alpha, beta, false, startTime, timeLimit);
            maxEval = (eval > maxEval) ? eval : maxEval;
            alpha = (alpha > eval) ? alpha : eval;
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int i = 0; i < PIECE_SIZE; i++) {
            Move move = {0}; // Dummy move
            // Apply move
            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, board, move);
            int eval = minimax(&newBoard, depth - 1, alpha, beta, true, startTime, timeLimit);
            minEval = (eval < minEval) ? eval : minEval;
            beta = (beta < eval) ? beta : eval;
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return minEval;
    }
}

Move bestMove(ChessBoard *board, int depth, int timeLimit) {
    clock_t startTime = clock();
    int bestVal = INT_MIN;
    Move bestMove;
    Branch b;
    BranchFill(NULL, board, &b);

    for (int i = 0; i < PIECE_SIZE; i++) {
        Move move = {0}; // Dummy move
        ChessBoard newBoard;
        ChessBoardPlayMove(&newBoard, board, move);
        int moveVal = minimax(&newBoard, depth - 1, INT_MIN, INT_MAX, false, startTime, timeLimit);
        if (moveVal > bestVal) {
            bestMove = move;
            bestVal = moveVal;
        }
    }

    if (bestVal == INT_MIN) {
        // If no move is found within time limit
        bestMove.from = bestMove.to = -1; // Indicating failure to find a move
    }

    return bestMove;
}