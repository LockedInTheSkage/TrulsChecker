#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BitBoard.h"
#include "Branch.h" 
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Heuristic.h"

// Maximum line length in the .in file
#define POSITIONS "src/data/heuristicTestPositions.in"
#define MAX_LINE_LENGTH 512

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
    }
}

int main(int argc, char *argv[]) {

    // Open the input file
    FILE *file = fopen(POSITIONS, "r");
    if (!file) {
        perror("Failed to open input file");
        return 1;
    }

    // Initialize a lookup table
    LookupTable lookup = LookupTableNew();
    if (!lookup) {
        fprintf(stderr, "Failed to initialize lookup table.\n");
        fclose(file);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int lineNumber = 0;
    int passed = 0, total = 0;

    // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        lineNumber++;
        trim_newline(line);

        // Split the line into FEN string and expected score
        char *fen = strtok(line, ",");
        char *scoreStr = strtok(NULL, ",");
        if (!fen || !scoreStr) {
            fprintf(stderr, "Invalid format on line %d: %s\n", lineNumber, line);
            continue;
        }

        int expectedScore = atoi(scoreStr);

        // Create a chessboard from the FEN string
        ChessBoard board = ChessBoardNew(fen, 0);

        // Compute the heuristic score
        int computedScore = heuristic(lookup, &board, NULL);

        // Compare with the expected score
        total++;
        if (computedScore == expectedScore) {
            printf("Line %d: PASS (FEN: %s, Score: %d)\n", lineNumber, fen, computedScore);
            passed++;
        } else {
            printf("Line %d: FAIL (FEN: %s, Computed: %d, Expected: %d)\n", lineNumber, fen, computedScore, expectedScore);
        }
    }

    // Clean up
    fclose(file);
    LookupTableFree(lookup);

    // Print summary
    printf("\nSummary: %d/%d tests passed.\n", passed, total);

    return 0;
}
