#pragma once
#include "geometry.h"
#include "XGeneralCurve.h"
#include "pnt.h"
class AFX_EXT_CLASS  XCircle :
								public XGeneralCurve
{
public:
	PNT  m_Center;
	double m_Radius;
	PNT m_Pnt_plane;
	PNT m_Norm_plane;

public:
	XCircle(void);
	virtual ~XCircle(void);
	virtual void SetUnit(const int& unit);
	virtual void InitObject() ;
	virtual void Translate(const PNT& delt_t);
	virtual void Rotate(const Ti_Rotation& R);
	double	GetAngle(PNT& pnt);
	void	SetAngle(PNT& sp, PNT& ep);
	double  GetStartAngle();
	double	GetEndAngle();


public:
	int		m_LineStyle;
	float	m_LineWidth;

	void	SetLineStyle(int ls);
	void	SetLineWidth(float lw);
	int		GetLineStyle();
	float	GetLineWidth();
    double      m_StartAngle;            /* expressed in radians */
    double      m_EndAngle;              /* expressed in radians */

	//by jm Feb 7th 2006 *************************************************//
	bool        m_bShowBox;
	double		m_BoxLLeft[3];   //the lower left point to define the box
	double		m_BoxURight[3];  //the upper right point to define the box
	int		    m_FitPntNum;  //the pnt number fitted to line
	double      m_RMS;        //the rms error when fitting
	void		SetShowBox(bool flag);
	bool		GetShowBox();
	bool		SetBox(double left[3], double right[3]);

protected:
	virtual void	UpdateBndBox();

	};
