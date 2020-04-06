#pragma once

#include "../Settings.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <exception>

using namespace glm;

struct CreatureData
{
	GLuint generation;
	GLuint offspringCount;

	// Brain
	vector<GLfloat> brainLinks;
	vector<GLfloat> brainNodes;
	vector<vec2> brainBiasesExponents;
	vector<GLuint> brainStructure;

	// Sensor input data
	vector<GLfloat> eyeConeSights;
	vector<GLfloat> eyePupilSights;

	// Devices
	vec4 spike;
	vec4 feeder;
	vec4 shield;
	mat4 adhesion;

	// Appearance
	vec2 skinPattern;
	GLfloat skinHue;
	GLfloat skinSaturation;
	GLfloat skinLightness;

	// Placement
	vec2 pos;
	vec2 vel;
	GLfloat angle;
	GLfloat angleVel;
	vec2 forwardDir;
	vec2 rightDir;

	// Body state
	GLfloat hardness;
	GLfloat rad;

	// Body parameters
	GLfloat life;
	GLfloat energy;
	GLfloat meat;
	
	// Eyes
	vec2 eyeMuscles;
	GLfloat eyeConeRadius;
	GLfloat eyePupilConeCoverageFraction;
	vec2 eyePos;

	// Device placements
	GLfloat spikeLocalAngle;
	GLfloat feederLocalAngle;
	GLfloat shieldLocalAngle;

};


////////////////
// -- MISC -- //
////////////////

typedef unsigned long long int CreatureUniqueID;

CreatureUniqueID CreatureData_AddCreature(CreatureData& newCreatureData);
void CreatureData_RemoveCreatureByUniqueID(CreatureUniqueID creatureID);
void CreatureData_Init();


extern GLuint creature_count; // The count of active creatures in the simulation
extern GLuint max_supported_creature_count_by_current_buffers; // The number of creatures supported by current SSBO buffers

GLuint CreatureData_CreatureUniqueIDToSSBOIndex(CreatureUniqueID creatureUniqueID);
CreatureUniqueID CreatureData_CreatureSSBOIndexToUniqueID(GLuint ssboIndex);

//////////////////////////
// -- CREATURE SSBOS -- //
//////////////////////////

struct CreaturesSSBOInfo
{
	GLuint unitByteSize;
	GLuint bufferHandle;

	GLbitfield bufferFlags;

	bool hasBuffer;
};

struct MappedCreaturesSSBOInfo
{
	CreaturesSSBOInfo creaturesSSBOInfo;
	GLuint extraUnitsToAllocate;

	GLbitfield mappingFlags;
	void* mapPtr;
};

void GetCreatureAttributeBySSBOIndex(CreaturesSSBOInfo creatureSSBOInfo, GLuint creatureSSBOIndex, void* data);
void GetCreatureAttributeByUniqueID(CreaturesSSBOInfo creatureSSBOInfo, CreatureUniqueID creatureID, void* data);
void GetCreatureAttributes(CreaturesSSBOInfo creatureSSBOInfo, void* data);
void SetCreatureAttribute(CreaturesSSBOInfo creatureSSBOInfo, GLuint creatureIndex, const void* data);

// Brains
extern CreaturesSSBOInfo creature_BrainsLinks;
extern CreaturesSSBOInfo creature_BrainsNodes;
extern CreaturesSSBOInfo creature_BrainsBiasesExponents;
extern CreaturesSSBOInfo creature_BrainsStructures;

// Eyes
extern CreaturesSSBOInfo creature_EyeMuscles;
extern CreaturesSSBOInfo creature_EyePositions;
extern CreaturesSSBOInfo creature_EyeConeRadii;
extern CreaturesSSBOInfo creature_EyePupilSights;
extern CreaturesSSBOInfo creature_EyePupilTargetCreatureIndices;
extern CreaturesSSBOInfo creature_EyeConeSights;

// Physics
extern CreaturesSSBOInfo creature_Positions;
extern CreaturesSSBOInfo creature_Velocities;
extern CreaturesSSBOInfo creature_Angles;
extern CreaturesSSBOInfo creature_AngleVelocities;
extern CreaturesSSBOInfo creature_ForwardDirections;
extern CreaturesSSBOInfo creature_RightDirections;

// Body parameters
extern CreaturesSSBOInfo creature_Radii;
extern CreaturesSSBOInfo creature_Lives;
extern CreaturesSSBOInfo creature_Energies;
extern CreaturesSSBOInfo creature_Meats;
extern CreaturesSSBOInfo creature_Harndesses;
extern CreaturesSSBOInfo creature_Horninesses;


// Creature misc
extern CreaturesSSBOInfo creature_Generations;
extern CreaturesSSBOInfo creature_OffspringCounts;
extern CreaturesSSBOInfo creature_UniformGridTiles;

// Colliders
extern CreaturesSSBOInfo creature_CollidersCounts;
extern CreaturesSSBOInfo creature_CollidersIndicesAndPlacements;
extern CreaturesSSBOInfo creature_CollidersToPosDirs;
extern CreaturesSSBOInfo creature_CollidersPositions;
extern CreaturesSSBOInfo creature_CollidersRadii;
extern CreaturesSSBOInfo creature_CollidersGivenEnergy;

// Appearances
extern CreaturesSSBOInfo creature_SkinHues;
extern CreaturesSSBOInfo creature_SkinSaturations;
extern CreaturesSSBOInfo creature_SkinValues;
extern CreaturesSSBOInfo creature_SkinRGBColors;
extern CreaturesSSBOInfo creature_SkinPatterns;

// Creature-localized devices (feeders, shields, sensors)
extern CreaturesSSBOInfo creature_SpikeLocalAngles;
extern CreaturesSSBOInfo creature_Spikes;

extern CreaturesSSBOInfo creature_FeederLocalAngles;
extern CreaturesSSBOInfo creature_Feeders;

extern CreaturesSSBOInfo creature_ShieldLocalAngles;
extern CreaturesSSBOInfo creature_Shields;

extern CreaturesSSBOInfo creature_Adhesions;


// General purpose data packets
extern CreaturesSSBOInfo creature_GeneralPurposeVec2;
extern CreaturesSSBOInfo creature_GeneralPurposeSecondVec2;
extern CreaturesSSBOInfo creature_GeneralPurposeFloat;
extern CreaturesSSBOInfo creature_GeneralPurposeUInt;

// Death and reproduction logging creature lists
extern MappedCreaturesSSBOInfo creatureList_Vanishes;
extern MappedCreaturesSSBOInfo creatureList_NewBorns;