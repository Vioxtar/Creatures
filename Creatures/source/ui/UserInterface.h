#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "../utils/MathUtils.h"
#include "Camera.h"
#include "../simulation/Simulation.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#ifndef GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

using namespace std;
using namespace glm;


namespace UserInterface
{
	///////////////////////////////////////////
	// -- DEAR IMGUI / GLFW INIT & UPDATE -- //
	///////////////////////////////////////////

	void Initialize(GLFWwindow* window);

	void PreUpdate();

	void Update();

	void PostUpdate();

	//////////////////////////
	// -- GLFW CALLBACKS -- //
	//////////////////////////

	void glfw_frame_buffer_size_callback(GLFWwindow*, int width, int height);

	void glfw_cursor_position_callback(GLFWwindow* window, double x, double y);

	void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

	void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	//////////////////////////////
	// -- SPACE TRANSLATIONS -- //
	//////////////////////////////

	vec2 ViewportSpaceToSimulationSpace(vec2 pos);
	vec2 SimulationSpaceToViewportSpace(vec2 pos);
	float SimulationScaleToViewportScale(float scaleVal);
}
