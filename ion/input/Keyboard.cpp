///////////////////////////////////////////////////
// File:		Keyboard.cpp
// Date:		1st August 2011
// Authors:		Matt Phillips
// Description:	SDL keyboard wrapper
///////////////////////////////////////////////////

#include "Keyboard.h"
#include "Keytable.h"

#include "core/memory/Memory.h"
#include "core/debug/Debug.h"

#include <algorithm>

namespace ion
{
	namespace input
	{
		Keyboard::Keyboard()
		{
#if defined ION_PLATFORM_WINDOWS
			mKeyboardDevice = NULL;

			if(!g_DirectInputInterface)
			{
				HRESULT hResult = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_DirectInputInterface, NULL);
				if(hResult != 0)
				{
					debug::Error("Keyboard::Keyboard() - Unable to initialise DirectInput interface");
				}
			}

			if(g_DirectInputInterface)
			{
				HRESULT hResult = g_DirectInputInterface->CreateDevice(GUID_SysKeyboard, &mKeyboardDevice, NULL);
				if(hResult != 0)
				{
					debug::Error("Keyboard::Keyboard() - Unable to initialise DirectInput keyboard device");
				}
			}

			if(mKeyboardDevice)
			{
				mKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
				mKeyboardDevice->Acquire();
			}

			for(int i = 0; i < Keycode::COUNT; i++)
			{
				mCurrKeyStates[i] = false;
				mPrevKeyStates[i] = false;
			}
#endif
		}

		Keyboard::~Keyboard()
		{
#if defined ION_PLATFORM_WINDOWS
			if(mKeyboardDevice)
			{
				mKeyboardDevice->Unacquire();
				mKeyboardDevice->Release();
			}
#endif
		}

		void Keyboard::RegisterHandler(KeyboardHandler& handler)
		{
			m_handlers.push_back(&handler);
		}

		void Keyboard::UnregisterHandler(KeyboardHandler& handler)
		{
			m_handlers.erase(std::find(m_handlers.begin(), m_handlers.end(), &handler));
		}
		
		bool Keyboard::KeyDown(Keycode key) const
		{
			debug::Assert(key >= 0 && key < Keycode::COUNT, "Keyboard::KeyDown() - Key out of range");
			return (mCurrKeyStates[key] != 0);
		}
		
		bool Keyboard::KeyPressedThisFrame(Keycode key) const
		{
			debug::Assert(key >= 0 && key < Keycode::COUNT, "Keyboard::KeyPressedThisFrame() - Key out of range");
			return (mCurrKeyStates[key] != 0) && (mPrevKeyStates[key] == 0);
		}

		void Keyboard::Update()
		{
			//Store prev key states
			memory::MemCopy(mPrevKeyStates, mCurrKeyStates, sizeof(mCurrKeyStates));

#if defined ION_PLATFORM_WINDOWS
			if(mKeyboardDevice)
			{
				//Read key states
				u8 diKeyStates[256] = { 0 };
				HRESULT hResult = mKeyboardDevice->GetDeviceState(sizeof(diKeyStates), diKeyStates);

				if(hResult != DI_OK)
				{
					//Device lost, attempt to reacquire it
					hResult = mKeyboardDevice->Acquire();

					if(hResult == DI_OK)
					{
						//Success, read key states again (if this fails, abandon it until next Update())
						mKeyboardDevice->GetDeviceState(sizeof(diKeyStates), diKeyStates);
					}
				}

				for(int i = 0; i < Keycode::COUNT; i++)
				{
					mCurrKeyStates[i] = diKeyStates[KeycodeTable[i]];
				}
			}
#elif defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_LINUX
			int numKeys = 0;
			const u8* keyStates = SDL_GetKeyboardState(&numKeys);

			for(int i = 0; i < Keycode::COUNT; i++)
			{
				mCurrKeyStates[i] = keyStates[KeycodeTable[i]];
			}
#endif

			//Determine changea and fire events
			for(int i = 0; i < Keycode::COUNT; i++)
			{
				if(mCurrKeyStates[i] != mPrevKeyStates[i])
				{
					for(int j = 0; j < m_handlers.size(); j++)
					{
						if(mCurrKeyStates[i])
							m_handlers[j]->OnKeyDown((ion::input::Keycode)i);
						else
							m_handlers[j]->OnKeyUp((ion::input::Keycode)i);
					}
				}
			}
		}

		void Keyboard::SetCooperativeWindow(CoopLevel coopLevel)
		{
#if defined ION_PLATFORM_WINDOWS
			int windowsCoopLevel = 0;

			switch(coopLevel)
			{
			case Background:
				windowsCoopLevel = DISCL_BACKGROUND | DISCL_NONEXCLUSIVE;
				break;

			case Foreground:
				windowsCoopLevel = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
				break;

			case Exclusive:
				windowsCoopLevel = DISCL_EXCLUSIVE | DISCL_FOREGROUND;
				break;
			}

			mKeyboardDevice->SetCooperativeLevel(GetActiveWindow(), windowsCoopLevel);
#endif
		}
	}
}
