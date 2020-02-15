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

struct IndexedVertices
{
	vector<float> vertices; // Shape vertices, including positions and normals
	vector<unsigned int> indices; // Correspoding
};

struct VectorizedIndexedVertices
{
	vector<vec2> vertices;
	vector<uvec3> indices;
};

vector <vec2> CreateCircleBase(unsigned int numOfPoints, float circleFraction);
void AppendCircleBase(vector<vec2>* appendTarget, unsigned int numOfPoints, float circleFraction);
IndexedVertices CreateStrokeVertices(vector<vec2>& shapeBase, bool loop);