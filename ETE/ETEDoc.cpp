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

// ETEDoc.cpp : implementation of the CETEDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ETE.h"
#endif

#include "ETEDoc.h"

#include <propkey.h>
#include "CommonFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CETEDoc

IMPLEMENT_DYNCREATE(CETEDoc, CDocument)

BEGIN_MESSAGE_MAP(CETEDoc, CDocument)
END_MESSAGE_MAP()


// CETEDoc construction/destruction

CETEDoc::CETEDoc()
{
	// TODO: add one-time construction code here
	this->m_sectionLeft = 0;
	this->m_sectionRight = VIEW_WIDTH;
	this->m_useCofocalSection = FALSE;
}

CETEDoc::~CETEDoc()
{
}

BOOL CETEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CETEDoc serialization

void CETEDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CETEDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CETEDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CETEDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CETEDoc diagnostics

#ifdef _DEBUG
void CETEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CETEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CETEDoc commands



BOOL CETEDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDocument::OnSaveDocument(lpszPathName);
}


void CETEDoc::SetTitle(LPCTSTR lpszTitle)
{
	CDocument::SetTitle(_T(""));
}

void CETEDoc::SetMaxValue(const Mat& maxValue)
{
	m_maxValue = generateMat(maxValue, m_sectionLeft);
}

void CETEDoc::SetHeightValue(const Mat& heightValue)
{
	m_heightValueOrign = heightValue;
	m_heightValue = generateMat(heightValue, m_sectionLeft);
}

void CETEDoc::SetSectionRange(LONG left, LONG right)
{
	m_sectionLeft = left;
	m_sectionRight = right;
}

void CETEDoc::GetSectionRange(LONG& left, LONG& right) const
{
	if (m_sectionRight - m_sectionLeft <= 10)
	{
		left = 0;
		right = VIEW_WIDTH;
		return;
	}
	left = m_sectionLeft;
	right = m_sectionRight;
}
