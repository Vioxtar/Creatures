#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_NONE
#include "ui/UserInterface.h"

#include "utils/ShapeUtils.h"
#include "utils/OpenGLErrorDebugger.h"

#include "Settings.h"

#include "simulation/Renderer.h"
#include "simulation/Simulation.h"
#include "ui/Camera.h"


//#define ENABLE_DEBUG


int main()
{
	///////////////////////////////////
	// -- BOILER PLATE CODE START -- //
	///////////////////////////////////

	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// @DEBUG Set debug hint (comment this out in non-debug configuration)
#ifdef ENABLE_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	// Starting width and height parameters
	const int startingWidth = WINDOW_STARTING_WIDTH;
	const int startingHeight = WINDOW_STARTING_HEIGHT;

	//glfwWindowHint(GLFW_SAMPLES, WINDOW_RENDER_MULTISAMPLES);

	// Create window object
	GLFWwindow* window = glfwCreateWindow(startingWidth, startingHeight, "Creatures", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// @DEBUG Initialize debugging callbacks @DEBUG
#ifdef ENABLE_DEBUG
	OpenGLErrorDebuggerInitialize();
#endif

	/////////////////////////////////
	// -- BOILER PLATE CODE END -- //
	/////////////////////////////////

	// Setup viewport
	glfwSetFramebufferSizeCallback(window, UserInterface::glfw_frame_buffer_size_callback);
	UserInterface::glfw_frame_buffer_size_callback(window, startingWidth, startingHeight);

	// Intialize input callbacks
	glfwSetCursorPosCallback(window, UserInterface::glfw_cursor_position_callback);
	glfwSetKeyCallback(window, UserInterface::glfw_key_callback);
	glfwSetMouseButtonCallback(window, UserInterface::glfw_mouse_button_callback);
	glfwSetScrollCallback(window, UserInterface::glfw_scroll_callback);

	// Initialize user interface
	UserInterface::Initialize(window);

	// Assert validity of settings' values
	Settings_AssertValuesValidity();

	// Intiailize simulation and renderer
	Simulation::Initialize();
	Renderer::Initialize();

	// Try and disable microstutters with V-Sync off
	glfwSwapInterval(0);

	// @DEBUG
	GLint maxWorkGroups;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroups);
	cout << "Max number of workgroups: " << maxWorkGroups << endl;
	GLint maxWorkGroupInvocations;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxWorkGroupInvocations);
	cout << "Max number of workgroup invocations: " << maxWorkGroupInvocations << endl;
	GLint maxSSBOBinds;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBinds);
	cout << "Max number of SSBO binds: " << maxSSBOBinds << endl;
	GLint maxComputeStorageBlocks;
	glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &maxComputeStorageBlocks);
	cout << "maxComputeStorageBlocks: " << maxComputeStorageBlocks << endl;
	GLint maxVertexStorageBlocks;
	glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &maxVertexStorageBlocks);
	cout << "maxVertexStorageBlocks: " << maxVertexStorageBlocks << endl;

	// @DEBUG
	double lastTime = 0;
	int frameCount = 0;

	/*
		make pupil not fuck up cones
		add grid
		scan for other temp shit
		research and implement lstms.Yes, that's right.
	*/

	// Start main loop
	while (!glfwWindowShouldClose(window))
	{
		// @DEBUG
		double currentTime = glfwGetTime();
		frameCount++;
		if ((currentTime - lastTime) >= 1.0)
		{
			cout << "FPS: " << frameCount << endl;
			frameCount = 0;
			lastTime = currentTime;
		}
		double timeDiff = currentTime - lastTime;


		// Clear last frame
		glClearColor(
			RENDER_CLEAR_COLOR_R.value,
			RENDER_CLEAR_COLOR_G.value,
			RENDER_CLEAR_COLOR_B.value,
			1.0f
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Poll input events and start a new frame
		UserInterface::PreUpdate();
		
		// Update camera, simulation and user interface
		Camera::Update(timeDiff);
		Simulation::Update();
		Renderer::Render();
		UserInterface::Update();

		// Render user interface
		UserInterface::PostUpdate();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
