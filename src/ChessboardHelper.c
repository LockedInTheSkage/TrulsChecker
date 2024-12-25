#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"
#include "Heuristic.h"


#include "ChessboardHelper.h"


#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
// NEW CODE

// Assumes the input string is in the correct format (e.g., "e2e4")
Move parseMove(const char *moveStr, ChessBoard *cb) {
    Piece *board = cb->squares;
    Move move;

    // Convert from algebraic notation to 0-63 square index
    move.from = (8 - (moveStr[1] - '0')) * 8 + (moveStr[0] - 'a');
    move.to = (8 - (moveStr[3] - '0')) * 8 + (moveStr[2] - 'a');
    move.moved = board[move.from]; // Get the piece from the board at the "from" square

    return move;
}

// Converts a Move object to a move string (e.g., "e2e4")
char *moveToString(Move move) {
    static char moveStr[5];
    // Convert from 0-63 square index to algebraic notation
    moveStr[0] = 'a' + (move.from % 8); // File of "from" square
    moveStr[1] = '8' - (move.from / 8); // Rank of "from" square
    moveStr[2] = 'a' + (move.to % 8);   // File of "to" square
    moveStr[3] = '8' - (move.to / 8);   // Rank of "to" square
    moveStr[4] = '\0';                  // Null terminator for the string
    return moveStr;
}