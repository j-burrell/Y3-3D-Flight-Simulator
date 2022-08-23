#ifndef _TEXUTUREHANDLER_H
#define _TEXUTUREHANDLER_H

#include "../images/ImageLoading.h"
#include <map>
#include <string>

/**************************************
Class to handle all the textures that are loaded with the OBJ Loader

A map is used to keep a unique list of texture maps based on the filenames used to load them.

Key reason for it is to reduce bindtexture calls and repeat loading of the same images

***************************************/
class CTextureHandler
{
private:
	static std::map<std::string, GLuint>	m_mapTextureMap;
	static GLuint							LoadTexture(const char* fileName);
	static bool								SearchMap(const char* fileName, GLuint& result);

public:
	static GLuint							LookUpTexture(std::string fileName);
};

#endif