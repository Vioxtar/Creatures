#include "CreatureTracking.h"

class CreatureTracker;

map<CreatureUniqueID, CreatureTracker> activeCreatureTrackers;

// Expired creature trackers to be deleted
vector<CreatureUniqueID> expiredCreatureTrackers;


class CreatureTracker
{
	CreatureUniqueID creatureID;
	CreatureData creatureSnapShot;
	string windowTitle;
	bool active;

	bool overlay_Halo;
	bool overlay_ForwardDir;
	bool overlay_RightDir;
	bool overlay_Eye;

	void Show()
	{
		// Show overlay even if we're minimized
		ShowOverlay();

		if (!ImGui::Begin(windowTitle.c_str(), &active))
		{
			ImGui::End();
			return;
		}

		if (ImGui::CollapsingHeader("Overlay Display"))
		{
			ImGui::Checkbox("Halo", &overlay_Halo);
			ImGui::Checkbox("Forward Direction", &overlay_ForwardDir);
			ImGui::Checkbox("Right Direction", &overlay_RightDir);
			ImGui::Checkbox("Eyes", &overlay_Eye);
		}

		ImGui::End();
	}

	void ShowOverlay()
	{
		if (overlay_Halo)
		{
			// Draw a halo around our creature
			ImGui::GetBackgroundDrawList()->AddCircle(
				SimulationSpaceToViewportSpace(creatureSnapShot.pos),
				SimulationScaleToViewportScale(creatureSnapShot.rad) + UI_CREATURE_TRACKER_HALO_RADIUS_PIXEL_BIAS,
				IM_COL32(0, 255, 0, 100),
				UI_CREATURE_TRACKER_HALO_NUM_OF_SEGMENTS,
				UI_CREATURE_TRACKER_HALO_PIXEL_THICKNESS
			);
		}

		if (overlay_ForwardDir)
		{
			vec2 p1 = SimulationSpaceToViewportSpace(creatureSnapShot.pos);
			vec2 p2 = SimulationSpaceToViewportSpace(creatureSnapShot.pos + creatureSnapShot.forwardDir);
			vec2 p1Add = normalize(p2 - p1);
			p1Add *= UI_CREATURE_TRACKER_DIRECTION_PIXEL_DISTANCE;
			ImGui::GetBackgroundDrawList()->AddLine(
				p1,
				p1 + p1Add,
				IM_COL32(0, 255, 0, 100),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);
		}

		if (overlay_RightDir)
		{
			vec2 p1 = SimulationSpaceToViewportSpace(creatureSnapShot.pos);
			vec2 p2 = SimulationSpaceToViewportSpace(creatureSnapShot.pos + creatureSnapShot.rightDir);
			vec2 p1Add = normalize(p2 - p1);
			p1Add *= UI_CREATURE_TRACKER_DIRECTION_PIXEL_DISTANCE;
			ImGui::GetBackgroundDrawList()->AddLine(
				p1,
				p1 + p1Add,
				IM_COL32(0, 255, 0, 100),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);
		}

		if (overlay_Eye)
		{
			// Draw eye probe area
			ImGui::GetBackgroundDrawList()->AddQuad(
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + (creatureSnapShot.rightDir + creatureSnapShot.forwardDir) * CREATURE_EYE_MAX_PROBE_DISTANCE.value),
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + (creatureSnapShot.rightDir - creatureSnapShot.forwardDir) * CREATURE_EYE_MAX_PROBE_DISTANCE.value),
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + (-creatureSnapShot.rightDir - creatureSnapShot.forwardDir) * CREATURE_EYE_MAX_PROBE_DISTANCE.value),
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + (-creatureSnapShot.rightDir + creatureSnapShot.forwardDir) * CREATURE_EYE_MAX_PROBE_DISTANCE.value),
				IM_COL32(255, 255, 0, 100),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);

			vec2 viewportEyePos = SimulationSpaceToViewportSpace(creatureSnapShot.eyePos);
			float viewportEyeConeRadius = SimulationScaleToViewportScale(creatureSnapShot.eyeConeRadius);
			
			// Draw eye cone ring
			ImGui::GetBackgroundDrawList()->AddCircle(
				viewportEyePos,
				viewportEyeConeRadius,
				IM_COL32(0, 255, 0, 35),
				int(viewportEyeConeRadius),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);

			// Draw eye cones
			float angAdd = 2.0 * M_PI / float(CREATURE_EYE_NUM_OF_CONES);
			float creatureAngle = creatureSnapShot.angle;
			for (unsigned int i = 0; i < CREATURE_EYE_NUM_OF_CONES; ++i)
			{
				// Draw cone line
				float lineAng = M_PI + i * angAdd - creatureAngle;
				vec2 coneLineDir = vec2(sin(lineAng), cos(lineAng));
				ImGui::GetBackgroundDrawList()->AddLine(
					viewportEyePos,
					viewportEyePos + coneLineDir * viewportEyeConeRadius,
					IM_COL32(0, 255, 0, 20),
					UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
				);

				// Draw cone activation
				float activationAng = M_PI + i * angAdd + (angAdd / 2) - creatureAngle;
				vec2 coneActivationDir = vec2(sin(activationAng), cos(activationAng));
				ImGui::GetBackgroundDrawList()->AddLine(
					viewportEyePos,
					viewportEyePos + coneActivationDir * viewportEyeConeRadius * creatureSnapShot.eyeConeSights.data()[i],
					IM_COL32(0, 255, 0, 100),
					UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
				);
			}
		}
	}

	void UpdateCreatureData()
	{
		creatureSnapShot = GetCreatureSnapshot(creatureID);
	}

	void Close()
	{
		expiredCreatureTrackers.push_back(creatureID);
	}

public:

	CreatureTracker(CreatureUniqueID creatureID) :
		creatureID(creatureID), active(true)
	{
		windowTitle = "Creature ";
		windowTitle.append(to_string(creatureID));

		overlay_Halo = true;
		overlay_ForwardDir = false;
		overlay_RightDir = false;
		overlay_Eye = true;
	}

	CreatureTracker(const CreatureTracker&) = delete;

	void Update()
	{

		if (!active)
		{
			Close();
			return;
		}

		// @TODO: Update in intervals instead of every frame...
		try
		{
			// Our creature is valid
			UpdateCreatureData();
			Show();
		}
		catch (out_of_range& e)
		{
			// If our unique creature ID was out of range, we're no longer valid
			Close();
		}
	}

};


void StartTrackingCreature(CreatureUniqueID creatureID)
{
	// Create new tracker, add to active trackers map
	activeCreatureTrackers.emplace(creatureID, creatureID);
}


void UpdateCreatureTrackers()
{
	// Update all trackers
	for (auto& it : activeCreatureTrackers)
	{
		it.second.Update();
	}

	// Clear closed creature trackers
	for (auto creatureID : expiredCreatureTrackers)
	{
		activeCreatureTrackers.erase(creatureID);
	}

	expiredCreatureTrackers.clear();
}