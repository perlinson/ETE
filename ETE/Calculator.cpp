#include "stdafx.h"
#include "Calculator.h"
#include "MatrixUntity.h"
#include <highgui\highgui.hpp>
#include "ETESystemPublicDefine.h"
#include "ETEDoc.h"
#include "MainFrm.h"
#include "ETEView.h"
#include "FlatView.h"
#include "StereoView.h"
#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>
#define _USE_MATH_DEFINES
Calculator * Calculator::pInstance = nullptr;






Calculator::Calculator(void)
	:m_picCount(0),
	m_currIdx(0),
	m_stepCount(0),
	m_width(0),
	m_height(0),
	m_pDoc(nullptr)
{
	this->m_leftSide = this->m_rightSide = 0;
	this->m_ConfocalMode = FALSE;
	this->m_Confocal3dMode = FALSE;
	this->m_PSIMode = FALSE;
	this->m_VSIMode = FALSE;
	this->m_pDoc = nullptr;
	CC.resize(3);
}


Calculator::~Calculator(void)
{
}

void Calculator::SetDocument(CETEDoc* pDoc)
{
	if (pDoc)
	{
		m_pDoc = pDoc;
	}
}

void Calculator::PushData(UINT8* data)
{
	ASSERT(data);

	Mat img(VIEW_HEIGHT, VIEW_WIDTH, CV_8UC1, (UINT8*)data);
	img.convertTo(img, CV_32FC1);

	if (m_currIdx > m_picCount-1)
	{
		return;
	}

	m_matImage[m_currIdx] = img.colRange(m_leftSide, m_rightSide);

	m_currIdx++;
}

void Calculator::confocal3d(Mat current_section, int current_index,Mat& maxindex, 
	Mat& m3value, Mat& m2value, Mat& m1value, Mat& maxvalue, Mat& n1value, Mat& n2value,   Mat& n3value)
{
	if (current_index ==0)
	{
		maxvalue=current_section;
		maxindex = 0;
	}
	else
	{
		for (int r = 0; r < m_height ; r++)
		{
			for (int c = 0; c < m_width ; c++)
			{
				if (current_section.ptr<float>(r)[c] >= maxvalue.ptr<float>(r)[c])
				{
					m3value.ptr<float>(r)[c] = m2value.ptr<float>(r)[c];
					m2value.ptr<float>(r)[c] = m1value.ptr<float>(r)[c];

					m1value.ptr<float>(r)[c]=maxvalue.ptr<float>(r)[c];
					maxvalue.ptr<float>(r)[c]=current_section.ptr<float>(r)[c];
					maxindex.ptr<float>(r)[c]= static_cast<float>(current_index);
				}
				else 
				{

					if ((current_index - maxindex.ptr<float>(r)[c])==1)
					{
						n1value.ptr<float>(r)[c]=current_section.ptr<float>(r)[c];
					}

					if ((current_index - maxindex.ptr<float>(r)[c])==2)
					{
						n2value.ptr<float>(r)[c]=current_section.ptr<float>(r)[c];
					}

					if ((current_index - maxindex.ptr<float>(r)[c])==3)
					{
						n3value.ptr<float>(r)[c]=current_section.ptr<float>(r)[c];
					}
				}
			}
		}
	}
}

static Mat sinMat(const cv::Mat& src)
{
	Mat output(src.rows, src.cols, CV_32FC1);
	for (int r = 0; r < src.rows; r++)
	{
		for (int c = 0; c < src.cols ; c++)
		{
			output.at<float>(r,c) = sin(src.at<float>(r,c));
		}
	}
	return output;
}

static Mat cosMat(const cv::Mat& src)
{
	Mat output(src.rows, src.cols, CV_32FC1);
	for (int r = 0; r < src.rows; r++)
	{
		for (int c = 0; c < src.cols ; c++)
		{
			output.at<float>(r,c) = cos(src.at<float>(r,c));
		}
	}
	return output;
}

static Mat fourDCT(const cv::Mat& ij_Matrix, const cv::Mat& phasevalue)
{
	Mat sinPhasevalue = sinMat(phasevalue);
	Mat cosPhasevalue = cosMat(phasevalue);

	Mat	first, out1;
	cv::dct(sinPhasevalue, first);
	cv::idct(ij_Matrix.mul(first), first);
	cv::dct(cosPhasevalue.mul(first), out1);
	cv::idct(out1 / ij_Matrix, out1);

	Mat	second, out2;
	cv::dct(cosPhasevalue, second);
	cv::idct(ij_Matrix.mul(second), second);
	cv::dct(sinPhasevalue.mul(second), out2);
	cv::idct(out2 / ij_Matrix, out2);
	return out1 - out2;
}



static Mat atan2Mat(cv::Mat src1, cv::Mat src2)
{
	for (int r = 0; r < src1.rows; r++)
	{
		for (int c = 0; c < src1.cols ; c++)
		{
			src1.at<float>(r,c) = cv::fastAtan2(src1.at<float>(r,c), src2.at<float>(r,c)) * 3.1415 / 180.0;
		}
	}
	return src1;
}



void Calculator::Calculate()
{
	Mat maxValue = Mat::zeros(m_height, m_width, CV_32FC1);
	Mat heightValue = Mat::zeros(m_height, m_width, CV_32FC1);

	if (m_VSIMode)
	{
		//vector<Mat> vecValue(5);
		//std::generate(vecValue.begin(), vecValue.end(), 
		//	[&]()
		//{
		//	return Mat::zeros(m_height, m_width, CV_32FC1);
		//});


		//Mat A(1, m_stepCount, CV_32FC1);
		//Mat B(1, m_stepCount, CV_32FC1);
		//Mat zhongxin(m_height, m_width, CV_32FC1);
		//for (int r = 0; r < m_height ; r++)
		//{
		//	for (int c = 0; c < m_width ; c++)
		//	{
		//		for (int i = 0; i < m_stepCount ; i++)
		//		{
		//			A.at<float>(i) = i;
		//			B.at<float>(i) = m_matImage[i].at<float>(r,c);					
		//		}
		//		cv::pow(B, 4.0, B);
		//		float grav = static_cast<float>(cv::sum(A.mul(B))[0] / cv::sum(B)[0]);

		//		for (int i = 0; i < m_stepCount ; i++)
		//		{
		//			//vecValue[i].at<float>(r,c) = 
		//		}
		//	}
		//}
		//return;
	}
	if (m_PSIMode)
	{
		try
		{
			int d ;
			std::fstream scan("scan.txt");
			scan >> d;
			Phases[0] = 0;
			Phases[1] = 2* M_PI/5;
			Phases[2] = 4* M_PI/5;
			Phases[3] = 6* M_PI/5;
			Phases[4] = 8* M_PI/5;
			std::ifstream pic_list (("pic_list.txt"));;
			string pic_filename;
			while (getline(pic_list,pic_filename))   //∂¡»°Õº∆¨
			{
				Mat source = imread(pic_filename.c_str(),0);
				Mat origin_source = Mat(Change_the_sampling_interval(source,1));
				origin_source_pic.push_back(origin_source);
				source = Mat(Change_the_sampling_interval(source,d));
				source_pic.push_back(source);
			}
			pic_filename.clear();
			//origin_source_pic.push_back(Change_the_sampling_interval(m_matImage[4],1));
			//origin_source_pic.push_back(Change_the_sampling_interval(m_matImage[5],1));
			//origin_source_pic.push_back(Change_the_sampling_interval(m_matImage[6],1));
			//origin_source_pic.push_back(Change_the_sampling_interval(m_matImage[7],1));
			//origin_source_pic.push_back(Change_the_sampling_interval(m_matImage[8],1));



			//source_pic.push_back(Change_the_sampling_interval(m_matImage[4],d));
			//source_pic.push_back(Change_the_sampling_interval(m_matImage[5],d));
			//source_pic.push_back(Change_the_sampling_interval(m_matImage[6],d));
			//source_pic.push_back(Change_the_sampling_interval(m_matImage[7],d));
			//source_pic.push_back(Change_the_sampling_interval(m_matImage[8],d));


			ImageSize = source_pic[0].size();
			double K=5.0;
			double p1=K;
			double p2=0;
			double p3=0;
			double p4=0;
			double p5=0;
			double p6=0;
			for(int Iterative_Number = 0;Iterative_Number<30;Iterative_Number++)
			{
				p1=K;
				p2=0;
				p3=0;
				p4=0;
				p5=0;
				p6=0;
				for (int k = 0; k < K ; k++)
				{
					p2+=cos(Phases[k]);                               
					p3+=sin(Phases[k]);
					p4+=(cos(Phases[k])*cos(Phases[k]));
					p5+=(cos(Phases[k])*sin(Phases[k]));
					p6+=(sin(Phases[k])*sin(Phases[k]));
				}
				PP[0]=p1;
				PP[1]=p2;
				PP[2]=p3;
				PP[3]=p2;
				PP[4]=p4;
				PP[5]=p5;
				PP[6]=p3;
				PP[7]=p5;
				PP[8]=p6;
				P = Mat(3,3,CV_64FC1,PP);
//				ImageSize =source_pic[0].size();
				Parameters_C(source_pic,Phases,P,ImageSize,K);
				for (int i = 0; i < K ; i++)
				{
					Mat sidual_Images = source_pic[i]-(CC[0]+CC[1]*cos(Phases[i])+CC[2]*sin(Phases[i]));
					Mat tk=-CC[1]*sin(Phases[i])+CC[2]*cos(Phases[i]);
					Residual_Images.push_back(sidual_Images);
					Tk.push_back(tk);
				}
				int v2=0;
				int v3=0;
				int v4=0;
				int v5=0;
				for (int i = 0; i < K ; i++)
				{
					v2=v2+FunSkl(1,i,P,Phases)*m_sum(Residual_Images[i].mul(Tk[1]));
					v3=v3+FunSkl(2,i,P,Phases)*m_sum(Residual_Images[i].mul(Tk[2]));
					v4=v4+FunSkl(3,i,P,Phases)*m_sum(Residual_Images[i].mul(Tk[3]));
					v5=v5+FunSkl(4,i,P,Phases)*m_sum(Residual_Images[i].mul(Tk[4]));
				}
				double vv[4] ={v2,v3,v4,v5};
				Mat v = Mat(4,1,CV_64FC1,vv);
				double W11=FunWmn(1,1,Tk,Phases,P);
				double W12=FunWmn(1,2,Tk,Phases,P);
				double W13=FunWmn(1,3,Tk,Phases,P);
				double W14=FunWmn(1,4,Tk,Phases,P);
				double W22=FunWmn(2,2,Tk,Phases,P);
				double W23=FunWmn(2,3,Tk,Phases,P);
				double W24=FunWmn(2,4,Tk,Phases,P);
				double W33=FunWmn(3,3,Tk,Phases,P);
				double W34=FunWmn(3,4,Tk,Phases,P);
				double W44=FunWmn(4,4,Tk,Phases,P);
				double WW[16]={W11,W12,W13,W14,W12,W22,W23,W24,W13,W23,W33,W34,W14,W24,W34,W44};
				Mat W = Mat(4,4,CV_64FC1,WW);
				//		print_mat(W);
				Mat E = W.inv()*v;
				double DE[5] = {0,E.ptr<double>(0)[0],E.ptr<double>(1)[0],E.ptr<double>(2)[0],E.ptr<double>(3)[0]};
				for (int i = 0; i < 5 ; i++)
				{
					Phases[i]+=DE[i];
				}
				Mat m_de =Mat(1,5,CV_64FC1,DE);
				m_de =abs(m_de);
				Tk.clear();
				Residual_Images.clear();
				Point max_index;
				Point min_index;
				minMaxLoc(m_de,&min_value, &max_value, &min_index,&max_index);
				if (abs(max_value)<=0.001)
				{
					break;
				}
			}

			ImageSize = origin_source_pic[0].size();
			CC.clear();
			CC.resize(3);
			Parameters_C(origin_source_pic,Phases,P,ImageSize,K);
			Mat P3 = m_atan2(CC[2],CC[1]);
			heightValue = phaseunwrap(P3);
			source_pic.clear();
			origin_source_pic.clear();
			save_mat(heightValue);
		}
// 		Mat I1 = m_matImage[4];
// 		Mat I2 = m_matImage[5];
// 		Mat I3 = m_matImage[6];
// 		Mat I4 = m_matImage[7];
// 		Mat I5 = m_matImage[8];
// 
// 
// 		Mat A24 = I2 - I4;
// 		Mat A15 = I1 - I5;
// 
// 		Mat C(m_height, m_width, CV_32FC1);
// 		C = cv::abs(A24) / A24;
// 
// 		for (int r = 0; r < m_height ; r++)
// 		{
// 			for (int c = 0; c < m_width ; c++)
// 			{
// 				if (C.at<float>(r,c) != 1 && C.at<float>(r,c) !=-1)   
// 					C.at<float>(r,c) =0;
// 			}
// 		}
// 
// 		Mat part1 = cv::abs( 4*A24.mul(A24) - A15.mul(A15));
// 		Mat part2 = 2 * I3 - (I1 + I5);
// 
// 		Mat divisor;
// 		cv::sqrt(part1, divisor);
// 
// 		divisor = divisor.mul(C);
// 		Mat phasevalue = atan2Mat(divisor, part2);
// 
// 		Mat ij_Matrix(m_height, m_width, CV_32FC1);
// 
// 		for (int r = 0; r < m_height ; r++)
// 		{
// 			for (int c = 0; c < m_width ; c++)
// 			{
// 				ij_Matrix.at<float>(r,c) = r*r + c*c;
// 			}
// 		}
// 
// 		ij_Matrix.at<float>(0,0) = 1;
// 
// 		heightValue = fourDCT(ij_Matrix, phasevalue);
// 		heightValue *= (0.28 / (2* 3.1415926));
// 
// 		double minnumber(0.0), maxnumber(0.0);
// 		cv::minMaxLoc(heightValue, &minnumber, &maxnumber);
// 		heightValue -= static_cast<float>(minnumber);

		//Helper::ImageUntity::SaveBitmap("heightValue.txt", heightValue, m_width, m_height);
//		}
		catch(cv::Exception e)
		{
			const char* info  = e.what();
			MessageBoxA(NULL, info, "Opencv Exception", MB_OK);
			return;
		}
	}

	if (!m_pDoc)
	{
		return;
	}


	if (m_ConfocalMode)
	{
		
		BOOL useConfocal3d = m_pDoc->UseCofocalSection();

		if (useConfocal3d)
		{
			vector<Mat> vecValue(7);
			std::generate(vecValue.begin(), vecValue.end(), 
				[&]()
			{
				return Mat::zeros(m_height, m_width, CV_32FC1);
			});

			Mat maxIndex(m_height, m_width, CV_32FC1);

			for (int current_index = 0; current_index < m_stepCount ; current_index++)
			{
				Mat i1 = m_matImage[current_index];
				Mat i2 = m_matImage[m_stepCount+current_index];
				Mat i3 = m_matImage[m_stepCount*2+current_index];

				Mat current_section = (i1 - i2).mul(i1 - i2) + (i1 - i3).mul(i1-i3) + (i2 -i3).mul(i2-i3);
				cv::sqrt(current_section, current_section);
				confocal3d(current_section, current_index, maxIndex, 
					vecValue[0], vecValue[1], vecValue[2], vecValue[3], 
					vecValue[4], vecValue[5], vecValue[6]);
			}

			Mat sumtt(m_height, m_width, CV_32FC1);
			Mat sumttb(m_height, m_width, CV_32FC1);
			for (int i = 0; i < 7 ; i++)
			{
				sumtt += vecValue[i].mul(maxIndex+ static_cast<float>(i - 4));
				sumttb += vecValue[i];
			}
			heightValue = sumtt / sumttb;
			maxValue = vecValue[3];
		}
		else
		{
			for (int i = 0; i < 3 ; i++)
			{
				Mat first  = m_matImage[i*3];
				Mat second = m_matImage[i*3+1];
				Mat third  = m_matImage[i*3+2];
				Mat m = (first - second).mul(first-second) + (first - third).mul(first - third) + 
					(second - third).mul(second - third);
				cv::sqrt(m, m);
				heightValue += m;
				maxValue += (first + second + third) / 3;
			}
			heightValue /= 3;
			maxValue /= 3;
		}
	}

	m_pDoc->SetMaxValue(maxValue);
	m_pDoc->SetHeightValue(heightValue);
	

	
	reset();
}

void Calculator::Init(int width, int height)
{
	if (m_width != width)
	{
		m_leftSide = 0;
		m_rightSide = m_width = width;
	}

	if (m_height != height)
	{
		m_height = height;
	}
}

void Calculator::SetStepCount(int stepCount)
{
	ASSERT(stepCount != 0);
	m_stepCount = stepCount;
	if (m_ConfocalMode)
	{
		m_picCount = m_stepCount * 3;
	}
	else
	{
		m_picCount = m_stepCount;
	}

	m_matImage.resize(m_picCount);
	InitMatrixStruct();
}

void  Calculator::InitMatrixStruct()
{
	for (UINT i = 0; i < m_matImage.size() ; i++)
	{
		m_matImage[i] = cv::Mat::zeros(m_height, m_width, CV_32FC1);
	}
}


Calculator * Calculator::GetInstance()
{
	if (!pInstance)
	{
		pInstance = new Calculator;
	}

	return pInstance;
}

void Calculator::DeInit()
{
	if (pInstance)
	{
		delete pInstance;
		pInstance = nullptr;
	}
}

void Calculator::InitDisplayRegion(int left, int right)
{
	if (right - left <= 10)
	{
		return;
	}

	m_leftSide = left;
	m_rightSide = right;
	m_width = m_rightSide - m_leftSide;

	if (m_pDoc)
	{
		m_pDoc->SetSectionRange(m_leftSide, m_rightSide);
	}
	//InitMatrixStruct();
}

void Calculator::PushCustomData()
{
	for (int i = 0; i < m_stepCount ; i++)
	{
		std::stringstream ss;
		ss << "C:\\Result\\V\\Image" << i+1 << ".bmp";
		string strPath = ss.str();
		Mat img = cv::imread(ss.str(), 0);
		img.convertTo(img, CV_32FC1);
		m_matImage[i] = img;
	}
}

void Calculator::reset()
{
	m_currIdx = 0;
	m_matImage.clear();
}







// std::ofstream ou("result.txt");

// void print_mat(Mat& x)
// {
// 	std::cout<<x<<std::endl;
// }
// void save_mat(Mat& x)
// {
// 	for (int i = 0; i < x.rows ; i++)
// 	{
// 		for (int j = 0; j < x.cols -1; j++)
// 		{
// 			ou<<setprecision(15)<<x.ptr<double>(i)[j]<<",";
// 		}
// 		ou<<setprecision(15)<<x.ptr<double>(i)[x.cols-1]<<";"<<std::endl;
// 	}
// 	//	ou<<x<<std::endl;
// }

cv::Mat Calculator::Change_the_sampling_interval(Mat source, int d)
{   
	int x,y;
	x = int(source.rows/d);
	y = int(source.cols/d);
	if (x%2 ==1)
	{
		x=x+1;
	}
	if (y%2 ==1)
	{
		y=y+1;
	}
	Mat  tmp = Mat(x,y,CV_64FC1);
	for (int i = 0; i < tmp.rows ; i++)
	{
		for (int j = 0; j < tmp.cols ; j++)
		{
			int x = i*d;
			int y = j*d;
			tmp.ptr<double>(i)[j] =static_cast<double>(source.ptr(x)[y]);
		}
	}
	//	print_mat(tmp);
	return tmp;
}

void Calculator::Parameters_C(vector<Mat>& Images, double *Phases,Mat &P, cv::Size ImageSize, int K)
{

	Mat q0 = Mat::zeros(ImageSize,CV_64FC1);
	Mat q1 = Mat::zeros(ImageSize,CV_64FC1);
	Mat q2 = Mat::zeros(ImageSize,CV_64FC1);
	CC[0] =q0;
	CC[1] =q1;
	CC[2] =q2;
	for (int i = 0; i < ImageSize.height ; i++)
	{
		for (int j = 0; j < ImageSize.width ; j++)
		{
			for (int k = 0; k < K ; k++)
			{ 
				q0.ptr<double>(i)[j] +=Images[k].ptr<double>(i)[j];
				q1.ptr<double>(i)[j] +=Images[k].ptr<double>(i)[j]*cos(Phases[k]);
				q2.ptr<double>(i)[j] +=Images[k].ptr<double>(i)[j]*sin(Phases[k]);
			}
			Mat q = Mat(3,1,CV_64FC1);		
			q.ptr<double>(0)[0]=q0.ptr<double>(i)[j];
			q.ptr<double>(1)[0]=q1.ptr<double>(i)[j];
			q.ptr<double>(2)[0]=q2.ptr<double>(i)[j];
			Mat cc = P.inv()*q;

			CC[0].ptr<double>(i)[j] =cc.ptr<double>(0)[0];
			CC[1].ptr<double>(i)[j] =cc.ptr<double>(0)[1];
			CC[2].ptr<double>(i)[j] =cc.ptr<double>(0)[2];
		}
	}
}

double Calculator::FunSkl(int k,int i,Mat &P,double *Phases)
{
	Mat tmp1 = Mat(1,3,CV_64FC1);
	Mat tmp2 = Mat(3,1,CV_64FC1);
	Mat Skl;
	if (k==i)
	{
		tmp1.ptr<double>(0)[0] = -1;
		tmp1.ptr<double>(0)[1] = -cos(Phases[i]);
		tmp1.ptr<double>(0)[2] = -sin(Phases[i]);
		tmp2.ptr<double>(0)[0] = 1;
		tmp2.ptr<double>(0)[1] = cos(Phases[k]);
		tmp2.ptr<double>(0)[2] = sin(Phases[k]);
		Skl = Mat(tmp1 *P.inv()*tmp2);
		Skl = Skl+Mat::ones(Skl.size(),Skl.type());
	}
	else
	{
		tmp1.ptr<double>(0)[0] = -1;
		tmp1.ptr<double>(0)[1] = -cos(Phases[i]);
		tmp1.ptr<double>(0)[2] = -sin(Phases[i]);
		tmp2.ptr<double>(0)[0] = 1;
		tmp2.ptr<double>(0)[1] = cos(Phases[k]);
		tmp2.ptr<double>(0)[2] = sin(Phases[k]);
		Skl = Mat(tmp1 *P.inv()*tmp2);
	}
	double skl = Skl.ptr<double>(0)[0];
	return skl;
}

double Calculator::m_sum(cv::Mat a)
{
	double sums=0;
	//	print_mat(a);
	for (int i = 0; i < a.rows ; i++)
	{
		for (int j = 0; j < a.cols ; j++)
		{
			sums+=a.ptr<double>(i)[j];
		}
	}
	return sums;
}

double Calculator::FunWmn(int m,int n,vector<Mat> & tk,double *Phases,Mat &P)
{
	int k=5;
	double WmnValueP1=0;
	for (int i = 0; i < k ; i++)
	{
		WmnValueP1=WmnValueP1+FunSkl(m,i,P,Phases)*FunSkl(n,i,P,Phases);
	}
	double WmnValueP2=m_sum(tk[m].mul(tk[n]));
	double WmnValue=WmnValueP1*WmnValueP2;
	return WmnValue;
}

cv::Mat Calculator::m_atan2(Mat &x,Mat &y)
{
	Mat p3 = Mat(x.size(),CV_64FC1);
	if (x.size()!=y.size())
	{
		p3 = Mat::zeros(p3.size(),p3.type());
	}
	for (int i = 0; i < x.rows ; i++)
	{
		for (int j = 0; j < x.cols ; j++)
		{
			// 			double xx = x.ptr<double>(i)[j];
			// 			double yy = y.ptr<double>(i)[j];
			// 			double pp3 = atan2(-xx,yy);
			// 			p3.ptr<double>(i)[j] = pp3;
			p3.ptr<double>(i)[j] = atan2(-x.ptr<double>(i)[j],y.ptr<double>(i)[j]);
		}
	}
	return p3;
}

cv::Mat Calculator::phaseunwrap(Mat &IM_phase)
{
	Mat IM_unwrap = Mat::zeros(ImageSize,CV_64FC1);
	Mat ij_Matrix = Mat::ones(ImageSize,CV_64FC1);
	for (int i = 0; i < ImageSize.height ; i++)
	{
		for (int j = 0; j <  ImageSize.width; j++)
		{
			ij_Matrix.ptr<double>(i)[j] = i*i+j*j;
		}
	}
	Mat tmp1 = m_idct(m_mul(ij_Matrix,m_dct(m_sin(IM_phase))));
	tmp1 = Mat(m_mul(m_cos(IM_phase),tmp1));
	tmp1 = Mat(m_dct(tmp1)/ij_Matrix);
	tmp1 = Mat(m_idct(tmp1));

	Mat tmp2 = m_idct(m_mul(ij_Matrix,m_dct(m_cos(IM_phase))));
	tmp2 = Mat(m_mul(m_sin(IM_phase),tmp2));
	tmp2 = Mat(m_dct(tmp2)/ij_Matrix);
	tmp2 = Mat(m_idct(tmp2));
	IM_unwrap = Mat(tmp1- tmp2);
	return IM_unwrap;
}

cv::Mat Calculator::m_cos(Mat &x)
{
	Mat y = Mat (x.size(),x.type());
	for (int i = 0; i < x.rows ; i++)
	{
		for (int j = 0; j < x.cols ; j++)
		{
			y.ptr<double>(i)[j] = cos(x.ptr<double>(i)[j]);
		}
	}
	return y;
}

cv::Mat Calculator::m_sin(Mat &x)
{
	Mat y = Mat (x.size(),x.type());
	for (int i = 0; i < x.rows ; i++)
	{
		for (int j = 0; j < x.cols ; j++)
		{
			y.ptr<double>(i)[j] = sin(x.ptr<double>(i)[j]);
		}
	}
	return y;
}

cv::Mat Calculator::m_dct(Mat &x)
{
	Mat y = Mat (x.size(),x.type());
	dct(x,y);
	return y;
}

cv::Mat Calculator::m_idct(Mat &x)
{
	Mat y = Mat (x.size(),x.type());
	idct(x,y);
	return y;
}

cv::Mat Calculator::m_mul(Mat &x,Mat &y)
{
	Mat z = x.mul(y);
	return z;
}
void Calculator::save_mat(Mat& x)
{
	std::ofstream ou("result.txt");
	for (int i = 0; i < x.rows ; i++)
	{
		for (int j = 0; j < x.cols -1; j++)
		{
			ou<<std::setprecision(15)<<x.ptr<double>(i)[j]<<",";
		}
		ou<<std::setprecision(15)<<x.ptr<double>(i)[x.cols-1]<<";"<<std::endl;
	}
	//	ou<<x<<std::endl;
}