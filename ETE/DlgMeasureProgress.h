#pragma once
#include <functional>
// CDlgMeasureProgress dialog


typedef std::function<void(int&)> callBackType;


class CDlgMeasureProgress : public CDialog
{
	DECLARE_DYNAMIC(CDlgMeasureProgress)

public:
	CDlgMeasureProgress(CWnd* pParent = NULL, int maxRange = 10, BOOL showStopBtn = FALSE);   // standard constructor
	virtual ~CDlgMeasureProgress();

// Dialog Data
	enum { IDD = IDD_DLGMEASUREPROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CProgressCtrl	m_progress;
	BOOL			m_bStopBtnShow;
	int				m_maxRange;
	int				m_scanRange;
	int				m_loopCounter;
	callBackType m_action;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	void AssignAction(const callBackType& action);
	void UpdateProgress(int i);
	
	int ScanRange() const { return m_scanRange; }
	void ScanRange(int val) { m_scanRange = val; }

	int LoopCounter() const { return m_loopCounter; }
	void LoopCounter(int val) { m_loopCounter = val; }
};
