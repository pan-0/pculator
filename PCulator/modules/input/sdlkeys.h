#ifndef _SDLKEYS_H_
#define _SDLKEYS_H_

#include <stdint.h>
#ifdef _WIN32
#include "SDL.h"
#else
#include "SDL.h"
#endif

//Scancodes set 1 (XT style)
const uint32_t sdlconsole_translateMatrix[95][2] = {
	{ SDLK_ESCAPE, 0x01 },
	{ SDLK_1, 0x02 },
	{ SDLK_2, 0x03 },
	{ SDLK_3, 0x04 },
	{ SDLK_4, 0x05 },
	{ SDLK_5, 0x06 },
	{ SDLK_6, 0x07 },
	{ SDLK_7, 0x08 },
	{ SDLK_8, 0x09 },
	{ SDLK_9, 0x0A },
	{ SDLK_0, 0x0B },
	{ SDLK_MINUS, 0x0C },
	{ SDLK_EQUALS, 0x0D },
	{ SDLK_BACKSPACE, 0x0E },
	{ SDLK_TAB, 0x0F },
	{ SDLK_q, 0x10 },
	{ SDLK_w, 0x11 },
	{ SDLK_e, 0x12 },
	{ SDLK_r, 0x13 },
	{ SDLK_t, 0x14 },
	{ SDLK_y, 0x15 },
	{ SDLK_u, 0x16 },
	{ SDLK_i, 0x17 },
	{ SDLK_o, 0x18 },
	{ SDLK_p, 0x19 },
	{ SDLK_LEFTBRACKET, 0x1A },
	{ SDLK_RIGHTBRACKET, 0x1B },
	{ SDLK_RETURN, 0x1C },
	{ SDLK_RETURN2, 0x1C },
	{ SDLK_KP_ENTER, 0x1C },
	{ SDLK_LCTRL, 0x1D },
	{ SDLK_a, 0x1E },
	{ SDLK_s, 0x1F },
	{ SDLK_d, 0x20 },
	{ SDLK_f, 0x21 },
	{ SDLK_g, 0x22 },
	{ SDLK_h, 0x23 },
	{ SDLK_j, 0x24 },
	{ SDLK_k, 0x25 },
	{ SDLK_l, 0x26 },
	{ SDLK_SEMICOLON, 0x27 },
	{ SDLK_QUOTE, 0x28 },
	{ SDLK_BACKQUOTE, 0x29 },
	{ SDLK_LSHIFT, 0x2A },
	{ SDLK_BACKSLASH, 0x2B },
	{ SDLK_z, 0x2C },
	{ SDLK_x, 0x2D },
	{ SDLK_c, 0x2E },
	{ SDLK_v, 0x2F },
	{ SDLK_b, 0x30 },
	{ SDLK_n, 0x31 },
	{ SDLK_m, 0x32 },
	{ SDLK_COMMA, 0x33 },
	{ SDLK_PERIOD, 0x34 },
	{ SDLK_SLASH, 0x35 },
	{ SDLK_RSHIFT, 0x36 },
	{ SDLK_KP_MULTIPLY, 0x37 },
	{ SDLK_LALT, 0x38 },
	{ SDLK_SPACE, 0x39 },
	{ SDLK_CAPSLOCK, 0x3A },
	{ SDLK_F1, 0x3B },
	{ SDLK_F2, 0x3C },
	{ SDLK_F3, 0x3D },
	{ SDLK_F4, 0x3E },
	{ SDLK_F5, 0x3F },
	{ SDLK_F6, 0x40 },
	{ SDLK_F7, 0x41 },
	{ SDLK_F8, 0x42 },
	{ SDLK_F9, 0x43 },
	{ SDLK_F10, 0x44 },
	{ SDLK_NUMLOCKCLEAR, 0x45 },
	{ SDLK_SCROLLLOCK, 0x46 },
	{ SDLK_KP_7, 0x47 },
	{ SDLK_HOME, 0x47 },
	{ SDLK_KP_8, 0x48 },
	{ SDLK_UP, 0x48 },
	{ SDLK_KP_9, 0x49 },
	{ SDLK_PAGEUP, 0x49 },
	{ SDLK_KP_MINUS, 0x4A },
	{ SDLK_KP_4, 0x4B },
	{ SDLK_LEFT, 0x4B },
	{ SDLK_KP_5, 0x4C },
	{ SDLK_KP_6, 0x4D },
	{ SDLK_RIGHT, 0x4D },
	{ SDLK_KP_PLUS, 0x4E },
	{ SDLK_KP_1, 0x4F },
	{ SDLK_END, 0x4F },
	{ SDLK_KP_2, 0x50 },
	{ SDLK_DOWN, 0x50 },
	{ SDLK_KP_3, 0x51 },
	{ SDLK_PAGEDOWN, 0x51 },
	{ SDLK_KP_0, 0x52 },
	{ SDLK_INSERT, 0x52 },
	{ SDLK_KP_PERIOD, 0x53 },
	{ SDLK_DELETE, 0x53 }
};

//Scancodes set 2 (AT style)
const uint8_t sdlconsole_scancodesMakeSet2[104][8] = {
	{ 0x1C, 0, 0, 0, 0, 0, 0, 0 }, //A
	{ 0x32, 0, 0, 0, 0, 0, 0, 0 }, //B
	{ 0x21, 0, 0, 0, 0, 0, 0, 0 }, //C
	{ 0x23, 0, 0, 0, 0, 0, 0, 0 }, //D
	{ 0x24, 0, 0, 0, 0, 0, 0, 0 }, //E
	{ 0x2B, 0, 0, 0, 0, 0, 0, 0 }, //F
	{ 0x34, 0, 0, 0, 0, 0, 0, 0 }, //G
	{ 0x33, 0, 0, 0, 0, 0, 0, 0 }, //H
	{ 0x43, 0, 0, 0, 0, 0, 0, 0 }, //I
	{ 0x3B, 0, 0, 0, 0, 0, 0, 0 }, //J
	{ 0x42, 0, 0, 0, 0, 0, 0, 0 }, //K
	{ 0x2B, 0, 0, 0, 0, 0, 0, 0 }, //L
	{ 0x3A, 0, 0, 0, 0, 0, 0, 0 }, //M
	{ 0x31, 0, 0, 0, 0, 0, 0, 0 }, //N
	{ 0x44, 0, 0, 0, 0, 0, 0, 0 }, //O
	{ 0x4D, 0, 0, 0, 0, 0, 0, 0 }, //P
	{ 0x15, 0, 0, 0, 0, 0, 0, 0 }, //Q
	{ 0x2D, 0, 0, 0, 0, 0, 0, 0 }, //R
	{ 0x1B, 0, 0, 0, 0, 0, 0, 0 }, //S
	{ 0x2C, 0, 0, 0, 0, 0, 0, 0 }, //T
	{ 0x3C, 0, 0, 0, 0, 0, 0, 0 }, //U
	{ 0x2A, 0, 0, 0, 0, 0, 0, 0 }, //V
	{ 0x1D, 0, 0, 0, 0, 0, 0, 0 }, //W
	{ 0x22, 0, 0, 0, 0, 0, 0, 0 }, //X
	{ 0x35, 0, 0, 0, 0, 0, 0, 0 }, //Y
	{ 0x1A, 0, 0, 0, 0, 0, 0, 0 }, //Z
	{ 0x45, 0, 0, 0, 0, 0, 0, 0 }, //0
	{ 0x16, 0, 0, 0, 0, 0, 0, 0 }, //1
	{ 0x1E, 0, 0, 0, 0, 0, 0, 0 }, //2
	{ 0x26, 0, 0, 0, 0, 0, 0, 0 }, //3
	{ 0x25, 0, 0, 0, 0, 0, 0, 0 }, //4
	{ 0x2E, 0, 0, 0, 0, 0, 0, 0 }, //5
	{ 0x36, 0, 0, 0, 0, 0, 0, 0 }, //6
	{ 0x3D, 0, 0, 0, 0, 0, 0, 0 }, //7
	{ 0x3E, 0, 0, 0, 0, 0, 0, 0 }, //8
	{ 0x46, 0, 0, 0, 0, 0, 0, 0 }, //9
	{ 0x0E, 0, 0, 0, 0, 0, 0, 0 }, //`
	{ 0x4E, 0, 0, 0, 0, 0, 0, 0 }, //-
	{ 0x55, 0, 0, 0, 0, 0, 0, 0 }, //=
	{ 0x5D, 0, 0, 0, 0, 0, 0, 0 }, //BACKSLASH
	{ 0x66, 0, 0, 0, 0, 0, 0, 0 }, //BACKSPACE
	{ 0x29, 0, 0, 0, 0, 0, 0, 0 }, //SPACE
	{ 0x0D, 0, 0, 0, 0, 0, 0, 0 }, //TAB
	{ 0x58, 0, 0, 0, 0, 0, 0, 0 }, //CAPS
	{ 0x12, 0, 0, 0, 0, 0, 0, 0 }, //L SHIFT
	{ 0x14, 0, 0, 0, 0, 0, 0, 0 }, //L CTRL
	{ 0xE0, 0x1F, 0, 0, 0, 0, 0, 0 }, //L GUI
	{ 0x11, 0, 0, 0, 0, 0, 0, 0 }, //L ALT
	{ 0x59, 0, 0, 0, 0, 0, 0, 0 }, //R SHIFT
	{ 0xE0, 0x14, 0, 0, 0, 0, 0, 0 }, //R CTRL
	{ 0xE0, 0x27, 0, 0, 0, 0, 0, 0 }, //R GUI
	{ 0xE0, 0x11, 0, 0, 0, 0, 0, 0 }, //R ALT
	{ 0xE0, 0x2F, 0, 0, 0, 0, 0, 0 }, //APPS
	{ 0x5A, 0, 0, 0, 0, 0, 0, 0 }, //ENTER
	{ 0x76, 0, 0, 0, 0, 0, 0, 0 }, //ESC
	{ 0x05, 0, 0, 0, 0, 0, 0, 0 }, //F1
	{ 0x06, 0, 0, 0, 0, 0, 0, 0 }, //F2
	{ 0x04, 0, 0, 0, 0, 0, 0, 0 }, //F3
	{ 0x0C, 0, 0, 0, 0, 0, 0, 0 }, //F4
	{ 0x03, 0, 0, 0, 0, 0, 0, 0 }, //F5
	{ 0x0B, 0, 0, 0, 0, 0, 0, 0 }, //F6
	{ 0x83, 0, 0, 0, 0, 0, 0, 0 }, //F7
	{ 0x0A, 0, 0, 0, 0, 0, 0, 0 }, //F8
	{ 0x01, 0, 0, 0, 0, 0, 0, 0 }, //F9
	{ 0x09, 0, 0, 0, 0, 0, 0, 0 }, //F10
	{ 0x78, 0, 0, 0, 0, 0, 0, 0 }, //F11
	{ 0x07, 0, 0, 0, 0, 0, 0, 0 }, //F12
	{ 0xE0, 0x12, 0xE0, 0x7C, 0, 0, 0, 0 }, //PRINT SCREEN
	{ 0x7E, 0, 0, 0, 0, 0, 0, 0 }, //SCROLL
	{ 0xE1, 0x14, 0x77, 0xE1, 0xF0, 0x14, 0xF0, 0x77 }, //PAUSE
	{ 0x54, 0, 0, 0, 0, 0, 0, 0 }, //L BRACKET
	{ 0xE0, 0x70, 0, 0, 0, 0, 0, 0 }, //INSERT
	{ 0xE0, 0x6C, 0, 0, 0, 0, 0, 0 }, //HOME
	{ 0xE0, 0x7D, 0, 0, 0, 0, 0, 0 }, //PG UP
	{ 0xE0, 0x71, 0, 0, 0, 0, 0, 0 }, //DELETE
	{ 0xE0, 0x69, 0, 0, 0, 0, 0, 0 }, //END
	{ 0xE0, 0x7A, 0, 0, 0, 0, 0, 0 }, //PG DOWN
	{ 0xE0, 0x75, 0, 0, 0, 0, 0, 0 }, //UP ARROW
	{ 0xE0, 0x6B, 0, 0, 0, 0, 0, 0 }, //LEFT ARROW
	{ 0xE0, 0x72, 0, 0, 0, 0, 0, 0 }, //DOWN ARROW
	{ 0xE0, 0x74, 0, 0, 0, 0, 0, 0 }, //RIGHT ARROW
	{ 0x77, 0, 0, 0, 0, 0, 0, 0 }, //NUM
	{ 0xE0, 0x4A, 0, 0, 0, 0, 0, 0 }, //KP /
	{ 0x7C, 0, 0, 0, 0, 0, 0, 0 }, //KP *
	{ 0x7B, 0, 0, 0, 0, 0, 0, 0 }, //KP -
	{ 0x79, 0, 0, 0, 0, 0, 0, 0 }, //KP +
	{ 0xE0, 0x5A, 0, 0, 0, 0, 0, 0 }, //KP ENTER
	{ 0x71, 0, 0, 0, 0, 0, 0, 0 }, //KP .
	{ 0x70, 0, 0, 0, 0, 0, 0, 0 }, //KP 0
	{ 0x69, 0, 0, 0, 0, 0, 0, 0 }, //KP 1
	{ 0x72, 0, 0, 0, 0, 0, 0, 0 }, //KP 2
	{ 0x7A, 0, 0, 0, 0, 0, 0, 0 }, //KP 3
	{ 0x6B, 0, 0, 0, 0, 0, 0, 0 }, //KP 4
	{ 0x73, 0, 0, 0, 0, 0, 0, 0 }, //KP 5
	{ 0x74, 0, 0, 0, 0, 0, 0, 0 }, //KP 6
	{ 0x6C, 0, 0, 0, 0, 0, 0, 0 }, //KP 7
	{ 0x75, 0, 0, 0, 0, 0, 0, 0 }, //KP 8
	{ 0x7D, 0, 0, 0, 0, 0, 0, 0 }, //KP 9
	{ 0x5B, 0, 0, 0, 0, 0, 0, 0 }, //R BRACKET
	{ 0x4C, 0, 0, 0, 0, 0, 0, 0 }, //SEMICOLON
	{ 0x52, 0, 0, 0, 0, 0, 0, 0 }, //QUOTE
	{ 0x41, 0, 0, 0, 0, 0, 0, 0 }, //COMMA
	{ 0x49, 0, 0, 0, 0, 0, 0, 0 }, //PERIOD
	{ 0x4A, 0, 0, 0, 0, 0, 0, 0 }, //FORWARD SLASH
};

const uint8_t sdlconsole_scancodesBreakSet2[104][8] = {
	{ 0xF0, 0x1C, 0, 0, 0, 0, 0, 0 }, //A
	{ 0xF0, 0x32, 0, 0, 0, 0, 0, 0 }, //B
	{ 0xF0, 0x21, 0, 0, 0, 0, 0, 0 }, //C
	{ 0xF0, 0x23, 0, 0, 0, 0, 0, 0 }, //D
	{ 0xF0, 0x24, 0, 0, 0, 0, 0, 0 }, //E
	{ 0xF0, 0x2B, 0, 0, 0, 0, 0, 0 }, //F
	{ 0xF0, 0x34, 0, 0, 0, 0, 0, 0 }, //G
	{ 0xF0, 0x33, 0, 0, 0, 0, 0, 0 }, //H
	{ 0xF0, 0x43, 0, 0, 0, 0, 0, 0 }, //I
	{ 0xF0, 0x3B, 0, 0, 0, 0, 0, 0 }, //J
	{ 0xF0, 0x42, 0, 0, 0, 0, 0, 0 }, //K
	{ 0xF0, 0x2B, 0, 0, 0, 0, 0, 0 }, //L
	{ 0xF0, 0x3A, 0, 0, 0, 0, 0, 0 }, //M
	{ 0xF0, 0x31, 0, 0, 0, 0, 0, 0 }, //N
	{ 0xF0, 0x44, 0, 0, 0, 0, 0, 0 }, //O
	{ 0xF0, 0x4D, 0, 0, 0, 0, 0, 0 }, //P
	{ 0xF0, 0x15, 0, 0, 0, 0, 0, 0 }, //Q
	{ 0xF0, 0x2D, 0, 0, 0, 0, 0, 0 }, //R
	{ 0xF0, 0x1B, 0, 0, 0, 0, 0, 0 }, //S
	{ 0xF0, 0x2C, 0, 0, 0, 0, 0, 0 }, //T
	{ 0xF0, 0x3C, 0, 0, 0, 0, 0, 0 }, //U
	{ 0xF0, 0x2A, 0, 0, 0, 0, 0, 0 }, //V
	{ 0xF0, 0x1D, 0, 0, 0, 0, 0, 0 }, //W
	{ 0xF0, 0x22, 0, 0, 0, 0, 0, 0 }, //X
	{ 0xF0, 0x35, 0, 0, 0, 0, 0, 0 }, //Y
	{ 0xF0, 0x1A, 0, 0, 0, 0, 0, 0 }, //Z
	{ 0xF0, 0x45, 0, 0, 0, 0, 0, 0 }, //0
	{ 0xF0, 0x16, 0, 0, 0, 0, 0, 0 }, //1
	{ 0xF0, 0x1E, 0, 0, 0, 0, 0, 0 }, //2
	{ 0xF0, 0x26, 0, 0, 0, 0, 0, 0 }, //3
	{ 0xF0, 0x25, 0, 0, 0, 0, 0, 0 }, //4
	{ 0xF0, 0x2E, 0, 0, 0, 0, 0, 0 }, //5
	{ 0xF0, 0x36, 0, 0, 0, 0, 0, 0 }, //6
	{ 0xF0, 0x3D, 0, 0, 0, 0, 0, 0 }, //7
	{ 0xF0, 0x3E, 0, 0, 0, 0, 0, 0 }, //8
	{ 0xF0, 0x46, 0, 0, 0, 0, 0, 0 }, //9
	{ 0xF0, 0x0E, 0, 0, 0, 0, 0, 0 }, //`
	{ 0xF0, 0x4E, 0, 0, 0, 0, 0, 0 }, //-
	{ 0xF0, 0x55, 0, 0, 0, 0, 0, 0 }, //=
	{ 0xF0, 0x5D, 0, 0, 0, 0, 0, 0 }, //BACKSLASH
	{ 0xF0, 0x66, 0, 0, 0, 0, 0, 0 }, //BACKSPACE
	{ 0xF0, 0x29, 0, 0, 0, 0, 0, 0 }, //SPACE
	{ 0xF0, 0x0D, 0, 0, 0, 0, 0, 0 }, //TAB
	{ 0xF0, 0x58, 0, 0, 0, 0, 0, 0 }, //CAPS
	{ 0xF0, 0x12, 0, 0, 0, 0, 0, 0 }, //L SHIFT
	{ 0xF0, 0x14, 0, 0, 0, 0, 0, 0 }, //L CTRL
	{ 0xE0, 0xF0, 0x1F, 0, 0, 0, 0, 0 }, //L GUI
	{ 0xF0, 0x11, 0, 0, 0, 0, 0, 0 }, //L ALT
	{ 0xF0, 0x59, 0, 0, 0, 0, 0, 0 }, //R SHIFT
	{ 0xE0, 0xF0, 0x14, 0, 0, 0, 0, 0 }, //R CTRL
	{ 0xE0, 0xF0, 0x27, 0, 0, 0, 0, 0 }, //R GUI
	{ 0xE0, 0xF0, 0x11, 0, 0, 0, 0, 0 }, //R ALT
	{ 0xE0, 0xF0, 0x2F, 0, 0, 0, 0, 0 }, //APPS
	{ 0xF0, 0x5A, 0, 0, 0, 0, 0, 0 }, //ENTER
	{ 0xF0, 0x76, 0, 0, 0, 0, 0, 0 }, //ESC
	{ 0xF0, 0x05, 0, 0, 0, 0, 0, 0 }, //F1
	{ 0xF0, 0x06, 0, 0, 0, 0, 0, 0 }, //F2
	{ 0xF0, 0x04, 0, 0, 0, 0, 0, 0 }, //F3
	{ 0xF0, 0x0C, 0, 0, 0, 0, 0, 0 }, //F4
	{ 0xF0, 0x03, 0, 0, 0, 0, 0, 0 }, //F5
	{ 0xF0, 0x0B, 0, 0, 0, 0, 0, 0 }, //F6
	{ 0xF0, 0x83, 0, 0, 0, 0, 0, 0 }, //F7
	{ 0xF0, 0x0A, 0, 0, 0, 0, 0, 0 }, //F8
	{ 0xF0, 0x01, 0, 0, 0, 0, 0, 0 }, //F9
	{ 0xF0, 0x09, 0, 0, 0, 0, 0, 0 }, //F10
	{ 0xF0, 0x78, 0, 0, 0, 0, 0, 0 }, //F11
	{ 0xF0, 0x07, 0, 0, 0, 0, 0, 0 }, //F12
	{ 0xE0, 0xF0, 0x7C, 0xE0, 0xF0, 0x12, 0, 0 }, //PRINT SCREEN
	{ 0xF0, 0x7E, 0, 0, 0, 0, 0, 0 }, //SCROLL
	{ 0, 0, 0, 0, 0, 0, 0, 0 }, //PAUSE (has no break code)
	{ 0xF0, 0x54, 0, 0, 0, 0, 0, 0 }, //L BRACKET
	{ 0xE0, 0xF0, 0x70, 0, 0, 0, 0, 0 }, //INSERT
	{ 0xE0, 0xF0, 0x6C, 0, 0, 0, 0, 0 }, //HOME
	{ 0xE0, 0xF0, 0x7D, 0, 0, 0, 0, 0 }, //PG UP
	{ 0xE0, 0xF0, 0x71, 0, 0, 0, 0, 0 }, //DELETE
	{ 0xE0, 0xF0, 0x69, 0, 0, 0, 0, 0 }, //END
	{ 0xE0, 0xF0, 0x7A, 0, 0, 0, 0, 0 }, //PG DOWN
	{ 0xE0, 0xF0, 0x75, 0, 0, 0, 0, 0 }, //UP ARROW
	{ 0xE0, 0xF0, 0x6B, 0, 0, 0, 0, 0 }, //LEFT ARROW
	{ 0xE0, 0xF0, 0x72, 0, 0, 0, 0, 0 }, //DOWN ARROW
	{ 0xE0, 0xF0, 0x74, 0, 0, 0, 0, 0 }, //RIGHT ARROW
	{ 0xF0, 0x77, 0, 0, 0, 0, 0, 0 }, //NUM
	{ 0xE0, 0xF0, 0x4A, 0, 0, 0, 0, 0 }, //KP /
	{ 0xF0, 0x7C, 0, 0, 0, 0, 0, 0 }, //KP *
	{ 0xF0, 0x7B, 0, 0, 0, 0, 0, 0 }, //KP -
	{ 0xF0, 0x79, 0, 0, 0, 0, 0, 0 }, //KP +
	{ 0xE0, 0xF0, 0x5A, 0, 0, 0, 0, 0 }, //KP ENTER
	{ 0xF0, 0x71, 0, 0, 0, 0, 0, 0 }, //KP .
	{ 0xF0, 0x70, 0, 0, 0, 0, 0, 0 }, //KP 0
	{ 0xF0, 0x69, 0, 0, 0, 0, 0, 0 }, //KP 1
	{ 0xF0, 0x72, 0, 0, 0, 0, 0, 0 }, //KP 2
	{ 0xF0, 0x7A, 0, 0, 0, 0, 0, 0 }, //KP 3
	{ 0xF0, 0x6B, 0, 0, 0, 0, 0, 0 }, //KP 4
	{ 0xF0, 0x73, 0, 0, 0, 0, 0, 0 }, //KP 5
	{ 0xF0, 0x74, 0, 0, 0, 0, 0, 0 }, //KP 6
	{ 0xF0, 0x6C, 0, 0, 0, 0, 0, 0 }, //KP 7
	{ 0xF0, 0x75, 0, 0, 0, 0, 0, 0 }, //KP 8
	{ 0xF0, 0x7D, 0, 0, 0, 0, 0, 0 }, //KP 9
	{ 0xF0, 0x5B, 0, 0, 0, 0, 0, 0 }, //R BRACKET
	{ 0xF0, 0x4C, 0, 0, 0, 0, 0, 0 }, //SEMICOLON
	{ 0xF0, 0x52, 0, 0, 0, 0, 0, 0 }, //QUOTE
	{ 0xF0, 0x41, 0, 0, 0, 0, 0, 0 }, //COMMA
	{ 0xF0, 0x49, 0, 0, 0, 0, 0, 0 }, //PERIOD
	{ 0xF0, 0x4A, 0, 0, 0, 0, 0, 0 }, //FORWARD SLASH
};

const SDL_Keycode sdlconsole_SDLtoSet2Map[104] = {
	SDLK_a,
	SDLK_b,
	SDLK_c,
	SDLK_d,
	SDLK_e,
	SDLK_f,
	SDLK_g,
	SDLK_h,
	SDLK_i,
	SDLK_j,
	SDLK_k,
	SDLK_l,
	SDLK_m,
	SDLK_n,
	SDLK_o,
	SDLK_p,
	SDLK_q,
	SDLK_r,
	SDLK_s,
	SDLK_t,
	SDLK_u,
	SDLK_v,
	SDLK_w,
	SDLK_x,
	SDLK_y,
	SDLK_z,
	SDLK_0,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_4,
	SDLK_5,
	SDLK_6,
	SDLK_7,
	SDLK_8,
	SDLK_9,
	SDLK_BACKQUOTE,
	SDLK_MINUS,
	SDLK_EQUALS,
	SDLK_BACKSLASH,
	SDLK_BACKSPACE,
	SDLK_SPACE,
	SDLK_TAB,
	SDLK_CAPSLOCK,
	SDLK_LSHIFT,
	SDLK_LCTRL,
	SDLK_LGUI,
	SDLK_LALT,
	SDLK_RSHIFT,
	SDLK_RCTRL,
	SDLK_RGUI,
	SDLK_RALT,
	0, //APPS key (?)
	SDLK_RETURN,
	SDLK_ESCAPE,
	SDLK_F1,
	SDLK_F2,
	SDLK_F3,
	SDLK_F4,
	SDLK_F5,
	SDLK_F6,
	SDLK_F7,
	SDLK_F8,
	SDLK_F9,
	SDLK_F10,
	SDLK_F11,
	SDLK_F12,
	SDLK_PRINTSCREEN,
	SDLK_SCROLLLOCK,
	SDLK_PAUSE,
	SDLK_LEFTBRACKET,
	SDLK_INSERT,
	SDLK_HOME,
	SDLK_PAGEUP,
	SDLK_DELETE,
	SDLK_END,
	SDLK_PAGEDOWN,
	SDLK_UP,
	SDLK_LEFT,
	SDLK_DOWN,
	SDLK_RIGHT,
	SDLK_NUMLOCKCLEAR,
	SDLK_KP_DIVIDE,
	SDLK_KP_MULTIPLY,
	SDLK_KP_MINUS,
	SDLK_KP_PLUS,
	SDLK_KP_ENTER,
	SDLK_KP_PERIOD,
	SDLK_KP_0,
	SDLK_KP_1,
	SDLK_KP_2,
	SDLK_KP_3,
	SDLK_KP_4,
	SDLK_KP_5,
	SDLK_KP_6,
	SDLK_KP_7,
	SDLK_KP_8,
	SDLK_KP_9,
	SDLK_RIGHTBRACKET,
	SDLK_SEMICOLON,
	SDLK_QUOTE,
	SDLK_COMMA,
	SDLK_PERIOD,
	SDLK_SLASH
};

#endif
