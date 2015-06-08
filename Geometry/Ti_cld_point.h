/*
              ®q®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®r
              ®U  ©ÁDigital Manufacturing Laboratory ©Á   ®U
®q®T®T®T®T®T®T©»       Global Research Center, GE         ©¿®T®T®T®T®T®T®r
®U            ®U        Copyright(c) (2003-2006)          ®U            ®U
®U            ®t®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®T®s            ®U
®U                                                                      ®U
®U      File :             Ti_cld_point.h                               ®U
®U                                                                      ®U
®U      Programmer:        Xiaoming Du, Tian Chen and Guofei Hu         ®U
®U                                                                      ®U
®U      Last modified:     09/19/05 by Guofei Hu                        ®U
®U                                                                      ®U
®U°°    Description:       Define operations for 3D point cloud         ®U
®U                                                                      ®U
®U    ®q©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§®r    ®U
®t®T®T©»                   http://www.ge.com                      ©¿®T®T®s
      ®t©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§©§®s

 */
#ifndef _Ti_cld_point_h_
#define _Ti_cld_point_h_


#pragma once

// LOCAL INCLUDES
#include "Ti_point.h"               // definition of Ti_point class
#include "QuPlane.h"                // definition of QuPlane class
#include "QuCylinder.h"             // definition of QuSylinder class
#include "QuParaSur.h"              // definition of QuParaSur class
#include "xcloud.h"
#include "pnt.h"

class Ti_cld_point
{
public:
    Ti_cld_point();                 // construction
    virtual ~Ti_cld_point();        // destruction

    int i,j;
// OPERATIONS
    void CalcBox();                 // calculate boundary box
    void InitObject();              // calear all points
	void XCloud2Ti_Cloud(XCloud & Ti_Cloud);
    void ClearCalcFlags();          // clear calculation falgs of all points
    void AddPoint(                  // add a point
        Ti_point* pPnt);            // pointer of the point
	void AddPoint(double x,double y,double z); 
									// add a point: x,y,z
    void AddCloud(                  // add a point cloud (delete points in
                                    // the point cloud to be added)
        Ti_cld_point* pAddCld);     // pointer of point cloud
    void RemovePoint(               // remove a point
        Ti_point*   pPnt,           // pointer of the point
        bool        rmFlag = true); // delete flag: true: delete point (default)
                                    //              false: do not delete point
    Ti_cld_point* CopyData();       // copy point cloud data
                                    // RETURN: pointer of new point cloud
                                    //         if success
                                    //         0 if failure
    bool SortXyz();                 // sort point cloud 
                                    // according to coordinates x, y, z
                                    // RETURN: true: success    false: failure
    bool CalcCntDen();              // calculate density of points
                                    // RETURN: true: success    false: failure
    Ti_cld_point* GetSection(       // slice point cloud
        double a, double b, double c, double d,
                                    // coefficients of the section plane:
                                    //      (a*x + b*y + c*z + d = 0)
        double delt);               // slicing width
                                    // RETURN: the pointer of slicing
                                    //         point cloud if success 
                                    //         0 if failure
    Ti_cld_point* ExtractBtwTwoPlns(// extract point data
                                    // between two parallel planes
        double a, double b, double c, double d1, double d2,
                                    // coefficients of the two planes:
                                    //      (a*x + b*y + c*z + d1 = 0)
                                    //      (a*x + b*y + c*z + d2 = 0)
        bool cpyFlag=true);         // copy flag: true: copy the point data
                                    //                  (default)
                                    //            false: extract the point data
                                    // RETURN: the pointer of point cloud
                                    //         between the two parallel planes
                                    //         if success
                                    //         0 if failure

    Ti_point**  FindNearPnts(       // find the points near a point
        Ti_point*   pPnt,           // pointer of the calculation point
        double      range,          // distance range
        int&        num);           // number of near points
                                    // RETURN: point array if success
                                    //         0 if failure

    Ti_point**  FindNearPnts(       // find the points near a point
        Ti_point*   pPnt,           // pointer of the calculation point
        int         minnum,         // minimum number of points
        int         maxnum,         // maximum number of points
        int&        num);           // number of near points
                                    // RETURN: point array if success
                                    //         0 if failure
    
    Ti_point* FindNearestPnt(       // find nearest point
        Ti_point*   pPnt,           // pointer of the calculation point
        double&     rMinDis);       // distance
                                    // RETURN: pointer of nearest point
                                    //         if success
                                    //         0 if failure

    void FilterData(                // filter point data, create bin cells
        double sp);                 // edge length of bin cell

    void FilterData(                // filter point data, create bin cells
        int spnum);                 // edge length of bin cell

    bool FitPlane(                  // fit a plane to point cloud
        QuPlane*   pPln,            // pointer of fitted plane
        double&     maxErr,         // maximum fitting error
        double&     avgErr);        // average fitting error
                                    // RETURN: true: success    false: failure

    bool CalcFitInits(              // set fitting initials for cylinder
        QuCylinder* pCy,            // pointer of cylinder to be fitted
        Ti_point* pInitPnt=0);      // pointer of point for
                                    // calculation fitting initials
                                    // RETURN: true: success   false: failure

    bool FitParaSur(                // fit a parabolic surface to point cloud
        Ti_point*   pCntPnt,        // pointer of center point
        QuParaSur*  pParaSur,       // pointer of fitted parabolic surface
        double&     maxErr,         // maximum fitting error
        double&     avgErr);        // average fitting error
                                    // RETURN: true: success    false: failure


    bool CalcPriCur(                // calculate principal curvatures
                                    // and directions
        Ti_point*   pCntPnt, Ti_cld_point * NearTiCld	);       // pointer of center point
                                    // RETURN: true: success    false: failure

    bool CalcNormal(                // calculate normal
                                    // and directions
        Ti_point*   pCntPnt,        // pointer of center point
        double& rNx, double& rNy, double& rNz);
                                    // coordinates of normal vector
                                    // RETURN: true: success    false: failure



// ACCESS

    long GetNumber();               // get the number of points
                                    // RETURN: number of points

    Ti_point* GetHead();            // get the head point
                                    // RETURN: pointer of head point

    Ti_point* GetEnd();             // get the end point
                                    // RETURN: pointer of end point

    void GetBox(                    // get boundary box
        double& rMaxX, double& rMaxY, double& rMaxZ,
                                    // maximum value of coordinate x, y, z
        double& rMinX, double& rMinY, double& rMinZ);
                                    // minimum value of coordinate x, y, z

    double GetCntDen();             // get density of points
                                    // RETURN: density


// INQUIRY

    bool IsSorted();                // inquire if the point cloud is sorted
                                    // RETURN: true: sorted
                                    //         false: not sorted

    bool IsContainPnt(              // inquire if point cloud contain a point
        Ti_point* pPnt);            // pointer of point
                                    // RETURN: true: contain
                                    //         false: not contain

	bool Sort_byDistance(PNT& aComprVec);
	friend int _cdecl CompareDis(const void* elem1, const void* elem2);
	friend int _cdecl SortCompareXyz(const void* elem1,const void* elem2);

private:

    bool            mshow;          // show on screen or not 
    long            mnum;           // number of points 
    Ti_point*       mphead;         // pointer of head point
    Ti_point*       mpend;          // pointer of end point


    struct Information
    {                       
        double  maxx; 
        double  maxy; 
        double  maxz; 
        double  maxwx; 
        double  maxwy; 
        double  minx; 
        double  miny; 
        double  minz; 
        double  minwx; 
        double  minwy; 
    }  minfpoint;                   // boundary of cloud

    bool			mIsSorted;      // sort flag
    double			mCntDen;        // density of points

};
#endif // _Ti_cld_point_h_
