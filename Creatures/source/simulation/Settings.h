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

extern const uint16_t TECH_DEFAULT_COMPUTE_PROGRAM_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_PULL_OUTPUTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UPDATE_CREATURE_PLACEMENTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BORDER_PHYSICS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UNIFORM_GRID_BIND_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UNIFORM_GRID_UNBIND_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_COLLISION_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_ACTUATIONS_WORKGROUP_LOCAL_SIZE;

// Camera settings
extern const float CAMERA_MAX_ZOOM_OUT;
extern const float CAMERA_MAX_ZOOM_IN;
extern const float CAMERA_ZOOM_INTERPOLATION_SPEED;
extern const float CAMERA_ZOOM_SCROLL_SCALE;
extern const float CAMERA_ZOOM_MOVE_SCALE;
extern const float CAMERA_VELOCITY_FRICTION;
extern const float CAMERA_START_ZOOM;
extern const float CAMERA_START_ZOOM_TARGET;

// Simulation settings
extern const float SIMULATION_UNIFORM_GRID_DIMENSION_BUFFER;
extern const int SIMULATION_UNIFORM_GRID_TILE_CREATURE_CAPACITY_SCALAR;

extern TweakableIntegerSetting SIMULATION_NUM_OF_CREATURES_ON_INIT;
extern TweakableFloatSetting SIMULATION_WIDTH;
extern TweakableFloatSetting SIMULATION_HEIGHT;

extern TweakableFloatSetting SIMULATION_BORDER_RESTITUTION;
extern TweakableFloatSetting SIMULATION_VELOCITY_DOWNSCALE;
extern TweakableFloatSetting SIMULATION_ANGLE_VELOCITY_DOWNSCALE;

// Creature body settings
extern TweakableFloatSetting CREATURE_MAX_RADIUS;
extern TweakableFloatSetting CREATURE_MIN_RADIUS;
extern TweakableFloatSetting CREATURE_MAX_SENSE_RADIUS;


// Creature brain settings
extern const uint16_t CREATURE_BRAIN_NUM_OF_INPUTS;
extern const uint16_t CREATURE_BRAIN_NUM_OF_OUTPUTS;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL;
extern const float CREATURE_BRAIN_ACTIVATION_EXPONENT;