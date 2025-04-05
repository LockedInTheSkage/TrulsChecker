#ifndef HEURISTIC_H
#define HEURISTIC_H

int heuristic(LookupTable l, ChessBoard *board, Dictionary *dict);
int betterDictScore(ChessBoard *board, Dictionary *dict);
int pieceScore(int pieceType);

#endif