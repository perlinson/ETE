// GameManager.h: interface for the CGameManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_GAMEMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ETESystemPublicDefine.h"
#include "DlgImageShow.h"
#include "DlgMeasureProgress.h"
class CModelBase;
class CXMTDev;
class CCamera;
class CCameraView;
class CStereoView;
class CWorkThread;
class PosController;

#define  CAMERA_TRIGGLE_MODE_EXTERNAL 1
#define  CAMERA_TRIGGLE_MODE_INTERNAL 0

class CMeasureManager  
{
public:
	void SetRect(CRect& rect);
	void ProcessKeyboardEvent(char nKey);
	void Init();

	void SetCamera(CCamera* pGrayCamera, CCamera* pColorCamera);
	void StartMeasure();
	bool GetAllDevsReadyState();

	CXMTDev* XMTInstance();
	CMeasureManager(CStereoView* pView);
	virtual ~CMeasureManager();


public:
	void ChangeState(CModelBase* pState);
	BOOL EnableRecordFrame(int icameraidx) const;
	void EnableRecordFrame(CAMERA_IDX icameraidx, BOOL bEnable, BOOL bInternalTriggerMode = TRUE);
	int StartCameraAcquisition(int iCameraIdx);
	void PushFrame( FCTLFRMINFO * pFrameInfo, UINT32 FrameNumber, int iCameraIdx );
	void ProcessNewFrame( int iCameraIdx );
	void GetNewFrameNotify( int iCameraIdx );
	void OnWorkThreadStop();
	void Initilize();
	void DeInitilize();
	void StopFireCameraWorkThread();
	void StopCamera();
	void WorkThreadStopNotify(UINT32 Result);
	BOOL ConfigCamera();
	BOOL ConfigXMT();
	BOOL ConfigARM();
	BOOL SetWhiteStrength(int strength);
	BOOL SetBlueStrength(int strength);
	BOOL ConfigGrayCamera();
	BOOL ConfigColorCamera();
	void ResetFrameIndex(CAMERA_IDX icameraidx);
	void Calculate();
	bool IsCameraConfigured();
	void Switch(CString strFilePath);
	void SaveCurrentBitmap();
	void SetCurrentBitmap();
	void MakeConfocalAction();
	void MakePSIAction();
	void MakeVSIAction();
	void DoConfocalAction();
	void DoPSIAction();
	void DoVSIAction();

	void ShowGrayCameraOnly() const;
	bool CameraConfigured() const { return m_isCameraConfigured; }
	void CameraConfigured(bool val) { m_isCameraConfigured = val; }
	CCameraView* GetCameraView(CAMERA_IDX camera_idx);
	void ResetFrameIndexForEveryMaskPic(int iCameraIdx, int mask_index=1);
	void SetFrameIndexDiffBetweenPic(int iCameraIdx, int differ=1);
	CStereoView* GetMainView() const { return m_pView; }
private:
	bool ConfigCameraSysPara(int iCameraIdx);
	bool ConfigCameraFeaturePara(int iCameraIdx);
	bool GetCameraDirectAccess(int iCameraIdx);

	//about Camera
	int SetCameraDirectAccess(int iCameraIdx, UINT32 data);
	int ConfigCameraScrollMode(int iCameraIdx, BOOL bScrollMode);
	
	
	bool GetArmReadyState();
	bool GetXMTReadyState();
	bool GetCameraReadyState();
	bool GetLEDReadyState();
	
	bool GetCameraDevStatus();
	bool StartFireCameraWorkThread();
	
	void RecordFrame( FCTLFRMINFO * pFrameInfo, UINT32 FrameNumber, int iCameraIdx );
	
	bool CreateCameraView(int index, BOOL bChanged= TRUE);
	bool ConfigCameraRecordFrameInfo(CAMERA_IDX iCameraIdx);

	friend class CModelBase;
	CModelBase* m_pMeasureModel;
	CStereoView* m_pView;
private:
	CXMTDev*	xmtDevPtr_;	//位移器对象
	CWorkThread* m_cameraThreadPtr; //相机线程
	CCamera*					stCamera_[MAX_CAMERA_SUPPORTED];
	ST_INTERFACE_PARA			stXmtIntPar_;
	ST_CAMERA_INPUT_PAR			stCameraPar_[MAX_CAMERA_SUPPORTED];
	ST_ARM_CONSOLE_DEV_INF_PARA stArmConsoleInfPar_;

	UINT32          m_RecordFrameCount[MAX_CAMERA_SUPPORTED];
	UINT32          m_RecordDivider[MAX_CAMERA_SUPPORTED];
	BOOL			m_bRecordModel[MAX_CAMERA_SUPPORTED];
	DWORD           m_StartTime[MAX_CAMERA_SUPPORTED];            // Starttime for acquisition
	RECORDINFO      m_RecordInfo[MAX_CAMERA_SUPPORTED];           // Our recording info
	BOOL			m_IsoReported[MAX_CAMERA_SUPPORTED];			// Iso parameters reported
	BOOL            m_IsNotified[MAX_CAMERA_SUPPORTED];           // Flag for already notified
	CCameraView *	m_pCameraView[MAX_CAMERA_SUPPORTED];
	BOOL			m_bCameraConfigFirst[MAX_CAMERA_SUPPORTED];
	bool m_isCameraConfigured;

};

extern CMeasureManager* pMeasureManager;

#endif // !defined(AFX_GAMEMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
