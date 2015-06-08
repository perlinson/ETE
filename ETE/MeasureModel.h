// MeasureModel.h: interface for the CMeasureModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MeasureModel_H__AB04DC06_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_MeasureModel_H__AB04DC06_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ETESystemPublicDefine.h"


class CMeasureManager;
class CMainFrame;
class CDlgMeasureProgress;
class CDlgImgShow;
class CModelBase  
{
public:
	virtual void Init(CMeasureManager * pManager);
	virtual void Resize() = 0;
	virtual void Calculate(CMeasureManager* pManager) = 0;
	virtual void ConfigCameraParameter(ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera) = 0;
	virtual void ConfigARMParameter(ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar) = 0;
	virtual void ConfigXMTParameter(CMeasureManager* pManager) = 0;
	virtual void StartMeasure(CMeasureManager* pManager) = 0;
	virtual void SetRuntimeChangeLiveView(BOOL bChange) { m_changeLiveViewAtRuntime = bChange; }
	virtual ~CModelBase();

public:
	static CRect m_Rect;
protected:
	int m_calcPictureNum;
	double m_currentStep;
	double m_changeLiveViewAtRuntime;
	CMainFrame* m_pMainFrame;
protected:
	CModelBase();

	//std::shared_ptr<CDlgMeasureProgress> m_measureDlgPtr;


};

#endif // !defined(AFX_MeasureModel_H__AB04DC06_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
