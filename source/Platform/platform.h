
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
            TOGGLE_PAUSE  = 0x1B,   // The Esc Key
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

        ID_MENU = 101,

        // file menu
        NEW_GAME = 5010,
        LOAD_FEN = 5020,
        GET_FEN  = 5030,
        QUIT     = 5040,

        // view menu
        FLIP_BOARD                = 6010,
        THEME_ONE                 = 6021,
        THEME_TWO                 = 6022,
        THEME_THREE               = 6023,
        THEME_FOUR                = 6024,
        THEME_FIVE                = 6025,
        TOGGLE_LEGAL_HIGHLIGHT    = 6030,
        TOGGLE_LATEST_HIGHLIGHT   = 6040,
        TOGGLE_SELECTED_HIGHLIGHT = 6050,
        TOGGLE_CHECK_HIGHLIGHT    = 6060,

        // variants menu
        PLAY_NORMAL           = 7010,
        PLAY_THREE_CHECKS     = 7020,
        PLAY_KING_OF_THE_HILL = 7030,
        PLAY_FOG_OF_WAR       = 7040,

        // help menu
        ABOUT    = 8010,
        KEYBINDS = 8020

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
