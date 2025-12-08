#include <stdio.h>
#include <assert.h>
#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"
#include "Heuristic.h"

void testStartingPosition() {
    printf("Testing starting position evaluation...\n");
    
    LookupTable l = LookupTableNew();
    ChessBoard board = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    int score = evaluate(l, &board);
    printf("Score: %d (should be 0 for equal material)\n", score);
    assert(score == 0);
    
    LookupTableFree(l);
    printf("PASSED\n\n");
}

void testMaterialAdvantage() {
    printf("Testing material advantage...\n");
    
    LookupTable l = LookupTableNew();
    // White is up a queen
    ChessBoard board = ChessBoardNew("rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    int score = evaluate(l, &board);
    printf("Score: %d (should be positive, around 900 for queen advantage)\n", score);
    assert(score > 800);
    
    LookupTableFree(l);
    printf("PASSED\n\n");
}

void testBlackAdvantage() {
    printf("Testing black material advantage...\n");
    
    LookupTable l = LookupTableNew();
    // Black is up a rook
    ChessBoard board = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kkq - 0 1");
    
    int score = evaluate(l, &board);
    printf("Score: %d (should be negative, around -500 for rook disadvantage)\n", score);
    assert(score < -400);
    
    LookupTableFree(l);
    printf("PASSED\n\n");
}

void testPawnAdvantage() {
    printf("Testing pawn advantage...\n");
    
    LookupTable l = LookupTableNew();
    // White is up two pawns
    ChessBoard board = ChessBoardNew("rnbqkbnr/pppppp2/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    int score = evaluate(l, &board);
    printf("Score: %d (should be around 200 for two pawn advantage)\n", score);
    assert(score >= 150 && score <= 250);
    
    LookupTableFree(l);
    printf("PASSED\n\n");
}

int main() {
    printf("=== Heuristic Test Suite ===\n\n");
    
    testStartingPosition();
    testMaterialAdvantage();
    testBlackAdvantage();
    testPawnAdvantage();
    
    printf("All tests passed!\n");
    return 0;
}
