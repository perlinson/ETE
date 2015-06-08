// Math_Algorithm.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Math_Algorithm.h"
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;      
}

//// This is an example of an exported variable
//MATH_ALGORITHM_API int nMath_Algorithm=0;
//
//// This is an example of an exported Calculator.
//MATH_ALGORITHM_API int fnMath_Algorithm(void)
//{
//	return 42;
//}
//
//// This is the constructor of a class that has been exported.
//// see Math_Algorithm.h for the class definition
//CMath_Algorithm::CMath_Algorithm()
//{ 
//	return; 
//}
