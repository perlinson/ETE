/**************************************************************************
file name : pnt.h   

File Description :  point/ vector operation 
created by Xiaoming Du 10/26/2004

Modification Record:	
              Dec 5, 2004 Xiaoming add operator == , friend Calculator operator*, Length2D
              Jan 4, 2005 Tian add Rotate(...)		Transform_nDirLooksasZ(PNT nDir)	  

***************************************************************************/
#pragma once
#include "Math_Algorithm.h"
class MATH_ALGORITHM_API  PNT
	{
	public:
		float  m[3];

	public:
		// construction  and destruction 
		PNT(void);			
		PNT(const float& x, const float& y, const float&  z);
		PNT(const float* pnt);
		PNT(const PNT& pnt);
		~PNT(void);
		// end construction and destruction 
		
		//  operator overwrite
		const float & operator[](int index) const;
		float & operator[](int index) ;
		PNT& operator=(const PNT& v);  // assignment
		PNT& operator+=(const PNT& v); // add a vector
		PNT& operator-=(const PNT& v); // subtract a vector
		PNT& operator*=(const float& aS); // multiply with scalar
		PNT& operator/=(float aS); // divide width scalar
		PNT operator+(const PNT& v)const; // addition
		PNT operator-(const PNT& v)const; // subtraction
		PNT operator*(float aS)const; // scalar multiplication
		friend MATH_ALGORITHM_API PNT __cdecl operator* (float aS, const PNT & aVector); // scalar multiplication

		double operator*(const PNT& S) const; // dot production
		PNT operator/(float aS)const; // scalar division
		PNT operator-()const; // change the sign of the vector
		PNT operator ^(const PNT& v)const; // cross product of two vectors
		bool operator==(const PNT& pnt)const;
		
		//Get length of the vector
		double Length()const;
		double Length2D()const;
		//vector normalization 
		const PNT& Normalize();
	
		void Rotate(
			const PNT	& pnt_axis,		// point on axis
			const PNT	& dir,			// unit direction vector
			const double		& angle		// rotation angle in rad
			);
		//make the transformation to ensure nDir align with z axis
		void Transform_nDirLooksasZ(PNT nDir);
		float Angle_ToVector(PNT& to_vec, PNT& normal);
		 float IsPntLeft(const PNT& spnt, const PNT& epnt);

	double PNTDisQua(double x[],double A[]);

		 int mcolor;// if ==0; by cloud
					// else   use color
		 int mpointsize;
	};
