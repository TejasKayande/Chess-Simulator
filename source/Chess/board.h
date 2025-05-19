
#ifndef _BOARD_H_
#define _BOARD_H_

#include "defines.h"

/*
  NOTE(Tejas): the board starts (i.e. when rank = 0 and file = 0) at top left
               square, where there is a White King Side Rook.

  NOTE(Tejas): The conventional Chess considers top right corner to be 0, 0
              (i.e, rank = 1 and file = a), but it ease to visualize rendering
              when using the following way (atleast for me!)... 

                (0) (1) (2) (3) (4) (5) (6) (7)

                 h   g   f   e   d   c   b   a
               +---+---+---+---+---+---+---+---+
        (0)  1 | R | N | B | K | Q | B | N | R |
               +---+---+---+---+---+---+---+---+
        (1)  2 | P | P | P | P | P | P | P | P |
               +---+---+---+---+---+---+---+---+
        (2)  3 |   |   |   |   |   |   |   |   |
               +---+---+---+---+---+---+---+---+
        (3)  4 |   |   |   |   |   |   |   |   |
               +---+---+---+---+---+---+---+---+
        (4)  5 |   |   |   |   |   |   |   |   |
               +---+---+---+---+---+---+---+---+
        (5)  6 |   |   |   |   |   |   |   |   |
               +---+---+---+---+---+---+---+---+
        (6)  7 | p | p | p | p | p | p | p | p |
               +---+---+---+---+---+---+---+---+
        (7)  8 | r | n | b | k | q | b | n | r |
               +---+---+---+---+---+---+---+---+

  NOTE(Tejas): Flip Board is a pure Visual effect, the white pawns always need
               to promote on the 7th rank and the black pawns always need
               to promote on the 0th rank.
 */


namespace Chess {
    
struct Board {

    BitBoard wPawn, wKnight, wBishop, wRook, wQueen, wKing;
    BitBoard bPawn, bKnight, bBishop, bRook, bQueen, bKing;

    BitBoard wOccupied, bOccupied;

    struct PlayerInfo {
        bool king_moved;
        bool krook_moved;
        bool qrook_moved;
    };

    PlayerInfo white;
    PlayerInfo black;

    Player turn;
};

Board* initBoard(void);
void   cleanUpBoard(Board *board);

void setFen(Board *board, char* fen);
char* getFen(Board *board);

Piece getPieceAt(Board *board, Square square);
void  setPiece(Board *board, Square square, Piece piece);
void  removePiece(Board *board, Square square);
void  changeTurn(Board *board);

void resetBoard(Board *board);

} // namespace Chess

#endif // _BOARD_H_
