///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		StateManager.h
// Date:		16th February 2014
// Authors:		Matt Phillips
// Description:	Game state manager
///////////////////////////////////////////////////

#pragma once

#include <ion/core/cryptography/UUID.h>

#include <vector>
#include <map>
#include <string>

namespace ion
{
	namespace render
	{
		class Renderer;
		class Camera;
		class Viewport;
	}

	namespace io
	{
		class ResourceManager;
	}

	namespace input
	{
		class Keyboard;
		class Mouse;
		class Gamepad;
	}

	namespace gamekit
	{
		class StateManager;

		class State
		{
		public:
			State(const std::string& name, StateManager& stateManager, io::ResourceManager& resourceManager);
			virtual ~State();

			virtual void OnEnterState() = 0;
			virtual void OnLeaveState() = 0;
			virtual void OnPauseState() = 0;
			virtual void OnResumeState() = 0;

			virtual bool Update(float deltaTime, input::Keyboard* keyboard, input::Mouse* mouse, const std::vector<input::Gamepad*>& gamepads) = 0;
			virtual void Render(render::Renderer& renderer, const render::Camera& camera, render::Viewport& viewport) = 0;

		protected:
			std::string m_name;
			ion::UUID64 m_id;
			StateManager& m_stateManager;
			io::ResourceManager& m_resourceManager;
		};

		class StateManager
		{
		public:
			StateManager();
			~StateManager();

			void PushState(const std::string& name);
			void PushState(State& state);
			void SwapState(const std::string& name);
			void SwapState(State& state);
			void PopState();

			void DeleteStates();

			void Pause();
			void Resume();

			bool Update(float deltaTime, input::Keyboard* keyboard, input::Mouse* mouse, const std::vector<input::Gamepad*>& gamepads);
			void Render(render::Renderer& renderer, const render::Camera& camera, render::Viewport& viewport);

		protected:
			void AddState(State& state, const std::string& name);

		private:
			std::map<std::string, State*> m_states;
			std::vector<State*> m_stateStack;
			State* m_renderingState;

			friend class State;
		};
	}
}