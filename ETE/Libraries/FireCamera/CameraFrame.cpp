////////////////////////////////////////////////////////////////////////////////
// Software for FirePackage (c) Evaluation.
// intek - c. kuehnel - 2007
// This software may be used and distributed freely. 
// Please leave a hint to the original supplier intek within your source code.
////////////////////////////////////////////////////////////////////////////////

#include "../../stdafx.h"
#include "FireDemo.h"
#include "CameraFrame.h"
#include "FireDemoDlg.h"
#include "dlgcamparms.h"
#include "dlgfeature.h"
#include "dlgusersize.h"
#include "dlgdirectaccess.h"
#include "dlgrecord.h"
#include "dlglicenserequest.h"
#include "dlgabout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCameraFrame

CCameraFrame::CCameraFrame(CCamera *pCamera)
{
  m_pCamera=pCamera;
  m_SettingsValid=FALSE;
  m_IsNotified=FALSE;
  m_IsoReported=FALSE;
  m_pDlgModal=NULL;

  strcpy(m_RecordInfo.Directory,"C:\\");
  strcpy(m_RecordInfo.NamePrefix,"Image");
  m_RecordInfo.FrameCount=10;
  m_RecordInfo.Divider=1;

  m_RecordMode=FALSE;
}

CCameraFrame::~CCameraFrame()
{
  if(m_pStatusBar)
   delete m_pStatusBar;
}

BEGIN_MESSAGE_MAP(CCameraFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CCameraFrame)

	ON_WM_CREATE()
	ON_WM_CLOSE()
#if 0 ////xxxx
	ON_COMMAND(IDI_EDITSETTINGS, OnEditsettings)
	ON_COMMAND(IDI_RUN, OnRun)
	ON_COMMAND(IDI_160X120, On160x120)
	ON_COMMAND(IDI_320X240, On320x240)
	ON_COMMAND(IDI_1024X768, On1024x768)
	ON_COMMAND(IDI_1280X960, On1280x960)
	ON_COMMAND(IDI_640X480, On640x480)
	ON_COMMAND(IDI_800X600, On800x600)
	ON_COMMAND(IDI_FEATURE, OnFeature)
        ON_MESSAGE(WM_NEWFRAME, OnNewFrame)
        ON_MESSAGE(WM_ENDTHREAD, OnEndThread)
	ON_COMMAND(IDI_1600X1200, On1600x1200)
	ON_COMMAND(IDI_USERSIZE, OnUsersize)
	ON_COMMAND(IDI_SAVE, OnSave)
	ON_COMMAND(IDI_SCROLLMODE, OnScrollmode)
	ON_COMMAND(IDI_DIRECTACCESS, OnDirectAccess)
	ON_COMMAND(IDI_RECORD, OnRecord)
	ON_COMMAND(IDI_LICENSEREQUEST, OnLicenserequest)
	ON_COMMAND(IDI_ABOUT, OnAbout)
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Reaction on window create.

int CCameraFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
   return -1;

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Set frame window size so that we get an exact view size.

void CCameraFrame::SetViewSize(UINT32 Width,UINT32 Height)
{
  RECT FrameRect,ViewRect;
  int  AddWidth,AddHeight;

  GetWindowRect(&FrameRect);
  m_pView->GetWindowRect(&ViewRect);

  AddWidth=(FrameRect.right-FrameRect.left)-(ViewRect.right-ViewRect.left)+
            2*GetSystemMetrics(SM_CXEDGE);
  AddHeight=FrameRect.bottom-FrameRect.top-(ViewRect.bottom-ViewRect.top)+
            2*GetSystemMetrics(SM_CYEDGE);

  MoveWindow(FrameRect.left,FrameRect.top,Width+AddWidth,Height+AddHeight);
}

/////////////////////////////////////////////////////////////////////////////
// Handle close message.

void CCameraFrame::OnClose()
{
  // We don't close!!!
}

/////////////////////////////////////////////////////////////////////////////
// Handle Edit settings message.
#if 0 ///xxxx
void CCameraFrame::OnEditsettings()
{
  VIDSET VidSet;
  char   cbuf[128];

  // Show property dialog
  CDlgCamParms *pDlg;

  // Return if camera is active
  if(m_pCamera && m_pCamera->IsoActive())
   return;

  // Create dialog
  m_pDlgModal=pDlg=new CDlgCamParms(this);
  if(pDlg)
  {
    // Assign a camera
    pDlg->AssignCamera(m_pCamera);

    // Do modal...
    if(pDlg->DoModal()==IDOK)
    {
      // Change bitmap on success
      m_SettingsValid=TRUE;
      m_pView->CreateBitmap(m_pCamera->pParms()->XSize,
                            m_pCamera->pParms()->YSize,
                            m_pCamera->pParms()->ColorFormat,
                            m_pCamera->pParms()->BayerPattern,
                            m_pCamera->pParms()->YIsRawMode,
                            m_pCamera->pParms()->Y16InversOrder);

      // Show actual mode
      m_pCamera->GetVidSetValues(m_pCamera->GetVidSet(),&VidSet);

      if(VidSet.VideoFormat!=PSFORMAT)
       wsprintf(cbuf,"%dx%d %s %s f/s",VidSet.XSize,VidSet.YSize,
                m_pCamera->GetColorFormatString(VidSet.ColorFormat),
                m_pCamera->GetFrameRateString(VidSet.FrameRate));
      else
       wsprintf(cbuf,"%dx%d %s",VidSet.XSize,VidSet.YSize,
                m_pCamera->GetColorFormatString(VidSet.ColorFormat));
      m_pStatusBar->SetPaneText(7,cbuf);
    }
    else
    {
      // Delete bitmap if we never got valid settings
      if(!m_SettingsValid)
       m_pView->DeleteBitmap();
    }

    // No modal dialog anymore
    m_pDlgModal=NULL;
    delete pDlg;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Edit actual camera settings.

void CCameraFrame::EditSettings()
{
  OnEditsettings();
}

/////////////////////////////////////////////////////////////////////////////
// Start acquisition.

BOOL CCameraFrame::StartAcquisition(BOOL Start)
{
  CAMPARMS *pParms;
  char      Msg[64];
  UINT32    Result;

  if(!m_pCamera || !m_SettingsValid)
   return FALSE;

  if(Start)
  {
    // Prepare record stuff
    m_RecordFrameCount=0;
    m_RecordDivider=1;

    // Reset frame counter
    m_pCamera->ClearFrameCounter();

    // Store start time to calculate framerate
    m_StartTime=GetTickCount();

    // Reset frame rate field
    m_pStatusBar->SetPaneText(5,"0.0");

    // Check control key to switch between user/system frames
    m_pCamera->UseUserFrames((0x8000&GetAsyncKeyState(VK_CONTROL))?TRUE:FALSE);
    
    // Start camera
    if(!m_pCamera->StartIso())
    {
      Result=m_pCamera->LastError();
      pFireDemoDlg->LogDWORD("Error <CCamera::StartIso>",Result,FALSE);
      return FALSE;
    }  

    // Check real allocated and intended buffer allocation
    pParms=m_pCamera->pParms();
    if(pParms->BufferCnt!=pParms->RealBufferCnt)
    {
      wsprintf(Msg,"W: Partial buffer allocation (%d from %d)",
               pParms->RealBufferCnt,pParms->BufferCnt);
      pFireDemoDlg->LogMessage(Msg);
    }
    
    GetMenu()->CheckMenuItem(IDI_RUN,MF_CHECKED);
    if(!m_IsoReported)
    {
      m_IsoReported=TRUE;
      pFireDemoDlg->ReportIsoSettings(m_pCamera->GetIsoPayload(),
                                      m_pCamera->GetFrameMemorySize());
    }
  }
  else
  {
    m_pCamera->StopIso();

    GetMenu()->CheckMenuItem(IDI_RUN,MF_UNCHECKED);
    if(m_IsoReported)
    {
      m_IsoReported=FALSE;
      pFireDemoDlg->ReportIsoSettings(-(long)m_pCamera->GetIsoPayload(),
                                      -(long)m_pCamera->GetFrameMemorySize());
    }
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Reaction on Run menu entry.

void CCameraFrame::OnRun()
{
  if(m_pCamera)
  {
    if(m_pCamera->IsoActive())
     StartAcquisition(FALSE);
    else
     StartAcquisition(TRUE);
  }
}

/////////////////////////////////////////////////////////////////////////////
// Setting view size...

void CCameraFrame::On160x120()
{
  SetViewSize(160,120);
}

void CCameraFrame::On320x240()
{
  SetViewSize(320,240);
}

void CCameraFrame::On1024x768()
{
  SetViewSize(1024,768);
}

void CCameraFrame::On1280x960()
{
  SetViewSize(1280,960);
}

void CCameraFrame::On1600x1200()
{
  SetViewSize(1600,1200);
}

void CCameraFrame::On640x480()
{
  SetViewSize(640,480);
}

void CCameraFrame::On800x600()
{
  SetViewSize(800,600);
}

/////////////////////////////////////////////////////////////////////////////
// Record function. FrameNumber is assigend by our camera object and
// increases on every frame coming from the camera. If it is not continous
// we see that we have missed frames and we are too slow.

void CCameraFrame::RecordFrame(FCTLFRMINFO *pFrameInfo,UINT32 FrameNumber)
{
  char FileName[256];
  char Name[64];

  // Give new frame to our view for conversion and assignment but don't update
  m_pView->ProcessFrame(pFrameInfo->pData,pFrameInfo->Length,FALSE);
  
  // Check our divider to figure out whether we have to store
  if(--m_RecordDivider==0)
  {
    m_RecordDivider=m_RecordInfo.Divider;
    m_RecordFrameCount++;

    // Only store until frame limit has been reached
    if((m_RecordInfo.FrameCount==0) ||
       (m_RecordFrameCount<=m_RecordInfo.FrameCount))
    {
      // Create filename
      wsprintf(Name,"%s%08d_%03d.bmp",
               m_RecordInfo.NamePrefix,
               m_RecordFrameCount%1000,
               FrameNumber);

      // Build actual filename
      if(m_RecordInfo.Directory[strlen(m_RecordInfo.Directory)-1]=='\\')
       wsprintf(FileName,"%s%s",m_RecordInfo.Directory,Name);
      else
       wsprintf(FileName,"%s\\%s",m_RecordInfo.Directory,Name);

      // Write bitmap and notify our record dialog
      if(WriteBitmap(FileName))
       ((CDlgRecord*)m_pDlgModal)->OnFileWritten(Name,m_RecordFrameCount);
    }

    // See if we must stop the record process
    if(m_RecordFrameCount==m_RecordInfo.FrameCount)
     ((CDlgRecord*)m_pDlgModal)->OnRecordEnd();
  }
}

/////////////////////////////////////////////////////////////////////////////
// Push frame through our work pipeline.

void CCameraFrame::PushFrame(FCTLFRMINFO *pFrameInfo,UINT32 FrameNumber)
{
  if(!m_RecordMode)
  {
    // Just put new frame to our view
    m_pView->ProcessFrame(pFrameInfo->pData,pFrameInfo->Length);
  }
  else
  {
    // Call our record function
    RecordFrame(pFrameInfo,FrameNumber);
  }
}

/////////////////////////////////////////////////////////////////////////////
// Reaction on message WM_NEWFRAME.

LRESULT CCameraFrame::OnNewFrame(WPARAM wParam,LPARAM lParam)
{
  UINT32      Total,Discarded;
  char        cbuf[32];
  FCTLFRMINFO FrameInfo;
  UINT32      MaxFrames;
  UINT32      FrameNumber;
  UINT8       Result;
  double      RunTime;

  if(m_pCamera)
  {
    MaxFrames=m_pCamera->FrameQueueSize();
    do
    {
      m_pCamera->Lock();
      Result=m_pCamera->GetFrame(&FrameInfo,0,&FrameNumber);
      if(Result)
      {
        // Push frame
        PushFrame(&FrameInfo,FrameNumber);

        // Return frame to camera
        m_pCamera->ReturnFrame(&FrameInfo);
      }
      m_pCamera->Unlock();
    }while(Result && --MaxFrames);

    // From now on we can be notified again
    m_IsNotified=FALSE;

    // Discard all frames here. If we are too slow we have time for GUI until
    // the next frame arrives
    m_pCamera->Lock();
    m_pCamera->DiscardFrames();
    m_pCamera->Unlock();

    // Show statistic
    m_pCamera->GetFrameCounter(&Total,&Discarded);

    wsprintf(cbuf,"%d",Total);
    m_pStatusBar->SetPaneText(1,cbuf);

    wsprintf(cbuf,"%d",Discarded);
    m_pStatusBar->SetPaneText(3,cbuf);

    // Calculate framerate every 10th frame
    if(Total%16==0)
    {
      // We accept overrun after 50 days
      RunTime=(double)(GetTickCount()-m_StartTime)/1000.0;
      if(RunTime>1.0)
      {
        sprintf(cbuf,"%.1lf",(double)(Total)/RunTime);
        m_pStatusBar->SetPaneText(5,cbuf);
      }
    }
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Reaction on new frame.

void CCameraFrame::NewFrameNotify()
{
  // Only react if we have a camera and no processing takes place
  if(m_pCamera && !m_IsNotified)
  {
    m_IsNotified=TRUE;

    // Post a message to our own window to start out thread (change in context)
    PostMessage(WM_NEWFRAME,0,0);
  }
}

/////////////////////////////////////////////////////////////////////////////
// Reaction on feature control.

void CCameraFrame::OnFeature()
{
  CDlgFeature *pDlg;

  pDlg=new CDlgFeature(this);
  if(pDlg)
  {
    pDlg->AssignCamera(m_pCamera);
    pDlg->Create(IDD_FEATURE,this);
  }
}

/////////////////////////////////////////////////////////////////////////////
// Reaction on End Thread message.

LRESULT CCameraFrame::OnEndThread(WPARAM wParam,LPARAM lParam)
{
  // If active modal dialog we must first close this one to return from MsgLoop
  if(m_pDlgModal!=NULL)
  {
    m_pDlgModal->EndDialog(IDCANCEL);
    m_pDlgModal=NULL;
  }

  // Post quit message to our thread
  PostQuitMessage(0);

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Set window size to user defined size.

void CCameraFrame::OnUsersize()
{
  CDlgUserSize *pDlg;
  RECT     ViewRect;
  DWORD    XSize,YSize;

  GetActiveView()->GetWindowRect(&ViewRect);
  XSize=ViewRect.right-ViewRect.left-2*GetSystemMetrics(SM_CXEDGE);
  YSize=ViewRect.bottom-ViewRect.top-2*GetSystemMetrics(SM_CYEDGE);;

  m_pDlgModal=pDlg=new CDlgUserSize(this);

  // Show dialog and change view size on success
  if(pDlg->DoModal(&XSize,&YSize)==IDOK)
   SetViewSize(XSize,YSize);

  m_pDlgModal=NULL;
  delete pDlg;
}

/////////////////////////////////////////////////////////////////////////////
// Write a bitmap file coming from our View.

BOOL CCameraFrame::WriteBitmap(char *pFilename)
{
  BITMAPINFO  *pInfo;
  UINT8       *pImage;
  OFSTRUCT    Ofs;
  HFILE       hFile;
  DWORD       i,LineSize,Offs;
  BITMAPFILEHEADER fh;
  BITMAPINFOHEADER bmi;

  // Get image information from our connected view
  pImage=m_pView->GetImage();
  pInfo=m_pView->GetBitmapInfo();

  if(!pInfo||!pImage)
   return FALSE;

  hFile=OpenFile(pFilename,&Ofs,OF_CREATE);
  if(hFile==HFILE_ERROR)
   return FALSE;

  // Copy Bitmapinfoheader (set Width and Height absolute)
  bmi=pInfo->bmiHeader;
  bmi.biWidth=abs(pInfo->bmiHeader.biWidth);
  bmi.biHeight=abs(pInfo->bmiHeader.biHeight);

  // Build fileheader and write
  fh.bfType=BMTYPE;
  if(pInfo->bmiHeader.biBitCount==8)
  {
    fh.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+
                 256*sizeof(RGBQUAD);
    fh.bfSize=fh.bfOffBits+pInfo->bmiHeader.biSizeImage;
    bmi.biClrUsed=256;
    bmi.biClrImportant=0;
  }
  else
  {
    fh.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    fh.bfSize   =fh.bfOffBits+pInfo->bmiHeader.biSizeImage;
    bmi.biClrUsed=0;
    bmi.biClrImportant=0;
  }

  fh.bfReserved1=0;
  fh.bfReserved2=0;

  // Write fileheader
  _hwrite(hFile,(char*)&fh,sizeof(BITMAPFILEHEADER));

  // Write bitmapinfoheader
  _hwrite(hFile,(char*)&bmi,sizeof(BITMAPINFOHEADER));

  // Write Palette (only if grayscale)
  if(bmi.biClrUsed!=0)
   _hwrite(hFile,(char*)pInfo->bmiColors,pInfo->bmiHeader.biClrUsed*sizeof(RGBQUAD));

  // Write all lines (from bottom up)
  LineSize=bmi.biWidth*bmi.biBitCount/8;
  Offs=LineSize*(bmi.biHeight-1);
  for(i=0;i<(UINT32)bmi.biHeight;i++)
  {
    _hwrite(hFile,(char*)&pImage[Offs],LineSize);
    Offs-=LineSize;
  }

  _lclose(hFile);

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Save bitmap from our View.

void CCameraFrame::OnSave()
{
  CFileDialog *pDlg;
  char         Filename[256]="Image.bmp";

  if(m_pView && m_pView->GetImage() && m_pView->GetBitmapInfo())
  {
    pDlg=new CFileDialog(FALSE,"bmp",Filename,
                         OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
                         "Bitmaps (*.bmp)|*.bmp|All files (*.*)|*.*||",
                         this);
    m_pDlgModal=pDlg;
    if(pDlg)
    {
      pDlg->m_ofn.lpstrFile=Filename;
      pDlg->m_ofn.nMaxFile=sizeof(Filename);
      if(pDlg->DoModal()==IDOK)
      {
        WriteBitmap(Filename);
      }
      delete pDlg;
    }

    // Special handling: CFileDialog does not close with EndDialog (s. OnEndThread)
    // Instead it quits with the PostQuitMessage. So if we are here and the pointer
    // is NULL we must post the quit message again.
    if(m_pDlgModal==NULL)
     PostQuitMessage(0);

    m_pDlgModal=NULL;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Change scrollmode of our view.

void CCameraFrame::OnScrollmode()
{
  if(GetMenu()->GetMenuState(IDI_SCROLLMODE,MF_BYCOMMAND)&MF_CHECKED)
  {
    GetMenu()->CheckMenuItem(IDI_SCROLLMODE,MF_UNCHECKED);
    if(m_pView)
     m_pView->SetScrollmode(FALSE);
  }
  else
  {
    GetMenu()->CheckMenuItem(IDI_SCROLLMODE,MF_CHECKED);
    if(m_pView)
     m_pView->SetScrollmode(TRUE);
  }
}

/////////////////////////////////////////////////////////////////////////////
// Allows direct register access to a camera.

void CCameraFrame::OnDirectAccess()
{
  CDlgDirectAccess *pDlg;

  m_pDlgModal=pDlg=new CDlgDirectAccess;
  if(pDlg)
  {
    pDlg->AssignCamera(m_pCamera);
    pDlg->DoModal();
    m_pDlgModal=NULL;
    delete pDlg;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Stores frames on harddisk.

void CCameraFrame::OnRecord()
{
  CDlgRecord *pDlg;

  if(m_pCamera && m_pCamera->IsoActive())
   return;

  m_pDlgModal=pDlg=new CDlgRecord(this,&m_RecordInfo);
  if(pDlg)
  {
    pDlg->AssignCamera(m_pCamera);

    // Prepare all settings for record mode
    m_RecordMode=TRUE;

    // Show dialog
    pDlg->DoModal();

    // Normal mode on exit
    m_RecordMode=FALSE;

    m_pDlgModal=NULL;
    delete pDlg;
  }
}

/////////////////////////////////////////////////////////////////////////////
// Create license request string.

void CCameraFrame::OnLicenserequest() 
{
  FCTLLICENSEREQUEST Arg;
  UINT32             Result;
  char               License[128];
  CDlgLicenseRequest *pDlg;

  Arg.BusAddress=m_pCamera->BusAddress();
  Arg.NodeAddress=m_pCamera->NodeAddress();
  
  Result=FCTLLicenseRequest(m_pCamera->CardHandle(),&Arg);

  if(Result==FCE_NOERROR)
  {
    wsprintf(License,"Camera: %s\15\15\12Host: %s\15\15\12Card: %s",
             Arg.NodeRequest,Arg.HostRequest,Arg.CardRequest);
    m_pDlgModal=pDlg=new CDlgLicenseRequest;
    pDlg->SetText(License);
    pDlg->DoModal();
    m_pDlgModal=NULL;
    delete pDlg;
  }
}

void CCameraFrame::OnAbout() 
{
  CDlgAbout *pDlg;

  m_pDlgModal=pDlg=new CDlgAbout;
  pDlg->DoModal();
  m_pDlgModal=NULL;
  delete pDlg;
}
#endif