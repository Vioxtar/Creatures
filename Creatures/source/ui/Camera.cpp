#include "Camera.h"

// Camera zoom cached settings
float maxZoomOut;
float cameraZoomTargetWeight;
float cameraZoomWeight;

// Camera zoom controls
float cameraZoom;
float cameraZoomTarget;

// Camera movement cached settings
float camVelocityFriction;

// Camera movement controls
vec2 cameraPos = vec2(0, 0);
vec2 cameraVel = vec2(0, 0);
bool canGlide = true;

// Camera transform
mat4 simToCamTransform = mat4(1.0f);

// Last frame time for consistent UI responsiveness
float lastFrameTime;

// @TODO: Consider last frame time for better UI responsiveness!
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

void Camera::Initialize()
{
	// Load camera cached settings
	maxZoomOut = CAMERA_MAX_ZOOM_OUT;
	camVelocityFriction = CAMERA_VELOCITY_FRICTION;

	float zoomInterpolationSpeed = CAMERA_ZOOM_INTERPOLATION_SPEED;
	cameraZoomTargetWeight = zoomInterpolationSpeed;
	cameraZoomWeight = 1.0f - zoomInterpolationSpeed;

	cameraZoom = CAMERA_START_ZOOM;
	cameraZoomTarget = CAMERA_START_ZOOM_TARGET;
}

void Camera::Update(double frameTime)
{
	lastFrameTime = (float)frameTime;
	UpdateCameraPlacement();
}

void Camera::EnableGlide(bool glide)
{
	canGlide = glide;
}

void Camera::Move(vec2 offset)
{
	cameraPos += offset;
	cameraVel = offset;
}

void Camera::InterpolatePositionTo(vec2 targetPos, float rate)
{
	if (!canGlide) return;
	rate *= sqrt(cameraZoom);
	cameraPos *= (1.0 - rate);
	targetPos *= rate;
	cameraPos -= targetPos;
}

void Camera::Zoom(float zoom)
{
	float zoomTargetAdd = zoom * CAMERA_ZOOM_SCROLL_SCALE;
	zoomTargetAdd *= cameraZoom; // Zoom out less the more we're already zoomed out

	cameraZoomTarget = mathutils::clamp(cameraZoomTarget + zoomTargetAdd, CAMERA_MAX_ZOOM_OUT, CAMERA_MAX_ZOOM_IN);
}

mat4 Camera::GetSimSpaceToCameraTransform()
{
	return simToCamTransform;
}
