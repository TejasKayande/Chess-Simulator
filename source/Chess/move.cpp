
#include "move.h"

using namespace Chess;

internal BitBoard getPawnAttackingSquares(Board *board, Square square, Player player) {

    BitBoard legal = U64(0);
    BitBoard pawn_square_mask = CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(square.rank, square.file));

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
    BitBoard pawn_square_mask = CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(square.rank, square.file));

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
    BitBoard knight_square_mask = CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(square.rank, square.file));

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

    // TODO(Tejas): Refactor this!!!

    BitBoard legal = U64(0);
    BitBoard king_square_mask = CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(square.rank, square.file));

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

internal BitBoard getCastlingSquares(Board *board, Player player) {

    // TODO(Tejas): Refactor This!!!
    
    BitBoard legal = U64(0);

    // NOTE(Tejas): cant castle when in check
    if (isInCheck(board, player)) return legal;

    switch (player) {

    case Player::WHITE: {

        // king sid
        if (!(board->white.king_moved || board->white.krook_moved)) {

            Player   opp_player            = (player == Player::WHITE) ? Player::BLACK : Player::WHITE;
            BitBoard opp_attacking_squares = getAttackingSquares(board, opp_player);

            Square king_castle_square = { 0, 1 };
            Square middle_square      = { 0, 2 };

            Piece p_at_king_castle_square = getPieceAt(board, king_castle_square);
            Piece p_at_middle_square      = getPieceAt(board, middle_square);

            if (p_at_king_castle_square == EMPTY_SQUARE && p_at_middle_square == EMPTY_SQUARE) {

                BitBoard middle_square_mask =
                    CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(middle_square.rank, middle_square.file));
                BitBoard king_castle_square_mask =
                    CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(king_castle_square.rank, king_castle_square.file));
            
                if (!((king_castle_square_mask & opp_attacking_squares) ||
                      (middle_square_mask & opp_attacking_squares)))
                {
                    legal |= CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(king_castle_square.rank, king_castle_square.file));
                }
            }
        }

        // queen side
        if (!(board->white.king_moved || board->white.qrook_moved)) {

            Player   opp_player            = (player == Player::WHITE) ? Player::BLACK : Player::WHITE;
            BitBoard opp_attacking_squares = getAttackingSquares(board, opp_player);

            Square king_castle_square = { 0, 5 };
            Square middle_square      = { 0, 4 };

            Piece p_at_king_castle_square = getPieceAt(board, king_castle_square);
            Piece p_at_middle_square      = getPieceAt(board, middle_square);

            if (p_at_king_castle_square == EMPTY_SQUARE && p_at_middle_square == EMPTY_SQUARE) {

                BitBoard middle_square_mask =
                    CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(middle_square.rank, middle_square.file));
                BitBoard king_castle_square_mask =
                    CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(king_castle_square.rank, king_castle_square.file));
            
                if (!((king_castle_square_mask & opp_attacking_squares) ||
                      (middle_square_mask & opp_attacking_squares)))
                {
                    legal |= CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(king_castle_square.rank, king_castle_square.file));
                }
            }
        }

    } break;

    case Player::BLACK: {

        // king sid
        if (!(board->black.king_moved || board->black.krook_moved)) {

            Player   opp_player            = (player == Player::WHITE) ? Player::BLACK : Player::WHITE;
            BitBoard opp_attacking_squares = getAttackingSquares(board, opp_player);

            Square king_castle_square = { 7, 1 };
            Square middle_square      = { 7, 2 };

            Piece p_at_king_castle_square = getPieceAt(board, king_castle_square);
            Piece p_at_middle_square      = getPieceAt(board, middle_square);

            if (p_at_king_castle_square == EMPTY_SQUARE && p_at_middle_square == EMPTY_SQUARE) {

                BitBoard middle_square_mask =
                    CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(middle_square.rank, middle_square.file));
                BitBoard king_castle_square_mask =
                    CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(king_castle_square.rank, king_castle_square.file));
            
                if (!((king_castle_square_mask & opp_attacking_squares) ||
                      (middle_square_mask & opp_attacking_squares)))
                {
                    legal |= CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(king_castle_square.rank, king_castle_square.file));
                }
            }
        }

        // queen side
        if (!(board->black.king_moved || board->black.qrook_moved)) {

            Player   opp_player            = (player == Player::WHITE) ? Player::BLACK : Player::WHITE;
            BitBoard opp_attacking_squares = getAttackingSquares(board, opp_player);

            Square king_castle_square = { 7, 5 };
            Square middle_square      = { 7, 4 };

            Piece p_at_king_castle_square = getPieceAt(board, king_castle_square);
            Piece p_at_middle_square      = getPieceAt(board, middle_square);

            if (p_at_king_castle_square == EMPTY_SQUARE && p_at_middle_square == EMPTY_SQUARE) {

                BitBoard middle_square_mask =
                    CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(middle_square.rank, middle_square.file));
                BitBoard king_castle_square_mask =
                    CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(king_castle_square.rank, king_castle_square.file));
            
                if (!((king_castle_square_mask & opp_attacking_squares) ||
                      (middle_square_mask & opp_attacking_squares)))
                {
                    legal |= CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(king_castle_square.rank, king_castle_square.file));
                }
            }
        }

    } break;

    }

    return legal;
}

void Chess::move(Board *board, Move *move) {

    if (move->from == OFF_SQUARE || move->to == OFF_SQUARE) return;

    Piece p_from = getPieceAt(board, move->from);
    Piece p_to   = getPieceAt(board, move->to);

    move->piece  = p_from;
    move->player = board->turn;
    move->type   = MoveType::SIMPLE;

    switch (p_from.type) {

    case PType::KING: {

        int file_difference = move->to.file - move->from.file;

        // NOTE(Tejas): this checks if the king was requested to
        //              move to more distance than 1 square
        if (file_difference > 1 || file_difference < -1) {

            // NOTE(Tejas): Castling the King

            move->type = MoveType::CASTLE;

            if (move->from.file > move->to.file) move->castle_side = Side::KING_SIDE;
            if (move->from.file < move->to.file) move->castle_side = Side::QUEEN_SIDE;

            Piece king_piece = { PType::KING, move->player };
            Piece rook_piece = { PType::ROOK, move->player };

            removePiece(board, move->from);
            setPiece(board, move->to, king_piece);

            int rook_rank      = move->to.rank;
            int rook_from_file = (move->castle_side == Side::KING_SIDE) ? 0 : 7;
            int rook_to_file   = (move->castle_side == Side::KING_SIDE) ? move->to.file + 1 : move->to.file - 1;

            Square rook_from_square = { rook_rank, rook_from_file };
            Square rook_to_square   = { rook_rank, rook_to_file };

            removePiece(board, rook_from_square);
            setPiece(board, rook_to_square, rook_piece);

            return;
        }

    } break;

    }

    move->board = *board;

    if (p_to != EMPTY_SQUARE) move->type = MoveType::CAPTURE;
    move->captured_piece = p_to;

    removePiece(board, move->from);
    setPiece(board, move->to, move->piece);
}

void Chess::clearMove(Move *move) {
    memset(move, 0, sizeof(Move));
    move->from = OFF_SQUARE;
    move->to   = OFF_SQUARE;
}

void Chess::undoMove(Board *board, Move move) {

    // NOTE(Tejas): this is a very scuffed function. It should only be used
    //              when you are sure that the move passed was the latest move
    //              made on the board

    if (move.from == OFF_SQUARE || move.to == OFF_SQUARE) return;

    setPiece(board, move.from, move.piece);
    setPiece(board, move.to, move.captured_piece);

    switch (move.piece.type) {
        
    case PType::KING: {

        if (!(move.type == MoveType::CASTLE)) return;

        Piece  rook      = { };
        Square rook_from = { };
        Square rook_to   = { };

        if (move.player == Player::WHITE) rook = WHITE_ROOK;   
        if (move.player == Player::BLACK) rook = BLACK_ROOK;   

        if (move.castle_side == Side::KING_SIDE) {
            rook_from = { move.to.rank, 0 };
            rook_to   = { move.to.rank, move.to.file + 1 };
        }

        if (move.castle_side == Side::QUEEN_SIDE) {
            rook_from = { move.to.rank, 7 };
            rook_to   = { move.to.rank, move.to.file - 1 };
        }

        setPiece(board, rook_from, rook);
        setPiece(board, rook_to, EMPTY_SQUARE);

        if (rook == WHITE_ROOK) {
            board->white.king_moved = false;
            if (move.castle_side == Side::KING_SIDE)
                board->white.krook_moved = false;
            if (move.castle_side == Side::QUEEN_SIDE)
                board->white.qrook_moved = false;
        }

        if (rook == BLACK_ROOK) {
            board->black.king_moved = false;
            if (move.castle_side == Side::KING_SIDE)
                board->black.krook_moved = false;
            if (move.castle_side == Side::QUEEN_SIDE)
                board->black.qrook_moved = false;
        }
        
    } break;

    }
}

BitBoard Chess::getValidSquares(Board *board, Square square, Piece piece) {

    BitBoard valid = U64(0);

    switch (piece.type) {

    case PType::PAWN:
        valid = getPawnValidSquares(board, square, piece.color);
        break;
    case PType::KNIGHT:
        valid = getKnightValidSquares(board, square, piece.color);
        break;
    case PType::BISHOP:
        valid = getBishopValidSquares(board, square, piece.color);
        break;
    case PType::ROOK:
        valid = getRookValidSquares(board, square, piece.color);
        break;
    case PType::QUEEN:
        valid  = getRookValidSquares(board, square, piece.color);
        valid |= getBishopValidSquares(board, square, piece.color);
        break;
    case PType::KING:
        valid = getKingValidSquares(board, square, piece.color);
        break;
    }

    return valid;
}

BitBoard Chess::getAttackingSquares(Board *board, Player player) {

    BitBoard attacking_squares = U64(0);

    for (int i = 0; i < 64; i++) {

        Square square = { i / MAX_RANK, i % MAX_FILE };
        Piece piece = getPieceAt(board, square);

        BitBoard piece_attacking_square = U64(0);

        if (piece.type == PType::PAWN)
            piece_attacking_square = getPawnAttackingSquares(board, square, player);
        else
            piece_attacking_square = getValidSquares(board, square, piece);

        if (piece.color == player)
            attacking_squares |= piece_attacking_square;
    }

    return attacking_squares;
}

BitBoard Chess::getLegalSquares(Board *board, Square square, Piece piece) {

    BitBoard legal = getValidSquares(board, square, piece) | getCastlingSquares(board, board->turn);

    // filtering out the moves that put self king in check
    Square from   = square;
    Square to     = OFF_SQUARE;
    Player player = piece.color;

    Board copy_board = *board;

    for (int i = 0; i < 64; i++) {

        u64 mask = CREATE_BITBOARD_MASK(i);

        if (mask & legal) {

            to = GET_SQUARE_FROM_INDEX(i);

            Move move = { };
            move.from  = from;
            move.to    = to;

            // trying the move here
            Chess::move(&copy_board, &move);

            if (isInCheck(&copy_board, player)) {
                mask = ~mask;
                legal &= mask;
            }

            // undo the made move
            Chess::undoMove(&copy_board, move);
        }
    }

    return legal;
}

void Chess::promotePawn(Board *board, PType promote_to) {

    // NOTE(Tejas): assuming that the black pawns can never be on the 8th rank
    //              and white pawns can never be on the 2st rank.
    for (int file = 0; file < MAX_FILE; file++) {

        Square s_rank1 = { 0, file };
        Square s_rank8 = { 7, file };

        Piece p_rank1 = getPieceAt(board, s_rank1);
        Piece p_rank8 = getPieceAt(board, s_rank8);

        if (p_rank1.type == PType::PAWN) {
            Piece promotion_piece = { promote_to, p_rank1.color };
            setPiece(board, s_rank1, promotion_piece);
        }

        if (p_rank8.type == PType::PAWN) {
            Piece promotion_piece = { promote_to, p_rank8.color };
            setPiece(board, s_rank8, promotion_piece);
        }
    }
}

Square Chess::getKingPosition(Board *board, Player player) {

    switch (player) {

    case Player::WHITE: {

        int index = GET_FIRST_SET_BIT(board->wKing);
        Square s = GET_SQUARE_FROM_INDEX(index);
        return s;

    } break;

    case Player::BLACK: {

        int index = GET_FIRST_SET_BIT(board->bKing);
        Square s = GET_SQUARE_FROM_INDEX(index);
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

    if (!isInCheck(board, player)) return false;

    Square king_pos = getKingPosition(board, player);

    bool is_checkmate = true;
    BitBoard legal = U64(0);

    BitBoard player_occupied_bitboard = U64(0);

    if (player == Player::WHITE) player_occupied_bitboard = board->wOccupied;
    if (player == Player::BLACK) player_occupied_bitboard = board->bOccupied;

    for (int i = 0; i < 64; i++) {

        BitBoard mask = CREATE_BITBOARD_MASK(i);

        if (mask & player_occupied_bitboard) {
                
            Square square = GET_SQUARE_FROM_INDEX(i);
            Piece piece = getPieceAt(board, square);
            legal |= getLegalSquares(board, square, piece);

            if (legal) break;
        }
    }

    if (legal) is_checkmate = false;

    return is_checkmate;
}

bool Chess::isKingInCenter(Board *board, Player player) {

    Square center1 = { 3, 3 };
    Square center2 = { 3, 4 };
    Square center3 = { 4, 3 };
    Square center4 = { 4, 4 };

    Square king_pos = getKingPosition(board, player);

    if (king_pos == center1) return true;
    if (king_pos == center2) return true;
    if (king_pos == center3) return true;
    if (king_pos == center4) return true;

    return false;
}
