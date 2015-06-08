#pragma once

#if !defined(ArraySize)
#ifdef _countof
#define ArraySize(ar)  _countof(ar)
#else
#define ArraySize(ar)  (sizeof(ar) / sizeof(ar[0]))
#endif
#endif // !defined(ArraySize)

#define ReleasePtr(ptr)  { if(ptr) { ptr->Release(); ptr = NULL; }}
#define DeletePtr(ptr)   { if(ptr) { delete ptr; ptr = NULL; }}
#define DeleteArrPtr(ptr) { if(ptr) { delete []ptr; ptr = NULL; }}

#define ReleaseHandle(h)  { if( h ) { ::CloseHandle(h); h = NULL; }}
#define DeleteGdiObj(hGdi) { if(hGdi) { DeleteObject(hGdi); hGdi = NULL; }}

#define StopTimer(hwnd, timerId) { if( timerId ) { ::KillTimer(hwnd, timerId); timerId = 0; }}
#define HAS_STYLE(flag, sub) ( 0!=sub && (flag & sub)==sub )

#ifndef IsNormalRect
#define IsNormalRect(rc) (rc.right>=rc.left && rc.bottom>=rc.top)
#endif

#ifndef  EM_SETCUEBANNER
#define EM_SETCUEBANNER 0x1501  // 设置Edit框，提示输入的文字 WPARAM(BOOL bFocusShow), LPARAM(Text)
#endif

#define EnableDlgItem(hwnd, id, bEnable) { HWND hChild = ::GetDlgItem(hwnd, id); if( hChild ) { ::EnableWindow(hChild, bEnable); } }
#define ShowDlgItem(hwnd, id, bShow) { HWND hChild = ::GetDlgItem(hwnd, id); if( hChild) { ::ShowWindow(hChild, bShow ? SW_SHOW : SW_HIDE);} }
#define BringWndToTop(hwnd) \
{  \
	if( !::IsWindowVisible(hwnd)) \
	{ \
		::ShowWindow(hwnd, SW_SHOW); \
	} \
	if( ::IsIconic(hwnd)) \
	{ \
		::ShowWindow(hwnd, SW_RESTORE); \
	} \
	::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE); \
	::SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE); \
	::BringWindowToTop(hwnd); \
}

#define COLOR_WHITE		RGB(255, 255, 255)
#define COLOR_BLACK		0
#define COLOR_RED		RGB(255, 0, 0)
#define COLOR_GREEN		RGB(0, 255, 0)
#define COLOR_MASK		RGB(255, 0, 255)
#define COLOR_BLUE		RGB(0, 0, 255)