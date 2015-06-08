// : interface for the Workspace class.
/*_____________________________________________________
 |
 |	Intra3D Layer1 :  简单图形对象 — PointCloud
 |                                          
 |  文件：Workspace.h
 |
 |  功能：内存管理
 |
 |  开发：杜晓明 ，2002/10/10
 |
 |	源程序测试：进行了应用测试
 |_____________________________________________________*/

/*_____________________________________________________
 |  
 |	基于 OpenGL 的交互式三维图形软件开发工具
 |
 |	**   HustSurf for Windows 9X/NT  **
 |
 |	          著作权人：杜晓明 
 |	
 |	华中科技大学机械学院 (430074)
 |_____________________________________________________*/
#if !defined(WORKSPACE_H)
#define WORKSPACE_H

#include<stdio.h>

class  Workspace  
{
public:
	int KeepFile( FILE* pFile );
	FILE* OpenFile( const char*, const char*);
	Workspace();
	virtual ~Workspace();

  // The GetMemory...() functions use rhmalloc() to get memory.
  void*        GetMemory( int );
  int*         GetIntMemory( int );
  double*      GetDoubleMemory( int );

  // The GrowMemory...() functions use onrealloc() to get more memory
  // The pointer passed to GrowMemory() must be one that was returned
  // by any of the GetMemory...() calls.
  void*        GrowMemory( void*, int );
  int*         GrowIntMemory( int*, int );
  double*      GrowDoubleMemory( double*, int );

  // Calling the KeepMemory() function with a pointer returned from
  // one of the Get...() or Grow...() calls keeps the workspace destructor
  // from rhfree()ing the memory.  The caller is responsible for calling 
  // rhfree() to release the memory.
  bool KeepMemory( void* );  // returns TRUE for success and FALSE for failure


private:
  struct FBLK 
  {
    struct FBLK* pNext;
    FILE* pFile;
  } * m_pFileBlk;

  struct MBLK 
  {
    struct MBLK* pNext;
    void* pMem;
  } * m_pMemBlk;

private:
  // no implementation of the following
  Workspace( const Workspace& );            // no copy constructor
  Workspace& operator=( const Workspace& ); // no operator=
};

#endif // !defined(WORKSPACE_H)
