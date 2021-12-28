///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		DebugText.cpp
// Date:		9th May 2019
// Authors:		Matt Phillips
// Description:	Debug text drawing
///////////////////////////////////////////////////

#include "DebugText.h"
#include "DebugFont.h"

#include <ion/maths/Maths.h>

namespace ion
{
	namespace gamekit
	{
		DebugText::DebugText()
		{
			m_textureSizeSq = (int)ion::maths::Sqrt(debugFontNumCharacters) * ion::maths::Max(debugFontCharacterWidth, debugFontCharacterHeight);

			m_fontTexture = ion::render::Texture::Create(m_textureSizeSq, m_textureSizeSq, ion::render::Texture::Format::R, ion::render::Texture::Format::RGBA, ion::render::Texture::BitsPerPixel::BPP24, false, false, (u8*)debugFont);
			m_material = new ion::render::Material();
			m_quad = new ion::render::Quad(ion::render::Quad::Axis::xy, ion::Vector2(debugFontCharacterWidth / 2.0f, debugFontCharacterHeight / 2.0f));

			m_material->AddDiffuseMap(m_fontTexture);
			m_material->SetDiffuseColour(ion::Colour(1.0f, 1.0f, 1.0f, 1.0f));
		}

		DebugText::~DebugText()
		{
			m_fontTexture.Clear();
			delete m_material;
			delete m_quad;
		}

		void DebugText::RenderString(const std::string& string, int x, int y)
		{
			ion::render::TexCoord coords[4];

			for (int i = 0; i < string.size(); i++)
			{
				u8 idx = string[i] - 'A';

				if (idx >= 0 && idx < debugFontNumCharacters)
				{
					GetTexCoords(idx, coords);
					m_quad->SetTexCoords(coords);
				}
			}
		}

		void DebugText::GetTexCoords(u8 character, ion::render::TexCoord coords[4])
		{

		}

		void DebugText::Draw(const std::string& string)
		{

		}

		void DebugText::Log(const std::string& string)
		{

		}

		void DebugText::Update(float deltaTime)
		{

		}

		void DebugText::Render(ion::render::Renderer& renderer, ion::render::Viewport& viewport)
		{

		}
	}
}