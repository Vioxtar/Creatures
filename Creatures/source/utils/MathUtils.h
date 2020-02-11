#pragma once

#include <iostream>
#include <algorithm> 
#include <limits>
#include <math.h>

namespace mathutils
{
	using namespace mathutils;

	static constexpr int clamp(int value, int minVal, int maxVal)
	{
		return std::max(minVal, std::min(value, maxVal));
	}

	static constexpr float clamp(float value, float minVal, float maxVal)
	{
		return std::max(minVal, std::min(value, maxVal));
	}

	static float random()
	{
		return (float)((double)rand() / (RAND_MAX));
	}
}
