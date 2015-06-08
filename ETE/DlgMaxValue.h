#pragma once

#include "stdafx.h"
#include "CvvImage.h"
// CDlgMaxValue dialog
class CETEDoc;
class CDlgMaxValue : public CDialog
{
	DECLARE_DYNAMIC(CDlgMaxValue)

public:
	CDlgMaxValue(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMaxValue();

// Dialog Data
	enum { IDD = IDD_DLGMAXVALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void SetInput(const cv::Mat& maxValue, int imageWidth, int imageHeight);

private:
	CETEDoc* m_pDoc;
	cv::Mat m_maxValue;
	IplImage* m_imgMaxValue;
	std::vector<UINT8> m_vecMaxValue;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	void SetDocument(CETEDoc* m_pDocument);
};
