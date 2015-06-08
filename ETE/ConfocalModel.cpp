// ConfocalState.cpp: implementation of the CConfocalState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfocalModel.h"
#include "ConfocalModel.h"
#include "VSIModel.h"
#include "MeasureManager.h"

#include "DlgImageShow.h"
#include "DlgMeasureProgress.h"
#include "MainFrm.h"
#include "cameraview.h"
#include "XMTDev.h"
#include "ArmDev.h"
#include "Calculator.h"
#include <fstream>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ConfocalModel ConfocalModel::m_ConfocalState;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConfocalModel::ConfocalModel()
{

}

ConfocalModel::~ConfocalModel()
{
}


void ConfocalModel::Calculate(CMeasureManager* pManager)
{
	Calculator::GetInstance()->Calculate();
	Calculator::GetInstance()->ConfocalMode(TRUE);
}

ConfocalModel* ConfocalModel::Instance()
{
	return ( &m_ConfocalState );
}


void ConfocalModel::Resize()
{
}

void ConfocalModel::Init(CMeasureManager * pManager)
{
	CModelBase::Init(pManager);
	pManager->ConfigGrayCamera();
	pManager->ShowGrayCameraOnly();
	CDlgImgShow::GetInstance()->UseMask();
}

void ConfocalModel::ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera ) 
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
	


	wcscpy_s(grayCamera.RecordInfo.Directory, TEXT("F:\\Result\\Confocal"));
	wcscpy_s(grayCamera.RecordInfo.NamePrefix, TEXT("Result_Gray"));
	grayCamera.RecordInfo.FrameCount = 0;
	grayCamera.RecordInfo.Divider = 0;

	wcscpy_s(colorCamera.RecordInfo.Directory, TEXT("F:\\Result\\Confocal"));
	wcscpy_s(colorCamera.RecordInfo.NamePrefix, TEXT("Result_Gray"));
	colorCamera.RecordInfo.FrameCount = 0;
	colorCamera.RecordInfo.Divider = 0;

}

void ConfocalModel::ConfigARMParameter( ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar )
{
	//modify the ARM
}

void ConfocalModel::ConfigXMTParameter( CMeasureManager* pManager )
{
	
}

void ConfocalModel::StartMeasure( CMeasureManager* pManager )
{	
	//SetRuntimeChangeLiveView(TRUE);
	//pManager->ConfigGrayCamera();

	//CDlgMeasureProgress dlg(AfxGetMainWnd());
	////ready for change to correct value
	//BOOL use3dView = m_pMainFrame->GetUse3dView();
	//CDlgImgShow::GetInstance()->Set3DViewModel(use3dView);
	//if (!use3dView)
	//{
	//	dlg.ScanRange(9);
	//	dlg.LoopCounter(9);
	//	CDlgImgShow::GetInstance()->LoopCounter(9);

	//	for (int i = 0; i < 9 ; i++)
	//	{
	//		CDlgImgShow::GetInstance()->Trigger();
	//		pManager->EnableRecordFrame(CAMERA_GRAY, TRUE);
	//		SleepMs(40);
	//	}
	//}
	//else
	//{
	//	int  scanRange = m_pMainFrame->GetScanRange();
	//	float dfScanStep = m_pMainFrame->GetScanStep();
	//	dlg.ScanRange(scanRange);
	//	dlg.LoopCounter(3);
	//	CDlgImgShow::GetInstance()->LoopCounter(3);
	//	Calculator::GetInstance()->ConfocalMode(TRUE);
	//	Calculator::GetInstance()->SetStepCount(scanRange);
	//	SetCurrentStep(dfScanStep );

	//	pManager->SetFrameIndexDiffBetweenPic(CAMERA_GRAY, MAX_MASK_COUNT);
	//	ArmDev::GetInstance()->SetTriggerPeriod(20);
	//	SleepMs(20);

	//	int posScanRange = scanRange;

	//	float *pPosArrRev = new float[posScanRange];
	//	float *pTimArr = new float[posScanRange];

	//	if(!pTimArr || !pPosArrRev)
	//		return;

	//	for (int i = 0; i < posScanRange ; ++i)
	//	{
	//		pPosArrRev[i] = (posScanRange -1 - i)*dfScanStep;
	//		pTimArr[i] = 2.0;//ms 为单位
	//	}
	//	
	//	for (int MaskIndex = 1; MaskIndex <= MAX_MASK_COUNT ; MaskIndex++)
	//		{
	//			
	//			ArmDev::GetInstance()->SetTriggerNum(scanRange);
	//			SleepMs(20);
	//			ArmDev::GetInstance()->SetTriggerEnter();
	//			SleepMs(20);
	//			ArmDev::GetInstance()->StartTrigger();
	//			pManager->EnableRecordFrame(CAMERA_GRAY, TRUE, FALSE);
	//			pManager->ResetFrameIndexForEveryMaskPic(CAMERA_GRAY, MaskIndex);
	//			for (int i = 0; i < posScanRange/1.55 ; i++)            // modified "/2"
	//			{
	//				pManager->XMTInstance()->SendSinglePosCmd(LOOP_CLOSE, 1, dfScanStep * i * 1.55);    //modified ''*2''
	//				
	//				SleepMs(20);
	//			}
	//			SleepMs(40);
	//			pManager->EnableRecordFrame(CAMERA_GRAY, FALSE, FALSE);

	//			for (int i = 0; i < posScanRange; i++)            // modified "/2"
	//			{
	//				pManager->XMTInstance()->SendSinglePosCmd(LOOP_CLOSE, 1, pPosArrRev[i]);    //modified ''*2''
	//			}

	//			CDlgImgShow::GetInstance()->Trigger();
	//			ArmDev::GetInstance()->SetTriggerLeave();
	//		}

	//	delete[] pPosArrRev;
	//	delete[] pTimArr;
	//	

	//}


	//



	////dlg.DoModal();

	//CDlgImgShow::GetInstance()->Reset();
	//pManager->EnableRecordFrame(CAMERA_GRAY, FALSE, FALSE);
	//pManager->ResetFrameIndex(CAMERA_GRAY);


	//SetRuntimeChangeLiveView(FALSE);
	//pManager->ConfigGrayCamera();



	CDlgMeasureProgress dlg(AfxGetMainWnd());

	Calculator::GetInstance()->ConfocalMode(TRUE);
	//ready for change to correct value
	BOOL use3dView = m_pMainFrame->GetUse3dView();
	CDlgImgShow::GetInstance()->Set3DViewModel(use3dView);
	if (!use3dView)
	{
		dlg.ScanRange(9);
		dlg.LoopCounter(9);
		CDlgImgShow::GetInstance()->LoopCounter(9);
		Calculator::GetInstance()->SetStepCount(3);
		for (int i = 0; i < 9 ; i++)
		{
			CDlgImgShow::GetInstance()->Trigger();
			pManager->EnableRecordFrame(CAMERA_GRAY, TRUE);
			SleepMs(40);
		}
	}
	else
	{
		SetRuntimeChangeLiveView(TRUE);
		pManager->ConfigGrayCamera();

		int  scanRange = m_pMainFrame->GetScanRange();
		float dfScanStep = m_pMainFrame->GetScanStep();
		dlg.ScanRange(scanRange);
		dlg.LoopCounter(3);
		CDlgImgShow::GetInstance()->LoopCounter(3);
		
		Calculator::GetInstance()->SetStepCount(scanRange);

		pManager->SetFrameIndexDiffBetweenPic(CAMERA_GRAY, MAX_MASK_COUNT);
		ArmDev::GetInstance()->SetTriggerPeriod(20);
		SleepMs(20);

		int posScanRange = scanRange;

		float *pPosArrRev = new float[posScanRange];
		float *pTimArr = new float[posScanRange];

		if(!pTimArr || !pPosArrRev)
			return;

		for (int i = 0; i < posScanRange ; ++i)
		{
			pPosArrRev[i] = (posScanRange -1 - i)*dfScanStep;
			pTimArr[i] = 2.0;//ms 为单位
		}

		for (int MaskIndex = 1; MaskIndex <= MAX_MASK_COUNT ; MaskIndex++)
		{
			ArmDev::GetInstance()->SetTriggerNum(scanRange);
			SleepMs(20);
			ArmDev::GetInstance()->SetNewTriggerEnter();
			SleepMs(20);
			ArmDev::GetInstance()->StartNewTrigger();
			pManager->EnableRecordFrame(CAMERA_GRAY, TRUE, FALSE);
			pManager->ResetFrameIndexForEveryMaskPic(CAMERA_GRAY, MaskIndex);

			float fengfengzhi = m_pMainFrame->GetFengFengZhi();
			float pianzhi	  = m_pMainFrame->GetPianZhi();
			float pinlv		  = m_pMainFrame->GetPinLv();
			float waittime    = m_pMainFrame->GetWaitTime();
			pManager->XMTInstance()->SendMultiPosCmd(fengfengzhi, pinlv, pianzhi);
			SleepMs(waittime);
			CDlgImgShow::GetInstance()->Trigger();
			ArmDev::GetInstance()->SetNewTriggerLeave();
			pManager->XMTInstance()->Stop();
			SleepMs(500);
		}

		delete[] pPosArrRev;
		delete[] pTimArr;


	}






	//dlg.DoModal();

	CDlgImgShow::GetInstance()->Reset();
	pManager->EnableRecordFrame(CAMERA_GRAY, FALSE, FALSE);
	pManager->ResetFrameIndex(CAMERA_GRAY);


	SetRuntimeChangeLiveView(FALSE);
	pManager->ConfigGrayCamera();
}
