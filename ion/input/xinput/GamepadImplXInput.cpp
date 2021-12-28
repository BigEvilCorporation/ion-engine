///////////////////////////////////////////////////
// File:		GamepadImplXInput.cpp
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	Xbox 360 gamepad handler
///////////////////////////////////////////////////

#include "GamepadImplXInput.h"

namespace ion
{
	namespace input
	{
		GamepadImpl* GamepadImplXInput::FindAvailableController(int index)
		{
			GamepadImpl* impl = nullptr;

			XINPUT_STATE state;

			if (XInputGetState(index, &state) == 0)
			{
				impl = new GamepadImplXInput(index);
			}

			return impl;
		}

		GamepadImplXInput::GamepadImplXInput(int index)
		{
			m_index = index;
			m_connected = false;
			memory::MemSet(&m_inputState, 0, sizeof(XINPUT_STATE));
			memory::MemSet(&m_prevInputState, 0, sizeof(XINPUT_STATE));
		}

		void GamepadImplXInput::Poll()
		{
			//Registered, update state
			memory::MemCopy(&m_prevInputState, &m_inputState, sizeof(m_inputState));
			int errorState = XInputGetState(m_index, &m_inputState);

			if (!errorState)
			{
				//No error, pad is connected
				m_connected = true;
			}
			else
			{
				//Error, pad disconnected
				m_connected = false;

				//Make sure values aren't cached
				memory::MemSet(&m_inputState, 0, sizeof(XINPUT_STATE));
			}
		}

		bool GamepadImplXInput::IsConnected() const
		{
			return m_connected;
		}

		GamepadType GamepadImplXInput::GetGamepadType() const
		{
			return GamepadType::Xbox360;
		}

		int GamepadImplXInput::ToPlatformButton(GamepadButtons button) const
		{
			int platformButton = 0;

			switch (button)
			{
				case GamepadButtons::DPAD_UP:			platformButton = XINPUT_GAMEPAD_DPAD_UP; break;
				case GamepadButtons::DPAD_DOWN:			platformButton = XINPUT_GAMEPAD_DPAD_DOWN; break;
				case GamepadButtons::DPAD_LEFT:			platformButton = XINPUT_GAMEPAD_DPAD_LEFT; break;
				case GamepadButtons::DPAD_RIGHT:		platformButton = XINPUT_GAMEPAD_DPAD_RIGHT; break;
				case GamepadButtons::BUTTON_A:			platformButton = XINPUT_GAMEPAD_A; break;
				case GamepadButtons::BUTTON_B:			platformButton = XINPUT_GAMEPAD_B; break;
				case GamepadButtons::BUTTON_X:			platformButton = XINPUT_GAMEPAD_X; break;
				case GamepadButtons::BUTTON_Y:			platformButton = XINPUT_GAMEPAD_Y; break;
				case GamepadButtons::START:				platformButton = XINPUT_GAMEPAD_START; break;
				case GamepadButtons::SELECT:			platformButton = XINPUT_GAMEPAD_BACK; break;
				case GamepadButtons::LEFT_TRIGGER:		platformButton = XINPUT_GAMEPAD_LEFT_SHOULDER; break;
				case GamepadButtons::RIGHT_TRIGGER:		platformButton = XINPUT_GAMEPAD_RIGHT_SHOULDER; break;
				case GamepadButtons::LEFT_STICK_CLICK:	platformButton = XINPUT_GAMEPAD_LEFT_THUMB; break;
				case GamepadButtons::RIGHT_STICK_CLICK:	platformButton = XINPUT_GAMEPAD_RIGHT_THUMB; break;
			}

			return platformButton;
		}

		ion::Vector2 GamepadImplXInput::GetLeftStick() const
		{
			ion::Vector2 leftStick((float)m_inputState.Gamepad.sThumbLX, (float)m_inputState.Gamepad.sThumbLY);

			if (leftStick.x != 0.0f)
				leftStick.x /= 32768;

			if (leftStick.y != 0.0f)
				leftStick.y /= 32768;

			return leftStick;
		}

		ion::Vector2 GamepadImplXInput::GetRightStick() const
		{
			ion::Vector2 rightStick((float)m_inputState.Gamepad.sThumbRX, (float)m_inputState.Gamepad.sThumbRY);

			if (rightStick.x != 0.0f)
				rightStick.x /= 32768;

			if (rightStick.y != 0.0f)
				rightStick.y /= 32768;

			return rightStick;
		}

		bool GamepadImplXInput::CheckButton(GamepadButtons button) const
		{
			int xInputButton = ToPlatformButton(button);
			return (m_inputState.Gamepad.wButtons & xInputButton) != 0;
		}

		bool GamepadImplXInput::CheckPrevButton(GamepadButtons button) const
		{
			int xInputButton = ToPlatformButton(button);
			return (m_prevInputState.Gamepad.wButtons & xInputButton) != 0;
		}
	}
}
