////////////////////////////////////////////////////////////////////////////////
// Headerfile for camera stuff.
// intek, C. Kuehnel, 14.2.2001
////////////////////////////////////////////////////////////////////////////////

#ifndef CAMERA_H
#define CAMERA_H

#include "osutil.h"
#include "firebus.h"

// Partial scan video format is 7
#define PSFORMAT                        7

// Some offsets for our Configuration-ROM
#define HIGHOFFSET                      0xFFFF
#define SYSBASE                         0xF0000000
#define INITOFFSET                      0x00F00000
#define VENDORIDOFFSET                  0x40C
#define MODULEIDOFFSET                  0x410
#define ROOTOFFSET                      0x414
#define UNITOFFSET                      0x424
#define UNITDEPOFFSET                   0x0C
#define CMDBASEOFFSET                   0x04
#define VENDORLEAFOFFSET                0x08
#define MODELLEAFOFFSET                 0x0C
#define FORMATINQUIREOFFSET             0x100
#define FORMAT0MODEINQUIREOFFSET        0x180
#define FRAMERATEINQUIREOFFSET          0x200

// Offsets for CSR register
#define CSRMAXIMAGEINQ                  0x00
#define CSRUNITSIZEINQ                  0x04
#define CSRIMAGEPOSITION                0x08
#define CSRIMAGESIZE                    0x0C
#define CSRCOLORCODING                  0x10
#define CSRCOLORCODINGINQ               0x14
#define CSRVENDORCOLORCODINGINQ         0x24
#define CSRBYTEPERFRAME                 0x3C
#define CSRPACKETINQ                    0x40
#define CSRBYTEPERPACKET                0x44
#define CSRPACKETPERFRAME               0x48
#define CSRUNITPOSINQ                   0x4C
#define CSRVALUESETTING                 0x7C

// Offsets for commandregister
#define CCR_FRAMERATE                   0x600
#define CCR_VMODE                       0x604
#define CCR_VFORMAT                     0x608
#define CCR_CHNSPEED                    0x60C
#define CCR_ISOENABLE                   0x614
#define CCR_ONESHOT                     0x61C
#define CCR_TRGMODE                     0x830

// Offsets for feature registers
#define BASIC_INQ                       0x400
#define BRIGHTNESS_INQ                  0x500
#define AUTOEXPOSURE_INQ                0x504
#define SHARPNESS_INQ                   0x508
#define WHITEBAL_INQ                    0x50C
#define HUE_INQ                         0x510
#define SATURATION_INQ                  0x514
#define GAMMA_INQ                       0x518
#define SHUTTER_INQ                     0x51C
#define GAIN_INQ                        0x520
#define IRIS_INQ                        0x524
#define FOCUS_INQ                       0x528
#define TEMPERATURE_INQ                 0x52C
#define TRIGGER_INQ                     0x530
#define ZOOM_INQ                        0x580
#define PAN_INQ                         0x584
#define TILT_INQ                        0x588
#define OPTICALFILTER_INQ               0x58C

#define INQTOCTRLOFFSET                 0x300
#define INQTOABSOFFSET                  0x200

#define BINQF_ONESHOT                   0x00001000

// Enumeration for framerates
enum{ FR_1_875=0,FR_3_75,FR_7_5,FR_15,FR_30,FR_60,FR_120,FR_240,FR_LAST };

// Enumeration for displayed colors
enum{ CV_256=0,CV_512,CV_1024,CV_4096,CV_8192,CV_16384,CV_LAST };

// Enumeration for Triggermode
enum{ TM_INTERN=0,TM_EXTERN, TM_MODE15, TM_ONESHOT, TM_LAST };

// Enumeration for color modes
enum{ CM_Y8=0,CM_YUV411,CM_YUV422,CM_YUV444,CM_RGB8,CM_Y16,CM_RGB16,
      CM_SY16,CM_SRGB16,CM_RAW8,CM_RAW16,
      CM_Y8R=128,CM_Y8G,CM_Y8B,CM_Y12=132,CM_RGB12,CM_SY12,CM_SRGB12,CM_RAW12,CM_LAST=255 };  //NDE

// Enumeration for Bayer pattern
enum{ BP_RGGB=0, BP_GRBG, BP_BGGR, BP_GBRG, BP_LAST };

// Feature types
enum
{
  CFT_BRIGHTNESS=0,
  CFT_AUTOEXPOSURE,
  CFT_SHARPNESS,
  CFT_WHITEBAL,
  CFT_HUE,
  CFT_SATURATION,
  CFT_GAMMA,
  CFT_SHUTTER,
  CFT_GAIN,
  CFT_IRIS,
  CFT_FOCUS,
  CFT_TEMPERATURE,
  CFT_ZOOM,
  CFT_PAN,
  CFT_TILT,
  CFT_OPTICALFILTER,
  CFT_LAST
};

////////////////////////////////////////////////////////////////////////////////
// Frame info that camera object stores.
////////////////////////////////////////////////////////////////////////////////

typedef struct 
{
  FCTLFRMINFO   Info;
  UINT32        Number;
}CAMFRMINFO;

////////////////////////////////////////////////////////////////////////////////
// Type for a video set.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT8         VideoFormat;                    // DCAM Video Format
  UINT8         VideoMode;                      // DCAM Video Mode

  UINT16        XPos;                           // X Position
  UINT16        YPos;                           // Y Position
  UINT16        XSize;                          // Image XSize
  UINT16        YSize;                          // Image YSize
  UINT8         ColorFormat;                    // Colorformat (Zerobased)
  UINT8         FrameRate;                      // Framerate (Zerobased)
  UINT16        PktSize;                        // Actual packetsize

  UINT8         IsScalable;                     // If scalable (special!)
  UINT16        XMaxSize;                       // Max X size
  UINT16        YMaxSize;                       // Max Y size
  UINT16        XPosUnit;                       // Unit of position if scalable
  UINT16        YPosUnit;                       // Unit of position if scalable
  UINT16        XSizeUnit;                      // Unit of size if scalable
  UINT16        YSizeUnit;                      // Unit of size if scalable
  UINT32        ColorInquiry;                   // Colorformats (Bitfield)
  UINT32        VendorColorInquiry;             // Vendor Unique Colorformats (Bitfield)  //NDE
}VIDSET;

////////////////////////////////////////////////////////////////////////////////
// Type for format 7 work request.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT8         VideoMode;                      // Mode we are working
  UINT8         ColorFormat;                    // Colorformat
  UINT8         Speed;                          // Speed
  UINT16        XPos;                           // Image Position
  UINT16        YPos;
  UINT16        XSize;                          // Image Size
  UINT16        YSize;

  UINT16        PacketUnit;                     // Packetinformation
  UINT16        PacketMax;
}FORMAT7INFO;

////////////////////////////////////////////////////////////////////////////////
// Type for feature property on the bus.
////////////////////////////////////////////////////////////////////////////////

typedef union
{
  struct
  {
    UINT32      MaxValue : 12;          // Maximum Value
    UINT32      MinValue : 12;          // Minimum Value
    UINT32      Manual   : 1;           // Is manual controllable
    UINT32      Auto     : 1;           // Automode available
    UINT32      OnOff    : 1;           // Can be switched on/off
    UINT32      ReadOut  : 1;           // Can be read
    UINT32      OneShot  : 1;           // One shot auto set possible
    UINT32      Reserved : 1;
    UINT32      Absolute : 1;           // Absolute controllable
    UINT32      Present  : 1;           // Is present
  }AsBits;

  UINT32        AsUINT32;

}DCAMFEATINQ;

////////////////////////////////////////////////////////////////////////////////
// Type for feature set on the bus.
////////////////////////////////////////////////////////////////////////////////

typedef union
{
  struct
  {
    UINT32      Value    : 12;          // Real value
    UINT32      SubValue : 12;          // Subvalue
    UINT32      Auto     : 1;           // Automode available
    UINT32      IsOn     : 1;           // Is on or off
    UINT32      OneShot  : 1;           // One shot flag
    UINT32      Reserved : 3;
    UINT32      Absolute : 1;           // Absolute controllable
    UINT32      Present  : 1;           // Is present
  }AsBits;

  UINT32        AsUINT32;

}DCAMFEATSET;

////////////////////////////////////////////////////////////////////////////////
// Type for user controllable feature property.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT8       Type;                     // Type specifier

  UINT16      OnOff       : 1;          // Can be switched On or Off
  UINT16      IsOn        : 1;          // Is actual On or off
  UINT16      Manual      : 1;          // Is manual controllable
  UINT16      Auto        : 1;          // Has an automode
  UINT16      IsAuto      : 1;          // Actual state of automode
  UINT16      ReadOut     : 1;          // Can be read
  UINT16      OneShot     : 1;          // Can adjust this value one time
  UINT16      OneShotIsOn : 1;          // One shot flag is active
  UINT16      HasSubValue : 1;          // Has a subvalue

  UINT16      MinValue;                 // Minimum Value
  UINT16      MaxValue;                 // Maximum Value

  UINT16      Value;                    // Real value
  UINT16      SubValue;                 // Real Subvalue

}FEATPROP;

////////////////////////////////////////////////////////////////////////////////
// Type for user controllable feature value.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32      On        : 1;            // Switch feature On or Off
  UINT32      Auto      : 1;            // Set to auto mode
  UINT32      OneShot   : 1;            // Adjust one time automatically

  UINT32      Value     : 12;           // Value to set
  UINT32      SubValue  : 12;           // Subvalue if available

}FEATSET;

////////////////////////////////////////////////////////////////////////////////
// Type for format 7 descriptor.
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32        Offset;
  UINT32        MaxImageSize;
  UINT32        UnitSize;
  UINT32        UnitPos;
  UINT32        ColorCoding;
  UINT32        VendorColorCoding;  //NDE
}FORMAT7DSC;

////////////////////////////////////////////////////////////////////////////////
// Properties for a camera (camera specific properties).
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32        CmdRegBase;                 // Address of command reg base
  UINT32        BasicInq;                   // basic function inquiry
  UINT32        UnitDirOffset;              // Unit directory offset
  UINT32        UnitDepDirOffset;           // Unit dependant directory offset
  UINT32        VendorLeafOffset;           // Vendor leaf offset
  UINT32        ModelLeafOffset;            // Model leaf offset
  char          VendorName[256];            // 0 terminated Vendorname
  char          ModelName[256];             // 0 terminated Modelname
  UINT8         VideoFormatMask;            // Mask for videoformats
  UINT8         VideoModeMask[8];           // Mask for videomodes
  UINT8         FrameRateMask[8][8];        // Mask for framerate
  FORMAT7DSC    Format7Dsc[8];              // Format 7 descriptors
}CAMPROPS;

////////////////////////////////////////////////////////////////////////////////
// Parameters for camera (actual working parameters).
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  UINT32    VidSet;                     // Set number actual selected
  UINT8     VideoFormat;                // Actual videoformat
  UINT8     VideoMode;                  // Actual videomode
  UINT8     FrameRate;                  // Actual framerate
  UINT8     ColorFormat;                // Actual ColorFormat
  UINT16    XPos;                       // X-Offset
  UINT16    YPos;                       // Y-Offset
  UINT16    XSize;                      // X-Dimension
  UINT16    YSize;                      // Y-Dimension
  UINT16    PktSize;                    // Payload per packet
  UINT16    PktCnt;                     // Packets per frame

  UINT32    BufferCnt;                  // Number of frames to allocate
  UINT32    RealBufferCnt;              // Really allocated frames
  UINT8     DmaChn;                     // Dma channel used
  UINT8     IsoChn;                     // Real ISO channel in camera
  UINT8     IsoSpeed;                   // ISO speed
  UINT8     DmaIsoChnMsk;               // DMA ISO channel mask
  UINT8     DmaIsoChnVal;               // DMA ISO channel value
  UINT16    FirstMsk;                   // Low 16-Bit mask of first word
  UINT16    FirstVal;                   // Low 16-Bit value of first word
  UINT8     TriggerMode;                // Triggermode
  UINT8     Y16InversOrder;             // Inversmode for Y16
  UINT8     YIsRawMode;                 // Raw mode flag
  UINT8     BayerPattern;               // Used bayer pattern
  UINT16    PktOffset;                  // Offset for each packet
}CAMPARMS;

////////////////////////////////////////////////////////////////////////////////
// Class for basic camera.
////////////////////////////////////////////////////////////////////////////////

class CCamera : public CFireNode
{
protected:
  CAMPROPS      m_Props;                // Camera properties (fix)
  CAMPARMS      m_Parms;                // Camera parameters (changeable)
  UINT8         m_PropsRead;            // Flag for properties read
  UINT8         m_IsoActive;            // Iso active flag
  UINT8         m_IsoAllocated;         // Iso resources allocated
  void*         m_User;                 // Users argument

  CAMFRMINFO   *m_pFrmInfo;             // Pointer to our frameinfos
  UINT32        m_FrmInfoReadIx;        // Read index
  UINT32        m_FrmInfoWriteIx;       // Write index
  UINT32        m_FrmInfoSize;          // Size of our queue
  UINT8         m_FrmDisabled;          // Frame queue is disabled (for shutdown)

  FCTLLOCKMEMORY  *m_pUserFrames;       // User allocated frames
  UINT32        m_UserFrameCnt;
  UINT8         m_UseUserFrames;        // Flag for using userframes
  
  POSSEMAPHORE  m_pFrmSemaphore;        // Semaphore for frame infos
  POSSEMAPHORE  m_pFrmAccessSemaphore;  // Access semaphore

  UINT32        m_TotalFrmCnt;          // Total frames seen
  UINT32        m_DiscardedFrmCnt;      // Discarded frames

  UINT32        m_SetCnt;               // Number of available sets
  UINT32        m_FirstScalableSet;     // First scalable set

  virtual void  CountSets();            // Count available sets
  virtual UINT8 FindHighestBit(UINT8 Val);  // Find highest set bit
  virtual UINT8 FindHighestBit(UINT32 Val);  // Find highest set bit

  virtual UINT8 ReadLeaf(UINT32 Offset,UINT32 MaxSize,char *pStr); // Read leaf
  virtual UINT8 GetBasics();            // Get basic address stuff
  virtual UINT8 GetFormats();           // Get available formats
  virtual UINT8 GetModes();             // Get available modes
  virtual UINT8 GetFrameRates();        // Get available frame rates
  virtual UINT8 GetFormat7();           // Get format 7 properties

  virtual UINT8 GetProperties();        // Get all above properties
  virtual void  AdjustParameters();     // Adjust parameters

  virtual UINT32 GetFeatureOffset(UINT8 Type,UINT8 *pHasSubValue);

  virtual UINT8 CreateFrmQueue(UINT32 Entries); // Init our frame queue
  virtual void  FreeFrmQueue();         // Free our frame queue

  virtual UINT8 IsStandardMode(VIDSET *pSet);   // Check if it's standard mode
  virtual UINT8 MatchPartialScan(VIDSET *pSet); // Match partial scan
  virtual UINT8 SelectPartialScan(VIDSET *pSet);// Select partial scan
  virtual UINT8 AllocateUserFrames();
  virtual void  FreeUserFrames();

// FOR SYSTEM USE ONLY!!!
public:
  CCamera();
  virtual ~CCamera();

  virtual UINT8  OnFrame(FCTLFRMINFO *pInfo);   // On frame receiption

  virtual UINT32 FrameSize();                   // Retrieve actual size of a frame
  virtual UINT32 FrameSize(UINT16 XSize,UINT16 YSize,UINT8 ColorFormat);

  virtual void   OnBusResetEnd();               // Functions from baseclass

  virtual UINT8  OnStartup();                   // Startup function
  virtual void   OnShutdown();                  // Shutdown function

  virtual void   SetIsoChannel(UINT8 Channel);  // Set Iso channel
  virtual UINT8  IsoAllocated();                // Return TRUE if Iso resources

// User functions
public:
  virtual UINT32 GetFrameMemorySize();          // Get memory size of all frames
  virtual UINT16 GetIsoPayload();               // Get iso payload
  virtual void   UseUserFrames(UINT8 Enable)    { m_UseUserFrames=Enable; }

  virtual UINT8  StartCamera();                 // Only start camera (no DMA)
  virtual UINT8  StopCamera();                  // Only stop camera (no DMA)
  virtual UINT8  StartIso();                    // Start image capture
  virtual UINT8  StopIso();                     // Stop image capture
  virtual UINT8  DoOneShot();                   // Do one shot (if supported)
  virtual UINT8  IsoActive()                    { return m_IsoActive; }

  virtual void   GetFrameCounter(UINT32 *pTotal,UINT32 *pDiscarded);
  virtual void   ClearFrameCounter();           // Clear counter

  virtual UINT8  GetFrame(FCTLFRMINFO *pFrame,UINT32 TimeouInMs=TIMEINFINITE,UINT32 *pNumber=NULL);
  virtual void   ReturnFrame(FCTLFRMINFO *pFrame);                // Return a frame
  virtual UINT32 DiscardFrames();               // Discard all frames
  virtual UINT32 FrameQueueSize()               { return m_FrmInfoSize; }

  virtual char*  GetColorFormatString(UINT8 ColorFormat);
  virtual char*  GetVendorColorFormatString(UINT8 ColorFormat); //NDE
  virtual char*  GetFrameRateString(UINT8 FrameRate);
  virtual char*  GetFeatureString(UINT8 Type);
  virtual char*  GetTriggerModeString(UINT8 TriggerMode);
  virtual char*  GetSpeedString(UINT8 Speed);
  virtual char*  GetBayerPatternString(UINT8 Pattern); // Get string for pattern

  virtual void   SetUser(void *pArg);           // Set user argument
  virtual void*  GetUser();                     // Get user argument

  virtual UINT32 GetVidSetCount();              // Get number of video sets
  virtual UINT8  GetVidSetValues(UINT32 Nr,VIDSET *pSet); // Get values
  virtual UINT32 GetVidSet();                   // Get selected VidSet
  virtual UINT8  SelectVidSetByNr(UINT32 Nr);   // Select a set by index
  virtual UINT8  SelectVidSetByValue(VIDSET *pSet);     // Select set by values

  virtual UINT8  AcquirePacketInfo(FORMAT7INFO *pInfo);

  virtual CAMPROPS* pProps()                    { return &m_Props; }
  virtual CAMPARMS* pParms()                    { return &m_Parms; }

  virtual UINT8  GetFeature(UINT8 Type,FEATPROP *pFeatProp);  // Get feature props
  virtual UINT8  SetFeature(UINT8 Type,FEATSET *pFeatSet);    // Set feature props
  virtual UINT8  SetFeature(UINT8 Type,FEATPROP *pFeatProp);    // Set feature props
};

#endif
