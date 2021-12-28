///////////////////////////////////////////////////
// (c) 2019 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
//
// File:		Engine.h
// Date:		15th September 2019
// Authors:		Matt Phillips
// Description:	Main game engine include and setup
///////////////////////////////////////////////////

#include "Engine.h"

#include <ion/core/debug/CrashHandler.h>
#include <ion/renderer/Material.h>
#include <ion/renderer/Texture.h>

#if defined ION_RENDERER_SHADER
#include <ion/renderer/Shader.h>
#endif

#include <ion/audio/Engine.h>
#include <ion/core/time/Time.h>

#define SAFE_DELETE(a) if(a) delete a; a = nullptr;

namespace ion
{
	class AudioThread : public ion::thread::Thread
	{
	public:
		AudioThread(ion::audio::Engine* engine)
			: ion::thread::Thread("ion::audio")
			, m_engine(engine)
		{
			
		}

		virtual ~AudioThread()
		{
			m_running = false;
		}

	protected:
		virtual void Entry()
		{
			m_running = true;
			float deltaTime = 0.0f;

			while (m_running)
			{
				u64 startTicks = ion::time::GetSystemTicks();
				m_engine->WaitNextUpdateEvent();
				m_engine->Update(deltaTime);
				u64 endTicks = ion::time::GetSystemTicks();
				deltaTime = (float)ion::time::TicksToSeconds(endTicks - startTicks);
				Yield();
			}
		}

	private:
		ion::audio::Engine* m_engine;
		bool m_running;
	};

	Engine::Engine()
	{
		io.fileSystem = nullptr;
		io.resourceManager = nullptr;
		input.keyboard = nullptr;
		input.mouse = nullptr;
		render.window = nullptr;
		render.renderer = nullptr;
		render.viewport = nullptr;
		render.camera = nullptr;
		audio.engine = nullptr;
		audio.thread = nullptr;
#if defined ION_SERVICES
		services.achievements = nullptr;
		services.platform = nullptr;
		services.saveManager = nullptr;
		services.userManager = nullptr;
#endif
	}

	Engine::~Engine()
	{
		//Exit fullscreen mode
		render.window->SetFullscreen(false, 0);

		//Tear down engine in reverse order
#if defined ION_SERVICES
		SAFE_DELETE(services.achievements);
		SAFE_DELETE(services.saveManager);
		SAFE_DELETE(services.userManager);
		SAFE_DELETE(services.platform);
#endif
		SAFE_DELETE(audio.thread);
		SAFE_DELETE(audio.engine);
		SAFE_DELETE(render.camera);
		SAFE_DELETE(render.viewport);
		SAFE_DELETE(render.renderer);
		SAFE_DELETE(render.window);

		SAFE_DELETE(input.gamepadManager);
		SAFE_DELETE(input.mouse);
		SAFE_DELETE(input.keyboard);
		SAFE_DELETE(io.resourceManager);
		SAFE_DELETE(io.fileSystem);
	}

	void Engine::Initialise(const Config& config)
	{
		//Initialise platform
		ion::platform::Initialise();

		//Init crash handler/core dump
		ion::debug::InstallDefaultCrashHandler();

		//Create all engine systems
		io.fileSystem = new ion::io::FileSystem();
		io.resourceManager = new ion::io::ResourceManager();

		SetDefaultResourceDirectories();

		if(config.input.supportKeyboard)
			input.keyboard = new ion::input::Keyboard();

		if(config.input.supportMouse)
			input.mouse = new ion::input::Mouse();

		if (config.input.maxSupportedControllers > 0)
		{
			input.gamepadManager = ion::input::GamepadManager::Create(config.input.maxSupportedControllers);
			input.gamepads.reserve(config.input.maxSupportedControllers);

			for (int i = 0; i < config.input.maxSupportedControllers; i++)
			{
				input.gamepads.push_back(input.gamepadManager->CreateGamepad());
			}
		}

		if(config.window.createWindow)
			render.window = ion::render::Window::Create(config.window.title, config.window.clientAreaWidth, config.window.clientAreaHeight, config.window.fullscreen);

		if (config.render.createRenderer)
		{
			render.renderer = ion::render::Renderer::Create(render.window->GetDeviceContext());
			render.viewport = new ion::render::Viewport(render.window->GetClientAreaWidth(), render.window->GetClientAreaHeight(), config.render.perspectiveMode);

			if(config.render.createCamera)
				render.camera = new ion::render::Camera();

			render.viewport->SetClearColour(ion::Colour(0.0f, 0.0f, 0.0f, 1.0f));
		}

		if (config.audio.supportAudio)
		{
			audio.engine = ion::audio::Engine::Create();
			audio.thread = new AudioThread(audio.engine);
			audio.thread->Run();
			audio.thread->SetPriority(ion::thread::Thread::Priority::High);
		}

#if defined ION_SERVICES
		if (config.services.supportPlatformServices)
		{
			services.platform = ion::services::Platform::Create(config.services.appId, config.services.appEncryptionKey);
			services.userManager = ion::services::UserManager::Create();
			services.saveManager = ion::services::SaveManager::Create();
			services.achievements = ion::services::Achievements::Create();
		}
#endif
	}

	void Engine::Shutdown()
	{
		//Shutdown platform
		ion::platform::Shutdown();
	}

	void Engine::SetDefaultResourceDirectories()
	{
		//Set resource directories
		io.resourceManager->SetResourceDirectory<ion::render::Texture>("assets/textures", ".ion.texture");
		io.resourceManager->SetResourceDirectory<ion::render::Material>("assets/materials", ".ion.material");

#if defined ION_RENDERER_SHADER
		io.resourceManager->SetResourceDirectory<ion::render::Shader>("assets/shaders", ".ion.shader");
#endif
	}

	bool Engine::Update(float deltaTime)
	{
		if(input.keyboard)
			input.keyboard->Update();

		if(input.mouse)
			input.mouse->Update();

		if(input.gamepadManager)
			input.gamepadManager->Update();

		io.resourceManager->Update();

#if defined ION_SERVICES
		if (services.platform)
		{
			services.platform->Update();
			services.userManager->Update();
		}
#endif

		if (render.window)
			return render.window->Update();
		else
			return true;
	}

	void Engine::BeginRenderFrame()
	{
		render.renderer->BeginFrame(*render.viewport, render.window->GetDeviceContext());

		render.renderer->ClearColour();
		render.renderer->ClearDepth();
	}

	void Engine::EndRenderFrame()
	{
		render.renderer->SwapBuffers();
		render.renderer->EndFrame();
	}

	Engine engine;
}