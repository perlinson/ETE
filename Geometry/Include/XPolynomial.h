#ifndef _XPolynomial_h_
#define _XPolynomial_h_

#pragma once
#include "geometry.h"
#include "xgeombase.h"
#include "PNT.h"

#include "XCloud.h"
#include "Xtrimesh.h"

class AFX_EXT_CLASS  XPolynomial :
	public XGeomBase
	{
public:
	XPolynomial(void);
	~XPolynomial(void);
	void SetPolynomial(int ParaNum, int RootFlag, double *A)
	{
		this->SetObjState(true);
		m_ParaNum=ParaNum;
		m_RF=RootFlag;
		m_A=(double*)malloc(ParaNum*sizeof(double) );
		for(int i=0;i<ParaNum;i++)
		    m_A[i]=A[i];
	}
void GetPolynomialInfo(int & ParaNum,int & RootFlag, double A[] );

	void SetUnit(const int& unit);
	void InitObject();

	void ComputePolynomial( int numPts,	double ** g_pnt);

		virtual void Translate(const PNT& delt_t);
		virtual void Rotate(const Ti_Rotation& R);

	XTriMesh *pTriMesh;
	 

		 //distance Calculator writing for polynomial
friend int __cdecl EvaluateforPolynomial(int m,		// number of points
						int n,		// number of parameters 
						double *const par, // the elements
						double* rv,		// residual value,e.g. distance diviation
						double**g_pnt
						);

protected:
	virtual void UpdateBndBox(void);
protected:
	int m_ParaNum;
	int m_RF;
	double *m_A;
	};

#endif //_XPolynomial_h_
