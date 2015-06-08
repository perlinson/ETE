#include "StdAfx.h"
#include "XFreeform.h"
#include "math.h"


XFreeform::XFreeform(void)
{
	this->m_ObjectType = OBJECT_FREEFORM;
}

XFreeform::~XFreeform(void)
{   
	if(pTriMesh)
		delete pTriMesh;
}

void XFreeform::Translate(const PNT& delt_t)
{
	this->m_Translation += delt_t;
	this->m_bDispChanged = true;
}
void XFreeform::Rotate(const Ti_Rotation& R)
{
	m_Orientation = R*m_Orientation;
	this->m_bDispChanged = true;
}

void XFreeform::UpdateBndBox()
{
	if(this->m_bObjChanged)
	{
		m_bObjChanged = false;
		m_bDispChanged = true;
        
	}
}


void XFreeform::SetUnit(const int& unit)
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
/*			for(int i=0;i<m_Unum*m_Vnum;i++)
			   XFreeform::m_CPt[i]/=25.4f;		*/	
		}
		break;
	case UNIT_INCH:
		if(unit == UNIT_MM)
		{
				//for(int i=0;i<m_Unum*m_Vnum;i++)
			 //  XFreeform::m_CPt[i]*=25.4f;
		}
		break;
	default:
		;
	}
	SetObjState(true);	
}


void XFreeform::InitObject()
{
	SetObjState(true);
	m_bDispChanged = true;

}

void XFreeform::GetFreeformInfo(int &Unum,int Vnum)
{
	for(int i=0;i<m_Unum*m_Vnum;i++)
  //  CPt[i]=this->m_CPt[i];
	Unum=this->m_Unum;
	Vnum=this->m_Vnum;
}