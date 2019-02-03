///////////////////////////////////////////////////
// File:		GamepadSDLLegacy.h
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	SDL legacy gamepad handler
///////////////////////////////////////////////////

#pragma once

#include <input/Gamepad.h>

#include <SDL2/SDL.h>

namespace ion
{
	namespace input
	{
		class GamepadSDLLegacy : public GamepadImpl
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
			GamepadSDLLegacy(int index, SDL_Joystick* sdlJoystick);
			~GamepadSDLLegacy();

			int m_index;
			bool m_connected;

			SDL_Joystick* m_sdlJoystick;

			s16* m_axis;
			u8* m_buttons;
			u8* m_prevButtons;

			int m_numAxis;
			int m_numButtons;

			friend class GamepadSDL;
		};
	}
}
