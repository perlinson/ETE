#ifndef _RE_PNT3D_H
#define _RE_PNT3D_H

#include <afxtempl.h>
#include "Re_Vector.h"

 typedef enum{
	NONE=0,
	TRISUR,
	CURVES
} PTYPE;


class AFX_CLASS_EXPORT Re_PntAttr
{
public:
	Re_PntAttr();

public:
	PTYPE  m_type;     // Surface or Curves
	int    m_iSurCur;  // which Surface or which Curves
	int    m_iPatch;   // which patch in surface or which segment in curves
	double m_u;       // the argument in patch or in segment
	double m_v;

public:
	Re_PntAttr(const Re_PntAttr& a);
	Re_PntAttr &operator =(Re_PntAttr a);

public:
	virtual ~Re_PntAttr();
};
 typedef  CArray<Re_PntAttr,Re_PntAttr&>  Re_PntAttrArray;

class AFX_CLASS_EXPORT Re_Pnt3D
{
public:
	Re_Pnt3D();

public:
	double m_x;
	double m_y;
	double m_z;
	double m_nx;
	double m_ny;
	double m_nz;
	Re_PntAttr m_attr;// describe which surface,patch and line the point belong to 
	BOOL m_IsPicked;

public:
	Re_Pnt3D(const Re_Pnt3D& a);
	Re_Pnt3D operator =(Re_Pnt3D a);
	int operator ==(Re_Pnt3D a);
	void Init();
	

public:
	virtual ~Re_Pnt3D();
	void RotateBack(double,double,double);
	void RotateToXY(double,double,double);
	double DistantTo(Re_Pnt3D);
	BOOL DistantTo(Re_Pnt3D,Re_Pnt3D,double &,double &); 
	BOOL ProjectToPln(double,double,double,double,double,double,double,Re_Pnt3D &);

};

 typedef  CArray<Re_Pnt3D,Re_Pnt3D> Re_Pnt3DArray;   // points set


#endif

