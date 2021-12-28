///////////////////////////////////////////////////
// File:		GamepadManager.h
// Date:		1st May 2021
// Authors:		Matt Phillips
// Description:	Gamepad manager
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/memory/Memory.h"
#include "maths/Vector.h"

#include "Gamepad.h"

#include <tuple>

namespace ion
{
	namespace input
	{
		class GamepadManager
		{
		public:
			static GamepadManager* Create(int maxControllers);

			GamepadManager(int maxControllers);
			virtual ~GamepadManager();

			virtual void Update();

			int GetMaxControllers();

			Gamepad* CreateGamepad();
			void DestroyGamepad(Gamepad* gamepad);

		protected:
			static const int s_maxControllersSupported = 4;
			static const int s_invalidIndex = -1;

			void FindAndCreateImplementation(int index);
			void DestroyImplementation(int index);

			int m_maxControllersRequested;
			std::pair<Gamepad*, GamepadImpl*> m_gamepads[s_maxControllersSupported];
		};
	}
}