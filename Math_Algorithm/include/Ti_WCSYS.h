// Ti_WCSYS.h: interface for the Ti_WCSYS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TI_WCSYS_H__6B6E934E_4FC1_4EB9_8E40_2E40F1D7EFC4__INCLUDED_)
#define AFX_TI_WCSYS_H__6B6E934E_4FC1_4EB9_8E40_2E40F1D7EFC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "pnt.h"

//class Ti_Point;

class  MATH_ALGORITHM_API  Ti_WCSYS  
{
public:
	//Store the info. of working csys relate to absolute csys
	double m_org[3];		//Coordinates of the wcsys' origin relative to acsys
	double m_x[3];			//Orientation of the wcsys' x axis relative to acsys
	double m_y[3];			//Orientation of the wcsys' y axis relative to acsys
	double m_z[3];			//Orientation of the wcsys' z axis relative to acsys

	double m_scale;			//the length of bounding box of the sketch 

public:
	
	Ti_WCSYS();
	virtual ~Ti_WCSYS();

	Ti_WCSYS(const Ti_WCSYS& a);
	Ti_WCSYS& operator =(const Ti_WCSYS& a);

	bool SetFromOrgXY(double,double,double,double,double,double,double,double,double);
	bool SetFromOrgZX(double ox,double oy,double oz,double zx,double zy,double zz,double xx,double xy,double xz);
	bool SetFromOrgXYZ(double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3);		
	
	bool Init();	

	// OPERATIONS
	void Shift(double x,double y,double z);			//Shift the working csys
	void Rotate(int iaxis,double angle);			//Rotate the working csys

	void Scale(PNT& pnt, double scale);	//Scale the point according to the working csys
	void Scale(double &x, double &y, double &z, double scale);

	void FromAbs3(double &x0, double &y0, double &z0);
	void FromAbsPP(PNT& pnt);
	void FromAbs(				 //traslate the coordinate from absolute to local system
				double,          //INPUT: x coordinate in absolute
				double,          //INPUT: y coordinate in absolute
				double,          //INPUT: z coordinate in absolute
				double&,         //OUTPUT: x coordinate in local system
				double&,         //OUTPUT: y coordinate in local system
				double&);        //OUTPUT: z coordiante in local system
	
	bool ToAbsolute(double& x,double& y,double& z);	                     //RETURN: no
	bool ToAbsoluteP(PNT& pnt, double& x, double& y, double& z); 
	bool ToAbsolutePP(PNT& pnt); 
	bool ToAbsolutePTP(PNT& pnt, PNT& outpnt);
};

#endif // !defined(AFX_TI_WCSYS_H__6B6E934E_4FC1_4EB9_8E40_2E40F1D7EFC4__INCLUDED_)
