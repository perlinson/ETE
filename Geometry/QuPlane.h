// NAME: 
//   QuPlane
//
// DESCRIPTION: definition of QuPlane class
// FILES: QuPlane.h
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////

#ifndef _QuPlane_h_
#define _QuPlane_h_



class QuPlane
{
public:

// LIFECYCLE

	QuPlane();                      // construction

	virtual ~QuPlane();             // destruction



// OPERATIONS

    void SetCoef(                   // set coefficients
        double a, double b, double c, double d);
                                    // coefficients



// ACCESS

    void GetCoef(                   // get coefficients
        double& rA, double& rB, double& rC, double& rD);
                                    // coefficients



private:

    double      mA,mB,mC,mD;        // coefficient of the plane
                                    // plane Calculator: 
                                    //          (a*x + b*y + c*z +d = 0)

};

#endif // _QuPlane_h_
////X////////X//////////////////////X//////////////////////////////////////////
