#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"

/**
 * Evaluate a chess position using material-based evaluation
 * 
 * @param l LookupTable for piece attacks
 * @param board Board to evaluate
 * @return Evaluation score (positive favors white, negative favors black)
 */
int evaluate(LookupTable l, ChessBoard *board);

/**
 * Evaluate a chess position (uses neural network if available, otherwise material)
 * 
 * @param l LookupTable for piece attacks
 * @param board Board to evaluate
 * @return Evaluation score (positive favors white, negative favors black)
 */
int evaluate_position(LookupTable l, ChessBoard *board);

#endif // HEURISTIC_H
