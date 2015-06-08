

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DLL_XMT_USB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DLL_XMT_USB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#include "cyapi.h"

#ifdef DLL_XMT_USB_EXPORTS
#define DLL_XMT_USB_API __declspec(dllexport)
#else
#define DLL_XMT_USB_API __declspec(dllimport)
#endif

// This class is exported from the DLL_XMT_USB.dll
class DLL_XMT_USB_API CDLL_XMT_USB {
public:
	CDLL_XMT_USB(void);
	// TODO: add your methods here.
	unsigned char*  DataAnla_Pro(double f,unsigned char kk[4]);
	void send_num_Arr_char(unsigned char arry[],int num_of_arr,CCyUSBDevice *USBDevice);
	void receive_usb_Arr(CCyUSBDevice  *USBDevice,unsigned char receive_arr[]); //读取的USB的数据值
	double XMT_ReDo_pro(unsigned char comand_Arr[]);
	void receive_pro_v_move_only(CCyUSBDevice *USBDevice,unsigned char reciv[]); //z读取电压 或 是位移的数据
	double Res_command_pro(unsigned char T_D_3,unsigned char T_D_4);//解包命令	 
    float CalData(unsigned char kk0,unsigned char kk1,unsigned char kk2,unsigned char kk3);//用来转化 正负数
	void XMT_ReadMultReal(  unsigned char comand_Arr[],
		                  	unsigned char *OpenOrCloseFlag_0,
							double *Data_0,
							unsigned char *OpenOrCloseFlag_1,
							double *Data_1,
							unsigned char *OpenOrCloseFlag_2,
							double *Data_2
					);//将采集的数组进行分包处理
	 //实际的分包处理过程
	void CDLL_XMT_USB::XMT_ReadMultReal_Do( unsigned char T_D_3,unsigned char T_D_4,
									   	unsigned char *OpenOrCloseFlag_0,
										double *Data_0,
										unsigned char *OpenOrCloseFlag_1,
										double *Data_1,
										unsigned char *OpenOrCloseFlag_2,
										double *Data_2
										);
};

extern DLL_XMT_USB_API int nDLL_XMT_USB;

DLL_XMT_USB_API int fnDLL_XMT_USB(void);
//////////////////////////////////////////////////////

int DLL_XMT_USB_API NUM_OF_TOTAL_USB_DEVICE_LABVIEW(); //检测现在开启的USB设备
int DLL_XMT_USB_API Open_Num_Of_USB_LABVIEW(int NUM_OF_USB); //连接编号为NUM的USB高速设备
bool DLL_XMT_USB_API IsOpenUsbDevice_LABVIEW();//查看USB设备是否打开
bool DLL_XMT_USB_API ReSetUSBDevice_LABVIEW();//重置USB接口设备

USHORT DLL_XMT_USB_API GetVID_LABVIEW(); //VID
USHORT DLL_XMT_USB_API GetPID_LABVIEW(); //PID

///////////////////////////////////////////////////////
DLL_XMT_USB_API void initUSB();

//单点类  0 1  
DLL_XMT_USB_API void XMT_COMMAND_SinglePoint(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char Channel_Num,
					double VoltOrMove_Data
					); //
//多路单点类 2 3  
DLL_XMT_USB_API void  XMT_COMMAND_MultSinglePoint(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					double VoltOrMove_Data_0,
					double VoltOrMove_Data_1,
					double VoltOrMove_Data_2
					);
//清零命令 4
DLL_XMT_USB_API void 	XMT_COMMAND_SinglePoint_Clear(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4
					);

//读数据类 5 6 
DLL_XMT_USB_API double XMT_COMMAND_ReadData(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char Channel_Num
					);

// 7 8 
DLL_XMT_USB_API void 	XMT_COMMAND_ReadData_TS(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char Channel_Num,
					unsigned char TimerSet_ms
					);

// 9 10 
DLL_XMT_USB_API void 	XMT_COMMAND_ReadData_TS_MultChannle(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char TimerSet_ms
					);
//停止读取 11
DLL_XMT_USB_API void 	XMT_COMMAND_ReadData_Stop(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4
					);
//波形类
//单路高速  12 13 
DLL_XMT_USB_API void 	XMT_COMMAND_WaveSetHighSingle(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char Channel_Num,
					unsigned char WaveType,
					double FengFengZhi,
					double PinLvHz,
					double Pianzhi
					);
//停止发送  14 
DLL_XMT_USB_API void 	XMT_COMMAND_WaveSetHighSingleStop(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4
					);

//多路标准速度模式 15 16 
DLL_XMT_USB_API void 	XMT_COMMAND_WaveSetMultWave(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char Channel_Num,
					unsigned char WaveType,
					double FengFengZhi,
					double PinLvHz,
					double Pianzhi
					);

//停止发送 17
DLL_XMT_USB_API void 	XMT_COMMAND_WaveSetMultWaveStop(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char Channel_Num
					);

//辅助协助类
//设置  18 20 22 
DLL_XMT_USB_API void 	XMT_COMMAND_Assist_SetFlag(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
                    unsigned char Channel_Num,
					unsigned char SetFlag
					);

//读取flag数据 19 21 23 
DLL_XMT_USB_API unsigned char 	XMT_COMMAND_Assist_ReadFlag(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
                    unsigned char Channel_Num
					);


//标定和配置类
//设定系统类  24 26 28 
DLL_XMT_USB_API void  XMT_COMMAND_SetSystemInfo(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
                    unsigned char Channel_Num,
					double SystemInfo
					);
//读数据类 25 27  29
DLL_XMT_USB_API double XMT_COMMAND_ReadSystemInfo(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char Channel_Num
					);

//设定高低限
//30 32 34
DLL_XMT_USB_API void 	XMT_COMMAND_SetSystemHL_Limit(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					double SystemInfo
					);

//读取系统高低限 31 33 35 
double DLL_XMT_USB_API  	XMT_COMMAND_ReadSystemHL_Limit(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4					
					);



//读取系统类 48 
DLL_XMT_USB_API void 	XMT_COMMAND_ReadData_TS_UpDoPro(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char TimerSet_ms,
					unsigned char Flag_Channe_OpenOrClose
					);
//读取系统类 49
DLL_XMT_USB_API void 	XMT_COMMAND_ReadData_TS_DownDoPro(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char TimerSet_ms
					);
//51 读取多路位移或电压数据
DLL_XMT_USB_API void 	XMT_COMMAND_ReadMultChannelMoveOrVolt(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char *OpenOrCloseFlag_0,
					double *Data_0,
					unsigned char *OpenOrCloseFlag_1,
					double *Data_1,
					unsigned char *OpenOrCloseFlag_2,
					double *Data_2
					);
//测试类
DLL_XMT_USB_API double XMT_COMMAND_ClearUSBArr(unsigned char address,
					unsigned char Command_B3,
					unsigned char Command_B4,
					unsigned char Channel_Num
					);
//读取一路或二路或三路定时数据  unsigned char Channel_Num 0 表示一路 1 表示二路 2表示三路
//配合// 7 8 开环单路定时读取以及 9 10闭环定时读取使用

DLL_XMT_USB_API double XMT_COMMAND_READUSBArrOnlyOneChannl(unsigned char Channel_Num); 





