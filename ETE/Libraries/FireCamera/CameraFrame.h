////////////////////////////////////////////////////////////////////////////////
// Software for FirePackage (c) Evaluation.
// intek - c. kuehnel - 2007
// This software may be used and distributed freely. 
// Please leave a hint to the original supplier intek within your source code.
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAFRAME_H__5A5F45AD_3A0A_4F07_AC5C_6DC6C1AD9546__INCLUDED_)
#define AFX_CAMERAFRAME_H__5A5F45AD_3A0A_4F07_AC5C_6DC6C1AD9546__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <cameraview.h>

//#define  USE_RECDLG

#ifdef USE_RECDLG
#include <dlgrecord.h>
#endif

class CCameraFrame : public CFrameWnd
{
protected:
        BOOLEAN         m_RecordMode;           // TRUE if we run in record mode
        UINT32          m_RecordFrameCount;
        UINT32          m_RecordDivider;
        
#ifdef USE_RECDLG
		RECORDINFO      m_RecordInfo;           // Our recording info
#endif
// Attribute
public:
        //xxxxCStatusBar     *m_pStatusBar;           // Our statusbar
        CCamera        *m_pCamera;              // Associated camera
        CCameraView    *m_pView;                // Pointer to our view
        BOOL            m_SettingsValid;        // Camera settings are valid
        BOOL            m_IsNotified;           // Flag for already notified
        BOOL            m_IsoReported;          // Iso parameters reported
        //xxxxCDialog        *m_pDlgModal;            // Pointer to modal dialog
        DWORD           m_StartTime;            // Starttime for acquisition
                        
// Operationen
protected:
        BOOL            WriteBitmap(char* pFilename); // Write a bitmap to disk
        void            RecordFrame(FCTLFRMINFO *pFrameInfo,UINT32 FrameNumber); 

public:
        CCamera*        pCamera()              { return m_pCamera; }
        void            SetViewSize(UINT32 Width,UINT32 Height);
        void            NewFrameNotify();
        void            PushFrame(FCTLFRMINFO *pFrameInfo,UINT32 FrameNumber);
        BOOL            StartAcquisition(BOOL Start);
        
	//{{AFX_VIRTUAL(CCameraFrame)
	//}}AFX_VIRTUAL

	                CCameraFrame(CCamera *pCamera); 
         void           EditSettings();

protected:
	virtual         ~CCameraFrame();


	//{{AFX_MSG(CCameraFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
#if 0 //xxxx
	afx_msg void OnEditsettings();
	afx_msg void OnRun();
	afx_msg void On160x120();
	afx_msg void On320x240();
	afx_msg void On1024x768();
	afx_msg void On1280x960();
	afx_msg void On640x480();
	afx_msg void On800x600();
	afx_msg void OnFeature();
        afx_msg LRESULT OnNewFrame(WPARAM,LPARAM);
        afx_msg LRESULT OnEndThread(WPARAM,LPARAM);
	afx_msg void On1600x1200();
	afx_msg void OnUsersize();
	afx_msg void OnSave();
	afx_msg void OnScrollmode();
	afx_msg void OnDirectAccess();
	afx_msg void OnRecord();
	afx_msg void OnLicenserequest();
	afx_msg void OnAbout();
#endif
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_CAMERAFRAME_H__5A5F45AD_3A0A_4F07_AC5C_6DC6C1AD9546__INCLUDED_
