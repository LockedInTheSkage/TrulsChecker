#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include "Minimax.h"
#include "ChessBoardHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIME_LIMIT 10000 // in milliseconds

static void runGame(ChessBoard *cbinit);

static int checkGameOver(ChessBoard *cb, LookupTable l);

static int legalMove(char *moveStr, ChessBoard *cb, LookupTable l);

int main(int argc  __attribute__((unused)), char **argv __attribute__((unused)))
{
    ChessBoard cb = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2); //  
    runGame(&cb);   
}


static void runGame(ChessBoard *cbinit)
{
    ChessBoard *cb = cbinit;
    LookupTable l = LookupTableNew();
    ChessBoard *new = malloc(sizeof(ChessBoard));


    if (cb->turn == Black){
        cb->depth = 2;
        Move aiMove = bestMove(l, cb, -1, TIME_LIMIT);
        
        
        printf("AI move: %s\n", moveToString(aiMove));
        ChessBoardPlayMove(new, cb, aiMove);
        memcpy(cb, new, sizeof(ChessBoard));

        int gameState= checkGameOver(cb, l);
        if (gameState == 1) {
            ChessBoardPrintBoard(*cb); // Print the board
            printf("You lose!\n");
            LookupTableFree(l);

        } else if (gameState == 2) {
            ChessBoardPrintBoard(*cb); // Print the board
            printf("Stalemate!\n");
            LookupTableFree(l);
        }
    }
    
    while (1)
    {   
        
        ChessBoardPrintBoard(*cb); // Print the board
        char moveStr[5] = {0};
        printf("Enter a move: ");
        if (scanf("%4s", moveStr) != 1) {
            fprintf(stderr, "Error reading input\n");
            return;
        }
        while(legalMove(moveStr, cb, l)==0){
            if(moveStr=="exit"){
                break;
            }
            printf("Invalid move. Enter a move (4 characters): ");
            if (scanf("%4s", moveStr) != 1) {
                fprintf(stderr, "Error reading input\n");
                return;
            }
        }
        if (strcmp(moveStr, "exit") == 0) {
            break;
        }

        

        Move playerMove = parseMove(moveStr, cb);
        ChessBoardPlayMove(new, cb, playerMove);
        memcpy(cb, new, sizeof(ChessBoard));
        printf("Player move: %s\n", moveStr);
        ChessBoardPrintBoard(*cb); // Print the board
        int gameState= checkGameOver(cb, l);
        printf("Game state: %d\n", gameState);
        if (gameState == 1) {
            printf("You win!\n");
            break;
        } else if (gameState == 2) {
            printf("Stalemate!\n");
            break;
        }

        
        
        
        cb->depth = 2;
        Move aiMove = bestMove(l, cb, -1, TIME_LIMIT);
        
        
        printf("AI move: %s\n", moveToString(aiMove));
        ChessBoardPlayMove(new, cb, aiMove);
        memcpy(cb, new, sizeof(ChessBoard));

        gameState= checkGameOver(cb, l);
        if (gameState == 1) {
            ChessBoardPrintBoard(*cb); // Print the board
            printf("You lose!\n");
            break;
        } else if (gameState == 2) {
            ChessBoardPrintBoard(*cb); // Print the board
            printf("Stalemate!\n");
            break;
        }
        

    }
    
    LookupTableFree(l);
    
}


int checkGameOver(ChessBoard *cb, LookupTable l){
  Branch branches[BRANCHES_SIZE];
  int branchesSize = BranchFill(l, cb, branches);
  Move moves[MOVES_SIZE];
  int movesSize = BranchExtract(branches, branchesSize, moves);

  if(movesSize == 0){
    BitBoard checking = ChessBoardChecking(l, cb);
    if (checking != EMPTY_BOARD){
      return 1;
    } else {
      return 2;
    }
  } else{
    return 0;
  }
}

int legalMove(char *moveStr, ChessBoard *cb, LookupTable l){
    if (strcmp(moveStr, "exit") == 0) {
        return 1;
    }
    if (strlen(moveStr) != 4) {
        return 0;
    }
    Move move = parseMove(moveStr, cb);

    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, cb, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);
    for (int i = 0; i < movesSize; i++) {
        if (moves[i].from == move.from && moves[i].to == move.to) {
            return 1;
        }
    }
    return 0;
}