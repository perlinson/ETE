// ImageShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgImageShow.h"
#include "afxdialogex.h"
#include "ETESystemPublicDefine.h"
// CImageShowDlg 对话框

const int MAX_PIC_NUM = 10;

CDlgImgShow* CDlgImgShow::pInstance = nullptr; 
IMPLEMENT_DYNAMIC(CDlgImgShow, CDialog)

	CDlgImgShow::CDlgImgShow(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImgShow::IDD, pParent)
{
	m_elapse	= 10;
	m_picIndex	= 0;
	m_loopCounter = 9;
	m_in3dViewModel = TRUE;
	m_currMeasureModel = enm_observer;
}

void CDlgImgShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_PICTURE,m_picMask);
	DDX_Control(pDX,IDC_STATIC_PICTURE2,m_picMask2);
	DDX_Control(pDX,IDC_STATIC_PICTURE3,m_picMask3);
	DDX_Control(pDX,IDC_STATIC_PICTURE4,m_picUsed);
	DDX_Control(pDX,IDC_STATIC_PICTURE5,m_picUnUsed);
	DDX_Control(pDX,IDC_STATIC_PICTURE6,m_picSpecified);
}

void CDlgImgShow::Trigger()
{
	Display();
	++m_picIndex;
}

BEGIN_MESSAGE_MAP(CDlgImgShow, CDialog)
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CImageShowDlg 消息处理程序


BOOL CDlgImgShow::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyle(WS_CAPTION, NULL, SWP_DRAWFRAME);


	CString strImageMask;
	strImageMask.Empty();
	strImageMask.Append(_T("pic0.bmp"));
	m_picMask.Load(strImageMask);


	CString strImage2;
	strImage2.Empty();
	strImage2.Append(_T("pic1.bmp"));
	m_picMask2.Load(strImage2);

	CString strImage3;
	strImage3.Empty();
	strImage3.Append(TEXT("pic2.bmp"));
	m_picMask3.Load(strImage3);

	CString strImage4;
	strImage4.Empty();
	strImage4.Append(TEXT("black.bmp"));
	m_picUnUsed.Load(strImage4);

	CString strImage5;
	strImage5.Empty();
	strImage5.Append(TEXT("white.bmp"));
	m_picUsed.Load(strImage5);

	HideAll();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgImgShow::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}



void CDlgImgShow::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_picMask.SetWindowPos(NULL,NULL,NULL,cx,cy,SWP_NOZORDER| SWP_NOMOVE);
	m_picMask2.SetWindowPos(NULL,NULL,NULL,cx,cy,SWP_NOZORDER| SWP_NOMOVE);
	m_picMask3.SetWindowPos(NULL,NULL,NULL,cx,cy,SWP_NOZORDER| SWP_NOMOVE);
	m_picUnUsed.SetWindowPos(NULL,NULL,NULL,cx,cy,SWP_NOZORDER| SWP_NOMOVE);
	m_picUsed.SetWindowPos(NULL,NULL,NULL,cx,cy,SWP_NOZORDER| SWP_NOMOVE);
	m_picSpecified.SetWindowPos(NULL,NULL,NULL,cx,cy,SWP_NOZORDER| SWP_NOMOVE);
}


void CDlgImgShow::Reset()
{
	m_picIndex = 0;
	Display();
}

void CDlgImgShow::Display()
{
	if (!m_in3dViewModel)
	{
		CString strFilePath;
		strFilePath.Format(TEXT("pic%d.bmp"),m_picIndex);
		m_picMask.Load(strFilePath);
		SleepMs(160);

		if (m_picIndex >= LoopCounter())
		{
			Reset();
		}
		return;
	}
	else
	{
		ShowSelectImage();
		SleepMs(160);
	}
}

void CDlgImgShow::HideAll()
{
	m_picMask.ShowWindow(SW_HIDE);
	m_picMask2.ShowWindow(SW_HIDE);
	m_picMask3.ShowWindow(SW_HIDE);
	m_picUnUsed.ShowWindow(SW_HIDE);
	m_picUsed.ShowWindow(SW_HIDE);
	m_picSpecified.ShowWindow(SW_HIDE);
}

void CDlgImgShow::ShowSelectImage()
{
	if (m_picMask.IsWindowVisible())
	{
		m_picMask2.ShowWindow(SW_SHOW);
		m_picMask3.ShowWindow(SW_HIDE);
		m_picMask.ShowWindow(SW_HIDE);
	}
	else if(m_picMask2.IsWindowVisible())
	{
		m_picMask3.ShowWindow(SW_SHOW);
		m_picMask.ShowWindow(SW_HIDE);
		m_picMask2.ShowWindow(SW_HIDE);
	}
	else
	{
		m_picMask.ShowWindow(SW_SHOW);
		m_picMask2.ShowWindow(SW_HIDE);
		m_picMask3.ShowWindow(SW_HIDE);
	}
	UpdateWindow();
}

void CDlgImgShow::UnUsed()
{
	HideAll();
	m_picUnUsed.ShowWindow(SW_SHOW);
	UpdateWindow();
}

void CDlgImgShow::Used()
{
	HideAll();
	m_picUsed.ShowWindow(SW_SHOW);
	UpdateWindow();
}

void CDlgImgShow::UseMask()
{
	HideAll();
	m_picMask.ShowWindow(SW_SHOW);
	UpdateWindow();
}


CCriticalSection g_cs;

CDlgImgShow * CDlgImgShow::GetInstance()
{
	if(pInstance == NULL)  
	{   //double check  
		g_cs.Lock();           //用lock实现线程安全，用资源管理类，实现异常安全  
		//使用资源管理类，在抛出异常的时候，资源管理类对象会被析构，析构总是发生的无论是因为异常抛出还是语句块结束。  
		if(pInstance == NULL)  
		{  
			pInstance = new CDlgImgShow(); 
			pInstance->Create(IDD_DLG_IMAGESHOW);
			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);
			int vCx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			int vCy = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			pInstance->SetWindowPos(NULL,cx, 0, cx, cy,
				SWP_NOZORDER );
			pInstance->ShowWindow(SW_MAXIMIZE);
			//pInstance->ShowWindow(SW_NORMAL);
		}  
		g_cs.Unlock();
	}  
	return pInstance;  
}

CDlgImgShow::~CDlgImgShow()
{
	if (pInstance)
	{
		delete pInstance;
		pInstance = nullptr;
	}
}

void CDlgImgShow::SwitchToSpecifedImage(CString strPath)
{
	HideAll();
	m_picSpecified.Load(strPath);
	m_picSpecified.ShowWindow(SW_SHOW);
	UpdateWindow();
}

