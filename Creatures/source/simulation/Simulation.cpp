#include "Simulation.h"


///////////////////////////////
// -- STRUCTS & CONSTANTS -- //
///////////////////////////////

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
	vector<vec2> brainBiasesExponents;
	vector<GLuint> brainStructure;
	vec3 col;
	vec2 pos;
	vec2 vel;
	GLfloat rad;
	GLfloat life;
	GLfloat angle; // Maybe temp
	GLfloat angleVel; // Temp
	GLfloat forwardThrust; // Temp
	GLfloat turnThrust; // Temp
	GLfloat hardness; // Temp
	vec2 skin; // Temp
	GLfloat spikeLocalAngle; // Temp
	GLfloat feederLocalAngle; // Temp
	GLfloat shieldLocalAngle; // Temp
	GLfloat shieldSpan;
};


//////////////////
// -- BRAINS -- //
//////////////////


// Calculate buffer size constants (used to define the size of each structure/node/link creature attributes)
const GLuint brains_MaxNumOfNodes = CREATURE_BRAIN_NUM_OF_INPUTS + CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS * CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL + CREATURE_BRAIN_NUM_OF_OUTPUTS;

const GLuint brains_MaxNumOfLinks = CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL > 0 ?

// We have midlevels in our max structure
(CREATURE_BRAIN_NUM_OF_INPUTS * CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL +
	CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL * CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL * (CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS - 1) +
	CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL * CREATURE_BRAIN_NUM_OF_OUTPUTS)
	:
	// We don't have midlevels in our max structure, number of links is just number of inputs * number of outputs
	(CREATURE_BRAIN_NUM_OF_INPUTS * CREATURE_BRAIN_NUM_OF_OUTPUTS);

const GLuint brains_MaxNumOfActivatedNodes = brains_MaxNumOfNodes - CREATURE_BRAIN_NUM_OF_INPUTS;

const GLuint brains_MaxNumOfStructureIndices = 1 + 1 + CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS + 1; // [NumOfLevels, NumOfInputs, NumOfMidLevels, NumOfOutputs]

void InitFirstGenBrain(vector<GLfloat>* brainNodes, vector<vec2>* brainBiasesExponents, vector<GLfloat>* brainLinks, vector<GLuint>* brainStructure)
{

	// WE CURRENTLY FILL THE ENTIRE BRAIN WITH DATA! NO PARTIAL BRAINS JUST YET!

	// Fill nodes with zeros
	brainNodes->reserve(brains_MaxNumOfNodes);
	for (int i = 0; i < brains_MaxNumOfNodes; i++)
	{
		brainNodes->emplace_back(0.0);
	}

	// Fill biases with some values between [-1, 1] for now
	// @TODO: Actually find what's a reasonable starting biases size 
	brainBiasesExponents->reserve(brains_MaxNumOfActivatedNodes);
	for (int i = 0; i < brains_MaxNumOfActivatedNodes; i++)
	{
		float bias = random();
		float activationExponent = random() * 10;
		brainBiasesExponents->emplace_back(vec2(bias, activationExponent));
	}

	// Fill links with random values in [0, 1)
	brainLinks->reserve(brains_MaxNumOfLinks);
	for (int i = 0; i < brains_MaxNumOfLinks; i++)
	{
		brainLinks->emplace_back(random());
	}

	// @TODO: First gen is currently getting max structure for performance testing
	brainStructure->reserve(brains_MaxNumOfStructureIndices);
	brainStructure->emplace_back(2 + CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS); // Number of levels (currently set to max)
	brainStructure->emplace_back(CREATURE_BRAIN_NUM_OF_INPUTS);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS; i++)
	{
		brainStructure->emplace_back(CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL);
	}
	brainStructure->emplace_back(CREATURE_BRAIN_NUM_OF_OUTPUTS);
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

// Brains
CreatureAttributesSSBOInfo creature_BrainsLinks{ 0, sizeof(GLfloat) * brains_MaxNumOfLinks };
CreatureAttributesSSBOInfo creature_BrainsNodes{ 0, sizeof(GLfloat) * brains_MaxNumOfNodes };
CreatureAttributesSSBOInfo creature_BrainsBiasesExponents{ 0, sizeof(vec2) * brains_MaxNumOfActivatedNodes };
CreatureAttributesSSBOInfo creature_BrainsStructures{ 0, sizeof(GLuint) * brains_MaxNumOfStructureIndices };

// Physics
CreatureAttributesSSBOInfo creature_Positions{ 0, sizeof(vec2) };
CreatureAttributesSSBOInfo creature_Velocities{ 0, sizeof(vec2) };
CreatureAttributesSSBOInfo creature_Angles{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_AngleVelocities{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_ForwardDirections{ 0, sizeof(vec2) };

// Movement
CreatureAttributesSSBOInfo creature_ForwardThrusts{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_TurnThrusts{ 0, sizeof(GLfloat) };

// Body parameters
CreatureAttributesSSBOInfo creature_Radii{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Lives{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Energies{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Meats{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Harndesses{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_Horninesses{ 0, sizeof(GLfloat) };

// Creature misc
CreatureAttributesSSBOInfo creature_Generations{ 0, sizeof(GLuint) };
CreatureAttributesSSBOInfo creature_UniformGridTiles{ 0, sizeof(GLint) };
CreatureAttributesSSBOInfo creature_GeneralPurpose{ 0, sizeof(vec2) };

// Appearances
CreatureAttributesSSBOInfo creature_Colors{ 0, sizeof(vec3) };
CreatureAttributesSSBOInfo creature_SkinPatterns{ 0, sizeof(vec2) };

// Creature-localized devices (feeders, shields, sensors)
CreatureAttributesSSBOInfo creature_SpikeStates{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_SpikeLocalAngles{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_SpikeDirections{ 0, sizeof(vec2) };

CreatureAttributesSSBOInfo creature_FeederStates{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_FeederLocalAngles{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_FeederDirections{ 0, sizeof(vec2) };

CreatureAttributesSSBOInfo creature_ShieldStates{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_ShieldLocalAngles{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_ShieldSpans{ 0, sizeof(GLfloat) };
CreatureAttributesSSBOInfo creature_ShieldDirections{ 0, sizeof(vec2) };

// @TODO: actually implement this placeholder
//CreatureAttributesSSBOInfo creature_SensorsStates{ 0, sizeof(GLfloat) };
//CreatureAttributesSSBOInfo creature_SensorsLocalAngles{ 0, sizeof(GLfloat) };
//reatureAttributesSSBOInfo creature_SensorsDirections{ 0, sizeof(vec2) };

// Deformations
const GLuint deformers_MaxNumOfDeformers = floor(M_PI / asin(CREATURE_MIN_RADIUS.min / (CREATURE_MAX_RADIUS.max + CREATURE_MIN_RADIUS.min)));
CreatureAttributesSSBOInfo creature_DeformerPositions{ 0, sizeof(vec2) * deformers_MaxNumOfDeformers };
CreatureAttributesSSBOInfo creature_DeformerRadii{ 0, sizeof(GLfloat) * deformers_MaxNumOfDeformers };
CreatureAttributesSSBOInfo creature_DeformerCounts{ 0, sizeof(GLuint) };



// The sole purpose of this vector is to contain our creature attributes SSBO infos for easier iteration during SSBO manipulations
vector<CreatureAttributesSSBOInfo*> creatureAttributesSSBOInfosRefs;
void LoadCreatureAttributeSSBOInfosIntoIterableVector()
{
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsLinks);
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsNodes);
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsBiasesExponents);
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsStructures);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Positions);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Velocities);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Angles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_AngleVelocities);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ForwardDirections);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ForwardThrusts);
	creatureAttributesSSBOInfosRefs.push_back(&creature_TurnThrusts);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Radii);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Lives);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Energies);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Meats);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Harndesses);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Horninesses);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Generations);
	creatureAttributesSSBOInfosRefs.push_back(&creature_UniformGridTiles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_GeneralPurpose);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Colors);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SkinPatterns);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SpikeStates);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SpikeLocalAngles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SpikeDirections);
	creatureAttributesSSBOInfosRefs.push_back(&creature_FeederStates);
	creatureAttributesSSBOInfosRefs.push_back(&creature_FeederLocalAngles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_FeederDirections);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ShieldStates);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ShieldLocalAngles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ShieldSpans);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ShieldDirections);
	//creatureAttributesSSBOInfosRefs.push_back(&creature_SensorsStates);
	//creatureAttributesSSBOInfosRefs.push_back(&creature_SensorsLocalAngles);
	//creatureAttributesSSBOInfosRefs.push_back(&creature_SensorsDirections);
	creatureAttributesSSBOInfosRefs.push_back(&creature_DeformerPositions);
	creatureAttributesSSBOInfosRefs.push_back(&creature_DeformerRadii);
	creatureAttributesSSBOInfosRefs.push_back(&creature_DeformerCounts);
}

GLuint creature_count = 0; // The count of active creatures in the simulation
GLuint max_supported_creature_count_by_current_buffers; // The number of creatures supported by current SSBO buffers
const GLenum ssbo_usage = GL_STATIC_DRAW;


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

ProgramInfo program_UpdateCreaturePlacements{ 0, 0, TECH_UPDATE_CREATURE_PLACEMENTS_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BorderPhysics{ 0, 0, TECH_BORDER_PHYSICS_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_UniformGridBind{ 0, 0, TECH_UNIFORM_GRID_BIND_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_UniformGridUnBind{ 0, 0, TECH_UNIFORM_GRID_UNBIND_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureInteractions{ 0, 0, TECH_CREATURE_INTERACTIONS_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BrainPushInputs{ 0, 0, TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BrainForwardPropagate{ 0, 0, TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BrainPullOutputs{ 0, 0, TECH_BRAIN_PULL_OUTPUTS_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureBodyWork{ 0, 0, TECH_CREATURE_BODY_WORK_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_InitNewFrame{ 0, 0, TECH_INIT_NEW_FRAME_WORKGROUP_LOCAL_SIZE };

void RecalculateAllProgramInfosNumberOfWorkGroupsNeeded()
{
	SetProgramInfoNumOfWorkGroupsNeeded(program_UpdateCreaturePlacements);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BorderPhysics);
	SetProgramInfoNumOfWorkGroupsNeeded(program_UniformGridBind);
	SetProgramInfoNumOfWorkGroupsNeeded(program_UniformGridUnBind);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureInteractions);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BrainPushInputs);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BrainForwardPropagate);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BrainPullOutputs);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureBodyWork);
	SetProgramInfoNumOfWorkGroupsNeeded(program_InitNewFrame);
}




///////////////////////////////////////////////
// -- CREATURE MANIPULATION COMFORT TOOLS -- //
///////////////////////////////////////////////

//@TODO: Keep a stack of recycle-able creature indices that were removed and later reused for adding new creatures without expanding buffers!

void SetCreatureAttribute(CreatureAttributesSSBOInfo attributes, GLuint creatureIndex, const void* data)
{
	GLuint writeOffset = attributes.attributeBytesSize * creatureIndex;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, attributes.ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, writeOffset, attributes.attributeBytesSize, data);
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
	SetCreatureAttribute(creature_BrainsBiasesExponents, newCreatureIndex, newCreatureData.brainBiasesExponents.data());
	SetCreatureAttribute(creature_BrainsStructures, newCreatureIndex, newCreatureData.brainStructure.data());
	SetCreatureAttribute(creature_Colors, newCreatureIndex, &newCreatureData.col);
	SetCreatureAttribute(creature_Positions, newCreatureIndex, &newCreatureData.pos);
	SetCreatureAttribute(creature_Velocities, newCreatureIndex, &newCreatureData.vel);
	SetCreatureAttribute(creature_GeneralPurpose, newCreatureIndex, NULL);
	SetCreatureAttribute(creature_Radii, newCreatureIndex, &newCreatureData.rad);
	SetCreatureAttribute(creature_Lives, newCreatureIndex, &newCreatureData.life);
	SetCreatureAttribute(creature_Angles, newCreatureIndex, &newCreatureData.angle);
	SetCreatureAttribute(creature_AngleVelocities, newCreatureIndex, &newCreatureData.angleVel);
	SetCreatureAttribute(creature_ForwardThrusts, newCreatureIndex, &newCreatureData.forwardThrust);
	SetCreatureAttribute(creature_TurnThrusts, newCreatureIndex, &newCreatureData.turnThrust);
	SetCreatureAttribute(creature_Harndesses, newCreatureIndex, &newCreatureData.hardness);
	SetCreatureAttribute(creature_UniformGridTiles, newCreatureIndex, NULL);
	SetCreatureAttribute(creature_SkinPatterns, newCreatureIndex, &newCreatureData.skin);
	SetCreatureAttribute(creature_SpikeLocalAngles, newCreatureIndex, &newCreatureData.spikeLocalAngle);
	SetCreatureAttribute(creature_FeederLocalAngles, newCreatureIndex, &newCreatureData.feederLocalAngle);
	SetCreatureAttribute(creature_ShieldLocalAngles, newCreatureIndex, &newCreatureData.shieldLocalAngle);
	SetCreatureAttribute(creature_ShieldSpans, newCreatureIndex, &newCreatureData.shieldSpan);


	creature_count++;

	// Recalculate number of workgroups needed for our programs
	RecalculateAllProgramInfosNumberOfWorkGroupsNeeded();

	return newCreatureIndex;
}

void RemoveCreatureAttribute(CreatureAttributesSSBOInfo attributes, GLuint creatureIndex)
{
	GLuint lastCreatureIndex = creature_count - 1;

	if (lastCreatureIndex != creatureIndex)
	{
		// Copy the data in lastCreatureIndex to our creatureIndex, the rest is taken care of through uniform memory limits
		glBindBuffer(GL_COPY_READ_BUFFER, attributes.ssbo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, attributes.ssbo);

		GLuint readOffset = lastCreatureIndex * attributes.attributeBytesSize;
		GLuint writeOffset = creatureIndex * attributes.attributeBytesSize;
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, attributes.attributeBytesSize);
	}
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

	float physicalInteractionDistOverlap = newMaxCreatureRadius * 2.0; // Multiply by two, because creatures > newMaxCreatureRadius distant from each other can still interact due to overlaps!
	float senseInteractionDist = newMaxCreatureSenseRadius; // Creatures sensors do not overlap in any way!
	float newInteractDist = std::max(physicalInteractionDistOverlap, senseInteractionDist);

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






/////////////////////////////////////
// -- SIMULATION INITIALIZATION -- //
/////////////////////////////////////

void InitOpenGLSettings()
{
	// Set some blending/depth settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Clean looking
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA); // Eye candy clipping but dark cores
	//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA); // Saturated, eye-candy look, opaqueness increases with color lightness
	//glBlendFunc(GL_ZERO, GL_SRC_COLOR); // Multiplicative
}

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

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_InitNewFrame.workGroupLocalSize)));
	GLenum initNewFrameShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* initNewFrameShaderPaths[] = { "resources/compute shaders/init_new_frame.computeShader" };
	program_InitNewFrame.program = CreateLinkedShaderProgram(1, initNewFrameShaderTypes, initNewFrameShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureBodyWork.workGroupLocalSize)));
	GLenum creatureBodyWorkShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureBodyWorkShaderPaths[] = { "resources/compute shaders/creature_body_work.computeShader" };
	program_CreatureBodyWork.program = CreateLinkedShaderProgram(1, creatureBodyWorkShaderTypes, creatureBodyWorkShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_UpdateCreaturePlacements.workGroupLocalSize)));
	GLenum updateCreaturePlacementsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* updateCreaturePlacementsShaderPaths[] = { "resources/compute shaders/update_creature_placements.computeShader" };
	program_UpdateCreaturePlacements.program = CreateLinkedShaderProgram(1, updateCreaturePlacementsShaderTypes, updateCreaturePlacementsShaderPaths, &replacers);
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

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureInteractions.workGroupLocalSize)));
	GLenum creatureInteractionsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureInteractionsShaderPaths[] = { "resources/compute shaders/creature_interactions.computeShader" };
	program_CreatureInteractions.program = CreateLinkedShaderProgram(1, creatureInteractionsShaderTypes, creatureInteractionsShaderPaths, &replacers);
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

// Render draw call datas
InstancedDrawCallData drawCallData_CreatureBody;
void InitDrawingPrograms()
{

	// Creature body
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

		drawCallData_CreatureBody.program = creatureBodyProgram;
		drawCallData_CreatureBody.numOfIndices = creatureBodyElementIndices.size() * 3;

		glGenVertexArrays(1, &drawCallData_CreatureBody.VAO);
		glBindVertexArray(drawCallData_CreatureBody.VAO);

		// Create vertices buffer
		glGenBuffers(1, &drawCallData_CreatureBody.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, drawCallData_CreatureBody.VBO);
		glBufferData(GL_ARRAY_BUFFER, creatureBodyVerts.size() * sizeof(vec2), creatureBodyVerts.data(), GL_STATIC_DRAW);

		// Create elements buffer
		glGenBuffers(1, &drawCallData_CreatureBody.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawCallData_CreatureBody.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, creatureBodyElementIndices.size() * sizeof(uvec3), creatureBodyElementIndices.data(), GL_STATIC_DRAW);

		// Vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);

		// Unbind (unbinding the vertex array essentially links the currently bound VBO and EBO)
		glBindVertexArray(0);
	}

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

	InitOpenGLSettings();
	InitSSBOs();
	InitLogicPrograms();
	InitDrawingPrograms();
	InitUniformGrid();

	// Add some creatures (TEMP)
	for (int i = 0; i < SIMULATION_NUM_OF_CREATURES_ON_INIT.value; i++)
	{
		CreatureCreationData data;

		InitFirstGenBrain(&data.brainNodes, &data.brainBiasesExponents, &data.brainLinks, &data.brainStructure);

		data.col = vec3(random(), random(), random());
		data.pos = vec2(0, 0);
		data.vel = vec2((random() - 0.5) * 2 * 0.001, (random() - 0.5) * 2 * 0.001);
		data.rad = CREATURE_MAX_RADIUS.value;
		data.life = random();
		data.angle = random() * 2 * M_PI;
		data.angleVel = (random() - 0.5) * 0.03;
		data.forwardThrust = 0;// random()* random() * 0.01;
		data.turnThrust = 0.0;
		data.hardness = 0.001;
		data.skin = vec2(random(), random());
		data.spikeLocalAngle = random() * 2 * M_PI;
		data.feederLocalAngle = random() * 2 * M_PI;
		data.shieldLocalAngle = random() * 2 * M_PI;
		data.shieldSpan = random() * M_PI * 0.35;
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


	// Init new frame
	programID = program_InitNewFrame.program;
	workGroupsNeeded = program_InitNewFrame.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_DeformerCounts.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	// Push brain inputs
	programID = program_BrainPushInputs.program;
	workGroupsNeeded = program_BrainPushInputs.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", brains_MaxNumOfStructureIndices);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", brains_MaxNumOfNodes);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Lives.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Velocities.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Brain forward propagate
	programID = program_BrainForwardPropagate.program;
	workGroupsNeeded = program_BrainForwardPropagate.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", brains_MaxNumOfStructureIndices);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", brains_MaxNumOfNodes);
	SetUniformUInteger(programID, "uMaxNumOfActivatedNodesInBrain", brains_MaxNumOfActivatedNodes);
	SetUniformUInteger(programID, "uMaxNumOfLinksInBrain", brains_MaxNumOfLinks);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_BrainsBiasesExponents.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_BrainsLinks.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Pull brain outputs
	programID = program_BrainPullOutputs.program;
	workGroupsNeeded = program_BrainPullOutputs.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", brains_MaxNumOfStructureIndices);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", brains_MaxNumOfNodes);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Colors.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Lives.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Creature body works
	programID = program_CreatureBodyWork.program;
	workGroupsNeeded = program_CreatureBodyWork.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Velocities.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_AngleVelocities.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_ForwardDirections.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_ForwardThrusts.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_TurnThrusts.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Update body placements
	programID = program_UpdateCreaturePlacements.program;
	workGroupsNeeded = program_UpdateCreaturePlacements.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformFloat(programID, "uVelocityDownscale", SIMULATION_VELOCITY_DOWNSCALE.value);
	SetUniformFloat(programID, "uAngleVelocityDownscale", SIMULATION_ANGLE_VELOCITY_DOWNSCALE.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_GeneralPurpose.ssbo); // Applies physics fix vector, zerofies
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Angles.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_AngleVelocities.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_ForwardDirections.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_SpikeLocalAngles.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_SpikeDirections.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_FeederLocalAngles.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_FeederDirections.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_ShieldLocalAngles.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, creature_ShieldDirections.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Border physics
	programID = program_BorderPhysics.program;
	workGroupsNeeded = program_BorderPhysics.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformVector2f(programID, "uSimDimensions", vec2(SIMULATION_WIDTH.value, SIMULATION_HEIGHT.value));
	SetUniformFloat(programID, "uBorderRestitution", SIMULATION_BORDER_RESTITUTION.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Radii.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Uniform grid bind
	programID = program_UniformGridBind.program;
	workGroupsNeeded = program_UniformGridBind.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformVector2f(programID, "uSimDimensions", vec2(ugrid_SimWidth, ugrid_SimHeight));
	SetUniformVector2ui(programID, "uGridDimensions", uvec2(ugrid_GridXDim, ugrid_GridYDim));
	SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ugrid_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_UniformGridTiles.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	// Creature interactions (including collisions, sensing and whatever else creature to creature interaction envelopes)
	programID = program_CreatureInteractions.program;
	workGroupsNeeded = program_CreatureInteractions.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformVector2ui(programID, "uGridDimensions", uvec2(ugrid_GridXDim, ugrid_GridYDim));
	SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
	SetUniformVector2f(programID, "uRandom", vec2(random() - 0.5, random() - 0.5)); // Used to resolve creatures absolutely clipped in each other
	SetUniformUInteger(programID, "uMaxNumOfDeformers", deformers_MaxNumOfDeformers);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Radii.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Harndesses.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ugrid_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_UniformGridTiles.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_GeneralPurpose.ssbo); // Writes physics fix vector for decoupling purposes
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_DeformerPositions.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_DeformerRadii.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_DeformerCounts.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Uniform grid unbind
	programID = program_UniformGridUnBind.program;
	workGroupsNeeded = program_UniformGridUnBind.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ugrid_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_UniformGridTiles.ssbo);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


}

void Simulation_Render()
{

	uint numOfInstances = creature_count;

	// Creature bodies

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Colors.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Positions.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_ForwardDirections.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Angles.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Radii.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_Lives.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_SkinPatterns.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_DeformerPositions.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_DeformerRadii.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_DeformerCounts.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_SpikeDirections.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, creature_FeederDirections.ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, creature_ShieldDirections.ssbo);


	glBindVertexArray(drawCallData_CreatureBody.VAO);
	glUseProgram(drawCallData_CreatureBody.program);
	SetUniformMatrix4(drawCallData_CreatureBody.program, "uTransform", GetSimSpaceToCameraTransform());
	SetUniformUInteger(drawCallData_CreatureBody.program, "uMaxNumOfDeformers", deformers_MaxNumOfDeformers);
	glDrawElementsInstanced(GL_TRIANGLES, drawCallData_CreatureBody.numOfIndices, GL_UNSIGNED_INT, 0, numOfInstances);
	glBindVertexArray(0);
}

void Simulation_Update()
{
	Simulation_Logic();
	Simulation_Render();
}




////////////////////////////////////////////////////////
// -- CREATURE DATA SNAPSHOTTING & UI INTERACTIONS -- //
////////////////////////////////////////////////////////

vector<vec2> GetCreaturePositions()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Positions.ssbo);
	void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	
	vector<vec2> positions(creature_count);
	unsigned int dataSize = size_t(creature_Positions.attributeBytesSize) * creature_count;
	memcpy(&positions[0], ptr, dataSize);

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	return positions;
}