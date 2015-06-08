/**************************************************************************
file name : Math_Algorithm.h   

File Description :  memory dynamic management functions  
created by Xiaoming Du 12/5/2004

Modification Record:	

***************************************************************************/
#pragma once
#include "Math_Algorithm.h"

#ifdef __cplusplus
extern "C" {
#endif

MATH_ALGORITHM_API void system_error(char error_message[]);


MATH_ALGORITHM_API void free_integer_matrix( int**& m, int lr, int ur, int lc);
//void free_integer_matrix(int** m, int lr, int ur, int lc);

MATH_ALGORITHM_API void free_integer_vector(int*& v, int l);
MATH_ALGORITHM_API void free_real_matrix( double**& m, int lr, int ur, int lc);
 //void free_real_matrix(double** m, int lr, int ur, int lc);

MATH_ALGORITHM_API void free_real_vector(double*& v, int l);

MATH_ALGORITHM_API int **allocate_integer_matrix(int lr, int ur, int lc, int uc);

MATH_ALGORITHM_API int *allocate_integer_vector(int l, int u);

MATH_ALGORITHM_API double **allocate_real_matrix(int lr, int ur, int lc, int uc);

MATH_ALGORITHM_API double *allocate_real_vector(int l, int u);

#ifdef __cplusplus
}
#endif
