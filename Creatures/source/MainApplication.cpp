#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_NONE
#include "UserControls.h"

#include "utils/ShapeUtils.h"
#include "utils/OpenGLErrorDebugger.h"

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
	const int startingWidth = 1200;
	const int startingHeight = 1200;

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


	//////////////////////////////////
	// -- DEAR IMGUI SETUP START -- //
	//////////////////////////////////

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark(); 

	// Setup Dear ImGui platform/renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	// Load Dear ImGui fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	////////////////////////////////
	// -- DEAR IMGUI SETUP END -- //
	////////////////////////////////


	// Setup viewport
	glfwSetFramebufferSizeCallback(window, glfwFrameBufferSizeCallback);
	glfwFrameBufferSizeCallback(window, startingWidth, startingHeight);

	// Intialize input callbacks
	glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetScrollCallback(window, glfw_scroll_callback);

	   
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

	bool show_demo_window = true;


	// Start main loop
	while (!glfwWindowShouldClose(window))
	{

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		ImGui::ShowDemoWindow(&show_demo_window);


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

		// Update
		Camera_Update(timeDiff);
		Simulation_Update();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



		glfwSwapBuffers(window);

	}

	glfwTerminate();
	return 0;
}
