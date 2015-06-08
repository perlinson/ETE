// De_IntplPnt.h: interface for the De_IntplPnt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DE_INTPLPNT_H__60CE6977_D773_11D2_A87D_000021E5AAC2__INCLUDED_)
#define AFX_DE_INTPLPNT_H__60CE6977_D773_11D2_A87D_000021E5AAC2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include <afxwin.h>

#include "De_BPnt.h"

class AFX_CLASS_EXPORT De_IntplPnt
{
public:
	De_BPnt m_pt;
	int		m_multi;
	BOOL	m_IsPicked;
	double knot_u; //该点在曲线上对应的U值
public:
	De_IntplPnt();
	virtual ~De_IntplPnt();

public:
	De_IntplPnt(const De_IntplPnt&);
	De_IntplPnt(De_BPnt& );
	De_IntplPnt &operator =(De_IntplPnt);
	bool operator !=(De_IntplPnt);
	bool operator ==(De_IntplPnt);
};
typedef  CArray<De_IntplPnt,De_IntplPnt> De_IntplPntArray;


#endif // !defined(AFX_DE_INTPLPNT_H__60CE6977_D773_11D2_A87D_000021E5AAC2__INCLUDED_)
