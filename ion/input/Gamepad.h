///////////////////////////////////////////////////
// File:		Gamepad.h
// Date:		2nd August 2011
// Authors:		Matt Phillips
// Description:	Gamepad input handler
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/memory/Memory.h"
#include "maths/Vector.h"

namespace ion
{
	namespace input
	{
		enum class GamepadType
		{
			Xbox360,
			Generic = Xbox360,
			DualShock3,
			SEGAMegaDrive,
			SwitchPro,
			JoyconDual,
			JoyconHandheld,
			JoyconLeft,
			JoyconRight,

			Count
		};

		enum class GamepadButtons : int
		{
			DPAD_UP				= (1 << 0),
			DPAD_DOWN			= (1 << 1),
			DPAD_LEFT			= (1 << 2),
			DPAD_RIGHT			= (1 << 3),

			BUTTON_A			= (1 << 4),
			BUTTON_B			= (1 << 5),
			BUTTON_X			= (1 << 6),
			BUTTON_Y			= (1 << 7),

			START				= (1 << 8),
			SELECT,

			LEFT_BUMPER			= (1 << 9),
			RIGHT_BUMPER		= (1 << 10),
			LEFT_TRIGGER		= (1 << 11),
			RIGHT_TRIGGER		= (1 << 12),

			LEFT_STICK_CLICK	= (1 << 13),
			RIGHT_STICK_CLICK	= (1 << 14),

			COUNT
		};

		enum class GamepadSticks : int
		{
			LEFT,
			RIGHT,

			COUNT
		};

		static const char* GamepadButtonNames[(int)GamepadType::Count][(int)GamepadButtons::COUNT] =
		{
			//Xbox 360
			{
				"DPad Up",
				"DPad Down",
				"DPad Left",
				"DPad Right",
				"A",
				"B",
				"X",
				"Y",
				"Start",
				"Back",
				"Left Shoulder",
				"Right Shoulder",
				"Left Stick Click",
				"Right Stick Click"
			},

			//Dual Shock 3
			{
				"DPad Up",
				"DPad Down",
				"DPad Left",
				"DPad Right",
				"Cross",
				"Circle",
				"Square",
				"Triangle",
				"Start",
				"Select",
				"L1",
				"R1",
				"L2",
				"R2"
			},

			//SEGA Mega Drive
			{
				"DPad Up",
				"DPad Down",
				"DPad Left",
				"DPad Right",
				"A",
				"B",
				"C",
				"[Unused]",
				"Start",
				"[Unused]",
				"[Unused]",
				"[Unused]",
				"[Unused]",
				"[Unused]"
			},
		};

		class GamepadImpl;

		class Gamepad
		{
		public:
			//Test if connected
			bool IsConnected() const;

			GamepadType GetGamepadType() const;

			ion::Vector2 GetLeftStick() const;
			ion::Vector2 GetRightStick() const;

			bool ButtonDown(GamepadButtons button) const;
			bool ButtonPressedThisFrame(GamepadButtons button) const;
			bool ButtonReleasedThisFrame(GamepadButtons button) const;
			bool AnyButtonDown() const;
			bool AnyStickInput() const;

			void SetDeadZone(float deadZone);
			float GetDeadZone() const;

			void SetOuterZone(float outerZone);
			float GetOuterZone() const;

			//Force poll, if immediate state needed
			void Poll();

		protected:
			friend class GamepadManager;
			friend class GamepadManagerSwitch;
			Gamepad();
			~Gamepad();
			void SetImplementation(GamepadImpl* implementation);

		private:
			bool CheckButton(GamepadButtons button) const;
			bool CheckPrevButton(GamepadButtons button) const;

			float m_deadZone;
			float m_outerZone;

			GamepadImpl* m_implementation;
		};

		class GamepadImpl
		{
		public:
			virtual ~GamepadImpl() {}
			virtual void Poll() = 0;
			virtual bool IsConnected() const = 0;
			virtual GamepadType GetGamepadType() const = 0;
			virtual int ToPlatformButton(GamepadButtons button) const = 0;
			virtual ion::Vector2 GetLeftStick() const = 0;
			virtual ion::Vector2 GetRightStick() const = 0;
			virtual bool CheckButton(GamepadButtons button) const = 0;
			virtual bool CheckPrevButton(GamepadButtons button) const = 0;
		};
	}
}
