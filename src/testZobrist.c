#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Zobrist.h"
#include "ChessBoard.h"

void test_zobrist_from_file(const char *filename) {
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

        setup_board(&cb, line);
        uint64_t hash = get_zobrist_hash(&cb, zobrist_table);
        printf("Position: %s\nHash: %llu\n", line, hash);
    }

    fclose(file);
    free_zobrist(zobrist_table);
}

int main() {
    test_zobrist_from_file("ZobristTestPositions.in");
    return 0;
}