#define _XOPEN_SOURCE 700
#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Zobrist.h"
#include "Dictionary.h"
#include "Branch.h"
#include "Minimax.h"
#include "ChessBoardHelper.h"
#include "OpeningBook.h"


#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIME_LIMIT 10000 // in milliseconds

static void runGame(ChessBoard *cbinit);

static int checkGameOver(ChessBoard *cb, LookupTable l);

static int legalMove(char *moveStr, ChessBoard *cb, LookupTable l);

void clean_lookups(int sig);

ChessBoard *cb;
LookupTable l;
Dictionary dict;
OpeningBook *openingBook;

int main(int argc  __attribute__((unused)), char **argv __attribute__((unused)))
{
    struct sigaction sa;
    sa.sa_handler = clean_lookups;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);  // Handle Ctrl+C
    sigaction(SIGTERM, &sa, NULL); // Handle termination
    sigaction(SIGQUIT, &sa, NULL); // Handle quit
    sigaction(SIGTSTP, &sa, NULL); // Handle Ctrl+Z

    ChessBoard cb = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2);
    /* ChessBoard cb = ChessBoardNew("4k3/8/8/8/8/1r6/r7/6K1 b - - 0 1", 2); */

    
    //init_dictionary(&dict);
    l = LookupTableNew();
    openingBook = OpeningBookNew(l, ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2));
    OpeningBookGenerate(openingBook, 6);
    runGame(&cb);   
}


static void runGame(ChessBoard *cbinit)
{
    cb = cbinit;


    if (cb->turn == Black){
        cb->depth = 2;
        Move aiMove = bestMove(l, cb, &dict, -1, TIME_LIMIT, 1, false);
        
        

        ChessBoardPlayMove(cb, cb, aiMove);

        int gameState= checkGameOver(cb, l);
        if (gameState == 1) {
            ChessBoardPrintBoard(*cb); 
            printf("You lose!\n");
            

        } else if (gameState == 2) {
            ChessBoardPrintBoard(*cb); 
            printf("Stalemate!\n");
            
        }
    }
    printf("Let's play!\n");
    while (!checkGameOver(cb, l))
    {   
        
        ChessBoardPrintBoard(*cb); 
        cb->depth = 2;
        Move whiteMove = bestMove(l, cb, &dict, -1, TIME_LIMIT, 1, false);
        
        ChessBoardPlayMove(cb, cb, whiteMove);

        printf("White move: %s\n", moveToString(whiteMove));
        int gameState= checkGameOver(cb, l);
        if (gameState == 1) {
            ChessBoardPrintBoard(*cb); 
            printf("You lose!\n");
            

        } else if (gameState == 2) {
            ChessBoardPrintBoard(*cb); 
            printf("Stalemate!\n");
            
        }

        ChessBoardPrintBoard(*cb); 
        
        
        cb->depth = 2;
        Move blackMove = bestMove(l, cb, &dict, -1, TIME_LIMIT, 1, false);
        
        
        printf("Black move: %s\n", moveToString(blackMove));
        ChessBoardPlayMove(cb, cb, blackMove);

        gameState= checkGameOver(cb, l);
        if (gameState == 1) {
            ChessBoardPrintBoard(*cb); 
            printf("You lose!\n");
            break;
        } else if (gameState == 2) {
            ChessBoardPrintBoard(*cb); 
            printf("Stalemate!\n");
            break;
        }
        

    }
    cb = OpeningBookNext(openingBook);
    if (cb == NULL) {
        printf("Opening book is empty\n");
        clean_lookups(0);
    } else {
        runGame(cb);
    }
    
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

void clean_lookups(int sig) {
    printf("\nGame over\n");
    
    if (openingBook != NULL){
        OpeningBookFree(openingBook);
    }
    
    if(dict.zobrist != NULL){
        exit_dictionary(&dict);
    }
    if (l != NULL){
        LookupTableFree(l);
    }
    exit(0);
}