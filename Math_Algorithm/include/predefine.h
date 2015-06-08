/*_____________________________________________________
 |
 |	                                         
 |  file£ºpredefine.h
 |
 |  function:  define some common use macros  and variation
 |
 |  Created:  Xiaoming Du 10/27/2004
 |_____________________________________________________*/


#pragma once
#ifndef   _TI_LARGE_FLOAT
#define _TI_LARGE_FLOAT
const float   TI_LARGE_FLOAT    = 1.0e+38F;
#endif
#ifndef _TI_LARGE_INTEGER
#define _TI_LARGE_INTEGER
const int     TI_LARGE_INTEGER  = 2147483647; // 2^31 - 1
#endif
#ifndef _PIE_
#define _PIE_
const double  PIE               = 3.1415926535897932384626433832795;
#endif

#if defined(DBL_MIN)
#define ON_DBL_MIN DBL_MIN
#else
#define ON_DBL_MIN 2.22507385850720200e-308
#endif

#if defined(DBL_EPSILON)
#define ON_EPSILON DBL_EPSILON
#else
#define ON_EPSILON 2.2204460492503131e-14
#endif
#define ON_SQRT_EPSILON 1.490116119385000000e-7

#define ON_SQRT2          1.414213562373095048801689
#define ON_SQRT3          1.732050807568877293527446

/*
// In cases where lazy evaluation of a double value is
// performed, b-rep tolerances being a notable example,
// this value is used to indicate the value has not been
// computed.  This value must be < -1.0e308. and > -ON_DBL_MAX
*/
#define ON_UNSET_VALUE -1.23432101234321e+308
