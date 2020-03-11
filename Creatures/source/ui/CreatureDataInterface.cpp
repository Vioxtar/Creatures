#include "CreatureDataInterface.h"

vector<vec2> GetSSBOIndexedCreaturePositions()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Positions.bufferHandle);
	void* positionsPtr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	
	vector<vec2> positions(creature_count);
	memcpy(&positions[0], positionsPtr, size_t(creature_Positions.unitByteSize) * creature_count);
	
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	return positions;
}


CreatureData GetCreatureSnapshot(CreatureUniqueID creatureID)
{
	// Obtain the SSBO index of our creature
	
	GLuint ssboIndex = CreatureUniqueIDToSSBOIndex(creatureID);

	CreatureData snapShot;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Positions.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_Positions.unitByteSize * ssboIndex, creature_Positions.unitByteSize, &snapShot.pos);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Radii.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_Radii.unitByteSize * ssboIndex, creature_Radii.unitByteSize, &snapShot.rad);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_ForwardDirections.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_ForwardDirections.unitByteSize * ssboIndex, creature_ForwardDirections.unitByteSize, &snapShot.forwardDir);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_RightDirections.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_RightDirections.unitByteSize * ssboIndex, creature_RightDirections.unitByteSize, &snapShot.rightDir);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyePositions.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyePositions.unitByteSize * ssboIndex, creature_EyePositions.unitByteSize, &snapShot.eyePos);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyeConeRadii.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyeConeRadii.unitByteSize * ssboIndex, creature_EyeConeRadii.unitByteSize, &snapShot.eyeConeRadius);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Angles.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_Angles.unitByteSize * ssboIndex, creature_Angles.unitByteSize, &snapShot.angle);

	snapShot.eyeConeSights.reserve(CREATURE_EYE_NUM_OF_CONES_VALUES);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyeConeSights.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyeConeSights.unitByteSize * ssboIndex, creature_EyeConeSights.unitByteSize, snapShot.eyeConeSights.data());

	snapShot.eyePupilSights.reserve(CREATURE_EYE_NUM_OF_PUPIL_VALUES);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyePupilSights.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyePupilSights.unitByteSize * ssboIndex, creature_EyePupilSights.unitByteSize, snapShot.eyePupilSights.data());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_EyePupilConeCoverageFraction.bufferHandle);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, creature_EyePupilConeCoverageFraction.unitByteSize * ssboIndex, creature_EyePupilConeCoverageFraction.unitByteSize, &snapShot.eyePupilConeCoverageFraction);

	return snapShot;
}