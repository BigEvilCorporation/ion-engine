/*
	Symmetry Engine 2.0
	Matt Phillips (c) 2009

	Maths.h

	Maths constants and utility functions
*/

#pragma once

namespace ion
{
	namespace maths
	{
		const float PI = 3.14159264f;
		const float TWO_PI = PI * 2.0f;
		const float HALF_PI = PI / 2.0f;
		const float PI_DIV_180 = PI / 180.0f;
		const float ONE80_DIV_PI = 180.0f / PI;

		const float DEGREES_TO_RADIANS = PI / 180.0f;
		const float RADIANS_TO_DEGREES = 180.0f / PI;

		const float FLOAT_MIN = 1.175494351e-38f;
		const float FLOAT_MAX = 3.402823466e+38f;
		const float FLOAT_EPSILON = 1.19209290e-7f;
		const int S32_MIN = (-2147483647 - 1);
		const int S32_MAX = 2147483647;

		int Round(float x);
		int Wrap(int value, int divisor);
		int WrapRange(int value, int rangeMin, int rangeMax);
		int NextPowerOfTwo(int x);

		float RoundToNearest(float value, int nearest);
		float RoundUpToNearest(float value, int nearest);
		float RoundDownToNearest(float value, int nearest);
		
		float Sqrt(float value);
		float Pow(float value, float power);
		float Floor(float value);
		float Ceil(float value);
		float Abs(float value);
		int Abs(int value);
		float Fmod(float value, float divisor);
		float Lerp(float valueA, float valueB, float time);
		float UnLerp(float valueFrom, float valueTo, float valueCurrent);

		template <typename T> T Min(const T& a, const T& b);
		template <typename T> T Max(const T& a, const T& b);
		template <typename T> T Clamp(const T& value, const T& min, const T& max);

		template <typename T> T Square(const T& value);

		float Cos(float value);
		float Sin(float value);
		float Tan(float value);
		float Acos(float value);
		float Asin(float value);
		float Atan(float value);
		float Atan2(float y, float x);

		bool IsZero(float value);
		bool CompareFloats(float val1, float val2);

		void RandSeed(int seed);
		int RandInt();
		float RandFloat();

		float DegreesToRadians(float degrees);
		float RadiansToDegrees(float radians);
	}

	namespace maths
	{
		template <typename T> T Min(const T& a, const T& b)
		{
			return a < b ? a : b;
		}

		template <typename T> T Max(const T& a, const T& b)
		{
			return a > b ? a : b;
		}

		template <typename T> T Clamp(const T& value, const T& min, const T& max)
		{
			T clamped = value;

			if (value < min)
				clamped = min;
			else if (value > max)
				clamped = max;

			return clamped;
		}

		template <typename T> T Square(const T& value)
		{
			return value * value;
		}
	}
}
