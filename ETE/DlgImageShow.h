#pragma once
#include "PictureCtrl.h"
#include "ETE.h"
#include <vector>
#include <array>
// CImageShowDlg 对话框

enum enm_measuremodel
{
	enm_observer,
	enm_confocal,
	enm_psi,
	enm_vsi,
};


#define WM_GENERATE_DATA (WM_USER + 100)
class CDlgImgShow : public CDialog
{
	DECLARE_DYNAMIC(CDlgImgShow)

private:
	CDlgImgShow(CWnd* pParent = NULL);   // 标准构造函数
	~CDlgImgShow();   // 标准构造函数
	CDlgImgShow(const CDlgImgShow &);  
	CDlgImgShow& operator = (const CDlgImgShow &);

public:  
	static CDlgImgShow *GetInstance();  
	static CDlgImgShow *pInstance;  
	static CCriticalSection cs; 


// 对话框数据
	enum { IDD = IDD_DLG_IMAGESHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	CPictureCtrl	m_picMask;
	CPictureCtrl	m_picMask2;
	CPictureCtrl	m_picMask3;

	CPictureCtrl	m_picUnUsed;
	CPictureCtrl	m_picUsed;
	CPictureCtrl	m_picSpecified;
	unsigned m_elapse;	
	unsigned int			m_picIndex;

	unsigned int 	m_loopCounter;
	BOOL			m_in3dViewModel;
	enm_measuremodel m_currMeasureModel;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg LRESULT OnGenerateData(WPARAM,LPARAM);
	void SetArea(int width, int height);
	void LoadImageDataFromFile(LPCTSTR filename, int picIndex);
	void Trigger();

	void Display();

	void Reset();
	
	unsigned int LoopCounter() const { return m_loopCounter; }
	void LoopCounter(unsigned int val) { m_loopCounter = val; }

	void Set3DViewModel(BOOL On)	{ m_in3dViewModel = On;		}
	BOOL Get3DViewModel() const		{ return m_in3dViewModel;	}
	void ShowSelectImage();


	void UnUsed();
	void Used();
	void UseMask();
	void HideAll();
	void SwitchToSpecifedImage(CString strPath);
};

