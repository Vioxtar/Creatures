#include "CreatureDataInterface.h"

vector<vec2> GetSSBOIndexedCreaturePositions()
{



	vector<vec2> positions(creature_count);
	GetCreatureAttributes(creature_Positions, positions.data());


	return positions;
}



CreatureData GetCreatureSnapshot(CreatureUniqueID creatureID)
{
	// Obtain the SSBO index of our creature


	CreatureData snapShot;

	GetCreatureAttributeByUniqueID(creature_Positions, creatureID, &snapShot.pos);
	GetCreatureAttributeByUniqueID(creature_Radii, creatureID, &snapShot.rad);
	GetCreatureAttributeByUniqueID(creature_ForwardDirections, creatureID, &snapShot.forwardDir);
	GetCreatureAttributeByUniqueID(creature_RightDirections, creatureID, &snapShot.rightDir);
	GetCreatureAttributeByUniqueID(creature_EyePositions, creatureID, &snapShot.eyePos);
	GetCreatureAttributeByUniqueID(creature_EyeConeRadii, creatureID, &snapShot.eyeConeRadius);
	GetCreatureAttributeByUniqueID(creature_Angles, creatureID, &snapShot.angle);
	GetCreatureAttributeByUniqueID(creature_Energies, creatureID, &snapShot.energy);
	GetCreatureAttributeByUniqueID(creature_Meats, creatureID, &snapShot.meat);
	GetCreatureAttributeByUniqueID(creature_Lives, creatureID, &snapShot.life);


	snapShot.eyeConeSights.reserve(CREATURE_EYE_NUM_OF_CONES_VALUES);
	GetCreatureAttributeByUniqueID(creature_EyeConeSights, creatureID, snapShot.eyeConeSights.data());
	
	snapShot.eyePupilSights.reserve(CREATURE_EYE_NUM_OF_PUPIL_VALUES);
	GetCreatureAttributeByUniqueID(creature_EyePupilSights, creatureID, snapShot.eyePupilSights.data());

	GetCreatureAttributeByUniqueID(creature_EyePupilConeCoverageFraction, creatureID, &snapShot.eyePupilConeCoverageFraction);

	snapShot.brainStructure.reserve(CREATURE_BRAIN_MAX_NUM_OF_STRUCTURE_INDICES);
	GetCreatureAttributeByUniqueID(creature_BrainsStructures, creatureID, snapShot.brainStructure.data());

	snapShot.brainNodes.reserve(CREATURE_BRAIN_MAX_NUM_OF_NODES);
	GetCreatureAttributeByUniqueID(creature_BrainsNodes, creatureID, snapShot.brainNodes.data());

	snapShot.brainLinks.reserve(CREATURE_BRAIN_MAX_NUM_OF_LINKS);
	GetCreatureAttributeByUniqueID(creature_BrainsLinks, creatureID, snapShot.brainLinks.data());
	
	return snapShot;
}
