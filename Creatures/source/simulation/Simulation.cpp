#include "Simulation.h"


/////////////////////////////////////
// -- DRAWING WORKING VARIABLES -- //
/////////////////////////////////////

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
InstancedDrawCallData drawCallData_CreatureBody;



//////////////////
// -- BRAINS -- //
//////////////////

void InitFirstGenBrain(vector<GLfloat>* brainNodes, vector<vec2>* brainBiasesExponents, vector<GLfloat>* brainLinks, vector<GLuint>* brainStructure)
{

	// WE CURRENTLY FILL THE ENTIRE BRAIN WITH DATA! NO PARTIAL BRAINS JUST YET!

	// Fill nodes with zeros
	brainNodes->reserve(CREATURE_BRAIN_MAX_NUM_OF_NODES);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_NODES; i++)
	{
		brainNodes->emplace_back(0.0);
	}

	// Fill biases with some values between [-1, 1] for now
	// @TODO: Actually find what's a reasonable starting biases size 
	brainBiasesExponents->reserve(CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES; i++)
	{
		float bias = random();
		float activationExponent = random() * 10;
		brainBiasesExponents->emplace_back(vec2(bias, activationExponent));
	}

	// Fill links with random values in [0, 1)
	brainLinks->reserve(CREATURE_BRAIN_MAX_NUM_OF_LINKS);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_LINKS; i++)
	{
		brainLinks->emplace_back(random());
	}

	// @TODO: First gen is currently getting max structure for performance testing
	brainStructure->reserve(CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	brainStructure->emplace_back(2 + CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS); // Number of levels (currently set to max)
	brainStructure->emplace_back(CREATURE_BRAIN_NUM_OF_INPUTS);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS; i++)
	{
		brainStructure->emplace_back(CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL);
	}
	brainStructure->emplace_back(CREATURE_BRAIN_NUM_OF_OUTPUTS);
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


unsigned int programs_CreatureCountSupportedByWorkGroups = 0;

void SetProgramInfoNumOfWorkGroupsNeeded(ProgramInfo& programInfo)
{
	int localSize = programInfo.workGroupLocalSize;
	programInfo.workGroupsNeeded = creature_count / localSize + (creature_count % localSize == 0 ? 0 : 1);
}

void RecalculateAllProgramInfosNumberOfWorkGroupsNeeded()
{
	// Check if we're not already supporting the current creature count
	if (programs_CreatureCountSupportedByWorkGroups == creature_count)
	{
		return;
	}

	// Creature count changed, actually recalculate work groups needed
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

	// Update our newly supported creature count
	programs_CreatureCountSupportedByWorkGroups = creature_count;
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
	glBufferData(GL_SHADER_STORAGE_BUFFER, newSSBOSize, NULL, TECH_SSBO_USAGE);


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
	CreatureData_Init();

	InitOpenGLSettings();
	InitLogicPrograms();
	InitDrawingPrograms();
	InitUniformGrid();

	// Add some creatures (TEMP)
	for (int i = 0; i < SIMULATION_NUM_OF_CREATURES_ON_INIT.value; i++)
	{
		CreatureData data;

		InitFirstGenBrain(&data.brainNodes, &data.brainBiasesExponents, &data.brainLinks, &data.brainStructure);

		data.col = vec3(random(), random(), random());
		data.pos = vec2(0, 0);
		data.vel = vec2((random() - 0.5) * 2 * 0.001, (random() - 0.5) * 2 * 0.001);
		data.rad = CREATURE_MAX_RADIUS.value;
		data.life = random();
		data.angle = random() * 2 * M_PI;
		data.angleVel = (random() - 0.5) * 0.03;
		data.forwardThrust = random()* random() * 0.01;
		data.turnThrust = 0.0;
		data.hardness = random();
		data.skin = vec2(random(), random());
		data.spikeLocalAngle = random() * 2 * M_PI;
		data.feederLocalAngle = random() * 2 * M_PI;
		data.shieldLocalAngle = random() * 2 * M_PI;
		data.shieldSpan = random() * M_PI * 0.35;
		CreatureData_AddCreature(data);
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

	RecalculateAllProgramInfosNumberOfWorkGroupsNeeded();


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
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", CREATURE_BRAIN_MAX_NUM_OF_NODES);
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
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", CREATURE_BRAIN_MAX_NUM_OF_NODES);
	SetUniformUInteger(programID, "uMaxNumOfActivatedNodesInBrain", CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES);
	SetUniformUInteger(programID, "uMaxNumOfLinksInBrain", CREATURE_BRAIN_MAX_NUM_OF_LINKS);
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
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", CREATURE_BRAIN_MAX_NUM_OF_NODES);
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
	SetUniformUInteger(programID, "uMaxNumOfDeformers", CREATURE_MAX_NUM_OF_DEFORMERS);
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
	SetUniformUInteger(drawCallData_CreatureBody.program, "uMaxNumOfDeformers", CREATURE_MAX_NUM_OF_DEFORMERS);
	glDrawElementsInstanced(GL_TRIANGLES, drawCallData_CreatureBody.numOfIndices, GL_UNSIGNED_INT, 0, numOfInstances);
	glBindVertexArray(0);
}

void Simulation_Update()
{
	Simulation_Logic();
	Simulation_Render();
}
