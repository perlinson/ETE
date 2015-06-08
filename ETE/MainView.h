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

// CMainView view

class CMainView : public CView
{
	DECLARE_DYNCREATE(CMainView)

protected:
	CMainView();           // protected constructor used by dynamic creation
	virtual ~CMainView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()


private:
	CStatic		m_liveViewGray, m_liveViewColor;
	CDlgPosController m_dlgPosController;
	std::shared_ptr<CMeasureManager> m_pMeasureManager;

protected:
	virtual void OnInitialUpdate();

public:
	int GetNewFrameNotify(int iCameraIdx);
	int RecordFrame(FCTLFRMINFO *pFrameInfo,UINT32 FrameNumber, int iCameraIdx);
	void InitAllDevs();
	void ReSetChildControllerPosition();
	bool Init();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnWorkThreadStop(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnCameraNewFrame(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	
};


