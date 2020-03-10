#include "CreatureData.h"


CreatureUniqueID creature_NextUniqueIDToBeAssigned = 0;
map<CreatureUniqueID, GLuint> creature_UniqueIDsToSSBOIndex;
vector<CreatureUniqueID> creature_UniqueIDs;

GLuint CreatureUniqueIDToSSBOIndex(CreatureUniqueID creatureID)
{
	return creature_UniqueIDsToSSBOIndex.at(creatureID);
}

CreatureUniqueID CreatureSSBOIndexToUniqueID(GLuint ssboIndex)
{
	// @TODO: Check that ssbo index < creature_count
	return creature_UniqueIDs[ssboIndex];
}

extern GLuint creature_count = 0; // The count of active creatures in the simulation
extern GLuint max_supported_creature_count_by_current_buffers = 0; // The number of creatures supported by current SSBO buffers


////////////////////////////////////
// -- CREATURE ATTRIBUTE SSBOS -- //
////////////////////////////////////


// Brains
extern CreaturesSSBOInfo creature_BrainsLinks{ 0, sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_LINKS, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_BrainsNodes{ 0, sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_NODES, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_BrainsBiasesExponents{ 0, sizeof(vec2) * CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_BrainsStructures{ 0, sizeof(GLuint) * CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// Eyes
extern CreaturesSSBOInfo creature_EyeMuscles{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_EyePositions{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_EyeConeRadii{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_EyePupilConeCoverageFraction{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_EyePupilSights{ 0, sizeof(GLfloat) * CREATURE_EYE_NUM_OF_PUPIL_VALUES, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_EyeConeSights{ 0, sizeof(GLfloat) * CREATURE_EYE_NUM_OF_CONES_VALUES, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// Physics
extern CreaturesSSBOInfo creature_Positions{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Velocities{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Angles{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_AngleVelocities{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_ForwardDirections{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_RightDirections{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// Movement
extern CreaturesSSBOInfo creature_ForwardThrusts{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_TurnThrusts{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// Body parameters
extern CreaturesSSBOInfo creature_Radii{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Lives{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Energies{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Meats{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Harndesses{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Horninesses{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// Creature misc
extern CreaturesSSBOInfo creature_Generations{ 0, sizeof(GLuint), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_UniformGridTiles{ 0, sizeof(GLint), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// Colliders
extern CreaturesSSBOInfo creature_CollidersCounts{ 0, sizeof(GLuint) };
extern CreaturesSSBOInfo creature_CollidersIndicesAndPlacements{ 0, sizeof(uvec2) * CREATURE_MAX_NUM_OF_COLLIDERS, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_CollidersToPosDirs{ 0, sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_CollidersPositions{ 0, sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_CollidersRadii{ 0, sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_CollidersGivenEnergy{ 0, sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };


// Appearances
extern CreaturesSSBOInfo creature_SkinHues{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_SkinSaturations{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_SkinValues{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_SkinRGBColors{ 0, sizeof(GLfloat) * 3, TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_SkinPatterns{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// Creature-localized devices (feeders, shields, sensors)
extern CreaturesSSBOInfo creature_SpikeLocalAngles{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Spikes{ 0, sizeof(vec4), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

extern CreaturesSSBOInfo creature_FeederLocalAngles{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Feeders{ 0, sizeof(vec4), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

extern CreaturesSSBOInfo creature_ShieldLocalAngles{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_Shields{ 0, sizeof(vec4), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// General purpose data packets
extern CreaturesSSBOInfo creature_GeneralPurposeVec2{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_GeneralPurposeSecondVec2{ 0, sizeof(vec2), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_GeneralPurposeFloat{ 0, sizeof(GLfloat), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };
extern CreaturesSSBOInfo creature_GeneralPurposeUInt{ 0, sizeof(GLuint), TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS };

// Death and reproduction logging creature lists
extern CreaturesSSBOInfo creatureList_Vanishes{ 0, sizeof(GLuint), TECH_CREATURE_LIST_BUFFER_FLAGS, 1 };
extern CreaturesSSBOInfo creatureList_Newborns{ 0, sizeof(uvec2), TECH_CREATURE_LIST_BUFFER_FLAGS, 1 };


// Set dynamic sizes again in runtime - some settings values are not visible during definition time!
// @TODO: Find a better solution to this, we shouldn't need to redefine attribute sizes here
void ResetDynamicAttributeSizes()
{
	creature_BrainsLinks.unitByteSize = sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_LINKS;
	creature_BrainsNodes.unitByteSize = sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_NODES;
	creature_BrainsBiasesExponents.unitByteSize = sizeof(vec2) * CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES;
	creature_BrainsStructures.unitByteSize = sizeof(GLuint) * CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES;

	creature_CollidersIndicesAndPlacements.unitByteSize = sizeof(uvec2) * CREATURE_MAX_NUM_OF_COLLIDERS;
	creature_CollidersToPosDirs.unitByteSize = sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS;
	creature_CollidersPositions.unitByteSize = sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS;
	creature_CollidersRadii.unitByteSize = sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS;
	creature_CollidersGivenEnergy.unitByteSize = sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS;

	creature_EyePupilSights.unitByteSize = sizeof(GLfloat) * CREATURE_EYE_NUM_OF_PUPIL_VALUES;
	creature_EyeConeSights.unitByteSize = sizeof(GLfloat) * CREATURE_EYE_NUM_OF_CONES_VALUES;
}




// The sole purpose of these vectors is to contain our creature SSBO infos for easier iteration during SSBO manipulations
vector<CreaturesSSBOInfo*> creatureAttributesSSBOInfosRefs;
vector<CreaturesSSBOInfo*> creatureListsSSBOInfosRefs;

void LoadCreatureSSBOInfosIntoIterableVectors()
{
	// Load creature attribute SSBO infos
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsLinks);
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsNodes);
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsBiasesExponents);
	creatureAttributesSSBOInfosRefs.push_back(&creature_BrainsStructures);
	creatureAttributesSSBOInfosRefs.push_back(&creature_EyeMuscles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_EyePositions);
	creatureAttributesSSBOInfosRefs.push_back(&creature_EyeConeRadii);
	creatureAttributesSSBOInfosRefs.push_back(&creature_EyePupilConeCoverageFraction);
	creatureAttributesSSBOInfosRefs.push_back(&creature_EyePupilSights);
	creatureAttributesSSBOInfosRefs.push_back(&creature_EyeConeSights);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Positions);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Velocities);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Angles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_AngleVelocities);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ForwardDirections);
	creatureAttributesSSBOInfosRefs.push_back(&creature_RightDirections);
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
	creatureAttributesSSBOInfosRefs.push_back(&creature_GeneralPurposeVec2);
	creatureAttributesSSBOInfosRefs.push_back(&creature_GeneralPurposeSecondVec2);
	creatureAttributesSSBOInfosRefs.push_back(&creature_GeneralPurposeFloat);
	creatureAttributesSSBOInfosRefs.push_back(&creature_GeneralPurposeUInt);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SkinHues);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SkinSaturations);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SkinValues);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SkinRGBColors);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SkinPatterns);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Spikes);
	creatureAttributesSSBOInfosRefs.push_back(&creature_SpikeLocalAngles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Feeders);
	creatureAttributesSSBOInfosRefs.push_back(&creature_FeederLocalAngles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_Shields);
	creatureAttributesSSBOInfosRefs.push_back(&creature_ShieldLocalAngles);
	creatureAttributesSSBOInfosRefs.push_back(&creature_CollidersCounts);
	creatureAttributesSSBOInfosRefs.push_back(&creature_CollidersIndicesAndPlacements);
	creatureAttributesSSBOInfosRefs.push_back(&creature_CollidersToPosDirs);
	creatureAttributesSSBOInfosRefs.push_back(&creature_CollidersPositions);
	creatureAttributesSSBOInfosRefs.push_back(&creature_CollidersRadii);
	creatureAttributesSSBOInfosRefs.push_back(&creature_CollidersGivenEnergy);
	
	// Load creature list SSBO infos
	creatureListsSSBOInfosRefs.push_back(&creatureList_Vanishes);
	creatureListsSSBOInfosRefs.push_back(&creatureList_Newborns);
}


//////////////////////////////////////////////
// -- DEFAULT CREATURE ATTRIBUTES VALUES -- //
//////////////////////////////////////////////

// The main purpose of defining these values is to have something to write into creature SSBO units that don't
// accept CreatureData values

const vec2 defaultGeneralPurposeVec2 = vec2(0.0, 0.0);
const vec2 defaultGeneralPurposeSecondVec2 = vec2(0.0, 0.0);
const GLfloat defaultGeneralPurposeFloat = 0.0;
const GLuint defaultGeneralPurposeUInt = 0;

const uvec2 defaultCreatureListNewbornsCount = uvec2(0, 0);
const GLuint defaultCreatureListVanishesCount = 0;



///////////////////////////////
// -- CREATURE SSBO UTILS -- //
///////////////////////////////

void InitOrExpandCreatureSSBO(CreaturesSSBOInfo& creatureSSBOInfo, GLuint numOfCreaturesToSupport)
{

	// How many creatures should we support on initialization? This number can't be zero!
	GLuint unitsCountToSupport = numOfCreaturesToSupport;
	GLuint unitsCountToAllocate = unitsCountToSupport + creatureSSBOInfo.extraUnitsToAllocate;
	
	// Calculate buffer size
	GLuint bufferSize = unitsCountToAllocate * creatureSSBOInfo.unitByteSize;

	// Initialize with NULL data
	GLuint newSSBO;
	glGenBuffers(1, &newSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, bufferSize, NULL, creatureSSBOInfo.flags);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, NULL, TECH_SSBO_USAGE);

	// Does this SSBO info already have an old buffer? If so, copy its data into the new buffer and delete it
	if (creatureSSBOInfo.hasBuffer)
	{
		GLuint oldSSBO = creatureSSBOInfo.ssbo;

		// Acquire the size of the old buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, oldSSBO);
		GLint64 oldSize;
		glGetBufferParameteri64v(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &oldSize);

		// Copy contents
		glBindBuffer(GL_COPY_READ_BUFFER, oldSSBO);
		glBindBuffer(GL_COPY_WRITE_BUFFER, newSSBO);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, oldSize); // This causes a performance warning for some reason on NVIDIA drivers

		// Delete old SSBO
		GLuint buffersToDelete = { oldSSBO };
		glDeleteBuffers(1, &buffersToDelete);
	}

	// Finalize
	creatureSSBOInfo.ssbo = newSSBO;
	creatureSSBOInfo.hasBuffer = true;
}


///////////////////////////////////////////////
// -- CREATURE MANIPULATION COMFORT TOOLS -- //
///////////////////////////////////////////////

void SetCreatureAttribute(CreaturesSSBOInfo creatureSSBOInfo, GLuint creatureIndex, const void* data)
{
	GLuint writeOffset = creatureSSBOInfo.unitByteSize * creatureIndex;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creatureSSBOInfo.ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, writeOffset, creatureSSBOInfo.unitByteSize, data);
}


GLuint CreatureData_AddCreature(CreatureData newCreatureData)
{

	unsigned int newCreatureCount = creature_count + 1;

	// Check if we're exceeding capacity
	if (newCreatureCount > max_supported_creature_count_by_current_buffers)
	{
		// We're exceeding capacities for all of our SSBOs, expand them
		
		GLuint newNumOfCreaturesToSupport = max_supported_creature_count_by_current_buffers + TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH;
		
		// Expand attributes
		for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
		{
			InitOrExpandCreatureSSBO(*creatureAttributeSSBOInfoRef, newNumOfCreaturesToSupport);
		}

		// Expand lists (make room for them as well! it might just be that ALL creatures somehow manage to die in the same frame and need to be listed! :))
		for (auto creatureListSSBOInfoRef : creatureListsSSBOInfosRefs)
		{
			InitOrExpandCreatureSSBO(*creatureListSSBOInfoRef, newNumOfCreaturesToSupport);
		}

		max_supported_creature_count_by_current_buffers = newNumOfCreaturesToSupport;
	}

	// Create the new creature by simply setting its attributes

	GLuint newCreatureIndex = creature_count;

	SetCreatureAttribute(creature_BrainsLinks, newCreatureIndex, newCreatureData.brainLinks.data());
	SetCreatureAttribute(creature_BrainsNodes, newCreatureIndex, newCreatureData.brainNodes.data());
	SetCreatureAttribute(creature_BrainsBiasesExponents, newCreatureIndex, newCreatureData.brainBiasesExponents.data());
	SetCreatureAttribute(creature_BrainsStructures, newCreatureIndex, newCreatureData.brainStructure.data());
	SetCreatureAttribute(creature_SkinHues, newCreatureIndex, &newCreatureData.skinHue);
	SetCreatureAttribute(creature_SkinSaturations, newCreatureIndex, &newCreatureData.skinSaturation);
	SetCreatureAttribute(creature_SkinValues, newCreatureIndex, &newCreatureData.skinValue);
	SetCreatureAttribute(creature_Positions, newCreatureIndex, &newCreatureData.pos);
	SetCreatureAttribute(creature_Velocities, newCreatureIndex, &newCreatureData.vel);
	SetCreatureAttribute(creature_Radii, newCreatureIndex, &newCreatureData.rad);
	SetCreatureAttribute(creature_Lives, newCreatureIndex, &newCreatureData.life);
	SetCreatureAttribute(creature_Angles, newCreatureIndex, &newCreatureData.angle);
	SetCreatureAttribute(creature_AngleVelocities, newCreatureIndex, &newCreatureData.angleVel);
	SetCreatureAttribute(creature_ForwardDirections, newCreatureIndex, &newCreatureData.forwardDir);
	SetCreatureAttribute(creature_RightDirections, newCreatureIndex, &newCreatureData.rightDir);
	SetCreatureAttribute(creature_ForwardThrusts, newCreatureIndex, &newCreatureData.forwardThrust);
	SetCreatureAttribute(creature_TurnThrusts, newCreatureIndex, &newCreatureData.turnThrust);
	SetCreatureAttribute(creature_Harndesses, newCreatureIndex, &newCreatureData.hardness);
	SetCreatureAttribute(creature_SkinPatterns, newCreatureIndex, &newCreatureData.skinPattern);
	SetCreatureAttribute(creature_SpikeLocalAngles, newCreatureIndex, &newCreatureData.spikeLocalAngle);
	SetCreatureAttribute(creature_FeederLocalAngles, newCreatureIndex, &newCreatureData.feederLocalAngle);
	SetCreatureAttribute(creature_ShieldLocalAngles, newCreatureIndex, &newCreatureData.shieldLocalAngle);
	SetCreatureAttribute(creature_Spikes, newCreatureIndex, &newCreatureData.spike);
	SetCreatureAttribute(creature_Feeders, newCreatureIndex, &newCreatureData.feeder);
	SetCreatureAttribute(creature_Shields, newCreatureIndex, &newCreatureData.shield);
	SetCreatureAttribute(creature_EyeMuscles, newCreatureIndex, &newCreatureData.eyeMuscles);
	SetCreatureAttribute(creature_EyeConeRadii, newCreatureIndex, &newCreatureData.eyeConeRadius);
	SetCreatureAttribute(creature_EyePupilConeCoverageFraction, newCreatureIndex, &newCreatureData.eyePupilConeCoverageFraction);
	SetCreatureAttribute(creature_Energies, newCreatureIndex, &newCreatureData.energy);
	SetCreatureAttribute(creature_Meats, newCreatureIndex, &newCreatureData.meat);

	// Also set some zero values to avoid undefined values lurking in our buffers
	SetCreatureAttribute(creature_GeneralPurposeVec2, newCreatureIndex, &defaultGeneralPurposeVec2);
	SetCreatureAttribute(creature_GeneralPurposeSecondVec2, newCreatureIndex, &defaultGeneralPurposeSecondVec2);
	SetCreatureAttribute(creature_GeneralPurposeFloat, newCreatureIndex, &defaultGeneralPurposeFloat);
	SetCreatureAttribute(creature_GeneralPurposeUInt, newCreatureIndex, &defaultGeneralPurposeUInt);


	// Map creature unique ID to creature index
	creature_UniqueIDsToSSBOIndex.emplace(creature_NextUniqueIDToBeAssigned, newCreatureIndex);

	// Map creature index to unique ID
	// @TODO: assert creature_UniqueIDs[creature_count] == creature_NextUniqueIDToBeAssigned?
	creature_UniqueIDs.emplace_back(creature_NextUniqueIDToBeAssigned);
	creature_NextUniqueIDToBeAssigned++;
	
	creature_count = newCreatureCount;

	return newCreatureIndex;
}

void RemoveCreatureAttribute(CreaturesSSBOInfo creatureSSBOInfo, GLuint creatureIndex)
{
	GLuint lastCreatureIndex = creature_count - 1;

	if (lastCreatureIndex != creatureIndex)
	{
		// Copy the data in lastCreatureIndex to our creatureIndex, the rest is taken care of through uniform memory limits
		glBindBuffer(GL_COPY_READ_BUFFER, creatureSSBOInfo.ssbo);
		glBindBuffer(GL_COPY_WRITE_BUFFER, creatureSSBOInfo.ssbo);

		GLuint readOffset = lastCreatureIndex * creatureSSBOInfo.unitByteSize;
		GLuint writeOffset = creatureIndex * creatureSSBOInfo.unitByteSize;
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, creatureSSBOInfo.unitByteSize);
	}
}


void CreatureData_RemoveCreature(GLuint creatureIndex)
{
	if (creature_count <= 0)
		return;

	// Remove creature attributes
	for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
	{
		RemoveCreatureAttribute(*creatureAttributeSSBOInfoRef, creatureIndex);
	}

	// No need to manipulate creature lists!

	// Obtain the unique ID of our removed creature
	CreatureUniqueID uniqueIDOfRemovedCreature = creature_UniqueIDs[creatureIndex];

	// Let the last-place creature take its place
	unsigned int lastCreatureIndex = creature_count - 1;
	CreatureUniqueID uniqueIDOfLastIndexedCreature = creature_UniqueIDs[lastCreatureIndex];
	creature_UniqueIDs[creatureIndex] = uniqueIDOfLastIndexedCreature;
	creature_UniqueIDsToSSBOIndex[uniqueIDOfLastIndexedCreature] = creatureIndex;

	// Remove this creature from the map
	creature_UniqueIDsToSSBOIndex.erase(uniqueIDOfRemovedCreature);

	// Remove the duplicate entry for the last element
	creature_UniqueIDs.pop_back();

	creature_count--;
}

//////////////////////////
// -- INITIALIZATION -- //
//////////////////////////

void CreatureData_Init()
{

	// Make sure our attribute sizes are set correctly - some settings values are not visible in compile time!
	ResetDynamicAttributeSizes();

	// Load all creature SSBO infos into a vector for easy iterations
	LoadCreatureSSBOInfosIntoIterableVectors();

	// How many creatures should we support on init?
	unsigned int numOfCreaturesOnInit = SIMULATION_NUM_OF_CREATURES_ON_INIT.value;
	unsigned int numOfCreaturesToSupportOnInit = std::max(numOfCreaturesOnInit, TECH_MIN_CREATURE_ATTRIBUTE_SSBO_CREATURES_SUPPORTED);

	// Initialize buffers
	for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
	{
		// Just in case boolean default values aren't false for some reason
		creatureAttributeSSBOInfoRef->hasBuffer = false;

		// Init buffer
		InitOrExpandCreatureSSBO(*creatureAttributeSSBOInfoRef, numOfCreaturesToSupportOnInit);
	}

	creature_count = 0;
	max_supported_creature_count_by_current_buffers = numOfCreaturesToSupportOnInit;
	
	// Set creature lists' counts to zero
	SetCreatureAttribute(creatureList_Newborns, 0, &defaultCreatureListNewbornsCount);
	SetCreatureAttribute(creatureList_Vanishes, 0, &defaultCreatureListVanishesCount);

	// Lastly pre-allocate memory for our unique IDs vector
	creature_UniqueIDs.reserve(numOfCreaturesToSupportOnInit);
}