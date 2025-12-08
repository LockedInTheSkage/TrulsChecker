#ifndef TEMPLECHESS_ADAPTER_H
#define TEMPLECHESS_ADAPTER_H

#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"

/**
 * Convert a move to algebraic notation string (e.g., "e2e4")
 * 
 * @param move Move to convert
 * @return Static string buffer with move notation
 */
char *moveToString(Move move);

/**
 * Parse a move from algebraic notation string (e.g., "e2e4")
 * 
 * @param moveStr String in format "e2e4" or "e7e8q" for promotion
 * @param board Current board position
 * @param l LookupTable for move generation
 * @return Parsed move, or a move with Empty type if invalid
 */
Move parseMove(const char *moveStr, ChessBoard *board, LookupTable l);

/**
 * Check if a position is checkmate
 * 
 * @param l LookupTable
 * @param board Board to check
 * @return 1 if checkmate, 0 otherwise
 */
int isCheckmate(LookupTable l, ChessBoard *board);

/**
 * Check if a position is stalemate
 * 
 * @param l LookupTable
 * @param board Board to check
 * @return 1 if stalemate, 0 otherwise
 */
int isStalemate(LookupTable l, ChessBoard *board);

/**
 * Check if a position is a draw by insufficient material
 * 
 * @param board Board to check
 * @return 1 if draw, 0 otherwise
 */
int isInsufficientMaterial(ChessBoard *board);

#endif // TEMPLECHESS_ADAPTER_H
