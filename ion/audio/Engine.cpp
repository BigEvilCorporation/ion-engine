#include "Engine.h"
#include "Voice.h"

#include <core/utils/STL.h>

namespace ion
{
	namespace audio
	{
		void Engine::AddVoice(Voice* voice)
		{
			m_voiceListCritSec.Begin();
			m_voices.push_back(voice);
			m_voiceListCritSec.End();
		}

		void Engine::RemoveVoice(Voice* voice)
		{
			m_voiceListCritSec.Begin();
			ion::utils::stl::FindAndRemove(m_voices, voice);
			m_voiceListCritSec.End();
		}

		void Engine::Update(float deltaTime)
		{
			m_voiceListCritSec.Begin();

			for (int i = 0; i < m_voices.size(); i++)
			{
				m_voices[i]->Update(deltaTime);
			}

			m_voiceListCritSec.End();
		}
	}
}