#include "WindowsHandler.h"
#include <map>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "../Settings.h"

struct WindowData
{
	WindowID id;
	string title;
	vec2 pos;
	vec2 dim;
	vec2 minDim;
	bool show;
	bool closing;
};


////////////////////////
// -- WINDOW UTILS -- //
////////////////////////

void SmoothWindowReSize(WindowData& windowData, vec2 targetSize)
{
	vec2 oldSize = ImGui::GetWindowSize();
	windowData.dim = targetSize * UI_DEARIMGUI_WINDOW_SMOOTH_RESIZE_RATE + oldSize * (1.0f - UI_DEARIMGUI_WINDOW_SMOOTH_RESIZE_RATE);
	ImGui::SetWindowSize(windowData.dim);
}

void WindowTick(WindowData& windowData)
{
	if (!windowData.show)
	{
		windowData.show = true;
		windowData.closing = true;
	}

	if (windowData.closing)
	{
		SmoothWindowReSize(windowData, vec2(0, 0));
		if (windowData.dim.y <= UI_DEARIMGUI_WINDOW_CLOSING_PIXEL_SIZE_THRESHOLD && windowData.dim.x <= UI_DEARIMGUI_WINDOW_CLOSING_PIXEL_SIZE_THRESHOLD)
		{
			windowData.show = false;
			WindowsHandler::UnRegisterWindow(windowData.id);
		}
		return;
	}
	   
	vec2 currWindowSize = ImGui::GetWindowSize();
	SmoothWindowReSize(windowData, vec2(
		std::max(currWindowSize.x, windowData.minDim.x),
		std::max(currWindowSize.y, windowData.minDim.y)
	));
}




std::map<WindowID, WindowData> activeWindows;
WindowID nextWindowIDToBeAssigned = 0;

WindowID WindowsHandler::RegisterNewWindow(string title, vec2 initPos, vec2 minDimensions)
{
	WindowData newWindowData;
	newWindowData.id = nextWindowIDToBeAssigned;
	newWindowData.title = title;
	newWindowData.pos = initPos;
	newWindowData.dim = vec2(0, 0);
	newWindowData.minDim = minDimensions;
	newWindowData.show = true;
	newWindowData.closing = false;

	activeWindows.emplace(newWindowData.id, newWindowData);

	nextWindowIDToBeAssigned++;

	return newWindowData.id;
}

void WindowsHandler::UnRegisterWindow(WindowID windowID)
{
	activeWindows.erase(windowID);
}

bool WindowsHandler::BeginAndUpdate(WindowID windowID)
{
	WindowData& windowData = activeWindows.at(windowID);

	if (!ImGui::Begin(windowData.title.c_str(), &windowData.show))
	{
		return false;
	}

	WindowTick(windowData);

	return true;
}

void WindowsHandler::InactivateWindow()
{
	ImGui::End();
}

bool WindowsHandler::IsWindowActive(WindowID windowID)
{
	try
	{
		WindowData windowData = activeWindows.at(windowID);
		return windowData.show;
	}
	catch (out_of_range & e)
	{
		return false;
	}
}
