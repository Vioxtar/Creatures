#include "Settings.h"



/////////////////////////////
// -- CONSTANT SETTINGS -- //
/////////////////////////////


// Window settings
extern const uint16_t WINDOW_STARTING_WIDTH = 1000;
extern const uint16_t WINDOW_STARTING_HEIGHT = 1000;

// UI settings
extern const float UI_DEARIMGUI_STYLE_FRAME_ROUNDING = 10.0;

extern const float UI_MAX_CREATURE_SELECTION_SQUARED_DISTANCE = 1.0;
extern const unsigned int UI_CREATURE_TRACKER_HALO_RADIUS_PIXEL_BIAS = 20;
extern const unsigned int UI_CREATURE_TRACKER_HALO_NUM_OF_SEGMENTS = 48;
extern const unsigned int UI_CREATURE_TRACKER_HALO_PIXEL_THICKNESS = 10;
extern const unsigned int UI_CREATURE_TRACKER_HALO_ALPHA = 10;

extern const unsigned int UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS = 2;
extern const unsigned int UI_CREATURE_TRACKER_DIRECTION_PIXEL_DISTANCE = 20;


// Technical settings
extern const uint16_t TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH = 500;
extern const GLenum TECH_SSBO_USAGE = GL_STATIC_DRAW;

// @TODO: Query the maximum local size
extern const uint16_t TECH_INIT_NEW_FRAME_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_DEFAULT_COMPUTE_PROGRAM_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE = 4;
extern const uint16_t TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_BRAIN_PULL_OUTPUTS_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_UPDATE_CREATURE_PLACEMENTS_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_BORDER_PHYSICS_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_UNIFORM_GRID_BIND_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_UNIFORM_GRID_UNBIND_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_CREATURE_INTERACTIONS_WORKGROUP_LOCAL_SIZE = 1536;
extern const uint16_t TECH_CREATURE_BODY_WORK_WORKGROUP_LOCAL_SIZE = 1536;

// Camera settings
extern const float CAMERA_MAX_ZOOM_OUT = 0.001f;
extern const float CAMERA_MAX_ZOOM_IN = 10.0f;
extern const float CAMERA_ZOOM_INTERPOLATION_SPEED = 0.1f;
extern const float CAMERA_ZOOM_SCROLL_SCALE = 0.1f;
extern const float CAMERA_ZOOM_MOVE_SCALE = 0.0002f;
extern const float CAMERA_VELOCITY_FRICTION = 0.985f;
extern const float CAMERA_START_ZOOM = 0.01f;
extern const float CAMERA_START_ZOOM_TARGET = 0.01f;

// Simulation settings
extern const float SIMULATION_UNIFORM_GRID_DIMENSION_BUFFER = 5.0f;
extern const int SIMULATION_UNIFORM_GRID_TILE_CREATURE_CAPACITY_SCALAR = 10;

// Render settings
extern const uint16_t RENDER_NUM_OF_CREATURE_BODY_VERTICES = 60;

// Creature brain settings
extern const uint16_t CREATURE_BRAIN_NUM_OF_INPUTS = 36;
extern const uint16_t CREATURE_BRAIN_NUM_OF_OUTPUTS = 12;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS = 3;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL = 20;

// Creature eye settings
extern const uint16_t CREATURE_EYE_NUM_OF_CONES = 16;
extern const uint16_t CREATURE_EYE_NUM_OF_CONE_VALUES = 1;
extern const uint16_t CREATURE_EYE_NUM_OF_PUPIL_VALUES = 10;


//////////////////////////////
// -- TWEAKABLE SETTINGS -- //
//////////////////////////////

extern TweakableIntegerSetting SIMULATION_NUM_OF_CREATURES_ON_INIT = { 20000, 0, 1000000 };
extern TweakableFloatSetting SIMULATION_WIDTH = { 1000, 1, 300 };
extern TweakableFloatSetting SIMULATION_HEIGHT = { 1000, 1, 300 };

extern TweakableFloatSetting SIMULATION_BORDER_RESTITUTION = { 0.0, 0.0, 1.0 };
extern TweakableFloatSetting SIMULATION_VELOCITY_DOWNSCALE = { 0.985, 0.0, 1.0 };
extern TweakableFloatSetting SIMULATION_ANGLE_VELOCITY_DOWNSCALE = { 1.0, 0.0, 1.0 };

// Render settings
extern TweakableFloatSetting RENDER_CLEAR_COLOR_R = { 0.1, 0.0, 1.0 };
extern TweakableFloatSetting RENDER_CLEAR_COLOR_G = { 0.1, 0.0, 1.0 };
extern TweakableFloatSetting RENDER_CLEAR_COLOR_B = { 0.1, 0.0, 1.0 };

// Creature body settings
extern TweakableFloatSetting CREATURE_MAX_RADIUS = { 0.4, 0.3, 0.5 };
extern TweakableFloatSetting CREATURE_MIN_RADIUS = { 0.2, 0.1, 0.3 };

// Creature eye settings
extern TweakableFloatSetting CREATURE_EYE_MAX_PROBE_DISTANCE = { 3.0, 0.1, 3.0 };
extern TweakableFloatSetting CREATURE_EYE_MAX_CONES_RADIUS = { 2.0, 0.1, 2.0 };



///////////////////////////////
// -- SUBSEQUENT SETTINGS -- //
///////////////////////////////

// These should not be edited lightly

// Calculate buffer size extern constants (used to define the size of each structure/node/link creature attributes)
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_NODES = CREATURE_BRAIN_NUM_OF_INPUTS + CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS * CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL + CREATURE_BRAIN_NUM_OF_OUTPUTS;

extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_LINKS = CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL > 0 ?

// We have midlevels in our max structure
(CREATURE_BRAIN_NUM_OF_INPUTS * CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL +
	CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL * CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL * (CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS - 1) +
	CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL * CREATURE_BRAIN_NUM_OF_OUTPUTS)
	:
	// We don't have midlevels in our max structure, number of links is just number of inputs * number of outputs
	(CREATURE_BRAIN_NUM_OF_INPUTS * CREATURE_BRAIN_NUM_OF_OUTPUTS);

extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES = CREATURE_BRAIN_MAX_NUM_OF_NODES - CREATURE_BRAIN_NUM_OF_INPUTS;

extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES = 1 + 1 + CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS + 1; // [NumOfLevels, NumOfInputs, NumOfMidLevels, NumOfOutputs]


// Deformations - the max number of deformers is the maximum amount of circles of minimal radius that can simultaneously touch a single circle of maximal radius
extern const GLuint CREATURE_MAX_NUM_OF_DEFORMERS = floor(M_PI / asin(CREATURE_MIN_RADIUS.min / (CREATURE_MAX_RADIUS.max + CREATURE_MIN_RADIUS.min)));



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