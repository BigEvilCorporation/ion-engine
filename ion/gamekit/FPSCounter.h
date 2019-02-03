///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		FPSCounter.h
// Date:		24th September 2018
// Authors:		Matt Phillips
// Description:	Basic FPS counter
///////////////////////////////////////////////////

#pragma once

#include <ion/core/time/Time.h>

namespace ion
{
	namespace gamekit
	{
		class FPSCounter
		{
		public:
			FPSCounter()
			{
				m_frameCount = 0;
				m_startTicks = 0;
				m_lastFPS = 0.0f;
			}

			void Update()
			{
				if (m_frameCount++ % 10 == 0)
				{
					//Get 10-frame end time and diff
					u64 endTicks = ion::time::GetSystemTicks();
					u64 diffTicks = endTicks - m_startTicks;

					//Calc frame time and frames per second
					float frameTime = (float)ion::time::TicksToSeconds(diffTicks) / 10.0f;
					float framesPerSecond = 1.0f / frameTime;
					m_lastFPS = framesPerSecond;

					//Reset timer
					m_startTicks = ion::time::GetSystemTicks();
				}
			}

			float GetLastFPS() const { return m_lastFPS; }

		private:
			u64 m_frameCount;
			u64 m_startTicks;
			float m_lastFPS;
		};
	}
}