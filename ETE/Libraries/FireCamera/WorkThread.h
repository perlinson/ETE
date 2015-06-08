////////////////////////////////////////////////////////////////////////////////
// Software for FirePackage (c) Evaluation.
// intek - c. kuehnel - 2007
// This software may be used and distributed freely. 
// Please leave a hint to the original supplier intek within your source code.
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKTHREAD_H__2EDF11B7_5B67_478F_8278_51FBFE2430D3__INCLUDED_)
#define AFX_WORKTHREAD_H__2EDF11B7_5B67_478F_8278_51FBFE2430D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <firebus.h>
#include <cameraview.h>
#include "camerathread.h"
//////////////////////////////////////////////////////////////////////
// This thread does all of our work required to handle a single card.
//////////////////////////////////////////////////////////////////////

class CWorkThread  
{
protected:
  BOOL          m_LogEnabled;           // Log enabled or not
  DWORD         m_Offset;               // Window offset
  CFireBus      m_FireBus;              // Our firebus object
  UINT8         m_CardNr;               // Actual card number

public:
  CWinThread   *m_pThread;              // Pointer to MFC thread
  HWND			*m_pHwndParent;
  CCameraView	*m_pCameraView[2];

                CWorkThread();
				CWorkThread(HWND *, CCameraView*, int iCameraIdx);
				CWorkThread(HWND *, CCameraView* pArrView[2]/*, int iCameraIdx*/);
  virtual       ~CWorkThread();

  void          ThreadMain();
  void          Callback(FBCBINFO *pInfo);
  
  CCameraView*  NewCamera(CCamera *pCamera);
  void          KillCamera(CCamera *pCamera);

  UINT8         Run(UINT8 CardNr);
  UINT8         Stop();

  //add by jianbo
  CCameraThread *m_pCameraThread[2];
  CCamera *m_pCamera[2];

  /*int m_CamearIdx;*/
};


#endif // !defined(AFX_WORKTHREAD_H__2EDF11B7_5B67_478F_8278_51FBFE2430D3__INCLUDED_)
