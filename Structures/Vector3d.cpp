/*******************************************************
				Implementation of Vector3d File
*******************************************************/
#include "vector3d.h"

/*******************************************************
				Vector 3d
*******************************************************/

Vector3d::Vector3d () 
{ 
	 x = 0.0f; y = 0.0f; z = 0.0f;
}

Vector3d::Vector3d (float newX, float newY, float newZ)
{
	x = newX; y = newY; z = newZ;
}

Vector3d::Vector3d (float *input)
{	
	x = input[0];
	y = input[1];
	z = input[2];
}

/*******************************************************
				Opperators
*******************************************************/

Vector3d Vector3d::operator * (const Vector3d &vector2) //Define Cross Product
{
	Vector3d results((y * vector2.z)-(z * vector2.y),
				(z * vector2.x)-(x * vector2.z),
				(x * vector2.y)-(y * vector2.x));
	return results;
}

Vector3d Vector3d::operator*(float scale)
{
	Vector3d results(x*scale, y*scale, z*scale);
	return results;
}

Vector3d Vector3d::operator/(float scale)
{
	Vector3d results(x/scale, y/scale, z/scale);
	return results;
}


Vector3d Vector3d::operator - (const Vector3d &vector2)
{
	Vector3d results(x - vector2.x, y - vector2.y, z - vector2.z);
	return results;
}

Vector3d& Vector3d::operator +=(const Vector3d& vector2)
{
	x += vector2.x;
	y += vector2.y;
	z += vector2.z;
	return *this;
}

Vector3d Vector3d::operator + (const Vector3d &vector2)
{
	Vector3d results(x + vector2.x, y + vector2.y,  z + vector2.z);
	return results;
}

bool Vector3d::operator != (const Vector3d &vector2)
{
	if ((x != vector2.x) || (y != vector2.y) || (z != vector2.z)) return true;
	else return false;
}
bool Vector3d::operator == (const Vector3d &vector2)
{
	if( (x == vector2.x) && (y == vector2.y) && (z == vector2.z) )
	{
		//cout << "returning true from Vector class" << endl;
		return true;
	}
	else
	{
		//cout << "returning false from Vector class" << endl;
		return false;
	}
}

float Vector3d::operator[](unsigned int index)
{
	switch (index)
	{
	case 0: {
				return x;
				break;
			}
		case 1: 
			{
				return y;
				break;
			}
		case 2: 
			{
				return z;
				break;
			}
	}
	
	return 0.0f;
}

Vector3d Vector3d::operator-()
{
	Vector3d results(-x, -y, -z);
	return results;
}

/*******************************************************
				Methods
*******************************************************/

float Vector3d::euclideanDistance(Vector3d &toDo)
{

		return sqrt(
					 ((toDo.x - this->x) * (toDo.x - this->x) ) + 
				     ((toDo.y - this->y) * (toDo.y - this->y) ) + 
				     ((toDo.z - this->z) * (toDo.z - this->z) ) 
				   );

}
float Vector3d::squaredEuclideanDistance(Vector3d &toDo)
{

		return		(
					 ((toDo.x - this->x) * (toDo.x - this->x) ) + 
				     ((toDo.y - this->y) * (toDo.y - this->y) ) + 
				     ((toDo.z - this->z) * (toDo.z - this->z) ) 
				   );

}
float Vector3d::squaredEuclideanDistanceSCALE(Vector3d &toDo)
{

		return		(
					 ((toDo.x - this->x) * (toDo.x - this->x) ) + 
				     ((toDo.y - this->y) * (toDo.y - this->y) ) + 
				     ((toDo.z - this->z) * (toDo.z - this->z) ) 
				   );

}
// Reflect vector a in the normal
Vector3d Vector3d::reflectInNormal(const Vector3d & d, const Vector3d & normal)
{
	//0 - ((dotProduct(normal,d)*2)*normal - d);   
   float tempF = dotProduct(normal,d) * 2;
   Vector3d temp(normal.x * tempF, normal.y * tempF, normal.z * tempF);
   temp.x = temp.x - d.x;
   temp.y = temp.y - d.y;
   temp.z = temp.z - d.z;
  
   temp.x = 0 - temp.x;
   temp.y = 0 - temp.y;
   temp.z = 0 - temp.z;

   return temp;
}


bool Vector3d::isEqual (Vector3d &vector1, Vector3d &vector2, float threshold)
{
	if ((fabs(vector1.x-vector2.x) < threshold) && (fabs(vector1.y-vector2.y) < threshold)  && (fabs(vector1.z-vector2.z) < threshold))
	{
		return true;
	}
	else 
	{
		return false;
	}
}
bool Vector3d::testEquality(Vector3d &toBeTested1, Vector3d &toBeTested2)
{
	if( (toBeTested1.x == toBeTested2.x) && (toBeTested1.y == toBeTested2.y) && (toBeTested1.z == toBeTested2.z) )
		return true;
	else
		return false;
}

void Vector3d::normalize()
{
	float factor = sqrt(x*x+y*y+z*z);
	if (factor != 0)
	{
		x /= factor; y/= factor; z /=factor;
	}
}

Vector3d Vector3d::normalize(const Vector3d &toDo)
{
	Vector3d result = toDo;
	float factor = sqrt(result.x*toDo.x+result.y*result.y+result.z*result.z);
	if (factor != 0)
	{
		result.x /= factor; result.y/= factor; result.z /=factor;
	}
	return result;
}

inline float Vector3d::size()
{
	return (float) sqrt((x*x)+(y*y)+(z*z));
}

float Vector3d::sizeSqr()
{
	return (float) (x*x)+(y*y)+(z*z);
}

float Vector3d::angleBetweenVectors(Vector3d &first,Vector3d &second)
{
	float angle;

	float topPart = dotProduct(first,second);
	float bottom = first.size() * second.size();

	angle = (float) acos(topPart/bottom);

	return angle;
}

float Vector3d::signedAngleBetweenVectors(Vector3d &first,Vector3d &second)
{
	float angle;

	float topPart = dotProduct(first,second);
	float bottom = first.size() * second.size();

	angle = (float) acos(topPart/bottom);

	Vector3d cross = first * second;
	float sign = cross.x + cross.y + cross.z;
	int sgn = ( sign > 0 ? 1 : -1);  

	return angle * sgn;
}

float Vector3d::dotProduct(const Vector3d &first, const Vector3d &second)
{
	return (first.x * second.x) + (first.y * second.y) +(first.z * second.z);
}

Vector3d Vector3d::crossProduct(const Vector3d &first, const Vector3d &second)
{

	return Vector3d(
            first.y * second.z - first.z * second.y,
            first.z * second.x - first.x * second.z,
            first.x * second.y - first.y * second.x
            );

}

float Vector3d::angleBetweenVectors2d(const Vector3d &first, const Vector3d &second, char axis)
{	
	Vector3d firstCopy = first;
	Vector3d secondCopy = second;
	
	switch (axis)
	{
	case 'x':
		{
			firstCopy.x = secondCopy.x = 0; break;
		}
	case 'y':
		{
			firstCopy.y = secondCopy.y = 0; break;
		}
	case 'z':
		{
			firstCopy.z = secondCopy.z = 0; break;
		}

	}
	
	float angle;

	float topPart = (firstCopy.x * secondCopy.x) + (firstCopy.y * secondCopy.y) +(firstCopy.z * secondCopy.z);
	float bottom = firstCopy.size() * secondCopy.size();

	angle = (float) acos(topPart/bottom);

	return angle;
}

void Vector3d::multiplyMatrix(float *toDo)
{
	Vector3d result;

	result.x = x*toDo[0]  + y*toDo[1] +z*toDo[2] +1*toDo[3];
	result.y = x*toDo[4]  + y*toDo[5] +z*toDo[6] +1*toDo[7];
	result.z = x*toDo[8]  + y*toDo[9] +z*toDo[10] +1*toDo[11];

	x = result.x;
	y = result.y;
	z = result.z;
}

// calculate the projection vector of vector U onto vector V
void Vector3d::CalcProjectionOfVecUOnVecV(Vector3d* pobVecU, Vector3d* pobVecV)
{
	float	fVecVSizeSquared = 0.0f;
	float	fUVCrossProduct = 0.0f;
	float	fProjScalar = 0.0f;

	x = y = z = 0.0f;	
	
	if (pobVecU == NULL || pobVecV == NULL)
		return;

	fVecVSizeSquared = pobVecV->sizeSqr();
	fUVCrossProduct = ((pobVecU->x*pobVecV->x)+(pobVecU->y*pobVecV->y)+(pobVecU->z*pobVecV->z));
	fProjScalar = fUVCrossProduct/fVecVSizeSquared;

	x = pobVecV->x * fProjScalar;
	y = pobVecV->y * fProjScalar;
	z = pobVecV->z * fProjScalar;
}
