
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
    Zobrist_Table table;
    memset(&table, 0, sizeof(Zobrist_Table));
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < PIECE_SIZE; j++){
            table.piece_pos_values[i][j] = rand64();
        }
    }
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        table.en_passant_values[i] = rand64();
    }
    for (int i = 0; i < 4; i++)
    {
        table.castling_values[i] = rand64();
    }
    table.black_to_move_value = rand64();
}

