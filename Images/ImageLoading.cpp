
#include "ImageLoading.h"

#include "FreeImage.h"

#include <iostream>
using namespace std;

/*
*	Method	: LoadImage
*
*	Scope	: Public
*
*	Purpose	: Loads an image and sets it up for use with the GLUint gl_texID
*
*	Returns	: true or false - true if the image loaded fine.
*/
bool CImageLoading::LoadImageUsingFreeImage(const char* pc_Filename, GLuint& gl_texID)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP* dib = nullptr;
	//pointer to the image data
	BYTE* bits = nullptr;
	//image width and height
	//unsigned int width(0), height(0);

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(pc_Filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(pc_Filename);
	//if still unkown, return failure
	if (fif == FIF_UNKNOWN)
		return false;

	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, pc_Filename);
	//if the image failed to load, return failure
	if (!dib)
		return false;

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	int outWidth = FreeImage_GetWidth(dib);
	int outHeight = FreeImage_GetHeight(dib);

	//if one of these failed (they shouldn't), return failure
	if ((bits == 0) || (outWidth == 0) || (outHeight == 0))
		return false;

	FREE_IMAGE_COLOR_TYPE colType = FreeImage_GetColorType(dib);

	//bits are stored in BGR order.
	if (colType == FIC_RGB)
	{
		glGenTextures(1, &gl_texID);
		glBindTexture(GL_TEXTURE_2D, gl_texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outWidth,
			outHeight, 0, GL_BGR, GL_UNSIGNED_BYTE,
			bits);
	}
	else if (colType == FIC_RGBALPHA)
	{
		glGenTextures(1, &gl_texID);
		glBindTexture(GL_TEXTURE_2D, gl_texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outWidth,
			outHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
			bits);
	}
	else
	{
		cout << "Image is not RGB or RGBA" << endl;
		FreeImage_Unload(dib);
		return false;
	}

	FreeImage_Unload(dib);

	return true;
}

/*
*	Method	: LoadImage
*
*	Scope	: Public
*
*	Purpose	: Loads an image and sets it up for use with the GLUint gl_texID
*
*	Returns	: true or false - true if the image loaded fine.
*/
bool CImageLoading::LoadImageUsingFreeImage(const char* pc_Filename, GLuint& gl_texID, int& imageWidth, int& imageHeight)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP* dib = nullptr;
	//pointer to the image data
	BYTE* bits = nullptr;
	//image width and height
	//unsigned int width(0), height(0);

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(pc_Filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(pc_Filename);
	//if still unkown, return failure
	if (fif == FIF_UNKNOWN)
		return false;

	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, pc_Filename);
	//if the image failed to load, return failure
	if (!dib)
		return false;

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	imageWidth = FreeImage_GetWidth(dib);
	imageHeight = FreeImage_GetHeight(dib);

	//if one of these failed (they shouldn't), return failure
	if ((bits == 0) || (imageWidth == 0) || (imageHeight == 0))
		return false;

	FREE_IMAGE_COLOR_TYPE colType = FreeImage_GetColorType(dib);

	//bits are stored in BGR order.
	if (colType == FIC_RGB)
	{
		glGenTextures(1, &gl_texID);
		glBindTexture(GL_TEXTURE_2D, gl_texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth,
			imageHeight, 0, GL_BGR, GL_UNSIGNED_BYTE,
			bits);
	}
	else if (colType == FIC_RGBALPHA)
	{
		glGenTextures(1, &gl_texID);
		glBindTexture(GL_TEXTURE_2D, gl_texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth,
			imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
			bits);
	}
	else
	{
		cout << "Image is not RGB or RGBA" << endl;
		FreeImage_Unload(dib);
		return false;
	}

	FreeImage_Unload(dib);

	return true;
}