#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include "Heuristic.h"
#include "Minimax.h"

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>




int minimax(LookupTable l, ChessBoard *oldBoard, int alpha, int beta, bool maximizingPlayer, clock_t startTime, int timeLimit) {
    // Check for time out
    if (((clock() - startTime) / CLOCKS_PER_SEC) * 1000 >= timeLimit) {
        return INT_MIN; // Indicate timeout
    }
    //printf("Copying board\n");
    // Create a copy of the oldBoard
    ChessBoard boardCopy = *oldBoard;
    ChessBoard *board = &boardCopy;

    //printf("Depth: %d\n", board->depth);

    if (board->depth == 0) {
        //printf("Reached depth 0\n");
        return heuristic(board);
    }

    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, board, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);

    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int i = 0; i < movesSize; i++) {
            Move move = moves[i];
            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, board, move);
            int eval = minimax(l, &newBoard, alpha, beta, false, startTime, timeLimit);
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
            
            int eval = minimax(l, &newBoard, alpha, beta, true, startTime, timeLimit);
            minEval = (eval < minEval) ? eval : minEval;
            beta = (beta < eval) ? beta : eval;
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return minEval;
    }
}

Move bestMove(LookupTable l, ChessBoard *boardPtr, int maxDepth, int timeLimit) {
    
    clock_t startTime = clock();
    int bestVal = INT_MIN;
    Move bestMove;
    
    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, boardPtr, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);
    //printf("Moves size: %d\n", movesSize);
    //printf("Looking for good moves| Time limit: %d ms\n", timeLimit);
    
    // Increase depth of board
    


    while (
        ((clock() - startTime) / CLOCKS_PER_SEC) * 1000 <= timeLimit
        && (maxDepth == -1 || boardPtr->depth < maxDepth)    
            ){
        //printf("In while loop\n");
        for (int i = 0; i < movesSize; i++) {
            //printf("Creating new board\n");
            Move move = moves[i];
            //printf("Creating new board\n");
            ChessBoard newBoard;
            //printf("Playing move\n");
            ChessBoardPlayMove(&newBoard, boardPtr, move);
            //printf("Minimaxing\n");
            int moveVal = minimax(l, &newBoard, INT_MIN, INT_MAX, false, startTime, timeLimit);
            if (moveVal > bestVal) {
                bestMove = move;
                bestVal = moveVal;
            }
        }
    }

    if (bestVal == INT_MIN) {
        // If no move is found within time limit
        bestMove.from = bestMove.to = -1; // Indicating failure to find a move
    }

    return bestMove;
}