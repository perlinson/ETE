/*_____________________________________________________
 |
 |	                                         
 |  file£ºGeometry_macro.h
 |
 |  Calculator:  define some common use macros  and variation, which are used by geometry.dll
 |
 |  Created:  Xiaoming Du 10/26/2004
 |_____________________________________________________*/
#ifndef _GEOMETRY_INCLUDE_H_
#define _GEOMETRY_INCLUDE_H_

#include <math.h>

#pragma once
const    int	  OBJECT_TYPE_NUM	= 10;
const    int      OBJECT_CLOUD	    = 1; 
const    int      OBJECT_MESH		= 2;
const    int      OBJECT_TRIMESH	= 3  ;  
const    int      OBJECT_CYLINDER	= 4;
const    int      OBJECT_PLANE		= 5;
const    int      OBJECT_CURVE		= 6;
const    int      OBJECT_CIRCLE		= 7;
const    int      OBJECT_ARC		= 8;
const    int      OBJECT_POLYLINE	= 9;
const    int      OBJECT_LINE      	= 10;
const    int	  OBJECT_ANNOTATION = 11;
const    int  OBJECT_POLYNOMIAL=12;
const    int  OBJECT_FREEFORM=13;

//by JM Feb 7 2006 **************************//
const    int      GEOM_NONE = 0;		//unknow
const    int      GEOM_CLD  = 10;		//1x for cloud
const    int      GEOM_LINE = 20;		//2x for line
const    int      GEOM_LINELEG = 21;
const    int      GEOM_LINETOL = 22;
const    int      GEOM_LINEFIT = 23;
const    int      GEOM_CIRCLE = 30;		//3x for circle
const    int      GEOM_CIRCLEANA = 31;
const    int      GEOM_CIRCLETOL = 32;
const    int      GEOM_CIRCLEFIT = 33;

const     int   BLUE_COLOR		= 0;   // 0  0  255
const     int   GREEN_COLOR		= 1;   // 0 255 0
const     int   RED_COLOR		= 2;   // 255 0 0
const     int   WHITE_COLOR		= 3;   //  255 255 255
const     int   GREY_COLOR		= 4;   // 192 192 192
const     int   CYAN_COLOR		= 5;   // 128 255 255
const     int   YELLOW_COLOR	= 6;   // 255 255 128
const     int   BLACK_COLOR		= 7;   //0 0 0
const     int   OLIVER_COLOR	= 8;   //RGB(128,128,0)
const	  int   DRAKPURPLE_COLOR= 9;   //(128,0,128)
const     int   DARKBLUE_COLOR  =10;   //(0,0,128)

//by JM
const	  int   LINE_SOLID		= 1;	//solid line
const	  int   LINE_DASHED		= 2;	//dash line			0X00FF
const	  int   LINE_DASHDOT	= 3;	//dash doted line	0X1C47
const	  int	LINE_DOT		= 4;	//dot line			0X0101
const	  int   LINE_LONGDASHED	= 5;	//dash line			0X00FF
const	  int   LINE_LONGDASHDOT= 6;	//dash doted line	0X1C47

#define			ROUND_EDGE	0
#define 		RADIUS		1
#define 		CHAMFER		2
#define 		BREAK_EDGE	3
//end JM

#define      UNIT_MICRON   0
#define      UNIT_MM       1
#define      UNIT_CM       2
#define      UNIT_INCH     3
#define      UNIT_FEET     4
#define      UNIT_METER    5
#define      UNIT_KM       6
#define      UNIT_MILES    7
#define		 UNIT_UIN      8
#define      UNIT_NONE     100

const double PI  =   3.1415926535897932384626433832795;

const double UMTOINCH = 0.00003937007874;
const double INCHTOMM = 25.4;

#define SQ(x) ((x)*(x))
inline float NORM(float *x,float *y) {float val = sqrt(SQ(*x)+SQ(*y)); if(val > 0) {*x /=val;*y/=val;} return val;}
inline float NORM(float x,float y) {return sqrt(SQ(x)+SQ(y));}
inline float NORM(float *x) {float val = sqrt(SQ(x[0])+SQ(x[1])); if(val > 0) {x[0] /=val;x[1]/=val;} return val;}
inline float DOT(float *x,float *y) {return x[0]*y[0]+x[1]*y[1];}
inline float DOT(float x1,float y1,float x2,float y2) {return x1*x2+y1*y2;}
inline float DOT(float x1,float y1,float *v) {return x1*v[0]+y1*v[1];}
inline float GEO_MEAN4(float *x) {return sqrt(sqrt(fabs(x[0]*x[1]*x[2]*x[3])));}


#endif //_GEOMETRY_INCLUDE_H_