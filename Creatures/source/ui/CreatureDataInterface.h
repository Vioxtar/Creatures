#pragma once

#include "../simulation/CreatureData.h"
#include <vector>
#include <glm/glm.hpp>

#ifndef GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

using namespace glm;
using namespace std;

vector<vec2> GetCreaturePositions();

CreatureData GetCreatureSnapshot(unsigned int creatureIndex);