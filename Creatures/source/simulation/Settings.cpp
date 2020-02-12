#include "Settings.h"






// Technical settings
extern const uint16_t TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH = 500;

extern const uint16_t TECH_DEFAULT_COMPUTE_PROGRAM_WORKGROUP_LOCAL_SIZE = 1024;
extern const uint16_t TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE = 4;
extern const uint16_t TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE = 1024;
extern const uint16_t TECH_BRAIN_PULL_OUTPUTS_WORKGROUP_LOCAL_SIZE = 1024;
extern const uint16_t TECH_UPDATE_CREATURE_PLACEMENTS_WORKGROUP_LOCAL_SIZE = 1024;
extern const uint16_t TECH_BORDER_PHYSICS_WORKGROUP_LOCAL_SIZE = 1024;
extern const uint16_t TECH_UNIFORM_GRID_BIND_WORKGROUP_LOCAL_SIZE = 1024;
extern const uint16_t TECH_UNIFORM_GRID_UNBIND_WORKGROUP_LOCAL_SIZE = 1024;
extern const uint16_t TECH_CREATURE_COLLISION_WORKGROUP_LOCAL_SIZE = 1024;
extern const uint16_t TECH_CREATURE_ACTUATIONS_WORKGROUP_LOCAL_SIZE = 1024;


// Camera settings
extern const float CAMERA_MAX_ZOOM_OUT = 0.001f;
extern const float CAMERA_MAX_ZOOM_IN = 10.0f;
extern const float CAMERA_ZOOM_INTERPOLATION_SPEED = 0.035f;
extern const float CAMERA_ZOOM_SCROLL_SCALE = 0.1f;
extern const float CAMERA_ZOOM_MOVE_SCALE = 0.0002f;
extern const float CAMERA_VELOCITY_FRICTION = 0.985f;
extern const float CAMERA_START_ZOOM = 0.1f;
extern const float CAMERA_START_ZOOM_TARGET = 0.01f;

// Simulation settings
extern const float SIMULATION_UNIFORM_GRID_DIMENSION_BUFFER = 5.0f;
extern const int SIMULATION_UNIFORM_GRID_TILE_CREATURE_CAPACITY_SCALAR = 2;

extern TweakableIntegerSetting SIMULATION_NUM_OF_CREATURES_ON_INIT = { 100000, 0, 1000000 };
extern TweakableFloatSetting SIMULATION_WIDTH = { 100, 1, 300 };
extern TweakableFloatSetting SIMULATION_HEIGHT = { 100, 1, 300 };

extern TweakableFloatSetting SIMULATION_BORDER_RESTITUTION = { 0.5, 0.0, 1.0 };
extern TweakableFloatSetting SIMULATION_VELOCITY_DOWNSCALE = { 0.99, 0.0, 1.0 };
extern TweakableFloatSetting SIMULATION_ANGLE_VELOCITY_DOWNSCALE = { 0.99, 0.0, 1.0 };

// Creature body settings
extern TweakableFloatSetting CREATURE_MAX_RADIUS = { 0.1, 0.1, 0.1 };
extern TweakableFloatSetting CREATURE_MIN_RADIUS = { 0.1, 0.1, 0.1 };
extern TweakableFloatSetting CREATURE_MAX_SENSE_RADIUS = { 0.1, 0.1, 2.0 };


// Creature brain settings
extern const uint16_t CREATURE_NUM_OF_INPUTS = 36;
extern const uint16_t CREATURE_NUM_OF_OUTPUTS = 12;
extern const uint16_t CREATURE_MAX_NUM_OF_MIDLEVELS = 3;
extern const uint16_t CREATURE_MAX_NUM_OF_NODES_IN_MIDLEVEL = 20;






////////////////////////////////////
// -- TWEAKABLE SETTINGS UTILS -- //
////////////////////////////////////


vector<SimSettingsChangedSubscriber> simSettingsChangedSubscribers;
SimSettingsChangedSubscriber SimulationSettingsChangedSubscribe(SimSettingsChangedSubscriber subscriber)
{
	simSettingsChangedSubscribers.push_back(subscriber);
	return subscriber;
}

void NotifySimSettingChangedSubscribers()
{
	for (auto sub : simSettingsChangedSubscribers)
	{
		sub();
	}
}

void SimulationSettings_SetInt(TweakableIntegerSetting& setting, const int value)
{
	int oldValue = setting.value;
	setting.value = clamp(value, setting.min, setting.max);
	if (setting.value != oldValue)
	{
		NotifySimSettingChangedSubscribers();
	}
}

void SimulationSettings_SetFloat(TweakableFloatSetting& setting, const float value)
{
	float oldValue = setting.value;
	setting.value = clamp(value, setting.min, setting.max);
	if (setting.value != oldValue)
	{
		NotifySimSettingChangedSubscribers();
	}
}

void SimulationSettings_SetBool(bool& setting, const bool value)
{
	bool oldValue = setting;
	setting = value;
	if (setting != oldValue)
	{
		NotifySimSettingChangedSubscribers();
	}
}