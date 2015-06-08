// De_BPnt.h: interface for the De_BPnt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DE_BPNT_H__60CE6976_D773_11D2_A87D_000021E5AAC2__INCLUDED_)
#define AFX_DE_BPNT_H__60CE6976_D773_11D2_A87D_000021E5AAC2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include <afxwin.h>

class AFX_CLASS_EXPORT De_BPnt 
{
public:
	double m_x;
	double m_y;
	double m_z;
	double m_nx;
	double m_ny;
	double m_nz;
	int m_IsPicked;

public:
	De_BPnt();
	virtual ~De_BPnt();

public:
	De_BPnt(const De_BPnt&);
	De_BPnt &operator =(De_BPnt);
    De_BPnt operator +(De_BPnt);
    De_BPnt operator -(De_BPnt);
	De_BPnt operator /(double);
	De_BPnt operator *(double);
	double  Dst2Pnt(De_BPnt& );
	BOOL    operator ==(De_BPnt a);
};
typedef  CArray<De_BPnt,De_BPnt> De_BPntArray;


#endif // !defined(AFX_DE_BPNT_H__60CE6976_D773_11D2_A87D_000021E5AAC2__INCLUDED_)
