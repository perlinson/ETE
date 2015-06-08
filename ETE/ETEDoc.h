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

// ETEDoc.h : interface of the CETEDoc class
//


#pragma once
#include "CommonFunc.h"

class CETEDoc : public CDocument
{
protected: // create from serialization only
	CETEDoc();
	DECLARE_DYNCREATE(CETEDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CETEDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper Calculator that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	
	void SetMaxValue(const cv::Mat& maxValue);
	void SetHeightValue(const cv::Mat& heightValue);

	void GetSectionRange(LONG& left, LONG& right) const;
	void SetSectionRange(LONG left, LONG right);

	BOOL UseCofocalSection() const { return m_useCofocalSection; }
	void UseCofocalSection(BOOL val) { m_useCofocalSection = val; }
public:
	cv::Mat m_maxValue;
	cv::Mat m_heightValue;
	cv::Mat m_heightValueOrign;
private:
	LONG m_sectionLeft;
	LONG m_sectionRight;
	BOOL m_useCofocalSection;
	
};
