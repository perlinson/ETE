////////////////////////////////////////////////////////////////////////////////
// Module for FireBus object.
// intek, C. Kuehnel, 5.12.2002
////////////////////////////////////////////////////////////////////////////////

#include <firebus.h>
#include <camera.h>
#include <fctlmain.h>
#include <string.h>

#define MAKEUINT32(hi,lo)       (((UINT32)(hi)<<16)|(lo))

static char* ReasonString[] =
{
  "FB: BusResetBegin",
  "FB: BusResetEnd",
  "FB: Create request",
  "FB: New Device",
  "FB: Kill Device",
  "FB: Error",
  "FB: Errorflags set",
  "FB: Frame",
  "FB: Debug"
};

enum
{
  FS_IDLE=0,
  FS_CARDOPENED,
  FS_BUSOPENED,
  FS_RUNNING
};

static UINT32 IsoMask[] =
{
  HALEVT_ISORX0,HALEVT_ISORX1,HALEVT_ISORX2,HALEVT_ISORX3,
  HALEVT_ISORX4,HALEVT_ISORX5,HALEVT_ISORX6,HALEVT_ISORX7
};

////////////////////////////////////////////////////////////////////////////////
// Routines for CFireBus - Constructor.
////////////////////////////////////////////////////////////////////////////////

CFireBus::CFireBus()
{
  m_State=FS_IDLE;
  m_hCard=NULL;
  m_pCallback=(FBCALLBACK*)NULL;
  m_BusMgr=NODEBROADCAST;
  m_IsoMgr=NODEBROADCAST;
  m_MyNodeAddress=NODEBROADCAST;
  m_NodeCnt=0;
  m_SelfIdCnt=0;
  memset(m_pIsoDevice,0,sizeof(m_pIsoDevice));
  m_pAccessSemaphore=OSSemaphoreCreate(1);
  m_MaxSpeed=SPEED800;
}

////////////////////////////////////////////////////////////////////////////////
// Destructor.

CFireBus::~CFireBus()
{
  // no Callback to user in any form (he forgot to close the object!)
  m_pCallback=NULL;

  // Always stop our thread
  Stop();

  OSSemaphoreDelete(m_pAccessSemaphore);
}

////////////////////////////////////////////////////////////////////////////////
// Get clear text string for reason.

char* CFireBus::GetReasonString(UINT32 Reason)
{
  if(Reason>=sizeof(ReasonString)/sizeof(char*))
   return "Reason does not exist";

  return ReasonString[Reason];
}

////////////////////////////////////////////////////////////////////////////////
// Call user.

void CFireBus::CallUser(UINT32 Reason,void* pData)
{
  FBCBINFO Info;

  if(m_pCallback)
  {
    Info.CardNumber=m_CardNumber;
    Info.Reason=Reason;
    Info.pData=pData;
    Info.pContext=m_pContext;

    // Call the callback
    (*m_pCallback)(&Info);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Run.

UINT32 CFireBus::Run(UINT8 CardNumber,FBCALLBACK *pCallback,FBPARMS *pParms,
                     void *pContext)
{
  UINT32      Result=FCE_NOERROR;
  FCTLOPENBUS ArgOpen;

  // Check actual state
  if(m_State!=FS_IDLE)
   return FCE_STARTERROR;

  m_pCallback=pCallback;
  m_pContext=pContext;
  m_CardNumber=CardNumber;

  // Open card
  if(Result==FCE_NOERROR)
   Result=FCTLOpenCard(m_CardNumber,&m_hCard);

  // Get Card Id
  if(Result==FCE_NOERROR)
   Result=FCTLGetCardSystemId(m_CardNumber,&m_CardId);

  // Open bus
  if(Result==FCE_NOERROR)
  {
    m_State=FS_CARDOPENED;

    // Open our bus
    if(!pParms)
    {
      ArgOpen.BusAddress=0x3FF;
      ArgOpen.AsyRxPktCnt=32;
      ArgOpen.AsyRxPktPayload=2048;
      ArgOpen.AsyTxPktCnt=32;
      ArgOpen.AsyTxPktPayload=2048;
      ArgOpen.PowerClass=0;
      ArgOpen.StoredAccessCnt=16;
      ArgOpen.IrqTaskPrio=230;
      ArgOpen.IrqPrio=6;
      ArgOpen.IrmBmTaskPrio=229;
      ArgOpen.pCfgRom=(UINT32*)NULL;
      ArgOpen.GuidHigh=0;
      ArgOpen.GuidLow=0;
      ArgOpen.CfgRomLength=0;
      ArgOpen.SpecialFlags=FCTLSPEC_USERFRAMES;
    }
    else
    {
      ArgOpen.BusAddress=pParms->BusAddress;
      ArgOpen.AsyRxPktCnt=pParms->AsyRxPktCnt;
      ArgOpen.AsyRxPktPayload=pParms->AsyRxPktPayload;
      ArgOpen.AsyTxPktCnt=pParms->AsyTxPktCnt;
      ArgOpen.AsyTxPktPayload=pParms->AsyTxPktPayload;
      ArgOpen.PowerClass=pParms->PowerClass;
      ArgOpen.StoredAccessCnt=pParms->StoredAccessCnt;
      ArgOpen.pCfgRom=pParms->pCfgRom;
      ArgOpen.CfgRomLength=pParms->CfgRomLength;
      ArgOpen.IrmBmTaskPrio=pParms->IrmBmTaskPrio;
      ArgOpen.IrqTaskPrio=pParms->IrqTaskPrio;
      ArgOpen.IrqPrio=pParms->IrqPrio;
      ArgOpen.SpecialFlags=pParms->SpecialFlags;
    }

    m_BusAddress=ArgOpen.BusAddress;

    // Open our bus
    Result=FCTLOpenBus(m_hCard,&ArgOpen);
  }

  // Do event loop
  if(Result==FCE_NOERROR)
  {
    m_State=FS_BUSOPENED;
    EventLoop();
  }

  // Do shutdown
  if(Result==FCE_NOERROR)
   Shutdown();

  // Close our bus to stop bus activity
  if(m_State==FS_BUSOPENED)
  {
    FCTLCloseBus(m_hCard);
    m_State=FS_CARDOPENED;
  }

  // Remove all FireNodes
  FreeNodeList();

  // Close our card
  if(m_State==FS_CARDOPENED)
  {
    FCTLCloseCard(m_hCard);
    m_hCard=(CARDHANDLE*)NULL;
    m_State=FS_IDLE;
  }

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Shut down all activities.

void CFireBus::Shutdown()
{
  CFireNode *pFireNode;
  UINT8      i;

  // Notify all members that we will shutdown
  pFireNode=NULL;
  while((pFireNode=(CFireNode*)m_NodeBase.Next(pFireNode))!=NULL)
   pFireNode->OnShutdown();

  // Stop all iso dmas
  for(i=0;i<MAXISORXCHN;i++)
  {
    if(m_pIsoDevice[i])
    {
      CloseIsoDma(m_pIsoDevice[i]);
      m_pIsoDevice[i]=NULL;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Cancel our event loop.

UINT32 CFireBus::Stop()
{
  UINT32 Result=FCE_STARTERROR;

  if(m_State==FS_RUNNING)
  {
    // Set cancel event
    if(m_hCard)
     Result=FCTLCancelWaitEvent(m_hCard);
  }

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Read quadlet.

UINT32 CFireBus::ReadQuad(FCTLREADQUAD *pArg)
{
  UINT8  OldAddress;
  UINT32 Result;

  OldAddress=pArg->NodeAddress;
  if(OldAddress==m_MyNodeAddress)
   pArg->NodeAddress=NODEIDSELF;

  Result=FCTLReadQuad(m_hCard,pArg,NULL);
  pArg->NodeAddress=OldAddress;

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Write quadlet.

UINT32 CFireBus::WriteQuad(FCTLWRITEQUAD *pArg)
{
  UINT8  OldAddress;
  UINT32 Result;

  OldAddress=pArg->NodeAddress;
  if(OldAddress==m_MyNodeAddress)
   pArg->NodeAddress=NODEIDSELF;

  Result=FCTLWriteQuad(m_hCard,pArg,NULL);
  pArg->NodeAddress=OldAddress;

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Read block.

UINT32 CFireBus::ReadBlock(FCTLREADBLK *pArg)
{
  UINT8   OldAddress;
  UINT32  Result;
  UINT8  *pOldMem;
  FCTLLOCKMEMORY ArgLock;

  // We MUST do a lock of the block, otherwise we might get a blue screen!
  ArgLock.pMem=pArg->pData;
  ArgLock.Length=pArg->DataLen;

  Result=FCTLLockMemory(m_hCard,&ArgLock);
  if(Result!=FCE_NOERROR)
   return Result;

  pOldMem=pArg->pData;
  pArg->pData=ArgLock.pLockMem;

  OldAddress=pArg->NodeAddress;
  if(OldAddress==m_MyNodeAddress)
   pArg->NodeAddress=NODEIDSELF;

  Result=FCTLReadBlk(m_hCard,pArg,NULL);

  // Restore original values
  pArg->NodeAddress=OldAddress;
  pArg->pData=pOldMem;

  // Unlock memory
  FCTLUnlockMemory(m_hCard,ArgLock.pLockMem);

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Write block.

UINT32 CFireBus::WriteBlock(FCTLWRITEBLK *pArg)
{
  UINT8  OldAddress;
  UINT32 Result;

  OldAddress=pArg->NodeAddress;
  if(OldAddress==m_MyNodeAddress)
   pArg->NodeAddress=NODEIDSELF;

  Result=FCTLWriteBlk(m_hCard,pArg,NULL);
  pArg->NodeAddress=OldAddress;

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Get maximum speed between devices.

UINT8 CFireBus::MaxSpeed(UINT8 NodeAddress)
{
  UINT32 Result;
  FCTLMAXSPEED Arg;

  Arg.Node0=m_MyNodeAddress;
  Arg.Node1=NodeAddress;

  Result=FCTLGetMaxSpeedBetweenDevices(m_hCard,&Arg);
  if(Result!=FCE_NOERROR)
   return SPEED100;
  else
   return Arg.MaxSpeed;
}

////////////////////////////////////////////////////////////////////////////////
// Limit maximum speed for all devices.

void CFireBus::LimitSpeed(UINT8 MaxSpeed)
{
  m_MaxSpeed=MaxSpeed;
}

////////////////////////////////////////////////////////////////////////////////
// Reaction on error.

void CFireBus::OnError()
{
  UINT32   Result,ErrorFlags;

  Result=FCTLGetErrors(m_hCard,&ErrorFlags);
  if(Result==FCE_NOERROR)
   CallUser(FBN_FLAGGEDERROR,&ErrorFlags);
}

////////////////////////////////////////////////////////////////////////////////
// Reaction on bus reset begin.

void CFireBus::OnBusResetBegin()
{
  CallUser(FBN_BUSRESETBEGIN,NULL);
}

////////////////////////////////////////////////////////////////////////////////
// Look whether we can find an identical node within a list.
// If we find one we unlink this from the list and return it.

CFireNode* CFireBus::FindNode(CLink *pBase,CFireNode *pTmpNode)
{
  CFireNode *pFireNode;
  FNGUID *pFireGuid,*pTmpGuid;

  pFireNode=(CFireNode*)NULL;

  do
  {
    pFireNode=(CFireNode*)pBase->Next(pFireNode);
    if(pFireNode)
    {
      // Compare these both nodes
      pFireGuid=pFireNode->pGuid();
      pTmpGuid=pTmpNode->pGuid();
      if(pFireGuid && pTmpGuid &&
         (memcmp(pFireGuid,pTmpGuid,sizeof(FNGUID))==0))
        return pFireNode;
    }
  }while(pFireNode);

  return (CFireNode*)NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Free our nodelist.

void CFireBus::FreeNodeList()
{
  CFireNode *pFireNode;

  while((pFireNode=(CFireNode*)m_NodeBase.FirstAndUnlink())!=NULL)
  {
    // Call user if device was valid
    if(pFireNode->pGuid())
     CallUser(FBN_KILLDEVICE,pFireNode);

    // Always lock before deletion
    pFireNode->Lock();

    delete pFireNode;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Link existing nodelist to represent actual nodetree.
// The algorithm is as follows:
// Search from lowest numbered node (upwards) and from right port to left for
// childconnected port.
// Search from actual node downwards for parentconnected port.
// Link these both.

void CFireBus::LinkNodeList()
{
  CFireNode *pActual,*pOther;
  CFirePort *pActualsPort,*pOthersPort;
  UINT8      i,j;
  UINT32     Error;

  pActual=NULL;
  m_pRoot=NULL;
  do
  {
    pActual=(CFireNode*)m_NodeBase.Next(pActual);
    if(pActual)
    {
      // Last node determines the root
      m_pRoot=pActual;
      for(i=pActual->PortCnt();i!=0;i--)
      {
        pActualsPort=pActual->pFirePort(i-1);
        if(pActualsPort->State()==SELF_ID_CONNECTED_TO_CHILD && pActualsPort->FireNode()==NULL)
        {
          pOther=pActual;
          do
          {
            pOther=(CFireNode*)m_NodeBase.Prev(pOther);
            if(pOther)
            {
              for(j=pOther->PortCnt();j!=0;j--)
              {
                pOthersPort=pOther->pFirePort(j-1);
                if(pOthersPort->State()==SELF_ID_CONNECTED_TO_PARENT &&
                   pOthersPort->FireNode()==NULL)
                {
                  pActualsPort->FireNode()=pOther;
                  pOthersPort->FireNode()=pActual;
                  pOther=NULL;
                  break;
                }
              }
            }
          }while(pOther);

          // Check for a link error (Port MUST be linked!)
          if(pActualsPort->FireNode()==NULL)
          {
            Error=FCE_NODETAILS;
            CallUser(FBN_ERROR,&Error);
          }
        }
      }
    }
  }while(pActual);
}

////////////////////////////////////////////////////////////////////////////////
// Process a temporarily created node.

void CFireBus::ProcessTmpNode(CLink *pBase,CFireNode *pTmpNode)
{
  CFireNode *pOldNode,*pNewNode;
  FBNCREATE Create;
  UINT32    Error;
  FNFLAGS   Flags;

  // Try to read GUID information and type
  Flags.AsUINT32=pTmpNode->Flags();
  if(Flags.AsBits.LinkActive)
  {
    // Stop processing if error occurs here
    if(!pTmpNode->ReadGuid() || !pTmpNode->InquireType())
     return;
  }

  // Find this node in old tree
  pOldNode=FindNode(pBase,pTmpNode);
  if(pOldNode)
  {
    // If found copy actual properties
    pOldNode->Unlink();
    pOldNode->CopyProperties(pTmpNode);

    // Append to our actual list
    pOldNode->LinkBefore(&m_NodeBase);
  }
  else
  {
    // Inform user anyway and let him create a derived FireNode object
    Create.pTmpNode=pTmpNode;
    Create.pNewNode=NULL;

    CallUser(FBN_CREATEDEVICE,&Create);

    // See if user has created a node
    pNewNode=Create.pNewNode;

    // If no object we create (we MUST create a node for complete bustree)
    if(!pNewNode)
    {
      // Create one of the known nodes
      switch(pTmpNode->Type())
      {
        case FNT_DCAM : pNewNode=new CCamera;
                        break;

        default       : pNewNode=new CFireNode;
                        break;
      }

      // Report error if no memory
      if(!pNewNode)
      {
        Error=FCE_NOMEM;
        CallUser(FBN_ERROR,&Error);
      }
    }

    // If there is a new one we link and call it's Startup routine
    if(pNewNode)
    {
      // Copy CFireNode values and link
      pNewNode->CopyFrom(pTmpNode);
      pNewNode->LinkBefore(&m_NodeBase);

      // Call objects Startup routine
      pNewNode->OnStartup();

      // Notify application anyway
      CallUser(FBN_NEWDEVICE,pNewNode);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Rebuild our nodelist from all SelfIds.

void CFireBus::RebuildNodelist()
{
  CLink     OldBase;
  CFireNode TmpNode,*pFireNode;
  UINT8     SelfIdIx;
  UINT8     SelfIdsRead;
  CLink    *pLink;
  UINT32    Error,i;
  CCamera  *pCamera;
  FCTLISORESOURCES Arg;

  // Save existing list
  if((pLink=m_NodeBase.First())!=(CLink*)NULL)
  {
    m_NodeBase.Unlink();
    OldBase.LinkBefore(pLink);
  }

  // Go through all SelfIds and move extracted node from old list or create new
  SelfIdIx=0;
  do
  {
    // Assign actual SelfIds to TmpNode
    SelfIdsRead=TmpNode.AssignProperties(this,&m_SelfId[SelfIdIx],m_MaxSpeed);
    SelfIdIx+=SelfIdsRead;

    // Check result of SelfId assignment and collect some information
    if(!SelfIdsRead)
    {
      Error=FCE_NODETAILS;
      CallUser(FBN_ERROR,&Error);
    }
    else
    {
      // Process our new node
      ProcessTmpNode(&OldBase,&TmpNode);
    }
  }while(SelfIdsRead && (SelfIdIx<m_SelfIdCnt));

  // Now we go through all nodes still hanging around in OldList and remove!
  while((pFireNode=(CFireNode*)OldBase.FirstAndUnlink())!=(CFireNode*)NULL)
  {
    pFireNode->InvalidateAddress();

    // Notify user (ATTENTION: address not valid)
    CallUser(FBN_KILLDEVICE,pFireNode);

    // Always lock before deletion
    pFireNode->Lock();
    delete pFireNode;
  }

  // Link our existing node list
  LinkNodeList();

  // Go through all ISO devices and reallocate channels
  for(i=0;i<MAXISORXCHN;i++)
  {
    if((pCamera=m_pIsoDevice[i])!=NULL && pCamera->IsoAllocated())
    {
      memset(&Arg,0,sizeof(Arg));
      if(pCamera->pParms()->IsoChn<32)
       Arg.ChannelsHigh|=0x80000000>>pCamera->pParms()->IsoChn;
      else
       Arg.ChannelsLow|=0x80000000>>(pCamera->pParms()->IsoChn-32);

      FCTLIsoAllocateResources(m_hCard,&Arg);
    }
  }

  // Notify all active nodes over BusResetEnd condition
  pFireNode=NULL;
  while((pFireNode=(CFireNode*)m_NodeBase.Next(pFireNode))!=(CFireNode*)NULL)
   pFireNode->OnBusResetEnd();
}

////////////////////////////////////////////////////////////////////////////////
// Reaction on bus reset end.

void CFireBus::OnBusResetEnd()
{
  FCTLGETBUSINFO BusInfo;
  UINT32   Result;

  // Get all properties from this reset
  BusInfo.MaxSelfIds=sizeof(m_SelfId)/sizeof(UINT32);
  BusInfo.pSelfId=m_SelfId;

  Result=FCTLGetBusInfo(m_hCard,&BusInfo);
  if(Result==FCE_NOERROR)
  {
    // Extract all information we need later on
    m_BusAddress=BusInfo.BusAddress;
    m_MyNodeAddress=BusInfo.NodeAddress;
    m_BusMgr=BusInfo.BmAddress;
    m_IsoMgr=BusInfo.IrmAddress;
    m_NodeCnt=BusInfo.DevCnt;
    m_GenerationCount=BusInfo.GenerationCount;
    m_SelfIdCnt=BusInfo.SelfIdsCopied;

    // Rebuild our nodelist
    RebuildNodelist();

    // Notify our application
    CallUser(FBN_BUSRESETEND,NULL);
  }
  else
  {
    // Notify application about error
    CallUser(FBN_ERROR,&Result);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Perform busreset.

UINT32 CFireBus::BusReset()
{
  if(m_hCard==NULL)
   return FCE_NOTOPENED;

  return FCTLBusReset(m_hCard);
}

////////////////////////////////////////////////////////////////////////////////
// Lock access.

void CFireBus::LockAccess()
{
  OSSemaphorePend(m_pAccessSemaphore,OSINFTIME);
}

////////////////////////////////////////////////////////////////////////////////
// Unlock access.

void CFireBus::UnlockAccess()
{
  OSSemaphorePost(m_pAccessSemaphore);
}

////////////////////////////////////////////////////////////////////////////////
// Get ISO channel. We allocate Iso channel within our iso resource manager.

UINT8 CFireBus::GetIsoChannel(CCamera *pCamera)
{
  UINT32 Result;
  FCTLISORESOURCES Arg;
  UINT32 i;

  do
  {
    Result=FCTLIsoQueryResources(m_hCard,&Arg);
    if(Result!=HALER_NOERROR)
     return FALSE;

    for(i=0;i<64;i++)
    {
      if(i<32)
      {
        if(Arg.ChannelsHigh&(0x80000000>>i))
         break;
      }
      else
      {
        if(Arg.ChannelsLow&(0x80000000>>(i-32)))
         break;
      }
    }

    // Return if no free channel
    if(i>63)
     return FALSE;

    memset(&Arg,0,sizeof(Arg));
    if(i<32)
     Arg.ChannelsHigh|=(0x80000000>>i);
    else
     Arg.ChannelsLow|=(0x80000000>>(i-32));

    Result=FCTLIsoAllocateResources(m_hCard,&Arg);
    if(Result!=HALER_NOERROR)
     return FALSE;

    if(!Arg.ChannelsHigh && !Arg.ChannelsLow)
     i=64;

  }while(i>=64);

  pCamera->SetIsoChannel((UINT8)i);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Free iso channel.

void CFireBus::FreeIsoChannel(CCamera *pCamera)
{
  UINT8 Channel;
  FCTLISORESOURCES Arg;

  LockAccess();
  Channel=pCamera->pParms()->IsoChn;
  pCamera->SetIsoChannel(0xFF);
  UnlockAccess();

  memset(&Arg,0,sizeof(Arg));
  if(Channel<32)
   Arg.ChannelsHigh|=(0x80000000>>Channel);
  else
   Arg.ChannelsLow|=(0x80000000>>(Channel-32));

  FCTLIsoFreeResources(m_hCard,&Arg);
}

////////////////////////////////////////////////////////////////////////////////
// Open ISO Rx dma (it's definetely active after this call).
// For configuration we use basic parameters from the camera (as far as we can
// do this).

UINT8 CFireBus::OpenIsoDma(CCamera *pCamera,FCTLLOCKMEMORY *pUserFrames)
{
  FCTLISOSTARTLISTEN Arg;
  UINT32             Result,i;
  FCTLFRMINFO        FrmInfo;
  UINT32             UserFrameCnt;

  if(m_State!=FS_RUNNING || !m_hCard || !GetIsoChannel(pCamera))
   return FALSE;

  // Set these parameters in camera
  pCamera->pParms()->DmaIsoChnMsk=0x3F;
  pCamera->pParms()->DmaIsoChnVal=pCamera->pParms()->IsoChn;

  // Set required parameters
  if(pUserFrames)
   Arg.FrameCnt=0;
  else
   Arg.FrameCnt=(UINT16)pCamera->pParms()->BufferCnt;

  Arg.PktCnt  =pCamera->pParms()->PktCnt;
  Arg.PktSize =((UINT32)pCamera->pParms()->PktOffset<<16)|pCamera->pParms()->PktSize;
  Arg.FirstMaskCmp =MAKEUINT32(pCamera->pParms()->FirstMsk,
                               pCamera->pParms()->FirstVal);
  Arg.IsoChnMaskCmp=MAKEUINT32(pCamera->pParms()->DmaIsoChnMsk,
                               pCamera->pParms()->DmaIsoChnVal);

  // Set our user info (not required for functionality!)
  Arg.UserInfo.XPos=pCamera->pParms()->XPos;
  Arg.UserInfo.YPos=pCamera->pParms()->YPos;
  Arg.UserInfo.XSize=pCamera->pParms()->XSize;
  Arg.UserInfo.YSize=pCamera->pParms()->YSize;
  Arg.UserInfo.ColorFormat=pCamera->pParms()->ColorFormat;

  // Call library
  Result=FCTLIsoStartListen(m_hCard,&Arg);
  if(Result!=FCE_NOERROR)
  {
    FreeIsoChannel(pCamera);
  }
  else
  {
    pCamera->pParms()->DmaChn=(UINT8)Arg.IsoNr;
    m_pIsoDevice[Arg.IsoNr]=pCamera;

    if(pUserFrames)
    {
      // Prepare frame info
      FrmInfo.UserInfo.XPos=pCamera->pParms()->XPos;
      FrmInfo.UserInfo.YPos=pCamera->pParms()->YPos;
      FrmInfo.UserInfo.XSize=pCamera->pParms()->XSize;
      FrmInfo.UserInfo.YSize=pCamera->pParms()->YSize;
      FrmInfo.UserInfo.ColorFormat=pCamera->pParms()->ColorFormat;

      FrmInfo.System=NULL;
      FrmInfo.IsoNr=(UINT8)Arg.IsoNr;
      FrmInfo.Length=pCamera->pParms()->PktCnt*pCamera->pParms()->PktSize;

      // Post all user buffers to our DMA (Set realbuffer count to zero)
      UserFrameCnt=pCamera->pParms()->RealBufferCnt;
      pCamera->pParms()->RealBufferCnt=0;
      for(i=0;i<UserFrameCnt;i++)
      {
        FrmInfo.pData=pUserFrames->pLockMem;
        Result=FCTLIsoPost(m_hCard,&FrmInfo);
        if(Result!=FCE_NOERROR)
        {
          // No memory is ok, we just stop post (happens if system is at the limit)
          if(Result==HALER_NOMEM)
          {
            // We just stop here
            return TRUE;
          }
          else
          {
            FCTLIsoStopListen(m_hCard,Arg.IsoNr);
            FreeIsoChannel(pCamera);
            break;
          }
        }
        pUserFrames++;
        pCamera->pParms()->RealBufferCnt++;
      }
    }
    else
    {
      pCamera->pParms()->RealBufferCnt=Arg.Allocated;
    }
  }

  return (Result==FCE_NOERROR)?TRUE:FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Close ISO Rx dma.

UINT8 CFireBus::CloseIsoDma(CCamera *pCamera)
{
  if(m_State!=FS_RUNNING || !m_hCard || pCamera->pParms()->DmaChn>=MAXISORXCHN ||
     m_pIsoDevice[pCamera->pParms()->DmaChn]!=pCamera)
   return FALSE;

  LockAccess();

  FCTLIsoStopListen(m_hCard,pCamera->pParms()->DmaChn);

  // Clear queue and event (driver does not do this by itself)!!!
  FCTLIsoClear(m_hCard,pCamera->pParms()->DmaChn);
  m_pIsoDevice[pCamera->pParms()->DmaChn]=NULL;

  UnlockAccess();

  FreeIsoChannel(pCamera);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Called on receiption of an iso buffer.

void CFireBus::OnIsoRx(UINT8 IsoChn)
{
  UINT32 Result;
  FCTLFRMINFO FrmInfo;

  // Check for errors
  if(IsoChn>=MAXISORXCHN)
   return;

  // Lock our access
  LockAccess();

  // Always catch frame
  Result=FCTLIsoInquire(m_hCard,IsoChn,&FrmInfo);
  if(Result==FCE_NOERROR)
  {
    // Ignore frame start events
    if(FrmInfo.pData!=NULL)
    {
      // See if someone has stolen our IsoDevice (i.e. CloseIsoDma)!
      if(m_pIsoDevice[IsoChn]==NULL)
      {
        // Immediately return this frame
        FCTLIsoPost(m_hCard,&FrmInfo);
      }
      else
      {
        // Post frame to our iso device and notify user on success
        if(m_pIsoDevice[IsoChn]->OnFrame(&FrmInfo))
         CallUser(FBN_FRAME,(void*)m_pIsoDevice[IsoChn]);
      }
    }
    else
    {
      // Little Notification on frame start events (for test purposes only)
      // Beep(1000,5);
    }
  }

  // Unlock access
  UnlockAccess();
}

////////////////////////////////////////////////////////////////////////////////
// Called on iso events.

void CFireBus::OnIsoEvent(UINT32 EventFlags)
{
  UINT8 i;

  for(i=0;i<8;i++)
  {
    if(EventFlags&IsoMask[i])
     OnIsoRx(i);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Event loop for our object.

void CFireBus::EventLoop()
{
  UINT32 EventFlags;
  UINT32 Result;

  m_State=FS_RUNNING;

  do
  {
    // Wait for an event
    Result=FCTLWaitEvent(m_hCard,&EventFlags,TIMEINFINITE);
    if(Result==FCE_NOERROR)
    {
      if(EventFlags&HALEVT_ERROR)
       OnError();

      if(EventFlags&HALEVT_BUSRESETBEGIN)
       OnBusResetBegin();

      if(EventFlags&HALEVT_BUSRESETEND)
       OnBusResetEnd();

      if(EventFlags&HALEVTMSK_ISORX)
       OnIsoEvent(EventFlags);
    }
  }while(Result==FCE_NOERROR && 
         !(EventFlags&HALEVT_CANCELLED) && !(EventFlags&HALEVT_SHUTDOWN));

  m_State=FS_BUSOPENED;
}

////////////////////////////////////////////////////////////////////////////////
// Return an iso frame back to our library.

void CFireBus::ReturnFrame(CCamera *pCamera,FCTLFRMINFO *pInfo)
{
  if(m_hCard && (m_State==FS_RUNNING) && pCamera->pParms()->DmaChn<=MAXISORXCHN &&
     (m_pIsoDevice[pCamera->pParms()->DmaChn]==pCamera))
   FCTLIsoPost(m_hCard,pInfo);
}

////////////////////////////////////////////////////////////////////////////////
// Running information.

UINT8 CFireBus::IsRunning()
{
  return m_State==FS_RUNNING?TRUE:FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Global get system information Calculator.
////////////////////////////////////////////////////////////////////////////////

UINT8 GetSysInfo(FBSYSINFO *pInfo)
{
  UINT32 i;
#if 1
  // Inquire Cardcount
  pInfo->CardCount=0;
  if(FCTLGetCardCount(&pInfo->CardCount)!=FCE_NOERROR)
   return FALSE;

  // Get system IDs for our cards
  for(i=0;i<pInfo->CardCount;i++)
  {
    if(FCTLGetCardSystemId(i,&pInfo->CardId[i].AsUINT32)!=FCE_NOERROR)
     return FALSE;
  }
#endif
  return TRUE;
}