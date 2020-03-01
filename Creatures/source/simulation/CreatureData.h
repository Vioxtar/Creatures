#pragma once

#include "../Settings.h"
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <exception>

using namespace glm;

struct CreatureData
{
	vector<GLfloat> brainLinks;
	vector<GLfloat> brainNodes;
	vector<vec2> brainBiasesExponents;
	vector<GLuint> brainStructure;
	vector<GLfloat> eyeConeSights;
	GLfloat skinHue;
	GLfloat skinSaturation;
	GLfloat skinValue;
	vec3 skinRGBColors;
	vec2 pos;
	vec2 vel;
	GLfloat rad;
	GLfloat life;
	GLfloat angle;
	GLfloat angleVel;
	vec2 forwardDir;
	vec2 rightDir;
	GLfloat forwardThrust;
	GLfloat turnThrust;
	GLfloat hardness;
	vec2 skin;
	GLfloat spikeLocalAngle;
	GLfloat feederLocalAngle;
	GLfloat shieldLocalAngle;
	GLfloat shieldSpan;
	vec2 eyePos;
	vec2 eyeMuscles;
	GLfloat eyeConeRadius;
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
extern CreatureAttributesSSBOInfo creature_SpikeStates;
extern CreatureAttributesSSBOInfo creature_SpikeLocalAngles;
extern CreatureAttributesSSBOInfo creature_SpikeDirections;

extern CreatureAttributesSSBOInfo creature_FeederStates;
extern CreatureAttributesSSBOInfo creature_FeederLocalAngles;
extern CreatureAttributesSSBOInfo creature_FeederDirections;

extern CreatureAttributesSSBOInfo creature_ShieldStates;
extern CreatureAttributesSSBOInfo creature_ShieldLocalAngles;
extern CreatureAttributesSSBOInfo creature_ShieldSpans;
extern CreatureAttributesSSBOInfo creature_ShieldDirections;

// @TODO: actually implement this placeholder
//extern CreatureAttributesSSBOInfo creature_SensorsStates;
//extern CreatureAttributesSSBOInfo creature_SensorsLocalAngles;
//extern reatureAttributesSSBOInfo creature_SensorsDirections;

// Deformations
extern CreatureAttributesSSBOInfo creature_DeformerPositions;
extern CreatureAttributesSSBOInfo creature_DeformerRadii;
extern CreatureAttributesSSBOInfo creature_DeformerCounts;
