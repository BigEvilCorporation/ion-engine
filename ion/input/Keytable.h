///////////////////////////////////////////////////
// File:		Keytable.h
// Date:		25th January 2017
// Authors:		Matt Phillips
// Description:	Keycode conversion tables
///////////////////////////////////////////////////

#pragma once

#include "Keycodes.h"

namespace ion
{
    namespace input
    {
#if defined ION_PLATFORM_WINDOWS
        static int KeycodeTable[Keycode::COUNT] =
        {
			//Alpha
			DIK_A,
			DIK_B,
			DIK_C,
			DIK_D,
			DIK_E,
			DIK_F,
			DIK_G,
			DIK_H,
			DIK_I,
			DIK_J,
			DIK_K,
			DIK_L,
			DIK_M,
			DIK_N,
			DIK_O,
			DIK_P,
			DIK_Q,
			DIK_R,
			DIK_S,
			DIK_T,
			DIK_U,
			DIK_V,
			DIK_W,
			DIK_X,
			DIK_Y,
			DIK_Z,

			//Numeric (non-numpad)
			DIK_1,
			DIK_2,
			DIK_3,
			DIK_4,
			DIK_5,
			DIK_6,
			DIK_7,
			DIK_8,
			DIK_9,
			DIK_0,

			//Functions
			DIK_F1,
			DIK_F2,
			DIK_F3,
			DIK_F4,
			DIK_F5,
			DIK_F6,
			DIK_F7,
			DIK_F8,
			DIK_F9,
			DIK_F10,
			DIK_F11,
			DIK_F12,

			DIK_RETURN,
			DIK_ESCAPE,
			DIK_BACKSPACE,
			DIK_TAB,
			DIK_SPACE,
			DIK_HOME,
			DIK_PGUP,
			DIK_DELETE,
			DIK_END,
			DIK_PGDN,
			DIK_RIGHT,
			DIK_LEFT,
			DIK_DOWN,
			DIK_UP
        };
#elif defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_LINUX || defined ION_PLATFORM_RASPBERRYPI
        static int KeycodeTable[Keycode::COUNT] =
        {
            //Alpha
            SDL_SCANCODE_A,
            SDL_SCANCODE_B,
            SDL_SCANCODE_C,
            SDL_SCANCODE_D,
            SDL_SCANCODE_E,
            SDL_SCANCODE_F,
            SDL_SCANCODE_G,
            SDL_SCANCODE_H,
            SDL_SCANCODE_I,
            SDL_SCANCODE_J,
            SDL_SCANCODE_K,
            SDL_SCANCODE_L,
            SDL_SCANCODE_M,
            SDL_SCANCODE_N,
            SDL_SCANCODE_O,
            SDL_SCANCODE_P,
            SDL_SCANCODE_Q,
            SDL_SCANCODE_R,
            SDL_SCANCODE_S,
            SDL_SCANCODE_T,
            SDL_SCANCODE_U,
            SDL_SCANCODE_V,
            SDL_SCANCODE_W,
            SDL_SCANCODE_X,
            SDL_SCANCODE_Y,
            SDL_SCANCODE_Z,

            //Numeric (non-numpad)
            SDL_SCANCODE_1,
            SDL_SCANCODE_2,
            SDL_SCANCODE_3,
            SDL_SCANCODE_4,
            SDL_SCANCODE_5,
            SDL_SCANCODE_6,
            SDL_SCANCODE_7,
            SDL_SCANCODE_8,
            SDL_SCANCODE_9,
            SDL_SCANCODE_0,

            //Functions
            SDL_SCANCODE_F1,
            SDL_SCANCODE_F2,
            SDL_SCANCODE_F3,
            SDL_SCANCODE_F4,
            SDL_SCANCODE_F5,
            SDL_SCANCODE_F6,
            SDL_SCANCODE_F7,
            SDL_SCANCODE_F8,
            SDL_SCANCODE_F9,
            SDL_SCANCODE_F10,
            SDL_SCANCODE_F11,
            SDL_SCANCODE_F12,

            SDL_SCANCODE_RETURN,
            SDL_SCANCODE_ESCAPE,
            SDL_SCANCODE_BACKSPACE,
            SDL_SCANCODE_TAB,
            SDL_SCANCODE_SPACE,
            SDL_SCANCODE_HOME,
            SDL_SCANCODE_PAGEUP,
            SDL_SCANCODE_DELETE,
            SDL_SCANCODE_END,
            SDL_SCANCODE_PAGEDOWN,
            SDL_SCANCODE_RIGHT,
            SDL_SCANCODE_LEFT,
            SDL_SCANCODE_DOWN,
            SDL_SCANCODE_UP
        };
#elif defined ION_PLATFORM_DREAMCAST
        static int KeycodeTable[Keycode::COUNT] =
        {
            0,
        };
#endif
    }
}
