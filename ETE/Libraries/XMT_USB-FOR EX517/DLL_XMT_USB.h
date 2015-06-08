
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DLL_XMT_USB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DLL_XMT_USB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

/********************哈尔滨芯明天科技有限公司************************************/
/********************哈尔滨市南岗区汉广街41号************************************/
/********************网址:www.xmtkj.com      ************************************/
/********************组长:chengfeng	         ************************************/
/********************作者:yuelilei           ************************************/
/********************版本号:201010           ************************************/
/********************使用说明:在文件夹有文本说明*********************************/
/********************动态库安全可以放心使用  ************************************/
/********************正确使用以及发送电压等命令**********************************/
//#include "stdafx.h"

#include "./Libraries/CyUsb/CyAPI.h"
#define DllExport   __declspec( dllexport ) 

#ifdef DLL_XMT_USB_EXPORTS
#define DLL_XMT_USB_API __declspec(dllexport)
#else
#define DLL_XMT_USB_API __declspec(dllimport)
#endif

// This class is exported from the DLL_XMT_USB.dll
//CCyUSBDevice *cyusb = new CCyUSBDevice(NULL); //生成一个USB的全局变量
class  DLL_XMT_USB_API CDLL_XMT_USB {
public:
	CDLL_XMT_USB(void);
	// TODO: add your methods here.
	
	unsigned char* DataAnla_length_command(int f,unsigned char kk[2]); //将长度转化为字符发下去
	unsigned char copy_arr_data(unsigned char target[],unsigned char source[],int num); //转移数组的内部的数据
    void copy_arr_xmt_yll(unsigned char target[],unsigned char source[]);//拷贝校验位
	void send_num_of_char(unsigned char send_arr[],int num,CCyUSBDevice *USBDevice); //发送数据
	void send_num_of_char(unsigned char send_arr[],int num); //发送数据
	void send_num_char(unsigned char arry[],int num_of_arr,CCyUSBDevice *USBDevice);
	void send_num_char(unsigned char arry[],int num_of_arr);
	unsigned char *  DataAnla(float f,unsigned char kk[4]);
	unsigned char copy_arr_data_float(unsigned char target[],unsigned char source[],int num); //将浮点数据转换过去
    int CalData_char(unsigned char kk0,unsigned char kk1);//用来转化 正负数
	float CalData(unsigned char kk0,unsigned char kk1,unsigned char kk2,unsigned char kk3);//用来转化 正负数	
};

//有关于USB设备操作
 DllExport bool New_USB_VIR_DEVICE(CCyUSBDevice *new_USB_DEVICE); //获得一个USB设备
 DllExport bool  IsOpen_Num_Of_USB(CCyUSBDevice *USBDevice,int NUM_OF_USB); //检测该标号的USB设备是否开始运转
 DllExport int  Open_Num_Of_USB(CCyUSBDevice *USBDevice,int NUM_OF_USB);//通过开启来控制多USB设备要控制
 DllExport int  NUM_OF_TOTAL_USB_DEVICE(CCyUSBDevice *USBDevice); //检测现在开启的USB设备

 DllExport int  NUM_OF_TOTAL_USB_DEVICE_LABVIEW(); //检测现在开启的USB设备


//芯明天科技有限公司 标准版 动态库
 DllExport void  SEND_V_COMMAND_SINGLE(CCyUSBDevice *USBDevice,int command_length,unsigned char a1,unsigned char b1,unsigned char c1,float data); //发送单点命令
 DllExport void  read_system_T_P_R(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_t,unsigned char char_p,unsigned char char_r); //读系统参数	
//读系统电压
 DllExport void read_System_V_S(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_t,unsigned char char_d,unsigned char char_v,unsigned char char_a); //读系统电压 位移值
 DllExport float read_System_real_V_S_and_fuzai(unsigned char a1,unsigned char b1,unsigned char c1); //RVN读到的电压的数值
//发送波形命令
 DllExport void send_T_Wave(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_T,unsigned char char_W,unsigned char char_channel,unsigned char char_type_wave,float fuzhi,float fHz,float pianyi);
 DllExport void write_System_IO(CCyUSBDevice *USBDevice,int length_command,unsigned char char_t,unsigned char char_i,unsigned char char_n,unsigned char_IO); //写IO口数据
 DllExport void read_System_real_info_IO(unsigned char receive_char_info[],unsigned char a1,unsigned char b1); //读系统的开闭环以及IO状态
//2010_09_14 增加有关于USB发送的波形启动 以及 停止的控制命令
 DllExport void send_wave_mode_CH_H_S(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_t,unsigned char char_m,unsigned char char_a,unsigned char char_H_h_s,unsigned char_L_h_s);
//将收到的USB的数据 调整好
//2010.09.14 加入
 DllExport void read_System_real_info_wave_begin_stop(unsigned char wave_begin_stop[],unsigned char a1,unsigned char b1);
 DllExport void read_System_real_info_16(float receive_info_data[],unsigned char a1,unsigned char b1);  //读系统的信息 最大位移
 DllExport void write_system_fuzai(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_t,unsigned char char_l,unsigned char char_r,unsigned char char_n,float data); //发送写负载命令
 DllExport void read_system_fuzai(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_t,unsigned char char_l,unsigned char char_r,unsigned char char_n);  //读负载
 DllExport bool BOOL_CONNECT_USB_XE(void); //USB设备是否连接正常 返回false 表示连接有误  返回true表示连接正常

 
 //快速单点辅助命令 将发送数据转化为 USB 发送的模式 返回给数组 unsigned char send_ARR[]
 DllExport void SEND_V_COMMAND_SINGLE_pro(int command_length,unsigned char a1,unsigned char b1,unsigned char c1,float data,unsigned char send_ARR[]);
 //矩形波  确定发送点数
 DllExport void send_num_char_pro(CCyUSBDevice *USBDevice,unsigned char arry[2][14],int num_of_arr,double um_sec,double SEND_ALL_POINT); //um_sec单点持续时间 SEND_ALL_POINT发送单点个数 
 //矩形波  确定发送时间
 DllExport void send_num_char_pro_SEC(CCyUSBDevice *USBDevice,unsigned char arry[2][14],int num_of_arr,double um_sec,double Using_time); //Using_time通过发送时间来定时



 //快速发送连续单点数据 
 //定时器时间通过拦截CPU时钟 使用改程序请使用高速CPU 
 //数组的间隔时间最小为500微秒 也就是数组send_time中的元素最小为0.5
 //send_data[]发送的数据元素
 //send_time[]发送数据一一对应的占用时间
 //发送数据的个数
 //CHANNEL选择的数据通道 'A'一路 'B'二路 'C'三路 
 //V_OR_S 开闭环选择 'V'开环 'S'闭环
 //USing_time 改函数运行时间 1对应1秒(大概时间)-拦截CPU时间获得
 DllExport void SEND_ARR_DATA_TIME_NUM(CCyUSBDevice *USBDevice,float send_data[],float send_time[],int SEND_NUM,char CHANNEL,char V_OR_S,float USing_time);
 

 //快速发送连续单点数据 
 //定时器时间通过拦截CPU时钟 使用改程序请使用高速CPU 
 //数组的间隔时间最小为500微秒 也就是数组send_time中的元素最小为0.5
 //send_data[]发送的数据元素
 //send_time[]发送数据一一对应的占用时间
 //发送数据的个数
 //CHANNEL选择的数据通道 'A'一路 'B'二路 'C'三路 
 //V_OR_S 开闭环选择 'V'开环 'S'闭环
 //double SEND_POINT 发送的总点数
 DllExport void SEND_ARR_DATA_TIME_NUM_NUMPOINT(CCyUSBDevice *USBDevice,float send_data[],float send_time[],int SEND_NUM,char CHANNEL,char V_OR_S,double SEND_POINT);


 DllExport void  read_system_T_P_R(int lenght_command,unsigned char char_t,unsigned char char_p,unsigned char char_r); //读系统参数	
 DllExport char send_abc();
 DllExport char send_DATA();

 DLL_XMT_USB_API void  receive_usb_info(CCyUSBDevice  *USBDevice,unsigned char receive_arr[]);//读取的USB的数据值
 DLL_XMT_USB_API void  receive_usb_info_16(CCyUSBDevice  *USBDevice,unsigned char receive_arr[]); //读取的USB的数据值  20120218加入
 void DLL_XMT_USB_API receive_pro_16(CCyUSBDevice *USBDevice,unsigned char reciv[]); //z读取电压 或 是位移的数据 
 
 //DLL FOR LABVIEW or simple command for XE_XMT_USB_PRO
int DLL_XMT_USB_API NUM_OF_TOTAL_USB_DEVICE_labview(void); //检测现在开启的USB设备
int DLL_XMT_USB_API Open_Num_Of_USB_labview(int NUM_OF_USB);//连接编号为NUM的USB高速设备
int DLL_XMT_USB_API send_UPMUC_CONTROL_DATA(int NUM_OF_USB);


//////////////////////////////////////////////////////////////////////////
//单点电压控制命令
//发送系统电压
DllExport int send_volt_labview(float send_data,char channel_type); //发送单点数据 channel_type->>>A B C
//读取系统电压命令
DLL_XMT_USB_API void read_System_V_labview(unsigned char CHANNEL_TYPE); //发送读取 读系统电压命令 channel_type->>>A B C
//读取实际系统电压数据
DLL_XMT_USB_API float read_System_real_V_labview(unsigned char CHANNEL_TYPE); //读系统电压 channel_type->>>A B C

DLL_XMT_USB_API void  receive_usb_V_AND_MOVE(CCyUSBDevice  *USBDevice,unsigned char receive_arr[]); //读取的USB的数据值
void DLL_XMT_USB_API receive_pro_v_move_only(CCyUSBDevice *USBDevice,unsigned char reciv[]); //z读取电压 或 是位移的数据



//选择 高级发送数据   三路同时发送 数据位 
DLL_XMT_USB_API void  SEND_VVV_COMMAND_SINGLE(CCyUSBDevice *USBDevice,int command_length,unsigned char a1,unsigned char b1,unsigned char flag_A,float data1,unsigned char flag_B,float data2,unsigned char flag_C,float data3);
 DLL_XMT_USB_API void  send_VVV_labview(unsigned char flag_A,float data1,unsigned char flag_B,float data2,unsigned char flag_C,float data3);
////////////////////////////////////////////////// ////////////////////////


//////////////////////////////////////////////////////////////////////////
//单点位移控制命令
DllExport int send_Move_labview(float send_data,char channel_type); //发送单点数据 channel_type->>>A B C
//读取系统电压命令
DLL_XMT_USB_API void read_System_Move_labview(unsigned char CHANNEL_TYPE); //发送读取 读系统电压命令
//读取实际系统电压数据
DLL_XMT_USB_API float read_System_real_Move_labview(unsigned char CHANNEL_TYPE); //读系统电压
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////// 
//新加入读取系统三路一同的数据的情况
DLL_XMT_USB_API void read_System_VVV_labview(unsigned char CHANNEL_TYPE); //发送读取 系统三路位移或是电压命令 'C'
DLL_XMT_USB_API void read_System_VVV_SSS(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_t,unsigned char char_d,unsigned char char_v); //读系统电压 位移值

DLL_XMT_USB_API void read_System_real_VVV_labview(unsigned char flag_ABC[3],float data_road_ABC[3]); //读系统电压
DLL_XMT_USB_API void read_System_real_VVV_labview_DLL(unsigned char flag_ABC[3],float data_road_ABC[3]); //RVN读到的电压的数值
DLL_XMT_USB_API void read_System_real_VVV_flag_ABC_flag_arr_labview(unsigned char &flag_ABC_1,unsigned char &flag_ABC_2,unsigned char &flag_ABC_3,float &data_road_ABC_1,float &data_road_ABC_2,float &data_road_ABC_3); //读系统电压
//////////////////////////////////////////////////////////////////////////
//波形控制命令
//波形数据发送命令
DLL_XMT_USB_API void send_T_Wave_labview(unsigned char CHANNEL_TYPE,unsigned char WAVE_TYPE,float FUZHI,float P_Hz,float P_Zhi);
//波形开始,停止命令
DLL_XMT_USB_API void send_T_Wave_Star_or_Fianl_labview(unsigned char Flag_wave_begin_or_Fianl); //Flag_wave_begin_or_Fianl 'S'<==>START,'F'<==>FINAL
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//IO口功能

//IO口设定功能
DLL_XMT_USB_API void write_System_IO_labview(unsigned char CHANNEL_TYPE,unsigned char IO_Flag); //一路输出 'I'为输入模式 'O' 为输出模式

//读取IO口状态命令
DLL_XMT_USB_API void read_System_IO_labview(unsigned char READ_FLAG); //READ_FLAG = 'R'

//读取IO口状态
DLL_XMT_USB_API void read_System_real_info_IO_labview(unsigned char receive_char_info[6]);
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
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/* IO空控制数据 来触发外部设配与XE系列电源做同步配合 20120607*/
DLL_XMT_USB_API void control_XE_channel_IO(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_t,unsigned char char_l,unsigned char char_n,unsigned char tmp_flag_begin,unsigned char tmp_flag_end); //发送写负载命令
DLL_XMT_USB_API void control_XE_channel_IO_labview(unsigned char CHANNEL_TYPE,unsigned char flag_begin,unsigned char flag_end);

/*  例子 控制 IO口输出 发出触发信号给外部设备使之能与XE系列电源做配合工作。
control_XE_channel_IO_labview('A','1','0'); //通道一路 IO口 先置高 延迟约150微秒 然后置低
control_XE_channel_IO_labview('A','0','1'); //通道一路 IO口 先置低 延迟约150微秒 然后置高

control_XE_channel_IO_labview('B','1','0'); //通道二路 IO口 先置高 延迟约150微秒 然后置低
control_XE_channel_IO_labview('B','0','1'); //通道二路 IO口 先置低 延迟约150微秒 然后置高

control_XE_channel_IO_labview('C','1','0'); //通道三路 IO口 先置高 延迟约150微秒 然后置低
control_XE_channel_IO_labview('C','0','1'); //通道三路 IO口 先置低 延迟约150微秒 然后置高
*/
////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//设置波形启停速度

void DLL_XMT_USB_API send_wave_mode_CH_H_S_labview(unsigned char CHANNEL_TYPE,unsigned char WAVE_BEGIN_SPEED,unsigned char WAVE_END_SPEED);
//设置波形启停速度
/*
命令格式:  send_wave_mode_CH_H_S_labview('A','H','H');

  通  道: 
  'A'---一路通道
  'B'---二路通道
  'C'---三路通道
  WAVE_BEGIN_SPEED:  波形起始速度 'H'表示高速 'S'表示低速
  WAVE_END_SPEED  :  波形停止速度 'H'表示高速 'S'表示低速			  
  /*  
  例如:
  send_wave_mode_CH_H_S_labview(14,'T','M','A','H','S'); //一路启动为低
  send_wave_mode_CH_H_S_labview(14,'T','M','B','H','H');
  send_wave_mode_CH_H_S_labview(14,'T','M','C','H','H');	
*/

//读波形启停速度命令
void DLL_XMT_USB_API read_Systeml_info_wave_begin_stop_labview(unsigned char READ_FLAG);  //READ_FLAG = 'R'
//读波形启停速度状态
void DLL_XMT_USB_API read_System_real_info_wave_begin_stop_labview(unsigned char recive_data_usb_wave_send_mode[6]);
/*
命令格式:  read_System_real_info_wave_begin_stop_labview(recive_data_usb_wave_send_mode,'R','M'); 该命令配合上面的
命令长度: **
命令说明: 上位机从USB端口读取1,2,3路波形启停速度 , read_System_real_info_wave_begin_stop_labview(recive_data_usb_wave_send_mode,'R','M');
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



//////////////////////////////////////////////////////////////////////////
//读取系统参数信息命令
DLL_XMT_USB_API void read_system_states_labview(unsigned char READ_FLAG); //READ_FLAG = 'R'

//读取系统参数信息
DLL_XMT_USB_API void read_system_states_real_labview(float recive_data_usb[9]);
/*
命令格式: 该命令配合上面的
命令长度: **
命令说明: 上位机从USB端口读取1,2,3路系统信息
通    道: 读取1，2，3路,波形启停速度
返回数据: 使用recive_data_usb[9]数组来接收1,2,3路信息
其中状态信息包括1，2，3路通道的路系统信息
recive_data_usb[0] ---  系统最小电压
recive_data_usb[1] ---  系统最大电压
recive_data_usb[2] ---  系统最小位移
recive_data_usb[3] ---  一路闭环陶瓷最大位移
recive_data_usb[4] ---  二路闭环陶瓷最大位移
recive_data_usb[5] ---  三路闭环陶瓷最大位移
recive_data_usb[6] ---  一路功率
recive_data_usb[7] ---  二路功率
recive_data_usb[8] ---  三路功率
*/
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//系统负载设置命令
//写负载命令
DLL_XMT_USB_API void write_system_fuzai_labview(unsigned char CHANNEL_TYPE,float fuzai_data);
/*
命令格式: write_system_fuzai_labview('A',1.9);

通    道: 
'A'---一路通道
'B'---二路通道
'C'---三路通道
变    量: 1.9 为标定的一路通道的负载数值 该数值根据实际情况填写

返回数据: *****/


//读取系统负载命令
DLL_XMT_USB_API void read_system_fuzai_labview(unsigned char CHANNEL_TYPE);
//读取系统负载状态
DLL_XMT_USB_API float read_system_fuzai_real_labview(unsigned char CHANNEL_TYPE);
/*
命令格式:  read_system_fuzai_real_labview(unsigned char CHANNEL_TYPE) 该命令配合上面的
命令说明: 上位机从USB端口读取一路电压命令(接收命令)  read_system_fuzai_real_labview('R','L','A');
通    道: 
读取1路的负载数值 
通  道
'A'---一路通道
'B'---二路通道			    
'C'---三路通道
返回数据: 使用float_fzai来接收数据信息  
*/


//20120213 加入
/*
   //成批量的读取系统的长度 20120213 加入
*/
///////////////////////////////////////
//成批量的读取系统的长度
DLL_XMT_USB_API int read_System_Bulk_from_USB(CCyUSBDevice *USBDevice,int lenght_command,int length_read,unsigned char char_t,unsigned char char_d,unsigned char char_v,unsigned char char_a);

DLL_XMT_USB_API void read_System_Bulk_labview(int length_of_read,unsigned char CHANNEL_TYPE); //批量读取相关的电压 以及 位移数据

//读取实际系统电压数据 批量
DLL_XMT_USB_API int read_System_real_Bulk_labview(unsigned char receive[],unsigned char CHANNEL_TYPE);  //外部调用 例如vc++或是labview等编程语言
DLL_XMT_USB_API int read_System_real_Bulk(unsigned char receive[],unsigned char CHANNEL_TYPE);   //动态库内部封装使用的函数

DLL_XMT_USB_API int receive_usb_Bulk_V_AND_MOVE(CCyUSBDevice  *USBDevice,unsigned char receive_arr[]); //读取系统的数据到数组中

DLL_XMT_USB_API void receive_pro_bulk_v_move_only(CCyUSBDevice *USBDevice,unsigned char reciv[]); //z读取电压 或 是位移的数据

DLL_XMT_USB_API void begin_Read_data_for_bulk_labview(unsigned char channel_type);
////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
//同时控制1，2，3路数据相应的电压以及位移数据 通过下位机自动判断开闭环 上位机只要填写一二三路的控制数据。
DLL_XMT_USB_API void xmt_usb_send_all_data_labview(float A_CHANNEL_data,float B_CHANNEL_data,float C_CHANNEL_data);  //发送一路 二路 三路 数据 发送USB形式的数据 
/*
命令格式:  xmt_usb_send_all_data(float data_01,float data_02,float data_03);
命令说明:  //同时控制1，2，3路数据相应的电压以及位移数据 通过下位机自动判断开闭环 上位机只要填写一二三路的控制数据。
通    道:  A_CHANNEL_data,B_CHANNEL_data,C_CHANNEL_data 分别为一路 二路 三路数据
向下位机发送一路 二路 三路数据 准备同时输出
*/
//下面为芯明天科技有限公司 调试使用的部分动态库仅供芯明天公司调试使用 用户使用可能会导致系统崩溃或是机器损坏或是硬件损坏或是人身伤害！！！
DllExport void write_system_canshu(CCyUSBDevice *USBDevice,int lenght_command,unsigned char char_t,unsigned char char_p,unsigned char char_w,float min_v,float max_v,float min_move,float move_01,float move_02,float move_03,float gl_01,float gl_02,float gl_03); //写系统数据

DllExport void write_system_canshu_labview(int lenght_command,unsigned char char_t,unsigned char char_p,unsigned char char_w,float min_v,float max_v,float min_move,float move_01,float move_02,float move_03,float gl_01,float gl_02,float gl_03); //写系统数据


DLL_XMT_USB_API int fnDLL_XMT_USB(void);
