// DlgOpenGL.cpp : implementation file
//

#include "stdafx.h"
#include "ETE.h"
#include "DlgHeightValue.h"
#include "ETESystemPublicDefine.h"




// CDlgOpenGL dialog

IMPLEMENT_DYNAMIC(CDlgHeightValue, CDialog)

	CDlgHeightValue::CDlgHeightValue(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHeightValue::IDD, pParent)
{
	this->m_GLPixelIndex = 0;
	this->m_hGLContext = NULL;
	this->sceneRot[0]	= 0;
	this->sceneRot[1]	= 0;
	this->sceneRot[2]	= 0;
	this->m_ScalFactor  = 0.2;
	this->m_SceneCenter.x = 0;
	this->m_SceneCenter.y = 0;
	this->m_SceneCenter.z = 0;
	this->mouseleftdown = FALSE;
	this->mouserightdown = FALSE;
	this->mouseprevpoint.x = 0;
	this->mouseprevpoint.y = 0;
	this->m_bReloading = FALSE;
	this->PointerPosX = 0;
	this->PointerPosY = 0;
	this->pWnd = nullptr;
	this->StartX = this->StartY = 0;
}

CDlgHeightValue::~CDlgHeightValue()
{
}

void CDlgHeightValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHeightValue, CDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_HEIGHTVALUE_SHOWSLICE, &CDlgHeightValue::OnHeightvalueShowslice)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgOpenGL message handlers


int CDlgHeightValue::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;



	return 0;
}


BOOL CDlgHeightValue::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!InitializeOpenGLContent())
	{
		AfxMessageBox(TEXT("InitializeOpenGLContent Failed!"));
		return FALSE;
	}

	GetSurRect();

	SetTimer(timer_render, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



BOOL CDlgHeightValue::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);//openGL必需的
	return CDialog::PreCreateWindow(cs);
}

BOOL CDlgHeightValue::SetWindowPixelFormat(HDC hDC)
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
	this->m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);//选择最相近的像素格式
	if(this->m_GLPixelIndex==0)
	{//选择失败
		this->m_GLPixelIndex = 1;//默认的像素格式
		if(DescribePixelFormat(hDC,this->m_GLPixelIndex,sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
		{//用默认的像素格式进行设置
			return FALSE;
		}
	}
	if(SetPixelFormat(hDC,this->m_GLPixelIndex,&pixelDesc)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgHeightValue::InitGL(GLvoid) // All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH); // Enable Smooth Shading
	glClearColor(0.0,0.0,0.0,0.0); // Black Background
	glClearDepth(1.0f);      // Depth Buffer Setup
	//glEnable(GL_DEPTH_TEST);  // Enables Depth Testing
	//glDepthFunc(GL_LEQUAL);  // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// Really Nice Perspective Calculations
	return TRUE;      // Initialization Went OK
}

BOOL CDlgHeightValue::CreateViewGLContext(HDC hDC)
{


	this->m_hGLContext = wglCreateContext(m_ppDC->GetSafeHdc());//创建RC
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




void CDlgHeightValue::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if(wglGetCurrentContext()!=NULL)
	{
		wglMakeCurrent(NULL,NULL);
	}
	if(this->m_hGLContext!=NULL)
	{
		wglDeleteContext(this->m_hGLContext);
		this->m_hGLContext = NULL;
	}
}


void CDlgHeightValue::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// 	GLsizei width,height;
	// 	GLdouble aspect;
	// 	width = cx;
	// 	height = cy;
	// 	if(cy==0)
	// 	{
	// 		aspect = (GLdouble)width;
	// 	}
	// 	else
	// 	{
	// 		aspect = (GLdouble)width/(GLdouble)height;
	// 	}
	// 	glViewport(0,0,width,height);
	// 	glMatrixMode(GL_PROJECTION);
	// 	glLoadIdentity();
	// 	gluOrtho2D(0.0,500.0*aspect,0.0,500.0);
	// 	glMatrixMode(GL_MODELVIEW);
	// 	glLoadIdentity();

	GLsizei width,height;
	width = cx;
	height = cy;
	if (height==0)    // Prevent A Divide By Zero By
	{
		height=1;     // Making Height Equal One
	}
	glViewport(0,0,width,height);   // Reset The Current Viewport
	// 	glMatrixMode(GL_PROJECTION);   // Select The Projection Matrix
	// 	glLoadIdentity();       // Reset The Projection Matrix
	// 	// Calculate The Aspect Ratio Of The Window
	// 
	// 
	// 	double length = GetLength();
	// 	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f, 1000.0f);//透视投影
	// 	glMatrixMode(GL_MODELVIEW);   // Select The Modelview Matrix
	// 	glLoadIdentity();     // Reset The Modelview Matrix
	//	GetClientRect(&m_rcClient);
//	m_Camera.ResizeWindow();
}

int CDlgHeightValue::DrawSurface(GLvoid)
{

	glClearColor(0.0f, 0.0f,0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


// 	double length = GetLength();
// 	m_Camera.SetViewVolume(length, length, -length/2, length/2);
// 	m_Camera.Apply();

	glMatrixMode(GL_PROJECTION);   // Select The Projection Matrix
	glLoadIdentity();       // Reset The Projection Matrix
	// Calculate The Aspect Ratio Of The Window

 	double length = GetLength();
 	gluPerspective(45.0f,(GLfloat)m_width/(GLfloat)m_height,0.1f, length * 1.5f);//透视投影

	gluLookAt(0.0f, 0.0f, length, 0,0,0, 0, 1, 0);

	////////////////////////////////////////////////////////////////////////////////////////////////	

	//save the view state information , project matrix model matrix
	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX, m_ProjMatrix);
	glGetIntegerv(GL_VIEWPORT, m_Viewport);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glScalef(m_ScalFactor,m_ScalFactor,m_ScalFactor);
	glRotatef(sceneRot[0], 1.0, 0, 0);
	glRotatef(sceneRot[1], 0, 1.0, 0);
	glRotatef(sceneRot[2], 0, 0, 1.0);

	glTranslatef(-m_SceneCenter.x, -m_SceneCenter.y, -m_SceneCenter.z);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_ModelMatrix);




	if (m_bReloading || m_vecZCoordListAfterFilter.empty())
	{
		SwapBuffers(wglGetCurrentDC());
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	float max = prMax.z;
	float min = prMin.z;

	for(int i=1;i<m_height- 1;i++)
	{	
		glBegin(GL_TRIANGLE_STRIP);
		for(int j=1;j<m_width-1;j++)
		{
			DrawVertex(i,j);
			DrawVertex(i+1,j);
			DrawVertex(i,j+1);
			DrawVertex(i+1,j+1);
		}
		glEnd();
	}

	float minX = prMin.x;
	float minY = prMin.y;
	float maxX = prMax.x;
	float maxY = prMax.y;

	if(mouseleftdown && m_bShowSlice)
	{			
		// 		glBegin(GL_LINES);
		// 		PNT tmppt;
		// 		CPoint ppt;
		// 		ppt.x=PointerPosX-StartX;
		// 		ppt.y=PointerPosY-StartY;
		// 		tmppt= GetScreenPnt(ppt);
		// 		glColor3f(1.0,0,0);
		// 		glVertex3d(minX-(maxX+minX)/2,tmppt.m[1],fabs(max));
		// 		glVertex3d(maxX-(maxX+minX)/2,tmppt.m[1],fabs(max));
		// 		glColor3f(0,1.0,0);
		// 		glVertex3d(tmppt.m[0],minY-(maxY+minY)/2,fabs(max));
		// 		glVertex3d(tmppt.m[0],maxY-(maxY+minY)/2,fabs(max));
		// 		glEnd();
	}






	// enable blending
	glEnable(GL_BLEND);
	// enable read-only depth buffer
	glDepthMask(GL_FALSE);
	// set the blend Calculator to what we use for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// set back to normal depth buffer mode (writable)
	glDepthMask(GL_TRUE);
	// disable blending
	glDisable(GL_BLEND);
	SwapBuffers(wglGetCurrentDC());

	return 0;
}

void CDlgHeightValue::DrawVertex(int rows, int cols)
{
	float x		= static_cast<float>(cols);
	float y		= static_cast<float>(rows);
	float z		= m_vecZCoordListAfterFilter[(rows*m_width+cols)];
	float tt	= 256/(prMax.z-prMin.z);
	float media	= (z-prMin.z)*tt;
	int colorIdx= (255-(int)media) * 4;
	if(colorIdx<0)
	{
		colorIdx = 0;
	}
	else if(colorIdx>256*4)
	{
		colorIdx = 97;
	}

	glColor3f(land[colorIdx],land[colorIdx+1],land[colorIdx+2]);
	glVertex3f( x, y, z);
}

void CDlgHeightValue::SetInput(const std::vector<float>& input, int width, int height)
{
	m_width = width;
	m_height = height;
	this->TopLeft.m[0]		= prMin.x = 1.0;
	this->TopLeft.m[1]		= prMin.y = 1.0;
	this->BottomRight.m[0]	= prMax.x = m_width;
	this->BottomRight.m[1]	= prMax.y = m_height;

	m_vecZCoordList.assign(input.begin(), input.end());
	m_vecZCoordListAfterFilter.assign(input.begin(), input.end());
	auto minmaxZ = std::minmax_element(m_vecZCoordList.begin(), m_vecZCoordList.end(),
		[&](float lhs, float rhs) ->bool
	{
		return lhs < rhs;
	});
	this->TopLeft.m[2]		= m_vecZCoordList[0];
	this->BottomRight.m[2]	= m_vecZCoordList[m_vecZCoordList.size() - 1];

	prMax.z = (*minmaxZ.second);
	prMin.z = (*minmaxZ.first);

// 	this->m_SceneCenter = (prMax+prMin) / 2;
// 	this->m_SceneVolume = (prMax-prMin) / 2;

	GetSurRect();
}


void CDlgHeightValue::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	//刷新窗口 
	pWnd=GetDlgItem(IDC_SURFACESHOW);//一定要再次得到指针才能更新，否则会出错
	m_ppDC=pWnd->GetDC();
	wglMakeCurrent(m_ppDC->GetSafeHdc(), this->m_hGLContext);
	DrawSurface();

	pWnd=GetDlgItem(IDC_XSLICE);//一定要再次得到指针才能更新，否则会出错
	m_sliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	DrawXSlice();
	pWnd=GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错
	m_sliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	DrawYSlice();


}

void CDlgHeightValue::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	mouseleftdown = TRUE;
	mouseprevpoint.x = point.x;
	mouseprevpoint.y = point.y;
	CDialog::OnLButtonDown(nFlags, point);
}
void CDlgHeightValue::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	mouseleftdown = FALSE;
	SetSceneRot(0, point.y - mouseprevpoint.y, TRUE, TRUE);
	SetSceneRot(2, point.x - mouseprevpoint.x, TRUE, TRUE);
	CDialog::OnLButtonUp(nFlags, point);
}
void CDlgHeightValue::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bShowSlice)
	{
		sceneRot[0] = 0;
		sceneRot[0] = 0;
		sceneRot[0] = 1.0;

		if(point.x>SurRect.left && point.x<SurRect.right+5 &&
			point.y>SurRect.top && point.y<SurRect.bottom+5 )
		{
			int i=1;
			PointerPosX=point.x;
			PointerPosY=point.y;
		}

		RefreshAll();
		return;
	}


	if (mouserightdown)
	{
		SetSceneRot(1, -(point.x - mouseprevpoint.x), TRUE, TRUE);
	}
	else if(mouseleftdown)
	{
		SetSceneRot(0, point.y - mouseprevpoint.y, TRUE, TRUE);
		SetSceneRot(2, point.x - mouseprevpoint.x, TRUE, TRUE);
	}

	mouseprevpoint.x = point.x;
	mouseprevpoint.y = point.y;
	CDialog::OnMouseMove(nFlags, point);
}
void CDlgHeightValue::SetSceneRot(int axis, int value, BOOL increment, BOOL apply)
{
	if (increment)
	{
		sceneRot[axis] += (sceneRot[axis] >= 360) ? (-360.0f + value / 2.0f) : value/2.0f;
	}
	else
	{
		sceneRot[axis] = (sceneRot[axis] >= 360) ? (-360.0f + value / 2.0f) : value/2.0f;
	}
}

BOOL CDlgHeightValue::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	m_ScalFactor *= (1.0f + (float)zDelta*0.0004f);
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CDlgHeightValue::PreTranslateMessage(MSG* pMsg)
{
	// 	if (pMsg->message == WM_MOUSEWHEEL)
	// 	{
	// 		SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
	// 	}

	return CDialog::PreTranslateMessage(pMsg);
}

double CDlgHeightValue::GetLength() const
{
	return Distance(prMax,prMin);
}


void CDlgHeightValue::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_HEIGHT_VALUE, point.x, point.y, this, TRUE);
}


void CDlgHeightValue::OnHeightvalueShowslice()
{

}

void CDlgHeightValue::GetMinMaxValue(int& minX, int& minY, int &minZ, int& maxX, int& maxY, int& maxZ) const
{
	minX = prMin.x; 
	minY = prMin.y;
	minZ = prMin.z;
	maxX = prMax.x; 
	maxY = prMax.y;
	maxZ = prMax.z;
}

BOOL CDlgHeightValue::InitializeOpenGLContent()
{
	pWnd=GetDlgItem(IDC_SURFACESHOW);
	m_ppDC=pWnd->GetDC();	

	PIXELFORMATDESCRIPTOR pfd;
	ASSERT(m_ppDC!=NULL);
	if (!SetWindowPixelFormat(m_ppDC->GetSafeHdc()))
	{
		return FALSE;
	}
	int n=::GetPixelFormat(m_ppDC->GetSafeHdc());
	::DescribePixelFormat(m_ppDC->GetSafeHdc(),n,sizeof(pfd),&pfd);
	this->m_hGLContext = wglCreateContext(m_ppDC->GetSafeHdc());
	wglMakeCurrent(m_ppDC->GetSafeHdc(), this->m_hGLContext); 

	wglUseFontBitmaps(wglGetCurrentDC(), 0, 256, 1000);
	glShadeModel(GL_SMOOTH);  
	glFrontFace (GL_CCW);
	glDepthFunc (GL_LESS);
	glEnable    (GL_DEPTH_TEST);
	return TRUE;
}

void CDlgHeightValue::DrawXSlice()
{
	float tmp=(float)(PointerPosY-SurRect.top)/SurRect.Height();
	if (tmp<0)
		tmp=0;
	if(tmp>1)
		tmp=1;
	int Ynum=tmp*(m_height-1);
	DrawSlice(IDC_XSLICE, TEXT("X Slice"), Ynum);
}

void CDlgHeightValue::DrawYSlice()
{
	float tmp=(float)(PointerPosX-SurRect.left)/SurRect.Width();
	if (tmp<0)
		tmp=0;
	if(tmp>1)
		tmp=1;
	int Xnum=tmp*(m_width-1);
	DrawSlice(IDC_YSLICE, TEXT("Y Slice"), Xnum);
}

void CDlgHeightValue::DrawSlice(UINT nID, CString strCoord, int pos)
{
	pWnd = GetDlgItem(nID);

	if (!pWnd)
	{
		ASSERT(FALSE);
	}

	CRect plotrect;
	pWnd->GetClientRect(&plotrect);

	m_sliceDC = pWnd->GetDC();
	m_sliceDC->TextOutW(plotrect.Width()/2-20,plotrect.top+plotrect.Height()/2-10,strCoord);


	m_sliceDC->SetBkColor(RGB(255,255,255));


	int		left=plotrect.left;
	int		wide=plotrect.Width ();

	int		top=plotrect.top;
	int		height=plotrect.Height ();

	if (nID == IDC_YSLICE)
	{
		left=plotrect.left +plotrect.Width ()*0.2;
		top=plotrect.top;
		wide=plotrect.Width ()*0.8;
		height=plotrect.Height();
	}

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

	pOldFont=(CFont*)m_sliceDC->SelectObject(pNewFont1.get());

	CString strTextOut;
	strTextOut.Format(TEXT("%.3f"), prMax.z);
	m_sliceDC->TextOut(left+wide/2-25,top-15,strTextOut);
	strTextOut.Format(TEXT("%.3f"), prMin.z);
	m_sliceDC->TextOut(left+wide/2-25,top+height+5,strTextOut);


	m_sliceDC->SelectObject(pOldFont);

	std::shared_ptr<CFont> pNewFont2= std::make_shared<CFont>();
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

	pOldFont=(CFont*)m_sliceDC->SelectObject(pNewFont2.get());

	strTextOut.Format(TEXT("%.3f"), prMin.x);
	m_sliceDC->TextOut(left+wide+15,top+height/2-20,strTextOut);

	strTextOut.Format(TEXT("%.3f"), prMax.x);
	m_sliceDC->TextOut(left-5,top+height/2-20,strTextOut);

	m_sliceDC->SelectObject(pOldFont);
	//////////////////////////////////////////////////////////////////////////////////



	if (nID == IDC_YSLICE)
	{
		CBrush *pOldBrush; 
		//画色块	
		for(int i=1;i<=34;i++)
		{
			std::shared_ptr<CBrush> pNewBrush= std::make_shared<CBrush>();
			if(pNewBrush->CreateSolidBrush(RGB(land[(i-1)*4*7]*255,land[(i-1)*4*7+1]*255,land[(i-1)*4*7+2]*255)))
			{
				pOldBrush=m_sliceDC->SelectObject(pNewBrush.get());
				m_sliceDC->Rectangle(plotrect.Width()/48,/*plotrect.Height()/6+*/(i-1)*plotrect.Height()/34,plotrect.Width()/6,plotrect.Height()/4.6+(i-1)*plotrect.Height()/34);			
				m_sliceDC->SelectObject(pOldBrush);
			}		
		}

		//画标尺
		m_sliceDC->MoveTo (plotrect.Width()/6+2,/*plotrect.Height()/9+*/plotrect.Height()/2/34);
		m_sliceDC->LineTo (plotrect.Width()/6+2,/*plotrect.Height()/9+*/33*plotrect.Height()/34+plotrect.Height()/2/34);

		for(int i=1;i<=34;i++)
		{
			m_sliceDC->MoveTo (plotrect.Width()/6+2,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/34+plotrect.Height()/2/34);
			if(i%2!=0)
				m_sliceDC->LineTo (plotrect.Width()/6+7,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/34+plotrect.Height()/2/34);
			else
				m_sliceDC->LineTo (plotrect.Width()/6+5,/*plotrect.Height()/9+*/(i-1)*plotrect.Height()/34+plotrect.Height()/2/34);
		}			  
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}



	//画网格
	CPen newPen;
	newPen.CreatePen(PS_DOT,1,RGB(10,10,60));//20,20,80));
	pOldPen=m_sliceDC->SelectObject(&newPen);

	int rows = nID == IDC_XSLICE ? 11 : 5;
	int cols = nID == IDC_XSLICE ? 5: 11;

	for(int i=1;i<=rows;i++)
	{
		m_sliceDC->MoveTo (left +(i-1)*wide/(rows - 1),top);
		m_sliceDC->LineTo (left+(i-1)*wide/(rows - 1),top+height);
	}

	for(int i=1;i<=cols;i++)
	{
		m_sliceDC->MoveTo (left ,top+(i-1)*height/(cols - 1));
		m_sliceDC->LineTo (left+ wide,top+(i-1)*height/(cols - 1));
	}

	m_sliceDC->SelectObject(pOldPen);

	m_sliceDC->MoveTo (left,top+height/2);
	m_sliceDC->LineTo (left+wide,top+height/2);
	m_sliceDC->MoveTo (left +wide/2,top);
	m_sliceDC->LineTo (left+wide/2,top+height);

	if (m_bShowSlice)
	{
		if (nID == IDC_XSLICE)
		{
			if(pos>0 && pos <=m_height)
			{
				m_sliceDC->MoveTo(left,top +m_height- (m_vecZCoordList[(m_height-1-pos)*m_width]-prMin.z)/(prMax.z-prMin.z)*m_height);		   		
				for(int j=1;j<m_width-1;j++)
				{
					int		X=left + j*wide/(m_width-2);
					int     Y=top +m_height- (m_vecZCoordList[((m_height-1-pos)*m_width+j)]-prMin.z)/(prMax.z-prMin.z)*m_height;
					if(j>1)
					{		 
						m_sliceDC->LineTo (X,Y);
					}
				}
			}
		}
		else
		{
			if (pos>0 && pos<=m_width)
			{
				m_sliceDC->MoveTo(left + (m_vecZCoordList[pos]-prMin.z)/(prMax.z-prMin.z)*wide,top);	
				for(int j=1;j<m_height-1;j++)
				{              
					int		X=left + (m_vecZCoordList[((m_height-1-j)*m_width+pos)]-prMin.z)/((prMax.z-prMin.z))*wide;
					int     Y=top+j*height/(m_height-2);
					if(j>1)
					{		 
						m_sliceDC->LineTo (X,Y);
					}
				}
			}
		}		
	}
	m_sliceDC->SelectObject (pOldPen);
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
	TopLeft=tmp1;
	BottomRight=tmp2;
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

void CDlgHeightValue::RefreshAll()
{
	//刷新窗口 
	pWnd=GetDlgItem(IDC_SURFACESHOW);//一定要再次得到指针才能更新，否则会出错
	m_ppDC=pWnd->GetDC();
	wglMakeCurrent(m_ppDC->GetSafeHdc(), this->m_hGLContext);
	DrawSurface();

	pWnd=GetDlgItem(IDC_XSLICE);//一定要再次得到指针才能更新，否则会出错
	m_sliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	DrawXSlice();
	pWnd=GetDlgItem(IDC_YSLICE);//一定要再次得到指针才能更新，否则会出错
	m_sliceDC=pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	DrawYSlice();
}

void CDlgHeightValue::OnTimer(UINT_PTR nIDEvent)
{
	//InvalidateRect(NULL);
	CDialog::OnTimer(nIDEvent);
}


BOOL CDlgHeightValue::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}
