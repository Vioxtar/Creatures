#include "Renderer.h"




// Drawing related structs
struct InstancedDrawCallData
{
	GLuint program;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	GLuint numOfIndices;
};

// Render draw call datas
InstancedDrawCallData drawCallData_DefaultCreatureBody;
InstancedDrawCallData drawCallData_CreatureHeatmap;

void InitOpenGLSettings()
{
	//glEnable(GL_MULTISAMPLE);

	// Set some blending/depth settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Clean looking
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA); // Eye candy clipping but dark cores
	//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA); // Saturated, eye-candy look, opaqueness increases with color lightness
	//glBlendFunc(GL_ZERO, GL_SRC_COLOR); // Multiplicative
}


void InitDrawingPrograms()
{

	// Default creature body
	{
		vector<vec2> creatureBodyVerts;
		creatureBodyVerts.push_back(vec2(0, 0));
		AppendCircleBase(&creatureBodyVerts, RENDER_NUM_OF_CREATURE_BODY_VERTICES, 1.0);

		vector<uvec3> creatureBodyElementIndices;
		for (int i = 1; i <= RENDER_NUM_OF_CREATURE_BODY_VERTICES; i++)
		{
			creatureBodyElementIndices.push_back(uvec3(0, i, i % RENDER_NUM_OF_CREATURE_BODY_VERTICES + 1));
		}

		GLenum creatureBodyShaderTypes[] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER
		};
		const char* creatureBodyShaderPaths[] = {
			"resources/graphical shaders/creature_body.vertexShader",
			"resources/graphical shaders/creature_body.fragmentShader"
		};
		GLuint creatureBodyProgram = CreateLinkedShaderProgram(2, creatureBodyShaderTypes, creatureBodyShaderPaths, NULL);

		drawCallData_DefaultCreatureBody.program = creatureBodyProgram;
		drawCallData_DefaultCreatureBody.numOfIndices = creatureBodyElementIndices.size() * 3;

		glGenVertexArrays(1, &drawCallData_DefaultCreatureBody.VAO);
		glBindVertexArray(drawCallData_DefaultCreatureBody.VAO);

		// Create vertices buffer
		glGenBuffers(1, &drawCallData_DefaultCreatureBody.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, drawCallData_DefaultCreatureBody.VBO);
		glBufferData(GL_ARRAY_BUFFER, creatureBodyVerts.size() * sizeof(vec2), creatureBodyVerts.data(), GL_STATIC_DRAW);

		// Create elements buffer
		glGenBuffers(1, &drawCallData_DefaultCreatureBody.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawCallData_DefaultCreatureBody.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, creatureBodyElementIndices.size() * sizeof(uvec3), creatureBodyElementIndices.data(), GL_STATIC_DRAW);

		// Vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);

		// Unbind (unbinding the vertex array essentially links the currently bound VBO and EBO)
		glBindVertexArray(0);
	}

}


void Renderer::Initialize()
{
	InitOpenGLSettings();
	InitDrawingPrograms();
}

void Renderer::Render()
{

	uint numOfInstances = creature_count;

	// Creature bodies

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_SkinRGBColors.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Positions.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_ForwardDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Angles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Radii.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_Lives.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_Energies.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_SkinPatterns.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_CollidersPositions.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_CollidersRadii.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_CollidersCounts.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, creature_Spikes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, creature_Feeders.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, creature_Shields.bufferHandle);

	glBindVertexArray(drawCallData_DefaultCreatureBody.VAO);
	glUseProgram(drawCallData_DefaultCreatureBody.program);
	SetUniformMatrix4(drawCallData_DefaultCreatureBody.program, "uTransform", Camera::GetSimSpaceToCameraTransform());
	SetUniformUInteger(drawCallData_DefaultCreatureBody.program, "uMaxNumOfColliders", CREATURE_MAX_NUM_OF_COLLIDERS);
	SetUniformFloat(drawCallData_DefaultCreatureBody.program, "uCreatureMaxEnergy", CREATURE_MAX_ENERGY.value);
	SetUniformFloat(drawCallData_DefaultCreatureBody.program, "uCreatureMaxLife", CREATURE_MAX_LIFE.value);
	SetUniformFloat(drawCallData_DefaultCreatureBody.program, "uCreatureTransparencyEnergyExponent", RENDER_CREATURE_TRANSPARENCY_ENERGY_EXPONENT.value);
	glDrawElementsInstanced(GL_TRIANGLES, drawCallData_DefaultCreatureBody.numOfIndices, GL_UNSIGNED_INT, 0, numOfInstances);
	glBindVertexArray(0);
}