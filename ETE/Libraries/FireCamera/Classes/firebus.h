////////////////////////////////////////////////////////////////////////////////
// Headerfile for FireBus object.
// intek, C. Kuehnel, 5.12.2002
////////////////////////////////////////////////////////////////////////////////

#ifndef FIREBUS_H
#define FIREBUS_H

#include <osutil.h>
#include <firenode.h>
#include <fctlmain.h>

//#include "../../../ETESystemPublicDefine.h"

#define MAXSELFIDS              (64*4)          // Memorysize for SelfIds
#define MAXISORXCHN             8               // Max. supported ISO Rx Dmas

enum
{
  FBN_BUSRESETBEGIN=0,                  // pData=NULL
  FBN_BUSRESETEND,                      // pData=NULL
  FBN_CREATEDEVICE,                     // pData=&FBNCREATE
  FBN_NEWDEVICE,                        // pData=&FireNode
  FBN_KILLDEVICE,                       // pData=&FireNode
  FBN_ERROR,                            // pData=&Error
  FBN_FLAGGEDERROR,                     // pData=&ErrorFlags
  FBN_FRAME,                            // pData=&CameraObject
  FBN_DEBUG,                            // Debug message
  FBN_LAST
};

class CFireNode;
class CCamera;

////////////////////////////////////////////////////////////////////////////////
// Type for callback information.
////////////////////////////////////////////////////////////////////////////////

typedef struct fbcbinfo
{
  UINT8         CardNumber;
  UINT32        Reason;
  void*         pData;
  void*         pContext;
}FBCBINFO,*PFBCBINFO;

////////////////////////////////////////////////////////////////////////////////
// Type for callback function.
////////////////////////////////////////////////////////////////////////////////

typedef void FBCALLBACK(PFBCBINFO);

////////////////////////////////////////////////////////////////////////////////
// Type for device creation notification.
////////////////////////////////////////////////////////////////////////////////

typedef struct fbcreate
{
  CFireNode    *pTmpNode;               // Pointer to basic node
  CFireNode    *pNewNode;               // User created node
}FBNCREATE;

////////////////////////////////////////////////////////////////////////////////
// Type for a card ID.
////////////////////////////////////////////////////////////////////////////////

typedef struct fbcardid
{
  union
  {
    struct
    {
      UINT8     HardwareType;
      UINT8     FunctionNumber;
      UINT8     DeviceNumber;
      UINT8     BusNumber;
    }AsBytes;

    UINT32      AsUINT32;
  };

}FBCARDID;

////////////////////////////////////////////////////////////////////////////////
// Type for system information.
////////////////////////////////////////////////////////////////////////////////

typedef struct fbsysinfo
{
  UINT32        CardCount;              // Number of valid cards
  FBCARDID      CardId[MAXCARDS];       // IDs for our cards
}FBSYSINFO;

////////////////////////////////////////////////////////////////////////////////
// Type for initialize parameters.
////////////////////////////////////////////////////////////////////////////////

typedef struct fbparms
{
  UINT16        BusAddress;             // Busaddress to use
  UINT32        AsyRxPktCnt;            // Max. asynch. receive buffers
  UINT32        AsyRxPktPayload;        // Size of asynch. receive buffers
  UINT32        AsyTxPktCnt;            // Max. asynch. transmit buffers
  UINT32        AsyTxPktPayload;        // Size of asynch. transmit buffers
  UINT16        StoredAccessCnt;        // Number of external stored accesses
  UINT8         PowerClass;             // Powerclass (in Phys SelfId)
  UINT32       *pCfgRom;                // Log. pointer to ROM
  UINT16        CfgRomLength;           // Length of CFGROM in quadlets
                                        // Controller only following
  UINT16        IrmBmTaskPrio;          // Priority of Iso/B-Manager
  UINT16        IrqTaskPrio;            // Deferred task priority 
  UINT8         IrqPrio;                // Irq prirority
  UINT32        SpecialFlags;           // Special flags
}FBPARMS;

////////////////////////////////////////////////////////////////////////////////
// Class for a Firewire bus.
////////////////////////////////////////////////////////////////////////////////

class CFireBus
{
protected:
  CFireNode    *m_pRoot;                // Pointer to our root
  UINT16        m_BusAddress;           // Our bus address
  UINT8         m_State;                // State we are in
  FBCALLBACK   *m_pCallback;            // Pointer to callback fucntion
  void         *m_pContext;             // Context pointer for callback function
  CARDHANDLE    m_hCard;                // Handle of card we use
  UINT8         m_CardNumber;           // Actual card number
  UINT32        m_CardId;               // System ID of our card
  CLink         m_NodeBase;             // Base for our nodes
  UINT8         m_BusMgr;               // Address of our bus manager
  UINT8         m_IsoMgr;               // Address of our Iso manager
  UINT8         m_NodeCnt;              // Number of nodes
  UINT8         m_MyNodeAddress;        // My node address
  UINT32        m_SelfIdCnt;            // Number of used SelfIds
  UINT32        m_SelfId[MAXSELFIDS];   // Space for all SelfIds
  UINT32        m_GenerationCount;      // Actual generation count
  CCamera      *m_pIsoDevice[MAXISORXCHN]; // Pointer to iso active cameras
  POSSEMAPHORE  m_pAccessSemaphore;     // Semaphore for some accesses
  void*         m_User;                 // User value
  UINT8         m_MaxSpeed;             // Max async. speed

  virtual void  OnError();              // Reaction on error
  virtual void  OnBusResetBegin();      // Reaction on busreset begin
  virtual void  OnBusResetEnd();        // Reaction on busreset end
  virtual void  OnIsoEvent(UINT32 EventFlags); // Reaction on ISO events
  virtual void  OnIsoRx(UINT8 IsoChn);  // Reaction on Iso receive

  virtual void  RebuildNodelist();      // Rebuild our nodelist
  virtual void  FreeNodeList();         // Free actual nodelist
  virtual void  LinkNodeList();         // Link actual nodelist

  virtual CFireNode*    FindNode(CLink *pBase,CFireNode *pTmpNode);
  virtual void  ProcessTmpNode(CLink *pBase,CFireNode *pTmpNode);

  virtual void  CallUser(UINT32 Reason,void* pData);

  virtual void  LockAccess();           // Synchronize access
  virtual void  UnlockAccess();

  virtual UINT8 GetIsoChannel(CCamera *pCamera);  // Get an iso channel
  virtual void  FreeIsoChannel(CCamera *pCamera); // Free iso channel

  virtual void  EventLoop();            // Our event loop
  virtual void  Shutdown();             // Shutdown

public:
                CFireBus();
  virtual      ~CFireBus();

  virtual UINT8 OpenIsoDma(CCamera *pCamera,FCTLLOCKMEMORY *pUserFrames=NULL);
  virtual UINT8 CloseIsoDma(CCamera *pCamera);
  virtual void  ReturnFrame(CCamera *pCamera,FCTLFRMINFO *pInfo);

// User functions....
public:
  virtual UINT16        BusAddress()            { return m_BusAddress; }
  virtual UINT8         BusMgr()                { return m_BusMgr;  }
  virtual UINT8         IsoMgr()                { return m_IsoMgr;  }
  virtual UINT8         NodeCnt()               { return m_NodeCnt; }
  virtual UINT8         CardNumber()            { return m_CardNumber; }
  virtual CARDHANDLE    CardHandle()            { return m_hCard; }

  virtual void          SetUser(void*  User)    { m_User=User; }
  virtual void*         GetUser()               { return m_User; }

  virtual UINT8         IsRunning();

  virtual UINT8         MaxSpeed(UINT8 NodeAddress);    // Get max. Speed
  virtual void          LimitSpeed(UINT8 MaxSpeed);     // Limit max. speed

  virtual UINT32        ReadQuad(FCTLREADQUAD *pArg);   // Read quadlet
  virtual UINT32        WriteQuad(FCTLWRITEQUAD *pArg); // Write quadlet

  virtual UINT32        ReadBlock(FCTLREADBLK *pArg);   // Read block
  virtual UINT32        WriteBlock(FCTLWRITEBLK *pArg); // Write block

  virtual UINT32        Run(UINT8 CardNumber,FBCALLBACK *pCallback=(FBCALLBACK*)NULL,
                            FBPARMS *pParms=(FBPARMS*)NULL,void* pContext=NULL);
  virtual UINT32        Stop();                         // Stop running

  virtual UINT32        BusReset();                     // Do bus reset

  virtual char*         GetReasonString(UINT32 Reason); // String for notification
};

////////////////////////////////////////////////////////////////////////////////
// Exported global functions (independant from FireBus object).
////////////////////////////////////////////////////////////////////////////////

UINT8 GetSysInfo(FBSYSINFO *pInfo);   // Get system info

#endif
