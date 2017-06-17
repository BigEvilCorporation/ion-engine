/*
	Symmetry Engine 2.0
	Matt Phillips (c) 2009

	Maths.cpp

	Maths constants and utility functions
*/

#include "maths/Maths.h"

#if defined ION_PLATFORM_DREAMCAST
#include <dc/fmath.h>
#endif

#include <math.h>
#include <stdlib.h>

namespace ion
{
	namespace maths
	{
		int Round(float x)
		{
			return (int)(x + 0.5f);
		}

		float RoundToNearest(float value, int nearest)
		{
			return floorf(value / nearest) * nearest;
		}

		float RoundUpToNearest(float value, int nearest)
		{
			float modulus = Fmod(value, nearest);
			if(modulus == 0.0f)
			{
				return value;
			}

			return value + nearest - modulus;
		}

		int NextPowerOfTwo(int x)
		{
			x--;
			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;
			x++;
			return x;
		}

		float Sqrt(float value)
		{
#if defined ION_PLATFORM_DREAMCAST
			return fsqrt(value);
#else
			return sqrtf(value);
#endif
		}

		float Floor(float value)
		{
			return floorf(value);
		}

		float Ceil(float value)
		{
			return ceilf(value);
		}

		float Clamp(float value, float min, float max)
		{
			float clamped = value;

			if(value < min)
				clamped = min;
			else if(value > max)
				clamped = max;

			return clamped;
		}

		float Abs(float value)
		{
			return fabs(value);
		}

		float Fmod(float value, float divisor)
		{
#if defined ION_PLATFORM_DREAMCAST
			return value - divisor * floorf(value / divisor);
#else
			return fmod(value, divisor);
#endif
		}

		float Lerp(float valueA, float valueB, float time)
		{
			return valueA + ((valueB - valueA) * time);
		}

		float UnLerp(float valueFrom, float valueTo, float valueCurrent)
		{
			float time = 0.0f;

			if(valueTo - valueFrom != 0.0f)
			{
				time = (valueFrom - valueCurrent) / (valueFrom - valueTo);
			}

			return time;
		}

		float Cos(float value)
		{
#if defined ION_PLATFORM_DREAMCAST
			return fcos(value);
#else
			return cosf(value);
#endif
		}

		float Sin(float value)
		{
#if defined ION_PLATFORM_DREAMCAST
			return fsin(value);
#else
			return sinf(value);
#endif
		}

		float Tan(float value)
		{
			return tanf(value);
		}

		float Acos(float value)
		{
			return acosf(value);
		}

		float Asin(float value)
		{
			return asinf(value);
		}

		float Atan(float value)
		{
			return atanf(value);
		}

		float Atan2(float y, float x)
		{
			return atan2f(y, x);
		}

		float DegreesToRadians(float degrees)
		{
			return degrees * DEGREES_TO_RADIANS;
		}

		float RadiansToDegrees(float radians)
		{
			return radians * RADIANS_TO_DEGREES;
		}

		int RandInt()
		{
			return rand();
		}

		float RandFloat()
		{
			return (float)rand() / (float)RAND_MAX;
		}
	}
}
