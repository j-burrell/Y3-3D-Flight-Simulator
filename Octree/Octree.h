
#pragma once

#include <vector>

/*
*	Class	: Octree
*
*	Purpose	: used to create octree for the threedmodel class. 
*			  Is used for accelerating the calculation of per vertex normals
*			  for the threeDModel class.
*/


class CThreeDModel;
class CBox;
class CShader;

const int MAX_DEPTH					=	4;   //Depth of the octree
const int NUM_OF_OCTREE_CHILDREN	=	8;   //Maximum number of children for each octree node.

class COctree
{
private:
	double			m_dMinX, m_dMinY, m_dMinZ, m_dMaxX, m_dMaxY, m_dMaxZ;
	COctree*		m_pobChildren[NUM_OF_OCTREE_CHILDREN];
	int				m_iLevel;
	int*			m_piPrimitiveList;
	int				m_iPrimitiveListSize;
	int*			m_piVertexList;
	int				m_iVertexListSize;
	CBox*			m_pobBox;

	void			CalculatePrimitiveWithinSubdividedOctreeCells(CThreeDModel* model, int primitiveIndex, int** primitiveSets, int primitiveSetsCount[8]);
	void			CalculateVerticesWithinSubdividedOctreeCells(CThreeDModel* model, int vertexIndex, int** vertexSets, int vertexSetsCount[8]);
	void			SetUpOctree(int L, double x, double y, double z, double X, double Y, double Z, int* PrimList, int PrimListSize, int* vertList, int VertListSize);

public:
	COctree();
	~COctree();
	void			SetUpRootFrom3DModel(int L, double x, double y, double z, double X, double Y, double Z, CThreeDModel* model);
	void			CreateChildren(std::vector<COctree*>& stackOctree, CThreeDModel* model);
	void			CalcVertexNormals(CThreeDModel* model);
	void			DrawBoundingBox(CShader* myShader);
	void			DrawAllBoxes(CShader* myShader);
	void			DrawOctreeLeaves(CShader* myShader);
};
