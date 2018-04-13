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
#include "Widget.h"

namespace ion
{
	namespace gui
	{
		class GUI
		{
		public:
			GUI(const Vector2i& size);

			void AddWidget(Widget& widget);

			void Update(float deltaTime);
			void Render(ion::render::Renderer& renderer, ion::render::Viewport& viewport);

		private:
			render::Texture* m_fontAtlasTexture;
			render::Material* m_fontAtlasMaterial;
			render::Material m_defaultMaterial;
			std::vector<Widget*> m_widgets;
		};
	}
}