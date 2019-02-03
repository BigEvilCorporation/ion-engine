///////////////////////////////////////////////////
// File:		Mouse.h
// Date:		3rd August 2011
// Authors:		Matt Phillips
// Description:	Mouse input class
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"

//Need DirectInput header and device from Keyboard.h. TODO: Clean this up
#include "Keyboard.h"

namespace ion
{
	namespace input
	{
		class Mouse
		{
		public:
			enum Buttons { LB, RB, MB };
			enum CoopLevel { Background, Foreground, Exclusive };

			Mouse();
			~Mouse();

			void Update();

			bool ButtonDown(Buttons button);

			s32 GetDeltaX();
			s32 GetDeltaY();
			s32 GetAbsoluteX();
			s32 GetAbsoluteY();

			s32 GetWheelDelta();
			s32 GetWheelAbsolute();

			void ShowCursor(bool enabled);

			void SetCooperativeWindow(CoopLevel coopLevel);

		private:
			s32 mAbsX;
			s32 mAbsY;
			s32 mWheelAbs;

#if defined ION_PLATFORM_WINDOWS
			LPDIRECTINPUTDEVICE8 mMouseDevice;
			DIMOUSESTATE2 mMouseState;
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
			s32 m_sdlMouseRelX;
			s32 m_sdlMouseRelY;
			u32 m_sdlMouseButtons;
#endif
		};
	}
}