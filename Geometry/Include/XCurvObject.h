/*_____________________________________________________
 XCurvObject.h
        Calculator: b-spline curve related functions
		Created by Pinghai Yang
		Revised by Xiaoming Du at Dec 24, 2004

 _____________________________________________________*/

//////////////////////////////////////////////////////////////////////
#pragma once

#include "xcloud.h"
#include "ti_matrix.h"
//#include "xgeombase.h"
#include "xgeneralcurve.h"
typedef CArray <double, double> CDArray;
typedef CArray <PNT,PNT> CVArray;

class  AFX_EXT_CLASS XCurvObj : public XGeneralCurve  
{
public:
	void ResetKnot();
	//
	bool LeastSquaresFromCloud(XCloud  & cloud,int degree,int iCtrlCount);
	bool LeastSquaresClosedFromCloud(XCloud & cloud,int degree,int iCtrlCount);
	//construct interpolate curve from sorted cloud
	bool InterpFromCloud(XCloud  & cloud, int degree);
	//从点云构造闭合的插值曲线，要求点云必须事先已经排好序
	bool InterpClosedFromCloud(XCloud & cloud,int degree);
	//获取曲线的类型
	inline int GetCurveType() const {return m_iCurveType;}
	//获取曲线的阶数
	inline int GetOrder() const {return m_iOrder;}
	//设置曲线的类型
	inline void SetCurveType(int type){m_iCurveType = type;}
	//最小二乘拟合曲线
	bool LeastSquares(const CVArray & dataPoints,int degree = 3,int iCtrlCount = 20);
	bool LeastSquaresClosed(const CVArray & dataPoints,int degree = 3,int iCtrlCount = 20);
	//获取曲线最小和最大参数
	void GetMinMaxPara(float & min,float & max) const;
	//设置布尔量m_bSetValue的值
	void  SetBoolValue(bool SetValue);
	void  ShowCtrol(bool bshow)
		{
		m_bShowCtrl = bshow;
		}
	void SetCurveWidth(float width)
		{
		m_fWidth = width;
		}
		float & GetCurveWidth()
		{
			return m_fWidth;
}
	void ChangeUnits(bool bmm2inch= true);			
	//计算曲线上指定位置的三维坐标，该位置由参数u决定
	const PNT  PointAt(float u)const  ;
	
	//得到曲线上指定位置的切矢量，该位置由参数u决定
	const PNT  GetTanVector(float u) ;
	
	//计算曲线上指定位置的曲率，该位置由参数u决定
	double ComputeCurvature(float u) ;
	
	//计算曲线的n阶导数，n＝deriDegree
	void ComputeDerivation(float u, int deriDegree,CVArray & ders) ;
	
	//输入一系列的数据点，插值生成一条不封闭的B样条曲线，默认为3次
	bool CreateInterp(const CVArray & dataPoints,int degree=3);
	//输入一系列的数据点，插值生成一条不封闭的B样条曲线，默认为3次(已指定节点矢量)
	bool CreateInterp(const CVArray & dataPoints,float * ub,int degree=3);
	//输入一系列的数据点(齐次点)，插值生成一条不封闭的B样条曲线，默认为3次(已指定节点矢量)
	bool CreateInterpH(const Ti_Matrix<float> & dataPoints,float * ub,float * knots,int degree=3);
	
	//输入一系列的数据点，插值生成一条封闭的B样条曲线，默认为3次
	bool CreateInterpClosed(const CVArray & dataPoints,int degree=3);
	
	
	//获取节点数量
	int GetKnotsCount() const;
	
	//获取节点矢量
    float * GetKnots();
	//yk0805: 以u0为初始参数，以epsilon为 u[k+1]-u[k] 的终止条件，用LM牛顿迭代法在curve上选取离pnt最近的点，将逼近后的u* 赋给u0
	PNT GetNearestPnt(const PNT& pnt, float& u0, const float epsilon);

	//获取控制点
	Ti_Matrix<float> & GetCtrlPoints();
	const Ti_Matrix<float> & GetCtrlPoints() const;
	//被ArchiveObject对象的CreateREObject函数调用，
	//用于通过IGES参数设置曲线
	void SetFromIgs(CDArray & curve,
		int nctrl,
		int degree,
		int unit);
	
	//无参数构造函数
	XCurvObj();
	
	//带参数构造函数
	XCurvObj(
		const Ti_Matrix<float> & CtrlPoints,
		const float * pKnots,	
		int  	Knots = 0,                     
		int	    Order = 0,
		int		CurveType = 0,
		bool	bClosed = false,
		bool    SetValue = true);
	

	//拷贝构造函数
	XCurvObj(const XCurvObj & curve);
	//析构函数
	virtual ~XCurvObj();
	////设置材质
	//void SetMaterial(Material *mat);
	////获取材质
	//Material* GetMaterial(void) const;

	////设置纹理
	//void SetTexture(Texture2D *texture);
	////获取纹理
	//Texture2D* GetTexture(void) const;
	XCurvObj& operator=( const XCurvObj& SourceCld );
	
	// 获取对象的最小坐标值(minx,miny,minz)与最大坐标值(maxx, maxy, maxz)
	virtual void UpdateBndBox() ;

	//// 获取对象的包围球，(cx,cy,cz) 为中心坐标，radius 为半径
	//virtual void GetBoundSphere(float *radius, float *cx, float *cy, float *cz) ;
	//
	//// 获取对象的包围盒，(cx,cy,cz) 为中心坐标，
	//virtual void GetBoundBox(float *dx, float *dy, float *dz, 
	//						 float *cx, float *cy, float *cz) ;


	// 绘制
	//virtual void Draw(void);
	//释放内存，析构前调用
	void FinalRelease();

	//曲线反向函数
	 void CurveReversal();

	//曲线插入节点函数
	void CurveKnotIns(float u,int r);

	//曲线节点重定义
	void CurveKnotRefine(float* X, int r);

	//曲线分解函数
	void CurveDecompose();

	//曲线升阶函数
	void CurveDegreeElevate(int t);

	//曲线降阶函数
	int CurveDegreeReduce(float TOL);

	//求两个整数的二项式系数
	int Bin(int a, int b);
	//求阶乘
	long int Factor(int a);

	//求两个整数的最小值
	int Min2i(int a, int b);
	//求两个整数的最大值
	int Max2i(int a, int b);

	virtual void Translate(const PNT& delt_t);
	virtual void Rotate(const Ti_Rotation& R);

protected:
	bool    m_bShowCtrl;	
	//曲线参数是否正确设置
	bool    m_bSetValue;

	//曲线是否闭合
	bool   	m_bClosed;
	// curve width
	float        m_fWidth;
	// 节点数量 
	int		m_iKnots;
	
	// B-样条曲线阶数
	int		m_iOrder;

	/*B-样条曲线类型
	0  非均匀有理b－样条(NURBS)
	1  多项式b－样条(一般b样条)
	2  直线
	3  圆、圆弧
	*/
	int		m_iCurveType;  
	
	// minimum param value  最小参数值 
	float		m_fMinPara;
	
	 // maximum param value 最大参数值
	float		m_fMaxPara;                  
	
	// bspline knot array,节点
	float*	m_fpKnots;                  
	
	//control points,控制点
	Ti_Matrix<float> m_fCtrlPoints;					
	
    //用户自定义指针
	void*   m_vpUser;             // user pointer will be  
	
	//// 引用材质
	//Material*    m_pMaterial;
 //   
	//// 引用纹理
	//Texture2D*   m_pTexture;
	
	//OpenGL渲染器
//	GLUnurbsObj* m_pNurbsObj;
protected:
	//曲线初始化函数
	void InitCurve(void);
	//计算B样条基函数及其n阶导数
	void DerBasisfuns(int n,float u,int span,Ti_Matrix<double> & ders);
	//计算B样条基函数
	void Basisfuns(float u,int i,float * N)const;
	//确定参数值所属的节点区间，并返回区间索引
	int FindSpan(float u) const;
};

//
/////////////////
////以下两个全局函数实现于XCurvObj.cpp
//friend bool AfxSetHelix(bool direction,
//									  float turns,
//									  float radius,
//									  float pitch,
//									  float spointc[3],
//									  float spoints[3],
//									  float orient[3],
//									  int & n_knot, CDArray & knot,
//									  int & n_ctrl, CDArray & ctrl);
//
//friend bool AfxSetHelixSurf(bool direction,
//										  float turns,
//										  float radius,
//										  float pitch,
//										  float spointc_1[3],
//										  float spointc_2[3],
//										  float spoints[3],
//										  float orient[3],
//										  int & n_knot, CDArray & knot,
//										  int & n_ctrl, CDArray & ctrl);
//};
