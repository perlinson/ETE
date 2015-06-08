#include "stdafx.h"
#include "MatrixUntity.h"
#include <fstream>
#include "freeimage.h"
#include "cv.h"
#include "highgui.h"
#include <imgproc\imgproc.hpp>
#include "ETESystemPublicDefine.h"

using namespace cv;

void Helper::ImageUntity::SaveBitmap(const char* filename,const std::vector<unsigned char>& vecData, int width, int height)
{
	FIBITMAP* bmp = FreeImage_Allocate(width, height, 8);
	BYTE* bits = FreeImage_GetBits(bmp);


	for (int h = 0; h < height ; h++)
	{
		for (int w = 0; w < width ; w++)
		{
			bits[h*width+w] = vecData[h*width+w];
		}
	}


	//memcpy(bits, vecData.data(), vecData.size());
	FreeImage_Save(FIF_PNG, bmp, filename, PNG_DEFAULT);
}

void Helper::ImageUntity::SaveColorBitmap(const char* filename,const std::vector<unsigned char>& vecData, int width, int height)
{
	FIBITMAP* bmp = FreeImage_Allocate(width, height, 32, 8, 8, 8);
	for(int y=0;y<height;y++)
	{
		//把图片内容分成三份
		//每份的颜色分别是RGB
		//Red的ALPHA是128,也就是0.5

		BYTE *bits =FreeImage_GetScanLine(bmp,y);

		for(int x=0;x< width;x++)
		{
			UINT8 u = vecData[y*width+x];
			bits[0] = land[u*4]*255;
			bits[1] = land[u*4+1]*255;
			bits[2] = land[u*4+2]*255;
			bits[3] = land[u*4+3]*255;
			bits +=4;
		}
	}


	//memcpy(bits, vecData.data(), vecData.size());
	FreeImage_Save(FIF_PNG, bmp, filename, PNG_DEFAULT);
}



void Helper::ImageUntity::SaveBitmap(const char* filename,const std::vector<float>& vecData, int width, int height)
{
	auto minmaxValue = std::minmax_element(vecData.begin(), vecData.end());


	std::fstream stream;
	stream.open("heightvalue.txt", std::ios::_Nocreate|std::ios::trunc|std::ios::out);




// 	for (int h = 0; h < height ; h++)
// 	{
// 		for (int w = 0; w < width ; w++)
// 		{
// 			stream /*<< vecData[h*width+w].x << " " << vecData[h*width+w].y << " "*/
// 				<< vecData[h*width+w] << ";";
// 		}
// 		stream << '\n';
// 	}
// 	 
// 	 	stream.close();

	float minValue = *minmaxValue.first;
	float maxValue = *minmaxValue.second;
	std::vector<float> vecNewData(width*height);
	std::transform(vecData.begin(), vecData.end(), vecNewData.begin(), 
		[&](float f)
	{
		return (f - minValue) / (maxValue - f);
	});



	FIBITMAP* bmp = FreeImage_Allocate(width, height, 32, 8, 8, 8);
	for(int y=0;y<height;y++)
	{
		//把图片内容分成三份
		//每份的颜色分别是RGB
		//Red的ALPHA是128,也就是0.5

		BYTE *bits =FreeImage_GetScanLine(bmp,y);

		for(int x=0;x< width;x++)
		{
			UINT8 u = vecNewData[y*width+x] * 64 - 1;
// 			bits[0] = hsv[u*3+2]*255;
// 			bits[1] = hsv[u*3+1]*255;
// 			bits[2] = hsv[u*3]*255;
// 			bits[3] = 255;

			bits[0] = 0;
			bits[1] = 255;
			bits[2] = 0;
			bits[3] = 255;

			bits +=4;

		}
	}


	//memcpy(bits, vecData.data(), vecData.size());
	FreeImage_Save(FIF_PNG, bmp, filename, PNG_DEFAULT);




	//SaveColorBitmap(filename, vecUnit, width, height);
// 	std::fstream stream;
// 	stream.open(filename, std::ios::_Nocreate|std::ios::trunc|std::ios::out);
// 
// 
// 	for (int h = 0; h < height ; h++)
// 	{
// 		for (int w = 0; w < width ; w++)
// 		{
// 			stream /*<< vecData[h*width+w].x << " " << vecData[h*width+w].y << " "*/
// 				<< vecData[h*width+w] << ";";
// 		}
// 		stream << '\n';
// 	}
// 
// 	stream.close();
}

void Helper::ImageUntity::SaveBitmap(const char* filename,const std::vector<POINT7D>& vecData, int width, int height)
{
	std::fstream stream;
	stream.open(filename, std::ios::_Nocreate|std::ios::trunc|std::ios::out);

	
	for (int h = 0; h < height ; h++)
	{
		for (int w = 0; w < width ; w++)
		{
			stream /*<< vecData[h*width+w].x << " " << vecData[h*width+w].y << " "*/
				<< vecData[h*width+w].z << ";";
		}
		stream << '\n';
	}

	stream.close();
}

void Helper::ImageUntity::SaveBitmap(const char* filename,const cv::Mat data, int width, int height)
{
	std::fstream stream;
	stream.open(filename, std::ios::_Nocreate|std::ios::trunc|std::ios::out);


	for (int h = 0; h < height ; h++)
	{
		for (int w = 0; w < width ; w++)
		{
			stream << std::setprecision(6) << data.at<float>(h, w) << ";";
		}
		stream << '\n';
	}

	stream.close();
}

void Helper::ImageUntity::MedioFilter(std::vector<float>& vecData, int strength)
{
	cv::medianBlur(vecData, vecData, strength);
}


void Helper::ImageUntity::Smooth(std::vector<float>& input, std::vector<float>& output,int rows, int cols, int smooth_type, int strength)
{
	/*ASSERT(input.size() == rows * cols);
	
	cv::Mat matInput(rows, cols, CV_32FC1, (float*)input.data());
	cv::Mat matOutput(rows, cols, CV_32FC1);


	bool copyData = false;
	int coiMode = 0;
	int 


	if( !arr )
		return Mat();
	if( CV_IS_MAT(arr) )
		return Mat((const CvMat*)arr, copyData );
	if( CV_IS_MATND(arr) )
		return Mat((const CvMatND*)arr, copyData );
	if( CV_IS_IMAGE(arr) )
	{
		const IplImage* iplimg = (const IplImage*)arr;
		if( coiMode == 0 && iplimg->roi && iplimg->roi->coi > 0 )
			CV_Error(CV_BadCOI, "COI is not supported by the Calculator");
		return Mat(iplimg, copyData);
	}
	if( CV_IS_SEQ(arr) )
	{
		CvSeq* seq = (CvSeq*)arr;
		CV_Assert(seq->total > 0 && CV_ELEM_SIZE(seq->flags) == seq->elem_size);
		if(!copyData && seq->first->next == seq->first)
			return Mat(seq->total, 1, CV_MAT_TYPE(seq->flags), seq->first->data);
		Mat buf(seq->total, 1, CV_MAT_TYPE(seq->flags));
		cvCvtSeqToArray(seq, buf.data, CV_WHOLE_SEQ);
		return buf;
	}
	CV_Error(CV_StsBadArg, "Unknown array type");
	return Mat();
*/
	//Mat  c = Mat::zeros()





	//cvSmooth(&matInput, &matOutput, smooth_type, strength, strength ,strength ,strength);
}


void Helper::ImageUntity::MedioFilter(const std::vector<float>& input, std::vector<float>& output,int strength)
{
	//cvSmooth(input.data(), output.data(), CV_MEDIAN)
	cv::medianBlur(input, output, strength);
}
