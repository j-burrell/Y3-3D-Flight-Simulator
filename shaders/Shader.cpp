
#include "./../GL/glew.h"

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#include "Shader.h"

/////////////////////////////////////////////////////////////////////////////
// CShader

CShader::CShader()
{
	m_stName = "";
	m_uiVertexShaderID = 0;
	m_uiFragmentShaderID = 0;
	m_uiProgramObjectID = 0;
	m_bInitShader = false;
}

CShader::~CShader()
{
	if (m_bInitShader)
	{
		glUseProgram(0);
		glDeleteShader(m_uiVertexShaderID);
		glDeleteShader(m_uiFragmentShaderID);
		glDeleteProgram(m_uiProgramObjectID);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CShader member functions

/* 
 *	Method	: CreateShaderProgram
 *
 *	Scope	: Public
 *
 *	Purpose	: Loads the shaders described in files 'strVertexShaderFileName' and 'strFragmentShaderFileName'
 *			  compiles them and links them into a program 
 *
 *	Returns	: true	- the shaders were loaded successfully 
 *			  false	- couldn't load the shaders
 */
bool CShader::CreateShaderProgram(string stName, char* strVertexShaderFileName, char* strFragmentShaderFileName)
{
	GLint uiError = 0;

	if (strVertexShaderFileName == NULL || strFragmentShaderFileName == NULL)
	{
		cout << "ERROR: GLSL create failed due to invalid GLSL shader file names" <<endl;
		return false;
	}

	m_stName = stName;
	
	// create a vertex shader and set the shader source
	m_uiVertexShaderID = LoadShaderFromFile(strVertexShaderFileName, GL_VERTEX_SHADER);
	if (m_uiVertexShaderID == 0)
	{
		cout << "Couldnot load GLSL vertex shader: '" << strVertexShaderFileName << "'" << endl;
		return false;
	}

	// compile the vertex shader
	glCompileShader(m_uiVertexShaderID);
	
	//check if it compiled ok
	glGetShaderiv(m_uiVertexShaderID, GL_COMPILE_STATUS, &uiError);
	if(!uiError)
	{
		cout << "Error compiling GLSL vertex shader: '" << strVertexShaderFileName << "'" << endl;
		cout << "Shader info log:" << endl << GetShaderInfoLog(m_uiVertexShaderID) << endl;
		return false;
	}
	else
	{
		// create a fragment shader and set the shader source
		m_uiFragmentShaderID = LoadShaderFromFile(strFragmentShaderFileName, GL_FRAGMENT_SHADER);
		if (m_uiFragmentShaderID == 0)
		{
			cout << "Couldnot load GLSL fragment shader: '" << strFragmentShaderFileName << "'" << endl;
			return false;
		}

		// compile the fragment shader
		glCompileShader(m_uiFragmentShaderID);

		// check for any compiling errors
		glGetShaderiv(m_uiFragmentShaderID, GL_COMPILE_STATUS, &uiError);
		if(!uiError)
		{
			cout << "Error compiling GLSL fragment shader: '" << strFragmentShaderFileName << "'" <<endl;
			cout << "Shader info log:" << endl << GetShaderInfoLog(m_uiFragmentShaderID) <<endl;
			return false;
		}
		else
		{
			// create the program
			m_uiProgramObjectID = glCreateProgram();

			// attach the vertex and fragment shaders
			glAttachShader(m_uiProgramObjectID, m_uiVertexShaderID);
			glAttachShader(m_uiProgramObjectID, m_uiFragmentShaderID);

			// link it all together
			glLinkProgram(m_uiProgramObjectID);

			// check for any errors with the shader program
			glGetProgramiv(m_uiProgramObjectID, GL_LINK_STATUS, &uiError);
			if(!uiError)
			{
				cout << "Error linking GLSL shaders into a shader program." <<endl;
				cout << "GLSL vertex shader: '" << strVertexShaderFileName << "'" <<endl;
				cout << "GLSL fragment shader: '" << strFragmentShaderFileName << "'" <<endl;
				cout << "Program info log:" << endl << GetProgramInfoLog(m_uiProgramObjectID) <<endl;
				return false;
			}
		}
    }

	m_bInitShader = true;
	cout << "Loaded GLSL program: '" << m_stName << "'" <<endl;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CShader member functions

/*
*	Method	: CreateShaderProgram
*
*	Scope	: Public
*
*	Purpose	: Loads the shaders described in files 'strVertexShaderFileName', 'strFragmentShaderFileName'
*			   and 'strGeometrtyShaderFilename'
*			  compiles them and links them into a program
*
*	Returns	: true	- the shaders were loaded successfully
*			  false	- couldn't load the shaders
*/
bool CShader::CreateShaderProgram(std::string stName, char* strVertexShaderFileName, char* strGeometryShaderFileName, char* strFragmentShaderFileName)
{
	GLint uiError = 0;

	if (strVertexShaderFileName == NULL || strFragmentShaderFileName == NULL || strGeometryShaderFileName == NULL)
	{
		cout << "ERROR: GLSL create failed due to invalid GLSL shader file names" << endl;
		return false;
	}

	m_stName = stName;

	// create a vertex shader and set the shader source
	m_uiVertexShaderID = LoadShaderFromFile(strVertexShaderFileName, GL_VERTEX_SHADER);
	if (m_uiVertexShaderID == 0)
	{
		cout << "Could not load GLSL vertex shader: '" << strVertexShaderFileName << "'" << endl;
		return false;
	}

	// compile the vertex shader
	glCompileShader(m_uiVertexShaderID);

	//check if it compiled ok
	glGetShaderiv(m_uiVertexShaderID, GL_COMPILE_STATUS, &uiError);
	if (!uiError)
	{
		cout << "Error compiling GLSL vertex shader: '" << strVertexShaderFileName << "'" << endl;
		cout << "Shader info log:" << endl << GetShaderInfoLog(m_uiVertexShaderID) << endl;
		return false;
	}
	else
	{
		// create a fragment shader and set the shader source
		m_uiFragmentShaderID = LoadShaderFromFile(strFragmentShaderFileName, GL_FRAGMENT_SHADER);
		if (m_uiFragmentShaderID == 0)
		{
			cout << "Could not load GLSL fragment shader: '" << strFragmentShaderFileName << "'" << endl;
			return false;
		}

		// compile the fragment shader
		glCompileShader(m_uiFragmentShaderID);

		// check for any compiling errors
		glGetShaderiv(m_uiFragmentShaderID, GL_COMPILE_STATUS, &uiError);
		if (!uiError)
		{
			cout << "Error compiling GLSL fragment shader: '" << strFragmentShaderFileName << "'" << endl;
			cout << "Shader info log:" << endl << GetShaderInfoLog(m_uiFragmentShaderID) << endl;
			return false;
		}
		else
		{
			// create the program
			m_uiProgramObjectID = glCreateProgram();

			// attach the vertex and fragment shaders
			glAttachShader(m_uiProgramObjectID, m_uiVertexShaderID);
			glAttachShader(m_uiProgramObjectID, m_uiFragmentShaderID);

			// create a fragment shader and set the shader source
			m_uiGeometryShader = LoadShaderFromFile(strGeometryShaderFileName, GL_GEOMETRY_SHADER);
			if (m_uiGeometryShader == 0)
			{
				cout << "Could not load GLSL Geometry shader: '" << strGeometryShaderFileName << "'" << endl;
				return false;
			}

			// compile the fragment shader
			glCompileShader(m_uiGeometryShader);

			// check for any compiling errors
			glGetShaderiv(m_uiGeometryShader, GL_COMPILE_STATUS, &uiError);
			if (!uiError)
			{
				cout << "Error compiling GLSL fragment shader: '" << strGeometryShaderFileName << "'" << endl;
				cout << "Shader info log:" << endl << GetShaderInfoLog(m_uiGeometryShader) << endl;
				return false;
			}

			glAttachShader(m_uiProgramObjectID, m_uiGeometryShader);

			// link it all together
			glLinkProgram(m_uiProgramObjectID);

			// check for any errors with the shader program
			glGetProgramiv(m_uiProgramObjectID, GL_LINK_STATUS, &uiError);
			if (!uiError)
			{
				cout << "Error linking GLSL shaders into a shader program." << endl;
				cout << "GLSL vertex shader: '" << strVertexShaderFileName << "'" << endl;
				cout << "GLSL fragment shader: '" << strFragmentShaderFileName << "'" << endl;
				cout << "Program info log:" << endl << GetProgramInfoLog(m_uiProgramObjectID) << endl;
				return false;
			}
		}
	}

	m_bInitShader = true;
	cout << "Loaded GLSL program: '" << m_stName << "'" << endl;
	return true;
}

/* 
 *	Method	: GetProgramObjID
 *
 *	Scope	: Private
 *
 *	Purpose	: Returns the shader program needed for rendering
 *
 *	Returns	: the shader program needed for rendering
 */
GLuint CShader::GetProgramObjID()
{ 
	return m_uiProgramObjectID; 
} 

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// PRIVATE METHOD SECTION //////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/* 
 *	Method	: LoadFileData
 *
 *	Scope	: Private
 *
 *	Purpose	: Reads a file and returns its contents in a string
 *
 *	Returns	: the file contents in a string
 */
string CShader::LoadFileData(char* stFileName)
{
	string			stShaderSource = "";
	unsigned int	uiFileSize = 0;

	if (stFileName != NULL)
	{
		ifstream		obInFile(stFileName, ios::binary);

		if(obInFile.is_open())
		{
			obInFile.seekg(0, ios::end);

			//static cast used to convert between types.
			uiFileSize = static_cast<unsigned int>(obInFile.tellg());
			stShaderSource.resize(uiFileSize);
			obInFile.seekg(0, ios::beg);
			obInFile.read(&stShaderSource[0], uiFileSize);
		}
	}
	else
		cout << "ERROR: Invalid GLSL shader file name" <<endl;

	return stShaderSource;
}

/* 
 *	Method	: LoadShaderFromFile
 *
 *	Scope	: Private
 *
 *	Purpose	: Reads the shader from a file and returns the shader source ID
 *
 *	Returns	: the shader source ID or 0 otherwise
 */
GLuint CShader::LoadShaderFromFile(char* stFileName, GLenum uiEnumType)
{
	GLuint			uiShaderID = 0;
	const GLchar*	stGlSource = NULL;
	string			stRet;

	stRet = LoadFileData(stFileName);

	if (!stRet.empty())
	{
		uiShaderID = glCreateShader(uiEnumType);
		// the reinterpret cast is used to convert between pointer types
		stGlSource = reinterpret_cast<const GLchar*>(stRet.c_str());
		glShaderSource(uiShaderID, 1, &stGlSource, 0);
	}
	else
		cout << "ERROR: The GLSL shader "<< stFileName << " is empty" <<endl;

	return uiShaderID;
}

/* 
 *	Method	: GetShaderInfoLog
 *
 *	Scope	: Private
 *
 *	Purpose	: Returns shader errors and other info in a string
 *
 *	Returns	: a string containing shader errors and other info
 */
string CShader::GetShaderInfoLog(GLuint uiShaderID)
{
	GLsizei		iInfoLogSize = 0;
	string		stInfoLogTxt;

	glGetShaderiv(uiShaderID, GL_INFO_LOG_LENGTH, &iInfoLogSize);
	stInfoLogTxt.resize(iInfoLogSize);
	glGetShaderInfoLog(uiShaderID, iInfoLogSize, &iInfoLogSize, &stInfoLogTxt[0]);
	return stInfoLogTxt;
}

/* 
 *	Method	: GetProgramInfoLog
 *
 *	Scope	: Private
 *
 *	Purpose	: Returns shading program errors and other info in a string
 *
 *	Returns	: a string containing hading program errors and other info
 */
string CShader::GetProgramInfoLog(GLuint uiProgramID)
{
	GLsizei		iInfoLogSize = 0;
	string		stInfoLogTxt;

	glGetProgramiv(uiProgramID, GL_INFO_LOG_LENGTH, &iInfoLogSize);
	stInfoLogTxt.resize(iInfoLogSize);
	glGetProgramInfoLog(uiProgramID, iInfoLogSize, &iInfoLogSize, &stInfoLogTxt[0]);
	return stInfoLogTxt;
}


