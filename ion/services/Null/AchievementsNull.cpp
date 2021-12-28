///////////////////////////////////////////////////
// File:		AchievementsNull.h
// Date:		30th July 2018
// Authors:		Matt Phillips
// Description:	Null achievements
///////////////////////////////////////////////////

#include "AchievementsNull.h"

namespace ion
{
	namespace services
	{
		Achievements* Achievements::Create()
		{
			return new AchievementsNull();
		}
	}
}