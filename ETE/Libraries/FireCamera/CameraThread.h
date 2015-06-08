////////////////////////////////////////////////////////////////////////////////
// Software for FirePackage (c) Evaluation.
// intek - c. kuehnel - 2007
// This software may be used and distributed freely. 
// Please leave a hint to the original supplier intek within your source code.
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERATHREAD_H__E09B170A_862D_4280_AC2C_4277170D3317__INCLUDED_)
#define AFX_CAMERATHREAD_H__E09B170A_862D_4280_AC2C_4277170D3317__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include <camera.h>
#include "cameraframe.h"

/////////////////////////////////////////////////////////////////////////////
// Thread CCameraThread 

class CCameraThread : public CWinThread
{
	DECLARE_DYNCREATE(CCameraThread)
protected:
	CCameraThread();
	
public:
        CCamera        *m_pCamera;              // Pointer to camera
        CCameraFrame   *m_pCameraFrame;         // Pointer to our camera frame
		CCameraView	   *m_pView;				// Pointer to our view   --add by jianbo
public:
        void            AssignCamera(CCamera* pCamera);
		void			AssignCameraview(CCameraView *pCameraView);

        void            Stop();

	//{{AFX_VIRTUAL(CCameraThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

protected:
	virtual ~CCameraThread();
	
	//{{AFX_MSG(CCameraThread)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_CAMERATHREAD_H__E09B170A_862D_4280_AC2C_4277170D3317__INCLUDED_
