#pragma once
#include  "afxtempl.h"

class AFX_EXT_CLASS XLayer
{

protected:
	CString				m_Name;	// Layer name
	bool				m_bShow;
	bool				m_bSelectable; //could be select or not
	int					m_Color;
	int					m_Index;

public:
	XLayer(void);
	XLayer(const XLayer& alayer);
	~XLayer(void);

	XLayer& operator=(const XLayer& alayer);

public:
	int GetColor() const;
	bool SetColor(const int& color);

	CString GetName() const;
	bool SetName(const CString & name);

	bool IsShown()const;
	bool SetVisible(const bool& bshow);

	bool IsSelectable() const;
	bool SetSelectable(bool) const;

	int GetLayerIndex() const;
	bool SetLayerIndex( int ); // index of this layer;

};

typedef CArray<XLayer, XLayer&> XLayerArray;