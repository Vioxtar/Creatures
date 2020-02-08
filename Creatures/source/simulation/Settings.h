#pragma once

#include <iostream>
#include <algorithm> 
#include <vector>

#include "..//utils/MathUtils.h"

using namespace std;
using namespace mathutils;

struct TweakableIntegerSetting
{
	int value;
	int min;
	int max;
};

struct TweakableFloatSetting
{
	float value;
	float min;
	float max;
};

void SimulationSettings_SetInt(TweakableIntegerSetting& setting, int value);
void SimulationSettings_SetFloat(TweakableFloatSetting& setting, float value);
void SimulationSettings_SetBool(bool& setting, bool value);

// Used to let subscribers know when they should update their values
typedef void (*SimSettingsChangedSubscriber)();
SimSettingsChangedSubscriber SimulationSettingsChangedSubscribe(SimSettingsChangedSubscriber subscriber);


// Technical settings
extern const uint16_t TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH;
extern const uint16_t TECH_COMPUTE_PROGRAM_WORKGROUP_SIZE;


// Camera settings
extern const float CAMERA_MAX_ZOOM_OUT;
extern const float CAMERA_MAX_ZOOM_IN;
extern const float CAMERA_ZOOM_INTERPOLATION_SPEED;
extern const float CAMERA_ZOOM_SCROLL_SCALE;
extern const float CAMERA_ZOOM_MOVE_SCALE;
extern const float CAMERA_VELOCITY_FRICTION;
extern const float CAMERA_START_ZOOM;


// Simulation settings
extern const float SIMULATION_UNIFORM_GRID_DIMENSION_BUFFER;
extern const int SIMULATION_UNIFORM_GRID_TILE_CREATURE_CAPACITY_SCALAR;

extern TweakableIntegerSetting SIMULATION_NUM_OF_CREATURES_ON_INIT;
extern TweakableFloatSetting SIMULATION_WIDTH;
extern TweakableFloatSetting SIMULATION_HEIGHT;


// Creature body settings
extern TweakableFloatSetting CREATURE_MAX_RADIUS;
extern TweakableFloatSetting CREATURE_MIN_RADIUS;
extern TweakableFloatSetting CREATURE_MAX_SENSE_RADIUS;


// Header compile time settings
#define CREATURE_MAX_BRAIN_LEVELS 3
#define CREATURE_MAX_NODES_IN_LEVEL 3
#define CREATURE_MAX_LINKS_IN_NODE CREATURE_MAX_NODES_IN_LEVEL // Same thing
#define CREATURE_MAX_NODES_IN_BRAIN CREATURE_MAX_BRAIN_LEVELS * CREATURE_MAX_NODES_IN_LEVEL