///////////////////////////////////////////////////
// File:		GUI.cpp
// Date:		12th April 2018
// Authors:		Matt Phillips
// Description:	GUI manager/renderer
///////////////////////////////////////////////////

#include "GUI.h"

#include <ion/renderer/Renderer.h>
#include <ion/renderer/VertexBuffer.h>
#include <ion/renderer/IndexBuffer.h>

#include <ion/dependencies/imgui/imgui.h>

#include <ion/renderer/Primitive.h>

namespace ion
{
	namespace gui
	{
		GUI::GUI(const Vector2i& size)
		{
			//Create imgui context
			ImGui::CreateContext();

			//Set default style
			ImGui::StyleColorsDark();

			//Set UI size
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)size.x, (float)size.y);

			//Create font atlas
			u8* fontAtlasData;
			int fontAtlasWidth;
			int fontAtlasHeight;
			io.Fonts->GetTexDataAsRGBA32(&fontAtlasData, &fontAtlasWidth, &fontAtlasHeight);

			m_fontAtlasTexture = render::Texture::Create(fontAtlasWidth, fontAtlasHeight, render::Texture::eRGBA, render::Texture::eRGBA, render::Texture::eBPP32, false, fontAtlasData);
			m_fontAtlasMaterial = new render::Material();
			m_fontAtlasMaterial->AddDiffuseMap(m_fontAtlasTexture);

			io.Fonts->SetTexID(m_fontAtlasMaterial);
		}

		void GUI::AddWidget(Widget& widget)
		{
			m_widgets.push_back(&widget);
		}

		void GUI::Update(float deltaTime)
		{
			ImGui::NewFrame();

			ImGuiIO& io = ImGui::GetIO();
			io.DeltaTime = deltaTime;

			ImGui::Begin("Test");

			for (int i = 0; i < m_widgets.size(); i++)
			{
				m_widgets[i]->Update(deltaTime);
			}

			ImGui::End();
			ImGui::EndFrame();
		}

		void GUI::Render(ion::render::Renderer& renderer, ion::render::Viewport& viewport)
		{
			ImGui::Render();

			if (ImDrawData* drawData = ImGui::GetDrawData())
			{
				ion::Matrix4 viewMtx;
				viewMtx.SetTranslation(ion::Vector3(-(float)viewport.GetWidth() / 2.0f, -(float)viewport.GetHeight() / 2.0f, -1.0f));

				for (int i = 0; i < drawData->CmdListsCount; i++)
				{
					const ImDrawList* commandList = drawData->CmdLists[i];
					const ImDrawVert* vertexBuffer = commandList->VtxBuffer.Data;
					const ImDrawIdx* indexBuffer = commandList->IdxBuffer.Data;

					//Super slow and lazy - build ion vertex buffer right here
					//TODO: allow VertexBuffer to be constructed with a ptr, size and layout descriptor
					ion::render::VertexBuffer vertices(render::VertexBuffer::eTriangles);

					for (int j = 0; j < commandList->VtxBuffer.size(); j++)
					{
						const ImDrawVert& vertex = vertexBuffer[j];
						vertices.AddVertex(Vector3(vertex.pos.x, vertex.pos.y, 0.0f),
							Vector3(),
							Colour(((u8)vertex.col & 0xFF), ((u8)(vertex.col >> 8) & 0xFF), (u8)((vertex.col >> 16) & 0xFF), (u8)((vertex.col >> 24) & 0xFF)),
							render::TexCoord(vertex.uv.x, vertex.uv.y));
					}

					for (int j = 0; j < commandList->CmdBuffer.Size; j++)
					{
						const ImDrawCmd* drawCommand = &commandList->CmdBuffer[j];
						if (drawCommand->UserCallback)
						{
							//TODO
						}
						else
						{
							render::Material* material = (render::Material*)drawCommand->TextureId;

							if (material)
							{
								material->Bind(ion::Matrix4(), viewMtx.GetInverse(), renderer.GetProjectionMatrix());
							}
							else
							{
								m_defaultMaterial.Bind(ion::Matrix4(), viewMtx.GetInverse(), renderer.GetProjectionMatrix());
							}

							//Super slow and lazy - build ion index buffer right here
							//TODO: allow IndexBuffer to be constructed with a ptr, size and element width
							ion::render::IndexBuffer indices;
							for (int k = 0; k < drawCommand->ElemCount; k++)
							{
								indices.Add(indexBuffer[k]);
							}

							renderer.DrawVertexBuffer(vertices, indices);

							if (material)
							{
								material->Unbind();
							}
							else
							{
								m_defaultMaterial.Unbind();
							}
						}

						indexBuffer += drawCommand->ElemCount;
					}
				}
			}
		}
	}
}