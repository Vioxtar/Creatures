#include "CreatureData.h"


CreatureUniqueID creature_NextUniqueIDToBeAssigned = 0;
map<CreatureUniqueID, GLuint> creature_UniqueIDsToSSBOIndex;
vector<CreatureUniqueID> creature_UniqueIDs;

GLuint CreatureData_CreatureUniqueIDToSSBOIndex(CreatureUniqueID creatureID)
{
	return creature_UniqueIDsToSSBOIndex.at(creatureID);
}

CreatureUniqueID CreatureData_CreatureSSBOIndexToUniqueID(GLuint ssboIndex)
{
	// @TODO: Check that ssbo index < creature_count
	return creature_UniqueIDs[ssboIndex];
}

extern GLuint creature_count = 0; // The count of active creatures in the simulation
extern GLuint max_supported_creature_count_by_current_buffers = 0; // The number of creatures supported by current SSBO buffers


//////////////////////////
// -- CREATURE SSBOS -- //
//////////////////////////


// Brains
extern CreaturesSSBOInfo creature_BrainsLinks{ sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_LINKS };
extern CreaturesSSBOInfo creature_BrainsNodes{ sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_NODES };
extern CreaturesSSBOInfo creature_BrainsBiasesExponents{ sizeof(vec2) * CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES };
extern CreaturesSSBOInfo creature_BrainsStructures{ sizeof(GLuint) * CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES };

// Eyes
extern CreaturesSSBOInfo creature_EyeMuscles{ sizeof(vec2) };
extern CreaturesSSBOInfo creature_EyePositions{ sizeof(vec2) };
extern CreaturesSSBOInfo creature_EyeConeRadii{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_EyePupilConeCoverageFraction{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_EyePupilSights{ sizeof(GLfloat) * CREATURE_EYE_NUM_OF_PUPIL_VALUES };
extern CreaturesSSBOInfo creature_EyeConeSights{ sizeof(GLfloat) * CREATURE_EYE_NUM_OF_CONES_VALUES };

// Physics
extern CreaturesSSBOInfo creature_Positions{ sizeof(vec2) };
extern CreaturesSSBOInfo creature_Velocities{ sizeof(vec2) };
extern CreaturesSSBOInfo creature_Angles{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_AngleVelocities{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_ForwardDirections{ sizeof(vec2) };
extern CreaturesSSBOInfo creature_RightDirections{ sizeof(vec2) };

// Movement
extern CreaturesSSBOInfo creature_ForwardThrusts{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_TurnThrusts{ sizeof(GLfloat) };

// Body parameters
extern CreaturesSSBOInfo creature_Radii{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_Lives{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_Energies{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_Meats{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_Harndesses{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_Horninesses{ sizeof(GLfloat) };

// Creature misc
extern CreaturesSSBOInfo creature_Generations{ sizeof(GLuint) };
extern CreaturesSSBOInfo creature_UniformGridTiles{ sizeof(GLint) };

// Colliders
extern CreaturesSSBOInfo creature_CollidersCounts{ sizeof(GLuint) };
extern CreaturesSSBOInfo creature_CollidersIndicesAndPlacements{ sizeof(uvec2) * CREATURE_MAX_NUM_OF_COLLIDERS };
extern CreaturesSSBOInfo creature_CollidersToPosDirs{ sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS };
extern CreaturesSSBOInfo creature_CollidersPositions{ sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS };
extern CreaturesSSBOInfo creature_CollidersRadii{ sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS };
extern CreaturesSSBOInfo creature_CollidersGivenEnergy{ sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS };


// Appearances
extern CreaturesSSBOInfo creature_SkinHues{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_SkinSaturations{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_SkinValues{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_SkinRGBColors{ sizeof(GLfloat) * 3 };
extern CreaturesSSBOInfo creature_SkinPatterns{ sizeof(vec2) };

// Creature-localized devices (feeders, shields, sensors)
extern CreaturesSSBOInfo creature_SpikeLocalAngles{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_Spikes{ sizeof(vec4) };

extern CreaturesSSBOInfo creature_FeederLocalAngles{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_Feeders{ sizeof(vec4) };

extern CreaturesSSBOInfo creature_ShieldLocalAngles{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_Shields{ sizeof(vec4) };

// General purpose data packets
extern CreaturesSSBOInfo creature_GeneralPurposeVec2{ sizeof(vec2) };
extern CreaturesSSBOInfo creature_GeneralPurposeSecondVec2{ sizeof(vec2) };
extern CreaturesSSBOInfo creature_GeneralPurposeFloat{ sizeof(GLfloat) };
extern CreaturesSSBOInfo creature_GeneralPurposeUInt{ sizeof(GLuint) };

// Death and reproduction logging creature lists
extern MappedCreaturesSSBOInfo creatureList_Vanishes{ { sizeof(GLuint) }, 1 };
extern MappedCreaturesSSBOInfo creatureList_NewBorns{ { sizeof(uvec2) }, 1 };




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
vector<MappedCreaturesSSBOInfo*> creatureListsRefs;

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
	creatureListsRefs.push_back(&creatureList_Vanishes);
	creatureListsRefs.push_back(&creatureList_NewBorns);
}


//////////////////////////////////////////////
// -- DEFAULT CREATURE ATTRIBUTES VALUES -- //
//////////////////////////////////////////////

// The main purpose of defining these values is to have something to write into creature SSBO units that don't
// accept CreatureData values

const vec2 defaultVec2 = vec2(0.0, 0.0);
const uvec2 defaultUVec2 = uvec2(0, 0);
const GLuint defaultUInt = 0;
const GLfloat defaultFloat = 0.0;
const GLbyte defaultZeroByte = 0;


/////////////////////////////
// -- SINGLE SSBO UTILS -- //
/////////////////////////////

void MapPersistentBuffer(MappedCreaturesSSBOInfo& creatureList)
{
	GLint64 bufferSize;
	glGetNamedBufferParameteri64v(creatureList.creaturesSSBOInfo.bufferHandle, GL_BUFFER_SIZE, &bufferSize);
	creatureList.mapPtr = glMapNamedBufferRange(creatureList.creaturesSSBOInfo.bufferHandle, 0, bufferSize, creatureList.mappingFlags);
}

void UnMapPersistentBuffer(MappedCreaturesSSBOInfo& creatureList)
{
	glUnmapNamedBuffer(creatureList.creaturesSSBOInfo.bufferHandle);
}

void InitOrExpandCreatureSSBO(CreaturesSSBOInfo& creatureSSBOInfo, GLuint numOfCreaturesToSupport)
{	
	// Calculate buffer size
	GLuint bufferSize = numOfCreaturesToSupport * creatureSSBOInfo.unitByteSize;

	// Initialize with NULL data
	GLuint newSSBO;
	glCreateBuffers(1, &newSSBO);
	
	// Immutable
	glNamedBufferStorage(newSSBO, bufferSize, NULL, creatureSSBOInfo.bufferFlags);
	
	// Mutable
	//glNamedBufferData(newSSBO, bufferSize, NULL, TECH_SSBO_USAGE);

	// Does this SSBO info already have an old buffer? If so, copy its data into the new buffer and delete it
	if (creatureSSBOInfo.hasBuffer)
	{
		GLuint oldSSBO = creatureSSBOInfo.bufferHandle;

		// Acquire the size of the old buffer
		GLint64 oldSize;
		glGetNamedBufferParameteri64v(oldSSBO, GL_BUFFER_SIZE, &oldSize);

		// Copy contents
		glCopyNamedBufferSubData(oldSSBO, newSSBO, 0, 0, oldSize); // This may cause a performance warning for some reason on NVIDIA drivers

		// Delete old SSBO
		GLuint buffersToDelete = { oldSSBO };
		glDeleteBuffers(1, &buffersToDelete);
	}

	// Finalize
	creatureSSBOInfo.bufferHandle = newSSBO;
	creatureSSBOInfo.hasBuffer = true;
}



//////////////////////////////////
// -- SSBOs INIT & EXPANSION -- //
//////////////////////////////////

void ExpandAllSSBOs()
{
	max_supported_creature_count_by_current_buffers += TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH;

	// Expand attributes
	for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
	{
		InitOrExpandCreatureSSBO(*creatureAttributeSSBOInfoRef, max_supported_creature_count_by_current_buffers);
	}

	// Expand lists (make room for them as well! it might just be that ALL creatures somehow manage to die in the same frame and need to be listed! :))
	for (auto creatureListRef : creatureListsRefs)
	{
		UnMapPersistentBuffer(*creatureListRef);
		InitOrExpandCreatureSSBO(creatureListRef->creaturesSSBOInfo, max_supported_creature_count_by_current_buffers + creatureListRef->extraUnitsToAllocate);
		MapPersistentBuffer(*creatureListRef);
	}
}

void InitAllSSBOs()
{

	// Initialize attributes
	for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
	{
		creatureAttributeSSBOInfoRef->hasBuffer = false;
		creatureAttributeSSBOInfoRef->bufferFlags = TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS;

		InitOrExpandCreatureSSBO(*creatureAttributeSSBOInfoRef, max_supported_creature_count_by_current_buffers);
	}

	// Initialize lists
	for (auto creatureListRef : creatureListsRefs)
	{
		creatureListRef->creaturesSSBOInfo.hasBuffer = false;
		creatureListRef->creaturesSSBOInfo.bufferFlags = TECH_CREATURE_LIST_BUFFER_FLAGS;
		creatureListRef->mappingFlags = TECH_CREATURE_LIST_MAPPING_FLAGS;

		InitOrExpandCreatureSSBO(creatureListRef->creaturesSSBOInfo, max_supported_creature_count_by_current_buffers + creatureListRef->extraUnitsToAllocate);

		MapPersistentBuffer(*creatureListRef);
	}
}


///////////////////////////////////////////////
// -- CREATURE MANIPULATION COMFORT TOOLS -- //
///////////////////////////////////////////////

void SetCreatureAttribute(CreaturesSSBOInfo creatureSSBOInfo, GLuint creatureIndex, const void* data)
{
	GLuint writeOffset = creatureSSBOInfo.unitByteSize * creatureIndex;
	glNamedBufferSubData(creatureSSBOInfo.bufferHandle, writeOffset, creatureSSBOInfo.unitByteSize, data);
}


CreatureUniqueID CreatureData_AddCreature(CreatureData newCreatureData)
{

	unsigned int newCreatureCount = creature_count + 1;

	// Check if we're exceeding capacity
	if (newCreatureCount > max_supported_creature_count_by_current_buffers)
	{
		// We're exceeding capacities for all of our SSBOs, expand them
		ExpandAllSSBOs();
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
	SetCreatureAttribute(creature_GeneralPurposeVec2, newCreatureIndex, &defaultVec2);
	SetCreatureAttribute(creature_GeneralPurposeSecondVec2, newCreatureIndex, &defaultVec2);
	SetCreatureAttribute(creature_GeneralPurposeFloat, newCreatureIndex, &defaultFloat);
	SetCreatureAttribute(creature_GeneralPurposeUInt, newCreatureIndex, &defaultUInt);

	// Set new creature unique ID
	CreatureUniqueID creatureUniqueID = creature_NextUniqueIDToBeAssigned;
	creature_NextUniqueIDToBeAssigned++;
	
	// Map creature unique ID to creature index
	creature_UniqueIDsToSSBOIndex.emplace(creatureUniqueID, newCreatureIndex);

	// Map creature index to unique ID
	creature_UniqueIDs.emplace_back(creatureUniqueID);
	
	// Increase count by one
	creature_count = newCreatureCount;

	// Return unique ID
	return creatureUniqueID;
}

void RemoveCreatureAttribute(CreaturesSSBOInfo creatureSSBOInfo, GLuint creatureIndex)
{
	GLuint lastCreatureIndex = creature_count - 1;

	if (lastCreatureIndex != creatureIndex)
	{
		// Copy the data in lastCreatureIndex to our creatureIndex, the rest is taken care of through uniform memory limits
		GLuint readOffset = lastCreatureIndex * creatureSSBOInfo.unitByteSize;
		GLuint writeOffset = creatureIndex * creatureSSBOInfo.unitByteSize;
		glCopyNamedBufferSubData(creatureSSBOInfo.bufferHandle, creatureSSBOInfo.bufferHandle, readOffset, writeOffset, creatureSSBOInfo.unitByteSize);
	}
}


void RemoveCreatureBySSBOIndex(GLuint creatureIndex)
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

void CreatureData_RemoveCreatureByUniqueID(CreatureUniqueID creatureID)
{
	RemoveCreatureBySSBOIndex(CreatureData_CreatureUniqueIDToSSBOIndex(creatureID));
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
	max_supported_creature_count_by_current_buffers = std::max(numOfCreaturesOnInit, TECH_CREATURES_SUPPORTED_ON_INIT_BUFFER_PREALLOCATION);

	creature_count = 0;

	InitAllSSBOs();
	
	// Set creature lists' counts to zero
	SetCreatureAttribute(creatureList_NewBorns.creaturesSSBOInfo, 0, &defaultUVec2);
	SetCreatureAttribute(creatureList_Vanishes.creaturesSSBOInfo, 0, &defaultUInt);

	// Lastly pre-allocate memory for our unique IDs vector
	creature_UniqueIDs.reserve(max_supported_creature_count_by_current_buffers);
}