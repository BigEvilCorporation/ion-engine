///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		RendererOpenGL.cpp
// Date:		4th December 2013
// Authors:		Matt Phillips
// Description:	OpenGL renderer implementation
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "renderer/colour.h"
#include "renderer/Viewport.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Shader.h"
#include "renderer/opengl/RendererOpenGL.h"

#if defined ION_RENDER_SUPPORTS_CGGL
#include "renderer/cggl/ShaderCgGL.h"
#endif

#if defined ION_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace ion
{
	namespace render
	{
		//OpenGL extensions
#if defined ION_RENDER_SUPPORTS_GLEW
		PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
		PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
		PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
		PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
		PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
		PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
		PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
		PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
		PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
		PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
		PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
		PFNGLDRAWBUFFERSPROC glDrawBuffers;
#endif

		Renderer* Renderer::Create(DeviceContext globalDeviceContext)
		{
			return new RendererOpenGL(globalDeviceContext);
		}

		RendererOpenGL::RendererOpenGL(DeviceContext globalDeviceContext)
		{
			m_currentDC = NULL;
			m_contextLockStack = 0;

			//Using existing global DC
			m_globalDC = globalDeviceContext;

			//Create OpenGL context
			CreateContext(globalDeviceContext);

			//Init context
			InitContext(globalDeviceContext);
		}

		RendererOpenGL::~RendererOpenGL()
		{
#if defined ION_RENDERER_SHADER
			delete m_shaderManager;
#endif
		}

		void RendererOpenGL::CreateContext(DeviceContext deviceContext)
		{
			//Create OpenGL context
#if defined ION_PLATFORM_WINDOWS
			m_openGLContext = wglCreateContext(deviceContext);
			if(!m_openGLContext)
			{
				debug::Error("Could not create OpenGL context");
			}
#endif
		}

		void RendererOpenGL::InitContext(DeviceContext deviceContext)
		{
			//Lock context
			LockGLContext(deviceContext);

			//Intialise OpenGL extensions
#if defined ION_RENDER_SUPPORTS_GLEW
			glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
			glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
			glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
			glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
			glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
			glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
			glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
			glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
			glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
			glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
			glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
			glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
#endif

#if defined ION_RENDERER_KGL
			ion::debug::Log("\n\nINIT KOS\n\n");
			ion::debug::PrintMemoryUsage();

			//Init KOS GL
			glKosInit();

			ion::debug::Log("\n\nFINISHED INIT KOS\n\n");
			ion::debug::PrintMemoryUsage();
#endif

			//Background colour
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			//Enable depth testing
			glEnable(GL_DEPTH_TEST);

			//Depth buffer setup
			glClearDepth(1.0f);
			glDepthFunc(GL_LEQUAL);

			//Set default vertex winding
			glFrontFace(GL_CCW);

			//Enable back face culling
			glEnable(GL_CULL_FACE);

			//Shading and perspective
			glShadeModel(GL_SMOOTH);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

			//Set default blending mode
			glBlendFunc(GL_ONE, GL_ONE);

			//Check for OpenGL errors
			if(!CheckGLError())
			{
				debug::Error("Could not initialise OpenGL");
			}

			//Clear depth and colour
			ClearColour();
			ClearDepth();

			//Create shader manager
#if defined ION_RENDERER_SHADER
			m_shaderManager = ShaderManager::Create();
#endif

			//Unlock context (binds global DC)
			UnlockGLContext();
		}

		void RendererOpenGL::SetupViewport(const Viewport& viewport)
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			int width = viewport.GetWidth();
			int height = viewport.GetHeight();

			//Make sure width/height is at least 1
			debug::Assert(width > 0 && height > 0, "RendererOpenGL::SetupViewport() - Bad width/height");

			//Set the viewport
			glViewport(0, 0, width, height);

			//Select projection matrix
			glMatrixMode(GL_PROJECTION);

			//Reset projection matrix
			glLoadIdentity();

			//Calculate aspect ratio
			float aspectRatio = (float)width / (float)height;

			//Set perspective mode
			switch(viewport.GetPerspectiveMode())
			{
			case Viewport::ePerspective3D:
				//TODO: Expose FOV and near/far
				gluPerspective(45.0f, aspectRatio, 0.1f, 1000.0f);
				break;

			case Viewport::eOrtho2DNormalised:
				glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
				break;

			case Viewport::eOrtho2DAbsolute:
				glOrtho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
				break;
			}

			//Select the modelview matrix
			glMatrixMode(GL_MODELVIEW);

			//Reset the modelview matrix
			glLoadIdentity();

			//Setup default lighting
			float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);

			//Set clear colour
			SetClearColour(viewport.GetClearColour());
		}

		void RendererOpenGL::LockGLContext(DeviceContext deviceContext)
		{
			//m_contextCriticalSection.Begin();

			if(m_contextLockStack == 0)
			{
#if defined ION_PLATFORM_WINDOWS
				//Unbind global DC
				wglMakeCurrent(m_globalDC, NULL);

				//Make context current
				wglMakeCurrent(deviceContext, m_openGLContext);
#endif

				//Set current DC
				m_currentDC = deviceContext;
			}
			else
			{
				//Cannot switch DC if locked
				if(deviceContext != m_currentDC)
				{
					debug::Assert(m_contextLockStack == 0, "RendererOpenGL::LockGLContext() - Cannot change device context when locked");
				}
			}

			m_contextLockStack++;
		}

		void RendererOpenGL::UnlockGLContext()
		{
			debug::Assert(m_contextLockStack > 0, "Bad OpenGL context lock count");

			m_contextLockStack--;
			if(!m_contextLockStack)
			{
#if defined ION_PLATFORM_WINDOWS
				//Unbind current DC
				wglMakeCurrent(m_currentDC, NULL);

				//Bind global DC
				wglMakeCurrent(m_globalDC, m_openGLContext);
#endif

				m_currentDC = NULL;
			}

			//m_contextCriticalSection.End();
		}

		bool RendererOpenGL::CheckGLError()
		{
			GLenum error = glGetError();

			if(error != GL_NO_ERROR)
			{
				const char* errString = (const char*)gluErrorString(error);
				debug::Log("OpenGL error");
				debug::Error(errString);
			}

			return error == GL_NO_ERROR;
		}

		bool RendererOpenGL::Update(float deltaTime)
		{
			return true;
		}

		void RendererOpenGL::OnResize(int width, int height)
		{

		}

		void RendererOpenGL::SetMatrix(const Matrix4& matrix)
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");
			glLoadMatrixf(matrix.GetAsFloatArray());
		}

		Matrix4 RendererOpenGL::GetProjectionMatrix()
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			float matrix[16] = { 0.0f };
			glGetFloatv(GL_PROJECTION_MATRIX, matrix);
			return Matrix4(matrix);
		}

		void RendererOpenGL::BeginFrame(const Viewport& viewport, const DeviceContext& deviceContext)
		{
			LockGLContext(deviceContext);
			SetupViewport(viewport);
		}

		void RendererOpenGL::EndFrame()
		{
			UnlockGLContext();
		}

		void RendererOpenGL::SwapBuffers()
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

#if defined ION_PLATFORM_WINDOWS
			::SwapBuffers(m_currentDC);
#elif defined ION_PLATFORM_DREAMCAST
			glutSwapBuffers();
#endif
		}

		void RendererOpenGL::SetClearColour(const Colour& colour)
		{
			glClearColor(colour.r, colour.g, colour.b, colour.a);
		}

		void RendererOpenGL::ClearColour()
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			//Clear colour buffer
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void RendererOpenGL::ClearDepth()
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			//Clear depth buffer
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		void RendererOpenGL::SetAlphaBlending(AlphaBlendType alphaBlendType)
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			switch(alphaBlendType)
			{
			case eNoBlend:
				glDisable(GL_BLEND);
				break;

			case eAdditive:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;

			case eTranslucent:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;

			default:
				break;
			}
		}

		void RendererOpenGL::SetFaceCulling(CullingMode cullingMode)
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			switch(cullingMode)
			{
			case eNoCull:
				glDisable(GL_CULL_FACE);
				break;

			case eClockwise:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;

			case eCounterClockwise:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;

			default:
				break;
			}
		}

		void RendererOpenGL::SetDepthTest(DepthTest depthTest)
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			if(depthTest == eDisabled)
			{
				glDisable(GL_DEPTH_TEST);
			}
			else
			{
				glEnable(GL_DEPTH_TEST);

				switch(depthTest)
				{
				case eAlways:
					glDepthFunc(GL_ALWAYS);
					break;
				case eLessEqual:
					glDepthFunc(GL_LEQUAL);
					break;
				}
			}
		}

		void RendererOpenGL::SetLineWidth(float width)
		{
			//TODO: Dreamcast
#if !defined ION_PLATFORM_DREAMCAST
			glLineWidth(width);
#endif
		}

		void RendererOpenGL::DrawVertexBuffer(const VertexBuffer& vertexBuffer)
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			//Enable client states
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			//Set element pointers
			glVertexPointer(vertexBuffer.GetVertexSize(), GL_FLOAT, vertexBuffer.GetStrideBytes(), vertexBuffer.GetVertexBuffer());
			glNormalPointer(GL_FLOAT, vertexBuffer.GetStrideBytes(), vertexBuffer.GetNormalBuffer());
			glTexCoordPointer(vertexBuffer.GetTexCoordSize(), GL_FLOAT, vertexBuffer.GetStrideBytes(), vertexBuffer.GetTexCoordBuffer());
			glColorPointer(vertexBuffer.GetColourSize(), GL_FLOAT, vertexBuffer.GetStrideBytes(), vertexBuffer.GetColourBuffer());

			//Determine pattern type
			int drawPattern = 0;
			int numVertices = vertexBuffer.GetNumVerts();

			switch(vertexBuffer.GetPattern())
			{
			case VertexBuffer::eLines:
				drawPattern = GL_LINES;
				break;

			case VertexBuffer::eLineStrip:
				drawPattern = GL_LINE_STRIP;
				break;

			case VertexBuffer::eTriangles:
				drawPattern = GL_TRIANGLES;
				break;

			case VertexBuffer::eQuads:
				drawPattern = GL_QUADS;
				break;

			default:
				debug::Error("Invalid vertex pattern");
			}

			//Draw
			glDrawArrays(drawPattern, 0, vertexBuffer.GetNumVerts());

			//Disable client states
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);

			if(!CheckGLError())
			{
				debug::Error("Could not draw vertex buffer");
			}
		}

		void RendererOpenGL::DrawVertexBuffer(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer)
		{
			debug::Assert(m_contextLockStack > 0, "OpenGL context is not locked");

			//Enable client states
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			//Set element pointers
			glVertexPointer(vertexBuffer.GetVertexSize(), GL_FLOAT, vertexBuffer.GetStrideBytes(), vertexBuffer.GetVertexBuffer());
			glNormalPointer(GL_FLOAT, vertexBuffer.GetStrideBytes(), vertexBuffer.GetNormalBuffer());
			glTexCoordPointer(vertexBuffer.GetTexCoordSize(), GL_FLOAT, vertexBuffer.GetStrideBytes(), vertexBuffer.GetTexCoordBuffer());
			glColorPointer(vertexBuffer.GetColourSize(), GL_FLOAT, vertexBuffer.GetStrideBytes(), vertexBuffer.GetColourBuffer());

			//Determine pattern type
			int drawPattern = 0;
			int numVertices = vertexBuffer.GetNumVerts();

			switch(vertexBuffer.GetPattern())
			{
			case VertexBuffer::eTriangles:
				drawPattern = GL_TRIANGLES;
				break;

			case VertexBuffer::eQuads:
				drawPattern = GL_QUADS;
				break;

			default:
				debug::Error("Invalid number of vertices");
			}

			//Draw
#if defined ION_PLATFORM_WINDOWS
			glDrawElements(drawPattern, indexBuffer.GetSize(), GL_UNSIGNED_INT, indexBuffer.GetAddress());
#elif defined ION_PLATFORM_DREAMCAST
			glDrawElements(drawPattern, indexBuffer.GetSize(), GL_UNSIGNED_SHORT, indexBuffer.GetAddress());
#endif

			//Disable client states
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);

			CheckGLError();
		}
	}
}
