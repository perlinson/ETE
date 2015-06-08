#include "StdAfx.h"
#include "de_bscurve.h"
#include "de_bsckont.h"
#include "de_Intplpnt.h"
#include "nubsurstruct.h"
#include "nubfitting.h"
#include <math.h>

CNubFitting::CNubFitting(void)
{
	m_num_u=8;
	m_num_v=8;
}

CNubFitting::~CNubFitting(void)
{
}

bool CNubFitting::SetUVNumforCoonsSur( int unum,	 int vunm)
{
	if (unum*vunm<=0) return false;
	m_num_u=unum;
	m_num_v=vunm;

	return true;
}

bool CNubFitting::SetBndforCrtCoonsSur( De_BSCurve uc0,   De_BSCurve uc1,  De_BSCurve vc0,   De_BSCurve vc1)
{
	Ucur0 = uc0;
	Ucur1 = uc1;
	Vcur0 = vc0;
	Vcur1 = vc1;

	Ucur0.ReCreateIntplPnts();
	Ucur1.ReCreateIntplPnts();
	Vcur0.ReCreateIntplPnts();
	Vcur1.ReCreateIntplPnts();

	return true;
}


bool CNubFitting::CreateCoonsSurface()
{
	int i,j;
	TmpUcur0.Init();TmpUcur0=Ucur0;
	TmpUcur1.Init();TmpUcur1=Ucur1;
	TmpVcur0.Init();TmpVcur0=Vcur0;
	TmpVcur1.Init();TmpVcur1=Vcur1;

	De_BSCKnotArray UKnot,VKnot;

	CalcUniteKnot_even(UKnot,VKnot);
	KnotVectorComp_even(TmpUcur0,UKnot,0);
	KnotVectorComp_even(TmpUcur1,UKnot,0);
	KnotVectorComp_even(TmpVcur0,VKnot,0);
	KnotVectorComp_even(TmpVcur1,VKnot,0);

	///////////// 4 boundary curves 
	// U curves
	De_BSCurve BlendCur;
	De_BSCurveArray UBlendCurs;
	De_IntplPnt pnt;
	De_BPnt     CPnt1,CPnt2;
	De_BSCKnot  knot;
	for (i=0;i<TmpVcur0.m_CtrlPntArray.GetSize();i++)
	{
		BlendCur.Init();
		//Interpolated points along U direction
		pnt.m_pt=TmpVcur0.m_CtrlPntArray[i];
		BlendCur.m_IntplPntArray.Add(pnt);
		pnt.m_pt=TmpVcur1.m_CtrlPntArray[i];
		BlendCur.m_IntplPntArray.Add(pnt);
		//Control points along U direction
		CPnt1=TmpVcur0.m_CtrlPntArray[i];
		CPnt2=TmpVcur1.m_CtrlPntArray[i];
		BlendCur.m_CtrlPntArray.Add(CPnt1);
		BlendCur.m_CtrlPntArray.Add(CPnt1*2./3.+CPnt2/3.);
		BlendCur.m_CtrlPntArray.Add(CPnt1/3.+CPnt2*2./3.);
		BlendCur.m_CtrlPntArray.Add(CPnt2);

		knot.m_u=0.;
		for(j=0;j<=2;j++)
			BlendCur.m_KnotArray.Add(knot);
		knot.m_u=1.;
		for(j=0;j<=2;j++)
			BlendCur.m_KnotArray.Add(knot);

		BlendCur.m_Degree=3;

		BlendCur.KnotStandardization();
		KnotVectorComp(BlendCur,UKnot);
		UBlendCurs.Add(BlendCur);
	}
	// V curves
	De_BSCurveArray VBlendCurs;
	for (i=0;i<TmpUcur0.m_CtrlPntArray.GetSize();i++)
	{
		BlendCur.Init();
		pnt.m_pt=TmpUcur0.m_CtrlPntArray[i];
		BlendCur.m_IntplPntArray.Add(pnt);
		pnt.m_pt=TmpUcur1.m_CtrlPntArray[i];
		BlendCur.m_IntplPntArray.Add(pnt);

		CPnt1=TmpUcur0.m_CtrlPntArray[i];
		CPnt2=TmpUcur1.m_CtrlPntArray[i];
		BlendCur.m_CtrlPntArray.Add(CPnt1);
		BlendCur.m_CtrlPntArray.Add(CPnt1*2./3.+CPnt2/3.);
		BlendCur.m_CtrlPntArray.Add(CPnt1/3.+CPnt2*2./3.);
		BlendCur.m_CtrlPntArray.Add(CPnt2);

		knot.m_u=0.;
		for(j=0;j<=2;j++)
			BlendCur.m_KnotArray.Add(knot);
		knot.m_u=1.;
		for(j=0;j<=2;j++)
			BlendCur.m_KnotArray.Add(knot);

		BlendCur.m_Degree=3;

		BlendCur.KnotStandardization();
		KnotVectorComp(BlendCur,VKnot);
		VBlendCurs.Add(BlendCur);
	}
	// Ferguson surface
	De_BSCurveArray FergusonCurs;
	ConstructFergusonNurbs(UKnot,VKnot,FergusonCurs);
	// Generate Coons surface
	//UArr.RemoveAll(); 
	//VArr.RemoveAll();
	CoonsNurbs.Init();
	CoonsNurbs.U_Knot.Add(UKnot[0].m_u);//4 repeated end knots
	for (i=0;i<UKnot.GetSize();i++)
		CoonsNurbs.U_Knot.Add(UKnot[i].m_u);
	CoonsNurbs.U_Knot.Add(UKnot[UKnot.GetSize()-1].m_u);
	CoonsNurbs.V_Knot.Add(VKnot[0].m_u);
	for (i=0;i<VKnot.GetSize();i++)
		CoonsNurbs.V_Knot.Add(VKnot[i].m_u);
	CoonsNurbs.V_Knot.Add(VKnot[VKnot.GetSize()-1].m_u);

	//for(i=0;i<CoonsNurbs.U_Knot.GetSize();i++)
	//	UArr.Add(CoonsNurbs.U_Knot[i]);
	//for(i=0;i<CoonsNurbs.V_Knot.GetSize();i++)
	//	VArr.Add(CoonsNurbs.V_Knot[i]);

	CoonsNurbs.m=VBlendCurs.GetSize();
	CoonsNurbs.n=UBlendCurs.GetSize();

	for (i=0;i<CoonsNurbs.n;i++)
	{
		for (int j=0;j<CoonsNurbs.m;j++)
		{
			Re_Pnt3D CPnt;
			CPnt.m_x=UBlendCurs[i].m_CtrlPntArray[j].m_x+VBlendCurs[j].m_CtrlPntArray[i].m_x-FergusonCurs[j].m_CtrlPntArray[i].m_x;
			CPnt.m_y=UBlendCurs[i].m_CtrlPntArray[j].m_y+VBlendCurs[j].m_CtrlPntArray[i].m_y-FergusonCurs[j].m_CtrlPntArray[i].m_y;
			CPnt.m_z=UBlendCurs[i].m_CtrlPntArray[j].m_z+VBlendCurs[j].m_CtrlPntArray[i].m_z-FergusonCurs[j].m_CtrlPntArray[i].m_z;
			CoonsNurbs.CtrlPnt.Add(CPnt);
		}
	}

	return true;
}

void CNubFitting::CalcUniteKnot_even(De_BSCKnotArray &U,De_BSCKnotArray &V)
{
	int i;
	De_BSCKnot TmpKnot;
	int num_u,num_v;
	num_u=m_num_u-4;
	num_v=m_num_v-4;
	TmpKnot.m_u=0.0;
	U.Add(TmpKnot);U.Add(TmpKnot);U.Add(TmpKnot);
	V.Add(TmpKnot);V.Add(TmpKnot);V.Add(TmpKnot);
	for (i=0;i<num_u;i++)
	{
		TmpKnot.m_u=(double)(i+1)/(double)(num_u+1);
		U.Add(TmpKnot);
	}
	for (i=0;i<num_v;i++)
	{
		TmpKnot.m_u=(double)(i+1)/(double)(num_v+1);
		V.Add(TmpKnot);
	}
	TmpKnot.m_u=1.0;
	U.Add(TmpKnot);U.Add(TmpKnot);U.Add(TmpKnot);
	V.Add(TmpKnot);V.Add(TmpKnot);V.Add(TmpKnot);
}

void CNubFitting::KnotVectorComp(De_BSCurve &cur,De_BSCKnotArray &UniteKnot)
{
	double er2=1e-5;

	for (int i=3;i<UniteKnot.GetSize()-3;i++)
	{
		for(int j=3;j<cur.m_KnotArray.GetSize()-2;j++)
		{
			double delt=UniteKnot[i].m_u-cur.m_KnotArray[j].m_u;
			if (fabs(delt)<er2)
			{
				int m1=UniteKnot[i].m_multi;
				int m2=cur.m_KnotArray[j].m_multi;
				for (int k=0;k<m2;k++)
					cur.m_KnotArray[j+k]=UniteKnot[i];
				if (m1>m2)
					cur.InsertKnot(j-1,m2,m1-m2,UniteKnot[i].m_u);
				i=i+m1-1;
				break;
			}
			else if (delt<0.0)
			{
				int m1=UniteKnot[i].m_multi;
				cur.InsertKnot(j-1,0,m1,UniteKnot[i].m_u);
				i=i+m1-1;
				break;
			}
		}
	}
}

void CNubFitting::KnotVectorComp_even(De_BSCurve &cur,De_BSCKnotArray &UniteKnot,int type)
{
               
	De_BSCurve newcur;
	De_IntplPnt pnt;
	newcur.m_SweepCurIndex=cur.m_SweepCurIndex;
	cur.KnotStandardization();
	if (type==0)
	{
		for (int i=2;i<UniteKnot.GetSize()-2;i++)
		{
			double x,y,z;
			cur.CalPnt(UniteKnot[i].m_u,x,y,z);
			pnt.m_pt.m_x=x;
			pnt.m_pt.m_y=y;
			pnt.m_pt.m_z=z;
			newcur.m_IntplPntArray.Add(pnt);
		}
		newcur.star_nx=-cur.m_IntplPntArray[0].m_pt.m_nx;
		newcur.star_ny=-cur.m_IntplPntArray[0].m_pt.m_ny;
		newcur.star_nz=-cur.m_IntplPntArray[0].m_pt.m_nz;
		newcur.end_nx=cur.m_IntplPntArray[cur.m_IntplPntArray.GetSize()-1].m_pt.m_nx;
		newcur.end_ny=cur.m_IntplPntArray[cur.m_IntplPntArray.GetSize()-1].m_pt.m_ny;
		newcur.end_nz=cur.m_IntplPntArray[cur.m_IntplPntArray.GetSize()-1].m_pt.m_nz;
		newcur.CurveDivide=true;
		newcur.CalCtrlPntNew();
		newcur.CurveDivide=false;
		cur.Init();
		cur=newcur;
	}
	else if (type==1)
	{
	}
	cur.m_SweepCurIndex=newcur.m_SweepCurIndex;
}

void CNubFitting::ConstructFergusonNurbs(De_BSCKnotArray &UKnot,De_BSCKnotArray &VKnot,De_BSCurveArray &FergusonCurs) 
{
    int i,j;
	FergusonCurs.RemoveAll();
	// 4 boundary curves
	De_BSCurve UTmpCur0,UTmpCur1,BlendCur;
	De_IntplPnt pnt;
	De_BPnt CPnt1,CPnt2,CPnt3,CPnt4;  
	////U Direction
	UTmpCur0.Init();
	UTmpCur1.Init();
	//Elevation along U direction
	//Interpolated points along U0 and U1 curve
	pnt=TmpUcur0.m_IntplPntArray[0];
	UTmpCur0.m_IntplPntArray.Add(pnt);
	pnt=TmpUcur0.m_IntplPntArray[TmpUcur0.m_IntplPntArray.GetSize()-1];
	UTmpCur0.m_IntplPntArray.Add(pnt);

	pnt=TmpUcur1.m_IntplPntArray[0];
	UTmpCur1.m_IntplPntArray.Add(pnt);
	pnt=TmpUcur1.m_IntplPntArray[TmpUcur0.m_IntplPntArray.GetSize()-1];
	UTmpCur1.m_IntplPntArray.Add(pnt);
	//Control points along U0 and U1 curve
	CPnt1=TmpUcur0.m_CtrlPntArray[0];
	CPnt2=TmpUcur0.m_CtrlPntArray[TmpUcur0.m_CtrlPntArray.GetSize()-1];
	UTmpCur0.m_CtrlPntArray.Add(CPnt1);
	UTmpCur0.m_CtrlPntArray.Add(CPnt1 *2./3.+CPnt2 /3.);
	UTmpCur0.m_CtrlPntArray.Add(CPnt1 /3.+CPnt2 *2./3.);
	UTmpCur0.m_CtrlPntArray.Add(CPnt2);
	CPnt3=TmpUcur1.m_CtrlPntArray[0];
	CPnt4=TmpUcur1.m_CtrlPntArray[TmpUcur1.m_CtrlPntArray.GetSize()-1];
	UTmpCur1.m_CtrlPntArray.Add(CPnt3);
	UTmpCur1.m_CtrlPntArray.Add(CPnt3 *2./3.+CPnt4 /3.);
	UTmpCur1.m_CtrlPntArray.Add(CPnt3 /3.+CPnt4 *2./3.);
	UTmpCur1.m_CtrlPntArray.Add(CPnt4);
	//Knot vector along U0 and U1 curve
	De_BSCKnot knot; 
	knot.m_u=0.;
	for(i=0;i<=2;i++)
	{
		UTmpCur0.m_KnotArray.Add(knot);
		UTmpCur1.m_KnotArray.Add(knot);
	}
	knot.m_u=1.;
	for(i=0;i<=2;i++)
	{	
		UTmpCur0.m_KnotArray.Add(knot);
		UTmpCur1.m_KnotArray.Add(knot);
	}
	//Degree of U0 and U1 curve
	UTmpCur0.m_Degree=3;
	UTmpCur1.m_Degree=3;
	//Knot insertion along U0 and U1 curve
	KnotVectorComp(UTmpCur0,UKnot);
	KnotVectorComp(UTmpCur1,UKnot);
	//KnotVectorComp(UTmpCur0,TmpUcur0.m_KnotArray);
	//KnotVectorComp(UTmpCur1,TmpUcur0.m_KnotArray);
	//V Direction
	for(i=0;i<UTmpCur0.m_CtrlPntArray.GetSize();i++)
	{
		BlendCur.Init();
		//Elevation along v direction
		//Interpolated points along v direction
		pnt.m_pt=UTmpCur0.m_CtrlPntArray[i];
		for(j=0;j<=2;j++)
			BlendCur.m_IntplPntArray.Add(pnt);
		pnt.m_pt=UTmpCur1.m_CtrlPntArray[i];
		for(j=0;j<=2;j++)
			BlendCur.m_IntplPntArray.Add(pnt);
		//Control points along v direction
		CPnt1=UTmpCur0.m_CtrlPntArray[i];
		CPnt2=UTmpCur1.m_CtrlPntArray[i];
		BlendCur.m_CtrlPntArray.Add(CPnt1);
		BlendCur.m_CtrlPntArray.Add(CPnt1*2./3.+CPnt2/3.);
		BlendCur.m_CtrlPntArray.Add(CPnt1/3.+CPnt2*2./3.);
		BlendCur.m_CtrlPntArray.Add(CPnt2);
		//Kont vector along v direction
		knot.m_u=0.;
		for(j=0;j<=2;j++)
			BlendCur.m_KnotArray.Add(knot);
		knot.m_u=1.;
		for(j=0;j<=2;j++)
			BlendCur.m_KnotArray.Add(knot);
		//Degree of v direction
		BlendCur.m_Degree=3;
		//Knot insertion along v direction
		KnotVectorComp(BlendCur,VKnot);
		//KnotVectorComp(BlendCur,TmpVcur0.m_KnotArray);
		FergusonCurs.Add(BlendCur);
	}
}
