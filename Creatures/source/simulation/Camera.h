#pragma once

#include "Simulation.h"
#include "..//utils/MathUtils.h"

#include "Settings.h"

using namespace glm;
using namespace std;

void Camera_Init();
void Camera_Update(double frameTime);

void Camera_Move(vec2 offset);
void Camera_Enable_Glide(bool glide);
void Camera_Zoom(float zoom);

mat4 GetSimSpaceToCameraTransform();
