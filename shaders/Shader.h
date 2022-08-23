#ifndef SHADER_H
#define SHADER_H

#include <string>

// GLSL wrapper
class CShader
{
// Data Members
public:

// Operations
public:
	CShader();
	virtual		~CShader();
	bool		CreateShaderProgram(std::string stName, char* strVertexShaderFileName, char* strFragmentShaderFileName);
	bool		CreateShaderProgram(std::string stName, char* strVertexShaderFileName, char* strGeometryShaderFileName, char* strFragmentShaderFileName);
	GLuint		GetProgramObjID();
	
// Operations
private:
	std::string		LoadFileData(char* stFileName);
	GLuint			LoadShaderFromFile(char* stFileName, GLenum uiEnumType);
	std::string		GetShaderInfoLog(GLuint uiShaderID);
	std::string		GetProgramInfoLog(GLuint uiProgramID);

// Data Members
private:
	std::string		m_stName;
	GLuint			m_uiVertexShaderID;       //identifier for the vertex shader
	GLuint			m_uiFragmentShaderID;     //identifier for the fragment shader
	GLuint			m_uiGeometryShader;		  //identifier for the geometry shader
	GLuint			m_uiProgramObjectID;      //identifier for the program- this is used when rendering
	bool			m_bInitShader;
};

#endif //#ifndef SHADER_H