// Re_Matrix44.cpp: implementation of the Re_Matrix44 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Re_Matrix44.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Re_Matrix44 Re_Matrix44::operator *(Re_Matrix44 org)
{
	Re_Matrix44 t;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
	{
		t.SetEnum(i,j,
			GetEnum(i,0)*org.GetEnum(0,j)+
			GetEnum(i,1)*org.GetEnum(1,j)+
			GetEnum(i,2)*org.GetEnum(2,j)+
			GetEnum(i,3)*org.GetEnum(3,j));
	}
	return t;
}

Re_Matrix44 Re_Matrix44::Multiple(Re_Matrix44 org)
{
	Re_Matrix44 t;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
	{
		t.SetEnum(i,j,GetEnum(i,0)*org.GetEnum(0,j)+GetEnum(i,1)*org.GetEnum(1,j)+GetEnum(i,2)*org.GetEnum(2,j)+GetEnum(i,3)*org.GetEnum(3,j));
	}
	return t;
}

Re_Matrix44 Re_Matrix44::Reverse(Re_Matrix44 org)
{
	Re_Matrix44 t;
	return t;
}

Re_Matrix44 Re_Matrix44::Sub(Re_Matrix44 org)
{
	Re_Matrix44 t;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
		{
			t.SetEnum(i,j, m_v[j*4+i]-org.GetEnum(i,j));
		}
   return t;
}

Re_Matrix44 Re_Matrix44::operator -(Re_Matrix44 org)
{
	Re_Matrix44 t;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
		{
			t.SetEnum(i,j, m_v[j*4+i]-org.GetEnum(i,j));
		}
   return t;
}

Re_Matrix44 Re_Matrix44::operator +(Re_Matrix44 org)
{
	Re_Matrix44 t;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
		{
			t.SetEnum(i,j, m_v[j*4+i]+org.GetEnum(i,j));
		}
   return t;
}

Re_Matrix44 Re_Matrix44::Add(Re_Matrix44 org)
{
	Re_Matrix44 t;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
		{
			t.SetEnum(i,j, m_v[j*4+i]+org.GetEnum(i,j));
		}
   return t;
}
	
BOOL Re_Matrix44::SetEnum(int row,int column,double val)
{
	if(row>3)
	{
		AfxMessageBox(_T("the first argument must be less than 3."));
		return FALSE;
	}
	if(column>3)
	{
		AfxMessageBox(TEXT("the first argument must be less than 3."));
		return FALSE;
	}
	m_v[column*4+row]=val;
	return TRUE;
}

// row 0-3
// column 0-3
double Re_Matrix44::GetEnum(int row,int column)
{
	if(row>3)
	{
		AfxMessageBox(TEXT("the first argument must be less than 3."));
		return 999999.9;
	}
	if(column>3)
	{
		AfxMessageBox(TEXT("the first argument must be less than 3."));
		return 999999.9;
	}
	return m_v[column*4+row];
}

Re_Matrix44::Re_Matrix44(double a[16])
{
	for(int i=0;i<16;i++)m_v[i]=a[i];
}


Re_Matrix44::Re_Matrix44()
{
	for(int i=0;i<16;i++)m_v[i]=0;
	m_v[0]=m_v[5]=m_v[10]=m_v[15]=1.0;
}

Re_Matrix44::~Re_Matrix44()
{

}
void Re_Matrix44::Identity()
{
	for(int i=0;i<16;i++)m_v[i]=0;
	m_v[0]=m_v[5]=m_v[10]=m_v[15]=1.0;
}

Re_Matrix44& Re_Matrix44::operator =(Re_Matrix44 org)
{
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
		{
			SetEnum(i,j,org.GetEnum(i,j));
		}
   return (*this);
}

Re_Matrix44::Re_Matrix44(Re_Matrix44& org)
{
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
	{
		SetEnum(i,j,org.GetEnum(i,j));
	}
}

// angle ½Ç¶È this*orgin [m4*4]*X[4]=X[4]
void Re_Matrix44::GetMatrixRotate(int axisnum,double angle)
{
	Identity();
	double sida =angle*3.1415926/180.;
	switch(axisnum)
	{
	case 1:   // x Öá
		m_v[5] = cos(sida);
		m_v[6] = sin(sida);
		m_v[9] = -m_v[6];
		m_v[10]= m_v[5];
		break;
	case 2:   // y Öá
		m_v[0]=cos(sida);
		m_v[2]=-sin(sida);
		m_v[8]=-m_v[2];
		m_v[10]=m_v[0];
		break;
	case 3:   // z Öá
		m_v[0]=cos(sida);
		m_v[1]=sin(sida);
		m_v[4]=-m_v[1];
		m_v[5]=m_v[0];
		break;
	default:
		break;
	}
}
void Re_Matrix44::GetMatrixTranslate(double x,double y,double z)
{
	Identity();
	m_v[12]=x;
	m_v[13]=y;
	m_v[14]=z;
}

void Re_Matrix44::GetMatrixScale(double a)
{
	m_v[0]=a*m_v[0];
	m_v[5]=a*m_v[5];
	m_v[10]=a*m_v[10];
	m_v[15]=m_v[15];
}

void Re_Matrix44::GetPoint(double x,double y,double z,double *ox,double *oy,double *oz)
{
	*ox = GetEnum(0,0)*x+GetEnum(0,1)*y+GetEnum(0,2)*z+GetEnum(0,3);
	*oy = GetEnum(1,0)*x+GetEnum(1,1)*y+GetEnum(1,2)*z+GetEnum(1,3);
	*oz = GetEnum(2,0)*x+GetEnum(2,1)*y+GetEnum(2,2)*z+GetEnum(2,3);
}


