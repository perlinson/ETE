#pragma once

#include "opencv.hpp"
#include <vector>
// CDlgSlice dialog
class CETEDoc;
class CDlgSlice : public CDialog
{
	DECLARE_DYNAMIC(CDlgSlice)

public:
	CDlgSlice(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSlice();

// Dialog Data
	enum { IDD = IDD_DLGSLICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
private:
	IplImage* m_imgSlice;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();

public:
	void SetInput(const std::vector<float>& input, int width, int height);
	BOOL NeedShowSlice() const { return needShowSlice; }
	void NeedShowSlice(BOOL val) { needShowSlice = val; }
private:
	CETEDoc* m_pDoc;

	BOOL	needShowSlice;
	
	CWnd*	pWnd;
	CPen*	pOldPen;
	CDC		* m_xsliceDC;
	CDC		* m_ysliceDC;
	BOOL	m_leftMouseDown;
	int		PointerPosX;
	int		PointerPosY;
	float	max, min;
	int		m_sliceWidth;
	int		m_sliceHeight;
	LONG	m_leftSide;
	LONG	m_rightSide;
	std::vector<float> m_vertexList;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

public:
	int GetXPosition() const { return PointerPosX - m_leftSide; }
	int GetYPosition() const { return PointerPosY; }
	void DrawXSlice();
	void DrawYSlice();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	void RefreshAll();
	void SetDocument(CETEDoc* m_pDocument);
};
