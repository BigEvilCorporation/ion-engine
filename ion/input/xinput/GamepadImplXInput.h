///////////////////////////////////////////////////
// File:		GamepadXInput.h
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	Xbox 360 gamepad handler
///////////////////////////////////////////////////

#pragma once

#include <input/Gamepad.h>

#include <XInput.h>

namespace ion
{
	namespace input
	{
		class GamepadImplXInput : public GamepadImpl
		{
		public:
			static GamepadImpl* FindAvailableController(int index);

			virtual void Poll();
			virtual bool IsConnected() const;
			virtual GamepadType GetGamepadType() const;
			virtual int ToPlatformButton(GamepadButtons button) const;
			virtual ion::Vector2 GetLeftStick() const;
			virtual ion::Vector2 GetRightStick() const;
			virtual bool CheckButton(GamepadButtons button) const;
			virtual bool CheckPrevButton(GamepadButtons button) const;

		private:
			GamepadImplXInput(int index);

			XINPUT_STATE m_inputState;
			XINPUT_STATE m_prevInputState;

			int m_index;
			bool m_connected;
		};
	}
}
