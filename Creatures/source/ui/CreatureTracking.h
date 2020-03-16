#pragma once
#include "imgui/imgui.h"
#include "CreatureDataInterface.h"
#include <map>
#include "UserInterface.h"

void StartTrackingCreature(CreatureUniqueID creatureID);

void UpdateCreatureTrackers();

void CreatureTracking_Init();