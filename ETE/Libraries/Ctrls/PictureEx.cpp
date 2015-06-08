#include "stdafx.h"
#include "GdiSupport.h"
#include "PictureEx.h"
#include <process.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//IMPLEMENT_TOOLTIP(CPictureEx, CWnd)

//////////////////////////////////////////////////////////////////////
// Nested structures member functions
//////////////////////////////////////////////////////////////////////

inline int CPictureEx::TGIFControlExt::GetPackedValue(enum ControlExtValues Value)
{
	int nRet = (int)m_cPacked;
	switch (Value)
	{
	case GCX_PACKED_DISPOSAL:
		nRet = (nRet & 28) >> 2;
		break;

	case GCX_PACKED_USERINPUT:
		nRet = (nRet & 2) >> 1;
		break;

	case GCX_PACKED_TRANSPCOLOR:
		nRet &= 1;
		break;
	};

	return nRet;
}

inline int CPictureEx::TGIFLSDescriptor::GetPackedValue(enum LSDPackedValues Value)
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
	case LSD_PACKED_GLOBALCT:
		nRet = nRet >> 7;
		break;

	case LSD_PACKED_CRESOLUTION:
		nRet = ((nRet & 0x70) >> 4) + 1;
		break;

	case LSD_PACKED_SORT:
		nRet = (nRet & 8) >> 3;
		break;

	case LSD_PACKED_GLOBALCTSIZE:
		nRet &= 7;
		break;
	};

	return nRet;
}

inline int CPictureEx::TGIFImageDescriptor::GetPackedValue(enum IDPackedValues Value)
{
	int nRet = (int)m_cPacked;

	switch (Value)
	{
	case ID_PACKED_LOCALCT:
		nRet >>= 7;
		break;

	case ID_PACKED_INTERLACE:
		nRet = ((nRet & 0x40) >> 6);
		break;

	case ID_PACKED_SORT:
		nRet = (nRet & 0x20) >> 5;
		break;

	case ID_PACKED_LOCALCTSIZE:
		nRet &= 7;
		break;
	};

	return nRet;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPictureEx::CPictureEx()
{
	// check structures size
	ASSERT(sizeof(TGIFImageDescriptor) == 10);
	ASSERT(sizeof(TGIFAppExtension)    == 14);
	ASSERT(sizeof(TGIFPlainTextExt)    == 15);
	ASSERT(sizeof(TGIFLSDescriptor)    ==  7);
	ASSERT(sizeof(TGIFControlExt)	   ==  8);
	ASSERT(sizeof(TGIFCommentExt)	   ==  2);
	ASSERT(sizeof(TGIFHeader)		   ==  6);

	m_pGIFLSDescriptor = NULL;
	m_pGIFHeader	   = NULL;
	m_pPicture		   = NULL;
	m_pRawData		   = NULL;
	m_hBitmap          = NULL;
	m_hMemDC		   = NULL;

	m_hDispMemDC       = NULL;
	m_hDispMemBM       = NULL;
	m_hDispOldBM       = NULL;

	m_bIsInitialized   = FALSE;
#ifdef USE_TIMER
	m_uTimerId = NULL;
#else
	m_hThread		   = NULL;
	m_bExitThread	   = FALSE;
	m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);	
//	m_bIsPlaying       = FALSE;
#endif
	m_bIsGIF		   = FALSE;
	//m_clrBackground    = RGB(255,255,255); // white by default
	m_nGlobalCTSize    = 0;
	m_nCurrOffset	   = 0;
	m_nCurrFrame	   = 0;
	m_nDataSize		   = 0;
	m_PictureSize.cx = m_PictureSize.cy = 0;
	SetRect(&m_PaintRect,0,0,0,0);

	m_bAutoStretch = FALSE;
	m_bAutoSize = TRUE;
	m_bAlignCenter = FALSE;
	
	m_bHandCursor = FALSE;
	
	m_nBorderWidth = 0;
	m_crBorder = ::GetSysColor(COLOR_WINDOWFRAME);

	m_bGrayBmp = FALSE;
	m_bDynamicParentBkgnd = FALSE;
	m_rcOld.SetRectEmpty();
	SetBkColor(COLOR_WHITE);
}

CPictureEx::~CPictureEx()
{
	UnLoad();

#ifndef USE_TIMER
	if( m_hExitEvent )
	{
		CloseHandle(m_hExitEvent);
	}
#endif
}

BEGIN_MESSAGE_MAP(CPictureEx, CWnd)
	//{{AFX_MSG_MAP(CPictureEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPictureEx::Load(HGLOBAL hGlobal, DWORD dwSize)
{
	IStream *pStream = NULL;
	UnLoad();

	if (!(m_pRawData = reinterpret_cast<unsigned char*> (GlobalLock(hGlobal))) )
	{
		TRACE(_T("Load: Error locking memory\n"));
		return FALSE;
	};

	m_nDataSize = dwSize;
	m_pGIFHeader = reinterpret_cast<TGIFHeader *> (m_pRawData);

	if ((memcmp(&m_pGIFHeader->m_cSignature,"GIF",3) != 0) &&
		((memcmp(&m_pGIFHeader->m_cVersion,"87a",3) != 0) ||
		 (memcmp(&m_pGIFHeader->m_cVersion,"89a",3) != 0)) )
	{
	// it's neither GIF87a nor GIF89a
	// do the default processing

		// clear GIF variables
		m_pRawData = NULL;
		GlobalUnlock(hGlobal);

		// don't delete memory on object's release
		if (CreateStreamOnHGlobal(hGlobal,FALSE,&pStream) != S_OK)
			return FALSE;

		if (OleLoadPicture(pStream,dwSize,FALSE,IID_IPicture,
			reinterpret_cast<LPVOID *>(&m_pPicture)) != S_OK)
		{
			pStream->Release();
			return FALSE;
		};
		pStream->Release();

		// store picture's size

		long hmWidth;
		long hmHeight;
		m_pPicture->get_Width(&hmWidth);
		m_pPicture->get_Height(&hmHeight);

		HDC hDC = ::GetDC(m_hWnd);
		m_PictureSize.cx = MulDiv(hmWidth, GetDeviceCaps(hDC,LOGPIXELSX), 2540);
		m_PictureSize.cy = MulDiv(hmHeight, GetDeviceCaps(hDC,LOGPIXELSY), 2540);
		::ReleaseDC(m_hWnd,hDC);
	}
	else
	{
		// it's a GIF
		m_bIsGIF = TRUE;
		m_pGIFLSDescriptor = reinterpret_cast<TGIFLSDescriptor *>
			(m_pRawData + sizeof(TGIFHeader));
		if (m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT) == 1)
		{
			// calculate the globat color table size
			m_nGlobalCTSize = static_cast<int>
				(3* (1 << (m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE)+1)));
			// get the background color if GCT is present
			unsigned char *pBkClr = m_pRawData + sizeof(TGIFHeader) + 
				sizeof(TGIFLSDescriptor) + 3*m_pGIFLSDescriptor->m_cBkIndex;
			m_clrBackground = RGB(pBkClr[0],pBkClr[1],pBkClr[2]);
		};

		// store the picture's size
		m_PictureSize.cx = m_pGIFLSDescriptor->m_wWidth;
		m_PictureSize.cy = m_pGIFLSDescriptor->m_wHeight;

		// determine frame count for this picture
		UINT nFrameCount=0;
		ResetDataPointer();
		while (SkipNextGraphicBlock())
			nFrameCount++;

#ifdef GIF_TRACING
		TRACE(
			_T(" -= GIF encountered\n"
			   "Logical Screen dimensions = %dx%d\n"
			   "Global color table = %d\n"
			   "Color depth = %d\n"
			   "Sort flag = %d\n"
			   "Size of Global Color Table = %d\n"
			   "Background color index = %d\n"
			   "Pixel aspect ratio = %d\n"
			   "Frame count = %d\n"
			   "Background color = %06Xh\n\n"
			  ),
			m_pGIFLSDescriptor->m_wWidth,
			m_pGIFLSDescriptor->m_wHeight,
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCT),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_CRESOLUTION),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_SORT),
			m_pGIFLSDescriptor->GetPackedValue(LSD_PACKED_GLOBALCTSIZE),
			m_pGIFLSDescriptor->m_cBkIndex,
			m_pGIFLSDescriptor->m_cPixelAspect,
			nFrameCount,
			m_clrBackground
			);
		EnumGIFBlocks();
#endif

		if (nFrameCount == 0) // it's an empty GIF!
		{
			m_pRawData = NULL;
			GlobalUnlock(hGlobal);
			return FALSE;
		};

		// now check the frame count
		// if there's only one frame, no need to animate this GIF
		// therefore, treat it like any other pic

		if (nFrameCount == 1)
		{
			// clear GIF variables
			m_pRawData = NULL;
			GlobalUnlock(hGlobal);

			// don't delete memory on object's release
			if (CreateStreamOnHGlobal(hGlobal,FALSE,&pStream) != S_OK)
				return FALSE;

			if (OleLoadPicture(pStream,dwSize,FALSE,IID_IPicture,
				(LPVOID *)&m_pPicture) != S_OK)
			{
				pStream->Release();
				return FALSE;
			};

			pStream->Release();
		}
		else
		{
		// if, on the contrary, there are several frames
		// then store separate frames in an array

			TFrame frame;
			UINT nBlockLen;
			HGLOBAL hFrameData;
			UINT nCurFrame = 0;

			ResetDataPointer();
			while (hFrameData = GetNextGraphicBlock(&nBlockLen,
				&frame.m_nDelay, &frame.m_frameSize,
				&frame.m_frameOffset, &frame.m_nDisposal) )
			{
				#ifdef GIF_TRACING
				//////////////////////////////////////////////
				// uncomment the following strings if you want 
				// to write separate frames on disk
				//
				//	CString szName;
				//	szName.Format(_T("%.4d.gif"),nCurFrame);
				//	WriteDataOnDisk(szName,hFrameData,nBlockLen);
				//	nCurFrame++;
				#endif // GIF_TRACING

				IStream *pStream = NULL;

				// delete memory on object's release
				if (CreateStreamOnHGlobal(hFrameData,TRUE,&pStream) != S_OK)
				{
					GlobalFree(hFrameData);
					continue;
				};

				if (OleLoadPicture(pStream,nBlockLen,FALSE,
					IID_IPicture,
					reinterpret_cast<LPVOID *>(&frame.m_pPicture)) != S_OK)
				{
					pStream->Release();
					continue;
				};
				pStream->Release();
			
				// everything went well, add this frame
				m_arrFrames.push_back(frame);
			};

			// clean after ourselves
			m_pRawData = NULL;
			GlobalUnlock(hGlobal);

			if (m_arrFrames.empty()) // couldn't load any frames
				return FALSE;
		};
	}; // if (!IsGIF...

	return PrepareDC(m_PictureSize.cx,m_PictureSize.cy);
}

void CPictureEx::UnLoad()
{
	Stop();
	if (m_pPicture)
	{
		m_pPicture->Release();
		m_pPicture = NULL;
	};
	
	std::vector<TFrame>::iterator it;
	for (it=m_arrFrames.begin();it<m_arrFrames.end();it++)
	{
		if( AfxIsValidAddress((*it).m_pPicture, sizeof(* ((*it).m_pPicture)), TRUE))
		{
			(*it).m_pPicture->Release();
		}
	}
	m_arrFrames.clear();

	if (m_hMemDC)
	{
		SelectObject(m_hMemDC,m_hOldBitmap);
		::DeleteDC(m_hMemDC);
		::DeleteObject(m_hBitmap);
		m_hMemDC  = NULL;
		m_hBitmap = NULL;
	};

	if (m_hDispMemDC)
	{
		SelectObject(m_hDispMemDC,m_hDispOldBM);
		::DeleteDC(m_hDispMemDC);
		::DeleteObject(m_hDispMemBM);
		m_hDispMemDC  = NULL;
		m_hDispMemBM = NULL;
	};

	SetRect(&m_PaintRect,0,0,0,0);
	m_pGIFLSDescriptor = NULL;
	m_pGIFHeader	   = NULL;
	m_pRawData		   = NULL;
	m_bIsInitialized   = FALSE;

#ifdef USE_TIMER
	m_uTimerId = 0;
#else
	m_hThread		   = NULL;
	m_bExitThread	   = FALSE;
#endif
	m_bIsGIF		   = FALSE;
	m_clrBackground    = RGB(255,255,255); // white by default
	m_nGlobalCTSize	   = 0;
	m_nCurrOffset	   = 0;
	m_nCurrFrame	   = 0;
	m_nDataSize		   = 0;

//	m_brBackground.DeleteObject();
}

BOOL CPictureEx::Draw()
{
	if (!m_bIsInitialized)
	{
		TRACE(_T("Call one of the CPictureEx::Load() member functions before calling Draw()\n"));
		return FALSE;
	};

	if (IsAnimatedGIF())
	{
	// the picture needs animation
	// we'll start the thread that will handle it for us
		m_rcOld.SetRectEmpty();
#ifndef USE_TIMER
		unsigned int nDummy;
		m_hThread = (HANDLE) _beginthreadex(NULL,0,_ThreadAnimation,this,
			CREATE_SUSPENDED,&nDummy);
		if (!m_hThread || INVALID_HANDLE_VALUE==m_hThread)
		{
			TRACE(_T("Draw: Couldn't start a GIF animation thread\n"));
			m_hThread = NULL;
			return FALSE;
		} 
		else 
			ResumeThread(m_hThread);
#else
		PrepareTimer();
		return TRUE;
#endif
	} 
	else
	{
		if (m_pPicture)
		{
			long hmWidth;
			long hmHeight;
			m_pPicture->get_Width(&hmWidth);
			m_pPicture->get_Height(&hmHeight);
			if (m_pPicture->Render(m_hMemDC, 0, 0, m_PictureSize.cx, m_PictureSize.cy, 
				0, hmHeight, hmWidth, -hmHeight, NULL) == S_OK)
			{
				Invalidate(FALSE);
				return TRUE;
			};
		};
	};

	return FALSE;	
}

SIZE CPictureEx::GetSize() const
{
	return m_PictureSize;
}

BOOL CPictureEx::Load(LPCTSTR szFileName)
{
	ASSERT(szFileName);
	
	CFile file;
	HGLOBAL hGlobal;
	DWORD dwSize;

	if (!file.Open(szFileName,
				CFile::modeRead | 
				CFile::shareDenyWrite) )
	{
		TRACE(_T("Load (file): Error opening file %s\n"),szFileName);
		return FALSE;
	};

	dwSize = (DWORD)file.GetLength();
	hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD,dwSize);
	if (!hGlobal)
	{
		TRACE(_T("Load (file): Error allocating memory\n"));
		return FALSE;
	};
	
	char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
	if (!pData)
	{
		TRACE(_T("Load (file): Error locking memory\n"));
		GlobalFree(hGlobal);
		return FALSE;
	};

	try
	{
		file.Read(pData,dwSize);
	}
	catch(CFileException* e)                                          
	{
		TRACE(_T("Load (file): An exception(%ld) occured while reading the file %s\n"),
			e->m_cause, szFileName);
		GlobalFree(hGlobal);
		e->Delete();
		file.Close();
		return FALSE;
	}
	GlobalUnlock(hGlobal);
	file.Close();

	BOOL bRetValue = Load(hGlobal,dwSize);
	GlobalFree(hGlobal);
	return bRetValue;
}

BOOL CPictureEx::Load(LPCTSTR szResourceName, LPCTSTR szResourceType)
{
	ASSERT(szResourceName);
	ASSERT(szResourceType);

	HRSRC hPicture = FindResource(AfxGetResourceHandle(),szResourceName,szResourceType);
	HGLOBAL hResData;
	if (!hPicture || !(hResData = LoadResource(AfxGetResourceHandle(),hPicture)))
	{
		TRACE(_T("Load (resource): Error loading resource %s\n"),szResourceName);
		return FALSE;
	};
	DWORD dwSize = SizeofResource(AfxGetResourceHandle(),hPicture);

	// hResData is not the real HGLOBAL (we can't lock it)
	// let's make it real

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD,dwSize);
	if (!hGlobal)
	{
		TRACE(_T("Load (resource): Error allocating memory\n"));
		FreeResource(hResData);
		return FALSE;
	};
	
	char *pDest = reinterpret_cast<char *> (GlobalLock(hGlobal));
	char *pSrc = reinterpret_cast<char *> (LockResource(hResData));
	if (!pSrc || !pDest)
	{
		TRACE(_T("Load (resource): Error locking memory\n"));
		GlobalFree(hGlobal);
		FreeResource(hResData);
		return FALSE;
	};
	CopyMemory(pDest,pSrc,dwSize);
	FreeResource(hResData);
	GlobalUnlock(hGlobal);

	BOOL bRetValue = Load(hGlobal,dwSize);
	GlobalFree(hGlobal);
	return bRetValue;
}

void CPictureEx::ResetDataPointer()
{
	// skip header and logical screen descriptor
	m_nCurrOffset = 
		sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize;
}

BOOL CPictureEx::SkipNextGraphicBlock()
{
	if (!m_pRawData) return FALSE;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	enum GIFBlockTypes nBlock;

	nBlock = GetNextBlock();

	while ((nBlock != BLOCK_CONTROLEXT) &&
		   (nBlock != BLOCK_IMAGE) &&
		   (nBlock != BLOCK_PLAINTEXT) &&
		   (nBlock != BLOCK_UNKNOWN) &&
		   (nBlock != BLOCK_TRAILER) )
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return FALSE;

	// it's either a control ext.block, an image or a plain text

	if (GetNextBlockLen() <= 0) return FALSE;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		if (!SkipNextBlock()) return FALSE;
		nBlock = GetNextBlock();

		// skip everything until we meet an image block or a plain-text block
		while ((nBlock != BLOCK_IMAGE) &&
			   (nBlock != BLOCK_PLAINTEXT) &&
			   (nBlock != BLOCK_UNKNOWN) &&
			   (nBlock != BLOCK_TRAILER) )
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
		};

		if ((nBlock == BLOCK_UNKNOWN) ||
			(nBlock == BLOCK_TRAILER))
			return FALSE;
	};

	// skip the found data block (image or plain-text)
	if (!SkipNextBlock()) return FALSE;

	return TRUE;
}

UINT CPictureEx::GetSubBlocksLen(UINT nStartingOffset) const
{
	UINT nRet = 0;
	UINT nCurOffset = nStartingOffset;
	
	while (m_pRawData[nCurOffset] != 0)
	{
		nRet += m_pRawData[nCurOffset]+1;
		nCurOffset += m_pRawData[nCurOffset]+1;
	};

	return nRet+1;
}

enum CPictureEx::GIFBlockTypes CPictureEx::GetNextBlock() const
{
	switch(m_pRawData[m_nCurrOffset])
	{
	case 0x21:
	// extension block
		switch(m_pRawData[m_nCurrOffset+1])
		{
		case 0x01:
		// plain text extension
			return BLOCK_PLAINTEXT;
			break;

		case 0xF9:
		// graphic control extension
			return BLOCK_CONTROLEXT;
			break;

		case 0xFE:
		// comment extension
			return BLOCK_COMMEXT;
			break;

		case 0xFF:
		// application extension
			return BLOCK_APPEXT;
			break;
		};
		break;
	
	case 0x3B:
	// trailer
		return BLOCK_TRAILER;
		break;

	case 0x2C:
	// image data
		return BLOCK_IMAGE;
		break;
	};

	return BLOCK_UNKNOWN;
}

BOOL CPictureEx::SkipNextBlock()
{
	if (!m_pRawData) return FALSE;

	int nLen = GetNextBlockLen();
	if ((nLen <= 0) || ((m_nCurrOffset+nLen) > m_nDataSize))
		return FALSE;

	m_nCurrOffset += nLen;
	return TRUE;
}

int CPictureEx::GetNextBlockLen() const
{
	GIFBlockTypes nBlock = GetNextBlock();

	int nTmp;

	switch(nBlock)
	{
	case BLOCK_UNKNOWN:
		return -1;
		break;

	case BLOCK_TRAILER:
		return 1;
		break;

	case BLOCK_APPEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFAppExtension));
		if (nTmp > 0)
			return sizeof(TGIFAppExtension)+nTmp;
		break;

	case BLOCK_COMMEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFCommentExt));
		if (nTmp > 0)
			return sizeof(TGIFCommentExt)+nTmp;
		break;

	case BLOCK_CONTROLEXT:
		return sizeof(TGIFControlExt);
		break;

	case BLOCK_PLAINTEXT:
		nTmp = GetSubBlocksLen(m_nCurrOffset+sizeof(TGIFPlainTextExt));
		if (nTmp > 0)
			return sizeof(TGIFPlainTextExt)+nTmp;
		break;

	case BLOCK_IMAGE:
		TGIFImageDescriptor *pIDescr = 
			reinterpret_cast<TGIFImageDescriptor *> (&m_pRawData[m_nCurrOffset]);
		int nLCTSize = (int)
			(pIDescr->GetPackedValue(ID_PACKED_LOCALCT)*3*
			(1 << (pIDescr->GetPackedValue(ID_PACKED_LOCALCTSIZE)+1)));

		int nTmp = GetSubBlocksLen(m_nCurrOffset+
			sizeof(TGIFImageDescriptor) + nLCTSize + 1);
		if (nTmp > 0)
			return sizeof(TGIFImageDescriptor) + nLCTSize + 1 + nTmp;
		break;
	};

	return 0;
}

#ifndef USE_TIMER
UINT WINAPI CPictureEx::_ThreadAnimation(LPVOID pParam)
{
	ASSERT(pParam);
	CPictureEx *pPic = reinterpret_cast<CPictureEx *> (pParam);

//	pPic->m_bIsPlaying = TRUE;
	pPic->ThreadAnimation();
//	pPic->m_bIsPlaying = FALSE;

//	// this thread has finished its work so we close the handle
//	if( pPic->m_hThread )
//	{
//		CloseHandle(pPic->m_hThread); 
//		// and init the handle to zero (so that Stop() doesn't Wait on it)
//		pPic->m_hThread = 0;
//	}

	return 0;
}

void CPictureEx::ThreadAnimation()
{
	TFrame curFrame = m_arrFrames[m_nCurrFrame];

	// first, restore background (for stop/draw support)
	// disposal method #2
	if (curFrame.m_nDisposal == 2)
	{
		if( (HBRUSH)m_brBackground == NULL )
		{
			CGdiSupport::DrawTransparentBkgnd(m_hWnd, m_hMemDC);
		}
		else
		{
			RECT rect = {
				curFrame.m_frameOffset.cx,
				curFrame.m_frameOffset.cy,
				curFrame.m_frameOffset.cx + curFrame.m_frameSize.cx,
				curFrame.m_frameOffset.cy + curFrame.m_frameSize.cy };
			FillRect(m_hMemDC,&rect, (HBRUSH)m_brBackground);
		}
	} 
	else
		// disposal method #3
		if (m_hDispMemDC && (curFrame.m_nDisposal == 3) )
		{
			// put it back
			BitBlt(m_hMemDC,
				curFrame.m_frameOffset.cx,
				curFrame.m_frameOffset.cy,
				curFrame.m_frameSize.cx,
				curFrame.m_frameSize.cy,
				m_hDispMemDC,0,0, SRCCOPY);
			// init variables
			SelectObject(m_hDispMemDC,m_hDispOldBM);
			DeleteDC(m_hDispMemDC); m_hDispMemDC = NULL;
			DeleteObject(m_hDispMemBM); m_hDispMemBM = NULL;
		};

	while (!m_bExitThread)
	{
		if( m_nCurrFrame >= m_arrFrames.size() )
		{
			TRACE2("frame out of range, %ld, %ld\n", m_nCurrFrame, m_arrFrames.size());
			ASSERT(FALSE);
			break;
		}

		curFrame = m_arrFrames[m_nCurrFrame];

		if (curFrame.m_pPicture)
		{
		///////////////////////////////////////////////////////
		// Before rendering a frame we should take care of what's 
		// behind that frame. TFrame::m_nDisposal will be our guide:
		//   0 - no disposal specified (do nothing)
		//   1 - do not dispose (again, do nothing)
		//   2 - restore to background color (m_clrBackground)
		//   3 - restore to previous

			//////// disposal method #3
			if (curFrame.m_nDisposal == 3)
			{
				// prepare a memory DC and store the background in it
				m_hDispMemDC = CreateCompatibleDC(m_hMemDC);
				m_hDispMemBM = CreateCompatibleBitmap(m_hMemDC,
							curFrame.m_frameSize.cx,
							curFrame.m_frameSize.cy);
				
				if (m_hDispMemDC && m_hDispMemBM)
				{
					m_hDispOldBM = reinterpret_cast<HBITMAP> (SelectObject(m_hDispMemDC,m_hDispMemBM));
					BitBlt(m_hDispMemDC,0,0,
						curFrame.m_frameSize.cx,
						curFrame.m_frameSize.cy,
						m_hMemDC,
						curFrame.m_frameOffset.cx,
						curFrame.m_frameOffset.cy,
						SRCCOPY);
				};
			};
			///////////////////////

			long hmWidth;
			long hmHeight;
			curFrame.m_pPicture->get_Width(&hmWidth);
			curFrame.m_pPicture->get_Height(&hmHeight);

			if (curFrame.m_pPicture->Render(m_hMemDC, 
				curFrame.m_frameOffset.cx, 
				curFrame.m_frameOffset.cy, 
				curFrame.m_frameSize.cx, 
				curFrame.m_frameSize.cy, 
				0, hmHeight, hmWidth, -hmHeight, NULL) == S_OK)
			{
				Invalidate(FALSE);
			};
			
			if (m_bExitThread) break;

			// if the delay time is too short (like in old GIFs), wait for 100ms
			if (curFrame.m_nDelay < 5) 
				WaitForSingleObject(m_hExitEvent, 100);
			else
				WaitForSingleObject(m_hExitEvent, 10*curFrame.m_nDelay);

			if (m_bExitThread) break;

			// disposal method #2
			if (curFrame.m_nDisposal == 2)
			{
				if( (HBRUSH)m_brBackground == NULL )
				{
					CGdiSupport::DrawTransparentBkgnd(m_hWnd, m_hMemDC);
				}
				else
				{
					RECT rect = {
						curFrame.m_frameOffset.cx,
							curFrame.m_frameOffset.cy,
							curFrame.m_frameOffset.cx + curFrame.m_frameSize.cx,
							curFrame.m_frameOffset.cy + curFrame.m_frameSize.cy };
						FillRect(m_hMemDC,&rect, (HBRUSH)m_brBackground);
				}
			} 
			else
				if (m_hDispMemDC && (curFrame.m_nDisposal == 3) )
				{
					// put it back
					BitBlt(m_hMemDC,
						curFrame.m_frameOffset.cx,
						curFrame.m_frameOffset.cy,
						curFrame.m_frameSize.cx,
						curFrame.m_frameSize.cy,
						m_hDispMemDC,0,0, SRCCOPY);
					// init variables
					SelectObject(m_hDispMemDC,m_hDispOldBM);
					DeleteDC(m_hDispMemDC); m_hDispMemDC = NULL;
					DeleteObject(m_hDispMemBM); m_hDispMemBM = NULL;
				};
		};
		m_nCurrFrame++;
		if (m_nCurrFrame == m_arrFrames.size())
		{
			m_nCurrFrame = 0; 
			// init the screen for the first frame,

			if( (HBRUSH)m_brBackground == NULL )
			{
				CGdiSupport::DrawTransparentBkgnd(m_hWnd, m_hMemDC);
			}
			else
			{
				RECT rect = {0,0,m_PictureSize.cx,m_PictureSize.cy};
				FillRect(m_hMemDC,&rect,(HBRUSH)m_brBackground);
			}
		};
	};
}
#endif

void CPictureEx::Stop()
{
#ifdef USE_TIMER
	if(  0 != m_uTimerId )
	{
		StopTimer(m_hWnd, m_uTimerId);
	}
#else
	if( NULL == m_hThread )
	{
		return;
	}

//	m_bIsPlaying = FALSE;
	if (m_hThread)
	{
		HANDLE hThread = m_hThread;
		m_bExitThread = TRUE;
		SetEvent(m_hExitEvent);

		// we'll wait for 5 seconds then continue execution
		if( WAIT_TIMEOUT == WaitForSingleObject(hThread,500))
		{
			TerminateThread(hThread, -1);
		}
		
		CloseHandle(hThread);
		m_hThread = NULL;

		// make it possible to Draw() again
		ResetEvent(m_hExitEvent);
		m_bExitThread = FALSE;
	}
#endif
}

HGLOBAL CPictureEx::GetNextGraphicBlock(UINT *pBlockLen, 
	UINT *pDelay, SIZE *pBlockSize, SIZE *pBlockOffset, 
	UINT *pDisposal)
{
	if (!m_pRawData) return NULL;

	// GIF header + LSDescriptor [+ GCT] [+ Control block] + Data

	*pDisposal = 0;
	enum GIFBlockTypes nBlock;
	nBlock = GetNextBlock();

	while (
		(nBlock != BLOCK_CONTROLEXT) &&
		(nBlock != BLOCK_IMAGE) &&
		(nBlock != BLOCK_PLAINTEXT) &&
		(nBlock != BLOCK_UNKNOWN) &&
		(nBlock != BLOCK_TRAILER)
		)
	{
		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
	};

	if ((nBlock == BLOCK_UNKNOWN) ||
		(nBlock == BLOCK_TRAILER))
		return NULL;

	// it's either a control ext.block, an image or a plain text

	int nStart = m_nCurrOffset;
	int nBlockLen = GetNextBlockLen();

	if (nBlockLen <= 0) return NULL;

	if (nBlock == BLOCK_CONTROLEXT)
	{
		// get the following data
		TGIFControlExt *pControl = 
			reinterpret_cast<TGIFControlExt *> (&m_pRawData[m_nCurrOffset]);
		// store delay time
		*pDelay = pControl->m_wDelayTime;
		// store disposal method
		*pDisposal = pControl->GetPackedValue(GCX_PACKED_DISPOSAL);

		if (!SkipNextBlock()) return NULL;
		nBlock = GetNextBlock();
		
		// skip everything until we find data to display 
		// (image block or plain-text block)
		
		while (
			(nBlock != BLOCK_IMAGE) &&
			(nBlock != BLOCK_PLAINTEXT) &&
			(nBlock != BLOCK_UNKNOWN) &&
			(nBlock != BLOCK_TRAILER)
			)
		{
			if (!SkipNextBlock()) return NULL;
			nBlock = GetNextBlock();
			nBlockLen += GetNextBlockLen();
		};

		if ((nBlock == BLOCK_UNKNOWN) || (nBlock == BLOCK_TRAILER))
			return NULL;
		nBlockLen += GetNextBlockLen();
	}
	else
		*pDelay = -1; // to indicate that there was no delay value

	if (nBlock == BLOCK_IMAGE)
	{
		// store size and offsets
		TGIFImageDescriptor *pImage = 
			reinterpret_cast<TGIFImageDescriptor *> (&m_pRawData[m_nCurrOffset]);
		pBlockSize->cx = pImage->m_wWidth;
		pBlockSize->cy = pImage->m_wHeight;
		pBlockOffset->cx = pImage->m_wLeftPos;
		pBlockOffset->cy = pImage->m_wTopPos;
	};

	if (!SkipNextBlock()) return NULL;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED,
		sizeof(TGIFHeader) +
		sizeof(TGIFLSDescriptor) +
		m_nGlobalCTSize +
		nBlockLen + 
		1);  // for the trailer

	if (!hGlobal) return NULL;

	int nOffset = 0; 

	// GMEM_FIXED means we get a pointer
	unsigned char *pGlobal = reinterpret_cast<unsigned char *> (hGlobal);

	CopyMemory(pGlobal,m_pRawData, 
		sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize);
	nOffset += sizeof(TGIFHeader)+sizeof(TGIFLSDescriptor)+m_nGlobalCTSize;

	CopyMemory(pGlobal + nOffset,&m_pRawData[nStart], nBlockLen);
	nOffset += nBlockLen;

	pGlobal[nOffset] = 0x3B; // trailer
	nOffset++;

	*pBlockLen = nOffset;

	return hGlobal;
}

BOOL CPictureEx::IsGIF() const
{
	return m_bIsGIF;
}

BOOL CPictureEx::IsAnimatedGIF() const
{
	return (m_bIsGIF && (m_arrFrames.size() > 1));
}

BOOL CPictureEx::IsPlaying() const
{
#ifdef USE_TIMER
	return (m_uTimerId != 0);
#else
	return (NULL != m_hThread && INVALID_HANDLE_VALUE != m_hThread);
#endif
//	return m_bIsPlaying;
}

int CPictureEx::GetFrameCount() const
{
	if (!IsAnimatedGIF())
		return 0;

	return m_arrFrames.size();
}

COLORREF CPictureEx::GetBkColor() const
{
	return m_clrBackground;
}

void CPictureEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
    
	
	LONG nPaintWidth = m_PaintRect.right-m_PaintRect.left;

	CRect rcClient;
	GetClientRect(&rcClient);
	POINT ptOffset = {0, 0};
	if( m_bAlignCenter )
	{
		if( nPaintWidth > 0 ) 
		{
			ptOffset.x = rcClient.CenterPoint().x - ((m_PaintRect.right - m_PaintRect.left) / 2);
			ptOffset.y = rcClient.CenterPoint().y - ((m_PaintRect.bottom - m_PaintRect.top) / 2);
		}
		else
		{
			ptOffset.x = rcClient.CenterPoint().x - (m_PictureSize.cx / 2);
			ptOffset.y = rcClient.CenterPoint().y - (m_PictureSize.cy / 2);
		}
	}

	NMPictureEx nmData = {0};

	// 灰度处理
	if(m_bGrayBmp)
	{
		HBITMAP hbmp = (HBITMAP)::GetCurrentObject(m_hMemDC, OBJ_BITMAP);
		if(hbmp)
		{
			HBITMAP hTempBmp= (HBITMAP)::CopyImage(hbmp, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
			if(hTempBmp)
			{
				POINT pt = {0, 0};
				CGdiSupport::DrawGrayBmp(m_hMemDC, pt, hTempBmp);
				DeleteGdiObj(hTempBmp);
			}
		}
	}
	if (nPaintWidth > 0)
	{
		LONG nPaintHeight = m_PaintRect.bottom - m_PaintRect.top;
		::BitBlt(dc.m_hDC, ptOffset.x, ptOffset.y, nPaintWidth, nPaintHeight,	
			m_hMemDC, m_PaintRect.left, m_PaintRect.top, SRCCOPY);
		::SetRect(&nmData.rcPaint, ptOffset.x, ptOffset.y, 
			ptOffset.x+nPaintWidth, ptOffset.y+nPaintHeight);
	}
	else if( m_bAutoStretch && (m_PictureSize.cx>rcClient.Width() || m_PictureSize.cy>rcClient.Height()))
	{
		SetStretchBltMode(dc.m_hDC, STRETCH_HALFTONE);
		::StretchBlt(dc.m_hDC, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
			m_hMemDC, 0, 0, m_PictureSize.cx, m_PictureSize.cy, SRCCOPY);
	}
	else
	{		
		::BitBlt(dc.m_hDC, ptOffset.x, ptOffset.y, m_PictureSize.cx, m_PictureSize.cy,
			m_hMemDC, 0, 0, SRCCOPY);
		::SetRect(&nmData.rcPaint, ptOffset.x, ptOffset.y, 
			ptOffset.x+m_PictureSize.cx, ptOffset.y+m_PictureSize.cy);
	};
	
	HWND hwndParent = ::GetParent(m_hWnd);
	if( hwndParent )
	{
		nmData.hdr.code = NM_PICTUREEX_AFTERPAINT;
		nmData.hdr.hwndFrom = m_hWnd;
		nmData.hdr.idFrom = GetDlgCtrlID();

		nmData.hdc = dc.m_hDC;
		::SendMessage(hwndParent, WM_NOTIFY, nmData.hdr.idFrom, (LPARAM)&nmData);
	}
}

BOOL CPictureEx::PrepareDC(int nWidth, int nHeight)
{
	if( m_bAutoSize )
	{
		SetWindowPos(NULL,0,0,nWidth + m_nBorderWidth * 2,nHeight + m_nBorderWidth * 2,SWP_NOMOVE | SWP_NOZORDER);
	}

	HDC hWinDC = ::GetDC(m_hWnd);
	if (!hWinDC) return FALSE;
	
	m_hMemDC = CreateCompatibleDC(hWinDC);
	if (!m_hMemDC) 
	{
		::ReleaseDC(m_hWnd,hWinDC);
		return FALSE;
	};

	m_hBitmap  = CreateCompatibleBitmap(hWinDC,nWidth,nHeight);
	if (!m_hBitmap) 
	{
		::ReleaseDC(m_hWnd,hWinDC);
		::DeleteDC(m_hMemDC);
		return FALSE;
	};

	m_hOldBitmap = reinterpret_cast<HBITMAP> 
						(SelectObject(m_hMemDC,m_hBitmap));
	
	if( NULL == (HBRUSH)m_brBackground )
	{
		CGdiSupport::DrawTransparentBkgnd(m_hWnd, m_hMemDC);
	}
	else
	{
		// fill the background
		RECT rect = {0,0,nWidth,nHeight};
		FillRect(m_hMemDC,&rect,(HBRUSH)m_brBackground);
	}

	::ReleaseDC(m_hWnd,hWinDC);
	m_bIsInitialized = TRUE;
	return TRUE;
}

void CPictureEx::OnDestroy() 
{
	Stop();	
	CWnd::OnDestroy();
}

void CPictureEx::SetBkColor(COLORREF clr)
{
//	if (!m_bIsInitialized) return;

	m_clrBackground = clr;

	m_brBackground.DeleteObject();

	if( clr == -1 )
	{

	}
	else
	{
		m_brBackground.CreateSolidBrush(clr);
	}
}

#ifdef GIF_TRACING
void CPictureEx::WriteDataOnDisk(CString szFileName, HGLOBAL hData, DWORD dwSize)
{
	CFile file;

	if (!file.Open(szFileName,
			CFile::modeCreate |
			CFile::modeWrite |
			CFile::shareDenyNone))
	{
		TRACE(_T("WriteData: Error creating file %s\n"),szFileName);
		return;
	};

	char *pData = reinterpret_cast<char *> (GlobalLock(hData));
	if (!pData)
	{
		TRACE(_T("WriteData: Error locking memory\n"));
		return;
	};

	TRY
	{
		file.Write(pData,dwSize);
	}
	CATCH(CFileException, e);                                          
	{
		TRACE(_T("WriteData: An exception occured while writing to the file %s\n"),
			szFileName);
		e->Delete();
		GlobalUnlock(hData);
		file.Close();
		return;
	}
	END_CATCH
	
	GlobalUnlock(hData);
	file.Close();
}

void CPictureEx::EnumGIFBlocks()
{
	enum GIFBlockTypes nBlock;

	ResetDataPointer();
	while(m_nCurrOffset < m_nDataSize)
	{
		nBlock = GetNextBlock();
		switch(nBlock)
		{
		case BLOCK_UNKNOWN:
			TRACE(_T("- Unknown block\n"));
			return;
			break;

		case BLOCK_TRAILER:
			TRACE(_T("- Trailer block\n"));
			break;

		case BLOCK_APPEXT:
			TRACE(_T("- Application extension block\n"));
			break;

		case BLOCK_COMMEXT:
			TRACE(_T("- Comment extension block\n"));
			break;

		case BLOCK_CONTROLEXT:
			{
			TGIFControlExt *pControl = 
				reinterpret_cast<TGIFControlExt *> (&m_pRawData[m_nCurrOffset]);
			TRACE(_T("- Graphic control extension block (delay %d, disposal %d)\n"),
					pControl->m_wDelayTime, pControl->GetPackedValue(GCX_PACKED_DISPOSAL));
			};
			break;

		case BLOCK_PLAINTEXT:
			TRACE(_T("- Plain text extension block\n"));
			break;

		case BLOCK_IMAGE:
			TGIFImageDescriptor *pIDescr = 
				reinterpret_cast<TGIFImageDescriptor *> (&m_pRawData[m_nCurrOffset]);
			TRACE(_T("- Image data block (%dx%d  %d,%d)\n"),
					pIDescr->m_wWidth,
					pIDescr->m_wHeight,
					pIDescr->m_wLeftPos,
					pIDescr->m_wTopPos);
			break;
		};

		SkipNextBlock();	
	};

	TRACE(_T("\n"));
}
#endif // GIF_TRACING

BOOL CPictureEx::SetPaintRect(const RECT *lpRect)
{
	return CopyRect(&m_PaintRect, lpRect);
}

BOOL CPictureEx::GetPaintRect(RECT *lpRect)
{
	return CopyRect(lpRect, &m_PaintRect);
}

void CPictureEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnLButtonDown(nFlags, point);

	if(!m_strAdvURL.IsEmpty())
    {
		ShellExecute( 0, 0, m_strAdvURL, 0, 0, SW_SHOW );
	}

	CWnd *pWndParent = GetParent();
	if(pWndParent)
	{
		pWndParent->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)GetSafeHwnd());
	}	
}

//设置广告条的URL
void CPictureEx::SetAdvURL(LPCTSTR pszAdvURL)
{
    m_strAdvURL = pszAdvURL;  
}

BOOL CPictureEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bHandCursor || !m_strAdvURL.IsEmpty() )
	{
		SetCursor( AfxGetApp()->LoadStandardCursor(IDC_HAND));
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CPictureEx::SetAutoSize(BOOL bAutoSize)
{
	if( bAutoSize == m_bAutoSize)
	{
		return m_bAutoSize;
	}

	BOOL bOldValue = m_bAutoSize;
	m_bAutoSize = bAutoSize;
	if(bAutoSize && m_bIsInitialized && IsWindow(m_hWnd))
	{
		SetWindowPos(NULL, 0, 0, m_PictureSize.cx + m_nBorderWidth * 2,m_PictureSize.cy + m_nBorderWidth * 2, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);		
	}

	return bOldValue;
}

BOOL CPictureEx::SetAutoStretch(BOOL bAutoStretch)
{
	if( bAutoStretch == m_bAutoStretch )
	{
		return bAutoStretch;
	}

	BOOL bOldValue = m_bAutoStretch;
	m_bAutoStretch = bAutoStretch;
	return bOldValue;
}

BOOL CPictureEx::SetAlighCenter(BOOL bAlignCenter)
{
	if( bAlignCenter == m_bAlignCenter)
	{
		return m_bAlignCenter;
	}

	BOOL bOldValue = m_bAlignCenter;
	m_bAlignCenter = bAlignCenter;
	if(bAlignCenter && m_bIsInitialized && IsWindow(m_hWnd))
	{
		Invalidate();
	}

	return bOldValue;
}

BOOL CPictureEx::OnEraseBkgnd(CDC *pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	if( (HBRUSH)m_brBackground )
	{
		pDC->FillRect(&rcClient, &m_brBackground);
	}
	else
	{
		CGdiSupport::DrawTransparentBkgnd(m_hWnd, m_hMemDC, NULL, &rcClient);
		CGdiSupport::DrawTransparentBkgnd(m_hWnd, pDC->GetSafeHdc(), NULL, &rcClient);
	}

	return TRUE;
}

BOOL CPictureEx::SetHandCursor(BOOL bHandCursor)
{
	BOOL bOld = m_bHandCursor;
	if( bOld != bHandCursor )
	{
		m_bHandCursor = bHandCursor;
	}
	return bOld;
}

void CPictureEx::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetStyle() & WS_BORDER)
	{
		::InflateRect(&lpncsp->rgrc[0], -m_nBorderWidth, -m_nBorderWidth);
	}
	else
	{
		CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
	}	
}

void CPictureEx::OnNcPaint() 
{
	// TODO: Add your message handler code here
	NMPictureEx nmData = {0};
	
	HWND hwndParent = ::GetParent(m_hWnd);
	if( hwndParent && m_nBorderWidth>0 )
	{
		CWindowDC dc(this);

		nmData.hdr.code = NM_PICTUREEX_NCPAINT;
		nmData.hdr.hwndFrom = m_hWnd;
		nmData.hdr.idFrom = GetDlgCtrlID();
		GetWindowRect(&nmData.rcPaint);
		OffsetRect(&nmData.rcPaint, -nmData.rcPaint.left, -nmData.rcPaint.top);

		nmData.hdc = dc.m_hDC;
		LRESULT lResult = ::SendMessage(hwndParent, WM_NOTIFY, nmData.hdr.idFrom, (LPARAM)&nmData);
		if( 0 == lResult )
		{
			int n;
			CBrush br;
			br.CreateSolidBrush(m_crBorder);

			for(n=0; n<m_nBorderWidth; n++)
			{
				dc.FrameRect(&nmData.rcPaint, &br);
				InflateRect(&nmData.rcPaint, -1, -1);

				if( nmData.rcPaint.right <= nmData.rcPaint.left ||
					nmData.rcPaint.bottom <= nmData.rcPaint.top)
				{
					break;
				}
			}
		}
	}	
	// Do not call CWnd::OnNcPaint() for painting messages
}

int CPictureEx::GetBorderWidth() const
{
	return m_nBorderWidth;
}
	
int  CPictureEx::SetBorderWidth(int nWidth)
{
	int nOldBorderWidth = m_nBorderWidth;

	if(
		nWidth != m_nBorderWidth 
		&& nWidth >= 0
		&& nWidth <= 3
	  )
	{
		m_nBorderWidth = nWidth;
		if( IsWindow(m_hWnd))
		{
			if( m_nBorderWidth > 0 )
			{
				ModifyStyle(0, WS_BORDER, SWP_FRAMECHANGED);
			}
			else
			{
				ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
			}
		}
	}
	return nOldBorderWidth;
}

COLORREF CPictureEx::SetBorderColor(COLORREF crBorder)
{
	COLORREF crOld = m_crBorder;
	if( m_crBorder != crBorder )
	{
		m_crBorder = crBorder;
		if( IsWindow(m_hWnd))
		{
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_FRAMECHANGED);
		}
	}

	return crOld;
}

void CPictureEx::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd::PreSubclassWindow();

	BOOL bWithBorder = (GetStyle() & WS_BORDER);
	if( bWithBorder )
	{
		if( m_nBorderWidth == 0 )
		{
			ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
		}
	}
	else
	{
		if( m_nBorderWidth > 0 )
		{
			ModifyStyle(0, WS_BORDER, SWP_FRAMECHANGED);
		}
	}
}


void CPictureEx::OnTimer(UINT nIDEvent)
{
#ifdef USE_TIMER
	if( nIDEvent != 0 && m_uTimerId == nIDEvent )
	{
		StopTimer(m_hWnd, m_uTimerId);
		ASSERT( m_nCurrFrame < m_arrFrames.size() );
		TFrame curFrame = m_arrFrames[m_nCurrFrame];
		if (curFrame.m_pPicture)
		{
			// disposal method #2
			// 指定的矩形，restore to background color (m_clrBackground)
			if (curFrame.m_nDisposal == 2)
			{
				RECT rcNew = {
						curFrame.m_frameOffset.cx,
						curFrame.m_frameOffset.cy,
						curFrame.m_frameOffset.cx + curFrame.m_frameSize.cx,
						curFrame.m_frameOffset.cy + curFrame.m_frameSize.cy 
						};

				RECT rect;

				UnionRect(&rect, &rcNew, &m_rcOld);
				m_rcOld = rect;

				if( (HBRUSH)m_brBackground == NULL )
				{
					CGdiSupport::DrawTransparentBkgnd(m_hWnd, m_hMemDC, NULL, &rect);
					if( !m_bDynamicParentBkgnd )
					{ // 父窗口背景非动态，则可以保存下来
						HBITMAP hBmp = (HBITMAP)SelectObject(m_hMemDC, m_hOldBitmap);
						m_brBackground.CreatePatternBrush(CBitmap::FromHandle(hBmp));
						SelectObject(m_hMemDC, hBmp);
					}
				}
				else
				{
					FillRect(m_hMemDC, &rcNew, (HBRUSH)m_brBackground);
				}
			} 
			else
			{	if (m_hDispMemDC && (curFrame.m_nDisposal == 3) )
				{
					// put it back
					BitBlt(m_hMemDC,
						curFrame.m_frameOffset.cx,
						curFrame.m_frameOffset.cy,
						curFrame.m_frameSize.cx,
						curFrame.m_frameSize.cy,
						m_hDispMemDC,0,0, SRCCOPY);
					// init variables
					SelectObject(m_hDispMemDC,m_hDispOldBM);
					DeleteDC(m_hDispMemDC); m_hDispMemDC = NULL;
					DeleteObject(m_hDispMemBM); m_hDispMemBM = NULL;
				}
			}
			m_nCurrFrame++;
			if (m_nCurrFrame >= m_arrFrames.size())
			{
				m_nCurrFrame = 0; 
				// init the screen for the first frame,
				RECT rect = {0,0,m_PictureSize.cx,m_PictureSize.cy};
								
				if( (HBRUSH)m_brBackground == NULL )
				{
					CGdiSupport::DrawTransparentBkgnd(m_hWnd, m_hMemDC, NULL, &rect);
				}
				else
				{
					FillRect(m_hMemDC, &rect, (HBRUSH)m_brBackground);
				}
			}

			curFrame = m_arrFrames[m_nCurrFrame];
		
			///////////////////////////////////////////////////////
			// Before rendering a frame we should take care of what's 
			// behind that frame. TFrame::m_nDisposal will be our guide:
			//   0 - no disposal specified (do nothing)
			//   1 - do not dispose (again, do nothing)
			//   2 - restore to background color (m_clrBackground)
			//   3 - restore to previous

			//////// disposal method #3
			if (curFrame.m_nDisposal == 3)
			{

				// prepare a memory DC and store the background in it
				m_hDispMemDC = CreateCompatibleDC(m_hMemDC);
				m_hDispMemBM = CreateCompatibleBitmap(m_hMemDC,
							curFrame.m_frameSize.cx,
							curFrame.m_frameSize.cy);
				
				if (m_hDispMemDC && m_hDispMemBM)
				{
					m_hDispOldBM = reinterpret_cast<HBITMAP> (SelectObject(m_hDispMemDC,m_hDispMemBM));
					BitBlt(m_hDispMemDC,0,0,
						curFrame.m_frameSize.cx,
						curFrame.m_frameSize.cy,
						m_hMemDC,
						curFrame.m_frameOffset.cx,
						curFrame.m_frameOffset.cy,
						SRCCOPY);
				}
			}
			
			long hmWidth;
			long hmHeight;
			curFrame.m_pPicture->get_Width(&hmWidth);
			curFrame.m_pPicture->get_Height(&hmHeight);
			
			if (curFrame.m_pPicture->Render(m_hMemDC, 
				curFrame.m_frameOffset.cx, 
				curFrame.m_frameOffset.cy, 
				curFrame.m_frameSize.cx, 
				curFrame.m_frameSize.cy, 
				0, hmHeight, hmWidth, -hmHeight, NULL) == S_OK)
			{
				RECT rcNew = {
					curFrame.m_frameOffset.cx,
					curFrame.m_frameOffset.cy,
					curFrame.m_frameOffset.cx + curFrame.m_frameSize.cx,
					curFrame.m_frameOffset.cy + curFrame.m_frameSize.cy 
					};

				RECT rect;
				UnionRect(&rect, &rcNew, &m_rcOld);
				m_rcOld = rect;

				InvalidateRect(&rect, FALSE);
				UpdateWindow();
			}
			
			DWORD dwDelay;
			// if the delay time is too short (like in old GIFs), wait for 100ms
			if (curFrame.m_nDelay < 7) // 原来是5
				dwDelay = 100;
			else
				dwDelay = 10*curFrame.m_nDelay;
			
			m_uTimerId = SetTimer(1, dwDelay, NULL);
		}
		return;
	}
#endif
	CWnd::OnTimer(nIDEvent);
}

#ifdef USE_TIMER
void CPictureEx::PrepareTimer()
{
	// first, restore background (for stop/draw support)
	// disposal method #2
	TFrame curFrame = m_arrFrames[m_nCurrFrame];
	if (curFrame.m_nDisposal == 2)
	{
		RECT rect = {
				curFrame.m_frameOffset.cx,
				curFrame.m_frameOffset.cy,
				curFrame.m_frameOffset.cx + curFrame.m_frameSize.cx,
				curFrame.m_frameOffset.cy + curFrame.m_frameSize.cy 
				};
		if( (HBRUSH)m_brBackground == NULL )
		{
			CGdiSupport::DrawTransparentBkgnd(m_hWnd, m_hMemDC, NULL, &rect);
		}
		else
		{
			FillRect(m_hMemDC, &rect, (HBRUSH)m_brBackground);
		}
	} 
	else
	{
		// disposal method #3
		if (m_hDispMemDC && (curFrame.m_nDisposal == 3) )
		{
			// put it back
			BitBlt(m_hMemDC,
				curFrame.m_frameOffset.cx,
				curFrame.m_frameOffset.cy,
				curFrame.m_frameSize.cx,
				curFrame.m_frameSize.cy,
				m_hDispMemDC,0,0, SRCCOPY);
			// init variables
			SelectObject(m_hDispMemDC,m_hDispOldBM);
			DeleteDC(m_hDispMemDC); m_hDispMemDC = NULL;
			DeleteObject(m_hDispMemBM); m_hDispMemBM = NULL;
		}
	}

	long hmWidth;
	long hmHeight;
	curFrame.m_pPicture->get_Width(&hmWidth);
	curFrame.m_pPicture->get_Height(&hmHeight);

	if (curFrame.m_pPicture->Render(m_hMemDC, 
		curFrame.m_frameOffset.cx, 
		curFrame.m_frameOffset.cy, 
		curFrame.m_frameSize.cx, 
		curFrame.m_frameSize.cy, 
		0, hmHeight, hmWidth, -hmHeight, NULL) == S_OK)
	{
		Invalidate(FALSE);
	}
	

	DWORD dwDelay;
	// if the delay time is too short (like in old GIFs), wait for 100ms
	if (curFrame.m_nDelay < 7) // 原来是5
		dwDelay = 100;
	else
		dwDelay = 10*curFrame.m_nDelay;
	
	m_uTimerId = SetTimer(1, dwDelay, NULL);
}
#endif

void CPictureEx::SetGrayBmp(BOOL bGray)
{
	if(m_bGrayBmp != bGray)
	{
		m_bGrayBmp = bGray;
		Invalidate(TRUE);
	}
}

// 设置动画背景为静态还是动态，如果是静态则可以保存背景，否则需要每次重画
BOOL CPictureEx::SetParentBkgndDynamic(BOOL bDynamic)
{
	BOOL bOldValue = m_bDynamicParentBkgnd;
	if( bOldValue != bDynamic )
	{
		m_bDynamicParentBkgnd = bDynamic;
		if( (-1 == (int)m_clrBackground) // 使用透明色
			&& bDynamic )
		{
			m_brBackground.DeleteObject();
		}
	}
	return bOldValue;
}
