#include "StdAfx.h"
#include "..\include\xpolyline.h"


XPolyline::XPolyline(void)
	{
	m_ObjectType = OBJECT_POLYLINE;
	m_Color = WHITE_COLOR;
	m_bObjChanged = true;
	}
XPolyline::XPolyline(const XPolyline& polyline)
	{
	*this = polyline;
	}

XPolyline::~XPolyline(void)
	{
	}
XPolyline&  XPolyline::operator=( const XPolyline& polyline)
	{
	m_Point = polyline.m_Point;
	(*(XGeomBase*)this)= (*(XGeomBase*)&polyline);
	//this->m_bObjChanged = polyline.m_bObjChanged;
	//this->m_bShow = polyline.m_bShow;
	//this->m_Name = polyline.m_Name;
	//this->
	return *this;
	}

void XPolyline::AddPoint( const PNT& aVector3)
	{
	m_Point.push_back( aVector3 );
	this->m_bObjChanged = true;
	}

void XPolyline:: DeleteVertex(const int& startindex, const int & num)
	{
	m_Point.erase(m_Point.begin()+startindex, m_Point.begin() + startindex+ num);
	this->m_bObjChanged = true;	
	}

const  PNT& XPolyline:: operator[](int index) const  // return point with index 
	{
	return m_Point[index];
	}
void XPolyline::SetUnit(const int& unit)
	{
	if(this->m_Unit == UNIT_NONE|| m_Unit == unit)
		{
		m_Unit = unit;
		return;
		}
	int i;
	int num = GetVertexNum();
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
			break;
		case UNIT_INCH:
			if(unit == UNIT_MM)
				{
				for(i =0; i < num; i++)
					{
					m_Point[i]*=25.4;
					}
				}
			break;
		default:
			;
		}
	m_bObjChanged = true;
	}


 void XPolyline::UpdateBndBox()
	{
	if(m_bObjChanged)
	{
		m_BndBox.Reset();
		int Num = GetVertexNum();
		for(int i = 0; i < Num; i++)
		{		
			m_BndBox.AddVertex( m_Point[i] );
		}
		PNT & min = m_BndBox.GetMin();
		PNT & max = m_BndBox.GetMax();
		for(int i=0;i<3;i++)
		{
			if(max[2]-min[2]<0.1) 
			{
				min[i] -= 0.05;
				max[i] += 0.05;
			}
		}
		m_bObjChanged = false;
	}
	}
	void XPolyline::Translate(const PNT& trans)
	{
		for(size_t i =0; i < this->m_Point.size(); i++)
		{
			m_Point[i] += trans;
		}
	}
	void XPolyline::Rotate(const Ti_Rotation& R)
	{
		for(size_t i =0; i < this->m_Point.size(); i++)
		{
			m_Point[i] = R.VectorTransform(m_Point[i]);
		}

	}

int XPolyline:: GetVertexNum( )const
	{
	return m_Point.size();
	}	

PNT& XPolyline:: operator[](int index)   // return point with index 
	{
	return m_Point[index];
	}

void XPolyline::InitObject()
	{
	m_Point.clear();
	}

bool XPolyline::IsPntIn(const PNT& checkpnt)const
	{
	float	left,right,top,bottom;  
	PNT  pnt, pnt1;
	float x, y;
	int i, j;
	int num = this->GetVertexNum();
	if(num < 3)
		return false;
	pnt = (*this)[0];
	left		= pnt[0];
	right	= pnt[0];
	top		= pnt[1];
	bottom	= pnt[1];

	/* obtain the enclosing box of polyline_cld    */
	for ( i=1; i < num; i++)
	{
		pnt = (*this)[i];
		if( left > pnt[0])
			left = pnt[0];
		else
		{
			if( right< pnt[0])	
				right = pnt[0];
		}
		if( bottom> pnt[1])
			bottom= pnt[1];
		else
		{
			if( top< pnt[1] )	
				top= pnt[1];
		}
	}

	/* end computing enclosing box*/
	int inter_count_num ;
	inter_count_num = 0 ;
	x = checkpnt[0];
	y = checkpnt[1];

	if( (x < right) &&  (x >left) && 
		(y<top) && (y>bottom) ) 
	{
		pnt = (*this)[0]; 
		pnt1 = (*this)[num-1];
		if( (x>pnt[0] && x<pnt1[0]) || (x>pnt1[0]  && x<pnt[0]) )
		{
			if ( ( (pnt[1]-pnt1[1])/
				(pnt[0] - pnt1[0])*
				(x- pnt1[0])+
				pnt1[1]) > y)
				inter_count_num++;
		}

		for ( j=0; j< num-1; j++)
		{
		pnt = (*this)[j]; 
		pnt1 = (*this)[j+1]; 
			if( (x>pnt[0] && x<pnt1[0]) || (x>pnt1[0]  && x<pnt[0]) )
			{
				if ( ( (pnt[1]-pnt1[1])/
					(pnt[0] - pnt1[0])*
					(x- pnt1[0])+
					pnt1[1]) > y)
					inter_count_num++;
			}
		}
		if(inter_count_num%2==1) 
			return true;
	}	
	return false;
	}