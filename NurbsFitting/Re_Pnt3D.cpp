#include "stdafx.h"
#include "Re_Pnt3D.h"
#include "math.h"
#include "Re_Matrix44.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[]=__FILE__;
#endif
#define TINY	1.0e-20


 

Re_PntAttr::Re_PntAttr()
{
	m_type=NONE;
	m_iSurCur = -1;
	m_iPatch  = -1;
	m_u = 0.;
	m_v = 0.;
}


Re_PntAttr::Re_PntAttr(const Re_PntAttr& a)
{
	m_type    = a.m_type;
	m_iSurCur = a.m_iSurCur;
	m_iPatch  = a.m_iPatch;
	m_u       = a.m_u;
	m_v       = a.m_v;
}


Re_PntAttr  &Re_PntAttr::operator =(Re_PntAttr a)
{
	m_type    = a.m_type;
	m_iSurCur = a.m_iSurCur;
	m_iPatch  = a.m_iPatch;
	m_u       = a.m_u;
	m_v       = a.m_v;
	return (*this);
}

Re_PntAttr::~Re_PntAttr()
{

}



//////////////
//
//    

Re_Pnt3D::Re_Pnt3D()
{
	m_x = 0.0;
	m_y = 0.;
	m_z = 0.;
	m_nx = 0.;
	m_ny = 0.;
	m_nz = 0.;
	m_IsPicked=FALSE;
//	m_attrArray.SetSize(0);
}

void Re_Pnt3D::Init()
{
	m_x = 0.;
	m_y = 0.;
	m_z = 0.;
	m_nx = 0.;
	m_ny = 0.;
	m_nz = 0.;
	m_IsPicked=FALSE;
//	m_attrArray.SetSize(0);
}

Re_Pnt3D::Re_Pnt3D(const Re_Pnt3D& a)
{
	m_x = a.m_x;
	m_y = a.m_y;
	m_z = a.m_z;
	m_nx = a.m_nx;
	m_ny = a.m_ny;
	m_nz = a.m_nz;
	m_attr=a.m_attr;
	m_IsPicked=a.m_IsPicked;
}

Re_Pnt3D Re_Pnt3D::operator =(Re_Pnt3D a)
{
	m_x = a.m_x;
	m_y = a.m_y;
	m_z = a.m_z;
	m_nx = a.m_nx;
	m_ny = a.m_ny;
	m_nz = a.m_nz;

	m_attr=a.m_attr;
	m_IsPicked=a.m_IsPicked;
	return (*this);
}


int  Re_Pnt3D::operator ==(Re_Pnt3D a)
{
	double dist;
	dist=sqrt((m_x-a.m_x)*(m_x-a.m_x)+(m_y-a.m_y)*(m_y-a.m_y)\
		+(m_z-a.m_z)*(m_z-a.m_z));
	if(dist<1.0e-2)
//	if(m_x==a.m_x&&m_y==a.m_y&&m_z==a.m_z)
		return 1;
	else return 0;
}

Re_Pnt3D::~Re_Pnt3D()
{
}


BOOL ludcmp(float *a,int n,int *indx,float *d)
{
	int i,imax,j,k;
	float big,dum,sum,temp;
	float *vv=new float[n+1];

	*d=1.0;
	for(i=1;i<=n;i++)
	{
		big=0.0;
		for(j=1;j<=n;j++)
			if((temp=(float)fabs(a[i*(n+1)+j]))>big) big=temp;
		if(big==0.0) return FALSE;
		vv[i]=(float)1.0/big;
	}
	for(j=1;j<=n;j++)
	{
		for(i=1;i<j;i++)
		{
			sum=a[i*(n+1)+j];
			for(k=1;k<i;k++)
				sum -= a[i*(n+1)+k]*a[k*(n+1)+j];
			a[i*(n+1)+j]=sum;
		}
		big=0.0;
		for(i=j;i<=n;i++)
		{
			sum=a[i*(n+1)+j];
			for(k=1;k<j;k++)
				sum -= a[i*(n+1)+k]*a[k*(n+1)+j];
			a[i*(n+1)+j]=sum;
			if((dum=vv[i]*(float)fabs(sum))>=big)
			{
				big=dum;
				imax=i;
			}
		}
		if(j != imax)
		{
			for(k=1;k<=n;k++)
			{
				dum=a[imax*(n+1)+k];
				a[imax*(n+1)+k]=a[j*(n+1)+k];
				a[j*(n+1)+k]=dum;
			}
			*d=-(*d);
			vv[imax]=vv[j];
		}
		indx[j]=imax;
		if(a[j*(n+1)+j]==0.0) a[j*(n+1)+j]=(float)TINY;
		if(j!=n)
		{
			dum=(float)1.0/a[j*(n+1)+j];
			for(i=j+1;i<=n;i++)
				a[i*(n+1)+j] *= dum;
		}
	}
	delete []vv;
	return TRUE;
}

void lubksb(float *a,int n,int *indx,float b[])
{
	int i,ii=0,ip,j;
	float sum;

	for(i=1;i<=n;i++)
	{
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if(ii)
		{
			for(j=ii;j<=i-1;j++)
				sum -= a[i*(n+1)+j]*b[j];
		}
		else if(sum) ii=i;
		b[i]=sum;
	}
	for(i=n;i>=1;i--)
	{
		sum=b[i];
		for(j=i+1;j<=n;j++) sum -= a[i*(n+1)+j]*b[j];
		b[i]=sum/a[i*(n+1)+i];
	}
}
////////////////////////////////////////////////////////////////////

void Re_Pnt3D::RotateToXY(double ax,double ay,double az)
{
	double x0,y0,z0;
	double x,y,z;

	x0=m_x;
	y0=m_y;
	z0=m_z;
	double xx = x0;
	double yy = y0*cos(ax)-z0*sin(ax);
	double zz = y0*sin(ax)+z0*cos(ax);

	x = xx*cos(ay)+zz*sin(ay);
	y = yy;
	z =-xx*sin(ay)+zz*cos(ay);

	m_x=x;
	m_y=y;
	m_z=z;
}

void Re_Pnt3D::RotateBack(double ax,double ay,double az)
{
	double x0,y0,z0;
	double x,y,z;

	x0=m_x;
	y0=m_y;
	z0=m_z;

	double xx = x0*cos(ay)-z0*sin(ay);
	double yy = y0;
	double zz = x0*sin(ay)+z0*cos(ay);	
	x = xx;
	y = yy*cos(ax)+zz*sin(ax);
	z = -yy*sin(ax)+zz*cos(ax);

	m_x=x;
	m_y=y;
	m_z=z;
}

double Re_Pnt3D::DistantTo(Re_Pnt3D a)
{
	double dist=sqrt((m_x-a.m_x)*(m_x-a.m_x)+\
					 (m_y-a.m_y)*(m_y-a.m_y)+\
					 (m_z-a.m_z)*(m_z-a.m_z));
	return dist;
}

BOOL Re_Pnt3D::DistantTo(Re_Pnt3D a,Re_Pnt3D b,double &dist,double &lamda)
{
	Re_Vector r(m_x-a.m_x,m_y-a.m_y,m_z-a.m_z);
	Re_Vector s(b.m_x-a.m_x,b.m_y-a.m_y,b.m_z-a.m_z);
	double len=s.Len();
	if (s.Normal()==0)
		return FALSE;
	Re_Vector d=r.Cross(s);
	dist=d.Len();
	double dd=r*s;
	lamda=dd/len;
	return TRUE;
}
BOOL Re_Pnt3D::ProjectToPln(double A,double B,double C,double D,double nx,double ny,double nz,Re_Pnt3D &newpnt)
{
	if (fabs(A*nx+B*ny+C*nz)<0.00001)
		return FALSE;
	double t=(A*m_x+B*m_y+C*m_z+D)/(A*nx+B*ny+C*nz);
	newpnt.m_x=m_x-nx*t;
	newpnt.m_y=m_y-ny*t;
	newpnt.m_z=m_z-nz*t;
	return TRUE;
}

