#include "./../GL/glew.h"
#include "TextureHandler.h"
#include <iostream>
#include <string>

std::map<std::string, GLuint> CTextureHandler::m_mapTextureMap;

using namespace std;
/*
*	Method	: LoadTexture
*
*	Scope	: Private
*
*	Purpose	: loads an image with the specified filename and saves the OpenGL int identifier in the map.
*
*	Returns	: GLuint (-1 if failed to load the texture)
*/
GLuint CTextureHandler::LoadTexture(const char* fileName)
{
	GLuint textureID = -1;

	const char* resultPNG = strstr(fileName, ".png");

	if (resultPNG != NULL)
	{
		GLuint texId = -1;
		if (CImageLoading::LoadImageUsingFreeImage(fileName, texId))
		{
			textureID = texId;
			m_mapTextureMap.insert(std::map<std::string, GLuint>::value_type(std::string(fileName), textureID)); // put it into the map
		}
	}
	if (textureID == -1)
	{
		std::cout << "Could not open " << fileName << std::endl;
	}

	return textureID;
}

/*
*	Method	: SearchMap
*
*	Scope	: Private
*
*	Purpose	: searches the texture map for the same filename. If found it updates the GLUint for the texture which is passed by reference
*
*	Returns	: bool (false if no texture found in the map, true if found)
*/
bool CTextureHandler::SearchMap(const char* fileName, GLuint& result)
{
	std::map<std::string, GLuint>::iterator locTexture;

	locTexture = m_mapTextureMap.find(fileName);

	if (locTexture != m_mapTextureMap.end())
	{
		result = (*locTexture).second;
		return true;
	}
	else
	{
		return false;
	}
}


/*
*	Method	: LookUpTexture
*
*	Scope	: Public
*
*	Purpose	: Looks for the filename in the map, if exists return the GLuint for the texture otherwise it loads the texture (adding it to the map)
*
*	Returns	: GLuint (-1 if texture not found or fails to load, otherwise returns the GLuint identifier for the texture)
*/
GLuint CTextureHandler::LookUpTexture(string fileName)
{
	GLuint textureID = -1;

	GLuint tempRes;
	if (SearchMap(fileName.c_str(), tempRes))
	{
		textureID = tempRes;
	}
	else
	{
		textureID = LoadTexture(fileName.c_str());
	}

	return textureID;
}

