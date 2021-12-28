///////////////////////////////////////////////////
// File:		Font.cpp
// Date:		22nd July 2018
// Authors:		Matt Phillips
// Description:	GUI font
///////////////////////////////////////////////////

#include "Font.h"

namespace ion
{
	namespace gui
	{
		Font::Font()
		{
			m_fontAtlasMaterial = nullptr;
			m_imFont = nullptr;
		}

		Font::~Font()
		{
			if (m_fontAtlasMaterial)
			{
				delete m_fontAtlasMaterial;
				m_fontAtlasMaterial = nullptr;
			}

			m_fontAtlasTexture.Clear();
		}

#if defined ION_RENDERER_SHADER
		void Font::SetShader(ion::io::ResourceHandle<ion::render::Shader> shader)
		{
			if(m_fontAtlasMaterial)
				m_fontAtlasMaterial->SetShader(shader);
		}
#endif

		bool Font::LoadTTF(const std::string filename, int size, ImGuiContext* imGuiContext)
		{
			bool result = false;

			ImGuiContext* prevContext = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(imGuiContext);

			ImGuiIO& io = ImGui::GetIO();

			//Load TTF
			m_imFont = io.Fonts->AddFontFromFileTTF(filename.c_str(), (float)size, nullptr, io.Fonts->GetGlyphRangesDefault());

			if (m_imFont)
			{
				//Re-create font atlas
				if (m_fontAtlasMaterial)
				{
					delete m_fontAtlasMaterial;
					m_fontAtlasMaterial = nullptr;
				}

				m_fontAtlasTexture.Clear();

				u8* fontAtlasData;
				int fontAtlasWidth;
				int fontAtlasHeight;
				io.Fonts->GetTexDataAsRGBA32(&fontAtlasData, &fontAtlasWidth, &fontAtlasHeight);

				m_fontAtlasTexture = render::Texture::Create(fontAtlasWidth, fontAtlasHeight, render::Texture::Format::RGBA, render::Texture::Format::RGBA, render::Texture::BitsPerPixel::BPP24, false, false, fontAtlasData);
				m_fontAtlasTexture->SetMinifyFilter(ion::render::Texture::Filter::Linear);
				m_fontAtlasTexture->SetMagnifyFilter(ion::render::Texture::Filter::Linear);

				m_fontAtlasMaterial = new render::Material();
				m_fontAtlasMaterial->AddDiffuseMap(m_fontAtlasTexture);
				m_fontAtlasMaterial->SetDiffuseColour(ion::Colour(1.0f, 1.0f, 1.0f, 1.0f));

				io.Fonts->SetTexID(m_fontAtlasMaterial);

				result = true;
			}

			ImGui::SetCurrentContext(prevContext);

			return result;
		}
	}
}