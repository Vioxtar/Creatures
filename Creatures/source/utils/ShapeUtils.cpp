#pragma once

#include "ShapeUtils.h"

///////////////////////
// -- SHAPE BASES -- //
///////////////////////

vector<vec2> createLineBase()
{
	return {
		vec2(0, 0),
		vec2(1, 0)
	};
}

vector<vec2> createGLetterBase()
{
	return {
		vec2(0, 0),
		vec2(0.5, 0),
		vec2(0.5, -0.5),
		vec2(-0.5, -0.5),
		vec2(-0.5, 0.5),
		vec2(0.5, 0.5),
	};
}

vector <vec2> CreateCircleBase(unsigned int numOfPoints, float circleFraction)
{
	double betaStart = -M_PI + (1 - circleFraction) * M_PI;
	double betaEnd = -betaStart;

	double interval = (betaEnd - betaStart) / numOfPoints;
	double beta = betaStart;

	vector<vec2> circleBase;

	for (unsigned int i = 0; i < numOfPoints; i++)
	{
		circleBase.push_back(vec2(sin(beta), cos(beta)));
		beta += interval;
	}

	return circleBase;
}

///////////////////////////////////
// -- STROKE VERTICES COMPUTE -- //
///////////////////////////////////


ShapeData CreateStrokeVertices(vector<vec2>& shapeBase, bool loop)
{
	vector<float> vertices;
	vector<unsigned int> indices;

	unsigned int indexOffset = 0;

	for (unsigned int i = 0; i < shapeBase.size(); i++) {
		unsigned int j = i + 1;
		if (j >= shapeBase.size())
		{
			if (!loop) break;
			else
			{
				j = 0;
				if (j == i) break;
			}
		}

		// Compute the up direction
		vec2 v1 = shapeBase[i];
		vec2 v2 = shapeBase[j];
		vec2 diff = v1 - v2;
		vec2 up(diff.y, -diff.x);
		up = normalize(up);

		// Top left, offset + 0
		vertices.push_back(v1.x);
		vertices.push_back(v1.y);
		vertices.push_back(up.x);
		vertices.push_back(up.y);

		// Bottom left, offset + 1
		vertices.push_back(v1.x);
		vertices.push_back(v1.y);
		vertices.push_back(-up.x);
		vertices.push_back(-up.y);

		// Top right, offset + 2
		vertices.push_back(v2.x);
		vertices.push_back(v2.y);
		vertices.push_back(up.x);
		vertices.push_back(up.y);

		// Bottom right, offset + 3
		vertices.push_back(v2.x);
		vertices.push_back(v2.y);
		vertices.push_back(-up.x);
		vertices.push_back(-up.y);

		// Update new indices for both triangles
		for (int k : {0, 2, 3}) // First triangle
			indices.push_back(indexOffset + k);
		for (int k : {0, 1, 3}) // Second triangle
			indices.push_back(indexOffset + k);

		// Connect to previous section
		// @TODO: One connection triangle is always unencessary (sometimes both in case of a straight line...), try and figure out which and not include it, cba for now
		if (i > 0 || loop)
		{
			unsigned int prevTopRight = indexOffset - 2;
			unsigned int prevBottomRight = indexOffset - 1;

			if (indexOffset == 0)
			{
				prevTopRight += shapeBase.size() * 4;
				prevBottomRight += shapeBase.size() * 4;
			}

			for (int k : {indexOffset + 0, indexOffset + 1, prevTopRight})
			{
				indices.push_back(k);
			}

			for (int k : {indexOffset + 0, indexOffset + 1, prevBottomRight})
			{
				indices.push_back(k);
			}
		}

		indexOffset += 4;
	}

	// Load into a shape data and return
	return ShapeData({
		vertices,
		indices
		});
}



/////////////////////////////
// -- ShapeGPUInstances -- //
/////////////////////////////

// Global data to beused in ShapeGPUInstances management
const char* vertexShaderPath("resources/graphical shaders/shape.vertexShader");
const char* fragmentShaderPath("resources/graphical shaders/shape.fragmentShader");
const char* transformUniformName("uTransform");

const unsigned int expectedInstanceCount = 100;
mat4 globalTransform = mat4();



struct ShapeDrawData
{
	vec3 color;
	vec2 position;
	float rotation;
	float scale;
	float strokeSize;
};

// Some ShapeDrawData size constants
const unsigned int colorSize = 3 * sizeof(GLfloat);
const unsigned int positionSize = 2 * sizeof(GLfloat);
const unsigned int singleFloatSize = 1 * sizeof(GLfloat);
const unsigned int shapeDrawDataBufferSegmentSize = colorSize + positionSize + singleFloatSize * 3;

const unsigned int maxInstanceSizeIncreaseOnPoolExceeded = 25;

class ShapeGPUInstance
{
	GLuint shapeProgramID;

	unsigned int shapeVAO;
	unsigned int shapeVBO;
	unsigned int shapeEBO;

	vector<unsigned int> indices;
	unsigned int shapeNumOfIndices;

	unsigned int maxInstanceCount;
	unsigned int shapeDrawData_instanceVBO;

	unsigned int poolSize;
	vector<vec2> positions;
	vector<vec3> colors;
	vector<float> rotations;
	vector<float> scales;
	vector<float> strokeSizes;


	void setVertexAttributePointers()
	{
		// Stride is the size of the attribute

		unsigned int offset = 0;

		// Color
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, colorSize, (void*)offset);
		glVertexAttribDivisor(2, 1);
		offset += colorSize * poolSize;

		// Position
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, positionSize, (void*)offset);
		glVertexAttribDivisor(3, 1);
		offset += positionSize * poolSize;

		// Rotation
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, singleFloatSize, (void*)offset);
		glVertexAttribDivisor(4, 1);
		offset += singleFloatSize * poolSize;

		// Scale
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, singleFloatSize, (void*)offset);
		glVertexAttribDivisor(5, 1);
		offset += singleFloatSize * poolSize;

		// Stroke Size
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, singleFloatSize, (void*)offset);
		glVertexAttribDivisor(6, 1);
	}

public:

	ShapeGPUInstance(vector<vec2> shapeBase, bool loop, GLuint shapeProgram, int expectedMaxInstanceCount)
	{
		shapeProgramID = shapeProgram;

		ShapeData shapeData = CreateStrokeVertices(shapeBase, loop);

		indices = shapeData.indices;
		shapeNumOfIndices = shapeData.indices.size();

		glGenVertexArrays(1, &shapeVAO);
		glBindVertexArray(shapeVAO);

		// Create vertices buffer
		glGenBuffers(1, &shapeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);
		glBufferData(GL_ARRAY_BUFFER, shapeData.vertices.size() * sizeof(GLfloat), shapeData.vertices.data(), GL_STATIC_DRAW);

		// Create elements buffer
		glGenBuffers(1, &shapeEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapeData.indices.size() * sizeof(GLuint), shapeData.indices.data(), GL_STATIC_DRAW);

		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		// Create empty instance VBO (NULL data), with a size that satisfies the expected max instance count
		maxInstanceCount = expectedMaxInstanceCount;
		glGenBuffers(1, &shapeDrawData_instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, shapeDrawData_instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, shapeDrawDataBufferSegmentSize * (long long)maxInstanceCount, NULL, GL_DYNAMIC_DRAW);

		setVertexAttributePointers();

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Reserve data vector memory
		colors.reserve(expectedMaxInstanceCount);
		positions.reserve(expectedMaxInstanceCount);
		rotations.reserve(expectedMaxInstanceCount);
		scales.reserve(expectedMaxInstanceCount);
		strokeSizes.reserve(expectedMaxInstanceCount);
	}

	void AddInstance(ShapeDrawData instanceData)
	{
		// Hopefully emplace_back won't cause memory copies
		colors.emplace_back(instanceData.color);
		positions.emplace_back(instanceData.position);
		rotations.emplace_back(instanceData.rotation);
		scales.emplace_back(instanceData.scale);
		strokeSizes.emplace_back(instanceData.strokeSize);

		poolSize++;
	}

	void DrawAllInstances()
	{
		glBindVertexArray(shapeVAO);

		// Make sure we have enough space
		if (poolSize > maxInstanceCount)
		{
			unsigned int bufferToDelete[1] = { shapeDrawData_instanceVBO };
			glDeleteBuffers(1, bufferToDelete);

			maxInstanceCount = poolSize + maxInstanceSizeIncreaseOnPoolExceeded;
			glGenBuffers(1, &shapeDrawData_instanceVBO);

			// Start out empty
			glBindBuffer(GL_ARRAY_BUFFER, shapeDrawData_instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, shapeDrawDataBufferSegmentSize * (long long)maxInstanceCount, NULL, GL_DYNAMIC_DRAW);
		}
		else
		{
			// Bind the instance VBO
			glBindBuffer(GL_ARRAY_BUFFER, shapeDrawData_instanceVBO);

			// Nullify the previous data to make sure we aren't drawing any old data
			glBufferSubData(GL_ARRAY_BUFFER, 0, shapeDrawDataBufferSegmentSize * (long long)poolSize, NULL);
		}


		// Write new data
		unsigned int offset = 0;
		unsigned int dataSize = colorSize * poolSize;

		// Colors
		glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, colors.data());
		offset += dataSize;

		// Positions
		dataSize = positionSize * poolSize;
		glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, positions.data());
		offset += dataSize;

		dataSize = singleFloatSize * poolSize;

		// Rotations
		glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, rotations.data());
		offset += dataSize;

		// Scales
		glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, scales.data());
		offset += dataSize;

		// Stroke Sizes
		glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, strokeSizes.data());



		// Set new vertex attributes (may only be necessary if pool size changed since last call)
		setVertexAttributePointers();



		// Draw call
		glUseProgram(shapeProgramID);
		SetUniformMatrix4(shapeProgramID, transformUniformName, globalTransform);
		glDrawElementsInstanced(GL_TRIANGLES, shapeNumOfIndices, GL_UNSIGNED_INT, 0, poolSize);

		glBindVertexArray(0);



		// Clear current data pool (hopefully without re-allocating memory via vector.erase(begin, end))
		colors.erase(colors.begin(), colors.end());
		positions.erase(positions.begin(), positions.end());
		rotations.erase(rotations.begin(), rotations.end());
		scales.erase(scales.begin(), scales.end());
		strokeSizes.erase(strokeSizes.begin(), strokeSizes.end());

		poolSize = 0;
	}
};


//////////////////////////////////
// -- INIT & PUBLIC INTERFACE-- //
//////////////////////////////////

// Cached ShapeGPUInstances & default settings
ShapeGPUInstance* circleShapeGPUInstance;

// Initialization of ShapeGPUInstances
void ShapeUtils_Init()
{
	//VFShader* shapeShader = new VFShader(vertexShaderPath, fragmentShaderPath);


	GLenum shapeShaderTypes[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	const char* shapeShaderPaths[] = { vertexShaderPath, fragmentShaderPath };
	GLuint shapeProgram = CreateLinkedShaderProgram(2, shapeShaderTypes, shapeShaderPaths);

	vector<vec2> circleBase = CreateCircleBase(10, 1);
	circleShapeGPUInstance = new ShapeGPUInstance(circleBase, true, shapeProgram, expectedInstanceCount);
}

void SetShapesTransform(mat4 transform)
{
	globalTransform = transform;
}

// Batch draw all shapes
void BatchDrawAllShapes()
{
	circleShapeGPUInstance->DrawAllInstances();
}

// Comfort draw calls for circles and lines
void DrawCircle(vec3 color, vec2 position, float radius, float fillPercentage)
{
	ShapeDrawData circleDrawData;
	circleDrawData.color = color;
	circleDrawData.position = position;
	circleDrawData.scale = radius * (2 - fillPercentage) / 2;
	circleDrawData.rotation = 0;
	circleDrawData.strokeSize = fillPercentage * radius / 2;
	circleShapeGPUInstance->AddInstance(circleDrawData);
}
