#include "UserControls.h"

int currWindowWidth;
int currWindowHeight;

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

void glfw_framebuffer_size_callback(GLFWwindow*, int width, int height)
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

bool mousePressed = false;
double currMouseXPos, currMouseYPos = 0;
void glfw_cursor_position_callback(GLFWwindow* window, double x, double y)
{
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
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		mousePressed = action == GLFW_PRESS;
		Camera_Enable_Glide(!mousePressed);
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		vec2 simPos = ViewportSpaceToSimulationSpace(vec2(currMouseXPos, currMouseYPos));
		cout << "MOUSE POS:" << endl;
		cout << "X: " << simPos.x << "  Y: " << simPos.y << endl;

	}
}

void glfw_scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	Camera_Zoom(yOffset);
}
