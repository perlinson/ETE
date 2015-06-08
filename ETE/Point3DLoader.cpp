#include "StdAfx.h"
#include "Point3DLoader.h"

static float land[]= {	
	0.999000f, 0.200000f, 0.200000f, 1.0f,
	0.999000f, 0.207843f, 0.207843f, 1.0f,
	0.999000f, 0.215686f, 0.215686f, 1.0f,
	0.999000f, 0.223529f, 0.223529f, 1.0f,
	0.999000f, 0.231373f, 0.231373f, 1.0f,
	0.999000f, 0.239216f, 0.239216f, 1.0f,
	0.999000f, 0.247059f, 0.247059f, 1.0f,
	0.999000f, 0.254902f, 0.254902f, 1.0f,
	0.999000f, 0.262745f, 0.262745f, 1.0f,
	0.999000f, 0.270588f, 0.270588f, 1.0f,
	0.999000f, 0.278431f, 0.278431f, 1.0f,
	0.999000f, 0.286275f, 0.286275f, 1.0f,
	0.999000f, 0.294118f, 0.294118f, 1.0f,
	0.999000f, 0.301961f, 0.301961f, 1.0f,
	0.999000f, 0.309804f, 0.309804f, 1.0f,
	0.999000f, 0.317647f, 0.317647f, 1.0f,
	0.999000f, 0.325490f, 0.325490f, 1.0f,
	0.999000f, 0.333333f, 0.333333f, 1.0f,
	0.999000f, 0.341176f, 0.341176f, 1.0f,
	0.999000f, 0.349020f, 0.349020f, 1.0f,
	0.999000f, 0.356863f, 0.356863f, 1.0f,
	0.999000f, 0.364706f, 0.364706f, 1.0f,
	0.999000f, 0.372549f, 0.372549f, 1.0f,
	0.999000f, 0.380392f, 0.380392f, 1.0f,
	0.999000f, 0.388235f, 0.388235f, 1.0f,
	0.999000f, 0.396078f, 0.396078f, 1.0f,
	0.999000f, 0.403922f, 0.403922f, 1.0f,
	0.999000f, 0.411765f, 0.411765f, 1.0f,
	0.999000f, 0.419608f, 0.419608f, 1.0f,
	0.999000f, 0.427451f, 0.427451f, 1.0f,
	0.999000f, 0.435294f, 0.435294f, 1.0f,
	0.999000f, 0.443137f, 0.443137f, 1.0f,
	0.999000f, 0.450980f, 0.450980f, 1.0f,
	0.999000f, 0.458824f, 0.458824f, 1.0f,
	0.999000f, 0.466667f, 0.466667f, 1.0f,
	0.999000f, 0.474510f, 0.474510f, 1.0f,
	0.999000f, 0.482353f, 0.482353f, 1.0f,
	0.999000f, 0.490196f, 0.490196f, 1.0f,
	0.999000f, 0.498039f, 0.498039f, 1.0f,
	0.999000f, 0.505882f, 0.505882f, 1.0f,
	0.999000f, 0.513725f, 0.513725f, 1.0f,
	0.999000f, 0.521569f, 0.521569f, 1.0f,
	0.999000f, 0.529412f, 0.529412f, 1.0f,
	0.999000f, 0.537255f, 0.537255f, 1.0f,
	0.999000f, 0.545098f, 0.545098f, 1.0f,
	0.999000f, 0.552941f, 0.552941f, 1.0f,
	0.999000f, 0.560784f, 0.560784f, 1.0f,
	0.999000f, 0.568627f, 0.568627f, 1.0f,
	0.999000f, 0.576471f, 0.576471f, 1.0f,
	0.999000f, 0.584314f, 0.584314f, 1.0f,
	0.999000f, 0.592157f, 0.592157f, 1.0f,
	0.999000f, 0.600000f, 0.600000f, 1.0f,
	0.999000f, 0.607843f, 0.607843f, 1.0f,
	0.999000f, 0.615686f, 0.615686f, 1.0f,
	0.999000f, 0.623529f, 0.623529f, 1.0f,
	0.999000f, 0.631373f, 0.631373f, 1.0f,
	0.999000f, 0.639216f, 0.639216f, 1.0f,
	0.999000f, 0.647059f, 0.647059f, 1.0f,
	0.999000f, 0.654902f, 0.654902f, 1.0f,
	0.999000f, 0.662745f, 0.662745f, 1.0f,
	0.999000f, 0.670588f, 0.670588f, 1.0f,
	0.999000f, 0.678431f, 0.678431f, 1.0f,
	0.999000f, 0.686275f, 0.686275f, 1.0f,
	0.999000f, 0.694118f, 0.694118f, 1.0f,
	0.999000f, 0.701961f, 0.701961f, 1.0f,
	0.999000f, 0.709804f, 0.709804f, 1.0f,
	0.999000f, 0.717647f, 0.717647f, 1.0f,
	0.999000f, 0.725490f, 0.725490f, 1.0f,
	0.999000f, 0.733333f, 0.733333f, 1.0f,
	0.999000f, 0.741176f, 0.741176f, 1.0f,
	0.999000f, 0.749020f, 0.749020f, 1.0f,
	0.999000f, 0.756863f, 0.756863f, 1.0f,
	0.999000f, 0.764706f, 0.764706f, 1.0f,
	0.999000f, 0.772549f, 0.772549f, 1.0f,
	0.999000f, 0.780392f, 0.780392f, 1.0f,
	0.999000f, 0.788235f, 0.788235f, 1.0f,
	0.999000f, 0.796078f, 0.796078f, 1.0f,
	0.999000f, 0.803922f, 0.803922f, 1.0f,
	0.999000f, 0.811765f, 0.811765f, 1.0f,
	0.999000f, 0.819608f, 0.819608f, 1.0f,
	0.999000f, 0.827451f, 0.827451f, 1.0f,
	0.999000f, 0.835294f, 0.835294f, 1.0f,
	0.999000f, 0.843137f, 0.843137f, 1.0f,
	0.999000f, 0.850980f, 0.850980f, 1.0f,
	0.999000f, 0.858824f, 0.858824f, 1.0f,
	0.999000f, 0.866667f, 0.866667f, 1.0f,
	0.999000f, 0.874510f, 0.874510f, 1.0f,
	0.999000f, 0.882353f, 0.882353f, 1.0f,
	0.999000f, 0.890196f, 0.890196f, 1.0f,
	0.999000f, 0.898039f, 0.898039f, 1.0f,
	0.999000f, 0.905882f, 0.905882f, 1.0f,
	0.999000f, 0.913725f, 0.913725f, 1.0f,
	0.999000f, 0.921569f, 0.921569f, 1.0f,
	0.999000f, 0.929412f, 0.929412f, 1.0f,
	0.999000f, 0.937255f, 0.937255f, 1.0f,
	0.999000f, 0.945098f, 0.945098f, 1.0f,
	0.999000f, 0.952941f, 0.952941f, 1.0f,
	0.984375f,0.984375f,0.984375f,1.0f,
	0.96875f,0.984375f,0.984375f,1.0f,
	0.953125f,0.984375f,0.984375f,1.0f,
	0.9375f,0.984375f,0.984375f,1.0f,
	0.90625f,0.984375f,0.984375f,1.0f,
	0.890625f,0.984375f,0.984375f,1.0f,
	0.875f,0.984375f,0.984375f,1.0f,
	0.84375f,0.984375f,0.984375f,1.0f,
	0.828125f,0.984375f,0.984375f,1.0f,
	0.8125f,0.984375f,0.984375f,1.0f,
	0.78125f,0.984375f,0.984375f,1.0f,
	0.765625f,0.984375f,0.984375f,1.0f,
	0.75f,0.984375f,0.984375f,1.0f,
	0.71875f,0.984375f,0.984375f,1.0f,
	0.703125f,0.984375f,0.984375f,1.0f,
	0.6875f,0.984375f,0.984375f,1.0f,
	0.65625f,0.984375f,0.984375f,1.0f,
	0.640625f,0.984375f,0.984375f,1.0f,
	0.625f,0.984375f,0.984375f,1.0f,
	0.609375f,0.984375f,0.984375f,1.0f,
	0.578125f,0.984375f,0.984375f,1.0f,
	0.5625f,0.984375f,0.984375f,1.0f,
	0.546875f,0.984375f,0.984375f,1.0f,
	0.515625f,0.984375f,0.984375f,1.0f,
	0.5f,0.984375f,0.984375f,1.0f,
	0.484375f,0.984375f,0.984375f,1.0f,
	0.453125f,0.984375f,0.984375f,1.0f,
	0.4375f,0.984375f,0.984375f,1.0f,
	0.421875f,0.984375f,0.984375f,1.0f,
	0.390625f,0.984375f,0.984375f,1.0f,
	0.375f,0.984375f,0.984375f,1.0f,
	0.359375f,0.984375f,0.984375f,1.0f,
	0.328125f,0.984375f,0.984375f,1.0f,
	0.3125f,0.984375f,0.984375f,1.0f,
	0.296875f,0.984375f,0.984375f,1.0f,
	0.28125f,0.984375f,0.984375f,1.0f,
	0.25f,0.984375f,0.984375f,1.0f,
	0.234375f,0.984375f,0.984375f,1.0f,
	0.21875f,0.984375f,0.984375f,1.0f,
	0.1875f,0.984375f,0.984375f,1.0f,
	0.171875f,0.984375f,0.984375f,1.0f,
	0.15625f,0.984375f,0.984375f,1.0f,
	0.125f,0.984375f,0.984375f,1.0f,
	0.109375f,0.984375f,0.984375f,1.0f,
	0.09375f,0.984375f,0.984375f,1.0f,
	0.0625f,0.984375f,0.984375f,1.0f,
	0.046875f,0.984375f,0.984375f,1.0f,
	0.03125f,0.984375f,0.984375f,1.0f,
	0.0f,0.984375f,0.984375f,1.0f,
	0.0f,0.96875f,0.984375f,1.0f,
	0.0f,0.953125f,0.984375f,1.0f,
	0.0f,0.9375f,0.984375f,1.0f,
	0.0f,0.90625f,0.984375f,1.0f,
	0.0f,0.890625f,0.984375f,1.0f,
	0.0f,0.875f,0.984375f,1.0f,
	0.0f,0.84375f,0.984375f,1.0f,
	0.0f,0.828125f,0.984375f,1.0f,
	0.0f,0.8125f,0.984375f,1.0f,
	0.0f,0.78125f,0.984375f,1.0f,
	0.0f,0.765625f,0.984375f,1.0f,
	0.0f,0.75f,0.984375f,1.0f,
	0.0f,0.71875f,0.984375f,1.0f,
	0.0f,0.703125f,0.984375f,1.0f,
	0.0f,0.6875f,0.984375f,1.0f,
	0.0f,0.65625f,0.984375f,1.0f,
	0.0f,0.640625f,0.984375f,1.0f,
	0.0f,0.625f,0.984375f,1.0f,
	0.0f,0.609375f,0.984375f,1.0f,
	0.0f,0.578125f,0.984375f,1.0f,
	0.0f,0.5625f,0.984375f,1.0f,
	0.0f,0.546875f,0.984375f,1.0f,
	0.0f,0.515625f,0.984375f,1.0f,
	0.0f,0.5f,0.984375f,1.0f,
	0.0f,0.484375f,0.984375f,1.0f,
	0.0f,0.453125f,0.984375f,1.0f,
	0.0f,0.4375f,0.984375f,1.0f,
	0.0f,0.421875f,0.984375f,1.0f,
	0.0f,0.390625f,0.984375f,1.0f,
	0.0f,0.375f,0.984375f,1.0f,
	0.0f,0.359375f,0.984375f,1.0f,
	0.0f,0.328125f,0.984375f,1.0f,
	0.0f,0.3125f,0.984375f,1.0f,
	0.0f,0.296875f,0.984375f,1.0f,
	0.0f,0.28125f,0.984375f,1.0f,
	0.0f,0.25f,0.984375f,1.0f,
	0.0f,0.234375f,0.984375f,1.0f,
	0.0f,0.21875f,0.984375f,1.0f,
	0.0f,0.1875f,0.984375f,1.0f,
	0.0f,0.171875f,0.984375f,1.0f,
	0.0f,0.15625f,0.984375f,1.0f,
	0.0f,0.125f,0.984375f,1.0f,
	0.0f,0.109375f,0.984375f,1.0f,
	0.0f,0.09375f,0.984375f,1.0f,
	0.0f,0.0625f,0.984375f,1.0f,
	0.0f,0.046875f,0.984375f,1.0f,
	0.0f,0.03125f,0.984375f,1.0f,
	0.0f,0.0f,0.984375f,1.0f,
	0.0f,0.0f,0.96875f,1.0f,
	0.0f,0.0f,0.953125f,1.0f,
	0.0f,0.0f,0.9375f,1.0f,
	0.0f,0.0f,0.921875f,1.0f,
	0.0f,0.0f,0.90625f,1.0f,
	0.0f,0.0f,0.890625f,1.0f,
	0.0f,0.0f,0.875f,1.0f,
	0.0f,0.0f,0.859375f,1.0f,
	0.0f,0.0f,0.84375f,1.0f,
	0.0f,0.0f,0.828125f,1.0f,
	0.0f,0.0f,0.8125f,1.0f,
	0.0f,0.0f,0.796875f,1.0f,
	0.0f,0.0f,0.78125f,1.0f,
	0.0f,0.0f,0.765625f,1.0f,
	0.0f,0.0f,0.75f,1.0f,
	0.0f,0.0f,0.734375f,1.0f,
	0.0f,0.0f,0.71875f,1.0f,
	0.0f,0.0f,0.703125f,1.0f,
	0.0f,0.0f,0.6875f,1.0f,
	0.0f,0.0f,0.671875f,1.0f,
	0.0f,0.0f,0.65625f,1.0f,
	0.0f,0.0f,0.640625f,1.0f,
	0.0f,0.0f,0.625f,1.0f,
	0.0f,0.0f,0.609375f,1.0f,
	0.0f,0.0f,0.59375f,1.0f,
	0.0f,0.0f,0.578125f,1.0f,
	0.0f,0.0f,0.5625f,1.0f,
	0.0f,0.0f,0.546875f,1.0f,
	0.0f,0.0f,0.53125f,1.0f,
	0.0f,0.0f,0.515625f,1.0f,
	0.0f,0.0f,0.5f,1.0f,
	0.0f,0.0f,0.484375f,1.0f,
	0.0f,0.0f,0.46875f,1.0f,
	0.0f,0.0f,0.453125f,1.0f,
	0.0f,0.0f,0.4375f,1.0f,
	0.0f,0.0f,0.421875f,1.0f,
	0.0f,0.0f,0.40625f,1.0f,
	0.0f,0.0f,0.390625f,1.0f,
	0.0f,0.0f,0.375f,1.0f,
	0.0f,0.0f,0.359375f,1.0f,
	0.0f,0.0f,0.34375f,1.0f,
	0.0f,0.0f,0.328125f,1.0f,
	0.0f,0.0f,0.3125f,1.0f,
	0.0f,0.0f,0.296875f,1.0f,
	0.0f,0.0f,0.28125f,1.0f,
	0.0f,0.0f,0.265625f,1.0f,
	0.0f,0.0f,0.25f,1.0f,
	0.0f,0.0f,0.234375f,1.0f,
	0.0f,0.0f,0.21875f,1.0f,
	0.0f,0.0f,0.203125f,1.0f,
	0.0f,0.0f,0.1875f,1.0f,
	0.0f,0.0f,0.171875f,1.0f,
	0.0f,0.0f,0.15625f,1.0f,
	0.0f,0.0f,0.140625f,1.0f,
	0.0f,0.0f,0.125f,1.0f,
	0.0f,0.0f,0.109375f,1.0f,
	0.0f,0.0f,0.09375f,1.0f,
	0.0f,0.0f,0.078125f,1.0f,
	0.0f,0.0f,0.0625f,1.0f,
	0.0f,0.0f,0.046875f,1.0f,
	0.0f,0.0f,0.03125f,1.0f,
	0.0f,0.0f,0.0f,1.0f,
	0.0f,0.0f,0.0f,1.0f
};




const UINT TEXTURE_ID = 10;


CPoint3DLoader::CPoint3DLoader()
{
	m_displayRect.SetRect(0,0,0,0);
	this->m_depthThreshold = 0.001008f;
// 	this->m_width = 0;
// 	this->m_height = 0;
	m_width			= 181;
	m_height		= 392;
	this->m_bReloading = FALSE;
}


CPoint3DLoader::~CPoint3DLoader(void)
{
	
}



BOOL CPoint3DLoader::Load(std::string filename)
{

	FILE *p_txt= fopen(filename.c_str(),"r");
	if (p_txt == NULL)
	{
		fclose(p_txt);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	//检索文件开头
	//   int nSize = sizeof('\n');
	TCHAR tHead ;
	int nBegin = 0;
	tHead = fgetc(p_txt);//   tHead = fgetc(p_txt);
	switch(tHead)
	{
	case EOF://
		nBegin = 0;
		break;
	case '!'://Catia
		return OnFileOpenCATIA(p_txt);
		break;
	case '#': //Geomagic    
		nBegin = 30;
		break;
		//		case '\n':
		//			nBegin = 0;
		//			break;
	default:
		nBegin = 0;
		break;
	}
	fseek(p_txt , nBegin , SEEK_SET);
	//检索文件开头
	//////////////////////////////////////////////////////////////////////////
	//检索文件结尾 是否有字符串标志
	fseek(p_txt ,-14L , SEEK_END);
	TCHAR  tchar[14]=_T("");
	_fgetts(tchar , 13 , p_txt);
	tchar[13] = '\0';
	CString str = tchar;
	int nEndCloud = str.Find(_T("cloud"));
	int nEnd = -1;
	if (nEndCloud == -1)
		nEnd = EOF;
	else if (nEndCloud != 6)
	{
		nEndCloud = nEndCloud - 20;
		nEnd = fseek(p_txt ,nEndCloud , SEEK_END);
	}
	//检索文件结尾 是否有字符串标志
	//////////////////////////////////////////////////////////////////////////
	// 判定数据点格式是三个参数 还是六个参数
	fseek(p_txt , nBegin , SEEK_SET);
	TCHAR tchar1[128] = _T("");
	_fgetts(tchar1 , 127 , p_txt);
	str = tchar1;
	int nLine = str.Find('\n');
	int nNull = str.Find(' ') , nPos = 0;
	int sum = 2;
	while(nNull < nLine)
	{
		nPos = str.Find(' ' , nNull + 1);
		if (nPos != -1)
		{
			nNull = nPos ;
			++sum;
		}
		else
			break;
	}
	// 判定数据点格式是三个参数 还是六个参数
	//////////////////////////////////////////////////////////////////////////
	prMax.x = -10000000.0;
	prMax.y = -10000000.0;
	prMax.z = -10000000.0;
	prMin.x = 10000000.0;
	prMin.y = 10000000.0;
	prMin.z = 10000000.0;

	//////////////////////////////////////////////////////////////////////////
	fseek(p_txt , nBegin , SEEK_SET);
	AfxGetApp()->BeginWaitCursor();	//	vector <POINT3D> m_prToothPointsVertexList;//2005.7.28添加//定义一个字符向量
	m_ptrVertexList.clear();//	POINT3D f_xyz;

	POINT7D f_xyz;
	POINT3D f_xyzw;
	switch(sum) 
	{
	case 6://
		while (fscanf(p_txt,"%f %f %f",&f_xyzw.x,&f_xyzw.y,&f_xyzw.z) != nEnd)
		{
			f_xyz = f_xyzw;
			prMax.x=max(prMax.x,f_xyz.x);
			prMax.y=max(prMax.y,f_xyz.y);
			prMax.z=max(prMax.z,f_xyz.z);
			prMin.x=min(prMin.x,f_xyz.x);
			prMin.y=min(prMin.y,f_xyz.y);
			prMin.z=min(prMin.z,f_xyz.z);		   
			m_ptrVertexList.push_back(f_xyz);	   
		}
		break;
	default://
		while (fscanf(p_txt,"%f %f %f %f %f %f",&f_xyz.x,&f_xyz.y,&f_xyz.z,&f_xyz.nx,&f_xyz.ny,&f_xyz.nz) != nEnd)
		{
			prMax.x=max(prMax.x,f_xyz.x);
			prMax.y=max(prMax.y,f_xyz.y);
			prMax.z=max(prMax.z,f_xyz.z);
			prMin.x=min(prMin.x,f_xyz.x);
			prMin.y=min(prMin.y,f_xyz.y);
			prMin.z=min(prMin.z,f_xyz.z);		   
			m_ptrVertexList.push_back(f_xyz);	   
		}
		break;
	}

	fclose(p_txt);//	vector <POINT3D>::iterator pItVector1,pItVector2;//为字符数组定义游标iterator

	
	//assert(m_ptrVertexList.size() == m_width * m_height);

	////////////////////////////////////////////////////////////////////////////
	//std::vector <POINT7D>::iterator pItVector1,pItVector2;
	//pItVector1= m_ptrVertexList.end()-1;//除了最后一个元素
	//pItVector2= pItVector1-1;
	//if (Distance(*pItVector1,*pItVector2)<0.003)
	//{
	//	m_ptrVertexList.erase(pItVector1);
	//	return ;
	//}
	////////////////////////////////////////////////////////////////////////////

	prBoxCenter = (prMax+prMin) / 2;
	prBoxSize = (prMax-prMin) / 2;
	AfxGetApp()->EndWaitCursor();
	return TRUE;
}

void CPoint3DLoader::Display()
{
	if (m_bReloading)
	{
		return;
	}


	if (!m_ptrVertexList.empty() && !m_ptrZCoordList.empty())
	{
		return;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*float c1,c2,c3,c4;
	float xd,zd;
	float media;
	float tt;
	int ka,kb,kc,kd;*/


	float max = prMax.z;
	float min = prMin.z;

	for(int i=1;i<m_height- 1;i++)
	{	
		glBegin(GL_TRIANGLE_STRIP);
		//glBegin(GL_POINTS);
		for(int j=1;j<m_width-1;j++)
		{
			//c1=m_ptrVertexList[(i*m_width+j)].z;
			//c2=m_ptrVertexList[((i+1)*m_width+j)].z;
			//c3=m_ptrVertexList[((i+1)*m_width+j+1)].z;
			//c4=m_ptrVertexList[(i*m_width+j+1)].z;


			//tt=256/(max-min);
			//// 根据高程求出颜色
			//media=c1-min;
			//media=media*tt;
			//ka=255-(int)media;
			//ka=ka*4;
			//if(ka<0)				
			//	ka=0;
			//else if(ka>256*4)
			//	ka=	97;
			//// 根据高程求出颜色
			//media=c2-min;
			//media=media*tt;
			//kb=255-(int)media;
			//kb=kb*4;
			//if(kb<0) 
			//	kb=0;
			//else if( kb>256*4)
			//	kb=97;
			//// 根据高程求出颜色
			//media=c3-min;
			//media=media*tt;
			//kc=255-(int)media;
			//kc=kc*4;
			//if(kc<0)
			//	kc=0;
			//else if( kc>256*4)
			//	kc=97;
			//media=c4-min;
			//media=media*tt;
			//kd=255-(int)media;
			//kd=kd*4;
			//if(kd<0)
			//	kd=0;
			//else if( kd>256*4)
			//	kd=97;
			DrawVertex(i,j);
			DrawVertex(i+1,j);
			DrawVertex(i,j+1);
			DrawVertex(i+1,j+1);
		}
		glEnd();
	}


	// enable blending
	glEnable(GL_BLEND);
	// enable read-only depth buffer
	glDepthMask(GL_FALSE);
	// set the blend Calculator to what we use for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// set back to normal depth buffer mode (writable)
	glDepthMask(GL_TRUE);
	// disable blending
	glDisable(GL_BLEND);
	glFinish();
	
}


void CPoint3DLoader::DrawVertex(int rows, int cols)
{
 	float x;
	float y;
	float z;
 
	if (!m_ptrVertexList.empty())
	{
		x = m_ptrVertexList[(rows*m_width+cols)].x;
		y = m_ptrVertexList[(rows*m_width+cols)].y;
		z = m_ptrVertexList[(rows*m_width+cols)].z;
	}

	if (!m_ptrZCoordList.empty())
	{
		x = static_cast<float>(cols);
		y = static_cast<float>(rows);
		z = m_ptrZCoordList[(rows*m_width+cols)];
	}

	float  tt=256/(prMax.z-prMin.z);
	float media=(z-prMin.z)*tt;
	int colorIdx=(255-(int)media) * 4;
	if(colorIdx<0)
	{
		colorIdx = 0;
	}
	else if(colorIdx>256*4)
	{
		colorIdx = 97;
	}

	//if(m_ptrVertexList[(vertexIndex)].z>m_depthThreshold)
	{
		glColor3f(land[colorIdx],land[colorIdx+1],land[colorIdx+2]);
	}
// 	else
// 	{
// 		glColor3f(0.5,0.5,0.5);
// 	}

	glVertex3f( x, y, z);
}

BOOL CPoint3DLoader::OnFileOpenCATIA(FILE *f_txt)
{
	AfxGetApp()->BeginWaitCursor();	//	vector <POINT3D> m_prToothPointsVertexList;//2005.7.28添加//定义一个字符向量
	m_ptrVertexList.clear();
	fseek(f_txt , 0L , SEEK_SET);
	//////////////////////////////////////////////////////////////////////////
	//catia head
	TCHAR tchar[128] = _T("");
	CString str ;
	int i = 0;
	for (i = 0 ; i < 5 ; ++i)
	{
		memset(tchar , 128 , 0);
		_fgetts(tchar , 127 , f_txt);
		tchar[127] = '\0';	
	}
	/////////////////////////////////////////////////////////////////////////
	//获取文件数据格式
	str = tchar;
	int nFirstChar = str.Find('\'') + 1;
	int nEndChar = str.ReverseFind('\'');
	int nChar = nEndChar - nFirstChar ;
	CString strchar = str.Mid(nFirstChar , nChar);
	nChar = (nChar + 1) / 5;
	CString *strXYZ = new CString[nChar];
	CString *strFormat = new CString[nChar];
	for ( i = 0 ; i < nChar ; ++i)
	{
		strXYZ[i] = strchar.Mid(i * 5 , 1);
		strFormat[i] = strchar.Mid(i * 5 + 3 , 1);
	}
	//获取文件数据格式
	//////////////////////////////////////////////////////////////////////////
	//点个数
	for (i = 0 ; i < 2 ; ++i)
	{
		memset(tchar , 128 , 0);
		_fgetts(tchar , 127 , f_txt);
		tchar[127] = '\0';	
	} 
	str = tchar;  
	nFirstChar = str.Find('=') + 1;
	nEndChar = str.GetLength() - nFirstChar;
	strchar = str.Mid(nFirstChar , nEndChar);
	int nPointSize = _tstoi(strchar);
	m_ptrVertexList.resize(nPointSize);
	//
	memset(tchar , 128 , 0);
	_fgetts(tchar , 127 , f_txt);
	tchar[127] = '\0';

	//catia head
	//////////////////////////////////////////////////////////////////////////
	int *nXYZ = new int[nChar + 1];
	int j = 0;

	prMax.x = -10000000.0;
	prMax.y = -10000000.0;
	prMax.z = -10000000.0;
	prMin.x = 10000000.0;
	prMin.y = 10000000.0;
	prMin.z = 10000000.0;

	std::vector<POINT7D>::iterator pItPoint7D = m_ptrVertexList.begin();
	for (j = 0 ; j < nPointSize ; ++j , ++pItPoint7D)
	{
		memset(tchar , 128 , 0);
		_fgetts(tchar , 127 , f_txt);
		tchar[127] = '\0';
		str = tchar;
		for ( i = 0 ; i < nChar ; ++i)
			nXYZ[i] = str.Find(strXYZ[i]);
		nXYZ[i] = str.GetLength();
		for (i = 0 ; i < 3 ; ++i)
		{
			strchar = str.Mid(nXYZ[i] +1 , nXYZ[i + 1] - nXYZ[i] - 1);
			switch(i)
			{
			case 0:	  
				pItPoint7D->x = _tstof(strchar);				
				prMax.x=max(prMax.x,pItPoint7D->x); 
				prMin.x=min(prMin.x,pItPoint7D->x); 
				break;
			case 1:  
				pItPoint7D->y = _tstof(strchar);
				prMax.y=max(prMax.y,pItPoint7D->y);
				prMin.y=min(prMin.y,pItPoint7D->y);
				break;
			case 2:
				pItPoint7D->z = _tstof(strchar);   
				prMax.z=max(prMax.z,pItPoint7D->z); 
				prMin.z=min(prMin.z,pItPoint7D->z);
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	delete [] strXYZ;
	delete []strFormat;
	delete []nXYZ;

	strXYZ = NULL;
	strFormat = NULL;
	nXYZ = NULL;
	fclose(f_txt);
	prBoxCenter = (prMax+prMin) / 2;
	prBoxSize = (prMax-prMin) / 2;
	//SetBox(tagCVector(prBoxCenter.x , prBoxCenter.y ,prBoxCenter.z) , tagCVector(prBoxSize.x , prBoxSize.y , prBoxSize.z));
	AfxGetApp()->EndWaitCursor();
	return TRUE;
}

 void CPoint3DLoader::GetBox( Point3D& fBoxCenter, Point3D& fBoxSize )
 {
	 if (!m_ptrVertexList.empty())
	 {
		 auto minmaxX = std::minmax_element(m_ptrVertexList.begin(), m_ptrVertexList.end(),
			 [&](POINT7D pntLeft, POINT7D pntRight) ->bool
		 {
			 return pntLeft.x < pntRight.x;
		 });

		 auto minmaxY = std::minmax_element(m_ptrVertexList.begin(), m_ptrVertexList.end(),
			 [&](POINT7D pntLeft, POINT7D pntRight) ->bool
		 {
			 return pntLeft.y < pntRight.y;
		 });

		 auto minmaxZ = std::minmax_element(m_ptrVertexList.begin(), m_ptrVertexList.end(),
			 [&](POINT7D pntLeft, POINT7D pntRight) ->bool
		 {
			 return pntLeft.z < pntRight.z;
		 });

		 prMin.x = (*minmaxX.first).x;
		 prMin.y = (*minmaxX.first).y;
		 prMin.z = (*minmaxX.first).z;
		 prMax.x = (*minmaxX.second).x;
		 prMax.y = (*minmaxX.second).y;
		 prMax.z = (*minmaxX.second).z;
	 }
	 else
	 {
		 auto minmaxZ = std::minmax_element(m_ptrZCoordList.begin(), m_ptrZCoordList.end(),
			 [&](float lhs, float rhs) ->bool
		 {
			 return lhs < rhs;
		 });

		 prMin.x = 1.0;
		 prMin.y = 1.0;
		 prMax.x = m_width;
		 prMax.y = m_height;
		 prMin.z = (*minmaxZ.first);
		 prMax.z = (*minmaxZ.second);
	 }

	 fBoxCenter = (prMax+prMin) / 2;
	 fBoxSize = (prMax-prMin) / 2;
 }

void CPoint3DLoader::LoadTexture(CString texture_path)
{
// 	USES_CONVERSION;
// 	BOOL is_load_secuess = TextureManager::Inst()->LoadTexture(W2A(texture_path.GetString()),TEXTURE_ID);
// 	m_textureLoaded = is_load_secuess;
}

void CPoint3DLoader::Init()
{
// 	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV, GL_MODULATE);
// 	currentCoeff = xequalzero;
// 	currentGenMode = GL_OBJECT_LINEAR;
// 	currentPlane = GL_OBJECT_PLANE;
// 	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, currentGenMode);
// 	glTexGenfv(GL_S, currentPlane, currentCoeff);
}

std::vector<POINT7D>& CPoint3DLoader::Data() 
{
	return m_ptrVertexList;
}

long CPoint3DLoader::GetLength() const
{
	return Distance(prMax,prMin);
}

void CPoint3DLoader::SetData(std::vector<POINT7D>& vecData)
{
	ASSERT(m_ptrVertexList.size() != 0);
	m_ptrZCoordList.clear();
	m_ptrVertexList.assign(vecData.begin(), vecData.end());
}

void CPoint3DLoader::SetZCoordData(const std::vector<float>& vecZCoordData)
{
	ASSERT(vecZCoordData.size() != 0);
	m_ptrVertexList.clear();
	m_ptrZCoordList.assign(vecZCoordData.begin(), vecZCoordData.end());
}

void CPoint3DLoader::SetDisplayRect(int width, int height)
{
	m_width = width;
	m_height = height;
}

void CPoint3DLoader::SetFilterStrength(int strength)
{
// 	m_bReloading = TRUE;
// 	m_ptrZCoordListAfterFilter.clear();
// 	Helper::ImageUntity::MedioFilter(m_ptrZCoordList, m_height, m_width, strength);
// 	m_bReloading = FALSE;
}

