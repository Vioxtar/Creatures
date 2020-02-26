
#include "CreatureTracking.h"

class CreatureTracker;

map<unsigned int, CreatureTracker> activeCreatureTrackers;

// Expired creature trackers to be deleted
vector<unsigned int> expiredCreatureTrackers;

class CreatureTracker
{
	unsigned int creatureIndex;

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
		lastCreatureDataSnapshot = GetCreatureSnapshot(creatureIndex);
	}

	void Close()
	{
		expiredCreatureTrackers.push_back(creatureIndex);
	}

public:
	CreatureTracker(unsigned int creatureIndex) : creatureIndex(creatureIndex)
	{
		cout << "NEW TRACKER " << creatureIndex << " from CONSTRUCTOR" << endl;
	}

	CreatureTracker(const CreatureTracker&) = delete;

	void Update()
	{
		// @TODO: Update in intervals instead of every frame...
		UpdateCreatureData();
		Show();
	}

	bool operator==(const CreatureTracker& other) const
	{
		return this->creatureIndex == other.creatureIndex;
	}

	bool operator<(const CreatureTracker& other) const
	{
		return this->creatureIndex < other.creatureIndex;
	}

};

void TrackCreature(unsigned int creatureIndex)
{
	// Create new tracker, add to active trackers vector
	activeCreatureTrackers.emplace(creatureIndex, creatureIndex);
}

void UpdateCreatureTrackers()
{
	for (auto& it : activeCreatureTrackers)
	{
		it.second.Update();
	}

	for (auto creatureIndex : expiredCreatureTrackers)
	{
		activeCreatureTrackers.erase(creatureIndex);
	}
}