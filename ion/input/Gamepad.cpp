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
		bool Gamepad::sRegisteredControllers[sMaxControllers] = { false };

		Gamepad::Gamepad()
		{
			mControllerIndex = FindAvailableController();
			mConnected = false;
			mDeadZone = 0.05f;
			mOuterZone = 0.9f;

#if defined ION_PLATFORM_WINDOWS
			memory::MemSet(&mInputState, 0, sizeof(XINPUT_STATE));
#elif defined ION_PLATFORM_DREAMCAST
			memory::MemSet(&m_mapleState, 0, sizeof(cont_state_t));
#endif
		}

		Gamepad::~Gamepad()
		{
			if(mControllerIndex != sInvalidIndex)
			{
				sRegisteredControllers[mControllerIndex] = false;
			}
		}

		int Gamepad::FindAvailableController()
		{
			//Find a free, connected controller
			int connectedControllerId = sInvalidIndex;

			for(int i = 0; i < sMaxControllers; i++)
			{
				if(!sRegisteredControllers[i])
				{
#if defined ION_PLATFORM_WINDOWS
					XINPUT_STATE state;
					int errorState = XInputGetState(i, &state);
					if(!errorState)
					{
						connectedControllerId = i;
						sRegisteredControllers[i] = true;
						break;
					}
#elif defined ION_PLATFORM_MACOSX
                    return 0;
#elif defined ION_PLATFORM_DREAMCAST
					if(i < maple_enum_count())
					{
						if(maple_device_t* device = maple_enum_type(i, MAPLE_FUNC_CONTROLLER))
						{
							if(cont_state_t* state = (cont_state_t*)maple_dev_status(device))
							{
								connectedControllerId = i;
								sRegisteredControllers[i] = true;
								break;
							}
						}
					}
#endif
				}
			}

			return connectedControllerId;
		}

		void Gamepad::Update()
		{
			if(mControllerIndex != sInvalidIndex)
			{
#if defined ION_PLATFORM_WINDOWS
				//Registered, update state
				int errorState = XInputGetState(mControllerIndex, &mInputState);

				if(!errorState)
				{
					//No error, pad is connected
					mConnected = true;
				}
				else
				{
					//Error, pad disconnected
					mConnected = false;

					//Make sure values aren't cached
					memory::MemSet(&mInputState, 0, sizeof(XINPUT_STATE));
				}
#elif defined ION_PLATFORM_DREAMCAST
				if(maple_device_t* device = maple_enum_type(mControllerIndex, MAPLE_FUNC_CONTROLLER))
				{
					if(cont_state_t* state = (cont_state_t*)maple_dev_status(device))
					{
						m_mapleState = *state;
					}
				}
				else
				{
					//Error, pad disconnected
					mConnected = false;

					//Make sure values aren't cached
					memory::MemSet(&m_mapleState, 0, sizeof(cont_state_t));
				}
#endif
			}
		}

		bool Gamepad::IsConnected() const
		{
			return mConnected;
		}

		bool Gamepad::FindConnectedGamepad()
		{
			mControllerIndex = FindAvailableController();
			return mControllerIndex != sInvalidIndex;
		}

		ion::Vector2 Gamepad::GetLeftStick() const
		{
#if defined ION_PLATFORM_DREAMCAST
			float lx = (float)m_mapleState.joyx;
			float ly = (float)m_mapleState.joyy;

			if(lx != 0.0f)
				lx /= 127;

			if(ly != 0.0f)
				ly /= 127;
#elif defined ION_PLATFORM_MACOSX
            float lx = 0.0f;
            float ly = 0.0f;
#elif defined ION_PLATFORM_WINDOWS
			float lx = (float)mInputState.Gamepad.sThumbLX;
			float ly = (float)mInputState.Gamepad.sThumbLY;

			if(lx != 0.0f)
				lx /= 32768;

			if(ly != 0.0f)
				ly /= 32768;
#endif

			if(lx > -mDeadZone && lx < mDeadZone)
				lx = 0.0f;

			if(ly > -mDeadZone && ly < mDeadZone)
				ly = 0.0f;

			return ion::Vector2(lx, ly);
		}

		ion::Vector2 Gamepad::GetRightStick() const
		{
#if defined ION_PLATFORM_DREAMCAST
			float rx = (float)m_mapleState.joy2x;
			float ry = (float)m_mapleState.joy2y;

			if(rx != 0.0f)
				rx /= 127;

			if(ry != 0.0f)
				ry /= 127;
#elif defined ION_PLATFORM_MACOSX
            float rx = 0.0f;
            float ry = 0.0f;
#elif defined ION_PLATFORM_WINDOWS
			float rx = (float)mInputState.Gamepad.sThumbRX;
			float ry = (float)mInputState.Gamepad.sThumbRY;

			if(rx != 0.0f)
				rx /= 32768;

			if(ry != 0.0f)
				ry /= 32768;
#endif

			if(rx > -mDeadZone && rx < mDeadZone)
				rx = 0.0f;

			if(ry > -mDeadZone && ry < mDeadZone)
				ry = 0.0f;

			return ion::Vector2(rx, ry);
		}

		bool Gamepad::ButtonDown(Buttons button) const
		{
#if defined ION_PLATFORM_DREAMCAST
			int mapleButton = ToPlatformButton(button);
			return (m_mapleState.buttons & mapleButton) != 0;
#elif defined ION_PLATFORM_MACOSX
            return false;
#else
			int xInputButton = ToPlatformButton(button);
			return (mInputState.Gamepad.wButtons & xInputButton) != 0;
#endif
		}

		int Gamepad::ToPlatformButton(Buttons button)
		{
			int platformButton = 0;

#if defined ION_PLATFORM_WINDOWS
			switch(button)
			{
				case DPAD_UP:			platformButton = XINPUT_GAMEPAD_DPAD_UP; break;
				case DPAD_DOWN:			platformButton = XINPUT_GAMEPAD_DPAD_DOWN; break;
				case DPAD_LEFT:			platformButton = XINPUT_GAMEPAD_DPAD_LEFT; break;
				case DPAD_RIGHT:		platformButton = XINPUT_GAMEPAD_DPAD_RIGHT; break;
				case BUTTON_A:			platformButton = XINPUT_GAMEPAD_A; break;
				case BUTTON_B:			platformButton = XINPUT_GAMEPAD_B; break;
				case BUTTON_X:			platformButton = XINPUT_GAMEPAD_X; break;
				case BUTTON_Y:			platformButton = XINPUT_GAMEPAD_Y; break;
				case START:				platformButton = XINPUT_GAMEPAD_START; break;
				case SELECT:			platformButton = XINPUT_GAMEPAD_BACK; break;
				case LEFT_SHOULDER:		platformButton = XINPUT_GAMEPAD_LEFT_SHOULDER; break;
				case RIGHT_SHOULDER:	platformButton = XINPUT_GAMEPAD_RIGHT_SHOULDER; break;
				case LEFT_STICK_CLICK:	platformButton = XINPUT_GAMEPAD_LEFT_THUMB; break;
				case RIGHT_STICK_CLICK:	platformButton = XINPUT_GAMEPAD_RIGHT_THUMB; break;
				default: break;
			}
#elif defined ION_PLATFORM_DREAMCAST
			switch(button)
			{
				case DPAD_UP:			platformButton = CONT_DPAD_UP; break;
				case DPAD_DOWN:			platformButton = CONT_DPAD_DOWN; break;
				case DPAD_LEFT:			platformButton = CONT_DPAD_LEFT; break;
				case DPAD_RIGHT:		platformButton = CONT_DPAD_RIGHT; break;
				case BUTTON_A:			platformButton = CONT_A; break;
				case BUTTON_B:			platformButton = CONT_B; break;
				case BUTTON_X:			platformButton = CONT_X; break;
				case BUTTON_Y:			platformButton = CONT_Y; break;
				case START:				platformButton = CONT_START; break;
				case SELECT:			platformButton = 0; break;
				case LEFT_SHOULDER:		platformButton = CONT_C; break;
				case RIGHT_SHOULDER:	platformButton = CONT_D; break;
				case LEFT_STICK_CLICK:	platformButton = 0; break;
				case RIGHT_STICK_CLICK:	platformButton = 0; break;
				default: break;
			}
#endif

			return platformButton;
		}

		void Gamepad::SetDeadZone(float deadZone)
		{
			mDeadZone = deadZone;
		}

		float Gamepad::GetDeadZone() const
		{
			return mDeadZone;
		}

		void Gamepad::SetOuterZone(float outerZone)
		{
			mOuterZone = outerZone;
		}

		float Gamepad::GetOuterZone() const
		{
			return mOuterZone;
		}
	}
}
