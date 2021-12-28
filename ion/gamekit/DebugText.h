///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		DebugText.h
// Date:		9th May 2019
// Authors:		Matt Phillips
// Description:	Debug text drawing
///////////////////////////////////////////////////

#include <ion/renderer/Renderer.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/renderer/Primitive.h>

#include <string>
#include <vector>

namespace ion
{
	namespace gamekit
	{
		class DebugText
		{
		public:
			DebugText();
			~DebugText();

			void Draw(const std::string& string);
			void Log(const std::string& string);

			void Update(float deltaTime);
			void Render(ion::render::Renderer& renderer, ion::render::Viewport& viewport);

		private:
			void RenderString(const std::string& string, int x, int y);
			void GetTexCoords(u8 character, ion::render::TexCoord coords[4]);

			std::vector<std::string> m_drawLines;
			std::vector<std::string> m_logLines;
		
			ion::io::ResourceHandle<ion::render::Texture> m_fontTexture;
			ion::render::Material* m_material;
			ion::render::Quad* m_quad;

			int m_textureSizeSq;
		};
	}
}