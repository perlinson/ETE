// Ti_Point.cpp: implementation of the Ti_Point class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "Ti_Point.h"
#include "math.h"                   // math functions
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Ti_Point::Ti_Point()
{
    mindex = 0;
	pnext = NULL;
    pprev = NULL;
    mMaxCur = 0.0;
    mMaxDirX = 0.0;
    mMaxDirY = 0.0;
    mMaxDirZ = 0.0;
    mMinCur = 0.0;
    mMinDirX = 0.0;
    mMinDirY = 0.0;
    mMinDirZ = 0.0;
	mCalcFlag = false;
    
}
/*Ti_Point::Ti_Point( Ti_Point& point)
{
	x = point.x;
	y = point.y;
	z = point.z;

	index = point.index;
	mCurInRange = point.mCurInRange;
	marchFlag = point.marchFlag;
	mCalcFlag = point.mCalcFlag;
	
	mMaxCur = point.mMaxCur;
	mMaxDirX = point.mMaxDirX;
	mMaxDirY = point.mMaxDirY;
	mMaxDirZ = point.mMaxDirZ;
	
	mMinCur = point.mMinCur;
	mMinDirX = point.mMinDirX;
	mMinDirY = point.mMinDirY;
	mMinDirZ = point.mMinDirZ;	
}
*/
Ti_Point::Ti_Point(Ti_Point* point)
{
	x = point->x;
	y = point->y;
	z = point->z;

	index = point->index;
	mCurInRange = point->mCurInRange;
	marchFlag = point->marchFlag;
	mCalcFlag = point->mCalcFlag;
	
	mMaxCur = point->mMaxCur;
	mMaxDirX = point->mMaxDirX;
	mMaxDirY = point->mMaxDirY;
	mMaxDirZ = point->mMaxDirZ;
	
	mMinCur = point->mMinCur;
	mMinDirX = point->mMinDirX;
	mMinDirY = point->mMinDirY;
	mMinDirZ = point->mMinDirZ;	
}
///////////////////////////////////////////////////////////////////////////////
// construction
// x, y, z: input
//          coordinates
///////////////////////////////////////////////////////////////////////////////

Ti_Point::Ti_Point(double xx, double yy, double zz)
{
    mindex = 0;
    pnext = NULL;
    pprev = NULL;
    mMaxCur = 0.0;
    mMaxDirX = 0.0;
    mMaxDirY = 0.0;
    mMaxDirZ = 0.0;
    mMinCur = 0.0;
    mMinDirX = 0.0;
    mMinDirY = 0.0;
    mMinDirZ = 0.0;
    SetXyz(xx, yy, zz);
}

/*
Ti_Point::~Ti_Point()
{

}
*/

///////////////////////////////////////////////////////////////////////////////
// initiate point data
///////////////////////////////////////////////////////////////////////////////

void
Ti_Point::Init()
{
    mindex = 0;
    pnext = NULL;
    pprev = NULL;
 
}
///////////////////////////////////////////////////////////////////////////////
// set coordinats
// x, y, z: input 
//          coordinates
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::SetXyz(double xx, double yy, double zz)
{
    x = xx;
    y = yy;
    z = zz;

    mx = x;
    my = y;
    mz = z;
}


///////////////////////////////////////////////////////////////////////////////
// set maximum curvature
// maxCur: input
//         maximum curvature
///////////////////////////////////////////////////////////////////////////////

void
Ti_Point::SetMaxCur(double maxCur)
{
    mMaxCur = maxCur;
}



///////////////////////////////////////////////////////////////////////////////
// set minimum curvature
// mimCur: input
//         minimum curvature
///////////////////////////////////////////////////////////////////////////////

void
Ti_Point::SetMinCur(double mimCur)
{
    mMinCur = mimCur;
}



///////////////////////////////////////////////////////////////////////////////
// set maximum curvature direction
// x, y, z: input
//          coordinates of direction
///////////////////////////////////////////////////////////////////////////////

void
Ti_Point::SetMaxDir(double x, double y, double z)
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
Ti_Point::SetMinDir(double x, double y, double z)
{
    mMinDirX = x;
    mMinDirY = y;
    mMinDirZ = z;
}


///////////////////////////////////////////////////////////////////////////////
// set next point 
// pPnt: input
//       pointer of next point
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::SetNext(Ti_Point* pPnt)
{
    if (pPnt == NULL)
        return;

    pPnt->pprev = this;
    pnext = pPnt;
}


///////////////////////////////////////////////////////////////////////////////
// set calculation flag
// clacFlag: input
//           calculation flag
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::SetCalcFlag(bool clacFlag)
{
    mCalcFlag = clacFlag;
}




///////////////////////////////////////////////////////////////////////////////
// remove previous and next pointers
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::ResetPtrs()
{
    // set pointer of previous point
    if (pprev != NULL)
        pprev->pnext = pnext;

    // set pointer of next point
    if (pnext != NULL)
        pnext->pprev = pprev;
}


///////////////////////////////////////////////////////////////////////////////
// rotate about axis x
// sinA, cosA: input
//             sine and cosine of the rotate angle
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::RotAxisX(double sinA, double cosA)
{
    double yy = y;
    double zz = z;

    y = yy * cosA - zz * sinA;
    z = yy * sinA + zz * cosA;
}



///////////////////////////////////////////////////////////////////////////////
// rotate about axis y
// sinB, cosB: input
//             sine and cosine of the rotate angle
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::RotAxisY(double sinB, double cosB)
{
    double xx = x;
    double zz = z;

    x = xx * cosB + zz * sinB;
    z = zz * cosB - xx * sinB;
}

///////////////////////////////////////////////////////////////////////////////
// rotate about axis Z
// sinC, cosC: input
//             sine and cosine of the rotate angle
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::RotAxisZ(double sinC, double cosC)
{
    double xx = x;
    double yy = y;

    x = xx * cosC - yy * sinC;
    y = xx * sinC + yy * cosC;
}


///////////////////////////////////////////////////////////////////////////////
// shift point to a new location
// deltX, deltY, deltZ: input
//                      shift distances in axes
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::Shift(double deltX, double deltY, double deltZ)
{
    x = x - deltX;
    y = y - deltY;
    z = z - deltZ;
}


///////////////////////////////////////////////////////////////////////////////
// calculate distance to plane
// a, b, c, d: input
//             coefficients of the plane: (a*x + b*y + c*z + d = 0)
// RETURN: distance
///////////////////////////////////////////////////////////////////////////////

double 
Ti_Point::DistToPln(double a, double b, double c, double d)
{
    double dis;

    dis = a * x + b * y + c * z + d;
	return dis;
}


///////////////////////////////////////////////////////////////////////////////
// calculate distance to point
// pPnt: input
//       pointer of the point
// RETURN: distance
///////////////////////////////////////////////////////////////////////////////

double 
Ti_Point::DistToPnt(Ti_Point *pPnt)
{
	double xx, yy, zz;
    double dis;

	xx = pPnt->x;
	yy = pPnt->y;
	zz = pPnt->z;
	dis = sqrt((x-xx) * (x-xx) + (y-yy) * (y-yy) + (z-zz) * (z-zz));
	return dis;
}

///////////////////////////////////////////////////////////////////////////////
// transform to local coordinate system
// lCs: input
//      pointer of the local coordinate system
// RETURN: the pointer of transformed point if success 
//         NULL if the lCs if NULL
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::TransToLocalCs(LocalCs* lCs)
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
Ti_Point::TransFromLocalCs(LocalCs* lCs)
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
Ti_Point::GetX()
{
    return mx;
}



///////////////////////////////////////////////////////////////////////////////
// get coordinate y
// RETURN: coordinate y
///////////////////////////////////////////////////////////////////////////////

double 
Ti_Point::GetY()
{
    return my;
}



///////////////////////////////////////////////////////////////////////////////
// get coordinate z
// RETURN: coordinate z
///////////////////////////////////////////////////////////////////////////////

double 
Ti_Point::GetZ()
{
    return mz;
}



///////////////////////////////////////////////////////////////////////////////
// get point index
// RETURN: point index
///////////////////////////////////////////////////////////////////////////////

long int
Ti_Point::GetIndex()
{
    return mindex;
}


///////////////////////////////////////////////////////////////////////////////
// get coordinate x
// RETURN: coordinate x
///////////////////////////////////////////////////////////////////////////////

int Ti_Point::GetNoiseMark()
{
	return mIsNoisepnts;
}




///////////////////////////////////////////////////////////////////////////////
// get maximum curvature
// RETURN: maximum curvature
///////////////////////////////////////////////////////////////////////////////

double
Ti_Point::GetMaxCur()
{
    return mMaxCur;
}



///////////////////////////////////////////////////////////////////////////////
// get minimum curvature
// RETURN: minimum curvature
///////////////////////////////////////////////////////////////////////////////

double
Ti_Point::GetMinCur()
{
    return mMinCur;
}



///////////////////////////////////////////////////////////////////////////////
// get maximum curvature direction
// rX, rY, rZ: output
//             coordinates of direction
///////////////////////////////////////////////////////////////////////////////

void
Ti_Point::GetMaxDir(double& rX, double& rY, double& rZ)
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
Ti_Point::GetMinDir(double& rX, double& rY, double& rZ)
{
    rX = mMinDirX;
    rY = mMinDirY;
    rZ = mMinDirZ;
}

///////////////////////////////////////////////////////////////////////////////
// test if two point are coincident
// pOthPnt: input
//          pointer of the other point
// RETURN: true: coincident     false: not coincident
///////////////////////////////////////////////////////////////////////////////

bool
Ti_Point::IsCoinPnt(Ti_Point* pOthPnt)
{
    if (fabs(x - pOthPnt->x) > MAX_TOL)
        return false;
    if (fabs(y - pOthPnt->y) > MAX_TOL)
        return false;
    if (fabs(z - pOthPnt->z) > MAX_TOL)
        return false;
    if (DistToPnt(pOthPnt) > MAX_TOL)
        return false;
    return true;
}

//***********************************************************************
//   caculate distance from point to plane
//   input : a, b, c, d 
//   coefficent of plane ax + by + cz + d = 0;
//   return: distance
//***********************************************************************
double 
Ti_Point::DistPnt2Pln(double a, double b, double c, double d)
{
	double distance ;
	double dis1, dis2;	
	dis1 = fabs(a*x + b*y + c*z + d);
	dis2 = sqrt(a*a + b*b + c*c );
	distance = dis1 / dis2;
	return distance;
}


///////////////////////////////////////////////////////////////////////////////
// inquire the calculation flag
// RETURN: true: flaged     false: not flaged
///////////////////////////////////////////////////////////////////////////////

bool
Ti_Point::IsCalcFlag()
{
    return mCalcFlag;
}


///////////////////////////////////////////////////////////////////////////////
// copy point data
// RETURN: pointer of new point if success 
//         NULL if failure
///////////////////////////////////////////////////////////////////////////////

Ti_Point* 
Ti_Point::CopyData()
{
    Ti_Point*       newpnt;
    
    newpnt = new Ti_Point(x, y, z);
    if (newpnt == NULL)
        return NULL;
    newpnt->SetCalcFlag(mCalcFlag);
    newpnt->SetMchFlag(marchFlag);
    newpnt->SetMaxCur(mMaxCur);
    newpnt->SetMaxDir(mMaxDirX, mMaxDirY, mMaxDirZ);
    newpnt->SetMinCur(mMinCur);
    newpnt->SetMinDir(mMinDirX, mMinDirY, mMinDirZ);
	newpnt->SetIndexNo(mOrigIndexNo);
    newpnt->mCurInRange=mCurInRange;

    return newpnt;
}


///////////////////////////////////////////////////////////////////////////////
// set marching flag
// mchFlag: input
//          marching flag
///////////////////////////////////////////////////////////////////////////////

void 
Ti_Point::SetMchFlag(bool mchFlag)
{
    marchFlag = mchFlag;
}

void Ti_Point::SetNoiseMark(int NoiseMark)
{
	mIsNoisepnts = NoiseMark;
}


void Ti_Point::SetDistToObject(double Dist)
{
	mDistToObject = Dist;
}


int Ti_Point::GetIndexNo()
{
	return mOrigIndexNo;
}


double Ti_Point::GetDistToObject()
{
	return mDistToObject;
}


double Ti_Point::GetFabsMaxCur()
{
	double AbsMaxCur;
    AbsMaxCur = fabs(mMaxCur);
	return AbsMaxCur;
}

double Ti_Point::GetFabsMinCur()
{
	double AbsMinCur;
    AbsMinCur = fabs(mMinCur);
	return AbsMinCur;
}


void Ti_Point::SetIndexNo(int IndexNo)
{
	mOrigIndexNo = IndexNo;
}

Ti_Point &Ti_Point::operator = (Ti_Point tmp)
{
	x = tmp.x;
	y = tmp.y;
	z = tmp.z;
	mx = x;
	my = y;
	mz = z;

	index = tmp.index;
	mCurInRange = tmp.mCurInRange;
	marchFlag = tmp.marchFlag;
	mCalcFlag = tmp.mCalcFlag;
	
	mMaxCur = tmp.mMaxCur;
	mMaxDirX = tmp.mMaxDirX;
	mMaxDirY = tmp.mMaxDirY;
	mMaxDirZ = tmp.mMaxDirZ;
	
	mMinCur = tmp.mMinCur;
	mMinDirX = tmp.mMinDirX;
	mMinDirY = tmp.mMinDirY;
	mMinDirZ = tmp.mMinDirZ;

	return (*this);
}


Ti_Point::Ti_Point(double x, double y, double z, double wx, double wy)
{
	this->x=x;
	this->y=y;
	this->z=z;
	this->wx=wx;
	this->wy=wy;
	this->wz=0.5;
	index=0;
    pnext=NULL;
    pprev=NULL;
	marchFlag=false;
}
