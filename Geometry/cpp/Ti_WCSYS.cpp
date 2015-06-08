// Ti_WCSYS.cpp: implementation of the Ti_WCSYS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ti_WCSYS.h"
#include "math.h"
//#include "Ti_Point.h"

static double vp[4];
static double md[16];
static double pj[16];

#define LITTLEPOSITIVE 0.0001

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Ti_WCSYS::Ti_WCSYS()
{
	m_org[0]=0;
	m_org[1]=0;
	m_org[2]=0;

	m_x[0]=1;
	m_x[1]=0;
	m_x[2]=0;

	m_y[0]=0;
	m_y[1]=1;
	m_y[2]=0;

	m_z[0]=0;
	m_z[1]=0;
	m_z[2]=1;

	m_scale = 1;
}

Ti_WCSYS::Ti_WCSYS(const Ti_WCSYS& a)
{
	m_org[0]=a.m_org[0];
	m_org[1]=a.m_org[1];
	m_org[2]=a.m_org[2];

	m_x[0]=a.m_x[0];
	m_x[1]=a.m_x[1];
	m_x[2]=a.m_x[2];

	m_y[0]=a.m_y[0];
	m_y[1]=a.m_y[1];
	m_y[2]=a.m_y[2];

	m_z[0]=a.m_z[0];
	m_z[1]=a.m_z[1];
	m_z[2]=a.m_z[2];

	m_scale = a.m_scale;
}
Ti_WCSYS::~Ti_WCSYS()
{
	m_org[0]=0;
	m_org[1]=0;
	m_org[2]=0;

	m_x[0]=1;
	m_x[1]=0;
	m_x[2]=0;

	m_y[0]=0;
	m_y[1]=1;
	m_y[2]=0;

	m_z[0]=0;
	m_z[1]=0;
	m_z[2]=1;
}

//x0 y0 z0 the origin coordinates relative to the absolute sys
//x1 y1 z1 is the x-axis relative to the x-axis of the absolute sys
//x2 y2 z2 is the y-axis relative to the y-axis of the absolute sys
bool Ti_WCSYS::SetFromOrgXY(double x0,double y0,double z0,double x1,double y1,double z1,double x2,double y2,double z2)
{
	double len;
	double x3,y3,z3;

	x1=x1-x0;
	y1=y1-y0;
	z1=z1-z0;
	len=sqrt(fabs(x1*x1+y1*y1+z1*z1));
	if(len<LITTLEPOSITIVE)
	{
		return false;
	}
	x1=x1/len;
	y1=y1/len;
	z1=z1/len;
	x2=x2-x0;
	y2=y2-y0;
	z2=z2-z0;
	len=sqrt(fabs(x2*x2+y2*y2+z2*z2));
	if(len<LITTLEPOSITIVE)
	{
		return  false;
	}
	x2=x2/len;
	y2=y2/len;
	z2=z2/len;
	////计算z轴方向
	x3=y1*z2-y2*z1;
	y3=x2*z1-x1*z2;
	z3=x1*y2-y1*x2;
	len=sqrt(fabs(x3*x3+y3*y3+z3*z3));
	if(len<LITTLEPOSITIVE)
	{
		return  false;
	}
	x3=x3/len;
	y3=y3/len;
	z3=z3/len;
	/////重新计算Y轴
	x2=y3*z1-z3*y1;
	y2=z3*x1-x3*z1;
	z2=x3*y1-y3*x1;
	len=sqrt(fabs(x2*x2+y2*y2+z2*z2));
	if(len<LITTLEPOSITIVE)
	{
		return false;
	}
	x2=x2/len;
	y2=y2/len;
	z2=z2/len;
	m_org[0]=x0;
	m_org[1]=y0;
	m_org[2]=z0;

	m_x[0]=x1;
	m_x[1]=y1;
	m_x[2]=z1;

	m_y[0]=x2;
	m_y[1]=y2;
	m_y[2]=z2;

	m_z[0]=x3;
	m_z[1]=y3;
	m_z[2]=z3;

	return true;
}

bool Ti_WCSYS::Init()
{
	this->m_org[0]=0.0;
	this->m_org[1]=0.0;
	this->m_org[2]=0.0;

	this->m_x[0]=1.0;
	this->m_x[1]=0.0;
	this->m_x[2]=0.0;

	this->m_y[0]=0.0;
	this->m_y[1]=1.0;
	this->m_y[2]=0.0;

	this->m_z[0]=0.0;
	this->m_z[1]=0.0;
	this->m_z[2]=1.0;
	return true;
}

Ti_WCSYS& Ti_WCSYS::operator =(const Ti_WCSYS& ucs)
{
	this->m_org[0]=ucs.m_org[0];
	this->m_org[1]=ucs.m_org[1];
	this->m_org[2]=ucs.m_org[2];
	this->m_x[0]=ucs.m_x[0];
	this->m_x[1]=ucs.m_x[1];
	this->m_x[2]=ucs.m_x[2];

	this->m_y[0]=ucs.m_y[0];
	this->m_y[1]=ucs.m_y[1];
	this->m_y[2]=ucs.m_y[2];

	this->m_z[0]=ucs.m_z[0];
	this->m_z[1]=ucs.m_z[1];
	this->m_z[2]=ucs.m_z[2];

	this->m_scale = ucs.m_scale;
	return (*this);
}

//****************************************************************************
//  Function name: ToAbsolute
//  Description:   transfer a point from wcs to absolute cs
//  Iutput Params:  pnt: a point in wcs
//                   
//  Output Params: x, y, z: coordinate in absolute cs
//  Returns: true for success, false for fail
//****************************************************************************
bool Ti_WCSYS::ToAbsoluteP(PNT& pnt, double& x, double& y, double& z)
{
//	if(!pnt)return false;
	x=m_org[0] + pnt[0]*m_x[0] + pnt[1]*m_y[0] + pnt[2]*m_z[0];
	y=m_org[1] + pnt[0]*m_x[1] + pnt[1]*m_y[1] + pnt[2]*m_z[1];
	z=m_org[2] + pnt[0]*m_x[2] + pnt[1]*m_y[2] + pnt[2]*m_z[2];	
	return true;
}

//****************************************************************************
//  Function name: ToAbsolute
//  Description:   transfer a point from wcs to absolute cs
//  Iutput Params:  pnt: a point in wcs
//                   
//  Output Params: x, y, z: coordinate in absolute cs
//  Returns: true for success, false for fail
//****************************************************************************
bool Ti_WCSYS::ToAbsolutePTP(PNT& pnt, PNT& outpnt)
{
	//if(!pnt)return false;
	outpnt[0] =m_org[0] + pnt[0]*m_x[0] + pnt[1]*m_y[0] + pnt[2]*m_z[0];
	outpnt[1] =m_org[1] + pnt[0]*m_x[1] + pnt[1]*m_y[1] + pnt[2]*m_z[1];
	outpnt[2] =m_org[2] + pnt[0]*m_x[2] + pnt[1]*m_y[2] + pnt[2]*m_z[2];	
	return true;
}

//****************************************************************************
//  Function name: ToAbsolute
//  Description:   transfer a point from wcs to absolute cs
//  Iutput Params:  pnt: a point in wcs
//                   
//  Output Params: pnt: coordinate in absolute cs
//  Returns: true for success, false for fail
//****************************************************************************
bool Ti_WCSYS::ToAbsolutePP(PNT& pnt)
{
	double x, y, z;

	//if(!pnt)return false;

	x=m_org[0] + pnt[0]*m_x[0] + pnt[1]*m_y[0] + pnt[2]*m_z[0];
	y=m_org[1] + pnt[0]*m_x[1] + pnt[1]*m_y[1] + pnt[2]*m_z[1];
	z=m_org[2] + pnt[0]*m_x[2] + pnt[1]*m_y[2] + pnt[2]*m_z[2];	

	pnt[0] = x;
	pnt[1] = y;
	pnt[2] = z;

	return true;
}

//****************************************************************************
//  Function name: SetFromOrgZX
//  Description:   create a wcs from origin point and z axis and x axis
//  Iutput Params:  x0: x coordinate of origin point
//                  y0: y coordinate of origin point
//                  z0: z coordiante of origin point
//                  x3, y3, z3: z axis 
//                  x1, y1, z1: x axis
//  Output Params: No
//  Returns: true for success, false for fail
//****************************************************************************
bool Ti_WCSYS::SetFromOrgZX(double x0, double y0, double z0, 
						   double x3, double y3, double z3, 
						   double x1, double y1, double z1)
{
	double len;
	double x2,y2,z2;

	x1=x1-x0;
	y1=y1-y0;
	z1=z1-z0;
	len=sqrt(fabs(x1*x1+y1*y1+z1*z1));
	if(len<LITTLEPOSITIVE)
	{
		return false;
	}
	x1=x1/len;
	y1=y1/len;
	z1=z1/len;
	x3=x3-x0;
	y3=y3-y0;
	z3=z3-z0;
	len=sqrt(fabs(x3*x3+y3*y3+z3*z3));
	if(len<LITTLEPOSITIVE)
	{
		return  false;
	}
	x3=x3/len;
	y3=y3/len;
	z3=z3/len;
	////计算y轴方向
	x2=y3*z1-y1*z3;
	y2=x1*z3-x3*z1;
	z2=x3*y1-y3*x1;
	len=sqrt(fabs(x1*x1+y1*y1+z1*z1));
	if(len<LITTLEPOSITIVE)
	{
		return  false;
	}
	x2=x2/len;
	y2=y2/len;
	z2=z2/len;
	/////重新计算X轴
	x1=y2*z3-z2*y3;
	y1=z2*x3-x2*z3;
	z1=x2*y3-y2*x3;
	len=sqrt(fabs(x1*x1+y1*y1+z1*z1));
	if(len<LITTLEPOSITIVE)
	{
		return false;
	}
	x1=x1/len;
	y1=y1/len;
	z1=z1/len;
	m_org[0]=x0;
	m_org[1]=y0;
	m_org[2]=z0;

	m_x[0]=x1;
	m_x[1]=y1;
	m_x[2]=z1;

	m_y[0]=x2;
	m_y[1]=y2;
	m_y[2]=z2;

	m_z[0]=x3;
	m_z[1]=y3;
	m_z[2]=z3;

	return true;	
}
//****************************************************************************
//  Function name: FromAbs
//  Description:   translate a point from absolute to local system 
//  Input Params:  x,y z
//	x: x coordinate in absolute
//  y: y coordinate in absolute
//  z: z coordinate in absolute
//  Output Param:  x0, y0, z0
//  x0: x coordinate in local system
//  y0: y coordinate in local system
//  z0: z coordinate in local system
//  Returns: no
//  Calls/References: open GL fuctions, Line, arc, and circle create function
//****************************************************************************
void Ti_WCSYS::FromAbs(double x, double y, double z, double &x0, double &y0, double &z0)
{
	double ax,ay,az;
	ax = x - m_org[0];
	ay = y - m_org[1];
	az = z - m_org[2];

	x0 = ax*m_x[0] + ay*m_x[1] + az*m_x[2];
	y0 = ax*m_y[0] + ay*m_y[1] + az*m_y[2];
	z0 = ax*m_z[0] + ay*m_z[1] + az*m_z[2];
}

void Ti_WCSYS::FromAbs3(double &x0, double &y0, double &z0)
{
	double ax,ay,az;
	ax = x0 - m_org[0];
	ay = y0 - m_org[1];
	az = z0 - m_org[2];

	x0 = ax*m_x[0] + ay*m_x[1] + az*m_x[2];
	y0 = ax*m_y[0] + ay*m_y[1] + az*m_y[2];
	z0 = ax*m_z[0] + ay*m_z[1] + az*m_z[2];
}

void Ti_WCSYS::FromAbsPP(PNT& pnt)
{
	double ax,ay,az,x0,y0,z0;
	ax = pnt[0] - m_org[0];
	ay = pnt[1] - m_org[1];
	az = pnt[2] - m_org[2];

	x0 = ax*m_x[0] + ay*m_x[1] + az*m_x[2];
	y0 = ax*m_y[0] + ay*m_y[1] + az*m_y[2];
	z0 = ax*m_z[0] + ay*m_z[1] + az*m_z[2];

	pnt[0] = x0;
	pnt[1] = y0;
	pnt[2] = z0;
}

//****************************************************************************
//  Function name: Rotate
//  Description:   rotate the wcs to given angle about a axis
//  Iutput Params:  iaxis: 1-x axis
//                         2-y axis
//                         3-z axis
//                  angle: the rotate angle
//                  
//  Output Params: No
//  Returns: no
//****************************************************************************
void Ti_WCSYS::Rotate(int iaxis, double angle)
{
	///角度转弧度
	angle=angle*3.1415926/180.0;
	double x[3],y[3],z[3];
	x[0]=1.0;x[1]=0.0;x[2]=0.0;
	y[0]=0.0;y[1]=1.0;y[2]=0.0;
	z[0]=0.0;z[1]=0.0;z[2]=1.0;
	double xp[3],yp[3],zp[3];

	if(iaxis==1)
	{
		yp[1]=y[1]*cos(angle)-y[2]*sin(angle);	
		yp[2]=y[1]*sin(angle)+y[2]*cos(angle);
		yp[0]=0.0;
		zp[1]=z[1]*cos(angle)-z[2]*sin(angle);	
		zp[2]=z[1]*sin(angle)+z[2]*cos(angle);
		zp[0]=0.0;
		////到绝对坐标系后
		ToAbsolute(yp[0],yp[1],yp[2]);
		ToAbsolute(zp[0],zp[1],zp[2]);

		this->m_y[0]=yp[0]-m_org[0];
		this->m_y[1]=yp[1]-m_org[1];
		this->m_y[2]=yp[2]-m_org[2];

		this->m_z[0]=zp[0]-m_org[0];
		this->m_z[1]=zp[1]-m_org[1];
		this->m_z[2]=zp[2]-m_org[2];
	}
	else if(iaxis==2)
	{
		zp[2]=z[2]*cos(angle)-z[0]*sin(angle);	
		zp[0]=z[2]*sin(angle)+z[0]*cos(angle);
		zp[1]=0.0;
		xp[2]=x[2]*cos(angle)-x[0]*sin(angle);	
		xp[0]=x[2]*sin(angle)+x[0]*cos(angle);
		xp[1]=0.0;
		////到绝对坐标系后
		ToAbsolute(zp[0],zp[1],zp[2]);
		ToAbsolute(xp[0],xp[1],xp[2]);

		this->m_x[0]=xp[0]-m_org[0];
		this->m_x[1]=xp[1]-m_org[1];
		this->m_x[2]=xp[2]-m_org[2];

		this->m_z[0]=zp[0]-m_org[0];
		this->m_z[1]=zp[1]-m_org[1];
		this->m_z[2]=zp[2]-m_org[2];
	}
	else if(iaxis==3)
	{
		////在局部坐标系中绕z轴旋转，得xp,yp
		xp[0]=x[0]*cos(angle)-x[1]*sin(angle);	
		xp[1]=x[0]*sin(angle)+x[1]*cos(angle);
		xp[2]=0.0;
		yp[0]=y[0]*cos(angle)-y[1]*sin(angle);	
		yp[1]=y[0]*sin(angle)+y[1]*cos(angle);
		yp[2]=0.0;
		////到绝对坐标系后
		ToAbsolute(xp[0],xp[1],xp[2]);
		ToAbsolute(yp[0],yp[1],yp[2]);

		this->m_x[0]=xp[0]-m_org[0];
		this->m_x[1]=xp[1]-m_org[1];
		this->m_x[2]=xp[2]-m_org[2];

		this->m_y[0]=yp[0]-m_org[0];
		this->m_y[1]=yp[1]-m_org[1];
		this->m_y[2]=yp[2]-m_org[2];
	}
}

//****************************************************************************
//  Function name: ToAbsolute
//  Description:   transfer a point from wcs to absolute coordinate system
//  Output Params:  x
//                  y
//                  z
//  Output Params: No
//  Returns: no
//****************************************************************************
bool Ti_WCSYS::ToAbsolute(double &x, double &y, double &z)
{
	double xx,yy,zz;
	xx = m_org[0] + x*m_x[0] + y*m_y[0] + z*m_z[0];
	yy = m_org[1] + x*m_x[1] + y*m_y[1] + z*m_z[1];
	zz = m_org[2] + x*m_x[2] + y*m_y[2] + z*m_z[2];
	
	x=xx;
	y=yy;
	z=zz;
	return true;
}

//****************************************************************************
//  Function name: Shift
//  Description:   shift the wcs according to delta in x y z direction
//  Input Params:  x: delta in x direction
//                 y: delta in y direcion
//                 z: delta in z direction
//  Output Params: No
//  Returns: no
//****************************************************************************
void Ti_WCSYS::Shift(double x, double y, double z)
{
	m_org[0]+=x;
	m_org[1]+=y;
	m_org[2]+=z;
}

//Assumption: the vector has been normalized
bool Ti_WCSYS::SetFromOrgXYZ(double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{

	m_org[0] = x0;
	m_org[1] = y0;
	m_org[2] = z0;

	m_x[0]=x1;
	m_x[1]=y1;
	m_x[2]=z1;

	m_y[0]=x2;
	m_y[1]=y2;
	m_y[2]=z2;

	m_z[0]=x3;
	m_z[1]=y3;
	m_z[2]=z3;

	return true;
}

//Scale a point with scaleFactor relative to the working coordinate system
void Ti_WCSYS::Scale(PNT& pnt, double scale)
{
	pnt = pnt*scale;
	//pnt->x *= scale;
	//pnt->y *= scale;
	//pnt->z *= scale;
}

void Ti_WCSYS::Scale(double &x, double &y, double &z, double scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}
