///////////////////////////////////////////////////
// File:		Gamepad.cpp
// Date:		2nd August 2011
// Authors:		Matt Phillips
// Description:	Gamepad input handler
///////////////////////////////////////////////////

#include "Gamepad.h"

#include <core/debug/Debug.h>

#if defined ION_INPUT_XINPUT
#include "xinput/GamepadXInput.h"
#endif

#if defined ION_INPUT_SDL
#include "sdl/GamepadSDL.h"
#endif

#if defined ION_INPUT_MAPLE
#include "dreamcast/GamepadMaple.h"
#endif

#if defined ION_INPUT_STEAM
#include "steam/GamepadSteam.h"
#endif

namespace ion
{
	namespace input
	{
		bool Gamepad::s_registeredControllers[s_maxControllers] = { false };

		Gamepad::Gamepad()
		{
			m_implementation = nullptr;
			m_deadZone = 0.2f;
			m_outerZone = 0.9f;
			
			FindAndRegisterController();
		}

		Gamepad::~Gamepad()
		{
			UnregisterController();
		}

		void Gamepad::FindAndRegisterController()
		{
			//Find a free, connected controller
			m_controllerIndex = s_invalidIndex;

			for(int i = 0; i < s_maxControllers && !m_implementation; i++)
			{
				if(!s_registeredControllers[i])
				{
#if defined ION_INPUT_STEAM
					if (!m_implementation)
					{
						m_implementation = GamepadSteam::FindAvailableController(i);
					}
#endif

#if defined ION_INPUT_XINPUT
					if (!m_implementation)
					{
						m_implementation = GamepadXInput::FindAvailableController(i);
					}
#endif

#if defined ION_INPUT_SDL
					if (!m_implementation)
					{
						m_implementation = GamepadSDL::FindAvailableController(i);
					}
#endif

#if defined ION_INPUT_MAPLE
					if (!m_implementation)
					{
						m_implementation = GamepadMaple::FindAvailableController(i);
					}
#endif

					if (m_implementation)
					{
						m_controllerIndex = i;
						s_registeredControllers[i] = true;
					}
				}
			}
		}

		void Gamepad::UnregisterController()
		{
			if (m_controllerIndex != s_invalidIndex)
			{
				delete m_implementation;
				m_implementation = nullptr;
				s_registeredControllers[m_controllerIndex] = false;
				m_controllerIndex = s_invalidIndex;
			}
		}

		void Gamepad::Update()
		{
			if(m_implementation)
			{
				//Poll input
				m_implementation->Poll();

				//If disconnected, discard implementation
				if (!m_implementation->IsConnected())
				{
					UnregisterController();
				}
			}
			else
			{
				//Search for connected controller
				FindAndRegisterController();
			}
		}

		bool Gamepad::IsConnected() const
		{
			if (m_implementation)
			{
				return m_implementation->IsConnected();
			}
			else
			{
				return false;
			}
		}

		ion::Vector2 Gamepad::GetLeftStick() const
		{
			ion::Vector2 value;

			if (m_implementation)
			{
				value = m_implementation->GetLeftStick();
			}

			if(value.x > -m_deadZone && value.x < m_deadZone)
				value.x = 0.0f;

			if(value.y > -m_deadZone && value.y < m_deadZone)
				value.y = 0.0f;

			return value;
		}

		ion::Vector2 Gamepad::GetRightStick() const
		{
			ion::Vector2 value;

			if (m_implementation)
			{
				value = m_implementation->GetRightStick();
			}

			if(value.x > -m_deadZone && value.x < m_deadZone)
				value.x = 0.0f;

			if(value.y > -m_deadZone && value.y < m_deadZone)
				value.y = 0.0f;

			return value;
		}

		bool Gamepad::CheckButton(GamepadButtons button) const
		{
			if (m_implementation)
			{
				return m_implementation->CheckButton(button);
			}

			return false;
		}

		bool Gamepad::CheckPrevButton(GamepadButtons button) const
		{
			if (m_implementation)
			{
				return m_implementation->CheckPrevButton(button);
			}

			return false;
		}

		bool Gamepad::ButtonDown(GamepadButtons button) const
		{
			return CheckButton(button);
		}

		bool Gamepad::ButtonPressedThisFrame(GamepadButtons button) const
		{
			return CheckButton(button) && !CheckPrevButton(button);
		}

		bool Gamepad::ButtonReleasedThisFrame(GamepadButtons button) const
		{
			return !CheckButton(button) && CheckPrevButton(button);
		}

		void Gamepad::SetDeadZone(float deadZone)
		{
			m_deadZone = deadZone;
		}

		float Gamepad::GetDeadZone() const
		{
			return m_deadZone;
		}

		void Gamepad::SetOuterZone(float outerZone)
		{
			m_outerZone = outerZone;
		}

		float Gamepad::GetOuterZone() const
		{
			return m_outerZone;
		}
	}
}
