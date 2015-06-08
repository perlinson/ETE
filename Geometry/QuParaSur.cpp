// NAME: 
//   QuParaSur
//
// DESCRIPTION: implementation of QuParaSur class
// FILES: QuParaSur.cpp
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////
#include "stdafx.h"
#include "QuParaSur.h"              // definition of QuParaSur class
#include "math.h"                   // math functions
#include "Ti_point.h"               // definition of Ti_point class



///////////////////////////////////////////////////////////////////////////////
// construction
///////////////////////////////////////////////////////////////////////////////

QuParaSur::QuParaSur()
{
    mA = 0.0;
    mB = 0.0;
    mC = 0.0;
    mMaxCur = 0.0;
    mMaxDirX = 0.0;
    mMaxDirY = 0.0;
    mMaxDirZ = 0.0;
    mMinCur = 0.0;
    mMinDirX = 0.0;
    mMinDirY = 0.0;
    mMinDirZ = 0.0;
    mpLcs = new LocalCs ;
}



///////////////////////////////////////////////////////////////////////////////
// destruction
///////////////////////////////////////////////////////////////////////////////

QuParaSur::~QuParaSur()
{
    if (mpLcs != 0)
	    {
	    delete mpLcs;
	    mpLcs = NULL;
	    }
}



///////////////////////////////////////////////////////////////////////////////
// set coefficients
// a, b, c: input
//          coefficients
// RETURN: true: success    false: failure
///////////////////////////////////////////////////////////////////////////////

bool
QuParaSur::SetCoef(double a, double b, double c)
{
    mA = a;
    mB = b;
    mC = c;

    if (CalcPriCur())
        return true;
    else
        return false;
}



///////////////////////////////////////////////////////////////////////////////
// set local coordinate system
// pLcs: input
//       pointer of local coordinate system
///////////////////////////////////////////////////////////////////////////////

void
QuParaSur::SetLocalCs(LocalCs* pLcs)
{
    *mpLcs = *pLcs;
}



///////////////////////////////////////////////////////////////////////////////
// calculate principal curvatures and directions
// RETURN: true: success    false: failure
///////////////////////////////////////////////////////////////////////////////

bool
QuParaSur::CalcPriCur()
{
    double      x,y;
    double      temp;
    Ti_point*   tmppnt1,* tmppnt2;
    double      len;

    temp = sqrt((mA-mC) * (mA-mC) + mB * mB);

    tmppnt1 = new Ti_point(0.0, 0.0, 0.0);
    if (tmppnt1 == 0)
        return false;
    tmppnt1->TransFromLocalCs(mpLcs);

    mMinCur = mA + mC - temp;
    if (mA < mC)
    {
        x = mC - mA + temp;
        y = -mB;
    }
    else
    {
        x = mB;
        y = mC - mA - temp;
    }
    tmppnt2 = new Ti_point(x, y, 0.0);
    if (tmppnt2 == 0)
    {
        delete tmppnt1;
        return false;
    }
    tmppnt2->TransFromLocalCs(mpLcs);
    len = tmppnt2->DistToPnt(tmppnt1);
    if (len < MAX_TOL)
    {
        delete tmppnt1;
        delete tmppnt2;
        return false;
    }
    mMinDirX = (tmppnt2->GetX() - tmppnt1->GetX()) / len;
    mMinDirY = (tmppnt2->GetY() - tmppnt1->GetY()) / len;
    mMinDirZ = (tmppnt2->GetZ() - tmppnt1->GetZ()) / len;

    mMaxCur = mA + mC + temp;
    if (mA < mC)
    {
        x = mB;
        y = mC - mA + temp;
    }
    else
    {
        x = mC - mA - temp;
        y = -mB;
    }
    tmppnt2->SetXyz(x, y, 0.0);
    tmppnt2->TransFromLocalCs(mpLcs);
    len = tmppnt2->DistToPnt(tmppnt1);
    if (len < MAX_TOL)
    {
        delete tmppnt1;
        delete tmppnt2;
        return false;
    }
    mMaxDirX = (tmppnt2->GetX() - tmppnt1->GetX()) / len;
    mMaxDirY = (tmppnt2->GetY() - tmppnt1->GetY()) / len;
    mMaxDirZ = (tmppnt2->GetZ() - tmppnt1->GetZ()) / len;
    delete tmppnt1;
    delete tmppnt2;

    if (fabs(mMaxCur) < fabs(mMinCur))
    {
        temp = mMaxCur;
        mMaxCur = mMinCur;
        mMinCur = temp;
        temp = mMaxDirX;
        mMaxDirX = mMinDirX;
        mMinDirX = temp;
        temp = mMaxDirY;
        mMaxDirY = mMinDirY;
        mMinDirY = temp;
        temp = mMaxDirZ;
        mMaxDirZ = mMinDirZ;
        mMinDirZ = temp;
    }
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// get coefficients
// rA, rB, rC: output
//             coefficients
///////////////////////////////////////////////////////////////////////////////

void
QuParaSur::GetCoef(double& rA, double& rB, double& rC)
{
    rA = mA;
    rB = mB;
    rC = mC;
}



///////////////////////////////////////////////////////////////////////////////
// get maximum curvature
// RETURN: maximum curvature
///////////////////////////////////////////////////////////////////////////////

double
QuParaSur::GetMaxCur()
{
    return mMaxCur;
}



///////////////////////////////////////////////////////////////////////////////
// get minimum curvature
// RETURN: minimum curvature
///////////////////////////////////////////////////////////////////////////////

double
QuParaSur::GetMinCur()
{
    return mMinCur;
}



///////////////////////////////////////////////////////////////////////////////
// get maximum curvature direction
// rX, rY, rZ: output
//             coordinates of direction vector
///////////////////////////////////////////////////////////////////////////////

void
QuParaSur::GetMaxDir(double& rX, double& rY, double& rZ)
{
    rX = mMaxDirX;
    rY = mMaxDirY;
    rZ = mMaxDirZ;
}



///////////////////////////////////////////////////////////////////////////////
// get minimum curvature direction
// rX, rY, rZ: output
//             coordinates of direction vector
///////////////////////////////////////////////////////////////////////////////

void
QuParaSur::GetMinDir(double& rX, double& rY, double& rZ)
{
    rX = mMinDirX;
    rY = mMinDirY;
    rZ = mMinDirZ;
}



///////////////////////////////////////////////////////////////////////////////
// get local coordinate system
// RETURN: pointer of local coordinate system
///////////////////////////////////////////////////////////////////////////////

LocalCs*
QuParaSur::GetLocalCs()
{
    return mpLcs;
}

////X////////X//////////////////////X//////////////////////////////////////////
