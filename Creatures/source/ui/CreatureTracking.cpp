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

	void Show()
	{

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
		}

		ImGui::End();

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
			ImGui::GetBackgroundDrawList()->AddLine(
				SimulationSpaceToViewportSpace(creatureSnapShot.pos),
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + creatureSnapShot.forwardDir),
				IM_COL32(0, 255, 0, 100),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);
		}

		if (overlay_RightDir)
		{
			ImGui::GetBackgroundDrawList()->AddLine(
				SimulationSpaceToViewportSpace(creatureSnapShot.pos),
				SimulationSpaceToViewportSpace(creatureSnapShot.pos + creatureSnapShot.rightDir),
				IM_COL32(0, 255, 0, 100),
				UI_CREATURE_TRACKER_DEFAULT_LINE_PIXEL_THICKNESS
			);
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


void TrackCreature(CreatureUniqueID creatureID)
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