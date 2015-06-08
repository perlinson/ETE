// NAME: 
//   Svd
//
// DESCRIPTION: definition of Svd class
// FILES: Svd.h
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////
#ifndef _Svd_h_
#define _Svd_h_
#include "math_algorithm.h"

class MATH_ALGORITHM_API Svd  
{
public:

// LIFECYCLE

    Svd();                          // construction
    Svd(int m, int n);
     virtual ~Svd();                 // destruction


// OPERATIONS

    void SetSvdM(
        double* m);

    void SetSvdW(
        double* w);

    void SetSvdB(
        double* b);

    bool Svdcmp(
        int m, int n);

    void Svbksb(
        int m, int n, double* x);

    bool Is_equation_ok(int m, int n, double* x);


private:

    double	* mSvdM, 
				* mSvdW, 
				* mSvdB, 
				* mSvdV, 
				* mSvdStor;
           // matrices of svd calculation

protected:
	int m_Row;
	int m_Col;
};

#endif // _Svd_h_
