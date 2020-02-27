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
 
// Physics
extern CreatureAttributesSSBOInfo creature_Positions{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_Velocities{ 0, sizeof(vec2) };
extern CreatureAttributesSSBOInfo creature_Angles{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_AngleVelocities{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_ForwardDirections{ 0, sizeof(vec2) };
 
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
extern CreatureAttributesSSBOInfo creature_GeneralPurpose{ 0, sizeof(vec2) };

// Appearances
extern CreatureAttributesSSBOInfo creature_Colors{ 0, sizeof(vec3) };
extern CreatureAttributesSSBOInfo creature_SkinPatterns{ 0, sizeof(vec2) };

// Creature-localized devices (feeders, shields, sensors)
extern CreatureAttributesSSBOInfo creature_SpikeStates{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_SpikeLocalAngles{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_SpikeDirections{ 0, sizeof(vec2) };

extern CreatureAttributesSSBOInfo creature_FeederStates{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_FeederLocalAngles{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_FeederDirections{ 0, sizeof(vec2) };

extern CreatureAttributesSSBOInfo creature_ShieldStates{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_ShieldLocalAngles{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_ShieldSpans{ 0, sizeof(GLfloat) };
extern CreatureAttributesSSBOInfo creature_ShieldDirections{ 0, sizeof(vec2) };

// @TODO: actually implement this placeholder
//extern CreatureAttributesSSBOInfo creature_SensorsStates{ 0, sizeof(GLfloat) };
//extern CreatureAttributesSSBOInfo creature_SensorsLocalAngles{ 0, sizeof(GLfloat) };
//extern reatureAttributesSSBOInfo creature_SensorsDirections{ 0, sizeof(vec2) };

// Deformations
extern CreatureAttributesSSBOInfo creature_DeformerPositions{ 0, sizeof(vec2) * CREATURE_MAX_NUM_OF_DEFORMERS };
extern CreatureAttributesSSBOInfo creature_DeformerRadii{ 0, sizeof(GLfloat) * CREATURE_MAX_NUM_OF_DEFORMERS };
extern CreatureAttributesSSBOInfo creature_DeformerCounts{ 0, sizeof(GLuint) };




// Set dynamic sizes again in runtime - some settings values are not visible during definition time!
// @TODO: Find a better solution to this, we shouldn't need to redefine attribute sizes here
void ResetDynamicAttributeSizes()
{
	creature_BrainsLinks.attributeBytesSize = sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_LINKS;
	creature_BrainsNodes.attributeBytesSize = sizeof(GLfloat) * CREATURE_BRAIN_MAX_NUM_OF_NODES;
	creature_BrainsBiasesExponents.attributeBytesSize = sizeof(vec2) * CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES;
	creature_BrainsStructures.attributeBytesSize = sizeof(GLuint) * CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES;

	creature_DeformerPositions.attributeBytesSize = sizeof(vec2) * CREATURE_MAX_NUM_OF_DEFORMERS;
	creature_DeformerRadii.attributeBytesSize = sizeof(GLfloat) * CREATURE_MAX_NUM_OF_DEFORMERS;
}




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