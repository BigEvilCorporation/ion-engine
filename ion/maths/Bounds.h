///////////////////////////////////////////////////
// (c) 2020 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Bounds.h
// Date:		7th June 2020
// Authors:		Matt Phillips
// Description:	2D bounding box class for all types implementing .x and .y
///////////////////////////////////////////////////

#pragma once

#include "Vector.h"
#include "Geometry.h"
#include "Maths.h"

namespace ion
{
	namespace maths
	{
		template<typename T> class Bounds2D
		{
		public:
			Bounds2D();
			Bounds2D(const T& boundsMin, const T& boundsMax);

			void Clear();
			void AddPoint(const T& point);
			void Grow(const T& delta);

			const T& GetMin() const;
			const T& GetMax() const;
			T GetCentre() const;
			T GetSize() const;

			bool PointInside(const T& point) const;

		private:
			T m_min;
			T m_max;
		};

		typedef Bounds2D<Vector2> Bounds2;
		typedef Bounds2D<Vector2i> Bounds2i;
	}
}

namespace ion
{
	namespace maths
	{
		template<typename T> Bounds2D<T>::Bounds2D()
			: m_min(T::Max)
			, m_max(T::Min)
		{

		}

		template<typename T> Bounds2D<T>::Bounds2D(const T& boundsMin, const T& boundsMax)
			: m_min(boundsMin)
			, m_max(boundsMax)
		{

		}

		template<typename T> void Bounds2D<T>::Clear()
		{
			m_min = T::Max
			m_max = T::Min
		}

		template<typename T> void Bounds2D<T>::AddPoint(const T& point)
		{
			if (point.x < m_min.x)
				m_min.x = point.x;
			if (point.x > m_max.x)
				m_max.x = point.x;
			if (point.y < m_min.y)
				m_min.y = point.y;
			if (point.y > m_max.y)
				m_max.y = point.y;
		}

		template<typename T> void Bounds2D<T>::Grow(const T& delta)
		{
			m_min.x -= delta.x / 2;
			m_min.y -= delta.y / 2;
			m_max.x += delta.x / 2;
			m_max.y += delta.y / 2;
		}

		template<typename T> const T& Bounds2D<T>::GetMin() const
		{
			return m_min;
		}

		template<typename T> const T& Bounds2D<T>::GetMax() const
		{
			return m_max;
		}

		template<typename T> T Bounds2D<T>::GetCentre() const
		{
			return m_min + (GetSize() / 2);
		}

		template<typename T> T Bounds2D<T>::GetSize() const
		{
			return m_max - m_min;
		}

		template<typename T> bool Bounds2D<T>::PointInside(const T& point) const
		{
			return PointInsideBox(point, m_min, m_max);
		}
	}
}