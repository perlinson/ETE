/////////////////////////////////////////////////////////////////////////////////////////
//			file name : xpolyline.h   
//
//			File Description : 
//				polyline geometry, which is mainly used for data circle selection . 
//                     It covers 
//                          - determine whether one point is within the polyline 
//							 - data reduction
//                          - add point and delete points 
 //          Created by  Xiaoming Du   Dec 5, 2005   
//			Modification Record:	
 //                      
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "xgeombase.h"
#include <vector>
using namespace std;
class AFX_EXT_CLASS XPolyline :
	public XGeomBase
	{
	protected:
		vector<PNT>			m_Point;
	public:
		XPolyline(void);
		XPolyline(const XPolyline& polyline);
		virtual ~XPolyline(void);
		XPolyline&  operator=( const XPolyline& polyline);  
		const  PNT&  operator[](int index) const;  // return point with index 
		PNT&  operator[](int index);   // return point with index 
		void AddPoint( const PNT& aVector3);
		void DeleteVertex(const int& startindex, const int & num);
		int GetVertexNum( )const;
		bool IsPntIn(const PNT& pnt)const;
		virtual void InitObject();
		virtual void SetUnit(const int& unit);
		void UpdateBndBox();
	void Translate(const PNT& trans);
	void Rotate(const Ti_Rotation& R);

	};
