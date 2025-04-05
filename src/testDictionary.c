#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Zobrist.h"
#include "Dictionary.h"
#include "Heuristic.h"

#define TEST_POSITIONS_FILE "src/data/dictionaryTestPositions.in"
#define TEST_PASSED "✓ PASSED: "
#define TEST_FAILED "✗ FAILED: "

// Test helper functions
void verify_dictionary_entry(Dictionary *dict, ChessBoard *cb, int expectedScore, int expectedDepth, const char *testName) {
    nlist *entry = lookup_board(dict, cb);
    
    if (entry && entry->score == expectedScore && entry->depth == expectedDepth) {
        printf("%s%s\n", TEST_PASSED, testName);
    } else {
        printf("%s%s - ", TEST_FAILED, testName);
        if (!entry) {
            printf("Entry not found\n");
        } else {
            printf("Found score: %d (expected: %d), depth: %d (expected: %d)\n", 
                   entry->score, expectedScore, entry->depth, expectedDepth);
        }
    }
}

// Test loading positions from file
void test_dictionary_positions(Dictionary *dict) {
    FILE *file = fopen(TEST_POSITIONS_FILE, "r");
    if (!file) {
        fprintf(stderr, "Failed to open test positions file\n");
        return;
    }

    printf("\n=== Testing Dictionary with Positions from File ===\n");
    
    char fen[100];
    int expectedScore, depth;
    int testCount = 0;
    
    while (fscanf(file, "%99[^,], %d, %d\n", fen, &expectedScore, &depth) == 3) {
        testCount++;
        printf("\nTest position %d: %s\n", testCount, fen);
        
        ChessBoard cb = ChessBoardNew(fen, depth);

        // Test initial lookup
        nlist *entry = lookup_board(dict, &cb);
        if (entry) {
            printf("Entry found in dictionary: Score: %d, Depth: %d\n", entry->score, entry->depth);
        } else {
            printf("Entry not found in dictionary (as expected for first lookup)\n");
        }
        
        // Calculate score and insert into dictionary
        int score = heuristic(LookupTableNew(), &cb, dict);
        printf("Calculated score: %d\n", score);

        // Verify entry was created
        nlist *newEntry = lookup_board(dict, &cb);
        if (newEntry) {
            printf("New entry created: Score: %d, Depth: %d\n", newEntry->score, newEntry->depth);
            
            // Test overwriting with deeper depth
            ChessBoard deeperCb = ChessBoardNew(fen, depth + 2);
            install_board(dict, &deeperCb, score * 2, depth + 2);
            nlist *updatedEntry = lookup_board(dict, &deeperCb);
            
            if (updatedEntry && updatedEntry->depth == depth + 2 && updatedEntry->score == score * 2) {
                printf("%sSuccessfully updated entry with deeper depth\n", TEST_PASSED);
            } else {
                printf("%sFailed to update entry with deeper depth\n", TEST_FAILED);
            }
        } else {
            printf("%sNew entry not found in dictionary after calculation\n", TEST_FAILED);
        }
    }

    fclose(file);
}

// Test identical positions with different depths
void test_identical_positions(Dictionary *dict) {
    printf("\n=== Testing Dictionary with Identical Positions at Different Depths ===\n");
    
    // Standard starting position
    char *startPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    // Create boards with different depths
    ChessBoard cb1 = ChessBoardNew(startPos, 1);
    ChessBoard cb2 = ChessBoardNew(startPos, 3);
    ChessBoard cb3 = ChessBoardNew(startPos, 5);
    
    // Insert with different scores
    install_board(dict, &cb1, 100, 1);
    
    // Verify first entry
    verify_dictionary_entry(dict, &cb1, 100, 1, "First entry at depth 1");
    
    // Insert with higher depth
    install_board(dict, &cb2, 200, 3);
    
    // Verify both entries
    verify_dictionary_entry(dict, &cb1, 100, 1, "Entry at depth 1 still correct");
    verify_dictionary_entry(dict, &cb2, 200, 3, "Entry at depth 3 correct");
    
    // Try to insert with lower depth (should not override)
    install_board(dict, &cb3, 50, 2);
    
    // Check that depth 3 entry wasn't downgraded
    verify_dictionary_entry(dict, &cb2, 200, 3, "Entry at depth 3 not downgraded");
    
    // Insert with higher depth
    install_board(dict, &cb3, 300, 5);
    
    // Check highest depth entry
    verify_dictionary_entry(dict, &cb3, 300, 5, "Entry at depth 5 correct");
}

// Test dictionary persistence
void test_dictionary_persistence() {
    printf("\n=== Testing Dictionary Persistence ===\n");
    
    // Create some test positions
    char *testPositions[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1",
        "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1"
    };
    int scores[] = {0, 10, -5};
    int depths[] = {3, 4, 5};
    
    // First dictionary instance
    {
        printf("Creating first dictionary instance...\n");
        Dictionary dict;
        init_dictionary(&dict);
        
        // Insert test positions
        for (int i = 0; i < 3; i++) {
            ChessBoard cb = ChessBoardNew(testPositions[i], depths[i]);
            install_board(&dict, &cb, scores[i], depths[i]);
            printf("Inserted position %d with score %d at depth %d\n", i, scores[i], depths[i]);
        }
        
        // Save and exit
        printf("Saving dictionary to file and freeing memory...\n");
        exit_dictionary(&dict);
    }
    
    // Second dictionary instance to verify persistence
    {
        printf("Creating second dictionary instance and loading from file...\n");
        Dictionary dict;
        init_dictionary(&dict);
        
        // Verify all positions were restored
        for (int i = 0; i < 3; i++) {
            ChessBoard cb = ChessBoardNew(testPositions[i], depths[i]);
            nlist *entry = lookup_board(&dict, &cb);
            
            char testName[100];
            sprintf(testName, "Persistence test for position %d", i);
            
            if (entry && entry->score == scores[i] && entry->depth == depths[i]) {
                printf("%s%s\n", TEST_PASSED, testName);
            } else {
                printf("%s%s - ", TEST_FAILED, testName);
                if (!entry) {
                    printf("Entry not found after reload\n");
                } else {
                    printf("Found score: %d (expected: %d), depth: %d (expected: %d)\n", 
                           entry->score, scores[i], entry->depth, depths[i]);
                }
            }
        }
        
        // Clean up
        exit_dictionary(&dict);
    }
}

// Test dictionary reset and persistence
void test_complete_reset_persistence() {
    printf("\n=== Testing Complete Reset and Persistence ===\n");
    
    // Create unique test position
    char *testPosition = "r2qkbnr/ppp2ppp/2n1p3/3pPb2/3P4/2P2N2/PP3PPP/RNBQKB1R w KQkq - 0 6";
    int testScore = 42;
    int testDepth = 7;
    
    // First dictionary instance - create and save
    {
        Dictionary dict;
        init_dictionary(&dict);
        
        ChessBoard cb = ChessBoardNew(testPosition, testDepth);
        install_board(&dict, &cb, testScore, testDepth);
        printf("Created and saved dictionary with test position\n");
        
        // Verify it exists
        verify_dictionary_entry(&dict, &cb, testScore, testDepth, "Initial entry exists");
        
        // Save and completely free all memory
        exit_dictionary(&dict);
        printf("Dictionary freed completely\n");
    }
    
    // Completely reinitialize everything
    printf("Completely reinitializing dictionary and Zobrist tables...\n");
    
    // Second dictionary instance - load and verify
    {
        Dictionary dict;
        init_dictionary(&dict);
        
        ChessBoard cb = ChessBoardNew(testPosition, testDepth);
        nlist *entry = lookup_board(&dict, &cb);
        
        if (entry && entry->score == testScore && entry->depth == testDepth) {
            printf("%sPersistence successful after complete reset\n", TEST_PASSED);
        } else {
            printf("%sPersistence failed after complete reset - ", TEST_FAILED);
            if (!entry) {
                printf("Entry not found\n");
            } else {
                printf("Found score: %d (expected: %d), depth: %d (expected: %d)\n", 
                       entry->score, testScore, entry->depth, testDepth);
            }
        }
        
        exit_dictionary(&dict);
    }
}

int main() {
    printf("=== Dictionary Test Suite ===\n");
    
    // Test 1: Basic dictionary operations
    Dictionary dict;
    init_dictionary(&dict);
    
    printf("Testing basic dictionary functionality...\n");
    test_dictionary_positions(&dict);
    
    printf("\nTesting identical positions with different depths...\n");
    test_identical_positions(&dict);
    
    printf("\nSaving dictionary to file...\n");
    exit_dictionary(&dict);
    
    // Test 2: Dictionary persistence after load
    init_dictionary(&dict);
    printf("\nTesting dictionary persistence after reload...\n");
    test_dictionary_positions(&dict);
    exit_dictionary(&dict);
    
    // Test 3: Complete persistence test with separate dictionary instances
    test_dictionary_persistence();
    
    // Test 4: Test with complete memory reset
    test_complete_reset_persistence();
    
    printf("\n=== Dictionary Test Suite Complete ===\n");
    return 0;
}