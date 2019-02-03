///////////////////////////////////////////////////
// File:		Keycodes.h
// Date:		25th January 2017
// Authors:		Matt Phillips
// Description:	Keyboard keys
///////////////////////////////////////////////////

#pragma once

namespace ion
{
    namespace input
    {
        //Very incomplete!
        enum class Keycode : int
        {
            //Alpha
            A,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,
            
            //Numeric (non-numpad)
            N1,
            N2,
            N3,
            N4,
            N5,
            N6,
            N7,
            N8,
            N9,
            N0,
            
            //Functions
            F1,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,
            
            RETURN,
            ESCAPE,
            BACKSPACE,
            TAB,
            SPACE,
            HOME,
            PAGEUP,
            DELETE,
            END,
            PAGEDOWN,
            RIGHT,
            LEFT,
            DOWN,
            UP,
            
            COUNT
        };

		static const char* KeycodeNames[(int)Keycode::COUNT] =
		{
			"A",
			"B",
			"C",
			"D",
			"E",
			"F",
			"G",
			"H",
			"I",
			"J",
			"K",
			"L",
			"M",
			"N",
			"O",
			"P",
			"Q",
			"R",
			"S",
			"T",
			"U",
			"V",
			"W",
			"X",
			"Y",
			"Z",
			"N1",
			"N2",
			"N3",
			"N4",
			"N5",
			"N6",
			"N7",
			"N8",
			"N9",
			"N0",
			"F1",
			"F2",
			"F3",
			"F4",
			"F5",
			"F6",
			"F7",
			"F8",
			"F9",
			"F10",
			"F11",
			"F12",
			"RETURN",
			"ESCAPE",
			"BACKSPACE",
			"TAB",
			"SPACE",
			"HOME",
			"PAGEUP",
			"DELETE",
			"END",
			"PAGEDOWN",
			"RIGHT",
			"LEFT",
			"DOWN",
			"UP"
		};
	}
}
