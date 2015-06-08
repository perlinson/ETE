#include "StdAfx.h"
#include "ti_matrix.h"
#include <memory.h>
#include <stdlib.h>
#include<string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T>
T* Ti_Matrix<T>::operator[](int i)
{
  return m[i];
}
template <class T>
const T* Ti_Matrix<T>::operator[](int i) const
{
  return m[i];
}

template <class T>
Ti_Matrix<T>::Ti_Matrix() : m(0), m_row_count(0), m_col_count(0)
{
}
template <class T>
Ti_Matrix<T>::Ti_Matrix( int row_size, int col_size ) 
                   : m(0), m_row_count(0), m_col_count(0)
{
  Create(row_size,col_size);
}

template <class T>
Ti_Matrix<T>::Ti_Matrix( int row0, int row1, int col0, int col1 ) 
                   : m(0), m_row_count(0), m_col_count(0)
{
  Create(row0,row1,col0,col1);
}


template <class T>
Ti_Matrix<T>::Ti_Matrix( const Ti_Matrix& src )
{
  *this = src;
}
template <class T>
Ti_Matrix<T>::Ti_Matrix(const Ti_Matrix& src,int nb,int ne)
{
  Create(src.RowCount(),ne-nb+1);
  for(int i=0;i<src.RowCount();i++)
	  for(int j=nb;j<=ne;j++)
		  m[i][j-nb]=src.m[i][j];
}

template <class T>
Ti_Matrix<T>::~Ti_Matrix()
{
	Destroy();
}

template <class T>
int Ti_Matrix<T>::RowCount() const
{
  return m_row_count;
}

template <class T>
int Ti_Matrix<T>::ColCount() const
{
  return m_col_count;
}
 template <class T>
void Ti_Matrix<T>::Identity()
 {
 int t;
 if( ColCount() < RowCount())
	 t = ColCount();
 else
	 t = RowCount();
 this->Zero();
 for(int i =0; i< t; i++)
	 {
	 m[i][i] = 1;
	 }
}
template <class T>
int Ti_Matrix<T>::MinCount() const
{
  return (m_row_count <= m_col_count) ? m_row_count : m_col_count;
}

template <class T>
int Ti_Matrix<T>::MaxCount() const
{
  return (m_row_count >= m_col_count) ? m_row_count : m_col_count;
}

template <class T>
bool Ti_Matrix<T>::Create( int row_count, int col_count)
{
  bool b = 0;
  Destroy();
  if ( row_count > 0 && col_count > 0 )
  {
    m_row.Reserve(row_count);
    m_row.SetCount(row_count);
    m_a.Reserve(row_count*col_count);
    m_a.SetCount(row_count*col_count);
    m = m_row.Array();
    m[0] = m_a.Array();
    int i;
    for ( i = 1; i < row_count; i++ )
      m[i] = m[i-1]+col_count;
    m_row_count = row_count;
    m_col_count = col_count;
    b = 1;
  }
  return b;
}

template <class T>
bool Ti_Matrix<T>::Create( // E.g., Create(1,5,1,7) creates a 5x7 sized matrix that with
             // "top" row = m[1][1],...,m[1][7] and "bottom" row
             // = m[5][1],...,m[5][7].  The result of Create(0,m,0,n) is
             // identical to the result of Create(m+1,n+1).
   int ri0, // first valid row index
   int ri1, // last valid row index
   int ci0, // first valid column index
   int ci1 // last valid column index
   )
{
  bool b = 0;
  if ( ri1 > ri0 && ci1 > ci0 ) {
    // juggle m[] pointers so that m[ri0+i][ci0+j] = m_row[i][j];
    b = Create( ri1-ri0, ci1-ci0 );
    if (b) {
      if ( ci0 != 0 ) {
        int i;
        for ( i = 0; i < m_row_count; i++ ) {
          m[i] -= ci0;
        }
      }
      if ( ri0 != 0 )
        m -= ri0;
    }
  }
  return b;
}

template <class T>
void Ti_Matrix<T>::Destroy()
{
  m = 0;
  m_row_count = 0;
  m_col_count = 0;
  m_row.SetCapacity(0);
  m_a.SetCapacity(0);
}

template <class T>
void Ti_Matrix<T>::EmergencyDestroy()
{
  // call if memory pool used matrix by becomes invalid
  m = 0;
  m_row_count = 0;
  m_col_count = 0;
  m_row.EmergencyDestroy();
  m_a.EmergencyDestroy();
}

template <class T>
Ti_Matrix<T>& Ti_Matrix<T>::operator=(const Ti_Matrix& src)
{
  if ( this != &src ) 
	  {
    Destroy();
    if ( Create( src.RowCount(), src.ColCount() ) ) 
	    {
      const int sizeof_row = m_col_count*sizeof(m[0][0]);
      int i;
      // src rows may be permuted - copy row by row
      T** m_dest = m_row.Array();
      T*const* m_src = src.m_row.Array();
      for ( i = 0; i < m_row_count; i++ )
	      {
        memcpy( m_dest[i], m_src[i], sizeof_row );
      }
    }
  }
  return *this;
}


template <class T>
bool Ti_Matrix<T>::Transpose()
{
  bool rc = 0;
  int i, j;
  T t;
  const int row_count = RowCount();
  const int col_count = ColCount();
  if ( row_count > 0 && col_count > 0 )
  {
	  rc = 1;
	  T** this_m = m_row.Array();
	  if ( row_count == col_count ) 
	  {
		  for ( i = 0; i < row_count; i++ ) 
			  for ( j = i+1; j < row_count; j++ ) 
			  {
				  t = this_m[i][j]; 
				  this_m[i][j] = this_m[j][i];
				  this_m[j][i] = t;
			  }
	  }
	  else 
	  {
		  Ti_Matrix A(*this);
		  Create(col_count,row_count);
		  this_m = m_row.Array();
		  for ( i = 0; i < row_count; i++ )
			  for ( j = 0; j < col_count; j++ )
			  {
				  this_m[j][i] = A.m[i][j];
			  }
	  }
  }
  return rc;
}

template <class T>
bool Ti_Matrix<T>::SwapRows( int row0, int row1 )
{
  bool b = 0;
  if ( m && 0 <= row0 && row0 < m_row_count && 0 <= row1 && row1 < m_row_count ) {
    if ( row0 != row1 ) {
      T** this_m = m_row.Array();
      T* tmp = this_m[row0]; this_m[row0] = this_m[row1]; this_m[row1] = tmp;
    }
    b = 1;
  }
  return b;
}

template <class T>
bool Ti_Matrix<T>::SwapCols( int col0, int col1 )
{
  bool b = 0;
  int i;
  T t;
  if ( m && 0 <= col0 && col0 < m_col_count && 0 <= col1 && col1 < m_col_count ) {
    if ( col0 != col1 ) {
      T** this_m = m_row.Array();
      for ( i = 0; i < m_row_count; i++ ) {
        t = this_m[i][col0]; this_m[i][col0] = this_m[i][col1]; this_m[i][col1] = t;
      }
    }
    b = 1;
  }
  return b;
}

template <class T>
void Ti_Matrix<T>::RowScale( int dest_row, double s )
{
  ON_ArrayScale( m_col_count, s, m_row[dest_row], m_row[dest_row] );
}

template <class T>
void Ti_Matrix<T>::RowOp( int dest_row, double s, int src_row )
{
  ON_Array_aA_plus_B( m_col_count, s, m_row[src_row], m_row[dest_row], m_row[dest_row] );
}

template <class T>
void Ti_Matrix<T>::ColScale( int dest_col, double s )
{
  int i;
  T** this_m = m_row.Array();
  for ( i = 0; i < m_row_count; i++ ) {
    this_m[i][dest_col] *= s;
  }
}

template <class T>
void Ti_Matrix<T>::ColOp( int dest_col, double s, int src_col )
{
  int i;
  T** this_m = m_row.Array();
  for ( i = 0; i < m_row_count; i++ ) {
    this_m[i][dest_col] += s*this_m[i][src_col];
  }
}


template <class T>
int
Ti_Matrix<T>::RowReduce( 
    double zero_tolerance,
    T& determinant,
    T& pivot 
    )
{
  double x;
  T piv, det;
  int i, k, ix, rank;

  T** this_m = m_row.Array();
  piv = det = 1.0;
  rank = 0;
  const int n = m_row_count <= m_col_count ? m_row_count : m_col_count;
  for ( k = 0; k < n; k++ ) {
    ix = k;
    x = fabs(this_m[ix][k]);
    for ( i = k+1; i < m_row_count; i++ ) {
      if ( fabs(this_m[i][k]) > x ) {
        ix = i;
        x = fabs(this_m[ix][k]);
      }
    }
    if ( x < piv || k == 0 ) {
      piv = x;
    }
    if ( x <= zero_tolerance ) {
      det = 0.0;
      break;
    }
    rank++;

    // swap rows
    SwapRows( ix, k );
    det = -det;

    // scale row k of matrix and B
    det *= this_m[k][k];
    x = 1.0/this_m[k][k];
    ON_ArrayScale( m_row_count - 1 - k, x, &this_m[k][k+1], &this_m[k][k+1] );

    // zero column k for rows below this_m[k][k]
    for ( i = k+1; i < m_row_count; i++ ) {
      x = -this_m[i][k];
      this_m[i][k] = 0.0;
      if ( fabs(x) > zero_tolerance ) {
        ON_Array_aA_plus_B( m_row_count - 1 - k, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
      }
    }
  }

  pivot = piv;
  determinant = det;

  return rank;
}

template <class T>
int
Ti_Matrix<T>::RowReduce( 
    double zero_tolerance,
    T* B,
    T* pivot 
    )
{
  T t;
  T x, piv;
  int i, k, ix, rank;

  T** this_m = m_row.Array();
  piv = 0.0;
  rank = 0;
  const int n = m_row_count <= m_col_count ? m_row_count : m_col_count;
  for ( k = 0; k < n; k++ ) {
    ix = k;
    x = fabs(this_m[ix][k]);
    for ( i = k+1; i < m_row_count; i++ ) {
      if ( fabs(this_m[i][k]) > x ) {
        ix = i;
        x = fabs(this_m[ix][k]);
      }
    }
    if ( x < piv || k == 0 ) {
      piv = x;
    }
    if ( x <= zero_tolerance )
      break;
    rank++;

    // swap rows of matrix and B
    SwapRows( ix, k );
    t = B[ix]; B[ix] = B[k]; B[k] = t;

    // scale row k of matrix and B
    x = 1.0/this_m[k][k];
    ON_ArrayScale( m_row_count - 1 - k, x, &this_m[k][k+1], &this_m[k][k+1] );
    B[k] *= x;

    // zero column k for rows below this_m[k][k]
    for ( i = k+1; i < m_row_count; i++ ) {
      x = -this_m[i][k];
      this_m[i][k] = 0.0;
      if ( fabs(x) > zero_tolerance ) {
        ON_Array_aA_plus_B( m_row_count - 1 - k, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
        B[i] += x*B[k];
      }
    }
  }

  if ( pivot )
    *pivot = piv;

  return rank;
}

/*int Ti_Matrix<T>::RowReduce( 
    T zero_tolerance,
    Ti_3dPoint* B,
    T* pivot 
    )
{
  Ti_3dPoint t;
  T x, piv;
  int i, k, ix, rank;

  T** this_m = m_row.Array();
  piv = 0.0;
  rank = 0;
  const int n = m_row_count <= m_col_count ? m_row_count : m_col_count;
  for ( k = 0; k < n; k++ ) {
    ix = k;
    x = fabs(this_m[ix][k]);
    for ( i = k+1; i < m_row_count; i++ ) {
      if ( fabs(this_m[i][k]) > x ) {
        ix = i;
        x = fabs(this_m[ix][k]);
      }
    }
    if ( x < piv || k == 0 ) {
      piv = x;
    }
    if ( x <= zero_tolerance )
      break;
    rank++;

    // swap rows of matrix and B
    SwapRows( ix, k );
    t = B[ix]; B[ix] = B[k]; B[k] = t;

    // scale row k of matrix and B
    x = 1.0/this_m[k][k];
    ON_ArrayScale( m_row_count - 1 - k, x, &this_m[k][k+1], &this_m[k][k+1] );
    B[k] *= x;

    // zero column k for rows below this_m[k][k]
    for ( i = k+1; i < m_row_count; i++ ) {
      x = -this_m[i][k];
      this_m[i][k] = 0.0;
      if ( fabs(x) > zero_tolerance ) {
        ON_Array_aA_plus_B( m_row_count - 1 - k, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
        B[i] += x*B[k];
      }
    }
  }

  if ( pivot )
    *pivot = piv;

  return rank;
}
*/

/*template <class T>
T RowAdd(int i )
 {
	 T sum=0;
	 for(int j=0;j<RowCount();j++)
		 sum+=this.m[j][i];
	 return sum;
 }*/

template <class T>
int
Ti_Matrix<T>::RowReduce( 
    double zero_tolerance,
    int pt_dim, int pt_stride, T* pt,
    T* pivot 
    )
{
  const int sizeof_pt = pt_dim*sizeof(pt[0]);
  T* tmp_pt = (T*)malloc(pt_dim);
  T *ptA, *ptB;
  T x, piv;
  int i, k, ix, rank, pti;

  T** this_m = m_row.Array();
  piv = 0.0;
  rank = 0;
  const int n = m_row_count <= m_col_count ? m_row_count : m_col_count;
  for ( k = 0; k < n; k++ ) {
    ix = k;
    x = fabs(this_m[ix][k]);
    for ( i = k+1; i < m_row_count; i++ ) {
      if ( fabs(this_m[i][k]) > x ) {
        ix = i;
        x = fabs(this_m[ix][k]);
      }
    }
    if ( x < piv || k == 0 ) {
      piv = x;
    }
    if ( x <= zero_tolerance )
      break;
    rank++;

    // swap rows of matrix and B
    SwapRows( ix, k );
    ptA = pt + (ix*pt_stride);
    ptB = pt + (k*pt_stride);
    memcpy( tmp_pt, ptA, sizeof_pt );
    memcpy( ptA, ptB, sizeof_pt );
    memcpy( ptB, tmp_pt, sizeof_pt );

    // scale row k of matrix and B
    x = 1.0/this_m[k][k];
    ON_ArrayScale( m_row_count - 1 - k, x, &this_m[k][k+1], &this_m[k][k+1] );
    ptA = pt + (k*pt_stride);
    for ( pti = 0; pti < pt_dim; pti++ )
      ptA[pti] *= x;

    // zero column k for rows below this_m[k][k]
    ptB = pt + (k*pt_stride);
    for ( i = k+1; i < m_row_count; i++ ) {
      x = -this_m[i][k];
      this_m[i][k] = 0.0;
      if ( fabs(x) > zero_tolerance ) {
        ON_Array_aA_plus_B( m_row_count - 1 - k, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
        ptA = pt + (i*pt_stride);
        for ( pti = 0; pti < pt_dim; pti++ ) {
          ptA[pti] += x*ptB[pti];
        }
      }
    }
  }

  if ( pivot )
    *pivot = piv;

  free(tmp_pt);

  return rank;
}


template <class T>
bool Ti_Matrix<T>::BackSolve( 
    T zero_tolerance,
    int Bsize,
    const T* B,
    T* X
    ) const
{
  int i;

  if ( m_col_count > m_row_count )
    return 0; // under determined

  if ( Bsize < m_col_count || Bsize > m_row_count )
    return 0; // under determined

  for ( i = m_col_count; i < Bsize; i++ ) {
    if ( fabs(B[i]) > zero_tolerance )
      return 0; // over determined
  }

  // backsolve
  T*const* them = m_row.Array();
  const int n = m_col_count-1;
  for ( i = n; i >= 0; i-- ) {
	  X[i] = B[i] - Ti_Math::ArrayDotProduct( n-i, &them[i][i+1], &X[i+1]);
  }

  return 1;
}
/*
bool
Ti_Matrix<T>::BackSolve( 
    T zero_tolerance,
    int Bsize,
    const Ti_3dPoint* B,
    Ti_3dPoint* X
    ) const
{
  int i, j;

  if ( m_col_count > m_row_count )
    return 0; // under determined

  if ( Bsize < m_col_count || Bsize > m_row_count )
    return 0; // under determined

  for ( i = m_col_count; i < Bsize; i++ ) {
    if ( B[i].MaximumCoordinate() > zero_tolerance )
      return 0; // over determined
  }

  // backsolve
  T*const* this_m = m_row.Array();
  for ( i = m_col_count; i >= 0; i-- ) {
    X[i] = B[i];
    for ( j = i+1; j < m_col_count; j++ ) {
      X[i] -= this_m[i][j]*X[j];
    }
  }

  return 1;
}*/

template <class T>
void Ti_Matrix<T>::Zero()
{
  m_a.Zero();
}

template <class T>
void Ti_Matrix<T>::SetDiagonal( T d)
{
  const int n = MinCount();
  int i;
  Zero();
  T** this_m = m_row.Array();
  for ( i = 0; i < n; i++ ) {
    this_m[i][i] = d;
  }
}

template <class T>
void Ti_Matrix<T>::SetDiagonal( const T* d )
{
  Zero();
  if (d) {
    T** this_m = m_row.Array();
    const int n = MinCount();
    int i;
    for ( i = 0; i < n; i++ ) {
      this_m[i][i] = *d++;
    }
  }
}

template <class T>
void Ti_Matrix<T>::SetDiagonal( int count, const T* d )
{
  Create(count,count);
  Zero();
  SetDiagonal(d);
}


template <class T>
bool Ti_Matrix<T>::IsValid() const
{
  if ( m_row_count < 1 || m_col_count < 1 )
    return 0;
  if ( !m )
    return 0;
  return 1;
}

/*template <class T>
void Ti_Matrix<T>::SetDiagonal(const Ti_SimpleArray<T>&  a)
{
  SetDiagonal( a.Count(), a.Array() );
}*/

template <class T>
int Ti_Matrix<T>::IsSquare() const
{
  return ( m_row_count > 0 && m_col_count == m_row_count ) ? m_row_count : 0;
}

template <class T>
bool Ti_Matrix<T>::IsRowOrthoganal() const
{
  T d0, d1, d;
  int i0, i1, j;
  T*const* this_m = m_row.Array();
  bool rc = ( m_row_count <= m_col_count && m_row_count > 0 );
  for ( i0 = 0; i0 < m_row_count && rc; i0++ ) for ( i1 = i0+1; i1 < m_row_count && rc; i1++ ) {
    d0 = d1 = d = 0.0;
    for ( j = 0; j < m_col_count; j++ ) {
      d0 += fabs(this_m[i0][j]);
      d1 += fabs(this_m[i0][j]);
      d  += this_m[i0][j]*this_m[i1][j];
    }
    if ( d0 <=  ON_EPSILON || d1 <=  ON_EPSILON || fabs(d) >= d0*d1* ON_SQRT_EPSILON )
      rc = 0;
  }
  return rc;
}

template <class T>
bool Ti_Matrix<T>::IsRowOrthoNormal() const
{
  T d;
  int i, j;
  bool rc = IsRowOrthoganal();
  if ( rc ) {
    T*const* this_m = m_row.Array();
    for ( i = 0; i < m_row_count; i++ ) {
      d = 0.0;
      for ( j = 0; j < m_col_count; j++ ) {
        d += this_m[i][j]*this_m[i][j];
      }
      if ( fabs(1.0-d) >=  ON_SQRT_EPSILON )
        rc = 0;
    }
  }
  return rc;
}

template <class T>
bool Ti_Matrix<T>::IsColOrthoganal() const
{
  T d0, d1, d;
  int i, j0, j1;
  bool rc = ( m_col_count <= m_row_count && m_col_count > 0 );
  T*const* this_m = m_row.Array();
  for ( j0 = 0; j0 < m_col_count && rc; j0++ )
	  for ( j1 = j0+1; j1 < m_col_count && rc; j1++ )
	  {
    d0 = d1 = d = 0.0;
    for ( i = 0; i < m_row_count; i++ ) {
      d0 += fabs(this_m[i][j0]);
      d1 += fabs(this_m[i][j0]);
      d  += this_m[i][j0]*this_m[i][j1];
    }
    if ( d0 <=  ON_EPSILON || d1 <=  ON_EPSILON || fabs(d) >  ON_SQRT_EPSILON )
      rc = 0;
  }
  return rc;
}

template <class T>
bool Ti_Matrix<T>::IsColOrthoNormal() const
{
  T d;
  int i, j;
  bool rc = IsColOrthoganal();
  T*const* this_m = m_row.Array();
  if ( rc ) {
    for ( j = 0; j < m_col_count; j++ ) {
      d = 0.0;
      for ( i = 0; i < m_row_count; i++ ) {
        d += this_m[i][j]*this_m[i][j];
      }
      if ( fabs(1.0-d) >=  ON_SQRT_EPSILON )
        rc = 0;
    }
  }
  return rc;
}

template <class T>
bool
Ti_Matrix<T>::Invert( double zero_tolerance )
{
  Workspace ws;
  int i, j, k, ix, jx, rank;
  double x;
  const int n = MinCount();
  if ( n < 1 )
    return 0;

  Ti_Matrix<T> I(m_col_count, m_row_count);

  int* col = ws.GetIntMemory(n);

  I.SetDiagonal(1);
  rank = 0;

  T** this_m = m_row.Array();

  for ( k = 0; k < n; k++ ) 
  {
	  // find largest value in sub matrix
	  ix = jx = k;
	  x = fabs(this_m[ix][jx]);
	  for ( i = k; i < n; i++ )
	  {
		  for ( j = k; j < n; j++ ) 
		  {
			  if ( fabs(this_m[i][j]) > x ) 
			  {
				  ix = i;
				  jx = j;
				  x = fabs(this_m[ix][jx]);
			  }
		  }
	  }
	  
	  SwapRows( k, ix );
	  I.SwapRows( k, ix );
	  
	  SwapCols( k, jx );
	  col[k] = jx;
	  
	  if ( x <= zero_tolerance )
	  {
		  break;
	  }
	  x = 1.0/this_m[k][k];
	  this_m[k][k] = 1.0;
	  ON_ArrayScale( m_col_count-k-1, x, &this_m[k][k+1], &this_m[k][k+1] );
	  I.RowScale( k, x );
	  
	  // zero this_m[!=k][k]'s 
	  for ( i = 0; i < n; i++ )
	  {
		  if ( i != k ) 
		  {
			  x = -this_m[i][k];
			  this_m[i][k] = 0.0;
			  if ( fabs(x) > zero_tolerance ) 
			  {
				  ON_Array_aA_plus_B( m_col_count-k-1, x, &this_m[k][k+1], &this_m[i][k+1], &this_m[i][k+1] );
				  I.RowOp( i, x, k );
			  }
		  }
	  }
  }

  // take care of column swaps
  for ( i = k-1; i >= 0; i-- ) 
  {
    if ( i != col[i] )
      I.SwapRows(i,col[i]);
  }

  *this = I;

  return (k == n) ? 1 : 0;
}

// this=A*B
template <class T>
bool Ti_Matrix<T>::Multiply( const Ti_Matrix& a, const Ti_Matrix& b )
{
  int i, j, k, mult_count;
  T x;

  if (a.ColCount() != b.RowCount() )
			  return 0;
  if ( a.RowCount() < 1 || a.ColCount() < 1 || b.ColCount() < 1 )
			  return 0;
  if ( this == &a ) {
    Ti_Matrix tmp(a);
    return Multiply(tmp,b);
  }
  if ( this == &b ) {
    Ti_Matrix tmp(b);
    return Multiply(a,tmp);
  }
 
  Create( a.RowCount(), b.ColCount() );
  mult_count = a.ColCount();
  T*const* am = a.m_row.Array();
  T*const* bm = b.m_row.Array();
  T** this_m = m_row.Array();
  for ( i = 0; i < m_row_count; i++ ) 
	  for ( j = 0; j < m_col_count; j++ ) 
	  {
		  x = 0;
		  for (k = 0; k < mult_count; k++ ) 
		  {
			  x += am[i][k] * bm[k][j];
		  }
		  this_m[i][j] = x;
	  }
  
  return 1;  
}


template <class T>
bool Ti_Matrix<T>::Add( const Ti_Matrix& a, const Ti_Matrix& b )
{
  int i, j;
  if (a.ColCount() != b.ColCount() )
    return 0;
  if (a.RowCount() != b.RowCount() )
    return 0;
  if ( a.RowCount() < 1 || a.ColCount() < 1 )
    return 0;
  if ( this != &a && this != &b ) {
    Create( a.RowCount(), b.ColCount() );
  }
  T*const* am = a.m_row.Array();
  T*const* bm = b.m_row.Array();
  T** this_m = m_row.Array();
  for ( i = 0; i < m_row_count; i++ ) for ( j = 0; j < m_col_count; j++ ) {
    this_m[i][j] = am[i][j] + bm[i][j];
  }
  return 1;  
}

template <class T>
bool Ti_Matrix<T>::Scale( double s )
{
  bool rc = 0;
  int i = m_a.Capacity();
  if ( m_row_count > 0 && m_col_count > 0 && m_row_count*m_col_count <= i ) {
    T* p = m_a.Array();
    while ( i-- )
      *p++ *= s;
    rc = 1;
  }
  return rc;
}

template <class T>
void Ti_Matrix<T>::ON_ArrayScale(int dim, double s, const T *A, T *sA)
{
  if ( dim > 0 ) {
    while ( dim-- )
      *sA++ = s * *A++;
  }
}

template <class T>
void Ti_Matrix<T>::ON_Array_aA_plus_B(int dim, double a, const T *A, const T *B, T *aA_plus_B)
{
   if ( dim > 0 ) {
    while ( dim-- )
      *aA_plus_B++ = a * *A++ + *B++;
  }
}

/*bool Ti_Matrix<T>::BackSolve(double zero_tolerance, int Bsize, const Ti_3dPoint * B, Ti_3dPoint *X)
{
 int i, j;

  if ( m_col_count > m_row_count )
    return FALSE; // under determined

  if ( Bsize < m_col_count || Bsize > m_row_count )
    return FALSE; // under determined

  for ( i = m_col_count; i < Bsize; i++ ) {
    if ( B[i].MaximumCoordinate() > zero_tolerance )
      return FALSE; // over determined
 }

  // backsolve
  T*const* this_m = m_row.Array();
  for ( i = m_col_count; i >= 0; i-- ) {
    X[i] = B[i];
    for ( j = i+1; j < m_col_count; j++ ) {
      X[i] -= this_m[i][j]*X[j];
    }
  }

  return TRUE;
}
*/

template <class T>
T Ti_Matrix<T>::ArrayDot(int dim, const T *A, const T *B)
{
T AoB;
  // do low dimensional cases on one line so we get 80 bit
  // intermediate precision in optimized mode.
  if (dim==1) return (A[0]*B[0]);
  if (dim==2) return (A[0]*B[0] + A[1]*B[1]);
  if (dim==3) return (A[0]*B[0] + A[1]*B[1] + A[2]*B[2]);
  if (dim==4) return (A[0]*B[0] + A[1]*B[1] + A[2]*B[2] +A[3]*B[3]);
  AoB = 0.0;
  while (dim--) AoB += *A++ * *B++;
 A-=dim;
 B-=dim;
 return AoB;
}

//计算行列式
template<class T>
bool Ti_Matrix<T>::Det(T&result)
{
	int i,j,n,k,is,js,l,u,v;
    T f,q,d;
	if(!IsSquare())
		return FALSE;
	T* a0=new T[RowCount()*RowCount()];
	for( i=0;i<RowCount();i++)
		for( j=0;j<ColCount();j++)
			a0[i*ColCount()+j]=this->m[i][j];
		
		f=1;
		result=1;n=RowCount();
		for (k=0; k<=n-2; k++)
		{ 
			q=0.0;
			for (i=k; i<=n-1; i++)
				for (j=k; j<=n-1; j++)
				{ 
					l=i*n+j; 
					d=fabs(a0[l]);
					if (d>q) 
					{ q=d; is=i; js=j;}
				}
				if (q+1.0==1.0)
				{ 
					result=0.0;
					delete []a0;
					return(TRUE);
				}
				if (is!=k)
				{ 
					f=-f;
					for (j=k; j<=n-1; j++)
					{ 
						u=k*n+j;
						v=is*n+j;
						d=a0[u]; 
						a0[u]=a0[v];
						a0[v]=d;
					}
				}
				if (js!=k)
				{ 
					f=-f;
					for (i=k; i<=n-1; i++)
					{ 
						u=i*n+js; v=i*n+k;
						d=a0[u]; a0[u]=a0[v]; a0[v]=d;
					}
				}
				l=k*n+k;
				result=result*a0[l];
				for (i=k+1; i<=n-1; i++)
				{ 
					d=a0[i*n+k]/a0[l];
					for (j=k+1; j<=n-1; j++)
					{ u=i*n+j;
					a0[u]=a0[u]-d*a0[k*n+j];
					}
				}
		}
		result=f*result*a0[n*n-1];
		delete []a0;
		return(TRUE);

}

template<class T>
void Ti_Matrix<T>:: SetColCount( int n) 
{
	m_col_count=n;
}

template<class T>
void Ti_Matrix<T>:: SetRowCount( int n) 
{
	m_row_count=n;
}

template<class T>
T  Ti_Matrix<T>:: Normal() const
{
	T sum=0;
	int i,j;
	for( i=0;i<RowCount();i++)
		for( j=0;j<ColCount();j++)
			sum+=m[i][j]*m[i][j];
	return sum;
}

template<class T>
Ti_Matrix<T> Ti_Matrix<T>::operator+(Ti_Matrix& b) const
{	
	//if(this->MinCount()!=b.MinCount()&&this->MaxCount()!=b.MaxCount())
	//	return 0;
	int i,j;
	for( i=0;i<RowCount();i++)
		for(j=0;j<ColCount();j++)
			m[i][j]=m[i][j]+b.m[i][j];
	return *this;
}

template<class T>
Ti_Matrix<T> Ti_Matrix<T>::operator-(Ti_Matrix& b) const
{	
	//if(this->MinCount()!=b.MinCount()&&this->MaxCount()!=b.MaxCount())
	//	return 0;
	Ti_Matrix<T> c(b);
	int i,j;
	for( i=0;i<RowCount();i++)
		for(j=0;j<ColCount();j++)
			c.m[i][j]=m[i][j]-b.m[i][j];

	return c;
}

 template<class T>
 T* Ti_Matrix<T>::GetCol(int n) const
 {
#ifdef _DEBUG
	 ASSERT (n<ColCount()&&n>=0);
#endif
	 T* coldata=new T[RowCount()];
	 for(int i=0;i<RowCount();i++)
		 coldata[i]=m[i][n];
	 return coldata;
 }

 template<class T>
 T* Ti_Matrix<T>::GetRow(int n) const
 {
#ifdef _DEBUG
	 ASSERT (n<RowCount()&&n>=0);
#endif
	 return m_row[n];
 }


#define TI_MAX_ROTATIONS 20
#define TI_ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
        a[k][l]=h+s*(g-h*tau);
// Jacobi iteration for the solution of eigenvectors/eigenvalues of a nxn
// real symmetric matrix. Square nxn matrix a; size of matrix in n;
// output eigenvalues in w; and output eigenvectors in v. Resulting
// eigenvalues/vectors are sorted in decreasing order; eigenvectors are
// normalized.
 template<class T>
bool  Ti_Matrix<T>::JacobiN(int n, T *w, Ti_Matrix& v)
 {
	 
	 int i, j, k, iq, ip, numPos;
	 T tresh, theta, tau, t, sm, s, h, g, c, tmp;
     T bspace[4], zspace[4];
     T *b = bspace;
     T *z = zspace;
	 if (m_col_count!=n||m_row_count!=n)
		 return false;
	 
	 
	 // only allocate memory if the matrix is large
	 if (n > 4)
	 {
		 b = new T[n];
		 z = new T[n]; 
	 }
	 
	 // initialize
	 Ti_Matrix<T> U(v);
	 U.SetDiagonal(1);
	 for (ip=0; ip<n; ip++) 
	 {
		 b[ip] = w[ip] = m[ip][ip];
		 z[ip] = 0.0;
	 }
	 
	 // begin rotation sequence
	 for (i=0; i<TI_MAX_ROTATIONS; i++) 
	 {
		 sm = 0.0;
		 for (ip=0; ip<n-1; ip++) 
		 {
			 for (iq=ip+1; iq<n; iq++)
			 {
				 sm += fabs(m[ip][iq]);
			 }
		 }
		 if (sm == 0.0)
		 {
			 break;
		 }
		 
		 if (i < 3)                                // first 3 sweeps
		 {
			 tresh = 0.2*sm/(n*n);
		 }
		 else
		 {
			 tresh = 0.0;
		 }
		 
		 for (ip=0; ip<n-1; ip++) 
		 {
			 for (iq=ip+1; iq<n; iq++) 
			 {
				 g = 100.0*fabs(m[ip][iq]);
				 
				 // after 4 sweeps
				 if (i > 3 && (fabs(w[ip])+g) == fabs(w[ip])
					 && (fabs(w[iq])+g) == fabs(w[iq]))
				 {
					 m[ip][iq] = 0.0;
				 }
				 else if (fabs(m[ip][iq]) > tresh) 
				 {
					 h = w[iq] - w[ip];
					 if ( (fabs(h)+g) == fabs(h))
					 {
						 t = (m[ip][iq]) / h;
					 }
					 else 
					 {
						 theta = 0.5*h / (m[ip][iq]);
						 t = 1.0 / (fabs(theta)+sqrt(1.0+theta*theta));
						 if (theta < 0.0)
						 {
							 t = -t;
						 }
					 }
					 c = 1.0 / sqrt(1+t*t);
					 s = t*c;
					 tau = s/(1.0+c);
					 h = t*m[ip][iq];
					 z[ip] -= h;
					 z[iq] += h;
					 w[ip] -= h;
					 w[iq] += h;
					 m[ip][iq]=0.0;
					 
					 // ip already shifted left by 1 unit
					 for (j = 0;j <= ip-1;j++) 
					 {
						 TI_ROTATE(m,j,ip,j,iq)
					 }
					 // ip and iq already shifted left by 1 unit
					 for (j = ip+1;j <= iq-1;j++) 
					 {
						 TI_ROTATE(m,ip,j,j,iq)
					 }
					 // iq already shifted left by 1 unit
					 for (j=iq+1; j<n; j++) 
					 {
						 TI_ROTATE(m,ip,j,iq,j)
					 }
					 for (j=0; j<n; j++) 
					 {
						 TI_ROTATE(U.m,j,ip,j,iq)
					 }
				 }
			 }
		 }
		 
		 for (ip=0; ip<n; ip++) 
		 {
			 b[ip] += z[ip];
			 w[ip] = b[ip];
			 z[ip] = 0.0;
		 }
	 }
	 
	 //// this is NEVER called
	 if ( i >= TI_MAX_ROTATIONS )
	 {
		 //GenericWarningMacro(
		 //   "Ti_Math::Jacobi: Error extracting eigenfunctions");
		 return 0;
	 }
	 
	 // sort eigenfunctions                 these changes do not affect accuracy 
	 for (j=0; j<n-1; j++)                  // boundary incorrect
	 {
		 k = j;
		 tmp = w[k];
		 for (i=j+1; i<n; i++)                // boundary incorrect, shifted already
		 {
			 if (w[i] >= tmp)                   // why exchage if same?
			 {
				 k = i;
				 tmp = w[k];
			 }
		 }
		 if (k != j) 
		 {
			 w[k] = w[j];
			 w[j] = tmp;
			 for (i=0; i<n; i++) 
			 {
				 tmp = U.m[i][j];
				 U.m[i][j] = U.m[i][k];
				 U.m[i][k] = tmp;
			 }
		 }
	 }
	 // insure eigenvector consistency (i.e., Jacobi can compute vectors that
	 // are negative of one another (.707,.707,0) and (-.707,-.707,0). This can
	 // reek havoc in hyperstreamline/other stuff. We will select the most
	 // positive eigenvector.
	 for (j=0; j<n; j++)
	 {
		 for (numPos=0, i=0; i<n; i++)
		 {
			 if ( U.m[i][j] >= 0.0 )
			 {
				 numPos++;
			 }
		 }
		 if ( numPos < ceil(double(n)/double(2.0)) )
		 {
			 for(i=0; i<n; i++)
			 {
				 U.m[i][j] *= -1.0;
			 }
		 }
	 }
	 
	 if (n > 4)
	 {
		 delete [] b;
		 delete [] z;
	 }
	 v=U;
	 U.Destroy();
	 return 1;
 }
#undef TI_MAX_ROTATIONS
#undef TI_ROTATE


#define TI_SMALL_NUMBER 1.0e-12

// Factor linear equations Ax = b using LU decompostion A = LU where L is
// lower triangular matrix and U is upper triangular matrix. Input is 
// square matrix A, integer array of pivot indices index[0->n-1], and size
// of square matrix n. Output factorization LU is in matrix A. If error is 
// found, method returns 0.
//------------------------------------------------------------------
// For thread safe, temporary memory array tmpSize of length size
// must be passed in.
 template<class T>
bool Ti_Matrix<T>::LUFactorLinearSystem(Ti_Matrix& A, int *index, int size,
				  T *tmpSize)
{
  int i, j, k;
  int maxI = 0;
  double largest, temp1, temp2, sum;

  if(m_col_count!=m_row_count||m_row_count!=size)
	  return FALSE;

  //
  // Loop over rows to get implicit scaling information
  //
  for ( i = 0; i < size; i++ ) 
    {
    for ( largest = 0.0, j = 0; j < size; j++ ) 
      {
      if ( (temp2 = fabs(m[i][j])) > largest )
	{
	largest = temp2;
	}
      }

    if ( largest == 0.0 )
      {
      return 0;
      }
      tmpSize[i] = 1.0 / largest;
    }
  //
  // Loop over all columns using Crout's method
  //
  for ( j = 0; j < size; j++ ) 
    {
    for (i = 0; i < j; i++) 
      {
      sum = m[i][j];
      for ( k = 0; k < i; k++ )
	{
	sum -= m[i][k] * m[k][j];
	}
      A.m[i][j] = sum;
      }
    //
    // Begin search for largest pivot element
    //
    for ( largest = 0.0, i = j; i < size; i++ ) 
      {
      sum = A.m[i][j];
      for ( k = 0; k < j; k++ )
	{
	sum -= A.m[i][k] * A.m[k][j];
	}
      A.m[i][j] = sum;

      if ( (temp1 = tmpSize[i]*fabs(sum)) >= largest ) 
        {
        largest = temp1;
        maxI = i;
        }
      }
    //
    // Check for row interchange
    //
    if ( j != maxI ) 
      {
      for ( k = 0; k < size; k++ ) 
        {
        temp1 = A.m[maxI][k];
        A.m[maxI][k] = A.m[j][k];
        A.m[j][k] = temp1;
        }
      tmpSize[maxI] = tmpSize[j];
      }
    //
    // Divide by pivot element and perform elimination
    //
    index[j] = maxI;

    if ( fabs(A.m[j][j]) <= TI_SMALL_NUMBER )
      {
      return 0;
      }

    if ( j != (size-1) ) 
      {
      temp1 = 1.0 / A.m[j][j];
      for ( i = j + 1; i < size; i++ )
	{
	A.m[i][j] *= temp1;
	}
      }
    }

  return TRUE;
}

#undef TI_SMALL_NUMBER
// Solve linear equations Ax = b using LU decompostion A = LU where L is
// lower triangular matrix and U is upper triangular matrix. Input is 
// load vector x[0->n-1], and size of square matrix n.  Also, solution
// vector is written directly over input load vector.
template<class T>
bool Ti_Matrix<T>::LUSolveLinearSystem(double *x, int size)
{
	if (m_col_count!=m_row_count||size!=m_row_count)
		return FALSE;
	Ti_Matrix<T> A(m_row_count,m_col_count);
	int * index= new int[size];
	double * tmpsize= new double[size];
	if (!LUFactorLinearSystem(A,index,size, tmpsize))
	{
		delete []index;
		delete []tmpsize;
		return FALSE;
	}


	 int i, j, ii, idx;
     double sum;
//
// Proceed with forward and backsubstitution for L and U
// matrices.  First, forward substitution.
//
    for ( ii = -1, i = 0; i < size; i++ ) 
    {
    idx = index[i];
    sum = x[idx];
    x[idx] = x[i];

    if ( ii >= 0 )
      {
      for ( j = ii; j <= (i-1); j++ )
	{
	sum -= A.m[i][j]*x[j];
	}
      }
    else if (sum)
      {
      ii = i;
      }

    x[i] = sum;
  }
//
// Now, back substitution
//
  for ( i = size-1; i >= 0; i-- ) 
    {
    sum = x[i];
    for ( j = i + 1; j < size; j++ )
      {
      sum -= A.m[i][j]*x[j];
      }
    x[i] = sum / A.m[i][i];
    }
   delete []index;
   delete []tmpsize;
   return TRUE;
}

//----------------------------------------------------------------------------
//  The solution is based on
//  Berthold K. P. Horn (1987),
//  "Closed-form solution of absolute orientation using unit quaternions,"
//  Journal of the Optical Society of America A, 4:629-642

template<class T>
bool Ti_Matrix<T>::Matrix3x3ToQuaternion(T quat[4])
{
	if (m_col_count!=m_row_count|| m_col_count!=3 )
		return FALSE;
  Ti_Matrix<T> N(4,4);

  // on-diagonal elements
  N.m[0][0] =  m[0][0]+m[1][1]+m[2][2];
  N.m[1][1] =  m[0][0]-m[1][1]-m[2][2];
  N.m[2][2] = -m[0][0]+m[1][1]-m[2][2];
  N.m[3][3] = -m[0][0]-m[1][1]+m[2][2];

  // off-diagonal elements
  N.m[0][1] = N.m[1][0] = m[2][1]-m[1][2];
  N.m[0][2] = N.m[2][0] = m[0][2]-m[2][0];
  N.m[0][3] = N.m[3][0] = m[1][0]-m[0][1];

  N.m[1][2] = N.m[2][1] = m[1][0]+m[0][1];
  N.m[1][3] = N.m[3][1] = m[0][2]+m[2][0];
  N.m[2][3] = N.m[3][2] = m[2][1]+m[1][2];

  Ti_Matrix<T> eigenvectors(4,4);
  T eigenvalues[4];
  
  // convert into format that JacobiN can use,
  // then use Jacobi to find eigenvalues and eigenvectors
  N.JacobiN(4,eigenvalues,eigenvectors);
  
  // the first eigenvector is the one we want
  quat[0] = eigenvectors[0][0];
  quat[1] = eigenvectors[1][0];
  quat[2] = eigenvectors[2][0];
  quat[3] = eigenvectors[3][0];
  N.Destroy();
  eigenvectors.Destroy();
  return TRUE;

}
template<class T>
bool Ti_Matrix<T>::QuaternionToMatrix3x3(T quat[4])
{
  T ww = quat[0]*quat[0];
  T wx = quat[0]*quat[1];
  T wy = quat[0]*quat[2];
  T wz = quat[0]*quat[3];

  T xx = quat[1]*quat[1];
  T yy = quat[2]*quat[2];
  T zz = quat[3]*quat[3];

  T xy = quat[1]*quat[2];
  T xz = quat[1]*quat[3];
  T yz = quat[2]*quat[3];

  T rr = xx + yy + zz;
  // normalization factor, just in case quaternion was not normalized
  T f = (T)(1/( sqrt(ww + rr)) );
  T s = (ww - rr)*f;
  f *= 2;
  if (!Create(3,3))
	  return FALSE;
  m[0][0] = xx*f + s;
  m[1][0] = (xy + wz)*f;
  m[2][0] = (xz - wy)*f;

  m[0][1] = (xy - wz)*f;
  m[1][1] = yy*f + s;
  m[2][1] = (yz + wx)*f;

  m[0][2] = (xz + wy)*f;
  m[1][2] = (yz - wx)*f;
  m[2][2] = zz*f + s;
  return TRUE;
}

//----------------------------------------------------------------------------
//  The orthogonalization is done via quaternions in order to avoid
//  having to use singular value decomposition. you must guarantee that it is rotation matrix
template<class T>
void  Ti_Matrix<T>::Orthogonalize3x3(Ti_Matrix& B)
{
  T quat[4];
#ifdef _DEBUG
  ASSERT(m_col_count==3&&m_row_count==3);
#endif 
  Matrix3x3ToQuaternion(quat);
  B.QuaternionToMatrix3x3(quat);
}

//奇异值分解 A＝U Diag(w) VT
 template<class T>
void  Ti_Matrix<T>:: SingularValueDecomposition3x3(
						Ti_Matrix& A,
	                    Ti_Matrix& U, 
						T w[3],
					    Ti_Matrix& VT
						)
 {
	  A.Orthogonalize3x3(VT);
	  VT.Transpose();
      U.Multiply(VT, A);
      VT.Transpose();
      U.Diagonalize3x3(w, U);
      VT.Multiply(VT, U);
      U.Transpose();
 }
template<class T>
void  Ti_Matrix<T>::Diagonalize3x3(
					 T w[3],      //eigenvalues
					 Ti_Matrix& V //columns are eigenvectors
					 )
{
	int i,j,k,maxI;
    T tmp, maxVal;
	
	// do the matrix[3][3] to **matrix conversion for Jacobi
	
	// diagonalize using Jacobi
    JacobiN(3,w,V);
	
	// if all the eigenvalues are the same, return identity matrix
	if (w[0] == w[1] && w[0] == w[2])
    {
		V.SetDiagonal(1);
		return;
    }
	
	// transpose temporarily, it makes it easier to sort the eigenvectors
	V.Transpose();
	
	// if two eigenvalues are the same, re-orthogonalize to optimally line
	// up the eigenvectors with the x, y, and z axes
	for (i = 0; i < 3; i++)
    {
		if (w[(i+1)%3] == w[(i+2)%3]) // two eigenvalues are the same
		{
			// find maximum element of the independant eigenvector
			maxVal = fabs(V.m[i][0]);
			maxI = 0;
			for (j = 1; j < 3; j++)
			{
				if (maxVal < (tmp = fabs(V.m[i][j])))
				{
					maxVal = tmp;
					maxI = j;
				}
			}
			// swap the eigenvector into its proper position
			if (maxI != i)
			{
				tmp = w[maxI];
				w[maxI] = w[i];
				w[i] = tmp;
				V.SwapRows(i,maxI);
			}
			// maximum element of eigenvector should be positive
			if (V.m[maxI][maxI] < 0)
			{
				V.m[maxI][0] = -V.m[maxI][0];
				V.m[maxI][1] = -V.m[maxI][1];
				V.m[maxI][2] = -V.m[maxI][2];
			}
			
			// re-orthogonalize the other two eigenvectors
			j = (maxI+1)%3;
			k = (maxI+2)%3;
			
			V.m[j][0] = 0.0; 
			V.m[j][1] = 0.0; 
			V.m[j][2] = 0.0;
			V.m[j][j] = 1.0;
			Cross(V.GetRow(maxI),V.GetRow(j),V.GetRow(k));
			Cross(V.GetRow(k),V.GetRow(maxI),V.GetRow(j));
			// transpose vectors back to columns
			V.Transpose();
			return;
		}
    }
	
	// the three eigenvalues are different, just sort the eigenvectors
	// to align them with the x, y, and z axes
	
	// find the vector with the largest x element, make that vector
	// the first vector
	maxVal = fabs(V.m[0][0]);
	maxI = 0;
	for (i = 1; i < 3; i++)
    {
		if (maxVal < (tmp = fabs(V.m[i][0])))
		{
			maxVal = tmp;
			maxI = i;
		}
    }
	// swap eigenvalue and eigenvector
	if (maxI != 0)
    {
		tmp = w[maxI];
		w[maxI] = w[0];
		w[0] = tmp;
		V.SwapRows(maxI,0);
    }
	// do the same for the y element
	if (fabs(V.m[1][1]) < fabs(V.m[2][1]))
    {
		tmp = w[2];
		w[2] = w[1];
		w[1] = tmp;
		V.SwapRows(2,1);
    }
	
	// ensure that the sign of the eigenvectors is correct
	for (i = 0; i < 2; i++)
    {
		if (V.m[i][i] < 0)
		{
			V.m[i][0] = -V.m[i][0];
			V.m[i][1] = -V.m[i][1];
			V.m[i][2] = -V.m[i][2];
		}
    }
	// set sign of final eigenvector to ensure that determinant is positive
	T det;
	V.Det(det);
	if (det < 0)
    {
		V.m[2][0] = -V.m[2][0];
		V.m[2][1] = -V.m[2][1];
		V.m[2][2] = -V.m[2][2];
    }
	
	// transpose the eigenvectors back again
	V.Transpose();
	
}

template<class T>
void  Ti_Matrix<T>::OutProduct(T* v, int n,T* u, int k)
{
	Destroy();
	Create(n,k);
	for (int i=0;i<n; i++)
		for ( int j=0 ;j<k ; j++)
		{
			this->m[i][j]=v[i]*u[j];
		}
}
/*
template<class T>
void Ti_Matrix<T>::OutProduct(const VECTOR v1,const VECTOR v2)
{
	Destroy();
	Create(3,3);
	T a[3],b[3];
	a[0]=(T)(v1.x);
	a[1]=(T)(v1.y);
	a[2]=(T)(v1.z);
	b[0]=(T)(v2.x);
	b[1]=(T)(v2.y);
	b[2]=(T)(v2.z);
	for (int i=0;i<3; i++)
		for ( int j=0 ;j<3 ; j++)
		{
			m[i][j]=a[i]*b[j];
		}
}
*/
// Solves for the least squares best fit matrix for the equation X'M' = Y'.
// Uses pseudoinverse to get the ordinary least squares. 
// The inputs and output are transposed matrices.
//    Dimensions: X' is numberOfSamples by xOrder,
//                Y' is numberOfSamples by yOrder,
//                M' dimension is xOrder by yOrder.
// M' should be pre-allocated. All matrices are row major. The resultant
// matrix M' should be pre-multiplied to X' to get Y', or transposed and
// then post multiplied to X to get Y
template<class T>
 bool Ti_Matrix<T>::SolveLeastSquares(
	  int numberOfSamples,//sample points
	  Ti_Matrix xt,       
	  int xOrder,
	  Ti_Matrix yt, 
	  int yOrder,
	  Ti_Matrix mt
	  )
{
	// check dimensional consistency
  if ((numberOfSamples < xOrder) || (numberOfSamples < yOrder))
    {
    //GenericWarningMacro("Insufficient number of samples. Underdetermined.");
    return 0;
    }

  int i, j, k;

  // set up intermediate variables
  Ti_Matrix<T> X(xt);
  Ti_Matrix<T> XXt(xOrder,xOrder);
  Ti_Matrix<T> XYt(xOrder,yOrder);
  XXt.Zero();
  XYt.Zero();
  X.Transpose();
  XXt.Multiply(X,xt);
  XYt.Multiply(X,yt);

  
  // next get the inverse of XXt
  if (!(XXt.Invert(0.0005)))
    {
    return 0;
    }
  mt.Multiply(XXt,XYt);
 
  XXt.Destroy();
  XYt.Destroy();
  X.Destroy();
  return 1;
}

template<class T>
bool Ti_Matrix<T>::SVD(
	  double eps,//the matrix to be decompostion
	  Ti_Matrix& U, 
	  Ti_Matrix& V
	    	)
{
	int ka;
	int i,j,k,l,it,ll,kk,ix,iy,mm,nn,iz,m1,ks;
	int m,n;
    T d,dd,t,sm,sm1,em1,sk,ek,b,c,shh,fg[2],cs[2];
    T *s,*e,*w;
	T *a,*u,*v;
	
	m = m_row_count;
	n = m_col_count;
	if (m > n )
		ka = m+1;
	else
		ka = n+1;
	U.Destroy();
	V.Destroy();
	U.Create(m,m);
	V.Create(n,n );
	U.Zero();
	V.Zero();
	a=&(this->m[0][0]);
	u=&(U.m[0][0]);
	v=&(V.m[0][0]);
    s=(T*)malloc(ka*sizeof(T));
    e=(T*)malloc(ka*sizeof(T));
    w=(T*)malloc(ka*sizeof(T));
    it=60; k=n;
    if (m-1<n) k=m-1;
    l=m;
    if (n-2<m) l=n-2;
    if (l<0) l=0;
    ll=k;
    if (l>k) ll=l;
    if (ll>=1)
      { for (kk=1; kk<=ll; kk++)
          { if (kk<=k)
              { d=0.0;
                for (i=kk; i<=m; i++)
                  { ix=(i-1)*n+kk-1; d=d+a[ix]*a[ix];}
                s[kk-1]=sqrt(d);
                if (s[kk-1]!=0.0)
                  { ix=(kk-1)*n+kk-1;
                    if (a[ix]!=0.0)
                      { s[kk-1]=fabs(s[kk-1]);
                        if (a[ix]<0.0) s[kk-1]=-s[kk-1];
                      }
                    for (i=kk; i<=m; i++)
                      { iy=(i-1)*n+kk-1;
                        a[iy]=a[iy]/s[kk-1];
                      }
                    a[ix]=1.0+a[ix];
                  }
                s[kk-1]=-s[kk-1];
              }
            if (n>=kk+1)
              { for (j=kk+1; j<=n; j++)
                  { if ((kk<=k)&&(s[kk-1]!=0.0))
                      { d=0.0;
                        for (i=kk; i<=m; i++)
                          { ix=(i-1)*n+kk-1;
                            iy=(i-1)*n+j-1;
                            d=d+a[ix]*a[iy];
                          }
                        d=-d/a[(kk-1)*n+kk-1];
                        for (i=kk; i<=m; i++)
                          { ix=(i-1)*n+j-1;
                            iy=(i-1)*n+kk-1;
                            a[ix]=a[ix]+d*a[iy];
                          }
                      }
                    e[j-1]=a[(kk-1)*n+j-1];
                  }
              }
            if (kk<=k)
              { for (i=kk; i<=m; i++)
                  { ix=(i-1)*m+kk-1; iy=(i-1)*n+kk-1;
                    u[ix]=a[iy];
                  }
              }
            if (kk<=l)
              { d=0.0;
                for (i=kk+1; i<=n; i++)
                  d=d+e[i-1]*e[i-1];
                e[kk-1]=sqrt(d);
                if (e[kk-1]!=0.0)
                  { if (e[kk]!=0.0)
                      { e[kk-1]=fabs(e[kk-1]);
                        if (e[kk]<0.0) e[kk-1]=-e[kk-1];
                      }
                    for (i=kk+1; i<=n; i++)
                      e[i-1]=e[i-1]/e[kk-1];
                    e[kk]=1.0+e[kk];
                  }
                e[kk-1]=-e[kk-1];
                if ((kk+1<=m)&&(e[kk-1]!=0.0))
                  { for (i=kk+1; i<=m; i++) w[i-1]=0.0;
                    for (j=kk+1; j<=n; j++)
                      for (i=kk+1; i<=m; i++)
                        w[i-1]=w[i-1]+e[j-1]*a[(i-1)*n+j-1];
                    for (j=kk+1; j<=n; j++)
                      for (i=kk+1; i<=m; i++)
                        { ix=(i-1)*n+j-1;
                          a[ix]=a[ix]-w[i-1]*e[j-1]/e[kk];
                        }
                  }
                for (i=kk+1; i<=n; i++)
                  v[(i-1)*n+kk-1]=e[i-1];
              }
          }
      }
    mm=n;
    if (m+1<n) mm=m+1;
    if (k<n) s[k]=a[k*n+k];
    if (m<mm) s[mm-1]=0.0;
    if (l+1<mm) e[l]=a[l*n+mm-1];
    e[mm-1]=0.0;
    nn=m;
    if (m>n) nn=n;
    if (nn>=k+1)
      { for (j=k+1; j<=nn; j++)
          { for (i=1; i<=m; i++)
              u[(i-1)*m+j-1]=0.0;
            u[(j-1)*m+j-1]=1.0;
          }
      }
    if (k>=1)
      { for (ll=1; ll<=k; ll++)
          { kk=k-ll+1; iz=(kk-1)*m+kk-1;
            if (s[kk-1]!=0.0)
              { if (nn>=kk+1)
                  for (j=kk+1; j<=nn; j++)
                    { d=0.0;
                      for (i=kk; i<=m; i++)
                        { ix=(i-1)*m+kk-1;
                          iy=(i-1)*m+j-1;
                          d=d+u[ix]*u[iy]/u[iz];
                        }
                      d=-d;
                      for (i=kk; i<=m; i++)
                        { ix=(i-1)*m+j-1;
                          iy=(i-1)*m+kk-1;
                          u[ix]=u[ix]+d*u[iy];
                        }
                    }
                  for (i=kk; i<=m; i++)
                    { ix=(i-1)*m+kk-1; u[ix]=-u[ix];}
                  u[iz]=1.0+u[iz];
                  if (kk-1>=1)
                    for (i=1; i<=kk-1; i++)
                      u[(i-1)*m+kk-1]=0.0;
              }
            else
              { for (i=1; i<=m; i++)
                  u[(i-1)*m+kk-1]=0.0;
                u[(kk-1)*m+kk-1]=1.0;
              }
          }
      }
    for (ll=1; ll<=n; ll++)
      { kk=n-ll+1; iz=kk*n+kk-1;
        if ((kk<=l)&&(e[kk-1]!=0.0))
          { for (j=kk+1; j<=n; j++)
              { d=0.0;
                for (i=kk+1; i<=n; i++)
                  { ix=(i-1)*n+kk-1; iy=(i-1)*n+j-1;
                    d=d+v[ix]*v[iy]/v[iz];
                  }
                d=-d;
                for (i=kk+1; i<=n; i++)
                  { ix=(i-1)*n+j-1; iy=(i-1)*n+kk-1;
                    v[ix]=v[ix]+d*v[iy];
                  }
              }
          }
        for (i=1; i<=n; i++)
          v[(i-1)*n+kk-1]=0.0;
        v[iz-n]=1.0;
      }
    for (i=1; i<=m; i++)
    for (j=1; j<=n; j++)
      a[(i-1)*n+j-1]=0.0;
    m1=mm; it=60;
    while (1==1)
      { if (mm==0)
          { 
			{ int i,j,p,q;
    double d;
    if (m>=n) i=n;
    else i=m;
    for (j=1; j<=i-1; j++)
      { a[(j-1)*n+j-1]=s[j-1];
        a[(j-1)*n+j]=e[j-1];
      }
    a[(i-1)*n+i-1]=s[i-1];
    if (m<n) a[(i-1)*n+i]=e[i-1];
    for (i=1; i<=n-1; i++)
    for (j=i+1; j<=n; j++)
      { p=(i-1)*n+j-1; q=(j-1)*n+i-1;
        d=v[p]; v[p]=v[q]; v[q]=d;
      }
  }
            free(s); free(e); free(w); return(1);
          }
        if (it==0)
          { 
			{ int i,j,p,q;
    double d;
    if (m>=n) i=n;
    else i=m;
    for (j=1; j<=i-1; j++)
      { a[(j-1)*n+j-1]=s[j-1];
        a[(j-1)*n+j]=e[j-1];
      }
    a[(i-1)*n+i-1]=s[i-1];
    if (m<n) a[(i-1)*n+i]=e[i-1];
    for (i=1; i<=n-1; i++)
    for (j=i+1; j<=n; j++)
      { p=(i-1)*n+j-1; q=(j-1)*n+i-1;
        d=v[p]; v[p]=v[q]; v[q]=d;
      }
  }
            free(s); free(e); free(w); return(-1);
          }
        kk=mm-1;
	while ((kk!=0)&&(fabs(e[kk-1])!=0.0))
          { d=fabs(s[kk-1])+fabs(s[kk]);
            dd=fabs(e[kk-1]);
            if (dd>eps*d) kk=kk-1;
            else e[kk-1]=0.0;
          }
        if (kk==mm-1)
          { kk=kk+1;
            if (s[kk-1]<0.0)
              { s[kk-1]=-s[kk-1];
                for (i=1; i<=n; i++)
                  { ix=(i-1)*n+kk-1; v[ix]=-v[ix];}
              }
            while ((kk!=m1)&&(s[kk-1]<s[kk]))
              { d=s[kk-1]; s[kk-1]=s[kk]; s[kk]=d;
                if (kk<n)
                  for (i=1; i<=n; i++)
                    { ix=(i-1)*n+kk-1; iy=(i-1)*n+kk;
                      d=v[ix]; v[ix]=v[iy]; v[iy]=d;
                    }
                if (kk<m)
                  for (i=1; i<=m; i++)
                    { ix=(i-1)*m+kk-1; iy=(i-1)*m+kk;
                      d=u[ix]; u[ix]=u[iy]; u[iy]=d;
                    }
                kk=kk+1;
              }
            it=60;
            mm=mm-1;
          }
        else
          { ks=mm;
            while ((ks>kk)&&(fabs(s[ks-1])!=0.0))
              { d=0.0;
                if (ks!=mm) d=d+fabs(e[ks-1]);
                if (ks!=kk+1) d=d+fabs(e[ks-2]);
                dd=fabs(s[ks-1]);
                if (dd>eps*d) ks=ks-1;
                else s[ks-1]=0.0;
              }
            if (ks==kk)
              { kk=kk+1;
                d=fabs(s[mm-1]);
                t=fabs(s[mm-2]);
                if (t>d) d=t;
                t=fabs(e[mm-2]);
                if (t>d) d=t;
                t=fabs(s[kk-1]);
                if (t>d) d=t;
                t=fabs(e[kk-1]);
                if (t>d) d=t;
                sm=s[mm-1]/d; sm1=s[mm-2]/d;
                em1=e[mm-2]/d;
                sk=s[kk-1]/d; ek=e[kk-1]/d;
                b=((sm1+sm)*(sm1-sm)+em1*em1)/2.0;
                c=sm*em1; c=c*c; shh=0.0;
                if ((b!=0.0)||(c!=0.0))
                  { shh=sqrt(b*b+c);
                    if (b<0.0) shh=-shh;
                    shh=c/(b+shh);
                  }
                fg[0]=(sk+sm)*(sk-sm)-shh;
                fg[1]=sk*ek;
                for (i=kk; i<=mm-1; i++)
                  { 
					{ double r,d;
    if ((fabs(fg[0])+fabs(fg[1]))==0.0)
      { cs[0]=1.0; cs[1]=0.0; d=0.0;}
    else 
      { d=sqrt(fg[0]*fg[0]+fg[1]*fg[1]);
        if (fabs(fg[0])>fabs(fg[1]))
          { d=fabs(d);
            if (fg[0]<0.0) d=-d;
          }
        if (fabs(fg[1])>=fabs(fg[0]))
          { d=fabs(d);
            if (fg[1]<0.0) d=-d;
          }
        cs[0]=fg[0]/d; cs[1]=fg[1]/d;
      }
    r=1.0;
    if (fabs(fg[0])>fabs(fg[1])) r=cs[1];
    else
      if (cs[0]!=0.0) r=1.0/cs[0];
    fg[0]=d; fg[1]=r;
  }
                    if (i!=kk) e[i-2]=fg[0];
                    fg[0]=cs[0]*s[i-1]+cs[1]*e[i-1];
                    e[i-1]=cs[0]*e[i-1]-cs[1]*s[i-1];
                    fg[1]=cs[1]*s[i];
                    s[i]=cs[0]*s[i];
                    if ((cs[0]!=1.0)||(cs[1]!=0.0))
                      for (j=1; j<=n; j++)
                        { ix=(j-1)*n+i-1;
                          iy=(j-1)*n+i;
                          d=cs[0]*v[ix]+cs[1]*v[iy];
                          v[iy]=-cs[1]*v[ix]+cs[0]*v[iy];
                          v[ix]=d;
                        }
{ double r,d;
    if ((fabs(fg[0])+fabs(fg[1]))==0.0)
      { cs[0]=1.0; cs[1]=0.0; d=0.0;}
    else 
      { d=sqrt(fg[0]*fg[0]+fg[1]*fg[1]);
        if (fabs(fg[0])>fabs(fg[1]))
          { d=fabs(d);
            if (fg[0]<0.0) d=-d;
          }
        if (fabs(fg[1])>=fabs(fg[0]))
          { d=fabs(d);
            if (fg[1]<0.0) d=-d;
          }
        cs[0]=fg[0]/d; cs[1]=fg[1]/d;
      }
    r=1.0;
    if (fabs(fg[0])>fabs(fg[1])) r=cs[1];
    else
      if (cs[0]!=0.0) r=1.0/cs[0];
    fg[0]=d; fg[1]=r;
  }           
         s[i-1]=fg[0];
                    fg[0]=cs[0]*e[i-1]+cs[1]*s[i];
                    s[i]=-cs[1]*e[i-1]+cs[0]*s[i];
                    fg[1]=cs[1]*e[i];
                    e[i]=cs[0]*e[i];
                    if (i<m)
                      if ((cs[0]!=1.0)||(cs[1]!=0.0))
                        for (j=1; j<=m; j++)
                          { ix=(j-1)*m+i-1;
                            iy=(j-1)*m+i;
                            d=cs[0]*u[ix]+cs[1]*u[iy];
                            u[iy]=-cs[1]*u[ix]+cs[0]*u[iy];
                            u[ix]=d;
                          }
                  }
                e[mm-2]=fg[0];
                it=it-1;
              }
            else
              { if (ks==mm)
                  { kk=kk+1;
                    fg[1]=e[mm-2]; e[mm-2]=0.0;
                    for (ll=kk; ll<=mm-1; ll++)
                      { i=mm+kk-ll-1;
                        fg[0]=s[i-1];
 { double r,d;
    if ((fabs(fg[0])+fabs(fg[1]))==0.0)
      { cs[0]=1.0; cs[1]=0.0; d=0.0;}
    else 
      { d=sqrt(fg[0]*fg[0]+fg[1]*fg[1]);
        if (fabs(fg[0])>fabs(fg[1]))
          { d=fabs(d);
            if (fg[0]<0.0) d=-d;
          }
        if (fabs(fg[1])>=fabs(fg[0]))
          { d=fabs(d);
            if (fg[1]<0.0) d=-d;
          }
        cs[0]=fg[0]/d; cs[1]=fg[1]/d;
      }
    r=1.0;
    if (fabs(fg[0])>fabs(fg[1])) r=cs[1];
    else
      if (cs[0]!=0.0) r=1.0/cs[0];
    fg[0]=d; fg[1]=r;
  }                        s[i-1]=fg[0];
                        if (i!=kk)
                          { fg[1]=-cs[1]*e[i-2];
                            e[i-2]=cs[0]*e[i-2];
                          }
                        if ((cs[0]!=1.0)||(cs[1]!=0.0))
                          for (j=1; j<=n; j++)
                            { ix=(j-1)*n+i-1;
                              iy=(j-1)*n+mm-1;
                              d=cs[0]*v[ix]+cs[1]*v[iy];
                              v[iy]=-cs[1]*v[ix]+cs[0]*v[iy];
                              v[ix]=d;
                            }
                      }
                  }
                else
                  { kk=ks+1;
                    fg[1]=e[kk-2];
                    e[kk-2]=0.0;
                    for (i=kk; i<=mm; i++)
                      { fg[0]=s[i-1];
 { double r,d;
    if ((fabs(fg[0])+fabs(fg[1]))==0.0)
      { cs[0]=1.0; cs[1]=0.0; d=0.0;}
    else 
      { d=sqrt(fg[0]*fg[0]+fg[1]*fg[1]);
        if (fabs(fg[0])>fabs(fg[1]))
          { d=fabs(d);
            if (fg[0]<0.0) d=-d;
          }
        if (fabs(fg[1])>=fabs(fg[0]))
          { d=fabs(d);
            if (fg[1]<0.0) d=-d;
          }
        cs[0]=fg[0]/d; cs[1]=fg[1]/d;
      }
    r=1.0;
    if (fabs(fg[0])>fabs(fg[1])) r=cs[1];
    else
      if (cs[0]!=0.0) r=1.0/cs[0];
    fg[0]=d; fg[1]=r;
  }                        s[i-1]=fg[0];
                        fg[1]=-cs[1]*e[i-1];
                        e[i-1]=cs[0]*e[i-1];
                        if ((cs[0]!=1.0)||(cs[1]!=0.0))
                          for (j=1; j<=m; j++)
                            { ix=(j-1)*m+i-1;
                              iy=(j-1)*m+kk-2;
                              d=cs[0]*u[ix]+cs[1]*u[iy];
                              u[iy]=-cs[1]*u[ix]+cs[0]*u[iy];
                              u[ix]=d;
                            }
                      }
                  }
              }
          }
      }
    return(1);
}



////////////////////////////////////////////////
/////by 杨平海 add down

template<class T>
bool Ti_Matrix<T>::DoHermite
			(Ti_Matrix & input,
			 Ti_Matrix & output,
			 int m,int n)
{
	if(m>n) return FALSE;

	int i,j,k;
	T factor;

	for(i=0;i<m;i++)
	{
		output.m[i]=input.m[i];
	}

	for(i=0;i<m;i++)
	{  
		for(j=i;j<m;j++)
		{
			if(fabs(output.m[i][i])<fabs(output.m[j][i]))
				SwapRows(i,j);		
		}
		if(fabs(output.m[i][i])<1e-6) continue;

		for(j=0;j<m;j++)
		{
			if (j==i)	continue;
			factor=output.m[j][i]/output.m[i][i];
			for(k=0;k<n;k++)
				output.m[j][k]-=factor*output.m[i][k];
		}

	}
	double t;
	bool *flag =new bool[m];

	for(i=0;i<m;i++)
	{
		t=0;
		flag[i]=FALSE;
		for(j=0;j<m;j++)
			t+= output.m[i][j]*output.m[i][j];
		if (t>1e-3)
		{
			t=sqrt(t);
			for(j=0;j<n;j++)
				output.m[i][j]/=t;
		}
		else
		{
			flag[i]=TRUE;
		}

	}
	int record=m-1;
	for(i=0;i<record;i++)
	{
		if (flag[i])
		{
			for(int j=record;j>=0;j--)
			{
				if(!flag[j])
				{
				    record--;
					output.SwapRows(i,j);
					flag[j]=TRUE;
					flag[i]=FALSE;
					break;
				}
			}
		}
	}
	delete []flag;
	return TRUE;

}
////////add up
  template<class T>
  void Cross(T* a, T* b, T* c)
	  {
	  	c[0] =	a[1]*b[2] - a[2]*b[1];
		c[1] =   a[2]*b[0] - a[0]*b[2];
		c[2] =  a[0]*b[1] - a[1]*b[0] ;
	  }

  template<class T>
  void Ti_Matrix<T>::DeleteRow(int i)
  {
	  if (i<0||i>RowCount())return ;
	  m_row.Remove(i);
	  m_row_count--;
  }
// end Ti_Matrix.cpp