#include "StdAfx.h"
#include "XPolynomial.h"
#include "math.h"


XPolynomial::XPolynomial(void)
{
	this->m_ObjectType = OBJECT_POLYNOMIAL;
}

XPolynomial::~XPolynomial(void)
{   
	if (m_A!=NULL)
		delete m_A;
}

void XPolynomial::Translate(const PNT& delt_t)
{
	this->m_Translation += delt_t;
	this->m_bDispChanged = true;
}
void XPolynomial::Rotate(const Ti_Rotation& R)
{
	m_Orientation = R*m_Orientation;
	this->m_bDispChanged = true;
}

void XPolynomial::UpdateBndBox()
{
	if(this->m_bObjChanged)
	{
		m_bObjChanged = false;
		m_bDispChanged = true;
        
	}
}


void XPolynomial::SetUnit(const int& unit)
{
	if(this->m_Unit == UNIT_NONE|| m_Unit == unit)
	{
		m_Unit = unit;
		return;
	}
	switch(m_Unit)// suppose only two type unit(inch and mm)
	{
	case UNIT_MM:
		if(unit == UNIT_INCH)
		{
			for(int i=0;i<m_ParaNum;i++)
			   XPolynomial::m_A[i]/=25.4f;			
		}
		break;
	case UNIT_INCH:
		if(unit == UNIT_MM)
		{
				for(int i=0;i<m_ParaNum;i++)
			   XPolynomial::m_A[i]*=25.4f;
		}
		break;
	default:
		;
	}
	SetObjState(true);	
}


void XPolynomial::InitObject()
{
	SetObjState(true);
	m_bDispChanged = true;

}

//extern double PIE;
void XPolynomial::ComputePolynomial( int Pts,double ** g_pnt)
{
	int MeshPtNum=Pts;
	PNT tmppt1,tmppt2;
	//Ti_Math * TM;
	//double *roots;

	//tmppt1=this->m_BndBox.GetMax();
	//tmppt2=this->m_BndBox.GetMin();

	//for(i=0;i<MeshPtNum;i++)
	//{
	//	tmppt3.m[1]=tmppt2.m[1]+(tmppt1.m[1]-tmppt2.m[1])/MeshPtNum*i;
	//	for(j=0;j<MeshPtNum;j++)
	//	{
	//		tmppt3.m[0]=tmppt2.m[0]+(tmppt1.m[0]-tmppt2.m[0])/MeshPtNum*j;
	//		t1=m_A[2];
	//		t2=m_A[4]*tmppt3.m[0]+m_A[5]*tmppt3.m[1]+m_A[8];
	//		t3=m_A[0]*tmppt3.m[0]*tmppt3.m[0]+m_A[1]*tmppt3.m[1]*tmppt3.m[1]+m_A[3]*tmppt3.m[0]*tmppt3.m[1]+m_A[6]*tmppt3.m[0]+m_A[7]*tmppt3.m[1]+m_A[9];

	//		roots=TM->SolveQuadratic(t1, t2, t3);

	//		if(roots[0]!=0)
	//		{			
	//			if (roots[m_RF]>=tmppt2.m[2] && roots[m_RF]<=tmppt1.m[2] )
	//			{		
	//				tmppt3.m[2]=roots[m_RF];		
	//				pcld->AddPoint(tmppt3);		
	//			}
	//		}
	//	}		
	//}		
}


void XPolynomial::GetPolynomialInfo(int & ParaNum,int & RootFlag, double A[] )
{
ParaNum=this->m_ParaNum;
for(int i=0;i<m_ParaNum;i++)
    A[i]=this->m_A[i];
RootFlag=this->m_RF;
}

int __cdecl EvaluateforPolynomial(int m,		// number of points
								int n,		// number of parameters 
								double *const par, // the parameters
								double* rv,		// residual value,e.g. distance diviation
								double**g_pnt
								)
{
	double norm;
	double f,g;
	norm = sqrt(par[4]*par[4]+par[5]*par[5]+par[6]*par[6]);
	par[4] /= norm;
	par[5] /= norm;
	par[6] /= norm;
	for( int i=1 ; i <= m; i++)
	{
		g = (g_pnt[i][1] - par[1])*par[4] +
			(g_pnt[i][2] - par[2])*par[5] +
			(g_pnt[i][3] - par[3])*par[6] ;
		f = sqrt(
			(g_pnt[i][1] - par[1])*(g_pnt[i][1] - par[1])+
			(g_pnt[i][2] - par[2])*(g_pnt[i][2] - par[2])+
			(g_pnt[i][3] - par[3])*(g_pnt[i][3] - par[3])-
			g*g
			);
		rv[i] = f-par[7];
	//	rv[i] = exp(-(rv[i]*rv[i])/(0.1*0.1))*rv[i];
	}
	return 1;
}

//derivatives for distance Calculator
//void __cdecl JacobianforCylinder(int m,	// number of points 
//								 int n,		// number of parameters = 7
//								 double *const  par,	  // the elements are x,y,z A,B,C,r
//								 double*& rv,   // residual value,e.g. distance diviation
//								 double **jac,
//								 double **g_pnt
//								 )
//{
//	double f,g,fx,fy,fz,fa,fb,fc;
//	double norm;
//	norm = sqrt(par[4]*par[4]+par[5]*par[5]+par[6]*par[6]);
//	par[4] /= norm;
//	par[5] /= norm;
//	par[6] /= norm;
//	for( int i=1 ; i <= m; i++)
//	{
//		g = (g_pnt[i][1] - par[1])*par[4] +
//			(g_pnt[i][2] - par[2])*par[5] +
//			(g_pnt[i][3] - par[3])*par[6] ;
//		f = sqrt(
//			(g_pnt[i][1] - par[1])*(g_pnt[i][1] - par[1])+
//			(g_pnt[i][2] - par[2])*(g_pnt[i][2] - par[2])+
//			(g_pnt[i][3] - par[3])*(g_pnt[i][3] - par[3])-
//			g*g
//			);
//		if (fabs(f)>1e-6)
//		{
//			fx = (par[4]*g-(g_pnt[i][1]-par[1]))/f;
//			fy = (par[5]*g-(g_pnt[i][2]-par[2]))/f;
//			fz = (par[6]*g-(g_pnt[i][3]-par[3]))/f;
//			fa = g*(par[4]*g-(g_pnt[i][1]-par[1]))/f;
//			fb = g*(par[5]*g-(g_pnt[i][2]-par[2]))/f;
//			fc = g*(par[6]*g-(g_pnt[i][3]-par[3]))/f;
//		}
//		else
//		{
//			fx = sqrt(1- par[4]*par[4]);
//			fy = sqrt(1- par[5]*par[5]);
//			fz = sqrt(1- par[6]*par[6]);
//			fa = g*fx;
//			fb = g*fy;
//			fc = g*fz;
//		}
//		jac[i][1] = fx;
//		jac[i][2] = fy;
//		jac[i][3] = fz;
//		jac[i][4] = fa;
//		jac[i][5] = fb;
//		jac[i][6] = fc;
//		jac[i][7] = -1;		
//	}
//}