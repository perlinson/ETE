/////////////////////////////////////////////////////////////////////////////////////////
//			file name : GLCamera.h   
//
//			File Description : 
//				this file is for camera setting:
//				
//				
//			Created by Xiaoming Du 11/4/2004
//
//			Modification Record:	
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once

class GLCamera
{

protected:

	BOOL		m_bProjectionChanged;	// 投影状态集改变,将调用函数 Projection
	float			m_fNearZ;			// 视点离近平面的距离
	float			m_fFarZ;			// 视点离远平面的距离
	float			m_fOrthoWidth;	// 平行投影平面的宽度（计算所得）
	float			m_fOrthoHeight;	// 平行投影平面的高度（计算所得）
	int			m_iViewportWidth;	// 视区的宽度（单位像素）
	int			m_iViewportHeight;// 视区的高度（单位像素）
	float			m_fAspect;		// 视区的宽高比例

public:
	GLCamera(void);
   ~GLCamera(void);

public: 

	// 执行相机变换（包含 Projection ）
	void ApplyForMouseSelect(CPoint start,CPoint end);
	void Apply(void);
	void Draw(void);
	// 响应 WM_SIZE
	void ResizeWindow(void); 

public: //----------------------- Set&Get 函数 --------------------------
	// 设置、获取视见体（包括透视投影与平行投影）
	// fOrthoZ  将用于计算平行投影的视见体
	void SetViewVolume(const float& width, const float &height,
					   const float &fNearZ, const  float &fFarZ);
	
	// 获取视见体的宽高比例、视角、近平面、远平面
	void GetViewVolume(float& width, float &height,
					   float &fNearZ, float &fFarZ);

	// 获取“平行投影”视见体的宽高
	void GetViewVolume( float &fOrthoWidth, float  &fOrthoHeight);
	const float& GetViewAspectRatio()const;

protected:
	// 实施投影变换
	void Projection(void);

};


