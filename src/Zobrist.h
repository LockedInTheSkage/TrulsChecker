#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <stdint.h>
#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"

// In templechess v2: Pawn, King, Knight, Bishop, Rook, Queen, Empty = 7 types
// We need 12 pieces: 6 types * 2 colors (excluding Empty)
#define PIECE_SIZE 12

typedef struct
{
    uint64_t piece_pos_values[BOARD_SIZE][PIECE_SIZE];
    uint64_t en_passant_values[BOARD_SIZE];
    uint64_t castling_values[4];
    uint64_t black_to_move_value;
} Zobrist_Table;

Zobrist_Table *init_zobrist();

int zobrist_file_exists();

void load_zobrist(Zobrist_Table *table);

void save_zobrist(Zobrist_Table *table);

void free_zobrist(Zobrist_Table *table);

uint64_t get_zobrist_hash(ChessBoard *cb, Zobrist_Table *table);
#endif