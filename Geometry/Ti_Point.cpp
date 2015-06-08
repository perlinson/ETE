/*
              ¨q¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨r
              ¨U  ©çDigital Manufacturing Laboratory ©ç   ¨U
¨q¨T¨T¨T¨T¨T¨T©È       Global Research Center, GE         ©À¨T¨T¨T¨T¨T¨T¨r
¨U            ¨U        Copyright(c) (2003-2006)          ¨U            ¨U
¨U            ¨t¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨s            ¨U
¨U                                                                      ¨U
¨U      File :             Ti_Point.cpp                                 ¨U
¨U                                                                      ¨U
¨U      Programmer:        Xiaoming Du, Tian Chen and Guofei Hu         ¨U
¨U                                                                      ¨U
¨U      Last modified:     09/19/05 by Guofei Hu                        ¨U
¨U                                                                      ¨U
¨U¡¡    Description:       Methods for Ti_Point.h                       ¨U
¨U                                                                      ¨U
¨U    ¨q©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤¨r    ¨U
¨t¨T¨T©È                   http://www.ge.com                      ©À¨T¨T¨s
      ¨t©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤¨s

 */



#include "stdafx.h"
#include "Ti_point.h"               // definition of Ti_point class
#include "math.h"                   // math functions

///////////////////////////////////////////////////////////////////////////////
// construction
///////////////////////////////////////////////////////////////////////////////

Ti_point::Ti_point()
{
    mindex = 0;
    mpnext = 0;
    mpprev = 0;
    mCalcFlag = false;
    mMaxCur = 0.0;
    mMaxDirX = 0.0;
    mMaxDirY = 0.0;
    mMaxDirZ = 0.0;
    mMinCur = 0.0;
    mMinDirX = 0.0;
    mMinDirY = 0.0;
    mMinDirZ = 0.0;
}



///////////////////////////////////////////////////////////////////////////////
// construction
// x, y, z: input
//          coordinates
///////////////////////////////////////////////////////////////////////////////

Ti_point::Ti_point(double x, double y, double z)
{
    mindex = 0;
    mpnext = 0;
    mpprev = 0;
    mCalcFlag = false;
    mMaxCur = 0.0;
    mMaxDirX = 0.0;
    mMaxDirY = 0.0;
    mMaxDirZ = 0.0;
    mMinCur = 0.0;
    mMinDirX = 0.0;
    mMinDirY = 0.0;
    mMinDirZ = 0.0;
    SetXyz(x, y, z);
}



///////////////////////////////////////////////////////////////////////////////
// destruction
///////////////////////////////////////////////////////////////////////////////

Ti_point::~Ti_point()
{
}



///////////////////////////////////////////////////////////////////////////////
// initiate point data
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::Init()
{
    mindex = 0;
    mpnext = 0;
    mpprev = 0;
    mCalcFlag = false;
}



///////////////////////////////////////////////////////////////////////////////
// set coordinats
// x, y, z: input 
//          coordinates
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::SetXyz(double x, double y, double z)
{
    mx = x;
    my = y;
    mz = z;
}



///////////////////////////////////////////////////////////////////////////////
// set next point 
// pPnt: input
//       pointer of next point
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::SetNext(Ti_point* pPnt)
{
    if (pPnt == 0)
        return;

    pPnt->mindex = mindex + 1;
    pPnt->mpprev = this;
    mpnext = pPnt;
}



///////////////////////////////////////////////////////////////////////////////
// set calculation flag
// clacFlag: input
//           calculation flag
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::SetCalcFlag(bool clacFlag)
{
    mCalcFlag = clacFlag;
}



///////////////////////////////////////////////////////////////////////////////
// set maximum curvature
// maxCur: input
//         maximum curvature
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::SetMaxCur(double maxCur)
{
    mMaxCur = maxCur;
}



///////////////////////////////////////////////////////////////////////////////
// set minimum curvature
// mimCur: input
//         minimum curvature
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::SetMinCur(double mimCur)
{
    mMinCur = mimCur;
}



///////////////////////////////////////////////////////////////////////////////
// set maximum curvature direction
// x, y, z: input
//          coordinates of direction
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::SetMaxDir(double x, double y, double z)
{
    mMaxDirX = x;
    mMaxDirY = y;
    mMaxDirZ = z;
}



///////////////////////////////////////////////////////////////////////////////
// set minimum curvature direction
// x, y, z: intput
//          coordinates of direction
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::SetMinDir(double x, double y, double z)
{
    mMinDirX = x;
    mMinDirY = y;
    mMinDirZ = z;
}



///////////////////////////////////////////////////////////////////////////////
// reset previous and next pointers
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::ResetPtrs()
{
    // set pointer of previous point
    if (mpprev != 0)
        mpprev->mpnext = mpnext;

    // set pointer of next point
    if (mpnext != 0)
        mpnext->mpprev = mpprev;
}



///////////////////////////////////////////////////////////////////////////////
// copy point data
// RETURN: pointer of new point if success 
//         0 if failure
///////////////////////////////////////////////////////////////////////////////

Ti_point* 
Ti_point::CopyData()
{
    Ti_point*       newpnt;
    
    newpnt = new Ti_point(mx, my, mz);
    if (newpnt == 0)
        return 0;
    newpnt->SetCalcFlag(mCalcFlag);
    newpnt->SetMaxCur(mMaxCur);
    newpnt->SetMaxDir(mMaxDirX, mMaxDirY, mMaxDirZ);
    newpnt->SetMinCur(mMinCur);
    newpnt->SetMinDir(mMinDirX, mMinDirY, mMinDirZ);
    return newpnt;
}



///////////////////////////////////////////////////////////////////////////////
// calculate distance to point
// pPnt: input
//       pointer of the point
// RETURN: distance
///////////////////////////////////////////////////////////////////////////////

double 
Ti_point::DistToPnt(Ti_point *pPnt)
{
	double x, y, z;
    double dis;

	x = pPnt->GetX();
	y = pPnt->GetY();
	z = pPnt->GetZ();
	dis = sqrt((mx-x) * (mx-x) + (my-y) * (my-y) + (mz-z) * (mz-z));
	return dis;
}



///////////////////////////////////////////////////////////////////////////////
// calculate distance to line segment
// pFstPnt: input
//          first point of the line segment
// pSndPnt: input
//          second point of the line segment
// RETURN: distance
///////////////////////////////////////////////////////////////////////////////

double
Ti_point::DistToLine(Ti_point* pFstPnt, Ti_point* pSndPnt)
{
    double len;
    double l, m, n;
    double x, y, z;
    double dx, dy, dz;

    len = pFstPnt->DistToPnt(pSndPnt);
    l = pSndPnt->GetX() - pFstPnt->GetX();
    m = pSndPnt->GetY() - pFstPnt->GetY();
    n = pSndPnt->GetZ() - pFstPnt->GetZ();
    l = l / len;
    m = m / len;
    n = n / len;
    x = mx - pFstPnt->GetX();
    y = my - pFstPnt->GetY();
    z = mz - pFstPnt->GetZ();
    dx = y * n - z * m;
    dy = z * l - x * n;
    dz = x * m - y * l;
    len = sqrt(dx * dx + dy * dy + dz * dz);
    return len;
}



///////////////////////////////////////////////////////////////////////////////
// calculate distance to plane
// a, b, c, d: input
//             coefficients of the plane: (a*x + b*y + c*z + d = 0)
// RETURN: distance
///////////////////////////////////////////////////////////////////////////////

double 
Ti_point::DistToPln(double a, double b, double c, double d)
{
    double dis;

    dis = a * mx + b * my + c * mz + d;
	return dis;
}



///////////////////////////////////////////////////////////////////////////////
// project to line segment
// pFstPnt: input
//          first point of the line segment
// pSndPnt: input
//          second point of the line segment
// rT: output
//     proportion value of the projected point
//     project point is: pFstPnt*(1-rT) + pSndPnt*rT)
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::PrjToLine(Ti_point* pFstPnt, Ti_point* pSndPnt, double& rT)
{
    double      len;
    double      l, m, n;
    double      x, y, z;

    len = pFstPnt->DistToPnt(pSndPnt);
    l = pSndPnt->GetX() - pFstPnt->GetX();
    m = pSndPnt->GetY() - pFstPnt->GetY();
    n = pSndPnt->GetZ() - pFstPnt->GetZ();
    x = mx - pFstPnt->GetX();
    y = my - pFstPnt->GetY();
    z = mz - pFstPnt->GetZ();
    rT = (l * x + m * y + n * z) / len;

    mx = pFstPnt->GetX() + l * rT;
    my = pFstPnt->GetY() + m * rT;
    mz = pFstPnt->GetZ() + n * rT;
}



///////////////////////////////////////////////////////////////////////////////
// project to a plane
// a, b, c, d: input
//             coefficients of the plane: (a*x + b*y + c*z + d = 0)
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::PrjToPln(double a, double b, double c, double d)
{
    double      x, y, z;

    x = mx - a*a*mx - a*b*my - a*c*mz - a*d;
    y = my - b*a*mx - b*b*my - b*c*mz - b*d;
    z = mz - c*a*mx - c*b*my - c*c*mz - c*d;
    mx = x;
    my = y;
    mz = z;
}



///////////////////////////////////////////////////////////////////////////////
// shift point to a new location
// deltX, deltY, deltZ: input
//                      shift distances in coordinates
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::Shift(double deltX, double deltY, double deltZ)
{
    mx = mx - deltX;
    my = my - deltY;
    mz = mz - deltZ;
}



///////////////////////////////////////////////////////////////////////////////
// rotate about axis x
// sinA, cosA: input
//             sine and cosine of the rotate angle
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::RotAxisX(double sinA, double cosA)
{
    double y = my;
    double z = mz;

    my = y * cosA - z * sinA;
    mz = y * sinA + z * cosA;
}



///////////////////////////////////////////////////////////////////////////////
// rotate about axis y
// sinB, cosB: input
//             sine and cosine of the rotate angle
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::RotAxisY(double sinB, double cosB)
{
    double x = mx;
    double z = mz;

    mx = x * cosB + z * sinB;
    mz = z * cosB - x * sinB;
}



///////////////////////////////////////////////////////////////////////////////
// rotate about axis Z
// sinC, cosC: input
//             sine and cosine of the rotate angle
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::RotAxisZ(double sinC, double cosC)
{
    double x = mx;
    double y = my;

    mx = x * cosC - y * sinC;
    my = x * sinC + y * cosC;
}



///////////////////////////////////////////////////////////////////////////////
// transform to local coordinate system
// lCs: input
//      pointer of the local coordinate system
// RETURN: the pointer of transformed point if success 
//         0 if the lCs if 0
///////////////////////////////////////////////////////////////////////////////

void 
Ti_point::TransToLocalCs(LocalCs* lCs)
{
    double      ox, oy, oz;
    double      sina, cosa, sinb, cosb, sinc, cosc;
 
    lCs->GetOrigin(ox, oy, oz);
    Shift(ox, oy, oz);

    lCs->GetAngAlpha(sina, cosa);
    lCs->GetAngBeta(sinb, cosb);
    lCs->GetAngGama(sinc, cosc);
    RotAxisX(sina, cosa);
    RotAxisY(sinb, cosb);
    RotAxisZ(sinc, cosc);
}



///////////////////////////////////////////////////////////////////////////////
// transform from local coordinate system
// lCs: input
//      pointer of the local coordinate system
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::TransFromLocalCs(LocalCs* lCs)
{
    double      ox, oy, oz;
    double      sina, cosa, sinb, cosb, sinc, cosc;
 
    lCs->GetAngAlpha(sina, cosa);
    lCs->GetAngBeta(sinb, cosb);
    lCs->GetAngGama(sinc, cosc);
    RotAxisZ(-sinc, cosc);
    RotAxisY(-sinb, cosb);
    RotAxisX(-sina, cosa);

    lCs->GetOrigin(ox, oy, oz);
    Shift(-ox, -oy, -oz);
}



///////////////////////////////////////////////////////////////////////////////
// get coordinate x
// RETURN: coordinate x
///////////////////////////////////////////////////////////////////////////////

double
Ti_point::GetX()
{
    return mx;
}



///////////////////////////////////////////////////////////////////////////////
// get coordinate y
// RETURN: coordinate y
///////////////////////////////////////////////////////////////////////////////

double 
Ti_point::GetY()
{
    return my;
}



///////////////////////////////////////////////////////////////////////////////
// get coordinate z
// RETURN: coordinate z
///////////////////////////////////////////////////////////////////////////////

double 
Ti_point::GetZ()
{
    return mz;
}



///////////////////////////////////////////////////////////////////////////////
// get point index
// RETURN: point index
///////////////////////////////////////////////////////////////////////////////

long int
Ti_point::GetIndex()
{
    return mindex;
}



///////////////////////////////////////////////////////////////////////////////
// get next point
// RETURN: pointer of next point
// WARNING: the next point of end point is 0
///////////////////////////////////////////////////////////////////////////////

Ti_point* 
Ti_point::GetNext()
{
    return mpnext;
}



///////////////////////////////////////////////////////////////////////////////
// get previous point
// RETURN: pointer of previous point
// WARNING: the previous point of head point is 0
///////////////////////////////////////////////////////////////////////////////

Ti_point* 
Ti_point::GetPrev()
{
    return mpprev;
}



///////////////////////////////////////////////////////////////////////////////
// get maximum curvature
// RETURN: maximum curvature
///////////////////////////////////////////////////////////////////////////////

double
Ti_point::GetMaxCur()
{
    return mMaxCur;
}



///////////////////////////////////////////////////////////////////////////////
// get minimum curvature
// RETURN: minimum curvature
///////////////////////////////////////////////////////////////////////////////

double
Ti_point::GetMinCur()
{
    return mMinCur;
}



///////////////////////////////////////////////////////////////////////////////
// get maximum curvature direction
// rX, rY, rZ: output
//             coordinates of direction
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::GetMaxDir(double& rX, double& rY, double& rZ)
{
    rX = mMaxDirX;
    rY = mMaxDirY;
    rZ = mMaxDirZ;
}



///////////////////////////////////////////////////////////////////////////////
// get minimum curvature direction
// rX, rY, rZ: output
//             coordinates of direction
///////////////////////////////////////////////////////////////////////////////

void
Ti_point::GetMinDir(double& rX, double& rY, double& rZ)
{
    rX = mMinDirX;
    rY = mMinDirY;
    rZ = mMinDirZ;
}



///////////////////////////////////////////////////////////////////////////////
// inquire the calculation flag
// RETURN: true: flaged     false: not flaged
///////////////////////////////////////////////////////////////////////////////

bool
Ti_point::IsCalcFlag()
{
    return mCalcFlag;
}



///////////////////////////////////////////////////////////////////////////////
// test if two point are coincident
// pOthPnt: input
//          pointer of the other point
// RETURN: true: coincident     false: not coincident
///////////////////////////////////////////////////////////////////////////////

bool
Ti_point::IsCoinPnt(Ti_point* pOthPnt)
{
    if (fabs(mx - pOthPnt->GetX()) > MAX_TOL)
        return false;
    if (fabs(my - pOthPnt->GetY()) > MAX_TOL)
        return false;
    if (fabs(mz - pOthPnt->GetZ()) > MAX_TOL)
        return false;
    if (DistToPnt(pOthPnt) > MAX_TOL)
        return false;
    return true;
}


////X////////X//////////////////////X//////////////////////////////////////////



















