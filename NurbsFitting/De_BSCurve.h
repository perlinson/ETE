// De_BSCurve.h: interface for the De_BSCurve class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DE_BSCURVE_H__60CE6974_D773_11D2_A87D_000021E5AAC2__INCLUDED_)
#define AFX_DE_BSCURVE_H__60CE6974_D773_11D2_A87D_000021E5AAC2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "De_BSCKont.h"
#include "De_BPnt.h"
#include "De_IntplPnt.h"
#include "Re_Pnt3D.h"
#include "Re_Vector.h"
    
class AFX_CLASS_EXPORT De_BSCurve  
{
public:
	De_BSCurve();
public:
	De_BSCKnotArray		m_KnotArray;      //节点序列
	De_BPntArray		m_CtrlPntArray;   //控制顶点序列
	De_IntplPntArray	m_DrawData;		  //保存离散后数据点(画图用)
	De_IntplPntArray	m_IntplPntArray;  //插值点
	int					m_Degree;         //次数
	int					m_Close;          //0, open; 1,close
	int					m_StartCond;      //0, 夹持端; 2, 抛物端; 1, 自由端
	int					m_EndCond;        //0, 夹持端; 2, 抛物端; 1, 自由端
	int                 m_iEndConstrain;  //0, two end constrain, 1,start constrain 2,end constrain 
	int					m_BndNum;		  //作为Nurbs片的边界点数目
	BOOL				m_IsPicked;
	BOOL                m_bReverse;       // head->end 与控制顶点序列不同=1 相同等于0
	BOOL				m_IsBlanked;
	UINT				m_ColorIndex;     // 索引 颜色值 
	double Normal_star[3],Normal_end[3];  //曲线首末端点的跨边界切矢
	De_BPntArray		m_D1CtrlPntArray; //导矢控制顶点序列
	De_BPntArray		m_D2CtrlPntArray; //导矢控制顶点序列
	De_BPntArray		m_D3CtrlPntArray; //导矢控制顶点序列
	BOOL				m_U_direction;
	int					m_SweepCurIndex;
	BOOL                m_IsPlane;        //是否平面曲线
	Re_Vector           m_PVector;        //所在平面的法矢

	//jjjj2008
	bool KnotOK;
	double star_nx,star_ny,star_nz,end_nx,end_ny,end_nz; //
	Re_Pnt3D BreakPnt; //
	Re_Pnt3DArray BreakPntArray;
	bool CurveDivide; //


public:
	bool CurveKnotCheck();
	void InsertKnot(double u,int i);

	virtual ~De_BSCurve();
	De_BSCurve(const De_BSCurve&);
	De_BSCurve &operator =(De_BSCurve);

	void	Init();
	BOOL	CalKnot();
	BOOL	CalCtrlPntNew() ;
	void	EndConstrain(int,int,int,int,double &,double &,double &,
			double &,double &,double &,double &,double &);
	BOOL	CalBase(int,int ,double ,double * );
	int 	FindInter(int ,int& );
	void	CalLineSec(int,int,int,int);
	void	CalCurSec(int,int,int,int);
	BOOL	CalCtrlPnt();                 /////////
	BOOL	CalCloseCtrlPnt(int,int);
	int 	CalPnt(double,double&,double&,double&);   ///////
	BOOL    UniformKnot(int Ctrl);
	void	CopyHeadEndPnt();

	void	KnotStandardization();						// 结点矢量标准化
    double  TolLength();                                // 曲线总长度

	BOOL	InsertKnot(int,int,int,double);				// Bs曲线结点插入
	double	CalTangent(double,Re_Pnt3D &);				// 求曲线一点的切矢
	De_BSCurve KontInsert(double u_ins, int rtims);
	BOOL	DiscreteKnot(Re_Pnt3DArray& pntarray);// 
	BOOL	ReCreateIntplPnts();



};
typedef CArray<De_BSCurve,De_BSCurve> De_BSCurveArray;

#endif // !defined(AFX_DE_BSCURVE_H__60CE6974_D773_11D2_A87D_000021E5AAC2__INCLUDED_)

