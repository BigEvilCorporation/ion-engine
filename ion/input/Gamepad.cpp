///////////////////////////////////////////////////
// File:		Gamepad.cpp
// Date:		2nd August 2011
// Authors:		Matt Phillips
// Description:	Gamepad input handler
///////////////////////////////////////////////////

#include "Gamepad.h"

#include <core/debug/Debug.h>

namespace ion
{
	namespace input
	{
		Gamepad::Gamepad()
		{
			m_implementation = nullptr;
			m_deadZone = 0.2f;
			m_outerZone = 0.9f;
		}

		Gamepad::~Gamepad()
		{

		}

		void Gamepad::SetImplementation(GamepadImpl* implementation)
		{
			m_implementation = implementation;
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

		GamepadType Gamepad::GetGamepadType() const
		{
			if (m_implementation)
			{
				return m_implementation->GetGamepadType();
			}
			else
			{
				return GamepadType::Generic;
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

		bool Gamepad::AnyButtonDown() const
		{
			for (int i = 0; i < (int)GamepadButtons::COUNT; i++)
			{
				if (CheckButton((GamepadButtons)i))
					return true;
			}

			return false;
		}

		bool Gamepad::AnyStickInput() const
		{
			Vector2 left = GetLeftStick();
			Vector2 right = GetRightStick();

			return left.GetLengthSq() != 0.0f || right.GetLengthSq() != 0.0f;
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

		void Gamepad::Poll()
		{
			if (m_implementation)
				m_implementation->Poll();
		}
	}
}
