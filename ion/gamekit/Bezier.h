///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Bezier.h
// Date:		2nd April 2016
// Authors:		Matt Phillips
// Description:	Bezier curves
///////////////////////////////////////////////////

#pragma once

#include <vector>
#include <maths/Vector.h>
#include <core/io/Archive.h>

namespace ion
{
	namespace gamekit
	{
		class BezierPath
		{
		public:
			int AddPoint(const Vector2& Position, const Vector2& controlA, const Vector2& controlB);
			void RemovePoint(int index);
			void SetPoint(int index, const Vector2& Position, const Vector2& controlA, const Vector2& controlB);
			void GetPoint(int index, Vector2& Position, Vector2& controlA, Vector2& controlB) const;
			void Move(const Vector2& offset);
			int GetNumPoints() const;
			int GetNumCurves() const;

			float GetLength() const;
			void CalculateLength();

			void GetBounds(Vector2& boundsMin, Vector2& boundsMax) const;
			void CalculateBounds();

			Vector2 GetPosition(float time) const;
			Vector2 GetNormal(float time) const;
			int GetPositions(std::vector<Vector2>& Positions, float startTime, float endTime, int numPositions) const;
			int GetDistributedPositions(std::vector<Vector2>& Positions, int numPositions) const;
			int GetNormals(std::vector<Vector2>& normals, float startTime, float endTime, int numNormals) const;
			int GetDistributedNormals(std::vector<Vector2>& normals, int numNormals) const;

			void Serialise(io::Archive& archive);

		private:
			Vector2 CalculatePosition(const Vector2 controlPoints[4], float time) const;
			Vector2 CalculateNormal(const Vector2 controlPoints[4], float time) const;
			Vector2 CalculateDerivative(const Vector2 controlPoints[4], float time) const;
			float CalculateLength(const Vector2 controlPoints[4]);
			int CalculateSubdivisionTimes(std::vector<float>& times, int divisionCount, int approximationSteps) const;

			void GetCurve(int index, Vector2 controlPoints[4]) const;

			struct CurvePoint
			{
				Vector2 controlA;
				Vector2 Position;
				Vector2 controlB;

				void Serialise(io::Archive& archive)
				{
					archive.Serialise(controlA);
					archive.Serialise(Position);
					archive.Serialise(controlB);
				}
			};

			std::vector<CurvePoint> m_curvePoints;
			Vector2 m_boundsMin;
			Vector2 m_boundsMax;
			float m_length;
		};
	}
}