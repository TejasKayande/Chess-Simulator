
#include "Platform/platform.h"
#include "Chess/chess.h"

#include "renderer.h"

struct GameState {

    bool pause_control; // controls if the player can interact with the board

    Chess::Board *board;

    Chess::Move   latest_move;
    Chess::Player winner;

    Chess::PType promote_to;

    enum GameMode {
        NORMAL = 0,
        THREE_CHECKS,
        KING_OF_THE_HILL,
        FOG_OF_WAR
    } game_mode;

    // this is for the three checks varient
    int white_check_counter;
    int black_check_counter; 
};
typedef GameState::GameMode GameMode;

global GameState       G_gameState;
global Platform::Event G_event;
global VisualSetting   G_vs;
global StatusBar       G_sb;

global const ColorTheme NATURE_GREEN    = { 0xFF316548, 0xFFE9E9E6, 0xFF9999FF, 0xAA44FF44 };
global const ColorTheme OCEAN_BLUE      = { 0xFF6666FF, 0xFFCCCCFF, 0xFFCCEEFF, 0xAA2299AA };
global const ColorTheme SUNSET_ORANGE   = { 0xFF8B4513, 0xFFFFE4B5, 0xFFFF6347, 0xAAFFAA22 };
global const ColorTheme MIDNIGHT_PURPLE = { 0xFF2E1A47, 0xFFD8C3A5, 0xFFB589D6, 0xAA8844FF };
global const ColorTheme CYBER_NEON      = { 0xFF0A0A5F, 0xFFFFFFFF, 0xFFFF00FF, 0xAA00FF00 };

internal void resetGame(void) {
    
    Chess::resetBoard(G_gameState.board);
    G_gameState.board->turn = Chess::Player::WHITE;
    G_gameState.winner = Chess::Player::NO_COLOR;

    G_gameState.latest_move = { };
    G_gameState.latest_move.from = Chess::OFF_SQUARE;
    G_gameState.latest_move.to   = Chess::OFF_SQUARE;

    G_vs.latest_move_from  = Chess::OFF_SQUARE;
    G_vs.latest_move_to    = Chess::OFF_SQUARE;
    G_vs.is_white_in_check = false;
    G_vs.is_black_in_check = false;

    G_gameState.pause_control = false;

    G_gameState.white_check_counter = 0;
    G_gameState.black_check_counter = 0; 
}

internal void makeMove(Chess::Square from, Chess::Square to) {

    Chess::Move move = { };
    move.from = from;
    move.to   = to;

    if (move.from == Chess::OFF_SQUARE || move.to == Chess::OFF_SQUARE) return;

    Chess::move(G_gameState.board, &move);
    Chess::changeTurn(G_gameState.board);

    if (move.type == MoveType::SIMPLE)  Platform::playSound(SoundType::MOVE);
    if (move.type == MoveType::CAPTURE) Platform::playSound(SoundType::CAPTURE);
    if (move.type == MoveType::CASTLE)  Platform::playSound(SoundType::CASTLE);

    Chess::clearMove(&G_gameState.latest_move);
    G_gameState.latest_move = move;

    if (Chess::isInCheck(G_gameState.board, Chess::Player::WHITE) && G_gameState.board->turn == Chess::Player::WHITE) {
        G_vs.is_white_in_check = true;
        G_gameState.white_check_counter++;
    } else {
        G_vs.is_white_in_check = false;
    }

    if (Chess::isInCheck(G_gameState.board, Chess::Player::BLACK) && G_gameState.board->turn == Chess::Player::BLACK) {
        G_gameState.black_check_counter++;
        G_vs.is_black_in_check = true;
    } else {
        G_vs.is_black_in_check = false;
    }
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
        }

        was_a_mouse_event = true;

    } break;

    case Mouse::LRELEASE: {

        if (G_vs.selected_square == Chess::OFF_SQUARE) break;

        Chess::Square clicked_square = pixelToBoard(G_event.mouse.x, G_event.mouse.y, G_vs.is_board_flipped);
        Chess::BitBoard clicked_square_mask
            = CREATE_BITBOARD_MASK(GET_INDEX_FROM_SQUARE(clicked_square.rank, clicked_square.file));

        if (G_vs.legal_squares & clicked_square_mask) {
            makeMove(G_vs.selected_square, clicked_square);
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

    case Key::PROMOTE_TO: {

        switch (G_event.kbd.promote_to) {
        case Promote::QUEEN:  G_gameState.promote_to = Chess::PType::QUEEN;  break;
        case Promote::ROOK:   G_gameState.promote_to = Chess::PType::ROOK;   break;
        case Promote::BISHOP: G_gameState.promote_to = Chess::PType::BISHOP; break;
        case Promote::KNIGHT: G_gameState.promote_to = Chess::PType::KNIGHT; break;
        }

        was_a_keyboard_event = true;

    } break;

    case Key::PREVIOUS_MOVE: {

        was_a_keyboard_event = true;
    } break;

    case Key::NEXT_MOVE: {

        was_a_keyboard_event = true;
    } break;

    case Key::LATEST_MOVE: {

        was_a_keyboard_event = true;
    } break;

    case Key::TOGGLE_PAUSE: {
        G_gameState.pause_control = (G_gameState.pause_control) ? false : true;
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
        // TODO(Tejas): this function needs an input box, implement that
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

    case MenuRequest::PLAY_NORMAL: {
        G_gameState.game_mode = GameMode::NORMAL;
        G_sb.mode = "Playing Normal Mode";
        resetGame();
    } break;

    case MenuRequest::PLAY_THREE_CHECKS : {
        G_gameState.game_mode = GameMode::THREE_CHECKS;
        G_sb.mode = "Playing Three Checks";
        resetGame();
    } break;

    case MenuRequest::PLAY_KING_OF_THE_HILL: {
        G_gameState.game_mode = GameMode::KING_OF_THE_HILL;
        G_sb.mode = "Playing King of the Hill Mode";
        resetGame();
    } break;

    case MenuRequest::PLAY_FOG_OF_WAR : {
        G_gameState.game_mode = GameMode::FOG_OF_WAR;
        G_sb.mode = "Playing For of War Mode";
        resetGame();
    } break;

    case MenuRequest::ABOUT: {
        char* about_text = "A Chess Simulator built using the C++ Programming Language\n";
        Platform::information(about_text);
    } break;

    case MenuRequest::KEYBINDS: {
        char* keybind_info =
            "KeyBinds\n"
            "Flip Board (F) \n"
            "Toggle Pause (ESC) \n"
            "Reset Board (X)\n"
            "Hold \'1\' before promoting, to promote to Queen\n"
            "Hold \'2\' before promoting, to promote to Rook\n"
            "Hold \'3\' before promoting, to promote to Bishop\n"
            "Hold \'4\' before promoting, to promote to Knight\n ";
        Platform::information(keybind_info);
    } break;

    default: {} break;
        
    }
}

internal void update() {

    G_vs.mousex = G_event.mouse.x;
    G_vs.mousey = G_event.mouse.y;

    handleMenuRequest();

    // NOTE(Tejas): by default the pawn promotes to a queen
    //              this is just resetting that.
    G_gameState.promote_to = Chess::PType::QUEEN;

    bool was_an_event = false;

    // NOTE(Tejas): keyboard shortcuts are allowed if the controls are paused
    //              as they dont affect the game state (except for resetting the game)
    was_an_event |= handleKeyboard();

    // NOTE(Tejas): resetting the selection if the player paused the game in the middle
    //              of making a move.
    if (!G_gameState.pause_control) was_an_event |= handleMouse();   
    else G_vs.selected_square = Chess::OFF_SQUARE;

    if (was_an_event) {

        // NOTE(Tejas): if any pawns are on the final ranks, promote them
        Chess::promotePawn(G_gameState.board, G_gameState.promote_to);

        if (Chess::isCheckMate(G_gameState.board, G_gameState.board->turn)) {
            G_gameState.winner = (G_gameState.board->turn == Chess::Player::WHITE)
                                 ? Chess::Player::BLACK : Chess::Player::WHITE;
        }

        if (G_gameState.game_mode == GameMode::THREE_CHECKS) {
            if (G_gameState.white_check_counter >= 3) G_gameState.winner = Chess::Player::BLACK;
            if (G_gameState.black_check_counter >= 3) G_gameState.winner = Chess::Player::WHITE;
        }

        if (G_gameState.game_mode == GameMode::KING_OF_THE_HILL) {
            if (Chess::isKingInCenter(G_gameState.board, Chess::Player::WHITE))
                G_gameState.winner = Chess::Player::WHITE;
            if (Chess::isKingInCenter(G_gameState.board, Chess::Player::BLACK))
                G_gameState.winner = Chess::Player::BLACK;
        }
    }

    if (G_gameState.winner != Chess::Player::NO_COLOR) G_gameState.pause_control = true;

    G_vs.latest_move_from = G_gameState.latest_move.from;
    G_vs.latest_move_to   = G_gameState.latest_move.to;
    G_vs.paused_control   = G_gameState.pause_control;

    G_sb.check = false;

    if (G_gameState.board->turn == Chess::Player::WHITE) {
        G_sb.turn = "Turn: W";   
        if (G_vs.is_white_in_check) G_sb.check = true;
    }
    if (G_gameState.board->turn == Chess::Player::BLACK) {
        G_sb.turn = "Turn: B"; 
        if (G_vs.is_black_in_check) G_sb.check = true;
    }
}

#if REMOVE_CONSOLE
// NOTE(Tejas): program only supports Windows.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int main(void) {
#endif

    if (!SUCCESS(Platform::init())) {
        Platform::error("There was an error initializing the platform layer");
        return -1;
    }

    G_gameState.board = Chess::initBoard();

    G_gameState.board->turn = Chess::Player::WHITE;
    G_gameState.winner      = Chess::Player::NO_COLOR;

    G_gameState.latest_move.from = Chess::OFF_SQUARE;
    G_gameState.latest_move.to   = Chess::OFF_SQUARE;

    G_gameState.game_mode = GameMode::NORMAL;
    G_gameState.white_check_counter = 0;
    G_gameState.black_check_counter = 0;

    G_vs.is_board_flipped = true;
    G_vs.selected_square  = Chess::OFF_SQUARE;
    G_vs.theme = OCEAN_BLUE;

    G_vs.highlight_legal_moves     = true;
    G_vs.highlight_selected_square = true;
    G_vs.highlight_latest_move     = true;
    G_vs.highlight_check           = true;

    G_vs.latest_move_from = Chess::OFF_SQUARE;
    G_vs.latest_move_to   = Chess::OFF_SQUARE;

    G_sb.mode = "Playing Normal Mode";
    G_sb.check = false;


    char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w QKqk";
    Chess::setFen(G_gameState.board, STARTING_FEN);

    while (!Platform::windowShouldClose()) {

        Platform::pollEvents(G_event);
        update();

        Platform::clear();

        renderBoard(G_gameState.board, G_vs); 

        if (G_gameState.winner == Chess::Player::WHITE) renderWinner(Chess::Player::WHITE);
        if (G_gameState.winner == Chess::Player::BLACK) renderWinner(Chess::Player::BLACK);

        renderStatusBar(G_sb);

        Platform::present();
    }

    Chess::cleanUpBoard(G_gameState.board);
    Platform::cleanUp();

    return 0;
}
