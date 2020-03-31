#pragma once

#include "..//simulation/Simulation.h"
#include "..//utils/MathUtils.h"

#include "../Settings.h"

using namespace glm;
using namespace std;

namespace Camera
{
	void Initialize();
	void Update(double frameTime);

	void Move(vec2 offset);
	void InterpolatePositionTo(vec2 targetPos, float rate);
	void EnableGlide(bool glide);
	void Zoom(float zoom);

	mat4 GetSimSpaceToCameraTransform();
}

