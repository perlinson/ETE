// DlgPosController.cpp : implementation file
//

#include "stdafx.h"
#include "DlgPosController.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "PosController.h"
#include "MeasureManager.h"

// CDlgPosController dialog

const int AxisX = 0;
const int AxisY = 1;
const int AxisZ = 2;
IMPLEMENT_DYNAMIC(CDlgPosController, CDialog)

CDlgPosController::CDlgPosController(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPosController::IDD, pParent),
	m_bMouseLeftDown(FALSE)
{

}

CDlgPosController::~CDlgPosController()
{
	
}

void CDlgPosController::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_MOVE_XY_FAST, m_btnRadioMoveXYFast);
	DDX_Control(pDX, IDC_RADIO_MOVE_XY_MED, m_btnRadioMoveXYMed);
	DDX_Control(pDX, IDC_RADIO_MOVE_XY_SLOW, m_btnRadioMoveXYSlow);
	DDX_Control(pDX, IDC_RADIO_MOVE_Z_FAST, m_btnRadioMoveZFast);
	DDX_Control(pDX, IDC_RADIO_MOVE_Z_MED, m_btnRadioMoveZMed);
	DDX_Control(pDX, IDC_RADIO_MOVE_Z_SLOW, m_btnRadioMoveZSlow);
	DDX_Control(pDX, IDC_WHITE, m_btnRadioWhiteLight);
	DDX_Control(pDX, IDC_BLUE, m_btnRadioBlueLight);
	DDX_Control(pDX, IDC_SLIDER_LIGHT_STRENGTH, m_btnSliderLightStrength);
	DDX_Control(pDX, IDC_MOVE_XY, m_staticMoveXY);
	DDX_Control(pDX, IDC_MOVE_Z, m_staticMoveZ);
}


BEGIN_MESSAGE_MAP(CDlgPosController, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND_RANGE(IDC_RADIO_MOVE_Z_FAST, IDC_RADIO_MOVE_Z_SLOW, OnButtonRadioMoveZ)
	ON_COMMAND_RANGE(IDC_RADIO_MOVE_XY_FAST, IDC_RADIO_MOVE_XY_SLOW, OnButtonRadioMoveXY)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LIGHT_STRENGTH, &CDlgPosController::OnNMCustomdrawSliderLightStrength)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()


// CDlgPosController message handlers


BOOL CDlgPosController::OnInitDialog()
{
	CDialog::OnInitDialog();

	BOOL b32BitIcons = TRUE;

	if (afxGlobalData.m_nBitsPerPixel < 16)
	{
		b32BitIcons = FALSE;
	}

	// TODO:  Add extra initialization here
	m_btnRadioMoveXYFast.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_btnRadioMoveXYMed.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_btnRadioMoveXYSlow.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_btnRadioMoveZFast.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_btnRadioMoveZMed.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_btnRadioMoveZSlow.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_btnRadioWhiteLight.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_btnRadioBlueLight.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_btnRadioMoveXYFast.SetImage(b32BitIcons ? IDB_RADIO_OFF32 : IDB_RADIO_OFF);
	m_btnRadioMoveXYMed.SetImage(b32BitIcons ? IDB_RADIO_OFF32 : IDB_RADIO_OFF);
	m_btnRadioMoveXYSlow.SetImage(b32BitIcons ? IDB_RADIO_OFF32 : IDB_RADIO_OFF);
	m_btnRadioMoveZFast.SetImage(b32BitIcons ? IDB_RADIO_OFF32 : IDB_RADIO_OFF);
	m_btnRadioMoveZMed.SetImage(b32BitIcons ? IDB_RADIO_OFF32 : IDB_RADIO_OFF);
	m_btnRadioMoveZSlow.SetImage(b32BitIcons ? IDB_RADIO_OFF32 : IDB_RADIO_OFF);
	m_btnRadioWhiteLight.SetImage(b32BitIcons ? IDB_RADIO_OFF32 : IDB_RADIO_OFF);
	m_btnRadioBlueLight.SetImage(b32BitIcons ? IDB_RADIO_OFF32 : IDB_RADIO_OFF);


	m_btnRadioMoveXYFast.SetCheckedImage(b32BitIcons ? IDB_RADIO_ON32 : IDB_RADIO_ON);
	m_btnRadioMoveXYMed.SetCheckedImage(b32BitIcons ? IDB_RADIO_ON32 : IDB_RADIO_ON);
	m_btnRadioMoveXYSlow.SetCheckedImage(b32BitIcons ? IDB_RADIO_ON32 : IDB_RADIO_ON);
	m_btnRadioMoveZFast.SetCheckedImage(b32BitIcons ? IDB_RADIO_ON32 : IDB_RADIO_ON);
	m_btnRadioMoveZMed.SetCheckedImage(b32BitIcons ? IDB_RADIO_ON32 : IDB_RADIO_ON);
	m_btnRadioMoveZSlow	.SetCheckedImage(b32BitIcons ? IDB_RADIO_ON32 : IDB_RADIO_ON);
	m_btnRadioWhiteLight.SetCheckedImage(b32BitIcons ? IDB_RADIO_ON32 : IDB_RADIO_ON);
	m_btnRadioBlueLight.SetCheckedImage(b32BitIcons ? IDB_RADIO_ON32 : IDB_RADIO_ON);


	m_btnRadioMoveXYSlow.SetCheck(TRUE);
	m_btnRadioMoveZSlow.SetCheck(TRUE);
	m_btnRadioWhiteLight.SetCheck(TRUE);

	m_btnSliderLightStrength.SetRange(0, 100);
	m_btnSliderLightStrength.SetPos(25);

	SetTimer(1,100,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgPosController::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	
	
}


void CDlgPosController::OnLButtonDown(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	m_bMouseLeftDown = TRUE;
	float xMove(0.0f), yMove(0.0f), zMove(0.0f);
	std::tr1::tie(xMove, yMove, zMove) = CalcluateXYZOffset(point);
	PosController::GetInstance()->MultiMove(xMove, yMove, zMove);
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgPosController::moveSeleteDistanceForward()
{
	PosController::GetInstance()->SetZSpeed(0.1f);
	PosController::GetInstance()->MultiMove(0.0, 0.0, 50);

}

void CDlgPosController::moveSeleteDistanceBackward()
{
	PosController::GetInstance()->SetZSpeed(0.1f);
	PosController::GetInstance()->MultiMove(0.0, 0.0, -50);

}

void CDlgPosController::Stop()
{
	PosController::GetInstance()->stop_All();
}

void CDlgPosController::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseLeftDown = FALSE;
	PosController::GetInstance()->stop_All();
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgPosController::CheckMoveInXYRegion(CPoint point)
{
	CStatic* pStaticMoveXY = (CStatic*)GetDlgItem(IDC_MOVE_XY);
	CRect rcClient;
	pStaticMoveXY->GetClientRect(&rcClient);
	pStaticMoveXY->ClientToScreen(&rcClient);
	if (rcClient.PtInRect(point))
	{
		AfxMessageBox(TEXT("Point in Move XY"));
	}
}

void CDlgPosController::CheckMoveInZRegion(CPoint point)
{
	CStatic* pStaticMoveZ = (CStatic*)GetDlgItem(IDC_MOVE_Z);
	CRect rcClient;
	pStaticMoveZ->GetClientRect(&rcClient);
	pStaticMoveZ->ClientToScreen(&rcClient);

	if (rcClient.PtInRect(point))
	{
		AfxMessageBox(TEXT("Point in Move Z"));
	}


}

std::tr1::tuple<float, float, float>
CDlgPosController::CalcluateXYZOffset( CPoint point )
{
	m_staticMoveXY.GetClientRect(&m_rcClientMoveXY);
	m_staticMoveXY.ClientToScreen(&m_rcClientMoveXY);
	m_ptCenterMoveXY = m_rcClientMoveXY.CenterPoint();

	m_staticMoveZ.GetClientRect(&m_rcClientMoveZ);
	m_staticMoveZ.ClientToScreen(&m_rcClientMoveZ);
	m_ptCenterMoveZ = m_rcClientMoveZ.CenterPoint();

	float percentOfX(0.0f), percentOfY(0.0f), percentOfZ(0.0f);
	if (m_rcClientMoveXY.PtInRect(point))
	{
		//Move x, y

		int xOffset = m_ptCenterMoveXY.x - point.x;
		int yOffset = m_ptCenterMoveXY.y - point.y;

		percentOfX = (float)xOffset / ((float)m_rcClientMoveXY.Width() / 2.0f);
		percentOfY = (float)yOffset / ((float)m_rcClientMoveXY.Height() / 2.0f);
	}
	else if(m_rcClientMoveZ.PtInRect(point))
	{
		int zOffset = point.y - m_ptCenterMoveZ.y;
		percentOfZ = (float)zOffset / ((float)m_rcClientMoveZ.Height() / 2.0f);
	}

	return std::tr1::make_tuple(percentOfX, percentOfY, percentOfZ);
}


BOOL CDlgPosController::OnEraseBkgnd(CDC* pDC)
{
	CDC* pDCStatic = m_staticMoveXY.GetDC();
	pDCStatic->MoveTo( 0,m_rcClientMoveXY.Height() / 2);
	pDCStatic->LineTo( m_rcClientMoveXY.Width(),m_rcClientMoveXY.Height() / 2);

	pDCStatic->MoveTo( m_rcClientMoveXY.Width() / 2,0);
	pDCStatic->LineTo( m_rcClientMoveXY.Width() / 2,m_rcClientMoveXY.Height());
	//m_staticMoveXY.ReleaseDC(pDCStatic);

	pDCStatic = m_staticMoveZ.GetDC();
	pDCStatic->MoveTo( m_rcClientMoveZ.Width() / 2,0);
	pDCStatic->LineTo( m_rcClientMoveZ.Width() / 2,m_rcClientMoveZ.Height());
	pDCStatic->MoveTo( m_rcClientMoveZ.Width() / 2,0);
	pDCStatic->LineTo( m_rcClientMoveZ.Width() / 2,m_rcClientMoveZ.Height());
	return TRUE;
	//m_staticMoveZ.ReleaseDC(pDCStatic);
	//return CDialog::OnEraseBkgnd(pDC);
}

void CDlgPosController::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case 1:
		SetCurrPosition();
		break;
	}



	CDialog::OnTimer(nIDEvent);
}


void CDlgPosController::OnDestroy()
{
	CDialog::OnDestroy();

	PosController::GetInstance()->stop_All();
	PosController::GetInstance()->Close();
	KillTimer(1);

	PosController::GetInstance()->Destroy();
}

void CDlgPosController::OnButtonRadioMoveZ(UINT uID)
{
	switch(uID)
	{
	case IDC_RADIO_MOVE_Z_FAST:
		{
			PosController::GetInstance()->SetZSpeed(100.0f);
		}
		break;
	case IDC_RADIO_MOVE_Z_MED:
		{
			PosController::GetInstance()->SetZSpeed(5.0f);
		}
		break;
	case IDC_RADIO_MOVE_Z_SLOW:
		{
			PosController::GetInstance()->SetZSpeed(0.1f);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CDlgPosController::OnButtonRadioMoveXY(UINT uID)
{
	switch(uID)
	{
	case IDC_RADIO_MOVE_XY_FAST:
		{
			PosController::GetInstance()->SetXYSpeed(100);
		}
		break;
	case IDC_RADIO_MOVE_XY_MED:
		{
			PosController::GetInstance()->SetXYSpeed(10);
		}
		break;
	case IDC_RADIO_MOVE_XY_SLOW:
		{
			PosController::GetInstance()->SetXYSpeed(1);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void CDlgPosController::OnMouseMove(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	if ( !m_bMouseLeftDown )
	{
		return;
	}
	if (!m_rcClientMoveXY.PtInRect(point) && !m_rcClientMoveZ.PtInRect(point))
	{
		PosController::GetInstance()->stop_All();
	}
	CDialog::OnMouseMove(nFlags, point);
}


void CDlgPosController::OnNMCustomdrawSliderLightStrength(NMHDR *pNMHDR, LRESULT *pResult)
{
 	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
 	
 	int strength = m_btnSliderLightStrength.GetPos();
 
 	if (IsDlgButtonChecked(IDC_WHITE))
 	{
 		pMeasureManager->SetWhiteStrength(strength);
 	}
 	else
 	{
 		pMeasureManager->SetBlueStrength(strength);
 	}

	*pResult = 0;
}

void CDlgPosController::SetCurrPosition()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pMainFrame->SetCurrPosition(PosController::GetInstance()->GetCurrZPosition());
}