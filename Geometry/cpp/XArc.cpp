#include "stdafx.h"
#include "math.h"
#include "stdlib.h"
#include <algorithm>
#include <ti_matrix.h>
#include "xarc.h"
#include "afxtempl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

XArc::XArc(void)
{
	m_ObjectType = OBJECT_ARC;
	m_Name = "Arc";// check if it's correct
	m_Color	=	7;
	m_bShow = true;	
	m_BndBox.Reset();	
	m_DispType = 1;
	m_bObjChanged = true;
//	this->m_GLID = -1;
	Initial();

	localsys.Init();
}

XArc::XArc(const PNT &c, const PNT &s, const PNT &e, const PNT &n)
{
	m_Cen = c;
	m_StartPt = s;
	m_EndPt = e;
	Initial();
}
XArc::XArc(const double& r,const PNT &c, const PNT &axis_dir,const double& sa,const double& ea)
//XArc::	XArc(const double& r =0, const PNT &c=PNT(0,0,1),const PNT &axis_dir=PNT(0,0,1),double sa=0,double ea=0)
{
	m_Cen = c;
	m_Normal = axis_dir;
	m_Rad = r;
	m_StartAngle = sa;
	m_EndAngle = ea;
	Initial();
}
XArc::~XArc(void)
{
}

void XArc::SetUnit(const int& unit)
{
	if(this->m_Unit == UNIT_NONE|| m_Unit == unit)
		{
		m_Unit = unit;
		return;
		}
	switch(m_Unit)// suppose only two type unit(inch and mm)
		{
		case UNIT_MM:
			if(unit == UNIT_INCH)
				{
					m_Rad/=25.4;
					m_Cen/=25.4;
					m_StartPt/=25.4;
					m_EndPt/=25.4;
				}
			break;
		case UNIT_INCH:
			if(unit == UNIT_MM)
				{
					m_Rad/=25.4;
					m_Cen/=25.4;
					m_StartPt/=25.4;
					m_EndPt/=25.4;
				
				}
			break;
		default:
			;
		}
	m_bObjChanged = true;
}

void XArc:: InitObject() 
{
}

void XArc::Initial()
{
	m_ObjectType = OBJECT_ARC;
	m_Name = "Arc";// check if it's correct
	m_Color	=	7;
	m_bShow = true;	
	m_BndBox.Reset();	
	m_DispType = 1;
	m_bObjChanged = true;
	m_DelFlag = true;

	m_Cen = PNT(0,0,0);
	m_StartPt = m_Cen;
	m_EndPt = m_Cen;
}

XArc& XArc::operator = (const XArc& aArc)
{
	m_c0 = aArc.m_c0;
	m_c1 = aArc.m_c1;
	m_c2 = aArc.m_c2;
	m_c3 = aArc.m_c3;

	m_StartAngle = aArc.m_StartAngle;
	m_EndAngle = aArc.m_EndAngle;
	m_Cen = aArc.m_Cen;
	m_Rad = aArc.m_Rad;

	m_DelFlag = aArc.m_DelFlag;
	return (*this);
}

//****************************************************************************
//  Calculator name: MiddlePoint
//  Description:   caculate the middle point of a arc
//  Returns: the middle point
//****************************************************************************
PNT XArc::MiddlePoint()
{
	PNT aVector;
	double mid_angle  = (m_StartAngle + m_EndAngle)/2;

	aVector[0] = m_Cen[0] + m_Rad * cos(mid_angle);
	aVector[1] = m_Cen[1] + m_Rad * sin(mid_angle);
	aVector[2] = m_Cen[2];	

	return aVector;
}


void XArc::ChangeUnits(bool bmm2inch )
{
	if(bmm2inch)
	{
		m_Cen /= 25.4f;
		m_Rad /= 25.4f;
	}
	else
	{
		m_Cen *= 25.4f;
		m_Rad *= 25.4f;
	}
	m_bObjChanged = true;
}
	void XArc::Translate(const PNT& trans)
	{

	}
	void XArc::Rotate(const Ti_Rotation& R)
	{
	}

void XArc::UpdateBndBox(void)
{	
	PNT aStrt, aEnd, aQuad0, aQuad1, aQuad2, aQuad3;

	if(m_bObjChanged)
	{
		m_BndBox.Reset();

		aStrt[0] = m_Cen[0] + m_Rad * cos(m_StartAngle);
		aStrt[1] = m_Cen[1] + m_Rad * sin(m_StartAngle);
		aStrt[2] = m_Cen[2];

		aEnd[0] = m_Cen[0] + m_Rad * cos(m_EndAngle);
		aEnd[1] = m_Cen[1] + m_Rad * sin(m_EndAngle);
		aEnd[2] = m_Cen[2];	

		aQuad0[0] = m_Cen[0] + m_Rad * cos((double)0);
		aQuad0[1] = m_Cen[1] + m_Rad * sin((double)0);
		aQuad0[2] = m_Cen[2];

		aQuad1[0] = m_Cen[0] + m_Rad * cos(0.5*PI);
		aQuad1[1] = m_Cen[1] + m_Rad * sin(0.5*PI);
		aQuad1[2] = m_Cen[2];

		aQuad2[0] = m_Cen[0] + m_Rad * cos(PI);
		aQuad2[1] = m_Cen[1] + m_Rad * sin(PI);
		aQuad2[2] = m_Cen[2];

		aQuad3[0] = m_Cen[0] + m_Rad * cos(1.5*PI);
		aQuad3[1] = m_Cen[1] + m_Rad * sin(1.5*PI);
		aQuad3[2] = m_Cen[2];


		m_BndBox.AddVertex( aStrt );
		m_BndBox.AddVertex( aEnd );

		if(m_StartAngle>0 && m_StartAngle<0.5*PI)
		{
			if(m_EndAngle>=0.5*PI)
			{
				m_BndBox.AddVertex( aQuad1 );
			
				if(m_EndAngle>=PI)
					m_BndBox.AddVertex( aQuad2 );
			
				if(m_EndAngle>=1.5*PI)
					m_BndBox.AddVertex( aQuad3 );
			}
			else if(m_EndAngle<m_StartAngle)
			{
				m_BndBox.AddVertex( aQuad0 );
				m_BndBox.AddVertex( aQuad1 );
				m_BndBox.AddVertex( aQuad2 );
				m_BndBox.AddVertex( aQuad3 );

			}

		}
		else if(m_StartAngle>=0.5*PI && m_StartAngle<PI)
		{		
			if(m_EndAngle>=PI)
			{
				m_BndBox.AddVertex( aQuad2 );
		
				if(m_EndAngle>=1.5*PI)
					m_BndBox.AddVertex( aQuad3 );
			}
			else
			{
				if(m_EndAngle>=0.5*PI)
					m_BndBox.AddVertex( aQuad1 );

				m_BndBox.AddVertex( aQuad0 );
				m_BndBox.AddVertex( aQuad2 );
				m_BndBox.AddVertex( aQuad3 );

			}
		}
		else if(m_StartAngle>=PI && m_StartAngle<1.5*PI)
		{	
			if(m_EndAngle>=1.5*PI)
				m_BndBox.AddVertex( aQuad3 );
			else
			{
				if(m_EndAngle>=PI && (m_StartAngle>m_EndAngle))
					m_BndBox.AddVertex( aQuad2 );
				if(m_EndAngle>0.5*PI)
					m_BndBox.AddVertex( aQuad1 );
					
				m_BndBox.AddVertex( aQuad0 );

			}
		}
		else //if(m_StartAngle>=1.5*PI && m_StartAngle<2*PI)
		{
			if((m_StartAngle>m_EndAngle))
			{
				m_BndBox.AddVertex( aQuad0 );
				if(m_EndAngle>1.5*PI)
					m_BndBox.AddVertex( aQuad3 );
				if(m_EndAngle>PI)
					m_BndBox.AddVertex( aQuad2 );
				if(m_EndAngle>0.5*PI)
					m_BndBox.AddVertex( aQuad1 );
			}
		}

		PNT & min = m_BndBox.GetMin();
		PNT & max = m_BndBox.GetMax();
		for( int i=0;i<3;i++)
		{
			if(max[2]-min[2]<0.1) 
			{
				min[i] -= 0.05;
				max[i] += 0.05;
			}
		}
		m_bObjChanged = false;
	}
}
//void XArc::SetRad(double r)
//{
//	m_Rad = r;
//}

const float&  XArc::GetRadius()
{
	return m_Rad;
}
const double& XArc::GetStartAngle()
{
	return m_StartAngle;
}

const double& XArc::GetEndAngle()
{
	return m_EndAngle;
}

PNT& XArc::GetNormal()
{
	return m_Normal;
}

PNT& XArc::GetCenter()
{
	return m_Cen;
}
bool XArc::GetDelFlag()
{
	return m_DelFlag;
}
void XArc::SetDelFlag(bool flag)
{
	m_DelFlag = flag;
}
void XArc::Set(double r,PNT &c, PNT &axis_dir,double sa,double ea)
{
	m_Cen = c;
	m_Normal = axis_dir;
	m_Rad = r;
	m_StartAngle = sa;
	m_EndAngle = ea;
}

//求点到圆弧的最近距离
float XArc::Distance(PNT &pt)
{
	PNT nearpt = NearPt(pt);

	return (nearpt-pt).Length();
}
//计算弧线上距离pt最近的点
PNT XArc::NearPt(PNT pt)
{
	float prop = Proportion(pt);

	//始点方向
	PNT dirBpt = m_StartPt-m_Cen;
	dirBpt.Normalize();
	
	//终点方向
	PNT dirEpt = m_EndPt-m_Cen;
	dirEpt.Normalize();

	PNT dir = dirBpt^dirEpt;//dirBpt.CrossProduct(dirEpt);
	
	float angle;
	if(dir*m_Normal>0)//始点，终点夹角小于180度
		angle = acos(dirBpt*dirEpt);
	else
		angle = PI*2.0-acos(dirBpt*dirEpt);

	float Rotangle = prop*angle;//旋转角度
	
	//最近点
	PNT nearpt = m_StartPt;
	nearpt.Rotate(m_Cen,m_Normal,Rotangle);
	
	return nearpt;
}
//计算弧线上距离pt最近的点将弧的分割比例
float XArc::Proportion(PNT pt)
{
	PNT Center = m_Cen;
	PNT Bpt = m_StartPt;
	PNT Ept = m_EndPt;

	//平移到原点
	pt -= Center;
	Bpt -= Center;
	Ept -= Center;

	//旋转，使圆弧平面的法向量和z轴重合
	pt.Transform_nDirLooksasZ(m_Normal);
	Bpt.Transform_nDirLooksasZ(m_Normal);
	Ept.Transform_nDirLooksasZ(m_Normal);

	//pt 在平面上的投影
	PNT ProPt(pt[0],pt[1],0);
	
	PNT dirBpt = Bpt;
	dirBpt.Normalize();
	PNT dirEpt = Ept;
	dirEpt.Normalize();
	PNT dirPt = ProPt;
	dirPt.Normalize();

	float angleBpt;//始点的逆时针角度
	if(dirBpt[1]>0)//一、二象限
		angleBpt = acos(dirBpt[0]);
	else
		angleBpt = PI*2.0-acos(dirBpt[0]);
	//旋转使开始点和x轴正向重合
	pt.Rotate(PNT(0,0,0),PNT(0,0,1),PI*2.0-angleBpt);
	Bpt.Rotate(PNT(0,0,0),PNT(0,0,1),PI*2.0-angleBpt);
	Ept.Rotate(PNT(0,0,0),PNT(0,0,1),PI*2.0-angleBpt);

	dirPt.Rotate(PNT(0,0,0),PNT(0,0,1),PI*2.0-angleBpt);
	dirBpt.Rotate(PNT(0,0,0),PNT(0,0,1),PI*2.0-angleBpt);
	dirEpt.Rotate(PNT(0,0,0),PNT(0,0,1),PI*2.0-angleBpt);

	float angleEpt;//终点的逆时针角度
	if (dirEpt[1]>0)
		angleEpt = acos(dirEpt[0]);
	else
		angleEpt = PI*2.0-acos(dirEpt[0]);

	float anglePt;//当前点的逆时针角度
	if (dirPt[1]>0)
		anglePt = acos(dirPt[0]);
	else
		anglePt = PI*2.0-acos(dirPt[0]);

	float prop;//分割比例

	if(anglePt<angleEpt)//在始点和终点之间
		prop = anglePt/angleEpt;
	else
	{
		if(anglePt>(angleEpt/2.0+PI))
			prop = 0.0;
		else
			prop = 1.0;
	}

	return prop;
}

