
#include "renderer.h"

// NOTE(Tejas): In the renderer the boards rank and file start at 0
//              for the pixel calculation purposes

using namespace Chess;

// TODO(Tejas): Lets not hard code pixel values!
global const int SQUARE_SIZE = 100;

internal const Platform::TexID G_textures[COLOR_COUNT][PIECE_COUNT] = {

    { Platform::WPAWN, Platform::WKNIGHT, Platform::WBISHOP,
      Platform::WROOK, Platform::WQUEEN , Platform::WKING  },

    { Platform::BPAWN, Platform::BKNIGHT, Platform::BBISHOP,
      Platform::BROOK, Platform::BQUEEN , Platform::BKING  }
};

internal void fillSquare(Square square, Color color, bool is_flipped) {

    if (square == OFF_SQUARE) return;

    if (is_flipped) {
        square.rank = (MAX_RANK - 1) - square.rank;
        square.file = (MAX_RANK - 1) - square.file;
    }

    int x = square.file * SQUARE_SIZE;
    int y = square.rank * SQUARE_SIZE;

    Platform::fillRect(x, y, SQUARE_SIZE, SQUARE_SIZE, color);
}

internal void renderPiece(Square square, Piece piece, bool is_flipped) {

    if (is_flipped) {
        square.rank = (MAX_RANK - 1) - square.rank;
        square.file = (MAX_RANK - 1) - square.file;
    }

    int x = square.file * SQUARE_SIZE;
    int y = square.rank * SQUARE_SIZE;

    if (piece.type != Piece::NO_PIECE)
        Platform::renderTexture(x, y, G_textures[piece.color - 1][piece.type - 1]);
}

internal void renderRankCoord(Square s, bool is_flipped) {

    Color c = 0x00000000;

    bool is_dark_square = (s.rank + s.file) % 2;
    if (is_dark_square) c = 0xFFFFFFFF;
    else c = 0xFF000000;

    char b[2];
    if (is_flipped)
        sprintf_s(b, sizeof(b), "%d", MAX_RANK - s.rank);
    else 
        sprintf_s(b, sizeof(b), "%d", s.rank+ 1);
        
    int px = 2, py = 2;

    int x = s.file * SQUARE_SIZE + px;
    int y = s.rank * SQUARE_SIZE + py;
    Platform::renderFont(b, x, y, FontType::VERY_SMALL, c);
}

internal void renderFileCoord(Square s, bool is_flipped) {

    Color c = 0x00000000;

    bool is_dark_square = (s.rank + s.file) % 2;
    if (is_dark_square) c = 0xFFFFFFFF;
    else c = 0xFF000000;

    char b[2];

    if (is_flipped)
        sprintf_s(b, sizeof(b), "%c", ('a' - 1) + s.file + 1);
    else
        sprintf_s(b, sizeof(b), "%c", 'h' - s.file);

    int px = 80, py = 72;

    int x = s.file * SQUARE_SIZE + px;
    int y = s.rank * SQUARE_SIZE + py;
    Platform::renderFont(b, x, y, FontType::VERY_SMALL, c);
}

void renderBoard(Board *board, VisualSetting &vs) {

    for (int rank = 0; rank < MAX_RANK; rank++) {

        for (int file = 0; file < MAX_FILE; file++) {

            Square current_square = { rank, file };

            if ((rank + file) % 2 == 0) {
                fillSquare(current_square, vs.theme.light, vs.is_board_flipped);
            } else {
                fillSquare(current_square, vs.theme.dark, vs.is_board_flipped);
            }

            Chess::BitBoard mask = U64(1) << GET_INDEX(rank, file);

            if (vs.legal_squares & mask) {
                fillSquare(current_square, vs.theme.legal, vs.is_board_flipped);
            }

            if (vs.selected_square == current_square)
                fillSquare(vs.selected_square, vs.theme.high, vs.is_board_flipped);

            Piece piece = getPieceAt(board, current_square);
            if (vs.selected_square != current_square)
                renderPiece(current_square, piece, vs.is_board_flipped);
        }
    }

    for (int i = 0; i < 8; i++) {
        renderRankCoord(Square{ i, 0 }, vs.is_board_flipped);
        renderFileCoord(Square{ 7, i }, vs.is_board_flipped);
    }

    // NOTE(Tejas): the selected piece follows the cursor around
    if (vs.selected_square != OFF_SQUARE) {

        int width, height;
        Platform::getWindowDimention(&width, &height);

        int x = vs.mousex;
        int y = vs.mousey;

        x = MIN(width - (TEXTURE_DIM / 2), x);
        y = MIN(height - (TEXTURE_DIM / 2), y);

        x = MAX(0, x);
        y = MAX(0, y);

        x -= TEXTURE_DIM / 2;
        y -= TEXTURE_DIM / 2;

        Piece piece = getPieceAt(board, vs.selected_square);
        Platform::renderTexture(x, y, G_textures[piece.color - 1][piece.type - 1]);
    }
}

Square pixelToBoard(int x, int y, bool is_flipped) {
    
    Square square = { };

    square.rank = y / SQUARE_SIZE;
    square.file = x / SQUARE_SIZE;

    if (is_flipped) {
        square.rank = (MAX_RANK - 1) - square.rank;
        square.file = (MAX_RANK - 1) - square.file;
    }

    return square;
}
