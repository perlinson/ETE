#pragma once

#include "stdafx.h"


class CNubSurStruct;
class De_BSCurve;

class AFX_CLASS_EXPORT CNubFitting
{
public:
	CNubFitting(void);
	~CNubFitting(void);

//////////////////////////////////////////////////////////
// User's interface access functions
// Operation interface for nurbs surface fitting to points
	
	// Coons surface is Coons=U+V-Corner Ferguson
	bool CreateCoonsSurface();

	bool SetBndforCrtCoonsSur(
			De_BSCurve uc0, 
			De_BSCurve uc1, 
			De_BSCurve vc0, 
			De_BSCurve vc1);

	bool SetUVNumforCoonsSur(
			int unum,	//>=8
			int vunm);	//>=8 at least
 
     
//////////////////////////////////////////////
// internal functions for implementation
// do not use it directly

	void CalcUniteKnot_even(
			De_BSCKnotArray &U,
			De_BSCKnotArray &V);

	void KnotVectorComp(
			De_BSCurve &cur,
			De_BSCKnotArray &UniteKnot);

	void KnotVectorComp_even(
			De_BSCurve &cur,
			De_BSCKnotArray &UniteKnot,
			int type);

	void ConstructFergusonNurbs(
			De_BSCKnotArray &UKnot,
			De_BSCKnotArray &VKnot,
			De_BSCurveArray &FergusonCurs);

/////////////////////////////////////////////////////
// data for fitting

	De_BSCurve Ucur0,Ucur1,Vcur0,Vcur1;
	De_BSCurve TmpUcur0,TmpUcur1,TmpVcur0,TmpVcur1;
	double star_nx,star_ny,star_nz,end_nx,end_ny,end_nz; 
	bool CurveDivide; 
	int	m_num_u;	//the number of u knot vector
	int	m_num_v;
	//CArray<double,double> UArr,VArr;

	//Coons surface
	CNubSurStruct CoonsNurbs;//for base surface

};
