#ifndef MINIMAX_ALPHA_BETA_H
#define MINIMAX_ALPHA_BETA_H

#include <limits.h>
#include <stdbool.h>
#include <time.h>

#define BOARD_SIZE 64
#define PIECE_SIZE 12 // Including empty pieces
#define MAX_DEPTH 10 // Arbitrary large number for max depth
#define TIME_LIMIT 2000 // in milliseconds

// Evaluation function for the current board state
int evaluate(ChessBoard *board);

// Minimax algorithm with alpha-beta pruning
int minimax(ChessBoard *board, int depth, int alpha, int beta, bool maximizingPlayer, clock_t startTime, int timeLimit);

// Function to find the best move within the given depth and time limits
Move bestMove(ChessBoard *board, int depth, int timeLimit);

#endif // MINIMAX_ALPHA_BETA_H
