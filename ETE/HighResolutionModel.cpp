// HighResolutionState.cpp: implementation of the CHighResolutionState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HighResolutionModel.h"
#include "ConfocalModel.h"
#include "VSIModel.h"
#include "MeasureManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HighResolutionModel HighResolutionModel::m_HighResolutionState;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HighResolutionModel::HighResolutionModel()
{

}

HighResolutionModel::~HighResolutionModel()
{
}

void HighResolutionModel::Calculate(CMeasureManager* pManager)
{
	/*switch ( nKey )
	{
	case VK_F1:
		TRACE("* Introduction State changed the current state to Play State\n");
		ChangeState(pManager, CConfocalState::Instance());
		break;
	case VK_F2:
		TRACE("* Introduction State changed the current state to HighScore State\n");
		ChangeState(pManager, CVsiState::Instance());
		break;
	}*/
}

HighResolutionModel* HighResolutionModel::Instance()
{
	return ( &m_HighResolutionState );
}

void HighResolutionModel::Resize()
{
}

void HighResolutionModel::Init(CMeasureManager * pManager)
{
	
}

void HighResolutionModel::ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera ) const
{
	//how to Modify Parameter
}

void HighResolutionModel::ConfigCameraParameter( ST_CAMERA_INPUT_PAR& grayCamera, ST_CAMERA_INPUT_PAR& colorCamera )
{
	
}

void HighResolutionModel::ConfigARMParameter( ST_ARM_CONSOLE_DEV_INF_PARA& stArmConsoleInfPar )
{
	//modify the ARM
}

void HighResolutionModel::ConfigXMTParameter( CMeasureManager* pManager )
{
	
}

void HighResolutionModel::StartMeasure( CMeasureManager* pManager )
{
	
}