#include "OpenGLComputeUtils.h"

// @TODO: Add these to a MiscUtils file
template<typename T>
size_t SizeOfVector(const typename std::vector<T>& vec)
{
	return sizeof(T) * vec.size();
}


template<typename T>
void BindAndUploadSSBOData(GLuint ssbo, typename vector<T>* dataVector)
{
	// Acquire size
	GLuint dataSize = SizeOfVector(*dataVector);

	// Bind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	// Init buffer size
	glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, NULL, GL_STREAM_READ);

	// Upload
	void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
	memcpy(ptr, dataVector->data(), dataSize);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

template<typename T>
void BindAndDownloadSSBOData(GLuint ssbo, typename vector<T>* dataVector)
{
	// Acquire size
	GLuint dataSize = SizeOfVector(*dataVector);

	// Bind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	// Download
	void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	memcpy(dataVector->data(), ptr, dataSize);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
 












void DoCreatureCollidePhysics()
{

}


const char* borderPhysicsShaderPath("resources/compute shaders/border_physics.computeShader");
const int borderPhysicsLocalSizeX = 1024;
GLuint borderPhysicsProgram;
GLuint ssbo_creature_poses;
GLuint ssbo_creature_velos;
void InitCreatureBorderPhysics()
{
	GLenum borderPhysicsShaderTypes[] = { GL_COMPUTE_SHADER };
	const char* borderPhysicsShaders[] = { borderPhysicsShaderPath };
	borderPhysicsProgram = CreateLinkedShaderProgram(1, borderPhysicsShaderTypes, borderPhysicsShaders);
		
	glGenBuffers(1, &ssbo_creature_poses);
	glGenBuffers(1, &ssbo_creature_velos);
}
void DoCreatureBorderPhysics()
{
	// Bind the program
	glUseProgram(borderPhysicsProgram);

	// Set dimensions uniform
	SetUniformVector2f(borderPhysicsProgram, "uSimDimensions", vec2(1, 1));

	BindAndUploadSSBOData(ssbo_creature_poses, &creature_poses);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_creature_poses);

	BindAndUploadSSBOData(ssbo_creature_velos, &creature_velos);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_creature_velos);


	// Dispatch
	int numOfWorkGroups = creature_count / borderPhysicsLocalSizeX +
		(creature_count % borderPhysicsLocalSizeX == 0 ? 0 : 1);
	glDispatchCompute(numOfWorkGroups, 1, 1);

	
	BindAndDownloadSSBOData(ssbo_creature_poses, &creature_poses);
	BindAndDownloadSSBOData(ssbo_creature_velos, &creature_velos);
}


void ComputeUtils_Init()
{
	// Border physics init
	InitCreatureBorderPhysics();
}