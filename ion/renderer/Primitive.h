///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Primitive.h
// Date:		4th August 2011
// Authors:		Matt Phillips
// Description:	Manually built primitive
///////////////////////////////////////////////////

#pragma once

#include "maths/Vector.h"
#include "renderer/Colour.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Entity.h"

#include <string>
#include <vector>

namespace ion
{
	namespace render
	{
		//Forward declaration
		class Material;

		class Primitive : public Entity
		{
		public:
			Primitive(VertexBuffer::Pattern pattern, VertexBuffer::Preset preset = VertexBuffer::eDefault);
			Primitive(VertexBuffer::Pattern pattern, const std::vector<VertexBuffer::Element>& layout);
			virtual ~Primitive();

			void SetCastShadows(bool shadows);

			void SetColour(const ion::Colour& colour);

			const VertexBuffer& GetVertexBuffer() const { return m_vertexBuffer; }
			const IndexBuffer& GetIndexBuffer() const { return m_indexBuffer; }

			VertexBuffer& GetVertexBuffer() { return m_vertexBuffer; }
			IndexBuffer& GetIndexBuffer() { return m_indexBuffer; }

		protected:
			VertexBuffer m_vertexBuffer;
			IndexBuffer m_indexBuffer;
		};

		class LineSegments : public Primitive
		{
		public:
			LineSegments(const std::vector<Vector3>& points);
		};

		class LineStrip : public Primitive
		{
		public:
			LineStrip(const std::vector<Vector3>& points);
		};

		class Quad : public Primitive
		{
		public:
			enum Axis { xy, xz, yz };
			Quad(Axis axis, const Vector2& halfExtents, VertexBuffer::Preset preset = VertexBuffer::eDefault);
			Quad(Axis axis, const Vector2& halfExtents, const std::vector<VertexBuffer::Element>& layout);
			void SetTexCoords(const TexCoord coords[4]);

		private:
			void Build(Axis axis, const Vector2& halfExtents);
		};

		class LineQuad : public Primitive
		{
		public:
			enum Axis { xy, xz, yz };
			LineQuad(Axis axis, const Vector2& halfExtents, const Vector2 offset = ion::Vector2());
		};

		class Grid : public Primitive
		{
		public:
			enum Axis { xy, xz, yz };
			Grid(Axis axis, const Vector2& halfExtents, int widthCells, int heightCells);
		};

		class Chessboard : public Primitive
		{
		public:
			enum Axis { xy, xz, yz };
			Chessboard(Axis axis, const Vector2& halfExtents, int widthCells, int heightCells, bool uniqueVerts);
			void SetTexCoords(int cellIndex, TexCoord coords[4], float z = 0.0f);
		};

		class Box : public Primitive
		{
		public:
			Box(const Vector3& halfExtents, const Vector3& offset = Vector3());
		};

		class Sphere : public Primitive
		{
		public:
			Sphere(float radius, int rings, int segments);
		};

		class Cylinder : public Primitive
		{
		public:
			Cylinder(float radius, float height, int steps, const Vector3& offset = Vector3());
		};
	}
}
