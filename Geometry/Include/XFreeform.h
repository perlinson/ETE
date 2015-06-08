#ifndef _XFreeform_h_
#define _XFreeform_h_

#pragma once
#include "geometry.h"
#include "xgeombase.h"
#include "PNT.h"

#include "XCloud.h"
#include "Xtrimesh.h"

#include "..\..\NurbsFitting\NubSurStruct.h"

class AFX_EXT_CLASS  XFreeform :
	public XGeomBase
	{
public:
	XFreeform(void);
	~XFreeform(void);
	//void SetFreeform(int Unum, int Vnum, double *CPt)
	//{
	//	this->SetObjState(true);
	//	m_Unum=Unum;
	//	m_Vnum=Vnum;
	//}
void GetFreeformInfo(int &Unum,int Vnum );

	void SetUnit(const int& unit);
	void InitObject();


	virtual void Translate(const PNT& delt_t);
	virtual void Rotate(const Ti_Rotation& R);

	XTriMesh *pTriMesh;

protected:
	virtual void UpdateBndBox(void);
protected:
	int m_Unum,m_Vnum;
	};

#endif //_XFreeform_h_
