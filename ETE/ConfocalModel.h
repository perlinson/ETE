// ConfocalState.h: interface for the CConfocalState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ConfocalState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_ConfocalState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MeasureModel.h"
class CDlgImgShow;
class ConfocalModel : public CModelBase
{
public:
	virtual void Init(CMeasureManager * pManager);
	virtual void Resize();
	virtual void Calculate(CMeasureManager* pManager);

	static ConfocalModel* Instance();

	virtual ~ConfocalModel();

protected:
	ConfocalModel();

	virtual void ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera );

	virtual void ConfigARMParameter( ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar );

	virtual void ConfigXMTParameter( CMeasureManager* pManager );

	virtual void StartMeasure(CMeasureManager* pManager);
private:
	static ConfocalModel m_ConfocalState; // singleton...
};

#endif // !defined(AFX_ConfocalState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
