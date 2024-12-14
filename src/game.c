#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long searchMove(LookupTable l, ChessBoard *cb, int base);

int main(int argc  __attribute__((unused)), char **argv __attribute__((unused)))
{

    ChessBoard cb = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3);
    char moveStr[5] = {0};
    ChessBoardPrintBoard(cb); // Print the board
    
    
    runGame(&cb);
    
}

static void runGame(ChessBoard *cb)
{
    ChessBoard cbinit = *cb;
    while (1)
    {
        char moveStr[5] = {0};
        ChessBoardPrintBoard(*cb); // Print the board
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

        Move move = bestMove(&cb, depthLimit, timeLimit);

        ChessBoard new;
        ChessBoardPlayMove(&new, cb, playerMove);

        ChessBoardPrintBoard(new); // Print the board
        LookupTable l = LookupTableNew();
    }
    
    
}

static long searchMove(LookupTable l, ChessBoard *cb, int base)
{
  if (cb->depth == 0)
    return 1;

  Branch branches[BRANCHES_SIZE];
  int branchesSize = BranchFill(l, cb, branches);

  if ((cb->depth == 1) && (!base))
    return BranchCount(branches, branchesSize);

  long nodes = 0;
  if (cb->depth == 2)
    nodes += BranchPrune(l, cb, branches, branchesSize); // Note that this function is not implemented

  ChessBoard new;
  Move moves[MOVES_SIZE];

  int movesSize = BranchExtract(branches, branchesSize, moves);
  for (int i = 0; i < movesSize; i++)
  {
    Move m = moves[i];
    ChessBoardPlayMove(&new, cb, m);
    int subTree = searchMove(l, &new, 0);
    if (base)
      ChessBoardPrintMove(m, subTree);
    nodes += subTree;
  }

  return nodes;
}