// CDlgHeightValue.cpp : 实现文件
//

#include "stdafx.h"
#include "ETE.h"
#include "DlgHeightValue.h"
//#include "Dialog_h\Dialog_SurfaceDisplay.h"
#include "OpticMAPView.h"
//#include "..\..\include\dialog_h\dialog_surfacedisplay.h"
#include <tchar.h>

#include "glfunctions.h"
#include "XPolynomial.h"
#include <FreeImage.h>
#include <atlconv.h>
// CDlgHeightValue 对话框



IMPLEMENT_DYNAMIC(CDlgHeightValue, CDialog)

	CDlgHeightValue::CDlgHeightValue(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHeightValue::IDD, pParent)
{

}

CDlgHeightValue::~CDlgHeightValue()
{

}

void CDlgHeightValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHeightValue, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_LOADTEXTURE, &CDlgHeightValue::OnBtnLoadtexture)
	ON_BN_CLICKED(IDC_SLICESHOW, OnBnClickedSliceshow)
	ON_BN_CLICKED(IDC_DEEPEST, OnBnClickedDeepest)
	ON_BN_CLICKED(IDC_PNTSHOW, OnBnClickedSurshow)
	ON_BN_CLICKED(IDC_ZOOMIN, OnBnClickedZoomin)
	ON_BN_CLICKED(IDC_ZOOMOUT, OnBnClickedZoomout)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_NEXTCOR, OnBnClickedNextcor)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(DIARESET, OnBnClickedDiareset)
	ON_BN_CLICKED(IDC_REVSUR, OnBnClickedRevsur)
	ON_BN_CLICKED(IDC_DISMEAS, OnBnClickedDismeas)
	ON_BN_CLICKED(IDC_SNAP, OnBnClickedSnap)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_LOAD_TEXTURE, &CDlgHeightValue::OnLoadTexture)
END_MESSAGE_MAP()


// CDlgHeightValue 消息处理程序


BOOL CDlgHeightValue::OnInitDialog()
{
	CDialog::OnInitDialog();


	InitializeSysAllParameters();

	GetClientRect(&Mysrect);
	BOOL result = loader_.Load("pfile.asc");

	if (result)
	{
		m_ptrVertexList = loader_.Data();
		loader_.GetBox(this->m_SceneCenter,this->m_SceneVolume);
	}
	else
	{
		AfxMessageBox(TEXT("Get Asc Data Failed!"));
		return FALSE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	m_TrackBall.SetTrackWindow(Mysrect.Width(), Mysrect.Height());
	///////////////////////////////////////////////////////////////////////////////////////////
	Height=392,Width=181;
	InitTerreno();

	m_depthThreshold = min;

	//if(maxX-minX>maxY-minY)
	//     m_ScalFactor		= 1/(maxX-minX);
	//else
	//	m_ScalFactor		= 1/(maxY-minY);	

	if (!InitializeOpenGLContent())
	{
		AfxMessageBox(TEXT("InitializeOpenGLContent Failed!"));
		return FALSE;
	}


	LayoutAllSysComponents();




	/////////////////////////////////////////////////////////////////////////////////////////////////////
	CurPitNum=0;
	DataDis();


	OnBnClickedDeepest();

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgHeightValue::OnTimer(UINT_PTR nIDEvent)
{
	openGLControl.Invalidate();
	CDialog::OnTimer(nIDEvent);
}


void CDlgHeightValue::OnBtnLoadtexture()
{
	CFileDialog dlgLoad(TRUE,_T("*"),_T("texturefile"),NULL,_T("Image Files (*.jpg;*.bmp)|*.jpg;*.bmp| All Files (*.*) | *.*|"));

	if (dlgLoad.DoModal() == IDOK)
	{
		CString strTexturePath = dlgLoad.GetPathName();

		openGLControl.LoadTexture(strTexturePath.GetString());
		strTexturePath.ReleaseBuffer();

	}
}



void CDlgHeightValue::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DataDis();


	//刷新窗口 
	pWnd=GetDlgItem(IDC_SURFACESHOW);//一定要再次得到指针才能更新，否则会出错
	m_ppDC=pWnd->GetDC();
	wglMakeCurrent(m_ppDC->GetSafeHdc(), hrc);
	DrawSurface();

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

	// 	pWnd=GetDlgItem(IDC_SURFACEHISTOGRAM);//一定要再次得到指针才能更新，否则会出错
	// 	m_surfacehisDC=pWnd->GetDC();	
	// 	pWnd->Invalidate();
	// 	pWnd->UpdateWindow(); 
	// 	DrawHistogram();
}

void CDlgHeightValue::OnBnClickedSliceshow()
{
	// TODO: Add your control notification handler code here
	m_bShowSlice = TRUE;
	m_Orientation.m_Angle=0;
	m_Orientation.m_Axis[0]=0, m_Orientation.m_Axis[1]=0, m_Orientation.m_Axis[2]=1;				
	DrawSurface();	
	m_deepestShow = FALSE;
}


// when the slider is painted, this function is called. data1 is the control
// id (so we know WHICH slider is calling us), sValue is the GetPos() value,
// and IsDragging indicates if the mouse is dragging the slider or not.
//
// We could have simplified this function call by just using the control ID
// to call GetPos() but determining whether the left button is down or not
// is more difficult.


void CDlgHeightValue::DrawYSlice()
{
	// TODO: Add your control notification handler code here

	// TODO: Add your control notification handler code here

	int i;
	//char Cbuffer[50];
	CString strTextOut;

	pWnd=GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错
	CRect plotrect;
	pWnd->GetClientRect(&plotrect);
	m_ysliceDC=pWnd->GetDC();
	////////////////////////////////////////////////////////////////////////////////////////////////////	
	//注意如果画的次数太多会出错，看情况可以注释掉
	m_ysliceDC->TextOut(plotrect.right/2+40,20+plotrect.top,TEXT("Y Slice"));
	///////////////////////////////////////////////////////////////////////////////////////////////////	

	m_ysliceDC->SetBkColor(RGB(255,255,255));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int		left=plotrect.left +plotrect.Width ()*0.5;
	int		top=plotrect.top+plotrect.Height ()*0.15 ;
	int		wide=plotrect.Width ()*0.4;
	int		height=plotrect.Height ()*0.75;


	//创建新字体

	CFont* pOldFont;
	boost::shared_ptr<CFont> pNewFont= boost::make_shared<CFont>();
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

	//if(maxrn==minrn)
	//	   minrn=0;


	strTextOut.Format(TEXT("%.6f"), max);
	m_ysliceDC->TextOut(plotrect.Width()/12+2,plotrect.Height()/9.5-plotrect.Height()/2/57.25,strTextOut);
	strTextOut.Format(TEXT("%.6f"), 0.0f);
	m_ysliceDC->TextOut(plotrect.Width()/12+2,plotrect.Height()/7.6+16*plotrect.Height()/58,strTextOut);
	strTextOut.Format(TEXT("%.6f"), min);
	m_ysliceDC->TextOut(plotrect.Width()/12+2,plotrect.Height()/7.6+33*plotrect.Height()/58,strTextOut);
	strTextOut.Format(TEXT("%.3f"), minY);
	m_ysliceDC->TextOut(left+wide/2-25,top-15,strTextOut);
	strTextOut.Format(TEXT("%.3f"), maxY);
	m_ysliceDC->TextOut(left+wide/2-25,top+height+5,strTextOut);
	m_ysliceDC->SelectObject(pOldFont);

	CBrush *pOldBrush; 
	//画色块	
	for(i=1;i<=34;i++)
	{
		boost::shared_ptr<CBrush> pNewBrush= boost::make_shared<CBrush>();
		if(pNewBrush->CreateSolidBrush(RGB(land[(i-1)*4*7]*255,land[(i-1)*4*7+1]*255,land[(i-1)*4*7+2]*255)))
		{
			pOldBrush=m_ysliceDC->SelectObject(pNewBrush.get());
			m_ysliceDC->Rectangle(plotrect.Width()/48,plotrect.Height()/9+(i-1)*plotrect.Height()/56,plotrect.Width()/14,plotrect.Height()/7.6+(i-1)*plotrect.Height()/56);			
			m_ysliceDC->SelectObject(pOldBrush);
		}		
	}

	//画标尺
	m_ysliceDC->MoveTo (plotrect.Width()/14+2,plotrect.Height()/9+plotrect.Height()/2/56);
	m_ysliceDC->LineTo (plotrect.Width()/14+2,plotrect.Height()/9+33*plotrect.Height()/56+plotrect.Height()/2/56);

	for(i=1;i<=34;i++)
	{
		m_ysliceDC->MoveTo (plotrect.Width()/14+2,plotrect.Height()/9+(i-1)*plotrect.Height()/56+plotrect.Height()/2/56);
		if(i%2!=0)
			m_ysliceDC->LineTo (plotrect.Width()/14+7,plotrect.Height()/9+(i-1)*plotrect.Height()/56+plotrect.Height()/2/56);
		else
			m_ysliceDC->LineTo (plotrect.Width()/14+5,plotrect.Height()/9+(i-1)*plotrect.Height()/56+plotrect.Height()/2/56);
	}			  
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	boost::shared_ptr<CFont> pNewFont1= boost::make_shared<CFont>();
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

	m_ysliceDC->MoveTo (left +(m_depthThreshold-min)/(max-min)*wide,top);
	m_ysliceDC->LineTo (left +(m_depthThreshold-min)/(max-min)*wide,top+height);

	CPen newPen1;
	newPen1.CreatePen(BS_SOLID,1,RGB(0,0,255));//20,20,80));
	pOldPen=m_ysliceDC->SelectObject(&newPen1);

	float tmp;
	if(m_deepestShow)
	{
		tmp=(1.0-(float)YRangeU/Height)*height;
		m_ysliceDC->MoveTo (left ,top+tmp);
		m_ysliceDC->LineTo (left +wide,top+tmp);

		tmp=(1.0-(float)YRangeD/Height)*height;
		m_ysliceDC->MoveTo (left ,top+tmp);
		m_ysliceDC->LineTo (left +wide,top+tmp);
	}
	m_ysliceDC->SelectObject(pOldPen);

	if(m_bShowSlice)
	{ 
		int Xnum,Ynum;

		CPen pNewPen;      
		pNewPen.CreatePen (PS_SOLID,2,RGB(0,255,0));
		pOldPen=m_ysliceDC->SelectObject (&pNewPen);

		tmp=(float)(PointerPosX-SurRect.left)/SurRect.Width();
		if (tmp<0)
			tmp=0;
		if(tmp>1)
			tmp=1;

		Xnum=tmp*(Width-1); 

		if(m_deepestShow)
		{
			Xnum=m_pitsMark[CurPitNum*PMN+1];
		}

		if (Xnum>0 && Xnum<=Width)
		{
			//			if(type==OBJECT_PLANE)
			//			{
			m_ysliceDC->MoveTo(left + (m_ptrVertexList[Xnum].z-min)/(max-min)*wide,top);	
			for(int j=1;j<Height-1;j++)
			{              
				int		X=left + (m_ptrVertexList[((Height-1-j)*Width+Xnum)].z-min)/(max-min)*wide;//Buffer[(j*(Width-1)+Xnum)*3+2]-min)/(max-min)*wide;
				int     Y=top+j*height/(Height-2);
				if(j>1)
				{		 
					m_ysliceDC->LineTo (X,Y);
				}
			}


			//		_gcvt( Buffer[Xnum*3+2], 3, Cbuffer );
			//		m_ysliceDC->TextOut(left + (Buffer[((Height-1-Ynum)*Width+Xnum)*3+2]-min)/(max-min)*wide,top+Ynum*height/(Height-2),Cbuffer);


			//			}
			//else if(type==OBJECT_CYLINDER)
			//{
			//	m_ysliceDC->MoveTo(left + (Colour[Xnum+2]-min)/(max-min)*wide,top);	
			//	for(int j=1;j<Height-1;j++)
			//	{              
			//		int		X=left + (Colour[(Height-1-j)*Width+Xnum+2]-min)/(max-min)*wide;//Buffer[(j*(Width-1)+Xnum)*3+2]-min)/(max-min)*wide;
			//		int     Y=top+j*height/(Height-2);
			//		if(j>1)
			//		{		 
			//			m_ysliceDC->LineTo (X,Y);
			//		}

			//	}
			//}
		}		
		m_ysliceDC->SelectObject (pOldPen);
	}

}

void CDlgHeightValue::DrawXSlice()
{
	int i;	
	pWnd=GetDlgItem(IDC_XSLICE);
	CRect plotrect;
	pWnd->GetClientRect(&plotrect);
	m_xsliceDC=pWnd->GetDC();
	///////////////////////////////////////////////////////////////////////////////////////////////////
	//注意如果画的次数太多会出错，看情况可以注释掉
	m_xsliceDC->TextOut(plotrect.left+20,plotrect.top+plotrect.Height()/2-10,TEXT("X Slice"));
	///////////////////////////////////////////////////////////////////////////////////////////////////

	/*	int		left=plotrect.left +plotrect.Width ()*0.2;
	int		top=plotrect.top+plotrect.Height ()*0.1 ;
	int		wide=plotrect.Width ()*0.7;
	int		height=plotrect.Height ()*0.8;*/

	int		left=plotrect.left +plotrect.Width ()*0.1;
	int		wide=plotrect.Width ()*0.7;

	int		top=plotrect.top+plotrect.Height ()*0.1 ;
	int		height=plotrect.Height ()*0.8;

	//top=plotrect.top+plotrect.Height ()*0.1;
	//height=(max-min)/(maxX-minX)*wide;

	//////////////////////////////////////////////////////////////////////////////

	CFont* pOldFont;
	boost::shared_ptr<CFont> pNewFont1= boost::make_shared<CFont>();
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

	boost::shared_ptr<CFont> pNewFont2= boost::make_shared<CFont>();
	pNewFont2->CreateFont(
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

	pOldFont=(CFont*)m_xsliceDC->SelectObject(pNewFont2.get());

	strTextOut.Format(TEXT("%.3f"), minX);
	m_xsliceDC->TextOut(left+wide+15,top+height/2-20,strTextOut);

	strTextOut.Format(TEXT("%.3f"), maxX);
	m_xsliceDC->TextOut(left-5,top+height/2-20,strTextOut);

	m_xsliceDC->SelectObject(pOldFont);
	//////////////////////////////////////////////////////////////////////////////////


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
	m_xsliceDC->MoveTo (left,top +height- (m_depthThreshold-min)/(max-min)*height);
	m_xsliceDC->LineTo (left+wide,top +height- (m_depthThreshold-min)/(max-min)*height);


	CPen newPen1;
	newPen1.CreatePen(BS_SOLID,1,RGB(0,0,255));
	pOldPen=m_xsliceDC->SelectObject(&newPen1);

	if(m_deepestShow)
	{
		float  tmp=(float)XRangeL/Width*wide;
		m_xsliceDC->MoveTo (left+tmp ,top);
		m_xsliceDC->LineTo (left+tmp,top+height);

		tmp=(float)XRangeR/Width*wide;
		m_xsliceDC->MoveTo (left+tmp ,top);
		m_xsliceDC->LineTo (left+tmp,top+height);
	}
	m_ysliceDC->SelectObject(pOldPen);

	if(m_bShowSlice)
	{ 
		int Ynum;

		CPen pNewPen;      
		pNewPen.CreatePen (PS_SOLID,2,RGB(255,0,0));
		pOldPen=m_xsliceDC->SelectObject (&pNewPen);

		float tmp=(float)(PointerPosY-SurRect.top)/SurRect.Height();
		if (tmp<0)
			tmp=0;
		if(tmp>1)
			tmp=1;

		Ynum=tmp*(Height-1); 

		if(m_deepestShow)
		{
			Ynum=Height-1-m_pitsMark[CurPitNum*PMN+2]/*MaxPosY*/;
		}
		if(Ynum>0 && Ynum<=Height)
		{
			//				if(type==OBJECT_PLANE)
			//				{
			m_xsliceDC->MoveTo(left,top +height- (m_ptrVertexList[(Height-1-Ynum)*Width].z-min)/(max-min)*height);		   		
			for(int j=1;j<Width-1;j++)
			{

				int		X=left + j*wide/(Width-2);
				int     Y=top +height- (m_ptrVertexList[((Height-1-Ynum)*Width+j)].z-min)/(max-min)*height;
				if(j>1)
				{		 
					m_xsliceDC->LineTo (X,Y);
				}

			}
			//				}
			//else if(type==OBJECT_CYLINDER)
			//{
			//	m_xsliceDC->MoveTo(left,top +height- (Colour[(Height-Ynum)*Width+2]-min)/(max-min)*height);		   		
			//	for(int j=1;j<Width-1;j++)
			//	{

			//		int		X=left + j*wide/(Width-2);
			//		int     Y=top +height- (Colour[(Height-1-Ynum)*Width+j+2]-min)/(max-min)*height;
			//		if(j>1)
			//		{		 
			//			m_xsliceDC->LineTo (X,Y);
			//		}

			//	}
			//}
		}

		m_xsliceDC->SelectObject (pOldPen);
	}
}

void CDlgHeightValue::OnBnClickedDeepest()
{
	// TODO: Add your control notification handler code here
	m_deepestShow = TRUE;
	m_Orientation.m_Angle=0;
	m_Orientation.m_Axis[0]=0, m_Orientation.m_Axis[1]=0, m_Orientation.m_Axis[2]=1;				


	float tmp;
	if(NextPitShowFlag==0)
		CurPitNum=0;

	tmp=(float)m_pitsMark[CurPitNum*PMN+1]/(Width-1);
	PointerPosX=SurRect.left +tmp*SurRect.Width();
	tmp=1-(float)m_pitsMark[CurPitNum*PMN+2]/(Height-1);		
	PointerPosY=SurRect.top+tmp*SurRect.Height();	
	m_LeftButtonDown = TRUE ;
	m_bShowSlice= TRUE;

	YRangeU=0;
	YRangeD=0;
	XRangeL=0;
	XRangeR=0;

	MaxPosY=m_pitsMark[CurPitNum*PMN+2];
	MaxPosX=m_pitsMark[CurPitNum*PMN+1];

	int t;
	for(t=MaxPosY;t<Height-1;t++)
	{	
		//	if(YRangeD==0 && m_ptrVertexList[(t*Width+MaxPosX)].z>0 && m_ptrVertexList[((t+1)*Width+MaxPosX)].z< 0)
		if(YRangeD==0 && m_ptrVertexList[(t*Width+MaxPosX)].z>m_depthThreshold && m_ptrVertexList[((t+1)*Width+MaxPosX)].z< m_depthThreshold)
			YRangeD=t;
	}		

	for(t=MaxPosY;t>0;t--)
	{	
		//	if(YRangeU==0 && m_ptrVertexList[(t*Width+MaxPosX)].z>0 && m_ptrVertexList[((t-1)*Width+MaxPosX)].z<0)
		if(YRangeU==0 && m_ptrVertexList[(t*Width+MaxPosX)].z>m_depthThreshold && m_ptrVertexList[((t-1)*Width+MaxPosX)].z<m_depthThreshold)
			YRangeU=t;
	}		


	for(t=MaxPosX;t<Width-1;t++)
	{
		//if(XRangeR==0 && m_ptrVertexList[(MaxPosY*Width+t)].z>0 && m_ptrVertexList[(MaxPosY*Width+t+1)].z<0) 
		if(XRangeR==0 && m_ptrVertexList[(MaxPosY*Width+t)].z>m_depthThreshold && m_ptrVertexList[(MaxPosY*Width+t+1)].z<m_depthThreshold) 
			XRangeR=t;
	}
	for(t=MaxPosX;t>0;t--)
	{
		//if(XRangeL==0 && m_ptrVertexList[(MaxPosY*Width+t)].z>0 && m_ptrVertexList[(MaxPosY*Width+t-1)].z<0) 
		if(XRangeL==0 && m_ptrVertexList[(MaxPosY*Width+t)].z>m_depthThreshold && m_ptrVertexList[(MaxPosY*Width+t-1)].z<m_depthThreshold) 
			XRangeL=t;
	}

	RefreshAll();
	DataDis();
}

void CDlgHeightValue::OnBnClickedSurshow()
{
	// TODO: Add your control notification handler code here

	pWnd=GetDlgItem(IDC_SURFACESHOW);
	m_ppDC=pWnd->GetDC();
	wglMakeCurrent(m_ppDC->GetSafeHdc(), hrc);
	DrawSurface();
	m_LeftButtonDown=FALSE;
	m_deepestShow = FALSE;
	m_bShowSlice= FALSE;
}

void CDlgHeightValue::OnBnClickedZoomin()
{
	// TODO: Add your control notification handler code here
	m_ScalFactor=m_ScalFactor*1.1;
	MapZoom();
}

void CDlgHeightValue::OnBnClickedZoomout()
{
	// TODO: Add your control notification handler code here
	m_ScalFactor=m_ScalFactor/1.1;
	MapZoom();
}

void CDlgHeightValue::MapZoom()
{
	pWnd=GetDlgItem(IDC_SURFACESHOW);
	m_ppDC=pWnd->GetDC();
	wglMakeCurrent(m_ppDC->GetSafeHdc(), hrc);
	DrawSurface();

	GetSurRect();

	pWnd=GetDlgItem(IDC_YSLICE);
	m_ysliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	DrawYSlice();

	pWnd=GetDlgItem(IDC_XSLICE);
	m_xsliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow(); 
	DrawXSlice();
}


void CDlgHeightValue::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	CString tmpstr;
	m_setflag = TRUE;
	m_depthThreshold=_ttof( tmpstr);
	m_Slider1.SetPos(maxRange*(1-(m_depthThreshold-min)/(max-min)));
	/////////////////////////////////////////////////////////////////////////////////
	MarkSurface();
	CurPitNum=0;
	OnBnClickedDeepest();
	/////////////////////////////////////////////////////////////////////////////////	
	Invalidate();
	m_setflag = FALSE;
}

void CDlgHeightValue::DrawHistogram()
{
	int       i, lMaxValue,lStartY;
	CRect rcClient;
	lStartY = HisRec.bottom - HisRec.top - 20;
	// caculate drawing rect within frame window
	rcClient.left   = 2;
	rcClient.top    = 2;
	rcClient.right  = HisRec.right  - HisRec.left - 2;
	rcClient.bottom = HisRec.bottom - HisRec.top  - 19;

	int Histo[256];

	int tmp;
	for (i=0;i<255;i++)
		Histo[i]=0;

	for (int t = 1; t < Height-1; t++)
	{
		for (int t2 = 1; t2 < Width-1; t2++)
		{
			tmp=(int)((m_ptrVertexList[(t*Width+t2)].z-min)/(max-min)*255);
			if(tmp>255)
				tmp=255;
			if(tmp<0)
				tmp=0;
			Histo[tmp]++;
		}
	}
	// normalize histogram for drawing
	lMaxValue = 1;
	for (i=0; i<255; i++)
	{
		if(Histo[i]>lMaxValue)
			lMaxValue = Histo[i];
	}
	for (i=0; i<255; i++)//划定坐标位置
	{
		Histo[i] = Histo[i] * (rcClient.bottom) / lMaxValue;
	}

	// overdraw old histogram
	m_surfacehisDC->FillRect(&rcClient, CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));
	// draw new histogram
	for (i=0; i<255; i++)
	{
		m_surfacehisDC->MoveTo ( i, lStartY);
		m_surfacehisDC->LineTo ( i, lStartY - Histo[i] );
	}

	CPen newPen;
	newPen.CreatePen(BS_SOLID,2,RGB(255,0,0));
	pOldPen=m_surfacehisDC->SelectObject(&newPen);
	m_surfacehisDC->MoveTo (2,lStartY);
	m_surfacehisDC->LineTo (2,lStartY-2);
	m_surfacehisDC->MoveTo (2+rcClient.right/2,lStartY);
	m_surfacehisDC->LineTo (2+rcClient.right/2,lStartY-2);	
	m_surfacehisDC->MoveTo (rcClient.right,lStartY);
	m_surfacehisDC->LineTo (rcClient.right,lStartY-2);
	m_surfacehisDC->MoveTo (2+(m_depthThreshold-min)/(max-min)*rcClient.Width(),lStartY);
	m_surfacehisDC->LineTo (2+(m_depthThreshold-min)/(max-min)*rcClient.Width(),lStartY-rcClient.Height());

	m_surfacehisDC->SelectObject(pOldPen);

	boost::shared_ptr<CFont> pNewFont= boost::make_shared<CFont>();
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

	CFont *pOldFont=(CFont*)m_surfacehisDC->SelectObject(pNewFont.get());
	m_surfacehisDC->SetBkColor(RGB(236,233,216));
	CHAR Cbuffer[50];

	CString strTextOut;
	strTextOut.Format(TEXT("%.3f"), min);
	m_surfacehisDC->TextOut(2,lStartY+5,strTextOut);
	strTextOut.Format(TEXT("%.3f"), (min+max)/2.0);
	m_surfacehisDC->TextOut(2+rcClient.right/2-20,lStartY+5,strTextOut);
	strTextOut.Format(TEXT("%.3f"), max);
	m_surfacehisDC->TextOut(rcClient.right-40,lStartY+5,strTextOut);	
	m_surfacehisDC->SelectObject(pOldFont);
}

long CDlgHeightValue::MarkSurface()
{
	////////////////////////////
	LPBYTE m_lpBMPFileData;
	BITMAPINFOHEADER m_BmpInfoHeader;

	long m_lngWidth;            //图像宽
	long m_lngWidthBytes;       //图像宽所占字节数 
	long m_lngHeight;           //图象高
	long m_ulngBitsCount;       //图像数据所占bit数
	LPBYTE m_lpImgBits;       //图像数据区域指针
	LPBYTE m_lpImgBitsMove;


	int * m_lpnMarkMove;       //标记数组移动指针
	int m_nMarkNumbers;       //标记的区域个数
	BYTE bObjectGray;

	bObjectGray=255;

	m_lpImgBits=NULL;
	m_lpBMPFileData=NULL;


	m_lngHeight=Height;
	m_lngWidth=Width;
	m_lngWidthBytes = (m_lngWidth+3) & 0xfffffffc;

	//***********加上4列像素使得算法在下列增加三个白色像素情况下不影响原始图像的判断******
	m_lngWidthBytes =m_lngWidthBytes+4;
	//************************************************************************************

	m_ulngBitsCount = m_lngWidthBytes*m_lngHeight;
	m_lpImgBits=new BYTE[m_ulngBitsCount];

	for(i=0;i<m_lngHeight;i++)
	{
		for(j=0;j<Width;j++)
		{
			if ( m_ptrVertexList[(i*Width+j)].z >=m_depthThreshold)//0.00003)//
				m_lpImgBits[i*m_lngWidthBytes+j]=255;
			else
				m_lpImgBits[i*m_lngWidthBytes+j]=0;
		}
	}
	//**************************赋三个像素的值为1可以使得算法在任何情况下均可找出连通域，
	//原来算法在markregion形状比较规则时有时会失效，人为补上一个region，算法就可以找出这些规则形状的连通域***
	m_lpImgBits[m_lngWidthBytes-1]=255;
	m_lpImgBits[m_lngWidthBytes+m_lngWidthBytes-2]=255;
	m_lpImgBits[m_lngWidthBytes+m_lngWidthBytes-3]=255;
	//************************************************************************************

	//bObjectGray为目标的灰度值，文本可以设为255
	/* 申请标记数组空间，记得要在CImage类的构造函数
	中将m_lpnMark初始化为NULL 。*/
	boost::scoped_array<int> m_lpnMark(new int[m_ulngBitsCount]);            //标记数组指针;
	ASSERT( m_lpnMark != NULL );
	m_lpnMarkMove=m_lpnMark.get();
	::memset((LPBYTE)m_lpnMark.get(),0,m_ulngBitsCount*4); 

	int nMarkValue=1;           
	/* 每次标识的值，nMarkValue会在后边递增，
	来表示不同的区域，从1开始标记。 */
	int nMaxMarkValue=0;     //记录最大的标识的值
	int i,j;                 //循环控制变量

	/* 定义存放等价对的链表，其元素是 EqualMark类型，
	定义list是为了节约存储空间。要使用Clist，
	应该#include <Afxtempl.h>。  */
	CList < EqualMark,EqualMark > lEqualMark;

	//初始化图像移动指针
	m_lpImgBitsMove = m_lpImgBits;

	/*进行第一次扫描，将所得的等价对(EqualMark类型)加到lEqualMark链表中。
	使用nMarkValue来进行每一次新的标记，标记之后将其值加1。
	由于版面关系，这部分代码也同样略去不写。作者提出以下几点编程时要注意
	的地方。
	Note1：图像的四周像素并不会有8个相邻的像素。这时就要根据上、下、左、
	右四种不同的情况做不同的寻找等价对的判断。
	Note2：可以先对等价对进行排序，每次都保证MarkValue1<MarkValue2，
	这样易于管理等价对。
	Note3：在实际工作中，连续寻找出的等价对很容易重复，将本次找出的等价对
	和链表中保存的最后一个等价对相比较，如果不相等的话再存入等价对链表，
	这样可以大大降低链表中等价对的重复。
	Note4：第一次扫描之后，nMarkValue-1即为nMaxMarkValue。 */

	/************************************************************************/
	//下面为补充代码,完成对图象的第一次扫描

	//初始化图象数组和标识数组的指针
	int nEqualNum=0;
	EqualMark tempEqualMark;    //用以暂时存放每次找到的等价关系
	m_lpnMarkMove=m_lpnMark.get();
	m_lpImgBitsMove = m_lpImgBits;


	//标记图象的第一行、第一列的象素(只有这一个象素)
	if ( *m_lpImgBitsMove==bObjectGray )
	{
		*m_lpnMarkMove=nMarkValue++;
	}
	m_lpnMarkMove++;
	m_lpImgBitsMove++;

	//标记图象的第一行，此时不会出现等价的情况
	for ( i=1; i < m_lngWidthBytes; i++)
	{
		//需要标记的情况
		if ( *m_lpImgBitsMove==bObjectGray )
		{	
			//前面没有被标记过，则开始一个新的标记
			if ( *(m_lpnMarkMove-1)==0 ) 
			{
				*m_lpnMarkMove=nMarkValue++;
			}
			//前面被标记过，则跟随前一个标记
			else
			{
				*m_lpnMarkMove=*(m_lpnMarkMove-1);
			}
		}
		m_lpnMarkMove++;
		m_lpImgBitsMove++;
	}

	//除第一行之外的标记，此时会出现等价的关系
	for ( j=1; j < m_lngHeight; j++ )
	{
		m_lpImgBitsMove=m_lpImgBits+j*m_lngWidthBytes;
		m_lpnMarkMove=m_lpnMark.get()+j*m_lngWidthBytes;

		//对每行的第一个点做处理，总体就是对图象的最左列做处理
		//只需要检视上，右上两个点 
		if ( *m_lpImgBitsMove==bObjectGray )
		{	
			//<上>位置被标记过
			if ( *(m_lpnMarkMove-m_lngWidthBytes)!=0 ) 
			{	
				//跟随<上>标记
				*m_lpnMarkMove=*(m_lpnMarkMove-m_lngWidthBytes);
				if ( *(m_lpnMarkMove-m_lngWidthBytes)!=*(m_lpnMarkMove-m_lngWidthBytes+1) && *(m_lpnMarkMove-m_lngWidthBytes+1)!=0)
				{
					//<上><右上>等价标记
					AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-m_lngWidthBytes),
						*(m_lpnMarkMove-m_lngWidthBytes+1),nEqualNum,lEqualMark);
				}

			}
			//<上>没有标记，此时一定不会存在等价关系
			else 
			{
				if ( *(m_lpnMarkMove-m_lngWidthBytes+1)!=0 )	
				{
					*m_lpnMarkMove=*(m_lpnMarkMove-m_lngWidthBytes+1);   //跟随<右上>标记
				}
				//<上>、<右上>都没有标记，则开始新的标记
				else
				{
					*m_lpnMarkMove=nMarkValue++;
				}
			}
		}
		m_lpnMarkMove++;
		m_lpImgBitsMove++;

		//对每行的中间点做标记处理，此时存在<左>、<左下>、<下>、<右下> 4种情况
		for ( i=1; i<=m_lngWidthBytes-1; i++ )
		{	
			//需要标记
			if ( (*m_lpImgBitsMove)==bObjectGray )
			{	
				//<左>被标记过
				if ( *(m_lpnMarkMove-1)!=0 )
				{
					*m_lpnMarkMove=*(m_lpnMarkMove-1);          //跟随<左>

					if ( *(m_lpnMarkMove-1)!=*(m_lpnMarkMove-m_lngWidthBytes-1) && *(m_lpnMarkMove-m_lngWidthBytes-1)!=0 )
					{
						//标记<左>、<左下>等价
						AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-1),
							*(m_lpnMarkMove-m_lngWidthBytes-1),nEqualNum,lEqualMark);
					}

					if ( *(m_lpnMarkMove-1)!=*(m_lpnMarkMove-m_lngWidthBytes) && *(m_lpnMarkMove-m_lngWidthBytes)!=0)
					{
						//标记<左>、<下>等价
						AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-1),
							*(m_lpnMarkMove-m_lngWidthBytes),nEqualNum,lEqualMark);
					}

					if ( *(m_lpnMarkMove-1)!=*(m_lpnMarkMove-m_lngWidthBytes+1) && *(m_lpnMarkMove-m_lngWidthBytes+1)!=0)
					{
						//标记<左>、<右下>等价
						AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-1),
							*(m_lpnMarkMove-m_lngWidthBytes+1),nEqualNum,lEqualMark);
					}
				}
				//<左>未被标记过
				else
				{
					//<左下>被标记过
					if ( *(m_lpnMarkMove-m_lngWidthBytes-1)!=0 )
					{
						*m_lpnMarkMove=*(m_lpnMarkMove-m_lngWidthBytes-1);

						if ( *(m_lpnMarkMove-m_lngWidthBytes-1)!=*(m_lpnMarkMove-m_lngWidthBytes) && *(m_lpnMarkMove-m_lngWidthBytes)!=0)
						{
							//标记<左下>、<下>等价
							AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-m_lngWidthBytes-1),
								*(m_lpnMarkMove-m_lngWidthBytes),nEqualNum,lEqualMark);
						}

						if ( *(m_lpnMarkMove-m_lngWidthBytes-1)!=*(m_lpnMarkMove-m_lngWidthBytes+1) && *(m_lpnMarkMove-m_lngWidthBytes+1)!=0)
						{
							//标记<左下>、<右下>等价
							AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-m_lngWidthBytes-1),
								*(m_lpnMarkMove-m_lngWidthBytes+1),nEqualNum,lEqualMark);
						}


					}
					//<左>、<左下>未标记过
					else 
					{
						if ( *(m_lpnMarkMove-m_lngWidthBytes)!=0 )
						{
							*m_lpnMarkMove=*(m_lpnMarkMove-m_lngWidthBytes);        	//跟随<下>标记

							if ( *(m_lpnMarkMove-m_lngWidthBytes)!=*(m_lpnMarkMove-m_lngWidthBytes+1) && *(m_lpnMarkMove-m_lngWidthBytes+1)!=0 )
							{
								//标记<下>和<右下>等价
								AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-m_lngWidthBytes),
									*(m_lpnMarkMove-m_lngWidthBytes+1),nEqualNum,lEqualMark);
							}
						}
						//<左>、<左下>、<下>未标记过，此时不存在等价关系
						else
						{
							if (*(m_lpnMarkMove-m_lngWidthBytes+1)!=0)
							{
								*m_lpnMarkMove=*(m_lpnMarkMove-m_lngWidthBytes+1);    	//跟随<右下>标记
							}
							//<左>、<左下>、<下>、<右下>未标记过，则开始新的标记值
							else
							{
								*m_lpnMarkMove=nMarkValue++;
							}

						}    //<左>、<左下>、<下>未标记过结束
					}   //<左>、<左下>未标记过结束
				}  //<左>未被标记过结束
			}     // else 不需要标记

			m_lpnMarkMove++;
			m_lpImgBitsMove++;
		}       //中间点处理的结束

		//对每行的最后一个点做处理，总体就是对图象的最左列做处理
		//此时存在<左>、<左上>、<上> 3种情况

		//需要标记
		if ( (*m_lpImgBitsMove)==bObjectGray )
		{	
			//<左>被标记过
			if ( *(m_lpnMarkMove-1)!=0 )
			{
				*m_lpnMarkMove=*(m_lpnMarkMove-1);

				if ( *(m_lpnMarkMove-1)!=*(m_lpnMarkMove-m_lngWidthBytes-1) && *(m_lpnMarkMove-m_lngWidthBytes-1)!=0)
				{
					//标记<左>、<左上>等价
					AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-1),
						*(m_lpnMarkMove-m_lngWidthBytes-1),nEqualNum,lEqualMark);
				}

				if ( *(m_lpnMarkMove-1)!=*(m_lpnMarkMove-m_lngWidthBytes) && *(m_lpnMarkMove-m_lngWidthBytes)!=0)
				{
					//标记<左>、<上>等价
					AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-1),
						*(m_lpnMarkMove-m_lngWidthBytes),nEqualNum,lEqualMark);
				}

			}
			//<左>未被标记过
			else
			{
				if ( *(m_lpnMarkMove-m_lngWidthBytes-1)!=0 )
				{
					*m_lpnMarkMove=*(m_lpnMarkMove-m_lngWidthBytes-1);    //跟随<左上>

					if ( *(m_lpnMarkMove-m_lngWidthBytes-1)!=*(m_lpnMarkMove-m_lngWidthBytes) && *(m_lpnMarkMove-m_lngWidthBytes)!=0)
					{
						//标记<左上>、<上>等价
						AttachEqualMark(tempEqualMark,*(m_lpnMarkMove-m_lngWidthBytes-1),
							*(m_lpnMarkMove-m_lngWidthBytes),nEqualNum,lEqualMark);
					}

				}
				//<左>、<左上>未标记过
				else 
				{
					if ( *(m_lpnMarkMove-m_lngWidthBytes)!=0 )
					{
						*m_lpnMarkMove=*(m_lpnMarkMove-m_lngWidthBytes);   	//跟随<上>标记
					}
					//<左>、<左上>、<上>未标记过,则开始新的标记值
					else
					{
						*m_lpnMarkMove=nMarkValue++;
					}

				}
			}
		}  //对每行的最后一个点做处理，总体就是对图象的最左列做处理

	}     //"除第一行之外的标记"的结束

	//因为在每次标记完之后，nMarkValue都会自动++
	//所以要通过（-1）操作来记录所标记的最大的个数

	nMaxMarkValue=nMarkValue-1;
	/************************************************************************/
	/* 定义双层链表的外层链表，它的元素是一个指向内层链表的指针。
	内层链表的型别也是CptrList，其元素是标记值。 */
	CPtrList exList;
	CPtrList * pInnerList;
	POSITION posExElem;	

	if ( lEqualMark.GetCount() !=0 )
	{
		// pInnerListAdd，每次向exList中添加的新元素
		CPtrList * pInnerListAdd=new CPtrList;     
		ASSERT ( pInnerListAdd != NULL );

		/* 添加第一个等价对到exList的第一个元素所指向的InnerList中。  */
		pInnerListAdd->AddTail(	(void *)lEqualMark.GetHead().MarkValue1);
		pInnerListAdd->AddTail(	(void *)lEqualMark.GetHead().MarkValue2);
		exList.AddTail( (void *)pInnerListAdd );
		lEqualMark.RemoveHead();

		/* 定义pFindValue1和pFindValue2， 存放在所有内层链表中找到特定值
		的某个内层链表的头指针，也就是外层链表的某个元素值。 */
		CPtrList * pFindValue1=NULL;
		CPtrList * pFindValue2=NULL;

		//整理剩余的等价对
		while ( !lEqualMark.IsEmpty() )
		{	
			posExElem=exList.GetHeadPosition();
			pFindValue1=NULL;
			pFindValue2=NULL;

			while ( posExElem )
			{
				pInnerList=(CPtrList *)exList.GetAt(posExElem);
				if ( pInnerList->Find( (void *)lEqualMark.GetHead().MarkValue1) )
				{	
					pFindValue1=pInnerList;
				}
				if( pInnerList->Find( (void *)lEqualMark.GetHead().MarkValue2) ) 
				{	
					pFindValue2=pInnerList;
				}
				exList.GetNext(posExElem);
			}

			//该等价对中两个值都在已经整理过的等价关系中
			if ( pFindValue1 && pFindValue2 )
			{	
				//当两个地址不一样时，对链表进行调整
				if ( pFindValue1!=pFindValue2 )
				{		
					pFindValue1->AddTail(pFindValue2);
					/* 清除链表元素，通过new得到的CptrList 类型，
					必须采用delete进行删除，否则会造成内存泄露。*/
					POSITION posDelete = exList.Find((void *)pFindValue2);
					pFindValue2->RemoveAll();
					delete pFindValue2;
					exList.RemoveAt( posDelete );
				}
			}
			/* 只在已经整理过的等价关系中找到Value1，
			那么将Vaule2加到Value1所在的链表中。 */
			else if ( pFindValue1 )
			{
				pFindValue1->AddTail((void *)lEqualMark.GetHead().MarkValue2 );
			}
			else if ( pFindValue2)
			{
				pFindValue2->AddTail( (void *)lEqualMark.GetHead().MarkValue1 );
			}
			/* 等价对中两个值在整理过的等价关系中都
			没有找到，则在exList中增加新元素。 */
			else
			{
				CPtrList * pInnerListAdd=new CPtrList;
				pInnerListAdd->AddTail( (void *)lEqualMark.GetHead().MarkValue1 );
				pInnerListAdd->AddTail( (void *)lEqualMark.GetHead().MarkValue2 );
				exList.AddTail((void *)pInnerListAdd);
			}			
			//去掉此时等价对的头元素
			lEqualMark.RemoveHead(); 
		}  // while ( !lEqualMark.IsEmpty() )循环结束
	}  // if ( lEqualMark.GetCount() !=0 )语句结束
	/* 等价对链表大小为0，说明第一次扫描之后没有产生等价对，标记已经完成。 */
	else
	{
		return TRUE; 
	}		

	/*等价关系整理完成，下面建立第一次扫描的标记值和
	第二次扫描的标记值之间的映射关系。*/

	int nTotalEqualNum=0; //列入等价关系的标记个数
	int nMarkRegion=0;   //图像中连通区域个数

	posExElem=exList.GetHeadPosition();
	while ( posExElem )
	{	
		pInnerList=(CPtrList *)exList.GetAt(posExElem);
		nTotalEqualNum += pInnerList->GetCount();
		exList.GetNext(posExElem);
	}
	nMarkRegion=nMaxMarkValue-nTotalEqualNum+exList.GetCount();

	/* 定义第一次扫描和第二次扫描之间的映射向量，要使用vector，
	应该#include <vector>并且使用std命名空间。 */
	vector<MarkMapping> vMarkMap(nMaxMarkValue); 

	//初始化映射向量，令其做自身映射
	for ( i=0;i<nMaxMarkValue;i++ )
	{	
		vMarkMap[i].nOriginalMark=i+1;
		vMarkMap[i].nMappingMark=i+1;
	}

	POSITION posInnerElem; //InnerList中元素的位置
	int nMin;              //InnerList中最小值
	int nIndex=0;

	posExElem=exList.GetHeadPosition();
	/* while循环实现了如下功能：找到每个等价组中最小的标记值，
	然后将映射向量中nMappingMark设定为其所在等价组的最小的标记值。*/
	while ( posExElem )
	{	
		pInnerList=(CPtrList *)exList.GetAt(posExElem);
		nMin=(int)pInnerList->GetHead();
		posInnerElem=pInnerList->GetHeadPosition();
		pInnerList->GetNext(posInnerElem);

		while ( posInnerElem )
		{
			if ( (int)pInnerList->GetAt(posInnerElem)<nMin )
			{	
				nMin=(int)pInnerList->GetAt(posInnerElem);
			}
			pInnerList->GetNext(posInnerElem);
		}

		/* 根据每组等价关系中的最小的标记值对Mapping向量做出调整。 */
		posInnerElem=pInnerList->GetHeadPosition();
		while ( posInnerElem )
		{
			nIndex=(int)pInnerList->GetAt(posInnerElem)-1;
			vMarkMap[ nIndex ].nMappingMark=nMin;
			pInnerList->GetNext(posInnerElem);
		}
		exList.GetNext(posExElem);
	}

	/* 将映射向量nMappingMark中不重复的部分找出并对其进行排序。
	使用find()和sort()这两种泛型算法，应该#include <algorithm>。*/
	vector <int> vSortMark(nMarkRegion); //排序向量
	nIndex=0;

	for ( i=0; i<nMaxMarkValue; i++ )
	{
		if  ( find( vSortMark.begin(),vSortMark.end(), vMarkMap[i].nMappingMark )
			==vSortMark.end() )
		{
			vSortMark[nIndex++]= vMarkMap[i].nMappingMark;
		}
	}
	sort ( vSortMark.begin(),vSortMark.end() );

	/* 根据排序后的标记在vSortMark向量中的位置，对映射向量做出重新调整。 */
	vector<int>::iterator itFind;
	vector<int>::iterator itBegin;
	itBegin=vSortMark.begin();

	for ( i=0;i<nMaxMarkValue;i++ )
	{
		itFind = find ( vSortMark.begin(),vSortMark.end(), vMarkMap[i].nMappingMark );
		vMarkMap[i].nMappingMark= ( itFind-itBegin + 1);
	}

	//根据映射向量对标记数组进行调整
	for ( j=0;j<m_lngHeight;j++ )
	{
		m_lpnMarkMove=m_lpnMark.get() + j*m_lngWidthBytes;
		for ( i=0;i<m_lngWidthBytes;i++ )
		{
			if ( *m_lpnMarkMove != 0 )
			{
				*m_lpnMarkMove = vMarkMap[ *m_lpnMarkMove-1 ].nMappingMark;
			}
			m_lpnMarkMove++;
		}
	}

	//删除链表结构中通过new得到的元素
	posExElem = exList.GetHeadPosition();
	while( posExElem )
	{
		pInnerList = (CPtrList *)exList.GetAt( posExElem );
		pInnerList->RemoveAll();
		delete pInnerList;
		exList.GetNext( posExElem );
	}
	exList.RemoveAll();


	//****************注意，由于自己多加了三个白像素即一个MarkRegion，因此实际的MarkRegion要少一个*******
	nMarkRegion--;
	//*******************************************************

	//通过类成员变量来记录连通区域的个数
	m_nMarkNumbers = nMarkRegion;


	PitsNum=nMarkRegion;
	//*******************************************************

	max=-100000;
	min=100000;
	MaxPosX=0;
	MaxPosY=0;

	if(nMarkRegion>=1)
	{				
		m_pitsMark.reset(new float[PMN*(nMarkRegion+2)+1]);		


		for(i=1;i<=PMN*(nMarkRegion+1)+1;i++)
		{
			m_pitsMark[i]=0;   
		}

		for(i=1;i<=nMarkRegion+1;i++)
		{
			m_pitsMark[(i-1)*PMN]=-1000000;
			m_pitsMark[(i-1)*PMN+5]=1;
			m_pitsMark[(i-1)*PMN+6]=-1000000;
			m_pitsMark[(i-1)*PMN+7]=1000000;
			m_pitsMark[(i-1)*PMN+8]=-1000000;
			m_pitsMark[(i-1)*PMN+9]=1000000;
		}
		float Vtmp=m_pixelSize*m_pixelSize*UMTOINCH*UMTOINCH;

		//PitsMark[...0]:local peak depth
		//PitsMark[...1]:local peak point x coordinate
		//PitsMark[...2]:local peak point y coordinate
		//PitsMark[...3]:local pit volume
		//PitsMark[...4]:local pit pixel num
		//PitsMark[...5]:pit property:1 normal pits, 2 scratches
		//PitsMark[...6]:max x coordinate
		//PitsMark[...7]:min x coordinate
		//PitsMark[...8]:max y coordinate
		//PitsMark[...9]:min y coordinate


		m_pointNum=0;
		for ( j=0;j<m_lngHeight;j++ )
		{	
			m_lpnMarkMove=m_lpnMark.get() + j*m_lngWidthBytes;	
			for (i=0;i<m_lngWidth;i++ )
			{						
				if(m_ptrVertexList[(j*m_lngWidth+i)].z <min)
					min=m_ptrVertexList[(j*m_lngWidth+i)].z;						

				if ( *m_lpnMarkMove != 0 )
				{						
					m_pointNum++;					
					//save local pit data
					if ( m_ptrVertexList[(j*m_lngWidth+i)].z>=m_pitsMark[(*m_lpnMarkMove-1)*PMN])
					{
						m_pitsMark[(*m_lpnMarkMove-1)*PMN]=m_ptrVertexList[(j*Width+i)].z;							
						m_pitsMark[(*m_lpnMarkMove-1)*PMN+1]=i;
						m_pitsMark[(*m_lpnMarkMove-1)*PMN+2]=j;	
					}
					//////////////////////////////////////////////////////////////////////////
					m_pitsMark[(*m_lpnMarkMove-1)*PMN+3]=m_pitsMark[(*m_lpnMarkMove-1)*PMN+3]+Vtmp*m_ptrVertexList[(j*Width+i)].z;	
					//////////////////////////////////////////////////////////////////////////
					m_pitsMark[(*m_lpnMarkMove-1)*PMN+4]=m_pitsMark[(*m_lpnMarkMove-1)*PMN+4]+1;

					if (i>=  m_pitsMark[(*m_lpnMarkMove-1)*PMN+6])							
						m_pitsMark[(*m_lpnMarkMove-1)*PMN+6]=i;
					if (i<  m_pitsMark[(*m_lpnMarkMove-1)*PMN+7])							
						m_pitsMark[(*m_lpnMarkMove-1)*PMN+7]=i;

					if (j>=  m_pitsMark[(*m_lpnMarkMove-1)*PMN+8])							
						m_pitsMark[(*m_lpnMarkMove-1)*PMN+8]=j;
					if (j<  m_pitsMark[(*m_lpnMarkMove-1)*PMN+9])							
						m_pitsMark[(*m_lpnMarkMove-1)*PMN+9]=j;
				}							
				m_lpnMarkMove++;
			}
		}


		int LFlag=1;
		int LCount=0;
		int LPitsPtNumLim=(m_diaLimit/(m_pixelSize*UMTOINCH*1000))*(m_diaLimit/(m_pixelSize*UMTOINCH*1000));
		while (LFlag==1&& LCount<nMarkRegion)
		{
			for(i=0;i<nMarkRegion;i++)
			{
				if (m_pitsMark[i*PMN]==-1000000)// || PitsMark[i*PMN+4]< LPitsPtNumLim)
				{
					for(j=i;j<nMarkRegion;j++)
					{
						for (int k=0;k<PMN;k++)
							m_pitsMark[j*PMN+k]=m_pitsMark[(j+1)*PMN+k];			
					}								
				}
			}
			LFlag=0;
			for(i=0;i<nMarkRegion;i++)
			{
				if (m_pitsMark[i*PMN]==-1000000)//|| PitsMark[i*PMN+4]< LPitsPtNumLim)
					LFlag=1;
			}
			LCount++;
		}


		float *TmpArr=new float[PMN*nMarkRegion+1];
		int *BFlag=new int[nMarkRegion];

		for(i=0;i<nMarkRegion;i++)
		{
			BFlag[i]=-1;
			for(j=0;j<PMN;j++)
				TmpArr[i*PMN+j]=m_pitsMark[i*PMN+j];
		}
		int BadArea=0;
		for(i=0;i<nMarkRegion;i++)
			if (m_pitsMark[i*PMN+4]< LPitsPtNumLim)
			{
				BFlag[i]=1;
				BadArea++;
			}

			PitsNum=nMarkRegion-BadArea;
			int tmp=0;
			for(i=0;i<nMarkRegion;i++)
			{
				if (BFlag[i]!=1)
				{ 
					for(j=0;j<PMN;j++)
						m_pitsMark[tmp*PMN+j]=TmpArr[i*PMN+j];
					tmp++;	
				}
			}
			delete BFlag; 
			BFlag=NULL;
			delete TmpArr;
			TmpArr=NULL;  

			double ftmp;
			m_scrNum=0;
			for(i=0;i<PitsNum;i++)
			{
				ftmp=sqrt((m_pitsMark[i*PMN+7]-m_pitsMark[i*PMN+6])*(m_pitsMark[i*PMN+7]-m_pitsMark[i*PMN+6])+(m_pitsMark[i*PMN+9]-m_pitsMark[i*PMN+8])*(m_pitsMark[i*PMN+9]-m_pitsMark[i*PMN+8]));
				int itmp=m_pitsMark[i*PMN+4];

				int itmp1=m_pitsMark[i*PMN+6];
				int itmp2=m_pitsMark[i*PMN+7];
				int itmp3=m_pitsMark[i*PMN+8];
				int itmp4=m_pitsMark[i*PMN+9];

				ftmp=m_pitsMark[i*PMN+4]/(3.1415926*ftmp*ftmp/4);
				if(ftmp<0.2)
				{
					m_pitsMark[i*PMN+5]=2;
					m_scrNum++;
				}
			}

			int x;
			int y;

			for(y=0;y<PitsNum-1;y++)
			{
				for(x=1;x<PitsNum-y;x++)
				{						
					if( m_pitsMark[x*PMN]>m_pitsMark[(x-1)*PMN])
					{
						float temp;
						for (int k=0;k<PMN;k++)
						{
							temp=m_pitsMark[(x-1)*PMN+k];
							m_pitsMark[(x-1)*PMN+k]=m_pitsMark[x*PMN+k];
							m_pitsMark[x*PMN+k]=temp;
						}				
					}
				}
			}


			max=m_pitsMark[0];
			MaxPosX=(int)m_pitsMark[1];
			MaxPosY=(int)m_pitsMark[2];

			////for(i=0;i<PitsNum*4;i++)
			////{
			////	tmp[i]=PitsMark[i];
			////}
			////i=i;
	}
	else
	{
		m_pitsMark.reset( new float[PMN]);
		//m_pitsMark=new float[PMN];

		max=-100000;
		min=100000;

		MaxPosX=0;
		MaxPosY=0;
		for(int t=0;t<Height;t++)
		{
			for(int t2=0;t2<Width;t2++)
			{
				//						if(type==OBJECT_PLANE)
				//						{
				if(m_ptrVertexList[(t*Width+t2)].z>max)
				{
					max=(float)m_ptrVertexList[(t*Width+t2)].z;
					MaxPosX=t2;
					MaxPosY=t;
				}
				if(m_ptrVertexList[(t*Width+t2)].z<min)
					min=(float)m_ptrVertexList[(t*Width+t2)].z;							
				//						}					
			}
		}
		m_pitsMark[0]=max;
		m_pitsMark[1]=(float)MaxPosX;
		m_pitsMark[2]=(float)MaxPosY;
		m_pitsMark[3]=0;
		m_pitsMark[4]=0;
	}
	/////////////////////////////////////////////////////////////////


	// 	if ( m_lpnMark )
	// 	{
	// 		delete[] m_lpnMark;
	// 		m_lpnMark = NULL;
	// 	}
	if ( m_lpImgBitsMove )
	{		
		m_lpImgBitsMove = NULL;
	}
	if ( m_lpImgBits )
	{
		delete[] m_lpImgBits;
		m_lpImgBits = NULL;
	}

	return (long)TRUE;
}

template<typename elemType> void CDlgHeightValue::AttachEqualMark(EqualMark &pEqualMark, 
	elemType num1, elemType num2, int & pEqualNum, CList< EqualMark,EqualMark > & plEqualMark)
{
	/*******************************************************
	作用：private 函数，将所选出的等价关系，attach到list上里
	参数1 pEqualMark   等价关系 
	参数2 num1         新的等价关系 1
	参数3 num2         新的等价关系 2
	参数4 nEqualNum    等价数组的个数
	参数5 plEqualMark  存放等价数组的list
	wholehope 2002.4  写
	修改记录： (1)  2002.5.23 原来所写会丢失掉部分等价关系，把逻辑改正
	判断不等 正确： ( pEqualMark.MarkValue1 != num1
	|| pEqualMark.MarkValue2 != num2 )
	错误： ( pEqualMark.MarkValue1 != num1
	&& pEqualMark.MarkValue2 != num2 )
	(2). 2002.6.20  加入到CImageAnalyze中
	********************************************************/

	//num1小的情况
	if ( num1 < num2 )
	{
		if ( pEqualMark.MarkValue1 != num1
			|| pEqualMark.MarkValue2 != num2 )
		{
			pEqualMark.MarkValue1=num1;
			pEqualMark.MarkValue2=num2;
			//插入到数组中
			pEqualNum++;
			plEqualMark.AddTail(pEqualMark);
		}
	}
	//num2小的情况
	else   
	{
		if ( pEqualMark.MarkValue2 != num1 
			|| pEqualMark.MarkValue1 != num2 )
		{
			pEqualMark.MarkValue1=num2;
			pEqualMark.MarkValue2=num1;
			//插入到数组中
			pEqualNum++;		
			plEqualMark.AddTail(pEqualMark);
		}
	}
}

void CDlgHeightValue::DataDis()
{

	//TCHAR Cbuffer[50];
	CString strTextOut;
	strTextOut.Empty();
	CRect CurRect;
	this->GetClientRect(CurRect);

	if (CurRect.Height()<800)
	{   
		int x,y,w,h;
		x=CurRect.right-150;
		y=70;
		w=83;
		h=20;

		(GetDlgItem(IDC_STATIC1))->MoveWindow(x-3,y-40,160,170,TRUE);
		(GetDlgItem(IDC_STATIC3))->MoveWindow(x,y-20,70,20,TRUE);
		(GetDlgItem(IDC_STATIC4))->MoveWindow(x+70,y-20,w,20,TRUE);

		(GetDlgItem(IDC_EDIT1))->MoveWindow(x,y,70,h,TRUE);
		(GetDlgItem(IDC_EDIT2))->MoveWindow(x,y+h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT16))->MoveWindow(x,y+2*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT3))->MoveWindow(x,y+3*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT4))->MoveWindow(x,y+4*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT5))->MoveWindow(x,y+5*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT6))->MoveWindow(x+70,y,w,h,TRUE);
		(GetDlgItem(IDC_EDIT7))->MoveWindow(x+70,y+h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT17))->MoveWindow(x+70,y+2*h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT8))->MoveWindow(x+70,y+3*h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT9))->MoveWindow(x+70,y+4*h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT10))->MoveWindow(x+70,y+5*h,w,h,TRUE);

		y=y+10;
		(GetDlgItem(IDC_STATIC22))->MoveWindow(x-3,y+6*h,160,170,TRUE);
		(GetDlgItem(IDC_STATIC5))->MoveWindow(x,y+7*h,70,20,TRUE);
		(GetDlgItem(IDC_STATIC6))->MoveWindow(x+70,y+7*h,w,20,TRUE);

		(GetDlgItem(IDC_EDIT111))->MoveWindow(x,y+8*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT116))->MoveWindow(x,y+9*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT112))->MoveWindow(x,y+10*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT113))->MoveWindow(x,y+11*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT114))->MoveWindow(x,y+12*h,70,h,TRUE);
		(GetDlgItem(IDC_EDIT115))->MoveWindow(x,y+13*h,70,h,TRUE);

		(GetDlgItem(IDC_EDIT121))->MoveWindow(x+70,y+8*h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT126))->MoveWindow(x+70,y+9*h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT122))->MoveWindow(x+70,y+10*h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT123))->MoveWindow(x+70,y+11*h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT124))->MoveWindow(x+70,y+12*h,w,h,TRUE);
		(GetDlgItem(IDC_EDIT125))->MoveWindow(x+70,y+13*h,w,h,TRUE);

		(GetDlgItem(IDC_BUTTON5))->MoveWindow(x+25,y+15*h,w+20,30,TRUE);
	}

	(GetDlgItem(IDC_EDIT1))->SetWindowText(TEXT("P-Depth"));
	(GetDlgItem(IDC_EDIT2))->SetWindowText(TEXT("PitNumber"));
	(GetDlgItem(IDC_EDIT16))->SetWindowText(TEXT("ScrNumber"));
	(GetDlgItem(IDC_EDIT3))->SetWindowText(TEXT("Defects/Area"));
	(GetDlgItem(IDC_EDIT4))->SetWindowText(TEXT("Coverage"));
	(GetDlgItem(IDC_EDIT5))->SetWindowText(TEXT("DepthLimit"));

	CString Stmp;
	Stmp.Format(TEXT(" %.5f"),max);
	(GetDlgItem(IDC_EDIT6))->SetWindowText(Stmp);

	// _gcvt( max, 6, Cbuffer );
	//(GetDlgItem(IDC_EDIT6))->SetWindowText(Cbuffer);

	if (PitsNum!=0)
		strTextOut.Format(TEXT("%.6d"), PitsNum-m_scrNum);
	else
	{
		strTextOut.Format(TEXT("%.6d"), PitsNum);
	}
	(GetDlgItem(IDC_EDIT7))->SetWindowText(strTextOut);

	strTextOut.Format(TEXT("%.6d"), m_scrNum);
	//_gcvt( ScrNum, 6, Cbuffer );
	(GetDlgItem(IDC_EDIT17))->SetWindowText(strTextOut);

	strTextOut.Format(TEXT(" %.1f"),PitsNum/(maxX-minX)/(maxY-minY));
	(GetDlgItem(IDC_EDIT8))->SetWindowText(strTextOut);

	//_gcvt( PitsNum/(maxX-minX)/(maxY-minY), 6, Cbuffer );
	//(GetDlgItem(IDC_EDIT8))->SetWindowText(Cbuffer);

	int j=(Width*Height);
	float tmp=(float)(m_pointNum)/(float)(j)*100.0;
	strTextOut.Format(TEXT("%.6f"), tmp);
	strTextOut.Append(TEXT("%"));
	(GetDlgItem(IDC_EDIT9))->SetWindowText(strTextOut);

	strTextOut.Format(TEXT("%.6f"), m_depthThreshold);
	(GetDlgItem(IDC_EDIT10))->SetWindowText(strTextOut);

	(GetDlgItem(IDC_EDIT111))->SetWindowText(TEXT("Serial No."));
	(GetDlgItem(IDC_EDIT116))->SetWindowText(TEXT("Def Class"));
	(GetDlgItem(IDC_EDIT112))->SetWindowText(TEXT("L Pk-Depth"));
	(GetDlgItem(IDC_EDIT113))->SetWindowText(TEXT("Volume"));
	(GetDlgItem(IDC_EDIT114))->SetWindowText(TEXT("Y-AspRatio"));
	(GetDlgItem(IDC_EDIT115))->SetWindowText(TEXT("X-AspRatio"));

	strTextOut.Format(TEXT("%.6f"), CurPitNum+1);
	(GetDlgItem(IDC_EDIT121))->SetWindowText(strTextOut);
	CString st;
	st = TEXT("Pit");
	if(m_pitsMark[CurPitNum*PMN+5]==1)
		(GetDlgItem(IDC_EDIT126))->SetWindowText(st);
	else if (m_pitsMark[CurPitNum*PMN+5]==2)
	{
		st = TEXT("Scratch");
		(GetDlgItem(IDC_EDIT126))->SetWindowText(st);
	}

	//_gcvt(  PitsMark[CurPitNum*PMN], 6, Cbuffer );
	//(GetDlgItem(IDC_EDIT122))->SetWindowText(Cbuffer);
	strTextOut.Format(TEXT(" %.5f"),m_pitsMark[CurPitNum*PMN]);
	(GetDlgItem(IDC_EDIT122))->SetWindowText(strTextOut);

	strTextOut.Format(TEXT(" %.6f"),m_pitsMark[CurPitNum*PMN+3]);
	(GetDlgItem(IDC_EDIT123))->SetWindowText(strTextOut);

	//_gcvt(  (float)(YRangeD-YRangeU)/(float)Height, 6, Cbuffer );
	//(GetDlgItem(IDC_EDIT124))->SetWindowText(Cbuffer);
	strTextOut.Format(TEXT(" %.2f"),(float)(YRangeD-YRangeU)/(float)Height);
	(GetDlgItem(IDC_EDIT124))->SetWindowText(strTextOut);

	//_gcvt(  (float)(XRangeR-XRangeL)/(float)Width, 6, Cbuffer );
	//(GetDlgItem(IDC_EDIT125))->SetWindowText(Cbuffer);
	strTextOut.Format(TEXT(" %.2f"), (float)(XRangeR-XRangeL)/(float)Width);
	(GetDlgItem(IDC_EDIT125))->SetWindowText(strTextOut);

}

void CDlgHeightValue::OnBnClickedNextcor()
{
	// TODO: Add your control notification handler code here
	NextPitShowFlag=1;
	if(PitsNum!=0)
	{
		if(CurPitNum!=PitsNum-1)
			CurPitNum++;
		else
			CurPitNum=0;
	}
	else
		CurPitNum=0;

	OnBnClickedDeepest();
	NextPitShowFlag=0;
	DataDis();	
}

void CDlgHeightValue::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here

	CFileDialog fDlg(false,TEXT("*.dat"),NULL,OFN_OVERWRITEPROMPT,\
		TEXT("(*.dat)|*.dat|All Files(*.*)|*.*|"),NULL);
	if(fDlg.DoModal()!=IDOK) 
		return;
	CString pFileName=fDlg.GetPathName();
	CString pFileExt = fDlg.GetFileExt();

	////////////////////////////////////////////////////////////////////////////
	CStdioFile *sourcefile;
	CString strTarget,FmSource,str,str1,str2;
	sourcefile=new CStdioFile;
	FmSource.Format(pFileName);


	if(sourcefile->Open(FmSource, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone )==NULL)
		AfxMessageBox(TEXT("Cannot creat export file"));
	else
	{
		sourcefile->SeekToBegin();
		strTarget.Format(TEXT("Corrosion Gage Data Report:\n"));
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("Over all properties:\n"));
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("Peak Depth:     %.6f\n"),max);
		sourcefile->WriteString(strTarget);

		if (PitsNum!=0)
			strTarget.Format(TEXT("Pit Number:     %d\n"),PitsNum-m_scrNum);
		else
			strTarget.Format(TEXT("Pit Number:     %d\n"),PitsNum);

		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("Scratch Number: %d\n"),m_scrNum);
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("Pits/Area:      %.2f\n"),PitsNum/(maxX-minX)/(maxY-minY));
		sourcefile->WriteString(strTarget);

		int j=(Width*Height);
		float tmp=(float)(m_pointNum)/(float)(j)*100.0;
		strTarget.Format(TEXT("Coverage(%%):    %.2f\n"),tmp);
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("DepthLimit:     %.4f\n"),m_depthThreshold);
		sourcefile->WriteString(strTarget);


		strTarget.Format(TEXT("Selected Defect Properties:\n"));
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("Serial No.:     %d\n"),CurPitNum+1);
		sourcefile->WriteString(strTarget);

		if (m_pitsMark[CurPitNum*PMN+5]==1)
			strTarget.Format(TEXT("Defect Class:   Pit\n"));
		else
			strTarget.Format(TEXT("Defect Class:   Scratch\n"));
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("L P-Depth:      %.6f\n"),m_pitsMark[CurPitNum*PMN]);
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("Volume:         %.12f\n"),m_pitsMark[CurPitNum*PMN+3]);
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("Y-AspRatio:     %.6f\n"),(float)(YRangeD-YRangeU)/(float)Height);
		sourcefile->WriteString(strTarget);

		strTarget.Format(TEXT("X-AspRatio:     %.6f\n"), (float)(XRangeR-XRangeL)/(float)Width);
		sourcefile->WriteString(strTarget);

		sourcefile->Close();
	}

	delete sourcefile;

}

void CDlgHeightValue::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here

	CFileDialog fDlg(false,TEXT("*.bmp"),NULL,OFN_OVERWRITEPROMPT,TEXT("Files(*.bmp)|*.bmp|All Files(*.*)|*.*||"),NULL);

	CRect rect;
	GetWindowRect(&rect);
	HBITMAP hMap =  CopyScreenToBitmap(rect); 

	if(fDlg.DoModal()!=IDOK) 
		return;
	CString name=fDlg.GetPathName();
	CString strError;
	SaveBitmapToFile(hMap,name); 
}


HBITMAP CDlgHeightValue::CopyScreenToBitmap(LPRECT lpRect) //lpRect 代表选定区域
{
	HDC hScrDC, hMemDC;      
	// 屏幕和内存设备描述表
	HBITMAP hBitmap,hOldBitmap;   
	// 位图句柄
	int       nX, nY, nX2, nY2;      
	// 选定区域坐标
	int       nWidth, nHeight;      
	// 位图宽度和高度
	int       xScrn, yScrn;         
	// 屏幕分辨率
	// 确保选定区域不为空矩形
	if (IsRectEmpty(lpRect))
		return NULL;
	//为屏幕创建设备描述表
	hScrDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;
	// 获得屏幕分辨率
	xScrn = GetDeviceCaps(hScrDC, HORZRES);
	yScrn = GetDeviceCaps(hScrDC, VERTRES);
	//确保选定区域是可见的
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > xScrn)
		nX2 = xScrn;
	if (nY2 > yScrn)
		nY2 = yScrn;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap=CreateCompatibleBitmap(hScrDC,nWidth,nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap=(HBITMAP)SelectObject(hMemDC,hBitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中
	BitBlt(hMemDC,0,0, nWidth,nHeight,hScrDC, nX, nY, SRCCOPY);
	//得到屏幕位图的句柄
	hBitmap=(HBITMAP)SelectObject(hMemDC,hOldBitmap);
	//清除 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	// 返回位图句柄
	return hBitmap;
}


int CDlgHeightValue::SaveBitmapToFile(HBITMAP hBitmap, CString lpFileName) //hBitmap 为刚才的屏幕位图句柄
{      //lpFileName 为位图文件名
	HDC     hDC;         
	//设备描述表
	int     iBits;      
	//当前显示分辨率下每个像素所占字节数
	WORD    wBitCount;   
	//位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，  位图文件大小 ， 写入文件字节数
	DWORD           dwPaletteSize=0,dwBmBitsSize,dwDIBSize, dwWritten;
	BITMAP          Bitmap;        
	//位图属性结构
	BITMAPFILEHEADER   bmfHdr;        
	//位图文件头结构
	BITMAPINFOHEADER   bi;            
	//位图信息头结构 
	LPBITMAPINFOHEADER lpbi;          
	//指向位图信息头结构
	HANDLE          fh, hDib, hPal;
	HPALETTE     hOldPal=NULL;
	//定义文件，分配内存句柄，调色板句柄

	//计算位图文件每个像素所占字节数
	hDC = CreateDC(TEXT("DISPLAY"),NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * 
		GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 32;
	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);

	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = Bitmap.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;

	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;
	//为位图内容分配内存

	/*xxxxxxxx计算位图大小分解一下(解释一下上面的语句)xxxxxxxxxxxxxxxxxxxx 
	//每个扫描行所占的字节数应该为4的整数倍，具体算法为:
	int biWidth = (Bitmap.bmWidth*wBitCount) / 32;
	if((Bitmap.bmWidth*wBitCount) % 32)
	biWidth++; //不是整数倍的加1
	biWidth *= 4;//到这里，计算得到的为每个扫描行的字节数。
	dwBmBitsSize = biWidth * Bitmap.bmHeight;//得到大小
	xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/


	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板   
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值
	GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize, (BITMAPINFO *)lpbi,DIB_RGB_COLORS);
	//恢复调色板   
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	//创建位图文件    
	fh=CreateFile(lpFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh==INVALID_HANDLE_VALUE)
		return FALSE;
	// 设置位图文件头
	bmfHdr.bfType = 0x4D42;  // "BM"
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	// 写入位图文件头
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容
	WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize , &dwWritten, NULL); 
	//清除   
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}


void CDlgHeightValue::OnBnClickedDiareset()
{
	// TODO: Add your control notification handler code here
	CString tmpstr;
	m_setflag = TRUE;
	m_diaLimit=_ttof( tmpstr);

	/////////////////////////////////////////////////////////////////////////////////
	MarkSurface();
	CurPitNum=0;
	OnBnClickedDeepest();
	/////////////////////////////////////////////////////////////////////////////////	
	Invalidate();
	m_setflag = TRUE;

}

void CDlgHeightValue::OnBnClickedRevsur()
{
	// TODO: Add your control notification handler code here
	RevSurFlag=-RevSurFlag;
	/////////////////////////////////////////////////////////////////////////////////
	for (int j =0; j < Height; j++)
	{     
		for (int i2 = 0; i2 < Width; i2++)
		{   					
			m_ptrVertexList[(j*Width+i2)].z=-m_ptrVertexList[(j*Width+i2)].z;
		}
	}  

	//	InitTerreno();

	MarkSurface();
	CurPitNum=0;
	OnBnClickedDeepest();
	/////////////////////////////////////////////////////////////////////////////////	
	Invalidate();

}

void CDlgHeightValue::OnBnClickedDismeas()
{
	// TODO: Add your control notification handler code here
	m_disMeasFlag =TRUE;
	(GetDlgItem(IDC_SNAP))->EnableWindow(TRUE);
}

void CDlgHeightValue::OnBnClickedSnap()
{
	// TODO: Add your control notification handler code here
	m_snapPit != m_snapPit;
}

void CDlgHeightValue::DrawSurface()
{
	int i,j;
	double length = loader_.GetLength();
	m_Camera.SetViewVolume(length, length, -length/2, length/2);
	m_Camera.Apply();

	//////////////////////////////////////////////////////////////////////////////////////////////	

	// save the view state information , project matrix model matrix
	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX, m_ProjMatrix);
	glGetIntegerv(GL_VIEWPORT, m_Viewport);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//	glTranslatef(m_ViewTranslationX, m_ViewTranslationY, 0); // translation in Camera
	glRotatef(m_Orientation.m_Angle, m_Orientation.m_Axis[0], m_Orientation.m_Axis[1], m_Orientation.m_Axis[2]);
	glScalef(m_ScalFactor,m_ScalFactor,m_ScalFactor);
	glTranslatef(-m_SceneCenter[0], -m_SceneCenter[1], -m_SceneCenter[2]);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_ModelMatrix);

	//glClearColor(1.0f, 1.0f,1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	float c1,c2,c3,c4;
	float xd,zd;
	float media;
	float tt;
	int ka,kb,kc,kd;

	//	if(type==OBJECT_PLANE)
	//	{
	for(i=0;i<Height-1;i++)
	{	
		glBegin(GL_TRIANGLE_STRIP);
		for(j=0;j<Width-1;j++)
		{

			c1=m_ptrVertexList[(i*Width+j)].z;
			c2=m_ptrVertexList[((i+1)*Width+j)].z;
			c3=m_ptrVertexList[((i+1)*Width+j+1)].z;
			c4=m_ptrVertexList[(i*Width+j+1)].z;


			tt=256/(max-min);
			// 根据高程求出颜色
			media=c1-min;
			media=media*tt;
			ka=255-(int)media;
			ka=ka*4;
			if(ka<0)				
				ka=0;
			else if(ka>256*4)
				ka=	97;
			// 根据高程求出颜色
			media=c2-min;
			media=media*tt;
			kb=255-(int)media;
			kb=kb*4;
			if(kb<0) 
				kb=0;
			else if( kb>256*4)
				kb=97;
			// 根据高程求出颜色
			media=c3-min;
			media=media*tt;
			kc=255-(int)media;
			kc=kc*4;
			if(kc<0)
				kc=0;
			else if( kc>256*4)
				kc=97;
			// 根据高程求出颜色
			media=c4-min;
			media=media*tt;
			kd=255-(int)media;
			kd=kd*4;
			if(kd<0)
				kd=0;
			else if( kd>256*4)
				kd=97;


			DrawVertex(i * Width + j, ka);
			DrawVertex((i+1)*Width+j, kb);
			DrawVertex(i*Width+j+1, kd);
			DrawVertex((i+1)*Width+j+1, kc);
		}
		glEnd();
	}

	float x1,x2,y1,y2;
	x1=minX-(maxX+minX)/2;
	x2=maxX-(maxX+minX)/2;
	y1=minY-(maxY+minY)/2;
	y2=maxY-(maxY+minY)/2;

	// 允许绘制虚线

	if (m_bShowSlice)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x0F0F);
		// glLineWidth(10.0f);
		glBegin(GL_LINES);
		glColor3f(0.5,0.5,0.5);
		for (i=0;i<5;i++)
		{
			glVertex3d(x1,y2,(i-2)*max);
			glVertex3d(x2,y2,(i-2)*max);
			glVertex3d(x2,y1,(i-2)*max);
			glVertex3d(x2,y2,(i-2)*max);
			glVertex3d(x1+i*(x2-x1)/5,y1,-2*max);
			glVertex3d(x1+i*(x2-x1)/5,y2,-2*max);
			glVertex3d(x1,y1+i*(y2-y1)/5,-2*max);
			glVertex3d(x2,y1+i*(y2-y1)/5,-2*max);
			glVertex3d(x1+i*(x2-x1)/5,y2,-2*max);
			glVertex3d(x1+i*(x2-x1)/5,y2,2*max);
			glVertex3d(x2,y1+i*(y2-y1)/5,-2*max);
			glVertex3d(x2,y1+i*(y2-y1)/5,2*max);
		}
		glEnd();
		glDisable(GL_LINE_STIPPLE);
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex3d(x2,y2,-2*max);
		glVertex3d(x2,y2,2*max);

		glVertex3d(x2,y2,-2*max);
		glVertex3d(x1,y2,-2*max);

		glVertex3d(x2,y1,-2*max);
		glVertex3d(x2,y2,-2*max);
		glEnd(); 
		glLineWidth(1.0f);
	}




	if(m_LeftButtonDown && m_bShowSlice && !m_disMeasFlag)
	{			
		glBegin(GL_LINES);
		PNT tmppt;
		CPoint ppt;
		ppt.x=PointerPosX-StartX;
		ppt.y=PointerPosY-StartY;
		tmppt= GetScreenPnt(ppt);
		glColor3f(1.0,0,0);
		glVertex3d(minX-(maxX+minX)/2,tmppt.m[1],fabs(max));
		glVertex3d(maxX-(maxX+minX)/2,tmppt.m[1],fabs(max));
		glColor3f(0,1.0,0);
		glVertex3d(tmppt.m[0],minY-(maxY+minY)/2,fabs(max));
		glVertex3d(tmppt.m[0],maxY-(maxY+minY)/2,fabs(max));
		glEnd();
	}

	if(m_LeftButtonDown && m_disMeasFlag)
	{		
		glLineWidth(2.0f);	
		m_PointFromView1 = GetScreenPnt(pt1);
		m_PointFromView2 = GetScreenPnt(pt2);
		glBegin(GL_LINES);
		glColor3f(1.0,1.0,0);
		glVertex3d(m_PointFromView1.m[0],m_PointFromView1.m[1],fabs(max));
		glVertex3d(m_PointFromView2.m[0],m_PointFromView2.m[1],fabs(max));
		glEnd();
		glLineWidth(1.0f);
	}

	// enable blending
	glEnable(GL_BLEND);
	// enable read-only depth buffer
	glDepthMask(GL_FALSE);
	// set the blend function to what we use for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// set back to normal depth buffer mode (writable)
	glDepthMask(GL_TRUE);
	// disable blending
	glDisable(GL_BLEND);
	glFinish();
	SwapBuffers(wglGetCurrentDC());
}

PNT CDlgHeightValue::GetScreenPnt(const CPoint& point)
{
	PNT viewpnt, screendir(0,0,1), spnt, centerpnt; 
	viewpnt[0] = point.x;
	viewpnt[1] = m_Viewport[3] - point.y;
	viewpnt[2] = 0.5;
	this->VertexTransform_WindowToObject(viewpnt);
	m_Orientation.m_Angle = -m_Orientation.m_Angle;
	screendir = m_Orientation.VectorTransform(screendir);
	screendir.Normalize();
	m_Orientation.m_Angle = -m_Orientation.m_Angle;
	centerpnt = this->m_SceneCenter;
	spnt= viewpnt;
	spnt = spnt - m_SceneCenter;		
	spnt = spnt - screendir*(spnt*screendir);
	spnt = spnt + centerpnt;                
	return spnt;
}

void CDlgHeightValue::VertexTransform_WindowToObject(PNT& pnt)
{
	double x, y, z;
	gluUnProject(pnt[0], pnt[1], pnt[2],m_ModelMatrix, m_ProjMatrix, m_Viewport, &x, &y, &z);
	pnt[0] = x;
	pnt[1] = y;
	pnt[2] = z;
}

void CDlgHeightValue::VertexTransform_ObjectToWindow(PNT& pnt)
{
	double x, y, z;
	//pnt = PNT();
	gluProject(pnt[0], pnt[1], pnt[2],	m_ModelMatrix, m_ProjMatrix, m_Viewport, &x, &y, &z);
	pnt[0] = x;
	pnt[1] = y;
	pnt[2] = z;
}

void CDlgHeightValue::SetLogicalPalette(void)
{
	struct
	{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} logicalPalette = { 0x300, 256 };

	BYTE reds[] = {0, 36, 72, 109, 145, 182, 218, 255};
	BYTE greens[] = {0, 36, 72, 109, 145, 182, 218, 255};
	BYTE blues[] = {0, 85, 170, 255};

	for (int colorNum=0; colorNum<256; ++colorNum)
	{
		logicalPalette.aEntries[colorNum].peRed =
			reds[colorNum & 0x07];
		logicalPalette.aEntries[colorNum].peGreen =
			greens[(colorNum >> 0x03) & 0x07];
		logicalPalette.aEntries[colorNum].peBlue =
			blues[(colorNum >> 0x06) & 0x03];
		logicalPalette.aEntries[colorNum].peFlags = 0;
	}
}



void CDlgHeightValue::InitTerreno(void)
{
	TopLeft.m[0] = m_ptrVertexList[0].x;
	TopLeft.m[1] = m_ptrVertexList[0].y;
	TopLeft.m[2] = m_ptrVertexList[0].z;

	int lastIndex = m_ptrVertexList.size() - 1;
	BottomRight.m[0] = m_ptrVertexList[lastIndex].x;
	BottomRight.m[1] = m_ptrVertexList[lastIndex].y;
	BottomRight.m[2] = m_ptrVertexList[lastIndex].z;


	minX=1000000000;
	maxX=-1000000000;
	minY=1000000000;
	maxY=-100000000;

	assert(m_ptrVertexList.size() != 0);

	for (UINT i = 0; i < m_height ; i++)
	{
		for (UINT j = 0; j < m_width ; j++)
		{
			float x = m_ptrVertexList[i*m_width+j].x;
			float y = m_ptrVertexList[i*m_width+i].y;
			if( x > maxX)
			{
				maxX= x;
			}
			if( x < minX)
			{
				minX= x;
			}
			if( y > maxY)
			{
				maxY = y;
			}
			if( y < minY)
			{
				minY= y;
			}
		}
	}

	for (UINT i =0; i < m_height; i++)
	{     
		for (UINT j = 0; j < m_width; j++)
		{
			m_ptrVertexList[i*m_width+j].x -= (maxX+minX)/2;
			m_ptrVertexList[i*m_width+j].y -= (maxY+minY)/2;			
		}
	}  



	int StartRow = m_height / 2,startCol = m_width / 2;
	double HSlope=(double)((m_ptrVertexList[(StartRow*m_width+(m_width-4))].z-m_ptrVertexList[(StartRow*m_width+1)].z)/((m_width-4)-1));
	double VSlope=(double)((m_ptrVertexList[((m_height - 1)*m_width+startCol)].z-m_ptrVertexList[(1*m_width+startCol)].z)/((m_height - 1)-1));


	std::vector<float> vcZtemp(m_width * m_height);

	for ( int i=0; i<m_width; i++)
	{
		for(int j=0;j<m_height;j++)
		{
			vcZtemp[j*m_width+i]=m_ptrVertexList[(j*m_width+i)].z;
		}
	}

	for (int i =0; i<m_width; i++)
	{
		for(int j=0;j<m_height;j++)
		{
			vcZtemp[j*m_width+i]=m_ptrVertexList[(j*m_width+i)].z - HSlope*i - m_ptrVertexList[0].z;
		}
	}

	VSlope=(double)((vcZtemp[(m_height - 1)*m_width+startCol]-vcZtemp[1*m_width+startCol])/((m_height - 1)-1));

	for (int i =0; i<m_width; i++)
	{
		for(int j=0;j<m_height;j++)
		{
			vcZtemp[j*m_width+i]=vcZtemp[j*m_width+i]-VSlope*j-vcZtemp[0];
		}
	}
	double tmp=0;
	int k=0;

	for (int  i =0; i<m_width; i++)
	{
		for(int j=0;j<m_height;j++)
		{
			if (vcZtemp[j*m_width+i]<0)
			{
				k++;
				tmp=tmp+vcZtemp[j*m_width+i];					
			}
		}
	}
	tmp=tmp/k;

	for (int  i =0; i<m_width; i++)
	{
		for(int j=0;j<m_height;j++)
		{			
			vcZtemp[j*m_width+i] -= -tmp;						
		}
	}

	for (int  i =0; i<m_width; i++)
	{
		for(int j=0;j<m_height;j++)
		{
			m_ptrVertexList[(j*m_width+i)].z = vcZtemp[j*m_width+i];
		}
	}

	Realmax=-100000;
	Realmin=100000;
	MaxPosX=0;
	MaxPosY=0;


	for(int t=0;t< m_height;t++)
	{
		for(int t2=0;t2< m_width;t2++)
		{
			if(m_ptrVertexList[(t*m_width+t2)].z >Realmax)
			{
				Realmax=m_ptrVertexList[(t*m_width+t2)].z;
				MaxPosX=t2;
				MaxPosY=t;
			}
			if(m_ptrVertexList[(t*m_width+t2)].z < Realmin)
			{
				Realmin=m_ptrVertexList[(t*m_width+t2)].z;
			}
		}
	}
	MarkSurface();
}


int CDlgHeightValue::SolveQuadratic( double c1, double c2, double c3, 
	double *r1, double *r2, int & num_roots )
{
	// 	double	Q;
	// 	double	determinant;
	// 
	// 	// Quadratic equation: c1*t^2 + c2*t + c3 = 0 
	// 
	// 	// Make sure this is a quadratic equation
	// 	if( c1 != 0.0 )
	// 	{
	// 		determinant = c2*c2 - 4*c1*c3;
	// 
	// 		if( determinant >= 0.0 )
	// 		{
	// 			Q = -0.5 * (c2 + TI_SIGN(c2)*sqrt(determinant));
	// 
	// 			*r1 = Q / c1;
	// 
	// 			if( Q == 0.0 )
	// 			{
	// 				*r2 = 0.0;
	// 			}
	// 			else
	// 			{
	// 				*r2 = c3 / Q;
	// 			}
	// 
	// 			num_roots = 2;
	// 
	// 			// Reduce Number Of Roots To One 
	// 			if( *r1 == *r2 )
	// 			{
	// 				num_roots = 1;
	// 			}
	// 			return num_roots;
	// 		}
	// 		else	// Equation Does Not Have Real Roots 
	// 		{
	// 			num_roots = 0;
	// 			return (-2);
	// 		}
	// 	}
	// 
	// 	else // Linear Equation: c2*t + c3 = 0 
	// 	{
	// 		// Okay this was not quadratic - lets try linear
	// 		return SolveLinear( c2, c3, r1, num_roots );
	// 	}
	return 0;
}

int CDlgHeightValue::SolveLinear( double c2, double c3, double *r1, int & num_roots )
{
	//// Linear equation: c2*t + c3 = 0 
	//// Now this had better be linear 
	//if( c2 != 0.0 )
	//{
	//	*r1 = -c3 / c2;
	//	num_roots = 1;
	//	return num_roots;
	//}
	//else
	//{
	//	num_roots = 0;
	//	if ( c3 == 0.0 )
	//	{
	//		return (-1);
	//	}
	//}

	return num_roots;
}

void CDlgHeightValue::OnMouseMove(UINT nFlags, CPoint point)
{

	// TODO: Add your message handler code here and/or call default
	if(m_LeftButtonDown && !m_deepestShow && !m_disMeasFlag)
	{
		CSize rotate = m_LeftDownPos - point;
		m_LeftDownPos = point;

		if(!m_bShowSlice)
		{
			if(point.x>Mysrect.Width()/4 && point.x<Mysrect.Width()/4+DisplaySize &&point.y>20&& point.y<DisplaySize+20)
			{   	    	
				float dx, dy;
				dx=point.x - m_MousePoint.x;
				dy=point.y - m_MousePoint.y;
				m_MousePoint.x=point.x;
				m_MousePoint.y=point.y;
				PNT axis;
				float  angle;
				m_TrackBall.Tracking(dx, dy, axis, angle);
				m_Orientation.m_Angle = -m_Orientation.m_Angle;
				axis = this->m_Orientation.VectorTransform(axis);
				m_Orientation.m_Angle = -m_Orientation.m_Angle;
				if(fabs(angle)< 1e-3)
					return;
				axis.Normalize();
				Ti_Rotation   R(axis, angle);
				m_Orientation = R*m_Orientation;				
				DrawSurface();	
			}
		}
		else 
		{		
			m_Orientation.m_Angle=0;
			m_Orientation.m_Axis[0]=0, m_Orientation.m_Axis[1]=0, m_Orientation.m_Axis[2]=1;				

			if(point.x>SurRect.left && point.x<SurRect.right+5 &&
				point.y>SurRect.top && point.y<SurRect.bottom+5 )
			{
				int i=1;
				PointerPosX=point.x;
				PointerPosY=point.y;
			}


			RefreshAll();
			////刷新窗口 
			//pWnd=GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错
			//m_ysliceDC=pWnd->GetDC();
			//pWnd->Invalidate();
			//pWnd->UpdateWindow();
			//DrawYSlice();
			////刷新窗口 
			//pWnd=GetDlgItem(IDC_SURFACESHOW);//一定要再次得到指针才能更新，否则会出错
			//m_ppDC=pWnd->GetDC();
			//wglMakeCurrent(m_ppDC->GetSafeHdc(), hrc);
			//DrawSurface();
			////刷新窗口 
			//pWnd=GetDlgItem(IDC_XSLICE);//一定要再次得到指针才能更新，否则会出错
			//m_xsliceDC=pWnd->GetDC();
			//pWnd->Invalidate();
			//pWnd->UpdateWindow(); 
			//DrawXSlice();
		}
	}

	if (m_LeftButtonDown == TRUE && m_disMeasFlag==TRUE )
	{	
		//::SetCursor(AfxGetApp()->LoadCursor(IDC_CROSS1));

		if(point.x>SurRect.left && point.x<SurRect.right+5 &&
			point.y>SurRect.top && point.y<SurRect.bottom+5 )
		{
			PointerPosX=point.x;
			PointerPosY=point.y;
		}
		pt1.x=m_LeftDownPos.x-StartX;
		pt1.y=m_LeftDownPos.y-StartY;	
		pt2.x=point.x-StartX;
		pt2.y=point.y-StartY;	

		pWnd=GetDlgItem(IDC_SURFACESHOW);//一定要再次得到指针才能更新，否则会出错
		m_ppDC=pWnd->GetDC();
		wglMakeCurrent(m_ppDC->GetSafeHdc(), hrc);
		DrawSurface();
	} 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CDialog::OnMouseMove(nFlags, point);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void CDlgHeightValue::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnLButtonDown(nFlags, point);
	if(point.x>Mysrect.Width()*0.25&& point.x<Mysrect.Width()*(0.25+0.7)&&point.y>20&& point.y<Mysrect.Height()*0.8+20)
	{
		m_LeftButtonDown = TRUE;
		m_LeftDownPos = point;
		m_TrackBall.Start(point.x, point.y);
		if (m_disMeasFlag==TRUE &&m_snapPit == TRUE)
		{
			int num;
			TargetPit( point,  num);
			float tmp=(float)m_pitsMark[num*PMN+1]/(Width-1);
			m_LeftDownPos.x=SurRect.left +tmp*SurRect.Width();
			tmp=1-(float)m_pitsMark[num*PMN+2]/(Height-1);		
			m_LeftDownPos.y=SurRect.top+tmp*SurRect.Height();	
		}
	}  
}

void CDlgHeightValue::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnLButtonUp(nFlags, point);

	m_LeftButtonDown = FALSE;

	if(m_disMeasFlag )
	{	
		float tmp;
		tmp=(m_PointFromView1-m_PointFromView2).Length();
		CString str;
		str.Empty();	
		str.Format(TEXT("Length = %.4f;"),tmp);
		MessageBox(str);
	}
}

void CDlgHeightValue::TargetPit(CPoint point, int & num)
{
	float tmp=1.0-(float)(point.y-SurRect.top)/SurRect.Height();
	int Ynum=tmp*(Height-1); 
	if (Ynum<0) Ynum=0;
	if(Ynum>Height-1) Ynum=Height;

	tmp=(float)(point.x-SurRect.left)/SurRect.Width();
	int Xnum=tmp*(Width-1); 
	if (Xnum<0) Xnum=0;
	if(Xnum>Width-1) Xnum=Width;

	float tmp1=10000000;
	for (int i=0;i<PitsNum;i++)
	{
		tmp=sqrt((Xnum-m_pitsMark[i*PMN+1])*(Xnum-m_pitsMark[i*PMN+1])+(Ynum-m_pitsMark[i*PMN+2])*(Ynum-m_pitsMark[i*PMN+2]));
		if(tmp<tmp1)
		{
			tmp1=tmp;
			num=i;
		}          
	}
}

void CDlgHeightValue::GetSurRect()
{
	PNT tmp1,tmp2;
	DrawSurface();
	tmp1=TopLeft;
	tmp2=BottomRight;
	this->VertexTransform_ObjectToWindow(TopLeft);
	this->VertexTransform_ObjectToWindow(BottomRight);
	SurRect.left=TopLeft.m[0]+StartX-(BottomRight.m[0]-TopLeft.m[0])/2-5;
	SurRect.top=TopLeft.m[1]+StartY-(BottomRight.m[1]-TopLeft.m[1])/2-5;
	SurRect.right=SurRect.left+BottomRight.m[0]-TopLeft.m[0];		
	SurRect.bottom=SurRect.top+BottomRight.m[1]-TopLeft.m[1];
	// 	SurRect.left = 551;
	// 	SurRect.top = 63;
	// 	SurRect.right = 755;
	// 	SurRect.bottom = 507;
	TopLeft=tmp1;
	BottomRight=tmp2;
}

int CDlgHeightValue::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

BOOL CDlgHeightValue::SetupPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),    // pfd结构的大小 
		1,                                // 版本号 
		PFD_DRAW_TO_WINDOW |              // 支持在窗口中绘图 
		PFD_SUPPORT_OPENGL |              // 支持 OpenGL 
		PFD_DOUBLEBUFFER,                 // 双缓存模式 
		PFD_TYPE_RGBA,                    // RGBA 颜色模式 
		24,                               // 24 位颜色深度 
		0, 0, 0, 0, 0, 0,                 // 忽略颜色位 
		0,                                // 没有非透明度缓存 
		0,                                // 忽略移位位 
		0,                                // 无累加缓存 
		0, 0, 0, 0,                       // 忽略累加位 
		32,                               // 32 位深度缓存     
		0,                                // 无模板缓存 
		0,                                // 无辅助缓存 
		PFD_MAIN_PLANE,                   // 主层 
		0,                                // 保留 
		0, 0, 0                           // 忽略层,可见性和损毁掩模 
	}; 	
	int pixelformat;
	pixelformat = ::ChoosePixelFormat(m_ppDC->GetSafeHdc(), &pfd);//选择像素格式
	::SetPixelFormat(m_ppDC->GetSafeHdc(), pixelformat, &pfd);	//设置像素格式
	if(pfd.dwFlags & PFD_NEED_PALETTE)
		SetLogicalPalette();	//设置逻辑调色板
	return TRUE;
}

void CDlgHeightValue::CreateRGBPalette(HDC hDC)
{
	// 	PIXELFORMATDESCRIPTOR pfd;
	// 	LOGPALETTE *pPal;
	// 	int n, i;
	// 
	// 	n = GetPixelFormat(hDC);
	// 	DescribePixelFormat(hDC, n, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	// 
	// 	if (pfd.dwFlags & PFD_NEED_PALETTE) {
	// 		n = 1 << pfd.cColorBits;
	// 		pPal = (PLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
	// 			n * sizeof(PALETTEENTRY));
	// 		pPal->palVersion = 0x300;
	// 		pPal->palNumEntries = n;
	// 		for(i=0; i<n; i++) {
	// 			pPal->palPalEntry[i].peRed =
	// 				ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
	// 			pPal->palPalEntry[i].peGreen =
	// 				ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
	// 			pPal->palPalEntry[i].peBlue =
	// 				ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
	// 			pPal->palPalEntry[i].peFlags = 0;
	// 		}
	// 
	// 		// fix up the palette to include the default GDI palette 
	// 		if ((pfd.cColorBits == 8)                           &&
	// 			(pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
	// 			(pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
	// 			(pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
	// 			) {
	// 				for (i = 1 ; i <= 12 ; i++)
	// 					pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
	// 		}
	// 
	// 		ghPalette = CreatePalette(pPal);
	// 		LocalFree(pPal);
	// 
	// 		ghpalOld = SelectPalette(hDC, ghPalette, FALSE);
	// 		n = RealizePalette(hDC);
	// 	}
}


void CDlgHeightValue::OnLoadTexture()
{
	CFileDialog dlgLoad(TRUE,_T("*"),_T("texturefile"),NULL,_T("Image Files (*.jpg;*.bmp)|*.jpg;*.bmp| All Files (*.*) | *.*|"));

	if (dlgLoad.DoModal() == IDOK)
	{
		CString strTexturePath = dlgLoad.GetPathName();

		USES_CONVERSION;
		m_bShowTexture = GetRawRGBData(strTexturePath.GetString());

		if (m_bShowTexture)
		{
			ApplyTexture();
		}

	}
}

BOOL CDlgHeightValue::GetRawRGBData( LPCTSTR filename )
{


	//初始化FreeImage
	FreeImage_Initialise(TRUE);

	//定义图片格式为未知
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	//获取图片格式
#ifdef _UNICODE
	fif = FreeImage_GetFileTypeU(filename,0);
#else
	fif = FreeImage_GetFileType(filename,0);
#endif


	//根据获取格式读取图片数据
#ifdef _UNICODE
	FIBITMAP* bitmap = FreeImage_LoadU(fif,filename,0);
#else
	FIBITMAP* bitmap = FreeImage_Load(fif,filename,0);
#endif

	if(!bitmap)
	{
		printf("load error!\n");
		return FALSE;
	}

	int x,y;
	RGBQUAD m_rgb;

	//获取图片长宽
	int width,height;
	width = (int)FreeImage_GetWidth(bitmap);
	height = (int)FreeImage_GetHeight(bitmap);


	if (width > Width)
	{
		width = Width;
	}

	if (height > Height)
	{
		height = Height;
	}

	m_rgbData.reserve(width * height);

	//获取图片数据
	//按RGBA格式保存到数组中




	for(y=0;y<height;y++)
	{
		for(x=0;x<width;x++)
		{
			//获取像素值
			BOOL bret = FreeImage_GetPixelColor(bitmap,x,y,&m_rgb);


			//判断是否透明图片
			//如果是就取alpha值保存
			if(!FreeImage_IsTransparent(bitmap))
			{
				m_rgb.rgbReserved = 255;
			}

			//将RGB值存入数组
			m_rgbData.push_back(m_rgb);
		}	
	}

	//释放FreeImage
	FreeImage_Unload(bitmap);

	return TRUE;
}

void CDlgHeightValue::ApplyTexture()
{
	RefreshAll();
}

void CDlgHeightValue::RefreshAll()
{

	//刷新窗口 
	pWnd=GetDlgItem(IDC_SURFACESHOW);//一定要再次得到指针才能更新，否则会出错
	m_ppDC=pWnd->GetDC();
	wglMakeCurrent(m_ppDC->GetSafeHdc(), hrc);
	DrawSurface();
	//刷新窗口 
	//刷新窗口 
	pWnd=GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错
	m_ysliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	DrawYSlice();
	pWnd=GetDlgItem(IDC_XSLICE);//一定要再次得到指针才能更新，否则会出错
	m_xsliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow(); 
	DrawXSlice();
}

void CDlgHeightValue::DrawVertex(UINT vertexIndex, UINT colorIndex)
{
	if (m_bShowTexture)
	{
		GLfloat red		 = (GLfloat)m_rgbData[(vertexIndex)].rgbRed / 255.0f; 
		GLfloat green	 = (GLfloat)m_rgbData[(vertexIndex)].rgbGreen/ 255.0f; 
		GLfloat blue	 = (GLfloat)m_rgbData[(vertexIndex)].rgbBlue / 255.0f; 
		glColor3f(red,green,blue);
	}
	else if(m_ptrVertexList[(vertexIndex)].z>m_depthThreshold)
	{
		glColor3f(land[colorIndex],land[colorIndex+1],land[colorIndex+2]);
	}
	else
	{
		glColor3f(0.5,0.5,0.5);
	}

	glVertex3f( m_ptrVertexList[(vertexIndex)].x,m_ptrVertexList[(vertexIndex)].y, m_ptrVertexList[(vertexIndex)].z);
}

void CDlgHeightValue::SetDisplayRect( int width, int height )
{

}

void CDlgHeightValue::EnableShow3D()
{
	pWnd=GetDlgItem(IDC_SURFACESHOW);
	m_ppDC=pWnd->GetDC();
	wglMakeCurrent(m_ppDC->GetSafeHdc(), hrc);
	DrawSurface();
	m_LeftButtonDown=FALSE;
	m_deepestShow = FALSE;
	m_bShowSlice= FALSE;
}

void CDlgHeightValue::EnableShowSlice()
{
	// TODO: Add your control notification handler code here
	m_bShowSlice = TRUE;
	m_Orientation.m_Angle=0;
	m_Orientation.m_Axis[0]=0, m_Orientation.m_Axis[1]=0, m_Orientation.m_Axis[2]=1;				
	m_deepestShow = FALSE;
}

void CDlgHeightValue::EnableShowTexture()
{
	EnableShow3D();
	m_bShowTexture = TRUE;
}

void CDlgHeightValue::InitializeSysAllParameters()
{
	m_scrNum				   = 0;
	m_pointNum				   = 0;
	m_MousePoint.x			   = 0;
	m_MousePoint.y			   = 0;
	RevSurFlag				   = 1;
	m_pixelSize				   = 3.35;
	pt1.x					   = 284;
	pt1.y					   = 284;
	m_width					   = 392;
	m_height				   = 181;
	m_depthThreshold		   = 0.001008f;
	m_diaLimit				   = 0.00052f;
	m_ScalFactor			   = 1;
	NextPitShowFlag			   = 0;
	pWnd					   = NULL;
	m_pDC					   = NULL;
	m_setflag				   = FALSE;
	m_LeftButtonDown		   = FALSE;
	m_snapPit				   = FALSE;
	m_disMeasFlag			   = FALSE;
	m_deepestShow			   = FALSE;
	m_bShowSlice			   = FALSE;
	m_bShowTexture			   = FALSE;

	this->m_Orientation.m_Axis = PNT(0,0,1);

	memset(&m_ModelMatrix,0,sizeof(m_ModelMatrix));
	memset(&m_ProjMatrix,0,sizeof(m_ProjMatrix));
	memset(&m_Viewport,0,sizeof(m_Viewport));
}

void CDlgHeightValue::LayoutAllSysComponents()
{
	int offset = 15;

	if (Mysrect.Height()<800)
	{	
		pWnd=GetDlgItem(IDC_SURFACESHOW);
		m_ppDC=pWnd->GetDC();
		StartX= Mysrect.Width() / 6;
		StartY= 40;
		DisplaySize = Mysrect.Width()*0.55<Mysrect.Height()*0.53?Mysrect.Width()*0.55:Mysrect.Height()*0.53;
		pWnd->MoveWindow(StartX,StartY,DisplaySize,DisplaySize,TRUE);	
		pWnd->GetClientRect(rect);

		//////////////////////////////////////////////////////////////////////////////////////////////	
		glViewport(0, 0, rect.Width(), rect.Height());
		GetSurRect();
		pWnd=GetDlgItem(IDC_XSLICE);
		m_xsliceDC=pWnd->GetDC();	
		pWnd->MoveWindow(Mysrect.Width() / 6,40+DisplaySize,DisplaySize+DisplaySize*0.6,DisplaySize/3,TRUE);	
		pWnd->GetClientRect(Xrect);

		pWnd=GetDlgItem(IDC_YSLICE);
		m_ysliceDC=pWnd->GetDC();	
		pWnd->MoveWindow(Mysrect.Width() / 6+DisplaySize,40,DisplaySize*0.6,DisplaySize/*+DisplaySize/4*/,TRUE);	
		pWnd->GetClientRect(Yrect);
	}
	else
	{	
		pWnd=GetDlgItem(IDC_SURFACESHOW);
		m_ppDC=pWnd->GetDC();
		StartX=Mysrect.Width()/3.6;
		StartY=40;
		DisplaySize=Mysrect.Width()*0.25<Mysrect.Height()*0.33?Mysrect.Width()*0.25:Mysrect.Height()*0.33;
		pWnd->MoveWindow(StartX,StartY,DisplaySize,DisplaySize,TRUE);	
		pWnd->GetClientRect(rect);

		//////////////////////////////////////////////////////////////////////////////////////////////	
		glViewport(0, 0, rect.Width(), rect.Height());
		GetSurRect();
		pWnd=GetDlgItem(IDC_XSLICE);
		m_xsliceDC=pWnd->GetDC();	
		pWnd->MoveWindow(Mysrect.Width()/3.6,40+DisplaySize,DisplaySize+DisplaySize*0.6,DisplaySize/3,TRUE);	
		pWnd->GetClientRect(Xrect);

		pWnd=GetDlgItem(IDC_YSLICE);
		m_ysliceDC=pWnd->GetDC();	
		pWnd->MoveWindow(Mysrect.Width()/3.6+DisplaySize,40,DisplaySize*0.6,DisplaySize/*+DisplaySize/4*/,TRUE);	
		pWnd->GetClientRect(Yrect);
	}
	// 		pWnd=GetDlgItem(IDC_SURFACEHISTOGRAM);
	// 		m_surfacehisDC=pWnd->GetDC();	
	// 		pWnd->GetClientRect(HisRec);
	// 		pWnd->Invalidate();
	// 		pWnd->UpdateWindow();
}

BOOL CDlgHeightValue::InitializeOpenGLContent()
{
	pWnd=GetDlgItem(IDC_SURFACESHOW);
	m_ppDC=pWnd->GetDC();	

	PIXELFORMATDESCRIPTOR pfd;
	ASSERT(m_ppDC!=NULL);
	if (!SetupPixelFormat())
	{
		return FALSE;
	}
	int n=::GetPixelFormat(m_ppDC->GetSafeHdc());
	::DescribePixelFormat(m_ppDC->GetSafeHdc(),n,sizeof(pfd),&pfd);
	hrc = wglCreateContext(m_ppDC->GetSafeHdc());
	wglMakeCurrent(m_ppDC->GetSafeHdc(), hrc); 

	wglUseFontBitmaps(wglGetCurrentDC(), 0, 256, 1000);
	glShadeModel(GL_SMOOTH);  
	glFrontFace (GL_CCW);
	glDepthFunc (GL_LESS);
	glEnable    (GL_DEPTH_TEST);

	return	TRUE;
}