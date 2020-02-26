#include "imgui/imgui.h"
#include "CreatureTracking.h"

class CreatureTracker
{
	unsigned int creatureIndex;

	CreatureData lastCreatureDataSnapshot;

	void Show()
	{

		// Draw our tracking halo
		ImGui::GetBackgroundDrawList()->AddCircle(
			SimulationSpaceToViewportSpace(lastCreatureDataSnapshot.pos),
			SimulationScaleToViewportScale(lastCreatureDataSnapshot.rad) + UI_CREATURE_TRACKER_HALO_RADIUS_PIXEL_BIAS,
			IM_COL32(0, 255, 0, 200),
			UI_CREATURE_TRACKER_HALO_NUM_OF_SEGMENTS,
			UI_CREATURE_TRACKER_HALO_PIXEL_THICKNESS
		);
	}


	void UpdateCreatureData()
	{
		lastCreatureDataSnapshot = GetCreatureSnapshot(creatureIndex);
	}

public:
	CreatureTracker(unsigned int creatureIndx)
	{
		creatureIndex = creatureIndx;
	}


	void Update()
	{
		// @TODO: Update in intervals instead of every frame...
		UpdateCreatureData();
		Show();
	}
};

vector<CreatureTracker> creatureTrackers;

void TrackCreature(unsigned int creatureIndex)
{
	// Create new tracker, add to active trackers vector
	creatureTrackers.push_back(CreatureTracker(creatureIndex));
}

void UpdateCreatureTrackers()
{
	for (auto tracker : creatureTrackers)
	{
		tracker.Update();
	}
}