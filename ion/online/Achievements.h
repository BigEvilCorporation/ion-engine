///////////////////////////////////////////////////
// File:		Achievements.h
// Date:		30th July 2018
// Authors:		Matt Phillips
// Description:	Achievements/Trophies interface
///////////////////////////////////////////////////

#pragma once

#include <ion/core/Types.h>

#include <string>

namespace ion
{
	namespace online
	{
		struct Achievement
		{
			u64 id;
			std::string idStr;
			std::string name;
			std::string description;
		};

		class Achievements
		{
		public:
			static Achievements* Create();

			virtual ~Achievements() {}

			virtual void RegisterAchievement(const Achievement& achievement) = 0;
			virtual void AwardAchievement(const Achievement& achievement) = 0;

			virtual void Update() = 0;

		protected:
			Achievements() {}
		};
	}
}