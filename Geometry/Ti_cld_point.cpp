/*
              ¨q¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨r
              ¨U  ©çDigital Manufacturing Laboratory ©ç   ¨U
¨q¨T¨T¨T¨T¨T¨T©È       Global Research Center, GE         ©À¨T¨T¨T¨T¨T¨T¨r
¨U            ¨U        Copyright(c) (2003-2006)          ¨U            ¨U
¨U            ¨t¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨T¨s            ¨U
¨U                                                                      ¨U
¨U      File :             Ti_cld_point.cpp                             ¨U
¨U                                                                      ¨U
¨U      Programmer:        Xiaoming Du, Tian Chen and Guofei Hu         ¨U
¨U                                                                      ¨U
¨U      Last modified:     09/19/05 by Guofei Hu                        ¨U
¨U                                                                      ¨U
¨U¡¡    Description:       Define operations for 3D point cloud         ¨U
¨U                                                                      ¨U
¨U    ¨q©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤¨r    ¨U
¨t¨T¨T©È                   http://www.ge.com                      ©À¨T¨T¨s
      ¨t©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤¨s

 */
#include "stdafx.h"
#include "Ti_cld_point.h"           // definition of Ti_cld_point class
#include "stdlib.h"                 // commonly used library functions
#include "math.h"                   // math functions
#include "ti_matrix.h"
#include "ti_svd1.h"


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
}



///////////////////////////////////////////////////////////////////////////////
// destruction
///////////////////////////////////////////////////////////////////////////////

Ti_cld_point::~Ti_cld_point()
{
    InitObject();
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
void Ti_cld_point::InitObject()
{
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
// clear calculation falgs of all points
///////////////////////////////////////////////////////////////////////////////
void Ti_cld_point::XCloud2Ti_Cloud(XCloud & Ti_Cloud)
{
	int i;
	int num = Ti_Cloud.GetPntNum();
	for( i=0;i<num;i++)
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
	int i;
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
    for (i=100; i>=1; i--)
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
	double      len, dis;
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
	    avgErr += fabs(dis);
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

///////////////////////////////////////////////////////////////////
     if(avgErr<0.00003)
	 {
		 delete pln;
		 pln = NULL;
        return false;
	 }
///////////////////////////////////////////////////////////////////

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
	{
		svdm[i] = 0.0;
	}
	for(int i=0; i<4; i++)
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

bool Ti_cld_point::CalcPriCur(Ti_point* pCntPnt,Ti_cld_point * NearTiCld)
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
			if(NearTiCld==NULL)
			{
           			// get a neighborhood point cloud with 24~32 points
				//	nearpnts = FindNearPnts(pCntPnt, 23, 31, nearnum);
					nearpnts = FindNearPnts(pCntPnt, 25, 51, nearnum);

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
			}
			else
					nearcld=NearTiCld;

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
		nearcld->InitObject();
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
        if (!CalcPriCur(pCntPnt,NULL))
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
	double dis1, dis2;
	PNT vtx1, vtx2;

	vtx1 = PNT((*((Ti_point**) elem1))->GetX(), (*((Ti_point**) elem1))->GetY(), (*((Ti_point**) elem1))->GetZ());
	vtx2 = PNT((*((Ti_point**) elem2))->GetX(), (*((Ti_point**) elem2))->GetY(), (*((Ti_point**) elem2))->GetZ());
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
extern int _cdecl SortCompareBcsMin(const void* elem1,const void* elem2);
