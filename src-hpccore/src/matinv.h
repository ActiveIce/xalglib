###########################################################################
# ALGLIB 4.00.0 (source code generated 2023-05-21)
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

#ifndef _matinv_h
#define _matinv_h

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
#include "trfac.h"
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"


/*$ Declarations $*/


/*************************************************************************
Matrix inverse report:
* terminationtype   completion code:
                    *  1 for success
                    * -3 for failure due to the matrix being singular or
                         nearly-singular
* r1                reciprocal of condition number in 1-norm
* rinf              reciprocal of condition number in inf-norm
*************************************************************************/
typedef struct
{
    ae_int_t terminationtype;
    double r1;
    double rinf;
} matinvreport;


/*$ Body $*/


/*************************************************************************
Inversion of a matrix given by its LU decomposition.

INPUT PARAMETERS:
    A       -   LU decomposition of the matrix
                (output of RMatrixLU subroutine).
    Pivots  -   table of permutations
                (the output of RMatrixLU subroutine).
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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
     05.02.2010
     Bochkanov Sergey
*************************************************************************/
void rmatrixluinverse(/* Real    */ ae_matrix* a,
     /* Integer */ const ae_vector* pivots,
     ae_int_t n,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Inversion of a general matrix.

INPUT PARAMETERS:
    A       -   matrix.
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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

  -- ALGLIB --
     Copyright 2005-2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixinverse(/* Real    */ ae_matrix* a,
     ae_int_t n,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Inversion of a matrix given by its LU decomposition.

INPUT PARAMETERS:
    A       -   LU decomposition of the matrix
                (output of CMatrixLU subroutine).
    Pivots  -   table of permutations
                (the output of CMatrixLU subroutine).
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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
     05.02.2010
     Bochkanov Sergey
*************************************************************************/
void cmatrixluinverse(/* Complex */ ae_matrix* a,
     /* Integer */ const ae_vector* pivots,
     ae_int_t n,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Inversion of a general matrix.

Input parameters:
    A       -   matrix
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)

Output parameters:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void cmatrixinverse(/* Complex */ ae_matrix* a,
     ae_int_t n,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Inversion of a symmetric positive definite matrix which is given
by Cholesky decomposition.

INPUT PARAMETERS:
    A       -   Cholesky decomposition of the matrix to be inverted:
                A=U'*U or A = L*L'.
                Output of  SPDMatrixCholesky subroutine.
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)
    IsUpper -   storage type:
                * if True, the symmetric  matrix  A  is given by its upper
                  triangle, and the lower triangle isn't  used/changed  by
                  the function
                * if False, the symmetric matrix  A  is given by its lower
                  triangle, and the  upper triangle isn't used/changed  by
                  the function

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0,   corresponding      triangle
                  contains inverse matrix,   the  other  triangle  is  not
                  modified.
                * for rep.terminationtype<0,  corresponding  triangle   is
                  zero-filled; the other triangle is not modified.
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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
     10.02.2010
     Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskyinverse(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Inversion of a symmetric positive definite matrix.

Given an upper or lower triangle of a symmetric positive definite matrix,
the algorithm generates matrix A^-1 and saves the upper or lower triangle
depending on the input.

INPUT PARAMETERS:
    A       -   matrix to be inverted (upper or lower triangle), array[N,N]
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)
    IsUpper -   storage type:
                * if True, symmetric  matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't  used/changed  by
                  function
                * if False,  symmetric matrix  A  is  given  by  its lower
                  triangle, and the  upper triangle isn't used/changed  by
                  function

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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
     10.02.2010
     Bochkanov Sergey
*************************************************************************/
void spdmatrixinverse(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Inversion of a Hermitian positive definite matrix which is given
by Cholesky decomposition.

Input parameters:
    A       -   Cholesky decomposition of the matrix to be inverted:
                A=U'*U or A = L*L'.
                Output of  HPDMatrixCholesky subroutine.
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)
    IsUpper -   storage type:
                * if True, symmetric  matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't  used/changed  by
                  function
                * if False,  symmetric matrix  A  is  given  by  its lower
                  triangle, and the  upper triangle isn't used/changed  by
                  function

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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
     10.02.2010
     Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskyinverse(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Inversion of a Hermitian positive definite matrix.

Given an upper or lower triangle of a Hermitian positive definite matrix,
the algorithm generates matrix A^-1 and saves the upper or lower triangle
depending on the input.
  
INPUT PARAMETERS:
    A       -   matrix to be inverted (upper or lower triangle), array[N,N]
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)
    IsUpper -   storage type:
                * if True, symmetric  matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't  used/changed  by
                  function
                * if False,  symmetric matrix  A  is  given  by  its lower
                  triangle, and the  upper triangle isn't used/changed  by
                  function

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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
     10.02.2010
     Bochkanov Sergey
*************************************************************************/
void hpdmatrixinverse(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Triangular matrix inverse (real)

The subroutine inverts the following types of matrices:
    * upper triangular
    * upper triangular with unit diagonal
    * lower triangular
    * lower triangular with unit diagonal

In case of an upper (lower) triangular matrix,  the  inverse  matrix  will
also be upper (lower) triangular, and after the end of the algorithm,  the
inverse matrix replaces the source matrix. The elements  below (above) the
main diagonal are not changed by the algorithm.

If  the matrix  has a unit diagonal, the inverse matrix also  has  a  unit
diagonal, and the diagonal elements are not passed to the algorithm.
  
INPUT PARAMETERS:
    A       -   matrix, array[0..N-1, 0..N-1].
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)
    IsUpper -   True, if the matrix is upper triangular.
    IsUnit  -   diagonal type (optional):
                * if True, matrix has unit diagonal (a[i,i] are NOT used)
                * if False, matrix diagonal is arbitrary
                * if not given, False is assumed

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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

  -- ALGLIB --
     Copyright 05.02.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixtrinverse(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_bool isunit,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Triangular matrix inverse (complex)

The subroutine inverts the following types of matrices:
    * upper triangular
    * upper triangular with unit diagonal
    * lower triangular
    * lower triangular with unit diagonal

In case of an upper (lower) triangular matrix,  the  inverse  matrix  will
also be upper (lower) triangular, and after the end of the algorithm,  the
inverse matrix replaces the source matrix. The elements  below (above) the
main diagonal are not changed by the algorithm.

If  the matrix  has a unit diagonal, the inverse matrix also  has  a  unit
diagonal, and the diagonal elements are not passed to the algorithm.

INPUT PARAMETERS:
    A       -   matrix, array[0..N-1, 0..N-1].
    N       -   size of the matrix A (optional):
                * if given, only principal NxN submatrix is processed  and
                  overwritten. Trailing elements are unchanged.
                * if not given, the size  is automatically determined from
                  the matrix size (A must be a square matrix)
    IsUpper -   True, if the matrix is upper triangular.
    IsUnit  -   diagonal type (optional):
                * if True, matrix has unit diagonal (a[i,i] are NOT used)
                * if False, matrix diagonal is arbitrary
                * if not given, False is assumed

OUTPUT PARAMETERS:
    A       -   inverse of matrix A, array[N,N]:
                * for rep.terminationtype>0, contains matrix inverse
                * for rep.terminationtype<0, zero-filled
    Rep     -   solver report:
                * rep.terminationtype>0 for success, <0 for failure
                * see below for more info

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* terminationtype   completion code:
                    *  1 for success 
                    * -3 for a singular or extremely ill-conditioned matrix
* r1                reciprocal of condition number: 1/cond(A), 1-norm.
* rinf              reciprocal of condition number: 1/cond(A), inf-norm.

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

  -- ALGLIB --
     Copyright 05.02.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixtrinverse(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     ae_bool isunit,
     matinvreport* rep,
     ae_state *_state);


/*************************************************************************
Recursive subroutine for SPD inversion.

NOTE: this function expects that matris is strictly positive-definite.

  -- ALGLIB routine --
     10.02.2010
     Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskyinverserec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* tmp,
     matinvreport* rep,
     ae_state *_state);
void _spawn_spdmatrixcholeskyinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    /* Real    */ ae_vector* tmp,
    matinvreport* rep, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spdmatrixcholeskyinverserec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_spdmatrixcholeskyinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    /* Real    */ ae_vector* tmp,
    matinvreport* rep, ae_state *_state);
void _matinvreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _matinvreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _matinvreport_clear(void* _p);
void _matinvreport_destroy(void* _p);


/*$ End $*/
#endif

