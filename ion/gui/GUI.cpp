///////////////////////////////////////////////////
// File:		GUI.cpp
// Date:		12th April 2018
// Authors:		Matt Phillips
// Description:	GUI manager/renderer
///////////////////////////////////////////////////

#include "GUI.h"
#include "MessageBox.h"

#include <ion/renderer/Renderer.h>
#include <ion/renderer/IndexBuffer.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/Camera.h>

#include <algorithm>

namespace ion
{
	namespace gui
	{
		GUI::GUI(const Vector2i& size, float scale)
		{
			m_visible = true;

			//Create imgui context
			ImGuiContext* prevContext = ImGui::GetCurrentContext();
			m_imguiContext = ImGui::CreateContext();
			ImGui::SetCurrentContext(m_imguiContext);

			//Get ImGui IO
			ImGuiIO& io = ImGui::GetIO();

			//Disable INI file saving/loading by default
			io.IniFilename = NULL;

			//Set default style
			ImGui::StyleColorsClassic();

			//Set UI size and scale
			io.DisplaySize = ImVec2((float)size.x, (float)size.y);
			io.DisplayFramebufferScale = ImVec2(scale, scale);

			//Draw mouse cursor
			io.MouseDrawCursor = true;

			//Create font atlas
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

			//Map keyboard
			io.KeyMap[ImGuiKey_Tab] = (int)ion::input::Keycode::TAB;
			io.KeyMap[ImGuiKey_LeftArrow] = (int)ion::input::Keycode::LEFT;
			io.KeyMap[ImGuiKey_RightArrow] = (int)ion::input::Keycode::RIGHT;
			io.KeyMap[ImGuiKey_UpArrow] = (int)ion::input::Keycode::UP;
			io.KeyMap[ImGuiKey_DownArrow] = (int)ion::input::Keycode::DOWN;
			io.KeyMap[ImGuiKey_PageUp] = (int)ion::input::Keycode::PAGEUP;
			io.KeyMap[ImGuiKey_PageDown] = (int)ion::input::Keycode::PAGEDOWN;
			io.KeyMap[ImGuiKey_Home] = (int)ion::input::Keycode::HOME;
			io.KeyMap[ImGuiKey_End] = (int)ion::input::Keycode::END;
			io.KeyMap[ImGuiKey_Insert] = 0;
			io.KeyMap[ImGuiKey_Delete] = (int)ion::input::Keycode::DELETE;
			io.KeyMap[ImGuiKey_Backspace] = (int)ion::input::Keycode::BACKSPACE;
			io.KeyMap[ImGuiKey_Space] = (int)ion::input::Keycode::SPACE;
			io.KeyMap[ImGuiKey_Enter] = (int)ion::input::Keycode::RETURN;
			io.KeyMap[ImGuiKey_Escape] = (int)ion::input::Keycode::ESCAPE;
			io.KeyMap[ImGuiKey_A] = (int)ion::input::Keycode::A;
			io.KeyMap[ImGuiKey_C] = (int)ion::input::Keycode::C;
			io.KeyMap[ImGuiKey_V] = (int)ion::input::Keycode::V;
			io.KeyMap[ImGuiKey_X] = (int)ion::input::Keycode::X;
			io.KeyMap[ImGuiKey_Y] = (int)ion::input::Keycode::Y;
			io.KeyMap[ImGuiKey_Z] = (int)ion::input::Keycode::Z;

			//Restore context
			ImGui::SetCurrentContext(prevContext);
		}

		GUI::~GUI()
		{
			SetVisible(false);
			delete m_fontAtlasMaterial;
			m_fontAtlasTexture.Clear();
			ImGui::DestroyContext(m_imguiContext);
		}

		Font* GUI::LoadFontTTF(const std::string filename, int size)
		{
			Font* font = nullptr;
			
			if (ion::io::FileExists(filename))
			{
				font = new Font();

				if (!font->LoadTTF(filename, size, m_imguiContext))
				{
					delete font;
					font = nullptr;
				}
			}

			return font;
		}

#if defined ION_RENDERER_SHADER
		void GUI::SetShader(ion::io::ResourceHandle<ion::render::Shader> shader)
		{
			m_fontAtlasMaterial->SetShader(shader);
			m_defaultMaterial.SetShader(shader);
		}
#endif

		void GUI::AddWindow(Window& window)
		{
			m_windows.push_back(&window);
			window.OnWindowAdded();
		}

		void GUI::RemoveWindow(Window& window)
		{
			m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), &window), m_windows.end());
		}

		void GUI::PushWindow(Window& window)
		{
			m_windowStack.push_back(&window);
			window.OnWindowAdded();
		}

		void GUI::PopWindow()
		{
			debug::Assert(m_windowStack.size() > 0, "GUI::PopWindow() - Window stack is empty");
			m_windowStack.pop_back();
		}

		void GUI::DeleteWindow(Window& window)
		{
			m_windowDeleteList.push_back(&window);
		}

		void GUI::MessageBox(const std::string& title, const std::string& message)
		{
			new ion::gui::MessageBox(title, message, ion::gui::MessageBox::Ok,
				[&](const ion::gui::MessageBox& messageBox, ion::gui::MessageBox::ButtonType buttonType)
			{
				RemoveWindow((ion::gui::MessageBox&)messageBox);
				DeleteWindow((ion::gui::MessageBox&)messageBox);
			});
		}

		void GUI::SetVisible(bool visible)
		{
			m_visible = visible;
		}

		void GUI::SetSize(const Vector2i& size)
		{
			ImGuiContext* prevContext = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(m_imguiContext);
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)size.x, (float)size.y);
			ImGui::SetCurrentContext(prevContext);
		}

		void GUI::StyleSetTitleAlignment(const Vector2& align)
		{
			ImGuiContext* prevContext = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(m_imguiContext);
			ImGui::GetStyle().WindowTitleAlign = ImVec2(align.x, align.y);
			ImGui::SetCurrentContext(prevContext);
		}

		void GUI::StyleSetWindowCornerRadius(float radius)
		{
			ImGuiContext* prevContext = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(m_imguiContext);
			ImGui::GetStyle().WindowRounding = radius;
			ImGui::SetCurrentContext(prevContext);
		}

		void GUI::Update(float deltaTime, input::Keyboard* keyboard, input::Mouse* mouse, input::Gamepad* gamepad)
		{
			//Set context
			ImGuiContext* prevContext = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(m_imguiContext);

			//Update delta
			ImGuiIO& io = ImGui::GetIO();
			io.DeltaTime = deltaTime;

			if (m_visible)
			{
				//Provide keyboard input
				if (keyboard)
				{
					io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

					for (int i = 0; i < (int)ion::input::Keycode::COUNT; i++)
					{
						io.KeysDown[i] = keyboard->KeyDown((ion::input::Keycode)i);
					}
				}

				//Provide mouse input
				if (mouse)
				{
					io.MousePos = ImVec2((float)mouse->GetAbsoluteX() / io.DisplayFramebufferScale.x, (float)mouse->GetAbsoluteY() / io.DisplayFramebufferScale.y);
					//io.MouseWheel = mouse->GetWheelAbsolute();
					io.MouseDown[0] = mouse->ButtonDown(input::Mouse::LB);
					io.MouseDown[1] = mouse->ButtonDown(input::Mouse::RB);
					io.MouseDown[2] = mouse->ButtonDown(input::Mouse::MB);
				}

				//Provide gamepad input
				if (gamepad)
				{
					io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad | ImGuiBackendFlags_HasGamepad;

					io.NavInputs[ImGuiNavInput_Activate] = gamepad->ButtonDown(ion::input::GamepadButtons::BUTTON_A) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_Cancel] = gamepad->ButtonDown(ion::input::GamepadButtons::BUTTON_B) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_Input] = gamepad->ButtonDown(ion::input::GamepadButtons::BUTTON_Y) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_Menu] = gamepad->ButtonDown(ion::input::GamepadButtons::BUTTON_X) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_DpadLeft] = gamepad->ButtonDown(ion::input::GamepadButtons::DPAD_LEFT) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_DpadRight] = gamepad->ButtonDown(ion::input::GamepadButtons::DPAD_RIGHT) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_DpadUp] = gamepad->ButtonDown(ion::input::GamepadButtons::DPAD_UP) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_DpadDown] = gamepad->ButtonDown(ion::input::GamepadButtons::DPAD_DOWN) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_LStickLeft] = -gamepad->GetLeftStick().x;
					io.NavInputs[ImGuiNavInput_LStickRight] = gamepad->GetLeftStick().x;
					io.NavInputs[ImGuiNavInput_LStickUp] = gamepad->GetLeftStick().y;
					io.NavInputs[ImGuiNavInput_LStickDown] = -gamepad->GetLeftStick().y;
					io.NavInputs[ImGuiNavInput_FocusPrev] = gamepad->ButtonDown(ion::input::GamepadButtons::LEFT_TRIGGER) ? 1.0f : 0.0f;
					io.NavInputs[ImGuiNavInput_FocusNext] = gamepad->ButtonDown(ion::input::GamepadButtons::RIGHT_TRIGGER) ? 1.0f : 0.0f;
				}

				//Begin UI frame
				ImGui::NewFrame();

				//Update windows
				for (int i = 0; i < m_windows.size(); i++)
				{
					m_windows[i]->Update(deltaTime);
				}

				//Update top of stack
				if (m_windowStack.size() > 0)
				{
					m_windowStack[m_windowStack.size() - 1]->Update(deltaTime);
				}

				//Generate render data
				ImGui::Render();

				//End UI frame
				ImGui::EndFrame();

				//Restore context
				ImGui::SetCurrentContext(prevContext);
			}

			//Delete deferred windows
			for (int i = 0; i < m_windowDeleteList.size(); i++)
			{
				RemoveWindow(*m_windowDeleteList[i]);
				delete m_windowDeleteList[i];
			}

			m_windowDeleteList.clear();
		}

		void GUI::Render(ion::render::Renderer& renderer, ion::render::Viewport& viewport)
		{
			//Set context
			ImGuiContext* prevContext = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(m_imguiContext);

			if (ImDrawData* drawData = ImGui::GetDrawData())
			{
				ion::Matrix4 viewMtx;
				viewMtx.SetTranslation(ion::Vector3(0.0f, -(float)viewport.GetHeight(), -1.0f));

				Vector3 scale(drawData->FramebufferScale.x, drawData->FramebufferScale.y, 1.0f);

				renderer.SetAlphaBlending(ion::render::Renderer::AlphaBlendType::Translucent);
				renderer.SetFaceCulling(ion::render::Renderer::CullingMode::None);
				renderer.SetScissorTest(ion::render::Renderer::ScissorTest::Enabled);

				ImVec2 displayPos = drawData->DisplayPos;

				for (int i = 0; i < drawData->CmdListsCount; i++)
				{
					const ImDrawList* commandList = drawData->CmdLists[i];
					const ImDrawVert* vertexBuffer = commandList->VtxBuffer.Data;
					const ImDrawIdx* indexBuffer = commandList->IdxBuffer.Data;

					//Build vertex and index buffers
					ion::render::VertexBuffer vertices(ion::render::VertexBuffer::Pattern::Triangles);
					ion::render::IndexBuffer indices;
					
					vertices.Reserve(commandList->VtxBuffer.size());
					indices.Reserve(commandList->IdxBuffer.size());
					
					for (int j = 0; j < commandList->VtxBuffer.size(); j++)
					{
						const ImDrawVert& vertex = vertexBuffer[j];
						vertices.AddVertex(Vector3(vertex.pos.x, vertex.pos.y, 0.0f) * scale,
							Vector3(),
							Colour(((u8)vertex.col & 0xFF), ((u8)(vertex.col >> 8) & 0xFF), (u8)((vertex.col >> 16) & 0xFF), (u8)((vertex.col >> 24) & 0xFF)),
							render::TexCoord(vertex.uv.x, vertex.uv.y));
					}
					
					for (unsigned int j = 0; j < commandList->IdxBuffer.size(); j++)
					{
						indices.Add(indexBuffer[j]);
					}
					
					vertices.CompileBuffer(&indices);

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
							if (!material)
							{
								material = &m_defaultMaterial;
							}
					
#if defined ION_RENDERER_SHADER
							if (material->GetShader())
#endif
							{
								ion::Matrix4 objMtx;
								objMtx.SetScale(ion::Vector3(1.0f, -1.0f, 1.0f));

								renderer.BindMaterial(*material, objMtx, viewMtx.GetInverse(), renderer.GetProjectionMatrix());

								//Scissor rect
								ImVec4 clipRect;
								clipRect.x = (drawCommand->ClipRect.x - displayPos.x) * scale.x;
								clipRect.y = (drawCommand->ClipRect.y - displayPos.y) * scale.y;
								clipRect.z = (drawCommand->ClipRect.z - displayPos.x) * scale.x;
								clipRect.w = (drawCommand->ClipRect.w - displayPos.y) * scale.y;
								renderer.SetScissorRegion(ion::Vector2i((int)clipRect.x, (int)(drawData->DisplaySize.y - clipRect.w)), ion::Vector2i((int)(clipRect.z - clipRect.x), (int)(clipRect.w - clipRect.y)));

								//Draw
								renderer.DrawVertexBuffer(vertices, drawCommand->IdxOffset * sizeof(ion::render::TIndex), drawCommand->ElemCount);

								renderer.UnbindMaterial(*material);
							}
						}
					}
				}

				renderer.SetAlphaBlending(ion::render::Renderer::AlphaBlendType::None);
				renderer.SetScissorTest(ion::render::Renderer::ScissorTest::Disabled);
			}

			//Restore context
			ImGui::SetCurrentContext(prevContext);
		}
	}
}