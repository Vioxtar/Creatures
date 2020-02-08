#include "Settings.h"


// Camera settings
extern const float CAMERA_MAX_ZOOM_OUT = 0.001f;
extern const float CAMERA_MAX_ZOOM_IN = 10.0f;
extern const float CAMERA_ZOOM_INTERPOLATION_SPEED = 0.035f;
extern const float CAMERA_ZOOM_SCROLL_SCALE = 0.1f;
extern const float CAMERA_ZOOM_MOVE_SCALE = 0.0002f;
extern const float CAMERA_VELOCITY_FRICTION = 0.985f;
extern const float CAMERA_START_ZOOM = 0.01f;

// Simulation settings
extern const float SIMULATION_UNIFORM_GRID_DIMENSION_BUFFER = 5.0f;
extern const int SIMULATION_UNIFORM_GRID_TILE_CREATURE_CAPACITY_SCALAR = 1;

extern TweakableIntegerSetting SIMULATION_NUM_OF_CREATURES_ON_INIT = { 100000, 0, 1000000 };
extern TweakableFloatSetting SIMULATION_WIDTH = { 200, 5, 300 };
extern TweakableFloatSetting SIMULATION_HEIGHT = { 200, 5, 300 };

// Creature body settings
extern TweakableFloatSetting CREATURE_MAX_RADIUS = { 0.1, 0.1, 0.1 };
extern TweakableFloatSetting CREATURE_MIN_RADIUS = { 0.1, 0.1, 0.1 };
extern TweakableFloatSetting CREATURE_MAX_SENSE_RADIUS = { 0.1, 0.1, 2.0 };

// Creature brain settings
const uint8_t CREATURE_MAX_BRAIN_LEVELS = 6;
const uint8_t CREATURE_MAX_NODES_IN_LEVEL = 35;



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