#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_NONE
#include "ui/UserInterface.h"

#include "utils/ShapeUtils.h"
#include "utils/OpenGLErrorDebugger.h"

#include "Settings.h"

#include "simulation/Simulation.h"
#include "simulation/Camera.h"


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
	glfwSetFramebufferSizeCallback(window, glfw_frame_buffer_size_callback);
	glfw_frame_buffer_size_callback(window, startingWidth, startingHeight);

	// Intialize input callbacks
	glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetScrollCallback(window, glfw_scroll_callback);

	// Initialize user interface
	UserInterface_Init(window);

	// Intiailize simulation and camera
	Simulation_Init();
	Camera_Init();

	// Try and disable microstutters with V-Sync off
	glfwSwapInterval(0);

	// @DEBUG
	GLint maxWorkGroups;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroups);
	cout << "Max number of workgroups: " << maxWorkGroups << endl;
	GLint maxWorkGroupInvocations;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxWorkGroupInvocations);
	cout << "Max number of workgroup invocations: " << maxWorkGroupInvocations << endl;
	
	// @DEBUG
	double lastTime = 0;
	int frameCount = 0;


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
		UserInterface_PreUpdate();
		
		// Update camera, simulation and user interface
		Camera_Update(timeDiff);
		Simulation_Update();
		UserInterface_Update();

		// Render user interface
		UserInterface_PostUpdate();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
