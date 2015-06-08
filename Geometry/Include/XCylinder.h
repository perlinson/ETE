#ifndef _XCylinder_h_
#define _XCylinder_h_

#pragma once
#include "geometry.h"
#include "xgeombase.h"
#include "PNT.h"

class AFX_EXT_CLASS  XCylinder :
	public XGeomBase
	{
public:
	XCylinder(void);
	~XCylinder(void);
	void SetCylinder(float height, float radius)
	{
		m_Height = height;
		m_Radius = radius;
		this->SetObjState(true);
	}
	void GetCylinderInfo(PNT& AxisDir, PNT& AxisPnt, double & Radius);

	void SetUnit(const int& unit);
	void InitObject();
	//yk0805: find the intersections(if any) with a line (linePnt + t* lineDir)
	// -1----no intersection on the infinite cylinder wall,i.e.,line is parallel to cylinder
	// 0 ----both intersections extends the finite cylinder wall
	// 1----1 intersection,interPnt1, on the finite cylinder wall.( the other intersection point extends the wall)
	// 2----2 intersections,interPnt1 and interPnt2, on the finite cylinder wall
	int LineIntersection(const PNT & linePnt,const PNT & lineDir, PNT & interPnt1, PNT & interPnt2);

	void ComputeCylinder(int numFacets,
		float c[][3], float tc[][3], int faces[]);
		virtual void Translate(const PNT& delt_t);
		virtual void Rotate(const Ti_Rotation& R);
		 //distance Calculator writing for cylinder
friend int __cdecl EvaluateforCylinder(int m,		// number of points
						int n,		// number of parameters = 7
						double *const par, // the elements are x,y,z A,B,C,r
						double* rv,		// residual value,e.g. distance diviation
						double**g_pnt
						);
friend int __cdecl EvaluateforCylinder_Guass(int m,		// number of points
						int n,		// number of parameters = 7
						double *const par, // the elements are x,y,z A,B,C,r
						double* rv,		// residual value,e.g. distance diviation
						double**g_pnt
						);

//derivatives for distance Calculator
friend void __cdecl JacobianforCylinder(int m,	// number of points 
			  int n,		// number of parameters = 7
			  double *const  par,	  // the elements are x,y,z A,B,C,r
			  double*& rv,   // residual value,e.g. distance diviation
			  double **jac,
			  double **g_pnt
			  );

protected:
	virtual void UpdateBndBox(void);
protected:
	float m_Height;
	float m_Radius;
	};

#endif //_XCylinder_h_
