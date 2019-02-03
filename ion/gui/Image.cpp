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
		Image::Image(render::Texture* texture)
			: m_texture(texture)
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

		void Image::SetTexture(render::Texture* texture)
		{
			m_texture = texture;

			if (m_material)
			{
				delete m_material;
			}

			if (m_texture)
			{
				m_material = new render::Material();
				m_material->AddDiffuseMap(m_texture);
			}
		}

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
			if (m_material && m_visible)
			{
				ImVec2 size;

				if (m_size.x > 0)
				{
					if (m_maintainAspect)
					{
						size.x = m_size.x - (m_imageBorder.x * 2);
						size.y = (int)((float)size.x * ((float)m_texture->GetHeight() / (float)m_texture->GetWidth()));
					}
					else
					{
						size.x = m_size.x - (m_imageBorder.x * 2);
						size.y = m_size.y - (m_imageBorder.y * 2);
					}
				}
				else
				{
					if (m_maintainAspect)
					{
						size.x = ImGui::GetWindowWidth() - (m_imageBorder.x * 2);
						size.y = (int)((float)size.x * ((float)m_texture->GetHeight() / (float)m_texture->GetWidth()));
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
				else if (m_position.x != 0 && m_position.y != 0)
				{
					ImGui::SetCursorPos(ImVec2(m_position.x, m_position.y));
				}

				if (m_arrangement == Arrangement::Horizontal)
				{
					ImGui::SameLine();
				}

				ImGui::Image((void*)m_material, size);
			}
		}
	}
}