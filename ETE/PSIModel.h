#pragma once
#include "MeasureModel.h"

class PSIModel : public CModelBase
{
public:
	~PSIModel(void);

	virtual void Init(CMeasureManager * pManager);

	virtual void Resize();

	virtual void Calculate(CMeasureManager* pManager);

	virtual void ConfigCameraParameter(ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera);

	virtual void ConfigARMParameter(ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar);

	virtual void ConfigXMTParameter(CMeasureManager* pManager);

	virtual void StartMeasure(CMeasureManager* pManager);
public:
	static PSIModel* Instance();
protected:
	PSIModel(void);

private:
	static PSIModel m_PSIState; // singleton...
};

