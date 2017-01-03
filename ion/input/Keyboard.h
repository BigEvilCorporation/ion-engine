///////////////////////////////////////////////////
// File:		Keyboard.h
// Date:		1st August 2011
// Authors:		Matt Phillips
// Description:	Keyboard input class
///////////////////////////////////////////////////

#pragma once

#if defined ION_PLATFORM_WINDOWS
#include <dinput.h>
#endif

#include <vector>

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
			virtual void OnKeyDown(int key) = 0;
			virtual void OnKeyUp(int key) = 0;
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
			bool KeyDown(int key) const;

			//Was key pressed this frame
			bool KeyPressedThisFrame(int key) const;

			void Update();

			void SetCooperativeWindow(CoopLevel coopLevel);

		private:
			static const int sMaxKeys = 256;
			char mCurrKeyStates[sMaxKeys];
			char mPrevKeyStates[sMaxKeys];

			std::vector<KeyboardHandler*> m_handlers;

#if defined ION_PLATFORM_WINDOWS
			LPDIRECTINPUTDEVICE8 mKeyboardDevice;
#endif
		};
	}
}