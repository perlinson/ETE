// GameManager.cpp: implementation of the CGameManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MeasureModel.h"
#include "MeasureManager.h"
#include "ObserverModel.h"
#include "ArmDev.h"
#include "XMTDev.h"
#include "StereoView.h"
#include "util.h"
#include "CameraView.h"
#include "WorkThread.h"
#include <ppl.h>
#include "PosController.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HANDLE g_hCameraAssignEvent;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMeasureManager* pMeasureManager = nullptr;

CMeasureManager::CMeasureManager(CStereoView* pView)
	:m_pView(pView),
	xmtDevPtr_(nullptr),
	m_cameraThreadPtr(nullptr)
{
	m_pMeasureModel = NULL;
	Init();
	pMeasureManager = this;
	// by default, we start with the introduction mode...
	//ChangeState( ObserverMeasureModel::Instance() );
}

CMeasureManager::~CMeasureManager()
{
	//DeInitilize();
}

void CMeasureManager::ChangeState(CModelBase * pState)
{
	ASSERT(pState);
	if ( pState != m_pMeasureModel )
	{
		m_pMeasureModel = pState;
		m_pMeasureModel->Init(this);
	}
}

void CMeasureManager::ProcessKeyboardEvent(char nKey)
{
	ASSERT(m_pMeasureModel);
	m_pMeasureModel->Calculate(this);
}

void CMeasureManager::SetRect(CRect & rect)
{
	ASSERT(m_pMeasureModel);
	CModelBase::m_Rect = rect;
	m_pMeasureModel->Resize();
}

BOOL CMeasureManager::ConfigCamera()
{
	ASSERT(m_pMeasureModel);

	TRACE("waiting newCamera all oking ...\n");
	DWORD dwTimeout = 5000;
	DWORD dwRet = WaitForSingleObject(g_hCameraAssignEvent,5000);

	if (dwRet == WAIT_TIMEOUT)
	{
		TRACE("waiting newCamera time out\n");
		return FALSE;
	}
	else if(dwRet == WAIT_FAILED)
	{
		TRACE("waiting newCamera failed \n");
		return FALSE;
	}
	

	if (!ConfigGrayCamera())
	{
		TRACE(TEXT("ConfigGrayCamera failed! \r\n"));
		return FALSE;
	}

	TRACE(TEXT("ConfigCamera OK! \r\n"));
	return TRUE;
}

BOOL CMeasureManager::ConfigARM()
{
	ASSERT(m_pMeasureModel);
	m_pMeasureModel->ConfigARMParameter(stArmConsoleInfPar_);

	if (!ArmDev::GetInstance()->ConfigParameters(stArmConsoleInfPar_))
	{
		TRACE(TEXT("ArmController::GetInstance()->ConfigParameters failed \r\n"));
		return FALSE;
	}
	return	TRUE;
}

void CMeasureManager::Init()
{
	
	stCameraPar_[CAMERA_GRAY].VidSetNumber = 12;		//No.1 Mode select
	stCameraPar_[CAMERA_GRAY].IsoSpeed = 3;       //No.2 ISO speed
	stCameraPar_[CAMERA_GRAY].TriggerMode = 0;      //No.3 Triggermode
	stCameraPar_[CAMERA_GRAY].BufferCnt = 16;        //No.4 Number of frames to allocate
	stCameraPar_[CAMERA_GRAY].BayerPattern = 0;     //No.5 Used bayer pattern
	stCameraPar_[CAMERA_GRAY].Y16InversOrder = 0;   //No.6 Inversmode for Y16
	stCameraPar_[CAMERA_GRAY].YIsRawMode = 0;       //No.7 Raw mode flag
	stCameraPar_[CAMERA_GRAY].XPos = 0;             //No.8 X-Offset
	stCameraPar_[CAMERA_GRAY].YPos = 0;             //No.9 Y-Offset
	stCameraPar_[CAMERA_GRAY].XSize = 780;            //No.10 X-Dimension
	stCameraPar_[CAMERA_GRAY].YSize = 580;            //No.11 Y-Dimension
	stCameraPar_[CAMERA_GRAY].ColorFormat = 0;      //No.12 Actual ColorFormat
	stCameraPar_[CAMERA_GRAY].PktSize = 3508;          //No.13 Payload per packet
	stCameraPar_[CAMERA_GRAY].IsSrollMode = 0;
	stCameraPar_[CAMERA_GRAY].IsoReported= FALSE;
	stCameraPar_[CAMERA_GRAY].IsNotified = FALSE;
	stCameraPar_[CAMERA_GRAY].RecordFrameCount= 0;
	stCameraPar_[CAMERA_GRAY].StartTime       = 0;        
	memset(&stCameraPar_[CAMERA_GRAY].RecordInfo, 0, sizeof(stCameraPar_[CAMERA_GRAY].RecordInfo));
	//ARM
	stArmConsoleInfPar_.iBraudRate = 19200;
	stArmConsoleInfPar_.iDelayTime = 50;
	stArmConsoleInfPar_.iPriodTime = 40;
	stArmConsoleInfPar_.iPulseNum  = 1;
	
	m_bRecordModel[CAMERA_GRAY] = FALSE;

	stCamera_[CAMERA_GRAY] = nullptr;

	m_pCameraView[CAMERA_GRAY] = nullptr;

	m_bCameraConfigFirst[CAMERA_GRAY] = TRUE;

	m_IsNotified[CAMERA_GRAY] = FALSE;
	m_isCameraConfigured = false;

	g_hCameraAssignEvent = CreateEvent(NULL, TRUE, FALSE, _T("GrayCamerAssign"));

}

void CMeasureManager::Initilize()
{
 	CreateCameraView(CAMERA_GRAY);
}

// 设置相机工作参数
bool CMeasureManager::ConfigCameraSysPara(int iCameraIdx)
{
	VIDSET     VidSet;
	UINT8      Result;
	UINT32     Error;

	ST_CAMERA_INPUT_PAR stCameraPar = stCameraPar_[iCameraIdx];
	//待设定参数集
	UINT32    VidSetNumber = stCameraPar.VidSetNumber;		//No.1
	UINT8     IsoSpeed = stCameraPar.IsoSpeed;		         //No.2 ISO speed
	UINT8     TriggerMode = stCameraPar.TriggerMode;		      //No.3 Triggermode
	UINT32    BufferCnt = stCameraPar.BufferCnt;			      //No.4 Number of frames to allocate
	UINT8     BayerPattern = stCameraPar.BayerPattern;	     //No.5 Used bayer pattern
	UINT8     Y16InversOrder = stCameraPar.Y16InversOrder;	   //No.6 Inversmode for Y16
	UINT8     YIsRawMode = stCameraPar.YIsRawMode;	       //No.7 Raw mode flag
	UINT16    XPos = stCameraPar.XPos;		             //No.8 X-Offset
	UINT16    YPos = stCameraPar.YPos;             //No.9 Y-Offset
	UINT16    XSize = stCameraPar.XSize;            //No.10 X-Dimension
	UINT16    YSize = stCameraPar.YSize;            //No.11 Y-Dimension
	UINT8     ColorFormat = stCameraPar.ColorFormat;      //No.12 Actual ColorFormat
	UINT16    PktSize = stCameraPar.PktSize;          //No.13 Payload per packet

	CCamera * pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];

	if (!pCamera)
	{
		return false;
	}

	// Get pointer to box and get current VidSet
	pCamera->GetVidSetValues(VidSetNumber,&VidSet);

	// Get speed
	pCamera->pParms()->IsoSpeed= IsoSpeed;

	// Get triggermode
	pCamera->pParms()->TriggerMode= TriggerMode;

	// Get buffer cnt
	pCamera->pParms()->BufferCnt= BufferCnt;

	// Get bayer pattern
	pCamera->pParms()->BayerPattern= BayerPattern;

	// Get Y-16 invers mode
	pCamera->pParms()->Y16InversOrder= Y16InversOrder;

	// Get Y-16 raw mode
	pCamera->pParms()->YIsRawMode= YIsRawMode;

	// Check scalable stuff
	if(VidSet.VideoFormat==PSFORMAT)
	{
		VidSet.XPos= XPos;

		VidSet.YPos= YPos;

		VidSet.XSize= XSize;

		VidSet.YSize= YSize;

		VidSet.ColorFormat= ColorFormat;

		VidSet.PktSize= PktSize;
	}

	// Always select this VidSet and if partial scan overwrite values
	Result=pCamera->SelectVidSetByNr(VidSetNumber);
	if(Result && VidSet.VideoFormat==PSFORMAT)
		Result=pCamera->SelectVidSetByValue(&VidSet);

	if(!Result)
	{
		Error=pCamera->LastError();
		MessageBeep(0);
		return false;
	}  

	return true;
}


// 设置相机feature参数
bool CMeasureManager::ConfigCameraFeaturePara(int iCameraIdx)
{
	ST_CAMERA_INPUT_PAR stCameraPar = stCameraPar_[iCameraIdx];

	FEATPROP	FeatPropGain = stCameraPar.FeatPropGain;
	FeatPropGain.Type = CFT_GAIN;

	FEATPROP	FeatPropBrightness = stCameraPar.FeatPropBrightness;
	FeatPropBrightness.Type = CFT_BRIGHTNESS;

	FEATPROP	FeatShutter = stCameraPar.FeatShutter;
	FeatShutter.Type = CFT_SHUTTER;

	FEATPROP	FeatGamma = stCameraPar.FeatGamma;
	FeatGamma.Type = CFT_GAMMA;

	FEATPROP	FeatAutoExposure = stCameraPar.FeatAutoExposure;
	FeatAutoExposure.Type = CFT_AUTOEXPOSURE;

	FEATPROP	FeatWhiteBal= stCameraPar.FeatWhiteBal;
	FeatWhiteBal.Type = CFT_WHITEBAL;


	if (!m_cameraThreadPtr)
	{
		return false;
	}

	/****************************************************************************/

	CCamera *pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];

	if (!pCamera)
	{
		return false;
	}

	pCamera->Lock();

	pCamera->GetFeature(FeatPropGain.Type, &stCameraPar_[iCameraIdx].FeatPropGain);
	pCamera->GetFeature(FeatPropBrightness.Type, &stCameraPar_[iCameraIdx].FeatPropBrightness);
	pCamera->GetFeature(FeatShutter.Type, &stCameraPar_[iCameraIdx].FeatShutter);
	pCamera->GetFeature(FeatGamma.Type, &stCameraPar_[iCameraIdx].FeatGamma);
	pCamera->GetFeature(FeatAutoExposure.Type, &stCameraPar_[iCameraIdx].FeatAutoExposure);
	

	stCameraPar_[iCameraIdx].FeatPropGain.Value = 0;
	stCameraPar_[iCameraIdx].FeatShutter.Value = 830;

	if (m_pMeasureModel)
	{
		m_pMeasureModel->ConfigCameraParameter(stCameraPar_[0],stCameraPar_[1]);
	}
	
	

	pCamera->SetFeature(FeatPropGain.Type, &stCameraPar_[iCameraIdx].FeatPropGain);
	pCamera->SetFeature(FeatPropBrightness.Type, &stCameraPar_[iCameraIdx].FeatPropBrightness);
	pCamera->SetFeature(FeatShutter.Type, &stCameraPar_[iCameraIdx].FeatShutter);
	pCamera->SetFeature(FeatGamma.Type, &stCameraPar_[iCameraIdx].FeatGamma);
	pCamera->SetFeature(FeatAutoExposure.Type, &stCameraPar_[iCameraIdx].FeatAutoExposure);
	

	pCamera->Unlock();
	return true;
}


// 获取相机DirectAccess
bool CMeasureManager::GetCameraDirectAccess(int iCameraIdx)
{
	CCamera *pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];

	UINT8  Buf[8];
	UINT32 Address,Quadlet;

	char cbuf[32] = {'F','0', 'F', '0', '0', '6', '1', '4'};

	if(ScanHex(cbuf,Buf,sizeof(Buf))!=4)
		return false;

	Address=MAKEULONG(Buf[0],Buf[1],Buf[2],Buf[3]); 
	if(!pCamera->ReadQuad(0xFFFF,Address,&Quadlet))
	{
		return false;
	}

	return true;
}

void CMeasureManager::SetCamera( CCamera* pGrayCamera, CCamera* pColorCamera )
{
	stCamera_[CAMERA_GRAY] = pGrayCamera;
}


// 设置相机DirectAccess
int CMeasureManager::SetCameraDirectAccess(int iCameraIdx, UINT32 data)
{

	CCamera *pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];

	if(!pCamera)
		return CAMERA_UNAVIALABLE;

	char cbufaddr[32] = {'F','0', 'F', '0', '0', '6', '1', '4'};
	char cbufdata[32] = {0};

	for(int i = 0; i < 8; ++i)
	{
		cbufdata[i] = data & (0xf << (7 - i)) + '0';
	}

	UINT8  BufAddr[8];
	UINT8  BufData[8];
	UINT32 Address,Data;
	UINT8 Broadcast;

	if(ScanHex(cbufaddr,BufAddr,sizeof(BufAddr))!=4 || 
		ScanHex(cbufdata,BufData,sizeof(BufData))!=4
		)
		return CAMERA_INPUT_CAL_PRAR_ERR;

	Address = MAKEULONG(BufAddr[0], BufAddr[1], BufAddr[2], BufAddr[3]); 
	Data = MAKEULONG(BufData[0], BufData[1], BufData[2], BufData[3]); 

	// Check keybord state for broadcast
	if((0x8000&GetAsyncKeyState(VK_SHIFT))!=0)
		Broadcast=TRUE;
	else
		Broadcast=FALSE; 

	if(!pCamera->WriteQuad(0xFFFF,Address,Data,Broadcast))
	{
		return CAMERA_CONFIG_PRAR_FAILED;
	}
	return 0;
}


// 配置相机Scroll模式
int CMeasureManager::ConfigCameraScrollMode(int iCameraIdx, BOOL bScrollMode)
{
	CCamera *pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];

	if(!pCamera)
		return CAMERA_UNAVIALABLE;

	/*m_bScrollMode[iCameraIdx] = bScrollMode;
	m_pCameraView[iCameraIdx]->SetScrollmode(m_bScrollMode[iCameraIdx]);*/

	return NO_ERR;
}

BOOL CMeasureManager::ConfigGrayCamera()
{
	if (!ConfigCameraFeaturePara(CAMERA_GRAY))
	{
		TRACE(TEXT("ConfigGrayCameraFeaturePara failed! \r\n"));
		return FALSE;
	}
	
// 	if (!m_bCameraConfigFirst[CAMERA_GRAY])
// 	{
// 		StartCameraAcquisition(CAMERA_GRAY);
// 	}
// 
// 	m_bCameraConfigFirst[CAMERA_GRAY] = FALSE;

	CreateCameraView(CAMERA_GRAY, FALSE);

	if (!ConfigCameraSysPara(CAMERA_GRAY))
	{
		TRACE(TEXT("ConfigGrayCameraSysPara failed! \r\n"));
		return FALSE;
	}

	if (!ConfigCameraRecordFrameInfo(CAMERA_GRAY))
	{
		TRACE(TEXT("ConfigCameraRecordFrameInfo failed! \r\n"));
		return FALSE;
	}


	StartCameraAcquisition(CAMERA_GRAY);
	return TRUE;
}


void CMeasureManager::StartMeasure()
{
	ASSERT(m_pMeasureModel);
	m_pMeasureModel->StartMeasure(this);
}

void CMeasureManager::Calculate()
{
	ASSERT(m_pMeasureModel);
	m_pMeasureModel->Calculate(this);
}

CXMTDev* CMeasureManager::XMTInstance()
{
	return xmtDevPtr_;
}

BOOL CMeasureManager::StartCameraAcquisition(int iCameraIdx)
{
	CCamera *pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];;
	ST_CAMERA_INPUT_PAR stCameraPar = stCameraPar_[iCameraIdx];
	if(!pCamera)
	{
		return FALSE;
	}

	CAMPARMS *pParms;
	TCHAR      Msg[64];
	UINT32    Result;

	if(!pCamera->IsoActive())
	{
		// Prepare record stuff
		stCameraPar.RecordFrameCount = 0;
		stCameraPar.RecordDivider = 1;

		// Reset frame counter
		pCamera->ClearFrameCounter();

		// Store start time to calculate framerate
		stCameraPar.StartTime = GetTickCount();

		// Check control key to switch between user/system frames
		pCamera->UseUserFrames((0x8000&GetAsyncKeyState(VK_CONTROL))?TRUE:FALSE);

		// Start camera
		if(!pCamera->StartIso())
		{
			Result = pCamera->LastError();
			TRACE(_T("Error <CCamera::StartIso>"));
			return FALSE;
		}  

		// Check real allocated and intended buffer allocation
		pParms = pCamera->pParms();
		if(pParms->BufferCnt!=pParms->RealBufferCnt)
		{
			wsprintf(Msg, _T("W: Partial buffer allocation (%d from %d)"),
				pParms->RealBufferCnt,pParms->BufferCnt);
			TRACE(Msg);
		}

		if(!m_IsoReported[iCameraIdx])
		{
			m_IsoReported[iCameraIdx] = TRUE;
			pCamera->GetIsoPayload();
			pCamera->GetFrameMemorySize();
		}
	}
	else
	{
		pCamera->StopIso();

		if(m_IsoReported[iCameraIdx])
		{
			m_IsoReported[iCameraIdx] = FALSE;
		}
	}

	return TRUE;
}

BOOL CMeasureManager::EnableRecordFrame(int icameraidx) const
{
	return m_bRecordModel[icameraidx];
}

void CMeasureManager::ResetFrameIndexForEveryMaskPic(int iCameraIdx, int mask_index)
{
	CCameraView* pView = m_cameraThreadPtr->m_pCameraView[iCameraIdx];

	if (pView)
	{
		pView->ResetToIndexForEveryMask(mask_index);
	}
}

void CMeasureManager::SetFrameIndexDiffBetweenPic(int iCameraIdx, int differ)
{
	CCameraView* pView = m_cameraThreadPtr->m_pCameraView[iCameraIdx];

	if (pView)
	{
		pView->Differ(differ);
	}
}

void CMeasureManager::EnableRecordFrame(CAMERA_IDX icameraidx, BOOL bEnable, BOOL bInternalTriggerMode)
{
	CCameraView* pView = m_cameraThreadPtr->m_pCameraView[icameraidx];

	if (pView)
	{
		pView->RecordFrame(bEnable, bInternalTriggerMode);
	}
}

void CMeasureManager::ResetFrameIndex(CAMERA_IDX icameraidx)
{
	CCameraView* pView = m_cameraThreadPtr->m_pCameraView[icameraidx];

	if (pView)
	{
		pView->ResetFrameIndex();
	}
}

bool CMeasureManager::GetAllDevsReadyState()
{
	if (!GetCameraReadyState())
	{
		return false;
	}
// 
	if(!GetArmReadyState())
	{
		return false;
	}

// 	if (!GetLEDReadyState())
// 	{
// 		return false;
// 	}

  	if (!GetXMTReadyState())
  	{
  		return false;
  	}

	

	

	/*if (!GetLEDReadyState() ||
		!GetArmReadyState() || 
		!GetPosReadyState() ||
		!GetXMTReadyState() ||
		!GetCameraReadyState())
	{
		return false;
	}*/
	return true;
}

bool CMeasureManager::GetArmReadyState()
{
	if(!ArmDev::GetInstance()->InitDevInterface())
	{
		TRACE(TEXT("armConsoleDevPtr_->InitDevInterface() failed!"));
		return false;
	}


	if(!ArmDev::GetInstance()->ShakeHandWithDevInterface())
	{
		TRACE(TEXT("armConsoleDevPtr_->ShakeHandWithDevInterface() failed!"));
		return false;
	}


	if(!ArmDev::GetInstance()->ConfigParameters(stArmConsoleInfPar_))
	{
		TRACE(TEXT("ArmController::GetInstance()->ConfigParameters(stArmConsoleInfPar_)"));
		return false;
	}
	return true;
}


bool CMeasureManager::GetXMTReadyState()
{
	xmtDevPtr_ = new CXMTDev;

	if (!xmtDevPtr_)
	{
		return false;
	}

	if(!xmtDevPtr_->InitDevInterface(stXmtIntPar_))
	{
		TRACE(TEXT("m_pXmtDev->InitDevInterface failed!"));
		return false;
	}

	if(!xmtDevPtr_->ShakeHandWithDevInterface())
	{
		TRACE(TEXT("m_pXmtDev->ShakeHandWithDevInterface failed!"));
		return false;
	}

// 	if (!xmtDevPtr_->CheckLoopState())
// 	{
// 		return false;
// 	}

// 	ST_XMT_SYS_INFO stXmtSysInfo;
// 	if(NO_ERR != xmtDevPtr_->ReadSysInfo(&stXmtSysInfo))
// 	{
// 		TRACE(_T("ERR: Read XMT System Par Failed!\n"));
// 		return false;
// 	}

	//m_CameraMesScope.start = stXmtSysInfo.systemMinStep;
	//m_CameraMesScope.end = stXmtSysInfo.systemMaxStep_ch1;


	return true;
}

bool CMeasureManager::GetCameraReadyState()
{
	bool cameraReady = false;
	//启动相机

	if (!GetCameraDevStatus())
	{
		return false;
	}

//  	if (!ConfigCamera())
//  	{
//  		return false;
//  	}

	return true;
}



bool CMeasureManager::GetCameraDevStatus()
{
	FBSYSINFO stCameraInfo;
	GetSysInfo(&stCameraInfo);

	if(stCameraInfo.CardCount < 1)
	{
		TRACE(_T("The count the Camera Card is Zero"));
		return false;
	}

	ResetEvent(g_hCameraAssignEvent);

	if(!StartFireCameraWorkThread())
	{
		TRACE(_T("StartFireCameraWorkThread Failed"));
		return false;
	}
	return true;
}

bool CMeasureManager::StartFireCameraWorkThread()
{
	if(m_cameraThreadPtr)
	{
		return false;
	}

	HWND hWnd = m_pView->GetSafeHwnd();

	m_cameraThreadPtr = new CWorkThread(&hWnd, m_pCameraView);
	//m_cameraThreadPtr = std::make_shared<CWorkThread>(&hWnd, m_pCameraView);

	
	if(m_cameraThreadPtr)
		m_cameraThreadPtr->Run((UINT8)0);

	return TRUE;
}


bool CMeasureManager::CreateCameraView(int index, BOOL bChanged/*= TRUE*/)
{
	if (!bChanged)
	{
		return TRUE;
	}

	CRect rcCamera;
	CWnd *pWndCamera = m_pView->GetDlgItem(IDC_LIVEVIEW_GRAY + index);
	pWndCamera->GetWindowRect(&rcCamera);
	m_pView->ScreenToClient(&rcCamera);

	if (m_pCameraView[index] == NULL)
	{
		m_pCameraView[index] = (CCameraView *)RUNTIME_CLASS(CCameraView)->CreateObject();
		if (m_pCameraView[index] == NULL)
		{
			return false;
		}
		m_pCameraView[index]->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rcCamera, m_pView, ID_LIVEVIEW_GRAY + index);
	}

	ST_CAMERA_INPUT_PAR stCameraPar = stCameraPar_[index];

	m_pCameraView[index]->CreateBitmap(stCameraPar.XSize,
		stCameraPar.YSize,
		stCameraPar.ColorFormat,
		stCameraPar.BayerPattern,
		stCameraPar.YIsRawMode,
		stCameraPar.Y16InversOrder);
	return true;

}

void CMeasureManager::ShowGrayCameraOnly() const
{
	m_pCameraView[CAMERA_GRAY]->ShowWindow(TRUE);
	m_pCameraView[CAMERA_GRAY]->UpdateWindow();
}

void CMeasureManager::PushFrame( FCTLFRMINFO * pFrameInfo, UINT32 FrameNumber, int iCameraIdx )
{
		CCamera *pCamera = NULL;
	
		pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];
	
		if(!pCamera)
			return ;
	
		if(!m_bRecordModel[iCameraIdx])
		{
			m_pCameraView[iCameraIdx]->ProcessFrame(pFrameInfo->pData,pFrameInfo->Length);
		}
		else
		{
			// Call our record Calculator
			RecordFrame(pFrameInfo,FrameNumber, iCameraIdx);
		}
}

void CMeasureManager::RecordFrame( FCTLFRMINFO * pFrameInfo, UINT32 FrameNumber, int iCameraIdx )
{
//	CCamera *pCamera = NULL;
//
//	pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];
//
//	if(!pCamera)
//		return ;
//
//	char FileName[256];
//	char Name[64];
//
//	char buf[256+64] = {0};
//	// Give new frame to our view for conversion and assignment but don't update
//	m_pCameraView[iCameraIdx]->ProcessFrame(pFrameInfo->pData,pFrameInfo->Length,FALSE);
//
//	// Check our divider to figure out whether we have to store
//	if(--m_RecordDivider[iCameraIdx]==0)
//	{
//		m_RecordDivider[iCameraIdx] = m_RecordInfo[iCameraIdx].Divider;
//		m_RecordFrameCount[iCameraIdx]++;
//
//		// Only store until frame limit has been reached
//		if((m_RecordInfo[iCameraIdx].FrameCount==0) ||
//			(m_RecordFrameCount[iCameraIdx] <= m_RecordInfo[iCameraIdx].FrameCount))
//		{
//			//// Create filename
//			//sprintf(Name,("%s%08d_%03d.bmp"),
//			//	m_RecordInfo[iCameraIdx].NamePrefix,
//			//	m_RecordFrameCount[iCameraIdx]%1000,
//			//	FrameNumber);
//
//			// Create filename
//			sprintf(Name,("%s%d.bmp"),
//				m_RecordInfo[iCameraIdx].NamePrefix,
//				m_indexOfPicSaved++);
//
//			// Build actual filename
//			if(m_RecordInfo[iCameraIdx].Directory[strlen(m_RecordInfo[iCameraIdx].Directory)-1] == ('\\'))
//				sprintf(FileName,("%s%s"),m_RecordInfo[iCameraIdx].Directory,Name);
//			else
//				sprintf(FileName,("%s\\%s"),m_RecordInfo[iCameraIdx].Directory,Name);
//
//			// Write bitmap and notify our record dialog
//			if(WriteBitmap(FileName,iCameraIdx))
//			{
//				PostMessage(WM_CAMERA_SAVEBMP_COMPLETED);
//				//TRACE(_T("RealSave frame count: %d\n"), m_RecordFrameCount[iCameraIdx]);
//				sprintf(buf, "RealSave %s\n", FileName);
//				TRACE(buf);
//			}
//		}
//
//		//let the camera always execute, needn't stop
//
//#if 1
//		// See if we must stop the record process
//		if(m_RecordFrameCount[iCameraIdx] == m_RecordInfo[iCameraIdx].FrameCount)
//		{
//			//reset the index of saved img
//			m_indexOfPicSaved = 0;
//		}
//#endif
//	}
}

void CMeasureManager::ProcessNewFrame( int iCameraIdx )
{
	CCamera *pCamera = NULL;

	pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];

	// Only react if we have a camera and no processing takes place
	if(!pCamera)
	{
		return;
	}


	UINT32      Total,Discarded;
//	TCHAR       cbuf[256];
	FCTLFRMINFO FrameInfo;
	UINT32      MaxFrames;
	UINT32      FrameNumber;
	UINT8       Result;
	double      RunTime;


	



	MaxFrames = pCamera->FrameQueueSize();
	do
	{
		pCamera->Lock();
		Result = pCamera->GetFrame(&FrameInfo,0,&FrameNumber);
		if(Result)
		{
			// Push frame
			PushFrame(&FrameInfo,FrameNumber,iCameraIdx);

			// Return frame to camera
			pCamera->ReturnFrame(&FrameInfo);
		}
		pCamera->Unlock();
	}while(Result && --MaxFrames);

	// From now on we can be notified again
	m_IsNotified[iCameraIdx] = FALSE;

	// Discard all frames here. If we are too slow we have time for GUI until
	// the next frame arrives
	pCamera->Lock();
	pCamera->DiscardFrames();
	pCamera->Unlock();

	// Show statistic
	pCamera->GetFrameCounter(&Total,&Discarded);

	//wsprintf(cbuf,"%d",Total);
	//m_pStatusBar->SetPaneText(1,cbuf);

	//wsprintf(cbuf,"%d",Discarded);
	//m_pStatusBar->SetPaneText(3,cbuf);

	// Calculate framerate every 10th frame
	if(Total%16==0)
	{
		// We accept overrun after 50 days
		RunTime=(double)(GetTickCount() - m_StartTime[iCameraIdx])/1000.0;
		if(RunTime>1.0)
		{
			CString strFrameRate;
			strFrameRate.Format(TEXT("FrameRate : %.1f frame per second \r\n"), ((double)Total)/RunTime);
			//TRACE(strFrameRate.GetBuffer(0));
			//strFrameRate.ReleaseBuffer();
			/*wsprintf(cbuf,_T("%.1f"),(double)(Total)/RunTime);
			TRACE("FrameRate:%s\n", cbuf);*/
		}
	}
}

void CMeasureManager::GetNewFrameNotify( int iCameraIdx )
{
	CCamera *pCamera = NULL;

	pCamera = m_cameraThreadPtr->m_pCamera[iCameraIdx];

	if(!pCamera)
		return;

	// Only react if we have a camera and no processing takes place
	if(pCamera && !m_IsNotified[iCameraIdx])
	{
		m_IsNotified[iCameraIdx] =TRUE;

		// Post a message to our own window to start out thread (change in context)
		m_pView->PostMessage(WM_CAMERA_NEWFRAME, 0, iCameraIdx);
	}
}

void CMeasureManager::StopFireCameraWorkThread()
{
	if (m_cameraThreadPtr)
	{
		m_cameraThreadPtr->Stop();
		// Wait until our afx thread really has stopped
		WaitForSingleObject(m_cameraThreadPtr->m_pThread->m_hThread,INFINITE);

		delete m_cameraThreadPtr;
		m_cameraThreadPtr = nullptr;
	}
}

void CMeasureManager::OnWorkThreadStop()
{
	//bool m_MustQuit;
	//// Delete old thread
	if(m_cameraThreadPtr)
	{
		// Wait until our afx thread really has stopped
		WaitForSingleObject(m_cameraThreadPtr->m_pThread->m_hThread,INFINITE);
		
		delete m_cameraThreadPtr;
		m_cameraThreadPtr = nullptr;
	}
}

void CMeasureManager::WorkThreadStopNotify(UINT32 Result)
{
 	if (m_pView)
 	{
 		m_pView->PostMessage(WM_WTSTOP, 0,Result);
 	}
}

BOOL CMeasureManager::SetWhiteStrength(int strength)
{
	if (!ArmDev::GetInstance()->SetWhiteStrength(strength))
	{
		TRACE("ArmController::GetInstance()->SetWhiteStrength Failed");
		return FALSE;
	}
	return TRUE;
}

BOOL CMeasureManager::SetBlueStrength(int strength)
{
	if (!ArmDev::GetInstance()->SetBlueStrength(strength))
	{
		TRACE("ArmController::GetInstance()->SetBlueStrength Failed");
		return FALSE;
	}
	return TRUE;
}

void CMeasureManager::DeInitilize()
{
	StopFireCameraWorkThread();
	
	if (xmtDevPtr_)
	{
		xmtDevPtr_->DeInitDevInterface();
		delete xmtDevPtr_;
		xmtDevPtr_ = nullptr;
	}

	ArmDev::GetInstance()->Destroy();
}

bool CMeasureManager::ConfigCameraRecordFrameInfo(CAMERA_IDX iCameraIdx)
{
	CCameraView* pView = m_cameraThreadPtr->m_pCameraView[iCameraIdx];
	ST_CAMERA_INPUT_PAR cameraPara = stCameraPar_[iCameraIdx];

	if (pView)
	{
		pView->SetImgSavePath(cameraPara.RecordInfo.Directory);
		return true;
	}
	return false;
}

bool CMeasureManager::IsCameraConfigured()
{
	return m_isCameraConfigured;
}

void CMeasureManager::Switch(CString strFilePath)
{
	/*if (pManager->XMTInstance())
	{
		pManager->XMTInstance()->SwtichImgage(strFilePath);
	}*/
}

void CMeasureManager::SaveCurrentBitmap()
{
	CCameraView* pView = m_cameraThreadPtr->m_pCameraView[1];
	

	if (pView)
	{
		pView->SetImgSavePath(_T("C:\\Users\\gys\\Desktop"));
		pView->SaveColorBitmap();
	}
}


CCameraView* CMeasureManager::GetCameraView(CAMERA_IDX camera_idx)
{
	return m_pCameraView[camera_idx];
}

