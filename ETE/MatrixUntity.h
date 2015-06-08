#pragma once
#include <vector>
#include <opencv.hpp>
#include "CPoint3D.h"
namespace Helper
{
	namespace ImageUntity
	{
		//void SaveBitmap(const char* filename, const std::vector<unsigned char>& vecData, int width, int height);
		void SaveBitmap(const char* filename, const std::vector<unsigned char>& vecData, int width, int height);
		void SaveColorBitmap(const char* filename, const std::vector<unsigned char>& vecData, int width, int height);
		void SaveBitmap(const char* filename,const std::vector<POINT7D>& vecData, int width, int height);
		void SaveBitmap(const char* filename,const std::vector<float>& vecData, int width, int height);
		void SaveBitmap(const char* filename,const cv::Mat data, int width, int height);
		//////////////////////////////////////////////////////////////////////////
		// strength 1/3/5/7
		//
		//////////////////////////////////////////////////////////////////////////
		void MedioFilter(std::vector<float>& vecData, int strength); 
		void MedioFilter(const std::vector<float>& input, std::vector<float>& output,int strength); 
		void Smooth(std::vector<float>& input, std::vector<float>& output,int rows, int cols,int smooth_type, int strength);
	}
}



