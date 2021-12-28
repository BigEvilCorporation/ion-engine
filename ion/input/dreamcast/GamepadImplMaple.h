///////////////////////////////////////////////////
// File:		GamepadImplMaple.h
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	Dreamcast Maple gamepad handler
///////////////////////////////////////////////////

#pragma once

#include <input/Gamepad.h>

namespace ion
{
	namespace input
	{
		class GamepadImplMaple : public GamepadImpl
		{
		public:
			static GamepadImpl* FindAvailableController(int index);

			virtual void Poll();
			virtual bool IsConnected() const;
			virtual int ToPlatformButton(GamepadButtons button) const;
			virtual ion::Vector2 GetLeftStick() const;
			virtual ion::Vector2 GetRightStick() const;
			virtual bool CheckButton(GamepadButtons button) const;
			virtual bool CheckPrevButton(GamepadButtons button) const;

		private:
			GamepadImplMaple(int index);

			cont_state_t m_mapleState;
			cont_state_t m_prevMapleState;

			int m_index;
			bool m_connected;
		};
	}
}
