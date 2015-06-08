// ObserverState.h: interface for the CObserverState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ObserverState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_ObserverState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MeasureModel.h"

class ObserverModel : public CModelBase
{
public:
	virtual void Init(CMeasureManager * pManager);
	virtual void Resize() {}
	virtual void Calculate(CMeasureManager* pManager) {}

	static ObserverModel* Instance();
	
	virtual ~ObserverModel();

protected:
	ObserverModel();

	virtual void ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera );

	virtual void ConfigARMParameter( ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar );

	virtual void ConfigXMTParameter( CMeasureManager* pManager ){}

	virtual void StartMeasure(CMeasureManager* pManager) {}

private:
	static ObserverModel m_ObserverState; // singleton...
};

#endif // !defined(AFX_ObserverState_H__AB04DC09_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
