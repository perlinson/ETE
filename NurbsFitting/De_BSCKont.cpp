// De_BSCKont.cpp: implementation of the De_BSCKont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "De_BSCKont.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

De_BSCKnot::De_BSCKnot()
{
	m_u = 0.0;
	m_multi = 1;	
	m_type = 0;
}

De_BSCKnot::~De_BSCKnot()
{

}

De_BSCKnot::De_BSCKnot(const De_BSCKnot& d)
{
	m_u = d.m_u;
	m_multi = d.m_multi;
	m_type = d.m_type;
}

De_BSCKnot &De_BSCKnot::operator =(De_BSCKnot d)
{
	m_u = d.m_u;
	m_multi = d.m_multi;	
	m_type = d.m_type;
	return (*this);
}
