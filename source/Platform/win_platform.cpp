
#include "base.h"
#include "platform.h"

#include <mmsystem.h>

#include <wincodec.h>
#include <d2d1.h>
#include <dwrite.h>

using namespace Platform;

struct KeyState {

    // keyboard
    bool flip_board_down;
    bool reset_board_down;
    bool prev_move_down;
    bool next_move_down;
    bool latest_move_down;
    bool toggle_pause_down;

    // mouse
    bool lmouse_down;
    bool rmouse_down;
};

struct Textures {
    ID2D1Bitmap *wPawn, *wKnight, *wBishop, *wRook, *wQueen, *wKing;
    ID2D1Bitmap *bPawn, *bKnight, *bBishop, *bRook, *bQueen, *bKing;
};

struct Window {
    char* name;
    u32   width;
    u32   height;
    HWND  handle;
    BOOL  running;
};

struct WindowDistribution {
    u32 board_x;
    u32 board_y;
    u32 board_w;
    u32 board_h;

    u32 status_x;
    u32 status_y;
    u32 status_w;
    u32 status_h;
};

struct Direct2D {
    ID2D1Factory          *factory;
    ID2D1HwndRenderTarget *target;
    ID2D1SolidColorBrush  *brush;
    IDWriteFactory        *write;

    IDWriteTextFormat     *sm_font;
    IDWriteTextFormat     *nr_font;
    IDWriteTextFormat     *lg_font;
};

global Window             G_window;
global WindowDistribution G_wnd_distribution;
global Direct2D           G_direct2D;

global KeyState G_keyState;
global Textures G_textures;

global MenuRequest G_menuRequest;

internal bool keyReleased(int key) {

    const int pressed = 0x8000;

    bool result = false;

    if (!(GetAsyncKeyState(key) & pressed)) {

        switch (key) {

        case Key::FLIP_BOARD: {
            result = G_keyState.flip_board_down;
            G_keyState.flip_board_down = false;
            return result;
        } break;

        case Key::RESET_BOARD: {
            result = G_keyState.reset_board_down;
            G_keyState.reset_board_down = false;
            return result;
        } break;

        case Key::PREVIOUS_MOVE: {
            result = G_keyState.prev_move_down;
            G_keyState.prev_move_down = false;
            return result;
        } break;

        case Key::NEXT_MOVE: {
            result = G_keyState.next_move_down;
            G_keyState.next_move_down = false;
            return result;
        } break;

        case Key::LATEST_MOVE: {
            result = G_keyState.latest_move_down;
            G_keyState.latest_move_down = false;
            return result;
        } break;

        case Key::TOGGLE_PAUSE: {
            result = G_keyState.toggle_pause_down;
            G_keyState.toggle_pause_down = false;
            return result;
        } break;

        case Mouse::LCLICK: {
            result = G_keyState.lmouse_down;
            G_keyState.lmouse_down = false;
            return result;
        } break;

        case Mouse::RCLICK: {
            result = G_keyState.rmouse_down;
            G_keyState.rmouse_down = false;
            return result;
        } break;

        }
    }

    return result;
}

internal LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    LRESULT result = 0;

    switch (msg) {

    case WM_COMMAND: {

        switch(LOWORD(wParam)) {

        case MenuRequest::NEW_GAME: G_menuRequest = MenuRequest::NEW_GAME; break;
        case MenuRequest::LOAD_FEN: G_menuRequest = MenuRequest::LOAD_FEN; break;
        case MenuRequest::GET_FEN:  G_menuRequest = MenuRequest::GET_FEN;  break;
        case MenuRequest::QUIT:     G_window.running = false;              break;

        case MenuRequest::FLIP_BOARD:  G_menuRequest = MenuRequest::FLIP_BOARD;  break;
        case MenuRequest::THEME_ONE:   G_menuRequest = MenuRequest::THEME_ONE;   break;
        case MenuRequest::THEME_TWO:   G_menuRequest = MenuRequest::THEME_TWO;   break;
        case MenuRequest::THEME_THREE: G_menuRequest = MenuRequest::THEME_THREE; break;
        case MenuRequest::THEME_FOUR:  G_menuRequest = MenuRequest::THEME_FOUR;  break;
        case MenuRequest::THEME_FIVE:  G_menuRequest = MenuRequest::THEME_FIVE;  break;

        case MenuRequest::TOGGLE_LEGAL_HIGHLIGHT:    G_menuRequest = MenuRequest::TOGGLE_LEGAL_HIGHLIGHT;    break;
        case MenuRequest::TOGGLE_SELECTED_HIGHLIGHT: G_menuRequest = MenuRequest::TOGGLE_SELECTED_HIGHLIGHT; break;
        case MenuRequest::TOGGLE_LATEST_HIGHLIGHT:   G_menuRequest = MenuRequest::TOGGLE_LATEST_HIGHLIGHT;   break;
        case MenuRequest::TOGGLE_CHECK_HIGHLIGHT:    G_menuRequest = MenuRequest::TOGGLE_CHECK_HIGHLIGHT;    break;

        case MenuRequest::PLAY_NORMAL:           G_menuRequest = MenuRequest::PLAY_NORMAL; break;
        case MenuRequest::PLAY_THREE_CHECKS:     G_menuRequest = MenuRequest::PLAY_THREE_CHECKS; break;
        case MenuRequest::PLAY_KING_OF_THE_HILL: G_menuRequest = MenuRequest::PLAY_KING_OF_THE_HILL; break;
        case MenuRequest::PLAY_FOG_OF_WAR:       G_menuRequest = MenuRequest::PLAY_FOG_OF_WAR; break;

        case MenuRequest::ABOUT:    G_menuRequest = MenuRequest::ABOUT; break;
        case MenuRequest::KEYBINDS: G_menuRequest = MenuRequest::KEYBINDS; break;

        }

    } break;

    case WM_DESTROY: {
        PostQuitMessage(0);
    } break;

    case WM_CLOSE: {
        G_window.running = false;
    } break;

    default: {
        result = DefWindowProc(hwnd, msg, wParam, lParam);
    } break;

    }

    return result;
}

ID2D1Bitmap* loadBitmapFromFile(ID2D1RenderTarget* render_target, const wchar_t* file_name) {

    IWICImagingFactory*    wic_factory = NULL;
    IWICBitmapDecoder*     decoder     = NULL;
    IWICBitmapFrameDecode* frame       = NULL;
    IWICFormatConverter*   converter   = NULL;
    ID2D1Bitmap*           bitmap      = NULL;

    // Create WIC factory
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IWICImagingFactory, (LPVOID*)&wic_factory);

    if (FAILED(hr)) return NULL;

    // Load image file
    hr = wic_factory->CreateDecoderFromFilename(file_name, NULL, GENERIC_READ,
                                                WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) goto defer;

    // Get the first frame
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) goto defer;

    // Convert format to 32-bit BGRA (required for Direct2D)
    hr = wic_factory->CreateFormatConverter(&converter);
    if (FAILED(hr)) goto defer;

    hr = converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA,
                               WICBitmapDitherTypeNone, NULL, 0.0,
                               WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) goto defer;

    // Create Direct2D bitmap
    hr = render_target->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);
    if (FAILED(hr)) goto defer;

defer:
    if (decoder)     decoder->Release();
    if (frame)       frame->Release();
    if (converter)   converter->Release();
    if (wic_factory) wic_factory->Release();

    return bitmap;
}

int Platform::init(void) {

    // G_window.width  = 1200;
    // G_window.height = 1000;
    G_window.width  = 600;
    G_window.height = 645;

    G_window.name   = "Chess Simulator";

    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEXA wc = { };

    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = G_window.name;
    wc.hIconSm = NULL;

    if (!RegisterClassExA(&wc)) {
        MessageBoxA(NULL, "There was an error registering the window", G_window.name, MB_ICONERROR);
        return -1;
    }

    RECT wr;
    wr.left = 0;
    wr.right = G_window.width;
    wr.top = 0;
    wr.bottom = G_window.height;
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, 0);

    G_window.width = wr.right - wr.left;
    G_window.height = wr.bottom - wr.top;

    G_wnd_distribution.board_x = 0;
    G_wnd_distribution.board_y = 0;
    G_wnd_distribution.board_w = G_window.width;
    G_wnd_distribution.board_h = 600;

    G_wnd_distribution.status_x = 0;
    G_wnd_distribution.status_y = G_wnd_distribution.board_h;
    G_wnd_distribution.status_w = G_window.width;
    G_wnd_distribution.status_h = G_wnd_distribution.status_y;

    G_window.handle = CreateWindowExA(0, wc.lpszClassName, G_window.name,
                                      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      G_window.width, G_window.height,
                                      NULL, NULL, hInstance, NULL);

    if (!G_window.handle) {
        MessageBoxA(NULL, "There was an error initializing the Window Handle", G_window.name, MB_ICONERROR);
        return -1;
    }

    SetWindowPos(G_window.handle, NULL, 0, 0, 0, 0, SWP_NOSIZE);

    ShowWindow(G_window.handle, SW_SHOWNORMAL);
    UpdateWindow(G_window.handle);

    { // adding the menu

        HMENU menu = CreateMenu();

        HMENU file_menu     = CreatePopupMenu();
        HMENU view_menu     = CreatePopupMenu();
        HMENU variants_menu = CreatePopupMenu();
        HMENU help_menu     = CreatePopupMenu();

        // file menu
        AppendMenu(file_menu, MF_STRING  , MenuRequest::NEW_GAME, "New Game \t X");
        AppendMenu(file_menu, MF_DISABLED, MenuRequest::LOAD_FEN, "Load FEN");
        AppendMenu(file_menu, MF_STRING  , MenuRequest::GET_FEN , "Get FEN");
        AppendMenu(file_menu, MF_STRING  , MenuRequest::QUIT    , "Quit");

        // view menu
        AppendMenu(view_menu, MF_STRING, MenuRequest::FLIP_BOARD, "Flip Board \t F");
        HMENU theme_menu = CreatePopupMenu();
        AppendMenu(theme_menu, MF_STRING, MenuRequest::THEME_ONE  , "Nature Green");
        AppendMenu(theme_menu, MF_STRING, MenuRequest::THEME_TWO  , "Ocean Blue");
        AppendMenu(theme_menu, MF_STRING, MenuRequest::THEME_THREE, "Sunset Orange");
        AppendMenu(theme_menu, MF_STRING, MenuRequest::THEME_FOUR , "Midnight Purple");
        AppendMenu(theme_menu, MF_STRING, MenuRequest::THEME_FIVE , "Cyber Neon");
        AppendMenu(view_menu , MF_POPUP , (UINT_PTR)theme_menu, "Theme");

        AppendMenu(view_menu, MF_SEPARATOR, 0, NULL);
        AppendMenu(view_menu, MF_STRING, MenuRequest::TOGGLE_LEGAL_HIGHLIGHT   , "toggle highlighting legal moves");
        AppendMenu(view_menu, MF_STRING, MenuRequest::TOGGLE_SELECTED_HIGHLIGHT, "toggle highlighting selected square");
        AppendMenu(view_menu, MF_STRING, MenuRequest::TOGGLE_LATEST_HIGHLIGHT  , "toggle highlighting latest move");
        AppendMenu(view_menu, MF_STRING, MenuRequest::TOGGLE_CHECK_HIGHLIGHT   , "toggle highlighting check");

        // variants menu
        AppendMenu(variants_menu, MF_STRING  , MenuRequest::PLAY_NORMAL          , "Normal Mode");
        AppendMenu(variants_menu, MF_STRING  , MenuRequest::PLAY_THREE_CHECKS    , "Three Checks");
        AppendMenu(variants_menu, MF_STRING  , MenuRequest::PLAY_KING_OF_THE_HILL, "King of the Hill");
        AppendMenu(variants_menu, MF_DISABLED, MenuRequest::PLAY_FOG_OF_WAR      , "Fog of War");

        // help menu
        AppendMenu(help_menu, MF_STRING, MenuRequest::ABOUT   , "About");
        AppendMenu(help_menu, MF_STRING, MenuRequest::KEYBINDS, "Keybinds");

        // final menu
        AppendMenu(menu, MF_POPUP, (UINT_PTR)file_menu    , "File");
        AppendMenu(menu, MF_POPUP, (UINT_PTR)view_menu    , "View");
        AppendMenu(menu, MF_POPUP, (UINT_PTR)help_menu    , "Help");
        AppendMenu(menu, MF_POPUP, (UINT_PTR)variants_menu, "Variants");

        SetMenu(G_window.handle, menu);
    }

    G_window.running = true;

    { // Direct 2D init
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &G_direct2D.factory);

        RECT rc;
        GetClientRect(G_window.handle, &rc);
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
        D2D1_HWND_RENDER_TARGET_PROPERTIES wnd_props =
            D2D1::HwndRenderTargetProperties(G_window.handle, D2D1::SizeU(rc.right, rc.bottom));

        G_direct2D.factory->CreateHwndRenderTarget(&props, &wnd_props, &G_direct2D.target);
        G_direct2D.target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &G_direct2D.brush);

        { // Font Stuff
            DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                __uuidof(IDWriteFactory),
                                (IUnknown**)&G_direct2D.write);


            G_direct2D.write->CreateTextFormat(L"Consolas", NULL,
                                              DWRITE_FONT_WEIGHT_REGULAR,
                                              DWRITE_FONT_STYLE_NORMAL,
                                              DWRITE_FONT_STRETCH_NORMAL,
                                              16.0f, L"en-us", &G_direct2D.sm_font);

            G_direct2D.write->CreateTextFormat(L"Consolas", NULL,
                                              DWRITE_FONT_WEIGHT_REGULAR,
                                              DWRITE_FONT_STYLE_NORMAL,
                                              DWRITE_FONT_STRETCH_NORMAL,
                                              22.0f, L"en-us", &G_direct2D.nr_font);

            G_direct2D.write->CreateTextFormat(L"Consolas", NULL,
                                              DWRITE_FONT_WEIGHT_REGULAR,
                                              DWRITE_FONT_STYLE_NORMAL,
                                              DWRITE_FONT_STRETCH_NORMAL,
                                              32.0f, L"en-us", &G_direct2D.lg_font);

            G_direct2D.nr_font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            G_direct2D.nr_font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        if (!G_direct2D.factory || !G_direct2D.target || !G_direct2D.brush) {
            MessageBoxA(NULL, "There was an error in intializing", G_window.name, MB_ICONERROR);
            return -1;
        }
    }

    G_textures.wPawn   = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/wPawn.png");
    G_textures.wKnight = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/wKnight.png");
    G_textures.wBishop = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/wBishop.png");
    G_textures.wRook   = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/wRook.png");
    G_textures.wQueen  = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/wQueen.png");
    G_textures.wKing   = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/wKing.png");

    G_textures.bPawn   = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/bPawn.png");
    G_textures.bKnight = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/bKnight.png");
    G_textures.bBishop = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/bBishop.png");
    G_textures.bRook   = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/bRook.png");
    G_textures.bQueen  = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/bQueen.png");
    G_textures.bKing   = loadBitmapFromFile(G_direct2D.target, L"./assets/texture/bKing.png");

    if (!G_textures.wPawn || !G_textures.wKnight || !G_textures.wBishop ||
        !G_textures.wRook || !G_textures.wQueen  || !G_textures.wKing   ||
        !G_textures.bPawn || !G_textures.bKnight || !G_textures.bBishop ||
        !G_textures.bRook || !G_textures.bQueen  || !G_textures.bKing)
    {
        MessageBoxA(NULL, "There was an error while creating the Textures", G_window.name, MB_ICONERROR);
        return -1;
    }

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Failed to initialize COM", G_window.name, MB_ICONERROR);
        return -1;
    }

    return 0;
}

void Platform::playSound(SoundType st) {

    switch (st) {

    case MOVE:
        PlaySoundA("assets/sound/movemade.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case CAPTURE:
        PlaySoundA("assets/sound/capture.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    case CASTLE:
        PlaySoundA("assets/sound/castle.wav", NULL, SND_FILENAME | SND_ASYNC);
        break;
    default:
        break;
    }
}

void Platform::cleanUp(void) {

    if (G_textures.wPawn)   G_textures.wPawn->Release();
    if (G_textures.wBishop) G_textures.wBishop->Release();
    if (G_textures.wRook)   G_textures.wRook->Release();
    if (G_textures.wQueen)  G_textures.wQueen->Release();
    if (G_textures.wKing)   G_textures.wKing->Release();

    if (G_textures.bPawn)   G_textures.bPawn->Release();
    if (G_textures.bKnight) G_textures.bKnight->Release();
    if (G_textures.bBishop) G_textures.bBishop->Release();
    if (G_textures.bRook)   G_textures.bRook->Release();
    if (G_textures.bQueen)  G_textures.bQueen->Release();
    if (G_textures.bKing)   G_textures.bKing->Release();
    
    if (G_direct2D.brush)   G_direct2D.brush->Release();
    if (G_direct2D.target)  G_direct2D.target->Release();
    if (G_direct2D.factory) G_direct2D.factory->Release();
    if (G_direct2D.nr_font) G_direct2D.nr_font->Release();
    if (G_direct2D.write)   G_direct2D.write->Release();

    if (G_window.handle) DestroyWindow(G_window.handle);

    CoUninitialize();
}

bool Platform::windowShouldClose(void) {

    return !G_window.running;
}

void Platform::pollEvents(Event &event) {

    MSG msg;

    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    // NOTE(Tejas): Networking inputs should not care if the 
    //              window was active or not
    if (GetActiveWindow() != G_window.handle) return;

    // NOTE(Tejas): clearing event every frame
    memset(&event, 0, sizeof(Event));

    // menu request
    event.menu_request = G_menuRequest;
    G_menuRequest = MenuRequest::NONE;

    // Mouse input
    POINT mouse_point;
    GetCursorPos(&mouse_point);
    ScreenToClient(G_window.handle, &mouse_point);

    event.mouse.x = mouse_point.x;
    event.mouse.y = mouse_point.y;

    const int pressed = 0x8000;

    // NOTE(Tejas): Mouse clicks are registered as soon as they are clicked but
    //              the next click is only registered after the button is released
    //              and re-pressed. (This to avoid dragging effects)
    if ((GetAsyncKeyState(LEFT_CLICK_KEY_CODE) & pressed) && G_keyState.lmouse_down == false) {
        G_keyState.lmouse_down = true;
        event.mouse.type = Mouse::LCLICK;
    }

    if ((GetAsyncKeyState(RIGHT_CLICK_KEY_CODE) & pressed) && G_keyState.rmouse_down == false) {
        G_keyState.rmouse_down = true;   
        event.mouse.type = Mouse::RCLICK;
    }

    if (keyReleased(LEFT_CLICK_KEY_CODE))  event.mouse.type = Mouse::LRELEASE;
    if (keyReleased(RIGHT_CLICK_KEY_CODE)) event.mouse.type = Mouse::RRELEASE;

    // Keyboard input

    // NOTE(Tejas): updating pressed keys for this frame
    if (GetAsyncKeyState(Key::FLIP_BOARD)    & pressed) G_keyState.flip_board_down   = true;
    if (GetAsyncKeyState(Key::RESET_BOARD)   & pressed) G_keyState.reset_board_down  = true;
    if (GetAsyncKeyState(Key::PREVIOUS_MOVE) & pressed) G_keyState.prev_move_down    = true;
    if (GetAsyncKeyState(Key::NEXT_MOVE)     & pressed) G_keyState.next_move_down    = true;
    if (GetAsyncKeyState(Key::LATEST_MOVE)   & pressed) G_keyState.latest_move_down  = true;
    if (GetAsyncKeyState(Key::TOGGLE_PAUSE)  & pressed) G_keyState.toggle_pause_down = true;

    if (GetAsyncKeyState(0x31) & pressed) {          // 1
        event.kbd.type       = Key::PROMOTE_TO;
        event.kbd.promote_to = Promote::QUEEN;
    }

    if (GetAsyncKeyState(0x32) & pressed) {          // 2
        event.kbd.type       = Key::PROMOTE_TO;
        event.kbd.promote_to = Promote::ROOK;
    }

    if (GetAsyncKeyState(0x33) & pressed) {          // 3
        event.kbd.type       = Key::PROMOTE_TO;
        event.kbd.promote_to = Promote::BISHOP;
    }

    if (GetAsyncKeyState(0x34) & pressed) {          // 4
        event.kbd.type       = Key::PROMOTE_TO;
        event.kbd.promote_to = Promote::KNIGHT;
    }

    if (keyReleased(Key::FLIP_BOARD))    event.kbd.type = Key::FLIP_BOARD;
    if (keyReleased(Key::RESET_BOARD))   event.kbd.type = Key::RESET_BOARD;
    if (keyReleased(Key::PREVIOUS_MOVE)) event.kbd.type = Key::PREVIOUS_MOVE;
    if (keyReleased(Key::NEXT_MOVE))     event.kbd.type = Key::NEXT_MOVE;
    if (keyReleased(Key::LATEST_MOVE))   event.kbd.type = Key::LATEST_MOVE;
    if (keyReleased(Key::TOGGLE_PAUSE))  event.kbd.type = Key::TOGGLE_PAUSE;
}

void Platform::clear(void) {
    
    G_direct2D.target->BeginDraw();
    G_direct2D.target->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)); 
}

void Platform::present(void) {
    
    HRESULT hr = G_direct2D.target->EndDraw();
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Failed EndDraw!", G_window.name, MB_ICONERROR);
    }
}

void Platform::drawRect(int x, int y, int w, int h, f32 lt, Color c) {

    f32 a = ((c >> 24) & 0xFF) / 255.0f;
    f32 r = ((c >> 16) & 0xFF) / 255.0f;
    f32 g = ((c >>  8) & 0xFF) / 255.0f;
    f32 b = ((c >>  0) & 0xFF) / 255.0f;

    G_direct2D.brush->SetColor(D2D1::ColorF(r, g, b, a));
    G_direct2D.target->DrawRectangle(D2D1::RectF((f32)x, (f32)y, (f32)(x + w), (f32)(y + h)), G_direct2D.brush, lt);
}

void Platform::drawLine(int x1, int y1, int x2, int y2, f32 lt, Color c) {

    f32 a = ((c >> 24) & 0xFF) / 255.0f;
    f32 r = ((c >> 16) & 0xFF) / 255.0f;
    f32 g = ((c >>  8) & 0xFF) / 255.0f;
    f32 b = ((c >>  0) & 0xFF) / 255.0f;
    
    D2D1_POINT_2F p1 = { (f32)x1, (f32)y1 };
    D2D1_POINT_2F p2 = { (f32)x2, (f32)y2 };

    G_direct2D.brush->SetColor(D2D1::ColorF(r, g, b, a));
    G_direct2D.target->DrawLine(p1, p2, G_direct2D.brush, lt, 0);
}

void Platform::fillRect(int x, int y, int w, int h, Color c) {

    f32 a = ((c >> 24) & 0xFF) / 255.0f;
    f32 r = ((c >> 16) & 0xFF) / 255.0f;
    f32 g = ((c >>  8) & 0xFF) / 255.0f;
    f32 b = ((c >>  0) & 0xFF) / 255.0f;

    G_direct2D.brush->SetColor(D2D1::ColorF(r, g, b, a));
    G_direct2D.target->FillRectangle(D2D1::RectF((f32)x, (f32)y, (f32)(x + w), (f32)(y + h)), G_direct2D.brush);
}

void Platform::renderTexture(int x, int y, int w, int h, TexID tex_id) {

    ID2D1Bitmap *tex = NULL;
    
    if (tex_id == WPAWN)   tex = G_textures.wPawn;
    if (tex_id == WKNIGHT) tex = G_textures.wKnight;
    if (tex_id == WBISHOP) tex = G_textures.wBishop;
    if (tex_id == WROOK)   tex = G_textures.wRook;
    if (tex_id == WQUEEN)  tex = G_textures.wQueen;
    if (tex_id == WKING)   tex = G_textures.wKing;
    if (tex_id == BPAWN)   tex = G_textures.bPawn;
    if (tex_id == BKNIGHT) tex = G_textures.bKnight;
    if (tex_id == BBISHOP) tex = G_textures.bBishop;
    if (tex_id == BROOK)   tex = G_textures.bRook;
    if (tex_id == BQUEEN)  tex = G_textures.bQueen;
    if (tex_id == BKING)   tex = G_textures.bKing;

    if (tex == NULL) error("Texture was null for a piece ID!");

    D2D1_RECT_F rect = { (f32)x, (f32)y, (f32)(x + w), (f32)(y + h) };
    G_direct2D.target->DrawBitmap(tex, rect);
}

void Platform::renderFont(const char* text, int x, int y, FontType f, Color c) {

    IDWriteTextFormat* font = NULL;
    if (f == FontType::SMALL)      font = G_direct2D.sm_font;
    if (f == FontType::NORMAL)     font = G_direct2D.nr_font;
    if (f == FontType::LARGE)      font = G_direct2D.lg_font;

    if (font == NULL) return;

    int len = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (len == 0) return;

    wchar_t* wideText = (wchar_t*)malloc(len * sizeof(wchar_t));
    if (wideText == NULL) return;

    if (MultiByteToWideChar(CP_UTF8, 0, text, -1, wideText, len) == 0) {
        free(wideText);
        return;
    }

    f32 a = ((c >> 24) & 0xFF) / 255.0f;
    f32 r = ((c >> 16) & 0xFF) / 255.0f;
    f32 g = ((c >>  8) & 0xFF) / 255.0f;
    f32 b = ((c >>  0) & 0xFF) / 255.0f;
    G_direct2D.brush->SetColor(D2D1::ColorF(r, g, b, a));

    D2D1_RECT_F rect = D2D1::RectF((f32)x, (f32)y, (f32)(x + 600), (f32)(y + 600));
    G_direct2D.target->DrawText(wideText, (u32)wcslen(wideText), font, rect, G_direct2D.brush);

    free(wideText);
}

void Platform::getWindowDimention(int *w, int *h) {

    *w = G_window.width;
    *h = G_window.height;
}

void Platform::getBoardDimention(int *x, int *y, int *w, int *h) {

    *x = G_wnd_distribution.board_x;
    *y = G_wnd_distribution.board_y;
    *w = G_wnd_distribution.board_w;
    *h = G_wnd_distribution.board_h;
}

void Platform::getStatusBarDimention(int *x, int *y, int *w, int *h) {
    
    *x = G_wnd_distribution.status_x;
    *y = G_wnd_distribution.status_y;
    *w = G_wnd_distribution.status_w;
    *h = G_wnd_distribution.status_h;
}

int Platform::getFirstSetBit(u64 b) {

    for (int i = 0; i < 64; i++) {
        if (b & (U64(1) << i)) {
            return i;
        }
    }

    return -1;
}

void Platform::error(char* message) {

    MessageBoxA(NULL, message, G_window.name, MB_ICONERROR);
}

void Platform::information(char* message) {

    MessageBoxA(NULL, message, G_window.name, MB_ICONINFORMATION);
}

void Platform::setClipboard(char *str) {

    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();

    size_t len = strlen(str);
    HGLOBAL msg_mem = GlobalAlloc(GMEM_MOVEABLE, len); // Allocate memory

    if (msg_mem == NULL) {
        CloseClipboard();
        return;
    }

    memcpy(GlobalLock(msg_mem), str, len);
    GlobalUnlock(msg_mem);

    SetClipboardData(CF_TEXT, msg_mem);
    CloseClipboard();

    MessageBoxA(NULL, "Fen Copied to Clipboard", G_window.name, MB_ICONINFORMATION);
}
