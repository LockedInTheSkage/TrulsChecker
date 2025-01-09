#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Zobrist.h"
#include "Dictionary.h"
#include "Heuristic.h"

#define TEST_POSITIONS_FILE "data/dictionaryTestPositions.in"

void test_dictionary(Dictionary *dict) {
    FILE *file = fopen(TEST_POSITIONS_FILE, "r");
    if (!file) {
        fprintf(stderr, "Failed to open test positions file\n");
        return;
    }

    char fen[100];
    int expectedScore, depth;
    while (fscanf(file, "%99[^,], %d, %d\n", fen, &expectedScore, &depth) == 3) {
        ChessBoard cb = ChessBoardNew(fen, depth);

        nlist *entry = lookup_board(dict, &cb);
        if (entry) {
            printf("Entry found in dictionary: Score: %d, Depth: %d\n", entry->score, entry->depth);
        } else {
            printf("Entry not found in dictionary\n");
        }
        int score = heuristic(LookupTableNew(), &cb, dict);

        nlist *newEntry = lookup_board(dict, &cb);
        if (newEntry) {
            printf("New entry found in dictionary: Score: %d, Depth: %d\n", newEntry->score, newEntry->depth);
        } else {
            printf("New entry not found in dictionary\n");
        }
    }

    fclose(file);
}

int main() {
    printf("Starting dict tests\n");
    Dictionary dict;
    init_dictionary(&dict);

    printf("Testing dictionary functionality...\n");
    test_dictionary(&dict);

    printf("Exiting dictionary and saving to file...\n");
    exit_dictionary(&dict);

    printf("Re-initializing dictionary and loading from file...\n");
    init_dictionary(&dict);

    printf("Testing dictionary functionality again...\n");
    test_dictionary(&dict);

    exit_dictionary(&dict);
    return 0;
}