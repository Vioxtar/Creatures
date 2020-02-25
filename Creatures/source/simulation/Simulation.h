#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "../Settings.h"

#include "Camera.h"

#include "..//utils/OpenGLShaderUtils.h"
#include "..//utils/ShapeUtils.h"
#include "..//utils/MathUtils.h"



using namespace glm;
using namespace std;

void Simulation_Init();
void Simulation_Update();



////////////////////////////////////////////////////////
// -- CREATURE DATA SNAPSHOTTING & UI INTERACTIONS -- //
////////////////////////////////////////////////////////

struct CreatureSnapshot
{
	vec2 pos;
};

vector<vec2> GetCreaturePositions();

void RemoveCreature(GLuint index);