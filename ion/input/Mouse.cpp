///////////////////////////////////////////////////
// File:		Mouse.cpp
// Date:		3rd August 2011
// Authors:		Matt Phillips
// Description:	Mouse input class
///////////////////////////////////////////////////

#include "Mouse.h"
#include "core/memory/Memory.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace input
	{
		Mouse::Mouse()
		{
			mAbsX = 0;
			mAbsY = 0;
			mWheelAbs = 0;

#if defined ION_PLATFORM_WINDOWS
			mMouseDevice = NULL;
			memory::MemSet(&mMouseState, 0, sizeof(DIMOUSESTATE));

			if(!g_DirectInputInterface)
			{
				HRESULT hResult = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_DirectInputInterface, NULL);
				if(hResult != 0)
				{
					debug::Error("Mouse::Mouse() - Unable to initialise DirectInput interface");
				}
			}

			if(g_DirectInputInterface)
			{
				HRESULT hResult = g_DirectInputInterface->CreateDevice(GUID_SysMouse, &mMouseDevice, NULL);
				if(hResult != 0)
				{
					debug::Error("Mouse::Mouse() - Unable to initialise DirectInput mouse device");
				}
			}

			if(mMouseDevice)
			{
				mMouseDevice->SetDataFormat(&c_dfDIMouse2);
				mMouseDevice->Acquire();
			}
#endif
		}

		Mouse::~Mouse()
		{
#if defined ION_PLATFORM_WINDOWS
			if(mMouseDevice)
			{
				mMouseDevice->Unacquire();
				mMouseDevice->Release();
			}
#endif
		}

		void Mouse::Update()
		{
#if defined ION_PLATFORM_WINDOWS
			if(mMouseDevice)
			{
				//Get mouse state
				HRESULT hResult = mMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mMouseState);

				if(hResult == DI_OK)
				{
					POINT absPos;
					GetCursorPos(&absPos);
					ScreenToClient(GetActiveWindow(), &absPos);
					mAbsX = absPos.x;
					mAbsY = absPos.y;
					mWheelAbs += mMouseState.lZ;
				}
				else
				{
					//Device lost, attempt to reaqcuire
					hResult = mMouseDevice->Acquire();

					if(hResult == DI_OK)
					{
						//Success, read data again (if this fails, abandon it until next Update())
						mMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mMouseState);
					}
				}
			}
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
			m_sdlMouseButtons = SDL_GetMouseState(&mAbsX, &mAbsY);
			SDL_GetRelativeMouseState(&m_sdlMouseRelX, &m_sdlMouseRelX);
#endif
		}

		bool Mouse::ButtonDown(Buttons button) const
		{
#if defined ION_PLATFORM_DREAMCAST
			return false;
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
            switch(button)
			{
				case LB:
					return (m_sdlMouseButtons & SDL_BUTTON_LEFT) != 0;
				case RB:
					return (m_sdlMouseButtons & SDL_BUTTON_RIGHT) != 0;
				case MB:
					return (m_sdlMouseButtons & SDL_BUTTON_MIDDLE) != 0;
			}
#elif defined ION_PLATFORM_WINDOWS
			return (mMouseState.rgbButtons[button] & 0x80) != 0;
#elif defined ION_PLATFORM_SWITCH
			return false;
#endif
		}

		bool Mouse::AnyButtonDown() const
		{
			for (int i = 0; i < Buttons::Count; i++)
			{
				if (ButtonDown((Buttons)i))
					return true;
			}

			return false;
		}

		s32 Mouse::GetDeltaX() const
		{
#if defined ION_PLATFORM_DREAMCAST
			return 0;
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
			return m_sdlMouseRelX;
#elif defined ION_PLATFORM_WINDOWS
			return mMouseState.lX;
#elif defined ION_PLATFORM_SWITCH
			return 0;
#endif
		}

		s32 Mouse::GetDeltaY() const
		{
#if defined ION_PLATFORM_DREAMCAST
			return 0;
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
            return m_sdlMouseRelY;
#elif defined ION_PLATFORM_WINDOWS
			return mMouseState.lY;
#elif defined ION_PLATFORM_SWITCH
			return 0;
#endif
		}

		s32 Mouse::GetAbsoluteX() const
		{
			return mAbsX;
		}

		s32 Mouse::GetAbsoluteY() const
		{
			return mAbsY;
		}

		s32 Mouse::GetWheelDelta() const
		{
#if defined ION_PLATFORM_DREAMCAST
			return 0;
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
            return 0;
#elif defined ION_PLATFORM_WINDOWS
			return mMouseState.lZ;
#elif defined ION_PLATFORM_SWITCH
			return 0;
#endif
		}

		s32 Mouse::GetWheelAbsolute() const
		{
			return mWheelAbs;
		}

		void Mouse::ShowCursor(bool enabled)
		{
#if defined ION_PLATFORM_WINDOWS
			::ShowCursor(enabled);
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
#endif
		}

		void Mouse::SetCooperativeWindow(CoopLevel coopLevel)
		{
#if defined ION_PLATFORM_WINDOWS
			int windowsCoopLevel = 0;

			switch(coopLevel)
			{
			case CoopLevel::Background:
				windowsCoopLevel = DISCL_BACKGROUND | DISCL_NONEXCLUSIVE;
				break;

			case CoopLevel::Foreground:
				windowsCoopLevel = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
				break;

			case CoopLevel::Exclusive:
				windowsCoopLevel = DISCL_EXCLUSIVE | DISCL_FOREGROUND;
				break;
			}

			mMouseDevice->Unacquire();
			mMouseDevice->SetCooperativeLevel(GetActiveWindow(), windowsCoopLevel);
			mMouseDevice->Acquire();
#endif
		}
	}
}
