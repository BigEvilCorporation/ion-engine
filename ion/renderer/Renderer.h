///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Renderer.h
// Date:		26th July 2011
// Authors:		Matt Phillips
// Description:	Renderer base class
///////////////////////////////////////////////////

#pragma once

#include "maths/Matrix.h"
#include "maths/Vector.h"
#include "renderer/Types.h"
#include "renderer/Window.h"
#include "renderer/Viewport.h"

#include <string>

namespace ion
{
	//Forward declaration
	class Colour;

	namespace render
	{
		//Forward declaration
		class VertexBuffer;
		class IndexBuffer;
		class Material;
		class Texture;
		class ShaderManager;

		class Renderer
		{
		public:
			//Alpha blending types
			enum AlphaBlendType { eNoBlend, eAdditive, eSubtractive, eTranslucent, eColourFilter };

			//Culling modes
			enum CullingMode { eNoCull, eClockwise, eCounterClockwise };

			//Depth test type
			enum DepthTest { eDisabled, eAlways, eLessEqual };

			//Render vertex pattern type
			enum VertexPattern { eTriangles, eQuads };

			static Renderer* Create(DeviceContext globalDeviceContext);
			static Renderer* Create(DeviceContext globalDeviceContext, RenderContext renderContext);

			virtual ~Renderer();

			//Update renderer
			virtual bool Update(float deltaTime) = 0;

			//Resize callback
			virtual void OnResize(int width, int height) = 0;

			//Fixed function transforms
			virtual void SetMatrix(const Matrix4& matrix) = 0;
			virtual Matrix4 GetProjectionMatrix() = 0;

			//Thread context lock/unlock
			virtual void LockContext(const DeviceContext& deviceContext) = 0;
			virtual void UnlockContext() = 0;

			//Rendering - general
			virtual void BeginFrame(const Viewport& viewport, const DeviceContext& deviceContext) = 0;
			virtual void EndFrame() = 0;
			virtual void SetupViewport(const Viewport& viewport) = 0;
			virtual void SwapBuffers() = 0;
			virtual void SetClearColour(const Colour& colour) = 0;
			virtual void ClearColour() = 0;
			virtual void ClearDepth() = 0;
			virtual void EnableVSync(bool enabled) = 0;

			//Render states
			virtual void SetAlphaBlending(AlphaBlendType alphaBlendType) = 0;
			virtual void SetBlendColour(const Colour& colour) = 0;
			virtual void SetFaceCulling(CullingMode cullingMode) = 0;
			virtual void SetDepthTest(DepthTest depthTest) = 0;
			virtual void SetLineWidth(float width) = 0;

			//Palettes
			virtual void LoadColourPalette(int paletteIdx, const std::vector<Colour>& palette) = 0;

			//Vertex buffer drawing
			virtual void DrawVertexBuffer(const VertexBuffer& vertexBuffer) = 0;
			virtual void DrawVertexBuffer(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer) = 0;

		protected:
			Renderer();

#if defined ION_RENDERER_SHADER
			ShaderManager* m_shaderManager;
#endif
		};
	}
}