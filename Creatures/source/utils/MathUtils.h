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

	static float randomNegate()
	{
		return random() < 0.5 ? 1.0 : -1.0;
	}

	static int randomIntRange(int min, int max)
	{
		return min + int(random() * (max - min + 1));
	}

	static float randomRange(float min, float max)
	{
		return min + random() * (max - min);
	}
	
	static float randomRangeMinGravity(float min, float max, float minGravity)
	{
		return min + pow(random(), minGravity) * (max - min);
	}

	static void randomJitter(float& value, float jitterSize, float gravityToZeroJitter)
	{
		value += pow(random(), gravityToZeroJitter) * jitterSize * randomNegate();
	}

}
