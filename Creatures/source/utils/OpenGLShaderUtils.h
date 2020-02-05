#pragma once
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace glm;
using namespace std;

bool CheckShaderCompileSuccess(GLuint obj);
bool CheckProgramLinkSuccess(GLuint obj);
string GetShaderSourceStringFromFilePath(const char* shaderFilePath);
GLuint CreateAndCompileShader(string& shaderSourceString, GLenum shaderType);
GLuint CreateLinkedShaderProgram(int numOfShaders, GLuint* shaderTypes, const char** shaderFilePaths);


// Comfort uniform functions
void SetUniformFloat(GLuint shaderID, const GLchar* uniformName, GLfloat value);
void SetUniformInteger(GLuint shaderID, const GLchar* uniformName, GLint value);
void SetUniformUInteger(GLuint shaderID, const GLchar* uniformName, GLuint value);
void SetUniformVector2f(GLuint shaderID, const GLchar* uniformName, GLfloat x, GLfloat y);
void SetUniformVector2f(GLuint shaderID, const GLchar* uniformName, const vec2& value);
void SetUniformVector2ui(GLuint shaderID, const GLchar* uniformName, const uvec2& value);
void SetUniformVector3f(GLuint shaderID, const GLchar* uniformName, GLfloat x, GLfloat y, GLfloat z);
void SetUniformVector3f(GLuint shaderID, const GLchar* uniformName, const vec3& value);
void SetUniformVector4f(GLuint shaderID, const GLchar* uniformName, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void SetUniformVector4f(GLuint shaderID, const GLchar* uniformName, const vec4& value);
void SetUniformMatrix3(GLuint shaderID, const GLchar* uniformName, const mat3& matrix);
void SetUniformMatrix4(GLuint shaderID, const GLchar* uniformName, const mat4& matrix);