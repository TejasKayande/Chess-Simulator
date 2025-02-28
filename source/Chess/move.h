
#ifndef _MOVE_H_
#define _MOVE_H_

#include "defines.h"
#include "board.h"

namespace Chess {

struct Move {

    Player player;     // The Player that made the move
    Piece  piece;      // The piece that was moved

    Square from;       // piece's square before the move
    Square to;         // piece's square after the move

    bool castle;       // was the move a castle move
    Side castle_side;  // King side or Queen side if castled

    bool promotion;    // was the move a pawn promotion
    bool enpassant;    // was the move an anpassant capture 

    char* fen;         // The FEN of the board after the move was made
};

void move(Board *board, Move move);

BitBoard getLegalMoves(Board *board, Square square, Piece piece);

} // namespace Chess

#endif // _MOVE_H_
