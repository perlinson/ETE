// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include <map>
#include <string>
class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	// 	COutputWnd        m_wndOutput;
	// 	CPropertiesWnd    m_wndProperties;

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnAppExit();
	afx_msg void OnMeasuerType();
	afx_msg void OnComMulplier();
	afx_msg void OnBtnStartMeasure();
	afx_msg void OnDestroy();

private:
	BOOL m_bStartMeasureButtonEnabled;
	BOOL m_useConfocal3d;
	BOOL m_inConfocalModel;
	BOOL m_inObserverModel;
	BOOL m_inPsiModel;
	BOOL m_inVsiModel;
	BOOL m_inHighResuModel;


	double m_dfStartPosition;
	double m_dfEndPosition;
	double m_dfCurrPosition;

	CRect m_rcDisplayRegion;
	CMFCRibbonComboBox* m_pScanSpeed;
	CMFCRibbonEdit* m_pScanRange;
	CMFCRibbonComboBox* m_pLensInfo;

#ifdef _UNICODE
	std::map<int, std::wstring> mLensInfo;
#else
	std::map<int, std::string> mLensInfo;
#endif // _UNICODE


	

public:
	afx_msg void OnUpdateBtnStartMeasure(CCmdUI *pCmdUI);
	afx_msg void OnBtnSaveBitmap();
	afx_msg void OnUpdateComObjective(CCmdUI *pCmdUI);
	afx_msg void OnUpdateComMultiplier(CCmdUI *pCmdUI);
	afx_msg void OnUpdateScanRange(CCmdUI *pCmdUI);
	afx_msg void OnCheckConfocal3d();
	afx_msg void OnUpdateCheckCofocal3d(CCmdUI *pCmdUI);
	afx_msg void OnUpdateScanSpeed(CCmdUI *pCmdUI);
	afx_msg void OnBtnSetStartPosition();
	afx_msg void OnBtnSetEndPosition();

	void	Init();
	BOOL	GetUse3dView();
	int		GetScanRange();
	float	GetScanStep();
	void SetCurrPosition(double dfPosition);

	afx_msg void OnUpdateBtnSetStartPosition(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnSetEndPosition(CCmdUI *pCmdUI);
	afx_msg void OnBtnLoadBitmap();
	afx_msg void OnBtnLoadBlack();
	afx_msg void OnBtnLoadMask();
	afx_msg void OnBtnLoadWhite();
	void SetRegion(CRect rcDisplay);
	float GetFengFengZhi() const;
	float GetPianZhi() const;
	float GetPinLv() const;
	float GetWaitTime() const;

	float GetFengFengZhi(int step) const;
	float GetPianZhi(int step) const;
	float GetPinLv(int step) const;
	float GetWaitTime(int step) const;
	void RefreshControls();
};


