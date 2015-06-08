#include "StdAfx.h"
#include "xmesh.h"
#include "stdlib.h"
#include <assert.h>
#include <math.h>
#include <algorithm>
#include "ti_memory.h"
#include "Geometry_macro.h"
#include "ti_optimization.h"
#include "xpolyline.h"


//////////////////////////////////////////////////////////////////////
// CLASS XFACET
//////////////////////////////////////////////////////////////////////
XFacet::XFacet(void)
	{
	}

XFacet::~XFacet(void)
	{
	}
XFacet& XFacet::operator = (const XFacet& facet)
	{
	m_Norm = facet.m_Norm;
	m_Vertex[0] = facet.m_Vertex[0];
	m_Vertex[1] = facet.m_Vertex[1];
	m_Vertex[2] = facet.m_Vertex[2];
	//((SimpleArray<int>)m_NeighbFacet) = facet.m_NeighbFacet;
	return (*this);
	}


bool XFacet::Int_Plane(
		       const PNT&  norm, 
		       const PNT&  PntOn , 
		       int& num_int, 
		       float Int_pnt[9])
{
int j;
int counter = 0;
PNT intpnt;
for(j=0; j<3;j++)
	{	
	if((m_Vertex[j] - PntOn)*norm > 6e-9) 
		{
		counter++;
		}
	}
if (counter ==0 || counter ==3)
	{
	num_int = 0;
	return false;
	}
else
	{
	num_int = 0;
	j=0;
	if(this->LineSeg_Plane( m_Vertex[0],  m_Vertex[1],  PntOn, norm, intpnt) )
		{
		Int_pnt[j++] = intpnt[0];
		Int_pnt[j++] = intpnt[1];
		Int_pnt[j++] = intpnt[2];
		num_int++;
		}
	if(this->LineSeg_Plane( m_Vertex[0],  m_Vertex[2],  PntOn, norm, intpnt) )
		{
		Int_pnt[j++] = intpnt[0];
		Int_pnt[j++] = intpnt[1];
		Int_pnt[j++] = intpnt[2];
		num_int++;
		}
	if(this->LineSeg_Plane( m_Vertex[2],  m_Vertex[1],  PntOn, norm, intpnt) )
		{
		Int_pnt[j++] = intpnt[0];
		Int_pnt[j++] = intpnt[1];
		Int_pnt[j++] = intpnt[2];
		num_int++;
		}
	return true;
	}
}

bool	 XFacet::Pnt2Facet_Dist(
							const PNT&	comparepnt, 
							PNT&		nearestpnt, 
							double &			dist, 
							const double&	tolerance)const
	{
	dist = 0; 
	double		t;
	double		w[3];
	PNT	V;
	PNT	Q[3];  
	const double	precision = 1e-6;
	int			region_num;
	V  = m_Vertex[0] - comparepnt;
	t    = V*m_Norm;
	if(fabs(V[0]) > 10*tolerance || 
		fabs(V[1]) > 10*tolerance||
		fabs(V[2]) > 10*tolerance||
		fabs(t) > tolerance)
		{
		return false;
		}

	nearestpnt = comparepnt+ m_Norm*t; //projected point
	Q[0] = m_Vertex[0] - m_Vertex[2];
	Q[1] = m_Vertex[1] - m_Vertex[2];
	Q[2] = nearestpnt - m_Vertex[2];

	double a, b, c, d,e;	
	a = Q[0]*Q[0];
	b = Q[0]*Q[1];
	c = Q[1]*Q[1];
	d = Q[0]*Q[2];
	e = Q[1]*Q[2];

	w[0] = (c*d-b*e)/(a*c-b*b);
	w[1] = (-b*d+a*e)/(c*a-b*b);
	w[2] = 1- w[0]-w[1];

	region_num =0;
	if(w[0] >= 0)
		{
		region_num +=4;
		}
	if(w[1] >= 0)
		{
		region_num += 2;
		}
	if(w[2] >= 0)
		{
		region_num += 1;
		}
	switch (region_num)
		{
		case 7:  // region I
			{
			V = nearestpnt - comparepnt;
			dist = V.Length();
			if(dist < tolerance)
				return true;
			else
				return false;
			break;
			}
		case 6: // region II
			{
			dist = Pnt2LineSeg(comparepnt, m_Vertex[0], m_Vertex[1], nearestpnt);
			if(dist < tolerance)
				return true;
			else
				return false;
			break;
			}
		case 2: // region III
			{
			double d[2];
			PNT nearpnt[2];
			d[0] = Pnt2LineSeg(comparepnt, m_Vertex[0], m_Vertex[1], nearpnt[0]);
			d[1] = Pnt2LineSeg(comparepnt, m_Vertex[1], m_Vertex[2], nearpnt[1]);
			if(d[0] < d[1])
				{
				nearestpnt = nearpnt[0];
				dist = d[0];
				if(dist < tolerance)
					return true;
				else
					return false;
				}
			else
				{
				nearestpnt = nearpnt[1];
				dist = d[1];
				if(dist < tolerance)
					return true;
				else
					return false;
				}
			break;
			}
		case 3:  // region IV
			{
			dist = Pnt2LineSeg(comparepnt, m_Vertex[1], m_Vertex[2], nearestpnt);
			if(dist < tolerance)
				return true;
			else
				return false;
			break;
			}
		case 1: // region V
			{
			double d[2];
			PNT nearpnt[2];
			d[0] = Pnt2LineSeg(comparepnt, m_Vertex[0], m_Vertex[2], nearpnt[0]);
			d[1] = Pnt2LineSeg(comparepnt, m_Vertex[1], m_Vertex[2], nearpnt[1]);
			if(d[0] < d[1])
				{
				nearestpnt = nearpnt[0];
				dist = d[0];
				if(dist < tolerance)
					return true;
				else
					return false;
				}
			else
				{
				nearestpnt = nearpnt[1];
				dist = d[1];
				if(dist < tolerance)
					return true;
				else
					return false;
				}
			break;
			}
		case 5: // region VI
			{
			dist = Pnt2LineSeg(comparepnt, m_Vertex[0], m_Vertex[2], nearestpnt);
			if(dist < tolerance)
				return true;
			else
				return false;
			break;
			}
		case 4: // region VII
			{
			double d[2];
			PNT  nearpnt[2];
			d[0] = Pnt2LineSeg(comparepnt, m_Vertex[0], m_Vertex[1], nearpnt[0]);
			d[1] = Pnt2LineSeg(comparepnt, m_Vertex[0], m_Vertex[2], nearpnt[1]);
			if(d[0] < d[1])
				{
				nearestpnt = nearpnt[0];
				dist = d[0];
				if(dist < tolerance)
					return true;
				else
					return false;
				}
			else
				{
				nearestpnt = nearpnt[1];
				dist = d[1];
				if(dist < tolerance)
					return true;
				else
					return false;
				}
			break;
			}
		}	
	return false;
	}
	double	XFacet:: Pnt2LineSeg(
							const PNT&  Comp, 
							const PNT&  startpnt, 
							const PNT& endpnt, 
							PNT& nearestpnt)const
		{
		PNT p, q;
		double t, dist, norm;
		//return norm;

		p = startpnt- endpnt;
		q = endpnt - Comp;
		norm = p*p;
		dist = p*q;

		if(dist > 0) // endpnt is the closest point
			{
			nearestpnt = endpnt;
			return q.Length();
			}
		else
			{
			if(dist < -norm) // startpnt is the closest point
				{
				nearestpnt = startpnt;
				p = startpnt - Comp;
				return p.Length();
				}
			else
				{
				t = -(dist/norm);
				nearestpnt = startpnt*t + endpnt*(1-t);
				p = nearestpnt - Comp;
				return p.Length();
				}
			}
		}
	void	 XFacet::GetCenterPnt(PNT& pnt) const
		{
			pnt = m_Vertex[0]+m_Vertex[1]+m_Vertex[2];
			pnt = pnt/3;
		}

	bool	 XFacet::IsNeighbor(const XFacet& facet1, int& index0, int& index1, int& type)const
		{
		int flag0, flag1;
		XFacet tempFacet0 = * this;
		XFacet tempFacet1 = facet1;
		flag0 = flag1 = 0;
		for(int i=0,j;i<3;i++)
			{
			for(j=0;j<3;j++)
				{//m_Vertex[0].Normalize();
				if(tempFacet0.m_Vertex[i]==tempFacet1.m_Vertex[j])
					{
					flag0 += 2*i + 1;//use a Calculator y=2x+1
					flag1 += 2*j + 1;
					}
				}
			}
		switch(flag0)
			{
			case 1:  index0 = 0;type = 0;break;
			case 3:  index0 = 1;type = 0;break;
			case 4:  index0 = 0;type = 1;break;
			case 5:  index0 = 2;type = 0;break;
			case 6:  index0 = 2;type = 1;break;
			case 8:  index0 = 1;type = 1;break;
			default: return false;
			}
		switch(flag1)
			{
			case 1:  index1 = 0;break;
			case 3:  index1 = 1;break;
			case 4:  index1 = 0;break;
			case 5:  index1 = 2;break;
			case 6:  index1 = 2;break;
			case 8:  index1 = 1;break;
			default: return false;
			}
		return true;
		}

	bool XFacet::IsPnt_in_Triangle(const PNT& pnt)const
		{
		float a,b,c;
		int  i;
		unsigned  s, u, g;
		XFacet  V = *this;
		for(i=0; i<3; i++)
			{
			V.m_Vertex[i] -= pnt;
			}
		s = 0;
		if( V.m_Vertex[0][0] >0)
			s+=1;
		if( V.m_Vertex[0][1] > 0)
			s+=2;
		if( V.m_Vertex[1][0] >0)
			s+=4;
		if( V.m_Vertex[1][1] > 0)
			s+=8;
		if( V.m_Vertex[2][0] >0)
			s+=16;
		if( V.m_Vertex[2][1] > 0)
			s+=32;
		g = 21;
		u = s& g;
		if(u ==0|| u ==21)
			return false;
		g = 42;
		u = s&g;
		if(u ==0|| u == 42)
			return  false;
		a = V.m_Vertex[1][0]*V.m_Vertex[2][1]-V.m_Vertex[1][1]*V.m_Vertex[2][0];
		b = V.m_Vertex[0][1]*V.m_Vertex[2][0]-V.m_Vertex[0][0]*V.m_Vertex[2][1];
		c = V.m_Vertex[0][0]*V.m_Vertex[1][1]-V.m_Vertex[0][1]*V.m_Vertex[1][0];
		if( (a>=0 && b>=0 && c>= 0)||
			(a<=0 && b<=0 && c<= 0) )
			return true;
		else 
			return false;
		}

	bool XFacet::LineSeg_Plane(
		const PNT& pnt1, 
		const PNT& pnt2,
		const PNT& planepnt,
		const PNT& planenorm,
		PNT& interpnt)
		{
		double a, b, tmp;
		a = (planepnt -pnt2)*planenorm;
		b = (pnt1 -pnt2)*planenorm;
		if(a*b > 0 &&  fabs(a) <fabs( b))
			{
			tmp = a/b;
			interpnt = pnt1*tmp + pnt2*(1- tmp);
			return true;
			}
		else 
			return false;
		}


//////////////////////////////////////////////////////////////////////
// CLASS XMESH
//////////////////////////////////////////////////////////////////////

XMesh::XMesh()
	{
	m_Color = 1;
	m_ObjectType = OBJECT_MESH;
	m_DispType = 1;
	m_bShow = true;
	m_Num = 0;
	m_Name = "mesh";
	m_bObjChanged = true;
	}
XMesh::XMesh(const XMesh& mesh)
	{
	*this = mesh;
	}
XMesh::~XMesh()
	{
	ClearMesh();
	}
	void XMesh::SetUnit(const int& unit)
	{
	}
	void XMesh:: InitObject() 
	 {
	 }

bool XMesh:: Load_STL_File( CFile*& loadfile)
	{
	if( loadfile == NULL)
		{	
		return false;			
		}
	char tmp[2];
	int i;
	float nx, ny, nz;
	double mag; 
	XFacet facet;
	loadfile->Read(m_Name.GetBuffer(80), 80);
	loadfile->Read(&m_Num, 4);
	m_Facet.reserve(m_Num);
	for( i=0; i< m_Num  ; i++)//
		{
		loadfile->Read(&nx,4);
		loadfile->Read(&ny,4);
		loadfile->Read(&nz,4);
		facet.m_Norm=PNT(nx,ny,nz);
		mag = (nx*nx+ ny*ny+ nz*nz);
		loadfile->Read(&nx,4);
		loadfile->Read(&ny,4);
		loadfile->Read(&nz,4);
		facet.m_Vertex[0]=PNT(nx,ny,nz);

		loadfile->Read(&nx,4);
		loadfile->Read(&ny,4);
		loadfile->Read(&nz,4);
		facet.m_Vertex[1]=PNT(nx,ny,nz);

		loadfile->Read(&nx,4);
		loadfile->Read(&ny,4);
		loadfile->Read(&nz,4);
		facet.m_Vertex[2]=PNT(nx,ny,nz);

		loadfile->Read(tmp, 2);
		m_Facet.push_back(facet);
		}
	return true;
	}

/*
void XMesh::Draw(void)
	{

	if(!m_bShow)
		return;
	if(this->m_bSelected)
		{
		this->DrawBndBox();
		}

	XFacet facet;
	//glShadeModel(GL_FLAT);
	glFrontFace(GL_CCW );
	glLineWidth(0.1f); 
	ApplyColor();
	//	glColor3f(0, 1, 0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_shininess[] = { 7.0f };
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	mat_specular[0] = 0.6;
	mat_specular[1] = 0.6;
	mat_specular[2] = 0.6;
	mat_specular[3] = 1;
	glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular);

	//glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);
	switch (m_DispType)
		{
		case 0:
			glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);
			break;
		case 1:
			glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
			glEnable(GL_LIGHTING);
			break;
		}

	glBegin(GL_TRIANGLES);
	for(int i = 0; i < m_Facet.size(); i++)
		{
		facet = m_Facet[i];
		glNormal3f(	facet.m_Norm[0],
			facet.m_Norm[1],
			facet.m_Norm[2]);
		glVertex3f(	facet.m_Vertex[0][0],
			facet.m_Vertex[0][1],
			facet.m_Vertex[0][2]);
		glVertex3f(	facet.m_Vertex[1][0],
			facet.m_Vertex[1][1],
			facet.m_Vertex[1][2]);
		glVertex3f(	facet.m_Vertex[2][0],
			facet.m_Vertex[2][1],
			facet.m_Vertex[2][2]);
		}
	glEnd();
	if(m_DispType==1)
		glDisable(GL_LIGHTING);
	}
*/
int XMesh::GetFacetNum(void) const
	{
	return  m_Facet.size();
	}

void XMesh::AddFacet(const XFacet& facet)
	{
	m_Facet.push_back(facet);
	m_Num ++;
	}

void XMesh::DeleteFacet(int index)
	{
	std::vector<XFacet>::iterator beginIndex;
	beginIndex = m_Facet.begin();
	m_Facet.erase(beginIndex+index);	
	}

void XMesh::ClearMesh(void)
	{
	if(m_Facet.size() > 0)
		{
		m_Facet.clear();
		m_Num = 0;
		}
	}

void XMesh::UpdateBndBox(void)
	{
	if(m_bObjChanged)
		{
		int i;
		PNT	cntpnt;
		m_bObjChanged = false;
		m_BndBox.Reset();
		int num = m_Facet.size();
		for( i =0; i <num ; i++)
			{
			m_Facet[i].GetCenterPnt(cntpnt);
			m_BndBox.AddVertex(cntpnt);
			}
		}	
	}

void XMesh::GetFacet(const int& index, XFacet& facet)const 
	{
#ifdef _DEBUG
	assert(index >= 0&& index <m_Num);
#endif
	facet = m_Facet[index];
	}

const int XMAX = 30;
const int YMAX = 30;
const int ZMAX = 30;
//double XMesh::Cloud2Mesh_Dist( XCloud& cloud, XCloud& nearestcld, const double& tolerance, int& num_withintol) const 
//	{
//	double dist, RMS_error = 0;
//	//long counter;
//	long num = cloud.GetPntNum();
//	if(num == 0 || this->GetFacetNum() ==0)
//		return -1;
//	num_withintol = 0;
//	nearestcld.ClearCloud();
//	nearestcld.SetCapacity(num);
//	XCloud	 source_cld(num);
//	if (this->GetFacetNum()< 2000)
//		{	
//		PNT tmpnt, nearestpnt;
//		int i;
//		for(i=0, RMS_error =0; i< num; i++)
//			{
//			cloud.GetPoint(i, tmpnt);
//			dist = Pnt2NearestMeshPnt(tmpnt, tolerance,nearestpnt);
//			if(fabs(dist+1000000)>1e-9)
//				{
//				nearestcld.AddPoint(nearestpnt);
//				source_cld.AddPoint(tmpnt);				 
//				RMS_error += (tmpnt - nearestpnt)*(tmpnt - nearestpnt);
//				num_withintol ++;
//				}
//			}		
//		RMS_error = sqrt(RMS_error/num_withintol);
//		}
//	else
//		{
//		double minx,miny,minz;
//		double mindist ,dist;
//		XCloud  Facetcld(GetFacetNum());
//		int l,m,n;
//		int i,j,k,t, r;
//		int x_s, y_s, z_s, x_e, y_e, z_e;
//		int	counter_pnt,  // point number in bincell
//			counter_Fa,  // facet number in bincell
//			tri_index;       // the facet index in which nearest point located
//		PNT     pnt, NearPnt, nearestpnt;
//		XFacet	     facet;
//		// start  filling the points and facets into voxel
//		std::vector<int> pvoxel[2][XMAX+1][YMAX+1][ZMAX+1]; //storage points in the voxel (XMAX+1)*(YMAX+1)*(ZMAX+1)
//		double xstep, ystep, zstep; // the division step of voxel 
//
//		for(i=0; i< m_Num; i++)
//			{
//			m_Facet[i].GetCenterPnt(pnt);
//			Facetcld.AddPoint(pnt);
//			}
//		XBndBox  bndbox = cloud.GetBBox() +Facetcld.GetBBox();
//		PNT  v, bottompnt,  uperpnt;
//		bottompnt	= bndbox.GetMin();
//		uperpnt		= bndbox.GetMax();
//		v			= uperpnt - bottompnt;
//		minx		= bottompnt[0];
//		miny		= bottompnt[1];
//		minz		= bottompnt[2];
//
//		xstep = (v[0] + 0.001) / XMAX;	//
//		ystep = (v[1] + 0.001) / YMAX;
//		zstep = (v[2] + 0.001) / ZMAX;
//		for( i = 0; i< num; i++)
//			{
//			cloud.GetPoint(i,pnt);
//			l	= (pnt[0] - minx)/xstep;
//			m	= (pnt[1] - miny)/ystep;
//			n	= (pnt[2] - minz)/zstep;
//#ifdef _DEBUG
//			ASSERT(l>=0 && l <= XMAX && m >= 0 && m <= YMAX && n >= 0 && n <= ZMAX);
//#endif
//			pvoxel[0][l][m][n].push_back(i);
//			}
//
//		for(i = 0; i< m_Num; i++)
//			{
//			Facetcld.GetPoint(i,pnt);
//			l	= (pnt[0] - minx)/xstep;
//			m	= (pnt[1] - miny)/ystep;
//			n	= (pnt[2] - minz)/zstep;
//#ifdef _DEBUG
//			ASSERT(!(l < 0 || l > XMAX||
//				m < 0 || m > YMAX||
//				n  <0 || n>ZMAX));
//#endif
//			pvoxel[1][l][m][n].push_back(i);
//			}
//		// end filling the points and facets into box
//
//		RMS_error = 0;
//		int layer_num = 1;     
//		int index;
//		bool b_Void = true; // no facet in the bincells
//		for(l = 0; l < XMAX+1; l++)
//			for( m = 0; m < YMAX+1; m++)
//				for(n = 0; n < ZMAX+1; n++)
//					{ 
//					counter_pnt= pvoxel[0][l][m][n].size();
//					for(i = 0; i< counter_pnt;  i++)
//						{	
//						// -------strat compute the neigbor bincells-------------
//						if( l -layer_num < 0)
//							{
//							x_s =0;
//							x_e = layer_num;
//							if(x_e > XMAX)
//								x_e = XMAX;
//							}
//						else
//							{
//							x_s = l -layer_num;
//							x_e = l+layer_num;
//							if(x_e > XMAX)
//								x_e = XMAX;
//							}
//						if( m -layer_num < 0)
//							{
//							y_s =0;
//							y_e = layer_num ;
//							if(y_e > YMAX)
//								y_e = YMAX;
//							}
//						else
//							{
//							y_s = m-layer_num;
//							y_e = m+layer_num;
//							if(y_e > YMAX)
//								y_e = YMAX;
//							}
//						if( n -layer_num < 0)
//							{
//							z_s =0;
//							z_e = layer_num;
//							if(z_e> ZMAX)
//								z_e = ZMAX;
//							}
//						else
//							{
//							z_s = n-layer_num;
//							z_e = n+layer_num;
//							if(z_e> ZMAX)
//								z_e = ZMAX;
//							}
//						//-----------end compute neighbor bincells------------------------------------------
//
//						mindist = tolerance; 
//						cloud.GetPoint(pvoxel[0][l][m][n][i], pnt);
//						for(r =x_s; r <= x_e; r++)
//							for(k =y_s; k<= y_e; k++)
//								for(t =z_s; t<= z_e; t++)
//									{
//									counter_Fa= pvoxel[1][r][k][t].size();
//									for(j =0; j< counter_Fa; j++)
//										{
//										b_Void = false; //  have facet
//										index = pvoxel[1][r][k][t][j];
//#ifdef _DEBUG
//										assert(index >=0 && index < m_Num);
//#endif
//										//if(m_pFacet[pvoxel[1][r][k][t][j]].Pnt2Facet_Dist(pnt, nearestpnt, dist,tolerance))
//										if(m_Facet[index].Pnt2Facet_Dist(pnt, nearestpnt, dist,tolerance))
//											{
//											if(dist < mindist)
//												{
//												mindist = dist ;
//												NearPnt = nearestpnt;
//												tri_index = index;
//												}
//											}
//										}
//									}
//								if(!b_Void)
//									{
//									nearestcld.AddPoint(NearPnt);
//									source_cld.AddPoint(pnt);
//									RMS_error += mindist*mindist;
//
//#ifdef _DEBUG
//									if(fabs(mindist- (NearPnt - pnt).Length()) >1e-5)
//										{
//										AfxMessageBox("Please manually adjust the model more closely");
//										//RMS_error = -1;
//										//break;
//										}
//#endif
//									num_withintol ++;
//									if( i == counter_pnt-1)
//										{
//										b_Void = true;
//										layer_num = 1;
//										}
//									}
//								else
//									{
//									n--;
//									layer_num++;									
//									break;
//									}
//						}
//					}
//				cloud.ClearCloud();
//				cloud = source_cld;
//				RMS_error = sqrt(RMS_error/num_withintol);
//
//				// 1. free the voxel array
//				for(l = 0; l < XMAX+1; l++)
//					for( m = 0; m < YMAX+1; m++)
//						for(n = 0; n < ZMAX+1; n++)
//							{
//							pvoxel[0][l][m][n].clear();	
//							pvoxel[1][l][m][n].clear();	
//							}
//		}
//	return RMS_error;
//	}
//
//double XMesh::Mesh_ErrorEvaluation(
//				   XCloud& cloud, 
//				   XErrorCloud&  errorcld, 
//				   const double& tolerance
//				   ) const 
//	{
//	double dist, RMS_error = 0;
//	XCloud nearestcld;
//	Ti_Timer timer;
//	long num = cloud.GetPntNum();
//	errorcld.m_Error.reserve(num);
//	nearestcld.SetCapacity(num);
//	XCloud	 source_cld(num);
//	errorcld.m_Error.reserve(num);
//	int step = (XMAX*YMAX)/100;
//	char text[10];
//	if(step==0) 
//		step++;	
//	Diag_Progress ProgressDlg;
//	ProgressDlg.Create(Diag_Progress::IDD);
//	ProgressDlg.m_ProgressCtrl.SetPos(0);
//	ProgressDlg.m_ProgressCtrl.SetRange32(0,100);
//	//	ProgressDlg.m_ProgressCtrl.SetStep(step);
//
//	if (this->GetFacetNum()< 2000)
//		{	
//		PNT tmpnt, nearestpnt;
//		int i;
//		for(i=0, RMS_error =0; i< num; i++)
//			{
//			cloud.GetPoint(i, tmpnt);
//			dist = Pnt2NearestMeshPnt(tmpnt, tolerance,nearestpnt);
//			if(fabs(dist+1000000)>1e-9)
//				{
//				nearestcld.AddPoint(nearestpnt);
//				source_cld.AddPoint(tmpnt);				 
//				RMS_error += (tmpnt - nearestpnt)*(tmpnt - nearestpnt);
//				errorcld.m_Error.push_back(dist);
//				}
//			}		
//		RMS_error = sqrt(RMS_error/source_cld.GetPntNum());
//		}
//	else
//		{
//		double minx,miny,minz;
//		double mindist ,dist;
//		CString str;
//
//		XCloud  Facetcld(GetFacetNum());
//		int l,m,n;
//		int i,j,k,t, r, jj;
//		int x_s, y_s, z_s, x_e, y_e, z_e;
//		int	counter_pnt,  // point number in bincell
//			counter_Fa,  // facet number in bincell
//			tri_index;       // the facet index in which nearest point located
//		PNT     pnt, NearPnt, nearestpnt;
//		XFacet	     facet;
//		// start  filling the points and facets into voxel
//		std::vector<int> pvoxel[2][XMAX+1][YMAX+1][ZMAX+1]; //storage points in the voxel (XMAX+1)*(YMAX+1)*(ZMAX+1)
//		double xstep, ystep, zstep; // the division step of voxel 
//
//		timer.TimerBegin();
//		for(i=0; i< m_Num; i++)
//			{
//			m_Facet[i].GetCenterPnt(pnt);
//			Facetcld.AddPoint(pnt);
//			}
//		XBndBox  bndbox = cloud.GetBBox() +Facetcld.GetBBox();
//		PNT  v, bottompnt,  uperpnt;
//		bottompnt	= bndbox.GetMin();
//		uperpnt		= bndbox.GetMax();
//		v			= uperpnt - bottompnt;
//		minx		= bottompnt[0];
//		miny		= bottompnt[1];
//		minz		= bottompnt[2];
//
//		xstep = (v[0] + 0.001) / XMAX;	//
//		ystep = (v[1] + 0.001) / YMAX;
//		zstep = (v[2] + 0.001) / ZMAX;
//		for( i = 0; i< num; i++)
//			{
//			cloud.GetPoint(i,pnt);
//			l	= (pnt[0] - minx)/xstep;
//			m	= (pnt[1] - miny)/ystep;
//			n	= (pnt[2] - minz)/zstep;
//#ifdef _DEBUG
//			ASSERT(l>=0 && l <= XMAX && m >= 0 && m <= YMAX && n >= 0 && n <= ZMAX);
//#endif
//			pvoxel[0][l][m][n].push_back(i);
//			}
//
//		for(i = 0; i< m_Num; i++)
//			{
//			Facetcld.GetPoint(i,pnt);
//			l	= (pnt[0] - minx)/xstep;
//			m	= (pnt[1] - miny)/ystep;
//			n	= (pnt[2] - minz)/zstep;
//#ifdef _DEBUG
//			ASSERT(!(l < 0 || l > XMAX||
//				m < 0 || m > YMAX||
//				n  <0 || n>ZMAX));
//#endif
//			pvoxel[1][l][m][n].push_back(i);
//			}
//		jj =2;
//		wsprintf(text,"%d%%",jj);
//		ProgressDlg.m_ProgressCtrl.SetPos(1);			
//		ProgressDlg.GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(text);
//		// end filling the points and facets into box
//
//		RMS_error = 0;
//		int layer_num = 1;     
//		int index;
//		bool b_Void = true; // no facet in the bincells
//
//		for(l = 0; l < XMAX+1; l++)
//			for( m = 0; m < YMAX+1; m++)
//				{
//				if((l*YMAX+m)%step==1)
//					{
//					jj++;
//					wsprintf(text,"%d%%",jj);
//					ProgressDlg.m_ProgressCtrl.SetPos(jj);			
//					ProgressDlg.GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(text);
//					}
//
//				for(n = 0; n < ZMAX+1; n++)
//					{ 
//					counter_pnt= pvoxel[0][l][m][n].size();
//					for(i = 0; i< counter_pnt;  i++)
//						{	
//
//						// -------strat compute the neigbor bincells-------------
//						if( l -layer_num < 0)
//							{
//							x_s =0;
//							x_e = layer_num;
//							if(x_e > XMAX)
//								x_e = XMAX;
//							}
//						else
//							{
//							x_s = l -layer_num;
//							x_e = l+layer_num;
//							if(x_e > XMAX)
//								x_e = XMAX;
//							}
//						if( m -layer_num < 0)
//							{
//							y_s =0;
//							y_e = layer_num ;
//							if(y_e > YMAX)
//								y_e = YMAX;
//							}
//						else
//							{
//							y_s = m-layer_num;
//							y_e = m+layer_num;
//							if(y_e > YMAX)
//								y_e = YMAX;
//							}
//						if( n -layer_num < 0)
//							{
//							z_s =0;
//							z_e = layer_num;
//							if(z_e> ZMAX)
//								z_e = ZMAX;
//							}
//						else
//							{
//							z_s = n-layer_num;
//							z_e = n+layer_num;
//							if(z_e> ZMAX)
//								z_e = ZMAX;
//							}
//						//-----------end compute neighbor bincells------------------------------------------
//
//						mindist = tolerance; 
//						cloud.GetPoint(pvoxel[0][l][m][n][i], pnt);
//						for(r =x_s; r <= x_e; r++)
//							for(k =y_s; k<= y_e; k++)
//								for(t =z_s; t<= z_e; t++)
//									{
//									counter_Fa= pvoxel[1][r][k][t].size();
//									for(j =0; j< counter_Fa; j++)
//										{
//										b_Void = false; //  have facet
//										index = pvoxel[1][r][k][t][j];
//#ifdef _DEBUG
//										assert(index >=0 && index < m_Num);
//#endif
//										//if(m_pFacet[pvoxel[1][r][k][t][j]].Pnt2Facet_Dist(pnt, nearestpnt, dist,tolerance))
//										if(m_Facet[index].Pnt2Facet_Dist(pnt, nearestpnt, dist,tolerance))
//											{
//											if(dist < mindist)
//												{
//												mindist = dist ;
//												NearPnt = nearestpnt;
//												tri_index = index;
//												}
//											}
//										}
//									}
//								if(!b_Void)
//									{
//									nearestcld.AddPoint(NearPnt);
//									source_cld.AddPoint(pnt);
//									if(m_Facet[tri_index].m_Norm*(pnt - NearPnt) > 0)
//										errorcld.m_Error.push_back(mindist);
//									else
//										errorcld.m_Error.push_back(-mindist);
//#ifdef _DEBUG
//									if(fabs(mindist- (NearPnt - pnt).Length()) >1e-5)
//										ASSERT(FALSE);
//#endif
//									RMS_error += mindist*mindist;
//									if( i == counter_pnt-1)
//										{
//										b_Void = true;
//										layer_num = 1;
//										}
//									}
//								else
//									{
//									n--;
//									layer_num++;									
//									break;
//									}
//						}
//					}
//				}
//			ProgressDlg.OnCancel();
//			cloud.ClearCloud();
//			cloud = source_cld;
//			RMS_error = sqrt(RMS_error/cloud.GetPntNum());
//			timer.TimerStop();
//			SYSTEMTIME sum_time = timer.GetResultTime();
//			str.Format("%d minutes, %d seconds, %d milseconds", sum_time.wMinute, sum_time.wSecond, sum_time.wMilliseconds);
//			AfxMessageBox(str);
//
//			// 1. free the voxel array
//			for(l = 0; l < XMAX+1; l++)
//				for( m = 0; m < YMAX+1; m++)
//					for(n = 0; n < ZMAX+1; n++)
//						{
//						pvoxel[0][l][m][n].clear();	
//						pvoxel[1][l][m][n].clear();	
//						}
//		}
//	errorcld.SetPointCld(nearestcld);
//	errorcld.ComputeError(cloud);
//	return RMS_error;
//	}
//
double XMesh::Pnt2NearestMeshPnt(const PNT& refpnt, const double& tolerance , PNT& nearestpnt) const
	{
	int i;
	double mindist, tmpdist;
	int index = -1;
	mindist = 10000;
	PNT pnt;
	for(i=0; i < m_Num; i++)
		{
		if( m_Facet[i].Pnt2Facet_Dist(refpnt,pnt, tmpdist, tolerance)) // revised for debug mesh reg Mar.17
			{
			if(tmpdist < mindist)
				{
				mindist = tmpdist;
				nearestpnt = pnt;
				index = i;
				}
			}
		}
	if(index == -1)
		return -1000000;
	if(m_Facet[index].m_Norm*(refpnt- pnt) > 0)
		return mindist;
	else
		return - mindist;
	}

//bool XMesh::SeekEdges(std::vector<XCloud>& edgelist, std::vector<bool>& type_edge)
//	{
//	if(m_Num < 1)
//		return false;
//
//	int  **pedge_state = NULL;  
//	// which is used to store edge information
//	pedge_state = allocate_integer_matrix(0, m_Num-1, 0, 2);	
//	// pedge_state[k][2] = 1  means facet k's edge 2 has neighbor facet 
//	assert(pedge_state != NULL);
//
//	XFacet facet0, facettest;
//	int i, j, k, l, type, edge_f0, edge_f1;
//	for(i=0;i<m_Num;i++)
//		{
//		for(k=0;k<3;k++)
//			{
//			pedge_state[i][k]=0;
//			}
//		}
//
//	for(i = 0; i< m_Num-1; i++)
//		{
//		//whether or not found a coedge
//		for(j = i+1; j< m_Num; j++)
//			{
//			type = 0;
//			if(m_Facet[i].IsNeighbor(m_Facet[j], edge_f0, edge_f1, type))
//				{
//				if(type == 1)
//					{
//					pedge_state[i][edge_f0] = 1;
//					pedge_state[j][edge_f1] = 1;
//					}
//				}
//			}
//		}
//
//	/* ------------end check facet edge state---------------*/
//
//	bool bNewEdge,bEndLoop;
//	vector<int> index;
//	XCloud edge;
//	PNT sVertex, cVertex;	
//
//
//	//find a start edge and continue;
//	for(i=0; i<m_Num; i++)
//		{
//		for(j=0; j<3; j++)
//			{
//			index.clear();
//			if( pedge_state[i][j] == 0)
//				{
//				sVertex = m_Facet[i].m_Vertex[j];
//				cVertex = m_Facet[i].m_Vertex[(j+1)%3];
//				edge.InitObject();
//				edge.AddPoint(sVertex);
//				edge.AddPoint(cVertex);
//				pedge_state[i][j] = 1; //remove edge
//				index.push_back(i);
//
//				bNewEdge = false;
//				bEndLoop = false;
//				for(k=i; k<m_Num; k++) 
//					{
//					for(l=0; l<3; l++)
//						{
//						if( pedge_state[k][l]==0&&cVertex==m_Facet[k].m_Vertex[l])
//							{			
//							cVertex = m_Facet[k].m_Vertex[(l+1)%3];
//							edge.AddPoint(cVertex);
//							pedge_state[k][l] = 1;//remove edge
//							index.push_back(k);
//							bNewEdge = true;
//							if(sVertex==cVertex) 
//								{
//								bEndLoop = true;
//								break;	
//								}
//							}
//						}
//					if(bNewEdge) {k=i;bNewEdge=false;}
//				if(bEndLoop) break;
//					}
//				DivideMeshEdge(edge,index,edgelist,type_edge);
//				}
//			}
//		}
//
//#ifdef _DEBUG
//	bool bx[5] = {false};
//	int xx[5] = {0};
//	int x = edgelist.size();
//	for(i=0;i<5;i++)
//		{
//		xx[i] = edgelist[i].GetPntNum();
//		bx[i] = type_edge[i];
//		}
//#endif
//
//	if(pedge_state != NULL)
//		free_integer_matrix(pedge_state, 0, m_Num-1, 0);
//	return true;
//	}
//
bool XMesh::GetVisibalMesh(const PNT& view_dir, const double & view_angle, XMesh& ex_mesh)
	{
	double thresh_cosangle = cos(view_angle/180.0*PI);
	XFacet facet;
	XMesh mesh(*this);
	/*PNT  v_z(0, 0, 1),  v_a, origin;
	double rot_angle;
	rot_angle = acos(view_dir*v_z);
	if(rot_angle > 1e-6)
	{
	v_a = view_dir.CrossProduct(v_z);
	v_a.Normalize();
	mesh.Rotate(origin, v_a, rot_angle);	
	}*/
	ex_mesh.ClearMesh();
	for(int i =0; i< mesh.GetFacetNum(); i++)
		{
		facet = this->m_Facet[i];
		if(facet.m_Norm[2] > thresh_cosangle)
			ex_mesh.AddFacet(facet);
		}
	ex_mesh = ex_mesh.HidenFacet(view_dir);

	/*if(rot_angle > 1e-6)
	ex_mesh.Rotate(origin, v_a, -rot_angle);*/	
	if(ex_mesh.GetFacetNum() > 1)
		return true;
	else 
		return false;
	}
/*
int XMesh::Cloud2Mesh_Reg(
			  XCloud&				cloud,
			  const PNT&		pnt_axis,
			  const PNT&		dir,
			  double&				rotation_angle,
			  double&				trans, 
			  double &				rms_error  
			  )
	{
	double r=0, t=0;
	double par[10];
	double out[8];
	//Ti_Cloud NearCld;
	rotation_angle = 0;
	trans               = 0;
	PNT   pnt;
	XFacet facet;

	par[1] = par[2]  = 0;
	par[3] = pnt_axis[0];
	par[4] = pnt_axis[1];
	par[5] = pnt_axis[2];
	par[6] = dir[0];
	par[7] = dir[1];
	par[8] = dir[2];
	// end initialization

	Ti_Optimization algorithm;
	//double** g_pnt = NULL; 
	double temp = 0;
	double	in[7],
		*rv,
		**jjinv;
	int m = cloud.GetPntNum();
	jjinv = allocate_real_matrix(1,2,1,2);	
	in[0]=1.0e-31; 
	in[3]=1.0e-4;	//the relative tolerance for the difference between  the Euclidean norm 
	//of the ultimate and penultimate residual vector 
	in[4]=1.0e-5;	// the absolute tolerence for the difference between  the Euclidean norm
	//of the ultimate and penultimate residual vector
	in[5]=30;		 //the maximum number of calls of funct allowed
	in[6]=1.0e-6;	//a starting value used for the relation between 
	// the gradient and the Gauss-Newton direction (value ¦Îabove);
	// if the problem is well conditioned then in[6] should be greater,
	// but the value of in[6] should satisfy:in[0]<in[6]<=1/in[0]
	if( m> 10000)
		{
		XCloud  roughcld;
		roughcld = cloud;
		roughcld.SampleUniform(m/5000);
		m = roughcld.GetPntNum();
		rv	  =  allocate_real_vector(1, m);
		MarquardtforMeshReg(
			m,
			2,
			roughcld,
			*this,
			par,
			rv,
			jjinv,
			EvaluateforMeshReg,
			JacobianforMeshReg,
			in,
			out);
		rotation_angle = par[1];
		trans     = par[2];
		if( rv != NULL)
			{
			free_real_vector(rv,1);			
			rv = NULL;
			}
		}
	else
		{
		m = cloud.GetPntNum();
		//in[3]=1.0e-4;     //the relative tolerance for the difference between  the Euclidean norm of the ultimate and penultimate residual vector 
		//in[4]=1.0e-5; // the absolute tolerence for the difference between  the Euclidean norm of the ultimate and penultimate residual vector
		//in[5]=15;   //the maximum number of calls of funct allowed
		//in[6] = 1.0e-6;
		rv	  =  allocate_real_vector(1, m);
		MarquardtforMeshReg(
			m,
			2,
			cloud,
			*this,
			par,
			rv,
			jjinv,
			EvaluateforMeshReg,
			JacobianforMeshReg,
			in,
			out);
		rotation_angle	= par[1];
		trans			= par[2];
		if( rv != NULL)
			{
			free_real_vector(rv,1);
			}
		}
	//cloud.Translate(dir*trans);
	//cloud.Rotate(pnt_axis, dir, rotation_angle);
	if (jjinv != NULL)
		{
		free_real_matrix(jjinv,1,2,1);		
		jjinv = NULL;
		}
	rms_error = sqrt(out[2]/ m);
	return out[5];
	}
*/
void XMesh::Rotate(
		   const PNT	& pnt_axis,	 // point on axis
		   const PNT	& dir,		// unit direction vector
		   const double		& angle		// rotation angle in rad
		   )
	{
	double s_thita, c_thita, vers_thita;
	s_thita = sin(angle);
	c_thita = cos(angle);
	vers_thita = 1- c_thita;
	double  trans[3][3];
	double x, y, z; 
	x = dir[0];
	y = dir[1];
	z = dir[2];

	trans[0][0] = x*x*vers_thita+ c_thita;
	trans[0][1] = y*x*vers_thita - z*s_thita;
	trans[0][2] = z*x*vers_thita + y*s_thita;	  
	trans[1][0] = x*y*vers_thita + z*s_thita;
	trans[1][1] = y*y*vers_thita + c_thita;
	trans[1][2] = y*z*vers_thita - x*s_thita;
	trans[2][0] = x*z*vers_thita - y*s_thita;
	trans[2][1] = y*z*vers_thita +x*s_thita;
	trans[2][2] = z*z*vers_thita + c_thita;

	int num = this->GetFacetNum();
	PNT  pnt;
	XFacet facet;
	int i, j;   
	for( i=0; i< num; i++)
		{
		facet = m_Facet[i];
		pnt = facet.m_Norm;
		//pnt = pnt - pnt_axis;
		facet.m_Norm[0] =	trans[0][0]*pnt[0] + 
			trans[0][1]*pnt[1] +
			trans[0][2]*pnt[2] ;
		facet.m_Norm[1] =	trans[1][0]*pnt[0] +
			trans[1][1]*pnt[1] +
			trans[1][2]*pnt[2] ;
		facet.m_Norm[2] =	trans[2][0]*pnt[0] +
			trans[2][1]*pnt[1] + 
			trans[2][2]*pnt[2] ;
		//facet.m_Norm = facet.m_Norm + pnt_axis;

		for(j =0; j< 3; j++)
			{
			pnt = facet.m_Vertex[j];
			pnt = pnt - pnt_axis;
			facet.m_Vertex[j][0] =	trans[0][0]*pnt[0] + 
				trans[0][1]*pnt[1] +
				trans[0][2]*pnt[2] ;
			facet.m_Vertex[j][1] =	trans[1][0]*pnt[0] + 
				trans[1][1]*pnt[1] +
				trans[1][2]*pnt[2] ;
			facet.m_Vertex[j][2] =	trans[2][0]*pnt[0] +
				trans[2][1]*pnt[1] +
				trans[2][2]*pnt[2] ;
			facet.m_Vertex[j] = facet.m_Vertex[j] + pnt_axis;
			}
		m_Facet.at(i) =  facet;
		this->m_Num = num;
		m_bObjChanged = true;
		}
	}

bool XMesh::Optimize_ViewDir(
			     PNT&		view_dir,
			     double &			tilt_angle,
			     double &			Q_angle,
			     bool				bConstr
			     )
	{
	PNT  tmp_dir, norm_pnt;
	int i, j, k;
	int Num  = GetFacetNum();
	XCloud mesh_norm;
	mesh_norm.SetCapacity(Num);
	for( i = 0; i< Num; i++)
		{
		mesh_norm.AddPoint(m_Facet[i].m_Norm);
		}
	double	dist0,
		dist1,
		mindist = 10; // overall minimun distance to view dir

	//view_dir[0] = 0;
	//view_dir[1] = 0;
	//view_dir[2] = 0;
	//for(i=0; i< Num; i++)
	// {
	// mesh_norm.GetPoint(i, pnt);
	// view_dir += pnt;
	// }
	//view_dir.Normalize();
	//tilt_angle = acos(view_dir[1])*180.0/PI;
	//Q_angle = asin(view_dir[0]/sqrt(1-view_dir[1]*view_dir[1]))*180.0/PI;

	if(!bConstr)
		{
		for (i=0; i<=120; i +=15)
			{
			for (j=0; j<360; j +=5)
				{
				tmp_dir[0] = -sin(i/180.f*PI)*sin(j/180.f*PI);
				tmp_dir[1] = cos(i/180.f*PI);
				tmp_dir[2] = sin(i/180.f*PI)*cos(j/180.f*PI); 
				tmp_dir.Normalize();

				dist1 = 0;
				for (k=0; k < Num; k++)
					{
					mesh_norm.GetPoint(k, norm_pnt);
					dist0 = (norm_pnt- tmp_dir).Length();
					if( dist0 > dist1)
						{
						dist1 = dist0;
						}
					/*if( dist0 > 1.4)
					break;*/
					}
				/*if(dist1 > 1.39)
				continue;*/
				if( dist1 < mindist)
					{
					mindist = dist1;
					view_dir = tmp_dir;
					tilt_angle = i;
					Q_angle = j;
					}
				}		
			}
		}
	else
		{
		for (j=0; j<360; j +=5)
			{
			tmp_dir[0] = -sin(tilt_angle/180.f*PI)*sin(j/180.f*PI);
			tmp_dir[1] = cos(tilt_angle/180.f*PI);
			tmp_dir[2] = sin(tilt_angle/180.f*PI)*cos(j/180.f*PI); 
			tmp_dir.Normalize();

			dist1 = 0;
			for (k=0; k < Num; k++)
				{
				mesh_norm.GetPoint(k, norm_pnt);
				dist0 = (norm_pnt- tmp_dir).Length();
				if( dist0 > dist1)
					{
					dist1 = dist0;
					}
				/*if( dist0 > 1.4)
				break;*/
				}
			/*if(dist1 > 1.39)
			continue;*/
			if( dist1 < mindist)
				{
				mindist = dist1;
				view_dir = tmp_dir;
				Q_angle = j;
				}
			}	
		}
	return true;
	}
/*
int __cdecl JacobianforMeshReg(
				int m,		// number of points 
				int n,		// number of parameters =2
				double *const  par,	// par[1] : rotaion angle
				// par[2]: tranlate value
				// par[3-5] point on axis
				// par[6-8] axis direction
				double*& rv,   // residual value,e.g. distance diviation
				double **jac,
				const XCloud&  g_pnt,
				const XMesh&  g_mesh
				)
	{
	PNT  spnt, rpnt, Vec, v, m_dir, m_pnt_line;
	XCloud  tmpcld,   
		nearcld;
	tmpcld					= g_pnt;
	m_pnt_line[0]	= par[3];
	m_pnt_line[1]	= par[4];
	m_pnt_line[2]	= par[5];
	m_dir[0]		= par[6];
	m_dir[1]		= par[7];
	m_dir[2]		= par[8];

	Vec = m_dir*(par[2]);
	tmpcld.Translate(Vec);
	tmpcld.Rotate(m_pnt_line, m_dir, par[1]);
	if(g_mesh.Cloud2Mesh_Dist(tmpcld, nearcld, 2, m) == -1)
		return -1;
	m = tmpcld.GetPntNum();
	for( int i =0; i< m; i++)
		{
		tmpcld.GetPoint(i, spnt);
		nearcld.GetPoint(i, rpnt);
		rv[i+1] = (spnt- rpnt).Length();
		if( rv[i+1]  > 1e-6)
			{
			v = spnt - Vec - m_pnt_line;
			v = m_dir.CrossProduct(v);
			jac[i+1][1] = (v*(spnt- rpnt))/ rv[i+1];
			jac[i+1][2] = ((spnt-rpnt)*m_dir)/rv[i+1];
			}
		else
			{
			jac[i+1][1] = 0;
			jac[i+1][2] = 0;
			}
		}
	return 1;
	}

int __cdecl EvaluateforMeshReg(
			       int		m,
			       int		n,
			       double *const par, 
			       double* rv, 
			       const XCloud& g_pnt,
			       const XMesh&  g_mesh
			       )
	{
	PNT  spnt, rpnt,Vec, m_dir, m_pnt_line;
	XCloud  tmpcld(g_pnt),
		nearcld;
	m_pnt_line[0]	= par[3];
	m_pnt_line[1]	= par[4];
	m_pnt_line[2]	= par[5];
	m_dir[0]		= par[6];
	m_dir[1]		= par[7];
	m_dir[2]		= par[8];
	Vec = m_dir*(par[2]);

	tmpcld.Translate(Vec);
	tmpcld.Rotate(m_pnt_line, m_dir, par[1]);
	g_mesh.Cloud2Mesh_Dist(tmpcld, nearcld, 2,m);
	m = tmpcld.GetPntNum();
	for( int i =0; i< m; i++)
		{
		tmpcld.GetPoint(i, spnt);
		nearcld.GetPoint(i, rpnt);
		rv[i+1] = (spnt- rpnt).Length();
		}
	return 1;
	}
*/
void XMesh::SetDisplay(bool bShow, int dis_type)
	{
	this->m_bShow = bShow;
	if(dis_type!=-1)
		{
		this->m_DispType = dis_type;
		}
	}

bool XMesh::Load_STL_File(CArchive & ar)
	{
	char tmp[2];
	int i;
	float nx, ny, nz;
	XFacet facet;
	ar.Read(m_Name.GetBuffer(80), 80);
	ar.Read(&m_Num, 4);
	m_Facet.reserve(m_Num);
	for( i=0;  i< m_Num; i++)//
		{
		ar.Read(&nx,4);
		ar.Read(&ny,4);
		ar.Read(&nz,4);
		facet.m_Norm=PNT(nx,ny,nz);

		ar.Read(&nx,4);
		ar.Read(&ny,4);
		ar.Read(&nz,4);
		facet.m_Vertex[0]=PNT(nx,ny,nz);

		ar.Read(&nx,4);
		ar.Read(&ny,4);
		ar.Read(&nz,4);
		facet.m_Vertex[1]=PNT(nx,ny,nz);

		ar.Read(&nx,4);
		ar.Read(&ny,4);
		ar.Read(&nz,4);
		facet.m_Vertex[2]=PNT(nx,ny,nz);

		ar.Read(tmp, 2);
		m_Facet.push_back(facet);
		}
//	this->SetUnits(UNITS_INCH);
	return true;
	}

bool XMesh::Save_STL_File(CArchive & ar) const
	{
	char  str[12] = "   ";
	int i, j;	
	ar.Write(m_Name, 80);
	ar<<(int)(m_Num);
	XFacet  facet;
	for(  i =0 ; i< m_Num ; i++)
		{
		GetFacet(i, facet);
		for(j = 0; j< 3; j++)
			{
			ar<< (float)(facet.m_Norm[j]);
			}
		for(j = 0; j< 3; j++)
			{
			ar<<(float) (facet.m_Vertex[0][j]);
			}
		for(j = 0; j< 3; j++)
			{
			ar<<(float)( facet.m_Vertex[1][j]);
			}
		for(j = 0; j< 3; j++)
			{
			ar<<(float)( facet.m_Vertex[2][j]);
			}
		ar.Write(str, 2);
		}
	return true;
	}



void XMesh::Translate(const PNT& trans)
	{
	int num = GetFacetNum();
	for(int i=0; i< num; i++)
		{
		XFacet&  facet = m_Facet.at(i);
		facet.m_Vertex[0]  = facet.m_Vertex[0] + trans;
		facet.m_Vertex[1]  = facet.m_Vertex[1] + trans;
		facet.m_Vertex[2]  = facet.m_Vertex[2] + trans;
		}
	m_bObjChanged = true;
	}

	void XMesh::Rotate(const Ti_Rotation& R)
	{
		Rotate(PNT(0,0,0), R.m_Axis, R.m_Angle*PI/180);
	this->m_bDispChanged = true;
	}

XMesh XMesh::operator - (const XMesh& mesh) const
	{
	XMesh m;
	int num, num1;
	num = GetFacetNum();
	num1 = mesh.GetFacetNum();
	PNT* centerList0 = new PNT[num];
	PNT* centerList1 = new PNT[num1];
	bool* bMatched = new bool[num1];
	ASSERT(centerList0!=NULL&&centerList1!=NULL&&bMatched!=NULL);

	int i,j;
	bool flag ;
	for(i=0;i<num;i++)	   
		{
		m_Facet[i].GetCenterPnt(centerList0[i]); 
		}
	for(i=0;i<num1;i++)
		{
		mesh.m_Facet[i].GetCenterPnt(centerList1[i]);
		bMatched[i] = false;
		}
	for(i=0;i<num;i++)
		{
		flag = false;
		for(j=0;j<num1;j++)
			{
			if(bMatched[j]) continue;
			if(centerList0[i]==centerList1[j])
				{
				bMatched[j] = true;
				flag = true;
				break;
				}
			}
		if(!flag)
			m.AddFacet(m_Facet[i]);
		}	
	if(centerList0 != NULL)
		{
		delete []centerList0;
		centerList0 = NULL;
		}
	if(centerList1 != NULL)
		{
		delete []centerList1;
		centerList1 = NULL;
		}
	if(bMatched != NULL)
		{
		delete []bMatched;
		bMatched = NULL;
		}
	return m;
	}
/*
void XMesh::DivideMeshEdge(const XCloud& edge,
			   const vector<int> index,
			   std::vector<XPolyline>& edgelist,
			   std::vector<bool>& type_edge)
	{
	int i,j,k,total=0;
	bool founded;
	vector<int> index1,index2;
	XPolyline  edge1,edge2;
	PNT cPnt0,cPnt1,tempPnt,centerPnt;
	XFacet checkFacet;	

	total = edge.GetPntNum();
	founded = false;
	for(i=0;i<total-1;i++)
		{
		edge.GetPoint(i,cPnt0);
		for(j=i+1;j<total-1;j++)
			{
			edge.GetPoint(j,cPnt1);
			if(cPnt0==cPnt1)
			 {
			 founded = true;
			 break;
			 }
		 }
		if(founded) break; 
		}

	if(founded)
		{

		//		 edge1.ClearCloud();
		//		 edge2.ClearCloud();	 
		for(k=i;k<=j;k++)
		 {
		 int xx = index[k];
		 index1.push_back(index[k]);
		 edge.GetPoint(k,tempPnt);
		 edge1.AddPoint(tempPnt);							
		 }
	 for(k=0;k<i;k++)
		 {
		 index2.push_back(index[k]);
		 edge.GetPoint(k,tempPnt);
		 edge2.AddPoint(tempPnt);							
		 }
	 for(k=j;k<total;k++)
		 {
		 index2.push_back(index[k]);
		 edge.GetPoint(k,tempPnt);
		 edge2.AddPoint(tempPnt);							
		 }
	 DivideMeshEdge(edge1,index1,edgelist,type_edge);
	 DivideMeshEdge(edge2,index2,edgelist,type_edge);		 
		}
	else
		{
		edgelist.push_back(edge); 
		//m_Facet[index[0]].GetCenterPnt(centerPnt);

		m_Facet[index[index.size()/2]].GetCenterPnt(centerPnt);
		if(edge.IsPntInPolyLine(centerPnt))  //centerPnt in the polyLine or not
			type_edge.push_back(true);
		//set edge types : true means outer boundary,false means inner
		else
			type_edge.push_back(false);
		}	 	 
	}
*/
XMesh XMesh::HidenFacet(const PNT& dir)const
	{
	PNT  trangle_pnt[3];
	int i,j;
	XFacet  facet;
	PNT pnt;
	XCloud  cld_center(m_Num);
	bool   * pflag = new bool[m_Num];
	assert(pflag != NULL);
	for( i = 0; i < m_Num; i++)
		{
		pflag[i] = true;
		}
	for(i=0; i< m_Num-1; i++)
		{
		this->m_Facet[i].GetCenterPnt(trangle_pnt[0]);
		for(j=i+1; j< m_Num; j++)
			{
			/*if(j == i)
			continue;	*/		
			m_Facet[j].GetCenterPnt(trangle_pnt[1]);
			trangle_pnt[2] =  trangle_pnt[0] -  trangle_pnt[1];
			if( trangle_pnt[2].Length2D() > 1.2)  // attention : if triangle's edge length is bigger than 1, 
				//then the threshhold value should be adjusted accordingly
				{
				continue;
				}
			if(m_Facet[j].IsPnt_in_Triangle(trangle_pnt[0]))
				{
				if( trangle_pnt[0][2] > trangle_pnt[1][2])
					{
					pflag[j] = false;
					}
				else
					{
					pflag[i] = false;
					}
				break;
				}
			}
		}
	XMesh mesh;
	for(i=0; i< m_Num; i++)
		{
		if(pflag[i])
			{
			mesh.AddFacet(m_Facet[i]);
			}
		}
	if(pflag != NULL)
		{
		delete []pflag;
		pflag = NULL;
		}
	return mesh;
	}


//////////////////////////////////////////////////////////////////////////
//new method
//////////////////////////////////////////////////////////////////////////
//bool XMesh::SeekEdges(
//		      PNT& view_dir,
//		      std::vector<XCloud>& edgelist, 
//		      std::vector<bool>& type_edge)
//	{
//	if(m_Num < 1)
//		return false;
//
//	int  **pedge_state = NULL;  
//	// which is used to store edge information
//	pedge_state = allocate_integer_matrix(0, m_Num-1, 0, 2);	
//	// pedge_state[k][2] = 1  means facet k's edge 2 has neighbor facet 
//	assert(pedge_state != NULL);
//
//	XFacet facet0, facettest;
//	int i, j, k, l, type, edge_f0, edge_f1;
//	for(i=0;i<m_Num;i++)
//		{
//		for(k=0;k<3;k++)
//			{
//			pedge_state[i][k]=0;
//			}
//		}
//
//	for(i = 0; i< m_Num-1; i++)
//		{
//		//whether or not found a coedge
//		for(j = i+1; j< m_Num; j++)
//			{
//			type = 0;
//			if(m_Facet[i].IsNeighbor(m_Facet[j], edge_f0, edge_f1, type))
//				{
//				if(type == 1)
//					{
//					pedge_state[i][edge_f0] = 1;
//					pedge_state[j][edge_f1] = 1;
//					}
//				}
//			}
//		}
//
//	/* ------------end check facet edge state---------------*/
//
//	bool bNewEdge,bEndLoop;
//	vector<int> index;
//	XCloud edge;
//	PNT sVertex, cVertex;	
//
//
//	//find a start edge and continue;
//	for(i=0; i<m_Num; i++)
//		{
//		for(j=0; j<3; j++)
//			{
//			index.clear();
//			if( pedge_state[i][j] == 0)
//				{
//				sVertex = m_Facet[i].m_Vertex[j];
//				cVertex = m_Facet[i].m_Vertex[(j+1)%3];
//				edge.InitObject();
//				edge.AddPoint(sVertex);
//				edge.AddPoint(cVertex);
//				pedge_state[i][j] = 1; //remove edge
//				index.push_back(i);
//
//				bNewEdge = false;
//				bEndLoop = false;
//				for(k=i; k<m_Num; k++) 
//					{
//					for(l=0; l<3; l++)
//						{
//						if( pedge_state[k][l]==0&&cVertex==m_Facet[k].m_Vertex[l])
//							{			
//							cVertex = m_Facet[k].m_Vertex[(l+1)%3];
//							edge.AddPoint(cVertex);
//							pedge_state[k][l] = 1;//remove edge
//							index.push_back(k);
//							bNewEdge = true;
//							if(sVertex==cVertex) 
//								{
//								bEndLoop = true;
//								break;	
//								}
//							}
//						}
//					if(bNewEdge) {k=i;bNewEdge=false;}
//				if(bEndLoop) break;
//					}
//				DivideMeshEdge(edge,view_dir,edgelist,type_edge);
//				}
//			}
//		}
//	/*	
//	#ifdef _DEBUG
//	bool bx[5] = {false};
//	int xx[5] = {0};
//	int x = edgelist.size();
//	for(i=0;i<5;i++)
//	{
//	xx[i] = edgelist[i].GetPntNum();
//	bx[i] = type_edge[i];
//	}
//	#endif*/
//
//	if(pedge_state != NULL)
//		free_integer_matrix(pedge_state, 0, m_Num-1, 0);
//	return true;
//	}
//
/////////////////////////////////////////////////////////////
//new
/////////////////////////////////////////////////////////////
//void XMesh::DivideMeshEdge(const XCloud& edge,
//			   PNT& view_dir,
//			   std::vector<XCloud>& edgelist,
//			   std::vector<bool>& type_edge)
//	{
//	int i,j,k,total=0;
//	bool founded;
//	vector<int> index1,index2;
//	XCloud edge1,edge2;
//	PNT cPnt0,cPnt1,tempPnt;
//	XFacet checkFacet;	
//
//	total = edge.GetPntNum();
//	founded = false;
//	for(i=0;i<total-1;i++){
//		edge.GetPoint(i,cPnt0);
//		for(j=i+1;j<total-1;j++){
//			edge.GetPoint(j,cPnt1);
//			if(cPnt0.IsSameVtr(cPnt1,3))
//			 {
//			 founded = true;
//			 break;
//			 }
//		 }
//	if(founded) break; 
//		}
//
//if(founded)
//	{
//
//	for(k=i;k<=j;k++)
//		{
//		edge.GetPoint(k,tempPnt);
//		edge1.AddPoint(tempPnt);							
//		}
//	for(k=0;k<i;k++)
//		{
//		edge.GetPoint(k,tempPnt);
//		edge2.AddPoint(tempPnt);							
//		}
//	for(k=j;k<total;k++)
//		{
//		edge.GetPoint(k,tempPnt);
//		edge2.AddPoint(tempPnt);							
//		}
//	DivideMeshEdge(edge1,view_dir,edgelist,type_edge);
//	DivideMeshEdge(edge2,view_dir,edgelist,type_edge);		 
//	}
//else
//	{
//	int count = edge.GetPntNum();
//	if(count<3) return;
//
//	edgelist.push_back(edge); 
//
//	PNT a,b,c,d1,d2,e,start;
//	double dot=0,theta=0;
//
//#ifndef PI
//#define PI
//	const double PI = 3.1415926;
//#endif
//
//	edge.GetPoint(0,a);
//	edge.GetPoint(1,b);
//	d1 = b - a;
//	d1.Normalize();
//	start = d1;
//
//	for(i=2;i<count;i++)
//		{
//		edge.GetPoint(i,c);
//		d2 = c - b;
//		d2.Normalize();
//		e = d1.CrossProduct(d2);
//		dot = d1.DotProduct(d2);
//
//		if(dot>1) dot = 1;
//		else if(dot<-1) dot = -1; 
//
//		if(e.DotProduct(view_dir)>0)
//			theta += acos(dot);				 				 	
//		else 
//			theta -= acos(dot);
//		b = c;
//		d1 = d2;
//		}
//
//	e = d1.CrossProduct(start);
//	dot = d1.DotProduct(start);
//
//	if(dot>1) dot = 1;
//	else if(dot<-1) dot = -1; 
//
//	if(e.DotProduct(view_dir)>0)
//		theta += acos(dot);				 				 	
//	else 
//		theta -= acos(dot);
//
//	if(theta>0)  
//		type_edge.push_back(true);
//	else
//		type_edge.push_back(false);
//	}
//
//	}
//
//
//
void XMesh::ChangeUnits(bool bmm2inch)
	{
	if(bmm2inch)
		{
		int i,j;
		for( i=0; i< m_Num; i++)
			{
			for(j=0;j<3;j++)
				XMesh::m_Facet[i].m_Vertex[j] /= 25.4f;
			}
		}
	else
		{
		int i,j;
		for( i=0; i< m_Num; i++)
			{
			for(j=0;j<3;j++)
				XMesh::m_Facet[i].m_Vertex[j] *= 25.4f;
			}
		}	
	m_bObjChanged = true;
	}

void XMesh::MeshMerge(const XMesh& mesh)
	{
	m_Facet.insert(m_Facet.end(), mesh.m_Facet.begin(), mesh.m_Facet.end());
	this->m_Num += mesh.GetFacetNum();
	}

//void XMesh::TransForm(const Ti_Matrix<double>& RegM)
//	{
//	int num = GetFacetNum();
//	XFacet	facet;
//	PNT  t((float)RegM.m[0][3], (float)RegM.m[1][3], (float)RegM.m[2][3]);
//	int i,j;
//	for( i=0; i<num; i++)
//		{
//		for(j =0; j<3; j++)
//			{
//			m_Facet[i].m_Vertex[j].TransForm(RegM);
//			}
//		t = m_Facet[i].m_Norm;
//		m_Facet[i].m_Norm[0] = RegM.m[0][0]*t[0] +RegM.m[0][1]*t[1] +RegM.m[0][2]*t[2];
//		m_Facet[i].m_Norm[1] = RegM.m[1][0]*t[0] +RegM.m[1][1]*t[1] +RegM.m[1][2]*t[2];
//		m_Facet[i].m_Norm[2] = RegM.m[2][0]*t[0] +RegM.m[2][1]*t[1] +RegM.m[2][2]*t[2];
//		m_Facet[i].m_Norm.Normalize();
//		}
//	this->m_bObjChanged = true;
//	}
//
XMesh XMesh::VME_Planning(	const PNT& axis_dir, 
			  const PNT& axis_pnt,
			  const PNT& laser_dir, 
			  double max_incidence_angle, 
			  double& Q_angle)
	{
	double  angle=0;
	const double ANGLE_STEP = 10.*PI/180.;
	int facetnum, max_fnum= 0;
	XMesh  mesh,
		ex_mesh,
		m;
	PNT  dir, dir0, dir1;
	/* mesh = *this;
	mesh.GetBestViewDir(dir);
	dir0 = dir.CrossProduct(axis_dir);
	dir1 = laser_dir.CrossProduct(axis_dir);
	dir0.Normalize();
	dir1.Normalize();
	reckon_angle = acos(dir0*dir1);
	if(dir[0] > 0)
	reckon_angle = 2*PI - reckon_angle;*/
	for(angle = 0; angle <2*PI; angle +=ANGLE_STEP)
		{
		mesh = *this;
		mesh.Rotate(axis_pnt, axis_dir, angle);
		if(mesh.GetPotentialVM(laser_dir, max_incidence_angle, ex_mesh) < max_fnum)
			continue;
		ex_mesh = ex_mesh.HidenFacet(laser_dir); // mesh.GetVisibalMesh(laser_dir, max_incidence_angle, ex_mesh);
		facetnum = ex_mesh.GetFacetNum();
		if( facetnum > max_fnum)
			{
			max_fnum = facetnum;
			Q_angle = angle;
			m = ex_mesh;
			}
		else
			{
			if(facetnum < max_fnum/3)
				{
				angle += 3*ANGLE_STEP;
				}
			}
		}
	return m;
	}

void XMesh::GetBestViewDir(PNT & dir)
	{
	int Num  = GetFacetNum();
	XCloud mesh_norm;
	mesh_norm.SetCapacity(Num);
	for(int  i = 0; i< Num; i++)
		{
		mesh_norm.AddPoint(m_Facet[i].m_Norm);
		}
	PNT pnt;
	mesh_norm.Fit2Plane(pnt, dir);
	if(pnt*dir < 0)
		dir = -dir;
	}

int XMesh::GetPotentialVM(const PNT& view_dir, double max_incidenceangle, XMesh& ex_mesh)
	{
	double thresh_cosangle = cos(max_incidenceangle/180.0*PI);
	XFacet facet;
	XMesh mesh(*this);
	int num = mesh.GetFacetNum();
	ex_mesh.ClearMesh();
	for(int i =0; i< num; i++)
		{
		facet = this->m_Facet[i];
		if(facet.m_Norm[2] > thresh_cosangle)
			ex_mesh.AddFacet(facet);
		}
	return ex_mesh.GetFacetNum();
	}

XCloud XMesh::GetFacetCenterCld(void)
	{
	XCloud cld;
	PNT pnt;
	int num = GetFacetNum();
	cld.SetCapacity(num);
	for(int i = 0 ; i < num ; i++)
		{
		m_Facet[i].GetCenterPnt(pnt);
		cld.AddPoint(pnt);
		}
	return cld;
	}
bool __cdecl SortCompareMX(const XFacet&  elem1, const XFacet&  elem2)
	{
	PNT  pnt1, pnt2;
	elem1.GetCenterPnt(pnt1);
	elem2.GetCenterPnt(pnt2);
	if (pnt1[0]> pnt2[0])
		return  false;
	else if (pnt1[0]< pnt2[0])
		return true;
	else
		{
		if (pnt1[1] > pnt2[1])
			return false;
		else if (pnt1[1] < pnt2[1])
			return true;
		else
			{
			if (pnt1[2] > pnt2[2])
				return  false;
			else if (pnt1[2] < pnt2[2])
				return true;
			else 
				return false;
			}
		}
	}
bool __cdecl SortCompareMY(const XFacet&  elem1, const XFacet&  elem2)
	{
	PNT  pnt1, pnt2;
	elem1.GetCenterPnt(pnt1);
	elem2.GetCenterPnt(pnt2);
	if (pnt1[1]> pnt2[1])
		return  false;
	else if (pnt1[1]< pnt2[1])
		return true;
	else
		{
		if (pnt1[2] > pnt2[2])
			return false;
		else if (pnt1[2] < pnt2[2])
			return true;
		else
			{
			if (pnt1[0] > pnt2[0])
				return  false;
			else if (pnt1[0] < pnt2[0])
				return true;
			else 
				return false;
			}
		}
	}
bool __cdecl SortCompareMZ(const XFacet&  elem1, const XFacet&  elem2)
	{
	PNT  pnt1, pnt2;
	elem1.GetCenterPnt(pnt1);
	elem2.GetCenterPnt(pnt2);
	if (pnt1[2]> pnt2[2])
		return  false;
	else if (pnt1[2]< pnt2[2])
		return true;
	else
		{
		if (pnt1[0] > pnt2[0])
			return false;
		else if (pnt1[0] < pnt2[0])
			return true;
		else
			{
			if (pnt1[1] > pnt2[1])
				return  false;
			else if (pnt1[1] < pnt2[1])
				return true;
			else 
				return false;
			}
		}
	}
int __cdecl MarquardtforMeshReg(
				int  m,        //<I>  the number of equations ,in other words, the measured points
				int   n,        //<I>  the number of unknown variables; n should satisfy n<=m
				const XCloud&  g_pnt,//<I>  the input points 
				const XMesh&  g_mesh,
				double* const par,  //<IO> input: the approximation to a least squares solution of 
				// the system,output: the calculated least squares solution
				double*& g,    //<O>  the residual vector at the calculated solutions
				double**v,    //<O>  the inverse of the matrix J'J where J denote the transpose of 
				// the matrix of partial derivatives g'(i)/par'(j)
				int (*funct)(int m,  int n, double* const par, double* g, const XCloud&  g_pnt,  const XMesh&  g_mesh), 
				// m,n have the same meaning as above
				// array par[] contains the current values of the unknowns and should not be altered 
				// exit: upon completion of a call of funct, the array g[1:m] should contain the 
				// residual vector obtained with the current values of the unknowns; 
				// e.g. in curve fitting problems, 
				// g[i]= theoretical value f(x,par) - observed value y[i]
				// after a successful call of funct, the Calculator should deliver the value nonzero; 
				// however, if funct delivers the value zero then it is assumed that the current 
				// estimates of the unknowns lie outside a feasible region and the process is 
				// terminated ; hence , proper programming of funct makes it possible to 
				// avoid calculation of a residual vector with values					
				// of the unkonwn variables which make no sense or which even may 
				// cause overflow in the computaion;
				int (*jacobian)(int m, int n , double* const par, double*& g, double **jac, const XCloud&  g_pnt,  const XMesh&  g_mesh),
				// m,n par see funct
				// g contains the residual vector obtained  with the current values of the unknonwns and
				// should not be altered ;
				// exit: upon completion of a call of jacobian , the array jac should contain the partial
				// derivatives g'(i)/par'(j), obtained with the current values of the unknown variables 
				// given in par[1:n]. it is a prerequisite for the proper operation of the procedure 
				// marquardt that the precision of the elements of the matrix jac is at least the precision
				// defined by in[3] and in[4];
				double in[],   // <I>
				// in[0]: the machine precision
				// in[1],in[2]: these are not used by marquardt
				// in[3]: the relative tolerance for the difference between 
				// the Euclidean norm of the ultimate and penultimate residual vector 
				// in[4]: the absolute tolerence for the difference between 
				// the Euclidean norm of the ultimate and penultimate residual vector
				// the process is terminated if the improvement of the sum of squares is 
				// less than in[3]*(sum of squares)+in[4]*in[4]; 
				// these tolerence should be chosen greater than the corresponding 
				// errors of the caculated residual vector
				// in[5]: the maximum number of calls of funct allowed
				// in[6]: a starting value used for the relation between 
				// the gradient and the Gauss-Newton direction (value ¦Îabove);
				// if the problem is well conditioned then in[6] should be greater,
				// but the value of in[6] should satisfy:in[0]<in[6]<=1/in[0]

				double out[]   //<O>
			// out[1]: this value gives information about the termination of the process
			// out[1] = 0: normal termination
			// out[1] = 1: the process has been broken off because the number of calls of funct exceeded the number given in in[5]
			// out[1] = 2: the process hase been broken off because a call of funct delivered the value zero
			// out[1] = 3: funct bacame zero when called with the initial estimates of par[1:n];
			// the interation process was not broken off because the precision asked for cannot be attained ; this precision is possibly chosen too high,relative to the precision in wich the residual vector is calculated
			// out[1] = 4: the process has been broken off because the precision asked for cann't be attained ; this precision
			//        is possibly chosen too high,relative to the precision in which the residual vector is calculated 
			// out[2]: the Euclidean norm of the residual vector calculated with values of the unknowns delived
			// out[3]: the Euclidean norm of the residual vector calculated with the initial values of the unknowns variables
			// out[4]: the number of calls of funct necessary to attain the calculated result
			// out[5]: the total number of iterations performed:note that in each interation one evaluation of the Jacobian matrix had to be made
			// out[6]: the improvement vector in the last interation step
			// out[7]: the condition number of J'J ,i.e. the ratio of its largest to smallest eigenvalues
)
	{
	int		maxfe,fe,it,i,j,err,emergency;
	double	vv,ww,w,mu,res,fpar,fparpres,lambda,lambdamin,p,pw,reltolres,
		abstolres,em[8], *val, *b, *bb, *parpres,**jac,temp;
	Ti_Optimization    algorithm;

	val		=  new double[n+1];
	b		=  new double[n+1];
	bb		=  new double[n+1]; //allocate_real_vector(1,n);
	parpres  =  new double[10]; //allocate_real_vector(1,n);
	jac		= new double*[m+1]; // allocate_real_matrix(1,m,1,n);
	assert(jac != NULL);
	for(i = 0 ; i< m+1; i++)
		{
		jac[i] = new double[n+1];
		assert(jac[i] != NULL);
		}
	assert( (val != NULL) &&
		(b   != NULL) &&
		(bb  != NULL) &&
		(parpres!= NULL)&&
		(jac != NULL)
		);
	for(i = 3; i< 9; i++)
		{
		parpres[i] = par[i];
		}
	vv      = 10.0;
	w		= 0.5;
	mu		= 0.01;
	ww		= (in[6] < 1.0e-7) ? 1.0e-8 : 1.0e-1*in[6];
	em[0] = em[2] = em[6] = in[0];
	em[4] = 10*n;
	//em[1] = 
	reltolres =in[3];
	abstolres=in[4]*in[4];
	maxfe=(int)in[5];
	err=0;
	fe=it=1;
	p=fpar=res=0.0;
	pw = -log(ww*in[0])/2.30;

	if (!(*funct)(m,n,par,g,g_pnt, g_mesh))
		{
		err=3;
		out[4]=fe;
		out[5]=it-1;
		out[1]=err;
		if(val != NULL)
			{
			//free_real_vector(val,1);
			delete []val;
			val = NULL;
			}
		if (b != NULL)
			{
			//free_real_vector(b,1);
			delete []b;
			b = NULL;
			}
		if(bb!=NULL)
			{
			//free_real_vector(bb,1);
			delete []bb;
			bb  = NULL;
			}
		if(parpres != NULL)
			{
			//free_real_vector(parpres,1);
			delete []parpres;
			parpres = NULL;
			}
		if (jac != NULL)
			{
			for(i = 0 ; i< m+1; i++)
				{
				delete [](jac[i]); // = new double[n+1];				
				}
			delete []jac;
			//free_real_matrix(jac,1,m,1);
			jac = NULL;
			}
		return -1;
		}
	fpar=algorithm.vecvec(1,m,0,g,g);// norm of residual vector
	out[3]=sqrt(fpar);
	emergency=0;
	it=1;
	do {
		if((*jacobian)(m,n,par,g,jac,g_pnt, g_mesh) == -1)
			{
			err = 4;// jacobian calculation error
			break;
			}
		i = algorithm.qrisngvaldec(jac,m,n,val,v,em);
		if (it == 1)
			lambda = in[6]*algorithm.vecvec(1,n,0,val,val);
		else
			if (p == 0.0)
				lambda *= w;
		for (i=1; i<=n; i++) 
			b[i] = val[i]*algorithm.tamvec(1,m,i,jac,g);
		while (1)
			{
			for (i=1; i<=n; i++) 
				bb[i]=b[i]/(val[i]*val[i]+lambda);
			for (i=1; i<=n; i++)
				parpres[i]=par[i]-algorithm.matvec(1,n,i,v,bb);

			fe++;

			if (fe >= maxfe)
				err=1;
			else
				if (!(*funct)(m,n,parpres,g,g_pnt, g_mesh))
					err=2;
			if (err != 0) 
				{
				emergency = 1;
				break;
				}
			fparpres=algorithm.vecvec(1,m,0,g,g);
			res=fpar-fparpres;
			if (res < mu*algorithm.vecvec(1,n,0,b,bb)) 
				{
				p += 1.0;
				lambda *= vv;
				if (p == 1.0) 
					{
					lambdamin=ww*algorithm.vecvec(1,n,0,val,val);
					if (lambda < lambdamin)
						lambda=lambdamin;
					}
				if (p >= pw)
					{
					err=4;
					emergency=1;
					break;
					}
				} // end if
			else 
				{
				algorithm.dupvec(1,n,0,par,parpres);
				fpar=fparpres;
				break;
				} // end else
			} // end while
		if (emergency) 
			break;
		it++;
		} 
while ( 
       (fpar > abstolres) &&   
       (res > reltolres*fpar+abstolres)
       );

for (i=1; i<=n; i++) 
algorithm.mulcol(1,n,i,i,jac,v,1.0/(val[i]+in[0]));
for (i=1; i<=n; i++)
	{
	for (j=1; j<=i; j++) 
		v[i][j]=v[j][i]=algorithm.mattam(1,n,i,j,jac,jac);
	lambda=lambdamin=val[1];
	}
for (i=2; i<=n; i++)
	{
	if (val[i] > lambda)
		lambda=val[i];
	else
		{
		if (val[i] < lambdamin) 
			lambdamin=val[i];
		}
	}

temp=lambda/(lambdamin+in[0]);
out[7]=temp*temp;
out[2]=sqrt(fpar);
out[6]=sqrt(res+fpar)-out[2];
out[4]=fe;
out[5]=it-1;
out[1]=err;
if(val != NULL)
	{
	delete []val;
	val = NULL;
	}
if (b != NULL)
	{
	delete []b;
	b = NULL;
	}
if(bb!=NULL)
	{
	delete []bb;
	bb  = NULL;
	}
if(parpres != NULL)
	{
	delete []parpres;
	parpres = NULL;
	}
if (jac != NULL)
	{
	for(i = 0 ; i< m+1; i++)
		{
		delete [](jac[i]); // = new double[n+1];				
		}
	delete []jac;
	jac = NULL;
	}
if(out[1] == 4)
return -1; 
else 
return 1;
	}

void XMesh::Sort_ByAxis(int axis_index)
	{
	int num = GetFacetNum();
	if (num <= 1)
		return ;
	switch(axis_index)
		{
		//--------sort pnts according to their object coordinates
		case 1:
			std::sort(m_Facet.begin(), m_Facet.end(), SortCompareMX);
			break;
		case 2:
			std::sort(m_Facet.begin(), m_Facet.end(), SortCompareMY);
			break;
		case 3:
			std::sort(m_Facet.begin(), m_Facet.end(), SortCompareMZ);
			break;
		}
	}

void XMesh::SortByDir(PNT& dir)
	{
	dir.Normalize();
	int  axis_index = 1;
	double angle ;
	dir[1] > dir[0]? \
		(dir[2] > dir[1]? (axis_index = 3):(axis_index = 1))\
		:(dir[0] > dir[2] ? (axis_index = 1):(axis_index = 2));
	PNT rot_axis, sort_axis, origin;
	switch(axis_index)
		{
		case 1:
			sort_axis = PNT(1, 0, 0);
			angle = acos(dir[0]);
			break;
		case 2:
			sort_axis = PNT(0, 1, 0);
			angle = acos(dir[1]);
			break;
		case 3:
			sort_axis = PNT(0, 0, 1);
			angle = acos(dir[2]);
			break;
		}
	rot_axis = dir^sort_axis;
	rot_axis.Normalize();
	this->Rotate(origin, rot_axis, angle);
	this->Sort_ByAxis(axis_index);
	this->Rotate(origin, rot_axis,- angle);
	}
bool XMesh::Slicing( PNT& section_pnt,  PNT& section_dir, double tolerance , XCloud& section_cld)
	{
	int i, j;
	int num = GetFacetNum();
	int begin = 0 ,
		end = num, 
		index,
		intnum;
	float intersecpnt[9];
	PNT pnt;
	index = (begin + end)/2;
	section_cld.InitObject();
	section_dir.Normalize();

	if( acos(section_dir[0])  < 1e-6 ||fabs(PI - acos(section_dir[0]))  < 1e-6)
		{
		Sort_ByAxis(1);
		while(1)
			{
			this->m_Facet[index].GetCenterPnt(pnt);
			if(pnt[0] >= section_pnt[0] - tolerance)
				{
				end = index;
				}
			else //if(mVector3[index][0] <= section_pnt[0] - tolerance)
				{
				begin = index;
				}
			if( end - begin  < 6)
				break;
			else
				index = (begin + end)/2;
			}
		for( i = begin ; i < num ; i++)
			{
			m_Facet[i].GetCenterPnt(pnt);
			if( pnt[0] > section_pnt[0] + tolerance)
				break;
			if( pnt[0] > section_pnt[0] - tolerance)
				{
				if(m_Facet[i].Int_Plane(section_dir,section_pnt,intnum, intersecpnt ))
					{
					for(j =0; j < intnum; j++)
						{
						pnt[0] = intersecpnt[j*3];
						pnt[1] = intersecpnt[j*3+1];
						pnt[2] = intersecpnt[j*3+2];
						section_cld.AddPoint(pnt);
						}
					}
				}
			}
		return true;
		}		
	else
		{
		PNT  rot_axis,  x_dir(1, 0, 0), origin_pnt(0,0,0);
		double angle = acos(section_dir[0]);
		rot_axis = section_dir^x_dir;
		rot_axis.Normalize();
		Translate(-section_pnt);
		Rotate(origin_pnt, rot_axis, angle);
		Sort_ByAxis(1);
		while(1)
			{
			this->m_Facet[index].GetCenterPnt(pnt);
			if(pnt[0] > section_pnt[0] - tolerance)
				{
				end = index;
				}
			else
				{
				begin = index;
				}
			if( end - begin  < 6)
				break;
			else
				index = (begin + end)/2;
			}
		for( i = begin ; i < num ; i++)
			{
			this->m_Facet[i].GetCenterPnt(pnt);
			if(pnt[0] >tolerance)
				break;
			if( pnt[0] > - tolerance)
				{
				if(m_Facet[i].Int_Plane(x_dir, origin_pnt, intnum, intersecpnt ))
					{
					for(j =0; j < intnum; j++)
						{
						pnt[0] = intersecpnt[j*3];
						pnt[1] = intersecpnt[j*3+1];
						pnt[2] = intersecpnt[j*3+2];
						section_cld.AddPoint(pnt);
						}
					}			
				}
			}
		section_cld.Rotate(origin_pnt, rot_axis, -angle);
		section_cld.Translate(section_pnt);
		Rotate(origin_pnt, rot_axis, -angle);
		Translate(section_pnt);
		return true;
		}
	return false;
	}

XCloud XMesh::GetGuassMap(void)
	{
	XCloud guasscld;
	CString str = this->m_Name;
	str = str + TEXT("guassmap");
	int num = GetFacetNum();
	guasscld.SetCapacity(num);
	for(int i =0; i < num; i++)
		{
		guasscld.AddPoint(this->m_Facet[i].m_Norm);
		}
	guasscld.SetName(str);
	return guasscld;
	}
