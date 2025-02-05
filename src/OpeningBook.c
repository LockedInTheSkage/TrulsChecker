#include <stdlib.h>
#include <string.h>

#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Branch.h"

typedef struct {
    ChessBoard *boards;
    int count;
    int index;
    int capacity;
    LookupTable l;
} OpeningBook;

static void expandBook(OpeningBook *book) {
    if (book->count >= book->capacity) {
        book->capacity *= 2;
        book->boards = realloc(book->boards, book->capacity * sizeof(ChessBoard));
    }
}

// Initializes the OpeningBook with a lookup table and a starting board
OpeningBook *OpeningBookNew(LookupTable l, ChessBoard start) {
    OpeningBook *book = malloc(sizeof(OpeningBook));
    book->capacity = 16;
    book->count = 0;
    book->index = 0;
    book->l = l;
    book->boards = malloc(book->capacity * sizeof(ChessBoard));
    book->boards[book->count++] = start;
    return book;
}

// Performs a simple BFS over random moves to generate new boards
void OpeningBookGenerate(OpeningBook *book, int maxDepth) {
    for (int i = 0; i < book->count && maxDepth > 0; i++) {
        Branch branches[BRANCHES_SIZE];
        int sz = BranchFill(book->l, &book->boards[i], branches);
        Move moves[MOVES_SIZE];
        int movesSize = BranchExtract(branches, sz, moves);
        for (int m = 0; m < movesSize; m++) {
            ChessBoard newBoard;
            ChessBoardPlayMove(&newBoard, &book->boards[i], moves[m]);
            expandBook(book);
            memcpy(&book->boards[book->count++], &newBoard, sizeof(ChessBoard));
        }
        maxDepth--;
    }
}

// Returns the next chessboard in the opening list
// Returns NULL if the opening book is empty or the index is out of bounds
ChessBoard *OpeningBookNext(OpeningBook *book) {
    int index = book->index;
    book->index++;
    if (book->count == 0) return NULL;
    if (index < 0 || index >= book->count) return NULL;
    return &book->boards[index];
}

void OpeningBookFree(OpeningBook *book) {
    if (!book) return;
    free(book->boards);
    free(book);
}