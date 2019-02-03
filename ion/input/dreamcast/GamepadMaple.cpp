///////////////////////////////////////////////////
// File:		GamepadXInput.cpp
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	Xbox 360 gamepad handler
///////////////////////////////////////////////////

#include "GamepadMaple.h"

namespace ion
{
	namespace input
	{
		GamepadImpl* GamepadMaple::FindAvailableController(int index)
		{
			GamepadImpl* impl = nullptr;

			if (index < maple_enum_count())
			{
				if (maple_device_t* device = maple_enum_type(index, MAPLE_FUNC_CONTROLLER))
				{
					if (cont_state_t* state = (cont_state_t*)maple_dev_status(device))
					{
						impl = new GamepadMaple(index);
					}
				}
			}

			return impl;
		}

		GamepadMaple::GamepadMaple(int index)
		{
			m_index = index;
			m_connected = false;
			memory::MemSet(&m_mapleState, 0, sizeof(cont_state_t));
		}

		void GamepadMaple::Poll()
		{
			if (maple_device_t* device = maple_enum_type(m_index, MAPLE_FUNC_CONTROLLER))
			{
				if (cont_state_t* state = (cont_state_t*)maple_dev_status(device))
				{
					m_prevMapleState = m_mapleState;
					m_mapleState = *state;
					m_connected = true;
				}
			}
			else
			{
				//Error, pad disconnected
				m_connected = false;

				//Make sure values aren't cached
				memory::MemSet(&m_mapleState, 0, sizeof(cont_state_t));
			}
		}

		bool GamepadMaple::IsConnected() const
		{
			return m_connected;
		}

		int GamepadMaple::ToPlatformButton(GamepadButtons button) const
		{
			int platformButton = 0;

			switch (button)
			{
				case GamepadButtons::DPAD_UP:			platformButton = CONT_DPAD_UP; break;
				case GamepadButtons::DPAD_DOWN:			platformButton = CONT_DPAD_DOWN; break;
				case GamepadButtons::DPAD_LEFT:			platformButton = CONT_DPAD_LEFT; break;
				case GamepadButtons::DPAD_RIGHT:		platformButton = CONT_DPAD_RIGHT; break;
				case GamepadButtons::BUTTON_A:			platformButton = CONT_A; break;
				case GamepadButtons::BUTTON_B:			platformButton = CONT_B; break;
				case GamepadButtons::BUTTON_X:			platformButton = CONT_X; break;
				case GamepadButtons::BUTTON_Y:			platformButton = CONT_Y; break;
				case GamepadButtons::START:				platformButton = CONT_START; break;
				case GamepadButtons::SELECT:			platformButton = 0; break;
				case GamepadButtons::LEFT_SHOULDER:		platformButton = CONT_C; break;
				case GamepadButtons::RIGHT_SHOULDER:	platformButton = CONT_D; break;
				case GamepadButtons::LEFT_STICK_CLICK:	platformButton = 0; break;
				case GamepadButtons::RIGHT_STICK_CLICK:	platformButton = 0; break;
			}

			return platformButton;
		}

		ion::Vector2 GamepadMaple::GetLeftStick() const
		{
			ion::Vector2 leftStick((float)m_mapleState.joyx, (float)m_mapleState.joyy);

			if (leftStick.x != 0.0f)
				leftStick.x /= 127;

			if (leftStick.y != 0.0f)
				leftStick.y /= 127;

			return leftStick;
		}

		ion::Vector2 GamepadMaple::GetRightStick() const
		{
			ion::Vector2 rightStick((float)m_mapleState.joy2x, (float)m_mapleState.joy2y);

			if (rightStick.x != 0.0f)
				rightStick.x /= 127;

			if (rightStick.y != 0.0f)
				rightStick.y /= 127;

			return rightStick;
		}

		bool GamepadMaple::CheckButton(GamepadButtons button) const
		{
			int mapleButton = ToPlatformButton(button);
			return (m_mapleState.buttons & mapleButton) != 0;
		}

		bool GamepadMaple::CheckPrevButton(GamepadButtons button) const
		{
			int mapleButton = ToPlatformButton(button);
			return (m_prevMapleState.buttons & mapleButton) != 0;
		}
	}
}
