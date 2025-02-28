
#include "Platform/platform.h"
#include "Chess/chess.h"

#include "renderer.h"

global Chess::Board   *G_board;
global Platform::Event G_event;
global VisualSetting   G_vs;

internal void handleMouse(void) {

    switch (G_event.mouse.type) {

    case Mouse::RCLICK: {

        G_vs.selected_square = Chess::OFF_SQUARE;
        G_vs.legal_squares = U64(0);

    } break;

    case Mouse::LCLICK: {

        // TODO(Tejas): add it so that if the user wants, they can click on one of the legal squares
        //              to make a move and not have to drag the piece to the square.

        if (G_vs.selected_square != Chess::OFF_SQUARE) break;

        Chess::Square clicked_square = pixelToBoard(G_event.mouse.x, G_event.mouse.y, G_vs.is_board_flipped);
        Chess::Piece  piece = Chess::getPieceAt(G_board, clicked_square);
        if (piece.color == G_board->turn) {
            G_vs.selected_square = clicked_square;   
            Chess::BitBoard legal_moves = Chess::getLegalMoves(G_board, G_vs.selected_square, piece);
            G_vs.legal_squares = legal_moves;
        }

    } break;

    case Mouse::LRELEASE: {
        
        if (G_vs.selected_square == Chess::OFF_SQUARE) break;

        Chess::Square clicked_square = pixelToBoard(G_event.mouse.x, G_event.mouse.y, G_vs.is_board_flipped);

        Chess::Move move = { };
        move.player = G_board->turn;
        move.piece  = Chess::getPieceAt(G_board, G_vs.selected_square);
        move.from   = G_vs.selected_square;
        move.to     = clicked_square;

        Chess::BitBoard clicked_square_mask = U64(1) << GET_INDEX(move.to.rank, move.to.file);

        if (G_vs.legal_squares & clicked_square_mask) {

            Chess::Piece p = Chess::getPieceAt(G_board, move.to);

            Chess::move(G_board, move);
            Chess::changeTurn(G_board);

            if (p == Chess::EMPTY_SQUARE)
                Platform::playSound(SoundType::MOVE);
            else 
                Platform::playSound(SoundType::CAPTURE);
        }

        G_vs.selected_square = Chess::OFF_SQUARE;
        G_vs.legal_squares = U64(0);
        
    } break;

    case Mouse::RRELEASE: {
        
    } break;

    }
}

internal void handleKeyboard(void) {

    switch (G_event.kbd.type) {

    case  Key::FLIP_BOARD: {
        G_vs.is_board_flipped = (G_vs.is_board_flipped) ? false : true;
    } break;

    case Key::RESET_BOARD: {
        Chess::resetBoard(G_board);
    } break;

    default: break;

    }
}

internal void update() {

    G_vs.mousex = G_event.mouse.x;
    G_vs.mousey = G_event.mouse.y;

    handleKeyboard();
    handleMouse();
}

int main(void) {

    if (!SUCCESS(Platform::init())) {
        printf("There was an error initializing the platform layer\n");
        return -1;
    }

    G_board        = Chess::initBoard();
    G_board->turn  = Chess::Player::WHITE;

    G_vs.is_board_flipped = false;
    G_vs.selected_square  = Chess::OFF_SQUARE;
    G_vs.theme.dark  = 0xFF316548;
    G_vs.theme.light = 0xFFE9E9E6;
    G_vs.theme.high  = 0xFF9999FF;
    G_vs.theme.legal = 0xFFCCFF99;

    Chess::setFen(G_board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk");

    while (!Platform::windowShouldClose()) {

        Platform::pollEvents(G_event);
        update();

        Platform::clear();
        renderBoard(G_board, G_vs); 
        Platform::present();
    }

    Chess::cleanUpBoard(&G_board);
    Platform::cleanUp();

    return 0;
}
