#ifndef MINIMAX_ALPHA_BETA_H
#define MINIMAX_ALPHA_BETA_H

#include <stdbool.h>
#include <time.h>



// Minimax algorithm with alpha-beta pruning
int minimax(LookupTable l, ChessBoard *board, int alpha, int beta, bool maximizingPlayer, clock_t startTime, int timeLimit);

// Function to find the best move within the given depth and time limits
Move bestMove(LookupTable l, ChessBoard *board, int depth, int timeLimit);

#endif // MINIMAX_ALPHA_BETA_H
