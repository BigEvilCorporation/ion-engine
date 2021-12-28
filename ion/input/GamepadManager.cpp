///////////////////////////////////////////////////
// File:		GamepadManager.cpp
// Date:		1st May 2021
// Authors:		Matt Phillips
// Description:	Gamepad manager
///////////////////////////////////////////////////

#include "GamepadManager.h"

#if defined ION_INPUT_XINPUT
#include "xinput/GamepadImplXInput.h"
#endif

#if defined ION_INPUT_SDL
#include "sdl/GamepadImplSDL.h"
#endif

#if defined ION_INPUT_MAPLE
#include "dreamcast/GamepadImplMaple.h"
#endif

#if defined ION_INPUT_STEAM
#include "steam/GamepadImplSteam.h"
#endif

#if defined ION_INPUT_SWITCH
#include "switch/GamepadImplSwitch.h"
#endif

namespace ion
{
	namespace input
	{
		GamepadManager::GamepadManager(int maxControllers)
		{
			ion::debug::Assert(maxControllers <= s_maxControllersSupported, "GamepadManager::SetMaxControllers() - Max supported controllers exceeded");
			m_maxControllersRequested = maxControllers;
		}

		GamepadManager::~GamepadManager()
		{
			for (int i = 0; i < s_maxControllersSupported; i++)
			{
				if (m_gamepads[i].second)
				{
					DestroyImplementation(i);
				}

				if (m_gamepads[i].first)
				{
					delete m_gamepads[i].first;
				}
			}
		}

		void GamepadManager::Update()
		{
			for (int i = 0; i < s_maxControllersSupported; i++)
			{
				if (m_gamepads[i].first)
				{
					if (m_gamepads[i].second)
					{
						//Poll input
						m_gamepads[i].second->Poll();

						//If disconnected, discard implementation
						if (!m_gamepads[i].second->IsConnected())
						{
							DestroyImplementation(i);
						}
					}
					else
					{
						//Search for connected controller
						FindAndCreateImplementation(i);
					}
				}
			}
		}

		int GamepadManager::GetMaxControllers()
		{
			return m_maxControllersRequested;
		}

		Gamepad* GamepadManager::CreateGamepad()
		{
			for (int i = 0; i < s_maxControllersSupported; i++)
			{
				if (!m_gamepads[i].first)
				{
					Gamepad* gamepad = new Gamepad();
					m_gamepads[i].first = gamepad;
					return gamepad;
				}
			}

			debug::error << "Could not create gamepad, exceeded max count (" << s_maxControllersSupported << ")" << debug::end;
			return nullptr;
		}

		void GamepadManager::DestroyGamepad(Gamepad* gamepad)
		{
			for (int i = 0; i < s_maxControllersSupported; i++)
			{
				if (m_gamepads[i].first == gamepad)
				{
					if (m_gamepads[i].second)
					{
						DestroyImplementation(i);
						delete m_gamepads[i].first;
						m_gamepads[i].first = nullptr;
						return;
					}
				}
			}

			debug::Error("Gamepad not registered");
		}

		void GamepadManager::FindAndCreateImplementation(int index)
		{
			if (m_gamepads[index].first && !m_gamepads[index].second)
			{
				//Find a free, connected controller
				GamepadImpl* implementation = nullptr;

#if defined ION_INPUT_STEAM
				if (!implementation)
				{
					implementation = GamepadImplSteam::FindAvailableController(index);
				}
#endif

#if defined ION_INPUT_XINPUT
				if (!implementation)
				{
					implementation = GamepadImplXInput::FindAvailableController(index);
				}
#endif

#if defined ION_INPUT_SDL
				if (!implementation)
				{
					implementation = GamepadImplSDL::FindAvailableController(index);
				}
#endif

#if defined ION_INPUT_MAPLE
				if (!implementation)
				{
					implementation = GamepadImplMaple::FindAvailableController(index);
				}
#endif

#if defined ION_INPUT_SWITCH
				if (!implementation)
				{
					implementation = GamepadImplSwitch::FindAvailableController(index);
				}
#endif
				if (implementation)
				{
					m_gamepads[index].second = implementation;
					m_gamepads[index].first->SetImplementation(implementation);
				}
			}
		}

		void GamepadManager::DestroyImplementation(int index)
		{
			if (index != s_invalidIndex)
			{
				delete m_gamepads[index].second;
				m_gamepads[index].second = nullptr;
				m_gamepads[index].first->SetImplementation(nullptr);
			}
		}
	}
}