/////////////////////////////////////////////////////////////////////////////////////////
//			file name : Ti_Trackball.h   
//
//			File Description : 
//				which is used for mouse rotation operation
//			created by Xiaoming Du 10/27/2004
//
//			Modification Record:	
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "math_algorithm.h"
#include "pnt.h"
class MATH_ALGORITHM_API Ti_Trackball
	{

	protected:
	// 将鼠标(mouseX, mouseY)投影到球面一点 P
	// 函数值即为球心到P的矢量
	int    m_iWidth,   m_iHeight;	// 窗口的宽高
	float    m_iCenterX, m_iCenterY;	// 跟踪球中心
	int    m_iRadius;				// 跟踪球半径
	float    m_iMouseX,  m_iMouseY;	// 鼠标位置
	PNT m_start;	// 刚按下时的鼠标投影矢量
	PNT m_stop;	// 移动(dx,dy)时的鼠标投影矢量

	public:
		Ti_Trackball(void);
		~Ti_Trackball(void);
public:
	// 跟踪球的中心为窗口的中心
	void SetTrackWindow(const int& iWidth,const  int&  iHeight);

	// 跟踪球的中心为窗口坐标(centerX, centerY) 
	void SetTrackWindow(const int & iWidth, const int & iHeight, const int & iCenterX, const int & iCenterY);

	// 输入刚按下鼠标时的坐标 (mouseX, mouseY)
	void Start(const int & iMouseX, const int & iMouseY);

	// 鼠标移动 (dx,dy), 计算旋转轴 axis 与角度 angle 
	void Tracking(const float & iDx, const float & iDy, PNT& axis, float &fAngle);
	PNT MouseToVector(const int&  iMouseX, const int& iMouseY);
	};
