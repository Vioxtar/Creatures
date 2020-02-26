#include "UserInterface.h"
#include "CreatureDataInterface.h"
#include "CreatureTracking.h"

int currWindowWidth;
int currWindowHeight;

//////////////////////////////
// -- SPACE TRANSLATIONS -- //
//////////////////////////////

vec2 ViewportSpaceToCameraSpace(vec2 pos)
{
	float halfWidth = currWindowWidth / 2;
	float halfHeight = currWindowHeight / 2;

	pos -= vec2(halfWidth, halfHeight);
	pos.x /= halfWidth;
	pos.y /= -halfHeight;
	
	return pos;
}

vec2 CameraSpaceToSimulationSpace(vec2 pos)
{
	mat4 simSpaceToCameraSpace = GetSimSpaceToCameraTransform();
	mat4 cameraSpaceToSimSpace = inverse(simSpaceToCameraSpace);
	vec4 simPosition = cameraSpaceToSimSpace * vec4(pos.x, pos.y, 1.0, 1.0);
	return simPosition;
}

vec2 ViewportSpaceToSimulationSpace(vec2 pos)
{
	return CameraSpaceToSimulationSpace(ViewportSpaceToCameraSpace(pos));
}

///////////////////////////////////////////
// -- DEAR IMGUI / GLFW INIT & UPDATE -- //
///////////////////////////////////////////

void UserInterface_Init(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO imguiIO = ImGui::GetIO(); (void)imguiIO;

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

}

void UserInterface_PreUpdate()
{

	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When imguiIO.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	// - When imguiIO.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

bool show_demo_window = true;
void UserInterface_Update()
{
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	UpdateCreatureTrackers();
}

void UserInterface_PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


////////////////////////////
// -- INPUT MANAGEMENT -- //
////////////////////////////

bool DearImGuiUsingMouse()
{
	return ImGui::GetIO().WantCaptureMouse;
}

bool DearImGuiUsingKeyboard()
{
	return ImGui::GetIO().WantCaptureKeyboard;
}



////////////////
// -- MISC -- //
////////////////

bool mousePressed = false;
double currMouseXPos, currMouseYPos = 0;

vec2 GetMouseSimPos()
{
	return ViewportSpaceToSimulationSpace(vec2(currMouseXPos, currMouseYPos));
}


bool SelectCreatureByDistanceToMouse(unsigned int& selectedCreature)
{
	// Find which creature is closest to our mouse
	vector<vec2> creaturePositions = GetCreaturePositions();
	
	vec2 mouseSimPos = GetMouseSimPos();
	
	float minSqrdDist = UI_MAX_CREATURE_SELECTION_SQUARED_DISTANCE;
	bool found = false;
	for (unsigned int i = 0; i < creaturePositions.size(); i++)
	{
		vec2 creaturePos = creaturePositions.data()[i];
		vec2 diff = creaturePos - mouseSimPos;
		float sqrdDist = dot(diff, diff);
		if (sqrdDist < minSqrdDist)
		{
			minSqrdDist = sqrdDist;
			selectedCreature = i;
			found = true;
		}
	}

	return found;
}


//////////////////////////
// -- GLFW CALLBACKS -- //
//////////////////////////

void glfw_frame_buffer_size_callback(GLFWwindow*, int width, int height)
{
	currWindowWidth = width;
	currWindowHeight = height;

	int maxDimension = std::max(currWindowWidth, currWindowHeight);
	glViewport(0, 0, maxDimension, maxDimension);
}


void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//if (key == GLFW_KEY_E && action == GLFW_PRESS)
	//{
		
	//}
}


void glfw_cursor_position_callback(GLFWwindow* window, double x, double y)
{
	if (DearImGuiUsingMouse()) return;

	if (mousePressed)
	{
		vec2 oldMouseSimPos = ViewportSpaceToSimulationSpace(vec2(currMouseXPos, currMouseYPos));
		vec2 newMouseSimPos = ViewportSpaceToSimulationSpace(vec2(x, y));
		vec2 offset = newMouseSimPos - oldMouseSimPos;
		Camera_Move(offset);
	}

	currMouseXPos = x;
	currMouseYPos = y;
}

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (DearImGuiUsingMouse()) return;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		mousePressed = action == GLFW_PRESS;
		Camera_Enable_Glide(!mousePressed);
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		unsigned int index;
		bool creatureClicked = SelectCreatureByDistanceToMouse(index);

		if (creatureClicked)
		{
			TrackCreature(index);
		}
	}
}

void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (DearImGuiUsingMouse()) return;

	Camera_Zoom(yOffset);
}