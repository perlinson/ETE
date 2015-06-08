/////////////////////////////////////////////////////////////////////////////////////////
//			file name : Ti_simpleArray.h   
//
//			File Description : 
//				Dynamic array used for Ti_Matrix class
//			created by Xiaoming Du 10/27/2004
//
//			Modification Record:	
//////////////////////////////////////////////////////////////////////////////////////////
// Ti_simpleArray.h: interface for the SimpleArray class.


#pragma once
#include <memory.h>
#include <stdlib.h>
#include<string.h>
template <class T> class   SimpleArray
{
	///////////////////////////////////////////////////////////////////////
	//             data  structure
	////////////////////////////////////////////////////////////////////////

private:
	int  m_count;    // 0 <= m_count <= m_capacity
	int  m_capacity; // actual length of m_a[]
	T*   m_a;        // pointer to array memory

public:
  	///////////////////////////////////////////////////////////////////////
	//            construction / destruction
	////////////////////////////////////////////////////////////////////////
  // These constructors create an array that uses realloc() to manage
  // the array memory.
  SimpleArray();
  SimpleArray( int ); // int = initial capacity

  // Copy constructor
  SimpleArray( const SimpleArray<T>& );

  virtual
  ~SimpleArray();

  // Assignment operator
  virtual   SimpleArray<T>& operator=( const SimpleArray<T>& );  
  // emergency bailout ///////////////////////////////////////////////////
  void EmergencyDestroy(void); // call only when memory used by this array
                               // may have become invalid for reasons beyond
                               // your control. EmergencyDestroy() zeros
                               // anything that could possibly cause
                               // ~SimpleArray() to crash.

  // query ///////////////////////////////////////////////////////////////
  
	int Count() const;      // number of elements in array
	
	int Capacity() const;  // capacity of array

  T& operator[]( int );              // grows array if index >= Capacity()

  const T& operator[]( int ) const;  // The const operator[] does not
                                     // check for a valid index.  Caller
                                     // is responsible for making sure
                                     // that the index is > 0 and < Capacity().

  operator T*();                     // The cast operators return a pointer
  operator const T*() const;         // to the array.  If Count() is zero,
                                     // this pointer is NULL.

  T* First();
  const T* First() const;             // returns NULL if count = 0

  T* At( int );
  const T* At( int ) const;          // returns NULL if index < 0 or index >= count

  T* Last();
  const T* Last() const;             // returns NULL if count = 0

  
  // array operations ////////////////////////////////////////////////////

  T& AppendNew();                    // Most efficient way to add a new element 
                                     // to the array.  Increases count by 1.

  void Append( const T& );           // Append copy of element.
                                     // Increments count by 1.

  void Append( int, const T* );      // Append copy of an array T[count]


  void Insert( int, const T& );      // Insert copy of element. Uses
                                     // memmove() to perform any
                                     // necessary moving.
                                     // Increases count by 1.

  void Remove();                     // Removes last element.  Decrements
                                     // count by 1.  Does not change capacity.

  virtual
  void Remove( int );                // Removes element. Uses memmove() to
                                     // perform any necessary shifting.
                                     // Decrements count by 1.  Does not change
                                     // capacity

  void Empty();           // Sets count to 0, leaves capacity untouched.

  void Reverse();         // reverse order

  void Swap(int,int);     // swap elements i and j

  //////////
  // Search( e ) does a SLOW search of the array starting at array[0]
  // and returns the index "i" of the first element that satisfies 
  // e == array[i]. (== is really memcmp()).  If the search is not 
  // successful, then Search() returns -1.  For Search(T) to work 
  // correctly, T must be a simple type.  Use Search(p,compare())
  // for Ts that are structs/classes that contain pointers.  Search()
  // is only suitable for performing infrequent searchs of small 
  // arrays.  Sort the array and use BinarySearch() for performing
  // efficient searches.
  int Search( const T& ) const;

  //////////
  // Search( p, compare ) does a SLOW search of the array starting
  // at array[0] and returns the index "i" of the first element 
  // that satisfies  compare(p,&array[i])==0.  If the search is not
  // successful, then Search() returns -1.  Search() is only suitable
  // for performing infrequent searches of small arrays.  Sort the
  // array and use BinarySearch() for performing efficient searches.
  int Search( const T*, int (*)(const T*,const T*) ) const;

  //////////
  // BinarySearch( p, compare ) does a fast search of a sorted array
  // and returns the smallest index "i" of the element that satisifies
  // 0==compare(p,&array[i]).  If the search is successful, 
  // BinarySearch() returns the index of the element (>=0).
  // If the search is not successful, BinarySearch() returns -1.  Use
  // HeapSort( compare ) or QuickSort( compare ) to sort the array.
  int BinarySearch( const T*, int (*)(const T*,const T*) ) const;

  //////////
  // Sorts the array using the heap sort algorithm.
  bool HeapSort( int (*)(const T*,const T*) );

  //////////
  // Sorts the array using the heap sort algorithm.
  bool QuickSort( int (*)(const T*,const T*) );

  //////////
  // Sort() computes fills in the index[] array so that
  // array[index[i]] <= array[index[i+1]].  The array is not
  // modified

  //////////
  // Zeros all array memory. 
  // Count and capacity are not changed.
  void Zero();

  //////////
  // Sets all bytes in array memory to value. 
  // Count and capacity are not changed.
  void MemSet(unsigned char); 
  
  // memory managment ////////////////////////////////////////////////////

  void Reserve( int );    // increase capacity to at least the requested value

  void Shrink();          // remove unused capacity

  void Destroy();         // free any memory and set count and capacity to zero
    
  // low level memory managment //////////////////////////////////////////

  // By default, SimpleArray<> uses realloc() to manage
  // the dynamic array memory. If you want to use something 
  // besides realloc() to manage the array memory, then override 
  // Realloc(). The T* Realloc( ptr, capacity ) should do the following:
  //
  // 1) If ptr and capacity are zero, return NULL.
  // 2) If ptr is NULL, an capacity > 0, allocate a memory block of capacity*sizeof(T)
  //    bytes and return a pointer to this block.  If the allocation request
  //    fails, return NULL.
  // 3) If ptr is not NULL, and capacity is 0, free the memory block pointed
  //    to by ptr and return NULL.
  // 4) If ptr is not NULL and capacity > 0, reallocate the memory block and
  //    return a pointer to the reallocated block.  If the reallocation
  //    request fails, return NULL.
  //
  // NOTE WELL: Microsoft's VC 6.0 realloc() contains a bug that can cause
  //            crashes and should be avoided.  See MSDN Knowledge Base
  //            article ID Q225099 for more information.
  virtual
  T* Realloc(T*,int); // (re)allocated capacity*sizeof(T) bytes

  T* Array();                         // The Array() function return the 
  
  const T* Array() const;             // m_a pointer value.

  void SetCount( int );               // If value is <= Capacity(), then
                                      // sets count to specified value.

  void SetCapacity( int );            // Shrink/grows capacity.  If value
                                      // is < current Count(), then count
                                      // is reduced to value.
                                      //

  T* KeepArray();                     // returns pointer to array and zeros
                                      // out this class.  Caller is responsible
                                      // for using free() to release array
                                      // memory.

protected:
  // implimentation //////////////////////////////////////////////////////
  void Move( int /* dest index*/, int /* src index */, int /* element count*/ );

};

/////////////////////////////////////////////////////////////////////////////////////
//  Class SimpleArray<>
/////////////////////////////////////////////////////////////////////////////////////

// construction ////////////////////////////////////////////////////////
  // By default, SimpleArray<> uses realloc() to manage
  // the dynamic array memory. If you want to use something 
  // besides realloc() to manage the array memory, then override 
  // Realloc(). The T* Realloc( ptr, capacity ) should do the following:
  //
  // 1) If ptr and capacity are zero, return NULL.
  // 2) If ptr is NULL, an capacity > 0, allocate a memory block of capacity*sizeof(T)
  //    bytes and return a pointer to this block.  If the allocation request
  //    fails, return NULL.
  // 3) If ptr is not NULL, and capacity is 0, free the memory block pointed
  //    to by ptr and return NULL.
  // 4) If ptr is not NULL and capacity > 0, reallocate the memory block and
  //    return a pointer to the reallocated block.  If the reallocation
  //    request fails, return NULL.
  //
  // NOTE WELL: Microsoft's VC 6.0 realloc() contains a bug that can cause
  //            crashes and should be avoided.  See MSDN Knowledge Base
  //            article ID Q225099 for more information.

template <class T>
T* SimpleArray<T>::Realloc(T* ptr,int capacity)
{
 //       T*  p = new T[capacity];
	////_assert(p!= NULL);
	////int num ;
	//if(Count() < capacity)
	//	{
	//	p = (T*)memcpy(p, ptr, sizeof(T)*Count());
	//	}
	//else
	//	{
	//	p = (T*)memcpy(p, ptr, sizeof(T)*capacity);
	//	}
	//delete  []ptr;
	//return p;
 return (T*)realloc(ptr,capacity*sizeof(T));
	/*if (ptr==NULL&&capacity==0)return NULL;
	if(capacity==0)
	{
		free(ptr);
		ptr=NULL;
		return NULL;
	}
	if (ptr==NULL)
	{
		ptr=(T*)malloc(capacity*sizeof(T));
		if(ptr!=NULL)
			return ptr;
	}
	T* ptmp=(T*)malloc(capacity*sizeof(T));

	_ASSERT(ptmp!=NULL);
	if(m_count<capacity)
    memmove( ptmp, ptr, capacity*sizeof(T) );
	else
	memmove( ptmp, ptr, m_count*sizeof(T) );
	free(ptr);
	ptr=ptmp;

  return ptr;*/
}

template <class T>
SimpleArray<T>::SimpleArray()
                          : m_count(0),
                            m_capacity(0),
                            m_a(0)
{}

template <class T>
SimpleArray<T>::SimpleArray( int c )
                          : m_count(0),
                            m_capacity(0),
                            m_a(0)
{
  if ( c > 0 ) 
    SetCapacity( c );
}

// Copy constructor
template <class T>
SimpleArray<T>::SimpleArray( const SimpleArray<T>& src )
                          : m_count( 0 ),
                            m_capacity( 0 ),
                            m_a( 0 )
{
  *this = src; // operator= defined below
}

template <class T>
SimpleArray<T>::~SimpleArray()
{ 
  SetCapacity(0);
}

template <class T>
SimpleArray<T>& SimpleArray<T>::operator=( const SimpleArray<T>& src )
{
  if( &src != this ) {
    if ( src.m_count <= 0 ) {
      m_count = 0;
    }
    else {
      if ( m_capacity < src.m_count ) {
        SetCapacity( src.m_count );
      }
      if ( m_a ) {
        m_count = src.m_count;
        memcpy( m_a, src.m_a, m_count*sizeof(T) );
      }
    }
  }  
  return *this;
}

// emergency destroy ///////////////////////////////////////////////////

template <class T>
void SimpleArray<T>::EmergencyDestroy(void)
{
  m_count = 0;
  m_capacity = 0;
  m_a = 0;
}

// query ///////////////////////////////////////////////////////////////

template <class T>
int SimpleArray<T>::Count() const
{
  return m_count;
}

template <class T>
int SimpleArray<T>::Capacity() const
{
  return m_capacity;
}

template <class T>
T& SimpleArray<T>::operator[]( int i )
{ 
//#if defined(_DEBUG)
//  ASSERT( i >=0 && i < m_capacity);
//#endif
  return m_a[i]; 
}

template <class T>
const T& SimpleArray<T>::operator[](int i) const
{
//#if defined(_DEBUG)
//  ASSERT( i >=0 && i < m_capacity);
//#endif
  return m_a[i];
}

template <class T>
SimpleArray<T>::operator T*()
{
  return (m_count > 0) ? m_a : 0;
}

template <class T>
SimpleArray<T>::operator const T*() const
{
  return (m_count > 0) ? m_a : 0;
}

template <class T>
T* SimpleArray<T>::Array()
{
  return m_a;
}

template <class T>
const T* SimpleArray<T>::Array() const
{
  return m_a;
}

template <class T>
T* SimpleArray<T>::KeepArray()
{
  T* p = m_a;
  m_count = 0;
  m_capacity = 0;
  m_a = NULL;
  return p;
}

template <class T>
T* SimpleArray<T>::First()
{ 
  return (m_count > 0) ? m_a : 0;
}

template <class T>
const T* SimpleArray<T>::First() const
{
  return (m_count > 0) ? m_a : 0;
}

template <class T>
T* SimpleArray<T>::At( int i )
{ 
  return (i >= 0 && i < m_count) ? m_a+i : 0;
}

template <class T>
const T* SimpleArray<T>::At( int i) const
{
  return (i >= 0 && i < m_count) ? m_a+i : 0;
}

template <class T>
T* SimpleArray<T>::Last()
{ 
  return (m_count > 0) ? m_a+(m_count-1) : 0;
}

template <class T>
const T* SimpleArray<T>::Last() const
{
  return (m_count > 0) ? m_a+(m_count-1) : 0;
}

// array operations ////////////////////////////////////////////////////

template <class T>
void SimpleArray<T>::Move( int dest_i, int src_i, int ele_cnt )
	{
	// private function for moving blocks of array memory
	// caller is responsible for updating m_count.
	if ( ele_cnt <= 0 || src_i < 0 || dest_i < 0 || src_i == dest_i || 
		src_i + ele_cnt > m_count || dest_i > m_count )
		return;

	int capacity = dest_i + ele_cnt;
	if ( capacity > m_capacity ) 
		{
		if ( capacity < 2*m_capacity )
			capacity = 2*m_capacity;
		SetCapacity( capacity );
		}

	memmove( &m_a[dest_i], &m_a[src_i], ele_cnt*sizeof(T) );
	}

template <class T>
T& SimpleArray<T>::AppendNew()
{
  if ( m_count == m_capacity )
	  {
    Reserve( m_count+1 );
  }
  memset( &m_a[m_count], 0, sizeof(T) );
  return m_a[m_count++];
}

template <class T>
void SimpleArray<T>::Append( const T& x ) 
{

  Insert( m_count, x );

}

template <class T>
void SimpleArray<T>::Append( int count, const T* p ) 
	{
	if ( count > 0 && p ) 
		{
		if ( count + m_count > m_capacity )
			{
			int newcapacity = m_capacity;
			if ( newcapacity < 2 )
				newcapacity = 2;
			while ( newcapacity < count + m_count )
				newcapacity *= 2;
			Reserve(newcapacity);
			}
		memcpy( m_a + m_count, p, count*sizeof(T) );
		m_count += count;
		}
	}

template <class T>
void SimpleArray<T>::Insert( int i, const T& x ) 
	{
	if( i >= 0 && i <= m_count ) 
		{
		if ( m_count == m_capacity ) 
			{
			if( m_capacity == 0 )
				Reserve( 2 );
			else
				Reserve( 2*m_capacity );
			}
		m_count++;
		Move( i+1, i, m_count-1-i );
		m_a[i] = x;
		}
	}

template <class T>
void SimpleArray<T>::Remove()
{
  Remove(m_count-1);
} 

template <class T>
void SimpleArray<T>::Remove( int i )
	{
	if ( i >= 0 && i < m_count ) 
		{
		Move( i, i+1, m_count-1-i );
		m_count--;
		memset( &m_a[m_count], 0, sizeof(T) );
		}
	} 

template <class T>
void SimpleArray<T>::Empty()
{
  if ( m_a )
    memset( m_a, 0, m_capacity*sizeof(T) );
  m_count = 0;
}

template <class T>
void SimpleArray<T>::Reverse()
	{
	// NOTE:
	// If anything in "T" depends on the value of this's address,
	// then don't call Reverse().
	T t;
	int i = 0;  
	int j = m_count-1;
	for ( /*empty*/; i < j; i++, j-- )
		{
		t = m_a[i];
		m_a[i] = m_a[j];
		m_a[j] = t;
		}
	}

template <class T>
void SimpleArray<T>::Swap( int i, int j )
{
if ( i != j ) 
	{
	const T t(m_a[i]);
	m_a[i] = m_a[j];
	m_a[j] = t;
	}
}

template <class T>
int SimpleArray<T>::Search( const T& key ) const
	{
	const T* p = &key;
	for ( int i = 0; i < m_count; i++ )
		{
		if (!memcmp(p,m_a+i,sizeof(T))) 
			return i;
		}
	return -1;
	}

template <class T>
int SimpleArray<T>::Search( const T* key, int (*compar)(const T*,const T*) ) const
	{
	for ( int i = 0; i < m_count; i++ ) 
		{
		if (!compar(key,m_a+i)) 
			return i;
		}
	return -1;
	}

template <class T>
int SimpleArray<T>::BinarySearch( const T* key, int (*compar)(const T*,const T*) ) const
{
  const T* found = (key&&m_a&&m_count>0) ? (const T*)bsearch( key, m_a, m_count, sizeof(T), (int(*)(const void*,const void*))compar ) : 0;
  return found ? ((((unsigned int)found) - ((unsigned int)m_a))/sizeof(T)) : -1;
}

template <class T>
bool SimpleArray<T>::HeapSort( int (*compar)(const T*,const T*) )
{
  bool rc = 0;
  if ( m_a && m_count > 0 && compar ) 
	  {
    if ( m_count > 1 )
      ON_hsort( m_a, m_count, sizeof(T), (int(*)(const void*,const void*))compar );
    rc = 1;
  }
  return rc;
}

template <class T>
bool SimpleArray<T>::QuickSort( int (*compar)(const T*,const T*) )
	{
	bool rc = 0;
	if ( m_a && m_count > 0 && compar )
		{
		if ( m_count > 1 )
			qsort( m_a, m_count, sizeof(T), (int(*)(const void*,const void*))compar );
		rc = 1;
		}
	return rc;
	}



template <class T>
void SimpleArray<T>::Zero()
	{
	if ( m_a && m_capacity > 0 ) 
		{
		memset( m_a, 0, m_capacity*sizeof(T) );
		}
	}

template <class T>
void SimpleArray<T>::MemSet( unsigned char value )
	{
	if ( m_a && m_capacity > 0 ) 
		{
		memset( m_a, value, m_capacity*sizeof(T) );
		}
	}

// memory managment ////////////////////////////////////////////////////

template <class T>
void SimpleArray<T>::Reserve( int newcap ) 
{
  if( m_capacity < newcap )
    SetCapacity( newcap );
}

template <class T>
void SimpleArray<T>::Shrink()
{
  SetCapacity( m_count );
}

template <class T>
void SimpleArray<T>::Destroy()
{
  SetCapacity( 0 );
}

// low level memory managment //////////////////////////////////////////

template <class T>
void SimpleArray<T>::SetCount( int count ) 
{
  if ( count >= 0 && count <= m_capacity )
    m_count = count;
}

template <class T>
void SimpleArray<T>::SetCapacity( int capacity ) 
{
  // sets capacity to input value
	if ( capacity != m_capacity ) 
	{
		if( capacity > 0 ) 
		{
			if ( m_count > capacity )
				m_count = capacity;
			// NOTE: Realloc() does an allocation if the first argument is NULL.
			m_a = Realloc( m_a, capacity );
			if ( m_a ) 
			{
				if ( capacity > m_capacity ) 
				{
					// zero new memory
					memset( m_a + m_capacity, 0, (capacity-m_capacity)*sizeof(T) );
				}
				m_capacity = capacity;
			}
			else 
			{
				// out of memory
				m_count = m_capacity = 0;
			}
		}
		else if (m_a) 
		{
			Realloc(m_a,0);
			m_a = 0;
			m_count = m_capacity = 0;
		}
	}
}
