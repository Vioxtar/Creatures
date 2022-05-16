#include "Settings.h"



/////////////////////////////
// -- CONSTANT SETTINGS -- //
/////////////////////////////


// Window settings
extern const uint16_t WINDOW_STARTING_WIDTH = 1000;
extern const uint16_t WINDOW_STARTING_HEIGHT = 1000;
extern const uint16_t WINDOW_RENDER_MULTISAMPLES = 16;

// UI settings
extern const float UI_DEARIMGUI_STYLE_FRAME_ROUNDING = 10.0;

extern const float UI_DEARIMGUI_WINDOW_SMOOTH_RESIZE_RATE = 0.1;
extern const float UI_DEARIMGUI_WINDOW_CLOSING_PIXEL_SIZE_THRESHOLD = 50.0;

extern const float UI_CREATURE_TRACKER_MIN_WINDOW_WIDTH = 200.0f;
extern const float UI_CREATURE_TRACKER_MIN_WINDOW_HEIGHT = 400.0f;

extern const float UI_MAX_CREATURE_SELECTION_SQUARED_DISTANCE = 1.0;
extern const unsigned int UI_CREATURE_TRACKER_HALO_RADIUS_PIXEL_BIAS = 20;
extern const unsigned int UI_CREATURE_TRACKER_HALO_NUM_OF_SEGMENTS = 48;
extern const unsigned int UI_CREATURE_TRACKER_HALO_PIXEL_THICKNESS = 10;
extern const unsigned int UI_CREATURE_TRACKER_HALO_ALPHA = 10;

extern const unsigned int UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS = 2;
extern const unsigned int UI_CREATURE_TRACKER_DIRECTION_PIXEL_DISTANCE = 20;

extern const float UI_CREATURE_TRACKER_EYE_PUPIL_SIGHTS_BAR_HEIGHT = 20.0;
extern const float UI_CREATURE_TRACKER_EYE_PUPIL_SIGHTS_BAR_WIDTH = 200.0;


extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_NUM_OF_SEGMENTS = 25;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_ALPHA = 255;

extern const unsigned int UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_R = 255;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_G = 200;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_B = 0;

extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_R = 0;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_G = 55;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_B = 255;

extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_BACKGROUND_COLOR_R = 30;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_BACKGROUND_COLOR_G = 30;
extern const unsigned int UI_CREATURE_TRACKER_BRAIN_NODE_BACKGROUND_COLOR_B = 30;

extern const float UI_CREATURE_TRACKER_BRAIN_LINK_NEUTRAL_COLOR_R = 20 / 255.0f;
extern const float UI_CREATURE_TRACKER_BRAIN_LINK_NEUTRAL_COLOR_G = 20 / 255.0f;
extern const float UI_CREATURE_TRACKER_BRAIN_LINK_NEUTRAL_COLOR_B = 20 / 255.0f;

extern const float UI_CREATURE_TRACKER_BRAIN_LINK_MAX_WIDTH = 5.0;

extern const float UI_CREATURE_TRACKER_BRAIN_LINK_WEIGHT_MAX_ABS_NORMALIZER = 10.0;

extern const unsigned int UI_CREATURE_TRACKER_PLOT_NUM_OF_VALUES = 500;

// Technical settings
extern const unsigned int TECH_CREATURE_CAPACITY_INCREASE_ON_BUFFER_CAPACITY_BREACH = 25000;
extern const unsigned int TECH_CREATURES_SUPPORTED_ON_INIT_BUFFER_PREALLOCATION = 100000;
extern const unsigned int TECH_CREATURE_DATA_PERSISTENT_MAP_BUFFER_BYTE_CAPACITY_BIAS = 10000;


extern const GLenum TECH_SSBO_USAGE = GL_STATIC_DRAW;

extern const GLbitfield TECH_CREATURE_ATTRIBUTE_BUFFER_FLAGS = GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT;

extern const GLbitfield TECH_CREATURE_LIST_BUFFER_FLAGS = GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
extern const GLbitfield TECH_CREATURE_LIST_MAPPING_FLAGS = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;


// @TODO: Query the maximum local size
extern const uint16_t TECH_FRAME_PRE_LOGIC_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_FRAME_POST_LOGIC_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_DEFAULT_COMPUTE_PROGRAM_WORKGROUP_LOCAL_SIZE = 512;

extern const uint16_t TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE_X = 4;
extern const uint16_t TECH_BRAIN_FORWARD_PROPAGATE_WORKGROUP_LOCAL_SIZE_Y = 64;

extern const uint16_t TECH_BRAIN_PUSH_INPUTS_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_BRAIN_PULL_OUTPUTS_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_UPDATE_CREATURE_PLACEMENTS_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_BORDER_PHYSICS_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_UNIFORM_GRID_BIND_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_UNIFORM_GRID_UNBIND_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_CREATURE_INTERACTIONS_PART1_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_CREATURE_INTERACTIONS_PART2_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_CREATURE_INTERACTIONS_PART3_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_CREATURE_SIGHTS_PART1_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_CREATURE_SIGHTS_PART2_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_CREATURE_SIGHTS_PART3_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_CREATURE_BRAIN_PULL_OUTPUTS_AND_BODY_WORKS_PART1_WORKGROUP_LOCAL_SIZE = 512;
extern const uint16_t TECH_CREATURE_BRAIN_PULL_OUTPUTS_AND_BODY_WORKS_PART2_WORKGROUP_LOCAL_SIZE = 512;



// Camera settings
extern const float CAMERA_MAX_ZOOM_OUT = 0.001f;
extern const float CAMERA_MAX_ZOOM_IN = 10.0f;
extern const float CAMERA_ZOOM_INTERPOLATION_SPEED = 0.1f;
extern const float CAMERA_ZOOM_SCROLL_SCALE = 0.3f;
extern const float CAMERA_ZOOM_MOVE_SCALE = 0.0002f;
extern const float CAMERA_VELOCITY_FRICTION = 0.985f;
extern const float CAMERA_START_ZOOM = 0.01f;
extern const float CAMERA_START_ZOOM_TARGET = 0.01f;

// Simulation settings
extern const float SIMULATION_UNIFORM_GRID_DIMENSION_BUFFER = 5.0f;
extern const int SIMULATION_UNIFORM_GRID_TILE_CREATURE_CAPACITY_SCALAR = 5;
extern const float SIMULATION_UNIFORM_GRID_TILE_SIZE_SCALAR = 10.0; // This should increase the more creatures are tightly packed

// Render settings
extern const uint16_t RENDER_NUM_OF_CREATURE_BODY_VERTICES = 60;

// Creature misc settings
extern const float CREATURE_DEFAULT_BODY_MASS = 0.01;

// Creature brain settings
extern const uint16_t CREATURE_BRAIN_NUM_OF_TEMPORALS = 3;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_MIDLEVELS = 1;
extern const uint16_t CREATURE_BRAIN_MAX_NUM_OF_NODES_IN_MIDLEVEL = 30;

extern const float CREATURE_BRAIN_LINK_WEIGHT_MAX_ABS = 10.0;
extern const float CREATURE_BRAIN_BIAS_MAX_ABS = 10.0;
extern const float CREATURE_BRAIN_ACTIVATION_EXPONENT_MAX_ABS = 10.0;

// Creature firstgen brain settings
extern const float CREATURE_BRAIN_FIRSTGEN_LINK_WEIGHT_CHANCE_FOR_NON_NEUTRAL_VALUE = 0.075;
extern const float CREATURE_BRAIN_FIRSTGEN_LINK_WEIGHT_MAX_ABS = 5.0;
extern const float CREATURE_BRAIN_FIRSTGEN_LINK_WEIGHT_ZERO_GRAVITY = 1.0;

extern const float CREATURE_BRAIN_FIRSTGEN_ACTIVATION_EXPONENT_CHANCE_FOR_NON_NEUTRAL_VALUE = 1.0;
extern const float CREATURE_BRAIN_FIRSTGEN_ACTIVATION_EXPONENT_MAX_ABS = 2.0;
extern const float CREATURE_BRAIN_FIRSTGEN_ACTIVATION_EXPONENT_ZERO_GRAVITY = 1.5;

extern const float CREATURE_BRAIN_FIRSTGEN_BIAS_CHANCE_FOR_NON_NEUTRAL_VALUE = 0.1;
extern const float CREATURE_BRAIN_FIRSTGEN_BIAS_MAX_ABS = 1.0;
extern const float CREATURE_BRAIN_FIRSTGEN_BIAS_ZERO_GRAVITY = 3.0;


// Creature brain mutation settings
extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_MIN_CHANCE = 0.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_MAX_CHANCE = 0.25;
extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_CHANCE_MIN_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_MIN_STRENGTH = 0.01;
extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_MAX_STRENGTH = 2.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_STRENGTH_MIN_GRAVITY = 1.0;

extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_MIN_ZERO_GRAVITY = 1.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_MAX_ZERO_GRAVITY = 2.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_LINK_WEIGHT_ZERO_GRAVITY_MIN_GRAVITY = 1.0;


extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_MIN_CHANCE = 0.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_MAX_CHANCE = 0.25;
extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_CHANCE_MIN_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_MIN_STRENGTH = 0.01;
extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_MAX_STRENGTH = 1.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_STRENGTH_MIN_GRAVITY = 1.0;

extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_MIN_ZERO_GRAVITY = 1.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_MAX_ZERO_GRAVITY = 2.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_BIAS_ZERO_GRAVITY_MIN_GRAVITY = 1.0;


extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_MIN_CHANCE = 0.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_MAX_CHANCE = 0.25;
extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_CHANCE_MIN_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_MIN_STRENGTH = 0.01;
extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_MAX_STRENGTH = 1.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_STRENGTH_MIN_GRAVITY = 1.0;

extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_MIN_ZERO_GRAVITY = 1.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_MAX_ZERO_GRAVITY = 2.0;
extern const float CREATURE_MUTATION_BRAIN_JITTER_ACTIVATION_EXPONENT_ZERO_GRAVITY_MIN_GRAVITY = 1.0;


// Creature body mutation settings
extern const float CREATURE_MUTATION_DEVICE_JITTER_SPIKE_ANGLE_CHANCE = 0.01;
extern const float CREATURE_MUTATION_DEVICE_JITTER_SPIKE_ANGLE_STRENGTH = 1.0;
extern const float CREATURE_MUTATION_DEVICE_JITTER_SPIKE_ANGLE_ZERO_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_DEVICE_JITTER_SHIELD_ANGLE_CHANCE = 0.01;
extern const float CREATURE_MUTATION_DEVICE_JITTER_SHIELD_ANGLE_STRENGTH = 1.0;
extern const float CREATURE_MUTATION_DEVICE_JITTER_SHIELD_ANGLE_ZERO_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_DEVICE_JITTER_FEEDER_ANGLE_CHANCE = 0.01;
extern const float CREATURE_MUTATION_DEVICE_JITTER_FEEDER_ANGLE_STRENGTH = 1.0;
extern const float CREATURE_MUTATION_DEVICE_JITTER_FEEDER_ANGLE_ZERO_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_DEVICE_JITTER_ADHESION_ANGLE_CHANCE = 0.01;
extern const float CREATURE_MUTATION_DEVICE_JITTER_ADHESION_ANGLE_STRENGTH = 1.0;
extern const float CREATURE_MUTATION_DEVICE_JITTER_ADHESION_ANGLE_ZERO_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_MEAT_PRIORITY_JITTER_CHANCE = 0.01;
extern const float CREATURE_MUTATION_MEAT_PRIORITY_JITTER_STRENGTH = 1.0;
extern const float CREATURE_MUTATION_MEAT_PRIORITY_JITTER_ZERO_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_BODY_JITTER_SKIN_PATTERN_CHANCE = 0.01;
extern const float CREATURE_MUTATION_BODY_JITTER_SKIN_PATTERN_STRENGTH = 0.1;
extern const float CREATURE_MUTATION_BODY_JITTER_SKIN_PATTERN_ZERO_GRAVITY = 10.0;

extern const float CREATURE_MUTATION_BODY_JITTER_SKIN_HUE_CHANCE = 0.01;
extern const float CREATURE_MUTATION_BODY_JITTER_SKIN_HUE_STRENGTH = 0.1;
extern const float CREATURE_MUTATION_BODY_JITTER_SKIN_HUE_ZERO_GRAVITY = 10.0;



// Creature eye settings
extern const uint16_t CREATURE_EYE_NUM_OF_CONES = 32;
extern const uint16_t CREATURE_EYE_NUM_OF_VALUES_IN_SINGLE_CONE = 1;
extern const uint16_t CREATURE_EYE_NUM_OF_PUPIL_VALUES = 12;


//////////////////////////////
// -- TWEAKABLE SETTINGS -- //
//////////////////////////////


// UI settings
extern TweakableFloatSetting UI_CREATURE_TRACKER_CAMERA_ZOOM_BASED_FOLLOW_INTERPOLATION_RATE = { 0.5, 0.001, 1.0 };

// Simulation space settings
extern TweakableFloatSetting SIMULATION_SPACE_WIDTH = { 250, 1, 2000 };
extern TweakableFloatSetting SIMULATION_SPACE_HEIGHT = { 250, 1, 2000 };

// Simulation physics settings
extern TweakableFloatSetting SIMULATION_BORDER_RESTITUTION = { 0.0, 0.0, 1.0 };
extern TweakableFloatSetting SIMULATION_VELOCITY_DOWNSCALE = { 0.99, 0.0, 1.0 };
extern TweakableFloatSetting SIMULATION_ANGLE_VELOCITY_DOWNSCALE = { 0.975, 0.0, 1.0 };

// Evolution incubation settings
extern TweakableIntegerSetting TRAINING_WHEELS_GENERATION_UPPER_THRESHOLD = { 10, 0, 100 };
extern TweakableFloatSetting TRAINING_WHEELS_OFFSPRING_COUNT_SCORE_WEIGHT = { 0.5, 0.0, 1.0 };
extern TweakableFloatSetting TRAINING_WHEELS_GENERATION_SCORE_WEIGHT = { 0.05, 0.0, 1.0 };

// Simulation creature settings
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_CONSTANT_SPAWN_RATE = { 0.0, 0.0, 50.0 };

extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_RATE = { 25.0, 0.0, 10000.0 };
extern TweakableIntegerSetting SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_NUM_OF_CREATURES_UPPER_TARGET = { 12000, 0, 10000 };
extern TweakableIntegerSetting SIMULATION_FIRSTGEN_CREATURE_PULSE_SPAWN_NUM_OF_CREATURES_LOWER_TARGET = { 500, 0, 10000 };

extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_UP_SPAWN_RATE = { 0.0, 0.0, 100.0 };
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_DOWN_SPAWN_RATE = { 0.0, 0.0, 100.0 };
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_STEP = { 0.00001, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_OSCILLATE_SPAWN_RATE_EXPONENT = { 1.0, 0.0, 10.0 };

extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_MOVING_SPAWN_RANDOM_VELOCITY_MAGNITUDE = { 1.00, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_MOVING_SPAWN_MAX_VELOCITY_LENGTH = { 0.00, 0.0, 10.0 };


// Render settings
extern TweakableFloatSetting RENDER_CLEAR_COLOR_R = { 0.035, 0.0, 1.0 };
extern TweakableFloatSetting RENDER_CLEAR_COLOR_G = { 0.035, 0.0, 1.0 };
extern TweakableFloatSetting RENDER_CLEAR_COLOR_B = { 0.035, 0.0, 1.0 };

extern TweakableFloatSetting RENDER_CREATURE_TRANSPARENCY_ENERGY_EXPONENT = { 0.5, 0.0, 1.0 };

// Creature body settings
extern TweakableFloatSetting CREATURE_MAX_RADIUS = { 0.4, 0.3, 0.5 };
extern TweakableFloatSetting CREATURE_MIN_RADIUS = { 0.1, 0.1, 0.3 };

extern TweakableFloatSetting CREATURE_RADIUS_INTERPOLATION_RATE = { 0.1, 0.001, 1.0 };
extern TweakableFloatSetting CREATURE_RADIUS_PERCENTAGE_MEAT_WEIGHT = { 0.7, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_RADIUS_PERCENTAGE_MUSCLE_WEIGHT = { 0.3, 0.0, 1.0 };

extern TweakableFloatSetting CREATURE_MAX_HARDNESS = { 2.0, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_MIN_HARDNESS = { 0.15, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_HARDNESS_INTERPOLATION_RATE = { 0.1, 0.001, 1.0 };

extern TweakableFloatSetting CREATURE_MAX_SKIN_LIGHTNESS = { 0.85, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_MIN_SKIN_LIGHTNESS = { 0.15, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_SKIN_LIGHTNESS_INTERPOLATION_RATE = { 0.025, 0.001, 1.0 };

extern TweakableFloatSetting CREATURE_MAX_SKIN_SATURATION = { 1.0, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_MIN_SKIN_SATURATION = { 0.2, 0.0, 1.0 };

// Creature energy/meat/life settings
extern TweakableFloatSetting CREATURE_MAX_ENERGY = { 5.0, 0.1, 50.0 };
extern TweakableFloatSetting CREATURE_MAX_MEAT = { 100.0, 0.1, 100000.0 };
extern TweakableFloatSetting CREATURE_MAX_LIFE = { 1.0, 0.1, 10.0 };

extern TweakableFloatSetting CREATURE_ENERGY_TO_MEAT_CONVERSION_RATE = { 0.085, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_MEAT_TO_ENERGY_CONVERSION_RATE = { 0.0001, 0.0, 1.0 }; // Consider constant energy drain
extern TweakableFloatSetting CREATURE_ENERGY_TO_LIFE_CONVERSION_RATE = { 0.005, 0.0, 1.0 };

extern TweakableFloatSetting CREATURE_LIFE_DRAIN_ON_NO_ENERGY = { 0.0025, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_ENERGY_DRAIN_CONSTANT = { 0.0001, 0.0, 1.0 }; // Consider meat to energy conversion rate

extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_ENERGY = { 2.0, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_MEAT = { 100.0, 0.0, 100000.0 };
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_LIFE = { 1.0, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_RADIUS = { 0.4, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_FIRSTGEN_CREATURE_INITIAL_HARDNESS = { 0.0, 0.0, 10.0 };

// Offspring initial energy + meat should NOT be more than reproduction energy cost for an energy balanced system, and for evolution to occur!
// Moreso, keeping offspring initial energy + meat + life values BELOW reproduction energy cost would discourage creatures to utilize the reproduce-quickly-die-quickly technique!
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_ENERGY = { 2.5, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_MEAT = { 0.0, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_LIFE = { 1.0, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_RADIUS = { 0.01, 0.0, 10.0 };
extern TweakableFloatSetting SIMULATION_OFFSPRING_CREATURE_INITIAL_HARDNESS = { 0.0, 0.0, 10.0 };

// Creature energy costs
extern TweakableFloatSetting CREATURE_REPRODUCTION_ENERGY_COST = { 2.5, 0.0, 5.0 };
extern TweakableFloatSetting CREATURE_REPRODUCTION_ENERGY_THRESHOLD = { 2.5, 0.0, 5.0 };
extern TweakableFloatSetting CREATURE_REPRODUCTION_MEAT_THRESHOLD = { 0.0, 0.0, 5.0 };


extern TweakableFloatSetting CREATURE_FORWARD_MOVEMENT_ENERGY_COST = { 0.0005, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_BACKWARD_MOVEMENT_ENERGY_COST = { 0.0005, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_STRAFE_MOVEMENT_ENERGY_COST = { 0.0005, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_TURN_MOVEMENT_ENERGY_COST = { 0.0002, 0.0, 1.0 };

extern TweakableFloatSetting CREATURE_DEVICE_FEEDER_ENERGY_COST = { 0.00002, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEVICE_SPIKE_ENERGY_COST = { 0.00002, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEVICE_SHIELD_ENERGY_COST = { 0.00001, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEVICE_ADHESION_EFFECTIVENESS = { 0.01, 0.0, 1.0 };

extern TweakableFloatSetting CREATURE_DEVICE_FEEDER_EFFECTIVENESS = { 0.1, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEVICE_SPIKE_VERSUS_LIFE_EFFECTIVENESS = { 0.01, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEVICE_SPIKE_VERSUS_MEAT_EFFECTIVENESS = { 0.09, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEVICE_SHIELD_EFFECTIVENESS = { 1.0, 0.0, 1.0 };


extern TweakableFloatSetting CREATURE_MEAT_CONSUMPTION_EFFECTIVENESS_MULTIPLIER_ON_PRIORITY_MATCH = { 1.0, -1.0, 1.0 };
extern TweakableFloatSetting CREATURE_MEAT_CONSUMPTION_EFFECTIVENESS_MULTIPLIER_ON_PRIORITY_MISMATCH = { -0.5, -1.0, 1.0 };
extern TweakableFloatSetting CREATURE_MEAT_CONSUMPTION_EFFECTIVENESS_EXPONENT_ON_PRIORITY_MATCH = { 0.2, 0.0, 2.0 };
extern TweakableFloatSetting CREATURE_MEAT_CONSUMPTION_EFFECTIVENESS_EXPONENT_ON_PRIORITY_MISMATCH = { 2.0, 0.0, 2.0 }; // Make sure this doesn't negate the value!

extern TweakableFloatSetting CREATURE_FORWARD_MOVEMENT_EFFECTIVENESS = { 0.0004, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_BACKWARD_MOVEMENT_EFFECTIVENESS = { 0.0001, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_STRAFE_MOVEMENT_EFFECTIVENESS = { 0.0001, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_TURN_MOVEMENT_EFFECTIVENESS = { 0.025, 0.0, 1.0 };



// Creature device & reproduction
extern TweakableFloatSetting CREATURE_ENERGY_PERCENTAGE_BASED_MOVEMENT_MULTIPLIER_EXPONENT = { 0.05, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_ENERGY_PERCENTAGE_BASED_DEVICE_STATE_MULTIPLIER_EXPONENT = { 0.1, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEVICE_STATE_INTERPOLATION_RATE = { 0.1, 0.001, 1.0 };
extern TweakableFloatSetting CREATURE_EYE_CONTROLS_INTERPOLATION_RATE = { 0.1, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEVICE_AIM_DOT_THRESHOLD = { 0.9, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_REPRODUCTION_AIM_DOT_THRESHOLD = { 0.9, 0.0, 1.0 };


// For normalization purposes
extern TweakableFloatSetting CREATURE_MAX_VELOCITY_LENGTH = { 0.4, 0.1, 10.0 };
extern TweakableFloatSetting CREATURE_MAX_ANGLE_VELOCITY_LENGTH = { 0.1, 0.1, 10.0 };

// Creature death settings
extern TweakableFloatSetting CREATURE_DEATH_MEAT_ROT_RATE = { 0.01, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEATH_WITHOUT_MEAT_SHRINK_RATE = { 0.001, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEATH_EXISTENCE_RADIUS_THRESHOLD = { 0.0025, 0.0, 1.0 };

extern TweakableFloatSetting CREATURE_DEATH_HARDNESS_TARGET = { 2.0, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEATH_HARDNESS_INTERPOLATION_RATE = { 0.05, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEATH_DEVICE_ZEROFICATION_INTERPOLATION_RATE = { 0.1, 0.0, 1.0 };

extern TweakableFloatSetting CREATURE_DEATH_ENERGY_ZEROFICATION_INTERPOLATION_RATE = { 0.01, 0.0, 1.0 };

extern TweakableFloatSetting CREATURE_DEATH_SKIN_LIGHTNESS_TARGET = { 0.2, 0.0, 1.0 };
extern TweakableFloatSetting CREATURE_DEATH_SKIN_LIGHTNESS_INTERPOLATION_RATE = { 0.001, 0.0, 1.0 };



// Creature body mutation settings

// Creature eye settings
extern TweakableFloatSetting CREATURE_EYE_MAX_PROBE_DISTANCE = { 12.0, 0.1, 100.0 };
extern TweakableFloatSetting CREATURE_EYE_MAX_CONES_RADIUS = { 5.0, 0.1, 10.0 };



///////////////////////////////////////////
// -- ASSISTING & SUBSEQUENT SETTINGS -- //
///////////////////////////////////////////

// These should not be edited lightly

// Eye settings
extern const uint16_t CREATURE_EYE_NUM_OF_CONES_VALUES = CREATURE_EYE_NUM_OF_CONES * CREATURE_EYE_NUM_OF_VALUES_IN_SINGLE_CONE;

// Calculate buffer size extern constants (used to define the size of each structure/node/link creature attributes)
extern const uint16_t CREATURE_BRAIN_NUM_OF_INPUTS = 6 + CREATURE_EYE_NUM_OF_PUPIL_VALUES + CREATURE_EYE_NUM_OF_CONES_VALUES + CREATURE_BRAIN_NUM_OF_TEMPORALS;

extern const uint16_t CREATURE_BRAIN_NUM_OF_OUTPUTS = 16 + CREATURE_BRAIN_NUM_OF_TEMPORALS;

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


// The max number of colliders is the maximum amount of circles of minimal radius that can simultaneously touch a single circle of maximal radius
extern const GLuint CREATURE_MAX_NUM_OF_COLLIDERS = floor(M_PI / asin(CREATURE_MIN_RADIUS.min / (CREATURE_MAX_RADIUS.max + CREATURE_MIN_RADIUS.min)));


// UI settings
extern const float UI_CREATURE_TRACKER_BRAIN_LINK_NEGATIVE_VALUE_COLOR_R = float(UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_R) / 255.0f;
extern const float UI_CREATURE_TRACKER_BRAIN_LINK_NEGATIVE_VALUE_COLOR_G = float(UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_G) / 255.0f;
extern const float UI_CREATURE_TRACKER_BRAIN_LINK_NEGATIVE_VALUE_COLOR_B = float(UI_CREATURE_TRACKER_BRAIN_NEGATIVE_VALUE_COLOR_B) / 255.0f;

extern const float UI_CREATURE_TRACKER_BRAIN_LINK_POSITIVE_VALUE_COLOR_R = float(UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_R) / 255.0f;
extern const float UI_CREATURE_TRACKER_BRAIN_LINK_POSITIVE_VALUE_COLOR_G = float(UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_G) / 255.0f;
extern const float UI_CREATURE_TRACKER_BRAIN_LINK_POSITIVE_VALUE_COLOR_B = float(UI_CREATURE_TRACKER_BRAIN_POSITIVE_VALUE_COLOR_B) / 255.0f;


//////////////////////////////////
// -- SETTING VALUES ASSERTS -- //
//////////////////////////////////

void Settings_AssertValuesValidity()
{
	// @TODO: balanced energy system
}




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