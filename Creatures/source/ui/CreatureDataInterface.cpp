#include "CreatureDataInterface.h"

vector<vec2> GetCreaturePositions()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creature_Positions.ssbo);
	void* positionsPtr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	
	vector<vec2> positions(creature_count);
	memcpy(&positions[0], positionsPtr, size_t(creature_Positions.attributeBytesSize) * creature_count);
	
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	return positions;
}


CreatureData GetCreatureSnapshot(unsigned int creatureIndex)
{
	CreatureData snapShot;
	

	CreatureAttributesSSBOInfo& creatureAttributes = creature_Positions;
	size_t attributeSize = creatureAttributes.attributeBytesSize;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, creatureAttributes.ssbo);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, attributeSize * creatureIndex, attributeSize, &snapShot.pos);

	return snapShot;
}