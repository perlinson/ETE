#include "StdAfx.h"
#include "ArmDev.h"

ArmDev* ArmDev::pInstance = nullptr; 
CCriticalSection ArmDev::m_sCriticalSection; 

ArmDev::ArmDev()
	: m_bDevInterfaceOK(FALSE),
	m_pSerialPort(NULL)
{
	initCmdPara();
}


ArmDev::~ArmDev(void)
{
	if(m_pSerialPort)
	{
		delete m_pSerialPort;
		m_pSerialPort = NULL;
	}
}

// 初始化用到的接口
BOOL ArmDev::InitDevInterface()
{
	BOOL bInitDevOK = FALSE;

	TRACE("XMT::Initializing COM interface ....\n");
	m_pSerialPort = new CSerialPort();
	if(!m_pSerialPort)
		return FALSE;

	CWnd *pWnd = AfxGetMainWnd();

	if(m_pSerialPort->InitPort(pWnd, 3 ,19200,'N',8,1,EV_RXFLAG | EV_RXCHAR,512000))
	//if(m_pSerialPort->InitPort(m_pHwnd, serivalNum,stIntfPara.iBraudRate,'N',8,1,EV_RXFLAG | EV_RXCHAR,512000))
	{
		m_bDevInterfaceOK = TRUE;
	}

	bInitDevOK = TRUE;

	return (m_bDevInterfaceOK == bInitDevOK);
}


// 关闭设备接口
void ArmDev::DeInitDevInterface(void)
{
	if (!m_bDevInterfaceOK)
	{
		return;
	}

	//若设备已经初始化
	if(m_pSerialPort)
	{
		delete m_pSerialPort;
		m_pSerialPort = NULL;
	}

	m_bDevInterfaceOK = FALSE;
}


// 和设备接口进行握手
BOOL ArmDev::ShakeHandWithDevInterface(void)
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	//若设备已经初始化
	

	m_cmdDetail.bOpcode = CMD_GET_DEV_NAME;
	m_cmdDetail.bPara = 0;
	m_cmdDetail.bChecksum = 3;
	m_cmdDetail.bReserveSz = 1;
	
	return SendCmd();
}

BOOL ArmDev::ConfigParameters(ST_ARM_CONSOLE_DEV_INF_PARA stIntfPara)
{
	if (!m_bDevInterfaceOK)
	{
		TRACE(TEXT("ARM_DEVICE_NOT_EIST \r\n"));
		return FALSE;
	}

	if (!SetDelay(stIntfPara.iDelayTime))
	{
		TRACE(TEXT("SetDelay Failed \r\n"));
		return FALSE;
	}

	Sleep(50);

	if (!SetPeriod(stIntfPara.iPriodTime))
	{
		TRACE(TEXT("SetPeriod Failed \r\n"));
		return FALSE;
	}

	Sleep(50);

	if (!SetPulseNum(stIntfPara.iPulseNum))
	{
		TRACE(TEXT("SetPulseNum Failed \r\n"));
		return FALSE;
	}
	return TRUE;
}

// 发送命令
BOOL ArmDev::SendCmd()
{
	T_CMD cmd;
	memcpy(cmd.chCmdPar, &m_cmdDetail, sizeof(CMD_FORMAT));
	cmd.nSendSz = sizeof(CMD_FORMAT);
	if (!m_bDevInterfaceOK)
	{
		TRACE(TEXT("ARM_DEVICE_NOT_EIST \r\n"));
		return FALSE;
	}

	int bytesWrited = 0;
	m_pSerialPort->WriteToPort(cmd.chCmdPar, cmd.nSendSz, bytesWrited);

	bool successed = bytesWrited != 0;
	SleepMs(50);
	return successed;
}

BOOL ArmDev::SetPulseNum( BYTE num )
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SET_PULSE_NUM;
	m_cmdDetail.bPara = num;
	m_cmdDetail.bChecksum = num + CMD_SET_PULSE_NUM;
	return SendCmd();
}


BOOL ArmDev::SetPeriod( BYTE period )
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_GET_PERIOD;
	m_cmdDetail.bPara = period;
	m_cmdDetail.bChecksum = period + CMD_GET_PERIOD;
	
	return SendCmd();
}


BOOL ArmDev::SetDelay( BYTE delay )
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_GET_DELAY;
	m_cmdDetail.bPara = delay;
	m_cmdDetail.bChecksum = delay + CMD_GET_DELAY;

	return SendCmd();
}

BOOL ArmDev::SetWhiteStrength(int strength)
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_WLEDON_SET_LUM;
	m_cmdDetail.bPara = strength;
	m_cmdDetail.bChecksum = strength + CMD_WLEDON_SET_LUM;
	return SendCmd();
}

void  ArmDev::initCmdPara()
{
	m_cmdDetail.headId[0] = 'c'; 
	m_cmdDetail.headId[1] = 'm'; 
	m_cmdDetail.headId[2] = 'd'; 
	m_cmdDetail.bOpcode = CMD_BEGIN;
	m_cmdDetail.bPara = 0;
	m_cmdDetail.bChecksum = 0;
}

BOOL ArmDev::SetBlueStrength(int strength)
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_BLEDON_SET_LUM;
	m_cmdDetail.bPara = strength;
	
	m_cmdDetail.bChecksum = strength + CMD_BLEDON_SET_LUM;
	return SendCmd();
}

BOOL ArmDev::SetTriggerNum(int howManyTime)
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SRS_TRIG_NUM;

	m_cmdDetail.bPara = howManyTime / 10;
	m_cmdDetail.bReserveSz = 10;
	//m_cmdDetail.bPara = howManyTime % 0xff;
	//m_cmdDetail.bReserveSz = (howManyTime / 0xff) + 1;
	m_cmdDetail.bChecksum = m_cmdDetail.bPara + CMD_SRS_TRIG_NUM;
	return SendCmd();
}

BOOL ArmDev::StartTrigger()
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SRS_TRG_START;
	m_cmdDetail.bPara = 0;
	m_cmdDetail.bChecksum = CMD_SRS_TRG_START;
	return SendCmd();
}

BOOL ArmDev::StartNewTrigger()
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SRS_TRG_2_START;
	m_cmdDetail.bPara = 0;
	m_cmdDetail.bChecksum = CMD_SRS_TRG_2_START;
	return SendCmd();
}

BOOL ArmDev::SetNewTriggerEnter()
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SRS_TRG_2_ENTER;
	m_cmdDetail.bPara = 0;
	m_cmdDetail.bChecksum = CMD_SRS_TRG_2_ENTER;
	return SendCmd();
}

BOOL ArmDev::SetNewTriggerLeave()
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SRS_TRG_2_END;
	m_cmdDetail.bPara = 0;
	m_cmdDetail.bChecksum = CMD_SRS_TRG_2_END;
	return SendCmd();
}

BOOL ArmDev::SetTriggerPeriod(int triggerPeriod)
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SRS_TRG_PRD;
	m_cmdDetail.bPara = triggerPeriod;
	m_cmdDetail.bChecksum = triggerPeriod + CMD_SRS_TRG_PRD;
	return SendCmd();
}

BOOL ArmDev::SetTriggerEnter()
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SRS_TRG_ENTER;
	m_cmdDetail.bPara = 0;
	m_cmdDetail.bChecksum = CMD_SRS_TRG_ENTER;
	return SendCmd();
}

BOOL ArmDev::SetTriggerLeave()
{
	if (!m_bDevInterfaceOK)
	{
		return FALSE;
	}

	m_cmdDetail.bOpcode = CMD_SRS_TRG_END;
	m_cmdDetail.bPara = 0;
	m_cmdDetail.bChecksum = CMD_SRS_TRG_END;
	return SendCmd();
}

ArmDev * ArmDev::GetInstance()
{
	if(pInstance == nullptr)  
	{   //double check  
		m_sCriticalSection.Lock();           //用lock实现线程安全，用资源管理类，实现异常安全  
		//使用资源管理类，在抛出异常的时候，资源管理类对象会被析构，析构总是发生的无论是因为异常抛出还是语句块结束。  
		pInstance = new ArmDev();
		if (!pInstance)
		{
			return nullptr;
		}

		
		m_sCriticalSection.Unlock();
	}  
	return pInstance;  
}

void ArmDev::Destroy()
{
	DeInitDevInterface();

	if (pInstance)
	{
		delete pInstance;
		pInstance = nullptr;
	}
}