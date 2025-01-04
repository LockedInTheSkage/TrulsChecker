
#include <stdlib.h>

#include "Zobrist.h"
#include "BitBoard.h"
#include "ChessBoard.h"



uint64_t rand64() {
  uint64_t r = 0;
  for (int i = 0; i < 64; i += 1) {
    r <<= 1;
    r ^= (unsigned) rand();
  }
  return r;
}

Zobrist_Table *init_zobrist()
{
    Zobrist_Table *table = malloc(sizeof(Zobrist_Table));


    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < PIECE_SIZE; j++){
            table->piece_pos_values[i][j] = rand64();
        }
    }
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        table->en_passant_values[i] = rand64();
    }
    for (int i = 0; i < 4; i++)
    {
        table->castling_values[i] = rand64();
    }
    table->black_to_move_value = rand64();
    return table;
}


void free_zobrist(Zobrist_Table *table)
{
    free(table);
}

uint64_t get_zobrist_hash(ChessBoard *cb, Zobrist_Table *table)
{
    uint64_t hash = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        hash ^= table->piece_pos_values[i][cb->squares[i]];
    }
    if (cb->enPassant != EMPTY_SQUARE)
    {
        hash ^= table->en_passant_values[cb->enPassant];
    }
    for (int i = 0; i < 4; i++)
    {
        if (cb->castling & (1 << i))
        {
            hash ^= table->castling_values[i];
        }
    }
    if (cb->turn == Black)
    {
        hash ^= table->black_to_move_value;
    }
    return hash;
}

