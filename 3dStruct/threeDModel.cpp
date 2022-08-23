#include "./../GL/glew.h"
#include "threeDModel.h"
#include "../Obj/OBJLoader.h"
//#include <gl/glext.h>
#include <math.h>
#include "../texturehandler/texturehandler.h"
#include "../Octree/Octree.h"
#include "../Utilities/IntersectionTests.h"
#include "../shaders/Shader.h"

/*
 *	Method	: ThreeDModel
 *
 *	Scope	: Public
 *
 *	Purpose	: constructor
 *
 *	Returns	: none
 */
CThreeDModel::CThreeDModel()
{
	m_pvVertices = nullptr;
	m_pvFaceNormals = nullptr;
	m_pvTexCoords = nullptr;
	m_pobTriangles = nullptr;
	m_pvVertNormals = nullptr;

	m_iNumberOfVertices = 0;
	m_iNumberOfFaceNormals = 0;
	m_iNumberOfTriangles = 0;
	m_iNumberOfTexCoords = 0;
	m_iNumberOfMaterials = 0;
	m_iNumberOfVertNormals = 0;

	m_pobOctree = nullptr;

	m_uiNumOfVBOs = 0;
	m_uiVaoID = 0;
	m_puiVBOs = nullptr;
}

/*
 *	Method	: ~ThreeDModel
 *
 *	Scope	: Public
 *
 *	Purpose	: destructor
 *
 *	Returns	: none
 */
CThreeDModel::~CThreeDModel()
{
	if (m_pvVertices)	delete[] m_pvVertices;
	if (m_pvFaceNormals)	delete[] m_pvFaceNormals;
	if (m_pobTriangles)	delete[] m_pobTriangles;
	if (m_pvTexCoords)delete[] m_pvTexCoords;
	if (m_pvVertNormals) delete[] m_pvVertNormals;

	m_pvVertices = nullptr;
	m_pvFaceNormals = nullptr;
	m_pvTexCoords = nullptr;
	m_pobTriangles = nullptr;
	m_pvVertNormals = nullptr;

	delete[] m_puiVBOs;
	m_puiVBOs = nullptr;
	
	m_vuiFaceIndexRangeForTrisWithSameTexture.clear();
}

/*
 *	Method	: ThreeDModel(const &)
 *
 *	Scope	: Public
 *
 *	Purpose	: copy constructor
 *
 *	Returns	: none
 */
CThreeDModel::CThreeDModel(const CThreeDModel & p)
{
	*this = p;
}

/*
 *	Method	: operator=
 *
 *	Scope	: Public
 *
 *	Purpose	: overload the assignment operator.
 *
 *	Returns	: none
 */
void CThreeDModel::operator=(const CThreeDModel & p)
{
	m_iNumberOfVertices = p.m_iNumberOfVertices;
	m_iNumberOfFaceNormals = p.m_iNumberOfFaceNormals;
	m_iNumberOfTriangles = p.m_iNumberOfTriangles;
	m_iNumberOfTexCoords = p.m_iNumberOfTexCoords;
	m_iNumberOfMaterials = p.m_iNumberOfMaterials;
	m_iNumberOfVertNormals = p.m_iNumberOfVertNormals;

	
	if (p.m_pvVertices != nullptr)
	{
		m_pvVertices = new Vector3d[m_iNumberOfVertices];
		m_pvFaceNormals = new Vector3d[m_iNumberOfFaceNormals];
		m_pvTexCoords = new Vector3d[m_iNumberOfTexCoords];
		m_pobTriangles = new ObjFace[m_iNumberOfTriangles];
		m_pvVertNormals = new Vector3d[m_iNumberOfVertNormals];

		//Copy over Data
		memcpy(m_pvVertices, p.m_pvVertices, sizeof(Vector3d) * m_iNumberOfVertices);
		memcpy(m_pvFaceNormals, p.m_pvFaceNormals, sizeof(Vector3d) * m_iNumberOfFaceNormals);
		memcpy(m_pvTexCoords, p.m_pvTexCoords, sizeof(Vector3d) * m_iNumberOfTexCoords);
		memcpy(m_pobTriangles,p.m_pobTriangles, sizeof(ObjFace) * m_iNumberOfTriangles);
		memcpy(m_pvVertNormals, p.m_pvVertNormals, sizeof(Vector3d) * m_iNumberOfVertNormals);
	}
	else
	{
		m_pvVertices = nullptr;
		m_pvFaceNormals = nullptr;
		m_pvTexCoords = nullptr;
		m_pobTriangles = nullptr;
		m_pvVertNormals = nullptr;
	}

	m_vuiFaceIndexRangeForTrisWithSameTexture.clear();
	for (unsigned int i = 0; i < p.m_vuiFaceIndexRangeForTrisWithSameTexture.size(); i++)
	{
		m_vuiFaceIndexRangeForTrisWithSameTexture.push_back(p.m_vuiFaceIndexRangeForTrisWithSameTexture[i]);
	}

	if (p.m_puiVBOs != nullptr)
	{
		m_puiVBOs = new GLuint[p.m_uiNumOfVBOs];
		m_uiNumOfVBOs = p.m_uiNumOfVBOs;
		memcpy(m_puiVBOs, p.m_puiVBOs, sizeof(GLuint) * p.m_uiNumOfVBOs);
	}

	m_uiVaoID = p.m_uiVaoID;

	m_pobOctree = p.m_pobOctree;
}

/*
 *	Method	: ConstructModelFromOBJLoader
 *
 *	Scope	: Public
 *
 *	Purpose	: copy all the geometry and texture information from an OBJLoader object
 *
 *	Returns	: none
 */
void CThreeDModel::ConstructModelFromOBJLoader(COBJLoader& refOBJLoader)
{
	m_iNumberOfVertices = (int)refOBJLoader.m_vVertices.size();
	m_iNumberOfTriangles = (int)refOBJLoader.m_vFaces.size();
	m_iNumberOfTexCoords = (int)refOBJLoader.m_vTexCoords.size();
	m_iNumberOfVertNormals = (int)refOBJLoader.m_vNormals.size();
	m_iNumberOfMaterials = (int)refOBJLoader.m_vMats.size();
	m_iNumberOfFaceNormals = (int)refOBJLoader.m_vFaces.size();

	//Output models Stats	
	std::cout << "Number of Vertices " << m_iNumberOfVertices << std::endl;
	std::cout << "Number of Triangles " << m_iNumberOfTriangles << std::endl;
	std::cout << "Number of Vert Normals " << m_iNumberOfVertNormals << std::endl;
	std::cout << "Number of Face Normals " << m_iNumberOfFaceNormals << std::endl;
	std::cout << "Number of TexCoords " << m_iNumberOfTexCoords << std::endl;
	std::cout << "Number of Materials " << m_iNumberOfMaterials << std::endl;

	//Copy over the model
	m_pvFaceNormals = new Vector3d[m_iNumberOfFaceNormals];
	m_pobTriangles = new ObjFace[m_iNumberOfTriangles];
	for (int i = 0; i < m_iNumberOfTriangles; i++)
	{
		ObjFace* currentFace = &m_pobTriangles[i];

		currentFace->m_uiVertIdx[0] = refOBJLoader.m_vFaces[i].m_uiVertIdx[0];
		currentFace->m_uiVertIdx[1] = refOBJLoader.m_vFaces[i].m_uiVertIdx[1];
		currentFace->m_uiVertIdx[2] = refOBJLoader.m_vFaces[i].m_uiVertIdx[2];

		currentFace->m_uiTexCoordIdx[0] = refOBJLoader.m_vFaces[i].m_uiTexCoordIdx[0];
		currentFace->m_uiTexCoordIdx[1] = refOBJLoader.m_vFaces[i].m_uiTexCoordIdx[1];
		currentFace->m_uiTexCoordIdx[2] = refOBJLoader.m_vFaces[i].m_uiTexCoordIdx[2];

		currentFace->m_iMatId = refOBJLoader.m_vFaces[i].m_iMatId;
	}

	//Load the First Frame into the threeDModel
	m_pvVertices = new Vector3d[m_iNumberOfVertices];
	for (int i = 0; i < m_iNumberOfVertices; i++)
	{
		m_pvVertices[i] = refOBJLoader.m_vVertices[i];
	}

	//Load the Normals
	m_pvVertNormals = new Vector3d[m_iNumberOfVertNormals];
	for (int i = 0; i < m_iNumberOfVertNormals; i++)
	{
		m_pvVertNormals[i] = refOBJLoader.m_vNormals[i];
	}

	m_pvTexCoords = new Vector3d[m_iNumberOfTexCoords];
	for (int i = 0; i < m_iNumberOfTexCoords; i++)
	{
		m_pvTexCoords[i] = refOBJLoader.m_vTexCoords[i];
	}

	m_vuiFaceIndexRangeForTrisWithSameTexture.clear();
	unsigned int polyCount = 0;
	unsigned int polyCounter = 0;
	int matID = refOBJLoader.m_vMats[refOBJLoader.m_vFaces[0].m_iMatId].m_iGLTextureIndex;
	for (int x = 0; x < m_iNumberOfTriangles; x++)
	{
		if (matID != refOBJLoader.m_vMats[refOBJLoader.m_vFaces[x].m_iMatId].m_iGLTextureIndex)
		{
			std::tuple<int, int, int> t = std::make_tuple(polyCount * 3, (polyCount + polyCounter - 1) * 3, matID);
			m_vuiFaceIndexRangeForTrisWithSameTexture.push_back(t);
			matID = refOBJLoader.m_vMats[refOBJLoader.m_vFaces[x].m_iMatId].m_iGLTextureIndex;
			polyCount = polyCount + polyCounter;
			polyCounter = 1;
		}
		else
		{
			polyCounter++;
		}
	}
	std::tuple<int, int, int> t = std::make_tuple(polyCount * 3, (polyCount + polyCounter - 1) * 3, matID);
	m_vuiFaceIndexRangeForTrisWithSameTexture.push_back(t);
	std::cout << " number of different textures: " << m_vuiFaceIndexRangeForTrisWithSameTexture.size() << std::endl;
}


/*
 *	Method	: ConstructModelFromOBJLoader
 *
 *	Scope	: Private
 *
 *	Purpose	: construct and octree around the 3d object.
 *
 *	Returns	: none
 */
void CThreeDModel::ConstructOctree()
{
	double minX,minY,minZ,maxX,maxY,maxZ;
	CalcBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
	
	m_pobOctree = new COctree();
	m_pobOctree->SetUpRootFrom3DModel(0, minX, minY, minZ, maxX, maxY, maxZ, this);

	std::vector<COctree*> stackOctree;
	stackOctree.push_back(m_pobOctree);

	while (stackOctree.size() > 0)
	{
		COctree* oct = stackOctree[0];
		oct->CreateChildren(stackOctree, this);

		stackOctree.erase(stackOctree.begin());
	}
}

/*
 *	Method	: CalcVertNormalsUsingOctree
 *
 *	Scope	: Private
 *
 *	Purpose	: compute vertex normals for the model using the octree.
 *
 *	Returns	: none
 */
void CThreeDModel::CalcVertNormalsUsingOctree()
{
	if (m_pvVertNormals != nullptr) //delete any previously created vertex normals
	{
		delete[] m_pvVertNormals;
	}

	if (m_pobOctree == nullptr) // construct the octree if it hasn't been created.
	{
		ConstructOctree();
	}

	m_pvVertNormals = new Vector3d[m_iNumberOfVertices];
	m_iNumberOfVertNormals = m_iNumberOfVertices;

	//calculate the vertex normals using the octree
	m_pobOctree->CalcVertexNormals(this);
}

/*
 *	Method	: CalcCentrePoint
 *
 *	Scope	: Public
 *
 *	Purpose	: compute the centre point point of a bounding box containing the 3d model
 *
 *	Returns	: none
 */
void CThreeDModel::CalcCentrePoint()
{
	if (m_pvVertices == nullptr)
		return;

	float maxX, maxY, maxZ;
	float minX, minY, minZ;

	minX = minY = minZ = 100000.0f;
	maxX = maxY = maxZ = -100000.0f;

	for (int count = 0; count < m_iNumberOfVertices; count++)
	{
		if (m_pvVertices[count].x > maxX) maxX = m_pvVertices[count].x;
		if (m_pvVertices[count].y > maxY) maxY = m_pvVertices[count].y;
		if (m_pvVertices[count].z > maxZ) maxZ = m_pvVertices[count].z;

		if (m_pvVertices[count].x < minX) minX = m_pvVertices[count].x;
		if (m_pvVertices[count].y < minY) minY = m_pvVertices[count].y;
		if (m_pvVertices[count].z < minZ) minZ = m_pvVertices[count].z;
	}

	m_obCentrePoint = Vector3d(minX + ((maxX - minX) / 2.0f),
		minY + ((maxY - minY) / 2.0f),
		minZ + ((maxZ - minZ) / 2.0f));
}

/*
 *	Method	: GetCentrePoint
 *
 *	Scope	: Public
 *
 *	Purpose	: returns a pointer to the centre point of the model
 *
 *	Returns	: ptr to vector3d
 */
Vector3d* CThreeDModel::GetCentrePoint()
{
	return &m_obCentrePoint;
}

/*
 *	Method	: GetOctreeTriangleListSize
 *
 *	Scope	: Public
 *
 *	Purpose	: get the number of triangles in the 3d model
 *
 *	Returns	: int
 */
int CThreeDModel::GetOctreeTriangleListSize()
{
	return m_iNumberOfTriangles;
}

/*
 *	Method	: GetOctreeVertexListSize
 *
 *	Scope	: Public
 *
 *	Purpose	: get the number of vertices in the 3d model
 *
 *	Returns	: int
 */
int CThreeDModel::GetOctreeVertexListSize()
{
	return m_iNumberOfVertices;
}

/*
 *	Method	: IsVertexIntersectingAABB
 *
 *	Scope	: Public
 *
 *	Purpose	: To check if the vertex is inside an AABB defined by min and max coordinates.
 *
 *	Returns	: bool - True if the vertex is inside the AABB, false otherwise.
 */
bool CThreeDModel::IsVertexIntersectingAABB(double min[DIMENSION_IN_3D], double max[DIMENSION_IN_3D], int VertIndex)
{
	if (m_pvVertices[VertIndex].x >= min[0] && m_pvVertices[VertIndex].x <= max[0] &&
		m_pvVertices[VertIndex].y >= min[1] && m_pvVertices[VertIndex].y <= max[1] &&
		m_pvVertices[VertIndex].z >= min[2] && m_pvVertices[VertIndex].z <= max[2])
	{
		return true;
	}
	return false;
}

/*
 *	Method	: IsTriangleIntersectingAABB
 *
 *	Scope	: Public
 *
 *	Purpose	: To check if the triangle cuts an AABB defined by its centre coordinate and dimensions from centre to edges of the box.
 *
 *	Returns	: bool - True if the triangle cuts the AABB, false otherwise.
 */
bool CThreeDModel::IsTriangleIntersectingAABB(double boxCenter[DIMENSION_IN_3D], double boxHalfSize[DIMENSION_IN_3D], int PrimIndex)
{
	double triVerts[NUM_OF_VERTS_IN_TRIANGLE][DIMENSION_IN_3D];

	for (int i = 0; i < NUM_OF_VERTS_IN_TRIANGLE; i++)
	{
		triVerts[i][0] = m_pvVertices[m_pobTriangles[PrimIndex].m_uiVertIdx[i]].x;
		triVerts[i][1] = m_pvVertices[m_pobTriangles[PrimIndex].m_uiVertIdx[i]].y;
		triVerts[i][2] = m_pvVertices[m_pobTriangles[PrimIndex].m_uiVertIdx[i]].z;
	}

	return (CIntersectionTests::TriBoxOverlap(boxCenter, boxHalfSize, triVerts) == 1);
}

/*
 *	Method	: CalcBoundingBox
 *
 *	Scope	: Public
 *
 *	Purpose	: To compute the AABB that bounds the 3d model
 *
 *	Returns	: void
 */
void CThreeDModel::CalcBoundingBox(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ)
{
	maxX = minX = m_pvVertices[0].x;
	maxY = minY = m_pvVertices[0].y;
	maxZ = minZ = m_pvVertices[0].z;
	double x,y,z;
	for (int i = 1; i < m_iNumberOfVertices; i++)
	{

		x = m_pvVertices[i].x;
		y = m_pvVertices[i].y;
		z = m_pvVertices[i].z;

		if (x < minX)
			minX = x;
		if (x > maxX)
			maxX = x;
		if (y < minY)
			minY = y;
		if (y > maxY)
			maxY = y;
		if (z < minZ)
			minZ = z;
		if (z > maxZ)
			maxZ = z;
	}
}

/*
 *	Method	: CalcFaceNormals
 *
 *	Scope	: Private
 *
 *	Purpose	: To compute the face normals for each face using the cross product of the edges.
 *
 *	Returns	: void
 */
void CThreeDModel::CalcFaceNormals()
{
	if(m_pvVertices != nullptr)
	{
		for (int i = 0; i < m_iNumberOfTriangles; i++)
		{
			Vector3d edge1 = m_pvVertices[m_pobTriangles[i].m_uiVertIdx[0]] - m_pvVertices[m_pobTriangles[i].m_uiVertIdx[1]];
			Vector3d edge2 = m_pvVertices[m_pobTriangles[i].m_uiVertIdx[0]] - m_pvVertices[m_pobTriangles[i].m_uiVertIdx[2]];

			Vector3d normal = edge1 * edge2;
			normal.normalize();

			m_pvFaceNormals[i] = normal;
		}
	}
}

/*
 *	Method	: CalcVertNormalsForConnectedMeshes
 *
 *	Scope	: Private
 *
 *	Purpose	: To compute the vertex normals for each vertex using the facenormals of the connected triangles
 *
 *	Returns	: void
 */
void CThreeDModel::CalcVertNormalsForConnectedMeshes()
{
	//clear the vertnormals if already created
	if (m_pvVertNormals != nullptr)
	{
		delete[] m_pvVertNormals;
		m_iNumberOfVertNormals = m_iNumberOfVertices;
		m_pvVertNormals = new Vector3d[m_iNumberOfVertNormals];
	}

	//clear all vert normals to zero
	for (int i = 0; i < m_iNumberOfVertices; i++)
	{
		m_pvVertNormals[i] = Vector3d(0, 0, 0);
	}

	//add face normals to all vertices 
	for (int i = 0; i < m_iNumberOfTriangles; i++)
	{
		m_pvVertNormals[m_pobTriangles[i].m_uiVertIdx[0]] += m_pvFaceNormals[i];
		m_pvVertNormals[m_pobTriangles[i].m_uiVertIdx[1]] += m_pvFaceNormals[i];
		m_pvVertNormals[m_pobTriangles[i].m_uiVertIdx[2]] += m_pvFaceNormals[i];
	}

	//normalise the vert normals
	for (int i = 0; i < m_iNumberOfVertices; i++)
	{
		m_pvVertNormals[i].normalize();
	}
}

/*
 *	Method	: CalcVertNormals
 *
 *	Scope	: Public
 *
 *	Purpose	: To compute vertex normals for a subset of the 3d model geometry based on the indices in the lists passed in.
 *				It is used by the Octree class to compute vertex normals for geometry inside an octree node.
 *
 *	Returns	: void
 */
void CThreeDModel::CalcVertNormals(int* VertList, int VertListSize, int* TriList, int TriListSize)
{
	double DISTANCE_TO_VERTEX = 0.005;

	if (m_pvVertices != nullptr)
	{
		for (int i = 0; i < VertListSize; i++) //For all the verts
		{
			Vector3d theNormal(0,0,0);
			for (int j = 0; j < TriListSize; j++)		 //Go through all the triangles in the list of triangles
			{
				for (int k = 0; k < NUM_OF_VERTS_IN_TRIANGLE; k++) //loop through the vertices in each triangle
				{
					if (m_pvVertices[m_pobTriangles[TriList[j]].m_uiVertIdx[k]].euclideanDistance(m_pvVertices[VertList[i]]) < DISTANCE_TO_VERTEX)
					{
						//add to theNormal when the vertex of the current triangle matches the current vertex.
						theNormal = theNormal + m_pvFaceNormals[TriList[j]];
					}
				}
			}
			theNormal.normalize();

			m_pvVertNormals[VertList[i]] = theNormal;
		}
	}
}

/*
 *	Method	: CentreOnZero
 *
 *	Scope	: Public
 *
 *	Purpose	: To subtract the centre point from all the vertices to centre the 3d model 
 *			   on the centre point. The centre point is then set to (0,0,0).
 *
 *	Returns	: void
 */
void CThreeDModel::CentreOnZero()
{
	if (m_pvVertices == nullptr)
		return;

	//Subtract centre point from each point
	for (int count = 0; count < m_iNumberOfVertices; count++)
	{
		m_pvVertices[count] = m_pvVertices[count] - m_obCentrePoint;
	}
	m_obCentrePoint = Vector3d(0,0,0);
}

/*
 *	Method	: InitVBO
 *
 *	Scope	: Public
 *
 *	Purpose	: To create the vbos needed to render the 3d model.
 *
 *	Returns	: void
 */
void CThreeDModel::InitVBO(CShader* myShader)
{
	CalcFaceNormals();

	CalcVertNormalsUsingOctree();
	//CalcVertNormalsUpdate();

	//Create C++ structures 
	GLfloat* vertexPositionList = new GLfloat[(m_iNumberOfTriangles * NUM_OF_VERTS_IN_TRIANGLE * DIMENSION_IN_3D)];
	GLfloat* vertexNormalList = new GLfloat[(m_iNumberOfTriangles * NUM_OF_VERTS_IN_TRIANGLE * DIMENSION_IN_3D)];
	GLfloat* vertexTexCoordList = new GLfloat[(m_iNumberOfTriangles * NUM_OF_VERTS_IN_TRIANGLE * DIMENSION_IN_2D)];
	GLuint* faceIDsList = new GLuint[(m_iNumberOfTriangles * NUM_OF_VERTS_IN_TRIANGLE)];

	//COPY DATA FROM STRUCTURES TO C++ Arrays for us in VBOS
	for (int i = 0; i < m_iNumberOfTriangles * NUM_OF_VERTS_IN_TRIANGLE; i += NUM_OF_VERTS_IN_TRIANGLE)
	{
		faceIDsList[i] = i;
		faceIDsList[i + 1] = (i + 1);
		faceIDsList[i + 2] = (i + 2);
	}

	for (int triIndex = 0, i = 0, j = 0; triIndex < m_iNumberOfTriangles; triIndex++, i += DIMENSION_IN_3D, j += DIMENSION_IN_2D)
	{
		ObjFace temp = m_pobTriangles[triIndex];

		unsigned int vert1 = temp.m_uiVertIdx[0];
		unsigned int vert2 = temp.m_uiVertIdx[1];
		unsigned int vert3 = temp.m_uiVertIdx[2];

		unsigned int norm1 = triIndex;// temp.theFaceNormal;

		unsigned int tex1 = temp.m_uiTexCoordIdx[0];
		unsigned int tex2 = temp.m_uiTexCoordIdx[1];
		unsigned int tex3 = temp.m_uiTexCoordIdx[2];

		vertexPositionList[i] = m_pvVertices[vert1].x;
		vertexPositionList[i + 1] = m_pvVertices[vert1].y;
		vertexPositionList[i + 2] = m_pvVertices[vert1].z;

		if (m_iNumberOfVertNormals > 0)
		{
			vertexNormalList[i] = m_pvVertNormals[vert1].x;
			vertexNormalList[i + 1] = m_pvVertNormals[vert1].y;
			vertexNormalList[i + 2] = m_pvVertNormals[vert1].z;
		}
		else if (m_iNumberOfFaceNormals > 0)
		{
			vertexNormalList[i] = m_pvFaceNormals[norm1].x;
			vertexNormalList[i + 1] = m_pvFaceNormals[norm1].y;
			vertexNormalList[i + 2] = m_pvFaceNormals[norm1].z;
		}
		else
		{
			vertexNormalList[i] = 0;
			vertexNormalList[i + 1] = 0;
			vertexNormalList[i + 2] = 0;
		}

		vertexTexCoordList[j] = m_pvTexCoords[tex1].x;;
		vertexTexCoordList[j + 1] = m_pvTexCoords[tex1].y;

		i += DIMENSION_IN_3D;
		j += DIMENSION_IN_2D;

		vertexPositionList[i] = m_pvVertices[vert2].x;
		vertexPositionList[i + 1] = m_pvVertices[vert2].y;
		vertexPositionList[i + 2] = m_pvVertices[vert2].z;

		if (m_iNumberOfVertNormals > 0)
		{
			vertexNormalList[i] = m_pvVertNormals[vert2].x;
			vertexNormalList[i + 1] = m_pvVertNormals[vert2].y;
			vertexNormalList[i + 2] = m_pvVertNormals[vert2].z;
		}
		else if (m_iNumberOfFaceNormals > 0)
		{
			vertexNormalList[i] = m_pvFaceNormals[norm1].x;
			vertexNormalList[i + 1] = m_pvFaceNormals[norm1].y;
			vertexNormalList[i + 2] = m_pvFaceNormals[norm1].z;
		}
		else
		{
			vertexNormalList[i] = 0;
			vertexNormalList[i + 1] = 0;
			vertexNormalList[i + 2] = 0;
		}

		vertexTexCoordList[j] = m_pvTexCoords[tex2].x;;
		vertexTexCoordList[j + 1] = m_pvTexCoords[tex2].y;

		i += DIMENSION_IN_3D;
		j += DIMENSION_IN_2D;

		vertexPositionList[i] = m_pvVertices[vert3].x;
		vertexPositionList[i + 1] = m_pvVertices[vert3].y;
		vertexPositionList[i + 2] = m_pvVertices[vert3].z;

		if (m_iNumberOfVertNormals > 0)
		{
			vertexNormalList[i] = m_pvVertNormals[vert3].x;
			vertexNormalList[i + 1] = m_pvVertNormals[vert3].y;
			vertexNormalList[i + 2] = m_pvVertNormals[vert3].z;
		}
		else if (m_iNumberOfFaceNormals > 0)
		{
			vertexNormalList[i] = m_pvFaceNormals[norm1].x;
			vertexNormalList[i + 1] = m_pvFaceNormals[norm1].y;
			vertexNormalList[i + 2] = m_pvFaceNormals[norm1].z;
		}
		else
		{
			vertexNormalList[i] = 0;
			vertexNormalList[i + 1] = 0;
			vertexNormalList[i + 2] = 0;
		}

		vertexTexCoordList[j] = m_pvTexCoords[tex3].x;;
		vertexTexCoordList[j + 1] = m_pvTexCoords[tex3].y;
	}
	//END set up of C++ Arrays

	glGenVertexArrays(1, &m_uiVaoID);

	// First VAO setup
	glBindVertexArray(m_uiVaoID);

	size_t numOfMaterials = m_vuiFaceIndexRangeForTrisWithSameTexture.size();// / 3;
	std::cout << " initVBO " << numOfMaterials << std::endl;

	m_uiNumOfVBOs = NUM_OF_VBOS_WITHOUT_TRI_IDS; //verts, normals, texcoords
	m_uiNumOfVBOs += numOfMaterials; //plus the trilist for each of the materials used.

	//create the VBOs
	m_puiVBOs = new GLuint[m_uiNumOfVBOs];
	glGenBuffers(m_uiNumOfVBOs, m_puiVBOs);

	glBindBuffer(GL_ARRAY_BUFFER, m_puiVBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, m_iNumberOfTriangles * NUM_OF_VERTS_IN_TRIANGLE * DIMENSION_IN_3D * sizeof(GLfloat), vertexPositionList, GL_STATIC_DRAW);
	GLint vertexLocation = glGetAttribLocation(myShader->GetProgramObjID(), "in_Position");
	glVertexAttribPointer(vertexLocation, DIMENSION_IN_3D, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLocation);


	glBindBuffer(GL_ARRAY_BUFFER, m_puiVBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, m_iNumberOfTriangles * NUM_OF_VERTS_IN_TRIANGLE * DIMENSION_IN_3D * sizeof(GLfloat), vertexNormalList, GL_STATIC_DRAW);
	GLint normalLocation = glGetAttribLocation(myShader->GetProgramObjID(), "in_Normal");
	glVertexAttribPointer(normalLocation, DIMENSION_IN_3D, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normalLocation);


	glBindBuffer(GL_ARRAY_BUFFER, m_puiVBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, m_iNumberOfTriangles * NUM_OF_VERTS_IN_TRIANGLE * DIMENSION_IN_2D * sizeof(GLfloat), vertexTexCoordList, GL_STATIC_DRAW);
	GLint texCoordLocation = glGetAttribLocation(myShader->GetProgramObjID(), "in_TexCoord");
	glVertexAttribPointer(texCoordLocation, DIMENSION_IN_2D, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texCoordLocation);

	int triIDVBOCounter = 0;
	for (auto i = m_vuiFaceIndexRangeForTrisWithSameTexture.begin(); i != m_vuiFaceIndexRangeForTrisWithSameTexture.end(); ++i)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_puiVBOs[NUM_OF_VBOS_WITHOUT_TRI_IDS + (triIDVBOCounter++)]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ((std::get<1>(*i) - std::get<0>(*i) + DIMENSION_IN_3D) * sizeof(GLuint)), faceIDsList + std::get<0>(*i), GL_STATIC_DRAW);
	}

	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	//delete C++ arrays for VBOs once the data is copied to GPU.
	delete[] vertexPositionList;
	delete[] vertexNormalList;
	delete[] vertexTexCoordList;
	delete[] faceIDsList;
}

/*
 *	Method	: DrawElementsUsingVBO
 *
 *	Scope	: Public
 *
 *	Purpose	: To render the object
 *
 *	Returns	: void
 */
void CThreeDModel::DrawElementsUsingVBO(CShader * myShader)
{
	glBindVertexArray(m_uiVaoID);

	glUniform1i(glGetUniformLocation(myShader->GetProgramObjID(), "DiffuseMap"), 0);

	int triIDVBOCounter = 0;
	for (auto i = m_vuiFaceIndexRangeForTrisWithSameTexture.begin(); i != m_vuiFaceIndexRangeForTrisWithSameTexture.end(); ++i)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, std::get<2>(*i));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_puiVBOs[NUM_OF_VBOS_WITHOUT_TRI_IDS + (triIDVBOCounter++)]);

		glDrawElements(GL_TRIANGLES, (std::get<1>(*i) - std::get<0>(*i) + DIMENSION_IN_3D), GL_UNSIGNED_INT, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);
}

/*
 *	Method	: DrawBoundingBox
 *
 *	Scope	: Public
 *
 *	Purpose	: To render the bounding box around the object
 *
 *	Returns	: void
 */
void CThreeDModel::DrawBoundingBox(CShader* shader)
{
	if (m_pobOctree != nullptr)
	{
		m_pobOctree->DrawBoundingBox(shader);
	}
}

/*
 *	Method	: DrawAllBoxesForOctreeNodes
 *
 *	Scope	: Public
 *
 *	Purpose	: To render the bounding box around each node in the octree
 *
 *	Returns	: void
 */
void CThreeDModel::DrawAllBoxesForOctreeNodes(CShader* shader)
{
	if (m_pobOctree != nullptr)
	{
		m_pobOctree->DrawAllBoxes(shader);
	}
}

/*
 *	Method	: DrawOctreeLeaves
 *
 *	Scope	: Public
 *
 *	Purpose	: To render the bounding boxes for all the leaves in the octree
 *
 *	Returns	: void
 */
void CThreeDModel::DrawOctreeLeaves(CShader* shader)
{
	if (m_pobOctree != nullptr)
	{
		m_pobOctree->DrawOctreeLeaves(shader);
	}
}
/*
 *	Method	: DeleteVertexFaceData
 *
 *	Scope	: Public
 *
 *	Purpose	: To delete the geometry from the structure, mostly needed to free up 
 *             memory if it is not used after the VBOs are set up.
 *
 *	Returns	: void
 */
void CThreeDModel::DeleteVertexFaceData()
{
	delete[] m_pvVertices;
	delete[] m_pvFaceNormals;
	delete[] m_pobTriangles;
	delete[] m_pvTexCoords;
	delete[] m_pvVertNormals;

	m_pvVertices = nullptr;
	m_pvFaceNormals = nullptr;
	m_pvTexCoords = nullptr;
	m_pobTriangles = nullptr;
	m_pvVertNormals = nullptr;
}