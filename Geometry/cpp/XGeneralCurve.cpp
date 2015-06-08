#include "stdafx.h"
#include "..\include\xgeneralcurve.h"

XGeneralCurve::XGeneralCurve(void)
{
}

XGeneralCurve::~XGeneralCurve(void)
{
}
const PNT& XGeneralCurve::GetStartPt()const
{
	return this->m_StartPt;
}
const PNT& XGeneralCurve::GetEndPt()const
{
	return this->m_EndPt;
}
void XGeneralCurve::SetStartPt(const PNT& pnt)
{
	this->m_StartPt = pnt;
}
void XGeneralCurve::SetEndPt(const PNT& pnt)
{
	m_EndPt = pnt;
}

void XGeneralCurve:: InitObject() 
{
}
void XGeneralCurve::SetUnit(const int& unit)
{
}
void XGeneralCurve::UpdateBndBox()
{
}
