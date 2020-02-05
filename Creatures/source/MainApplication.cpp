#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_NONE
#include "UserControls.h"

#include "utils/ShapeUtils.h"
#include "utils/OpenGLErrorDebugger.h"

#include "simulation/Settings.h"
#include "simulation/Simulation.h"
#include "simulation/Camera.h"


#define ENABLE_DEBUG



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
	const int startingWidth = 500;
	const int startingHeight = 500;

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
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	glfw_framebuffer_size_callback(window, startingWidth, startingHeight);

	// Intialize input callbacks
	glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetScrollCallback(window, glfw_scroll_callback);


	// Intiailize settings, simulation and camera
	SimulationSettings_Init();
	Simulation_Init();
	Camera_Init();


	// @DEBUG
	double lastTime = 0;
	int frameCount = 0;

	// Start main loop
	while (!glfwWindowShouldClose(window))
	{

		// Clear
		glClearColor(0.0f, 0.0f, 0.0f, 0.001f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update
		Simulation_Update();
		Camera_Update();

		glfwSwapBuffers(window);
		glfwPollEvents();

		// @DEBUG
		float currentTime = glfwGetTime();
		frameCount++;
		if ((currentTime - lastTime) >= 1.0)
		{
			cout << "FPS: " << frameCount << endl;
			frameCount = 0;
			lastTime = currentTime;
		}
	}

	glfwTerminate();
	return 0;
}
