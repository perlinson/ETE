#include "StdAfx.h"
#include "GdiSupport.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGdiSupport::CGdiSupport(void)
{
}


CGdiSupport::~CGdiSupport(void)
{
}


HBITMAP CGdiSupport::GetSubAreaBmp(HBITMAP hBmpBig, int nLeft, int nTop, int nWidth, int nHeight)
{
	BITMAP bm = {0};
	VERIFY( 0 < GetObject(hBmpBig, sizeof(bm), &bm));

//	TRACE("hBmpBig:0x%X, %ld,%ld, %ld,%ld, BmpWidth:%ld, BmpHeight:%ld\n", (LONG)hBmpBig, nLeft, nTop, nWidth, nHeight, bm.bmWidth, bm.bmHeight);

	if(nLeft >= bm.bmWidth || nTop >= bm.bmHeight)
	{
		return NULL;
	}

	nWidth = (-1 == nWidth) ? (bm.bmWidth-nLeft) : min(nWidth, bm.bmWidth-nLeft);
	nHeight = (-1 == nHeight) ? (bm.bmHeight-nTop) : min(nHeight, bm.bmHeight-nTop);

	HDC hdcScreen = NULL;
	HBITMAP hBmpSmall = NULL;

	hdcScreen = ::GetDC(NULL);
	if(hdcScreen)
	{
		HDC hdcMem1 = ::CreateCompatibleDC(hdcScreen);
		HDC hdcMem2 = ::CreateCompatibleDC(hdcScreen);
		if(hdcMem1 !=NULL && hdcMem2!=NULL)
		{
			hBmpSmall = ::CreateCompatibleBitmap(hdcScreen, nWidth, nHeight);
			if(hBmpSmall)
			{
				HBITMAP hOldBmp2 = (HBITMAP)::SelectObject(hdcMem2, hBmpSmall);
				HBITMAP hOldBmp1 = (HBITMAP)::SelectObject(hdcMem1, hBmpBig);
				::BitBlt(hdcMem2, 0, 0, nWidth, nHeight, hdcMem1, nLeft, nTop, SRCCOPY);
				::SelectObject(hdcMem1, hOldBmp1);
				::SelectObject(hdcMem2, hOldBmp2);
			}
		}

		if(hdcMem1)
		{
			DeleteDC(hdcMem1);
			hdcMem1 = NULL;
		}

		if(hdcMem2)
		{
			DeleteDC(hdcMem2);
			hdcMem2 = NULL;
		}

		::ReleaseDC(NULL, hdcScreen);
		hdcScreen = NULL;
	}

//	TRACE("Returned Bitmap: 0x%X\n", (LONG)hBmpSmall);

	return hBmpSmall;
}

/**************************************************************************** 
 *                                                                          * 
 *  FUNCTION   : CopyBitmap (HBITMAP hbm)                                   * 
 *                                                                          * 
 *  PURPOSE    : Copies the given bitmap to another.                        * 
 *                                                                          * 
 *  RETURNS    : A handle to the new bitmap.                                * 
 *                                                                          * 
 ****************************************************************************/ 
HBITMAP CGdiSupport::CopyBitmap (HBITMAP hbm) 
{ 
	if(hbm && OBJ_BITMAP == GetObjectType(hbm))
	{
		return (HBITMAP)CopyImage(hbm, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
	}
	return NULL;
}


// Description	: 粘贴位图到指定的场景DC
BOOL CGdiSupport::PasteBmp(HDC hDC, HBITMAP hBmpSrc, int nLeft, int nTop, int nWidth, int nHeight, int nSrcX, int nSrcY)
{
	if(!hDC || !hBmpSrc)
	{
		return FALSE;
	}

	HDC hTempDC = ::CreateCompatibleDC(hDC);
	if(!hTempDC)
	{
		hTempDC = ::CreateCompatibleDC(NULL); // use default screen DC
	}

	if (hTempDC)
	{
		BITMAP bm;
		if(!::GetObject(hBmpSrc, sizeof(BITMAP), &bm))
		{
			DeleteDC(hTempDC);
			return FALSE;
		}
				
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hTempDC, hBmpSrc);

		if(nWidth == -1) nWidth = bm.bmWidth;
		if(nHeight == -1) nHeight = bm.bmHeight;

		BOOL bReturn = ::BitBlt(hDC, nLeft, nTop, nWidth, nHeight, hTempDC, nSrcX, nSrcY, SRCCOPY); 

		::SelectObject(hTempDC, hOldBmp);

		DeleteDC(hTempDC);
		return bReturn;
	}

	return FALSE;
}


BOOL CGdiSupport::StretchBmp(HDC hDC, HBITMAP hBmpSrc, int nLeft, int nTop, int nWidth, int nHeight, int nSrcX, int nSrcY, int nSrcW, int nSrcH, COLORREF crTransparent)
{
	if(!hDC || !hBmpSrc)
	{
		return FALSE;
	}

	HDC hTempDC = ::CreateCompatibleDC(hDC);

	if(!hTempDC)
	{
		hTempDC = ::CreateCompatibleDC(NULL); // use default screen DC
	}

	if (hTempDC)
	{
		BITMAP bm;
		if(!::GetObject(hBmpSrc, sizeof(BITMAP), &bm))
		{
			DeleteDC(hTempDC);
			return FALSE;
		}
				
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hTempDC, hBmpSrc);
		if(nSrcW == -1) nSrcW = bm.bmWidth;
		if(nSrcH == -1) nSrcH = bm.bmHeight;
		if(nWidth == -1) nWidth = bm.bmWidth;
		if(nHeight == -1) nHeight = bm.bmHeight;

		BOOL bResult = FALSE;
		if( -1 != crTransparent )
		{
			HBITMAP hBmpMem, hBmpOld;
			HDC hdcMem = CreateMemDC(hDC, &hBmpMem, &hBmpOld, nWidth, nHeight);
			if( hdcMem )
			{ // TransparentBlt 函数的拉伸功能在个别机器上不可靠，所以先使用StretchBlt拉伸，再透明粘贴
				bResult = ::StretchBlt(hdcMem, 0, 0, nWidth, nHeight, hTempDC, nSrcX, nSrcY, nSrcW, nSrcH, SRCCOPY); 
				bResult = TransparentBlt(hDC, nLeft, nTop, nWidth, nHeight, hdcMem, 0, 0, nWidth, nHeight, crTransparent);
				DeleteObject(SelectObject(hdcMem, hBmpOld));
				DeleteDC(hdcMem);
			}
			else
			{
				bResult = ::TransparentBlt(hDC, nLeft, nTop, nWidth, nHeight, hTempDC, nSrcX, nSrcY,
					nSrcW, nSrcH, crTransparent ); 
			}
		}
		else
		{
			bResult = ::StretchBlt(hDC, nLeft, nTop, nWidth, nHeight, hTempDC, nSrcX, nSrcY,
				nSrcW, nSrcH, SRCCOPY); 
		}

		::SelectObject(hTempDC, hOldBmp);

		DeleteDC(hTempDC);
		return bResult;
	}

	return FALSE;
}

// 使用屏蔽颜色画出透明位图
void CGdiSupport::DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, 
						   int nXDest, int nYDest,  int nWidth, int nHeight,
						   int nXSrc,  int nYSrc,
						   COLORREF cTransparentColor)
{
	HDC hdcTmp;
	HBITMAP hBmpTmp, hBmpOld;

	ASSERT(hdc);
	ASSERT(hBitmap);

	if(NULL==hdc || NULL==hBitmap)
	{
		return;
	}

	if( -1==nWidth || -1==nHeight)
	{
		BITMAP bm;
		if(0 < GetObject(hBitmap, sizeof(bm), &bm))
		{
			if(nWidth == -1) nWidth = bm.bmWidth;
			if(nHeight == -1) nHeight = bm.bmHeight;
		}
	}

	hdcTmp = CreateCompatibleDC(hdc);
	hBmpTmp = CreateCompatibleBitmap(hdc, nWidth, nHeight);
	hBmpOld = (HBITMAP)SelectObject(hdcTmp, hBmpTmp);
	PasteBmp(hdcTmp, hBitmap, 0, 0, nWidth, nHeight, nXSrc, nYSrc);
	::TransparentBlt(hdc, nXDest, nYDest, nWidth, nHeight, hdcTmp, 0, 0, nWidth, nHeight, cTransparentColor);

	DeleteObject(SelectObject(hdcTmp, hBmpOld));
	DeleteDC(hdcTmp);
}


//加上图层蒙版效果
BOOL CGdiSupport::AddLayerMaskEffect(HDC hdc, const RECT& rc, COLORREF crMask, BYTE btAlphaValue)
{
	BOOL bResult = FALSE;
	HBITMAP hbmpMem, hbmpOld;
	HDC hdcMem = CreateMemDC(hdc, &hbmpMem, &hbmpOld, rc.right-rc.left, rc.bottom-rc.top);
	if( hdcMem )
	{
		FillSolidRect(hdcMem, 0, 0, rc.right-rc.left, rc.bottom-rc.top, crMask);

		BLENDFUNCTION bf = {0};
		bf.AlphaFormat = 0;
		bf.SourceConstantAlpha = btAlphaValue;
		bf.BlendOp = AC_SRC_OVER;
		if(AlphaBlend(hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top,
			hdcMem, 0, 0, rc.right-rc.left, rc.bottom-rc.top, bf))
		{
			bResult = TRUE;
		}
		SelectObject(hdcMem, hbmpOld);
		DeleteObject(hbmpMem);
		DeleteDC(hdcMem);
	}
	return bResult;
}

// 由位图得到区域，去掉透明色部分，cTolerance为透明色允许的偏差
HRGN CGdiSupport::BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{
    HRGN hRgn = NULL;

    if (hBmp)
    {
        // Create a memory DC inside which we will scan the bitmap content
        HDC hMemDC = CreateCompatibleDC(NULL);
        if (hMemDC)
        {
            // Get bitmap size
            BITMAP bm;
            GetObject(hBmp, sizeof(bm), &bm);

            // Create a 32 bits depth bitmap and select it into the memory DC 
            BITMAPINFOHEADER RGB32BITSBITMAPINFO = {    
                    sizeof(BITMAPINFOHEADER),    // biSize 
                    bm.bmWidth,                    // biWidth; 
                    bm.bmHeight,                // biHeight; 
                    1,                            // biPlanes; 
                    32,                            // biBitCount 
                    BI_RGB,                        // biCompression; 
                    0,                            // biSizeImage; 
                    0,                            // biXPelsPerMeter; 
                    0,                            // biYPelsPerMeter; 
                    0,                            // biClrUsed; 
                    0                            // biClrImportant; 
            };
            VOID * pbits32; 
            HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
            if (hbm32)
            {
                HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

                // Create a DC just to copy the bitmap into the memory DC
                HDC hDC = CreateCompatibleDC(hMemDC);
                if (hDC)
                {
                    // Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
                    BITMAP bm32;
                    GetObject(hbm32, sizeof(bm32), &bm32);
                    while (bm32.bmWidthBytes % 4)
                        bm32.bmWidthBytes++;

                    // Copy the bitmap into the memory DC
                    HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
                    BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

                    // For better performances, we will use the ExtCreateRegion() function to create the
                    // region. This function take a RGNDATA structure on entry. We will add rectangles by
                    // amount of ALLOC_UNIT number in this structure.
                    #define ALLOC_UNIT    100
                    DWORD maxRects = ALLOC_UNIT;
                    HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
                    RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
                    pData->rdh.dwSize = sizeof(RGNDATAHEADER);
                    pData->rdh.iType = RDH_RECTANGLES;
                    pData->rdh.nCount = pData->rdh.nRgnSize = 0;
                    SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

                    // Keep on hand highest and lowest values for the "transparent" pixels
                    BYTE lr = GetRValue(cTransparentColor);
                    BYTE lg = GetGValue(cTransparentColor);
                    BYTE lb = GetBValue(cTransparentColor);
                    BYTE hr = min(0xff, lr + GetRValue(cTolerance));
                    BYTE hg = min(0xff, lg + GetGValue(cTolerance));
                    BYTE hb = min(0xff, lb + GetBValue(cTolerance));

                    // Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
                    BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
                    for (int y = 0; y < bm.bmHeight; y++)
                    {
                        // Scan each bitmap pixel from left to right
                        for (int x = 0; x < bm.bmWidth; x++)
                        {
                            // Search for a continuous range of "non transparent pixels"
                            int x0 = x;
                            LONG *p = (LONG *)p32 + x;
                            while (x < bm.bmWidth)
                            {
                                BYTE b = GetRValue(*p);
                                if (b >= lr && b <= hr)
                                {
                                    b = GetGValue(*p);
                                    if (b >= lg && b <= hg)
                                    {
                                        b = GetBValue(*p);
                                        if (b >= lb && b <= hb)
                                            // This pixel is "transparent"
                                            break;
                                    }
                                }
                                p++;
                                x++;
                            }

                            if (x > x0)
                            {
                                // Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
                                if (pData->rdh.nCount >= maxRects)
                                {
                                    GlobalUnlock(hData);
                                    maxRects += ALLOC_UNIT;
                                    hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
                                    pData = (RGNDATA *)GlobalLock(hData);
                                }
                                RECT *pr = (RECT *)&pData->Buffer;
                                SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
                                if (x0 < pData->rdh.rcBound.left)
                                    pData->rdh.rcBound.left = x0;
                                if (y < pData->rdh.rcBound.top)
                                    pData->rdh.rcBound.top = y;
                                if (x > pData->rdh.rcBound.right)
                                    pData->rdh.rcBound.right = x;
                                if (y+1 > pData->rdh.rcBound.bottom)
                                    pData->rdh.rcBound.bottom = y+1;
                                pData->rdh.nCount++;

                                // On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
                                // large (ie: > 4000). Therefore, we have to create the region by multiple steps.
                                if (pData->rdh.nCount == 2000)
                                {
                                    HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
                                    if (hRgn)
                                    {
                                        CombineRgn(hRgn, hRgn, h, RGN_OR);
                                        DeleteGdiObj(h);
                                    }
                                    else
                                        hRgn = h;
                                    pData->rdh.nCount = 0;
                                    SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
                                }
                            }
                        }

                        // Go to next row (remember, the bitmap is inverted vertically)
                        p32 -= bm32.bmWidthBytes;
                    }

                    // Create or extend the region with the remaining rectangles
                    HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
                    if (hRgn)
                    {
                        CombineRgn(hRgn, hRgn, h, RGN_OR);
                        DeleteGdiObj(h);
                    }
                    else
                        hRgn = h;

                    // Clean up
                    GlobalFree(hData);
                    SelectObject(hDC, holdBmp);
                    DeleteDC(hDC);
                }

                DeleteObject(SelectObject(hMemDC, holdBmp));
            }

            DeleteDC(hMemDC);
        }    
    }

    return hRgn;
} 

HBITMAP CGdiSupport::CreateBmpFromFileBuf(LPBYTE lpFileBuf, HDC hdc)
{
	BITMAPFILEHEADER* bf = (BITMAPFILEHEADER*)lpFileBuf;
	if(bf->bfType != /*19778*/'MB')
		return NULL;

	BITMAPINFO* binfo = (BITMAPINFO*)((DWORD)lpFileBuf+sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER * bi = (BITMAPINFOHEADER*)(binfo);

	HBITMAP hbitmap = NULL;
	void * pData = (void*)(lpFileBuf+bf->bfOffBits);

	// 创建位图
	if(NULL == hdc)
	{
		hdc = GetDC(NULL);
		hbitmap = CreateDIBitmap ( hdc, bi, CBM_INIT, pData, binfo, DIB_RGB_COLORS );
		ReleaseDC(NULL, hdc);
	}
	else
	{
		hbitmap = CreateDIBitmap ( hdc, bi, CBM_INIT, pData, binfo, DIB_RGB_COLORS );
	}

	return hbitmap;
}


void CGdiSupport::FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF clr)
{
	_ASSERTE(hdc != NULL);

	COLORREF crOld = ::SetBkColor(hdc, clr);
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, crOld);
}

void CGdiSupport::FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr)
{
	RECT rect = {x, y, x + cx, y + cy};
	FillSolidRect(hdc, &rect, clr);
}


void CGdiSupport::Draw3dRect(HDC hdc, int x, int y, int cx, int cy,
	COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void CGdiSupport::Draw3dRect(HDC hdc, LPCRECT lpRect,
	COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	Draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
		lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}


void  CGdiSupport::ScreenToClient(HWND hwnd, LPRECT lpRect )
{
	ASSERT(NULL==hwnd || IsWindow(hwnd));
	::ScreenToClient(hwnd, (LPPOINT)lpRect);
	::ScreenToClient(hwnd, (LPPOINT)lpRect + 1);
 }

void  CGdiSupport::ClientToScreen(HWND hwnd, LPRECT lpRect )
{
	ASSERT(NULL==hwnd || IsWindow(hwnd));
	::ClientToScreen(hwnd, (LPPOINT)lpRect);
	::ClientToScreen(hwnd, (LPPOINT)lpRect + 1);
}

BOOL CGdiSupport::ReplaceOneColor(HDC hdc, LPCRECT lpRect, COLORREF crSrc, COLORREF crDst)
{
	if( !(hdc && lpRect && IsNormalRect((*lpRect))) ) 
	{
		ASSERT( FALSE );
		return FALSE;
	}

	DWORD dwObjType = GetObjectType(hdc);
	HBITMAP hbmpMem, hbmpOld;
	HDC hdcMem;

	BOOL bResult = FALSE;	
	RECT r1, r2, rcClip;
	int nRgn = ::GetClipBox(hdc, &rcClip);
	
	r1 = *lpRect;
	if( nRgn == SIMPLEREGION || nRgn == COMPLEXREGION )
	{
		IntersectRect(&r1, lpRect, &rcClip);
	}
	
	r2 = r1;

	OffsetRect(&r2, -r2.left, -r2.top);
	
	hbmpMem = hbmpOld = NULL;

	if(dwObjType == OBJ_MEMDC)
	{
		hdcMem = CreateMemDC(NULL, &hbmpMem, &hbmpOld, r1.right-r1.left, r1.bottom-r1.top);
	}
	else
	{
		hdcMem = CreateMemDC(hdc, &hbmpMem, &hbmpOld, r1.right-r1.left, r1.bottom-r1.top);
	}

	if(hdcMem)
	{
		FillSolidRect(hdcMem, &r2, crDst);	
		
		if(TransparentBlt(hdcMem, r2.left, r2.top, r2.right-r2.left, r2.bottom-r2.top,
			hdc,r1.left, r1.top, r1.right-r1.left, r1.bottom-r1.top, crSrc))
		{
			BitBlt(hdc, r1.left, r1.top, r1.right-r1.left, r1.bottom-r1.top, 
				hdcMem, r2.left, r2.top, SRCCOPY);
				bResult = TRUE;
		}
		else
		{
			TRACE1("Failed to TransparentBlt, error: %ld\n", GetLastError());
		}

		SelectObject(hdcMem, hbmpOld);
		DeleteGdiObj(hbmpMem);
		DeleteDC(hdcMem);

		return TRUE;
	}
	
	return FALSE;
}


HDC CGdiSupport::CreateMemDC(HDC hdcSrc, HBITMAP *lphBmpMem, HBITMAP * lphBmpOld, int width, int height)
{
//	ASSERT(hdcSrc);
	ASSERT(lphBmpMem && lphBmpOld);

	if(width<=0 || height<=0)
	{
		return NULL;
	}

	HDC hdcMem = NULL;

	*lphBmpMem = NULL;
	*lphBmpOld = NULL;

	hdcMem = CreateCompatibleDC(hdcSrc);
	if(NULL == hdcMem)
	{
		hdcMem = CreateCompatibleDC(NULL); // screen dc
		if( NULL == hdcMem )
		{
			return NULL;
		}
	}

	if(hdcSrc)
	{
		*lphBmpMem = CreateCompatibleBitmap(hdcSrc, width, height);
	}
	else
	{
		hdcSrc = ::GetDC(NULL);
		*lphBmpMem = CreateCompatibleBitmap(hdcSrc, width, height);
		::ReleaseDC(NULL, hdcSrc);
	}
	if(NULL == *lphBmpMem)
	{
		DeleteDC(hdcMem);
		return NULL;
	}

	*lphBmpOld = (HBITMAP)SelectObject(hdcMem, *lphBmpMem);
	
	return hdcMem;
}


void CGdiSupport::DeflateRect(HRGN hRgn, int l, int t, int r, int b)
{
    HRGN  hRgnTmp;
	HRGN hRgn2;

    hRgn2 = CreateRectRgn(0, 0, 0, 0);
	hRgnTmp = CreateRectRgn(0, 0, 0, 0);
    ::CombineRgn(hRgnTmp, hRgn, NULL, RGN_COPY);

    for (int i = -l; i <= r; i++)
    {
        for (int j = -t; j <= b; j++)
        {            
			CombineRgn(hRgn2, hRgn, NULL, RGN_COPY);
            OffsetRgn(hRgn2, i, j);            
			::CombineRgn(hRgnTmp, hRgnTmp, hRgn2, RGN_AND);
        }
    }

    ::CombineRgn(hRgn, hRgnTmp, NULL, RGN_COPY);
	DeleteObject(hRgn2);
	DeleteObject(hRgnTmp);
} 

void CGdiSupport::DrawBorderEx(HWND hwnd, HDC hdc, int nBorders, const COLORREF *lpcrBorders)
{
	int n, nReturn;
	RECT rc;
	HRGN hRgn = NULL;

	GetWindowRect(hwnd, &rc);
	if( !IsNormalRect(rc))
	{		
		return;
	}

	if( nBorders<=0 || nBorders>6
		|| IsBadReadPtr(lpcrBorders, nBorders * sizeof(COLORREF))
		|| !IsWindow(hwnd)
		|| hdc==NULL)
	{
		OutputDebugString(TEXT("Wrong params in DrawBdEx\n"));
		return;
	}

	hRgn = CreateRectRgn(0, 0, 0, 0);

	OffsetRect(&rc, -rc.left, -rc.top);
	nReturn = GetWindowRgn(hwnd, hRgn);

	if(nReturn==ERROR)
	{
		for(n=0; n<nBorders; n++)
		{
#ifdef _AFX
			CDC::FromHandle(hdc)->Draw3dRect(&rc, lpcrBorders[n], lpcrBorders[n]);
#else
			Draw3dRect(hdc, &rc, lpcrBorders[n], lpcrBorders[n]);
#endif
			::InflateRect(&rc, -1, -1);
		}
	}
	else
	{
		HBRUSH hbr = NULL;
		for(n=0; n<nBorders; n++)
		{
			hbr = CreateSolidBrush(lpcrBorders[n]);
			if(n>0)
			{
				DeflateRect(hRgn, 1, 1, 1, 1);
			}
			FrameRgn(hdc, hRgn, hbr, 1, 1);
			DeleteGdiObj(hbr);			
		}		
	}

	DeleteGdiObj(hRgn);
}

void CGdiSupport::DrawBorderEx(HDC hdc, const RECT & r, int nWidthEllipse, int nHeightEllipse, int nBorders, const COLORREF *lpcrBorders, BOOL bNoRoundBottom)
{
	ASSERT(nBorders>=0 && nBorders<=10);
	ASSERT( !IsBadReadPtr(lpcrBorders, nBorders * sizeof(COLORREF)) );
//	ASSERT(r.right-r.left >= nBorders*2 && r.bottom-r.top>=nBorders*2);
//	ASSERT(r.right-r.left >= 2*nWidthEllipse && r.bottom-r.top>=nHeightEllipse*2);

	if(r.right < r.left || r.top > r.bottom || nBorders<=0)
	{
		return;  // not allow for invalid rect
	}

	RECT rc = r;

	int n;
	if(nWidthEllipse>0 && nHeightEllipse>0)
	{
		HRGN hRgn1 = NULL;
		HBRUSH hbr = NULL;

		for(n=0; n<nBorders; n++)
		{
			hbr = CreateSolidBrush(lpcrBorders[n]);
			if(!hRgn1)
			{
				hRgn1 = CreateRoundRectRgn(rc.left, rc.top, rc.right+1, rc.bottom+1, nWidthEllipse, nHeightEllipse);
				if(bNoRoundBottom)
				{
					HRGN hRgn2 = CreateRectRgn(rc.left, min(rc.bottom, max(rc.top+nHeightEllipse, rc.bottom-nHeightEllipse)), rc.right, rc.bottom);
					CombineRgn(hRgn1, hRgn1, hRgn2, RGN_OR);
					DeleteGdiObj(hRgn2);
				}
			}
			
			::FrameRgn(hdc, hRgn1, hbr, 1, 1);
			DeflateRect(hRgn1, 1, 1, 1, 1);

			DeleteGdiObj(hbr);
		}

		DeleteGdiObj(hRgn1);
	}
	else
	{
		nWidthEllipse = nHeightEllipse = 0;
		for(n=0; n<nBorders; n++)
		{
#ifdef _AFX
			CDC::FromHandle(hdc)->Draw3dRect(&rc, lpcrBorders[n], lpcrBorders[n]);
#else
			Draw3dRect(hdc, &rc, lpcrBorders[n], lpcrBorders[n]);
#endif
			::InflateRect(&rc, -1, -1);
		}
	}
}


void  CGdiSupport::DrawTransparentBkgnd( HWND hWnd, HDC hdc, HWND hWndTransparentParent, LPCRECT lprcClient )
{
	if( !IsWindow(hWnd) || !HAS_STYLE( GetWindowLong(hWnd, GWL_STYLE), WS_CHILD) )
	{
		return;
	}

	ASSERT( hdc );

	RECT rc;

	if( lprcClient && !IsBadReadPtr(lprcClient, sizeof(*lprcClient)))
	{
		CopyRect(&rc, lprcClient);
	}
	else
	{
		GetClientRect(hWnd, &rc);
	}

	HRGN hRgnOld;
	HRGN hRgnClient;

	hRgnOld = ::CreateRectRgn(0, 0, 0, 0);
	hRgnClient = ::CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

	if(GetClipRgn(hdc, hRgnOld) <= 0)
	{
		// -1: error, 0: No ClipRgn
		DeleteGdiObj(hRgnOld);
	}
	ExtSelectClipRgn(hdc, hRgnClient, RGN_AND);
	DeleteGdiObj(hRgnClient);
	
	HWND hwndParent;

	if( IsWindow(hWndTransparentParent))
	{
		hwndParent = hWndTransparentParent;
	}
	else
	{
		hwndParent 	= GetParent(hWnd);
		ASSERT( IsWindow(hwndParent) );
	}

	HRGN hRgnParentErase = CreateRectRgn(0, 0, 0, 0);
	HRGN hRgnParentPaint = CreateRectRgn(0, 0, 0, 0);
	int nRgnParentErase = GetUpdateRgn(hwndParent, hRgnParentErase, TRUE);
	int nRgnParentPaint = GetUpdateRgn(hwndParent, hRgnParentPaint, FALSE);

	::MapWindowPoints(hWnd, hwndParent, (LPPOINT)&rc, 2);

	if(rc.left>=0 && rc.top>=0 && IsNormalRect(rc))
	{
		HDC hdcMem=NULL;
		HBITMAP hBmp=NULL, hBmpOld=NULL;
		BOOL bOK = FALSE;
		while(!bOK)
		{
			if(GetObjectType(hdc) == OBJ_DC)
			{
				hdcMem = CreateCompatibleDC(hdc);
				hBmp = ::CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
			}
			else
			{
				HDC hdcScreen = GetDC(NULL);
				hdcMem = CreateCompatibleDC(hdcScreen);
				hBmp = ::CreateCompatibleBitmap(hdcScreen, rc.right, rc.bottom);
				DeleteDC(hdcScreen);
			}

			if(hBmp==NULL || hdcMem==NULL)
			{
				break;
			}
			
			hBmpOld = (HBITMAP)SelectObject(hdcMem, hBmp);

			SendMessage(hwndParent, WM_ERASEBKGND, (WPARAM)hdcMem, 0L);
//			SendMessage(hwndParent, WM_PAINT, (WPARAM)hdcMem, 0L);

			BitBlt(hdc, 0, 0, rc.right-rc.left, rc.bottom-rc.top, hdcMem, rc.left, rc.top, SRCCOPY);
			
			SelectObject(hdcMem, hBmpOld);

			DeleteGdiObj(hBmp);
			DeleteDC(hdcMem);
			hdcMem = NULL;

			bOK = TRUE;
		}

		if( !bOK )
		{
			if(hdcMem) DeleteDC(hdcMem);
			if(hBmp) DeleteGdiObj(hBmp);

			POINT ptViewPort = {0, 0};
			::GetViewportOrgEx(hdc, &ptViewPort);
			SetViewportOrgEx(hdc, ptViewPort.x-rc.left, ptViewPort.y-rc.top, NULL);
			SendMessage(hwndParent, WM_ERASEBKGND, (WPARAM)hdc, 0L);
			SendMessage(hwndParent, WM_PAINT, (WPARAM)hdc, 0L);
			::SetViewportOrgEx(hdc, ptViewPort.x, ptViewPort.y, NULL);
		}
	}
	
	if(hRgnOld)
	{
		ExtSelectClipRgn(hdc, hRgnOld, RGN_COPY);
		DeleteGdiObj(hRgnOld);
	}
	else
	{
		SelectClipRgn(hdc, NULL);
	}

	if( nRgnParentErase == COMPLEXREGION || nRgnParentErase == SIMPLEREGION )
	{
		InvalidateRgn(hwndParent, hRgnParentErase, TRUE);
	}
	if( nRgnParentPaint == COMPLEXREGION || nRgnParentPaint == SIMPLEREGION )
	{
		InvalidateRgn(hwndParent, hRgnParentPaint, FALSE);
	}

	DeleteObject(hRgnParentErase);
	DeleteObject(hRgnParentPaint);
}


// 获取系统缺省字体名称和大小
void CGdiSupport::GetSystemFont(LPTSTR lpFontName, int nNameMax, long & nPointSize, BOOL bPointSize)
{
	LOGFONT lf;
	LPCTSTR pszFace = _T("System");

	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (hFont == NULL)
	{
		hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	}

	//
	// default size
	//
	nPointSize = bPointSize ? 9 : -12;

	if (hFont != NULL)
	{
		::GetObject(hFont, sizeof(LOGFONT), &lf);
		pszFace = lf.lfFaceName;
	}

	if(bPointSize)
	{
		HDC hDC = ::GetDC(NULL);
		nPointSize = -MulDiv(lf.lfHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY));
		::ReleaseDC(NULL, hDC);
	}
	else
	{
		nPointSize = lf.lfHeight;
	}

	lstrcpyn(lpFontName, pszFace, nNameMax);
}

#define Gray(r, g, b) (b * 11 + g * 59 + r * 30) / 100

void CGdiSupport::DrawGrayBmp(HDC hDesDC, POINT pt, HBITMAP hSrcBitmap)
{
    BITMAP bm;
	HBITMAP hBmpOld = NULL;
    ::GetObject(hSrcBitmap, sizeof(BITMAP), &bm);
    HDC hMemDC=NULL;

    // create a DIBSection copy of the original bitmap
    HBITMAP hDib = (HBITMAP)::CopyImage(hSrcBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG | LR_CREATEDIBSECTION);
    if(hDib == NULL)
	{
		return;
	}
    if(bm.bmBitsPixel < 16)
    {   // bitmap has a colour table, so we modify the colour table
        hMemDC = ::CreateCompatibleDC(hDesDC);
		if( NULL == hMemDC)
		{
			DeleteObject(hDib);
			return;
		}

        hBmpOld = (HBITMAP)::SelectObject(hMemDC, hDib);
        int nColours = 1 << bm.bmBitsPixel;

        RGBQUAD pal[256];

        // Get the colour table
        ::GetDIBColorTable(hMemDC, 0, nColours, pal);

		BYTE nGray;
        // modify the colour table
        for (int x = 0; x < nColours; x++)
        {
            nGray = (pal[x].rgbRed + pal[x].rgbGreen + pal[x].rgbBlue) / 3;
            pal[x].rgbRed = nGray;
            pal[x].rgbGreen = nGray;
            pal[x].rgbBlue = nGray;
        }

        // set the modified colour tab to the DIBSection bitmap
        ::SetDIBColorTable(hMemDC, 0, nColours, pal);
        
		::BitBlt(hDesDC, pt.x, pt.y, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hBmpOld);
		::DeleteDC(hMemDC);
		hMemDC = NULL;
    }
    else
    {   // the bitmap does not have a colour table, so we modify the bitmap bits directly
		int Size = bm.bmHeight * bm.bmWidth;
        
		BITMAPINFO bmi;
		bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biHeight        = bm.bmHeight;
		bmi.bmiHeader.biWidth         = bm.bmWidth;
		bmi.bmiHeader.biPlanes        = 1;
		bmi.bmiHeader.biBitCount      = bm.bmBitsPixel;
		bmi.bmiHeader.biCompression   = BI_RGB;
		bmi.bmiHeader.biSizeImage     = ((bm.bmWidth * bm.bmBitsPixel + 31) & (~31)) / 8 * bm.bmHeight;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed       = 0;
		bmi.bmiHeader.biClrImportant  = 0;
		
		// Get the bitmaps data bits
		BYTE *pBits = new BYTE[bmi.bmiHeader.biSizeImage];
		if(!pBits)
		{
			DeleteObject(hDib);
			return;
		}
		
		::GetDIBits(hDesDC, hDib, 0, bm.bmHeight, pBits, &bmi, DIB_RGB_COLORS);
		
        if (bm.bmBitsPixel == 32)
        {
			DWORD *dst=(DWORD *)pBits;
			int nGray;
			
			while (Size--)
			{
				nGray = Gray(GetBValue(*dst), GetGValue(*dst), GetRValue(*dst));
				*dst = (DWORD)RGB(nGray, nGray, nGray);
				
				dst++;
			}
			
       }
       else if (bm.bmBitsPixel == 24)
       {
            BYTE *dst=(BYTE*)pBits;
			int nGray;
			int pos;
			int rem;

            for (int dh = 0; dh < bm.bmHeight; dh++)
            {
                for (int dw = 0; dw < bm.bmWidth; dw++)
                {
                    nGray = Gray(dst[2], dst[1], dst[0]);
                
                    dst[0]=(BYTE)nGray;
                    dst[1]=(BYTE)nGray;
                    dst[2]=(BYTE)nGray;

                    dst += 3;
                }

                // each row is DWORD aligned, so when we reach the end of a row, we
                // have to realign the pointer to point to the start of the next row
                pos = (int)dst - (int)pBits;
                rem = pos % 4;
                if (rem)
                    dst += 4 - rem;
            }
        }
        else if (bm.bmBitsPixel == 16)
        {
            WORD *dst=(WORD*)pBits;
			BYTE b;
			BYTE g;
			BYTE r;
			int nGray;
            
            while (Size--)
            {
                b = (BYTE)((*dst)&(0x1F));
                g = (BYTE)(((*dst)>>5)&(0x1F));
                r = (BYTE)(((*dst)>>10)&(0x1F));
                
                nGray = Gray(r, g, b);
                *dst = ((WORD)(((BYTE)(nGray)|((WORD)((BYTE)(nGray))<<5))|(((DWORD)(BYTE)(nGray))<<10)));

                dst++;
            }
        }

		// set the modified bitmap data bits to the DIBSection
		::SetDIBits(hDesDC, hDib, 0, bm.bmHeight, pBits, &bmi, DIB_RGB_COLORS);
		delete[] pBits;
		pBits = NULL;
		
		hMemDC = CreateCompatibleDC(hDesDC);

		hBmpOld = (HBITMAP)::SelectObject(hMemDC, hDib);
		::BitBlt(hDesDC, pt.x, pt.y, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, hBmpOld);

		::DeleteDC(hMemDC);
		hMemDC = NULL;
    }
	::DeleteObject(hDib);
}

HFONT CGdiSupport::CreateSysFont()
{
	HFONT hFont = NULL;
	WORD wDefSize = 10;
	HFONT hSysFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (hSysFont == NULL)
	{
		hSysFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	}

	if (hSysFont != NULL)
	{
		LOGFONT lf;
		if (::GetObject(hSysFont, sizeof(LOGFONT), &lf) != 0)
		{
			hFont = ::CreateFontIndirect(&lf);
		}
	}

	return hFont;
}

/////////////////////////////////////////////////////////////////////////////
// 画出渐变矩形
/////////////////////////////////////////////////////////////////////////////
void CGdiSupport::DrawGraidentRect(HDC hdc, const RECT & r, BOOL bTop2Bottom, COLORREF crTop, COLORREF crBottom/*,
					  int nBorders, COLORREF *lpcrBorders*/)
{
//	ASSERT(nBorders>=0 && nBorders<=6);
//	ASSERT( !IsBadReadPtr(lpcrBorders, nBorders * sizeof(COLORREF)) );

	if(r.right < r.left || r.top > r.bottom)
	{
		return;  // not allow for invalid rect
	}

	RECT rc = r;

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;

	vert [0] .x      = rc.left;
	vert [0] .y      = rc.top;
	vert [0] .Red    = GetRValue(crTop) << 8;
	vert [0] .Green  = GetGValue(crTop) << 8;
	vert [0] .Blue   = GetBValue(crTop) << 8;
	vert [0] .Alpha  = 0x0000;
	
	vert [1] .x      = rc.right;
	vert [1] .y      = rc.bottom; 
	vert [1] .Red    = GetRValue(crBottom) << 8;
	vert [1] .Green  = GetGValue(crBottom) << 8;
	vert [1] .Blue   = GetBValue(crBottom) << 8;
	vert [1] .Alpha  = 0x0000;
	
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	GradientFill(hdc, vert, 2,&gRect, 1, 
			bTop2Bottom ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);

}

