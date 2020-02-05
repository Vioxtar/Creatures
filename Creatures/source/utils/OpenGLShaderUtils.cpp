#include "OpenGLShaderUtils.h"

bool CheckShaderCompileSuccess(GLuint obj)
{
	GLint success;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		GLint msgLength;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &msgLength);
		std::vector<char> messageLog(msgLength);
		glGetShaderInfoLog(obj, msgLength, &msgLength, &messageLog[0]);
		std::cerr << &messageLog[0] << std::endl;
		return false;
	}
	return true;
}

bool CheckProgramLinkSuccess(GLuint obj) 
{
	GLint success;
	glGetProgramiv(obj, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		GLint msgLength;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &msgLength);
		std::vector<char> messageLog(msgLength);
		glGetShaderInfoLog(obj, msgLength, &msgLength, &messageLog[0]);
		std::cerr << &messageLog[0] << std::endl;
		return false;
	}
	return true;
}

string GetShaderSourceStringFromFilePath(const char* shaderFilePath)
{
	std::string codeString;
	std::ifstream shaderFile;

	// Ensure the ifstream can throw exceptions
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Open file
		shaderFile.open(shaderFilePath);
		std::stringstream vShaderStream;

		// Read file's buffer contents into stream
		vShaderStream << shaderFile.rdbuf();

		// Close file handler
		shaderFile.close();

		// Convert to a large string
		codeString = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SCCESFULLY_READ" << std::endl;
		std::cerr << shaderFilePath << std::endl;
	}

	return codeString;
}

GLuint CreateAndCompileShader(string& shaderSourceString, GLenum shaderType)
{
	GLuint shaderID = glCreateShader(shaderType);

	const char* src = shaderSourceString.c_str();
	glShaderSource(shaderID, 1, &src, NULL);

	glCompileShader(shaderID);

	if (!CheckShaderCompileSuccess(shaderID))
	{
		return -1;
	}
	return shaderID;
}

GLuint CreateLinkedShaderProgram(int numOfShaders, GLuint* shaderTypes, const char** shaderFilePaths)
{
	bool success = true;
	
	// Register the new program
	GLuint shaderProgram = glCreateProgram();

	// Create a dynamic shader ID storage vector
	vector<GLuint> compiledShaders;
	compiledShaders.reserve(numOfShaders);

	// Create, compile and attach our shaders
	for (unsigned int i = 0; i < numOfShaders; i++)
	{
		const char* shaderFilePath = shaderFilePaths[i];
		GLuint shaderType = shaderTypes[i];
		string shaderSourceString = GetShaderSourceStringFromFilePath(shaderFilePath);
		GLuint compiledShaderID = CreateAndCompileShader(shaderSourceString, shaderType);
		compiledShaders.emplace_back(compiledShaderID);
	}
	
	// Attached all compiled shaders
	for (unsigned int i = 0; i < numOfShaders; i++)
	{
		GLuint compiledShaderID = compiledShaders.at(i);
		glAttachShader(shaderProgram, compiledShaderID);
	}

	// Link the program, check for errors and finalize
	glLinkProgram(shaderProgram);
	if (!CheckProgramLinkSuccess(shaderProgram))
	{
		success = false;
	}

	// Delete the shaders after linking (we don't need them anymore)
	for (unsigned int i = 0; i < numOfShaders; i++)
	{
		GLuint compiledShaderID = compiledShaders.at(i);
		glDeleteShader(compiledShaderID);
	}

	if (!success)
	{
		return -1;
	}

	return shaderProgram;
}

// Comfort uniform functions
void SetUniformFloat(GLuint shaderID, const GLchar* uniformName, GLfloat value)
{
	glUniform1f(glGetUniformLocation(shaderID, uniformName), value);
}
void SetUniformInteger(GLuint shaderID, const GLchar* uniformName, GLint value)
{
	glUniform1i(glGetUniformLocation(shaderID, uniformName), value);
}
void SetUniformUInteger(GLuint shaderID, const GLchar* uniformName, GLuint value)
{
	glUniform1ui(glGetUniformLocation(shaderID, uniformName), value);
}
void SetUniformVector2f(GLuint shaderID, const GLchar* uniformName, GLfloat x, GLfloat y)
{
	glUniform2f(glGetUniformLocation(shaderID, uniformName), x, y);
}
void SetUniformVector2f(GLuint shaderID, const GLchar* uniformName, const vec2& value)
{
	glUniform2f(glGetUniformLocation(shaderID, uniformName), value.x, value.y);
}
void SetUniformVector2ui(GLuint shaderID, const GLchar* uniformName, const uvec2& value)
{
	glUniform2ui(glGetUniformLocation(shaderID, uniformName), value.x, value.y);
}
void SetUniformVector3f(GLuint shaderID, const GLchar* uniformName, GLfloat x, GLfloat y, GLfloat z)
{
	glUniform3f(glGetUniformLocation(shaderID, uniformName), x, y, z);
}
void SetUniformVector3f(GLuint shaderID, const GLchar* uniformName, const vec3& value)
{
	glUniform3f(glGetUniformLocation(shaderID, uniformName), value.x, value.y, value.z);
}
void SetUniformVector4f(GLuint shaderID, const GLchar* uniformName, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glUniform4f(glGetUniformLocation(shaderID, uniformName), x, y, z, w);
}
void SetUniformVector4f(GLuint shaderID, const GLchar* uniformName, const vec4& value)
{
	glUniform4f(glGetUniformLocation(shaderID, uniformName), value.x, value.y, value.z, value.w);
}
void SetUniformMatrix3(GLuint shaderID, const GLchar* uniformName, const mat3& matrix)
{
	glUniformMatrix3fv(glGetUniformLocation(shaderID, uniformName), 1, GL_FALSE, value_ptr(matrix));
}
void SetUniformMatrix4(GLuint shaderID, const GLchar* uniformName, const mat4& matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderID, uniformName), 1, GL_FALSE, value_ptr(matrix));
}