#if !defined(AFX_7FBUTTONEX_H__2E283B43_3EF7_3AFF_FE34_A002A3C9DA64__INCLUDED_)
#define AFX_7FBUTTONEX_H__2E283B43_3EF7_3AFF_FE34_A002A3C9DA64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 7fButtonEx.h : header file
//

#include <GdiPlus.h>
using namespace Gdiplus;

#define ICON_ALIGN_LEFT  1
#define ICON_ALIGN_RIGHT 2
#define ICON_BEFORE_TEXT 3
#define ICON_AFTER_TEXT  4

#define WM_7FBUTTON_CLICKARROW	WM_USER+110

/////////////////////////////////////////////////////////////////////////////
// CButtonEx window
class CPictureEx;
class Gdiplus::Bitmap;
class CButtonEx : public CBitmapButton
{
// Construction
public:
	CButtonEx();

	void SetManualCheckState(BOOL bManual, BOOL bHasRightArrow=FALSE, int nArrowRightOffx=0, COLORREF crArrowColor=RGB(0,0,0));
	// add by benekbo on 2005-12-15, no text in bmp for multi_lang app
	void ShouldDrawCaption(BOOL bDraw=TRUE, BOOL bBoldFont=FALSE, int nLeftMargin=0, int nTopMargin=0);
	void SetTextColor(COLORREF crNormal, COLORREF crHover, COLORREF crDown, COLORREF crDisabled);
	BOOL LoadBmpEx(HBITMAP hBmp, BOOL bUseMask=FALSE, BOOL bUseMaskRgn=TRUE, COLORREF crMask=RGB(255, 0, 255));
	BOOL LoadBmpEx(UINT uIDBmp, BOOL bUseMask=FALSE, BOOL bUseMaskRgn=TRUE, COLORREF crMask=RGB(255, 0, 255));
	BOOL LoadBmpEx(LPCTSTR lpBmpFile, BOOL bUseMask=FALSE, BOOL bUseMaskRgn=TRUE, COLORREF crMask=RGB(255, 0, 255));
	BOOL SetCheckBmp(HBITMAP hBmp);
	BOOL LoadPng(HANDLE hGlobal, int nStatusCount = 4);
	BOOL LoadPngFile(LPCTSTR lpFile);
	
	BOOL LoadGif(HANDLE hGlobal, LPCRECT lpGifRect=NULL);  // 允许使用gif，只有在Enable, 且鼠标没有Hover或按下时显示
	void RemoveGif();
	void StopGif();
	void ShowGif(BOOL bShow=TRUE);
	BOOL IsGifReady() const;
	BOOL IsPressed() const;

	BOOL SetCheckBoxStyle(BOOL bCheckBox=TRUE);
	BOOL IsCheckBox() const;
	BOOL IsChecked() const;

	BOOL SetIconBitmaps(HBITMAP hBmpIcon, int nStateCount, UINT nIconAlgin=ICON_ALIGN_LEFT, CPoint ptOffset=CPoint(2, 2));
	BOOL SetIconInfo(HICON hIcon, UINT nIconAlgin=ICON_ALIGN_LEFT, CPoint ptOffset=CPoint(2, 2));
	void SetTransparentParent(HWND hwnd);
protected:
	void DrawCaption(HDC hdc, int nBtnState, CRect rItem);
// Attributes
private:
	COLORREF m_crArrowColor;	//小箭头颜色
	int m_nArrowRightOffx; //小箭头离按钮右边的x偏移
	BOOL m_bHasRightArrow;	// 在手动check状态下，是否按钮右边显示向下小箭头
	BOOL m_bManualChecked; // 是否需要手动的设置check状态
	//指示鼠标是否在按钮上面
    BOOL m_bHover;
    //按钮是否跟踪到鼠标
    BOOL m_bTracking;
    //保存图片的变量
    CBitmap mybitmap, m_bmpCheck;
	Gdiplus::Bitmap *m_pImage;

    //按钮尺寸
    CSize m_ButtonSize;

	CBitmap m_bmpIcon;
	HICON   m_hIcon;
	CPoint  m_ptIconPos;
	SIZE	m_sizeIcon;
	int     m_nIconStateCount;
	UINT    m_nIconAlign;

	HRGN  m_hRgn;
  
	int   m_nPartCountInBmp;
	COLORREF m_crMaskColor;
	COLORREF m_crBgColor;

	CString m_strCaption, m_strCaptionChecked;
	CString m_strToolTip, m_strToolTipChecked;
	BOOL    m_bUseMaskColor;

	BOOL  m_bHighlightAlways;

	BOOL      m_bDrawCaption, m_bBoldFont;  // add by benekbo on 2005-12-15
	COLORREF  m_arTxtColors[4]; // 0: Normal, 1: Hover, 2: Down, 3: Gray
	int       m_nLeftMargin;
	int       m_nTopMargin;

	CPictureEx *m_pGifChild;
// Operations
public:
	BOOL SetHighlightAlways(BOOL bHighlight, BOOL bRedraw=TRUE) {BOOL bOld=m_bHighlightAlways ; m_bHighlightAlways = bHighlight; Invalidate(FALSE); return bOld;}
	BOOL LoadBitmap(UINT bitmapid);
	void ReleaseBmp(BOOL bDelete=TRUE) 
	{
		if(bDelete)
		{
			mybitmap.DeleteObject();
		}
		else
		{
			mybitmap.Detach();
		}
	}

// Implementation
public:
	void SetNormalTextAlign(UINT uFlag);
	BOOL m_bAutoAlign;
	UINT m_uTextAlign;
	virtual ~CButtonEx();
	void SetBgColor(COLORREF crBgColor, BOOL bRedraw=FALSE);
	void SetToolTipText(LPCTSTR pszTip);
	void SetToolTipText(UINT nIDTip);
	void SetCaption(LPCTSTR pszCaption); // \n 后面为Tip
protected:
	void CalcuSize();
	void DrawRightArrow(HDC hdc);
	BOOL HitOnRightArrow(CPoint pt);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonEx)
	public:
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	
	// Generated message map functions
protected:
	BOOL m_bIsGif;
	//{{AFX_MSG(CButtonEx)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnNotifyAfterPaintPicture (NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

	virtual void PreSubclassWindow();
	virtual int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;

private:
	BOOL m_bCheckBox, m_bChecked;
	HWND m_hwndTransparentParent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_7FBUTTONEX_H__2E283B43_3EF7_3AFF_FE34_A002A3C9DA64__INCLUDED_)
