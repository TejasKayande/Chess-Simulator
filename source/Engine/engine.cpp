
#include "engine.h"

int Engine::moveGenerationTest(Chess::Board board, int depth) {

    if (depth == 0) return 1;

    int number_of_positions = 0;

    for (int from_rank = 0; from_rank < MAX_RANK - 1; from_rank++) {
        
        for (int from_file = 0; from_file < MAX_FILE - 1; from_file++) {

            Chess::Square from_square = { from_rank, from_file };
            Chess::Piece from_piece = Chess::getPieceAt(&board, from_square);

            if (from_piece == Chess::EMPTY_SQUARE || from_piece.color != board.turn) continue;

            Chess::BitBoard legal_squares = Chess::getLegalSquares(&board, from_square, from_piece);
            if (from_piece.type == Chess::PType::KING) legal_squares |= Chess::getCastlingSquares(&board, board.turn);

            if (!(legal_squares || U64(0))) return 1;

            int index = GET_FIRST_SET_BIT(legal_squares);
            Chess::Square to_square = GET_SQUARE_FROM_INDEX(index);
            
            Chess::Move move;
            move.from = from_square;
            move.to   = to_square;

            Chess::move(&board, &move);
            number_of_positions += moveGenerationTest(board, depth-1);
            Chess::undoMove(&board, move);
        }
    }

    return number_of_positions;
}

Chess::Move Engine::getFirstMove(Chess::Board board) {

    Chess::Move legal_move = { };
    Chess::clearMove(&legal_move);
    
    for (int from_rank = 0; from_rank < MAX_RANK - 1; from_rank++) {
        
        for (int from_file = 0; from_file < MAX_FILE - 1; from_file++) {

            Chess::Square from_square = { from_rank, from_file };
            Chess::Piece from_piece = Chess::getPieceAt(&board, from_square);

            if (from_piece == Chess::EMPTY_SQUARE || from_piece.color != board.turn) continue;

            Chess::BitBoard legal_squares = Chess::getLegalSquares(&board, from_square, from_piece);
            if (from_piece.type == Chess::PType::KING) legal_squares |= Chess::getCastlingSquares(&board, board.turn);

            int index = GET_FIRST_SET_BIT(legal_squares);
            Chess::Square to_square = GET_SQUARE_FROM_INDEX(index);

            Chess::Move move;
            move.from = from_square;
            move.to   = to_square;

            return move;
        }
    }

    return legal_move;
}
