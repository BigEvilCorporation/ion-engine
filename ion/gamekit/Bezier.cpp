///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Bezier.cpp
// Date:		2nd April 2016
// Authors:		Matt Phillips
// Description:	Bezier curves
///////////////////////////////////////////////////

#include "Bezier.h"
#include "ion/core/debug/Debug.h"
#include "ion/maths/Maths.h"

namespace ion
{
	namespace gamekit
	{
		int BezierPath::AddPoint(const Vector2& position, const Vector2& controlA, const Vector2& controlB)
		{
			m_controlPoints.push_back(position + controlA);
			m_controlPoints.push_back(position);
			m_controlPoints.push_back(position + controlB);
			CalculateBounds();
			return m_controlPoints.size() / 3;
		}

		void BezierPath::RemovePoint(int index)
		{
			ion::debug::Assert(index < GetNumPoints(), "BezierPath::RemovePoint() - Out of range");
			m_controlPoints.erase(m_controlPoints.begin() + (index * 3), m_controlPoints.begin() + (index * 3) + 3);
			CalculateBounds();
		}

		void BezierPath::SetPoint(int index, const Vector2& position, const Vector2& controlA, const Vector2& controlB)
		{
			ion::debug::Assert(index < GetNumPoints(), "BezierPath::SetPoint() - Out of range");
			m_controlPoints[(index * 3) + 0] = position + controlA;
			m_controlPoints[(index * 3) + 1] = position;
			m_controlPoints[(index * 3) + 2] = position + controlB;
			CalculateBounds();
		}

		void BezierPath::GetPoint(int index, Vector2& position, Vector2& controlA, Vector2& controlB) const
		{
			ion::debug::Assert(index < GetNumPoints(), "BezierPath::GetPoint() - Out of range");
			controlA = m_controlPoints[(index * 3) + 0];
			position = m_controlPoints[(index * 3) + 1];
			controlB = m_controlPoints[(index * 3) + 2];
			controlA -= position;
			controlB -= position;
		}

		void BezierPath::Move(const Vector2& offset)
		{
			for(int i = 0; i < m_controlPoints.size(); i++)
			{
				m_controlPoints[i] += offset;
			}

			CalculateBounds();
		}

		int BezierPath::GetNumCurves() const
		{
			int numControlPoints = m_controlPoints.size();
			return (numControlPoints > 3) ? ((numControlPoints - 3) / 3) : 0;
		}

		int BezierPath::GetNumPoints() const
		{
			return m_controlPoints.size() / 3;
		}

		void BezierPath::GetBounds(Vector2& boundsMin, Vector2& boundsMax) const
		{
			boundsMin = m_boundsMin;
			boundsMax = m_boundsMax;
		}

		Vector2 BezierPath::GetPosition(float time) const
		{
			if(m_controlPoints.size() == 0)
			{
				return Vector2(0.0f, 0.0f);
			}
			else if(m_controlPoints.size() < 6)
			{
				return m_controlPoints[0];
			}
			else
			{
				ion::debug::Assert(time >= 0.0f && time <= 1.0f, "BezierPath::GetPosition() - Out of range");

				//Get curve index (points in groups of 3, skipping first)
				int curveIndex = maths::Floor(((float)(m_controlPoints.size() - 3) / 3.0f) * time);

				//Skip first point control A (it's meanlingless)
				int pointIndex = (curveIndex * 3) + 1;

				//Calculate time within current curve
				float totalTime = (m_controlPoints.size() - 3) / 3.0f;
				float segmentTime = maths::Fmod(time * totalTime, 1.0f);

				//Prev point pos/controlB + next point controlA/pos
				Vector2 controlPoints[4] = { m_controlPoints[pointIndex], m_controlPoints[pointIndex + 1], m_controlPoints[pointIndex + 2], m_controlPoints[pointIndex + 3] };
				return CalculatePosition(controlPoints, segmentTime);
			}
		}

		int BezierPath::GetPositions(std::vector<Vector2>& positions, float startTime, float endTime, int numPositions) const
		{
			if(m_controlPoints.size() == 0)
			{
				positions.push_back(Vector2(0.0f, 0.0f));
			}
			else if(m_controlPoints.size() < 4)
			{
				positions.push_back(m_controlPoints[0]);
			}
			else
			{
				for(int i = 0; i < numPositions; i++)
				{
					float time = startTime + ((endTime - startTime) * ((float)i / (float)numPositions));
					positions.push_back(GetPosition(time));
				}
			}
			
			return positions.size();
		}

		Vector2 BezierPath::CalculatePosition(const Vector2 controlPoints[4], float time) const
		{
			const float timeSq = time * time;
			const float timeCb = timeSq * time;
			const float timeInv = 1.0f - time;
			const float timeInvSq = timeInv * timeInv;
			const float timeInvCb = timeInvSq * timeInv;

			Vector2 position = controlPoints[0] * timeInvCb;
			position += controlPoints[1] * (3.0f * timeInvSq * time);
			position += controlPoints[2] * (3.0f * timeInv * timeSq);
			position += controlPoints[3] * timeCb;

			return position;
		}

		void AddBounds(const ion::Vector2& point, ion::Vector2& boundsMin, ion::Vector2& boundMax)
		{
			if(point.x > boundMax.x)
				boundMax.x = point.x;
			if(point.x < boundsMin.x)
				boundsMin.x = point.x;
			if(point.y > boundMax.y)
				boundMax.y = point.y;
			if(point.y < boundsMin.y)
				boundsMin.y = point.y;
		}

		void BezierMinMax(const ion::Vector2& p0, const ion::Vector2& p1, const ion::Vector2& p2, const ion::Vector2& p3, ion::Vector2& boundsMin, ion::Vector2& boundsMax)
		{
			boundsMin.x = maths::FLOAT_MAX;
			boundsMin.y = maths::FLOAT_MAX;
			boundsMax.x = maths::FLOAT_MIN;
			boundsMax.y = maths::FLOAT_MIN;
			AddBounds(p0, boundsMin, boundsMax);
			AddBounds(p3, boundsMin, boundsMax);

			std::vector<float> tvalues;
			float a, b, c, t, t1, t2, b2ac, sqrtb2ac;

			for(int i = 0; i < 2; i++)
			{
				if(i == 0)
				{
					b = 6 * p0.x - 12 * p1.x + 6 * p2.x;
					a = -3 * p0.x + 9 * p1.x - 9 * p2.x + 3 * p3.x;
					c = 3 * p1.x - 3 * p0.x;
				}
				else
				{
					b = 6 * p0.y - 12 * p1.y + 6 * p2.y;
					a = -3 * p0.y + 9 * p1.y - 9 * p2.y + 3 * p3.y;
					c = 3 * p1.y - 3 * p0.y;
				}

				if(ion::maths::Abs(a) < ion::maths::FLOAT_EPSILON)
				{
					if(ion::maths::Abs(b) < ion::maths::FLOAT_EPSILON)
					{
						continue;
					}

					t = -c / b;
					if(0 < t && t < 1)
					{
						tvalues.push_back(t);
					}

					continue;
				}

				b2ac = b * b - 4 * c * a;
				if(b2ac < 0)
				{
					continue;
				}

				sqrtb2ac = ion::maths::Sqrt(b2ac);

				t1 = (-b + sqrtb2ac) / (2 * a);
				if(0 < t1 && t1 < 1)
				{
					tvalues.push_back(t1);
				}

				t2 = (-b - sqrtb2ac) / (2 * a);
				if(0 < t2 && t2 < 1)
				{
					tvalues.push_back(t2);
				}
			}

			int j = tvalues.size();
			float mt;
			while(j--)
			{
				t = tvalues[j];
				mt = 1 - t;
				float xval = (mt * mt * mt * p0.x) + (3 * mt * mt * t * p1.x) + (3 * mt * t * t * p2.x) + (t * t * t * p3.x);
				float yval = (mt * mt * mt * p0.y) + (3 * mt * mt * t * p1.y) + (3 * mt * t * t * p2.y) + (t * t * t * p3.y);
				AddBounds(ion::Vector2(xval, yval), boundsMin, boundsMax);
			}
		}

		void BezierPath::CalculateBounds()
		{
			m_boundsMin.x = maths::FLOAT_MAX;
			m_boundsMin.y = maths::FLOAT_MAX;
			m_boundsMax.x = maths::FLOAT_MIN;
			m_boundsMax.y = maths::FLOAT_MIN;

			if(m_controlPoints.size() > 3)
			{
				ion::Vector2 position;
				ion::Vector2 controlA;
				ion::Vector2 controlB;
				GetPoint(0, position, controlA, controlB);
				AddBounds(position, m_boundsMin, m_boundsMax);
				GetPoint(GetNumCurves(), position, controlA, controlB);
				AddBounds(position, m_boundsMin, m_boundsMax);

				ion::Vector2 curveMin;
				ion::Vector2 curveMax;

				for(int i = 0; i < GetNumCurves(); i++)
				{
					int pointIndex = (i * 3) + 1;
					BezierMinMax(m_controlPoints[pointIndex],
						m_controlPoints[pointIndex + 1],
						m_controlPoints[pointIndex + 2],
						m_controlPoints[pointIndex + 3],
						curveMin, curveMax);

					AddBounds(curveMin, m_boundsMin, m_boundsMax);
					AddBounds(curveMax, m_boundsMin, m_boundsMax);
				}
			}
		}

		void BezierPath::Serialise(io::Archive& archive)
		{
			archive.Serialise(m_controlPoints, "controlPoints");
			archive.Serialise(m_boundsMin, "boundsMin");
			archive.Serialise(m_boundsMax, "boundsMax");
		}
	}
}