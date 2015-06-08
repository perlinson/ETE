/**************************************************************************
file name : Ti_optimization.h   

File Description :  nonlinear optimization functions 
created by Xiaoming Du 12/5/2004

Modification Record:	

***************************************************************************/
#pragma once
#include "Math_Algorithm.h"
#include "math.h"
class MATH_ALGORITHM_API Ti_Optimization  
{
public:
  // objective: calculates the least squares solution of an overdetermined system of nonlinear equations with 
  // Marquardt's method
void marquardt(
			   int m,        //<I>  the number of equations ,in other words, the measured points
			   int n,        //<I>  the number of unknown variables; n should satisfy n<=m
			   double** g_pnt,//<I>  the input points 
			   double* const par,  //<IO> input: the approximation to a least squares solution of 
								  // the system,output: the calculated least squares solution
			   double*& g,    //<O>  the residual vector at the calculated solutions
			   double**v,    //<O>  the inverse of the matrix J'J where J denote the transpose of 
							// the matrix of partial derivatives g'(i)/par'(j)
			   int (*funct)(int& m, int n, double* const par, double* g,double**g_pnt), 
			                 // m,n have the same meaning as above
							 // array par[] contains the current values of the unknowns and should not be altered 
							 // exit: upon completion of a call of funct, the array g[1:m] should contain the 
							 // residual vector obtained with the current values of the unknowns; 
							 // e.g. in curve fitting problems, 
							 // g[i]= theoretical value f(x,par) - observed value y[i]
			                 // after a successful call of funct, the Calculator should deliver the value nonzero; 
							 // however, if funct delivers the value zero then it is assumed that the current 
							 // estimates of the unknowns lie outside a feasible region and the process is 
							 // terminated ; hence , proper programming of funct makes it possible to 
							 // avoid calculation of a residual vector with values					
							 // of the unkonwn variables which make no sense or which even may 
							 // cause overflow in the computaion;
			   void (*jacobian)(int&  m, int n , double* const par, double*& g, double **jac,double**g_pnt),
			                 // m,n par see funct
							 // g contains the residual vector obtained  with the current values of the unknonwns and
							 // should not be altered ;
							 // exit: upon completion of a call of jacobian , the array jac should contain the partial
							 // derivatives g'(i)/par'(j), obtained with the current values of the unknown variables 
							 // given in par[1:n]. it is a prerequisite for the proper operation of the procedure 
							 // marquardt that the precision of the elements of the matrix jac is at least the precision
							 // defined by in[3] and in[4];
			   double in[],   // <I>
			                 // in[0]: the machine precision
							 // in[1],in[2]: these are not used by marquardt
							 // in[3]: the relative tolerance for the difference between 
							 // the Euclidean norm of the ultimate and penultimate residual vector 
							 // in[4]: the absolute tolerence for the difference between 
							 // the Euclidean norm of the ultimate and penultimate residual vector
							 // the process is terminated if the improvement of the sum of squares is 
							 // less than in[3]*(sum of squares)+in[4]*in[4]; 
							 // these tolerence should be chosen greater than the corresponding 
							 // errors of the caculated residual vector
							 // in[5]: the maximum number of calls of funct allowed
							 // in[6]: a starting value used for the relation between 
							 // the gradient and the Gauss-Newton direction (value ¦Îabove);
							 // if the problem is well conditioned then in[6] should be greater,
							 // but the value of in[6] should satisfy:in[0]<in[6]<=1/in[0]

			   double out[]   //<O>
			                 // out[1]: this value gives information about the termination of the process
								// out[1] = 0: normal termination
								// out[1] = 1: the process has been broken off because the number of calls of funct exceeded the number given in in[5]
								// out[1] = 2: the process hase been broken off because a call of funct delivered the value zero
								// out[1] = 3: funct bacame zero when called with the initial estimates of par[1:n];
							           // the interation process was not broken off because the precision asked for cannot be attained ; this precision is possibly chosen too high,relative to the precision in wich the residual vector is calculated
							    // out[1] = 4: the process has been broken off because the precision asked for cann't be attained ; this precision
								//        is possibly chosen too high,relative to the precision in which the residual vector is calculated 
							 // out[2]: the Euclidean norm of the residual vector calculated with values of the unknowns delived
							 // out[3]: the Euclidean norm of the residual vector calculated with the initial values of the unknowns variables
							 // out[4]: the number of calls of funct necessary to attain the calculated result
							 // out[5]: the total number of iterations performed:note that in each interation one evaluation of the Jacobian matrix had to be made
							 // out[6]: the improvement vector in the last interation step
							 // out[7]: the condition number of J'J ,i.e. the ratio of its largest to smallest eigenvalues
			   );
void MarquardtforConefitting(
			   int m,        //<I>  the number of equations ,in other words, the measured points
			   int n,        //<I>  the number of unknown variables; n should satisfy n<=m
			   double** g_pnt,//<I>  the input points 
			   double* const par,  //<IO> input: the approximation to a least squares solution of 
								  // the system,output: the calculated least squares solution
			   double*& g,    //<O>  the residual vector at the calculated solutions
			   double**v,    //<O>  the inverse of the matrix J'J where J denote the transpose of 
							// the matrix of partial derivatives g'(i)/par'(j)
			   int (*funct)(int m, int n, double* const par, double* g,double**g_pnt), 
			                 // m,n have the same meaning as above
							 // array par[] contains the current values of the unknowns and should not be altered 
							 // exit: upon completion of a call of funct, the array g[1:m] should contain the 
							 // residual vector obtained with the current values of the unknowns; 
							 // e.g. in curve fitting problems, 
							 // g[i]= theoretical value f(x,par) - observed value y[i]
			                 // after a successful call of funct, the Calculator should deliver the value nonzero; 
							 // however, if funct delivers the value zero then it is assumed that the current 
							 // estimates of the unknowns lie outside a feasible region and the process is 
							 // terminated ; hence , proper programming of funct makes it possible to 
							 // avoid calculation of a residual vector with values					
							 // of the unkonwn variables which make no sense or which even may 
							 // cause overflow in the computaion;
			   void (*jacobian)(int m, int n , double* const par, double*& g, double **jac,double**g_pnt),
			                 // m,n par see funct
							 // g contains the residual vector obtained  with the current values of the unknonwns and
							 // should not be altered ;
							 // exit: upon completion of a call of jacobian , the array jac should contain the partial
							 // derivatives g'(i)/par'(j), obtained with the current values of the unknown variables 
							 // given in par[1:n]. it is a prerequisite for the proper operation of the procedure 
							 // marquardt that the precision of the elements of the matrix jac is at least the precision
							 // defined by in[3] and in[4];
			   double in[],   // <I>
			                 // in[0]: the machine precision
							 // in[1],in[2]: these are not used by marquardt
							 // in[3]: the relative tolerance for the difference between 
							 // the Euclidean norm of the ultimate and penultimate residual vector 
							 // in[4]: the absolute tolerence for the difference between 
							 // the Euclidean norm of the ultimate and penultimate residual vector
							 // the process is terminated if the improvement of the sum of squares is 
							 // less than in[3]*(sum of squares)+in[4]*in[4]; 
							 // these tolerence should be chosen greater than the corresponding 
							 // errors of the caculated residual vector
							 // in[5]: the maximum number of calls of funct allowed
							 // in[6]: a starting value used for the relation between 
							 // the gradient and the Gauss-Newton direction (value ¦Îabove);
							 // if the problem is well conditioned then in[6] should be greater,
							 // but the value of in[6] should satisfy:in[0]<in[6]<=1/in[0]

			   double out[]   //<O>
			                 // out[1]: this value gives information about the termination of the process
								// out[1] = 0: normal termination
								// out[1] = 1: the process has been broken off because the number of calls of funct exceeded the number given in in[5]
								// out[1] = 2: the process hase been broken off because a call of funct delivered the value zero
								// out[1] = 3: funct bacame zero when called with the initial estimates of par[1:n];
							           // the interation process was not broken off because the precision asked for cannot be attained ; this precision is possibly chosen too high,relative to the precision in wich the residual vector is calculated
							    // out[1] = 4: the process has been broken off because the precision asked for cann't be attained ; this precision
								//        is possibly chosen too high,relative to the precision in which the residual vector is calculated 
							 // out[2]: the Euclidean norm of the residual vector calculated with values of the unknowns delived
							 // out[3]: the Euclidean norm of the residual vector calculated with the initial values of the unknowns variables
							 // out[4]: the number of calls of funct necessary to attain the calculated result
							 // out[5]: the total number of iterations performed:note that in each interation one evaluation of the Jacobian matrix had to be made
							 // out[6]: the improvement vector in the last interation step
							 // out[7]: the condition number of J'J ,i.e. the ratio of its largest to smallest eigenvalues
			   );
void MarquardtforCylinderFitting(
			   int m,        //<I>  the number of equations ,in other words, the measured points
			   int n,        //<I>  the number of unknown variables; n should satisfy n<=m
			   double** g_pnt,//<I>  the input points 
			   double* const par,  //<IO> input: the approximation to a least squares solution of 
								  // the system,output: the calculated least squares solution
			   double*& g,    //<O>  the residual vector at the calculated solutions
			   double**v,    //<O>  the inverse of the matrix J'J where J denote the transpose of 
							// the matrix of partial derivatives g'(i)/par'(j)
			   int (*funct)(int m, int n, double* const par, double* g,double**g_pnt), 
			                 // m,n have the same meaning as above
							 // array par[] contains the current values of the unknowns and should not be altered 
							 // exit: upon completion of a call of funct, the array g[1:m] should contain the 
							 // residual vector obtained with the current values of the unknowns; 
							 // e.g. in curve fitting problems, 
							 // g[i]= theoretical value f(x,par) - observed value y[i]
			                 // after a successful call of funct, the Calculator should deliver the value nonzero; 
							 // however, if funct delivers the value zero then it is assumed that the current 
							 // estimates of the unknowns lie outside a feasible region and the process is 
							 // terminated ; hence , proper programming of funct makes it possible to 
							 // avoid calculation of a residual vector with values					
							 // of the unkonwn variables which make no sense or which even may 
							 // cause overflow in the computaion;
			   void (*jacobian)(int m, int n , double* const par, double*& g, double **jac,double**g_pnt),
			                 // m,n par see funct
							 // g contains the residual vector obtained  with the current values of the unknonwns and
							 // should not be altered ;
							 // exit: upon completion of a call of jacobian , the array jac should contain the partial
							 // derivatives g'(i)/par'(j), obtained with the current values of the unknown variables 
							 // given in par[1:n]. it is a prerequisite for the proper operation of the procedure 
							 // marquardt that the precision of the elements of the matrix jac is at least the precision
							 // defined by in[3] and in[4];
			   double in[],   // <I>
			                 // in[0]: the machine precision
							 // in[1],in[2]: these are not used by marquardt
							 // in[3]: the relative tolerance for the difference between 
							 // the Euclidean norm of the ultimate and penultimate residual vector 
							 // in[4]: the absolute tolerence for the difference between 
							 // the Euclidean norm of the ultimate and penultimate residual vector
							 // the process is terminated if the improvement of the sum of squares is 
							 // less than in[3]*(sum of squares)+in[4]*in[4]; 
							 // these tolerence should be chosen greater than the corresponding 
							 // errors of the caculated residual vector
							 // in[5]: the maximum number of calls of funct allowed
							 // in[6]: a starting value used for the relation between 
							 // the gradient and the Gauss-Newton direction (value ¦Îabove);
							 // if the problem is well conditioned then in[6] should be greater,
							 // but the value of in[6] should satisfy:in[0]<in[6]<=1/in[0]

			   double out[]   //<O>
			                 // out[1]: this value gives information about the termination of the process
								// out[1] = 0: normal termination
								// out[1] = 1: the process has been broken off because the number of calls of funct exceeded the number given in in[5]
								// out[1] = 2: the process hase been broken off because a call of funct delivered the value zero
								// out[1] = 3: funct bacame zero when called with the initial estimates of par[1:n];
							           // the interation process was not broken off because the precision asked for cannot be attained ; this precision is possibly chosen too high,relative to the precision in wich the residual vector is calculated
							    // out[1] = 4: the process has been broken off because the precision asked for cann't be attained ; this precision
								//        is possibly chosen too high,relative to the precision in which the residual vector is calculated 
							 // out[2]: the Euclidean norm of the residual vector calculated with values of the unknowns delived
							 // out[3]: the Euclidean norm of the residual vector calculated with the initial values of the unknowns variables
							 // out[4]: the number of calls of funct necessary to attain the calculated result
							 // out[5]: the total number of iterations performed:note that in each interation one evaluation of the Jacobian matrix had to be made
							 // out[6]: the improvement vector in the last interation step
							 // out[7]: the condition number of J'J ,i.e. the ratio of its largest to smallest eigenvalues
			   );
	void mulcol(int l, int u, int i, int j, double **a, double **b, double x);
    void dupvec(int l, int u, int shift, double a[], double b[]);
	double matvec(int l, int u, int i, double **a, double b[]);
double tamvec(int l, int u, int i, double **a, double b[]);
double mattam(int l, int u, int i, int j, double **a, double **b);
int qrisngvaldecbid(double d[], double b[], int m, int n, double **u,
							double **v, double em[]);
int qrisngvaldec(
				 double **a,     // the given matrix, exit: the matrix U in the singular value decomposition UDV'
				 int m,         // entry : the number of rows of a
				 int n,         // entry : the number of columns of a, n should satisfy n<=m
				 double val[],   // exit: the singular values
				 double **v,     // exit: the transpose of matrix V in the singular value decomposition
				 double em[8]    // entry:
				                // em[0]: the machine precision
								// em[2]: the relative precision in the singular values
								// em[4]: the maximal number of interations to be performed
				 				// em[6]: the minimal non-neglectable singular value;
				                // exit:
								// em[1]: the infinity norm of the matrix
								// em[3]: the maximal neglected superdiagonal element;
								// em[5]: the number of iterations performed;
								// em[7]: the numerical rank of the matrix; i.e. the number of singular values greater than or equal to em[6]
				 );

				 
//distance Calculator writing for 3D circle
friend int __cdecl Evaluatefor2DCircle(int &m,			/*point number*/
					  int n,			/*the number of unknown variables, for circle=7*/
					  double *const par, /*array of unknown variables
										 Par[1--3], circle center 
										 Par[4--6], circle plane vector
										 Par[7], circle radius*/
					  double* rv,		/*distance Calculator*/
					  double**g_pnt);	/*point array*/

//derivatives for distance Calculator
friend void __cdecl Jacobianfor2DCircle(
			  int& m,					// number of points 
			  int n,					/*the number of unknown variables, for circle=7*/
			  double *const  par,	   /*array of unknown variables
										 Par[1--3], circle center 
										 Par[4--6], circle plane vector
										 Par[7], circle radius*/
			  double*& rv,				/*distance Calculator*/
			  double **jac,				/*jacobian */
			  double **g_pnt			/*point array*/
			  );

int D2Circle_fitting(int m,				// number of points
double**g_pnt,		/*point array*/
double* const par);	/*array of unknown variables
						Par[1--3], circle center 
						Par[4--6], circle plane vector
						Par[7], circle radius*/

// Description:
// Given a matrix a, find its eigenvector and eigenvalue.
// The eigenvector is returned by v[] and the eigenvalue by
// a[].
void GetEigenvector(double a[], int n, double v[], double eps);
void GaussSeidelIteration(int a, int b,double *x0,double  *c,double   *d,double   eps);   
void brmul(double a[],double b[],int m,int n,int k,double c[]);

void rotcol(int l, int u, int i, int j, double **a, double c, double s);
double vecvec(int l, int u, int shift, double a[], double b[]);

void pretfmmat(double **a, int m, int n, double d[]);

void psttfmmat(double **a, int n, double **v, double b[]);

void hshreabid(double **a, int m, int n, double d[], double b[],
					double em[]);

void elmcol(int l, int u, int i, int j, double **a, double **b, double x);
double tammat(int l, int u, int i, int j, double **a, double **b);
double matmat(int l, int u, int i, int j, double **a, double **b);
void elmrow(int l, int u, int i, int j, double **a, double **b, double x);


//reform newton method for non-linear equations */
 int NLineraSolverNewtns(  int DIM, int maxiter, int multi, double x[],double pt[],double A[] );


void ff(  double pt[],double A[],double x[], double f[] )
{
	//f[0]=x[0]-5.0*x[1]*x[1]+7.0*x[2]*x[2]+12.0;
	//f[1]=3.0*x[0]*x[1]+x[0]*x[2]-11.0*x[0];
	//f[2]=2.0*x[1]*x[2]+40.0*x[0];

	f[0]=(2+2*A[0]*x[3])*x[0]+A[3]*x[3]*x[1]+A[4]*x[3]*x[2]+A[6]*x[3]-2*pt[0];
	f[1]=(2+2*A[1]*x[3])*x[1]+A[3]*x[3]*x[0]+A[5]*x[3]*x[2]+A[7]*x[3]-2*pt[1];
	f[2]=(2+2*A[2]*x[3])*x[2]+A[4]*x[3]*x[0]+A[5]*x[3]*x[1]+A[8]*x[3]-2*pt[2];
	f[3]=A[0]*x[0]*x[0]+A[1]*x[1]*x[1]+A[2]*x[2]*x[2]+A[3]*x[0]*x[1]+A[4]*x[0]*x[2]+A[5]*x[1]*x[2]+A[6]*x[0]+A[7]*x[1]+A[8]*x[2]+A[9];

}
/********* Calculator fd(  x[N], Df[N][N] ) **************/
void fd(  double pt[],double A[],double x[], double **Df )
{
	//Df[0][0]= 1.0;
	//Df[0][1]=-10.0*x[1];
	//Df[0][2]= 14.0*x[2];
	//Df[1][0]= 3.0*x[1]+x[2]-11.0;
	//Df[1][1]= 3.0*x[0];
	//Df[1][2]= x[0];
	//Df[2][0]= 40.0;
	//Df[2][1]= 2.0*x[2];
	//Df[2][2]= 2.0*x[1];

	Df[0][0]= 2+2*A[0]*x[3];
	Df[0][1]=A[3]*x[3];
	Df[0][2]= A[4]*x[3];
	Df[0][3]= 2*A[0]*x[0]+A[3]*x[1]+A[4]*x[2]+A[6];

	Df[1][0]= A[3]*x[3];
	Df[1][1]=2+2*A[1]*x[3];
	Df[1][2]= A[5]*x[3];
	Df[1][3]= 2*A[1]*x[1]+A[3]*x[0]+A[5]*x[2]+A[7];

	Df[2][0]= A[4]*x[3];
	Df[2][1]= A[5]*x[3];
	Df[2][2]= 2+2*A[2]*x[3];
	Df[2][3]= 2*A[2]*x[2]+A[4]*x[0]+A[5]*x[1]+A[8];

	Df[3][0]= 2*A[0]*x[0]+A[3]*x[1]+A[4]*x[2]+A[6];
	Df[3][1]= 2*A[1]*x[1]+A[3]*x[0]+A[5]*x[2]+A[7];
	Df[3][2]= 2*A[2]*x[2]+A[4]*x[0]+A[5]*x[1]+A[8];
	Df[3][3]= 0;

}

int doolicp( int DIM, double **AA, int ik[] )
{
	int k,i,j,i0;
	double pelement;
	double EPSILON=1.0e-8;

	for(k=0;k<DIM;k++)
	{
		/* for principal element*/
		pelement=fabs( AA[k][k] );       ik[k]=k;
		for(i=k;i<DIM;i++)
			if( fabs(AA[i][k]) > pelement ) { pelement=fabs(AA[i][k]); ik[k]=i; }
			i0=ik[k];
			if( i0 != k )
				for(j=0;j<DIM;j++)
				{ pelement=AA[k][j]; AA[k][j]=AA[i0][j]; AA[i0][j]=pelement; }
				if( fabs(AA[k][k]) < EPSILON ) return(1);
				for(i=k+1;i<DIM;i++)
				{
					AA[i][k]=AA[i][k]/AA[k][k];
					for(j=k+1;j<DIM;j++) AA[i][j]=AA[i][j]-AA[i][k]*AA[k][j];
				}
	}
	return(0);
}
/***** for solution LRx=b, L is unit lower triangular matrix. *****/
int trilr(int DIM, double **AA, double xx[] )
{
	int i,j;
	double EPSILON=1.0e-8;

	for(i=1;i<DIM;i++)
		for(j=0;j<i;j++) xx[i]=xx[i]-AA[i][j]*xx[j];
	for(i=DIM-1;i>=0;i--)
	{
		for(j=i+1;j<DIM;j++) xx[i]=xx[i]-AA[i][j]*xx[j];
		if( fabs( AA[i][i] ) < EPSILON ) return(1);
		xx[i]=xx[i]/AA[i][i];
	}
	return(0);
}



	Ti_Optimization();
	virtual ~Ti_Optimization();

};
