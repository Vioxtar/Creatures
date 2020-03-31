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
	vec2 size;
	vec2 minSize;
	bool active;
	bool closing;
	bool needToResolveInteraction;
	bool initialized;
	bool needToResolveCollapse;

	vec4 targetBordersPositions;
};


////////////////////////
// -- WINDOW UTILS -- //
////////////////////////

void SmoothWindowReSize(WindowData& windowData, vec2 targetSize)
{
	vec2 oldSize = ImGui::GetWindowSize();
	windowData.size = targetSize * UI_DEARIMGUI_WINDOW_SMOOTH_RESIZE_RATE + oldSize * (1.0f - UI_DEARIMGUI_WINDOW_SMOOTH_RESIZE_RATE);
	ImGui::SetWindowSize(windowData.size);
}

vec4 GetBorderPositions()
{
	vec2 pos = ImGui::GetWindowPos();
	vec2 size = ImGui::GetWindowSize();
	return vec4(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
}


void SmoothWindowBorderPositioning(WindowData& windowData, vec4 targetPositions)
{
	vec4 oldPositions = vec4(
		windowData.pos.x,
		windowData.pos.y,
		windowData.pos.x + windowData.size.x,
		windowData.pos.y + windowData.size.y
	);

	vec4 newPositions = targetPositions * UI_DEARIMGUI_WINDOW_SMOOTH_RESIZE_RATE + oldPositions * (1.0f - UI_DEARIMGUI_WINDOW_SMOOTH_RESIZE_RATE);

	windowData.pos = vec2(newPositions.x, newPositions.y);
	windowData.size = vec2(newPositions.z - newPositions.x, newPositions.w - newPositions.y);

	ImGui::SetWindowPos(windowData.pos);
	ImGui::SetWindowSize(windowData.size);
}

void WindowTick(WindowData& windowData)
{
	// Initialization
	if (!windowData.initialized)
	{
		ImGui::SetWindowPos(windowData.pos);
		ImGui::SetWindowSize(windowData.size);

		windowData.initialized = true;
	}

	// Did the user close the window? If so activate closing state
	if (!windowData.active)
	{
		// Keep the window visible
		windowData.active = true;

		windowData.closing = true;
	}

	// Closing logic
	if (windowData.closing)
	{
		// Squeeze to the top right
		windowData.targetBordersPositions.x = windowData.targetBordersPositions.z;
		windowData.targetBordersPositions.w = windowData.targetBordersPositions.y;
		SmoothWindowBorderPositioning(windowData, windowData.targetBordersPositions);

		// If we're small enough, close the window
		if (windowData.size.y <= UI_DEARIMGUI_WINDOW_CLOSING_PIXEL_SIZE_THRESHOLD || windowData.size.x <= UI_DEARIMGUI_WINDOW_CLOSING_PIXEL_SIZE_THRESHOLD)
		{
			windowData.active = false;
			WindowsHandler::UnRegisterWindow(windowData.id);
		}
		return;
	}

	// Is the window collapsed?
	bool collapsed = ImGui::IsWindowCollapsed();
	if (collapsed)
	{
		windowData.needToResolveCollapse = true;
		return;
	}

	// Did we just uncollapse? @TODO: Actually make this work
	if (windowData.needToResolveCollapse)
	{
		windowData.pos = ImGui::GetWindowPos();
		windowData.size = ImGui::GetWindowSize();
		windowData.needToResolveCollapse = false;

		return;
	}

	// Is the window being interacted with?
	bool beingInteracted = ImGui::IsAnyMouseDown() && ImGui::IsWindowFocused();

	// If we're being interacted with, bail for the sake of correctness / responsiveness
	if (beingInteracted)
	{
		windowData.needToResolveInteraction = true;
		return;
	}


	// We're no longer being interacted with, resolve
	if (windowData.needToResolveInteraction)
	{
		windowData.needToResolveInteraction = false;

		// Update our new positions
		vec4 currentBorderPositions = GetBorderPositions();
		windowData.pos = vec2(currentBorderPositions.x, currentBorderPositions.y);
		windowData.size = vec2(currentBorderPositions.z - currentBorderPositions.x, currentBorderPositions.w - currentBorderPositions.y);

		// Find out how our borders changed
		vec4 borderPositionsLastInteractionDelta = windowData.targetBordersPositions - currentBorderPositions;
		windowData.targetBordersPositions = currentBorderPositions;

		// Handle resize logic
		if (borderPositionsLastInteractionDelta != vec4(0))
		{

			// Find out which border was resized

			bool resizedLeft = borderPositionsLastInteractionDelta.x != 0;
			bool resizedRight = borderPositionsLastInteractionDelta.z != 0;
			bool resizedUp = borderPositionsLastInteractionDelta.y != 0;
			bool resizedDown = borderPositionsLastInteractionDelta.w != 0;

			if (resizedLeft && resizedRight)
			{
				if (abs(borderPositionsLastInteractionDelta.x) > abs(borderPositionsLastInteractionDelta.z))
				{
					resizedRight = false;
				}
				else
				{
					resizedLeft = false;
				}
			}

			if (resizedUp && resizedDown)
			{
				if (abs(borderPositionsLastInteractionDelta.y) > abs(borderPositionsLastInteractionDelta.w))
				{
					resizedDown = false;
				}
				else
				{
					resizedUp = false;
				}
			}

			// Resolve min size violations

			float newTargetWidth = windowData.targetBordersPositions.z - windowData.targetBordersPositions.x;
			if (newTargetWidth < windowData.minSize.x)
			{
				float violation = windowData.minSize.x - newTargetWidth;
				if (resizedLeft)
				{
					windowData.targetBordersPositions.x -= violation;
				}
				else if (resizedRight)
				{
					windowData.targetBordersPositions.z += violation;
				}
			}

			float newTargetHeight = windowData.targetBordersPositions.w - windowData.targetBordersPositions.y;
			if (newTargetHeight < windowData.minSize.y)
			{
				float violation = windowData.minSize.y - newTargetHeight;
				if (resizedUp)
				{
					windowData.targetBordersPositions.y -= violation;
				}
				else if (resizedDown)
				{
					windowData.targetBordersPositions.w += violation;
				}
			}
		}
	}

	// Interpolate to target
	SmoothWindowBorderPositioning(windowData, windowData.targetBordersPositions);
}




std::map<WindowID, WindowData> activeWindows;
WindowID nextWindowIDToBeAssigned = 0;

WindowID WindowsHandler::RegisterNewWindow(string title, vec2 initPos, vec2 minSize)
{
	WindowData newWindowData;
	newWindowData.id = nextWindowIDToBeAssigned;
	newWindowData.title = title;
	newWindowData.minSize = minSize;
	newWindowData.active = true;
	newWindowData.closing = false;
	newWindowData.needToResolveInteraction = false;
	newWindowData.initialized = false;
	newWindowData.needToResolveCollapse = false;

	newWindowData.pos = initPos;
	newWindowData.size = vec2(0, 0);

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

	if (!ImGui::Begin(windowData.title.c_str(), &windowData.active))
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
		return windowData.active;
	}
	catch (out_of_range & e)
	{
		return false;
	}
}
