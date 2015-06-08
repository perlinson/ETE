#include "stdafx.h"
#include <math.h>
#include <assert.h>
#include "Ti_Rotation.h"
#include "ti_Matrix.h"
Ti_Rotation::Ti_Rotation(
			const float& fX,
			const  float& fY, 
			const float&  fZ, 
			const double& fAngle)
{
m_Axis[0]=fX; 
m_Axis[1]=fY;
m_Axis[2]=fZ;
m_Angle=fAngle; 
m_Axis.Normalize(); 
}
Ti_Rotation::Ti_Rotation(
			 const PNT& Axis, 
			 const double& fAngle)
{
m_Axis=Axis;
m_Angle=fAngle; 
m_Axis.Normalize(); 
}

Ti_Rotation  Ti_Rotation::operator * (const  Ti_Rotation& R2)const
{
     Ti_Rotation R1 = *this;
       if( (R1.m_Angle>-DELTA_ROT) && (R1.m_Angle<DELTA_ROT) )
	{
		return R2;
	}
	else if( (R2.m_Angle>-DELTA_ROT) && (R2.m_Angle<DELTA_ROT) )
	{
		return R1;
	}
	
	// 原始角度不为 0
	Ti_Rotation   R; 
	QUATERNION Q1= RotationToQuaternion(R1);
	QUATERNION Q2 =RotationToQuaternion(R2);
	QUATERNION Q = Q1*Q2; // 注意顺序
	R=Q.QuaternionToRotation();
	return R;
}


// V 经过 R 变换后为 V', 函数返回 V'
// 要求 R 先归一化
PNT  Ti_Rotation::VectorTransform(const PNT& V)const
{
	Ti_Rotation R = *this;
	if( (R.m_Angle>-DELTA_ROT) && (R.m_Angle<DELTA_ROT) ) 
		return V;
	R.m_Axis.Normalize();
	QUATERNION Q = RotationToQuaternion(R);
	Q.QuaternionNormalize();
	return Q.VectorTransform(V);
}

void Ti_Rotation::Convert2RPY(double &zangle, double &yangle, double &xangle)
	{
	if(fabs(m_Angle)< 1e-6)
		{
		zangle = yangle = xangle = 0;
		}
	double   M[4][4];
	RotationToMatrix(M);
	if(M[0][0]*M[0][0] + M[1][0]*M[1][0] > 1e-8)
		{
		yangle = atan2(-M[2][0], sqrt(M[0][0]*M[0][0] + M[1][0]*M[1][0]));
		zangle = atan2(M[1][0], M[0][0]);
		xangle = atan2(M[2][1], M[2][2]);
		}
	else
		{
		yangle = PIE/2;
		zangle = 0;
		xangle = atan2(M[0][1], M[1][1]);
		}
	}

//PNT  __cdecl VectorTransform( const PNT& V, const Ti_Matrix<double> R)
//{
//	/*if ( R.RowCount()!=3 || R.ColCount()!=3 || T.RowCount()!=3 || T.ColCount()!=1 )
//		return V;*/
//	PNT  nv;
//	nv[0]=R.m[0][0]*V[0]+R.m[0][1]*V[1]+R.m[0][2]*V[2]+R.m[0][3];
//	nv[1]=R.m[1][0]*V[0]+R.m[1][1]*V[1]+R.m[1][2]*V[2]+R.m[1][3];
//	nv[2]=R.m[2][0]*V[0]+R.m[2][1]*V[1]+R.m[2][2]*V[2]+R.m[2][3];
//	return nv;
//}

void Ti_Rotation::RotationFromVecs(const PNT& V1, const PNT& V2)
	{
	if(V1*V2>0.99999999999)
		{
		m_Angle =0;
		return;
		}
	m_Axis = V1^V2;
	m_Axis.Normalize();
	m_Angle = 180*acos((V1*V2)/(V1.Length()*V2.Length()))/PIE;
	}
void Ti_Rotation::MatrixToRotation(double M[4][4])
	{
	QUATERNION  q;
	q.MatrixToQuaternion(M);
	*this = q.QuaternionToRotation();
	}

// 将 Ti_Rotation 结构表示成 Matrix
// 要求 R 先归一化
void Ti_Rotation::RotationToMatrix(double M[4][4])
{
    Ti_Rotation R = *this;
	double x, y, z; 
	double s_thita, c_thita, vers_thita, angle ;	

	int i,j;
	for( i =0; i < 4; i++)
		for(j =0 ; j < 4; j++)
			{
			if(i ==j)
				M[i][j] = 1;
			else
				M[i][j] = 0;
			}
	if( (R.m_Angle>-DELTA_ROT) && (R.m_Angle<DELTA_ROT) )
		{		
		return ;
		}
	angle = R.m_Angle*PIE/180.;
	s_thita = sin(angle);
	c_thita = cos(angle);
	vers_thita = 1- c_thita;	

	R.m_Axis.Normalize();
	x =R.m_Axis[0];
	y =R.m_Axis[1];
	z = R.m_Axis[2];

	M[0][0] = x*x*vers_thita+c_thita;
	M[0][1] = y*x*vers_thita -z*s_thita;
	M[0][2] = z*x*vers_thita + y*s_thita;	 

	M[1][0] = x*y*vers_thita + z*s_thita;
	M[1][1] = y*y*vers_thita + c_thita;
	M[1][2] = y*z*vers_thita - x*s_thita;

	M[2][0] = x*z*vers_thita - y*s_thita;
	M[2][1] = y*z*vers_thita +x*s_thita;
	M[2][2] = z*z*vers_thita + c_thita;	
}


// 四元组求模，返回值 = |A|
double QUATERNION::QuaternionMagnitude()
{
	return sqrt(x*x + y*y + z*z +w*w);
}

// 四元组归一化
// 如果 |A|=0，输出值 = A；输出值 = A/(|A|)
void  QUATERNION::QuaternionNormalize()
{
	double magnitude = double(sqrt(x*x + y*y + z*z +w*w));

	if(magnitude >= DELTA_ROT)
    {
	  x = x/magnitude;
	  y = y/magnitude;
	  z = z/magnitude;
	  w = w/magnitude;
    }
}

// 四元组求逆
// 如果 |A|=0，输出值 = A；否则输出值 = A 的逆
void  QUATERNION::QuaternionInverse(QUATERNION &A)
{
	double magnitude2 = x*x + y*y + z*z + w*w;

	if(magnitude2 >= DELTA_ROT)
    {
	  A.x = -x/magnitude2;
	  A.y = -y/magnitude2;
	  A.z = -z/magnitude2;
	  A.w =  w/magnitude2;
    }
}

// 四元组共扼，输出值 = A 的共扼
void  QUATERNION::QuaternionConjugate(QUATERNION &A)
{
	A.x = -x;
	A.y = -y;
	A.z = -z;
}


QUATERNION QUATERNION::operator+( const QUATERNION& B)
{
	QUATERNION Q;
	Q.x = x + B.x;
	Q.y = y + B.y;
	Q.z = z + B.z;
	Q.w = w + B.w;
	return Q;
}


QUATERNION QUATERNION::operator-( const QUATERNION& B)
{
	QUATERNION Q;
	Q.x = x - B.x;
	Q.y = y - B.y;
	Q.z = z - B.z;
	Q.w = w - B.w;
	return Q;
}


QUATERNION QUATERNION::operator * (const double& s)
{
	QUATERNION Q;
	Q.x = s * x;
	Q.y = s * y;
	Q.z = s * z;
	Q.w = s * w;
	return Q;
}


// 四元组相乘，返回值 = q1 * q2
QUATERNION QUATERNION::operator * (const QUATERNION& q2)
{
	QUATERNION Q;
    Q.x =	w * q2.x + x * q2.w +	y * q2.z - z * q2.y;
    Q.y =	w * q2.y + y * q2.w +	z * q2.x - x * q2.z;
    Q.z =	w * q2.z + z * q2.w +	x * q2.y - y * q2.x;
    Q.w =	w * q2.w - x * q2.x -	y * q2.y - z * q2.z;
	return Q;
/*
// test code
	float  s, s1, s2;
	VECTOR v, v1, v2;

	s1 = q1.w;
	s2 = q2.w;
	v1.x=q1.x; v1.y=q1.y; v1.z=q1.z;
	v2.x=q2.x; v2.y=q2.y; v2.z=q2.z;

	s  = s1*s2 - v1^v2;
	v  = s1*v2 + s2*v1 + v1*v2;
	QUATERNION Q;
	Q.w= s;
	Q.x= v.x;
	Q.y= v.y;
	Q.z= v.z;
	return Q;
*/
/*
// Jeff Lander 编写，变成了 Q2*Q1
	QUATERNION Q;

    Q.x =	q2.w * q1.x + q2.x * q1.w +
			q2.y * q1.z - q2.z * q1.y;
    Q.y =	q2.w * q1.y + q2.y * q1.w +
			q2.z * q1.x - q2.x * q1.z;
    Q.z =	q2.w * q1.z + q2.z * q1.w +
			q2.x * q1.y - q2.y * q1.x;
    Q.w =	q2.w * q1.w - q2.x * q1.x -
			q2.y * q1.y - q2.z * q1.z;
	return Q;
*/

/*
// Nick Bobick 编写，变成了 Q2*Q1

	float A, B, C, D, E, F, G, H;

	A = (q1.w + q1.x) * (q2.w + q2.x);
	B = (q1.z - q1.y) * (q2.y - q2.z);
	C = (q1.x - q1.w) * (q2.y - q2.z);
	D = (q1.y + q1.z) * (q2.x - q2.w);
	E = (q1.x + q1.z) * (q2.x + q2.y);
	F = (q1.x - q1.z) * (q2.x - q2.y);
	G = (q1.w + q1.y) * (q2.w - q2.z);
	H = (q1.w - q1.y) * (q2.w + q2.z);

	QUATERNION Q;
	Q.w =  B + (-E - F + G + H)/2.0f;
	Q.x =  A - ( E + F + G + H)/2.0f; 
	Q.y = -C + ( E - F + G - H)/2.0f;
	Q.z = -D + ( E - F - G + H)/2.0f;
	return Q;
*/
}

// Nick Bobick 编写
// Purpose:		Spherical Linear Interpolation Between two Quaternions
// Arguments:	Two Quaternions, blend factor
QUATERNION QUATERNION::QuaternionSlerp(const QUATERNION& from, const QUATERNION& to, const double& t)
{
	double  to1[4];
        double  omega, cosom, sinom, scale0, scale1;

        // calc cosine
	cosom = from.x * to.x + from.y * to.y + from.z * to.z
		+ from.w * to.w;

	// adjust signs (if necessary)
	if ( cosom <0.0 )
		{ cosom = -cosom; to1[0] = - to.x;
	to1[1] = - to.y;
	to1[2] = - to.z;
	to1[3] = - to.w;
		}
	else  
		{
		to1[0] = to.x;
		to1[1] = to.y;
		to1[2] = to.z;
		to1[3] = to.w;
		}

	// calculate coefficients

	if ( (1.0 - cosom) > DELTA_ROT )
		{
		// standard case (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 =sin((1.0 - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;
		}
	else {        
			// "from" and "to" quaternions are very close 
			//  ... so we can do a linear interpolation
			scale0 = 1.0f - t;
			scale1 = t;
			}
	// calculate final values
	QUATERNION res;
	res.x = scale0 * from.x + scale1 * to1[0];
	res.y = scale0 * from.y + scale1 * to1[1];
	res.z = scale0 * from.z + scale1 * to1[2];
	res.w = scale0 * from.w + scale1 * to1[3];
	return res;
}


/*_____________________________________________________
 |                                                     
 |   为提高计算性能，以下变换函数均假定
 |
 |   输入参数 R Q V 已经进行了 Normalize 处理    
 |_____________________________________________________*/



/*-----------------------------------------------*/
/*-----------  QUATERNION — Ti_Rotation  ----------*/

// 将 Ti_Rotation 结构表示成 QUATERNION
// 假定 Ti_Rotation 参数已经进行了归一化处理
QUATERNION RotationToQuaternion(const Ti_Rotation& R)
{
	QUATERNION Q;
	double cosValue, sinValue, theta;

	theta    = R.m_Angle/180.0f*PIE;
	cosValue = cos(theta/2.0f);  
	sinValue = sin(theta/2.0f);  

	Q.x = sinValue*R.m_Axis[0];
	Q.y = sinValue*R.m_Axis[1];
	Q.z = sinValue*R.m_Axis[2];
	Q.w = cosValue;
	return Q;
}

// 将 QUATERNION 结构表示成 Ti_Rotation
// 假定 QUATERNION 参数已经进行了归一化处理
Ti_Rotation   QUATERNION::QuaternionToRotation()
{
	Ti_Rotation   R; 
	if( ((w>1-DELTA_ROT)&&(w<1+DELTA_ROT)) || 
		((w>-1-DELTA_ROT)&&(w<-1+DELTA_ROT)) ) 
	{
		R.m_Angle=0.0f;
		R.m_Axis[0]=0.0f;
		R.m_Axis[1]=0.0f;
		R.m_Axis[2]=1.0f;
		return R;
	}

	double  sinValue, halfTheta;
	halfTheta= acos(w);
	sinValue = sin(halfTheta);  
	
	if(sinValue <= DELTA_ROT)
	{
		R.m_Angle=0.0f;
		R.m_Axis[0]=0.0f;
		R.m_Axis[1]=0.0f;
		R.m_Axis[2]=1.0f;
		return R;
	}

	R.m_Angle  = halfTheta * 2.0f * 180.0f /PIE;
	R.m_Axis[0] = x/sinValue;
	R.m_Axis[1] = y/sinValue;
	R.m_Axis[2] = z/sinValue;
	R.m_Axis.Normalize();
	return R;
}


/*-------------------------------------------------------------*/
/*-------------------  QUATERNION — Matrix  ------------------*/

// Nick Bobick 编写
// 将 Matrix 结构表示成 QUATERNION
// 要求 Matrix 是一种旋转矩阵,否则不能得到正确结果
void  QUATERNION::MatrixToQuaternion( double M[4][4])
{
	double tr, s, q[4];
	int   i, j, k;
	int	  nxt[3] = {1, 2, 0};

	tr = M[0][0] + M[1][1] + M[2][2];

	// check the diagonal
	if (tr > 0.0) 
	{
		s = sqrt (tr + 1.0);   // s 为什么 !=  -sqrt (tr + 1.0)
		w = s / 2.0f;
		s = 0.5f / s;
		x = (M[1][2] - M[2][1]) * s;
		y = (M[2][0] - M[0][2]) * s;
		z = (M[0][1] - M[1][0]) * s;
	} 
	else 
	{		
	 // diagonal is negative
    	i = 0;
        if (M[1][1] > M[0][0]) i = 1;
	    if (M[2][2] > M[i][i]) i = 2;
        j = nxt[i];
        k = nxt[j];
        s = sqrt ((M[i][i] - (M[j][j] + M[k][k])) + 1.0f);
	    q[i] = s * 0.5f;
        if (s != 0.0f) s = 0.5f / s;
	    q[3] = (M[j][k] - M[k][j]) * s;
        q[j] = (M[i][j] + M[j][i]) * s;
        q[k] = (M[i][k] + M[k][i]) * s;
		x = q[0];
		y = q[1];
		z = q[2];
		w = q[3];
	}
}


// 将 QUATERNION 结构表示成 Matrix
void  QUATERNION::QuaternionToMatrix(double M[4][4])
{

  double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

  // calculate coefficients
  x2 =x * 2.0f; 
  y2 =y * 2.0f; 
  z2 =z * 2.0f;
  xx =x * x2;   xy =x * y2;   xz =x * z2;
  yy =y * y2;   yz =y * z2;   zz =z * z2;
  wx =w * x2;   wy =w * y2;   wz =w * z2;

  M[0][0] = 1.0 - (yy + zz); 	
  M[0][1] = xy + wz;
  M[0][2] = xz - wy;		
  M[0][3] = 0.0;
 
  M[1][0] = xy - wz;		
  M[1][1] = 1.0 - (xx + zz);
  M[1][2] = yz + wx;		
  M[1][3] = 0.0;

  M[2][0] = xz + wy;		
  M[2][1] = yz - wx;
  M[2][2] = 1.0 - (xx + yy);		
  M[2][3] = 0.0;

  M[3][0] = 0;			
  M[3][1] = 0;
  M[3][2] = 0;			
  M[3][3] = 1;  
}

/*-----------  QUATERNION — VECTOR  ------------*/
 
// 将矢量（或三维空间的一点）表示成四元组
QUATERNION QUATERNION::VectorToQuaternion(const PNT& V)
{
	QUATERNION Q;
    Q.x = V[0];
    Q.y = V[1];
    Q.z = V[2];
    Q.w = 0.0f;
	return Q;
}

// 将四元组的虚部用矢量表示
PNT QUATERNION::QuaternionToVector()
{
	PNT V;
    V[0] = x;
    V[1] = y;
    V[2] = z;
	return V;
}

PNT QUATERNION::VectorTransform(const PNT& V)
{
	if( ((w>1-DELTA_ROT)&&(w<1+DELTA_ROT)) || 
		((w>-1-DELTA_ROT)&&(w<-1+DELTA_ROT)) ) return V;

	QUATERNION A, B, C;
	A=VectorToQuaternion(V);
	B=*this;
	QuaternionInverse(B);
	C=*this*A*B;
	return C.QuaternionToVector();
}

