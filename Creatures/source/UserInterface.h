#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include "utils/MathUtils.h"
#include "simulation/Camera.h"
#include "simulation/Simulation.h"
#include <glm/glm.hpp>

#ifndef GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

using namespace std;

void UserInterface_Init(GLFWwindow* window);

void UserInterface_PreUpdate();

void UserInterface_Update();

void UserInterface_PostUpdate();


void glfw_frame_buffer_size_callback(GLFWwindow*, int width, int height);

void glfw_cursor_position_callback(GLFWwindow* window, double x, double y);

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);