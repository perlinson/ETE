/*
	FileName	: ButtonEx.cpp

	Description	: 

	Programmer	: yubenxi

	Make Date	: 2006-8-28

	Change		: XXXX...(Name) YYYY.MM.DD(Date) XXXX...(Description) XXXX…(Base Version)  @X
*/

// HoverButton.cpp : implementation file
//

#include "stdafx.h"

#include "ButtonEx.h"
//#include "Resource.h"
//#include "QFCommon.h"
#include "PictureEx.h"
#include "GdiSupport.h"
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonEx

CButtonEx::CButtonEx()
{
	m_bHover = FALSE;       
    m_bTracking = FALSE;

	m_nPartCountInBmp = 4;

	m_crBgColor = ::GetSysColor(COLOR_BTNFACE);	
	m_bUseMaskColor = FALSE;

	m_bHighlightAlways = FALSE;

	m_crMaskColor = RGB(255, 0, 255);

	m_bDrawCaption = m_bBoldFont = FALSE;  // add by benekbo on 2005-12-15
	ZeroMemory(m_arTxtColors, sizeof(m_arTxtColors)); // default color: BLACK
	m_nLeftMargin = 0;
	m_nTopMargin = 0;

	m_hRgn = NULL;

	m_pGifChild = NULL;
	
	m_bCheckBox = FALSE;
	m_bChecked = FALSE;

	m_hIcon = NULL;
	m_ptIconPos.x = m_ptIconPos.y = 3;
	m_sizeIcon.cx = m_sizeIcon.cy = 14;
	m_nIconStateCount = 0;
	m_nIconAlign = ICON_ALIGN_LEFT;

	m_bAutoAlign = TRUE;
	m_uTextAlign = DT_CENTER;

	m_bIsGif = FALSE; // 是否动画按钮
	m_hwndTransparentParent = NULL;
	
	m_bManualChecked = FALSE;
	m_bHasRightArrow = FALSE;
	m_nArrowRightOffx = 0;
	m_crArrowColor = RGB(0,0,0);
	m_pImage = NULL;
}

CButtonEx::~CButtonEx()
{
	DeleteGdiObj(m_hRgn);

	mybitmap.DeleteObject();
	
	if( m_hIcon )
	{
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}

	DeletePtr(m_pImage);
}


BEGIN_MESSAGE_MAP(CButtonEx, CBitmapButton)
	//{{AFX_MSG_MAP(CButtonEx)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DRAWITEM()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_PICTUREEX_AFTERPAINT, 100, OnNotifyAfterPaintPicture)
END_MESSAGE_MAP()

void CButtonEx::PreSubclassWindow()
{
	CButton::PreSubclassWindow();

	m_bChecked = FALSE;
	m_bCheckBox = (BS_CHECKBOX==GetButtonStyle() || BS_AUTOCHECKBOX==GetButtonStyle());
	SetButtonStyle(BS_OWNERDRAW, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CButtonEx message handlers

void CButtonEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bTracking /*&& !IsCheckBox()*/)
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE|TME_HOVER;
        tme.dwHoverTime = 1;
        m_bTracking = _TrackMouseEvent(&tme);
    }
    CBitmapButton::OnMouseMove(nFlags, point);
}

void CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	//获取保存在DRAWITEMSTRUCT结构中且在绘制按钮时必须使用的设备上下文
    HDC hdc =lpDrawItemStruct->hDC;
	CRect rcItem = lpDrawItemStruct->rcItem;
	int nIndex = 0;
//    CDC* mydc=CDC::FromHandle(lpDrawItemStruct->hDC);

	// 如果是动画按钮，则不处理
	if(m_bIsGif && ::IsWindowVisible(m_pGifChild->GetSafeHwnd()))
	{
		return;
	}

	if( m_bUseMaskColor || m_pImage )
	{
		CGdiSupport::DrawTransparentBkgnd(m_hWnd, lpDrawItemStruct->hDC,m_hwndTransparentParent);
	}

	if(lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		nIndex = 3;
	}
	else if((lpDrawItemStruct->itemState & ODS_SELECTED) || (IsCheckBox() && IsChecked()))
	{
		//判断按钮是否处于选择状态，
		//如果是则绘制选择状态的按钮位图，在我们提供的位图中，选中状态的按钮图片是第三个
		nIndex = 2;
	}
	else if(/*!IsCheckBox() &&*/ (m_bHover || m_bHighlightAlways))
	{
		//判断鼠标是否离开还是在按钮上面，以便绘制相应的位图
		nIndex = 1;
	}
	else
	{
		nIndex = 0;
	}

	HBITMAP hBmp = (GetCheck() && (HBITMAP) m_bmpCheck) ? (HBITMAP)m_bmpCheck : (HBITMAP)mybitmap;

	if(m_bUseMaskColor)
	{
		CGdiSupport::DrawTransparentBitmap(hdc, hBmp, 0, 0, m_ButtonSize.cx,m_ButtonSize.cy, 0, m_ButtonSize.cy*nIndex, m_crMaskColor);
	}
	else
	{
		if(m_nIconStateCount == 1)
		{
			nIndex = 0;
		}
		if( m_pImage )
		{
			Graphics g(hdc);
//			g.DrawImage(m_pImage, 0, 0, 0, m_ButtonSize.cy*nIndex, m_ButtonSize.cx, m_ButtonSize.cy, UnitPixel);
			RectF rcDst(0.0f, 0.0f, (Gdiplus::REAL)m_ButtonSize.cx, (Gdiplus::REAL)m_ButtonSize.cy);
			g.DrawImage(m_pImage, rcDst, 0, (Gdiplus::REAL)m_ButtonSize.cy*nIndex, (Gdiplus::REAL)m_ButtonSize.cx, (Gdiplus::REAL)m_ButtonSize.cy, UnitPixel);
		}
		else
		{
			CGdiSupport::PasteBmp(hdc, hBmp, 0, 0, m_ButtonSize.cx,m_ButtonSize.cy, 0, m_ButtonSize.cy*nIndex);
		}
	}
	
	if (m_bHover && IsCheckBox() && m_bManualChecked && m_bHasRightArrow)
	{	// 手工checkbox 右边画向下小箭头
		DrawRightArrow(hdc);
	}

	SIZE  sizeText = {0};
	if( (m_hIcon || m_bmpIcon.m_hObject)
		&& m_nIconAlign != ICON_ALIGN_LEFT
		&& m_bDrawCaption)
	{
		CRect rcTest = rcItem;
		CString strCaption;
		GetWindowText(strCaption);
		rcTest.DeflateRect(2, 0);
		if(!::GetTextExtentPoint32(hdc, strCaption, strCaption.GetLength(), &sizeText))
		{
			CRect rcCalc = rcTest;
			if( 0 < ::DrawText(hdc, strCaption, strCaption.GetLength(), &rcCalc, DT_LEFT|DT_SINGLELINE|DT_CALCRECT))
			{
				sizeText.cx = rcCalc.Width();
				sizeText.cy = rcCalc.Height();
			}
			else
			{
				sizeText.cx = strCaption.GetLength() * 10;
				sizeText.cy = 12;
			}
		}
		
		if( sizeText.cx + m_sizeIcon.cx + 2 >= rcTest.Width() ) 
		{
			if( m_nIconAlign == ICON_BEFORE_TEXT )
			{
				m_ptIconPos.x = 2;
			}
			else
			{
				m_ptIconPos.x = rcTest.right - rcItem.left - m_sizeIcon.cx - 2;
			}
		}
		else
		{
			if( m_nIconAlign == ICON_BEFORE_TEXT )
			{
				m_ptIconPos.x = 2 + (rcTest.Width() - (sizeText.cx + m_sizeIcon.cx + 2)) / 2;
			}
			else
			{
				int cx = sizeText.cx + m_sizeIcon.cx + 2;
				m_ptIconPos.x = 2 + (rcTest.Width() - cx) / 2 + cx - m_sizeIcon.cx;
			}
		}

		m_ptIconPos.y = 2 + (rcTest.Height() - max(sizeText.cy, m_sizeIcon.cy)) / 2;
	}

	if( m_hIcon )
	{
		DrawIconEx(hdc, rcItem.left+m_ptIconPos.x, rcItem.top+m_ptIconPos.y, m_hIcon,
			0, 0, 0, NULL, DI_NORMAL);
	}
	else if( m_bmpIcon.m_hObject )
	{
		CGdiSupport::DrawTransparentBitmap(hdc, (HBITMAP)m_bmpIcon, 
			rcItem.left+m_ptIconPos.x, rcItem.top+m_ptIconPos.y,
			m_sizeIcon.cx, m_sizeIcon.cy, 0, m_sizeIcon.cy*min(nIndex, m_nIconStateCount-1) );
	}

	if( m_hIcon || m_bmpIcon.m_hObject )
	{
		if( m_nIconAlign == ICON_ALIGN_LEFT || m_nIconAlign == ICON_BEFORE_TEXT )
		{
			rcItem.left += m_ptIconPos.x + m_sizeIcon.cx;
		}
		else
		{
			rcItem.right = lpDrawItemStruct->rcItem.left + m_ptIconPos.x - 2;
			if( sizeText.cx > 0 )
			{
				rcItem.left = max(rcItem.left , rcItem.right - sizeText.cx);
			}
		}
	}
	
	DrawCaption(hdc, nIndex, rcItem);
}

LRESULT CButtonEx::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
    m_bTracking = FALSE;
	if(m_bHover)
	{
		if( m_bIsGif && ::IsWindowVisible(m_pGifChild->GetSafeHwnd()) )
		{
			ShowGif();
		}
		m_bHover=FALSE;
		//重画按钮
		Invalidate(TRUE);  // 不要Erase bg, 否则会有白色闪烁
	}
    return 0;
}

LRESULT CButtonEx::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	if(!m_bHover)
	{
		if( m_bIsGif && ::IsWindowVisible(m_pGifChild->GetSafeHwnd()) )
		{
			StopGif();
		}
		m_bHover=TRUE;
		Invalidate(TRUE);  // 不要Erase bg, 否则会有白色闪烁
	}
    return 0;
}

/////////////////////////////////////////////////////////////
BOOL CButtonEx::LoadBmpEx(HBITMAP hBmp, BOOL bUseMask, BOOL bUseMaskRgn, COLORREF crMask)
{
	BITMAP    bitmapbits;
    //获取位图信息并存入bitmapbits结构中
    if(hBmp && 0 < GetObject(hBmp, sizeof(BITMAP), &bitmapbits))
	{    
		DeletePtr(m_pImage);
		mybitmap.DeleteObject();

		mybitmap.Attach(hBmp);

		//取位图相应的1/4高度和宽度。
		m_ButtonSize.cy=bitmapbits.bmHeight/4;
		m_ButtonSize.cx=bitmapbits.bmWidth;
    
		m_bUseMaskColor = bUseMask;
		m_crMaskColor = crMask;

		if(IsWindow(m_hWnd))
		{
			SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE | SWP_NOZORDER);
			DeleteGdiObj(m_hRgn);

			if(bUseMask)
			{
				if( bUseMaskRgn )
				{
					HBITMAP hSubBmp = CGdiSupport::GetSubAreaBmp(hBmp, 0, 0, bitmapbits.bmWidth, bitmapbits.bmHeight/4);
					m_hRgn = CGdiSupport::BitmapToRegion(hSubBmp, crMask, 0);
					DeleteGdiObj(hSubBmp);

					SetWindowRgn(m_hRgn, FALSE);
				}
			}

			if(IsWindowVisible())
			{
				Invalidate();
			}
		}

		return TRUE;
	}
	return FALSE;
}

BOOL CButtonEx::LoadBmpEx(UINT bitmapid, BOOL bUseMask, BOOL bUseMaskRgn, COLORREF crMask)
{
    //载入图片
    HBITMAP hBmp = (HBITMAP)::LoadImage(::AfxGetResourceHandle(),MAKEINTRESOURCE(bitmapid), IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);	
	return LoadBmpEx(hBmp, bUseMask, bUseMaskRgn, crMask);
}

BOOL CButtonEx::LoadBmpEx(LPCTSTR lpBmpFile, BOOL bUseMask, BOOL bUseMaskRgn, COLORREF crMask)
{
	//载入图片
	HBITMAP hBmp = (HBITMAP)::LoadImage(NULL, lpBmpFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_DEFAULTSIZE);
	return LoadBmpEx(hBmp, bUseMask, bUseMaskRgn, crMask);
}


BOOL CButtonEx::SetCheckBmp(HBITMAP hBmp)
{
	m_bmpCheck.DeleteObject();
	return m_bmpCheck.Attach(hBmp);
}

//nStatusCount按钮状态数量，目前为1或4态
BOOL CButtonEx::LoadPng(HANDLE hGlobal, int nStatusCount)
{
	if(nStatusCount != 1 && nStatusCount != 4)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	DeletePtr(m_pImage);

	IStream *pStream = NULL; 
	m_nIconStateCount = nStatusCount;
	if(S_OK == ::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream) && pStream)
	{
		m_pImage = Bitmap::FromStream(pStream, FALSE);
		pStream->Release();
	}
	if( !m_pImage )
	{
		return FALSE;
	}

	mybitmap.DeleteObject();
	m_ButtonSize.cx = m_pImage->GetWidth();
	m_ButtonSize.cy = m_pImage->GetHeight() / nStatusCount;
	if( m_ButtonSize.cx <= 0 || m_ButtonSize.cy <= 0 )
	{
		DeletePtr(m_pImage);
		return FALSE;
	}

	m_bUseMaskColor = FALSE;
	DeleteGdiObj(m_hRgn);
	if(IsWindow(m_hWnd))
	{
		SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE | SWP_NOZORDER);
//		ModifyStyleEx(0, WS_EX_TRANSPARENT);
	}
	return TRUE;
}

BOOL CButtonEx::LoadPngFile(LPCTSTR lpFile)
{
	DeletePtr(m_pImage);

	

#ifdef _UNICODE
	m_pImage = Bitmap::FromFile(lpFile);
#else
	USES_CONVERSION;
	m_pImage = Bitmap::FromFile(A2W(lpFile));
#endif
	
	if( !m_pImage )
	{
		return FALSE;
	}
	
	mybitmap.DeleteObject();
	m_ButtonSize.cx = m_pImage->GetWidth();
	m_ButtonSize.cy = m_pImage->GetHeight() / 4;
	if( m_ButtonSize.cx <= 0 || m_ButtonSize.cy <= 0 )
	{
		DeletePtr(m_pImage);
		return FALSE;
	}

	m_bUseMaskColor = FALSE;
	DeleteGdiObj(m_hRgn);
	if(IsWindow(m_hWnd))
	{
		SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE | SWP_NOZORDER);
//		ModifyStyleEx(0, WS_EX_TRANSPARENT);
	}
	return TRUE;
}

void CButtonEx::CalcuSize()
{
    BITMAP    bitmapbits;
    //获取位图信息并存入bitmapbits结构中
    mybitmap.GetBitmap(&bitmapbits);
    
    //取位图相应的1/4高度和宽度。
	m_ButtonSize.cy=bitmapbits.bmHeight/m_nPartCountInBmp;

	m_ButtonSize.cx=bitmapbits.bmWidth;
	
    SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE |SWP_NOOWNERZORDER );

}

void CButtonEx::SetBgColor(COLORREF crBgColor, BOOL bRedraw)
{
	m_crBgColor = crBgColor;

	if(bRedraw && IsWindow(GetSafeHwnd()))
	{
		Invalidate();
	}
}

BOOL CButtonEx::SetIconBitmaps(HBITMAP hBmpIcon, int nStateCount, UINT nIconAlign, CPoint ptOffset)
{
	BITMAP bm;
	if( hBmpIcon && GetObject(hBmpIcon, sizeof(bm), &bm)>0 && nStateCount>0 && nStateCount<=4)
	{
		m_nIconStateCount = nStateCount;
		m_bmpIcon.DeleteObject();
		m_bmpIcon.Attach(hBmpIcon);
		m_ptIconPos = ptOffset;
		m_sizeIcon.cx = bm.bmWidth;
		m_sizeIcon.cy = bm.bmHeight / nStateCount;
		m_nIconAlign = nIconAlign;

		if( m_hIcon )
		{
			DestroyIcon(m_hIcon);
			m_hIcon = NULL;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CButtonEx::SetIconInfo(HICON hIcon, UINT nIconAlgin, CPoint ptOffset)
{
	ICONINFO iconInfo = {0};
	if( hIcon && hIcon!=m_hIcon && GetIconInfo(hIcon, &iconInfo) )
	{
		BITMAP bm;
		if(GetObject(iconInfo.hbmColor, sizeof(bm), &bm)>0)
		{
			if( m_hIcon )
			{
				DestroyIcon(m_hIcon);
			}
			m_hIcon = hIcon;
			m_nIconAlign = nIconAlgin;
			m_sizeIcon.cx = bm.bmWidth;
			m_sizeIcon.cy = bm.bmHeight;
			m_bmpIcon.DeleteObject();
			m_nIconStateCount = 1;
			m_ptIconPos = ptOffset;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CButtonEx::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
//	CRect rect;
//	pDC->GetClipBox(&rect);
//	if( m_bUseMaskColor && !m_hRgn )
//	{
//		CGdiSupport::DrawTransparentBkgnd(m_hWnd, pDC->GetSafeHdc());
//	}
//	else
//	{
//		// 四四方方的矩形按钮不需要重画背景
//		// pDC->FillSolidRect(rect, m_crBgColor);
//	}
	
	return TRUE;
//	return CBitmapButton::OnEraseBkgnd(pDC);
}

void CButtonEx::SetToolTipText(LPCTSTR pszTip)
{
	CString strTip = pszTip;

	int nPos = strTip.Find('\n');
	if( nPos > 0)
	{
		m_strToolTip = strTip.Left(nPos);
		m_strToolTipChecked = strTip.Mid(nPos+1);
	}
	else
	{
		m_strToolTip = strTip;
		m_strToolTipChecked = m_strToolTip;
	}

	EnableToolTips(!m_strToolTip.IsEmpty());
}

void CButtonEx::SetToolTipText(UINT nIDTip)
{
	CString strToolTip;
	strToolTip.LoadString(nIDTip);
	SetToolTipText((LPCTSTR)strToolTip);
}

void CButtonEx::SetCaption(LPCTSTR pszCaption)
{
	CString strCaption = pszCaption;

	int nPos = strCaption.Find('\n');
	if( nPos > 0)
	{
		m_strCaption = strCaption.Left(nPos);
		m_strCaptionChecked = strCaption.Mid(nPos+1);
	}
	else
	{
		m_strCaption = strCaption;
		m_strCaptionChecked = m_strToolTip;
	}
	
	if( IsCheckBox())
	{
		SetWindowText( IsChecked() ? m_strCaptionChecked : m_strCaption );
	}
	else
	{
		SetWindowText( m_strCaption );
	}
}


int CButtonEx::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
	int nResult = CWnd::OnToolHitTest(point, pTI);
	if(!m_strToolTip.IsEmpty())
	{
		pTI->hwnd = m_hWnd;
		pTI->uId =  0; // ID for Child Ctrl of this button
		pTI->uFlags = TTF_ALWAYSTIP | TTF_NOTBUTTON;  // show tip while the btn's topFrame is not Actived
		GetClientRect( &(pTI->rect) );

		if(IsCheckBox() && IsChecked())
		{
			pTI->lpszText = (TCHAR *)(LPCTSTR)m_strToolTipChecked;
		}
		else
		{
			pTI->lpszText = (TCHAR *)(LPCTSTR)m_strToolTip;
		}

		pTI->hinst = AfxGetInstanceHandle(); //      not NULL, let OnFilterToolTipMsg() not call free(),

		//
		//// 加上下面的语句保证所有的 7fButton 都能显示 Tooltips
		//// 也许是微软程序的Bug，导致必须调用这些
		////
		//_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
		//CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;
		//if( IsWindow(pToolTip->GetSafeHwnd()))
		//{
		//	if(0 == memcmp(pTI, &pThreadState->m_lastInfo, min(pTI->cbSize, pThreadState->m_lastInfo.cbSize)))
		//	{
		//		ZeroMemory(&pThreadState->m_lastInfo, pThreadState->m_lastInfo.cbSize);
		//	}
		//}

		return 1;
	}
	return -1;	
}

void CButtonEx::ShouldDrawCaption(BOOL bDraw, BOOL bBoldFont, int nLeftMargin, int nTopMargin)
{
	m_bDrawCaption = bDraw;
	m_bBoldFont = bBoldFont;

	m_nLeftMargin = max(0, nLeftMargin);
	m_nTopMargin = max(0, nTopMargin);
}

void CButtonEx::SetTextColor(COLORREF crNormal, COLORREF crHover, COLORREF crDown, COLORREF crDisabled)
{
	m_arTxtColors[0] = crNormal;
	m_arTxtColors[1] = crHover;
	m_arTxtColors[2] = crDown;
	m_arTxtColors[3] = crDisabled;
}

// nBtnState: 0: Normal, 1: Hover, 2: Down, 3: Gray
void CButtonEx::DrawCaption(HDC hdc, int nBtnState, CRect rItem)
{
	if(!m_bDrawCaption) return;
	CString strCaption;
	GetWindowText(strCaption);
	if(strCaption.IsEmpty()) return;

	ASSERT(hdc != NULL);
	ASSERT(nBtnState>=0 && nBtnState <= 3);
		
	CDC *pDC = CDC::FromHandle(hdc);
	CFont *pFont = GetFont();

	LOGFONT logFont = {0};

	if(pFont)
	{
		pFont->GetLogFont(&logFont);
	}
	else
	{
		CGdiSupport::GetSystemFont(logFont.lfFaceName, ArraySize(logFont.lfFaceName), logFont.lfHeight, FALSE);
	}

	if(m_bBoldFont)
	{
		logFont.lfWeight = FW_BOLD;
	}
	CFont fntBtn;
	fntBtn.CreateFontIndirect(&logFont);
	CFont *pOldFont = NULL;
	pOldFont = pDC->SelectObject(&fntBtn);
	COLORREF crOld = pDC->SetTextColor(m_arTxtColors[nBtnState]);
	int nBkMode = pDC->SetBkMode(TRANSPARENT);

	CRect rBtn = rItem;
	rBtn.OffsetRect(0, 1);
	if(nBtnState == 2)
	{ // down
		rBtn.InflateRect(-2, -2, -1, -1);
	}
	else if( nBtnState == 1)
	{ // hover
		rBtn.DeflateRect(0, 0, 2, 2);		
	}
	else
	{
		rBtn.DeflateRect(1, 1);
	}

	if(this->m_nLeftMargin > 0)
	{
		rBtn.left += m_nLeftMargin;
		if(m_nTopMargin > 0)
		{
			rBtn.top += m_nTopMargin;
			pDC->DrawText(strCaption, rBtn, (m_bAutoAlign ? DT_SINGLELINE|DT_LEFT : m_uTextAlign));
		}
		else
		{
			pDC->DrawText(strCaption, rBtn, DT_VCENTER|(m_bAutoAlign ? DT_SINGLELINE|DT_LEFT : m_uTextAlign));
		}		
	}
	else
	{
		if(m_nTopMargin > 0)
		{
			rBtn.top += m_nTopMargin;
			pDC->DrawText(strCaption, rBtn, (m_bAutoAlign ? DT_SINGLELINE|DT_CENTER : m_uTextAlign));
		}
		else
		{
			pDC->DrawText(strCaption, rBtn, DT_VCENTER|(m_bAutoAlign ? DT_SINGLELINE|DT_CENTER : m_uTextAlign));
		}
	}

	pDC->SetBkMode(nBkMode);
	pDC->SetTextColor(crOld);
	pDC->SelectObject(pOldFont);
}

//
// 去除动画
//
void CButtonEx::RemoveGif()
{
	if( m_pGifChild )
	{
		m_pGifChild->Stop();
		m_pGifChild->DestroyWindow();
		delete m_pGifChild;
		m_pGifChild = NULL;
	}
	m_bIsGif = FALSE;
}

void CButtonEx::StopGif()
{
	if( IsWindow(m_pGifChild->GetSafeHwnd()) )
	{
		m_pGifChild->Stop();
	}
}

//
//  显示或隐藏动画 
//
void CButtonEx::ShowGif(BOOL bShow)
{
	if( IsWindow(m_pGifChild->GetSafeHwnd()))
	{
		if( bShow )
		{ //  显示动画
			m_pGifChild->ShowWindow(SW_SHOW);
			m_pGifChild->Draw(); 
		}
		else
		{ // 隐藏动画
			m_pGifChild->Stop();
			m_pGifChild->ShowWindow(SW_HIDE);		
		}
	}
}

// gif 是否就绪
BOOL CButtonEx::IsGifReady() const
{
	return IsWindow(m_pGifChild->GetSafeHwnd());
}


//
// 加载动画，缺省不显示
//
BOOL CButtonEx::LoadGif(HANDLE hGlobal, LPCRECT lpGifRect)
{
	DWORD dwSize = GlobalSize(hGlobal);

	if( IsWindow(m_hWnd) && dwSize>0)
	{
		if( !m_pGifChild )
		{
			m_pGifChild = new CPictureEx();
		}

		if( m_pGifChild )
		{
			m_pGifChild->DestroyWindow();
			m_pGifChild->UnLoad();
			
			RECT rcGif;
			if( lpGifRect )
			{
				rcGif = *lpGifRect;
			}
			else
			{
				GetClientRect(&rcGif);
			}

			if( m_pGifChild->Create(NULL, NULL, WS_CHILD|WS_DISABLED, rcGif, this, 100))
			{	
				if( NULL == lpGifRect )
				{
					m_bIsGif = TRUE;
				}
				// default is hidden
				return m_pGifChild->Load(hGlobal, dwSize);
			}
			
			delete m_pGifChild;
			m_pGifChild = NULL;
		}
	}

	return FALSE;
}

void CButtonEx::OnDestroy() 
{
	CBitmapButton::OnDestroy();
	
	// TODO: Add your message handler code here
	RemoveGif();

	if( m_hIcon )
	{
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}
	
	mybitmap.DeleteObject();
	DeleteGdiObj(m_hRgn);
	m_bmpIcon.DeleteObject();
	m_bmpCheck.DeleteObject();
}

int CButtonEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBitmapButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetButtonStyle(BS_OWNERDRAW, FALSE);
	return 0;
}

BOOL  CButtonEx::SetCheckBoxStyle(BOOL bCheckBox)
{
	BOOL bOld = m_bCheckBox;	
	m_bCheckBox = bCheckBox;
	return bOld;
}

BOOL  CButtonEx::IsCheckBox() const
{
	return m_bCheckBox;
}

BOOL CButtonEx::IsChecked() const
{
	return m_bChecked;
}

LRESULT CButtonEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	LRESULT lResult = CBitmapButton::WindowProc(message, wParam, lParam);

	switch( message )
	{
	case BM_SETCHECK:
	{
		BOOL bOldValue = m_bChecked;
		m_bChecked = (wParam!=BST_UNCHECKED);
		if( bOldValue != m_bChecked && IsWindowVisible())
		{
			if( !m_strCaptionChecked.IsEmpty())
			{
				SetWindowText( m_bChecked ? m_strCaptionChecked : m_strCaption);
			}
			else
			{
				Invalidate();
			}
		}
		break;
	}
	case BM_GETCHECK:
		lResult = m_bChecked ? BST_CHECKED : BST_UNCHECKED;
		break;
	}

	return lResult;
}

void CButtonEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CBitmapButton::OnLButtonDown(nFlags, point);
	//if( IsCheckBox() && !m_bManualChecked)
	//{
	//	SetCheck( m_bChecked ? BST_UNCHECKED : BST_CHECKED );
	//}
	if (IsCheckBox() && m_bManualChecked && m_bHasRightArrow && HitOnRightArrow(point))
	{
		CWnd *pWnd = GetParent();
		if(::IsWindow(pWnd->GetSafeHwnd()))
		{
			pWnd->SendMessage(WM_7FBUTTON_CLICKARROW, (WPARAM)GetDlgCtrlID(), (LPARAM)GetSafeHwnd());
		}
	}
}

void CButtonEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( IsCheckBox() && !m_bManualChecked )
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		if( rcClient.PtInRect(point))
		{
			SetCheck( m_bChecked ? BST_UNCHECKED : BST_CHECKED );
		}
	}
	CBitmapButton::OnLButtonUp(nFlags, point);
}

void CButtonEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CBitmapButton::OnKeyDown(nChar, nRepCnt, nFlags);

	if( IsCheckBox() && !m_bManualChecked)
	{
		if( nChar == VK_SPACE)
		{
			SetCheck( m_bChecked ? BST_UNCHECKED : BST_CHECKED );
		}
	}
}

// 设置一般情况下文字对齐方式
void CButtonEx::SetNormalTextAlign(UINT uFlag)
{
	m_bAutoAlign = FALSE;	
	m_uTextAlign = uFlag;
}

void CButtonEx::OnNotifyAfterPaintPicture (NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMPictureEx pnmPicEx = reinterpret_cast<LPNMPictureEx>(pNMHDR);
	
	CWnd *pWndCtrl = GetDlgItem(pnmPicEx->hdr.idFrom);
	CDC *pDC = CDC::FromHandle(pnmPicEx->hdc);
	if( !pWndCtrl || !pDC || !pWndCtrl->IsWindowVisible())
	{
		return;
	}
	
	CRect rcClient;
	pWndCtrl->GetClientRect(&rcClient);

	int nIndex = 0;
	if(!IsWindowEnabled())
	{
		nIndex = 3;
	}
	else if(IsCheckBox() && IsChecked())
	{
		//判断按钮是否处于选择状态，
		//如果是则绘制选择状态的按钮位图，在我们提供的位图中，选中状态的按钮图片是第三个
		nIndex = 2;
	}
	else
	{
		nIndex = 0;
	}
	DrawCaption(pDC->GetSafeHdc(), nIndex, rcClient);
}

// 增加这个函数以允许设置hwndTransparentParent，此hwndTransparentParent可以不是按钮的父窗口
void CButtonEx::SetTransparentParent(HWND hwnd)
{
	m_hwndTransparentParent = hwnd;
}

void CButtonEx::SetManualCheckState(BOOL bManual, BOOL bHasRightArrow, int nArrowRightOffx, COLORREF crArrowColor)
{
	m_bManualChecked = bManual;
	m_bHasRightArrow = bHasRightArrow;
	m_nArrowRightOffx = nArrowRightOffx;
	m_crArrowColor = crArrowColor;
}

void CButtonEx::DrawRightArrow(HDC hdc)
{
	int nArrowHeight = 5;
	int cx = m_ButtonSize.cx;
	int cy = m_ButtonSize.cy;
	if (cx > m_nArrowRightOffx)
	{
		CDC *pDC = CDC::FromHandle(hdc);
		
		CPen *pOldPen;	
		CPen pen(PS_SOLID, 1, m_crArrowColor);
		pOldPen = pDC->SelectObject(&pen);

		int x = cx-m_nArrowRightOffx;
		int y = (cy - nArrowHeight) / 2;
		pDC->MoveTo(x, y);
		pDC->LineTo(x+9, y);
		pDC->MoveTo(x+1, y+1);
		pDC->LineTo(x+8, y+1);
		pDC->MoveTo(x+2, y+2);
		pDC->LineTo(x+7, y+2);
		pDC->MoveTo(x+3, y+3);
		pDC->LineTo(x+6, y+3);
		pDC->MoveTo(x+4, y+4);
		pDC->LineTo(x+5, y+4);

		pDC->SelectObject(pOldPen);
	}
}

BOOL CButtonEx::HitOnRightArrow(CPoint pt)
{
	int nArrowHeight = 5;
	int cx = m_ButtonSize.cx;
	int cy = m_ButtonSize.cy;
	if (cx > m_nArrowRightOffx)
	{
		CRect rect;
		rect.left = cx-m_nArrowRightOffx;
		rect.right = rect.left + 9;
		rect.top = (cy-nArrowHeight) / 2;
		rect.bottom = rect.top + 5;
		if (rect.PtInRect(pt))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CButtonEx::IsPressed() const
{
	return GetState() == BST_PUSHED;
}
