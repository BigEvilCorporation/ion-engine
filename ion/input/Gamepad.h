///////////////////////////////////////////////////
// File:		Gamepad.h
// Date:		2nd August 2011
// Authors:		Matt Phillips
// Description:	Gamepad input handler
///////////////////////////////////////////////////

#pragma once

#include "core/memory/Memory.h"
#include "maths/Vector.h"

#if defined ION_PLATFORM_WINDOWS
//TODO: Windows headers in platform header
#include <windows.h>
#include <XInput.h>
#elif defined ION_PLATFORM_DREAMCAST
#include <kos.h>
#endif

namespace ion
{
	namespace input
	{
		class Gamepad
		{
		public:
			enum Buttons
			{
				DPAD_UP				= 1 << 1,
				DPAD_DOWN			= 1 << 2,
				DPAD_LEFT			= 1 << 3,
				DPAD_RIGHT			= 1 << 4,

				BUTTON_A			= 1 << 5,
				BUTTON_B			= 1 << 6,
				BUTTON_X			= 1 << 7,
				BUTTON_Y			= 1 << 8,

				START				= 1 << 9,
				SELECT				= 1 << 10,

				LEFT_SHOULDER		= 1 << 11,
				RIGHT_SHOULDER		= 1 << 12,
				
				LEFT_STICK_CLICK	= 1 << 13,
				RIGHT_STICK_CLICK	= 1 << 14
			};

			Gamepad();
			~Gamepad();

			//Update input/connection state
			void Update();
			
			//Test if connected
			bool IsConnected() const;

			//Find available gamepads
			bool FindConnectedGamepad();

			ion::Vector2 GetLeftStick() const;
			ion::Vector2 GetRightStick() const;

			bool ButtonDown(Buttons button) const;

			void SetDeadZone(float deadZone);
			float GetDeadZone() const;

			void SetOuterZone(float outerZone);
			float GetOuterZone() const;

		private:
			static int FindAvailableController();
			static int ToPlatformButton(Buttons button);

			int mControllerIndex;
			bool mConnected;
			float mDeadZone;
			float mOuterZone;

#if defined ION_PLATFORM_WINDOWS
			XINPUT_STATE mInputState;
#elif defined ION_PLATFORM_DREAMCAST
			cont_state_t m_mapleState;
#endif

			static const int sInvalidIndex = -1;
			static const int sMaxControllers = 4;
			static bool sRegisteredControllers[sMaxControllers];
		};
	}
}