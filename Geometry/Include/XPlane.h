#pragma once
#include "geometry.h"
#include "xgeombase.h"
#include "xcloud.h"
class AFX_EXT_CLASS  XPlane : public XGeomBase
{
public:
	XPlane(void);
	~XPlane(void);

public:  
	double m_a, m_b, m_c, m_d;	// ma*x + mb*y + mc*z + md = 0  another kind of configuration  representation

public:
	void SetPoint(PNT aPoint);
	void MovePlane(double step);
	void SetParameter(double a, double b, double c, double d);
	void SetNormPoint(PNT aNorm, PNT aPoint);
		/*Calculator: fitting cloud to plane
		    Input: cld the cloud to fit
		                bRject whether reject the outlier points
				tolerance  if bReject is true, the tolerance will be used as the creteria 
		    Return RMS error of fitting */
	double  FitFromCld(const XCloud& cld, bool bReject=false, double tolerance=0.03);

	//override virtual functions
	void InitObject();
	virtual void SetUnit(const int& unit);
	void Translate(const PNT& trans);
	void Rotate(const Ti_Rotation& R);

	const float&  GetLength()const;
	const float& GetWidth()const;
protected:
	void UpdateBndBox();


public:
	PNT	m_Vector; //  configuration  parameter
	PNT	m_Pt;     // configuration  parameter
	float m_Length;  // shape parameter
	float m_Width;  // shape parameter

};
