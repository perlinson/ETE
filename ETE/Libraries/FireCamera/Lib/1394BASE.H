/*******************************************************************************
  Headerfile for declaring 1394 basic stuff.
  intek, c. kuehnel, 11.10.2000
*******************************************************************************/

/*******************************************************************************
  REMARKS:
  ========
  ALL STRUCTURES IN THIS HEADERFILE MUST BE BYTEPACKED!
*******************************************************************************/

#ifndef BASE1394_H
#define BASE1394_H

#include <basetype.h>

#define MAXASYPAYLOAD                   4096    /* Max. supported asy. size */
#define MAXISOPAYLOAD                   8192    /* Max. supported iso. size */

#define TCODE_WRREQQUAD                 0
#define TCODE_WRREQBLK                  1
#define TCODE_WRRES                     2       /* Pending write response */
#define TCODE_RDREQQUAD                 4
#define TCODE_RDREQBLK                  5
#define TCODE_RDRESQUAD                 6
#define TCODE_RDRESBLK                  7
#define TCODE_CYCLESTART                8
#define TCODE_LOCKREQ                   9
#define TCODE_STREAM                    10      /* Isochronous stream */
#define TCODE_LOCKRES                   11      /* Pending lock response */
#define TCODE_PRIVATE                   14      /* Private used in some impl. */

#define RCODE_COMPLETE                  0       /* Response codes (complete) */
#define RCODE_CONFLICT_ERROR            4
#define RCODE_DATA_ERROR                5
#define RCODE_TYPE_ERROR                6
#define RCODE_ADDRESS_ERROR             7
#define RCODE_TIMED_OUT                 15

#define ACK_MISSING                     0       /* Missing ACK */
#define ACK_COMPLETE                    1
#define ACK_PENDING                     2
#define ACK_BUSY_X                      4
#define ACK_BUSY_A                      5
#define ACK_BUSY_B                      6
#define ACK_TARDY                       11
#define ACK_CONFLICT_ERROR              12
#define ACK_DATA_ERROR                  13
#define ACK_TYPE_ERROR                  14
#define ACK_ADDR_ERROR                  15

#define LOCK_MASK_SWAP                  1
#define LOCK_COMPARE_SWAP               2
#define LOCK_FETCH_ADD                  3
#define LOCK_LITTLE_ADD                 4
#define LOCK_BOUNDED_ADD                5
#define LOCK_WRAP_ADD                   6
#define LOCK_VENDOR_DEPENDANT           7

#define PC_NOT_NEED_NOT_REPEAT          0
#define PC_SELF_POWER_PROVIDE_15W       1
#define PC_SELF_POWER_PROVIDE_30W       2
#define PC_SELF_POWER_PROVIDE_45W       3
#define PC_MAYBE_POWERED_UPTO_1W        4
#define PC_IS_POWERED_UPTO_1W_NEEDS_2W  5
#define PC_IS_POWERED_UPTO_1W_NEEDS_5W  6
#define PC_IS_POWERED_UPTO_1W_NEEDS_9W  7

#define SPEED100                        0x0     /* Speedcode 100MBit */
#define SPEED200                        0x1     /* Speedcode 200MBit */
#define SPEED400                        0x2     /* Speedcode 400MBit */
#define SPEED800                        0x3     /* Speedcode 800MBit */

#define MAXBANDWIDTH                   0x1333

#define PHY_PACKET_ID_CONFIGURATION     0
#define PHY_PACKET_ID_LINK_ON           1
#define PHY_PACKET_ID_SELF_ID           2

#define SELF_ID_CONNECTED_TO_CHILD      3
#define SELF_ID_CONNECTED_TO_PARENT     2
#define SELF_ID_NOT_CONNECTED           1
#define SELF_ID_NOT_PRESENT             0

#define INITIAL_REGISTER_SPACE_HI       0xFFFF
#define INITIAL_REGISTER_SPACE_LO       0xF0000000

#define STATE_CLEAR_LOCATION            0x000
#define STATE_SET_LOCATION              0x004
#define NODE_IDS_LOCATION               0x008
#define RESET_START_LOCATION            0x00C
#define SPLIT_TIMEOUT_HI_LOCATION       0x018
#define SPLIT_TIMEOUT_LO_LOCATION       0x01C
#define INTERRUPT_TARGET_LOCATION       0x050
#define INTERRUPT_MASK_LOCATION         0x054
#define CYCLE_TIME_LOCATION             0x200
#define BUS_TIME_LOCATION               0x204
#define POWER_FAIL_IMMINENT_LOCATION    0x208
#define POWER_SOURCE_LOCATION           0x20C
#define BUSY_TIMEOUT_LOCATION           0x210
#define BUS_MANAGER_ID_LOCATION         0x21C
#define BANDWIDTH_AVAILABLE_LOCATION    0x220
#define CHANNELS_AVAILABLE_HI_LOCATION  0x224
#define CHANNELS_AVAILABLE_LO_LOCATION  0x228
#define NETWORK_CHANNELS_LOCATION       0x234
#define CONFIG_ROM_LOCATION             0x400
#define TOPOLOGY_MAP_LOCATION           0x1000
#define SPEED_MAP_LOCATION              0x2000

#define BUS_OPTIONS                     8

#define BUS_CHARACTERISTIC_IRMC         0x80000000
#define BUS_CHARACTERISTIC_CMC          0x40000000
#define BUS_CHARACTERISTIC_ISC          0x20000000
#define BUS_CHARACTERISTIC_BMC          0x10000000

#define CFGROMSIZE                      0x400
#define CFGROMBASEHIGH                  INITIAL_REGISTER_SPACE_HI
#define CFGROMBASELOW                   (INITIAL_REGISTER_SPACE_LO+CONFIG_ROM_LOCATION)

#define VENDORIDPHILIPS                 0x00026C

#define SWAP(n)   ((((UINT32)(n)&(UINT32)0xFF)<<24)| \
                  (((UINT32)(n)&(UINT32)0xFF00)<<8)| \
                  (((UINT32)(n)&(UINT32)0xFF0000)>>8)| \
                  (((UINT32)(n)&(UINT32)0xFF000000)>>24))

#define SWAP16(n) ((((n)&0xFF)<<8)|((n)>>8))

#define BUSBROADCAST    0x3FF           /* Bus broadcast address */
#define NODEBROADCAST   0x3F            /* Node broadcast address */

/*******************************************************************************
  Firewire busaddress.
*******************************************************************************/

typedef union
{
  struct
  {
    UINT16      Node : 6;
    UINT16      Bus  : 10;
  }AsBits;
  UINT16        AsUINT16;
}BUSADDRESS;

/*******************************************************************************
  1 quadlet packetheader.
                  16                    2        6          4        4
   ----------------------------------- ---- ------------ -------- --------
  |            Datalength             |Tag |  Channel   | TCode  |  Sync  |
   ----------------------------------- -------- -------- -------- --------
*******************************************************************************/

typedef struct
{
  UINT32        Sync       : 4;
  UINT32        TCode      : 4;
  UINT32        Channel    : 6;
  UINT32        Tag        : 2;
  UINT32        DataLength : 16;
}PKTHDR1;

/*******************************************************************************
  3 quadlet packetheader:
      4        4        4        4        4        4        4        4
   -------- -------- -------- -------- -------- -------- -------- --------
  |      Destination Address          | TLbl   | Retry  | TCode  |  Pri   |
   -------- -------- -------- -------- -------- -------- -------- --------
  |       Source Address              |  Destination Offset High / RCode  |
   -------- -------- -------- -------- -------- -------- -------- --------
  |                         Destination Offset Low                        |
   -------- -------- -------- -------- -------- -------- -------- --------
*******************************************************************************/

typedef struct
{
  UINT16        Pri             : 4;      /* must be zero for cable operation */
  UINT16        TCode           : 4;
  UINT16        RetryCode       : 2;
  UINT16        TLabel          : 6;

  BUSADDRESS    Dst;

  union                                   /* 2nd quadlet */
  {
    struct
    {
      UINT16    Reserved        : 12;
      UINT16    Code            : 4;
    }Response;
    UINT16      DstOffsHigh;
  }u1;

  BUSADDRESS    Src;

  UINT32        DstOffsLow;               /* 3rd quadlet */

}PKTHDR3;

/*******************************************************************************
  4 quadlet packetheader:
      4        4        4        4        4        4        4        4
   -------- -------- -------- -------- -------- -------- -------- --------
  |      Destination Address          | TLbl   | Retry  | TCode  |  Pri   |
   -------- -------- -------- -------- -------- -------- -------- --------
  |       Source Address              |  Destination Offset High / RCode  |
   -------- -------- -------- -------- -------- -------- -------- --------
  |                         Destination Offset Low                        |
   -------- -------- -------- -------- -------- -------- -------- --------
  |        (DataLength)       One quadlet ...     (Extended TCode)        |
   -------- -------- -------- -------- -------- -------- -------- --------
*******************************************************************************/

typedef struct
{
  UINT16        Pri             : 4;    /* must be zero for cableoperation */
  UINT16        TCode           : 4;
  UINT16        RetryCode       : 2;
  UINT16        TLabel          : 6;

  BUSADDRESS    Dst;

  union
  {                                     /* 2nd quadlet */
    struct
    {
      UINT16    Reserved        : 12;
      UINT16    Code            : 4;
    }Response;
    UINT16      DstOffsHigh;
  }u1;

  BUSADDRESS    Src;

  UINT32        DstOffsLow;             /* 3rd quadlet */

  union                                 /* 4th quadlet */
  {
    struct
    {
      UINT32    ExtendedTCode   : 16;
      UINT32    DataLength      : 16;
    }Block;
    UINT32      Data;
  }u2;

}PKTHDR4;

/*******************************************************************************
  First SelfId packet.
*******************************************************************************/

typedef union
{
  struct
  {
    UINT32      More            : 1;
    UINT32      InitiatedReset  : 1;
    UINT32      Port2State      : 2;
    UINT32      Port1State      : 2;
    UINT32      Port0State      : 2;

    UINT32      PowerClass      : 3;
    UINT32      Contender       : 1;
    UINT32      Reserved        : 2;
    UINT32      Speed           : 2;

    UINT32      GapCount        : 6;
    UINT32      LinkActive      : 1;
    UINT32      SelfIdMore      : 1;

    UINT32      PhyId           : 6;
    UINT32      PktType         : 2;
  }AsBits;

  UINT32        AsUINT32;
}PKTSELFID;

/*******************************************************************************
  More SelfId packet.
*******************************************************************************/

typedef union
{
  struct
  {
    UINT32      More            : 1;
    UINT32      Reserved0       : 1;
    UINT32      Port7State      : 2;
    UINT32      Port6State      : 2;
    UINT32      Port5State      : 2;

    UINT32      Port4State      : 2;
    UINT32      Port3State      : 2;
    UINT32      Port2State      : 2;
    UINT32      Port1State      : 2;

    UINT32      Port0State      : 2;
    UINT32      Reserved1       : 2;
    UINT32      Number          : 3;
    UINT32      SelfIdMore      : 1;

    UINT32      PhyId           : 6;
    UINT32      PktType         : 2;
  }AsBits;

  UINT32        AsUINT32;
}PKTSELFIDMORE;

/*******************************************************************************
  Phy packet.
*******************************************************************************/

typedef union
{
  struct
  {
    UINT32      Zero       : 16;
    UINT32      GapCnt     : 6;
    UINT32      T          : 1;
    UINT32      R          : 1;
    UINT32      RootId     : 6;
    UINT32      Fct        : 2;
  }AsBits;

  UINT32        AsUINT32;

}PKTPHYCFG;

/*******************************************************************************
  Address in device.
*******************************************************************************/

typedef struct
{
  UINT16        High;
  UINT32        Low;
}MEMOFFSET;

#endif
