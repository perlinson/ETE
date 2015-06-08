#include "stdafx.h"
#include "math.h"
#include "stdlib.h"
#include <algorithm>
#include <ti_matrix.h>
#include "xline.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

XLine::XLine(void)
{
	m_ObjectType = OBJECT_LINE;
	m_Name = "Line";// check if it's correct
	m_Color	=	3;
	//m_bRawdata = false;
	//mDrawBBox = false;
	//mMaxLeng = 0;	
	m_bShow = true;	
	m_BndBox.Reset();	
	m_DispType = 1;
	m_bObjChanged = true;
//	this->m_GLID = -1;
	m_LineWidth = 1.0;
	m_Track = -1;
	m_LineStyle = 1;

	//by jm Feb 7th 2006 *************************************************//
	m_bShowBox=false;
	m_BoxLLeft[0] = 0.0;
	m_BoxLLeft[1] = 0.0;
	m_BoxLLeft[2] = 0.0;
	m_BoxURight[0] = 0.0;
	m_BoxURight[1] = 0.0;
	m_BoxURight[2] = 0.0;
	m_FitPntNum = 0;
	m_RMS = -1.0;

	//by jm Feb 14 2006 *******************************//
	m_st = m_ed = 0;

	Initial();
}
XLine::XLine(PNT &s, PNT &e)
{
	m_ObjectType = OBJECT_LINE;
	m_Name = "Line";// check if it's correct
	m_Color	=	3;
	//m_bRawdata = false;
	//mDrawBBox = false;
	//mMaxLeng = 0;	
	m_bShow = true;	
	m_BndBox.Reset();	
	m_DispType = 1;
	m_bObjChanged = true;
//	this->m_GLID = -1;
	m_StartPt = s;
	m_EndPt = e;
	Initial();

	mL0 = m_StartPt[1] - m_EndPt[1];
	mL1 = m_EndPt[0] - m_StartPt[0];
	mL2 = (m_StartPt[0]-m_EndPt[0])*m_EndPt[1]-(m_StartPt[1]-m_EndPt[1])*m_EndPt[0];
}
XLine::~XLine(void)
{
}
void XLine::SetCoff()
{
	mL0 = m_StartPt[1] - m_EndPt[1];
	mL1 = m_EndPt[0] - m_StartPt[0];
	mL2 = (m_StartPt[0]-m_EndPt[0])*m_EndPt[1]-(m_StartPt[1]-m_EndPt[1])*m_EndPt[0]; 
}

void XLine:: InitObject() 
{
}
void XLine:: Translate(const PNT& delt_t)
{
}
void XLine:: Rotate(const Ti_Rotation& R)
{
}

XLine& XLine::operator = (const XLine& aLine)
{
	m_ObjectType = OBJECT_LINE;

	mL0 = aLine.mL0;
	mL1 = aLine.mL1;
	mL2 = aLine.mL2;

	m_StartPt = aLine.m_StartPt;
	m_EndPt = aLine.m_EndPt;
	m_Color = aLine.m_Color;
	//m_Name = aLine.m_Name;  //error why?
	m_DelFlag = aLine.m_DelFlag;
	mL0 = aLine.mL0;
	mL1 = aLine.mL1;
	mL2 = aLine.mL2; 

	m_LineWidth = aLine.m_LineWidth;
	m_Track = aLine.m_Track;
	m_LineStyle = aLine.m_LineStyle;

	m_AnPnt = aLine.m_AnPnt;

	//by jm Feb 7th 2006 *************************************************//
	m_bShowBox=aLine.m_bShowBox;
	m_BoxLLeft[0] = aLine.m_BoxLLeft[0];
	m_BoxLLeft[1] = aLine.m_BoxLLeft[1];
	m_BoxLLeft[2] = aLine.m_BoxLLeft[2];
	m_BoxURight[0] = aLine.m_BoxURight[0];
	m_BoxURight[1] = aLine.m_BoxURight[1];
	m_BoxURight[2] = aLine.m_BoxURight[2];
	m_FitPntNum = aLine.m_FitPntNum;
	m_RMS = aLine.m_RMS;

	//by jm Feb 14 2006 **********************//
	m_st = aLine.m_st;
	m_ed = aLine.m_ed;

	return (*this);
}

void XLine::SetUnit(const int& unit)
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
				m_StartPt/=25.4;
				m_EndPt/=25.4;
			}
			break;
		case UNIT_INCH:
			if(unit == UNIT_MM)
			{
				m_StartPt*=25.4;
				m_EndPt*=25.4;
			
			}
			break;
		default:
			;
		}
	m_bObjChanged = true;
}

void XLine::Initial()
{
	m_ObjectType = OBJECT_LINE;
	m_Name = "Line";// check if it's correct
	m_Color	=	5;
	//m_bRawdata = false;
	//mDrawBBox = false;
	//mMaxLeng = 0;	
	m_bShow = true;	
	m_BndBox.Reset();	
	m_DispType = 1;
	m_bObjChanged = true;
//	this->m_GLID = -1;
	m_DelFlag = true;
	m_LineStyle = 1;
	//by jm Feb 7th 2006 *************************************************//
	m_bShowBox=false;
	m_BoxLLeft[0] = 0.005;
	m_BoxLLeft[1] = 0.005;
	m_BoxLLeft[2] = 0.0;
	m_BoxURight[0] = 0.01;
	m_BoxURight[1] = 0.01;
	m_BoxURight[2] = 0.0;
	m_FitPntNum = 0;
	m_RMS = -1.0;
	m_Attribute = GEOM_LINE;
	//by jm Feb 14 2006 **************//
	m_st = m_ed = 0;

}
//****************************************************************************
//  Calculator name: MiddlePoint
//  Description:   caculate the middle point of a line
//  Returns:  middle point
//****************************************************************************
PNT XLine::MiddlePoint()
{
	PNT aVector;
	aVector[0] = (m_StartPt[0] + m_EndPt[0])/2;
	aVector[1] = (m_StartPt[1] + m_EndPt[1])/2;
	aVector[2] = (m_StartPt[2] + m_EndPt[2])/2;

	return aVector;
}

void XLine::ChangeUnits(bool bmm2inch )
{
	PNT pnt;
	if(bmm2inch)
	{
		m_StartPt /= 25.4f;
		m_EndPt /= 25.4f;
		}
	else
	{
		m_StartPt *= 25.4f;
		m_EndPt *= 25.4f;
	}
	m_bObjChanged = true;
}

void XLine::UpdateBndBox()
{	
	if(m_bObjChanged)
	{
		m_BndBox.Reset();

		m_BndBox.AddVertex( m_StartPt );
		m_BndBox.AddVertex( m_EndPt );

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

void XLine::Set(PNT &s,PNT &e)
{
	m_StartPt = s;
	m_EndPt = e;
}
bool XLine::GetDelFlag()
{
	return m_DelFlag;
}
void XLine::SetDelFlag(bool flag)
{
	m_DelFlag = flag;
}

//The distance from pt to this Line
float XLine::Distance(PNT &pt)
{
	PNT dir = m_EndPt-m_StartPt;
	dir.Normalize();
	float t1 = (pt-m_StartPt)*dir;
	//float t2 = pt.Distance(m_StartPt);
	float t2 = (pt-m_StartPt).Length();
	if (t2*t2<t1*t1)
	return 0;

	return sqrt(t2*t2-t1*t1);
}

void XLine::Extend(double lm)
{
	PNT dir = m_EndPt-m_StartPt;
	dir.Normalize();
	m_StartPt = m_StartPt-lm*dir;
	m_EndPt = m_EndPt+lm*dir;
	return;
}
//求点到线段的最近距离
float XLine::SectDistance(PNT &pt)
{
	float dis;
	PNT ProPt, dir, dir1;
	dir = m_EndPt-m_StartPt;//线段方向
	dir.Normalize();
	//得到垂足
	ProPt = GetProjectPt(pt);
	dir1 = ProPt-m_StartPt;
	dir1.Normalize();
	if (dir1*dir<0)//最近点为m_StartPt
		dis = (m_StartPt-pt).Length();//.Distance(pt);
	else
	{
		if((ProPt-m_StartPt).Length()>(m_EndPt-m_StartPt).Length())
			dis = (m_EndPt-pt).Length();//.Distance(pt);
		else
			dis = (ProPt-pt).Length();//.Distance(pt); 
	}
	return dis;
}

//求线段上距离pt最近的点
PNT XLine::NearPt(PNT &pt)
{
	PNT ProPt, nearpt, dir, dir1;
	ProPt = GetProjectPt(pt);
	dir = m_EndPt-m_StartPt;
	dir1 = ProPt-m_StartPt;
	if (dir*dir1<0)
		nearpt = m_StartPt;
	else
	{
		if((ProPt-m_StartPt).Length()>(m_EndPt-m_StartPt).Length())
			nearpt = m_EndPt;
		else
			nearpt = ProPt;
	}
	return nearpt;
}

float XLine::FallProportion(PNT &pt)
{
	PNT nearpt = NearPt(pt);
	return ((nearpt-m_StartPt).Length()/(m_EndPt-m_StartPt).Length());
}

PNT XLine::GetProjectPt(PNT &pt)
{
	PNT dir = m_EndPt-m_StartPt;
	dir.Normalize();

	float t = (pt-m_StartPt)*dir;

	return (m_StartPt+t*dir);
}

double XLine::GetLen()
{
	return sqrt((m_StartPt[0]-m_EndPt[0])*(m_StartPt[0]-m_EndPt[0]) +
				(m_StartPt[1]-m_EndPt[1])*(m_StartPt[1]-m_EndPt[1]) +
				(m_StartPt[2]-m_EndPt[2])*(m_StartPt[2]-m_EndPt[2]) );
}

void XLine::SetLineStyle(int ls)
{
	m_LineStyle = ls;
}
//by jm Feb 7 2006 ***************//
void	XLine::SetShowBox(bool flag)
{
	m_bShowBox = flag;
}
bool	XLine::GetShowBox()
{
	return m_bShowBox;
}
bool	XLine::SetBox(double left[3], double right[3])
{
	m_BoxLLeft[0] = left[0];
	m_BoxLLeft[1] = left[1];
	m_BoxLLeft[2] = 0.0;
	m_BoxURight[0]= right[0];
	m_BoxURight[1]= right[1];
	m_BoxURight[2]= 0.0;
	return true;
}
