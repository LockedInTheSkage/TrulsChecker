#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"
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
        
        ChessBoard cb = ChessBoardNew(fen);

        // Test initial lookup
        nlist *entry = lookup_board(dict, &cb);
        if (entry) {
            printf("Entry found in dictionary: Score: %d, Depth: %d\n", entry->score, entry->depth);
        } else {
            printf("Entry not found in dictionary (as expected for first lookup)\n");
        }
        
        // Calculate score and insert into dictionary
        int score = evaluate(LookupTableNew(), &cb);
        printf("Calculated score: %d\n", score);
        install_board(dict, &cb, score, depth);

        // Verify entry was created
        nlist *newEntry = lookup_board(dict, &cb);
        if (newEntry) {
            printf("New entry created: Score: %d, Depth: %d\n", newEntry->score, newEntry->depth);
            
            // Test overwriting with deeper depth
            ChessBoard deeperCb = ChessBoardNew(fen);
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
    ChessBoard cb1 = ChessBoardNew(startPos);
    ChessBoard cb2 = ChessBoardNew(startPos);
    ChessBoard cb3 = ChessBoardNew(startPos);
    
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
            ChessBoard cb = ChessBoardNew(testPositions[i]);
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
            ChessBoard cb = ChessBoardNew(testPositions[i]);
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
        
        ChessBoard cb = ChessBoardNew(testPosition);
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
        
        ChessBoard cb = ChessBoardNew(testPosition);
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

void test_persistence_within_nlist(Dictionary *dict) {
    printf("\n=== Testing Persistence within nlist ===\n");
    
    // Create a dictionary and add some entries
    
    ChessBoard cb = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    // Find the key for the board
    uint64_t key = get_zobrist_hash(&cb, dict->zobrist);
    nlist *entry = lookup(dict, key);
    if (entry) {
        printf("Entry found in dictionary: Score: %d, Depth: %d\n", entry->score, entry->depth);
    } else {
        printf("Entry not found in dictionary (as expected for first lookup)\n");
    }
    // Insert a new entry
    put(dict, key, 100, 3);

    //Try inserting new entries with roughly the same key
    uint64_t testKey1 = key + HASHSIZE;
    uint64_t testKey2 = key + 2 * HASHSIZE;
    uint64_t testKey3 = key + HASHSIZE+1;
    uint64_t testKey4 = key + HASHSIZE-1;

    //Insert new entries
    put(dict, testKey1, 101, 4);
    put(dict, testKey2, 102, 5);
    put(dict, testKey3, 103, 6);
    put(dict, testKey4, 104, 7);

    // Verify the entries
    nlist *entry0 = lookup(dict, key);
    nlist *entry1 = lookup(dict, testKey1);
    nlist *entry2 = lookup(dict, testKey2);
    nlist *entry3 = lookup(dict, testKey3);
    nlist *entry4 = lookup(dict, testKey4);

    if (entry0 && entry0->score == 100 && entry0->depth == 3) {
        printf("%sEntry 0 found and correct\n", TEST_PASSED);
    } else {
        printf("%sEntry 0 not found or incorrect\n", TEST_FAILED);
    }
    if (entry1 && entry1->score == 101 && entry1->depth == 4) {
        printf("%sEntry 1 found and correct\n", TEST_PASSED);
    } else {
        printf("%sEntry 1 not found or incorrect\n", TEST_FAILED);
    }
    if (entry2 && entry2->score == 102 && entry2->depth == 5) {
        printf("%sEntry 2 found and correct\n", TEST_PASSED);
    } else {
        printf("%sEntry 2 not found or incorrect\n", TEST_FAILED);
    }
    if (entry3 && entry3->score == 103 && entry3->depth == 6) {
        printf("%sEntry 3 found and correct\n", TEST_PASSED);
    } else {
        printf("%sEntry 3 not found or incorrect\n", TEST_FAILED);
    }
    if (entry4 && entry4->score == 104 && entry4->depth == 7) {
        printf("%sEntry 4 found and correct\n", TEST_PASSED);
    } else {
        printf("%sEntry 4 not found or incorrect\n", TEST_FAILED);
    }
}

// Add additional test to verify binary search tree properties
void test_binary_tree_properties(Dictionary *dict) {
    printf("\n=== Testing Binary Tree Properties ===\n");
    
    // Generate a sequence of keys that would create an unbalanced tree 
    // if inserted in order
    uint64_t keys[10];
    int scores[10];
    int depths[10];
    
    for (int i = 0; i < 10; i++) {
        keys[i] = 1000 + i;  // Sequential keys
        scores[i] = 100 + i;
        depths[i] = i + 1;
    }
    
    // Insert keys in order
    unsigned hashval = hash(keys[0]);  // All keys will hash to the same bucket
    
    for (int i = 0; i < 10; i++) {
        put(dict, keys[i], scores[i], depths[i]);
        printf("Inserted key %lu with score %d and depth %d\n", 
               keys[i], scores[i], depths[i]);
    }
    
    // Verify all keys can be found
    for (int i = 0; i < 10; i++) {
        nlist *entry = lookup(dict, keys[i]);
        if (entry && entry->key == keys[i] && entry->score == scores[i] && entry->depth == depths[i]) {
            printf("%sSuccessfully found key %lu in BST\n", TEST_PASSED, keys[i]);
        } else {
            printf("%sFailed to find key %lu in BST\n", TEST_FAILED, keys[i]);
        }
    }
    
    // Test with a non-existent key
    uint64_t nonExistentKey = 9999;
    if (lookup(dict, nonExistentKey) == NULL) {
        printf("%sCorrectly returned NULL for non-existent key\n", TEST_PASSED);
    } else {
        printf("%sIncorrectly found a non-existent key\n", TEST_FAILED);
    }
}

// Test collision handling
void test_hash_collisions(Dictionary *dict) {
    printf("\n=== Testing Hash Collision Handling ===\n");
    
    // Create keys that hash to the same value but are different
    uint64_t baseKey = 12345;
    unsigned hashval = hash(baseKey);
    uint64_t collisionKey = baseKey + HASHSIZE;  // Will hash to same value
    
    // Verify they hash to the same value
    if (hash(baseKey) == hash(collisionKey)) {
        printf("Keys %lu and %lu hash to the same value %u\n", baseKey, collisionKey, hashval);
    } else {
        printf("ERROR: Keys don't hash to the same value\n");
        return;
    }
    
    // Insert first key
    put(dict, baseKey, 100, 3);
    
    // Verify first key is stored correctly
    nlist *entry1 = lookup(dict, baseKey);
    if (entry1 && entry1->key == baseKey && entry1->score == 100 && entry1->depth == 3) {
        printf("%sBaseKey stored correctly\n", TEST_PASSED);
    } else {
        printf("%sBaseKey not stored correctly\n", TEST_FAILED);
    }
    
    // Insert second key (collision)
    put(dict, collisionKey, 200, 4);
    
    // Verify both keys are stored and retrievable
    entry1 = lookup(dict, baseKey);
    nlist *entry2 = lookup(dict, collisionKey);
    
    if (entry1 && entry1->key == baseKey && entry1->score == 100 && entry1->depth == 3) {
        printf("%sAfter collision, baseKey still retrieved correctly\n", TEST_PASSED);
    } else {
        printf("%sAfter collision, baseKey not retrieved correctly\n", TEST_FAILED);
    }
    
    if (entry2 && entry2->key == collisionKey && entry2->score == 200 && entry2->depth == 4) {
        printf("%sCollisionKey stored and retrieved correctly\n", TEST_PASSED);
    } else {
        printf("%sCollisionKey not stored/retrieved correctly\n", TEST_FAILED);
    }
    
    // Insert a few more keys to the same hash bucket
    uint64_t collisionKey2 = baseKey + 2 * HASHSIZE;
    uint64_t collisionKey3 = baseKey + 3 * HASHSIZE;
    
    put(dict, collisionKey2, 300, 5);
    put(dict, collisionKey3, 400, 6);
    
    // Verify all keys are still retrievable
    nlist *entry3 = lookup(dict, collisionKey2);
    nlist *entry4 = lookup(dict, collisionKey3);
    
    if (entry3 && entry3->key == collisionKey2 && entry3->score == 300 && entry3->depth == 5) {
        printf("%sCollisionKey2 stored and retrieved correctly\n", TEST_PASSED);
    } else {
        printf("%sCollisionKey2 not stored/retrieved correctly\n", TEST_FAILED);
    }
    
    if (entry4 && entry4->key == collisionKey3 && entry4->score == 400 && entry4->depth == 6) {
        printf("%sCollisionKey3 stored and retrieved correctly\n", TEST_PASSED);
    } else {
        printf("%sCollisionKey3 not stored/retrieved correctly\n", TEST_FAILED);
    }
}

void delete_dictionary_file() {
    // Delete the dictionary file if it exists
    if (remove(DICT_FILENAME) == 0) {
        printf("Deleted existing dictionary file: %s\n", DICT_FILENAME);
    } else {
        printf("No existing dictionary file to delete.\n");
    }
}

int main() {

    delete_dictionary_file();


    printf("=== Dictionary Test Suite ===\n");
    
    // Test 1: Basic dictionary operations
    Dictionary dict;
    init_dictionary(&dict);
    
    printf("Testing basic dictionary functionality...\n");
    test_dictionary_positions(&dict);
    
    printf("\nTesting identical positions with different depths...\n");
    test_identical_positions(&dict);

    printf("\nTesting persistence within nlist...\n");
    test_persistence_within_nlist(&dict);
    
    // Test binary tree specific properties
    test_binary_tree_properties(&dict);
    
    // Test hash collision handling in binary tree
    test_hash_collisions(&dict);
    
    printf("\nSaving dictionary to file...\n");
    exit_dictionary(&dict);
    
    // Test 2: Dictionary persistence after load
    init_dictionary(&dict);
    printf("\nTesting dictionary persistence after reload...\n");
    test_dictionary_positions(&dict);
    
    // Test binary tree properties again after reload
    test_binary_tree_properties(&dict);
    test_hash_collisions(&dict);
    
    exit_dictionary(&dict);
    
    // Test 3: Complete persistence test with separate dictionary instances
    test_dictionary_persistence();
    
    // Test 4: Test with complete memory reset
    test_complete_reset_persistence();
    
    printf("\n=== Dictionary Test Suite Complete ===\n");
    // Clean up
    delete_dictionary_file();
    return 0;
}