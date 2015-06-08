////////////////////////////////////////////////////////////////////////////////
// Software for FirePackage (c) Evaluation.
// intek - c. kuehnel - 2007
// This software may be used and distributed freely. 
// Please leave a hint to the original supplier intek within your source code.
////////////////////////////////////////////////////////////////////////////////

#include "../../stdafx.h"
//#include "FireDemo.h"
#include "CameraThread.h"
//#include "FireDemoDlg.h"
#include "CameraFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Message map for our thread.

BEGIN_MESSAGE_MAP(CCameraThread, CWinThread)
	//{{AFX_MSG_MAP(CCameraThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraThread

IMPLEMENT_DYNCREATE(CCameraThread, CWinThread)

CCameraThread::CCameraThread()
{
  m_pCamera = NULL;
  m_pView = NULL;
}

CCameraThread::~CCameraThread()
{
}

/////////////////////////////////////////////////////////////////////////////
// Called on thread startup. We create our Camera Frame window.
// When we start our assigned camera IS LOCKED!

BOOL CCameraThread::InitInstance()
{
  CCreateContext   Context;
  //CRuntimeClass   *pRTC = RUNTIME_CLASS(CCameraView);
  char             Title[128] = "";
  //RECT             DlgRect,FrameRect;

#if 0
  UINT             StatusIds[] = 
                   { IDS_SEEN,IDS_PANEDEF,
                     IDS_SKIP,IDS_PANEDEF,
                     IDS_RATE,IDS_RATEDEF,
                     IDS_MODE,IDS_PANEDEF };

  // Create frame window
  m_pCameraFrame=new CCameraFrame(m_pCamera);
  if(m_pCameraFrame)
  {
    // Set title
    if(m_pCamera)
     wsprintf(Title,"Camera %s %s (Card%d)",
                    m_pCamera->pProps()->VendorName,
                    m_pCamera->pProps()->ModelName,
                    pFireDemoDlg->m_CardIx+1);
    
    // Create frame window without a View
    m_pCameraFrame->Create(((CFireDemoApp*)AfxGetApp())->m_pFrameClassName,
                            Title,
                            WS_OVERLAPPEDWINDOW,
                            CFrameWnd::rectDefault,
                            NULL,
                            MAKEINTRESOURCE(IDM_CAMFRAME),
                            0,
                            NULL);

    // Put our cameraview class into our context
    memset(&Context,0,sizeof(Context));
    Context.m_pNewViewClass=pRTC;


    // Create our CCameraView implicit and select as active one
    m_pCameraFrame->m_pView=(CCameraView*)m_pCameraFrame->CreateView(&Context);
    if(m_pCameraFrame->m_pView)
     m_pCameraFrame->SetActiveView(m_pCameraFrame->m_pView);
   
    // Create Statusbar
    m_pCameraFrame->m_pStatusBar=new CStatusBar;
    m_pCameraFrame->m_pStatusBar->Create(m_pCameraFrame,WS_CHILD|WS_VISIBLE|CBRS_BOTTOM,0);
  
    // Set fields of our statusbar
    m_pCameraFrame->m_pStatusBar->SetIndicators(StatusIds,8);
    
    // Set style of our legend
    m_pCameraFrame->m_pStatusBar->SetPaneStyle(0,SBPS_NOBORDERS);   
    m_pCameraFrame->m_pStatusBar->SetPaneStyle(2,SBPS_NOBORDERS);   
    m_pCameraFrame->m_pStatusBar->SetPaneStyle(4,SBPS_NOBORDERS);   
    m_pCameraFrame->m_pStatusBar->SetPaneStyle(6,SBPS_NOBORDERS);   
    
    // Set value fields
    m_pCameraFrame->m_pStatusBar->SetPaneText(1,"0");
    m_pCameraFrame->m_pStatusBar->SetPaneText(3,"0");
    m_pCameraFrame->m_pStatusBar->SetPaneText(5,"0.0");
    m_pCameraFrame->m_pStatusBar->SetPaneStyle(7,SBPS_STRETCH);
    m_pCameraFrame->m_pStatusBar->SetPaneText(7,"Unknown");

    // Move to appropriate position
    m_pCameraFrame->GetWindowRect(&FrameRect);
    pFireDemoDlg->GetWindowRect(&DlgRect);

    m_pCameraFrame->MoveWindow(DlgRect.right+1,DlgRect.top+pFireDemoDlg->m_Offset,
                               FrameRect.right-FrameRect.left,
                               FrameRect.bottom-FrameRect.top);

    // Set our view size
    m_pCameraFrame->SetViewSize(320,240);

    pFireDemoDlg->m_Offset+=GetSystemMetrics(SM_CYMENUSIZE)+4;
    if(pFireDemoDlg->m_Offset>(DWORD)10*(GetSystemMetrics(SM_CYMENUSIZE)+4))
     pFireDemoDlg->m_Offset=0;
    
    // Disable close button
    m_pCameraFrame->GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,
                                                         MF_BYCOMMAND|MF_GRAYED);
    
    // Display frame window
    m_pCameraFrame->ShowWindow(TRUE);
    m_pCameraFrame->UpdateWindow();
    
    // Set our view size again (Statusbar!)
    m_pCameraFrame->SetViewSize(320,240);

    // Unlock our creator
    m_pCamera->Unlock();

    // Edit settings for this camera
    m_pCameraFrame->EditSettings();


  }
#endif

  // Create our CCameraView implicit and select as active one

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Called when our thread exits.

int CCameraThread::ExitInstance()
{
  // Destroy our camera frame (destroy will also call our destructor)
 /* if(m_pCameraFrame)
   m_pCameraFrame->DestroyWindow();*/
  
  return CWinThread::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Assign camera.

void CCameraThread::AssignCamera(CCamera *pCamera)
{
  m_pCamera=pCamera;
#if 0
  if(pCamera)
   pCamera->Lock();
#endif
}
void CCameraThread::AssignCameraview(CCameraView *pCameraView)
{
	m_pView = pCameraView;
}

/////////////////////////////////////////////////////////////////////////////
// Stop the thread and kill all resources.

void CCameraThread::Stop()
{
  // Post a quit message to our thread
	PostQuitMessage(0);
  if(m_pCameraFrame)
  {
#if 0 ///xxxxxxxxxxxxx

    // Post end thread message to our window
    m_pCameraFrame->PostMessage(WM_ENDTHREAD,0,0);

#endif
  }
}

