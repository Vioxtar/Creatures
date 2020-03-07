#pragma once

#include "../Settings.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <exception>

using namespace glm;

struct CreatureData
{
	// Brain
	vector<GLfloat> brainLinks;
	vector<GLfloat> brainNodes;
	vector<vec2> brainBiasesExponents;
	vector<GLuint> brainStructure;

	// Sensor input data
	vector<GLfloat> eyeConeSights;
	vector<GLfloat> eyePupilSights;
	
	// Appearance
	vec2 skinPattern;
	GLfloat skinHue;
	GLfloat skinSaturation;
	GLfloat skinValue;
	vec3 skinRGBColors;

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

	// Devices
	vec4 spike;
	vec4 feeder;
	vec4 shield;

	// Movement
	GLfloat forwardThrust;
	GLfloat turnThrust;
	
	// Eyes
	vec2 eyeMuscles;
	GLfloat eyeConeRadius;
	GLfloat eyePupilConeCoverageFraction;
	vec2 eyePos;

	// Device placements
	GLfloat spikeLocalAngle;
	GLfloat feederLocalAngle;
	GLfloat shieldLocalAngle;
	GLfloat shieldSpan;
};


GLuint CreatureData_AddCreature(CreatureData newCreatureData);

void CreatureData_RemoveCreature(GLuint creatureIndex);

void CreatureData_Init();


extern GLuint creature_count; // The count of active creatures in the simulation
extern GLuint max_supported_creature_count_by_current_buffers; // The number of creatures supported by current SSBO buffers

typedef unsigned long long int CreatureUniqueID;

GLuint CreatureUniqueIDToSSBOIndex(CreatureUniqueID creatureUniqueID);
CreatureUniqueID CreatureSSBOIndexToUniqueID(GLuint ssboIndex);

////////////////////////////////////
// -- CREATURE ATTRIBUTE SSBOS -- //
////////////////////////////////////


struct CreatureAttributesSSBOInfo
{
	GLuint ssbo;
	GLuint attributeBytesSize;
};


// Brains
extern CreatureAttributesSSBOInfo creature_BrainsLinks;
extern CreatureAttributesSSBOInfo creature_BrainsNodes;
extern CreatureAttributesSSBOInfo creature_BrainsBiasesExponents;
extern CreatureAttributesSSBOInfo creature_BrainsStructures;

// Eyes
extern CreatureAttributesSSBOInfo creature_EyeMuscles;
extern CreatureAttributesSSBOInfo creature_EyePositions;
extern CreatureAttributesSSBOInfo creature_EyeConeRadii;
extern CreatureAttributesSSBOInfo creature_EyePupilConeCoverageFraction;
extern CreatureAttributesSSBOInfo creature_EyePupilSights;
extern CreatureAttributesSSBOInfo creature_EyePupilTargetCreatureIndices;
extern CreatureAttributesSSBOInfo creature_EyeConeSights;

// Physics
extern CreatureAttributesSSBOInfo creature_Positions;
extern CreatureAttributesSSBOInfo creature_Velocities;
extern CreatureAttributesSSBOInfo creature_Angles;
extern CreatureAttributesSSBOInfo creature_AngleVelocities;
extern CreatureAttributesSSBOInfo creature_ForwardDirections;
extern CreatureAttributesSSBOInfo creature_RightDirections;

// Movement
extern CreatureAttributesSSBOInfo creature_ForwardThrusts;
extern CreatureAttributesSSBOInfo creature_TurnThrusts;

// Body parameters
extern CreatureAttributesSSBOInfo creature_Radii;
extern CreatureAttributesSSBOInfo creature_Lives;
extern CreatureAttributesSSBOInfo creature_Energies;
extern CreatureAttributesSSBOInfo creature_Meats;
extern CreatureAttributesSSBOInfo creature_Harndesses;
extern CreatureAttributesSSBOInfo creature_Horninesses;

// Creature misc
extern CreatureAttributesSSBOInfo creature_Generations;
extern CreatureAttributesSSBOInfo creature_UniformGridTiles;

// Colliders
extern CreatureAttributesSSBOInfo creature_CollidersCounts;
extern CreatureAttributesSSBOInfo creature_CollidersIndicesAndPlacements;
extern CreatureAttributesSSBOInfo creature_CollidersToPosDirs;
extern CreatureAttributesSSBOInfo creature_CollidersPositions;
extern CreatureAttributesSSBOInfo creature_CollidersRadii;
extern CreatureAttributesSSBOInfo creature_CollidersGivenEnergy;

// General purpose data packets
extern CreatureAttributesSSBOInfo creature_GeneralPurposeVec2;
extern CreatureAttributesSSBOInfo creature_GeneralPurposeSecondVec2;
extern CreatureAttributesSSBOInfo creature_GeneralPurposeFloat;
extern CreatureAttributesSSBOInfo creature_GeneralPurposeUInt;

// Appearances
extern CreatureAttributesSSBOInfo creature_SkinHues;
extern CreatureAttributesSSBOInfo creature_SkinSaturations;
extern CreatureAttributesSSBOInfo creature_SkinValues;
extern CreatureAttributesSSBOInfo creature_SkinRGBColors;
extern CreatureAttributesSSBOInfo creature_SkinPatterns;

// Creature-localized devices (feeders, shields, sensors)
extern CreatureAttributesSSBOInfo creature_SpikeLocalAngles;
extern CreatureAttributesSSBOInfo creature_Spikes;

extern CreatureAttributesSSBOInfo creature_FeederLocalAngles;
extern CreatureAttributesSSBOInfo creature_Feeders;

extern CreatureAttributesSSBOInfo creature_ShieldLocalAngles;
extern CreatureAttributesSSBOInfo creature_Shields;