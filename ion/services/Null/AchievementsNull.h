///////////////////////////////////////////////////
// File:		AchievementsNull.h
// Date:		30th July 2018
// Authors:		Matt Phillips
// Description:	Null achievements
///////////////////////////////////////////////////

#pragma once

#include <ion/services/Achievements.h>

namespace ion
{
	namespace services
	{
		class AchievementsNull : public Achievements
		{
		public:
			AchievementsNull() {}
			virtual ~AchievementsNull() {}

			virtual void RegisterAchievement(const Achievement& achievement) {}
			virtual void AwardAchievement(User& user, const Achievement& achievement) {}

			virtual void Update() {}
		};
	}
}