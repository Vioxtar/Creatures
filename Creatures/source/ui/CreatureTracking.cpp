#include "imgui/imgui.h"
#include "CreatureTracking.h"

class CreatureTracker
{
	unsigned int creatureIndex;

	void Show()
	{
		ImGui::GetBackgroundDrawList()->AddCircle(vec2(0, 0), 25, IM_COL32(0, 255, 0, 200), 48, 10);
	}


	void UpdateCreatureData()
	{
	}

public:
	CreatureTracker(unsigned int creatureIndx)
	{
		creatureIndex = creatureIndx;
	}


	void Update()
	{
		// @TODO: If passed update frequency...
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