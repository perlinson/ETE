////////////////////////////////////////////////////////////////////////////////
// Module for FireNode object.
// intek, C. Kuehnel, 5.12.2002
////////////////////////////////////////////////////////////////////////////////

#include <firenode.h>
#include <firebus.h>

#define VENDORID_SONY 0x080046
#define VENDORID_AVT  0x000A47

////////////////////////////////////////////////////////////////////////////////
// Routines for CFireNode - Constructor.
////////////////////////////////////////////////////////////////////////////////

CFireNode::CFireNode()
{
  m_BusAddress=BUSBROADCAST;
  m_NodeAddress=NODEBROADCAST;
  m_SelfIdCnt=0;
  m_Flags.AsUINT32=0;
  m_PortCnt=0;
  m_Type=FNT_UNKNOWN;
  m_AccessLatency=0;
  m_AccessRepeat=10;
  m_GuidValid=FALSE;
  m_LastError=FCE_NOERROR;

  m_pLockSemaphore=OSSemaphoreCreate(1);
}

////////////////////////////////////////////////////////////////////////////////
// Destructor.

CFireNode::~CFireNode()
{
  OSSemaphoreDelete(m_pLockSemaphore);
}

////////////////////////////////////////////////////////////////////////////////
// Lock access (user).

void CFireNode::Lock()
{
  OSSemaphorePend(m_pLockSemaphore,OSINFTIME);
}

////////////////////////////////////////////////////////////////////////////////
// Unlock access (user).

void CFireNode::Unlock()
{
  OSSemaphorePost(m_pLockSemaphore);
}

////////////////////////////////////////////////////////////////////////////////
// Build CRC16 for a quadlet starting from existing sum.
// The quadlet must be in big endian!

UINT16 CFireNode::CrcQuadlet(UINT32 data,UINT16 check)
{
  UINT32 shift,sum,next;

  next=(UINT32)check;
  for(shift=28;shift<=28;shift-=4)
  {
    sum=((next>>12)^(data>>shift))&0XF;
    next=(next<<4)^(sum<<12)^(sum<<5)^(sum);
  }

  return (UINT16)(next&0XFFFF);
}

////////////////////////////////////////////////////////////////////////////////
// Build 16 bit crc checksum over specified area.

UINT16 CFireNode::BuildCrc16(UINT32 *pQuad,UINT32 Cnt,UINT8 IsBigEndian)
{
  UINT16 Crc=0;

  // If it's big endian we must swap to get appropriate value
  if(IsBigEndian)
  {
    while(Cnt--)
    {
      Crc=CrcQuadlet(SWAP(*pQuad),Crc);
      pQuad++;
    }
  }
  else
  {
    while(Cnt--)
    {
      Crc=CrcQuadlet(*pQuad,Crc);
      pQuad++;
    }
  }

  return Crc;
}

////////////////////////////////////////////////////////////////////////////////
// Assign SelfIds.
// Returns the number of SelfIds assigned (0=Error).

UINT8 CFireNode::AssignProperties(CFireBus *pFireBus,UINT32 *pSelfId,UINT8 MaxSpeed)
{
  PKTSELFID SelfId;
  PKTSELFIDMORE SelfIdMore;
  int i;

  m_pFireBus=pFireBus;

  m_SelfIdCnt=0;
  m_PortCnt=0;
  m_NodeAddress=NODEBROADCAST;
  m_Type=FNT_UNKNOWN;
  m_GuidValid=FALSE;

  // Copy first SelfId
  SelfId.AsUINT32=SelfIdMore.AsUINT32=pSelfId[m_SelfIdCnt];
  m_SelfId[m_SelfIdCnt++]=SelfId.AsUINT32;

  // Check for valid first SelfId
  if(SelfId.AsBits.SelfIdMore)
   return 0;

  m_Flags.AsUINT32=0;

  m_BusAddress=pFireBus->BusAddress();
  m_NodeAddress=SelfId.AsBits.PhyId;

  m_Flags.AsBits.LinkActive=SelfId.AsBits.LinkActive;
  m_Flags.AsBits.BusMgr=(m_NodeAddress==pFireBus->BusMgr())?1:0;
  m_Flags.AsBits.IsoMgr=(m_NodeAddress==pFireBus->IsoMgr())?1:0;

  m_Flags.AsBits.Contender=SelfId.AsBits.Contender;
  m_Flags.AsBits.Speed=min(MaxSpeed,SelfId.AsBits.Speed);

  m_Flags.AsBits.PowerClass=SelfId.AsBits.PowerClass;
  m_Flags.AsBits.InitReset=SelfId.AsBits.InitiatedReset;
  m_Flags.AsBits.MaxSpeed=pFireBus->MaxSpeed(m_NodeAddress);

  // Check first three ports
  for(i=0;i<3;i++)
  {
    m_FirePort[m_PortCnt].State()=(UINT8)(SelfId.AsUINT32>>((3-i)*2))&3;
    if(m_FirePort[m_PortCnt].State()!=SELF_ID_NOT_PRESENT)
    {
      m_FirePort[m_PortCnt].FireNode()=(CFireNode*)NULL;
      m_PortCnt++;
    }
  }

  // Go through all additional SelfIds
  while(SelfIdMore.AsBits.More)
  {
    SelfIdMore.AsUINT32=pSelfId[m_SelfIdCnt];
    m_SelfId[m_SelfIdCnt++]=SelfIdMore.AsUINT32;

    for(i=0;i<8;i++)
    {
      if(m_PortCnt<MAXPORTS)
      {
        m_FirePort[m_PortCnt].State()=(UINT8)(SelfId.AsUINT32>>((8-i)*2))&3;
        if(m_FirePort[m_PortCnt].State()!=SELF_ID_NOT_PRESENT)
        {
          m_FirePort[m_PortCnt].FireNode()=(CFireNode*)NULL;
          m_PortCnt++;
        }
      }
    }
  }

  return m_SelfIdCnt;
}

////////////////////////////////////////////////////////////////////////////////
// Read quad from this node.

UINT8 CFireNode::ReadQuad(UINT16 OffsetHigh,UINT32 OffsetLow,UINT32 *pQuad)
{
  UINT32 Result;
  FCTLREADQUAD Arg;
  UINT32       i;

  // Check for non address assignment
  if(m_NodeAddress==NODEBROADCAST)
   return FALSE;

  // Defaulting to no error
  Result=HALER_NOERROR;

  for(i=0;i<=m_AccessRepeat;i++)
  {
    Arg.Speed=(UINT8)min(m_Flags.AsBits.Speed,m_Flags.AsBits.MaxSpeed);
    Arg.BusAddress=m_BusAddress;
    Arg.NodeAddress=m_NodeAddress;
    Arg.OffsetHigh=OffsetHigh;
    Arg.OffsetLow=OffsetLow;
    Arg.AckCode=ACK_MISSING;

    Result=m_pFireBus->ReadQuad(&Arg);

    // Check for continuous access restrictions
    if(m_AccessLatency)
     OSDelay(m_AccessLatency);

    if(Result==FCE_NOERROR)
    {
      *pQuad=SWAP(Arg.Quadlet);
      return TRUE;
    }

    // See if AckCode came from a target (>=0x10 means hardware error)
    if(Arg.AckCode!=ACK_MISSING && Arg.AckCode<0x10)
    {
      switch(Arg.AckCode)
      {
        case ACK_BUSY_X:
        case ACK_BUSY_A:
        case ACK_BUSY_B:
         Sleep(10);
         break;
        
        case ACK_TYPE_ERROR:
        case ACK_ADDR_ERROR :
         m_LastError=Result;
         return FALSE;
        
        case ACK_COMPLETE:
        case ACK_PENDING:
         if(Arg.ResCode==RCODE_TYPE_ERROR || Arg.ResCode==RCODE_ADDRESS_ERROR)
         {
           m_LastError=Result;
           return FALSE;
         }
         break; 
      }
    }
    
    // Default sleep and repeat
    OSDelay(3); 
  }

  m_LastError=Result;

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Write quad to this node.

UINT8 CFireNode::WriteQuad(UINT16 OffsetHigh,UINT32 OffsetLow,UINT32 Quad,
                           UINT8 Broadcast)
{
  UINT32 Result;
  FCTLWRITEQUAD Arg;
  UINT32       i;

  // Check for non address assignment
  if(m_NodeAddress==NODEBROADCAST)
   return FALSE;

  // Defaulting to no error
  Result=HALER_NOERROR;

  for(i=0;i<=m_AccessRepeat;i++)
  {
    Arg.Speed=(UINT8)min(m_Flags.AsBits.Speed,m_Flags.AsBits.MaxSpeed);
    Arg.BusAddress=m_BusAddress;
    Arg.NodeAddress=Broadcast?NODEBROADCAST:m_NodeAddress;
    Arg.OffsetHigh=OffsetHigh;
    Arg.OffsetLow=OffsetLow;
    Arg.Quadlet=SWAP(Quad);
    Arg.AckCode=ACK_MISSING;

    Result=m_pFireBus->WriteQuad(&Arg);

    // Check for continuous access restrictions
    if(m_AccessLatency)
     OSDelay(m_AccessLatency);

    if(Result==FCE_NOERROR)
     return TRUE;

    // See if AckCode came from a target (>=0x10 means hardware error)
    if(Arg.AckCode!=ACK_MISSING && Arg.AckCode<0x10)
    {
      switch(Arg.AckCode)
      {
        case ACK_BUSY_X:
        case ACK_BUSY_A:
        case ACK_BUSY_B:
         Sleep(10);
         break;
        
        case ACK_TYPE_ERROR:
        case ACK_ADDR_ERROR :
         m_LastError=Result;
         return FALSE;
        
        case ACK_COMPLETE:
        case ACK_PENDING:
         if(Arg.ResCode==RCODE_TYPE_ERROR || Arg.ResCode==RCODE_ADDRESS_ERROR)
         {
           m_LastError=Result;
           return FALSE;
         }
         break; 
      }
    }
    
    // Default sleep and repeat
    OSDelay(3); 
  }

  m_LastError=Result;

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Read block of data from a device.

UINT8 CFireNode::ReadBlock(UINT16 OffsetHigh,UINT32 OffsetLow,UINT8 *pMem,
                           UINT32 Len)
{
  UINT32      Result;
  FCTLREADBLK Arg;
  UINT32      i;

  // Check for non address assignment
  if(m_NodeAddress==NODEBROADCAST)
   return FALSE;

  // Defaulting to no error
  Result=HALER_NOERROR;

  for(i=0;i<=m_AccessRepeat;i++)
  {
    Arg.Speed=(UINT8)min(m_Flags.AsBits.Speed,m_Flags.AsBits.MaxSpeed);
    Arg.BusAddress=m_BusAddress;
    Arg.NodeAddress=m_NodeAddress;
    Arg.OffsetHigh=OffsetHigh;
    Arg.OffsetLow=OffsetLow;
    Arg.NoCopy=FALSE;
    Arg.pData=pMem;
    Arg.DataLen=Len;
    Arg.PktSize=0;

    Result=m_pFireBus->ReadBlock(&Arg);

    // Check for continuous access restrictions
    if(m_AccessLatency)
     OSDelay(m_AccessLatency);

    if(Result==FCE_NOERROR)
     return TRUE;

    // Give SONY cameras a chance on nonsuccess
    if(i>=1 && Result==HALER_NOACK)
     OSDelay(3);
  }

  m_LastError=Result;

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Write a block of data to a device.

UINT8 CFireNode::WriteBlock(UINT16 OffsetHigh,UINT32 OffsetLow,UINT8 *pMem,
                            UINT32 Len)
{
  UINT32       Result;
  FCTLWRITEBLK Arg;
  UINT32       i;

  // Check for non address assignment
  if(m_NodeAddress==NODEBROADCAST)
   return FALSE;

  // Defaulting to no error
  Result=HALER_NOERROR;

  for(i=0;i<=m_AccessRepeat;i++)
  {
    Arg.Speed=(UINT8)min(m_Flags.AsBits.Speed,m_Flags.AsBits.MaxSpeed);
    Arg.BusAddress=m_BusAddress;
    Arg.NodeAddress=m_NodeAddress;
    Arg.OffsetHigh=OffsetHigh;
    Arg.OffsetLow=OffsetLow;
    Arg.NoCopy=FALSE;
    Arg.pData=pMem;
    Arg.DataLen=Len;
    Arg.PktSize=0;

    Result=m_pFireBus->WriteBlock(&Arg);

    // Check for continuous access restrictions
    if(m_AccessLatency)
     OSDelay(m_AccessLatency);

    if(Result==FCE_NOERROR)
     return TRUE;

    // Give SONY cameras a chance on error
    if(i>=1 && Result==HALER_NOACK)
     OSDelay(3);
  }

  m_LastError=Result;

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Read vendor and device ID.

UINT8 CFireNode::ReadGuid()
{
  UINT32 Quad;

  m_Guid.ChipHigh=0;
  m_Guid.ChipLow=0;
  m_GuidValid=FALSE;

  if(!ReadQuad(INITIAL_REGISTER_SPACE_HI,
               INITIAL_REGISTER_SPACE_LO+CONFIG_ROM_LOCATION,
               &Quad))
   return FALSE;

  // We always delay here to give slow devices a chance
  OSDelay(3);

  // Check for minimal config ROM
  if((Quad>>24)==1)
  {
    m_Guid.Vendor=Quad&0xFFFFFF;
    m_GuidValid=TRUE;
    return TRUE;
  }

  // Read Vendor/Device Id from other location
  if(!ReadQuad(INITIAL_REGISTER_SPACE_HI,
               INITIAL_REGISTER_SPACE_LO+CONFIG_ROM_LOCATION+3*4,
               &Quad))
   return FALSE;

  // We always delay here to give slow devices a chance
  OSDelay(3);

  m_Guid.Vendor=Quad>>8;
  m_Guid.ChipHigh=Quad&0xFF;

  // Always slow down for stupid SONY cameras
  switch(m_Guid.Vendor)
  {
    case VENDORID_SONY:
     m_AccessLatency=3;
     break;
    
    case VENDORID_AVT: 
     m_AccessLatency=1;
     break;
     
    default:
     m_AccessLatency=1;
     break;  
  }

  if(!ReadQuad(INITIAL_REGISTER_SPACE_HI,
               INITIAL_REGISTER_SPACE_LO+CONFIG_ROM_LOCATION+4*4,
               &Quad))
   return FALSE;

  m_Guid.ChipLow=Quad;
  m_GuidValid=TRUE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Return pointer to our GUID (if valid).

FNGUID *CFireNode::pGuid()
{
  if(!m_GuidValid)
   return NULL;

  return &m_Guid;
}

////////////////////////////////////////////////////////////////////////////////
// Copy properties that can be changed by a busreset.

void CFireNode::CopyProperties(CFireNode *pFireNode)
{
  UINT8 i;

  m_BusAddress=pFireNode->BusAddress();
  m_NodeAddress=pFireNode->NodeAddress();

  m_SelfIdCnt=pFireNode->SelfIdCnt();
  for(i=0;i<m_SelfIdCnt;i++)
   m_SelfId[i]=pFireNode->SelfId(i);

  m_Flags.AsUINT32=pFireNode->Flags();

  m_PortCnt=pFireNode->PortCnt();
  for(i=0;i<m_PortCnt;i++)
   m_FirePort[i]=*(pFireNode->pFirePort(i));
}

////////////////////////////////////////////////////////////////////////////////
// Try to find what type of device we have found.

UINT8 CFireNode::InquireType()
{
  UINT32 Val;
  UINT32 Addr,RootLength,RootHdr,i;

  // Set default type
  m_Type=FNT_UNKNOWN;

  // Read ROOT header and go through all entries to find unit offset
  if(!ReadQuad(INITIAL_REGISTER_SPACE_HI,INITIAL_REGISTER_SPACE_LO+ROOTOFFSET,&RootHdr))
   return FALSE;

  Addr=0;
  RootLength=RootHdr>>16;
  for(i=0;i<RootLength;i++)
  {
    if(!ReadQuad(INITIAL_REGISTER_SPACE_HI,INITIAL_REGISTER_SPACE_LO+
                                           ROOTOFFSET+(i+1)*4,&Val))
     return FALSE;

    // See if we found unit directory offset
    if((Val>>24)==0xD1)
    {
      Addr=INITIAL_REGISTER_SPACE_LO+ROOTOFFSET+(i+1)*4+4*(Val&0xFFFFFF);
      break;
    }
  }
  if(!Addr)
   return FALSE;

  // Read unit specific ID
  if(!ReadQuad(INITIAL_REGISTER_SPACE_HI,Addr+4,&Val))
   return FALSE;

  if(Val==0x1200A02D)
   m_Type=FNT_DCAM;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Called on end of a busreset. Normal firenode does nothing.

void CFireNode::OnBusResetEnd()
{
}

////////////////////////////////////////////////////////////////////////////////
// Called on first detect after a busreset has occured.

UINT8 CFireNode::OnStartup()
{
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Called on bus shutdown.

void CFireNode::OnShutdown()
{
}

////////////////////////////////////////////////////////////////////////////////
// Copy operator.

void CFireNode::CopyFrom(CFireNode *pSrcNode)
{
  POSSEMAPHORE MyHandle;

  MyHandle=m_pLockSemaphore;

  // Copy whole other properties
  *this=*pSrcNode;

  m_pLockSemaphore=MyHandle;
}

////////////////////////////////////////////////////////////////////////////////
// Check identity fo GUID.

UINT8 CFireNode::CheckGuid(FNGUID *pGuid)
{
  if(m_Guid.Vendor==pGuid->Vendor &&
     m_Guid.ChipHigh==pGuid->ChipHigh &&
     m_Guid.ChipLow==pGuid->ChipLow)
   return TRUE;

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Set access latency between two accesses.

UINT32 CFireNode::SetAccessLatency(UINT32 Value)
{
  UINT32 OldLatency;

  OldLatency=m_AccessLatency;
  m_AccessLatency=Value;

  return OldLatency;
}

////////////////////////////////////////////////////////////////////////////////
// Return number of card we live on.

UINT8 CFireNode::CardNumber()
{
  if(!m_pFireBus)
   return (UINT8)-1;

  return m_pFireBus->CardNumber();
}

////////////////////////////////////////////////////////////////////////////////
// Return card handle we are living on.

CARDHANDLE CFireNode::CardHandle()
{
  if(!m_pFireBus)
   return NULL;

  return m_pFireBus->CardHandle();
}
