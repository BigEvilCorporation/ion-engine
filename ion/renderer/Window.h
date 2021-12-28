///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Window.h
// Date:		12th January 2014
// Authors:		Matt Phillips
// Description:	Window management
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"
#include "maths/Vector.h"
#include "renderer/Types.h"

#include <functional>

namespace ion
{
	namespace render
	{
		struct Display
		{
			int index;
			std::string name;
			Vector2i size;
			Vector2i topLeft;
		};

		class Window
		{
		public:
			static Window* Create(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen);

			typedef std::function<void(int displayIdx, const ion::Vector2i& resolution)> OnDisplayChanged;

			Window(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen);
			virtual ~Window() {}

			u32 GetWindowWidth() const;
			u32 GetWindowHeight() const;

			u32 GetClientAreaWidth() const;
			u32 GetClientAreaHeight() const;

			bool GetFullscreen() const;

			virtual u32 GetDesktopWidth(int displayIdx) const = 0;
			virtual u32 GetDesktopHeight(int displayIdx) const = 0;

			virtual int GetDisplays(std::vector<Display>& displays) const = 0;
			virtual int GetSupportedResolutions(std::vector<Vector2i>& resolutions, int displayIdx) const = 0;
			virtual void RegisterDisplayChangedCallback(OnDisplayChanged const& onDisplayChanged) {}

			float GetAspectRatio() const;

			virtual bool Update() = 0;
			virtual bool Resize(u32 clientAreaWidth, u32 clientAreaHeight, bool adjustForTitle) = 0;
			virtual bool SetFullscreen(bool fullscreen, int displayIdx) = 0;
			virtual void SetTitle(const std::string& title) = 0;
			virtual void ShowCursor(bool show) = 0;
			virtual bool HasFocus() const = 0;

			virtual DeviceContext GetDeviceContext() const = 0;
			virtual WindowHandle GetWindowHandle() const = 0;

		protected:
			u32 m_windowWidth;
			u32 m_windowHeight;
			u32 m_clientAreaWidth;
			u32 m_clientAreaHeight;
			float m_aspectRatio;
			bool m_fullscreen;
		};
	}
}