// NubSurStruct.h: interface for the CNubSurStruct class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUBSURSTRUCT_H__B473EE83_8485_11D3_AA04_0000E83E3BCC__INCLUDED_)
#define AFX_NUBSURSTRUCT_H__B473EE83_8485_11D3_AA04_0000E83E3BCC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <afxtempl.h>
#include "Re_Pnt3D.h"

class AFX_CLASS_EXPORT CNubSurStruct
{
public:
	CNubSurStruct();
	virtual ~CNubSurStruct();

	Re_Pnt3DArray CtrlPnt;			//Nurbs surface control point array
	Re_Pnt3DArray PrimPnt;			//Nurbs surface interpolation point array
	CArray<double,double> U_Knot;	//U knot array
	CArray<double,double> V_Knot;	//V knot array
	Re_Pnt3DArray TrimPnt;			//Trim curve convex point，

	int dgr_u;						//degree of u direction
	int dgr_v;						//degree of v direction

	int m;
	int n,i,j;

	//jjjj2008
	bool KnotOK;
	double star_nx,star_ny,star_nz,end_nx,end_ny,end_nz; //
	Re_Pnt3D BreakPnt; //
	Re_Pnt3DArray BreakPntArray;
	bool CurveDivide; //

	void Init();
	CNubSurStruct(const CNubSurStruct& );
	CNubSurStruct &operator =(CNubSurStruct);

	bool			CalPntZ(double &x,double &y,double &z,double & u,double & v);

	void           CalSufPnt(double &x,double &y,double &z,double u,double v);
	void           CalSufDu1(double &x,double &y,double &z,double u,double v);
	void           CalSufDv1(double &x,double &y,double &z,double u,double v);
	void           CalSufDu2(double &x,double &y,double &z,double u,double v);
	void           CalSufDv2(double &x,double &y,double &z,double u,double v);
	void           CalSufDuv(double &x,double &y,double &z,double u,double v);
	void           Normal(double &x,double &y,double &z,double u,double v);
	void		   Curvature(double &K_Gaussian,double &K_Average,double u,double v);
	double         BsBase(int i,int k,double ur,CArray<double,double> &KnotArray);
	double         BaseDv1(int i,int k,double ur,CArray<double,double> &KnotArray);
	double         BaseDv2(int i,int k,double ur,CArray<double,double> &KnotArray);
	void           Standardlize();
	void		   NormalFlip(void);
	void		   CalLineDt(double u,double v, double detau,double detav,\
							double &x,double &y,double &z);
	void		   CalLineDc(double u,double v, double detau,double detav,\
							double &x,double &y,double &z);

	BOOL GetCtrlPnt(int im, int in, double *x, double *y, double *z);
	BOOL SetCtrlPnt(int im, int in, double x, double y, double z);
	BOOL PntProject(double outpx,double outpy,double outpz,double i_u,double iv,double&uu,double& vv);

	bool PrjPnt(const Re_Pnt3D& pnt,Re_Pnt3D& prj_pnt,double& return_u,double& return_w,double &dist,bool& m_outpnt);
	
	bool ProjectPntToSurf(double, double, double,double, double,double&, double&,int&);//point project to surface and find the out boundary.

	void CalCtrlPnt();        //NURBS曲面反求控制顶点La
};

//typedef	 CArray<CNubSurStruct,CNubSurStruct&> NurbsSurfaceArray;

#endif // !defined(AFX_NUBSURSTRUCT_H__B473EE83_8485_11D3_AA04_0000E83E3BCC__INCLUDED_)
