///////////////////////////////////////////////////
// File:		Image.cpp
// Date:		18th July 2018
// Authors:		Matt Phillips
// Description:	UI image
///////////////////////////////////////////////////

#include "Image.h"

#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		Image::Image(ion::io::ResourceHandle<render::Texture>& texture)
		{
			m_material = nullptr;
			m_maintainAspect = true;
			m_imageBorder = Vector2i(10, 10);
			SetTexture(texture);
		}

		Image::~Image()
		{
			if (m_material)
			{
				delete m_material;
			}
		}

		void Image::SetTexture(ion::io::ResourceHandle<render::Texture>& texture)
		{
			m_texture = texture;

			if (m_material)
			{
				delete m_material;
			}

			m_material = new render::Material();
			m_material->AddDiffuseMap(m_texture);
		}

#if defined ION_RENDERER_SHADER
		void Image::SetShader(ion::io::ResourceHandle<ion::render::Shader>& shader)
		{
			m_material->SetShader(shader);
		}
#endif

		void Image::SetImageBorder(const Vector2i& border)
		{
			m_imageBorder = border;
		}

		const Vector2i& Image::GetImageBorder() const
		{
			return m_imageBorder;
		}

		void Image::SetMaintainAspect(bool maintain)
		{
			m_maintainAspect = maintain;
		}

		void Image::Update(float deltaTime)
		{
			if (m_material && m_texture && m_visible)
			{
				ImVec2 size;

				if (m_size.x > 0)
				{
					if (m_maintainAspect)
					{
						size.x = (float)m_size.x - (m_imageBorder.x * 2);
						size.y = ((float)size.x * ((float)m_texture->GetHeight() / (float)m_texture->GetWidth()));
					}
					else
					{
						size.x = (float)m_size.x - (m_imageBorder.x * 2);
						size.y = (float)m_size.y - (m_imageBorder.y * 2);
					}
				}
				else
				{
					if (m_maintainAspect)
					{
						size.x = ImGui::GetWindowWidth() - (m_imageBorder.x * 2);
						size.y = ((float)size.x * ((float)m_texture->GetHeight() / (float)m_texture->GetWidth()));
					}
					else
					{
						size.x = ImGui::GetWindowWidth() - (m_imageBorder.x * 2);
						size.y = ImGui::GetWindowHeight() - (m_imageBorder.y * 2);
					}
				}

				if (m_centred)
				{
					ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (size.x / 2));
				}
				else if (m_position.x != -1 && m_position.y != -1)
				{
					ImGui::SetCursorPos(ImVec2((float)m_position.x, (float)m_position.y));
				}

				if (m_arrangement == Arrangement::Horizontal)
				{
					ImGui::SameLine();
				}

				ImGui::Image((void*)m_material, size, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, m_alpha));
			}
		}
	}
}