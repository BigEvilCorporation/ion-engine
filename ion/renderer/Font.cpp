///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Font.cpp
// Date:		12th October 2016
// Authors:		Matt Phillips
// Description:	Font loading and text rendering
///////////////////////////////////////////////////

#if ION_GUI_SUPPORTS_FREETYPE

#include "Font.h"

namespace ion
{
	namespace render
	{
		Font::Font()
		{
			int result = FT_Init_FreeType(&m_freetypeLibrary);
		}

		bool Font::Load(const std::string& filename)
		{
			return false;
		}

		Texture* Font::RenderTotexture(const std::string& string, int size)
		{
			return NULL;
		}
	}
}

#endif