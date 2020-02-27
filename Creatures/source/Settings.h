#pragma once

#include <iostream>
#include <algorithm> 
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glad/glad.h>
#include "utils/MathUtils.h"

using namespace std;
using namespace mathutils;


/////////////////////////////
// -- CONSTANT SETTINGS -- //
/////////////////////////////


// Window settings
extern const uint16_t WINDOW_STARTING_WIDTH;
extern const uint16_t WINDOW_STARTING_HEIGHT;

// UI settings
extern const float UI_DEARIMGUI_STYLE_FRAME_ROUNDING;

extern const float UI_MAX_CREATURE_SELECTION_SQUARED_DISTANCE;
extern const unsigned int UI_CREATURE_TRACKER_HALO_RADIUS_PIXEL_BIAS;
extern const unsigned int UI_CREATURE_TRACKER_HALO_NUM_OF_SEGMENTS;
extern const unsigned int UI_CREATURE_TRACKER_HALO_PIXEL_THICKNESS;

extern const unsigned int UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS;


// Technical settings
extern const uint16_t TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH;
extern const GLenum TECH_SSBO_USAGE;

// @TODO: Query the maximum local size
extern const uint16_t TECH_INIT_NEW_FRAME_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_DEFAULT_COMPUTE_PROGRAM_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_PULL_OUTPUTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UPDATE_CREATURE_PLACEMENTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BORDER_PHYSICS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UNIFORM_GRID_BIND_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UNIFORM_GRID_UNBIND_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_INTERACTIONS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_BODY_WORK_WORKGROUP_LOCAL_SIZE;


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

// Render settings
extern const uint16_t RENDER_NUM_OF_CREATURE_BODY_VERTICES;

// Creature brain settings
extern const uint16_t CREATURE_BRAIN_NUM_OF_INPUTS;
extern const uint16_t CREATURE_BRAIN_NUM_OF_OUTPUTS;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL;




////////////////////////////////////
// -- TWEAKABLE SETTINGS UTILS -- //
////////////////////////////////////

struct TweakableIntegerSetting
{
	int value;
	const int min;
	const int max;
};

struct TweakableFloatSetting
{
	float value;
	const float min;
	const float max;
};

void SimulationSettings_SetInt(TweakableIntegerSetting& setting, int value);
void SimulationSettings_SetFloat(TweakableFloatSetting& setting, float value);
void SimulationSettings_SetBool(bool& setting, bool value);


// Used to let subscribers know when they should update their values
typedef void (*SimSettingsChangedSubscriber)();
SimSettingsChangedSubscriber SimulationSettingsChangedSubscribe(SimSettingsChangedSubscriber subscriber);




//////////////////////////////
// -- TWEAKABLE SETTINGS -- //
//////////////////////////////

// Simulation settings
extern TweakableIntegerSetting SIMULATION_NUM_OF_CREATURES_ON_INIT;
extern TweakableFloatSetting SIMULATION_WIDTH;
extern TweakableFloatSetting SIMULATION_HEIGHT;
extern TweakableFloatSetting SIMULATION_BORDER_RESTITUTION;
extern TweakableFloatSetting SIMULATION_VELOCITY_DOWNSCALE;
extern TweakableFloatSetting SIMULATION_ANGLE_VELOCITY_DOWNSCALE;

// Render settings
extern TweakableFloatSetting RENDER_CLEAR_COLOR_R;
extern TweakableFloatSetting RENDER_CLEAR_COLOR_G;
extern TweakableFloatSetting RENDER_CLEAR_COLOR_B;

// Creature body settings
extern TweakableFloatSetting CREATURE_MAX_RADIUS;
extern TweakableFloatSetting CREATURE_MIN_RADIUS;
extern TweakableFloatSetting CREATURE_MAX_SENSE_RADIUS;





///////////////////////////////
// -- SUBSEQUENT SETTINGS -- //
///////////////////////////////

// Calculate buffer size extern constants (used to define the size of each structure/node/link creature attributes)
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_NODES;
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_LINKS;
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES;
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES;

// Deformations
extern const GLuint CREATURE_MAX_NUM_OF_DEFORMERS;
