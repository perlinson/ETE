#include "stdafx.h"
#include "xannotation.h"

XAnnotation::XAnnotation(void)
{
	m_ObjectType = OBJECT_ANNOTATION;
	m_Name = "Annotation";// check if it's correct
	m_Color	= BLACK_COLOR;
	m_bShow = true;	
	m_Annotation = false;
	m_sAnnotation=_T("TTEST!!!");
}

XAnnotation::~XAnnotation(void)
{
}

void XAnnotation::SetAnchorPnt(PNT pnt)
{
	m_Anchor = pnt;
}

void XAnnotation::SetUnit(const int& unit)
{
}

void XAnnotation::InitObject()
{
}

void XAnnotation::Translate(const PNT& delt_t)
{
}

void XAnnotation::Rotate(const Ti_Rotation& R)
{
}

void	XAnnotation::UpdateBndBox()
{
}
