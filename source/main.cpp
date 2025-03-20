
#include "Platform/platform.h"
#include "Chess/chess.h"
#include "Engine/engine.h"

#include "renderer.h"

struct GameState {
    Chess::Board *board;
    Chess::Move   latest_move;
    Chess::Player winner;
};

global GameState G_gameState;

// global Chess::Board   *G_gameState.board;
global Platform::Event G_event;
global VisualSetting   G_vs;

global const ColorTheme NATURE_GREEN    = { 0xFF316548, 0xFFE9E9E6, 0xFF9999FF, 0xAA44FF44 };
global const ColorTheme OCEAN_BLUE      = { 0xFF6666FF, 0xFFCCCCFF, 0xFFCCEEFF, 0xAA2299AA };
global const ColorTheme SUNSET_ORANGE   = { 0xFF8B4513, 0xFFFFE4B5, 0xFFFF6347, 0xAAFFAA22 };
global const ColorTheme MIDNIGHT_PURPLE = { 0xFF2E1A47, 0xFFD8C3A5, 0xFFB589D6, 0xAA8844FF };
global const ColorTheme CYBER_NEON      = { 0xFF0A0A5F, 0xFFFFFFFF, 0xFFFF00FF, 0xAA00FF00 };

internal void resetGame(void) {
    
    Chess::resetBoard(G_gameState.board);
    G_gameState.winner = Chess::Player::NO_COLOR;

    Chess::clearMove(&G_gameState.latest_move);

    G_vs.latest_move_from = Chess::OFF_SQUARE;
    G_vs.latest_move_to   = Chess::OFF_SQUARE;
}

internal bool handleMouse(void) {

    bool was_a_mouse_event = false;

    switch (G_event.mouse.type) {

    case Mouse::RCLICK: {

        G_vs.selected_square = Chess::OFF_SQUARE;
        G_vs.legal_squares = U64(0);

        was_a_mouse_event = true;

    } break;

    case Mouse::LCLICK: {

        // TODO(Tejas): add it so that if the user wants, they can click on one of the legal squares
        //              to make a move and not have to drag the piece to the square.

        if (G_vs.selected_square != Chess::OFF_SQUARE) break;

        Chess::Square clicked_square = pixelToBoard(G_event.mouse.x, G_event.mouse.y, G_vs.is_board_flipped);
        Chess::Piece  piece          = Chess::getPieceAt(G_gameState.board, clicked_square);

        if (piece.color == G_gameState.board->turn) {

            G_vs.selected_square = clicked_square;

            G_vs.legal_squares = Chess::getLegalSquares(G_gameState.board, G_vs.selected_square, piece);

            if (piece.type == Chess::PType::KING)
                G_vs.legal_squares |= Chess::getCastlingSquares(G_gameState.board, G_gameState.board->turn);
        }

        was_a_mouse_event = true;

    } break;

    case Mouse::LRELEASE: {

        if (G_vs.selected_square == Chess::OFF_SQUARE) break;

        // Chess::Move move = Engine::getFirstMove(*G_gameState.board);
        // printf("from: (%d, %d), to: (%d, %d)\n", move.from.rank, move.from.file, move.to.rank, move.to.file);

        // if (!(move.from == Chess::OFF_SQUARE || move.from == Chess::OFF_SQUARE)) {
        //     Chess::move(G_gameState.board, &move);
        //     Chess::changeTurn(G_gameState.board);

        //     if (move.type == MoveType::SIMPLE)  Platform::playSound(SoundType::MOVE);
        //     if (move.type == MoveType::CAPTURE) Platform::playSound(SoundType::CAPTURE);
        //     if (move.type == MoveType::CASTLE)  Platform::playSound(SoundType::CASTLE);

        //     Chess::clearMove(&G_gameState.latest_move);
        //     G_gameState.latest_move = move;
        // }

        Chess::Square clicked_square = pixelToBoard(G_event.mouse.x, G_event.mouse.y, G_vs.is_board_flipped);

        Chess::Move move = { };
        move.from   = G_vs.selected_square;
        move.to     = clicked_square;

        Chess::BitBoard clicked_square_mask = CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(move.to.rank, move.to.file));

        if (G_vs.legal_squares & clicked_square_mask) {

            if (move.from != Chess::OFF_SQUARE && move.to != Chess::OFF_SQUARE) {

                Chess::move(G_gameState.board, &move);
                Chess::changeTurn(G_gameState.board);

                if (move.type == MoveType::SIMPLE)  Platform::playSound(SoundType::MOVE);
                if (move.type == MoveType::CAPTURE) Platform::playSound(SoundType::CAPTURE);
                if (move.type == MoveType::CASTLE)  Platform::playSound(SoundType::CASTLE);

                Chess::clearMove(&G_gameState.latest_move);
                G_gameState.latest_move = move;
            }
        }

        G_vs.selected_square = Chess::OFF_SQUARE;
        G_vs.legal_squares = U64(0);

        was_a_mouse_event = true;
        
    } break;

    case Mouse::RRELEASE: {

        was_a_mouse_event = true;
        
    } break;

    }

    return was_a_mouse_event;
}

internal bool handleKeyboard(void) {

    bool was_a_keyboard_event = false;

    switch (G_event.kbd.type) {

    case  Key::FLIP_BOARD: {
        G_vs.is_board_flipped = (G_vs.is_board_flipped) ? false : true;
        was_a_keyboard_event = true;
    } break;

    case Key::RESET_BOARD: {
        resetGame();
        was_a_keyboard_event = true;
    } break;

    case Key::PREVIOUS_MOVE: {
        Chess::undoMove(G_gameState.board, G_gameState.latest_move);
        Chess::changeTurn(G_gameState.board);
        was_a_keyboard_event = true;
    } break;

    case Key::NEXT_MOVE: {
        int number_of_moves = Engine::moveGenerationTest(*G_gameState.board, 1);
        printf("%d\n", number_of_moves);
        was_a_keyboard_event = true;
    } break;

    default: break;

    }

    return was_a_keyboard_event;
}

internal void handleMenuRequest() {

    switch (G_event.menu_request) {

    case MenuRequest::NEW_GAME: {
        resetGame();
    } break;

    case MenuRequest::LOAD_FEN: {
    } break;

    case MenuRequest::GET_FEN: {
        char* fen = Chess::getFen(G_gameState.board);
        Platform::setClipboard(fen);
        free(fen);
    } break;

    case MenuRequest::FLIP_BOARD: {
        G_vs.is_board_flipped = (G_vs.is_board_flipped) ? false : true;
    } break;
        
    case MenuRequest::THEME_ONE: {
        G_vs.theme = NATURE_GREEN;
    } break;

    case MenuRequest::THEME_TWO: {
        G_vs.theme = OCEAN_BLUE;
    } break;

    case MenuRequest::THEME_THREE: {
        G_vs.theme = SUNSET_ORANGE;
    } break;

    case MenuRequest::THEME_FOUR: {
        G_vs.theme = MIDNIGHT_PURPLE;
    } break;

    case MenuRequest::THEME_FIVE: {
        G_vs.theme = CYBER_NEON;
    } break;

    case MenuRequest::TOGGLE_LEGAL_HIGHLIGHT: {
        G_vs.highlight_legal_moves = (G_vs.highlight_legal_moves) ? false : true;
    } break;

    case MenuRequest::TOGGLE_LATEST_HIGHLIGHT: {
        G_vs.highlight_latest_move = (G_vs.highlight_latest_move) ? false : true;
    } break;

    case MenuRequest::TOGGLE_SELECTED_HIGHLIGHT: {
        G_vs.highlight_selected_square = (G_vs.highlight_selected_square) ? false : true;
    } break;

    case MenuRequest::TOGGLE_CHECK_HIGHLIGHT: {
        G_vs.highlight_check = (G_vs.highlight_check) ? false : true;
    } break;

    case MenuRequest::ABOUT: {
        char* about_text = 
            "Chess Simulator\n"
            "Version 1.0\n"
            "Developed by Tejas, Sammed and Abhishek\n"
            "----------------------------------------------------------------------\n"
            "This application is a project assignment submitted to the\n"
            "Department of Computer Science, S.B.E.S College of Science\n"
            "Acedemic Year: 2024-2025";
        Platform::information(about_text);
    } break;

    case MenuRequest::KEYBINDS: {
        char* keybind_info =
            "KeyBinds\n"
            "1. Flip Board (F) \n"
            "2. Reset Board (X)\n"
            "3. Hold \'1\' before promoting, to promote to Rook\n"
            "4. Hold \'2\' before promoting, to promote to Bishop\n"
            "5. Hold \'3\' before promoting, to promote to Knight\n ";
        Platform::information(keybind_info);
    } break;

    default: {} break;
        
    }
}

internal void update() {

    G_vs.mousex = G_event.mouse.x;
    G_vs.mousey = G_event.mouse.y;

    handleMenuRequest();

    bool was_an_event = true;
    was_an_event |= handleKeyboard();
    was_an_event |= handleMouse();

    if (was_an_event) {
        if (Chess::isCheckMate(G_gameState.board, Chess::Player::WHITE)) G_gameState.winner = Chess::Player::BLACK;
        if (Chess::isCheckMate(G_gameState.board, Chess::Player::BLACK)) G_gameState.winner = Chess::Player::WHITE;
    }

    G_vs.latest_move_from = G_gameState.latest_move.from;
    G_vs.latest_move_to   = G_gameState.latest_move.to;
}

#ifndef _ON_WINDOWS_
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int main(void) {
#endif

    if (!SUCCESS(Platform::init())) {
        Platform::error("There was an error initializing the platform layer");
        return -1;
    }

    G_gameState.board       = Chess::initBoard();
    G_gameState.board->turn = Chess::Player::WHITE;
    G_gameState.winner      = Chess::Player::NO_COLOR;

    G_gameState.latest_move.from = Chess::OFF_SQUARE;
    G_gameState.latest_move.to   = Chess::OFF_SQUARE;

    G_vs.is_board_flipped = true;
    G_vs.selected_square  = Chess::OFF_SQUARE;
    G_vs.theme = OCEAN_BLUE;

    G_vs.highlight_legal_moves     = true;
    G_vs.highlight_selected_square = true;
    G_vs.highlight_latest_move     = true;
    G_vs.highlight_check           = true;

    G_vs.latest_move_from = Chess::OFF_SQUARE;
    G_vs.latest_move_to   = Chess::OFF_SQUARE;

    char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk";
    Chess::setFen(G_gameState.board, STARTING_FEN);

    while (!Platform::windowShouldClose()) {

        Platform::pollEvents(G_event);
        update();

        Platform::clear();

        renderBoard(G_gameState.board, G_vs); 
        if (G_gameState.winner == Chess::Player::WHITE) renderWinner(Chess::Player::WHITE);
        if (G_gameState.winner == Chess::Player::BLACK) renderWinner(Chess::Player::BLACK);

        Platform::present();
    }

    Chess::cleanUpBoard(&G_gameState.board);
    Platform::cleanUp();

    return 0;
}
