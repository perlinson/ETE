#pragma once
#include <string>
#include <vector>
#include "CPoint3D.h"
class COpenGLControl;

class CPoint3DLoader
{
public:
	CPoint3DLoader();
	~CPoint3DLoader(void);

	BOOL Load(std::string filename);
	void DrawPoints();
	void LoadTexture(CString texture_path);
	void ApplyTexture();
	std::vector<POINT7D>& Data();
	long GetLength() const;
	void SetData(std::vector<POINT7D>& vecData);
	void GetBox( Point3D& fBoxCenter, Point3D& fBoxSize );
	void Display();
	void SetZCoordData(const std::vector<float>& vecZCoordData);
	void SetDisplayRect(int width, int height);
	void SetFilterStrength(int strength);
private:
	BOOL OnFileOpenCATIA(FILE *f_txt);
	void Init();
	void DrawVertex(int rows, int cols);
	
private:
	POINT3D prBoxCenter;
	POINT3D prBoxSize;
	std::vector<POINT7D> m_ptrVertexList;
	std::vector<float> m_ptrZCoordList;
	std::vector<float> m_ptrZCoordListAfterFilter;
	int m_width;
	int m_height;
	CRect m_displayRect;
	POINT3D prMax, prMin;
	float m_depthThreshold;
	BOOL m_bReloading;
};

