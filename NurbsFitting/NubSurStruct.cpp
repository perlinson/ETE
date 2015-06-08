// NubSurStruct.cpp: implementation of the CNubSurStruct class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gl\gl.h"
#include "gl\glu.h"
#include "NubSurStruct.h"
#include <math.h>
#include "De_BSCurve.h"
#include <time.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//IMPLEMENT_SERIAL(CNubSurStruct,CObject,0)

CNubSurStruct::CNubSurStruct()
{
	CtrlPnt.RemoveAll();
	PrimPnt.RemoveAll();
	U_Knot.RemoveAll();
	V_Knot.RemoveAll();
	TrimPnt.RemoveAll();
	dgr_u=3;
	dgr_v=3;
}

CNubSurStruct::~CNubSurStruct()
{
	CtrlPnt.RemoveAll();
	PrimPnt.RemoveAll();
	U_Knot.RemoveAll();
	V_Knot.RemoveAll();
	TrimPnt.RemoveAll();
}

CNubSurStruct &CNubSurStruct::operator =(CNubSurStruct a)
{
	int i;
	CtrlPnt.RemoveAll();
	PrimPnt.RemoveAll();
	U_Knot.RemoveAll();
	V_Knot.RemoveAll();
	TrimPnt.RemoveAll();
	for(i=0;i<a.CtrlPnt.GetSize();i++)
		CtrlPnt.Add(a.CtrlPnt[i]);
	for(i=0;i<a.PrimPnt.GetSize();i++)
		PrimPnt.Add(a.PrimPnt[i]);
	for(i=0;i<a.U_Knot.GetSize();i++)
		U_Knot.Add(a.U_Knot[i]);
	for(i=0;i<a.V_Knot.GetSize();i++)
		V_Knot.Add(a.V_Knot[i]);
	for(i=0;i<a.TrimPnt.GetSize();i++)
		TrimPnt.Add(a.TrimPnt[i]);
	m=a.m;
	n=a.n;
	dgr_u=a.dgr_u;
	dgr_v=a.dgr_v;
	return (*this);
}

CNubSurStruct::CNubSurStruct(const CNubSurStruct& a)
{
	CtrlPnt.RemoveAll();
	PrimPnt.RemoveAll();
	U_Knot.RemoveAll();
	V_Knot.RemoveAll();
	TrimPnt.RemoveAll();
	int i;
	for(i=0;i<a.CtrlPnt.GetSize();i++)
		CtrlPnt.Add(a.CtrlPnt[i]);
	for(i=0;i<a.PrimPnt.GetSize();i++)
		PrimPnt.Add(a.PrimPnt[i]);
	for(i=0;i<a.U_Knot.GetSize();i++)
		U_Knot.Add(a.U_Knot[i]);
	for(i=0;i<a.V_Knot.GetSize();i++)
		V_Knot.Add(a.V_Knot[i]);
	for(i=0;i<a.TrimPnt.GetSize();i++)
		TrimPnt.Add(a.TrimPnt[i]);
	m=a.m;
	n=a.n;
	dgr_u=a.dgr_u;
	dgr_v=a.dgr_v;
}

void CNubSurStruct::Init()
{
	U_Knot.RemoveAll();
	V_Knot.RemoveAll();
	CtrlPnt.RemoveAll();
	PrimPnt.RemoveAll();
	TrimPnt.RemoveAll();
	dgr_u=3;
	dgr_v=3;
}


void  CNubSurStruct::CalSufPnt(double &x,double &y,double &z,double u,double v)
{
	//Standardlize();

	/*int kku=1;
	for(int ii=0;ii<U_Knot.GetSize();ii++)
	{
		if(U_Knot.GetAt(ii)==U_Knot.GetAt(ii+1))
		{
			kku=kku+1;	
		}
		else
			break;
	}
	int kkv=1;
	for(ii=0;ii<V_Knot.GetSize();ii++)
	{
		if(V_Knot.GetAt(ii)==V_Knot.GetAt(ii+1))
		{
			kkv=kkv+1;	
		}
		else
			break;
	}*/

	int degreeu=dgr_u;
	int degreev=dgr_v;
	double *d_base_u=new double[degreeu+2];
	double *d_base_v=new double[degreev+2];
	int pos_u,pos_v;
    int i,j;
	if(u>U_Knot[U_Knot.GetSize()-1]) return;
	if(v>V_Knot[V_Knot.GetSize()-1]) return;
	if((U_Knot[U_Knot.GetSize()-1]-u)<1.0e-6) u=u-1.0e-6;
	if((V_Knot[V_Knot.GetSize()-1]-v)<1.0e-6) v=v-1.0e-6;
	//if(u>=1) u=u-1.0e-6;
	//if(v>=1) v=v-1.0e-6;

	for(int i=0;i<U_Knot.GetSize()-2;i++)
		if((U_Knot[i]<=u)&&(U_Knot[i+1]>u))
		{	pos_u=i;break;	}
	
	for(i=0;i<V_Knot.GetSize()-2;i++)
		if((V_Knot[i]<=v)&&(V_Knot[i+1]>v))
		{	pos_v=i;break;}

	x=0;y=0;z=0;i=1;
	for(int j=pos_v-degreev;j<=pos_v;j++)
	{
		d_base_v[i]=BsBase(j,degreev,v,V_Knot);
		i++;
	}
	j=1;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		d_base_u[j]=BsBase(i,degreeu,u,U_Knot);
		j++;
	}
	int ii=1;
	int jj=1;
	double xx=0,yy=0,zz=0;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		jj=1;xx=0;yy=0;zz=0;
		for(j=pos_v-degreev;j<=pos_v;j++)
		{
			xx=xx+d_base_v[jj]*CtrlPnt[i+j*m].m_x;
			yy=yy+d_base_v[jj]*CtrlPnt[i+j*m].m_y;
			zz=zz+d_base_v[jj]*CtrlPnt[i+j*m].m_z;
			jj++;
		}
		x=x+d_base_u[ii]*xx;
		y=y+d_base_u[ii]*yy;
		z=z+d_base_u[ii]*zz;
		ii++;
	}
	delete []d_base_u;
	delete []d_base_v;
	return;
}

double  CNubSurStruct::BsBase(int i,int k,double ur,CArray<double,double> &KnotArray)
{
	double temp1,temp2,temp3;
	if((ur<KnotArray[i])||(ur>KnotArray[i+k+1]))
		return 0.0;
	else
	{
	    if(k==0)
		{
			if((ur>=KnotArray[i])&&(ur<KnotArray[i+1])) return 1.0;
			else  return 0.0;
		}
	    else
		{
			if(((ur-KnotArray[i])==0.0)||((KnotArray[i+k]-KnotArray[i])==0.0))
				temp1=0.0;
			else
				temp1=(ur-KnotArray[i])/(KnotArray[i+k]-KnotArray[i]);
			if(((KnotArray[i+k+1]-ur)==0.0)||((KnotArray[i+k+1]-KnotArray[i+1])==0.0))
				temp2=0.0;
			else
				temp2=(KnotArray[i+k+1]-ur)/(KnotArray[i+k+1]-KnotArray[i+1]);
			temp3=temp1*BsBase(i,k-1,ur,KnotArray)+temp2*BsBase(i+1,k-1,ur,KnotArray);
			return temp3;
		}
	}

}

double  CNubSurStruct::BaseDv1(int i,int k,double ur,CArray<double,double> &KnotArray)
{
	double c1,c2;
	if(KnotArray[i+k]-KnotArray[i]==0.0)
		c1=0;
	else c1=BsBase(i,k-1,ur,KnotArray)/(KnotArray[i+k]-KnotArray[i]);
    if((KnotArray[i+k+1]-KnotArray[i+1])==0.0)
		c2=0;
	else c2=BsBase(i+1,k-1,ur,KnotArray)/(KnotArray[i+k+1]-KnotArray[i+1]);

	return k*(c1-c2);
}

double  CNubSurStruct::BaseDv2(int i,int k,double ur,CArray<double,double> &KnotArray)
{
	double c1,c2,c4;
	if(KnotArray[i+k]-KnotArray[i]==0.0)
		c1=0.0;
	else c1=BaseDv1(i,k-1,ur,KnotArray)/(KnotArray[i+k]-KnotArray[i]);

    if(KnotArray[i+k+1]-KnotArray[i+1]==0.0)
		c4=0.0;
	else c4=BaseDv1(i+1,k-1,ur,KnotArray)/(KnotArray[i+k+1]-KnotArray[i+1]);

	c2=k*(c1-c4);
	return c2;
}
void  CNubSurStruct::Standardlize()//节点矢量标准化
{
	int ii;
	//U 节点矢量标准化
	double u0=U_Knot[0];
	int up=U_Knot.GetSize();
	for(int ii=0;ii<up;ii++)
		U_Knot[ii] -= u0;
	double length=U_Knot[up-1]-U_Knot[0];
	if(length!=0.0)
	{
	   for(int i=0;i<up;i++)
		   U_Knot[i] /= length;
	}
	//V节点矢量标准化
	double v0=V_Knot[0];
	up=V_Knot.GetSize();
	for(ii=0;ii<up;ii++)
		V_Knot[ii] -= v0;
	length=V_Knot[up-1]-V_Knot[0];
	if(length!=0.0)
	{
	   for(int i=0;i<up;i++)
		   V_Knot[i] /= length;
	}
}

void   CNubSurStruct::CalSufDu1(double &x,double &y,double &z,double u,double v)
{
	//Standardlize();

	/*int kku=1;
	for(int ii=0;ii<U_Knot.GetSize();ii++)
	{
		if(U_Knot.GetAt(ii)==U_Knot.GetAt(ii+1))
		{
			kku=kku+1;	
		}
		else
			break;
	}
	int kkv=1;
	for(ii=0;ii<V_Knot.GetSize();ii++)
	{
		if(V_Knot.GetAt(ii)==V_Knot.GetAt(ii+1))
		{
			kkv=kkv+1;	
		}
		else
			break;
	}*/

	int degreeu=dgr_u;
	int degreev=dgr_v;
	double *d_base_u=new double[degreeu+2];
	double *d_base_v=new double[degreev+2];
	int pos_u,pos_v;
    int i,j;

	if(u>U_Knot[U_Knot.GetSize()-1]) return;
	if(v>V_Knot[V_Knot.GetSize()-1]) return;
	if((U_Knot[U_Knot.GetSize()-1]-u)<1.0e-6) u=u-1.0e-6;
	if((V_Knot[V_Knot.GetSize()-1]-v)<1.0e-6) v=v-1.0e-6;
	//if(u>=1) u=u-1.0e-6;
	//if(v>=1) v=v-1.0e-6;

	for(int i=0;i<U_Knot.GetSize()-2;i++)
		if((U_Knot[i]<=u)&&(U_Knot[i+1]>u))
		{	pos_u=i;break;	}
	
	for(i=0;i<V_Knot.GetSize()-2;i++)
		if((V_Knot[i]<=v)&&(V_Knot[i+1]>v))
		{	pos_v=i;break;}

	x=0;y=0;z=0;i=1;
	for(int j=pos_v-degreev;j<=pos_v;j++)
	{
		d_base_v[i]=BsBase(j,degreev,v,V_Knot);
		i++;
	}
	j=1;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		d_base_u[j]=BaseDv1(i,degreeu,u,U_Knot);
		j++;
	}
	int ii=1;
	int jj=1;
	double xx=0,yy=0,zz=0;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		jj=1;xx=0;yy=0;zz=0;
		for(j=pos_v-degreev;j<=pos_v;j++)
		{
			xx=xx+d_base_v[jj]*CtrlPnt[i+j*m].m_x;
			yy=yy+d_base_v[jj]*CtrlPnt[i+j*m].m_y;
			zz=zz+d_base_v[jj]*CtrlPnt[i+j*m].m_z;
			jj++;
		}
		x=x+d_base_u[ii]*xx;
		y=y+d_base_u[ii]*yy;
		z=z+d_base_u[ii]*zz;
		ii++;
	}
	delete []d_base_u;
	delete []d_base_v;
	return;
}


void   CNubSurStruct::CalSufDv1(double &x,double &y,double &z,double u,double v)
{
	//Standardlize();

	/*int kku=1;
	for(int ii=0;ii<U_Knot.GetSize();ii++)
	{
		if(U_Knot.GetAt(ii)==U_Knot.GetAt(ii+1))
		{
			kku=kku+1;	
		}
		else
			break;
	}
	int kkv=1;
	for(ii=0;ii<V_Knot.GetSize();ii++)
	{
		if(V_Knot.GetAt(ii)==V_Knot.GetAt(ii+1))
		{
			kkv=kkv+1;	
		}
		else
			break;
	}*/

	int degreeu=dgr_u;
	int degreev=dgr_v;
	double *d_base_u=new double[degreeu+2];
	double *d_base_v=new double[degreev+2];
	int pos_u,pos_v;

	if(u>U_Knot[U_Knot.GetSize()-1]) return;
	if(v>V_Knot[V_Knot.GetSize()-1]) return;
	if((U_Knot[U_Knot.GetSize()-1]-u)<1.0e-6) u=u-1.0e-6;
	if((V_Knot[V_Knot.GetSize()-1]-v)<1.0e-6) v=v-1.0e-6;
	//if(u>=1) u=u-1.0e-6;
	//if(v>=1) v=v-1.0e-6;
	int i,j;
	for(i=0;i<U_Knot.GetSize()-2;i++)
		if((U_Knot[i]<=u)&&(U_Knot[i+1]>u))
		{	pos_u=i;break;	}
	
	for(i=0;i<V_Knot.GetSize()-2;i++)
		if((V_Knot[i]<=v)&&(V_Knot[i+1]>v))
		{	pos_v=i;break;}

	x=0;y=0;z=0;i=1;
	for(j=pos_v-degreev;j<=pos_v;j++)
	{
		d_base_v[i]=BaseDv1(j,degreev,v,V_Knot);
		i++;
	}
	j=1;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		d_base_u[j]=BsBase(i,degreeu,u,U_Knot);
		j++;
	}
	int ii=1;
	int jj=1;
	double xx=0,yy=0,zz=0;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		jj=1;xx=0;yy=0;zz=0;
		for(j=pos_v-degreev;j<=pos_v;j++)
		{
			xx=xx+d_base_v[jj]*CtrlPnt[i+j*m].m_x;
			yy=yy+d_base_v[jj]*CtrlPnt[i+j*m].m_y;
			zz=zz+d_base_v[jj]*CtrlPnt[i+j*m].m_z;
			jj++;
		}
		x=x+d_base_u[ii]*xx;
		y=y+d_base_u[ii]*yy;
		z=z+d_base_u[ii]*zz;
		ii++;
	}
	delete []d_base_u;
	delete []d_base_v;
	return;
}

void   CNubSurStruct::CalSufDu2(double &x,double &y,double &z,double u,double v)
{
	//Standardlize();

	/*int kku=1;
	for(int ii=0;ii<U_Knot.GetSize();ii++)
	{
		if(U_Knot.GetAt(ii)==U_Knot.GetAt(ii+1))
		{
			kku=kku+1;	
		}
		else
			break;
	}
	int kkv=1;
	for(ii=0;ii<V_Knot.GetSize();ii++)
	{
		if(V_Knot.GetAt(ii)==V_Knot.GetAt(ii+1))
		{
			kkv=kkv+1;	
		}
		else
			break;
	}*/

	int degreeu=dgr_u;
	int degreev=dgr_v;
	double *d_base_u=new double[degreeu+2];
	double *d_base_v=new double[degreev+2];
	int pos_u,pos_v;

	if(u>U_Knot[U_Knot.GetSize()-1]) return;
	if(v>V_Knot[V_Knot.GetSize()-1]) return;
	if((U_Knot[U_Knot.GetSize()-1]-u)<1.0e-6) u=u-1.0e-6;
	if((V_Knot[V_Knot.GetSize()-1]-v)<1.0e-6) v=v-1.0e-6;
	//if(u>=1) u=u-1.0e-6;
	//if(v>=1) v=v-1.0e-6;
	int i,j;
	for(i=0;i<U_Knot.GetSize()-2;i++)
		if((U_Knot[i]<=u)&&(U_Knot[i+1]>u))
		{	pos_u=i;break;	}
	
	for(i=0;i<V_Knot.GetSize()-2;i++)
		if((V_Knot[i]<=v)&&(V_Knot[i+1]>v))
		{	pos_v=i;break;}

	x=0;y=0;z=0;i=1;
	for(j=pos_v-degreev;j<=pos_v;j++)
	{
		d_base_v[i]=BsBase(j,degreev,v,V_Knot);
		i++;
	}
	j=1;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		d_base_u[j]=BaseDv2(i,degreeu,u,U_Knot);
		j++;
	}
	int ii=1;
	int jj=1;
	double xx=0,yy=0,zz=0;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		jj=1;xx=0;yy=0;zz=0;
		for(j=pos_v-degreev;j<=pos_v;j++)
		{
			xx=xx+d_base_v[jj]*CtrlPnt[i+j*m].m_x;
			yy=yy+d_base_v[jj]*CtrlPnt[i+j*m].m_y;
			zz=zz+d_base_v[jj]*CtrlPnt[i+j*m].m_z;
			jj++;
		}
		x=x+d_base_u[ii]*xx;
		y=y+d_base_u[ii]*yy;
		z=z+d_base_u[ii]*zz;
		ii++;
	}
	delete []d_base_u;
	delete []d_base_v;
	return;
}

void   CNubSurStruct::CalSufDv2(double &x,double &y,double &z,double u,double v)
{
	//Standardlize();

	/*int kku=1;
	for(int ii=0;ii<U_Knot.GetSize();ii++)
	{
		if(U_Knot.GetAt(ii)==U_Knot.GetAt(ii+1))
		{
			kku=kku+1;	
		}
		else
			break;
	}
	int kkv=1;
	for(ii=0;ii<V_Knot.GetSize();ii++)
	{
		if(V_Knot.GetAt(ii)==V_Knot.GetAt(ii+1))
		{
			kkv=kkv+1;	
		}
		else
			break;
	}*/

	int degreeu=dgr_u;
	int degreev=dgr_v;
	double *d_base_u=new double[degreeu+2];
	double *d_base_v=new double[degreev+2];
	int pos_u,pos_v;

	if(u>U_Knot[U_Knot.GetSize()-1]) return;
	if(v>V_Knot[V_Knot.GetSize()-1]) return;
	if((U_Knot[U_Knot.GetSize()-1]-u)<1.0e-6) u=u-1.0e-6;
	if((V_Knot[V_Knot.GetSize()-1]-v)<1.0e-6) v=v-1.0e-6;
	//if(u>=1) u=u-1.0e-6;
	//if(v>=1) v=v-1.0e-6;
	int i,j;
	for(i=0;i<U_Knot.GetSize()-2;i++)
		if((U_Knot[i]<=u)&&(U_Knot[i+1]>u))
		{	pos_u=i;break;	}
	
	for(i=0;i<V_Knot.GetSize()-2;i++)
		if((V_Knot[i]<=v)&&(V_Knot[i+1]>v))
		{	pos_v=i;break;}

	x=0;y=0;z=0;i=1;
	for(int j=pos_v-degreev;j<=pos_v;j++)
	{
		d_base_v[i]=BaseDv2(j,degreev,v,V_Knot);
		i++;
	}
	j=1;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		d_base_u[j]=BsBase(i,degreeu,u,U_Knot);
		j++;
	}
	int ii=1;
	int jj=1;
	double xx=0,yy=0,zz=0;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		jj=1;xx=0;yy=0;zz=0;
		for(j=pos_v-degreev;j<=pos_v;j++)
		{
			xx=xx+d_base_v[jj]*CtrlPnt[i+j*m].m_x;
			yy=yy+d_base_v[jj]*CtrlPnt[i+j*m].m_y;
			zz=zz+d_base_v[jj]*CtrlPnt[i+j*m].m_z;
			jj++;
		}
		x=x+d_base_u[ii]*xx;
		y=y+d_base_u[ii]*yy;
		z=z+d_base_u[ii]*zz;
		ii++;
	}
	delete []d_base_u;
	delete []d_base_v;
	return;
}

void   CNubSurStruct::CalSufDuv(double &x,double &y,double &z,double u,double v)
{
	//Standardlize();

	/*int kku=1;
	for(int ii=0;ii<U_Knot.GetSize();ii++)
	{
		if(U_Knot.GetAt(ii)==U_Knot.GetAt(ii+1))
		{
			kku=kku+1;	
		}
		else
			break;
	}
	int kkv=1;
	for(ii=0;ii<V_Knot.GetSize();ii++)
	{
		if(V_Knot.GetAt(ii)==V_Knot.GetAt(ii+1))
		{
			kkv=kkv+1;	
		}
		else
			break;
	}*/

	int degreeu=dgr_u;
	int degreev=dgr_v;
	double *d_base_u=new double[degreeu+2];
	double *d_base_v=new double[degreev+2];
	int pos_u,pos_v;

	if(u>U_Knot[U_Knot.GetSize()-1]) return;
	if(v>V_Knot[V_Knot.GetSize()-1]) return;
	if((U_Knot[U_Knot.GetSize()-1]-u)<1.0e-6) u=u-1.0e-6;
	if((V_Knot[V_Knot.GetSize()-1]-v)<1.0e-6) v=v-1.0e-6;
	//if(u>=1) u=u-1.0e-6;
	//if(v>=1) v=v-1.0e-6;
	int i,j;
	for(i=0;i<U_Knot.GetSize()-2;i++)
		if((U_Knot[i]<=u)&&(U_Knot[i+1]>u))
		{	pos_u=i;break;	}
	
	for(i=0;i<V_Knot.GetSize()-2;i++)
		if((V_Knot[i]<=v)&&(V_Knot[i+1]>v))
		{	pos_v=i;break;}

	x=0;y=0;z=0;i=1;
	for(j=pos_v-degreev;j<=pos_v;j++)
	{
		d_base_v[i]=BaseDv1(j,degreev,v,V_Knot);
		i++;
	}
	j=1;
	for(i=pos_u-degreeu;i<=pos_u;i++)
	{
		d_base_u[j]=BaseDv1(i,degreeu,u,U_Knot);
		j++;
	}
	int ii=1;
	int jj=1;
	double xx=0,yy=0,zz=0;
	for(int i=pos_u-degreeu;i<=pos_u;i++)
	{
		jj=1;xx=0;yy=0;zz=0;
		for(int j=pos_v-degreev;j<=pos_v;j++)
		{
			xx=xx+d_base_v[jj]*CtrlPnt[i+j*m].m_x;
			yy=yy+d_base_v[jj]*CtrlPnt[i+j*m].m_y;
			zz=zz+d_base_v[jj]*CtrlPnt[i+j*m].m_z;
			jj++;
		}
		x=x+d_base_u[ii]*xx;
		y=y+d_base_u[ii]*yy;
		z=z+d_base_u[ii]*zz;
		ii++;
	}
	delete []d_base_u;
	delete []d_base_v;
	return;
}

void CNubSurStruct::Normal(double &x,double &y,double &z,double u,double v)
{
	double xu,yu,zu;
	double xv,yv,zv;
	CalSufDu1(xu,yu,zu,u,v);
	CalSufDv1(xv,yv,zv,u,v);
	x=yu*zv-zu*yv;
	y=zu*xv-xu*zv;
	z=xu*yv-yu*xv;
	return;
}

void CNubSurStruct::Curvature(double &K_Gaussian,double &K_Average,double u,double v)
{
	double L,M,N,E,F,G;
	Re_Vector normal,Puu,Puv,Pvv,Pu,Pv;

	Normal(normal.m_x,normal.m_y,normal.m_z,u,v);
	CalSufDu1(Pu.m_x,Pu.m_y,Pu.m_z,u,v);
	CalSufDv1(Pv.m_x,Pv.m_y,Pv.m_z,u,v);
	CalSufDu2(Puu.m_x,Puu.m_y,Puu.m_z,u,v);
	CalSufDv2(Pvv.m_x,Pvv.m_y,Pvv.m_z,u,v);
	CalSufDuv(Puv.m_x,Puv.m_y,Puv.m_z,u,v);
	normal.Normal();
	/*Pu.Normal();
	Pv.Normal();
	Puu.Normal();
	Pvv.Normal();
	Puv.Normal();*/
	E=Pu.m_x*Pu.m_x+Pu.m_y*Pu.m_y+Pu.m_z*Pu.m_z;
	F=Pu.m_x*Pv.m_x+Pu.m_y*Pv.m_y+Pu.m_z*Pv.m_z;
	G=Pv.m_x*Pv.m_x+Pv.m_y*Pv.m_y+Pv.m_z*Pv.m_z;
	L=Puu.m_x*normal.m_x+Puu.m_y*normal.m_y+Puu.m_z*normal.m_z;
	M=Puv.m_x*normal.m_x+Puv.m_y*normal.m_y+Puv.m_z*normal.m_z;
	N=Pvv.m_x*normal.m_x+Pvv.m_y*normal.m_y+Pvv.m_z*normal.m_z;
	K_Gaussian=((L*N-M*M)/(E*G-F*F))/sqrt(fabs((L*N-M*M)/(E*G-F*F)));//UG documenta
	K_Average=(N*E-2*M*F+L*G)/(2*(E*G-F*F));
}

void CNubSurStruct::NormalFlip(void)
{
	CNubSurStruct  temp_nurbs;
	temp_nurbs.Init();
	for(int i=0;i<U_Knot.GetSize();i++)
	{
		temp_nurbs.V_Knot.Add(U_Knot[i]);
	}
	for(int i=0;i<V_Knot.GetSize();i++)
	{
		temp_nurbs.U_Knot.Add(V_Knot[i]);
	}
	temp_nurbs.m=n;
	temp_nurbs.n=m;
	temp_nurbs.dgr_u=dgr_v;
	temp_nurbs.dgr_v=dgr_u;
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
		{
			temp_nurbs.CtrlPnt.Add(CtrlPnt[i+j*m]);
		}
	}
	temp_nurbs.TrimPnt.RemoveAll();
	
	this->Init();
	(*this)=temp_nurbs;
	return;
}

BOOL CNubSurStruct::GetCtrlPnt(int im, int in, double *x, double *y, double *z)
{
	if(in*n+m>m*n)return false;
	*x=CtrlPnt[in*n+im].m_x;
	*y=CtrlPnt[in*n+im].m_y;
	*z=CtrlPnt[in*n+im].m_z;
	return true;
}

BOOL CNubSurStruct::SetCtrlPnt(int im, int in, double x, double y, double z)
{
	if(in*n+m>m*n)return false;
	CtrlPnt[in*n+im].m_x=x;
	CtrlPnt[in*n+im].m_y=y;
	CtrlPnt[in*n+im].m_z=z;
	return true;
}

void	CNubSurStruct::CalLineDt(double u,double v, double detau,double detav,\
							double &x,double &y,double &z)
{
	double xpu, ypu, zpu, xpv, ypv, zpv;
	CalSufDu1(xpu, ypu, zpu, u, v);
	CalSufDv1(xpv, ypv, zpv, u, v);
	x = detau * xpu + detav * xpv;
	y = detau * ypu + detav * ypv;
	z = detau * zpu + detav * zpv;
	return;
}
void	CNubSurStruct::CalLineDc(double u,double v, double detau,double detav,\
							double &x,double &y,double &z)
{
	double xpu, ypu, zpu, xpv, ypv, zpv;
	CalSufDu1(xpu, ypu, zpu, u, v);
	CalSufDv1(xpv, ypv, zpv, u, v);
	double xn, yn, zn, xt, yt, zt;
	CalLineDt(u, v, detau, detav, xt, yt, zt);
	xn = ypu * zpv - ypv * zpu;
	yn = xpv * zpu - xpu * zpv;
	zn = xpu * ypv - xpv * ypu;
	x = yn * zt - yt * zn;
	y = xt * zn - xn * zt;
	z = xn * yt - xt * yn;
	return;
}

///////////////////////////////////////////////////////////////////////////////////
//
BOOL CNubSurStruct::PntProject(double outpx, double outpy, double outpz, double i_u, double i_v, double &uu_p, double &vv_p)
{
	double /*dist,u,w,*/uu,ww,uw,ud,wd;
	double init_u,init_w,delt_u,delt_w;
	bool outpnt=true;
	int times=0;
	Re_Pnt3D repnt;
	Re_Vector ru,rw,d;
	init_u=i_u;
	init_w=i_v;
	double init_x,init_y,init_z;
do{
		CalSufPnt(init_x,init_y,init_z,init_u,init_w);
		d.m_x=outpx-init_x;
		d.m_y=outpy-init_y;
		d.m_z=outpz-init_z;
		CalSufDu1(ru.m_x,ru.m_y,ru.m_z,init_u,init_w);
		CalSufDv1(rw.m_x,rw.m_y,rw.m_z,init_u,init_w);
		uu=ru.m_x*ru.m_x+ru.m_y*ru.m_y+ru.m_z*ru.m_z;
		ww=rw.m_x*rw.m_x+rw.m_y*rw.m_y+rw.m_z*rw.m_z;
		uw=ru.m_x*rw.m_x+ru.m_y*rw.m_y+ru.m_z*rw.m_z;
		ud=ru.m_x*d.m_x+ru.m_y*d.m_y+ru.m_z*d.m_z;
		wd=rw.m_x*d.m_x+rw.m_y*d.m_y+rw.m_z*d.m_z;
		delt_u=(ud*ww-wd*uw)/(uu*ww-uw*uw);
		delt_w=(wd*uu-ud*uw)/(uu*ww-uw*uw);
		init_u+=delt_u;
		init_w+=delt_w;
		times++; ////////
		if(init_u>1.0 || init_w>1.0 || init_u<0|| init_w<0)
		{
			outpnt=false;
			break;	
		}   
	}while(times<100&&(fabs(delt_u)>0.001 ||fabs(delt_w)>0.001)); //lon!!
	if(outpnt)
	{
		uu_p=init_u;
		vv_p=init_w;
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  CNubSurStruct::CalPntZ(double &x,double &y,double &z,double & u,double & v)
{
	double Utmp,Vtmp;
	double Xtmp,Ytmp,Ztmp;
	double delu,delv,A,B,C,D,E,F;
	int times;
	//用牛顿法计算解非线形方程组，见数值分析199页
	int maxn=0;  
	srand( (unsigned)time( NULL ) );

deb:  maxn++;
	times=0;
	Utmp=0.5;//(float)rand()/RAND_MAX;//用随机数给Utmp和Vtmp的初值 
	Vtmp=0.5;//(float)rand()/RAND_MAX;   
loop: times++;  
	CalSufDu1(Xtmp,Ytmp,Ztmp,Utmp,Vtmp);
	A=Xtmp;
	B=Ytmp;

	CalSufDv1(Xtmp,Ytmp,Ztmp,Utmp,Vtmp);
	C=Xtmp;
	D=Ytmp;
	
	CalSufPnt(Xtmp,Ytmp,Ztmp,Utmp,Vtmp);
	E=Xtmp-x;
	F=Ytmp-y;

	//解方程组得到delu，delv
	if(fabs(A*B*C*D)>0.0000000000001)
	{
		delu=(F*C-E*D)/(A*D-C*B);
		delv=(-E-A*delu)/C;
	}
	else
	{
		if(fabs(A)<0.00000001)
		{
			delv=-E/C;
			delu=(-F-D*delv)/B;
		}
		if(fabs(C)<0.00000001)
		{
			delu=-E/A;
			delv=(-F-B*delu)/D;
		}
	}

	Utmp=Utmp+delu;
	Vtmp=Vtmp+delv;

if (Utmp>0 && Vtmp>0)
{
	if((fabs(delu)>=0.001 || fabs(delv)>=0.001) && times<=100)
	{
		goto loop;
	}
}
	if(fabs(Utmp)<0.0001)
	{
		Utmp=0.0;
	}
	if(fabs(Vtmp)<0.0001)
	{
		Vtmp=0.0;
	}
	if(Utmp>1.0 && Utmp<1.0001)
	{
		Utmp=1.0;
	}
	if(Vtmp>1.0 && Vtmp<1.0001)
	{
		Vtmp=1.0;
	}

	if((Utmp <0.0 || Utmp>1.0 || Vtmp<0.0 || Vtmp>1.0) && maxn<=4)//要特别注意，由于Utmp，Vtmp初值给的不当，有可能
	{                                              //有可能得到的解不在0-1范围内，这时需要重新给
		//初值再算  
		goto deb;
	}
	
	if (Utmp>0 && Vtmp>0)
	   CalSufPnt(x,y,z,Utmp,Vtmp);

	if (maxn<4)
	 	return TRUE;
	else
		return FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CNubSurStruct::PrjPnt(const Re_Pnt3D& pnt,Re_Pnt3D& prj_pnt,double& return_u,double& return_w,double& dist,bool& m_outpnt)
{
	double uu,ww,uw,ud,wd;
	double init_u,init_w,delt_u,delt_w;
	bool outpnt=true;
	m_outpnt=true;
	int times=0;
	int laptimes=0;
	Re_Pnt3D repnt;
	Re_Vector ru,rw,d;
	init_u=0.5;
	init_w=0.5;
star:	do{
		CalSufPnt(repnt.m_x,repnt.m_y,repnt.m_z,init_u,init_w);
		d.m_x=pnt.m_x-repnt.m_x;
		d.m_y=pnt.m_y-repnt.m_y;
		d.m_z=pnt.m_z-repnt.m_z;
		CalSufDu1(ru.m_x,ru.m_y,ru.m_z,init_u,init_w);
		CalSufDv1(rw.m_x,rw.m_y,rw.m_z,init_u,init_w);
		uu=ru.m_x*ru.m_x+ru.m_y*ru.m_y+ru.m_z*ru.m_z;
		ww=rw.m_x*rw.m_x+rw.m_y*rw.m_y+rw.m_z*rw.m_z;
		uw=ru.m_x*rw.m_x+ru.m_y*rw.m_y+ru.m_z*rw.m_z;
		ud=ru.m_x*d.m_x+ru.m_y*d.m_y+ru.m_z*d.m_z;
		wd=rw.m_x*d.m_x+rw.m_y*d.m_y+rw.m_z*d.m_z;
		delt_u=(ud*ww-wd*uw)/(uu*ww-uw*uw);
		delt_w=(wd*uu-ud*uw)/(uu*ww-uw*uw);
		init_u+=delt_u;
		init_w+=delt_w;
		if(init_u>1.0 || init_w>1.0 || init_u<1.0e-5 || init_w<1.0e-5)
		{outpnt=false;break;}
		laptimes++;
		if(laptimes>1000) 
			{m_outpnt=false;return false;}//break;}
	}while(fabs(delt_u)>0.001||fabs(delt_w)>0.001);
	times++;
	if(outpnt) 	
		CalSufPnt(prj_pnt.m_x,prj_pnt.m_y,prj_pnt.m_z,init_u,init_w);
	else if (times<4)
	{
		switch(times)
		{
		case 1:	init_u=0.1;	init_w=0.1;
			outpnt=true;
			laptimes=0;
			goto star;
			break;
		case 2: init_u=0.9; init_w=0.1;
			outpnt=true;
			laptimes=0;
			goto star;
			break;
		case 3: init_u=0.1; init_w=0.9;
			outpnt=true;
			laptimes=0;
			goto star;
			break;
		case 4: init_u=0.9; init_w=0.9;
			outpnt=true;
			laptimes=0;
			goto star;
			break;
		}
	}
	else
		return false; //改变初值计算四次还没有计算处结果，则认为无最近点

	dist=sqrt((pnt.m_x-prj_pnt.m_x)*(pnt.m_x-prj_pnt.m_x)+(pnt.m_y-prj_pnt.m_y)*(pnt.m_y-prj_pnt.m_y)\
				+(pnt.m_z-prj_pnt.m_z)*(pnt.m_z-prj_pnt.m_z));
	return_u=init_u;
	return_w=init_w;
	return true;
}
   
/////////////La
void CNubSurStruct::CalCtrlPnt()
{
	De_BSCurveArray s_cur,t_cur;
	De_BSCurve cross_u0,cross_u1,c1;
	De_IntplPnt pnt;
	De_BSCurve u0,v0,u1,v1;
	int i,j;
	double x,y,z;
	double u;
	Re_Pnt3D pt;
	int u_num = U_Knot.GetSize()-2*dgr_u;
	int v_num = V_Knot.GetSize()-2*dgr_v;
	for(j=0;j<v_num;j++) //求U向线控制顶点
	{
		c1.Init();
		for(i=0;i<u_num;i++)
		{
			pnt.m_pt.m_x=PrimPnt[i+j*u_num].m_x;
			pnt.m_pt.m_y=PrimPnt[i+j*u_num].m_y;
			pnt.m_pt.m_z=PrimPnt[i+j*u_num].m_z;
			c1.m_IntplPntArray.Add(pnt);
		}
		CalSufDu1(x,y,z, 0.0, 0.0);
		//jjjj2008, need change
		star_nx=x;
		star_ny=y;
		star_nz=z;
		CalSufDu1(x,y,z, 1.0, 0.0);
		end_nx=-x;
		end_ny=-y;
		end_nz=-z;
	
		CurveDivide=true;
		c1.CalCtrlPnt();
    	CurveDivide=false;
		s_cur.Add(c1);
		
	}

	for(i=0;i<u_num;i++) //求跨界切矢矢端曲线
	{
	
		CalSufDv1(x,y,z, U_Knot[i+dgr_u], 0.0);
		pnt.m_pt.m_x=-x;
		pnt.m_pt.m_y=-y;
		pnt.m_pt.m_z=-z;
		cross_u0.m_IntplPntArray.Add(pnt);
	
		CalSufDv1(x,y,z, U_Knot[i+dgr_u], 1.0);
		pnt.m_pt.m_x=-x;
		pnt.m_pt.m_y=-y;
		pnt.m_pt.m_z=-z;
		cross_u1.m_IntplPntArray.Add(pnt);
		
	}
	CalSufDuv(x,y,z, 0.0, 0.0);
	star_nx=x;
	star_ny=y;
	star_nz=z;
	CalSufDuv(x,y,z, 1.0, 0.0);
	end_nx=x;
	end_ny=y;
	end_nz=z;
	CurveDivide=true;
	cross_u0.CalCtrlPnt();
	CurveDivide=false;
	CalSufDuv(x,y,z,0.0,1.0);
	star_nx=x;
	star_ny=y;
	star_nz=z;
	CalSufDuv(x,y,z,1.0,1.0);
	end_nx=x;
	end_ny=y;
	end_nz=z;
	CurveDivide=true;
	cross_u1.CalCtrlPnt();
	CurveDivide=false;

 	for(i=0;i<u_num+dgr_u-1;i++)//沿V向求曲面控制顶点
	{
		c1.Init();
		for(j=0;j<v_num;j++)
		{
			pnt.m_pt=s_cur[j].m_CtrlPntArray[i];
			c1.m_IntplPntArray.Add(pnt);
		}
		star_nx=-(cross_u0.m_CtrlPntArray[i].m_x);
		star_ny=-(cross_u0.m_CtrlPntArray[i].m_y);
		star_nz=-(cross_u0.m_CtrlPntArray[i].m_z);
		end_nx=cross_u1.m_CtrlPntArray[i].m_x;
		end_ny=cross_u1.m_CtrlPntArray[i].m_y;
		end_nz=cross_u1.m_CtrlPntArray[i].m_z;
		CurveDivide=true;
		c1.CalCtrlPnt();
		CurveDivide=false;
		t_cur.Add(c1);

	}

	for(j=0;j<n;j++)//将得到的控制顶点赋值到CtrlPnt()数组中
	{
		for(i=0;i<m;i++)
		{
			CtrlPnt[j*(u_num+dgr_u-1)+i].m_x=t_cur[i].m_CtrlPntArray[j].m_x;
		    CtrlPnt[j*(u_num+dgr_u-1)+i].m_y=t_cur[i].m_CtrlPntArray[j].m_y;
		    CtrlPnt[j*(u_num+dgr_u-1)+i].m_z=t_cur[i].m_CtrlPntArray[j].m_z;

		}
	}
	
	s_cur[0].KnotStandardization();
	t_cur[0].KnotStandardization();
	U_Knot.RemoveAll();
	V_Knot.RemoveAll();
	U_Knot.Add(0.0); //节点四重
	for(i=0;i<s_cur[0].m_KnotArray.GetSize();i++)
	{
		u=(GLfloat)s_cur[0].m_KnotArray[i].m_u;
		U_Knot.Add(u);
	}
	U_Knot.Add(1.0);
	V_Knot.Add(0.0);
	for(i=0;i<t_cur[0].m_KnotArray.GetSize();i++)
	{
		u=(GLfloat)t_cur[0].m_KnotArray[i].m_u;
		V_Knot.Add(u);
	}

	V_Knot.Add(1.0);
}
/*****************************************************************************/
//function:find the projection point on surface.
//input:out point,initial value of u and v;
//output:the number of boundary curve,u and v value of surface point.
//return:if find the nearest point return ture.else return false.
//
/*****************************************************************************/
bool CNubSurStruct::ProjectPntToSurf(double outpx, double outpy, double outpz,
									 double i_u, double i_v,double &uu_p,
									 double &vv_p,int &curnum)
{
	curnum=-1;
	double uu,ww,uw,ud,wd;
	double init_u,init_w,delt_u,delt_w;
	bool outpnt=true;
	int times=0;
	Re_Pnt3D repnt;
	Re_Vector ru,rw,d;
	init_u=i_u;
	init_w=i_v;
	double init_x,init_y,init_z;
do{
		CalSufPnt(init_x,init_y,init_z,init_u,init_w);
		d.m_x=outpx-init_x;
		d.m_y=outpy-init_y;
		d.m_z=outpz-init_z;
		CalSufDu1(ru.m_x,ru.m_y,ru.m_z,init_u,init_w);
		CalSufDv1(rw.m_x,rw.m_y,rw.m_z,init_u,init_w);
		uu=ru.m_x*ru.m_x+ru.m_y*ru.m_y+ru.m_z*ru.m_z;
		ww=rw.m_x*rw.m_x+rw.m_y*rw.m_y+rw.m_z*rw.m_z;
		uw=ru.m_x*rw.m_x+ru.m_y*rw.m_y+ru.m_z*rw.m_z;
		ud=ru.m_x*d.m_x+ru.m_y*d.m_y+ru.m_z*d.m_z;
		wd=rw.m_x*d.m_x+rw.m_y*d.m_y+rw.m_z*d.m_z;
		delt_u=(ud*ww-wd*uw)/(uu*ww-uw*uw);
		delt_w=(wd*uu-ud*uw)/(uu*ww-uw*uw);
		init_u+=delt_u;
		init_w+=delt_w;
		times++; ///
		if(init_u>1.0 || init_w>1.0 || init_u<0|| init_w<0)
		{
			//记录超出的边界.
			if(init_w<0.0) curnum=1;
			if(init_w>1.0) curnum=2;
			if(init_u<0.0) curnum=3;
			if(init_u>1.0) curnum=4;

			outpnt=false;
			break;	
		}   
	}while(times<100&&(fabs(delt_u)>0.001 ||fabs(delt_w)>0.001));
	if(outpnt)
	{
		uu_p=init_u;
		vv_p=init_w;
		return true;
	}
	else return false;
}


