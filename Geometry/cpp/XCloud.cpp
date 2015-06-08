#include "StdAfx.h"
#include "XCloud.h"
#include <algorithm>

#include "xcylinder.h"
#include "..\ti_cld_point.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

double minyvalue,maxyvalue;

XBinBox::XBinBox(void)
{
	m_bNoEmpty = false;
}

XBinBox::~XBinBox(void)
{
}

XCloud::XCloud(void)
: m_iSampleCoef(1)
, m_BinArray(NULL)

	{
	m_ObjectType = OBJECT_CLOUD;
	m_Name = "Cloud";// check if it's correct
	m_Color	= BLACK_COLOR;
	m_bRawdata = 0;
	m_bShow = true;	
	m_BndBox.Reset();	
	m_DispType = 0;
	m_Point.reserve(5);
	m_SNR.reserve(5);
	m_bObjChanged = true;
	m_PointSize = 1;
	m_pAnalysis.RemoveAll();
	m_bShowAnalysis=true;
	m_bShowTolZone = true;
	iBreakS = iBreakE = -1;
	m_bAnalysis = false;
	m_bTolzone = false;
	sample_dist = 0;
	//by jm Feb 7 2006 *********************//
	m_bShowROIRect = false;
	}

XCloud::XCloud(const XCloud& aXCloud)
: m_iSampleCoef(1)
, m_BinArray(NULL)

	{
	(*this) = aXCloud;
	}
XCloud::XCloud(long aNum)
: m_iSampleCoef(1)
, m_BinArray(NULL)

	{	
	m_bRawdata = 0;
	m_ObjectType = OBJECT_CLOUD;
	m_Point.reserve(aNum);
	m_PointSize = 1;
	m_DispType = 0;
	m_Color = WHITE_COLOR;
	m_iSampleCoef = 1;
	m_bObjChanged = true;
	iBreakS = iBreakE = -1;
	m_bAnalysis = false;
	m_bTolzone = false;
	}
XCloud::~XCloud(void)
	{
	InitObject();
	}
void XCloud::SetPointSize(int size)
	{
		if(	m_PointSize != size)
		{
			this->m_PointSize = size;
			this->m_bDispChanged = true;
		}
	}

void  XCloud::SetCldSize(int width, int height)
{
	this-> m_cldwidth=width;
	this-> m_cldheight=height;
}


void XCloud::InitObject(void)
	{
	if(m_Point.size() > 0)
		m_Point.clear();
	if(m_SNR.size() > 0)
		m_SNR.clear();
	if(m_Total.size() > 0)
		m_Total.clear();
	if(m_BinArray!=NULL)
	{
		delete []m_BinArray;
		m_BinArray = NULL;
	}
	iBreakS = iBreakE = -1;
	m_bAnalysis = false;
	m_bTolzone = false;
	sample_dist = 0;
	//by jm Feb 7 2006 *********************//
	m_bShowROIRect = false;

	m_pAnalysis.RemoveAll();

	m_bRawdata = 0;
	this->m_bObjChanged = true;
	}
	XCloud& XCloud::operator+( const XCloud& SourceCld )
	{
			m_Point.insert(m_Point.end(), SourceCld.m_Point.begin(), SourceCld.m_Point.end());
	if(this->m_bRawdata &&SourceCld.m_bRawdata&&(m_bRawdata==SourceCld.m_bRawdata))
		{
		
		this->m_SNR.insert(m_SNR.end(), SourceCld.m_SNR.begin(), SourceCld.m_SNR.end());
		if(m_bRawdata>1)
		this->m_Total.insert(m_Total.end(), SourceCld.m_Total.begin(), SourceCld.m_Total.end());
		//if(m_bRawdata>2)
		//this->m_LaserInvert.insert(m_LaserInvert.end(), SourceCld.m_LaserInvert.begin(), SourceCld.m_LaserInvert.end());
		}
	this->m_bObjChanged = true;
	return *this;

	}

XCloud& XCloud::operator=( const XCloud& SourceCld )
	{

	if( &SourceCld != this)
		{
		this->InitObject();
		(*(XGeomBase*)this)= SourceCld;
		m_Name = SourceCld.m_Name;// check if it's correct
		m_bRawdata = SourceCld.m_bRawdata;
		//mDrawBBox = SourceCld.mDrawBBox;
		//mMaxLeng = SourceCld.mMaxLeng;	
		m_bShow = SourceCld.m_bShow;	
		m_BndBox = SourceCld.m_BndBox;	
		m_Unit = SourceCld.m_Unit;
		if(SourceCld.m_Point.size() > 0)
			m_Point = SourceCld.m_Point;
		if(SourceCld.m_SNR.size() > 0)
			m_SNR = SourceCld.m_SNR;
		m_Color	= SourceCld.m_Color;
		m_PointSize = SourceCld.m_PointSize;
		m_DispType = SourceCld.m_DispType;
		m_bObjChanged = SourceCld.m_bObjChanged;
		if(SourceCld.m_Total.size() >0)
			m_Total = SourceCld.m_Total;
			m_BinArray = NULL;
		iBreakS = SourceCld.iBreakS;
		iBreakE = SourceCld.iBreakE;
		m_bAnalysis = SourceCld.m_bAnalysis;
		m_bTolzone = SourceCld.m_bTolzone;
		sample_dist = SourceCld.sample_dist;
		//by jm Feb 7 2006 *********************//
		m_bShowROIRect = SourceCld.m_bShowROIRect;
	}
	return *this;
	}

int XCloud::GetPntNum(void)const
	{
	return  m_Point.size();
	}
void XCloud::GetPoint(const int& index, PNT& aVector3)const
{
//#ifdef _DEBUG
//	ASSERT( index >= 0 && index <GetPntNum());
//#endif 
	if(index < 0 || index >= GetPntNum())
		return;
	aVector3 = this->m_Point[index];	
}
int XCloud::GetPointSize(void)const
	{
	return  m_PointSize;
	}
const int&XCloud:: GetSampleCoef()const
	{
	return this->m_iSampleCoef;
	}
	void XCloud::SetSampleCoef(const int& newcoef)
	{
		if(this->m_iSampleCoef != newcoef)
		{
			this->m_iSampleCoef = newcoef;
			this->m_bDispChanged = true;
		}
	}

BOOL XCloud::SaveFile_Asc(CArchive &ar)const
	{
	int num = GetPntNum();
	if(num < 1)
		return FALSE;
	PNT pnt;
	TCHAR headinformation[256];
	_stprintf_s(headinformation,TEXT("//Cloud name:%s\n"), this->GetName());
	ar.WriteString(headinformation);
	_stprintf_s(headinformation,TEXT("//Point number:%d\n"),num);
	ar.WriteString(headinformation);
	if(this->GetUnit() == UNIT_INCH)
		ar.WriteString(TEXT("//Unit: inch\n"));
	if(this->GetUnit()== UNIT_MM)
		ar.WriteString(TEXT("//Unit: mm\n"));
	for (int j=0;j<num; j++)
		{
		pnt =(*this)[j];
		_stprintf_s(headinformation,TEXT("%9.6f %9.6f %9.6f\n"), pnt[0], pnt[1], pnt[2]);
		ar.WriteString(headinformation);
		}
	_stprintf_s(headinformation,TEXT("**end cloud** %s\n"), GetName());
	ar.WriteString(headinformation);
	return TRUE;
	}

BOOL XCloud::SaveFile_Txt(CArchive &ar)const
	{
	PNT pnt;
	TCHAR  pntstring[100];
	if(!this->m_bRawdata)
		return FALSE;
	int num = GetPntNum();
	TCHAR headinformation[256];
	wsprintf(headinformation,TEXT("//Cloud name:%s\n"), this->GetName());
	ar.WriteString(headinformation);
	wsprintf(headinformation,TEXT("//Point number:%d\n"),num);
	ar.WriteString(headinformation);
	if(this->GetUnit() == UNIT_INCH)
		ar.WriteString(TEXT("//Unit: inch\n"));
	if(this->GetUnit()== UNIT_MM)
		ar.WriteString(TEXT("//Unit: mm\n"));
	for (int j=0;j<num; j++)
		{
		pnt = (*this)[j];
		wsprintf(pntstring,TEXT("%9.6f, %9.6f, %9.6f,%9.6f, %9.6f,"), pnt[0], pnt[1], pnt[2],m_SNR[j], m_Total[j]);
		ar.WriteString(pntstring);
		ar<<'\n';			
		}
	wsprintf(pntstring,TEXT("**end cloud** %s"), GetName());
	ar.WriteString(pntstring);
	ar<<'\n';
	return TRUE;
	}

BOOL XCloud::SaveFile_Asc(FILE*&  savedfile)const
	{
	if( savedfile == NULL)
		{		
		return false;
		}
	char str[150];
	char  pntstring[100];
	DWORD cchBuff = 100;
	char headinformation[256];
	//fputs("//***********************File Header***********************************\n", savedfile);
	//		GetComputerName(pntstring, &cchBuff);
	//		sprintf(headinformation,"//computer name:%s\n", pntstring);
	//		fputs(headinformation, savedfile);
	//		GetUserName(pntstring, &cchBuff);
	//		sprintf(headinformation,"//user  name:%s\n", pntstring);
	//		fputs(headinformation, savedfile);
	//		SYSTEMTIME time;
	//		GetSystemTime(&time);
	//		sprintf(headinformation,"//Create time:%d/%d/%d\n", time.wDay,time.wMonth,time.wYear);
	//		fputs("// Version: 1.0\n", savedfile);
	//		fputs(headinformation, savedfile);
	//		sprintf(headinformation,"//%s\n", "Unit: inch");
	//		fputs(headinformation, savedfile);
	//		fputs("//***********************File Header end*********************************\n", savedfile);
	int num = GetPntNum();
	sprintf(headinformation,"//Cloud name:%s\n", this->GetName());
	fputs(headinformation, savedfile);
	sprintf(headinformation,"//Point number:%d\n",num);
	fputs(headinformation, savedfile);
	if(this->GetUnit() == UNIT_INCH)
		fputs("//Unit: inch\n", savedfile);
	if(this->GetUnit()== UNIT_MM)
		fputs("//Unit: mm\n", savedfile);
	for(int i =0; i< num; i++)
		{
		sprintf(str, "%9.6f  %9.6f  %9.6f\n ", m_Point[i][0], m_Point[i][1], m_Point[i][2]);
		fputs(str, savedfile);
		}
	sprintf(str, "** end cloud **%s", this->m_Name);
	fputs(str, savedfile);
	fclose(savedfile);
	return true;
	}

	BOOL XCloud::LoadFile_Dat(CArchive &ar)
	{
		PNT pnt;
		m_iSampleCoef = 1;
		CString  pntstring, unitstring;
		CString filename;
		float pixelunit;
		int num = 0 ;
		bool bflag = false;  // non standard file format 
		this ->InitObject();
		//unit:  micron   1 pixel = 3.75 micron
		ar.ReadString(pntstring);
		if(2 !=_stscanf_s(pntstring, TEXT("//unit:  %s   1 pixel = %f micron"),unitstring.GetBuffer(10), &pixelunit))
				return false;
		pntstring = CString("micron");
		if(pntstring == unitstring)
			this->m_Unit = UNIT_MM;
		unitstring.ReleaseBuffer();
		while( ar.ReadString(pntstring) )
		{
			if(2==_stscanf_s(pntstring,TEXT("%f  %f "),  &pnt[0],& pnt[1]))
			{
				pnt[0] *=(pixelunit/1000);
				pnt[1] /= 1000;
				pnt[2] = 0;
				AddPoint( pnt);
			}		
		}
		return true;
	}

BOOL XCloud::LoadFile_Asc(CArchive &ar)
	{
	PNT pnt;
	m_iSampleCoef = 1;
	CString  pntstring;
	CString  endstring = TEXT("**end cloud**");
	CString filename;
	int num = 0 ;
	bool bflag = false;  // non standard file format 
	this ->InitObject();
	while( ar.ReadString(pntstring) )
		{
		if(3==_stscanf_s(pntstring,TEXT("%f %f %f"),  &pnt[0],& pnt[1], &pnt[2]))
			{
			AddPoint( pnt);
			}
		else
			{
			if(pntstring[0] =='/')
				{
				if(pntstring.Find(TEXT("Cloud name")) != -1)
					{
					_stscanf_s(pntstring,TEXT("//Cloud name:%s\t Point number:%d"), m_Name.GetBuffer(32), &num);
					this->SetCapacity(num);
					bflag = true; // standard file format
					}		
				if(pntstring.Find(TEXT("Unit:")) != -1)
					{
					CString unitstring;
					_stscanf_s(pntstring,TEXT("//Unit: %s"), unitstring.GetBuffer(32));
					unitstring.ReleaseBuffer();
					if(unitstring == "inch")
						this->SetUnit( UNIT_INCH );
					if(unitstring == "mm")
						this->SetUnit( UNIT_MM );
					}
				}

			if(pntstring.Find( endstring.GetBuffer()) != -1)
				{				
				break;
				}
			}
		}
	endstring.ReleaseBuffer();
	if(this->m_Name == "Cloud")
		{
		filename = ar.GetFile()->GetFileName();
		filename = filename.Left(filename.GetLength() - 4);
		this->SetName(filename);
		}
	m_Name.ReleaseBuffer();
	if(this->GetUnit() == UNIT_NONE)
		this ->SetUnit(UNIT_MM);
	m_bObjChanged = true;
	if(GetPntNum() > 0)
		return true;
	else
		return false;
	}

/*------------------------------------------------------------------
Txt file should be like following format:
61.295000,52.560952,-91.633882,250,630,
61.295000,52.561002,-91.639975,262,631,
61.295000,52.561002,-91.687886,196,633,
61.295000,52.561052,-91.630320,234,631,
61.295000,52.561102,-91.645391,232,632,
61.295000,52.561102,-91.598423,239,631,
61.295000,52.561102,-91.643805,244,630,
61.295000,52.561102,-91.639711,273,631,
61.295000,52.561102,-87.721077,116,631,
61.295000,52.561152,-91.640360,273,630,
61.295000,52.561202,-91.635268,265,631,
61.295000,52.561202,-91.588599,284,630,
----------------------------------------------------------
x_cord, y_cord, z_cord, snr value, total value,
------------------------------------------------------------------*/
BOOL XCloud::LoadFile_Txt(CArchive &ar)
	{
	PNT pnt;
	m_iSampleCoef = 1;
	float snr, total_power;
	int num;
	CString  pntstring, filename;
	CString  endstring = TEXT("**end cloud**");
	this ->InitObject();
	bool bflag = false; // if true for impeller data loading
	while( ar.ReadString(pntstring) )
		{
		if( (pntstring.Find(endstring.GetBuffer()) != -1) || (pntstring.GetLength()<4))
			break;
		if(5 == _stscanf_s(pntstring,TEXT("%f,%f,%f,%f,%f,"),  &pnt[0], &pnt[1], &pnt[2], &snr, &total_power))
			this->AddPoint( pnt, snr, total_power);
		else
			{
			if(pntstring[0] =='/')
				{
				if(pntstring.Find(TEXT("Cloud name")) != -1)
					{
					_stscanf_s(pntstring,TEXT("//Cloud name:%s\t Point number:%d"), m_Name.GetBuffer(32), &num);
					this->SetCapacity(num);
					bflag = true; // standard file format
					}		
				if(pntstring.Find(TEXT("Unit:")) != -1)
					{
					CString unitstring;
					_stscanf_s(pntstring,TEXT("//Unit: %s"), unitstring.GetBuffer(32));
					unitstring.ReleaseBuffer();
					if(unitstring == "inch")
						this->SetUnit( UNIT_INCH );
					if(unitstring == "mm")
						this->SetUnit( UNIT_MM );
					}
				}
			if(pntstring.Find( endstring.GetBuffer()) != -1)
				{				
				break;
				}
			}
		}
	endstring.ReleaseBuffer();
	if( (pntstring.Find(endstring.GetBuffer()) != -1) )
		{
		pntstring = pntstring.TrimLeft(endstring);
		this->SetName(pntstring);
		}
	else
		{
		filename = ar.GetFile()->GetFileName();
		filename = filename.Left(filename.GetLength() - 4);
		this->SetName(filename);
		}
	if(!bflag)
		this->SetUnit(UNIT_MM);
	m_bObjChanged = true;
	if(GetPntNum() > 0)
		return true;
	else
		return false;
	}

BOOL XCloud::LoadFile_Mini_Txt(CArchive &ar)
{
	PNT pnt;
	m_iSampleCoef = 1;
	CString  pntstring, filename;
	CString  unitstring,us1,us2;
	int LNum = 0;
	this ->InitObject();
	bool bflag = false; // if true for impeller data loading
	ar.ReadString(unitstring);
	//set unit
	//sscanf(unitstring,"%s %s", us1.GetBuffer(10),us2.GetBuffer(10));
	//us1.ReleaseBuffer();
	//us2.ReleaseBuffer();
	//if(us1 == "uin" || us1 == "UIN")
	//	this->SetUnit(UNIT_UIN);
	//if(us1 == "in" || us1 == "inch" || us1 == "INCH" || us1 == "IN")
	//	this->SetUnit(UNIT_INCH);
	//if(us1 == "mm" || us1 == "MM")
	//	this->SetUnit(UNIT_MM);

	//-----------------   File Parsing -----------------------//
	//anyway, set it to inch. then parse the file head
	
	int fileunit = -1; bool bodyend = false;
	if(unitstring.Find(TEXT("uin")) != -1 || unitstring.Find(TEXT("UIN")) != -1) 
	{
		fileunit = UNIT_UIN;this->SetUnit(UNIT_UIN);
	}
	else if(unitstring.Find(TEXT("in")) != -1 || unitstring.Find(TEXT("IN")) != -1) 
	{
		fileunit = UNIT_INCH;this->SetUnit(UNIT_INCH);
	}
	else
	{
		return false;
	}
	ar.ReadString(pntstring);
	if( pntstring.Find('#') != -1 )
	{
		pntstring.Remove('#');
		if(1 != _stscanf_s(pntstring,TEXT("%d"),  &LNum))
		{
			bodyend = true;
		}
	}
	else if(pntstring.Find(',') != -1)
	{
			pntstring.Remove(' ');	
			pntstring.Remove('\t');	
			if(2 == _stscanf_s(pntstring,TEXT("%f,%f"),  &pnt[0], &pnt[1]))
				this->AddPoint( pnt);
			else
				return false;
		
	}else if(1 != _stscanf_s(pntstring,TEXT("%d"),  &LNum))
	{
		return FALSE;
	}
	
	if(!bodyend)
	{
		while( ar.ReadString(pntstring) )
		{
			if(pntstring.Find('#') != -1) 
			{
				bodyend = true; break;
			}
			pntstring.Remove(' ');	
			pntstring.Remove('\t');	
			if(2 == _stscanf_s(pntstring,TEXT("%f,%f"),  &pnt[0], &pnt[1]))
				this->AddPoint( pnt);
			else
				break;
		}
	}
	//now parsing the annotation at the end of file, like this one
	//#Badge Num 58397
	//#Plan Name plan
	//#Ser Num serno
	//#Part Num partno
	//#label 1 
	//#label 2 
	//#Roundedge Tol. 
	//#Max Val 0.040000
	//#Min Val 0.010000
	if(bodyend)
	{
	}

	//Feb 17 2006 ***********************//
	double ymax=-111111111111111111;
	if(m_Point[0][1] > ymax) ymax = m_Point[0][1];

	if(m_Point[this->GetPntNum()-1][1] > ymax) ymax= m_Point[this->GetPntNum()-1][1];

	if(ymax < m_Point[this->GetPntNum()/2][1])
	{
		for(int ii=0; ii< this->GetPntNum(); ii++)
		{
			m_Point[ii][1] *= -1;
		}
	}

	SetUnit(UNIT_INCH);
	filename = ar.GetFile()->GetFileName();
	filename = filename.Left(filename.GetLength() - 4);
	this->SetName(filename);
	this->EstimatedSampleDistance();
	m_bObjChanged = true;
	if(GetPntNum() > 0)
		return true;
	else
		return false;
}

void XCloud::EstimatedSampleDistance() //n change for autotrim, check the differes for the Calculator
{
	float val=0;
	//double  sample_dist1,sample_dist2;  //n change for autotrim
	int npts = this->GetPntNum();
	sample_dist1=0;//n change for autotrim sample_dist=0;
	sample_dist2=0;
	minyvalue=100000;
	maxyvalue=0;
	for(int i=1;i<npts;i++)
	{
		if (maxyvalue<m_Point[i][1]) maxyvalue=m_Point[i][1];
		if (minyvalue>m_Point[i][1]) minyvalue=m_Point[i][1];
	}
	for(int i=.25*npts;i<.5*npts;i++)
		sample_dist1 +=sqrt(SQ(m_Point[i][0]-m_Point[i-1][0])+SQ(m_Point[i][1]-m_Point[i-1][1]));
		for(int i=.5*npts;i<.75*npts;i++)
		sample_dist2 +=sqrt(SQ(m_Point[i][0]-m_Point[i-1][0])+SQ(m_Point[i][1]-m_Point[i-1][1]));

		//if (sample_dist1<sample_dist2)
		sample_dist1 =sample_dist1/npts*2;
		//else
		sample_dist2 =sample_dist2/npts*2;
		sample_dist=(sample_dist1+sample_dist2)/2;
	return ;
}
void XCloud::AddPoint( const PNT& aVector3)
	{
	m_Point.push_back( aVector3 );
	this->m_bObjChanged = true;
	}

void XCloud::AddPoint(PNT& aVector3, float aSNR, float aTotal)
	{
	m_Point.push_back( aVector3 );
	m_SNR.push_back(aSNR);
	m_Total.push_back(aTotal);
	m_bRawdata = 2;
	this->m_bObjChanged = true;
	}

void XCloud::DeletePoint(const int& startindex, const int & num)
	{
	m_Point.erase(m_Point.begin()+startindex, m_Point.begin() + startindex+ num);
	int tSize = m_Point.size();
	if(m_bRawdata==2)
		{
		m_SNR.erase(m_SNR.begin()+startindex, m_SNR.begin() + startindex+ num);
		m_Total.erase(m_Total.begin()+startindex, m_Total.begin() + startindex+ num);
		}
	this->m_bObjChanged = true;
	}



const  PNT& XCloud:: operator[](int index) const  // return point with index 
	{
	return m_Point[index];
	}
PNT& XCloud:: operator[](int index)   // return point with index 
	{
	return this->m_Point[index];
	}
void XCloud::SetCapacity(int num)
	{
	if(num > GetPntNum())
		{
		m_Point.reserve(num);
		}
	if(this->m_bRawdata==2)
		{
		m_SNR.reserve(num);		
		m_Total.reserve(num);
		}
	}


void XCloud::UpdateBndBox()
{	
	int i;
	if(m_bObjChanged)
	{
		m_BndBox.Reset();
		int Num = GetPntNum();
		for(i = 0; i < Num; i++)
		{		
			m_BndBox.AddVertex( m_Point[i] );
		}
		PNT & min = m_BndBox.GetMin();
		PNT & max = m_BndBox.GetMax();
		for( i=0;i<3;i++)
		{
			if(max[2]-min[2]<0.1) 
			{
				min[i] -= 0.005;
				max[i] += 0.005;
			}
		}
		m_bObjChanged = false;
	}
}

void XCloud::SetUnit(const int& unit)
	{
	if(this->m_Unit == UNIT_NONE|| m_Unit == unit)
		{
		m_Unit = unit;
		return;
		}
	int i;
	int num = GetPntNum();
	switch(m_Unit)// suppose only two type unit(inch and mm)
		{
		case UNIT_MM:
			if(unit == UNIT_INCH)
				{
				for(i =0; i < num; i++)
					{
					m_Point[i]/=25.4;
					}
				}
			if(unit == UNIT_UIN)
				{
				for(i =0; i < num; i++)
					{
					m_Point[i]/=(0.000001*25.4);
					}
				}
			break;
		case UNIT_INCH:
			if(unit == UNIT_MM)
				{
				for(i =0; i < num; i++)
					{
					m_Point[i]*=25.4;
					}
				}
			if(unit == UNIT_UIN)
				{
				for(i =0; i < num; i++)
					{
					m_Point[i]*=1000000;
					}
				}
			break;
		case UNIT_UIN:
			if(unit == UNIT_MM)
				{
				for(i =0; i < num; i++)
					{
					m_Point[i]/=(1000000/25.4);
					}
				}
			if(unit == UNIT_INCH)
				{
				for(i =0; i < num; i++)
					{
					m_Point[i]/=1000000;
					}
				}
			break;
		default:
			;
		}
	m_bObjChanged = true;
	}

void XCloud::Translate(const PNT& delta_trans)
{
	int num = GetPntNum();
	for(int i=0; i< num; i++)
	{
		m_Point[i] = m_Point[i] + delta_trans;
	}
	m_bObjChanged = true;
}
void XCloud::Rotate(const Ti_Rotation& R)
{
	this->Rotate(PNT(0,0,0), R.m_Axis, R.m_Angle*PI/180);
}

void XCloud::Rotate(
					const PNT	& pnt_axis,	 // point on axis
					const PNT	& dir,		// unit direction vector
					const double		& angle		// rotation angle in rad
					)
{
	double s_thita, c_thita, vers_thita;
	if(fabs(angle) < 1e-6)
		return;
	s_thita = sin(angle);
	c_thita = cos(angle);
	vers_thita = 1- c_thita;
	double  trans[3][3];
	double x, y, z; 
	x = dir[0];
	y = dir[1];
	z = dir[2];

	trans[0][0] = x*x*vers_thita+c_thita;
	trans[0][1] = y*x*vers_thita -z*s_thita;
	trans[0][2] = z*x*vers_thita + y*s_thita;	  
	trans[1][0] = x*y*vers_thita + z*s_thita;
	trans[1][1] = y*y*vers_thita + c_thita;
	trans[1][2] = y*z*vers_thita - x*s_thita;
	trans[2][0] = x*z*vers_thita - y*s_thita;
	trans[2][1] = y*z*vers_thita +x*s_thita;
	trans[2][2] = z*z*vers_thita + c_thita;

	int num = GetPntNum();
	PNT pnt;
	for(int i=0; i< num; i++)
	{
		pnt = this->m_Point[i];
		pnt = pnt - pnt_axis;
		m_Point[i][0] =	
			trans[0][0]*pnt[0] +
			trans[0][1]*pnt[1] +
			trans[0][2]*pnt[2] ;
		m_Point[i][1] =
			trans[1][0]*pnt[0] +
			trans[1][1]*pnt[1] +
			trans[1][2]*pnt[2] ;
		m_Point[i][2] =	
			trans[2][0]*pnt[0] + 
			trans[2][1]*pnt[1] + 
			trans[2][2]*pnt[2] ;
		m_Point[i] = m_Point[i] + pnt_axis;
	}
	m_bObjChanged = true;
}	

void XCloud::SampleUniform(int interval_num)
{
	int num = GetPntNum();
	XCloud  tmpcld(*this);
	tmpcld.InitObject();
	tmpcld.SetCapacity(num/interval_num + 1);
	for( int i =0 ; i< num ; i += interval_num)
	{
		tmpcld.AddPoint((*this)[i]);
	}
	tmpcld.m_bObjChanged = true;
	tmpcld.m_bDispChanged = true;
	tmpcld.UpdateBndBox();
	*this = tmpcld;
}
void XCloud::AdaptiveFilter(double sampedist, double angle)
	{
	XCloud cld(*this),  newcld;
	newcld = *this;
	newcld.InitObject();
	double cos_theta = cos(angle*PI/180.);
	double d1, d2,  angle_cos;
	int num = cld.GetPntNum();
	if(num < 100)
		return;
	int step = 2;
	newcld.SetCapacity(num);
	newcld.AddPoint(cld[0]);
	for(int i = 1 ; i < num-1; i+= step)
		{
		if(step ==2)
			{
			d1 = (cld[i]-cld[i-1]).Length();
			d2 = (cld[i+1]-cld[i]).Length();
			}
		else
			{
			d1 = d2;
			d2 = (cld[i+1]-cld[i]).Length();
			}		
		if(d1> sampedist && d2 < sampedist)
			{
			step =1;
			newcld.AddPoint(cld[i]);
			continue;
			}
		if( d2 > sampedist)
			{
			step =2;
			newcld.AddPoint(cld[i]);
			newcld.AddPoint(cld[i+1]);
			continue;
			}
		angle_cos = (cld[i]-cld[i-1])*(cld[i+1]-cld[i])/(d1*d2);
		if(angle_cos < cos_theta)
			{
			step =1;
			newcld.AddPoint(cld[i]);
			}
		if(i==num -2)
			{
			newcld.AddPoint(cld[i+1]);
			}
		}
	*this = newcld;
	}

	void XCloud::data_smoothing(int iter)
	{	 
		//首先在两侧角上一部分点
		int pts_num=GetPntNum();
		if(pts_num==0)
			return;
		int addpntnum=pts_num/10;
		XCloud tempcld1;
		for(int i=addpntnum;i>0;i--)
			tempcld1.AddPoint( (*this)[0]*2-(*this)[i] );
		for(int i=0;i<pts_num;i++)
			tempcld1.AddPoint( (*this)[i] );
		for(int i=pts_num-2;i>=pts_num-addpntnum-1;i--)
			tempcld1.AddPoint( (*this)[pts_num-1]*2-(*this)[i] );
		//处理数据
		pts_num=tempcld1.GetPntNum();
		int j;
		double * Orig_x=new double[pts_num];
		double * Orig_y=new double[pts_num];
		double * Orig_z=new double[pts_num];
		PNT tPt1;
		//-
		if (Orig_x==NULL || Orig_y==NULL|| Orig_z==NULL)
		{
			AfxMessageBox(TEXT("Orig_x or Orig_y's memory error!"));
			return;
		}
		for (j=0; j<pts_num; j++)
		{
			tempcld1.GetPoint(j,tPt1);
			Orig_x[j]=tPt1[0];
			Orig_y[j]=tPt1[1];
			Orig_z[j]=tPt1[2];
		}
		Smoothing_ANID(Orig_x,pts_num,0,iter);
		Smoothing_ANID(Orig_y,pts_num,1,iter);
		Smoothing_ANID(Orig_z,pts_num,2,iter);
		for (j=0; j<pts_num; j++)
		{
			tempcld1[j][0]=Orig_x[j];
			tempcld1[j][1]=Orig_y[j];
			tempcld1[j][2]=Orig_z[j];
		}
		delete []Orig_x;
		delete []Orig_y;
		delete []Orig_z;
		//转存到this里面
		this->InitObject();
		for(int i=addpntnum;i<pts_num-addpntnum;i++)
			this->AddPoint(tempcld1[i]);
	}

	int XCloud::Smoothing_ANID(const PNT& smoothing_dir)
	{
		double rotation_angle;
		PNT   rotation_axis;
		if(smoothing_dir[2] < 1)
		{
			rotation_angle = acos(smoothing_dir[2]);
			rotation_axis = smoothing_dir^(PNT(0,0,1));
			this->Rotate(PNT(0,0,0), rotation_axis, rotation_angle);
		}
		int j,count=0;
		int iter=5;
		double dt=0.25, K;
		double DataL,DataR, CL, CR;
		double ImdValDif, ImdValDifN;
		double sumdot, sumnoise, sumdiff;
		// corr[0]: last corr; corr[1]; current corr;
		double difference,corr[2]={1,1};

		int mNum = GetPntNum();
		double * Orig_Z=new double[mNum];
		for (j=0; j<mNum; j++)
		{
			Orig_Z[j]= (*this)[j][2];
		}
		//Orig_Z=m_Z;
		K=1.4826*Para_K();
		//SimpleArray  prsd_Z(mNum);
		do 
		{
			corr[0]=corr[1];
			count +=1;
			DataL=(*this)[1][2]-(*this)[0][2];
			DataR=(*this)[1][2]-(*this)[0][2];
			CL=exp(-(fabs(DataL)/K)*(fabs(DataL)/K));
			CR=exp(-(fabs(DataR)/K)*(fabs(DataR)/K));

			// compute the diffusion result
			///		ImdValDif=m_Z[1]+dt*(CL*DataL +CR*DataR);
			ImdValDif=(*this)[1][2]+dt*(CL*DataL +CR*DataR);
			ImdValDifN=ImdValDif;
			for (j=1;j<mNum-1;j++)
			{
				DataL=(*this)[j-1][2]-(*this)[j][2];
				DataR=(*this)[j+1][2]-(*this)[j][2];
				CL=exp(-(fabs(DataL)/K)*(fabs(DataL)/K));
				CR=exp(-(fabs(DataR)/K)*(fabs(DataR)/K));
				ImdValDif=(*this)[j][2]+dt*(CL*DataL+CR*DataR);
				(*this)[j-1][2]=ImdValDifN;
				ImdValDifN=ImdValDif;
			}
			//process the right boundary
			DataL=(*this)[mNum-2][2]-(*this)[mNum-1][2];
			DataR=(*this)[mNum-2][2]-(*this)[mNum-1][2];
			CL=exp(-(fabs(DataL)/K)*(fabs(DataL)/K));
			CR=exp(-(fabs(DataR)/K)*(fabs(DataR)/K));
			(*this)[mNum-1][2]=(*this)[mNum-1][2]+dt*(CL*DataL+CR*DataR);

			// computer the stop time
			sumdot=0, sumnoise=0, sumdiff=0;
			for (j=0;j<mNum;j++)
			{
				difference=Orig_Z[j]-(*this)[j][2];
				sumdot +=fabs(difference*(*this)[j][2]);
				sumnoise +=pow(difference,2);
				sumdiff +=pow((*this)[j][2],2);
			}
			corr[1]=sumdot/(sqrt(sumnoise)*sqrt(sumdiff));
			if (count>iter) 
			{
				this->m_bObjChanged = true;
				if(smoothing_dir[2] < 1)
				{
					this->Rotate(PNT(0,0,0), rotation_axis, -rotation_angle);
				}
				delete [] Orig_Z;
				return count;
			}
		}while (corr[1]-corr[0]<0.01);
		delete [] Orig_Z;
		this->m_bObjChanged = true;
		if(smoothing_dir[2] < 1)
		{
			this->Rotate(PNT(0,0,0), rotation_axis, -rotation_angle);
		}

		return count;
	}

	int XCloud::Smoothing_ANID(double* array, int pts_num,int axis,int iter)
	{
		int j,count=0;	
		double dt=0.25, K=5;
		double DataL,DataR, CL, CR;
		double ImdValDif, ImdValDifN;
		double sumdot, sumnoise, sumdiff;
		// corr[0]: last corr; corr[1]; current corr;
		double difference,corr[2]={1,1};
		double * Orig=new double[pts_num];
		if (Orig== NULL)
		{
			AfxMessageBox(TEXT("Orig_z's memeory error in the smoothing_ANID funciton!"));
			exit(1);
		}
		for (j=0; j<pts_num; j++)
		{
			Orig[j]=array[j];
		}

		K=100;	
		//K=Para_K(axis);
		do 
		{		
			corr[0]=corr[1];
			count +=1;
			// process the left boundary
			DataL=array[1]-array[0]; 
			//DataR=array[1]-array[0]; 
			CL=exp(-(fabs(DataL)/K)*(fabs(DataL)/K));
			//CR=exp(-(fabs(DataR)/K)*(fabs(DataR)/K));
			// compute the diffusion result
			ImdValDif=array[0]+dt*(CL*DataL);//+CR*DataR);
			ImdValDifN=ImdValDif;
			for (j=1;j<pts_num-1;j++)
			{
				DataL=array[j-1]-array[j];
				DataR=array[j+1]-array[j];
				CL=exp(-(fabs(DataL)/K)*(fabs(DataL)/K));
				CR=exp(-(fabs(DataR)/K)*(fabs(DataR)/K));
				ImdValDif=array[j]+dt*(CL*DataL+CR*DataR);
				array[j-1]=ImdValDifN;
				ImdValDifN=ImdValDif;
			}
			//process the right boundary
			DataL=array[pts_num-2]-array[pts_num-1];
			//DataR=array[pts_num-2]-array[pts_num-1];
			CL=exp(-(fabs(DataL)/K)*(fabs(DataL)/K));
			//CR=exp(-(fabs(DataR)/K)*(fabs(DataR)/K));
			array[pts_num-1]=array[pts_num-1]+dt*(CL*DataL);//+//CR*DataR);
			// computer the stop time
			sumdot=0, sumnoise=0, sumdiff=0;
			for (j=0;j<pts_num;j++)
			{
				difference=Orig[j]-array[j];
				sumdot +=fabs(difference*array[j]);
				sumnoise +=pow(difference,2);
				sumdiff +=pow(array[j],2);
			}
			if(sumnoise==0.0 || sumdiff==0.0)
			{
				corr[1] = 1.0;
				continue;
			}
			corr[1]=sumdot/(sqrt(sumnoise)*sqrt(sumdiff));
			if (count>iter)
			{ 
				delete [] Orig; 
				return count;
			}
		}while (fabs(corr[1]-corr[0])>1e-4);
		delete [] Orig;
		return count;
	}

double XCloud::Para_K()
{
	int j;

	int mNum = GetPntNum();

	double *Gra_Z=new double[mNum];
	///	Gra_Z[0]=fabs(m_Z[1]-m_Z[0]); 
	Gra_Z[0]=fabs((*this)[1][2]-(*this)[0][2]); 
	for (j=1;j<mNum;j++)
	{
		///		Gra_Z[j]=fabs(m_Z[j-1]-m_Z[j]);
		Gra_Z[j]=fabs((*this)[j-1][2]-(*this)[j][2]);
	}

	double meanval=0;
	for (j=0;j<mNum;j++)
	{
		meanval+=Gra_Z[j];
	}
	meanval /=mNum;
	double meangra=0;	
	for (j=0;j<mNum;j++)
	{
		meangra +=fabs(Gra_Z[j]-meanval);
	}
	meangra /=mNum;
	delete [] Gra_Z;
	//Bubble(Gra_Z,mNum);
	return  meangra;

}

void XCloud::SampleSpace(float distance)
{
	InitialBin(distance);
	int num = GetPntNum();
	XCloud  tmpcld(num);
	tmpcld.SetName(m_Name);
	tmpcld.SetDispType(m_DispType);
	tmpcld.SetColor(m_Color);
	tmpcld.SetVisible(this->m_bShow);

	PNT tempVector;
	int pntIndex = 0;
	double d1,d;
	for (int i=0,j,k;i<m_iBinNum;i++)
	{
		if (m_BinArray[i].m_bNoEmpty)
		{
			d1 = 2 * distance;
			pntIndex = m_BinArray[i].m_InnerPoints.GetHead();
			POSITION pos = m_BinArray[i].m_InnerPoints.GetHeadPosition();
			for (j=0;j<m_BinArray[i].m_InnerPoints.GetCount();j++)
			{
				k = m_BinArray[i].m_InnerPoints.GetNext(pos);
				tempVector = (*this)[k]-m_BinArray[i].m_CenterPoint;
				d = tempVector.Length();
				if(d < d1)
				{
					pntIndex = k;
					d1 = d;
				}
			}
			tmpcld.AddPoint((*this)[pntIndex]);
		}
	}
	tmpcld.m_bObjChanged = true;
	tmpcld.UpdateBndBox();
	*this = tmpcld;

}

void XCloud::InitialBin(float & binLength)
{
	if(m_BinArray!=NULL) 
		delete []m_BinArray;
	if(binLength<1e-6)
		binLength = 1e-6;
	int xNum,yNum,zNum;
	int pointNum = this->GetPntNum();
	XBndBox box = GetBBox();
	PNT min = box.GetMin();
	PNT max = box.GetMax();
	PNT temp(max-min);
	//if(temp.Length()/binLength>200) binLength *= temp.Length()/(binLength*200);
	xNum = temp[0]/binLength + 1;
	yNum = temp[1]/binLength + 1;
	zNum = temp[2]/binLength + 1;
	m_iBinNum = xNum*yNum*zNum;
	//m_iBinNum = xNum*yNum*zNum-xNum+zNum;
	m_BinArray = new XBinBox[m_iBinNum];
	for(int i=0,j,k;i<zNum;i++)
	{
		for(j=0;j<yNum;j++)
		{
			for(k=0;k<xNum;k++)
			{	
				m_BinArray[i*xNum*yNum+j*xNum+k].m_CenterPoint[0] = (k+0.5)*binLength+min[0];
				m_BinArray[i*xNum*yNum+j*xNum+k].m_CenterPoint[1] = (j+0.5)*binLength+min[1];
				m_BinArray[i*xNum*yNum+j*xNum+k].m_CenterPoint[2] = (i+0.5)*binLength+min[2];
			}
		}
	}
	int x, y, z;
	for(int i=0; i<pointNum;i++)
	{
		x = (this->m_Point[i][0]-min[0])/binLength;
		y = (m_Point[i][1]-min[1])/binLength;
		z = (m_Point[i][2]-min[2])/binLength;
		ASSERT(x>=0 && y >=0 && z >= 0);
		m_BinArray[z*xNum*yNum+y*xNum+x].m_InnerPoints.AddTail(i);
		m_BinArray[z*xNum*yNum+y*xNum+x].m_bNoEmpty = true;
	}

}

void XCloud::InitialBin2D(int numX,int numY)
{
	if(m_BinArray!=NULL) 
		delete []m_BinArray;
	int pointNum = this->GetPntNum();
	XBndBox box = GetBBox();
	PNT min = box.GetMin();
	PNT max = box.GetMax();
	PNT temp(max-min);
	float binLengthX = temp[0]/numX;
	float binLengthY = temp[1]/numY;
	m_iBinNum = numX*numY;
	m_BinArray = new XBinBox[m_iBinNum];

	for(int i=0,x,y;i<pointNum;i++)
	{
		x = (m_Point[i][0]-min[0])/binLengthX;
		y = (m_Point[i][1]-min[1])/binLengthY;
		if(x>numX-1) 
			x=numX-1;
		if(y>numY-1) 
			y=numY-1;
		m_BinArray[y*numX+x].m_InnerPoints.AddTail(i);
	}
}

void XCloud::InitialBin2D(int num)
{
	if(m_BinArray!=NULL) 
		delete []m_BinArray;
	int pointNum = m_Point.size();
	XBndBox box = this->GetBBox();
	PNT min = box.GetMin();
	PNT max = box.GetMax();
	PNT temp(max-min);
	float binLengthX = temp[0]/num;
	float binLengthY = temp[1]/num;
	m_iBinNum = num*num;
	m_BinArray = new XBinBox[m_iBinNum];

	for(int i=0,x,y;i<pointNum;i++)
	{
		x = (m_Point[i][0]-min[0])/binLengthX;
		y = (m_Point[i][1]-min[1])/binLengthY;
		if(x>num-1) 
			x=num-1;
		if(y>num-1) 
			y=num-1;
		m_BinArray[y*num+x].m_InnerPoints.AddTail(i);
	}
}

void XCloud::CloudInBin2D(int i,XCloud & cld)
{
	cld.InitObject();
	if(i>m_iBinNum) return;
	int pntNum = m_BinArray[i].m_InnerPoints.GetCount();
	for(int k=0;k<pntNum;k++)
	{
		cld.AddPoint(m_Point[m_BinArray[i].m_InnerPoints.GetAt(m_BinArray[i].m_InnerPoints.FindIndex(k))]);
	}
	cld.m_bObjChanged = true;
}

void XCloud::SamplePercentage(int percent)
{

	if(percent==0) return;
	int num = GetPntNum();
	int  reduceTime;
	int i,j, increment, start;
	if(num<=1) 
		reduceTime = 1;
	else
		reduceTime = logf(num)/logf(2);

	float * decomposeNo = new float[reduceTime];

	for( i=0;i<reduceTime;i++)
	{
		decomposeNo[i] = 1;
	}

	float fPercent = 1 - float(percent)/100.;
	float endValue = 0.5,leftPercent; 
	leftPercent = fPercent;
	for( i=0;i<reduceTime;i++)
	{
		leftPercent -= endValue;

		if(leftPercent < 0)
		{	
			leftPercent += endValue;
			decomposeNo[i] = 0;
		}
		endValue *= 0.5;
	}

	XCloud  tmpcld(num);
	tmpcld.SetName(m_Name);
	tmpcld.SetDispType(m_DispType);
	tmpcld.SetColor(m_Color);
	tmpcld.SetVisible(m_bShow);

	for( i=0,j,increment=2,start=0;i<reduceTime;i++)
	{
		if(decomposeNo[i]==1)
		{
			for(j=0;j<num;j += increment)
			{
				tmpcld.AddPoint(m_Point[j]);		
			}
		}
		start = 2*start+1;
		increment *= 2;
	}
	tmpcld.m_bObjChanged = true;
	tmpcld.UpdateBndBox();
	*this = tmpcld;
	delete []decomposeNo;

}


PNT XCloud::GetMassCenter(void)
{
	PNT    C;
	int num = this->GetPntNum();
	for(int i=0; i < num; i++)
	{
		C += this->m_Point[i];
	}
	return C/num;
}


double XCloud::Fit2PlanePointRejection(PNT& PntPlane, PNT& DirPlane, double Tolerance, double& PointRatio) const 
{
	XCloud cld = *this;
	double rmsError ;
	rmsError = cld.Fit2Plane(PntPlane, DirPlane);

	double dist = 5;
	int counter =0;
	PNT pntPlane, dirPlane;
	while(1)
	{
		counter++;
		if (counter> 10)
		{
			break;
		}
		if (dist < 5e-5)
		{
			break;
		}
		cld.InitObject();
		for (int i = 0;i < GetPntNum(); i++)
		{
			if(fabs(( m_Point[i] - PntPlane)*DirPlane) < Tolerance)
			{
				cld.AddPoint(m_Point[i]);
			}
		}// end for
		rmsError = cld.Fit2Plane(pntPlane,dirPlane);
		dist = (pntPlane-PntPlane).Length();
		//angle = acos(dirPlane*DirPlane)*180/PIE;
		PntPlane = pntPlane;
		DirPlane = dirPlane;
	}
	PointRatio = cld.GetPntNum();
	//rmsError = cld.Fit2Plane(PntPlane, DirPlane);
	PointRatio /= GetPntNum();
	return rmsError;
}
double XCloud::Fit2Plane(PNT& m_Pnt_plane, PNT& norm_dir) const 
{
	int i, num;
	PNT pnt;
	XCloud cld(*this);
	Ti_Matrix<double> matrix(3,3),	U(3,3);
	double w[3]; 
	num = cld.GetPntNum();
	m_Pnt_plane[0] = 0;
	m_Pnt_plane[1] = 0;
	m_Pnt_plane[2] = 0;
	for(i =0; i< num; i++)
	{
		m_Pnt_plane += cld[i];
	}
	m_Pnt_plane /= num;
	cld.Translate(-m_Pnt_plane);
	for(i=0; i< num; i++)
	{
		cld.GetPoint(i, pnt);
		matrix.m[0][0] += pnt[0]*pnt[0];
		matrix.m[0][1] += pnt[0]*pnt[1];
		matrix.m[0][2] += pnt[0]*pnt[2];
		matrix.m[1][0] += pnt[1]*pnt[0];
		matrix.m[1][1] += pnt[1]*pnt[1];
		matrix.m[1][2] += pnt[1]*pnt[2];
		matrix.m[2][0] += pnt[2]*pnt[0];
		matrix.m[2][1] += pnt[2]*pnt[1];
		matrix.m[2][2] += pnt[2]*pnt[2];
	}	
	matrix.JacobiN(3, w, U);
	norm_dir[0] = U.m[0][2];
	norm_dir[1] = U.m[1][2];
	norm_dir[2] = U.m[2][2];
	matrix.Destroy();
	U.Destroy();
	return sqrt(w[2] / num);

}

double XCloud::Fit2Quadratic(double A[],bool bReject,int & RFlag)const
{	
	// initialize the original value
	int num;
	XCloud  cld = *this;
	if(cld.GetPntNum() < 5)
	{
		return -1.0 ; // fitting failed for less of points
	}
	bool bSucceed = false;
	int i = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	XCloud remaincld, tmpcld;		
//	double rmserror=0;
	cld.CFit2Quadratic(A);
//	tmpcld = cld;

	//PNT tmppt3;
	//double t1,t2,t3,roots[5],tmp,UF,DF;
	//int RF;
	////Ti_Optimization   algorithm;
	////algorithm.

	////for(i=0;i<10;i++)
	////{	
	////	tmpcld.GetPoint(Height/2*Width+i*Width/10,tmppt3);	
	////	t1=A[2];
	////	t2=A[4]*tmppt3.m[0]+A[5]*tmppt3.m[1]+A[8];
	////	t3=A[0]*tmppt3.m[0]*tmppt3.m[0]+A[1]*tmppt3.m[1]*tmppt3.m[1]+A[3]*tmppt3.m[0]*tmppt3.m[1]+A[6]*tmppt3.m[0]+A[7]*tmppt3.m[1]+A[9];
	////	roots=TM->SolveQuadratic(t1, t2, t3);
	////	if(roots[0]!=0)
	////	{
	////		UF=UF+fabs(roots[1]-tmppt3.m[2]);
	////		DF=DF+fabs(roots[2]-tmppt3.m[2]);
	////	}   

	////	tmpcld3.GetPoint(i*Height/10*Width+Width/2,tmppt3);
	////	t1=A[2];
	////	t2=A[4]*tmppt3.m[0]+A[5]*tmppt3.m[1]+A[8];
	////	t3=A[0]*tmppt3.m[0]*tmppt3.m[0]+A[1]*tmppt3.m[1]*tmppt3.m[1]+A[3]*tmppt3.m[0]*tmppt3.m[1]+A[6]*tmppt3.m[0]+A[7]*tmppt3.m[1]+A[9];
	////	roots=TM->SolveQuadratic(t1, t2, t3);
	////	if(roots[0]!=0)
	////	{
	////		UF=UF+fabs(roots[1]-tmppt3.m[2]);
	////		DF=DF+fabs(roots[2]-tmppt3.m[2]);
	////	}   
	////}

	////if(UF>DF)
	////	RF=2;
	////else
	////	RF=1;


	////for(i=0;i<num;i++)
	////{
	////	tmpcld.GetPoint(i,tmppt3);
	////	t1=A[2];
	////	t2=A[4]*tmppt3.m[0]+A[5]*tmppt3.m[1]+A[8];
	////	t3=A[0]*tmppt3.m[0]*tmppt3.m[0]+A[1]*tmppt3.m[1]*tmppt3.m[1]+A[3]*tmppt3.m[0]*tmppt3.m[1]+A[6]*tmppt3.m[0]+A[7]*tmppt3.m[1]+A[9];
	////	roots[0]=0;
	////	roots[1]=0;
	////	roots[2]=0;
	////	roots[3]=0;
	////	int rootsnum=SolveQuadratic(t1, t2, t3,&roots[1],&roots[2],rootsnum);
	////	tmp=0;
	////	if(rootsnum==2)	
	////	{
	////		if (roots[RF]>=tmppt2.m[2] && roots[RF]<=tmppt1.m[2] )
	////			Buffer[(i2*Height+i)*3+2]=RevSurFlag*(roots[RF]-tmppt3.m[2]);							
	////	}
	////	if (rootsnum==1)
	////		Buffer[(i2*Height+i)*3+2]=RevSurFlag*(roots[1]-tmppt3.m[2]);	

	////}


	//	//	if(bReject)
	//	//	{			
	//	//		for(i = 1; i< 60; i++)
	//	//		{
	//	//			num  = tmpcld.GetPntNum();
	//	////			tmpcld.Slicing(axispnt, axisdir, rmserror*2 , remaincld);
	//	//			if(remaincld.GetPntNum() == num)
	//	//			{
	//	//				break;
	//	//			}
	//	//			else
	//	//			{
	//	//				tmpcld  = remaincld;
	//	//				remaincld.CFit2Quadratic(A);
	//	//			}
	//	////////////////////////////////////////////////////////////////////////
	//	//
	//	//
	//	///////////////////////////////////////////////////////////////////////
	//	//		}
	//	//		tmpcld  = remaincld;
	//	//	}	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}


void XCloud::CFit2Quadratic(double A[])
{
	PNT pnt;
	Ti_Matrix<double> MatrixArray(10,10), U(10,10), tempMatrix(10,10);
	Ti_Matrix<double> secondOrderMatrix(3,3),CU(3,3);

	int PointNum,i;
	double w[10],cw[3];
	double tempVec[10];
	double EigenVector[10];

	MatrixArray.Zero(); //Initialization
	U.Zero();
	tempMatrix.Zero();
	secondOrderMatrix.Zero();
	CU.Zero();

	//Initiate the matrix
	PointNum = this->GetPntNum();
	for(i=0;i<PointNum;i++)
	{
		pnt = this->m_Point[i];

		tempVec[0] = pnt[0]*pnt[0]; //x2
		tempVec[1] = pnt[1]*pnt[1]; //y2
		tempVec[2] = pnt[2]*pnt[2]; //z2
		tempVec[3] = pnt[0]*pnt[1]; //xy
		tempVec[4] = pnt[0]*pnt[2];	//xz
		tempVec[5] = pnt[1]*pnt[2]; //yz
		tempVec[6] = pnt[0];        //x
		tempVec[7] = pnt[1];        //y
		tempVec[8] = pnt[2];        //z
		tempVec[9] = 1;             //Constant
		tempMatrix.OutProduct(tempVec, 10, tempVec, 10); //constructing a 10X10 matrix
		MatrixArray = MatrixArray + tempMatrix;

	}
	MatrixArray.JacobiN(10,w,U);

	for(i=0;i<10;i++)
	{
		EigenVector[i] = U.m[i][9];
	}

	//Get the max eigenvactor of second order items
	double tempMaxVec;
	int tempIndex;
	for(i=0;i<1;i++)
	{
		if(EigenVector[i]>EigenVector[i+1])
		{
			tempMaxVec=EigenVector[i];
			tempIndex=i;
		}
		else
		{
			tempMaxVec=EigenVector[i+1];
			tempIndex=i+1;
		}
	}
	for(i=0;i<10;i++)
	{
		EigenVector[i]/= tempMaxVec;
		A[i]=EigenVector[i];
	}

	MatrixArray.Destroy();
	U.Destroy();
	tempMatrix.Destroy();
	secondOrderMatrix.Destroy();
	CU.Destroy();
}







double XCloud::Fit2Cylinder(PNT& axis_pnt, PNT& axis_dir, double& radius,PNT& bottom_center,PNT& top_center,bool bFixedRadius)const
{
	// initialize the original value
	int num;
	XCloud  cld = *this;
	if(cld.GetPntNum() < 5)
	{
		return -1.0 ; // fitting failed for less of points
	}

	if(axis_dir.Length()< 1e-5)
	{
		Ti_cld_point   ti_cld;
		Ti_point  pricurcld,
			* cylinder_pnt;
		PNT center, pnt;
		QuCylinder   cylinder;
		double dx = 0 , dy = 0, dz = 0;
		int i;

		// begin initial value estimation
		XBndBox box =  cld.GetBBox();
		//cld.SampleUniform(10);
		cld.Sort(3);
		center = cld[cld.GetPntNum()/2];

		num = GetPntNum();
		cld.InitObject();
		for(i =0; i < num; i++)
		{
			if(m_Point[i][0] < center[0] - box.GetLength()/10)
				continue;
			if( m_Point[i][0] > center[0] +box.GetLength()/10 )
				continue;
			if( m_Point[i][1] < center[1] - box.GetLength()/10 )
				continue;
			if( m_Point[i][1] > center[1] +box.GetLength()/10)
				continue;
			if(	 m_Point[i][2] < center[2] - box.GetLength()/10 )
				continue;
			if ( m_Point[i][2] < center[2] +box.GetLength()/10 )
				cld.AddPoint(m_Point[i]);
		}
		num = cld.GetPntNum();
		cld.SampleUniform(num/50);

		XCircle circle;
		double rms = cld.Fit2Circle(circle);
		if(rms < 0.005/25.4)
		{
			axis_pnt = circle.m_Center;
			axis_dir = circle.m_Norm_plane;
			radius = circle.m_Radius;
		}
		else
		{
			num = cld.GetPntNum();
			cld.Sort(2);
			ti_cld.XCloud2Ti_Cloud(cld);
			ti_cld.CalcBox();
			box = cld.GetBBox();
			center = (box.GetMax() + box.GetMin())/2;
			pnt  = box.GetMax() -box.GetMin();
			for( i = 0 , cylinder_pnt = ti_cld.GetHead(); i < num ; i++)
			{
				cylinder_pnt = cylinder_pnt->GetNext();
				if(cylinder_pnt == NULL)
					break;
				if((fabs(cylinder_pnt->GetX() - center[0]) < pnt[0]/4)&&
					(fabs(cylinder_pnt->GetY() - center[1]) < pnt[1]/4))
					break;
			}

			while(!ti_cld.CalcPriCur(cylinder_pnt,NULL))
			{
				for(i=0; i < num/3 && cylinder_pnt!= NULL; i++)
					cylinder_pnt = cylinder_pnt->GetNext();
				if(cylinder_pnt == NULL)
					break;
			}
			if(cylinder_pnt != NULL)
			{
				cylinder_pnt->GetMinDir(dx, dy, dz);
				axis_dir[0] =  dx;
				axis_dir[1] =  dy;
				axis_dir[2] =  dz;
				radius = cylinder_pnt->GetMaxCur();
				radius  = 1/radius;
			}
			else
			{
				axis_dir = PNT(0, 0, 1);// xiaoming revise Dec 27, 2005
				radius = 6.3;
			}
			axis_dir.Normalize();

			if(radius > 1)
				radius = 6;
			axis_pnt = cld.GetMassCenter();
		}
		axis_dir = PNT(0, 0, 1);// xiaoming revise Dec 27, 2005
		radius = 0.25;
		axis_pnt = PNT(-1.169796, -2.55105, 1.2);
	}


	// end initial value estimation        
	Ti_Optimization   algorithm;
	num = GetPntNum();
	double ** g_pnt = allocate_real_matrix(1, num , 1, 3);
	ASSERT(g_pnt != NULL);
	for(int i = 1 ; i <= num; i++)
	{
		g_pnt[i][1]  = (*this)[i-1][0];
		g_pnt[i][2]  = (*this)[i-1][1];
		g_pnt[i][3]  = (*this)[i-1][2];
	}
	double par[8];
	par[1] = axis_pnt[0];
	par[2] = axis_pnt[1];
	par[3] = axis_pnt[2];
	par[4] = axis_dir[0];
	par[5] = axis_dir[1];
	par[6] = axis_dir[2];
	par[7] = radius;

	double in[7], out[8], *rv,	**jjinv;
	jjinv =  allocate_real_matrix(1,7,1,7);
	rv =  allocate_real_vector(1, num);
	in[0]=1.0e-30; 
	in[3]=1.0e-5;	//the relative tolerance for the difference between  the Euclidean norm 
	//of the ultimate and penultimate residual vector 
	in[4]=1.0e-7;	// the absolute tolerence for the difference between  the Euclidean norm
	//of the ultimate and penultimate residual vector
	in[5]=30;		 //the maximum number of calls of funct allowed
	in[6]=1.0e-6;	//a starting value used for the relation between 
	// the gradient and the Gauss-Newton direction (value ξabove);
	// if the problem is well conditioned then in[6] should be greater,
	// but the value of in[6] should satisfy:in[0]<in[6]<=1/in[0]
	algorithm.MarquardtforCylinderFitting(num, 7, g_pnt, par, rv, jjinv, EvaluateforCylinder, JacobianforCylinder, in, out);
	axis_pnt[0] =par[1] ;
	axis_pnt[1]  =par[2]  ;
	axis_pnt[2] =par[3] ; 
	axis_dir[0]  =par[4];
	axis_dir[1]  =par[5]  ;
	axis_dir[2] =par[6]  ;
	radius = par[7]  ;
	axis_dir.Normalize();
	if( jjinv != NULL)
	{
		free_real_matrix(jjinv, 1,7,1);
		jjinv = NULL;
	}
	if( rv != NULL)
	{
		free_real_vector(rv, 1);
		rv = NULL;
	}
	if(g_pnt != NULL)
	{
		free_real_matrix(g_pnt, 1, num, 1);
		g_pnt = NULL;
	}
	cld = *this;
	cld.SortByDir(axis_dir);
	cld.GetPoint(0, bottom_center);
	bottom_center -=  axis_pnt;
	bottom_center  = axis_pnt + (axis_dir*(bottom_center*axis_dir));
	cld.GetPoint(num-1, top_center);
	top_center -=  axis_pnt;
	top_center  = axis_pnt + (axis_dir*(top_center*axis_dir));
	axis_pnt = (top_center + bottom_center )/2;
	return sqrt(out[2]*out[2]/num);
}


double XCloud::Fit2Cylinder_Outlier(PNT& axis_pnt,PNT& axis_dir,double& radius,PNT& bottom_center,PNT& top_center,bool bFixedRadius)
{
	// initialize the original value
	PNT center, pnt;

	XCloud  cld(*this) ;
	int num = GetPntNum();
	if(cld.GetPntNum() < 5)
	{
		return -1.0 ; // fitting failed
	}		       
	num = cld.GetPntNum();
	Ti_Optimization   algorithm;

	double ** g_pnt = allocate_real_matrix(1, num , 1, 3);
	ASSERT(g_pnt != NULL);
	for(int i = 1 ; i <= num; i++)
	{
		g_pnt[i][1]  = cld[i-1][0];
		g_pnt[i][2]  = cld[i-1][1];
		g_pnt[i][3]  = cld[i-1][2];
	}
	double par[8];
	par[1] = axis_pnt[0];
	par[2] = axis_pnt[1];
	par[3] = axis_pnt[2];
	par[4] = axis_dir[0];
	par[5] = axis_dir[1];
	par[6] = axis_dir[2];
	par[7] = radius;

	double in[7], out[8], *rv,	**jjinv;
	jjinv =  allocate_real_matrix(1,7,1,7);
	rv =  allocate_real_vector(1, num);

	in[0]=1.0e-30; 
	in[3]=1.0e-5;	//the relative tolerance for the difference between  the Euclidean norm 
	//of the ultimate and penultimate residual vector 
	in[4]=1.0e-7;	// the absolute tolerence for the difference between  the Euclidean norm
	//of the ultimate and penultimate residual vector
	in[5]=50;		 //the maximum number of calls of funct allowed
	in[6]=1.0e-6;	//a starting value used for the relation between 
	// the gradient and the Gauss-Newton direction (value ξabove);
	// if the problem is well conditioned then in[6] should be greater,
	// but the value of in[6] should satisfy:in[0]<in[6]<=1/in[0]
	algorithm.MarquardtforCylinderFitting(num, 7, g_pnt, par, rv, jjinv, EvaluateforCylinder_Guass, JacobianforCylinder, in, out);
	axis_pnt[0] =par[1] ;
	axis_pnt[1]  =par[2]  ;
	axis_pnt[2] =par[3] ; 
	axis_dir[0]  =par[4];
	axis_dir[1]  =par[5]  ;
	axis_dir[2] =par[6]  ;
	radius = par[7]  ;
	if( jjinv != NULL)
	{
		free_real_matrix(jjinv, 1,7,1);
		jjinv = NULL;
	}
	if( rv != NULL)
	{
		free_real_vector(rv, 1);
		rv = NULL;
	}
	if(g_pnt != NULL)
	{
		free_real_matrix(g_pnt, 1, num, 1);
		g_pnt = NULL;
	}
	cld = *this;
	cld.SortByDir(axis_dir);
	cld.GetPoint(0, bottom_center);
	bottom_center -=  axis_pnt;
	bottom_center  = axis_pnt + (axis_dir*(bottom_center*axis_dir));
	cld.GetPoint(num-1, top_center);
	top_center -=  axis_pnt;
	top_center  = axis_pnt + (axis_dir*(top_center*axis_dir));

	return sqrt(out[2]*out[2]/num);
}

void XCloud::CloudTransform(XCloud OrgCld, Ti_Matrix<double> matrix)
{
	Ti_Matrix<double>  pnt(1, 4), newpnt(1, 4);

	PNT tmp_Pt;
	
	for(int i = 0; i < OrgCld.GetPntNum(); i++)
	{
		pnt[0][0] = OrgCld[i][0];
		pnt[0][1] = OrgCld[i][1];
		pnt[0][2] = OrgCld[i][2];
		pnt[0][3] = 1;

		newpnt.Multiply( pnt, matrix);

		tmp_Pt[0] = newpnt[0][0];
		tmp_Pt[1] = newpnt[0][1];
		tmp_Pt[2] = newpnt[0][2];

		this->AddPoint(tmp_Pt);	

	}
	pnt.Destroy();
	newpnt.Destroy();

}

int BackSub(double *a,int n,double *x)
{
	int i,j;
	float val;
	//assumes column vector is packed into the last column of working array
	for(i=n-1;i>-1;i--)
	{
		for(val=0,j=i+1;j<n;j++) val +=a[j+i*(n+1)]*x[j];
		x[i]= (a[n+i*(n+1)]-val)/a[i+i*(n+1)];
	}
	return 0;
}


int GaussElim(double *a,double *b,int n,double *x)
{
	double W[100];
	int i,j,k;

	for(i=0;i<n;i++)  //initialize the working array,tested
	{
		for(j=0;j<n;j++)
		  W[j+i*(n+1)]= a[j+i*n];
		W[n+i*(n+1)]=b[i];
	}
	

	
	for(i=0;i<n;i++)  //column sum
	{
		float temp,maxval;
		int pos;
		maxval= -1.;
		//pivot about biggest mag. coefficient
		for(j=i;j<n;j++)
			if(fabs(W[i+j*(n+1)])>maxval) 
			{
				maxval=fabs(W[i+j*(n+1)]);
				pos=j;
			}
		if(fabs(maxval) < -.9) 
		{
			//MSGFLOAT("badder",W[0],W[4],W[8],W[12]);
			return -1;
		}
		for(j=0;j<n+1;j++) 
		{
			temp=W[j+i*(n+1)];
			W[j+i*(n+1)]=W[j+pos*(n+1)];
			W[j+pos*(n+1)]=temp;
		}
	
		//now triangularize by mostly eliminating ith variable
		for(k=i+1;k<n;k++) //row sum
		{
			temp = W[i+k*(n+1)]= W[i+k*(n+1)]/W[i+i*(n+1)];
			for(j=i+1;j<n+1;j++) W[j+k*(n+1)] -=temp*W[j+i*(n+1)];
		}
		
	}
	
	BackSub(W,n,x);
	
	return 0;
}



//int nl_fit_circle(int num,float *x,float *y,CIRCLE *c)
double XCloud::Fit2CircleNew(XCircle &circlefit, int s, int e)

{
#define TOL .0002
	
	double A[9],X[3],xbar,ybar;
	double del[3],err[3],mag,fiterr;
	float err_val=1,err_val2=1;
	int status,i,j;
	int ctr=0;
	xbar=ybar=0;
	int num;
	num = e-s+1;
	for(i=0;i<num;i++) 
	{
		xbar += m_Point[i+s][0];
		ybar += m_Point[i+s][1];
	}
	xbar /=num;ybar /=num;
		
	normal_fit_circle(circlefit,s,e); //get a good first guess
	X[0]=circlefit.m_Center[0];X[1]=circlefit.m_Center[1];X[2]=circlefit.m_Radius;
	while(err_val > TOL || err_val2 >TOL)
	{
		
		err[0] = -2*num*xbar+2*num*X[0];
		err[1] =-2*num*ybar+2*num*X[1];
		err[2] = 2.*num*X[2];
			
		memset(A,0,9*sizeof(double));
		A[0+0*3]=2*num;A[1+1*3]=2*num;
		A[2+3*2]=2*num;
		for(i=0;i<num;i++) 
		{
			mag = sqrt(SQ(m_Point[i+s][0]-X[0])+SQ(m_Point[i+s][1]-X[1]));
			err[0] += 2*X[2]*(m_Point[i+s][0]-X[0])/mag;
			err[1] += 2*X[2]*(m_Point[i+s][1]-X[1])/mag;
			err[2] += -2*mag;
			
			A[0+0*3]+= -2*X[2]/mag + 2*X[2]*SQ(m_Point[i+s][0]-X[0])/(mag*SQ(mag));
			A[0+1*3]+= 2*X[2]*(m_Point[i+s][0]-X[0])*(m_Point[i+s][1]-X[1])/(mag*SQ(mag));
			A[0+2*3]+= (m_Point[i+s][0]-X[0])/mag;

			A[1+0*3]+=  2*X[2]*(m_Point[i+s][0]-X[0])*(m_Point[i+s][1]-X[1])/(mag*SQ(mag));
			A[1+1*3]+= -2*X[2]/mag+2*X[2]*SQ(m_Point[i+s][1]-X[1])/(mag*SQ(mag));
			A[1+2*3]+= (m_Point[i+s][1]-X[1])/mag;

			A[2+0*3]+= 2*(m_Point[i+s][0]-X[0])/mag;
			A[2+1*3]+= 2*(m_Point[i+s][1]-X[1])/mag;
		}
		
		status = GaussElim(A,err,3,del);
		if(status==-1) {return 1;}
		 X[0]-=del[0];X[1]-=del[1];X[2]-=del[2];
			
		err_val=sqrt(SQ(del[0])+SQ(del[1])+SQ(del[2]));
	    err_val2=sqrt(SQ(err[0])+SQ(err[1])+SQ(err[2]));
		ctr++;
		if(ctr > 10) {return 1;}
	}
	//c->chi=0;
	fiterr=0;
	for(j=0;j<num;j++) 
		fiterr += SQ(sqrt(SQ(m_Point[j+s][0]-X[0])+SQ(m_Point[j+s][1]-X[1]))-X[2]);
	fiterr = sqrt(fiterr/(num-3));
	circlefit.m_Center[0]=X[0];circlefit.m_Center[1]=X[1];circlefit.m_Radius=X[2];
	//c->x[0]=X[0];c->x[1]=X[1];c->r=X[2];
	//status = test_if_circle(num,x,y,c);
	//c->sign = sign_curvature(num,x,y,c);

	circlefit.m_FitPntNum = num;
	circlefit.m_RMS = fiterr;
	
	//if (status==1) return 0; 
	//else return 1;
	return fiterr;
}

double XCloud::normal_fit_circle(XCircle &circlefit, int s, int e)
{
	int	i,status;
	double	sx,sxx,sxxx;
	double	sy,syy,syyy;
	double	sxy,sxxy,sxyy;
	double	xdet,c1,c2,c3;
	double  cx,cy,r;

	double	sxxsyy,sxxxsxyy,sxxysyyy;
	double X,Y,error=0;
	int	num = e-s+1;
	if( num < 3 )  return -2;

	sx = sxx = sxxx = sy = syy = syyy = sxy = sxxy = sxyy = 0.0;
	
	for (i = 0; i < num; i++) {
		X = m_Point[i+s][0];//s[i];
		Y = m_Point[i+s][1];//t[i];
		sx += X;
		sxx += X * X;
		sxxx += X * X * X;
		sy += Y;
		syy += Y * Y;
		syyy += Y * Y * Y;
		sxy += X * Y;
		sxyy += X * Y * Y;
		sxxy += X * X * Y;
	}
	sxxsyy = sxx + syy;
	sxxxsxyy = sxxx + sxyy;
	sxxysyyy = sxxy + syyy;

	/* best fit a circle */

    xdet = num*sxx*syy+2.*(sx*sy*sxy)-sy*sy*sxx-sx*sx*syy-num*sxy*sxy;
	if (fabs(xdet) < 1.0e-10) 
	 {
	////printf("Determinant fails\n");
	return -1;
	 }
    c1 = (sxxsyy*sxx*syy+sxxxsxyy*sxy*sy+sxxysyyy*sx*sxy-
        sy*sxx*sxxysyyy-sxy*sxy*sxxsyy-syy*sx*sxxxsxyy) / (-xdet);
    c2 = (num*sxxxsxyy*syy+sx*sxxysyyy*sy+sy*sxy*sxxsyy-
        sy*sy*sxxxsxyy-num*sxy*sxxysyyy-syy*sx*sxxsyy) / (-xdet);
    c3 = (num*sxx*sxxysyyy+sx*sxy*sxxsyy+sy*sx*sxxxsxyy-
        sxxsyy*sxx*sy-sxxxsxyy*sxy*num-sxxysyyy*sx*sx) / (-xdet);

	cx = -c2 / 2.;
    cy = -c3 / 2.;

    r  = sqrt(fabs(SQ(cx)+SQ(cy)-c1));

	for (i = 0; i < num; i++) 
	{
		double val = sqrt(SQ(m_Point[i+s][0] - cx) +SQ(m_Point[i+s][1] - cy))-r;
        error += SQ(val);
	}
	
	circlefit.m_Center[0] = cx;
	circlefit.m_Center[1] = cy;
	circlefit.m_Center[2] = 0;

	circlefit.m_Radius = r;
	
	error = sqrt(error/num);
	//status=0;
	return error; //TRUE or FALSE
}
double XCloud::normal_fit_circle(XCircle &circlefit, double **m_Point, int n)
{
	int	i,status;
	double	sx,sxx,sxxx;
	double	sy,syy,syyy;
	double	sxy,sxxy,sxyy;
	double	xdet,c1,c2,c3;
	double  cx,cy,r;

	double	sxxsyy,sxxxsxyy,sxxysyyy;
	double X,Y,error=0;
	int	num = n;
	if( num < 3 )  return -2;

	sx = sxx = sxxx = sy = syy = syyy = sxy = sxxy = sxyy = 0.0;
	
	for (i = 0; i < num; i++) {
		X = m_Point[i+1][1];//s[i];
		Y = m_Point[i+1][2];//t[i];
		sx += X;
		sxx += X * X;
		sxxx += X * X * X;
		sy += Y;
		syy += Y * Y;
		syyy += Y * Y * Y;
		sxy += X * Y;
		sxyy += X * Y * Y;
		sxxy += X * X * Y;
	}
	sxxsyy = sxx + syy;
	sxxxsxyy = sxxx + sxyy;
	sxxysyyy = sxxy + syyy;

	/* best fit a circle */

    xdet = num*sxx*syy+2.*(sx*sy*sxy)-sy*sy*sxx-sx*sx*syy-num*sxy*sxy;
	if (fabs(xdet) < 1.0e-10) 
	 {
	////printf("Determinant fails\n");
	return -1;
	 }
    c1 = (sxxsyy*sxx*syy+sxxxsxyy*sxy*sy+sxxysyyy*sx*sxy-
        sy*sxx*sxxysyyy-sxy*sxy*sxxsyy-syy*sx*sxxxsxyy) / (-xdet);
    c2 = (num*sxxxsxyy*syy+sx*sxxysyyy*sy+sy*sxy*sxxsyy-
        sy*sy*sxxxsxyy-num*sxy*sxxysyyy-syy*sx*sxxsyy) / (-xdet);
    c3 = (num*sxx*sxxysyyy+sx*sxy*sxxsyy+sy*sx*sxxxsxyy-
        sxxsyy*sxx*sy-sxxxsxyy*sxy*num-sxxysyyy*sx*sx) / (-xdet);

	cx = -c2 / 2.;
    cy = -c3 / 2.;

    r  = sqrt(fabs(SQ(cx)+SQ(cy)-c1));

	for (i = 0; i < num; i++) 
	{
		double val = sqrt(SQ(m_Point[i+1][1] - cx) +SQ(m_Point[i+1][2] - cy))-r;
        error += SQ(val);
	}
	
	circlefit.m_Center[0] = cx;
	circlefit.m_Center[1] = cy;
	circlefit.m_Center[2] = 0;

	circlefit.m_Radius = r;
	
	error = sqrt(error/num);
	//status=0;
	return error; //TRUE or FALSE
}

//now cannot use it again. it conflicts with another fitting
//2005,June 16 by jm
double XCloud::Fit2XYCircle(XCircle &circlefit, int s, int e)
{
	int	num = e-s+1;
	if( num < 3 )  return -2;

	int i;

	double  xc=0,yc=0;
	double	**g_pnt			= NULL;
	g_pnt = allocate_real_matrix(1, num,  1, 2);
	double  scal= this->m_BndBox.GetLength()/2.828;
	for(i=0; i<num; i++ )
	{
		xc += m_Point[i+s][0];
		yc += m_Point[i+s][1];	 
	
	}
	xc=xc/(num-1);yc=yc/(num-1);
	for(i=0; i<num; i++ )
	{
		g_pnt[i+1][1] = m_Point[i+s][0]-xc;
		g_pnt[i+1][2] = m_Point[i+s][1]-yc;	 
	
		g_pnt[i+1][1] *= (1/scal);
		g_pnt[i+1][2] *= (1/scal);	 
	}
	double   par[9];
	if((normal_fit_circle(circlefit,g_pnt,num))<0)
	{
		par[1] = xc; par[2] = yc; par[3] = 1.0;
	}
	else
	{
		par[1] = circlefit.m_Center[0];
		par[2] = circlefit.m_Center[1];
		par[3] = circlefit.m_Radius;
	}
	//circle fitting
	Ti_Optimization circle_Optimization;
	circle_Optimization.D2Circle_fitting(num,g_pnt,par);	

	circlefit.m_Center[0] = par[1]+xc;
	circlefit.m_Center[1] = par[2]+yc;
	circlefit.m_Center[2] = 0;

	circlefit.m_Radius = par[3];

	double fitting_error = 0;
	for(i=0; i<num; i++ )
		fitting_error += pow( (sqrt(SQ(m_Point[i+s][0] - circlefit.m_Center[0]) + 
									SQ(m_Point[i+s][1] - circlefit.m_Center[1])) - circlefit.m_Radius), 2);

	fitting_error = sqrt(fitting_error/num);
	return fitting_error;
}

double XCloud::Fit2Circle(XCircle &circlefit)
{
	int i=0;
	XCloud cld(*this);
	int num = this->GetPntNum();
	num = cld.GetPntNum();

	//fit to a plane first
	cld.Fit2Plane(circlefit.m_Pnt_plane, circlefit.m_Norm_plane);

	 //rotate the plane vector to the + direction of Z axis	
	if( circlefit.m_Norm_plane[2]< 0)
		for ( i=0; i<3; i++)
			circlefit.m_Norm_plane[i] = -circlefit.m_Norm_plane[i];

	//initiate the transformation matrix
		 int  j=0;
		 Ti_Matrix<double> translaion_matrix(4,4),  Cumulative_matrix(4,4), X_matrix(4,4), Y_matrix(4,4);

		 for(i=0; i<4;i++)
			 for(j=0;j<4;j++)
				 if(i == j)
				 {
					X_matrix[i][j] = 1;
					Y_matrix[i][j] = 1;
					translaion_matrix[i][j] = 1;
					Cumulative_matrix[i][j] = 1;
				 }
				 else
				 {
					X_matrix[i][j] = 0;
					Y_matrix[i][j] = 0;
					translaion_matrix[i][j] = 0;
					Cumulative_matrix[i][j] = 0;
				 }

		//translation from centroid to origin, m_Pnt_plane is the centroid
		 translaion_matrix[3][0] = -circlefit.m_Pnt_plane[0];
		 translaion_matrix[3][1] = -circlefit.m_Pnt_plane[1];
		 translaion_matrix[3][2] = -circlefit.m_Pnt_plane[2];

		  //X axis rotation	 
		 double norm1 = sqrt(circlefit.m_Norm_plane[2]*circlefit.m_Norm_plane[2] +  circlefit.m_Norm_plane[1]*circlefit.m_Norm_plane[1]);
		 if(norm1 == 0)
		 {
			X_matrix[1][1] = 1;
			X_matrix[1][2] = 0;
			X_matrix[2][1] = 0;
			X_matrix[2][2] = 1;
		 }
		 else
		 {
			X_matrix[1][1] = circlefit.m_Norm_plane[2]/norm1;
			X_matrix[1][2] = circlefit.m_Norm_plane[1]/norm1;
			X_matrix[2][1] = -circlefit.m_Norm_plane[1]/norm1;
			X_matrix[2][2] = circlefit.m_Norm_plane[2]/norm1;
		 }

		  //Y axis rotation		 
		 Y_matrix[0][0] = norm1;
		 Y_matrix[2][2] = norm1;
		 Y_matrix[0][2] = circlefit.m_Norm_plane[0];
		 Y_matrix[2][0] = -circlefit.m_Norm_plane[0];


		 Cumulative_matrix.Multiply(Y_matrix, Cumulative_matrix);
		 Cumulative_matrix.Multiply(X_matrix, Cumulative_matrix);
		 Cumulative_matrix.Multiply(translaion_matrix, Cumulative_matrix);

		 double a[16];
		 a[0] = Cumulative_matrix[0][0];
		 a[1] = Cumulative_matrix[0][1];
		 a[2] = Cumulative_matrix[0][2];
		 a[3] = Cumulative_matrix[0][3];
		 a[4] = Cumulative_matrix[1][0];
		 a[5] = Cumulative_matrix[1][1];
		 a[6] = Cumulative_matrix[1][2];
		 a[7] = Cumulative_matrix[1][3];
		 a[8] = Cumulative_matrix[2][0];
		 a[9] = Cumulative_matrix[2][1];
		 a[10] = Cumulative_matrix[2][2];
		 a[11] = Cumulative_matrix[2][3];		  
		 a[12] = Cumulative_matrix[3][0];
		 a[13] = Cumulative_matrix[3][1];
		 a[14] = Cumulative_matrix[3][2];
		 a[15] = Cumulative_matrix[3][3];

		 //transform the cloud into XOY plane and create the new cloud
		 XCloud trans_Z_cld;
		 trans_Z_cld.CloudTransform((*this),Cumulative_matrix);	
		 num = trans_Z_cld.GetPntNum();
		 num = this->GetPntNum();

		 double	**g_pnt			= NULL;
		 g_pnt = allocate_real_matrix(1, trans_Z_cld.GetPntNum(),  1, 2);
		 for(i=0; i<trans_Z_cld.GetPntNum(); i++ )
		 {
			 g_pnt[i+1][1] = trans_Z_cld[i][0];
			 g_pnt[i+1][2] = trans_Z_cld[i][1];	 
		 
		 }

		 //circle fitting
		 Ti_Optimization circle_Optimization;
		 double   par[9];
		circle_Optimization.D2Circle_fitting(trans_Z_cld.GetPntNum(),
						 g_pnt,
						 par);	

		//calculate the inverse matrix
		Cumulative_matrix.Invert(1e-10);
		a[0] = Cumulative_matrix[0][0];
		 a[1] = Cumulative_matrix[0][1];
		 a[2] = Cumulative_matrix[0][2];
		 a[3] = Cumulative_matrix[0][3];
		 a[4] = Cumulative_matrix[1][0];
		 a[5] = Cumulative_matrix[1][1];
		 a[6] = Cumulative_matrix[1][2];
		 a[7] = Cumulative_matrix[1][3];
		 a[8] = Cumulative_matrix[2][0];
		 a[9] = Cumulative_matrix[2][1];
		 a[10] = Cumulative_matrix[2][2];
		 a[11] = Cumulative_matrix[2][3];		  
		 a[12] = Cumulative_matrix[3][0];
		 a[13] = Cumulative_matrix[3][1];
		 a[14] = Cumulative_matrix[3][2];
		 a[15] = Cumulative_matrix[3][3];


		Ti_Matrix<double>  circle_center(1,4);	
	
		circle_center[0][0] = par[1];
		circle_center[0][1] = par[2];
		circle_center[0][2] = 0;
		circle_center[0][3] = 1;	

		circle_center.Multiply(circle_center, Cumulative_matrix);

		circlefit.m_Center[0] = circle_center[0][0];
		circlefit.m_Center[1] = circle_center[0][1];
		circlefit.m_Center[2] = circle_center[0][2];

		circlefit.m_Radius = par[3];

		//calculate the fitting errors
		double fitting_error = 0;
		for(i=0; i<trans_Z_cld.GetPntNum(); i++ )
			fitting_error += pow( (sqrt((cld[i][0] - circlefit.m_Center[0])*(cld[i][0] - circlefit.m_Center[0]) + 
														   (cld[i][1] - circlefit.m_Center[1])*(cld[i][1] - circlefit.m_Center[1]) + 
														   (cld[i][2] - circlefit.m_Center[2])*(cld[i][2] - circlefit.m_Center[2])
														  ) - circlefit.m_Radius), 2);

		fitting_error = sqrt(fitting_error/trans_Z_cld.GetPntNum());


		translaion_matrix.Destroy();
		Cumulative_matrix.Destroy();
		X_matrix.Destroy();
		Y_matrix.Destroy();
		circle_center.Destroy();
		free_real_matrix(g_pnt, 1, trans_Z_cld.GetPntNum(), 1);
		
		return fitting_error;


}

double XCloud::Fit2DCircle(XCircle &circlefit, int s , int e)
{
	int			i, j;
	int         num;
	int			jt = 500;
	double		eps = 1e-50;
	double		*mmatrix;
	double		s1[16], c1[16], c1_inv[16], m[16];
	double		lamda_r[4], lamda_v[4], pos_value;
	double		k_coe, a1[4], a2[4];

	double		temp, temp_m[16];

	num = e-s+1;
	if( num < 3 )  return -2;

	mmatrix = new double[ num*2 ];
	for (i=0; i<num; i++)
	{
		mmatrix[2*i]	=	m_Point[i+s][0];;
		mmatrix[2*i+1]	=	m_Point[i+s][1];;
	}
}


bool XCloud::Slicing( PNT& section_pnt,  PNT& section_dir, double tolerance , XCloud& section_cld)
{
	int i;
	int num = GetPntNum();
	int begin = 0 , end = num, index;
	index = (begin + end)/2;
	section_cld.InitObject();
	section_dir.Normalize();
	if( acos(section_dir[0])  < 1e-6 ||fabs(PI - acos(section_dir[0]))  < 1e-6)
	{
		Sort(1);
		while(1)
		{
			if((*this)[index][0] >= section_pnt[0] - tolerance)
			{
				end = index;
			}
			else //if((*this)[index][0] <= section_pnt[0] - tolerance)
			{
				begin = index;
			}
			if( end - begin  < 6)
				break;
			else
				index = (begin + end)/2;
		}
		for( i = begin ; i < num ; i++)
		{
			if( (*this)[i][0] > section_pnt[0] + tolerance)
				break;
			if( (*this)[i][0] > section_pnt[0] - tolerance)
				section_cld.AddPoint((*this)[i]);
		}
		return true;
	}		
	else
	{
		PNT  rot_axis,  x_dir(1, 0, 0), origin_pnt(0,0,0);
		double angle = acos(section_dir[0]);
		rot_axis = section_dir^x_dir;
		rot_axis.Normalize();
		Translate(-section_pnt);
		Rotate(origin_pnt, rot_axis, angle);
		Sort(1);
		while(1)
		{
			if((*this)[index][0] > - tolerance)
			{
				end = index;
			}
			else
			{
				begin = index;
			}
			if( end - begin  < 6)
				break;
			else
				index = (begin + end)/2;
		}
		for( i = begin ; i < num ; i++)
		{
			if( (*this)[i][0] >  tolerance)
				break;
			if( (*this)[i][0] >  - tolerance)
				section_cld.AddPoint((*this)[i]);
		}
		section_cld.Rotate(origin_pnt, rot_axis, -angle);
		section_cld.Translate(section_pnt);
		Rotate(origin_pnt, rot_axis, -angle);
		Translate(section_pnt);
		return true;
	}
	return false;
}
void XCloud::SortByDir(PNT& dir)
{
	dir.Normalize();
	int  axis_index = 1;
	double angle ;
	dir[1] > dir[0]? \
		(dir[2] > dir[1]? (axis_index = 3):(axis_index = 1))\
		:(dir[0] > dir[2] ? (axis_index = 1):(axis_index = 2));
	PNT rot_axis, sort_axis, origin;
	switch(axis_index)
		{
		case 1:
			sort_axis = PNT(1, 0, 0);
			angle = acos(dir[0]);
			break;
		case 2:
			sort_axis = PNT(0, 1, 0);
			angle = acos(dir[1]);
			break;
		case 3:
			sort_axis = PNT(0, 0, 1);
			angle = acos(dir[2]);
			break;
		}
	rot_axis = dir^sort_axis;
	rot_axis.Normalize();
	this->Rotate(origin, rot_axis, angle);
	this->Sort(axis_index);
	this->Rotate(origin, rot_axis,- angle);
}

void XCloud::Sort(int axis_index)
{
	int num = GetPntNum();
	if (num <= 1)
		return ;
	switch(axis_index)
	{
		//--------sort pnts according to their object coordinates
	case 1:
		sort(m_Point.begin(), m_Point.end(), SortCompareX);
		break;
	case 2:
		sort(m_Point.begin(), m_Point.end(), SortCompareY);
		break;
	case 3:
		std::sort(m_Point.begin(), m_Point.end(), SortCompareZ);
		break;
	}
}

//  used for cloud sorting
int  __cdecl SortCompareX(const  PNT&  elem1, const PNT&  elem2)
{
	if (elem1[0]> elem2[0])
		return 0;
	else if (elem1[0]< elem2[0])
		return 1;
	else
	{
		if (elem1[1] > elem2[1])
			return 0;
		else if (elem1[1] < elem2[1])
			return 1;
		else
		{
			if (elem1[2] > elem2[2])
				return 0;
			else if (elem1[2] < elem2[2])
				return 1;
			else 
				return 0;
		}
	}
}

int __cdecl SortCompareY(const PNT&  elem1, const PNT&  elem2)
{	
	if (elem1[1] > elem2[1])
		return 0;
	else if (elem1[1] < elem2[1])
		return 1;
	else
	{
		if (elem1[2] > elem2[2])
			return 0;
		else if (elem1[2] < elem2[2])
			return 1;
		else
		{
			if (elem1[0]> elem2[0])
				return 0;
			else if (elem1[0]< elem2[0])
				return 1;
			else 
				return 0;
		}
	}
}

int  __cdecl SortCompareZ(const PNT&  elem1, const PNT&   elem2)
{
	if (elem1[2] > elem2[2])
		return 0;
	else if (elem1[2] < elem2[2])
		return 1;
	else
	{
		if (elem1[0]> elem2[0])
			return 0;
		else if (elem1[0]< elem2[0])
			return 1;
		else
		{
			if (elem1[1] > elem2[1])
				return 0;
			else if (elem1[1] < elem2[1])
				return 1;
			else 
				return 0;
		}
	}
}

//this sorting algorithms fits: when the angle of the curve arc is less than 180 degrees.
void XCloud::SortByDistance()
{
	double dist_a = 10e10, dist_b = 10e10;

	//XCloud cld(*this);
	int* col = allocate_integer_vector(0, this->GetPntNum()-1);

	int i=0, j=0, k=0, next_pt =0, pre_pt =0, i_tmp =0, breaker=0;
	pre_pt = k = 0;
	breaker = 0;
	//look for the end point
	for(i=0; i< this->GetPntNum()-1; i++)
	{		
		col[i] = k;
		dist_a = 10e10;	

		for(j=0; j< this->GetPntNum(); j++)
		{
			for(i_tmp=0; i_tmp<i; i_tmp++)
				if(j == col[i_tmp]) 
					breaker = 1;

			if((j != k)&&(j != pre_pt)&&(breaker==0))
			{
				dist_b = sqrt(((*this)[k][0] - (*this)[j][0])*((*this)[k][0] - (*this)[j][0]) + 
										((*this)[k][1] - (*this)[j][1])*((*this)[k][1] - (*this)[j][1]) + 
										((*this)[k][2] - (*this)[j][2])*((*this)[k][2] - (*this)[j][2]) );
				if(dist_b < dist_a)
				{
					dist_a = dist_b;					
					next_pt  = j;
				}
			}
			breaker = 0;
		}
		pre_pt = k;
		k = next_pt;		
	}


	PNT tmp_pt((*this)[0][0], (*this)[0][1], (*this)[0][2]);

	(*this)[0][0] = (*this)[next_pt][0];
	(*this)[0][1] = (*this)[next_pt][1];
	(*this)[0][2] = (*this)[next_pt][2];
	
	(*this)[next_pt][0] = tmp_pt[0];
	(*this)[next_pt][1] = tmp_pt[1];
	(*this)[next_pt][2] = tmp_pt[2];


	//here...........................................
	//change the position
	pre_pt = k = 0;
	breaker = 0;
	for(i=0; i< this->GetPntNum()-1; i++)
	{		
		col[i] = k;
		dist_a = 10e10;	

		for(j=0; j< this->GetPntNum(); j++)
		{
			for(i_tmp=0; i_tmp<i; i_tmp++)
				if(j == col[i_tmp]) 
					breaker = 1;

			if((j != k)&&(j != pre_pt)&&(breaker==0))
			{
				dist_b = sqrt(((*this)[k][0] - (*this)[j][0])*((*this)[k][0] - (*this)[j][0]) + 
										((*this)[k][1] - (*this)[j][1])*((*this)[k][1] - (*this)[j][1]) + 
										((*this)[k][2] - (*this)[j][2])*((*this)[k][2] - (*this)[j][2]) );
				if(dist_b < dist_a)
				{
					dist_a = dist_b;					
					next_pt  = j;
				}
			}
			breaker = 0;
		}

		 tmp_pt[0] = (*this)[k+1][0];
		 tmp_pt[1] = (*this)[k+1][1];
		 tmp_pt[2] = (*this)[k+1][2];

		 (*this)[k+1][0] = (*this)[next_pt][0];
		 (*this)[k+1][1] = (*this)[next_pt][1];
		 (*this)[k+1][2] = (*this)[next_pt][2];

		(*this)[next_pt][0] = tmp_pt[0];
		(*this)[next_pt][1] = tmp_pt[1];
		(*this)[next_pt][2] = tmp_pt[2];


		pre_pt = k;
		k++;

	}


	PNT tmp_pt3((*this)[0][0], (*this)[0][1], (*this)[0][2]);
	PNT tmp_pt4((*this)[next_pt][0], (*this)[next_pt][1], (*this)[next_pt][2]);


	free_integer_vector(col, 0);

}//end


double XCloud::Fit2Arc(XArc &Arcfit)
{
	int num = this->GetPntNum();
	XCloud cld(*this);
	num = cld.GetPntNum();

	XCircle circlefit;
	double fitting_error = 0;
	fitting_error = cld.Fit2Circle(circlefit);

	Arcfit.m_Rad = circlefit.m_Radius;
	Arcfit.m_Cen = circlefit.m_Center;
	Arcfit.m_Normal = circlefit.m_Norm_plane;
	

	XCloud cld_on_circle;
	PNT tmp_Pt;
	int i = 0;
	double tmp_dist;
	for(i=0; i < cld.GetPntNum(); i++)
	{
		tmp_dist = sqrt ((cld[i][0] - circlefit.m_Center[0])*(cld[i][0] - circlefit.m_Center[0]) +
									  (cld[i][1] - circlefit.m_Center[1])*(cld[i][1] - circlefit.m_Center[1]) +
									  (cld[i][2] - circlefit.m_Center[2])*(cld[i][2] - circlefit.m_Center[2]) );
		tmp_Pt[0] = (cld[i][0] - circlefit.m_Center[0])*circlefit.m_Radius/tmp_dist + circlefit.m_Center[0];
		tmp_Pt[1] = (cld[i][1] - circlefit.m_Center[1])*circlefit.m_Radius/tmp_dist + circlefit.m_Center[1];
		tmp_Pt[2] = (cld[i][2] - circlefit.m_Center[2])*circlefit.m_Radius/tmp_dist + circlefit.m_Center[2];

		cld_on_circle.AddPoint(tmp_Pt);
	
	}

	cld_on_circle.SortByDistance( );

	//need to make the start and end point to fit the norm vector
	int tmp_i = (int)cld.GetPntNum()/2;

	PNT middlept, startpt, endpt;
	middlept[0] = cld_on_circle[tmp_i][0];
	middlept[1] = cld_on_circle[tmp_i][1];
	middlept[2] = cld_on_circle[tmp_i][2];

	startpt[0] = cld_on_circle[0][0];
	startpt[1] = cld_on_circle[0][1];
	startpt[2] = cld_on_circle[0][2];

	endpt[0] = cld_on_circle[cld.GetPntNum()-1][0];
	endpt[1] = cld_on_circle[cld.GetPntNum()-1][1];
	endpt[2] = cld_on_circle[cld.GetPntNum()-1][2];

	PNT v1, v2, v3;

	v1[0] = middlept[0] - startpt[0];
	v1[1] = middlept[1] - startpt[1];
	v1[2] = middlept[2] - startpt[2];

	
	v2[0] = endpt[0] - middlept[0];
	v2[1] = endpt[1] - middlept[1];
	v2[2] = endpt[2] - middlept[2];

	v3 = v1 ^v2;

	if((v3*circlefit.m_Norm_plane) > 0)
	{
		tmp_Pt[0] = cld_on_circle[0][0];
        tmp_Pt[1] = cld_on_circle[0][1];
        tmp_Pt[2] = cld_on_circle[0][2];
        Arcfit.SetStartPt(tmp_Pt);

        tmp_Pt[0] = cld_on_circle[cld.GetPntNum()-1][0];
        tmp_Pt[1] = cld_on_circle[cld.GetPntNum()-1][1];
        tmp_Pt[2] = cld_on_circle[cld.GetPntNum()-1][2];
        Arcfit.SetEndPt(tmp_Pt);
	}
	else
	{
		tmp_Pt[0] = cld_on_circle[0][0];
        tmp_Pt[1] = cld_on_circle[0][1];
        tmp_Pt[2] = cld_on_circle[0][2];
        Arcfit.SetEndPt(tmp_Pt);

        tmp_Pt[0] = cld_on_circle[cld.GetPntNum()-1][0];
        tmp_Pt[1] = cld_on_circle[cld.GetPntNum()-1][1];
        tmp_Pt[2] = cld_on_circle[cld.GetPntNum()-1][2];
        Arcfit.SetStartPt(tmp_Pt);
	}	

	return fitting_error;

}


//which can return direction and sql error. // tnnd
double XCloud::Fit2Line(PNT& pnt_line, PNT& direction)
{
	int i, num;
	PNT pnt;
	XCloud cld(*this);
	pnt_line =cld.GetMassCenter();
	Ti_Matrix<double> matrix(3,3),
		U(3,3);
	double w[3];
	num = cld.GetPntNum();
	cld.Translate(-pnt_line);
	double rms = 0 ;
	for(i=0; i< num; i++)
	{
		cld.GetPoint(i, pnt);
		rms += pnt*pnt;
		matrix.m[0][0] += pnt[0]*pnt[0];
		matrix.m[0][1] += pnt[0]*pnt[1];
		matrix.m[0][2] += pnt[0]*pnt[2];
		matrix.m[1][0] += pnt[1]*pnt[0];
		matrix.m[1][1] += pnt[1]*pnt[1];
		matrix.m[1][2] += pnt[1]*pnt[2];
		matrix.m[2][0] += pnt[2]*pnt[0];
		matrix.m[2][1] += pnt[2]*pnt[1];
		matrix.m[2][2] += pnt[2]*pnt[2];
	}	
	matrix.JacobiN(3, w, U);
	direction[0] = U.m[0][0];
	direction[1] = U.m[1][0];
	direction[2] = U.m[2][0];

	matrix.Destroy();
	U.Destroy();
	return sqrt((rms - w[0]) / num);
}

double XCloud::Fit2Line(XLine& fittedLine)
{
	double	a[3][3], b[3][3];
	double	min_eigenvalue;
	int		min_index, num;
	double	*mmatrix;
	int		i, j;
	double	eps=0.00000001;
	PNT		eigen_vec;

	num = this->GetPntNum();
	if( num < 3 )  return 0;

	// Prepare matrix M:  [trans(M)*M] * a = lamda * a
	mmatrix = new double[ num*3 ];

	//calculate the centroid
	PNT	cent = GetMassCenter();

	//set the origin as the line start
	for (i=0; i<num; i++)
	{
		mmatrix[3*i]	=	this->m_Point[i][0] - cent[0];
		mmatrix[3*i+1]	=	this->m_Point[i][1] - cent[1];
		mmatrix[3*i+2]	=	this->m_Point[i][2] - cent[2];
		//p=p->pnext;
	}

	//calculate the matrix
	for (i=0; i<3; i++)
		for(j=0; j<3; j++)
			a[i][j] = 0;

	for(i=0; i<num; i++)
	{
		a[0][0] += mmatrix[3*i+1] * mmatrix[3*i+1] + mmatrix[3*i+2] * mmatrix[3*i+2];
		a[1][1] += mmatrix[3*i+0] * mmatrix[3*i+0] + mmatrix[3*i+2] * mmatrix[3*i+2];
		a[2][2] += mmatrix[3*i+1] * mmatrix[3*i+1] + mmatrix[3*i+0] * mmatrix[3*i+0];

		a[0][1] -= mmatrix[3*i+0] * mmatrix[3*i+1];
		a[0][2] -= mmatrix[3*i+0] * mmatrix[3*i+2];
		a[1][2] -= mmatrix[3*i+1] * mmatrix[3*i+2];
	}
	a[1][0] = a[0][1];
	a[2][0] = a[0][2];
	a[2][1] = a[1][2];
	
	delete mmatrix;

	// Solve the eigenvector problem
	Ti_Optimization tMath;
	tMath.GetEigenvector(*a,3,*b,eps);

	min_eigenvalue = a[0][0];
	min_index = 0;
	for (i=1; i<3; i++)
	{
		if( min_eigenvalue>a[i][i] )
		{
			min_eigenvalue = a[i][i];
			min_index=i;
		}
	}

	for( i=0; i<3; i++)
	{
		eigen_vec[i] = b[i][min_index];
	}

	//eigen_vec would be the line direction	
	
	//Next:
	//Calculate the start and end points along this direction*/
	double dis=0;
	PNT max_point, min_point;

	//max point along this vector
	for( i=0; i<num; i++ )
	{
		if( dis<fabs(eigen_vec[0]*this->m_Point[i][0] + eigen_vec[1]*this->m_Point[i][1] + eigen_vec[2]*this->m_Point[i][2]))
		{
			dis=fabs(eigen_vec[0]*this->m_Point[i][0] + eigen_vec[1]*this->m_Point[i][1] + eigen_vec[2]*this->m_Point[i][2]);

			max_point = this->m_Point[i];
		}
	}

	//min point along this vector
	dis = 0;
	for( i=0; i<num; i++ )
	{
		if( dis<fabs(eigen_vec[0]*this->m_Point[i][0] + eigen_vec[1]*this->m_Point[i][1] + eigen_vec[2]*this->m_Point[i][2] 
				   -(eigen_vec[0]*max_point[0] + eigen_vec[1]*max_point[1] + 
				     eigen_vec[2]*max_point[2])))
		{
			dis=fabs(eigen_vec[0]*this->m_Point[i][0] + eigen_vec[1]*this->m_Point[i][1] + eigen_vec[2]*this->m_Point[i][2] 
				   -(eigen_vec[0]*max_point[0] + eigen_vec[1]*max_point[1] + 
					 eigen_vec[2]*max_point[2]));

			min_point = this->m_Point[i];

		}

	}

	XLine tLine;
	float tLen = (max_point-min_point).Length();
	PNT tEnd = cent+tLen*eigen_vec;
	tLine = XLine(cent, tEnd);

	fittedLine.SetStartPt(tLine.GetProjectPt(min_point));
	fittedLine.SetEndPt(tLine.GetProjectPt(max_point));
	fittedLine.SetCoff();

	double dist=0.0;
	for(i = 0 ; i<num; i++)
	{
		dist = dist + SQ(fittedLine.mL0*m_Point[i][0]+fittedLine.mL1*m_Point[i][1]+fittedLine.mL2);
	}
	dist = dist / (fittedLine.mL0*fittedLine.mL0 +fittedLine.mL1*fittedLine.mL1);
	dist = sqrt(dist/(num-1));
	fittedLine.m_RMS = dist;
	fittedLine.m_FitPntNum = num;
	return dist;
}
double XCloud::Fit2LineNEW(XLine &fittedLine,int s, int e)
{
	int i,j,status;
	double m00,m0,m01,m11,m1;
	float m,b,chi;
	PNT stp,edp;
	int	num = e-s+1;
	if( num < 3 )  return 0;

	
	m00=m11=m01=m0=m1=0;
	for(i=0;i<num;i++)
	{
		m00 += SQ(m_Point[i+s][0]); //SQ(s[i]);
		m0 += m_Point[i+s][0]; //s[i];
		m01 += m_Point[i+s][0]*m_Point[i+s][1]; //s[i]*t[i];
		m11 += SQ(m_Point[i+s][1]); //SQ(t[i]);
		m1 += m_Point[i+s][1]; //t[i];
	}
	m00 /=num;m11/=num;m01/=num;m0/=num;m1/=num;
	if ((m00-SQ(m0)) > (m11-SQ(m1)))
	{
	   m = (m01-m1*m0)/(m00-SQ(m0)); b= (m00*m1-m0*m01)/(m00-SQ(m0));
	   chi=0;
	   for(i=0;i<num;i++) 
		   chi += SQ(m_Point[i+s][1]-m*m_Point[i+s][0]-b);
	   chi = sqrt(chi/(num-1));
	   stp[0] = m_Point[s][0]; edp[0] = m_Point[e][0];
	   stp[1] = m*stp[0] + b;  edp[1] = m*edp[0] + b;
	   //l->x[0]=s[0];l->x[1]=s[num-1];
	   //l->y[0] =m*l->x[0]+b; l->y[1]=m*l->x[1]+b;  	   
	}
	else
	{
	   m = (m01-m1*m0)/(m11-SQ(m1)); b= (m11*m0-m1*m01)/(m11-SQ(m1));
	   chi=0;
	   for(i=0;i<num;i++) 
		   chi += SQ(m_Point[i+s][0]-m*m_Point[i+s][1]-b);
	   chi = sqrt(chi/(num-1)); //normalized
	   stp[1] = m_Point[s][1]; edp[1] = m_Point[e][1];
	   stp[0] = m*stp[1] + b;  edp[0] = m*edp[1] + b;
	   //l->y[0]=t[0];l->y[1]=t[num-1];
	   //l->x[0] =m*l->y[0]+b; l->x[1]=m*l->y[1]+b; 
	}
	fittedLine.SetStartPt(stp);
	fittedLine.SetEndPt(edp);
	fittedLine.SetCoff();
	//l->n[1] = l->x[1]-l->x[0];
	//l->n[0]= -l->y[1]+l->y[0];
	//
	//NORM(l->n);
	//l->d = l->n[0]*l->x[0] +l->n[1]*l->y[0];
	//status = test_if_line(num,s,t,l);

	//if(status ==0) 
	//	return 1;
	//else 
	//	return 0;
}

double XCloud::Fit2Line(XLine &fittedLine,int s, int e)
{
	double	a[3][3], b[3][3];
	double	min_eigenvalue;
	int		min_index, num;
	double	*mmatrix;
	int		i, j;
	double	eps=0.00000001;
	PNT		eigen_vec;

	num = e-s+1;
	if( num < 3 )  return 0;

	// Prepare matrix M:  [trans(M)*M] * a = lamda * a
	mmatrix = new double[ num*3 ];

	//calculate the centroid
	//PNT	cent = GetMassCenter();
	PNT    cent;
	for( i=0; i < num; i++)
	{
		cent += this->m_Point[i+s];
	}
	cent = cent/num;

	//set the origin as the line start
	for (i=0; i<num; i++)
	{
		mmatrix[3*i]	=	this->m_Point[i+s][0] - cent[0];
		mmatrix[3*i+1]	=	this->m_Point[i+s][1] - cent[1];
		mmatrix[3*i+2]	=	this->m_Point[i+s][2] - cent[2];
		//p=p->pnext;
	}

	//calculate the matrix
	for (i=0; i<3; i++)
		for(j=0; j<3; j++)
			a[i][j] = 0;

	for(i=0; i<num; i++)
	{
		a[0][0] += mmatrix[3*i+1] * mmatrix[3*i+1] + mmatrix[3*i+2] * mmatrix[3*i+2];
		a[1][1] += mmatrix[3*i+0] * mmatrix[3*i+0] + mmatrix[3*i+2] * mmatrix[3*i+2];
		a[2][2] += mmatrix[3*i+1] * mmatrix[3*i+1] + mmatrix[3*i+0] * mmatrix[3*i+0];

		a[0][1] -= mmatrix[3*i+0] * mmatrix[3*i+1];
		a[0][2] -= mmatrix[3*i+0] * mmatrix[3*i+2];
		a[1][2] -= mmatrix[3*i+1] * mmatrix[3*i+2];
	}
	a[1][0] = a[0][1];
	a[2][0] = a[0][2];
	a[2][1] = a[1][2];
	
	delete mmatrix;

	// Solve the eigenvector problem
	Ti_Optimization tMath;
	tMath.GetEigenvector(*a,3,*b,eps);

	min_eigenvalue = a[0][0];
	min_index = 0;
	for (i=1; i<3; i++)
	{
		if( min_eigenvalue>a[i][i] )
		{
			min_eigenvalue = a[i][i];
			min_index=i;
		}
	}

	for( i=0; i<3; i++)
	{
		eigen_vec[i] = b[i][min_index];
	}

	//eigen_vec would be the line direction	
	
	//Next:
	//Calculate the start and end points along this direction*/
	//double dis=0;
	//PNT max_point, min_point;

	////max point along this vector
	//for( i=0; i<num; i++ )
	//{
	//	if( dis<fabs(eigen_vec[0]*this->m_Point[i+s][0] + eigen_vec[1]*this->m_Point[i+s][1] + eigen_vec[2]*this->m_Point[i+s][2]))
	//	{
	//		dis=fabs(eigen_vec[0]*this->m_Point[i+s][0] + eigen_vec[1]*this->m_Point[i+s][1] + eigen_vec[2]*this->m_Point[i+s][2]);

	//		max_point = this->m_Point[i+s];
	//	}
	//}

	////min point along this vector
	//dis = 0;
	//for( i=0; i<num; i++ )
	//{
	//	if( dis<fabs(eigen_vec[0]*this->m_Point[i+s][0] + eigen_vec[1]*this->m_Point[i+s][1] + eigen_vec[2]*this->m_Point[i+s][2] 
	//			   -(eigen_vec[0]*max_point[0] + eigen_vec[1]*max_point[1] + 
	//			     eigen_vec[2]*max_point[2])))
	//	{
	//		dis=fabs(eigen_vec[0]*this->m_Point[i+s][0] + eigen_vec[1]*this->m_Point[i+s][1] + eigen_vec[2]*this->m_Point[i+s][2] 
	//			   -(eigen_vec[0]*max_point[0] + eigen_vec[1]*max_point[1] + 
	//				 eigen_vec[2]*max_point[2]));

	//		min_point = this->m_Point[i+s];

	//	}

	//}

	XLine tLine;
	float tLen = (m_Point[e]-m_Point[s]).Length();
	PNT tEnd = cent+tLen*eigen_vec;
	tLine = XLine(cent, tEnd);

	fittedLine.SetStartPt(tLine.GetProjectPt(m_Point[s]));
	fittedLine.SetEndPt(tLine.GetProjectPt(m_Point[e]));
	fittedLine.SetCoff();
}


void	XCloud::ViewAnalysis(bool bflag)
{
	if(this->m_pAnalysis.GetSize()<=0) return;
	if(bflag)
		for(int i = 0 ; i<m_pAnalysis.GetSize(); i++)
			(m_pAnalysis[i])->SetVisible(true);
	else
		for(int i = 0 ; i<m_pAnalysis.GetSize(); i++)
			(m_pAnalysis[i])->SetVisible(false);
	return;
}

void XCloud::ViewTolZone(bool bflag)
{
	if(bflag)
	{
		for(int i = 0 ; i < m_tolzone.m_TolGeo.GetSize(); i++)
			(m_tolzone.m_TolGeo[i])->SetVisible(true);
	}
	else
	{
		for(int i = 0 ; i < m_tolzone.m_TolGeo.GetSize(); i++)
			(m_tolzone.m_TolGeo[i])->SetVisible(false);
	}
}

void	XCloud::ResetPntColor(bool bf)
{
	for(size_t i = 0 ; i < m_Point.size(); i++)
	{
		if(m_Point[i].mpointsize !=1) continue;
		m_Point[i].mcolor = bf;
	}
}

void	XCloud::ResetPntSize(int bf)
{
	for(size_t i = 0 ; i < m_Point.size(); i++)
		m_Point[i].mpointsize = bf;
}

bool XCloud::JudgeTopologyEdgebreakTolzone(int sIndex, int eIndex, int edgeBreakType, int outNumber)
{
	int i;
	int fail;
	PNT out_pnt;

	fail = 0;
	outNumber = 0;

	for(i = sIndex; i <= eIndex; i ++)
	{
		if( !JudgeTopologyPntTolzone(m_Point[i], edgeBreakType) )
		{
			fail = 1;
			outNumber ++;
			//Charge the attribute of m_Point[i]
			m_Point[i].mcolor = 1;
			//
		}
	}

	if(fail)
		return false;	//Some points outside tolerance zone
	else
		return true;	//All points inside tolerance zone
}

bool XCloud::JudgeTopologyPntTolzone(PNT P, int edgeBreakType)
{
	PNT T1, T2, T3, T4, T5, T6, O;
	XCircle C0, C1, C2, C3;

	double dis_OP, dis_OT3;


	switch(edgeBreakType)
	{
	case 0:		//Roundedge
		{
			O[0] = m_tolzone.anchor_pt[0];	O[1] = m_tolzone.anchor_pt[1];

			T1[0] = m_tolzone.tp_4[0];	T1[1] = m_tolzone.tp_4[1];
			T2[0] = m_tolzone.tp_5[0];	T2[1] = m_tolzone.tp_5[1];

			T3[0] = m_tolzone.tp_3[0];	T3[1] = m_tolzone.tp_3[1];
			T4[0] = m_tolzone.tp_2[0];	T4[1] = m_tolzone.tp_2[1];

			T5[0] = m_tolzone.tp_0[0];	T5[1] = m_tolzone.tp_0[1];
			T6[0] = m_tolzone.tp_1[0];	T6[1] = m_tolzone.tp_1[1];

			C1 = *((XCircle*)(m_tolzone.m_TolGeo[1]));
			C2 = *((XCircle*)(m_tolzone.m_TolGeo[2]));
			C3 = *((XCircle*)(m_tolzone.m_TolGeo[0]));

			dis_OP  = (O[0]-P[0])*(O[0]-P[0]) + (O[1]-P[1])*(O[1]-P[1]);
			dis_OT3 = (O[0]-T3[0])*(O[0]-T3[0]) + (O[1]-T3[1])*(O[1]-T3[1]);

			if (!JudgeTopologyPntTriangle(P, T5, O, T6) )
				return false;										//outside
			else if (JudgeTopologyPntCircle(P, C1) )
				return false;										//outside
			else if (JudgeTopologyPntCircle(P, C2) )
				return false;										//outside
			else if (!JudgeTopologyPntLineSegmentInOut(P, T1, T2) )
				return true;										//inside
			else if (!JudgeTopologyPntLineLeftRight(P, O, T1, T2) )
				return false;										//outside
			else if ( JudgeTopologyPntCircle(P, C3) )
				return true;										//inside
			else if ( dis_OP > dis_OT3 )
				return true;										//inside
			else
				return false;										//outside
		}
		break;
	case 5:		//Chamfer  1 for old chamfer now
		{
			O[0] = m_tolzone.anchor_pt[0];	O[1] = m_tolzone.anchor_pt[1];

			T3[0] = m_tolzone.tp_2[0];	T3[1] = m_tolzone.tp_2[1];
			T4[0] = m_tolzone.tp_0[0];	T4[1] = m_tolzone.tp_0[1];

			T5[0] = m_tolzone.tp_1[0];	T5[1] = m_tolzone.tp_1[1];
			T6[0] = m_tolzone.tp_3[0];	T6[1] = m_tolzone.tp_3[1];

			if ( (!JudgeTopologyPntTriangle(P, T4, O, T3)) && JudgeTopologyPntTriangle(P, T5, O, T6) )
				return true;										//inside
			else
				return false;										//outside
		}
		break;
	case 2:		//Radius
		{
			O[0] = m_tolzone.anchor_pt[0];	O[1] = m_tolzone.anchor_pt[1];

			T3[0] = m_tolzone.tp_3[0];	T3[1] = m_tolzone.tp_3[1];
			T4[0] = m_tolzone.tp_2[0];	T4[1] = m_tolzone.tp_2[1];

			T5[0] = m_tolzone.tp_0[0];	T5[1] = m_tolzone.tp_0[1];
			T6[0] = m_tolzone.tp_1[0];	T6[1] = m_tolzone.tp_1[1];

			C0 = *((XCircle*)(m_tolzone.m_TolGeo[0]));
			C3 = *((XCircle*)(m_tolzone.m_TolGeo[1]));

			dis_OP  = (O[0]-P[0])*(O[0]-P[0]) + (O[1]-P[1])*(O[1]-P[1]);
			dis_OT3 = (O[0]-T3[0])*(O[0]-T3[0]) + (O[1]-T3[1])*(O[1]-T3[1]);

			if (!JudgeTopologyPntTriangle(P, T5, O, T6) )
				return false;										//outside
			else if (JudgeTopologyPntCircle(P, C0) )
				return false;										//outside
			else if (JudgeTopologyPntCircle(P, C3) )
				return true;										//inside
			else if ( dis_OP > dis_OT3 )
				return true;										//inside
			else
				return false;										//outside
		}
		break;

		case 4:		//added by ny
		{
			O[0] = m_tolzone.anchor_pt[0];	O[1] = m_tolzone.anchor_pt[1];

			T1[0] = m_tolzone.tp_4[0];	T1[1] = m_tolzone.tp_4[1];
			T2[0] = m_tolzone.tp_5[0];	T2[1] = m_tolzone.tp_5[1];

			T3[0] = m_tolzone.tp_3[0];	T3[1] = m_tolzone.tp_3[1];
			T4[0] = m_tolzone.tp_2[0];	T4[1] = m_tolzone.tp_2[1];

			T5[0] = m_tolzone.tp_0[0];	T5[1] = m_tolzone.tp_0[1];
			T6[0] = m_tolzone.tp_1[0];	T6[1] = m_tolzone.tp_1[1];

			C1 = *((XCircle*)(m_tolzone.m_TolGeo[1]));
			C2 = *((XCircle*)(m_tolzone.m_TolGeo[2]));
			C3 = *((XCircle*)(m_tolzone.m_TolGeo[0]));

			dis_OP  = (O[0]-P[0])*(O[0]-P[0]) + (O[1]-P[1])*(O[1]-P[1]);
			dis_OT3 = (O[0]-T3[0])*(O[0]-T3[0]) + (O[1]-T3[1])*(O[1]-T3[1]);

			if (!JudgeTopologyPntTriangle(P, T5, O, T6) )
				return false;										//outside
			else if (JudgeTopologyPntCircle(P, C1) )
				return false;										//outside
			else if (JudgeTopologyPntCircle(P, C2) )
				return false;										//outside
			else if (!JudgeTopologyPntLineSegmentInOut(P, T1, T2) )
				return true;										//inside
			else if (!JudgeTopologyPntLineLeftRight(P, O, T1, T2) )
				return false;										//outside
			else if ( JudgeTopologyPntCircle(P, C3) )
				return true;										//inside
			else if ( dis_OP > dis_OT3 )
				return true;										//inside
			else
				return false;										//outside
		}
		break; 

	default: 
		break;
	}

}

bool XCloud::JudgeTopologyPntTriangle(PNT P, PNT P1, PNT P2, PNT P3)
{
	PNT P1P2, P2P3, P3P1;
	PNT P1P, P2P, P3P;
	PNT P1P2P, P2P3P, P3P1P;
	float sign1, sign2, sign3;

	P1P2 = P2 - P1;
	P2P3 = P3 - P2;
	P3P1 = P1 - P3;

	P1P  = P - P1;
	P2P  = P - P2;
	P3P  = P - P3;

	P1P2P = P1P2 ^ P2P;
	P2P3P = P2P3 ^ P3P;
	P3P1P = P3P1 ^ P1P;

	sign1 = P1P2P[2];
	sign2 = P2P3P[2];
	sign3 = P3P1P[2];

	if( (sign1 >= 0 && sign2 >= 0 && sign3 >=0) || (sign1 <= 0 && sign2 <= 0 && sign3 <=0) )
		return true;			//inside
	else
		return false;			//outside
}

bool XCloud::JudgeTopologyPntCircle(PNT P, XCircle C)
{
	double dis, r;
	PNT Cen;

	Cen = C.m_Center;
	r   = C.m_Radius;

	dis = sqrt((P[0]-Cen[0])*(P[0]-Cen[0]) + (P[1]-Cen[1])*(P[1]-Cen[1]));

	if(dis < r)
		return true;		//inside the circle
	else
		return false;		//on/outside the circle
	
}

bool XCloud::JudgeTopologyPntLineSegmentInOut(PNT P, PNT P1, PNT P2)
{
	PNT P1P2, P2P1;
	PNT P1P, P2P;

	P1P2 = P2 - P1;
	P2P1 = P1 - P2;

	P1P  = P - P1;
	P2P  = P - P2;

	if( (P1P2*P1P >= 0) && (P2P1*P2P >= 0) )
		return true;			//inside line segment P1P2
	else
		return false;			//outside line segment P1P2
	
}

bool XCloud::JudgeTopologyPntLineLeftRight(PNT P, PNT Ref, PNT P1, PNT P2)
{
	PNT P1P2, P1P, P1R;

	PNT P1P2P, P1P2R;

	double sign;

	P1P2 = P2 - P1;
	P1P  = P  - P1;
	P1R  = Ref - P1;

	P1P2P = P1P2 ^ P1P;
	P1P2R = P1P2 ^ P1R;

	sign = P1P2P[2]*P1P2R[2];

	if( sign >= 0)
		return true;			//same side with ref point 
	else
		return false;			//different side with ref point

}
//End of judgment

void XCloud::AverageFilter()
{

}

void XCloud::AutoTrim(int box, bool ask)  //n added sth for auto trim
{

	//linearization the profile and use Graham scan to find the 
	//noisy points
	//tnnd
	//by JM
	//////////////////////////////////////////////////////////////////////////

	int NumberofBox,tenper,st,ed;
	int pointsinbox=50;
	int pntnum;
	int result=0;
	int tempask=0;
    int oo1;

	if (ask)
	{
		if(AfxMessageBox(TEXT("autotrim?"),MB_YESNO) == IDYES)
			tempask=1;

	}
	else
		tempask=1;

	if(tempask==1)
		do
		{

			////////This part is for line trim


			pntnum = GetPntNum();

			int TotalTrimBox=0;
			double sumtemp=0.0;

			XLine	line;
			double	sum, samdis1,samdis2, NONLINE=3.0;
			int		Numdeletes = 0;
			int		Numdeletee = 0;
			double pointdisinbox1,yaverage;

			CString tt;
			NumberofBox=int(pntnum/box);
			tenper=int(pntnum/box/15);


			EstimatedSampleDistance();
			samdis1 = sample_dist1;
			samdis2 = sample_dist2;

			if( box*2 > pntnum-1 ) return;

			for(int i = 1; i < pntnum/2 - box; i+=box)
			{
				this->Fit2Line(line,i,i+box);

				sum = 0.0;
				sumtemp=0.0;
				for (int j = i; j < i+box; j ++)
					sum = sum + line.Distance(m_Point[j]);
				pointdisinbox1=0;
				yaverage=0;
				for (int j = i; j < i+box*4; j ++)
				{
					yaverage+=m_Point[j][1]/box/4;
					pointdisinbox1+=sqrt(SQ(m_Point[j][0]-m_Point[j-1][0])+SQ(m_Point[j][1]-m_Point[j-1][1]));

					sumtemp = sumtemp + line.Distance(m_Point[j]);
				}


				//	tt.Format("sum%f,temp%f,dis%f,box%dttt%d", sum/box, sumtemp/box/1,samdis1*3,box,TotalTrimBox);

				//		AfxMessageBox(tt);

				if(((sum/box) > (samdis1*NONLINE))||((sumtemp/box/4)> (samdis1*NONLINE)))//||(i<(tenper*box))||/*((pointdisinbox1/box/5)*0.9>(samdis1*2))||*/(((yaverage-minyvalue)/(maxyvalue-minyvalue))>0.85))
				{

					Numdeletes += box;
					TotalTrimBox++;
				}
				else

					break;

			}

			for(int i = pntnum-1; i > pntnum/2 + box; i -= box)
			{
				this->Fit2Line(line,i-box,i);

				sum = 0.0;
				sumtemp=0.0;
				for (int j = i-box; j < i; j ++)
					sum = sum + line.Distance(m_Point[j]);
				pointdisinbox1=0;
				yaverage=0;
				for (int j = i-box*4; j < i; j ++)
				{

					pointdisinbox1+=sqrt(SQ(m_Point[j][0]-m_Point[j-1][0])+SQ(m_Point[j][1]-m_Point[j-1][1]));
					yaverage+=m_Point[j][1]/box/4;
					sumtemp = sumtemp + line.Distance(m_Point[j]);
				}
				//					tt.Format("2sum%f,temp%f,dis%f,box%dttt%d", sum/box, sumtemp/box/5,samdis*3,box,TotalTrimBox);
				//AfxMessageBox(tt);
				if(((sum/box) > (samdis2*NONLINE))||((sumtemp/box/4) > (samdis2*NONLINE)))//||(i>(pntnum-tenper*box))||/*((pointdisinbox1/box/5)*0.9>(samdis2*2))||*/(((yaverage-minyvalue)/(maxyvalue-minyvalue))>0.85))
				{   
					TotalTrimBox++;
					Numdeletee += box;
				}
				else
				{ 
					break;
				}
			}
			tt.Format(TEXT("%d,%d"), Numdeletes,Numdeletee);
			//AfxMessageBox(tt);
			if(ask)
			{
				//if(AfxMessageBox("autotrim?",MB_YESNO) == IDYES)
				{
					if (TotalTrimBox<(int(NumberofBox*0.7)))
					{

						DeletePoint(pntnum-Numdeletee,Numdeletee);

						DeletePoint(0,Numdeletes);
					}

					else 
					{			
						result=1;

					}
				}
			}
			else
			{
				if (TotalTrimBox<(int(NumberofBox*0.7)))
				{

					DeletePoint(pntnum-Numdeletee,Numdeletee);

					DeletePoint(0,Numdeletes);
				}

				else 
				{		
					result=1;

				}
			}
			////////This part is for line trim




			////////This part is for turn trim 



			CArray<PNT,PNT> ptarr;
			ptarr.RemoveAll();
			pntnum = GetPntNum();
			if (pntnum<400) result=1;
			else
			{
				NumberofBox = pntnum/pointsinbox;
				for(int oo = 0; oo < NumberofBox; oo++)
				{
					st = oo*pointsinbox;
					if(oo==NumberofBox-1)
						ed = pntnum;
					else
						ed = st+pointsinbox;

					//construct vector family here
					PNT myline = m_Point[ed]-m_Point[st];
					myline.Normalize();
					ptarr.Add(myline);
					//m_Point[ed].mpointsize = 7;
					//m_Point[ed].mcolor = 6;
					//m_Point[st].mpointsize = 7;
					//m_Point[st].mcolor = 6;
				}

				//calculate the angle of two vector
				CArray<int,int> anarr;
				anarr.RemoveAll();
				for(int oo = 0; oo<ptarr.GetSize()-1; oo++)
				{
					PNT csp = ptarr[oo]^ptarr[oo+1];
					//csp.Normalize();
					if(csp[2] > 0.1)
						anarr.Add(1);
					else if(csp[2] < -0.1)
						anarr.Add(-1);
					else
						anarr.Add(0); 
				}

				int firstsign=0;
				int presign=0;
				int prepos=0;
				int TurnSign;
				int leftcutpos,rightcutpos,leftcutpoints,rightcutpoints,negativenum=0,positivenum=0;

				for(int oo = 0; oo< anarr.GetSize(); oo++)
				{
					int ggg=anarr[oo];
					if ((anarr[oo]==1)||(anarr[oo]==-1))
					{
						firstsign++;

						if (firstsign==1)
						{
							for(int oo1 = 0; oo1< oo; oo1++)
							{
								if (anarr[oo]==1)
									positivenum++;
								else
									negativenum++;

								anarr[oo1]=anarr[oo];
							}
							if (anarr[oo]==1)
								positivenum++;
							else
								negativenum++;

							prepos=oo;
							presign=anarr[oo];
						}
						else //firstsign>1
						{
							if (anarr[oo]==presign)
							{
								for(int oo1 = prepos+1; oo1< oo; oo1++)
								{
									if (anarr[oo]==1)
										positivenum++;
									else
										negativenum++;
									anarr[oo1]=anarr[oo];
								}
							}
							prepos=oo;
							presign=anarr[oo];
							if (anarr[oo]==1)
								positivenum++;
							else
								negativenum++;
						}
					}

					for(oo1 = prepos+1; oo1< oo; oo1++)
					{
						if (presign==1)
							positivenum++;
						else
							negativenum++;
						anarr[oo1]=presign;
					}
				}

				


				if (negativenum>positivenum)
					TurnSign=-1;
				else
					TurnSign=1;

				leftcutpos=0;
				int pp=0;
				do
				{
					if ((anarr[leftcutpos]==-TurnSign)||(anarr[leftcutpos+1]==-TurnSign)||(anarr[leftcutpos+2]==-TurnSign))
					{
						leftcutpos++;
						if (leftcutpos>(NumberofBox/2))
							pp=2;
					}
					else
						pp=1;
				} while (pp==0);
				if (pp==2)
					result=1;
				else
				{
					pp=0;
					rightcutpos=anarr.GetSize()-1;
					do
					{
						if ((anarr[rightcutpos]==-TurnSign)||(anarr[rightcutpos-1]==-TurnSign)||(anarr[rightcutpos-2]==-TurnSign))
						{
							rightcutpos--;
							if (rightcutpos<(NumberofBox/2))
								pp=2;
						}
						else pp=1;

					} while (pp==0);
				}
				if (pp==2)
					result=1;
				else
				{

					leftcutpoints=pointsinbox*leftcutpos;
					rightcutpoints=pointsinbox*(NumberofBox-2-rightcutpos);
					if ((leftcutpoints==rightcutpoints)&&(rightcutpoints==0))
						result=2;


					DeletePoint(pntnum-rightcutpoints,rightcutpoints);

					DeletePoint(0,leftcutpoints);
				}
			}

			////////This part is for turn trim 
		} while (result==0);
	if (ask)
	{

		if (result==1)
			AfxMessageBox(TEXT(" Auto-trim cannot finish automatically due to the poor image quality, you can trim it manually or snap another image!"));
	}

		//count if contour is left turn or right turn;
		//keep the most turns as profile
		/*CArray<int, int> signarr;
		signarr.RemoveAll();
		int currentsign = 0;
		for(int oo = 0; oo< anarr.GetSize(); oo++)
		{
		if(anarr[oo]==-1)
		{
		if(currentsign == 0)//first round
		signarr.Add(-1);
		else if(currentsign == -1)//keep same sign
		signarr[signarr.GetSize()-1] = signarr[signarr.GetSize()-1]-1;
		else if(currentsign == 1)//change to another sign
		signarr.Add(-1);

		currentsign = -1;
		}
		if(anarr[oo] == 1)
		{
		if(currentsign == 0)//first round
		signarr.Add(1);
		else if(currentsign == -1)//change to another sign
		signarr.Add(-1);
		else if(currentsign == 1)//keep same sign
		signarr[signarr.GetSize()-1] = signarr[signarr.GetSize()-1]+1;

		currentsign = 1;

		}
		}


		*/





		//////////////////////////////////////////////////////////////////////////

		EstimatedSampleDistance();
		return;


	
}

int XCloud::AutoTrim(int box, bool ask,int linetrimstep)  //n added sth for auto trim//Jan19
{

	//linearization the profile and use Graham scan to find the 
	//noisy points
	//tnnd
	//by JM
	//////////////////////////////////////////////////////////////////////////

	int NumberofBox,tenper,st,ed;
	int pointsinbox=50;
	int pntnum;
	int result=0;
	int tempask=0;
	XLine	line;//Jan19
	double	sum, samdis1,samdis2, NONLINE=3.0;//Jan19
	if (ask)
	{
		if(AfxMessageBox(TEXT("autotrim?"),MB_YESNO) == IDYES)
			tempask=1;

	}
	else
		tempask=1;

	if(tempask==1)
		do
		{

			////////This part is for line trim


			pntnum = GetPntNum();

			int TotalTrimBox=0;
			double sumtemp=0.0;

			//XLine	line;//Jan19

			int		Numdeletes = 0;
			int		Numdeletee = 0;
			double pointdisinbox1,yaverage;

			CString tt;
			NumberofBox=int(pntnum/box);
			tenper=int(pntnum/box/15);


			EstimatedSampleDistance();
			samdis1 = sample_dist1;
			samdis2 = sample_dist2;

			if( box*2 > pntnum-1 ) return 1; //Jan19

			for(int i = 1; i < pntnum/2 - box; i+=box)
			{
				this->Fit2Line(line,i,i+box);

				sum = 0.0;
				sumtemp=0.0;
				for (int j = i; j < i+box; j ++)
					sum = sum + line.Distance(m_Point[j]);
				pointdisinbox1=0;
				yaverage=0;
				for (int j = i; j < i+box*linetrimstep; j ++)//Jan19
				{
					yaverage+=m_Point[j][1]/box/linetrimstep;//Jan19
					pointdisinbox1+=sqrt(SQ(m_Point[j][0]-m_Point[j-1][0])+SQ(m_Point[j][1]-m_Point[j-1][1]));

					sumtemp = sumtemp + line.Distance(m_Point[j]);
				}


				//	tt.Format("sum%f,temp%f,dis%f,box%dttt%d", sum/box, sumtemp/box/1,samdis1*3,box,TotalTrimBox);

				//		AfxMessageBox(tt);

				if(((sum/box) > (samdis1*NONLINE))||((sumtemp/box/linetrimstep)> (samdis1*NONLINE)))//||(i<(tenper*box))||/*((pointdisinbox1/box/5)*0.9>(samdis1*2))||*/(((yaverage-minyvalue)/(maxyvalue-minyvalue))>0.85))//Jan19
				{

					Numdeletes += box;
					TotalTrimBox++;
				}
				else

					break;

			}

			for(int i = pntnum-1; i > pntnum/2 + box; i -= box)
			{
				this->Fit2Line(line,i-box,i);

				sum = 0.0;
				sumtemp=0.0;
				for (int j = i-box; j < i; j ++)
					sum = sum + line.Distance(m_Point[j]);
				pointdisinbox1=0;
				yaverage=0;
				for (int j = i-box*linetrimstep; j < i; j ++)//Jan19
				{

					pointdisinbox1+=sqrt(SQ(m_Point[j][0]-m_Point[j-1][0])+SQ(m_Point[j][1]-m_Point[j-1][1]));
					yaverage+=m_Point[j][1]/box/linetrimstep;//Jan19
					sumtemp = sumtemp + line.Distance(m_Point[j]);
				}
				//					tt.Format("2sum%f,temp%f,dis%f,box%dttt%d", sum/box, sumtemp/box/5,samdis*3,box,TotalTrimBox);
				//AfxMessageBox(tt);
				if(((sum/box) > (samdis2*NONLINE))||((sumtemp/box/linetrimstep) > (samdis2*NONLINE)))//||(i>(pntnum-tenper*box))||/*((pointdisinbox1/box/5)*0.9>(samdis2*2))||*/(((yaverage-minyvalue)/(maxyvalue-minyvalue))>0.85))//Jan19
				{   
					TotalTrimBox++;
					Numdeletee += box;
				}
				else
				{ 
					break;
				}
			}
			tt.Format(TEXT("%d,%d"), Numdeletes,Numdeletee);
			//AfxMessageBox(tt);
			if(ask)
			{
				//if(AfxMessageBox("autotrim?",MB_YESNO) == IDYES)
				{
					if (TotalTrimBox<(int(NumberofBox*0.7)))
					{

						DeletePoint(pntnum-Numdeletee,Numdeletee);

						DeletePoint(0,Numdeletes);
					}

					else 
					{			
						result=1;

					}
				}
			}
			else
			{
				if (TotalTrimBox<(int(NumberofBox*0.7)))
				{

					DeletePoint(pntnum-Numdeletee,Numdeletee);

					DeletePoint(0,Numdeletes);
				}

				else 
				{		
					result=1;

				}
			}
			////////This part is for line trim




			////////This part is for turn trim 



			CArray<PNT,PNT> ptarr;
			ptarr.RemoveAll();
			pntnum = GetPntNum();
			if (pntnum<400) result=1;
			else
			{
				NumberofBox = pntnum/pointsinbox;
				for(int oo = 0; oo < NumberofBox; oo++)
				{
					st = oo*pointsinbox;
					if(oo==NumberofBox-1)
						ed = pntnum;
					else
						ed = st+pointsinbox;

					//construct vector family here
					PNT myline = m_Point[ed]-m_Point[st];
					myline.Normalize();
					ptarr.Add(myline);
					//m_Point[ed].mpointsize = 7;
					//m_Point[ed].mcolor = 6;
					//m_Point[st].mpointsize = 7;
					//m_Point[st].mcolor = 6;
				}

				//calculate the angle of two vector
				CArray<int,int> anarr;
				anarr.RemoveAll();
				for(int oo = 0; oo<ptarr.GetSize()-1; oo++)
				{
					PNT csp = ptarr[oo]^ptarr[oo+1];
					//csp.Normalize();
					if(csp[2] > 0.1)
						anarr.Add(1);
					else if(csp[2] < -0.1)
						anarr.Add(-1);
					else
						anarr.Add(0); 
				}

				int firstsign=0;
				int presign=0;
				int prepos=0;
				int TurnSign;
				int leftcutpos,rightcutpos,leftcutpoints,rightcutpoints,negativenum=0,positivenum=0;

				for(int oo = 0; oo< anarr.GetSize(); oo++)
				{
					int ggg=anarr[oo];
					if ((anarr[oo]==1)||(anarr[oo]==-1))
					{
						firstsign++;

						if (firstsign==1)
						{
							for(int oo1 = 0; oo1< oo; oo1++)
							{
								if (anarr[oo]==1)
									positivenum++;
								else
									negativenum++;

								anarr[oo1]=anarr[oo];
							}
							if (anarr[oo]==1)
								positivenum++;
							else
								negativenum++;

							prepos=oo;
							presign=anarr[oo];
						}
						else //firstsign>1
						{
							if (anarr[oo]==presign)
							{
								for(int oo1 = prepos+1; oo1< oo; oo1++)
								{
									if (anarr[oo]==1)
										positivenum++;
									else
										negativenum++;
									anarr[oo1]=anarr[oo];
								}
							}
							prepos=oo;
							presign=anarr[oo];
							if (anarr[oo]==1)
								positivenum++;
							else
								negativenum++;
						}
					}
					for(int oo1 = prepos+1; oo1< oo; oo1++)
					{
						if (presign==1)
							positivenum++;
						else
							negativenum++;
						anarr[oo1]=presign;
					}
				}

				


				if (negativenum>positivenum)
					TurnSign=-1;
				else
					TurnSign=1;

				leftcutpos=0;
				int pp=0;
				do
				{
					if ((anarr[leftcutpos]==-TurnSign)||(anarr[leftcutpos+1]==-TurnSign)||(anarr[leftcutpos+2]==-TurnSign))
					{
						leftcutpos++;
						if (leftcutpos>(NumberofBox/2))
							pp=2;
					}
					else
						pp=1;
				} while (pp==0);
				if (pp==2)
					result=1;
				else
				{
					pp=0;
					rightcutpos=anarr.GetSize()-1;
					do
					{
						if ((anarr[rightcutpos]==-TurnSign)||(anarr[rightcutpos-1]==-TurnSign)||(anarr[rightcutpos-2]==-TurnSign))
						{
							rightcutpos--;
							if (rightcutpos<(NumberofBox/2))
								pp=2;
						}
						else pp=1;

					} while (pp==0);
				}
				if (pp==2)
					result=1;
				else
				{

					leftcutpoints=pointsinbox*leftcutpos;
					rightcutpoints=pointsinbox*(NumberofBox-2-rightcutpos);
					if ((leftcutpoints==rightcutpoints)&&(rightcutpoints==0))
						result=2;


					DeletePoint(pntnum-rightcutpoints,rightcutpoints);

					DeletePoint(0,leftcutpoints);
				}
			}

			////////This part is for turn trim 
		} while (result==0);
	if (ask)
	{

		//if (result==1) Jan19
		//	AfxMessageBox(" Auto-trim cannot finish automatically due to the poor image quality, you can trim it manually or snap another image!"); //Jan19
	}

		//count if contour is left turn or right turn;
		//keep the most turns as profile
		/*CArray<int, int> signarr;
		signarr.RemoveAll();
		int currentsign = 0;
		for(int oo = 0; oo< anarr.GetSize(); oo++)
		{
		if(anarr[oo]==-1)
		{
		if(currentsign == 0)//first round
		signarr.Add(-1);
		else if(currentsign == -1)//keep same sign
		signarr[signarr.GetSize()-1] = signarr[signarr.GetSize()-1]-1;
		else if(currentsign == 1)//change to another sign
		signarr.Add(-1);

		currentsign = -1;
		}
		if(anarr[oo] == 1)
		{
		if(currentsign == 0)//first round
		signarr.Add(1);
		else if(currentsign == -1)//change to another sign
		signarr.Add(-1);
		else if(currentsign == 1)//keep same sign
		signarr[signarr.GetSize()-1] = signarr[signarr.GetSize()-1]+1;

		currentsign = 1;

		}
		}


		*/



// AfxMessageBox("in the trim");

		//////////////////////////////////////////////////////////////////////////


		EstimatedSampleDistance();
		//AfxMessageBox("in the trim -111");

		double minvalue=10000;//Jan19
		double leftmaxvalue=-10000.0;//Jan19
		double rightmaxvalue=-10000.0;//Jan19
		double sum1;
		int midpoint=0;
		if (result==2)//Jan19
		{//Jan19
//AfxMessageBox("in the trim 000");
		pntnum = GetPntNum();//Jan19
//AfxMessageBox("in the trim 111");
		//this->Fit2Line(line,1,pntnum);//Jan19

		sum1 = 0.0;//Jan19
		for (int j = 0; j < pntnum-30; j ++)//Jan19
		{
			sum1 = 0.0;
			for (int i=0;i<30;i++)
			sum1 = sum1 + m_Point[j+i][1]/30;//Jan19
			if (sum1 <minvalue)
			{
				minvalue=sum1;
				midpoint=j;
			}
		}//Jan19
		//AfxMessageBox("in the trim  222");
		for (int j = 0; j < midpoint-30; j ++)//Jan19
		{
			sum1 = 0.0;
			for (int i=0;i<30;i++)
			sum1 = sum1 + m_Point[j+i][1]/30;//Jan19
			if (sum1 >leftmaxvalue)
			{
				leftmaxvalue=sum1;
			}
		}//Jan19
//AfxMessageBox("in the trim 333");
		for (int j = midpoint; j < pntnum-30; j ++)//Jan19
		{
			sum1 = 0.0;
			for (int i=0;i<30;i++)
			sum1 = sum1 + m_Point[j+i][1]/30;//Jan19
			if (sum1 >rightmaxvalue)
			{
				rightmaxvalue=sum1;
			}
		}//Jan19

		
	if (((leftmaxvalue-minvalue)<(samdis1*(pntnum/9)))||((rightmaxvalue-minvalue)<(samdis2*(pntnum/9))))
			result=1;//Jan19
				CString mess;
mess.Format(TEXT("%f,%f,%f,%f,%f"),minvalue,leftmaxvalue,rightmaxvalue,samdis1,samdis2);
	//if (ask)
	
//AfxMessageBox(mess);
		}
   


//AfxMessageBox("in the trim rrrr");

		return result; //Jan19


	
}


//by jm Feb 7 2006 **************************//
bool XCloud::CalBox(int start, int end, double bl[3], double ur[3])
{
	if(start<0 || end >= GetPntNum()) return false;

	if(start >=end ) return false;

	double minx = 100000000, miny = 100000000;
	double maxx = -111111111, maxy = -111111111;
	for(int i = start; i <= end ; i++)
	{
		if(m_Point[i][0] > maxx)
			maxx = m_Point[i][0];
		if(m_Point[i][1] > maxy)
			maxy = m_Point[i][1];
		if(m_Point[i][0] < minx)
			minx = m_Point[i][0];
		if(m_Point[i][1] < miny)
			miny = m_Point[i][1];
	}
	bl[0]=minx;
	bl[1]=miny;
	bl[2]=0;
	ur[0]=maxx;
	ur[1]=maxy;
	ur[2]=0;
	return true;
}



/********************************************************************
Calculator:    Save File with only path name  
Description: Created on 2006/06/13   
History:
Calls:          
Called By:      
Input:          
Output:         
Return:
Others:         
**********************************************************************/
bool XCloud::SaveFile_Asc(CString FilePath)
{
	CFile pfile(FilePath, CFile::modeCreate|CFile::modeWrite);
	{
		CArchive ar(&pfile, CArchive::store);
		if( SaveFile_Asc(ar))
		{
			ar.Close();
			pfile.Close();
			return true;
		}
	}		
	return false;
}


void XCloud::CalCurvature(float *CldCurvature)
{		
	int i;
    Ti_point *TmpPt;
	Ti_cld_point   ti_cld;
	
	Ti_cld_point *NearTi_cld;

	XCloud cld(*this);
     
	XCloud TmpCld;
	int HalfNSize=6;
    int CurX=0,CurY=0;
	int num = cld.GetPntNum();
    PNT TmpCldPt;
    int XF=1,YF=1;
    int XHF=0,YHF=0;

	ti_cld.XCloud2Ti_Cloud(cld);

	for( i = 0 , TmpPt = ti_cld.GetHead(); i < num ; i++)
	{
			TmpCld.InitObject();
			for (int ii=-HalfNSize;ii<=HalfNSize;ii++)
			    for (int jj=-HalfNSize;jj<=HalfNSize;jj++)
			    {
					if (CurX+jj>m_cldwidth)					
						      XF=-1,XHF=-1;
					if(CurX+jj<0)
							  XF=-1,XHF=1;
					if (CurY+ii>m_cldheight) 
						YF=-1,YHF=-1;
                    if(CurY+ii<0)		
						YF=-1,YHF=1;

					cld.GetPoint((CurY+YHF*HalfNSize+YF*ii)*m_cldwidth+CurX+XHF*HalfNSize+XF*jj,TmpCldPt);
					TmpCld.AddPoint(TmpCldPt);					
					XF=1,YF=1,XHF=0;YHF=0;
			    }			

				NearTi_cld = new Ti_cld_point;
				NearTi_cld->InitObject();
				TmpCld.SampleUniform(2);
				NearTi_cld->XCloud2Ti_Cloud(TmpCld);

			////////////////////////////////
			if( ti_cld.CalcPriCur(TmpPt,NearTi_cld))
			CldCurvature[i]=fabs(TmpPt->GetMaxCur()*TmpPt->GetMinCur()/10000000.0);
			//   CldCurvature[i]=(abs(TmpPt->GetMaxCur())+abs(TmpPt->GetMinCur()))/200000.0;
			else
				CldCurvature[i]=0;

			TmpPt = TmpPt->GetNext();

		//	delete NearTi_cld;
		//	NearTi_cld = NULL;
			
			CurX++;  
			if(CurX==m_cldwidth)
			{
				CurX=0;  
				CurY++;   
			}

			if(TmpPt == NULL)
			break;
	}
}



// chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
//     Input:  *this= an array of 2D points 
//                   presorted by increasing x- and y-coordinates
//     Output: H[] = an array of the convex hull vertices (max is n)
//     Return: the number of points in H[]
int XCloud::ChainHull_2D(XCloud& H )
{
	// the output array H[] will be used as the stack
	XCloud P = *this;
	H = *this;
	int n = GetPntNum();
	int    bot=0, top=(-1);  // indices for bottom and top of the stack
	int    i;                // array scan index
	P.Sort(1);
	// Get the indices of points with min x-coord and min|max y-coord
	int minmin = 0, minmax;
	float xmin = P[0][0];
	for (i=1; i<n; i++)
		if (P[i][0] != xmin) break;
	minmax = i-1;
	if (minmax == n-1)
	{       // degenerate case: all x-coords == xmin
		H[++top] = P[minmin];
		if (P[minmax][1] != P[minmin][1]) // a nontrivial segment
			H[++top] = P[minmax];
		H[++top] = P[minmin];           // add polygon endpoint
		return top+1;
	}

	// Get the indices of points with max x-coord and min|max y-coord
	int maxmin, maxmax = n-1;
	float xmax = P[n-1][0];
	for (i=n-2; i>=0; i--)
		if (P[i][0] != xmax) break;
	maxmin = i+1;

	// Compute the lower hull on the stack H
	H[++top] = P[minmin];      // push minmin point onto stack
	i = minmax;
	while (++i <= maxmin)
	{
		// the lower line joins P[minmin] with P[maxmin]
		if (P[i].IsPntLeft( P[minmin], P[maxmin]) >= 0 && i < maxmin)
			continue;          // ignore P[i] above or on the lower line

		while (top > 0)        // there are at least 2 points on the stack
		{
			// test if P[i] is left of the line at the stack top
			if ( P[i].IsPntLeft( H[top-1], H[top]) > 0)
				break;         // P[i] is a new hull vertex
			else
				top--;         // pop top point off stack
		}
		H[++top] = P[i];       // push P[i] onto stack
	}

	// Next, compute the upper hull on the stack H above the bottom hull
	if (maxmax != maxmin)      // if distinct xmax points
		H[++top] = P[maxmax];  // push maxmax point onto stack
	bot = top;                 // the bottom point of the upper hull stack
	i = maxmin;
	while (--i >= minmax)
	{
		// the upper line joins P[maxmax] with P[minmax]
		if (P[i].IsPntLeft( P[maxmax], P[minmax]) >= 0 && i > minmax)
			continue;          // ignore P[i] below or on the upper line

		while (top > bot)    // at least 2 points on the upper stack
		{
			// test if P[i] is left of the line at the stack top
			if (P[i].IsPntLeft( H[top-1], H[top]) > 0)
				break;         // P[i] is a new hull vertex
			else
				top--;         // pop top point off stack
		}
		H[++top] = P[i];       // push P[i] onto stack
	}
	if (minmax != minmin)
		H[++top] = P[minmin];  // push joining endpoint onto stack
	H.DeletePoint(top, n-top);
	return top+1;
}




