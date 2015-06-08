// Re_Vector.h: interface for the Re_Vector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RE_VECTOR_H__F0A23022_3A63_11D2_8A6E_D0FF93CAAF33__INCLUDED_)
#define AFX_RE_VECTOR_H__F0A23022_3A63_11D2_8A6E_D0FF93CAAF33__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class AFX_CLASS_EXPORT Re_Vector  
{
public:
	Re_Vector();
	virtual ~Re_Vector();
public:
    double m_x;
    double m_y;
    double m_z;
public:
    Re_Vector(double,double,double);
public:
    int IsNull();
    int Normal();
    void SetValue(double,double,double);
    Re_Vector Add(Re_Vector);
    Re_Vector operator +(Re_Vector);
    Re_Vector Sub(Re_Vector);
    Re_Vector operator -(Re_Vector);
    Re_Vector Mul(double);
    Re_Vector operator *(double);
    double DotMul(Re_Vector);
    double operator* (Re_Vector);
    Re_Vector Cross(Re_Vector);
    Re_Vector operator ^(Re_Vector);
    Re_Vector& operator =(Re_Vector);
    double Angle(Re_Vector );
    Re_Vector(const Re_Vector&);
	void RotateAng(double &, double &, double &);
	double Length();
	double Len();
	void RotateX(double);
	void RotateY(double);
	void RotateZ(double);
    
};

#endif // !defined(AFX_RE_VECTOR_H__F0A23022_3A63_11D2_8A6E_D0FF93CAAF33__INCLUDED_)
