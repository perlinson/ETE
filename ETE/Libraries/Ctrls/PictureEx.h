#if !defined(AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_)
#define AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
//#include "..\7fCommon\QfMacro.h"
//#define GIF_TRACING  // uncomment it if you want detailed TRACEs

#ifndef NM_PICTUREEX_FIRST
#define NM_PICTUREEX_FIRST          100
#define NM_PICTUREEX_AFTERPAINT     (NM_PICTUREEX_FIRST+1)
#define NM_PICTUREEX_NCPAINT        (NM_PICTUREEX_FIRST+2)
#endif

#define USE_TIMER

typedef struct tagNMPictureEx {
    NMHDR hdr; 
	HDC   hdc;
	RECT  rcPaint;				
} NMPictureEx, FAR *LPNMPictureEx; 

class CPictureEx : public CWnd
{
	//DECLARE_TOOLTIP;
public:

struct TFrame    // structure that keeps a single frame info
{
	IPicture *m_pPicture;  // pointer to the interface used for drawing
	SIZE     m_frameSize;
	SIZE     m_frameOffset;
	UINT     m_nDelay;     // delay (in 1/100s of a second)
	UINT     m_nDisposal;  // disposal method
};

#pragma pack(1)   // turn byte alignment on

enum GIFBlockTypes
{
	BLOCK_UNKNOWN,
	BLOCK_APPEXT,
	BLOCK_COMMEXT,
	BLOCK_CONTROLEXT,
	BLOCK_PLAINTEXT,
	BLOCK_IMAGE,
	BLOCK_TRAILER
};

enum ControlExtValues // graphic control extension packed field values
{
	GCX_PACKED_DISPOSAL,  // disposal method
	GCX_PACKED_USERINPUT,
	GCX_PACKED_TRANSPCOLOR
};

enum LSDPackedValues  // logical screen descriptor packed field values
{
	LSD_PACKED_GLOBALCT,
	LSD_PACKED_CRESOLUTION,
	LSD_PACKED_SORT,
	LSD_PACKED_GLOBALCTSIZE
};

enum IDPackedValues   // image descriptor packed field values
{
	ID_PACKED_LOCALCT,
	ID_PACKED_INTERLACE,
	ID_PACKED_SORT,
	ID_PACKED_LOCALCTSIZE
};

struct TGIFHeader       // GIF header  
{
	char m_cSignature[3]; // Signature - Identifies the GIF Data Stream
						  // This field contains the fixed value 'GIF'
	char m_cVersion[3];	// Version number. May be one of the following:
						// "87a" or "89a"
};

struct TGIFLSDescriptor // Logical Screen Descriptor
{
	WORD m_wWidth;	// 2 bytes. Logical screen width
	WORD m_wHeight; // 2 bytes. Logical screen height

	unsigned char m_cPacked;      // packed field	

	unsigned char m_cBkIndex;     // 1 byte. Background color index
	unsigned char m_cPixelAspect; // 1 byte. Pixel aspect ratio
	inline int GetPackedValue(enum LSDPackedValues Value);
};

struct TGIFAppExtension // application extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cExtLabel; // app. extension label (0xFF)
	unsigned char m_cBlockSize; // fixed value of 11
	char m_cAppIdentifier[8];   // application identifier
	char m_cAppAuth[3];  // application authentication code
};

struct TGIFControlExt // graphic control extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cControlLabel;  // control extension label (0xF9)
	unsigned char m_cBlockSize; // fixed value of 4
	unsigned char m_cPacked;    // packed field
	WORD m_wDelayTime;	// delay time
	unsigned char m_cTColorIndex; // transparent color index
	unsigned char m_cBlockTerm;   // block terminator (0x00)
public:
	inline int GetPackedValue(enum ControlExtValues Value);
};

struct TGIFCommentExt  // comment extension block
{
	unsigned char m_cExtIntroducer; // extension introducer (0x21)
	unsigned char m_cCommentLabel;  // comment extension label (0xFE)
};

struct TGIFPlainTextExt // plain text extension block
{
	unsigned char m_cExtIntroducer;  // extension introducer (0x21)
	unsigned char m_cPlainTextLabel; // text extension label (0x01)
	unsigned char m_cBlockSize; // fixed value of 12
	WORD m_wLeftPos;    // text grid left position
	WORD m_wTopPos;     // text grid top position
	WORD m_wGridWidth;  // text grid width
	WORD m_wGridHeight; // text grid height
	unsigned char m_cCellWidth;  // character cell width
	unsigned char m_cCellHeight; // character cell height
	unsigned char m_cFgColor; // text foreground color index
	unsigned char m_cBkColor; // text background color index
};

struct TGIFImageDescriptor // image descriptor block
{
	unsigned char m_cImageSeparator; // image separator byte (0x2C)
	WORD m_wLeftPos; // image left position
	WORD m_wTopPos;  // image top position
	WORD m_wWidth;   // image width
	WORD m_wHeight;  // image height
	unsigned char m_cPacked; // packed field
	inline int GetPackedValue(enum IDPackedValues Value);
};

#pragma pack() // turn byte alignment off

public:
	void SetGrayBmp(BOOL bGray);
	void SetAdvURL(LPCTSTR pszAdvURL);
	BOOL GetPaintRect(RECT *lpRect);
	BOOL SetPaintRect(const RECT *lpRect);
	CPictureEx();
	virtual ~CPictureEx();
	void Stop();   // stops animation
	void UnLoad(); // stops animation plus releases all resources

	BOOL IsGIF() const;
	BOOL IsPlaying() const;
	BOOL IsAnimatedGIF() const;
	SIZE GetSize() const;
	int GetFrameCount() const;
	COLORREF GetBkColor() const;
	void SetBkColor(COLORREF clr);

	// draws the picture (starts an animation thread if needed)
	// if an animation was previously stopped by Stop(),
	// continues it from the last displayed frame
	BOOL Draw();

	// loads a picture from a file
	// i.e. Load(_T("mypic.gif"));
	BOOL Load(LPCTSTR szFileName);

	// loads a picture from a global memory block (allocated by GlobalAlloc)
	// Warning: this function DOES NOT free the global memory, pointed to by hGlobal
	BOOL Load(HGLOBAL hGlobal, DWORD dwSize);

	// loads a picture from a program resource
	// i.e. Load(MAKEINTRESOURCE(IDR_MYPIC),_T("GIFTYPE"));
	BOOL Load(LPCTSTR szResourceName,LPCTSTR szResourceType);

	BOOL IsAutoSize() const { return m_bAutoSize; }
	BOOL IsAlignCenter() const { return m_bAlignCenter; }
	BOOL SetAutoSize(BOOL bAutoSize);
	BOOL SetAlighCenter(BOOL bAlignCenter);	
	BOOL SetAutoStretch(BOOL bAutoStretch);

	int  GetBorderWidth() const;
	int  SetBorderWidth(int nWidth); // border range: 0 ~ 3
	COLORREF SetBorderColor(COLORREF crBorder);

	BOOL SetHandCursor(BOOL bHandCursor);
	BOOL SetParentBkgndDynamic(BOOL bDynamic);
protected:
	CString   m_strAdvURL;

#ifdef GIF_TRACING
	void EnumGIFBlocks();
	void WriteDataOnDisk(CString szFileName, HGLOBAL hData, DWORD dwSize);
#endif // GIF_TRACING

	RECT m_PaintRect;
	SIZE m_PictureSize;
	COLORREF m_clrBackground;
	UINT m_nCurrFrame;
	UINT m_nDataSize;
	UINT m_nCurrOffset;
	UINT m_nGlobalCTSize;
	BOOL m_bIsGIF;
//	BOOL m_bIsPlaying;
	BOOL m_bIsInitialized;
	HDC m_hMemDC;

	HDC m_hDispMemDC;
	HBITMAP m_hDispMemBM;
	HBITMAP m_hDispOldBM;

	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
#ifdef USE_TIMER
	UINT   m_uTimerId;
#else
	volatile BOOL m_bExitThread;
	HANDLE m_hThread;
	HANDLE m_hExitEvent;
#endif
	IPicture * m_pPicture;
	TGIFHeader * m_pGIFHeader;
	unsigned char * m_pRawData;
	TGIFLSDescriptor * m_pGIFLSDescriptor;
	std::vector<TFrame> m_arrFrames;

#ifndef USE_TIMER
	void ThreadAnimation();
	static UINT WINAPI _ThreadAnimation(LPVOID pParam);
#endif

	int GetNextBlockLen() const;
	BOOL SkipNextBlock();
	BOOL SkipNextGraphicBlock();
	BOOL PrepareDC(int nWidth, int nHeight);
	void ResetDataPointer();
	enum GIFBlockTypes GetNextBlock() const;
	UINT GetSubBlocksLen(UINT nStartingOffset) const;
	HGLOBAL GetNextGraphicBlock(UINT *pBlockLen, UINT *pDelay, 
		SIZE *pBlockSize, SIZE *pBlockOffset, UINT *pDisposal);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureEx)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CPictureEx)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

protected:
#ifdef USE_TIMER
	void PrepareTimer();
#endif
private:
	BOOL m_bGrayBmp;		// 是否灰度处理图像
	BOOL m_bAutoStretch;
	BOOL m_bAutoSize;      // Add by benkebo
	BOOL m_bAlignCenter;   // Add by benkebo
	CBrush m_brBackground; // Add by benkebo
	BOOL   m_bHandCursor;

	int    m_nBorderWidth;
	COLORREF m_crBorder;

	BOOL   m_bDynamicParentBkgnd; // 父窗口背景不变化
	CRect  m_rcOld;
};

#endif // !defined(AFX_PICTUREEX_H__0EFE5DE0_7B68_4DB7_8B34_5DC634948438__INCLUDED_)
