//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "math.h"
#include "CPoint3D.h"

POINT3D::POINT3D(): x(0) , y(0) , z(0) , w(1)
{
   
}

POINT3D::~POINT3D()
{

}

bool POINT3D::operator < (const POINT3D &point3d)
{
	return this->z > point3d.z;
}

POINT3D POINT3D::operator + (const POINT3D &point3d)
{
   POINT3D p;
   p.x = x + point3d.x;
   p.y = y + point3d.y;
   p.z = z + point3d.z;
   return p;
}

POINT3D POINT3D::operator - (const POINT3D &point3d)
{
   POINT3D p;
   p.x = x - point3d.x;
   p.y = y - point3d.y;
   p.z = z - point3d.z;
   return p;
}

POINT3D POINT3D::operator / (const int &sum)
{
	if (sum <= 0)
	   return (*this);
	POINT3D p;
	p.x = x / sum;
	p.y = y / sum;
	p.z = z / sum;
	return p;
}

POINT3D POINT3D::operator * (const int &sum)
{
	POINT3D p;
	p.x = x * sum;
	p.y = y * sum;
	p.z = z * sum;
	return p;
}

POINT3D POINT3D::operator += (const POINT3D &point3d)
{
   x = x + point3d.x;
   y = y + point3d.y;
   z = z + point3d.z;
   return (*this);
}

POINT3D POINT3D::operator -= (const POINT3D &point3d)
{
   x = x - point3d.x;
   y = y - point3d.y;
   z = z - point3d.z;
   return (*this);
}

POINT3D POINT3D::operator /= (const int &sum)
{
	if (sum <= 0)
	   return (*this);
	x = x / sum;
	y = y / sum;
	z = z / sum;
    return (*this);
}

POINT3D POINT3D::operator *= (const int &sum)
{
	x = x * sum;
	y = y * sum;
	z = z * sum;
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
POINT7D::POINT7D() : nx(0.0) , ny(0.0) , nz(0.0)
{

}

POINT7D::~POINT7D()
{

}



//////////////////////////////////////////////////////////////////////////
double Distance(const POINT3D &p1 , const POINT3D &p2)
{
	return (double)sqrt((p1.x - p2.x) * (p1.x - p2.x) +(p1.y - p2.y) * (p1.y - p2.y) +(p1.z - p2.z) * (p1.z - p2.z));
}

POINT7D POINT7D::operator =(const POINT3D &point3d)
{
    x = point3d.x;
	y = point3d.y;
	z = point3d.z;
	w = point3d.w;
	return (*this);
}
