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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ETE.h"

#include "MainFrm.h"
#include "MeasureManager.h"
#include "ObserverModel.h"
#include "PSIModel.h"
#include "VSIModel.h"
#include "ConfocalModel.h"
#include "HighResolutionModel.h"


#include "ETEView.h"
#include "cameraview.h"
#include "PosController.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int stepDistance = 80;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

	BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
		ON_WM_CREATE()
		ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
		ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
		ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
		ON_WM_SETTINGCHANGE()
		ON_COMMAND(ID_APP_EXIT, &CMainFrame::OnAppExit)
		ON_COMMAND(ID_MEASUER_TYPE, &CMainFrame::OnMeasuerType)
		ON_COMMAND(ID_COM_MULTIPLIER, &CMainFrame::OnComMulplier)
		ON_WM_DESTROY()
		ON_COMMAND(ID_BTN_START_MEASURE, &CMainFrame::OnBtnStartMeasure)
		ON_COMMAND(ID_BTN_SAVE_BITMAP, &CMainFrame::OnBtnSaveBitmap)
		ON_COMMAND(ID_CHECK_3D_VIEW, &CMainFrame::OnCheckConfocal3d)
		ON_UPDATE_COMMAND_UI(ID_BTN_START_MEASURE, &CMainFrame::OnUpdateBtnStartMeasure)
		ON_UPDATE_COMMAND_UI(ID_COM_OBJECTIVE, &CMainFrame::OnUpdateComObjective)
		ON_UPDATE_COMMAND_UI(ID_COM_MULTIPLIER, &CMainFrame::OnUpdateComMultiplier)
		ON_UPDATE_COMMAND_UI(ID_CONFOCAL_SCAN_RANGE, &CMainFrame::OnUpdateScanRange)
		ON_UPDATE_COMMAND_UI(ID_CONFOCAL_SCAN_SPEED, &CMainFrame::OnUpdateScanSpeed)
		ON_UPDATE_COMMAND_UI(ID_CHECK_3D_VIEW, &CMainFrame::OnUpdateCheckCofocal3d)
		ON_COMMAND(ID_BTN_SET_START_POSITION, &CMainFrame::OnBtnSetStartPosition)
		ON_COMMAND(ID_BTN_SET_END_POSITION, &CMainFrame::OnBtnSetEndPosition)
		ON_UPDATE_COMMAND_UI(ID_BTN_SET_START_POSITION, &CMainFrame::OnUpdateBtnSetStartPosition)
		ON_UPDATE_COMMAND_UI(ID_BTN_SET_END_POSITION, &CMainFrame::OnUpdateBtnSetEndPosition)
		ON_COMMAND(ID_BTN_LOAD_BITMAP, &CMainFrame::OnBtnLoadBitmap)
		ON_COMMAND(ID_BTN_LOAD_BLACK, &CMainFrame::OnBtnLoadBlack)
		ON_COMMAND(ID_BTN_LOAD_MASK, &CMainFrame::OnBtnLoadMask)
		ON_COMMAND(ID_BTN_LOAD_WHITE, &CMainFrame::OnBtnLoadWhite)
	END_MESSAGE_MAP()

	// CMainFrame construction/destruction

	CMainFrame::CMainFrame()
	{
		// TODO: add member initialization code here
		theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
		m_useConfocal3d = FALSE;
		m_bStartMeasureButtonEnabled = TRUE;


		m_pScanSpeed	= nullptr;
		m_pScanRange	= nullptr;

		m_dfStartPosition = m_dfEndPosition = m_dfCurrPosition = 0.0;
		//m_leftSide = m_rightSide = m_topSize = m_bottomSize = 0;

		mLensInfo.insert(std::pair<int, std::wstring> (0, _T("0.2")));
		mLensInfo.insert(std::pair<int, std::wstring> (1, _T("0.4")));
		mLensInfo.insert(std::pair<int, std::wstring> (2, _T("0.5")));

	}

	CMainFrame::~CMainFrame()
	{
	}

	int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
			return -1;

		BOOL bNameValid;
		// set the visual manager and style based on persisted value
		OnApplicationLook(theApp.m_nAppLook);

		CMDITabInfo mdiTabParams;
		mdiTabParams.m_style = CMFCTabCtrl::STYLE_FLAT; // other styles available...
		mdiTabParams.m_bActiveTabCloseButton = FALSE;      // set to FALSE to place close button at right of tab area
		mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
		mdiTabParams.m_bAutoColor = FALSE;    // set to FALSE to disable auto-coloring of MDI tabs
		mdiTabParams.m_bDocumentMenu = FALSE; // enable the document menu at the right edge of the tab area
		EnableMDITabbedGroups(FALSE, mdiTabParams);

		m_wndRibbonBar.Create(this);
		m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

		if (!m_wndStatusBar.Create(this))
		{
			TRACE0("Failed to create status bar\n");
			return -1;      // fail to create
		}

		CString strTitlePane1;
		CString strTitlePane2;
		bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
		ASSERT(bNameValid);
		bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
		ASSERT(bNameValid);
		m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
		m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

		// enable Visual Studio 2005 style docking window behavior
		CDockingManager::SetDockingMode(DT_SMART);
		// enable Visual Studio 2005 style docking window auto-hide behavior
		EnableAutoHidePanes(CBRS_ALIGN_ANY);

		// create docking windows
		if (!CreateDockingWindows())
		{
			TRACE0("Failed to create docking windows\n");
			return -1;
		}

		// 	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
		// 	DockPane(&m_wndOutput);
		// 	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
		// 	DockPane(&m_wndProperties);


		// Enable enhanced windows management dialog
		EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

		// Switch the order of document name and application name on the window title bar. This
		// improves the usability of the taskbar because the document name is visible with the thumbnail.
		ModifyStyle(0, FWS_PREFIXTITLE);

		//³õÊ¼»¯FreeImage
		//FreeImage_Initialise(TRUE);


		


		RefreshControls();

		
		return 0;
	}

	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		if( !CMDIFrameWndEx::PreCreateWindow(cs) )
			return FALSE;
		// TODO: Modify the Window class or styles here by modifying
		//  the CREATESTRUCT cs
		cs.style &= ~(LONG) FWS_ADDTOTITLE;
		m_strTitle = TEXT("¸ÉÉæ¹²½¹ÏÔÎ¢¾µ");
		return TRUE;
	}

	BOOL CMainFrame::CreateDockingWindows()
	{
		//BOOL bNameValid;
		//// Create output window
		//CString strOutputWnd;
		//bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
		//ASSERT(bNameValid);
		//if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
		//{
		//	TRACE0("Failed to create Output window\n");
		//	return FALSE; // failed to create
		//}

		//// Create properties window
		//CString strPropertiesWnd;
		//bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
		//ASSERT(bNameValid);
		//if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
		//{
		//	TRACE0("Failed to create Properties window\n");
		//	return FALSE; // failed to create
		//}

		//SetDockingWindowIcons(theApp.m_bHiColorIcons);
		return TRUE;
	}

	void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
	{
		/*	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
		m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

		HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
		m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

		UpdateMDITabbedBarsIcons();*/
	}

	// CMainFrame diagnostics

#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{
		CMDIFrameWndEx::AssertValid();
	}

	void CMainFrame::Dump(CDumpContext& dc) const
	{
		CMDIFrameWndEx::Dump(dc);
	}
#endif //_DEBUG


	// CMainFrame message handlers

	void CMainFrame::OnWindowManager()
	{
		ShowWindowsDialog();
	}

	void CMainFrame::OnApplicationLook(UINT id)
	{
		CWaitCursor wait;

		theApp.m_nAppLook = id;

		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_WIN_2000:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
			m_wndRibbonBar.SetWindows7Look(FALSE);
			break;

		case ID_VIEW_APPLOOK_OFF_XP:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
			m_wndRibbonBar.SetWindows7Look(FALSE);
			break;

		case ID_VIEW_APPLOOK_WIN_XP:
			CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
			m_wndRibbonBar.SetWindows7Look(FALSE);
			break;

		case ID_VIEW_APPLOOK_OFF_2003:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
			CDockingManager::SetDockingMode(DT_SMART);
			m_wndRibbonBar.SetWindows7Look(FALSE);
			break;

		case ID_VIEW_APPLOOK_VS_2005:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
			CDockingManager::SetDockingMode(DT_SMART);
			m_wndRibbonBar.SetWindows7Look(FALSE);
			break;

		case ID_VIEW_APPLOOK_VS_2008:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
			CDockingManager::SetDockingMode(DT_SMART);
			m_wndRibbonBar.SetWindows7Look(FALSE);
			break;

		case ID_VIEW_APPLOOK_WINDOWS_7:
			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
			CDockingManager::SetDockingMode(DT_SMART);
			m_wndRibbonBar.SetWindows7Look(TRUE);
			break;

		default:
			switch (theApp.m_nAppLook)
			{
			case ID_VIEW_APPLOOK_OFF_2007_BLUE:
				CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
				break;

			case ID_VIEW_APPLOOK_OFF_2007_BLACK:
				CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
				break;

			case ID_VIEW_APPLOOK_OFF_2007_SILVER:
				CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
				break;

			case ID_VIEW_APPLOOK_OFF_2007_AQUA:
				CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
				break;
			}

			CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
			CDockingManager::SetDockingMode(DT_SMART);
			m_wndRibbonBar.SetWindows7Look(FALSE);
		}

		RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

		theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
	}

	void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
	{
		pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
	}

	void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
	{
		/*CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
		m_wndOutput.UpdateFonts();*/
	}


	void CMainFrame::OnAppExit()
	{

	}

	void CMainFrame::Init()
	{
		CMFCRibbonComboBox *pOPComboBox = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_MEASUER_TYPE));

		if (!pOPComboBox)
		{
			return;
		}

		if (!pMeasureManager)
		{
			return;
		}

		pOPComboBox->SelectItem(0);
		OnMeasuerType();
	}

	void CMainFrame::OnMeasuerType()
	{
		CMFCRibbonComboBox *pOPComboBox = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_MEASUER_TYPE));

		if (!pOPComboBox)
		{
			return;
		}

		if (!pMeasureManager)
		{
			return;
		}

		m_inConfocalModel	= FALSE;
		m_inVsiModel		= FALSE;
		m_inPsiModel		= FALSE;
		m_inObserverModel	= FALSE;
		m_inHighResuModel	= FALSE;



		int nIndex = pOPComboBox->GetCurSel();

		switch(nIndex)
		{
		case 0:
			{
				pMeasureManager->ChangeState(ObserverModel::Instance());
				m_inObserverModel = TRUE;
			}
			break;
		case 1:
			{
				pMeasureManager->ChangeState(PSIModel::Instance());
				m_inPsiModel = TRUE;
			}
			break;
		case 2:
			{
				pMeasureManager->ChangeState(VSIModel::Instance());
				m_inVsiModel = TRUE;
			}
			break;
		case 3:
			{
				pMeasureManager->ChangeState(ConfocalModel::Instance());
				m_inConfocalModel = TRUE;
			}
			break;
		case 4:
			{
				pMeasureManager->ChangeState(HighResolutionModel::Instance());
				m_inHighResuModel = TRUE;
			}
			break;
		default:
			//ASSERT(FALSE);
			return;
		}
	}


	void CMainFrame::OnBtnStartMeasure()
	{
		m_bStartMeasureButtonEnabled = FALSE;
		if (pMeasureManager)
		{
			pMeasureManager->StartMeasure();

			//Concurrency::task_group tg;

			//tg.run(
			//	[&]
			//{
				pMeasureManager->Calculate();	
			//});
			//tg.wait();
		}
		m_bStartMeasureButtonEnabled = TRUE;
	}

	void CMainFrame::OnDestroy()
	{
		CMDIFrameWndEx::OnDestroy();
	}


	void CMainFrame::OnUpdateBtnStartMeasure(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(m_bStartMeasureButtonEnabled);
	}

	void CMainFrame::OnBtnSaveBitmap()
	{
		pMeasureManager->SaveCurrentBitmap();
	}

	void CMainFrame::OnUpdateComObjective(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(m_inPsiModel);
	}


	void CMainFrame::OnUpdateComMultiplier(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(m_inPsiModel);
	}


	afx_msg void CMainFrame::OnUpdateScanRange(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable((m_useConfocal3d&&m_inConfocalModel) || m_inVsiModel);
	}


	BOOL CMainFrame::GetUse3dView()
	{
		CMFCRibbonCheckBox *pChkUse3dView = DYNAMIC_DOWNCAST(CMFCRibbonCheckBox, m_wndRibbonBar.FindByID(ID_CHECK_3D_VIEW));

		pChkUse3dView->SetTextAlwaysOnRight(FALSE);

		if (!pChkUse3dView)
		{
			return FALSE;
		}
		return pChkUse3dView->IsChecked();
	}

	int CMainFrame::GetScanRange()
	{
		if (!m_pScanSpeed)
		{
			return FALSE;
		}

		if (!m_pScanRange)
		{
			return FALSE;
		}

		int speed = (int)m_pScanSpeed->GetItemData(m_pScanSpeed->GetCurSel());

		int totalRange = _ttoi(m_pScanRange->GetEditText());

		if (speed == 0)
		{
			return 0;
		}

		return totalRange * 1000 / (stepDistance * speed) ;
	}

	float CMainFrame::GetFengFengZhi() const
	{
		int totalRange = _ttoi(m_pScanRange->GetEditText());
		return static_cast<float>(totalRange + 2);
	}

	float CMainFrame::GetFengFengZhi(int step) const    /*step Ö¸µÄÊÇÉ¨Ãè·¶Î§ */
	{
		return static_cast<float>(step + 1)*3/4;
	}

	float CMainFrame::GetPianZhi() const
	{
		return 0.0f/*GetFengFengZhi()/ 2*/;
	}

	float CMainFrame::GetPianZhi(int step) const
	{
		return 0.0f/*GetFengFengZhi()/ 2*/;
	}

	float CMainFrame::GetPinLv() const
	{
		float fengfengzhi = GetFengFengZhi() ;
		float time = fengfengzhi * 1000.0f / stepDistance;
		return 50.0f / time;
	}

	float CMainFrame::GetPinLv(int step) const
	{
		float fengfengzhi = GetFengFengZhi(step) *4/3;
		float time = fengfengzhi * 1000.0f / stepDistance;
		return 50.0f / time;
	}

	float CMainFrame::GetWaitTime() const
{
		float pinlv = GetPinLv();
		return 1.0f * 1000.0f / pinlv;
	}

	float CMainFrame::GetWaitTime(int step) const
	{
		float pinlv = GetPinLv(step);
		return 1.0f * 1000.0f / pinlv;
	}

	float CMainFrame::GetScanStep()
	{
		int factor	   = _ttoi(m_pScanSpeed->GetEditText());
		return factor * 0.1f;
	}

	void CMainFrame::OnCheckConfocal3d()
	{
		m_useConfocal3d = !m_useConfocal3d;

		CETEDoc* pDoc = (CETEDoc*)MDIGetActive()->GetActiveDocument();
		if (!pDoc)
		{
			return;
		}
		pDoc->UseCofocalSection(m_useConfocal3d);
	}


	void CMainFrame::OnUpdateCheckCofocal3d(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(m_inConfocalModel);
		pCmdUI->SetCheck(m_useConfocal3d);
	}

	void CMainFrame::OnUpdateScanSpeed(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable((m_useConfocal3d&&m_inConfocalModel) || m_inVsiModel);
	}

	void CMainFrame::OnBtnSetStartPosition()
	{
		m_dfStartPosition = 0.0;
		//SetCurrPosition(m_dfStartPosition);
		PosController::GetInstance()->SetCurrZPosition(m_dfStartPosition);
	}


	void CMainFrame::OnBtnSetEndPosition()
	{
		m_dfEndPosition = m_dfCurrPosition;

		if (m_dfEndPosition <= m_dfStartPosition)
		{
			return;
		}

		int scanRange = static_cast<int>(1000 * (m_dfEndPosition - m_dfStartPosition));
		CString strScanRange;
		strScanRange.Format(_T("%d"), scanRange);

		if (m_inConfocalModel)
		{
			m_pScanRange->SetEditText(strScanRange);
		}

		if (m_inVsiModel)
		{
			m_pScanRange->SetEditText(strScanRange);
		}
	}

	void CMainFrame::SetCurrPosition(double dfPosition)
	{
		m_dfCurrPosition = dfPosition;
		CMFCRibbonEdit* pCurrPosition = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_E_CURR_POSITION));
		CString str;
		str.Format(_T("%d"), static_cast<int>(m_dfCurrPosition * 1000));
		pCurrPosition->SetEditText(str);
	}


	void CMainFrame::OnUpdateBtnSetStartPosition(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable((m_useConfocal3d&&m_inConfocalModel) || m_inVsiModel);
	}


	void CMainFrame::OnUpdateBtnSetEndPosition(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable((m_useConfocal3d&&m_inConfocalModel) || m_inVsiModel);
	}


	void CMainFrame::OnBtnLoadBitmap()
	{
		CFileDialog dlg(TRUE, _T("Image Files"), NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
			_T("Mask Files (*.TIFF;*.BMP;*.PNG;*.JPG)|*.TIFF;*.BMP;*.PNG;*.JPG;)||"));

		if (dlg.DoModal() == IDOK)
		{
			CString strPath = dlg.GetPathName();
			CDlgImgShow::GetInstance()->SwitchToSpecifedImage(strPath);
		}
	}


	void CMainFrame::OnBtnLoadBlack()
	{
		CDlgImgShow::GetInstance()->UnUsed();
	}


	void CMainFrame::OnBtnLoadMask()
	{
		CDlgImgShow::GetInstance()->UseMask();
	}


	void CMainFrame::OnBtnLoadWhite()
	{
		CDlgImgShow::GetInstance()->Used();
	}

	void CMainFrame::SetRegion(CRect rcDisplay)
	{
		m_rcDisplayRegion.CopyRect(rcDisplay);
	}

	void CMainFrame::RefreshControls()
	{
		m_pScanRange = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_CONFOCAL_SCAN_RANGE));
		m_pScanRange->EnableSpinButtons(1,80);
		m_pScanRange->SetEditText(_T("1"));
		m_pScanSpeed= DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_CONFOCAL_SCAN_SPEED));
		m_pScanSpeed->AddItem(_T("1x"), 1);
		m_pScanSpeed->AddItem(_T("3x"), 3);
		m_pScanSpeed->AddItem(_T("5x"), 5);
		m_pScanSpeed->AddItem(_T("7x"), 7);
		m_pScanSpeed->SelectItem(_T("1 ±¶"));

		m_pLensInfo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COM_MULTIPLIER));
		m_pLensInfo->AddItem(_T("5x"));
		m_pLensInfo->AddItem(_T("20x"));
		m_pLensInfo->AddItem(_T("50x"));
		m_pLensInfo->SelectItem(_T("5x"));
		OnComMulplier();
	}

	void CMainFrame::OnComMulplier()
	{
		int nIndex = m_pLensInfo->GetCurSel();
		CMFCRibbonEdit* pRadius = DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_COM_OBJECTIVE));
		pRadius->SetEditText(mLensInfo[nIndex].c_str());
	}
