#pragma once
#include "pnt.h"
#include "geometry.h"
class AFX_EXT_CLASS  XBndBox
	{

private:
	PNT			m_Min;
	PNT			m_Max;
	bool				m_bNoObject; // new boundarybox
public:
	// construction & destruction
	XBndBox();
	XBndBox(const XBndBox& aBndBox);
	virtual ~XBndBox();

	//initialization
	void Reset();
	const PNT& GetMin()const;
	const PNT& GetMax()const;
	PNT GetCenter()const;
	PNT& GetMin();
	PNT& GetMax();
	double GetLength();
	XBndBox  operator+(const XBndBox& aBndBox)const;
	XBndBox& operator=(const XBndBox& aBndBox);
	void AddVertex(const PNT& aVtx);
	bool IsVoid(void);
	};
