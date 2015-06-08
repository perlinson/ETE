// De_IntplPnt.cpp: implementation of the De_IntplPnt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "De_IntplPnt.h"
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

De_IntplPnt::De_IntplPnt()
{
	m_multi=1;
	m_IsPicked=0;
}

De_IntplPnt::~De_IntplPnt()
{

}

De_IntplPnt::De_IntplPnt(De_BPnt& bpnt)
{
	m_pt=bpnt ;
	m_multi=1;
	m_IsPicked=0;
	knot_u=0.0;
}


De_IntplPnt::De_IntplPnt(const De_IntplPnt& b)
{
	m_pt=b.m_pt;
	m_multi=b.m_multi;
	m_IsPicked=b.m_IsPicked;
	knot_u=b.knot_u;
}

De_IntplPnt &De_IntplPnt::operator =(De_IntplPnt b)
{
	m_pt=b.m_pt;
	m_multi=b.m_multi;
	m_IsPicked=b.m_IsPicked;
	knot_u=b.knot_u;
	return(*this);
}


bool De_IntplPnt::operator !=(De_IntplPnt b)
{
	double dist;
	dist=sqrt((m_pt.m_x-b.m_pt.m_x)*(m_pt.m_x-b.m_pt.m_x)+(m_pt.m_y-b.m_pt.m_y)*(m_pt.m_y-b.m_pt.m_y)\
		+(m_pt.m_z-b.m_pt.m_z)*(m_pt.m_z-b.m_pt.m_z));
	if(dist>0.001) // this small number is to judge repeat point //jjjj2008
		return true;
	else return false;
}

bool De_IntplPnt::operator ==(De_IntplPnt b)
{
	double dist;
	dist=sqrt((m_pt.m_x-b.m_pt.m_x)*(m_pt.m_x-b.m_pt.m_x)+(m_pt.m_y-b.m_pt.m_y)*(m_pt.m_y-b.m_pt.m_y)\
		+(m_pt.m_z-b.m_pt.m_z)*(m_pt.m_z-b.m_pt.m_z));
	if(dist<0.001) // this small number is to judge repeat point //jjjj2008
		return true;
	else return false;
}
