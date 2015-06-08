// Math_Algorithm.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <math.h>
#include "pnt.h"
#include "Ti_Optimization.h"
// construction  and destruction 
PNT::PNT(void)
	{
	for(int i =0; i < 3; i ++)
		{
		m[i] = 0;
		}
		mcolor = 0;
		mpointsize = 1;
	}
PNT::PNT(const float& x, const float& y, const float&  z)
	{
	m[0] = x;
	m[1] = y;
	m[2] = z;
		mcolor = 0;
		mpointsize = 1;
	}
PNT::PNT(const float* pnt)
	{
	m[0] = pnt[0];
	m[1] = pnt[1];
	m[2] = pnt[2];
		mcolor = 0;
		mpointsize = 1;
	}
PNT::PNT(const PNT& pnt)
	{
	m[0] = pnt[0];
	m[1] = pnt[1];
	m[2] = pnt[2];
	mcolor = pnt.mcolor;
	mpointsize = pnt.mpointsize;
	}

PNT::~PNT(void)
	{
	}
// end construction and destruction 

// overwrite operator 
const float &PNT:: operator[](int index) const
	{
	return m[index];
	}
float & PNT::operator[](int index) 
	{
	return m[index];
	}
	bool PNT:: operator==(const PNT& pnt)const
	{
			PNT v = *this - pnt;
        if(v.Length()<1e-6 )
		return (1);    
    return (0);
	}

PNT& PNT::operator=(const PNT& v)  // assignment
	{
	m[0] = v[0];
	m[1] = v[1];
	m[2] = v[2];
	mcolor = v.mcolor;
	mpointsize = v.mpointsize;
	return *this;
	}
PNT&PNT:: operator+=(const PNT& v) // add a vector
	{
	m[0] += v[0];
	m[1] += v[1];
	m[2] += v[2];
	return *this;
	}
PNT& PNT::operator-=(const PNT& v) // subtract a vector
	{
	m[0] -= v[0];
	m[1] -= v[1];
	m[2] -= v[2];
	return *this;
	}
PNT& PNT::operator*=(const float& aS) // multiply with scalar
	{
	m[0] *= aS;
	m[1] *= aS;
	m[2] *= aS;
	return *this;
	}
PNT& PNT::operator/=(float aS) // divide width scalar
	{
	m[0] /= aS;
	m[1] /= aS;
	m[2] /= aS;
	return *this;
	}
PNT PNT::operator+(const PNT& v)const // addition
	{
	PNT pnt(*this);
	pnt += v;
	return pnt;
	}
PNT PNT::operator-(const PNT& v)const // subtraction
	{
	PNT pnt(*this);
	pnt -= v;
	return pnt;
	}
PNT PNT::operator*(float aS)const // scalar multiplication
	{
	PNT pnt(*this);
	pnt *= aS;
	return pnt;
	}
	PNT __cdecl operator* (float aS, const PNT& aVector) // scalar multiplication
{
	return aVector * aS;
}
double PNT::operator*(const PNT& S) const // dot production
	{
	return (double) (m[0]*S[0] + m[1]*S[1] + m[2]*S[2] );
	}

PNT PNT::operator/(float aS)const // scalar division
	{
	PNT pnt(*this);
	pnt /= aS;
	return pnt;
	}

PNT PNT:: operator-()const // change the sign of the vector
	{
	PNT pnt;
	pnt -= *this;
	return pnt;
	}
PNT PNT::operator ^(const PNT& v)const 
	{
	return PNT(	m[1]*v[2] - m[2]*v[1],
		m[2]*v[0] - m[0]*v[2],
		m[0]*v[1] - m[1]*v[0] );
	}

double PNT::Length() const
	{
	return 	  sqrt((*this)*(*this));
	}
double PNT::Length2D() const
	{
	return 	  sqrt(this->m[0]*m[0]+m[1]*m[1]);
	}

const PNT&PNT::Normalize()
	{
		double  l;
	if((l=Length()) <1e-10)
		return *this;
	*this /= l;
	return *this;
	}

void PNT::Rotate(
			const PNT	& pnt_axis,	 // point on axis
			const PNT	& dir,		// unit direction vector
			const double		& angle		// rotation angle in rad
			)
{
	double s_thita, c_thita, vers_thita;
	if(fabs(angle) < 1e-6)
		return;
	s_thita = sin(angle);
	c_thita = cos(angle);
	vers_thita = 1- c_thita;
	double  trans[3][3];
	double x, y, z; 
	x = dir[0];
	y = dir[1];
	z = dir[2];

	trans[0][0] = x*x*vers_thita+c_thita;
	trans[0][1] = y*x*vers_thita -z*s_thita;
	trans[0][2] = z*x*vers_thita + y*s_thita;	  
	trans[1][0] = x*y*vers_thita + z*s_thita;
	trans[1][1] = y*y*vers_thita + c_thita;
	trans[1][2] = y*z*vers_thita - x*s_thita;
	trans[2][0] = x*z*vers_thita - y*s_thita;
	trans[2][1] = y*z*vers_thita +x*s_thita;
	trans[2][2] = z*z*vers_thita + c_thita;

	float px,py,pz;
	px = m[0];
	py = m[1];
	pz = m[2];
	
	//Æ½ÒÆ±ä»»
	px -= pnt_axis[0];
	py -= pnt_axis[1];
	pz -= pnt_axis[2];

	//ÈÆÖáÐý×ª
	m[0] =	trans[0][0]*px +
			trans[0][1]*py +
			trans[0][2]*pz ;

	m[1] =	trans[1][0]*px +
			trans[1][1]*py +
			trans[1][2]*pz ;

	m[2] =	trans[2][0]*px + 
			trans[2][1]*py + 
			trans[2][2]*pz ;

	m[0] += pnt_axis[0];
	m[1] += pnt_axis[1];
	m[2] += pnt_axis[2];

}

void PNT::Transform_nDirLooksasZ(PNT nDir)
{
	float x,y,z,x0,y0,z0,x1,y1,z1,x2,y2,z2;
	float sinx,cosx,siny,cosy;

	nDir.Normalize();
	x = nDir[0];
	y = nDir[1];
	z = nDir[2];

	if((z*z+y*y)!=0)
	{
		cosx = z/sqrt(z*z+y*y);
		sinx = y/sqrt(z*z+y*y);
	}
	else
	{
		cosx = 1;
		sinx = 0;
	}

	cosy = sqrt(y*y+z*z);
	siny = -x;

	x0 = m[0];
	y0 = m[1];
	z0 = m[2];

	//ÈÆxÖáÐý×ª
	x1 = x0;
	y1 = y0*cosx-z0*sinx;
	z1 = y0*sinx+z0*cosx;

	//ÈÆyÖáÐý×ª
	x2 = z1*siny+x1*cosy;
	y2 = y1;
	z2 = z1*cosy-x1*siny;

	m[0] = x2;
	m[1] = y2;
	m[2] = z2;
}
// Return the angle from current vector to to_vec around normal
//
float PNT::Angle_ToVector(PNT& to_vec, PNT& normal)
{

	PNT temp;
	PNT up;
	PNT vp;

    up=m;
	vp=to_vec;

	up.Normalize();
	vp.Normalize();

	temp = up^vp;

    float mag = temp*normal;

    // Vectors are parallel at 0 or 180 
    if (mag*mag < 1e-8)
    {
	if ( up*vp < 0 )
	    return (float)PIE;
	else
	    return 0;
    }

    int sign = (mag > 0) ? 1 : -1;
    float t = up*vp; 
    if (t > 1.0)
		t = 1.0;
    else if (t < -1.0)
		t = -1.0;
    return (float)(sign*acos(t));

} 		

float PNT::IsPntLeft(const PNT& spnt, const PNT& epnt)
	 {
		 return( (epnt[0] -spnt[0] )*(this->m[1] -spnt[1] ) -\
		 (epnt[1] -spnt[1] )*(m[0] -spnt[0] ));
	 }


double PNT::PNTDisQua( double x[],double A[])
{
		 //	int i,iter,maxiter=1000,multi=3;
		 //	static double x[N]={ 1.5, 6.5, -1.5 };
		 //    double f[3],Df[3][3];
		 ////	Ti_Math * TM;
		 //
		 //	f[0]=x[0]-5.0*x[1]*x[1]+7.0*x[2]*x[2]+12.0;
		 //	f[1]=3.0*x[0]*x[1]+x[0]*x[2]-11.0*x[0];
		 //	f[2]=2.0*x[1]*x[2]+40.0*x[0];
		 //
		 //	Df[0][0]= 1.0;
		 //	Df[0][1]=-10.0*x[1];
		 //	Df[0][2]= 14.0*x[2];
		 //	Df[1][0]= 3.0*x[1]+x[2]-11.0;
		 //	Df[1][1]= 3.0*x[0];
		 //	Df[1][2]= x[0];
		 //	Df[2][0]= 40.0;
		 //	Df[2][1]= 2.0*x[2];
		 //	Df[2][2]= 2.0*x[1];

		 //iter=rnewtns( maxiter, multi, x );

		 //if( iter > maxiter ) 
		 //	AfxMessageBox("It isn't successful!\n");

		 Ti_Optimization algorithm;
	//	 double x[3]={ 1.5, 6.5, -1.5 };
 //double x[3]={ 1.5, 1, 1 };
// double x[3]={ 1.5, 6, -1 };

			double pt[3];
			pt[0]=this->m[0];
			pt[1]=this->m[1];
			pt[2]=this->m[2];
		    algorithm.NLineraSolverNewtns(4,1000,4,x, pt,A);
			double tmp;
			tmp=sqrt((x[0]-pt[0])*(x[0]-pt[0])+(x[1]-pt[1])*(x[1]-pt[1])+(x[2]-pt[2])*(x[2]-pt[2]));
			return(tmp);
	 }


