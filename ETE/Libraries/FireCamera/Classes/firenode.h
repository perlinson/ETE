////////////////////////////////////////////////////////////////////////////////
// Headerfile for FireNode object.
// intek, C. Kuehnel, 5.12.2002
////////////////////////////////////////////////////////////////////////////////

#ifndef FIRENODE_H
#define FIRENODE_H

#include <osutil.h>
#include <clink.h>
#include <fctlmain.h>

#define MAXPORTS                24              // Maximum ports a node may have
#define ROOTOFFSET              0x414

enum
{
  FNT_UNKNOWN=0,
  FNT_DCAM,
  FNT_LAST
};

typedef struct
{
  UINT32        ChipLow;
  UINT32        ChipHigh : 8;
  UINT32        Vendor   : 24;
}FNGUID;

class CFireNode;
class CFireBus;

////////////////////////////////////////////////////////////////////////////////
// Type for config rom header.
////////////////////////////////////////////////////////////////////////////////

typedef union
{
  struct
  {
    UINT16    Crc;
    UINT8     CrcLength;
    UINT8     Tag;
  }AsBits;

  UINT32 AsUINT32;
}FNCFGROMHDR;

////////////////////////////////////////////////////////////////////////////////
// Type for different node flags.
////////////////////////////////////////////////////////////////////////////////

typedef union fnflags
{
  struct
  {
    UINT32      LinkActive : 1;         // Linklayer is active
    UINT32      BusMgr : 1;             // This node is bus manager
    UINT32      IsoMgr : 1;             // This node is ISO manager
    UINT32      CycleMaster : 1;        // This node is cycle master
    UINT32      Contender : 1;          // Contender for Iso-/Bus-Manager
    UINT32      Speed : 2;              // Speed
    UINT32      PowerClass : 3;         // Powerclass
    UINT32      InitReset : 1;          // Initiated reset
    UINT32      MaxSpeed : 2;           // Max speed due to net layout
  }AsBits;

  UINT32        AsUINT32;

}FNFLAGS;

////////////////////////////////////////////////////////////////////////////////
// Structure for a fire port.
////////////////////////////////////////////////////////////////////////////////

class CFirePort
{
protected:
  UINT8         m_State;                // Portstate (=Connection type)
  CFireNode    *m_pFireNode;            // Pointer to connected node

public:
  UINT8&        State()         { return m_State; }
  CFireNode*&   FireNode()      { return m_pFireNode; }
};

////////////////////////////////////////////////////////////////////////////////
// Class for a node.
////////////////////////////////////////////////////////////////////////////////

class CFireNode : public CLink
{
protected:
  UINT16        m_BusAddress;           // Bus address
  UINT8         m_NodeAddress;          // Node address
  UINT8         m_SelfIdCnt;            // Number of SelfIds
  UINT32        m_SelfId[4];            // SelfIds
  FNFLAGS       m_Flags;                // Flags for this node
  UINT8         m_PortCnt;              // Number of ports
  CFirePort     m_FirePort[MAXPORTS];   // Our Fireports
  UINT8         m_Type;                 // Type of this node
  UINT32        m_AccessLatency;        // Latency between accesses
  UINT32        m_AccessRepeat;         // Repeat on no success
  CFireBus     *m_pFireBus;             // The bus we live on
  FNGUID        m_Guid;                 // Vendor Device Id (GUID)
  UINT8         m_GuidValid;            // Valid flag for ID
  UINT32        m_LastError;            // Stores last error

  POSSEMAPHORE  m_pLockSemaphore;       // Semaphore for user lock

  virtual UINT16 CrcQuadlet(UINT32 Data,UINT16 Check); // CRC helper routine

public:
                CFireNode();
  virtual      ~CFireNode();

  virtual void   CopyFrom(CFireNode *pSrcNode);

  // More or less internal functions
  virtual void   SetNodeAddress(UINT8 Val) { m_NodeAddress=Val; }
  virtual UINT16 BuildCrc16(UINT32 *pQuad,UINT32 Cnt,UINT8 IsBigEndian=TRUE);
  virtual UINT8  AssignProperties(CFireBus *pFireBus,UINT32 *pSelfId,UINT8 MaxSpeed=SPEED800);
  virtual void   CopyProperties(CFireNode *pFireNode);
  virtual UINT8  InquireType();
  virtual UINT8  ReadGuid();
  virtual void   InvalidateGuid()       { m_GuidValid=FALSE; }
  virtual void   InvalidateAddress()    { m_NodeAddress=NODEBROADCAST; }
  virtual UINT32 VendorId()             { return m_Guid.Vendor; }
  virtual UINT32 SetAccessLatency(UINT32 Val);	// Set delay between two accesses
  virtual UINT8  CheckGuid(FNGUID *pGuid); // Return TRUE if same

  // User functions
  virtual void   Lock();                         // User lock function
  virtual void   Unlock();                       // User unlock function

  virtual UINT8  Type()                 { return m_Type; }
  virtual UINT8  NodeAddress()          { return m_NodeAddress; }
  virtual UINT16 BusAddress()           { return m_BusAddress; }
  virtual UINT8  PortCnt()              { return m_PortCnt; }
  virtual UINT8  CardNumber();
  virtual CARDHANDLE CardHandle();
  virtual CFirePort* pFirePort(UINT8 Ix){ return &m_FirePort[Ix]; }
  virtual UINT8  SelfIdCnt()            { return m_SelfIdCnt; }
  virtual UINT32 SelfId(UINT8 Ix)       { return m_SelfId[Ix]; }
  virtual UINT32 Flags()                { return m_Flags.AsUINT32; }
  virtual UINT8  MaxSpeed()             { return m_Flags.AsBits.MaxSpeed; }
  virtual UINT32 LastError()            { return m_LastError; }

  virtual FNGUID* pGuid();              // Return pointer to GUID

  virtual UINT8 OnStartup();            // Called on first detection
  virtual void  OnShutdown();           // Called when object shuts down
  virtual void  OnBusResetEnd();        // Called when a bus reset ends

  virtual UINT8 ReadQuad(UINT16 OffsetHigh,UINT32 OffsetLow,UINT32 *pQuad);
  virtual UINT8 WriteQuad(UINT16 OffsetHigh,UINT32 OffsetLow,UINT32 Quad,UINT8 BroadCast=FALSE);
  virtual UINT8 ReadBlock(UINT16 OffsetHigh,UINT32 OffsetLow,UINT8 *pMem,
                          UINT32 Len);
  virtual UINT8 WriteBlock(UINT16 OffsetHigh,UINT32 OffsetLow,UINT8 *pMem,
                           UINT32 Len);
};

#endif
