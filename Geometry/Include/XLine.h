#pragma once
#include "XGeneralCurve.h"

class  AFX_EXT_CLASS XLine : public XGeneralCurve
{
public:
	XLine(PNT &s, PNT &e);
	XLine(void);
	~XLine(void);

public:
	XLine& operator = (const XLine& aLine);
	PNT MiddlePoint();  
	void Initial();
	void Set(PNT &s,PNT &e);
	bool GetDelFlag();
	void SetDelFlag(bool flag);
	void SetCoff();
	void SetLineStyle(int ls);

	double GetLen();	
	float Distance(PNT &pt);		//点到直线的最短距离
	float SectDistance(PNT &pt);	//点到线段的最短距离
	float FallProportion(PNT &pt);	//找到最近点，并返回该最近点在线段中的分割比例
	PNT NearPt(PNT &pt);			//线段上距离pt最近的点
	PNT GetProjectPt(PNT &pt);		//返回直线上到pt点最近的点

public:
	void UpdateBndBox();
	void ChangeUnits(bool bmm2inch );
	virtual void SetUnit(const int& unit);
	virtual void InitObject();
	virtual void Translate(const PNT& delt_t);
	virtual void Rotate(const Ti_Rotation& R);

	//by JM
	double		mL0, mL1, mL2;		//mL0 * x + mL1 * y + mL2 = 0
	float		m_LineWidth;
	int			m_Track;			//only use in edgebreak -JM
	int			m_LineStyle;
	PNT			m_AnPnt;        //record the position of annotation
	void   Extend(double lm);

	//by jm Feb 7th 2006 *************************************************//
	bool        m_bShowBox;
	double		m_BoxLLeft[3];   //the lower left point to define the box
	double		m_BoxURight[3];  //the upper right point to define the box
	int		    m_FitPntNum;  //the pnt number fitted to line
	double      m_RMS;        //the rms error when fitting
	void		SetShowBox(bool flag);
	bool		GetShowBox();
	bool		SetBox(double left[3], double right[3]);

	//by jm Feb 14 2006 *******************************//
	int			m_st;
	int			m_ed;

protected:
	bool m_DelFlag;					//曲线是否可删除标志位。true：可删除；false：不可删除。
	//PNT	m_StartPt, m_EndPt;		//Start and end points for line segment，确保m_StartPt离轴心较近，m_EndPt离轴心较远

};
