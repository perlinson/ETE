// MeasureModel.cpp: implementation of the CMeasureModel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeasureManager.h"
#include "MeasureModel.h"
#include "MainFrm.h"
#include "DlgImageShow.h"
#include "DlgMeasureProgress.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CRect CModelBase::m_Rect;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelBase::CModelBase()
	:m_calcPictureNum(1),
	m_currentStep(0.0),
	m_changeLiveViewAtRuntime(FALSE),
	m_pMainFrame(nullptr)
{
	
}

CModelBase::~CModelBase()
{

}

void CModelBase::Init(CMeasureManager * pManager)
{
	if (!m_pMainFrame)
	{
		m_pMainFrame = (CMainFrame*)AfxGetMainWnd();
	}
}