#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"

#define OUR(t) (cb->pieces[GET_PIECE(t, cb->turn)])                                     // Bitboard representing our pieces of type t
#define THEIR(t) (cb->pieces[GET_PIECE(t, !cb->turn)])                                  // Bitboard representing their pieces of type t
#define ALL (~cb->pieces[EMPTY_PIECE])                                                  // Bitboard of all the pieces
#define US (OUR(Pawn) | OUR(Knight) | OUR(Bishop) | OUR(Rook) | OUR(Queen) | OUR(King)) // Bitboard of all our pieces
#define THEM (ALL & ~US)                                                                // Bitboard of all their pieces

#define GET_RANK(s) (SOUTH_EDGE >> (EDGE_SIZE * (EDGE_SIZE - BitBoardGetRank(s) - 1))) // BitBoard representing the rank of a specific square
#define BACK_RANK(c) (BitBoard)((c == White) ? SOUTH_EDGE : NORTH_EDGE)                // BitBoard representing the back rank given a color

// Masks used for castling
#define KINGSIDE_CASTLING 0x9000000000000090
#define QUEENSIDE_CASTLING 0x1100000000000011

// Returns a bitboard representing a set of moves given a set of pawns and a color
#define PAWN_ATTACKS(b, c) ((c == White) ? BitBoardShiftNW(b) | BitBoardShiftNE(b) : BitBoardShiftSW(b) | BitBoardShiftSE(b))

static Color getColorFromASCII(char asciiColor);
static char getASCIIFromPiece(Piece p);
static Piece getPieceFromASCII(char asciiPiece);
static void addPiece(ChessBoard *cb, Square s, Piece replacement);

// Assumes FEN and depth is valid
ChessBoard ChessBoardNew(char *fen, int depth)
{
  ChessBoard cb;
  memset(&cb, 0, sizeof(ChessBoard));
  cb.moves_completed = 0;
  memset(cb.movelist, 0, sizeof(Move) * MOVELIST_SIZE);

  // Parse pieces and squares
  for (Square s = 0; s < BOARD_SIZE && *fen; fen++)
  {
    if (*fen == '/')
      continue;

    if (*fen >= '1' && *fen <= '8')
    {
      for (int numSquares = *fen - '0'; numSquares > 0; numSquares--)
      {
        cb.pieces[EMPTY_PIECE] |= BitBoardSetBit(EMPTY_BOARD, s);
        cb.squares[s] = EMPTY_PIECE;
        s++;
      }
    }
    else
    {
      Piece p = getPieceFromASCII(*fen);
      cb.pieces[p] |= BitBoardSetBit(EMPTY_BOARD, s);
      cb.squares[s] = p;
      s++;
    }
  }
  fen++;

  // Parse turn and depth
  cb.turn = getColorFromASCII(*fen);
  cb.depth = depth;
  fen += 2;

  // Parse castling
  if (*fen == '-')
  {
    fen += 2;
  }
  else
  {
    while (*fen != ' ')
    {
      Color c = (*fen == 'K' || *fen == 'Q') ? 0 : 7;
      BitBoard castlingSquares = (*fen == 'K' || *fen == 'k') ? KINGSIDE_CASTLING : QUEENSIDE_CASTLING;
      cb.castling |= (castlingSquares & BACK_RANK(c));
      fen++;
    }
    fen++;
  }

  // Parse en passant
  if (*fen != '-')
  {
    int file = *fen - 'a';
    fen++;
    int rank = EDGE_SIZE - (*fen - '0');
    cb.enPassant = rank * EDGE_SIZE + file;
  }

  return cb;
}

// Assumes that asciiPiece is valid
static Piece getPieceFromASCII(char asciiPiece)
{
  if (asciiPiece == '-')
    return EMPTY_PIECE;
  char *pieces = "PKNBRQ";
  Type t = (Type)(strchr(pieces, toupper(asciiPiece)) - pieces);
  Color c = isupper(asciiPiece) ? White : Black;
  return GET_PIECE(t, c);
}

// Assumes that piece is valid
static char getASCIIFromPiece(Piece p)
{
  if (p == EMPTY_PIECE)
    return '-';
  char *pieces = "PKNBRQ";
  char asciiPiece = pieces[GET_TYPE(p)];
  return GET_COLOR(p) == Black ? tolower(asciiPiece) : asciiPiece;
}

static Color getColorFromASCII(char asciiColor)
{
  return (asciiColor == 'w') ? White : Black;
}

// Given an old board and a new board, copy the old board and play the move on the new board
void ChessBoardPlayMove(ChessBoard *new, ChessBoard *old, Move m)
{
  memcpy(new, old, sizeof(ChessBoard));
  int offset = m.from - m.to;
  new->enPassant = EMPTY_SQUARE;
  new->castling &= ~(BitBoardSetBit(EMPTY_BOARD, m.from) | BitBoardSetBit(EMPTY_BOARD, m.to));
  
  int pieceType = GET_TYPE(new->squares[m.from]);
  addPiece(new, m.from, EMPTY_PIECE);
  if (GET_RANK(m.to) == BACK_RANK(!new->turn) && pieceType == Pawn)
  { // Promotion
    addPiece(new, m.to, GET_PIECE(Queen, new->turn));
  }else{
    addPiece(new, m.to, m.moved);
  }

  new->movelist[new->moves_completed++] = m;
  if (GET_TYPE(m.moved) == Pawn)
  {
    if ((offset == 16) || (offset == -16))
    { // Double push
      new->enPassant = m.from - (offset / 2);
    }
    else if (m.to == old->enPassant)
    { // Enpassant
      addPiece(new, m.to + (new->turn ? -8 : 8), EMPTY_PIECE);
    }
    
  }
  else if (GET_TYPE(m.moved) == King)
  {
    if (offset == 2)
    { // Queenside castling
      addPiece(new, m.to - 2, EMPTY_PIECE);
      addPiece(new, m.to + 1, GET_PIECE(Rook, new->turn));
    }
    else if (offset == -2)
    { // Kingside castling
      addPiece(new, m.to + 1, EMPTY_PIECE);
      addPiece(new, m.to - 1, GET_PIECE(Rook, new->turn));
    }
  }

  new->turn = !new->turn;
  new->depth--;
}

// Adds a piece to a chessboard
static void addPiece(ChessBoard *cb, Square s, Piece replacement)
{
  BitBoard b = BitBoardSetBit(EMPTY_BOARD, s);
  Piece captured = cb->squares[s];
  cb->squares[s] = replacement;
  cb->pieces[replacement] |= b;
  cb->pieces[captured] &= ~b;
}

void ChessBoardPrintBoard(ChessBoard cb)
{
  for (int rank = 0; rank < EDGE_SIZE; rank++)
  {
    for (int file = 0; file < EDGE_SIZE; file++)
    {
      Square s = rank * EDGE_SIZE + file;
      Piece p = cb.squares[s];
      printf("%c ", getASCIIFromPiece(p));
    }
    printf("%d\n", EDGE_SIZE - rank);
  }
  printf("a b c d e f g h\n\n");
}

void ChessBoardPrintMove(Move m, long nodes)
{
  printf("%c%d%c%d: %ld\n", 'a' + (m.from % EDGE_SIZE), EDGE_SIZE - (m.from / EDGE_SIZE), 'a' + (m.to % EDGE_SIZE), EDGE_SIZE - (m.to / EDGE_SIZE), nodes);
}


BitBoard ChessBoardChecking(LookupTable l, ChessBoard *cb)
{
  Square ourKing = BitBoardGetLSB(OUR(King));
  BitBoard checking = (PAWN_ATTACKS(OUR(King), cb->turn) & THEIR(Pawn)) |
                      (LookupTableAttacks(l, ourKing, Knight, EMPTY_BOARD) & THEIR(Knight));
  BitBoard candidates = (LookupTableAttacks(l, ourKing, Bishop, THEM) & (THEIR(Bishop) | THEIR(Queen))) |
                        (LookupTableAttacks(l, ourKing, Rook, THEM) & (THEIR(Rook) | THEIR(Queen)));

  while (candidates)
  {
    Square s = BitBoardPopLSB(&candidates);
    BitBoard b = LookupTableGetSquaresBetween(l, ourKing, s) & ALL & ~THEM;
    if (b == EMPTY_BOARD)
    {
      checking |= BitBoardSetBit(EMPTY_BOARD, s);
    }
  }

  return checking;
}

BitBoard ChessBoardPinned(LookupTable l, ChessBoard *cb)
{
  Square ourKing = BitBoardGetLSB(OUR(King));
  BitBoard candidates = (LookupTableAttacks(l, ourKing, Bishop, THEM) & (THEIR(Bishop) | THEIR(Queen))) |
                        (LookupTableAttacks(l, ourKing, Rook, THEM) & (THEIR(Rook) | THEIR(Queen)));
  BitBoard pinned = EMPTY_BOARD;

  while (candidates)
  {
    Square s = BitBoardPopLSB(&candidates);
    BitBoard b = LookupTableGetSquaresBetween(l, ourKing, s) & ALL & ~THEM;
    if (b != EMPTY_BOARD && (b & (b - 1)) == EMPTY_BOARD)
    {
      pinned |= b;
    }
  }

  return pinned;
}

BitBoard ChessBoardAttacked(LookupTable l, ChessBoard *cb)
{
  BitBoard attacked, b;
  BitBoard occupancies = ALL & ~OUR(King);

  attacked = PAWN_ATTACKS(THEIR(Pawn), (!cb->turn));
  b = THEM & ~THEIR(Pawn);
  while (b)
  {
    Square s = BitBoardPopLSB(&b);
    if (GET_TYPE(cb->squares[s]) == 6){
      ChessBoardPrintBoard(*cb);
      ChessBoardPrintMovelist(*cb);
      
      fprintf(stderr, "6 is an invalid piece type\n");
      exit(EXIT_FAILURE);
    }
    attacked |= LookupTableAttacks(l, s, GET_TYPE(cb->squares[s]), occupancies);
  }
  return attacked;
}

void ChessBoardPrintMovelist(ChessBoard cb){
  for (int i = 0; i < cb.moves_completed; i++){
    Move m = cb.movelist[i];
    ChessBoardPrintMove(m, 0);
  }

}
