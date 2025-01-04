#ifndef CHESSBOARDHELPER_H
#define CHESSBOARDHELPER_H

Move parseMove(const char *moveStr, ChessBoard *cb);

char *moveToString(Move move);

#endif