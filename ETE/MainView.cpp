// MainView.cpp : implementation file
//

#include "stdafx.h"
#include "ETE.h"
#include "MainView.h"
#include "MainFrm.h"


// CMainView

IMPLEMENT_DYNCREATE(CMainView, CView)

CMainView::CMainView()
{

}

CMainView::~CMainView()
{
}

BEGIN_MESSAGE_MAP(CMainView, CView)
	ON_WM_RBUTTONUP()
	
	//ON_COMMAND(ID_BTN_START_MEASURE, &CETEView::OnBtnStartMeasure)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_CAMERA_NEWFRAME, OnCameraNewFrame)
	ON_MESSAGE(WM_WTSTOP, OnWorkThreadStop)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_KEYUP()
END_MESSAGE_MAP()


// CMainView drawing

void CMainView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CMainView diagnostics

#ifdef _DEBUG
void CMainView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG



// CMainView message handlers




void CMainView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

// 	m_dlgPosController.Create(IDD_DLGPOSCONTROLLER, this);
// 	m_dlgPosController.ShowWindow(SW_SHOW);
// 	m_liveViewGray.Create(NULL,WS_CHILD | WS_VISIBLE,CRect(1,1,1,1),this,ID_LIVEVIEW_GRAY);
// 	m_liveViewColor.Create(NULL,WS_CHILD | WS_VISIBLE,CRect(1,1,1,1),this,ID_LIVEVIEW_COLOR);
// 
// 	ReSetChildControllerPosition();
// 	m_liveViewGray.ShowWindow(SW_SHOW);
// 	m_liveViewColor.ShowWindow(SW_SHOW);
// 	m_pMeasureManager = boost::make_shared<CMeasureManager>(this);
// 
// 	m_pMeasureManager->Initilize();		
// 
// 	if (!m_pMeasureManager->GetAllDevsReadyState())
// 	{
// 		PostQuitMessage(0);
// 		return;
// 	}
// 
// 	((CMainFrame*)AfxGetMainWnd())->Init();

}



void CMainView::OnDestroy()
{
	//psicalcdllTerminate();
	m_dlgPosController.DestroyWindow();
	m_pMeasureManager->DeInitilize();
	CView::OnDestroy();
}

void CMainView::OnClose()
{

	CView::OnClose();
}

void CMainView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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


LRESULT CMainView::OnWorkThreadStop(WPARAM wParam,LPARAM lParam)
{
	m_pMeasureManager->OnWorkThreadStop();
	return 0;
}

LRESULT CMainView::OnCameraNewFrame(WPARAM wParam, LPARAM lParam)
{
	int cameraIdx = (int)lParam;
	m_pMeasureManager->ProcessNewFrame(cameraIdx);
	return 0;
}

void CMainView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	//if (IsWindowVisible())
	//{
	//	ReSetChildControllerPosition();
	//}
}

const int liveView_width	= 640;
const int liveView_height	= 480;

void CMainView::ReSetChildControllerPosition()
{
	int view_width(0), view_height(1);

	CRect rcClient, rcColorLiveView, rcGrayLiveView, rcPosController;
	CRect rcViewClient;
	AfxGetMainWnd()->GetClientRect(&rcClient);
	GetWindowRect(&rcViewClient);

	m_dlgPosController.GetClientRect(&rcPosController);
	int xPos = (rcViewClient.Width() - liveView_width) / 2;

	rcPosController.left	= xPos;
	rcPosController.right	= xPos+ liveView_width;
	rcPosController.bottom	= liveView_height + rcPosController.Height();
	rcPosController.top		= liveView_height;

	rcGrayLiveView.top		= 0;
	rcGrayLiveView.bottom	= liveView_height;
	rcGrayLiveView.left		= xPos;
	rcGrayLiveView.right	= rcGrayLiveView.left + liveView_width;

	rcColorLiveView.top		= 0;
	rcColorLiveView.bottom	= liveView_height;
	rcColorLiveView.left	= xPos;
	rcColorLiveView.right	= rcColorLiveView.left + liveView_width;

	m_liveViewColor.MoveWindow(&rcColorLiveView);
	m_liveViewGray.MoveWindow(&rcGrayLiveView);
	m_dlgPosController.MoveWindow(&rcPosController);
}


void CMainView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	m_dlgPosController.Stop();
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}