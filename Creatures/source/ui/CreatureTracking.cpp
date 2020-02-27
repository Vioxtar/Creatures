
#include "CreatureTracking.h"

class CreatureTracker;

map<CreatureUniqueID, CreatureTracker> activeCreatureTrackers;

// Expired creature trackers to be deleted
vector<CreatureUniqueID> expiredCreatureTrackers;


class CreatureTracker
{
	CreatureUniqueID creatureID;

	CreatureData lastCreatureDataSnapshot;

	void Show()
	{
		// Draw a halo around our creature
		ImGui::GetBackgroundDrawList()->AddCircle(
			SimulationSpaceToViewportSpace(lastCreatureDataSnapshot.pos),
			SimulationScaleToViewportScale(lastCreatureDataSnapshot.rad) + UI_CREATURE_TRACKER_HALO_RADIUS_PIXEL_BIAS,
			IM_COL32(0, 255, 0, 100),
			UI_CREATURE_TRACKER_HALO_NUM_OF_SEGMENTS,
			UI_CREATURE_TRACKER_HALO_PIXEL_THICKNESS
		);
	}

	void UpdateCreatureData()
	{
		lastCreatureDataSnapshot = GetCreatureSnapshot(creatureID);
	}

	void Close()
	{
		expiredCreatureTrackers.push_back(creatureID);
	}

public:

	CreatureTracker(CreatureUniqueID creatureID) : creatureID(creatureID) {}

	CreatureTracker(const CreatureTracker&) = delete;

	void Update()
	{
		// @TODO: Update in intervals instead of every frame...
		try
		{
			UpdateCreatureData();
			Show();
		}
		catch (out_of_range& e)
		{
			// If our unique creature ID was out of range then we were probably removed, close the tracker
			Close();
		}
	}

};


void TrackCreature(CreatureUniqueID creatureID)
{
	// Create new tracker, add to active trackers vector
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