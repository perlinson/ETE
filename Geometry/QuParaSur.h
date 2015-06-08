// NAME: 
//   QuParaSur
//
// DESCRIPTION: definition of QuParaSur class
// FILES: QuParaSur.h
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////

#ifndef _QuParaSur_h_
#define _QuParaSur_h_


// LOCAL INCLUDES

#include "LocalCs.h"                // definition of LocalCs class



class QuParaSur
{
public:

// LIFECYCLE

	QuParaSur();                    // construction

	virtual ~QuParaSur();           // destruction



// OPERATIONS

    bool SetCoef(                   // set coefficients
        double a, double b, double c);
                                    // coefficients
                                    // RETURN: true: success    false: failure

    void SetLocalCs(                // set local coordinate system
        LocalCs* pLcs);             // pointer of local coordinate system

    bool CalcPriCur();              // calculate principal 
                                    // curvatures and directions
                                    // RETURN: true: success    false: failure


// ACCESS

    void GetCoef(                   // get coefficients
        double& rA, double& rB, double& rC);
                                    // coefficients

    double GetMaxCur();             // get maximum curvature
                                    // RETURN: maximum curvature

    double GetMinCur();             // get minimum curvature
                                    // RETURN: minimum curvature

    void GetMaxDir(                 // get maximum curvature direction
        double& rX, double& rY, double& rZ);
                                    // coordinates of direction vector

    void GetMinDir(                 // get minimum curvature direction
        double& rX, double& rY, double& rZ);
                                    // coordinates of direction vector

    LocalCs* GetLocalCs();          // get local coordinate system
                                    // RETURN: pointer of local 
                                    //         coordinate system

    

private:
    
    double      mA,mB,mC;           // coefficients of the parabolic surface
                                    // Calculator of parabolic surface:
                                    //          (z = a*x*x + b*x*y + c*y*y)
    LocalCs*    mpLcs;              // pointer of local coordinate system

    double      mMaxCur,mMaxDirX,mMaxDirY,mMaxDirZ;
                                    // maximum  curvature and direction

    double      mMinCur,mMinDirX,mMinDirY,mMinDirZ;
                                    // minimum  curvature and direction

};

#endif // _QuParaSur_h_
////X////////X//////////////////////X//////////////////////////////////////////
