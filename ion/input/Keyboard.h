///////////////////////////////////////////////////
// File:		Keyboard.h
// Date:		1st August 2011
// Authors:		Matt Phillips
// Description:	Keyboard input class
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"

#if defined ION_PLATFORM_WINDOWS
#include <XInput.h>
#include <dinput.h>
#elif defined ION_PLATFORM_MACOSX
#include <SDL.h>
#elif defined ION_PLATFORM_LINUX
#include <SDL2/SDL.h>
#endif

#include <vector>

#include "Keycodes.h"

namespace ion
{
	namespace input
	{
#if defined ION_PLATFORM_WINDOWS
		//TODO: Platform specific header
		static LPDIRECTINPUT8 g_DirectInputInterface = NULL;
#endif

		class KeyboardHandler
		{
		public:
			virtual void OnKeyDown(ion::input::Keycode key) = 0;
			virtual void OnKeyUp(ion::input::Keycode key) = 0;
		};

		class Keyboard
		{
		public:
			enum CoopLevel { Background, Foreground, Exclusive };

			Keyboard();
			~Keyboard();

			void RegisterHandler(KeyboardHandler& handler);
			void UnregisterHandler(KeyboardHandler& handler);

			//Is key currently down
			bool KeyDown(Keycode key) const;

			//Was key pressed this frame
			bool KeyPressedThisFrame(Keycode key) const;

			void Update();

			void SetCooperativeWindow(CoopLevel coopLevel);

		private:
			char mCurrKeyStates[Keycode::COUNT];
			char mPrevKeyStates[Keycode::COUNT];

			std::vector<KeyboardHandler*> m_handlers;

#if defined ION_PLATFORM_WINDOWS
			LPDIRECTINPUTDEVICE8 mKeyboardDevice;
#endif
		};
	}
}
