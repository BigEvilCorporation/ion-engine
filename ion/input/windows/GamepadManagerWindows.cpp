///////////////////////////////////////////////////
// File:		GamepadManagerWindows.cpp
// Date:		1st May 2021
// Authors:		Matt Phillips
// Description:	Gamepad manager for Windows
///////////////////////////////////////////////////

#include "GamepadManager.h"

namespace ion
{
	namespace input
	{
		GamepadManager* GamepadManager::Create(int maxControllers)
		{
			return new GamepadManager(maxControllers);
		}
	}
}