#pragma once

#include <cmath>

namespace Math
{
	static constexpr float DEFAULT_EPSILON = 0.01f;

	inline bool IsNearlyZero(float value, float epsilon = DEFAULT_EPSILON)
	{
		return std::abs(value) < epsilon;
	}
}