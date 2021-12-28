///////////////////////////////////////////////////
// File:		Font.h
// Date:		22nd July 2018
// Authors:		Matt Phillips
// Description:	GUI font
///////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/renderer/Shader.h>
#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		class Font
		{
		public:
			~Font();

#if defined ION_RENDERER_SHADER
			void SetShader(ion::io::ResourceHandle<ion::render::Shader> shader);
#endif

		protected:
			//Protected constructor - create fonts with ion::gui::GUI::LoadFontTTF()
			Font();

			bool LoadTTF(const std::string filename, int size, ImGuiContext* imGuiContext);

			ImFont* m_imFont;

		private:
			ion::io::ResourceHandle<ion::render::Texture> m_fontAtlasTexture;
			render::Material* m_fontAtlasMaterial;

			friend class GUI;
			friend class Window;
			friend class DialogBox;
		};
	}
}