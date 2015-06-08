#pragma once
#include "geometry.h"
#include "xgeombase.h"

class AFX_EXT_CLASS XAnnotation :	public XGeomBase
{
public:
	XAnnotation(void);
	~XAnnotation(void);

	CString		m_sAnnotation;
	PNT			m_Anchor;


	void 		SetAnchorPnt(PNT pnt);
	void 		SetUnit(const int& unit);
	void 		InitObject() ;
	void 		Translate(const PNT& delt_t);
	void 		Rotate(const Ti_Rotation& R) ;
protected:
	void		UpdateBndBox();
};
