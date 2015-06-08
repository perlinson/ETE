#pragma once

class AFX_EXT_CLASS TolZone
{
public:
	TolZone(void);
	~TolZone(void);

public:
	int		m_TolType;// 0-round edge; 1- chamfer ; 2- radius
	bool	m_bShow;
	bool    m_bBuild;

	float	tp_0[2],tp_1[2],tp_2[2],tp_3[2],tp_4[2],tp_5[2];
	float	anchor_pt[2];
	float	bisector[2];

	XCircle *c[10];
	XLine	*l[10];

	XLine   *m_L1,*m_L2;
	double	m_MinR;
	double  m_MaxR;
	
	pXGeomBaseArray m_TolGeo;

	int		build2(XLine *l1,XLine *l2,float in_r,float in_R,int tol);
	int     rebuild(float in_r,float in_R,int tol);
	int		test();
	//by Jm Feb 14 2006  ***************//
	void    Clear();
};
