#include "StdAfx.h"
#include <memory.h>
#include <stdlib.h>
#include<string.h>
#include "ti_simplearray.h"

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

// emergency destroy ///////////////////////////////////////////////////

template <class T>
void SimpleArray<T>::EmergencyDestroy(void)
{
  m_count = 0;
  m_capacity = 0;
  m_a = 0;
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