#pragma once

#include "./Libraries/CyUsb/CyAPI.h"
#include "./Libraries/ComSerial/SerialPort.h"
#include "./Libraries/FireCamera/Classes/camera.h"
#include "./Libraries/FireCamera/Classes/firebus.h"
#include "./Libraries/FireCamera/WorkThread.h"
#include "./Libraries/PosController/PosController.h"
#include "DlgPosController.h"
#include "ETESystemPublicDefine.h"
#include "MeasureManager.h"

#define WM_WTSTOP                       (WM_APP+1)      // Workthread stops
#define  WM_LIGHT_STRENGTH				(WM_USER + 100) 

// CStereoView form view

class CETEDoc;

class CStereoView : public CFormView
{
	DECLARE_DYNCREATE(CStereoView)

protected:
	CStereoView();           // protected constructor used by dynamic creation
	virtual ~CStereoView();

public:
	enum { IDD = IDD_STEREOVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();

	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnWorkThreadStop(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnCameraNewFrame(WPARAM wParam, LPARAM lParam);

public:
	void ReSetChildControllerPosition();
private:
	CStatic		m_liveViewGray, m_liveViewColor;
	CDlgPosController m_dlgPosController;
	std::shared_ptr<CMeasureManager> m_pMeasureManager;


	void select(int type);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnSelect();
	afx_msg void OnReset();

	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};


