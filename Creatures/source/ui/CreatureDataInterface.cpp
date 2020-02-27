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

	return snapShot;
}