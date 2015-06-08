/////////////////////////////////////////////////////////////////////////////////////////
//			file name : Ti_Rotation.h   
//
//			File Description : 
//				Rotation operation
//			created by Xiaoming Du 10/27/2004
//
//			Modification Record:	
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "math_algorithm.h"
#include "pnt.h"
const  float  DELTA_ROT = 0.000001f;
class  MATH_ALGORITHM_API Ti_Rotation
	{
  	///////////////////////////////////////////////////////////////////////
	//            data 
	////////////////////////////////////////////////////////////////////////
	public:
		PNT		 m_Axis ;	// Unit Vector  
		double	 m_Angle;	// 0 - 360 degree

  	///////////////////////////////////////////////////////////////////////
	//            construction  / destruction 
	////////////////////////////////////////////////////////////////////////
	public:	
		Ti_Rotation(
			const float& fX=0.0f,
			const  float& fY=0.0f, 
			const float&  fZ=1.0f, 
			const double& fAngle=0.0f);

		Ti_Rotation(const PNT& Axis, const double& fAngle=0.0f);
		Ti_Rotation	operator *(const  Ti_Rotation& R2)const;
		PNT  VectorTransform(const PNT& V)const;
		void Convert2RPY(double &zangle, double &yangle, double &xangle);// rhe return angle represent in radian
		/*Get rotation from two vectors , that means v1 will align to v2 after rotation*/
		void RotationFromVecs(const PNT& V1, const PNT& V2);
		void  RotationToMatrix(double  M[4][4]);
		void  MatrixToRotation(double  M[4][4]);
	};


class  QUATERNION
{
public:
  	///////////////////////////////////////////////////////////////////////
	//            data 
	////////////////////////////////////////////////////////////////////////
	double x, y, z, w;

	QUATERNION(double fX, double fY, double fZ, double fW=0.0f)
	{ x=fX; y=fY; z=fZ; w=fW; }
	QUATERNION(void)
	{
	 x = 0; y = 0; z = 1; w = 0;
	}
	~QUATERNION(void)
	{
	}

// 四元组求模，返回值 = |A|
  double QuaternionMagnitude();

// 四元组归一化
// 如果 |A|=0，输出值 = A；输出值 = A/(|A|)
  void  QuaternionNormalize();

// 四元组求逆
// 如果 |A|=0，输出值 = A；否则输出值 = A 的逆
  void  QuaternionInverse(QUATERNION  &A);

// 四元组共扼，输出值 = A 的共扼
  void  QuaternionConjugate(QUATERNION &A);

// 四元组相加，返回值 = A + B
  QUATERNION operator +   (const QUATERNION& B);

// 四元组相减，返回值 = A - B
  QUATERNION operator -   (const QUATERNION& B);

// 四元组缩放，返回值 = s * A 
  QUATERNION operator *  ( const double& s);

// 四元组相乘，返回值 = A * B

  QUATERNION operator *  ( const QUATERNION& B);  

// Spherical Linear Interpolation Between two Quaternions, 0 < t < 1
  QUATERNION QuaternionSlerp(const QUATERNION& from, const QUATERNION& to,const  double& t);

/*_____________________________________________________
 |                                                     
 |   为提高计算性能，以下变换函数均假定
 |
 |   输入参数 R Q V 已经进行了 Normalize 处理    
 |_____________________________________________________*/


/*------------------------------------------------------------------*/
/*----------------------  QUATERNION — Ti_Rotation  ------------------*/

// 将 Ti_Rotation 结构表示成 QUATERNION
  friend QUATERNION RotationToQuaternion(const Ti_Rotation& R);

// 将 QUATERNION 结构表示成 Ti_Rotation
  Ti_Rotation   QuaternionToRotation();

/*------------------------------------------------------------------*/
/*----------------------  QUATERNION — Matrix  --------------------*/

// 将 QUATERNION 结构表示成 Matrix
  void QuaternionToMatrix(double M[4][4]);

// 将 Matrix 结构表示成 QUATERNION
// 要求 Matrix 是一种旋转矩阵,否则不能得到正确结果
  void MatrixToQuaternion( double M[4][4]);

/*------------------------------------------------------------------*/
/*-----------------------  QUATERNION — VECTOR  -------------------*/


// 将矢量（或三维空间的一点）表示成四元组
  QUATERNION VectorToQuaternion(const PNT& V);

// 将四元组的虚部用矢量表示
  PNT     QuaternionToVector();

// V 经过 Q 变换后为 V', 函数返回 V'
  PNT VectorTransform(const PNT& V);
};