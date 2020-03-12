#include "CreatureDataInterface.h"

vector<vec2> GetSSBOIndexedCreaturePositions()
{
	void* positionsPtr = glMapNamedBuffer(creature_Positions.bufferHandle, GL_READ_ONLY);
	vector<vec2> positions(creature_count);
	memcpy(&positions[0], positionsPtr, size_t(creature_Positions.unitByteSize) * creature_count);
	glUnmapNamedBuffer(creature_Positions.bufferHandle);



	//glGetNamedBufferSubData(creature_Positions.bufferHandle, 0, creature_Positions.unitByteSize * creature_count, positions.data());



	return positions;
}


CreatureData GetCreatureSnapshot(CreatureUniqueID creatureID)
{
	// Obtain the SSBO index of our creature
	
	GLuint ssboIndex = CreatureData_CreatureUniqueIDToSSBOIndex(creatureID);

	CreatureData snapShot;

	//void* ptr = glMapNamedBufferRange(creature_Positions.bufferHandle, creature_Positions.unitByteSize * ssboIndex, creature_Positions.unitByteSize, GL_MAP_READ_BIT);
	//snapShot.pos = *(vec2*)ptr;
	//glUnmapNamedBuffer(creature_Positions.bufferHandle);

	glGetNamedBufferSubData(creature_Positions.bufferHandle, creature_Positions.unitByteSize * ssboIndex, creature_Positions.unitByteSize, &snapShot.pos);

	glGetNamedBufferSubData(creature_Radii.bufferHandle, creature_Radii.unitByteSize * ssboIndex, creature_Radii.unitByteSize, &snapShot.rad);

	glGetNamedBufferSubData(creature_ForwardDirections.bufferHandle, creature_ForwardDirections.unitByteSize * ssboIndex, creature_ForwardDirections.unitByteSize, &snapShot.forwardDir);

	glGetNamedBufferSubData(creature_RightDirections.bufferHandle, creature_RightDirections.unitByteSize * ssboIndex, creature_RightDirections.unitByteSize, &snapShot.rightDir);

	glGetNamedBufferSubData(creature_EyePositions.bufferHandle, creature_EyePositions.unitByteSize * ssboIndex, creature_EyePositions.unitByteSize, &snapShot.eyePos);

	glGetNamedBufferSubData(creature_EyeConeRadii.bufferHandle, creature_EyeConeRadii.unitByteSize * ssboIndex, creature_EyeConeRadii.unitByteSize, &snapShot.eyeConeRadius);

	glGetNamedBufferSubData(creature_Angles.bufferHandle, creature_Angles.unitByteSize * ssboIndex, creature_Angles.unitByteSize, &snapShot.angle);

	snapShot.eyeConeSights.reserve(CREATURE_EYE_NUM_OF_CONES_VALUES);
	glGetNamedBufferSubData(creature_EyeConeSights.bufferHandle, creature_EyeConeSights.unitByteSize * ssboIndex, creature_EyeConeSights.unitByteSize, snapShot.eyeConeSights.data());

	snapShot.eyePupilSights.reserve(CREATURE_EYE_NUM_OF_PUPIL_VALUES);
	glGetNamedBufferSubData(creature_EyePupilSights.bufferHandle, creature_EyePupilSights.unitByteSize * ssboIndex, creature_EyePupilSights.unitByteSize, snapShot.eyePupilSights.data());

	glGetNamedBufferSubData(creature_EyePupilConeCoverageFraction.bufferHandle, creature_EyePupilConeCoverageFraction.unitByteSize * ssboIndex, creature_EyePupilConeCoverageFraction.unitByteSize, &snapShot.eyePupilConeCoverageFraction);

	return snapShot;
}