
#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "../Platform/platform.h"

// NOTE(Tejas): MAX_FILE and MAX_RANK start counting from 1 whereas indexing
//              on the board start from 0. MAX_FILE and MAX_RANK are adjust
//              with -1 where they are begin compared with a Square.
#define MAX_FILE 8
#define MAX_RANK 8

#define U64(x) x##ULL
#define GET_INDEX(rank, file) ((rank) * (MAX_FILE) + (file))

#define PIECE_COUNT 6
#define COLOR_COUNT 2

namespace Chess {

struct Square {

    int rank;
    int file;

    inline bool operator==(const Square &square) const {
        return (rank == square.rank && file == square.file);
    }

    inline bool operator!=(const Square &square) const {
        return (rank != square.rank || file != square.file);
    }
};

struct Piece {

    enum Type {
        NO_PIECE,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    } type;

    enum Color {

        NO_COLOR,
        WHITE,
        BLACK
    } color;

    inline bool operator==(const Piece& piece) const {
        return (type == piece.type && color == piece.color);
    }

    inline bool operator!=(const Piece& piece) const {
        return (type != piece.type || color != piece.color);
    }
};

enum Side {
    NONE = 0,
    KING_SIDE,
    QUEEN_SIDE
};

typedef Piece::Type  PType;
typedef Piece::Color PColor;
typedef Piece::Color Player;
typedef u64          BitBoard;

const Piece EMPTY_SQUARE = { PType::NO_PIECE  , PColor::NO_COLOR };
const Piece WHITE_PAWN   = { PType::PAWN      , PColor::WHITE };
const Piece BLACK_PAWN   = { PType::PAWN      , PColor::BLACK };
const Piece WHITE_KNIGHT = { PType::KNIGHT    , PColor::WHITE };
const Piece BLACK_KNIGHT = { PType::KNIGHT    , PColor::BLACK };
const Piece WHITE_BISHOP = { PType::BISHOP    , PColor::WHITE };
const Piece BLACK_BISHOP = { PType::BISHOP    , PColor::BLACK };
const Piece WHITE_ROOK   = { PType::ROOK      , PColor::WHITE };
const Piece BLACK_ROOK   = { PType::ROOK      , PColor::BLACK };
const Piece WHITE_QUEEN  = { PType::QUEEN     , PColor::WHITE };
const Piece BLACK_QUEEN  = { PType::QUEEN     , PColor::BLACK };
const Piece WHITE_KING   = { PType::KING      , PColor::WHITE };
const Piece BLACK_KING   = { PType::KING      , PColor::BLACK };

const Square OFF_SQUARE = { -1, -1 };

} // namespace Chess

#endif // _DEFINES_H_
