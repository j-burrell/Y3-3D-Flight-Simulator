/*************************************************************/
/*                           OBJ                             */
/*                                                           */
/* Purpose: classes to open and parse the .obj and .mtl files*/ 
/*         exported from Alias|Wavefront's OBJ file          */
/*                                                           */
/*************************************************************/
#ifndef OBJ_H
#define OBJ_H

//-------------------------------------------------------------
//                       INCLUDES                             -
//-------------------------------------------------------------
#include <vector>
#include "./../Structures/Vector3d.h"
#include "./../GL/glew.h"
#include <string>

//to store the mtl file data for an OBJ file
class ObjMat
{
public:
	//Loaded from the File
	char			m_carrMatName[255];
	float			m_farrAmbientCol[3];
	float			m_farrDiffuseCol[3];
	float			m_farrSpecularCol[3];
	float			m_fTransparency;
	float			m_fSpecularExponent;
	int				m_iIlluminationModel;
	char			m_carrTextureName[255];

	//Created
	GLuint			m_iGLTextureIndex;

	ObjMat()
	{
		m_carrMatName[0] = ' ';
		m_carrTextureName[0] = ' ';

		m_farrAmbientCol[0] = m_farrAmbientCol[1] = m_farrAmbientCol[2] = 0.0f;
		m_farrDiffuseCol[0] = m_farrDiffuseCol[1] = m_farrDiffuseCol[2] = 0.0f;
		m_farrSpecularCol[0] = m_farrSpecularCol[1] = m_farrSpecularCol[2] = 0.0f;

		m_fTransparency			= 1.0;
		m_fSpecularExponent		= 50;
		m_iIlluminationModel	= 1;
		m_iGLTextureIndex		= -1;
	}
};

//-------------------------------------------------------------
//- ObjFace
//- Indices for each face of the OBJ file
class ObjFace
{
public:

	unsigned int	m_uiVertIdx[3];
	unsigned int	m_uiNormalIdx[3];
	unsigned int	m_uiTexCoordIdx[3];

	int				m_iMatId;

	ObjFace()
	{
		m_uiVertIdx[0] = -1;
		m_uiVertIdx[1] = -1;
		m_uiVertIdx[2] = -1;

		m_uiTexCoordIdx[0] = -1;
		m_uiTexCoordIdx[1] = -1;
		m_uiTexCoordIdx[2] = -1;

		m_uiNormalIdx[0] = 0;
		m_uiNormalIdx[1] = 0;
		m_uiNormalIdx[2] = 1;

		m_iMatId = -1;
	}
	ObjFace(unsigned int vert[3], unsigned int text[3], unsigned int norm[3], int newMatId)
	{
		//copy the data;
		m_uiVertIdx[0] = vert[0] - 1;
		m_uiVertIdx[1] = vert[1] - 1;
		m_uiVertIdx[2] = vert[2] - 1;

		m_uiTexCoordIdx[0] = text[0] - 1;
		m_uiTexCoordIdx[1] = text[1] - 1;
		m_uiTexCoordIdx[2] = text[2] - 1;

		m_uiNormalIdx[0] = norm[0] - 1;
		m_uiNormalIdx[1] = norm[1] - 1;
		m_uiNormalIdx[2] = norm[2] - 1;

		m_iMatId = newMatId;
	}
};


//-------------------------------------------------------------
//Class to load the OBJ and MTL file data into stl vectors.
class COBJLoader
{
public:
	COBJLoader();

	bool					LoadModel(std::string fileName);

	//vectors for geometry
	std::vector<Vector3d>	m_vVertices;
	std::vector<Vector3d>	m_vNormals;
	std::vector<Vector3d>	m_vTexCoords;
	std::vector<ObjFace>	m_vFaces;

	//vector for materials
	std::vector<ObjMat>		m_vMats;
		
private:
	void					SortFacesOnMaterial();
	void					ReadTriangleFaceVertTexNorm(char* line, int matId);
	bool					LoadOBJFile(const char* filename);
	bool					LoadMTLFile(const char* mainName, const char* filename);
	int						LookupMaterial(char* matName);
	void					SplitBySpaces(char* inputString, char* frontString, char* restString, int size);
};

#endif //OBJ_H