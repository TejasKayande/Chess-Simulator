
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Platform/platform.h"
#include "Chess/chess.h"

struct ColorTheme {
    Color dark;
    Color light;
    Color high;
    Color legal;
};

struct VisualSetting {

    bool paused_control;

    bool highlight_legal_moves;
    bool highlight_selected_square;
    bool highlight_latest_move;
    bool highlight_check;

    bool is_board_flipped; 

    bool is_white_in_check;
    bool is_black_in_check;

    Chess::Square   selected_square; // To highlight selected square on the board
    Chess::BitBoard legal_squares;   // To highlight the legal squares on the board for the selected piece

    // the latest move made on the board
    Chess::Square latest_move_from;
    Chess::Square latest_move_to;

    ColorTheme theme;           // The theme of the board
};

struct StatusBar {

    char *mode;
    char *turn;
    bool check;
};

void renderBoard(Chess::Board *board, VisualSetting &vs);
void renderStatusBar(StatusBar sb);
void renderWinner(Chess::Player player);

Chess::Square pixelToBoard(int x, int y, bool is_flipped);

#endif // _RENDERER_H_
