#include "CreatureDataInterface.h"

vector<vec2> GetSSBOIndexedCreaturePositions()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Positions.ssbo);
	void* positionsPtr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	
	vector<vec2> positions(creature_count);
	memcpy(&positions[0], positionsPtr, size_t(creature_Positions.attributeBytesSize) * creature_count);
	
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	return positions;
}


CreatureData GetCreatureSnapshot(CreatureUniqueID creatureID)
{
	// Obtain the SSBO index of our creature
	
	GLuint ssboIndex = CreatureUniqueIDToSSBOIndex(creatureID);

	CreatureData snapShot;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Positions.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_Positions.attributeBytesSize * ssboIndex, creature_Positions.attributeBytesSize, &snapShot.pos);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Radii.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_Radii.attributeBytesSize * ssboIndex, creature_Radii.attributeBytesSize, &snapShot.rad);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_ForwardDirections.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_ForwardDirections.attributeBytesSize * ssboIndex, creature_ForwardDirections.attributeBytesSize, &snapShot.forwardDir);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_RightDirections.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_RightDirections.attributeBytesSize * ssboIndex, creature_RightDirections.attributeBytesSize, &snapShot.rightDir);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyePositions.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyePositions.attributeBytesSize * ssboIndex, creature_EyePositions.attributeBytesSize, &snapShot.eyePos);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyeConeRadii.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyeConeRadii.attributeBytesSize * ssboIndex, creature_EyeConeRadii.attributeBytesSize, &snapShot.eyeConeRadius);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Angles.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_Angles.attributeBytesSize * ssboIndex, creature_Angles.attributeBytesSize, &snapShot.angle);

	snapShot.eyeConeSights.reserve(CREATURE_EYE_NUM_OF_CONES_VALUES);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyeConeSights.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyeConeSights.attributeBytesSize * ssboIndex, creature_EyeConeSights.attributeBytesSize, snapShot.eyeConeSights.data());

	snapShot.eyePupilSights.reserve(CREATURE_EYE_NUM_OF_PUPIL_VALUES);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyePupilSights.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyePupilSights.attributeBytesSize * ssboIndex, creature_EyePupilSights.attributeBytesSize, snapShot.eyePupilSights.data());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyePupilConeCoverageFraction.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyePupilConeCoverageFraction.attributeBytesSize * ssboIndex, creature_EyePupilConeCoverageFraction.attributeBytesSize, &snapShot.eyePupilConeCoverageFraction);

	return snapShot;
}