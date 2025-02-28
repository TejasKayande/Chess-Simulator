
#include "move.h"

using namespace Chess;

internal BitBoard getPawnLegalMoves(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard pawn_square_mask = U64(1) << GET_INDEX(square.rank, square.file);

    // TODO(Tejas): Add promotion and enpassant

    switch (player) {

    case Player::WHITE: {

        // forward movement
        legal = pawn_square_mask << MAX_RANK;
        legal &= ~(board->wOccupied | board->bOccupied);

        if (square.rank == 1 && legal != U64(0))
            legal |= pawn_square_mask << (MAX_RANK * 2);

        legal &= ~(board->wOccupied | board->bOccupied);

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

        // forward movement
        legal = pawn_square_mask >> 8;
        legal &= ~(board->wOccupied | board->bOccupied);

        if (square.rank == 6 && legal != U64(0))
            legal |= pawn_square_mask >> (MAX_RANK * 2);

        legal &= ~(board->wOccupied | board->bOccupied);

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

internal BitBoard getKnightLegalMoves(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard knight_square_mask = U64(1) << GET_INDEX(square.rank, square.file);

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

internal BitBoard getBishopLegalMoves(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard bishop_square_mask = U64(1) << GET_INDEX(square.rank, square.file);

    // TODO(Tejas): Refactor this!!!

    for (int rank = square.rank + 1, file = square.file + 1;
         rank < MAX_RANK && file < MAX_FILE;
         rank++, file++)
    {
        BitBoard mask = U64(1) << GET_INDEX(rank, file);

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
        BitBoard mask = U64(1) << GET_INDEX(rank, file);

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
        BitBoard mask = U64(1) << GET_INDEX(rank, file);

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
        BitBoard mask = U64(1) << GET_INDEX(rank, file);

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

internal BitBoard getRookLegalMoves(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard bishop_square_mask = U64(1) << GET_INDEX(square.rank, square.file);

    // TODO(Tejas): Refactor this!!!

    for (int rank = square.rank + 1; rank < MAX_RANK; rank++) {

        BitBoard mask = U64(1) << GET_INDEX(rank, square.file);

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

        BitBoard mask = U64(1) << GET_INDEX(rank, square.file);

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
        
        BitBoard mask = U64(1) << GET_INDEX(square.rank, file);

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
        
        BitBoard mask = U64(1) << GET_INDEX(square.rank, file);

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

internal BitBoard getKingLegalMoves(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard king_square_mask = U64(1) << GET_INDEX(square.rank, square.file);

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

BitBoard Chess::getLegalMoves(Board *board, Square square, Piece piece) {

    BitBoard legal = U64(0);

    switch (piece.type) {
    case PType::PAWN:
        legal = getPawnLegalMoves(board, square, piece.color);
        break;
    case PType::KNIGHT:
        legal = getKnightLegalMoves(board, square, piece.color);
        break;
    case PType::BISHOP:
        legal = getBishopLegalMoves(board, square, piece.color);
        break;
    case PType::ROOK:
        legal = getRookLegalMoves(board, square, piece.color);
        break;
    case PType::QUEEN:
        legal  = getRookLegalMoves(board, square, piece.color);
        legal |= getBishopLegalMoves(board, square, piece.color);
        break;
    case PType::KING:
        legal = getKingLegalMoves(board, square, piece.color);
        break;
    }

    return legal;
}
