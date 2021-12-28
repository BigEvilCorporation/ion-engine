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
#include "renderer/Material.h"
#include "renderer/Viewport.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Shader.h"
#include "renderer/opengl/OpenGLVersion.h"
#include "renderer/opengl/RendererOpenGL.h"
#include "renderer/opengl/OpenGLExtensions.h"

namespace ion
{
	namespace render
	{
		DeviceContext RendererOpenGL::s_globalDC = DeviceContext();
		thread::LocalStorage<DeviceContext, RendererOpenGL::s_maxThreadContexts> RendererOpenGL::s_currentDC;
		thread::LocalStorage<RenderContext, RendererOpenGL::s_maxThreadContexts> RendererOpenGL::s_threadContexts;
		thread::LocalStorage<u32, RendererOpenGL::s_maxThreadContexts> RendererOpenGL::s_contextLockStack;

		int RendererOpenGL::s_glVertexDataTypes[(int)VertexBuffer::DataType::Count] =
		{
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_SHORT,
#if defined ION_RENDERER_OPENGL_ES
			0,
#else
			GL_UNSIGNED_INT,
#endif
			GL_FLOAT
		};

		int RendererOpenGL::s_glVertexPatternTypes[(int)VertexBuffer::Pattern::Count] =
		{
			GL_LINES,
			GL_LINE_STRIP,
			GL_TRIANGLES,
#if defined ION_RENDERER_OPENGL_ES
			0,
#else
			GL_QUADS
#endif
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
			debug::Assert(!s_globalDC, "RendererOpenGL::RendererOpenGL() - Global context already exists, create a shared context instead");

			//Initialise platform specific GL lib
			InitPlatformGL(globalDeviceContext);

			//Intialise OpenGL extensions
			opengl::LoadExtensions(globalDeviceContext);

			//Using existing global DC
			s_globalDC = globalDeviceContext;

			//Create OpenGL context
			RenderContext mainRenderContext = CreateContext(globalDeviceContext, 0);

			//Set for this thread
			s_threadContexts.SetData(mainRenderContext);
			s_currentDC.SetData(s_globalDC);
			s_contextLockStack.SetData(0);

			//Create shared contexts for other threads, store in TLS
			for (int i = 1; i < s_maxThreadContexts; i++)
			{
				RenderContext renderContext = CreateContext(s_globalDC, mainRenderContext);
				s_threadContexts.SetData(renderContext, i);
				s_currentDC.SetData(s_globalDC, i);
				s_contextLockStack.SetData(0, i);
			}

			//Init context
			InitContext(globalDeviceContext);
		}

		RendererOpenGL::RendererOpenGL(DeviceContext globalDeviceContext, RenderContext mainRenderContext)
		{
			//Can only have one global DC and context
			debug::Assert(!s_globalDC, "RendererOpenGL::RendererOpenGL() - Global context already exists, create a shared context instead");

			//Initialise platform specific GL lib
			InitPlatformGL(globalDeviceContext);

			//Intialise OpenGL extensions
			opengl::LoadExtensions(globalDeviceContext);

			//Using existing global DC
			s_globalDC = globalDeviceContext;

			//Set for this thread
			s_threadContexts.SetData(mainRenderContext);
			s_currentDC.SetData(s_globalDC);
			s_contextLockStack.SetData(0);

			//Create shared contexts for other threads, store in TLS
			for (int i = 1; i < s_maxThreadContexts; i++)
			{
				RenderContext renderContext = CreateContext(s_globalDC, mainRenderContext);
				s_threadContexts.SetData(renderContext, i);
				s_currentDC.SetData(s_globalDC, i);
				s_contextLockStack.SetData(0, i);
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

		void RendererOpenGL::InitContext(DeviceContext deviceContext)
		{
			//Lock context
			LockContext(deviceContext);

			//Output version
			const GLubyte* vendor = (const GLubyte*)glGetString(GL_VENDOR);
			const GLubyte* renderer = (const GLubyte*)glGetString(GL_RENDERER);
			const GLubyte* version = (const GLubyte*)glGetString(GL_VERSION);
			debug::log << "OpenGL vendor: " << (const char*)vendor << debug::end;
			debug::log << "OpenGL device: " << (const char*)renderer << debug::end;
			debug::log << "OpenGL version: " << (const char*)version << debug::end;

			//Background colour
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			//Enable depth testing
			glEnable(GL_DEPTH_TEST);

			//Depth buffer setup
			#if !defined ION_RENDERER_OPENGL_ES
			glClearDepth(1.0f);
			#endif

			glDepthFunc(GL_LEQUAL);

			//Set default vertex winding
			glFrontFace(GL_CCW);

			//Enable back face culling
			glEnable(GL_CULL_FACE);

			//Shading and perspective
#if defined ION_RENDERER_OPENGL_CORE && defined ION_RENDERER_FIXED
			glShadeModel(GL_SMOOTH);

			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif

			//Set default blending mode
			glBlendFunc(GL_ONE, GL_ONE);

			//Disable lighting
#if defined ION_RENDERER_OPENGL_CORE && defined ION_RENDERER_FIXED
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHTING);
#endif

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
			CheckGLContext();

			int width = viewport.GetWidth();
			int height = viewport.GetHeight();

			//Make sure width/height is at least 1
			debug::Assert(width > 0 && height > 0, "RendererOpenGL::SetupViewport() - Bad width/height");

			//Set the viewport
			glViewport(0, 0, width, height);

			//Select projection matrix
#if defined ION_RENDERER_OPENGL_CORE && defined ION_RENDERER_FIXED
			glMatrixMode(GL_PROJECTION);

			//Reset projection matrix
			glLoadIdentity();
#endif

			//Calculate aspect ratio
			float aspectRatio = (float)width / (float)height;

			//Set perspective mode
			switch(viewport.GetPerspectiveMode())
			{
				case Viewport::PerspectiveMode::Perspective3D:
				{
					//TODO: Expose FOV and near/far
					m_projectionMatrix = ion::Matrix4(aspectRatio, 45.0f, 0.1f, 1000.0f);
#if defined ION_RENDERER_OPENGL_CORE && defined ION_RENDERER_FIXED
					glLoadMatrixf(m_projectionMatrix.GetAsFloatArray());
#endif
					break;
				}

				case Viewport::PerspectiveMode::Ortho2DNormalised:
				{
					m_projectionMatrix = ion::Matrix4(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
#if defined ION_RENDERER_OPENGL_CORE && defined ION_RENDERER_FIXED
					glLoadMatrixf(m_projectionMatrix.GetAsFloatArray());
#endif
					break;
				}

				case Viewport::PerspectiveMode::Ortho2DAbsolute:
				{
					m_projectionMatrix = ion::Matrix4(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
#if defined ION_RENDERER_OPENGL_CORE && defined ION_RENDERER_FIXED
					glLoadMatrixf(m_projectionMatrix.GetAsFloatArray());
#endif
					break;
				}
			}

#if defined ION_RENDERER_OPENGL_CORE && defined ION_RENDERER_FIXED
			//Select the modelview matrix
			glMatrixMode(GL_MODELVIEW);

			//Reset the modelview matrix
			glLoadIdentity();
#endif

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
			//If context not allocated for this thread, allocate now
			if (!s_threadContexts.IsAllocated())
			{
				s_threadContexts.Allocate();
				s_currentDC.Allocate();
				s_contextLockStack.Allocate();
			}

			//Lock context against current DC
			LockGLContext(s_currentDC.GetData());
		}

		void RendererOpenGL::LockGLContext(const DeviceContext& deviceContext)
		{
			CheckGLError("RendererOpenGL::LockContext");

#if defined ION_PLATFORM_DREAMCAST
			//TEMP
			RenderContext renderContext = 0;
#else
			//If context not allocated for this thread, allocate now
			if (!s_threadContexts.IsAllocated())
			{
				s_threadContexts.Allocate();
				s_currentDC.Allocate();
				s_contextLockStack.Allocate();
			}

			//Get context for this thread
			RenderContext renderContext = s_threadContexts.GetData();
#endif

			u32 lockCount = s_contextLockStack.GetData();
			if (lockCount == 0 || deviceContext != s_currentDC.GetData())
			{
				//Make context current
				SetThreadContext(deviceContext, renderContext);

				//Set current DC
				s_currentDC.SetData(deviceContext);
			}

			CheckGLError("RendererOpenGL::LockContext");

			s_contextLockStack.SetData(lockCount + 1);
		}

		void RendererOpenGL::UnlockGLContext()
		{
			CheckGLContext();
			CheckGLError("RendererOpenGL::UnlockContext");

			u32 lockCount = s_contextLockStack.GetData();

			lockCount--;
			if (!lockCount)
			{
				//Unbind current DC
				SetThreadContext(s_currentDC.GetData(), NullRenderContext);
				s_currentDC.SetData(s_globalDC);
			}

			s_contextLockStack.SetData(lockCount);
		}

		bool RendererOpenGL::CheckGLContext(const char* message)
		{
			if (s_contextLockStack.GetData() == 0)
			{
				if(message)
					ion::debug::error << "OpenGL context is not locked : " << message << ion::debug::end;
				else
					ion::debug::error << "OpenGL context is not locked" << ion::debug::end;

				return false;
			}

			return true;
		}

		bool RendererOpenGL::CheckGLError(const char* message)
		{
#if defined DEBUG
			if (s_contextLockStack.GetData() > 0)
			{
				GLenum error = glGetError();

				if (error != GL_NO_ERROR)
				{
#if defined ION_RENDERER_OPENGL_ES || defined ION_PLATFORM_SWITCH
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
#if defined ION_RENDERER_OPENGL_CORE
			CheckGLContext();
			glLoadMatrixf((float*)&matrix.GetAsFloatArray()[0]);
			CheckGLError("RendererOpenGL::SetMatrix");
#endif
		}

		Matrix4 RendererOpenGL::GetProjectionMatrix()
		{
			CheckGLContext();
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

		void RendererOpenGL::SetClearColour(const Colour& colour)
		{
			glClearColor(colour.r, colour.g, colour.b, colour.a);

			CheckGLError("RendererOpenGL::SetClearColour");
		}

		void RendererOpenGL::ClearColour()
		{
			CheckGLContext();

			//Clear colour buffer
			glClear(GL_COLOR_BUFFER_BIT);

			CheckGLError("RendererOpenGL::ClearColour");
		}

		void RendererOpenGL::ClearDepth()
		{
			CheckGLContext();

			//Clear depth buffer
			glClear(GL_DEPTH_BUFFER_BIT);

			CheckGLError("RendererOpenGL::ClearDepth");
		}

		void RendererOpenGL::SetAlphaBlending(AlphaBlendType alphaBlendType)
		{
			CheckGLContext();

			switch(alphaBlendType)
			{
			case AlphaBlendType::None:
				glDisable(GL_BLEND);
				break;

			case AlphaBlendType::Additive:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;

			case AlphaBlendType::Translucent:
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;

#if defined ION_PLATFORM_WINDOWS
			case AlphaBlendType::ColourFilter:
				glEnable(GL_BLEND);
				glBlendFunc(GL_ZERO, GL_CONSTANT_COLOR);
				break;
#endif

			default:
				break;
			}

			CheckGLError("RendererOpenGL::SetAlphaBlending");
		}

		void RendererOpenGL::SetFaceCulling(CullingMode cullingMode)
		{
			CheckGLContext();

			switch(cullingMode)
			{
			case CullingMode::None:
				glDisable(GL_CULL_FACE);
				break;

			case CullingMode::Clockwise:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;

			case CullingMode::CounterClockwise:
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
			CheckGLContext();

			switch (depthTest)
			{
			case DepthTest::Always:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_ALWAYS);
				break;
			case DepthTest::Less:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				break;
			case DepthTest::LessOrEqual:
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				break;
			case DepthTest::Disabled:
				glDisable(GL_DEPTH_TEST);
				break;
			}

			CheckGLError("RendererOpenGL::SetDepthTest");
		}

		void RendererOpenGL::SetScissorTest(ScissorTest scissorTest)
		{
			CheckGLContext();

			switch (scissorTest)
			{
			case ScissorTest::Enabled:
				glEnable(GL_SCISSOR_TEST);
				break;
			case ScissorTest::Disabled:
				glDisable(GL_SCISSOR_TEST);
				break;
			}

			CheckGLError("RendererOpenGL::SetScissorTest");
		}

		void RendererOpenGL::SetScissorRegion(const ion::Vector2i& position, const ion::Vector2i& size)
		{
			CheckGLContext();

			glScissor(position.x, position.y, size.x, size.y);

			CheckGLError("RendererOpenGL::SetScissorRegion");
		}

		//Materials
		void RendererOpenGL::BindMaterial(Material& material, const Matrix4& worldMtx, const Matrix4& viewMtx, const Matrix4& projectionMtx)
		{
#if defined ION_RENDERER_SHADER
			if (material.GetShader())
			{
				material.GetShader()->Bind();
				Material::ShaderParams& shaderParams = material.GetShaderParams();
				
				Matrix4 worldViewMtx = worldMtx * viewMtx;
				Matrix4 worldViewProjMtx = worldViewMtx * projectionMtx;

//#error TODO need to extract the 3x3
				Matrix4 normalMtx = worldViewMtx.GetInverse().GetTranspose();

				shaderParams.matrices.world.SetValue(worldMtx);
				shaderParams.matrices.view.SetValue(viewMtx);
				shaderParams.matrices.worldView.SetValue(worldViewMtx);
				shaderParams.matrices.worldViewProjection.SetValue(worldViewProjMtx);
				shaderParams.matrices.normal.SetValue(normalMtx);
				shaderParams.colours.ambient.SetValue(material.GetAmbientColour());
				shaderParams.colours.diffuse.SetValue(material.GetDiffuseColour());
				shaderParams.colours.specular.SetValue(material.GetSpecularColour());
				shaderParams.colours.emissive.SetValue(material.GetEmissiveColour());

				if (material.GetNumDiffuseMaps() > 0)
				{
					shaderParams.textures.diffuseMap.SetValue(*material.GetDiffuseMap(0));
				}

				if (material.GetNormalMap())
				{
					shaderParams.textures.normalMap.SetValue(*material.GetNormalMap());
				}

				if (material.GetSpecularMap())
				{
					shaderParams.textures.specularMap.SetValue(*material.GetSpecularMap());
				}

				if (material.GetOpacityMap())
				{
					shaderParams.textures.opacityMap.SetValue(*material.GetOpacityMap());
				}
			}
			else
			{
				ion::debug::Error("RendererOpenGL::BindMaterial() - Material has no shader");
			}
#elif defined ION_RENDERER_FIXED
			//Set fixed function matrix
			glLoadMatrixf((worldMtx * viewMtx).GetAsFloatArray());

			//Bind diffuse map
			if (material.GetNumDiffuseMaps() > 0)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, ((TextureOpenGL*)material.GetDiffuseMap(0))->GetTextureId());
			}

			//Setup default lighting
			if (material.GetLightingEnabled())
			{
				float ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				float diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
				glEnable(GL_LIGHT0);
				glEnable(GL_LIGHTING);

				//Set material properties
				glMaterialfv(GL_FRONT, GL_AMBIENT, (float*)&material.GetAmbientColour().r);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, (float*)&material.GetDiffuseColour().r);
				glMaterialfv(GL_FRONT, GL_SPECULAR, (float*)&material.GetSpecularColour().r);

				//Disable lighting
				glDisable(GL_LIGHT0);
				glDisable(GL_LIGHTING);
			}

			RendererOpenGL::CheckGLError("RendererOpenGL::BindMaterial");
#endif
		}

		void RendererOpenGL::UnbindMaterial(Material& material)
		{
#if defined ION_RENDERER_SHADER
			if (material.GetShader())
			{
				material.GetShader()->Unbind();
			}
			else
			{
				ion::debug::Error("RendererOpenGL::UnbindMaterial() - Material has no shader");
			}
#elif defined ION_RENDERER_FIXED
			//Restore fixed function matrix
			glLoadMatrixf(Matrix4().GetAsFloatArray());

			glBindTexture(GL_TEXTURE_2D, 0);
#if !defined ION_RENDERER_OPENGL_ES
			glDisable(GL_TEXTURE_2D);
#endif

			RendererOpenGL::CheckGLError("RendererOpenGL::UnbindMaterial");
#endif
		}

		void RendererOpenGL::BindVertexBuffer(const VertexBuffer& vertexBuffer)
		{
			int positionSize = vertexBuffer.GetElementNumComponents(VertexBuffer::ElementType::Position);
			int normalSize = vertexBuffer.GetElementNumComponents(VertexBuffer::ElementType::Normal);
			int colourSize = vertexBuffer.GetElementNumComponents(VertexBuffer::ElementType::Colour);
			int texCoordSize = vertexBuffer.GetElementNumComponents(VertexBuffer::ElementType::TexCoord);

#if defined ION_RENDERER_FIXED
			//Enable client states and set element pointers
			if (positionSize)
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(positionSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::ElementType::Position)], vertexBuffer.GetElementStride(VertexBuffer::ElementType::Position), vertexBuffer.GetStartAddress(VertexBuffer::ElementType::Position));
			}

			if (normalSize)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::ElementType::Normal)], vertexBuffer.GetElementStride(VertexBuffer::ElementType::Normal), vertexBuffer.GetStartAddress(VertexBuffer::ElementType::Normal));
			}

			if (colourSize)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(colourSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::ElementType::Colour)], vertexBuffer.GetElementStride(VertexBuffer::ElementType::Colour), vertexBuffer.GetStartAddress(VertexBuffer::ElementType::Colour));
			}

			if (texCoordSize)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(texCoordSize, s_glVertexDataTypes[vertexBuffer.GetDataType(VertexBuffer::ElementType::TexCoord)], vertexBuffer.GetElementStride(VertexBuffer::ElementType::TexCoord), vertexBuffer.GetStartAddress(VertexBuffer::ElementType::TexCoord));
			}
#else
			debug::Assert(vertexBuffer.IsCompiled(), "RendererOpenGL::BindVertexBuffer() - Vertex buffer has not been compiled");

			//Bind vertex array
			opengl::extensions->glBindVertexArray(vertexBuffer.m_glVAO);

			//Enable vertex attributes
			if (positionSize)
				opengl::extensions->glEnableVertexAttribArray((int)ion::render::VertexBuffer::ElementType::Position);

			if (normalSize)
				opengl::extensions->glEnableVertexAttribArray((int)ion::render::VertexBuffer::ElementType::Normal);

			if (colourSize)
				opengl::extensions->glEnableVertexAttribArray((int)ion::render::VertexBuffer::ElementType::Colour);

			if (texCoordSize)
				opengl::extensions->glEnableVertexAttribArray((int)ion::render::VertexBuffer::ElementType::TexCoord);

			//Bind index data
			if (vertexBuffer.m_glEAB)
				opengl::extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer.m_glEAB);
#endif

			CheckGLError("RendererOpenGL::BindVertexBuffer()");
		}

		void RendererOpenGL::UnbindVertexBuffer()
		{
#if defined ION_RENDERER_FIXED
			//Disable client states
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
#else
			opengl::extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			opengl::extensions->glDisableVertexAttribArray((int)ion::render::VertexBuffer::ElementType::Position);
			opengl::extensions->glDisableVertexAttribArray((int)ion::render::VertexBuffer::ElementType::Normal);
			opengl::extensions->glDisableVertexAttribArray((int)ion::render::VertexBuffer::ElementType::Colour);
			opengl::extensions->glDisableVertexAttribArray((int)ion::render::VertexBuffer::ElementType::TexCoord);
			opengl::extensions->glBindVertexArray(0);
#endif
		}

		void RendererOpenGL::DrawVertexBuffer(const VertexBuffer& vertexBuffer)
		{
			CheckGLContext();

			//Apply vertex attributes and bind vertex data
			BindVertexBuffer(vertexBuffer);

			//Draw
			glDrawArrays(s_glVertexPatternTypes[(int)vertexBuffer.GetPattern()], 0, vertexBuffer.GetNumVerts());

			//Unbind vertex data and disable vertex attributes
			UnbindVertexBuffer();

			CheckGLError("RendererOpenGL::DrawVertexBuffer(const VertexBuffer&)");
		}

		void RendererOpenGL::DrawVertexBuffer(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer)
		{
			CheckGLContext();

			//Apply vertex attributes and bind vertex data
			BindVertexBuffer(vertexBuffer);

			//Draw
			GLuint indexType = sizeof(TIndex) == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
#if defined ION_RENDERER_FIXED
			glDrawElements(s_glVertexPatternTypes[vertexBuffer.GetPattern()], indexBuffer.GetSize(), indexType, indexBuffer.GetAddress());
#else
			if(vertexBuffer.m_glEAB)
				glDrawElements(s_glVertexPatternTypes[(int)vertexBuffer.GetPattern()], indexBuffer.GetSize(), indexType, 0);
			else
				glDrawElements(s_glVertexPatternTypes[(int)vertexBuffer.GetPattern()], indexBuffer.GetSize(), indexType, indexBuffer.GetAddress());
#endif

			//Unbind vertex data and disable vertex attributes
			UnbindVertexBuffer();

			CheckGLError("RendererOpenGL::DrawVertexBuffer(const VertexBuffer&, const IndexBuffer&)");
		}

		void RendererOpenGL::DrawVertexBuffer(const VertexBuffer& compiledVertexBuffer, int indexOffset, int indexCount)
		{
			//TODO: Mesh object

			CheckGLContext();

			//Apply vertex attributes and bind vertex data
			BindVertexBuffer(compiledVertexBuffer);

			//Draw
			GLuint indexType = sizeof(TIndex) == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
#if defined ION_RENDERER_FIXED
			glDrawElements(s_glVertexPatternTypes[(int)compiledVertexBuffer.GetPattern()], indexCount, indexType, (void*)(intptr_t)indexBuffer.GetAddress() + (indexOffset * sizeof(TIndex)));
#else
			glDrawElements(s_glVertexPatternTypes[(int)compiledVertexBuffer.GetPattern()], indexCount, indexType, (void*)(intptr_t)indexOffset);
#endif

			//Unbind vertex data and disable vertex attributes
			UnbindVertexBuffer();

			CheckGLError("RendererOpenGL::DrawVertexBuffer(const VertexBuffer&, const IndexBuffer&)");
		}
	}
}
