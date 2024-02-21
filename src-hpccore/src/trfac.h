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

#ifndef _trfac_h
#define _trfac_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "scodes.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "creflections.h"
#include "matgen.h"
#include "rotations.h"


/*$ Declarations $*/


/*************************************************************************
An analysis of the sparse matrix decomposition, performed prior to  actual
numerical factorization. You should not directly  access  fields  of  this
object - use appropriate ALGLIB functions to work with this object.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t facttype;
    ae_int_t permtype;
    spcholanalysis analysis;
    sparsematrix wrka;
    sparsematrix wrkat;
    sparsematrix crsa;
    sparsematrix crsat;
} sparsedecompositionanalysis;


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
ae_bool sparsecholesky(sparsematrix* a, ae_bool isupper, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


void rmatrixlup(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state);


void cmatrixlup(/* Complex */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state);


void rmatrixplu(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state);


void cmatrixplu(/* Complex */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state);


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
     ae_state *_state);
void _sparsedecompositionanalysis_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sparsedecompositionanalysis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sparsedecompositionanalysis_clear(void* _p);
void _sparsedecompositionanalysis_destroy(void* _p);


/*$ End $*/
#endif

