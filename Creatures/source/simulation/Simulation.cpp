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

//////////////////////////
// -- MUTATION UTILS -- //
//////////////////////////

void MutateClampedCreatureTrait(GLfloat& value, GLfloat minValue, GLfloat maxValue, GLfloat chanceToMutate, GLfloat mutationJitter, GLfloat mutationJitterPercentageExponent)
{
	if (random() >= chanceToMutate) return;
	GLfloat mutation = pow(random(), mutationJitterPercentageExponent) * mutationJitter	* randomNegate();
	value = clamp(value + mutation, minValue, maxValue);
}

void MutateCreatureTrait(GLfloat& value, GLfloat chanceToMutate, GLfloat mutationJitter, GLfloat mutationJitterPercentageExponent)
{
	if (random() >= chanceToMutate) return;
	GLfloat mutation = pow(random(), mutationJitterPercentageExponent) * mutationJitter * randomNegate();
	value += mutation;
}


//////////////////
// -- BRAINS -- //
//////////////////

void InitFirstGenBrain(vector<GLfloat>* brainNodes, vector<vec2>* brainBiasesExponents, vector<GLfloat>* brainLinks, vector<GLuint>* brainStructure)
{
	// @TEMP v
	// WE CURRENTLY FILL THE ENTIRE BRAIN WITH DATA! NO PARTIAL BRAINS JUST YET! EVERYTHING IS TEMP
	// @TEMP ^

	// Fill nodes with zeros
	brainNodes->resize(CREATURE_BRAIN_MAX_NUM_OF_NODES);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_NODES; i++)
	{
		brainNodes->at(i) = 0.0;
	}

	// Fill biases and exponents
	brainBiasesExponents->resize(CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES; i++)
	{
		GLfloat bias = pow(random(), CREATURE_BRAIN_NEW_BIAS_PERCENTAGE_EXPONENT)
			* CREATURE_BRAIN_NEW_BIAS_MAX_ABS
			* randomNegate();

		GLfloat activationExponent = pow(random(), CREATURE_BRAIN_NEW_ACTIVATION_EXPONENT_PERCENTAGE_EXPONENT)
			* CREATURE_BRAIN_NEW_ACTIVATION_EXPONENT_MAX_ABS
			* randomNegate();

		brainBiasesExponents->at(i) = (vec2(bias, activationExponent));
	}

	// Fill links
	brainLinks->resize(CREATURE_BRAIN_MAX_NUM_OF_LINKS);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_LINKS; i++)
	{
		GLfloat linkWeight = pow(random(), CREATURE_BRAIN_NEW_LINK_WEIGHT_PERCENTAGE_EXPONENT)
			* CREATURE_BRAIN_NEW_LINK_WEIGHT_MAX_VAL;

		brainLinks->at(i) = (linkWeight);
	}

	// @TODO: First gen is currently getting max structure for performance testing
	brainStructure->reserve(CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	
	// First structure value is the number of overall levels
	brainStructure->emplace_back(2 + CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS);
	brainStructure->emplace_back(CREATURE_BRAIN_NUM_OF_INPUTS);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS; i++)
	{
		brainStructure->emplace_back(CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL);
	}
	brainStructure->emplace_back(CREATURE_BRAIN_NUM_OF_OUTPUTS);
}

void InitOffspringBrain(unsigned int p1SSBO, vector<GLfloat>* oNodes, vector<vec2>* oBiasesExponents, vector<GLfloat>* oLinks, vector<GLuint>* oStructure)
{
	// Acquire prominent parent's brain
	vector<GLuint> p1Structure(CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	vector<GLfloat> p1Links(CREATURE_BRAIN_MAX_NUM_OF_LINKS);
	vector<vec2> p1BiasesExponents(CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES);

	GetCreatureAttributeBySSBOIndex(creature_BrainsStructures, p1SSBO, p1Structure.data());
	GetCreatureAttributeBySSBOIndex(creature_BrainsLinks, p1SSBO, p1Links.data());
	GetCreatureAttributeBySSBOIndex(creature_BrainsBiasesExponents, p1SSBO, p1BiasesExponents.data());

	// Make room in offspring's brain
	oStructure->resize(CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	oLinks->resize(CREATURE_BRAIN_MAX_NUM_OF_LINKS);
	oBiasesExponents->resize(CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES);
	oNodes->resize(CREATURE_BRAIN_MAX_NUM_OF_NODES);

	// New very-possibly-naive mutation model (definitely for the lazy)
	// Just copy the links, biasExponents, and nodes as is with a chance of error, plus (possibly) a change in structure if we can, and be done with it
	
	// Fill nodes with zeros
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_NODES; i++)
	{
		oNodes->at(i) = 0.0;
	}

	// Fill biases and exponents
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES; i++)
	{
		vec2 biasExponent = p1BiasesExponents.at(i);
		GLfloat bias = biasExponent.x;
		GLfloat exponent = biasExponent.y;

		MutateClampedCreatureTrait(bias,
			-CREATURE_BRAIN_NEW_BIAS_MAX_ABS, CREATURE_BRAIN_NEW_BIAS_MAX_ABS,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_CHANCE,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_MAX_ABS,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_PERCENTAGE_EXPONENT
		);

		MutateClampedCreatureTrait(exponent,
			-CREATURE_BRAIN_NEW_ACTIVATION_EXPONENT_MAX_ABS, CREATURE_BRAIN_NEW_ACTIVATION_EXPONENT_MAX_ABS,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_CHANCE,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_MAX_ABS,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_PERCENTAGE_EXPONENT
		);

		oBiasesExponents->at(i) = vec2(bias, exponent);
	}

	// Fill links
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_LINKS; i++)
	{
		GLfloat linkWeight = p1Links.at(i);

		MutateClampedCreatureTrait(linkWeight,
			0.0, CREATURE_BRAIN_NEW_LINK_WEIGHT_MAX_VAL,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_CHANCE,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_MAX_ABS,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_PERCENTAGE_EXPONENT
		);

		oLinks->at(i) = linkWeight;
	}

	// Finally structure
	// @TODO: Actually mutate structure as well...
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES; i++)
	{
		oStructure->at(i) = p1Structure.at(i);
	}


	// Old mutation model (not for lazy people)
	/*
	We have 8 different mutation oriented brain copying types:

		0. No change

		Adjustment mutation (can be several)
			1. Change weights
			2. Change biases
			3. Change exponents

		Structure mutation (single)
			4. Add midlevel
			5. Remove midlevel
			6. Add midlevel node
			7. Remove midlevel node

	When adding nodes or midlevels, we can effectively output an equivalent brain.

	On the contrary removal of nodes or midlevels may severly harm brain functionality, and will likely
	devolve the offspring's fitness. However we still wish to support removals for symmetry's sakes, and
	to prevent the system from converging to full brain structures.
	*/
	/*
	enum class BrainMutationPassType
	{
		NONE,
		ADJUSTMENT,
		ADD_MIDLEVEL,
		REM_MIDLEVEL,
		ADD_MIDLEVEL_NODE,
		REM_MIDLEVEL_NODE
	};

	BrainMutationPassType mutationType = BrainMutationPassType::NONE;
	//float ran = random();
	//if (ran < 0.1) mutationType = BrainMutationPassType::ADJUSTMENT;
	//else if (ran < 0.15) mutationType = BrainMutationPassType::ADD_MIDLEVEL;
	//else if (ran < 0.2) mutationType = BrainMutationPassType::REM_MIDLEVEL;
	//else if (ran < 0.25) mutationType = BrainMutationPassType::ADD_MIDLEVEL_NODE;
	//else if (ran < 0.3) mutationType = BrainMutationPassType::REM_MIDLEVEL_NODE;

	GLuint linkCopyIndex = 0;
	GLuint linkPasteIndex = 0;
	GLuint biasExpCopyIndex = 0;
	GLuint biasExpPasteIndex = 0;
	GLuint levelCopyIndex = 0;
	GLuint levelPasteIndex = 0;

	GLuint p1NumOfLevels = p1Structure[0];

	if (mutationType == BrainMutationPassType::ADJUSTMENT)
	{
		for (GLuint p1Level = 0; p1Level < p1NumOfLevels; ++p1Level)
		{
			GLuint p1NumOfNodesInLevel = p1Structure.at(1 + levelCopyIndex);
			GLuint p1NumOfNodesInPrevLevel = levelCopyIndex <= 0 ? 0 : p1Structure.at(levelCopyIndex);
			for (GLuint p1Node = 0; p1Node < p1NumOfNodesInLevel; ++p1Node)
			{
				if (levelCopyIndex > 0)
				{
					vec2 biasExponent = p1BiasesExponents.at(biasExpCopyIndex);
					GLfloat bias = biasExponent.x;
					GLfloat exponent = biasExponent.y;

					// @TODO: Maybe put these mutation code snippets in a function

					if (random() < CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_CHANCE)
					{
						GLfloat mutation = pow(random(), CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_PERCENTAGE_EXPONENT)
							* CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_MAX_ABS
							* randomNegate();

						bias = clamp(bias + mutation, -CREATURE_BRAIN_NEW_BIAS_MAX_ABS, CREATURE_BRAIN_NEW_BIAS_MAX_ABS);
					}

					if (random() < CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_CHANCE)
					{
						GLfloat mutation = pow(random(), CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_PERCENTAGE_EXPONENT)
							* CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_MAX_ABS
							* randomNegate();

						exponent = clamp(exponent + mutation, -CREATURE_BRAIN_NEW_ACTIVATION_EXPONENT_MAX_ABS, CREATURE_BRAIN_NEW_ACTIVATION_EXPONENT_MAX_ABS);
					}

					oBiasesExponents->at(biasExpPasteIndex) = vec2(bias, exponent);

					biasExpCopyIndex++;
					biasExpPasteIndex++;

					for (GLuint p1PrevNode = 0; p1PrevNode < p1NumOfNodesInPrevLevel; ++p1PrevNode)
					{
						GLfloat linkWeight = p1Links[linkCopyIndex];

						if (random() < CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_CHANCE)
						{
							GLfloat mutation = pow(random(), CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_PERCENTAGE_EXPONENT)
								* CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_MAX_ABS;

							linkWeight = clamp(linkWeight + mutation, -CREATURE_BRAIN_NEW_LINK_WEIGHT_MAX_VAL, CREATURE_BRAIN_NEW_LINK_WEIGHT_MAX_VAL);
						}

						oLinks->at(linkPasteIndex) = linkWeight;

						linkCopyIndex++;
						linkPasteIndex++;
					}
				}
			}

			oStructure->at(1 + levelPasteIndex) = p1NumOfNodesInLevel;

			levelCopyIndex++;
			levelPasteIndex++;
		}
		oStructure->at(0) = p1NumOfLevels;
	}

	if (mutationType == BrainMutationPassType::ADD_MIDLEVEL)
	{
		// If there isn't enough room for an additional midlevel, bail
		GLuint p1NumOfMidlevels = p1NumOfLevels - 2;
		if (p1NumOfMidlevels >= CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS)
		{
			mutationType = BrainMutationPassType::NONE;
		}
		else
		{
			GLuint levelIndexToBeCopied = randomIntRange(1, p1NumOfMidlevels);

			for (GLuint p1Level = 0; p1Level < p1NumOfLevels; ++p1Level)
			{
				GLuint p1NumOfNodesInLevel = p1Structure.at(1 + levelCopyIndex);
				GLuint p1NumOfNodesInPrevLevel = levelCopyIndex <= 0 ? 0 : p1Structure.at(levelCopyIndex);

				for (GLuint p1Node = 0; p1Node < p1NumOfNodesInLevel; ++p1Node)
				{
					if (levelCopyIndex > 0)
					{
						oBiasesExponents->at(biasExpPasteIndex) = p1BiasesExponents.at(biasExpCopyIndex);

						biasExpCopyIndex++;
						biasExpPasteIndex++;

						for (GLuint p1PrevNode = 0; p1PrevNode < p1NumOfNodesInPrevLevel; ++p1PrevNode)
						{
							oLinks->at(linkPasteIndex) = p1Links.at(linkCopyIndex);

							linkCopyIndex++;
							linkPasteIndex++;
						}
					}
				}

				oStructure->at(1 + levelPasteIndex) = p1NumOfNodesInLevel;

				levelCopyIndex++;
				levelPasteIndex++;

				// We just finished writing the new level of our parent, should we copy it?
				bool copyingActive = levelPasteIndex - 1 == levelIndexToBeCopied;
				if (copyingActive)
				{

					GLuint numOfNodesInNewLevel = p1NumOfNodesInLevel;
					// If there are no mid-levels to copy from, then we 'copy' the input level, in which case the num of nodes in new level is min(#ofinputs, max nodes in midlevel)
					//... but if numOfNodesInNewLevel < #ofInputs (very likely...) then we can't just blindly copy all the links that follow!... there will be too many

					for (GLuint copyNode = 0; copyNode < numOfNodesInNewLevel; ++copyNode)
					{
						// Write a neutral bias and exponent to maintain brain equivalence
						GLfloat bias = 0.0;
						GLfloat exponent = 1.0;

						oBiasesExponents->at(biasExpPasteIndex) = vec2(bias, exponent);

						biasExpPasteIndex++;

						for (GLuint copyLink = 0; copyLink < numOfNodesInNewLevel; ++copyLink)
						{
							// Write neutral link weights to maintain brain equivalence (every duplicated node's value will be as its original during forward propagation)
							oLinks->at(linkPasteIndex) = copyNode == copyLink ? 1.0 : 0.0;

							linkPasteIndex++;
						}
					}

					oStructure->at(1 + levelPasteIndex) = numOfNodesInNewLevel;

					levelPasteIndex++;

					// We're done adding the neutral level, continue as usual...
				}
			}

			oStructure->at(0) = p1NumOfLevels + 1;
		}
	}

	if (mutationType == BrainMutationPassType::NONE)
	{
		for (GLuint p1Level = 0; p1Level < p1NumOfLevels; ++p1Level)
		{
			GLuint p1NumOfNodesInLevel = p1Structure.at(1 + levelCopyIndex);
			GLuint p1NumOfNodesInPrevLevel = levelCopyIndex <= 0 ? 0 : p1Structure.at(levelCopyIndex);
			for (GLuint p1Node = 0; p1Node < p1NumOfNodesInLevel; ++p1Node)
			{
				if (levelCopyIndex > 0)
				{
					oBiasesExponents->at(biasExpPasteIndex) = p1BiasesExponents.at(biasExpCopyIndex);

					biasExpCopyIndex++;
					biasExpPasteIndex++;

					for (GLuint p1PrevNode = 0; p1PrevNode < p1NumOfNodesInPrevLevel; ++p1PrevNode)
					{
						oLinks->at(linkPasteIndex) = p1Links.at(linkCopyIndex);

						linkCopyIndex++;
						linkPasteIndex++;
					}
				}
			}

			oStructure->at(1 + levelPasteIndex) = p1NumOfNodesInLevel;

			levelCopyIndex++;
			levelPasteIndex++;
		}
		oStructure->at(0) = p1NumOfLevels;
	}
	*/
}


/////////////////////////////
// -- CREATURE CREATION -- //
/////////////////////////////

void AddFirstGenerationCreature()
{
	CreatureData data;

	InitFirstGenBrain(&data.brainNodes, &data.brainBiasesExponents, &data.brainLinks, &data.brainStructure);
	
	data.skinPattern = vec2(random(), random());
	data.skinHue = random();
	data.skinSaturation = 1.0;
	data.skinValue = 1.0;

	data.pos = vec2(0, 0);
	data.vel = vec2((random() - 0.5) * 2 * 0.001, (random() - 0.5) * 2 * 0.001);
	data.angle = random() * 2 * M_PI;
	data.angleVel = 0;

	data.hardness = SIMULATION_FIRSTGEN_CREATURE_INITIAL_HARDNESS.value;
	data.rad = SIMULATION_FIRSTGEN_CREATURE_INITIAL_RADIUS.value;
	
	data.life = SIMULATION_FIRSTGEN_CREATURE_INITIAL_LIFE.value;
	data.energy = SIMULATION_FIRSTGEN_CREATURE_INITIAL_ENERGY.value;
	data.meat = SIMULATION_FIRSTGEN_CREATURE_INITIAL_MEAT.value;

	float spikeState = 0.0;
	float feederState = 0.0;
	float shieldState = 0.0;
	float shieldSpan = 0.5;
	data.spike = vec4(0, 0, spikeState, 0);
	data.feeder = vec4(0, 0, feederState, 0);
	data.shield = vec4(0, 0, shieldState, shieldSpan);

	data.spikeLocalAngle = random() * 2 * M_PI;
	data.feederLocalAngle = random() * 2 * M_PI;
	data.shieldLocalAngle = random() * 2 * M_PI;

	CreatureData_AddCreature(data);
}

void AddOffspringCreature(unsigned int p1SSBO, unsigned int p2SSBO)
{

	CreatureUniqueID parent1ID = CreatureData_CreatureSSBOIndexToUniqueID(p1SSBO);
	CreatureUniqueID parent2ID = CreatureData_CreatureSSBOIndexToUniqueID(p2SSBO);

	CreatureData data;

	InitOffspringBrain(p1SSBO, &data.brainNodes, &data.brainBiasesExponents, &data.brainLinks, &data.brainStructure);

	// Copy some parent values to the offspring values
	GetCreatureAttributeBySSBOIndex(creature_SkinPatterns, p1SSBO, &data.skinPattern);
	GetCreatureAttributeBySSBOIndex(creature_SkinHues, p1SSBO, &data.skinHue);
	GetCreatureAttributeBySSBOIndex(creature_SpikeLocalAngles, p1SSBO, &data.spikeLocalAngle);
	GetCreatureAttributeBySSBOIndex(creature_FeederLocalAngles, p1SSBO, &data.feederLocalAngle);
	GetCreatureAttributeBySSBOIndex(creature_ShieldLocalAngles, p1SSBO, &data.shieldLocalAngle);

	MutateClampedCreatureTrait(data.skinPattern.x,
		0.0, 1.0,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_CHANCE,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_MAX_ABS,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_PERCENTAGE_EXPONENT
	);

	MutateClampedCreatureTrait(data.skinPattern.y,
		0.0, 1.0,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_CHANCE,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_MAX_ABS,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_PERCENTAGE_EXPONENT
	);

	MutateCreatureTrait(data.skinHue,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_HUE_CHANCE,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_HUE_MAX_ABS,
		CREATURE_MUTATION_BODY_CHANGE_SKIN_HUE_PERCENTAGE_EXPONENT
	);
	data.skinHue = mod(data.skinHue, 1.0f);

	MutateCreatureTrait(data.spikeLocalAngle,
		CREATURE_MUTATION_DEVICE_CHANGE_SPIKE_ANGLE_CHANCE,
		CREATURE_MUTATION_DEVICE_CHANGE_SPIKE_ANGLE_MAX_ABS,
		CREATURE_MUTATION_DEVICE_CHANGE_SPIKE_ANGLE_PERCENTAGE_EXPONENT
	);
	data.spikeLocalAngle = mod(data.spikeLocalAngle, float(2.0 * M_PI));

	MutateCreatureTrait(data.feederLocalAngle,
		CREATURE_MUTATION_DEVICE_CHANGE_FEEDER_ANGLE_CHANCE,
		CREATURE_MUTATION_DEVICE_CHANGE_FEEDER_ANGLE_MAX_ABS,
		CREATURE_MUTATION_DEVICE_CHANGE_FEEDER_ANGLE_PERCENTAGE_EXPONENT
	);
	data.feederLocalAngle = mod(data.feederLocalAngle, float(2.0 * M_PI));

	MutateCreatureTrait(data.shieldLocalAngle,
		CREATURE_MUTATION_DEVICE_CHANGE_SHIELD_ANGLE_CHANCE,
		CREATURE_MUTATION_DEVICE_CHANGE_SHIELD_ANGLE_MAX_ABS,
		CREATURE_MUTATION_DEVICE_CHANGE_SHIELD_ANGLE_PERCENTAGE_EXPONENT
	);
	data.shieldLocalAngle = mod(data.shieldLocalAngle, float(2.0 * M_PI));


	GetCreatureAttributeBySSBOIndex(creature_Positions, p1SSBO, &data.pos);



	data.skinSaturation = 1.0;
	data.skinValue = 1.0;

	data.vel = vec2(0, 0);
	data.angle = random() * 2.0 * M_PI;
	data.angleVel = 0;

	data.hardness = SIMULATION_OFFSPRING_CREATURE_INITIAL_HARDNESS.value;
	data.rad = SIMULATION_OFFSPRING_CREATURE_INITIAL_RADIUS.value;

	data.life = SIMULATION_OFFSPRING_CREATURE_INITIAL_LIFE.value;
	data.energy = SIMULATION_OFFSPRING_CREATURE_INITIAL_ENERGY.value;
	data.meat = SIMULATION_OFFSPRING_CREATURE_INITIAL_MEAT.value;

	float spikeState = 0;
	float feederState = 0;
	float shieldState = 0;
	float shieldSpan = 0.5;
	data.spike = vec4(0, 0, spikeState, 0);
	data.feeder = vec4(0, 0, feederState, 0);
	data.shield = vec4(0, 0, shieldState, shieldSpan);

	CreatureUniqueID newCreatureID = CreatureData_AddCreature(data);
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
ProgramInfo program_CreatureInteractionsPart1{ 0, 0, TECH_CREATURE_INTERACTIONS_PART1_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureInteractionsPart2{ 0, 0, TECH_CREATURE_INTERACTIONS_PART2_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureInteractionsPart3{ 0, 0, TECH_CREATURE_INTERACTIONS_PART3_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureSightsPart1{ 0, 0, TECH_CREATURE_SIGHTS_PART1_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureSightsPart2{ 0, 0, TECH_CREATURE_SIGHTS_PART2_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureSightsPart3{ 0, 0, TECH_CREATURE_SIGHTS_PART3_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BrainPushInputs{ 0, 0, TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_BrainForwardPropagate{ 0, 0, TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureBrainPullOutputsAndBodyWorksPart1{ 0, 0, TECH_CREATURE_BRAIN_PULL_OUTPUTS_AND_BODY_WORKS_PART1_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_CreatureBrainPullOutputsAndBodyWorksPart2{ 0, 0, TECH_CREATURE_BRAIN_PULL_OUTPUTS_AND_BODY_WORKS_PART2_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_FramePreLogic{ 0, 0, TECH_FRAME_PRE_LOGIC_WORKGROUP_LOCAL_SIZE };
ProgramInfo program_FramePostLogic{ 0, 0, TECH_FRAME_POST_LOGIC_WORKGROUP_LOCAL_SIZE };


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
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureInteractionsPart1);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureInteractionsPart2);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureInteractionsPart3);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureSightsPart1);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureSightsPart2);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureSightsPart3);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BrainPushInputs);
	SetProgramInfoNumOfWorkGroupsNeeded(program_BrainForwardPropagate);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureBrainPullOutputsAndBodyWorksPart1);
	SetProgramInfoNumOfWorkGroupsNeeded(program_CreatureBrainPullOutputsAndBodyWorksPart2);
	SetProgramInfoNumOfWorkGroupsNeeded(program_FramePreLogic);
	SetProgramInfoNumOfWorkGroupsNeeded(program_FramePostLogic);

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
	float newMaxCreatureSenseRadius = CREATURE_EYE_MAX_CONES_RADIUS.value;// sqrt(2 * pow(CREATURE_EYE_MAX_PROBE_DISTANCE.value, 2)) + CREATURE_EYE_MAX_CONES_RADIUS.value;
	float newMinCreatureRadius = CREATURE_MIN_RADIUS.value;
	float newSimulationWidth = SIMULATION_SPACE_WIDTH.value;
	float newSimulationHeight = SIMULATION_SPACE_HEIGHT.value;

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
	float tileArea = pow(newInteractDist, 2);
	float minRadiusCreatureArea = M_PI * (pow(newMinCreatureRadius, 2));
	uint maxNumOfCreaturesSqueezableInTile = ceil(tileArea / minRadiusCreatureArea);
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
	//glEnable(GL_MULTISAMPLE);

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

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_FramePreLogic.workGroupLocalSize)));
	GLenum framePreLogicShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* framePreLogicShaderPaths[] = { "resources/compute shaders/frame_pre_logic.computeShader" };
	program_FramePreLogic.program = CreateLinkedShaderProgram(1, framePreLogicShaderTypes, framePreLogicShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_FramePostLogic.workGroupLocalSize)));
	GLenum framePostLogicShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* framePostLogicShaderPaths[] = { "resources/compute shaders/frame_post_logic.computeShader" };
	program_FramePostLogic.program = CreateLinkedShaderProgram(1, framePostLogicShaderTypes, framePostLogicShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureBrainPullOutputsAndBodyWorksPart1.workGroupLocalSize)));
	GLenum creatureBodyWorksPart1ShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureBodyWorksPart1ShaderPaths[] = { "resources/compute shaders/creature_brain_pull_outputs_and_body_works_part1.computeShader" };
	program_CreatureBrainPullOutputsAndBodyWorksPart1.program = CreateLinkedShaderProgram(1, creatureBodyWorksPart1ShaderTypes, creatureBodyWorksPart1ShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureBrainPullOutputsAndBodyWorksPart2.workGroupLocalSize)));
	GLenum creatureBodyWorksPart2ShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureBodyWorksPart2ShaderPaths[] = { "resources/compute shaders/creature_brain_pull_outputs_and_body_works_part2.computeShader" };
	program_CreatureBrainPullOutputsAndBodyWorksPart2.program = CreateLinkedShaderProgram(1, creatureBodyWorksPart2ShaderTypes, creatureBodyWorksPart2ShaderPaths, &replacers);
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

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureInteractionsPart1.workGroupLocalSize)));
	GLenum creatureInteractionsPart1ShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureInteractionsPart1ShaderPaths[] = { "resources/compute shaders/creature_interactions_part1.computeShader" };
	program_CreatureInteractionsPart1.program = CreateLinkedShaderProgram(1, creatureInteractionsPart1ShaderTypes, creatureInteractionsPart1ShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureInteractionsPart2.workGroupLocalSize)));
	GLenum creatureInteractionsPart2ShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureInteractionsPart2ShaderPaths[] = { "resources/compute shaders/creature_interactions_part2.computeShader" };
	program_CreatureInteractionsPart2.program = CreateLinkedShaderProgram(1, creatureInteractionsPart2ShaderTypes, creatureInteractionsPart2ShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureInteractionsPart3.workGroupLocalSize)));
	GLenum creatureInteractionsPart3ShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureInteractionsPart3ShaderPaths[] = { "resources/compute shaders/creature_interactions_part3.computeShader" };
	program_CreatureInteractionsPart3.program = CreateLinkedShaderProgram(1, creatureInteractionsPart3ShaderTypes, creatureInteractionsPart3ShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureSightsPart1.workGroupLocalSize)));
	replacers.push_back(make_pair("@CREATURE_EYE_NUM_OF_CONES_VALUES@", to_string(CREATURE_EYE_NUM_OF_CONES_VALUES)));
	GLenum creatureSightsPart1ShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureSightsPart1ShaderPaths[] = { "resources/compute shaders/creature_sights_part1.computeShader" };
	program_CreatureSightsPart1.program = CreateLinkedShaderProgram(1, creatureSightsPart1ShaderTypes, creatureSightsPart1ShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureSightsPart2.workGroupLocalSize)));
	GLenum creatureSightsPart2ShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureSightsPart2ShaderPaths[] = { "resources/compute shaders/creature_sights_part2.computeShader" };
	program_CreatureSightsPart2.program = CreateLinkedShaderProgram(1, creatureSightsPart2ShaderTypes, creatureSightsPart2ShaderPaths, &replacers);
	replacers.clear();

	replacers.push_back(make_pair("@LOCAL_SIZE@", to_string(program_CreatureSightsPart3.workGroupLocalSize)));
	GLenum creatureSightsPart3ShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* creatureSightsPart3ShaderPaths[] = { "resources/compute shaders/creature_sights_part3.computeShader" };
	program_CreatureSightsPart3.program = CreateLinkedShaderProgram(1, creatureSightsPart3ShaderTypes, creatureSightsPart3ShaderPaths, &replacers);
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
		AddFirstGenerationCreature();
	}

}

//////////////////////////////////
// -- SIMULATION UPDATE STEP -- //
//////////////////////////////////

float firstgenCreatureSpawn_CreaturesToSpawn = 0.0;
bool firstgenCreatureSpawn_PulseActive = false;
float firstgenCreatureSpawn_OscillateX = 0.0;
void Simulation_FirstgenCreatureSpawns()
{
	// Constant spawn
	firstgenCreatureSpawn_CreaturesToSpawn += SIMULATION_FIRSTGEN_CREATURE_CONSTANT_SPAWN_RATE.value;


	// Pulse spawn
	if (firstgenCreatureSpawn_PulseActive)
	{
		firstgenCreatureSpawn_CreaturesToSpawn += SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_RATE.value;
		firstgenCreatureSpawn_PulseActive = creature_count + firstgenCreatureSpawn_CreaturesToSpawn < SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_NUM_OF_CREATURES_UPPER_TARGET.value;
	}
	else
	{
		firstgenCreatureSpawn_PulseActive = creature_count < SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_NUM_OF_CREATURES_LOWER_TARGET.value;
	}

	// Oscilalte spawn
	float oscillateZeroToOne = cos(firstgenCreatureSpawn_OscillateX) / 2.0 + 0.5;
	float oscillateZeroToOneExp = pow(oscillateZeroToOne, SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_SPAWN_RATE_EXPONENT.value);
	float upRate = SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_UP_SPAWN_RATE.value;
	float downRate = SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_DOWN_SPAWN_RATE.value;
	float xStep = SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_STEP.value;
	firstgenCreatureSpawn_CreaturesToSpawn += (downRate + (upRate - downRate) * oscillateZeroToOneExp);
	firstgenCreatureSpawn_OscillateX = mod(firstgenCreatureSpawn_OscillateX + xStep, (float)(M_PI * 2.0));

	// Finalize
	while (firstgenCreatureSpawn_CreaturesToSpawn >= 1.0)
	{
		AddFirstGenerationCreature();
		firstgenCreatureSpawn_CreaturesToSpawn -= 1.0;
	}
}

void Simulation_Programs_Sequence()
{

	/* @TODO:
	The number of work groups that can be dispatched in a single dispatch call is defined
	by GL_MAX_COMPUTE_WORK_GROUP_COUNT. This must be queried with glGetIntegeri_v.
	*/

	RecalculateAllProgramInfosNumberOfWorkGroupsNeeded();


	GLuint programID;
	GLuint workGroupsNeeded;


	// Init new frame
	programID = program_FramePreLogic.program;
	workGroupsNeeded = program_FramePreLogic.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_CollidersCounts.bufferHandle); // Zerofiy collider counts after rendering them
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	// Push brain inputs
	programID = program_BrainPushInputs.program;
	workGroupsNeeded = program_BrainPushInputs.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", CREATURE_BRAIN_MAX_NUM_OF_NODES);
	SetUniformFloat(programID, "uCreatureMaxEnergy", CREATURE_MAX_ENERGY.value);
	SetUniformFloat(programID, "uCreatureMaxLife", CREATURE_MAX_LIFE.value);
	SetUniformFloat(programID, "uCreatureMaxMeat", CREATURE_MAX_MEAT.value);
	SetUniformFloat(programID, "uCreatureMaxVelocityLength", CREATURE_MAX_VELOCITY_LENGTH.value);
	SetUniformFloat(programID, "uCreatureMaxAngleVelocityLength", CREATURE_MAX_ANGLE_VELOCITY_LENGTH.value);
	SetUniformUInteger(programID, "uCreatureEyeNumOfPupilValues", CREATURE_EYE_NUM_OF_PUPIL_VALUES);
	SetUniformUInteger(programID, "uCreatureEyeNumOfConesValues", CREATURE_EYE_NUM_OF_CONES_VALUES);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Lives.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Energies.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Meats.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_ForwardDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_RightDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_Velocities.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_AngleVelocities.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_EyePupilSights.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_EyeConeSights.bufferHandle);
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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_BrainsBiasesExponents.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_BrainsLinks.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Lives.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	// Creature pull brain outputs and body works part 1 (movement and eye control)
	programID = program_CreatureBrainPullOutputsAndBodyWorksPart1.program;
	workGroupsNeeded = program_CreatureBrainPullOutputsAndBodyWorksPart1.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", CREATURE_BRAIN_MAX_NUM_OF_NODES);
	SetUniformFloat(programID, "uCreatureMaxEnergy", CREATURE_MAX_ENERGY.value);
	SetUniformFloat(programID, "uCreatureEnergyPercentageBasedMovementMultiplierExponent", CREATURE_ENERGY_PERCENTAGE_BASED_MOVEMENT_MULTIPLIER_EXPONENT.value);
	SetUniformFloat(programID, "uCreatureStrafeMovementEffectivness", CREATURE_STRAFE_MOVEMENT_EFFECTIVENESS.value);
	SetUniformFloat(programID, "uCreatureForwardMovementEffectiveness", CREATURE_FORWARD_MOVEMENT_EFFECTIVENESS.value);
	SetUniformFloat(programID, "uCreatureBackwardMovementEffectivness", CREATURE_BACKWARD_MOVEMENT_EFFECTIVENESS.value);
	SetUniformFloat(programID, "uCreatureTurnMovementEffectiveness", CREATURE_TURN_MOVEMENT_EFFECTIVENESS.value);
	SetUniformFloat(programID, "uCreatureForwardMovementEnergyCost", CREATURE_FORWARD_MOVEMENT_ENERGY_COST.value);
	SetUniformFloat(programID, "uCreatureBackwardMovementEnergyCost", CREATURE_BACKWARD_MOVEMENT_ENERGY_COST.value);
	SetUniformFloat(programID, "uCreatureStrafeMovementEnergyCost", CREATURE_STRAFE_MOVEMENT_ENERGY_COST.value);
	SetUniformFloat(programID, "uCreatureTurnMovementEnergyCost", CREATURE_TURN_MOVEMENT_ENERGY_COST.value);
	SetUniformFloat(programID, "uCreatureEyeMaxConeRadius", CREATURE_EYE_MAX_CONES_RADIUS.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Lives.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Energies.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_ForwardDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_RightDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_Velocities.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_AngleVelocities.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_EyeMuscles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_EyeConeRadii.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_EyePupilConeCoverageFraction.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Creature pull brain outputs and body works part 2 (life/energy/meat + devices management)
	programID = program_CreatureBrainPullOutputsAndBodyWorksPart2.program;
	workGroupsNeeded = program_CreatureBrainPullOutputsAndBodyWorksPart2.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uMaxNumOfStructureIndices", CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	SetUniformUInteger(programID, "uMaxNumOfNodesInBrain", CREATURE_BRAIN_MAX_NUM_OF_NODES);
	SetUniformFloat(programID, "uCreatureMaxEnergy", CREATURE_MAX_ENERGY.value);
	SetUniformFloat(programID, "uCreatureMaxMeat", CREATURE_MAX_MEAT.value);
	SetUniformFloat(programID, "uCreatureMaxLife", CREATURE_MAX_LIFE.value);
	SetUniformFloat(programID, "uCreatureEnergyToMeatConversionRate", CREATURE_ENERGY_TO_MEAT_CONVERSION_RATE.value);
	SetUniformFloat(programID, "uCreatureMeatToEnergyConversionRate", CREATURE_MEAT_TO_ENERGY_CONVERSION_RATE.value);
	SetUniformFloat(programID, "uCreatureEnergyToLifeConversionRate", CREATURE_ENERGY_TO_LIFE_CONVERSION_RATE.value);
	SetUniformFloat(programID, "uCreatureMaxRadius", CREATURE_MAX_RADIUS.value);
	SetUniformFloat(programID, "uCreatureMinRadius", CREATURE_MIN_RADIUS.value);
	SetUniformFloat(programID, "uCreatureRadiusInterpolationRate", CREATURE_RADIUS_INTERPOLATION_RATE.value);
	SetUniformFloat(programID, "uCreatureRadiusPercentageMeatWeight", CREATURE_RADIUS_PERCENTAGE_MEAT_WEIGHT.value);
	SetUniformFloat(programID, "uCreatureRadiusPercentageMuscleWeight", CREATURE_RADIUS_PERCENTAGE_MUSCLE_WEIGHT.value);
	SetUniformFloat(programID, "uCreatureMaxHardness", CREATURE_MAX_HARDNESS.value);
	SetUniformFloat(programID, "uCreatureMinHardness", CREATURE_MIN_HARDNESS.value);
	SetUniformFloat(programID, "uCreatureHardnessInterpolationRate", CREATURE_HARDNESS_INTERPOLATION_RATE.value);
	SetUniformFloat(programID, "uCreatureDeviceSpikeEnergyCost", CREATURE_DEVICE_SPIKE_ENERGY_COST.value);
	SetUniformFloat(programID, "uCreatureDeviceShieldEnergyCost", CREATURE_DEVICE_SHIELD_ENERGY_COST.value);
	SetUniformFloat(programID, "uCreatureDeviceFeederEnergyCost", CREATURE_DEVICE_FEEDER_ENERGY_COST.value);
	SetUniformFloat(programID, "uCreatureDeviceStateInterpolationRate", CREATURE_DEVICE_STATE_INTERPOLATION_RATE.value);
	SetUniformFloat(programID, "uCreatureMaxSkinValue", CREATURE_MAX_SKIN_VALUE.value);
	SetUniformFloat(programID, "uCreatureMinSkinValue", CREATURE_MIN_SKIN_VALUE.value);
	SetUniformFloat(programID, "uCreatureMaxSkinSaturation", CREATURE_MAX_SKIN_SATURATION.value);
	SetUniformFloat(programID, "uCreatureMinSkinSaturation", CREATURE_MIN_SKIN_SATURATION.value);
	SetUniformFloat(programID, "uCreatureSkinValueInterpolationRate", CREATURE_SKIN_VALUE_INTERPOLATION_RATE.value);
	SetUniformFloat(programID, "uCreatureLifeDrainOnNoEnergy", CREATURE_LIFE_DRAIN_ON_NO_ENERGY.value);
	SetUniformFloat(programID, "uCreatureEnergyDrainConstant", CREATURE_ENERGY_DRAIN_CONSTANT.value);
	SetUniformFloat(programID, "uCreatureDeathMeatRotRate", CREATURE_DEATH_MEAT_ROT_RATE.value);
	SetUniformFloat(programID, "uCreatureDeathWithoutMeatShrinkRate", CREATURE_DEATH_WITHOUT_MEAT_SHRINK_RATE.value);
	SetUniformFloat(programID, "uCreatureDeathExistenceRadiusThreshold", CREATURE_DEATH_EXISTENCE_RADIUS_THRESHOLD.value);
	SetUniformFloat(programID, "uCreatureDeathHardnessTarget", CREATURE_DEATH_HARDNESS_TARGET.value);
	SetUniformFloat(programID, "uCreatureDeathHardnessInterpolationRate", CREATURE_DEATH_HARDNESS_INTERPOLATION_RATE.value);
	SetUniformFloat(programID, "uCreatureDeathDeviceZeroficationInterpolationRate", CREATURE_DEATH_DEVICE_ZEROFICATION_INTERPOLATION_RATE.value);
	SetUniformFloat(programID, "uCreatureDeathEnergyZeroficationRate", CREATURE_DEATH_ENERGY_ZEROFICATION_INTERPOLATION_RATE.value);
	SetUniformFloat(programID, "uCreatureDeathSkinValueTarget", CREATURE_DEATH_SKIN_VALUE_TARGET.value);
	SetUniformFloat(programID, "uCreatureDeathSkinValueInterpolationRate", CREATURE_DEATH_SKIN_VALUE_INTERPOLATION_RATE.value);
	SetUniformFloat(programID, "uCreatureEnergyPercentageBasedDeviceStateMultiplierExponent", CREATURE_ENERGY_PERCENTAGE_BASED_DEVICE_STATE_MULTIPLIER_EXPONENT.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_BrainsStructures.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_BrainsNodes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Lives.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Energies.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Meats.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_Radii.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_Harndesses.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_Spikes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_Shields.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_Feeders.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_SkinValues.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, creature_SkinSaturations.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, creature_Horninesses.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, creatureList_Vanishes.creaturesSSBOInfo.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	// Update body placements
	programID = program_UpdateCreaturePlacements.program;
	workGroupsNeeded = program_UpdateCreaturePlacements.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformFloat(programID, "uVelocityDownscale", SIMULATION_VELOCITY_DOWNSCALE.value);
	SetUniformFloat(programID, "uAngleVelocityDownscale", SIMULATION_ANGLE_VELOCITY_DOWNSCALE.value);
	SetUniformFloat(programID, "uCreatureEyeMaxProbeDistance", CREATURE_EYE_MAX_PROBE_DISTANCE.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_GeneralPurposeVec2.bufferHandle); // Applies physics fix vector, zerofies
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Angles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_AngleVelocities.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_ForwardDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_RightDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_SpikeLocalAngles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_Spikes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_FeederLocalAngles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_Feeders.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, creature_ShieldLocalAngles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, creature_Shields.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, creature_EyeMuscles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 14, creature_EyePositions.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Border physics
	programID = program_BorderPhysics.program;
	workGroupsNeeded = program_BorderPhysics.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformVector2f(programID, "uSimDimensions", vec2(SIMULATION_SPACE_WIDTH.value, SIMULATION_SPACE_HEIGHT.value));
	SetUniformFloat(programID, "uBorderRestitution", SIMULATION_BORDER_RESTITUTION.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Radii.bufferHandle);
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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ugrid_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_UniformGridTiles.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Creature interactions part 1 - collision detection and registration, physics fix
	programID = program_CreatureInteractionsPart1.program;
	workGroupsNeeded = program_CreatureInteractionsPart1.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformVector2ui(programID, "uGridDimensions", uvec2(ugrid_GridXDim, ugrid_GridYDim));
	SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
	SetUniformUInteger(programID, "uMaxNumOfColliders", CREATURE_MAX_NUM_OF_COLLIDERS);
	SetUniformVector2f(programID, "uRandom", vec2(random() - 0.5, random() - 0.5)); // Used to resolve creatures absolutely clipped in each other
	SetUniformFloat(programID, "uCreatureDefaultBodyMass", CREATURE_DEFAULT_BODY_MASS); // Used to avoid 0 mass on collisions
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Velocities.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_Radii.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Meats.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Harndesses.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ugrid_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_UniformGridTiles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_GeneralPurposeVec2.bufferHandle); // Writes physics fix vector for decoupling purposes
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_CollidersCounts.bufferHandle); // Count colliders
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_CollidersIndicesAndPlacements.bufferHandle); // Remember colliders (for interaction part 2/3)
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_CollidersToPosDirs.bufferHandle); // Remember direction of colliders (for interaction part 2/3)
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, creature_CollidersPositions.bufferHandle); // Remember positions of colliders (for deformations)
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, creature_CollidersRadii.bufferHandle); // Remember radius of colliders (for deformations)
	// @TODO: Add stick states/dirs
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Creature interactions part 2 - device based energy flow calculation
	programID = program_CreatureInteractionsPart2.program;
	workGroupsNeeded = program_CreatureInteractionsPart2.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uMaxNumOfColliders", CREATURE_MAX_NUM_OF_COLLIDERS);
	SetUniformFloat(programID, "uDeviceAimDotThreshold", CREATURE_DEVICE_AIM_DOT_THRESHOLD.value);
	SetUniformFloat(programID, "uCreatureFeederEffectiveness", CREATURE_DEVICE_FEEDER_EFFECTIVENESS.value);
	SetUniformFloat(programID, "uCreatureSpikeVersusLifeEffectiveness", CREATURE_DEVICE_SPIKE_VERSUS_LIFE_EFFECTIVENESS.value);
	SetUniformFloat(programID, "uCreatureSpikeVersusMeatEffectiveness", CREATURE_DEVICE_SPIKE_VERSUS_MEAT_EFFECTIVENESS.value);
	SetUniformFloat(programID, "uCreatureShieldEffectiveness", CREATURE_DEVICE_SHIELD_EFFECTIVENESS.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_CollidersCounts.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_CollidersIndicesAndPlacements.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_CollidersToPosDirs.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Spikes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Feeders.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_Shields.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_CollidersGivenEnergy.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_Lives.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_Energies.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_Meats.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Creature interactions part 3 - device based energy flow submission and reproduction
	programID = program_CreatureInteractionsPart3.program;
	workGroupsNeeded = program_CreatureInteractionsPart3.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uMaxNumOfColliders", CREATURE_MAX_NUM_OF_COLLIDERS);
	SetUniformFloat(programID, "uCreatureReproductionEnergyCost", CREATURE_REPRODUCTION_ENERGY_COST.value);
	SetUniformFloat(programID, "uCreatureReproductionEnergyThreshold", CREATURE_REPRODUCTION_ENERGY_THRESHOLD.value);
	SetUniformFloat(programID, "uCreatureReproductionMeatThreshold", CREATURE_REPRODUCTION_MEAT_THRESHOLD.value);
	SetUniformFloat(programID, "uCreatureReproductionAimDotThreshold", CREATURE_REPRODUCTION_AIM_DOT_THRESHOLD.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_CollidersCounts.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_CollidersGivenEnergy.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_CollidersToPosDirs.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Energies.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Meats.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_ForwardDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_Lives.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_Horninesses.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_CollidersIndicesAndPlacements.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creatureList_NewBorns.creaturesSSBOInfo.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Creature sights part 1 (find which creatures we're looking at in the simulation space, cone activation)
	// We don't load actual observed data just yet due to the GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS <= 16 limit
	programID = program_CreatureSightsPart1.program;
	workGroupsNeeded = program_CreatureSightsPart1.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformVector2f(programID, "uSimDimensions", vec2(ugrid_SimWidth, ugrid_SimHeight));
	SetUniformVector2ui(programID, "uGridDimensions", uvec2(ugrid_GridXDim, ugrid_GridYDim));
	SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
	SetUniformUInteger(programID, "uCreatureEyeNumOfCones", CREATURE_EYE_NUM_OF_CONES);
	SetUniformUInteger(programID, "uCreatureEyeNumOfValuesInSingleCone", CREATURE_EYE_NUM_OF_VALUES_IN_SINGLE_CONE);
	SetUniformUInteger(programID, "uCreatureEyeNumOfConesValues", CREATURE_EYE_NUM_OF_CONES_VALUES);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_Positions.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_Radii.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ugrid_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_Angles.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_EyePositions.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_EyeConeRadii.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_EyeConeSights.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_EyePupilConeCoverageFraction.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_GeneralPurposeUInt.bufferHandle); // Write pupil creature target index
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, creature_GeneralPurposeFloat.bufferHandle); // Write pupil activation
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, creature_GeneralPurposeSecondVec2.bufferHandle); // Write pupil normalized direction between creatures
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, creature_Lives.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Load observed creatures' data observed by pupils

	// Creature sights part 2 - devices
	programID = program_CreatureSightsPart2.program;
	workGroupsNeeded = program_CreatureSightsPart2.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uCreatureEyeNumOfPupilValues", CREATURE_EYE_NUM_OF_PUPIL_VALUES);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_EyePupilSights.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_GeneralPurposeUInt.bufferHandle); // Read pupil creature target index
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_GeneralPurposeFloat.bufferHandle); // Read pupil activation
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_GeneralPurposeSecondVec2.bufferHandle); // Read pupil normalized direction between creatures
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_Shields.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_Spikes.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_Feeders.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_ForwardDirections.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_Lives.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Creature sights part 3 - appearances
	programID = program_CreatureSightsPart3.program;
	workGroupsNeeded = program_CreatureSightsPart3.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uCreatureEyeNumOfPupilValues", CREATURE_EYE_NUM_OF_PUPIL_VALUES);
	SetUniformFloat(programID, "uCreatureMaxRadius", CREATURE_MAX_RADIUS.value);
	SetUniformFloat(programID, "uCreatureMinRadius", CREATURE_MIN_RADIUS.value);
	SetUniformFloat(programID, "uCreatureMaxEnergy", CREATURE_MAX_ENERGY.value);
	SetUniformFloat(programID, "uCreatureMaxLife", CREATURE_MAX_LIFE.value);
	SetUniformFloat(programID, "uCreatureMaxSkinValue", CREATURE_MAX_SKIN_VALUE.value);
	SetUniformFloat(programID, "uCreatureMinSkinValue", CREATURE_MIN_SKIN_VALUE.value);
	SetUniformFloat(programID, "uCreatureMaxSkinSaturation", CREATURE_MAX_SKIN_SATURATION.value);
	SetUniformFloat(programID, "uCreatureMinSkinSaturation", CREATURE_MIN_SKIN_SATURATION.value);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_EyePupilSights.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_GeneralPurposeUInt.bufferHandle); // Read pupil creature target index
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_SkinHues.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_SkinSaturations.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, creature_SkinValues.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, creature_SkinPatterns.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, creature_Lives.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, creature_Energies.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, creature_Radii.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// Uniform grid unbind
	programID = program_UniformGridUnBind.program;
	workGroupsNeeded = program_UniformGridUnBind.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	SetUniformUInteger(programID, "uIndicesInTile", ugrid_IndicesInTile);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ugrid_SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_UniformGridTiles.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	// End frame (convert HSV to RGB)
	programID = program_FramePostLogic.program;
	workGroupsNeeded = program_FramePostLogic.workGroupsNeeded;
	glUseProgram(programID);
	SetUniformUInteger(programID, "uCreatureCount", creature_count);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, creature_SkinHues.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, creature_SkinSaturations.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, creature_SkinValues.bufferHandle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, creature_SkinRGBColors.bufferHandle);
	glDispatchCompute(workGroupsNeeded, 1, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

const GLuint zeroCount = 0;
void VanishedCreatureRemoval()
{
	GLuint unitSize = creatureList_Vanishes.creaturesSSBOInfo.unitByteSize;
	GLuint handle = creatureList_Vanishes.creaturesSSBOInfo.bufferHandle;

	// Acquire number of creatures to be removed
	GLuint numOfVanishedCreatures = ((GLuint*)creatureList_Vanishes.mapPtr)[0];

	if (numOfVanishedCreatures <= 0) return;

	// Zerofiy count, make the write immediately visible to OpenGL
	((GLuint*)creatureList_Vanishes.mapPtr)[0] = 0;
	glFlushMappedNamedBufferRange(handle, 0, unitSize); // @TODO: Is this needed?

	// Each call to remove creature (by SSBO index) can shift creatures in the buffer, work with unique IDs instead!
	vector<CreatureUniqueID> vanishedCreatureIDs(numOfVanishedCreatures);
	for (unsigned int i = 0; i < numOfVanishedCreatures; ++i)
	{
		GLuint ssboIndex = ((GLuint*)creatureList_Vanishes.mapPtr)[i + 1];
		CreatureUniqueID uniqueID = CreatureData_CreatureSSBOIndexToUniqueID(ssboIndex);
		vanishedCreatureIDs[i] = uniqueID;
	}

	// Remove creatures based on the unique IDs we found
	for (unsigned int i = 0; i < numOfVanishedCreatures; ++i)
	{
		CreatureData_RemoveCreatureByUniqueID(vanishedCreatureIDs[i]);
	}
}

void CreatureNewbornsCreation()
{
	GLuint unitSize = creatureList_NewBorns.creaturesSSBOInfo.unitByteSize;
	GLuint handle = creatureList_NewBorns.creaturesSSBOInfo.bufferHandle;

	// Acquire number of creatures to be created
	GLuint numOfNewbornCreatures = ((uvec2*)creatureList_NewBorns.mapPtr)[0].x;

	if (numOfNewbornCreatures <= 0) return;

	// Zerofiy count, make the write immediately visible to OpenGL
	((uvec2*)creatureList_NewBorns.mapPtr)[0].x = 0;
	glFlushMappedNamedBufferRange(handle, 0, unitSize); // @TODO: Is this needed?

	for (unsigned int i = 0; i < numOfNewbornCreatures; ++i)
	{
		uvec2 parentsSSBOIndices = ((uvec2*)creatureList_NewBorns.mapPtr)[i + 1];
		AddOffspringCreature(parentsSSBOIndices.x, parentsSSBOIndices.y);
	}
}


void Simulation_Render()
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

	glBindVertexArray(drawCallData_CreatureBody.VAO);
	glUseProgram(drawCallData_CreatureBody.program);
	SetUniformMatrix4(drawCallData_CreatureBody.program, "uTransform", GetSimSpaceToCameraTransform());
	SetUniformUInteger(drawCallData_CreatureBody.program, "uMaxNumOfColliders", CREATURE_MAX_NUM_OF_COLLIDERS);
	SetUniformFloat(drawCallData_CreatureBody.program, "uCreatureMaxEnergy", CREATURE_MAX_ENERGY.value);
	SetUniformFloat(drawCallData_CreatureBody.program, "uCreatureTransparencyEnergyExponent", RENDER_CREATURE_TRANSPARENCY_ENERGY_EXPONENT.value);
	glDrawElementsInstanced(GL_TRIANGLES, drawCallData_CreatureBody.numOfIndices, GL_UNSIGNED_INT, 0, numOfInstances);
	glBindVertexArray(0);
}

bool logicPause = false;
void Simulation_LogicPause(bool enablePause)
{
	logicPause = enablePause;
}

void Simulation_Update()
{
	if (!logicPause)
	{
		// Finish everything the GPU needs to do from the last frame
		glFlush();

		// Add first generation creatures
		Simulation_FirstgenCreatureSpawns();

		// Program logic sequence
		Simulation_Programs_Sequence();

		// Wait until OpenGL finished with all command dequeues
		glFinish();

		// Remove creatures that vanished
		VanishedCreatureRemoval();

		// Handle newborns
		CreatureNewbornsCreation();
	}


	// Render
	Simulation_Render();



	// @DEBUG
	if (random() > 0.99)
	{
		cout << creature_count << endl;
	}

	// @DEBUG sum energy
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Energies.ssbo);
	//void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	//GLfloat energies[100000];
	//memcpy(energies, ptr, sizeof(GLfloat) * 100000);
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	//
	//float sum = 0.0;
	//for (auto energy : energies)
	//{
	//	sum += energy;
	//}

	//cout << sum << endl;
}