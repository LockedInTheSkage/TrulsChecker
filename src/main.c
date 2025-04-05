#define _XOPEN_SOURCE 700
#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Zobrist.h"
#include "Dictionary.h"
#include "Branch.h"
#include "Minimax.h"
#include "ChessBoardHelper.h"

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIME_LIMIT 10000 // in milliseconds

static void runGame(ChessBoard *cbinit);
static void runApi(char *fen);
static int checkGameOver(ChessBoard *cb, LookupTable l);
static int legalMove(char *moveStr, ChessBoard *cb, LookupTable l);
void clean_lookups(int sig);

ChessBoard *cb;
LookupTable l;
Dictionary dict;

int main(int argc, char *argv[]) {
    struct sigaction sa;
    sa.sa_handler = clean_lookups;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);

    if (argc > 1) {
        if (strcmp(argv[1], "--train") == 0) {
            //train_main(); // Call your training function
        } else if (strcmp(argv[1], "--api") == 0) {
            if (argc > 2) {
                runApi(argv[2]); // Call api function with fen string.
            } else {
                fprintf(stderr, "Usage: %s --api <fen>\n", argv[0]);
                return 1;
            }
        }
    } else {
        ChessBoard cb = ChessBoardNew("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2);
        runGame(&cb);
    }

    return 0;
}

static void runApi(char *fen) {
    l = LookupTableNew();
    ChessBoard cb = ChessBoardNew(fen, 2);
    Move aiMove = bestMove(l, &cb, &dict, 2, TIME_LIMIT, 2, false);
    printf("%s\n", moveToString(aiMove));
    LookupTableFree(l);
}

static void runGame(ChessBoard *cbinit) {
    cb = cbinit;
    l = LookupTableNew();
    ChessBoard *new = malloc(sizeof(ChessBoard));

    if (cb->turn == Black) {
        cb->depth = 2;
        Move aiMove = bestMove(l, cb, &dict, -1, TIME_LIMIT, 2, true);
        printf("AI move: %s\n", moveToString(aiMove));
        ChessBoardPlayMove(new, cb, aiMove);
        memcpy(cb, new, sizeof(ChessBoard));

        int gameState = checkGameOver(cb, l);
        if (gameState == 1) {
            ChessBoardPrintBoard(*cb);
            printf("You lose!\n");
            LookupTableFree(l);
            return;
        } else if (gameState == 2) {
            ChessBoardPrintBoard(*cb);
            printf("Stalemate!\n");
            LookupTableFree(l);
            return;
        }
    }

    while (1) {
        ChessBoardPrintBoard(*cb);
        char moveStr[5] = {0};
        printf("Enter a move: ");
        if (scanf("%4s", moveStr) != 1) {
            fprintf(stderr, "Error reading input\n");
            break;
        }
        while (legalMove(moveStr, cb, l) == 0) {
            if (strcmp(moveStr, "exit") == 0) {
                break;
            }
            printf("Invalid move. Enter a move (4 characters): ");
            if (scanf("%4s", moveStr) != 1) {
                fprintf(stderr, "Error reading input\n");
                break;
            }
        }
        if (strcmp(moveStr, "exit") == 0) {
            ChessBoardPrintMovelist(*cb);
            break;
        }

        Move playerMove = parseMove(moveStr, cb);
        ChessBoardPlayMove(new, cb, playerMove);
        memcpy(cb, new, sizeof(ChessBoard));
        printf("Player move: %s\n", moveStr);
        ChessBoardPrintBoard(*cb);
        int gameState = checkGameOver(cb, l);
        printf("Game state: %d\n", gameState);
        if (gameState == 1) {
            printf("You win!\n");
            break;
        } else if (gameState == 2) {
            printf("Stalemate!\n");
            break;
        }

        cb->depth = 2;
        Move aiMove = bestMove(l, cb, &dict, 2, TIME_LIMIT, 2, true);
        printf("AI move: %s\n", moveToString(aiMove));
        ChessBoardPlayMove(new, cb, aiMove);
        memcpy(cb, new, sizeof(ChessBoard));

        gameState = checkGameOver(cb, l);
        if (gameState == 1) {
            ChessBoardPrintBoard(*cb);
            printf("You lose!\n");
            break;
        } else if (gameState == 2) {
            ChessBoardPrintBoard(*cb);
            printf("Stalemate!\n");
            break;
        }
    }

    clean_lookups(0);
}

int checkGameOver(ChessBoard *cb, LookupTable l) {
    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, cb, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);

    if (movesSize == 0) {
        BitBoard checking = ChessBoardChecking(l, cb);
        if (checking != EMPTY_BOARD) {
            return 1;
        } else {
            return 2;
        }
    } else {
        return 0;
    }
}

int legalMove(char *moveStr, ChessBoard *cb, LookupTable l) {
    if (strcmp(moveStr, "exit") == 0) {
        return 1;
    }
    if (strlen(moveStr) != 4) {
        return 0;
    }
    Move move = parseMove(moveStr, cb);

    Branch branches[BRANCHES_SIZE];
    int branchesSize = BranchFill(l, cb, branches);
    Move moves[MOVES_SIZE];
    int movesSize = BranchExtract(branches, branchesSize, moves);
    for (int i = 0; i < movesSize; i++) {
        if (moves[i].from == move.from && moves[i].to == move.to) {
            return 1;
        }
    }
    return 0;
}

void clean_lookups(int sig) {
    printf("\nGame over\n");
    if (dict.zobrist != NULL) {
        exit_dictionary(&dict);
    }
    if (l != NULL) {
        LookupTableFree(l);
    }
    exit(0);
}