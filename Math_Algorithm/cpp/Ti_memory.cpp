#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "..\include\ti_memory.h"
#include <string.h>

void free_integer_matrix( int**& m, int lr, int ur, int lc)
{
	/*  Frees an integer matrix of range [lr..ur][lc..uc].  */

	int i;
	for (i=ur; i>= lr; i--) 
		{
		if((m[i]+lc) != NULL)
		delete [] (m[i]+lc);
		}
	m +=lr;
        if(m!= NULL)
		{
	delete []m;
	m = NULL;
		}
	//m[lr] = NULL;
}


void free_integer_vector(int*& v, int l)
	{
	/*  Frees an integer vector of range [l..u].  */
	v += l;
	if(v != NULL)
		{
		delete [] v;
		v = NULL;
		}
	//v+l = NULL;
	}

void free_real_matrix( double**& m, int lr, int ur, int lc)
{
	/*  Frees a real matrix of range [lr..ur][lc..uc].  */

	int i;
	for (i=ur; i>=lr; i--)
		{
		if((m[i]+lc)!= NULL)
		    delete [](m[i]+lc);
		}
	m += lr;
	if(m!= NULL)
		{
		delete [ ]m;
		m  = NULL;
		}
	//m+lr = NULL;
}

void free_real_vector(double*& v, int l)
	{
	/*  Frees a real vector of range [l..u].  */
	v +=l;
	if(v!= NULL)
		{
		delete [] v;
		v = NULL;
		}
	//v+l = NULL;
	}

int **allocate_integer_matrix(int lr, int ur, int lc, int uc)
{
	/*  Allocates an integer matrix of range [lr..ur][lc..uc].  */
	int i;
	int **p;	
	p = new int*[ur-lr+1];
	assert(p!=NULL);
	p -= lr;
	for (i=lr; i<=ur; i++)
	{
		p[i]= new int [ (uc-lc+1)];
		assert(p[i]!=NULL);// system_error("Failure in allocate_real_matrix().");
		memset(p[i],0,(uc-lc+1)*sizeof(int));
		p[i] -= lc;
	}
	return p;

	//int i;
	//int **p;

	//p=(int **)malloc((unsigned) (ur-lr+1)*sizeof(int*));
	//assert(p!=NULL);
	//p -= lr;

	//for (i=lr; i<=ur; i++)
	//{
	//	p[i]=(int *)malloc((unsigned) (uc-lc+1)*sizeof(int));
	//	assert(p[i] != NULL);// system_error("Failure in allocate_real_matrix().");
	//	memset(p[i],0,(uc-lc+1)*sizeof(int));
	//	p[i] -= lc;
	//}
	//return p;
}

int *allocate_integer_vector(int l, int u)
{
	/*  Allocates an integer vector of range [l..u].  */

	int *p;
	p= new int[(u-l+1)]; //*sizeof(int));
	assert(p!=NULL);
	memset(p, 0, (u-l+1)*sizeof(int));
	p-=l;
	return p;
}

/*  Allocates a real matrix of range [lr..ur][lc..uc].  */
double **allocate_real_matrix(int lr, int ur, int lc, int uc)
{
	int i;
	double **p;

	p=(double **)malloc((unsigned) (ur-lr+1)*sizeof(double*));
	assert(p!=NULL);
	p -= lr;

	for (i=lr; i<=ur; i++)
	{
		p[i]=(double *)malloc((unsigned) (uc-lc+1)*sizeof(double));
		assert(p[i]!=NULL);// system_error("Failure in allocate_real_matrix().");
		memset(p[i],0,(uc-lc+1));
		p[i] -= lc;
	}
	return p;
}

/*
double **allocate_real_matrix(int lr, int ur, int lc, int uc)
{
	int i;
	double **p;

	//p=(double **)malloc((unsigned) (ur-lr+1)*sizeof(double*));
	//assert(p!=NULL);
	//p -= lr;

	//for (i=lr; i<=ur; i++)
	//{
	//	p[i]=(double *)malloc((unsigned) (uc-lc+1)*sizeof(double));
	//	assert(p[i]!=NULL);// system_error("Failure in allocate_real_matrix().");
	//	memset(p[i],0,(uc-lc+1)*sizeof(double));
	//	p[i] -= lc;
	//}

	p = new double*[ur-lr+1];
	assert(p!=NULL);
	p -= lr;
	//for (i=lc; i<=ur; i++)
	for (i=lr; i<=ur; i++)//Zhongguo modified from last sentence to this. I think it is a coding error
	{
		p[i]= new double [ (uc-lc+1)];
		assert(p[i]!=NULL);// system_error("Failure in allocate_real_matrix().");
		//memset(p[i],0,(uc-lc+1)*sizeof(double));
		memset(p[i],0,(uc-lc+1));
		p[i] -= lc;
	}
	return p;
}

*/


double *allocate_real_vector(int l, int u)
{
	/*  Allocates a real vector of range [l..u].  */
	double *p;
	p =  new double[(u-l+1)];
	assert(p !=NULL);
	memset(p,0,(u-l+1)*sizeof(double));
	p -=l;
	return p;
}

