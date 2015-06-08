/////////////////////////////////////////////////////////////////////////////////////////
//			file name : xgeombase.h   
//
//			File Description : 
//				base class of geometry class, it covers the object attribute setting and  operation 
//				other geometric objects should be derived from this class
//			created by Xiaoming Du 10/26/2004
//
//			Modification Record:	
//					Nov 23,2004  add data member m_Layer for the layer management requirement
//					Dec 13, 2004  add data member m_uDisplayList and m_bDispChanged for the use of display list
//                
//////////////////////////////////////////////////////////////////////////////////////////


#pragma once
#include <atlstr.h>
#include "xbndbox.h"
#include "ti_rotation.h"
#include "pnt.h"
#include  "afxtempl.h"

class AFX_EXT_CLASS  XGeomBase {

	///////////////////////////////////////////////////////////////////////
	//            Attribute data which is shared by all geometric objects
	////////////////////////////////////////////////////////////////////////
public: //Feb 6th
	CString				m_Name;	// Object name
	bool				m_bShow;
	bool				m_bShowName;
	int					m_Layer;     // the layer the object will be located
	bool				m_bSelected;
	bool				m_bObjChanged; // which is used to update the object data information
	bool				m_bDispChanged;
	bool				m_Annotation;
	int					m_ObjectType; //
	int					m_DispType;	// object display type
	int					m_Color;
	int					m_Unit;
	XBndBox				m_BndBox;   // bound box of this object
public:
	Ti_Rotation			m_Orientation; // geometric  feature orientation 
	PNT					m_Translation;  // geometric feature translation vector
	int					m_uDisplayList; // for OpenGL display list index

	//by JM Feb 7 2006 *********************//
	int					m_Attribute;   //used for small feature geometry entities

	///////////////////////////////////////////////////////////////////////
	//            construction / destruction
	////////////////////////////////////////////////////////////////////////
public:
	XGeomBase(void);
	virtual ~XGeomBase(void); 
	XGeomBase(const XGeomBase& obj);

	///////////////////////////////////////////////////////////////////////
	//            Attribute operation 
	////////////////////////////////////////////////////////////////////////
public:
	 XGeomBase& operator=(const XGeomBase& obj);
	//Get or set  Object Name
	const CString& GetName() const;
	void SetName(const CString & name);
	//inquire object layer information
	const int& GetLayer()const;
	void SetLayer(const int& layer);
	//Get or set object color
	const int& GetColor()const;
	void SetColor(const int& color);
	// 
	const bool& IsShown()const;
	void SetVisible(const bool& bshow);

	void SetObjState(bool state);
	bool IsObjChanged()const
	{
		return m_bObjChanged;
	}
	bool& IsDispChanged();
	//
	const bool& IsSelected()const;
	void SetSelState(const bool& bselected);

	void SetDispType(const int & disptype);
	int&  GetDispType();

	void SetAnnotation(bool flag);
	bool GetAnnotation();

	void SetShowName(bool flag);
	bool GetShowName();
	//object type operation
	const int& GetType()const;
	//get boundbox 
	const XBndBox& GetBBox();
	
	//by jm Feb 7 2006 *********************//
	const int& GetAttribute()const;
	void  SetAttribute(const int &attr);

	//unit operation
	const int& GetUnit()const;
	virtual void SetUnit(const int& unit)=0;
	virtual void InitObject() = 0;
	virtual void Translate(const PNT& delt_t) =0;
	virtual void Rotate(const Ti_Rotation& R) =0;
protected:
	virtual void	UpdateBndBox()=0;
	};

typedef CArray <XGeomBase*,XGeomBase*&> pXGeomBaseArray;