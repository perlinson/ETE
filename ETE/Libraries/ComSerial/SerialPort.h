/*
**	FILENAME			CSerialPort.h
**
**	PURPOSE				This class can read, write and watch one serial port.
**						It sends messages to its owner when something happends on the port
**						The class creates a thread for reading and writing so the main
**						program is not blocked.
**
**	CREATION DATE		15-09-1997
**	LAST MODIFICATION	12-11-1997
**
**	AUTHOR				Remon Spekreijse
**
**
************************************************************************************
**  author: mrlong date:2007-12-25
**
**  改进
**    1) 增加ClosePort
**    2) 增加 writetoProt() 两个方法
**    3) 增加 SendData 与 RecvData 方法
**************************************************************************************
***************************************************************************************
**  author：liquanhai date:2011-11-04
**  改进
**    1）增加 ClosePort中交出控制权，防止死锁问题
**
 */

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__


#include "..\..\ETESystemPublicDefine.h"

class CSerialPort
{														 
public:
	// contruction and destruction
	CSerialPort();
	virtual		~CSerialPort();

	// port initialisation											
	BOOL		InitPort(CWnd* pPortOwner, UINT portnr = 1, UINT baud = 19200, 
		char parity = 'N', UINT databits = 8, UINT stopsbits = 1, 
		DWORD dwCommEvents = EV_RXCHAR | EV_CTS, UINT nBufferSize = 512,

		DWORD ReadIntervalTimeout = 1000,
		DWORD ReadTotalTimeoutMultiplier = 1000,
		DWORD ReadTotalTimeoutConstant = 1000,
		DWORD WriteTotalTimeoutMultiplier = 1000,
		DWORD WriteTotalTimeoutConstant = 1000);

	// start/stop comm watching
	BOOL		StartMonitoring();
	BOOL		RestartMonitoring();
	BOOL		StopMonitoring();

	DWORD		GetWriteBufferSize();
	DWORD		GetCommEvents();
	DCB			GetDCB();

	void		WriteToPort(char* string);
	void        WriteToPort(char* string,int n); // add by mrlong 2007-12-25
	void		WriteToPort(unsigned char* string);     // add by mrlong 2007-12-25
	void        WriteToPort(BYTE* Buffer, int n, int &byteReads);// add by mrlong
	void        ClosePort();                     // add by mrlong 2007-12-2  

	void SendData(unsigned char* lpszData, const size_t nLength);   //串口发送函数 by mrlong 2008-2-15
	BOOL RecvData(unsigned char* lpszData, const size_t nSize);      //串口接收函数 by mrlong 2008-2-15

protected:
	// protected memberfunctions
	void		ProcessErrorMessage(char* ErrorText);
	static UINT	CommThread(LPVOID pParam);
	static void	ReceiveChar(CSerialPort* port, COMSTAT comstat);
	static void	WriteChar(CSerialPort* port);

	// thread
	CWinThread*			m_Thread;

	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync;
	BOOL				m_bThreadAlive;

	// handles
	HANDLE				m_hShutdownEvent;  //stop发生的事件
	HANDLE				m_hComm;           // read  
	HANDLE				m_hWriteEvent;     // write

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3];

	// structures
	OVERLAPPED			m_ov;
	COMMTIMEOUTS		m_CommTimeouts;
	DCB					m_dcb;

	// owner window
	CWnd*				m_pOwner;

	// misc
	UINT				m_nPortNr;        //?????
	unsigned char*		m_szWriteBuffer;
	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;

	int                 m_nWriteSize; //add by mrlong 2007-12-25
};

#endif __SERIALPORT_H__