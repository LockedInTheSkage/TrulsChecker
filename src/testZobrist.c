#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"
#include "Zobrist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_zobrist_from_file(const char *filename);

int main(int argc  __attribute__((unused)), char **argv __attribute__((unused))){
    test_zobrist_from_file("src/data/ZobristTestPosition.in");
    return 0;  
}

void test_zobrist_from_file(const char *filename) {
    printf("Testing Zobrist hash from file\n");
    printf("File: %s\n", filename);
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    Zobrist_Table *zobrist_table = init_zobrist();
    ChessBoard cb;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        cb = ChessBoardNew(line); 
        uint64_t hash = get_zobrist_hash(&cb, zobrist_table);
        printf("Position: %s\nHash: %lu\n", line, hash);
    }

    fclose(file);
    free_zobrist(zobrist_table);
}

