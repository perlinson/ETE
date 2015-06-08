#include "StdAfx.h"
#include "xcylinder.h"
#include "math.h"
XCylinder::XCylinder(void)
{
	this->m_ObjectType = OBJECT_CYLINDER;

}

XCylinder::~XCylinder(void)
{
}
void XCylinder::Translate(const PNT& delt_t)
{
	this->m_Translation += delt_t;
	this->m_bDispChanged = true;
}
void XCylinder::Rotate(const Ti_Rotation& R)
{
	m_Orientation = R*m_Orientation;
	this->m_bDispChanged = true;
}

void XCylinder::SetUnit(const int& unit)
{
	if(this->m_Unit == UNIT_NONE|| m_Unit == unit)
	{
		m_Unit = unit;
		return;
	}
	switch(m_Unit)// suppose only two type unit(inch and mm)
	{
	case UNIT_MM:
		if(unit == UNIT_INCH)
		{
			XCylinder::m_Height/=25.4f;
			XCylinder::m_Radius /=25.4f;
		}
		break;
	case UNIT_INCH:
		if(unit == UNIT_MM)
		{
			XCylinder::m_Height *=25.4f;
			XCylinder::m_Radius *=25.4f;
		}
		break;
	default:
		;
	}
	SetObjState(true);	
}

void XCylinder::UpdateBndBox()
{
	if(this->m_bObjChanged)
	{
		m_bObjChanged = false;
		m_bDispChanged = true;
		PNT  pnt;
		double angle;
		for(int t = 0 ; t < 360; t+=5)
		{
			angle = t*PI/180.;
			pnt[0] = (float)(m_Radius*cos(angle));
			pnt[1] = -m_Height/2;
			pnt[2] = (float)(m_Radius*sin(angle));
			pnt =  this->m_Orientation.VectorTransform(pnt);
			pnt+= this->m_Translation;
			this->m_BndBox.AddVertex(pnt);

			pnt[0] = (float)(m_Radius*cos(angle));
			pnt[1] = m_Height/2;
			pnt[2] = (float)(m_Radius*sin(angle));
			pnt = m_Orientation.VectorTransform(pnt);
			pnt+= m_Translation;
			this->m_BndBox.AddVertex(pnt);
		}
	}
}
void XCylinder::InitObject()
{
	m_Radius = 1;
	m_Height = 2;
	SetObjState(true);
	m_bDispChanged = true;

}

//extern double PIE;
void XCylinder::ComputeCylinder( int numFacets,
								float c[][3], float tc[][3], int faces[])
{
	float angle, x, y;
	int i, polyIndex;
	float height,  radius;
	height = m_Height;
	radius = m_Radius;
	// Compute coordinates, texture coordinates:
	for (i = 0; i < numFacets; ++i) 
	{
		angle =(float)( i * 2 * PI / numFacets);
		x = (float)cos(angle);
		y = (float)sin(angle);
		c[i][0] = radius * x;
		c[i][1] = 0.5f * height;
		c[i][2] = radius * y;
		c[numFacets+i][0] = radius * x; //y负向的顶点
		c[numFacets+i][1] = -0.5f * height;
		c[numFacets+i][2] = radius * y;

		if (tc)
		{
			tc[i][0] = ((float) i) / numFacets;
			tc[i][1] = 0.0;
			tc[i][2] = 0.0;
			tc[numFacets+i][0] = ((float) i) / numFacets;
			tc[numFacets+i][1] = 1.0;
			tc[numFacets+i][2] = 0.0;
		}
	}
	// And compute indices:
	for (i = 0; i < numFacets; ++i)
	{
		polyIndex = 5*i; //每个狭长矩形的facet为4个顶点组成
		faces[polyIndex + 0] = i;
		faces[polyIndex + 1] = (i+1) % numFacets;
		faces[polyIndex + 2] = (i+1) % numFacets + numFacets;
		faces[polyIndex + 3] = i + numFacets;
		faces[polyIndex + 4] = -1;
	}
}

//yk0805: find the intersections(if any) with a line (linePnt + t* lineDir)
// -1----no intersection on the infinite cylinder wall,i.e.,line is parallel to cylinder
// 0 ----both intersections extends the finite cylinder wall
// 1----1 intersection,interPnt1, on the finite cylinder wall.( the other intersection point extends the wall)
// 2----2 intersections,interPnt1 and interPnt2, on the finite cylinder wall
int XCylinder::LineIntersection(const PNT & linePnt,const PNT & lineDir,
								PNT & interPnt1, PNT & interPnt2) 
{
	PNT p, v, pa, va,p1, p2, q;
	int interNumber = 0;//交点数及位置：(q1,q2): 00, 01, 10, 11
	//this->ChangeSelState();
	//line: p + vt
	p = linePnt;
	v = lineDir;
	//cylinder axis: pa + va* t, pa can be any point on axis
	double r;
	GetCylinderInfo(va, pa, r); //here pa is the barycenter
	va.Normalize();
	v.Normalize();
	PNT x = p-pa;
	double A = (v-va*(v*va)) * (v-va*(v*va)) ;
	double B = (v-va*(v*va)) * (x-va*(x*va)) * 2.0;
	double C = (x-va*(x*va)) * (x-va*(x*va))  - r*r;

	double delta = B*B -4.0*A*C;
	if (delta < 0)
		return -1; //no intersection,i.e.,line is parallel to cylinder

	double t1 = (0-B + sqrt(delta)) *0.5 /A;	//GOD BLESS ME.Aug.13-16
	PNT q1 = p + v*t1;
	//assert q1's projection on axis has a positive length less than height, i.e, q1 is on the wall
	if(fabs((q1-pa)*va) < m_Height*0.5)
		interNumber = 0x2;//q1q2 = 10

	double t2 = (0-B - sqrt(delta)) *0.5 /A;
	PNT q2 = p + v*t2;
	//assert q2's projection on axis has a length less than height/2, i.e, q2 is on the wall
	if( fabs((q2-pa)*va) < m_Height*0.5)
		interNumber += 0x1; //q1q2 = 01 or 11

	switch(interNumber) {
	case 0x1://q2 is the only intersection
		interPnt1 = q2;
		return 1;
		break;
	case 0x2://q1 is the only intersection
		interPnt1 = q1;
		return 1;
		break;
	case 0x3://both q1 and q2 are meaningful

		interPnt1 = q1; //choose the one with a nearer z coordinate
		interPnt2 = q2;
		return 2;
		break;
	default: //0x0, both intersections extends the finite cylinder wall
		return 0;
		break;
	}
}


void XCylinder::GetCylinderInfo(PNT& AxisDir, PNT& AxisPnt, double & Radius)
{
	AxisDir = PNT(0, 1, 0);
	AxisDir = m_Orientation.VectorTransform(AxisDir);
	AxisPnt = this->m_Translation;
	Radius = this->m_Radius;
}
int __cdecl EvaluateforCylinder(int m,		// number of points
								int n,		// number of parameters = 7
								double *const par, // the elements are x,y,z A,B,C,r
								double* rv,		// residual value,e.g. distance diviation
								double**g_pnt
								)
{
	double norm;
	double f,g;
	norm = sqrt(par[4]*par[4]+par[5]*par[5]+par[6]*par[6]);
	par[4] /= norm;
	par[5] /= norm;
	par[6] /= norm;
	for( int i=1 ; i <= m; i++)
	{
		g = (g_pnt[i][1] - par[1])*par[4] +
			(g_pnt[i][2] - par[2])*par[5] +
			(g_pnt[i][3] - par[3])*par[6] ;
		f = sqrt(
			(g_pnt[i][1] - par[1])*(g_pnt[i][1] - par[1])+
			(g_pnt[i][2] - par[2])*(g_pnt[i][2] - par[2])+
			(g_pnt[i][3] - par[3])*(g_pnt[i][3] - par[3])-
			g*g
			);
		rv[i] = f-par[7];
	//	rv[i] = exp(-(rv[i]*rv[i])/(0.1*0.1))*rv[i];
	}
	return 1;
}
int __cdecl EvaluateforCylinder_Guass(int m,		// number of points
								int n,		// number of parameters = 7
								double *const par, // the elements are x,y,z A,B,C,r
								double* rv,		// residual value,e.g. distance diviation
								double**g_pnt
								)
{
	double norm;
	double f,g;
	norm = sqrt(par[4]*par[4]+par[5]*par[5]+par[6]*par[6]);
	par[4] /= norm;
	par[5] /= norm;
	par[6] /= norm;
	double mean = 0, sigma = 0;
	int i;
	for( i=1 ; i <= m; i++)
	{
		g = (g_pnt[i][1] - par[1])*par[4] +
			(g_pnt[i][2] - par[2])*par[5] +
			(g_pnt[i][3] - par[3])*par[6] ;
		f = sqrt(
			(g_pnt[i][1] - par[1])*(g_pnt[i][1] - par[1])+
			(g_pnt[i][2] - par[2])*(g_pnt[i][2] - par[2])+
			(g_pnt[i][3] - par[3])*(g_pnt[i][3] - par[3])-
			g*g
			);
		rv[i] = f-par[7];
		mean += rv[i]*rv[i];
	}
	sigma = sqrt(mean/m);
	if(sigma < 0.0001)
		sigma = 0.0001;
	for(i =1; i<= m; i++)
	{
		rv[i] = exp(1-(rv[i]*rv[i])/(2*sigma*sigma))*rv[i];
	}
	return 1;
}


//derivatives for distance Calculator
void __cdecl JacobianforCylinder(int m,	// number of points 
								 int n,		// number of parameters = 7
								 double *const  par,	  // the elements are x,y,z A,B,C,r
								 double*& rv,   // residual value,e.g. distance diviation
								 double **jac,
								 double **g_pnt
								 )
{
	double f,g,fx,fy,fz,fa,fb,fc;
	double norm;
	norm = sqrt(par[4]*par[4]+par[5]*par[5]+par[6]*par[6]);
	par[4] /= norm;
	par[5] /= norm;
	par[6] /= norm;
	for( int i=1 ; i <= m; i++)
	{
		g = (g_pnt[i][1] - par[1])*par[4] +
			(g_pnt[i][2] - par[2])*par[5] +
			(g_pnt[i][3] - par[3])*par[6] ;
		f = sqrt(
			(g_pnt[i][1] - par[1])*(g_pnt[i][1] - par[1])+
			(g_pnt[i][2] - par[2])*(g_pnt[i][2] - par[2])+
			(g_pnt[i][3] - par[3])*(g_pnt[i][3] - par[3])-
			g*g
			);
		if (fabs(f)>1e-6)
		{
			fx = (par[4]*g-(g_pnt[i][1]-par[1]))/f;
			fy = (par[5]*g-(g_pnt[i][2]-par[2]))/f;
			fz = (par[6]*g-(g_pnt[i][3]-par[3]))/f;
			fa = g*(par[4]*g-(g_pnt[i][1]-par[1]))/f;
			fb = g*(par[5]*g-(g_pnt[i][2]-par[2]))/f;
			fc = g*(par[6]*g-(g_pnt[i][3]-par[3]))/f;
		}
		else
		{
			fx = sqrt(1- par[4]*par[4]);
			fy = sqrt(1- par[5]*par[5]);
			fz = sqrt(1- par[6]*par[6]);
			fa = g*fx;
			fb = g*fy;
			fc = g*fz;
		}
		jac[i][1] = fx;
		jac[i][2] = fy;
		jac[i][3] = fz;
		jac[i][4] = fa;
		jac[i][5] = fb;
		jac[i][6] = fc;
		jac[i][7] = -1;		
	}
}