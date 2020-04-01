#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "../Settings.h"
#include "CreatureData.h"
#include "..//ui/Camera.h"

#include "..//utils/OpenGLShaderUtils.h"
#include "..//utils/ShapeUtils.h"
#include "..//utils/MathUtils.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace CreatureSpawns
{
	void Update();
	void AddOffspringCreature(unsigned int p1SSBO, unsigned int p2SSBO);
}