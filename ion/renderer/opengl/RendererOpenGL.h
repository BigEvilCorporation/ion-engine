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
#include "core/thread/LocalStorage.h"
#include "renderer/Renderer.h"
#include "renderer/VertexBuffer.h"
#include "renderer/opengl/OpenGLInclude.h"

namespace ion
{
	namespace render
	{
		class RendererOpenGL;

		class RendererOpenGL : public Renderer
		{
		public:
			//Create from existing DC
			RendererOpenGL(DeviceContext globalDeviceContext);

			//Create from existing GL context
			RendererOpenGL(DeviceContext globalDeviceContext, RenderContext renderContext);

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
			virtual void SetupViewport(const Viewport& viewport);
			virtual void SwapBuffers();
			virtual void SetClearColour(const Colour& colour);
			virtual void ClearColour();
			virtual void ClearDepth();
			virtual void EnableVSync(bool enabled);

			//Render states
			virtual void SetAlphaBlending(AlphaBlendType alphaBlendType);
			virtual void SetBlendColour(const Colour& colour);
			virtual void SetFaceCulling(CullingMode cullingMode);
			virtual void SetDepthTest(DepthTest depthTest);
			virtual void SetScissorTest(ScissorTest scissorTest);
			virtual void SetScissorRegion(const ion::Vector2i& position, const ion::Vector2i& size);
			virtual void SetLineWidth(float width);

			//Materials
			virtual void BindMaterial(Material& material, const Matrix4& worldMtx, const Matrix4& viewMtx, const Matrix4& projectionMtx);
			virtual void UnbindMaterial(Material& material);

			//Palettes
			virtual void LoadColourPalette(int paletteIdx, const std::vector<Colour>& palette);

			//Vertex buffer drawing
			virtual void DrawVertexBuffer(const VertexBuffer& vertexBuffer);
			virtual void DrawVertexBuffer(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer);

			//TODO: Mesh object
			virtual void DrawVertexBuffer(const VertexBuffer& compiledVertexBuffer, int indexOffset, int indexCount);

			//Check for OpenGL errors
			static bool CheckGLError(const char* message);

			//Threaded GL contexts
			static void LockGLContext();
			static void LockGLContext(const DeviceContext& deviceContext);
			static void UnlockGLContext();
			static bool CheckGLContext(const char* message = nullptr);

			//Vertex data type
			static int GetGLDataType(VertexBuffer::DataType dataType) { return s_glVertexDataTypes[(int)dataType]; }

		protected:
			static const int s_maxThreadContexts = 4;

			//Setup
			void InitPlatformGL(DeviceContext deviceContext);
			static RenderContext CreateContext(DeviceContext deviceContext, RenderContext sharedFrom);
			void InitContext(DeviceContext deviceContext);
			static bool SetThreadContext(DeviceContext deviceContext, RenderContext renderContext);

			//Drawing
			void BindVertexBuffer(const VertexBuffer& vertexBuffer);
			void UnbindVertexBuffer();

			//Main DC
			static DeviceContext s_globalDC;

			//Context threading
			static thread::LocalStorage<RenderContext, s_maxThreadContexts> s_threadContexts;
			static thread::LocalStorage<u32, RendererOpenGL::s_maxThreadContexts> s_contextLockStack;
			static thread::LocalStorage<DeviceContext, RendererOpenGL::s_maxThreadContexts> s_currentDC;

			Matrix4 m_projectionMatrix;

			static int s_glVertexDataTypes[(int)VertexBuffer::DataType::Count];
			static int s_glVertexPatternTypes[(int)VertexBuffer::Pattern::Count];
		};

		class OpenGLContextStackLock
		{
		public:
			OpenGLContextStackLock()
			{
				RendererOpenGL::LockGLContext();
			}

			~OpenGLContextStackLock()
			{
				RendererOpenGL::UnlockGLContext();
			}
		};
	}
}
