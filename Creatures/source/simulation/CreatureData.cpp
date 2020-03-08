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
extern CreatureAttributesSSBOInfo creature_BrainsLinks{ 0, sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_LINKS };
extern CreatureAttributesSSBOInfo creature_BrainsNodes{ 0, sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_NODES };
extern CreatureAttributesSSBOInfo creature_BrainsBiasesExponents{ 0, sizeof(vec2) * CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES };
extern CreatureAttributesSSBOInfo creature_BrainsStructures{ 0, sizeof(GLuint) * CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES };

// Eyes
extern CreatureAttributesSSBOInfo creature_EyeMuscles{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_EyePositions{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_EyeConeRadii{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_EyePupilConeCoverageFraction{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_EyePupilSights{ 0, sizeof(GLfloat) * CREATURE_EYE_NUM_OF_PUPIL_VALUES };
extern CreatureAttributesSSBOInfo creature_EyeConeSights{ 0, sizeof(GLfloat) * CREATURE_EYE_NUM_OF_CONES_VALUES };

// Physics
extern CreatureAttributesSSBOInfo creature_Positions{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_Velocities{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_Angles{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_AngleVelocities{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_ForwardDirections{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_RightDirections{ 0, sizeof(vec2) };

// Movement
extern CreatureAttributesSSBOInfo creature_ForwardThrusts{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_TurnThrusts{ 0, sizeof(GLfloat) };

// Body parameters
extern CreatureAttributesSSBOInfo creature_Radii{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_Lives{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_Energies{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_Meats{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_Harndesses{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_Horninesses{ 0, sizeof(GLfloat) };

// Creature misc
extern CreatureAttributesSSBOInfo creature_Generations{ 0, sizeof(GLuint) };
extern CreatureAttributesSSBOInfo creature_UniformGridTiles{ 0, sizeof(GLint) };

// Colliders
extern CreatureAttributesSSBOInfo creature_CollidersCounts{ 0, sizeof(GLuint) };
extern CreatureAttributesSSBOInfo creature_CollidersIndicesAndPlacements{ 0, sizeof(uvec2) * CREATURE_MAX_NUM_OF_COLLIDERS };
extern CreatureAttributesSSBOInfo creature_CollidersToPosDirs{ 0, sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS };
extern CreatureAttributesSSBOInfo creature_CollidersPositions{ 0, sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS };
extern CreatureAttributesSSBOInfo creature_CollidersRadii{ 0, sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS };
extern CreatureAttributesSSBOInfo creature_CollidersGivenEnergy{ 0, sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS };


// General purpose data packets
extern CreatureAttributesSSBOInfo creature_GeneralPurposeVec2{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_GeneralPurposeSecondVec2{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_GeneralPurposeFloat{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_GeneralPurposeUInt{ 0, sizeof(GLuint) };

// Appearances
extern CreatureAttributesSSBOInfo creature_SkinHues{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_SkinSaturations{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_SkinValues{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_SkinRGBColors{ 0, sizeof(GLfloat) * 3 };
extern CreatureAttributesSSBOInfo creature_SkinPatterns{ 0, sizeof(vec2) };

// Creature-localized devices (feeders, shields, sensors)
extern CreatureAttributesSSBOInfo creature_SpikeLocalAngles{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_Spikes{ 0, sizeof(vec4) };

extern CreatureAttributesSSBOInfo creature_FeederLocalAngles{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_Feeders{ 0, sizeof(vec4) };

extern CreatureAttributesSSBOInfo creature_ShieldLocalAngles{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_Shields{ 0, sizeof(vec4) };



// Set dynamic sizes again in runtime - some settings values are not visible during definition time!
// @TODO: Find a better solution to this, we shouldn't need to redefine attribute sizes here
void ResetDynamicAttributeSizes()
{
	creature_BrainsLinks.attributeBytesSize = sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_LINKS;
	creature_BrainsNodes.attributeBytesSize = sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_NODES;
	creature_BrainsBiasesExponents.attributeBytesSize = sizeof(vec2) * CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES;
	creature_BrainsStructures.attributeBytesSize = sizeof(GLuint) * CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES;

	creature_CollidersIndicesAndPlacements.attributeBytesSize = sizeof(uvec2) * CREATURE_MAX_NUM_OF_COLLIDERS;
	creature_CollidersToPosDirs.attributeBytesSize = sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS;
	creature_CollidersPositions.attributeBytesSize = sizeof(vec2) * CREATURE_MAX_NUM_OF_COLLIDERS;
	creature_CollidersRadii.attributeBytesSize = sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS;
	creature_CollidersGivenEnergy.attributeBytesSize = sizeof(GLfloat) * CREATURE_MAX_NUM_OF_COLLIDERS;

	creature_EyePupilSights.attributeBytesSize = sizeof(GLfloat) * CREATURE_EYE_NUM_OF_PUPIL_VALUES;
	creature_EyeConeSights.attributeBytesSize = sizeof(GLfloat) * CREATURE_EYE_NUM_OF_CONES_VALUES;
}




// The sole purpose of this vector is to contain our creature attributes SSBO infos for easier iteration during SSBO manipulations
vector<CreatureAttributesSSBOInfo*> creatureAttributesSSBOInfosRefs;
void LoadCreatureAttributeSSBOInfosIntoIterableVector()
{
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
}




///////////////////////////////
// -- CREATURE SSBO UTILS -- //
///////////////////////////////


void InitEmptyCreatureAttributesSSBO(CreatureAttributesSSBOInfo& attributes, GLuint startingAttributesCount)
{
	// Create a new SSBO
	GLuint newSSBO;
	glGenBuffers(1, &newSSBO);

	// Initialize with NULL data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, newSSBO);
	GLuint size = startingAttributesCount * attributes.attributeBytesSize;
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, TECH_SSBO_USAGE);

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
	glBufferData(GL_SHADER_STORAGE_BUFFER, newSize, NULL, TECH_SSBO_USAGE);

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


GLuint CreatureData_AddCreature(CreatureData newCreatureData)
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
	SetCreatureAttribute(creature_SkinHues, newCreatureIndex, &newCreatureData.skinHue);
	SetCreatureAttribute(creature_SkinSaturations, newCreatureIndex, &newCreatureData.skinSaturation);
	SetCreatureAttribute(creature_SkinValues, newCreatureIndex, &newCreatureData.skinValue);
	SetCreatureAttribute(creature_Positions, newCreatureIndex, &newCreatureData.pos);
	SetCreatureAttribute(creature_Velocities, newCreatureIndex, &newCreatureData.vel);
	SetCreatureAttribute(creature_GeneralPurposeVec2, newCreatureIndex, NULL);
	SetCreatureAttribute(creature_Radii, newCreatureIndex, &newCreatureData.rad);
	SetCreatureAttribute(creature_Lives, newCreatureIndex, &newCreatureData.life);
	SetCreatureAttribute(creature_Angles, newCreatureIndex, &newCreatureData.angle);
	SetCreatureAttribute(creature_AngleVelocities, newCreatureIndex, &newCreatureData.angleVel);
	SetCreatureAttribute(creature_ForwardDirections, newCreatureIndex, &newCreatureData.forwardDir);
	SetCreatureAttribute(creature_RightDirections, newCreatureIndex, &newCreatureData.rightDir);
	SetCreatureAttribute(creature_ForwardThrusts, newCreatureIndex, &newCreatureData.forwardThrust);
	SetCreatureAttribute(creature_TurnThrusts, newCreatureIndex, &newCreatureData.turnThrust);
	SetCreatureAttribute(creature_Harndesses, newCreatureIndex, &newCreatureData.hardness);
	SetCreatureAttribute(creature_UniformGridTiles, newCreatureIndex, NULL);
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

	// Map creature unique ID to creature index
	creature_UniqueIDsToSSBOIndex.emplace(creature_NextUniqueIDToBeAssigned, newCreatureIndex);

	// Map creature index to unique ID
	// @TODO: assert creature_UniqueIDs[creature_count] == creature_NextUniqueIDToBeAssigned?
	creature_UniqueIDs.emplace_back(creature_NextUniqueIDToBeAssigned);
	creature_NextUniqueIDToBeAssigned++;
	
	creature_count++;

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


void CreatureData_RemoveCreature(GLuint creatureIndex)
{
	if (creature_count <= 0)
		return;

	for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
	{
		RemoveCreatureAttribute(*creatureAttributeSSBOInfoRef, creatureIndex);
	}

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

	// Before anything, load all creature attribute SSBO infos into a vector for easy iterations
	LoadCreatureAttributeSSBOInfosIntoIterableVector();

	const unsigned int numOfCreaturesOnInit = SIMULATION_NUM_OF_CREATURES_ON_INIT.value;

	creature_count = 0;
	max_supported_creature_count_by_current_buffers = numOfCreaturesOnInit;

	for (auto creatureAttributeSSBOInfoRef : creatureAttributesSSBOInfosRefs)
	{
		InitEmptyCreatureAttributesSSBO(*creatureAttributeSSBOInfoRef, numOfCreaturesOnInit);
	}

	// Lastly pre-allocate memory for our unique IDs vector
	creature_UniqueIDs.reserve(numOfCreaturesOnInit);
}