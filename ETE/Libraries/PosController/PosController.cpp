#include "PosController.h"
#include <exception>
#include <xutility>

const float MAX_DISANCE_LIMIT = 60.0f;

PosController* PosController::pInstance = nullptr; 
CCriticalSection PosController::m_sCriticalSection; 
static bool EqualToZero(float f)
{
	const float EPSINON = 0.00001f;
	if (f > -EPSINON && f < EPSINON)
	{
		return true;
	}
	return false;
}


PosController::PosController(void)
{
	Initialize();
}


PosController::~PosController(void)
{
}

void PosController::Initialize()
{
	pciState	= false;
	curAxis		= X_Axis;
	curZStart	= curStart = 0.02;
	curZSpeed	= curSpeed = 0.06;
	curZAcc		= curAcc   = 0.01;

	m_xPercent = 0.0f;
	m_yPercent = 0.0f;
	m_zPercent = 0.0f;

}


bool PosController::ShakeHandWithDevInterface()
{
	//控制卡初始化
	int nCard = 0;
	nCard = d1000_board_init();
	if (nCard <= 0)
	{
		//MessageBox(NULL,TEXT("初始化失败"),TEXT("控制卡初始化失败，请关闭所有程序后再试!"),MB_OK);
		return false;
	}

	try
	{
		d1000_set_pls_outmode(X_Axis, 0);  //pulse/dir 模式，脉冲上升沿有效
		d1000_set_pls_outmode(Y_Axis, 0);  //pulse/dir 模式，脉冲上升沿有效
		d1000_set_pls_outmode(Z_Axis, 0);  //pulse/dir 模式，脉冲上升沿有效
	}
	catch(std::exception & e)
	{
		//MessageBox(NULL,TEXT("初始化失败"),TEXT("控制卡初始化失败，请关闭所有程序后再试!"),MB_OK);
		return false;
	}

	pciState = true;
	return true;

}
/// <summary>
/// 单轴直线运动函数
/// </summary>
/// <param name="UseAxis">轴号：0：X   1：Y   2：Z   3：T</param>
/// <param name="UseStart"></param>
/// <param name="UseAcc"></param>
/// <param name="UseSpeed"></param>
/// <param name="UseMove"></param>运动方向: 
void PosController::axisMove( int UseAxis,  double UseStart, double UseAcc, double UseSpeed, double UseMove )
{
	if (!check_Val(UseStart, MAX_START_SPEED/*,  HT_Public.AxisName[this.UseAxis] + " 轴起始速度必须正数，且只允许[ 0-" + MaxStartSpeed.ToString().Trim() + " ]之间数值！"*/))
	{
		return;
	}
	if (!check_Val(UseAcc, MAX_ACC_SPEED/*, HT_Public.AxisName[this.UseAxis] + " 轴加减速度必须正数，且只允许[ 0-" + MaxAccSpeed.ToString().Trim() + " ]之间数值！"*/))
	{
		return;
	}
	if (!check_Val(UseSpeed, MAX_RUN_SPEED/*, HT_Public.AxisName[this.UseAxis] + " 轴运行速度必须为正数，且只允许[ 0-" + MaxRunSpeed.ToString().Trim() + " ]之间数值！"*/))
	{
		return;
	}

	// 	if (!check_Val(UseMove, 0/*, HT_Public.AxisName[this.UseAxis] + " 轴运行位移必须为正数！"*/))
	// 	{
	// 		return;
	// 	}

	double UseEncConst = PulseUnit[UseAxis];  //获取脉冲当量

	int start   = (int)(UseStart * UseEncConst);		    //启动速度
	double fAcc = (double)(UseAcc * UseEncConst);				//加速度
	int speed   = (int)(UseSpeed * UseEncConst);		    //运行速度
	int move    = (int)(UseMove * UseEncConst);			//运行步长

	if (!isConnected()) 
	{
		return;
	}
	//检测当量轴是否在运动
	if (!checkAxisDown(UseAxis)) 
	{
		return;
	}
	//检测轴是否到极限
	if (!check_Limit(UseAxis, UseMove,true)) 
	{
		//MessageBox(NULL, TEXT("警告!"), TEXT("已运动到极限！请反向运动！"),MB_OK);
		return;
	}
	//停止当前轴
	int Ret = d1000_immediate_stop(UseAxis);
	d1000_start_t_move(UseAxis, move, start, speed, fAcc);
}

/// <summary>
/// 多轴运动函数
/// </summary>
/// <param name="UseAxis">轴号：0：X   1：Y   2：Z   3：T</param>
/// <param name="UseStart"></param>
/// <param name="UseAcc"></param>
/// <param name="UseSpeed"></param>
/// <param name="UseMove"></param>运动方向: 
void PosController::axisXYMove( double UseStart, double UseAcc, double UseSpeed)
{
	if (!check_Val(UseStart, MAX_START_SPEED/*,  HT_Public.AxisName[this.UseAxis] + " 轴起始速度必须正数，且只允许[ 0-" + MaxStartSpeed.ToString().Trim() + " ]之间数值！"*/))
	{
		return;
	}
	if (!check_Val(UseAcc, MAX_ACC_SPEED/*, HT_Public.AxisName[this.UseAxis] + " 轴加减速度必须正数，且只允许[ 0-" + MaxAccSpeed.ToString().Trim() + " ]之间数值！"*/))
	{
		return;
	}
	if (!check_Val(UseSpeed, MAX_RUN_SPEED/*, HT_Public.AxisName[this.UseAxis] + " 轴运行速度必须为正数，且只允许[ 0-" + MaxRunSpeed.ToString().Trim() + " ]之间数值！"*/))
	{
		return;
	}

	// 	if (!check_Val(UseMove, 0/*, HT_Public.AxisName[this.UseAxis] + " 轴运行位移必须为正数！"*/))
	// 	{
	// 		return;
	// 	}

	double UseEncConst = PulseUnit[X_Axis];  //获取脉冲当量


	float tempPercent = min(abs(m_xPercent), abs(m_yPercent));

	int start   = (int)(UseStart * UseEncConst * tempPercent);		    //启动速度
	double fAcc = (double)(UseAcc * UseEncConst * tempPercent);				//加速度
	int speed   = (int)(UseSpeed * UseEncConst * tempPercent);		    //运行速度
	
	if (!isConnected()) 
	{
		return;
	}
	//检测当量轴是否在运动
	if (!checkAxisDown(X_Axis) || !checkAxisDown(Y_Axis)) 
	{
		return;
	}

	long DistArray[2] = {0};
	DistArray[0] = getUserMove(m_xPercent) * UseEncConst; 
	DistArray[1]= getUserMove(m_yPercent) * UseEncConst;

	//检测轴是否到极限
	if (!check_Limit(X_Axis, getUserMove(m_xPercent),true))
	{
		YMove();
		return;
	}
	if (!check_Limit(Y_Axis, getUserMove(m_yPercent), true))
	{
		XMove();
		return;
	}

	//停止当前轴
	d1000_immediate_stop(X_Axis);
	d1000_immediate_stop(Y_Axis);


	short AxisArray[2] = {X_Axis, Y_Axis};
	d1000_start_t_line (2,AxisArray,DistArray, start, speed,fAcc);
}




/// <summary>
/// 检测轴是否到极限位置
/// </summary>
/// <param name="UseAxis">轴号：0：X   1：Y   2：Z   3：T</param>
/// <param name="UseMove">运动行程，有正负之分</param>
/// <param name="ifMsg">是否弹出提示</param>
/// <returns></returns>
bool PosController::check_Limit(int UseAxis, double UseMove,bool ifMsg)
{

	int plState = 0;
	int BitNo=0;
	bool lmtState = true;
	switch (UseAxis)
	{ 
	case 0:
		BitNo = UseMove >= 0 ? 4 : 5;
		break;
	case 1:
		BitNo = UseMove >= 0 ? 6 : 7;
		break;
	case 2:
		BitNo = UseMove >= 0 ? 8 : 9;
		break;
	}

	//if (ifMsg) HT_Public.Msg(BitNo.ToString());

	plState = d1000_in_bit(BitNo); //1表示有效，0表示无效


	if (plState == 0 && UseMove > 0)
	{
		//if (ifMsg) HT_Public.Msg(HT_Public.AxisName[UseAxis] + " 轴已到正向极限位置，请反向运行!");
		lmtState= false;
	}
	if (plState == 0 && UseMove < 0)
	{
		//if (ifMsg) HT_Public.Msg(HT_Public.AxisName[UseAxis] + " 轴已到负向极限位置，请反向运行!");
		lmtState= false;
	}
	return lmtState;
}

bool PosController::isConnected()
{
	return pciState;
}

bool PosController::check_Val(double nowVal, double maxValue)
{
	if ( nowVal <= 0 || nowVal > maxValue )
	{
		return false;
	}
	return true;
}

bool PosController::checkAxisDown( int CurAxis )
{
	int ret;
	ret = d1000_check_done(CurAxis);
	if (ERR_OK == ret)
	{
		//if (ifMsg) HT_Public.Msg("对不起，" + HT_Public.AxisName[CurAxis] + " 轴正在运行中！");
		return false;
	}
	return true;
}
/// <summary>
/// 急停所有轴
/// </summary>
void PosController::stop_All()
{
	if (!isConnected()) 
	{
		return;
	}
	axisStop(X_Axis, false);
	axisStop(Y_Axis, false);
	axisStop(Z_Axis, false);
}

/// <summary>
/// 轴减速、立即停止
/// </summary>
/// <param name="CurAxis">轴号：0：X   1：Y   2：Z   3：T</param>
/// <param name="ifDecStop">true：减速停止  false:立即停止</param>
void PosController::axisStop(int CurAxis, bool ifDecStop)
{
	if (!isConnected()) 
	{
		return;
	}

	int Ret;
	if (ifDecStop)
	{
		Ret = d1000_decel_stop(CurAxis);
	}
	else
	{
		Ret = d1000_immediate_stop(CurAxis);
	}
}

void PosController::XMove()
{
	//TODO:Need to set value
	curAxis  = X_Axis;
	axisMove(curAxis,curStart,curAcc,curSpeed,getUserMove(m_xPercent));
}


void PosController::YMove()
{
	//TODO:Need to set value
	curAxis  = Y_Axis;
	axisMove(curAxis,curStart,curAcc,curSpeed,getUserMove(m_yPercent));
}

void PosController::ZMove()
{
	//TODO:Need to set value
	curAxis  = Z_Axis;
	axisMove(curAxis,curZStart,curZAcc,curZSpeed,getUserMove(m_zPercent));
}


void PosController::Close()
{
	if (pciState)
	{
		stop_All();
		//关闭控制器
		d1000_board_close();
		pciState = false;
	}
}

void PosController::SetSpeed( double speed )
{
	if (curSpeed != speed)
	{
		curSpeed = speed;
	}
}

void PosController::MultiMove(float xPercent, float yPercent, float zPercent)
{
	m_xPercent = xPercent;
	m_yPercent = yPercent;
	m_zPercent = zPercent;
	
	if (!EqualToZero(zPercent))
	{
		ZMove();
		return;
	}

	if (!EqualToZero(xPercent) && !EqualToZero(yPercent))
	{
		XYMove();
		return;
	}
	if(!EqualToZero(xPercent))
	{
		XMove();
		return;
	}
	
	if (!EqualToZero(yPercent))
	{
		YMove();
	}
}

void PosController::XYMove()
{
	axisXYMove(curStart, curAcc, curSpeed);
}


float PosController::getUserMove(float f)
{
	if (f < 0)
	{
		return MAX_DISANCE_LIMIT;
	}
	else
	{
		return -MAX_DISANCE_LIMIT;
	}
}

void PosController::SetZSpeed(float factor)
{
	curZStart	= 0.02 * factor;
	curZSpeed	= 0.06 * factor;
	curZAcc		= 0.01 * factor;
}

void PosController::SetXYSpeed(int factor)
{
	curStart = 0.02 * factor;
	curSpeed = 0.06 * factor;
	curAcc	 = 0.01 * factor;
}

double PosController::GetCurrZPosition()
{
	long pposition = 0;
	pposition = d1000_get_command_pos(Z_Axis);
	return (double)pposition / PulseUnit[Z_Axis];
}

void PosController::SetCurrZPosition(double dfPos)
{
	d1000_set_command_pos(Z_Axis, dfPos);
}



PosController * PosController::GetInstance()
{
	if(pInstance == nullptr)  
	{   //double check  
		m_sCriticalSection.Lock();           //用lock实现线程安全，用资源管理类，实现异常安全  
		//使用资源管理类，在抛出异常的时候，资源管理类对象会被析构，析构总是发生的无论是因为异常抛出还是语句块结束。  
		pInstance = new PosController();
		if (!pInstance)
		{
			return nullptr;
		}
		if(!pInstance->ShakeHandWithDevInterface())
		{
			TRACE(TEXT("m_piezoPtr->ShakeHandWithDevInterface() Failed"));
			return nullptr;
		}
		m_sCriticalSection.Unlock();
	}  
	return pInstance;  
}

void PosController::Destroy()
{
	if (pInstance)
	{
		delete pInstance;
		pInstance = nullptr;
	}
}





