
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "base.h"

#ifdef _ON_WINDOWS_
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define TEXTURE_DIM 100

#define LEFT_CLICK_KEY_CODE  0x01
#define RIGHT_CLICK_KEY_CODE 0x02

#define U64(x) (x##ULL)
#define GET_FIRST_SET_BIT(x) (Platform::getFirstSetBit(x))

namespace Platform {

enum TexID {
    WPAWN, WKNIGHT, WBISHOP,
    WROOK, WQUEEN , WKING  ,
    BPAWN, BKNIGHT, BBISHOP,
    BROOK, BQUEEN , BKING
};

enum FontType {
    VERY_SMALL,
    SMALL,
    NORMAL,
    LARGE,
    VERY_LARGE
};

enum SoundType {
    MOVE,
    CAPTURE,
    CASTLE
};

struct Event {

    struct Keyboard {

        enum Type {
            NONE          = 0x00,
            FLIP_BOARD    = 0x46,   // f key
            RESET_BOARD   = 0x58,   // x key
            PROMOTE_TO    = 0x31,   // any key in { 1, 2, 3, 4 }
            PREVIOUS_MOVE = 0x25,   // Left arrow key
            NEXT_MOVE     = 0x27,   // Right Arrow Key 
            LATEST_MOVE   = 0x26,   // UP Arrow key
        } type;

        enum Promote {
            QUEEN = 0,
            ROOK,
            BISHOP,
            KNIGHT
        } promote_to;
        
    } kbd;

    struct Mouse  {

        enum Type {
            NONE = 0,
            LCLICK,
            RCLICK,
            LRELEASE,
            RRELEASE
        } type;

        int x, y;
        
    } mouse;

    enum MenuRequest {
        NONE = 0,

        // file menu
        NEW_GAME,
        LOAD_FEN,
        GET_FEN,
        QUIT,

        // view menu
        FLIP_BOARD,
        THEME_ONE,
        THEME_TWO,
        THEME_THREE,
        THEME_FOUR,
        THEME_FIVE,
        TOGGLE_LEGAL_HIGHLIGHT,
        TOGGLE_LATEST_HIGHLIGHT,
        TOGGLE_SELECTED_HIGHLIGHT,
        TOGGLE_CHECK_HIGHLIGHT,

        // help menu
        ABOUT,
        KEYBINDS

    } menu_request;
};

int init(void);
void cleanUp(void);

void pollEvents(Event &event);
bool windowShouldClose(void);

void fillRect(int x, int y, int w, int h, Color c);
void drawRect(int x, int y, int w, int h, f32 lt, Color c);
void renderTexture(int x, int y, int w, int h, TexID tex_id);
void renderFont(const char* text, int x, int y, FontType f, Color c);

void clear(void);
void present(void);

void playSound(SoundType st);

void getWindowDimention(int *w, int *h);

int getFirstSetBit(u64 b);

void error(char* message);
void information(char* message);

void setClipboard(char* str);
   
} // namespace Platform

typedef Platform::Event::Mouse::Type       Mouse;
typedef Platform::FontType                 FontType;
typedef Platform::SoundType                SoundType;
typedef Platform::Event::Keyboard::Type    Key;
typedef Platform::Event::Keyboard::Promote Promote;
typedef Platform::Event::MenuRequest       MenuRequest;

#endif // _PLATFORM_H_
