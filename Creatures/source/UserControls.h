#pragma once

#include <iostream>
#include "utils/MathUtils.h"
#include "simulation/Camera.h"
#include "simulation/Simulation.h"
#include <glm/glm.hpp>

#ifndef GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

using namespace std;

void glfw_framebuffer_size_callback(GLFWwindow*, int width, int height);

void glfw_cursor_position_callback(GLFWwindow* window, double x, double y);

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
