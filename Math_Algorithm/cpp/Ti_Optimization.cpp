#include "stdafx.h"
#include "ti_optimization.h"
#include "ti_memory.h"
#include <stdio.h>
#include<math.h>
#include <assert.h>
//#include "marquarth.h"
#include <malloc.h> 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Ti_Optimization::Ti_Optimization()
{

}

Ti_Optimization::~Ti_Optimization()
{

}
/*-------------------------------------------------------------------------------
 calculate the least squares solution of an overdetermined system of nonlinear equations
 with Marquardt's method 

-------------------------------------------------------------------------------*/
void Ti_Optimization::marquardt(
			   int m,
			   int n, 
			   double**g_pnt,
			   double* const par, 
			   double*& g, 
			   double**v,
			   int (*funct)(int& m, int n, double* const par, double* g,double**g_pnt),
			   void (*jacobian)(int& m, int n, double* const par, double*& g, double **jac,double**g_pnt),
			   double in[], 
			   double out[]
			   )
{
	int		maxfe,fe,it,i,j,err,emergency;
	double	vv,ww,w,mu,res,fpar,fparpres,lambda,lambdamin,p,pw,reltolres,
			abstolres,em[8],*val,*b,*bb,*parpres,**jac,temp;

	val		= allocate_real_vector(1,n);
	b		= allocate_real_vector(1,n);
	bb		= allocate_real_vector(1,n);
	parpres = allocate_real_vector(1,n);
	jac		= allocate_real_matrix(1,m,1,n);
	assert( (val != NULL) &&
		    (b   != NULL) &&
			(bb  != NULL) &&
			(parpres!= NULL)&&
			(jac != NULL)
			);
	vv      = 10.0;
	w		= 0.5;
	mu		= 0.01;
	ww		= (in[6] < 1.0e-7) ? 1.0e-8 : 1.0e-1*in[6];
	em[0] = em[2] = em[6] = in[0];
	em[4] = 10*n;
	reltolres =in[3];
	abstolres=in[4]*in[4];
	maxfe=(int)in[5];
	err=0;
	fe=it=1;
	p=fpar=res=0.0;
	pw = -log(ww*in[0])/2.30;
	if (!(*funct)(m,n,par,g,g_pnt))
	{
		err=3;
		out[4]=fe;
		out[5]=it-1;
		out[1]=err;
		free_real_vector(val,1);
		free_real_vector(b,1);
		free_real_vector(bb,1);
		free_real_vector(parpres,1);
		free_real_matrix(jac,1,m,1);
		return;
	}
	fpar=vecvec(1,m,0,g,g);// norm of residual vector
	out[3]=sqrt(fpar);
	emergency=0;
	it=1;
	do {
		(*jacobian)(m,n,par,g,jac,g_pnt);
		i = qrisngvaldec(jac,m,n,val,v,em);
		if (it == 1)
			lambda = in[6]*vecvec(1,n,0,val,val);
		else
			if (p == 0.0)
				lambda *= w;
		for (i=1; i<=n; i++) 
			b[i] = val[i]*tamvec(1,m,i,jac,g);
		while (1)
		{
			for (i=1; i<=n; i++) 
				bb[i]=b[i]/(val[i]*val[i]+lambda);
			for (i=1; i<=n; i++)
				parpres[i]=par[i]-matvec(1,n,i,v,bb);

			fe++;

			if (fe >= maxfe)
				err=1;
			else
				if (!(*funct)(m,n,parpres,g,g_pnt))
					err=2;
			if (err != 0) 
			{
				emergency = 1;
				break;
			}
			fparpres=vecvec(1,m,0,g,g);
			res=fpar-fparpres;
			if (res < mu*vecvec(1,n,0,b,bb)) 
			{
				p += 1.0;
				lambda *= vv;
				if (p == 1.0) 
				{
					lambdamin=ww*vecvec(1,n,0,val,val);
					if (lambda < lambdamin)
						lambda=lambdamin;
				}
				if (p >= pw)
				{
					err=4;
					emergency=1;
					break;
				}
			} // end if
			else 
			{

				dupvec(1,n,0,par,parpres);
				fpar=fparpres;
				break;
			} // end else
		} // end while
		if (emergency) 
			break;
		it++;
		//jm add
		//if(it > 20) break;
	} 
	while ( 
			(fpar > abstolres) &&   
		    (res > reltolres*fpar+abstolres)
		  );

	for (i=1; i<=n; i++) 
		mulcol(1,n,i,i,jac,v,1.0/(val[i]+in[0]));
	for (i=1; i<=n; i++)
	{
		for (j=1; j<=i; j++) 
			v[i][j]=v[j][i]=mattam(1,n,i,j,jac,jac);
		lambda=lambdamin=val[1];
	}
	for (i=2; i<=n; i++)
	{
		if (val[i] > lambda)
			lambda=val[i];
		else
		{
			if (val[i] < lambdamin) 
				lambdamin=val[i];
		}
	}

	temp=lambda/(lambdamin+in[0]);
	out[7]=temp*temp;
	out[2]=sqrt(fpar);
	out[6]=sqrt(res+fpar)-out[2];
	out[4]=fe;
	out[5]=it-1;
	out[1]=err;

    if(val != NULL)
	{
		free_real_vector(val,1);
		//val = NULL;
	}
	if (b != NULL)
	{
		free_real_vector(b,1);
		//b = NULL;
	}
	if(bb!=NULL)
	{
		free_real_vector(bb,1);
		//bb  = NULL;
	}
	if(parpres != NULL)
	{
		free_real_vector(parpres,1);
		//parpres = NULL;
	}
	if (jac != NULL)
	{
		free_real_matrix(jac,1,m,1);
		//jac = NULL;
	}
}


/*-------------------------------------------------------------------------------
 calculate the least squares solution of an overdetermined system of nonlinear equations
 with Marquardt's method 

-------------------------------------------------------------------------------*/
void Ti_Optimization::MarquardtforConefitting(
			   int m,
			   int n, 
			   double**g_pnt,
			   double* const par, 
			   double*& g, 
			   double**v,
			   int (*funct)(int m, int n, double* const par, double* g,double**g_pnt),
			   void (*jacobian)(int m, int n, double* const par, double*& g, double **jac,double**g_pnt),
			   double in[], 
			   double out[]
			   )
{
	int		maxfe,fe,it,i,j,err,emergency;
	double	vv,ww,w,mu,res,fpar,fparpres,lambda,lambdamin,p,pw,reltolres,
			abstolres,em[8],*val,*b,*bb,*parpres,**jac,temp;

	val		= allocate_real_vector(1,n);
	b		= allocate_real_vector(1,n);
	bb		= allocate_real_vector(1,n);
	parpres = allocate_real_vector(1,n);
	jac		= allocate_real_matrix(1,m,1,n);
	assert( (val != NULL) &&
		    (b   != NULL) &&
			(bb  != NULL) &&
			(parpres!= NULL)&&
			(jac != NULL)
			);
	vv      = 10.0;
	w		= 0.5;
	mu		= 0.01;
	ww		= (in[6] < 1.0e-7) ? 1.0e-8 : 1.0e-1*in[6];
	em[0] = em[2] = em[6] = in[0];
	em[4] = 10*n;
	reltolres =in[3];
	abstolres=in[4]*in[4];
	maxfe=(int)in[5];
	err=0;
	fe=it=1;
	p=fpar=res=0.0;
	pw = -log(ww*in[0])/2.30;
	if (!(*funct)(m,n,par,g,g_pnt))
	{
		err=3;
		out[4]=fe;
		out[5]=it-1;
		out[1]=err;
		free_real_vector(val,1);
		free_real_vector(b,1);
		free_real_vector(bb,1);
		free_real_vector(parpres,1);
		free_real_matrix(jac,1,m,1);
		return;
	}
	fpar=vecvec(1,m,0,g,g);// norm of residual vector
	out[3]=sqrt(fpar);
	emergency=0;
	it=1;
	do {
 		(*jacobian)(m,n,par,g,jac,g_pnt);
		i = qrisngvaldec(jac,m,n,val,v,em);
		if (it == 1)
			lambda = in[6]*vecvec(1,n,0,val,val);
		else
			if (p == 0.0)
				lambda *= w;
		for (i=1; i<=n; i++) 
			b[i] = val[i]*tamvec(1,m,i,jac,g);
		while (1)
		{
			for (i=1; i<=n; i++) 
				bb[i]=b[i]/(val[i]*val[i]+lambda);
			for (i=1; i<=n; i++)
				parpres[i]=par[i]-matvec(1,n,i,v,bb);

	     	//normalization ,this section only used for cone fitting, 
			//when it is used in other situations,it should be removed
			  temp = sqrt(parpres[4]*parpres[4]+parpres[5]*parpres[5]+parpres[6]*parpres[6]);
			  parpres[4] /= temp;
			  parpres[5] /= temp;
			  parpres[6] /= temp;
			/*  temp = parpres[1]*parpres[4]+ parpres[2]*parpres[5]+ parpres[3]*parpres[6];
			  parpres[1] += temp*parpres[4];
			  parpres[2] += temp*parpres[5];
			  parpres[3] += temp*parpres[6];*/
            //end normalization
			fe++;

			if (fe >= maxfe)
				err=1;
			else
				if (!(*funct)(m,n,parpres,g,g_pnt))
					err=2;
			if (err != 0) 
			{
				emergency = 1;
				break;
			}
			fparpres=vecvec(1,m,0,g,g);
			res=fpar-fparpres;
			if (res < mu*vecvec(1,n,0,b,bb)) 
			{
				p += 1.0;
				lambda *= vv;
				if (p == 1.0) 
				{
					lambdamin=ww*vecvec(1,n,0,val,val);
					if (lambda < lambdamin)
						lambda=lambdamin;
				}
				if (p >= pw)
				{
					err=4;
					emergency=1;
					break;
				}
			} // end if
			else 
			{

				dupvec(1,n,0,par,parpres);
				fpar=fparpres;
				break;
			} // end else
		} // end while
		if (emergency) 
			break;
		it++;
	} 
	while ( 
			(fpar > abstolres) &&   
		    (res > reltolres*fpar+abstolres)
		  );

	for (i=1; i<=n; i++) 
		mulcol(1,n,i,i,jac,v,1.0/(val[i]+in[0]));
	for (i=1; i<=n; i++)
	{
		for (j=1; j<=i; j++) 
			v[i][j]=v[j][i]=mattam(1,n,i,j,jac,jac);
		lambda=lambdamin=val[1];
	}
	for (i=2; i<=n; i++)
	{
		if (val[i] > lambda)
			lambda=val[i];
		else
		{
			if (val[i] < lambdamin) 
				lambdamin=val[i];
		}
	}
	temp=lambda/(lambdamin+in[0]);
	out[7]=temp*temp;
	out[2]=sqrt(fpar);
	out[6]=sqrt(res+fpar)-out[2];
	out[4]=fe;
	out[5]=it-1;
	out[1]=err;

    if(val != NULL)
	{
		free_real_vector(val,1);
		//val = NULL;
	}
	if (b != NULL)
	{
		free_real_vector(b,1);
		//b = NULL;
	}
	if(bb!=NULL)
	{
		free_real_vector(bb,1);
		//bb  = NULL;
	}
	if(parpres != NULL)
	{
		free_real_vector(parpres,1);
		//parpres = NULL;
	}
	if (jac != NULL)
	{
		free_real_matrix(jac,1,m,1);
		//jac = NULL;
	}
}
void Ti_Optimization::mulcol(int l, int u, int i, int j, double **a, double **b, double x)
{
    int k;
	for (k =l; k<=u; k++) 
		{
		a[k][i] = b[k][j]*x;
		}
}

void Ti_Optimization::dupvec(int l, int u, int shift, double a[], double b[])
{
	int k;
	for (k =l; k<=u; k++) 
		a[k]=b[k+shift];
}
double Ti_Optimization::vecvec(int l, int u, int shift, double a[], double b[])
{
	int k;
	double s;
	s=0.0;
	for (k=l; k<=u; k++) 
		s += a[k]*b[k+shift];
	return (s);
}
//
double Ti_Optimization::matvec(int l, int u, int i, double **a, double b[])
{
	int k;
	double s;
	s=0.0;
	for (k=l; k<=u; k++) 
		s += a[i][k]*b[k];
	return (s);
}
//
double Ti_Optimization::tamvec(int l, int u, int i, double **a, double b[])
{
	int k;
	double s;
	s=0.0;
	for (k=l; k<=u; k++)
		s += a[k][i]*b[k];
	return (s);
}
double Ti_Optimization::mattam(int l, int u, int i, int j, double **a, double **b)
{
	int k;
	double s;
	s=0.0;
	for (k=l; k<=u; k++) 
		s += a[i][k]*b[j][k];
	return (s);
}


int Ti_Optimization::qrisngvaldecbid(double d[], double b[], int m, int n, double **u,
			     double **v, double em[])
	{
	int n0,n1,k,k1,i,i1,count,max,rnk;
	double tol,bmax,z,x,y,g,h,f,c,s,min;

	tol=em[2]*em[2];
	count=0;
	bmax=0.0;
	max=(int)em[4];
	min=em[6];
	rnk=n0=n;
	do 
		{
		k=n;
		n1=n-1;
		while (1) 
			{
			k--;
			if (k <= 0) break;
			if (fabs(b[k]) >= tol) 
				{
				if (fabs(d[k]) < tol) 
					{
					c=0.0;
					s=1.0;
					for (i=k; i<=n1; i++)
						{
						f=s*b[i];
						b[i] *= c;
						i1=i+1;
						if (fabs(f) < tol)
							break;
						g=d[i1];
						d[i1]=h=sqrt(f*f+g*g);
						c=g/h;
						s = -f/h;
						rotcol(1,m,k,i1,u,c,s);
						}
					break;
					}
				} 
			else 
				{
				if (fabs(b[k]) > bmax) bmax=fabs(b[k]);
				break;
				}
			}//end while
		if (k == n1) 
			{
			if (d[n] < 0.0) 
				{
				d[n] = -d[n];
				for (i=1; i<=n0; i++) v[i][n] = -v[i][n];
				}
			if (d[n] <= min) rnk--;
			n=n1;
			} 
		else 
			{
			count++;
			if (count > max) break;
			k1=k+1;
			z=d[n];
			x=d[k1];
			y=d[n1];
			g = (n1 == 1) ? 0.0 : b[n1-1];
			h=b[n1];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=sqrt(f*f+1.0);
			f=((x-z)*(x+z)+h*(y/((f < 0.0) ? f-g : f+g)-h))/x;
			c=s=1.0;
			for (i=k1+1; i<=n; i++) 
				{
				i1=i-1;
				g=b[i1];
				y=d[i];
				h=s*g;
				g *= c;
				z=sqrt(f*f+h*h);
				c=f/z;
				s=h/z;
				if (i1 != k1) b[i1-1]=z;
				f=x*c+g*s;
				g=g*c-x*s;
				h=y*s;
				y *= c;
				rotcol(1,n0,i1,i,v,c,s);
				d[i1]=z=sqrt(f*f+h*h);
				c=f/z;
				s=h/z;
				f=c*g+s*y;
				x=c*y-s*g;
				rotcol(1,m,i1,i,u,c,s);
				}
			b[n1]=f;
			d[n]=x;
			}
		} while (n > 0);
	em[3]=bmax;
	em[5]=count;
	em[7]=rnk;
	return n;
	}

int Ti_Optimization::qrisngvaldec(
				 double **a,     // the given matrix, exit: the matrix U in the singular value decomposition UDV'
				 int m,         // entry : the number of rows of a
				 int n,         // entry : the number of columns of a, n should satisfy n<=m
				 double val[],   // exit: the singular values
				 double **v,     // exit: the transpose of matrix V in the singular value decomposition
				 double em[8]   
			 // entry:
			 // em[0]: the machine precision
			 // em[2]: the relative precision in the singular values
			 // em[4]: the maximal number of interations to be performed
			 // em[6]: the minimal non-neglectable singular value;
			 // exit:
			 // em[1]: the infinity norm of the matrix
			 // em[3]: the maximal neglected superdiagonal element;
			 // em[5]: the number of iterations performed;
			 // em[7]: the numerical rank of the matrix; i.e. the number of singular values greater than or equal to em[6]
				 )
{
	/*double *allocate_real_vector(int, int);
	void free_real_vector(double *, int);
	void hshreabid(double **, int, int, double [], double [], double []);
	void psttfmmat(double **, int, double **, double []);
	void pretfmmat(double **, int, int, double []);
	int qrisngvaldecbid(double [], double [], int, int, double **,
							double **, double []);*/
	int i = 0;
	double *b;

	b=allocate_real_vector(1,n);
	hshreabid(a,m,n,val,b,em);
	psttfmmat(a,n,v,b);
	pretfmmat(a,m,n,val);
	i=qrisngvaldecbid(val,b,m,n,a,v,em);
	free_real_vector(b,1);
	return i;
}
void Ti_Optimization::rotcol(int l, int u, int i, int j, double **a, double c, double s)
{
	double x, y;
	int t;
	for (t = l; t<=u; t++) 
		{
		x=a[t][i];
		y=a[t][j];
		a[t][i]=x*c+y*s;
		a[t][j]=y*c-x*s;
	}
}
void Ti_Optimization::pretfmmat(double **a, int m, int n, double d[])
{
	/*double tammat(int, int, int, int, double **, double **);
	void elmcol(int, int, int, int, double **, double **, double);*/
	int i,i1,j;
	double g,h;

	for (i=n; i>=1; i--) {
		i1=i+1;
		g=d[i];
		h=g*a[i][i];
		for (j=i1; j<=n; j++) a[i][j]=0.0;
		if (h < 0.0) {
			for (j=i1; j<=n; j++)
				elmcol(i,m,j,i,a,a,tammat(i1,m,i,j,a,a)/h);
			for (j=i; j<=m; j++) a[j][i] /= g;
		} else
			for (j=i; j<=m; j++) a[j][i]=0.0;
		a[i][i] += 1.0;
	}
}
void Ti_Optimization::psttfmmat(double **a, int n, double **v, double b[])
{
	/*double matmat(int, int, int, int, double **, double **);
	void elmcol(int, int, int, int, double **, double **, double);*/
	int i,i1,j;
	double h;

	i1=n;
	v[n][n]=1.0;
	for (i=n-1; i>=1; i--) {
		h=b[i]*a[i][i1];
		if (h < 0.0) {
			for (j=i1; j<=n; j++) v[j][i]=a[i][j]/h;
			for (j=i1; j<=n; j++)
				elmcol(i1,n,j,i,v,v,matmat(i1,n,i,j,a,v));
		}
		for (j=i1; j<=n; j++) v[i][j]=v[j][i]=0.0;
		v[i][i]=1.0;
		i1=i;
	}
}
void Ti_Optimization::hshreabid(double **a, int m, int n, double d[], double b[],
					double em[])
	{

	int i,j,i1;
	double norm,machtol,w,s,f,g,h;

	norm=0.0;
	for (i=1; i<=m; i++)
		{
		w=0.0;
		for (j=1; j<=n; j++) 
			w += fabs(a[i][j]);
		if (w > norm)
			norm=w;
		}
	machtol=em[0]*norm;
	em[1]=norm;
	for (i=1; i<=n; i++) 
		{
		i1=i+1;
		s=tammat(i1,m,i,i,a,a);
		if (s < machtol)
			d[i]=a[i][i];
		else
			{
			f=a[i][i];
			s += f*f;
			d[i] = g = (f < 0.0) ? sqrt(s) : -sqrt(s);
			h=f*g-s;
			a[i][i]=f-g;
			for (j=i1; j<=n; j++)
				elmcol(i,m,j,i,a,a,tammat(i,m,i,j,a,a)/h);
			}
		if (i < n) 
			{
			s=mattam(i1+1,n,i,i,a,a);
			if (s < machtol)
				b[i]=a[i][i1];
			else 
				{
				f=a[i][i1];
				s += f*f;
				b[i] = g = (f < 0.0) ? sqrt(s) : -sqrt(s);
				h=f*g-s;
				a[i][i1]=f-g;
				for (j=i1; j<=m; j++)
					elmrow(i1,n,j,i,a,a,mattam(i1,n,i,j,a,a)/h);
				}
			}
		}
	}
void Ti_Optimization::elmcol(int l, int u, int i, int j, double **a, double **b, double x)
{
    int k;
    for (k =l; k<=u; k++) a[k][i] += b[k][j]*x;
}
double Ti_Optimization::tammat(int l, int u, int i, int j, double **a, double **b)
{
	int k;
	double s;
	s=0.0;
	for (k=l; k<=u; k++) 
		s += a[k][i]*b[k][j];
	return (s);
}

double Ti_Optimization::matmat(int l, int u, int i, int j, double **a, double **b)
{
	int k;
	double s;
	s=0.0;
	for (k=l; k<=u; k++)
		s += a[i][k]*b[k][j];
	return (s);
}
void Ti_Optimization::elmrow(int l, int u, int i, int j, double **a, double **b, double x)
{
	int k;
	for (k=l; k<=u; k++)  
		a[i][k] += b[j][k]*x;
}



/*-------------------------------------------------------------------------------
 calculate the least squares solution of an overdetermined system of nonlinear equations
 with Marquardt's method 

-------------------------------------------------------------------------------*/
void Ti_Optimization::MarquardtforCylinderFitting(
			   int m,
			   int n, 
			   double**g_pnt,
			   double* const par, 
			   double*& g, 
			   double**v,
			   int (*funct)(int m, int n, double* const par, double* g,double**g_pnt),
			   void (*jacobian)(int m, int n, double* const par, double*& g, double **jac,double**g_pnt),
			   double in[], 
			   double out[]
			   )
{
	int		maxfe,fe,it,i,j,err,emergency;
	double	vv,ww,w,mu,res,fpar,fparpres,lambda,lambdamin,p,pw,reltolres,
			abstolres,em[8],*val,*b,*bb,*parpres,**jac,temp;

	val		= allocate_real_vector(1,n);
	b		= allocate_real_vector(1,n);
	bb		= allocate_real_vector(1,n);
	parpres = allocate_real_vector(1,n);
	jac		= allocate_real_matrix(1,m,1,n);
	assert( (val != NULL) &&
		    (b   != NULL) &&
			(bb  != NULL) &&
			(parpres!= NULL)&&
			(jac != NULL)
			);
	vv      = 10.0;
	w		= 0.5;
	mu		= 0.01;
	ww		= (in[6] < 1.0e-7) ? 1.0e-8 : 1.0e-1*in[6];
	em[0] = em[2] = em[6] = in[0];
	em[4] = 10*n;
	reltolres =in[3];
	abstolres=in[4]*in[4];
	maxfe=(int)in[5];
	err=0;
	fe=it=1;
	p=fpar=res=0.0;
	pw = -log(ww*in[0])/2.30;
	if (!(*funct)(m,n,par,g,g_pnt))
	{
		err=3;
		out[4]=fe;
		out[5]=it-1;
		out[1]=err;
		free_real_vector(val,1);
		free_real_vector(b,1);
		free_real_vector(bb,1);
		free_real_vector(parpres,1);
		free_real_matrix(jac,1,m,1);
		return;
	}
	fpar=vecvec(1,m,0,g,g);// norm of residual vector
	out[3]=sqrt(fpar);
	emergency=0;
	it=1;
	do {
 		(*jacobian)(m,n,par,g,jac,g_pnt);
		i = qrisngvaldec(jac,m,n,val,v,em);
		if (it == 1)
			lambda = in[6]*vecvec(1,n,0,val,val);
		else
			if (p == 0.0)
				lambda *= w;
		for (i=1; i<=n; i++) 
			b[i] = val[i]*tamvec(1,m,i,jac,g);
		while (1)
		{
			for (i=1; i<=n; i++) 
				bb[i]=b[i]/(val[i]*val[i]+lambda);
			for (i=1; i<=n; i++)
				parpres[i]=par[i]-matvec(1,n,i,v,bb);

	     	//normalization ,this section only used for cylinder fitting, 
			//when it is used in other situations,it should be removed
			  temp = sqrt(parpres[4]*parpres[4]+parpres[5]*parpres[5]+parpres[6]*parpres[6]);
			  parpres[4] /= temp;
			  parpres[5] /= temp;
			  parpres[6] /= temp;
            //end normalization
			fe++;
			if (fe >= maxfe)
				err=1;
			else
				if (!(*funct)(m,n,parpres,g,g_pnt))
					err=2;
			if (err != 0) 
			{
				emergency = 1;
				break;
			}
			fparpres=vecvec(1,m,0,g,g);
			res=fpar-fparpres;
			if (res < mu*vecvec(1,n,0,b,bb)) 
			{
				p += 1.0;
				lambda *= vv;
				if (p == 1.0) 
				{
					lambdamin=ww*vecvec(1,n,0,val,val);
					if (lambda < lambdamin)
						lambda=lambdamin;
				}
				if (p >= pw)
				{
					err=4;
					emergency=1;
					break;
				}
			} // end if
			else 
			{

				dupvec(1,n,0,par,parpres);
				fpar=fparpres;
				break;
			} // end else
		} // end while
		if (emergency) 
			break;
		it++;
	} 
	while ( 
			(fpar > abstolres) &&   
		    (res > reltolres*fpar+abstolres)
		  );

	for (i=1; i<=n; i++) 
		mulcol(1,n,i,i,jac,v,1.0/(val[i]+in[0]));
	for (i=1; i<=n; i++)
	{
		for (j=1; j<=i; j++) 
			v[i][j]=v[j][i]=mattam(1,n,i,j,jac,jac);
		lambda=lambdamin=val[1];
	}
	for (i=2; i<=n; i++)
	{
		if (val[i] > lambda)
			lambda=val[i];
		else
		{
			if (val[i] < lambdamin) 
				lambdamin=val[i];
		}
	}
	temp=lambda/(lambdamin+in[0]);
	out[7]=temp*temp;
	out[2]=sqrt(fpar);
	out[6]=sqrt(res+fpar)-out[2];
	out[4]=fe;
	out[5]=it-1;
	out[1]=err;

    if(val != NULL)
	{
		free_real_vector(val,1);
		val = NULL;
	}
	if (b != NULL)
	{
		free_real_vector(b,1);
		b = NULL;
	}
	if(bb!=NULL)
	{
		free_real_vector(bb,1);
		bb  = NULL;
	}
	if(parpres != NULL)
	{
		free_real_vector(parpres,1);
		parpres = NULL;
	}
	if (jac != NULL)
	{
		free_real_matrix(jac,1,m,1);
		jac = NULL;
	}
}


//distance Calculator writing for 3D circle
int Evaluatefor2DCircle(int& m,			/*point number*/
					  int n,			/*the number of unknown variables, for circle=7*/
					  double *const par, /*array of unknown variables
										 Par[1--3], circle center 
										 Par[4--6], circle plane vector
										 Par[7], circle radius*/
					  double* rv,		/*distance Calculator*/
					  double**g_pnt)	/*point array*/
{
	double norm;
	for( int i=1 ; i <= m; i++)
	{
		norm =0;
		for (int j=1; j<=2; j++)
		{
			norm +=(g_pnt[i][j]-par[j])*(g_pnt[i][j]-par[j]);
		} 
		norm = sqrt(norm);	

		rv[i] = norm - par[3];
	}

	return 1;
}


//derivatives for distance Calculator
void Jacobianfor2DCircle(
			  int& m,					// number of points 
			  int n,					/*the number of unknown variables, for circle=7*/
			  double *const  par,	   /*array of unknown variables
										 Par[1--3], circle center 
										 Par[4--6], circle plane vector
										 Par[7], circle radius*/
			  double*& rv,				/*distance Calculator*/
			  double **jac,				/*jacobian */
			  double **g_pnt			/*point array*/
			  )
{
	double norm, norm1;
	
	for( int i=1 ; i <= m; i++)
	{
		norm =0;
		for (int j=1; j<=2; j++)
		{
			norm +=(g_pnt[i][j]-par[j])*(g_pnt[i][j]-par[j]);
		} 
		norm = sqrt(norm);	

		norm1 = norm - par[3];

		jac[i][1] = -(g_pnt[i][1] - par[1])/(norm1 + par[3]);
		jac[i][2] = -(g_pnt[i][2] - par[2])/(norm1 + par[3]);
		jac[i][3] = -1;
	}		
	
}



int  Ti_Optimization::D2Circle_fitting(int m,				// number of points
				   double**g_pnt,		/*point array*/
				   double* const par)	/*array of unknown variables
										 Par[1--3], circle center 
										 Par[4--6], circle plane vector
										 Par[7], circle radius*/
{
	/*/begin parameter initialization*/
    int i;
	//initialize the center point
	par[1] = par[2] = par[3] = 0;
	for (int i=1; i <= m; i++)
	{
		par[1] += g_pnt[i][1];
		par[2] += g_pnt[i][2];

	}
	par[1] /= m;
	par[2] /= m;


	/*initialize the circle radius*/

	par[3] = sqrt((g_pnt[1][1] - par[1])*(g_pnt[1][1] - par[1]) +
				  (g_pnt[1][2] - par[2])*(g_pnt[1][2] - par[2]));	

/*	par[1] = 0.1;
	par[2] = 0;
	par[3] = 0;
*/
	// end initialization

	//Ti_Optimization algorithm;
	double temp = 0;
	double in[7],out[8],*rv,**jjinv;
	rv	  = allocate_real_vector(1,m);
	jjinv = allocate_real_matrix(1,3,1,3);//7 stand for the number of variables
	
	in[0]=1.0e-30; 
	in[3]=1.0e-10; 
	in[4]=1.0e-10;  
	in[5]=2000;
	in[6]=1.0e-6;

	marquardt(
		m,
		3,
		g_pnt,
		par,
		rv,
		jjinv,
		Evaluatefor2DCircle,
		Jacobianfor2DCircle,
		in,
		out);


	/*calculate the average errors*/
	double ave_error=0, temp_center[3];

	temp_center[0] = par[1];
	temp_center[1] = par[2];


	for (i=1; i <= m; i++)
		ave_error += sqrt((g_pnt[i][1] - par[1])*(g_pnt[i][1] - par[1]) +
						  (g_pnt[i][2] - par[2])*(g_pnt[i][2] - par[2])) - par[3];
	ave_error /= m;

	/*free the memory*/
    if (jjinv != NULL)
	{
		free_real_matrix(jjinv,1,3,1);///
		jjinv = NULL;
	}
	if( rv != NULL)
	{
		free_real_vector(rv,1);
		rv = NULL;
	}

	return 0;

}


void  Ti_Optimization::GaussSeidelIteration(int a, int b,double *x0,double   *c,double   *d,double   eps)   
{   
	int   k,i;   
	double   *y=new   double[a],*x=new   double[a],s,temp=0.0;   
	int m=1000;

	for(i=0;i<a;i++)   
	{   
		x[i]=x0[i];   
		y[i]=x[i];   
	}   
	k=1;   
	while(1)   
	{   
		temp=0.0;   
		for(i=0;i<a;i++)   
		{   
			s=0.0;   
			for(int   j=0;j<a;j++)   
			{   
				if(j!=i)   
				{   
					s+=c[i*a+j]*y[j];   
				}   
			}   
			s=(d[i]-s)/c[i*a+i];   
			y[i]=s;   
			if(fabs(x[i]-s)>temp)   
			{   
				temp=fabs(x[i]-s);   
			}   
		}   
		if(temp<eps)   
		{   
/*			cout<<"迭代成功!迭代结果为："<<endl;   
			for(i=0;i<a;i++)   
			{   
				cout<<"x["<<i<<"]   ="<<y[i]<<endl;   
			} */  
			break;   
		}   
		if(k==m)   
		{   
		//	cout<<"迭代失败!!"<<endl;   
			break;   
		}   
		k+=1;   
		for(i=0;i<a;i++)   
		{   
			x0[i]=y[i];   
		}   
	}   
}

void Ti_Optimization::brmul(double a[],double b[],int m,int n,int k,double c[])
{
	int i,j,l,u;

	for (i=0; i<=m-1; i++)
		for (j=0; j<=k-1; j++)
		{ 
			u=i*k+j; c[u]=0.0;
			for (l=0; l<=n-1; l++)
				c[u]=c[u]+a[i*n+l]*b[l*k+j];
		}
}

void Ti_Optimization::GetEigenvector(double a[], int n, double v[], double eps)
{
	int i,j,p,q,u,w,t,s;
    double ff,fm,cn,sn,omega,x,y,d;
    for (i=0; i<=n-1; i++)
      { v[i*n+i]=1.0;
        for (j=0; j<=n-1; j++)
          if (i!=j) v[i*n+j]=0.0;
      }
    ff=0.0;
    for (i=1; i<=n-1; i++)
    for (j=0; j<=i-1; j++)
      { d=a[i*n+j]; ff=ff+d*d; }
    ff=sqrt(2.0*ff);
    loop0:
    ff=ff/(1.0*n);
    loop1:
        for (i=1; i<=n-1; i++)
        for (j=0; j<=i-1; j++)
          { d=fabs(a[i*n+j]);
            if (d>ff)
              { p=i; q=j;
                goto loop;
              }
          }
        if (ff<eps) return;
        goto loop0;
  loop: u=p*n+q; w=p*n+p; t=q*n+p; s=q*n+q;
        x=-a[u]; y=(a[s]-a[w])/2.0;
        omega=x/sqrt(x*x+y*y);
        if (y<0.0) omega=-omega;
        sn=1.0+sqrt(1.0-omega*omega);
        sn=omega/sqrt(2.0*sn);
        cn=sqrt(1.0-sn*sn);
        fm=a[w];
        a[w]=fm*cn*cn+a[s]*sn*sn+a[u]*omega;
        a[s]=fm*sn*sn+a[s]*cn*cn-a[u]*omega;
        a[u]=0.0; a[t]=0.0;
        for (j=0; j<=n-1; j++)
        if ((j!=p)&&(j!=q))
          { u=p*n+j; w=q*n+j;
            fm=a[u];
            a[u]=fm*cn+a[w]*sn;
            a[w]=-fm*sn+a[w]*cn;
          }
        for (i=0; i<=n-1; i++)
          if ((i!=p)&&(i!=q))
            { u=i*n+p; w=i*n+q;
              fm=a[u];
              a[u]=fm*cn+a[w]*sn;
              a[w]=-fm*sn+a[w]*cn;
            }
        for (i=0; i<=n-1; i++)
          { u=i*n+p; w=i*n+q;
            fm=v[u];
            v[u]=fm*cn+v[w]*sn;
            v[w]=-fm*sn+v[w]*cn;
          }
       goto loop1;

}


int Ti_Optimization::NLineraSolverNewtns( int DIM, int maxiter, int multi, double x[],double pt[],double A[] )
{
	int i,j,k;
	int *ik;
	double EPSILON=1.0e-8;
    int N=DIM; 

	ik=(int*)malloc(DIM*sizeof(int));
	double *fk;
	fk=(double*)malloc(DIM*sizeof(double));
	double **Df;
	Df=allocate_real_matrix(0,DIM,0,DIM);


	static double delta,pelement;
	k=0;
	ff(  pt, A,x, fk );
	again:k=k+1;
	fd( pt, A,x, Df );
	if( doolicp( DIM, Df, ik ) == 1 )
	{
	//	AfxMessageBox(" The matrix Df is a singular! Strike any key to exit!");
		return(1);
	}
	for(j=0;j<multi;j++)
	{
		for(i=0;i<DIM;i++)
			if( ik[i] != i )
			{ pelement=fk[i]; fk[i]=fk[ik[i]]; fk[ik[i]]=pelement; }
			trilr( DIM, Df, fk );
			for(i=0;i<N;i++) x[i]=x[i]-fk[i];
			ff( pt, A,x, fk );
	}
	delta=0.0;
	for(i=0;i<N;i++) delta=delta+fk[i]*fk[i];
	delta=sqrt(delta);
	if( k > maxiter ) goto endd;
	if( delta > EPSILON ) goto again;
	endd:return k;

	delete ik;
	delete fk;
	delete Df;

}




