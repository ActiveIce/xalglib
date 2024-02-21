###########################################################################
# ALGLIB 4.01.0 (source code generated 2023-12-27)
# Copyright (c) Sergey Bochkanov (ALGLIB project).
# 
# >>> SOURCE LICENSE >>>
# This software is a non-commercial edition of  ALGLIB  package,  which  is
# licensed under ALGLIB Personal and Academic Use License Agreement (PAULA).
# 
# See paula-v1.0.pdf file in the archive  root  for  full  text  of  license
# agreement.
# >>> END OF LICENSE >>>

##########################################################################


#include <stdafx.h>
#include "trfac.h"


/*$ Declarations $*/
static ae_bool trfac_hpdmatrixcholeskyrec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* tmp,
     ae_state *_state);
static ae_bool trfac_hpdmatrixcholesky2(/* Complex */ ae_matrix* aaa,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* tmp,
     ae_state *_state);
static ae_bool trfac_spdmatrixcholesky2(/* Real    */ ae_matrix* aaa,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
LU decomposition of a general real matrix with row pivoting

A is represented as A = P*L*U, where:
* L is lower unitriangular matrix
* U is upper triangular matrix
* P = P0*P1*...*PK, K=min(M,N)-1,
  Pi - permutation matrix for I and Pivots[I]
  
INPUT PARAMETERS:
    A       -   array[0..M-1, 0..N-1].
    M       -   number of rows in matrix A.
    N       -   number of columns in matrix A.


OUTPUT PARAMETERS:
    A       -   matrices L and U in compact form:
                * L is stored under main diagonal
                * U is stored on and above main diagonal
    Pivots  -   permutation matrix in compact form.
                array[0..Min(M-1,N-1)].
  
  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     10.01.2010
     Bochkanov Sergey
*************************************************************************/
void rmatrixlu(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state)
{

    ae_vector_clear(pivots);

    ae_assert(m>0, "RMatrixLU: incorrect M!", _state);
    ae_assert(n>0, "RMatrixLU: incorrect N!", _state);
    ae_assert(a->rows>=m, "RMatrixLU: rows(A)<M", _state);
    ae_assert(a->cols>=n, "RMatrixLU: cols(A)<N", _state);
    ae_assert(apservisfinitematrix(a, m, n, _state), "RMatrixLU: A contains infinite or NaN values!", _state);
    rmatrixplu(a, m, n, pivots, _state);
}


/*************************************************************************
LU decomposition of a general complex matrix with row pivoting

A is represented as A = P*L*U, where:
* L is lower unitriangular matrix
* U is upper triangular matrix
* P = P0*P1*...*PK, K=min(M,N)-1,
  Pi - permutation matrix for I and Pivots[I]

INPUT PARAMETERS:
    A       -   array[0..M-1, 0..N-1].
    M       -   number of rows in matrix A.
    N       -   number of columns in matrix A.


OUTPUT PARAMETERS:
    A       -   matrices L and U in compact form:
                * L is stored under main diagonal
                * U is stored on and above main diagonal
    Pivots  -   permutation matrix in compact form.
                array[0..Min(M-1,N-1)].

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     10.01.2010
     Bochkanov Sergey
*************************************************************************/
void cmatrixlu(/* Complex */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state)
{

    ae_vector_clear(pivots);

    ae_assert(m>0, "CMatrixLU: incorrect M!", _state);
    ae_assert(n>0, "CMatrixLU: incorrect N!", _state);
    ae_assert(a->rows>=m, "CMatrixLU: rows(A)<M", _state);
    ae_assert(a->cols>=n, "CMatrixLU: cols(A)<N", _state);
    ae_assert(isfinitecmatrix(a, m, n, _state), "CMatrixLU: A contains infinite or NaN values!", _state);
    cmatrixplu(a, m, n, pivots, _state);
}


/*************************************************************************
Cache-oblivious Cholesky decomposition

The algorithm computes Cholesky decomposition  of  a  Hermitian  positive-
definite matrix. The result of an algorithm is a representation  of  A  as
A=U'*U  or A=L*L' (here X' denotes conj(X^T)).

INPUT PARAMETERS:
    A       -   upper or lower triangle of a factorized matrix.
                array with elements [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper -   if IsUpper=True, then A contains an upper triangle of
                a symmetric matrix, otherwise A contains a lower one.

OUTPUT PARAMETERS:
    A       -   the result of factorization. If IsUpper=True, then
                the upper triangle contains matrix U, so that A = U'*U,
                and the elements below the main diagonal are not modified.
                Similarly, if IsUpper = False.

RESULT:
    If  the  matrix  is  positive-definite,  the  function  returns  True.
    Otherwise, the function returns False. Contents of A is not determined
    in such case.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     15.12.2009-22.01.2018
     Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixcholesky(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tmp;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    ae_vector_init(&tmp, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixCholesky: incorrect N!", _state);
    ae_assert(a->rows>=n, "HPDMatrixCholesky: rows(A)<N", _state);
    ae_assert(a->cols>=n, "HPDMatrixCholesky: cols(A)<N", _state);
    ae_assert(isfinitectrmatrix(a, n, isupper, _state), "HPDMatrixCholesky: A contains infinite or NaN values!", _state);
    result = trfac_hpdmatrixcholeskyrec(a, 0, n, isupper, &tmp, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Cache-oblivious Cholesky decomposition

The algorithm computes Cholesky decomposition  of  a  symmetric  positive-
definite matrix. The result of an algorithm is a representation  of  A  as
A=U^T*U  or A=L*L^T

INPUT PARAMETERS:
    A       -   upper or lower triangle of a factorized matrix.
                array with elements [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper -   if IsUpper=True, then A contains an upper triangle of
                a symmetric matrix, otherwise A contains a lower one.

OUTPUT PARAMETERS:
    A       -   the result of factorization. If IsUpper=True, then
                the upper triangle contains matrix U, so that A = U^T*U,
                and the elements below the main diagonal are not modified.
                Similarly, if IsUpper = False.

RESULT:
    If  the  matrix  is  positive-definite,  the  function  returns  True.
    Otherwise, the function returns False. Contents of A is not determined
    in such case.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixcholesky(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tmp;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixCholesky: incorrect N!", _state);
    ae_assert(a->rows>=n, "SPDMatrixCholesky: rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixCholesky: cols(A)<N", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixCholesky: A contains infinite or NaN values!", _state);
    result = spdmatrixcholeskyrec(a, 0, n, isupper, &tmp, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Update of Cholesky decomposition: rank-1 update to original A.  "Buffered"
version which uses preallocated buffer which is saved  between  subsequent
function calls.

This function uses internally allocated buffer which is not saved  between
subsequent  calls.  So,  if  you  perform  a lot  of  subsequent  updates,
we  recommend   you   to   use   "buffered"   version   of  this function:
SPDMatrixCholeskyUpdateAdd1Buf().

INPUT PARAMETERS:
    A       -   upper or lower Cholesky factor.
                array with elements [0..N-1, 0..N-1].
                Exception is thrown if array size is too small.
    N       -   size of matrix A, N>0
    IsUpper -   if IsUpper=True, then A contains  upper  Cholesky  factor;
                otherwise A contains a lower one.
    U       -   array[N], rank-1 update to A: A_mod = A + u*u'
                Exception is thrown if array size is too small.

OUTPUT PARAMETERS:
    A       -   updated factorization.  If  IsUpper=True,  then  the  upper
                triangle contains matrix U, and the elements below the main
                diagonal are not modified. Similarly, if IsUpper = False.
                
NOTE: this function always succeeds, so it does not return completion code

NOTE: this function checks sizes of input arrays, but it does  NOT  checks
      for presence of infinities or NAN's.

  -- ALGLIB --
     03.02.2014
     Sergey Bochkanov
*************************************************************************/
void spdmatrixcholeskyupdateadd1(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_vector* u,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector bufr;

    ae_frame_make(_state, &_frame_block);
    memset(&bufr, 0, sizeof(bufr));
    ae_vector_init(&bufr, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixCholeskyUpdateAdd1: N<=0", _state);
    ae_assert(a->rows>=n, "SPDMatrixCholeskyUpdateAdd1: Rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixCholeskyUpdateAdd1: Cols(A)<N", _state);
    ae_assert(u->cnt>=n, "SPDMatrixCholeskyUpdateAdd1: Length(U)<N", _state);
    spdmatrixcholeskyupdateadd1buf(a, n, isupper, u, &bufr, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Update of Cholesky decomposition: "fixing" some variables.

This function uses internally allocated buffer which is not saved  between
subsequent  calls.  So,  if  you  perform  a lot  of  subsequent  updates,
we  recommend   you   to   use   "buffered"   version   of  this function:
SPDMatrixCholeskyUpdateFixBuf().

"FIXING" EXPLAINED:

    Suppose we have N*N positive definite matrix A. "Fixing" some variable
    means filling corresponding row/column of  A  by  zeros,  and  setting
    diagonal element to 1.
    
    For example, if we fix 2nd variable in 4*4 matrix A, it becomes Af:
    
        ( A00  A01  A02  A03 )      ( Af00  0   Af02 Af03 )
        ( A10  A11  A12  A13 )      (  0    1    0    0   )
        ( A20  A21  A22  A23 )  =>  ( Af20  0   Af22 Af23 )
        ( A30  A31  A32  A33 )      ( Af30  0   Af32 Af33 )
    
    If we have Cholesky decomposition of A, it must be recalculated  after
    variables were  fixed.  However,  it  is  possible  to  use  efficient
    algorithm, which needs O(K*N^2)  time  to  "fix"  K  variables,  given
    Cholesky decomposition of original, "unfixed" A.

INPUT PARAMETERS:
    A       -   upper or lower Cholesky factor.
                array with elements [0..N-1, 0..N-1].
                Exception is thrown if array size is too small.
    N       -   size of matrix A, N>0
    IsUpper -   if IsUpper=True, then A contains  upper  Cholesky  factor;
                otherwise A contains a lower one.
    Fix     -   array[N], I-th element is True if I-th  variable  must  be
                fixed. Exception is thrown if array size is too small.
    BufR    -   possibly preallocated  buffer;  automatically  resized  if
                needed. It is recommended to  reuse  this  buffer  if  you
                perform a lot of subsequent decompositions.

OUTPUT PARAMETERS:
    A       -   updated factorization.  If  IsUpper=True,  then  the  upper
                triangle contains matrix U, and the elements below the main
                diagonal are not modified. Similarly, if IsUpper = False.
                
NOTE: this function always succeeds, so it does not return completion code

NOTE: this function checks sizes of input arrays, but it does  NOT  checks
      for presence of infinities or NAN's.
      
NOTE: this  function  is  efficient  only  for  moderate amount of updated
      variables - say, 0.1*N or 0.3*N. For larger amount of  variables  it
      will  still  work,  but  you  may  get   better   performance   with
      straightforward Cholesky.

  -- ALGLIB --
     03.02.2014
     Sergey Bochkanov
*************************************************************************/
void spdmatrixcholeskyupdatefix(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Boolean */ const ae_vector* fix,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector bufr;

    ae_frame_make(_state, &_frame_block);
    memset(&bufr, 0, sizeof(bufr));
    ae_vector_init(&bufr, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixCholeskyUpdateFix: N<=0", _state);
    ae_assert(a->rows>=n, "SPDMatrixCholeskyUpdateFix: Rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixCholeskyUpdateFix: Cols(A)<N", _state);
    ae_assert(fix->cnt>=n, "SPDMatrixCholeskyUpdateFix: Length(Fix)<N", _state);
    spdmatrixcholeskyupdatefixbuf(a, n, isupper, fix, &bufr, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Update of Cholesky decomposition: rank-1 update to original A.  "Buffered"
version which uses preallocated buffer which is saved  between  subsequent
function calls.

See comments for SPDMatrixCholeskyUpdateAdd1() for more information.

INPUT PARAMETERS:
    A       -   upper or lower Cholesky factor.
                array with elements [0..N-1, 0..N-1].
                Exception is thrown if array size is too small.
    N       -   size of matrix A, N>0
    IsUpper -   if IsUpper=True, then A contains  upper  Cholesky  factor;
                otherwise A contains a lower one.
    U       -   array[N], rank-1 update to A: A_mod = A + u*u'
                Exception is thrown if array size is too small.
    BufR    -   possibly preallocated  buffer;  automatically  resized  if
                needed. It is recommended to  reuse  this  buffer  if  you
                perform a lot of subsequent decompositions.

OUTPUT PARAMETERS:
    A       -   updated factorization.  If  IsUpper=True,  then  the  upper
                triangle contains matrix U, and the elements below the main
                diagonal are not modified. Similarly, if IsUpper = False.

  -- ALGLIB --
     03.02.2014
     Sergey Bochkanov
*************************************************************************/
void spdmatrixcholeskyupdateadd1buf(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_vector* u,
     /* Real    */ ae_vector* bufr,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t nz;
    double cs;
    double sn;
    double v;
    double vv;


    ae_assert(n>0, "SPDMatrixCholeskyUpdateAdd1Buf: N<=0", _state);
    ae_assert(a->rows>=n, "SPDMatrixCholeskyUpdateAdd1Buf: Rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixCholeskyUpdateAdd1Buf: Cols(A)<N", _state);
    ae_assert(u->cnt>=n, "SPDMatrixCholeskyUpdateAdd1Buf: Length(U)<N", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixCholeskyUpdateAdd1Buf: A contains infinite/NAN values", _state);
    ae_assert(isfinitevector(u, n, _state), "SPDMatrixCholeskyUpdateAdd1Buf: A contains infinite/NAN values", _state);
    
    /*
     * Find index of first non-zero entry in U
     */
    nz = n;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(u->ptr.p_double[i],(double)(0)) )
        {
            nz = i;
            break;
        }
    }
    if( nz==n )
    {
        
        /*
         * Nothing to update
         */
        return;
    }
    
    /*
     * If working with upper triangular matrix
     */
    if( isupper )
    {
        
        /*
         * Perform a sequence of updates which fix variables one by one.
         * This approach is different from one which is used when we work
         * with lower triangular matrix.
         */
        rvectorsetlengthatleast(bufr, n, _state);
        for(j=nz; j<=n-1; j++)
        {
            bufr->ptr.p_double[j] = u->ptr.p_double[j];
        }
        for(i=nz; i<=n-1; i++)
        {
            if( ae_fp_neq(bufr->ptr.p_double[i],(double)(0)) )
            {
                generaterotation(a->ptr.pp_double[i][i], bufr->ptr.p_double[i], &cs, &sn, &v, _state);
                a->ptr.pp_double[i][i] = v;
                bufr->ptr.p_double[i] = 0.0;
                for(j=i+1; j<=n-1; j++)
                {
                    v = a->ptr.pp_double[i][j];
                    vv = bufr->ptr.p_double[j];
                    a->ptr.pp_double[i][j] = cs*v+sn*vv;
                    bufr->ptr.p_double[j] = -sn*v+cs*vv;
                }
            }
        }
    }
    else
    {
        
        /*
         * Calculate rows of modified Cholesky factor, row-by-row
         * (updates performed during variable fixing are applied
         * simultaneously to each row)
         */
        rvectorsetlengthatleast(bufr, 3*n, _state);
        for(j=nz; j<=n-1; j++)
        {
            bufr->ptr.p_double[j] = u->ptr.p_double[j];
        }
        for(i=nz; i<=n-1; i++)
        {
            
            /*
             * Update all previous updates [Idx+1...I-1] to I-th row
             */
            vv = bufr->ptr.p_double[i];
            for(j=nz; j<=i-1; j++)
            {
                cs = bufr->ptr.p_double[n+2*j+0];
                sn = bufr->ptr.p_double[n+2*j+1];
                v = a->ptr.pp_double[i][j];
                a->ptr.pp_double[i][j] = cs*v+sn*vv;
                vv = -sn*v+cs*vv;
            }
            
            /*
             * generate rotation applied to I-th element of update vector
             */
            generaterotation(a->ptr.pp_double[i][i], vv, &cs, &sn, &v, _state);
            a->ptr.pp_double[i][i] = v;
            bufr->ptr.p_double[n+2*i+0] = cs;
            bufr->ptr.p_double[n+2*i+1] = sn;
        }
    }
}


/*************************************************************************
Update of Cholesky  decomposition:  "fixing"  some  variables.  "Buffered"
version which uses preallocated buffer which is saved  between  subsequent
function calls.

See comments for SPDMatrixCholeskyUpdateFix() for more information.

INPUT PARAMETERS:
    A       -   upper or lower Cholesky factor.
                array with elements [0..N-1, 0..N-1].
                Exception is thrown if array size is too small.
    N       -   size of matrix A, N>0
    IsUpper -   if IsUpper=True, then A contains  upper  Cholesky  factor;
                otherwise A contains a lower one.
    Fix     -   array[N], I-th element is True if I-th  variable  must  be
                fixed. Exception is thrown if array size is too small.
    BufR    -   possibly preallocated  buffer;  automatically  resized  if
                needed. It is recommended to  reuse  this  buffer  if  you
                perform a lot of subsequent decompositions.

OUTPUT PARAMETERS:
    A       -   updated factorization.  If  IsUpper=True,  then  the  upper
                triangle contains matrix U, and the elements below the main
                diagonal are not modified. Similarly, if IsUpper = False.

  -- ALGLIB --
     03.02.2014
     Sergey Bochkanov
*************************************************************************/
void spdmatrixcholeskyupdatefixbuf(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Boolean */ const ae_vector* fix,
     /* Real    */ ae_vector* bufr,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t nfix;
    ae_int_t idx;
    double cs;
    double sn;
    double v;
    double vv;


    ae_assert(n>0, "SPDMatrixCholeskyUpdateFixBuf: N<=0", _state);
    ae_assert(a->rows>=n, "SPDMatrixCholeskyUpdateFixBuf: Rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixCholeskyUpdateFixBuf: Cols(A)<N", _state);
    ae_assert(fix->cnt>=n, "SPDMatrixCholeskyUpdateFixBuf: Length(Fix)<N", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixCholeskyUpdateAdd1Buf: A contains infinite/NAN values", _state);
    
    /*
     * Count number of variables to fix.
     * Quick exit if NFix=0 or NFix=N
     */
    nfix = 0;
    for(i=0; i<=n-1; i++)
    {
        if( fix->ptr.p_bool[i] )
        {
            inc(&nfix, _state);
        }
    }
    if( nfix==0 )
    {
        
        /*
         * Nothing to fix
         */
        return;
    }
    if( nfix==n )
    {
        
        /*
         * All variables are fixed.
         * Set A to identity and exit.
         */
        if( isupper )
        {
            for(i=0; i<=n-1; i++)
            {
                a->ptr.pp_double[i][i] = (double)(1);
                for(j=i+1; j<=n-1; j++)
                {
                    a->ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        else
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=i-1; j++)
                {
                    a->ptr.pp_double[i][j] = (double)(0);
                }
                a->ptr.pp_double[i][i] = (double)(1);
            }
        }
        return;
    }
    
    /*
     * If working with upper triangular matrix
     */
    if( isupper )
    {
        
        /*
         * Perform a sequence of updates which fix variables one by one.
         * This approach is different from one which is used when we work
         * with lower triangular matrix.
         */
        rvectorsetlengthatleast(bufr, n, _state);
        for(k=0; k<=n-1; k++)
        {
            if( fix->ptr.p_bool[k] )
            {
                idx = k;
                
                /*
                 * Quick exit if it is last variable
                 */
                if( idx==n-1 )
                {
                    for(i=0; i<=idx-1; i++)
                    {
                        a->ptr.pp_double[i][idx] = 0.0;
                    }
                    a->ptr.pp_double[idx][idx] = 1.0;
                    continue;
                }
                
                /*
                 * We have Cholesky decomposition of quadratic term in A,
                 * with upper triangle being stored as given below:
                 *
                 *         ( U00 u01 U02 )
                 *     U = (     u11 u12 )
                 *         (         U22 )
                 *
                 * Here u11 is diagonal element corresponding to variable K. We
                 * want to fix this variable, and we do so by modifying U as follows:
                 *
                 *             ( U00  0  U02 )
                 *     U_mod = (      1   0  )
                 *             (         U_m )
                 *
                 * with U_m = CHOLESKY [ (U22^T)*U22 + (u12^T)*u12 ]
                 *
                 * Of course, we can calculate U_m by calculating (U22^T)*U22 explicitly,
                 * modifying it and performing Cholesky decomposition of modified matrix.
                 * However, we can treat it as follows:
                 * * we already have CHOLESKY[(U22^T)*U22], which is equal to U22
                 * * we have rank-1 update (u12^T)*u12 applied to (U22^T)*U22
                 * * thus, we can calculate updated Cholesky with O(N^2) algorithm
                 *   instead of O(N^3) one
                 */
                for(j=idx+1; j<=n-1; j++)
                {
                    bufr->ptr.p_double[j] = a->ptr.pp_double[idx][j];
                }
                for(i=0; i<=idx-1; i++)
                {
                    a->ptr.pp_double[i][idx] = 0.0;
                }
                a->ptr.pp_double[idx][idx] = 1.0;
                for(i=idx+1; i<=n-1; i++)
                {
                    a->ptr.pp_double[idx][i] = 0.0;
                }
                for(i=idx+1; i<=n-1; i++)
                {
                    if( ae_fp_neq(bufr->ptr.p_double[i],(double)(0)) )
                    {
                        generaterotation(a->ptr.pp_double[i][i], bufr->ptr.p_double[i], &cs, &sn, &v, _state);
                        a->ptr.pp_double[i][i] = v;
                        bufr->ptr.p_double[i] = 0.0;
                        for(j=i+1; j<=n-1; j++)
                        {
                            v = a->ptr.pp_double[i][j];
                            vv = bufr->ptr.p_double[j];
                            a->ptr.pp_double[i][j] = cs*v+sn*vv;
                            bufr->ptr.p_double[j] = -sn*v+cs*vv;
                        }
                    }
                }
            }
        }
    }
    else
    {
        
        /*
         * Calculate rows of modified Cholesky factor, row-by-row
         * (updates performed during variable fixing are applied
         * simultaneously to each row)
         */
        rvectorsetlengthatleast(bufr, 3*n, _state);
        for(k=0; k<=n-1; k++)
        {
            if( fix->ptr.p_bool[k] )
            {
                idx = k;
                
                /*
                 * Quick exit if it is last variable
                 */
                if( idx==n-1 )
                {
                    for(i=0; i<=idx-1; i++)
                    {
                        a->ptr.pp_double[idx][i] = 0.0;
                    }
                    a->ptr.pp_double[idx][idx] = 1.0;
                    continue;
                }
                
                /*
                 * store column to buffer and clear row/column of A
                 */
                for(j=idx+1; j<=n-1; j++)
                {
                    bufr->ptr.p_double[j] = a->ptr.pp_double[j][idx];
                }
                for(i=0; i<=idx-1; i++)
                {
                    a->ptr.pp_double[idx][i] = 0.0;
                }
                a->ptr.pp_double[idx][idx] = 1.0;
                for(i=idx+1; i<=n-1; i++)
                {
                    a->ptr.pp_double[i][idx] = 0.0;
                }
                
                /*
                 * Apply update to rows of A
                 */
                for(i=idx+1; i<=n-1; i++)
                {
                    
                    /*
                     * Update all previous updates [Idx+1...I-1] to I-th row
                     */
                    vv = bufr->ptr.p_double[i];
                    for(j=idx+1; j<=i-1; j++)
                    {
                        cs = bufr->ptr.p_double[n+2*j+0];
                        sn = bufr->ptr.p_double[n+2*j+1];
                        v = a->ptr.pp_double[i][j];
                        a->ptr.pp_double[i][j] = cs*v+sn*vv;
                        vv = -sn*v+cs*vv;
                    }
                    
                    /*
                     * generate rotation applied to I-th element of update vector
                     */
                    generaterotation(a->ptr.pp_double[i][i], vv, &cs, &sn, &v, _state);
                    a->ptr.pp_double[i][i] = v;
                    bufr->ptr.p_double[n+2*i+0] = cs;
                    bufr->ptr.p_double[n+2*i+1] = sn;
                }
            }
        }
    }
}


/*************************************************************************
Sparse LU decomposition with column pivoting for sparsity and row pivoting
for stability. Input must be square sparse matrix stored in CRS format.

The algorithm  computes  LU  decomposition  of  a  general  square  matrix
(rectangular ones are not supported). The result  of  an  algorithm  is  a
representation of A as A = P*L*U*Q, where:
* L is lower unitriangular matrix
* U is upper triangular matrix
* P = P0*P1*...*PK, K=N-1, Pi - permutation matrix for I and P[I]
* Q = QK*...*Q1*Q0, K=N-1, Qi - permutation matrix for I and Q[I]
    
This function pivots columns for higher sparsity, and then pivots rows for
stability (larger element at the diagonal).

INPUT PARAMETERS:
    A       -   sparse NxN matrix in CRS format. An exception is generated
                if matrix is non-CRS or non-square.
    PivotType-  pivoting strategy:
                * 0 for best pivoting available (2 in current version)
                * 1 for row-only pivoting (NOT RECOMMENDED)
                * 2 for complete pivoting which produces most sparse outputs

OUTPUT PARAMETERS:
    A       -   the result of factorization, matrices L and U stored in
                compact form using CRS sparse storage format:
                * lower unitriangular L is stored strictly under main diagonal
                * upper triangilar U is stored ON and ABOVE main diagonal
    P       -   row permutation matrix in compact form, array[N]
    Q       -   col permutation matrix in compact form, array[N]
    
This function always succeeds, i.e. it ALWAYS returns valid factorization,
but for your convenience it also returns  boolean  value  which  helps  to
detect symbolically degenerate matrices:
* function returns TRUE, if the matrix was factorized AND symbolically
  non-degenerate
* function returns FALSE, if the matrix was factorized but U has strictly
  zero elements at the diagonal (the factorization is returned anyway).


  -- ALGLIB routine --
     03.09.2018
     Bochkanov Sergey
*************************************************************************/
ae_bool sparselu(sparsematrix* a,
     ae_int_t pivottype,
     /* Integer */ ae_vector* p,
     /* Integer */ ae_vector* q,
     ae_state *_state)
{
    ae_frame _frame_block;
    sluv2buffer buf2;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&buf2, 0, sizeof(buf2));
    ae_vector_clear(p);
    ae_vector_clear(q);
    _sluv2buffer_init(&buf2, _state, ae_true);

    ae_assert((pivottype==0||pivottype==1)||pivottype==2, "SparseLU: unexpected pivot type", _state);
    ae_assert(sparseiscrs(a, _state), "SparseLU: A is not stored in CRS format", _state);
    ae_assert(sparsegetnrows(a, _state)==sparsegetncols(a, _state), "SparseLU: non-square A", _state);
    result = sptrflu(a, pivottype, p, q, &buf2, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Sparse Cholesky decomposition for skyline matrixm using in-place algorithm
without allocating additional storage.

The algorithm computes Cholesky decomposition  of  a  symmetric  positive-
definite sparse matrix. The result of an algorithm is a representation  of
A as A=U^T*U or A=L*L^T

This function allows to perform very efficient decomposition of low-profile
matrices (average bandwidth is ~5-10 elements). For larger matrices it  is
recommended to use supernodal Cholesky decomposition: SparseCholeskyP() or
SparseCholeskyAnalyze()/SparseCholeskyFactorize().

INPUT PARAMETERS:
    A       -   sparse matrix in skyline storage (SKS) format.
    N       -   size of matrix A (can be smaller than actual size of A)
    IsUpper -   if IsUpper=True, then factorization is performed on  upper
                triangle. Another triangle is ignored (it may contant some
                data, but it is not changed).
    

OUTPUT PARAMETERS:
    A       -   the result of factorization, stored in SKS. If IsUpper=True,
                then the upper  triangle  contains  matrix  U,  such  that
                A = U^T*U. Lower triangle is not changed.
                Similarly, if IsUpper = False. In this case L is returned,
                and we have A = L*(L^T).
                Note that THIS function does not  perform  permutation  of
                rows to reduce bandwidth.

RESULT:
    If  the  matrix  is  positive-definite,  the  function  returns  True.
    Otherwise, the function returns False. Contents of A is not determined
    in such case.

NOTE: for  performance  reasons  this  function  does NOT check that input
      matrix  includes  only  finite  values. It is your responsibility to
      make sure that there are no infinite or NAN values in the matrix.

  -- ALGLIB routine --
     16.01.2014
     Bochkanov Sergey
*************************************************************************/
ae_bool sparsecholeskyskyline(sparsematrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t jnz;
    ae_int_t jnza;
    ae_int_t jnzl;
    double v;
    double vv;
    double a12;
    ae_int_t nready;
    ae_int_t nadd;
    ae_int_t banda;
    ae_int_t offsa;
    ae_int_t offsl;
    ae_bool result;


    ae_assert(n>=0, "SparseCholeskySkyline: N<0", _state);
    ae_assert(sparsegetnrows(a, _state)>=n, "SparseCholeskySkyline: rows(A)<N", _state);
    ae_assert(sparsegetncols(a, _state)>=n, "SparseCholeskySkyline: cols(A)<N", _state);
    ae_assert(sparseissks(a, _state), "SparseCholeskySkyline: A is not stored in SKS format", _state);
    result = ae_false;
    
    /*
     * transpose if needed
     */
    if( isupper )
    {
        sparsetransposesks(a, _state);
    }
    
    /*
     * Perform Cholesky decomposition:
     * * we assume than leading NReady*NReady submatrix is done
     * * having Cholesky decomposition of NReady*NReady submatrix we
     *   obtain decomposition of larger (NReady+NAdd)*(NReady+NAdd) one.
     *
     * Here is algorithm. At the start we have
     *
     *     (      |   )
     *     (  L   |   )
     * S = (      |   )
     *     (----------)
     *     (  A   | B )
     *
     * with L being already computed Cholesky factor, A and B being
     * unprocessed parts of the matrix. Of course, L/A/B are stored
     * in SKS format.
     *
     * Then, we calculate A1:=(inv(L)*A')' and replace A with A1.
     * Then, we calculate B1:=B-A1*A1'     and replace B with B1
     *
     * Finally, we calculate small NAdd*NAdd Cholesky of B1 with
     * dense solver. Now, L/A1/B1 are Cholesky decomposition of the
     * larger (NReady+NAdd)*(NReady+NAdd) matrix.
     */
    nready = 0;
    nadd = 1;
    while(nready<n)
    {
        ae_assert(nadd==1, "SkylineCholesky: internal error", _state);
        
        /*
         * Calculate A1:=(inv(L)*A')'
         *
         * Elements are calculated row by row (example below is given
         * for NAdd=1):
         * * first, we solve L[0,0]*A1[0]=A[0]
         * * then, we solve  L[1,0]*A1[0]+L[1,1]*A1[1]=A[1]
         * * then, we move to next row and so on
         * * during calculation of A1 we update A12 - squared norm of A1
         *
         * We extensively use sparsity of both A/A1 and L:
         * * first, equations from 0 to BANDWIDTH(A1)-1 are completely zero
         * * second, for I>=BANDWIDTH(A1), I-th equation is reduced from
         *     L[I,0]*A1[0] + L[I,1]*A1[1] + ... + L[I,I]*A1[I] = A[I]
         *   to
         *     L[I,JNZ]*A1[JNZ] + ... + L[I,I]*A1[I] = A[I]
         *   where JNZ = max(NReady-BANDWIDTH(A1),I-BANDWIDTH(L[i]))
         *   (JNZ is an index of the firts column where both A and L become
         *   nonzero).
         *
         * NOTE: we rely on details of SparseMatrix internal storage format.
         *       This is allowed by SparseMatrix specification.
         */
        a12 = 0.0;
        if( a->didx.ptr.p_int[nready]>0 )
        {
            banda = a->didx.ptr.p_int[nready];
            for(i=nready-banda; i<=nready-1; i++)
            {
                
                /*
                 * Elements of A1[0:I-1] were computed:
                 * * A1[0:NReady-BandA-1] are zero (sparse)
                 * * A1[NReady-BandA:I-1] replaced corresponding elements of A
                 *
                 * Now it is time to get I-th one.
                 *
                 * First, we calculate:
                 * * JNZA  - index of the first column where A become nonzero
                 * * JNZL  - index of the first column where L become nonzero
                 * * JNZ   - index of the first column where both A and L become nonzero
                 * * OffsA - offset of A[JNZ] in A.Vals
                 * * OffsL - offset of L[I,JNZ] in A.Vals
                 *
                 * Then, we solve SUM(A1[j]*L[I,j],j=JNZ..I-1) + A1[I]*L[I,I] = A[I],
                 * with A1[JNZ..I-1] already known, and A1[I] unknown.
                 */
                jnza = nready-banda;
                jnzl = i-a->didx.ptr.p_int[i];
                jnz = ae_maxint(jnza, jnzl, _state);
                offsa = a->ridx.ptr.p_int[nready]+(jnz-jnza);
                offsl = a->ridx.ptr.p_int[i]+(jnz-jnzl);
                v = 0.0;
                k = i-1-jnz;
                for(j=0; j<=k; j++)
                {
                    v = v+a->vals.ptr.p_double[offsa+j]*a->vals.ptr.p_double[offsl+j];
                }
                vv = (a->vals.ptr.p_double[offsa+k+1]-v)/a->vals.ptr.p_double[offsl+k+1];
                a->vals.ptr.p_double[offsa+k+1] = vv;
                a12 = a12+vv*vv;
            }
        }
        
        /*
         * Calculate CHOLESKY(B-A1*A1')
         */
        offsa = a->ridx.ptr.p_int[nready]+a->didx.ptr.p_int[nready];
        v = a->vals.ptr.p_double[offsa];
        if( ae_fp_less_eq(v,a12) )
        {
            result = ae_false;
            return result;
        }
        a->vals.ptr.p_double[offsa] = ae_sqrt(v-a12, _state);
        
        /*
         * Increase size of the updated matrix
         */
        inc(&nready, _state);
    }
    
    /*
     * transpose if needed
     */
    if( isupper )
    {
        sparsetransposesks(a, _state);
    }
    result = ae_true;
    return result;
}


/*************************************************************************
Sparse Cholesky decomposition for a matrix  stored  in  any sparse storage,
without rows/cols permutation.

This function is the most convenient (less parameters to specify), although
less efficient, version of sparse Cholesky.

Internally it:
* calls SparseCholeskyAnalyze()  function  to  perform  symbolic  analysis
  phase with no permutation being configured.
* calls SparseCholeskyFactorize() function to perform numerical  phase  of
  the factorization

Following alternatives may result in better performance:
* using SparseCholeskyP(), which selects best  pivoting  available,  which
  almost always results in improved sparsity and cache locality
* using  SparseCholeskyAnalyze() and SparseCholeskyFactorize()   functions
  directly,  which  may  improve  performance of repetitive factorizations
  with same sparsity patterns.

The latter also allows one to perform  LDLT  factorization  of  indefinite
matrix (one with strictly diagonal D, which is known  to  be  stable  only
in few special cases, like quasi-definite matrices).

INPUT PARAMETERS:
    A       -   a square NxN sparse matrix, stored in any storage format.
    IsUpper -   if IsUpper=True, then factorization is performed on  upper
                triangle.  Another triangle is ignored on  input,  dropped
                on output. Similarly, if IsUpper=False, the lower triangle
                is processed.

OUTPUT PARAMETERS:
    A       -   the result of factorization, stored in CRS format:
                * if IsUpper=True, then the upper triangle contains matrix
                  U such  that  A = U^T*U and the lower triangle is empty.
                * similarly, if IsUpper=False, then lower triangular L  is
                  returned and we have A = L*(L^T).
                Note that THIS function does not  perform  permutation  of
                the rows to reduce fill-in.

RESULT:
    If  the  matrix  is  positive-definite,  the  function  returns  True.
    Otherwise, the function returns False.  Contents  of  A  is  undefined
    in such case.

NOTE: for  performance  reasons  this  function  does NOT check that input
      matrix  includes  only  finite  values. It is your responsibility to
      make sure that there are no infinite or NAN values in the matrix.

  -- ALGLIB routine --
     16.09.2020
     Bochkanov Sergey
*************************************************************************/
ae_bool sparsecholesky(sparsematrix* a, ae_bool isupper, ae_state *_state)
{
    ae_frame _frame_block;
    sparsedecompositionanalysis analysis;
    ae_int_t facttype;
    ae_int_t permtype;
    ae_int_t donotreusemem;
    ae_vector priorities;
    ae_vector dummyd;
    ae_vector dummyp;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&analysis, 0, sizeof(analysis));
    memset(&priorities, 0, sizeof(priorities));
    memset(&dummyd, 0, sizeof(dummyd));
    memset(&dummyp, 0, sizeof(dummyp));
    _sparsedecompositionanalysis_init(&analysis, _state, ae_true);
    ae_vector_init(&priorities, 0, DT_INT, _state, ae_true);
    ae_vector_init(&dummyd, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dummyp, 0, DT_INT, _state, ae_true);

    ae_assert(sparsegetnrows(a, _state)==sparsegetncols(a, _state), "SparseCholesky: A is not square", _state);
    
    /*
     * Quick exit
     */
    if( sparsegetnrows(a, _state)==0 )
    {
        result = ae_true;
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Choose factorization and permutation: vanilla Cholesky and no permutation,
     * Priorities[] array is not set.
     */
    facttype = 0;
    permtype = -1;
    donotreusemem = -1;
    
    /*
     * Easy case - CRS matrix in lower triangle, no conversion or transposition is needed
     */
    if( sparseiscrs(a, _state)&&!isupper )
    {
        result = spsymmanalyze(a, &priorities, 0.0, 0, facttype, permtype, donotreusemem, &analysis.analysis, _state);
        if( !result )
        {
            ae_frame_leave(_state);
            return result;
        }
        result = spsymmfactorize(&analysis.analysis, _state);
        if( !result )
        {
            ae_frame_leave(_state);
            return result;
        }
        spsymmextract(&analysis.analysis, a, &dummyd, &dummyp, _state);
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * A bit more complex - we need conversion and/or transposition
     */
    if( isupper )
    {
        sparsecopytocrsbuf(a, &analysis.wrkat, _state);
        sparsecopytransposecrsbuf(&analysis.wrkat, &analysis.wrka, _state);
    }
    else
    {
        sparsecopytocrsbuf(a, &analysis.wrka, _state);
    }
    result = spsymmanalyze(&analysis.wrka, &priorities, 0.0, 0, facttype, permtype, donotreusemem, &analysis.analysis, _state);
    if( !result )
    {
        ae_frame_leave(_state);
        return result;
    }
    result = spsymmfactorize(&analysis.analysis, _state);
    if( !result )
    {
        ae_frame_leave(_state);
        return result;
    }
    if( isupper )
    {
        spsymmextract(&analysis.analysis, &analysis.wrka, &dummyd, &dummyp, _state);
        sparsecopytransposecrsbuf(&analysis.wrka, a, _state);
    }
    else
    {
        spsymmextract(&analysis.analysis, a, &dummyd, &dummyp, _state);
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Sparse Cholesky decomposition for a matrix  stored  in  any sparse storage
format, with performance-enhancing permutation of rows/cols.

Present version is configured  to  perform  supernodal  permutation  which
sparsity reducing ordering.

This function is a wrapper around generic sparse  decomposition  functions
that internally:
* calls SparseCholeskyAnalyze()  function  to  perform  symbolic  analysis
  phase with best available permutation being configured.
* calls SparseCholeskyFactorize() function to perform numerical  phase  of
  the factorization.

NOTE: using  SparseCholeskyAnalyze() and SparseCholeskyFactorize() directly
      may improve  performance  of  repetitive  factorizations  with  same
      sparsity patterns. It also allows one to perform  LDLT factorization
      of  indefinite  matrix  -  a factorization with strictly diagonal D,
      which  is  known to be stable only in few special cases, like quasi-
      definite matrices.

INPUT PARAMETERS:
    A       -   a square NxN sparse matrix, stored in any storage format.
    IsUpper -   if IsUpper=True, then factorization is performed on  upper
                triangle.  Another triangle is ignored on  input,  dropped
                on output. Similarly, if IsUpper=False, the lower triangle
                is processed.

OUTPUT PARAMETERS:
    A       -   the result of factorization, stored in CRS format:
                * if IsUpper=True, then the upper triangle contains matrix
                  U such  that  A = U^T*U and the lower triangle is empty.
                * similarly, if IsUpper=False, then lower triangular L  is
                  returned and we have A = L*(L^T).
    P       -   a row/column permutation, a product of P0*P1*...*Pk, k=N-1,
                with Pi being permutation of rows/cols I and P[I]

RESULT:
    If  the  matrix  is  positive-definite,  the  function  returns  True.
    Otherwise, the function returns False.  Contents  of  A  is  undefined
    in such case.

NOTE: for  performance  reasons  this  function  does NOT check that input
      matrix  includes  only  finite  values. It is your responsibility to
      make sure that there are no infinite or NAN values in the matrix.

  -- ALGLIB routine --
     16.09.2020
     Bochkanov Sergey
*************************************************************************/
ae_bool sparsecholeskyp(sparsematrix* a,
     ae_bool isupper,
     /* Integer */ ae_vector* p,
     ae_state *_state)
{
    ae_frame _frame_block;
    sparsedecompositionanalysis analysis;
    ae_vector dummyd;
    ae_int_t facttype;
    ae_int_t permtype;
    ae_int_t donotreusemem;
    ae_vector priorities;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&analysis, 0, sizeof(analysis));
    memset(&dummyd, 0, sizeof(dummyd));
    memset(&priorities, 0, sizeof(priorities));
    ae_vector_clear(p);
    _sparsedecompositionanalysis_init(&analysis, _state, ae_true);
    ae_vector_init(&dummyd, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&priorities, 0, DT_INT, _state, ae_true);

    ae_assert(sparsegetnrows(a, _state)==sparsegetncols(a, _state), "SparseCholeskyP: A is not square", _state);
    
    /*
     * Quick exit
     */
    if( sparsegetnrows(a, _state)==0 )
    {
        result = ae_true;
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Choose factorization and permutation: vanilla Cholesky and best permutation available.
     * Priorities[] array is not set.
     */
    facttype = 0;
    permtype = 0;
    donotreusemem = -1;
    
    /*
     * Easy case - CRS matrix in lower triangle, no conversion or transposition is needed
     */
    if( sparseiscrs(a, _state)&&!isupper )
    {
        result = spsymmanalyze(a, &priorities, 0.0, 0, facttype, permtype, donotreusemem, &analysis.analysis, _state);
        if( !result )
        {
            ae_frame_leave(_state);
            return result;
        }
        result = spsymmfactorize(&analysis.analysis, _state);
        if( !result )
        {
            ae_frame_leave(_state);
            return result;
        }
        spsymmextract(&analysis.analysis, a, &dummyd, p, _state);
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * A bit more complex - we need conversion and/or transposition
     */
    if( isupper )
    {
        sparsecopytocrsbuf(a, &analysis.wrkat, _state);
        sparsecopytransposecrsbuf(&analysis.wrkat, &analysis.wrka, _state);
    }
    else
    {
        sparsecopytocrsbuf(a, &analysis.wrka, _state);
    }
    result = spsymmanalyze(&analysis.wrka, &priorities, 0.0, 0, facttype, permtype, donotreusemem, &analysis.analysis, _state);
    if( !result )
    {
        ae_frame_leave(_state);
        return result;
    }
    result = spsymmfactorize(&analysis.analysis, _state);
    if( !result )
    {
        ae_frame_leave(_state);
        return result;
    }
    if( isupper )
    {
        spsymmextract(&analysis.analysis, &analysis.wrka, &dummyd, p, _state);
        sparsecopytransposecrsbuf(&analysis.wrka, a, _state);
    }
    else
    {
        spsymmextract(&analysis.analysis, a, &dummyd, p, _state);
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Sparse Cholesky/LDLT decomposition: symbolic analysis phase.

This function is a part of the 'expert' sparse Cholesky API:
* SparseCholeskyAnalyze(), that performs symbolic analysis phase and loads
  matrix to be factorized into internal storage
* SparseCholeskySetModType(), that allows to  use  modified  Cholesky/LDLT
  with lower bounds on pivot magnitudes and additional overflow safeguards
* SparseCholeskyFactorize(),  that performs  numeric  factorization  using
  precomputed symbolic analysis and internally stored matrix - and outputs
  result
* SparseCholeskyReload(), that reloads one more matrix with same  sparsity
  pattern into internal storage so  one  may  reuse  previously  allocated
  temporaries and previously performed symbolic analysis

This specific function performs preliminary analysis of the  Cholesky/LDLT
factorization. It allows to choose  different  permutation  types  and  to
choose between classic Cholesky and  indefinite  LDLT  factorization  (the
latter is computed with strictly diagonal D, i.e.  without  Bunch-Kauffman
pivoting).

NOTE: L*D*LT family of factorization may be used to  factorize  indefinite
      matrices. However, numerical stability is guaranteed ONLY for a class
      of quasi-definite matrices.

NOTE: all internal processing is performed with lower triangular  matrices
      stored  in  CRS  format.  Any  other  storage  formats  and/or upper
      triangular storage means  that  one  format  conversion  and/or  one
      transposition will be performed  internally  for  the  analysis  and
      factorization phases. Thus, highest  performance  is  achieved  when
      input is a lower triangular CRS matrix.

INPUT PARAMETERS:
    A           -   sparse square matrix in any sparse storage format.
    IsUpper     -   whether upper or lower  triangle  is  decomposed  (the
                    other one is ignored).
    FactType    -   factorization type:
                    * 0 for traditional Cholesky of SPD matrix
                    * 1 for LDLT decomposition with strictly  diagonal  D,
                        which may have non-positive entries.
    PermType    -   permutation type:
                    *-1 for absence of permutation
                    * 0 for best fill-in reducing  permutation  available,
                        which is 3 in the current version
                    * 1 for supernodal ordering (improves locality and
                      performance, does NOT change fill-in factor)
                    * 2 for original AMD ordering
                    * 3 for  improved  AMD  (approximate  minimum  degree)
                        ordering with better  handling  of  matrices  with
                        dense rows/columns

OUTPUT PARAMETERS:
    Analysis    -   contains:
                    * symbolic analysis of the matrix structure which will
                      be used later to guide numerical factorization.
                    * specific numeric values loaded into internal  memory
                      waiting for the factorization to be performed

This function fails if and only if the matrix A is symbolically degenerate
i.e. has diagonal element which is exactly zero. In  such  case  False  is
returned, contents of Analysis object is undefined.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
ae_bool sparsecholeskyanalyze(const sparsematrix* a,
     ae_bool isupper,
     ae_int_t facttype,
     ae_int_t permtype,
     sparsedecompositionanalysis* analysis,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector priorities;
    ae_int_t reusemem;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&priorities, 0, sizeof(priorities));
    _sparsedecompositionanalysis_clear(analysis);
    ae_vector_init(&priorities, 0, DT_INT, _state, ae_true);

    ae_assert(sparsegetnrows(a, _state)==sparsegetncols(a, _state), "SparseCholeskyAnalyze: A is not square", _state);
    ae_assert(facttype==0||facttype==1, "SparseCholeskyAnalyze: unexpected FactType", _state);
    ae_assert((((((permtype==0||permtype==1)||permtype==2)||permtype==3)||permtype==-1)||permtype==-2)||permtype==-3, "SparseCholeskyAnalyze: unexpected PermType", _state);
    
    /*
     * Prepare wrapper object
     */
    analysis->n = sparsegetnrows(a, _state);
    analysis->facttype = facttype;
    analysis->permtype = permtype;
    reusemem = 1;
    
    /*
     * Prepare default priorities for the priority ordering
     */
    if( permtype==-3||permtype==3 )
    {
        isetallocv(analysis->n, 0, &priorities, _state);
    }
    
    /*
     * Analyse
     */
    if( !sparseiscrs(a, _state) )
    {
        
        /*
         * The matrix is stored in non-CRS format. First, we have to convert
         * it to CRS. Then we may need to transpose it in order to get lower
         * triangular one (as supported by SPSymmAnalyze).
         */
        sparsecopytocrs(a, &analysis->crsa, _state);
        if( isupper )
        {
            sparsecopytransposecrsbuf(&analysis->crsa, &analysis->crsat, _state);
            result = spsymmanalyze(&analysis->crsat, &priorities, 0.0, 0, facttype, permtype, reusemem, &analysis->analysis, _state);
        }
        else
        {
            result = spsymmanalyze(&analysis->crsa, &priorities, 0.0, 0, facttype, permtype, reusemem, &analysis->analysis, _state);
        }
    }
    else
    {
        
        /*
         * The matrix is stored in CRS format. However we may need to
         * transpose it in order to get lower triangular one (as supported
         * by SPSymmAnalyze).
         */
        if( isupper )
        {
            sparsecopytransposecrsbuf(a, &analysis->crsat, _state);
            result = spsymmanalyze(&analysis->crsat, &priorities, 0.0, 0, facttype, permtype, reusemem, &analysis->analysis, _state);
        }
        else
        {
            result = spsymmanalyze(a, &priorities, 0.0, 0, facttype, permtype, reusemem, &analysis->analysis, _state);
        }
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Allows to control stability-improving  modification  strategy  for  sparse
Cholesky/LDLT decompositions. Modified Cholesky is more  robust  than  its
unmodified counterpart.

This function is a part of the 'expert' sparse Cholesky API:
* SparseCholeskyAnalyze(), that performs symbolic analysis phase and loads
  matrix to be factorized into internal storage
* SparseCholeskySetModType(), that allows to  use  modified  Cholesky/LDLT
  with lower bounds on pivot magnitudes and additional overflow safeguards
* SparseCholeskyFactorize(),  that performs  numeric  factorization  using
  precomputed symbolic analysis and internally stored matrix - and outputs
  result
* SparseCholeskyReload(), that reloads one more matrix with same  sparsity
  pattern into internal storage so  one  may  reuse  previously  allocated
  temporaries and previously performed symbolic analysis

INPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure
    ModStrategy -   modification type:
                    * 0 for traditional Cholesky/LDLT (Cholesky fails when
                      encounters nonpositive pivot, LDLT fails  when  zero
                      pivot   is  encountered,  no  stability  checks  for
                      overflows/underflows)
                    * 1 for modified Cholesky with additional checks:
                      * pivots less than ModParam0 are increased; (similar
                        sign-preserving procedure is applied during LDLT)
                      * if,  at  some  moment,  sum  of absolute values of
                        elements in column  J  will  become  greater  than
                        ModParam1, Cholesky/LDLT will treat it as  failure
                        and will stop immediately
    P0, P1, P2,P3 - modification parameters #0 #1, #2 and #3.
                    Params #2 and #3 are ignored in current version.

OUTPUT PARAMETERS:
    Analysis    -   symbolic analysis of the matrix structure, new strategy
                    Results will be seen with next SparseCholeskyFactorize()
                    call.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
void sparsecholeskysetmodtype(sparsedecompositionanalysis* analysis,
     ae_int_t modstrategy,
     double p0,
     double p1,
     double p2,
     double p3,
     ae_state *_state)
{


    spsymmsetmodificationstrategy(&analysis->analysis, modstrategy, p0, p1, p2, p3, _state);
}


/*************************************************************************
Sparse Cholesky decomposition: numerical analysis phase.

This function is a part of the 'expert' sparse Cholesky API:
* SparseCholeskyAnalyze(), that performs symbolic analysis phase and loads
  matrix to be factorized into internal storage
* SparseCholeskySetModType(), that allows to  use  modified  Cholesky/LDLT
  with lower bounds on pivot magnitudes and additional overflow safeguards
* SparseCholeskyFactorize(),  that performs  numeric  factorization  using
  precomputed symbolic analysis and internally stored matrix - and outputs
  result
* SparseCholeskyReload(), that reloads one more matrix with same  sparsity
  pattern into internal storage so  one  may  reuse  previously  allocated
  temporaries and previously performed symbolic analysis

Depending on settings specified during SparseCholeskyAnalyze() call it may
produce classic Cholesky or L*D*LT  decomposition  (with strictly diagonal
D), without permutation or with performance-enhancing permutation P.

NOTE: all internal processing is performed with lower triangular  matrices
      stored  in  CRS  format.  Any  other  storage  formats  and/or upper
      triangular storage means  that  one  format  conversion  and/or  one
      transposition will be performed  internally  for  the  analysis  and
      factorization phases. Thus, highest  performance  is  achieved  when
      input is a lower triangular CRS matrix, and lower triangular  output
      is requested.

NOTE: L*D*LT family of factorization may be used to  factorize  indefinite
      matrices. However, numerical stability is guaranteed ONLY for a class
      of quasi-definite matrices.

INPUT PARAMETERS:
    Analysis    -   prior analysis with internally stored matrix which will
                    be factorized
    NeedUpper   -   whether upper triangular or lower triangular output is
                    needed

OUTPUT PARAMETERS:
    A           -   Cholesky decomposition of A stored in lower triangular
                    CRS format, i.e. A=L*L' (or upper triangular CRS, with
                    A=U'*U, depending on NeedUpper parameter).
    D           -   array[N], diagonal factor. If no diagonal  factor  was
                    required during analysis  phase,  still  returned  but
                    filled with 1's
    P           -   array[N], pivots. Permutation matrix P is a product of
                    P(0)*P(1)*...*P(N-1), where P(i) is a  permutation  of
                    row/col I and P[I] (with P[I]>=I).
                    If no permutation was requested during analysis phase,
                    still returned but filled with identity permutation.
    
The function returns True  when  factorization  resulted  in nondegenerate
matrix. False is returned when factorization fails (Cholesky factorization
of indefinite matrix) or LDLT factorization has exactly zero  elements  at
the diagonal. In the latter case contents of A, D and P is undefined.

The analysis object is not changed during  the  factorization.  Subsequent
calls to SparseCholeskyFactorize() will result in same factorization being
performed one more time.

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
ae_bool sparsecholeskyfactorize(sparsedecompositionanalysis* analysis,
     ae_bool needupper,
     sparsematrix* a,
     /* Real    */ ae_vector* d,
     /* Integer */ ae_vector* p,
     ae_state *_state)
{
    ae_bool result;

    _sparsematrix_clear(a);
    ae_vector_clear(d);
    ae_vector_clear(p);

    if( needupper )
    {
        result = spsymmfactorize(&analysis->analysis, _state);
        if( !result )
        {
            return result;
        }
        spsymmextract(&analysis->analysis, &analysis->wrka, d, p, _state);
        sparsecopytransposecrsbuf(&analysis->wrka, a, _state);
    }
    else
    {
        result = spsymmfactorize(&analysis->analysis, _state);
        if( !result )
        {
            return result;
        }
        spsymmextract(&analysis->analysis, a, d, p, _state);
    }
    return result;
}


/*************************************************************************
Sparse  Cholesky  decomposition:  update  internally  stored  matrix  with
another one with exactly same sparsity pattern.

This function is a part of the 'expert' sparse Cholesky API:
* SparseCholeskyAnalyze(), that performs symbolic analysis phase and loads
  matrix to be factorized into internal storage
* SparseCholeskySetModType(), that allows to  use  modified  Cholesky/LDLT
  with lower bounds on pivot magnitudes and additional overflow safeguards
* SparseCholeskyFactorize(),  that performs  numeric  factorization  using
  precomputed symbolic analysis and internally stored matrix - and outputs
  result
* SparseCholeskyReload(), that reloads one more matrix with same  sparsity
  pattern into internal storage so  one  may  reuse  previously  allocated
  temporaries and previously performed symbolic analysis

This specific function replaces internally stored  numerical  values  with
ones from another sparse matrix (but having exactly same sparsity  pattern
as one that was used for initial SparseCholeskyAnalyze() call).

NOTE: all internal processing is performed with lower triangular  matrices
      stored  in  CRS  format.  Any  other  storage  formats  and/or upper
      triangular storage means  that  one  format  conversion  and/or  one
      transposition will be performed  internally  for  the  analysis  and
      factorization phases. Thus, highest  performance  is  achieved  when
      input is a lower triangular CRS matrix.

INPUT PARAMETERS:
    Analysis    -   analysis object
    A           -   sparse square matrix in any sparse storage format.  It
                    MUST have exactly same sparsity pattern as that of the
                    matrix that was passed to SparseCholeskyAnalyze().
                    Any difference (missing elements or additional elements)
                    may result in unpredictable and undefined  behavior  -
                    an algorithm may fail due to memory access violation.
    IsUpper     -   whether upper or lower  triangle  is  decomposed  (the
                    other one is ignored).

OUTPUT PARAMETERS:
    Analysis    -   contains:
                    * symbolic analysis of the matrix structure which will
                      be used later to guide numerical factorization.
                    * specific numeric values loaded into internal  memory
                      waiting for the factorization to be performed

  -- ALGLIB routine --
     20.09.2020
     Bochkanov Sergey
*************************************************************************/
void sparsecholeskyreload(sparsedecompositionanalysis* analysis,
     const sparsematrix* a,
     ae_bool isupper,
     ae_state *_state)
{


    ae_assert(sparsegetnrows(a, _state)==sparsegetncols(a, _state), "SparseCholeskyReload: A is not square", _state);
    ae_assert(sparsegetnrows(a, _state)==analysis->n, "SparseCholeskyReload: size of A does not match that stored in Analysis", _state);
    if( !sparseiscrs(a, _state) )
    {
        
        /*
         * The matrix is stored in non-CRS format. First, we have to convert
         * it to CRS. Then we may need to transpose it in order to get lower
         * triangular one (as supported by SPSymmAnalyze).
         */
        sparsecopytocrs(a, &analysis->crsa, _state);
        if( isupper )
        {
            sparsecopytransposecrsbuf(&analysis->crsa, &analysis->crsat, _state);
            spsymmreload(&analysis->analysis, &analysis->crsat, _state);
        }
        else
        {
            spsymmreload(&analysis->analysis, &analysis->crsa, _state);
        }
    }
    else
    {
        
        /*
         * The matrix is stored in CRS format. However we may need to
         * transpose it in order to get lower triangular one (as supported
         * by SPSymmAnalyze).
         */
        if( isupper )
        {
            sparsecopytransposecrsbuf(a, &analysis->crsat, _state);
            spsymmreload(&analysis->analysis, &analysis->crsat, _state);
        }
        else
        {
            spsymmreload(&analysis->analysis, a, _state);
        }
    }
}


void rmatrixlup(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tmp;
    ae_int_t i;
    ae_int_t j;
    double mx;
    double v;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    ae_vector_clear(pivots);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);

    
    /*
     * Internal LU decomposition subroutine.
     * Never call it directly.
     */
    ae_assert(m>0, "RMatrixLUP: incorrect M!", _state);
    ae_assert(n>0, "RMatrixLUP: incorrect N!", _state);
    
    /*
     * Scale matrix to avoid overflows,
     * decompose it, then scale back.
     */
    mx = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            mx = ae_maxreal(mx, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
        }
    }
    if( ae_fp_neq(mx,(double)(0)) )
    {
        v = (double)1/mx;
        for(i=0; i<=m-1; i++)
        {
            ae_v_muld(&a->ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
        }
    }
    ae_vector_set_length(pivots, ae_minint(m, n, _state), _state);
    ae_vector_set_length(&tmp, 2*ae_maxint(m, n, _state), _state);
    rmatrixluprec(a, 0, m, n, pivots, &tmp, _state);
    if( ae_fp_neq(mx,(double)(0)) )
    {
        v = mx;
        for(i=0; i<=m-1; i++)
        {
            ae_v_muld(&a->ptr.pp_double[i][0], 1, ae_v_len(0,ae_minint(i, n-1, _state)), v);
        }
    }
    ae_frame_leave(_state);
}


void cmatrixlup(/* Complex */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tmp;
    ae_int_t i;
    ae_int_t j;
    double mx;
    double v;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    ae_vector_clear(pivots);
    ae_vector_init(&tmp, 0, DT_COMPLEX, _state, ae_true);

    
    /*
     * Internal LU decomposition subroutine.
     * Never call it directly.
     */
    ae_assert(m>0, "CMatrixLUP: incorrect M!", _state);
    ae_assert(n>0, "CMatrixLUP: incorrect N!", _state);
    
    /*
     * Scale matrix to avoid overflows,
     * decompose it, then scale back.
     */
    mx = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            mx = ae_maxreal(mx, ae_c_abs(a->ptr.pp_complex[i][j], _state), _state);
        }
    }
    if( ae_fp_neq(mx,(double)(0)) )
    {
        v = (double)1/mx;
        for(i=0; i<=m-1; i++)
        {
            ae_v_cmuld(&a->ptr.pp_complex[i][0], 1, ae_v_len(0,n-1), v);
        }
    }
    ae_vector_set_length(pivots, ae_minint(m, n, _state), _state);
    ae_vector_set_length(&tmp, 2*ae_maxint(m, n, _state), _state);
    cmatrixluprec(a, 0, m, n, pivots, &tmp, _state);
    if( ae_fp_neq(mx,(double)(0)) )
    {
        v = mx;
        for(i=0; i<=m-1; i++)
        {
            ae_v_cmuld(&a->ptr.pp_complex[i][0], 1, ae_v_len(0,ae_minint(i, n-1, _state)), v);
        }
    }
    ae_frame_leave(_state);
}


void rmatrixplu(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tmp;
    ae_int_t i;
    ae_int_t j;
    double mx;
    double v;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    ae_vector_clear(pivots);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);

    
    /*
     * Internal LU decomposition subroutine.
     * Never call it directly.
     */
    ae_assert(m>0, "RMatrixPLU: incorrect M!", _state);
    ae_assert(n>0, "RMatrixPLU: incorrect N!", _state);
    ae_vector_set_length(&tmp, 2*ae_maxint(m, n, _state), _state);
    ae_vector_set_length(pivots, ae_minint(m, n, _state), _state);
    
    /*
     * Scale matrix to avoid overflows,
     * decompose it, then scale back.
     */
    mx = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            mx = ae_maxreal(mx, ae_fabs(a->ptr.pp_double[i][j], _state), _state);
        }
    }
    if( ae_fp_neq(mx,(double)(0)) )
    {
        v = (double)1/mx;
        for(i=0; i<=m-1; i++)
        {
            ae_v_muld(&a->ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
        }
    }
    rmatrixplurec(a, 0, m, n, pivots, &tmp, _state);
    if( ae_fp_neq(mx,(double)(0)) )
    {
        v = mx;
        for(i=0; i<=ae_minint(m, n, _state)-1; i++)
        {
            ae_v_muld(&a->ptr.pp_double[i][i], 1, ae_v_len(i,n-1), v);
        }
    }
    ae_frame_leave(_state);
}


void cmatrixplu(/* Complex */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector tmp;
    ae_int_t i;
    ae_int_t j;
    double mx;
    ae_complex v;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    ae_vector_clear(pivots);
    ae_vector_init(&tmp, 0, DT_COMPLEX, _state, ae_true);

    
    /*
     * Internal LU decomposition subroutine.
     * Never call it directly.
     */
    ae_assert(m>0, "CMatrixPLU: incorrect M!", _state);
    ae_assert(n>0, "CMatrixPLU: incorrect N!", _state);
    ae_vector_set_length(&tmp, 2*ae_maxint(m, n, _state), _state);
    ae_vector_set_length(pivots, ae_minint(m, n, _state), _state);
    
    /*
     * Scale matrix to avoid overflows,
     * decompose it, then scale back.
     */
    mx = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            mx = ae_maxreal(mx, ae_c_abs(a->ptr.pp_complex[i][j], _state), _state);
        }
    }
    if( ae_fp_neq(mx,(double)(0)) )
    {
        v = ae_complex_from_d((double)1/mx);
        for(i=0; i<=m-1; i++)
        {
            ae_v_cmulc(&a->ptr.pp_complex[i][0], 1, ae_v_len(0,n-1), v);
        }
    }
    cmatrixplurec(a, 0, m, n, pivots, &tmp, _state);
    if( ae_fp_neq(mx,(double)(0)) )
    {
        v = ae_complex_from_d(mx);
        for(i=0; i<=ae_minint(m, n, _state)-1; i++)
        {
            ae_v_cmulc(&a->ptr.pp_complex[i][i], 1, ae_v_len(i,n-1), v);
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Advanced interface for SPDMatrixCholesky, performs no temporary allocations.

INPUT PARAMETERS:
    A       -   matrix given by upper or lower triangle
    Offs    -   offset of diagonal block to decompose
    N       -   diagonal block size
    IsUpper -   what half is given
    Tmp     -   temporary array; allocated by function, if its size is too
                small; can be reused on subsequent calls.
                
OUTPUT PARAMETERS:
    A       -   upper (or lower) triangle contains Cholesky decomposition

RESULT:
    True, on success
    False, on failure

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixcholeskyrec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_bool result;


    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    
    /*
     * Allocate temporaries
     */
    if( tmp->cnt<2*n )
    {
        ae_vector_set_length(tmp, 2*n, _state);
    }
    
    /*
     * Basecases
     */
    if( n<1 )
    {
        result = ae_false;
        return result;
    }
    if( n==1 )
    {
        if( ae_fp_greater(a->ptr.pp_double[offs][offs],(double)(0)) )
        {
            a->ptr.pp_double[offs][offs] = ae_sqrt(a->ptr.pp_double[offs][offs], _state);
            result = ae_true;
        }
        else
        {
            result = ae_false;
        }
        return result;
    }
    if( n<=tsb )
    {
        if( spdmatrixcholeskymkl(a, offs, n, isupper, &result, _state) )
        {
            return result;
        }
    }
    if( n<=tsa )
    {
        result = trfac_spdmatrixcholesky2(a, offs, n, isupper, tmp, _state);
        return result;
    }
    
    /*
     * Split task into smaller ones
     */
    if( n>tsb )
    {
        
        /*
         * Split leading B-sized block from the beginning (block-matrix approach)
         */
        n1 = tsb;
        n2 = n-n1;
    }
    else
    {
        
        /*
         * Smaller than B-size, perform cache-oblivious split
         */
        tiledsplit(n, tsa, &n1, &n2, _state);
    }
    result = spdmatrixcholeskyrec(a, offs, n1, isupper, tmp, _state);
    if( !result )
    {
        return result;
    }
    if( n2>0 )
    {
        if( isupper )
        {
            rmatrixlefttrsm(n1, n2, a, offs, offs, isupper, ae_false, 1, a, offs, offs+n1, _state);
            rmatrixsyrk(n2, n1, -1.0, a, offs, offs+n1, 1, 1.0, a, offs+n1, offs+n1, isupper, _state);
        }
        else
        {
            rmatrixrighttrsm(n2, n1, a, offs, offs, isupper, ae_false, 1, a, offs+n1, offs, _state);
            rmatrixsyrk(n2, n1, -1.0, a, offs+n1, offs, 0, 1.0, a, offs+n1, offs+n1, isupper, _state);
        }
        result = spdmatrixcholeskyrec(a, offs+n1, n2, isupper, tmp, _state);
        if( !result )
        {
            return result;
        }
    }
    return result;
}


/*************************************************************************
Recursive computational subroutine for HPDMatrixCholesky

  -- ALGLIB routine --
     15.12.2009
     Bochkanov Sergey
*************************************************************************/
static ae_bool trfac_hpdmatrixcholeskyrec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_bool result;


    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    
    /*
     * check N
     */
    if( n<1 )
    {
        result = ae_false;
        return result;
    }
    
    /*
     * Prepare buffer
     */
    if( tmp->cnt<2*n )
    {
        ae_vector_set_length(tmp, 2*n, _state);
    }
    
    /*
     * Basecases
     *
     * NOTE: we do not use MKL for basecases because their price is only
     *       minor part of overall running time for N>256.
     */
    if( n==1 )
    {
        if( ae_fp_greater(a->ptr.pp_complex[offs][offs].x,(double)(0)) )
        {
            a->ptr.pp_complex[offs][offs] = ae_complex_from_d(ae_sqrt(a->ptr.pp_complex[offs][offs].x, _state));
            result = ae_true;
        }
        else
        {
            result = ae_false;
        }
        return result;
    }
    if( n<=tsa )
    {
        result = trfac_hpdmatrixcholesky2(a, offs, n, isupper, tmp, _state);
        return result;
    }
    
    /*
     * Split task into smaller ones
     */
    if( n>tsb )
    {
        
        /*
         * Split leading B-sized block from the beginning (block-matrix approach)
         */
        n1 = tsb;
        n2 = n-n1;
    }
    else
    {
        
        /*
         * Smaller than B-size, perform cache-oblivious split
         */
        tiledsplit(n, tsa, &n1, &n2, _state);
    }
    result = trfac_hpdmatrixcholeskyrec(a, offs, n1, isupper, tmp, _state);
    if( !result )
    {
        return result;
    }
    if( n2>0 )
    {
        if( isupper )
        {
            cmatrixlefttrsm(n1, n2, a, offs, offs, isupper, ae_false, 2, a, offs, offs+n1, _state);
            cmatrixherk(n2, n1, -1.0, a, offs, offs+n1, 2, 1.0, a, offs+n1, offs+n1, isupper, _state);
        }
        else
        {
            cmatrixrighttrsm(n2, n1, a, offs, offs, isupper, ae_false, 2, a, offs+n1, offs, _state);
            cmatrixherk(n2, n1, -1.0, a, offs+n1, offs, 0, 1.0, a, offs+n1, offs+n1, isupper, _state);
        }
        result = trfac_hpdmatrixcholeskyrec(a, offs+n1, n2, isupper, tmp, _state);
        if( !result )
        {
            return result;
        }
    }
    return result;
}


/*************************************************************************
Level-2 Hermitian Cholesky subroutine.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992
*************************************************************************/
static ae_bool trfac_hpdmatrixcholesky2(/* Complex */ ae_matrix* aaa,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double ajj;
    ae_complex v;
    double r;
    ae_bool result;


    result = ae_true;
    if( n<0 )
    {
        result = ae_false;
        return result;
    }
    
    /*
     * Quick return if possible
     */
    if( n==0 )
    {
        return result;
    }
    if( isupper )
    {
        
        /*
         * Compute the Cholesky factorization A = U'*U.
         */
        for(j=0; j<=n-1; j++)
        {
            
            /*
             * Compute U(J,J) and test for non-positive-definiteness.
             */
            v = ae_v_cdotproduct(&aaa->ptr.pp_complex[offs][offs+j], aaa->stride, "Conj", &aaa->ptr.pp_complex[offs][offs+j], aaa->stride, "N", ae_v_len(offs,offs+j-1));
            ajj = ae_c_sub(aaa->ptr.pp_complex[offs+j][offs+j],v).x;
            if( ae_fp_less_eq(ajj,(double)(0)) )
            {
                aaa->ptr.pp_complex[offs+j][offs+j] = ae_complex_from_d(ajj);
                result = ae_false;
                return result;
            }
            ajj = ae_sqrt(ajj, _state);
            aaa->ptr.pp_complex[offs+j][offs+j] = ae_complex_from_d(ajj);
            
            /*
             * Compute elements J+1:N-1 of row J.
             */
            if( j<n-1 )
            {
                if( j>0 )
                {
                    ae_v_cmoveneg(&tmp->ptr.p_complex[0], 1, &aaa->ptr.pp_complex[offs][offs+j], aaa->stride, "Conj", ae_v_len(0,j-1));
                    cmatrixmv(n-j-1, j, aaa, offs, offs+j+1, 1, tmp, 0, tmp, n, _state);
                    ae_v_cadd(&aaa->ptr.pp_complex[offs+j][offs+j+1], 1, &tmp->ptr.p_complex[n], 1, "N", ae_v_len(offs+j+1,offs+n-1));
                }
                r = (double)1/ajj;
                ae_v_cmuld(&aaa->ptr.pp_complex[offs+j][offs+j+1], 1, ae_v_len(offs+j+1,offs+n-1), r);
            }
        }
    }
    else
    {
        
        /*
         * Compute the Cholesky factorization A = L*L'.
         */
        for(j=0; j<=n-1; j++)
        {
            
            /*
             * Compute L(J+1,J+1) and test for non-positive-definiteness.
             */
            v = ae_v_cdotproduct(&aaa->ptr.pp_complex[offs+j][offs], 1, "Conj", &aaa->ptr.pp_complex[offs+j][offs], 1, "N", ae_v_len(offs,offs+j-1));
            ajj = ae_c_sub(aaa->ptr.pp_complex[offs+j][offs+j],v).x;
            if( ae_fp_less_eq(ajj,(double)(0)) )
            {
                aaa->ptr.pp_complex[offs+j][offs+j] = ae_complex_from_d(ajj);
                result = ae_false;
                return result;
            }
            ajj = ae_sqrt(ajj, _state);
            aaa->ptr.pp_complex[offs+j][offs+j] = ae_complex_from_d(ajj);
            
            /*
             * Compute elements J+1:N of column J.
             */
            if( j<n-1 )
            {
                r = (double)1/ajj;
                if( j>0 )
                {
                    ae_v_cmove(&tmp->ptr.p_complex[0], 1, &aaa->ptr.pp_complex[offs+j][offs], 1, "Conj", ae_v_len(0,j-1));
                    cmatrixmv(n-j-1, j, aaa, offs+j+1, offs, 0, tmp, 0, tmp, n, _state);
                    for(i=0; i<=n-j-2; i++)
                    {
                        aaa->ptr.pp_complex[offs+j+1+i][offs+j] = ae_c_mul_d(ae_c_sub(aaa->ptr.pp_complex[offs+j+1+i][offs+j],tmp->ptr.p_complex[n+i]),r);
                    }
                }
                else
                {
                    for(i=0; i<=n-j-2; i++)
                    {
                        aaa->ptr.pp_complex[offs+j+1+i][offs+j] = ae_c_mul_d(aaa->ptr.pp_complex[offs+j+1+i][offs+j],r);
                    }
                }
            }
        }
    }
    return result;
}


/*************************************************************************
Level-2 Cholesky subroutine

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992
*************************************************************************/
static ae_bool trfac_spdmatrixcholesky2(/* Real    */ ae_matrix* aaa,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* tmp,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double ajj;
    double v;
    double r;
    ae_bool result;


    result = ae_true;
    if( n<0 )
    {
        result = ae_false;
        return result;
    }
    
    /*
     * Quick return if possible
     */
    if( n==0 )
    {
        return result;
    }
    if( isupper )
    {
        
        /*
         * Compute the Cholesky factorization A = U'*U.
         */
        for(j=0; j<=n-1; j++)
        {
            
            /*
             * Compute U(J,J) and test for non-positive-definiteness.
             */
            v = ae_v_dotproduct(&aaa->ptr.pp_double[offs][offs+j], aaa->stride, &aaa->ptr.pp_double[offs][offs+j], aaa->stride, ae_v_len(offs,offs+j-1));
            ajj = aaa->ptr.pp_double[offs+j][offs+j]-v;
            if( ae_fp_less_eq(ajj,(double)(0)) )
            {
                aaa->ptr.pp_double[offs+j][offs+j] = ajj;
                result = ae_false;
                return result;
            }
            ajj = ae_sqrt(ajj, _state);
            aaa->ptr.pp_double[offs+j][offs+j] = ajj;
            
            /*
             * Compute elements J+1:N-1 of row J.
             */
            if( j<n-1 )
            {
                if( j>0 )
                {
                    ae_v_moveneg(&tmp->ptr.p_double[0], 1, &aaa->ptr.pp_double[offs][offs+j], aaa->stride, ae_v_len(0,j-1));
                    rmatrixmv(n-j-1, j, aaa, offs, offs+j+1, 1, tmp, 0, tmp, n, _state);
                    ae_v_add(&aaa->ptr.pp_double[offs+j][offs+j+1], 1, &tmp->ptr.p_double[n], 1, ae_v_len(offs+j+1,offs+n-1));
                }
                r = (double)1/ajj;
                ae_v_muld(&aaa->ptr.pp_double[offs+j][offs+j+1], 1, ae_v_len(offs+j+1,offs+n-1), r);
            }
        }
    }
    else
    {
        
        /*
         * Compute the Cholesky factorization A = L*L'.
         */
        for(j=0; j<=n-1; j++)
        {
            
            /*
             * Compute L(J+1,J+1) and test for non-positive-definiteness.
             */
            v = ae_v_dotproduct(&aaa->ptr.pp_double[offs+j][offs], 1, &aaa->ptr.pp_double[offs+j][offs], 1, ae_v_len(offs,offs+j-1));
            ajj = aaa->ptr.pp_double[offs+j][offs+j]-v;
            if( ae_fp_less_eq(ajj,(double)(0)) )
            {
                aaa->ptr.pp_double[offs+j][offs+j] = ajj;
                result = ae_false;
                return result;
            }
            ajj = ae_sqrt(ajj, _state);
            aaa->ptr.pp_double[offs+j][offs+j] = ajj;
            
            /*
             * Compute elements J+1:N of column J.
             */
            if( j<n-1 )
            {
                r = (double)1/ajj;
                if( j>0 )
                {
                    ae_v_move(&tmp->ptr.p_double[0], 1, &aaa->ptr.pp_double[offs+j][offs], 1, ae_v_len(0,j-1));
                    rmatrixmv(n-j-1, j, aaa, offs+j+1, offs, 0, tmp, 0, tmp, n, _state);
                    for(i=0; i<=n-j-2; i++)
                    {
                        aaa->ptr.pp_double[offs+j+1+i][offs+j] = (aaa->ptr.pp_double[offs+j+1+i][offs+j]-tmp->ptr.p_double[n+i])*r;
                    }
                }
                else
                {
                    for(i=0; i<=n-j-2; i++)
                    {
                        aaa->ptr.pp_double[offs+j+1+i][offs+j] = aaa->ptr.pp_double[offs+j+1+i][offs+j]*r;
                    }
                }
            }
        }
    }
    return result;
}


void _sparsedecompositionanalysis_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sparsedecompositionanalysis *p = (sparsedecompositionanalysis*)_p;
    ae_touch_ptr((void*)p);
    _spcholanalysis_init(&p->analysis, _state, make_automatic);
    _sparsematrix_init(&p->wrka, _state, make_automatic);
    _sparsematrix_init(&p->wrkat, _state, make_automatic);
    _sparsematrix_init(&p->crsa, _state, make_automatic);
    _sparsematrix_init(&p->crsat, _state, make_automatic);
}


void _sparsedecompositionanalysis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sparsedecompositionanalysis       *dst = (sparsedecompositionanalysis*)_dst;
    const sparsedecompositionanalysis *src = (const sparsedecompositionanalysis*)_src;
    dst->n = src->n;
    dst->facttype = src->facttype;
    dst->permtype = src->permtype;
    _spcholanalysis_init_copy(&dst->analysis, &src->analysis, _state, make_automatic);
    _sparsematrix_init_copy(&dst->wrka, &src->wrka, _state, make_automatic);
    _sparsematrix_init_copy(&dst->wrkat, &src->wrkat, _state, make_automatic);
    _sparsematrix_init_copy(&dst->crsa, &src->crsa, _state, make_automatic);
    _sparsematrix_init_copy(&dst->crsat, &src->crsat, _state, make_automatic);
}


void _sparsedecompositionanalysis_clear(void* _p)
{
    sparsedecompositionanalysis *p = (sparsedecompositionanalysis*)_p;
    ae_touch_ptr((void*)p);
    _spcholanalysis_clear(&p->analysis);
    _sparsematrix_clear(&p->wrka);
    _sparsematrix_clear(&p->wrkat);
    _sparsematrix_clear(&p->crsa);
    _sparsematrix_clear(&p->crsat);
}


void _sparsedecompositionanalysis_destroy(void* _p)
{
    sparsedecompositionanalysis *p = (sparsedecompositionanalysis*)_p;
    ae_touch_ptr((void*)p);
    _spcholanalysis_destroy(&p->analysis);
    _sparsematrix_destroy(&p->wrka);
    _sparsematrix_destroy(&p->wrkat);
    _sparsematrix_destroy(&p->crsa);
    _sparsematrix_destroy(&p->crsat);
}


/*$ End $*/
