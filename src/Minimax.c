#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Zobrist.h"
#include "Dictionary.h"
#include "Branch.h"
#include "Heuristic.h"
#include "Minimax.h"
#include "ChessBoardHelper.h"


#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>


int stage = 0;

int minimax(LookupTable l, ChessBoard *oldBoard, Dictionary *dict, int alpha, int beta, bool maximizingPlayer, clock_t startTime, int timeLimit) {


    if (((clock() - startTime) * 1000) >= timeLimit*CLOCKS_PER_SEC) {
        return INT_MIN;
    }

    int dictScore = betterDictScore(oldBoard, dict);
    if (dictScore) {
        return dictScore;
    }

    if (oldBoard->depth == 0) {
        return heuristic(l, oldBoard, dict);
    }

    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, oldBoard, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);

    if (movesSize == 0) {
        if (ChessBoardChecking(l, oldBoard) != EMPTY_BOARD) {
            if (oldBoard->turn == Black) {
                return INT_MIN;
            } else {
                return INT_MAX;
            }
        } else {
            return 0;
        }
    }
    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int i = 0; i < movesSize; i++) {
            
            Move move = moves[i];

            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, oldBoard, move);

            if (newBoard.depth == 0 && (oldBoard->squares[move.to] != EMPTY_PIECE) ) {
                newBoard.depth = 1;
            }

            int eval = minimax(l, &newBoard, dict, alpha, beta, false, startTime, timeLimit);
            maxEval = (eval > maxEval) ? eval : maxEval;
            alpha = (alpha > eval) ? alpha : eval;
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int i = 0; i < movesSize; i++) {

            Move move = moves[i];

            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, oldBoard, move);

            if (newBoard.depth == 0 && (oldBoard->squares[move.to] != EMPTY_PIECE) ) {
                newBoard.depth = 1;
            }

            int eval = minimax(l, &newBoard, dict, alpha, beta, true, startTime, timeLimit);
            
            minEval = (eval < minEval) ? eval : minEval;
            beta = (beta < eval) ? beta : eval;
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return minEval;
    }
}

Move bestMove(LookupTable l, ChessBoard *boardPtr, Dictionary *dict, int maxDepth, int timeLimit) {
    clock_t startTime = clock();
    int bestVal = INT_MIN;
    Move bestMove;
    int depthFrontier = boardPtr->depth;
    printf("Depth: %d\n", depthFrontier);

    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, boardPtr, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);
    
    while (
        !((((clock() - startTime) * 1000)) >= timeLimit*CLOCKS_PER_SEC)
        && (maxDepth == -1 || depthFrontier < maxDepth)    
            ){
        int tempBestVal = INT_MIN;
        Move tempBestMove;
        for (int i = 0; i < movesSize; i++) {
            Move move = moves[i];
            
            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, boardPtr, move);
            newBoard.depth = depthFrontier;
            
            int moveVal = minimax(l, &newBoard, dict, INT_MIN, INT_MAX, newBoard.turn, startTime, timeLimit);
            if ((moveVal > tempBestVal && newBoard.turn == White) || (moveVal < tempBestVal && newBoard.turn == Black)) {
                tempBestMove = move;
                tempBestVal = moveVal;
            }
        }

        depthFrontier+=2;
        if ((((clock() - startTime) * 1000)) <= timeLimit*CLOCKS_PER_SEC) {
            bestMove = tempBestMove;
            bestVal = tempBestVal;
            printf("Best move: %s\n", moveToString(bestMove));
            printf("Best score: %d\n", bestVal);
            printf("Depth: %d\n", depthFrontier);
        }
        
        if (tempBestVal == INT_MAX || tempBestVal == INT_MIN) {
            break;
        }
        
    }

    if (bestVal == INT_MIN) {
        bestMove = moves[0];
    }

    return bestMove;
}


