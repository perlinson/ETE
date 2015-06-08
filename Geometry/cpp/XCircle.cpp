#include "StdAfx.h"
#include "xcircle.h"

XCircle::XCircle(void)
{
	m_ObjectType = OBJECT_CIRCLE;
	m_Name = "Circle";// check if it's correct
	m_Color	=	7;
	m_bShow = true;	
	m_BndBox.Reset();	
	m_DispType = 1;
	m_bObjChanged = true;
	m_Norm_plane = PNT(0,0,1);
	m_LineStyle = 1;
	m_LineWidth = 1.0;
	m_Radius = 0.0;
    m_StartAngle=0.0;            /* expressed in radians */
    m_EndAngle=2*3.1415926;              /* expressed in radians */

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
}

XCircle::~XCircle(void)
{
}

void	XCircle::UpdateBndBox()
{
}

void XCircle::Translate(const PNT& delt_t)
{
	m_Center += delt_t;
}

void XCircle::Rotate(const Ti_Rotation& R)
{
	this->m_Orientation =  R*m_Orientation;
}

void XCircle::SetUnit(const int& unit)
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
					m_Radius/=25.4;
					m_Center/=25.4;
				}
			break;
		case UNIT_INCH:
			if(unit == UNIT_MM)
				{
					m_Radius*=25.4;
					m_Center*=25.4;			
				}
			break;
		default:
			;
		}
	m_bObjChanged = true;
}

void XCircle::InitObject() 
{
	this->m_Radius  = 0;
	this->m_Center = PNT(0, 0, 0);
	this->m_bObjChanged = true;
    m_StartAngle=0.0;            /* expressed in radians */
    m_EndAngle=2*3.1415926;              /* expressed in radians */

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
}

void	XCircle::SetLineStyle(int ls)
{
	m_LineStyle = ls;
}

void	XCircle::SetLineWidth(float lw)
{
	m_LineWidth = lw;
}

int		XCircle::GetLineStyle()
{
	return m_LineStyle;
}

float	XCircle::GetLineWidth()
{
	return m_LineWidth;
}

//by jm March 6 2006 ******///
double XCircle::GetAngle(PNT& pnt)
{
	if( fabs(pnt[1] - m_Center[1]) < 0.0000001)
	{
		if(pnt[0] >= m_Center[0]) 
			return 0.0;
		else
			return PI;
	}
	if( fabs(pnt[0] - m_Center[0]) < 0.0000001)
	{
		if(pnt[1] >= m_Center[1]) 
			return PI/2.0;
		else
			return PI*3.0/2.0;
	}
	//other situation
	double mathsine, mathcos, dis, arcmathsin;
	dis = sqrt((pnt[1] - m_Center[1])*(pnt[1] - m_Center[1])+ (pnt[0] - m_Center[0])*(pnt[0] - m_Center[0]));
	mathsine = (pnt[1] - m_Center[1]) / dis;
	mathcos = (pnt[0] - m_Center[0] ) / dis;

	arcmathsin = asin(mathsine);
	if(mathsine > 0 )
	{
		if(mathcos > 0)
			return arcmathsin;
		else
			return PI - arcmathsin;
	}
	else
	{
		if(mathcos > 0)
			return 2*PI + arcmathsin;
		else
			return PI - arcmathsin;
	}


	return 0.0;
}
void	XCircle::SetAngle(PNT& sp, PNT& ep)
{
	double sa,ea;
	sa = GetAngle(sp);
	ea = GetAngle(ep);
	if(ea < sa) ea = ea+ 2*PI;

	if((ea - sa ) > PI)
	{
		double tt = sa;
		sa = ea-2*PI;
		ea = tt;
	}

	m_StartAngle = sa;
	m_EndAngle = ea;

	return;
}
double XCircle::GetStartAngle()
{
	return m_StartAngle;
}
double XCircle::GetEndAngle()
{
	return m_EndAngle;
}


//by jm Feb 7 2006 ***************//
void	XCircle::SetShowBox(bool flag)
{
	m_bShowBox = flag;
}
bool	XCircle::GetShowBox()
{
	return m_bShowBox;
}
bool	XCircle::SetBox(double left[3], double right[3])
{
	m_BoxLLeft[0] = left[0];
	m_BoxLLeft[1] = left[1];
	m_BoxLLeft[2] = 0.0;
	m_BoxURight[0]= right[0];
	m_BoxURight[1]= right[1];
	m_BoxURight[2]= 0.0;
	return true;
}