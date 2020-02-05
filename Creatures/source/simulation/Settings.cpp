#include "Settings.h"

SimSettings SIM_SETTINGS;


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

void SimulationSettings_Init()
{
	// Camera settings
	SIM_SETTINGS.MAX_CAMERA_ZOOM_OUT = 0.01f;
	SIM_SETTINGS.MAX_CAMERA_ZOOM_IN = 10.0f;
	SIM_SETTINGS.CAMERA_ZOOM_INTERPOLATION_SPEED = 0.035f;
	SIM_SETTINGS.CAMERA_ZOOM_SCROLL_SCALE = 0.1f;
	SIM_SETTINGS.CAMERA_ZOOM_MOVE_SCALE = 0.0002f;
	SIM_SETTINGS.CAMERA_VELOCITY_FRICTION = 0.985f;

	// Simulation settings
	SIM_SETTINGS.NUM_OF_CREATURES_ON_INIT.min = 0;
	SIM_SETTINGS.NUM_OF_CREATURES_ON_INIT.max = 1000000;
	SimulationSettings_SetInt(SIM_SETTINGS.NUM_OF_CREATURES_ON_INIT, 100000);

	SIM_SETTINGS.SIMULATION_WIDTH.min = 5;
	SIM_SETTINGS.SIMULATION_WIDTH.max = 300;
	SimulationSettings_SetFloat(SIM_SETTINGS.SIMULATION_WIDTH, 100);

	SIM_SETTINGS.SIMULATION_HEIGHT.min = 5;
	SIM_SETTINGS.SIMULATION_HEIGHT.max = 300;
	SimulationSettings_SetFloat(SIM_SETTINGS.SIMULATION_HEIGHT, 100);

	SIM_SETTINGS.MAX_CREATURE_RADIUS.min = 0.1;
	SIM_SETTINGS.MAX_CREATURE_RADIUS.max = 0.1;
	SimulationSettings_SetFloat(SIM_SETTINGS.MAX_CREATURE_RADIUS, 0.1);

	SIM_SETTINGS.MIN_CREATURE_RADIUS.min = 0.1;
	SIM_SETTINGS.MIN_CREATURE_RADIUS.max = 0.1;
	SimulationSettings_SetFloat(SIM_SETTINGS.MIN_CREATURE_RADIUS, 0.1);

	SIM_SETTINGS.MAX_CREATURE_SENSE_RADIUS.min = 0.1;
	SIM_SETTINGS.MAX_CREATURE_SENSE_RADIUS.max = 1;
	SimulationSettings_SetFloat(SIM_SETTINGS.MAX_CREATURE_SENSE_RADIUS, 0.25);
}