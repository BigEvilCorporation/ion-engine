///////////////////////////////////////////////////
// File:		GUI.h
// Date:		12th April 2018
// Authors:		Matt Phillips
// Description:	GUI manager/renderer
///////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Renderer.h>
#include <ion/renderer/Viewport.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/renderer/Shader.h>
#include <ion/renderer/VertexBuffer.h>
#include <ion/input/Keyboard.h>
#include <ion/input/Mouse.h>
#include <ion/input/Gamepad.h>

#include <ion/dependencies/imgui/imgui.h>

#include "Window.h"
#include "Font.h"

namespace ion
{
	namespace gui
	{
		class GUI
		{
		public:
			GUI(const Vector2i& size, float scale = 1.0f);
			virtual ~GUI();

			Font* LoadFontTTF(const std::string filename, int size);

#if defined ION_RENDERER_SHADER
			void SetShader(ion::io::ResourceHandle<ion::render::Shader> shader);
#endif

			void AddWindow(Window& window);
			void RemoveWindow(Window& window);

			void PushWindow(Window& window);
			void PopWindow();

			void DeleteWindow(Window& window);

			void MessageBox(const std::string& title, const std::string& message);

			void Update(float deltaTime, input::Keyboard* keyboard, input::Mouse* mouse, input::Gamepad* gamepad);
			void Render(ion::render::Renderer& renderer, ion::render::Viewport& viewport);

			void SetVisible(bool visible);
			void SetSize(const Vector2i& size);

			void StyleSetTitleAlignment(const Vector2& align);
			void StyleSetWindowCornerRadius(float radius);

		private:
			std::vector<Window*> m_windows;
			std::vector<Window*> m_windowStack;
			std::vector<Window*> m_windowDeleteList;
			bool m_visible;

			//Imgui
			ImGuiContext* m_imguiContext;
			io::ResourceHandle<render::Texture> m_fontAtlasTexture;
			render::Material* m_fontAtlasMaterial;
			render::Material m_defaultMaterial;
		};
	}
}