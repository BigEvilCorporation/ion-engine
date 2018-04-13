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

#include <vector>

namespace ion
{
	namespace render
	{
		class VertexBuffer
		{
		public:
			enum Pattern { eLines, eLineStrip, eTriangles, eQuads };
			enum ElementType { ePosition, eNormal, eColour, eTexCoord, eElementCount };
			enum DataType { eByte, eFloat, eDataTypeCount };
			enum Preset { eNone, eDefault };

			struct Element
			{
				ElementType elementType;
				DataType dataType;
				int size;
			};

			VertexBuffer(Pattern pattern, Preset preset = eDefault);
			VertexBuffer(Pattern pattern, const std::vector<Element>& layout);
			~VertexBuffer();

			//Builds layout
			void SetLayout(const std::vector<Element>& layout);
			void AddLayoutElement(ElementType elementType, DataType dataType, int size);

			//Builds buffer
			void Reserve(int size);
			void AddVertex(const Vector3& position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord);
			void SetVertex(int vertexIdx, const Vector3& position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord);
			void AddFace(const Face& face);

			//Lock/unlock
			bool Lock();
			bool Unlock();
			bool IsLocked() const { return m_locked; }

			//Get layout
			int GetNumLayoutElements() const { return m_numElements; }
			ElementType GetElementType(int index) const;

			//Get element offsets/start addresses
			const void* GetStartAddress(ElementType elementType) const;
			int GetElementSize(ElementType elementType) const;
			int GetElementByteOffset(ElementType elementType) const;
			Vertex GetPosition(int index) const;

			//Get metrics
			Pattern GetPattern() const { return m_pattern; }
			int GetNumVerts() const { return m_numVertices; }
			int GetStrideBytes() const;

			//Clear vertices
			void ClearVertices();

			//Clear layout and vertices
			void ClearLayout();

		protected:
			static const int s_dataTypeSizes[eDataTypeCount];
			static const std::vector<Element> s_defaultLayout;

			struct ElementLayoutEntry
			{
				DataType dataType;
				int size = 0;
				int stride = 0;
			};

			//Layout
			ElementLayoutEntry m_elements[eElementCount];
			ElementType m_layout[eElementCount];

			//Vertex buffer
			std::vector<u8> m_buffer;

			Pattern m_pattern;
			int m_numVertices;
			int m_numElements;
			int m_strideBytes;
			bool m_locked;
		};
	}
}