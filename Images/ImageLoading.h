#pragma once

#include "./../GL/glew.h"

/********************************
*  Class ImageLoading
* 
*  Purpose: to load png images using the FreeImage library. Used by the OBJLoading code
* 
* 
/******************************/
class CImageLoading
{
public:
	static bool		LoadImageUsingFreeImage(const char* pcName, GLuint& refGLITexId);
	static bool		LoadImageUsingFreeImage(const char* pc_Filename, GLuint& gl_texID, int& imageWidth, int& imageHeight);
};
