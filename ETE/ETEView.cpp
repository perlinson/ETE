// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// ETEView.cpp : implementation of the CETEView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ETE.h"
#endif

#include "ETEDoc.h"
#include "ETEView.h"


#include "MainFrm.h"


#include "MainView.h"
#include "StereoView.h"
#include "FlatView.h"
#include "Calculator.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CETEView

IMPLEMENT_DYNCREATE(CETEView, CTabView)

BEGIN_MESSAGE_MAP(CETEView, CTabView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CETEView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

// CETEView construction/destruction

CETEView::CETEView()
{
	// TODO: add construction code here

}

CETEView::~CETEView()
{
}

BOOL CETEView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTabView::PreCreateWindow(cs);
}

// CETEView drawing

void CETEView::OnDraw(CDC* /*pDC*/)
{
	CETEDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	
}


// CETEView printing


void CETEView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CETEView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CETEView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CETEView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CETEView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	//ClientToScreen(&point);
	//OnContextMenu(this, point);
}

void CETEView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CETEView diagnostics

#ifdef _DEBUG
void CETEView::AssertValid() const
{
	CTabView::AssertValid();
}

void CETEView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}

CETEDoc* CETEView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CETEDoc)));
	return (CETEDoc*)m_pDocument;
}
#endif //_DEBUG


// CETEView message handlers

int CETEView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	AddView (RUNTIME_CLASS (CStereoView), _T("Stereo"), 102);
	AddView (RUNTIME_CLASS (CFlatView), _T("Result Image"), 101);
	this -> GetTabControl().ModifyTabStyle( CMFCTabCtrl:: STYLE_3D_ROUNDED_SCROLL);
	this -> GetTabControl().SetLocation(CMFCBaseTabCtrl::LOCATION_TOP);
	this -> GetTabControl().EnableAutoColor( TRUE );  //自动着色
	this -> GetTabControl().SetTabBorderSize( 0 ); //边框大小
	this -> GetTabControl().EnableTabSwap( FALSE );    //禁止拖动

	Calculator::GetInstance()->SetDocument(GetDocument());



	return 0;
}



BOOL CETEView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}