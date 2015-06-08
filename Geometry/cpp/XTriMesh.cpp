/********************************************************************
	created:	2004/08/08
	created:	8:8:2004   8:40
	filename: 	XTriMesh.cpp
	file base:	XTriMesh
	file ext:	cpp
	author:		Yang,Pinghai
	
	purpose:	Triangle mesh model construction 
				& multi-view data integration
*********************************************************************/
#include "stdafx.h"
#include "XTriMesh.h"
//#include "gl\gl.h"
//#include "gl\glu.h"
#include "math.h"
//#ifdef _DEBUG
//#include "Diag_Progress.h"
//#endif


static PNT v01;
static PNT v12;
static PNT v20;
static XTriMesh::TriEdge * afxEdge01;
static XTriMesh::TriEdge * afxEdge12;
static XTriMesh::TriEdge * afxEdge20;
static const double error		= 1e-8;
static const double totalCount	= 128;

//class TriFacet
XTriMesh::TriFacet::TriFacet(void)
:m_iFacetType(0)
{
	for (int i =0; i< 3; i++)
	{
	m_edge[i] = NULL;
	m_pNeighbFacet[i] = NULL;
	}
	m_iIndex = NULL;
}

XTriMesh::TriFacet& XTriMesh::TriFacet::operator=(const TriFacet& facet)
{
		m_iFacetType = facet.m_iFacetType;	    // -1: will be delete; 0: normal;  8: for overlap; 9:
		for(int i = 0; i< 3; i++)
		{
			m_VertexIndex[i]	= facet.m_VertexIndex[i];// vertex in m_PointArray index
			m_edge[i]			=  facet.m_edge[i];
			m_pNeighbFacet[i]	=  facet.m_pNeighbFacet[i];
		}
		m_iIndex = facet.m_iIndex;			// facet index
		m_Normal = facet.m_Normal;
		return *this;
}

bool  XTriMesh::TriFacet::operator==(const TriFacet & facet)
	{
	if( (m_VertexIndex[0] == facet.m_VertexIndex[0] )&&
		(m_VertexIndex[1] == facet.m_VertexIndex[1] )&&
		(m_VertexIndex[2] == facet.m_VertexIndex[2] ))
		{
		return true;
		}
	else
		return false;

	}

XTriMesh::TriFacet::~TriFacet(void)
{	
}

//class TriEdge
XTriMesh::TriEdge::TriEdge(void)
:m_iEdgeType(0)
{
m_iCorrespIndex = NULL;
m_iNeighbFacet = NULL;
m_iIndex = NULL;

}
XTriMesh::TriEdge& XTriMesh::TriEdge::operator=(const TriEdge& edge)
	{
	m_iCorrespIndex = edge.m_iCorrespIndex;
	m_iNeighbFacet = edge.m_iNeighbFacet;
	m_iIndex = edge.m_iIndex;
	m_iEdgeType = edge.m_iEdgeType;
	m_VertexIndex[0] = edge.m_VertexIndex[0];
	m_VertexIndex[1] = edge.m_VertexIndex[1];
	return *this;
	}

bool  XTriMesh::TriEdge::operator==(const TriEdge & edge)
	{
	if( (this->m_VertexIndex[0] == edge.m_VertexIndex[0])&&
		(this->m_VertexIndex[1] == edge.m_VertexIndex[1]) )
		{
		return true;
		}
	else
		return false;
	}

XTriMesh::TriEdge::~TriEdge(void)
{
}

//class TriEdge
XTriMesh::TriPoint::TriPoint(void)
:m_iPntStat(1)
,m_iPntType(0)
,m_pNeighbFacetList(NULL)
,m_pNeighbPntList(NULL)
{
}
XTriMesh::TriPoint::~TriPoint(void)
{
	Release();
}

XTriMesh::TriPoint& XTriMesh::TriPoint::operator=(const TriPoint & point) // for depth copy
{
	m_iPntStat = point.m_iPntStat;
	m_iPntType = point.m_iPntType;
	m_iIndex = point.m_iIndex;
	m_Coord = point.m_Coord;
	m_pNeighbFacetList = point.m_pNeighbFacetList;
	m_pNeighbPntList	= point.m_pNeighbPntList;
	//int i, num;
	//POSITION pos = NULL;
	//if(point.m_pNeighbFacetList != NULL)
	//	{
	//	if(m_pNeighbFacetList != NULL)
	//		{
	//		m_pNeighbFacetList->RemoveAll();
	//		delete m_pNeighbFacetList;
	//		m_pNeighbFacetList= NULL;
	//		}
	//	m_pNeighbFacetList = new CPosList;
	//	 pos = point.m_pNeighbFacetList->GetHeadPosition();
	//	num = point.m_pNeighbFacetList->GetCount();
	//	for(i =0 ; pos &&i<num; i++)
	//		m_pNeighbFacetList->AddTail(point.m_pNeighbFacetList->GetNext(pos));
	//	}
	//else
	//	m_pNeighbFacetList = NULL;

	//if(point.m_pNeighbPntList != NULL)
	//	{
	//	if(m_pNeighbPntList != NULL)
	//		{
	//		m_pNeighbPntList->RemoveAll();
	//		delete m_pNeighbPntList;
	//		m_pNeighbPntList = NULL;
	//		}
	//	m_pNeighbPntList = new CIntList;
	//	pos = point.m_pNeighbPntList->GetHeadPosition();
	//	num = point.m_pNeighbPntList->GetCount();
	//	//m_pNeighbPntList->AddTail( point.m_pNeighbPntList->GetHead(s));
	//	for(i=0; pos&&i < num; i++)
	//		m_pNeighbPntList->AddTail( point.m_pNeighbPntList->GetNext(pos));
	//	}
	//else
	//	m_pNeighbPntList = NULL;
	return *this;
}

void XTriMesh::TriPoint::Release()
{
	if(m_pNeighbFacetList!=NULL)
	{
		//m_pNeighbFacetList->RemoveAll();
		//delete m_pNeighbFacetList;
		m_pNeighbFacetList = NULL;
	}
	if(m_pNeighbPntList!=NULL)
	{
		//m_pNeighbPntList->RemoveAll ();
		//delete m_pNeighbPntList;
		m_pNeighbPntList = NULL;
	}	
}

//class XTriMesh
XTriMesh::XTriMesh(void)
{
	InitObject();
}

XTriMesh::XTriMesh(CString meshName)
{
	InitObject();
	m_Name = meshName;
}

XTriMesh::XTriMesh(const XTriMesh & triMesh)
{
	InitObject();
	*this = triMesh;
}

void XTriMesh::InitObject()
{
	m_Color = 1;
	m_DispType = 1;
	m_iPointNum = 0;
	m_iSeedIndex = -1;
	m_iSeedCount = -1;
	m_iPnt2SeedCount = -1;
	m_pSeed = NULL;
	m_pPnt2Seed = NULL;
	m_bSetVertexesDone = false;
	m_bShow = true;
	m_pDrawX = NULL;
	m_pDrawY = NULL;
	m_pDrawZ = NULL;
	m_pDrawIndex = NULL;
	m_Name = _T("");
	m_ViewDirection = PNT(0,0,1);
	m_ObjectType = OBJECT_TRIMESH;
}

XTriMesh & XTriMesh::operator=(const XTriMesh & triMesh)
{
	m_Color = triMesh.m_Color;
	this->m_bShow = triMesh.m_bShow;
	m_iPointNum = triMesh.m_iPointNum;
	m_DispType = triMesh.m_DispType;	
	m_Name = triMesh.m_Name;
	m_Unit = triMesh.m_Unit;
	m_BndBox = triMesh.m_BndBox;
	m_ViewDirection = triMesh.m_ViewDirection;
	if(triMesh.m_pDrawIndex != NULL)
		{
		int num = triMesh.m_FacetList.GetCount() *3;
		if(m_pDrawIndex != NULL)
			delete []m_pDrawIndex;
		m_pDrawIndex = new int[num];
		for(int i = 0; i < num; i++)
		m_pDrawIndex[i] = triMesh.m_pDrawIndex[i];
		}
	if(triMesh.m_pDrawX != NULL)
		{
		int num = triMesh.m_FacetList.GetCount() + m_iPointNum;
		if(m_pDrawX != NULL)
			delete [] m_pDrawX;
		m_pDrawX = new float[num];
		if(m_pDrawY != NULL)
			delete []m_pDrawY;
		m_pDrawY = new float[num];
		if(m_pDrawZ != NULL)
			delete []m_pDrawZ;
		m_pDrawZ = new float[num];
		for(int i = 0; i < num; i++)
			{
			m_pDrawX[i] = triMesh.m_pDrawX[i];
			m_pDrawY[i] = triMesh.m_pDrawY[i];
			m_pDrawZ[i] = triMesh.m_pDrawZ[i];
			}
		}

	if(!m_EdgeList.IsEmpty())
		m_EdgeList.RemoveAll();
	if(!m_FacetList.IsEmpty())
		m_FacetList.RemoveAll();
	//if(triMesh.m_PointArray.GetSize()<1)
	//{
	//	return ;
	//}
	//m_PointArray = triMesh.m_PointArray;
	m_PointArray.SetSize(triMesh.m_PointArray.GetSize());
	m_PointArray.Copy(triMesh.m_PointArray);
	/*m_EdgeList.AddTail() = triMesh.m_EdgeList;
	m_FacetList = triMesh.m_FacetList;*/

	m_EdgeList.AddTail(&((XTriMesh &)triMesh).m_EdgeList);
	m_FacetList.AddTail(&((XTriMesh &)triMesh).m_FacetList);

	/*CList<TriEdge,TriEdge&> edgeList;
	edgeList.AddTail(&m_EdgeList);
	CList<TriFacet,TriFacet&> facetList;
	facetList.AddTail(&m_FacetList);*/

	int i,j,k;
	TriEdge * pEdge;
	TriFacet * pFacet;

	int edgeCount = m_EdgeList.GetCount();
	int facetCount = m_FacetList.GetCount();

	CPosArray edgeArray;
	CPosArray facetArray;
	edgeArray.SetSize(edgeCount);
	facetArray.SetSize(facetCount);

	POSITION pos,posNew,posOld;
	pos = triMesh.m_EdgeList.GetHeadPosition();
	posNew = m_EdgeList.GetHeadPosition();
	for(i=0;	pos;	i++)
	{
		((XTriMesh &)triMesh).m_EdgeList.GetNext(pos).m_iEdgeType = i;
		posOld = posNew;
		pEdge = &m_EdgeList.GetNext(posNew);
		pEdge->m_iIndex = edgeArray[i] = posOld;
	}

	pos = triMesh.m_FacetList.GetHeadPosition();
	posNew = m_FacetList.GetHeadPosition();
	for(i=0;	pos;	i++)
	{
		((XTriMesh &)triMesh).m_FacetList.GetNext(pos).m_iFacetType = i;
		posOld = posNew;
		pFacet = &m_FacetList.GetNext(posNew);
		pFacet->m_iIndex = facetArray[i] = posOld;
	}
	ASSERT(i== m_FacetList.GetCount());
	pos = m_EdgeList.GetHeadPosition();
	for(i=0;	pos;	i++)
	{
		pEdge = &m_EdgeList.GetNext(pos);
		if(pEdge->m_iCorrespIndex!=NULL)
		{
			j = triMesh.m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iEdgeType;
			pEdge->m_iCorrespIndex = edgeArray[j];
		}
		if(pEdge->m_iNeighbFacet!=NULL)
		{
			j = triMesh.m_FacetList.GetAt(pEdge->m_iNeighbFacet).m_iFacetType;
			pEdge->m_iNeighbFacet = facetArray[j];
		}
	}
	ASSERT(i== m_EdgeList.GetCount());

	pos = m_FacetList.GetHeadPosition();
	for(i=0;pos;i++)
	{
		pFacet = &m_FacetList.GetNext(pos);
		for(k=0;k<3;k++)
		{
			if(pFacet->m_edge[k]!=NULL)
			{
				j = triMesh.m_EdgeList.GetAt(pFacet->m_edge[k]).m_iEdgeType;
				pFacet->m_edge[k] = edgeArray[j];
			}
			if(pFacet->m_pNeighbFacet[k]!=NULL)
			{
				j = triMesh.m_FacetList.GetAt(pFacet->m_pNeighbFacet[k]).m_iFacetType;
				pFacet->m_pNeighbFacet[k] = facetArray[j];
			}
		}
	}	
	ASSERT(i== m_FacetList.GetCount());

	for(pos = triMesh.m_EdgeList.GetHeadPosition();pos;)
	{
		((XTriMesh &)triMesh).m_EdgeList.GetNext(pos).m_iEdgeType = 0;
	}

	for(pos = triMesh.m_FacetList.GetHeadPosition();pos;)
	{
		((XTriMesh &)triMesh).m_FacetList.GetNext(pos).m_iFacetType = 0;
	}	
	SetVertexesForDraw();
	return *this;

	if(pEdge)
		delete pEdge;
	if(pFacet)
		delete pFacet;
}


XTriMesh::~XTriMesh(void)
{
	if(m_pDrawX!=NULL)
	{
		delete []m_pDrawX;
		m_pDrawX = NULL;
	}
	if(m_pDrawY!=NULL)
	{
		delete []m_pDrawY;
		m_pDrawY = NULL;
	}	
	if(m_pDrawZ!=NULL)
	{
		delete []m_pDrawZ;
		m_pDrawZ = NULL;
	}	
	if(m_pDrawIndex!=NULL)
	{
		delete []m_pDrawIndex;
		m_pDrawIndex = NULL;
	}
	if(m_pPnt2Seed != NULL)
		{
		delete []m_pPnt2Seed;
		m_pPnt2Seed = NULL;
		}
	if(m_pSeed != NULL)
		{
		delete [] m_pSeed;
		m_pSeed =NULL;
		}
	if(!m_PointArray.IsEmpty())
		{
		m_PointArray.RemoveAll();
                }
	if(!m_FacetList.IsEmpty())
		{
		m_FacetList.RemoveAll();
		}
	if(!m_EdgeList.IsEmpty())
		{
		m_EdgeList.RemoveAll();
		}
}


void XTriMesh::SetUnit(const int& unit)
{
}
void XTriMesh::UpdateBndBox()
{	
	int i;
	m_BndBox.Reset();

	for( i=0;i<m_iPointNum; i++)
	{		
		m_BndBox.AddVertex(m_PointArray[i].m_Coord);
	}
	PNT & min = m_BndBox.GetMin();
	PNT & max = m_BndBox.GetMax();
	for( i=0;i<3;i++)
	{
		if(max[2]-min[2]<0.1) 
		{
			min[i] -= 0.05;
			max[i] += 0.05;
		}
	}
}



inline bool XTriMesh::IsPntInFacet(const TriPoint & triPoint,const TriFacet & triAngle) const
{
	float a,b,c;
	int  i;
	unsigned  s, u, g;
	PNT tri[3];

	for(i=0; i<3; i++)
	{
		tri[i] = m_PointArray[triAngle.m_VertexIndex[i]].m_Coord - triPoint.m_Coord;
	}
	s = 0;
	if( tri[0][0] > 0)
		s+=1;
	if( tri[0][1] > 0)
		s+=2;
	if( tri[1][0] > 0)
		s+=4;
	if( tri[1][1] > 0)
		s+=8;
	if( tri[2][0] > 0)
		s+=16;
	if( tri[2][1] > 0)
		s+=32;
	g = 21;
	u = s&g;
	if(u ==0|| u ==21)
		return false;
	g = 42;
	u = s&g;
	if(u ==0|| u == 42)
		return  false;
	a = tri[1][0]*tri[2][1]-tri[1][1]*tri[2][0];
	b = tri[0][1]*tri[2][0]-tri[0][0]*tri[2][1];
	c = tri[0][0]*tri[1][1]-tri[0][1]*tri[1][0];

	return  ((a>=0 && b>=0 && c>= 0)||(a<=0 && b<=0 && c<= 0));
}

inline bool XTriMesh::IsPntInFacet(const PNT & v0,const PNT & v1,const PNT & v2) const
{
	float a,b,c;
	unsigned  s, u, g;
	PNT tri[3] = {v0,v1,v2};

	s = 0;
	if( tri[0][0] > 0)
		s+=1;
	if( tri[0][1] > 0)
		s+=2;
	if( tri[1][0] > 0)
		s+=4;
	if( tri[1][1] > 0)
		s+=8;
	if( tri[2][0] > 0)
		s+=16;
	if( tri[2][1] > 0)
		s+=32;
	g = 21;
	u = s&g;
	if(u ==0|| u ==21)
		return false;
	g = 42;
	u = s&g;
	if(u ==0|| u == 42)
		return  false;
	a = tri[1][0]*tri[2][1]-tri[1][1]*tri[2][0];
	b = tri[0][1]*tri[2][0]-tri[0][0]*tri[2][1];
	c = tri[0][0]*tri[1][1]-tri[0][1]*tri[1][0];

	return  ((a>=0 && b>=0 && c>= 0)||(a<=0 && b<=0 && c<= 0));
}

//Calculator for test overlap
bool XTriMesh::IsPntOverlapFacet(CArray<TriPoint,TriPoint&> & pntArray,const TriFacet & triAngle,float heightFact)
{
	// coordination translate&rotate
	int i;
	float x[3];
	PNT axis_z(triAngle.m_Normal),triCenter,triV0,triV1,triV2,proPnt;
	for(i=0;i<3;i++)
	{
		if(fabs(axis_z[i])>error)
			break;
	}
	if(i==3) return false;

	x[i] = axis_z[(i+1)%3] + axis_z[(i+2)%3];
	x[(i+1)%3] = x[(i+2)%3] = -axis_z[i];
	PNT axis_x(x),axis_y;

	axis_x.Normalize();
	axis_z.Normalize();

	axis_y = axis_z^axis_x;
	axis_y.Normalize();
	axis_x = axis_y^axis_z;
	axis_x.Normalize();

	int count = pntArray.GetSize();
	int iCounter = 0;

	//here need revise 8.5 Yang,Pinghai
	for(i=0;i<count;i++)
	{
		if(pntArray[i].m_iPntType!=3) continue;
		if(pntArray[i].m_iPntStat==-1||pntArray[i].m_iPntStat==2) continue;

		proPnt[0] = axis_x[0]*pntArray[i].m_Coord[0] + axis_x[1]*pntArray[i].m_Coord[1] + axis_x[2]*pntArray[i].m_Coord[2];
		proPnt[1] = axis_y[0]*pntArray[i].m_Coord[0] + axis_y[1]*pntArray[i].m_Coord[1] + axis_y[2]*pntArray[i].m_Coord[2];
		proPnt[2] = axis_z[0]*pntArray[i].m_Coord[0] + axis_z[1]*pntArray[i].m_Coord[1] + axis_z[2]*pntArray[i].m_Coord[2];

		triV0[0] = axis_x[0]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[0] + axis_x[1]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[1] + axis_x[2]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[2];
		triV0[1] = axis_y[0]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[0] + axis_y[1]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[1] + axis_y[2]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[2];
		triV0[2] = axis_z[0]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[0] + axis_z[1]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[1] + axis_z[2]*m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[2];

		triV1[0] = axis_x[0]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[0] + axis_x[1]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[1] + axis_x[2]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[2];
		triV1[1] = axis_y[0]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[0] + axis_y[1]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[1] + axis_y[2]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[2];
		triV1[2] = axis_z[0]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[0] + axis_z[1]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[1] + axis_z[2]*m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[2];

		triV2[0] = axis_x[0]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[0] + axis_x[1]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[1] + axis_x[2]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[2];
		triV2[1] = axis_y[0]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[0] + axis_y[1]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[1] + axis_y[2]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[2];
		triV2[2] = axis_z[0]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[0] + axis_z[1]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[1] + axis_z[2]*m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[2];

		triCenter = (triV0 + triV1 + triV2)/3;

		//if(fabs(triCenter[2]-proPnt[2])>heightFact*((triCenter-triV0).Length2D()+(triCenter-triV1).Length2D()+(triCenter-triV2).Length2D())) continue;
		if(fabs(triCenter[2]-proPnt[2])>heightFact) continue;

		//triV0 = heightFact*(triV0-triCenter) + triCenter;
		//triV1 = heightFact*(triV1-triCenter) + triCenter;
		//triV2 = heightFact*(triV2-triCenter) + triCenter;

		if(IsPntInFacet(triV0-proPnt,triV1-proPnt,triV2-proPnt))
		{
			pntArray[i].m_iPntStat = 2;
			iCounter++;
		}
	}
	return iCounter>0;
}


void XTriMesh::DeleteOverlayRegion(XTriMesh & triMeshB,PNT & viewDir)
{
	int i;

	//select by bndbox 
	XBndBox box = GetBBox();
	PNT  minA = box.GetMin();
	PNT  maxA = box.GetMax();
	XBndBox boxb = triMeshB.GetBBox();
	PNT  minB = boxb.GetMin();
	PNT  maxB = boxb.GetMax();
	TriPoint * pPoint = NULL;
	TriFacet * pFacet = NULL;
	for(i=0;i<triMeshB.m_iPointNum;i++)
	{
		pPoint = &triMeshB.m_PointArray[i];
		if(pPoint->m_Coord[0]<maxA[0]&&pPoint->m_Coord[1]<maxA[1]&&pPoint->m_Coord[2]<maxA[2]
		&&pPoint->m_Coord[0]>minA[0]&&pPoint->m_Coord[1]>minA[1]&&pPoint->m_Coord[2]>minA[2])
		{
			pPoint->m_iPntType = 3;
		}
	}
	for(i=0;i<m_iPointNum;i++)
	{
		pPoint = &m_PointArray[i];
		if(pPoint->m_Coord[0]<maxB[0]&&pPoint->m_Coord[1]<maxB[1]&&pPoint->m_Coord[2]<maxB[2]
		&&pPoint->m_Coord[0]>minB[0]&&pPoint->m_Coord[1]>minB[1]&&pPoint->m_Coord[2]>minB[2])
		{
			pPoint->m_iPntType = 3;
		}
	}
	//select end

	POSITION pos = triMeshB.m_FacetList.GetHeadPosition();
	while(pos)
	{
		pFacet = &triMeshB.m_FacetList.GetNext(pos);

		if(triMeshB.m_PointArray[pFacet->m_VertexIndex[0]].m_iPntType==3
			||triMeshB.m_PointArray[pFacet->m_VertexIndex[1]].m_iPntType==3
			||triMeshB.m_PointArray[pFacet->m_VertexIndex[2]].m_iPntType==3)
		{
			triMeshB.IsPntOverlapFacet(m_PointArray,*pFacet);
		}	
	}

	pos = m_FacetList.GetHeadPosition();
	while(pos)
	{
		pFacet = &m_FacetList.GetNext(pos);
		if(m_PointArray[pFacet->m_VertexIndex[0]].m_iPntType==3
			||m_PointArray[pFacet->m_VertexIndex[1]].m_iPntType==3
			||m_PointArray[pFacet->m_VertexIndex[2]].m_iPntType==3)
		{
			IsPntOverlapFacet(triMeshB.m_PointArray,*pFacet);
		}
	}

	CList<PNT,PNT&> viewDirList;
	PNT tempDir;
	m_iSeedCount = 0;
	PNT * pFitCloud = new PNT[m_iPointNum+triMeshB.m_iPointNum];
	int m=0;
	for(i=0;i<m_iPointNum;i++)
	{
		m_PointArray[i].m_iPntType = 0;
		if(m_PointArray[i].m_iPntStat==2)
		{
			m_iSeedCount++;
			DeletePntForIntegration(i,tempDir,-2);
			viewDirList.AddTail(tempDir);
			pFitCloud[m++] = m_PointArray[i].m_Coord;
		}
	}
	for(i=0;i<triMeshB.m_iPointNum;i++)
	{
		triMeshB.m_PointArray[i].m_iPntType = 0;
		if(triMeshB.m_PointArray[i].m_iPntStat==2)
		{
			m_iSeedCount++;
			triMeshB.DeletePntForIntegration(i,tempDir,-3);
			viewDirList.AddTail(tempDir);
			pFitCloud[m++] = triMeshB.m_PointArray[i].m_Coord;
		}
	}

	//important changes: use different method to decide the view direction
	//need further work
	XCloud tempCld;
	for(i=0;i<m;i++)
	{
		tempCld.AddPoint(pFitCloud[i]);
	}
	tempCld.Fit2Plane(tempDir,viewDir);

	/*viewDir.Zero();
	pos = viewDirList.GetHeadPosition();
	while(pos)
	{
		viewDir += viewDirList.GetNext(pos);
	}
	viewDir.Normalize();
	viewDir += m_ViewDirection + triMeshB.m_ViewDirection;*/
	viewDir.Normalize();
	
	delete []pFitCloud;
}


inline bool XTriMesh::IsPntInfluRegion(const TriPoint & pnt,const TriFacet & triAngle) const
{

	float x1,x2,x3,x4,y1,y2,y3,y4;
	x1 = m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[0];
	x2 = m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[0];
	x3 = m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[0];
	x4 = pnt.m_Coord[0];
	y1 = m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[1];
	y2 = m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[1];
	y3 = m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[1];
	y4 = pnt.m_Coord[1];

	return ((y4-y1)*(x2-x3)+(x4-x1)*(y2-y3))*((x4-x3)*(x2-x1)-(y4-y3)*(y2-y1)) >
		((y4-y3)*(x2-x1)+(x4-x3)*(y2-y1))*((x4-x1)*(x2-x3)-(y4-y1)*(y2-y3));
}

POSITION XTriMesh::FindPntOuterFacet(const TriPoint & pnt)
{
	int i,j;
	//TriFacet * pTempTri = &m_FacetList.GetTail();
	//debug
	TriFacet * pTempTri = NULL;
	if(m_iPointNum>2)
	{
		int switchNum = 0;
		float dis[4];
		dis[0] = fabsf(v01[0]-pnt.m_Coord[0])+fabsf(v01[1]-pnt.m_Coord[1]);
		dis[1] = fabsf(v12[0]-pnt.m_Coord[0])+fabsf(v12[1]-pnt.m_Coord[1]);
		dis[2] = fabsf(v20[0]-pnt.m_Coord[0])+fabsf(v20[1]-pnt.m_Coord[1]);
		dis[3] = 1.2*(fabsf(m_PointArray[m_iPointNum-2].m_Coord[0]-pnt.m_Coord[0])+fabsf(m_PointArray[m_iPointNum-2].m_Coord[1]-pnt.m_Coord[1]));

		for(int i=1;i<4;i++)
		{
			if(dis[i-1]<dis[i])
			{
				swap(dis[i-1],dis[i]);
				switchNum += i*i;
			}
		}
		switch(switchNum)
		{
		case 0:
		case 1:
		case 4:
		case 5:
			pTempTri = &m_FacetList.GetTail();		
			break;
		case 9:
		case 10:
			pTempTri = &m_FacetList.GetAt(afxEdge20->m_iNeighbFacet);
			break;
		case 13:
			pTempTri = &m_FacetList.GetAt(afxEdge12->m_iNeighbFacet);
			break;
		case 14:
			pTempTri = &m_FacetList.GetAt(afxEdge01->m_iNeighbFacet);
			break;
		default:
			pTempTri = &m_FacetList.GetTail();		
			break;
		}
	}
	else
	{
		pTempTri = &m_FacetList.GetTail();	
	}

	//debug end

	//TriFacet * pTempTri = &m_FacetList.GetHead();
	POSITION facetPos = pTempTri->m_iIndex;
	int facetCount = m_FacetList.GetCount();
	PNT *v0,*v1,*v2;
	float area0,area1,area2;

	v0 = &m_PointArray[pTempTri->m_VertexIndex[0]].m_Coord;
	v1 = &m_PointArray[pTempTri->m_VertexIndex[1]].m_Coord;
	v2 = &m_PointArray[pTempTri->m_VertexIndex[2]].m_Coord;

	area0 = CalculateTriArea(pnt.m_Coord,*v0,*v1);
	area1 = CalculateTriArea(pnt.m_Coord,*v1,*v2);
	area2 = CalculateTriArea(pnt.m_Coord,*v2,*v0);

	if(area0<=area1&&area0<=area2)
	{
		if(area0>=0) return facetPos;
		pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[0]);
	}
	else if(area1<=area2)
	{
		if(area1>=0) return facetPos;
		pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[1]);
	}
	else
	{
		if(area2>=0) return facetPos;
		pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[2]);
	}

	for(i=1;i<facetCount;i++)
	{
		for(j=0;j<3;j++)
		{
			if(pTempTri->m_pNeighbFacet[j] == facetPos)
				break;
		}
		v0 = &m_PointArray[pTempTri->m_VertexIndex[(j+1)%3]].m_Coord;
		v1 = &m_PointArray[pTempTri->m_VertexIndex[(j+2)%3]].m_Coord;
		v2 = &m_PointArray[pTempTri->m_VertexIndex[j]].m_Coord;
		facetPos = pTempTri->m_iIndex;

		area0 = CalculateTriArea(pnt.m_Coord,*v0,*v1);
		area1 = CalculateTriArea(pnt.m_Coord,*v1,*v2);
		if(area0>=0&&area1>=0) return facetPos;

		if(area0<=area1)
		{
			pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[(j+1)%3]);
		}
		else
		{
			pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[(j+2)%3]);
		}
	}
#ifdef _RELEASE
	if(IsPntInfluRegion(pnt,*pTempTri)) return facetPos;
#endif
	return NULL;
}


POSITION XTriMesh::FindPntOuterFacetSeed(const TriPoint & pnt)
{
	int i,j;

	if(m_pSeed[m_iSeedIndex] == NULL)
		return NULL;
	TriFacet * pTempTri = &m_FacetList.GetAt(m_pSeed[m_iSeedIndex]);

	POSITION facetPos = pTempTri->m_iIndex;
	
	int facetCount = m_FacetList.GetCount();
	PNT *v0,*v1,*v2;
	double area0,area1,area2;

	v0 = &m_PointArray[pTempTri->m_VertexIndex[0]].m_Coord;
	v1 = &m_PointArray[pTempTri->m_VertexIndex[1]].m_Coord;
	v2 = &m_PointArray[pTempTri->m_VertexIndex[2]].m_Coord;

	area0 = CalculateTriArea(pnt.m_Coord,*v0,*v1);
	area1 = CalculateTriArea(pnt.m_Coord,*v1,*v2);
	area2 = CalculateTriArea(pnt.m_Coord,*v2,*v0);

	if(area0<=area1&&area0<=area2)
		{
		if(area0>=0) return facetPos;
		pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[0]);
		}
	else 
		if(area1<=area2)
			{
			if(area1>=0) return facetPos;
			pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[1]);
			}
		else
			{
			if(area2>=0) return facetPos;
			pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[2]);
			}

	//test
	for(i=1;i<sqrt((double)facetCount);i++)
		//for(i=1;i<facetCount;i++)
	{
		for(j=0;j<3;j++)
		{
			if(pTempTri->m_pNeighbFacet[j] == facetPos)
				break;
		}
		if(j==3) break;
		
		v0 = &m_PointArray[pTempTri->m_VertexIndex[(j+1)%3]].m_Coord;
		v1 = &m_PointArray[pTempTri->m_VertexIndex[(j+2)%3]].m_Coord;
		v2 = &m_PointArray[pTempTri->m_VertexIndex[j]].m_Coord;
		facetPos = pTempTri->m_iIndex;

		area0 = CalculateTriArea(pnt.m_Coord,*v0,*v1);
		area1 = CalculateTriArea(pnt.m_Coord,*v1,*v2);

		if(area0>=0&&area1>=0) return facetPos;
		if(area0<=area1)
		{
			pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[(j+1)%3]);
		}
		else
		{
			pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[(j+2)%3]);
		}
	}

#ifdef _RELEASE
	if(IsPntInfluRegion(pnt,*pTempTri)) return facetPos;
#endif

	return NULL;
}


POSITION XTriMesh::FindPntOuterFacetIntegration(const TriPoint & pnt)
{
	int i,j;

	TriFacet * pTempTri = &m_FacetList.GetTail();

	POSITION facetPos = pTempTri->m_iIndex;
	int facetCount = m_FacetList.GetCount();

	PNT *v0,*v1,*v2;
	double area0,area1,area2;

	v0 = &m_PointArray[pTempTri->m_VertexIndex[0]].m_Coord;
	v1 = &m_PointArray[pTempTri->m_VertexIndex[1]].m_Coord;
	v2 = &m_PointArray[pTempTri->m_VertexIndex[2]].m_Coord;

	area0 = CalculateTriArea(pnt.m_Coord,*v0,*v1);
	area1 = CalculateTriArea(pnt.m_Coord,*v1,*v2);
	area2 = CalculateTriArea(pnt.m_Coord,*v2,*v0);

	if(area0>=0&&area1>=0&&area2>=0) return facetPos;

	if(pTempTri->m_pNeighbFacet[0]==NULL) area0 = 256;
	if(pTempTri->m_pNeighbFacet[1]==NULL) area1 = 256;
	if(pTempTri->m_pNeighbFacet[2]==NULL) area2 = 256;

	if(area0>0&&area1>0&&area2>0) return NULL;

	if(area0<=area1&&area0<=area2)
	{
		pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[0]);
		if(pTempTri->m_iFacetType==9)
		{
			return NULL;
		}
	}
	else if(area1<=area2)
	{
		pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[1]);
		if(pTempTri->m_iFacetType==9)
		{
			return NULL;
		}
	}
	else
	{
		pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[2]);
		if(pTempTri->m_iFacetType==9)
		{
			return NULL;
		}
	}

	//for(i=1;i<facetCount;i++)
	for(i=1;i<m_iSeedCount*4;i++)
	{
		if(pTempTri->m_iIndex==NULL) return NULL;
		for(j=0;j<3;j++)
		{
			if(pTempTri->m_pNeighbFacet[j] == facetPos)
				break;
		}
		if(j==3) return NULL;

		v0 = &m_PointArray[pTempTri->m_VertexIndex[(j+1)%3]].m_Coord;
		v1 = &m_PointArray[pTempTri->m_VertexIndex[(j+2)%3]].m_Coord;
		v2 = &m_PointArray[pTempTri->m_VertexIndex[j]].m_Coord;
		facetPos = pTempTri->m_iIndex;

		area0 = CalculateTriArea(pnt.m_Coord,*v0,*v1);
		area1 = CalculateTriArea(pnt.m_Coord,*v1,*v2);

		if(area0>=0&&area1>=0) return facetPos;

		if(pTempTri->m_pNeighbFacet[(j+1)%3]==NULL) area0 = 256;
		if(pTempTri->m_pNeighbFacet[(j+2)%3]==NULL) area1 = 256;

		if(area0>0&&area1>0) break;

		if(area0<=area1)
		{
			pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[(j+1)%3]);
			if(pTempTri->m_iFacetType==9)
			{
				if(area1<0)
				{
					pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[(j+2)%3]);
					if(pTempTri->m_iFacetType==9) return NULL;
				}
				else
				{
					break;
				}	
			}
		}
		else
		{
			pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[(j+2)%3]);
			if(pTempTri->m_iFacetType==9)
			{
				if(area0<0)
				{
					pTempTri = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[(j+1)%3]);
					if(pTempTri->m_iFacetType==9) return NULL;
				}
				else
				{
					break;
				}	
			}
		}
	}
	return NULL;
}


void XTriMesh::CalculateInfluAndDelete(const TriPoint & pnt,CPosList & outerEdge)
{
	int i;

	POSITION facetPos = FindPntOuterFacet(pnt);
	if(facetPos==NULL) return;
	TriFacet * pFacet = &m_FacetList.GetAt(facetPos);
	if(pFacet==NULL) return;
	outerEdge.RemoveAll();

	POSITION pos,oldPos;
	CPosList facetInRegion;
	CPosList edgeInRegion;
	TriEdge * pEdge = NULL;

	facetInRegion.AddTail(pFacet->m_iIndex);
	for(i=0;i<3;i++)
	{
		if(pFacet->m_pNeighbFacet[i]==NULL)
		{
			outerEdge.AddTail(pFacet->m_edge[i]);
		}
		else
		{
			edgeInRegion.AddTail(pFacet->m_edge[i]);
		}
	}

	pFacet->m_iFacetType = 8;

	for(pos=edgeInRegion.GetHeadPosition();pos;)
	{
		oldPos = pos;
		pEdge = &m_EdgeList.GetAt(edgeInRegion.GetNext(pos));

		pFacet = &m_FacetList.GetAt(m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iNeighbFacet);
		if(pFacet->m_iFacetType==8)
		{
			m_EdgeList.RemoveAt(pEdge->m_iCorrespIndex);
			m_EdgeList.RemoveAt(pEdge->m_iIndex);
			continue;
		}

		if(IsPntInfluRegion(pnt,*pFacet))
		{	
			facetInRegion.AddTail(pFacet->m_iIndex);
			for(i=0;i<3;i++)
			{
				if(pFacet->m_edge[i]==pEdge->m_iCorrespIndex) break;
			}
			if(pFacet->m_pNeighbFacet[(i+1)%3]==NULL)
			{
				outerEdge.AddTail(pFacet->m_edge[(i+1)%3]);
			}
			else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[(i+1)%3]).m_iFacetType!=8)
			{
				edgeInRegion.AddTail(pFacet->m_edge[(i+1)%3]);
			}
			if(pFacet->m_pNeighbFacet[(i+2)%3]==NULL)
			{
				outerEdge.AddTail(pFacet->m_edge[(i+2)%3]);
			}
			else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[(i+2)%3]).m_iFacetType!=8)
			{
				edgeInRegion.AddTail(pFacet->m_edge[(i+2)%3]);
			}
			pFacet->m_iFacetType = 8;
			if(pos==NULL)
			{
				edgeInRegion.GetNext(oldPos);
				pos = oldPos;
			}
			m_EdgeList.RemoveAt(pEdge->m_iCorrespIndex);
			m_EdgeList.RemoveAt(pEdge->m_iIndex);
		}
		else
		{
			outerEdge.AddTail(pEdge->m_iIndex);
		}
	}
	for(pos = facetInRegion.GetHeadPosition();pos;)
	{
		m_FacetList.RemoveAt(facetInRegion.GetNext(pos));
	}
}


void XTriMesh::CalculateInfluAndDeleteSeed(const TriPoint & pnt,CPosList & outerEdge)
{
	int i;

	POSITION facetPos = FindPntOuterFacetSeed(pnt);
	if(facetPos==NULL) return;
	TriFacet * pFacet = &m_FacetList.GetAt(facetPos);
	if(pFacet==NULL) return;
	outerEdge.RemoveAll();

	POSITION pos,oldPos;
	CPosList facetInRegion;
	CPosList edgeInRegion;
	TriEdge * pEdge = NULL;

	//add all valid edges into edgeInRegion List
	facetInRegion.AddTail(pFacet->m_iIndex);
	for(i=0;i<3;i++)
	{

		if(pFacet->m_pNeighbFacet[i]==NULL)
		{
			outerEdge.AddTail(pFacet->m_edge[i]);
		}
		else
		{
			edgeInRegion.AddTail(pFacet->m_edge[i]);
		}
	}
	pFacet->m_iFacetType = 8;

	for(pos=edgeInRegion.GetHeadPosition();pos;)
	{
		oldPos = pos;
		pEdge = &m_EdgeList.GetAt(edgeInRegion.GetNext(pos));

		pFacet = &m_FacetList.GetAt(m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iNeighbFacet);
		if(pFacet->m_iFacetType==8)
		{
		/*	m_EdgeList.RemoveAt(pEdge->m_iCorrespIndex);
			m_EdgeList.RemoveAt(pEdge->m_iIndex);*/
			continue;
		}

		if(IsPntInfluRegion(pnt,*pFacet))
		{	
			facetInRegion.AddTail(pFacet->m_iIndex);
			for(i=0;i<3;i++)
			{
				if(pFacet->m_edge[i]==pEdge->m_iCorrespIndex) break;
			}

			if(pFacet->m_pNeighbFacet[(i+1)%3]==NULL)
			{
				outerEdge.AddTail(pFacet->m_edge[(i+1)%3]);
			}
			else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[(i+1)%3]).m_iFacetType!=8)
			{
				edgeInRegion.AddTail(pFacet->m_edge[(i+1)%3]);
			}
			if(pFacet->m_pNeighbFacet[(i+2)%3]==NULL)
			{
				outerEdge.AddTail(pFacet->m_edge[(i+2)%3]);
			}
			else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[(i+2)%3]).m_iFacetType!=8)
			{
				edgeInRegion.AddTail(pFacet->m_edge[(i+2)%3]);
			}
			pFacet->m_iFacetType = 8;
			if(pos==NULL)
			{
				edgeInRegion.GetNext(oldPos);
				pos = oldPos;
			}
		/*	m_EdgeList.RemoveAt(pEdge->m_iCorrespIndex);
			m_EdgeList.RemoveAt(pEdge->m_iIndex);*/
		}
		else
		{
			outerEdge.AddTail(pEdge->m_iIndex);
		}
	}
	//for(pos = facetInRegion.GetHeadPosition();pos;)
	//{
	//	m_FacetList.RemoveAt(facetInRegion.GetNext(pos));
	//}

	//add test
	edgeInRegion.RemoveAll();
	outerEdge.RemoveAll();
	for(pos = facetInRegion.GetHeadPosition();pos;)
	{
		TriFacet * pFacet = &m_FacetList.GetAt(facetInRegion.GetNext(pos));
		for(int j=0;j<3;j++)
		{
			edgeInRegion.AddTail(pFacet->m_edge[j]);
		}
		m_FacetList.RemoveAt(pFacet->m_iIndex);
	}

	int outEdge = edgeInRegion.GetCount();
	POSITION edgeInRegionPos1,edgePos1 ,edgePos ,edgeInRegionPos;
	for(i=0;i<outEdge;i++)
	{
		edgeInRegionPos1 = edgeInRegion.FindIndex(i);
		edgePos1 = edgeInRegion.GetAt(edgeInRegionPos1);
		edgePos = m_EdgeList.GetAt(edgeInRegion.GetAt(edgeInRegionPos1)).m_iCorrespIndex;

		if(edgePos!=NULL)
		{		
			edgeInRegionPos = edgeInRegion.Find(edgePos,edgeInRegionPos1);
			if(edgeInRegionPos!=NULL)
			{
				m_EdgeList.RemoveAt(edgePos1);
				m_EdgeList.RemoveAt(edgePos);
				edgeInRegion.RemoveAt(edgeInRegionPos1);
				edgeInRegion.RemoveAt(edgeInRegionPos);
				outEdge -= 2;
				i--;//important revise
			}
		}
	}
	outerEdge.AddTail(&edgeInRegion);
}

void XTriMesh::CalculateInfluAndDeleteIntegration(const TriPoint & pnt,CPosList & outerEdge)
{
	int i;

	POSITION facetPos = FindPntOuterFacetIntegration(pnt);
	if(facetPos==NULL) return;

	TriFacet * pFacet = &m_FacetList.GetAt(facetPos);
	if(pFacet==NULL) return;
	outerEdge.RemoveAll();

	POSITION pos,oldPos;
	CPosList facetInRegion;
	CPosList edgeInRegion;
	TriEdge * pEdge = NULL;

	facetInRegion.AddTail(pFacet->m_iIndex);
	for(i=0;i<3;i++)
	{
		if(pFacet->m_pNeighbFacet[i]==NULL)
		{
			outerEdge.AddTail(pFacet->m_edge[i]);
		}
		else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[i]).m_iFacetType==9)
		{
			outerEdge.AddTail(pFacet->m_edge[i]);
		}
		else
		{
			edgeInRegion.AddTail(pFacet->m_edge[i]);
		}
	}
	pFacet->m_iFacetType = 8;

	for(pos=edgeInRegion.GetHeadPosition();pos;)
	{
		oldPos = pos;
		pEdge = &m_EdgeList.GetAt(edgeInRegion.GetNext(pos));

		pFacet = &m_FacetList.GetAt(m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iNeighbFacet);
		if(pFacet->m_iFacetType==8)
		{
			m_EdgeList.RemoveAt(pEdge->m_iCorrespIndex);
			m_EdgeList.RemoveAt(pEdge->m_iIndex);
			continue;
		}

		if(IsPntInfluRegion(pnt,*pFacet))
		{	
			facetInRegion.AddTail(pFacet->m_iIndex);
			for(i=0;i<3;i++)
			{
				if(pFacet->m_edge[i]==pEdge->m_iCorrespIndex) break;
			}

			if(pFacet->m_pNeighbFacet[(i+1)%3]==NULL)
			{
				outerEdge.AddTail(pFacet->m_edge[(i+1)%3]);
			}
			else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[(i+1)%3]).m_iFacetType==9)
			{
				outerEdge.AddTail(pFacet->m_edge[(i+1)%3]);
			}
			else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[(i+1)%3]).m_iFacetType!=8)
			{
				edgeInRegion.AddTail(pFacet->m_edge[(i+1)%3]);
			}

			if(pFacet->m_pNeighbFacet[(i+2)%3]==NULL)
			{
				outerEdge.AddTail(pFacet->m_edge[(i+2)%3]);
			}
			else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[(i+2)%3]).m_iFacetType==9)
			{
				outerEdge.AddTail(pFacet->m_edge[(i+2)%3]);
			}
			else if(m_FacetList.GetAt(pFacet->m_pNeighbFacet[(i+2)%3]).m_iFacetType!=8)
			{
				edgeInRegion.AddTail(pFacet->m_edge[(i+2)%3]);
			}
			pFacet->m_iFacetType = 8;
			if(pos==NULL)
			{
				edgeInRegion.GetNext(oldPos);
				pos = oldPos;
			}
			m_EdgeList.RemoveAt(pEdge->m_iCorrespIndex);
			m_EdgeList.RemoveAt(pEdge->m_iIndex);
		}
		else
		{
			outerEdge.AddTail(pEdge->m_iIndex);
		}
	}
	for(pos = facetInRegion.GetHeadPosition();pos;)
	{
		m_FacetList.RemoveAt(facetInRegion.GetNext(pos));
	}
}


void XTriMesh::CalculateInfluRegion(const TriPoint & pnt,CPosList& facetInRegion)
{
	int j;
	facetInRegion.RemoveAll();
	POSITION facetPos = FindPntOuterFacet(pnt);
	if(facetPos==NULL) return;
	TriFacet * pInitialTri = &m_FacetList.GetAt(facetPos);

	if(pInitialTri!=NULL)
	{
		facetInRegion.AddTail(pInitialTri->m_iIndex);
		pInitialTri->m_iFacetType = 8;
		TriFacet * pFacet = NULL;
		TriFacet * pFacetJ = NULL;

		POSITION pos,oldPos;
		for(pos=facetInRegion.GetHeadPosition();pos;)
		{
			oldPos = pos;
			pFacet = &m_FacetList.GetAt(facetInRegion.GetNext(pos));
			for(j=0;j<3;j++)
			{
				if(pFacet->m_pNeighbFacet[j]==NULL) continue;
				pFacetJ = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[j]);
				if(pFacetJ->m_iFacetType==8) continue;
				if(IsPntInfluRegion(pnt,*pFacetJ))
				{
					pFacetJ->m_iFacetType = 8;
					facetInRegion.AddTail(pFacetJ->m_iIndex);
					if(pos==NULL)
					{
						facetInRegion.GetNext(oldPos);
						pos = oldPos;
					}
				}
			}
		}
	}
}


bool XTriMesh::AddInnerPntToTriMesh(const TriPoint & pnt)
{
	int i,j;
	TriPoint newPnt(pnt);
	newPnt.m_iIndex = m_iPointNum;
	m_PointArray.SetAt(m_iPointNum,newPnt);
	m_iPointNum++;

	CPosList edgeInRegion;
	CalculateInfluAndDelete(newPnt,edgeInRegion);
	if(edgeInRegion.GetCount() == 0)	
	{
		m_PointArray[m_iPointNum-1].m_iPntStat = -1;
		return false;
	}

	/*if(deleteFacet+2!=outEdge)
	{

	}*/
	//now all wrong edges are deleted
	//then begin to add new edges. NewEdgeCount = OldEdgeCount + 2

	CIntList pointLoop;
	CPosList edgeLoop;
	InitialPolygonLoop(edgeInRegion,pointLoop,edgeLoop);

	int pointCount = pointLoop.GetCount();
	pointLoop.AddTail(pointLoop.GetHead());

	POSITION edge1,startFacetPos,startEdgePos,pntLoopPos,edgeLoopPos;
	TriEdge * pNewEdge0= NULL;
	TriEdge * pNewEdge2 = NULL;
	TriFacet * pOldFacet = NULL;
	TriFacet * pNewFacet = NULL;
	int startPnt,endPnt;
	bool bNeighbor;

	edgeLoopPos = edgeLoop.GetHeadPosition();
	pntLoopPos = pointLoop.GetHeadPosition();
	edge1 = edgeLoop.GetNext(edgeLoopPos);
	startPnt = pointLoop.GetNext(pntLoopPos);
	endPnt = pointLoop.GetAt(pntLoopPos);

	m_EdgeList.AddTail(TriEdge());
	pNewEdge0 = &m_EdgeList.GetTail();
	pNewEdge0->m_VertexIndex[0] = newPnt.m_iIndex;
	pNewEdge0->m_VertexIndex[1] = startPnt;
	pNewEdge0->m_iIndex = startEdgePos = m_EdgeList.GetTailPosition();

	m_EdgeList.AddTail(TriEdge());
	pNewEdge2 = &m_EdgeList.GetTail();
	pNewEdge2->m_VertexIndex[0] = endPnt;
	pNewEdge2->m_VertexIndex[1] = newPnt.m_iIndex;
	pNewEdge2->m_iIndex = m_EdgeList.GetTailPosition();

	m_FacetList.AddTail(TriFacet());
	pNewFacet = pOldFacet = &m_FacetList.GetTail();
	pNewFacet->m_iIndex = startFacetPos = m_FacetList.GetTailPosition();

	pNewFacet->m_VertexIndex[0] = newPnt.m_iIndex;
	pNewFacet->m_VertexIndex[1] = startPnt;
	pNewFacet->m_VertexIndex[2] = endPnt;
	pNewFacet->m_edge[0] = pNewEdge0->m_iIndex;
	pNewFacet->m_edge[1] = edge1;
	pNewFacet->m_edge[2] = pNewEdge2->m_iIndex;

	bNeighbor = false;
	if(m_EdgeList.GetAt(edge1).m_iCorrespIndex != NULL) 
	{
		bNeighbor = true;
		pNewFacet->m_pNeighbFacet[1] = m_EdgeList.GetAt(m_EdgeList.GetAt(edge1).m_iCorrespIndex).m_iNeighbFacet;
	}
	else pNewFacet->m_pNeighbFacet[1] = NULL;

	//m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet =
	//	m_EdgeList.GetAt(edge1).m_iNeighbFacet =
	//	m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	if(bNeighbor)
	{
		for(j=0;j<3;j++)
		{
			if(m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_edge[j] 
			== m_EdgeList.GetAt(edge1).m_iCorrespIndex)
				break;
		}
		m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_pNeighbFacet[j] = pNewFacet->m_iIndex;
	}
	m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet = 
		m_EdgeList.GetAt(edge1).m_iNeighbFacet = 
		m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	for(i=1;i<pointCount;i++)
	{
		edge1 = edgeLoop.GetNext(edgeLoopPos);
		startPnt = pointLoop.GetNext(pntLoopPos);
		endPnt = pointLoop.GetAt(pntLoopPos);

		m_EdgeList.AddTail(TriEdge());
		pNewEdge0 = &m_EdgeList.GetTail();
		pNewEdge0->m_VertexIndex[0] = newPnt.m_iIndex;
		pNewEdge0->m_VertexIndex[1] = startPnt;
		pNewEdge0->m_iIndex = m_EdgeList.GetTailPosition();
		pNewEdge0->m_iCorrespIndex = pNewEdge2->m_iIndex;
		pNewEdge2->m_iCorrespIndex = pNewEdge0->m_iIndex;

		m_EdgeList.AddTail(TriEdge());
		pNewEdge2 = &m_EdgeList.GetTail();
		pNewEdge2->m_VertexIndex[0] = endPnt;
		pNewEdge2->m_VertexIndex[1] = newPnt.m_iIndex;
		pNewEdge2->m_iIndex = m_EdgeList.GetTailPosition();

		m_FacetList.AddTail(TriFacet());
		pNewFacet = &m_FacetList.GetTail();
		pNewFacet->m_iIndex = m_FacetList.GetTailPosition();

		pNewFacet->m_VertexIndex[0] = newPnt.m_iIndex;
		pNewFacet->m_VertexIndex[1] = startPnt;
		pNewFacet->m_VertexIndex[2] = endPnt;
		pNewFacet->m_edge[0] = pNewEdge0->m_iIndex;
		pNewFacet->m_edge[1] = edge1;
		pNewFacet->m_edge[2] = pNewEdge2->m_iIndex;

		pNewFacet->m_pNeighbFacet[0] = pOldFacet->m_iIndex;
		pOldFacet->m_pNeighbFacet[2] = pNewFacet->m_iIndex;
		pOldFacet = pNewFacet;

		bNeighbor = false;
		if(m_EdgeList.GetAt(edge1).m_iCorrespIndex != NULL) 
		{
			bNeighbor = true;
			pNewFacet->m_pNeighbFacet[1] = 
				m_EdgeList.GetAt(m_EdgeList.GetAt(edge1).m_iCorrespIndex).m_iNeighbFacet;
		}
		else pNewFacet->m_pNeighbFacet[1] = NULL;

		//m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet =
		//	m_EdgeList.GetAt(edge1).m_iNeighbFacet =
		//	m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

		if(bNeighbor)
		{
			for(j=0;j<3;j++)
			{
				if(m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_edge[j] 
				== m_EdgeList.GetAt(edge1).m_iCorrespIndex)
					break;
			}
			m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_pNeighbFacet[j] = pNewFacet->m_iIndex;
		}
		m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet = 
			m_EdgeList.GetAt(edge1).m_iNeighbFacet = 
			m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	}

	m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iCorrespIndex = startEdgePos;
	m_EdgeList.GetAt(startEdgePos).m_iCorrespIndex = pNewEdge2->m_iIndex;
	m_FacetList.GetAt(pNewFacet->m_iIndex).m_pNeighbFacet[2] = startFacetPos;
	m_FacetList.GetAt(startFacetPos).m_pNeighbFacet[0] = pNewFacet->m_iIndex;

	/*if(m_FacetList.GetCount()!=2*m_iPointNum+1||m_FacetList.GetCount()*3!=m_EdgeList.GetCount())
	{
	}*/
	return true;
}

void XTriMesh::AddInnerPntToTriMeshSeed(const TriPoint & pnt)
{
	int i,j;
	TriPoint newPnt(pnt);
	newPnt.m_iIndex = m_iPointNum;
	m_PointArray.SetAt(m_iPointNum,newPnt);
	m_iPointNum++;

	CPosList edgeInRegion;
	CalculateInfluAndDeleteSeed(newPnt,edgeInRegion);
	if(edgeInRegion.GetCount() == 0) 	
	{
		m_PointArray[m_iPointNum-1].m_iPntStat = -1;
		return ;
	}

	/*if(deleteFacet+2!=outEdge)
	{
	}*/

	CIntList pointLoop;
	CPosList edgeLoop;
	if(!InitialPolygonLoop(edgeInRegion,pointLoop,edgeLoop))
	{
		m_PointArray[m_iPointNum-1].m_iPntStat = -1;
		return;
	}

	int pointCount = pointLoop.GetCount();
	pointLoop.AddTail(pointLoop.GetHead());

	POSITION edge1,startFacetPos,startEdgePos,pntLoopPos,edgeLoopPos;
	edge1 = startEdgePos = startFacetPos = pntLoopPos = edgeLoopPos = NULL;
	TriEdge * pNewEdge0= NULL;
	TriEdge * pNewEdge2 = NULL;
	TriFacet * pOldFacet = NULL;
	TriFacet * pNewFacet = NULL;
	int startPnt,endPnt;
	bool bNeighbor= false;

	edgeLoopPos = edgeLoop.GetHeadPosition();
	pntLoopPos = pointLoop.GetHeadPosition();
	edge1 = edgeLoop.GetNext(edgeLoopPos);
	startPnt = pointLoop.GetNext(pntLoopPos);
	endPnt = pointLoop.GetAt(pntLoopPos);

	m_EdgeList.AddTail(TriEdge());
	pNewEdge0 = &m_EdgeList.GetTail();
	pNewEdge0->m_VertexIndex[0] = newPnt.m_iIndex;
	pNewEdge0->m_VertexIndex[1] = startPnt;
	pNewEdge0->m_iIndex = startEdgePos = m_EdgeList.GetTailPosition();

	m_EdgeList.AddTail(TriEdge());
	pNewEdge2 = &m_EdgeList.GetTail();
	pNewEdge2->m_VertexIndex[0] = endPnt;
	pNewEdge2->m_VertexIndex[1] = newPnt.m_iIndex;
	pNewEdge2->m_iIndex = m_EdgeList.GetTailPosition();

	m_FacetList.AddTail(TriFacet());
	pNewFacet = pOldFacet = &m_FacetList.GetTail();
	pNewFacet->m_iIndex = startFacetPos = m_FacetList.GetTailPosition();

	pNewFacet->m_VertexIndex[0] = newPnt.m_iIndex;
	pNewFacet->m_VertexIndex[1] = startPnt;
	pNewFacet->m_VertexIndex[2] = endPnt;
	pNewFacet->m_edge[0] = pNewEdge0->m_iIndex;
	pNewFacet->m_edge[1] = edge1;
	pNewFacet->m_edge[2] = pNewEdge2->m_iIndex;

	if(startPnt<m_iPnt2SeedCount) 
	{
		m_pSeed[m_pPnt2Seed[startPnt]] = pNewFacet->m_iIndex;
	}

	bNeighbor = false;
	if(m_EdgeList.GetAt(edge1).m_iCorrespIndex != NULL) 
	{
		bNeighbor = true;
		pNewFacet->m_pNeighbFacet[1] = m_EdgeList.GetAt(m_EdgeList.GetAt(edge1).m_iCorrespIndex).m_iNeighbFacet;
	}
	else pNewFacet->m_pNeighbFacet[1] = NULL;

	//m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet =
	//	m_EdgeList.GetAt(edge1).m_iNeighbFacet =
	//	m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	if(bNeighbor)
	{
		for(j=0;j<3;j++)
		{
			if(m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_edge[j] 
			== m_EdgeList.GetAt(edge1).m_iCorrespIndex)
				break;
		}
		m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_pNeighbFacet[j] = pNewFacet->m_iIndex;
	}
	m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet = 
		m_EdgeList.GetAt(edge1).m_iNeighbFacet = 
		m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	for(i=1;i<pointCount&&edgeLoopPos;i++)
	{
		edge1 = edgeLoop.GetNext(edgeLoopPos);
		startPnt = pointLoop.GetNext(pntLoopPos);
		endPnt = pointLoop.GetAt(pntLoopPos);

		m_EdgeList.AddTail(TriEdge());
		pNewEdge0 = &m_EdgeList.GetTail();
		pNewEdge0->m_VertexIndex[0] = newPnt.m_iIndex;
		pNewEdge0->m_VertexIndex[1] = startPnt;
		pNewEdge0->m_iIndex = m_EdgeList.GetTailPosition();
		pNewEdge0->m_iCorrespIndex = pNewEdge2->m_iIndex;
		pNewEdge2->m_iCorrespIndex = pNewEdge0->m_iIndex;

		m_EdgeList.AddTail(TriEdge());
		pNewEdge2 = &m_EdgeList.GetTail();
		pNewEdge2->m_VertexIndex[0] = endPnt;
		pNewEdge2->m_VertexIndex[1] = newPnt.m_iIndex;
		pNewEdge2->m_iIndex = m_EdgeList.GetTailPosition();

		m_FacetList.AddTail(TriFacet());
		pNewFacet = &m_FacetList.GetTail();
		pNewFacet->m_iIndex = m_FacetList.GetTailPosition();

		pNewFacet->m_VertexIndex[0] = newPnt.m_iIndex;
		pNewFacet->m_VertexIndex[1] = startPnt;
		pNewFacet->m_VertexIndex[2] = endPnt;
		pNewFacet->m_edge[0] = pNewEdge0->m_iIndex;
		pNewFacet->m_edge[1] = edge1;
		pNewFacet->m_edge[2] = pNewEdge2->m_iIndex;

		if(startPnt<m_iPnt2SeedCount) 
		{
			m_pSeed[m_pPnt2Seed[startPnt]] = pNewFacet->m_iIndex;
		}

		pNewFacet->m_pNeighbFacet[0] = pOldFacet->m_iIndex;
		pOldFacet->m_pNeighbFacet[2] = pNewFacet->m_iIndex;
		pOldFacet = pNewFacet;

		bNeighbor = false;
		if(m_EdgeList.GetAt(edge1).m_iCorrespIndex != NULL) 
		{
			bNeighbor = true;
			pNewFacet->m_pNeighbFacet[1] = 
				m_EdgeList.GetAt(m_EdgeList.GetAt(edge1).m_iCorrespIndex).m_iNeighbFacet;
		}
		else pNewFacet->m_pNeighbFacet[1] = NULL;

		//m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet =
		//	m_EdgeList.GetAt(edge1).m_iNeighbFacet =
		//	m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

		if(bNeighbor)
		{
			for(j=0;j<3;j++)
			{
				if(m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_edge[j] 
				== m_EdgeList.GetAt(edge1).m_iCorrespIndex)
					break;
			}
			m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_pNeighbFacet[j] = pNewFacet->m_iIndex;
		}
		m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet = 
			m_EdgeList.GetAt(edge1).m_iNeighbFacet = 
			m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	}

	m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iCorrespIndex = startEdgePos;
	m_EdgeList.GetAt(startEdgePos).m_iCorrespIndex = pNewEdge2->m_iIndex;
	m_FacetList.GetAt(pNewFacet->m_iIndex).m_pNeighbFacet[2] = startFacetPos;
	m_FacetList.GetAt(startFacetPos).m_pNeighbFacet[0] = pNewFacet->m_iIndex;

	/*if(m_FacetList.GetCount()!=2*m_iPointNum+1||m_FacetList.GetCount()*3!=m_EdgeList.GetCount())
	{
	}*/
}

void XTriMesh::AddInnerPntToTriMeshIntegration(int index)
{
	int i,j;
	TriPoint & newPnt = m_PointArray[index];

	CPosList edgeInRegion;
	CalculateInfluAndDeleteIntegration(newPnt,edgeInRegion);
	if(edgeInRegion.GetCount() == 0) return ;

	CIntList pointLoop;
	CPosList edgeLoop;
	InitialPolygonLoop(edgeInRegion,pointLoop,edgeLoop);

	int pointCount = pointLoop.GetCount();
	pointLoop.AddTail(pointLoop.GetHead());

	POSITION edge1,startFacetPos,startEdgePos,pntLoopPos,edgeLoopPos;
	TriEdge * pNewEdge0;
	TriEdge * pNewEdge2;
	TriFacet * pOldFacet = NULL;
	TriFacet * pNewFacet;
	int startPnt,endPnt;
	bool bNeighbor;

	edgeLoopPos = edgeLoop.GetHeadPosition();
	pntLoopPos = pointLoop.GetHeadPosition();
	edge1 = edgeLoop.GetNext(edgeLoopPos);
	startPnt = pointLoop.GetNext(pntLoopPos);
	endPnt = pointLoop.GetAt(pntLoopPos);

	m_EdgeList.AddTail(TriEdge());
	pNewEdge0 = &m_EdgeList.GetTail();
	pNewEdge0->m_VertexIndex[0] = newPnt.m_iIndex;
	pNewEdge0->m_VertexIndex[1] = startPnt;
	pNewEdge0->m_iIndex = startEdgePos = m_EdgeList.GetTailPosition();

	m_EdgeList.AddTail(TriEdge());
	pNewEdge2 = &m_EdgeList.GetTail();
	pNewEdge2->m_VertexIndex[0] = endPnt;
	pNewEdge2->m_VertexIndex[1] = newPnt.m_iIndex;
	pNewEdge2->m_iIndex = m_EdgeList.GetTailPosition();

	m_FacetList.AddTail(TriFacet());
	pNewFacet = pOldFacet = &m_FacetList.GetTail();
	pNewFacet->m_iIndex = startFacetPos = m_FacetList.GetTailPosition();

	pNewFacet->m_VertexIndex[0] = newPnt.m_iIndex;
	pNewFacet->m_VertexIndex[1] = startPnt;
	pNewFacet->m_VertexIndex[2] = endPnt;
	pNewFacet->m_edge[0] = pNewEdge0->m_iIndex;
	pNewFacet->m_edge[1] = edge1;
	pNewFacet->m_edge[2] = pNewEdge2->m_iIndex;

	bNeighbor = false;
	if(m_EdgeList.GetAt(edge1).m_iCorrespIndex != NULL) 
	{
		bNeighbor = true;
		pNewFacet->m_pNeighbFacet[1] = m_EdgeList.GetAt(m_EdgeList.GetAt(edge1).m_iCorrespIndex).m_iNeighbFacet;
	}
	else pNewFacet->m_pNeighbFacet[1] = NULL;

	m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet =
		m_EdgeList.GetAt(edge1).m_iNeighbFacet =
		m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	if(bNeighbor)
	{
		for(j=0;j<3;j++)
		{
			if(m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_edge[j] 
			== m_EdgeList.GetAt(edge1).m_iCorrespIndex)
				break;
		}
		m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_pNeighbFacet[j] = pNewFacet->m_iIndex;
	}
	m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet = 
		m_EdgeList.GetAt(edge1).m_iNeighbFacet = 
		m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	for(i=1;i<pointCount;i++)
	{
		edge1 = edgeLoop.GetNext(edgeLoopPos);
		startPnt = pointLoop.GetNext(pntLoopPos);
		endPnt = pointLoop.GetAt(pntLoopPos);

		m_EdgeList.AddTail(TriEdge());
		pNewEdge0 = &m_EdgeList.GetTail();
		pNewEdge0->m_VertexIndex[0] = newPnt.m_iIndex;
		pNewEdge0->m_VertexIndex[1] = startPnt;
		pNewEdge0->m_iIndex = m_EdgeList.GetTailPosition();
		pNewEdge0->m_iCorrespIndex = pNewEdge2->m_iIndex;
		pNewEdge2->m_iCorrespIndex = pNewEdge0->m_iIndex;

		m_EdgeList.AddTail(TriEdge());
		pNewEdge2 = &m_EdgeList.GetTail();
		pNewEdge2->m_VertexIndex[0] = endPnt;
		pNewEdge2->m_VertexIndex[1] = newPnt.m_iIndex;
		pNewEdge2->m_iIndex = m_EdgeList.GetTailPosition();

		m_FacetList.AddTail(TriFacet());
		pNewFacet = &m_FacetList.GetTail();
		pNewFacet->m_iIndex = m_FacetList.GetTailPosition();

		pNewFacet->m_VertexIndex[0] = newPnt.m_iIndex;
		pNewFacet->m_VertexIndex[1] = startPnt;
		pNewFacet->m_VertexIndex[2] = endPnt;
		pNewFacet->m_edge[0] = pNewEdge0->m_iIndex;
		pNewFacet->m_edge[1] = edge1;
		pNewFacet->m_edge[2] = pNewEdge2->m_iIndex;

		pNewFacet->m_pNeighbFacet[0] = pOldFacet->m_iIndex;
		pOldFacet->m_pNeighbFacet[2] = pNewFacet->m_iIndex;
		pOldFacet = pNewFacet;

		bNeighbor = false;
		if(m_EdgeList.GetAt(edge1).m_iCorrespIndex != NULL) 
		{
			bNeighbor = true;
			pNewFacet->m_pNeighbFacet[1] = 
				m_EdgeList.GetAt(m_EdgeList.GetAt(edge1).m_iCorrespIndex).m_iNeighbFacet;
		}
		else pNewFacet->m_pNeighbFacet[1] = NULL;

		m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet =
			m_EdgeList.GetAt(edge1).m_iNeighbFacet =
			m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

		if(bNeighbor)
		{
			for(j=0;j<3;j++)
			{
				if(m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_edge[j] 
				== m_EdgeList.GetAt(edge1).m_iCorrespIndex)
					break;
			}
			m_FacetList.GetAt(pNewFacet->m_pNeighbFacet[1]).m_pNeighbFacet[j] = pNewFacet->m_iIndex;
		}
		m_EdgeList.GetAt(pNewEdge0->m_iIndex).m_iNeighbFacet = 
			m_EdgeList.GetAt(edge1).m_iNeighbFacet = 
			m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iNeighbFacet = pNewFacet->m_iIndex;

	}

	m_EdgeList.GetAt(pNewEdge2->m_iIndex).m_iCorrespIndex = startEdgePos;
	m_EdgeList.GetAt(startEdgePos).m_iCorrespIndex = pNewEdge2->m_iIndex;
	m_FacetList.GetAt(pNewFacet->m_iIndex).m_pNeighbFacet[2] = startFacetPos;
	m_FacetList.GetAt(startFacetPos).m_pNeighbFacet[0] = pNewFacet->m_iIndex;
}


void XTriMesh::XCloud2TriMesh(XCloud & inputCloud)
{
	m_BndBox = inputCloud.GetBBox();
	XCloud2TriMesh_2D(inputCloud);
	CalculateMeshNormal();
	SetVertexesForDraw();

}

void XTriMesh::XCloud2TriMesh(const PNT & newDirection,XCloud & inputCloud)
{
	XCloud newCloud;
	AlignAxisZtoVector(newDirection,inputCloud,newCloud);
	newCloud.UpdateBndBox();

	int * index = new int[inputCloud.GetPntNum()];
	XCloud2TriMesh_2D(newCloud,index);

	PNT tempPnt;
	for(int i=0;i<m_iPointNum;i++)
	{
		inputCloud.GetPoint(index[i],m_PointArray[i].m_Coord);
	}
	m_ViewDirection = newDirection;

	CalculateMeshNormal();
	SetVertexesForDraw();

	m_BndBox = inputCloud.GetBBox();
	delete []index;
}


void XTriMesh::XCloud2TriMesh_2D(XCloud & inputCloud,int * index)
{
	//to construct the first triangle
	if(!m_PointArray.GetSize() != 0)
		m_PointArray.RemoveAll();
	if(!m_EdgeList.GetCount() != 0)
		m_EdgeList.RemoveAll();
	if(!m_FacetList.GetCount() != 0)
		m_FacetList.RemoveAll();

	int num = inputCloud.GetPntNum()+3;
	m_PointArray.SetSize(num);
	m_iPointNum = 0;
	TriPoint initialPnt0,initialPnt1,initialPnt2;
	double disX, disY;

	PNT BndMaxPt, BndMinPt;
	XBndBox box;
	box = inputCloud.GetBBox();
	BndMaxPt = box.GetMax();
	BndMinPt = box.GetMin();
	disX = BndMaxPt[0] - BndMinPt[0];
	disY = BndMaxPt[1] - BndMinPt[1];
	double disT = (disX + disY)/8;

	initialPnt0.m_Coord[0] = (BndMinPt[0] + BndMaxPt[0])/2;
	initialPnt0.m_Coord[1] = BndMinPt[1] + disY + disX/2 + disT/4;
	initialPnt0.m_Coord[2] = (BndMinPt[2]+BndMaxPt[2])/2;
	initialPnt0.m_iIndex = num-1;
	m_PointArray.SetAt(num-1,initialPnt0);

	initialPnt1.m_Coord[0] = BndMinPt[0] - disY - disT/2;
	initialPnt1.m_Coord[1] = BndMinPt[1] - disT/4;
	initialPnt1.m_Coord[2] = (BndMinPt[2]+BndMaxPt[2])/2;
	initialPnt1.m_iIndex = num-2;
	m_PointArray.SetAt(num-2,initialPnt1);

	initialPnt2.m_Coord[0] = BndMaxPt[0] + disY + disT/2;
	initialPnt2.m_Coord[1] = BndMinPt[1] - disT/4;
	initialPnt2.m_Coord[2] = (BndMinPt[2]+BndMaxPt[2])/2;
	initialPnt2.m_iIndex = num-3;
	m_PointArray.SetAt(num-3,initialPnt2);

	v01 = (initialPnt0.m_Coord+initialPnt1.m_Coord)/2;
	v12 = (initialPnt1.m_Coord+initialPnt2.m_Coord)/2;
	v20 = (initialPnt2.m_Coord+initialPnt0.m_Coord)/2;

	TriEdge newEdge0,newEdge1,newEdge2;
	POSITION edgePos0,edgePos1,edgePos2;
	newEdge0.m_iCorrespIndex = NULL;
	newEdge0.m_VertexIndex[0] = num-1;
	newEdge0.m_VertexIndex[1] = num-2;
	m_EdgeList.AddTail(newEdge0);
	edgePos0 = m_EdgeList.GetTailPosition();
	m_EdgeList.GetAt(edgePos0).m_iIndex = edgePos0;

	afxEdge01 = &m_EdgeList.GetTail();

	newEdge1.m_iCorrespIndex = NULL;
	newEdge1.m_VertexIndex[0] = num-2;
	newEdge1.m_VertexIndex[1] = num-3;
	m_EdgeList.AddTail(newEdge1);
	edgePos1 = m_EdgeList.GetTailPosition();
	m_EdgeList.GetAt(edgePos1).m_iIndex = edgePos1;

	afxEdge12 = &m_EdgeList.GetTail();

	newEdge2.m_iCorrespIndex = NULL;
	newEdge2.m_VertexIndex[0] = num-3;
	newEdge2.m_VertexIndex[1] = num-1;
	m_EdgeList.AddTail(newEdge2);
	edgePos2 = m_EdgeList.GetTailPosition();
	m_EdgeList.GetAt(edgePos2).m_iIndex = edgePos2;

	afxEdge20 = &m_EdgeList.GetTail();

	TriFacet newFacet;
	POSITION facetPos;
	newFacet.m_pNeighbFacet[0] =
		newFacet.m_pNeighbFacet[1] =
		newFacet.m_pNeighbFacet[2] = NULL;
	newFacet.m_VertexIndex[0] = num-1;
	newFacet.m_VertexIndex[1] = num-2;
	newFacet.m_VertexIndex[2] = num-3;
	newFacet.m_edge[0] = edgePos0;
	newFacet.m_edge[1] = edgePos1;
	newFacet.m_edge[2] = edgePos2;

	m_FacetList.AddTail(newFacet);
	facetPos = m_FacetList.GetTailPosition();
	m_FacetList.GetAt(facetPos).m_iIndex = 
		m_EdgeList.GetAt(edgePos0).m_iNeighbFacet =
		m_EdgeList.GetAt(edgePos1).m_iNeighbFacet =
		m_EdgeList.GetAt(edgePos2).m_iNeighbFacet = facetPos;

	TriPoint tempPnt;

	//test
	int numX = 256;
	int numY = 256;

	inputCloud.InitialBin2D(numX,numY);

	int step = (num-3)/100;
	if(step==0) step++;

//#ifdef _DEBUG
//	Diag_Progress ProgressDlg;
//	ProgressDlg.Create(Diag_Progress::IDD);
//	ProgressDlg.m_ProgressCtrl.SetPos(0);
//	ProgressDlg.m_ProgressCtrl.SetRange32(0,100);
//	Ti_Timer meshTime;
//	meshTime.TimerBegin();
//#endif

	int i,j,l,m,n=0;

	/*for(i=0,j=0;i<num-3;i++)
	{
	inputCloud.GetPoint(i,tempPnt.m_Coord);	 
	AddInnerPntToTriMesh(tempPnt);
	if(i%step==1)
	{
	j++;
	wsprintf(text,"%d%%",min(j,100));
	ProgressDlg.m_ProgressCtrl.SetPos(j);

	ProgressDlg.GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(text);
	}
	}*/	//test
	if(inputCloud.m_BinArray==NULL)
	{
		for(i=0,j=0;i<num-3;i++)
		{
			inputCloud.GetPoint(i,tempPnt.m_Coord);	
			if(index!=NULL) index[n++] = i;
			AddInnerPntToTriMesh(tempPnt);
//#ifdef _DEBUG
//			if(i%step==1)
//			{
//				j++;
//				wsprintf(text,"%d%%",min(j,100));
//				ProgressDlg.m_ProgressCtrl.SetPos(j);
//				ProgressDlg.GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(text);
//			}
//#endif
		}
	}
	else
	{
		m_iSeedCount = inputCloud.m_iBinNum;

		if(m_pSeed!=NULL) 
			{
			delete []m_pSeed;
			m_pSeed = NULL;
			}
		if(m_pPnt2Seed!=NULL) 
			{
			delete []m_pPnt2Seed;
			m_pPnt2Seed = NULL;
			}
		m_pSeed = new POSITION[m_iSeedCount];
		m_pPnt2Seed = new int[m_iSeedCount];

		int * noSeedIndex = new int[m_iSeedCount];
		int noCount = 0;

		POSITION pos;

		l = m = 0;
		for(i=0;i<m_iSeedCount;i++)
		{
			m_pSeed[i] = NULL;
			if(!inputCloud.m_BinArray[i].m_InnerPoints.IsEmpty())
			{
				m_pPnt2Seed[l] = i;
				
				l++;
				if(index!=NULL) 
				{
					index[n] = inputCloud.m_BinArray[i].m_InnerPoints.RemoveHead();
					inputCloud.GetPoint(index[n],tempPnt.m_Coord);
					n++;
				}
				else
				{
					inputCloud.GetPoint(inputCloud.m_BinArray[i].m_InnerPoints.RemoveHead(),tempPnt.m_Coord);
				}
				if(!AddInnerPntToTriMesh(tempPnt))
				{	
					/*while(!inputCloud.m_BinArray[i].m_InnerPoints.IsEmpty())
					{
						if(index!=NULL) 
						{
							n--;
							index[n] = inputCloud.m_BinArray[i].m_InnerPoints.RemoveHead();
							inputCloud.GetPoint(index[n],tempPnt.m_Coord);
							n++;
						}
						else
						{
							inputCloud.GetPoint(inputCloud.m_BinArray[i].m_InnerPoints.RemoveHead(),tempPnt.m_Coord);
						}
						if(AddInnerPntToTriMesh(tempPnt)) break;
					}*/
					//m_pSeed[i] = m_FacetList.GetTailPosition(); 
					noSeedIndex[noCount++] = i;
				}

//#ifdef _DEBUG
//				if(l%step==1)
//				{
//					m++;
//					wsprintf(text,"%d%%",min(m,100));
//					ProgressDlg.m_ProgressCtrl.SetPos(m);
//					ProgressDlg.GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(text);
//				}
//#endif
			}
		}
		m_iPnt2SeedCount = l;

		pos = m_FacetList.GetHeadPosition();
		TriFacet * pFacet;
		while(pos)
		{
			pFacet = &m_FacetList.GetNext(pos);
			for(i=0;i<3;i++)
			{
				if(pFacet->m_VertexIndex[i]>=m_iPnt2SeedCount) continue;
				if(m_pSeed[m_pPnt2Seed[pFacet->m_VertexIndex[i]]]==NULL) 
				{
					m_pSeed[m_pPnt2Seed[pFacet->m_VertexIndex[i]]] = pFacet->m_iIndex;
					break;
				}
			}
		}

		for(i=0;i<noCount;i++)
		{
			m_pSeed[noSeedIndex[i]] = m_pSeed[m_pPnt2Seed[0]];
		}

		for(m_iSeedIndex=0;m_iSeedIndex<m_iSeedCount;m_iSeedIndex++)
		{
			pos = inputCloud.m_BinArray[m_iSeedIndex].m_InnerPoints.GetHeadPosition();
			for(;pos;l++)
			{
				if(index!=NULL) 
				{
					index[n] = inputCloud.m_BinArray[m_iSeedIndex].m_InnerPoints.GetNext(pos);
					inputCloud.GetPoint(index[n],tempPnt.m_Coord);
					n++;
				}
				else
				{
					inputCloud.GetPoint(inputCloud.m_BinArray[m_iSeedIndex].m_InnerPoints.GetNext(pos),tempPnt.m_Coord);	 
				}
				AddInnerPntToTriMeshSeed(tempPnt);
//#ifdef _DEBUG
//				if(l%step==1)
//				{
//					m++;
//					wsprintf(text,"%d%%",min(m,100));
//					ProgressDlg.m_ProgressCtrl.SetPos(m);
//					ProgressDlg.GetDlgItem(IDC_PROGRESS_STATIC)->SetWindowText(text);
//				}
//#endif
			}
		}
			delete []noSeedIndex;
	}
	m_iSeedCount = -1;
	m_iPnt2SeedCount = -1;
	m_iSeedIndex = -1;
	if(m_pSeed != NULL)
		{
		delete []m_pSeed ;
		m_pSeed = NULL;
		}
	if(m_pPnt2Seed != NULL)
		{
		delete []m_pPnt2Seed;
		m_pPnt2Seed = NULL;
		}


	m_iPointNum +=3;
	DeletePnt(num-1,true);
	DeletePnt(num-2,true);
	DeletePnt(num-3,true);
	if(inputCloud.m_BinArray!=NULL) 
	{
		delete []inputCloud.m_BinArray;
		inputCloud.m_BinArray = NULL;
	}

//#ifdef _DEBUG
//	//ProgressDlg.OnCancel();
//	meshTime.TimerStop();
//	SYSTEMTIME timeSpan = meshTime.GetResultTime();
//	int milliSpan = timeSpan.wMilliseconds;
//	int cost_time = (timeSpan.wHour*60 + timeSpan.wMinute)*60 + timeSpan.wSecond;
//	CString str;
//	str.Format("triangulation complete in %d.%d seconds!",cost_time,milliSpan);
//	AfxMessageBox(str);
//#endif

}

void XTriMesh::XCloud2TriMesh_Hybrid2D(XCloud & inputCloud,int splitNum)
{
	if(splitNum<1) return;
	if(splitNum==1) 
	{
		XCloud2TriMesh_2D(inputCloud);
		return;
	}
	XTriMesh * pSplitMesh = new XTriMesh[splitNum-1];
	XTriMesh meshB;
	XCloud splitCld;
	inputCloud.InitialBin2D(splitNum);
	int i,j;
	inputCloud.CloudInBin2D(0,splitCld);
	splitCld.UpdateBndBox();
	XCloud2TriMesh_2D(splitCld);
	for(i=1;i<splitNum;i++)
	{
		inputCloud.CloudInBin2D(i,splitCld);
		//splitCld.m_bObjChanged = true;
		//splitCld.UpdateBndBox();
		meshB.Clear();
		meshB.XCloud2TriMesh_2D(splitCld);
		AddWithJoin(meshB);
	}
	for(i=1;i<splitNum;i++)
	{
		inputCloud.CloudInBin2D(i*splitNum,splitCld);
		splitCld.SetObjState(true);
		pSplitMesh[i-1].XCloud2TriMesh_2D(splitCld);
		for(j=1;j<splitNum;j++)
		{
			inputCloud.CloudInBin2D(i*splitNum+j,splitCld);
			//splitCld.m_bObjChanged = true;
			//splitCld.UpdateBndBox();
			meshB.Clear();
			meshB.XCloud2TriMesh_2D(splitCld);
			pSplitMesh[i-1].AddWithJoin(meshB);
		}
		AddWithJoin(pSplitMesh[i-1]);
	}
	delete []pSplitMesh;
}


//void XTriMesh::Draw(void)
//{

//	if(!m_bSetVertexesDone) return;
//	if(!m_bShow) return;
//	int i;
//	if(m_bSelected)
//		{
//		this->DrawBndBox();
//		}
//	ApplyColor();
//	int facetNum = m_FacetList.GetCount();
//	glFrontFace(GL_CCW );
//	glLineWidth(0.1f); 
//
//	if(m_DispType== 0)
//	{
//		glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);
//		glBegin(GL_TRIANGLES);
//		for(i=0;i<facetNum;i++)
//		{
//			glNormal3f(m_pDrawX[i],m_pDrawY[i],m_pDrawZ[i]);
//			glVertex3f(m_pDrawX[m_pDrawIndex[3*i]],m_pDrawY[m_pDrawIndex[3*i]],m_pDrawZ[m_pDrawIndex[3*i]]);
//			glVertex3f(m_pDrawX[m_pDrawIndex[3*i+1]],m_pDrawY[m_pDrawIndex[3*i+1]],m_pDrawZ[m_pDrawIndex[3*i+1]]);
//			glVertex3f(m_pDrawX[m_pDrawIndex[3*i+2]],m_pDrawY[m_pDrawIndex[3*i+2]],m_pDrawZ[m_pDrawIndex[3*i+2]]);
//		}
//		glEnd();
//	}
//
//	if(m_DispType== 1)
//	{
//		glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
//		glEnable(GL_COLOR_MATERIAL);
//		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
//		GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
//		GLfloat mat_shininess[] = { 7.0f };
//	//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
//		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);	
//		glEnable(GL_LIGHTING);
//		glBegin(GL_TRIANGLES);
//		for(i=0;i<facetNum;i++)
//		{
//			glNormal3f(m_pDrawX[i],m_pDrawY[i],m_pDrawZ[i]);
//			glVertex3f(m_pDrawX[m_pDrawIndex[3*i]],
//				m_pDrawY[m_pDrawIndex[3*i]],
//				m_pDrawZ[m_pDrawIndex[3*i]]);
//			glVertex3f(m_pDrawX[m_pDrawIndex[3*i+1]],
//				m_pDrawY[m_pDrawIndex[3*i+1]],
//				m_pDrawZ[m_pDrawIndex[3*i+1]]);
//			glVertex3f(m_pDrawX[m_pDrawIndex[3*i+2]],
//				m_pDrawY[m_pDrawIndex[3*i+2]],
//				m_pDrawZ[m_pDrawIndex[3*i+2]]);
//		}
//		glEnd();
//		glDisable(GL_LIGHTING);
//	}
//
//	if(m_DispType==4)
//	{
//		//test
//		glLineWidth(2.f); 
//		glColor3f(1, 1, 0);
//		int edgeNum = m_OuterEdge.GetCount();
//		TriEdge * tempEdge;
//		glBegin(GL_LINES);
//		for(POSITION pos=m_OuterEdge.GetHeadPosition();pos;)
//		{
//			tempEdge = &m_EdgeList.GetAt(m_OuterEdge.GetNext(pos));
//			glVertex3f(m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[0], m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[1],m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[2]);
//			glVertex3f(m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[0], m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[1],m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[2]);
//		}	
//		glEnd();
//
//		edgeNum = m_InnerEdge.GetCount();
//		glBegin(GL_LINES);
//		for( pos=m_InnerEdge.GetHeadPosition();pos;)
//		{
//			tempEdge = &m_EdgeList.GetAt(m_InnerEdge.GetNext(pos));
//			glVertex3f(m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[0], m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[1],m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[2]);
//			glVertex3f(m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[0], m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[1],m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[2]);
//		}	
//		glEnd();
//
//		//test
//		glBegin(GL_LINES);
//		for( pos=m_EdgeList.GetHeadPosition();pos;)
//		{
//			tempEdge = &m_EdgeList.GetNext(pos);
//			if(tempEdge->m_iEdgeType==6)
//			{
//				glVertex3f(m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[0], m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[1],m_PointArray[tempEdge->m_VertexIndex[0]].m_Coord[2]);
//				glVertex3f(m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[0], m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[1],m_PointArray[tempEdge->m_VertexIndex[1]].m_Coord[2]);
//			}
//		}	
//		glEnd();
//	}
//}
//
void XTriMesh::TriMesh2XCloud(XCloud & outputCloud,int type)
{
	int i;
	POSITION pos;
	TriFacet * pFacet = NULL;

	outputCloud.InitObject();
	if(type==0)
	{
		outputCloud.SetCapacity(m_FacetList.GetCount());
		pos = m_FacetList.GetHeadPosition();	
		while(pos)
		{
			pFacet = &m_FacetList.GetNext(pos);
			outputCloud.AddPoint((m_PointArray[pFacet->m_VertexIndex[0]].m_Coord+m_PointArray[pFacet->m_VertexIndex[1]].m_Coord+m_PointArray[pFacet->m_VertexIndex[2]].m_Coord)/3);
		}
	}
	else if(type==1)
	{
		outputCloud.SetCapacity(m_iPointNum);
		for(i=0;i<m_iPointNum;i++)
		{
			outputCloud.AddPoint(m_PointArray[i].m_Coord);
		}
	}
}

void XTriMesh::TriMesh2XMesh(XMesh & outputMesh)
{
	int num = m_FacetList.GetCount();
	XFacet newFacet;
	TriFacet * pTriFacet;
	int i=0,j;
	for(POSITION pos=m_FacetList.GetHeadPosition();pos;i++)
	{	
		pTriFacet = &m_FacetList.GetNext(pos);
		for(j=0;j<3;j++)
		{
			newFacet.m_Vertex[j] = m_PointArray[pTriFacet->m_VertexIndex[j]].m_Coord;
		}
		newFacet.m_Norm = pTriFacet->m_Normal;
		outputMesh.AddFacet(newFacet);
	}
}
void XTriMesh::TriMeshProjection(const PNT& projdir, XTriMesh & outputMesh)
	{
	int num = m_FacetList.GetCount();
	outputMesh.Clear();
	outputMesh = *this;
	TriFacet * pTriFacet;
	int i=0,j;
	for(POSITION pos=outputMesh.m_FacetList.GetHeadPosition();pos;i++)
		{	
		pTriFacet = &outputMesh.m_FacetList.GetNext(pos);
		for(j=0;j<3;j++)
			{
			outputMesh.m_PointArray[pTriFacet->m_VertexIndex[j]].m_Coord -= projdir*(outputMesh.m_PointArray[pTriFacet->m_VertexIndex[j]].m_Coord*projdir);
			}
		}
	}


bool XTriMesh::DeletePnt(
			 int index,                  // point index
			 bool bDelete,
			 int delTag
			 )
{
	if(index<0||index>m_iPointNum)
		return false;

	//find neighbor facets
	int i,j,k;
	CPosList neighborFacet;
	TriFacet * pInitialTri = NULL;
	TriFacet * pTempTri = NULL;
	TriFacet * pTempTri1 = NULL;

	int facetCount = m_FacetList.GetCount();
	pTempTri = &m_FacetList.GetHead();
	POSITION pos = m_FacetList.GetHeadPosition();
	while(pos)
	{
		pTempTri = &m_FacetList.GetNext(pos);
		if(pTempTri->m_VertexIndex[0]==index||pTempTri->m_VertexIndex[1]==index||pTempTri->m_VertexIndex[2]==index)
		{
			pInitialTri = pTempTri;
			break;
		}
	}

	if(pInitialTri==NULL)
		return false;

	pInitialTri->m_iFacetType = 9;
	neighborFacet.AddTail(pInitialTri->m_iIndex);
	int neighborFacetCount = 1;
	for(i=0;i<neighborFacetCount;i++)
	{
		pTempTri = &m_FacetList.GetAt(neighborFacet.GetAt(neighborFacet.FindIndex(i)));
		for(j=0;j<3;j++)
		{
			if(pTempTri->m_pNeighbFacet[j]==NULL) continue;

			if(m_FacetList.GetAt(pTempTri->m_pNeighbFacet[j]).m_iFacetType!=9)
			{
				pTempTri1 = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[j]);
				for(k=0;k<3;k++)
				{
					if(pTempTri1->m_VertexIndex[k]==index)
					{
						neighborFacet.AddTail(pTempTri1->m_iIndex);
						pTempTri1->m_iFacetType = 9;
						neighborFacetCount++;
					}
				}
			}
		}
	}

	//find edges
	//delete facets
	CPosList neighborEdge;
	for(i=0;i<neighborFacetCount;i++)
	{
		pTempTri = &m_FacetList.GetAt(neighborFacet.GetHead());
		for(j=0;j<3;j++)
		{
			neighborEdge.AddTail(pTempTri->m_edge[j]);
		}
		neighborFacet.RemoveHead();
		m_FacetList.RemoveAt(pTempTri->m_iIndex);
	}

	//find keep edges and delete other edges
	bool bNoHole = false;
	int keepEdgeCount = neighborEdge.GetCount();
	POSITION edgePos,edgePos1,neighborEdgePos,neighborEdgePos1;
	neighborEdgePos = neighborEdge.GetHeadPosition();

	for(i=0;i<keepEdgeCount;i++)
	{
		neighborEdgePos = neighborEdge.FindIndex(i);
		edgePos = neighborEdge.GetAt(neighborEdgePos);
		edgePos1 = m_EdgeList.GetAt(neighborEdge.GetAt(neighborEdgePos)).m_iCorrespIndex;
		if(edgePos1!=NULL)
		{		
			neighborEdgePos1 = neighborEdge.Find(edgePos1);
			if(neighborEdgePos1!=NULL)
			{
				m_EdgeList.RemoveAt(edgePos);
				m_EdgeList.RemoveAt(edgePos1);
				neighborEdge.RemoveAt(neighborEdgePos);
				neighborEdge.RemoveAt(neighborEdgePos1);
				keepEdgeCount -=2;
				i--;//important revise
			}
		}
		else
		{
			m_EdgeList.RemoveAt(edgePos);
			neighborEdge.RemoveAt(neighborEdgePos);
			keepEdgeCount -= 1;
			bNoHole = true;
			i--;
		}
	}

	//delete edges and edge&facet links
	if(bNoHole)
	{
		TriEdge * pTempEdge;
		for(i=0;i<keepEdgeCount;i++)
		{
			pTempEdge = &m_EdgeList.GetAt(neighborEdge.GetHead());
			m_EdgeList.GetAt(pTempEdge->m_iCorrespIndex).m_iCorrespIndex = NULL;
			pTempTri = &m_FacetList.GetAt(m_EdgeList.GetAt(pTempEdge->m_iCorrespIndex).m_iNeighbFacet);
			for(j=0;j<3;j++)
			{
				if(pTempTri->m_edge[j]==pTempEdge->m_iCorrespIndex)
					break;
			}
			pTempTri->m_pNeighbFacet[j] = NULL;
			neighborEdge.RemoveHead();
			m_EdgeList.RemoveAt(pTempEdge->m_iIndex);
		}
	}
	else
	{
		FillPolygonHole(neighborEdge);
	}
	if(bDelete)
	{
		m_PointArray.RemoveAt(index);
		m_iPointNum--;
	}
	else
	{
		//default -1 for delete Point , others for tag point;
		m_PointArray[index].m_iPntStat = delTag;
	}
	return true;
}


bool XTriMesh::DeletePntForIntegration(int index,PNT & viewDir,int delTag)
{
	if(index<0||index>m_iPointNum)
		return false;

	//find neighbor facets
	int i,j,k;
	CPosList neighborFacet;
	TriFacet * pInitialTri = NULL;
	TriFacet * pTempTri = NULL;
	TriFacet * pTempTri1 = NULL;

	int facetCount = m_FacetList.GetCount();
	pTempTri = &m_FacetList.GetHead();
	POSITION pos = m_FacetList.GetHeadPosition();
	while(pos)
	{
		pTempTri = &m_FacetList.GetNext(pos);
		if(pTempTri->m_VertexIndex[0]==index||pTempTri->m_VertexIndex[1]==index||pTempTri->m_VertexIndex[2]==index)
		{
			pInitialTri = pTempTri;
			break;
		}
	}

	if(pInitialTri==NULL)
		return false;

	pInitialTri->m_iFacetType = 9;
	neighborFacet.AddTail(pInitialTri->m_iIndex);
	int neighborFacetCount = 1;
	for(i=0;i<neighborFacetCount;i++)
	{
		pTempTri = &m_FacetList.GetAt(neighborFacet.GetAt(neighborFacet.FindIndex(i)));
		for(j=0;j<3;j++)
		{
			if(pTempTri->m_pNeighbFacet[j]==NULL) continue;

			if(m_FacetList.GetAt(pTempTri->m_pNeighbFacet[j]).m_iFacetType!=9)
			{
				pTempTri1 = &m_FacetList.GetAt(pTempTri->m_pNeighbFacet[j]);
				for(k=0;k<3;k++)
				{
					if(pTempTri1->m_VertexIndex[k]==index)
					{
						neighborFacet.AddTail(pTempTri1->m_iIndex);
						pTempTri1->m_iFacetType = 9;
						neighborFacetCount++;
					}
				}
			}
		}
	}

	//find edges
	//delete facets
	viewDir = PNT(0,0,0);
	CPosList neighborEdge;
	for(i=0;i<neighborFacetCount;i++)
	{
		pTempTri = &m_FacetList.GetAt(neighborFacet.GetHead());
		for(j=0;j<3;j++)
		{
			neighborEdge.AddTail(pTempTri->m_edge[j]);
		}
		neighborFacet.RemoveHead();
		viewDir += pTempTri->m_Normal;
		m_FacetList.RemoveAt(pTempTri->m_iIndex);
	}

	//find keep edges and delete other edges
	int keepEdgeCount = neighborEdge.GetCount();
	POSITION edgePos,edgePos1,neighborEdgePos,neighborEdgePos1;
	neighborEdgePos = neighborEdge.GetHeadPosition();

	for(i=0;i<keepEdgeCount;i++)
	{
		neighborEdgePos = neighborEdge.FindIndex(i);
		edgePos = neighborEdge.GetAt(neighborEdgePos);
		edgePos1 = m_EdgeList.GetAt(neighborEdge.GetAt(neighborEdgePos)).m_iCorrespIndex;
		if(edgePos1!=NULL)
		{		
			neighborEdgePos1 = neighborEdge.Find(edgePos1);
			if(neighborEdgePos1!=NULL)
			{
				m_EdgeList.RemoveAt(edgePos);
				m_EdgeList.RemoveAt(edgePos1);
				neighborEdge.RemoveAt(neighborEdgePos);
				neighborEdge.RemoveAt(neighborEdgePos1);
				keepEdgeCount -=2;
				i--;//important revise
			}
		}
		else
		{
			m_EdgeList.RemoveAt(edgePos);
			neighborEdge.RemoveAt(neighborEdgePos);
			keepEdgeCount -= 1;
			i--;
		}
	}

	TriEdge * pTempEdge;
	for(i=0;i<keepEdgeCount;i++)
	{
		pTempEdge = &m_EdgeList.GetAt(neighborEdge.GetHead());
		m_EdgeList.GetAt(pTempEdge->m_iCorrespIndex).m_iCorrespIndex = NULL;
		pTempTri = &m_FacetList.GetAt(m_EdgeList.GetAt(pTempEdge->m_iCorrespIndex).m_iNeighbFacet);
		for(j=0;j<3;j++)
		{
			if(pTempTri->m_edge[j]==pTempEdge->m_iCorrespIndex)
				break;
		}
		pTempTri->m_pNeighbFacet[j] = NULL;
		neighborEdge.RemoveHead();
		m_EdgeList.RemoveAt(pTempEdge->m_iIndex);
	}

	m_PointArray[index].m_iPntStat = delTag;

	return true;
}

void XTriMesh::FillPolygonHole(CPosList& holeEdgeList)
{
	//find three consecutive vertex
	int i,j;
	CIntList pointLoop;
	CPosList edgeLoop,testEdges;
	TriFacet * testFacet;
	InitialPolygonLoop(holeEdgeList,pointLoop,edgeLoop);

	int count = pointLoop.GetCount()-2;
	for(i=0;i<count;i++)
	{
		EarCutting(pointLoop,edgeLoop);
		testFacet = &m_FacetList.GetTail();
		for(j=0;j<3;j++)
		{
			if(m_EdgeList.GetAt(testFacet->m_edge[j]).m_iEdgeType!=2&&m_EdgeList.GetAt(testFacet->m_edge[j]).m_iCorrespIndex!=NULL)
			{
				if(m_EdgeList.GetAt(m_EdgeList.GetAt(testFacet->m_edge[j]).m_iCorrespIndex).m_iEdgeType!=2)
				{
					m_EdgeList.GetAt(testFacet->m_edge[j]).m_iEdgeType = 2;
					testEdges.AddTail(testFacet->m_edge[j]);
				}
			}
		}
	}

	POSITION pos = testEdges.GetHeadPosition();
	POSITION oldPos,edgePos;
	POSITION newTestEdge[4] = {NULL};
	TriEdge * pEdge = NULL;
	while(pos)
	{
		//could be revised
		oldPos = pos;
		edgePos = testEdges.GetNext(pos);
		m_EdgeList.GetAt(edgePos).m_iEdgeType = 0;

		if(SwapEdgeWithTest(edgePos,newTestEdge))
		{
			for(j=0;j<4;j++)
			{
				pEdge = & m_EdgeList.GetAt(newTestEdge[j]);
				if(pEdge->m_iEdgeType!=2&&pEdge->m_iCorrespIndex!=NULL)
				{
					if(m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iEdgeType!=2)
					{
						pEdge->m_iEdgeType=2;
						testEdges.AddTail(newTestEdge[j]);
						if(pos==NULL)
						{
							testEdges.GetNext(oldPos);
							pos = oldPos;
						}
					}
				}
			}
		}
	}
}

void XTriMesh::FillWithoutInitial(CPosList& edgeLoop)
{
	//find three consecutive vertex
	int i,j;
	CIntList pointLoop;
	CPosList testEdges;
	TriFacet * testFacet;
	int num = edgeLoop.GetCount();
	for(i=0;i<num;i++)
	{
		pointLoop.AddTail(m_EdgeList.GetAt(edgeLoop.GetAt(edgeLoop.FindIndex(i))).m_VertexIndex[0]);
	}

	int count = pointLoop.GetCount()-2;
	for(i=0;i<count;i++)
	{
		EarCutting(pointLoop,edgeLoop);
		testFacet = &m_FacetList.GetTail();
		//testFacet->m_iFacetType = 3;

		for(j=0;j<3;j++)
		{
			if(m_EdgeList.GetAt(testFacet->m_edge[j]).m_iEdgeType!=2&&m_EdgeList.GetAt(testFacet->m_edge[j]).m_iCorrespIndex!=NULL)
			{
				if(m_EdgeList.GetAt(m_EdgeList.GetAt(testFacet->m_edge[j]).m_iCorrespIndex).m_iEdgeType!=2)
				{
					m_EdgeList.GetAt(testFacet->m_edge[j]).m_iEdgeType = 2;
					testEdges.AddTail(testFacet->m_edge[j]);
				}
			}
		}
	}

	POSITION pos = testEdges.GetHeadPosition();
	POSITION oldPos,edgePos;
	POSITION newTestEdge[4] = {NULL};
	TriEdge * pEdge = NULL;
	while(pos)
	{
		//could be revised
		oldPos = pos;
		edgePos = testEdges.GetNext(pos);
		m_EdgeList.GetAt(edgePos).m_iEdgeType = 0;

		if(SwapEdgeWithTest(edgePos,newTestEdge))
		{
			for(j=0;j<4;j++)
			{
				pEdge = & m_EdgeList.GetAt(newTestEdge[j]);
				if(pEdge->m_iEdgeType!=2&&pEdge->m_iCorrespIndex!=NULL)
				{
					if(m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iEdgeType!=2)
					{
						pEdge->m_iEdgeType=2;
						testEdges.AddTail(newTestEdge[j]);
						if(pos==NULL)
						{
							testEdges.GetNext(oldPos);
							pos = oldPos;
						}
					}
				}
			}
		}
	}
}

void XTriMesh::FillForIntegration(CPosList& edgeLoop)
{
	//find three consecutive vertex
	int i,j;
	CIntList pointLoop;
	CPosList testEdges;
	TriFacet * testFacet;
	int num = edgeLoop.GetCount();
	for(i=0;i<num;i++)
	{
		pointLoop.AddTail(m_EdgeList.GetAt(edgeLoop.GetAt(edgeLoop.FindIndex(i))).m_VertexIndex[0]);
	}

	int count = pointLoop.GetCount()-2;
	for(i=0;i<count;i++)
	{
		EarCutting(pointLoop,edgeLoop);
		testFacet = &m_FacetList.GetTail();
		testFacet->m_iFacetType = 3;

		for(j=0;j<3;j++)
		{
			if(m_EdgeList.GetAt(testFacet->m_edge[j]).m_iEdgeType!=2&&m_EdgeList.GetAt(testFacet->m_edge[j]).m_iCorrespIndex!=NULL)
			{
				if(m_EdgeList.GetAt(m_EdgeList.GetAt(testFacet->m_edge[j]).m_iCorrespIndex).m_iEdgeType!=2)
				{
					m_EdgeList.GetAt(testFacet->m_edge[j]).m_iEdgeType = 2;
					testEdges.AddTail(testFacet->m_edge[j]);
				}
			}
		}
	}


	POSITION pos = testEdges.GetHeadPosition();
	POSITION oldPos,edgePos;
	POSITION newTestEdge[4] = {NULL};
	TriEdge * pEdge = NULL;
	while(pos)
	{
		//could be revised
		oldPos = pos;
		edgePos = testEdges.GetNext(pos);
		m_EdgeList.GetAt(edgePos).m_iEdgeType = 0;

		if(SwapEdgeWithTest(edgePos,newTestEdge))
		{
			for(j=0;j<4;j++)
			{
				pEdge = & m_EdgeList.GetAt(newTestEdge[j]);
				if(pEdge->m_iEdgeType!=2&&pEdge->m_iCorrespIndex!=NULL)
				{
					if(m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iEdgeType!=2
						&&m_FacetList.GetAt(m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iNeighbFacet).m_iFacetType==3
						&&m_FacetList.GetAt(pEdge->m_iNeighbFacet).m_iFacetType==3)
					{
						pEdge->m_iEdgeType=2;
						testEdges.AddTail(newTestEdge[j]);
						if(pos==NULL)
						{
							testEdges.GetNext(oldPos);
							pos = oldPos;
						}
					}
				}
			}
		}
	}
}

bool XTriMesh::InitialPolygonLoop(CPosList & edgeList,CIntList & pointLoop,CPosList & edgeLoop)
{
	if(edgeList.GetCount()<1) return false;

	POSITION pos,oldPos;
	pos = edgeList.GetHeadPosition();

	POSITION edgePos = edgeList.GetAt(pos);
	int startPnt = m_EdgeList.GetAt(edgePos).m_VertexIndex[0];
	int endPnt = m_EdgeList.GetAt(edgePos).m_VertexIndex[1];

	edgeList.RemoveHead();
	TriEdge * pEdge;

	pointLoop.RemoveAll();
	pointLoop.AddTail(startPnt);
	pointLoop.AddTail(endPnt);
	edgeLoop.RemoveAll();
	edgeLoop.AddTail(edgePos);

	int count = edgeList.GetCount();

	bool bFlag;
	while(endPnt!=startPnt&&count>0)
	{
		bFlag = false;
		for(pos=edgeList.GetHeadPosition();pos;)
		{
			oldPos = pos;
			pEdge = &m_EdgeList.GetAt(edgeList.GetNext(pos));
			if(pEdge->m_VertexIndex[0]==endPnt)
			{
				//add code here
				count--;
				edgeList.RemoveAt(oldPos);
				edgeLoop.AddTail(pEdge->m_iIndex);
				endPnt = pEdge->m_VertexIndex[1];
				pointLoop.AddTail(endPnt);
				bFlag = true;
				break;
			}
			else if(pEdge->m_VertexIndex[1]==startPnt)
			{
				count--;
				edgeList.RemoveAt(oldPos);
				edgeLoop.AddHead(pEdge->m_iIndex);
				startPnt = pEdge->m_VertexIndex[0];
				pointLoop.AddHead(startPnt);
				bFlag = true;
				break;
			}
		}
		if(!bFlag) break;
	}
	if(endPnt==startPnt)
	{
		pointLoop.RemoveTail();
		return true;
	}
	return false;
	//while(pointLoop.GetTail()!=startPnt)
	//{
	//	pos = edgeList.GetHeadPosition();
	//	for(int i=0;i<count;i++)
	//	{
	//		pEdge = &m_EdgeList.GetAt(edgeList.GetNext(pos));
	//		if(pEdge->m_VertexIndex[0]==pointLoop.GetTail())
	//		{
	//			edgeList.RemoveAt(edgeList.FindIndex(i));
	//			count--;
	//			i--;
	//			//add code here
	//			edgeLoop.AddTail(pEdge->m_iIndex);
	//			pointLoop.AddTail(pEdge->m_VertexIndex[1]);
	//			break;
	//		}
	//	}
	//}
	//pointLoop.RemoveTail();
	//return true;
}


void XTriMesh::InitialPolygonLoop(CPosList & edgeList,CPosList & edgeLoop)
{
	if(edgeList.GetCount()<1) return;

	POSITION pos,oldPos;
	pos = edgeList.GetHeadPosition();

	POSITION edgePos = edgeList.GetAt(pos);
	int startPnt = m_EdgeList.GetAt(edgePos).m_VertexIndex[0];
	int endPnt = m_EdgeList.GetAt(edgePos).m_VertexIndex[1];

	edgeList.RemoveHead();
	TriEdge * pEdge;

	edgeLoop.RemoveAll();
	edgeLoop.AddTail(edgePos);

	while(endPnt!=startPnt)
	{
		for(pos=edgeList.GetHeadPosition();pos;)
		{

			oldPos = pos;
			pEdge = &m_EdgeList.GetAt(edgeList.GetNext(pos));
			if(pEdge->m_VertexIndex[0]==endPnt)
			{
				//add code here
				edgeList.RemoveAt(oldPos);
				edgeLoop.AddTail(pEdge->m_iIndex);
				endPnt = pEdge->m_VertexIndex[1];
				break;
			}
			else if(pEdge->m_VertexIndex[1]==startPnt)
			{
				edgeList.RemoveAt(oldPos);
				edgeLoop.AddHead(pEdge->m_iIndex);
				startPnt = pEdge->m_VertexIndex[0];
				break;
			}
		}
	}
}

void XTriMesh::EarCutting(CIntList & pointLoop,CPosList & edgeLoop)
{
	static int step(1);
	int i,j;
	int count = pointLoop.GetCount();
	TriFacet newFacet,testFacet;
	TriEdge newEdge,newCorrEdge;
	TriEdge * pEdge0;
	TriEdge * pEdge1;
	POSITION newEdgePos,newCorrEdgePos,newFacetPos;
	bool bRightTri;
	if(step%count==0) step = count;
	
	//int m;
	//bool bInnerPnt;
	for(i=step;i<count+step;i++)
	{
		bRightTri = true;
		//bInnerPnt = false;
		newFacet.m_VertexIndex[0] = pointLoop.GetAt(pointLoop.FindIndex((i-1)%count));
		newFacet.m_VertexIndex[1] = pointLoop.GetAt(pointLoop.FindIndex(i%count));
		newFacet.m_VertexIndex[2] = pointLoop.GetAt(pointLoop.FindIndex((i+1)%count));
		if(!IsTriangleCCW(newFacet))
		{
			continue;
		}
		else if(CalculateMinAngle(newFacet)<error) continue;

		/*m = (i+2)%count;
		for(j=m;j<m+count-3;j++)
		{
			if(IsPntInFacet(m_PointArray[pointLoop.GetAt(pointLoop.FindIndex(j%count))],newFacet))
			{
				bInnerPnt = true;
				break;
			}
		}	
		if(bInnerPnt) 	continue;
		*/

		if(count==4)
		{
			testFacet.m_VertexIndex[0] = pointLoop.GetAt(pointLoop.FindIndex((i+1)%count));
			testFacet.m_VertexIndex[1] = pointLoop.GetAt(pointLoop.FindIndex((i+2)%count));
			testFacet.m_VertexIndex[2] = pointLoop.GetAt(pointLoop.FindIndex((i+3)%count));
			if(!IsTriangleCCW(testFacet))
			{
				continue;
			}
			else if(CalculateMinAngle(testFacet)<error) continue;
		}
		for(j=i+2;j<count+i-1;j++)
		{
			if(IsPntInFacet(m_PointArray[pointLoop.GetAt(pointLoop.FindIndex(j%count))],newFacet))
			{
				bRightTri = false;
			}
		}
		if(bRightTri) break;
	}

	if(bRightTri)
	{
		step++;
		if(count==3)
		{
			pEdge0 = &m_EdgeList.GetAt(edgeLoop.GetAt(edgeLoop.FindIndex((i-1)%count)));
			pEdge1 = &m_EdgeList.GetAt(edgeLoop.GetAt(edgeLoop.FindIndex(i%count)));
			TriEdge * pEdge2 = &m_EdgeList.GetAt(edgeLoop.GetAt(edgeLoop.FindIndex((i+1)%count)));

			newFacet.m_edge[0] = pEdge0->m_iIndex;
			newFacet.m_edge[1] = pEdge1->m_iIndex;
			newFacet.m_edge[2] = pEdge2->m_iIndex;

			bool bNeighbor0 = false;
			if(pEdge0->m_iCorrespIndex != NULL) 
			{
				bNeighbor0 = true;
				newFacet.m_pNeighbFacet[0] = m_EdgeList.GetAt(pEdge0->m_iCorrespIndex).m_iNeighbFacet;
			}
			else newFacet.m_pNeighbFacet[0] = NULL;

			bool bNeighbor1 = false;
			if(pEdge1->m_iCorrespIndex != NULL) 
			{
				bNeighbor1 = true;
				newFacet.m_pNeighbFacet[1] = m_EdgeList.GetAt(pEdge1->m_iCorrespIndex).m_iNeighbFacet;
			}
			else newFacet.m_pNeighbFacet[1] = NULL;

			bool bNeighbor2 = false;
			if(pEdge2->m_iCorrespIndex != NULL) 
			{
				bNeighbor2 = true;
				newFacet.m_pNeighbFacet[2] = m_EdgeList.GetAt(pEdge2->m_iCorrespIndex).m_iNeighbFacet;
			}
			else newFacet.m_pNeighbFacet[2] = NULL;

			m_FacetList.AddTail(newFacet);
			newFacetPos = m_FacetList.GetTailPosition();
			m_FacetList.GetTail().m_iIndex = 
				pEdge0->m_iNeighbFacet =
				pEdge1->m_iNeighbFacet =
				pEdge2->m_iNeighbFacet = newFacetPos;

			if(bNeighbor0)
			{
				for(j=0;j<3;j++)
				{
					if(m_FacetList.GetAt(newFacet.m_pNeighbFacet[0]).m_edge[j] == pEdge0->m_iCorrespIndex)
						break;
				}
				m_FacetList.GetAt(newFacet.m_pNeighbFacet[0]).m_pNeighbFacet[j] = newFacetPos;
			}
			if(bNeighbor1)
			{
				for(j=0;j<3;j++)
				{
					if(m_FacetList.GetAt(newFacet.m_pNeighbFacet[1]).m_edge[j] == pEdge1->m_iCorrespIndex)
						break;
				}
				m_FacetList.GetAt(newFacet.m_pNeighbFacet[1]).m_pNeighbFacet[j] = newFacetPos;
			}
			if(bNeighbor2)
			{
				for(j=0;j<3;j++)
				{
					if(m_FacetList.GetAt(newFacet.m_pNeighbFacet[2]).m_edge[j] == pEdge2->m_iCorrespIndex)
						break;
				}
				m_FacetList.GetAt(newFacet.m_pNeighbFacet[2]).m_pNeighbFacet[j] = newFacetPos;
			}

			CalculateOneFacetNormal(m_FacetList.GetAt(newFacetPos));
		}
		else
		{
			pEdge0 = &m_EdgeList.GetAt(edgeLoop.GetAt(edgeLoop.FindIndex((i-1)%count)));
			pEdge1 = &m_EdgeList.GetAt(edgeLoop.GetAt(edgeLoop.FindIndex(i%count)));

			newEdge.m_VertexIndex[0] = newFacet.m_VertexIndex[2];
			newEdge.m_VertexIndex[1] = newFacet.m_VertexIndex[0];
			m_EdgeList.AddTail(newEdge);
			newEdgePos = m_EdgeList.GetTailPosition();
			m_EdgeList.GetTail().m_iIndex = newEdgePos;

			newCorrEdge.m_VertexIndex[0] = newFacet.m_VertexIndex[0];
			newCorrEdge.m_VertexIndex[1] = newFacet.m_VertexIndex[2];
			newCorrEdge.m_iNeighbFacet = NULL;
			m_EdgeList.AddTail(newCorrEdge);
			newCorrEdgePos = m_EdgeList.GetTailPosition();
			m_EdgeList.GetTail().m_iIndex = newCorrEdgePos;

			m_EdgeList.GetTail().m_iCorrespIndex = newEdgePos;
			m_EdgeList.GetAt(newEdgePos).m_iCorrespIndex = newCorrEdgePos;

			newFacet.m_edge[0] = pEdge0->m_iIndex;
			newFacet.m_edge[1] = pEdge1->m_iIndex;
			newFacet.m_edge[2] = newEdgePos;

			bool bNeighbor0 = false;
			if(pEdge0->m_iCorrespIndex != NULL) 
			{
				bNeighbor0 = true;
				newFacet.m_pNeighbFacet[0] = m_EdgeList.GetAt(pEdge0->m_iCorrespIndex).m_iNeighbFacet;
			}
			else newFacet.m_pNeighbFacet[0] = NULL;

			bool bNeighbor1 = false;
			if(pEdge1->m_iCorrespIndex != NULL) 
			{
				bNeighbor1 = true;
				newFacet.m_pNeighbFacet[1] = m_EdgeList.GetAt(pEdge1->m_iCorrespIndex).m_iNeighbFacet;
			}
			else newFacet.m_pNeighbFacet[1] = NULL;
			newFacet.m_pNeighbFacet[2] = NULL;

			m_FacetList.AddTail(newFacet);
			newFacetPos = m_FacetList.GetTailPosition();
			m_FacetList.GetTail().m_iIndex = 
				pEdge0->m_iNeighbFacet =
				pEdge1->m_iNeighbFacet =
				m_EdgeList.GetAt(newEdgePos).m_iNeighbFacet = newFacetPos;

			if(bNeighbor0)
			{
				for(j=0;j<3;j++)
				{
					if(m_FacetList.GetAt(newFacet.m_pNeighbFacet[0]).m_edge[j] == pEdge0->m_iCorrespIndex)
						break;
				}
				m_FacetList.GetAt(newFacet.m_pNeighbFacet[0]).m_pNeighbFacet[j] = newFacetPos;
			}

			if(bNeighbor1)
			{
				for(j=0;j<3;j++)
				{
					if(m_FacetList.GetAt(newFacet.m_pNeighbFacet[1]).m_edge[j] == pEdge1->m_iCorrespIndex)
						break;
				}
				m_FacetList.GetAt(newFacet.m_pNeighbFacet[1]).m_pNeighbFacet[j] = newFacetPos;
			}

			CalculateOneFacetNormal(m_FacetList.GetAt(newFacetPos));
			pointLoop.RemoveAt(pointLoop.FindIndex(i%count));
			edgeLoop.SetAt(edgeLoop.FindIndex((i-1)%count),newCorrEdgePos);
			edgeLoop.RemoveAt(edgeLoop.FindIndex(i%count));		
		}
	}
}


inline bool XTriMesh::IsTriangleCCW(const PNT & v0,const PNT & v1,const PNT & v2) const
{
	return (v1[0]*v2[1]-v1[1]*v2[0]) - (v0[0]*v2[1]-v0[1]*v2[0]) 
		+ (v0[0]*v1[1]-v0[1]*v1[0]) >= 0;
}

inline double XTriMesh::CalculateTriArea(const PNT & v0,const PNT & v1,const PNT & v2) const
{

	return  ((v1[0]*v2[1]-v1[1]*v2[0]) - (v0[0]*v2[1]-v0[1]*v2[0]) 
		+ (v0[0]*v1[1]-v0[1]*v1[0]))/2.0;
}


inline bool XTriMesh::IsTriangleCCW(const TriFacet & triAngle) const
{
	double x1 = m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[0];
	double y1 = m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[1];
	double x2 = m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[0];
	double y2 = m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[1];
	double x3 = m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[0];
	double y3 = m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[1];

	return (x2*y3-y2*x3) - (x1*y3-y1*x3) + (x1*y2-y1*x2) >= 0;
}


inline double XTriMesh::CalculateTriArea(const TriFacet & triAngle) const
{
	double x1 = m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[0];
	double y1 = m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[1];
	double x2 = m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[0];
	double y2 = m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[1];
	double x3 = m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[0];
	double y3 = m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[1];

	return ((x2*y3-y2*x3) - (x1*y3-y1*x3) + (x1*y2-y1*x2))/2.0;
}


bool XTriMesh::IsOuterEdge(CPosList & edgeLoop)
{
	int i;
	int count = edgeLoop.GetCount();
	CArray<PNT,PNT &> polyPnt;
	polyPnt.SetSize(count+2);

	POSITION pos=edgeLoop.GetHeadPosition();
	for(i=0;pos;i++)
	{
		polyPnt.SetAt(i,m_PointArray[m_EdgeList.GetAt(edgeLoop.GetNext(pos)).m_VertexIndex[0]].m_Coord);
	}
	polyPnt.SetAt(i,polyPnt[0]);

	int rmin = 0;
	float xmin = polyPnt[0][0];
	float ymin = polyPnt[0][1];

	for(i=1; i<count; i++)
	{
		if (polyPnt[i][1] > ymin)
			continue;
		if (polyPnt[i][1] <= ymin) 
		{   
			if (polyPnt[i][0] < xmin)  
				continue;
		}
		rmin = i;        
		xmin = polyPnt[i][0];
		ymin = polyPnt[i][1];
	}

	float area;
	if(rmin == 0)
	{
		area = CalculateTriArea( polyPnt[count-1], polyPnt[0], polyPnt[1]);
	}
	else
	{
		area =  CalculateTriArea( polyPnt[rmin-1], polyPnt[rmin], polyPnt[rmin+1]);
	}
	if(fabs(area)<1e-3)
	{
		area = CalculatePolygonArea_2D(edgeLoop);
	}
	return area>=0;
}

bool XTriMesh::IsOuterEdgeDir(CPosList & edgeLoop)
{
	int i;
	int count = edgeLoop.GetCount();
	CArray<PNT,PNT &> polyPnt;
	polyPnt.SetSize(count+2);

	float x[3];
	PNT axis_z(m_ViewDirection),tempPnt,cldPnt;

	for(i=0;i<3;i++)
	{
		if(fabs(m_ViewDirection[i])>error)
			break;
	}
	if(i==3) return false;

	x[i] = m_ViewDirection[(i+1)%3] + m_ViewDirection[(i+2)%3];
	x[(i+1)%3] = x[(i+2)%3] = -m_ViewDirection[i];
	PNT axis_x(x),axis_y;

	axis_x.Normalize();
	axis_z.Normalize();

	axis_y = axis_z^axis_x;
	axis_y.Normalize();
	axis_x = axis_y^axis_z;
	axis_x.Normalize();

	POSITION pos=edgeLoop.GetHeadPosition();
	for(i=0;pos;i++)
	{
		tempPnt = m_PointArray[m_EdgeList.GetAt(edgeLoop.GetNext(pos)).m_VertexIndex[0]].m_Coord;
		cldPnt[0] = axis_x[0]*tempPnt[0] + axis_x[1]*tempPnt[1] + axis_x[2]*tempPnt[2];
		cldPnt[1] = axis_y[0]*tempPnt[0] + axis_y[1]*tempPnt[1] + axis_y[2]*tempPnt[2];
		cldPnt[2] = axis_z[0]*tempPnt[0] + axis_z[1]*tempPnt[1] + axis_z[2]*tempPnt[2];
		polyPnt.SetAt(i,cldPnt);
	}
	polyPnt.SetAt(i,polyPnt[0]);

	int rmin = 0;
	float xmin = polyPnt[0][0];
	float ymin = polyPnt[0][1];

	for(i=1; i<count; i++)
	{
		if (polyPnt[i][1] > ymin)
			continue;
		if (polyPnt[i][1] <= ymin) 
		{   
			if (polyPnt[i][0] < xmin)  
				continue;
		}
		rmin = i;        
		xmin = polyPnt[i][0];
		ymin = polyPnt[i][1];
	}

	float area;
	if(rmin == 0)
	{
		area = CalculateTriArea( polyPnt[count-1], polyPnt[0], polyPnt[1]);
	}
	else
	{
		area =  CalculateTriArea( polyPnt[rmin-1], polyPnt[rmin], polyPnt[rmin+1]);
	}
	if(fabs(area)<1e-3)
	{
		area = CalculatePolygonArea_2D(edgeLoop);
	}
	return area>=0;
}


inline double XTriMesh::CalculateMinAngle(const TriFacet & triAngle) const
{
	return CalculateMinAngle(m_PointArray[triAngle.m_VertexIndex[0]].m_Coord,
		m_PointArray[triAngle.m_VertexIndex[1]].m_Coord,m_PointArray[triAngle.m_VertexIndex[2]].m_Coord);
}


inline double XTriMesh::CalculateMinAngle(const PNT & v0,const PNT & v1,const PNT & v2) const
{
	float  xdo, ydo, xao, yao, xco,yco;
	float  a2, b2,c2;           
	float  cosa, cosc;
	float  angleA, angleC; 
	xdo = v1[0] - v0[0];
	ydo = v1[1] - v0[1];
	xao = v2[0] - v0[0];
	yao = v2[1] - v0[1];
	xco = v1[0] - v2[0];
	yco = v1[1] - v2[1];
	a2 = xdo*xdo + ydo*ydo ;
	b2 = xao*xao + yao*yao ;
	c2 = xco*xco + yco*yco ;

	cosa =(b2+c2-a2)/sqrt(4.0*b2*c2);
	if(cosa>1.) cosa = 1.;
	else if(cosa<-1.) cosa = -1.;
	cosc =(a2+b2-c2)/sqrt(4.0*a2*b2);
	if(cosc>1.) cosc = 1.;
	else if(cosc<-1.) cosc = -1.;
	angleA = acos(cosa);
	angleC = acos(cosc); 
	return min(min(angleA, angleC), PI-angleA-angleC);
}

bool XTriMesh::IsEdgeValidate(POSITION edgePos)
{
	TriFacet * tri0 = &m_FacetList.GetAt(m_EdgeList.GetAt(edgePos).m_iNeighbFacet);
	POSITION coEdgePos = m_EdgeList.GetAt(edgePos).m_iCorrespIndex;
	TriFacet * tri1 = &m_FacetList.GetAt(m_EdgeList.GetAt(coEdgePos).m_iNeighbFacet);
	float minAngleNow = min(CalculateMinAngle(*tri0),CalculateMinAngle(*tri1));
	int i,j;
	for(i=0;i<3;i++)
	{
		if(tri0->m_edge[i] == edgePos)
			break;
	}
	for(j=0;j<3;i++)
	{
		if(tri1->m_edge[i] == coEdgePos)
			break;
	}
	float minAngleSwap = min(CalculateMinAngle(m_PointArray[tri0->m_VertexIndex[(i+2)%3]].m_Coord,
		m_PointArray[tri1->m_VertexIndex[(j+2)%3]].m_Coord
		,m_PointArray[tri0->m_VertexIndex[i]].m_Coord)
		,CalculateMinAngle(m_PointArray[tri0->m_VertexIndex[(i+2)%3]].m_Coord
		,m_PointArray[tri1->m_VertexIndex[(j+2)%3]].m_Coord
		,m_PointArray[tri0->m_VertexIndex[(i+1)%3]].m_Coord));
	return minAngleNow>minAngleSwap;
}

bool XTriMesh::SwapEdgeWithTest(POSITION edgePos,POSITION newEdgePos[4])
{
	TriFacet * tri0 = &m_FacetList.GetAt(m_EdgeList.GetAt(edgePos).m_iNeighbFacet);
	POSITION coEdgePos = m_EdgeList.GetAt(edgePos).m_iCorrespIndex;
	TriFacet * tri1 = &m_FacetList.GetAt(m_EdgeList.GetAt(coEdgePos).m_iNeighbFacet);
	float minAngleNow = min(CalculateMinAngle(*tri0),CalculateMinAngle(*tri1));
	int i,j,k;
	for(i=0;i<3;i++)
	{
		if(tri0->m_edge[i] == edgePos)
			break;
	}
	for(j=0;j<3;j++)
	{
		if(tri1->m_edge[j] == coEdgePos)
			break;
	}

	int v00,v01,v02,v10,v11,v12;
	v00 = tri1->m_VertexIndex[(j+2)%3];
	v01 = tri0->m_VertexIndex[(i+2)%3];
	v02 = tri0->m_VertexIndex[i];
	v10 = tri0->m_VertexIndex[(i+2)%3];
	v11 = tri1->m_VertexIndex[(j+2)%3];
	v12 = tri0->m_VertexIndex[(i+1)%3];

	float minAngleSwap = min(CalculateMinAngle(m_PointArray[v00].m_Coord
		,m_PointArray[v01].m_Coord
		,m_PointArray[v02].m_Coord)
		,CalculateMinAngle(m_PointArray[v10].m_Coord
		,m_PointArray[v11].m_Coord
		,m_PointArray[v12].m_Coord));

	if(minAngleNow>=minAngleSwap) return false;
	if(!IsTriangleCCW(m_PointArray[v00].m_Coord,m_PointArray[v01].m_Coord,m_PointArray[v02].m_Coord)
		||!IsTriangleCCW(m_PointArray[v10].m_Coord,m_PointArray[v11].m_Coord,m_PointArray[v12].m_Coord))
		/*if(CalculateTriArea(m_PointArray[v00].m_Coord,m_PointArray[v01].m_Coord,m_PointArray[v02].m_Coord)<error
		||CalculateTriArea(m_PointArray[v10].m_Coord,m_PointArray[v11].m_Coord,m_PointArray[v12].m_Coord)<error)*/
		return false;


	POSITION neiFacet01,neiFacet02,neiFacet11,neiFacet12;

	newEdgePos[0] = tri0->m_edge[(i+2)%3];
	newEdgePos[1] = tri1->m_edge[(j+1)%3];
	newEdgePos[2] = tri1->m_edge[(j+2)%3];
	newEdgePos[3] = tri0->m_edge[(i+1)%3];

	m_EdgeList.GetAt(newEdgePos[1]).m_iNeighbFacet = tri0->m_iIndex;
	m_EdgeList.GetAt(newEdgePos[3]).m_iNeighbFacet = tri1->m_iIndex;

	neiFacet01 = tri0->m_pNeighbFacet[(i+2)%3];
	neiFacet02 = tri1->m_pNeighbFacet[(j+1)%3];
	neiFacet11 = tri1->m_pNeighbFacet[(j+2)%3];
	neiFacet12 = tri0->m_pNeighbFacet[(i+1)%3];

	TriFacet * pFacet = NULL;
	if(neiFacet02!=NULL)
	{
		pFacet = &m_FacetList.GetAt(neiFacet02);
		for(k=0;k<3;k++)
		{
			if(pFacet->m_pNeighbFacet[k]==tri1->m_iIndex) break;
		}
		pFacet->m_pNeighbFacet[k] = tri0->m_iIndex;
	}

	if(neiFacet12!=NULL)
	{
		pFacet = &m_FacetList.GetAt(neiFacet12);
		for(int k=0;k<3;k++)
		{
			if(pFacet->m_pNeighbFacet[k]==tri0->m_iIndex) 
			{
				break;
			}
			pFacet->m_pNeighbFacet[k] = tri1->m_iIndex;	
		}
	}

	m_EdgeList.GetAt(edgePos).m_VertexIndex[0] = v00;
	m_EdgeList.GetAt(edgePos).m_VertexIndex[1] = v01;
	m_EdgeList.GetAt(coEdgePos).m_VertexIndex[0] = v10;
	m_EdgeList.GetAt(coEdgePos).m_VertexIndex[1] = v11;

	tri0->m_edge[0] = edgePos;
	tri0->m_edge[1] = newEdgePos[0];
	tri0->m_edge[2] = newEdgePos[1];
	tri1->m_edge[0] = coEdgePos;
	tri1->m_edge[1] = newEdgePos[2];
	tri1->m_edge[2] = newEdgePos[3];

	tri0->m_pNeighbFacet[0] = tri1->m_iIndex;
	tri0->m_pNeighbFacet[1] = neiFacet01;
	tri0->m_pNeighbFacet[2] = neiFacet02;
	tri1->m_pNeighbFacet[0] = tri0->m_iIndex;
	tri1->m_pNeighbFacet[1] = neiFacet11;
	tri1->m_pNeighbFacet[2] = neiFacet12;

	tri0->m_VertexIndex[0] = v00;
	tri0->m_VertexIndex[1] = v01;
	tri0->m_VertexIndex[2] = v02;
	tri1->m_VertexIndex[0] = v10;
	tri1->m_VertexIndex[1] = v11;
	tri1->m_VertexIndex[2] = v12;

	CalculateOneFacetNormal(*tri0);
	CalculateOneFacetNormal(*tri1);
	return true;
}


int XTriMesh::EdgeIntersection(const PNT & startPnt0,const PNT & endPnt0,
							   const PNT & startPnt1,const PNT & endPnt1) const
{
	double u,v,delta;
	double t1,t2;
	double a,b,c,d;
	double xba,yba,xdc,ydc,xca,yca;
	xba = endPnt0[0] - startPnt0[0];   
	yba = endPnt0[1] - startPnt0[1];
	xdc = endPnt1[0] - startPnt1[0];    
	ydc = endPnt1[1] - startPnt1[1];
	xca = startPnt1[0] - startPnt0[0];    
	yca = startPnt1[1] - startPnt0[1];
	delta = xba*ydc - yba*xdc;
	t1 = xca*ydc - yca*xdc;
	t2 = xca*yba - yca*xba;
	if(delta!=0)
	{
		u = t1/delta;   
		v = t2/delta;
		/*two segments intersect (including intersect at end points)*/
		if(u<=1&&u>=0&&v<=1&&v>=0) return 1;
		else return 0; 
	}
	else
	{       
		/* AB & CD are parallel. */
		if((t1!=0)&&(t2!=0)) return 2;
		/* when AB & CD are collinear */
		/*if AB isn't a vertical line segment, project to x-axis */
		if(startPnt0[0]!=endPnt0[0])   
		{
			a = min(startPnt0[0],endPnt0[0]); 
			b = max(startPnt0[0],endPnt0[0]);
			c = min(startPnt1[0],endPnt1[0]); 
			d = max(startPnt1[0],endPnt1[0]);
			if ((d<a)||(c>b)) return  3;
			else if((d==a)||(c==b)) return 4;  
			else return 5;
		}
		else 
			/* if AB is a vertical line segment, project to y-axis */  
		{
			a=min(startPnt0[1],endPnt0[1]); b=max(startPnt0[1],endPnt0[1]);
			c=min(startPnt1[1],endPnt1[1]); d=max(startPnt1[1],endPnt1[1]); 
			if((d<a)||(c>b)) return  3;
			else if((d==a)||(c==b)) return 4;
			else return 5;

		}
	}
}


int XTriMesh::EdgeIntersection(POSITION edgeA,POSITION edgeB) const
{
	const TriEdge * pEdgeA = &m_EdgeList.GetAt(edgeA);
	const TriEdge * pEdgeB = &m_EdgeList.GetAt(edgeB);
	return EdgeIntersection(m_PointArray[pEdgeA->m_VertexIndex[0]].m_Coord,m_PointArray[pEdgeA->m_VertexIndex[1]].m_Coord,
		m_PointArray[pEdgeB->m_VertexIndex[0]].m_Coord,m_PointArray[pEdgeB->m_VertexIndex[1]].m_Coord);
}

//===================================================================
//    Input:  m = number of vertices in polygon 1
//            V = array of vertices for convex polygon 1 with V[m]=V[0]
//            n = number of vertices in polygon 2
//            W = array of vertices for convex polygon 2 with W[n]=W[0]
//    Output: t1,t2 = index of tangent point V[t1] for polygon 1,W[t2] for polygon 2
void XTriMesh::LeftAndRightTangentPnt(int m, PNT * V, int n, PNT *  W, int& t1, int& t2)
{
	int ix1, ix2;
	ix1 = RightTangentPnt(W[0], m,V);   
	ix2 = LeftTangentPnt(V[ix1], n,W); 

	int done = FALSE;               
	while(done==FALSE) 
	{
		done = TRUE;                 
		while(CalculateTriArea(W[ix2], V[ix1], V[ix1+1])<=0)
		{
			++ix1;
			if(ix1>=m) ix1 = 0;
		}
		while(CalculateTriArea(V[ix1], W[ix2], W[ix2-1])>=0)
		{
			--ix2; 
			if(ix2<0) ix2 = n;
			done = FALSE;            
		}
	}
	t1 = ix1;
	t2 = ix2;  
}


int XTriMesh::LeftTangentPnt(PNT & P, int n, PNT * V)
{
	int a, b, c;          
	int dnA, dnC;         
	if(IsTriangleCCW(P,V[n-1],V[0]) && !!IsTriangleCCW(P,V[1],V[0]))
		return 0;            

	for(a=0, b=n;;) 
	{         
		c = (a + b) / 2;      
		dnC = !IsTriangleCCW(P,V[c+1],V[c]);
		if (IsTriangleCCW(P,V[c-1],V[c]) && !dnC)
			return c;        
		dnA = !IsTriangleCCW(P,V[a+1],V[a]);
		if(dnA) 
		{                     
			if(!dnC)                       
				b = c;                         
			else 
			{                      
				if(!IsTriangleCCW(P,V[a],V[c]))    
					b = c;                     
				else                        
					a = c;                    
			}
		}
		else 
		{                        
			if(dnC)                       
				a = c;                       
			else 
			{                         
				if(IsTriangleCCW(P,V[a],V[c]))    
					b = c;                    
				else                        
					a = c;                  
			}
		}
	}  
}

int XTriMesh::RightTangentPnt(PNT & P, int n, PNT * V)
{
	int  a, b, c;         
	int  upA, dnC;        

	if(!IsTriangleCCW(P,V[1],V[0])&&!IsTriangleCCW(P,V[n-1],V[0]))
		return 0;              

	for(a=0, b=n;;) 
	{       
		c = (a + b) / 2;      
		dnC = !IsTriangleCCW(P,V[c+1],V[c]);
		if(dnC && !IsTriangleCCW(P,V[c-1],V[c]))
			return c;        
		upA = IsTriangleCCW(P,V[a+1],V[a]);
		if(upA) 
		{                   
			if (dnC)                      
				b = c;                       
			else 
			{                      
				if (IsTriangleCCW(P,V[a],V[c]))   
					b = c;                     
				else                    
					a = c;                     
			}
		}
		else 
		{                       
			if(!dnC)                      
				a = c;                        
			else 
			{                         
				if(!IsTriangleCCW(P,V[a],V[c]))     
					b = c;                  
				else                        
					a = c;                    
			}
		}
	}
}


void XTriMesh::JoinTwoMesh(CPosList & edgeListA,CPosList & edgeListB)
{
	CIntList pointLoopA,pointLoopB;
	CPosList edgeLoopA,edgeLoopB;
	InitialPolygonLoop(edgeListA,pointLoopA,edgeLoopA);
	InitialPolygonLoop(edgeListB,pointLoopB,edgeLoopB);

	int m = pointLoopA.GetCount();
	int n = pointLoopB.GetCount();
	int i;
	PNT * vectorA = new PNT[m+1];
	PNT * vectorB = new PNT[n+1];
	PNT * vectorAhull = new PNT[m+1];
	PNT * vectorBhull = new PNT[n+1];
	int * vectorAindex = new int[m+1];
	int * vectorBindex = new int[n+1];

	for(i=0;i<m+1;i++)
	{
		vectorA[i] = m_PointArray[pointLoopA.GetAt(pointLoopA.FindIndex(i%m))].m_Coord;
	}
	for(i=0;i<n+1;i++)
	{
		vectorB[i] = m_PointArray[pointLoopB.GetAt(pointLoopB.FindIndex(i%n))].m_Coord;
	}

	int a1,b1,a2,b2,a1hull,b1hull,a2hull,b2hull;
	int m1 = FindHull_2D(vectorA,m,vectorAhull,vectorAindex);
	int n1 = FindHull_2D(vectorB,n,vectorBhull,vectorBindex);

	LeftAndRightTangentPnt(m1,vectorAhull,n1,vectorBhull,a1hull,b1hull);
	LeftAndRightTangentPnt(n1,vectorBhull,m1,vectorAhull,b2hull,a2hull);
	a1 = vectorAindex[a1hull];
	a2 = vectorAindex[a2hull];
	b1 = vectorBindex[b1hull];
	b2 = vectorBindex[b2hull];

	/*int a1,b1,a2,b2;
	LeftAndRightTangentPnt(m,vectorA,n,vectorB,a1,b1);
	LeftAndRightTangentPnt(n,vectorB,m,vectorA,b2,a2);*/

	a1 = a1%m;
	a2 = a2%m;
	b1 = b1%n;
	b2 = b2%n;

	int indexA = min(a1,a2);
	int indexAEnd = max(a1,a2);
	int indexB,indexBEnd;
	CPosList edgeA,edgeB;

	bool bWay; 
	if(fabsf(CalculateTriArea(vectorA[a1],vectorA[a2],vectorA[(a1+a2)/2]))
	>fabsf(CalculateTriArea(vectorA[a1],vectorA[a2],vectorA[((a1+a2+m)/2)%m])))
	{
		bWay = IsTriangleCCW(vectorA[a1],vectorA[a2],vectorA[(a1+a2)/2])
			!= IsTriangleCCW(vectorB[b1],vectorB[b2],vectorA[(a1+a2)/2]);
	}
	else
	{
		bWay = IsTriangleCCW(vectorA[a1],vectorA[a2],vectorA[((a1+a2+m)/2)%m])
			== IsTriangleCCW(vectorB[b1],vectorB[b2],vectorA[((a1+a2+m)/2)%m]);
	}
	if(bWay)
	{
		while(indexA!=indexAEnd)
		{
			edgeA.AddTail(edgeLoopA.GetAt(edgeLoopA.FindIndex(indexA)));
			indexA++;
		}
		if(indexAEnd==a1) 
		{
			indexB=b1;indexBEnd=b2;
		}
		else
		{
			indexB=b2;indexBEnd=b1;
		}
		while(indexB!=indexBEnd)
		{
			edgeB.AddTail(edgeLoopB.GetAt(edgeLoopB.FindIndex(indexB)));
			indexB++;
			if(indexB==n) indexB = 0;
		}
	}
	else
	{
		swap(indexA,indexAEnd);
		while(indexA!=indexAEnd)
		{
			edgeA.AddTail(edgeLoopA.GetAt(edgeLoopA.FindIndex(indexA)));
			indexA++;
			if(indexA==m) indexA = 0;
		}
		if(indexAEnd==a1) 
		{
			indexB=b1;indexBEnd=b2;
		}
		else
		{
			indexB=b2;indexBEnd=b1;
		}
		while(indexB!=indexBEnd)
		{
			edgeB.AddTail(edgeLoopB.GetAt(edgeLoopB.FindIndex(indexB)));
			indexB++;
			if(indexB==n) indexB = 0;
		}
	}

	CPosList newInnerEdge;
	POSITION edgeApos = edgeA.GetTailPosition();
	POSITION edgePos;
	TriEdge newEdge;
	TriEdge * pEdge;
	int edgeAcount = edgeA.GetCount();

	for(i=0;i<edgeAcount;i++)
	{
		pEdge = &m_EdgeList.GetAt(edgeA.GetPrev(edgeApos));

		newEdge.m_VertexIndex[0] = pEdge->m_VertexIndex[1];
		newEdge.m_VertexIndex[1] = pEdge->m_VertexIndex[0];
		newEdge.m_iCorrespIndex = pEdge->m_iIndex;
		newEdge.m_iNeighbFacet = NULL;
		m_EdgeList.AddTail(newEdge);
		edgePos = m_EdgeList.GetTailPosition();
		pEdge->m_iCorrespIndex = 
			m_EdgeList.GetAt(edgePos).m_iIndex = edgePos;
		newInnerEdge.AddTail(edgePos);
	}

	POSITION edgeBpos = edgeB.GetTailPosition();
	int edgeBcount = edgeB.GetCount();
	pEdge = &m_EdgeList.GetAt(edgeB.GetAt(edgeBpos));

	newEdge.m_VertexIndex[0] = newEdge.m_VertexIndex[1];
	newEdge.m_VertexIndex[1] = pEdge->m_VertexIndex[1];
	newEdge.m_iCorrespIndex = NULL;
	newEdge.m_iNeighbFacet = NULL;
	m_EdgeList.AddTail(newEdge);
	edgePos = m_EdgeList.GetTailPosition();
	m_EdgeList.GetAt(edgePos).m_iIndex = edgePos;
	newInnerEdge.AddTail(edgePos);

	for(i=0;i<edgeBcount;i++)
	{
		pEdge = &m_EdgeList.GetAt(edgeB.GetPrev(edgeBpos));

		newEdge.m_VertexIndex[0] = pEdge->m_VertexIndex[1];
		newEdge.m_VertexIndex[1] = pEdge->m_VertexIndex[0];
		newEdge.m_iCorrespIndex = pEdge->m_iIndex;
		newEdge.m_iNeighbFacet = NULL;
		m_EdgeList.AddTail(newEdge);
		edgePos = m_EdgeList.GetTailPosition();
		pEdge->m_iCorrespIndex = 
			m_EdgeList.GetAt(edgePos).m_iIndex = edgePos;
		newInnerEdge.AddTail(edgePos);
	}

	pEdge = &m_EdgeList.GetAt(edgeA.GetTail());
	newEdge.m_VertexIndex[0] = newEdge.m_VertexIndex[1];
	newEdge.m_VertexIndex[1] = pEdge->m_VertexIndex[1];
	newEdge.m_iCorrespIndex = NULL;
	newEdge.m_iNeighbFacet = NULL;
	m_EdgeList.AddTail(newEdge);
	edgePos = m_EdgeList.GetTailPosition();
	m_EdgeList.GetAt(edgePos).m_iIndex = edgePos;
	newInnerEdge.AddTail(edgePos);

	FillWithoutInitial(newInnerEdge);

	delete []vectorA;
	delete []vectorB;
	delete []vectorAhull;
	delete []vectorBhull;
	delete []vectorAindex;
	delete []vectorBindex;
}


void XTriMesh::JoinForIntegration(CPosList & edgeListA,CPosList & edgeListB,PNT & viewDir)
{
	viewDir = PNT(0,0,1);
	int i,j;
	CIntList pointLoopA,pointLoopB;
	CPosList edgeLoopA,edgeLoopB;
	InitialPolygonLoop(edgeListA,pointLoopA,edgeLoopA);
	InitialPolygonLoop(edgeListB,pointLoopB,edgeLoopB);

	//object rotation
	float x[3];
	//CList<PNT,PNT&> vectorListA,vectorListB;
	int m = pointLoopA.GetCount();
	int n = pointLoopB.GetCount();
	PNT * vectorA = new PNT[m+1];
	PNT * vectorB = new PNT[n+1];
	PNT axis_z(viewDir),tempPnt;
	for(i=0;i<3;i++)
	{
		if(fabs(axis_z[i])>error)
			break;
	}
	if(i==3) return;

	x[i] = axis_z[(i+1)%3] + axis_z[(i+2)%3];
	x[(i+1)%3] = x[(i+2)%3] = -axis_z[i];
	PNT axis_x(x),axis_y;

	axis_x.Normalize();
	axis_z.Normalize();

	axis_y = axis_z^axis_x;
	axis_y.Normalize();
	axis_x = axis_y^axis_z;
	axis_x.Normalize();

	POSITION pos = pointLoopA.GetHeadPosition();
	for(j=0;pos;j++)
	{
		i = pointLoopA.GetNext(pos);
		tempPnt = m_PointArray[i].m_Coord;
		m_PointArray[i].m_Coord[0] = axis_x[0]*tempPnt[0] + axis_x[1]*tempPnt[1] + axis_x[2]*tempPnt[2];
		m_PointArray[i].m_Coord[1] = axis_y[0]*tempPnt[0] + axis_y[1]*tempPnt[1] + axis_y[2]*tempPnt[2];
		m_PointArray[i].m_Coord[2] = axis_z[0]*tempPnt[0] + axis_z[1]*tempPnt[1] + axis_z[2]*tempPnt[2];
		//vectorListA.AddTail(tempPnt);
		vectorA[j] = tempPnt;
	}

	pos = pointLoopB.GetHeadPosition();
	for(j=0;pos;j++)
	{
		i = pointLoopB.GetNext(pos);
		tempPnt = m_PointArray[i].m_Coord;
		m_PointArray[i].m_Coord[0] = axis_x[0]*tempPnt[0] + axis_x[1]*tempPnt[1] + axis_x[2]*tempPnt[2];
		m_PointArray[i].m_Coord[1] = axis_y[0]*tempPnt[0] + axis_y[1]*tempPnt[1] + axis_y[2]*tempPnt[2];
		m_PointArray[i].m_Coord[2] = axis_z[0]*tempPnt[0] + axis_z[1]*tempPnt[1] + axis_z[2]*tempPnt[2];
		//vectorListB.AddTail(tempPnt);
		vectorB[j] = tempPnt;
	}


	/*int a1,b1,a2,b2;
	vectorA[m] = vectorA[0];
	vectorB[n] = vectorB[0];
	LeftAndRightTangentPnt(m,vectorA,n,vectorB,a1,b1);
	LeftAndRightTangentPnt(n,vectorB,m,vectorA,b2,a2);

	a1 = a1%m;
	a2 = a2%m;
	b1 = b1%n;
	b2 = b2%n;*/
	
	//rotation end;

	CPosList newInnerEdge,newInnerEdgeA,newInnerEdgeB;
	POSITION edgeApos = edgeLoopA.GetTailPosition();
	POSITION edgePos;
	TriEdge newEdge;
	TriEdge * pEdge;
	int edgeAcount = edgeLoopA.GetCount();

	for(i=0;i<edgeAcount;i++)
	{
		pEdge = &m_EdgeList.GetAt(edgeLoopA.GetPrev(edgeApos));
		m_FacetList.GetAt(pEdge->m_iNeighbFacet).m_iFacetType = 9;

		newEdge.m_VertexIndex[0] = pEdge->m_VertexIndex[1];
		newEdge.m_VertexIndex[1] = pEdge->m_VertexIndex[0];
		newEdge.m_iCorrespIndex = pEdge->m_iIndex;
		newEdge.m_iNeighbFacet = NULL;
		m_EdgeList.AddTail(newEdge);
		edgePos = m_EdgeList.GetTailPosition();
		pEdge->m_iCorrespIndex = 
			m_EdgeList.GetAt(edgePos).m_iIndex = edgePos;
		newInnerEdgeA.AddTail(edgePos);
	}

	POSITION edgeBpos = edgeLoopB.GetTailPosition();
	int edgeBcount = edgeLoopB.GetCount();

	for(i=0;i<edgeBcount;i++)
	{
		pEdge = &m_EdgeList.GetAt(edgeLoopB.GetPrev(edgeBpos));
		m_FacetList.GetAt(pEdge->m_iNeighbFacet).m_iFacetType = 9;

		newEdge.m_VertexIndex[0] = pEdge->m_VertexIndex[1];
		newEdge.m_VertexIndex[1] = pEdge->m_VertexIndex[0];
		newEdge.m_iCorrespIndex = pEdge->m_iIndex;
		newEdge.m_iNeighbFacet = NULL;
		m_EdgeList.AddTail(newEdge);
		edgePos = m_EdgeList.GetTailPosition();
		pEdge->m_iCorrespIndex = 
			m_EdgeList.GetAt(edgePos).m_iIndex = edgePos;
		newInnerEdgeB.AddTail(edgePos);
	}

	PNT endA,startB;
	bool flag;
	while(newInnerEdgeA.GetCount()>0&&newInnerEdgeB.GetCount()>0)
	{
		flag = false;
		endA = m_PointArray[m_EdgeList.GetAt(newInnerEdgeA.GetTail()).m_VertexIndex[1]].m_Coord;
		startB = m_PointArray[m_EdgeList.GetAt(newInnerEdgeB.GetHead()).m_VertexIndex[0]].m_Coord;
		pos = newInnerEdgeA.GetHeadPosition();
		while(true)
		{
			pEdge = &m_EdgeList.GetAt(newInnerEdgeA.GetNext(pos));
			if(pos==NULL) break;
			
			i = EdgeIntersection(endA,startB,m_PointArray[pEdge->m_VertexIndex[0]].m_Coord,
				m_PointArray[pEdge->m_VertexIndex[1]].m_Coord);
			if(i==1||i==5)
			{
				newInnerEdgeA.RemoveTail();
				newInnerEdgeB.RemoveHead();
				flag = true;
				break;
			}
		}
		if(flag) continue;
		flag = false;

		pos = newInnerEdgeB.GetHeadPosition();
		newInnerEdgeB.GetNext(pos);
		while(pos)
		{
			pEdge = &m_EdgeList.GetAt(newInnerEdgeB.GetNext(pos));
			i = EdgeIntersection(endA,startB,m_PointArray[pEdge->m_VertexIndex[0]].m_Coord,
				m_PointArray[pEdge->m_VertexIndex[1]].m_Coord);
			if(i==1||i==5)
			{
				newInnerEdgeA.RemoveTail();
				newInnerEdgeB.RemoveHead();
				flag = true;
				break;
			}
		}
		if(flag) continue;
		else break;
	}

	PNT startA,endB;
	while(newInnerEdgeA.GetCount()>0&&newInnerEdgeB.GetCount()>0)
	{
		flag = false;
		startA = m_PointArray[m_EdgeList.GetAt(newInnerEdgeA.GetHead()).m_VertexIndex[0]].m_Coord;
		endB = m_PointArray[m_EdgeList.GetAt(newInnerEdgeB.GetTail()).m_VertexIndex[1]].m_Coord;
		pos = newInnerEdgeA.GetHeadPosition();
			newInnerEdgeA.GetNext(pos);
		while(pos)
		{
			pEdge = &m_EdgeList.GetAt(newInnerEdgeA.GetNext(pos));

			i = EdgeIntersection(endB,startA,m_PointArray[pEdge->m_VertexIndex[0]].m_Coord,
				m_PointArray[pEdge->m_VertexIndex[1]].m_Coord);
			if(i==1||i==5)
			{
				newInnerEdgeB.RemoveTail();
				newInnerEdgeA.RemoveHead();
				flag = true;
				break;
			}
		}
		if(flag) continue;
		flag = false;

		pos = newInnerEdgeB.GetHeadPosition();
		while(true)
		{
			pEdge = &m_EdgeList.GetAt(newInnerEdgeB.GetNext(pos));
				if(pos==NULL) break;

			i = EdgeIntersection(endB,startA,m_PointArray[pEdge->m_VertexIndex[0]].m_Coord,
				m_PointArray[pEdge->m_VertexIndex[1]].m_Coord);
			if(i==1||i==5)
			{
				newInnerEdgeB.RemoveTail();
				newInnerEdgeA.RemoveHead();
				flag = true;
				break;
			}
		}
		if(flag) continue;
		else break;
	}

	newInnerEdge.AddTail(&newInnerEdgeA);

	newEdge.m_VertexIndex[0] = m_EdgeList.GetAt(newInnerEdgeA.GetTail()).m_VertexIndex[1];
	newEdge.m_VertexIndex[1] = m_EdgeList.GetAt(newInnerEdgeB.GetHead()).m_VertexIndex[0];
	newEdge.m_iCorrespIndex = NULL;
	newEdge.m_iNeighbFacet = NULL;
	m_EdgeList.AddTail(newEdge);
	edgePos = m_EdgeList.GetTailPosition();
	m_EdgeList.GetAt(edgePos).m_iIndex = edgePos;
	
	newInnerEdge.AddTail(edgePos);
	newInnerEdge.AddTail(&newInnerEdgeB);

	newEdge.m_VertexIndex[0] = m_EdgeList.GetAt(newInnerEdgeB.GetTail()).m_VertexIndex[1];
	newEdge.m_VertexIndex[1] = m_EdgeList.GetAt(newInnerEdgeA.GetHead()).m_VertexIndex[0];
	newEdge.m_iCorrespIndex = NULL;
	newEdge.m_iNeighbFacet = NULL;
	m_EdgeList.AddTail(newEdge);
	edgePos = m_EdgeList.GetTailPosition();
	m_EdgeList.GetAt(edgePos).m_iIndex = edgePos;
	
	newInnerEdge.AddTail(edgePos);

	//test
	/*pos = newInnerEdge.GetHeadPosition();
	while(pos)
	{
		m_EdgeList.GetAt(newInnerEdge.GetNext(pos)).m_iEdgeType = 6;
	}
	m_DispType = 4;*/
	//test end

	FillForIntegration(newInnerEdge);

	CIntList addPntIndex;
	CList<PNT,PNT&> addPntCoord;
	
	for(i=0;i<m_iPointNum;i++)
	{
		if(m_PointArray[i].m_iPntStat==-2)
		{
			tempPnt = m_PointArray[i].m_Coord;
			m_PointArray[i].m_Coord[0] = axis_x[0]*tempPnt[0] + axis_x[1]*tempPnt[1] + axis_x[2]*tempPnt[2];
			m_PointArray[i].m_Coord[1] = axis_y[0]*tempPnt[0] + axis_y[1]*tempPnt[1] + axis_y[2]*tempPnt[2];
			m_PointArray[i].m_Coord[2] = axis_z[0]*tempPnt[0] + axis_z[1]*tempPnt[1] + axis_z[2]*tempPnt[2];
			addPntIndex.AddTail(i);
			addPntCoord.AddTail(tempPnt);
			//if(IsPntInPolygon(m_PointArray[i].m_Coord,newInnerEdge))
			{
				AddInnerPntToTriMeshIntegration(i);
			}
		}
	}

	pos = pointLoopA.GetHeadPosition();
	for(i=0;pos;i++)
	{
		m_PointArray[pointLoopA.GetNext(pos)].m_Coord = vectorA[i];
	}

	pos = pointLoopB.GetHeadPosition();
	for(i=0;pos;i++)
	{
		m_PointArray[pointLoopB.GetNext(pos)].m_Coord = vectorB[i];
	}
	POSITION posVList = addPntCoord.GetHeadPosition();
	pos = addPntIndex.GetHeadPosition();
	while(pos)
	{
		m_PointArray[addPntIndex.GetNext(pos)].m_Coord = addPntCoord.GetNext(posVList);
	}
	delete []vectorA;
	delete []vectorB;
}


void XTriMesh::Add(XTriMesh & triMeshB)
{
	int oldPntNum = m_iPointNum;
	if(oldPntNum==0)
	{
		m_PointArray.SetSize(triMeshB.m_PointArray.GetSize());
		m_PointArray.Copy(triMeshB.m_PointArray);
	}
	else
	{
		m_iPointNum += triMeshB.m_iPointNum;
		m_PointArray.Append(triMeshB.m_PointArray);
	}

	int oldEdgeCount = m_EdgeList.GetCount();
	int oldFacetCount = m_FacetList.GetCount();

	m_EdgeList.AddTail(&triMeshB.m_EdgeList);
	m_FacetList.AddTail(&triMeshB.m_FacetList);

	int i,j,k;
	TriEdge * pEdge;
	TriFacet * pFacet;
	POSITION pos,posNew,posOld;

	CPosArray edgeArray;
	CPosArray facetArray;
	edgeArray.SetSize(m_EdgeList.GetCount()-oldEdgeCount);
	facetArray.SetSize(m_FacetList.GetCount()-oldFacetCount);

	for(i=oldPntNum;i<m_iPointNum;i++)
	{
		m_PointArray[i].m_iIndex += oldPntNum;
	}

	pos = triMeshB.m_EdgeList.GetHeadPosition();
	posNew = m_EdgeList.FindIndex(oldEdgeCount);
	for(i=0;pos;i++)
	{
		triMeshB.m_EdgeList.GetNext(pos).m_iEdgeType = i;
		posOld = posNew;
		pEdge = &m_EdgeList.GetNext(posNew);
		pEdge->m_VertexIndex[0] += oldPntNum;
		pEdge->m_VertexIndex[1] += oldPntNum;
		pEdge->m_iIndex = edgeArray[i] = posOld;
	}

	pos = triMeshB.m_FacetList.GetHeadPosition();
	posNew = m_FacetList.FindIndex(oldFacetCount);
	for(i=0;pos;i++)
	{
		triMeshB.m_FacetList.GetNext(pos).m_iFacetType = i;
		posOld = posNew;
		pFacet = &m_FacetList.GetNext(posNew);
		pFacet->m_VertexIndex[0] += oldPntNum;
		pFacet->m_VertexIndex[1] += oldPntNum;
		pFacet->m_VertexIndex[2] += oldPntNum;
		pFacet->m_iIndex = facetArray[i] = posOld;
	}

	posNew = m_EdgeList.FindIndex(oldEdgeCount);
	for(i=oldEdgeCount;posNew;i++)
	{
		pEdge = &m_EdgeList.GetNext(posNew);
		if(pEdge->m_iCorrespIndex!=NULL)
		{
			j = triMeshB.m_EdgeList.GetAt(pEdge->m_iCorrespIndex).m_iEdgeType;
			pEdge->m_iCorrespIndex = edgeArray[j];
		}
		if(pEdge->m_iNeighbFacet!=NULL)
		{
			j = triMeshB.m_FacetList.GetAt(pEdge->m_iNeighbFacet).m_iFacetType;
			pEdge->m_iNeighbFacet = facetArray[j];
		}
	}

	posNew = m_FacetList.FindIndex(oldFacetCount);
	for(i=oldFacetCount;posNew;i++)
	{
		pFacet = &m_FacetList.GetNext(posNew);
		for(k=0;k<3;k++)
		{
			if(pFacet->m_edge[k]!=NULL)
			{
				j = triMeshB.m_EdgeList.GetAt(pFacet->m_edge[k]).m_iEdgeType;
				pFacet->m_edge[k] = edgeArray[j];
			}
			if(pFacet->m_pNeighbFacet[k]!=NULL)
			{
				j = triMeshB.m_FacetList.GetAt(pFacet->m_pNeighbFacet[k]).m_iFacetType;
				pFacet->m_pNeighbFacet[k] = facetArray[j];
			}
		}
	}

	for(pos = triMeshB.m_EdgeList.GetHeadPosition();pos;)
	{
		triMeshB.m_EdgeList.GetNext(pos).m_iEdgeType = 0;
	}

	for(pos = triMeshB.m_FacetList.GetHeadPosition();pos;)
	{
		triMeshB.m_FacetList.GetNext(pos).m_iFacetType = 0;
	}	
}


int XTriMesh::FindMeshInnerEdge(CPosList * edgeList)
{
	CPosList iniEdgeList;
	CIntList pointLoop;
	POSITION pos = m_EdgeList.GetHeadPosition();
	int count = m_EdgeList.GetCount();
	int i,oldCount;
	TriEdge * pEdge = NULL;

	for(i=0;i<count;i++)
	{
		pEdge = &m_EdgeList.GetNext(pos);
		if(pEdge->m_iCorrespIndex==NULL)
		{
			iniEdgeList.AddTail(pEdge->m_iIndex);
		}
	}
	count = iniEdgeList.GetCount();

	if(count<3) return false;
	i = 0;
	while(count>=3&&i<totalCount)
	{
		InitialPolygonLoop(iniEdgeList,edgeList[i]);

		if(!IsOuterEdge(edgeList[i]))
			i++;
		oldCount = count; 
		count = iniEdgeList.GetCount();
		if(oldCount == count) break;
	}
	return i;
}

int XTriMesh::FindMeshOuterEdge(CPosList * edgeList)
{
	CPosList iniEdgeList;
	CIntList pointLoop;
	POSITION pos = m_EdgeList.GetHeadPosition();
	int count = m_EdgeList.GetCount();
	int i,oldCount;
	TriEdge * pEdge = NULL;

	for(i=0;i<count;i++)
	{
		pEdge = &m_EdgeList.GetNext(pos);
		if(pEdge->m_iCorrespIndex==NULL)
		{
			iniEdgeList.AddTail(pEdge->m_iIndex);
		}
	}
	count = iniEdgeList.GetCount();
	if(count<3) return false;
	i = 0;
	while(count>=3&&i<totalCount)
	{
		InitialPolygonLoop(iniEdgeList,edgeList[i]);
		if(IsOuterEdge(edgeList[i]))
			i++;
		oldCount = count; 
		count = iniEdgeList.GetCount();
		if(oldCount == count) break;
	}
	return i;
}


int XTriMesh::FindMeshOuterEdgeDir(CPosList * edgeList)
{
	CPosList iniEdgeList;
	CIntList pointLoop;
	POSITION pos = m_EdgeList.GetHeadPosition();
	int count = m_EdgeList.GetCount();
	int i,oldCount;
	TriEdge * pEdge = NULL;

	for(i=0;i<count;i++)
	{
		pEdge = &m_EdgeList.GetNext(pos);
		if(pEdge->m_iCorrespIndex==NULL)
		{
			iniEdgeList.AddTail(pEdge->m_iIndex);
		}
	}
	count = iniEdgeList.GetCount();
	if(count<3) return false;
	i = 0;
	while(count>=3&&i<totalCount)
	{
		InitialPolygonLoop(iniEdgeList,edgeList[i]);
		if(IsOuterEdgeDir(edgeList[i]))
			i++;
		oldCount = count; 
		count = iniEdgeList.GetCount();
		if(oldCount == count) break;
	}
	return i;
}


void XTriMesh::AddWithJoin(XTriMesh & triMeshB)
{
	CPosList * edgeList = new CPosList[totalCount];

	Add(triMeshB);
	if(FindMeshOuterEdge(edgeList)==2)
	{
		if(edgeList[0].GetCount()>=3&&edgeList[1].GetCount()>=3)
		{
			JoinTwoMesh(edgeList[0],edgeList[1]);
		}
	}

	SetVertexesForDraw();
	delete []edgeList;
}			


void XTriMesh::IntegrateTwoMesh(XTriMesh & triMeshB)
{
	CPosList * edgeListA = new CPosList[totalCount];
	CPosList * edgeListB = new CPosList[totalCount];

	FindMeshOuterEdgeDir(edgeListA);
	triMeshB.FindMeshOuterEdgeDir(edgeListB);

	POSITION pos = edgeListA[0].GetHeadPosition();
	while(pos)
	{
		m_EdgeList.GetAt(edgeListA[0].GetNext(pos)).m_iEdgeType = 3;
	}
	pos = edgeListB[0].GetHeadPosition();
	while(pos)
	{
		triMeshB.m_EdgeList.GetAt(edgeListB[0].GetNext(pos)).m_iEdgeType = 3;
	}

	PNT viewDir;
	DeleteOverlayRegion(triMeshB,viewDir);

	FindMeshOuterEdgeDir(edgeListA);
	triMeshB.FindMeshOuterEdgeDir(edgeListB);

	TriEdge * pEdge = NULL;
	pos = edgeListA[0].GetHeadPosition();
	while(pos)
	{
		pEdge = &m_EdgeList.GetAt(edgeListA[0].GetNext(pos));
		if(pEdge->m_iEdgeType!=3)
		{
			m_EdgeList.GetAt(pEdge->m_iIndex).m_iEdgeType = 2;
		}
	}
	pos = edgeListB[0].GetHeadPosition();
	while(pos)
	{
		pEdge = &triMeshB.m_EdgeList.GetAt(edgeListB[0].GetNext(pos));
		if(pEdge->m_iEdgeType!=3)
		{
			triMeshB.m_EdgeList.GetAt(pEdge->m_iIndex).m_iEdgeType = 1;
		}
	}

	Add(triMeshB);

	CPosList outerEdgeA,outerEdgeB;
	pos = m_EdgeList.GetHeadPosition();
	while(pos)
	{
		pEdge = &m_EdgeList.GetNext(pos);
		if(pEdge->m_iEdgeType==2)
		{
			outerEdgeA.AddTail(pEdge->m_iIndex);
		}
		else if(pEdge->m_iEdgeType==1)
		{
			outerEdgeB.AddTail(pEdge->m_iIndex);
		}
	}

	JoinForIntegration(outerEdgeA,outerEdgeB,viewDir);

	//add points
	m_iSeedCount = -1;
	CalculateMeshNormal();
	SetVertexesForDraw();
	delete []edgeListA;
	delete []edgeListB;
}


int XTriMesh::GetPntNum() const
{
	return m_iPointNum;
}

int XTriMesh::GetFacetNum() const
{
	return m_FacetList.GetCount();
}

const CArray<XTriMesh::TriPoint,XTriMesh::TriPoint&> & XTriMesh::GetPointArray() const
{
	return m_PointArray;
}

void XTriMesh::SetPointArray(CArray<XTriMesh::TriPoint,XTriMesh::TriPoint&> & inputArray)
{
	if(!m_PointArray.GetSize() != 0)
		m_PointArray.RemoveAll();
	m_PointArray.SetSize(inputArray.GetSize());
	m_PointArray.Copy(inputArray);
}

void XTriMesh::SetBoolDrawState(bool state)
{
	m_bSetVertexesDone = state;
}

void XTriMesh::Clear()
{
	if(!m_PointArray.GetSize()!= 0)
		m_PointArray.RemoveAll();
	if(!m_EdgeList.GetCount()!= 0)
		m_EdgeList.RemoveAll();
	if(!m_FacetList.GetCount() != 0)
		m_FacetList.RemoveAll();
	m_iPointNum = 0;
}

void XTriMesh::DeleteFacet(POSITION facetPos)
{
	TriFacet * pFacet = &m_FacetList.GetAt(facetPos);
	if(pFacet==NULL) return;
	TriFacet * pFacet1 = NULL;
	int i,j;
	for(i=0;i<3;i++)
	{
		if(pFacet->m_pNeighbFacet[i]!=NULL)
		{
			pFacet1 = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[i]);
			for(j=0;j<3;j++)
			{
				if(pFacet1->m_pNeighbFacet[j] == pFacet->m_iIndex)
				{
					pFacet1->m_pNeighbFacet[j] = NULL;
					m_EdgeList.GetAt(pFacet1->m_edge[j]).m_iCorrespIndex = NULL;
				}
			}
		}
		m_EdgeList.RemoveAt(pFacet->m_edge[i]);
	}
	m_FacetList.RemoveAt(facetPos);
}


void XTriMesh::DeleteFacet(int facetIndex)
{
	TriFacet * pFacet = &m_FacetList.GetAt(m_FacetList.FindIndex(facetIndex));
	if(pFacet==NULL) return;
	TriFacet * pFacet1 = NULL;
	int i,j,iDeletePnt;
	for(i=0;i<3;i++)
	{
		iDeletePnt = 0;
		if(pFacet->m_pNeighbFacet[i]!=NULL)
		{
			iDeletePnt += i*i+1;
			pFacet1 = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[i]);
			for(j=0;j<3;j++)
			{
				if(pFacet1->m_pNeighbFacet[j] == pFacet->m_iIndex)
				{
					pFacet1->m_pNeighbFacet[j] = NULL;
					m_EdgeList.GetAt(pFacet1->m_edge[j]).m_iCorrespIndex = NULL;
				}
			}
		}
		m_EdgeList.RemoveAt(pFacet->m_edge[i]);
	}
	switch(iDeletePnt) 
	{
	case 3:
		m_PointArray[pFacet->m_VertexIndex[1]].m_iPntStat = -1;
		break;
	case 6:
		m_PointArray[pFacet->m_VertexIndex[1]].m_iPntStat = -1;
		break;
	case 7:
		m_PointArray[pFacet->m_VertexIndex[2]].m_iPntStat = -1;
		break;
	case 8:
		m_PointArray[pFacet->m_VertexIndex[0]].m_iPntStat = -1;
		m_PointArray[pFacet->m_VertexIndex[1]].m_iPntStat = -1;
		m_PointArray[pFacet->m_VertexIndex[2]].m_iPntStat = -1;
		break;
	default:break;
	}
	m_FacetList.RemoveAt(pFacet->m_iIndex);
}

//need further test
void XTriMesh::CleanPointArray(int mark)
{
	CArray<TriPoint,TriPoint&> tempPntArray;
	tempPntArray.SetSize(m_iPointNum);
	int * deleteIndex = new int[m_iPointNum];
	int i,j,deletePnt=0;
	for(i=0;i<m_iPointNum;i++)
	{
		if(m_PointArray[i].m_iPntStat==-1)
		{
			deleteIndex[deletePnt] = i;
			deletePnt++;
		}
		else
		{
			tempPntArray.SetAt(i-deletePnt,m_PointArray[i]);
		}
	}
	m_iPointNum -= deletePnt;
	m_PointArray.RemoveAll();
	m_PointArray.Copy(tempPntArray);

	POSITION pos;
	TriFacet * pFacet = NULL;
	TriEdge * pEdge = NULL;
	for(i=deletePnt-1;i>=0;i--)
	{
		pos = m_FacetList.GetHeadPosition();
		while(pos)
		{
			pFacet = &m_FacetList.GetNext(pos);
			for(j=0;j<3;j++)
			{
				if(pFacet->m_VertexIndex[j]>deleteIndex[i])
				{
					pFacet->m_VertexIndex[j]--;
					m_EdgeList.GetAt(pFacet->m_edge[j]).m_VertexIndex[0]--;
				}
			}
		}
	}

	delete []deleteIndex;
}

void XTriMesh::DeleteMarkedFacet(int mark)
{
	TriFacet * pFacet = NULL;
	TriFacet * pFacet1 = NULL;
	int i,j;
	POSITION pos = m_FacetList.GetHeadPosition();
	while(pos)
	{
		pFacet = &m_FacetList.GetNext(pos);
		if(pFacet->m_iFacetType!=mark) continue;
		for(i=0;i<3;i++)
		{
			if(pFacet->m_pNeighbFacet[i]!=NULL)
			{
				pFacet1 = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[i]);
				for(j=0;j<3;j++)
				{
					if(pFacet1->m_pNeighbFacet[j] == pFacet->m_iIndex)
					{
						pFacet1->m_pNeighbFacet[j] = NULL;
						m_EdgeList.GetAt(pFacet1->m_edge[j]).m_iCorrespIndex = NULL;
					}
				}
			}
			m_EdgeList.RemoveAt(pFacet->m_edge[i]);
		}
		m_FacetList.RemoveAt(pFacet->m_iIndex);
	}
}


PNT XTriMesh::CalculateCircumCenter(POSITION facetPos)
{
	if(facetPos==NULL) return PNT(0,0,0);
	TriFacet & triAngle = m_FacetList.GetAt(facetPos);
	PNT facetCenter;
	float x1,x2,x3,y1,y2,y3;
	x1 = m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[0];
	x2 = m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[0];
	x3 = m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[0];
	y1 = m_PointArray[triAngle.m_VertexIndex[0]].m_Coord[1];
	y2 = m_PointArray[triAngle.m_VertexIndex[1]].m_Coord[1];
	y3 = m_PointArray[triAngle.m_VertexIndex[2]].m_Coord[1];

	facetCenter[0] = ((y3-y1)*(y2-y1)*(y2-y3)+(x1*x1-x2*x2)*(y2-y3)+(x2*x2-x3*x3)*(y2-y1))
		/(2*((x1-x2)*(y2-y3)+(x2-x3)*(y2-y1)));

	if(fabsf(y2-y1)>fabsf(y3-y2))
		facetCenter[1] = ((x1-x2)/(y2-y1)*(2*facetCenter[0]-(x1+x2))+y1+y2)/2;
	else
		facetCenter[1] = ((x2-x3)/(y3-y2)*(2*facetCenter[0]-(x2+x3))+y2+y3)/2;
	facetCenter[2] = 0.0;
	return facetCenter;
}

float XTriMesh::CalculateCircumRadius(POSITION facetPos)
{
	if(facetPos==NULL) return -1.0;

	TriFacet & triAngle = m_FacetList.GetAt(facetPos);
	PNT facetCenter(CalculateCircumCenter(facetPos));
	PNT vertex2Center(m_PointArray[triAngle.m_VertexIndex[0]].m_Coord-facetCenter);

	return vertex2Center.Length2D();
}

//length
float XTriMesh::CalculateMaxEdgeLength(POSITION facetPos)
{
	if(facetPos==NULL) return -1.0;
	TriFacet & triAngle = m_FacetList.GetAt(facetPos);
	PNT edge0(m_PointArray[triAngle.m_VertexIndex[1]].m_Coord-m_PointArray[triAngle.m_VertexIndex[0]].m_Coord);
	PNT edge1(m_PointArray[triAngle.m_VertexIndex[2]].m_Coord-m_PointArray[triAngle.m_VertexIndex[1]].m_Coord);
	PNT edge2(m_PointArray[triAngle.m_VertexIndex[0]].m_Coord-m_PointArray[triAngle.m_VertexIndex[2]].m_Coord);

	return sqrt(max(max(edge0*edge0,edge1*edge1),edge2*edge2));
}


void XTriMesh::FiltrateByLength(float length)
{
	if(length<=0) return;
	int facetCount = m_FacetList.GetCount();
	CPosList facetPos;
	TriFacet * pFacet = NULL;
	POSITION pos;
	for(pos=m_FacetList.GetHeadPosition();pos;)
	{
		pFacet = &m_FacetList.GetNext(pos);
		if(CalculateMaxEdgeLength(pFacet->m_iIndex)>length)
		{
			facetPos.AddTail(pFacet->m_iIndex);
		}
	}
	for( pos=facetPos.GetHeadPosition();pos;)
	{
		DeleteFacet(facetPos.GetNext(pos));
	}
	SetVertexesForDraw();
}

void XTriMesh::FiltrateByNormal(const PNT & viewDir,float dotError)
{
	if(dotError<=0) return;

	PNT dir(viewDir);
	int facetCount = m_FacetList.GetCount();
	CPosList facetPos;
	TriFacet * pFacet = NULL;
	POSITION pos;
	for(pos=m_FacetList.GetHeadPosition();pos;)
	{
		pFacet = &m_FacetList.GetNext(pos);
		if(fabsf(dir*pFacet->m_Normal)<dotError)
		{
			facetPos.AddTail(pFacet->m_iIndex);
		}
	}
	for( pos=facetPos.GetHeadPosition();pos;)
	{
		DeleteFacet(facetPos.GetNext(pos));
	}
	SetVertexesForDraw();
}


void XTriMesh::FindPntNeighbFacetAndPnt(int pntIndex,int index,TriFacet * pIniFacet)
{
	int i,j,indexJ;
	int facetCount =  m_FacetList.GetCount();
	TriFacet * pFacet = NULL;

	TriPoint * pPoint = &m_PointArray[pntIndex];
	if(pPoint->m_pNeighbFacetList==NULL)
	{
		pPoint->m_pNeighbFacetList = new CPosList;
	}
	else 
	{
		pPoint->m_pNeighbFacetList->RemoveAll();
	}

	if(pPoint->m_pNeighbPntList==NULL)
	{
		pPoint->m_pNeighbPntList = new CIntList;
	}
	else 
	{
		pPoint->m_pNeighbPntList->RemoveAll();
	}

	bool bFound = false;
	if(pIniFacet==NULL)
	{
		for(i=0;i<facetCount;i++)
		{
			pFacet = &m_FacetList.GetAt(m_FacetList.FindIndex(i));
			for(j=0;j<3;j++)
			{
				if(pFacet->m_VertexIndex[j]==pntIndex)
				{
					bFound = true; 
					break;
				}
			}
			if(bFound) break;
		}
	}
	else 
	{
		bFound = true;
		j = index;
		pFacet = pIniFacet;
	}

	indexJ = j;

	if(!bFound) return;
	POSITION firstPos = pFacet->m_iIndex;
	pPoint->m_pNeighbFacetList->AddTail(firstPos);

	if(pFacet->m_pNeighbFacet[j]==NULL&&pFacet->m_pNeighbFacet[(j+2)%3]==NULL)
	{
		pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+1)%3]);
		pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+2)%3]);
		return;
	}
	else if(pFacet->m_pNeighbFacet[j]!=NULL&&pFacet->m_pNeighbFacet[(j+2)%3]!=NULL)
	{
		bool bCW = false;
		pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+1)%3]);
		pFacet = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[j]);
		while(pFacet->m_iIndex!=firstPos)
		{
			pPoint->m_pNeighbFacetList->AddTail(pFacet->m_iIndex);
			pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+1)%3]);
			for(j=0;j<3;j++)
			{
				if(pFacet->m_VertexIndex[j]==pntIndex)
				{
					break;
				}
			}
			if(j==3)
				break;

			if(pFacet->m_pNeighbFacet[j]==NULL) 
			{
				bCW = true;
				break;
			}
			pFacet = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[j]);
		}
		if(bCW)
		{
			j = indexJ;
			pFacet = &m_FacetList.GetAt(firstPos);
			pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+2)%3]);
			pFacet = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[(j+2)%3]);
			while(true)
			{
				pPoint->m_pNeighbFacetList->AddTail(pFacet->m_iIndex);
				pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+2)%3]);
				for(j=0;j<3;j++)
				{
					if(pFacet->m_VertexIndex[j]==pntIndex)
					{
						break;
					}
				}

				if(pFacet->m_pNeighbFacet[(j+2)%3]==NULL) break;
				pFacet = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[(j+2)%3]);
			}
		}
	}
	else if(pFacet->m_pNeighbFacet[(j+2)%3]==NULL)
	{
		pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+1)%3]);
		pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+2)%3]);
		pFacet = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[j]);
		while(true)
		{
			pPoint->m_pNeighbFacetList->AddTail(pFacet->m_iIndex);
			pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+1)%3]);
			for(j=0;j<3;j++)
			{
				if(pFacet->m_VertexIndex[j]==pntIndex)
				{
					break;
				}
			}

			if(pFacet->m_pNeighbFacet[j]==NULL) break;
			pFacet = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[j]);
		}
	}
	else 
	{
		pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+1)%3]);
		pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+2)%3]);
		pFacet = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[(j+2)%3]);
		while(true)
		{
			pPoint->m_pNeighbFacetList->AddTail(pFacet->m_iIndex);
			pPoint->m_pNeighbPntList->AddTail(pFacet->m_VertexIndex[(j+2)%3]);
			for(j=0;j<3;j++)
			{
				if(pFacet->m_VertexIndex[j]==pntIndex)
				{
					break;
				}
			}

			if(pFacet->m_pNeighbFacet[(j+2)%3]==NULL) break;
			pFacet = &m_FacetList.GetAt(pFacet->m_pNeighbFacet[(j+2)%3]);
		}
	}
}

int XTriMesh::FindHull_2D(PNT * V,int n,PNT * H,int * hullIndex)
{
	PNT * polyLinePnt = new PNT[2*n+1];
	int * indexPolyPnt = new int[2*n+1];
	int bot = n-2, top = bot+3;   
	polyLinePnt[bot] = polyLinePnt[top] = V[2];       
	indexPolyPnt[bot] = indexPolyPnt[top] = 2;
	if(CalculateTriArea(V[0], V[1], V[2])>= 0) 
	{
		indexPolyPnt[bot+1] = 0;
		indexPolyPnt[bot+2] = 1;
		polyLinePnt[bot+1] = V[0];
		polyLinePnt[bot+2] = V[1];          
	}
	else 
	{
		indexPolyPnt[bot+1] = 1;
		indexPolyPnt[bot+2] = 0;
		polyLinePnt[bot+1] = V[1];
		polyLinePnt[bot+2] = V[0];          
	}

	for(int i=3; i<n; i++) 
	{   
		if((CalculateTriArea(polyLinePnt[bot], polyLinePnt[bot+1], V[i])>0) &&
			(CalculateTriArea(polyLinePnt[top-1], polyLinePnt[top], V[i])>=0) )
			continue;        
		else if((CalculateTriArea(polyLinePnt[bot], polyLinePnt[bot+1], V[i])>=0) &&
			(CalculateTriArea(polyLinePnt[top-1], polyLinePnt[top], V[i])>0))
			continue;         

		while(CalculateTriArea(polyLinePnt[bot], polyLinePnt[bot+1], V[i])<0)
			++bot;              

		--bot;
		polyLinePnt[bot] = V[i];        
		indexPolyPnt[bot] = i;

		while(CalculateTriArea(polyLinePnt[top-1], polyLinePnt[top], V[i])<0)
			--top;    

		++top;
		polyLinePnt[top] = V[i];         
		indexPolyPnt[top] = i;
	}

	int hullCount;       
	for(hullCount=0;hullCount<=(top-bot);hullCount++)
	{
		H[hullCount] = polyLinePnt[bot+hullCount];
		hullIndex[hullCount] = indexPolyPnt[bot+hullCount];
	}

	delete []polyLinePnt;
	delete []indexPolyPnt;
	return hullCount-1;
}


void XTriMesh::SetVertexesForDraw()
{
	int count = m_FacetList.GetCount() + m_iPointNum;
	int facetCount = m_FacetList.GetSize();
	if(facetCount==0) return;

	m_bSetVertexesDone = false;
	if(m_pDrawX!=NULL) 
		{
		delete []m_pDrawX ;
		m_pDrawX = NULL;
		}
	if(m_pDrawY!=NULL)
		{
		delete []m_pDrawY ;
		m_pDrawY = NULL;
		}
	if(m_pDrawZ!=NULL) 
		{
		delete []m_pDrawZ ;
		m_pDrawZ = NULL;
		}
	if(m_pDrawIndex!=NULL)
		{
		delete []m_pDrawIndex ;
		m_pDrawIndex = NULL;
		}

	m_pDrawX = new float[count];
	m_pDrawY = new float[count];
	m_pDrawZ = new float[count];
	m_pDrawIndex = new int[3*facetCount];

	int i=0,j;
	TriFacet * pFacet = NULL;	
	for(POSITION pos=m_FacetList.GetHeadPosition();pos;i++)
	{
		if(i== facetCount)break;
		pFacet = &m_FacetList.GetNext(pos);
		m_pDrawX[i] = pFacet->m_Normal[0];
		m_pDrawY[i] = pFacet->m_Normal[1];
		m_pDrawZ[i] = pFacet->m_Normal[2];
		m_pDrawIndex[3*i] = pFacet->m_VertexIndex[0] + facetCount;
		m_pDrawIndex[3*i+1] = pFacet->m_VertexIndex[1] + facetCount;
		m_pDrawIndex[3*i+2] = pFacet->m_VertexIndex[2] + facetCount;
	}
	for(j=0;j<m_iPointNum;j++)
	{
		m_pDrawX[facetCount+j] = m_PointArray[j].m_Coord[0];
		m_pDrawY[facetCount+j] = m_PointArray[j].m_Coord[1];
		m_pDrawZ[facetCount+j] = m_PointArray[j].m_Coord[2];
	}
	m_bSetVertexesDone = true;
}


void XTriMesh::TriMeshSmooth_LA(float alpha,bool bRecalNeighb)
{
	int i=0,j=0,k;
	if(alpha<=0) return;
	else if(alpha>1) 
		alpha = 1;

	CPosList * edgeList = new CPosList[totalCount];

	int count,facetNum;
	TriPoint * pPoint = NULL;
	TriFacet * pFacet = NULL;
	facetNum = m_FacetList.GetCount();
	i = 0; 
	for(POSITION pos=m_FacetList.GetHeadPosition();pos;i++)
	{
		if(i>= facetNum)
			break;
		pFacet = &m_FacetList.GetNext(pos);
		for(k=0;k<3;k++)
		{
			if(m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType==3) continue;
			else
			{
				m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType=3;
				if(bRecalNeighb) 
					FindPntNeighbFacetAndPnt(pFacet->m_VertexIndex[k],k,pFacet);
				pPoint = & m_PointArray[pFacet->m_VertexIndex[k]];
				if(pPoint->m_pNeighbPntList->GetCount()!=pPoint->m_pNeighbFacetList->GetCount())
					continue;
				else
				{
					PNT temp;
					count = pPoint->m_pNeighbPntList->GetCount();
					for(POSITION pos=pPoint->m_pNeighbPntList->GetHeadPosition();pos;)
					{	
						temp += m_PointArray[pPoint->m_pNeighbPntList->GetNext(pos)].m_Coord;
					}
					pPoint->m_Coord = pPoint->m_Coord*(1-alpha) + alpha*temp/count;
				}
			}
		}
	}
	i=0;
	for( POSITION pos=m_FacetList.GetHeadPosition();pos;i++)
	{
		if(i== facetNum)
			break;
		pFacet = &m_FacetList.GetNext(pos);
		for(k=0;k<3;k++)
		{
			m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType=0;
		}
	}
	CalculateMeshNormal();
	SetVertexesForDraw();
	delete []edgeList;
}


void XTriMesh::TriMeshSmooth_HC(float alpha,float omega,bool bRecalNeighb)
{
	int i=0,j=0,k;
	if(alpha<=0) return;
	else if(alpha>1) alpha = 1;

	CPosList * edgeList = new CPosList[totalCount];
	PNT * vDi = new PNT[m_iPointNum];
	PNT * oldPnt = new PNT[m_iPointNum];

	int count,facetNum;
	TriPoint * pPoint = NULL;
	TriFacet * pFacet = NULL;
	facetNum = m_FacetList.GetCount();

	for(k=0;k<m_iPointNum;k++)
	{
		oldPnt[k] = m_PointArray[k].m_Coord;
	}
	POSITION pos;
	for(pos=m_FacetList.GetHeadPosition();pos;)
	{
		pFacet = &m_FacetList.GetNext(pos);
		for(k=0;k<3;k++)
		{
			if(m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType==3) 
				continue;
			else
			{
				m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType=3;
				if(bRecalNeighb)
					FindPntNeighbFacetAndPnt(pFacet->m_VertexIndex[k],k,pFacet);
				pPoint = & m_PointArray[pFacet->m_VertexIndex[k]];
				if(pPoint->m_pNeighbPntList->GetCount()!=pPoint->m_pNeighbFacetList->GetCount())
					continue;
				else
				{
					PNT temp;
					count = pPoint->m_pNeighbPntList->GetCount();
					for(POSITION pos=pPoint->m_pNeighbPntList->GetHeadPosition();pos;)
					{	
						temp += m_PointArray[pPoint->m_pNeighbPntList->GetNext(pos)].m_Coord;
					}
					pPoint->m_Coord = pPoint->m_Coord*(1-alpha) + temp/count*alpha;
					vDi[pPoint->m_iIndex] = pPoint->m_Coord - oldPnt[pPoint->m_iIndex];

					/*pPoint->m_Coord = temp/count;
					vDi[pPoint->m_iIndex] = pPoint->m_Coord - oldPnt[pPoint->m_iIndex];*/
				}
			}
		}
	}

	for( pos=m_FacetList.GetHeadPosition();pos;i++)
	{
		pFacet = &m_FacetList.GetNext(pos);
		for(k=0;k<3;k++)
		{
			m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType=0;
		}
	}

	for( pos=m_FacetList.GetHeadPosition();pos;)
	{
		pFacet = &m_FacetList.GetNext(pos);
		for(k=0;k<3;k++)
		{
			if(m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType==3) continue;
			else
			{
				m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType=3;
				if(bRecalNeighb) 
					FindPntNeighbFacetAndPnt(pFacet->m_VertexIndex[k],k,pFacet);
				pPoint = & m_PointArray[pFacet->m_VertexIndex[k]];
				if(pPoint->m_pNeighbPntList->GetCount()!=pPoint->m_pNeighbFacetList->GetCount())
					continue;
				else
				{
					PNT temp;
					count = pPoint->m_pNeighbPntList->GetCount();
					for(POSITION pos=pPoint->m_pNeighbPntList->GetHeadPosition();pos;)
					{	
						temp += vDi[pPoint->m_pNeighbPntList->GetNext(pos)];
					}
					pPoint->m_Coord = pPoint->m_Coord - (vDi[pPoint->m_iIndex]*(1-omega) + temp/count*omega);
				}
			}
		}
	}

	for( pos=m_FacetList.GetHeadPosition();pos;i++)
	{
		pFacet = &m_FacetList.GetNext(pos);
		for(k=0;k<3;k++)
		{
			m_PointArray[pFacet->m_VertexIndex[k]].m_iPntType = 0;
		}
	}

	CalculateMeshNormal();
	SetVertexesForDraw();
	delete []edgeList;
	delete []vDi;
	delete []oldPnt;
}

void XTriMesh::CalculateMeshNormal()
{
	int i=0;
	int facetnum = m_FacetList.GetCount();
	for(POSITION pos=m_FacetList.GetHeadPosition();pos;i++)
	{
		if(i== facetnum)
			break;
		CalculateOneFacetNormal(m_FacetList.GetNext(pos));
	}
}

void XTriMesh::CalculateOneFacetNormal(TriFacet & triFacet)
{
	//calculate facet normal
	PNT edge0,edge1;
	edge0 = m_PointArray[triFacet.m_VertexIndex[1]].m_Coord
		-m_PointArray[triFacet.m_VertexIndex[0]].m_Coord;
	edge1 = m_PointArray[triFacet.m_VertexIndex[2]].m_Coord
		-m_PointArray[triFacet.m_VertexIndex[1]].m_Coord;
	triFacet.m_Normal = edge0^edge1;
	triFacet.m_Normal.Normalize();
}


void XTriMesh::SetDisplay(bool bShow, int type)
{
	m_bShow = bShow;
	if(type!=-1)
	{
		m_DispType = type;
	}
}

void XTriMesh::SetFacetType(int index,int type)
{
	m_FacetList.GetAt(m_FacetList.FindIndex(index)).m_iFacetType = type;
}


float XTriMesh::CalculatePolygonArea_2D(CPosList & edgeLoop)
{
	float area = 0;
	int i,j,k;
	int count = edgeLoop.GetCount();
	CArray<PNT,PNT &> polyPnt;
	polyPnt.SetSize(count+2);

	POSITION pos=edgeLoop.GetHeadPosition();
	for(i=0;pos;i++)
	{
		polyPnt.SetAt(i,m_PointArray[m_EdgeList.GetAt(edgeLoop.GetNext(pos)).m_VertexIndex[0]].m_Coord);
	}
	polyPnt.SetAt(i,polyPnt[0]);
	polyPnt.SetAt(i+1,polyPnt[1]);

	for(i=1,j=2,k=0;i<=count;i++,j++,k++) 
	{
		area += polyPnt[i][0] * (polyPnt[j][1] - polyPnt[k][1]);
	}
	return area/2.0;
}


float XTriMesh::CalculatePolygonArea_2D(CIntList & pntLoop)
{
	float area = 0;
	int i,j,k;
	int count = pntLoop.GetCount();
	CArray<PNT,PNT &> polyPnt;
	polyPnt.SetSize(count+2);

	POSITION pos=pntLoop.GetHeadPosition();
	for(i=0;pos;i++)
	{
		polyPnt.SetAt(i,m_PointArray[pntLoop.GetNext(pos)].m_Coord);
	}
	polyPnt.SetAt(i,polyPnt[0]);
	polyPnt.SetAt(i+1,polyPnt[1]);

	for(i=1,j=2,k=0;i<=count;i++,j++,k++) 
	{
		area += polyPnt[i][0] * (polyPnt[j][1] - polyPnt[k][1]);
	}
	return area/2.0;
}

float XTriMesh::CalculatePolygonArea_2D(CArray<PNT,PNT &> & polyPnt)
{
	float area = 0;
	int i,j,k;
	int count = polyPnt.GetSize();

	polyPnt.SetSize(count+2);
	polyPnt.SetAt(count,polyPnt[0]);
	polyPnt.SetAt(count+1,polyPnt[1]);

	for(i=1,j=2,k=0;i<=count;i++,j++,k++) 
	{
		area += polyPnt[i][0] * (polyPnt[j][1] - polyPnt[k][1]);
	}
	return area/2.0;
}

bool XTriMesh::IsPntInPolygon(const PNT & pnt,CPosList & edgeLoop)
{
	int i;
	int windCount = 0;  
	int count = edgeLoop.GetCount();
	CArray<PNT,PNT &> polyPnt;
	polyPnt.SetSize(count+1);

	POSITION pos=edgeLoop.GetHeadPosition();
	for(i=0;pos;i++)
	{
		polyPnt.SetAt(i,m_PointArray[m_EdgeList.GetAt(edgeLoop.GetNext(pos)).m_VertexIndex[0]].m_Coord);
	}
	polyPnt.SetAt(i,polyPnt[0]);

	for(i=0;i<count;i++) 
	{   
		if (polyPnt[i][1]<= pnt[1]) 
		{         
			if (polyPnt[i+1][1]>pnt[1])      
				if (CalculateTriArea(polyPnt[i],polyPnt[i+1],pnt)>0) 
					++windCount;           
		}
		else
		{                       
			if (polyPnt[i+1][1]<=pnt[1])    
				if (CalculateTriArea(polyPnt[i],polyPnt[i+1],pnt)<0)  
					--windCount;          
		}
	}
	return windCount&1;
}


float XTriMesh::CalculateImpellerInnerEdgeArea(const PNT & innerPnt)
{
	CPosList * edgeList = new CPosList[totalCount];

	int i,j,k;
	float area(-1);
	POSITION	 pos = NULL;
	int innerEdgeCount = FindMeshInnerEdge(edgeList);

	CPosList * reverseEdgeList = new CPosList[innerEdgeCount];

	for(i=0;i<innerEdgeCount;i++)
	{		
		pos = edgeList[i].GetTailPosition();
		while(pos)
		{
			reverseEdgeList[i].AddTail(edgeList[i].GetPrev(pos));
		}

		if(IsPntInPolygon(innerPnt,reverseEdgeList[i]))
		{
			m_InnerEdge.RemoveAll();
			m_Color = 4;
			m_InnerEdge.AddTail(&reverseEdgeList[i]);

			int topNum = 0;
			int bottomNum = 0;
			int leftNum = 0;
			int rightNum = 0;
			int Num = reverseEdgeList[i].GetCount();
			CArray<PNT,PNT &> polyPnt;
			CArray<PNT,PNT &> top;
			CArray<PNT,PNT &> bottom;
			CArray<PNT,PNT &> left;
			CArray<PNT,PNT &> right;
			polyPnt.SetSize(Num);
			top.SetSize(Num);
			bottom.SetSize(Num);
			left.SetSize(Num);
			right.SetSize(Num);
			XBndBox bndBox;

			POSITION pos = reverseEdgeList[i].GetHeadPosition();
			for(j=0;pos;j++)
			{
				polyPnt.SetAt(j,m_PointArray[m_EdgeList.GetAt(reverseEdgeList[i].GetNext(pos)).m_VertexIndex[0]].m_Coord);
				bndBox.AddVertex(polyPnt[j]);
			}

			float minX = bndBox.GetMin()[0]+2.8;
			float middleY = (bndBox.GetMin()[1]+bndBox.GetMax()[1])/2.0;
			float maxX = bndBox.GetMax()[0]-2.8;

			for(j=0;j<Num;j++)
			{
				if(polyPnt[j][0]>minX&&polyPnt[j][0]<maxX&&polyPnt[j][1]<middleY)
					break;
			}
			for(k=j;k<Num+j;k++)
			{
				if(polyPnt[k%Num][0]>maxX)
					break;
			}
			k = k%Num;
			for(j=k;j<Num+k;j++)
			{
				if(polyPnt[j%Num][0]<maxX)
					break;
				else
				{
					right.SetAt(rightNum,polyPnt[j%Num]);
					rightNum++;
				}
			}
			j = j%Num;
			for(k=j;k<Num+j;k++)
			{
				if(polyPnt[k%Num][0]<minX)
					break;
				else
				{
					top.SetAt(topNum,polyPnt[k%Num]);
					topNum++;
				}
			}
			k = k%Num;
			for(j=k;j<Num+k;j++)
			{
				if(polyPnt[j%Num][0]>minX)
					break;
				else
				{
					left.SetAt(leftNum,polyPnt[j%Num]);
					leftNum++;
				}
			}
			j = j%Num;
			for(k=j;k<Num+j;k++)
			{
				if(polyPnt[k%Num][0]>maxX)
					break;
				else
				{
					bottom.SetAt(bottomNum,polyPnt[k%Num]);
					bottomNum++;
				}
			}

			bndBox.Reset();
			for(j=0;j<topNum;j++)
			{
				bndBox.AddVertex(top[j]);
			}
			PNT topLeft(bndBox.GetMin()[0],bndBox.GetMin()[1],(bndBox.GetMin()[2]+bndBox.GetMax()[2]))
				,topRight(bndBox.GetMax()[0],bndBox.GetMin()[1],(bndBox.GetMin()[2]+bndBox.GetMax()[2]));

			bndBox.Reset();
			for(j=0;j<bottomNum;j++)
			{
				bndBox.AddVertex(bottom[j]);
			}
			PNT bottomLeft(bndBox.GetMin()[0],bndBox.GetMax()[1],(bndBox.GetMin()[2]+bndBox.GetMax()[2]))
				,bottomRight(bndBox.GetMax()[0],bndBox.GetMax()[1],(bndBox.GetMin()[2]+bndBox.GetMax()[2]));

			CArray<PNT,PNT &> total;
			for(i=0;i<left.GetSize();i++)
			{
				if(left[i][1]>bottomLeft[1]&&left[i][1]<topLeft[1])
				{
					total.Add(left[i]);
				}
			}
			total.Add(bottomLeft);
			total.Add(bottomRight);
			for(i=0;i<right.GetSize();i++)
			{
				if(right[i][1]>bottomRight[1]&&right[i][1]<topRight[1])
				{
					total.Add(right[i]);
				}
			}
			total.Add(topRight);
			total.Add(topLeft);
			area = CalculatePolygonArea_2D(total);
			break;
		}
	}
	delete []edgeList;
	delete []reverseEdgeList;
	return area;
}


void XTriMesh::SetInnerEdge()
{
	CPosList * edgeList = new CPosList[totalCount];

	int innerEdgeCount = FindMeshInnerEdge(edgeList);

	for(int i=0;i<innerEdgeCount;i++)
	{	
		m_InnerEdge.AddTail(&edgeList[i]);
	}
	delete []edgeList;
}




void XTriMesh::CircleSelectDelete(XPolyline & hullCld,XCloud & centerCld,int type)
{
	int i;
	PNT projectPnt;
	POSITION pos = m_FacetList.GetHeadPosition();
	CPosList deletePos;
	if(type==0)
	{
		for(i=0;pos;i++)
		{		
			centerCld.GetPoint(i,projectPnt);
			if(!hullCld.IsPntIn(projectPnt))
			{
				deletePos.AddTail(pos);
			}
			m_FacetList.GetNext(pos);
		}
		pos = deletePos.GetHeadPosition();
		while(pos)
		{
			DeleteFacet(deletePos.GetNext(pos));
		}
	}
	else if(type==1)
	{
		for(i=0;pos;i++)
		{		
			centerCld.GetPoint(i,projectPnt);
			if(hullCld.IsPntIn(projectPnt))
			{
				deletePos.AddTail(pos);
			}
			m_FacetList.GetNext(pos);
		}
		pos = deletePos.GetHeadPosition();
		while(pos)
		{
			DeleteFacet(deletePos.GetNext(pos));
		}
	}
}


void XTriMesh::Translate(const PNT & delta_trans)
{
	int i;
	int num = GetPntNum();
	for(i=0;i<num;i++)
	{
		m_PointArray[i].m_Coord = m_PointArray[i].m_Coord + delta_trans;
	}
	SetVertexesForDraw();
	UpdateBndBox();	
}

	void XTriMesh::Rotate(const Ti_Rotation& R)
	{
		Rotate(PNT(0,0,0), R.m_Axis, R.m_Angle*PI/180);
	}
void XTriMesh::Rotate(const PNT  & pnt_axis,	// point on axis
					  const PNT  & dir,		// unit direction vector
					  const double  & angle)		// rotation angle in rad					
{
	double s_thita, c_thita, vers_thita;
	if(fabs(angle) < 1e-6)
		return;
	s_thita = sin(angle);
	c_thita = cos(angle);
	vers_thita = 1- c_thita;
	double  trans[3][3];
	double x, y, z; 
	x = dir[0];
	y = dir[1];
	z = dir[2];

	trans[0][0] = x*x*vers_thita + c_thita;
	trans[0][1] = y*x*vers_thita - z*s_thita;
	trans[0][2] = z*x*vers_thita + y*s_thita;	  
	trans[1][0] = x*y*vers_thita + z*s_thita;
	trans[1][1] = y*y*vers_thita + c_thita;
	trans[1][2] = y*z*vers_thita - x*s_thita;
	trans[2][0] = x*z*vers_thita - y*s_thita;
	trans[2][1] = y*z*vers_thita + x*s_thita;
	trans[2][2] = z*z*vers_thita + c_thita;

	int num = GetPntNum();
	PNT pnt;
	for(int i=0; i< num; i++)
	{
		pnt = m_PointArray[i].m_Coord;
		pnt = pnt - pnt_axis;
		m_PointArray[i].m_Coord[0] = trans[0][0]*pnt[0] +
			trans[0][1]*pnt[1] +
			trans[0][2]*pnt[2];
		m_PointArray[i].m_Coord[1] = trans[1][0]*pnt[0] +
			trans[1][1]*pnt[1] +
			trans[1][2]*pnt[2];
		m_PointArray[i].m_Coord[2] = trans[2][0]*pnt[0] + 
			trans[2][1]*pnt[1] + 
			trans[2][2]*pnt[2];
		m_PointArray[i].m_Coord = m_PointArray[i].m_Coord + pnt_axis;
	}
	SetVertexesForDraw();
	UpdateBndBox();	
}


void XTriMesh::AlignAxisZtoVector(const PNT & newDirection,XCloud & inputCloud,XCloud & newCloud)
{
	int i;
	float x[3];
	PNT axis_z(newDirection),tempPnt,cldPnt;

	for(i=0;i<3;i++)
	{
		if(fabs(newDirection[i])>error)
			break;
	}
	if(i==3) return;

	x[i] = newDirection[(i+1)%3] + newDirection[(i+2)%3];
	x[(i+1)%3] = x[(i+2)%3] = -newDirection[i];
	PNT axis_x(x),axis_y;

	axis_x.Normalize();
	axis_z.Normalize();

	axis_y = axis_z^axis_x;
	axis_y.Normalize();
	axis_x = axis_y^axis_z;
	axis_x.Normalize();

	int count = inputCloud.GetPntNum();
	for(i=0;i<count;i++)
	{
		inputCloud.GetPoint(i,tempPnt);

		cldPnt[0] = axis_x[0]*tempPnt[0] + axis_x[1]*tempPnt[1] + axis_x[2]*tempPnt[2];
		cldPnt[1] = axis_y[0]*tempPnt[0] + axis_y[1]*tempPnt[1] + axis_y[2]*tempPnt[2];
		cldPnt[2] = axis_z[0]*tempPnt[0] + axis_z[1]*tempPnt[1] + axis_z[2]*tempPnt[2];
		newCloud.AddPoint(cldPnt);
	}
	newCloud.SetObjState( true);
}


void XTriMesh::ChangeUnits(bool bMm2Inch)
{
	int i,j;
	if(bMm2Inch)
	{
		for(i=0;i<m_iPointNum;i++)
		{
			m_PointArray[i].m_Coord /= 25.4f;
		}
	}
	else
	{
		for(i=0;i<m_iPointNum;i++)
		{
			m_PointArray[i].m_Coord *= 25.4f;
		}
	}
	//add 'set array' code here
	SetVertexesForDraw();
	m_bObjChanged = true;
	int facetCount = m_FacetList.GetCount();
	for(j=0;j<m_iPointNum;j++)
	{
		m_pDrawX[facetCount+j] = m_PointArray[j].m_Coord[0];
		m_pDrawY[facetCount+j] = m_PointArray[j].m_Coord[1];
		m_pDrawZ[facetCount+j] = m_PointArray[j].m_Coord[2];
	}
}


void XTriMesh::FindOverlapRegion(XTriMesh & triMeshB)
{
	//set binCell

	float binLength = 1e-1;
	int xNum,yNum,zNum;
	XBndBox newBndBox = GetBBox() + triMeshB.GetBBox();
	PNT min = newBndBox.GetMin();
	PNT max = newBndBox.GetMax();
	PNT temp(max-min);
	xNum = temp[0]/binLength + 1;
	yNum = temp[1]/binLength + 1;
	zNum = temp[2]/binLength + 1;
	int binNum = xNum*yNum*zNum;

	XBinBox * binArray = new XBinBox[binNum];

	int i,x,y,z;
	PNT facetCenter;
	TriFacet * pFacet = NULL;
	POSITION pos = m_FacetList.GetHeadPosition();
	POSITION oldPos;
	while(pos)
	{
		oldPos = pos;
		pFacet = &m_FacetList.GetNext(pos);
		facetCenter = (m_PointArray[pFacet->m_VertexIndex[0]].m_Coord + m_PointArray[pFacet->m_VertexIndex[1]].m_Coord
			+ m_PointArray[pFacet->m_VertexIndex[2]].m_Coord)/3;
		x = (facetCenter[0]-min[0])/binLength;
		y = (facetCenter[1]-min[1])/binLength;
		z = (facetCenter[2]-min[2])/binLength;
		binArray[z*xNum*yNum+y*xNum+x].m_InnerFacetsA.AddTail(oldPos);
	}
	pos = triMeshB.m_FacetList.GetHeadPosition();
	while(pos)
	{
		oldPos = pos;
		pFacet = &triMeshB.m_FacetList.GetNext(pos);
		facetCenter = (triMeshB.m_PointArray[pFacet->m_VertexIndex[0]].m_Coord + triMeshB.m_PointArray[pFacet->m_VertexIndex[1]].m_Coord
			+ triMeshB.m_PointArray[pFacet->m_VertexIndex[2]].m_Coord)/3;
		x = (facetCenter[0]-min[0])/binLength;
		y = (facetCenter[1]-min[1])/binLength;
		z = (facetCenter[2]-min[2])/binLength;
		binArray[z*xNum*yNum+y*xNum+x].m_InnerFacetsB.AddTail(oldPos);
	}

	for(i=0;i<binNum;i++)
	{
		if(binArray[i].m_InnerFacetsA.GetCount()>0&&binArray[i].m_InnerFacetsB.GetCount()>0)
		{
			pos = binArray[i].m_InnerFacetsA.GetHeadPosition();
			while(pos)
			{
				pFacet = &m_FacetList.GetAt(binArray[i].m_InnerFacetsA.GetNext(pos));
				pFacet->m_iFacetType = 8;
			}
			while(pos)
			{
				pFacet = &triMeshB.m_FacetList.GetAt(binArray[i].m_InnerFacetsB.GetNext(pos));
				pFacet->m_iFacetType = 8;
			}
		}
	}
	//set end
	delete []binArray;
}