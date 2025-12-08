#include "TemplechessAdapter.h"
#include "templechess/templechess/src/MoveSet.h"
#include <stdio.h>
#include <string.h>

/**
 * Convert a move to algebraic notation string (e.g., "e2e4")
 */
char *moveToString(Move move) {
    static char moveStr[6];
    
    Square from = move.from.square;
    Square to = move.to.square;
    
    // Convert from 0-63 square index to algebraic notation
    moveStr[0] = 'a' + BitBoardFile(from);
    moveStr[1] = '8' - BitBoardRank(from);
    moveStr[2] = 'a' + BitBoardFile(to);
    moveStr[3] = '8' - BitBoardRank(to);
    
    // Handle promotion
    if (move.from.type == Pawn && move.to.type != Pawn) {
        char promoChar = 'q';
        switch (move.to.type) {
            case Knight: promoChar = 'n'; break;
            case Bishop: promoChar = 'b'; break;
            case Rook: promoChar = 'r'; break;
            case Queen: promoChar = 'q'; break;
            default: break;
        }
        moveStr[4] = promoChar;
        moveStr[5] = '\0';
    } else {
        moveStr[4] = '\0';
    }
    
    return moveStr;
}

/**
 * Parse a move from algebraic notation string (e.g., "e2e4")
 */
Move parseMove(const char *moveStr, ChessBoard *board, LookupTable l) {
    Move invalidMove = {0};
    invalidMove.from.type = Empty;
    
    if (strlen(moveStr) < 4) {
        return invalidMove;
    }
    
    // Parse source and destination squares
    Square from = (8 - (moveStr[1] - '0')) * 8 + (moveStr[0] - 'a');
    Square to = (8 - (moveStr[3] - '0')) * 8 + (moveStr[2] - 'a');
    
    // Validate squares
    if (from >= BOARD_SIZE || to >= BOARD_SIZE) {
        return invalidMove;
    }
    
    // Generate all legal moves and find matching move
    MoveSet ms = MoveSetNew();
    MoveSetFill(l, board, &ms);
    int moveCount = MoveSetCount(&ms);
    
    for (int i = 0; i < moveCount; i++) {
        Move move = MoveSetPop(&ms);
        if (move.from.square == from && move.to.square == to) {
            // Check for promotion
            if (strlen(moveStr) >= 5) {
                char promoChar = moveStr[4];
                Type promoType = Queen;
                switch (promoChar) {
                    case 'n': promoType = Knight; break;
                    case 'b': promoType = Bishop; break;
                    case 'r': promoType = Rook; break;
                    case 'q': promoType = Queen; break;
                    default: continue;
                }
                if (move.to.type == promoType) {
                    return move;
                }
            } else {
                return move;
            }
        }
    }
    
    return invalidMove;
}

/**
 * Check if a position is checkmate
 */
int isCheckmate(LookupTable l, ChessBoard *board) {
    // Generate legal moves
    int legalMoves = ChessBoardCount(l, board);
    
    if (legalMoves > 0) {
        return 0; // Not checkmate if there are legal moves
    }
    
    // No legal moves - check if in check
    // TODO: Implement proper check detection
    // For now, assume no legal moves with pieces on board = checkmate
    BitBoard ourPieces = board->colors[board->turn];
    return (ourPieces != 0);
}

/**
 * Check if a position is stalemate
 */
int isStalemate(LookupTable l, ChessBoard *board) {
    // Generate legal moves
    int legalMoves = ChessBoardCount(l, board);
    
    if (legalMoves > 0) {
        return 0; // Not stalemate if there are legal moves
    }
    
    // No legal moves - check if NOT in check
    // TODO: Implement proper check detection
    // For now, return 0
    return 0;
}

/**
 * Check if a position is a draw by insufficient material
 */
int isInsufficientMaterial(ChessBoard *board) {
    // Count material
    int whitePawns = BitBoardCount(board->types[Pawn] & board->colors[White]);
    int blackPawns = BitBoardCount(board->types[Pawn] & board->colors[Black]);
    int whiteKnights = BitBoardCount(board->types[Knight] & board->colors[White]);
    int blackKnights = BitBoardCount(board->types[Knight] & board->colors[Black]);
    int whiteBishops = BitBoardCount(board->types[Bishop] & board->colors[White]);
    int blackBishops = BitBoardCount(board->types[Bishop] & board->colors[Black]);
    int whiteRooks = BitBoardCount(board->types[Rook] & board->colors[White]);
    int blackRooks = BitBoardCount(board->types[Rook] & board->colors[Black]);
    int whiteQueens = BitBoardCount(board->types[Queen] & board->colors[White]);
    int blackQueens = BitBoardCount(board->types[Queen] & board->colors[Black]);
    
    // King vs King
    if (whitePawns + blackPawns + whiteKnights + blackKnights + 
        whiteBishops + blackBishops + whiteRooks + blackRooks + 
        whiteQueens + blackQueens == 0) {
        return 1;
    }
    
    // King and Bishop vs King
    if (whitePawns + blackPawns + whiteKnights + blackKnights + 
        whiteRooks + blackRooks + whiteQueens + blackQueens == 0 &&
        whiteBishops + blackBishops == 1) {
        return 1;
    }
    
    // King and Knight vs King
    if (whitePawns + blackPawns + whiteBishops + blackBishops + 
        whiteRooks + blackRooks + whiteQueens + blackQueens == 0 &&
        whiteKnights + blackKnights == 1) {
        return 1;
    }
    
    return 0;
}
