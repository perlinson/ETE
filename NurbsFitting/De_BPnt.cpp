// De_BPnt.cpp: implementation of the De_BPnt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "De_BPnt.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

De_BPnt::De_BPnt()
{
	m_x=0.0;
	m_y=0.0;
	m_z=0.0;
	m_nx=0.0;
	m_ny=0.0;
	m_nz=1.0;
	m_IsPicked=0;
}

De_BPnt::~De_BPnt()
{

}

De_BPnt::De_BPnt(const De_BPnt& p)
{
	m_x = p.m_x;
	m_y = p.m_y;
	m_z = p.m_z;
	m_nx = p.m_nx;
	m_ny = p.m_ny;
	m_nz = p.m_nz;
	m_IsPicked=p.m_IsPicked;
}

De_BPnt &De_BPnt::operator =(De_BPnt p)
{
	m_x = p.m_x;
	m_y = p.m_y;
	m_z = p.m_z;
	m_nx = p.m_nx;
	m_ny = p.m_ny;
	m_nz = p.m_nz;
	m_IsPicked=p.m_IsPicked;
	return(*this);
}
  
BOOL  De_BPnt::operator ==(De_BPnt a)
{
	double dist;
	dist=sqrt((m_x-a.m_x)*(m_x-a.m_x)+(m_y-a.m_y)*(m_y-a.m_y)\
		+(m_z-a.m_z)*(m_z-a.m_z));

	if(dist<1.0e-2)
		return TRUE;
	else 
		return FALSE;
}

De_BPnt De_BPnt::operator +(De_BPnt p)
{
	De_BPnt pnt;
	pnt.m_x = m_x + p.m_x;
	pnt.m_y = m_y + p.m_y;
	pnt.m_z = m_z + p.m_z;
	return(pnt);
}

De_BPnt De_BPnt::operator -(De_BPnt p)
{
	De_BPnt pnt;
	pnt.m_x = m_x - p.m_x;
	pnt.m_y = m_y - p.m_y;
	pnt.m_z = m_z - p.m_z;
	return(pnt);
}

De_BPnt De_BPnt::operator /(double t)
{
	De_BPnt pnt;
	pnt.m_x = m_x/t;
	pnt.m_y = m_y/t;
	pnt.m_z = m_z/t;
	return(pnt);
}

De_BPnt De_BPnt::operator *(double t)
{
	De_BPnt pnt;
	pnt.m_x = m_x*t;
	pnt.m_y = m_y*t;
	pnt.m_z = m_z*t;
	return(pnt);
}

double De_BPnt::Dst2Pnt(De_BPnt& pt)
{
	return (sqrt((m_x-pt.m_x)*(m_x-pt.m_x)+(m_y-pt.m_y)*(m_y-pt.m_y)+
			(m_z-pt.m_z)*(m_z-pt.m_z)));
}
