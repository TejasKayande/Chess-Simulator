
#include "renderer.h"

// NOTE(Tejas): In the renderer the boards rank and file start at 0
//              for the pixel calculation purposes

// TODO(Tejas): Seperate all the rendering components (logs, board and winner)

using namespace Chess;

// TODO(Tejas): do something better than this
global int G_logMessageCounter = 0;

struct BoardInfo {

    // offsets inside window
    int x_offset;
    int y_offset;

    bool is_flipped;

    ColorTheme theme;

    int square_size; // size of a square in pixel
    int texture_dim; // the size of a texture (square)
}; 
global BoardInfo G_boardInfo;


internal const Platform::TexID G_textures[COLOR_COUNT][PIECE_COUNT] = {

    { Platform::WPAWN, Platform::WKNIGHT, Platform::WBISHOP,
      Platform::WROOK, Platform::WQUEEN , Platform::WKING  },

    { Platform::BPAWN, Platform::BKNIGHT, Platform::BBISHOP,
      Platform::BROOK, Platform::BQUEEN , Platform::BKING  }
};

internal void fillSquare(Square square, Color color) {

    if (square == OFF_SQUARE) return;

    if (G_boardInfo.is_flipped) {
        square.rank = (MAX_RANK - 1) - square.rank;
        square.file = (MAX_RANK - 1) - square.file;
    }

    int x = square.file * G_boardInfo.square_size + G_boardInfo.x_offset;
    int y = square.rank * G_boardInfo.square_size + G_boardInfo.y_offset;

    Platform::fillRect(x, y, G_boardInfo.square_size, G_boardInfo.square_size, color);
}

internal void outlineSquare(Square square, f32 line_thinkness, Color color) {

    if (square == OFF_SQUARE) return;

    if (G_boardInfo.is_flipped) {
        square.rank = (MAX_RANK - 1) - square.rank;
        square.file = (MAX_RANK - 1) - square.file;
    }

    int x = square.file * G_boardInfo.square_size + G_boardInfo.x_offset;
    int y = square.rank * G_boardInfo.square_size + G_boardInfo.y_offset;

    Platform::drawRect(x, y, G_boardInfo.square_size, G_boardInfo.square_size, line_thinkness, color);
}

internal void renderPieceOnSquare(Square square, Piece piece) {

    if (G_boardInfo.is_flipped) {
        square.rank = (MAX_RANK - 1) - square.rank;
        square.file = (MAX_RANK - 1) - square.file;
    }

    int x = square.file * G_boardInfo.square_size + G_boardInfo.x_offset;
    int y = square.rank * G_boardInfo.square_size + G_boardInfo.y_offset;

    if (piece.type != Piece::NO_PIECE)
        Platform::renderTexture(x, y, G_boardInfo.texture_dim, G_boardInfo.texture_dim,
                                G_textures[piece.color - 1][piece.type - 1]);
}

internal void renderPieceOnMouse(int mousex, int mousey, Piece piece) {
    
    int width  = (G_boardInfo.square_size * MAX_FILE) + G_boardInfo.x_offset;
    int height = (G_boardInfo.square_size * MAX_RANK) + G_boardInfo.y_offset;

    int x = mousex;
    int y = mousey;

    x = MIN(width  - (G_boardInfo.texture_dim / 2), x);
    y = MIN(height - (G_boardInfo.texture_dim / 2), y);

    x = MAX(G_boardInfo.x_offset + (G_boardInfo.texture_dim / 2), x);
    y = MAX(G_boardInfo.y_offset + (G_boardInfo.texture_dim / 2), y);

    x -= G_boardInfo.texture_dim / 2;
    y -= G_boardInfo.texture_dim / 2;

    Platform::renderTexture(x, y, G_boardInfo.texture_dim, G_boardInfo.texture_dim,
                            G_textures[piece.color - 1][piece.type - 1]);
}

internal void renderRankCoord(Square s) {

    Color c = 0x00000000;

    bool is_dark_square = (s.rank + s.file) % 2;
    if (is_dark_square) c = G_boardInfo.theme.light;
    else c = G_boardInfo.theme.dark;

    char b[2];
    if (G_boardInfo.is_flipped)
        sprintf_s(b, sizeof(b), "%d", MAX_RANK - s.rank);
    else 
        sprintf_s(b, sizeof(b), "%d", s.rank+ 1);
        
    int px = 2, py = 2;

    int x = s.file * G_boardInfo.square_size + px + G_boardInfo.x_offset;
    int y = s.rank * G_boardInfo.square_size + py + G_boardInfo.y_offset;
    Platform::renderFont(b, x, y, FontType::NORMAL, c);
}

internal void renderFileCoord(Square s) {

    Color c = 0x00000000;

    bool is_dark_square = (s.rank + s.file) % 2;
    if (is_dark_square) c = G_boardInfo.theme.light;
    else c = G_boardInfo.theme.dark;

    char b[2];

    if (G_boardInfo.is_flipped)
        sprintf_s(b, sizeof(b), "%c", ('a' - 1) + s.file + 1);
    else
        sprintf_s(b, sizeof(b), "%c", 'h' - s.file);

    int px = (int)(G_boardInfo.square_size * 0.8f), py = (int)(G_boardInfo.square_size * 0.7f);

    int x = s.file * G_boardInfo.square_size + px + G_boardInfo.x_offset;
    int y = s.rank * G_boardInfo.square_size + py + G_boardInfo.y_offset;
    Platform::renderFont(b, x, y, FontType::NORMAL, c);
}

void renderBoard(Board *board, VisualSetting &vs) {

    G_boardInfo.x_offset    = 0;
    G_boardInfo.y_offset    = 0;
    G_boardInfo.square_size = 75;
    G_boardInfo.texture_dim = G_boardInfo.square_size;
    G_boardInfo.is_flipped  = vs.is_board_flipped;
    G_boardInfo.theme       = vs.theme;

    Square king_pos = getKingPosition(board, board->turn);

    // NOTE(Tejas): The rendering is done in stages to layer everything for
    //              better visual

    // rendering the background squares
    for (int rank = 0; rank < MAX_RANK; rank++) {
        for (int file = 0; file < MAX_FILE; file++) {
            if ((rank + file) % 2 == 0)
                fillSquare(Square{rank,file}, vs.theme.light);
            else
                fillSquare(Square{rank,file}, vs.theme.dark);
        }
    }

    // rendering the rank and file numbers
    for (int i = 0; i < 8; i++) {
        renderRankCoord(Square{ i, 0 });
        renderFileCoord(Square{ 7, i });
    }

    // rendering the pieces and 
    for (int rank = 0; rank < MAX_RANK; rank++) {

        for (int file = 0; file < MAX_FILE; file++) {

            Square current_square = { rank, file };
            Piece  piece          = getPieceAt(board, current_square);

            Chess::BitBoard mask = CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(rank, file));

            if (vs.selected_square != OFF_SQUARE) {

                if (vs.legal_squares & mask && vs.selected_square != OFF_SQUARE && vs.highlight_legal_moves)
                    fillSquare(current_square, vs.theme.legal);
                if (vs.selected_square == current_square && vs.highlight_selected_square)
                    fillSquare(vs.selected_square, vs.theme.high);
            }

            if (vs.highlight_latest_move) {
                if (current_square == vs.latest_move_from)
                    outlineSquare(current_square, 5.0f, 0xFFFFFF00);
                if (current_square == vs.latest_move_to)
                    outlineSquare(current_square, 5.0f, 0xFFFFFF00);
            }

            if ((vs.is_white_in_check) && piece == WHITE_KING && vs.highlight_check)
                fillSquare(king_pos, 0x99FF0000);
            if ((vs.is_black_in_check) && piece == BLACK_KING && vs.highlight_check)
                fillSquare(king_pos, 0x99FF0000);

            if (vs.selected_square != current_square)
                renderPieceOnSquare(current_square, piece);
        }
    }

    // NOTE(Tejas): the selected piece follows the cursor around
    if (vs.selected_square != OFF_SQUARE) {
        Piece piece = getPieceAt(board, vs.selected_square);
        renderPieceOnMouse(vs.mousex, vs.mousey, piece);
    }

    // BitBoard b = Chess::getAttackingSquares(board, board->turn);

    // for (int index = 0; index < 64; index++) {

    //     BitBoard mask = U64(1) << index;
    //     if (mask & b) {
    //         Square square = { index / MAX_RANK, index % MAX_FILE };
    //         fillSquare(square, 0x99FF0000);
    //     }
    // }

    if (vs.paused_control) {

        // TODO(Tejas): Maybe move this to the status bar.
        int x, y, w, h;
        Platform::getBoardDimention(&x, &y, &w, &h);

        // NOTE(Tejas): this is for a fog effect
        Platform::fillRect(x, y, w, h, 0xCCAAAAAA);

        Color text_color = 0xFF992222;
        // TODO(Tejas): remove the hard coded values
        int align_offset = 143;
        Platform::renderFont("Board Controls Paused", (w / 2) - align_offset, (h / 2),
                             FontType::NORMAL, text_color);
    }
}

void renderStatusBar(StatusBar sb) {

    int x, y, w, h;
    Platform::getStatusBarDimention(&x, &y, &w, &h);
    Platform::fillRect(x, y, w, h, 0xFF000000);

    if (sb.mode != NULL) {
        Color text_color = 0xFFFFFFFF;
        int x_offset = 0;
        Platform::renderFont(sb.mode, x + x_offset, y, FontType::SMALL, text_color);
    }

    if (sb.turn != NULL) {
        Color text_color = 0xFFFFFFFF;
        int x_offset = 300;
        Platform::renderFont(sb.turn, x + x_offset, y, FontType::SMALL, text_color);
    }

    if (sb.check) {
        Color text_color = 0xFFFF0000;
        int x_offset = 500;
        Platform::renderFont("+", x + x_offset, y, FontType::SMALL, text_color);
    }
}

void renderWinner(Player player) {

    // NOTE(Tejas): this is for a fog effect
    int x, y, w, h;
    Platform::getBoardDimention(&x, &y, &w, &h);
    Platform::fillRect(x, y, w, h, 0xCCAAAAAA);

    Color text_color = 0xFF000000;

    // TODO(Tejas): Remove the hard coded values here
    if (player == Player::WHITE)
        renderFont("White is the Winner!", 130, 300, FontType::LARGE, text_color);

    if (player == Player::BLACK)
        renderFont("Black is the Winner!", 130, 300, FontType::LARGE, text_color);
}

Square pixelToBoard(int x, int y, bool is_flipped) {

    if (x < G_boardInfo.x_offset) return OFF_SQUARE;
    if (y < G_boardInfo.y_offset) return OFF_SQUARE;

    if (x > (G_boardInfo.x_offset + (G_boardInfo.square_size * MAX_FILE))) return OFF_SQUARE;
    if (y > (G_boardInfo.y_offset + (G_boardInfo.square_size * MAX_RANK))) return OFF_SQUARE;

    Square square = { };

    square.rank = (y - G_boardInfo.y_offset) / G_boardInfo.square_size;
    square.file = (x - G_boardInfo.x_offset) / G_boardInfo.square_size;

    if (is_flipped) {
        square.rank = (MAX_RANK - 1) - square.rank;
        square.file = (MAX_RANK - 1) - square.file;
    }

    return square;
}
