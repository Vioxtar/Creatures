#include "Simulation.h"



///////////////////
// -- STRUCTS -- //
///////////////////

struct InstancedDrawCallData
{
	GLuint program;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	GLuint numOfIndices;
};


// Just used for passing around creature data arguments more neatly
struct CreatureData
{
	vec3 col;
	vec2 pos;
	vec2 vel;
	GLfloat rad;
	GLfloat life;
	GLuint tile;
};


/////////////////////////////
// -- WORKING VARIABLES -- //
/////////////////////////////

// Creature Data SSBOs
GLuint creature_colrs; // The colors of the creatures
GLuint creature_poses; // The positions of the creatures
GLuint creature_velos; // The position velocities of the creatures
GLuint creature_radii; // The radii of the creatures
GLuint creature_lives; // The health of the creatures
GLuint creature_tiles; // Uniform grid tile index keeping

GLuint creature_gprps; // A general purpose buffer of vec2's

GLuint creature_count;
GLuint max_supported_creature_count_by_current_buffers;
const GLuint max_supported_creature_count_increase_on_reallocation = 500;

const GLenum ssbo_usage = GL_STREAM_READ;

// Logic programs
const GLuint workGroupSize = 1024;

GLuint applyVelocitiesProgram;
GLuint borderPhysicsProgram;
GLuint uniformGridBindProgram;
GLuint uniformGridUnBindProgram;
GLuint creatureCollisionsProgram;


// Render working variables
const char* cameraTransformUniformName("uTransform");
mat4 cameraTransform = mat4(1.0f);

// Render draw call datas
InstancedDrawCallData circleDrawCallData;



//////////////////////
// -- DRAW UTILS -- //
//////////////////////


InstancedDrawCallData InitializeInstancedDrawCallData(GLuint program, vector<vec2> shapeBase, bool loop)
{
	InstancedDrawCallData newDrawCallData;

	newDrawCallData.program = program;

	ShapeData shapeData = CreateStrokeVertices(shapeBase, loop);
	newDrawCallData.numOfIndices = shapeData.indices.size();
	
	glGenVertexArrays(1, &newDrawCallData.VAO);
	glBindVertexArray(newDrawCallData.VAO);

	// Create vertices buffer
	glGenBuffers(1, &newDrawCallData.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, newDrawCallData.VBO);
	glBufferData(GL_ARRAY_BUFFER, shapeData.vertices.size() * sizeof(GLfloat), shapeData.vertices.data(), GL_STATIC_DRAW);

	// Create elements buffer
	glGenBuffers(1, &newDrawCallData.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newDrawCallData.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapeData.indices.size() * sizeof(GLuint), shapeData.indices.data(), GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Unbind (unbinding the vertex array essentially stores the currently bound VBO and EBO)
	glBindVertexArray(0);

	return newDrawCallData;
}

void InstancedDrawCall(InstancedDrawCallData data, GLuint numOfInstances)
{
	glBindVertexArray(data.VAO);

	glUseProgram(data.program);
	SetUniformMatrix4(data.program, cameraTransformUniformName, cameraTransform);
	glDrawElementsInstanced(GL_TRIANGLES, data.numOfIndices, GL_UNSIGNED_INT, 0, numOfInstances);

	glBindVertexArray(0);
}


//////////////////////
// -- SSBO UTILS -- //
//////////////////////

GLuint InitEmptySSBO(GLuint attributeCount, GLuint attributeSize)
{
	// Create a new SSBO
	GLuint newSSBO;
	glGenBuffers(1, &newSSBO);

	// Initialize with NULL data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
	GLuint size = attributeCount * attributeSize;
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, ssbo_usage);

	return newSSBO;
}

void ExpandSSBO(GLuint& oldSSBO, GLuint attributeSize, GLuint attributeCountAdd)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, oldSSBO);

	// Get old size, and calculate new size
	GLint64 oldSize;
	glGetBufferParameteri64v(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &oldSize);
	GLuint newSize = (GLuint)oldSize + attributeCountAdd * attributeSize;

	// Create a new empty SSBO
	GLuint newSSBO;
	glGenBuffers(1, &newSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newSize, NULL, ssbo_usage);

	// Copy old SSBO to new SSBO
	glBindBuffer(GL_COPY_READ_BUFFER, oldSSBO);
	glBindBuffer(GL_COPY_WRITE_BUFFER, newSSBO);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, oldSize); // This causes a performance running for some reason

	// Delete old SSBO
	GLuint buffersToDelete = { oldSSBO };
	glDeleteBuffers(1, &buffersToDelete);

	// Finalize
	oldSSBO = newSSBO;
}


///////////////////////////////////////////////
// -- CREATURE MANIPULATION COMFORT TOOLS -- //
///////////////////////////////////////////////

void SetCreatureAttribute(GLuint ssbo, GLuint creatureIndex, GLuint attributeSize, const void* data)
{
	GLuint writeOffset = attributeSize * creatureIndex;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, writeOffset, attributeSize, data);
}

void RemoveCreatureAttribute(GLuint ssbo, GLuint creatureIndex, GLuint attributeSize)
{
	GLuint lastCreatureIndex = creature_count - 1;
	if (lastCreatureIndex != creatureIndex)
	{
		// Copy the data in lastCreatureIndex to our creatureIndex
		glBindBuffer(GL_COPY_READ_BUFFER, ssbo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, ssbo);

		GLuint readOffset = lastCreatureIndex * attributeSize;
		GLuint writeOffset = creatureIndex * attributeSize;
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, attributeSize);
	}

	// Nullify the new last creature index
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, lastCreatureIndex, attributeSize, NULL);
}

// @TODO: At some point in time we'll eventually need to account for the absolute max buffer size supported by our GPU
GLuint AddCreature(CreatureData newCreatureData)
{
	// Check if we're exceeding capacity
	if (creature_count >= max_supported_creature_count_by_current_buffers)
	{
		// We're exceeding capacities for all of our SSBOs, expand them
		GLuint increaseSize = max_supported_creature_count_increase_on_reallocation;
		ExpandSSBO(creature_colrs, 3 * sizeof(GLfloat), increaseSize);
		ExpandSSBO(creature_poses, 2 * sizeof(GLfloat), increaseSize);
		ExpandSSBO(creature_velos, 2 * sizeof(GLfloat), increaseSize);
		ExpandSSBO(creature_gprps, 2 * sizeof(GLfloat), increaseSize);
		ExpandSSBO(creature_radii, sizeof(GLfloat), increaseSize);
		ExpandSSBO(creature_lives, sizeof(GLfloat), increaseSize);
		ExpandSSBO(creature_tiles, sizeof(GLuint), increaseSize);


		max_supported_creature_count_by_current_buffers += max_supported_creature_count_increase_on_reallocation;
	}

	GLuint newCreatureIndex = creature_count;
	SetCreatureAttribute(creature_colrs, newCreatureIndex, 3 * sizeof(GLfloat), &newCreatureData.col);
	SetCreatureAttribute(creature_poses, newCreatureIndex, 2 * sizeof(GLfloat), &newCreatureData.pos);
	SetCreatureAttribute(creature_velos, newCreatureIndex, 2 * sizeof(GLfloat), &newCreatureData.vel);
	SetCreatureAttribute(creature_gprps, newCreatureIndex, 2 * sizeof(GLfloat), NULL);
	SetCreatureAttribute(creature_radii, newCreatureIndex, sizeof(GLfloat), &newCreatureData.rad);
	SetCreatureAttribute(creature_lives, newCreatureIndex, sizeof(GLfloat), &newCreatureData.life);
	SetCreatureAttribute(creature_tiles, newCreatureIndex, sizeof(GLuint), &newCreatureData.tile);


	creature_count++;

	return newCreatureIndex;
}

void RemoveCreature(GLuint creatureIndex)
{
	if (creature_count <= 0)
		return;

	RemoveCreatureAttribute(creature_colrs, creatureIndex, 3 * sizeof(GLfloat));
	RemoveCreatureAttribute(creature_poses, creatureIndex, 2 * sizeof(GLfloat));
	RemoveCreatureAttribute(creature_velos, creatureIndex, 2 * sizeof(GLfloat));
	RemoveCreatureAttribute(creature_gprps, creatureIndex, 2 * sizeof(GLfloat));
	RemoveCreatureAttribute(creature_radii, creatureIndex, sizeof(GLfloat));
	RemoveCreatureAttribute(creature_lives, creatureIndex, sizeof(GLfloat));
	RemoveCreatureAttribute(creature_tiles, creatureIndex, sizeof(GLuint));


	creature_count--;
}


//////////////////
// -- BRAINS -- //
//////////////////

/*
	
	We don't even need to define our C++ structs, we can just create a buffer with the appropriate size of bytes and send it away.
	Struct layout will be dynamically defined in the GLSL shader source via DEFINE replacers.

*/

struct Link
{
	float scalar;
	float bias;
};

struct Node
{
	float value;
	Link links[5];
};

struct Brain
{
	uint structure[5];
	Node nodes[10];
};


////////////////////////
// -- UNIFORM GRID -- //
////////////////////////

	/*

	(This is outdated but still relevant for understanding uniform grid approach)

	Assuming:
		1. Creature sense radius is measured from center of creature
		2. Creature max body radius < creature max sense radius
		3. Creature c1 can sense creature c2 if dist(c1, c2) - c2.radius < c1.senseRadius
	We may safely define the max creature interaction distance threshold as:
		InteractDist: MaxCreatureRadius + MaxCreatureSenseRadius
	... beyond which creatures do not "know" (sense or collide) each other.
	We build a uniform grid made out of square tiles with dimensions InteractDist x InteractDist, correspond
	creatures to their relevant tile, and then iterate said tile and its neighbourhood, hence the dimensions of each interaction 'bubble' is:
		BufferedTileDim = 3 * InteractDist x 3 * InteractDist
	The uniform grid will cover the entire simulation space.
	Inside every tile of the uniform grid is a creature index buffer with the size MaxTileCreatureCapacity, where:
		MaxTileCreatureCapacity >= the maximum number of creatures, with the minimal radii, that can be squeezed into
								   a InteractDist x InteractDist square.
		To satisfy this property, assuming creatures are 'hard' (do not allow prolonged penetrative collisions) we must make sure that:

			MaxTileCreatureCapacity >= InteractDist^2 / (pi * MinRadius^2)
	In the uniform grid binding phase, each creature will associate itself to a single tile by:
		1. Writing its own index into the tile creature indices buffer
		2. Storing the tile it was mapped to in a specialized SSBO whose size is 2 * sizeof(int) * creature_count, where
		   two numbers are stored:
			a. the 1D tile index
			b. the offset within the tile buffer
	In the usage phase, for every creature that is inside tile t and its neighbourhood, we perform a naive physics / sensory solution for all
	other creatures inside t, and the 8 (or less, in the case of simulation edge tiles) neighbouring tiles. To avoid duplicate
	calculations, we avoid (continue) entering solution if our creature index is bigger than the other (in which case the other
	will take care of us).
	In the uniform grid unbind phase, every creature will reset:
		1. The uniform grid SSBO's tile (by checking its specialized uniform grid map attributes)
		2. The map attributes(?)
		------------
	Thus we are to rebuild the uniform grid SSBO every time the following values are changed:
		1. InteractDist (check if it changed whenever MaxCreatureRadius and MaxCreatureSenseRadius change) (to change tile dimensions)
		2. MinCreatureRadius (to change MaxTileCreatureCapacity)
		3. Simulation Height / Width dimensions (to change the number of tiles)
	The number of tiles to be spread across the simulation space is: GridXDim x GridYDim, where:
		GridXDim = ceiling(SimulationWidth / InteractDist)
		GridYDim = ceiling(SimulationHeight / InteractDist)
	We can 1D-ify the 2D uniform grid:
		1DTileIndex = xTileIndex + GridXDim * yTileIndex
	As a creature, we would like to find our 'intile' xTileIndex and yTileIndex:
		xTileIndex = floor((remapped pos.x) * GridXDim / SimWidth)
		yTileIndex = floor((remapped pos.y) * GridYDim / SimHeight)
		Where we remap pos.x from (-SimWidth / 2, SimWidth / 2) to (0, SimWidth)
					   pos.y from (-SimHeght / 2, SimHeght / 2) to (0, SimHeght)
		For example:
			Remmaped pos.x = pos.x + simWidth / 2
	*/

// The uniform grid buffer: stores the creature indices per tiles
GLuint ugrid_SSBO;

// Used to find out when we need to rebuild the uniform grid
float ugrid_LastMaxCreatureRadius;
float ugrid_LastMaxCreatureSenseRadius;
float ugrid_LastMinCreatureRadius;
float ugrid_LastSimulationWidth;
float ugrid_LastSimulationHeight;
float ugrid_LastInteractDist;

// Working variables (updated and passed down to programs as uniforms)
GLuint ugrid_GridXDim;
GLuint ugrid_GridYDim;
GLfloat ugrid_SimWidth;
GLfloat ugrid_SimHeight;
GLuint ugrid_IndicesInTile;


void BuildUniformGrid()
{
	// Start by checking whether or not we need to rebuild the grid
	float newMaxCreatureRadius = CREATURE_MAX_RADIUS.value;
	float newMaxCreatureSenseRadius = CREATURE_MAX_SENSE_RADIUS.value;
	float newMinCreatureRadius = CREATURE_MIN_RADIUS.value;
	float newSimulationWidth = SIMULATION_WIDTH.value;
	float newSimulationHeight = SIMULATION_HEIGHT.value;
	float newInteractDist = newMaxCreatureRadius + newMaxCreatureSenseRadius;

	bool interactDistChanged = ugrid_LastInteractDist != newInteractDist;
	bool minCreatureRadiusChanged = ugrid_LastMinCreatureRadius != newMinCreatureRadius;
	bool simulationDimsChanged = (ugrid_LastSimulationWidth != newSimulationWidth) || (ugrid_LastSimulationHeight != newSimulationHeight);

	bool needToRebuildGrid = interactDistChanged || minCreatureRadiusChanged || simulationDimsChanged;

	if (!needToRebuildGrid)
		return;

	// Things changed, we need to rebuild the uniform grid and update a bunch of values

	// A small buffer around our simulation width/height ensures that creatures never over-step our uniform grid space
	// as long as they remain within the actual simulation space
	float uniformGridSimulationDimensionBuffer = SIMULATION_UNIFORM_GRID_DIMENSION_BUFFER;
	ugrid_SimWidth = newSimulationWidth + uniformGridSimulationDimensionBuffer;
	ugrid_SimHeight = newSimulationHeight + uniformGridSimulationDimensionBuffer;

	// Calculate how many tiles we are spreading over our grid
	ugrid_GridXDim = (GLuint)std::max(1, (int)floor(ugrid_SimWidth / newInteractDist));
	ugrid_GridYDim = (GLuint)std::max(1, (int)floor(ugrid_SimHeight / newInteractDist));

	// Calculate how many creatures we can squeeze in a tile, and scale
	uint maxNumOfCreaturesSqueezableInTile = ceil(pow(newInteractDist, 2) / (M_PI * (pow(newMinCreatureRadius, 2))));
	maxNumOfCreaturesSqueezableInTile *= SIMULATION_UNIFORM_GRID_TILE_CREATURE_CAPACITY_SCALAR;
	
	// Calculate buffer sizes
	ugrid_IndicesInTile = maxNumOfCreaturesSqueezableInTile + 1; // We also have our creature counter at the first index!
	uint tileBufferSize = ugrid_IndicesInTile * sizeof(GLuint);
	GLuint newSSBOSize = ugrid_GridXDim * ugrid_GridYDim * tileBufferSize;

	// Delete the old buffer
	GLuint buffersToDelete = { ugrid_SSBO };
	glDeleteBuffers(1, &buffersToDelete);

	// Create the new buffer
	glGenBuffers(1, &ugrid_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ugrid_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newSSBOSize, NULL, ssbo_usage);


	// Finally, update our values again
	ugrid_LastMaxCreatureRadius = newMaxCreatureRadius;
	ugrid_LastMaxCreatureSenseRadius = newMaxCreatureSenseRadius;
	ugrid_LastMinCreatureRadius = newMinCreatureRadius;
	ugrid_LastSimulationWidth = newSimulationWidth;
	ugrid_LastSimulationHeight = newSimulationHeight;
	ugrid_LastInteractDist = newInteractDist;
}



/////////////////////////////////////
// -- SIMULATION INITIALIZATION -- //
/////////////////////////////////////

void Simulation_Init()
{
	const unsigned int numOfCreaturesOnInit = SIMULATION_NUM_OF_CREATURES_ON_INIT.value;

	creature_count = 0;
	max_supported_creature_count_by_current_buffers = numOfCreaturesOnInit;
	   
	// Initialize creature data SSBOs
	creature_colrs = InitEmptySSBO(numOfCreaturesOnInit, 3 * sizeof(GLfloat));
	creature_poses = InitEmptySSBO(numOfCreaturesOnInit, 2 * sizeof(GLfloat));
	creature_velos = InitEmptySSBO(numOfCreaturesOnInit, 2 * sizeof(GLfloat));
	creature_gprps = InitEmptySSBO(numOfCreaturesOnInit, 2 * sizeof(GLfloat));
	creature_radii = InitEmptySSBO(numOfCreaturesOnInit, sizeof(GLfloat));
	creature_lives = InitEmptySSBO(numOfCreaturesOnInit, sizeof(GLfloat));
	creature_tiles = InitEmptySSBO(numOfCreaturesOnInit, sizeof(GLuint));



	// Initialize logic programs
	GLenum applyVelocitiesShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* applyVelocitiesShaderPaths[] = { "resources/compute shaders/apply_velocities.computeShader" };
	applyVelocitiesProgram = CreateLinkedShaderProgram(1, applyVelocitiesShaderTypes, applyVelocitiesShaderPaths, NULL);

	GLenum borderPhysicsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* borderPhysicsShaderPaths[] = { "resources/compute shaders/border_physics.computeShader" };
	borderPhysicsProgram = CreateLinkedShaderProgram(1, borderPhysicsShaderTypes, borderPhysicsShaderPaths, NULL);

	GLenum uniformGridBindShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* uniformGridBindShaderPaths[] = { "resources/compute shaders/uniform_grid_bind.computeShader" };
	uniformGridBindProgram = CreateLinkedShaderProgram(1, uniformGridBindShaderTypes, uniformGridBindShaderPaths, NULL);
	
	GLenum uniformGridUnBindShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* uniformGridUnBindShaderPaths[] = { "resources/compute shaders/uniform_grid_unbind.computeShader" };
	uniformGridUnBindProgram = CreateLinkedShaderProgram(1, uniformGridUnBindShaderTypes, uniformGridUnBindShaderPaths, NULL);

	GLenum creatureCollisionsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureCollisionsShaderPaths[] = { "resources/compute shaders/creature_collisions.computeShader" };
	creatureCollisionsProgram = CreateLinkedShaderProgram(1, creatureCollisionsShaderTypes, creatureCollisionsShaderPaths, NULL);



	// Initialize drawing programs
	GLenum shapeShaderTypes[] = {
		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER
	};
	const char* shapeShaderPaths[] = {
		"resources/graphical shaders/shape.vertexShader",
		"resources/graphical shaders/shape.fragmentShader"
	};
	GLuint circleShapeProgram = CreateLinkedShaderProgram(2, shapeShaderTypes, shapeShaderPaths, NULL);
	vector<vec2> circleBase = CreateCircleBase(10, 1);
	circleDrawCallData = InitializeInstancedDrawCallData(circleShapeProgram, circleBase, true);



	// Initialize uniform grid
	// @TODO: Check that this actually works and that the grid updates succesfully
	SimulationSettingsChangedSubscribe(BuildUniformGrid); // Set callback

	// Initialize values to force BuildUniformGrid to recreate the grid
	ugrid_LastMaxCreatureRadius = -1;
	ugrid_LastMaxCreatureSenseRadius = -1;
	ugrid_LastMinCreatureRadius = -1;
	ugrid_LastSimulationWidth = -1;
	ugrid_LastSimulationHeight = -1;
	ugrid_LastInteractDist = -1;

	BuildUniformGrid();

	// Add some creatures (TEMP)
	for (int i = 0; i < numOfCreaturesOnInit; i++)
	{
		CreatureData data;
		data.col = vec3(random(), random(), random());
		data.pos = vec2(0, 0);
		data.vel = vec2((random() - 0.5) * 2 * 0.1, (random() - 0.5) * 2 * 0.1);
		data.rad = CREATURE_MIN_RADIUS.value;
		data.life = random() * 0.8 + 0.1;
		data.tile = -1;
		AddCreature(data);
	}

}

//////////////////////////////////
// -- SIMULATION UPDATE STEP -- //
//////////////////////////////////

void Simulation_Logic()
{

	/* @TODO:
	The number of work groups that can be dispatched in a single dispatch call is defined
	by GL_MAX_COMPUTE_WORK_GROUP_COUNT. This must be queried with glGetIntegeri_v.
	*/
	int numOfWorkGroups = creature_count / workGroupSize +
		(creature_count % workGroupSize == 0 ? 0 : 1);

	GLuint program;






	// Uniform grid bind
	program = uniformGridBindProgram;
	glUseProgram(program);
		SetUniformVector2f(program, "uSimDimensions", vec2(ugrid_SimWidth, ugrid_SimHeight));
		SetUniformVector2ui(program, "uGridDimensions", uvec2(ugrid_GridXDim, ugrid_GridYDim));
		SetUniformUInteger(program, "uIndicesInTile", ugrid_IndicesInTile);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_poses);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ugrid_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_tiles);
	glDispatchCompute(numOfWorkGroups, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);



	// Creature collision
	program = creatureCollisionsProgram;
	glUseProgram(program);
		SetUniformVector2ui(program, "uGridDimensions", uvec2(ugrid_GridXDim, ugrid_GridYDim));
		SetUniformUInteger(program, "uIndicesInTile", ugrid_IndicesInTile);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_poses);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_velos);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_radii);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ugrid_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_tiles);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_gprps); // Writes physics fix vector for decoupling purposes
	glDispatchCompute(numOfWorkGroups, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);



	// Velocity / physics value application
	program = applyVelocitiesProgram;
	glUseProgram(program);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_poses);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_velos);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_gprps); // Applies physics fix vector, zerofies
	glDispatchCompute(numOfWorkGroups, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	// Sensors


	
	// Uniform grid unbind
	program = uniformGridUnBindProgram;
	glUseProgram(program);
		SetUniformUInteger(program, "uIndicesInTile", ugrid_IndicesInTile);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ugrid_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_tiles);
	glDispatchCompute(numOfWorkGroups, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	// Border physics (before uniform bind so we don't step out of bounds!)
	program = borderPhysicsProgram;
	glUseProgram(program);
		SetUniformVector2f(program, "uSimDimensions", vec2(SIMULATION_WIDTH.value, SIMULATION_HEIGHT.value));
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_poses);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_velos);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_radii);
	glDispatchCompute(numOfWorkGroups, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void Simulation_Render()
{
	cameraTransform = GetSimSpaceToCameraTransform();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_colrs);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_poses);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_radii);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_lives);

	InstancedDrawCall(circleDrawCallData, creature_count);

}

void Simulation_Update()
{
	Simulation_Logic();
	Simulation_Render();
}