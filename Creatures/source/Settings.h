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

//////////////////////////////////
// -- SETTING VALUES ASSERTS -- //
//////////////////////////////////

void Settings_AssertValuesValidity();


/////////////////////////////
// -- CONSTANT SETTINGS -- //
/////////////////////////////


// Window settings
extern const uint16_t WINDOW_STARTING_WIDTH;
extern const uint16_t WINDOW_STARTING_HEIGHT;
extern const uint16_t WINDOW_RENDER_MULTISAMPLES;

// UI settings
extern const float UI_DEARIMGUI_STYLE_FRAME_ROUNDING;

extern const float UI_MAX_CREATURE_SELECTION_SQUARED_DISTANCE;
extern const unsigned int UI_CREATURE_TRACKER_HALO_RADIUS_PIXEL_BIAS;
extern const unsigned int UI_CREATURE_TRACKER_HALO_NUM_OF_SEGMENTS;
extern const unsigned int UI_CREATURE_TRACKER_HALO_PIXEL_THICKNESS;
extern const unsigned int UI_CREATURE_TRACKER_HALO_ALPHA;

extern const unsigned int UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS;
extern const unsigned int UI_CREATURE_TRACKER_DIRECTION_PIXEL_DISTANCE;

extern const float UI_CREATURE_TRACKER_EYE_PUPIL_SIGHTS_BAR_HEIGHT;
extern const float UI_CREATURE_TRACKER_EYE_PUPIL_SIGHTS_BAR_WIDTH;


extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_NUM_OF_SEGMENTS;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_ALPHA;
extern const float UI_CREATURE_TRACKER_BRAIN_NODE_OUTLINE_THICKNESS;

extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_R;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_G;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_COLOR_B;

extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_OUTLINE_COLOR_R;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_OUTLINE_COLOR_G;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_OUTLINE_COLOR_B;


// Technical settings
extern const unsigned int TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH;
extern const unsigned int TECH_CREATURES_SUPPORTED_ON_INIT_BUFFER_PREALLOCATION;
extern const unsigned int TECH_CREATURE_DATA_PERSISTENT_MAP_BUFFER_BYTE_CAPACITY_BIAS;


extern const GLenum TECH_SSBO_USAGE;

extern const GLbitfield TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS;

extern const GLbitfield TECH_CREATURE_LIST_BUFFER_FLAGS;
extern const GLbitfield TECH_CREATURE_LIST_MAPPING_FLAGS;


// @TODO: Query the maximum local size
extern const uint16_t TECH_FRAME_PRE_LOGIC_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_FRAME_POST_LOGIC_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_DEFAULT_COMPUTE_PROGRAM_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BRAIN_PULL_OUTPUTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UPDATE_CREATURE_PLACEMENTS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_BORDER_PHYSICS_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UNIFORM_GRID_BIND_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_UNIFORM_GRID_UNBIND_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_INTERACTIONS_PART1_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_INTERACTIONS_PART2_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_INTERACTIONS_PART3_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_SIGHTS_PART1_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_SIGHTS_PART2_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_SIGHTS_PART3_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_BRAIN_PULL_OUTPUTS_AND_BODY_WORKS_PART1_WORKGROUP_LOCAL_SIZE;
extern const uint16_t TECH_CREATURE_BRAIN_PULL_OUTPUTS_AND_BODY_WORKS_PART2_WORKGROUP_LOCAL_SIZE;


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

// Creature misc settings
extern const float CREATURE_DEFAULT_BODY_MASS;

// Creature brain settings
extern const uint16_t CREATURE_BRAIN_NUM_OF_OUTPUTS;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL;

extern const float CREATURE_BRAIN_NEW_ACTIVATION_EXPONENT_MAX_ABS;
extern const float CREATURE_BRAIN_NEW_ACTIVATION_EXPONENT_PERCENTAGE_EXPONENT;
extern const float CREATURE_BRAIN_NEW_BIAS_MAX_ABS;
extern const float CREATURE_BRAIN_NEW_BIAS_PERCENTAGE_EXPONENT;
extern const float CREATURE_BRAIN_NEW_LINK_WEIGHT_MAX_ABS;
extern const float CREATURE_BRAIN_NEW_LINK_WEIGHT_PERCENTAGE_EXPONENT;


// Creature brain mutation settings
extern const float CREATURE_MUTATION_BRAIN_CHANGE_MIDLEVELS_CHANCE;

extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_CHANCE;
extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_MAX_ABS;
extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_BIAS_PERCENTAGE_EXPONENT;

extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_CHANCE;
extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_MAX_ABS;
extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_ACTIVATION_EXPONENT_PERCENTAGE_EXPONENT;

extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_CHANCE;
extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_MAX_ABS;
extern const float CREATURE_MUTATION_BRAIN_CHANGE_SINGLE_LINK_WEIGHT_PERCENTAGE_EXPONENT;

// Creature body mutation settings
extern const float CREATURE_MUTATION_DEVICE_CHANGE_SPIKE_ANGLE_CHANCE;
extern const float CREATURE_MUTATION_DEVICE_CHANGE_SPIKE_ANGLE_MAX_ABS;
extern const float CREATURE_MUTATION_DEVICE_CHANGE_SPIKE_ANGLE_PERCENTAGE_EXPONENT;

extern const float CREATURE_MUTATION_DEVICE_CHANGE_SHIELD_ANGLE_CHANCE;
extern const float CREATURE_MUTATION_DEVICE_CHANGE_SHIELD_ANGLE_MAX_ABS;
extern const float CREATURE_MUTATION_DEVICE_CHANGE_SHIELD_ANGLE_PERCENTAGE_EXPONENT;

extern const float CREATURE_MUTATION_DEVICE_CHANGE_FEEDER_ANGLE_CHANCE;
extern const float CREATURE_MUTATION_DEVICE_CHANGE_FEEDER_ANGLE_MAX_ABS;
extern const float CREATURE_MUTATION_DEVICE_CHANGE_FEEDER_ANGLE_PERCENTAGE_EXPONENT;

extern const float CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_CHANCE;
extern const float CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_MAX_ABS;
extern const float CREATURE_MUTATION_BODY_CHANGE_SKIN_PATTERN_PERCENTAGE_EXPONENT;

extern const float CREATURE_MUTATION_BODY_CHANGE_SKIN_HUE_CHANCE;
extern const float CREATURE_MUTATION_BODY_CHANGE_SKIN_HUE_MAX_ABS;
extern const float CREATURE_MUTATION_BODY_CHANGE_SKIN_HUE_PERCENTAGE_EXPONENT;


// Creature eye settings
extern const uint16_t CREATURE_EYE_NUM_OF_CONES;
extern const uint16_t CREATURE_EYE_NUM_OF_VALUES_IN_SINGLE_CONE;
extern const uint16_t CREATURE_EYE_NUM_OF_PUPIL_VALUES;





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

// UI settings
extern TweakableFloatSetting UI_CREATURE_TRACKER_CAMERA_FOLLOW_INTERPOLATION_RATE;


// Simulation space settings
extern TweakableFloatSetting SIMULATION_SPACE_WIDTH;
extern TweakableFloatSetting SIMULATION_SPACE_HEIGHT;

// Simulation physics settings
extern TweakableFloatSetting SIMULATION_BORDER_RESTITUTION;
extern TweakableFloatSetting SIMULATION_VELOCITY_DOWNSCALE;
extern TweakableFloatSetting SIMULATION_ANGLE_VELOCITY_DOWNSCALE;

// Simulation creature settings
extern TweakableIntegerSetting SIMULATION_NUM_OF_CREATURES_ON_INIT;
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_CONSTANT_SPAWN_RATE;

extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_RATE;
extern TweakableIntegerSetting SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_NUM_OF_CREATURES_UPPER_TARGET;
extern TweakableIntegerSetting SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_NUM_OF_CREATURES_LOWER_TARGET;

extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_UP_SPAWN_RATE;
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_DOWN_SPAWN_RATE;
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_STEP;
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_SPAWN_RATE_EXPONENT;

extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_ENERGY;
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_MEAT;
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_LIFE;
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_RADIUS;
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_HARDNESS;

extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_ENERGY;
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_MEAT;
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_LIFE;
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_RADIUS;
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_HARDNESS;

// Render settings
extern TweakableFloatSetting RENDER_CLEAR_COLOR_R;
extern TweakableFloatSetting RENDER_CLEAR_COLOR_G;
extern TweakableFloatSetting RENDER_CLEAR_COLOR_B;

extern TweakableFloatSetting RENDER_CREATURE_TRANSPARENCY_ENERGY_EXPONENT;

// Creature body settings
extern TweakableFloatSetting CREATURE_MAX_RADIUS;
extern TweakableFloatSetting CREATURE_MIN_RADIUS;

extern TweakableFloatSetting CREATURE_RADIUS_INTERPOLATION_RATE;
extern TweakableFloatSetting CREATURE_RADIUS_PERCENTAGE_MEAT_WEIGHT;
extern TweakableFloatSetting CREATURE_RADIUS_PERCENTAGE_MUSCLE_WEIGHT;

extern TweakableFloatSetting CREATURE_MAX_HARDNESS;
extern TweakableFloatSetting CREATURE_MIN_HARDNESS;
extern TweakableFloatSetting CREATURE_HARDNESS_INTERPOLATION_RATE;

extern TweakableFloatSetting CREATURE_MAX_SKIN_VALUE;
extern TweakableFloatSetting CREATURE_MIN_SKIN_VALUE;
extern TweakableFloatSetting CREATURE_SKIN_VALUE_INTERPOLATION_RATE;

extern TweakableFloatSetting CREATURE_MAX_SKIN_SATURATION;
extern TweakableFloatSetting CREATURE_MIN_SKIN_SATURATION;

// Creature energy/meat/life settings
extern TweakableFloatSetting CREATURE_MAX_ENERGY;
extern TweakableFloatSetting CREATURE_MAX_MEAT;
extern TweakableFloatSetting CREATURE_MAX_LIFE;

extern TweakableFloatSetting CREATURE_ENERGY_TO_MEAT_CONVERSION_RATE;
extern TweakableFloatSetting CREATURE_MEAT_TO_ENERGY_CONVERSION_RATE;
extern TweakableFloatSetting CREATURE_ENERGY_TO_LIFE_CONVERSION_RATE;

extern TweakableFloatSetting CREATURE_LIFE_DRAIN_ON_NO_ENERGY;
extern TweakableFloatSetting CREATURE_ENERGY_DRAIN_CONSTANT;


// Creature device settings
extern TweakableFloatSetting CREATURE_ENERGY_PERCENTAGE_BASED_DEVICE_STATE_MULTIPLIER_EXPONENT;

extern TweakableFloatSetting CREATURE_DEVICE_AIM_DOT_THRESHOLD;
extern TweakableFloatSetting CREATURE_DEVICE_FEEDER_EFFECTIVENESS;
extern TweakableFloatSetting CREATURE_DEVICE_SPIKE_VERSUS_LIFE_EFFECTIVENESS;
extern TweakableFloatSetting CREATURE_DEVICE_SPIKE_VERSUS_MEAT_EFFECTIVENESS;
extern TweakableFloatSetting CREATURE_DEVICE_SHIELD_EFFECTIVENESS;

extern TweakableFloatSetting CREATURE_DEVICE_STATE_INTERPOLATION_RATE;

// Creature reproduction settings
extern TweakableFloatSetting CREATURE_REPRODUCTION_AIM_DOT_THRESHOLD;


// Creature movement settings
extern TweakableFloatSetting CREATURE_ENERGY_PERCENTAGE_BASED_MOVEMENT_MULTIPLIER_EXPONENT;

extern TweakableFloatSetting CREATURE_FORWARD_MOVEMENT_EFFECTIVENESS;
extern TweakableFloatSetting CREATURE_BACKWARD_MOVEMENT_EFFECTIVENESS;
extern TweakableFloatSetting CREATURE_STRAFE_MOVEMENT_EFFECTIVENESS;
extern TweakableFloatSetting CREATURE_TURN_MOVEMENT_EFFECTIVENESS;

extern TweakableFloatSetting CREATURE_MAX_VELOCITY_LENGTH;
extern TweakableFloatSetting CREATURE_MAX_ANGLE_VELOCITY_LENGTH;

// Creature energy costs
extern TweakableFloatSetting CREATURE_FORWARD_MOVEMENT_ENERGY_COST;
extern TweakableFloatSetting CREATURE_BACKWARD_MOVEMENT_ENERGY_COST;
extern TweakableFloatSetting CREATURE_STRAFE_MOVEMENT_ENERGY_COST;
extern TweakableFloatSetting CREATURE_TURN_MOVEMENT_ENERGY_COST;

extern TweakableFloatSetting CREATURE_DEVICE_FEEDER_ENERGY_COST;
extern TweakableFloatSetting CREATURE_DEVICE_SPIKE_ENERGY_COST;
extern TweakableFloatSetting CREATURE_DEVICE_SHIELD_ENERGY_COST;

extern TweakableFloatSetting CREATURE_REPRODUCTION_ENERGY_COST;
extern TweakableFloatSetting CREATURE_REPRODUCTION_ENERGY_THRESHOLD;
extern TweakableFloatSetting CREATURE_REPRODUCTION_MEAT_THRESHOLD;

// Creature death settings
extern TweakableFloatSetting CREATURE_DEATH_MEAT_ROT_RATE;
extern TweakableFloatSetting CREATURE_DEATH_WITHOUT_MEAT_SHRINK_RATE;
extern TweakableFloatSetting CREATURE_DEATH_EXISTENCE_RADIUS_THRESHOLD;

extern TweakableFloatSetting CREATURE_DEATH_HARDNESS_TARGET;
extern TweakableFloatSetting CREATURE_DEATH_HARDNESS_INTERPOLATION_RATE;
extern TweakableFloatSetting CREATURE_DEATH_DEVICE_ZEROFICATION_INTERPOLATION_RATE;

extern TweakableFloatSetting CREATURE_DEATH_ENERGY_ZEROFICATION_INTERPOLATION_RATE;

extern TweakableFloatSetting CREATURE_DEATH_SKIN_VALUE_TARGET;
extern TweakableFloatSetting CREATURE_DEATH_SKIN_VALUE_INTERPOLATION_RATE;

// Creature body mutation settings

// Creature eye settings
extern TweakableFloatSetting CREATURE_EYE_MAX_PROBE_DISTANCE;
extern TweakableFloatSetting CREATURE_EYE_MAX_CONES_RADIUS;




///////////////////////////////////////////
// -- ASSISTING & SUBSEQUENT SETTINGS -- //
///////////////////////////////////////////

// Eye settings
extern const uint16_t CREATURE_EYE_NUM_OF_CONES_VALUES;

// Calculate buffer size extern constants (used to define the size of each structure/node/link creature attributes)
extern const uint16_t CREATURE_BRAIN_NUM_OF_INPUTS;
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_NODES;
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_LINKS;
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_ACTIVATED_NODES;
extern const GLuint CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES;

// Colliders
extern const GLuint CREATURE_MAX_NUM_OF_COLLIDERS;
