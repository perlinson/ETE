// QuCylinder.cpp: implementation of the QuCylinder class.
//
//////////////////////////////////////////////////////////////////////




#include "stdafx.h"
#include "QuCylinder.h"
#include "LocalCs.h"                // definition of LocalCs class
#include "math.h"    


//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QuCylinder::QuCylinder()
{
    mpTopPnt = 0;
    mpBotPnt = 0;
    mRadius = 0.0;
    mInitRo = 0.0;
    mInitPhi = 0.0;
    mInitTheta = 0.0;
    mInitK = 0.0;
    mInitAlpha = 0.0;
}




///////////////////////////////////////////////////////////////////////////////
// destruction
///////////////////////////////////////////////////////////////////////////////

QuCylinder::~QuCylinder()
{
  if (mpTopPnt != 0)
       delete mpTopPnt;
   if (mpBotPnt != 0)
      delete mpBotPnt;
}



///////////////////////////////////////////////////////////////////////////////
// set coefficients
// pTopPnt, pBotPnt: input
//                   pointers of center points of top and bottom circle
// rad: input
//      radius
// RETURN: true: success    false: failure
///////////////////////////////////////////////////////////////////////////////

bool
QuCylinder::SetCoef(Ti_point* pTopPnt, Ti_point* pBotPnt, double rad)
{
    double l,m,n;

    if (mpTopPnt != 0)
        delete mpTopPnt;
    if (mpBotPnt != 0)
        delete mpBotPnt;
    mpTopPnt = pTopPnt;
    mpBotPnt = pBotPnt;
    mRadius = rad;
    if (!GetAxis(l, m, n))
        return false;

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// set fitting initials
// ro, phi, theta, k, alpha: input
//                           fitting initials
///////////////////////////////////////////////////////////////////////////////

void
QuCylinder::SetFitInits(double ro, double phi, double theta,
                        double k, double alpha)
{
    mInitRo = ro;
    mInitPhi = phi;
    mInitTheta = theta;
    mInitK = k;
    mInitAlpha = alpha;
}



///////////////////////////////////////////////////////////////////////////////
// get axis direction
// rX, rY, rZ: output
//             coordinate of axis direction
// RETURN: true: success    false: failure
///////////////////////////////////////////////////////////////////////////////

bool
QuCylinder::GetAxis(double& rX, double& rY, double& rZ)
{
    double len;

    len = mpBotPnt->DistToPnt(mpTopPnt);
    if (len < 1e-5)
        return false;

    rX = mpTopPnt->GetX() - mpBotPnt->GetX();
    rY = mpTopPnt->GetY() - mpBotPnt->GetY();
    rZ = mpTopPnt->GetZ() - mpBotPnt->GetZ();
    rX = rX / len;
    rY = rY / len;
    rZ = rZ / len;
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// get center of the top circle
// RETURN: pointer of center point
///////////////////////////////////////////////////////////////////////////////

Ti_point*
QuCylinder::GetTopPnt()
{
    return mpTopPnt;
}



///////////////////////////////////////////////////////////////////////////////
// get center of the bottom circle
// RETURN: pointer of center point
///////////////////////////////////////////////////////////////////////////////

Ti_point*
QuCylinder::GetBotPnt()
{
    return mpBotPnt;
}



///////////////////////////////////////////////////////////////////////////////
// get radius
// RETURN: radius
///////////////////////////////////////////////////////////////////////////////

double
QuCylinder::GetRad()
{
    return mRadius;
}



///////////////////////////////////////////////////////////////////////////////
// get fitting initials
// rRo, rPhi, rTheta, rK, rAlpha: output
//                                fitting initials
///////////////////////////////////////////////////////////////////////////////

void
QuCylinder::GetFitInits(double& rRo, double& rPhi, double& rTheta,
                        double& rK, double& rAlpha)
{
    rRo = mInitRo;
    rPhi = mInitPhi;
    rTheta = mInitTheta;
    rK = mInitK;
    rAlpha = mInitAlpha;
}
