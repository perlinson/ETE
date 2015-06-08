// NAME: 
//   QuPlane
//
// DESCRIPTION: implementation of QuPlane class
// FILES: QuPlane.cpp
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////
#include "stdafx.h"
#include "QuPlane.h"                // definition of QuParaSur class



///////////////////////////////////////////////////////////////////////////////
// construction
///////////////////////////////////////////////////////////////////////////////

QuPlane::QuPlane()
{
    mA = 0.0;
    mB = 0.0;
    mC = 0.0;
    mD = 0.0;
}



///////////////////////////////////////////////////////////////////////////////
// destruction
///////////////////////////////////////////////////////////////////////////////

QuPlane::~QuPlane()
{
}



///////////////////////////////////////////////////////////////////////////////
// set coefficients
// a, b, c, d: input
//             coefficients
///////////////////////////////////////////////////////////////////////////////

void
QuPlane::SetCoef(double a, double b, double c, double d)
{
    mA = a;
    mB = b;
    mC = c;
    mD = d;
}



///////////////////////////////////////////////////////////////////////////////
// get coefficients
// rA, rB, rC, rD: output
//                 coefficients
///////////////////////////////////////////////////////////////////////////////

void
QuPlane::GetCoef(double& rA, double& rB, double& rC, double& rD)
{
    rA = mA;
    rB = mB;
    rC = mC;
    rD = mD;
}


////X////////X//////////////////////X//////////////////////////////////////////
