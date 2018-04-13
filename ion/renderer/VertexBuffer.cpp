///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		VertexBuffer.cpp
// Date:		3rd December 2013
// Authors:		Matt Phillips
// Description:	Vertex buffer class
///////////////////////////////////////////////////

#include "VertexBuffer.h"
#include <core/memory/Memory.h>

namespace ion
{
	namespace render
	{
		const std::vector<VertexBuffer::Element> VertexBuffer::s_defaultLayout =
		{
			Element({ ePosition, eFloat, 3 }),
			Element({ eNormal, eFloat, 3 }),
			Element({ eTexCoord, eFloat, 2 }),
			Element({ eColour, eFloat, 3 })
		};

		const int VertexBuffer::s_dataTypeSizes[eDataTypeCount] =
		{
			sizeof(u8),
			sizeof(float)
		};

		VertexBuffer::VertexBuffer(Pattern pattern, Preset preset)
		{
			m_pattern = pattern;
			ClearLayout();

			switch (preset)
			{
			case eDefault:
				SetLayout(s_defaultLayout);
			default:
				break;
			}
		}

		VertexBuffer::VertexBuffer(Pattern pattern, const std::vector<Element>& layout)
		{
			SetLayout(layout);
		}

		VertexBuffer::~VertexBuffer()
		{
			m_buffer.clear();
		}

		void VertexBuffer::SetLayout(const std::vector<VertexBuffer::Element>& layout)
		{
			for (int i = 0; i < layout.size(); i++)
			{
				AddLayoutElement(layout[i].elementType, layout[i].dataType, layout[i].size);
			}
		}

		void VertexBuffer::AddLayoutElement(ElementType elementType, DataType dataType, int size)
		{
			debug::Assert(m_elements[elementType].size == 0, "VertexBuffer::AddLayoutElement() Element already added");

			m_layout[m_numElements++] = elementType;

			m_elements[elementType].dataType = dataType;
			m_elements[elementType].size = size;
			m_elements[elementType].stride = m_strideBytes;

			m_strideBytes += size * s_dataTypeSizes[dataType];
		}

		int VertexBuffer::GetStrideBytes() const
		{
			return m_strideBytes;
		}

		VertexBuffer::ElementType VertexBuffer::GetElementType(int index) const
		{
			debug::Assert(index > 0 && index < m_numElements, "VertexBuffer::GetElementType() - Bad element index");
			return m_layout[index];
		}

		const void* VertexBuffer::GetStartAddress(ElementType elementType) const
		{
			return (void*)(((u8*)m_buffer.data()) + m_elements[elementType].stride);
		}

		int VertexBuffer::GetElementSize(ElementType elementType) const
		{
			return m_elements[elementType].size;
		}

		int VertexBuffer::GetElementByteOffset(ElementType elementType) const
		{
			return m_elements[elementType].stride;
		}

		void VertexBuffer::AddVertex(const Vector3& position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord)
		{
			int elementOffset = m_buffer.size();

			m_buffer.resize(m_buffer.size() + m_strideBytes);

			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[ePosition].stride, (u8*)position.Data(), m_elements[ePosition].size * s_dataTypeSizes[m_elements[ePosition].dataType]);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eNormal].stride, (u8*)normal.Data(), m_elements[eNormal].size * s_dataTypeSizes[m_elements[eNormal].dataType]);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eTexCoord].stride, (u8*)texCoord.Data(), m_elements[eTexCoord].size * s_dataTypeSizes[m_elements[eTexCoord].dataType]);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, (u8*)colour.Data(), m_elements[eColour].size * s_dataTypeSizes[m_elements[eColour].dataType]);

			m_numVertices++;
		}

		void VertexBuffer::AddFace(const Face& face)
		{
			for(int i = 0; i < 3; i++)
			{
				AddVertex(face.m_vertices[i], face.m_normals[i], face.m_colours[i], face.m_texCoords[i]);
			}
		}

		void VertexBuffer::Reserve(int size)
		{
			int stride = GetStrideBytes();
			debug::Assert(stride > 0, "VertexBuffer::Reserve() - Layout not set");
			m_buffer.resize(size * stride);
			m_numVertices = size;
		}

		void VertexBuffer::SetVertex(int vertexIdx, const Vector3& position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord)
		{
			debug::Assert(vertexIdx >= 0 && vertexIdx < m_numVertices, "Bad vertex id");

			int elementOffset = m_strideBytes * vertexIdx;

			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[ePosition].stride, (u8*)position.Data(), m_elements[ePosition].size * s_dataTypeSizes[m_elements[ePosition].dataType]);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eNormal].stride, (u8*)normal.Data(), m_elements[eNormal].size * s_dataTypeSizes[m_elements[eNormal].dataType]);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eTexCoord].stride, (u8*)texCoord.Data(), m_elements[eTexCoord].size * s_dataTypeSizes[m_elements[eTexCoord].dataType]);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, (u8*)colour.Data(), m_elements[eColour].size * s_dataTypeSizes[m_elements[eColour].dataType]);
		}

		Vertex VertexBuffer::GetPosition(int vertexIdx) const
		{
			debug::Assert(vertexIdx >= 0 && vertexIdx < m_numVertices, "Bad vertex id");

			int elementOffset = m_strideBytes * vertexIdx;

			Vertex position;

			memory::MemCopy((u8*)position.Data(), m_buffer.data() + elementOffset + m_elements[ePosition].stride, m_elements[ePosition].size * s_dataTypeSizes[m_elements[ePosition].dataType]);

			return position;
		}

		void VertexBuffer::ClearVertices()
		{
			m_buffer.clear();
			m_numVertices = 0;
		}

		void VertexBuffer::ClearLayout()
		{
			for (int i = 0; i < eElementCount; i++)
			{
				m_elements[i].size = 0;
				m_elements[i].stride = 0;
			}

			m_buffer.clear();
			m_numVertices = 0;
			m_numElements = 0;
			m_strideBytes = 0;
		}

		bool VertexBuffer::Lock()
		{
			return false;
		}

		bool VertexBuffer::Unlock()
		{
			return false;
		}
	}
}