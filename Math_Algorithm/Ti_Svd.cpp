// NAME: 
//   Svd
//
// DESCRIPTION: implementation of Svd class
// FILES: Svd.cpp
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////
#include "stdafx.h"
#include "Ti_Svd1.h"
#include "math.h"
///////////////////////////////////////////////////////////////////////////////
// construction
///////////////////////////////////////////////////////////////////////////////

Svd::Svd()
: m_Row(0)
	{
	mSvdM = 0;
	mSvdW = 0;
	mSvdB = 0;
	mSvdV = 0;
	mSvdStor = 0;
	}
Svd::Svd(int m, int n)
	{
	mSvdM = new double[(m+1)*n+1];
	mSvdW = new double[m+1];
	mSvdB = new double[n+1];
	mSvdV = new double[n*(n+1)+1];
	mSvdStor =new double [m+1];
	m_Row = m;
	m_Col = n;
	}


///////////////////////////////////////////////////////////////////////////////
// destruction
///////////////////////////////////////////////////////////////////////////////

Svd::~Svd()
	{
	if(mSvdM != 0)
		{
		delete[] mSvdM;
		mSvdM = NULL;
		}

	if(mSvdW != 0)
		{
		delete[] mSvdW;
		mSvdW = NULL;
		}
	if(mSvdB != 0)
		{
		delete[] mSvdB;
		mSvdB= NULL;
		}
	if(mSvdV != 0)
		{
		delete[] mSvdV;
		mSvdV = NULL;
		}
	if(mSvdStor != 0)
		{
		delete[] mSvdStor;
		mSvdStor = NULL;
		}
	}



///////////////////////////////////////////////////////////////////////////////
// set matrices
///////////////////////////////////////////////////////////////////////////////

void
Svd::SetSvdM(double* m)
	{
	/*if (mSvdM != 0)
		delete[] mSvdM;*/
	for(int i = 0 ; i <(m_Row+1)*m_Col+1; i++)
		mSvdM[i] = m[i];
	}

void
Svd::SetSvdW(double* w)
	{
	/*if (mSvdW != 0)
		delete[] mSvdW;*/
	for(int i = 0 ; i <=m_Row; i++)
	mSvdW[i] = w[i];
	}

void
Svd::SetSvdB(double* b)
	{
        for(int i = 0 ; i <=m_Col; i++)
	mSvdB[i] = b[i];
	}



///////////////////////////////////////////////////////////////////////////////
// singular value decomposition
// RETURN: true: success    false: failure
///////////////////////////////////////////////////////////////////////////////

//Given a matrix SvdM[1..m][1..n], this routine computes its singular value decomposition, SvdM =
//U¡¤SvdW¡¤SvdV T. Thematrix U replaces SvdM on output. The diagonal matrix of singular values SvdW is output
//as a vector SvdW[1..n]. Thematrix SvdV (not the transpose V T ) is output as SvdV[1..n][1..n].


bool 
Svd::Svdcmp(int m,int n)
	{
	int	    flag,i,its,j,jj,k,l,nm;
	double	c, f, h, s, x, y, z;
	double	anorm, g, scale;
	double	*rv1;


	anorm = 0.0;
	g = 0.0;
	scale = 0.0;
	rv1 = mSvdStor;

	if (m < n)
		return false;

	for(i=1; i<=n; i++)
		{
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m)
			{
			for (k=i; k<=m; k++)	
				scale += fabs(mSvdM[k*n+i]);
			if (scale)
				{
				for (k=i; k<=m; k++)
					{
					mSvdM[k*n+i] /= scale;
					s += mSvdM[k*n+i] * mSvdM[k*n+i];
					}
				f = mSvdM[i*n+i];
				g = (f>=0.0 ? -sqrt(s) : sqrt(s));
				h = f * g - s;
				mSvdM[i*n+i] = f - g;
				if (i != n)
					{
					for (j=l; j<=n; j++)
						{
						for (s=0.0, k=i; k<=m; k++)
							s += mSvdM[k*n+i] * mSvdM[k*n+j];
						f = s/h;
						for ( k=i; k<=m; k++)
							mSvdM[k*n+j] += f * mSvdM[k*n+i];
						}
					}
				for(k=i; k<=m; k++)
					mSvdM[k*n+i] *= scale;
				}
			}
		mSvdW[i] = scale * g;
		g = s = scale = 0.0;
		if (i<=m && i!=n)
			{
			for (k=l; k<=n; k++)
				scale += mSvdM[i*n+k];
			if (scale)
				{
				for ( k=l; k<=n; k++)
					{
					mSvdM[i*n+k] /= scale;
					s += mSvdM[i*n+k] * mSvdM[i*n+k];
					}
				f = mSvdM[i*n+l];
				g = (f>=0.0 ? -sqrt(s) : sqrt(s));
				h = f*g - s;
				mSvdM[i*n+l] = f - g;
				for(k=l; k<=n; k++)
					rv1[k] = mSvdM[i*n+k] / h;
				if (i != m)
					{
					for (j=l; j<=m; j++)
						{
						for (s=0.0, k=l; k<=n; k++)
							s += mSvdM[j*n+k] * mSvdM[i*n+k];
						for (k=l; k<=n; k++)
							mSvdM[j*n+k] += s * rv1[k];
						}
					}
				for (k=l; k<=n; k++)
					mSvdM[i*n+k] *= scale;
				}
			}
		anorm = (anorm > (fabs(mSvdW[i]) + fabs(rv1[i])) ? 
anorm : (fabs(mSvdW[i]) + fabs(rv1[i])));
		}

	// right hand transformations 
	for (i=n; i>=1; i--)
		{
		if (i < n)
			{
			if (g)
				{
				for (j=l; j<=n; j++)	
					mSvdV[j*n+i] = (mSvdM[i*n+j] / mSvdM[i*n+l]) / g;
				for (j=l; j<=n; j++)
					{
					for(s=0.0, k=l; k<=n; k++)
						s += mSvdM[i*n+k] * mSvdV[k*n+j];
					for(k=l; k<=n; k++)
						mSvdV[k*n+j] += s * mSvdV[k*n+i];
					}
				}
			for(j=l; j<=n; j++)
				mSvdV[i*n+j] = mSvdV[j*n+i] = 0.0;
			}
		mSvdV[i*n+i] = 1.0;
		g = rv1[i];
		l = i;
		}

	// left hand transformations 
	for (i=n; i>=1; i--)
		{
		l = i + 1;
		g = mSvdW[i];
		if (i < n)
			{
			for (j=l; j<=n; j++)
				mSvdM[i*n+j] = 0.0;
			}
		if (g)
			{
			g = 1.0 / g;
			if (i != n)
				{
				for (j=l; j<=n; j++)
					{
					for (s=0.0, k=l; k<=m; k++)
						s += mSvdM[k*n+i] * mSvdM[k*n+j];
					f = (s / mSvdM[i*n+i]) * g;
					for (k=i; k<=m; k++)
						mSvdM[k*n+j] += f * mSvdM[k*n+i];
					}
				}
			for(j=i; j<=m; j++)
				mSvdM[j*n+i] *= g;
			} else {
				for (j=i; j<=m; j++)
					mSvdM[j*n+i] = 0.0;
				}
		++ mSvdM[i*n+i];
		}

	// diagonalization of the bidiagonal form
	for (k=n; k>=1; k--)
		{
		for (its=1; its<=30; its++)
			{
			flag = 1;
			for(l=k; l>=1; l--)
				{
				nm = l - 1;
				if (fabs(rv1[l]) + anorm == anorm )
					{
					flag = 0;
					break;
					}
				if (fabs(mSvdW[nm]) + anorm == anorm)
					break;
				}
			if (flag)
				{
				c = 0.0;
				s = 1.0;
				for (i=l; i<=k; i++)
					{
					f = s * rv1[i];
					if (fabs(f) + anorm != anorm )
						{
						g = mSvdW[i];
						h = 
							(fabs(f)>fabs(g) ?
							(fabs(f)*sqrt(1.0+fabs(g)*fabs(g)/fabs(f)/fabs(f))) :
						(fabs(g) ?
							(fabs(g)*sqrt(1.0+fabs(f)*fabs(f)/fabs(g)/fabs(g))):
						0.0));
						mSvdW[i] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j=1; j<=m; j++)
							{
							y = mSvdM[j*n+nm];
							z = mSvdM[j*n+i];
							mSvdM[j*n+nm] = y*c + z*s;
							mSvdM[j*n+i] = z*c - y*s;
							}
						}
					}
				}
			z = mSvdW[k];
			if(l == k)
				{
				if (z < 0.0)
					{
					mSvdW[k] = -z;
					for(j=1; j<=n; j++)
						mSvdV[j*n+k] = -mSvdV[j*n+k];
					}
				break;
				}
			if (its == 30 )
				return false;
			x = mSvdW[l];
			nm = k - 1;
			y = mSvdW[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y-z) * (y + z) + (g-h) * (g+h) ) / (2.0 * h * y);
			g = (fabs(f)>1.0 ? (fabs(f)*sqrt(1.0+1.0/fabs(f)/fabs(f))) :
			(1.0*sqrt(1.0+fabs(f)*fabs(f))));
			f = ((x-z) * (x+z) + h*((y/(f + (f>=0.0 ? fabs(g) : -fabs(g))))
				- h))/x;

			// next QR transformation 
			c = s = 1.0;
			for (j=l; j<=nm; j++)
				{
				i = j + 1;
				g = rv1[i];
				y = mSvdW[i];
				h = s * g;
				g = c * g;
				z = (fabs(f)>fabs(h) ?
					(fabs(f)*sqrt(1.0+fabs(h)*fabs(h)/fabs(f)/fabs(f))) :
				(fabs(h) ?
					(fabs(h)*sqrt(1.0+fabs(f)*fabs(f)/fabs(h)/fabs(h))) :
				0.0));
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj=1; jj<=n; jj++)
					{
					x = mSvdV[jj*n+j];
					z = mSvdV[jj*n+i];
					mSvdV[jj*n+j] = x * c + z * s;
					mSvdV[jj*n+i] = z * c - x * s;
					}
				z = (fabs(f)>fabs(h) ?
					(fabs(f)*sqrt(1.0+fabs(h)*fabs(h)/fabs(f)/fabs(f))) :
				(fabs(h) ?
					(fabs(h)*sqrt(1.0+fabs(f)*fabs(f)/fabs(h)/fabs(h))) :
				0.0));
				mSvdW[j] = z;
				if (z)
					{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
					}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj=1; jj<=m; jj++)
					{
					y = mSvdM[jj*n+j];
					z = mSvdM[jj*n+i];
					mSvdM[jj*n+j] = y * c + z * s;
					mSvdM[jj*n+i] = z * c - y * s;
					}
				}
			rv1[l] = 0.0;
			rv1[k] = f;
			mSvdW[k] = x;
			}
		}
	return true;
	}



void
Svd::Svbksb(int m, int n, double *x)
	{
	int	    jj,j,i;
	double	s, * tmp;


	tmp=mSvdStor;
	for (j=1; j<=n; j++)
		{
		s = 0.0;
		if(mSvdW[j] != 0.0)
			{
			for (i=1; i<=m; i++)
				s += mSvdM[i*n+j] * mSvdB[i];
			s /= mSvdW[j];
			}
		tmp[j] = s;
		}

	for (j=1; j<=n; j++)
		{
		s = 0.0;
		for (jj=1; jj<=n; jj++)
			s += mSvdV[j*n+jj] * tmp[jj];
		x[j] = s;
		}
	}



bool	
Svd::Is_equation_ok(int m, int n, double *x)
	{
	int	    i;
	double	tato;
	double  wmax;


	tato = 1.0e-9;
	wmax=0.0;

	if (mSvdV != 0)
		delete[] mSvdV;
	mSvdV = new double[(m+1)*n+1];
	if (mSvdV == 0)
		return false;

	if (mSvdStor != 0)
		delete[] mSvdStor;
	mSvdStor = new double[m+1];
	if (mSvdStor == 0)
		return false;

	if (!Svdcmp(m, n))
		return false;
	for (i=1; i<=m; i++)	
		{
		if (fabs(mSvdW[i]) > wmax)
			wmax = fabs(mSvdW[i]);
		}
	wmax *= tato;
	for (i=1; i<=m; i++)	
		{
		if (fabs(mSvdW[i]) < wmax)
			mSvdW[i] = 0.0;
		}
	Svbksb(m, n, x);
	return true;
	}

////X////////X//////////////////////X//////////////////////////////////////////
