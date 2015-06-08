//DlgSlice.cpp : implementation file
//

#include "stdafx.h"
#include "ETE.h"
#include "DlgSlice.h"
#include "ETESystemPublicDefine.h"
#include "MainFrm.h"
#include "ETEDoc.h"
#include "CommonFunc.h"
// CDlgSlice dialog

IMPLEMENT_DYNAMIC(CDlgSlice, CDialog)

CDlgSlice::CDlgSlice(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSlice::IDD, pParent)
	, m_imgSlice(nullptr)
	, m_leftMouseDown(FALSE)
	, needShowSlice(FALSE)
	, PointerPosY(0)
	, PointerPosX(0)
{
	this->m_pDoc = nullptr;
	this->max = 0.0f;
	this->min = 0.0f;
	this->pOldPen = nullptr;
	this->m_sliceWidth;
	this->m_sliceHeight = 0;
	this->m_leftSide = 0;
	this->m_rightSide = 0;
}

CDlgSlice::~CDlgSlice()
{
}

void CDlgSlice::SetInput(const std::vector<float>& input, int width, int height)
{
	m_vertexList.assign(input.begin(), input.end());
	auto iterator = std::minmax_element(m_vertexList.begin(), m_vertexList.end());
	this->min = *iterator.first;
	this->max = *iterator.second;

	m_sliceWidth = width;
	m_sliceHeight = height;

	if (m_imgSlice)
	{
		cvReleaseImage(&m_imgSlice);
		m_imgSlice = nullptr;
	}
	m_imgSlice = cvLoadImage("heightvalue.png");
	InvalidateRect(NULL);

}

void CDlgSlice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSlice, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgSlice message handlers


BOOL CDlgSlice::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);



	if (!m_pDoc)
	{
		return TRUE;
	}
	

	m_pDoc->GetSectionRange(m_leftSide, m_rightSide);
	m_sliceWidth = m_rightSide - m_leftSide;

 	if (!m_pDoc->m_heightValue.empty())
 	{
		Mat cJetScale;
 		applyColorMap(m_pDoc->m_heightValue, cJetScale, COLORMAP_JET);
 		BitMatToWnd(this, cJetScale, &rcClient);
 	}
 	else
 	{
 		pDC->FillSolidRect(&rcClient, RGB(0,0,0));
 	}
	return TRUE;
}


void CDlgSlice::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_imgSlice)
	{
		cvReleaseImage(&m_imgSlice);
		m_imgSlice = nullptr;
	}
}

void CDlgSlice::OnLButtonDown(UINT nFlags, CPoint point)
{
	//SetCapture();
	m_leftMouseDown = TRUE;
	CDialog::OnLButtonDown(nFlags, point);
}


void CDlgSlice::OnLButtonUp(UINT nFlags, CPoint point)
{
	//ReleaseCapture();
	m_leftMouseDown = FALSE;
	PointerPosX = point.x;
	PointerPosY = point.y;
	CDialog::OnLButtonUp(nFlags, point);
}


void CDlgSlice::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_leftMouseDown)
	{
		PointerPosX = point.x;
		PointerPosY = point.y;
		GetParent()->SendMessage(wm_renderSlice, 0, 0);
	}
	else
	{
		NeedShowSlice(FALSE);
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgSlice::DrawXSlice()
{
	int i;	
	pWnd= GetParent()->GetDlgItem(IDC_XSLICE);

	if (!pWnd)
	{
		return;
	}

	CRect plotrect;
	pWnd->GetClientRect(&plotrect);
	m_xsliceDC=pWnd->GetDC();

	//m_xsliceDC = pXSliceDC;
	//plotrect.CopyRect(&rcXSlice);
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

	int		left=plotrect.left;
	int		wide=plotrect.Width ();

	int		top=plotrect.top;
	int		height=plotrect.Height ();

	//////////////////////////////////////////////////////////////////////////////

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

	pOldFont=(CFont*)m_xsliceDC->SelectObject(pNewFont1.get());

	CString strTextOut;
	strTextOut.Format(TEXT("%.3f"), max);
	m_xsliceDC->TextOut(left+wide/2-25,top-15,strTextOut);
	strTextOut.Format(TEXT("%.3f"), min);
	m_xsliceDC->TextOut(left+wide/2-25,top+height+5,strTextOut);


	m_xsliceDC->SelectObject(pOldFont);

	//画网格
	CPen newPen;
	newPen.CreatePen(PS_DOT,1,RGB(10,10,60));//20,20,80));
	pOldPen=m_xsliceDC->SelectObject(&newPen);

	for(i=1;i<=11;i++)
	{
		m_xsliceDC->MoveTo (left +(i-1)*wide/10,top);
		m_xsliceDC->LineTo (left+(i-1)*wide/10,top+height);
	}

	for(i=1;i<=5;i++)
	{
		m_xsliceDC->MoveTo (left ,top+(i-1)*height/4);
		m_xsliceDC->LineTo (left+ wide,top+(i-1)*height/4);
	}
	m_xsliceDC->SelectObject(pOldPen);

	m_xsliceDC->MoveTo (left,top+height/2);
	m_xsliceDC->LineTo (left+wide,top+height/2);
	m_xsliceDC->MoveTo (left +wide/2,top);
	m_xsliceDC->LineTo (left+wide/2,top+height);

	//	   CPen newPen;
	//	   newPen.CreatePen(PS_DOT,1,RGB(0,255,0));
	//	   pOldPen=m_xsliceDC->SelectObject(&newPen);
	m_xsliceDC->MoveTo (left,top);
	m_xsliceDC->LineTo (left+wide,top);

	CPen pNewPen;      
	pNewPen.CreatePen (PS_SOLID,2,RGB(255,0,0));
	pOldPen=m_xsliceDC->SelectObject (&pNewPen);

	if(PointerPosY>0 && PointerPosY<=m_sliceHeight)
	{
		//				if(type==OBJECT_PLANE)
		//				{
		m_xsliceDC->MoveTo(left,top +height- (m_vertexList[(m_sliceHeight-1-PointerPosY)*m_sliceWidth]-min)/(max-min)*height);		   		
		for(int j=1;j<m_sliceWidth-1;j++)
		{

			int		X=left + j*wide/(m_sliceWidth-2);
			int     Y=top +height- (m_vertexList[((m_sliceHeight-1-PointerPosY)*m_sliceWidth+j)]-min)/(max-min)*height;
			if(j>1)
			{		 
				m_xsliceDC->LineTo (X,Y);
			}
		}
	}

	m_xsliceDC->SelectObject (pOldPen);
//	pWnd->ReleaseDC(m_xsliceDC);
}

void CDlgSlice::DrawYSlice()
{
	// TODO: Add your control notification handler code here

	// TODO: Add your control notification handler code here

	int i;
	//char Cbuffer[50];
	CString strTextOut;

	pWnd=GetParent()->GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错

	if (!pWnd)
	{
		return;
	}

	CRect plotrect;

	pWnd->GetClientRect(&plotrect);
	m_ysliceDC=pWnd->GetDC();
// 	m_ysliceDC = pYSliceDC;
// 	plotrect.CopyRect(&rcYSlice);
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	//注意如果画的次数太多会出错，看情况可以注释掉
	//m_ysliceDC->TextOut(plotrect.right/2+40,20+plotrect.top,TEXT("Y Slice"));
	///////////////////////////////////////////////////////////////////////////////////////////////////	

	m_ysliceDC->SetBkColor(RGB(255,255,255));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int left=plotrect.left +plotrect.Width ()*0.2;
	int top=plotrect.top;
	int wide=plotrect.Width ()*0.7;
	int height=plotrect.Height();


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

	pOldFont=(CFont*)m_ysliceDC->SelectObject(pNewFont.get());


	strTextOut.Format(TEXT("%.6f"), max);
	m_ysliceDC->TextOut(plotrect.Width()/12+2,plotrect.Height()/9.5-plotrect.Height()/2/57.25,strTextOut);
	strTextOut.Format(TEXT("%.6f"), 0.0f);
	m_ysliceDC->TextOut(plotrect.Width()/12+2,plotrect.Height()/7.6+16*plotrect.Height()/58,strTextOut);
	strTextOut.Format(TEXT("%.6f"), min);
	m_ysliceDC->TextOut(plotrect.Width()/12+2,plotrect.Height()/7.6+33*plotrect.Height()/58,strTextOut);
// 	strTextOut.Format(TEXT("%.3f"), minY);
// 	m_ysliceDC->TextOut(left+wide/2-25,top-15,strTextOut);
// 	strTextOut.Format(TEXT("%.3f"), maxY);
// 	m_ysliceDC->TextOut(left+wide/2-25,top+height+5,strTextOut);
 	m_ysliceDC->SelectObject(pOldFont);

// 	CBrush *pOldBrush; 
// 	//画色块	
// 	for(i=1;i<=34;i++)
// 	{
// 		std::shared_ptr<CBrush> pNewBrush= std::make_shared<CBrush>();
// 		if(pNewBrush->CreateSolidBrush(RGB(land[(i-1)*4*7]*255,land[(i-1)*4*7+1]*255,land[(i-1)*4*7+2]*255)))
// 		{
// 			pOldBrush=m_ysliceDC->SelectObject(pNewBrush.get());
// 			m_ysliceDC->Rectangle(plotrect.Width()/48,plotrect.Height()/9+(i-1)*plotrect.Height()/56,plotrect.Width()/14,plotrect.Height()/7.6+(i-1)*plotrect.Height()/56);			
// 			m_ysliceDC->SelectObject(pOldBrush);
// 		}		
// 	}
// 
// 	//画标尺
// 	m_ysliceDC->MoveTo (plotrect.Width()/14+2,plotrect.Height()/9+plotrect.Height()/2/56);
// 	m_ysliceDC->LineTo (plotrect.Width()/14+2,plotrect.Height()/9+33*plotrect.Height()/56+plotrect.Height()/2/56);
// 
// 	for(i=1;i<=34;i++)
// 	{
// 		m_ysliceDC->MoveTo (plotrect.Width()/14+2,plotrect.Height()/9+(i-1)*plotrect.Height()/56+plotrect.Height()/2/56);
// 		if(i%2!=0)
// 			m_ysliceDC->LineTo (plotrect.Width()/14+7,plotrect.Height()/9+(i-1)*plotrect.Height()/56+plotrect.Height()/2/56);
// 		else
// 			m_ysliceDC->LineTo (plotrect.Width()/14+5,plotrect.Height()/9+(i-1)*plotrect.Height()/56+plotrect.Height()/2/56);
// 	}			  
// 	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CBrush *pOldBrush; 
	//画色块	
	for(int i=1;i<=34;i++)
	{
		std::shared_ptr<CBrush> pNewBrush= std::make_shared<CBrush>();
		if(pNewBrush->CreateSolidBrush(RGB(land[(i-1)*4*7]*255,land[(i-1)*4*7+1]*255,land[(i-1)*4*7+2]*255)))
		{
			pOldBrush=m_ysliceDC->SelectObject(pNewBrush.get());
			m_ysliceDC->Rectangle(plotrect.Width()/48,/*plotrect.Height()/6+*/(i-1)*plotrect.Height()/24,plotrect.Width()/12,i*plotrect.Height()/34);			
			m_ysliceDC->SelectObject(pOldBrush);
		}		
	}

	//画标尺
	m_ysliceDC->MoveTo (plotrect.Width()/6+2,/*plotrect.Height()/9+*/plotrect.Height()/2/34);
	m_ysliceDC->LineTo (plotrect.Width()/6+2,/*plotrect.Height()/9+*/33*plotrect.Height()/34+plotrect.Height()/2/34);

	for(int i=1;i<=34;i++)
	{
		m_ysliceDC->MoveTo (plotrect.Width()/6+2,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/34+plotrect.Height()/2/34);
		if(i%2!=0)
			m_ysliceDC->LineTo (plotrect.Width()/6+7,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/34+plotrect.Height()/2/34);
		else
			m_ysliceDC->LineTo (plotrect.Width()/6+5,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/34+plotrect.Height()/2/34);
	}			  


	//画网格
	CPen newPen;
	newPen.CreatePen(PS_DOT,1,RGB(10,10,60));//20,20,80));
	pOldPen=m_ysliceDC->SelectObject(&newPen);

	for(i=1;i<=11;i++)
	{
		m_ysliceDC->MoveTo (left,top+(i-1)*height/10);
		m_ysliceDC->LineTo (left+wide,top+(i-1)*height/10);
	}

	for(i=1;i<=5;i++)
	{
		m_ysliceDC->MoveTo (left +(i-1)*wide/4,top);
		m_ysliceDC->LineTo (left+(i-1)*wide/4,top+height);
	}
	m_ysliceDC->SelectObject(pOldPen);

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

	pOldFont=(CFont*)m_ysliceDC->SelectObject(pNewFont1.get());
	strTextOut.Format(TEXT("%.3f"), max);
	m_ysliceDC->TextOut(left+wide+15,top+height/2-20,strTextOut);

	strTextOut.Format(TEXT("%.3f"), min);
	m_ysliceDC->TextOut(left-5,top+height/2-20,strTextOut);

	m_ysliceDC->SelectObject(pOldFont);

	//////////////////////////////////////////////////////////////////////////////////
	m_ysliceDC->MoveTo (left +wide/2,top);
	m_ysliceDC->LineTo (left+wide/2,top+height);
	m_ysliceDC->MoveTo (left ,top+height/2);
	m_ysliceDC->LineTo (left+wide,top+height/2);

	m_ysliceDC->MoveTo (left+wide,top);
	m_ysliceDC->LineTo (left +wide,top+height);

	if(PointerPosX>0 && PointerPosX<=m_sliceWidth)
	{ 
		CPen pNewPen;      
		pNewPen.CreatePen (PS_SOLID,2,RGB(0,255,0));
		pOldPen=m_ysliceDC->SelectObject (&pNewPen);

		if (PointerPosX>0 && PointerPosX<=m_sliceWidth)
		{
			//			if(type==OBJECT_PLANE)
			//			{
			m_ysliceDC->MoveTo(left + (m_vertexList[PointerPosX]-min)/(max-min)*wide,top);	
			for(int j=1;j<m_sliceHeight-1;j++)
			{              
				int		X=left + (m_vertexList[((m_sliceHeight-1-j)*m_sliceWidth+PointerPosX)]-min)/(max-min)*wide;//Buffer[(j*(Width-1)+Xnum)*3+2]-min)/(max-min)*wide;
				int     Y=top+j*height/(m_sliceHeight-2);
				if(j>1)
				{		 
					m_ysliceDC->LineTo (X,Y);
				}
			}
		}		
		m_ysliceDC->SelectObject (pOldPen);
	}
//	pWnd->ReleaseDC(m_ysliceDC);
}



void CDlgSlice::OnTimer(UINT_PTR nIDEvent)
{
	InvalidateRect(NULL);
	CDialog::OnTimer(nIDEvent);
}


BOOL CDlgSlice::OnInitDialog()
{
	CDialog::OnInitDialog();


	//SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSlice::RefreshAll()
{


//   	pWnd=GetParent()->GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错
//   	m_ysliceDC=pWnd->GetDC();
//   	pWnd->Invalidate();
//   	pWnd->UpdateWindow();
//   	DrawYSlice();
//   	pWnd=GetParent()->GetDlgItem(IDC_XSLICE);//一定要再次得到指针才能更新，否则会出错
//   	m_xsliceDC=pWnd->GetDC();
//   	pWnd->Invalidate();
//   	pWnd->UpdateWindow(); 
//   	DrawXSlice();
}

void CDlgSlice::SetDocument(CETEDoc* m_pDocument)
{
	if (m_pDocument)
	{
		m_pDoc = m_pDocument;
	}
}
