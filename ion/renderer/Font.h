///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Font.h
// Date:		12th October 2016
// Authors:		Matt Phillips
// Description:	Font loading and text rendering
///////////////////////////////////////////////////

#if ION_GUI_SUPPORTS_FREETYPE

#pragma once

#include "core/Types.h"
#include "renderer/Texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ion
{
	namespace render
	{
		class Font
		{
		public:
			Font();

			bool Load(const std::string& filename);
			Texture* RenderTotexture(const std::string& string, int size);

		private:
			FT_Library  m_freetypeLibrary;
		};
	}
}

#endif