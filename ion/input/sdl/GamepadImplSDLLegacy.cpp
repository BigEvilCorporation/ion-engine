///////////////////////////////////////////////////
// File:		GamepadImplSDLLegacy.cpp
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	SDL legacy gamepad handler
///////////////////////////////////////////////////

#if ION_INPUT_SUPPORTS_SDL2

#include "GamepadImplSDLLegacy.h"

#define SDL_JOYSTICK_BUTTON_DPAD_UP			13 
#define SDL_JOYSTICK_BUTTON_DPAD_DOWN		15
#define SDL_JOYSTICK_BUTTON_DPAD_LEFT		16
#define SDL_JOYSTICK_BUTTON_DPAD_RIGHT		14
#define SDL_JOYSTICK_BUTTON_A				1
#define SDL_JOYSTICK_BUTTON_B				2
#define SDL_JOYSTICK_BUTTON_X				3
#define SDL_JOYSTICK_BUTTON_Y				4
#define SDL_JOYSTICK_BUTTON_START			8
#define SDL_JOYSTICK_BUTTON_BACK			7
#define SDL_JOYSTICK_BUTTON_LEFTSHOULDER	5
#define SDL_JOYSTICK_BUTTON_RIGHTSHOULDER	6
#define SDL_JOYSTICK_BUTTON_LEFTSTICK		9
#define SDL_JOYSTICK_BUTTON_RIGHTSTICK		10

#define SDL_JOYSTICK_AXIS_LEFTX				0
#define SDL_JOYSTICK_AXIS_LEFTY				1
#define SDL_JOYSTICK_AXIS_RIGHTX			2
#define SDL_JOYSTICK_AXIS_RIGHTY			3

#ifndef SDL_JOYSTICK_AXIS_MAX
#define SDL_JOYSTICK_AXIS_MAX   32767
#endif

namespace ion
{
	namespace input
	{
		GamepadImplSDLLegacy::GamepadImplSDLLegacy(int index, SDL_Joystick* sdlJoystick)
		{
			m_index = index;
			m_sdlJoystick = sdlJoystick;
			m_connected = false;

			m_numAxis = SDL_JoystickNumAxes(m_sdlJoystick);
			m_numButtons = SDL_JoystickNumButtons(m_sdlJoystick);

			m_axis = new s16[m_numAxis];
			m_buttons = new u8[m_numButtons];
			m_prevButtons = new u8[m_numButtons];

			memory::MemSet(m_axis, 0, sizeof(s16) * m_numAxis);
			memory::MemSet(m_buttons, 0, sizeof(u8) * m_numButtons);
			memory::MemSet(m_prevButtons, 0, sizeof(u8) * m_numButtons);
		}

		GamepadImplSDLLegacy::~GamepadImplSDLLegacy()
		{
			SDL_JoystickClose(m_sdlJoystick);
			delete m_axis;
			delete m_buttons;
			delete m_prevButtons;
		}

		void GamepadImplSDLLegacy::Poll()
		{
			m_connected = m_sdlJoystick && SDL_JoystickGetAttached(m_sdlJoystick);
			if (m_connected)
			{
				for (int i = 0; i < m_numAxis; i++)
				{
					m_axis[i] = SDL_JoystickGetAxis(m_sdlJoystick, i);
				}

				for (int i = 0; i < m_numButtons; i++)
				{
					m_prevButtons[i] = m_buttons[i];
					m_buttons[i] = SDL_JoystickGetButton(m_sdlJoystick, i);
				}
			}
		}

		bool GamepadImplSDLLegacy::IsConnected() const
		{
			return m_connected;
		}

		GamepadType GamepadImplSDLLegacy::GetGamepadType() const
		{
			return GamepadType::Generic;
		}

		int GamepadImplSDLLegacy::ToPlatformButton(GamepadButtons button) const
		{
			int platformButton = 0;

			switch (button)
			{
				case GamepadButtons::DPAD_UP:			platformButton = SDL_JOYSTICK_BUTTON_DPAD_UP; break;
				case GamepadButtons::DPAD_DOWN:			platformButton = SDL_JOYSTICK_BUTTON_DPAD_DOWN; break;
				case GamepadButtons::DPAD_LEFT:			platformButton = SDL_JOYSTICK_BUTTON_DPAD_LEFT; break;
				case GamepadButtons::DPAD_RIGHT:		platformButton = SDL_JOYSTICK_BUTTON_DPAD_RIGHT; break;
				case GamepadButtons::BUTTON_A:			platformButton = SDL_JOYSTICK_BUTTON_A; break;
				case GamepadButtons::BUTTON_B:			platformButton = SDL_JOYSTICK_BUTTON_B; break;
				case GamepadButtons::BUTTON_X:			platformButton = SDL_JOYSTICK_BUTTON_X; break;
				case GamepadButtons::BUTTON_Y:			platformButton = SDL_JOYSTICK_BUTTON_Y; break;
				case GamepadButtons::START:				platformButton = SDL_JOYSTICK_BUTTON_START; break;
				case GamepadButtons::SELECT:			platformButton = SDL_JOYSTICK_BUTTON_BACK; break;
				case GamepadButtons::LEFT_TRIGGER:		platformButton = SDL_JOYSTICK_BUTTON_LEFTSHOULDER; break;
				case GamepadButtons::RIGHT_TRIGGER:	platformButton = SDL_JOYSTICK_BUTTON_RIGHTSHOULDER; break;
				case GamepadButtons::LEFT_STICK_CLICK:	platformButton = SDL_JOYSTICK_BUTTON_LEFTSTICK; break;
				case GamepadButtons::RIGHT_STICK_CLICK:	platformButton = SDL_JOYSTICK_BUTTON_RIGHTSTICK; break;
			}

			return platformButton;
		}

		ion::Vector2 GamepadImplSDLLegacy::GetLeftStick() const
		{
			ion::Vector2 leftStick;

			if(m_numAxis > SDL_JOYSTICK_AXIS_LEFTX)
				leftStick.x = (float)m_axis[SDL_JOYSTICK_AXIS_LEFTX];

			if (m_numAxis > SDL_JOYSTICK_AXIS_LEFTY)
				leftStick.y = (float)-m_axis[SDL_JOYSTICK_AXIS_LEFTY];

			if (leftStick.x != 0.0f)
				leftStick.x /= SDL_JOYSTICK_AXIS_MAX;

			if (leftStick.y != 0.0f)
				leftStick.y /= SDL_JOYSTICK_AXIS_MAX;

			return leftStick;
		}

		ion::Vector2 GamepadImplSDLLegacy::GetRightStick() const
		{
			ion::Vector2 rightStick;

			if (m_numAxis > SDL_JOYSTICK_AXIS_RIGHTX)
				rightStick.x = (float)m_axis[SDL_JOYSTICK_AXIS_RIGHTX];

			if (m_numAxis > SDL_JOYSTICK_AXIS_RIGHTY)
				rightStick.y = (float)m_axis[SDL_JOYSTICK_AXIS_RIGHTY];

			if (rightStick.x != 0.0f)
				rightStick.x /= SDL_JOYSTICK_AXIS_MAX;

			if (rightStick.y != 0.0f)
				rightStick.y /= SDL_JOYSTICK_AXIS_MAX;

			return rightStick;
		}

		bool GamepadImplSDLLegacy::CheckButton(GamepadButtons button) const
		{
			int sdlButton = ToPlatformButton(button);
			return (sdlButton < m_numButtons) ? (m_buttons[sdlButton] != 0) : false;
		}

		bool GamepadImplSDLLegacy::CheckPrevButton(GamepadButtons button) const
		{
			int sdlButton = ToPlatformButton(button);
			return (sdlButton < m_numButtons) ? (m_prevButtons[sdlButton] != 0) : false;
		}
	}
}

#endif // ION_INPUT_SUPPORTS_SDL2