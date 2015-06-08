#include "../../../stdafx.h"
#include "convert.h"
#include "cameraview.h"
#include "camera.h"
#include "../../ETESystemPublicDefine.h"
#include "../../Calculator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CCameraView, CView)
	//{{AFX_MSG_MAP(CCameraView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
        ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONUP()
        ON_WM_MOUSEMOVE()
        ON_WM_RBUTTONUP()
		ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraView

IMPLEMENT_DYNCREATE(CCameraView, CView)

CCameraView::CCameraView()
{
  m_pBitmapInfo=NULL;
  m_pImage=NULL;

  m_HScrollPos		 = 0;
  m_VScrollPos		 = 0;
  m_HScrollMax		 = 0;
  m_VScrollMax		 = 0;
  m_BkgndVisible     = TRUE;
  m_ScrollMode       = FALSE;
  m_SelectionEnabled = FALSE;
  m_DrawSelection    = FALSE;
  m_bROISelect       = FALSE;
  m_bNROISelect      = FALSE;
  m_bReqRegSelect    = FALSE;
  m_bDrawRegSelect   = FALSE;
  m_bUseMirror		 = FALSE;
  m_bRecordFrame		 = FALSE;
  m_SliderXPos = 0;
  m_SliderYPos = 0;
  m_frameIndex = 0;
  m_frameIndexInit = 1;
  m_differ = 1;
  m_bInternalTriggerMode = TRUE;
  ZeroMemory(&m_area, sizeof(m_area));

  m_currentIdx = 0;

}

CCameraView::~CCameraView()
{
  // Remove all bitmap stuff
  DeleteBitmap();
}


/////////////////////////////////////////////////////////////////////////////
// Diagnose CCameraView

#ifdef _DEBUG
void CCameraView::AssertValid() const
{
	CView::AssertValid();
}

void CCameraView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Process a frame that comes from our camera.
// Remark: Don't use pFrame->Length, it can be larger then our image!

void CCameraView::ProcessFrame(UINT8 *pData,UINT32,UINT8 Redraw)
{
  UINT32 BytesPerDstPix;

  // Copy frame to our bitmap
  if(m_pBitmapInfo)
  {
    BytesPerDstPix=3;
    switch(m_ColorMode)
    {
      case CM_Y8    : memcpy(m_pImage,pData,(UINT32)m_XSize*(UINT32)m_YSize);
                      BytesPerDstPix=1;
                      break;

      case CM_RAW8  : ConvertRawY8(m_XSize,m_YSize,pData,m_pImage,m_BayerPattern);
                      break;

      case CM_RGB8  : ConvertRGB(m_XSize,m_YSize,pData,m_pImage);
                      break;

      case CM_RGB16 : ConvertRGB16(m_XSize,m_YSize,pData,m_pImage,m_Y16Invert);
                      break;

      case CM_YUV411: Convert411(m_XSize,m_YSize,pData,m_pImage);
                      break;

      case CM_YUV422: Convert422(m_XSize,m_YSize,pData,m_pImage);
                      break;

      case CM_YUV444: Convert444(m_XSize,m_YSize,pData,m_pImage);
                      break;

      case CM_RAW16 : if(m_Y16Invert)
                       ConvertRawY16a(m_XSize,m_YSize,pData,m_pImage,m_BayerPattern);
                      else
                       ConvertRawY16(m_XSize,m_YSize,pData,m_pImage,m_BayerPattern);
                      break;

      case CM_Y16   : if(m_Y16Invert)
                       ConvertY16a(m_XSize,m_YSize,pData,m_pImage);
                      else
                       ConvertY16(m_XSize,m_YSize,pData,m_pImage);
                      break;

      // No other color formats
      default       : return;
    }


	if (m_bRecordFrame)
	{
		//如果是内部触发 设置标志位 防止多次触发
		if (m_bInternalTriggerMode)
		{
			m_bRecordFrame = FALSE;
		}

		Calculator::GetInstance()->PushData(m_pImage);
		SaveGrayBitmap();
// 		if (Calculator::GetInstance()->ConfocalMode())
// 		{
// 			Calculator::GetInstance()->PushData(m_pImage);
// 		}
// 		else
// 		{
// 			SaveGrayBitmap();
// 		}
	}


    // Redraw our window
    if(m_hWnd)
    {
      // Redraw complete or just invalidate?
      if(Redraw)
       RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW);
      else
       InvalidateRect(NULL,FALSE);  
    }  
  }
}

/////////////////////////////////////////////////////////////////////////////
// Init gray bitmap.

void CCameraView::InitGrayBitmap()
{
  UINT32 i,j,Width,Height;
  UINT8  Value,*pMem;

  Width=abs(m_pBitmapInfo->bmiHeader.biWidth);
  Height=abs(m_pBitmapInfo->bmiHeader.biHeight);
  pMem=m_pImage;

  for(i=0;i<Height;i++)
  {
    Value=(UINT8)(i&0xFF);
    for(j=0;j<Width;j++)
     *pMem++=Value++;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Init color bitmap.

void CCameraView::InitColorBitmap()
{
  UINT32  i,j,Width,Height;
  UINT32  Pattern,Offset;
  UINT32  StartPattern;
  UINT8  *pMem;

  Width=abs(m_pBitmapInfo->bmiHeader.biWidth);
  Height=abs(m_pBitmapInfo->bmiHeader.biHeight);

  // Clear whole memory
  memset(m_pImage,0,Width*Height*3);
  pMem=m_pImage;
  StartPattern=0;

  for(i=0;i<Height;i++)
  {
    Pattern=StartPattern;
    Offset=(Pattern>>8)%3;

    for(j=0;j<Width;j++)
    {
      pMem[Offset]=(UINT8)(Pattern&0xFF);
      pMem+=3;

      Pattern+=2;
      Offset=(Pattern>>8)%3;
    }

    StartPattern+=2;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Create a bitmap info for blitting.

BOOL CCameraView::CreateBitmap(UINT32 Width,UINT32 Height,UINT8 ColorMode,
                               UINT8 BayerPattern,UINT8 RawMode,UINT8 Y16Invert)
{
  UINT32 BytesPerPixel,ColorEntries;
  UINT32 i;

  // Width always is a multiple of 4
  Width=(Width+3)&0xFFFFFFFC;
  
  // Always cleanup in advance
  DeleteBitmap();

  // Copy parameters
  m_XSize=m_OrgXSize=(UINT16)Width;
  m_YSize=m_OrgYSize=(UINT16)Height;
  m_ColorMode=ColorMode;
  m_BayerPattern=BayerPattern;
  m_Y16Invert=Y16Invert;
  
  // Replace raw modes
  if((m_ColorMode==CM_Y8) && RawMode)
   m_ColorMode=CM_RAW8;

  if((m_ColorMode==CM_Y16) && RawMode)
   m_ColorMode=CM_RAW16;

  // Get number of bits per pixel
  switch(m_ColorMode)
  {
    case CM_Y8    : BytesPerPixel=1;
                    ColorEntries=256;
					Calculator::GetInstance()->Init(m_XSize, m_YSize);
                    break;

    case CM_RAW8:
    case CM_RAW16:
    case CM_YUV411:
    case CM_YUV422:
    case CM_YUV444:
    case CM_RGB8  :
    case CM_RGB16 :
    case CM_Y16   : BytesPerPixel=3;
                    ColorEntries=0;
                    break;

    default       : return FALSE;
  }

  m_pImage=(UINT8*)new char[Width*Height*BytesPerPixel];
  m_pBitmapInfo=(BITMAPINFO*)(new char[sizeof(BITMAPINFOHEADER)+
                                       ColorEntries*sizeof(RGBQUAD)]);
  // Return if no memory
  if(!m_pBitmapInfo||!m_pImage)
  {
    DeleteBitmap();
    return FALSE;
  }

  // Set info header
  m_pBitmapInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  m_pBitmapInfo->bmiHeader.biWidth=Width;
  m_pBitmapInfo->bmiHeader.biHeight=-(LONG)Height;
  m_pBitmapInfo->bmiHeader.biPlanes=1;
  m_pBitmapInfo->bmiHeader.biBitCount=(UINT16)(BytesPerPixel*8);
  m_pBitmapInfo->bmiHeader.biCompression=BI_RGB;
  m_pBitmapInfo->bmiHeader.biSizeImage=Width*Height*BytesPerPixel;
  m_pBitmapInfo->bmiHeader.biXPelsPerMeter=0;
  m_pBitmapInfo->bmiHeader.biYPelsPerMeter=0;
  m_pBitmapInfo->bmiHeader.biClrUsed=ColorEntries;
  m_pBitmapInfo->bmiHeader.biClrImportant=0;

  // Set color values if greyscale
  if(BytesPerPixel==1)
  {
    // Set our gray scale values
    for(i=0;i<256;i++)
    {
      m_pBitmapInfo->bmiColors[i].rgbRed=(UINT8)i;
      m_pBitmapInfo->bmiColors[i].rgbGreen=(UINT8)i;
      m_pBitmapInfo->bmiColors[i].rgbBlue=(UINT8)i;
      m_pBitmapInfo->bmiColors[i].rgbReserved=0;
    }
    InitGrayBitmap();
  }
  else
  {
    InitColorBitmap();
  }

  if(m_hWnd!=NULL)
  {
    m_SrcRect.left=m_SrcRect.top=0;
    if(m_pBitmapInfo)
    {
      m_SrcRect.right=m_pBitmapInfo->bmiHeader.biWidth;
      m_SrcRect.bottom=abs(m_pBitmapInfo->bmiHeader.biHeight);
    }
    
    RecalcScrollbars();
    InvalidateRect(NULL,TRUE);
  }



  

  

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Delete bitmap info (used for blitting).

void CCameraView::DeleteBitmap()
{
  if(m_pBitmapInfo)
  {
    // Just free our bitmap info
    delete[] m_pBitmapInfo;
    m_pBitmapInfo=NULL;
  }

  if(m_pImage)
  {
    delete[] m_pImage;
    m_pImage=NULL;
  }

  if(m_hWnd!=NULL)
   InvalidateRect(NULL,TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Reaction on window create.

int CCameraView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CView::OnCreate(lpCreateStruct) == -1)
   return -1;

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CCameraView

BOOL CCameraView::OnEraseBkgnd(CDC* pDC)
{
  if(!m_pBitmapInfo)
  {
    return CView::OnEraseBkgnd(pDC);
  }
  else
  {
    m_EraseBkgnd=TRUE;
    return TRUE;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Set scroller.

void CCameraView::SetScroller(DWORD Which,int Min,int Max,UINT Page,int Pos,
                              BOOL Redraw)
{
  SCROLLINFO Info;

  Info.cbSize=sizeof(SCROLLINFO);
  Info.fMask=SIF_ALL;
  Info.nMin=Min;
  Info.nMax=Max;
  Info.nPage=Page;
  Info.nPos=Pos;

  SetScrollInfo(Which,&Info,Redraw);
}

////////////////////////////////////////////////////////////////////////////////
// Adjust our blit rectangles (for ScrollMode).
// We assume that the right and bottom coordinate is NOT included.
// XSize and YSize is the size of our client windows area.

void CCameraView::AdjustBlitRectangles(DWORD XSize,DWORD YSize)
{
  DWORD Width,Height;

  if(!m_pBitmapInfo||!m_pImage)
   return;

  Width=(DWORD)abs(m_pBitmapInfo->bmiHeader.biWidth);
  Height=(DWORD)abs(m_pBitmapInfo->bmiHeader.biHeight);

  m_BkgndVisible=FALSE;

  // Adjust X direction
  if(XSize>=Width)
  {
    // Center if all visible
    m_BkgndVisible=TRUE;

    m_SrcRect.left=0;
    m_SrcRect.right=Width;
    m_DstRect.left=(XSize-Width)/2;
    m_DstRect.right=m_DstRect.left+Width;
  }
  else
  {
    m_SrcRect.left=m_HScrollPos;
    m_SrcRect.right=m_SrcRect.left+XSize;
    m_DstRect.left=0;
    m_DstRect.right=XSize;
  }

  // Adjust Y direction
  if(YSize>=Height)
  {
    m_BkgndVisible=TRUE;

    m_SrcRect.top=0;
    m_SrcRect.bottom=Height;
    m_DstRect.top=(YSize-Height)/2;
    m_DstRect.bottom=m_DstRect.top+Height;
  }
  else
  {
    m_SrcRect.top=m_VScrollPos;
    m_SrcRect.bottom=m_SrcRect.top+YSize;
    m_DstRect.top=0;
    m_DstRect.bottom=YSize;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Reaction on horizontal scroller.

void CCameraView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar*)
{
  int  iScrollPos;
  RECT r;

  iScrollPos=m_HScrollPos;
  switch(nSBCode)
  {
    case SB_LINERIGHT:
     iScrollPos+=1;
     if(iScrollPos>(int)m_HScrollMax)
      iScrollPos=m_HScrollMax;
     break;

    case SB_LINELEFT:
     iScrollPos-=1;
     if(iScrollPos<0)
      iScrollPos=0;
     break;

    case SB_PAGERIGHT:
     iScrollPos+=20*1;
     if(iScrollPos>(int)m_HScrollMax)
      iScrollPos=m_HScrollMax;
     break;

    case SB_PAGELEFT:
     iScrollPos-=20*1;
     if(iScrollPos<0)
      iScrollPos=0;
     break;

    case SB_THUMBTRACK:
     iScrollPos=(short int)nPos;
     break;
  }

  if(iScrollPos!=(int)m_HScrollPos)
  {
    m_HScrollPos=iScrollPos;
    SetScrollPos(SB_HORZ,iScrollPos,TRUE);
    GetClientRect(&r);
    AdjustBlitRectangles(r.right-r.left,r.bottom-r.top);
    InvalidateRect(NULL,TRUE);
  }
}

/////////////////////////////////////////////////////////////////////////////
// Reaction on vertical scroller.

void CCameraView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar*)
{
  int  iScrollPos;
  RECT r;

  iScrollPos=m_VScrollPos;
  switch(nSBCode)
  {
    case SB_LINERIGHT:
     iScrollPos+=1;
     if(iScrollPos>(int)m_VScrollMax)
      iScrollPos=m_VScrollMax;
     break;

    case SB_LINELEFT:
     iScrollPos-=1;
     if(iScrollPos<0)
      iScrollPos=0;
     break;

    case SB_PAGERIGHT:
     iScrollPos+=20*1;
     if(iScrollPos>(int)m_VScrollMax)
      iScrollPos=m_VScrollMax;
     break;

    case SB_PAGELEFT:
     iScrollPos-=20*1;
     if(iScrollPos<0)
      iScrollPos=0;
     break;

    case SB_THUMBTRACK:
     iScrollPos=(short int)nPos;
     break;
  }

  if(iScrollPos!=(int)m_VScrollPos)
  {
    m_VScrollPos=iScrollPos;
    SetScrollPos(SB_VERT,iScrollPos,TRUE);
    GetClientRect(&r);
    AdjustBlitRectangles(r.right-r.left,r.bottom-r.top);
    InvalidateRect(NULL,TRUE);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Adjust our scrollbars.

void CCameraView::RecalcScrollbars()
{
  int   Dif;
  UINT  SPage;
  UINT  SMax;
  DWORD XSize,YSize,Width,Height;
  RECT  rClient;
  BOOL  HTooSmall;

  if(!m_pBitmapInfo || !m_pImage)
   return;

  if(m_ScrollMode)
  {
    Width=abs(m_pBitmapInfo->bmiHeader.biWidth);
    Height=abs(m_pBitmapInfo->bmiHeader.biHeight);

    // Get pure client area
    GetClientRect(&rClient);
    XSize=rClient.right-rClient.left;
    YSize=rClient.bottom-rClient.top;

    // Add scroller area if we have scrollers
    if(m_VScrollMax!=0)
     XSize+=GetSystemMetrics(SM_CXVSCROLL);
    if(m_HScrollMax!=0)
     YSize+=GetSystemMetrics(SM_CYHSCROLL);

    // Check whether we need scrollers and correct the size
    HTooSmall=FALSE;
    if(Width>XSize)
    {
      YSize-=GetSystemMetrics(SM_CYHSCROLL);
      HTooSmall=TRUE;
    }

    if(Height>YSize)
    {
      XSize-=GetSystemMetrics(SM_CXVSCROLL);
      // Check HSize again
      if(!HTooSmall)
       if(Width>XSize)
        YSize-=GetSystemMetrics(SM_CYHSCROLL);
    }

    // Set horizontal scrollbar
    if((Dif=Width-XSize)>0)
    {
      m_HScrollMax=Dif;

      // Probably reduce scroll position on enlarging
      if(m_HScrollPos+XSize>Width)
       m_HScrollPos=(Width-XSize);

      SPage=((Width-XSize+1)*XSize)/(Width-XSize);
      SMax=m_HScrollMax+SPage-1;
      SetScroller(SB_HORZ,0,SMax,SPage,m_HScrollPos);
    }
    else
    {
      SetScroller(SB_HORZ,0,0,0,0);
      m_HScrollMax=0;
      m_HScrollPos=0;
    }

    // Set vertical scrollbar
    if((Dif=Height-YSize)>0)
    {
      m_VScrollMax=Dif;

      // Probably reduce scroll position on enlarging
      if(m_VScrollPos+YSize>Height)
       m_VScrollPos=(Height-YSize);

      SPage=((Height-YSize+1)*YSize)/(Height-YSize);
      SMax=m_VScrollMax+SPage-1;
      SetScroller(SB_VERT,0,SMax,SPage,m_VScrollPos);
    }
    else
    {
      SetScroller(SB_VERT,0,0,0,0);
      m_VScrollPos=0;
      m_VScrollMax=0;
    }

    AdjustBlitRectangles(XSize,YSize);
  }
  else
  {
    SetScroller(SB_HORZ,0,0,0,0);
    SetScroller(SB_VERT,0,0,0,0);
    GetClientRect(&m_DstRect);
  }
  
}

////////////////////////////////////////////////////////////////////////////////
// Adjust our scrollbars.

void CCameraView::OnSize(UINT nType, int cx, int cy)
{
  RecalcScrollbars();
  CView::OnSize(nType, cx, cy);
}

////////////////////////////////////////////////////////////////////////////////
// Reaction on scrollmode entry.

void CCameraView::SetScrollmode(BOOL Enable)
{
  m_ScrollMode=Enable;

  m_HScrollPos=0;
  m_VScrollPos=0;

  if(!m_ScrollMode)
  {
    m_HScrollMax=0;
    m_VScrollMax=0;
    
    if(m_pBitmapInfo)
    {
      m_SrcRect.left=m_SrcRect.top=0;
      m_SrcRect.right=m_pBitmapInfo->bmiHeader.biWidth;
      m_SrcRect.bottom=abs(m_pBitmapInfo->bmiHeader.biHeight);
    }  
  }

  RecalcScrollbars();
  InvalidateRect(NULL,TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// Erase background.

void CCameraView::EraseBkgnd(CDC *pDC)
{
  CBrush *pOldBrush,*pBrush;
  CPen   *pOldPen,*pPen;
  RECT    Rect;

  if(m_ScrollMode && m_BkgndVisible)
  {
    pBrush=new CBrush;
    pPen=new CPen;

    if(pBrush && pPen)
    {
      pBrush->CreateSolidBrush(RGB(64,64,64));
      pPen->CreatePen(PS_SOLID,1,RGB(64,64,64));

      pOldBrush=pDC->SelectObject(pBrush);
      pOldPen=pDC->SelectObject(pPen);

      GetClientRect(&Rect);

      // Draw 4 rectangles to hide background
      pDC->Rectangle(Rect.left,Rect.top,Rect.right,m_DstRect.top);
      pDC->Rectangle(Rect.left,m_DstRect.top,m_DstRect.left,m_DstRect.bottom);
      pDC->Rectangle(m_DstRect.right,m_DstRect.top,Rect.right,m_DstRect.bottom);
      pDC->Rectangle(Rect.left,m_DstRect.bottom,Rect.right,Rect.bottom);

      pDC->SelectObject(pOldBrush);
      pDC->SelectObject(pOldPen);
    }

    if(pBrush)
     delete pBrush;

    if(pPen)
     delete pPen;
  }

  m_EraseBkgnd=FALSE;
}

void CCameraView::OnDraw(CDC* pDC)
{
  DWORD Height;
  TCHAR *pStr;

  // Check background erase
  if(m_EraseBkgnd)
   EraseBkgnd(pDC);

  // Check whether we have a bitmap or not
  if(m_pBitmapInfo)
  {
    // Set appropriate blitmode
    pDC->SetStretchBltMode(STRETCH_DELETESCANS);
    Height=abs(m_pBitmapInfo->bmiHeader.biHeight);

	int destWidth = 0;

	if(!m_ScrollMode)
	{
		if (m_bUseMirror)
		{
			// Blit bitmap (inverted HEIGHT for top down bitmap)
			StretchDIBits(pDC->m_hDC,
				m_DstRect.right -m_DstRect.left,
				m_DstRect.top,
				-(m_DstRect.right-m_DstRect.left),
				m_DstRect.bottom-m_DstRect.top,
				m_SrcRect.left,
				Height-m_SrcRect.bottom,
				m_SrcRect.right-m_SrcRect.left,
				m_SrcRect.bottom-m_SrcRect.top,
				m_pImage,m_pBitmapInfo,DIB_RGB_COLORS,SRCCOPY);
		}
		else
		{
			// Blit bitmap (inverted HEIGHT for top down bitmap)
			StretchDIBits(pDC->m_hDC,
				m_DstRect.left,
				m_DstRect.top,
				m_DstRect.right-m_DstRect.left,
				m_DstRect.bottom-m_DstRect.top,
				m_SrcRect.left,
				Height-m_SrcRect.bottom,
				m_SrcRect.right-m_SrcRect.left,
				m_SrcRect.bottom-m_SrcRect.top,
				m_pImage,m_pBitmapInfo,DIB_RGB_COLORS,SRCCOPY);
		}

	}
	else
	{
		// Windows definitely has a blitting bug:
		// It shows if we have a vertical scroller and we blit with vertical scroller
		// set to max and start with source rectangle zero
		if((m_VScrollMax!=0) && (m_VScrollPos==m_VScrollMax) && (m_HScrollPos==0))
		{
			if (m_bUseMirror)
			{
				m_pBitmapInfo->bmiHeader.biHeight*=-1;
				StretchDIBits(pDC->m_hDC,
					m_DstRect.left,
					m_DstRect.top,
					m_DstRect.right-m_DstRect.left,
					m_DstRect.bottom-m_DstRect.top,
					m_SrcRect.left,
					m_SrcRect.bottom+1,
					m_SrcRect.right-m_SrcRect.left,
					-(m_SrcRect.bottom-m_SrcRect.top),
					m_pImage,m_pBitmapInfo,DIB_RGB_COLORS,SRCCOPY);
				m_pBitmapInfo->bmiHeader.biHeight*=-1;
			}
			else
			{

			}


		}
		else
		{
			if (m_bUseMirror)
			{
				StretchDIBits(pDC->m_hDC,
					m_DstRect.left,
					m_DstRect.top,
					m_DstRect.right-m_DstRect.left,
					m_DstRect.bottom-m_DstRect.top,
					m_SrcRect.left,
					Height-m_SrcRect.bottom,
					m_SrcRect.right-m_SrcRect.left,
					m_SrcRect.bottom-m_SrcRect.top,
					m_pImage,m_pBitmapInfo,DIB_RGB_COLORS,SRCCOPY);
			}
			else
			{
				StretchDIBits(pDC->m_hDC,
					m_DstRect.left,
					m_DstRect.top,
					m_DstRect.right-m_DstRect.left,
					m_DstRect.bottom-m_DstRect.top,
					m_SrcRect.left,
					Height-m_SrcRect.bottom,
					m_SrcRect.right-m_SrcRect.left,
					m_SrcRect.bottom-m_SrcRect.top,
					m_pImage,m_pBitmapInfo,DIB_RGB_COLORS,SRCCOPY);
			}
		}
	}

    
    if(m_DrawSelection)
    {
      pDC->SelectStockObject(HOLLOW_BRUSH);
      pDC->SelectStockObject(WHITE_PEN);
      pDC->SetROP2(R2_XORPEN);
      
      pDC->Rectangle(&m_DragRect);
    }
  }
  else
  {
    pStr = _T("Camera has no valid settings!");
    pDC->TextOut(0,0,pStr,(int)_tcslen(pStr));
  }

  //画十字线
  CPen newPen, *pOldPen;
#if 1
  if(0)
	  newPen.CreatePen(PS_DOT,2,RGB(255,255,255));    //网格的颜色为灰色
  else
	  newPen.CreatePen(PS_DOT,2,RGB(169,169,169));    //网格的颜色为深灰色
#else
  if(flag == 0)
	  newPen.CreatePen(PS_DOT,1,RGB(100, 149, 237));    //网格的颜色为灰色
  else
	  newPen.CreatePen(PS_DOT,1,RGB(100, 149, 237));    //网格的颜色为深灰色
#endif
  pOldPen = pDC->SelectObject(&newPen);        //选择画笔

  m_SliderXPos;
  RECT    Rect;
  GetClientRect(&Rect);

  double xP = m_SliderXPos*(Rect.right - Rect.left)/100;
  pDC->MoveTo((int)xP, 0);
  pDC->LineTo((int)xP, Rect.bottom);

  double yP = m_SliderYPos*(Rect.bottom - Rect.top)/100;
  pDC->MoveTo(0, (int)yP);
  pDC->LineTo(Rect.right, (int)yP);

  //销毁画笔
  newPen.DeleteObject();

  if(m_bDrawRegSelect)
  {
	  pDC->SelectStockObject(HOLLOW_BRUSH);
	  if(m_bROISelect)
		pDC->SelectStockObject(WHITE_PEN);
	  else
		pDC->SelectStockObject(GRAY_BRUSH);
	  
	  pDC->SetROP2(R2_XORPEN);

	  pDC->Rectangle(&m_RegRect);
  }

#if 1
  //::SetTextColor(GetDlgItem(IDC_BTN_ROI)->GetDC(),RGB(255, 0, 0));
  if(m_area.iAreaNum == 0)
	  return;

  for(int i = 0; i < m_area.iAreaNum; ++i)
  {
	  if(m_area.pos[i].type)
	  {
		  //重点区域
		  CPen pen(PS_SOLID,3,RGB(255,0,0));
		  pDC->SelectObject(&pen);
		  CPoint point1,point2,point3,point4;
		  point1 = m_area.pos[i].PtLeftUp;
		  point2.x = m_area.pos[i].PtRightDn.x;
		  point2.y = m_area.pos[i].PtLeftUp.y;
		  point3.x = m_area.pos[i].PtLeftUp.x;
		  point3.y = m_area.pos[i].PtRightDn.y;
		  point4 = m_area.pos[i].PtRightDn;

		  pDC->MoveTo(point1.x,point1.y);
		  pDC->LineTo(point2.x,point2.y);

		  //dc.MoveTo(point1.x,point1.y);
		  pDC->LineTo(point2.x,point2.y);

		  //dc.MoveTo(point1.x,point1.y);
		  pDC->LineTo(point4.x,point4.y);

		  //dc.MoveTo(point1.x,point1.y);
		  pDC->LineTo(point3.x,point3.y);

		  pDC->LineTo(point1.x,point1.y);
	  }
	  else
	  {
		  //屏蔽区域
		  CPoint point21,point22,point23,point24;
		  
		  point21 = m_area.pos[i].PtLeftUp;
		  point22.x = m_area.pos[i].PtRightDn.x;
		  point22.y = m_area.pos[i].PtLeftUp.y;
		  point23.x = m_area.pos[i].PtLeftUp.x;
		  point23.y = m_area.pos[i].PtRightDn.y;
		  point24 = m_area.pos[i].PtRightDn;

		  CPen pen2(PS_SOLID,3,RGB(0,255,0));
		  pDC->SelectObject(&pen2);

		  pDC->MoveTo(point21.x,point21.y);
		  pDC->LineTo(point22.x,point22.y);

		  pDC->LineTo(point22.x,point22.y);

		  pDC->LineTo(point24.x,point24.y);

		  pDC->LineTo(point23.x,point23.y);

		  pDC->LineTo(point21.x,point21.y);
	  }
  }
#endif
}

void CCameraView::OnLButtonDown(UINT nFlags, CPoint point)
{
  if(!m_ScrollMode && m_SelectionEnabled)
  {
    SetCapture();
    m_DrawSelection=TRUE;
    m_StartPoint=point;
    m_DragRect.left=m_DragRect.right=point.x;
    m_DragRect.top=m_DragRect.bottom=point.y;
  }

  if( m_bReqRegSelect && (m_bROISelect || m_bNROISelect))
  {
	  m_bReqRegSelect = FALSE;
	  m_bDrawRegSelect = TRUE;
	  m_RegStartPoint = point;
	  m_RegRect.left = m_RegRect.right = point.x;
	  m_RegRect.top = m_RegRect.bottom = point.y;
  }
  
  CView::OnLButtonDown(nFlags, point);
}

void CCameraView::OnMouseMove(UINT nFlags, CPoint point)
{
  RECT Rect,Client;
  
  if(m_DrawSelection)
  {
    m_DragRect.left=m_StartPoint.x;
    m_DragRect.top=m_StartPoint.y;
    m_DragRect.right=point.x;
    m_DragRect.bottom=point.y;
    
    Rect=m_DragRect;
    
    if(Rect.top>Rect.bottom)
    {
      m_DragRect.bottom=Rect.top;
      m_DragRect.top=Rect.bottom;
    }
    
    if(Rect.left>Rect.right)
    {
      m_DragRect.right=Rect.left;
      m_DragRect.left=Rect.right;
    }
    
    GetClientRect(&Client);
    
    if(m_DragRect.left<0)
     m_DragRect.left=0;
     
    if(m_DragRect.top<0)
     m_DragRect.top=0;
    
    if(m_DragRect.right>Client.right)
     m_DragRect.right=Client.right; 
     
    if(m_DragRect.bottom>Client.bottom)
     m_DragRect.bottom=Client.bottom; 
     
    InvalidateRect(NULL,TRUE);
  }
  
  if(m_bDrawRegSelect)
  {
	  m_RegRect.left=m_RegStartPoint.x;
	  m_RegRect.top=m_RegStartPoint.y;
	  m_RegRect.right=point.x;
	  m_RegRect.bottom=point.y;

	  Rect=m_RegRect;

	  if(Rect.top>Rect.bottom)
	  {
		  m_DragRect.bottom=Rect.top;
		  m_DragRect.top=Rect.bottom;
	  }

	  if(Rect.left>Rect.right)
	  {
		  m_DragRect.right=Rect.left;
		  m_DragRect.left=Rect.right;
	  }

	  GetClientRect(&Client);

	  if(m_RegRect.left<0)
		  m_RegRect.left=0;

	  if(m_RegRect.top<0)
		  m_RegRect.top=0;

	  if(m_RegRect.right>Client.right)
		  m_RegRect.right=Client.right; 

	  if(m_RegRect.bottom>Client.bottom)
		  m_RegRect.bottom=Client.bottom; 

	  InvalidateRect(NULL,TRUE);
  }

  CView::OnMouseMove(nFlags, point);
}

void CCameraView::OnLButtonUp(UINT nFlags, CPoint point)
{
  RECT   Client;
  double XScale,YScale;
  
  // Only if select mode was active
  if(m_DrawSelection)
  {
    ReleaseCapture();
    m_DrawSelection=FALSE;
    
    if(m_DragRect.right>=m_DragRect.left+4 && m_DragRect.bottom>=m_DragRect.top)
    {
      GetClientRect(&Client);
     
      XScale=(double)(m_SrcRect.right-m_SrcRect.left)/(double)Client.right;
      YScale=(double)(m_SrcRect.bottom-m_SrcRect.top)/(double)Client.bottom;
    
      // Get real rectangle within actual source rectangle
      m_DragRect.top=(LONG)(m_DragRect.top*YScale+0.5);
      m_DragRect.bottom=(LONG)(m_DragRect.bottom*YScale+0.5);
      if(m_DragRect.top==m_DragRect.bottom)
       m_DragRect.bottom=m_DragRect.top+1;
     
      m_DragRect.left=(LONG)(m_DragRect.left*XScale+0.5);
      m_DragRect.right=(LONG)(m_DragRect.right*XScale+0.5);
      if(m_DragRect.left==m_DragRect.right)
       m_DragRect.right=m_DragRect.left+1;
    
      ::PostMessage(m_hMsgWnd,m_Msg,
                    MAKELONG((USHORT)m_DragRect.left,(USHORT)m_DragRect.right),
                    MAKELONG((USHORT)m_DragRect.top,(USHORT)m_DragRect.bottom));  

      // Redraw to hide rectangle
      InvalidateRect(NULL,TRUE);
    }  
  }
  
  if(m_bDrawRegSelect)
  {
	  m_bDrawRegSelect = FALSE;

	  if(m_area.iAreaNum < 9)
	  {
		  m_area.pos[m_area.iAreaNum].PtLeftUp = m_RegStartPoint;
		  m_area.pos[m_area.iAreaNum].PtRightDn = point;

		  if(m_bROISelect)
		  {
			  m_area.pos[m_area.iAreaNum].type = 1;
		  }
		  else
		  {
			  m_area.pos[m_area.iAreaNum].type = 0;
		  }

		  
		  Calculator::GetInstance()->InitDisplayRegion(m_area.pos[0].PtLeftUp.x, m_area.pos[0].PtRightDn.x);




		  TRACE(_T("New Area Add++ Idx: %d type: %d Pos %d %d %d %d\n"), m_area.iAreaNum, m_bROISelect, m_area.pos[m_area.iAreaNum].PtLeftUp.x,
			  m_area.pos[m_area.iAreaNum].PtLeftUp.y, m_area.pos[m_area.iAreaNum].PtRightDn.x, m_area.pos[m_area.iAreaNum].PtRightDn.y);

		  m_area.iAreaNum++;
		  InvalidateRect(NULL,TRUE);
	  }
	  else
	  {
		  TRACE(_T("Warning: All ready achive max area num\n"));
	  }
  }

  CView::OnLButtonUp(nFlags, point);
}

void CCameraView::OnRButtonUp(UINT nFlags, CPoint point)
{
  if(!m_ScrollMode && m_SelectionEnabled)
   ::PostMessage(m_hMsgWnd,m_Msg,0,0);
   
  CView::OnRButtonUp(nFlags, point);
}

void CCameraView::Resize(RECT *pSrcRect,UINT16 NewWidth,UINT16 NewHeight)
{
  BOOL MustClear;
  
  // Not in scroll mode
  if(m_ScrollMode)
   return;

  MustClear=FALSE;
  if(!pSrcRect)
  {
    // Check for clear
    if(m_XSize!=m_OrgXSize || m_YSize!=m_OrgYSize)
     MustClear=TRUE;
    
    // Reset and show full bitmap
    m_SrcRect.left=m_SrcRect.top=0;
    m_SrcRect.right=m_XSize=m_OrgXSize;
    m_SrcRect.bottom=m_YSize=m_OrgYSize;
  }
  else
  {
    m_SrcRect=*pSrcRect;
    if(NewWidth)
    {
      MustClear=TRUE;
      m_XSize=NewWidth;
    }  
    
    if(NewHeight) 
    {
      MustClear=TRUE;
      m_YSize=NewHeight;
    }  
  }  
  
  m_pBitmapInfo->bmiHeader.biWidth=m_XSize;
  m_pBitmapInfo->bmiHeader.biHeight=-m_YSize;

  if(MustClear)
   memset(m_pImage,0,m_pBitmapInfo->bmiHeader.biSizeImage);


  Calculator::GetInstance()->Init(m_XSize, m_YSize);


  InvalidateRect(NULL,FALSE);
}

void CCameraView::SetFrameRegions(int type)
{
	
	if(type == -1)
	{
		m_area.iAreaNum = 0;

		CRect rcClient;
		GetClientRect(&rcClient);
		Calculator::GetInstance()->InitDisplayRegion(rcClient.left, rcClient.right);
	}
	else
	{
		m_bReqRegSelect = TRUE;
		m_bROISelect = type ? 1 : 0;
		m_bNROISelect = type ? 0 : 1;
	}
	InvalidateRect(NULL, FALSE);
}

void CCameraView::SetSliderXPos(int pos)
{
	m_SliderXPos = pos;
	InvalidateRect(NULL, FALSE);
}
void CCameraView::SetSliderYPos(int pos)
{
	m_SliderYPos = pos;
	InvalidateRect(NULL, FALSE);
}

void CCameraView::SetUseMirror(bool bUseMirror)
{
	m_bUseMirror = bUseMirror;
}

void CCameraView::SetImgSavePath(CString imagePath)
{
	m_strImgePath = imagePath;
}

void CCameraView::SaveGrayBitmap()
{
	if (!m_pImage)
	{
		return;
	}

	FIBITMAP *bmp = FreeImage_Allocate(m_XSize,m_YSize,8);
	BYTE *bits = FreeImage_GetBits(bmp);

	memcpy(bits,m_pImage,(UINT32)m_XSize*(UINT32)m_YSize);

	if (!m_strImgePath.IsEmpty())
	{
		CString str;
		str.Format(TEXT("%s\\Image%d.bmp"), m_strImgePath, m_frameIndexInit + m_frameIndex);	
		m_frameIndex += m_differ;
		FreeImage_SaveU(FIF_BMP,bmp, str);
	}

	FreeImage_Unload(bmp);
}


void CCameraView::ResetFrameIndex()
{
	m_frameIndex = 0;
	m_frameIndexInit = 1;
	m_differ = 1;
}

void CCameraView::ResetToIndexForEveryMask(int mask_Index)
{
	m_frameIndex = 0;
	m_frameIndexInit = mask_Index;
}

void CCameraView::SaveColorBitmap()
{
	if (!m_pImage)
	{
		return;
	}

	FIBITMAP *bmp = FreeImage_Allocate(m_XSize,m_YSize,24, 8, 8, 8);
	BYTE *bits = FreeImage_GetBits(bmp);

	memcpy(bits,m_pImage,(UINT32)m_XSize*(UINT32)m_YSize*3);

	if (!m_strImgePath.IsEmpty())
	{
		CString str;
		str.Format(TEXT("%s\\Image%d.bmp"), m_strImgePath, ++m_frameIndex);
		FreeImage_SaveU(FIF_BMP,bmp, str);
	}
}

void CCameraView::GetSelection(double& left, double& right)
{
	if (m_area.iAreaNum  != 0)
	{
		left = static_cast<double>(m_area.pos[0].PtLeftUp.x);
		right = static_cast<double>(m_area.pos[0].PtRightDn.x);
	}
	else
	{
		left = 1.0;
		right = static_cast<double>(m_XSize);
	}
}

int CCameraView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return MA_NOACTIVATE;
	//return CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
