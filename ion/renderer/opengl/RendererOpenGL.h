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
#include "renderer/opengl/OpenGLInclude.h"

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

			//Render contexts
			virtual RenderContext GetRenderContext();
			virtual void ShareRenderContext(RenderContext& renderContext);

			//Rendering - general
			virtual void BeginFrame(const Viewport& viewport, const DeviceContext& deviceContext);
			virtual void EndFrame();
			virtual void SwapBuffers();
			virtual void SetClearColour(const Colour& colour);
			virtual void ClearColour();
			virtual void ClearDepth();
			virtual void EnableVSync(bool enabled);

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
			RenderContext m_openGLContext;

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
