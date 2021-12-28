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
		int BezierPath::AddPoint(const Vector2& Position, const Vector2& controlA, const Vector2& controlB)
		{
			CurvePoint point;
			point.controlA = Position + controlA;
			point.Position = Position;
			point.controlB = Position + controlB;
			m_curvePoints.push_back(point);
			CalculateBounds();
			CalculateLength();
			return (int)m_curvePoints.size() / 3;
		}

		void BezierPath::RemovePoint(int index)
		{
			ion::debug::Assert(index < GetNumPoints(), "BezierPath::RemovePoint() - Out of range");
			m_curvePoints.erase(m_curvePoints.begin() + index);
			CalculateBounds();
			CalculateLength();
		}

		void BezierPath::SetPoint(int index, const Vector2& Position, const Vector2& controlA, const Vector2& controlB)
		{
			ion::debug::Assert(index < GetNumPoints(), "BezierPath::SetPoint() - Out of range");
			m_curvePoints[index].controlA = Position + controlA;
			m_curvePoints[index].Position = Position;
			m_curvePoints[index].controlB = Position + controlB;
			CalculateBounds();
			CalculateLength();
		}

		void BezierPath::GetPoint(int index, Vector2& Position, Vector2& controlA, Vector2& controlB) const
		{
			ion::debug::Assert(index < GetNumPoints(), "BezierPath::GetPoint() - Out of range");
			controlA = m_curvePoints[index].controlA;
			Position = m_curvePoints[index].Position;
			controlB = m_curvePoints[index].controlB;
			controlA -= Position;
			controlB -= Position;
		}

		void BezierPath::Move(const Vector2& offset)
		{
			for(int i = 0; i < m_curvePoints.size(); i++)
			{
				m_curvePoints[i].controlA += offset;
				m_curvePoints[i].Position += offset;
				m_curvePoints[i].controlB += offset;
			}

			CalculateBounds();
		}

		int BezierPath::GetNumCurves() const
		{
			return (m_curvePoints.size() > 0) ? ((int)m_curvePoints.size() - 1) : 0;
		}

		int BezierPath::GetNumPoints() const
		{
			return (int)m_curvePoints.size();
		}

		void BezierPath::GetCurve(int index, Vector2 controlPoints[4]) const
		{
			controlPoints[0] = m_curvePoints[index].Position;
			controlPoints[1] = m_curvePoints[index].controlB;
			controlPoints[2] = m_curvePoints[index+1].controlA;
			controlPoints[3] = m_curvePoints[index+1].Position;
		}

		void BezierPath::GetBounds(Vector2& boundsMin, Vector2& boundsMax) const
		{
			boundsMin = m_boundsMin;
			boundsMax = m_boundsMax;
		}

		float BezierPath::GetLength() const
		{
			return m_length;
		}

		void BezierPath::CalculateLength()
		{
			m_length = 0.0f;

			for (int i = 0; i < GetNumCurves(); i++)
			{
				Vector2 controlPoints[4];
				GetCurve(i, controlPoints);
				m_length += CalculateLength(controlPoints);
			}
		}

		Vector2 BezierPath::GetPosition(float time) const
		{
			if(m_curvePoints.size() == 0)
			{
				return Vector2(0.0f, 0.0f);
			}
			else if(m_curvePoints.size() == 1)
			{
				return m_curvePoints[0].Position;
			}
			else
			{
				ion::debug::Assert(time >= 0.0f && time <= 1.0f, "BezierPath::GetPosition() - Out of range");

				//Get curve index
				int numCurves = GetNumCurves();
				int curveIndex = (int)maths::Floor((float)(numCurves * time));

				if (curveIndex == numCurves)
				{
					return m_curvePoints[numCurves].Position;
				}
				else
				{
					//Calculate time within current curve
					float segmentTime = maths::Fmod(time * (float)numCurves, 1.0f);

					Vector2 controlPoints[4];
					GetCurve(curveIndex, controlPoints);
					return CalculatePosition(controlPoints, segmentTime);
				}
			}
		}

		Vector2 BezierPath::GetNormal(float time) const
		{
			if (m_curvePoints.size() == 0)
			{
				return Vector2(0.0f, 0.0f);
			}
			else if (m_curvePoints.size() == 1)
			{
				return Vector2(0.0f, 1.0f);
			}
			else
			{
				ion::debug::Assert(time >= 0.0f && time <= 1.0f, "BezierPath::GetNormal() - Out of range");

				//Get curve index
				int numCurves = GetNumCurves();
				int curveIndex = (int)maths::Floor((float)(numCurves * time));

				//Calculate time within current curve
				float segmentTime = maths::Fmod(time * (float)numCurves, 1.0f);

				if (curveIndex == numCurves)
				{
					//End point
					curveIndex = numCurves - 1;
					segmentTime = 1.0f;
				}

				Vector2 controlPoints[4];
				GetCurve(curveIndex, controlPoints);
				return CalculateNormal(controlPoints, segmentTime);
			}
		}

		int BezierPath::GetPositions(std::vector<Vector2>& Positions, float startTime, float endTime, int numPositions) const
		{
			if(m_curvePoints.size() == 0)
			{
				Positions.push_back(Vector2(0.0f, 0.0f));
			}
			else if(m_curvePoints.size() == 1)
			{
				Positions.push_back(m_curvePoints[0].Position);
			}
			else
			{
				float timePerPos = endTime / (numPositions-1);

				for(int i = 0; i < numPositions; i++)
				{
					Positions.push_back(GetPosition(startTime + (timePerPos * i)));
				}
			}
			
			return (int)Positions.size();
		}

		int BezierPath::GetDistributedPositions(std::vector<Vector2>& Positions, int numPositions) const
		{
			std::vector<float> times;
			CalculateSubdivisionTimes(times, numPositions, 100);

			Positions.resize(numPositions);

			for (int i = 0; i < times.size(); i++)
			{
				Positions[i] = GetPosition(times[i]);
			}

			return (int)Positions.size();
		}

		int BezierPath::GetNormals(std::vector<Vector2>& normals, float startTime, float endTime, int numNormals) const
		{
			if (m_curvePoints.size() == 0)
			{
				normals.push_back(Vector2(0.0f, 0.0f));
			}
			else if (m_curvePoints.size() < 4)
			{
				normals.push_back(Vector2(1.0f, 0.0f));
			}
			else
			{
				for (int i = 0; i < numNormals; i++)
				{
					float time = startTime + ((endTime - startTime) * ((float)i / (float)numNormals));
					normals.push_back(GetNormal(time));
				}
			}

			return (int)normals.size();
		}

		int BezierPath::GetDistributedNormals(std::vector<Vector2>& normals, int numNormals) const
		{
			std::vector<float> times;
			CalculateSubdivisionTimes(times, numNormals, 100);

			normals.resize(numNormals);

			for (int i = 0; i < times.size(); i++)
			{
				normals[i] = GetNormal(times[i]);
			}

			return (int)normals.size();
		}

		Vector2 BezierPath::CalculatePosition(const Vector2 controlPoints[4], float time) const
		{
			const float timeSq = time * time;
			const float timeCb = timeSq * time;
			const float timeInv = 1.0f - time;
			const float timeInvSq = timeInv * timeInv;
			const float timeInvCb = timeInvSq * timeInv;
			
			Vector2 Position = controlPoints[0] * timeInvCb;
			Position += controlPoints[1] * (3.0f * timeInvSq * time);
			Position += controlPoints[2] * (3.0f * timeInv * timeSq);
			Position += controlPoints[3] * timeCb;

			return Position;
		}

		Vector2 BezierPath::CalculateDerivative(const Vector2 controlPoints[4], float time) const
		{
			const float timeSq = time * time;
			const float timeInv = 1.0f - time;
			const float timeInvSq = timeInv * timeInv;

			Vector2 a = (controlPoints[1] - controlPoints[0]) * 3.0f;
			Vector2 b = (controlPoints[2] - controlPoints[1]) * 3.0f;
			Vector2 c = (controlPoints[3] - controlPoints[2]) * 3.0f;
			return (a * timeInvSq) + (b * timeInv * time * 3.0f) + (c * timeSq);
		}

		Vector2 BezierPath::CalculateNormal(const Vector2 controlPoints[4], float time) const
		{
			Vector2 derivative = CalculateDerivative(controlPoints, time);
			float length = derivative.GetLength();
			return Vector2(-derivative.y / length, derivative.x / length);
		}

		float BezierPath::CalculateLength(const Vector2 controlPoints[4])
		{
			Vector2 p0 = controlPoints[0] - controlPoints[1];
			Vector2 p1 = controlPoints[2] - controlPoints[1];
			Vector2 p2;
			Vector2 p3 = controlPoints[3] - controlPoints[2];

			float l0 = p0.GetLength();
			float l1 = p1.GetLength();
			float l3 = p3.GetLength();

			if (l0 > 0)
				p0 /= l0;
			if (l1 > 0)
				p1 /= l1;
			if (l3 > 0)
				p3 /= l3;

			p2 = -p1;

			float a = maths::Abs(p0.Dot(p1)) + maths::Abs(p2.Dot(p3));

			if ((a > 1.98f) || ((l0 + l1 + l3) < ((4 - a) * 8)))
				return l0 + l1 + l3;

			Vector2 bl[4];
			Vector2 br[4];

			bl[0] = controlPoints[0];
			bl[1] = (controlPoints[0] + controlPoints[1]) * 0.5f;

			Vector2 mid = (controlPoints[1] + controlPoints[2]) * 0.5f;

			bl[2] = (bl[1] + mid) * 0.5f;
			br[3] = controlPoints[3];
			br[2] = (controlPoints[2] + controlPoints[3]) * 0.5f;
			br[1] = (br[2] + mid) * 0.5f;
			br[0] = (br[1] + bl[2]) * 0.5f;
			bl[3] = br[0];

			return CalculateLength(bl) + CalculateLength(br);
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

			int j = (int)tvalues.size();
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

		int BezierPath::CalculateSubdivisionTimes(std::vector<float>& times, int divisionCount, int approximationSteps) const
		{
			if (GetNumCurves() == 0)
			{
				times.push_back(0.0f);
			}
			else
			{
				//Estimate distances along the curve in regular intervals of t by approximating the curve with a polyline
				float stepSize = 1.0f / approximationSteps;
				std::vector<float> distances;
				distances.resize(approximationSteps + 1);
				times.resize(divisionCount);

				//Start/end added manually
				times[0] = 0.0f;
				times[divisionCount - 1] = 1.0f;
				divisionCount -= 2;

				Vector2 last = GetPosition(0.0f);
				Vector2 current;

				for (int i = 1; i < distances.size(); i++)
				{
					current = GetPosition(i * stepSize);
					distances[i] = distances[i - 1] + (current - last).GetLength();
					last = current;
				}

				//Walk through distance array to find the t for each subdivision
				float sectionLength = distances[distances.size() - 1] / (divisionCount + 1);

				int tInt = 0;

				for (int i = 0; i < divisionCount; i++)
				{
					//Where the next subdivision should be
					float target = (i + 1) * sectionLength;

					//Skip until target is exceeded
					while (distances[tInt] < target)
						tInt++;

					float tFrac = maths::UnLerp(distances[tInt - 1], distances[tInt], target);
					times[i+1] = ((tInt - 1.0f + tFrac) / approximationSteps);
				}
			}

			return (int)times.size();
		}

		void BezierPath::CalculateBounds()
		{
			m_boundsMin.x = maths::FLOAT_MAX;
			m_boundsMin.y = maths::FLOAT_MAX;
			m_boundsMax.x = maths::FLOAT_MIN;
			m_boundsMax.y = maths::FLOAT_MIN;

			if(m_curvePoints.size() > 0)
			{
				ion::Vector2 Position;
				ion::Vector2 controlA;
				ion::Vector2 controlB;
				GetPoint(0, Position, controlA, controlB);
				AddBounds(Position, m_boundsMin, m_boundsMax);
				GetPoint(GetNumCurves(), Position, controlA, controlB);
				AddBounds(Position, m_boundsMin, m_boundsMax);

				ion::Vector2 curveMin;
				ion::Vector2 curveMax;

				for(int i = 0; i < GetNumCurves(); i++)
				{
					Vector2 controlPoints[4];
					GetCurve(i, controlPoints);
					BezierMinMax(controlPoints[0],
								controlPoints[1],
								controlPoints[2],
								controlPoints[3],
								curveMin, curveMax);

					AddBounds(curveMin, m_boundsMin, m_boundsMax);
					AddBounds(curveMax, m_boundsMin, m_boundsMax);
				}
			}
		}

		void BezierPath::Serialise(io::Archive& archive)
		{
			archive.Serialise(m_curvePoints, "curvePoints");
			archive.Serialise(m_boundsMin, "boundsMin");
			archive.Serialise(m_boundsMax, "boundsMax");
			archive.Serialise(m_length, "length");

			if (archive.GetDirection() == ion::io::Archive::Direction::In && m_curvePoints.size() == 0)
			{
				//Legacy format
				std::vector<Vector2> points;
				archive.Serialise(points, "controlPoints");
				m_curvePoints.resize(points.size() / 3);

				for (int i = 0; i < points.size(); i += 3)
				{
					m_curvePoints[i / 3].controlA = points[i + 0];
					m_curvePoints[i / 3].Position = points[i + 1];
					m_curvePoints[i / 3].controlB = points[i + 2];
				}
			}
		}
	}
}