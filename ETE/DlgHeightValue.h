#pragma once
#include "CPoint3D.h"
#include "Stereo\GLCamera.h"
#include "pnt.h"
#include "ti_rotation.h"
#include "Ti_Trackball.h"
// CDlgOpenGL dialog

class CDlgHeightValue : public CDialog
{
	DECLARE_DYNAMIC(CDlgHeightValue)

	static const UINT timer_render = 1;

public:
	CDlgHeightValue(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHeightValue();

// Dialog Data
	enum { IDD = IDD_DLGHEIGHTVALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


public:
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL InitGL(GLvoid); // All Setup For OpenGL Goes Here;
	BOOL CreateViewGLContext(HDC hDC);
	int  DrawSurface(GLvoid);
	void SetInput(const std::vector<float>& input, int width, int height);
	void SetSceneRot(int axis, int value, BOOL increment, BOOL apply);
	void DrawVertex(int rows, int cols);
	double GetLength() const;


	void DrawXSlice();
	void DrawYSlice();
	void DrawSlice(UINT nID, CString strCoord, int pos);

private:
	CPen*	pOldPen;
	CDC*	m_ppDC;//ÏÔÊ¾´°¿ÚDC
	CDC*	m_sliceDC;
	CWnd* pWnd;
	HGLRC m_hGLContext;
	POINT3D prMax, prMin;
	POINT3D m_SceneCenter;	// the center point of the whole displayed scene
	POINT3D m_SceneVolume;	// the center point of the whole displayed scene
	float sceneRot[3];
	float m_bReloading;
	float	m_ScalFactor;	// scene view zoom factor
	int		m_GLPixelIndex;
	int m_width;
	int m_height;
	int StartX, StartY;
	BOOL mouseleftdown;
	BOOL mouserightdown;
	BOOL m_bShowSlice;
	CPoint mouseprevpoint;
	std::vector<float> m_vecZCoordList;
	std::vector<float> m_vecZCoordListAfterFilter;
	GLdouble	m_ModelMatrix[16];//model transformation matrix in OpenGL	
	GLdouble	m_ProjMatrix[16];//project transformation matrix in OpenGL	
	GLint		m_Viewport[4];//view transformation vector 
	GLCamera m_Camera;
	CRect m_rcClient;
	CRect rect,SurRect, Xrect, Yrect, HisRec,Mysrect;
	PNT	TopLeft,BottomRight;
	int		PointerPosX;
	int		PointerPosY;
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	void GetMinMaxValue(int& minX, int& minY, int &minZ, int& maxX, int& maxY, int& maxZ) const;
	afx_msg void OnHeightvalueShowslice();
	BOOL InitializeOpenGLContent();
	void VertexTransform_WindowToObject(PNT& pnt);
	void VertexTransform_ObjectToWindow(PNT& pnt);
	void GetSurRect();
	void RefreshAll();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
