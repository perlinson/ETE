// HighResolutionState.h: interface for the CHighResolutionState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HighResolutionState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_HighResolutionState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MeasureModel.h"

class HighResolutionModel : public CModelBase
{
public:
	virtual void Init(CMeasureManager * pManager);
	virtual void Resize();
	virtual void Calculate(CMeasureManager* pManager);

	static HighResolutionModel* Instance();

	virtual ~HighResolutionModel();

protected:
	CFont m_TextFont;
	HighResolutionModel();

	virtual void ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera ) const;

	virtual void ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera );

	virtual void ConfigARMParameter( ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar );

	virtual void ConfigXMTParameter( CMeasureManager* pManager );

	virtual void StartMeasure(CMeasureManager* pManager);

private:
	static HighResolutionModel m_HighResolutionState; // singleton...
};

#endif // !defined(AFX_HighResolutionState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
