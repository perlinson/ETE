///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//			file name : xcloud.h   
//
//			File Description : 
//				point cloud related to optical measure data. 
//                     It covers 
//                          -importing and exporting 
//							 - data reduction
//                          - add point and delete points 
//                          - basic geometry fitting 
//				if your measure data has its own property, you can drive from this class and add your own property data members
//			created by Xiaoming Du 10/26/2004
//
//			Modification Record:	
//                        Xiaoming Du   Dec 5, 2004   add point reduction functions and plane fitting cloud rigid transformation
//															add data member m_BinArray and m_iBinNum for data space reduction
//						  Xiaoming Du   Jan 5, 2005    add cloud smoothing, which is the revised version from Honggen Luo.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once
#include "geometry.h"
#include "xgeombase.h"
#include "pnt.h"
#include "XCircle.h"
#include "XArc.h"
#include "XLine.h"
#include "tolzone.h"
#include "ti_matrix.h"
#include "Ti_Optimization.h"

#include "QuParaSur.h"              // definition of QuParaSur class

#include <vector>
using namespace std;

typedef CList<int,int> CIntList;
typedef CList<POSITION,POSITION> CPosList;

class XBinBox
{
public:
	XBinBox(void);
	~XBinBox(void);
public:
	CIntList m_InnerPoints;
	CPosList m_InnerFacetsA;
	CPosList m_InnerFacetsB;
	PNT  m_CenterPoint;
	bool m_bNoEmpty;
};

class AFX_EXT_CLASS  XCloud :
	public XGeomBase

{
protected:
	vector<PNT>				m_Point;
	vector<float>			m_SNR;
	vector<float>			m_Total;
	int						m_iSampleCoef;
	int						m_PointSize;
	int						m_bRawdata;

	int                     m_cldwidth;
	int                     m_cldheight;
public:
	XBinBox *				m_BinArray;
	int						m_iBinNum;	

public:
	XCloud(void);
	XCloud(long aNum);
	XCloud(const XCloud& aXCloud);
	virtual ~XCloud(void);

	void UpdateBndBox();
	void SetUnit(const int& unit);

	XCloud&  operator=( const XCloud& SourceCld );
	XCloud&  operator+( const XCloud& SourceCld );
	void InitObject(void);
	void SetPointSize(int size);
	const  PNT&  operator[](int index)const;
	PNT&  operator[](int index);
	const int& GetSampleCoef()const;
	void SetSampleCoef(const int& newcoef);
	int GetPointSize(void)const;
	int  GetPntNum(void)const;
	void GetPoint(const int& index, PNT& aVector3)const;
	void SetCapacity(int num);

	void SetCldSize(int width, int height);

	virtual BOOL SaveFile_Asc(CArchive &ar)const;
	BOOL SaveFile_Txt(CArchive &ar)const;
	virtual BOOL SaveFile_Asc(FILE*&  savedfile)const;

	virtual BOOL LoadFile_Asc(CArchive &ar);
	virtual BOOL LoadFile_Dat(CArchive &ar);

	/*------------------------------------------------------------------
	Txt file should be like following format:
	61.295000,52.560952,-91.633882,250,630,
	61.295000,52.561002,-91.639975,262,631,
	61.295000,52.561002,-91.687886,196,633,
	61.295000,52.561052,-91.630320,234,631,
	61.295000,52.561102,-91.645391,232,632,
	61.295000,52.561102,-91.598423,239,631,
	61.295000,52.561102,-91.643805,244,630,
	61.295000,52.561102,-91.639711,273,631,
	61.295000,52.561102,-87.721077,116,631,
	61.295000,52.561152,-91.640360,273,630,
	61.295000,52.561202,-91.635268,265,631,
	61.295000,52.561202,-91.588599,284,630,
	----------------------------------------------------------
	x_cord, y_cord, z_cord, snr value, total value,
	------------------------------------------------------------------*/
	BOOL LoadFile_Txt(CArchive &ar);

	BOOL LoadFile_Mini_Txt(CArchive &ar);
	void AddPoint(const  PNT& aVector3 );
	void AddPoint( PNT& aVector3, float aSNR, float aTotal);	
	void DeletePoint(const int& startindex, const int & num);

	// cloud data reduction
	void SampleUniform(int interval_num);
	void SamplePercentage(int percent);
	void SampleSpace(float distance);
	void AdaptiveFilter(double sampedist, double angle);


	/**The Calculator of Smothing_ANID() is to remove and smoothing the 
	noisy data, and its underling principle bases on the heat 
	diffusion equation. The details may refer reader to the following
	paper:
	Perna and Malik: Scale-space and edge detection using anisotropic
	diffusion IEEE Trans. on PAM
	Parameter:smoothing_dir is the data capture direction, */
	int Smoothing_ANID(const PNT& smoothing_dir = PNT(0,0,1));

	/*The funciton of Para_K() is to solution the parameter K to
	determine the degree the diffusion*/
	int Smoothing_ANID(double* array, int pts_num,int axis,int iter);
	double Para_K();
	void data_smoothing(int iter=100);



	// rigid transformation
	void Translate(const PNT& delta_trans);
	void Rotate(
		const PNT			& pnt_axis,	 // point on axis
		const PNT			& dir,			// unit direction vector
		const double		& angle		// rotation angle in rad
		);
	void Rotate(const Ti_Rotation& R);
	// cloud initial bincell
	void InitialBin(float & binLength);
	void InitialBin2D(int numX,int numY);
	void InitialBin2D(int num);
	void CloudInBin2D(int i,XCloud & cld);
	friend int __cdecl SortCompareX(const PNT&  elem1, const PNT&  elem2);
	friend int __cdecl SortCompareY(const PNT&  elem1, const PNT&  elem2);
	friend int __cdecl SortCompareZ(const PNT&  elem1, const PNT&  elem2);

	PNT GetMassCenter(void);
	bool Slicing( PNT & section_pnt,  PNT & section_dir, double tolerance , XCloud& section_cld);
	void Sort(int axis_index);
	void SortByDir(PNT& dir);

	//jm
	double Fit2Plane(PNT& pnt_plane, PNT& norm_dir) const ;
	double Fit2Circle(XCircle &circlefit);
	double Fit2Arc(XArc &Arcfit);
	double Fit2CircleNew(XCircle &circlefit, int s, int e);
	double Fit2XYCircle(XCircle &circlefit, int s, int e);
	double normal_fit_circle(XCircle &circlefit, int s, int e);
	double normal_fit_circle(XCircle &circlefit, double **m_Point, int n);
     //yb
	double Fit2PlanePointRejection(PNT& PntPlane, PNT& DirPlane, double Tolerance, double& PointRatio) const; 
	double Fit2Cylinder(PNT& axis_pnt, PNT& axis_dir, double& radius,PNT& bottom_center,PNT& top_center,bool bFixedRadius= false)const;

	double Fit2Cylinder_Outlier(PNT& axis_pnt,PNT& axis_dir,double& radius,PNT& bottom_center,PNT& top_center,bool bFixedRadius = false);


	double Fit2Quadratic(double A[],bool bReject,int& RFlag )const;
	void     CFit2Quadratic(double A[]);
    
	void CalCurvature(float *CldCurvature);
	// chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
	//     Attention this cloud should be allocated on X-Y plane:
	//     Output: H[] = an array of the convex hull vertices (max is n)
	//     Return: the number of points in H[]
	int ChainHull_2D(XCloud& H );


	//2005,09,23 - jm
	//this is a another circle fitting method 
	double Fit2DCircle(XCircle &circlefit, int s , int e);
	//2005,09,23 end - jm 

	//jm
	double Fit2Line(PNT& pnt_line, PNT& direction);
	double Fit2Line(XLine &fittedLine);
	double Fit2Line(XLine &fittedLine,int s, int e);
	double Fit2LineNEW(XLine &fittedLine,int s, int e);

	void CloudTransform(XCloud OrgCld,Ti_Matrix<double> matrix); //transform the cloud

	void SortByDistance();

	void AutoTrim(int box, bool ask);
	int AutoTrim(int box, bool ask,int linetrimstep);  //n added sth for auto trim//Jan19


	void AverageFilter();

	//by JM
	CArray<XGeomBase*, XGeomBase*>	m_pAnalysis;
	void	ViewAnalysis(bool bflag);
	bool	m_bShowAnalysis;
	bool    ClearAnalysis();
	void	ViewTolZone(bool bflag);
	bool    m_bShowTolZone;
	bool    ClearTolZone();
	void	ResetPntColor(bool bf=0);
	void    ResetPntSize(int bf=1);
	void    EstimatedSampleDistance();

	TolZone	m_tolzone;
	int		iBreakS,iBreakE;
	bool	m_bAnalysis;
	bool    m_bTolzone;
	double  sample_dist;
	double  sample_dist1;
	double  sample_dist2;

	//To judge whether a point is located inside or outside the tolerance zone
	bool JudgeTopologyEdgebreakTolzone(int sIndex, int eIndex, int edgeBreakType, int outNumber);
	bool JudgeTopologyPntTolzone(PNT P, int edgeBreakType);	
	bool JudgeTopologyPntTriangle(PNT P, PNT P1, PNT P2, PNT P3);
	bool JudgeTopologyPntCircle(PNT P, XCircle C);
	bool JudgeTopologyPntLineSegmentInOut(PNT P, PNT P1, PNT P2);
	bool JudgeTopologyPntLineLeftRight(PNT P, PNT Ref, PNT P1, PNT P2);
	//End of judgement

	//by jm Feb 7 2006 *******************************//
	bool CalBox(int start, int end, double bl[3], double ur[3]);
	double CalRMStoLine(int start, int end, XLine line);
	double CalRMStoCircle(int start, int end, XCircle circle);
	bool   m_bShowROIRect;
	bool  SaveFile_Asc(CString FilePath);


	};

