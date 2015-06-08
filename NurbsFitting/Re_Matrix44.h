// Re_Matrix44.h: interface for the Re_Matrix44 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RE_MATRIX44_H__4ACE23A3_8105_11D2_A769_000021E5AAC2__INCLUDED_)
#define AFX_RE_MATRIX44_H__4ACE23A3_8105_11D2_A769_000021E5AAC2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class AFX_CLASS_EXPORT Re_Matrix44  
{
public:
	Re_Matrix44();
	Re_Matrix44(double[16]);
	void Identity();

	virtual ~Re_Matrix44();
public:
	double m_v[16];
	double GetEnum(int row,int column);
	BOOL   SetEnum(int row,int column,double val);
	Re_Matrix44  operator +(Re_Matrix44);
	Re_Matrix44  Add(Re_Matrix44);
	Re_Matrix44  operator -(Re_Matrix44);
	Re_Matrix44  Sub(Re_Matrix44);
	Re_Matrix44  operator *(Re_Matrix44);
	Re_Matrix44  Multiple(Re_Matrix44);
	Re_Matrix44  Reverse(Re_Matrix44);
	Re_Matrix44& operator =(Re_Matrix44 org);
	Re_Matrix44(Re_Matrix44& org);
	void GetMatrixRotate(int axisnum,double angle);
	void GetMatrixTranslate(double x,double y,double z);
	void GetMatrixScale(double);
	void GetPoint(double x,double y,double z,double *ox,double *oy,double *oz);
};

#endif // !defined(AFX_RE_MATRIX44_H__4ACE23A3_8105_11D2_A769_000021E5AAC2__INCLUDED_)
