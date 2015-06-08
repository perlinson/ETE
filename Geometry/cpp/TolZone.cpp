#include "stdafx.h"
//#include "Geometry_macro.h"
#include "math.h"
#include "XLine.h"
#include "XCircle.h"
#include "tolzone.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4244)

TolZone::TolZone(void)
{
	m_bBuild = false;
	m_TolType = 0;
	for(int i = 0; i< 10 ; i++)
	{
		c[i]=NULL;
		l[i]=NULL;
	}
}

TolZone::~TolZone(void)
{
	//Clear();
}

int	TolZone::build2(XLine *l1,XLine *l2,float in_r,float in_R,int tol)
{	
	//Actually this part is round edge, for other type need to specify it
	m_TolType = tol;
	m_MinR = in_r;
	m_MaxR = in_R;
	m_L1 = l1;
	m_L2 = l2;

	double temp = l1->mL0*l2->mL1-l1->mL1*l2->mL0;
	if( fabs(temp) < 0.00000001 ) return -1;

	anchor_pt[0] = (l2->mL2*l1->mL1-l1->mL2*l2->mL1)/temp;
	anchor_pt[1] = (l1->mL2*l2->mL0-l2->mL2*l1->mL0)/temp;

	if(tol == 3) return tol; //break edge	

	float v1[2],v2[2],cos_half_angle,val;
	v1[0] = l1->m_StartPt[0] - anchor_pt[0]; v1[1] = l1->m_StartPt[1] - anchor_pt[1]; 
	v2[0] = l2->m_StartPt[0] - anchor_pt[0]; v2[1] = l2->m_StartPt[1] - anchor_pt[1]; 
	NORM(v1);NORM(v2);

	if( tol == 5 ) //chamfer  tol == 1 now changed to three line chamfer now.
	{
		PNT p1,p2,p3,p0;
		p0[0]=tp_0[0] = anchor_pt[0] + v1[0]*in_r;  p0[1]=tp_0[1] = anchor_pt[1] + v1[1]*in_r;
		p1[0]=tp_1[0] = anchor_pt[0] + v1[0]*in_R;  p1[1]=tp_1[1] = anchor_pt[1] + v1[1]*in_R;
		p2[0]=tp_2[0] = anchor_pt[0] + v2[0]*in_r;  p2[1]=tp_2[1] = anchor_pt[1] + v2[1]*in_r;
		p3[0]=tp_3[0] = anchor_pt[0] + v2[0]*in_R;  p3[1]=tp_3[1] = anchor_pt[1] + v2[1]*in_R;

		XGeomBase *myline;
		myline = new XLine(p0,p1);
		l[0] = (XLine*)myline;
		l[0]->SetColor(8);
		l[0]->SetLineStyle(4);
		this->m_TolGeo.Add(myline);

		myline = new XLine(p1,p3);
		l[1] = (XLine*)myline;
		l[1]->SetColor(8);
		l[1]->SetLineStyle(4);
		this->m_TolGeo.Add(myline);

		myline = new XLine(p3,p2);
		l[2] = (XLine*)myline;
		l[2]->SetColor(8);
		l[2]->SetLineStyle(4);
		this->m_TolGeo.Add(myline);

		myline = new XLine(p2,p0);
		l[3] = (XLine*)myline;
		l[3]->SetColor(8);
		l[3]->SetLineStyle(4);
		this->m_TolGeo.Add(myline);

		m_bBuild = true;

		return tol;
	}

	bisector[0]= .5*(v1[0]+v2[0]);
	bisector[1]= .5*(v1[1]+v2[1]);

	NORM(bisector); //normalize. Assumes sign is correct
	//compute the big circle, tangent to the edges
	cos_half_angle = DOT(bisector,v1);
	val = 1./sqrt(1-SQ(cos_half_angle));

	//big circle
	XGeomBase *c0 = new XCircle;
	((XCircle*)c0)->m_Radius = m_MaxR;
	((XCircle*)c0)->m_Center[0] = anchor_pt[0]+m_MaxR*val*bisector[0];
	((XCircle*)c0)->m_Center[1] = anchor_pt[1]+m_MaxR*val*bisector[1];
	c[0] = (XCircle*)c0;
	if(tol == 2)	//radius
	{
		c[0]->SetColor(8);
		c[0]->SetLineStyle(2);
		this->m_TolGeo.Add(c0);
	}

	//this->m_TolGeo.Add(c0);

	//small circle
	XGeomBase *c1 = new XCircle;
	((XCircle*)c1)->m_Radius = m_MinR;
	((XCircle*)c1)->m_Center[0] = anchor_pt[0]+m_MinR*val*bisector[0];
	((XCircle*)c1)->m_Center[1] = anchor_pt[1]+m_MinR*val*bisector[1];
	c[1] = (XCircle*)c1;
	c[1]->SetColor(8);
	c[1]->SetLineStyle(2);
	this->m_TolGeo.Add(c1);

	PNT tp0,tp1,ttp;
	tp0 = l1->GetProjectPt(c[0]->m_Center);
	tp1 = l2->GetProjectPt(c[0]->m_Center);

	tp_0[0] = tp0[0];	tp_0[1] = tp0[1];
	tp_1[0] = tp1[0];	tp_1[1] = tp1[1];

	XGeomBase *lll;	
	PNT tpls, tple;
	if(tol == 0 || tol == 4)
	{
		//small circle 1 near edge 
		ttp = c[0]->m_Center - tp0; ttp.Normalize();
		XGeomBase *c2 = new XCircle;
		((XCircle*)c2)->m_Radius = m_MinR;
		((XCircle*)c2)->m_Center[0] = tp0[0]+m_MinR*(ttp[0]);
		((XCircle*)c2)->m_Center[1] = tp0[1]+m_MinR*(ttp[1]);
		c[2] = (XCircle*)c2;
		c[2]->SetColor(8);
		c[2]->SetLineStyle(2);

		this->m_TolGeo.Add(c2);
	
		//small circle 2 near edge 
		ttp = c[0]->m_Center - tp1; ttp.Normalize();
		XGeomBase *c3 = new XCircle;
		((XCircle*)c3)->m_Radius = m_MinR;
		((XCircle*)c3)->m_Center[0] = tp1[0]+m_MinR*(ttp[0]);
		((XCircle*)c3)->m_Center[1] = tp1[1]+m_MinR*(ttp[1]);
		c[3] = (XCircle*)c3;
		c[3]->SetColor(8);
		c[3]->SetLineStyle(2);

		this->m_TolGeo.Add(c3);
	
		//line between two small circle
		tp_4[0]=tpls[0] = c[2]->m_Center[0] - m_MinR*bisector[0];
		tp_4[1]=tpls[1] = c[2]->m_Center[1] - m_MinR*bisector[1];
		tp_5[0]=tple[0] = c[3]->m_Center[0] - m_MinR*bisector[0];
		tp_5[1]=tple[1] = c[3]->m_Center[1] - m_MinR*bisector[1];

		lll = new XLine(tpls,tple);
		l[0] = (XLine*)lll;
		l[0]->SetColor(8);
		l[0]->SetLineStyle(4);

		this->m_TolGeo.Add(lll);
	}
	//line between small circle and small circle 1
	PNT tps0,tps1;
	tps0 = l1->GetProjectPt(c[1]->m_Center);
	tps1 = l2->GetProjectPt(c[1]->m_Center);

	tp_2[0] = tps0[0];	tp_2[1] = tps0[1];
	tp_3[0] = tps1[0];	tp_3[1] = tps1[1];

	lll = new XLine(tp0,tps0);
	l[1] = (XLine*)lll;
	l[1]->SetColor(8);
	l[1]->SetLineStyle(4);
	this->m_TolGeo.Add(lll);

	//line between small circle and small circle 2
	lll = new XLine(tp1,tps1);
	l[2] = (XLine*)lll;
	l[2]->SetColor(8);
	l[2]->SetLineStyle(4);
	this->m_TolGeo.Add(lll);

	m_bBuild = true;

	//set start and end angle
	c[0]->SetAngle(tp0,tp1);
	c[1]->SetAngle(tps0,tps1);
	if(tol == 0 || tol == 4)
	{
		c[2]->SetAngle(tp0,tpls);
		c[3]->SetAngle(tple,tp1);
	}
	return tol;
}

int TolZone::rebuild(float in_r,float in_R,int tol)
{
	if (!m_bBuild) return -1;

	m_TolType = tol;
	m_MinR = in_r;
	m_MaxR = in_R;

	XLine *l1, *l2;
	l1 = m_L1;
	l2 = m_L2;

	build2(l1,l2,in_r,in_R,tol);

	return 0;

	double temp = l1->mL0*l2->mL1-l1->mL1*l2->mL0;
	if( fabs(temp) < 0.00000001 ) return -1;

	anchor_pt[0] = (l2->mL2*l1->mL1-l1->mL2*l2->mL1)/temp;
	anchor_pt[1] = (l1->mL2*l2->mL0-l2->mL2*l1->mL0)/temp;

	if(tol != 0) return 0; 

	float v1[2],v2[2],cos_half_angle,val;
	v1[0] = l1->m_StartPt[0] - anchor_pt[0]; v1[1] = l1->m_StartPt[1] - anchor_pt[1]; 
	v2[0] = l2->m_StartPt[0] - anchor_pt[0]; v2[1] = l2->m_StartPt[1] - anchor_pt[1]; 
	NORM(v1);NORM(v2);

	bisector[0]= .5*(v1[0]+v2[0]);
	bisector[1]= .5*(v1[1]+v2[1]);

	NORM(bisector); //normalize. Assumes sign is correct
	//compute the big circle, tangent to the edges
	cos_half_angle = DOT(bisector,v1);
	val = 1./sqrt(1-SQ(cos_half_angle));

	//big circle
	XGeomBase *c0 = c[0];
	((XCircle*)c0)->m_Radius = m_MaxR;
	((XCircle*)c0)->m_Center[0] = anchor_pt[0]+m_MaxR*val*bisector[0];
	((XCircle*)c0)->m_Center[1] = anchor_pt[1]+m_MaxR*val*bisector[1];
	//c[0] = (XCircle*)c0;

	//this->m_TolGeo.Add(c0);

	//small circle
	XGeomBase *c1 = c[1];
	((XCircle*)c1)->m_Radius = m_MinR;
	((XCircle*)c1)->m_Center[0] = anchor_pt[0]+m_MinR*val*bisector[0];
	((XCircle*)c1)->m_Center[1] = anchor_pt[1]+m_MinR*val*bisector[1];
	//c[1] = (XCircle*)c1;
	//c[1]->SetColor(4);
	//c[1]->SetLineStyle(2);
	//this->m_TolGeo.Add(c1);

	PNT tp0,tp1,ttp;
	tp0 = l1->GetProjectPt(c[0]->m_Center);
	tp1 = l2->GetProjectPt(c[0]->m_Center);

	//small circle 1 near edge 
	ttp = c[0]->m_Center - tp0; ttp.Normalize();
	XGeomBase *c2 = c[2];
	((XCircle*)c2)->m_Radius = m_MinR;
	((XCircle*)c2)->m_Center[0] = tp0[0]+m_MinR*(ttp[0]);
	((XCircle*)c2)->m_Center[1] = tp0[1]+m_MinR*(ttp[1]);
	//c[2] = (XCircle*)c2;
	//c[2]->SetColor(4);
	//c[2]->SetLineStyle(2);

	//this->m_TolGeo.Add(c2);

	//small circle 2 near edge 
	ttp = c[0]->m_Center - tp1; ttp.Normalize();
	XGeomBase *c3 = c[3];
	((XCircle*)c3)->m_Radius = m_MinR;
	((XCircle*)c3)->m_Center[0] = tp1[0]+m_MinR*(ttp[0]);
	((XCircle*)c3)->m_Center[1] = tp1[1]+m_MinR*(ttp[1]);
	//c[3] = (XCircle*)c3;
	//c[3]->SetColor(4);
	//c[3]->SetLineStyle(2);

	//this->m_TolGeo.Add(c3);

	//line between two small circle
	PNT tpls, tple;
	tpls[0] = c[2]->m_Center[0] - m_MinR*bisector[0];
	tpls[1] = c[2]->m_Center[1] - m_MinR*bisector[1];
	tple[0] = c[3]->m_Center[0] - m_MinR*bisector[0];
	tple[1] = c[3]->m_Center[1] - m_MinR*bisector[1];

	l[0]->m_StartPt[0] = tpls[0];
	l[0]->m_StartPt[1] = tpls[1];
	l[0]->m_EndPt[0] = tple[0];
	l[0]->m_EndPt[1] = tple[1];

	l[0]->SetObjState(true);

	//this->m_TolGeo.Add(lll);

	//line between small circle and small circle 1
	PNT tps0,tps1;
	tps0 = l1->GetProjectPt(c[1]->m_Center);
	tps1 = l2->GetProjectPt(c[1]->m_Center);

	//lll = new XLine(tp0,tps0);
	l[1]->m_StartPt = tp0;
	l[1]->m_EndPt = tps0;
	l[1]->SetObjState(true);

	//line between small circle and small circle 2
	//lll = new XLine(tp1,tps1);
	l[2]->m_StartPt = tp1;
	l[2]->m_EndPt = tps1;
	l[2]->SetObjState(true);


	return 0;
}


void TolZone::Clear()
{
	for(int i = 0; i< 10 ; i++)
	{
		if(c[i]!=NULL)
			delete c[i];
		c[i]=NULL;
		if(l[i]!=NULL)
			delete l[i];
		l[i]=NULL;
	}
	return;
}