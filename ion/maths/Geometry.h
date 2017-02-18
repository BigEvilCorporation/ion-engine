///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Geometry.h
// Date:		10th April 2016
// Authors:		Matt Phillips
// Description:	Basic geometry tests
///////////////////////////////////////////////////

#pragma once

#include "Vector.h"

namespace ion
{
	namespace maths
	{
		template<typename T>  bool PointInsideBox(const TVector2<T>& point, const TVector2<T>& boxMin, const TVector2<T>& boxMax);
		template<typename T>  bool BoxInsideBox(const TVector2<T>& boxAMin, const TVector2<T>& boxAMax, const TVector2<T>& boxBMin, const TVector2<T>& boxBMax);
		template<typename T>  bool BoxIntersectsBox(const TVector2<T>& boxAMin, const TVector2<T>& boxAMax, const TVector2<T>& boxBMin, const TVector2<T>& boxBMax);
	}
}

namespace ion
{
	namespace maths
	{
		template<typename T> bool PointInsideBox(const TVector2<T>& point, const TVector2<T>& boxMin, const TVector2<T>& boxMax)
		{
			if(point.x < boxMin.x) return false;
			if(point.x > boxMax.x) return false;
			if(point.y < boxMin.y) return false;
			if(point.y > boxMax.y) return false;
			return true;
		}

		template<typename T>  bool BoxInsideBox(const TVector2<T>& boxAMin, const TVector2<T>& boxAMax, const TVector2<T>& boxBMin, const TVector2<T>& boxBMax)
		{
			if(boxAMin.y > boxBMin.y) return false;
			if(boxAMax.y < boxBMax.y) return false;
			if(boxAMin.x > boxBMin.x) return false;
			if(boxAMax.x < boxBMax.x) return false;
			return true;
		}

		template<typename T>  bool BoxIntersectsBox(const TVector2<T>& boxAMin, const TVector2<T>& boxAMax, const TVector2<T>& boxBMin, const TVector2<T>& boxBMax)
		{
			if(boxAMax.y < boxBMin.y) return false;
			if(boxAMin.y > boxBMax.y) return false;
			if(boxAMax.x < boxBMin.x) return false;
			if(boxAMin.x > boxBMax.x) return false;
			return true;
		}
	}
}