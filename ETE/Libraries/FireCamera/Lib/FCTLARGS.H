/*******************************************************************************
  Headerfile for FCTL function arguments.
  c. kuehnel, intek, 27.11.2001
*******************************************************************************/

#ifndef FCTLARGS_H
#define FCTLARGS_H

#include <basetype.h>

#define FIRESTACKVERSION         0x02080000     /* Version of this module */

#ifndef POSEVENT
#define POSEVENT void*                          /* For use under WIN32 API */
#endif

typedef void* CARDHANDLE;                       /* Type for Cardhandle */
typedef void* FCTLHANDLE;                       /* Handle */

#define AUTOSPEED                 0xFF          /* Choose max. possible speed */
#define NODEIDSELF                0xFF          /* ID for accessing self */

#define FCTLACCESSREADQUAD        0x00000001    /* Accessflags for map */
#define FCTLACCESSWRITEQUAD       0x00000002
#define FCTLACCESSREADBLK         0x00000004
#define FCTLACCESSWRITEBLK        0x00000008

#define FCTLACCESSLOCKMASKSWAP    0x00000010
#define FCTLACCESSLOCKCOMPARESWAP 0x00000020
#define FCTLACCESSLOCKFETCHADD    0x00000040
#define FCTLACCESSLOCKLITTLEADD   0x00000080
#define FCTLACCESSLOCKBOUNDEDADD  0x00000100
#define FCTLACCESSLOCKWRAPADD     0x00000200

#define FCTLACCESSREAD   (FCTLACCESSREADQUAD|FCTLACCESSREADBLK)
#define FCTLACCESSWRITE  (FCTLACCESSWRITEQUAD|FCTLACCESSWRITEBLK)
#define FCTLACCESSLOCK   (FCTLACCESSLOCKMASKSWAP|FCTLACCESSLOCKCOMPARESWAP|\
                          FCTLACCESSLOCKFETCHADD|FCTLACCESSLOCKLITTLEADD|\
                          FCTLACCESSLOCKBOUNDEDADD|FCTLACCESSLOCKWRAPADD)

#define FCTLACCESSALL    (FCTLACCESSREAD|FCTLACCESSWRITE)

#define FCTLLOCK_ISNONPAGED       0x00000008    /* Memory is locked already */
#define FCTLLOCK_NOSYSTEMMAP      0x00000004    /* Don't map to system */

#define FCTLAR_NORELEASE          0             /* No User release */
#define FCTLAR_BUSYSIGNALING      1             /* Stack signals busy */
#define FCTLAR_APPRESPONSE        2             /* Response by app. */

#define NODEPROPIRMC              0x80000000
#define NODEPROPCMC               0x40000000
#define NODEPROPISC               0x20000000
#define NODEPROPBMC               0x10000000

#define FSNOOP_DATA               0x80000000

#define TIMEINFINITE              0xFFFFFFFF

typedef void FCTLUSERCB(void *pContext,UINT32 Arg0,UINT32 Arg1);

/*******************************************************************************
  Synchronisation object.
*******************************************************************************/

/* This object is private and exists outside */
struct fctlsync;
#define FCTLSYNC struct fctlsync

/*******************************************************************************
  System header.
*******************************************************************************/

typedef struct
{
  UINT32        Code;                   /* Code of command and result */
  union
  {
    void*       Ptr;
    UINT32      Val;
  }             Arg[3];                 /* Arguments (Pointer or UINT32) */
}SYSHDR;

/*******************************************************************************
  System only.
*******************************************************************************/

typedef struct
{
  UINT32        LogLen  : 14;
  UINT32        Speed   : 2;
  UINT32        AckCode : 4;
  UINT32        Unused  : 3;
  UINT32        DataLog : 1;
  UINT32        Pattern : 8;
  UINT32        Time;
}SNOOPTOKEN;

/*******************************************************************************
  System only.
*******************************************************************************/

typedef struct
{
  UINT32        TCode[16];
  UINT32        AckCode[32];
  UINT32        Speed[4];
  UINT32        PhyPkt;
  UINT32        PktTotal;
  UINT32        BusResetBegin;
  UINT32        BusResetEnd;
  UINT32        Error;
}SNOOPSTAT;

/*******************************************************************************
  System only.
*******************************************************************************/

typedef struct snoopmem
{
  UINT32        ReadIx;
  UINT32        WriteIx;
  void         *UserAddress;
  void         *pOSPhysMem;
  UINT32        MemSize;
  UINT32        BufSize;
  UINT32        Control;
  UINT32        Filter;
  UINT8         OverRun;
  SNOOPSTAT     Stat;
}SNOOPMEM;

/*******************************************************************************
  Card property descriptor.
*******************************************************************************/

typedef struct
{
  void*         BaseAddress;            /* Base address of this card  */
  UINT8         HardwareType;           /* Hardware type */
  UINT8         PCIBus;                 /* PCI bus */
  UINT8         PCIDevice;              /* PCI device */
  UINT8         PCIFunction;            /* PCI function */
  void*         User;                   /* Free user parameter */
  UINT32        PCIVendorId;            /* PCI Vendor ID */
  UINT32        PCIDeviceId;            /* PCI Device ID */
  UINT8         PCIRevisionId;          /* PCI Revision ID */
}FCTLCARDDESC;

/*******************************************************************************
  Argument types.
*******************************************************************************/

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT16        BusAddress;             /* Busaddress to use */
  UINT32        AsyRxPktCnt;            /* Max. asynch. receive buffers */
  UINT32        AsyRxPktPayload;        /* Size of asynch. receive buffers */
  UINT32        AsyTxPktCnt;            /* Max. asynch. transmit buffers */
  UINT32        AsyTxPktPayload;        /* Size of asynch. transmit buffers */
  UINT16        StoredAccessCnt;        /* Number of external stored accesses */
  UINT8         PowerClass;             /* Powerclass (in Phys SelfId) */
  UINT32       *pCfgRom;                /* Log. pointer to ROM */
  UINT16        CfgRomLength;           /* Length of CFGROM in quadlets */
  UINT16        IrmBmTaskPrio;          /* Taskpriority Ir/Bmanager (0=none) */
  UINT16        IrqTaskPrio;            /* Taskpriority of deferred handler */
  UINT8         IrqPrio;                /* IRQ-Prio and upper 2 bits IRQ-Line */
  UINT32        SpecialFlags;           /* Flags for special handling */
  UINT8         IsoRxDmaCnt;            /* Number of available ISO RX DMAs */
  UINT8         IsoTxDmaCnt;            /* Number of available ISO TX DMAs */
  UINT32        GuidHigh;               /* VendorId ChipdIdHigh */
  UINT32        GuidLow;                /* ChipIdLow */
}FCTLOPENBUS;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT16        BusAddress;             /* Bus address */
  UINT8         NodeAddress;            /* Node address */
  UINT8         BmAddress;              /* Busmanager address */
  UINT8         IrmAddress;             /* Isochronous resource manager addr. */
  UINT8         DevCnt;                 /* Filled with number of busmembers */
  UINT32        GenerationCount;        /* Bus reset generation count */
  UINT32        SelfIdsCopied;          /* Number of SelfIds copied */
  UINT32        MaxSelfIds;             /* Maximum number of SelfIds */
  UINT32       *pSelfId;                /* Pointer to SelfId area */
}FCTLGETBUSINFO;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT8         Speed;                  /* Speed */
  UINT16        BusAddress;             /* Bus address */
  UINT8         NodeAddress;            /* Node address */
  UINT16        OffsetHigh;             /* Offset high */
  UINT32        OffsetLow;              /* Offset low */
  UINT32        Quadlet;                /* Quadlet to write */
  UINT8         AckCode;                /* Acknowledge code */
  UINT8         ResCode;                /* Responsecode */
}FCTLWRITEQUAD;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT8         Speed;                  /* Speed */
  UINT16        BusAddress;             /* Bus address */
  UINT8         NodeAddress;            /* Node address */
  UINT16        OffsetHigh;             /* Offset high */
  UINT32        OffsetLow;              /* Offset low */
  UINT32        Quadlet;                /* Space for quadlet */
  UINT8         AckCode;                /* Acknowledge code */
  UINT8         ResCode;                /* Responsecode */
}FCTLREADQUAD;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT8         Speed;                  /* Speed */
  UINT16        BusAddress;             /* Bus address */
  UINT8         NodeAddress;            /* Node address */
  UINT16        OffsetHigh;             /* Offset high */
  UINT32        OffsetLow;              /* Offset low */
  UINT8         NoCopy;                 /* Flag for no copy */
  UINT8        *pData;                  /* Pointer to data */
  UINT32        DataLen;                /* Length of data */
  UINT32        PktSize;                /* Packet size to use */
  UINT8         AckCode;                /* Acknowledge code */
  UINT8         ResCode;                /* Responsecode */
}FCTLWRITEBLK;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT8         Speed;                  /* Speed */
  UINT16        BusAddress;             /* Bus address */
  UINT8         NodeAddress;            /* Node address */
  UINT16        OffsetHigh;             /* Offset high */
  UINT32        OffsetLow;              /* Offset low */
  UINT8         NoCopy;                 /* Flag for no copy */
  UINT8        *pData;                  /* Pointer to buffer to hold data */
  UINT32        DataLen;                /* Length of data */
  UINT32        PktSize;                /* Packet size to use */
  UINT8         AckCode;                /* Acknowledge code */
  UINT8         ResCode;                /* Responsecode */
}FCTLREADBLK;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT8         Speed;                  /* Speed */
  UINT16        BusAddress;             /* Bus address */
  UINT8         NodeAddress;            /* Node address */
  UINT16        OffsetHigh;             /* Offset high */
  UINT32        OffsetLow;              /* Offset low */
  UINT16        LockCode;               /* Type of lock operation */
  UINT8         LockSizeIs64;           /* Length of lock operation */
  UINT32        Argument[2];            /* Input argument */
  UINT32        Data[2];                /* Input data */
  UINT32        Result[2];              /* Result data */
  UINT8         AckCode;                /* Acknowledge code */
  UINT8         ResCode;                /* Responsecode */
}FCTLLOCKREQUEST;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT16        FrameCnt;               /* Number of frames to allocate */
  UINT8         DmaMode;                /* Dma mode: 0=normal 1=Limp 2=Replace */
  UINT8         DmaFlags;               /* DMA flags */
  UINT32        PktCnt;                 /* Number of packets for a frame */
  UINT32        PktSize;                /* Payload per packet */
  UINT32        FirstMaskCmp;           /* First compare value */
  UINT32        IsoChnMaskCmp;          /* Iso channel check value */
  USERINFO      UserInfo;               /* UserInfo for our frames */
  UINT32        IsoNr;                  /* Return value for selected DMA */
  UINT32        Allocated;              /* Return value for allocated frames */
}FCTLISOSTARTLISTEN;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT32        IsoNr;                  /* Iso Number */
  UINT32        PktCnt;                 /* New packet count */
  UINT32        PktSize;                /* New packet size */
  USERINFO      UserInfo;               /* New user info */
}FCTLISORESIZE;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT32        Type;                   /* Type of access */
  UINT16        OffsetHigh;             /* Offset high on 1394 (16 Bit!!!) */
  UINT32        OffsetLow;              /* Offset low on 1394 */
  UINT32        Length;                 /* Length of area in bytes */
  UINT8        *pMem;                   /* Memory address for access */
  UINT8         Notify;                 /* Notify on access? */
  UINT8         AppRelease;             /* Application release after write */
  void*         User;                   /* Software, free for user */
  FCTLHANDLE    MapHandle;              /* Handle for this range */
}FCTLMAPADDRESS;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  void*         System;                 /* Used internally only */
  USERINFO      UserInfo;               /* Userinfo */
  UINT8         IsoNr;                  /* IsoNr of frame */
  UINT8         Last;                   /* Last in queue */
  UINT16        Id;                     /* Id of frame */
  UINT32        Length;                 /* Length of data frame */
  UINT8        *pData;                  /* Pointer to video data */
}FCTLFRMINFO;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT32        Type;                   /* Type */
  UINT32        Offset;                 /* Offset of accessed memory */
  UINT32        Length;                 /* Length of access */
  UINT8         Speed;                  /* Speed of access */
  UINT16        SrcBus;                 /* Bus address of requester */
  UINT8         SrcNode;                /* Node address of requester */
  UINT8         MustRelease;            /* Must release? */
  void*         User;                   /* Userdefineable from MemRange */
  FCTLHANDLE    MapHandle;              /* For system use */
  UINT8         AppResponse;            /* For system use */
  FCTLHANDLE    FirstHandle;            /* For system use */
  UINT32        FirstOffset;            /* For system use */
  UINT32        TotalLength;            /* Total length of access */
  UINT32        RngCnt;                 /* For system use only */
  UINT32        AsyHdr[4];              /* For system use */
}FCTLACCESSINFO;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use only */
  UINT8        *pMem;                   /* Pointer to original memory */
  UINT32        Length;                 /* Length of area */
  UINT8        *pLockMem;               /* Pointer to locked memory */
}FCTLLOCKMEMORY;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT8         Speed;                  /* Speed of transaction */
  UINT8         FirstSync;              /* Sync state of first packet */
  UINT8         IsoChannel;             /* Iso channel to send on */
  UINT32        IsoPayload;             /* Payload for transmission */
  UINT32        PktOffset;              /* Additional mem. offset per packet */
  UINT8         Append;                 /* Append or send parallel */
  UINT8         IsoNr;                  /* IsoNr we sent on (0xFF=AsyStream) */
  UINT32        DataLen;                /* Length of data to write */
  UINT8        *pData;                  /* Pointer to data area (locked!) */
}FCTLISOWRITE;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT32        ChannelsHigh;           /* Available channels (0...31=LSB) */
  UINT32        ChannelsLow;            /* Available channels (32..63=LSB) */
  UINT32        Bandwidth;              /* Available bandwidth */
}FCTLISORESOURCES;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT8         Node0;                  /* First node */
  UINT8         Node1;                  /* Second node */
  UINT32        GenerationCount;        /* Generation count */
  UINT8         MaxSpeed;               /* Maximum speed */
}FCTLMAXSPEED;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
}FCTLSNOOP;

typedef struct
{
  SYSHDR        SysHdr;                 /* For system use */
  UINT16        BusAddress;             /* Bus address */
  UINT8         NodeAddress;            /* Node address */
  char          NodeRequest[17];        /* String for node license request */
  char          HostRequest[22];        /* License for host */
  char          CardRequest[17];        /* License for card */
}FCTLLICENSEREQUEST;

typedef struct
{
  SYSHDR        SysHdr;
  UINT16        CmdWait;                /* R/W Timeout in ms for transactions */
  UINT32        Reserved1[8];
  UINT16        BusAddress;             /* R Actual bus address */
  UINT32        AsyRxPktCnt;            /* R Max. asynch. receive buffers */
  UINT32        AsyRxPktPayload;        /* R Size of asynch. receive buffers */
  UINT32        AsyTxPktCnt;            /* R Max. asynch. transmit buffers */
  UINT32        AsyTxPktPayload;        /* R Size of asynch. transmit buffers */
  UINT16        StoredAccessCnt;        /* R Number of external stored accesses */
  UINT8         PowerClass;             /* R Powerclass (in Phys SelfId) */
  UINT16        CfgRomLength;           /* R Length of CFGROM in quadlets */
  UINT16        IrmBmTaskPrio;          /* R Taskpriority Ir/Bmanager (0=none) */
  UINT16        IrqTaskPrio;            /* R Taskpriority of deferred handler */
  UINT8         IrqPrio;                /* R IRQ-Prio and upper 2 bits IRQ-Line */
  UINT32        SpecialFlags;           /* R Flags for special handling */
  UINT32        Reserved2[8];
}FCTLPARAMETER;

#endif
