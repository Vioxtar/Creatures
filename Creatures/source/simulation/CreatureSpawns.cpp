#include "CreatureSpawns.h"
#include <queue>


//////////////////////////
// -- MUTATION UTILS -- //
//////////////////////////

void MutateClampedCreatureTrait(GLfloat& value, GLfloat minValue, GLfloat maxValue, GLfloat chanceToMutate, GLfloat mutationJitter, GLfloat mutationJitterPercentageExponent)
{
	if (random() >= chanceToMutate) return;
	GLfloat mutation = pow(random(), mutationJitterPercentageExponent) * mutationJitter * randomNegate();
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
		GLfloat bias = 0.0;
		if (random() < CREATURE_BRAIN_FIRSTGEN_BIAS_CHANCE_FOR_NON_NEUTRAL_VALUE)
		{
			bias = pow(random(), CREATURE_BRAIN_FIRSTGEN_BIAS_PERCENTAGE_EXPONENT)
				* CREATURE_BRAIN_FIRSTGEN_BIAS_MAX_ABS
				* randomNegate();
		}

		GLfloat activationExponent = 1.0;
		if (random() < CREATURE_BRAIN_FIRSTGEN_ACTIVATION_EXPONENT_CHANCE_FOR_NON_NEUTRAL_VALUE)
		{
			activationExponent = pow(random(), CREATURE_BRAIN_FIRSTGEN_ACTIVATION_EXPONENT_PERCENTAGE_EXPONENT)
				* CREATURE_BRAIN_FIRSTGEN_ACTIVATION_EXPONENT_MAX_ABS
				* randomNegate();
		}

		brainBiasesExponents->at(i) = vec2(bias, activationExponent);
	}

	// Fill links
	brainLinks->resize(CREATURE_BRAIN_MAX_NUM_OF_LINKS);
	for (int i = 0; i < CREATURE_BRAIN_MAX_NUM_OF_LINKS; i++)
	{
		GLfloat linkWeight = 0.0;
		if (random() < CREATURE_BRAIN_FIRSTGEN_LINK_WEIGHT_CHANCE_FOR_NON_NEUTRAL_VALUE)
		{
			linkWeight = pow(random(), CREATURE_BRAIN_FIRSTGEN_LINK_WEIGHT_PERCENTAGE_EXPONENT)
				* CREATURE_BRAIN_FIRSTGEN_LINK_WEIGHT_MAX_ABS
				* randomNegate();
		}

		brainLinks->at(i) = linkWeight;
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
			-CREATURE_BRAIN_BIAS_MAX_ABS, CREATURE_BRAIN_BIAS_MAX_ABS,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_CHANCE,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_MAX_ABS,
			CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_PERCENTAGE_EXPONENT
		);

		MutateClampedCreatureTrait(exponent,
			-CREATURE_BRAIN_ACTIVATION_EXPONENT_MAX_ABS, CREATURE_BRAIN_ACTIVATION_EXPONENT_MAX_ABS,
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
			-CREATURE_BRAIN_LINK_WEIGHT_MAX_ABS, CREATURE_BRAIN_LINK_WEIGHT_MAX_ABS,
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


/////////////////////////////////////////////////////////////////////////////////////////////////
// -- FIRST GENERATION CREATURE SPAWNING, EVOLUTION INCUBATION AND TRAINING WHEELS PROTOCOL -- //
/////////////////////////////////////////////////////////////////////////////////////////////////

namespace TrainingWheels
{
	bool active = false;
	queue<pair<CreatureData, float>> creaturesToAdd;
	float enqueuedCreaturesAverageScore = 0.0;

	void GiveDeadCreature(CreatureUniqueID creatureID)
	{
		// This is where we test whether or not a recently past creature should be enqueued...
		// This only happens if its score is above enqueuedCreaturesAverageScore, where a creature's
		// score rises with the number of children it produced and decreases with its generation number
		// So new creature's training wheels score can be = (# of children * weight1) / (generation * weight2)... should also consider a max generation threshold just to be safe

		// If enqueued, we update the average score as well

		// We should also do any sort of mutating here, generate a CreatureData instance and enqueue that so that it's ready for drop off
		
		
		GLuint ssbo = CreatureData_CreatureUniqueIDToSSBOIndex(creatureID);
		
		GLuint generation;
		GetCreatureAttributeBySSBOIndex(creature_Generations, ssbo, &generation);

		if (generation > TRAINING_WHEELS_GENERATION_UPPER_THRESHOLD.value) return;

		GLuint offspringCount;
		GetCreatureAttributeBySSBOIndex(creature_OffspringCounts, ssbo, &offspringCount);

		float score =
			(offspringCount * TRAINING_WHEELS_OFFSPRING_COUNT_SCORE_WEIGHT.value) /
			(generation * TRAINING_WHEELS_GENERATION_SCORE_WEIGHT.value);

		if (score <= enqueuedCreaturesAverageScore) return;

		// Create our to-be-enqueued creature data

		CreatureData data;
	}

	bool HasCreatureToGive()
	{
		return creaturesToAdd.size() > 0;
	}

	void AddFirstGenerationCreature(vec2 pos, vec2 vel)
	{
		// This is where the default spawn firstgen logic asks the training wheels protocol to give it a creature to spawn
		// In this case, we simply return the already mutated CreatureData instance by popping it from the queue
		// Only gets called under the assumption there's a creature to provide

		// Dont forget to update score!
	}


	void ActivateOrInactiveIfNeeded()
	{
		// This is where we check if the training wheels protocol should be active or inactive
		// Ideally we would have just iterated all active creatures and check the resulting generations histogram
		// or something like that, but that's not too performant to do every frame - so we have two prominent options:
		// 1. Only call this once every couple of frames (bad)
		// 2. Keep an active statistics packet of information that we update every time we add or remove creatures and use those constantly updated statistics (good)
		// Obviously we should work with 2...

		// however to simplify this further (temporary), just store the current max generation here and return true iff maxGen < threshold
	
		//active = true;
	}
}


void AddFirstGenerationCreature(vec2 pos, vec2 vel)
{
	CreatureData data;

	data.pos = pos;
	data.vel = vel;

	data.generation = 1;
	data.offspringCount = 0;

	InitFirstGenBrain(&data.brainNodes, &data.brainBiasesExponents, &data.brainLinks, &data.brainStructure);

	data.skinPattern = vec2(random(), random());
	data.skinHue = random();
	data.skinSaturation = CREATURE_MAX_SKIN_SATURATION.value;
	data.skinLightness = CREATURE_MAX_SKIN_LIGHTNESS.value;

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

	data.forwardDir = vec2(0.0f, 0.0f);
	data.rightDir = vec2(0.0f, 0.0f);

	data.eyeConeRadius = 0.0;
	data.eyePos = vec2(0, 0);
	data.eyeMuscles = vec2(0.0, 0.0);

	CreatureData_AddCreature(data);
}

float firstgenCreatureSpawn_CreaturesToSpawn = 0.0;
bool firstgenCreatureSpawn_PulseActive = false;
float firstgenCreatureSpawn_OscillateX = 0.0;
vec2 firstgenCreatureSpawn_MovingSpawnPos = vec2(0, 0);
vec2 firstgenCreatureSpawn_MovingSpawnVel = vec2(0, 0);
void FirstgenCreatureSpawns()
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
		float movingSpawnSpeed = SIMULATION_FIRSTGEN_CREATURE_MOVING_SPAWN_VELOCITY_MAGNITUDE.value;
		firstgenCreatureSpawn_MovingSpawnVel += vec2((random() - 0.5) * movingSpawnSpeed, (random() - 0.5) * movingSpawnSpeed);
		firstgenCreatureSpawn_MovingSpawnPos += firstgenCreatureSpawn_MovingSpawnVel;
		if (abs(firstgenCreatureSpawn_MovingSpawnPos.x) > SIMULATION_SPACE_WIDTH.value / 2.0)
		{
			firstgenCreatureSpawn_MovingSpawnPos.x = clamp(firstgenCreatureSpawn_MovingSpawnPos.x, -SIMULATION_SPACE_WIDTH.value / 2.0, SIMULATION_SPACE_WIDTH.value / 2.0);
			firstgenCreatureSpawn_MovingSpawnVel.x *= -0.95;
		}
		if (abs(firstgenCreatureSpawn_MovingSpawnPos.y) > SIMULATION_SPACE_HEIGHT.value / 2.0)
		{
			firstgenCreatureSpawn_MovingSpawnPos.y = clamp(firstgenCreatureSpawn_MovingSpawnPos.y, -SIMULATION_SPACE_HEIGHT.value / 2.0, SIMULATION_SPACE_HEIGHT.value / 2.0);
			firstgenCreatureSpawn_MovingSpawnVel.y *= -0.95;
		}

		AddFirstGenerationCreature(firstgenCreatureSpawn_MovingSpawnPos, vec2(0.0, 0.0));
		firstgenCreatureSpawn_CreaturesToSpawn -= 1.0;
	}
}


void CreatureSpawns::Update()
{
	FirstgenCreatureSpawns();
}

void CreatureSpawns::AddOffspringCreature(unsigned int p1SSBO, unsigned int p2SSBO)
{

	CreatureUniqueID parent1ID = CreatureData_CreatureSSBOIndexToUniqueID(p1SSBO);
	CreatureUniqueID parent2ID = CreatureData_CreatureSSBOIndexToUniqueID(p2SSBO);

	CreatureData data;

	GetCreatureAttributeBySSBOIndex(creature_Generations, p1SSBO, &data.generation);
	data.generation++;

	data.offspringCount = 0;

	// Init mutated brain
	InitOffspringBrain(p1SSBO, &data.brainNodes, &data.brainBiasesExponents, &data.brainLinks, &data.brainStructure);

	// Copy some parent values to the offspring values and mutate
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

	// Set more initial values
	data.skinSaturation = CREATURE_MAX_SKIN_SATURATION.value;
	data.skinLightness = CREATURE_MAX_SKIN_LIGHTNESS.value;

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

	data.forwardDir = vec2(0.0f, 0.0f);
	data.rightDir = vec2(0.0f, 0.0f);

	data.eyeConeRadius = 0.0;
	data.eyePos = vec2(0, 0);
	data.eyeMuscles = vec2(0.0, 0.0);

	CreatureUniqueID newCreatureID = CreatureData_AddCreature(data);
}

void CreatureSpawns::RemoveCreatureByUniqueID(CreatureUniqueID creatureID)
{
	CreatureData_RemoveCreatureByUniqueID(creatureID);
}