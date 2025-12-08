#ifndef NEURAL_HEURISTIC_H
#define NEURAL_HEURISTIC_H

#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"

/**
 * Initialize the neural network evaluator
 * Returns 0 on success, -1 on failure
 */
int neural_init(void);

/**
 * Shutdown the neural network evaluator
 */
void neural_shutdown(void);

/**
 * Evaluate a position using the neural network
 * Falls back to material evaluation if neural network is not available
 */
int neural_evaluate(LookupTable l, ChessBoard *board);

/**
 * Check if neural network is available and initialized
 */
int neural_is_available(void);

#endif // NEURAL_HEURISTIC_H
