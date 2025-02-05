#ifndef OPENINGBOOK_H
#define OPENINGBOOK_H

#include "ChessBoard.h"
#include "LookupTable.h"

typedef struct {
    ChessBoard *boards;
    int count;
    int capacity;
    LookupTable l;
} OpeningBook;

OpeningBook *OpeningBookNew(LookupTable l, ChessBoard start);
void OpeningBookGenerate(OpeningBook *book, int maxDepth);
ChessBoard *OpeningBookNext(OpeningBook *book);
void OpeningBookFree(OpeningBook *book);

#endif