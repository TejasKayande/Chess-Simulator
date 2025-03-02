
#include "move.h"

using namespace Chess;

internal BitBoard getPawnAttackingSquares(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard pawn_square_mask = U64(1) << GET_INDEX_FROM_SQUARE(square.rank, square.file);

    switch (player) {

    case Player::WHITE: {

        // captures
        if (square.file != 0) {
            BitBoard pawn_right_diagonal_mask = pawn_square_mask << (MAX_RANK - 1);
            if (pawn_right_diagonal_mask & board->bOccupied) legal |= pawn_right_diagonal_mask;
        }

        if (square.file != MAX_FILE - 1) {
            BitBoard pawn_left_diagonal_mask = pawn_square_mask << (MAX_RANK + 1);
            if (pawn_left_diagonal_mask & board->bOccupied) legal |= pawn_left_diagonal_mask;
        }
        
    } break;

    case Player::BLACK: {

        // captures
        if (square.file != MAX_FILE - 1) {
            BitBoard pawn_right_diagonal_mask = pawn_square_mask >> (MAX_RANK - 1);
            if (pawn_right_diagonal_mask & board->wOccupied) legal |= pawn_right_diagonal_mask;
        }

        if (square.file != 0) {
            BitBoard pawn_left_diagonal_mask = pawn_square_mask >> (MAX_RANK + 1);
            if (pawn_left_diagonal_mask & board->wOccupied) legal |= pawn_left_diagonal_mask;
        }

    } break;
        
    }

    return legal;
}

internal BitBoard getPawnValidSquares(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard pawn_square_mask = U64(1) << GET_INDEX_FROM_SQUARE(square.rank, square.file);

    // TODO(Tejas): Add promotion and enpassant

    switch (player) {

    case Player::WHITE: {

        // forward movement
        legal = pawn_square_mask << MAX_RANK;
        legal &= ~(board->wOccupied | board->bOccupied);

        if (square.rank == 1 && legal != U64(0))
            legal |= pawn_square_mask << (MAX_RANK * 2);

        legal &= ~(board->wOccupied | board->bOccupied);
        
    } break;

    case Player::BLACK: {

        // forward movement
        legal = pawn_square_mask >> 8;
        legal &= ~(board->wOccupied | board->bOccupied);

        if (square.rank == 6 && legal != U64(0))
            legal |= pawn_square_mask >> (MAX_RANK * 2);

        legal &= ~(board->wOccupied | board->bOccupied);

    } break;
        
    }

    // NOTE(Tejas): unlike other pieces pawns attacking squares and
    //              are not all the squares the pawn can move to
    legal |= getPawnAttackingSquares(board, square, player);

    return legal;
}

internal BitBoard getKnightValidSquares(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard knight_square_mask = U64(1) << GET_INDEX_FROM_SQUARE(square.rank, square.file);

    // TOP RIGHT
    if (square.file != 0)
        legal |= knight_square_mask << ((MAX_RANK * 2) - 1);
    if (square.file > 1)
        legal |= knight_square_mask << (MAX_RANK - 2);

    // TOP LEFT
    if (square.file != (MAX_FILE - 1))
        legal |= knight_square_mask << ((MAX_RANK * 2) + 1);
    if (square.file < (MAX_FILE - 1) - 1)
        legal |= knight_square_mask << (MAX_RANK + 2);

    // BOTTOM RIGHT
    if (square.file != 0)
        legal |= knight_square_mask >> ((MAX_RANK * 2) + 1);
    if (square.file > 1)
        legal |= knight_square_mask >> (MAX_RANK + 2);

    // BOTTOM LEFT 
    if (square.file != (MAX_FILE - 1))
        legal |= knight_square_mask >> ((MAX_RANK * 2) - 1);
    if (square.file < (MAX_FILE - 1) - 1)
        legal |= knight_square_mask >> (MAX_RANK - 2);

    if (player == Player::WHITE) legal &= ~(board->wOccupied);
    if (player == Player::BLACK) legal &= ~(board->bOccupied);

    return legal;
}

internal BitBoard getBishopValidSquares(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);

    // TODO(Tejas): Refactor this!!!

    for (int rank = square.rank + 1, file = square.file + 1;
         rank < MAX_RANK && file < MAX_FILE;
         rank++, file++)
    {
        BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(rank, file);

        if (player == Player::WHITE) {
            if (board->wOccupied & mask) break;
            if (board->bOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        if (player == Player::BLACK) {
            if (board->bOccupied & mask) break;
            if (board->wOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        legal |= mask;
    }

    for (int rank = square.rank + 1, file = square.file - 1;
         rank < MAX_RANK && file >= 0;
         rank++, file--)
    {
        BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(rank, file);

        if (player == Player::WHITE) {
            if (board->wOccupied & mask) break;
            if (board->bOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        if (player == Player::BLACK) {
            if (board->bOccupied & mask) break;
            if (board->wOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        legal |= mask;
    }

    for (int rank = square.rank - 1, file = square.file + 1;
         rank >= 0 && file < MAX_FILE;
         rank--, file++)
    {
        BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(rank, file);

        if (player == Player::WHITE) {
            if (board->wOccupied & mask) break;
            if (board->bOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        if (player == Player::BLACK) {
            if (board->bOccupied & mask) break;
            if (board->wOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        legal |= mask;
    }

    for (int rank = square.rank - 1, file = square.file - 1;
         rank >= 0 && file >= 0;
         rank--, file--)
    {
        BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(rank, file);

        if (player == Player::WHITE) {
            if (board->wOccupied & mask) break;
            if (board->bOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        if (player == Player::BLACK) {
            if (board->bOccupied & mask) break;
            if (board->wOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        legal |= mask;
    }

    return legal;
}

internal BitBoard getRookValidSquares(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);

    // TODO(Tejas): Refactor this!!!

    for (int rank = square.rank + 1; rank < MAX_RANK; rank++) {

        BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(rank, square.file);

        if (player == Player::WHITE) {
            if (board->wOccupied & mask) break;
            if (board->bOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        if (player == Player::BLACK) {
            if (board->bOccupied & mask) break;
            if (board->wOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        legal |= mask;
    }

    for (int rank = square.rank - 1; rank >= 0; rank--) {

        BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(rank, square.file);

        if (player == Player::WHITE) {
            if (board->wOccupied & mask) break;
            if (board->bOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        if (player == Player::BLACK) {
            if (board->bOccupied & mask) break;
            if (board->wOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        legal |= mask;
    }

    for (int file = square.file + 1; file < MAX_FILE; file++) {
        
        BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(square.rank, file);

        if (player == Player::WHITE) {
            if (board->wOccupied & mask) break;
            if (board->bOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        if (player == Player::BLACK) {
            if (board->bOccupied & mask) break;
            if (board->wOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        legal |= mask;
    }

    for (int file = square.file - 1; file >= 0; file--) {
        
        BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(square.rank, file);

        if (player == Player::WHITE) {
            if (board->wOccupied & mask) break;
            if (board->bOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        if (player == Player::BLACK) {
            if (board->bOccupied & mask) break;
            if (board->wOccupied & mask) {
                legal |= mask;
                break;
            } 
        }

        legal |= mask;
    }

    return legal;
}

internal BitBoard getKingValidSquares(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard king_square_mask = U64(1) << GET_INDEX_FROM_SQUARE(square.rank, square.file);

    if (square.file != MAX_RANK - 1)
        legal |= king_square_mask << (MAX_RANK + 1);
    legal |= king_square_mask << (MAX_RANK + 0);
    if (square.file != 0)
        legal |= king_square_mask << (MAX_RANK - 1);

    if (square.file != MAX_RANK - 1)
        legal |= king_square_mask << 1;
    if (square.file != 0)
        legal |= king_square_mask >> 1;

    if (square.file != MAX_RANK - 1)
        legal |= king_square_mask >> (MAX_RANK - 1);
    legal |= king_square_mask >> (MAX_RANK + 0);
    if (square.file != 0)
        legal |= king_square_mask >> (MAX_RANK + 1);


    if (player == Player::WHITE) legal &= ~(board->wOccupied);
    if (player == Player::BLACK) legal &= ~(board->bOccupied);
    
    return legal;
}

void Chess::move(Board *board, Move move) {

    if (move.from == OFF_SQUARE || move.to == OFF_SQUARE) return;

    removePiece(board, move.from);
    setPiece(board, move.to, move.piece);
}

BitBoard Chess::getValidSquares(Board *board, Square square, Piece piece) {

    BitBoard legal = U64(0);

    switch (piece.type) {
    case PType::PAWN:
        legal = getPawnValidSquares(board, square, piece.color);
        break;
    case PType::KNIGHT:
        legal = getKnightValidSquares(board, square, piece.color);
        break;
    case PType::BISHOP:
        legal = getBishopValidSquares(board, square, piece.color);
        break;
    case PType::ROOK:
        legal = getRookValidSquares(board, square, piece.color);
        break;
    case PType::QUEEN:
        legal  = getRookValidSquares(board, square, piece.color);
        legal |= getBishopValidSquares(board, square, piece.color);
        break;
    case PType::KING:
        legal = getKingValidSquares(board, square, piece.color);
        break;
    }

    return legal;
}

BitBoard Chess::getAttackingSquares(Board *board, Player player) {

    BitBoard attacking_squares = U64(0);

    for (int i = 0; i < 64; i++) {

        Square square = { i / MAX_RANK, i % MAX_FILE };
        Piece piece = getPieceAt(board, square);

        BitBoard piece_attacking_square = U64(0);

        if (piece.type == PType::PAWN) {
            piece_attacking_square = getPawnAttackingSquares(board, square, player);
        } else {
            piece_attacking_square = getValidSquares(board, square, piece);
        }

        if (piece.color == player)
            attacking_squares |= piece_attacking_square;
    }

    return attacking_squares;
}

Square Chess::getKingPosition(Board *board, Player player) {

    switch (player) {

    case Player::WHITE: {

        int index = GET_FIST_SET_BIT(board->wKing);

        Square s = GET_SQUARE_FROM_INDEX(index);
        
        // s.file = index % MAX_FILE;
        // s.rank = index / MAX_RANK;

        return s;

    } break;

    case Player::BLACK: {

        int index = GET_FIST_SET_BIT(board->bKing);

        Square s;
        s.file = index % 8;
        s.rank = index / 8;

        return s;

    } break;
        
    }

    return OFF_SQUARE;
}

bool Chess::isInCheck(Board *board, Player player) {

    Player attacker = (player == Player::WHITE) ? Player::BLACK : Player::WHITE;
    BitBoard attacking_squares = getAttackingSquares(board, attacker);

    BitBoard king_pos = U64(0);
    if (player == Player::WHITE) king_pos = board->wKing;
    if (player == Player::BLACK) king_pos = board->bKing;

    return ( king_pos & attacking_squares);
}

bool Chess::isCheckMate(Board *board, Player player) {

    Square king_pos = getKingPosition(board, player);


    return false;
}
