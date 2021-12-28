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
#include <core/debug/Debug.h>

#if !defined ION_RENDERER_FIXED
#include "opengl/OpenGLInclude.h"
#include "opengl/OpenGLExtensions.h"
#include "opengl/RendererOpenGL.h"
#endif

namespace ion
{
	namespace render
	{
		const std::vector<VertexBuffer::Element> VertexBuffer::s_defaultLayout =
		{
			Element({ ElementType::Position, DataType::Float, 3 }),
			Element({ ElementType::Normal, DataType::Float, 3 }),
			Element({ ElementType::TexCoord, DataType::Float, 2 }),
			Element({ ElementType::Colour, DataType::Float, 4 })
		};

		const int VertexBuffer::s_dataTypeSizes[(int)DataType::Count] =
		{
			sizeof(u8),
			sizeof(u16),
			sizeof(u32),
			sizeof(float)
		};

		VertexBuffer::VertexBuffer(Pattern pattern, Preset preset)
		{
			m_pattern = pattern;
			m_packType = PackType::Interleaved;
			m_compiled = false;
			ClearLayout();

			switch (preset)
			{
			case Preset::Default:
				SetLayout(s_defaultLayout);
			default:
				break;
			}
		}

		VertexBuffer::VertexBuffer(Pattern pattern, const std::vector<Element>& layout, PackType packType)
		{
			m_pattern = pattern;
			m_packType = packType;
			m_compiled = false;
			ClearLayout();
			SetLayout(layout);
		}

		VertexBuffer::~VertexBuffer()
		{
			ClearLayout();
		}

		void VertexBuffer::SetLayout(const std::vector<VertexBuffer::Element>& layout)
		{
			debug::Assert(!m_compiled, "VertexBuffer::SetLayout() - Vertex layout has been compiled, cannot make changes");

			for (int i = 0; i < layout.size(); i++)
			{
				AddLayoutElement(layout[i].elementType, layout[i].dataType, layout[i].numComponents);
			}
		}

		void VertexBuffer::AddLayoutElement(ElementType elementType, DataType dataType, int numComponents)
		{
			debug::Assert(!m_compiled, "VertexBuffer::AddLayoutElement() - Vertex layout has been compiled, cannot make changes");
			debug::Assert(m_elements[(int)elementType].size == 0, "VertexBuffer::AddLayoutElement() Element already added");

			m_layout[m_numElements++] = elementType;

			m_elements[(int)elementType].dataType = dataType;
			m_elements[(int)elementType].size = numComponents * s_dataTypeSizes[(int)dataType];
			m_elements[(int)elementType].stride = m_strideBytes;
			m_elements[(int)elementType].numComponents = numComponents;

			m_strideBytes += numComponents * s_dataTypeSizes[(int)dataType];
		}

		void VertexBuffer::CompileBuffer(IndexBuffer* indexBuffer)
		{
			debug::Assert(!m_compiled, "VertexBuffer::CompileLayout() - Vertex layout has already been compiled");

#if !defined ION_RENDERER_FIXED
			ion::debug::Assert(m_packType == PackType::Interleaved, "VertexBuffer::CompileLayout() - Only interleaved vertex buffers currently supported on OpenGL");
			RendererOpenGL::LockGLContext();

			m_glVAO = 0;
			m_glVBO = 0;
			m_glEAB = 0;

			int PositionSize = GetElementNumComponents(VertexBuffer::ElementType::Position);
			int normalSize = GetElementNumComponents(VertexBuffer::ElementType::Normal);
			int colourSize = GetElementNumComponents(VertexBuffer::ElementType::Colour);
			int texCoordSize = GetElementNumComponents(VertexBuffer::ElementType::TexCoord);

			//Generate vertex array
			opengl::extensions->glGenVertexArrays(1, &m_glVAO);
			opengl::extensions->glBindVertexArray(m_glVAO);

			//Generate vertex buffer and bind attribute buffers
			opengl::extensions->glGenBuffers(1, &m_glVBO);
			opengl::extensions->glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);
			opengl::extensions->glBufferData(GL_ARRAY_BUFFER, m_buffer.size(), m_buffer.data(), GL_STATIC_DRAW);

			//Position attribute
			if (PositionSize)
			{
				opengl::extensions->glVertexAttribPointer(
					(int)ion::render::VertexBuffer::ElementType::Position,
					GetElementNumComponents(ion::render::VertexBuffer::ElementType::Position),
					RendererOpenGL::GetGLDataType(GetDataType(ion::render::VertexBuffer::ElementType::Position)),
					GL_FALSE,
					GetElementStride(ion::render::VertexBuffer::ElementType::Position),
					(void*)(uintptr_t)(GetElementByteOffset(ion::render::VertexBuffer::ElementType::Position)));
			}

			//Normal attribute
			if (normalSize)
			{
				opengl::extensions->glVertexAttribPointer(
					(int)ion::render::VertexBuffer::ElementType::Normal,
					GetElementNumComponents(ion::render::VertexBuffer::ElementType::Normal),
					RendererOpenGL::GetGLDataType(GetDataType(ion::render::VertexBuffer::ElementType::Normal)),
					GL_FALSE,
					GetElementStride(ion::render::VertexBuffer::ElementType::Normal),
					(void*)(uintptr_t)(GetElementByteOffset(ion::render::VertexBuffer::ElementType::Normal)));
			}

			//Colour attribute
			if (colourSize)
			{
				opengl::extensions->glVertexAttribPointer(
					(int)ion::render::VertexBuffer::ElementType::Colour,
					GetElementNumComponents(ion::render::VertexBuffer::ElementType::Colour),
					RendererOpenGL::GetGLDataType(GetDataType(ion::render::VertexBuffer::ElementType::Colour)),
					GL_FALSE,
					GetElementStride(ion::render::VertexBuffer::ElementType::Colour),
					(void*)(uintptr_t)(GetElementByteOffset(ion::render::VertexBuffer::ElementType::Colour)));
			}

			//TexCoord attribute
			if (texCoordSize)
			{
				opengl::extensions->glVertexAttribPointer(
					(int)ion::render::VertexBuffer::ElementType::TexCoord,
					GetElementNumComponents(ion::render::VertexBuffer::ElementType::TexCoord),
					RendererOpenGL::GetGLDataType(GetDataType(ion::render::VertexBuffer::ElementType::TexCoord)),
					GL_FALSE,
					GetElementStride(ion::render::VertexBuffer::ElementType::TexCoord),
					(void*)(uintptr_t)(GetElementByteOffset(ion::render::VertexBuffer::ElementType::TexCoord)));
			}

			opengl::extensions->glBindBuffer(GL_ARRAY_BUFFER, 0);

			//Generate index buffer
			if (indexBuffer)
			{
				opengl::extensions->glGenBuffers(1, &m_glEAB);
				opengl::extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glEAB);
				opengl::extensions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->GetSize() * sizeof(TIndex), indexBuffer->GetAddress(), GL_STATIC_DRAW);
				opengl::extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			//Unbound vertex array
			opengl::extensions->glBindVertexArray(0);
			RendererOpenGL::UnlockGLContext();
#endif

			m_compiled = true;
		}

		void VertexBuffer::CommitBuffer()
		{
			debug::Assert(m_compiled, "VertexBuffer::CommitBuffer() - Buffer has not been compiled");

#if !defined ION_RENDERER_FIXED
			RendererOpenGL::LockGLContext();

			opengl::extensions->glBindVertexArray(m_glVAO);

			opengl::extensions->glBindBuffer(GL_ARRAY_BUFFER, m_glVBO);
			opengl::extensions->glBufferSubData(GL_ARRAY_BUFFER, 0, m_buffer.size(), m_buffer.data());
			opengl::extensions->glBindBuffer(GL_ARRAY_BUFFER, 0);

			opengl::extensions->glBindVertexArray(0);

			RendererOpenGL::UnlockGLContext();
#endif
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

		const u8* VertexBuffer::GetStartAddress(ElementType elementType) const
		{
			if(m_packType == PackType::Interleaved)
				return (m_buffer.data() + m_elements[(int)elementType].stride);
			else
				return m_elements[(int)elementType].buffer.data();
		}

		u8* VertexBuffer::GetStartAddress(ElementType elementType)
		{
			if (m_packType == PackType::Interleaved)
				return ((u8*)m_buffer.data() + m_elements[(int)elementType].stride);
			else
				return m_elements[(int)elementType].buffer.data();
		}

		int VertexBuffer::GetElementSize(ElementType elementType) const
		{
			return m_elements[(int)elementType].size;
		}

		int VertexBuffer::GetElementStride(ElementType elementType) const
		{
			if (m_packType == PackType::Interleaved)
				return m_strideBytes;
			else
				return m_elements[(int)elementType].size;
		}

		int VertexBuffer::GetElementByteOffset(ElementType elementType) const
		{
			return m_elements[(int)elementType].stride;
		}

		int VertexBuffer::GetElementNumComponents(ElementType elementType) const
		{
			return m_elements[(int)elementType].numComponents;
		}

		VertexBuffer::DataType VertexBuffer::GetDataType(ElementType elementType) const
		{
			return m_elements[(int)elementType].dataType;
		}

		void VertexBuffer::AddVertex(const Vector3& Position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord)
		{
			if (m_packType == PackType::Interleaved)
			{
				int elementOffset = (int)m_buffer.size();

				m_buffer.resize(m_buffer.size() + m_strideBytes);

				if (m_elements[(int)ElementType::Position].size > 0)
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Position].stride, (u8*)Position.Data(), m_elements[(int)ElementType::Position].size);

				if (m_elements[(int)ElementType::Normal].size > 0)
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Normal].stride, (u8*)normal.Data(), m_elements[(int)ElementType::Normal].size);

				if (m_elements[(int)ElementType::TexCoord].size > 0)
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::TexCoord].stride, (u8*)texCoord.Data(), m_elements[(int)ElementType::TexCoord].size);

				if (m_elements[(int)ElementType::Colour].size > 0)
				{
					if (m_elements[(int)ElementType::Colour].dataType == DataType::Float)
					{
						memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Colour].stride, (u8*)colour.Data(), m_elements[(int)ElementType::Colour].size);
					}
					else if (m_elements[(int)ElementType::Colour].dataType == DataType::Int)
					{
						u32 colourRGBA = colour.AsRGBA();
						memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Colour].stride, &colourRGBA, m_elements[(int)ElementType::Colour].size);
					}
					else if (m_elements[(int)ElementType::Colour].dataType == DataType::Byte)
					{
						u32 colourRGBA = colour.AsRGBA();
						memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Colour].stride, &colourRGBA, m_elements[(int)ElementType::Colour].size);
					}
				}
			}
			else
			{
				if (m_elements[(int)ElementType::Position].size > 0)
				{
					u64 offset = m_elements[(int)ElementType::Position].buffer.size();
					m_elements[(int)ElementType::Position].buffer.resize(offset + m_elements[(int)ElementType::Position].size);
					memory::MemCopy(m_elements[(int)ElementType::Position].buffer.data() + offset, (u8*)Position.Data(), m_elements[(int)ElementType::Position].size);
				}

				if (m_elements[(int)ElementType::Normal].size > 0)
				{
					u64 offset = m_elements[(int)ElementType::Normal].buffer.size();
					m_elements[(int)ElementType::Normal].buffer.resize(offset + m_elements[(int)ElementType::Normal].size);
					memory::MemCopy(m_elements[(int)ElementType::Normal].buffer.data() + offset, (u8*)normal.Data(), m_elements[(int)ElementType::Normal].size);
				}

				if (m_elements[(int)ElementType::TexCoord].size > 0)
				{
					u64 offset = m_elements[(int)ElementType::TexCoord].buffer.size();
					m_elements[(int)ElementType::TexCoord].buffer.resize(offset + m_elements[(int)ElementType::TexCoord].size);
					memory::MemCopy(m_elements[(int)ElementType::TexCoord].buffer.data() + offset, (u8*)texCoord.Data(), m_elements[(int)ElementType::TexCoord].size);
				}

				if (m_elements[(int)ElementType::Colour].size > 0)
				{
					u64 offset = m_elements[(int)ElementType::Colour].buffer.size();
					m_elements[(int)ElementType::Colour].buffer.resize(offset + m_elements[(int)ElementType::Colour].size);
					memory::MemCopy(m_elements[(int)ElementType::Colour].buffer.data() + offset, (u8*)colour.Data(), m_elements[(int)ElementType::Colour].size);

					if (m_elements[(int)ElementType::Colour].dataType == DataType::Float)
					{
						memory::MemCopy(m_elements[(int)ElementType::Colour].buffer.data() + offset, (u8*)colour.Data(), m_elements[(int)ElementType::Colour].size);
					}
					else if (m_elements[(int)ElementType::Colour].dataType == DataType::Int)
					{
						u32 colourRGBA = colour.AsRGBA();
						memory::MemCopy(m_elements[(int)ElementType::Colour].buffer.data() + offset, &colourRGBA, m_elements[(int)ElementType::Colour].size);
					}
					else if (m_elements[(int)ElementType::Colour].dataType == DataType::Byte)
					{
						u32 colourRGBA = colour.AsRGBA();
						memory::MemCopy(m_elements[(int)ElementType::Colour].buffer.data() + offset, &colourRGBA, m_elements[(int)ElementType::Colour].size);
					}
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
			m_buffer.reserve(size * stride);
		}

		void VertexBuffer::Resize(int size)
		{
			int stride = GetStrideBytes();
			debug::Assert(stride > 0, "VertexBuffer::Reserve() - Layout not set");
			m_buffer.resize(size * stride);
			m_numVertices = size;
		}

		void VertexBuffer::SetVertex(int vertexIdx, const Vector3& Position, const Vector3& normal, const Colour& colour, const TexCoord& texCoord)
		{
			debug::Assert(vertexIdx >= 0 && vertexIdx < m_numVertices, "Bad vertex id");

			if (m_packType == PackType::Interleaved)
			{
				int elementOffset = m_strideBytes * vertexIdx;

				if(m_elements[(int)ElementType::Position].size > 0)
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Position].stride, (u8*)Position.Data(), m_elements[(int)ElementType::Position].size);
				if (m_elements[(int)ElementType::Normal].size > 0)
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Normal].stride, (u8*)normal.Data(), m_elements[(int)ElementType::Normal].size);
				if (m_elements[(int)ElementType::TexCoord].size > 0)
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::TexCoord].stride, (u8*)texCoord.Data(), m_elements[(int)ElementType::TexCoord].size);
				if (m_elements[(int)ElementType::Colour].size > 0)
					memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Colour].stride, (u8*)colour.Data(), m_elements[(int)ElementType::Colour].size);
			}
			else
			{
				if (m_elements[(int)ElementType::Position].size > 0)
					memory::MemCopy(m_elements[(int)ElementType::Position].buffer.data() + (vertexIdx * m_elements[(int)ElementType::Position].size), (u8*)Position.Data(), m_elements[(int)ElementType::Position].size);
				if (m_elements[(int)ElementType::Normal].size > 0)
					memory::MemCopy(m_elements[(int)ElementType::Normal].buffer.data() + (vertexIdx * m_elements[(int)ElementType::Normal].size), (u8*)normal.Data(), m_elements[(int)ElementType::Normal].size);
				if (m_elements[(int)ElementType::TexCoord].size > 0)
					memory::MemCopy(m_elements[(int)ElementType::TexCoord].buffer.data() + (vertexIdx * m_elements[(int)ElementType::TexCoord].size), (u8*)texCoord.Data(), m_elements[(int)ElementType::TexCoord].size);
				if (m_elements[(int)ElementType::Colour].size > 0)
					memory::MemCopy(m_elements[(int)ElementType::Colour].buffer.data() + (vertexIdx * m_elements[(int)ElementType::Colour].size), (u8*)colour.Data(), m_elements[(int)ElementType::Colour].size);
			}
		}

		void VertexBuffer::SetColour(int vertexIdx, const Colour& colour)
		{
			debug::Assert(vertexIdx >= 0 && vertexIdx < m_numVertices, "Bad vertex id");
			int elementOffset = m_strideBytes * vertexIdx;

			if (m_elements[(int)ElementType::Colour].size > 0)
			{
				if (m_packType == PackType::Interleaved)
				{
					if (m_elements[(int)ElementType::Colour].dataType == DataType::Float)
					{
						memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Colour].stride, (u8*)colour.Data(), m_elements[(int)ElementType::Colour].size);
					}
					else if (m_elements[(int)ElementType::Colour].dataType == DataType::Int)
					{
						u32 colourARGB = colour.AsARGB();
						memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Colour].stride, &colourARGB, m_elements[(int)ElementType::Colour].size);
					}
					else if (m_elements[(int)ElementType::Colour].dataType == DataType::Byte)
					{
						u32 colourARGB = colour.AsARGB();
						memory::MemCopy(m_buffer.data() + elementOffset + m_elements[(int)ElementType::Colour].stride, &colourARGB, m_elements[(int)ElementType::Colour].size);
					}
				}
				else
				{
					if (m_elements[(int)ElementType::Colour].dataType == DataType::Float)
					{
						memory::MemCopy(m_elements[(int)ElementType::Colour].buffer.data() + (vertexIdx * m_elements[(int)ElementType::Colour].size), (u8*)colour.Data(), m_elements[(int)ElementType::Colour].size);
					}
					else if (m_elements[(int)ElementType::Colour].dataType == DataType::Int)
					{
						u32 colourARGB = colour.AsARGB();
						memory::MemCopy(m_elements[(int)ElementType::Colour].buffer.data() + (vertexIdx * m_elements[(int)ElementType::Colour].size), &colourARGB, m_elements[(int)ElementType::Colour].size);
					}
					else if (m_elements[(int)ElementType::Colour].dataType == DataType::Byte)
					{
						u32 colourARGB = colour.AsARGB();
						memory::MemCopy(m_elements[(int)ElementType::Colour].buffer.data() + (vertexIdx * m_elements[(int)ElementType::Colour].size), &colourARGB, m_elements[(int)ElementType::Colour].size);
					}
				}
			}
		}

		Vertex VertexBuffer::GetPosition(int vertexIdx) const
		{
			debug::Assert(vertexIdx >= 0 && vertexIdx < m_numVertices, "Bad vertex id");

			Vertex Position;

			if (m_packType == PackType::Interleaved)
			{
				int elementOffset = m_strideBytes * vertexIdx;
				memory::MemCopy((u8*)Position.Data(), m_buffer.data() + elementOffset + m_elements[(int)ElementType::Position].stride, m_elements[(int)ElementType::Position].size);
			}
			else
			{
				memory::MemCopy((u8*)Position.Data(), m_elements[(int)ElementType::Position].buffer.data() + (vertexIdx * m_elements[(int)ElementType::Position].size), m_elements[(int)ElementType::Position].size);
			}

			return Position;
		}

		void VertexBuffer::ClearVertices()
		{
#if !defined ION_RENDERER_FIXED
			if (m_compiled)
			{
				RendererOpenGL::LockGLContext();

				if (m_glEAB)
					opengl::extensions->glDeleteBuffers(1, &m_glEAB);

				opengl::extensions->glDeleteBuffers(1, &m_glVBO);
				opengl::extensions->glDeleteVertexArrays(1, &m_glVAO);

				RendererOpenGL::UnlockGLContext();
			}
#endif

			m_buffer.clear();
			m_numVertices = 0;
			m_compiled = false;

			for (int i = 0; i < (int)ElementType::Count; i++)
			{
				m_elements[i].buffer.clear();
			}
		}

		void VertexBuffer::ClearLayout()
		{
			ClearVertices();

			for (int i = 0; i < (int)ElementType::Count; i++)
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