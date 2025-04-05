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
#include <stdlib.h>

int stage = 0;

void sortMoves(Move *moves, int size, ChessBoard *board, LookupTable l, Dictionary *dict);
void mergeSort(int *scores, Move *moves, int l, int r);
void merge(int *scores, Move *moves, int l, int m, int r);


int minimax(LookupTable l, ChessBoard *oldBoard, Dictionary *dict, int alpha, int beta, bool maximizingPlayer, clock_t startTime, int timeLimit, bool mustFinish) {

    int final_score;

    if (((clock() - startTime) * 1000) >= timeLimit*CLOCKS_PER_SEC && !mustFinish) {
        return maximizingPlayer ? INT_MIN : INT_MAX;
    }

    if (dict->zobrist != NULL) {
        int dictScore = betterDictScore(oldBoard, dict);
        if (dictScore) {
            return dictScore;
        }
    }
    

    if (oldBoard->depth == 0) {
        return heuristic(l, oldBoard, dict);
    }

    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, oldBoard, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);

    sortMoves(moves, movesSize, oldBoard, l, dict);


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

            // Rerun the heuristic if the depth is 0
            // and the move is a capture
            if (newBoard.depth == 0 && (oldBoard->squares[move.to] != EMPTY_PIECE) ) {
                newBoard.depth = 1;
            }

            int eval = minimax(l, &newBoard, dict, alpha, beta, false, startTime, timeLimit, mustFinish);
            maxEval = (eval > maxEval) ? eval : maxEval;
            alpha = (alpha > eval) ? alpha : eval;
            if (beta <= alpha){
                break; // Alpha-beta pruning
            }
        }
        final_score = maxEval;
    } else {
        int minEval = INT_MAX;
        for (int i = 0; i < movesSize; i++) {

            Move move = moves[i];

            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, oldBoard, move);

            // Rerun the heuristic if the depth is 0
            // and the move is a capture, same as line 71
            if (newBoard.depth == 0 && (oldBoard->squares[move.to] != EMPTY_PIECE) ) {
                newBoard.depth = 1;
            }

            int eval = minimax(l, &newBoard, dict, alpha, beta, true, startTime, timeLimit, mustFinish);
            
            minEval = (eval < minEval) ? eval : minEval;
            beta = (beta < eval) ? beta : eval;
            if (beta <= alpha){
                break; // Alpha-beta pruning
            }
        }
        final_score = minEval;
    }

    // update the dictionary with the final score
    if (dict->zobrist != NULL) {
        install_board(dict, oldBoard, final_score, oldBoard->depth);
    }

    return final_score;
}

Move bestMove(LookupTable l, ChessBoard *boardPtr, Dictionary *dict, int minDepth, int timeLimit, int depth_speed, bool verbose) {
    clock_t startTime = clock();
    int bestVal = boardPtr->turn == White ? INT_MAX : INT_MIN;
    Move bestMove;
    int depthFrontier = boardPtr->depth;

    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, boardPtr, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);
    sortMoves(moves, movesSize, boardPtr, l, dict);
    
    while (
        (((clock() - startTime) * 1000)) <= timeLimit*CLOCKS_PER_SEC
        || depthFrontier <= minDepth
    ) {

        int tempBestVal = boardPtr->turn == White ? INT_MAX : INT_MIN;
        Move tempBestMove;
        for (int i = 0; i < movesSize; i++) {
            Move move = moves[i];
            
            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, boardPtr, move);
            newBoard.depth = depthFrontier;
            

            // Main call of minimax
            int moveVal = minimax(l, &newBoard, dict, INT_MIN, INT_MAX, newBoard.turn, startTime, timeLimit, depthFrontier <= minDepth);



            if ((moveVal > tempBestVal && newBoard.turn == White) || (moveVal < tempBestVal && newBoard.turn == Black)) {
                tempBestMove = move;
                tempBestVal = moveVal;
            }
        }

        
        if ((((clock() - startTime) * 1000)) <= timeLimit*CLOCKS_PER_SEC || depthFrontier <= minDepth) {
            bestMove = tempBestMove;
            bestVal = tempBestVal;
            if (verbose) {
                printf("Depth: %d\n", depthFrontier);
                printf("Best move: %s\n", moveToString(bestMove));
                printf("Best score: %d\n", bestVal);
            }
        }
        depthFrontier+=depth_speed;
        
        if (tempBestVal == INT_MAX || tempBestVal == INT_MIN) {
            break;
        }
        
    }

    if (bestVal == INT_MIN) {
        bestMove = moves[0];
    }

    // update the dictionary with the final score
    if (dict->zobrist != NULL) {
        install_board(dict, boardPtr, bestVal, depthFrontier);
    }

    return bestMove;
}


void sortMoves(Move *moves, int size, ChessBoard *board, LookupTable l, Dictionary *dict) {
    int scores[size];
    ChessBoard *newBoard = malloc(sizeof(ChessBoard));
    for (int i = 0; i < size; i++) {
        ChessBoardPlayMove(newBoard, board, moves[i]);
        scores[i] = heuristic(l, newBoard, dict);
    }
    free(newBoard);
    
    mergeSort(scores, moves, 0, size - 1);
}

void mergeSort(int *scores, Move *moves, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(scores, moves, l, m);
        mergeSort(scores, moves, m + 1, r);
        merge(scores, moves, l, m, r);
    }
}

void merge(int *scores, Move *moves, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];
    Move Lm[n1], Rm[n2];
    for (int i = 0; i < n1; i++) {
        L[i] = scores[l + i];
        Lm[i] = moves[l + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = scores[m + 1 + j];
        Rm[j] = moves[m + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = l;
    while (i < n1 && j < n2) {
        if (L[i] >= R[j]) {
            scores[k] = L[i];
            moves[k] = Lm[i];
            i++;
        } else {
            scores[k] = R[j];
            moves[k] = Rm[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        scores[k] = L[i];
        moves[k] = Lm[i];
        i++;
        k++;
    }

    while (j < n2) {
        scores[k] = R[j];
        moves[k] = Rm[j];
        j++;
        k++;
    }
}