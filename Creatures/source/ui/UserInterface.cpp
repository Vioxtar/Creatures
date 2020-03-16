#include "UserInterface.h"
#include "CreatureDataInterface.h"
#include "CreatureTracking.h"

int currWindowWidth;
int currWindowHeight;

// Used for space translations
float widthDiff;
float heightDiff;

float widthScale;
float heightScale;

//////////////////////////////
// -- SPACE TRANSLATIONS -- //
//////////////////////////////

vec2 ViewportSpaceToCameraSpace(vec2 pos)
{
	float halfWidth = widthScale * currWindowWidth / 2;
	float halfHeight = heightScale * currWindowHeight / 2;

	pos -= vec2(halfWidth - widthDiff, halfHeight - heightDiff);
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

vec2 CameraSpaceToViewportSpace(vec2 pos)
{
	float halfWidth = widthScale * currWindowWidth / 2;
	float halfHeight = heightScale * currWindowHeight / 2;

	pos.x *= halfWidth;
	pos.y *= -halfHeight;

	pos += vec2(halfWidth - widthDiff, halfHeight - heightDiff);

	return pos;
}
vec2 SimulationSpaceToCameraSpace(vec2 pos)
{
	mat4 simSpaceToCameraSpace = GetSimSpaceToCameraTransform();
	vec4 camPosition = simSpaceToCameraSpace * vec4(pos.x, pos.y, 1.0, 1.0);
	return camPosition;
}
vec2 SimulationSpaceToViewportSpace(vec2 pos)
{
	return CameraSpaceToViewportSpace(SimulationSpaceToCameraSpace(pos));
}


float SimulationScaleToViewportScale(float scaleVal)
{
	// Scale factor is the distance between the viewport equivalents of (1,0) and (0,0)
	vec2 simOne = vec2(1, 0);
	vec2 simZero = vec2(0, 0);

	vec2 viewportOne = SimulationSpaceToViewportSpace(simOne);
	vec2 viewportZero = SimulationSpaceToViewportSpace(simZero);

	return abs(viewportOne.x - viewportZero.x) * scaleVal;
}


///////////////////
// -- STYLING -- //
///////////////////

unsigned int stylePushes = 0;
void DearImGuiPushMainStyle()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, UI_DEARIMGUI_STYLE_FRAME_ROUNDING); stylePushes++;
}

void DearImGuiPopMainStyle()
{
	ImGui::PopStyleVar(stylePushes);
	stylePushes = 0;
}

///////////////////////////////////////////
// -- DEAR IMGUI / GLFW INIT & UPDATE -- //
///////////////////////////////////////////

void UserInterface_Init(GLFWwindow* window)
{
	// Initialize camera and creature tracking
	Camera_Init();
	CreatureTracking_Init();

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

bool show_demo_window = true; // @DEBUG
void UserInterface_Update()
{
	// @DEBUG
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	DearImGuiPushMainStyle();

	UpdateCreatureTrackers();

	DearImGuiPopMainStyle();
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

bool lmbPressed = false;
bool rmbPressed = false;
double currMouseXPos, currMouseYPos = 0;

vec2 GetMouseSimPos()
{
	return ViewportSpaceToSimulationSpace(vec2(currMouseXPos, currMouseYPos));
}


bool SelectCreatureByDistanceToMouse(CreatureUniqueID& selectedCreatureID)
{
	// Find which creature is closest to our mouse
	vector<vec2> creaturePositions = GetSSBOIndexedCreaturePositions();
	
	vec2 mouseSimPos = GetMouseSimPos();
	
	float minSqrdDist = UI_MAX_CREATURE_SELECTION_SQUARED_DISTANCE;
	unsigned int closestCreatureSSBOIndex;
	bool found = false;
	for (unsigned int i = 0; i < creaturePositions.size(); i++)
	{
		vec2 creaturePos = creaturePositions.data()[i];
		vec2 diff = creaturePos - mouseSimPos;
		float sqrdDist = dot(diff, diff);
		if (sqrdDist < minSqrdDist)
		{
			minSqrdDist = sqrdDist;
			closestCreatureSSBOIndex = i;
			found = true;
		}
	}

	if (found)
	{
		selectedCreatureID = CreatureData_CreatureSSBOIndexToUniqueID(closestCreatureSSBOIndex);
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

	// Used to avoid stretching
	int maxDimension = std::max(currWindowWidth, currWindowHeight);

	// Used to keep things offset away from the center of the window
	widthDiff = maxDimension - currWindowWidth;
	heightDiff = maxDimension - currWindowHeight;
	widthDiff *= 0.5;
	heightDiff *= 0.5;

	glViewport(-widthDiff, -heightDiff, maxDimension, maxDimension);

	// Ratios to stretch space translations
	widthScale = float(maxDimension) / float(currWindowWidth);
	heightScale = float(maxDimension) / float(currWindowHeight);
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

	if (rmbPressed)
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
	// Regardless of DearImGui, update our own mousePressed variables
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		lmbPressed = action == GLFW_PRESS;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		rmbPressed = action == GLFW_PRESS;
	}

	if (DearImGuiUsingMouse()) return;

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		Camera_Enable_Glide(!rmbPressed);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		CreatureUniqueID creatureID;
		bool creatureClicked = SelectCreatureByDistanceToMouse(creatureID);

		if (creatureClicked)
		{
			StartTrackingCreature(creatureID);
		}
	}
}

void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (DearImGuiUsingMouse()) return;

	Camera_Zoom(yOffset);
}
