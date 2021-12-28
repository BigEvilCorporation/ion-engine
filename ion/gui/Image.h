///////////////////////////////////////////////////
// File:		Image.h
// Date:		18th July 2018
// Authors:		Matt Phillips
// Description:	UI image
///////////////////////////////////////////////////

#pragma once

#include "Widget.h"

#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/resource/ResourceHandle.h>

#include <string>
#include <functional>

namespace ion
{
	namespace gui
	{
		class Image : public Widget
		{
		public:
			Image(ion::io::ResourceHandle<render::Texture>& texture);
			virtual ~Image();

			void SetTexture(ion::io::ResourceHandle<render::Texture>& texture);

#if defined ION_RENDERER_SHADER
			void SetShader(ion::io::ResourceHandle<ion::render::Shader>& shader);
#endif

			void SetMaintainAspect(bool maintain);
			void SetImageBorder(const Vector2i& border);
			const Vector2i& GetImageBorder() const;

			virtual void Update(float deltaTime);

		private:
			ion::io::ResourceHandle<render::Texture> m_texture;
			render::Material* m_material;
			Vector2i m_imageBorder;
			bool m_maintainAspect;
		};
	}
}