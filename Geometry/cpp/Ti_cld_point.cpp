// NAME: 
//   Ti_cld_point
//
// DESCRIPTION: implementation of Ti_cld_point class
// FILES: Ti_cld_point.cpp
// TYPE: C++-CLASS 
////X////////X//////////////////////X//////////////////////////////////////////
#include "stdafx.h"
#include "Ti_cld_point.h"           // definition of Ti_cld_point class
#include "stdlib.h"                 // commonly used library functions
#include "math.h"                   // math functions
#include "ti_matrix.h"
#include "ti_svd.h"

PNT mComprVec;

///////////////////////////////////////////////////////////////////////////////
// construction
///////////////////////////////////////////////////////////////////////////////

Ti_cld_point::Ti_cld_point()
{
    mnum = 0;
    mphead = 0;
    mpend = 0;
    mIsSorted = false;
    mCntDen = -1.0;
    //mpBcs = 0;
    //mBcsNum = 0;
}



///////////////////////////////////////////////////////////////////////////////
// destruction
///////////////////////////////////////////////////////////////////////////////

Ti_cld_point::~Ti_cld_point()
{
    ClearCloud();
}



///////////////////////////////////////////////////////////////////////////////
// calculate boundary box
///////////////////////////////////////////////////////////////////////////////

void 
Ti_cld_point::CalcBox()
{
    double      x, y, z;
    Ti_point*   cirpnt;


    if (mphead == 0)
    {
        minfpoint.maxx = minfpoint.minx = 0.0;
        minfpoint.maxy = minfpoint.miny = 0.0;
        minfpoint.maxz = minfpoint.minz = 0.0;
        return;
    }

    minfpoint.maxx = minfpoint.minx = mphead->GetX();
    minfpoint.maxy = minfpoint.miny = mphead->GetY();
    minfpoint.maxz = minfpoint.minz = mphead->GetZ();

    cirpnt = mphead->GetNext();
    while (cirpnt != 0)
    {
        x = cirpnt->GetX();
        y = cirpnt->GetY();
        z = cirpnt->GetZ();
        if (minfpoint.maxx < x)
            minfpoint.maxx = x;
        if (minfpoint.maxy < y)
            minfpoint.maxy = y;
        if (minfpoint.maxz < z)
            minfpoint.maxz = z;
        if (minfpoint.minx > x)
            minfpoint.minx = x;
        if (minfpoint.miny > y)
            minfpoint.miny = y;
        if (minfpoint.minz > z)
            minfpoint.minz = z;
        cirpnt = cirpnt->GetNext();
    }
}



///////////////////////////////////////////////////////////////////////////////
// clear all points
///////////////////////////////////////////////////////////////////////////////

void Ti_cld_point::ClearCloud()
{
	
	
	// delete bin cells
    //RemoveBinCells();
	Ti_point* ptr=mphead;
	for(;mphead!=NULL;)
	{
		mphead=mphead->mpnext;		
		delete ptr;
		ptr=mphead;
	}
	mpend=NULL;
	mphead=NULL;
	mnum=0;
}



///////////////////////////////////////////////////////////////////////////////
// delete bin cells
///////////////////////////////////////////////////////////////////////////////

//void 
//Ti_cld_point::RemoveBinCells()
//{
//    if (mpBcs != 0)
//    {
//        for (int i=0; i<mBcsNum; i++)
//        {
//            if (mpBcs[i] != 0)
//                delete mpBcs[i];
//        }
//        free(mpBcs);
//    }
//    mpBcs = 0;
//    mBcsNum = 0;
//}
//
//
//
///////////////////////////////////////////////////////////////////////////////
// clear calculation falgs of all points
///////////////////////////////////////////////////////////////////////////////
void Ti_cld_point::XCloud2Ti_Cloud(XCloud & Ti_Cloud)
{
	int num = Ti_Cloud.GetPntNum();
	for(int i=0;i<num;i++)
	{
		this->AddPoint(Ti_Cloud[i][0],Ti_Cloud[i][1],Ti_Cloud[i][2]);
	}
}
void 
Ti_cld_point::ClearCalcFlags()
{
    Ti_point*   cirpnt;


    cirpnt = mphead;
    while(cirpnt != 0)
    {
        cirpnt->SetCalcFlag(false);
        cirpnt = cirpnt->GetNext();
    }
}



///////////////////////////////////////////////////////////////////////////////
// add a point
// pPnt: input
//       pointer of the point
///////////////////////////////////////////////////////////////////////////////

void 
Ti_cld_point::AddPoint(Ti_point* pPnt)
{
    double x, y, z;


    if (pPnt == 0)
        return;

    pPnt->Init();

    x = pPnt->GetX();
    y = pPnt->GetY();
    z = pPnt->GetZ();

    // if the cloud contains no point
    if (mnum == 0)
    {
        mphead = pPnt;
        minfpoint.maxx = minfpoint.minx = x;
        minfpoint.maxy = minfpoint.miny = y;
        minfpoint.maxz = minfpoint.minz = z;
    }

    // if the cloud contains point(s)
    else
    {
        mpend->SetNext(pPnt);
        if (minfpoint.maxx < x)
            minfpoint.maxx = x;
        if (minfpoint.maxy < y)
            minfpoint.maxy = y;
        if (minfpoint.maxz < z)
            minfpoint.maxz = z;
        if (minfpoint.minx > x)
            minfpoint.minx = x;
        if (minfpoint.miny > y)
            minfpoint.miny = y;
        if (minfpoint.minz > z)
            minfpoint.minz = z;
    }

    mpend = pPnt;
    mnum ++;
    mIsSorted = false;    
    // delete bin cells
    //RemoveBinCells();
}


///////////////////////////////////////////////////////////////////////////////
// add a point
// x, y,z: input
//  
///////////////////////////////////////////////////////////////////////////////

void 
Ti_cld_point::AddPoint(double x, double y, double z)
{
	Ti_point *p=new Ti_point(x,y,z);
	p->mindex = mnum;
	if(mphead==NULL)
		mphead=p;
	else
    {
        p->mpprev = mpend;
	    mpend->mpnext=p;
    }
	mpend=p;
    mphead->mpprev = NULL;
	mpend->mpnext=NULL;
	mnum++;
	
}

///////////////////////////////////////////////////////////////////////////////
// add a point cloud (delete points in the point cloud to be added)
// pAddCld: input
//          pointer of point cloud
///////////////////////////////////////////////////////////////////////////////

void 
Ti_cld_point::AddCloud(Ti_cld_point* pAddCld)
{
    Ti_point*   cirpnt, * rmpnt;


    if (pAddCld == 0)
        return;

    cirpnt = pAddCld->GetHead();
    while (cirpnt != 0)
    {
        rmpnt = cirpnt;
        cirpnt = cirpnt->GetNext();
        pAddCld->RemovePoint(rmpnt, false);
        AddPoint(rmpnt);
    }
}



///////////////////////////////////////////////////////////////////////////////
// remove a point
// pPnt: input
//       pointer of the point
// rmFlag: input
//         delete flag: true: delete point (default)
//                      false: do not delete point
///////////////////////////////////////////////////////////////////////////////

void 
Ti_cld_point::RemovePoint(Ti_point* pPnt, bool rmFlag)
{
    if (pPnt == 0)
        return;

    if (pPnt == mphead)
        mphead = pPnt->GetNext();
    if (pPnt == mpend)
        mpend = pPnt->GetPrev();
    pPnt->ResetPtrs();
    mnum --;
    mIsSorted = false;
    
    // delete bin cells
    //RemoveBinCells();

    if (rmFlag)
        delete pPnt;
}



///////////////////////////////////////////////////////////////////////////////
// copy point cloud data
// RETURN: pointer of new point cloud if success 
//         0 if failure
///////////////////////////////////////////////////////////////////////////////

Ti_cld_point* 
Ti_cld_point::CopyData()
{
    Ti_point*       cirpnt, * newpnt;
    Ti_cld_point*   newcld;
    

    newcld = new Ti_cld_point;
    if (newcld == 0)
        return 0;

    cirpnt = mphead;
    while(cirpnt != 0)
    {
        newpnt = cirpnt->CopyData();
        if (newpnt == 0)
        {
            delete newcld;
            return 0;
        }
        newcld->AddPoint(newpnt);
        cirpnt = cirpnt->GetNext();
    }
    return newcld;
}



int _cdecl 
SortCompareXyz(const void* elem1,const void* elem2)
{
    double x1 = (*((Ti_point**) elem1))->GetX();
    double y1 = (*((Ti_point**) elem1))->GetY();
    double z1 = (*((Ti_point**) elem1))->GetZ();
    double x2 = (*((Ti_point**) elem2))->GetX();
    double y2 = (*((Ti_point**) elem2))->GetY();
    double z2 = (*((Ti_point**) elem2))->GetZ();


	if (x1 > x2)
        return 1;
	else if (x1 < x2)
        return -1;
    else
	{
		if (y1 > y2)
            return 1;
		else if (y1 < y2)
            return -1;
		else
		{
			if (z1 > z2)
                return 1;
			else if (z1 < z2)
                return -1;
			else 
                return 0;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
// sort point cloud according to coordinates x, y, z
// RETURN: true: success   false: failure
///////////////////////////////////////////////////////////////////////////////

bool 
Ti_cld_point::SortXyz()
{
    Ti_point**  pntarray;
    Ti_point*   cirpnt;
    int         count;


    if (mnum <= 1)
        return false;

    pntarray = (Ti_point**) calloc((size_t)mnum, sizeof(int));
    if (pntarray == 0)
        return false;

    // initialize the pointer array by the point data
    cirpnt = mphead;
    count = 0;
    while (cirpnt != 0)
    {
        pntarray[count] = cirpnt;
        cirpnt = cirpnt->GetNext();
        count ++;
	}

    // sort the pointer array
    qsort((void*)pntarray, (size_t)mnum, sizeof(Ti_point*), SortCompareXyz);

    // change the point data by sorted pointer array
    for (int i=0; i<mnum; i++)
    {
        cirpnt = (Ti_point*) pntarray[i];
        pntarray[i] = 0;
        cirpnt->Init();
        if (i == 0)
            mphead = cirpnt;
        else
            mpend->SetNext(cirpnt);
        mpend = cirpnt;
    }

    mIsSorted = true;
    free(pntarray);
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// calculate density of points
// RETURN: true: success   false: failure
///////////////////////////////////////////////////////////////////////////////

bool 
Ti_cld_point::CalcCntDen()
{
    int             halfnum;
    Ti_point*       cirpnt = NULL;
    Ti_point*       nearpnt = NULL;


    // sort the points
    if (!mIsSorted)
    {
        if (!SortXyz())
            return false;
    }

    // pick a center point
    halfnum = (int) (mnum / 2);
    if (halfnum == 0)
        return false;
    cirpnt = mphead;
    for (int i=0; i<halfnum; i++)
    {
        if (cirpnt == 0)
            break;
        cirpnt = cirpnt->GetNext();
    }
    if (cirpnt == 0)
        return false;

    // find the nearest point of the center point
    nearpnt = FindNearestPnt(cirpnt, mCntDen);
    if (nearpnt == 0)
        return false;
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// slice point cloud
// a, b, c, d: input
//             coefficients of the section plane: (a*x + b*y + c*z + d = 0)
// delt: input
//       slicing width
// RETURN: the pointer of slicing point cloud  if success 
//         0 if failure
///////////////////////////////////////////////////////////////////////////////

Ti_cld_point* 
Ti_cld_point::GetSection(double a, double b, double c, double d, double delt)
{
    Ti_cld_point*   seccld, * poscld, * negcld;
    Ti_point*       cirpnt, * cirpnt1, * newpnt, * nearpnt;
    double          dis, dis1, dis2, mindis;
    double          t;
    double          x, y, z;


    poscld = new Ti_cld_point;
    if (poscld == 0)
        return 0;
    negcld = new Ti_cld_point;
    if (negcld == 0)
    {
        delete poscld;
        return 0;
    }

    if (delt < 0.0)
        delt = -delt;
    // get the points in the two portions with slicing width
    cirpnt = mphead;
	while (cirpnt != 0)
	{
		dis = cirpnt->DistToPln(a, b, c, d);
        if (dis>-delt && dis<delt)
        {
            newpnt = cirpnt->CopyData();
            if (newpnt == 0)
            {
                delete poscld;
                delete negcld;
                return 0;
            }
		    if(dis >= 0)
			    poscld->AddPoint(newpnt);
		    else
			    negcld->AddPoint(newpnt);
        }
        cirpnt = cirpnt->GetNext();
	}

    // no point in one portion
	if (poscld->GetNumber()==0 || negcld->GetNumber()==0)
    {
        delete poscld;
        delete negcld;
        return 0;
    }

    // calculate the slicing points
    seccld = new Ti_cld_point;
    if (seccld == 0)
    {
        delete poscld;
        delete negcld;
        return 0;
    }
    if (poscld->GetNumber() > negcld->GetNumber())
    {
        // test the points in positive portion
        cirpnt = poscld->GetHead();
        while(cirpnt != 0)
        {
            // find the nearest point in negative portion
            cirpnt1 = negcld->GetHead();
		    mindis = cirpnt->DistToPnt(cirpnt1);
            nearpnt = cirpnt1;
            cirpnt1 = cirpnt1->GetNext();
            while(cirpnt1 != 0)
            {
			    dis = cirpnt->DistToPnt(cirpnt1); 
			    if(dis < mindis)
			    {
				    mindis = dis;
				    nearpnt = cirpnt1;
			    }
                cirpnt1 = cirpnt1->GetNext();
		    }

            // get the intersection of the line of the two points
            // and the plane
		    dis1 = cirpnt->DistToPln(a, b, c, d);
		    dis2 = nearpnt->DistToPln(a, b, c, d);
		    if (fabs(dis1) + fabs(dis2) <= 0.00001)
			    t = 0.0;
		    else 
                t = fabs(dis1) / (fabs(dis1) + fabs(dis2));

		    x = cirpnt->GetX() + t * (nearpnt->GetX() - cirpnt->GetX());
		    y = cirpnt->GetY() + t * (nearpnt->GetY() - cirpnt->GetY());
		    z = cirpnt->GetZ() + t * (nearpnt->GetZ() - cirpnt->GetZ());

            newpnt = new Ti_point(x, y, z);
            if (newpnt == 0)
            {
                delete poscld;
                delete negcld;
                delete seccld;
                return 0;
            }
		    seccld->AddPoint(newpnt);

            cirpnt = cirpnt->GetNext();
        }
    }
    else
    {
        // test the points in negative portion
        cirpnt = negcld->GetHead();
        while(cirpnt != 0)
        {
            // find the nearest point in positive portion
            cirpnt1 = poscld->GetHead();
		    mindis = cirpnt->DistToPnt(cirpnt1);
            nearpnt = cirpnt1;
            cirpnt1 = cirpnt1->GetNext();
            while(cirpnt1 != 0)
            {
			    dis = cirpnt->DistToPnt(cirpnt1); 
			    if(dis < mindis)
			    {
				    mindis = dis;
				    nearpnt = cirpnt1;
			    }
                cirpnt1 = cirpnt1->GetNext();
		    }

            // get the intersection of the line of the two points
            // and the plane
		    dis1 = cirpnt->DistToPln(a, b, c, d);
		    dis2 = nearpnt->DistToPln(a, b, c, d);
		    if (fabs(dis1) + fabs(dis2) <= 0.00001)
			    t = 0.0;
		    else 
                t = fabs(dis1) / (fabs(dis1) + fabs(dis2));

		    x = cirpnt->GetX() + t * (nearpnt->GetX() - cirpnt->GetX());
		    y = cirpnt->GetY() + t * (nearpnt->GetY() - cirpnt->GetY());
		    z = cirpnt->GetZ() + t * (nearpnt->GetZ() - cirpnt->GetZ());

            newpnt = new Ti_point(x, y, z);
            if (newpnt == 0)
            {
                delete poscld;
                delete negcld;
                delete seccld;
                return 0;
            }
		    seccld->AddPoint(newpnt);

            cirpnt = cirpnt->GetNext();
        }
    }

    delete poscld;
    delete negcld;
    return seccld;
}



///////////////////////////////////////////////////////////////////////////////
// extract point data between two parallel planes
// a, b, c, d1, d2: input
//                  coefficients of the two planes: 
//                  (a*x + b*y + c*z + d1 = 0) and (a*x + b*y + c*z + d2 = 0)
// cpyFlag: input
//          copy flag: true: copy the point data (default)
//                     false: extract the point data
// RETURN: the pointer of point cloud between the two parallel planes
//         if success, 0 if failure
///////////////////////////////////////////////////////////////////////////////

Ti_cld_point*
Ti_cld_point::ExtractBtwTwoPlns(double a, double b, double c, double d1,
                                double d2, bool cpyFlag)
{
    double          len1, len2;
    Ti_point*       cirpnt, * newpnt;
    Ti_cld_point*   newcld;


    if (mnum == 0)
        return 0;

    newcld = new Ti_cld_point;
    if (newcld == 0)
        return 0;

    cirpnt = mphead;
    while(cirpnt != 0)
    {
        len1 = cirpnt->DistToPln(a, b, c, d1);
        len2 = cirpnt->DistToPln(a, b, c, d2);
        if (len1*len2 < 0.0)
        {
            if (cpyFlag)
            {
                newpnt = cirpnt->CopyData();
                if (newpnt == 0)
                {
                    delete newcld;
                    return 0;
                }
            }
            else
            {
                newpnt = cirpnt;
                cirpnt = cirpnt->GetPrev();
                RemovePoint(newpnt, false);
            }
            newcld->AddPoint(newpnt);
        }
        cirpnt = cirpnt->GetNext();
    }
    return newcld;
}



///////////////////////////////////////////////////////////////////////////////
// find the points near a point
// pPnt: input
//       pointer of the calculation point
// range: input
//        distance range
// num: output
//      number of near points
// RETURN: point array if success, 0 if failure
///////////////////////////////////////////////////////////////////////////////

Ti_point**
Ti_cld_point::FindNearPnts(Ti_point* pPnt, double range, int& num)
{
    Ti_point*   cirpnt;
    double      x1, y1, z1, x2, y2, z2;
    Ti_point*   stpnt, * edpnt;
    Ti_point**  nearpnts;
    int		index;


    num = 0;

    if (pPnt == 0)
        return 0;

    if (range < 0.0)
        range = -range;

    // test if the point is in current point cloud
    cirpnt = mphead;
    while (cirpnt != 0)
    {
        if (cirpnt == pPnt)
            break;
        if (cirpnt->IsCoinPnt(pPnt))
            break;
		cirpnt=cirpnt->GetNext();
    }
    if (cirpnt == 0)
        return 0;

    // sort the points
    if (!mIsSorted)
    {
        if (!SortXyz())
            return 0;
    }

    // search forward
    cirpnt = pPnt->GetPrev();
    x1 = pPnt->GetX();
    y1 = pPnt->GetY();
    z1 = pPnt->GetZ();
    stpnt = 0;
    while (cirpnt != 0)
    {
        x2 = cirpnt->GetX();
        y2 = cirpnt->GetY();
        z2 = cirpnt->GetZ();
		if (x2 < x1 - range)
            break;
		if (y2>y1-range && y2<y1+range && z2>z1-range && z2<z1+range)
		{
			if (cirpnt->DistToPnt(pPnt) <= range)
            {
                cirpnt->SetCalcFlag(true);
                stpnt = cirpnt;
                num ++;
            }
		}
	    cirpnt = cirpnt->GetPrev();
    }
    
    // search backward
    cirpnt = pPnt->GetNext();
    edpnt = 0;
    while (cirpnt != 0)
    {
        x2 = cirpnt->GetX();
        y2 = cirpnt->GetY();
        z2 = cirpnt->GetZ();
		if (x2 > x1 + range)
            break;
		if (y2>y1-range && y2<y1+range && z2>z1-range && z2<z1+range)
		{
			if (cirpnt->DistToPnt(pPnt) <= range)
            {
                cirpnt->SetCalcFlag(true);
                edpnt = cirpnt;
                num ++;
            }
		}
	    cirpnt = cirpnt->GetNext();
    }

    if (num == 0)
        return 0;

    // get the near points
    nearpnts = (Ti_point**) calloc((size_t)num, sizeof(int));
    if (nearpnts == 0)
    {
        ClearCalcFlags();
        return 0;
    }
    if (stpnt == 0)
        cirpnt = mphead;
    else
        cirpnt = stpnt;
    index = 0;
    while(cirpnt != 0)
    {
        if (cirpnt->IsCalcFlag())
        {
            cirpnt->SetCalcFlag(false);
            nearpnts[index] = cirpnt;
            index ++;
        }
        if (cirpnt == edpnt)
            break;
        cirpnt = cirpnt->GetNext();
    }
    return nearpnts;
}



///////////////////////////////////////////////////////////////////////////////
// find the points near a point
// pPnt: input
//       pointer of the calculation point
// minnum: input
//         minimum number of points
// maxnum: input
//         maximum number of points
// num: output
//      number of near points
// RETURN: point array if success, 0 if failure
///////////////////////////////////////////////////////////////////////////////

Ti_point**
Ti_cld_point::FindNearPnts(Ti_point* pPnt, int minnum, int maxnum, int& num)
{
    Ti_point**  nearpnts;
    int         circount;
    int         temp;
    double      range;
    double      multi;


    num = 0;

    if (pPnt == 0)
        return 0;

    if (mnum<minnum || minnum<1 || maxnum<1)
        return 0;

    if (maxnum < minnum)
    {
        temp = minnum;
        minnum = maxnum;
        maxnum = temp;
    }

    if (mCntDen < 0.0)
        CalcCntDen();

    // test the distance range to find the near points
    multi = 1.1;
    range = multi * mCntDen;
    circount = 0;
    while(1)
    {
        circount ++;
        if (circount > 100)
            break;
        nearpnts = FindNearPnts(pPnt, range, num);
        if (num >= minnum)
        {
            if (num > maxnum)
            {
                multi = multi / 2.0;
                range = range - multi * mCntDen;
                free(nearpnts);
                continue;
            }
            return nearpnts;
        }
        if (multi < 1.1)
            multi = multi / 2.0;
        range = range + multi * mCntDen;
        free(nearpnts);
    }

    num = 0;
    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// find nearest point
// pPnt: input
//       pointer of the calculation point
// rMinDis: output
//          distance
// RETURN: pointer of the nearest point if success
//         0 if failure
///////////////////////////////////////////////////////////////////////////////

Ti_point*
Ti_cld_point::FindNearestPnt(Ti_point* pPnt, double& rMinDis)
{
    double      dis;
    Ti_point**  nearpnts;
    Ti_point*   cirpnt, * nearpnt;
    int         num;
    // calculate the maximum length of boundary box

    dis = minfpoint.maxx - minfpoint.minx;
    if ((minfpoint.maxx-minfpoint.minx) < (minfpoint.maxy-minfpoint.miny))
    {
        dis = minfpoint.maxy - minfpoint.miny;
        if ((minfpoint.maxy-minfpoint.miny) < (minfpoint.maxz-minfpoint.minz))
            dis = minfpoint.maxz - minfpoint.minz;
    }
    else
    {
        if ((minfpoint.maxx-minfpoint.minx) < (minfpoint.maxz-minfpoint.minz))
            dis = minfpoint.maxz - minfpoint.minz;
    }

    // test the distance range to find the near points
    for (int i=100; i>=1; i--)
    {
        nearpnts = FindNearPnts(pPnt, dis/i, num);
        if (num != 0)
            break;
    }
    if (num == 0)
        return 0;

    // calculate the nearest point in the found near points
    cirpnt = nearpnts[0];
    rMinDis = dis = pPnt->DistToPnt(cirpnt);
    nearpnt = cirpnt;
    for (i=1; i<num; i++)
    {
        cirpnt = nearpnts[i];
        dis = pPnt->DistToPnt(cirpnt);
        if (rMinDis > dis)
        {
            rMinDis = dis;
            nearpnt = cirpnt;
        }
    }
    free(nearpnts);
    return nearpnt;
}



extern int _cdecl SortCompareYzx(const void* elem1, const void* elem2);
extern int _cdecl SortCompareZxy(const void* elem1, const void* elem2);
///////////////////////////////////////////////////////////////////////////////
// filter point data, create bin cells
// sp: input
//     edge length of bin cell
///////////////////////////////////////////////////////////////////////////////

//void
//Ti_cld_point::FilterData(double sp)
//{
//    double      maxx, maxy, maxz, minx, miny, minz;
//    double      dnum;
//    double      numx, numy, numz;
//    double      spx, spy, spz;
//    BinCell*    bin;
//    BinCell**   bins;
//    Ti_point*   cirpnt, * stpnt, * edpnt, * boxpnt;
//    int         countx, county, countz;
//    double      dis, mindis;
//    Ti_point**  pntarrayx, ** pntarrayy, ** cellpnts;
//    int         sty, stz;
//#ifdef _DEBUG 
//	_CrtMemState s1, s2, s3; 
//	_CrtMemCheckpoint( &s1 ); 
//#endif 
//
//    // delete bin cells
//    RemoveBinCells();
//
//    // sort the point data
//    if (!mIsSorted)
//    {
//        if (!SortXyz())
//            return;
//    }
//
//    // calculate the number of bin cells
//    maxx = minfpoint.maxx + MAX_TOL;
//    maxy = minfpoint.maxy + MAX_TOL;
//    maxz = minfpoint.maxz + MAX_TOL;
//    minx = minfpoint.minx - MAX_TOL;
//    miny = minfpoint.miny - MAX_TOL;
//    minz = minfpoint.minz - MAX_TOL;
//
//    dnum = (maxx - minx) / sp;
//    if (dnum < 0.0)
//        return;
//    numx = (int)dnum + 1;
//    dnum = (maxy - miny) / sp;
//    if (dnum < 0.0)
//        return;
//    numy = (int)dnum + 1;
//    dnum = (maxz - minz) / sp;
//    if (dnum < 0.0)
//        return;
//    numz = (int)dnum + 1;
//
//    spx = (maxx - minx) / numx;
//    spy = (maxy - miny) / numy;
//    spz = (maxz - minz) / numz;
//
//    bins = (BinCell**) calloc((size_t)(numx*numy*numz), sizeof(int));
//    if (bins == 0)
//        return;
//
//    // calculate each bin cell
//    stpnt = mphead;
//    edpnt = 0;
//    for (int i=0; i<numx; i++)
//    {
//        // start split in axis x
//        cirpnt = stpnt;
//        countx = 0;
//
//        // calculate the number of pointers in the portion
//        // splitted by plane perpendicular axis x
//        while(cirpnt != 0)
//        {
//            dis = cirpnt->DistToPln(1.0, 0.0, 0.0, -(minx+(i+1)*spx));
//            if (dis > 0.0)
//            {
//                edpnt = cirpnt;
//                break;
//            }
//            countx ++;
//            cirpnt = cirpnt->GetNext();
//        }
//        
//        // start process this portion
//        // sort the pointers in this portion
//        if (countx != 0)
//        {
//            pntarrayx = (Ti_point**) calloc((size_t)countx, sizeof(int));
//            if (pntarrayx == 0)
//            {
//                for (int ii=0; ii<numx*numy*numz; ii++)
//                {
//                    if (bins[ii] != 0)
//                        delete bins[ii];
//                }
//                free(bins);
//                return;
//            }
//            cirpnt = stpnt;
//            for (int l=0; l<countx; l++)
//            {
//                pntarrayx[l] = cirpnt;
//                cirpnt = cirpnt->GetNext();
//            }
//            qsort((void*)pntarrayx, (size_t)countx, sizeof(Ti_point*),
//                  SortCompareYzx);
//
//            sty = 0;
//            for (int j=0; j<numy; j++)
//            {
//                // start split in axis y
//                county = 0;
//                
//                // calculate the number of pointers in the portion
//                // splitted by plane perpendicular axis y
//                for (l=sty; l<countx; l++)
//                {
//                    cirpnt = pntarrayx[l];
//                    dis = cirpnt->DistToPln(0.0, 1.0, 0.0,
//                                            -(miny+(j+1)*spy));
//                    if (dis > 0.0)
//                        break;
//                    county ++;
//                }
//
//                // start process this portion
//                // sort the pointers in this portion
//                if (county != 0)
//                {
//                    pntarrayy = (Ti_point**) calloc((size_t)county,
//                                                    sizeof(int));
//                    if (pntarrayy == 0)
//                    {
//                        free(pntarrayx);
//                        for (int ii=0; ii<numx*numy*numz; ii++)
//                        {
//                            if (bins[ii] != 0)
//                                delete bins[ii];
//                        }
//                        free(bins);
//                        return;
//                    }
//                    for (l=0; l<county; l++)
//                        pntarrayy[l] = pntarrayx[l + sty];
//                    qsort((void*)pntarrayy, (size_t)county,
//                          sizeof(Ti_point*), SortCompareZxy);
//
//                    stz = 0;
//                    for (int k=0; k<numz; k++)
//                    {
//                        // start split in axis z
//                        countz = 0;
//
//                        // calculate the number of pointers in the portion
//                        // splitted by plane perpendicular axis z
//                        for (l=stz; l<county; l++)
//                        {
//                            cirpnt = pntarrayy[l];
//                            dis = cirpnt->DistToPln(0.0, 0.0, 1.0,
//                                                    -(minz+(k+1)*spz));
//                            if (dis > 0.0)
//                                break;
//                            countz ++;
//                        }
//                        // start process bin cell
//                        if (countz != 0 )
//                        {
//                            bin = new BinCell;
//                            if (bin == 0)
//                            {
//                                free(pntarrayx);
//                                free(pntarrayy);
//                                for (int ii=0; ii<numx*numy*numz; ii++)
//                                {
//                                    if (bins[ii] != 0)
//                                        delete bins[ii];
//                                }
//                                free(bins);
//                                return;
//                            }
//
//                            // set boundary box of bin cell
//                            bin->SetBox(minx+i*spx, minx+(i+1)*spx,
//                                        miny+j*spy, miny+(j+1)*spy,
//                                        minz+k*spz, minz+(k+1)*spz);
//
//                            cellpnts = (Ti_point**) calloc((size_t)countz,
//                                                            sizeof(int));
//                            // set center point and point array of bin cell
//                            if (cellpnts == 0)
//                            {
//                                free(pntarrayx);
//                                free(pntarrayy);
//                                for (int ii=0; ii<numx*numy*numz; ii++)
//                                {
//                                    if (bins[ii] != 0)
//                                        delete bins[ii];
//                                }
//                                free(bins);
//                                return;
//                            }
//
//                            boxpnt= new Ti_point(minx+(i+0.5)*spx,
//                                                 miny+(j+0.5)*spy,
//                                                 minz+(k+0.5)*spz);
//                            if (boxpnt == 0)
//                            {
//                                free(cellpnts);
//                                free(pntarrayx);
//                                free(pntarrayy);
//                                for (int ii=0; ii<numx*numy*numz; ii++)
//                                {
//                                    if (bins[ii] != 0)
//                                        delete bins[ii];
//                                }
//                                free(bins);
//                                return;
//                            }
//                            cirpnt = pntarrayy[stz];
//                            cellpnts[0] = cirpnt;
//                            mindis = dis = cirpnt->DistToPnt(boxpnt);
//                            bin->SetCntPnt(cirpnt);
//                            for (l=1; l<countz; l++)
//                            {
//                                cirpnt = pntarrayy[l + stz];
//                                cellpnts[l] = cirpnt;
//                                dis = cirpnt->DistToPnt(boxpnt);
//                                if (mindis > dis)
//                                {
//                                    mindis = dis;
//                                    bin->SetCntPnt(cirpnt);
//                                }
//                            }
//                            bin->SetPoints(countz, cellpnts);
//                            delete boxpnt;
//                            bins[mBcsNum] = bin;
//                            mBcsNum ++;
//                        }
//                        // end process bin cell
//
//                        stz = stz + countz;
//                        // end split in axis z
//                    }
//                    free(pntarrayy);
//                }
//                sty = sty + county;
//                // end split in axis y
//            }
//            free(pntarrayx);
//        }
//        stpnt = edpnt;
//        edpnt = 0;
//        // end split in axis x
//    }
//
//    // write down bin cells
//    mpBcs = (BinCell**) calloc((size_t)(mBcsNum), sizeof(int));
//    if (mpBcs == 0)
//    {
//        for (int ii=0; ii<numx*numy*numz; ii++)
//        {
//            if (bins[ii] != 0)
//                delete bins[ii];
//        }
//        free(bins);
//        return;
//    }
//    for (i=0; i<mBcsNum; i++)
//        mpBcs[i] = bins[i];
//    free(bins);
//#ifdef _DEBUG
//	_CrtMemCheckpoint( &s2 ); 
//	if ( _CrtMemDifference( &s3, &s1, &s2 ) )
//		{
//		_CrtMemDumpStatistics( &s3 ); 
//		//_CrtDumpMemoryLeaks();
//		}
//#endif
//}
//
//
//
///////////////////////////////////////////////////////////////////////////////
// filter point data, create bin cells
// percent: input
//          edge length of bin cell
///////////////////////////////////////////////////////////////////////////////

//void
//Ti_cld_point::FilterData(int spnum)
//{
//    double sp;
//
//
//    if (spnum < 1)
//        spnum = 1;
//    spnum ++;
//    if (mCntDen < 0)
//        CalcCntDen();
//    sp = mCntDen * spnum;
//   if(sp < 0.01)
//	    sp = 0.01;
//    FilterData(sp);
//}
//
//
//
///////////////////////////////////////////////////////////////////////////////
// extract point cloud in a bin cell
// pBc: input
//      pointer of bin cell
// cpyFlag: input
//          copy flag: true: copy the point data
//                     false: extract the point data (default)
// RETURN: pointer of point cloud if success
//         0 if falilure
///////////////////////////////////////////////////////////////////////////////

//Ti_cld_point*
//Ti_cld_point::ExtractFrmBinCell(BinCell* pBc, bool cpyFlag)
//{
//    Ti_point*       cirpnt, * newpnt;
//    Ti_cld_point*   newcld;
//    int             num;
//
//
//    num = pBc->GetNumber();
//    if (num == 0)
//        return 0;
//
//    newcld = new Ti_cld_point;
//    if (newcld == 0)
//        return 0;
//
//    for (int i=0; i<num; i++)
//    {
//        cirpnt = pBc->GetPoint(i);
//        if (cpyFlag)
//        {
//            newpnt = cirpnt->CopyData();
//            if (newpnt == 0)
//            {
//                delete newcld;
//                return 0;
//            }
//        }
//        else
//        {
//            newpnt = cirpnt;
//            RemovePoint(cirpnt, false);
//        }
//        newcld->AddPoint(newpnt);
//    }
//    return newcld;
//}
//
//
//
extern int _cdecl SortCompareBcs(const void* elem1, const void* elem2);
///////////////////////////////////////////////////////////////////////////////
// extract point data in blending region
// fltNum: input
//         edge length of bin cell for filtering point data
// curShr: input
//         curvature threshold cross blending region
// brkShr: input
//         curvature threshold along blending region
// RETURN: pointer of point cloud in blending region if success
//         0 if failure
//  Revised by Xiaoming Du on Sep 8, 2004
///////////////////////////////////////////////////////////////////////////////

//Ti_cld_point* 
//Ti_cld_point::SegmentBlend(int fltNum, double curShr, double brkShr)
//{
//    double          brkcur;
//    Ti_point*       cntpnt, * comppnt;
//    double          maxcur;
//    double          mindirx, mindiry, mindirz;
//    double          compmaxcur;
//    double          a, b, c, d;
//    BinCell**       bldbcs;
//    Ti_cld_point*   bldcld, * tempcld;
//    int             index;
//
//	if((minfpoint.maxx - minfpoint.minx) < MAX_TOL)
//	{
//		CalcBox();
//	}
//	CalcCntDen();
//    if (mnum < 24)
//	{ 
//		AfxMessageBox("Not Enough Point Data Provided! Please Select Another File!") ;
//		return 0;
//	}
//
//    // filter data
//    FilterData(fltNum);
//
//    // calculate principal curvatures and directions
//    for (int i=0; i<mBcsNum; i++)
//    {
//        cntpnt = mpBcs[i]->GetCntPnt();
//        if (!CalcPriCur(cntpnt))
//        {
//            RemoveBinCells();
//            return NULL;
//        }
//    }
//
//
//
//    // sort the bincell
//    if(mpBcs == NULL)
//	    return NULL;
//    // sort the bincell according to its  maximum curvature 
//    qsort((void*)mpBcs, (size_t)mBcsNum, sizeof(BinCell*), SortCompareBcs);
//
//
//	
//    // extract bin cells in blend
//    bldbcs = (BinCell**) calloc((size_t)mBcsNum, sizeof(int));
//    
//	
//	if (bldbcs == 0)
//    {
//        RemoveBinCells();
//        return 0;
//    }
//    cntpnt = mpBcs[0]->GetCntPnt();
//    for(i=0; i< 4; i++)
//	    {
//	            cntpnt = mpBcs[i]->GetCntPnt();
//		    maxcur = cntpnt->GetMaxCur();
//	    }
//    brkcur = fabs(cntpnt->GetMaxCur()) * brkShr;
//	
//    index = 0;
//    for (i=0; i<mBcsNum; i++)
//    {
//        if (mpBcs[i] == 0)
//            continue;
//        cntpnt = mpBcs[i]->GetCntPnt();
//        cntpnt->GetMinDir(mindirx, mindiry, mindirz);
//        maxcur = fabs(cntpnt->GetMaxCur());
//       		
//		
//		if (maxcur < brkcur)
//            break;
//        bldbcs[index] = mpBcs[i];
//        index ++;
//        mpBcs[i] = 0;
//        		
//	
//		for (int j=i+1; j<mBcsNum; j++)
//        {
//            if (mpBcs[j] == 0)
//                continue;
//            a = mindirx;
//            b = mindiry;
//            c = mindirz;
//            d = -(a*cntpnt->GetX() + b*cntpnt->GetY() + c*cntpnt->GetZ());
//            if (mpBcs[j]->IsOnPln(a, b, c, d))
//            {
//                comppnt = mpBcs[j]->GetCntPnt();
//                compmaxcur = fabs(comppnt->GetMaxCur());
//                if (compmaxcur > maxcur*curShr)
//                {
//                    bldbcs[index] = mpBcs[j];
//                    index ++;
//                    mpBcs[j] = 0;
//                }
//                else
//                {
//                    delete mpBcs[j];
//                    mpBcs[j] = 0;
//                }
//            }
//        }
//    }
//
//    RemoveBinCells();
//
//    if (index == 0)
//        return 0;
//
//    // extract point cloud from the bin cells
//    	
//	bldcld = ExtractFrmBinCell(bldbcs[0]);
//    delete bldbcs[0];
//    for (i=1; i<index; i++)
//    {
//		tempcld = ExtractFrmBinCell(bldbcs[i]);
//        bldcld->AddCloud(tempcld);
//        delete bldbcs[i];
//		delete tempcld;
//		tempcld = NULL;
//    }
//    free((void*)bldbcs);
//
//    return bldcld;
//}
//
//
//
///////////////////////////////////////////////////////////////////////////////
// recover parameters of blending surface
// step: input
//       spacing of the recover profile curves
// RETURN: true: success    false: failure
///////////////////////////////////////////////////////////////////////////////

//bool
//Ti_cld_point::RecoverBlend(int step, Blend*  pBld)
//{
//    double          maxcur;
//    Ti_point*       startpnt;
//    double          x, y, z, nx, ny, nz;
//    double          compnx, compny, compnz;
//    double          startx, starty, startz;
//    double          startnx, startny, startnz;
//    double          marchnx, marchny, marchnz;
//    QuCylinder*     startcy, * cylinder;
//    Ti_cld_point*   beltcld1, * beltcld2;
//    double          lastx, lasty, lastz;
//    double          ro, phi, theta, k, alpha;
//    double          maxerr, avgerr;
//    Ti_point*       toppnt, * botpnt;
//    double          width, d;
//    bool            posover, negover;
//    int             num, lastnum, lastnum1, lastnum2;
////    bool            endctrl;
//    CircleArc*      arc;
//    int             circount;
//    int             minnum = 12;//modify from 12 to 2.
//    double          errang = 30.0;
//    double          errrad = 2.0;
//
//	if((minfpoint.maxx - minfpoint.minx) < MAX_TOL)
//	{
//		CalcBox();
//	}
//
//    CalcCntDen();
//
//    // pick start point
//    startpnt = mphead;
//    circount = 0;
//    while(startpnt != 0)
//    {
//    //calculate the max Curvature at this point
//	CalcPriCur(startpnt);
//
//	maxcur = fabs(startpnt->GetMaxCur());
//        if (maxcur > MAX_TOL)
//        {
//            circount ++;
//            if (circount > 100)
//                return false;
//            // get a belt type data and fit a cylinder
//            startpnt->GetMinDir(nx, ny, nz);
//            x = startpnt->GetX();
//            y = startpnt->GetY();
//            z = startpnt->GetZ();
//            d = -x*nx - y*ny - z*nz;
//            beltcld1 = 0;
//            for (int i=1; i<=minnum/2; i++)
//            {
//                width = i * mCntDen;
//                beltcld1 = ExtractBtwTwoPlns(nx, ny, nz, d, d+width);
//                if (beltcld1 == 0)
//                    return false;
//                beltcld2 = ExtractBtwTwoPlns(nx, ny, nz, d, d-width);
//                if (beltcld2 == 0)
//                {
//                    delete beltcld1;
//                    return false;
//                }
//                lastnum1 = beltcld1->GetNumber();
//                lastnum2 = beltcld2->GetNumber();
//                beltcld1->AddCloud(beltcld2);
//                delete beltcld2;
//                beltcld1->AddPoint(startpnt->CopyData());
//                num = beltcld1->GetNumber();
//                if (i>1 && num>lastnum && num>=minnum)
//                    break;
//                lastnum = num;
//
//				//hide the following two lines. // can't hide, or leads to potential memory leaks! by Xiaobo
//                delete beltcld1;
//                beltcld1 = 0;
//            }
//
//            // there are not enough points to fit a cylinder,
//            // pick another start point
//            if (beltcld1 == 0)
//            {
//                startpnt = startpnt->GetNext();
//                continue;
//            }
//
//            // check for marching over in positive or negative direction
//            posover = false;
//            negover = false;
//            if (lastnum1 == 0)
//                negover = true;
//            if (lastnum2 == 0)
//                posover = true;
//
//            // fit a cylinder to the picked belt type data
//            startcy = new QuCylinder;
//            if (startcy == 0)
//            {
//                delete beltcld1;
//                return false;
//            }
//
//            // calculate initial value of cylinder fitting
//            // with the principal curvatures and directions
//            // if error, pick another start point
//            if (!CalcFitInits(startcy, startpnt))
//            {
//                delete beltcld1;
//                delete startcy;
//                startpnt = startpnt->GetNext();
//                continue;
//            }
//
//            // fit a cylinder
//            // if error, fit again
//            if (!beltcld1->FitCylinder(startcy, maxerr, avgerr))
//            {
//                // calculate initial value of cylinder fitting
//                // if error, pick another start point
//                if (!CalcFitInits(startcy))
//                {
//                    delete beltcld1;
//                    delete startcy;
//                    startpnt = startpnt->GetNext();
//                    continue;
//                }
//                // fit a cylinder again
//                // if error, pick another start point
//                if (!beltcld1->FitCylinder(startcy, maxerr, avgerr))
//                {
//                    delete beltcld1;
//                    delete startcy;
//                    startpnt = startpnt->GetNext();
//                    continue;
//                }
//            }
//
//            delete beltcld1;
//
//            // compare the fitted cylinder
//            // with principal curvatures and directions of start point
//            // if the difference is too large, pick another start point
//            startcy->GetAxis(compnx, compny, compnz);
//            if (startcy->GetRad() > errrad/maxcur ||
//                startcy->GetRad() < 1.0/maxcur/errrad ||
//                fabs(compnx*nx+compny*ny+compnz*nz) < cos(PI*errang/180.0))
//            {
//                delete startcy;
//                startpnt = startpnt->GetNext();
//                continue;
//            }
//            break;
//        }
//        startpnt = startpnt->GetNext();
//    }//end while
//
//    // can not pick start point
//    if (startpnt == 0)
//        return false;
//
//    // get the start value for marching
//    toppnt = startcy->GetTopPnt();
//    botpnt = startcy->GetBotPnt();
//    startx = (toppnt->GetX()+botpnt->GetX()) / 2.0;
//    starty = (toppnt->GetY()+botpnt->GetY()) / 2.0;
//    startz = (toppnt->GetZ()+botpnt->GetZ()) / 2.0;
//    startcy->GetAxis(startnx, startny, startnz);
//
//    // keep down the parameter of blend at start point
//    arc = new CircleArc;
//    if (arc == 0)
//    {
//        delete startcy;
//        return false;
//    }
//    arc->mCntX = startx;
//    arc->mCntY = starty;
//    arc->mCntZ = startz;
//    arc->mCntNx = startnx;
//    arc->mCntNy = startny;
//    arc->mCntNz = startnz;
//    arc->mRad = startcy->GetRad();
//    pBld->AddHead(arc);
//
//
//    //////////////////////// march along positive direction
//    marchnx = startnx;
//    marchny = startny;
//    marchnz = startnz;
//    x = startx;
//    y = starty;
//    z = startz;
//    startcy->GetFitInits(ro, phi, theta, k, alpha);
//
////    endctrl = false;
//    while (!posover)
//    {
//        // get a belt type data and fit a cylinder
//        lastx = x;
//        lasty = y;
//        lastz = z;
//        x = lastx + step*mCntDen*marchnx;
//        y = lasty + step*mCntDen*marchny;
//        z = lastz + step*mCntDen*marchnz;
//        nx = marchnx;
//        ny = marchny;
//        nz = marchnz;
//        d = -x*nx - y*ny - z*nz;
//        beltcld1 = 0;
//        for (int i=1; i<=minnum/2; i++)
//        {
//            width = i * mCntDen;
//            beltcld1 = ExtractBtwTwoPlns(nx, ny, nz, d, d+width);
//            if (beltcld1 == 0)
//            {
//                delete startcy;
//                return false;
//            }
//            beltcld2 = ExtractBtwTwoPlns(nx, ny, nz, d, d-width);
//            if (beltcld2 == 0)
//            {
//                delete beltcld1;
//                delete startcy;
//                return false;
//            }
//            lastnum2 = beltcld2->GetNumber();
//            beltcld1->AddCloud(beltcld2);
//            delete beltcld2;
//            num = beltcld1->GetNumber();
//            if (i>1 && num>lastnum && num>=minnum)
//                break;
//            lastnum = num;
//            delete beltcld1;
//           beltcld1 = 0;
//        }
//
//        // check for marching over in positive direction
//        if (lastnum2 == 0)
//            posover = true;
//
//        // there are not enough points to fit a cylinder,
//        // continue marching
//        if (beltcld1 == 0)
//            continue;
//
//        // fit a cylinder to the picked belt type data
//        cylinder = new QuCylinder;
//        if (cylinder == 0)
//        {
//            delete beltcld1;
//            delete startcy;
//            return false;
//        }
//
//        // set initial value of cylinder fitting
//        // with the previous fitted cylinder
//        cylinder->SetFitInits(ro, phi, theta, k, alpha);
//
//        // fit a cylinder
//        // if error, fit again
//        if (!beltcld1->FitCylinder(cylinder, maxerr, avgerr))
//        {
//            // calculate initial value of cylinder fitting
//            // if error, continue marching
//            if (!CalcFitInits(cylinder))
//            {
//                delete beltcld1;
//                delete cylinder;
//                continue;
//            }
//            // fit a cylinder again
//            // if error, continue marching
//            if (!beltcld1->FitCylinder(cylinder, maxerr, avgerr))
//            {
//                delete beltcld1;
//                delete cylinder;
//                continue;
//            }
//        }
//
//
//        // calculate new marching value
//        cylinder->GetAxis(nx, ny, nz);
//        if (nx*marchnx+ny*marchny+nz*marchnz < 0.0)
//        {
//            marchnx = -nx;
//            marchny = -ny;
//            marchnz = -nz;
//        }
//        else
//        {
//            marchnx = nx;
//            marchny = ny;
//            marchnz = nz;
//        }
//        toppnt = cylinder->GetTopPnt();
//        botpnt = cylinder->GetBotPnt();
//        x = (toppnt->GetX()+botpnt->GetX()) / 2.0;
//        y = (toppnt->GetY()+botpnt->GetY()) / 2.0;
//        z = (toppnt->GetZ()+botpnt->GetZ()) / 2.0;
//        cylinder->GetFitInits(ro, phi, theta, k, alpha);
//
//        // check the new marching value
//        // if error, set original marching value and continue marching
//        if (((x-lastx)*marchnx<0.0 && (y-lasty)*marchny<0.0 &&
//             (z-lastz)*marchnz<0.0 ) ||
//             sqrt((x-lastx)*(x-lastx) + (y-lasty)*(y-lasty) +
//                 (z-lastz)*(z-lastz)) < step*mCntDen/10.0)
//        {
//            x = lastx + step*mCntDen*marchnx;
//            y = lasty + step*mCntDen*marchny;
//            z = lastz + step*mCntDen*marchnz;
//            delete beltcld1;
//            delete cylinder;
//            continue;
//        }
//
//        // keep down the parameter of blend at start point
//        arc = new CircleArc;
//        if (arc == 0)
//        {
//            delete beltcld1;
//            delete cylinder;
//            delete startcy;
//            return false;
//        }
//        arc->mCntX = x;
//        arc->mCntY = y;
//        arc->mCntZ = z;
//        arc->mCntNx = marchnx;
//        arc->mCntNy = marchny;
//        arc->mCntNz = marchnz;
//        arc->mRad = cylinder->GetRad();
//        pBld->AddHead(arc);
//
//        // check for marching process over
///*        if (beltcld1->IsContainPnt(startpnt))
//        {
//            if (endctrl)
//            {
//                delete beltcld1;
//                delete cylinder;
//                delete startcy;
//                return true;
//            }
//        }
//        else
//            endctrl = true;
//*/
//        delete cylinder;
//        delete beltcld1;
//    }
//
//
//    //////////////////////// march along negative direction
//    marchnx = -startnx;
//    marchny = -startny;
//    marchnz = -startnz;
//    x = startx;
//    y = starty;
//    z = startz;
//    startcy->GetFitInits(ro, phi, theta, k, alpha);
//
////    endctrl = false;
//    while (!negover)
//    {
//        // get a belt type data and fit a cylinder
//        lastx = x;
//        lasty = y;
//        lastz = z;
//        x = lastx + step*mCntDen*marchnx;
//        y = lasty + step*mCntDen*marchny;
//        z = lastz + step*mCntDen*marchnz;
//        nx = marchnx;
//        ny = marchny;
//        nz = marchnz;
//        d = -x*nx - y*ny - z*nz;
//        beltcld1 = 0;
//        for (int i=1; i<=minnum/2; i++)
//        {
//            width = i * mCntDen;
//            beltcld1 = ExtractBtwTwoPlns(nx, ny, nz, d, d+width);
//            if (beltcld1 == 0)
//            {
//                delete startcy;
//                return false;
//            }
//            beltcld2 = ExtractBtwTwoPlns(nx, ny, nz, d, d-width);
//            if (beltcld2 == 0)
//            {
//                delete beltcld1;
//                delete startcy;
//                return false;
//            }
//            lastnum2 = beltcld2->GetNumber();
//            beltcld1->AddCloud(beltcld2);
//            delete beltcld2;
//            num = beltcld1->GetNumber();
//            if (i>1 && num>lastnum && num>=minnum)
//                break;
//            lastnum = num;
//           delete beltcld1;
//            beltcld1 = 0;
//        }
//
//        // check for marching over in negative direction
//        if (lastnum2 == 0)
//            negover = true;
//
//        // there are not enough points to fit a cylinder,
//        // continue marching
//        if (beltcld1 == 0)
//            continue;
//
//        // fit a cylinder to the picked belt type data
//        cylinder = new QuCylinder;
//        if (cylinder == 0)
//        {
//            delete beltcld1;
//            delete startcy;
//            return false;
//        }
//
//        // set initial value of cylinder fitting
//        // with the previous fitted cylinder
//        cylinder->SetFitInits(ro, phi, theta, k, alpha);
//
//        // fit a cylinder
//        // if error, fit again
//        if (!beltcld1->FitCylinder(cylinder, maxerr, avgerr))
//        {
//            // calculate initial value of cylinder fitting
//            // if error, continue marching
//            if (!CalcFitInits(cylinder))
//            {
//                delete beltcld1;
//                delete cylinder;
//                continue;
//            }
//            // fit a cylinder again
//            // if error, continue marching
//            if (!beltcld1->FitCylinder(cylinder, maxerr, avgerr))
//            {
//                delete beltcld1;
//                delete cylinder;
//                continue;
//            }
//        }
//
//        // calculate new marching value
//        cylinder->GetAxis(nx, ny, nz);
//        if (nx*marchnx+ny*marchny+nz*marchnz < 0.0)
//        {
//            marchnx = -nx;
//            marchny = -ny;
//            marchnz = -nz;
//        }
//        else
//        {
//            marchnx = nx;
//            marchny = ny;
//            marchnz = nz;
//        }
//        toppnt = cylinder->GetTopPnt();
//        botpnt = cylinder->GetBotPnt();
//        x = (toppnt->GetX()+botpnt->GetX()) / 2.0;
//        y = (toppnt->GetY()+botpnt->GetY()) / 2.0;
//        z = (toppnt->GetZ()+botpnt->GetZ()) / 2.0;
//        cylinder->GetFitInits(ro, phi, theta, k, alpha);
//
//
//        // check the new marching value
//        // if error, set original marching value and continue marching
//        if (((x-lastx)*marchnx<0.0 && (y-lasty)*marchny<0.0 &&
//             (z-lastz)*marchnz<0.0 ) ||
//             sqrt((x-lastx)*(x-lastx) + (y-lasty)*(y-lasty) +
//                 (z-lastz)*(z-lastz)) < step*mCntDen/10.0)
//        {
//            x = lastx + step*mCntDen*marchnx;
//            y = lasty + step*mCntDen*marchny;
//            z = lastz + step*mCntDen*marchnz;
//            delete beltcld1;
//            delete cylinder;
//            continue;
//        }
//
//        // keep down the parameter of blend at start point
//        arc = new CircleArc;
//        if (arc == 0)
//        {
//            delete beltcld1;
//            delete startcy;
//            delete cylinder;
//            return false;
//        }
//        arc->mCntX = x;
//        arc->mCntY = y;
//        arc->mCntZ = z;
//        arc->mCntNx = -marchnx;
//        arc->mCntNy = -marchny;
//        arc->mCntNz = -marchnz;
//        arc->mRad = cylinder->GetRad();
//        pBld->AddEnd(arc);
//
//        // check for marching process over
///*        if (beltcld1->IsContainPnt(startpnt))
//        {
//            if (endctrl)
//            {
//                delete beltcld1;
//                delete startcy;
//                delete cylinder;
//                return true;
//            }
//        }
//        else
//            endctrl = true;
//*/
//        delete beltcld1;
//        delete cylinder;
//    }
//
//    delete startcy;
//    pBldtmp=pBld;
//    return true;
//}
//
//
//
extern bool jcb (double *a, double *v);
///////////////////////////////////////////////////////////////////////////////
// fit a plane to point cloud
// pPln: output
//       pointer of fitted plane
// maxErr: output
//         maximum fitting error
// avgErr: output
//         average fitting error
// RETURN: true: success   false: failure
///////////////////////////////////////////////////////////////////////////////

bool 
Ti_cld_point::FitPlane(QuPlane* pPln, double& maxErr, double& avgErr)
{
	double      val[4];
	double      len, dis, evmin;
    Ti_point*   cirpnt;


	if (mphead == 0)
        return false;
    if (pPln == 0)
        return false;

    // set matrix value in eigenvectors and eigenvalues calculation
	Ti_Matrix<double>  matrix0(3,3), U(3,3);
	int index = 0;
 	PNT pnt, centerpnt(0,0,0), norm_dir;
   cirpnt = mphead;
    while (cirpnt != 0)
    {
		pnt = PNT(cirpnt->GetX(), cirpnt->GetY(),cirpnt->GetZ());
		centerpnt+= pnt;
		index ++;
        cirpnt = cirpnt->GetNext();
    }
   centerpnt /= index;

	 cirpnt = mphead;
    while (cirpnt != 0)
    {
		pnt = PNT(cirpnt->GetX(), cirpnt->GetY(),cirpnt->GetZ());
		pnt -= centerpnt;
		matrix0.m[0][0] += pnt[0]*pnt[0];
		matrix0.m[0][1] += pnt[0]*pnt[1];
		matrix0.m[0][2] += pnt[0]*pnt[2];
		matrix0.m[1][0] += pnt[1]*pnt[0];
		matrix0.m[1][1] += pnt[1]*pnt[1];
		matrix0.m[1][2] += pnt[1]*pnt[2];
		matrix0.m[2][0] += pnt[2]*pnt[0];
		matrix0.m[2][1] += pnt[2]*pnt[1];
		matrix0.m[2][2] += pnt[2]*pnt[2];
        cirpnt = cirpnt->GetNext();
    }

	matrix0.JacobiN(3, val, U);

	norm_dir[0] = U.m[0][2];
	norm_dir[1] = U.m[1][2];
	norm_dir[2] = U.m[2][2];
	norm_dir.Normalize();

	matrix0.Destroy();

	U.Destroy();
	//return sqrt(w[2] / num);

	

    // get the minimum eigenvalue and its eigenvector
 //   evmin = fabs(val[2]);
 //   imin = 0;
	//for (i=1; i<4; i++)
 //   {
	//	if (fabs(matri2[i*4+i]) < evmin)
	//	{
	//		evmin = fabs(matri2[i*4+i]);
	//		imin = i;
	//	}
 //   }

    // set the parameters of fitted plane
	len = -norm_dir*centerpnt;
    pPln->SetCoef(norm_dir[0], norm_dir[1], norm_dir[2], len);

    //calculate the maximum and average error
	maxErr = 0.0;
    avgErr = 0.0;
    cirpnt = mphead;
    while (cirpnt != 0)
    {
        dis = cirpnt->DistToPln(norm_dir[0], norm_dir[1], norm_dir[2], len);
	    avgErr += dis;
	    if(dis > maxErr)
		    maxErr = dis;
        cirpnt = cirpnt->GetNext();
	}
    avgErr /= (double)mnum;
	return true;
}



///////////////////////////////////////////////////////////////////////////////
// set fitting initials for cylinder
// pCy: output
//      pointer of cylinder to be fitted
// pInitPnt: input
//           pointer of point for calculation fitting initials
// RETURN: true: success   false: failure
///////////////////////////////////////////////////////////////////////////////

bool
Ti_cld_point::CalcFitInits(QuCylinder* pCy, Ti_point* pInitPnt)
{
    int             halfnum;
    Ti_point*       cirpnt, * cntpnt;
    double          ax, ay, az;
    double          nx, ny, nz;
    double          ro, phi, theta, k, alpha;
    Ti_point*       origpnt;
    Ti_point*       axispnt1, * axispnt2;
    double          len, t;


    if (pCy == 0)
        return false;

    if (pInitPnt == 0)
    {
        // sort the points
        if (!mIsSorted)
        {
            if (!SortXyz())
                return false;
        }

        // pick a center point
        halfnum = (int) (mnum / 2);
        if (halfnum == 0)
            return false;
        cirpnt = mphead;
        for (int i=0; i<halfnum; i++)
        {
            if (cirpnt == 0)
                break;
            cirpnt = cirpnt->GetNext();
        }
        if (cirpnt == 0)
            return false;
        cirpnt->SetMaxCur(0.0);
    }
    else
        cirpnt = pInitPnt;

    // calculate normal
    if (!CalcNormal(cirpnt, nx, ny, nz))
        return false;
    cirpnt->GetMinDir(ax, ay, az);

    // get maximum curvature
    k = fabs(cirpnt->GetMaxCur());
    if (k < MAX_TOL)
        return false;

    // create center point
    cntpnt = cirpnt->CopyData();
    if (cntpnt == 0)
        return false;
    cntpnt->Shift(-nx/k, -ny/k, -nz/k);

    axispnt1 = cntpnt->CopyData();
    if (axispnt1 == 0)
    {
        delete cntpnt; 
        return false;
    }
    axispnt2 = cntpnt->CopyData();
    if (axispnt2 == 0)
    {
        delete cntpnt;
        delete axispnt1;
        return false;
    }
    axispnt2->Shift(ax, ay, az);

    origpnt = new Ti_point(0.0, 0.0, 0.0);
    if (origpnt == 0)
    {
        delete cntpnt;
        delete axispnt1;
        delete axispnt2;
        return false;
    }
    origpnt->PrjToLine(axispnt1, axispnt2, t);

    len = sqrt((origpnt->GetX()*origpnt->GetX()) +
               (origpnt->GetY()*origpnt->GetY()) +
               (origpnt->GetZ()*origpnt->GetZ()));
    if (len > MAX_TOL)
    {
        nx = origpnt->GetX() / len;
        ny = origpnt->GetY() / len;
        nz = origpnt->GetZ() / len;
    }
    delete cntpnt;
    delete origpnt;
    delete axispnt1;
    delete axispnt2;

    ro = len-1/k;
    len = sqrt(nx * nx + ny * ny);
    phi = acos(nx / len);
    if (ny < 0.0)
        phi = -phi;
    theta = acos(nz);
    alpha = acos((ax*nx*nz+ay*ny*nz)/len - az*len);
	if (ay*nx-ax*ny < 0.0)
		alpha = -alpha;
    pCy->SetFitInits(ro, phi, theta, k, alpha);
    return true;
}



extern bool Is_convergent(int n, double delta1, double delta2,
                          double *x1,double *x2);

///////////////////////////////////////////////////////////////////////////////
// fit a cylinder to point cloud
// pCy: output
//      pointer of fitted cylinder
// maxErr: output
//         maximum fitting error
// avgErr: output
//         average fitting error
// RETURN: true: success   false: failure
///////////////////////////////////////////////////////////////////////////////
//bool 
//Ti_cld_point::FitCylinder(QuCylinder* pCy, double& maxErr, double& avgErr)
//{
//    double      ro, phi, theta, k, alpha;
//    double      u, v;
//    double      x1[5], x2[5], xx[6];
//    double      *a, f[5], aa[25];
//    double      sigma   ; 
//    int         mItr1, mItr2;
//    double      delta1, delta2;
//    double      tem_n, tem_nph, tem_nth, tem_a, tem_aph, tem_ath, tem_aal;
//	Svd         svdfit(5,5);
//    double     svdm[31],  svdw[6],  svdb[6];
//    Ti_point*   cirpnt, * toppnt, * botpnt;
//	double      x, y, z;
//    double      bsx, bsy, bsz;
//    double      t, tmax, tmin;
//    double      dis;
//    double      rad;
//    int         index;
//
//
//    if (pCy == 0)
//        return false;
//	if (mphead == 0)
//        return false;
//
//    // get fitting initials
//    pCy->GetFitInits(ro, phi, theta, k, alpha);
//
//	u = 0.01;
//    v = 5.0;
//    x2[0] = ro;
//    x2[1] = phi;
//    x2[2] = theta;
//    x2[3] = k;
//	x2[4] = alpha;
//	
//	   
//    sigma = 0.0;
//    mItr1 = 0;
//	do
//	{
//		u = u / (v*v);
//		for(int i=0; i<5; i++)
//			x1[i] = x2[i];
//		mItr1 ++;
//		if (mItr1 > 200)
//		    return false;
//		mItr2 = 0;
//	    do
//	    {
//		    u = u * v;
//		    delta1 = 0.0;
//		    delta2 = 0.0;
//    		mItr2 ++;
//		    if (mItr2 > 200)
//		        return false;
//		
//		    for(i=0; i<5; i++)
//			    f[i] = 0.0;
//
//	        a = new double[mnum*5];
//            if (a == 0)
//               return false;
//
//            // set maxtrix value in SVD calculation
//            index = 0;
//	        cirpnt = mphead;
//	        while (cirpnt != 0)	
//	        {
//		        x = cirpnt->GetX();
//		        y = cirpnt->GetY();
//		        z = cirpnt->GetZ();
//                tem_n = x*cos(x1[1])*sin(x1[2]) + y*sin(x1[1])*sin(x1[2]) +
//                        z*cos(x1[2]);
//		        tem_nph = -x*sin(x1[1])*sin(x1[2]) + y*cos(x1[1])*sin(x1[2]);
//		        tem_nth = x*cos(x1[1])*cos(x1[2]) + y*sin(x1[1])*cos(x1[2]) -
//                          z*sin(x1[2]);
//		        tem_a = x*(cos(x1[1])*cos(x1[2])*cos(x1[4]) -
//                        sin(x1[1])*sin(x1[4])) +
//			            y*(sin(x1[1])*cos(x1[2])*cos(x1[4]) +
//                        cos(x1[1])*sin(x1[4])) -
//			            z*sin(x1[2])*cos(x1[4]);
//		        tem_aph = x*(-sin(x1[1])*cos(x1[2])*cos(x1[4]) -
//                          cos(x1[1])*sin(x1[4])) +
//			              y*(cos(x1[1])*cos(x1[2])*cos(x1[4]) -
//                          sin(x1[1])*sin(x1[4]));
//		        tem_ath = - x*cos(x1[1])*sin(x1[2])*cos(x1[4]) -
//			                y*sin(x1[1])*sin(x1[2])*cos(x1[4]) -
//			                z*cos(x1[2])*cos(x1[4]);
//		        tem_aal = - x*(cos(x1[1])*cos(x1[2])*sin(x1[4]) +
//                            sin(x1[1])*cos(x1[4])) +
//			                y*(-sin(x1[1])*cos(x1[2])*sin(x1[4]) +
//                            cos(x1[1])*cos(x1[4])) +
//			                z*sin(x1[2])*sin(x1[4]);
//		        sigma = x1[3] * 0.5 * (x*x + y*y + z*z - 2.0*x1[0]*tem_n -
//                        tem_a*tem_a + x1[0]*x1[0]) + x1[0] - tem_n;
//		        delta1 += sigma*sigma;   
//		        a[index*5]   = -x1[3]*tem_n + x1[0]*x1[3] + 1.0;
//		        a[index*5+1] = -x1[3]*x1[0]*tem_nph -x1[3]*tem_a*tem_aph -
//                                tem_nph;
//		        a[index*5+2] = -x1[3]*x1[0]*tem_nth -x1[3]*tem_a*tem_ath -
//                                tem_nth;
//		        a[index*5+3] = 0.5 * (x*x + y*y + z*z - tem_a*tem_a +
//                               x1[0]*x1[0]) - x1[0]*tem_n;
//		        a[index*5+4] = -x1[3] * tem_a * tem_aal;
//		        f[0] += a[index*5] * sigma;
//		        f[1] += a[index*5+1] * sigma;
//		        f[2] += a[index*5+2] * sigma;
//		        f[3] += a[index*5+3] * sigma;
//		        f[4] += a[index*5+4] * sigma;
//		        index ++;
//		        cirpnt = cirpnt->GetNext();
//	        }                   
//	        for (i=0; i<5; i++)
//            {
//		        for (int j=0; j<5; j++)
//		        {
//			        int k = i * 5 + j;
//			        aa[k] = 0.0;
//			        for (int l=0; l<mnum; l++)
//				        aa[k] += a[l*5+i] * a[l*5+j];
//		        }
//            }
//	        for (i=0; i<5; i++)
//	        {
//		        aa[i*5+i] += u * aa[i*5+i];
//		        svdw[i+1] = 0.0;
//		        svdb[i+1] = -f[i];
//	        }
//	        for (i=0; i<25; i++)
//		        svdm[i+6] = aa[i];
//
//			svdfit.SetSvdM(svdm);
//			svdfit.SetSvdW(svdw);
//			svdfit.SetSvdB(svdb);
//
//            delete[] a;
//
//
//            // SVD calculation
//            if (!svdfit.Is_equation_ok(5, 5, xx))
//	            return false;
//	
//	        for (i=0; i<5; i++)
//		        x2[i] = x1[i] + xx[i+1];
//
//	        if ((fabs(x2[1])>2*PI) || (fabs(x2[2])>2*PI))
//                return false;
//
//	        cirpnt = mphead;
//	        while (cirpnt != 0)	
//	        {
//		        x = cirpnt->GetX();
//		        y = cirpnt->GetY();
//		        z = cirpnt->GetZ();
//		        tem_n = x*cos(x2[1])*sin(x2[2]) + y*sin(x2[1])*sin(x2[2]) +
//                        z*cos(x2[2]);
//		        tem_a = x*(cos(x2[1])*cos(x2[2])*cos(x2[4]) -
//                        sin(x2[1])*sin(x2[4])) +
//			            y*(sin(x2[1])*cos(x2[2])*cos(x2[4]) +
//                        cos(x2[1])*sin(x2[4])) -
//			            z*sin(x2[2])*cos(x2[4]);
//		        sigma = x2[3] * 0.5 * (x*x+y*y+z*z - 2.0*x2[0]*tem_n -
//                        tem_a*tem_a + x2[0]*x2[0]) + x2[0] - tem_n;
//		        delta2 += sigma * sigma;  
//                cirpnt = cirpnt->GetNext();
//            }
//		
//	    }while (delta2 > delta1);
//	
//	}while(!Is_convergent(5, delta1, delta2, x1, x2));
//
//
//	/*/
//	SVD: singular value decomposition
//
//	The algorithm looks right until here.
//	Need to verify the following.
//
//			Zhongguo Li
//			5.6.2003
//	*/
//    // set the parameters of fitted cylinder
//    toppnt = new Ti_point;
//	if (toppnt == 0)
//        return false;
//	botpnt = new Ti_point;
//	if (botpnt == 0)
//    {
//		delete toppnt;
//        return false;
//    }
//    
//	tmax = -999999.0,
//	tmin = 999999.0;
//	bsx = (x2[0] + 1.0/x2[3]) * cos(x2[1]) * sin(x2[2]);
//	bsy = (x2[0] + 1.0/x2[3]) * sin(x2[1]) * sin(x2[2]);
//	bsz = (x2[0] + 1.0/x2[3]) * cos(x2[2]);
//	cirpnt = mphead;
//	while (cirpnt != 0)	
//	{
//		x = cirpnt->GetX();
//		y = cirpnt->GetY();
//		z = cirpnt->GetZ();
//		t = x*(cos(x2[1])*cos(x2[2])*cos(x2[4]) - sin(x2[1])*sin(x2[4])) +
//			y*(sin(x2[1])*cos(x2[2])*cos(x2[4]) + cos(x2[1])*sin(x2[4])) -
//			z*sin(x2[2])*cos(x2[4]);
//		if (t > tmax)
//		{
//			toppnt->SetXyz(bsx + t*(cos(x2[1])*cos(x2[2])*cos(x2[4]) -
//                           sin(x2[1])*sin(x2[4])),
//				           bsy + t*(sin(x2[1])*cos(x2[2])*cos(x2[4]) +
//                           cos(x2[1])*sin(x2[4])),
//						   bsz - t*sin(x2[2])*cos(x2[4]));
//			tmax = t;
//		}
//		if (t < tmin)
//		{
//			botpnt->SetXyz(bsx + t*(cos(x2[1])*cos(x2[2])*cos(x2[4]) -
//                           sin(x2[1])*sin(x2[4])),
//				           bsy + t*(sin(x2[1])*cos(x2[2])*cos(x2[4]) +
//                           cos(x2[1])*sin(x2[4])),
//						   bsz - t*sin(x2[2])*cos(x2[4]));
//			tmin = t;
//		}
//		cirpnt = cirpnt->GetNext();
//	}
//    rad = fabs(1.0 / x2[3]);
//    pCy->SetCoef(toppnt , botpnt , rad);
//    pCy->SetFitInits(x2[0], x2[1], x2[2], x2[3], x2[4]);
//
//    //calculate the maximum and average error
//	maxErr = 0.0;
//    avgErr = 0.0;
//	cirpnt = mphead;
//	while(cirpnt != 0)
//	{
//		dis = fabs(rad - cirpnt->DistToLine(toppnt, botpnt));
//		avgErr += dis;
//	    if(dis > maxErr)
//		    maxErr = dis;
//		cirpnt = cirpnt->GetNext();
//	}
//    avgErr /= (double)mnum;
//
//    return true;
//}
//
//
//
//
///////////////////////////////////////////////////////////////////////////////
// fit a parabolic surface to point cloud
// pCntPnt: input
//          pointer of center point
// pParaSur: output
//           pointer of fitted parabolic surface
// maxErr: output
//         maximum fitting error
// avgErr: output
//         average fitting error
// RETURN: true: success   false: failure
///////////////////////////////////////////////////////////////////////////////

bool 
Ti_cld_point::FitParaSur(Ti_point* pCntPnt, QuParaSur*  pParaSur,
			 double& maxErr, double& avgErr)
	{
	double          a, b, c, d;    
	Svd             svdfit(3,3);    
	QuPlane*        pln = NULL;
	LocalCs*        lcs = NULL;
	Ti_cld_point*   transcld = NULL;
	double		 svdm[13],
				 svdw[4],
				 svdb[4];
	Ti_point*       cirpnt = NULL;
	double          x, y, z;
	double          dis, maxdis;
	double          val[4];
	double          weight;


	if (mphead == 0)
		return false;
	if (pCntPnt == 0)
		return false;
	if (pParaSur == 0)
		return false;

	// fit a plane to the point cloud
	pln = new QuPlane;
	if (pln == 0)
		return false;
	if (!FitPlane(pln, maxErr, avgErr))
		{
		delete pln;
		pln = NULL;
		return false;
		}
	pln->GetCoef(a, b, c, d);
	delete pln;
	pln = NULL;
	// create local coordinate system
	lcs = new LocalCs;
	if (lcs == 0)
		return false;
	lcs->SetOrigin(pCntPnt->GetX(), pCntPnt->GetY(), pCntPnt->GetZ());
	if (!lcs->SetAxisZ(a, b, c))
		{
		delete lcs;
		lcs = NULL;
		return false;
		}

	// transform the points
	transcld = CopyData();
	if (transcld ==0)
		{
		delete lcs;
		lcs = NULL;
		return false;
		}
	cirpnt = transcld->GetHead();
	while(cirpnt != 0)
		{
		cirpnt->TransToLocalCs(lcs);
		cirpnt = cirpnt->GetNext();
		}
	transcld->CalcBox();

	// fit a parabolic surface to the transformed point cloud	
	// set maxtrix value in SVD calculation
	for(int i=0; i<13; i++)
		svdm[i] = 0.0;
	for(i=0; i<4; i++)
		{
		svdw[i] = 0.0;
		svdb[i] = 0.0;
		}

	maxdis = 0;
	cirpnt = transcld->GetHead();
	while(cirpnt != 0)
		{
		x = cirpnt->GetX();
		y = cirpnt->GetY();
		z = cirpnt->GetZ();
		dis = x*x + y*y + z*z;
		if (dis > maxdis)
			maxdis = dis;
		cirpnt = cirpnt->GetNext();
		}

	cirpnt = transcld->GetHead();
	while(cirpnt != 0)
		{
		x = cirpnt->GetX();
		y = cirpnt->GetY();
		z = cirpnt->GetZ();
		dis = x*x + y*y + z*z;
		weight = 100 - 99*dis/maxdis;
		svdm[4] += weight*x*x*x*x;
		svdm[5] += weight*x*x*x*y;
		svdm[6] += weight*x*x*y*y;
		svdm[7] += weight*y*x*x*x;
		svdm[8] += weight*y*x*x*y;
		svdm[9] += weight*y*x*y*y;
		svdm[10] += weight*y*y*x*x;
		svdm[11] += weight*y*y*x*y;
		svdm[12] += weight*y*y*y*y;
		svdb[1] += weight*x*x*z;
		svdb[2] += weight*y*x*z;
		svdb[3] += weight*y*y*z;
		cirpnt = cirpnt->GetNext();
		}

	// SVD calculation
		svdfit.SetSvdM(svdm);	
		svdfit.SetSvdW(svdw);
		svdfit.SetSvdB(svdb);
	if (!svdfit.Is_equation_ok(3, 3, val))
		{
		delete lcs;
		lcs = NULL;
		delete transcld;
		transcld = NULL;
		return false;
		}

	// set the parameters of fitted sphere
	pParaSur->SetLocalCs(lcs);
	if (!pParaSur->SetCoef(val[1], val[2], val[3]))
		{
		delete transcld;// the lcs has been added to the cloud
		transcld = NULL;
		return false;
		}

	//calculate the maximum and average error
	maxErr = 0.0;
	avgErr = 0.0;
	cirpnt = transcld->GetHead();
	while(cirpnt != 0)
		{   
		x = cirpnt->GetX();
		y = cirpnt->GetY();
		z = cirpnt->GetZ();
		dis = fabs(z - val[1]*x*x - val[2]*y*x - val[3]*y*y);
		avgErr += dis;
		if (maxErr < dis)
			maxErr = dis;
		cirpnt = cirpnt->GetNext();
		}
	avgErr /=(double)transcld->GetNumber();
	if(transcld != NULL)
		{
		delete transcld;
		transcld = NULL;
		}
	//if(pln!= NULL)
	//	{
	//	delete pln;
	//	pln = NULL;
	//	}
	if(lcs!= NULL)
		{
		delete lcs;
		lcs = NULL;
		}
	return true;
	}



///////////////////////////////////////////////////////////////////////////////
// calculate principal curvatures and directions
// pCntPnt: input
//          pointer of center point
// RETURN: true: success   false: failure
///////////////////////////////////////////////////////////////////////////////

bool
Ti_cld_point::CalcPriCur(Ti_point* pCntPnt)
	{
	Ti_point**      nearpnts	= NULL;
	Ti_point*       nearpnt		= NULL;
	Ti_cld_point*   nearcld	= NULL;
	QuParaSur*      parasur	= NULL;
	int			nearnum;
	double          maxerr,avgerr;
	double          mincur, mindirx, mindiry, mindirz;
	double          maxcur, maxdirx, maxdiry, maxdirz;

	// fit a parabolic surface to a point cloud
	if (mnum > 32)
		{
		// get a neighborhood point cloud with 24~32 points
		nearpnts = FindNearPnts(pCntPnt, 23, 31, nearnum);
		if (nearnum == 0)
			return false;
		nearcld = new Ti_cld_point;
		if (nearcld == 0)
			{
			free(nearpnts);
			nearpnts = NULL;
			return false;
			}
		for (int i=0; i<nearnum; i++)
			{
			nearpnt = nearpnts[i]->CopyData();
			if (nearpnt == 0)
				{
				free(nearpnts);
				delete nearcld;
				return false;
				}
			nearcld->AddPoint(nearpnt);
			}
		if(nearpnts != NULL)
			{
			free((void*)nearpnts);
			nearpnts = NULL;
			}
		nearpnt = pCntPnt->CopyData();
		if (nearpnt == 0)
			{
			delete nearcld;
			nearcld = NULL;
			return false;
			}
		nearcld->AddPoint(nearpnt);

		// fit a parabolic surface to the neighborhood point cloud
		parasur = new QuParaSur;
		if (parasur == 0)
			{
			delete nearcld;
			nearcld = NULL;
			return false;
			}
		if (!nearcld->FitParaSur(pCntPnt, parasur, maxerr, avgerr))
			{
			delete parasur;
			delete nearcld;
			parasur = NULL;
			nearcld = NULL;
			return false;
			}
		mincur = parasur->GetMinCur();
		maxcur = parasur->GetMaxCur();
		parasur->GetMinDir(mindirx, mindiry, mindirz);
		parasur->GetMaxDir(maxdirx, maxdiry, maxdirz);
		delete parasur;
		parasur = NULL;
		nearcld->ClearCloud();
		delete nearcld;
		nearcld = NULL;
		}
	else
		{
		// fit a parabolic surface to the whole point cloud
		parasur = new QuParaSur;
		if (parasur == 0)
			return false;
		if (!FitParaSur(pCntPnt, parasur, maxerr, avgerr))
			{
			delete parasur;
			parasur = NULL;
			return false;
			}
		mincur = parasur->GetMinCur();
		maxcur = parasur->GetMaxCur();
		parasur->GetMinDir(mindirx, mindiry, mindirz);
		parasur->GetMaxDir(maxdirx, maxdiry, maxdirz);
		delete parasur;
		parasur = NULL;
		}

	// set principal curvatures and directions
	pCntPnt->SetMinCur(mincur);
	pCntPnt->SetMinDir(mindirx, mindiry, mindirz);
	pCntPnt->SetMaxCur(maxcur);
	pCntPnt->SetMaxDir(maxdirx, maxdiry, maxdirz);
	if( nearpnts != NULL)
		{
		free(nearpnts);
		nearpnts = NULL;
		}
	/*if( nearpnt != NULL)
		{
		delete nearpnt;
		nearpnt = NULL;
		}*/
	if( nearcld != NULL)
		{
		delete nearcld;
		nearcld = NULL;
		}
	if( parasur!= NULL)
		{
		delete parasur;
		parasur = NULL;
		}
	return true;
	}




///////////////////////////////////////////////////////////////////////////////
// calculate normal
// pCntPnt: input
//          pointer of center point
// rNx, rNy, rNz: output
//                coordinates of normal vector
// RETURN: true: success   false: failure
///////////////////////////////////////////////////////////////////////////////

bool
Ti_cld_point::CalcNormal(Ti_point* pCntPnt,
                         double& rNx, double& rNy, double& rNz)
{
    double          mindirx, mindiry, mindirz;
    double          maxcur, maxdirx, maxdiry, maxdirz;
    Ti_point*       cntpnt, * boxpnt;
    double          k, len;


    // get principal curvatures and directions
    maxcur = pCntPnt->GetMaxCur();
    k = fabs(maxcur);
    if (k<MAX_TOL)
    {
        if (!CalcPriCur(pCntPnt))
           return false;
    }
    maxcur = pCntPnt->GetMaxCur();
    k = fabs(maxcur);
    if (k<MAX_TOL)
        return false;

    // calculate normal vector
    pCntPnt->GetMaxDir(maxdirx, maxdiry, maxdirz);
    pCntPnt->GetMinDir(mindirx, mindiry, mindirz);
    rNx = mindiry * maxdirz - mindirz * maxdiry;
    rNy = mindirz * maxdirx - mindirx * maxdirz;
    rNz = mindirx * maxdiry - mindiry * maxdirx;

    // adjust the normal vector
    boxpnt = new Ti_point((minfpoint.maxx + minfpoint.minx) / 2.0,
                          (minfpoint.maxy + minfpoint.miny) / 2.0,
                          (minfpoint.maxz + minfpoint.minz) / 2.0);
    if (boxpnt == 0)
        return false;

    cntpnt = pCntPnt->CopyData();
    if (cntpnt == 0)
    {
        delete boxpnt;
        return false;
    }
    cntpnt->Shift(rNx/k, rNy/k, rNz/k);
    len = cntpnt->DistToPnt(boxpnt);
    cntpnt->Shift(-2.0*rNx/k, -2.0*rNy/k, -2.0*rNz/k);
    if (cntpnt->DistToPnt(boxpnt) > len)
    { 
        rNx = -rNx;
        rNy = -rNy;
        rNz = -rNz;
    }
    delete boxpnt;
    delete cntpnt;
    
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// get the number of points
// RETURN: number of points
///////////////////////////////////////////////////////////////////////////////

long 
Ti_cld_point::GetNumber()
{
    return mnum;
}



///////////////////////////////////////////////////////////////////////////////
// get the head point
// RETURN: pointer of head point
///////////////////////////////////////////////////////////////////////////////

Ti_point* 
Ti_cld_point::GetHead()
{
    return mphead;
}



///////////////////////////////////////////////////////////////////////////////
// get the end point
// RETURN: pointer of end point
///////////////////////////////////////////////////////////////////////////////

Ti_point* 
Ti_cld_point::GetEnd()
{
    return mpend;
}



///////////////////////////////////////////////////////////////////////////////
// get boundary box
// rMaxX, rMaxY, rMaxZ, rMinX, rMinY, rMinZ:
//          output
//          maximum and minimum values of coordinates x, y, z
///////////////////////////////////////////////////////////////////////////////

void 
Ti_cld_point::GetBox(double& rMaxX, double& rMaxY, double& rMaxZ,
                     double& rMinX, double& rMinY, double& rMinZ)
{
    rMaxX = minfpoint.maxx;
    rMaxY = minfpoint.maxy;
    rMaxZ = minfpoint.maxz;
    rMinX = minfpoint.minx;
    rMinY = minfpoint.miny;
    rMinZ = minfpoint.minz;
}



///////////////////////////////////////////////////////////////////////////////
// get density of points
// RETURN: density
///////////////////////////////////////////////////////////////////////////////

double 
Ti_cld_point::GetCntDen()
{
    return mCntDen;
}

//Blend* Ti_cld_point::GetpBld()
//{
//	return pBldtmp;
//}

///////////////////////////////////////////////////////////////////////////////
// inquire if the point cloud is sorted
// RETURN: true: sorted   false: not sorted
///////////////////////////////////////////////////////////////////////////////

bool 
Ti_cld_point::IsSorted()
{
    return mIsSorted;
}



///////////////////////////////////////////////////////////////////////////////
// inquire if point cloud contain a point
// RETURN: true: contain   false: not contain
///////////////////////////////////////////////////////////////////////////////

bool
Ti_cld_point::IsContainPnt(Ti_point* pPnt)
{
    Ti_point* cirpnt;


    if (pPnt == 0)
        return false;

    cirpnt = mphead;
    while(cirpnt != 0)
    {
        if (cirpnt->IsCoinPnt(pPnt))
            return true;
        cirpnt = cirpnt->GetNext();
    }
    return false;
}

////X////////X//////////////////////X//////////////////////////////////////////

bool Ti_cld_point::Sort_byDistance(PNT& aComprVec)
{
 
    Ti_point**  pntarray;
    Ti_point*   cirpnt;
    int         index;

    if (mnum <= 1)
        return false;

    pntarray = (Ti_point**) calloc((size_t)mnum, sizeof(int));

    // initialize the pointer array by the point data
    cirpnt = mphead;
    index = 0;
    while (cirpnt != NULL)
    {
        pntarray[index] = cirpnt;
        cirpnt = cirpnt->mpnext;
        index ++;
	}

	mComprVec = aComprVec;

	qsort((void*)pntarray, (size_t)mnum, sizeof(Ti_point*),
			CompareDis);

    // change the point data by sorted pointer array
    for (int i=0; i<mnum; i++)
    {
        cirpnt = (Ti_point*) pntarray[i];
        pntarray[i] = NULL;
        if (i == 0)
            mphead = cirpnt;
        else
        {
            cirpnt->mpprev = mpend;
	        mpend->mpnext=cirpnt;
        }
        mpend = cirpnt;
    }
    mphead->mpprev = NULL;
	mpend->mpnext=NULL;

    free(pntarray);
    return true;

}


int _cdecl CompareDis(const void* elem1, const void* elem2)
{ 
//	double x,y,z;
	double dis1, dis2;
	PNT vtx1, vtx2;

	vtx1 = PNT((*((Ti_point**) elem1))->GetX(), (*((Ti_point**) elem1))->GetY(), (*((Ti_point**) elem1))->GetZ());
	vtx2 = PNT((*((Ti_point**) elem2))->GetX(), (*((Ti_point**) elem2))->GetY(), (*((Ti_point**) elem2))->GetZ());
//	vtx2 = PNT((*((Ti_Point**) elem2))->x, (*((Ti_Point**) elem2))->y, (*((Ti_Point**) elem2))->z));
	vtx1 = vtx1-mComprVec;
	vtx2 = vtx2-mComprVec;
	dis1 = vtx1.Length();
	dis2 = vtx2.Length();

	if (dis1>dis2)       
		return 1;
	else if (dis1<dis2)       
		return -1;
	else 
        return 0;
}



extern int _cdecl SortCompareMinCur(const void* elem1,const void* elem2);
extern int _cdecl SortCompareMaxCur(const void* elem1,const void* elem2);


//void 
//Ti_cld_point::CalInitualThreshold(Ti_cld_point* pCld,double & CrossThreshold, double & AlongThreshold)
//{
//
//    Ti_point* cirpnt;
//	double MaxSum = 0;
//	double MinSum = 0;
//	Ti_point**  pntarraymax;
//	Ti_point**  pntarraymin;
//    int         count;
//	int i;
//
//	if((minfpoint.maxx - minfpoint.minx) < MAX_TOL)
//	{
//		CalcBox();
//	}
//
//	CalcCntDen();
//
//    if (mnum < 24)
//	{ 
//		AfxMessageBox("Not Enough Point Data Provided! Please Select Another File!") ;
//		return;
//	}
//
//    // calculate principal curvatures and directions
//    
//	cirpnt = mphead;
//
//    while (cirpnt != 0)
//    {
//        if (!CalcPriCur(cirpnt))
//        {
//            return ;
//        }
//		cirpnt = cirpnt->GetNext();
//    }
//	
//
//	pntarraymax = (Ti_point**) calloc((size_t)mnum, sizeof(int));
//    if (pntarraymax == 0)
//    return ;
//
//    // initialize the pointer array by the point data
//    cirpnt = mphead;
//    count = 0;
//    while (cirpnt != 0)
//    {
//        pntarraymax[count] = cirpnt;
//        cirpnt = cirpnt->GetNext();
//        count ++;
//	}
//
//
//    // sort the point according to the maxmum curvature.
//    qsort((void*)pntarraymax, (size_t)mnum, sizeof(Ti_point*), SortCompareMaxCur);
//
//
//   // change the point data by sorted pointer array
//    for ( i=0; i<mnum; i++)
//    {
//        cirpnt = (Ti_point*) pntarraymax[i];
//        pntarraymax[i] = 0;
//        cirpnt->Init();
//        if (i == 0)
//            mphead = cirpnt;
//        else
//            mpend->SetNext(cirpnt);
//        mpend = cirpnt;
//    }
//
//
//	cirpnt = mphead;
//    
//	for ( i=0; i < mnum/50; i++)
//	{
//        MaxSum = MaxSum + cirpnt->GetMaxCur();
//		cirpnt = cirpnt->GetNext();
//	}
//
//
//    cirpnt = mpend;
//
//	for (i=0; i < mnum/50; i++)
//	{
//        MinSum = MinSum + cirpnt->GetMaxCur();
//		cirpnt = cirpnt->GetPrev();
//	}
//
//
//    CrossThreshold = fabs(MinSum/MaxSum);
//
//	  free(pntarraymax);
//
//
//    pntarraymin = (Ti_point**) calloc((size_t)mnum, sizeof(int));
//    if (pntarraymin == 0)
//    return ;
//
//    // initialize the pointer array by the point data
//    cirpnt = mphead;
//    count = 0;
//    while (cirpnt != 0)
//    {
//        pntarraymin[count] = cirpnt;
//        cirpnt = cirpnt->GetNext();
//        count ++;
//	}
//
//    // sort the point according to the minmum curvature.
//    qsort((void*)pntarraymin, (size_t)mnum, sizeof(Ti_point*), SortCompareMinCur);
//
//
//   // change the point data by sorted pointer array
//    for ( i=0; i<mnum; i++)
//    {
//        cirpnt = (Ti_point*) pntarraymin[i];
//        pntarraymin[i] = 0;
//        cirpnt->Init();
//        if (i == 0)
//            mphead = cirpnt;
//        else
//            mpend->SetNext(cirpnt);
//        mpend = cirpnt;
//    }
//
//    mIsSorted = true;
//    free(pntarraymin);
//
//
//    MaxSum = 0;
//	MinSum = 0;
//    
//	cirpnt = mphead;
//    
//	for (i=0; i < mnum/50; i++)
//	{
//        MaxSum = MaxSum + cirpnt->GetMinCur();
//		cirpnt = cirpnt->GetNext();
//	}
//
//
//    cirpnt = mpend;
//
//	for (i=0; i < mnum/50; i++)
//	{
//        MinSum = MinSum + cirpnt->GetMinCur();
//		cirpnt = cirpnt->GetPrev();
//	}
//
//    AlongThreshold = fabs(MinSum/MaxSum);
//
//}
//
//
//------------------------------------------------------
extern int _cdecl SortCompareBcsMin(const void* elem1,const void* elem2);

//void 
//Ti_cld_point::CalThresholdBin(Ti_cld_point* pCld,int samplelenth,double & CrossThreshold, double & AlongThreshold)
//{
//
//    Ti_point* cirpnt;
//	double MaxSum = 0;
//	double MinSum = 0;
//	int i;
//
//	if((minfpoint.maxx - minfpoint.minx) < MAX_TOL)
//	{
//		CalcBox();
//	}
//
//	CalcCntDen();
//
//    if (mnum < 24)
//	{ 
//		AfxMessageBox("Not Enough Point Data Provided! Please Select Another File!") ;
//		return;
//	}
//
//    // filter data
//	FilterData( samplelenth*2 > 8 ? (int)(samplelenth * 1.5) :( (samplelenth == 1) ? samplelenth*4 : samplelenth * 2) );
//
//    // calculate principal curvatures and directions
//    for ( i=0; i<mBcsNum; i++)
//    {
//        cirpnt = mpBcs[i]->GetCntPnt();
//        if (!CalcPriCur(cirpnt))
//        {
//            RemoveBinCells();
//            return;
//        }
//    }
//
//
//    // sort the bincell according to the maximum curvature
//    qsort((void*)mpBcs, (size_t)mBcsNum, sizeof(BinCell*), SortCompareBcs);
//
//    
//	for ( i=0; i < mBcsNum/20; i++)
//	{
//        MaxSum = MaxSum + fabs(mpBcs[i]->GetCntPnt()->GetMaxCur());
//	}
//
//
//	for (i = mBcsNum; i > mBcsNum - mBcsNum/20; i--)
//	{
//        MinSum = MinSum + fabs(mpBcs[i-1]->GetCntPnt()->GetMaxCur());
//	}
//
//
//    CrossThreshold = MinSum*0.8/MaxSum;
//
//    char CrossThresholdchar[50]; 
//
//	_gcvt( CrossThreshold, 4, CrossThresholdchar );
//
//	CrossThreshold = atof(CrossThresholdchar);
//
//
//  // sort the bincell according to the minimum curvature
//    qsort((void*)mpBcs, (size_t)mBcsNum, sizeof(BinCell*), SortCompareBcsMin);
//
//
//    MaxSum = 0;
//	MinSum = 0;
//    
//	for ( i=0; i < mBcsNum/20; i++)
//	{
//        MaxSum = MaxSum + fabs(mpBcs[i]->GetCntPnt()->GetMinCur());
//	}
//
//
//    cirpnt = mpend;
//
//	for (i = mBcsNum; i > mBcsNum - mBcsNum/20; i--)
//	{
//        MinSum = MinSum + fabs(mpBcs[i-1]->GetCntPnt()->GetMinCur());
//	}
//
//    AlongThreshold = MinSum*0.8/MaxSum;
//
//    char AlongThresholdchar[50]; 
//
//	_gcvt( AlongThreshold, 4, AlongThresholdchar );
//
//    AlongThreshold = atof(AlongThresholdchar);
//
//    RemoveBinCells();
//
//    return;
//
//}
//
//
