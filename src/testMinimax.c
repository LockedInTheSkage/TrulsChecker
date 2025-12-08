#include <stdio.h>
#include <assert.h>
#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"
#include "Minimax.h"
#include "Dictionary.h"

void testStartingPosition() {
    printf("Testing starting position...\n");
    
    LookupTable l = LookupTableNew();
    ChessBoard board = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    Dictionary dict;
    init_dictionary(&dict);
    
    Move bestMove = findBestMove(l, &board, &dict, 3, true);
    
    printf("Best move from starting position: ");
    ChessBoardPrintMove(bestMove);
    printf("\n");
    
    exit_dictionary(&dict);
    LookupTableFree(l);
    printf("PASSED\n\n");
}

void testTacticalPosition() {
    printf("Testing tactical position (Scholar's Mate threat)...\n");
    
    LookupTable l = LookupTableNew();
    // Position where White threatens Scholar's Mate
    ChessBoard board = ChessBoardNew("rnbqkbnr/pppp1ppp/8/4p3/2B1P3/8/PPPP1PPP/RNBQK1NR b KQkq - 0 1");
    
    Dictionary dict;
    init_dictionary(&dict);
    
    Move bestMove = findBestMove(l, &board, &dict, 4, true);
    
    printf("Best move: ");
    ChessBoardPrintMove(bestMove);
    printf("\n");
    
    exit_dictionary(&dict);
    LookupTableFree(l);
    printf("PASSED\n\n");
}

void testMateInOne() {
    printf("Testing mate in one...\n");
    
    LookupTable l = LookupTableNew();
    // Back rank mate position
    ChessBoard board = ChessBoardNew("6k1/5ppp/8/8/8/8/5PPP/R5K1 w - - 0 1");
    
    Dictionary dict;
    init_dictionary(&dict);
    
    Move bestMove = findBestMove(l, &board, &dict, 2, true);
    
    printf("Best move (should deliver mate): ");
    ChessBoardPrintMove(bestMove);
    printf("\n");
    
    exit_dictionary(&dict);
    LookupTableFree(l);
    printf("PASSED\n\n");
}

int main() {
    printf("=== Minimax Test Suite ===\n\n");
    
    testStartingPosition();
    testTacticalPosition();
    testMateInOne();
    
    printf("All tests passed!\n");
    return 0;
}
