#include "StdAfx.h"
#include "xlayer.h"

XLayer::XLayer(void)
{
	m_Name = _T("Layer");
	m_bShow = true;
	m_bSelectable= true;
}

XLayer::~XLayer(void)
{
}

XLayer::XLayer(const XLayer& alayer)
{
	m_Name = alayer.m_Name;
	m_bShow = alayer.m_bShow;
	m_bSelectable = alayer.m_bSelectable;
	m_Color = alayer.m_Color;
	m_Index = alayer.m_Index;
}

XLayer& XLayer::operator=(const XLayer& alayer)
{
	m_Name = alayer.m_Name;
	m_bShow = alayer.m_bShow;
	m_bSelectable = alayer.m_bSelectable;
	m_Color = alayer.m_Color;
	m_Index = alayer.m_Index;
	return *this;
}

CString XLayer::GetName() const
{
	return m_Name;
}

bool XLayer::SetName(const CString & name)
{
	m_Name = name;
	return true;
}

int XLayer::GetLayerIndex() const
{
	return m_Index;
}

bool XLayer::SetLayerIndex( int index)
{
	m_Index = index;
	return true;
}

bool XLayer::IsShown()const
{
	return m_bShow;
}

bool XLayer::SetVisible(const bool& bshow)
{
	m_bShow = bshow;
	return true;
}
