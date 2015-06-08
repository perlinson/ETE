////////////////////////////////////////////////////////////////////////////////
// Software for FirePackage (c) Evaluation.
// intek - c. kuehnel - 2007
// This software may be used and distributed freely. 
// Please leave a hint to the original supplier intek within your source code.
////////////////////////////////////////////////////////////////////////////////

#include "../../stdafx.h"
#include "WorkThread.h"
//#include "firedemodlg.h"
#include "../../../ETESystemPublicDefine.h"
#include "../../MeasureManager.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern HANDLE g_hCameraAssignEvent;
// Global variable for our work thread (we only have one)
CWorkThread *pWorkThread=NULL;

/////////////////////////////////////////////////////////////////////////////
// Global callback Calculator for our firebus object.

void CardCallback(FBCBINFO *pInfo)
{
  if(pWorkThread)
   pWorkThread->Callback(pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// Our global thread.

UINT FireBusThread(LPVOID)
{
  // Call our main loop
  if(pWorkThread)
   pWorkThread->ThreadMain();
  
  AfxEndThread(0);

  return 0;
}

//////////////////////////////////////////////////////////////////////
// Konstruktion.
//////////////////////////////////////////////////////////////////////

CWorkThread::CWorkThread()
{
  m_pThread=NULL;
  m_pCameraThread[0] = NULL;
  m_pCameraThread[1] = NULL;
  m_pCamera[0] = NULL;
  m_pCamera[1] = NULL;
  m_Offset=0;
  m_pHwndParent = NULL;
  /*m_CamearIdx = -1;*/
  m_pCameraView[0] = m_pCameraView[1] = NULL;
}

CWorkThread::CWorkThread(HWND *pHwnd, CCameraView* pCameraView, int iCameraIdx)
{
	m_pThread=NULL;
	m_pCameraThread[0] = NULL;
	m_pCameraThread[1] = NULL;
	m_pCamera[0] = NULL;
	m_pCamera[1] = NULL;
	m_Offset=0;
	m_pHwndParent = pHwnd;
	/*m_CamearIdx = iCameraIdx;*/
	m_pCameraView[0] = pCameraView;

}

CWorkThread::CWorkThread(HWND *pHwnd, CCameraView* pArrView[2]/*, int iCameraIdx*/)
{
	m_pThread=NULL;
	m_pCameraThread[0] = NULL;
	m_pCameraThread[1] = NULL;
	m_pCamera[0] = NULL;
	m_pCamera[1] = NULL;
	m_Offset=0;
	m_pHwndParent = pHwnd;
	/*m_CamearIdx = iCameraIdx;*/

	m_pCameraView[0] = pArrView[0];
	m_pCameraView[1] = pArrView[1];
}

//////////////////////////////////////////////////////////////////////
// Destruktion.

CWorkThread::~CWorkThread()
{
}

/////////////////////////////////////////////////////////////////////////////
// Start our FireBus thread.

UINT8 CWorkThread::Run(UINT8 CardNr) 
{
  if(!m_pThread)
  {
    // Make this object public
    pWorkThread=this;
    
    // Set card number
    m_CardNr=CardNr;

    // Create MFC thread
    m_pThread=AfxBeginThread(FireBusThread,NULL);
    if(m_pThread)
     return TRUE;
  }

  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Stop our thread.

UINT8 CWorkThread::Stop()
{
  // Stop our firebus (just trigger the end)
  if(m_pThread)
   m_FireBus.Stop();

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Thread main Calculator.

void CWorkThread::ThreadMain()
{
  UINT32 Result;

#if 0
  // Notify our dialog
  pFireDemoDlg->WorkThreadStartNotify();

  pFireDemoDlg->LogMessage("WorkThread started successfully");
#endif

  // Boost execution
  SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);

#if 0
  // See if we have to limit speed
  if(((CButton*)pFireDemoDlg->GetDlgItem(IDB_LIMITSPEED))->GetCheck()==1)
   m_FireBus.LimitSpeed(SPEED100);
  else
   m_FireBus.LimitSpeed(SPEED800);
#else

  //Modify by jianbo
 //Asume speed is 100 first
  m_FireBus.LimitSpeed(SPEED800);

#endif
  // Call CFireBus loop
  Result=m_FireBus.Run(m_CardNr,::CardCallback);
  
  // Unboost execution
  SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);

#if 0
  // FireBus has stopped...
  pFireDemoDlg->LogMessage("WorkThread exits");
#endif

  // Remove pointer for public access
  pWorkThread=NULL;


  if (pMeasureManager)
  {
	  pMeasureManager->WorkThreadStopNotify(Result);
  }
  

#if 0
  // Notify our dialog
  pFireDemoDlg->WorkThreadStopNotify(Result);
#endif

  // End our thread
  AfxEndThread(0,FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// Create a new camera thread and associate this with our new camera.

CCameraView* CWorkThread::NewCamera(CCamera *pCamera)
{
  CRuntimeClass* pRTC=RUNTIME_CLASS(CCameraThread);
  CCameraThread *pThread;

  static int iCameraIdx = 0;
  
  // Create a camera thread for each camera (suspended)
  pThread=(CCameraThread*)AfxBeginThread(pRTC,THREAD_PRIORITY_NORMAL,
                                         0,CREATE_SUSPENDED,NULL);
  if(pThread)
  {
    // Assign camera ( the camera is locked here !!!)
    pThread->AssignCamera(pCamera);
	pThread->AssignCameraview(m_pCameraView[iCameraIdx]);

    // Put thread to camera object
    pCamera->SetUser((void*)pThread);

	//Add by jianbo
	m_pCameraThread[iCameraIdx] = pThread;
	m_pCamera[iCameraIdx] = pCamera;

#if 0
    // Report new device to main dialog
    pFireDemoDlg->ReportCamera(pCamera,TRUE);
#endif

    // Start execution of our new camera thread
    pThread->ResumeThread();
    
    // Try to lock so we really wait until thread unlocks the camera
    pCamera->Lock();
	
	SetEvent(g_hCameraAssignEvent);
    
	// Unlock again to make it available for everybody
    iCameraIdx++;

	pCamera->Unlock();

  }

  return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Kill camera view. This routine IS ALWAYS called for each camera.

void CWorkThread::KillCamera(CCamera *pCamera)
{
  CCameraThread *pCameraThread;
  HANDLE hThread;;
 
#if 0
  // Remove in main dialog
  pFireDemoDlg->ReportCamera(pCamera,FALSE);
#endif

  // Stop appropriate thread
  pCameraThread=(CCameraThread*)pCamera->GetUser();

#if 0
  // Unreport resources
  if(pCameraThread->m_pCameraFrame->m_IsoReported)
   pFireDemoDlg->ReportIsoSettings(-(long)pCamera->GetIsoPayload(),
                                   -(long)pCamera->GetFrameMemorySize());
#endif

  // Get thread handle to wait for end of thread
  hThread=pCameraThread->m_hThread;
  
  // Stop camera thread 
  //pCameraThread->Stop();
  PostThreadMessage(pCameraThread->m_nThreadID, WM_QUIT, 0, 0);

  // Wait for real end of our thread
  WaitForSingleObject(hThread,INFINITE);

  //Add by jianbo
  if(pCameraThread == m_pCameraThread[0])
  {
	m_pCameraThread[0] = NULL;
	m_pCamera[0] = NULL;
  }
  else if(pCameraThread == m_pCameraThread[1])
  {
	  m_pCameraThread[1] = NULL;
	  m_pCamera[1] = NULL;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Our callback Calculator for firebus events. We are called with our own 
// thread context.

void CWorkThread::Callback(FBCBINFO *pInfo)
{
  CCameraThread *pCameraThread;
  TCHAR           cbuf[128];
  FBNCREATE     *pCreate;
  int			idx = -1;
  //TRACE(_T("Msg: %d\n"),pInfo->Reason); 
  switch(pInfo->Reason)
  {
    case FBN_CREATEDEVICE:
     pCreate=(FBNCREATE*)pInfo->pData;
	 TRACE(_T("msg:FBN_CREATEDEVICE\n"));
     break;
    
    case FBN_NEWDEVICE:
     //pFireDemoDlg->LogMessage(m_FireBus.GetReasonString(pInfo->Reason));
     if(((CFireNode*)pInfo->pData)->Type()==FNT_DCAM)
	 {
		NewCamera((CCamera*)pInfo->pData);
		TRACE(_T("msg:FBN_NEWDEVICE\n"));
	 }
     break;

    case FBN_KILLDEVICE:
     //pFireDemoDlg->LogMessage(m_FireBus.GetReasonString(pInfo->Reason));
     if(((CFireNode*)pInfo->pData)->Type()==FNT_DCAM)
      KillCamera((CCamera*)pInfo->pData);
	 TRACE(_T("msg:FBN_KILLDEVICE\n"));
     break;

    case FBN_FRAME:
     pCameraThread=(CCameraThread*)((CCamera*)(pInfo->pData))->GetUser();
#if 0 //del by jianbo
     if(pCameraThread && pCameraThread->m_pCameraFrame)
      pCameraThread->m_pCameraFrame->NewFrameNotify();
#else
	 if(pCameraThread)
	 {
		 if(m_pCameraThread[0] == pCameraThread)
			 idx = 0;
		 else if(m_pCameraThread[1] == pCameraThread)
			 idx = 1;
	 }
	 if (pMeasureManager)
	 {
		 pMeasureManager->GetNewFrameNotify(idx);
	 }
	 return;
#endif

     break;
    
    case FBN_ERROR:
     wsprintf(cbuf,_T("E: Errorcode is %d"),*(UINT32*)pInfo->pData);
#if 0
     pFireDemoDlg->LogMessage(cbuf);
#endif
	 TRACE(_T("msg:%s\n"), cbuf);
	 break;
    
    case FBN_FLAGGEDERROR:
     wsprintf(cbuf, _T("E: Errorflags are %08X"),*(UINT32*)pInfo->pData);
#if 0
     pFireDemoDlg->LogMessage(cbuf);
#endif
	 TRACE(_T("msg:%s\n"), cbuf);
	 break;

    default:
#if 0
     pFireDemoDlg->LogMessage(m_FireBus.GetReasonString(pInfo->Reason));
#endif
     break;
  }

  //report current state to ETE dlg
  //add by jianbo
  PostMessage(*m_pHwndParent, WM_CAMERA_STATE, pInfo->Reason, idx);
  //TRACE(_T("msg-----\n"));
}

