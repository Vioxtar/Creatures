#include "Camera.h"

const float cameraBuffer = 1;


// Camera zoom cached settings
float maxZoomOut;
float cameraZoomTargetWeight;
float cameraZoomWeight;

// Camera zoom controls
float cameraZoom = 0.0;
float cameraZoomTarget = 1.0;


// Camera movement cached settings
float camVelocityFriction;

// Camera movement controls
vec2 cameraPos = vec2(0, 0);
vec2 cameraVel = vec2(0, 0);
bool canGlide = false;


// Camera transform
mat4 simToCamTransform = mat4(1.0f);


void UpdateCameraPlacement()
{
	cameraZoom = cameraZoom * cameraZoomWeight + cameraZoomTarget * cameraZoomTargetWeight;
	
	if (canGlide)
	{
		cameraPos += cameraVel;
		cameraVel *= camVelocityFriction;
	}
	else
	{
		// We're not gliding, zerofiy camera velocity
		cameraVel = vec2();
	}
	
	// Set the global camera transform
	simToCamTransform = mat4(1.0f); // Identity matrix
	simToCamTransform = scale(simToCamTransform, vec3(cameraZoom));
	simToCamTransform = translate(simToCamTransform, vec3(cameraPos.x, cameraPos.y, 0.0));
}

void Camera_Init()
{
	// Load camera cached settings
	maxZoomOut = SIM_SETTINGS.MAX_CAMERA_ZOOM_OUT;
	camVelocityFriction = SIM_SETTINGS.CAMERA_VELOCITY_FRICTION;

	float zoomInterpolationSpeed = SIM_SETTINGS.CAMERA_ZOOM_INTERPOLATION_SPEED;
	cameraZoomTargetWeight = zoomInterpolationSpeed;
	cameraZoomWeight = 1.0f - zoomInterpolationSpeed;
}

void Camera_Update()
{
	UpdateCameraPlacement();
}

void Camera_Enable_Glide(bool glide)
{
	canGlide = glide;
}

void Camera_Move(vec2 offset)
{
	cameraPos += offset;
	cameraVel = offset;
}

void Camera_Zoom(float zoom)
{
	float zoomTargetAdd = zoom * SIM_SETTINGS.CAMERA_ZOOM_SCROLL_SCALE;
	zoomTargetAdd *= cameraZoom; // Zoom out less the more we're already zoomed out

	cameraZoomTarget = mathutils::clamp(cameraZoomTarget + zoomTargetAdd, SIM_SETTINGS.MAX_CAMERA_ZOOM_OUT, SIM_SETTINGS.MAX_CAMERA_ZOOM_IN);
}

mat4 GetSimSpaceToCameraTransform()
{
	return simToCamTransform;
}
