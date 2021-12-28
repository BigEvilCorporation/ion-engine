///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		VertexBuffer.h
// Date:		3rd December 2013
// Authors:		Matt Phillips
// Description:	Vertex buffer class
///////////////////////////////////////////////////

#pragma once

#include "maths/Vector.h"
#include "renderer/Colour.h"
#include "renderer/Vertex.h"
#include "renderer/Face.h"
#include "renderer/IndexBuffer.h"

#include <vector>

namespace ion
{
	namespace render
	{
		class VertexBuffer
		{
		public:
			enum class Pattern { Lines, LineStrip, Triangles, Quads, Count };
			enum class ElementType { Position, Normal, Colour, TexCoord, Count };
			enum class DataType { Byte, Short, Int, Float, Count };
			enum class PackType { Interleaved, Contiguous };
			enum class Preset { None, Default };

			struct Element
			{
				ElementType elementType;
				DataType dataType;
				int numComponents;
			};

			VertexBuffer(Pattern pattern, Preset preset = Preset::Default);
			VertexBuffer(Pattern pattern, const std::vector<Element>& layout, PackType packType);
			~VertexBuffer();

			//Builds layout
			void SetLayout(const std::vector<Element>& layout);
			void AddLayoutElement(ElementType elementType, DataType dataType, int numComponents);

			//Builds buffer
			void Reserve(int size);
			void Resize(int size);
			void AddVertex(const Vector3& position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord);
			void SetVertex(int vertexIdx, const Vector3& position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord);
			void SetColour(int vertexIdx, const Colour& colour);
			void AddFace(const Face& face);
			void CompileBuffer(IndexBuffer* indexBuffer = nullptr);
			void CommitBuffer();
			bool IsCompiled() const { return m_compiled; }

			//Lock/unlock
			bool Lock();
			bool Unlock();
			bool IsLocked() const { return m_locked; }

			//Get layout
			int GetNumLayoutElements() const { return m_numElements; }
			ElementType GetElementType(int index) const;

			//Get element offsets/start addresses
			const u8* GetStartAddress(ElementType elementType) const;
			u8* GetStartAddress(ElementType elementType);
			int GetElementSize(ElementType elementType) const;
			int GetElementStride(ElementType elementType) const;
			int GetElementByteOffset(ElementType elementType) const;
			int GetElementNumComponents(ElementType elementType) const;
			DataType GetDataType(ElementType elementType) const;
			Vertex GetPosition(int index) const;

			//Get metrics
			Pattern GetPattern() const { return m_pattern; }
			int GetNumVerts() const { return m_numVertices; }
			int GetStrideBytes() const;

			//Get raw data
			std::vector<u8>& GetData() { return m_buffer; }

			//Clear vertices
			void ClearVertices();

			//Clear layout and vertices
			void ClearLayout();

			//TODO: Renderer-specific vertex buffers and layout descriptors
#if !defined ION_RENDERER_FIXED
			unsigned int m_glVAO;
			unsigned int m_glVBO;
			unsigned int m_glEAB;
#endif

		protected:
			static const int s_dataTypeSizes[(int)DataType::Count];
			static const std::vector<Element> s_defaultLayout;

			struct ElementLayoutEntry
			{
				DataType dataType;
				int size = 0;
				int stride = 0;
				int numComponents = 0;
				std::vector<u8> buffer;
			};

			//Layout
			ElementLayoutEntry m_elements[(int)ElementType::Count];
			ElementType m_layout[(int)ElementType::Count];

			//Vertex buffer
			std::vector<u8> m_buffer;

			Pattern m_pattern;
			PackType m_packType;
			int m_numVertices;
			int m_numElements;
			int m_strideBytes;
			bool m_locked;
			bool m_compiled;
		};
	}
}
