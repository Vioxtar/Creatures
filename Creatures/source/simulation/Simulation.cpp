#include "Simulation.h"


/*

	Some notes

	Creatures grow in size the bigger they are, however this only reflects some ~0.8 percentage of their final radii, the remaining ~0.2 percent - they control!
	
	Creatures control hardness target, and have a stick mechanism (either arms or body or some sort of friction side)

	Sigmoid activation can be described as:

	sigmoidActivation(x) =
		if x == 0.5 then return x
		if x < 0.5 then return ((2x)^a)/2
		if x > 0.5 then return -(((2(1-x))^a) / 2) + 1

	for some constant a = 8-ish

	It could also probably be approximated further by dividing into regions and returning linear functions within those regions
*/


/*

	Approach to brains:
	We wish to minimize memory complexity of brains with minimal time complexity tradeoffs.
	
	Assume that:
	1. We have about 2 GB of usage space for brains
	2. We want to support up to 100,000 creatures at once

	Some numbers:
		1 GB =~ 250,000,000 floats
		250,000,000 / 100,000 = 2500 floats per creature
		However we about just about 2 GB available, so that's 500,000,000 floats / 100,000 creatures = 5000 floats per creature.
		Going by that number, that gives us just about 4500 floats per brain, leaving us with 500*4 bytes for other attributes.
		

	Some more numbers:
		If we would have 40 nodes in a level, and up to 5 levels, then every link level would have 40^2 = 1600 links, meaning 1600*5 = 8000 total links in the brain. If every link has a scalar float and a bias float, then that's 16000 floats per brain!
		A more feasible scenario is have 60 input nodes, 10 nodes in every hidden level, and 5 levels, that means: 60*10 + 10*10*4 = 1000 links in the entire brain! 2 floats in a link implies 2000 floats, and were we to consider node values, then that's 110 more floats = 2110 floats per brain.
	
		If a sensor each has:
			1. Activation
			2. Hue
			3. Lightness
			4. Saturation
		
		And we have 8 sensors, then that's 4*8 = 32 inputs. 

	To minimize memory complexity we can take several approaches:

		a. Instead of merely defining # of nodes per level (which is always a very big number since inputs is about 40-50 nodes), define # of nodes per hidden level and # of nodes per input level, this will drastically decrease size!
		b. Instead of having every node connect to every other node in the previous layer, have it only connect to X nodes
		c. Don't use biases! Just scalars...

	The overall approach for brains:

		A single brain buffer would contain:

			[STRUCTURE HEADER UINTS | A SEQUENCE OF ALL NODES' CURRENT VALUES | A SEQUENCE OF LINKS]

		The structure header is simply a sequence of uints that tells us the structure of the brain.
			[NUMOFLEVELS = 5, 32, 15, 10, 18, 12] for example means 32 inputs, 3 middle levels (15, 10 and 18 nodes), and 12 outputs.

		The sequence of notes is merely the current values stored at each node. We know that this buffer always starts immediately after
		the structure header, and is exactly sum(structure values) indices long.

		Afterwards, a sequence of links: the length of this buffer is exactly 32*15 + 15*10 + 10*18 + 18*12 indices long.

	We can use this layout to cleverly iterate our brains and perform forward propagations.
	While this layout is borderline dynamic, we must make each total brain size fixed for proper SSBO indexing.
	We can choose one of two data layout approaches:
		
		[FIXED STRUCTURE | DYNAMIC NODES | DYNAMIC LINKS, empty space]
		(gives us more flexibility in how we forward propagate, less convenient for mutation logic)
	
	Or this:

		[FIXED STRUCTURE | FIXED NODES, empty space | FIXED LINKS, empty space]
		(gives us more comfort in mutation handling, less in forward propagations)

	We'll choose the first, since it may prove more flexible to changes in the future (no hard-coding of sub-buffer sizes)

	Finally we wish for a straight forward method to provide an upper bound on memory complexity of brains.
	
*/


///////////////////
// -- STRUCTS -- //
///////////////////

// Drawing related structs
struct InstancedDrawCallData
{
	GLuint program;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	GLuint numOfIndices;
};

// The sole purpose of the CreatureCreationData is to neatly pack creature attribute values upon new creature creation
struct CreatureCreationData
{
	vector<GLfloat> brainLinks;
	vector<GLfloat> brainNodes;
	vector<GLuint> brainStructure;
	vec3 col;
	vec2 pos;
	vec2 vel;
	GLfloat rad;
	GLfloat life;
	GLfloat angle;
	GLfloat angleVel;
};


//////////////////
// -- BRAINS -- //
//////////////////


// Calculate buffer size constants (used to define the size of each structure/node/link creature attributes)
const GLuint brains_MaxNumOfNodes = CREATURE_NUM_OF_INPUTS + CREATURE_MAX_NUM_OF_MIDLEVELS * CREATURE_MAX_NUM_OF_NODES_IN_MIDLEVEL + CREATURE_NUM_OF_OUTPUTS;

const GLuint brains_MaxNumOfLinks = CREATURE_MAX_NUM_OF_NODES_IN_MIDLEVEL > 0 ?
		
		// We have midlevels in our max structure
		(CREATURE_NUM_OF_INPUTS * CREATURE_MAX_NUM_OF_NODES_IN_MIDLEVEL +
		CREATURE_MAX_NUM_OF_NODES_IN_MIDLEVEL * CREATURE_MAX_NUM_OF_NODES_IN_MIDLEVEL * (CREATURE_MAX_NUM_OF_MIDLEVELS - 1) +
		CREATURE_MAX_NUM_OF_NODES_IN_MIDLEVEL * CREATURE_NUM_OF_OUTPUTS)
	:
		// We don't have midlevels in our max structure, number of links is just number of inputs * number of outputs
		(CREATURE_NUM_OF_INPUTS * CREATURE_NUM_OF_OUTPUTS);

const GLuint brains_MaxNumOfStructureIndices = 1 + 1 + CREATURE_MAX_NUM_OF_MIDLEVELS + 1; // [NumOfLevels, NumOfInputs, NumOfMidLevels, NumOfOutputs]

void InitFirstGenBrain(vector<GLfloat>* brainNodes, vector<GLfloat>* brainLinks, vector<GLuint>* brainStructure)
{
	// Fill nodes with zeros
	brainNodes->reserve(brains_MaxNumOfNodes);
	for (int i = 0; i < brains_MaxNumOfNodes; i++)
	{
		brainNodes->emplace_back(0.0);
	}

	// Fill links with random values in [0, 1)
	brainLinks->reserve(brains_MaxNumOfLinks);
	for (int i = 0; i < brains_MaxNumOfLinks; i++)
	{
		brainLinks->emplace_back(random());
	}

	// @TODO: First gen is currently getting max structure for performance testing
	brainStructure->reserve(brains_MaxNumOfStructureIndices);
	brainStructure->emplace_back(2 + CREATURE_MAX_NUM_OF_MIDLEVELS); // Number of levels (currently set to max)
	brainStructure->emplace_back(CREATURE_NUM_OF_INPUTS);
	for (int i = 0; i < CREATURE_MAX_NUM_OF_MIDLEVELS; i++)
	{
		brainStructure->emplace_back(CREATURE_MAX_NUM_OF_NODES_IN_MIDLEVEL);
	}
	brainStructure->emplace_back(CREATURE_NUM_OF_OUTPUTS);
}



/////////////////////////////////////////////////////
// -- CREATURE ATTRIBUTE SSBO WORKING VARIABLES -- //
/////////////////////////////////////////////////////

// Creature Data SSBOs

struct CreatureAttributesSSBOInfo
{
	GLuint ssbo;
	GLuint attributeBytesSize;
};

CreatureAttributesSSBOInfo creature_BrainsLinks{ 0, sizeof(GLfloat) * brains_MaxNumOfLinks };
CreatureAttributesSSBOInfo creature_BrainsNodes{ 0, sizeof(GLfloat) * brains_MaxNumOfNodes };
CreatureAttributesSSBOInfo creature_BrainsStructures{ 0, sizeof(GLuint) * brains_MaxNumOfStructureIndices };
CreatureAttributesSSBOInfo creature_Colors{ 0, sizeof(vec3) };
CreatureAttributesSSBOInfo creature_Positions{ 0, sizeof(vec2) };
CreatureAttributesSSBOInfo creature_Velocities{ 0, sizeof(vec2) };
CreatureAttributesSSBOInfo creature_Radii{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Lives{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Angles{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_AngleVelocities{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Energies{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Meats{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_ForwardThrusts{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_TurnThrusts{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Harndesses{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Horninesses{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Generations{ 0, sizeof(GLuint) };
CreatureAttributesSSBOInfo creature_UniformGridTiles{ 0, sizeof(GLuint) };
CreatureAttributesSSBOInfo creature_GeneralPurpose{ 0, sizeof(vec2) };

// The sole purpose of this vector is to contain our creature attributes SSBO infos for easier iteration during SSBO manipulations
vector<CreatureAttributesSSBOInfo*> creatureAttributesSSBOInfosRefs;
void LoadCreatureAttributeSSBOInfosIntoIterableVector()
{
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsLinks);
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsNodes);
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsStructures);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Colors);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Positions);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Velocities);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Radii);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Lives);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Angles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_AngleVelocities);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Energies);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Meats);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ForwardThrusts);
	creatureAttributesSSBOInfosRefs.push_back(&creature_TurnThrusts);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Harndesses);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Horninesses);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Generations);
	creatureAttributesSSBOInfosRefs.push_back(&creature_UniformGridTiles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_GeneralPurpose);
}

GLuint creature_count = 0; // The count of active creatures in the simulation
GLuint max_supported_creature_count_by_current_buffers; // The number of creatures supported by current SSBO buffers

const GLenum ssbo_usage = GL_STREAM_READ;


////////////////////////////////////////////
// -- LOGIC PROGRAMS WORKING VARIABLES -- //
////////////////////////////////////////////


struct ProgramInfo
{
	GLuint program;
	GLuint workGroupsNeeded;
	const GLuint workGroupLocalSize;
};

void SetProgramInfoNumOfWorkGroupsNeeded(ProgramInfo& programInfo)
{
	int localSize = programInfo.workGroupLocalSize;
	programInfo.workGroupsNeeded = creature_count / localSize + (creature_count % localSize == 0 ? 0 : 1);
}

ProgramInfo program_ApplyVelocities{ 0, 0, TECH_APPLY_VELOCITIES_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BorderPhysics{ 0, 0, TECH_BORDER_PHYSICS_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_UniformGridBind{ 0, 0, TECH_UNIFORM_GRID_BIND_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_UniformGridUnBind{ 0, 0, TECH_UNIFORM_GRID_UNBIND_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureCollision{ 0, 0, TECH_CREATURE_COLLISION_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BrainPushInputs{ 0, 0, TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BrainForwardPropagate{ 0, 0, TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BrainPullOutputs{ 0, 0, TECH_BRAIN_PULL_OUTPUTS_WORKGROUP_LOCAL_SIZE };

void RecalculateAllProgramInfosNumberOfWorkGroupsNeeded()
{
	SetProgramInfoNumOfWorkGroupsNeeded(program_ApplyVelocities);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BorderPhysics);
	SetProgramInfoNumOfWorkGroupsNeeded(program_UniformGridBind);
	SetProgramInfoNumOfWorkGroupsNeeded(program_UniformGridUnBind);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureCollision);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BrainPushInputs);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BrainForwardPropagate);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BrainPullOutputs);
}


// Render draw call datas
InstancedDrawCallData drawCallData_Circle;



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

	// Calculate how many tiles we are spreading over our simulation space
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

	// Unbind (unbinding the vertex array essentially links the currently bound VBO and EBO)
	glBindVertexArray(0);

	return newDrawCallData;
}

void InstancedDrawCall(InstancedDrawCallData data, GLuint numOfInstances)
{
	glBindVertexArray(data.VAO);

	glUseProgram(data.program);
	SetUniformMatrix4(data.program, "uTransform", GetSimSpaceToCameraTransform());
	glDrawElementsInstanced(GL_TRIANGLES, data.numOfIndices, GL_UNSIGNED_INT, 0, numOfInstances);

	glBindVertexArray(0);
}


///////////////////////////////
// -- CREATURE SSBO UTILS -- //
///////////////////////////////


void InitEmptyCreatureAttributesSSBO(CreatureAttributesSSBOInfo& attributes, GLuint attributesCount)
{
	// Create a new SSBO
	GLuint newSSBO;
	glGenBuffers(1, &newSSBO);

	// Initialize with NULL data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
	GLuint size = attributesCount * attributes.attributeBytesSize;
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, ssbo_usage);

	attributes.ssbo = newSSBO;
}

void ExpandCreatureAttributesSSBO(CreatureAttributesSSBOInfo& attributes, GLuint attributeCountAdd)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, attributes.ssbo);

	// Get old size, and calculate new size
	GLint64 oldSize;
	glGetBufferParameteri64v(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &oldSize);
	GLuint newSize = (GLuint)oldSize + attributeCountAdd * attributes.attributeBytesSize;

	// Create a new empty SSBO
	GLuint newSSBO;
	glGenBuffers(1, &newSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newSize, NULL, ssbo_usage);

	// Copy old SSBO to new SSBO
	glBindBuffer(GL_COPY_READ_BUFFER, attributes.ssbo);
	glBindBuffer(GL_COPY_WRITE_BUFFER, newSSBO);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, oldSize); // This causes a performance warning for some reason on NVIDIA drivers

	// Delete old SSBO
	GLuint buffersToDelete = { attributes.ssbo };
	glDeleteBuffers(1, &buffersToDelete);

	// Finalize
	attributes.ssbo = newSSBO;
}


///////////////////////////////////////////////
// -- CREATURE MANIPULATION COMFORT TOOLS -- //
///////////////////////////////////////////////

void SetCreatureAttribute(CreatureAttributesSSBOInfo attributes, GLuint creatureIndex, const void* data)
{
	GLuint writeOffset = attributes.attributeBytesSize * creatureIndex;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, attributes.ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, writeOffset, attributes.attributeBytesSize, data);
}

void RemoveCreatureAttribute(CreatureAttributesSSBOInfo attributes, GLuint creatureIndex)
{
	GLuint lastCreatureIndex = creature_count - 1;
	if (lastCreatureIndex != creatureIndex)
	{
		// Copy the data in lastCreatureIndex to our creatureIndex
		glBindBuffer(GL_COPY_READ_BUFFER, attributes.ssbo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, attributes.ssbo);

		GLuint readOffset = lastCreatureIndex * attributes.attributeBytesSize;
		GLuint writeOffset = creatureIndex * attributes.attributeBytesSize;
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, attributes.attributeBytesSize);
	}

	// Nullify the new last creature index
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, attributes.ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, lastCreatureIndex, attributes.attributeBytesSize, NULL);
}

// @TODO: At some point in time we'll eventually need to account for the absolute max buffer size supported by our GPU
GLuint AddCreature(CreatureCreationData newCreatureData)
{
	// Check if we're exceeding capacity
	if (creature_count >= max_supported_creature_count_by_current_buffers)
	{
		// We're exceeding capacities for all of our SSBOs, expand them
		GLuint increaseSize = TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH;

		for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
		{
			ExpandCreatureAttributesSSBO(*creatureAttributeSSBOInfoRef, increaseSize);
		}

		max_supported_creature_count_by_current_buffers += increaseSize;
	}

	// Create the new creature by simply setting its attributes

	GLuint newCreatureIndex = creature_count;

	SetCreatureAttribute(creature_BrainsLinks, newCreatureIndex, newCreatureData.brainLinks.data());
	SetCreatureAttribute(creature_BrainsNodes, newCreatureIndex, newCreatureData.brainNodes.data());
	SetCreatureAttribute(creature_BrainsStructures, newCreatureIndex, newCreatureData.brainStructure.data());
	SetCreatureAttribute(creature_Colors, newCreatureIndex, &newCreatureData.col);
	SetCreatureAttribute(creature_Positions, newCreatureIndex, &newCreatureData.pos);
	SetCreatureAttribute(creature_Velocities, newCreatureIndex, &newCreatureData.vel);
	SetCreatureAttribute(creature_GeneralPurpose, newCreatureIndex, NULL);
	SetCreatureAttribute(creature_Radii, newCreatureIndex, &newCreatureData.rad);
	SetCreatureAttribute(creature_Lives, newCreatureIndex, &newCreatureData.life);
	SetCreatureAttribute(creature_Angles, newCreatureIndex, &newCreatureData.angle);
	SetCreatureAttribute(creature_AngleVelocities, newCreatureIndex, &newCreatureData.angleVel);
	SetCreatureAttribute(creature_UniformGridTiles, newCreatureIndex, NULL);

	creature_count++;

	// Recalculate number of workgroups needed for our programs
	RecalculateAllProgramInfosNumberOfWorkGroupsNeeded();

	return newCreatureIndex;
}

void RemoveCreature(GLuint creatureIndex)
{
	if (creature_count <= 0)
		return;

	for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
	{
		RemoveCreatureAttribute(*creatureAttributeSSBOInfoRef, creatureIndex);
	}

	creature_count--;

	// Recalculate number of workgroups needed for our programs
	RecalculateAllProgramInfosNumberOfWorkGroupsNeeded();
}



/////////////////////////////////////
// -- SIMULATION INITIALIZATION -- //
/////////////////////////////////////

void InitSSBOs()
{
	// Before anything, load all creature attribute SSBO infos into a vector for easy iterations
	LoadCreatureAttributeSSBOInfosIntoIterableVector();

	const unsigned int numOfCreaturesOnInit = SIMULATION_NUM_OF_CREATURES_ON_INIT.value;
	
	creature_count = 0;
	max_supported_creature_count_by_current_buffers = numOfCreaturesOnInit;

	for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
	{
		InitEmptyCreatureAttributesSSBO(*creatureAttributeSSBOInfoRef, numOfCreaturesOnInit);
	}
}

void InitLogicPrograms()
{
	vector<pair<string, string>> replacers;

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_ApplyVelocities.workGroupLocalSize)));
	GLenum applyVelocitiesShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* applyVelocitiesShaderPaths[] = { "resources/compute shaders/apply_velocities.computeShader" };
	program_ApplyVelocities.program = CreateLinkedShaderProgram(1, applyVelocitiesShaderTypes, applyVelocitiesShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_BorderPhysics.workGroupLocalSize)));
	GLenum borderPhysicsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* borderPhysicsShaderPaths[] = { "resources/compute shaders/border_physics.computeShader" };
	program_BorderPhysics.program = CreateLinkedShaderProgram(1, borderPhysicsShaderTypes, borderPhysicsShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_UniformGridBind.workGroupLocalSize)));
	GLenum uniformGridBindShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* uniformGridBindShaderPaths[] = { "resources/compute shaders/uniform_grid_bind.computeShader" };
	program_UniformGridBind.program = CreateLinkedShaderProgram(1, uniformGridBindShaderTypes, uniformGridBindShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_UniformGridUnBind.workGroupLocalSize)));
	GLenum uniformGridUnBindShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* uniformGridUnBindShaderPaths[] = { "resources/compute shaders/uniform_grid_unbind.computeShader" };
	program_UniformGridUnBind.program = CreateLinkedShaderProgram(1, uniformGridUnBindShaderTypes, uniformGridUnBindShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureCollision.workGroupLocalSize)));
	GLenum creatureCollisionsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureCollisionsShaderPaths[] = { "resources/compute shaders/creature_collisions.computeShader" };
	program_CreatureCollision.program = CreateLinkedShaderProgram(1, creatureCollisionsShaderTypes, creatureCollisionsShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_BrainPushInputs.workGroupLocalSize)));
	GLenum brainPushInputsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* brainPushInputsShaderPaths[] = { "resources/compute shaders/brain_push_inputs.computeShader" };
	program_BrainPushInputs.program = CreateLinkedShaderProgram(1, brainPushInputsShaderTypes, brainPushInputsShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_BrainForwardPropagate.workGroupLocalSize)));
	GLenum brainForwardPropagateShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* brainForwardPropagateShaderPaths[] = { "resources/compute shaders/brain_forward_propagate.computeShader" };
	program_BrainForwardPropagate.program = CreateLinkedShaderProgram(1, brainForwardPropagateShaderTypes, brainForwardPropagateShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_BrainPullOutputs.workGroupLocalSize)));
	GLenum brainPullOutputsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* brainPullOutputsShaderPaths[] = { "resources/compute shaders/brain_pull_outputs.computeShader" };
	program_BrainPullOutputs.program = CreateLinkedShaderProgram(1, brainPullOutputsShaderTypes, brainPullOutputsShaderPaths, &replacers);
	replacers.clear();

}

void InitDrawingPrograms()
{
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
	vector<vec2> circleBase = CreateCircleBase(7, 1);
	drawCallData_Circle = InitializeInstancedDrawCallData(circleShapeProgram, circleBase, true);
}

void InitUniformGrid()
{
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
}

void Simulation_Init()
{

	InitSSBOs();
	InitLogicPrograms();
	InitDrawingPrograms();
	InitUniformGrid();

	// Add some creatures (TEMP)
	for (int i = 0; i < SIMULATION_NUM_OF_CREATURES_ON_INIT.value; i++)
	{
		CreatureCreationData data;

		InitFirstGenBrain(&data.brainNodes, &data.brainLinks, &data.brainStructure);

		data.col = vec3(0.1, 0.1, 0.1);
		data.pos = vec2(0, 0);
		data.vel = vec2((random() - 0.5) * 2 * 0.001, (random() - 0.5) * 2 * 0.001);
		data.rad = CREATURE_MIN_RADIUS.value;
		data.life = random() * 0.8 + 0.1;
		data.angle = 0.0;
		data.angleVel = random() * 0.1;
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

	GLuint programID;
	GLuint workGroupsNeeded;

	// Push brain inputs
	programID = program_BrainPushInputs.program;
	workGroupsNeeded = program_BrainPushInputs.workGroupsNeeded;
	glUseProgram(programID);
		SetUniformUInteger(programID, "uMaxNumOfStructureIndices", brains_MaxNumOfStructureIndices);
		SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", brains_MaxNumOfNodes);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Lives.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Velocities.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	// Brain forward propagate
	programID = program_BrainForwardPropagate.program;
	workGroupsNeeded = program_BrainForwardPropagate.workGroupsNeeded;
	glUseProgram(programID);
		SetUniformUInteger(programID, "uMaxNumOfStructureIndices", brains_MaxNumOfStructureIndices);
		SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", brains_MaxNumOfNodes);
		SetUniformUInteger(programID, "uMaxNumOfLinksInBrain", brains_MaxNumOfLinks);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_BrainsLinks.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	// Pull brain outputs
	programID = program_BrainPullOutputs.program;
	workGroupsNeeded = program_BrainPullOutputs.workGroupsNeeded;
	glUseProgram(programID);
		SetUniformUInteger(programID, "uMaxNumOfStructureIndices", brains_MaxNumOfStructureIndices);
		SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", brains_MaxNumOfNodes);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Colors.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Lives.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);



	// Uniform grid bind
	programID = program_UniformGridBind.program;
	workGroupsNeeded = program_UniformGridBind.workGroupsNeeded;
	glUseProgram(programID);
		SetUniformVector2f(programID, "uSimDimensions", vec2(ugrid_SimWidth, ugrid_SimHeight));
		SetUniformVector2ui(programID, "uGridDimensions", uvec2(ugrid_GridXDim, ugrid_GridYDim));
		SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ugrid_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_UniformGridTiles.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);



	// Creature collision
	programID = program_CreatureCollision.program;
	workGroupsNeeded = program_CreatureCollision.workGroupsNeeded;
	glUseProgram(programID);
		SetUniformVector2ui(programID, "uGridDimensions", uvec2(ugrid_GridXDim, ugrid_GridYDim));
		SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Radii.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ugrid_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_UniformGridTiles.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_GeneralPurpose.ssbo); // Writes physics fix vector for decoupling purposes
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);



	// Velocity / physics value application
	programID = program_ApplyVelocities.program;
	workGroupsNeeded = program_ApplyVelocities.workGroupsNeeded;
	glUseProgram(programID);
		SetUniformFloat(programID, "uVelocityDownscale", SIMULATION_VELOCITY_DOWNSCALE.value);
		SetUniformFloat(programID, "uAngleVelocityDownscale", SIMULATION_ANGLE_VELOCITY_DOWNSCALE.value);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_GeneralPurpose.ssbo); // Applies physics fix vector, zerofies
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Angles.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_AngleVelocities.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);



	
	// Uniform grid unbind
	programID = program_UniformGridUnBind.program;
	workGroupsNeeded = program_UniformGridUnBind.workGroupsNeeded;
	glUseProgram(programID);
		SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ugrid_SSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_UniformGridTiles.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


	// Border physics
	programID = program_BorderPhysics.program;
	workGroupsNeeded = program_BorderPhysics.workGroupsNeeded;
	glUseProgram(programID);
		SetUniformVector2f(programID, "uSimDimensions", vec2(SIMULATION_WIDTH.value, SIMULATION_HEIGHT.value));
		SetUniformFloat(programID, "uBorderRestitution", SIMULATION_BORDER_RESTITUTION.value);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Radii.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void Simulation_Render()
{

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Colors.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Positions.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Radii.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Lives.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Angles.ssbo);

	InstancedDrawCall(drawCallData_Circle, creature_count);

}

void Simulation_Update()
{
	Simulation_Logic();
	Simulation_Render();
}