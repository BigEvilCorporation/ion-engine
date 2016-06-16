///////////////////////////////////////////////////
// File:		Keyboard.h
// Date:		1st August 2011
// Authors:		Matt Phillips
// Description:	Keyboard input class
///////////////////////////////////////////////////

#pragma once

#include <dinput.h>
#include <vector>

namespace ion
{
	namespace input
	{
		//TODO: Platform specific header
		static LPDIRECTINPUT8 g_DirectInputInterface = NULL;

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

			LPDIRECTINPUTDEVICE8 mKeyboardDevice;

			std::vector<KeyboardHandler*> m_handlers;
		};
	}
}