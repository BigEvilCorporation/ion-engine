///////////////////////////////////////////////////
// File:		GamepadImplSDL.cpp
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	SDL gamepad handler
///////////////////////////////////////////////////

#if ION_INPUT_SUPPORTS_SDL2

#include "GamepadImplSDL.h"
#include "GamepadImplSDLLegacy.h"

namespace ion
{
	namespace input
	{
		const char* GamepadImplSDL::s_additionalPads[(int)AdditionalPadTypes::Count] =
		{
			"03000000790000002418000000000000,Mayflash MD USB Adapter,platform:Windows,a:b1,b:b2,x:b0,start:b9,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,"
		};

		bool GamepadImplSDL::s_sdlSubsystemInitialised = false;

		GamepadImpl* GamepadImplSDL::FindAvailableController(int index)
		{
			if (!s_sdlSubsystemInitialised)
			{
				if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0)
				{
					debug::Error("Gamepad::Gamepad() - SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) failed");
				}

				//Add additional mappings
#if defined ION_INPUT_SDL_GAMEPAD_MAPPINGS
				for (int i = 0; i < AdditionalPadCount; i++)
				{
					SDL_GameControllerAddMapping(s_additionalPads[i]);
				}
#endif

				s_sdlSubsystemInitialised = true;
			}

			GamepadImpl* impl = nullptr;

			//Try SDL game controller
			if ((index < SDL_NumJoysticks()) && SDL_IsGameController(index))
			{
				SDL_GameController* sdlController = SDL_GameControllerOpen(index);

				if (sdlController)
				{
					impl = new GamepadImplSDL(index, sdlController);
				}
			}

			//Try SDL legacy joystick
			if (!impl)
			{
				if ((index < SDL_NumJoysticks()) && SDL_JoystickOpen(index))
				{
					SDL_Joystick* sdlJoystick = SDL_JoystickOpen(index);

					if (sdlJoystick)
					{
						impl = new GamepadImplSDLLegacy(index, sdlJoystick);
					}
				}
			}

			return impl;
		}

		GamepadImplSDL::GamepadImplSDL(int index, SDL_GameController* sdlController)
		{
			m_index = index;
			m_sdlController = sdlController;
			m_connected = false;
			memory::MemSet(&m_axis, 0, sizeof(s16) * SDL_CONTROLLER_AXIS_MAX);
			memory::MemSet(&m_buttons, 0, sizeof(u8) * SDL_CONTROLLER_BUTTON_MAX);
		}

		void GamepadImplSDL::Poll()
		{
			m_connected = m_sdlController && SDL_GameControllerGetAttached(m_sdlController);
			if (m_connected)
			{
				for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++)
				{
					m_axis[i] = SDL_GameControllerGetAxis(m_sdlController, (SDL_GameControllerAxis)i);
				}

				for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
				{
					m_prevButtons[i] = m_buttons[i];
					m_buttons[i] = SDL_GameControllerGetButton(m_sdlController, (SDL_GameControllerButton)i);
				}
			}
		}

		bool GamepadImplSDL::IsConnected() const
		{
			return m_connected;
		}

		GamepadType GamepadImplSDL::GetGamepadType() const
		{
			return GamepadType::Generic;
		}

		int GamepadImplSDL::ToPlatformButton(GamepadButtons button) const
		{
			int platformButton = 0;

			switch (button)
			{
				case GamepadButtons::DPAD_UP:			platformButton = SDL_CONTROLLER_BUTTON_DPAD_UP; break;
				case GamepadButtons::DPAD_DOWN:			platformButton = SDL_CONTROLLER_BUTTON_DPAD_DOWN; break;
				case GamepadButtons::DPAD_LEFT:			platformButton = SDL_CONTROLLER_BUTTON_DPAD_LEFT; break;
				case GamepadButtons::DPAD_RIGHT:		platformButton = SDL_CONTROLLER_BUTTON_DPAD_RIGHT; break;
				case GamepadButtons::BUTTON_A:			platformButton = SDL_CONTROLLER_BUTTON_A; break;
				case GamepadButtons::BUTTON_B:			platformButton = SDL_CONTROLLER_BUTTON_B; break;
				case GamepadButtons::BUTTON_X:			platformButton = SDL_CONTROLLER_BUTTON_X; break;
				case GamepadButtons::BUTTON_Y:			platformButton = SDL_CONTROLLER_BUTTON_Y; break;
				case GamepadButtons::START:				platformButton = SDL_CONTROLLER_BUTTON_START; break;
				case GamepadButtons::SELECT:			platformButton = SDL_CONTROLLER_BUTTON_BACK; break;
				case GamepadButtons::LEFT_TRIGGER:		platformButton = SDL_CONTROLLER_BUTTON_LEFTSHOULDER; break;
				case GamepadButtons::RIGHT_TRIGGER:	platformButton = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER; break;
				case GamepadButtons::LEFT_STICK_CLICK:	platformButton = SDL_CONTROLLER_BUTTON_LEFTSTICK; break;
				case GamepadButtons::RIGHT_STICK_CLICK:	platformButton = SDL_CONTROLLER_BUTTON_RIGHTSTICK; break;
			}

			return platformButton;
		}

		ion::Vector2 GamepadImplSDL::GetLeftStick() const
		{
			ion::Vector2 leftStick((float)m_axis[SDL_CONTROLLER_AXIS_LEFTX], (float)-m_axis[SDL_CONTROLLER_AXIS_LEFTY]);

			if (leftStick.x != 0.0f)
				leftStick.x /= 32768;

			if (leftStick.y != 0.0f)
				leftStick.y /= 32768;

			return leftStick;
		}

		ion::Vector2 GamepadImplSDL::GetRightStick() const
		{
			ion::Vector2 rightStick((float)m_axis[SDL_CONTROLLER_AXIS_RIGHTX], (float)m_axis[SDL_CONTROLLER_AXIS_RIGHTY]);

			if (rightStick.x != 0.0f)
				rightStick.x /= 32768;

			if (rightStick.y != 0.0f)
				rightStick.y /= 32768;

			return rightStick;
		}

		bool GamepadImplSDL::CheckButton(GamepadButtons button) const
		{
			int sdlButton = ToPlatformButton(button);
			return m_buttons[sdlButton] != 0;
		}

		bool GamepadImplSDL::CheckPrevButton(GamepadButtons button) const
		{
			int sdlButton = ToPlatformButton(button);
			return m_prevButtons[sdlButton] != 0;
		}
	}
}

#endif // ION_INPUT_SUPPORTS_SDL2