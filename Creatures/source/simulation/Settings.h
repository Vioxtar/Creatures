#pragma once

#include <iostream>
#include <algorithm> 
#include <vector>

#include "..//utils/MathUtils.h"

using namespace std;
using namespace mathutils;

struct TweakableIntegerSetting
{
	int min;
	int max;
	int value;
};

struct TweakableFloatSetting
{
	float min;
	float max;
	float value;
};

struct SimSettings
{
	// Camera settings
	float MAX_CAMERA_ZOOM_OUT;
	float MAX_CAMERA_ZOOM_IN;
	float CAMERA_ZOOM_INTERPOLATION_SPEED;
	float CAMERA_ZOOM_SCROLL_SCALE;
	float CAMERA_ZOOM_MOVE_SCALE;
	float CAMERA_VELOCITY_FRICTION;
	float CAMERA_START_ZOOM;

	// Simulation settings
	float SIMULATION_UNIFORM_GRID_DIMENSION_BUFFER;
	int SIMULATION_UNIFORM_GRID_TILE_CREATURE_CAPACITY_SCALAR;

	TweakableIntegerSetting NUM_OF_CREATURES_ON_INIT;
	TweakableFloatSetting SIMULATION_WIDTH;
	TweakableFloatSetting SIMULATION_HEIGHT;

	// Creature body settings
	TweakableFloatSetting MAX_CREATURE_RADIUS;
	TweakableFloatSetting MIN_CREATURE_RADIUS;
	TweakableFloatSetting MAX_CREATURE_SENSE_RADIUS;

	

};

void SimulationSettings_SetInt(TweakableIntegerSetting& setting, int value);
void SimulationSettings_SetFloat(TweakableFloatSetting& setting, float value);
void SimulationSettings_SetBool(bool& setting, bool value);

extern SimSettings SIM_SETTINGS;

void SimulationSettings_Init();

// Used to let subscribers know when they should update their values
typedef void (*SimSettingsChangedSubscriber)();
SimSettingsChangedSubscriber SimulationSettingsChangedSubscribe(SimSettingsChangedSubscriber subscriber);