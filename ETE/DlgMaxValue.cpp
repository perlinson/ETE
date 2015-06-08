// DlgMaxValue.cpp : implementation file
//

#include "stdafx.h"
#include "ETE.h"
#include "DlgMaxValue.h"
#include "afxdialogex.h"
#include <vector>
#include "MainFrm.h"
#include "ETEDoc.h"
#include "CommonFunc.h"
#include "FlatView.h"
#include "ETEView.h"
#include "ETESystemPublicDefine.h"
const int IMAGE_WIDTH = 780;
const int IMAGE_HEIGHT = 580;

// CDlgMaxValue dialog

IMPLEMENT_DYNAMIC(CDlgMaxValue, CDialog)

CDlgMaxValue::CDlgMaxValue(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMaxValue::IDD, pParent)
	, m_imgMaxValue(nullptr)
{
	this->m_pDoc = nullptr;
}

CDlgMaxValue::~CDlgMaxValue()
{
}

void CDlgMaxValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMaxValue, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CDlgMaxValue::SetInput(const cv::Mat& maxValue, int imageWidth, int imageHeight)
{
	applyColorMap(maxValue, m_maxValue, cv::COLORMAP_JET);
	InvalidateRect(NULL);
}


// CDlgMaxValue message handlers


BOOL CDlgMaxValue::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
 	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
 
	if (!m_pDoc)
	{
		return TRUE;
	}

	CRect rcClient;
	GetClientRect(&rcClient);

	if (!m_pDoc->m_maxValue.empty())
	{
		BitMatToWnd(this, m_pDoc->m_maxValue, &rcClient);
	}
	else
	{
		pDC->FillSolidRect(&rcClient, RGB(0,0,0));
	}
	return TRUE;
}


void CDlgMaxValue::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}


void CDlgMaxValue::OnDestroy()
{
	CDialog::OnDestroy();
}

void CDlgMaxValue::SetDocument(CETEDoc* m_pDocument)
{
	if (m_pDocument)
	{
		m_pDoc = m_pDocument;
	}
}

