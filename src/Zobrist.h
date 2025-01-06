#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <stdint.h>


typedef struct
{
    uint64_t piece_pos_values[BOARD_SIZE][PIECE_SIZE];
    uint64_t en_passant_values[BOARD_SIZE];
    uint64_t castling_values[4];
    uint64_t black_to_move_value;
} Zobrist_Table;

Zobrist_Table *init_zobrist();

void free_zobrist(Zobrist_Table *table);

uint64_t get_zobrist_hash(ChessBoard *cb, Zobrist_Table *table);
#endif