// Re_Vector.cpp: implementation of the Re_Vector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Re_Vector.h"

#include <math.h>
#define  mySUCCED 1
#define  myFAILED 0
#define  PIE  3.1415926

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



Re_Vector::~Re_Vector()
{

}

double Re_Vector::Angle(Re_Vector v)
{
    double ang;
    double len = sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
    if(len<0.00000001)return PIE;
    v.Normal();
    ang = (*this)*v/len;
	if(ang>0.99999999)ang=0.999999999;
	if(ang<-0.999999999)ang=-0.999999999;
    ang = acos(ang);
    return ang;
}

void Re_Vector::SetValue(double a,double b,double c)
{
    m_x = a;
    m_y = b;
    m_z = c;
}

int Re_Vector::IsNull()//是否为零
{
	if (m_x==0.0 && m_y==0.0 && m_z==0.0)return 1;
	else return 0;
}

int Re_Vector::Normal()//单位化
{
   
   double r;
   r=sqrt(m_x*m_x+m_y*m_y+m_z*m_z);

   if (fabs(r)<=0.00000001) {
//		MessageBox(NULL,"Vector is (0,0,0) in AG_VectorNormal","ERROR MESSAGE",MB_OK||MB_ICONHAND);
		return myFAILED;
	};

   m_x=m_x/r;
   m_y=m_y/r;
   m_z=m_z/r;
   return mySUCCED;
}

Re_Vector Re_Vector::Add(Re_Vector vec1)
{
   Re_Vector v;
   v.m_x=m_x+vec1.m_x;
   v.m_y=m_y+vec1.m_y;
   v.m_z=m_z+vec1.m_z;
   return (v);
}
//Re_MsrPnt3D &Re_MsrPnt3D:: operator =(Re_MsrPnt3D a)
Re_Vector &Re_Vector::operator=(Re_Vector vec1)
{
    m_x = vec1.m_x;
    m_y = vec1.m_y;
    m_z = vec1.m_z;
    return(*this);
}


Re_Vector Re_Vector::operator+(Re_Vector vec1)
{
    Re_Vector v;
   v.m_x=m_x+vec1.m_x;
   v.m_y=m_y+vec1.m_y;
   v.m_z=m_z+vec1.m_z;
   return (v);
}
Re_Vector Re_Vector::operator-(Re_Vector vec1)
{
    Re_Vector v;
    v.m_x=m_x-vec1.m_x;
    v.m_y=m_y-vec1.m_y;
    v.m_z=m_z-vec1.m_z;
    return(v);
}
Re_Vector Re_Vector::Sub(Re_Vector vec1)
{
    Re_Vector v;
    v.m_x=m_x-vec1.m_x;
    v.m_y=m_y-vec1.m_y;
    v.m_z=m_z-vec1.m_z;
    return(v);
}

Re_Vector Re_Vector::Mul(double len)
{
    Re_Vector v;
    v.m_x=m_x*len;
    v.m_y=m_y*len;
    v.m_z=m_z*len;
    return(v);
}
Re_Vector Re_Vector::operator*(double len)
{
    Re_Vector v;
    v.m_x=m_x*len;
    v.m_y=m_y*len;
    v.m_z=m_z*len;
    return(v);
}

double Re_Vector::DotMul(Re_Vector v)
{
    return(m_x*v.m_x+m_y*v.m_y+m_z*v.m_z);
}
double Re_Vector::operator* (Re_Vector v)
{
    return(m_x*v.m_x+m_y*v.m_y+m_z*v.m_z);
}

Re_Vector Re_Vector::Cross(Re_Vector v)
{
    double a1,b1,c1;
	double a2,b2,c2;
    Re_Vector mm;
	a1=m_x,b1=m_y,c1=m_z;
	a2=v.m_x,b2=v.m_y,c2=v.m_z;
	mm.m_x= b1*c2-c1*b2;
	mm.m_y= c1*a2-a1*c2;
	mm.m_z= a1*b2-b1*a2;
    return(mm);
}
Re_Vector Re_Vector::operator^(Re_Vector v)
{
    double a1,b1,c1;
	double a2,b2,c2;
    Re_Vector mm;
	a1=m_x,b1=m_y,c1=m_z;
	a2=v.m_x,b2=v.m_y,c2=v.m_z;
	mm.m_x= b1*c2-c1*b2;
	mm.m_y= c1*a2-a1*c2;
	mm.m_z= a1*b2-b1*a2;
    return(mm);
}
Re_Vector::Re_Vector()
{
    m_x=0.0;
    m_y=0.0;
    m_z=0.0;
}
Re_Vector::Re_Vector(double x,double y,double z)
{
    m_x=x;
    m_y=y;
    m_z=z;
}
Re_Vector::Re_Vector(const Re_Vector& vec1)
{
    m_x = vec1.m_x;
    m_y = vec1.m_y;
    m_z = vec1.m_z;
    //return(*this);
}

void Re_Vector::RotateAng(double& angX,double& angY,double& angZ)	 
{
	Normal();
	if(m_x >= 0.9999999)
	{
		angX=0.0;
		angY=-PIE/2.0;
		angZ=0.0;
	}
	else if(m_x <= -0.9999999)
	{
		angX=0.0;
		angY=PIE/2.0;
		angZ=0.0;
	}
	else
	{
		Re_Vector v,vv,axis,vec;
		vec.m_x=0.0;
		vec.m_y=1.0;
		vec.m_z=0.0;
		axis.m_x=0.0;
		axis.m_y=0.0;
		if(m_z==0.0) m_z=1.0e-7;
		axis.m_z=m_z;
		v.m_x=0.0;
		v.m_y=m_y;
		v.m_z=m_z;

		angX=v.Angle(axis);
		if(m_z<0.0)
		{
			if(v*vec>0.0)
				angX=-angX;
		}
		else
		{
			if(v*vec<.0)
				angX=-angX;
		}
		vec.m_x=1.0;
		vec.m_y=0.0;
		vec.m_z=0.0;
		v.m_x=m_x;
		v.m_y=m_y;
		v.m_z=m_z;
		vv.m_x=0.0;
		vv.m_y=m_y;
		vv.m_z=m_z;

		angY=v.Angle(vv);
		if(v*vec>0.0)
			angY=-angY;
		if(m_z<0.0)
		{
			if(angY>0.0)
				angY =PIE-angY;
			else
				angY =-PIE-angY;
		}
		angZ=0.0;
	}
} 


double Re_Vector::Length()
{
	double x;
	x = m_x*m_x;
	x=x+m_y*m_y;
	x=x+m_z*m_z;
	x = sqrt(x);
	return(x); 
}
double Re_Vector::Len()	
{
	return sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
}

void Re_Vector::RotateX(double ang)
{
	double x,y,z;
	x=m_x;y=m_y;z=m_z;
	m_y=y*cos(ang)-z*sin(ang);
	m_z=y*sin(ang)+z*cos(ang);
}
void Re_Vector::RotateY(double ang)
{
	double x,y,z;
	x=m_x;y=m_y;z=m_z;
	m_x=x*cos(ang)+z*sin(ang);
	m_z=(-x)*sin(ang)+z*cos(ang);
}
void Re_Vector::RotateZ(double ang)
{
	double x,y,z;
	x=m_x;y=m_y;z=m_z;
	m_x=x*cos(ang)-y*sin(ang);
	m_y=x*sin(ang)+y*cos(ang);
}