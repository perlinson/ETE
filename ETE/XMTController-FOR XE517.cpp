#include "StdAfx.h"
#include "XMTController.h"

#pragma comment(lib, "./Libraries/XMT_USB/DLL_XMT_USB.lib")
#pragma comment(lib, "./Libraries/CyUsb/cyApi.lib")

CXMTDev::CXMTDev(void)
	: m_eInterfaceType(DEV_INTERFACE_NONE),
	m_bDevInterfaceOK(false)
{
	m_eInterfaceType = DEV_INTERFACE_USB;
}


CXMTDev::~CXMTDev(void)
{
}


// 初始化用到的接口
int CXMTDev::InitDevInterface( ST_INTERFACE_PARA stIntfPara )
{
	BOOL bInitDevOK = FALSE;

	//设备已经打开
	if (m_bDevInterfaceOK)
	{
		return TRUE;
	}


	if (DEV_INTERFACE_COM == m_eInterfaceType)
	{
		TRACE("XMT::Initializing COM interface ....\n");
		m_pSerialDev.reset(new CMSComm);

		CString strComInit;
		if(m_pSerialDev->GetPortOpen())
			m_pSerialDev->SetPortOpen(FALSE);//用GetPortOpen()来返回现在的计算机端口    
		m_pSerialDev->SetCommPort(stIntfPara.com.iSerialNum); // 指定串口号
		strComInit.Format(_T("%d"), stIntfPara.com.iBraudRate);
		strComInit+=",N,8,1";
		m_pSerialDev->SetSettings(strComInit); // 通信参数设置
		m_pSerialDev->SetInBufferSize(1024); // 指定接收缓冲区大小
		m_pSerialDev->SetOutBufferSize(1024);//发送缓冲区
		m_pSerialDev->SetInBufferCount(0); // 清空接收缓冲区
		m_pSerialDev->SetInputMode(1); // 设置数据获取方式
		m_pSerialDev->SetInputLen(0); // 设置读取方式
		m_pSerialDev->SetPortOpen(1); // 打开指定的串口
		m_pSerialDev->GetInput();//先预读缓冲区以清除残留数据

		m_bDevInterfaceOK = TRUE;
	}
	else if (DEV_INTERFACE_USB == m_eInterfaceType)
	{
		
		TRACE("XMT::Initializing USB interface ....\n");

		m_pUsbDev.reset(new CCyUSBDevice);

		int num_of_USB = NUM_OF_TOTAL_USB_DEVICE(m_pUsbDev.get());

		if (num_of_USB == 0)
		{
			TRACE("Has no usb device connected \r\n");
			return USB_DEVICE_NOT_EXIST;
		}

		bInitDevOK =m_pUsbDev->Open(num_of_USB -1);

	


		if (bInitDevOK)
		{
			read_system_T_P_R(10,'A','B','C');
			Read_V_S_Status(1);
			m_bDevInterfaceOK = TRUE;
			return TRUE;
		}

		TRACE("OpenUsb Failed \r\n");

		
	}
	else
	{
		
	}

	return USB_DEVICE_NOT_OPEN;;
}


// 关闭设备接口
void CXMTDev::DeInitDevInterface(void)
{
	if (!m_bDevInterfaceOK)
	{
		return;
	}

	//若设备已经初始化
	if (DEV_INTERFACE_COM == m_eInterfaceType)
	{

	}
	else if (DEV_INTERFACE_USB == m_eInterfaceType)
	{
		m_pUsbDev->Close();

		BOOL isOpen = m_pUsbDev->IsOpen();

		if (isOpen)
		{
			TRACE(_T("Close Device Failed!"));
		}
	}
	else
	{

	}
}


// 和设备接口进行握手
BOOL CXMTDev::ShakeHandWithDevInterface(void)
{
	if (!m_bDevInterfaceOK)
	{
		return false;
	}

	//若设备已经初始化
	if (DEV_INTERFACE_COM == m_eInterfaceType)
	{

	}
	else if (DEV_INTERFACE_USB == m_eInterfaceType)
	{
	}
	else
	{

	}
	
	return true;
}

BOOL CXMTDev::CheckLoopState()
{
	int iLoopState;

	if(NO_ERR != GetLoopConfig(1, &iLoopState))
	{
		TRACE(TEXT("GetLoopConfig(1, &iLoopState) failed"));
		return FALSE;
	}
	if(LOOP_CLOSE != iLoopState)
	{
		TRACE(TEXT("iLoopState != LOOP_Close failed"));
		return FALSE;
	}
	return TRUE;
}


// 定位单点

/*
	  命令格式: SendSiglePosCmd('S','A',m_point_MVS);
	  命令说明: 发送一路位移数据
	  通    道: 'A'---一路通道
	            'B'---二路通道
			    'C'---三路通道
	  
	  变    量: point_MVS 输入的位移数值 该数值应该在最小位移和最大位移数据
				V_OR_S 开闭环选择 'V'开环 'S'闭环

	  返回数据: ****
*/
int CXMTDev::SendSinglePosCmd(LOOP_STATUS V_or_S, int iChannel, float point_MVS)
{
	//LOOP_STATUS loopStatus = LOOP_NONE;

	//if(!m_bDevInterfaceOK || !m_pUsbDev)
	//	return SEND_CMD_ERR;

	//loopStatus = Read_V_S_Status(iChannel);

	//if (LOOP_NONE == loopStatus || V_or_S != loopStatus)
	//{
	//	return SEND_CMD_ERR;
	//}
	
	unsigned char chV_or_S = 'N';
	unsigned char chChannel = 'N';

	if (chV_or_S == LOOP_OPEN)
	{
		chV_or_S = 'V';
	}
	else
	{
		chV_or_S = 'S';
	}

	chChannel = 'A' + (iChannel - 1);
	float curPos = 0.0;
	
	SEND_V_COMMAND_SINGLE(m_pUsbDev.get(), 14, 'T', chV_or_S, chChannel, point_MVS);

	return NO_ERR;
}

int CXMTDev::SendMultiPosCmdEx(LOOP_STATUS V_or_S, int iChannel, float point_MVS[], int num)
{
	LOOP_STATUS loopStatus = LOOP_NONE;

	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return SEND_CMD_ERR;


	unsigned char chV_or_S = 'N';
	unsigned char chChannel = 'N';

	if (chV_or_S == LOOP_OPEN)
	{
		chV_or_S = 'V';
	}
	else
	{
		chV_or_S = 'S';
	}

	chChannel = 'A' + (iChannel - 1);
	float curPos = 0.0;

	for(int i = 0; i < num; ++i)
	{
		SEND_V_COMMAND_SINGLE(m_pUsbDev.get(), 14, 'T', chV_or_S, chChannel, point_MVS[i]);
		SleepMs(50);
	}
	

	return NO_ERR;
}

void CXMTDev::SleepMs(int timesspan)
{
	long   timeon   =   GetTickCount();
	long   timeoff   =   timeon;
	MSG   msg;
	do{
		if   (::PeekMessage(&msg,NULL,0,0,PM_REMOVE))   {   //从Windows消息队列中取出消息
			if   (msg.message==   WM_QUIT)//如果消息为退出，发送退出消息
			{
				::PostQuitMessage(-1);
			}
			if(!AfxGetApp()-> PreTranslateMessage(&msg))//如果无法预处理消息
			{
				::TranslateMessage(&msg);//转换消息
				::DispatchMessage(&msg);//发送消息
			}
			timeoff   =   GetTickCount();
		}
	}while(timeoff-timeon <timesspan);
}

// 定位多点
int CXMTDev::SendMultiPosCmd(LOOP_STATUS loopStatus, int iChannel, float send_data[], float send_time[], int iArrSz)
{
		
	// TODO: Add your control notification handler code here
	LOOP_STATUS loopStatusCur = LOOP_NONE;

	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return SEND_CMD_ERR;

	loopStatusCur = Read_V_S_Status(iChannel);

	if (LOOP_NONE == loopStatusCur || loopStatusCur != loopStatus)
	{
		return SEND_CMD_ERR;
	}

	unsigned char chV_or_S;
	unsigned char chChannel;

	if (loopStatus == LOOP_OPEN)
	{
		chV_or_S = 'V';
	}
	else
	{
		chV_or_S = 'S';
	}

	chChannel = 'A' + (iChannel - 1);

	float sec_use = 20;//维持的时间长度


	SEND_ARR_DATA_TIME_NUM_NUMPOINT(m_pUsbDev.get(),send_data,send_time,iArrSz,chChannel,chV_or_S,iArrSz/2);
	//SEND_ARR_DATA_TIME_NUM(m_pUsbDev.get(), send_data, send_time, iArrSz, chChannel, chV_or_S, sec_use);
	//SEND_V_COMMAND_SINGLE(m_pUsbDev.get(), 14, 'T', chV_or_S, chChannel, 0); //发送命令零点
	
	return NO_ERR;
#if 0
	//发送连续单点数据 
    //SEND_ARR_DATA_TIME_NUM(XMT_USBDEVICE,float send_data[],float send_time[],int N_ARR,char CHANNEL,char V_OR_S,float Send_time);
	/*
	float send_data[]; //发送的数据
	float send_time[]; //数据对应的时间
	int seg_send_data ;//发送的数据个数
	char CHANNEL;//通道数
	char V_OR_S;//开闭环 选择 'V'开环 'S'闭环
	float Send_time;//运行时间长短
	*/
	int  N_ARR;//定义发送数据个数
	N_ARR = 10;
	float send_data[10];
	float dis_time[10];  
	int i=0;
	for(i=0;i<N_ARR;i++)
	    send_data[i] = i*10;
	for(i=0;i<N_ARR;i++) 
	    dis_time[i] = 1; //1 对应1毫秒  0.5对应 500微秒	
	float sec_use=20;//维持的时间长度


	SEND_ARR_DATA_TIME_NUM(XMT_USBDEVICE,send_data,dis_time,N_ARR,'A','V',sec_use);
	SEND_V_COMMAND_SINGLE(XMT_USBDEVICE,14,'T','V','A',0); //发送命令零点
#endif
}


// 读取开闭环状态
LOOP_STATUS CXMTDev::Read_V_S_Status(int iChannel)
{

	LOOP_STATUS loopStatus = LOOP_NONE;

	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return loopStatus;

	if(iChannel < 0 || iChannel > 3)
		return loopStatus;

	DEV_LOOP_IO_CONFIGURATION_INFO devLoopAndIOConiguration;

	if (NO_ERR != GetLoopAndIOConfiguration(&devLoopAndIOConiguration))
	{
		return loopStatus;
	}

	unsigned char chLoopState;

	switch(iChannel)
	{
	case 1:
		chLoopState = devLoopAndIOConiguration.LoopState_ch1;
		break;
	case 2:
		chLoopState = devLoopAndIOConiguration.LoopState_ch2;
		break;
	case 3:
		chLoopState = devLoopAndIOConiguration.LoopState_ch3;
		break;
	}

    if(chLoopState == 'O')
	{
		loopStatus = LOOP_OPEN;
	}
	else
	{
		loopStatus = LOOP_CLOSE;
	}

	return loopStatus;
}


// 读取位移器的系统信息
int CXMTDev::ReadSysInfo(ST_XMT_SYS_INFO *stSysInfo)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	//读系统参数
	read_system_T_P_R(m_pUsbDev.get(), 10, 'T', 'P', 'R');

    /*
	  命令格式: read_system_T_P_R(10,'T','P','R');
	  命令长度: 10/2=5
	  命令说明: read_system_T_P_R(10,'T','P','R');
	            读取下位机的系统信息
	*/
	Sleep(200);
	float recive_data_usb[9];	
	read_System_real_info_16(recive_data_usb,'R','P');

	stSysInfo->systemMinV = recive_data_usb[0];
	stSysInfo->systemMaxV = recive_data_usb[1];

	stSysInfo->systemMinStep = recive_data_usb[2];
	stSysInfo->systemMaxStep_ch1 = recive_data_usb[3];
	stSysInfo->systemMaxStep_ch2 = recive_data_usb[4];
	stSysInfo->systemMaxStep_ch3 = recive_data_usb[5];
	
	stSysInfo->systemPower_ch1 = recive_data_usb[6];
	stSysInfo->systemPower_ch2 = recive_data_usb[7];
	stSysInfo->systemPower_ch3 = recive_data_usb[8];
	
	return SEND_CMD_OK;
}


// 设置负载
int CXMTDev::SetLoadSz(int iChannel, float LoadSz )
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	if(iChannel < 0 || iChannel > 3)
		return CHANNEL_NOT_EXSIT;

	unsigned char chChannel = 'A' + (iChannel - 1);

	write_system_fuzai(m_pUsbDev.get(), 16, 'T', 'L', 'W', chChannel, LoadSz);	 //标定一路负载

	/*
	  命令格式: write_system_fuzai(16,'T','L','W','A',1.9);	 //标定一路负载
	  命令长度: 16/2=8
	  命令说明: 标定一路负载
	  通    道: 'A'---一路通道
	            'B'---二路通道
			    'C'---三路通道
	  变    量: 1.9 为标定的一路通道的负载数值 该数值根据实际情况填写

	  返回数据: ****
	*/

	return SEND_CMD_OK;
}


// 读取负载
int CXMTDev::GetLoadSz(int iChannel, float *LoadSz)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	if(iChannel < 0 || iChannel > 3)
		return CHANNEL_NOT_EXSIT;

	unsigned char chChannel = 'A' + (iChannel - 1);

	float float_fzai;

	read_system_fuzai(m_pUsbDev.get(), 12, 'T', 'L', 'R', chChannel);
	 /*
	  命令格式: read_system_T_P_R(10,'T','R','A');
	  命令长度: 10/2=5
	  命令说明: read_system_T_P_R(10,'T','R','A');
	  通    道: 发送预备读取1路的负载数值 
	            通  道
	                  'A'---一路通道
	                  'B'---二路通道
					  'C'---三路通道
	         
	*/
	Sleep(200);
	float_fzai = read_System_real_V_S_and_fuzai('R', 'L', chChannel); //根据不同的命令来调用
	 /*
	  命令格式:  read_System_real_V_S_and_fuzai('R','L','A'); 该命令配合上面的
	  命令长度: **
	  命令说明: 上位机从USB端口读取一路电压命令(接收命令)  read_System_real_V_S_and_fuzai('R','L','A');
	  通    道: 读取1路的负载数值 
		        通  道
		        'A'---一路通道
		        'B'---二路通道			    
				'C'---三路通道

	  返回数据: 使用float_fzai来接收数据信息 
	*/

	*LoadSz = float_fzai;

	return SEND_CMD_OK;
}


// 设置设备启停速度
int CXMTDev::SetDevStartAndStopSpeed(int iChannel, ST_DEV_START_STOP_SPEED stSpeed)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	if(iChannel < 0 || iChannel > 3)
		return CHANNEL_NOT_EXSIT;

	unsigned char chChannel = 'A' + (iChannel - 1);

	unsigned char chStartSpeed, chStopSpeed;

	switch(iChannel)
	{
	case 1:
		chStartSpeed = stSpeed.startSpeed_ch1;
		chStopSpeed = stSpeed.stopSpeed_ch1;
		break;
	case 2:
		chStartSpeed = stSpeed.startSpeed_ch2;
		chStopSpeed = stSpeed.stopSpeed_ch2;
		break;
	case 3:
		chStartSpeed = stSpeed.startSpeed_ch3;
		chStopSpeed = stSpeed.stopSpeed_ch3;
		break;
	default:
		chStartSpeed = 'L';
		chStopSpeed = 'L';
	}
	//开始 //停止
	send_wave_mode_CH_H_S(m_pUsbDev.get(), 14, 'T', 'M', chChannel, chStartSpeed, chStopSpeed);

	//设置波形启停速度
   	/*
	  命令格式:  send_wave_mode_CH_H_S(14,'T','M','A','H','H');
	  命令长度:  14/2=7
	  命令说明:  send_wave_mode_CH_H_S(14,'T','M','A',WAVE_BEGIN_SPEED,WAVE_END_SPEED); 
	           通  道: 
	             'A'---一路通道
	             'B'---二路通道
			     'C'---三路通道
               WAVE_BEGIN_SPEED:  波形起始速度 'H'表示高速 'S'表示低速
	           WAVE_END_SPEED  :  波形停止速度 'H'表示高速 'S'表示低速			  
	/*  
	  例如:
	  USER_USB.send_wave_mode_CH_H_S(14,'T','M','A','H','S'); //一路启动为低
	  
	  USER_USB.send_wave_mode_CH_H_S(14,'T','M','B','H','H');
		
	  USER_USB.send_wave_mode_CH_H_S(14,'T','M','C','H','H');	
	*/
	return SEND_CMD_OK;
}


// 获取设备启停速度
int CXMTDev::GetDevStartAndStopSpeedAllChannel(ST_DEV_START_STOP_SPEED *stSpeed)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

#if 0
	if(iChannel < 0 || iChannel > 3)
		return CHANNEL_NOT_EXSIT;

	char chChannel = 'A' + (iChannel - 1);
#endif

	read_system_T_P_R(m_pUsbDev.get(),10,'T','M','R');//读取下位机的波形的启动模式	

	/*
	  命令格式: read_system_T_P_R(10,'T','M','R');
	  命令长度: 10/2=5
	  命令说明: read_system_T_P_R(10,'T','M','R');
	  通    道: 读取下位机的波形的启动模式
	*/

	Sleep(200);
	unsigned char  recive_data_usb_wave_send_mode[6];	
	read_System_real_info_wave_begin_stop(recive_data_usb_wave_send_mode,'R','M');

	/*
	  命令格式:  read_System_real_info_wave_begin_stop(recive_data_usb_wave_send_mode,'R','M'); 该命令配合上面的
	  命令长度: **
	  命令说明: 上位机从USB端口读取1,2,3路波形启停速度 , read_System_real_info_wave_begin_stop(recive_data_usb_wave_send_mode,'R','M');
	  通    道: 读取1，2，3路,波形启停速度
	  返回数据: 使用recive_data_usb_wave_send_mode[6]数组来接收1,2,3路波形的启停、速度
	            其中状态信息包括1，2，3路通道的路波形的启停、速度
                recive_data_usb_wave_send_mode[0] -- 一路通道波形的启停、速度信息('H'---高速,'S'---低速)
				recive_data_usb_wave_send_mode[1] -- 一路通道波形的启停、速度信息('H'---高速,'S'---低速)

				recive_data_usb_wave_send_mode[2] -- 二路通道波形的启停、速度信息('H'---高速,'S'---低速)
                recive_data_usb_wave_send_mode[3] -- 二路通道波形的启停、速度信息('H'---高速,'S'---低速)

				recive_data_usb_wave_send_mode[4] -- 三路通道波形的启停、速度信息('H'---高速,'S'---低速)
				recive_data_usb_wave_send_mode[5] -- 三路通道波形的启停、速度信息('H'---高速,'S'---低速)
	*/

	stSpeed->startSpeed_ch1 = recive_data_usb_wave_send_mode[0];
	stSpeed->stopSpeed_ch1 = recive_data_usb_wave_send_mode[1];

	stSpeed->startSpeed_ch2 = recive_data_usb_wave_send_mode[2];
	stSpeed->stopSpeed_ch2 = recive_data_usb_wave_send_mode[3];

	stSpeed->startSpeed_ch3 = recive_data_usb_wave_send_mode[4];
	stSpeed->stopSpeed_ch3 = recive_data_usb_wave_send_mode[5];

	return SEND_CMD_OK;
}


// 读取单点坐标
int CXMTDev::ReadSiglePosCmd(LOOP_STATUS V_or_S, int iChannel, float *Val_VS)
{

	LOOP_STATUS loopStatus = LOOP_NONE;

	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	loopStatus = Read_V_S_Status(iChannel);

	if (LOOP_NONE == loopStatus || V_or_S != loopStatus)
	{
		return SEND_CMD_ERR;
	}

	unsigned char chV_or_S = 'N';
	unsigned char chChannel = 'N';

	if (chV_or_S == LOOP_OPEN)
	{
		chV_or_S = 'V';
	}
	else
	{
		chV_or_S = 'S';
	}

	chChannel = 'A' + (iChannel - 1);

	read_System_V_S(m_pUsbDev.get(), 12, 'T', 'D', chV_or_S, chChannel);
	/*
	  命令格式: read_System_V_S(12,'T','D','V','A');
	  命令长度:12/2=6
	  命令说明:发送预备读取一路电压命令  read_System_V_S(12,'T','D','V','A');
	  通    道:'A'---一路通道
	           'B'---二路通道
			   'C'---三路通道
	*/
	Sleep(200);
	*Val_VS = read_System_real_V_S_and_fuzai('R', chV_or_S, chChannel);
	/*
	  命令格式: read_System_real_V_S_and_fuzai('R','V','A'); 该命令配合上面的
	  命令长度: **
	  命令说明:上位机从USB端口读取一路电压命令(接收命令)  read_System_real_V_S_and_fuzai('R','V','A');
	  通    道:'A'---一路通道
	           'B'---二路通道
			   'C'---三路通道
	  返回数据:一路的电压数值(读取到一路的电压数值)
	*/
	return SEND_CMD_OK;
}


// 使能上位机通过USB控制
int CXMTDev::EnablePCConsole(void)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	read_system_T_P_R(m_pUsbDev.get(),10,'A','B','C'); //发送上位机控制命令 'A','B','C'：通知下位机做好通信准备 10/2=5为通信命令的长度

	return NO_ERR;
}


// 设置IO配置
int CXMTDev::SetIOConfig(int iChannel, int IO_type)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	if(iChannel < 0 || iChannel > 3)
		return CHANNEL_NOT_EXSIT;

	if(IO_type != IO_INPUT && IO_type != IO_OUTPUT)
		return IO_SETTING_INVALID;

	unsigned char chChannel = 'A' + (iChannel - 1);

	unsigned char chIOtype;
	if (IO_type == IO_INPUT)
	{
		chIOtype = 'I';
	}
	else if (IO_type == IO_OUTPUT)
	{
		chIOtype = 'O';
	}

	//写IO口
    write_System_IO(m_pUsbDev.get(), 12, 'T', 'I', chChannel, chIOtype); //一路输出 'I'为输入模式
	
	/*
	  命令格式: write_System_IO(12,'T','I','A','O'); //一路输出 'I'为输入模式
	  命令长度: 12/2=6
	  命令说明: write_System_IO(12,'T','I','A','O'); //一路输出 'I'为输入模式
	  通    道:'A'---一路通道
	           'B'---二路通道
			   'C'---三路通道
	  输入输出:
	           'O'---输出
			   'I'---输入  
	*/
	return NO_ERR;
}


// 获取IO配置
int CXMTDev::GetIOConfig(int iChannel, int *IO_type)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	if(iChannel < 0 || iChannel > 3)
		return CHANNEL_NOT_EXSIT;

	DEV_LOOP_IO_CONFIGURATION_INFO devLoopAndIOConiguration;

	if (NO_ERR != GetLoopAndIOConfiguration(&devLoopAndIOConiguration))
	{
		return GET_LOOP_IO_INFO_ERR;
	}

	unsigned char chIOState;

	switch(iChannel)
	{
	case 1:
		chIOState = devLoopAndIOConiguration.IOState_ch1;
		break;
	case 2:
		chIOState = devLoopAndIOConiguration.IOState_ch2;
		break;
	case 3:
		chIOState = devLoopAndIOConiguration.IOState_ch3;
		break;
	}

	if(chIOState == 'O')
	{
		*IO_type = IO_OUTPUT;
	}
	else
	{
		*IO_type = IO_OUTPUT;
	}

	return NO_ERR;
}

int CXMTDev::GetLoopConfig(int iChannel, int *Loop_type)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	if(iChannel < 0 || iChannel > 3)
		return CHANNEL_NOT_EXSIT;

	DEV_LOOP_IO_CONFIGURATION_INFO devLoopAndIOConiguration;

	if (NO_ERR != GetLoopAndIOConfiguration(&devLoopAndIOConiguration))
	{
		return GET_LOOP_IO_INFO_ERR;
	}

	unsigned char chLoopState;

	switch(iChannel)
	{
	case 1:
		chLoopState = devLoopAndIOConiguration.LoopState_ch1;
		break;
	case 2:
		chLoopState = devLoopAndIOConiguration.LoopState_ch2;
		break;
	case 3:
		chLoopState = devLoopAndIOConiguration.LoopState_ch3;
		break;
	}

	if(chLoopState == 'O')
	{
		*Loop_type = LOOP_OPEN;
	}
	else
	{
		*Loop_type = LOOP_CLOSE;
	}

	return NO_ERR;
}

// 获取系统开闭环，IO配置信息
int CXMTDev::GetLoopAndIOConfiguration(DEV_LOOP_IO_CONFIGURATION_INFO *pConfiguration)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	//读IO口状态
    read_system_T_P_R(m_pUsbDev.get(),10,'T','T','R');
	/*
	  命令格式: read_system_T_P_R(10,'T','T','R');
	  命令长度: 10/2=5
	  命令说明: read_system_T_P_R(10,'T','T','R');
	  通    道: 发送预备读取1，2，3路的IO口状态命令  
	*/
	Sleep(200);
	unsigned char temp_io_info[6];
	read_System_real_info_IO(temp_io_info,'R','T');	 //这里包含有IO口的状态情况

	/*
	  命令格式:  read_System_real_info_IO(temp_io_info,'R','T'); 该命令配合上面的
	  命令长度: **
	  命令说明: 上位机从USB端口读取一路电压命令(接收命令)  read_System_real_V_S_and_fuzai('R','V','A');
	  通    道: 读取1，2，3路的IO口状态命令
	  返回数据: 使用temp_io_info[6]数组来接接收IO口的状态信息 
	            其中状态信息包括1，2，3路通道的IO口的设置信息 
                temp_io_info[0] -- 一路通道开闭环信息('I'---闭环,'O'---开环)
				temp_io_info[1] -- 二路通道开闭环信息('I'---闭环,'O'---开环)
				temp_io_info[2] -- 三路通道开闭环信息('I'---闭环,'O'---开环)

                temp_io_info[3] -- 一路通道IO信息('I'---输入 ,'O'---输出)
				temp_io_info[4] -- 二路通道IO信息('I'---输入 ,'O'---输出)
				temp_io_info[5] -- 三路通道IO信息('I'---输入 ,'O'---输出)
	*/

	pConfiguration->LoopState_ch1 = temp_io_info[0];
	pConfiguration->LoopState_ch2 = temp_io_info[1];
	pConfiguration->LoopState_ch3 = temp_io_info[2];

	pConfiguration->IOState_ch1 = temp_io_info[3];
	pConfiguration->IOState_ch2 = temp_io_info[4];
	pConfiguration->IOState_ch3 = temp_io_info[5];

	return NO_ERR;
}


// 设置波形
int CXMTDev::SetWaveConfiguration(int iChannel, int iWaveIdx, float amp, float freq, float pZhi)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	if(iChannel < 0 || iChannel > 3)
		return CHANNEL_NOT_EXSIT;

	if(iWaveIdx < 0 || iWaveIdx > 4)
		return SEL_WAVE_TYPE_ERR;

	unsigned char chChannel = 'A' + (iChannel - 1);
	unsigned char chWaveType = 'A' + (iWaveIdx - 1);

	/*加入相关的协议就行*/
	send_T_Wave(m_pUsbDev.get(), 24, 'T', 'W', chChannel, chWaveType, amp, freq, pZhi);

	/*
	  命令格式: send_T_Wave(24,'T','W','A','A',3,3,3);
	  命令长度: 24/2=12
	  命令说明: send_T_Wave(24,'T','W','A','A',3,3,3);

      命   名:
	  send_T_Wave(24,'T','W',CHANNEL_TYPE,WAVE_TYPE,FUZHI,P_Hz,P_Zhi);
	  
	            CHANNEL_TYPE: 通    道
				             'A'---一路通道
							 'B'---二路通道
							 'C'---三路通道
				   WAVE_TYPE:
				             'A'--- 正弦
				             'B'--- 方波
							 'C'--- 三角波
							 'D'--- 锯齿波
                   FUZHI    : 幅值
                   P_Hz     : 频率
                   P_Zhi    : 偏执					
	*/
	return NO_ERR;
}


// 开始波形
int CXMTDev::StartWave(void)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	read_system_T_P_R(m_pUsbDev.get(), 10, 'T', 'W', 'S');

	return NO_ERR;
}


// 停止波形
int CXMTDev::StopWave(void)
{
	if(!m_bDevInterfaceOK || !m_pUsbDev)
		return USB_DEVICE_NOT_OPEN;

	read_system_T_P_R(m_pUsbDev.get(), 10, 'T', 'W', 'F');

	return NO_ERR;
}
