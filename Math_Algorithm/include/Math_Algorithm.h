// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MATH_ALGORITHM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MATH_ALGORITHM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MATH_ALGORITHM_EXPORTS
#define MATH_ALGORITHM_API __declspec(dllexport)
#else
#define MATH_ALGORITHM_API __declspec(dllimport)
#endif

// This class is exported from the Math_Algorithm.dll
//class MATH_ALGORITHM_API CMath_Algorithm {
//public:
//	CMath_Algorithm(void);
//	// TODO: add your methods here.
//};
//
//extern MATH_ALGORITHM_API int nMath_Algorithm;
//
//MATH_ALGORITHM_API int fnMath_Algorithm(void);
