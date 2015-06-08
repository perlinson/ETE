// De_BSCurve.cpp: implementation of the De_BSCurve class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "De_BSCurve.h"
#include "Re_Vector.h"
#include "gl\gl.h"
#include "gl\glu.h"
#include "math.h"
#include "De_BPnt.h"
#include "De_IntplPnt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TINY	1.0e-20

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

De_BSCurve::De_BSCurve()
{
	m_KnotArray.SetSize(0);
	m_CtrlPntArray.SetSize(0);
	m_IntplPntArray.SetSize(0);
	m_DrawData.SetSize(0);
	m_Degree = 3;
	m_Close = 0;//0, open; 1,close
	m_StartCond = 0;//0, tangent; 1, hold; 2, free
	m_EndCond = 0;//0, tangent; 1, hold; 2, free
	m_iEndConstrain=0;//0,two end 1,star constrain 2,end constrain
	m_IsPicked = 0;
	m_IsBlanked = 0;
	m_IsPlane = 0;
	m_bReverse=0;
	m_BndNum=0;
	m_SweepCurIndex=-1;
	for(int i=0;i<3;i++) {Normal_star[i]=0;Normal_end[i]=0;}
}

De_BSCurve::~De_BSCurve()
{
	m_KnotArray.RemoveAll();
	m_KnotArray.FreeExtra();
	m_CtrlPntArray.RemoveAll();
	m_CtrlPntArray.FreeExtra();
	m_IntplPntArray.RemoveAll();
	m_IntplPntArray.FreeExtra();
	m_DrawData.RemoveAll();
}

void De_BSCurve::Init()
{
	m_KnotArray.SetSize(0);
	m_CtrlPntArray.SetSize(0);
	m_IntplPntArray.SetSize(0);
	m_DrawData.RemoveAll();
	m_BndNum=0;
	m_iEndConstrain=0;
	for(int i=0;i<3;i++) {Normal_star[i]=0;Normal_end[i]=0;}
	m_IsPlane = 0;
}

De_BSCurve::De_BSCurve(const De_BSCurve& b)
{
	int i;
	De_BSCKnot		kt;
	De_BPnt			pt;
	De_IntplPnt	ipt;
	for(i=0;i<b.m_KnotArray.GetSize();i++)
	{
		kt=b.m_KnotArray[i];
		m_KnotArray.Add(kt);
	}
	for(i=0;i<b.m_CtrlPntArray.GetSize();i++)
	{
		pt=b.m_CtrlPntArray[i];
		m_CtrlPntArray.Add(pt);
	}
	for(i=0;i<b.m_IntplPntArray.GetSize();i++)
	{
		ipt=b.m_IntplPntArray[i];
		m_IntplPntArray.Add(ipt);
	}
	for(i=0;i<b.m_DrawData.GetSize();i++)
		m_DrawData.Add(b.m_DrawData[i]);

	m_D1CtrlPntArray.SetSize(b.m_D1CtrlPntArray.GetSize());
	for(i=0;i<b.m_D1CtrlPntArray.GetSize();i++)
		m_D1CtrlPntArray[i]=b.m_D1CtrlPntArray[i];

	m_D2CtrlPntArray.SetSize(b.m_D2CtrlPntArray.GetSize());
	for(i=0;i<b.m_D2CtrlPntArray.GetSize();i++)
		m_D2CtrlPntArray[i]=b.m_D2CtrlPntArray[i];

	m_D3CtrlPntArray.SetSize(b.m_D3CtrlPntArray.GetSize());
	for(i=0;i<b.m_D3CtrlPntArray.GetSize();i++)
		m_D3CtrlPntArray[i]=b.m_D3CtrlPntArray[i];

	m_Degree = b.m_Degree;
	m_Close = b.m_Close;//0, open; 1,close
	m_StartCond = b.m_StartCond;//0, tangent; 1, hold; 2, free
	m_EndCond = b.m_EndCond;//0, tangent; 1, hold; 2, free
	m_IsPicked =b.m_IsPicked;
	m_IsBlanked=b.m_IsBlanked;
	m_IsPlane = b.m_IsPlane;
	m_PVector = b.m_PVector;
	m_ColorIndex=b.m_ColorIndex;
	m_BndNum=b.m_BndNum;
	m_SweepCurIndex=b.m_SweepCurIndex;
	m_iEndConstrain=b.m_iEndConstrain;
	for(i=0;i<3;i++) {Normal_star[i]=b.Normal_star[i];Normal_end[i]=b.Normal_end[i];}
}

De_BSCurve &De_BSCurve::operator =(De_BSCurve b)
{
	int i;
	De_BSCKnot		kt;
	De_BPnt			pt;
	De_IntplPnt	ipt;
	m_KnotArray.RemoveAll();
	m_CtrlPntArray.RemoveAll();
	m_IntplPntArray.RemoveAll();
	m_DrawData.RemoveAll();
	for(i=0;i<b.m_KnotArray.GetSize();i++)
	{
		kt=b.m_KnotArray[i];
		m_KnotArray.Add(kt);
	}
	for(i=0;i<b.m_CtrlPntArray.GetSize();i++)
	{
		pt=b.m_CtrlPntArray[i];
		m_CtrlPntArray.Add(pt);
	}
	for(i=0;i<b.m_IntplPntArray.GetSize();i++)
	{
		ipt=b.m_IntplPntArray[i];
		m_IntplPntArray.Add(ipt);
	}
	for(i=0;i<b.m_DrawData.GetSize();i++)
		m_DrawData.Add(b.m_DrawData[i]);

	m_D1CtrlPntArray.SetSize(b.m_D1CtrlPntArray.GetSize());
	for(i=0;i<b.m_D1CtrlPntArray.GetSize();i++)
		m_D1CtrlPntArray[i]=b.m_D1CtrlPntArray[i];

	m_D2CtrlPntArray.SetSize(b.m_D2CtrlPntArray.GetSize());
	for(i=0;i<b.m_D2CtrlPntArray.GetSize();i++)
		m_D2CtrlPntArray[i]=b.m_D2CtrlPntArray[i];

	m_D3CtrlPntArray.SetSize(b.m_D3CtrlPntArray.GetSize());
	for(i=0;i<b.m_D3CtrlPntArray.GetSize();i++)
		m_D3CtrlPntArray[i]=b.m_D3CtrlPntArray[i];

	m_Degree = b.m_Degree;
	m_Close = b.m_Close;//0, open; 1,close
	m_StartCond = b.m_StartCond;//0, tangent; 1, hold; 2, free
	m_EndCond = b.m_EndCond;//0, tangent; 1, hold; 2, free
	m_iEndConstrain=b.m_iEndConstrain;//0, two end 1, star 2,end
	m_IsPicked =b.m_IsPicked;
	m_IsBlanked =b.m_IsBlanked;
	m_IsPlane = b.m_IsPlane;
	m_PVector = b.m_PVector;
	m_ColorIndex=b.m_ColorIndex;
	m_BndNum=b.m_BndNum;
	m_SweepCurIndex=b.m_SweepCurIndex;
	for(i=0;i<3;i++) {Normal_star[i]=b.Normal_star[i];Normal_end[i]=b.Normal_end[i];}
	return (*this);
}
//////////////////////////////////////////////////////////////
//	有重节点BSC的分段
/////////////////////////////////////////////////////////////
int 	De_BSCurve::FindInter(int m_Frm,int& m_End)
{
	int KnotSize = m_KnotArray.GetSize();
	if(m_Frm >= KnotSize-1) 
	{
		m_End = m_Frm;
		return 0;
	}
	for(int i = m_Frm+1;i < KnotSize;i++)
	{
		if(m_KnotArray[i].m_multi == 2)
		{
			m_End = i;
			return 1;
		}
		else if(m_KnotArray[i].m_multi == 3)
		{
			m_End = i;
			return 2;
		}
		else
			;
	}
	m_End = KnotSize-1;			//close curve
	return 3;
}
////////////////////////////////////////////////////////////////
//	累加弦长法
///////////////////////////////////////////////////////////////
BOOL	De_BSCurve::CalKnot()
{
	De_BSCKnot kt;
	De_IntplPnt ipt;
	int i;

	kt.m_u=0.0;
	kt.m_type=1;
	m_KnotArray.Add(kt);
	kt.m_type=0;
	//
	if(m_Close==1)
	{
		m_IntplPntArray.Add(m_IntplPntArray.GetAt(0));
	}
    int ipSize=m_IntplPntArray.GetSize();
	if(m_Close==0)
	{
		if(m_IntplPntArray[0].m_multi==2 || m_IntplPntArray[ipSize-1].m_multi==2)
			return FALSE;
	}
	else
	{
		if(m_IntplPntArray[0].m_multi==2)
		{
			i=0;
			while(m_IntplPntArray[i].m_multi==2)
			{
				ipt=m_IntplPntArray[i];
				m_IntplPntArray.RemoveAt(0);
				m_IntplPntArray.Add(ipt);
				i++;
				if(i>=ipSize-1)
					return FALSE;
			}
		}
		if(m_IntplPntArray[ipSize-1].m_multi==2)
		{
			i=ipSize-1;
			while(m_IntplPntArray[i].m_multi==2)
			{
				ipt=m_IntplPntArray[i];
				m_IntplPntArray.RemoveAt(ipSize-1);
				m_IntplPntArray.InsertAt(0,ipt);
				i--;
				if(i<=0)
					return FALSE;
			}
		}
	}
	//
	for(i=0;i<m_IntplPntArray.GetSize()-1;i++)
	{
		kt.m_u=m_KnotArray[i].m_u+
			m_IntplPntArray[i].m_pt.Dst2Pnt(m_IntplPntArray[i+1].m_pt);
		kt.m_multi=m_IntplPntArray[i+1].m_multi;
		m_KnotArray.Add(kt);
	}
	
	if(m_Close==0)
	{
		m_KnotArray[m_KnotArray.GetSize()-1].m_type=1;
		m_KnotArray[m_KnotArray.GetSize()-1].m_multi=3;
		m_KnotArray[0].m_multi=3;
		m_KnotArray[0].m_type=1;
	}
	return TRUE;
}

///////////////////////////////////////////////////
//  计算端条件
//  m_StartEnd  0,start;  1,end
//////////////////////////////////////////////////
void	De_BSCurve::EndConstrain(int m_FrmCond,int m_EndCond,int m_Frm,int m_End,
								 double &f1,double &f2,double &f3,double &f4
								 ,double &e1,double &e2,double &e3,double &e4)
{
	double h1,h2,h3;
	h1=m_KnotArray[m_Frm+1].m_u-m_KnotArray[m_Frm+0].m_u;
	h2=m_KnotArray[m_Frm+2].m_u-m_KnotArray[m_Frm+1].m_u;
	h3=m_KnotArray[m_Frm+3].m_u-m_KnotArray[m_Frm+2].m_u;
	switch(m_FrmCond)
	{
	case 0:
		f1 = -3.0/h1;
		f2 = 3.0/h1;
		f3 = 0.0;
		f4 = 0.0;
		break;
	case 2:
		f1 = h1+h2;
		f2 = -(2.0*h1+h2);
		f3 = h1;
		f4 = 0.0;
		break;
	case 1:
		f1 = 1.0/(h1*h1);
		f2 = -f1 - 1.0/(h1*(h1+h2)) - 1.0/((h1+h2)*(h1+h2));
		f3 = 1.0/(h1*(h1+h2)) + 1.0/((h1+h2)*(h1+h2)) + 1.0/((h1+h2)*(h1+h2+h3));
		f4 = -1.0/((h1+h2)*(h1+h2+h3));
		break;
	default:
		//error
		break;
	}
	h1=m_KnotArray[m_End].m_u-m_KnotArray[m_End-1].m_u;
	h2=m_KnotArray[m_End-1].m_u-m_KnotArray[m_End-2].m_u;
	h3=m_KnotArray[m_End-2].m_u-m_KnotArray[m_End-3].m_u;
	switch(m_EndCond)
	{
	case 0:
		e1 = 3.0/h1;
		e2 = -3.0/h1;
		e3 = 0.0;
		e4 = 0.0;
		break;
	case 2:
		e1 = h1+h2;
		e2 = -(2.0*h1+h2);
		e3 = h1;
		e4 = 0.0;
		break;
	case 1:
		e1 = 1.0/(h1*h1);
		e2 = -1.0/(h1*h1) - 1.0/(h1*(h1+h2)) - 1.0/((h1+h2)*(h1+h2));
		e3 = 1.0/(h1*(h1+h2)) + 1.0/((h1+h2)*(h1+h2)) + 1.0/((h1+h2)*(h1+h2+h3));
		e4 = -1.0/((h1+h2)*(h1+h2+h3));
		break;
	default:
		//error
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////
//	计算基序列
//	m_No 参数区间标号 [t[m_No],t[m_No+1]]
//	m_u  参数值
//	m_b  基函数序列值
///////////////////////////////////////////////////////////////////////////////////
BOOL	De_BSCurve::CalBase(int Degree,int m_No,double m_u,double m_b[])
{
	double *siga_r=new double[Degree+2];
	double *siga_l=new double[Degree+2];
	double term,save;
	int i,j;
	
	m_b[0]=0.0;
	m_b[1]=1.0;
	siga_r[0]=0.0;
	siga_l[0]=0.0;

	for(j=1;j<=Degree;j++)
	{
		siga_r[j] = m_KnotArray[m_No+j].m_u - m_u;
		siga_l[j] = m_u - m_KnotArray[m_No-j+1].m_u;
		save = 0.0;
		for(i=1;i<=j;i++)
		{
			term = m_b[i]/(siga_r[i]+siga_l[j+1-i]);
			m_b[i] = save + siga_r[i]*term;
			save = siga_l[j+1-i]*term;
		}
		m_b[j+1] = save;
	}
	delete []siga_r;
	delete []siga_l;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////
//	追赶法：用于解三对角方程组
//////////////////////////////////////////////////////////////////////////////////
BOOL tridag(double a[],double b[],double c[],double r[],double u[],unsigned long n)
{
	unsigned long j;
	double bet;

	double *gam=new double[n+1];
	if(b[1]==0.0) return FALSE;
	u[1]=r[1]/(bet=b[1]);
	for(j=2;j<=n;j++)
	{
		gam[j]=c[j-1]/bet;
		bet=b[j]-a[j]*gam[j];
		if(bet==0.0) return FALSE;
		u[j]=(r[j]-a[j]*u[j-1])/bet;
	}
	for(j=(n-1);j>=1;j--)
		u[j]-=gam[j+1]*u[j+1];
	delete gam;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////
//	反求直线段控制顶点
///////////////////////////////////////////////////////////////////////////
void	De_BSCurve::CalLineSec(int FrmPnt,int ToPnt,int FrmCond,int ToCond)
{
	double *a=new double[5];
	double *b=new double[5];
	double *c=new double[5];
	double *ux=new double[5];
	double *uy=new double[5];
	double *uz=new double[5];
	double *r=new double[5];

	double h=m_KnotArray[ToPnt].m_u-m_KnotArray[FrmPnt].m_u;

	a[0]=0.0;		b[0]=0.0;		c[0]=0.0;
	a[1]=0.0;		b[1]=1.0;		c[1]=0.0;
	a[2]=-3.0/h;	b[2]=3.0/h;		c[2]=0.0;
	a[3]=0.0;		b[3]=-3.0/h;	c[3]=3.0/h;
	a[4]=0.0;		b[4]=1.0;		c[4]=0.0;
		
	Re_Vector v;
	v.m_x=m_IntplPntArray[ToPnt].m_pt.m_x-m_IntplPntArray[FrmPnt].m_pt.m_x;
	v.m_y=m_IntplPntArray[ToPnt].m_pt.m_y-m_IntplPntArray[FrmPnt].m_pt.m_y;
	v.m_z=m_IntplPntArray[ToPnt].m_pt.m_z-m_IntplPntArray[FrmPnt].m_pt.m_z;
	v.Normal();

	r[0]=0.0;
	r[2]=v.m_x;
	r[1]=m_IntplPntArray[FrmPnt].m_pt.m_x;
	r[4]=m_IntplPntArray[FrmPnt+1].m_pt.m_x;
	r[3]=v.m_x;
	tridag(a,b,c,r,ux,4);  //x
	r[0]=0.0;
	r[2]=v.m_y;
	r[1]=m_IntplPntArray[FrmPnt].m_pt.m_y;
	r[4]=m_IntplPntArray[FrmPnt+1].m_pt.m_y;
	r[3]=v.m_y;
	tridag(a,b,c,r,uy,4);  //y
	r[0]=0.0;
	r[2]=v.m_z;
	r[1]=m_IntplPntArray[FrmPnt].m_pt.m_z;
	r[4]=m_IntplPntArray[FrmPnt+1].m_pt.m_z;
	r[3]=v.m_z;
	tridag(a,b,c,r,uz,4);  //z
	De_BPnt tt;
	tt.m_x=ux[2];
	tt.m_y=uy[2];
	tt.m_z=uz[2];
	m_CtrlPntArray.Add(tt);
	tt.m_x=ux[3];
	tt.m_y=uy[3];
	tt.m_z=uz[3];
	m_CtrlPntArray.Add(tt);
	delete []a;
	delete []b;
	delete []c;
	delete []ux;
	delete []uy;
	delete []uz;
	delete []r;
}
////////////////////////////////////////////////////////////////////////////
//	交换两浮点数
////////////////////////////////////////////////////////////////////////////
void	swap(double &d1,double &d2)
{
	double t;
	t=d1;
	d1=d2;
	d2=t;
}

///////////////////////////////////////////////////////////////////////////
//	
//////////////////////////////////////////////////////////////////////////
void	De_BSCurve::CalCurSec(int FrmPnt,int ToPnt,int m_FrmCond,int m_ToCond)
{
	double *a=new double[ToPnt-FrmPnt+4];
	double *b=new double[ToPnt-FrmPnt+4];
	double *c=new double[ToPnt-FrmPnt+4];
	double *ux=new double[ToPnt-FrmPnt+4];
	double *uy=new double[ToPnt-FrmPnt+4];
	double *uz=new double[ToPnt-FrmPnt+4];
	double *r=new double[ToPnt-FrmPnt+4];
	double *base=new double[m_Degree+2];
	int i;
	double f1,f2,f3,f4;
	double e1,e2,e3,e4;

	EndConstrain(m_FrmCond,m_ToCond,FrmPnt,ToPnt,f1,f2,f3,f4,e1,e2,e3,e4);


	a[0]=0.0;
	a[1]=0.0;
	a[2]=f1;
	a[ToPnt-FrmPnt+2]=e3;
	a[ToPnt-FrmPnt+3]=0.0;

	b[0]=0.0;
	b[1]=1.0;
	b[2]=f2;
	b[ToPnt-FrmPnt+2]=e2;
	b[ToPnt-FrmPnt+3]=1.0;

	c[0]=0.0;
	c[1]=0.0;
	c[2]=f3;
	c[ToPnt-FrmPnt+2]=e1;
	c[ToPnt-FrmPnt+3]=0.0;

	for(i=3;i<ToPnt-FrmPnt+2;i++)
	{
		CalBase(m_Degree,FrmPnt+i-2,m_KnotArray[FrmPnt+i-2].m_u,base);
		a[i]=base[1];
		b[i]=base[2];
		c[i]=base[3];
	}

	Re_Vector v,vv;
	double p1,p2;
	if(m_FrmCond==0)
	{
		if(m_KnotArray[FrmPnt].m_multi==2)
		{
			v.m_x=m_IntplPntArray[FrmPnt].m_pt.m_x-m_IntplPntArray[FrmPnt-3].m_pt.m_x;
			v.m_y=m_IntplPntArray[FrmPnt].m_pt.m_y-m_IntplPntArray[FrmPnt-3].m_pt.m_y;
			v.m_z=m_IntplPntArray[FrmPnt].m_pt.m_z-m_IntplPntArray[FrmPnt-3].m_pt.m_z;
		}
		else//
		{
			if(!CurveDivide||(2 == m_iEndConstrain&&true == CurveDivide))
			{                                                                            //end of the curve has been imposed by the tangent constain but the curve start hasn't
				p1=(m_KnotArray[FrmPnt+1].m_u+m_KnotArray[FrmPnt+2].m_u-
					2*m_KnotArray[FrmPnt].m_u)/(m_KnotArray[FrmPnt+1].m_u-
					m_KnotArray[FrmPnt].m_u);
				p2=(m_KnotArray[FrmPnt+1].m_u-m_KnotArray[FrmPnt].m_u)/
					(m_KnotArray[FrmPnt+2].m_u-m_KnotArray[FrmPnt+1].m_u);

				v.m_x=p1*(m_IntplPntArray[FrmPnt+1].m_pt.m_x-m_IntplPntArray[FrmPnt].m_pt.m_x)-
					p2*(m_IntplPntArray[FrmPnt+2].m_pt.m_x-m_IntplPntArray[FrmPnt+1].m_pt.m_x);
				v.m_y=p1*(m_IntplPntArray[FrmPnt+1].m_pt.m_y-m_IntplPntArray[FrmPnt].m_pt.m_y)-
					p2*(m_IntplPntArray[FrmPnt+2].m_pt.m_y-m_IntplPntArray[FrmPnt+1].m_pt.m_y);
				v.m_z=p1*(m_IntplPntArray[FrmPnt+1].m_pt.m_z-m_IntplPntArray[FrmPnt].m_pt.m_z)-
					p2*(m_IntplPntArray[FrmPnt+2].m_pt.m_z-m_IntplPntArray[FrmPnt+1].m_pt.m_z);
			}
			else 
			{
				if(0 == m_iEndConstrain||1 == m_iEndConstrain)
				{
					v.m_x=star_nx;
					v.m_y=star_ny;
					v.m_z=star_nz;
				}
			}
			v.Normal();
		}
	}
	if(m_ToCond==0)
	{
		if(m_KnotArray[ToPnt].m_multi==2)
		{
			vv.m_x=-(m_IntplPntArray[ToPnt+3].m_pt.m_x-m_IntplPntArray[ToPnt].m_pt.m_x);
			vv.m_y=-(m_IntplPntArray[ToPnt+3].m_pt.m_y-m_IntplPntArray[ToPnt].m_pt.m_y);
			vv.m_z=-(m_IntplPntArray[ToPnt+3].m_pt.m_z-m_IntplPntArray[ToPnt].m_pt.m_z);
		}
		else
		{
			if(!CurveDivide||(1 == m_iEndConstrain&&true == CurveDivide)) //
			{                                                                           //star of the curve has been imposed by the tangent constain but the curve end hasn't
				p1=(m_KnotArray[ToPnt-1].m_u+m_KnotArray[ToPnt-2].m_u-
					2*m_KnotArray[ToPnt].m_u)/(m_KnotArray[ToPnt-1].m_u-
					m_KnotArray[ToPnt].m_u);
				p2=(m_KnotArray[ToPnt-1].m_u-m_KnotArray[ToPnt].m_u)/
					(m_KnotArray[ToPnt-2].m_u-m_KnotArray[ToPnt-1].m_u);

				vv.m_x=p1*(m_IntplPntArray[ToPnt-1].m_pt.m_x-m_IntplPntArray[ToPnt].m_pt.m_x)-
					p2*(m_IntplPntArray[ToPnt-2].m_pt.m_x-m_IntplPntArray[ToPnt-1].m_pt.m_x);
				vv.m_y=p1*(m_IntplPntArray[ToPnt-1].m_pt.m_y-m_IntplPntArray[ToPnt].m_pt.m_y)-
					p2*(m_IntplPntArray[ToPnt-2].m_pt.m_y-m_IntplPntArray[ToPnt-1].m_pt.m_y);
				vv.m_z=p1*(m_IntplPntArray[ToPnt-1].m_pt.m_z-m_IntplPntArray[ToPnt].m_pt.m_z)-
					p2*(m_IntplPntArray[ToPnt-2].m_pt.m_z-m_IntplPntArray[ToPnt-1].m_pt.m_z);
			}
			else //
			{
				if(0 == m_iEndConstrain||2 == m_iEndConstrain)
				{
					vv.m_x=end_nx;
					vv.m_y=end_ny;
					vv.m_z=end_nz;
				}
			}
			vv.Normal();
		}
	}
	if(m_Close==1)
	{
		v=v-vv;
		vv.m_x=v.m_x/2.0;
		vv.m_y=v.m_y/2.0;
		vv.m_z=v.m_z/2.0;
		v=vv;
		vv.m_x=-vv.m_x;
		vv.m_y=-vv.m_y;
		vv.m_z=-vv.m_z;
	}
	//    x
	r[0]=0.0;
	if(m_FrmCond==1 || m_FrmCond==2)
		r[1]=0.0;
	else
		r[1]=v.m_x;
	if(m_ToCond==1 || m_ToCond==2)
		r[ToPnt-FrmPnt+3]=0.0;
	else
		r[ToPnt-FrmPnt+3]=-vv.m_x; 

	int j;
	for(i=FrmPnt,j=2;i<=ToPnt;i++,j++)
		r[j]=m_IntplPntArray[i].m_pt.m_x;
	swap(r[1],r[2]);
	swap(r[ToPnt-FrmPnt+3],r[ToPnt-FrmPnt+2]);
	b[2] -= f4*a[3]/c[3];
	c[2] -= f4*b[3]/c[3];
	a[ToPnt-FrmPnt+2] -= e4*b[ToPnt-FrmPnt+1]/a[ToPnt-FrmPnt+1];
	b[ToPnt-FrmPnt+2] -= e4*c[ToPnt-FrmPnt+1]/a[ToPnt-FrmPnt+1];
	r[2] -= f4*r[3]/c[3];
	r[ToPnt-FrmPnt+2] -= e4*r[ToPnt-FrmPnt+1]/a[ToPnt-FrmPnt+1];
	tridag(a,b,c,r,ux,ToPnt-FrmPnt+3);
	//
	//y
	r[0]=0.0;
	if(m_FrmCond==1 || m_FrmCond==2)
		r[1]=0.0;
	else
		r[1]=v.m_y;
	if(m_ToCond==1 || m_ToCond==2)
		r[ToPnt-FrmPnt+3]=0.0;
	else
		r[ToPnt-FrmPnt+3]=-vv.m_y;

	for(j=2,i=FrmPnt;i<=ToPnt;i++,j++)
		r[j]=m_IntplPntArray[i].m_pt.m_y;
	swap(r[1],r[2]);
	swap(r[ToPnt-FrmPnt+3],r[ToPnt-FrmPnt+2]);

	r[2] -= f4*r[3]/c[3];
	r[ToPnt-FrmPnt+2] -= e4*r[ToPnt-FrmPnt+1]/a[ToPnt-FrmPnt+1];
	tridag(a,b,c,r,uy,ToPnt-FrmPnt+3);
	//
	//z
	r[0]=0.0;
	if(m_FrmCond==1 || m_FrmCond==2)
		r[1]=0.0;
	else
		r[1]=v.m_z;
	if(m_ToCond==1 || m_ToCond==2)
		r[ToPnt-FrmPnt+3]=0.0;
	else
		r[ToPnt-FrmPnt+3]=-vv.m_z;

	for(j=2,i=FrmPnt;i<=ToPnt;i++,j++)
		r[j]=m_IntplPntArray[i].m_pt.m_z;

	swap(r[1],r[2]);
	swap(r[ToPnt-FrmPnt+3],r[ToPnt-FrmPnt+2]);

	r[2] -= f4*r[3]/c[3];
	r[ToPnt-FrmPnt+2] -= e4*r[ToPnt-FrmPnt+1]/a[ToPnt-FrmPnt+1];
	tridag(a,b,c,r,uz,ToPnt-FrmPnt+3);
	//
	int ctrlend=ToPnt-FrmPnt+3;
	if(m_KnotArray[ToPnt].m_multi==2)
		ctrlend--;
	int ctrlfrm=0;
	if(m_KnotArray[FrmPnt].m_multi==2 ||
		(m_KnotArray[FrmPnt].m_multi==3 && m_KnotArray[FrmPnt].m_type != 1))
		ctrlfrm++;
	De_BPnt tt;
	for(i=ctrlfrm;i<ctrlend;i++)                                                    //include line construction
	{
		if(m_KnotArray.GetSize()==6&&!CurveDivide)
		{
			tt.m_x=m_IntplPntArray[0].m_pt.m_x+\
				(m_IntplPntArray[5].m_pt.m_x-m_IntplPntArray[0].m_pt.m_x)*i/3.0;
			tt.m_y=m_IntplPntArray[0].m_pt.m_y+\
				(m_IntplPntArray[5].m_pt.m_y-m_IntplPntArray[0].m_pt.m_y)*i/3.0;
			tt.m_z=m_IntplPntArray[0].m_pt.m_z+\
				(m_IntplPntArray[5].m_pt.m_z-m_IntplPntArray[0].m_pt.m_z)*i/3.0;
			m_CtrlPntArray.Add(tt);
		}
		else
		{
			tt.m_x=ux[i+1];
			tt.m_y=uy[i+1];
			tt.m_z=uz[i+1];
			m_CtrlPntArray.Add(tt);
		}
	}
	delete []a;
	delete []b;
	delete []c;
	delete []ux;
	delete []uy;
	delete []uz;
	delete []r;
	delete []base;
} 

/////////////////////////////////////////////////////////////////////////////////
//	求控制顶点
/////////////////////////////////////////////////////////////////////////////////
BOOL	De_BSCurve::CalCtrlPnt(/**/)
{
	int		m_Frm, m_To, i;
	int		m_FrmCond,m_ToCond;
    int k;

	m_Frm=0;
	m_FrmCond=0;
	m_ToCond=0;

	m_KnotArray.RemoveAll();
	m_CtrlPntArray.RemoveAll();

	if(CalKnot()==FALSE) return FALSE;
	//if(pMain->KnotOK)
	//{ 
	//	for(i=0;i<m_KnotArray.GetSize();i++) 
	//		m_KnotArray[i].m_u=pView->PntErr[i];
	//} //jjjj2008, need modified

	int ksize=m_KnotArray.GetSize();
	int ipSize=m_IntplPntArray.GetSize();
	int mm=0;
	De_BSCKnot kt;
	De_IntplPnt pp;
	if(m_Close==1)
	{
		m_KnotArray[m_KnotArray.GetSize()-1].m_type=1;
		m_KnotArray[m_KnotArray.GetSize()-1].m_multi=3;
		m_KnotArray[0].m_multi=3;
		m_KnotArray[0].m_type=1;
	}
	for(i=ipSize-1;i>=0;i--)
	{
		mm=m_KnotArray[i].m_multi-1;
		kt=m_KnotArray[i];
		pp=m_IntplPntArray[i];
		for(int j=0;j<mm;j++)
		{
			m_KnotArray.InsertAt(i,kt);
			m_IntplPntArray.InsertAt(i,pp);
		}
	}
	for(i=0;i<m_KnotArray[m_Frm].m_multi-1;i++)
		m_Frm++;
	while(FindInter(m_Frm,m_To))
	{
		if(m_KnotArray[m_Frm].m_type)
			m_FrmCond=m_StartCond;
		else
			m_FrmCond=0;

		if(m_KnotArray[m_To].m_type)
			m_ToCond=m_EndCond;
		else
			m_ToCond=0;

		if(m_KnotArray[m_To].m_multi==2)
		{
			kt=m_KnotArray[m_To];
			pp=m_IntplPntArray[m_To];
			m_KnotArray.InsertAt(m_To,kt);
			m_IntplPntArray.InsertAt(m_To,pp);
		}
		if(m_KnotArray[m_Frm].m_multi==2 && m_KnotArray[m_To].m_multi==2 
			&& m_To==m_Frm+1)
			CalLineSec(m_Frm,m_To,m_FrmCond,m_EndCond);	//cal  line
		else
		{
			if(m_Close)
				CalCurSec(m_Frm,m_To,m_FrmCond,m_EndCond);//cal normal BSC
			else
				CalCurSec(m_Frm,m_To,m_FrmCond,m_EndCond);//cal normal BSC
		}
		m_Frm=m_To;
		for(i=0;i<m_Degree-1;i++) m_Frm++;
	}
	for(i=m_KnotArray.GetSize()-1;i>=0;i--)
	{
		if(m_KnotArray[i].m_multi==2)
		{
			m_KnotArray.RemoveAt(i);
			m_IntplPntArray.RemoveAt(i);
			i -= 2;
		}
	}
/////////////////////////////////////////////////计算端点切矢
	double t0=0;
	double t1=m_KnotArray[3].m_u;
	double nx1,ny1,nz1,nx2,ny2,nz2,x1,y1;
	nx1=3*(m_CtrlPntArray[0].m_x-m_CtrlPntArray[1].m_x)/(t1-t0);
	ny1=3*(m_CtrlPntArray[0].m_y-m_CtrlPntArray[1].m_y)/(t1-t0);
	nz1=3*(m_CtrlPntArray[0].m_z-m_CtrlPntArray[1].m_z)/(t1-t0);
	if(fabs(nx1)<1.0e-5) nx1=1.0e-5;
	if(fabs(ny1)<1.0e-5) ny1=1.0e-5;
	if(fabs(nz1)<1.0e-5) nz1=1.0e-5;
	double mold=sqrt(nx1*nx1+ny1*ny1+nz1*nz1);
	nx1=nx1/mold;	ny1=ny1/mold;	nz1=nz1/mold;
	m_IntplPntArray[0].m_pt.m_nx=nx1;
	m_IntplPntArray[0].m_pt.m_ny=ny1;
	m_IntplPntArray[0].m_pt.m_nz=nz1;

	t0=m_KnotArray[m_KnotArray.GetSize()-1].m_u;
	t1=m_KnotArray[m_KnotArray.GetSize()-4].m_u;
	x1=m_CtrlPntArray[m_CtrlPntArray.GetSize()-1].m_x;
	y1=m_CtrlPntArray[m_CtrlPntArray.GetSize()-2].m_x;
	nx2=3*(x1-y1)/(t1-t0);
	x1=m_CtrlPntArray[m_CtrlPntArray.GetSize()-1].m_y;
	y1=m_CtrlPntArray[m_CtrlPntArray.GetSize()-2].m_y;
	ny2=3*(x1-y1)/(t1-t0);
	x1=m_CtrlPntArray[m_CtrlPntArray.GetSize()-1].m_z;
	y1=m_CtrlPntArray[m_CtrlPntArray.GetSize()-2].m_z;
	nz2=3*(x1-y1)/(t1-t0);
	if(fabs(nx2)<1.0e-5) nx2=1.0e-5;
	if(fabs(ny2)<1.0e-5) ny2=1.0e-5;
	if(fabs(nz2)<1.0e-5) nz2=1.0e-5;
	mold=sqrt(nx2*nx2+ny2*ny2+nz2*nz2);
	nx2=nx2/mold;	ny2=ny2/mold;	nz2=nz2/mold;
	m_IntplPntArray[m_IntplPntArray.GetSize()-1].m_pt.m_nx=nx2;
	m_IntplPntArray[m_IntplPntArray.GetSize()-1].m_pt.m_ny=ny2;
	m_IntplPntArray[m_IntplPntArray.GetSize()-1].m_pt.m_nz=nz2;
///////////////////////////////////////////////离散
	De_IntplPnt tmp;
	m_DrawData.RemoveAll();
	for(int k=2;k<m_IntplPntArray.GetSize()-2;k++)
	{
		m_DrawData.Add(m_IntplPntArray[k]);
		m_DrawData[m_DrawData.GetSize()-1].knot_u=m_KnotArray[k].m_u;
	}
	double dist,uu,l,m,n;
	for(k=0;k<m_DrawData.GetSize()-1;k++)//B样条离散-精度error,Par_value保存与插入点对应的u值
	{
		uu=(m_DrawData[k].knot_u+m_DrawData[k+1].knot_u)/2;
		CalPnt(uu,tmp.m_pt.m_x,tmp.m_pt.m_y,tmp.m_pt.m_z);
		l=m_DrawData[k+1].m_pt.m_x-m_DrawData[k].m_pt.m_x;
		m=m_DrawData[k+1].m_pt.m_y-m_DrawData[k].m_pt.m_y;
		n=m_DrawData[k+1].m_pt.m_z-m_DrawData[k].m_pt.m_z;
		double fm=(l*(tmp.m_pt.m_x-m_DrawData[k].m_pt.m_x)+m*(tmp.m_pt.m_y-m_DrawData[k].m_pt.m_y)+\
			n*(tmp.m_pt.m_z-m_DrawData[k].m_pt.m_z))*(l*(tmp.m_pt.m_x-m_DrawData[k].m_pt.m_x)+m*(tmp.m_pt.m_y-m_DrawData[k].m_pt.m_y)+\
			n*(tmp.m_pt.m_z-m_DrawData[k].m_pt.m_z));
		fm=fm/(l*l+m*m+n*n);
		dist=sqrt((tmp.m_pt.m_x-m_DrawData[k].m_pt.m_x)*(tmp.m_pt.m_x-m_DrawData[k].m_pt.m_x)+\
			(tmp.m_pt.m_y-m_DrawData[k].m_pt.m_y)*(tmp.m_pt.m_y-m_DrawData[k].m_pt.m_y)+\
			(tmp.m_pt.m_z-m_DrawData[k].m_pt.m_z)*(tmp.m_pt.m_z-m_DrawData[k].m_pt.m_z)-fm);//p-lint distence;
		if(dist>0.01) 
		{
			tmp.knot_u=uu;
			m_DrawData.InsertAt(k+1,tmp);
			k--;
		}
	}
//	CopyHeadEndPnt();
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
//	计算一点坐标
//////////////////////////////////////////////////////////////////////////////
int	De_BSCurve::CalPnt(double m_u,double& x,double& y,double& z)
{
	int Ksize=m_KnotArray.GetSize();
	int s=0;
	int ip=0;
	int i;
	double *m_b=new double[m_Degree+2];
	if(Ksize<=0) return FALSE;
/*	if(m_u==m_KnotArray[Ksize-1].m_u) //最后一个点
	{
		x=m_IntplPntArray[m_IntplPntArray.GetSize()-1].m_pt.m_x;
		y=m_IntplPntArray[m_IntplPntArray.GetSize()-1].m_pt.m_y;
		z=m_IntplPntArray[m_IntplPntArray.GetSize()-1].m_pt.m_z;
		return TRUE ;
	}
*/	if(m_u < m_KnotArray[0].m_u || m_u>m_KnotArray[Ksize-1].m_u)
	{
		return FALSE;
	}

	for(i=0;i<Ksize;i++)
	{
		if(i>=Ksize-1) break;
		if(m_u>=m_KnotArray[i].m_u && m_u<m_KnotArray[i+1].m_u)
		{
			s=i;
			break;
		}
	}
	if(i>=Ksize-1)
		s=Ksize-m_Degree-1/*4*/;
	CalBase(m_Degree,s,m_u,m_b);

	x=0.0;
	y=0.0;
	z=0.0;

	for(i=0;i<=m_Degree;i++)
	{
		x += m_b[i+1]*m_CtrlPntArray[s+i-m_Degree+1/*2*/].m_x;
		y += m_b[i+1]*m_CtrlPntArray[s+i-m_Degree+1/*2*/].m_y;
		z += m_b[i+1]*m_CtrlPntArray[s+i-m_Degree+1/*2*/].m_z;
	}
	delete []m_b;
	return s;
}

double De_BSCurve::CalTangent(double s,Re_Pnt3D &pnt) //计算曲线上一点的切矢
{
	int i,pos,Ksize;
	double m_b[4],U_max,delt_u,dx,dy,dz;
	Ksize=m_KnotArray.GetSize();
	U_max=m_KnotArray[Ksize-1].m_u;
	double u=s*U_max;
	if(fabs(1.0-s)<1.0e-4) u-=1.0e-5;
	for(i=0;i<Ksize;i++)
	{
		if(i>=Ksize-1) break;
		if(u>=m_KnotArray[i].m_u && u<m_KnotArray[i+1].m_u)
		{
			pos=i;
			break;
		}
	}
	CalBase(2,pos,u,m_b); //2次B样条基
	pnt.m_nx=0.0;
	pnt.m_ny=0.0;
	pnt.m_nz=0.0;
	for(i=pos-2;i<=pos;i++)
	{
		delt_u=m_KnotArray[i+3].m_u-m_KnotArray[i].m_u;
		dx=m_CtrlPntArray[i+1].m_x-m_CtrlPntArray[i].m_x;
		dx=dx/delt_u;
		dy=m_CtrlPntArray[i+1].m_y-m_CtrlPntArray[i].m_y;
		dy=dy/delt_u;
		dz=m_CtrlPntArray[i+1].m_z-m_CtrlPntArray[i].m_z;
		dz=dz/delt_u;
		pnt.m_nx += 3*m_b[i-pos+2+1]*dx;
		pnt.m_ny += 3*m_b[i-pos+2+1]*dy;
		pnt.m_nz += 3*m_b[i-pos+2+1]*dz;
	}
	double mold=sqrt(pnt.m_nx*pnt.m_nx+pnt.m_ny*pnt.m_ny+pnt.m_nz*pnt.m_nz);
	pnt.m_nx=pnt.m_nx/mold;
	pnt.m_ny=pnt.m_ny/mold;
	pnt.m_nz=pnt.m_nz/mold;
	CalPnt(u,pnt.m_x,pnt.m_y,pnt.m_z);

	return mold;
}

BOOL De_BSCurve::UniformKnot(int iCtrl)
{
	float *knots = (float *)new BYTE[sizeof(float)*(iCtrl+2)];
	int j;
	for(j=2;j<iCtrl+2;j++)
	{
		knots[j]=(float)(j-2.);
	}
	knots[0]=0.;
	knots[1]=0.;
	knots[2]=0.;
	knots[iCtrl+1]=(float)(iCtrl-3);
	knots[iCtrl]=(float)(iCtrl-3);
	knots[iCtrl-1]=(float)(iCtrl-3);
	m_KnotArray.SetSize(iCtrl+2);
	for(j=0;j<iCtrl+2;j++)
	{
		m_KnotArray[j].m_u=knots[j];
	}
	delete knots;
	return TRUE;
}

//////////////////////////////////////////////
BOOL	De_BSCurve::CalCtrlPntNew(/**/)
{
	int		m_Frm, m_To, i;
	int		m_FrmCond,m_ToCond;

	m_Frm=0;
	m_FrmCond=0;
	m_ToCond=0;

	m_KnotArray.RemoveAll();
	m_CtrlPntArray.RemoveAll();

	if(CalKnot()==FALSE) return FALSE;
	int ksize=m_KnotArray.GetSize();
	int ipSize=m_IntplPntArray.GetSize();
	int mm=0;
	De_BSCKnot kt;
	De_IntplPnt pp;
	if(m_Close==1)
	{
		m_KnotArray[m_KnotArray.GetSize()-1].m_type=1;
		m_KnotArray[m_KnotArray.GetSize()-1].m_multi=3;
		m_KnotArray[0].m_multi=3;
		m_KnotArray[0].m_type=1;
	}
	for(i=ipSize-1;i>=0;i--)
	{
		mm=m_KnotArray[i].m_multi-1;
		kt=m_KnotArray[i];
		pp=m_IntplPntArray[i];
		for(int j=0;j<mm;j++)
		{
			m_KnotArray.InsertAt(i,kt);
			m_IntplPntArray.InsertAt(i,pp);
		}
	}
	for(i=0;i<m_KnotArray[m_Frm].m_multi-1;i++)
		m_Frm++;
	while(FindInter(m_Frm,m_To))
	{
		if(m_KnotArray[m_Frm].m_type)
			m_FrmCond=m_StartCond;
		else
			m_FrmCond=0;

		if(m_KnotArray[m_To].m_type)
			m_ToCond=m_EndCond;
		else
			m_ToCond=0;

		if(m_KnotArray[m_To].m_multi==2)
		{
			kt=m_KnotArray[m_To];
			pp=m_IntplPntArray[m_To];
			m_KnotArray.InsertAt(m_To,kt);
			m_IntplPntArray.InsertAt(m_To,pp);
		}
		if(m_KnotArray[m_Frm].m_multi==2 && m_KnotArray[m_To].m_multi==2 
			&& m_To==m_Frm+1)
			CalLineSec(m_Frm,m_To,m_FrmCond,m_EndCond);	//cal  line
		else
		{
			if(m_Close)
				CalCurSec(m_Frm,m_To,m_FrmCond,m_EndCond);//cal normal BSC
			else
				CalCurSec(m_Frm,m_To,m_FrmCond,m_EndCond);//cal normal BSC
		}
		m_Frm=m_To;
		for(i=0;i<m_Degree-1;i++) m_Frm++;
	}
	for(i=m_KnotArray.GetSize()-1;i>=0;i--)
	{
		if(m_KnotArray[i].m_multi==2)
		{
			m_KnotArray.RemoveAt(i);
			m_IntplPntArray.RemoveAt(i);
			i -= 2;
		}
	}
	//m_ColorIndex=BRIGHTGREEN_COLORRGBA;
	return TRUE;
}
////////////////////////////////////

void De_BSCurve::KnotStandardization()
{
	double u0=m_KnotArray[0].m_u;
	int i;
	for (i=0;i<m_KnotArray.GetSize();i++)
		m_KnotArray[i].m_u-=u0;
	double len=m_KnotArray[m_KnotArray.GetSize()-1].m_u-m_KnotArray[0].m_u;
	for (i=0;i<m_KnotArray.GetSize();i++)
		m_KnotArray[i].m_u=(m_KnotArray[i].m_u-m_KnotArray[0].m_u)/len;
}


double De_BSCurve::TolLength()
{
	double x0,y0,z0,x,y,z,dis=0;
	CalPnt(0,x0,y0,z0);
	double star = m_KnotArray[m_KnotArray.GetSize()-1].m_u/100.0;
	double step = m_KnotArray[m_KnotArray.GetSize()-1].m_u/100.0;
	for(double i=star;i<m_KnotArray[m_KnotArray.GetSize()-1].m_u;i+=step)
	{
		CalPnt(i,x,y,z);
		dis+=sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0)+(z-z0)*(z-z0));
		x0=x;y0=y;z0=z;
	}
	CalPnt(m_KnotArray[m_KnotArray.GetSize()-1].m_u,x,y,z);
	dis+=sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0)+(z-z0)*(z-z0));
    return dis;
}


//////////////////////////////////////////////////////////// 
// u: 被插节点的前一个节点序号
// l: 插入节点 t 之前的重数
// n: 插入次数
// t: 插入的节点值
BOOL De_BSCurve::InsertKnot(int u,int l,int n,double t)
{
	int i,r;
	int Ksize=m_KnotArray.GetSize();
	if (Ksize<=0) return FALSE;
	if ((n+l)>m_Degree) return FALSE;
	if (t < m_KnotArray[u].m_u || t>m_KnotArray[u+l+1].m_u)
		return FALSE;
	if (u>=Ksize-1)
		return FALSE;
	
	int k=m_Degree+1;
	int start,end;
	start=u-(k-l-1);
	end=u;
	for (r=1;r<=n;r++)
	{
		De_BPntArray TmpCtrlPnts;
		De_BPnt TmpCtrlPnt;
		for (int i=start+r-1;i<=end;i++)
			TmpCtrlPnts.Add(m_CtrlPntArray[i+1]);
		int j=1;
		for (i=start+r;i<end;i++)
		{
			double lamda=(t-m_KnotArray[i].m_u)/(m_KnotArray[i+k-r].m_u-m_KnotArray[i].m_u);
			m_CtrlPntArray[i+1].m_x=(1-lamda)*TmpCtrlPnts[j-1].m_x+lamda*TmpCtrlPnts[j].m_x;
			m_CtrlPntArray[i+1].m_y=(1-lamda)*TmpCtrlPnts[j-1].m_y+lamda*TmpCtrlPnts[j].m_y;
			m_CtrlPntArray[i+1].m_z=(1-lamda)*TmpCtrlPnts[j-1].m_z+lamda*TmpCtrlPnts[j].m_z;
			j++;
		}
		double lamda=(t-m_KnotArray[i].m_u)/(m_KnotArray[i+k-r].m_u-m_KnotArray[i].m_u);
		TmpCtrlPnt.m_x=(1-lamda)*TmpCtrlPnts[j-1].m_x+lamda*TmpCtrlPnts[j].m_x;
		TmpCtrlPnt.m_y=(1-lamda)*TmpCtrlPnts[j-1].m_y+lamda*TmpCtrlPnts[j].m_y;
		TmpCtrlPnt.m_z=(1-lamda)*TmpCtrlPnts[j-1].m_z+lamda*TmpCtrlPnts[j].m_z;
		m_CtrlPntArray.InsertAt(i+1,TmpCtrlPnt);
		TmpCtrlPnts.RemoveAll();
	}
	for (r=0;r<l;r++)
		m_KnotArray[u+1+r].m_multi+=n;
	De_BSCKnot newknot;
	newknot.m_u=t;
	newknot.m_multi=l+n;
	for (r=1;r<=n;r++)
		m_KnotArray.InsertAt(u+1,newknot);
	return TRUE;
}


////////////////节点插入
//// 插入新节点值为 u_ins,插入rtims次
//// 输出新的De_BSCurve    存在问题：在插入前应该判断rtims+s(重数)<=曲线次数??
De_BSCurve De_BSCurve::KontInsert(double u_ins, int rtims)
{
	//De_BSCKnotArray		m_KnotArray;      //节点序列
	//De_BPntArray		m_CtrlPntArray;   //控制顶点序列

	int r=rtims;//////////////////////////插入次数
	
	int np=m_CtrlPntArray.GetSize();//////np为控制顶点的数量/////////
	int mp=np+m_Degree+1;  ///////////////节点数量
	int nq=np+r;//////////////////////////新曲线的控制顶点数量
	int mq=nq+m_Degree+1;/////////////////新曲线的节点数量
	int p=m_Degree;///////////////////////次数
	int s =0;////           s 为u_ins在节点序列中重数
	int k;//////////// u(k)<=u_ins<u(k+1)///需要程序实现查找
	int L;
    int ius;
	De_BSCurve newCurve;

	/////////判断首点的节点重数
	int ks=0;
	double us1,us0;
	us0=m_KnotArray[0].m_u;


 	for(ius=1;ius<=m_Degree;ius++)
	{
		us1=m_KnotArray[ius].m_u;
		if(us1-us0>1e-10)break;        // modify 2002/04/19//from 1e-5 to 1e-10 everything is ok;
		us0=us1;
	}
	if(ius==m_Degree+1);
	else
	{
		m_KnotArray.InsertAt(0,m_KnotArray[0]);
		m_KnotArray.Add(m_KnotArray[m_KnotArray.GetSize()-1]);
	}
	///////Find the k  
	k=-1;
	for(int jj=0;jj<m_KnotArray.GetSize()-1;jj++)
	{
		double a=m_KnotArray[jj].m_u;
		double b=m_KnotArray[jj+1].m_u;
		
		if(fabs(a-u_ins)<0.0000001&& fabs(b-u_ins)>0.0000001&&b>u_ins)
		{
			k=jj;
			s=m_KnotArray[jj].m_multi;
			break;
		}
		else if(fabs(a-u_ins)<0.0000001)continue;
		else if(a<=u_ins&&b>u_ins)
		{
			k=jj;
			s=0;
			break;
		}
	}    
	if(k==-1||r==0||mp!=m_KnotArray.GetSize())  //.2002/03.modify.if(k==-1||r==0) is old.
	{
		return newCurve;
	}
	///////
	De_BPntArray Rw;
	Rw.SetSize(p+1);
			
	////Loaqd new knot vector///..
	newCurve.m_KnotArray.SetSize(mq);
    int i;
	for(i=0;i<=k;i++)newCurve.m_KnotArray[i]=m_KnotArray[i];
	for(i=1;i<=r;i++){
		newCurve.m_KnotArray[i+k].m_u=u_ins;
		newCurve.m_KnotArray[i+k].m_multi=r+s;
	}
	for(i=k+1;i<mp;i++)newCurve.m_KnotArray[i+r]=m_KnotArray[i];
	//////save unaltered control points
	newCurve.m_CtrlPntArray.SetSize(nq);
	for(i=0;i<=k-p;i++)
		newCurve.m_CtrlPntArray[i]=m_CtrlPntArray[i];
	for(i=k-s;i<np;i++)
		newCurve.m_CtrlPntArray[i+r]=m_CtrlPntArray[i];
	for(i=0;i<=p-s;i++)
		Rw[i]=m_CtrlPntArray[k-p+i];
	
	for(int j=1;j<=r-s;j++)
	{
		L=k-p+j;
		for(i=0;i<=p-j-s;i++)
		{
			double alpha=(u_ins-m_KnotArray[L+i].m_u)/(m_KnotArray[i+k+1].m_u-m_KnotArray[L+i].m_u);
			Rw[i].m_x=alpha*Rw[i+1].m_x+(1.0-alpha)*Rw[i].m_x;
			Rw[i].m_y=alpha*Rw[i+1].m_y+(1.0-alpha)*Rw[i].m_y;
			Rw[i].m_z=alpha*Rw[i+1].m_z+(1.0-alpha)*Rw[i].m_z;
		}
		newCurve.m_CtrlPntArray[L]=Rw[0];
		newCurve.m_CtrlPntArray[k+r-j-s]=Rw[p-j-s];
	}

	
	for(i=L+1;i<k-s;i++)newCurve.m_CtrlPntArray[i]=Rw[i-L];
	m_KnotArray.RemoveAt(0);
	m_KnotArray.RemoveAt(m_KnotArray.GetSize()-1);
	if(ius==m_Degree+1);
	else
	{
	newCurve.m_KnotArray.RemoveAt(0);
	newCurve.m_KnotArray.RemoveAt( newCurve.m_KnotArray.GetSize()-1);
	}
	newCurve.m_ColorIndex=2;
//	for(int kkk=0;kkk<newCurve.m_CtrlPntArray.GetSize();kkk++)
//	{
//		newCurve.m_CtrlPntArray[kkk].m_y+=30.0;
//	}
	return newCurve;
}



bool De_BSCurve::CurveKnotCheck() //check the curve knot to see if the curve have a repeated knot
{
	if(m_KnotArray.GetSize()!=0)
	{
		for(int i=3;i<m_KnotArray.GetSize()-2;i++)
		{
            double Unow=m_KnotArray[i].m_u;
			double Ubefore=m_KnotArray[i-1].m_u;
			if(Unow == Ubefore)
			{
                return false;
			}
		}
		return true;
	}
	else
	{
	    return false;
	}
}

BOOL  De_BSCurve::DiscreteKnot(Re_Pnt3DArray& pntarray)// 
{
	Re_Pnt3D   pnt;
	pntarray.RemoveAll();
	for(int i=m_Degree-1;i<m_KnotArray.GetSize()-m_Degree+1;i++)
	{
		CalPnt(m_KnotArray[i].m_u,pnt.m_x,pnt.m_y,pnt.m_z);
		pntarray.Add(pnt);
	}
	return true;
}


BOOL De_BSCurve::ReCreateIntplPnts()
{
	m_IntplPntArray.RemoveAll();

	Re_Pnt3DArray pntarray;
	DiscreteKnot(pntarray);
	if (pntarray.GetSize()!=m_KnotArray.GetSize()-2*m_Degree+2)
		return FALSE;
	double stnx=m_Degree*(m_CtrlPntArray[0].m_x-m_CtrlPntArray[1].m_x)/pntarray[0].DistantTo(pntarray[1]);
	double stny=m_Degree*(m_CtrlPntArray[0].m_y-m_CtrlPntArray[1].m_y)/pntarray[0].DistantTo(pntarray[1]);
	double stnz=m_Degree*(m_CtrlPntArray[0].m_z-m_CtrlPntArray[1].m_z)/pntarray[0].DistantTo(pntarray[1]);
	double ednx=m_Degree*(m_CtrlPntArray[m_CtrlPntArray.GetSize()-2].m_x-m_CtrlPntArray[m_CtrlPntArray.GetSize()-1].m_x)/pntarray[pntarray.GetSize()-2].DistantTo(pntarray[pntarray.GetSize()-1]);
	double edny=m_Degree*(m_CtrlPntArray[m_CtrlPntArray.GetSize()-2].m_y-m_CtrlPntArray[m_CtrlPntArray.GetSize()-1].m_y)/pntarray[pntarray.GetSize()-2].DistantTo(pntarray[pntarray.GetSize()-1]);
	double ednz=m_Degree*(m_CtrlPntArray[m_CtrlPntArray.GetSize()-2].m_z-m_CtrlPntArray[m_CtrlPntArray.GetSize()-1].m_z)/pntarray[pntarray.GetSize()-2].DistantTo(pntarray[pntarray.GetSize()-1]);
	for (int i=0;i<pntarray.GetSize();i++)
	{
		De_IntplPnt pnt;
		pnt.m_pt.m_x=pntarray[i].m_x;
		pnt.m_pt.m_y=pntarray[i].m_y;
		pnt.m_pt.m_z=pntarray[i].m_z;
		if (i==0)
		{
			pnt.m_pt.m_nx=stnx;
			pnt.m_pt.m_ny=stny;
			pnt.m_pt.m_nz=stnz;
			for (int j=0;j<m_Degree;j++)
				m_IntplPntArray.Add(pnt);
		}
		else if (i==pntarray.GetSize()-1)
		{
			pnt.m_pt.m_nx=ednx;
			pnt.m_pt.m_ny=edny;
			pnt.m_pt.m_nz=ednz;
			for (int j=0;j<m_Degree;j++)
				m_IntplPntArray.Add(pnt);
		}
		else
			m_IntplPntArray.Add(pnt);
	}
	return TRUE;
}


void Sort_LSIFT(CArray<double,double>& H,int m,int n,CArray<int,int>& M)
{
	int i=m;
	int j=2*i;
	double x=H[i];
	int    xm=M[i];
	while(j<=n)
	{
		if(j<n&&H[j]<H[j+1])j=j+1;
		if(x<H[j])
		{
			H[i]=H[j];
			M[i]=M[j];
			i=j;
			j=2*i;
		}
		else break;
	}
	H[i]=x;
	M[i]=xm;
	return;
}



void Sort_LHEAP(CArray<double,double>& H,int n,CArray<int,int>& M)
{
	int p= n/2;
	for(int i=p;i>=1;i--)
	{
		Sort_LSIFT(H,i,n,M);
	}
	return;
}


void Sort_LHEAPSORT(CArray<double,double>& H,CArray<int,int> &M)
{
	int i;
	int n= H.GetSize();
	H.InsertAt(0,-9999999.0);
	M.SetSize(n+1);
	for(i=0;i<n+1;i++)
	{
		M[i] = i-1;
	}
	Sort_LHEAP(H,n,M);

	for(i=n;i>=2;i--)
	{
		double temp = H[1];
		int    itmp = M[1];
		H[1]=H[i];
		M[1]=M[i];
		H[i] = temp;
		M[i] = itmp;
		Sort_LSIFT(H,1,i-1,M);
	}
	H.RemoveAt(0);
	M.RemoveAt(0);
	return;
}
