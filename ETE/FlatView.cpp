// FlatView.cpp : implementation file
//

#include "stdafx.h"
#include "ETE.h"
#include "FlatView.h"
#include "Calculator.h"
#include "MatrixUntity.h"
#include "ETESystemPublicDefine.h"
#include <core\core.hpp>
#include "ETEDoc.h"


const int timer_render = 1;

#define  wm_renderSlice (WM_USER + 1)

// CFlatView

IMPLEMENT_DYNCREATE(CFlatView, CFormView)

CFlatView::CFlatView()
	: CFormView(CFlatView::IDD)
	, pWnd(nullptr)
{
// 	this->m_hDC = NULL;
// 	this->m_ScalFactor = 0.2f;
// 	this->m_width = 0;
// 	this->m_height = 0;
// 	this->m_bReloading = 0;
// 	this->m_GLPixelIndex = 0;
// 	this->m_hGLContext = NULL;
// 	this->sceneRot[0]	= 0;
// 	this->sceneRot[1]	= 0;
// 	this->sceneRot[2]	= 0;
// 	this->m_SceneCenter.x = 0;
// 	this->m_SceneCenter.y = 0;
// 	this->m_SceneCenter.z = 0;
// 	this->mouseleftdown = FALSE;
// 	this->mouserightdown = FALSE;
// 	this->mouseprevpoint.x = 0;
// 	this->mouseprevpoint.y = 0;

	this->minX = this->minY = this->minZ = 0;
	this->maxX = this->maxY = this->maxZ = 0;

	this->min = this->max = 0.0f;
	this->m_sliceWidth = this->m_sliceHeight = VIEW_HEIGHT;
}

CFlatView::~CFlatView()
{
}

void CFlatView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
// 	DDX_Control(pDX, IDC_CMB_FILTER, m_cmbFilter);
// 	DDX_Control(pDX, IDC_BTN_FILTER, m_btnStartFilter);
// 	DDX_Control(pDX, IDC_CMB_FILTER_TYPE, m_cmbFilterType);
}

BEGIN_MESSAGE_MAP(CFlatView, CFormView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(wm_renderSlice, &CFlatView::OnRenderSlice)
// 	ON_BN_CLICKED(IDC_BTN_FILTER, &CFlatView::OnBnClickedBtnFilter)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CFlatView diagnostics

#ifdef _DEBUG
void CFlatView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFlatView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG



// CFlatView message handlers


BOOL CFlatView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
	{
		SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}

// 	if (pMsg->message == WM_MOUSEWHEEL)
// 	{
// 		m_dlgHeightValue.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
// 	}


	return CFormView::PreTranslateMessage(pMsg);
}


BOOL CFlatView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CFormView::PreCreateWindow(cs);
}

BOOL CFlatView::SetWindowPixelFormat(HDC hDC)
{//定义窗口的像素格式
	PIXELFORMATDESCRIPTOR pixelDesc=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|
		PFD_DOUBLEBUFFER|PFD_SUPPORT_GDI,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	this->m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(this->m_GLPixelIndex==0)
	{
		this->m_GLPixelIndex = 1;
		if(DescribePixelFormat(hDC,this->m_GLPixelIndex,sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
		{
			return FALSE;
		}
	}

	if(SetPixelFormat(hDC,this->m_GLPixelIndex,&pixelDesc)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CFlatView::CreateViewGLContext(HDC hDC)
{
	this->m_hGLContext = wglCreateContext(hDC);
	if(this->m_hGLContext==NULL)
	{//创建失败
		return FALSE;
	}

	if(wglMakeCurrent(hDC,this->m_hGLContext)==FALSE)
	{//选为当前RC失败
		return FALSE;
	}

	return TRUE;


} 


int CFlatView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void CFlatView::OnDestroy()
{
	CFormView::OnDestroy();
}


void CFlatView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	
	CWnd* pWnd = GetDlgItem(IDC_STATIC_HEIGHT_VALUE);

	if (!pWnd)
	{
		return;
	}

	CRect rcClient;
	pWnd->GetWindowRect(&rcClient);
	ScreenToClient(&rcClient);

	glViewport(rcClient.left,rcClient.top, rcClient.Width(),rcClient.Height());

	double aspect = (double)rcClient.Width()/ rcClient.Height();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, aspect, 1.0, 100.0);
	gluLookAt(0,0, 70, 0,0,0, 0, 1, 0);
}


void CFlatView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CFormView::OnPaint() for painting messages
	//DrawSurface();

	CETEDoc* pDoc = (CETEDoc*)GetDocument();

	if (!pDoc)
	{
		return;
	}

	cv::minMaxIdx(pDoc->m_heightValueOrign, &min, &max);

	//刷新窗口 
	pWnd=GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错
	m_ysliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	DrawYSlice();

	//刷新窗口 
	pWnd=GetDlgItem(IDC_XSLICE);//一定要再次得到指针才能更新，否则会出错
	m_xsliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow(); 
	DrawXSlice();
}

void CFlatView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	GetDlgItem(IDC_STATIC_PICTURE)->MoveWindow(0,0, VIEW_WIDTH, VIEW_HEIGHT);

	m_dlgSlice.Create(IDD_DLGSLICE, this);
 	m_dlgSlice.MoveWindow(0,0, VIEW_WIDTH, VIEW_HEIGHT);
  	m_dlgSlice.ShowWindow(SW_NORMAL);
	m_dlgSlice.SetDocument(dynamic_cast<CETEDoc*>(m_pDocument));
	CRect rcTemp;
	GetDlgItem(IDC_YSLICE)->GetWindowRect(&rcTemp);
	GetDlgItem(IDC_YSLICE)->MoveWindow(VIEW_WIDTH + 50, 0, rcTemp.Width(), VIEW_HEIGHT);

	GetDlgItem(IDC_XSLICE)->GetWindowRect(rcTemp);
	GetDlgItem(IDC_XSLICE)->MoveWindow(0, VIEW_HEIGHT + 50, VIEW_WIDTH, rcTemp.Height());

	this->GetDlgItem(IDC_STATIC_MAX_VALUE)->GetWindowRect(&m_rcMaxalue);
	ScreenToClient(&m_rcMaxalue);
	m_dlgMaxValue.Create(IDD_DLGMAXVALUE, this);
	m_dlgMaxValue.MoveWindow(m_rcMaxalue.left, m_rcMaxalue.top, VIEW_WIDTH, VIEW_HEIGHT);
	m_dlgMaxValue.ShowWindow(SW_NORMAL);
	m_dlgMaxValue.SetDocument(dynamic_cast<CETEDoc*>(m_pDocument));

	GetDlgItem(IDC_YSLICE)->GetWindowRect(&m_rcYSlice);
	ScreenToClient(&m_rcYSlice);

	GetDlgItem(IDC_XSLICE)->GetWindowRect(&m_rcXSlice);
	ScreenToClient(&m_rcXSlice);
	// Uncomment out the first block of code below, and then the second block,   
	//  to see how they affect line and point drawing.   

	// The following commands should cause points and line to be drawn larger   
	// than a single pixel width.   
	//glPointSize(8);  
	//glLineWidth(5);  
	//SetTimer(1, 100, NULL);
	// The following commands should induce OpenGL to create round points and    
	// antialias points and lines.  (This is implementation dependent unfortunately).   
	//RGBA mode antialias need cooperate with blend Calculator.   
	glEnable(GL_POINT_SMOOTH);  
	glEnable(GL_LINE_SMOOTH);  
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points   
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines   
	glEnable(GL_BLEND);  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

//	m_cmbFilter.SetCurSel(0);
//	m_cmbFilterType.SetCurSel(2);
//	Calculator::GetInstance()->AssignView(this);
}


void CFlatView::OnTimer(UINT_PTR nIDEvent)
{
	if (m_dlgSlice.NeedShowSlice())
	{
		KillTimer(1);
		InvalidateRect(m_rcXSlice);
		InvalidateRect(m_rcYSlice);
		SetTimer(1, 100, NULL);
	}
	CFormView::OnTimer(nIDEvent);
}


BOOL CFlatView::OnEraseBkgnd(CDC* pDC)
{
	return CView::OnEraseBkgnd(pDC);
}


void CFlatView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CFormView::OnRButtonDown(nFlags, point);
}





void CFlatView::SetFilterStrength(int strength)
{
	m_bReloading = TRUE;
	
	m_ptrZCoordListAfterFilter.clear();

//	int curSel = m_cmbFilterType.GetCurSel();


// 	CV_BLUR_NO_SCALE =0,
// 		CV_BLUR  =1,
// 		CV_GAUSSIAN  =2,
// 		CV_MEDIAN =3,
// 		CV_BILATERAL =4


	try
	{
// 		switch(curSel)
// 		{
// 		case 0:
// 			Helper::ImageUntity::Smooth(m_ptrZCoordList, m_ptrZCoordListAfterFilter, m_height, m_width, 3, strength);
// 			break;
// 		case 1:
// 			Helper::ImageUntity::Smooth(m_ptrZCoordList, m_ptrZCoordListAfterFilter, m_height, m_width, 1, strength);
// 			break;
// 		case 2:
// 			Helper::ImageUntity::Smooth(m_ptrZCoordList, m_ptrZCoordListAfterFilter, m_height, m_width, 2, strength);
// 			break;
// 		default:
// 			break;
// 		}

		Helper::ImageUntity::MedioFilter(m_ptrZCoordList, m_ptrZCoordListAfterFilter, strength);
	}
	catch(cv::Exception& e)
	{
		MessageBoxA(NULL, e.what(), "CV Exception", MB_OK);
	}
	m_bReloading = FALSE;
	SleepMs(10);
	m_btnStartFilter.EnableWindow(TRUE);
}

void CFlatView::OnBnClickedBtnFilter()
{
	m_btnStartFilter.EnableWindow(FALSE);
	int curSel = m_cmbFilter.GetCurSel();
	int strength = 1;
	switch(curSel)
	{
	case 0:
		strength = 1;
		break;
	case 1:
		strength = 3;
		break;
	case 2:
		strength = 5;
		break;
	case 3:
		strength = 7;
		break;
	case 4:
		strength = 9;
		break;
	default:
		break;
	}

	SetFilterStrength(strength);
}

void CFlatView::DrawSurface()
{
	CRect plotrect;
	pWnd->GetClientRect(&plotrect);
}

void CFlatView::DrawXSlice()
{
	int i;	
	pWnd= GetDlgItem(IDC_XSLICE);

	if (!pWnd)
	{
		return;
	}

	CRect plotrect;
	pWnd->GetClientRect(&plotrect);
	m_xsliceDC=pWnd->GetDC();

	int		left=plotrect.left;
	int		wide=plotrect.Width ();

	int		top=plotrect.top+plotrect.Height()*0.1;
	int		height=plotrect.Height ()*0.8;


	CDC   MemDC;   //首先定义一个显示设备对象 ，就像一只画笔。 
	CBitmap   MemBitmap;//定义一个位图对象 ，这个就如同纸B ，但现在还是无限大的

	//随后建立与屏幕显示兼容的内存显示设备  ， 就像给将要绘制的图像定一个基调，是黑白图、彩色图、灰度图？
	MemDC.CreateCompatibleDC(NULL);  
	//这时还不能绘图，因为没有地方画   ^_^  
	//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小  ，这里就是将大纸裁剪成合适的大小

	MemBitmap.CreateCompatibleBitmap(m_xsliceDC,wide,height);  

	//将位图选入到内存显示设备中  
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上 ，将画笔拿在手里，这个时候就能在纸B上开画了，否则空手怎么画？ 
	CBitmap   *pOldBit=MemDC.SelectObject(&MemBitmap);  

	//先用背景色将位图清除干净，这里我用的是白色作为背景  
	//你也可以用自己应该用的颜色  

	//这里的作用是因为Windows平台提供的最原始的纸都是黑色的，所以我们需要将他们粉刷成白色。
	MemDC.FillSolidRect(0,0,wide,height,RGB(255,255,255));  

	//绘图  ，这里就开始绘图了，画条直线，画朵花，全凭你的想象了，但是，嘿嘿，要有好的绘图技术哦！
	CFont* pOldFont;
	std::shared_ptr<CFont> pNewFont1= std::make_shared<CFont>();
	pNewFont1->CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));      

	pOldFont=(CFont*)MemDC.SelectObject(pNewFont1.get());

	CString strTextOut;
	strTextOut.Format(TEXT("%.3f"), static_cast<float>(max));
	MemDC.TextOut(left+wide/2-25,top,strTextOut);
	strTextOut.Format(TEXT("%.3f"), static_cast<float>(min));
	MemDC.TextOut(left+wide/2-25,top+height-15,strTextOut);

	MemDC.SelectObject(pOldFont);

	// 	std::shared_ptr<CFont> pNewFont2= std::make_shared<CFont>();
	// 	pNewFont2->CreateFont(
	// 		10,                        // nHeight
	// 		0,                         // nWidth
	// 		2700,                         // nEscapement
	// 		0,                         // nOrientation
	// 		FW_NORMAL,                 // nWeight
	// 		FALSE,                     // bItalic
	// 		FALSE,                     // bUnderline
	// 		0,                         // cStrikeOut
	// 		ANSI_CHARSET,              // nCharSet
	// 		OUT_DEFAULT_PRECIS,        // nOutPrecision
	// 		CLIP_DEFAULT_PRECIS,       // nClipPrecision
	// 		DEFAULT_QUALITY,           // nQuality
	// 		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	// 		_T("Arial"));      
	// 
	// 	pOldFont=(CFont*)MemDC.SelectObject(pNewFont2.get());
	// 
	// 	strTextOut.Format(TEXT("%.3f"), minX);
	// 	MemDC.TextOut(left+wide+15,top+height/2-20,strTextOut);
	// 
	// 	strTextOut.Format(TEXT("%.3f"), maxX);
	// 	MemDC.TextOut(left-5,top+height/2-20,strTextOut);
	// 
	// 	MemDC.SelectObject(pOldFont);
	//////////////////////////////////////////////////////////////////////////////////


	//画网格
	CPen newPen;
	newPen.CreatePen(PS_DOT,1,RGB(10,10,60));//20,20,80));
	pOldPen=MemDC.SelectObject(&newPen);

	for(i=1;i<=11;i++)
	{
		MemDC.MoveTo (left +(i-1)*wide/10,top);
		MemDC.LineTo (left+(i-1)*wide/10,top+height);
	}

	for(i=1;i<=5;i++)
	{
		MemDC.MoveTo (left ,top+(i-1)*height/4);
		MemDC.LineTo (left+ wide,top+(i-1)*height/4);
	}
	MemDC.SelectObject(pOldPen);

	MemDC.MoveTo (left,top+height/2);
	MemDC.LineTo (left+wide,top+height/2);
	MemDC.MoveTo (left +wide/2,top);
	MemDC.LineTo (left+wide/2,top+height);
	MemDC.MoveTo (left,top);
	MemDC.LineTo (left+wide,top);

	CPen pNewPen;      
	pNewPen.CreatePen (PS_SOLID,2,RGB(255,0,0));
	pOldPen=MemDC.SelectObject (&pNewPen);

	CETEDoc* pDoc = dynamic_cast<CETEDoc*>(GetDocument());

	if (!pDoc)
	{
		return;
	}

	if (pDoc->m_heightValueOrign.empty())
	{
		return;
	}

	Mat heightValue = pDoc->m_heightValueOrign;

	LONG leftSide(0), rightSide(0);
	pDoc->GetSectionRange(leftSide, rightSide);
	m_sliceWidth = rightSide - leftSide;
	int PointerPosY = m_dlgSlice.GetYPosition();
	int PointerPosX = m_dlgSlice.GetXPosition();
	if(PointerPosX > 0 && PointerPosX < m_sliceWidth&& PointerPosY>0 && PointerPosY<=m_sliceHeight && !heightValue.empty())
	{
		//MemDC.MoveTo(left,top +height- (m_vertexList[(m_sliceHeight-1-PointerPosY)*m_sliceWidth]-min)/(max-min)*height);		   		
 		MemDC.MoveTo(left,top +height- (heightValue.at<float>(m_sliceHeight-1-PointerPosY, 0)-min)/(max-min)*height);		   		
 		for(int j=1;j<m_sliceWidth-1;j++)
 		{
 
 			int		X=left + j*wide/(m_sliceWidth-2);
 			//int     Y=top +height- (m_vertexList[((m_sliceHeight-1-PointerPosY)*m_sliceWidth+j)]-min)/(max-min)*height;
 			int     Y=top +height- (heightValue.at<float>(m_sliceHeight-1-PointerPosY,j)-min)/(max-min)*height;
 			if(j>1)
 			{		 
 				MemDC.LineTo (X,Y);
 			}
 		}
	}

	MemDC.SelectObject (pOldPen);
	//将内存中的图拷贝到屏幕上进行显示  ，这里就是将纸B覆盖到纸A上。
	m_xsliceDC->BitBlt(0,0,wide,height,&MemDC,0,0,SRCCOPY);  

	//绘图完成后的清理  ， 之后呢就将纸B销毁，就是释放内存了，不然这样的纸B多了，电脑会慢的。
	MemBitmap.DeleteObject();  
	MemDC.DeleteDC();  




	///////////////////////////////////////////////////////////////////////////////////////////////////
	//注意如果画的次数太多会出错，看情况可以注释掉
	//m_xsliceDC->TextOut(plotrect.left+20,plotrect.top+plotrect.Height()/2-10,TEXT("X Slice"));
	///////////////////////////////////////////////////////////////////////////////////////////////////

	/*	int		left=plotrect.left +plotrect.Width ()*0.2;
	int		top=plotrect.top+plotrect.Height ()*0.1 ;
	int		wide=plotrect.Width ()*0.7;
	int		height=plotrect.Height ()*0.8;*/

// 	int		left=plotrect.left +plotrect.Width ()*0.1;
// 	int		wide=plotrect.Width ()*0.7;
// 
// 	int		top=plotrect.top+plotrect.Height ()*0.1 ;
// 	int		height=plotrect.Height ()*0.8;

	

	//////////////////////////////////////////////////////////////////////////////


//	pWnd->ReleaseDC(m_xsliceDC);
}

void CFlatView::DrawYSlice()
{
	// TODO: Add your control notification handler code here

	// TODO: Add your control notification handler code here

	int i;
	//char Cbuffer[50];
	CString strTextOut;

	pWnd=GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错

	if (!pWnd)
	{
		return;
	}

	CRect plotrect;

	pWnd->GetClientRect(&plotrect);
	m_ysliceDC=pWnd->GetDC();

	int left=plotrect.left +plotrect.Width ()*0.2;
	int top=plotrect.top;
	int wide=plotrect.Width () - plotrect.Width() / 12;
	int height=plotrect.Height();

	CDC   MemDC;   //首先定义一个显示设备对象 ，就像一只画笔。 
	CBitmap   MemBitmap;//定义一个位图对象 ，这个就如同纸B ，但现在还是无限大的

	//随后建立与屏幕显示兼容的内存显示设备  ， 就像给将要绘制的图像定一个基调，是黑白图、彩色图、灰度图？
	MemDC.CreateCompatibleDC(NULL);  
	//这时还不能绘图，因为没有地方画   ^_^  
	//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小  ，这里就是将大纸裁剪成合适的大小

	MemBitmap.CreateCompatibleBitmap(m_ysliceDC,wide,height);  

	//将位图选入到内存显示设备中  
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上 ，将画笔拿在手里，这个时候就能在纸B上开画了，否则空手怎么画？ 
	CBitmap   *pOldBit=MemDC.SelectObject(&MemBitmap);  

	//先用背景色将位图清除干净，这里我用的是白色作为背景  
	//你也可以用自己应该用的颜色  

	//这里的作用是因为Windows平台提供的最原始的纸都是黑色的，所以我们需要将他们粉刷成白色。
	MemDC.FillSolidRect(0,0,wide,height,RGB(255,255,255));  



	//创建新字体

	CFont* pOldFont;
	std::shared_ptr<CFont> pNewFont= std::make_shared<CFont>();
	pNewFont->CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));      

	pOldFont=(CFont*)MemDC.SelectObject(pNewFont.get());


 	strTextOut.Format(TEXT("%.3f"), max);
 	MemDC.TextOut(plotrect.Width()/12+2,plotrect.Height()/9.5-plotrect.Height()/2/57.25,strTextOut);
 	strTextOut.Format(TEXT("%.3f"), 0.0f);
 	MemDC.TextOut(plotrect.Width()/12+2,plotrect.Height()/7.6+16*plotrect.Height()/58,strTextOut);
 	strTextOut.Format(TEXT("%.3f"), min);
 	MemDC.TextOut(plotrect.Width()/12+2,plotrect.Height()/7.6+33*plotrect.Height()/58,strTextOut);
// 	strTextOut.Format(TEXT("%.3f"), minY);
// 	MemDC.TextOut(left+wide/2-25,top-15,strTextOut);
// 	strTextOut.Format(TEXT("%.3f"), maxY);
// 	MemDC.TextOut(left+wide/2-25,top+height+5,strTextOut);
 	MemDC.SelectObject(pOldFont);

	CBrush *pOldBrush; 
	//画色块	
	for(int i=1;i<=64;i++)
	{
		std::shared_ptr<CBrush> pNewBrush= std::make_shared<CBrush>();
		if(pNewBrush->CreateSolidBrush(RGB(hsv[(i-1)*3]*255,hsv[(i-1)*3+1]*255,hsv[(i-1)*3+2]*255)))
		{
			pOldBrush=MemDC.SelectObject(pNewBrush.get());
			MemDC.Rectangle(plotrect.Width()/48,/*plotrect.Height()/6+*/(i-1)*plotrect.Height()/64,plotrect.Width()/12,i*plotrect.Height()/64);			
			MemDC.SelectObject(pOldBrush);
		}		
	}

	//画标尺
	MemDC.MoveTo (plotrect.Width()/12+2,/*plotrect.Height()/9+*/plotrect.Height()/2/64);
	MemDC.LineTo (plotrect.Width()/12+2,/*plotrect.Height()/9+*/33*plotrect.Height()/64+plotrect.Height()/2/64);

	for(int i=1;i<=64;i++)
	{
		MemDC.MoveTo (plotrect.Width()/12+2,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/64+plotrect.Height()/2/64);
		if(i%2!=0)
			MemDC.LineTo (plotrect.Width()/12+7,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/64+plotrect.Height()/2/64);
		else
			MemDC.LineTo (plotrect.Width()/12+5,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/64+plotrect.Height()/2/64);
	}			  


	//画网格
	CPen newPen;
	newPen.CreatePen(PS_DOT,1,RGB(10,10,60));//20,20,80));
	pOldPen=MemDC.SelectObject(&newPen);

	for(i=1;i<=11;i++)
	{
		MemDC.MoveTo (left,top+(i-1)*height/10);
		MemDC.LineTo (left+wide,top+(i-1)*height/10);
	}

	for(i=1;i<=5;i++)
	{
		MemDC.MoveTo (left +(i-1)*wide/4,top);
		MemDC.LineTo (left+(i-1)*wide/4,top+height);
	}
	MemDC.SelectObject(pOldPen);

	//////////////////////////////////////////////////////////////////////////////
	std::shared_ptr<CFont> pNewFont1= std::make_shared<CFont>();
	pNewFont1->CreateFont(
		10,                        // nHeight
		0,                         // nWidth
		2700,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));      

	pOldFont=(CFont*)MemDC.SelectObject(pNewFont1.get());
	strTextOut.Format(TEXT("%.3f"), max);
	MemDC.TextOut(left+wide+15,top+height/2-20,strTextOut);

	strTextOut.Format(TEXT("%.3f"), min);
	MemDC.TextOut(left-5,top+height/2-20,strTextOut);

	MemDC.SelectObject(pOldFont);

	//////////////////////////////////////////////////////////////////////////////////
	MemDC.MoveTo (left +wide/2,top);
	MemDC.LineTo (left+wide/2,top+height);
	MemDC.MoveTo (left ,top+height/2);
	MemDC.LineTo (left+wide,top+height/2);

	MemDC.MoveTo (left+wide,top);
	MemDC.LineTo (left +wide,top+height);

	int PointerPosX = m_dlgSlice.GetXPosition();


	CETEDoc* pDoc = dynamic_cast<CETEDoc*>(GetDocument());

	if (!pDoc)
	{
		return;
	}

	Mat heighvalue = pDoc->m_heightValueOrign;
	LONG leftSide(0), rightSide(0);
	pDoc->GetSectionRange(leftSide, rightSide);
	m_sliceWidth = rightSide - leftSide;
	if(PointerPosX >0&& PointerPosX < m_sliceWidth&& !heighvalue.empty())
	{ 
  		CPen pNewPen;      
  		pNewPen.CreatePen (PS_SOLID,2,RGB(0,255,0));
  		pOldPen=MemDC.SelectObject (&pNewPen);
  
  			MemDC.MoveTo(left + (heighvalue.at<float>(0, PointerPosX)-min)/(max-min)*wide,top);	
  			for(int j=1;j<m_sliceHeight-1;j++)
  			{              
  				int		X=left + (heighvalue.at<float>(m_sliceHeight-1-j, PointerPosX)-min)/(max-min)*wide;//Buffer[(j*(Width-1)+Xnum)*3+2]-min)/(max-min)*wide;
  				int     Y=top+j*height/(m_sliceHeight-2);
  				if(j>1)
  				{		 
  					MemDC.LineTo (X,Y);
  				}
  			}
  		MemDC.SelectObject (pOldPen);
	}

	MemDC.SelectObject (pOldPen);
	//将内存中的图拷贝到屏幕上进行显示  ，这里就是将纸B覆盖到纸A上。
	m_ysliceDC->BitBlt(0,0,wide,height,&MemDC,0,0,SRCCOPY);  

	//绘图完成后的清理  ， 之后呢就将纸B销毁，就是释放内存了，不然这样的纸B多了，电脑会慢的。
	MemBitmap.DeleteObject();  
	MemDC.DeleteDC();  

}

LRESULT CFlatView::OnRenderSlice(WPARAM wParam, LPARAM lParam)
{
	InvalidateRect(&m_rcYSlice, FALSE);
	InvalidateRect(&m_rcXSlice, FALSE);
	return 0;
}
