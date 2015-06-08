#include "StdAfx.h"
#include "xplane.h"
#include "math.h"

XPlane::XPlane(void)
{
	this->m_ObjectType = OBJECT_PLANE;
}

XPlane::~XPlane(void)
{
}


void XPlane::InitObject()
{
	this->m_Length = 2;
	this->m_Width = 1;
	this->m_Pt   = PNT(0,0,0);
	this->m_Vector = PNT(0,0,1);
	this->m_ObjectType = OBJECT_PLANE;

} 
void XPlane::SetUnit(const int& unit)
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
			m_Length/=25.4;
			m_Width/=25.4;
		}
		break;
	case UNIT_INCH:
		if(unit == UNIT_MM)
		{
			m_Length *=25.4;
			m_Width *=25.4;
		}
		break;
	default:
		;
	}
	m_bObjChanged = true;
}

void XPlane::UpdateBndBox()
{
	if(this->m_bObjChanged)
	{
		m_bObjChanged = false;
		PNT V[4];
		V[0] = PNT(m_Length/2,  -m_Width/2,  0);		
		V[1] = PNT(m_Length/2,  m_Width/2,   0);
		V[2] = PNT(-m_Length/2, -m_Width/2,  0);
		V[3] = PNT(-m_Length/2,  m_Width/2,  0);

		for(int i = 0 ; i <4; i++)
		{
			V[i] = m_Orientation.VectorTransform(V[i]);
			V[i]  = V[i]  + this->m_Translation;
			this->m_BndBox.AddVertex(V[i]);
		}
	}
}

void XPlane::SetNormPoint(PNT aNorm, PNT aPoint)
{
	m_a = m_Vector[0] = aNorm[0];
	m_b = m_Vector[1] = aNorm[1];
	m_c = m_Vector[2] = aNorm[2];

	double sum = sqrt( m_a*m_a + m_b*m_b + m_c*m_c );
	if(sum>0)
	{
		m_a /= sum;
		m_b /= sum;
		m_c /= sum;
	}
	m_Pt = aPoint;

	m_d = -(m_a*aPoint[0] + m_b*aPoint[1] + m_c*aPoint[2]);
}

void XPlane::SetParameter(double a, double b, double c, double d)
{
	PNT origin;
	origin[0] = origin[1] = origin[2] = 0;
	m_a = a;
	m_b = b;
	m_c = c;
	m_d = d;

	double sum = sqrt( m_a*m_a + m_b*m_b + m_c*m_c );
	if(sum>0)
	{
		m_a /= sum;
		m_b /= sum;
		m_c /= sum;
		m_d /= sum;
	}
	else m_d = 0;

	SetPoint(origin);
}
void XPlane::Translate(const PNT& trans)
{
	this->m_Translation += trans;
	this->m_bDispChanged = true;
}
void XPlane::Rotate(const Ti_Rotation& R)
{
	m_Orientation = R* m_Orientation;
	this->m_bDispChanged = true;

}

void XPlane::MovePlane(double step)
{
	m_d -=step;
}

double XPlane::FitFromCld(const XCloud& cld,  bool bReject, double tolerance)
{ 
	int i;
	int num = cld.GetPntNum();
	if (num < 3)
		return 0;
	double dx = 0 , dy = 0, dz = 0;
	PNT  pnt, axispnt, axisdir, topcenter, bottomcenter;
	XCloud remaincld, tmpcld;		
	double rmserror=0;
	rmserror = cld.Fit2Plane(axispnt ,axisdir);
	tmpcld = cld;
	if(bReject)
	{			
		for(i = 1; i< 60; i++)
		{
			num  = tmpcld.GetPntNum();
			tmpcld.Slicing(axispnt, axisdir, rmserror*2 , remaincld);
			if(remaincld.GetPntNum() == num|| rmserror < tolerance)
			{
				break;
			}
			else
			{
				tmpcld  = remaincld;
				rmserror = remaincld.Fit2Plane(axispnt ,axisdir);
			}
		}
		tmpcld  = remaincld;
	}	

	/* -------------------compute length & width---------------------------*/
	PNT z_axis(0,  0, 1);	
	m_Orientation.RotationFromVecs(z_axis, axisdir);	
	m_a = axisdir[0];
	m_b = axisdir[1];
	m_c = axisdir[2];
	m_d = -(axisdir*axispnt);

	tmpcld.Rotate(axispnt, m_Orientation.m_Axis, -m_Orientation.m_Angle*PI/180.);
	tmpcld.Translate(-axispnt);	
	XBndBox box = tmpcld.GetBBox();
	m_Length =( box.GetMax() - box.GetMin())[0];	
	m_Width = ( box.GetMax() - box.GetMin())[1];
	pnt = (box.GetMax()+ box.GetMin())/2;		
	pnt = m_Orientation.VectorTransform(pnt);		
	m_Translation = pnt+ axispnt;
	/* ------------------end compute length & width-------------------------*/
	this->SetName(TEXT("Fitplane"));
	m_bObjChanged  = true;
	this->SetUnit(cld.GetUnit());
	return rmserror;	
}

void XPlane::SetPoint(PNT aPoint)
{
	double dist = m_a*aPoint[0] + m_b*aPoint[1] + m_c*aPoint[2] + m_d;
	m_Pt[0] = aPoint[0] - m_a*dist;
	m_Pt[1] = aPoint[1] - m_b*dist;
	m_Pt[2] = aPoint[2] - m_c*dist;

	m_Vector[0] = m_a;
	m_Vector[1] = m_b;
	m_Vector[2] = m_c;

}
const float& XPlane::GetLength()const
{
	return m_Length;
}
const float& XPlane::GetWidth()const
{
	return m_Width;
}
