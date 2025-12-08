#include <stdio.h>
#include <string.h>
#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"

// Piece type to index mapping for binary representation
// Order must match Python: P, N, B, R, Q, K for white (0-5), black (6-11)
// templechess Type enum: Pawn=0, King=1, Knight=2, Bishop=3, Rook=4, Queen=5
static const int PIECE_INDEX[7][2] = {
    {0, 6},   // Pawn: White=0, Black=6
    {5, 11},  // King: White=5, Black=11
    {1, 7},   // Knight: White=1, Black=7
    {2, 8},   // Bishop: White=2, Black=8
    {3, 9},   // Rook: White=3, Black=9
    {4, 10},  // Queen: White=4, Black=10
    {-1, -1}  // Empty
};

/**
 * Convert board to 768-dimensional binary representation
 * Format: 12 piece types * 64 squares
 */
void board_to_binary(ChessBoard *board, float *output) {
    // Initialize to zeros
    memset(output, 0, 768 * sizeof(float));
    
    // For each square
    for (int square = 0; square < BOARD_SIZE; square++) {
        Type type = board->squares[square];
        
        if (type == Empty) continue;
        
        // Determine color
        Color color = (board->colors[White] & ((BitBoard)1 << square)) ? White : Black;
        
        // Get piece index (0-11)
        int piece_idx = PIECE_INDEX[type][color];
        
        if (piece_idx >= 0) {
            // Set bit: piece_idx * 64 + square
            output[piece_idx * 64 + square] = 1.0f;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <FEN>\n", argv[0]);
        return 1;
    }
    
    ChessBoard board = ChessBoardNew(argv[1]);
    
    float binary[768];
    board_to_binary(&board, binary);
    
    // Output as comma-separated values (0 or 1)
    for (int i = 0; i < 768; i++) {
        printf("%d", (int)(binary[i] + 0.5f));
        if (i < 767) printf(",");
    }
    printf("\n");
    
    return 0;
}
