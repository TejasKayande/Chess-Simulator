
#include "board.h"

using namespace Chess;

internal void updateBoardInfo(Board *board) {

    // check white
    Piece white_king = getPieceAt(board, Square{0, 3});
    if (white_king != WHITE_KING) board->white.king_moved = true;   

    Piece white_king_rook = getPieceAt(board, Square{0, 0});
    if (white_king_rook != WHITE_ROOK) board->white.krook_moved = true;      

    Piece white_queen_rook = getPieceAt(board, Square{0, 7});
    if (white_queen_rook != WHITE_ROOK) board->white.qrook_moved = true;

    // check black
    Piece black_king = getPieceAt(board, Square{7, 3});
    if (black_king != BLACK_KING) board->black.king_moved = true;

    Piece black_king_rook = getPieceAt(board, Square{7, 0});
    if (black_king_rook != BLACK_ROOK) board->black.krook_moved = true;

    Piece black_queen_rook = getPieceAt(board, Square{7, 7});
    if (black_queen_rook != BLACK_ROOK) board->black.qrook_moved = true;
    
    board->wOccupied = board->wPawn | board->wKnight | board->wBishop | board->wRook | board->wQueen | board->wKing;
    board->bOccupied = board->bPawn | board->bKnight | board->bBishop | board->bRook | board->bQueen | board->bKing;
}

Board* Chess::initBoard(void) {

    // TODO(Tejas): add memory pool in Platform Layer
    Board *board = (Board*)malloc(sizeof(Board));
    if (board == NULL) {
        printf("Couldn't allocate memory to Board\n");
        return NULL;
    }

    // Platform layer function
    memset(board, 0, sizeof(Board));

    setFen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk");

    return board;
}

void Chess::cleanUpBoard(Board **board) {

    free(*board);
    *board = NULL;
}

void Chess::setFen(Board *board, char *fen) {

    board->white.king_moved  = true; board->black.king_moved  = true;
    board->white.krook_moved = true; board->black.krook_moved = true;
    board->white.qrook_moved = true; board->black.qrook_moved = true;

    board->wOccupied = U64(0);
    board->bOccupied = U64(0);

    int rank = MAX_RANK - 1;
    int file = MAX_FILE - 1;

    // true when the position on the board is loaded and now additional
    // information about the position is being provided like castling info,
    // whose turn is it and so on...
    bool check_for_addition_data = false;

    char ch;
    while ((ch = *fen++) != '\0') {

        if (ch == ' ') {
            check_for_addition_data = true;
            continue;
        }

        if (check_for_addition_data) {

            if (ch == 'Q') {
                board->white.qrook_moved = false;
                board->white.king_moved  = false;
                continue;
            }

            if (ch == 'q') {
                board->black.qrook_moved = false;
                board->black.king_moved  = false;
                continue;
            }

            if (ch == 'K') {
                board->white.krook_moved = false;
                board->white.king_moved  = false;
                continue;
            }

            if (ch == 'k') {
                board->black.krook_moved = false;
                board->black.king_moved  = false;
                continue;
            }

            if (ch == 'w') {
                board->turn = Player::WHITE;
                continue;
            }

            if (ch == 'b') {
                board->turn = Player::BLACK;
                continue;
            }
        }

        else {

            if (file < 0) file = MAX_FILE - 1;

            if (ch == '/') {
                rank--;
                file = MAX_FILE - 1;
                continue;
            }

            // TODO(Tejas): here the value should be less the MAX_FILE
            if (isdigit(ch)) {
                int num = (int)(ch - '0');
                for (int i = 0; i < num; i++) {
                    setPiece(board, Square{rank, file}, EMPTY_SQUARE);
                    file--;
                }
                continue;
            }

            Square square = { rank, file };

            switch (ch) {
            case 'P': setPiece(board, Square{rank, file}, WHITE_PAWN);   break;
            case 'N': setPiece(board, Square{rank, file}, WHITE_KNIGHT); break;
            case 'B': setPiece(board, Square{rank, file}, WHITE_BISHOP); break;
            case 'R': setPiece(board, Square{rank, file}, WHITE_ROOK);   break;
            case 'Q': setPiece(board, Square{rank, file}, WHITE_QUEEN);  break;
            case 'K': setPiece(board, Square{rank, file}, WHITE_KING);   break;

            case 'p': setPiece(board, Square{rank, file}, BLACK_PAWN);   break;
            case 'n': setPiece(board, Square{rank, file}, BLACK_KNIGHT); break;
            case 'b': setPiece(board, Square{rank, file}, BLACK_BISHOP); break;
            case 'r': setPiece(board, Square{rank, file}, BLACK_ROOK);   break;
            case 'q': setPiece(board, Square{rank, file}, BLACK_QUEEN);  break;
            case 'k': setPiece(board, Square{rank, file}, BLACK_KING);   break;
                // TODO(Tejas): add invalid piece handling
            default: break;
            }

            file--;
        }
    }
}

char* Chess::getFen(Board *board) {

    (void*)(board);
    return NULL;
}

Piece Chess::getPieceAt(Board *board, Square square) {

    BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(square.rank, square.file);

    if (board->wOccupied & mask) {

        if (board->wPawn   & mask) return Piece { PType::PAWN  , PColor::WHITE };
        if (board->wKnight & mask) return Piece { PType::KNIGHT, PColor::WHITE };
        if (board->wBishop & mask) return Piece { PType::BISHOP, PColor::WHITE };
        if (board->wRook   & mask) return Piece { PType::ROOK  , PColor::WHITE };
        if (board->wQueen  & mask) return Piece { PType::QUEEN , PColor::WHITE };
        if (board->wKing   & mask) return Piece { PType::KING  , PColor::WHITE };
    }

    if (board->bOccupied & mask) {

        if (board->bPawn   & mask) return Piece { PType::PAWN  , PColor::BLACK };
        if (board->bKnight & mask) return Piece { PType::KNIGHT, PColor::BLACK };
        if (board->bBishop & mask) return Piece { PType::BISHOP, PColor::BLACK };
        if (board->bRook   & mask) return Piece { PType::ROOK  , PColor::BLACK };
        if (board->bQueen  & mask) return Piece { PType::QUEEN , PColor::BLACK };
        if (board->bKing   & mask) return Piece { PType::KING  , PColor::BLACK };
    }

    return Piece { PType::NO_PIECE, PColor::NO_COLOR };
}

void Chess::setPiece(Board *board, Square square, Piece piece) {

    BitBoard mask = U64(1) << GET_INDEX_FROM_SQUARE(square.rank, square.file);
    
    // TODO(Tejas): add invalid piece handling

    // NOTE(Tejas): setting the bit to 0 from all the bitboards
    BitBoard n_mask = ~(mask);
    board->wPawn   &= n_mask; board->bPawn   &= n_mask;
    board->wKnight &= n_mask; board->bKnight &= n_mask;
    board->wBishop &= n_mask; board->bBishop &= n_mask;
    board->wRook   &= n_mask; board->bRook   &= n_mask;
    board->wQueen  &= n_mask; board->bQueen  &= n_mask;
    board->wKing   &= n_mask; board->bKing   &= n_mask;

    // TODO(Tejas): Change these to switch
    if      (piece == WHITE_PAWN)   board->wPawn   |= mask;
    else if (piece == BLACK_PAWN)   board->bPawn   |= mask;
    else if (piece == WHITE_KNIGHT) board->wKnight |= mask;
    else if (piece == BLACK_KNIGHT) board->bKnight |= mask;
    else if (piece == WHITE_BISHOP) board->wBishop |= mask;
    else if (piece == BLACK_BISHOP) board->bBishop |= mask;
    else if (piece == WHITE_ROOK)   board->wRook   |= mask;
    else if (piece == BLACK_ROOK)   board->bRook   |= mask;
    else if (piece == WHITE_QUEEN)  board->wQueen  |= mask;
    else if (piece == BLACK_QUEEN)  board->bQueen  |= mask;
    else if (piece == WHITE_KING)   board->wKing   |= mask;
    else if (piece == BLACK_KING)   board->bKing   |= mask;

    updateBoardInfo(board);
}

void Chess::removePiece(Board *board, Square square) {

    BitBoard mask = ~(U64(1) << GET_INDEX_FROM_SQUARE(square.rank, square.file));

    board->wPawn   &= mask; board->bPawn   &= mask;
    board->wKnight &= mask; board->bKnight &= mask;
    board->wBishop &= mask; board->bBishop &= mask;
    board->wRook   &= mask; board->bRook   &= mask;
    board->wQueen  &= mask; board->bQueen  &= mask;
    board->wKing   &= mask; board->bKing   &= mask;
    
    updateBoardInfo(board);
}

void Chess::changeTurn(Board *board) {

    board->turn = (board->turn == Player::WHITE) ? Player::BLACK : Player::WHITE;
}

void Chess::resetBoard(Board *board) {

    memset(board, 0, sizeof(Board));
    setFen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk");
}
