#pragma once
#include "Resource.h"
#include <tuple>

// CDlgPosController dialog

class CDlgPosController : public CDialog
{
	DECLARE_DYNAMIC(CDlgPosController)

public:
	CDlgPosController(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPosController();

// Dialog Data
	enum { IDD = IDD_DLGPOSCONTROLLER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


private:
	CMFCButton m_btnRadioMoveXYFast;
	CMFCButton m_btnRadioMoveXYMed;
	CMFCButton m_btnRadioMoveXYSlow;
	CMFCButton m_btnRadioMoveZFast;
	CMFCButton m_btnRadioMoveZMed;
	CMFCButton m_btnRadioMoveZSlow;
	CMFCButton m_btnRadioWhiteLight;
	CMFCButton m_btnRadioBlueLight;
	CSliderCtrl m_btnSliderLightStrength;
	int		m_CurrentPosX;
	int		m_CurrentPosY;
	int		m_CurrentPosZ;


	CStatic	m_staticMoveXY;
	CStatic m_staticMoveZ;
	
	CRect m_rcClientMoveXY;
	CRect m_rcClientMoveZ;
	CPoint m_ptCenterMoveXY;
	CPoint m_ptCenterMoveZ;
	CPoint m_curPoint;

	BOOL m_bMouseLeftDown;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void OnButtonRadioMoveZ(UINT uID);
	void OnButtonRadioMoveXY(UINT uID);
	void CheckMoveInXYRegion(CPoint point);
	void CheckMoveInZRegion(CPoint point);
	void moveSeleteDistanceForward();
	void moveSeleteDistanceBackward();
	void Stop();
	afx_msg void OnNMCustomdrawSliderLightStrength(NMHDR *pNMHDR, LRESULT *pResult);
	std::tr1::tuple<float, float, float> CalcluateXYZOffset( CPoint point ) ;
	void SetCurrPosition();
};
