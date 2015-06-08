// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GEOMETRY_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GEOMETRY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GEOMETRY_EXPORTS
#define GEOMETRY_API __declspec(dllexport)
#else
#define GEOMETRY_API __declspec(dllimport)
#endif
// This class is exported from the Geometry.dll
//#pragma once
//class GEOMETRY_API CGeometry {
//public:
//	CGeometry(void);
//	// TODO: add your methods here.
//};
//
//extern GEOMETRY_API int nGeometry;
//
//GEOMETRY_API int fnGeometry(void);
