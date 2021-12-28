///////////////////////////////////////////////////
// File:		GamepadImplSDL.h
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	SDL gamepad handler
///////////////////////////////////////////////////

#pragma once

#if ION_INPUT_SUPPORTS_SDL2

#include <input/Gamepad.h>

#include <SDL2/SDL.h>

namespace ion
{
	namespace input
	{
		class GamepadImplSDL : public GamepadImpl
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
			GamepadImplSDL(int index, SDL_GameController* sdlController);

			int m_index;
			bool m_connected;

			SDL_GameController* m_sdlController;
			s16 m_axis[SDL_CONTROLLER_AXIS_MAX];
			u8 m_buttons[SDL_CONTROLLER_BUTTON_MAX];
			u8 m_prevButtons[SDL_CONTROLLER_BUTTON_MAX];

			enum class AdditionalPadTypes
			{
				MayflashMDUSB,
				Count
			};

			static const char* s_additionalPads[(int)AdditionalPadTypes::Count];
			static bool s_sdlSubsystemInitialised;
		};
	}
}

#endif // ION_INPUT_SUPPORTS_SDL2