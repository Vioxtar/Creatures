#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "OpenGLShaderUtils.h"

#define _USE_MATH_DEFINES
#include <math.h>




using namespace std;
using namespace glm;

void ShapeUtils_Init();
void SetShapesTransform(mat4 transform);
void BatchDrawAllShapes();

void DrawCircle(vec3 color, vec2 position, float radius, float fillPercentage);

struct ShapeData
{
	vector<float> vertices; // Shape vertices, including positions and normals
	vector<unsigned int> indices; // Correspoding
};


vector <vec2> CreateCircleBase(unsigned int numOfPoints, float circleFraction);
ShapeData CreateStrokeVertices(vector<vec2>& shapeBase, bool loop);