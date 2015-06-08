// ObserverState.cpp: implementation of the CObserverState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObserverModel.h"
#include "ConfocalModel.h"
#include "VSIModel.h"
#include "MeasureManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ObserverModel ObserverModel::m_ObserverState;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ObserverModel::ObserverModel()
{

}

ObserverModel::~ObserverModel()
{
}

ObserverModel* ObserverModel::Instance()
{
	return ( &m_ObserverState );
}

void ObserverModel::Init(CMeasureManager * pManager)
{
	CModelBase::Init(pManager);

	if (!pManager->CameraConfigured())
	{
		pManager->CameraConfigured(true);
		pManager->ConfigCamera();
	}

    CDlgImgShow::GetInstance()->Used();
	pManager->ShowGrayCameraOnly();
}

void ObserverModel::ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera )
{
	grayCamera.TriggerMode = CAMERA_TRIGGLE_MODE_INTERNAL;
}

void ObserverModel::ConfigARMParameter( ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar )
{
	stArmConsoleInfPar.iBraudRate = 19200;
	stArmConsoleInfPar.iDelayTime = 50;
	stArmConsoleInfPar.iPriodTime = 40;
	stArmConsoleInfPar.iPulseNum  = 1;
}
