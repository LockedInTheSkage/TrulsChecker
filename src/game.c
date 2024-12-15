#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include "Minimax.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIME_LIMIT 2000 // in milliseconds

static void runGame(ChessBoard *cbinit);

int main(int argc  __attribute__((unused)), char **argv __attribute__((unused)))
{
    ChessBoard cb = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2);
    runGame(&cb);   
}

static void runGame(ChessBoard *cbinit)
{
    ChessBoard *cb = cbinit;
    LookupTable l = LookupTableNew();
    while (1)
    {
        ChessBoardPrintBoard(*cb); // Print the board

        char moveStr[5] = {0};
        printf("Enter a move: ");
        if (scanf("%4s", moveStr) != 1) {
            fprintf(stderr, "Error reading input\n");
            return;
        }

        while (strlen(moveStr) != 4) {
            printf("Invalid move. Enter a move (4 characters): ");
            if (scanf("%4s", moveStr) != 1) {
                fprintf(stderr, "Error reading input\n");
                return;
            }
        }

        Move playerMove = parseMove(moveStr, cb);

        ChessBoard *new = malloc(sizeof(ChessBoard));
        ChessBoardPlayMove(new, cb, playerMove);
        cb = new;
        ChessBoardPrintBoard(*cb); // Print the board
      
        Move aiMove = bestMove(l, cb, 2, TIME_LIMIT);

        
        ChessBoardPlayMove(new, cb, aiMove);
        cb = new;
    }
    
    
}