#include "StdAfx.h"
#include "XGeomBase.h"
#include "pnt.h"

XGeomBase::XGeomBase(void)
	{
	m_Name		= _T("");	// Object name
	m_bShow		= true;    
	m_bSelected		= false;
	m_Annotation = false;
	m_bShowName = false;
	m_Layer		= 0;
	m_bObjChanged	= true; // which is used to update the object data information
	m_Color			= WHITE_COLOR;
	m_Unit			= UNIT_NONE;
	m_uDisplayList	= 0;
	m_bDispChanged = true;
	//by jm Feb 7 2006 **************//
	m_Attribute = GEOM_NONE;
	}
XGeomBase::~XGeomBase(void)
	{
	}
XGeomBase::XGeomBase(const XGeomBase& obj)
	{
	*this = obj;
	return;
	}
XGeomBase& XGeomBase::operator=(const XGeomBase& obj)
	{
	m_Name		= obj.m_Name;	// Object name
	m_bShow		= obj.m_bShow; 
	m_bShowName = obj.m_bShowName;
	m_bSelected		= obj.m_bSelected;
	m_bObjChanged	= obj.m_bObjChanged; // which is used to update the object data information
	m_ObjectType	= obj.m_ObjectType; //
	m_Color			= obj.m_Color;
	m_Annotation   = obj.m_Annotation;
	m_DispType  = obj.m_DispType;
	m_Layer		 = obj.m_Layer;
	m_Unit			= obj.m_Unit;
	m_Orientation = obj.m_Orientation;
	m_Translation = obj.m_Translation;
	m_BndBox		= obj.m_BndBox;   // bound box of this object
	m_bDispChanged = obj.m_bDispChanged;
	//by jm Feb 7 2006 **********************//
	m_Attribute = obj.m_Attribute;
	return *this;
	}

const CString&XGeomBase:: GetName() const
	{
	return m_Name;
	}
void XGeomBase::SetName(const CString& name)
	{
	m_Name = name;
	}

const int& XGeomBase::GetLayer()const
	{
	return m_Layer;
	}
void XGeomBase::SetLayer(const int& layer)
	{
	m_Layer = layer;
	}

const int& XGeomBase::GetColor()const
	{
	return m_Color;
	}
void XGeomBase::SetColor(const int& color)
	{
		if(m_Color == color)
			return;
		m_bDispChanged = true;
	m_Color = color;
	}
const bool&XGeomBase:: IsShown()const
	{
	return m_bShow;
	}
	void XGeomBase::SetVisible(const bool& bshow)
	{
		m_bShow = bshow;
	}

	void XGeomBase::SetObjState(bool state)
	{
		this->m_bObjChanged = state;
	}

	bool& XGeomBase::IsDispChanged()
	{
		if(m_bObjChanged)
		{
			m_bDispChanged = true;
		}
		return m_bDispChanged;
	}

	//
	const bool& XGeomBase::IsSelected()const
	{
		return m_bSelected;
	}
void XGeomBase::SetSelState(const bool& bselected)
	{
	m_bSelected = bselected;
	}

	//object type operation
const int& XGeomBase::GetType()const
	{
	return m_ObjectType;
	}
int&  XGeomBase::GetDispType()
	{
	return this->m_DispType;
	}
void XGeomBase::SetDispType(const int & disptype)
	{
		if(m_DispType == disptype)
			return;
		m_bDispChanged = true;
	m_DispType = disptype;
	}
	//unit operation
const int& XGeomBase::GetUnit()const
	{
	return m_Unit;
	}

	//get boundbox 
const XBndBox&XGeomBase:: GetBBox()
	{
	UpdateBndBox();
	return m_BndBox;
	}

void XGeomBase::SetAnnotation(bool flag)
{
	m_Annotation = flag;
}
bool XGeomBase::GetAnnotation()
{
	return m_Annotation;
}

void XGeomBase::SetShowName(bool flag)
{
	m_bShowName = flag;
}
bool XGeomBase::GetShowName()
{
	return m_bShowName;
}
//by jm Feb 7 2006 *************************//
const int& XGeomBase::GetAttribute()const
{
	return m_Attribute;
}
void  XGeomBase::SetAttribute(const int &attr)
{
	m_Attribute = attr;
}

