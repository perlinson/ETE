// StereoView.cpp : implementation file
//

#include "stdafx.h"
#include "ETE.h"
#include "StereoView.h"
#include "MainFrm.h"
#include "ETEDoc.h"

// CStereoView

IMPLEMENT_DYNCREATE(CStereoView, CFormView)

CStereoView::CStereoView()
	: CFormView(CStereoView::IDD)
{

}

CStereoView::~CStereoView()
{
}

void CStereoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIVEVIEW_GRAY,	m_liveViewGray);
	DDX_Control(pDX, IDC_LIVEVIEW_COLOR, m_liveViewColor);
}

BEGIN_MESSAGE_MAP(CStereoView, CFormView)
	ON_WM_RBUTTONUP()

	//ON_COMMAND(ID_BTN_START_MEASURE, &CETEView::OnBtnStartMeasure)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_CAMERA_NEWFRAME, OnCameraNewFrame)
	ON_MESSAGE(WM_WTSTOP, OnWorkThreadStop)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SELECT, &CStereoView::OnSelect)
	ON_COMMAND(ID_RESET, &CStereoView::OnReset)
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// CStereoView diagnostics

#ifdef _DEBUG
void CStereoView::AssertValid() const
{
	CFormView::AssertValid();
}


#ifndef _WIN32_WCE
void CStereoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CStereoView message handlers


void CStereoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_dlgPosController.Create(IDD_DLGPOSCONTROLLER, this);
	m_dlgPosController.ShowWindow(SW_SHOW);
	ReSetChildControllerPosition();
	m_pMeasureManager = std::make_shared<CMeasureManager>(this);

	m_pMeasureManager->Initilize();		

	if (!m_pMeasureManager->GetAllDevsReadyState())
	{
		PostQuitMessage(0);
		return;
	}

	((CMainFrame*)AfxGetMainWnd())->Init();
}



void CStereoView::OnDestroy()
{
	//psicalcdllTerminate();
	m_dlgPosController.DestroyWindow();
	m_pMeasureManager->DeInitilize();
	CView::OnDestroy();
}

void CStereoView::OnClose()
{

	CView::OnClose();
}

void CStereoView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_UP:
		m_dlgPosController.moveSeleteDistanceForward();
		break;
	case VK_DOWN:
		m_dlgPosController.moveSeleteDistanceBackward();
		break;
	default:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


LRESULT CStereoView::OnWorkThreadStop(WPARAM wParam,LPARAM lParam)
{
	m_pMeasureManager->OnWorkThreadStop();
	return 0;
}

LRESULT CStereoView::OnCameraNewFrame(WPARAM wParam, LPARAM lParam)
{
	int cameraIdx = (int)lParam;
	m_pMeasureManager->ProcessNewFrame(cameraIdx);
	return 0;
}

void CStereoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
}

const int liveView_width	= 780;
const int liveView_height	= 580;

void CStereoView::ReSetChildControllerPosition()
{
	
	CRect rcViewClient;
	GetDlgItem(IDC_LIVEVIEW_COLOR)->GetWindowRect(&rcViewClient);
	this->ScreenToClient(&rcViewClient);
	rcViewClient.right = liveView_width + rcViewClient.left;
	rcViewClient.bottom = liveView_height + rcViewClient.top;
	GetDlgItem(IDC_LIVEVIEW_COLOR)->MoveWindow(&rcViewClient);
	GetDlgItem(IDC_LIVEVIEW_GRAY)->MoveWindow(&rcViewClient);

	CRect rcPosController;
	m_dlgPosController.GetWindowRect(&rcPosController);
	this->ScreenToClient(&rcPosController);
	rcPosController.OffsetRect(rcViewClient.left, rcViewClient.bottom+10);
	m_dlgPosController.MoveWindow(&rcPosController);

	rcViewClient.OffsetRect(rcViewClient.Width() + 10, rcViewClient.top);
	GetDlgItem(IDC_RESULT_VIEW)->MoveWindow(&rcViewClient);
}


void CStereoView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	m_dlgPosController.Stop();
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CStereoView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		return TRUE;
	} 

	return CFormView::PreTranslateMessage(pMsg);
}


void CStereoView::OnRButtonDown(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
	CFormView::OnRButtonDown(nFlags, point);
}

void CStereoView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
#ifndef SHARED_HANDLERS
	ScreenToClient(&point);
	CRect rcCameraViewClient;
	CWnd* pWndCameraView = GetDlgItem(IDC_LIVEVIEW_GRAY);
	pWndCameraView->GetWindowRect(rcCameraViewClient);
	ScreenToClient(&rcCameraViewClient);

	if (PtInRect(&rcCameraViewClient, point))
	{
		ClientToScreen(&point);
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_SELECTION, point.x, point.y, this, TRUE);
	}
	
#endif
}

void CStereoView::select(int type)
{
	if(m_pMeasureManager)
	{
		CCameraView* pCameraView = m_pMeasureManager->GetCameraView(CAMERA_GRAY);
		if (pCameraView)
		{
			pCameraView->SetFrameRegions(type);
		}
	}
}

void CStereoView::OnSelect()
{
	select(Selection_Select);
}



void CStereoView::OnReset()
{
	select(Selection_Reset);
}


int CStereoView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//return MA_NOACTIVATE;
	return CFormView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
