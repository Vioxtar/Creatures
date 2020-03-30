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

using namespace glm;
using namespace std;

void Simulation_Init();
void Simulation_Update();
void Simulation_LogicPause(bool enablePause);