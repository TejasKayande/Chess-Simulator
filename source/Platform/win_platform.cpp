
#include "base.h"
#include "platform.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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
    bool toggle_menu_down;
    bool prev_item_down;
    bool next_item_down;
    bool action_down;

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

struct Direct2D {
    ID2D1Factory          *factory;
    ID2D1HwndRenderTarget *target;
    ID2D1SolidColorBrush  *brush;
    IDWriteFactory        *write;
    IDWriteTextFormat     *nr_font;
};

global Window   G_window;
global Direct2D G_direct2D;

global KeyState G_keyState;
global Textures G_textures;

internal bool keyReleased(int key) {

    const int pressed = 0x8000;

    bool result = false;

    if (!(GetAsyncKeyState(key) & pressed)) {
        if (key == Key::FLIP_BOARD) {
            result = G_keyState.flip_board_down;
            G_keyState.flip_board_down = false;
            return result;
        } 

        if (key == Key::RESET_BOARD) {
            result = G_keyState.reset_board_down;
            G_keyState.reset_board_down = false;
            return result;
        } 

        if (key == Key::PREVIOUS_MOVE) {
            result = G_keyState.prev_move_down;
            G_keyState.prev_move_down = false;
            return result;
        } 

        if (key == Key::NEXT_MOVE) {
            result = G_keyState.next_move_down;
            G_keyState.next_move_down = false;
            return result;
        } 

        if (key == Key::LATEST_MOVE) {
            result = G_keyState.latest_move_down;
            G_keyState.latest_move_down = false;
            return result;
        } 

        if (key == Key::TOGGLE_MENU) {
            result = G_keyState.toggle_menu_down;
            G_keyState.toggle_menu_down = false;
            return result;
        } 

        if (key == Key::PREVIOUS_ITEM) {
            result = G_keyState.prev_item_down;
            G_keyState.prev_item_down = false;
            return result;
        } 

        if (key == Key::NEXT_ITEM) {
            result = G_keyState.next_item_down;
            G_keyState.next_item_down = false;
            return result;
        } 

        if (key == Key::ACTION) {
            result = G_keyState.action_down;
            G_keyState.action_down = false;
            return result;
        } 

        if (key == Mouse::LCLICK) {
            result = G_keyState.lmouse_down;
            G_keyState.lmouse_down = false;
            return result;
        } 

        if (key == Mouse::RCLICK) {
            result = G_keyState.rmouse_down;
            G_keyState.rmouse_down = false;
            return result;
        } 
    }

    return result;
}

internal LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    LRESULT result = 0;

    switch (msg) {

    case WM_DESTROY: {
        PostQuitMessage(0);
    } break;

    case WM_CLOSE : {
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

    G_window.width = 800;
    G_window.height = 800;
    G_window.name = "Chess Simulator";

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

    // int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    // int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // G_window.handle = CreateWindowExA(0, wc.lpszClassName, G_window.name,
    //                                  WS_POPUP, 0, 0, screenWidth, screenHeight,
    //                                  NULL, NULL, hInstance, NULL);

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
                                              24.0f, L"en-us", &G_direct2D.nr_font);

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
    if (GetAsyncKeyState(LEFT_CLICK_KEY_CODE) & pressed) {
        G_keyState.lmouse_down = true;
        event.mouse.type = Mouse::LCLICK;
    }

    if (GetAsyncKeyState(RIGHT_CLICK_KEY_CODE) & pressed) {
        G_keyState.rmouse_down = true;   
        event.mouse.type = Mouse::RCLICK;
    }

    if (keyReleased(LEFT_CLICK_KEY_CODE)) event.mouse.type = Mouse::LRELEASE;
    if (keyReleased(RIGHT_CLICK_KEY_CODE)) event.mouse.type = Mouse::RRELEASE;

    // Keyboard input

    // NOTE(Tejas): updating pressed keys for this frame
    if (GetAsyncKeyState(Key::FLIP_BOARD)    & pressed) G_keyState.flip_board_down  = true;
    if (GetAsyncKeyState(Key::RESET_BOARD)   & pressed) G_keyState.reset_board_down = true;
    if (GetAsyncKeyState(Key::PREVIOUS_MOVE) & pressed) G_keyState.prev_move_down   = true;
    if (GetAsyncKeyState(Key::NEXT_MOVE)     & pressed) G_keyState.next_move_down   = true;
    if (GetAsyncKeyState(Key::LATEST_MOVE)   & pressed) G_keyState.latest_move_down = true;
    if (GetAsyncKeyState(Key::TOGGLE_MENU)   & pressed) G_keyState.toggle_menu_down = true;
    if (GetAsyncKeyState(Key::PREVIOUS_ITEM) & pressed) G_keyState.prev_item_down   = true;
    if (GetAsyncKeyState(Key::NEXT_ITEM)     & pressed) G_keyState.next_item_down   = true;
    if (GetAsyncKeyState(Key::ACTION)        & pressed) G_keyState.action_down      = true;

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
    if (keyReleased(Key::TOGGLE_MENU))   event.kbd.type = Key::TOGGLE_MENU;
    if (keyReleased(Key::PREVIOUS_MOVE)) event.kbd.type = Key::PREVIOUS_MOVE;
    if (keyReleased(Key::NEXT_MOVE))     event.kbd.type = Key::NEXT_MOVE;
    if (keyReleased(Key::LATEST_MOVE))   event.kbd.type = Key::LATEST_MOVE;
    if (keyReleased(Key::PREVIOUS_ITEM)) event.kbd.type = Key::PREVIOUS_ITEM;
    if (keyReleased(Key::NEXT_ITEM))     event.kbd.type = Key::NEXT_ITEM;
    if (keyReleased(Key::ACTION))        event.kbd.type = Key::ACTION;
}

void Platform::clear(void) {
    
    G_direct2D.target->BeginDraw();
    G_direct2D.target->Clear(D2D1::ColorF(D2D1::ColorF::Black)); 
}

void Platform::present(void) {
    
    HRESULT hr = G_direct2D.target->EndDraw();
    if (FAILED(hr)) {
        printf("Failed EndDraw!\n");
    }
}

void Platform::fillRect(int x, int y, int w, int h, Color c) {

    f32 a = ((c >> 24) & 0xFF) / 255.0f;
    f32 r = ((c >> 16) & 0xFF) / 255.0f;
    f32 g = ((c >>  8) & 0xFF) / 255.0f;
    f32 b = ((c >>  0) & 0xFF) / 255.0f;

    G_direct2D.brush->SetColor(D2D1::ColorF(r, g, b, a));
    G_direct2D.target->FillRectangle(D2D1::RectF((f32)x, (f32)y, (f32)(x + w), (f32)(y + h)), G_direct2D.brush);
}

void Platform::renderTexture(int x, int y, TexID tex_id) {

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

    ASSERT(tex == NULL, "Texture was null for the piece ID!");

    D2D1_SIZE_F size = tex->GetSize();
    D2D1_RECT_F rect = { (f32)x, (f32)y, x + size.width, y + size.height };
    G_direct2D.target->DrawBitmap(tex, rect);
}

void Platform::renderFont(const char* text, int x, int y, FontType f, Color c) {

    IDWriteTextFormat* font = NULL;
    if (f == FontType::VERY_SMALL) font = G_direct2D.nr_font;
    // TODO(Tejas): add these fonts
    if (f == FontType::SMALL) return;
    if (f == FontType::NORMAL) return;
    if (f == FontType::LARGE) return;
    if (f == FontType::VERY_LARGE) return;

    if (font == NULL) return;

    int len = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (len == 0) return;

    wchar_t* wideText = (wchar_t*)malloc(len * sizeof(wchar_t));
    if (!wideText) return;

    MultiByteToWideChar(CP_UTF8, 0, text, -1, wideText, len);

    f32 a = ((c >> 24) & 0xFF) / 255.0f;
    f32 r = ((c >> 16) & 0xFF) / 255.0f;
    f32 g = ((c >>  8) & 0xFF) / 255.0f;
    f32 b = ((c >>  0) & 0xFF) / 255.0f;
    G_direct2D.brush->SetColor(D2D1::ColorF(r, g, b, a));

    D2D1_RECT_F rect = D2D1::RectF((f32)x, (f32)y, (f32)(x + 500), (f32)(y + 500));
    G_direct2D.target->DrawText(wideText, (u32)wcslen(wideText), font, rect, G_direct2D.brush);

    free(wideText);
}

void Platform::getWindowDimention(int *width, int *height) {

    *width = G_window.width;
    *height = G_window.height;
}

int Platform::getFirstSetBit(u64 b) {

    for (int i = 0; i < 64; i++) {
        if (b & (U64(1) << i)) {
            return i;
        }
    }

    return -1;
}
