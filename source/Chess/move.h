
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

    enum MoveType {
        SIMPLE = 0,
        CAPTURE,
        CASTLE,
        PROMOTION,
        ENPASSANT
    } type;

    Piece captured_piece;  // if the move was a caputer move, this holds what piece was captuered
    Side  castle_side;

    char* fen;         // The FEN of the board after the move was made
};

void move(Board *board, Move *move);
void undoMove(Board *board, Move move);

void clearMove(Move *move);

BitBoard getValidSquares(Board *board, Square square, Piece piece);
BitBoard getAttackingSquares(Board *board, Player player);
BitBoard getCastlingSquares(Board *board, Player player);
BitBoard getLegalSquares(Board *board, Square square, Piece piece);

Square getKingPosition(Board *board, Player player);

bool isInCheck(Board *board, Player player);
bool isCheckMate(Board *board, Player player);

} // namespace Chess

typedef Chess::Move::MoveType MoveType;

#endif // _MOVE_H_
