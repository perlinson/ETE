#pragma once
#include "Dmc1380.h"
#include "..\..\stdafx.h"
#pragma comment(lib, "Dmc1380.lib")
typedef struct tagMotionPara
{
	int		Axis; //方向
	double	Start;//启动速度
	double	Acc;  //加速度	
	double	Speed;//运行速度
	double	Move; //轴对应行走的位移	
}MotionPara,*LPMotionPara;


const double PulseUnit[4] = { 4000, 4000, 4000, 4000 };

const int MAX_START_SPEED	=3;    //最大起始速度
const int MAX_ACC_SPEED     =10;    //最大加速度
const int MAX_RUN_SPEED     =6;      //最大运行速度 
const int ERR_OK			=0;
class PosController
{
	enum { X_Axis, Y_Axis, Z_Axis };

	//正向 反向 运动
	enum { Forward =1, Backward = -1 }; 
	

public:
	static PosController *GetInstance();  
	static PosController *pInstance;  
	static CCriticalSection m_sCriticalSection;

private:
	PosController(void);
	~PosController(void);

public:
	void Initialize();

public:
	bool ShakeHandWithDevInterface();
	void SetSpeed( double speed );
	void Close();
	void MultiMove(float xMove, float yMove, float zMove);
	void stop_All();
	void SetZSpeed(float factor);
	void SetXYSpeed(int factor);
	double GetCurrZPosition();
	void SetCurrZPosition(double dfPos);
	void Destroy();
private:
	bool isConnected();
	bool checkAxisDown( int CurAxis );
	void axisStop(int CurAxis, bool ifDecStop);
	bool check_Val(double nowVal, double maxValue);
	bool check_Limit(int UseAxis, double UseMove,bool ifMsg);
	void axisMove(int UseAxis,  double UseStart, double UseAcc, double UseSpeed, double UseMove);
	void XMove();
	void YMove();
	void ZMove();
	void XYMove();
	void axisXYMove( double UseStart, double UseAcc, double UseSpeed);
	float getUserMove(float f);
	

private:
	int curAxis;
	double curStart;
	double curSpeed;
	double curAcc;

	double curZStart;
	double curZSpeed;
	double curZAcc;

	bool pciState;
	
	float m_xPercent;
	float m_yPercent;
	float m_zPercent;
};

