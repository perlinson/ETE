// De_BSCKont.h: interface for the De_BSCKont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DE_BSCKONT_H__60CE6975_D773_11D2_A87D_000021E5AAC2__INCLUDED_)
#define AFX_DE_BSCKONT_H__60CE6975_D773_11D2_A87D_000021E5AAC2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <afxtempl.h>
#include <afxwin.h>

class AFX_CLASS_EXPORT De_BSCKnot  
{
public:
	double	m_u;
	int		m_multi;	
	BOOL	m_type;//0,inter; 1,end
	
public:
	De_BSCKnot();
	virtual ~De_BSCKnot();

public:
	De_BSCKnot(const De_BSCKnot&);
	De_BSCKnot &operator =(De_BSCKnot);
};
typedef  CArray<De_BSCKnot,De_BSCKnot> De_BSCKnotArray;

#endif // !defined(AFX_DE_BSCKONT_H__60CE6975_D773_11D2_A87D_000021E5AAC2__INCLUDED_)
