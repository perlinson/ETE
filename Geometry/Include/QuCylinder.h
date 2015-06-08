// QuCylinder.h: interface for the QuCylinder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUCYLINDER_H__A11D6D59_EA3C_47E7_810D_B1E109750A5B__INCLUDED_)
#define AFX_QUCYLINDER_H__A11D6D59_EA3C_47E7_810D_B1E109750A5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ti_Point.h"              

class QuCylinder
{
public:

// LIFECYCLE

	QuCylinder();                   // construction

	virtual ~QuCylinder();          // destruction




// OPERATIONS

    bool SetCoef(                   // set coefficients
        Ti_point*   pTopPnt,        // pointer of center point of top circle
        Ti_point*   pBotPnt,        // pointer of center point of bottom circle
        double rad);                // radius
                                    // RETURN: true: success    false: failure

    void SetFitInits(               // set fitting initials
        double ro, double phi, double theta, double k, double alpha);
                                    // fitting initials



// ACCESS

    bool GetAxis(                   // get axis direction
        double& rX,                 // coordinate x of axis direction
        double& rY,                 // coordinate y of axis direction
        double& rZ);                // coordinate z of axis direction
                                    // RETURN: true: success    false: failure

    Ti_point* GetTopPnt();          // get center of the top circle
                                    // RETURN: pointer of center point

    Ti_point* GetBotPnt();          // get center of the bottom circle
                                    // RETURN: pointer of center point

    double GetRad();                // get radius
                                    // RETURN: radius

    void GetFitInits(               // get fitting initials
        double& rRo, double& rPhi, double& rTheta, double& rK, double& rAlpha);
                                    // fitting initials


private:

    double      mInitRo, mInitPhi, mInitTheta, mInitK, mInitAlpha;
    Ti_point*   mpTopPnt;           // pointer of center point
                                    // of the top circle
    Ti_point*   mpBotPnt;           // pointer of center point
                                    // of the bottom circle
    double      mRadius;            // radius of the cylinder
};

#endif // !defined(AFX_QUCYLINDER_H__A11D6D59_EA3C_47E7_810D_B1E109750A5B__INCLUDED_)
