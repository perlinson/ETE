/********************************************************************
	created:	2004/08/08
	created:	8:8:2004   8:44
	filename: 	f:\share\Yangph\NewPlatform\Include\Geometric\XTriMesh.h
	file path:	f:\share\Yangph\NewPlatform\Include\Geometric
	file base:	XTriMesh
	file ext:	h
	author:		Yang,Pinghai
	
	purpose:	Triangle mesh model construction 
				& multi-view data integration
				//			Modification Record:	
						Dec 5, 2004 Xiaoming Du revised for data structure change remove the draw Calculator
*********************************************************************/
#pragma once
#include <afxtempl.h>
#include "pnt.h"
#include "XCloud.h"
#include "XMesh.h"
#include "geometry.h"
#include "xgeombase.h"
#include "xpolyline.h"

typedef CList<int,int> CIntList; 
typedef CArray<int,int> CIntArray; 
typedef CList<POSITION,POSITION> CPosList; 
typedef CArray<POSITION,POSITION> CPosArray; 

class AFX_EXT_CLASS  XTriMesh :
	public XGeomBase
{
	//member class
public:
	class TriFacet
	{
	public:
		int					m_iFacetType;	    // -1: will be delete; 0: normal;  8: for overlap; 9:
		int					m_VertexIndex[3];// vertex in m_PointArray index
		POSITION			m_edge[3];
		POSITION			m_iIndex;			// facet index
		POSITION			m_pNeighbFacet[3];  // the triangle index which share edge i
		PNT			m_Normal;			
		//member Calculator
		TriFacet& operator=(const TriFacet& facet);
		bool  operator==(const TriFacet & facet);
		TriFacet(void);	
		~TriFacet(void);	
	};
	class TriEdge
	{
	public:
		int						m_iEdgeType;        //edge type -1: will be delete 0: normal
		int						m_VertexIndex[2];   //vertex in m_PointArray index
		POSITION				m_iIndex;			
		POSITION				m_iCorrespIndex;	//
		POSITION				m_iNeighbFacet;		// facet index
		//member Calculator
		TriEdge(void);
		~TriEdge(void);
		TriEdge& operator=(const TriEdge& edge);
		bool  operator==(const TriEdge & edge);
	};

	class TriPoint
	{
	public:
		void Release();
		TriPoint(void);
		~TriPoint(void);
		TriPoint & operator=(const TriPoint & point);
	public:
		int						m_iIndex;                    //point index
		int						m_iPntType;              //point type
		int						m_iPntStat;                //point status:  -1 represent  delete Point , others for tag point;

		PNT						m_Coord;		      //point 3D Coordinate 
		CPosList *				m_pNeighbFacetList;//point Neighbor Facet
		CIntList *				m_pNeighbPntList;   //point Neighbor point
	};	

//member variable
private:
	//int m_facetNum;
	//int m_edgeNum;	
	bool								m_bSetVertexesDone;  //Show the TriMesh could be draw or not 
	int								m_iPointNum;         //Points count of TriMesh
	int								m_iSeedIndex;        //The index of Binbox Seed
	int								m_iSeedCount;        //Count of Binbox Seed
	int								m_iPnt2SeedCount;    //Count of Point to BinSeed
	int *								m_pPnt2Seed;         //Array for Point to BinSeed
	POSITION *						m_pSeed;             //Array for Binbox Seed

	CPosList							m_OuterEdge;         //List of Mesh outer edge
	CPosList							m_InnerEdge;         //List of Mesh inner edge
	CList<TriFacet,	TriFacet&>		m_FacetList;         //List of Mesh facet
	CList<TriEdge,	TriEdge&>		m_EdgeList;          //List of Mesh edge
	CArray<TriPoint,TriPoint&>		m_PointArray;        //Array of Mesh point
	PNT						m_ViewDirection;     //Triangulation direction

	//for draw
	float *							m_pDrawX;            //x coordinate for draw
	float *							m_pDrawY;            //y coordinate for draw
	float *							m_pDrawZ;            //z coordinate for draw
	int *								m_pDrawIndex;        //index array for draw
	
	//member Calculator
public:
	//constructor Calculator
	XTriMesh(void);
	XTriMesh(CString meshName);
	XTriMesh(const XTriMesh & triMesh);
	XTriMesh & operator=(const XTriMesh & triMesh); 
	~XTriMesh(void);
	virtual void SetUnit(const int& unit);
	//Update the Bound box of the Mesh
	void UpdateBndBox();
	//Initiate the TriMesh
	void InitObject();

	//This three functions are defined for adding an inner point to mesh
	bool AddInnerPntToTriMesh(const TriPoint & pnt);
	void AddInnerPntToTriMeshSeed(const TriPoint & pnt);
	void AddInnerPntToTriMeshIntegration(int index);

	//Add two meshes together without other operation
	void Add(XTriMesh & triMeshB);
	//Add two meshes together and join them
	void AddWithJoin(XTriMesh & triMeshB);
	//Coordinate frame transformation
	void AlignAxisZtoVector(const PNT & newDirection,XCloud & inputCloud,XCloud & newCloud);

	//Calculate all facet normals of Mesh
	void CalculateMeshNormal();
	//A tip for Impeller Data
	float CalculateImpellerInnerEdgeArea(const PNT & innerPnt);

	//Change the  Unit of Mesh(from mm to inch,or from inch to mm)
	void ChangeUnits(bool bMm2Inch);
	//Delete all of the useless points, and cut down the length of the point Array
	void CleanPointArray(int mark=-1);

	//0 keep inner;1 keep outer;
	void CircleSelectDelete(XPolyline & hullCld,XCloud & centerCld,int type);

	//This two functions is defined for inner point delete
	bool DeletePnt(int index,bool bDelete=false,int delTag=-1);
	bool DeletePntForIntegration(int index,PNT & viewDir,int delTag=-1);

    //This four functions is used for delete facets of the mesh
	//And especially, the last one is used in Mesh Integration
	void DeleteFacet(POSITION facetPos);
	void DeleteFacet(int facetIndex);
	void DeleteMarkedFacet(int mark);
	void DeleteOverlayRegion(XTriMesh & triMeshB,PNT & viewDir);



	//Draw the Mesh by OpneGL
//	void Draw();

	//Fill holes in Mesh,three method
	void FillPolygonHole(CPosList & holeEdgeList);
	void FillWithoutInitial(CPosList& edgeLoop);
	void FillForIntegration(CPosList& edgeLoop);
	
	//Delete bad facets in Mesh,one by length, another by normal deviation
	void FiltrateByLength(float length);
	void FiltrateByNormal(const PNT & viewDir,float dotError = 0.2);

    //Establish the neighbor net of point, find the neighbor of a point
	void FindPntNeighbFacetAndPnt(int pntIndex,int index,TriFacet * pIniFacet=NULL);

	//This three functions are used for Mesh Integration & Join
	void JoinTwoMesh(CPosList & edgeListA,CPosList & edgeListB);
	void JoinForIntegration(CPosList & edgeListA,CPosList & edgeListB,PNT & viewDir=PNT(0,0,1));
	void IntegrateTwoMesh(XTriMesh & triMeshB);
    
	//This two functions are used for Mesh Transformation:Rotate and Translate
	void Rotate(const PNT	& pnt_axis,const PNT & dir,const double & angle);
	void Translate(const PNT & delta_trans);
    void Rotate(const Ti_Rotation& R);

	//Get Point Number and Facet Number of Mesh
	int GetPntNum() const;
	int GetFacetNum() const;
	
	//Get the Point Array
	const CArray<TriPoint,TriPoint&> & GetPointArray() const;

	//Set the Mesh parameters
	void SetDisplay(bool bShow, int type=-1);
	void SetFacetType(int index,int type);
	void SetInnerEdge();	
	void SetVertexesForDraw();
	void SetBoolDrawState(bool state);
	void SetPointArray(CArray<TriPoint,TriPoint&> & inputArray);

	//Two methods for Mesh Smooth
	void TriMeshSmooth_LA(float alpha,bool bRecalNeighb = false);
	void TriMeshSmooth_HC(float alpha,float omega,bool bRecalNeighb = false);

	//Translate the Mesh type from TriMesh to XMesh
	void TriMesh2XMesh(XMesh & outputMesh);

	//Project  the Mesh along the given direction
	void TriMeshProjection(const PNT& projdir, XTriMesh & outputMesh);

	//type 0,center points;type 1 vertex points;
	void TriMesh2XCloud(XCloud & outputCloud,int type=0);


	//Polygon Mesh construction, four relative functions
	void XCloud2TriMesh(XCloud & inputCloud);
	void XCloud2TriMesh(const PNT & newDirection,XCloud & inputCloud);
	void XCloud2TriMesh_2D(XCloud & inputCloud,int * index=NULL);
	void XCloud2TriMesh_Hybrid2D(XCloud & inputCloud,int splitNum = 2);

private:
    
	//These two functions used for circumcircle Radius and Center calculation
	float CalculateCircumRadius(POSITION facetPos);
	PNT CalculateCircumCenter(POSITION facetPos);

	//Four functions for defining the influence area of a point
	void CalculateInfluRegion(const TriPoint & pnt,CPosList & facetInRegion);
	void CalculateInfluAndDelete(const TriPoint & pnt,CPosList & outerEdge);
	void CalculateInfluAndDeleteSeed(const TriPoint & pnt,CPosList & outerEdge);
	void CalculateInfluAndDeleteIntegration(const TriPoint & pnt,CPosList & outerEdge);

	//Find the Max Length of Mesh edge
	float CalculateMaxEdgeLength(POSITION facetPos);

    //Two functions to calculate the minimum Angle of a triangle,different only in input parameter
	inline double CalculateMinAngle(const TriFacet & triAngle) const;
	inline double CalculateMinAngle(const PNT & v0,const PNT & v1,const PNT & v2) const;

	//Calculate the normal of One Facet
	void CalculateOneFacetNormal(TriFacet & triFacet);

	//Calculate the area of a Polygon, three functions,different in input
	float CalculatePolygonArea_2D(CPosList & edgeLoop);
	float CalculatePolygonArea_2D(CIntList & pntLoop);
	float CalculatePolygonArea_2D(CArray<PNT,PNT &> & polyPnt);

	//Calculate triangle area, two functions, different in input
	inline double CalculateTriArea(const TriFacet & triAngle) const;
	inline double CalculateTriArea(const PNT & v0,const PNT & v1,const PNT & v2) const;

	//Clear the Mesh,Initiate all the parameters
	void Clear();

	//Calculator used in Polygon hole Fill
	void EarCutting(CIntList & pointLoop,CPosList & edgeLoop);


	//Judge two edge intersect or not, two functions different in input
	/* 0----Intersection dosn't exists                                                 */
	/* 1----Intersection exists.                                                       */
	/* 2----two line segments are parallel.                                            */
	/* 3----two line segments are collinear, but not overlap.                          */
	/* 4----two line segments are collinear, and share one same end point.             */
	/* 5----two line segments are collinear, and overlap.                              */    
	int EdgeIntersection(const PNT & startPnt0,const PNT & endPnt0,
		const PNT & startPnt1,const PNT & endPnt1) const;
	int EdgeIntersection(POSITION edgeA,POSITION edgeB) const;

	//Three functions used to find Mesh Inner and outer Boundary
	int FindMeshInnerEdge(CPosList * edgeList);
	int FindMeshOuterEdge(CPosList * edgeList);
	int FindMeshOuterEdgeDir(CPosList * edgeList);

    //Find a triangle contain the point, Three functions
	POSITION FindPntOuterFacet(const TriPoint & pnt);
	POSITION FindPntOuterFacetSeed(const TriPoint & pnt);
	POSITION FindPntOuterFacetIntegration(const TriPoint & pnt);

	//Find the overlapped region of two Meshes
	void FindOverlapRegion(XTriMesh & triMeshB);

	//Construct a edge Loop from giving Edges, two functions
	bool InitialPolygonLoop(CPosList & edgeList,CIntList & pointLoop,CPosList & edgeLoop);
	void InitialPolygonLoop(CPosList & edgeList,CPosList & edgeLoop);


	//Detect the giving edge valid or not
	bool IsEdgeValidate(POSITION edgePos);

	//Check the Boundary type: true for Outer edge; false for Inner edge, two functions
	bool IsOuterEdge(CPosList & edgeLoop);
	bool IsOuterEdgeDir(CPosList & edgeLoop);

	//Check a point in the Triangle or not: true for in; false for out, two functions
	inline bool IsPntInFacet(const TriPoint & pnt,const TriFacet & triAngle) const;
	inline bool IsPntInFacet(const PNT & v0,const PNT & v1,const PNT & v2) const;

    //Check a point in the triangle circumcircle or not: true for in; false for out
	inline bool IsPntInfluRegion(const TriPoint & pnt,const TriFacet & triAngle) const;

	//Check a point in the Polygon or not: true for in; false for out
	bool IsPntInPolygon(const PNT & pnt,CPosList & edgeLoop);

	//Check a point overlap the triangle or not: true for overlap; false for not overlap
	//In fact this Calculator is a 3-D "IsPntInFacet" Calculator
	bool IsPntOverlapFacet(CArray<TriPoint,TriPoint&> & pntArray,const TriFacet & triAngle,float heightFact=0.01);

	//Check the Triangle counter clockwise or not from Z direction: true for ccw; false for cw, two functions
	inline bool IsTriangleCCW(const TriFacet & triAngle) const;
	inline bool IsTriangleCCW(const PNT & v0,const PNT & v1,const PNT & v2) const;

	//Tangent lines between two Hull Polygons, three functions
	int LeftTangentPnt(PNT & P, int n, PNT * V);
	void LeftAndRightTangentPnt( int m, PNT * V, int n, PNT *  W, int & t1, int & t2);
	int RightTangentPnt(PNT & P, int n, PNT * V);

	//Test the edge is valid or not, then modify the invalid edge
	bool SwapEdgeWithTest(POSITION edgePos,POSITION newEdgePos[4]);

	//Find the Hull of the input points, for PolyLine points only
	int FindHull_2D(PNT * V,int n,PNT * H,int * hullIndex);
};