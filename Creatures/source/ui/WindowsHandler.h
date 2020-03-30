#pragma once

#include <iostream>
#include <string>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <string>

typedef unsigned int WindowID;

using namespace std;
using namespace glm;

namespace WindowsHandler
{

	WindowID RegisterNewWindow(string title, vec2 initPos, vec2 minDimensions);

	void UnRegisterWindow(WindowID windowID);

	bool BeginAndUpdate(WindowID windowID);

	void InactivateWindow();

	bool IsWindowActive(WindowID windowID);
}