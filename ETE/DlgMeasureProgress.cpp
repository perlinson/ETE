// DlgMeasureProgress.cpp : implementation file
//

#include "stdafx.h"
#include "ETE.h"
#include "DlgMeasureProgress.h"
#include "afxdialogex.h"
#include "ETESystemPublicDefine.h"


// CDlgMeasureProgress dialog

IMPLEMENT_DYNAMIC(CDlgMeasureProgress, CDialog)

CDlgMeasureProgress::CDlgMeasureProgress(CWnd* pParent /*=NULL*/, int maxRange /*= 10*/,  BOOL showStopBtn /*= FALSE*/)
	: CDialog(CDlgMeasureProgress::IDD, pParent)
{
	m_maxRange = maxRange;
	m_bStopBtnShow = showStopBtn;
	m_scanRange = 1;
}

CDlgMeasureProgress::~CDlgMeasureProgress()
{
}

void CDlgMeasureProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_MEASURE, m_progress);
}


BEGIN_MESSAGE_MAP(CDlgMeasureProgress, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgMeasureProgress message handlers


BOOL CDlgMeasureProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STOP)->ShowWindow(m_bStopBtnShow);

	if (m_maxRange <= 0)
	{
		m_maxRange = 1;
	}

	m_progress.SetRange(0, m_maxRange);
	m_progress.SetPos(0);
	SetTimer(1, 500, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgMeasureProgress::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(1);

// 	for (int i = 0; i < ScanRange(); i++)
// 	{
// 		m_action(i);
// 		UpdateProgress(i);
// 	}

	OnOK();
	CDialog::OnTimer(nIDEvent);
}

void CDlgMeasureProgress::AssignAction(const callBackType& action)
{
	m_action = action;
}

void CDlgMeasureProgress::UpdateProgress(int i)
{
	m_progress.SetPos(i);
}