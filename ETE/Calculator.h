#pragma once

#include "stdafx.h"
#include <vector>
#include <opencv.hpp>
using namespace cv;
class CETEDoc;
class Calculator
{
public:  
	static Calculator *GetInstance();  
	static Calculator *pInstance;  
	static CCriticalSection cs;

public:
	void SetStepCount(int stepCount);
	void Calculate();
	void PushData(UINT8* data);
	void Init(int width, int height);
private:
	CETEDoc* m_pDoc;
	//std::vector<POINT7D> m_vecPointList;
	int m_stepCount;
	int m_picCount;
	int m_currIdx;
	int m_leftSide;
	int m_rightSide;
	int m_width;
	int m_height;
	BOOL m_ConfocalMode;
	BOOL m_Confocal3dMode;
	BOOL m_VSIMode;
	BOOL m_PSIMode;
	std::vector<cv::Mat> m_matImage;
	
	void DeInit();
private:
	Calculator(void);
	~Calculator(void);
	
public:
	void SetDocument(CETEDoc* pDoc);
	BOOL ConfocalMode() const { return m_ConfocalMode; }
	void ConfocalMode(BOOL val) { m_ConfocalMode = val; }

	BOOL Confocal3dMode() const { return m_Confocal3dMode; }
	void Confocal3dMode(BOOL val) { m_Confocal3dMode = val; }

	BOOL VSIMode() const { return m_VSIMode; }
	void VSIMode(BOOL val) { m_VSIMode = val; }

	BOOL PSIMode() const { return m_PSIMode; }
	void PSIMode(BOOL val) { m_PSIMode = val; }

	void InitDisplayRegion(int left, int right);
	void confocal3d(cv::Mat current_section, int current_index,cv::Mat& maxindex, 
		cv::Mat& m3value, cv::Mat& m2value, cv::Mat& m1value, cv::Mat& maxvalue, cv::Mat& n1value, cv::Mat& n2value, cv::Mat& n3value);


	//For Test PSI Model Need Delete After used
	void PushCustomData();
	void InitMatrixStruct();
	void reset();

	Mat Change_the_sampling_interval(Mat source, int d);
	Mat calculate_P();
	void Parameters_C(vector<Mat>& Images, double *Phases,Mat &P, cv::Size ImageSize, int K);
	double m_sum(cv::Mat a);
	double FunSkl(int k,int i,Mat &P,double *Phases);
	double FunWmn(int m,int n,vector<Mat> & tk,double *Phases,Mat &P);
	Mat m_atan2(Mat &x,Mat &y);
	Mat m_cos(Mat &x);
	Mat m_sin(Mat &x);
	Mat m_dct(Mat &x);
	Mat m_idct(Mat &x);
	Mat m_mul(Mat &x,Mat &y);
	Mat phaseunwrap(Mat &IM_phase);
	void save_mat(Mat& x);

	vector<Mat> source_pic;
	vector<Mat> origin_source_pic;
	//double Phases[5] ={0.0};
	double Phases[5];
	double PP[9];
	Mat P;
	vector<Mat>  CC;
	vector<Mat> Residual_Images;
	vector<Mat> Tk;
	cv::Size ImageSize;
	double max_value, min_value; 
};

