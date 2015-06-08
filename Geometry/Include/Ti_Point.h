// NAME: 
//   Ti_point
//
// DESCRIPTION: definition of Ti_point class
// FILES: Ti_point.h
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////
#ifndef _Ti_point_h_
#define _Ti_point_h_

// LOCAL INCLUDES

#include "LocalCs.h"                // definition of LocalCs class



class Ti_point
{
public: 

// LIFECYCLE

    Ti_point();                     // construction

    Ti_point(                       // construction
        double x, double y, double z);
                                    // coordinates

    virtual ~Ti_point();            // destruction

	long int    mindex;             // index in the point cloud
	Ti_point*   mpnext;             // pointer of the next point
    Ti_point*   mpprev;             // pointer of the previous point


// OPERATORS


// OPERATIONS

    void Init();                    // initiate point data

    void SetXyz(                    // set coordinates
        double x, double y, double z);
                                    // coordinates

    void SetNext(                   // set next point
        Ti_point* pPnt);            // pointer of next point

    void SetCalcFlag(               // set calculation flag
        bool clacFlag);             // calculation flag

    void SetMaxCur(                 // set maximum curvature
        double maxCur);             // maximum curvature

    void SetMinCur(                 // set minimum curvature
        double mimCur);             // minimum curvature

    void SetMaxDir(                 // set maximum curvature direction
        double x, double y, double z);
                                    // coordinates of direction

    void SetMinDir(                 // set minimum curvature direction
        double x, double y, double z);
                                    // coordinates of direction

    void ResetPtrs();               // reset previous and next pointers

    Ti_point* CopyData();           // copy point data
                                    // RETURN: pointer of new point
                                    //         if success
                                    //         0 if failure

    double DistToPnt(               // calculate distance to point
        Ti_point *pPnt);            // pointer of the point
                                    // RETURN: distance,

    double DistToLine(              // calculate distance to line segment
        Ti_point*   pFstPnt,        // first point of the line segment
        Ti_point*   pSndPnt);       // second point of the line segment
                                    // RETURN: distance

    double DistToPln(               // calculate distance to plane
        double a, double b, double c, double d);
                                    // coefficients of the plane:
                                    //      (a*x + b*y + c*z + d = 0)
                                    // RETURN: distance 

    void PrjToLine(                 // project to line segment
        Ti_point*   pFstPnt,        // first point of the line segment
        Ti_point*   pSndPnt,        // second point of the line segment
        double&     rT);            // proportion value of the projected point
                                    // project point is:
                                    //          pFstPnt * (1-rT) + pSndPnt * rT

    void PrjToPln(                 // project to plane
        double a, double b, double c, double d);
                                    // coefficients of the plane:
                                    //      (a*x + b*y + c*z + d = 0)

    void Shift(                     // shift point to a new location
        double deltX, double deltY, double deltZ);
                                    // shift distances in coordinates

    void RotAxisX(                  // rotate about axis x
        double sinA, double cosA);  // sine and cosine of the rotate angle

    void RotAxisY(                  // rotate about axis y
        double sinB, double cosB);  // sine and cosine of the rotate angle

    void RotAxisZ(                  // rotate about axis z
        double sinC, double cosC);  // sine and cosine of the rotate angle

    void TransToLocalCs(            // transform to local coordinate system
        LocalCs* lCs);              // pointer of local coordinate system

    void TransFromLocalCs(          // transform from local coordinate system
        LocalCs* lCs);              // pointer of local coordinate system
    


// ACCESS

    double GetX();                  // get coordinate x
                                    // RETURN: coordinate x

    double GetY();                  // get coordinate y
                                    // RETURN: coordinate y

    double GetZ();                  // get coordinate z
                                    // RETURN: coordinate z

    long int GetIndex();            // get point index
                                    // RETURN: point index

    Ti_point* GetNext();            // get next point
                                    // RETURN: pointer of next point
                                    // WARNING: the next point
                                    //          of end point is 0

    Ti_point* GetPrev();            // get previous point
                                    // RETURN: pointer of previous point
                                    // WARNING: the previous point
                                    //          of head point is 0

    double GetMaxCur();             // get maximum curvature
                                    // RETURN: maximum curvature

    double GetMinCur();             // get minimum curvature
                                    // RETURN: minimum curvature

    void GetMaxDir(                 // get maximum curvature direction
        double& rX, double& rY, double& rZ);
                                    // coordinates of direction

    void GetMinDir(                 // get minimum curvature direction
        double& rX, double& rY, double& rZ);
                                    // coordinates of direction



// INQUIRY

    bool IsCalcFlag();              // inquire the calculation flag
                                    // RETURN: true: flaged
                                    //         false: not flaged

    bool IsCoinPnt(                 // test if two point are coincident
        Ti_point* pOthPnt);         // pointer of the other point
                                    // RETURN: true: coincident
                                    //         false: not coincident



private: 

    //Ti_point*   mpnext;             // pointer of the next point
    //Ti_point*   mpprev;             // pointer of the previous point
    double      mx;                 // coordinate value x
    double      my;                 // coordinate value y 
    double      mz;                 // coordinate value z
    double      mwx;
    double      mwy; 
    double      mwz;
    //long int    mindex;             // index in the point cloud

    bool        mCalcFlag;          // calculation flag

    double      mMaxCur, mMaxDirX, mMaxDirY, mMaxDirZ;
                                    // maximum  curvature and direction

    double      mMinCur, mMinDirX, mMinDirY, mMinDirZ;
                                    // minimum  curvature and direction
};

#endif // _Ti_point_h_
////X////////X//////////////////////X//////////////////////////////////////////
