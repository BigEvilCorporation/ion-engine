///////////////////////////////////////////////////
// File:		GamepadImplSteam.cpp
// Date:		1st August 2018
// Authors:		Matt Phillips
// Description:	Steam controller gamepad handler
///////////////////////////////////////////////////

#include "GamepadImplSteam.h"

namespace ion
{
	namespace input
	{
		ControllerHandle_t GamepadImplSteam::s_activeHndls[STEAM_CONTROLLER_MAX_COUNT] = { 0 };
		ControllerDigitalActionHandle_t GamepadImplSteam::s_actionHndlsDigital[(int)GamepadButtons::COUNT] = { 0 };
		ControllerAnalogActionHandle_t GamepadImplSteam::s_actionHndlsAnalog[(int)GamepadSticks::COUNT] = { 0 };

		const char* GamepadImplSteam::s_actionHndlNamesDigital[(int)GamepadButtons::COUNT] =
		{
			"DPAD_UP",
			"DPAD_DOWN",
			"DPAD_LEFT",
			"DPAD_RIGHT",
			"BUTTON_A",
			"BUTTON_B",
			"BUTTON_X",
			"BUTTON_Y",
			"START",
			"SELECT",
			"LEFT_TRIGGER",
			"RIGHT_TRIGGER",
			"LEFT_STICK_CLICK",
			"RIGHT_STICK_CLICK",
		};

		const char* GamepadImplSteam::s_actionHndlNamesAnalog[(int)GamepadSticks::COUNT] =
		{
			"LEFT_ANALOG",
			"RIGHT_ANALOG"
		};

		GamepadImpl* GamepadImplSteam::FindAvailableController(int index)
		{
			if (!SteamAPI_Init())
			{
				debug::Error("GamepadImplSteam::FindAvailableController() - Could not initialise Steam API");
			}

			if (!SteamController()->Init())
			{
				debug::Error("GamepadImplSteam::FindAvailableController() - Could not initialise Steam Controller API");
			}

			for (int i = 0; i < (int)GamepadButtons::COUNT; i++)
			{
				s_actionHndlsDigital[i] = SteamController()->GetDigitalActionHandle(s_actionHndlNamesDigital[i]);
			}

			for (int i = 0; i < (int)GamepadSticks::COUNT; i++)
			{
				s_actionHndlsAnalog[i] = SteamController()->GetDigitalActionHandle(s_actionHndlNamesAnalog[i]);
			}

			GamepadImpl* impl = nullptr;

			SteamController()->RunFrame();

			ControllerHandle_t controllerHandles[STEAM_CONTROLLER_MAX_COUNT];
			int numActive = SteamController()->GetConnectedControllers(controllerHandles);

			if(index < numActive)
			{
				if (!s_activeHndls[index])
				{
					impl = new GamepadImplSteam(index, controllerHandles[index]);
					s_activeHndls[index] = controllerHandles[index];
				}
			}

			return impl;
		}

		GamepadImplSteam::GamepadImplSteam(int index, ControllerHandle_t controllerHndl)
		{
			m_index = index;
			m_connected = false;
			m_controllerHndl = controllerHndl;

			memory::MemSet(&m_buttonStates, 0, sizeof(bool) * (int)GamepadButtons::COUNT);
			memory::MemSet(&m_buttonPrevStates, 0, sizeof(bool) * (int)GamepadButtons::COUNT);
		}

		GamepadImplSteam::~GamepadImplSteam()
		{
			s_activeHndls[m_index] = 0;
		}

		void GamepadImplSteam::Poll()
		{
			SteamController()->RunFrame();

			memory::MemCopy(&m_buttonPrevStates, &m_buttonStates, sizeof(bool) * (int)GamepadButtons::COUNT);

			for (int i = 0; i < (int)GamepadButtons::COUNT; i++)
			{
				ControllerDigitalActionData_t digitalData = SteamController()->GetDigitalActionData(m_controllerHndl, s_actionHndlsDigital[i]);

				if (digitalData.bActive)
				{
					m_buttonStates[i] = digitalData.bState;
				}
			}

			for (int i = 0; i < (int)GamepadSticks::COUNT; i++)
			{
				ControllerAnalogActionData_t analogData = SteamController()->GetAnalogActionData(m_controllerHndl, s_actionHndlsAnalog[i]);

				if (analogData.bActive)
				{
					m_analogStates[i].x = analogData.x;
					m_analogStates[i].y = analogData.y;
				}
			}
		}

		bool GamepadImplSteam::IsConnected() const
		{
			return m_connected;
		}

		int GamepadImplSteam::ToPlatformButton(GamepadButtons button) const
		{
			return (int)button;
		}

		ion::Vector2 GamepadImplSteam::GetLeftStick() const
		{
			return m_analogStates[(int)GamepadSticks::LEFT];
		}

		ion::Vector2 GamepadImplSteam::GetRightStick() const
		{
			return m_analogStates[(int)GamepadSticks::RIGHT];
		}

		bool GamepadImplSteam::CheckButton(GamepadButtons button) const
		{
			int buttonIdx = ToPlatformButton(button);
			return m_buttonStates[buttonIdx];
		}

		bool GamepadImplSteam::CheckPrevButton(GamepadButtons button) const
		{
			int buttonIdx = ToPlatformButton(button);
			return m_buttonPrevStates[buttonIdx];
		}
	}
}
