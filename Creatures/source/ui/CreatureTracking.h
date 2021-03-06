#pragma once
#include <iostream>
#include "imgui/imgui.h"
#include "CreatureDataInterface.h"
#include <map>
#include "UserInterface.h"

using namespace UserInterface;

void StartTrackingCreature(CreatureUniqueID creatureID);

void UpdateCreatureTrackers();

void CreatureTracking_Init();