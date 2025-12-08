#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"
#include "Minimax.h"
#include "Dictionary.h"
#include "TemplechessAdapter.h"
#include "NeuralHeuristic.h"

#define MAX_INPUT 100
#define DEFAULT_DEPTH 5

void printHelp() {
    printf("\nCommands:\n");
    printf("  move <e2e4>  - Make a move in algebraic notation\n");
    printf("  ai           - Let AI make a move\n");
    printf("  undo         - Undo last move (not implemented)\n");
    printf("  fen          - Show FEN string\n");
    printf("  board        - Display the board\n");
    printf("  quit         - Exit the game\n");
    printf("  help         - Show this help\n\n");
}

int main(int argc, char **argv) {
    printf("=== TrulsChecker Chess AI ===\n");
    printf("Using templechess v2 library\n\n");
    
    // Initialize neural network evaluator
    if (neural_init() == 0) {
        printf("Neural network evaluation enabled\n");
    } else {
        printf("Using material-based evaluation (neural network not available)\n");
    }
    
    // Initialize lookup table
    LookupTable l = LookupTableNew();
    
    // Initialize dictionary
    Dictionary dict;
    init_dictionary(&dict);
    
    // Initialize board
    char *startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessBoard board = ChessBoardNew(startFen);
    
    // Game settings
    int searchDepth = DEFAULT_DEPTH;
    if (argc > 1) {
        searchDepth = atoi(argv[1]);
        if (searchDepth < 1 || searchDepth > 10) {
            printf("Invalid depth, using default: %d\n", DEFAULT_DEPTH);
            searchDepth = DEFAULT_DEPTH;
        }
    }
    
    printf("Search depth: %d\n", searchDepth);
    printHelp();
    
    // Display initial board
    ChessBoardPrintBoard(board);
    
    // Game loop
    char input[MAX_INPUT];
    while (1) {
        printf("\n%s to move> ", board.turn == White ? "White" : "Black");
        
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        // Parse command
        if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
            break;
        } else if (strcmp(input, "help") == 0 || strcmp(input, "h") == 0) {
            printHelp();
        } else if (strcmp(input, "board") == 0 || strcmp(input, "b") == 0) {
            ChessBoardPrintBoard(board);
        } else if (strcmp(input, "fen") == 0) {
            char *fen = ChessBoardToFEN(&board);
            printf("FEN: %s\n", fen);
        } else if (strcmp(input, "ai") == 0 || strcmp(input, "a") == 0) {
            printf("AI thinking...\n");
            Move bestMove = findBestMove(l, &board, &dict, searchDepth, false);
            
            if (bestMove.from.type == Empty) {
                printf("No legal moves available!\n");
                if (isCheckmate(l, &board)) {
                    printf("Checkmate! %s wins!\n", board.turn == White ? "Black" : "White");
                } else {
                    printf("Stalemate!\n");
                }
                break;
            }
            
            printf("AI plays: %s\n", moveToString(bestMove));
            ChessBoardPlayMove(&board, bestMove);
            ChessBoardPrintBoard(board);
            
            // Check for game end
            if (ChessBoardCount(l, &board) == 0) {
                if (isCheckmate(l, &board)) {
                    printf("Checkmate! %s wins!\n", board.turn == White ? "Black" : "White");
                } else {
                    printf("Stalemate!\n");
                }
                break;
            }
            
            if (isInsufficientMaterial(&board)) {
                printf("Draw by insufficient material!\n");
                break;
            }
        } else if (strncmp(input, "move ", 5) == 0 || strncmp(input, "m ", 2) == 0) {
            char *moveStr = strchr(input, ' ') + 1;
            Move move = parseMove(moveStr, &board, l);
            
            if (move.from.type == Empty) {
                printf("Invalid or illegal move: %s\n", moveStr);
                continue;
            }
            
            ChessBoardPlayMove(&board, move);
            ChessBoardPrintBoard(board);
            
            // Check for game end
            if (ChessBoardCount(l, &board) == 0) {
                if (isCheckmate(l, &board)) {
                    printf("Checkmate! %s wins!\n", board.turn == White ? "Black" : "White");
                } else {
                    printf("Stalemate!\n");
                }
                break;
            }
            
            if (isInsufficientMaterial(&board)) {
                printf("Draw by insufficient material!\n");
                break;
            }
        } else {
            printf("Unknown command: %s (type 'help' for commands)\n", input);
        }
    }
    
    // Cleanup
    printf("\nSaving dictionary...\n");
    save_dictionary(&dict);
    exit_dictionary(&dict);
    neural_shutdown();
    LookupTableFree(l);
    
    printf("Thanks for playing!\n");
    return 0;
}
