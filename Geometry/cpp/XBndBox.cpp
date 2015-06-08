#include "StdAfx.h"
#include "xbndbox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
XBndBox::XBndBox()
{
	Reset();
}

XBndBox::XBndBox(const XBndBox& aBndBox)
{
	m_Min = aBndBox.m_Min;
	m_Max = aBndBox.m_Max;
	m_bNoObject = aBndBox.m_bNoObject;
}

XBndBox::~XBndBox()
{
}

//////////////////////////////////////////////////////////////////////
// bound box operation
//////////////////////////////////////////////////////////////////////
void XBndBox::Reset()
{
	m_Min[0] = -1e-1;
	m_Min[1] = -1e-1;
	m_Min[2] = -1e-1;

	m_Max[0] = 1e-1;
	m_Max[1] = 1e-1;
	m_Max[2] = 1e-1;
	m_bNoObject = true;
}


const PNT& XBndBox::GetMin()const
{
	return m_Min;
}

const PNT& XBndBox::GetMax()const
{
	return m_Max;
}
 PNT XBndBox::GetCenter()const
	{
	return (m_Max - m_Min)/2;
	}

PNT& XBndBox::GetMin()
{
	return m_Min;
}

 PNT& XBndBox::GetMax()
{
	return m_Max;
}

XBndBox& XBndBox::operator=(const XBndBox& aBndBox)
{
	m_Min = aBndBox.m_Min;
	m_Max = aBndBox.m_Max;
	m_bNoObject = aBndBox.m_bNoObject;
	return *this;
}
XBndBox XBndBox::operator+(const XBndBox& aBndBox)const
{
	XBndBox box;
	PNT  v;
	if(m_bNoObject)
		{
		box = aBndBox;
		return box;
		}
	if(aBndBox.m_bNoObject)
		{
		return *this;
		}
	box.AddVertex(m_Min);
	box.AddVertex(m_Max);
	box.AddVertex(aBndBox.m_Min);
	box.AddVertex(aBndBox.m_Max);	
	return box;
}
double XBndBox::GetLength()
	{
	return (m_Max - m_Min).Length();
	}

void XBndBox::AddVertex(const PNT& aVtx)
{
	if(m_bNoObject)
		{
		 m_Max = aVtx;
		 m_Min = aVtx;
		 m_bNoObject = false;
		 return;
		}
	if(aVtx[0] < m_Min[0])
		m_Min[0] = aVtx[0];

	if(aVtx[1] < m_Min[1])
		m_Min[1] = aVtx[1];

	if(aVtx[2] < m_Min[2])
		m_Min[2] = aVtx[2];

	if(aVtx[0] > m_Max[0])
		m_Max[0] = aVtx[0];

	if(aVtx[1] > m_Max[1])
		m_Max[1] = aVtx[1];

	if(aVtx[2] > m_Max[2])
		m_Max[2] = aVtx[2];
}

bool XBndBox::IsVoid(void)
	{
	return this->m_bNoObject;
	}
