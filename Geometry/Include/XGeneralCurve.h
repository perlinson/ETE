#pragma once
#include "xgeombase.h"

class AFX_EXT_CLASS XGeneralCurve :	public XGeomBase
{
public:
	XGeneralCurve(void);
	virtual ~XGeneralCurve(void);
	const PNT& GetStartPt()const;
	const PNT& GetEndPt()const;
	void SetStartPt(const PNT& pnt);
	void SetEndPt(const PNT& pnt);
	PNT m_StartPt;
	PNT m_EndPt;

	void UpdateBndBox();				
	virtual void SetUnit(const int& unit);
	virtual void InitObject();

protected:

};
