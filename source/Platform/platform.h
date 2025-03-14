
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "base.h"

#define TEXTURE_DIM 100

#define LEFT_CLICK_KEY_CODE  0x01
#define RIGHT_CLICK_KEY_CODE 0x02

#define U64(x) (x##ULL)
#define GET_FIST_SET_BIT(x) (Platform::getFirstSetBit(x))

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
            // On Board Keybinds
            NONE          = 0x00,
            FLIP_BOARD    = 0x46,   // f key
            RESET_BOARD   = 0x58,   // x key
            PROMOTE_TO    = 0x31,   // any key in { 1, 2, 3, 4 }
            PREVIOUS_MOVE = 0x25,   // Left arrow key
            NEXT_MOVE     = 0x27,   // Right Arrow Key 
            LATEST_MOVE   = 0x52,   // r key

            TOGGLE_MENU   = 0x1B,   // ESC key

            // On Menu Keybinds
            NEXT_ITEM     = 0x28,   // Arrow Down key
            PREVIOUS_ITEM = 0x26,   // Arrow Up key

            ACTION        = 0x0D    // Return (Enter) Key
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

    struct Menu {
        
    };
};

int init(void);
void cleanUp(void);

void pollEvents(Event &event);
bool windowShouldClose(void);

void fillRect(int x, int y, int w, int h, Color c);
void renderTexture(int x, int y, int w, int h, TexID tex_id);
void renderFont(const char* text, int x, int y, FontType f, Color c);

void clear(void);
void present(void);

void playSound(SoundType st);

void getWindowDimention(int *w, int *h);

int getFirstSetBit(u64 b);
   
} // namespace Platform

typedef Platform::Event::Mouse::Type       Mouse;
typedef Platform::FontType                 FontType;
typedef Platform::SoundType                SoundType;
typedef Platform::Event::Keyboard::Type    Key;
typedef Platform::Event::Keyboard::Promote Promote;

#endif // _PLATFORM_H_
