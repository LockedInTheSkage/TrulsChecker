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

#define TIME_LIMIT 10*60000 // In milliseconds. 60000 = 1 minute

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

    
    init_dictionary(&dict);
    l = LookupTableNew();
    openingBook = OpeningBookNew(l, ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2));
    OpeningBookGenerate(openingBook, 6);
    runGame(&cb);   
}


static void runGame(ChessBoard *cbinit)
{
    cb = cbinit;

    cb->depth = 2;
    ChessBoardPrintBoard(*cb); 
    Move aiMove = bestMove(l, cb, &dict, -1, TIME_LIMIT, 1, true);
    
    cb = OpeningBookNext(openingBook);
    if (cb == NULL) {
        printf("Opening book is empty\n");
        clean_lookups(0);
    } else {
        runGame(cb);
    }
    
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