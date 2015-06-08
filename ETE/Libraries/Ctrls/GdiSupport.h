#pragma once
#include "CommonDefine.h"
class CGdiSupport
{
public:
	CGdiSupport(void);
	~CGdiSupport(void);

	static HBITMAP GetSubAreaBmp(HBITMAP hBmpBig, int nLeft, int nTop, int nWidth, int nHeight);
	static HBITMAP CopyBitmap (HBITMAP hbm);
	static BOOL PasteBmp(HDC hDC, HBITMAP hBmpSrc, int nLeft, int nTop, int nWidth=-1, 
						int nHeight=-1, int nSrcX=0, int nSrcY=0);
	static BOOL StretchBmp(HDC hDC, HBITMAP hBmpSrc, int nLeft, int nTop, int nWidth, 
						int nHeight, int nSrcX=0, int nSrcY=0, int nSrcW=-1, int nSrcH=-1,
						COLORREF crTransparent=-1);

	static void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, 
					   int nXDest, int nYDest,  int nWidth=-1, int nHeight=-1,
					   int nXSrc=0,  int nYSrc=0,
					   COLORREF cTransparentColor=RGB(255, 0, 255));

	static BOOL AddLayerMaskEffect(HDC hdc, const RECT& rc, COLORREF crMask, BYTE btAlphaValue);

	// 由位图得到区域，去掉透明色部分，cTolerance为透明色允许的偏差
	static HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance);

	// 由bitmap文件数据的缓冲区创建位图句柄
	static HBITMAP CreateBmpFromFileBuf(LPBYTE lpFileBuf, HDC hdc);


	static void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr);
	static void FillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF clr);
	static void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
	static void Draw3dRect(HDC hdc, LPCRECT lpRect,	COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void  ScreenToClient(HWND hwnd, LPRECT lpRect );
	static void  ClientToScreen(HWND hwnd, LPRECT lpRect );

	// 替换设备场景中的颜色
	static BOOL ReplaceOneColor(HDC hdc, LPCRECT lpRect, COLORREF crSrc, COLORREF crDst);

	static HDC CreateMemDC(HDC hdcSrc, HBITMAP *lphBmpMem, HBITMAP * lphBmpOld, int width, int height);
	static void DeflateRect(HRGN hRgn, int l, int t, int r, int b);
	static void DrawBorderEx(HWND hwnd, HDC hdc, int nBorders, const COLORREF *lpcrBorders);
	static void DrawBorderEx(HDC hdc, const RECT & r, int nWidthEllipse, int nHeightEllipse, int nBorders, const COLORREF *lpcrBorders, BOOL bNoRoundBottom=FALSE);

	static void  DrawTransparentBkgnd( HWND hWnd, HDC hdc, HWND hWndTransparentParent=NULL, LPCRECT lprcClient=NULL );
	static void GetSystemFont(LPTSTR lpFontName, int nNameMax, long & nPointSize, BOOL bPointSize=TRUE);
	static HFONT CreateSysFont();
	static void DrawGrayBmp(HDC hDesDC, POINT pt, HBITMAP hSrcBitmap);

	static void DrawGraidentRect(HDC hdc, const RECT & r, BOOL bTop2Bottom, COLORREF crTop,
						  COLORREF crBottom/*,  int nBorders, COLORREF *lpcrBorders*/);

};

