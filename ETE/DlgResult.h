#pragma once


// CDlgResult dialog

class CDlgResult : public CDialog
{
	DECLARE_DYNAMIC(CDlgResult)

public:
	CDlgResult(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgResult();

// Dialog Data
	enum { IDD = IDD_DLGRESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()




};
