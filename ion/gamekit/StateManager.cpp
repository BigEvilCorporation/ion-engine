///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		StateManager.cpp
// Date:		16th February 2014
// Authors:		Matt Phillips
// Description:	Game state manager
///////////////////////////////////////////////////

#include "gamekit/StateManager.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace gamekit
	{
		State::State(const std::string& name, StateManager& stateManager, io::ResourceManager& resourceManager)
			: m_name(name)
			, m_stateManager(stateManager)
			, m_resourceManager(resourceManager)
		{
			m_id = ion::GenerateUUID64();
			m_stateManager.AddState(*this, m_name);
		}

		State::~State()
		{
		}

		StateManager::StateManager()
		{
			m_renderingState = NULL;
		}

		StateManager::~StateManager()
		{
		}

		void StateManager::DeleteStates()
		{
			ion::debug::Assert(m_stateStack.empty(), "StateManager::DeleteStates() - Cannot delete states if the state stack is not empty");

			for (std::map<std::string, State*>::iterator it = m_states.begin(), end = m_states.end(); it != end; ++it)
			{
				delete it->second;
			}

			m_states.clear();
		}

		void StateManager::AddState(State& state, const std::string& name)
		{
			m_states.insert(std::make_pair(name, &state));
		}

		void StateManager::PushState(const std::string& name)
		{
			std::map<std::string, State*>::iterator it = m_states.find(name);
			ion::debug::Assert(it != m_states.end(), "StateManager::PushState() - State not found");
			PushState(*it->second);
		}

		void StateManager::PushState(State& state)
		{
			//If existing state, pause it first
			if(m_stateStack.size() > 0)
				m_stateStack.back()->OnPauseState();

			//Enter state callback
			state.OnEnterState();

			//Push new state
			m_stateStack.push_back(&state);
		}

		void StateManager::SwapState(const std::string& name)
		{
			std::map<std::string, State*>::iterator it = m_states.find(name);
			ion::debug::Assert(it != m_states.end(), "StateManager::SwapState() - State not found");
			SwapState(*it->second);
		}

		void StateManager::SwapState(State& state)
		{
			//If existing state, pop it
			if(m_stateStack.size() > 0)
				PopState();

			//Enter state callback
			state.OnEnterState();

			//Push new state
			m_stateStack.push_back(&state);
		}

		void StateManager::PopState()
		{
			debug::Assert(m_stateStack.size() > 0, "StateManager::PopState() - State stack is empty");

			//Remove rendering state
			m_renderingState = nullptr;

			//Leave state callback
			m_stateStack.back()->OnLeaveState();

			//Pop state
			m_stateStack.pop_back();

			//If prev state, resume it
			if (m_stateStack.size() > 0)
				m_stateStack.back()->OnResumeState();
		}

		void StateManager::Pause()
		{
			if(m_stateStack.size() > 0)
				m_stateStack.back()->OnPauseState();
		}

		void StateManager::Resume()
		{
			if(m_stateStack.size() > 0)
				m_stateStack.back()->OnResumeState();
		}

		bool StateManager::Update(float deltaTime, input::Keyboard* keyboard, input::Mouse* mouse, const std::vector<input::Gamepad*>& gamepads)
		{
			if (m_stateStack.size() > 0)
			{
				m_renderingState = m_stateStack.back();
				return m_stateStack.back()->Update(deltaTime, keyboard, mouse, gamepads);
			}
			else
			{
				m_renderingState = NULL;
				return true;
			}
		}

		void StateManager::Render(render::Renderer& renderer, const render::Camera& camera, render::Viewport& viewport)
		{
			if(m_renderingState)
				m_renderingState->Render(renderer, camera, viewport);
		}
	}
}