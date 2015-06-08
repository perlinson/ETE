#if !defined(AFX_CAMERAVIEW_H__454C0879_6383_4671_B441_C41B6670285D__INCLUDED_)
#define AFX_CAMERAVIEW_H__454C0879_6383_4671_B441_C41B6670285D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <basetype.h>
//#include <mclcppclass.h>

#define BMTYPE          0x4d42

typedef struct POS
{
	CPoint PtLeftUp;
	CPoint PtRightDn;

	int type;

}POS;

typedef struct AREA
{
	int iAreaNum;
	POS pos[10];
}AREA;

typedef struct
{
  UINT8         Blue;
  UINT8         Green;
  UINT8         Red;
}BGRPIX;

class CCameraView : public CView
{
protected:
        // Variables used for compatible bitmaps in memory
        BITMAPINFO     *m_pBitmapInfo;             // Pointer to Bitmap info
        UINT8          *m_pImage;                  // Pointer to actual image

        UINT16          m_OrgXSize;                // Original XSize
        UINT16          m_OrgYSize;                // Original YSize
        
        UINT16          m_XSize;                   // Actual XSize
        UINT16          m_YSize;                   // Actual YSize

        UINT8           m_ColorMode;               // Our color mode
        UINT8           m_BayerPattern;            // Bayer pattern
        UINT8           m_Y16Invert;               // Invert high/low byte

        DWORD           m_HScrollPos;              // Actual HScroll position
        DWORD           m_HScrollMax;              // Maximum HScroll value     
  
        DWORD           m_VScrollPos;              // Actual VScroll position
        DWORD           m_VScrollMax;              // Maximum VScroll position

        BOOL            m_BkgndVisible;            // BkGnd visible or not
        BOOL            m_ScrollMode;              // Scroll mode active flag
        
        RECT            m_SrcRect;                 // Blit rectangle within Image
        RECT            m_DstRect;                 // Blit rectangle within View

        BOOL            m_EraseBkgnd;              // Erase BKGnd flag
        
        BOOL            m_SelectionEnabled;        // Selection enabled
        BOOL            m_DrawSelection;           // Draw selection or not
        RECT            m_DragRect;                // Drag rectangle
        POINT           m_StartPoint;              // Drag point
        int             m_Msg;                     // Message to post 
        HWND            m_hMsgWnd;                 // Window to post to

		AREA			m_area;
		BOOL			m_bROISelect;
		BOOL			m_bNROISelect;
		BOOL			m_bReqRegSelect;
		BOOL			m_bDrawRegSelect;
		POINT			m_RegStartPoint;
		RECT			m_RegRect;

		int				m_SliderXPos;
		int				m_SliderYPos;
		int				m_frameIndex;				//保存的图片名称的索引
		int				m_frameIndexInit;			//保存的图片名称的初始索引
		bool			m_bUseMirror;
		BOOL			m_bRecordFrame;
		BOOL			m_bCompleteSave;
		
		int				m_bInternalTriggerMode;
		CString			m_strImgePath;
		bool			m_bInConfocal3dModel;
		int				m_differ;
		
		
// 		mwArray			m_mwFirst;
// 		mwArray			m_mwSecond;
// 		mwArray			m_mwThird;
// 		mwArray			m_currentSelection;


		int				m_currentIdx;

protected:
	DECLARE_DYNCREATE(CCameraView)
	
	bool InConfocal3dModel() const { return m_bInConfocal3dModel; }
	void InConfocal3dModel(bool val) { m_bInConfocal3dModel = val; }

	

	void            SetScroller(DWORD Which,int Min,int Max,UINT Page,
                                    int Pos,BOOL Redraw=TRUE);
        void            AdjustBlitRectangles(DWORD XSize,DWORD YSize);
        void            RecalcScrollbars();
        void            InitGrayBitmap();
        void            InitColorBitmap();
        void            EraseBkgnd(CDC *pDC);
		void			SaveGrayBitmap();
		void			SaveColorBitmap();

// Attribute
public:

// Operationen
public:
	CCameraView(); 

        virtual BOOL    CreateBitmap(UINT32 Width,UINT32 Height,UINT8 ColorMode,
                                     UINT8 BayerPattern=0,UINT8 RawMode=FALSE,
                                     UINT8 Y16Invert=FALSE);
        virtual void   DeleteBitmap();

        virtual BITMAPINFO* GetBitmapInfo()     { return m_pBitmapInfo; }
        virtual UINT8* GetImage()               { return m_pImage; }  
        
        virtual void   ProcessFrame(UINT8 *pData,UINT32 Length,UINT8 Redraw=TRUE);

        virtual void   SetScrollmode(BOOL Enable);
        virtual void   Resize(RECT *pSrcRect=NULL,UINT16 NewWidth=0,UINT16 NewHeight=0);

		void SetFrameRegions(int type);
		void SetSliderXPos(int pos);
		void SetSliderYPos(int pos);
		BOOL RecordFrame() const { return m_bRecordFrame; }
		void RecordFrame(BOOL val, BOOL bInternalTriggerMode = TRUE) { m_bRecordFrame = val; m_bInternalTriggerMode = bInternalTriggerMode; }
	//{{AFX_VIRTUAL(CCameraView)
	protected:
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CCameraView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_MSG(CCameraView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		void SetUseMirror(bool bUseMirror);
		void SetImgSavePath(CString imagePath);
		void RecordFrame();
		void ResetFrameIndex();
		BOOL CompleteSave() const { return m_bCompleteSave; }
		void CompleteSave(BOOL val) { m_bCompleteSave = val; }
		void Differ(int val) { m_differ = val; }
		void GetSelection(double& left, double& right);
		void ResetToIndexForEveryMask(int mask_Index);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // AFX_CAMERAVIEW_H__454C0879_6383_4671_B441_C41B6670285D__INCLUDED_
