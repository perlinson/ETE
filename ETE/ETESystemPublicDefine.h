#ifndef _SYSTEM_PUBLIC_DEFINE_H_
#define _SYSTEM_PUBLIC_DEFINE_H_

#include "stdafx.h"
#include "camera.h"
#include "math.h"

// system return value
#define  NO_ERR							0
#define  SEND_CMD_OK					0
#define	 DEV_STATUS_OK					0
#define  SEND_CMD_ERR					-1
#define  USB_DEVICE_NOT_OPEN			-2
#define  CHANNEL_NOT_EXSIT				-3
#define	 IO_SETTING_INVALID				-4
#define  GET_LOOP_IO_INFO_ERR			-5
#define  SEL_WAVE_TYPE_ERR				-6
#define  CAMERA_NOT_IDENTIFY			-7
#define  CAMERA_ALREADY_USED			-8
#define  CAMERA_THREAD_UNAVIALBLE		-9
#define	 CAMERA_NOT_START_WORKING		-10
#define  CAMERA_CONFIG_PRAR_FAILED		-11
#define  CAMERA_INPUT_CAL_PRAR_ERR		-12
#define	 CONFIG_DIRECT_ACCESS_FAILED	-13
#define	 CAMERA_UNAVIALABLE				-14
#define  CAMERA_SET_ACQUISITION_FAILED	-15
#define  CAMERA_VIEW_UNAVAILABLE		-16
#define  XMT_DEV_INIT_FAILED			-17
#define  ARM_DEV_INIT_FAILED			-18
#define  IMG_GENERATOR_DEV_INIT_FAILED	-19
#define  CAMERA_DEV_INIT_FAILED			-20
#define  LED_DEV_INIT_FAILED			-21
#define  SYS_ALL_PAR_NOT_INITIALIZED	-22
#define  POS_CONTROLLOER_DEV_INIT_FAILED		-23
#define	 USB_DEVICE_NOT_EXIST			-23


#define  START_CAMERA_FAILED				-23
#define  START_MEAS_CAMERA_CONFIG_FAILED	-24
#define  START_XMT_FAILED					-25
#define  START_DMD_CONFIG_FAILED			-26
#define  START_ARM_CONFIG_FAILED			-27
#define  START_ARM_CONFIG_PAR_ERR			-28


#define DMD_ERR_DEV_NOT_EXSIT			-100
#define DMD_ERR_IN_PAR_INVALID			-101
#define DMD_FILE_NOT_EXSIT				-102
#define DMD_NO_MEM						-103
#define DMD_DEV_INIT_FAILED				-104


#define ARM_ERR_DEV_NOT_EXSIT			-200
#define ARM_ERR_CONFIG_MODE_INVALID		-201
#define ARM_ERR_INIT_DEV				-202
#define ARM_ERR_CONFIG_PARAMETER		-203

//add system message IDs
#define WM_CAMERA_STATE				WM_USER + 1
#define WM_CAMERA_NEWFRAME			WM_USER + 2
#define WM_CAMERA_SAVEBMP_COMPLETED	WM_USER + 3

#define WM_COMM_BREAK_DETECTED		WM_USER+21	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_USER+22	// The CTS (clear-to-send) signal changed state. 
#define WM_COMM_DSR_DETECTED		WM_USER+23	// The DSR (data-set-ready) signal changed state. 
#define WM_COMM_ERR_DETECTED		WM_USER+24	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY. 
#define WM_COMM_RING_DETECTED		WM_USER+25	// A ring indicator was detected. 
#define WM_COMM_RLSD_DETECTED		WM_USER+26	// The RLSD (receive-line-signal-detect) signal changed state. 
#define WM_COMM_RXCHAR				WM_USER+27	// A character was received and placed in the input buffer. 
#define WM_COMM_RXFLAG_DETECTED		WM_USER+28	// The event character was received and placed in the input buffer.  
#define WM_COMM_TXEMPTY_DETECTED	WM_USER+29	// The last character in the output buffer was sent.  



//PosController
#define POS_CONSOLE_X_SPEED_EXCEED		1
#define POS_CONSOLE_X_RANGE_EXCEED		2
#define POS_CONSOLE_Y_SPEED_EXCEED		3
#define POS_CONSOLE_Y_RANGE_EXCEED		4
#define POS_CONSOLE_Z_SPEED_EXCEED		5
#define POS_CONSOLE_Z_RANGE_EXCEED		6

#define  MAX_IMG_GEN_NUM 6
//
//enum
//{
//	FBN_BUSRESETBEGIN=0,                  // pData=NULL
//	FBN_BUSRESETEND,                      // pData=NULL
//	FBN_CREATEDEVICE,                     // pData=&FBNCREATE
//	FBN_NEWDEVICE,                        // pData=&FireNode
//	FBN_KILLDEVICE,                       // pData=&FireNode
//	FBN_ERROR,                            // pData=&Error
//	FBN_FLAGGEDERROR,                     // pData=&ErrorFlags
//	FBN_FRAME,                            // pData=&CameraObject
//	FBN_DEBUG,                            // Debug message
//	FBN_LAST
//};

#define MAX_CAMERA_SUPPORTED	2

enum CAMERA_IDX
{
	CAMERA_GRAY,
	CAMERA_COLOR,
};

typedef struct
{
	TCHAR  Directory[MAX_PATH];
	TCHAR  NamePrefix[128];
	UINT32 FrameCount;
	UINT32 Divider;
}RECORDINFO;

typedef struct ST_CAMERA_INPUT_PAR
{
	/****************************************************************************/
	//待设定参数集
	UINT32    VidSetNumber;		//No.1 Mode select
	UINT8     IsoSpeed;         //No.2 ISO speed
	UINT8     TriggerMode;      //No.3 Triggermode
	UINT32    BufferCnt;        //No.4 Number of frames to allocate
	UINT8     BayerPattern;     //No.5 Used bayer pattern
	UINT8     Y16InversOrder;   //No.6 Inversmode for Y16
	UINT8     YIsRawMode;       //No.7 Raw mode flag
	UINT16    XPos;             //No.8 X-Offset
	UINT16    YPos;             //No.9 Y-Offset
	UINT16    XSize;            //No.10 X-Dimension
	UINT16    YSize;            //No.11 Y-Dimension
	UINT8     ColorFormat;      //No.12 Actual ColorFormat
	UINT16    PktSize;          //No.13 Payload per packet
	/***********************************************************************/

	/****************************************************************************/
	//待设定参数集
	FEATPROP	FeatPropGain,FeatPropBrightness, FeatShutter, FeatGamma, FeatAutoExposure, FeatWhiteBal;
	/****************************************************************************/

	/****************************************************************************/
	//待设定参数集
	UINT8	IsSrollMode;
	/****************************************************************************/

	//待设定参数集 保存图片相关
	BOOL			IsoReported;
	BOOL			IsNotified ;
	DWORD           StartTime;            // Starttime for acquisition
	RECORDINFO      RecordInfo;           // Our recording info
	UINT32          RecordFrameCount;
	UINT32          RecordDivider;
}ST_CAMERA_INPUT_PAR;


typedef struct ST_CAMERA_SCOPE
{
	float start;
	float end;
}ST_CAMERA_SCOPE;


typedef struct ST_ARM_CONSOLE_DEV_INF_PARA
{
	short	iBraudRate;
	BYTE  	iDelayTime;
	BYTE	iPriodTime;
	BYTE	iPulseNum;
}ST_ARM_CONSOLE_DEV_INF_PARA;

typedef struct ST_DMD_PARAMETERS
{
	/*************参数********************/
	int m_iSynMode;						//1: 内同步 0： 外同步
	int m_iIntegraiTime;				//积分时间设置
	int m_iShowGray;					//是否是灰度图像
	int m_iFrame;						//缓存设置
	int m_iPulseWidth;					//帧频设置
	int m_iThd;							//阈值设置

	CString m_strFilePath;				//发送文件路径

	char *m_pBitMap[MAX_IMG_GEN_NUM];
	int m_iBitMapCurNum;
}ST_DMD_PARAMETERS;


enum MEASURE_MODE
{
	M_OBSERVER,
	M_PSI,
	M_VSI,
	M_CONFOCAL,
	M_HIGH_RESOLUTION
};

enum CMDs
{
	CMD_BEGIN = 0,

	CMD_SET_TIME_UNIT,			//没有用到
	CMD_SET_PULSE_NUM,			//设置被动触发脉冲数
	CMD_GET_DEV_NAME,			//获取设备名，没有用到                      _____        ______
	CMD_GET_DELAY,				//设置被动触发延迟    位移器发出的移动信号       |______|
	CMD_GET_PERIOD,                         //设置触发周期
	CMD_WLEDON_SET_LUM,			//设置一路LED的亮度
	CMD_GLEDON_SET_LUM,                     //设置另一路LED的亮度
	//CMD_DMD_ON,				//没用
	CMD_SRS_TRIG_NUM,			//设置连续触发次数
	CMD_SRS_TRG_PRD,			//设置连续触发周期  单位ms
	CMD_SRS_TRG_START,		//连续触发开始命令，
	CMD_SRS_TRG_ENTER,              //关闭被动触发模式，进入连续触发模式 （下位机将关闭外部中断，即不响应压电陶瓷控制器发出的位移信号）
	//调用CMD_SRS_TRG_START前需要先调用CMD_SRS_TRG_ENTER,注意，调用CMD_SRS_TRG_ENTER的时候，压电陶瓷控制器必须处于
	//待机状态，即发送这条命令的时候 压电陶瓷应该是停止状态，这样能保证ARM正确接收到这条命令及时关闭外部触发
	CMD_SRS_TRG_END,			//退出连续触发模式，连续模式使用完毕以后需要调用这条命令，这样ARM才能继续工作在正常的触发模式下
	CMD_SET_DEFALUT,			//返回默认设置
	CMD_SRS_TRG_2_START,    //mod3 带延迟连续触发开始信号                    
	CMD_SRS_TRG_2_ENTER,    //mod3 进入mod3连续触发模式                      
	CMD_SRS_TRG_2_END,      //mod3 退出mod3连续触发模式                      

	CMD_BLEDON_SET_LUM,     //2015.1.27新增加 
	CMD_END
};

enum CMD_STATUS
{
	CMD_STATUS_BEGIN = 0,

	CMD_SUCCESS,
	CMD_DEV_NOT_REPONSE,

	CMD_STATUS_END
};

typedef struct CMD_FORMAT
{
	char headId[3];			// 'cmd'
	BYTE bOpcode;			//命令代码
	BYTE bPara;				//该命令参数
	BYTE bChecksum;			//校验和

	BYTE bReserveSz;			//保留字节长度
	//BYTE *pReservedBuffer;	//保留字节buffer
}CMD_FORMAT;

typedef struct ARM_PARA 
{
	int delay;
	int period;
	int pulseNum;
}ARM_PARA;

typedef struct CMD_STATUS_T
{
	char headId[3];				// 'cmd'
	BYTE bOpcode;				//命令代码
	BYTE bResult;				//命令执行结果
	BYTE bChecksum;				//校验和

	BYTE bReserveSz;			//保留字节长度
	//BYTE *pReservedBuffer;	//保留字节buffer
}CMD_STATUS_T;

typedef struct T_CMDSend
{
	int idx;
	unsigned char chCmdPar[256];
	unsigned char chCmdRet[256];
	unsigned char nSendSz;
	unsigned char nRecieveSz;
}T_CMD;



static void SleepMs(int timesspan)
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

typedef enum LOOP_STATUS
{
	LOOP_NONE,
	LOOP_OPEN,
	LOOP_CLOSE,
};

typedef enum IO_CONFIGURATION
{
	IO_NONE,
	IO_INPUT,
	IO_OUTPUT,
};

typedef struct XMT_SYS_INFO
{
	float systemMinV;			//系统最小电压
	float systemMaxV;			//系统最小电压

	float systemMinStep;		//系统最小位移
	float systemMaxStep_ch1;	//一路闭环陶瓷最大位移
	float systemMaxStep_ch2;	//二路闭环陶瓷最大位移
	float systemMaxStep_ch3;	//三路闭环陶瓷最大位移

	float systemPower_ch1;		//一路功率
	float systemPower_ch2;		//二路功率
	float systemPower_ch3;		//三路功率

}ST_XMT_SYS_INFO;

typedef struct DEV_LOOP_IO_CONFIGURATION_INFO
{
	unsigned char LoopState_ch1;		//一路通道开闭环信息('I'---闭环,'O'---开环)
	unsigned char LoopState_ch2;		//二路通道开闭环信息('I'---闭环,'O'---开环)
	unsigned char LoopState_ch3;		//三路通道开闭环信息('I'---闭环,'O'---开环)

	unsigned char IOState_ch1;			//一路通道IO信息('I'---输入 ,'O'---输出)
	unsigned char IOState_ch2;			//二路通道IO信息('I'---输入 ,'O'---输出)
	unsigned char IOState_ch3;			//三路通道IO信息('I'---输入 ,'O'---输出)
}ST_DEV_IO_CONFIGURATION_INFO;

#define	START_STOP_SPEED_HI	'H'
#define START_STOP_SPEED_LW	'S'

typedef struct DEV_START_STOP_SPEED
{
	unsigned char startSpeed_ch1;		//一路通道波形的启停、速度信息('H'---高速,'S'---低速)
	unsigned char stopSpeed_ch1;			//一路通道波形的启停、速度信息('H'---高速,'S'---低速)

	unsigned char startSpeed_ch2;		//二路通道波形的启停、速度信息('H'---高速,'S'---低速)
	unsigned char stopSpeed_ch2;			//二路通道波形的启停、速度信息('H'---高速,'S'---低速)

	unsigned char startSpeed_ch3;		//三路通道波形的启停、速度信息('H'---高速,'S'---低速)
	unsigned char stopSpeed_ch3;			//三路通道波形的启停、速度信息('H'---高速,'S'---低速)

}ST_DEV_START_STOP_SPEED;

typedef enum
{
	DEV_INTERFACE_NONE,
	DEV_INTERFACE_USB,
	DEV_INTERFACE_COM
};

typedef struct ST_INTERFACE_PARA
{
	union
	{
		struct 
		{
		}usb;

		struct  
		{
			short iSerialNum;
			short iBraudRate;
		}com;

	};

}ST_INTERFACE_PARA;


enum enuMeasureType
{
	MeasureType_Confocal,
	MeasureType_Confocal3D,
	MeasureType_PSI,
	MeasureType_VSI,

};

enum
{
	Selection_Select = 1,
	Selection_Reset = -1
};

const int MAX_MASK_COUNT = 3;

const int VIEW_HEIGHT = 580;
const int VIEW_WIDTH = 780;


static float land[]= {	
	0.999000f, 0.200000f, 0.200000f, 1.0f,
	0.999000f, 0.207843f, 0.207843f, 1.0f,
	0.999000f, 0.215686f, 0.215686f, 1.0f,
	0.999000f, 0.223529f, 0.223529f, 1.0f,
	0.999000f, 0.231373f, 0.231373f, 1.0f,
	0.999000f, 0.239216f, 0.239216f, 1.0f,
	0.999000f, 0.247059f, 0.247059f, 1.0f,
	0.999000f, 0.254902f, 0.254902f, 1.0f,
	0.999000f, 0.262745f, 0.262745f, 1.0f,
	0.999000f, 0.270588f, 0.270588f, 1.0f,
	0.999000f, 0.278431f, 0.278431f, 1.0f,
	0.999000f, 0.286275f, 0.286275f, 1.0f,
	0.999000f, 0.294118f, 0.294118f, 1.0f,
	0.999000f, 0.301961f, 0.301961f, 1.0f,
	0.999000f, 0.309804f, 0.309804f, 1.0f,
	0.999000f, 0.317647f, 0.317647f, 1.0f,
	0.999000f, 0.325490f, 0.325490f, 1.0f,
	0.999000f, 0.333333f, 0.333333f, 1.0f,
	0.999000f, 0.341176f, 0.341176f, 1.0f,
	0.999000f, 0.349020f, 0.349020f, 1.0f,
	0.999000f, 0.356863f, 0.356863f, 1.0f,
	0.999000f, 0.364706f, 0.364706f, 1.0f,
	0.999000f, 0.372549f, 0.372549f, 1.0f,
	0.999000f, 0.380392f, 0.380392f, 1.0f,
	0.999000f, 0.388235f, 0.388235f, 1.0f,
	0.999000f, 0.396078f, 0.396078f, 1.0f,
	0.999000f, 0.403922f, 0.403922f, 1.0f,
	0.999000f, 0.411765f, 0.411765f, 1.0f,
	0.999000f, 0.419608f, 0.419608f, 1.0f,
	0.999000f, 0.427451f, 0.427451f, 1.0f,
	0.999000f, 0.435294f, 0.435294f, 1.0f,
	0.999000f, 0.443137f, 0.443137f, 1.0f,
	0.999000f, 0.450980f, 0.450980f, 1.0f,
	0.999000f, 0.458824f, 0.458824f, 1.0f,
	0.999000f, 0.466667f, 0.466667f, 1.0f,
	0.999000f, 0.474510f, 0.474510f, 1.0f,
	0.999000f, 0.482353f, 0.482353f, 1.0f,
	0.999000f, 0.490196f, 0.490196f, 1.0f,
	0.999000f, 0.498039f, 0.498039f, 1.0f,
	0.999000f, 0.505882f, 0.505882f, 1.0f,
	0.999000f, 0.513725f, 0.513725f, 1.0f,
	0.999000f, 0.521569f, 0.521569f, 1.0f,
	0.999000f, 0.529412f, 0.529412f, 1.0f,
	0.999000f, 0.537255f, 0.537255f, 1.0f,
	0.999000f, 0.545098f, 0.545098f, 1.0f,
	0.999000f, 0.552941f, 0.552941f, 1.0f,
	0.999000f, 0.560784f, 0.560784f, 1.0f,
	0.999000f, 0.568627f, 0.568627f, 1.0f,
	0.999000f, 0.576471f, 0.576471f, 1.0f,
	0.999000f, 0.584314f, 0.584314f, 1.0f,
	0.999000f, 0.592157f, 0.592157f, 1.0f,
	0.999000f, 0.600000f, 0.600000f, 1.0f,
	0.999000f, 0.607843f, 0.607843f, 1.0f,
	0.999000f, 0.615686f, 0.615686f, 1.0f,
	0.999000f, 0.623529f, 0.623529f, 1.0f,
	0.999000f, 0.631373f, 0.631373f, 1.0f,
	0.999000f, 0.639216f, 0.639216f, 1.0f,
	0.999000f, 0.647059f, 0.647059f, 1.0f,
	0.999000f, 0.654902f, 0.654902f, 1.0f,
	0.999000f, 0.662745f, 0.662745f, 1.0f,
	0.999000f, 0.670588f, 0.670588f, 1.0f,
	0.999000f, 0.678431f, 0.678431f, 1.0f,
	0.999000f, 0.686275f, 0.686275f, 1.0f,
	0.999000f, 0.694118f, 0.694118f, 1.0f,
	0.999000f, 0.701961f, 0.701961f, 1.0f,
	0.999000f, 0.709804f, 0.709804f, 1.0f,
	0.999000f, 0.717647f, 0.717647f, 1.0f,
	0.999000f, 0.725490f, 0.725490f, 1.0f,
	0.999000f, 0.733333f, 0.733333f, 1.0f,
	0.999000f, 0.741176f, 0.741176f, 1.0f,
	0.999000f, 0.749020f, 0.749020f, 1.0f,
	0.999000f, 0.756863f, 0.756863f, 1.0f,
	0.999000f, 0.764706f, 0.764706f, 1.0f,
	0.999000f, 0.772549f, 0.772549f, 1.0f,
	0.999000f, 0.780392f, 0.780392f, 1.0f,
	0.999000f, 0.788235f, 0.788235f, 1.0f,
	0.999000f, 0.796078f, 0.796078f, 1.0f,
	0.999000f, 0.803922f, 0.803922f, 1.0f,
	0.999000f, 0.811765f, 0.811765f, 1.0f,
	0.999000f, 0.819608f, 0.819608f, 1.0f,
	0.999000f, 0.827451f, 0.827451f, 1.0f,
	0.999000f, 0.835294f, 0.835294f, 1.0f,
	0.999000f, 0.843137f, 0.843137f, 1.0f,
	0.999000f, 0.850980f, 0.850980f, 1.0f,
	0.999000f, 0.858824f, 0.858824f, 1.0f,
	0.999000f, 0.866667f, 0.866667f, 1.0f,
	0.999000f, 0.874510f, 0.874510f, 1.0f,
	0.999000f, 0.882353f, 0.882353f, 1.0f,
	0.999000f, 0.890196f, 0.890196f, 1.0f,
	0.999000f, 0.898039f, 0.898039f, 1.0f,
	0.999000f, 0.905882f, 0.905882f, 1.0f,
	0.999000f, 0.913725f, 0.913725f, 1.0f,
	0.999000f, 0.921569f, 0.921569f, 1.0f,
	0.999000f, 0.929412f, 0.929412f, 1.0f,
	0.999000f, 0.937255f, 0.937255f, 1.0f,
	0.999000f, 0.945098f, 0.945098f, 1.0f,
	0.999000f, 0.952941f, 0.952941f, 1.0f,
	0.984375f,0.984375f,0.984375f,1.0f,
	0.96875f,0.984375f,0.984375f,1.0f,
	0.953125f,0.984375f,0.984375f,1.0f,
	0.9375f,0.984375f,0.984375f,1.0f,
	0.90625f,0.984375f,0.984375f,1.0f,
	0.890625f,0.984375f,0.984375f,1.0f,
	0.875f,0.984375f,0.984375f,1.0f,
	0.84375f,0.984375f,0.984375f,1.0f,
	0.828125f,0.984375f,0.984375f,1.0f,
	0.8125f,0.984375f,0.984375f,1.0f,
	0.78125f,0.984375f,0.984375f,1.0f,
	0.765625f,0.984375f,0.984375f,1.0f,
	0.75f,0.984375f,0.984375f,1.0f,
	0.71875f,0.984375f,0.984375f,1.0f,
	0.703125f,0.984375f,0.984375f,1.0f,
	0.6875f,0.984375f,0.984375f,1.0f,
	0.65625f,0.984375f,0.984375f,1.0f,
	0.640625f,0.984375f,0.984375f,1.0f,
	0.625f,0.984375f,0.984375f,1.0f,
	0.609375f,0.984375f,0.984375f,1.0f,
	0.578125f,0.984375f,0.984375f,1.0f,
	0.5625f,0.984375f,0.984375f,1.0f,
	0.546875f,0.984375f,0.984375f,1.0f,
	0.515625f,0.984375f,0.984375f,1.0f,
	0.5f,0.984375f,0.984375f,1.0f,
	0.484375f,0.984375f,0.984375f,1.0f,
	0.453125f,0.984375f,0.984375f,1.0f,
	0.4375f,0.984375f,0.984375f,1.0f,
	0.421875f,0.984375f,0.984375f,1.0f,
	0.390625f,0.984375f,0.984375f,1.0f,
	0.375f,0.984375f,0.984375f,1.0f,
	0.359375f,0.984375f,0.984375f,1.0f,
	0.328125f,0.984375f,0.984375f,1.0f,
	0.3125f,0.984375f,0.984375f,1.0f,
	0.296875f,0.984375f,0.984375f,1.0f,
	0.28125f,0.984375f,0.984375f,1.0f,
	0.25f,0.984375f,0.984375f,1.0f,
	0.234375f,0.984375f,0.984375f,1.0f,
	0.21875f,0.984375f,0.984375f,1.0f,
	0.1875f,0.984375f,0.984375f,1.0f,
	0.171875f,0.984375f,0.984375f,1.0f,
	0.15625f,0.984375f,0.984375f,1.0f,
	0.125f,0.984375f,0.984375f,1.0f,
	0.109375f,0.984375f,0.984375f,1.0f,
	0.09375f,0.984375f,0.984375f,1.0f,
	0.0625f,0.984375f,0.984375f,1.0f,
	0.046875f,0.984375f,0.984375f,1.0f,
	0.03125f,0.984375f,0.984375f,1.0f,
	0.0f,0.984375f,0.984375f,1.0f,
	0.0f,0.96875f,0.984375f,1.0f,
	0.0f,0.953125f,0.984375f,1.0f,
	0.0f,0.9375f,0.984375f,1.0f,
	0.0f,0.90625f,0.984375f,1.0f,
	0.0f,0.890625f,0.984375f,1.0f,
	0.0f,0.875f,0.984375f,1.0f,
	0.0f,0.84375f,0.984375f,1.0f,
	0.0f,0.828125f,0.984375f,1.0f,
	0.0f,0.8125f,0.984375f,1.0f,
	0.0f,0.78125f,0.984375f,1.0f,
	0.0f,0.765625f,0.984375f,1.0f,
	0.0f,0.75f,0.984375f,1.0f,
	0.0f,0.71875f,0.984375f,1.0f,
	0.0f,0.703125f,0.984375f,1.0f,
	0.0f,0.6875f,0.984375f,1.0f,
	0.0f,0.65625f,0.984375f,1.0f,
	0.0f,0.640625f,0.984375f,1.0f,
	0.0f,0.625f,0.984375f,1.0f,
	0.0f,0.609375f,0.984375f,1.0f,
	0.0f,0.578125f,0.984375f,1.0f,
	0.0f,0.5625f,0.984375f,1.0f,
	0.0f,0.546875f,0.984375f,1.0f,
	0.0f,0.515625f,0.984375f,1.0f,
	0.0f,0.5f,0.984375f,1.0f,
	0.0f,0.484375f,0.984375f,1.0f,
	0.0f,0.453125f,0.984375f,1.0f,
	0.0f,0.4375f,0.984375f,1.0f,
	0.0f,0.421875f,0.984375f,1.0f,
	0.0f,0.390625f,0.984375f,1.0f,
	0.0f,0.375f,0.984375f,1.0f,
	0.0f,0.359375f,0.984375f,1.0f,
	0.0f,0.328125f,0.984375f,1.0f,
	0.0f,0.3125f,0.984375f,1.0f,
	0.0f,0.296875f,0.984375f,1.0f,
	0.0f,0.28125f,0.984375f,1.0f,
	0.0f,0.25f,0.984375f,1.0f,
	0.0f,0.234375f,0.984375f,1.0f,
	0.0f,0.21875f,0.984375f,1.0f,
	0.0f,0.1875f,0.984375f,1.0f,
	0.0f,0.171875f,0.984375f,1.0f,
	0.0f,0.15625f,0.984375f,1.0f,
	0.0f,0.125f,0.984375f,1.0f,
	0.0f,0.109375f,0.984375f,1.0f,
	0.0f,0.09375f,0.984375f,1.0f,
	0.0f,0.0625f,0.984375f,1.0f,
	0.0f,0.046875f,0.984375f,1.0f,
	0.0f,0.03125f,0.984375f,1.0f,
	0.0f,0.0f,0.984375f,1.0f,
	0.0f,0.0f,0.96875f,1.0f,
	0.0f,0.0f,0.953125f,1.0f,
	0.0f,0.0f,0.9375f,1.0f,
	0.0f,0.0f,0.921875f,1.0f,
	0.0f,0.0f,0.90625f,1.0f,
	0.0f,0.0f,0.890625f,1.0f,
	0.0f,0.0f,0.875f,1.0f,
	0.0f,0.0f,0.859375f,1.0f,
	0.0f,0.0f,0.84375f,1.0f,
	0.0f,0.0f,0.828125f,1.0f,
	0.0f,0.0f,0.8125f,1.0f,
	0.0f,0.0f,0.796875f,1.0f,
	0.0f,0.0f,0.78125f,1.0f,
	0.0f,0.0f,0.765625f,1.0f,
	0.0f,0.0f,0.75f,1.0f,
	0.0f,0.0f,0.734375f,1.0f,
	0.0f,0.0f,0.71875f,1.0f,
	0.0f,0.0f,0.703125f,1.0f,
	0.0f,0.0f,0.6875f,1.0f,
	0.0f,0.0f,0.671875f,1.0f,
	0.0f,0.0f,0.65625f,1.0f,
	0.0f,0.0f,0.640625f,1.0f,
	0.0f,0.0f,0.625f,1.0f,
	0.0f,0.0f,0.609375f,1.0f,
	0.0f,0.0f,0.59375f,1.0f,
	0.0f,0.0f,0.578125f,1.0f,
	0.0f,0.0f,0.5625f,1.0f,
	0.0f,0.0f,0.546875f,1.0f,
	0.0f,0.0f,0.53125f,1.0f,
	0.0f,0.0f,0.515625f,1.0f,
	0.0f,0.0f,0.5f,1.0f,
	0.0f,0.0f,0.484375f,1.0f,
	0.0f,0.0f,0.46875f,1.0f,
	0.0f,0.0f,0.453125f,1.0f,
	0.0f,0.0f,0.4375f,1.0f,
	0.0f,0.0f,0.421875f,1.0f,
	0.0f,0.0f,0.40625f,1.0f,
	0.0f,0.0f,0.390625f,1.0f,
	0.0f,0.0f,0.375f,1.0f,
	0.0f,0.0f,0.359375f,1.0f,
	0.0f,0.0f,0.34375f,1.0f,
	0.0f,0.0f,0.328125f,1.0f,
	0.0f,0.0f,0.3125f,1.0f,
	0.0f,0.0f,0.296875f,1.0f,
	0.0f,0.0f,0.28125f,1.0f,
	0.0f,0.0f,0.265625f,1.0f,
	0.0f,0.0f,0.25f,1.0f,
	0.0f,0.0f,0.234375f,1.0f,
	0.0f,0.0f,0.21875f,1.0f,
	0.0f,0.0f,0.203125f,1.0f,
	0.0f,0.0f,0.1875f,1.0f,
	0.0f,0.0f,0.171875f,1.0f,
	0.0f,0.0f,0.15625f,1.0f,
	0.0f,0.0f,0.140625f,1.0f,
	0.0f,0.0f,0.125f,1.0f,
	0.0f,0.0f,0.109375f,1.0f,
	0.0f,0.0f,0.09375f,1.0f,
	0.0f,0.0f,0.078125f,1.0f,
	0.0f,0.0f,0.0625f,1.0f,
	0.0f,0.0f,0.046875f,1.0f,
	0.0f,0.0f,0.03125f,1.0f,
	0.0f,0.0f,0.0f,1.0f,
	0.0f,0.0f,0.0f,1.0f
};

static float hsv[]= {	
	0.0f,0.0f,0.562500,
	0.0f,0.0f,0.625000,
	0.0f,0.0f,0.687500,
	0.0f,0.0f,0.750000,
	0.0f,0.0f,0.812500,
	0.0f,0.0f,0.875000,
	0.0f,0.0f,0.937500,
	0.0f,0.0f, 1,
	0.0f,0.062500,  1.0f,
	0.0f,0.125000,	1.0f,
	0.0f,0.187500,	1.0f,
	0.0f,0.250000,	1.0f,
	0.0f,0.312500,	1.0f,
	0.0f,0.375000,	1.0f,
	0.0f,0.437500,	1.0f,
	0.0f,0.500000,	1.0f,
	0.0f,0.562500,	1.0f,
	0.0f,0.625000,	1.0f,
	0.0f,0.687500,	1.0f,
	0.0f,0.750000,	1.0f,
	0.0f,0.812500,	1.0f,
	0.0f,0.875000,	1.0f,
	0.0f,0.937500,	1.0f,
	0.0f,1.0f, 1.0f,
	0.0625,1,0.93750,
	0.1250,1,0.87500,
	0.1875,1,0.81250,
	0.2500,1,0.75000,
	0.3125,1,0.68750,
	0.3750,1,0.62500,
	0.4375,1,0.56250,
	0.5000,1,0.50000,
	0.5625,1,0.43750,
	0.6250,1,0.37500,
	0.6875,1,0.31250,
	0.7500,1,0.25000,
	0.8125,1,0.18750,
	0.8750,1,0.12500,
	0.9375,1,0.06250,
	1.0f, 1.0f,	  0.0f,
	1.0f, 0.9375, 0.0f,
	1.0f, 0.8750, 0.0f,
	1.0f, 0.8125, 0.0f,
	1.0f, 0.7500, 0.0f,
	1.0f, 0.6875, 0.0f,
	1.0f, 0.6250, 0.0f,
	1.0f, 0.5625, 0.0f,
	1.0f, 0.5000, 0.0f,
	1.0f, 0.4375, 0.0f,
	1.0f, 0.3750, 0.0f,
	1.0f, 0.3125, 0.0f,
	1.0f, 0.2500, 0.0f,
	1.0f, 0.1875, 0.0f,
	1.0f, 0.1250, 0.0f,
	1.0f, 0.06250,0.0f,
	1.0f, 0.0f,	  0.0f,
	0.9375,0.0f, 0.0f,
	0.8750,0.0f, 0.0f,
	0.8125,0.0f, 0.0f,
	0.7500,0.0f, 0.0f,
	0.6875,0.0f, 0.0f,
	0.6250,0.0f, 0.0f,
	0.5625,0.0f, 0.0f,
	0.5000,0.0f, 0.0f
};


#define  wm_renderSlice (WM_USER + 1)

#endif //_SYSTEM_PUBLIC_DEFINE_H_