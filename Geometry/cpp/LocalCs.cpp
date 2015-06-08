//////////////////////////////////////////////////////////////////////
// NAME: 
//   LocalCs
//
// LocalCs.cpp: implementation of the LocalCs class.
// FILES: LocalCs.cpp
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////
#include "StdAfx.h"

#include "stdlib.h"
#include "LocalCs.h"                // definition of LocalCs class
#include "math.h"                   // math functions
#include "Ti_Point.h"               // definition of Ti_Point class

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LocalCs::LocalCs()
{
    mOx = 0.0;
    mOy = 0.0;
    mOz = 0.0;
    mAxx = 0.0;
    mAxy = 0.0;
    mAxz = 0.0;
    mAyx = 0.0;
    mAyy = 0.0;
    mAyz = 0.0;
    mAzx = 0.0;
    mAzy = 0.0;
    mAzz = 0.0;
    mSinA = 0.0;
    mCosA = 0.0;
    mSinB = 0.0;
    mCosB = 0.0;
    mSinC = 0.0;
    mCosC = 0.0;


}

LocalCs::~LocalCs()
{

}


///////////////////////////////////////////////////////////////////////////////
// set axis z
// aZx, aZy, aZz: input
//                coordinates of vectors of axes z and x
// RETURN: true: success  false: failure
///////////////////////////////////////////////////////////////////////////////

bool
LocalCs::SetAxisZ(double aZx, double aZy, double aZz)
{
    double len1, len2;
 
    len1 = sqrt(aZx * aZx + aZy * aZy + aZz * aZz);
    if (len1<MAX_TOL)
        return false;
    
    mAzx = aZx / len1;
    mAzy = aZy / len1;
    mAzz = aZz / len1;

    if (mAzx>=mAzy && mAzx>=mAzz)
    {
        mAxx = -(mAzy+mAzz) / mAzx;
        mAxy = 1.0;
        mAxz = 1.0;
    }
    else if (mAzy>=mAzx && mAzy>=mAzz)
    {
        mAxx = 1.0;
        mAxy = -(mAzz+mAzx) / mAzy;
        mAxz = 1.0;
    }
    else
    {
        mAxx = 1.0;
        mAxy = 1.0;
        mAxz = -(mAzx+mAzy) / mAzz;
    }
    len2 = sqrt(mAxx * mAxx + mAxy * mAxy + mAxz * mAxz);
    mAxx = mAxx / len2;
    mAxy = mAxy / len2;
    mAxz = mAxz / len2;

    mAyx = mAzy * mAxz - mAzz * mAxy;
    mAyy = mAzz * mAxx - mAzx * mAxz;
    mAyz = mAzx * mAxy - mAzy * mAxx;
    
    if (!CalcRotAng())
        return false;
    
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// set origin
// oX, oY, oZ: input
//             coordinates of origin
///////////////////////////////////////////////////////////////////////////////

void
LocalCs::SetOrigin(double oX, double oY, double oZ)
{
    mOx = oX;
    mOy = oY;
    mOz = oZ;
}


///////////////////////////////////////////////////////////////////////////////
// calculate rotate angle
// RETURN: true: success  false: failure
///////////////////////////////////////////////////////////////////////////////

bool
LocalCs::CalcRotAng()
{
    double      len;
    Ti_Point*   temppnt;

    len = sqrt(mAzy * mAzy + mAzz * mAzz);

	if (len < MAX_TOL)
    {
        mSinA = 0.0;
        mCosA = 1.0;
        if (mAzx > 0.0)
            mSinB = -1.0;
        else
            mSinB = 1.0;
        mCosB = 0.0;
    }
	else
	{

		mSinA = mAzy / len;
		mCosA = mAzz / len;
		mSinB = -mAzx;
		mCosB = len;
	}

    temppnt = new Ti_Point(mAyx, mAyy, mAyz);
    if (temppnt == NULL)
        return false;

    temppnt->RotAxisX(mSinA, mCosA);
    temppnt->RotAxisY(mSinB, mCosB);
    mSinC = temppnt->x;
    mCosC = temppnt->y;
    delete temppnt;

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// get Origin
// rOx, rOy, rOz: output
//                coordinates of origin
///////////////////////////////////////////////////////////////////////////////

void
LocalCs::GetOrigin(double& rOx, double& rOy, double& rOz)
{
    rOx = mOx;
    rOy = mOy;
    rOz = mOz;
}
    

///////////////////////////////////////////////////////////////////////////////
// get rotate angle alpha
// rSina, rCosa: output
//               sine and cosine of the angle alpha
///////////////////////////////////////////////////////////////////////////////

void
LocalCs::GetAngAlpha(double& rSina, double& rCosa)
{
    rSina = mSinA;
    rCosa = mCosA;
}



///////////////////////////////////////////////////////////////////////////////
// get rotate angle belta
// rSinb, rCosb: output
//               sine and cosine of the angle alpha
///////////////////////////////////////////////////////////////////////////////

void
LocalCs::GetAngBeta(double& rSinb, double& rCosb)
{
    rSinb = mSinB;
    rCosb = mCosB;
}



///////////////////////////////////////////////////////////////////////////////
// get rotate angle gama
// rSinc, rCosc: output
//               sine and cosine of the angle alpha
///////////////////////////////////////////////////////////////////////////////

void
LocalCs::GetAngGama(double& rSinc, double& rCosc)
{
    rSinc = mSinC;
    rCosc = mCosC;
}


///////////////////////////////////////////////////////////////////////////////
// get axis x
// rAxX, rAxY, rAxZ: output
//                   coordinates of vector of axis x
///////////////////////////////////////////////////////////////////////////////

void
LocalCs::GetAxisX(double& rAxX, double& rAxY, double& rAxZ)
{
    rAxX = mAxx;
    rAxY = mAxy;
    rAxZ = mAxz;
}



///////////////////////////////////////////////////////////////////////////////
// get axis y
// rAyX, rAyY, rAyZ: output
//                   coordinates of vector of axis y
///////////////////////////////////////////////////////////////////////////////

void
LocalCs::GetAxisY(double& rAyX, double& rAyY, double& rAyZ)
{
    rAyX = mAyx;
    rAyY = mAyy;
    rAyZ = mAyz;
}



///////////////////////////////////////////////////////////////////////////////
// get axis z
// rAzX, rAzY, rAzZ: output
//                   coordinates of vector of axis z
///////////////////////////////////////////////////////////////////////////////

void
LocalCs::GetAxisZ(double& rAzX, double& rAzY, double& rAzZ)
{
    rAzX = mAzx;
    rAzY = mAzy;
    rAzZ = mAzz;
}

///////////////////////////////////////////////////////////////////////////////
// set axis x and y
// aXx, aXy, aXz, aYx, aYy, aYz: input
//                               coordinates of vectors of axes x and y
// RETURN: true: success  false: failure
///////////////////////////////////////////////////////////////////////////////

bool
LocalCs::SetAxisXy(double aXx, double aXy, double aXz,
                   double aYx, double aYy, double aYz)
{
    double len, len1, len2;

    len1 = sqrt(aXx * aXx + aXy * aXy + aXz * aXz);
    len2 = sqrt(aYx * aYx + aYy * aYy + aYz * aYz);
    if (len1<MAX_TOL || len2<MAX_TOL)
        return false;

    mAxx = aXx / len1;
    mAxy = aXy / len1;
    mAxz = aXz / len1;
    mAyx = aYx / len2;
    mAyy = aYy / len2;
    mAyz = aYz / len2;
    len = mAxx * mAyx + mAxy * mAyy + mAxz * mAyz;
    if (len > MAX_TOL)
        return false;

    mAzx = mAxy * mAyz - mAxz * mAyy;
    mAzy = mAxz * mAyx - mAxx * mAyz;
    mAzz = mAxx * mAyy - mAxy * mAyx;
    
    if (!CalcRotAng())
        return false;
    
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// set axis y and z
// aYx, aYy, aYz, aZx, aZy, aZz: input
//                               coordinates of vectors of axes y and z
// RETURN: true: success  false: failure
///////////////////////////////////////////////////////////////////////////////

bool
LocalCs::SetAxisYz(double aYx, double aYy, double aYz,
                   double aZx, double aZy, double aZz)
{
    double len, len1, len2;

    len1 = sqrt(aYx * aYx + aYy * aYy + aYz * aYz);
    len2 = sqrt(aZx * aZx + aZy * aZy + aZz * aZz);
    if (len1<MAX_TOL || len2<MAX_TOL)
        return false;

    mAyx = aYx / len1;
    mAyy = aYy / len1;
    mAyz = aYz / len1;
    mAzx = aZx / len2;
    mAzy = aZy / len2;
    mAzz = aZz / len2;
    len = mAyx * mAzx + mAyy * mAzy + mAyz * mAzz;
    if (len > MAX_TOL)
        return false;

    mAxx = mAyy * mAzz - mAyz * mAzy;
    mAxy = mAyz * mAzx - mAyx * mAzz;
    mAxz = mAyx * mAzy - mAyy * mAzx;
    
    if (!CalcRotAng())
        return false;
    
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// set axis z and x
// aZx, aZy, aZz, aXx, aXy, aXz: input
//                               coordinates of vectors of axes z and x
// RETURN: true: success  false: failure
///////////////////////////////////////////////////////////////////////////////

bool
LocalCs::SetAxisZx(double aZx, double aZy, double aZz,
                   double aXx, double aXy, double aXz)
{
    double len, len1, len2;
 
    len1 = sqrt(aZx * aZx + aZy * aZy + aZz * aZz);
    len2 = sqrt(aXx * aXx + aXy * aXy + aXz * aXz);
    if (len1<MAX_TOL || len2<MAX_TOL)
        return false;

    mAzx = aZx / len1;
    mAzy = aZy / len1;
    mAzz = aZz / len1;
    mAxx = aXx / len2;
    mAxy = aXy / len2;
    mAxz = aXz / len2;
    len = mAzx * mAxx + mAzy * mAxy + mAzz * mAxz;
    if (len > MAX_TOL)
        return false;

    mAyx = mAzy * mAxz - mAzz * mAxy;
    mAyy = mAzz * mAxx - mAzx * mAxz;
    mAyz = mAzx * mAxy - mAzy * mAxx;
    
    if (!CalcRotAng())
        return false;
    
    return true;
}
