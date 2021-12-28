///////////////////////////////////////////////////
// (c) 2019 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
//
// File:		Engine.h
// Date:		15th September 2019
// Authors:		Matt Phillips
// Description:	Main game engine include and setup
///////////////////////////////////////////////////

#include <ion/core/Types.h>
#include <ion/core/Platform.h>
#include <ion/core/thread/Thread.h>
#include <ion/core/io/FileSystem.h>
#include <ion/resource/ResourceManager.h>
#include <ion/input/Keyboard.h>
#include <ion/input/Mouse.h>
#include <ion/input/Gamepad.h>
#include <ion/input/GamepadManager.h>
#include <ion/renderer/Window.h>
#include <ion/renderer/Renderer.h>
#include <ion/renderer/Viewport.h>
#include <ion/renderer/Camera.h>
#include <ion/audio/Engine.h>
#include <ion/services/Platform.h>
#include <ion/services/Achievements.h>
#include <ion/services/SaveManager.h>
#include <ion/services/UserManager.h>

#include <string>
#include <vector>

namespace ion
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		struct Config
		{
			struct Io
			{
				ion::io::FileSystem* fileSystem;
				ion::io::ResourceManager* resourceManager;
			} io;

			struct
			{
				bool supportKeyboard = true;
				bool supportMouse = true;
				int maxSupportedControllers = 4;
			} input;

			struct
			{
				bool createWindow = true;
				std::string title;
				u32 clientAreaWidth = 1024;
				u32 clientAreaHeight = 768;
				bool fullscreen = false;
			} window;

			struct
			{
				bool createRenderer = true;
				bool createCamera = true;
				ion::render::Viewport::PerspectiveMode perspectiveMode = ion::render::Viewport::PerspectiveMode::Ortho2DAbsolute;
			} render;

			struct
			{
				bool supportAudio = true;
			} audio;

			struct
			{
				bool supportPlatformServices = true;
				std::string appId = "";
				std::string appEncryptionKey = "";
			} services;
		};

		void Initialise(const Config& config);
		void Shutdown();
		bool Update(float deltaTime);
		void BeginRenderFrame();
		void EndRenderFrame();

		struct Io
		{
			ion::io::FileSystem* fileSystem;
			ion::io::ResourceManager* resourceManager;
		} io;

		struct
		{
			ion::input::Keyboard* keyboard;
			ion::input::Mouse* mouse;
			ion::input::GamepadManager* gamepadManager;
			std::vector<ion::input::Gamepad*> gamepads;
		} input;

		struct
		{
			ion::render::Window* window;
			ion::render::Renderer* renderer;
			ion::render::Viewport* viewport;
			ion::render::Camera* camera;
		} render;
		
		struct
		{
			ion::audio::Engine* engine;
			ion::thread::Thread* thread;
		} audio;

#if defined ION_SERVICES
		struct
		{
			ion::services::Platform* platform;
			ion::services::UserManager* userManager;
			ion::services::SaveManager* saveManager;
			ion::services::Achievements* achievements;
		} services;
#endif

	private:
		void SetDefaultResourceDirectories();
	};

	extern Engine engine;
}