///////////////////////////////////////////////////
// File:		GamepadImplSteam.h
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	Steam controller gamepad handler
///////////////////////////////////////////////////

#pragma once

#include <input/Gamepad.h>

#include <dependencies/Steamworks/include/steam/steam_api.h>

namespace ion
{
	namespace input
	{
		class GamepadImplSteam : public GamepadImpl
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
			GamepadImplSteam(int index, ControllerHandle_t controllerHndl);
			~GamepadImplSteam();

			ControllerHandle_t m_controllerHndl;

			int m_index;
			bool m_connected;

			bool m_buttonStates[(int)GamepadButtons::COUNT];
			bool m_buttonPrevStates[(int)GamepadButtons::COUNT];
			Vector2 m_analogStates[(int)GamepadSticks::COUNT];

			static ControllerHandle_t s_activeHndls[STEAM_CONTROLLER_MAX_COUNT];
			static ControllerDigitalActionHandle_t s_actionHndlsDigital[(int)GamepadButtons::COUNT];
			static ControllerAnalogActionHandle_t s_actionHndlsAnalog[(int)GamepadSticks::COUNT];
			static const char* s_actionHndlNamesDigital[(int)GamepadButtons::COUNT];
			static const char* s_actionHndlNamesAnalog[(int)GamepadSticks::COUNT];
		};
	}
}
