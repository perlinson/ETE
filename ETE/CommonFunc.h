#pragma once
#include "stdafx.h"
#include "ETESystemPublicDefine.h"
#include <windows.h>
#include <opencv.hpp>

using namespace cv;
static Mat toGrayscale(InputArray _src) {
	Mat src = _src.getMat();
	// only allow one channel
	if(src.channels() != 1) {
		CV_Error(CV_StsBadArg, "Only Matrices with one channel are supported");
	}
	// create and return normalized image
	Mat dst;
	cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
	return dst;
}

static Mat generateMat(const Mat& mat, int left)
{
	Mat GrayScale = toGrayscale(mat);
	Mat m = Mat::zeros(VIEW_HEIGHT, VIEW_WIDTH, CV_8UC1);
	for (int r = 0; r <  VIEW_HEIGHT; r++)
	{
		for (int c = 0; c < mat.cols; c++)
		{
			m.ptr(r)[c+left] = GrayScale.ptr(r)[c];
		}
	}
	return m;
}

static void BitMatToWnd(CWnd* pWnd, cv::Mat img, CRect *Roi)
{
	if(img.empty()) 
		return;

	CRect drect; 
	drect.CopyRect(Roi);

	CClientDC dc(pWnd); 
	HDC hDC =dc.GetSafeHdc();

	//内存中的图像数据拷贝到屏幕上 
	BYTE *bitBuffer = NULL; 
	BITMAPINFO *bitMapinfo = NULL;

	int ichannels =img.channels(); 
	if( ichannels == 1) 
	{ 
		bitBuffer = new BYTE[40+4*256]; 
	} 
	else if( ichannels == 3) 
	{ 
		bitBuffer = new BYTE[sizeof(BITMAPINFO)]; 
	} 
	else 
	{ 
		return; 
	}


	if(bitBuffer == NULL) 
	{ 
		return; 
	}


	bitMapinfo = (BITMAPINFO *)bitBuffer; 
	bitMapinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bitMapinfo->bmiHeader.biHeight = -img.rows; //如果高度为正的，位图的起始位置在左下角。如果高度为负，起始位置在左上角。 
	bitMapinfo->bmiHeader.biWidth = img.cols; 
	bitMapinfo->bmiHeader.biPlanes = 1; // 目标设备的级别，必须为1 
	bitMapinfo->bmiHeader.biBitCount = ichannels *8; // 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一 
	bitMapinfo->bmiHeader.biCompression = BI_RGB; //位图压缩类型，必须是 0(不压缩), 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一 
	bitMapinfo->bmiHeader.biSizeImage = 0; // 位图的大小，以字节为单位 
	bitMapinfo->bmiHeader.biXPelsPerMeter = 0; // 位图水平分辨率，每米像素数 
	bitMapinfo->bmiHeader.biYPelsPerMeter = 0; // 位图垂直分辨率，每米像素数 
	bitMapinfo->bmiHeader.biClrUsed = 0; // 位图实际使用的颜色表中的颜色数 
	bitMapinfo->bmiHeader.biClrImportant = 0; // 位图显示过程中重要的颜色数

	if(ichannels == 1) 
	{ 
		for(int i=0; i<256; i++) 
		{ //颜色的取值范围 (0-255) 
			bitMapinfo->bmiColors[i].rgbBlue =bitMapinfo->bmiColors[i].rgbGreen =bitMapinfo->bmiColors[i].rgbRed =(BYTE) i; 
		}

		bitMapinfo->bmiHeader.biClrUsed = 256; // 位图实际使用的颜色表中的颜色数 
	} 
	SetStretchBltMode(hDC, COLORONCOLOR);


	StretchDIBits(hDC, 
		drect.left, 
		0, 
		drect.right, //显示窗口宽度 
		drect.bottom, //显示窗口高度 
		0, 
		0, 
		img.cols, //图像宽度 
		img.rows, //图像高度 
		img.data, 
		bitMapinfo, 
		DIB_RGB_COLORS, 
		SRCCOPY 
		);

	delete []bitBuffer;
}
