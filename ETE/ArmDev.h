#pragma once

#include "SerialPort.h"
#include "ETESystemPublicDefine.h"

class ArmDev
{
public:
	static ArmDev *GetInstance();  
	static ArmDev *pInstance;  
	static CCriticalSection m_sCriticalSection;
	void Destroy();

	// 初始化用到的接口
	BOOL InitDevInterface();
	// 关闭设备接口
	void DeInitDevInterface(void);
	// 和设备接口进行握手
	BOOL ShakeHandWithDevInterface(void);
	// 配置选择模式
	BOOL ConfigParameters(ST_ARM_CONSOLE_DEV_INF_PARA stIntfPara);

	BOOL SetPulseNum( BYTE num );
	BOOL SetPeriod( BYTE period );
	BOOL SetTriggerNum( int howManyTime);
	BOOL SetDelay( BYTE delay);
	BOOL SetWhiteStrength(int strength);
	BOOL SetBlueStrength(int strength);
	BOOL StartNewTrigger();
	BOOL SetNewTriggerEnter();
	BOOL SetNewTriggerLeave();

	BOOL StartTrigger();
	BOOL SetTriggerEnter();
	BOOL SetTriggerLeave();
	BOOL SetTriggerPeriod(int triggerPeriod);
private:
	ArmDev();
	~ArmDev(void);
private:
	CSerialPort* m_pSerialPort;
	// 当前设备接口状态
	BOOL m_bDevInterfaceOK;
	CMD_FORMAT m_cmdDetail;
	void initCmdPara();
	// 发送命令
	BOOL SendCmd();

};

