#pragma once

/*
*	Create by Jianbo 2013-11-3
*/
#include "mscomm.h"
#include "./Libraries/CyUsb/CyAPI.h"
#include "./Libraries/XMT_USB/DLL_XMT_USB.h"
#include "ETESystemPublicDefine.h"
#include <memory>
class CXMTDev
{
public:
	CXMTDev(void);
	~CXMTDev(void);

public:
	// 初始化用到的接口
	int InitDevInterface(ST_INTERFACE_PARA stIntfPara);
	// 关闭设备接口
	void DeInitDevInterface(void);
	
private:
	// 使用接口类型（USB or Com...）
	unsigned char m_eInterfaceType;
	std::shared_ptr<CMSComm> m_pSerialDev;
	std::shared_ptr<CCyUSBDevice> m_pUsbDev;
	// 当前设备接口状态
	BOOL m_bDevInterfaceOK;
public:
	// 和设备接口进行握手
	BOOL ShakeHandWithDevInterface(void);
	// 定位单点
	int SendSinglePosCmd(LOOP_STATUS V_or_S, int iChannel, double point_MVS);
	// 读取单点坐标
	int ReadSiglePosCmd(LOOP_STATUS V_or_S, int iChannel, float *Val_VS);

	// 定位多点
	int SendMultiPosCmd(float fengfengzhi, float pinlv, float pianzhi);
	// 读取开闭环状态
	LOOP_STATUS Read_V_S_Status(int iChannel);
	// 读取位移器的系统信息
	int ReadSysInfo(ST_XMT_SYS_INFO *stSysInfo);
	// 设置负载
	int SetLoadSz(int iChannel, float LoadSz );
	// 读取负载
	int GetLoadSz(int iChannel, float *LoadSz);
	// 设置设备启停速度
	int SetDevStartAndStopSpeed(int iChannel, ST_DEV_START_STOP_SPEED stSpeed);
	// 获取设备启停速度
	int GetDevStartAndStopSpeedAllChannel(ST_DEV_START_STOP_SPEED *stSpeed);
	// 使能上位机通过USB控制
	int EnablePCConsole(void);
	// 设置IO配置
	int SetIOConfig(int iChannel, int IO_type);
	// 获取IO配置
	int GetIOConfig(int iChannel, int *IO_type);
	// 获取开闭环配置
	int GetLoopConfig(int iChannel, int *Loop_type);
	// 获取系统开闭环，IO配置信息
	int GetLoopAndIOConfiguration(DEV_LOOP_IO_CONFIGURATION_INFO *pConfiguration);
	// 设置波形
	int SetWaveConfiguration(int iChannel, int iWaveIdx, float amp, float freq, float pZhi);
	// 开始波形
	int StartWave(void);
	// 停止波形
	int StopWave(void);
	void SleepMs(int timesspan);
	int SendMultiPosCmdEx(LOOP_STATUS V_or_S, int iChannel, float point_MVS[], int num);
	BOOL CheckLoopState();
	void SetSz();
	void SetCloseState();
	BOOL GetCloseState();
	void Stop();
private:
	unsigned char Now_control_mcu_address;
	int m_num_device;
};

