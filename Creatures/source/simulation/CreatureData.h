#pragma once

#include "../Settings.h"
#include <glm/glm.hpp>
#include <vector>

using namespace glm;

struct CreatureData
{
	vector<GLfloat> brainLinks;
	vector<GLfloat> brainNodes;
	vector<vec2> brainBiasesExponents;
	vector<GLuint> brainStructure;
	vec3 col;
	vec2 pos;
	vec2 vel;
	GLfloat rad;
	GLfloat life;
	GLfloat angle;
	GLfloat angleVel;
	GLfloat forwardThrust;
	GLfloat turnThrust;
	GLfloat hardness;
	vec2 skin;
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

// Physics
extern CreatureAttributesSSBOInfo creature_Positions;
extern CreatureAttributesSSBOInfo creature_Velocities;
extern CreatureAttributesSSBOInfo creature_Angles;
extern CreatureAttributesSSBOInfo creature_AngleVelocities;
extern CreatureAttributesSSBOInfo creature_ForwardDirections;

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
extern CreatureAttributesSSBOInfo creature_GeneralPurpose;

// Appearances
extern CreatureAttributesSSBOInfo creature_Colors;
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