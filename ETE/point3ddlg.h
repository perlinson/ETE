#pragma once
#include "stdafx.h"
#include "pnt.h"
#include "ti_rotation.h"
#include "Ti_Trackball.h"
#include "CPoint3D.h"
#include "Point3DLoader.h"


// CDlgHeightValue 对话框

typedef struct tagRGBFloat{
	float frgbBlue;
	float frgbGreen;
	float frgbRed;
	float frgbReserved;
} RGBFloat;


class CDlgHeightValue : public CDialog
{
	DECLARE_DYNAMIC(CDlgHeightValue)

public:
	static void sItemUpdate(CDlgHeightValue *obj, LPARAM data1, int sValue, BOOL IsDragging);


	CDlgHeightValue(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHeightValue();


public:





	//////////////////////////////////
	HBITMAP CopyScreenToBitmap(LPRECT lpRect); 
	int SaveBitmapToFile(HBITMAP hBitmap, CString lpFileName);
	/////////////////////////////////


	// 对话框数据
	enum { IDD = IDD_DLGHEIGHTVALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	
	int m_DisplayType;

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnBtnLoadtexture();
	afx_msg	void OnBnClickedButton3();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedSliceshow();
	afx_msg void OnBnClickedSurshow();
	afx_msg void OnBnClickedZoomin();
	afx_msg void OnBnClickedZoomout();
	afx_msg void OnEnUpdateEdit15();
	afx_msg void OnEnSetfocusEdit15();
	afx_msg void OnEnChangeEdit15();
	afx_msg void OnBnClickedNextcor();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedDiareset();
	afx_msg void OnBnClickedRevsur();
	afx_msg void OnBnClickedDismeas();
	afx_msg void OnBnClickedSnap();
	afx_msg void OnLoadTexture();

private:

public:
	void OnBnClickedDeepest();

public:

	void CreateRGBPalette(HDC hDC);
protected:
	HGLRC					m_hRC;				// OpenGL 的 RC Handle 
	HDC						m_hDC;				// Window 的 DC Handle 
	HPALETTE				ghpalOld, ghPalette;
private:
	BOOL GetRawRGBData( LPCTSTR filename );
	void ApplyTexture();
	template<typename elemType> 
	void AttachEqualMark(EqualMark &pEqualMark, elemType num1, elemType num2, 
		int & pEqualNum, CList< EqualMark,EqualMark > & plEqualMark);
	void RefreshAll();
	void VertexTransform_WindowToObject(PNT& pnt);
	void VertexTransform_ObjectToWindow(PNT& pnt);
	void TargetPit(CPoint point, int & num);
	void Init();
	void InitTerreno(void);
	PNT GetScreenPnt(const CPoint& point);
	void GetSurRect();


	void DrawSurface(void);
	void DrawYSlice();
	void DrawXSlice();
	////////////////////////////////////////////////////////////////////////////////
	void DrawHistogram();

	void DrawVertex(UINT vertexIndex, UINT colorIndex);
	void delviewport(int x,int y,int w,int h);
	BOOL SetupPixelFormat();
	void SetLogicalPalette(void);
	////////////////////////////////////////////////////////////////////////////////
	void ItemUpdate(LPARAM data1, int sValue, BOOL IsDragging);

	long MarkSurface();
	void DataDis();
	void MapZoom();

	int SolveQuadratic( double c1, double c2, double c3, 
		double *r1, double *r2, int & num_roots );
	int SolveLinear( double c2, double c3, double *r1, int & num_roots );

public:
	void SetDisplayRect(int width, int height);
	void EnableShow3D();
	void EnableShowSlice();
	void EnableShowTexture();
	void InitializeSysAllParameters();
	void LayoutAllSysComponents();
	BOOL InitializeOpenGLContent();
private:
	CDC*    m_pDC;
	CDC*	m_ppDC;//显示窗口DC
	CDC*	m_xsliceDC;
	CDC*	m_ysliceDC;
	CDC*	m_surfacehisDC;
	CPen*	pOldPen;

	CPoint	pt1,pt2;
	CPoint	m_MousePoint;//mouse button  point
	CPoint	m_LeftDownPos;
	CPoint	m_RightDownPos;

	PNT		m_PointFromView1;
	PNT		m_PointFromView2;
	PNT		TopLeft,BottomRight;
	PNT		Axis_dir,Axis_pnt;
	PNT		m_SceneCenter;	// the center point of the whole displayed scene
	PNT		m_SceneVolume;	// the center point of the whole displayed scene
	int		i,j;
	int		PitsNum;
	int		RevSurFlag;
	int		maxRange;
	int		CurPitNum;
	int		StartX,StartY;
	int		DisplaySize;
	int		Width,Height;
	int		MaxPosX,MaxPosY;
	int		NextPitShowFlag;
	int		m_pointNum,m_scrNum;
	int		PointerPosY, YRangeU, YRangeD; 
	int		PointerPosX, XRangeL, XRangeR;
	float	min,max;
	float	minX,maxX,minY,maxY;
	float	Realmax,Realmin;
	float	m_pixelSize;
	float	m_diaLimit;
	float	m_depthThreshold;
	float	m_ScalFactor;	// scene view zoom factor
	float	m_ViewTranslationX;
	float	m_ViewTranslationY;
	boost::shared_array<float> m_pitsMark;
	float*	Buffer;
	double	Radius;
	double* Depth;
	GLdouble	m_ModelMatrix[16];//model transformation matrix in OpenGL	
	GLdouble	m_ProjMatrix[16];//project transformation matrix in OpenGL	
	GLint		m_Viewport[4];//view transformation vector 

	BOOL	m_RightButtonDown;
	BOOL	m_LeftButtonDown; 
	BOOL	m_disMeasFlag;
	BOOL	m_snapPit;
	BOOL	m_setflag;
	BOOL	m_deepestShow;
	BOOL	m_bShowSlice;		
	BOOL	m_bShowTexture;
	
	CWnd*			pWnd;
	HGLRC			hrc;
	UINT			m_width;
	UINT			m_height;
	GLCamera		m_Camera;
	Ti_Rotation		m_Orientation;	//rotation in the camera
	Ti_Trackball	m_TrackBall; // for mouse rotation
	CRect			rect,SurRect, Xrect, Yrect, HisRec,Mysrect;
	CSliderCtrlEx m_Slider1;
	std::vector<RGBQUAD> m_rgbData;
	std::vector<POINT7D> m_ptrVertexList;
	CPoint3DLoader loader_;
};
