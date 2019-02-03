///////////////////////////////////////////////////
// File:		GamepadSDL.h
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	SDL gamepad handler
///////////////////////////////////////////////////

#pragma once

#include <input/Gamepad.h>

#include <SDL2/SDL.h>

namespace ion
{
	namespace input
	{
		class GamepadSDL : public GamepadImpl
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
			GamepadSDL(int index, SDL_GameController* sdlController);

			int m_index;
			bool m_connected;

			SDL_GameController* m_sdlController;
			s16 m_axis[SDL_CONTROLLER_AXIS_MAX];
			u8 m_buttons[SDL_CONTROLLER_BUTTON_MAX];
			u8 m_prevButtons[SDL_CONTROLLER_BUTTON_MAX];

			enum AdditionalPadTypes
			{
				MayflashMDUSB,

				AdditionalPadCount
			};

			static const char* s_additionalPads[AdditionalPadCount];
			static bool s_sdlSubsystemInitialised;
		};
	}
}
