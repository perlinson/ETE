// VSIState.cpp: implementation of the CVSIState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VSIModel.h"
#include "ConfocalModel.h"
#include "VSIModel.h"
#include "MeasureManager.h"
#include "XMTDev.h"
#include "MainFrm.h"
#include "ArmDev.h"
#include "Calculator.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

VSIModel VSIModel::m_VSIState;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VSIModel::VSIModel()
{

}

VSIModel::~VSIModel()
{
}

VSIModel* VSIModel::Instance()
{
	return ( &m_VSIState );
}

void VSIModel::Resize()
{
}

void VSIModel::Init(CMeasureManager * pManager)
{
	CModelBase::Init(pManager);
	pManager->ConfigGrayCamera();
	pManager->ConfigARM();
	pManager->ShowGrayCameraOnly();
	CDlgImgShow::GetInstance()->Used();

}

void VSIModel::ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera )
{
	if (m_changeLiveViewAtRuntime)
	{
		grayCamera.TriggerMode = CAMERA_TRIGGLE_MODE_EXTERNAL;
	}
	else
	{
		grayCamera.TriggerMode = CAMERA_TRIGGLE_MODE_INTERNAL;
	}

	wcscpy_s(grayCamera.RecordInfo.Directory, TEXT("F:\\Result\\VSI\\"));
	wcscpy_s(grayCamera.RecordInfo.NamePrefix, TEXT("Result_Gray"));
	grayCamera.RecordInfo.FrameCount = 0;
	grayCamera.RecordInfo.Divider = 0;
}

void VSIModel::ConfigARMParameter( ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar )
{
	stArmConsoleInfPar.iPriodTime = 0;
	stArmConsoleInfPar.iDelayTime = 0;
}

void VSIModel::ConfigXMTParameter( CMeasureManager* pManager )
{

}

void VSIModel::StartMeasure( CMeasureManager* pManager )
{
 	SetRuntimeChangeLiveView(TRUE);
 	pManager->ConfigGrayCamera();

	int scanRange = m_pMainFrame->GetScanRange();
	float dfScanStep = m_pMainFrame->GetScanStep();
	Calculator::GetInstance()->SetStepCount(scanRange);
	ArmDev::GetInstance()->SetTriggerPeriod(20);
	ArmDev::GetInstance()->SetTriggerNum(scanRange);
	ArmDev::GetInstance()->SetNewTriggerEnter();
	ArmDev::GetInstance()->StartNewTrigger();
	pManager->SetFrameIndexDiffBetweenPic(CAMERA_GRAY);
	pManager->ResetFrameIndexForEveryMaskPic(CAMERA_GRAY);
	pManager->EnableRecordFrame(CAMERA_GRAY, TRUE, FALSE);

	float fengfengzhi = m_pMainFrame->GetFengFengZhi();
	float pianzhi	  = m_pMainFrame->GetPianZhi();
	float pinlv		  = m_pMainFrame->GetPinLv();
	float waittime    = m_pMainFrame->GetWaitTime();
	pManager->XMTInstance()->SendMultiPosCmd(fengfengzhi, pinlv, pianzhi);

	//the newest method

	SleepMs(waittime);

	pManager->XMTInstance()->Stop();

	ArmDev::GetInstance()->SetNewTriggerLeave();

	
	pManager->EnableRecordFrame(CAMERA_GRAY, FALSE, FALSE);
	pManager->ResetFrameIndex(CAMERA_GRAY);

	Calculator::GetInstance()->VSIMode(FALSE);
	SetRuntimeChangeLiveView(FALSE);
	pManager->ConfigGrayCamera();


	// use exsited data for test
	//Calculator::GetInstance()->VSIMode(TRUE);
	//Calculator::GetInstance()->Init(780, 580);
	//Calculator::GetInstance()->SetStepCount(36);
	//Calculator::GetInstance()->PushCustomData();
 

	////the early version
	//int scanRange = m_pMainFrame->GetScanRange();
	//float dfScanStep = m_pMainFrame->GetScanStep();
	//SetCurrentStep(dfScanStep);
	//SetCalcPictureCount(scanRange);
	//pManager->SetFrameIndexDiffBetweenPic(CAMERA_GRAY);
	//ArmDev::GetInstance()->SetTriggerNum(scanRange);
	//ArmDev::GetInstance()->SetTriggerPeriod(20);
	//ArmDev::GetInstance()->StartTrigger();
	//Calculator::GetInstance()->SetStepCount(scanRange);
	//pManager->EnableRecordFrame(CAMERA_GRAY, TRUE, FALSE);
	//pManager->ResetFrameIndexForEveryMaskPic(CAMERA_GRAY);

	//for (int i = 0; i < scanRange ; i++)
	//{
	//	pManager->XMTInstance()->SendSinglePosCmd(LOOP_CLOSE, 1, dfScanStep * i);
	//	SleepMs(20);
	//}

	//SleepMs(100);

	//pManager->EnableRecordFrame(CAMERA_GRAY, FALSE, FALSE);


	//for (int i = 0; i < scanRange ; i++)
	//{
	//	pManager->XMTInstance()->SendSinglePosCmd(LOOP_CLOSE, 1, dfScanStep * (scanRange - i));
	//	SleepMs(10);
	//}


	//pManager->ResetFrameIndex(CAMERA_GRAY);
}

void VSIModel::Calculate(CMeasureManager* pManager)
{
	Calculator::GetInstance()->Calculate();
}