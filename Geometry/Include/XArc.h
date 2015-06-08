#pragma once
#include "XGeneralCurve.h"
#include "Ti_WCSYS.h"

class  AFX_EXT_CLASS  XArc : public XGeneralCurve
{
public:
	XArc(void);
	XArc(const PNT &c, const PNT &s=PNT(0,0,0), const PNT &e=PNT(0,0,0), const PNT &n=PNT(0,0,1));
	XArc(const double& r, const PNT &c=PNT(0,0,0),const PNT &axis_dir=PNT(0,0,1),const double& sa=0,const double& ea=0);
	~XArc(void);

public:
	void Set(double r,PNT &c, PNT &axis_dir,double sa,double ea);
	XArc& operator = (const XArc& aArc);
	PNT MiddlePoint();  
	void Initial();
	//void SetRad(double r); //need to remove
	void SetDelFlag(bool flag);
	bool GetDelFlag();
	const float& GetRadius();
	const double& GetStartAngle();
	const double& GetEndAngle();
	PNT& GetNormal();
	void InitObject();
	PNT& GetCenter();
	void Translate(const PNT& trans);
	void Rotate(const Ti_Rotation& R);

	float Proportion(PNT pt);
	float Distance(PNT &pt);
	PNT NearPt(PNT pt);

	void ChangeUnits(bool bmm2inch );
	void SetUnit(const int& unit);

public:    

	double		m_c0, m_c1, m_c2, m_c3;         /* c0(x^2 + y^2) + c1*x + c2*y + c3 = 0 */
	float		m_Rad;                 /* radius of the arc */
    PNT			m_Cen;		        /* center of the arc */
    double      m_StartAngle;            /* expressed in radians */
    double      m_EndAngle;              /* expressed in radians */
	PNT			m_Normal;

	Ti_WCSYS	localsys;

protected:
	bool m_DelFlag;//曲线是否可删除标志位。true：可删除；false：不可删除。

	virtual void UpdateBndBox(void);
};
