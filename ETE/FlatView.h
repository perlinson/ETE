#pragma once
#include "Point3DLoader.h"
#include "resource.h"
#include "Stereo\GLCamera.h"
#include "afxwin.h"
#include "DlgHeightValue.h"
#include "DlgMaxValue.h"
#include "DlgSlice.h"



// CFlatView form view
class CETEDoc;
class CFlatView : public CFormView
{
	DECLARE_DYNCREATE(CFlatView)

protected:
	CFlatView();           // protected constructor used by dynamic creation
	virtual ~CFlatView();

public:
	enum { IDD = IDD_FLATVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	// Attributes
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);
	
	//void DrawSurface();
	int m_GLPixelIndex;
	HGLRC m_hGLContext;
	HDC	  m_hDC;
	GLUquadricObj *quadratic;
	CPoint3DLoader loader_;

	GLCamera		m_Camera;
	POINT3D m_SceneCenter;	// the center point of the whole displayed scene
	POINT3D m_SceneVolume;	// the center point of the whole displayed scene
	float	m_ScalFactor;	// scene view zoom factor

	GLdouble	m_ModelMatrix[16];//model transformation matrix in OpenGL	
	GLdouble	m_ProjMatrix[16];//project transformation matrix in OpenGL	
	GLint		m_Viewport[4];//view transformation vector 


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// 	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
// 	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
// 	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
// 	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnFilter();
	afx_msg LRESULT OnRenderSlice(WPARAM wParam, LPARAM lParam);

	void SetFilterStrength(int strength);
	void DrawXSlice();
	void DrawYSlice();
	void DrawSurface();
	
private:

	CDC*    m_pDC;
	CDC*	m_ppDC;//ÏÔÊ¾´°¿ÚDC
	CDC*	m_xsliceDC;
	CDC*	m_ysliceDC;
	CDC*	m_surfacehisDC;
	CPen*	pOldPen;
	std::vector<float> m_ptrZCoordList;
	std::vector<float> m_ptrZCoordListAfterFilter;
	CButton m_btnStartFilter;
	CComboBox m_cmbFilter;
// 	POINT3D prMax, prMin;
	BOOL m_bReloading;
	CDlgHeightValue m_dlgHeightValue;
	CDlgMaxValue m_dlgMaxValue;
	CDlgSlice m_dlgSlice;
	CWnd*			pWnd;
	int minX, minY, maxX, maxY, minZ, maxZ;
	CRect m_rcMaxalue;
	CRect m_rcXSlice;
	CRect m_rcYSlice;

	double min, max;

	std::vector<float> m_vertexList;
public:

//	void SetSceneRot(int axis, int value, BOOL increment, BOOL apply);
	
	CComboBox m_cmbFilterType;
	int m_sliceWidth;
	int m_sliceHeight;
};


