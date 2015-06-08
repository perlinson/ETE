// PSIState.cpp: implementation of the CPSIState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PSIModel.h"
#include "ConfocalModel.h"
#include "VSIModel.h"
#include "MeasureManager.h"
#include "XMTDev.h"
#include "cameraview.h"
#include "StereoView.h"
#include "DlgResult.h"
#include "Calculator.h"
#include "ArmDev.h"
#include "MainFrm.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

PSIModel PSIModel::m_PSIState;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PSIModel::PSIModel()
{

}

PSIModel::~PSIModel()
{
}

void PSIModel::Calculate(CMeasureManager* pManager)
{
	Calculator::GetInstance()->Calculate();
}

PSIModel* PSIModel::Instance()
{
	return ( &m_PSIState );
}


void PSIModel::Resize()
{
}

void PSIModel::Init(CMeasureManager * pManager)
{
	CModelBase::Init(pManager);
	pManager->ConfigGrayCamera();
	pManager->ShowGrayCameraOnly();
	CDlgImgShow::GetInstance()->Used();
	pManager->ConfigARM();
}

void PSIModel::ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera ) 
{
	//how to Modify Parameter

	if (m_changeLiveViewAtRuntime)
	{
		grayCamera.TriggerMode = CAMERA_TRIGGLE_MODE_EXTERNAL;
	}
	else
	{
		grayCamera.TriggerMode = CAMERA_TRIGGLE_MODE_INTERNAL;
	}
	wcscpy_s(grayCamera.RecordInfo.Directory, TEXT("F:\\Result\\PSI\\"));
	wcscpy_s(grayCamera.RecordInfo.NamePrefix, TEXT("Result_Gray"));
	grayCamera.RecordInfo.FrameCount = 0;
	grayCamera.RecordInfo.Divider = 0;
}

void PSIModel::ConfigARMParameter( ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar )
{
	stArmConsoleInfPar.iDelayTime = 50;
	stArmConsoleInfPar.iPriodTime = 40;
	stArmConsoleInfPar.iPulseNum = 1;
}

void PSIModel::ConfigXMTParameter( CMeasureManager* pManager )
{
	
}

void PSIModel::StartMeasure( CMeasureManager* pManager )
{
	//SetRuntimeChangeLiveView(TRUE);
	//pManager->ConfigGrayCamera();

	////Calculator::GetInstance()->PSIMode(TRUE);
	////Calculator::GetInstance()->Init(780, 580);
	////Calculator::GetInstance()->SetStepCount(5);
	////Calculator::GetInstance()->PushCustomData();
	////quick test



 //	LOOP_STATUS iLoopMode  = LOOP_CLOSE;
 //	pManager->XMTInstance()->SendSinglePosCmd(iLoopMode, 1, 0);	

	//Calculator::GetInstance()->PSIMode(TRUE);
	//Calculator::GetInstance()->SetStepCount(5);

	////old move style

	////float curPos = 0.0f;
	////float iPos = 0.18f;
	////for (int i = 0; i < 5 ; i++)
	////{
	////	pManager->XMTInstance()->SendSinglePosCmd(iLoopMode, 1, (float)iPos*(i+1) + (float)curPos);	
	////	SleepMs(150);
	////}

	//pManager->EnableRecordFrame(CAMERA_GRAY, FALSE, FALSE);
	////
	////
	////



	//pManager->SetFrameIndexDiffBetweenPic(CAMERA_GRAY);
	//ArmDev::GetInstance()->SetTriggerNum(5);
	//ArmDev::GetInstance()->SetTriggerPeriod(20);
	//ArmDev::GetInstance()->StartTrigger();

 //	//
 //	pManager->EnableRecordFrame(CAMERA_GRAY, TRUE, FALSE);
 //
 //	float curPos = 0.0f;
 //	float iPos = 0.18f;
 //	for (int i = 0; i < 5.0/1.55 ; i++)
 //	{
 //		pManager->XMTInstance()->SendSinglePosCmd(iLoopMode, 1, (float)iPos*(i+1)*1.55 + (float)curPos);	
 //		SleepMs(20);
 //	}
	//pManager->EnableRecordFrame(CAMERA_GRAY, FALSE, FALSE);
	//SleepMs(40);
	//for (int i = 0; i < 5 ; i++)
	//{
	//	pManager->XMTInstance()->SendSinglePosCmd(iLoopMode, 1, curPos - (float)iPos*(i+1));	
	//}



	//pManager->ResetFrameIndex(CAMERA_GRAY);
	////²âÁ¿½áÊø


// 	SetRuntimeChangeLiveView(TRUE);
// 	pManager->ConfigGrayCamera();

	Calculator::GetInstance()->PSIMode(TRUE);
	Calculator::GetInstance()->SetStepCount(20);

	pManager->SetFrameIndexDiffBetweenPic(CAMERA_GRAY);
	ArmDev::GetInstance()->SetTriggerNum(20);
	SleepMs(20);
	ArmDev::GetInstance()->SetTriggerPeriod(20);
	SleepMs(20);
	ArmDev::GetInstance()->SetNewTriggerEnter();
	SleepMs(20);
	ArmDev::GetInstance()->StartNewTrigger();
	SleepMs(20);

	pManager->EnableRecordFrame(CAMERA_GRAY, TRUE, FALSE);

	float fengfengzhi = m_pMainFrame->GetFengFengZhi(1);
	float pianzhi	  = m_pMainFrame->GetPianZhi(1);
	float pinlv		  = m_pMainFrame->GetPinLv(1);
	float waittime    = m_pMainFrame->GetWaitTime(1);
	pManager->XMTInstance()->SendMultiPosCmd(fengfengzhi, pinlv, pianzhi);

	SleepMs(waittime);

	pManager->XMTInstance()->Stop();

	ArmDev::GetInstance()->SetNewTriggerLeave();
	
	pManager->EnableRecordFrame(CAMERA_GRAY, FALSE, FALSE);
	pManager->ResetFrameIndex(CAMERA_GRAY);

// 	SetRuntimeChangeLiveView(FALSE);
// 	pManager->ConfigGrayCamera();
}