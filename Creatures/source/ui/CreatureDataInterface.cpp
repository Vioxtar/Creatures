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

	GetCreatureAttribute(creature_Positions, creatureID, &snapShot.pos);
	GetCreatureAttribute(creature_Radii, creatureID, &snapShot.rad);
	GetCreatureAttribute(creature_ForwardDirections, creatureID, &snapShot.forwardDir);
	GetCreatureAttribute(creature_RightDirections, creatureID, &snapShot.rightDir);
	GetCreatureAttribute(creature_EyePositions, creatureID, &snapShot.eyePos);
	GetCreatureAttribute(creature_EyeConeRadii, creatureID, &snapShot.eyeConeRadius);
	GetCreatureAttribute(creature_Angles, creatureID, &snapShot.angle);

	snapShot.eyeConeSights.reserve(CREATURE_EYE_NUM_OF_CONES_VALUES);
	GetCreatureAttribute(creature_EyeConeSights, creatureID, snapShot.eyeConeSights.data());
	
	snapShot.eyePupilSights.reserve(CREATURE_EYE_NUM_OF_PUPIL_VALUES);
	GetCreatureAttribute(creature_EyePupilSights, creatureID, snapShot.eyePupilSights.data());

	GetCreatureAttribute(creature_EyePupilConeCoverageFraction, creatureID, &snapShot.eyePupilConeCoverageFraction);

	return snapShot;
}
