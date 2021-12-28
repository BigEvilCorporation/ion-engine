///////////////////////////////////////////////////
// File:		GamepadImplSDLLegacy.h
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	SDL legacy gamepad handler
///////////////////////////////////////////////////

#pragma once

#if ION_INPUT_SUPPORTS_SDL2

#include <input/Gamepad.h>

#include <SDL2/SDL.h>

namespace ion
{
	namespace input
	{
		class GamepadImplSDLLegacy : public GamepadImpl
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
			GamepadImplSDLLegacy(int index, SDL_Joystick* sdlJoystick);
			~GamepadImplSDLLegacy();

			int m_index;
			bool m_connected;

			SDL_Joystick* m_sdlJoystick;

			s16* m_axis;
			u8* m_buttons;
			u8* m_prevButtons;

			int m_numAxis;
			int m_numButtons;

			friend class GamepadImplSDL;
		};
	}
}

#endif // ION_INPUT_SUPPORTS_SDL2