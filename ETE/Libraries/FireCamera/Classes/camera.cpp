////////////////////////////////////////////////////////////////////////////////
// Module for camerastuff.
// intek, C. Kuehnel, 14.2.2001
////////////////////////////////////////////////////////////////////////////////

#include <camera.h>
#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
#pragma comment(lib, "./Libraries/FireCamera/lib/Stdcall/debug/FCTLMAIN.lib")
#else
#pragma comment(lib, "./Libraries/FireCamera/lib/Stdcall/release/FCTLMAIN.lib")
#endif

#define ISOFREQUENCY            8000.0
#define DEFFRMBUFCNT            16

typedef struct
{
  UINT16        XSize;
  UINT16        YSize;
  UINT8         ColorFormat;
  UINT16        PktSize[FR_LAST];         // Number of bytes per packet
}CAMSET;

// Strings for cameraspeed
char* SpeedStr[] =
{
  "100","200","400","800"
};

// Names for ColorFormat
char* ColorFormatStr[] =
{
  "Y-8","YUV-411","YUV-422","YUV-444","RGB-8","Y-16","RGB-16","SY-16", "SRGB-16",
  "RAW-8", "RAW-16",""
};

// Names for VendorColorFormat (AVT specific, 32 strings)
char* VendorColorFormatStr[] =
{
  "Y-8 Red","Y-8 Green","Y-8 Blue",      "?",
     "Y-12",   "RGB-12",   "SY-12","SRGB-12",
   "RAW-12",        "?",       "?",      "?",
        "?",        "?",       "?",      "?",
        "?",        "?",       "?",      "?",
        "?",        "?",       "?",      "?",
        "?",        "?",       "?",      "?",
        "?",        "?",       "?",      "?"
};

// Names for framerates
char* FrameRateStr[] =
{
  "1.875","3.75","7.5","15","30","60","120","240",""
};

// Frametime depending on frame rate
double FrameTime[] =
{
  1.0/1.875, 1.0/3.75, 1.0/7.5, 1.0/15.0, 1.0/30.0, 1.0/60.0, 1.0, 1.0, 1.0
};

// Strings for Triggermode
char* TriggerModeStr[] =
{
  "intern","extern", "mode15", "oneshot"
};

// Strings for BayerPatterns
char* BayerPatternStr[] =
{
  "R-G G-B",
  "G-R B-G",
  "B-G G-R",
  "G-B R-G"
};

char* FeatureString[] =
{
  "Brightness",
  "Autoexposure",
  "Sharpness",
  "Whitebal.-V (Sub-U)",
  "Hue",
  "Saturation",
  "Gamma",
  "Shutter",
  "Gain",
  "Iris",
  "Focus",
  "Temperature",
  "Zoom",
  "Pan",
  "Tilt",
  "Optical filter"
};

// Datasettings for Format 0-2, Mode 0-7, Framerate 0-240fps
CAMSET CamSets[3][8]=
{
  // Format0             1.875   3.75    7.5     15     30     60    120    240
   160,   120,CM_YUV444,     0,    30,    60,   120,   240,   480,   960,  1920,
   320,   240,CM_YUV422,    40,    80,   160,   320,   640,  1280,  2560,  5120,
   640,   480,CM_YUV411,   120,   240,   480,   960,  1920,  3840,  7680, 15360,
   640,   480,CM_YUV422,   160,   320,   640,  1280,  2560,  5120, 10240, 20480,
   640,   480,  CM_RGB8,   240,   480,   960,  1920,  3840,  7680, 15360, 30720,
   640,   480,    CM_Y8,    80,   160,   320,   640,  1280,  2560,  5120, 10240,
   640,   480,   CM_Y16,   160,   320,   640,  1280,  2560,  5120, 10240, 20480,
     0,     0,        0,     0,     0,     0,     0,     0,     0,     0,     0,

  // Format1
   800,   600,CM_YUV422,     0,   500,  1000,  2000,  4000,  8000, 16000, 32000,
   800,   600,  CM_RGB8,     0,     0,  1500,  3000,  6000, 12000, 24000,     0,
   800,   600,    CM_Y8,     0,     0,   500,  1000,  2000,  4000,  8000, 16000,
  1024,   768,CM_YUV422,   384,   768,  1536,  3072,  6144, 12288, 24576,     0,
  1024,   768,  CM_RGB8,   576,  1152,  2304,  4608,  9216, 18432,     0,     0,
  1024,   768,    CM_Y8,   192,   384,   768,  1536,  3072,  6144, 12288, 24576,
   800,   600,   CM_Y16,     0,   500,  1000,  2000,  4000,  8000, 16000, 32000,
  1024,   768,   CM_Y16,   384,   768,  1536,  3072,  6144, 12288, 24576,     0,

  // Format2
  1280,   960,CM_YUV422,   640,  1280,  2560,  5120, 10240, 20480,     0,     0,
  1280,   960,  CM_RGB8,   960,  1920,  3840,  7680, 15360, 30720,     0,     0,
  1280,   960,    CM_Y8,   320,   640,  1280,  2560,  5120, 10240, 20480,     0,
  1600,  1200,CM_YUV422,  1000,  2000,  4000,  8000, 16000, 32000,     0,     0,
  1600,  1200,  CM_RGB8,  1500,  3000,  6000, 12000, 24000,     0,     0,     0,
  1600,  1200,    CM_Y8,   500,  1000,  2000,  4000,  8000, 16000, 32000,     0,
  1280,   960,   CM_Y16,   640,  1280,  2560,  5120, 10240, 20480,     0,     0,
  1600,  1200,   CM_Y16,  1000,  2000,  4000,  8000, 16000, 32000,     0,     0
};

////////////////////////////////////////////////////////////////////////////////
// Routines for CCamera - Constructor.
////////////////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
  memset((void*)&m_Parms,0,sizeof(CAMPARMS));
  memset((void*)&m_Props,0,sizeof(CAMPROPS));

  m_PropsRead=0;
  m_IsoAllocated=FALSE;

  // Set properties to default camera (it's SX-900)
  m_Props.CmdRegBase=0;
  m_Props.UnitDirOffset=0;
  m_Props.UnitDepDirOffset=0;
  m_Props.VendorLeafOffset=0;
  m_Props.ModelLeafOffset=0;
  strcpy(m_Props.VendorName,"Vendor");
  strcpy(m_Props.ModelName, "Model");

  m_Props.VideoFormatMask = 0xFF;
  memset(m_Props.VideoModeMask,sizeof(m_Props.VideoModeMask),0xFF);
  memset(m_Props.FrameRateMask,sizeof(m_Props.FrameRateMask),0x78);

  m_Parms.VidSet          = 0;
  m_Parms.VideoFormat     = 0;
  m_Parms.VideoMode       = 0;
  m_Parms.FrameRate       = FR_7_5;
  m_Parms.ColorFormat     = CM_Y8;
  m_Parms.XPos            = 0;
  m_Parms.YPos            = 0;
  m_Parms.XSize           = 1280;
  m_Parms.YSize           = 960;
  m_Parms.PktSize         = 1280;
  m_Parms.PktCnt          = 960;
  m_Parms.BufferCnt       = DEFFRMBUFCNT;
  m_Parms.RealBufferCnt   = 0;
  m_Parms.DmaIsoChnMsk    = 0x3F;
  m_Parms.DmaIsoChnVal    = 0;
  m_Parms.FirstMsk        = 0x000F;
  m_Parms.FirstVal        = 0x0001;
  m_Parms.TriggerMode     = TM_INTERN;
  m_Parms.Y16InversOrder  = FALSE;
  m_Parms.YIsRawMode      = 0;
  m_Parms.BayerPattern    = 0;
  m_Parms.PktOffset       = 0;

  m_IsoActive=FALSE;
  m_User=NULL;
  m_pFrmInfo=NULL;
  m_FrmInfoSize=0;
  m_pUserFrames=NULL;
  m_UseUserFrames=FALSE;

  m_pFrmAccessSemaphore=OSSemaphoreCreate(1);
  m_pFrmSemaphore=OSSemaphoreCreate(0);
}

////////////////////////////////////////////////////////////////////////////////
// Destructor.

CCamera::~CCamera()
{
  // Last chance to stop (if removed from bus Shutdown is NOT called)
  StopIso();

  // Kill this one (if it has been forgotten)
  if(m_pFrmInfo)
  {
    delete[] m_pFrmInfo;
    m_pFrmInfo=NULL;
  }

  // Remove OS resources
  OSSemaphoreDelete(m_pFrmAccessSemaphore);
  OSSemaphoreDelete(m_pFrmSemaphore);
}

////////////////////////////////////////////////////////////////////////////////
// Read leaf.

UINT8 CCamera::ReadLeaf(UINT32 Offset,UINT32 MaxSize,char *pStr)
{
  UINT32 Length,i,val;

  // Get leaf length in quads
  if(!ReadQuad(HIGHOFFSET,Offset,&Length))
   return FALSE;

  // Calculate string length in quads (Leaf length means complete leaf-LenCrc)
  Length=HIUINT16(Length)-2;
  if(Length*4>=MaxSize)
  {
    // Dummy string
    strcpy_s(pStr, strlen("..."), "...");
  }
  else
  {
    for(i=0;i<Length;i++)
    {
      if(!ReadQuad(HIGHOFFSET,Offset+0xC+i*4,&val))
       return FALSE;
      val=SWAP(val);
      memcpy(&pStr[i*4],(UINT8*)&val,sizeof(UINT32));
    }
  }
  pStr[Length*4]=0;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Get basic address settings.

UINT8 CCamera::GetBasics()
{
  UINT32 Offset,Addr;
  UINT32 RootHdr,Length,Val,i;

  // Read ROOT header and go through all entries to find unit offset
  if(!ReadQuad(HIGHOFFSET,SYSBASE+ROOTOFFSET,&RootHdr))
   return FALSE;

  Addr=0;
  Length=RootHdr>>16;
  for(i=0;i<Length;i++)
  {
    if(!ReadQuad(HIGHOFFSET,SYSBASE+ROOTOFFSET+(i+1)*4,&Val))
     return FALSE;

    // See if we found unit directory offset
    if((Val>>24)==0xD1)
    {
      Addr=SYSBASE+ROOTOFFSET+(i+1)*4+4*(Val&0xFFFFFF);
      break;
    }
  }
  if(!Addr)
   return FALSE;

  m_Props.UnitDirOffset=Addr;
  Addr=m_Props.UnitDirOffset+UNITDEPOFFSET;

  // Read Dependant offset
  if(!ReadQuad(HIGHOFFSET,Addr,&Offset))
   return FALSE;
  m_Props.UnitDepDirOffset=Addr+(Offset&0x00FFFFFF)*4;

  // Get length of unit dependant directory
  if(!ReadQuad(HIGHOFFSET,m_Props.UnitDepDirOffset,&Length))
   return FALSE;

  Length>>=16;
  Addr=m_Props.UnitDepDirOffset+4;
  m_Props.CmdRegBase=m_Props.VendorLeafOffset=m_Props.ModelLeafOffset=0;
  while(Length)
  {
    if(!ReadQuad(HIGHOFFSET,Addr,&Offset))
     return FALSE;

    if((Offset>>24)==0x40)
     m_Props.CmdRegBase=SYSBASE+(Offset&0x00FFFFFF)*4;

    if((Offset>>24)==0x81)
     m_Props.VendorLeafOffset=Addr+(Offset&0x00FFFFFF)*4;

    if((Offset>>24)==0x82)
     m_Props.ModelLeafOffset=Addr+(Offset&0x00FFFFFF)*4;

    // Did we read all registers?
    if(m_Props.CmdRegBase && m_Props.VendorLeafOffset && m_Props.ModelLeafOffset)
     break;

    Addr+=4;
    Length--;
  }

  // Read Vendorname
  if(!ReadLeaf(m_Props.VendorLeafOffset,sizeof(m_Props.VendorName),
               m_Props.VendorName))
   return FALSE;

  // Read Modelname
  if(!ReadLeaf(m_Props.ModelLeafOffset,sizeof(m_Props.ModelName),
               m_Props.ModelName))
   return FALSE;

  // Read basic inquiry
  if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+BASIC_INQ,&Val))
   return FALSE;
  m_Props.BasicInq=Val;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Get available formats.

UINT8 CCamera::GetFormats()
{
  UINT32 Mask;

  // Read available formats
  if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+FORMATINQUIREOFFSET,&Mask))
   return FALSE;
  m_Props.VideoFormatMask=(UINT8)(Mask>>24);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Get available modes.
// On error while reading a register we disable the appropriate mode.

UINT8 CCamera::GetModes()
{
  UINT32 i,Mask;

  // Read available modes
  for(i=0;i<8;i++)
  {
    if(m_Props.VideoFormatMask&(0x80>>i))
    {
      if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+FORMAT0MODEINQUIREOFFSET+4*i,
                   &Mask))
       Mask=0;
      m_Props.VideoModeMask[i]=(UINT8)(Mask>>24);
    }
    else
    {
      m_Props.VideoModeMask[i]=0;
    }
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Get available frame rates (Mode 7 only allows framerate 0 which is none).
// On error we disable all framesrates.

UINT8 CCamera::GetFrameRates()
{
  UINT32 Format,Mode,Mask,i;

  // Clear all masks
  memset(m_Props.FrameRateMask,0,sizeof(m_Props.FrameRateMask));

  // Go through all formats (0-7)
  for(Format=0;Format<7;Format++)
   if(m_Props.VideoFormatMask&(0x80>>Format))
   {
     for(Mode=0;Mode<8;Mode++)
      if(m_Props.VideoModeMask[Format]&(0x80>>Mode))
      {
        if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+
            FRAMERATEINQUIREOFFSET+(Format*8+Mode)*4,&Mask))
         Mask=0;
        m_Props.FrameRateMask[Format][Mode]=(UINT8)(Mask>>24);
      }
      else
      {
        m_Props.FrameRateMask[Format][Mode]=0;
      }
   }

  // Reset framerates for format 7 (no meaning)
  for(i=0;i<8;i++)
   m_Props.FrameRateMask[7][i]=0x80;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Read all properties for format 7.

UINT8 CCamera::GetFormat7()
{
  UINT32 val,Mode,Offset;
  UINT8  Result;

  // Read all stuff for format 7
  if(m_Props.VideoFormatMask&0x01)
  {
    for(Mode=0;Mode<8;Mode++)
    {
      if(m_Props.VideoModeMask[7]&(0x80>>Mode))
      {
        // Get register offset
        if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+
                     FRAMERATEINQUIREOFFSET+(7*8+Mode)*4,&Offset))
         return FALSE;

        m_Props.Format7Dsc[Mode].Offset=Offset*4;

        // Read MaxImageSize
        if(!ReadQuad(HIGHOFFSET,SYSBASE+Offset*4+CSRMAXIMAGEINQ,&val))
         return FALSE;
        m_Props.Format7Dsc[Mode].MaxImageSize=val;

        // Read Color coding
        if(!ReadQuad(HIGHOFFSET,SYSBASE+Offset*4+CSRCOLORCODINGINQ,&val))
         return FALSE;
        m_Props.Format7Dsc[Mode].ColorCoding=val;

        // Read Vendor Colorcoding //NDE
        if(!ReadQuad(HIGHOFFSET,SYSBASE+Offset*4+CSRVENDORCOLORCODINGINQ,&val))
         return FALSE;
        m_Props.Format7Dsc[Mode].VendorColorCoding=val;

        // Read Unit for Size
        if(!ReadQuad(HIGHOFFSET,SYSBASE+Offset*4+CSRUNITSIZEINQ,&val))
         return FALSE;
        m_Props.Format7Dsc[Mode].UnitPos=m_Props.Format7Dsc[Mode].UnitSize=val;

        // Read Unit for position
        Result=ReadQuad(HIGHOFFSET,SYSBASE+Offset*4+CSRUNITPOSINQ,&val);

        // If not present or zero we use units of size (for position)
        if(!Result || ((val&0xFFFF0000)==0))
         val|=m_Props.Format7Dsc[Mode].UnitPos&0xFFFF0000;

        if(!Result || ((val&0xFFFF)==0))
         val|=m_Props.Format7Dsc[Mode].UnitPos&0xFFFF;

        m_Props.Format7Dsc[Mode].UnitPos=val;
      }
    }
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Count our available sets.

void CCamera::CountSets()
{
  UINT32 Format,Mode,FrameRate;

  m_SetCnt=0;
  m_FirstScalableSet=(UINT32)-1;
  for(Format=0;Format<8;Format++)
  {
    if(m_Props.VideoFormatMask&(0x80>>Format))
    {
      for(Mode=0;Mode<8;Mode++)
      {
        if(m_Props.VideoModeMask[Format]&(0x80>>Mode))
        {
          for(FrameRate=0;FrameRate<8;FrameRate++)
          {
            if(m_Props.FrameRateMask[Format][Mode]&(0x80>>FrameRate))
            {
              if((Format==PSFORMAT) && (m_FirstScalableSet==(UINT32)-1))
               m_FirstScalableSet=m_SetCnt;
              m_SetCnt++;
            }
          }
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Get all properties for our camera.

UINT8 CCamera::GetProperties()
{
  // Clear all settings
  memset(&m_Props,0,sizeof(CAMPROPS));

  if(!GetBasics())
   return FALSE;

  if(!GetFormats())
   return FALSE;

  if(!GetModes())
   return FALSE;

  if(!GetFrameRates())
   return FALSE;

  if(!GetFormat7())
   return FALSE;

  // Count our available settings
  CountSets();

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Find highest bit set in a byte.

UINT8 CCamera::FindHighestBit(UINT8 Val)
{
  UINT8 Mask,i;

  for(i=0,Mask=0x80;Mask;i++,Mask>>=1)
   if(Val&Mask)
    return i;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Find highest bit set in a UINT32.

UINT8 CCamera::FindHighestBit(UINT32 Val)
{
  UINT8 i;
  UINT32 Mask;

  for(i=0,Mask=0x80000000;Mask;i++,Mask>>=1)
   if(Val&Mask)
    return i;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Adjust parameters to valid values so camera can start immediately.
// Cameras properties must be valid.

void CCamera::AdjustParameters()
{
  CAMSET *pSet;

  m_Parms.VidSet=0;

  m_Parms.VideoFormat=FindHighestBit(m_Props.VideoFormatMask);
  m_Parms.VideoMode=FindHighestBit(m_Props.VideoModeMask[m_Parms.VideoFormat]);

  m_Parms.FrameRate=FindHighestBit(m_Props.FrameRateMask[m_Parms.VideoFormat][m_Parms.VideoMode]);
  m_Parms.ColorFormat=CM_Y8;

  if(m_Parms.VideoFormat==7)
  {
    m_Parms.XPos=0;
    m_Parms.YPos=0;
    m_Parms.XSize=HIUINT16(m_Props.Format7Dsc[m_Parms.VideoMode].MaxImageSize);
    m_Parms.YSize=LOUINT16(m_Props.Format7Dsc[m_Parms.VideoMode].MaxImageSize);
    m_Parms.ColorFormat=FindHighestBit(m_Props.Format7Dsc[m_Parms.VideoMode].ColorCoding);
    m_Parms.PktSize=0;
    m_Parms.PktCnt=0;
    m_Parms.BufferCnt=DEFFRMBUFCNT;
    m_Parms.IsoChn=0;
    m_Parms.IsoSpeed=MaxSpeed();
    m_Parms.DmaIsoChnMsk=0x3F;
    m_Parms.DmaIsoChnVal=m_Parms.IsoChn;
    m_Parms.FirstMsk=0xF;
    m_Parms.FirstVal=1;
    m_Parms.TriggerMode=TM_INTERN;
    m_Parms.Y16InversOrder=FALSE;
  }

  if(m_Parms.VideoFormat<=2)
  {
    pSet=&CamSets[m_Parms.VideoFormat][m_Parms.VideoMode];

    m_Parms.ColorFormat=pSet->ColorFormat;
    m_Parms.XPos=0;
    m_Parms.YPos=0;
    m_Parms.XSize=pSet->XSize;
    m_Parms.YSize=pSet->YSize;
    m_Parms.PktSize=pSet->PktSize[m_Parms.FrameRate];
    if(m_Parms.PktSize)
     m_Parms.PktCnt=(UINT16)((FrameSize()-1)/m_Parms.PktSize+1);
    else
     m_Parms.PktCnt=0;
    m_Parms.BufferCnt=DEFFRMBUFCNT;
    m_Parms.IsoChn=0;
    m_Parms.IsoSpeed=MaxSpeed();
    m_Parms.DmaIsoChnMsk=0x3F;
    m_Parms.DmaIsoChnVal=m_Parms.IsoChn;
    m_Parms.FirstMsk=0xF;
    m_Parms.FirstVal=1;
    m_Parms.TriggerMode=TM_INTERN;
    m_Parms.Y16InversOrder=FALSE;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Do one shot with the help of one shot register.

UINT8 CCamera::DoOneShot()
{
  if(m_Parms.TriggerMode!=TM_ONESHOT || !(m_Props.BasicInq&BINQF_ONESHOT))
   return FALSE;

  return WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_ONESHOT,0x80000000);
}

////////////////////////////////////////////////////////////////////////////////
// Just stop camera (only stop iso mode).

UINT8 CCamera::StopCamera()
{
  return WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_ISOENABLE,0);
}

////////////////////////////////////////////////////////////////////////////////
// Start camera.

UINT8 CCamera::StartCamera()
{
  UINT8   Result;
  UINT32  Mode;
  FNFLAGS MyFlags;

  // Set framerate (MUST BE VALID IN FORMAT7 for X-700)
  Result=WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_FRAMERATE,
                   (UINT32)m_Parms.FrameRate<<29);

  // Set channel and speed
  if(Result)
  {
    MyFlags.AsUINT32=Flags();
    if(MyFlags.AsBits.Speed>SPEED400) // write B settings
    {
      Result=WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_CHNSPEED,
                       ((UINT32)1<<15)|
                       ((UINT32)m_Parms.IsoChn<<8)|m_Parms.IsoSpeed);
    }
    else // Legacy
    {
      Result=WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_CHNSPEED,
                       ((UINT32)m_Parms.IsoChn<<28)|
                       ((UINT32)m_Parms.IsoSpeed<<24));
    }
  }

  // Set mode
  if(Result)
   Result=WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_VMODE,
                    (UINT32)m_Parms.VideoMode<<29);

  // Set format
  if(Result)
   Result=WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_VFORMAT,
                   (UINT32)m_Parms.VideoFormat<<29);

  // Set trigger (we don't care for the result here ->WebCams)
  if(Result)
  {
    Mode=0;
    if(m_Parms.TriggerMode==TM_EXTERN)
     Mode=0x82000000;
    if(m_Parms.TriggerMode==TM_MODE15)
     Mode=0x820F0000;
    WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_TRGMODE,Mode);
  }

  // If scalable mode we must set some additional parameters
  if(Result && (m_Parms.VideoFormat==PSFORMAT))
  {
    // Set ColorFormat
    if(!WriteQuad(HIGHOFFSET,
                  SYSBASE+m_Props.Format7Dsc[m_Parms.VideoMode].Offset+CSRCOLORCODING,
                  (UINT32)m_Parms.ColorFormat<<24))
     return FALSE;

    // Set image position
    if(!WriteQuad(HIGHOFFSET,
                  SYSBASE+m_Props.Format7Dsc[m_Parms.VideoMode].Offset+CSRIMAGEPOSITION,
                  MAKEUINT32(m_Parms.XPos,m_Parms.YPos)))
     return FALSE;

    // Set image size
    if(!WriteQuad(HIGHOFFSET,
                  SYSBASE+m_Props.Format7Dsc[m_Parms.VideoMode].Offset+CSRIMAGESIZE,
                  MAKEUINT32(m_Parms.XSize,m_Parms.YSize)))
     return FALSE;

    // Advise camera to calculate new settings
    if(!WriteQuad(HIGHOFFSET,
                  SYSBASE+m_Props.Format7Dsc[m_Parms.VideoMode].Offset+CSRVALUESETTING,
                  0x40000000))
     return FALSE;

    // Wait fixed time
    OSDelay(20);

    // Set packet size
    if(!WriteQuad(HIGHOFFSET,SYSBASE+
                  m_Props.Format7Dsc[m_Parms.VideoMode].Offset+CSRBYTEPERPACKET,
                  (UINT32)m_Parms.PktSize<<16))
     return FALSE;
  }

  // Let camera think about new settings
  if(Result)
   OSDelay(20);

  // Start continous ISO if not oneshot triggermode
  if(Result && m_Parms.TriggerMode!=TM_ONESHOT)
   Result=WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_ISOENABLE,0x80000000);

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Allocate user frames.

UINT8 CCamera::AllocateUserFrames()
{
  UINT32   i,Result;
  char    *pMem;
  FCTLLOCKMEMORY *pLock;

  m_pUserFrames=new FCTLLOCKMEMORY[m_Parms.BufferCnt];
  if(!m_pUserFrames)
   return FALSE;

  pLock=m_pUserFrames;
  m_UserFrameCnt=0;
  for(i=0;i<m_Parms.BufferCnt;i++)
  {
    // Allocate memory
    pMem=new char[m_Parms.PktCnt*m_Parms.PktSize];
    if(pMem)
    {
      pLock->pMem=(UINT8*)pMem;
      pLock->Length=m_Parms.PktCnt*m_Parms.PktSize|(FCTLLOCK_NOSYSTEMMAP<<28);

      // Lock memory
      Result=FCTLLockMemory(CardHandle(),pLock);
      if(Result!=FCE_NOERROR)
      {
        delete[] pMem;
        break;
      }

      pLock++;
      m_UserFrameCnt++;
    }
  }

  m_Parms.RealBufferCnt=m_UserFrameCnt;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Free user frames.

void CCamera::FreeUserFrames()
{
  UINT32 i;

  if(m_pUserFrames)
  {
    for(i=0;i<m_UserFrameCnt;i++)
    {
      FCTLUnlockMemory(CardHandle(),m_pUserFrames[i].pLockMem);
      delete[] (char*)m_pUserFrames[i].pMem;
    }

    delete[] m_pUserFrames;
    m_pUserFrames=NULL;
    m_UserFrameCnt=0;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Bring camera to ISO state (transmit frames).

UINT8 CCamera::StartIso()
{
  UINT8 Result;

  if(m_IsoActive)
   return FALSE;

  // Allocate user frames
  if(m_UseUserFrames)
   AllocateUserFrames();

  // Open DMA
  Result=m_pFireBus->OpenIsoDma(this,m_pUserFrames);

  // Create frame queue (3 less then we have buffers -> DMA keeps 2 buffers)
  if(Result)
   Result=CreateFrmQueue(max(m_Parms.RealBufferCnt,5)-3);

  // Start camera
  if(Result)
   Result=StartCamera();

  if(Result)
  {
    m_IsoActive=TRUE;
  }
  else
  {
    m_pFireBus->CloseIsoDma(this);
    FreeFrmQueue();
    FreeUserFrames();
  }

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Stop ISO-transmission in camera.

UINT8 CCamera::StopIso()
{
  // Stop camera
  if(m_IsoActive)
  {
    // Stop camera so we don't have new frames
    WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_ISOENABLE,0x00000000);

    // Wait a full frame before we stop
    OSDelay((UINT32)(m_Parms.PktCnt*1100.0/ISOFREQUENCY));

    // Free our frame queue
    FreeFrmQueue();

    // Close our DMA channel
    m_pFireBus->CloseIsoDma(this);

    // Free user frames
    FreeUserFrames();

    m_IsoActive=FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Return size of one frame on bus.

UINT32 CCamera::FrameSize(UINT16 XSize,UINT16 YSize,UINT8 ColorFormat)
{
  UINT32 Size;

  Size=XSize*YSize;
  switch(ColorFormat)
  {
    case CM_Y12     :                             //NDE
    case CM_RAW12   :                             //NDE
    case CM_YUV411  : Size=(Size*3)/2; break;
    case CM_YUV422  : Size*=2; break;
    case CM_YUV444  : Size*=3; break;
    case CM_RGB8    : Size*=3; break;
    case CM_RAW16   :                             //NDE
    case CM_Y16     : Size*=2; break;
    case CM_RGB16   : Size*=6; break;
  }

  return Size;
}

////////////////////////////////////////////////////////////////////////////////
// Return size of one frame on bus.

UINT32 CCamera::FrameSize()
{
  return FrameSize(m_Parms.XSize,m_Parms.YSize,m_Parms.ColorFormat);
}

////////////////////////////////////////////////////////////////////////////////
// Called on end of a busreset.

void CCamera::OnBusResetEnd()
{
  // Always restart camera
  if(m_IsoActive)
   StartCamera();
}

////////////////////////////////////////////////////////////////////////////////
// Called on first detection.

UINT8 CCamera::OnStartup()
{
  // For safetiness (SONY!!!) we insert an access latency
  if(VendorId()==0x080046)
   m_AccessLatency=3;

  if(!GetProperties())
   return FALSE;

  m_PropsRead=TRUE;
  AdjustParameters();

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Called on shutdown. This is NOT called if the device is removed and we do
// not have any firewire access to this device.
// In this case only our destructor is called.

void CCamera::OnShutdown()
{
  // Always stop ISO
  StopIso();
}

////////////////////////////////////////////////////////////////////////////////
// Get specific string.

char* CCamera::GetSpeedString(UINT8 Speed)
{
  return SpeedStr[Speed];
}

////////////////////////////////////////////////////////////////////////////////
// Get specific string.

char* CCamera::GetColorFormatString(UINT8 ColorFormat)
{
  return ColorFormatStr[ColorFormat];
}

////////////////////////////////////////////////////////////////////////////////
// Get vendor specific string.

char* CCamera::GetVendorColorFormatString(UINT8 ColorFormat)
{
  if((ColorFormat<128)||(ColorFormat>159))
   return NULL;
   
  return VendorColorFormatStr[ColorFormat-128];
}

////////////////////////////////////////////////////////////////////////////////
// Get specific string.

char* CCamera::GetFrameRateString(UINT8 FrameRate)
{
  return FrameRateStr[FrameRate];
}

////////////////////////////////////////////////////////////////////////////////
// Get specific string.

char* CCamera::GetTriggerModeString(UINT8 TriggerMode)
{
  return TriggerModeStr[TriggerMode];
}

////////////////////////////////////////////////////////////////////////////////
// Set user argument.

void CCamera::SetUser(void *pArg)
{
  m_User=pArg;
}

////////////////////////////////////////////////////////////////////////////////
// Get user argument.

void* CCamera::GetUser()
{
  return m_User;
}

////////////////////////////////////////////////////////////////////////////////
// Create and initialize our frame queue.

UINT8 CCamera::CreateFrmQueue(UINT32 Entries)
{
  m_pFrmInfo=new CAMFRMINFO[Entries];
  if(!m_pFrmInfo)
   return FALSE;

  m_FrmInfoReadIx=0;
  m_FrmInfoWriteIx=0;
  m_FrmInfoSize=Entries;
  m_FrmDisabled=FALSE;

  m_TotalFrmCnt=0;
  m_DiscardedFrmCnt=0;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Free all stuff from our frame queue.

void CCamera::FreeFrmQueue()
{
  FCTLFRMINFO Info;

  // Get access to our frame queue and disable frame queueing and access
  OSSemaphorePend(m_pFrmAccessSemaphore,OSINFTIME);
  m_FrmDisabled=TRUE;
  OSSemaphorePost(m_pFrmAccessSemaphore);

  // Only if still existing
  if(m_pFrmInfo)
  {
    // Return all frames stored in our queue
    if(m_pFireBus)
    {
      while(GetFrame(&Info,0))
       m_pFireBus->ReturnFrame(this,&Info);
    }

    delete[] m_pFrmInfo;
    m_pFrmInfo=NULL;
    m_FrmInfoSize=0;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Called on receiption of a complete frame.
// We return TRUE if we can store this frame in our queue.

UINT8 CCamera::OnFrame(FCTLFRMINFO *pInfo)
{
  UINT8  Full;
  UINT32 NewIndex;

  // Get access to our frame queue
  OSSemaphorePend(m_pFrmAccessSemaphore,OSINFTIME);

  // Only try to store if not disabled
  Full=TRUE;
  if(!m_FrmDisabled)
  {
    m_TotalFrmCnt++;

    // See if we have space for our frames
    NewIndex=(m_FrmInfoWriteIx+1)%m_FrmInfoSize;
    if(NewIndex==m_FrmInfoReadIx)
    {
      m_DiscardedFrmCnt++;
    }
    else
    {
      Full=FALSE;

      m_pFrmInfo[m_FrmInfoWriteIx].Info=*pInfo;
      m_pFrmInfo[m_FrmInfoWriteIx].Number=m_TotalFrmCnt;

      m_FrmInfoWriteIx=NewIndex;

      // Increase semaphore when queue gets new entry to allow pending for frames
      OSSemaphorePost(m_pFrmSemaphore);
    }
  }

  // Release access
  OSSemaphorePost(m_pFrmAccessSemaphore);

  // Return frame if we are full
  if(Full)
   m_pFireBus->ReturnFrame(this,pInfo);

  // Return TRUE to always notify our application
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Return number of available sets.

UINT32 CCamera::GetVidSetCount()
{
  return m_SetCnt;
}

////////////////////////////////////////////////////////////////////////////////
// Return values for a specific set.

UINT8 CCamera::GetVidSetValues(UINT32 Nr,VIDSET *pSet)
{
  UINT32 Format,Mode,FrameRate,Cnt;

  if(Nr>=m_SetCnt)
   return FALSE;

  Cnt=0;
  for(Format=0;Format<8;Format++)
   if(m_Props.VideoFormatMask&(0x80>>Format))
   {
     for(Mode=0;Mode<8;Mode++)
      if(m_Props.VideoModeMask[Format]&(0x80>>Mode))
      {
        // Special handling for scalable format (framerates are implicit)
        if(Format==7)
        {
          if(Cnt==Nr)
          {
            // See if this is the actual mode and copy parameters if yes
            if(m_Parms.VidSet==Cnt)
            {
              pSet->XSize=m_Parms.XSize;
              pSet->YSize=m_Parms.YSize;
              pSet->XPos=m_Parms.XPos;
              pSet->YPos=m_Parms.YPos;
              pSet->ColorFormat=m_Parms.ColorFormat;
              pSet->PktSize=m_Parms.PktSize;
            }
            else
            {
              pSet->XSize=HIUINT16(m_Props.Format7Dsc[Mode].MaxImageSize);;
              pSet->YSize=LOUINT16(m_Props.Format7Dsc[Mode].MaxImageSize);;
              pSet->XPos=0;
              pSet->YPos=0;
              pSet->ColorFormat=FindHighestBit(m_Props.Format7Dsc[Mode].ColorCoding);
              pSet->PktSize=0;
            }

            pSet->VideoFormat=(UINT8)Format;
            pSet->VideoMode=(UINT8)Mode;
            pSet->IsScalable=TRUE;
            pSet->FrameRate=0;
            pSet->XMaxSize=HIUINT16(m_Props.Format7Dsc[Mode].MaxImageSize);
            pSet->YMaxSize=LOUINT16(m_Props.Format7Dsc[Mode].MaxImageSize);
            pSet->XPosUnit=HIUINT16(m_Props.Format7Dsc[Mode].UnitPos);
            pSet->YPosUnit=LOUINT16(m_Props.Format7Dsc[Mode].UnitPos);
            pSet->XSizeUnit=HIUINT16(m_Props.Format7Dsc[Mode].UnitSize);
            pSet->YSizeUnit=LOUINT16(m_Props.Format7Dsc[Mode].UnitSize);
            pSet->ColorInquiry=m_Props.Format7Dsc[Mode].ColorCoding;
            pSet->VendorColorInquiry=m_Props.Format7Dsc[Mode].VendorColorCoding;  //NDE

            // Ready
            return TRUE;
          }
          else
          {
            Cnt++;
          }
        }
        else
        {
          // Go through all framerates
          for(FrameRate=0;FrameRate<8;FrameRate++)
           if(m_Props.FrameRateMask[Format][Mode]&(0x80>>FrameRate))
           {
             if(Cnt==Nr)
             {
               pSet->VideoFormat=(UINT8)Format;
               pSet->VideoMode=(UINT8)Mode;
               pSet->XSize=(UINT16)CamSets[Format][Mode].XSize;
               pSet->YSize=(UINT16)CamSets[Format][Mode].YSize;
               pSet->XPos=0;
               pSet->YPos=0;
               pSet->ColorFormat=CamSets[Format][Mode].ColorFormat;
               pSet->FrameRate=(UINT8)FrameRate;
               pSet->IsScalable=FALSE;
               pSet->PktSize=CamSets[Format][Mode].PktSize[FrameRate];

               // Set to save defaults
               pSet->XMaxSize=pSet->XSize;    // Max X size
               pSet->YMaxSize=pSet->YSize;    // Max Y size
               pSet->XPosUnit=0;              // Unit of position if scalable
               pSet->YPosUnit=0;              // Unit of position if scalable
               pSet->XSizeUnit=0;             // Unit of size if scalable
               pSet->YSizeUnit=0;             // Unit of size if scalable
               pSet->ColorInquiry=0x80000000>>CamSets[Format][Mode].ColorFormat; // Colorformat (Bitfield)

               // Ready...
               return TRUE;
             }
             else
             {
               Cnt++;
             }
           }
         }
      }
   }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Select a specific set.
// Format 7 cannot be selected here!

UINT8 CCamera::SelectVidSetByNr(UINT32 Nr)
{
  UINT8  Format,Mode,FrameRate;
  UINT32 Cnt;
  VIDSET VidSet;

  if(Nr>=m_SetCnt)
   return FALSE;

  Cnt=0;
  for(Format=0;Format<8;Format++)
   if(m_Props.VideoFormatMask&(0x80>>Format))
   {
     for(Mode=0;Mode<8;Mode++)
      if(m_Props.VideoModeMask[Format]&(0x80>>Mode))
      {
        for(FrameRate=0;FrameRate<8;FrameRate++)
         if(m_Props.FrameRateMask[Format][Mode]&(0x80>>FrameRate))
         {
           if(Cnt==Nr)
           {
             // Check if it's a scalable mode
             if(Format==7)
             {
               // Try to select maximum in camera
               VidSet.VideoFormat=Format;
               VidSet.VideoMode=Mode;
               VidSet.XPos=0;
               VidSet.YPos=0;
               VidSet.XSize=HIUINT16(m_Props.Format7Dsc[Mode].MaxImageSize);
               VidSet.YSize=LOUINT16(m_Props.Format7Dsc[Mode].MaxImageSize);
               VidSet.ColorFormat=FindHighestBit(m_Props.Format7Dsc[Mode].ColorCoding);
               VidSet.FrameRate=0;
               VidSet.PktSize=0;
               VidSet.IsScalable=TRUE;
               if(!SelectPartialScan(&VidSet))
                return FALSE;

               m_Parms.VidSet=Nr;
               m_Parms.VideoFormat=VidSet.VideoFormat;
               m_Parms.VideoMode=VidSet.VideoMode;
               m_Parms.FrameRate=VidSet.FrameRate;
               m_Parms.ColorFormat=VidSet.ColorFormat;
               m_Parms.XPos=VidSet.XPos;
               m_Parms.YPos=VidSet.YPos;
               m_Parms.XSize=VidSet.XSize;
               m_Parms.YSize=VidSet.YSize;
               m_Parms.PktSize=VidSet.PktSize;
               if(m_Parms.PktSize)
                m_Parms.PktCnt=(UINT16)((FrameSize()-1)/m_Parms.PktSize+1);
             }
             else
             {
               m_Parms.VidSet=Nr;
               m_Parms.VideoFormat=Format;
               m_Parms.VideoMode=Mode;
               m_Parms.FrameRate=FrameRate;
               m_Parms.ColorFormat=CamSets[Format][Mode].ColorFormat;
               m_Parms.XPos=0;
               m_Parms.YPos=0;
               m_Parms.XSize=CamSets[Format][Mode].XSize;
               m_Parms.YSize=CamSets[Format][Mode].YSize;
               m_Parms.PktSize=CamSets[Format][Mode].PktSize[FrameRate];
               if(m_Parms.PktSize)
                m_Parms.PktCnt=(UINT16)((FrameSize()-1)/m_Parms.PktSize+1);
             }

             return TRUE;
           }
           else
           {
             Cnt++;
           }
         }
      }
   }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Check if these parameters can be satisfied in standard mode.

UINT8 CCamera::IsStandardMode(VIDSET *pSet)
{
  UINT8   Mode,Format;
  CAMSET *pCamSet;

  // Position MUST be zero!
  if(pSet->XPos!=0 || pSet->YPos!=0)
   return FALSE;

  // Go through all standard parameters and see whether we match
  for(Format=0;Format<3;Format++)
  {
    for(Mode=0;Mode<8;Mode++)
    {
      pCamSet=&CamSets[Format][Mode];

      // We MUST match size, colorformat and framerate
      if((pCamSet->XSize==pSet->XSize) &&
         (pCamSet->YSize==pSet->YSize) &&
         (pCamSet->ColorFormat==pSet->ColorFormat) &&
         (pCamSet->PktSize[pSet->FrameRate]!=0))
      {
        // See if we support this mode
        if((m_Props.VideoFormatMask&(0x80>>Format)) &&
           (m_Props.VideoModeMask[Format]&(0x80>>Mode)) &&
           (m_Props.FrameRateMask[Format][Mode]&(0x80>>pSet->FrameRate)) &&
           ((pSet->PktSize==0) || (pSet->PktSize==pCamSet->PktSize[pSet->FrameRate])))
        {
          // Set our actual parameters
          m_Parms.VideoFormat=Format;
          m_Parms.VideoMode=Mode;
          m_Parms.FrameRate=pSet->FrameRate;
          m_Parms.ColorFormat=pSet->ColorFormat;
          m_Parms.XPos=0;
          m_Parms.YPos=0;
          m_Parms.XSize=pSet->XSize;
          m_Parms.YSize=pSet->YSize;
          m_Parms.PktSize=pCamSet->PktSize[pSet->FrameRate];
          m_Parms.PktCnt=(UINT16)((FrameSize()-1)/m_Parms.PktSize+1);

          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Try to match the requested parameters by partial scan.
// The members VideoFormat and VideoMode must be set within the set.
// We adjust parameters within the set and calculate a packetsize.

UINT8 CCamera::MatchPartialScan(VIDSET *pSet)
{
  UINT32 FrmSize;
  double FrmsPerSec;

  // Check format/mode
  if(!m_Props.VideoFormatMask&(0x80>>PSFORMAT) ||
     (m_Props.VideoModeMask[PSFORMAT]==0) ||
     (pSet->VideoMode>7))
   return FALSE;

  // Check mode
  if((m_Props.VideoModeMask[PSFORMAT]&(0x80>>pSet->VideoMode))==0)
   return FALSE;

  // Look for propper color format  //NDE
  if(128>pSet->ColorFormat)
  {
     if((m_Props.Format7Dsc[pSet->VideoMode].ColorCoding&(0x80000000>>pSet->ColorFormat))==0)
       return FALSE;
  }
  // Vendor color format  //NDE
  else
  {
    if((m_Props.Format7Dsc[pSet->VideoMode].VendorColorCoding&(0x80000000>>(pSet->ColorFormat-128)))==0)
      return FALSE;
  }

  if(!m_Props.Format7Dsc[pSet->VideoMode].UnitSize||
     !m_Props.Format7Dsc[pSet->VideoMode].UnitPos)
   return FALSE;

  // Adjust Size
  pSet->XSize=((pSet->XSize-1)/HIUINT16(m_Props.Format7Dsc[pSet->VideoMode].UnitSize))+1;
  pSet->XSize=pSet->XSize*HIUINT16(m_Props.Format7Dsc[pSet->VideoMode].UnitSize);

  pSet->YSize=((pSet->YSize-1)/LOUINT16(m_Props.Format7Dsc[pSet->VideoMode].UnitSize))+1;
  pSet->YSize=pSet->YSize*LOUINT16(m_Props.Format7Dsc[pSet->VideoMode].UnitSize);

  if(pSet->XSize>HIUINT16(m_Props.Format7Dsc[pSet->VideoMode].MaxImageSize) ||
     pSet->YSize>LOUINT16(m_Props.Format7Dsc[pSet->VideoMode].MaxImageSize))
   return FALSE;

  // Adjust position
  pSet->XPos=min(pSet->XPos,HIUINT16(m_Props.Format7Dsc[pSet->VideoMode].MaxImageSize)-pSet->XSize);
  pSet->YPos=min(pSet->YPos,LOUINT16(m_Props.Format7Dsc[pSet->VideoMode].MaxImageSize)-pSet->YSize);

  pSet->XPos-=pSet->XPos%HIUINT16(m_Props.Format7Dsc[pSet->VideoMode].UnitPos);
  pSet->YPos-=pSet->YPos%LOUINT16(m_Props.Format7Dsc[pSet->VideoMode].UnitPos);

  // Match framerate by adjusting packet size
  if(pSet->PktSize==0)
  {
    FrmSize=FrameSize(pSet->XSize,pSet->YSize,pSet->ColorFormat);
    FrmsPerSec=1/FrameTime[pSet->FrameRate];

    // Set packetsize we would like to have
    pSet->PktSize=(UINT16)(((double)FrmSize*FrmsPerSec)/ISOFREQUENCY);
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Try to program partial scan parameters into our camera.

UINT8 CCamera::SelectPartialScan(VIDSET *pSet)
{
  UINT32 Val,PktMinMax;
  UINT8  Result;
  UINT8  VFormat,VMode;
  FORMAT7INFO Info;

  // Prepare FORMAT7INFO
  Info.ColorFormat=pSet->ColorFormat;
  Info.Speed=m_Parms.IsoSpeed;
  Info.VideoMode=pSet->VideoMode;
  Info.XPos=pSet->XPos;
  Info.YPos=pSet->YPos;
  Info.XSize=pSet->XSize;
  Info.YSize=pSet->YSize;

  if(!AcquirePacketInfo(&Info))
   return FALSE;

  // Set PktMinMax variable
  PktMinMax=((UINT32)Info.PacketUnit<<16)|Info.PacketMax;

  // Adjust packet size
  if(pSet->PktSize)
  {
    pSet->PktSize=((pSet->PktSize-1)/HIUINT16(PktMinMax)+1)*HIUINT16(PktMinMax);
    pSet->PktSize=min(pSet->PktSize,LOUINT16(PktMinMax));
  }
  else
  {
    pSet->PktSize=LOUINT16(PktMinMax);

    // Read recommended setting
    if(!ReadQuad(HIGHOFFSET,SYSBASE+m_Props.Format7Dsc[pSet->VideoMode].Offset+
                 CSRBYTEPERPACKET,&Val))
     return FALSE;

    // Check for not present of recommended stuff (is X-700)
    if(LOUINT16(Val)==0)
    {
      // Set packetsize to nearest horizontal size (one line should always fit)
      pSet->PktSize=LOUINT16(PktMinMax);
    }
  }

  // Set actual packetsize
  if(!WriteQuad(HIGHOFFSET,SYSBASE+m_Props.Format7Dsc[pSet->VideoMode].Offset+
                CSRBYTEPERPACKET,MAKEUINT32(pSet->PktSize,0)))
   return FALSE;

  // Check whether camera has accepted this
  Result=ReadQuad(HIGHOFFSET,SYSBASE+m_Props.Format7Dsc[pSet->VideoMode].Offset+
               CSRVALUESETTING,&Val);

  // Error if this register exists and packet size does not fit
  if(!Result || ((Val&0x80000000) && (Val&0x0040000)))
   return FALSE;

  // Use any allowed framerate (X-700 won't start otherwise)
  pSet->FrameRate=0;
  for(VFormat=0;VFormat<8;VFormat++)
   for(VMode=0;VMode<8;VMode++)
    if(m_Props.VideoFormatMask&(0x80>>VFormat) &&
       m_Props.VideoModeMask[VFormat]&(0x80>>VMode))
    {
      pSet->FrameRate=FindHighestBit(m_Props.FrameRateMask[VFormat][VMode]);
      return TRUE;
    }

  // Return TRUE whenever we reach this point
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Select set by values.
// We try to get the most nearest setting for this camera.

UINT8 CCamera::SelectVidSetByValue(VIDSET *pSet)
{
  int i;

  // If we can match a standard mode we use this one
  if(IsStandardMode(pSet))
   return TRUE;

  // Otherwise we check for partial scan
  if(!MatchPartialScan(pSet))
   return FALSE;

  // Select partial scan
  if(!SelectPartialScan(pSet))
   return FALSE;

  // Determine vidset number
  m_Parms.VidSet=m_FirstScalableSet;
  for(i=0;i<pSet->VideoMode;i++)
  {
    if(m_Props.VideoModeMask[PSFORMAT]&(0x80>>i))
     m_Parms.VidSet++;
  }

  // Put all values to our parameter set
  m_Parms.VideoFormat=pSet->VideoFormat;
  m_Parms.VideoMode=pSet->VideoMode;
  m_Parms.ColorFormat=pSet->ColorFormat;
  m_Parms.FrameRate=pSet->FrameRate;
  m_Parms.XPos=pSet->XPos;
  m_Parms.YPos=pSet->YPos;
  m_Parms.XSize=pSet->XSize;
  m_Parms.YSize=pSet->YSize;
  m_Parms.PktSize=pSet->PktSize;

  // Little adjustment for cameras with 400 MBit and large packets
  if(m_Parms.PktSize>4096 && m_Parms.IsoSpeed<=SPEED400)
   m_Parms.PktSize/=2;

  m_Parms.PktCnt=(UINT16)((FrameSize()-1)/m_Parms.PktSize+1);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Return a frame.

void CCamera::ReturnFrame(FCTLFRMINFO *pInfo)
{
  if(m_pFireBus)
   m_pFireBus->ReturnFrame(this,pInfo);
}

////////////////////////////////////////////////////////////////////////////////
// Read a frame from our frame queue.

UINT8 CCamera::GetFrame(FCTLFRMINFO *pInfo,UINT32 TimeoutInMs,UINT32 *pNumber)
{
  UINT8 Result=FALSE;

  if(!m_IsoActive)
   return FALSE;

  // Wait for a frame
  if(!OSSemaphorePend(m_pFrmSemaphore,TimeoutInMs))
   return FALSE;

  // Get access to our frame queue
  OSSemaphorePend(m_pFrmAccessSemaphore,OSINFTIME);

  // Only if not killed
  if(m_pFrmInfo!=NULL)
  {
    // Check if we really have frames
    if(m_FrmInfoReadIx!=m_FrmInfoWriteIx)
    {
      // Copy frame info itself
      *pInfo=m_pFrmInfo[m_FrmInfoReadIx].Info;

      // Copy continous frame number
      if(pNumber)
       *pNumber=m_pFrmInfo[m_FrmInfoReadIx].Number;

      // Adjust read index
      m_FrmInfoReadIx=(m_FrmInfoReadIx+1)%m_FrmInfoSize;

      // Set result to TRUE
      Result=TRUE;
    }
  }

  OSSemaphorePost(m_pFrmAccessSemaphore);

  return Result;
}

////////////////////////////////////////////////////////////////////////////////
// Discard all frames in the frame queue and return number of frames discarded.

UINT32 CCamera::DiscardFrames()
{
  UINT32 FrameCnt=0;

  if(!m_pFrmInfo)
   return 0;

  // Get access to our frame queue
  OSSemaphorePend(m_pFrmAccessSemaphore,OSINFTIME);

  // Do loop until it's empty
  while(m_FrmInfoReadIx!=m_FrmInfoWriteIx)
  {
    FrameCnt++;
    m_DiscardedFrmCnt++;

    // Return frame to driver
    if(m_pFireBus)
     m_pFireBus->ReturnFrame(this,&m_pFrmInfo[m_FrmInfoReadIx].Info);

    // Adjust read index
    m_FrmInfoReadIx=(m_FrmInfoReadIx+1)%m_FrmInfoSize;

    // Decrease frame semaphore
    OSSemaphorePend(m_pFrmSemaphore,0);
  }

  OSSemaphorePost(m_pFrmAccessSemaphore);

  return FrameCnt;
}

////////////////////////////////////////////////////////////////////////////////
// Get address offset for a feature.

UINT32 CCamera::GetFeatureOffset(UINT8 Type,UINT8 *pHasSubValue)
{
  *pHasSubValue=FALSE;

  // Get appropriate offset
  switch(Type)
  {
    case CFT_BRIGHTNESS    : return BRIGHTNESS_INQ;
    case CFT_AUTOEXPOSURE  : return AUTOEXPOSURE_INQ;
    case CFT_SHARPNESS     : return SHARPNESS_INQ;
    case CFT_WHITEBAL      : *pHasSubValue=TRUE;
                             return WHITEBAL_INQ;
    case CFT_HUE           : return HUE_INQ;
    case CFT_SATURATION    : return SATURATION_INQ;
    case CFT_GAMMA         : return GAMMA_INQ;
    case CFT_SHUTTER       : return SHUTTER_INQ;
    case CFT_GAIN          : return GAIN_INQ;
    case CFT_IRIS          : return IRIS_INQ;
    case CFT_FOCUS         : return FOCUS_INQ;
    case CFT_TEMPERATURE   : *pHasSubValue=TRUE;
                             return TEMPERATURE_INQ;
    case CFT_ZOOM          : return ZOOM_INQ;
    case CFT_PAN           : return PAN_INQ;
    case CFT_TILT          : return TILT_INQ;
    case CFT_OPTICALFILTER : return OPTICALFILTER_INQ;
    default                : return 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Get xxx-feature.

UINT8 CCamera::GetFeature(UINT8 Type,FEATPROP *pFeatProp)
{
  UINT32      Offset;
  DCAMFEATINQ Prop;
  DCAMFEATSET Set;
  UINT8       HasSubValue;

  // Get offset for this register
  if((Offset=GetFeatureOffset(Type,&HasSubValue))==0)
   return FALSE;

  // Read cameras inquiry register for this feature
  if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+Offset,&Prop.AsUINT32))
   return FALSE;

  // Check if it's present
  if(!Prop.AsBits.Present)
   return FALSE;

  // Copy properties
  pFeatProp->Type       =Type;
  pFeatProp->HasSubValue=HasSubValue?1:0;
  pFeatProp->OneShot    =Prop.AsBits.OneShot;
  pFeatProp->ReadOut    =Prop.AsBits.ReadOut;
  pFeatProp->OnOff      =Prop.AsBits.OnOff;
  pFeatProp->Auto       =Prop.AsBits.Auto;
  pFeatProp->Manual     =Prop.AsBits.Manual;
  pFeatProp->MinValue   =Prop.AsBits.MinValue;
  pFeatProp->MaxValue   =Prop.AsBits.MaxValue;

  // Set default settings
  pFeatProp->IsOn       =0;
  pFeatProp->IsAuto     =0;
  pFeatProp->Value      =0;
  pFeatProp->SubValue   =0;

  // Read cameras settings register for this feature
  if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+Offset+INQTOCTRLOFFSET,
               &Set.AsUINT32))
   return FALSE;

  // Check all switchable states here
  if(Prop.AsBits.OnOff)
   pFeatProp->IsOn=Set.AsBits.IsOn;

  if(Prop.AsBits.Auto)
   pFeatProp->IsAuto=Set.AsBits.Auto;

  if(Prop.AsBits.OneShot)
   pFeatProp->OneShotIsOn=Set.AsBits.OneShot;

  // Read value if it's readable
  if(pFeatProp->ReadOut)
  {
    // We don't support absolute mode and switch back
    if(Set.AsBits.Absolute)
    {
      Set.AsBits.Absolute=0;
      if(!WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+Offset+INQTOCTRLOFFSET,
                    Set.AsUINT32))
       return FALSE;
    }

    pFeatProp->Value=Set.AsBits.Value;
    if(HasSubValue)
     pFeatProp->SubValue=Set.AsBits.SubValue;
    else
     pFeatProp->SubValue=0;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Set xxx-feature.

UINT8 CCamera::SetFeature(UINT8 Type,FEATSET *pFeatSet)
{
  DCAMFEATSET Set;
  DCAMFEATINQ Prop;
  UINT32      Offset;
  UINT8       HasSubValue;

  // Get offset for this register
  if((Offset=GetFeatureOffset(Type,&HasSubValue))==0)
   return FALSE;

  // Read cameras inquiry register for this feature
  if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+Offset,&Prop.AsUINT32))
   return FALSE;

  // Read actual setting
  if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+Offset+INQTOCTRLOFFSET,
               &Set.AsUINT32))
   return FALSE;

  // See if it's present
  if(!Prop.AsBits.Present)
   return FALSE;

  // Check On/Off possiblity - some cameras need this flag to be set to one
  // though they don't report On/Off capability
  if(!Prop.AsBits.OnOff)
   Set.AsBits.IsOn=1;
  else
   Set.AsBits.IsOn=pFeatSet->On;

  if(pFeatSet->Auto)
  {
    Set.AsBits.Auto=1;
  }
  else
  {
    Set.AsBits.Auto=0;
    if(pFeatSet->OneShot)
    {
      Set.AsBits.OneShot=1;
    }
    else
    {
      Set.AsBits.OneShot=0;
      Set.AsBits.Value=pFeatSet->Value;
      if(HasSubValue)
       Set.AsBits.SubValue=pFeatSet->SubValue;
    }
  }

  // We don't support absolute mode and switch back
  if(Set.AsBits.Absolute)
   Set.AsBits.Absolute=0;

  // Write value to register
  if(!WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+Offset+INQTOCTRLOFFSET,
                Set.AsUINT32))
   return FALSE;

//  if(Set.AsBits.Absolute)
//  {
//    // Read offset for absolute control register
//    if(!ReadQuad(HIGHOFFSET,m_Props.CmdRegBase+Offset+INQTOABSOFFSET,
//                 &Offset))
//     return FALSE;
//
//    // Write absolute value
//    if(!WriteQuad(HIGHOFFSET,SYSBASE+Offset*4,
//                  HasSubValue?Set.AsUINT32&0xFFFFFF:Set.AsUINT32&0xFFF))
//     return FALSE;
//  }

  return TRUE;
}

UINT8 CCamera::SetFeature(UINT8 Type,FEATPROP *pFeatProp)
{
	FEATSET FeatSet;
	FeatSet.Auto = pFeatProp->Auto && pFeatProp->IsAuto ? 1 : 0;
	FeatSet.On = pFeatProp->OnOff && pFeatProp->IsOn ? 1 : 0;
	FeatSet.OneShot = 0;

	FeatSet.Value = pFeatProp->Value;
	FeatSet.SubValue = pFeatProp->SubValue;
	
	return SetFeature(Type, &FeatSet);
}

////////////////////////////////////////////////////////////////////////////////
// Get name for a specific feature.

char* CCamera::GetFeatureString(UINT8 Type)
{
  if(Type>=CFT_LAST)
   return NULL;

  return FeatureString[Type];
}

////////////////////////////////////////////////////////////////////////////////
// Get actual VidSet number.

UINT32 CCamera::GetVidSet()
{
  return m_Parms.VidSet;
}

////////////////////////////////////////////////////////////////////////////////
// Get string for bayer pattern.

char* CCamera::GetBayerPatternString(UINT8 Pattern)
{
  return BayerPatternStr[Pattern];
}

////////////////////////////////////////////////////////////////////////////////
// Acquire packet parameter for partial scan. We put all parameters to our
// camera and ask for packet parameters.

UINT8 CCamera::AcquirePacketInfo(FORMAT7INFO *pInfo)
{
  UINT32  Val;
  UINT32  i,PktMinMax;
  UINT8   Result;
  FNFLAGS MyFlags;

  // Set format
  if(!WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_VFORMAT,
                (UINT32)7<<29))
   return FALSE;

  // Set mode
  if(!WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_VMODE,
                (UINT32)pInfo->VideoMode<<29))
   return FALSE;

  // Clear bytes per packet
  if(!WriteQuad(HIGHOFFSET,SYSBASE+
                m_Props.Format7Dsc[pInfo->VideoMode].Offset+CSRBYTEPERPACKET,0))
   return FALSE;

  // Set transmission speed
  MyFlags.AsUINT32=Flags();
  if(MyFlags.AsBits.Speed>SPEED400) // write B settings
  {
    Result=WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_CHNSPEED,
                       ((UINT32)1<<15)|pInfo->Speed);
  }
  else // Legacy
  {
    Result=WriteQuad(HIGHOFFSET,m_Props.CmdRegBase+CCR_CHNSPEED,
                     (UINT32)pInfo->Speed<<24);
  }

  if(!Result)
   return FALSE;

  // Set Position
  if(!WriteQuad(HIGHOFFSET,
                SYSBASE+m_Props.Format7Dsc[pInfo->VideoMode].Offset+CSRIMAGEPOSITION,
                MAKEUINT32(pInfo->XPos,pInfo->YPos)))
   return FALSE;

  // Set Size
  if(!WriteQuad(HIGHOFFSET,
                SYSBASE+m_Props.Format7Dsc[pInfo->VideoMode].Offset+CSRIMAGESIZE,
                MAKEUINT32(pInfo->XSize,pInfo->YSize)))
   return FALSE;

  // Set ColorFormat
  if(!WriteQuad(HIGHOFFSET,
                SYSBASE+m_Props.Format7Dsc[pInfo->VideoMode].Offset+CSRCOLORCODING,
                (UINT32)pInfo->ColorFormat<<24))
   return FALSE;

  // Advise camera to calculate new settings
  if(!WriteQuad(HIGHOFFSET,
                SYSBASE+m_Props.Format7Dsc[pInfo->VideoMode].Offset+CSRVALUESETTING,
                0x40000000))
   return FALSE;

  // Read value settings register and check if it's present
  Result=ReadQuad(HIGHOFFSET,
                  SYSBASE+m_Props.Format7Dsc[pInfo->VideoMode].Offset+CSRVALUESETTING,
                  &Val);

  // If register not present or no readybit just wait fixed time
  if(!Result || !(Val&0x80000000))
  {
    OSDelay(100);
  }
  else
  {
    // Read Readybit
    for(i=0;i<100;i++)
    {
      if(!ReadQuad(HIGHOFFSET,
                   SYSBASE+m_Props.Format7Dsc[pInfo->VideoMode].Offset+CSRVALUESETTING,
                   &Val))
       return FALSE;

      // Break if feature not present or ready
      if(!(Val&0x80000000) || !(Val&0x40000000))
       break;
    }

    // Timeout or error
    if(i>=100 || ((Val&0x80000000) && (Val&0x00800000)))
     return FALSE;
  }

  // Read min/max
  if(!ReadQuad(HIGHOFFSET,
      SYSBASE+m_Props.Format7Dsc[pInfo->VideoMode].Offset+CSRPACKETINQ,&PktMinMax))
   return FALSE;

  // Return FALSE if we read garbage
  if(HIUINT16(PktMinMax)==0)
   return FALSE;

  pInfo->PacketUnit=HIUINT16(PktMinMax);
  pInfo->PacketMax=LOUINT16(PktMinMax);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Get frame counters.

void CCamera::GetFrameCounter(UINT32 *pTotal,UINT32 *pDiscarded)
{
  *pTotal=m_TotalFrmCnt;
  *pDiscarded=m_DiscardedFrmCnt;
}

////////////////////////////////////////////////////////////////////////////////
// Clear frame counter.

void CCamera::ClearFrameCounter()
{
  m_TotalFrmCnt=0;
  m_DiscardedFrmCnt=0;
}

////////////////////////////////////////////////////////////////////////////////
// Get memory size of all frames.

UINT32 CCamera::GetFrameMemorySize()
{
  UINT32 Size;

  Size=m_Parms.XSize*m_Parms.YSize;

  switch(m_Parms.ColorFormat)
  {
    case CM_Y8    :
                    break;
    case CM_YUV411: Size=Size*3/2;
                    break;
    case CM_YUV422: Size*=2;
                    break;
    case CM_YUV444: Size*=3;
                    break;
    case CM_RGB8  : Size*=3;
                    break;
    case CM_Y16   : Size*=2;
                    break;
    case CM_RGB16 : Size*=6;
                    break;
  }

  return Size*m_Parms.RealBufferCnt;
}

////////////////////////////////////////////////////////////////////////////////
// Get iso packet size.

UINT16 CCamera::GetIsoPayload()
{
  return m_Parms.PktSize;
}

////////////////////////////////////////////////////////////////////////////////
// Set iso channel.

void CCamera::SetIsoChannel(UINT8 Channel)
{
  if(Channel>=64)
  {
    m_IsoAllocated=FALSE;
  }
  else
  {
    m_IsoAllocated=TRUE;
    m_Parms.IsoChn=Channel;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Return TRUE if our iso channel has been allocated.

UINT8 CCamera::IsoAllocated()
{
  return m_IsoAllocated;
}
