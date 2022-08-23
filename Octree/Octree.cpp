#include "./../GL/glew.h"
#include <iostream>
#include <vector>
using namespace std;

#include "Octree.h"
#include "./../Box/Box.h"
#include "../3DStruct/threeDModel.h"

const int FACE_LIMIT = 1;

/*
*	Method	: Octree
*
*	Scope	: public
*
*	Purpose	: Default constructor
*
*/
COctree::COctree()
{
	m_dMinX = 0;
	m_dMinY = 0;
	m_dMinZ = 0;

	m_dMaxX = 0;
	m_dMaxY = 0;
	m_dMaxZ = 0;

	m_iPrimitiveListSize = -1;
	m_piPrimitiveList = nullptr;

	m_iVertexListSize = -1;
	m_piVertexList = nullptr;

	m_iLevel = 0;

	for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
	{
		m_pobChildren[i] = nullptr;
	}

	m_pobBox = nullptr;
}

/*
*	Method	: ~Octree
*
*	Scope	: public
*
*	Purpose	: Destructor 
*/
COctree::~COctree()
{
	if (m_iLevel >= MAX_DEPTH)
	{
		delete[] m_piPrimitiveList;
		m_iPrimitiveListSize = 0;

		delete[] m_piVertexList;
		m_iVertexListSize = 0;
	}
	else
	{
		for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
		{
			if (m_pobChildren[i] != nullptr)
			{
				delete m_pobChildren[i];
				m_pobChildren[i] = nullptr;
			}
		}
		delete[] m_piPrimitiveList;
		delete[] m_piVertexList;
	}
}

/*
*	Method	: SetUpOctree
*
*	Scope	: private
*
*	Purpose	: sets the dimensions of the octree node and sets the primitive and vertex indices that are 
*			  contained within the octree.
*
*	Returns	: void
*/
void COctree::SetUpOctree(int L, double x, double y, double z, double X, double Y, double Z, int* PrimList, int PrimListSize, int* VertList, int VertListSize)
{
	m_iLevel = L;

	m_dMinX = x;
	m_dMinY = y;
	m_dMinZ = z;

	m_dMaxX = X;
	m_dMaxY = Y;
	m_dMaxZ = Z;

	if (PrimListSize > 0)
	{
		m_iPrimitiveListSize = PrimListSize;

		m_piPrimitiveList = new int[m_iPrimitiveListSize];

		memcpy(m_piPrimitiveList, PrimList, sizeof(int) * m_iPrimitiveListSize);
	}
	else
		m_iPrimitiveListSize = 0;

	if (VertListSize > 0)
	{
		m_iVertexListSize = VertListSize;

		m_piVertexList = new int[m_iVertexListSize];

		memcpy(m_piVertexList, VertList, sizeof(int) * m_iVertexListSize);
	}
	else
		m_iVertexListSize = 0;
}

/*
*	Method	: SetUpRootFrom3DModel
*
*	Scope	: public
*
*	Purpose	: called for setting up the root node of an octree. The primitive and vertex information is 
*			  obtained from the threeDModel object passed in.
*
*	Returns	: void
*/
void COctree::SetUpRootFrom3DModel(int L, double x, double y, double z, double X, double Y, double Z, CThreeDModel* obj)
{
	m_iLevel = L;

	m_dMinX = x;
	m_dMinY = y;
	m_dMinZ = z;

	m_dMaxX = X;
	m_dMaxY = Y;
	m_dMaxZ = Z;

	m_iPrimitiveListSize = obj->GetOctreeTriangleListSize();

	if (m_iPrimitiveListSize > 0)
	{
		m_piPrimitiveList = new int[m_iPrimitiveListSize];

		for (int i = 0; i < m_iPrimitiveListSize; i++)
		{
			m_piPrimitiveList[i] = i;
		}
	}

	m_iVertexListSize = obj->GetOctreeVertexListSize();

	if (m_iVertexListSize > 0)
	{
		m_piVertexList = new int[m_iVertexListSize];

		for (int i = 0; i < m_iVertexListSize; i++)
		{
			m_piVertexList[i] = i;
		}
	}
}

/*
*	Method	: CreateChildren
*
*	Scope	: public
*
*	Purpose	: Used in octree construction to create the children to an octree node.
*			  The children are added to the stack passed in.
*
*	Returns	: void
*/
void COctree::CreateChildren(vector<COctree*>& stackOctree, CThreeDModel* obj)
{
	if (m_iLevel >= MAX_DEPTH)
	{
		return;
	}
	int PrimitiveSetsCount[NUM_OF_OCTREE_CHILDREN];
	int** PrimitiveSets = new int* [NUM_OF_OCTREE_CHILDREN];
	for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
	{
		PrimitiveSets[i] = new int[m_iPrimitiveListSize];
		PrimitiveSetsCount[i] = 0;
		m_pobChildren[i] = nullptr;
	}

	int VertexSetsCount[NUM_OF_OCTREE_CHILDREN];
	int** VertexSets = new int* [NUM_OF_OCTREE_CHILDREN];
	for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
	{
		VertexSets[i] = new int[m_iVertexListSize];
		VertexSetsCount[i] = 0;
	}


	for (int i = 0; i < m_iVertexListSize; i++)
	{
		CalculateVerticesWithinSubdividedOctreeCells(obj, m_piVertexList[i], VertexSets, VertexSetsCount);
	}

	for (int i = 0; i < m_iPrimitiveListSize; i++)
	{
		CalculatePrimitiveWithinSubdividedOctreeCells(obj, m_piPrimitiveList[i], PrimitiveSets, PrimitiveSetsCount);
	}

	if (PrimitiveSetsCount[0] > 0)
	{
		m_pobChildren[0] = new COctree();
		m_pobChildren[0]->SetUpOctree(m_iLevel + 1, m_dMinX, m_dMinY, m_dMinZ, (m_dMaxX + m_dMinX) / 2.0, (m_dMaxY + m_dMinY) / 2.0, (m_dMinZ + m_dMaxZ) / 2.0, PrimitiveSets[0], PrimitiveSetsCount[0], VertexSets[0], VertexSetsCount[0]);

		stackOctree.push_back(m_pobChildren[0]);
	}

	if (PrimitiveSetsCount[1] > 0)
	{
		m_pobChildren[1] = new COctree();
		m_pobChildren[1]->SetUpOctree(m_iLevel + 1, m_dMinX, m_dMinY, (m_dMinZ + m_dMaxZ) / 2.0, (m_dMaxX + m_dMinX) / 2.0, (m_dMaxY + m_dMinY) / 2.0, m_dMaxZ, PrimitiveSets[1], PrimitiveSetsCount[1], VertexSets[1], VertexSetsCount[1]);
		stackOctree.push_back(m_pobChildren[1]);
	}

	if (PrimitiveSetsCount[2] > 0)
	{
		m_pobChildren[2] = new COctree();
		m_pobChildren[2]->SetUpOctree(m_iLevel + 1, m_dMinX, (m_dMaxY + m_dMinY) / 2.0, m_dMinZ, (m_dMaxX + m_dMinX) / 2.0, m_dMaxY, (m_dMinZ + m_dMaxZ) / 2.0, PrimitiveSets[2], PrimitiveSetsCount[2], VertexSets[2], VertexSetsCount[2]);
		stackOctree.push_back(m_pobChildren[2]);
	}

	if (PrimitiveSetsCount[3] > 0)
	{
		m_pobChildren[3] = new COctree();
		m_pobChildren[3]->SetUpOctree(m_iLevel + 1, m_dMinX, (m_dMaxY + m_dMinY) / 2.0, (m_dMinZ + m_dMaxZ) / 2.0, (m_dMaxX + m_dMinX) / 2.0, m_dMaxY, m_dMaxZ, PrimitiveSets[3], PrimitiveSetsCount[3], VertexSets[3], VertexSetsCount[3]);
		stackOctree.push_back(m_pobChildren[3]);
	}

	if (PrimitiveSetsCount[4] > 0)
	{
		m_pobChildren[4] = new COctree();
		m_pobChildren[4]->SetUpOctree(m_iLevel + 1, (m_dMaxX + m_dMinX) / 2.0, m_dMinY, m_dMinZ, m_dMaxX, (m_dMaxY + m_dMinY) / 2.0, (m_dMinZ + m_dMaxZ) / 2.0, PrimitiveSets[4], PrimitiveSetsCount[4], VertexSets[4], VertexSetsCount[4]);
		stackOctree.push_back(m_pobChildren[4]);
	}

	if (PrimitiveSetsCount[5] > 0)
	{
		m_pobChildren[5] = new COctree();
		m_pobChildren[5]->SetUpOctree(m_iLevel + 1, (m_dMaxX + m_dMinX) / 2.0, m_dMinY, (m_dMinZ + m_dMaxZ) / 2.0, m_dMaxX, (m_dMaxY + m_dMinY) / 2.0, m_dMaxZ, PrimitiveSets[5], PrimitiveSetsCount[5], VertexSets[5], VertexSetsCount[5]);
		stackOctree.push_back(m_pobChildren[5]);
	}

	if (PrimitiveSetsCount[6] > 0)
	{
		m_pobChildren[6] = new COctree();
		m_pobChildren[6]->SetUpOctree(m_iLevel + 1, (m_dMaxX + m_dMinX) / 2.0, (m_dMaxY + m_dMinY) / 2.0, m_dMinZ, m_dMaxX, m_dMaxY, (m_dMinZ + m_dMaxZ) / 2.0, PrimitiveSets[6], PrimitiveSetsCount[6], VertexSets[6], VertexSetsCount[6]);
		stackOctree.push_back(m_pobChildren[6]);
	}

	if (PrimitiveSetsCount[7] > 0)
	{
		m_pobChildren[7] = new COctree();
		m_pobChildren[7]->SetUpOctree(m_iLevel + 1, (m_dMaxX + m_dMinX) / 2.0, (m_dMaxY + m_dMinY) / 2.0, (m_dMinZ + m_dMaxZ) / 2.0, m_dMaxX, m_dMaxY, m_dMaxZ, PrimitiveSets[7], PrimitiveSetsCount[7], VertexSets[7], VertexSetsCount[7]);
		stackOctree.push_back(m_pobChildren[7]);
	}

	for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
	{
		delete[] PrimitiveSets[i];
	}

	delete[] PrimitiveSets;

	for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
	{
		delete[] VertexSets[i];
	}

	delete[] VertexSets;
}

/*
*	Method	: CalculateVerticesWithinSubdividedOctreeCells
*
*	Scope	: private
*
*	Purpose	: Used in CreateChildren method to determine which of the children to the current node
*			  will contain vertices from the parent.
*
*	Returns	: void
*/
void COctree::CalculateVerticesWithinSubdividedOctreeCells(CThreeDModel* obj, int vertexIndex, int** vertexSets, int vertexSetsCount[8])
{
	//determine which vertices are in which boxes
	double min[3];
	double max[3];

	min[0] = m_dMinX;
	min[1] = m_dMinY;
	min[2] = m_dMinZ;
	max[0] = (m_dMinX + m_dMaxX) / 2.0;
	max[1] = (m_dMaxY + m_dMinY) / 2.0;
	max[2] = (m_dMinZ + m_dMaxZ) / 2.0;

	if (obj->IsVertexIntersectingAABB(min, max, vertexIndex))
	{
		vertexSets[0][vertexSetsCount[0]] = vertexIndex;
		vertexSetsCount[0]++;
	}

	min[0] = m_dMinX;
	min[1] = m_dMinY;
	min[2] = (m_dMinZ + m_dMaxZ) / 2.0;
	max[0] = (m_dMinX + m_dMaxX) / 2.0;
	max[1] = (m_dMaxY + m_dMinY) / 2.0;
	max[2] = m_dMaxZ;

	if (obj->IsVertexIntersectingAABB(min, max, vertexIndex))
	{
		vertexSets[1][vertexSetsCount[1]] = vertexIndex;
		vertexSetsCount[1]++;
	}

	min[0] = m_dMinX;
	min[1] = ((m_dMaxY + m_dMinY) / 2.0);
	min[2] = m_dMinZ;
	max[0] = (m_dMinX + m_dMaxX) / 2.0;
	max[1] = m_dMaxY;
	max[2] = (m_dMinZ + m_dMaxZ) / 2.0;

	if (obj->IsVertexIntersectingAABB(min, max, vertexIndex))
	{
		vertexSets[2][vertexSetsCount[2]] = vertexIndex;
		vertexSetsCount[2]++;
	}

	min[0] = m_dMinX;
	min[1] = ((m_dMaxY + m_dMinY) / 2.0);
	min[2] = (m_dMaxZ + m_dMinZ) / 2.0;
	max[0] = (m_dMinX + m_dMaxX) / 2.0;
	max[1] = m_dMaxY;
	max[2] = m_dMaxZ;

	if (obj->IsVertexIntersectingAABB(min, max, vertexIndex))
	{
		vertexSets[3][vertexSetsCount[3]] = vertexIndex;
		vertexSetsCount[3]++;
	}

	min[0] = (m_dMinX + m_dMaxX) / 2.0;
	min[1] = m_dMinY;
	min[2] = m_dMinZ;
	max[0] = m_dMaxX;
	max[1] = ((m_dMaxY + m_dMinY) / 2.0);
	max[2] = (m_dMaxZ + m_dMinZ) / 2.0;

	if (obj->IsVertexIntersectingAABB(min, max, vertexIndex))
	{
		vertexSets[4][vertexSetsCount[4]] = vertexIndex;
		vertexSetsCount[4]++;
	}

	min[0] = (m_dMinX + m_dMaxX) / 2.0;
	min[1] = m_dMinY;
	min[2] = (m_dMaxZ + m_dMinZ) / 2.0;
	max[0] = m_dMaxX;
	max[1] = ((m_dMaxY + m_dMinY) / 2.0);
	max[2] = m_dMaxZ;

	if (obj->IsVertexIntersectingAABB(min, max, vertexIndex))
	{
		vertexSets[5][vertexSetsCount[5]] = vertexIndex;
		vertexSetsCount[5]++;
	}

	min[0] = (m_dMinX + m_dMaxX) / 2.0;
	min[1] = ((m_dMaxY + m_dMinY) / 2.0);
	min[2] = m_dMinZ;
	max[0] = m_dMaxX;
	max[1] = m_dMaxY;
	max[2] = (m_dMinZ + m_dMaxZ) / 2.0;

	if (obj->IsVertexIntersectingAABB(min, max, vertexIndex))
	{
		vertexSets[6][vertexSetsCount[6]] = vertexIndex;
		vertexSetsCount[6]++;
	}

	min[0] = (m_dMinX + m_dMaxX) / 2.0;
	min[1] = ((m_dMaxY + m_dMinY) / 2.0);
	min[2] = (m_dMinZ + m_dMaxZ) / 2.0;
	max[0] = m_dMaxX;
	max[1] = m_dMaxY;
	max[2] = m_dMaxZ;

	if (obj->IsVertexIntersectingAABB(min, max, vertexIndex))
	{
		vertexSets[7][vertexSetsCount[7]] = vertexIndex;
		vertexSetsCount[7]++;
	}
}

/*
*	Method	: CalculatePrimitiveWithinSubdividedOctreeCells
*
*	Scope	: private
*
*	Purpose	: Used in CreateChildren method to determine which of the children to the current node
*			  will contain triangles from the parent.
*
*	Returns	: void
*/
void COctree::CalculatePrimitiveWithinSubdividedOctreeCells(CThreeDModel* obj, int primitiveIndex, int** primitiveSets, int primitiveSetsCount[8])
{
	double boxCenter[3];
	double boxHalfSize[3];

	double inc = 1.001;

	boxCenter[0] = ((m_dMaxX + m_dMinX) / 2.0 + m_dMinX) / 2.0;
	boxCenter[1] = ((m_dMaxY + m_dMinY) / 2.0 + m_dMinY) / 2.0;
	boxCenter[2] = ((m_dMinZ + m_dMaxZ) / 2.0 + m_dMinZ) / 2.0;
	boxHalfSize[0] = (boxCenter[0] - m_dMinX);
	boxHalfSize[1] = (boxCenter[1] - m_dMinY);
	boxHalfSize[2] = (boxCenter[2] - m_dMinZ);
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if (obj->IsTriangleIntersectingAABB(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[0][primitiveSetsCount[0]] = primitiveIndex;
		primitiveSetsCount[0]++;
	}

	boxCenter[0] = ((m_dMaxX + m_dMinX) / 2.0 + m_dMinX) / 2.0;
	boxCenter[1] = ((m_dMaxY + m_dMinY) / 2.0 + m_dMinY) / 2.0;
	boxCenter[2] = (m_dMaxZ + ((m_dMinZ + m_dMaxZ) / 2.0)) / 2.0;
	boxHalfSize[0] = boxCenter[0] - m_dMinX;
	boxHalfSize[1] = boxCenter[1] - m_dMinY;
	boxHalfSize[2] = boxCenter[2] - (m_dMinZ + m_dMaxZ) / 2.0;
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if (obj->IsTriangleIntersectingAABB(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[1][primitiveSetsCount[1]] = primitiveIndex;
		primitiveSetsCount[1]++;
	}

	boxCenter[0] = ((m_dMaxX + m_dMinX) / 2.0 + m_dMinX) / 2.0;
	boxCenter[1] = (m_dMaxY + ((m_dMaxY + m_dMinY) / 2.0)) / 2.0;
	boxCenter[2] = ((m_dMinZ + m_dMaxZ) / 2.0 + m_dMinZ) / 2.0;
	boxHalfSize[0] = boxCenter[0] - m_dMinX;
	boxHalfSize[1] = boxCenter[1] - ((m_dMaxY + m_dMinY) / 2.0);
	boxHalfSize[2] = boxCenter[2] - m_dMinZ;
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if (obj->IsTriangleIntersectingAABB(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[2][primitiveSetsCount[2]] = primitiveIndex;
		primitiveSetsCount[2]++;
	}

	boxCenter[0] = ((m_dMaxX + m_dMinX) / 2.0 + m_dMinX) / 2.0;
	boxCenter[1] = (m_dMaxY + ((m_dMaxY + m_dMinY) / 2.0)) / 2.0;
	boxCenter[2] = (m_dMaxZ + ((m_dMaxZ + m_dMinZ) / 2.0)) / 2.0;
	boxHalfSize[0] = boxCenter[0] - m_dMinX;
	boxHalfSize[1] = boxCenter[1] - ((m_dMaxY + m_dMinY) / 2.0);
	boxHalfSize[2] = boxCenter[2] - ((m_dMaxZ + m_dMinZ) / 2.0);
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if (obj->IsTriangleIntersectingAABB(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[3][primitiveSetsCount[3]] = primitiveIndex;
		primitiveSetsCount[3]++;
	}

	boxCenter[0] = (m_dMaxX + ((m_dMaxX + m_dMinX) / 2.0)) / 2.0;
	boxCenter[1] = ((m_dMaxY + m_dMinY) / 2.0 + m_dMinY) / 2.0;
	boxCenter[2] = ((m_dMinZ + m_dMaxZ) / 2.0 + m_dMinZ) / 2.0;
	boxHalfSize[0] = boxCenter[0] - ((m_dMaxX + m_dMinX) / 2.0);
	boxHalfSize[1] = boxCenter[1] - m_dMinY;
	boxHalfSize[2] = boxCenter[2] - m_dMinZ;
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if (obj->IsTriangleIntersectingAABB(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[4][primitiveSetsCount[4]] = primitiveIndex;
		primitiveSetsCount[4]++;
	}

	boxCenter[0] = (m_dMaxX + ((m_dMaxX + m_dMinX) / 2.0)) / 2.0;
	boxCenter[1] = ((m_dMaxY + m_dMinY) / 2.0 + m_dMinY) / 2.0;
	boxCenter[2] = (m_dMaxZ + ((m_dMaxZ + m_dMinZ) / 2.0)) / 2.0;
	boxHalfSize[0] = boxCenter[0] - ((m_dMaxX + m_dMinX) / 2.0);
	boxHalfSize[1] = boxCenter[1] - m_dMinY;
	boxHalfSize[2] = boxCenter[2] - ((m_dMaxZ + m_dMinZ) / 2.0);
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if (obj->IsTriangleIntersectingAABB(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[5][primitiveSetsCount[5]] = primitiveIndex;
		primitiveSetsCount[5]++;
	}

	boxCenter[0] = (m_dMaxX + ((m_dMaxX + m_dMinX) / 2.0)) / 2.0;
	boxCenter[1] = (m_dMaxY + ((m_dMaxY + m_dMinY) / 2.0)) / 2.0;
	boxCenter[2] = ((m_dMinZ + m_dMaxZ) / 2.0 + m_dMinZ) / 2.0;
	boxHalfSize[0] = boxCenter[0] - ((m_dMaxX + m_dMinX) / 2.0);
	boxHalfSize[1] = boxCenter[1] - ((m_dMaxY + m_dMinY) / 2.0);
	boxHalfSize[2] = boxCenter[2] - m_dMinZ;
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if (obj->IsTriangleIntersectingAABB(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[6][primitiveSetsCount[6]] = primitiveIndex;
		primitiveSetsCount[6]++;
	}

	boxCenter[0] = (m_dMaxX + ((m_dMaxX + m_dMinX) / 2.0)) / 2.0;
	boxCenter[1] = (m_dMaxY + ((m_dMaxY + m_dMinY) / 2.0)) / 2.0;
	boxCenter[2] = (m_dMaxZ + ((m_dMaxZ + m_dMinZ) / 2.0)) / 2.0;
	boxHalfSize[0] = boxCenter[0] - ((m_dMaxX + m_dMinX) / 2.0);
	boxHalfSize[1] = boxCenter[1] - ((m_dMaxY + m_dMinY) / 2.0);
	boxHalfSize[2] = boxCenter[2] - ((m_dMaxZ + m_dMinZ) / 2.0);
	boxHalfSize[0] *= inc; boxHalfSize[1] *= inc; boxHalfSize[2] *= inc;

	if (obj->IsTriangleIntersectingAABB(boxCenter, boxHalfSize, primitiveIndex))
	{
		primitiveSets[7][primitiveSetsCount[7]] = primitiveIndex;
		primitiveSetsCount[7]++;
	}
}

/*
*	Method	: DrawBoundingBox
*
*	Scope	: public
*
*	Purpose	: Renders the bounding box of the root of the octree.
*
*	Returns	: void
*/
void COctree::DrawBoundingBox(CShader* myShader)
{
	if (m_pobBox == nullptr)
	{
		m_pobBox = new CBox();
		m_pobBox->constructGeometry(myShader, m_dMinX, m_dMinY, m_dMinZ, m_dMaxX, m_dMaxY, m_dMaxZ);
	}
	else
	{
		m_pobBox->render();
	}
}

/*
*	Method	: DrawAllBoxes
*
*	Scope	: public
*
*	Purpose	: Renders the bounding box of all octree notes in the octree.
*
*	Returns	: void
*/
void COctree::DrawAllBoxes(CShader* myShader)
{
	if (m_iLevel >= MAX_DEPTH) //leaf
	{
		//draw the bounding box for a leaf node.
		DrawBoundingBox(myShader);
	}
	else
	{
		//recurse on all the children of the current node
		for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
		{
			if (m_pobChildren[i] != nullptr)
				m_pobChildren[i]->DrawAllBoxes(myShader);
		}
		
		//draw the bounding box for the non-leaf node
		DrawBoundingBox(myShader);
	}
}

/*
*	Method	: DrawOctreeLeaves
*
*	Scope	: public
*
*	Purpose	: Renders the bounding boxes of all the leaves in the octree.
*
*	Returns	: void
*/
void COctree::DrawOctreeLeaves(CShader* myShader)
{
	if (m_iLevel >= MAX_DEPTH) //leaf
	{
		//draw the bounding box for a leaf node.
		DrawBoundingBox(myShader);
	}
	else
	{
		//recurse on all the children of the current node
		for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
		{
			if (m_pobChildren[i] != nullptr)
				m_pobChildren[i]->DrawOctreeLeaves(myShader);
		}
	}
}

/*
*	Method	: CalcVertexNormals
*
*	Scope	: public
*
*	Purpose	: To calculate the vertex normals. Vertex normals are calculated by averaging the 
*			  face normals of triangles connected to the current vertex. Only triangles in the 
*			  same octree node are considered.
*
*	Returns	: void
*/
void COctree::CalcVertexNormals(CThreeDModel* model)
{
	if (m_iLevel >= MAX_DEPTH) //leaf
	{
		if (m_iVertexListSize > 0) //only draw boxes which contain vertices
		{
			model->CalcVertNormals(m_piVertexList, m_iVertexListSize, m_piPrimitiveList, m_iPrimitiveListSize);
		}
	}
	else
	{
		for (int i = 0; i < NUM_OF_OCTREE_CHILDREN; i++)
		{
			if (m_pobChildren[i] != nullptr)
				m_pobChildren[i]->CalcVertexNormals(model);
		}
	}
}
