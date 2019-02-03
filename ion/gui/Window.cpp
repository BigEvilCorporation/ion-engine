///////////////////////////////////////////////////
// File:		Window.cpp
// Date:		23nd September 2011
// Authors:		Matt Phillips
// Description:	UI window
///////////////////////////////////////////////////

#include "Window.h"

#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		Window::Window(const std::string& title, const Vector2i& position, const Vector2i& size)
			: Widget(position, size)
			, m_title(title)
		{
			m_allowResize = true;
			m_allowMove = true;
			m_allowRollup = true;
			m_rolledUp = false;
			m_font = nullptr;
			m_backgroundAlpha = 1.0f;
			m_hashIdx = 0;
		}

		Window::~Window()
		{
		}

		void Window::AddWidget(Widget& widget)
		{
			m_widgets.push_back(&widget);
		}

		void Window::AllowResize(bool allow)
		{
			m_allowResize = allow;
		}

		void Window::AllowMove(bool allow)
		{
			m_allowMove = allow;
		}

		void Window::AllowRollUp(bool allow)
		{
			m_allowRollup = allow;
		}

		void Window::RollUp(bool rolled)
		{
			m_rolledUp = rolled;
		}

		void Window::SetFont(Font& font)
		{
			m_font = &font;
		}

		void Window::SetBackgroundAlpha(float alpha)
		{
			m_backgroundAlpha = alpha;
		}

		void Window::OnWindowAdded()
		{
			//Create unique hash for this visible "instance", to clear first use flags in ImGui
			m_hashIdx++;
			std::stringstream stream;
			stream << m_title << "##" << m_hashIdx;
			m_hashName = stream.str();
		}

		void Window::Update(float deltaTime)
		{
			if (m_visible)
			{
				//Apply rules
				ImGuiWindowFlags flags = 0;

				if (!m_allowResize)
					flags |= ImGuiWindowFlags_NoResize;
				if (!m_allowMove)
					flags |= ImGuiWindowFlags_NoMove;
				if (!m_allowRollup)
					flags |= ImGuiWindowFlags_NoCollapse;

				//Apply initial position/size
				ImGui::SetNextWindowSize(ImVec2(m_size.x, m_size.y), m_allowResize ? ImGuiCond_FirstUseEver : 0);

				//Apply collapse state
				ImGui::SetNextWindowCollapsed(m_rolledUp, m_allowRollup ? ImGuiCond_FirstUseEver : 0);

				if (m_centred)
				{
					ImGuiIO& io = ImGui::GetIO();
					ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), m_allowMove ? ImGuiCond_FirstUseEver : 0, ImVec2(0.5f, 0.5f));
				}
				else
				{
					ImGui::SetNextWindowPos(ImVec2(m_position.x, m_position.y), m_allowMove ? ImGuiCond_FirstUseEver : 0);
				}

				//Apply styling
				ImGui::SetNextWindowBgAlpha(m_backgroundAlpha);

				//Apply font
				if (m_font)
				{
					ImGui::PushFont(m_font->m_imFont);
				}

				//Begin window
				ImGui::Begin(m_hashName.c_str(), NULL, flags);

				//Poll new position/size
				if (m_allowMove)
				{
					m_position.x = ImGui::GetWindowPos().x;
					m_position.y = ImGui::GetWindowPos().y;
				}

				if (m_allowResize && !ImGui::IsWindowCollapsed())
				{
					m_size.x = ImGui::GetWindowWidth();
					m_size.y = ImGui::GetWindowHeight();
				}

				if (m_allowRollup)
				{
					m_rolledUp = ImGui::IsWindowCollapsed();
				}

				//Update widgets
				for (int i = 0; i < m_widgets.size(); i++)
				{
					m_widgets[i]->Update(deltaTime);
				}

				//End window
				ImGui::End();

				if (m_font)
				{
					ImGui::PopFont();
				}
			}
		}
	}
}