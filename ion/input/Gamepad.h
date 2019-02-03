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

			Count
		};

		enum class GamepadButtons : int
		{
			DPAD_UP,
			DPAD_DOWN,
			DPAD_LEFT,
			DPAD_RIGHT,

			BUTTON_A,
			BUTTON_B,
			BUTTON_X,
			BUTTON_Y,

			START,
			SELECT,

			LEFT_SHOULDER,
			RIGHT_SHOULDER,

			LEFT_STICK_CLICK,
			RIGHT_STICK_CLICK,

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
			Gamepad();
			~Gamepad();

			//Update input/connection state
			void Update();
			
			//Test if connected
			bool IsConnected() const;

			ion::Vector2 GetLeftStick() const;
			ion::Vector2 GetRightStick() const;

			bool ButtonDown(GamepadButtons button) const;
			bool ButtonPressedThisFrame(GamepadButtons button) const;
			bool ButtonReleasedThisFrame(GamepadButtons button) const;

			void SetDeadZone(float deadZone);
			float GetDeadZone() const;

			void SetOuterZone(float outerZone);
			float GetOuterZone() const;

			static const int s_maxControllers = 4;

		private:
			void FindAndRegisterController();
			void UnregisterController();
			bool CheckButton(GamepadButtons button) const;
			bool CheckPrevButton(GamepadButtons button) const;

			int m_controllerIndex;
			float m_deadZone;
			float m_outerZone;

			static const int s_invalidIndex = -1;
			static bool s_registeredControllers[s_maxControllers];

			GamepadImpl* m_implementation;
		};

		class GamepadImpl
		{
		public:
			virtual ~GamepadImpl() {}
			virtual void Poll() = 0;
			virtual bool IsConnected() const = 0;
			virtual int ToPlatformButton(GamepadButtons button) const = 0;
			virtual ion::Vector2 GetLeftStick() const = 0;
			virtual ion::Vector2 GetRightStick() const = 0;
			virtual bool CheckButton(GamepadButtons button) const = 0;
			virtual bool CheckPrevButton(GamepadButtons button) const = 0;
		};
	}
}
