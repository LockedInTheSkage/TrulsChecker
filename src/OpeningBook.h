#ifndef OPENINGBOOK_H
#define OPENINGBOOK_H

#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"

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