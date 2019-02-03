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
			Element({ eColour, eFloat, 4 })
		};

		const int VertexBuffer::s_dataTypeSizes[eDataTypeCount] =
		{
			sizeof(u8),
			sizeof(u16),
			sizeof(u32),
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
			m_pattern = pattern;
			ClearLayout();
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
				AddLayoutElement(layout[i].elementType, layout[i].dataType, layout[i].numComponents);
			}
		}

		void VertexBuffer::AddLayoutElement(ElementType elementType, DataType dataType, int numComponents)
		{
			debug::Assert(m_elements[elementType].size == 0, "VertexBuffer::AddLayoutElement() Element already added");

			m_layout[m_numElements++] = elementType;

			m_elements[elementType].dataType = dataType;
			m_elements[elementType].size = numComponents * s_dataTypeSizes[dataType];
			m_elements[elementType].stride = m_strideBytes;
			m_elements[elementType].numComponents = numComponents;

			m_strideBytes += numComponents * s_dataTypeSizes[dataType];
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

		int VertexBuffer::GetElementNumComponents(ElementType elementType) const
		{
			return m_elements[elementType].numComponents;
		}

		VertexBuffer::DataType VertexBuffer::GetDataType(ElementType elementType) const
		{
			return m_elements[elementType].dataType;
		}

		void VertexBuffer::AddVertex(const Vector3& position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord)
		{
			int elementOffset = m_buffer.size();

			m_buffer.resize(m_buffer.size() + m_strideBytes);

			if(m_elements[ePosition].size > 0)
				memory::MemCopy(m_buffer.data() + elementOffset + m_elements[ePosition].stride, (u8*)position.Data(), m_elements[ePosition].size);

			if(m_elements[eNormal].size > 0)
				memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eNormal].stride, (u8*)normal.Data(), m_elements[eNormal].size);

			if(m_elements[eTexCoord].size > 0)
				memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eTexCoord].stride, (u8*)texCoord.Data(), m_elements[eTexCoord].size);

			if (m_elements[eColour].size > 0)
			{
				if (m_elements[eColour].dataType == eFloat)
				{
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, (u8*)colour.Data(), m_elements[eColour].size);
				}
				else if (m_elements[eColour].dataType == eInt)
				{
					u32 colourRGBA = colour.AsRGBA();
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, &colourRGBA, m_elements[eColour].size);
				}
				else if (m_elements[eColour].dataType == eByte)
				{
					u32 colourRGBA = colour.AsRGBA();
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, &colourRGBA, m_elements[eColour].size);
				}
			}

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

			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[ePosition].stride, (u8*)position.Data(), m_elements[ePosition].size);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eNormal].stride, (u8*)normal.Data(), m_elements[eNormal].size);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eTexCoord].stride, (u8*)texCoord.Data(), m_elements[eTexCoord].size);
			memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, (u8*)colour.Data(), m_elements[eColour].size);
		}

		void VertexBuffer::SetColour(int vertexIdx, const Colour& colour)
		{
			debug::Assert(vertexIdx >= 0 && vertexIdx < m_numVertices, "Bad vertex id");
			int elementOffset = m_strideBytes * vertexIdx;

			if (m_elements[eColour].size > 0)
			{
				if (m_elements[eColour].dataType == eFloat)
				{
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, (u8*)colour.Data(), m_elements[eColour].size);
				}
				else if (m_elements[eColour].dataType == eInt)
				{
					u32 colourARGB = colour.AsARGB();
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, &colourARGB, m_elements[eColour].size);
				}
				else if (m_elements[eColour].dataType == eByte)
				{
					u32 colourARGB = colour.AsARGB();
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[eColour].stride, &colourARGB, m_elements[eColour].size);
				}
			}
		}

		Vertex VertexBuffer::GetPosition(int vertexIdx) const
		{
			debug::Assert(vertexIdx >= 0 && vertexIdx < m_numVertices, "Bad vertex id");

			int elementOffset = m_strideBytes * vertexIdx;

			Vertex position;

			memory::MemCopy((u8*)position.Data(), m_buffer.data() + elementOffset + m_elements[ePosition].stride, m_elements[ePosition].size);

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