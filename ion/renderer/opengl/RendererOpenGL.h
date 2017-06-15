///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		RendererOpenGL.h
// Date:		4th December 2013
// Authors:		Matt Phillips
// Description:	OpenGL renderer implementation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/thread/CriticalSection.h"
#include "renderer/Renderer.h"

#if defined ION_RENDER_SUPPORTS_GLEW
#include <GL/glew.h>
#endif

#if defined ION_RENDER_SUPPORTS_GLUT
#include <GL/glut.h>
#endif

#if defined ION_PLATFORM_MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <SDL.h>
#include <SDL_OpenGL.h>
#elif defined ION_PLATFORM_LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#elif defined ION_PLATFORM_RASPBERRYPI
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#if defined ION_PLATFORM_WINDOWS
#include <GL/wglext.h>
#endif

namespace ion
{
	namespace render
	{
		class RendererOpenGL : public Renderer
		{
		public:
			//Create from existing DC
			RendererOpenGL(DeviceContext deviceContext);

			virtual ~RendererOpenGL();

			virtual bool Update(float deltaTime);
			virtual void OnResize(int width, int height);

			//Fixed function transforms
			virtual void SetMatrix(const Matrix4& matrix);
			virtual Matrix4 GetProjectionMatrix();

			//Thread context lock/unlock
			virtual void LockContext(const DeviceContext& deviceContext);
			virtual void UnlockContext();

			//Rendering - general
			virtual void BeginFrame(const Viewport& viewport, const DeviceContext& deviceContext);
			virtual void EndFrame();
			virtual void SwapBuffers();
			virtual void SetClearColour(const Colour& colour);
			virtual void ClearColour();
			virtual void ClearDepth();

			//Render states
			virtual void SetAlphaBlending(AlphaBlendType alphaBlendType);
			virtual void SetFaceCulling(CullingMode cullingMode);
			virtual void SetDepthTest(DepthTest depthTest);
			virtual void SetLineWidth(float width);

			//Vertex buffer drawing
			virtual void DrawVertexBuffer(const VertexBuffer& vertexBuffer);
			virtual void DrawVertexBuffer(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer);

			//Check for OpenGL errors
			static bool CheckGLError(const char* message);

		protected:
			//Setup
			void BindDC(DeviceContext deviceContext);
			void CreateContext(DeviceContext deviceContext);
			void InitContext(DeviceContext deviceContext);
			void SetupViewport(const Viewport& viewport);

			//OpenGL context
#if defined ION_PLATFORM_WINDOWS
            HGLRC m_openGLContext;
#elif defined ION_PLATFORM_MACOSX
            SDL_GLContext m_openGLContext;
#elif defined ION_PLATFORM_LINUX
            SDL_GLContext m_openGLContext;
#elif defined ION_PLATFORM_RASPBERRYPI
			SDL_GLContext m_openGLContext;
#elif defined ION_PLATFORM_DREAMCAST
            int m_openGLContext;
#endif

			//DC for gobal (non-rendering) context
			DeviceContext m_globalDC;

			//DC for current locked context
			DeviceContext m_currentDC;

			thread::CriticalSection m_contextCriticalSection;
			u32 m_contextLockStack;

#if defined ION_RENDERER_OPENGLES
			Matrix4 m_projectionMatrix;
#endif
		};
	}
}
