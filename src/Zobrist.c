
#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Zobrist.h"
#include <stdio.h>
#include <stdlib.h>

uint64_t rand64() {
  uint64_t r = 0;
  for (int i = 0; i < 64; i += 1) {
    r <<= 1;
    r ^= (unsigned) rand();
  }
  return r;
}


static const char *ZOBRIST_FILE = "src/data/zobrist.dat";


void free_zobrist(Zobrist_Table *table);
void load_zobrist(Zobrist_Table *table);
void save_zobrist(Zobrist_Table *table);

Zobrist_Table *init_zobrist()
{
    Zobrist_Table *table = malloc(sizeof(Zobrist_Table));


    if (zobrist_file_exists()) {
        load_zobrist(table);
    }else{

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
        
        save_zobrist(table);
    }
    return table;
}

int zobrist_file_exists() {
    FILE *file = fopen(ZOBRIST_FILE, "r");
    if (file) {
        fclose(file);
        return 1; // File exists
    }
    return 0; // File does not exist
}

void load_zobrist(Zobrist_Table *table) {
    FILE *file = fopen(ZOBRIST_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open Zobrist file for reading\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            fscanf(file, "%lu", &table->piece_pos_values[i][j]);
        }
    }
    for (int i = 0; i < BOARD_SIZE; i++) {
        fscanf(file, "%lu", &table->en_passant_values[i]);
    }
    for (int i = 0; i < 4; i++) {
        fscanf(file, "%lu", &table->castling_values[i]);
    }
    fscanf(file, "%lu", &table->black_to_move_value);

    fclose(file);
}

void save_zobrist(Zobrist_Table *table) {
    FILE *file = fopen(ZOBRIST_FILE, "w");

    if (file == NULL) {
        fprintf(stderr, "Failed to open Zobrist file for writing\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            fprintf(file, "%lu ", table->piece_pos_values[i][j]);
        }
        fprintf(file, "\n");
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        fprintf(file, "%lu ", table->en_passant_values[i]);
    }

    fprintf(file, "\n");
    for (int i = 0; i < 4; i++) {
        fprintf(file, "%lu ", table->castling_values[i]);
    }

    fprintf(file, "\n%lu\n", table->black_to_move_value);

    fclose(file);
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

