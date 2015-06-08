// NAME: 
//   LocalCs
//  this class don't have to export
// DESCRIPTION: definition of LocalCs class
// FILES: LocalCs.h
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////
#ifndef _LocalCs_h_
#define _LocalCs_h_


#define MAX_TOL     1e-5            // maximum tolerance



class LocalCs  
{
public:

// LIFECYCLE

    LocalCs();                      // construction

    virtual ~LocalCs();             // destruction



// OPERATIONS

    void SetOrigin(                 // set origin
        double oX, double oY, double oZ);
                                    // coordinates of origin

    bool SetAxisXy(                 // set axis x and y
        double aXx, double aXy, double aXz,
                                    // coordinates of vector of axis x
        double aYx, double aYy, double aYz);
                                    // coordinates of vector of axis y
                                    // RETURN: true: success  false: failure
    
    bool SetAxisYz(                 // set axis y and z
        double aYx, double aYy, double aYz,
                                    // coordinates of vector of axis y
        double aZx, double aZy, double aZz);
                                    // coordinates of vector of axis z
                                    // RETURN: true: success  false: failure

    bool SetAxisZx(                 // set axis z and x
        double aZx, double aZy, double aZz,
                                    // coordinates of vector of axis z
        double aXx, double aXy, double aXz);
                                    // coordinates of vector of axis x
                                    // RETURN: true: success  false: failure

    bool SetAxisZ(                  // set axis z
        double aZx, double aZy, double aZz);
                                    // coordinates of vector of axis z

    bool CalcRotAng();              // calculate rotate angle
                                    // RETURN: true: success  false: failure



// ACCESS

    void GetOrigin(                 // get origin
        double& rOx, double& rOy, double& rOz);
                                    // coordinates of origin

    void GetAxisX(                  // get axis x
        double& rAxX, double& rAxY, double& rAxZ);
                                    // coordinates of vector of axis x

    void GetAxisY(                  // get axis y
        double& rAyX, double& rAyY, double& rAyZ);
                                    // coordinates of vector of axis y

    void GetAxisZ(                  // get axis z
        double& rAzX, double& rAzY, double& rAzZ);
                                    // coordinates of vector of axis z

    void GetAngAlpha(               // get rotate angle alpha
        double& rSina,              // sine of the rotate angle
        double& rCosa);             // cosine of the rotate angle
        

    void GetAngBeta(                // get rotate angle beta
        double& rSinb,              // sine of the rotate angle
        double& rCosb);             // cosine of the rotate angle

    void GetAngGama(                // get rotate angle gama
        double& rSinc,              // sine of the rotate angle
        double& rCosc);             // cosine of the rotate angle



private: 

    double mOx;                     // coordinate value x of origin
    double mOy;                     // coordinate value y of origin
    double mOz;                     // coordinate value z of origin
    double mAxx;                    // coordinate value x of vector of axis x
    double mAxy;                    // coordinate value y of vector of axis x
    double mAxz;                    // coordinate value z of vector of axis x
    double mAyx;                    // coordinate value x of vector of axis y
    double mAyy;                    // coordinate value y of vector of axis y
    double mAyz;                    // coordinate value z of vector of axis y
    double mAzx;                    // coordinate value x of vector of axis z
    double mAzy;                    // coordinate value y of vector of axis z
    double mAzz;                    // coordinate value z of vector of axis z
    double mSinA;                   // sine of the rotate angle alpha
    double mCosA;                   // cosine of the rotate angle alpha
    double mSinB;                   // sine of the rotate angle belta
    double mCosB;                   // cosine of the rotate angle belta
    double mSinC;                   // sine of the rotate angle gama
    double mCosC;                   // cosine of the rotate angle gama
};

#endif // _LocalCs_h_
////X////////X//////////////////////X//////////////////////////////////////////
