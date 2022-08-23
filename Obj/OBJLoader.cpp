#include "OBJLoader.h"
#include <iostream>
#include <fstream>
#include "../TextureHandler/TextureHandler.h"
#include <sstream>
#include <algorithm>
#include <string>

using namespace std;

COBJLoader::COBJLoader()
{
}

/*
*	Method	: LoadModel
*
*	Scope	: Public
*
*	Purpose	: loads an obj file with corresponding mtl file to import 3d object with texture
*
*	Returns	: bool (true if loaded, false if failed to load)
*/
bool COBJLoader::LoadModel(string fileName)
{
	m_vVertices.clear();
	m_vNormals.clear();
	m_vTexCoords.clear();
	m_vFaces.clear();
	m_vMats.clear();

	if (!LoadOBJFile(fileName.c_str()))
	{
		return false;
	}

	//sort all the triangles based on the texture maps used.
	//SortFacesOnMaterial();

	return true;
}

/*
*	Method	: SplitFrontString
*
*	Scope	: private
*
*	Purpose	: splits the char* based on spaces
*
*	Returns	: void
*/
void COBJLoader::SplitBySpaces(char* inputString, char* frontString, char* restString, int size)
{
	sscanf_s(inputString, "%s", frontString, size);

	//Get length of frontString

	int x = 0;
	while (frontString[x] != '\0') x++;
	if (inputString[0] == ' ') x++;


	int y = 0;
	while (inputString[y + x + 1] != '\0')
	{
		restString[y] = inputString[y + x + 1];
		y++;
	}
	restString[y] = '\0';


}

/*
*	Method	: ReadTriangleFaceVertTexNorm
*
*	Scope	: private
*
*	Purpose	: reads the information for a face comprising vertex, texture and normal indices.
*
*	Returns	: void
*/
void COBJLoader::ReadTriangleFaceVertTexNorm(char* line, int matId)
{
	unsigned int m_uiVertIdx[3];
	unsigned int m_uiTexCoordIdx[3];
	unsigned int m_uiNormalIdx[3];

	for (int x = 0; x < 3; x++)
	{
		char currentArg[255];
		char remStr[255];

		SplitBySpaces(line, currentArg, remStr, sizeof(char) * 255);

		sscanf_s(currentArg, "%i/%i/%i", &m_uiVertIdx[x], &m_uiTexCoordIdx[x], &m_uiNormalIdx[x]);
		memcpy_s(line, sizeof(char) * 255, remStr, 255);
	}

	ObjFace newFace(m_uiVertIdx, m_uiTexCoordIdx, m_uiNormalIdx, matId);

	m_vFaces.push_back(newFace);
}

/*
*	Method	: LoadMTLFile
*
*	Scope	: private
*
*	Purpose	: reads the MTL information
*
*	Returns	: bool (true when read, false if it fails to open the mtl file)
*/
bool COBJLoader::LoadMTLFile(const char* mainName, const char* filename)
{
	char line[255];

	std::string fullPath(mainName);

	int whereDash = (int)(fullPath.find_last_of('/'));

	std::string actualPath = fullPath.substr(0, whereDash);

	string s = actualPath + string("\\") + string(filename);

	ifstream fin;

	fin.open(s.c_str());

	if (fin.fail())

	{

		std::cout << "Can't open the file" << std::endl;

		return false;

	}

	string identifierStr;

	bool foundNewMaterial = false;

	ObjMat newMaterial;

	while (!fin.eof())

	{

		fin >> identifierStr;

		if (identifierStr == "#")

		{

			fin.getline(line, 255);

			fin >> ws;

		}
		else if (identifierStr == "newmtl")
		{
			//cout << "newmtl" <<endl;
			if (foundNewMaterial)
			{
				string s = actualPath + "\\" + newMaterial.m_carrTextureName;
				newMaterial.m_iGLTextureIndex = CTextureHandler::LookUpTexture(s);

				m_vMats.push_back(newMaterial);

			}

			fin.getline(line, 255);

			fin >> ws;

			sscanf_s(line, "%s", newMaterial.m_carrMatName, _countof(line));
			cout << newMaterial.m_carrMatName << " MATERIAL NAME>" << endl;

			foundNewMaterial = true;

		}

		else if (identifierStr == "Ns" || identifierStr == "Ni" || identifierStr == "Tr" || identifierStr == "Tf")//skip all these

		{

			fin.getline(line, 255);

			fin >> ws;

		}

		else if (identifierStr == "d")

		{

			fin.getline(line, 255);

			fin >> ws;

			sscanf_s(line, " %f", &newMaterial.m_fTransparency);

		}

		else if (identifierStr == "illum")

		{

			fin.getline(line, 255);

			fin >> ws;

			sscanf_s(line, "llum %i", &newMaterial.m_iIlluminationModel);

		}

		else if (identifierStr == "Ka")

		{

			fin.getline(line, 255); //Ambient

			sscanf_s(line, "%f %f %f", &newMaterial.m_farrAmbientCol[0], &newMaterial.m_farrAmbientCol[1], &newMaterial.m_farrAmbientCol[2]);

		}

		else if (identifierStr == "Kd")

		{

			fin.getline(line, 255); //diffuse

			fin >> ws;

			sscanf_s(line, "Kd %f %f %f", &newMaterial.m_farrDiffuseCol[0], &newMaterial.m_farrDiffuseCol[1], &newMaterial.m_farrDiffuseCol[2]);

		}

		else if (identifierStr == "Ks")

		{

			fin.getline(line, 255); //specular

			fin >> ws;

			sscanf_s(line, "Ks %f %f %f", &newMaterial.m_farrSpecularCol[0], &newMaterial.m_farrSpecularCol[1], &newMaterial.m_farrSpecularCol[2]);

		}

		else if (identifierStr == "Ke")///not used so skip

		{

			fin.getline(line, 255); //emissive

			fin >> ws;

		}

		else if (identifierStr == "map_Kd")

		{

			fin.getline(line, 255); //textureName

			fin >> ws;

			sscanf_s(line, "%s", &newMaterial.m_carrTextureName, _countof(line));

		}

		else if (identifierStr == "map_Ka")//skip not used

		{

			fin.getline(line, 255); //textureName

			fin >> ws;

		}

		else//skip anything else

		{

			fin.getline(line, 255);

			fin >> ws;

		}

	}

	if (foundNewMaterial)
	{
		string s = actualPath + "\\" + newMaterial.m_carrTextureName;
		newMaterial.m_iGLTextureIndex = CTextureHandler::LookUpTexture(s);

		cout << "MATERIAL " << newMaterial.m_carrTextureName << " " << newMaterial.m_iGLTextureIndex << endl;

		m_vMats.push_back(newMaterial);

	}

	std::cout << "Number of Materials Loaded " << (int)m_vMats.size() << std::endl;

	return true;

}

/*
*	Method	: LookupMaterial
*
*	Scope	: private
*
*	Purpose	: looks for the material in the array of materials.
*
*	Returns	: int (-1 if failed to find material, otherwise return number in the materials array)
*/
int COBJLoader::LookupMaterial(char* matName)
{
	for (int count = 0; count < (int)m_vMats.size(); count++)
	{
		if (strcmp(m_vMats[count].m_carrMatName, matName) == 0)
		{
			return count;
		}
	}
	return -1;
}

/*
*	Method	: LoadOBJFile
*
*	Scope	: private
*
*	Purpose	: loads the OBJ File.
*
*	Returns	: int (-1 if failed to open the file, otherwise return true)
*/
bool COBJLoader::LoadOBJFile(const char* filename)
{
	char line[255];

	int currentMat = -1;

	FILE* theFile;
	fopen_s(&theFile, filename, "rt");
	if (!theFile)
	{
		std::cout << "Can't open the file" << std::endl;
		return false;
	}

	while (!feof(theFile))
	{
		char firstChar = fgetc(theFile);

		switch (firstChar)
		{
		case 'v':   //It's a vertex or vertex attribut
		{
			char secondChar = fgetc(theFile);

			switch (secondChar)
			{
			case ' ':   //space or tab - must be just a vert
			case '\t':
			{
				float thePoints[3];
				fgets(line, 255, theFile); //read in the whole line				
				sscanf_s(line, " %f %f %f", &thePoints[0], &thePoints[1], &thePoints[2]); //get the vertex coords	
				m_vVertices.push_back(thePoints); //add to the vertex array
				break;
			}
			case 'n':
			{
				float theNormals[3];
				fgets(line, 255, theFile); //get the Normals						
				sscanf_s(line, " %f %f %f", &theNormals[0], &theNormals[1], &theNormals[2]); //get the normal coords							
				m_vNormals.push_back(theNormals); //add to the normal array
				break;
			}
			case 't':
			{
				float theTex[2];
				fgets(line, 255, theFile); //get the Tex						
				sscanf_s(line, " %f %f", &theTex[0], &theTex[1]); //get the texture coords							
				//theTex[1] = 1-theTex[1];							
				m_vTexCoords.push_back(theTex); //add to the text-coord array
				break;
			}
			}
			break;
		}
		case 'f': //Read in a face
		{
			char line[255];
			fgets(line, 255, theFile); //read in the whole line				

			ReadTriangleFaceVertTexNorm(line, currentMat);
			break;
		}
		case 'm': //It's the materail lib
		{
			char buff[255];
			char buff2[255];
			fgets(line, 255, theFile); //read in the whole line	

			sscanf_s(line, "%s %s", &buff, sizeof(char) * 255, &buff2, sizeof(char) * 255);
			LoadMTLFile(filename, buff2);
			break;
		}
		case 'u': //Change current Material
		{
			char buff[255];
			char buff2[255];
			fgets(line, 255, theFile); //read in the whole line	

			sscanf_s(line, "%s %s", &buff, sizeof(char) * 255, &buff2, sizeof(char) * 255);

			currentMat = LookupMaterial(buff2);

			break;
		}
		default: // A bit we don't know about - skip line
		{
			if ((firstChar != 10))
			{
				fgets(line, 255, theFile); //read and throw away
			}
		}
		}

	}



	return true;
}

static int   sortByMat(const ObjFace& tpi, const ObjFace& tpj)
{
	if (tpi.m_iMatId < tpj.m_iMatId)
	{
		return -1;
	}
	else if (tpi.m_iMatId > tpj.m_iMatId)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
*	Method	: SortFacesOnMaterial
*
*	Scope	: private
*
*	Purpose	: sorts the faces based on the textures.
*
*	Returns	: void
*/
void COBJLoader::SortFacesOnMaterial()
{
	sort(m_vFaces.begin(), m_vFaces.end(), sortByMat);
}


