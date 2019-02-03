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
#include "renderer/Colour.h"
#include "renderer/Viewport.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Shader.h"
#include "renderer/opengl/RendererOpenGL.h"
#include "renderer/opengl/OpenGLExtensions.h"

#define ION_RENDERER_LOG_WARNINGS 0
#define ION_RENDERER_WARNING_AS_ERROR 0

namespace ion
{
	namespace render
	{
#if defined DEBUG
#if defined ION_PLATFORM_WINDOWS
		void OnOpenGLError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void *userParam)
		{
#if !ION_RENDERER_LOG_WARNINGS
			if (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
#endif
			{
#if !ION_RENDERER_WARNING_AS_ERROR
				if (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
#endif
				{
					debug::error
						<< "==============================================================\n"
						<< "OpenGL error: \n"
						<< "==============================================================\n"
						<< "GLenum source = " << source << "\n"
						<< "GLenum type = " << type << "\n"
						<< "GLuint id = " << id << "\n"
						<< "GLenum severity = " << severity << "\n"
						<< "GLsizei length = " << length << "\n"
						<< "const GLchar* message = " << message << "\n"
						<< "==============================================================\n"
						<< debug::end;
				}
#if !ION_RENDERER_WARNING_AS_ERROR
				else
				{
					debug::log
						<< "==============================================================\n"
						<< "OpenGL warning: \n"
						<< "==============================================================\n"
						<< "GLenum source = " << source << "\n"
						<< "GLenum type = " << type << "\n"
						<< "GLuint id = " << id << "\n"
						<< "GLenum severity = " << severity << "\n"
						<< "GLsizei length = " << length << "\n"
						<< "const GLchar* message = " << message << "\n"
						<< "==============================================================\n"
						<< debug::end;
				}
#endif
			}
		}
#endif
#endif

		RenderContext RendererOpenGL::s_globalContext = 0;
		DeviceContext RendererOpenGL::s_globalDC = 0;
		RenderContext RendererOpenGL::s_currentContext = 0;
		DeviceContext RendererOpenGL::s_currentDC = 0;
		thread::LocalStorage<RenderContext, RendererOpenGL::s_maxThreadContexts> RendererOpenGL::s_threadContexts;
		thread::CriticalSection RendererOpenGL::s_contextCriticalSection;
		u32 RendererOpenGL::s_contextLockStack = 0;

		int RendererOpenGL::s_glVertexDataTypes[VertexBuffer::eDataTypeCount] =
		{
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
			GL_FLOAT
		};

		Renderer* Renderer::Create(DeviceContext globalDeviceContext)
		{
			return new RendererOpenGL(globalDeviceContext);
		}

		Renderer* Renderer::Create(DeviceContext globalDeviceContext, RenderContext renderContext)
		{
			return new RendererOpenGL(globalDeviceContext, renderContext);
		}

		RendererOpenGL::RendererOpenGL(DeviceContext globalDeviceContext)
		{
			//Can only have one global DC and context
			debug::Assert(s_globalDC == NULL, "RendererOpenGL::RendererOpenGL() - Global context already exists, create a shared context instead");
			debug::Assert(s_globalContext == NULL, "RendererOpenGL::RendererOpenGL() - Global context already exists, create a shared context instead");

			//Intialise OpenGL extensions
			OpenGLExt::LoadExtensions(globalDeviceContext);

			//Using existing global DC
			s_globalDC = globalDeviceContext;

			//Create OpenGL context
			s_globalContext = CreateContext(globalDeviceContext, 0);

			//Set for this thread
			s_threadContexts.SetData(s_globalContext);

			//Create shared contexts for other threads, store in TLS
			for (int i = 1; i < s_maxThreadContexts; i++)
			{
				RenderContext renderContext = CreateContext(s_globalDC, s_globalContext);
				s_threadContexts.SetData(renderContext, i);
			}

			//Init context
			InitContext(globalDeviceContext);
		}

		RendererOpenGL::RendererOpenGL(DeviceContext globalDeviceContext, RenderContext renderContext)
		{
			//Can only have one global DC and context
			debug::Assert(s_globalDC == NULL, "RendererOpenGL::RendererOpenGL() - Global context already exists, create a shared context instead");
			debug::Assert(s_globalContext == NULL, "RendererOpenGL::RendererOpenGL() - Global context already exists, create a shared context instead");

			//Intialise OpenGL extensions
			OpenGLExt::LoadExtensions(globalDeviceContext);

			//Using existing global DC
			s_globalDC = globalDeviceContext;

			//Using existing GL context
			s_globalContext = renderContext;

			//Set for this thread
			s_threadContexts.SetData(renderContext);

			//Create shared contexts for other threads, store in TLS
			for (int i = 1; i < s_maxThreadContexts; i++)
			{
				RenderContext renderContext = CreateContext(s_globalDC, s_globalContext);
				s_threadContexts.SetData(renderContext, i);
			}

			//Init context
			InitContext(globalDeviceContext);
		}

		RendererOpenGL::~RendererOpenGL()
		{
#if defined ION_RENDERER_SHADER
			delete m_shaderManager;
#endif
		}

		RenderContext RendererOpenGL::CreateContext(DeviceContext deviceContext, RenderContext sharedFrom)
		{
			//Create OpenGL context
			RenderContext renderContext;

#if defined ION_PLATFORM_WINDOWS
			if (OpenGLExt::wglCreateContextAttribs)
			{
				int flags[] = 
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
					WGL_CONTEXT_MINOR_VERSION_ARB, 0,
#if defined DEBUG
					WGL_CONTEXT_FLAGS_ARB,  WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
					0
				};

				renderContext = OpenGLExt::wglCreateContextAttribs(deviceContext, sharedFrom, flags);
			}
			else
			{
				renderContext = wglCreateContext(deviceContext);

				if (sharedFrom)
				{
					//Share with another context
					wglShareLists(sharedFrom, renderContext);
				}
			}
#elif defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_LINUX || defined ION_PLATFORM_RASPBERRYPI
            if(sharedFrom)
            {
                LockGLContext();
                SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
            }
            
			renderContext = SDL_GL_CreateContext(deviceContext);
            
            if(sharedFrom)
            {
                UnlockGLContext();
            }
#elif defined ION_PLATFORM_DREAMCAST
			renderContext = 1;
#endif

            if(!renderContext)
            {
                debug::Error("Could not create OpenGL context");
            }

			return renderContext;
		}

		void RendererOpenGL::InitContext(DeviceContext deviceContext)
		{
			//Lock context
			LockContext(deviceContext);

			//Output version
			const GLubyte* vendor = (const GLubyte*)glGetString(GL_VENDOR);
			const GLubyte* renderer = (const GLubyte*)glGetString(GL_RENDERER);
			const GLubyte* version = (const GLubyte*)glGetString(GL_VERSION);
			const GLubyte* extensions = (const GLubyte*)glGetString(GL_EXTENSIONS);
			debug::log << "OpenGL vendor: " << (const char*)vendor << debug::end;
			debug::log << "OpenGL device: " << (const char*)renderer << debug::end;
			debug::log << "OpenGL version: " << (const char*)version << debug::end;
			//debug::log << "OpenGL extensions: " << (const char*)extensions << debug::end;

			//Set error callback
#if defined DEBUG && defined ION_PLATFORM_WINDOWS && defined ION_PLATFORM_64BIT
			if (OpenGLExt::glDebugMessageCallback)
			{
				glEnable(GL_DEBUG_OUTPUT);
				OpenGLExt::glDebugMessageCallback(OnOpenGLError, nullptr);
			}
#endif

#if defined ION_RENDERER_KGL
			int ramBefore = debug::GetRAMUsed();

			//Init KOS GL
			glKosInit();

			int ramAfter = debug::GetRAMUsed();
			ion::debug::log << "Initialised KGL, used memory: " << ramAfter << " bytes (" << (float)ramAfter / (1024.0f*1024.0f) << "mb)" << debug::end;
#endif

			//Background colour
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			//Enable depth testing
			glEnable(GL_DEPTH_TEST);

			//Depth buffer setup
			#if !defined ION_RENDERER_OPENGLES
			glClearDepth(1.0f);
			#endif

			glDepthFunc(GL_LEQUAL);

			//Set default vertex winding
			glFrontFace(GL_CCW);

			//Enable back face culling
			glEnable(GL_CULL_FACE);

			//Shading and perspective
			glShadeModel(GL_SMOOTH);

#if !defined ION_RENDERER_OPENGLES
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif

			//Set default blending mode
			glBlendFunc(GL_ONE, GL_ONE);

			//Check for OpenGL errors
			if(!CheckGLError("RendererOpenGL::InitContext()"))
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
			UnlockContext();
		}

		void RendererOpenGL::SetupViewport(const Viewport& viewport)
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

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
				{
					//TODO: Expose FOV and near/far
					m_projectionMatrix = ion::Matrix4(45.0f, aspectRatio, 0.1f, 1000.0f);
					glLoadMatrixf(m_projectionMatrix.GetAsFloatArray());
					break;
				}

				case Viewport::eOrtho2DNormalised:
				{
					m_projectionMatrix = ion::Matrix4(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
					glLoadMatrixf(m_projectionMatrix.GetAsFloatArray());
					break;
				}

				case Viewport::eOrtho2DAbsolute:
				{
					m_projectionMatrix = ion::Matrix4(0.0f, width, 0.0f, height, -1.0f, 1.0f);
					glLoadMatrixf(m_projectionMatrix.GetAsFloatArray());
					break;
				}
			}

			//Select the modelview matrix
			glMatrixMode(GL_MODELVIEW);

			//Reset the modelview matrix
			glLoadIdentity();

			CheckGLError("RendererOpenGL::SetupViewport");

			//Set clear colour
			SetClearColour(viewport.GetClearColour());
		}

		void RendererOpenGL::LockContext(const DeviceContext& deviceContext)
		{
			//Lock context against provided DC
			LockGLContext(deviceContext);
		}

		void RendererOpenGL::UnlockContext()
		{
			UnlockGLContext();
		}

		void RendererOpenGL::LockGLContext()
		{
			//Lock context against global DC
			LockGLContext(s_globalDC);
		}

		void RendererOpenGL::LockGLContext(const DeviceContext& deviceContext)
		{
#if defined ION_PLATFORM_DREAMCAST
			//TEMP
			RenderContext renderContext = 0;
#else
			//If context not allocated for this thread, allocate now
			if (!s_threadContexts.IsAllocated())
			{
				s_threadContexts.Allocate();
			}

			//Get context for this thread
			RenderContext renderContext = s_threadContexts.GetData();
#endif

#if !defined ION_PLATFORM_DREAMCAST
			//TEMP
			s_contextCriticalSection.Begin();
#endif

			if (s_contextLockStack == 0)
			{
				if (deviceContext != s_currentDC)
				{
					//Make context current
#if defined ION_PLATFORM_WINDOWS
					wglMakeCurrent(deviceContext, renderContext);
#elif defined ION_PLATFORM_LINUX
                    SDL_GL_MakeCurrent(deviceContext, renderContext);
#endif
				}

				//Set current DC
				s_currentDC = deviceContext;
			}
			else
			{
				//Cannot switch DC if locked
				if (deviceContext != s_currentDC)
				{
					debug::Assert(s_contextLockStack == 0, "RendererOpenGL::LockGLContext() - Cannot change device context when locked");
				}
			}

			CheckGLError("RendererOpenGL::LockContext");

			s_contextLockStack++;
		}

		void RendererOpenGL::UnlockGLContext()
		{
			debug::Assert(s_contextLockStack > 0, "Bad OpenGL context lock count");
			CheckGLError("RendererOpenGL::UnlockContext");

			s_contextLockStack--;
			if (!s_contextLockStack)
			{
				if (s_currentDC != s_globalDC)
				{
					//Unbind current DC
#if defined ION_PLATFORM_WINDOWS
					wglMakeCurrent(s_currentDC, NULL);
#elif defined ION_PLATFORM_LINUX
                    SDL_GL_MakeCurrent(s_currentDC, NULL);
#endif
				}

				s_currentDC = NULL;
			}

#if !defined ION_PLATFORM_DREAMCAST
			//TEMP
			s_contextCriticalSection.End();
#endif
		}

		bool RendererOpenGL::CheckGLError(const char* message)
		{
#if defined DEBUG
			if (s_contextLockStack > 0)
			{
				GLenum error = glGetError();

				if (error != GL_NO_ERROR)
				{
#if defined ION_RENDERER_OPENGLES
					debug::error << message << " - OpenGL error: " << error << debug::end;
#else
					const char* errString = (const char*)gluErrorString(error);
					debug::error << message << " - OpenGL error: " << error << " - " << errString << debug::end;
#endif
				}

				return error == GL_NO_ERROR;
			}
			else
			{
				return true;
			}
#else
			return true;
#endif
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
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");
			glLoadMatrixf(matrix.GetAsFloatArray());
			CheckGLError("RendererOpenGL::SetMatrix");
		}

		Matrix4 RendererOpenGL::GetProjectionMatrix()
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");
			return m_projectionMatrix;
		}

		void RendererOpenGL::BeginFrame(const Viewport& viewport, const DeviceContext& deviceContext)
		{
			LockContext(deviceContext);
			SetupViewport(viewport);
		}

		void RendererOpenGL::EndFrame()
		{
			UnlockContext();
		}

		void RendererOpenGL::SwapBuffers()
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

#if defined ION_PLATFORM_WINDOWS
			::SwapBuffers(s_currentDC);
#elif defined ION_PLATFORM_MACOSX
			SDL_GL_SwapWindow(s_currentDC);
#elif defined ION_PLATFORM_LINUX
			SDL_GL_SwapWindow(s_currentDC);
#elif defined ION_PLATFORM_RASPBERRYPI
			SDL_GL_SwapWindow(s_currentDC);
#elif defined ION_PLATFORM_DREAMCAST
			glutSwapBuffers();
#endif

			CheckGLError("RendererOpenGL::SwapBuffers");
		}

		void RendererOpenGL::SetClearColour(const Colour& colour)
		{
			glClearColor(colour.r, colour.g, colour.b, colour.a);

			CheckGLError("RendererOpenGL::SetClearColour");
		}

		void RendererOpenGL::ClearColour()
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

			//Clear colour buffer
			glClear(GL_COLOR_BUFFER_BIT);

			CheckGLError("RendererOpenGL::ClearColour");
		}

		void RendererOpenGL::ClearDepth()
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

			//Clear depth buffer
			glClear(GL_DEPTH_BUFFER_BIT);

			CheckGLError("RendererOpenGL::ClearDepth");
		}

		void RendererOpenGL::SetAlphaBlending(AlphaBlendType alphaBlendType)
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

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

#if defined ION_PLATFORM_WINDOWS
			case eColourFilter:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ZERO, GL_CONSTANT_COLOR);
				break;
#endif

			default:
				break;
			}

			CheckGLError("RendererOpenGL::SetAlphaBlending");
		}

		void RendererOpenGL::SetBlendColour(const Colour& colour)
		{
#if defined ION_PLATFORM_WINDOWS
			OpenGLExt::glBlendColor(colour.r, colour.g, colour.b, colour.a);
#endif
		}

		void RendererOpenGL::SetFaceCulling(CullingMode cullingMode)
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

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

			CheckGLError("RendererOpenGL::SetFaceCulling");
		}

		void RendererOpenGL::SetDepthTest(DepthTest depthTest)
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

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

			CheckGLError("RendererOpenGL::SetDepthTest");
		}

		void RendererOpenGL::SetLineWidth(float width)
		{
			//TODO: Dreamcast
#if !defined ION_PLATFORM_DREAMCAST
			glLineWidth(width);
#endif

			CheckGLError("RendererOpenGL::SetLineWidth");
		}

		void RendererOpenGL::EnableVSync(bool enabled)
		{
#if defined ION_PLATFORM_WINDOWS
			if (OpenGLExt::glSwapInterval)
			{
				OpenGLExt::glSwapInterval(enabled ? 1 : 0);
			}
#endif
		}

		void RendererOpenGL::LoadColourPalette(int paletteIdx, const std::vector<Colour>& palette)
		{
#if defined ION_PLATFORM_DREAMCAST
			ion::debug::Assert(paletteIdx >= 0 && paletteIdx < GL_COLOR_TABLE_COUNT, "RenderereOpenGL::LoadColourPalette() - Palette out of range");

			std::vector<u32> data;
			data.resize(palette.size());

			for (int i = 0; i < palette.size(); i++)
			{
				data[i] = palette[i].AsARGB();
			}

			glColorTable(GL_COLOR_TABLE + paletteIdx, GL_RGBA, palette.size(), GL_RGBA, GL_UNSIGNED_INT, data.data());
#endif
		}

		void RendererOpenGL::DrawVertexBuffer(const VertexBuffer& vertexBuffer)
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

			//Enable client states and set element pointers
			int positionSize = vertexBuffer.GetElementNumComponents(VertexBuffer::ePosition);
			int normalSize = vertexBuffer.GetElementNumComponents(VertexBuffer::eNormal);
			int colourSize = vertexBuffer.GetElementNumComponents(VertexBuffer::eColour);
			int texCoordSize = vertexBuffer.GetElementNumComponents(VertexBuffer::eTexCoord);

			if (positionSize)
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(positionSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::ePosition)], vertexBuffer.GetStrideBytes(), vertexBuffer.GetStartAddress(VertexBuffer::ePosition));
			}

			if (normalSize)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::eNormal)], vertexBuffer.GetStrideBytes(), vertexBuffer.GetStartAddress(VertexBuffer::eNormal));
			}

			if (colourSize)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(colourSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::eColour)], vertexBuffer.GetStrideBytes(), vertexBuffer.GetStartAddress(VertexBuffer::eColour));
			}

			if (texCoordSize)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(texCoordSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::eTexCoord)], vertexBuffer.GetStrideBytes(), vertexBuffer.GetStartAddress(VertexBuffer::eTexCoord));
			}

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

			if(!CheckGLError("RendererOpenGL::DrawVertexBuffer(const VertexBuffer&)"))
			{
				debug::Error("Could not draw vertex buffer");
			}
		}

		void RendererOpenGL::DrawVertexBuffer(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer)
		{
			debug::Assert(s_contextLockStack > 0, "OpenGL context is not locked");

			//Enable client states and set element pointers
			int positionSize = vertexBuffer.GetElementNumComponents(VertexBuffer::ePosition);
			int normalSize = vertexBuffer.GetElementNumComponents(VertexBuffer::eNormal);
			int colourSize = vertexBuffer.GetElementNumComponents(VertexBuffer::eColour);
			int texCoordSize = vertexBuffer.GetElementNumComponents(VertexBuffer::eTexCoord);

			if (positionSize)
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(positionSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::ePosition)], vertexBuffer.GetStrideBytes(), vertexBuffer.GetStartAddress(VertexBuffer::ePosition));
			}

			if (normalSize)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::eNormal)], vertexBuffer.GetStrideBytes(), vertexBuffer.GetStartAddress(VertexBuffer::eNormal));
			}

			if (colourSize)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(colourSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::eColour)], vertexBuffer.GetStrideBytes(), vertexBuffer.GetStartAddress(VertexBuffer::eColour));
			}

			if (texCoordSize)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(texCoordSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::eTexCoord)], vertexBuffer.GetStrideBytes(), vertexBuffer.GetStartAddress(VertexBuffer::eTexCoord));
			}

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
#elif defined ION_PLATFORM_MACOSX
            glDrawElements(drawPattern, indexBuffer.GetSize(), GL_UNSIGNED_INT, indexBuffer.GetAddress());
#elif defined ION_PLATFORM_LINUX
            glDrawElements(drawPattern, indexBuffer.GetSize(), GL_UNSIGNED_INT, indexBuffer.GetAddress());
#elif defined ION_PLATFORM_RASPBERRYPI
			glDrawElements(drawPattern, indexBuffer.GetSize(), GL_UNSIGNED_SHORT, indexBuffer.GetAddress());
#elif defined ION_PLATFORM_DREAMCAST
			glDrawElements(drawPattern, indexBuffer.GetSize(), GL_UNSIGNED_SHORT, indexBuffer.GetAddress());
#endif

			//Disable client states
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);

			CheckGLError("RendererOpenGL::DrawVertexBuffer(const VertexBuffer&, const IndexBuffer&)");
		}
	}
}
