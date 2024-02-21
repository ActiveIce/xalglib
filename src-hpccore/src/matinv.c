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
#include "matinv.h"


/*$ Declarations $*/
static void matinv_rmatrixtrinverserec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     ae_bool isunit,
     /* Real    */ ae_vector* tmp,
     matinvreport* rep,
     ae_state *_state);
void _spawn_matinv_rmatrixtrinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    ae_bool isunit,
    /* Real    */ ae_vector* tmp,
    matinvreport* rep, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_matinv_rmatrixtrinverserec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_matinv_rmatrixtrinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    ae_bool isunit,
    /* Real    */ ae_vector* tmp,
    matinvreport* rep, ae_state *_state);
static void matinv_cmatrixtrinverserec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     ae_bool isunit,
     /* Complex */ ae_vector* tmp,
     matinvreport* rep,
     ae_state *_state);
void _spawn_matinv_cmatrixtrinverserec(/* Complex */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    ae_bool isunit,
    /* Complex */ ae_vector* tmp,
    matinvreport* rep, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_matinv_cmatrixtrinverserec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_matinv_cmatrixtrinverserec(/* Complex */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    ae_bool isunit,
    /* Complex */ ae_vector* tmp,
    matinvreport* rep, ae_state *_state);
static void matinv_rmatrixluinverserec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     /* Real    */ ae_vector* work,
     matinvreport* rep,
     ae_state *_state);
void _spawn_matinv_rmatrixluinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    /* Real    */ ae_vector* work,
    matinvreport* rep, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_matinv_rmatrixluinverserec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_matinv_rmatrixluinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    /* Real    */ ae_vector* work,
    matinvreport* rep, ae_state *_state);
static void matinv_cmatrixluinverserec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     /* Complex */ ae_vector* work,
     matinvreport* rep,
     ae_state *_state);
void _spawn_matinv_cmatrixluinverserec(/* Complex */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    /* Complex */ ae_vector* work,
    matinvreport* rep, ae_task_group *_group, ae_bool smp_enabled, ae_state *_state);
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_matinv_cmatrixluinverserec(ae_task_data *_data, ae_state *_state);
#endif
ae_bool _trypexec_matinv_cmatrixluinverserec(/* Complex */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    /* Complex */ ae_vector* work,
    matinvreport* rep, ae_state *_state);
static void matinv_hpdmatrixcholeskyinverserec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* tmp,
     matinvreport* rep,
     ae_state *_state);


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector work;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;

    ae_frame_make(_state, &_frame_block);
    memset(&work, 0, sizeof(work));
    _matinvreport_clear(rep);
    ae_vector_init(&work, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixLUInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "RMatrixLUInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "RMatrixLUInverse: rows(A)<N!", _state);
    ae_assert(pivots->cnt>=n, "RMatrixLUInverse: len(Pivots)<N!", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixLUInverse: A contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        if( pivots->ptr.p_int[i]>n-1||pivots->ptr.p_int[i]<0 )
        {
            ae_assert(ae_false, "RMatrixLUInverse: incorrect Pivots array!", _state);
        }
    }
    
    /*
     * calculate condition numbers
     */
    rep->terminationtype = 1;
    rep->r1 = rmatrixlurcond1(a, n, _state);
    rep->rinf = rmatrixlurcondinf(a, n, _state);
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                a->ptr.pp_double[i][j] = (double)(0);
            }
        }
        rep->terminationtype = -3;
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Call cache-oblivious code
     */
    ae_vector_set_length(&work, n, _state);
    matinv_rmatrixluinverserec(a, 0, n, &work, rep, _state);
    
    /*
     * apply permutations
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=n-2; j>=0; j--)
        {
            k = pivots->ptr.p_int[j];
            v = a->ptr.pp_double[i][j];
            a->ptr.pp_double[i][j] = a->ptr.pp_double[i][k];
            a->ptr.pp_double[i][k] = v;
        }
    }
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector pivots;

    ae_frame_make(_state, &_frame_block);
    memset(&pivots, 0, sizeof(pivots));
    _matinvreport_clear(rep);
    ae_vector_init(&pivots, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "RMatrixInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "RMatrixInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "RMatrixInverse: rows(A)<N!", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixInverse: A contains infinite or NaN values!", _state);
    rmatrixlu(a, n, n, &pivots, _state);
    rmatrixluinverse(a, &pivots, n, rep, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector work;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_complex v;

    ae_frame_make(_state, &_frame_block);
    memset(&work, 0, sizeof(work));
    _matinvreport_clear(rep);
    ae_vector_init(&work, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixLUInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "CMatrixLUInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "CMatrixLUInverse: rows(A)<N!", _state);
    ae_assert(pivots->cnt>=n, "CMatrixLUInverse: len(Pivots)<N!", _state);
    ae_assert(apservisfinitecmatrix(a, n, n, _state), "CMatrixLUInverse: A contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        if( pivots->ptr.p_int[i]>n-1||pivots->ptr.p_int[i]<0 )
        {
            ae_assert(ae_false, "CMatrixLUInverse: incorrect Pivots array!", _state);
        }
    }
    
    /*
     * calculate condition numbers
     */
    rep->terminationtype = 1;
    rep->r1 = cmatrixlurcond1(a, n, _state);
    rep->rinf = cmatrixlurcondinf(a, n, _state);
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                a->ptr.pp_complex[i][j] = ae_complex_from_i(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Call cache-oblivious code
     */
    ae_vector_set_length(&work, n, _state);
    matinv_cmatrixluinverserec(a, 0, n, &work, rep, _state);
    
    /*
     * apply permutations
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=n-2; j>=0; j--)
        {
            k = pivots->ptr.p_int[j];
            v = a->ptr.pp_complex[i][j];
            a->ptr.pp_complex[i][j] = a->ptr.pp_complex[i][k];
            a->ptr.pp_complex[i][k] = v;
        }
    }
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector pivots;

    ae_frame_make(_state, &_frame_block);
    memset(&pivots, 0, sizeof(pivots));
    _matinvreport_clear(rep);
    ae_vector_init(&pivots, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "CRMatrixInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "CRMatrixInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "CRMatrixInverse: rows(A)<N!", _state);
    ae_assert(apservisfinitecmatrix(a, n, n, _state), "CMatrixInverse: A contains infinite or NaN values!", _state);
    cmatrixlu(a, n, n, &pivots, _state);
    cmatrixluinverse(a, &pivots, n, rep, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    _matinvreport_clear(rep);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixCholeskyInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "SPDMatrixCholeskyInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "SPDMatrixCholeskyInverse: rows(A)<N!", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixCholeskyInverse: A contains infinite or NaN values!", _state);
    
    /*
     * calculate condition numbers
     */
    rep->terminationtype = 1;
    rep->r1 = spdmatrixcholeskyrcond(a, n, isupper, _state);
    rep->rinf = rep->r1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        if( isupper )
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=i; j<=n-1; j++)
                {
                    a->ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        else
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=i; j++)
                {
                    a->ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Inverse
     */
    ae_vector_set_length(&tmp, n, _state);
    spdmatrixcholeskyinverserec(a, 0, n, isupper, &tmp, rep, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{

    _matinvreport_clear(rep);

    ae_assert(n>0, "SPDMatrixInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "SPDMatrixInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "SPDMatrixInverse: rows(A)<N!", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixInverse: A contains infinite or NaN values!", _state);
    rep->r1 = (double)(0);
    rep->rinf = (double)(0);
    rep->terminationtype = -3;
    if( !spdmatrixcholesky(a, n, isupper, _state) )
    {
        return;
    }
    spdmatrixcholeskyinverse(a, n, isupper, rep, _state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    _matinvreport_clear(rep);
    ae_vector_init(&tmp, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixCholeskyInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "HPDMatrixCholeskyInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "HPDMatrixCholeskyInverse: rows(A)<N!", _state);
    ae_assert(isfinitectrmatrix(a, n, isupper, _state), "HPDMatrixCholeskyInverse: A contains infinite/NAN values!", _state);
    
    /*
     * calculate condition numbers
     */
    rep->terminationtype = 1;
    rep->r1 = hpdmatrixcholeskyrcond(a, n, isupper, _state);
    rep->rinf = rep->r1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        if( isupper )
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=i; j<=n-1; j++)
                {
                    a->ptr.pp_complex[i][j] = ae_complex_from_i(0);
                }
            }
        }
        else
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=i; j++)
                {
                    a->ptr.pp_complex[i][j] = ae_complex_from_i(0);
                }
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Inverse
     */
    ae_vector_set_length(&tmp, n, _state);
    matinv_hpdmatrixcholeskyinverserec(a, 0, n, isupper, &tmp, rep, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{

    _matinvreport_clear(rep);

    ae_assert(n>0, "HPDMatrixInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "HPDMatrixInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "HPDMatrixInverse: rows(A)<N!", _state);
    ae_assert(apservisfinitectrmatrix(a, n, isupper, _state), "HPDMatrixInverse: A contains infinite or NaN values!", _state);
    rep->r1 = (double)(0);
    rep->rinf = (double)(0);
    rep->terminationtype = -3;
    if( !hpdmatrixcholesky(a, n, isupper, _state) )
    {
        return;
    }
    hpdmatrixcholeskyinverse(a, n, isupper, rep, _state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    _matinvreport_clear(rep);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixTRInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "RMatrixTRInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "RMatrixTRInverse: rows(A)<N!", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "RMatrixTRInverse: A contains infinite or NaN values!", _state);
    
    /*
     * calculate condition numbers
     */
    rep->terminationtype = 1;
    rep->r1 = rmatrixtrrcond1(a, n, isupper, isunit, _state);
    rep->rinf = rmatrixtrrcondinf(a, n, isupper, isunit, _state);
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                a->ptr.pp_double[i][j] = (double)(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Invert
     */
    ae_vector_set_length(&tmp, n, _state);
    matinv_rmatrixtrinverserec(a, 0, n, isupper, isunit, &tmp, rep, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector tmp;

    ae_frame_make(_state, &_frame_block);
    memset(&tmp, 0, sizeof(tmp));
    _matinvreport_clear(rep);
    ae_vector_init(&tmp, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixTRInverse: N<=0!", _state);
    ae_assert(a->cols>=n, "CMatrixTRInverse: cols(A)<N!", _state);
    ae_assert(a->rows>=n, "CMatrixTRInverse: rows(A)<N!", _state);
    ae_assert(apservisfinitectrmatrix(a, n, isupper, _state), "CMatrixTRInverse: A contains infinite or NaN values!", _state);
    
    /*
     * calculate condition numbers
     */
    rep->terminationtype = 1;
    rep->r1 = cmatrixtrrcond1(a, n, isupper, isunit, _state);
    rep->rinf = cmatrixtrrcondinf(a, n, isupper, isunit, _state);
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                a->ptr.pp_complex[i][j] = ae_complex_from_i(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Invert
     */
    ae_vector_set_length(&tmp, n, _state);
    matinv_cmatrixtrinverserec(a, 0, n, isupper, isunit, &tmp, rep, _state);
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    if( n<1 )
    {
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( n<=tsb )
    {
        tscur = tsa;
    }
    
    /*
     * Base case
     */
    if( n<=tsa )
    {
        matinv_rmatrixtrinverserec(a, offs, n, isupper, ae_false, tmp, rep, _state);
        ae_assert(rep->terminationtype>0, "SPDMatrixCholeskyInverseRec: integrity check failed", _state);
        if( isupper )
        {
            
            /*
             * Compute the product U * U'.
             * NOTE: we never assume that diagonal of U is real
             */
            for(i=0; i<=n-1; i++)
            {
                if( i==0 )
                {
                    
                    /*
                     * 1x1 matrix
                     */
                    a->ptr.pp_double[offs+i][offs+i] = ae_sqr(a->ptr.pp_double[offs+i][offs+i], _state);
                }
                else
                {
                    
                    /*
                     * (I+1)x(I+1) matrix,
                     *
                     * ( A11  A12 )   ( A11^H        )   ( A11*A11^H+A12*A12^H  A12*A22^H )
                     * (          ) * (              ) = (                                )
                     * (      A22 )   ( A12^H  A22^H )   ( A22*A12^H            A22*A22^H )
                     *
                     * A11 is IxI, A22 is 1x1.
                     */
                    ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs][offs+i], a->stride, ae_v_len(0,i-1));
                    for(j=0; j<=i-1; j++)
                    {
                        v = a->ptr.pp_double[offs+j][offs+i];
                        ae_v_addd(&a->ptr.pp_double[offs+j][offs+j], 1, &tmp->ptr.p_double[j], 1, ae_v_len(offs+j,offs+i-1), v);
                    }
                    v = a->ptr.pp_double[offs+i][offs+i];
                    ae_v_muld(&a->ptr.pp_double[offs][offs+i], a->stride, ae_v_len(offs,offs+i-1), v);
                    a->ptr.pp_double[offs+i][offs+i] = ae_sqr(a->ptr.pp_double[offs+i][offs+i], _state);
                }
            }
        }
        else
        {
            
            /*
             * Compute the product L' * L
             * NOTE: we never assume that diagonal of L is real
             */
            for(i=0; i<=n-1; i++)
            {
                if( i==0 )
                {
                    
                    /*
                     * 1x1 matrix
                     */
                    a->ptr.pp_double[offs+i][offs+i] = ae_sqr(a->ptr.pp_double[offs+i][offs+i], _state);
                }
                else
                {
                    
                    /*
                     * (I+1)x(I+1) matrix,
                     *
                     * ( A11^H  A21^H )   ( A11      )   ( A11^H*A11+A21^H*A21  A21^H*A22 )
                     * (              ) * (          ) = (                                )
                     * (        A22^H )   ( A21  A22 )   ( A22^H*A21            A22^H*A22 )
                     *
                     * A11 is IxI, A22 is 1x1.
                     */
                    ae_v_move(&tmp->ptr.p_double[0], 1, &a->ptr.pp_double[offs+i][offs], 1, ae_v_len(0,i-1));
                    for(j=0; j<=i-1; j++)
                    {
                        v = a->ptr.pp_double[offs+i][offs+j];
                        ae_v_addd(&a->ptr.pp_double[offs+j][offs], 1, &tmp->ptr.p_double[0], 1, ae_v_len(offs,offs+j), v);
                    }
                    v = a->ptr.pp_double[offs+i][offs+i];
                    ae_v_muld(&a->ptr.pp_double[offs+i][offs], 1, ae_v_len(offs,offs+i-1), v);
                    a->ptr.pp_double[offs+i][offs+i] = ae_sqr(a->ptr.pp_double[offs+i][offs+i], _state);
                }
            }
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive code: triangular factor inversion merged with
     * UU' or L'L multiplication
     */
    tiledsplit(n, tscur, &n1, &n2, _state);
    
    /*
     * form off-diagonal block of trangular inverse
     */
    if( isupper )
    {
        for(i=0; i<=n1-1; i++)
        {
            ae_v_muld(&a->ptr.pp_double[offs+i][offs+n1], 1, ae_v_len(offs+n1,offs+n-1), -1.0);
        }
        rmatrixlefttrsm(n1, n2, a, offs, offs, isupper, ae_false, 0, a, offs, offs+n1, _state);
        rmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, isupper, ae_false, 0, a, offs, offs+n1, _state);
    }
    else
    {
        for(i=0; i<=n2-1; i++)
        {
            ae_v_muld(&a->ptr.pp_double[offs+n1+i][offs], 1, ae_v_len(offs,offs+n1-1), -1.0);
        }
        rmatrixrighttrsm(n2, n1, a, offs, offs, isupper, ae_false, 0, a, offs+n1, offs, _state);
        rmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, isupper, ae_false, 0, a, offs+n1, offs, _state);
    }
    
    /*
     * invert first diagonal block
     */
    spdmatrixcholeskyinverserec(a, offs, n1, isupper, tmp, rep, _state);
    
    /*
     * update first diagonal block with off-diagonal block,
     * update off-diagonal block
     */
    if( isupper )
    {
        rmatrixsyrk(n1, n2, 1.0, a, offs, offs+n1, 0, 1.0, a, offs, offs, isupper, _state);
        rmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, isupper, ae_false, 1, a, offs, offs+n1, _state);
    }
    else
    {
        rmatrixsyrk(n1, n2, 1.0, a, offs+n1, offs, 1, 1.0, a, offs, offs, isupper, _state);
        rmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, isupper, ae_false, 1, a, offs+n1, offs, _state);
    }
    
    /*
     * invert second diagonal block
     */
    spdmatrixcholeskyinverserec(a, offs+n1, n2, isupper, tmp, rep, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_spdmatrixcholeskyinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    /* Real    */ ae_vector* tmp,
    matinvreport* rep,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        spdmatrixcholeskyinverserec(a,offs,n,isupper,tmp,rep, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_spdmatrixcholeskyinverserec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = a;
        _task->data.parameters[1].value.ival = offs;
        _task->data.parameters[2].value.ival = n;
        _task->data.parameters[3].value.bval = isupper;
        _task->data.parameters[4].value.val = tmp;
        _task->data.parameters[5].value.val = rep;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_spdmatrixcholeskyinverserec(ae_task_data *_data, ae_state *_state)
{
    ae_matrix* a;
    ae_int_t offs;
    ae_int_t n;
    ae_bool isupper;
    ae_vector* tmp;
    matinvreport* rep;
    a = (ae_matrix*)_data->parameters[0].value.val;
    offs = _data->parameters[1].value.ival;
    n = _data->parameters[2].value.ival;
    isupper = _data->parameters[3].value.bval;
    tmp = (ae_vector*)_data->parameters[4].value.val;
    rep = (matinvreport*)_data->parameters[5].value.val;
   ae_state_set_flags(_state, _data->flags);
   spdmatrixcholeskyinverserec(a,offs,n,isupper,tmp,rep, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_spdmatrixcholeskyinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    /* Real    */ ae_vector* tmp,
    matinvreport* rep,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_spdmatrixcholeskyinverserec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = a;
    _task->data.parameters[1].value.ival = offs;
    _task->data.parameters[2].value.ival = n;
    _task->data.parameters[3].value.bval = isupper;
    _task->data.parameters[4].value.val = tmp;
    _task->data.parameters[5].value.val = rep;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


/*************************************************************************
Triangular matrix inversion, recursive subroutine

NOTE: this function sets Rep.TermiantionType on failure, leaves it unchanged on success.

NOTE: only Tmp[Offs:Offs+N-1] is modified, other entries of the temporary array are not modified

  -- ALGLIB --
     05.02.2010, Bochkanov Sergey.
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992.
*************************************************************************/
static void matinv_rmatrixtrinverserec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     ae_bool isunit,
     /* Real    */ ae_vector* tmp,
     matinvreport* rep,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t mn;
    ae_int_t i;
    ae_int_t j;
    double v;
    double ajj;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    ae_assert(n>=1, "MATINV: integrity check 6755 failed", _state);
    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( n<=tsb )
    {
        tscur = tsa;
    }
    
    /*
     * Try to activate parallelism
     */
    if( n>=2*tsb&&ae_fp_greater_eq(rmul3((double)(n), (double)(n), (double)(n), _state)*((double)1/(double)3),smpactivationlevel(_state)) )
    {
        if( _trypexec_matinv_rmatrixtrinverserec(a,offs,n,isupper,isunit,tmp,rep, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Base case
     */
    if( n<=tsa )
    {
        if( isupper )
        {
            
            /*
             * Compute inverse of upper triangular matrix.
             */
            for(j=0; j<=n-1; j++)
            {
                if( !isunit )
                {
                    if( ae_fp_eq(a->ptr.pp_double[offs+j][offs+j],(double)(0)) )
                    {
                        rep->terminationtype = -3;
                        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
                        return;
                    }
                    a->ptr.pp_double[offs+j][offs+j] = (double)1/a->ptr.pp_double[offs+j][offs+j];
                    ajj = -a->ptr.pp_double[offs+j][offs+j];
                }
                else
                {
                    ajj = (double)(-1);
                }
                
                /*
                 * Compute elements 1:j-1 of j-th column.
                 */
                if( j>0 )
                {
                    ae_v_move(&tmp->ptr.p_double[offs+0], 1, &a->ptr.pp_double[offs+0][offs+j], a->stride, ae_v_len(offs+0,offs+j-1));
                    for(i=0; i<=j-1; i++)
                    {
                        if( i<j-1 )
                        {
                            v = ae_v_dotproduct(&a->ptr.pp_double[offs+i][offs+i+1], 1, &tmp->ptr.p_double[offs+i+1], 1, ae_v_len(offs+i+1,offs+j-1));
                        }
                        else
                        {
                            v = (double)(0);
                        }
                        if( !isunit )
                        {
                            a->ptr.pp_double[offs+i][offs+j] = v+a->ptr.pp_double[offs+i][offs+i]*tmp->ptr.p_double[offs+i];
                        }
                        else
                        {
                            a->ptr.pp_double[offs+i][offs+j] = v+tmp->ptr.p_double[offs+i];
                        }
                    }
                    ae_v_muld(&a->ptr.pp_double[offs+0][offs+j], a->stride, ae_v_len(offs+0,offs+j-1), ajj);
                }
            }
        }
        else
        {
            
            /*
             * Compute inverse of lower triangular matrix.
             */
            for(j=n-1; j>=0; j--)
            {
                if( !isunit )
                {
                    if( ae_fp_eq(a->ptr.pp_double[offs+j][offs+j],(double)(0)) )
                    {
                        rep->terminationtype = -3;
                        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
                        return;
                    }
                    a->ptr.pp_double[offs+j][offs+j] = (double)1/a->ptr.pp_double[offs+j][offs+j];
                    ajj = -a->ptr.pp_double[offs+j][offs+j];
                }
                else
                {
                    ajj = (double)(-1);
                }
                if( j<n-1 )
                {
                    
                    /*
                     * Compute elements j+1:n of j-th column.
                     */
                    ae_v_move(&tmp->ptr.p_double[offs+j+1], 1, &a->ptr.pp_double[offs+j+1][offs+j], a->stride, ae_v_len(offs+j+1,offs+n-1));
                    for(i=j+1; i<=n-1; i++)
                    {
                        if( i>j+1 )
                        {
                            v = ae_v_dotproduct(&a->ptr.pp_double[offs+i][offs+j+1], 1, &tmp->ptr.p_double[offs+j+1], 1, ae_v_len(offs+j+1,offs+i-1));
                        }
                        else
                        {
                            v = (double)(0);
                        }
                        if( !isunit )
                        {
                            a->ptr.pp_double[offs+i][offs+j] = v+a->ptr.pp_double[offs+i][offs+i]*tmp->ptr.p_double[offs+i];
                        }
                        else
                        {
                            a->ptr.pp_double[offs+i][offs+j] = v+tmp->ptr.p_double[offs+i];
                        }
                    }
                    ae_v_muld(&a->ptr.pp_double[offs+j+1][offs+j], a->stride, ae_v_len(offs+j+1,offs+n-1), ajj);
                }
            }
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive case
     */
    tiledsplit(n, tscur, &n1, &n2, _state);
    mn = imin2(n1, n2, _state);
    touchint(&mn, _state);
    ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(rmul3((double)(mn), (double)(mn), (double)(mn), _state)*((double)1/(double)3),spawnlevel(_state)), _state);
    if( n2>0 )
    {
        if( isupper )
        {
            for(i=0; i<=n1-1; i++)
            {
                ae_v_muld(&a->ptr.pp_double[offs+i][offs+n1], 1, ae_v_len(offs+n1,offs+n-1), -1.0);
            }
            rmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, isupper, isunit, 0, a, offs, offs+n1, _state);
            _spawn_matinv_rmatrixtrinverserec(a, offs+n1, n2, isupper, isunit, tmp, rep, _child_tasks, _smp_enabled, _state);
            rmatrixlefttrsm(n1, n2, a, offs, offs, isupper, isunit, 0, a, offs, offs+n1, _state);
        }
        else
        {
            for(i=0; i<=n2-1; i++)
            {
                ae_v_muld(&a->ptr.pp_double[offs+n1+i][offs], 1, ae_v_len(offs,offs+n1-1), -1.0);
            }
            rmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, isupper, isunit, 0, a, offs+n1, offs, _state);
            _spawn_matinv_rmatrixtrinverserec(a, offs+n1, n2, isupper, isunit, tmp, rep, _child_tasks, _smp_enabled, _state);
            rmatrixrighttrsm(n2, n1, a, offs, offs, isupper, isunit, 0, a, offs+n1, offs, _state);
        }
    }
    matinv_rmatrixtrinverserec(a, offs, n1, isupper, isunit, tmp, rep, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_matinv_rmatrixtrinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    ae_bool isunit,
    /* Real    */ ae_vector* tmp,
    matinvreport* rep,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        matinv_rmatrixtrinverserec(a,offs,n,isupper,isunit,tmp,rep, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_matinv_rmatrixtrinverserec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = a;
        _task->data.parameters[1].value.ival = offs;
        _task->data.parameters[2].value.ival = n;
        _task->data.parameters[3].value.bval = isupper;
        _task->data.parameters[4].value.bval = isunit;
        _task->data.parameters[5].value.val = tmp;
        _task->data.parameters[6].value.val = rep;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_matinv_rmatrixtrinverserec(ae_task_data *_data, ae_state *_state)
{
    ae_matrix* a;
    ae_int_t offs;
    ae_int_t n;
    ae_bool isupper;
    ae_bool isunit;
    ae_vector* tmp;
    matinvreport* rep;
    a = (ae_matrix*)_data->parameters[0].value.val;
    offs = _data->parameters[1].value.ival;
    n = _data->parameters[2].value.ival;
    isupper = _data->parameters[3].value.bval;
    isunit = _data->parameters[4].value.bval;
    tmp = (ae_vector*)_data->parameters[5].value.val;
    rep = (matinvreport*)_data->parameters[6].value.val;
   ae_state_set_flags(_state, _data->flags);
   matinv_rmatrixtrinverserec(a,offs,n,isupper,isunit,tmp,rep, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_matinv_rmatrixtrinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    ae_bool isunit,
    /* Real    */ ae_vector* tmp,
    matinvreport* rep,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_matinv_rmatrixtrinverserec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = a;
    _task->data.parameters[1].value.ival = offs;
    _task->data.parameters[2].value.ival = n;
    _task->data.parameters[3].value.bval = isupper;
    _task->data.parameters[4].value.bval = isunit;
    _task->data.parameters[5].value.val = tmp;
    _task->data.parameters[6].value.val = rep;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


/*************************************************************************
Triangular matrix inversion, recursive subroutine.

Rep.TerminationType is modified on failure, left unchanged on success.

  -- ALGLIB --
     05.02.2010, Bochkanov Sergey.
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992.
*************************************************************************/
static void matinv_cmatrixtrinverserec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     ae_bool isunit,
     /* Complex */ ae_vector* tmp,
     matinvreport* rep,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t i;
    ae_int_t j;
    ae_complex v;
    ae_complex ajj;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;
    ae_int_t mn;


    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( n<=tsb )
    {
        tscur = tsa;
    }
    
    /*
     * Try to activate parallelism
     */
    if( n>=2*tsb&&ae_fp_greater_eq(rmul3((double)(n), (double)(n), (double)(n), _state)*((double)4/(double)3),smpactivationlevel(_state)) )
    {
        if( _trypexec_matinv_cmatrixtrinverserec(a,offs,n,isupper,isunit,tmp,rep, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Base case
     */
    if( n<=tsa )
    {
        if( isupper )
        {
            
            /*
             * Compute inverse of upper triangular matrix.
             */
            for(j=0; j<=n-1; j++)
            {
                if( !isunit )
                {
                    if( ae_c_eq_d(a->ptr.pp_complex[offs+j][offs+j],(double)(0)) )
                    {
                        rep->terminationtype = -3;
                        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
                        return;
                    }
                    a->ptr.pp_complex[offs+j][offs+j] = ae_c_d_div((double)(1),a->ptr.pp_complex[offs+j][offs+j]);
                    ajj = ae_c_neg(a->ptr.pp_complex[offs+j][offs+j]);
                }
                else
                {
                    ajj = ae_complex_from_i(-1);
                }
                
                /*
                 * Compute elements 1:j-1 of j-th column.
                 */
                if( j>0 )
                {
                    ae_v_cmove(&tmp->ptr.p_complex[offs+0], 1, &a->ptr.pp_complex[offs+0][offs+j], a->stride, "N", ae_v_len(offs+0,offs+j-1));
                    for(i=0; i<=j-1; i++)
                    {
                        if( i<j-1 )
                        {
                            v = ae_v_cdotproduct(&a->ptr.pp_complex[offs+i][offs+i+1], 1, "N", &tmp->ptr.p_complex[offs+i+1], 1, "N", ae_v_len(offs+i+1,offs+j-1));
                        }
                        else
                        {
                            v = ae_complex_from_i(0);
                        }
                        if( !isunit )
                        {
                            a->ptr.pp_complex[offs+i][offs+j] = ae_c_add(v,ae_c_mul(a->ptr.pp_complex[offs+i][offs+i],tmp->ptr.p_complex[offs+i]));
                        }
                        else
                        {
                            a->ptr.pp_complex[offs+i][offs+j] = ae_c_add(v,tmp->ptr.p_complex[offs+i]);
                        }
                    }
                    ae_v_cmulc(&a->ptr.pp_complex[offs+0][offs+j], a->stride, ae_v_len(offs+0,offs+j-1), ajj);
                }
            }
        }
        else
        {
            
            /*
             * Compute inverse of lower triangular matrix.
             */
            for(j=n-1; j>=0; j--)
            {
                if( !isunit )
                {
                    if( ae_c_eq_d(a->ptr.pp_complex[offs+j][offs+j],(double)(0)) )
                    {
                        rep->terminationtype = -3;
                        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
                        return;
                    }
                    a->ptr.pp_complex[offs+j][offs+j] = ae_c_d_div((double)(1),a->ptr.pp_complex[offs+j][offs+j]);
                    ajj = ae_c_neg(a->ptr.pp_complex[offs+j][offs+j]);
                }
                else
                {
                    ajj = ae_complex_from_i(-1);
                }
                if( j<n-1 )
                {
                    
                    /*
                     * Compute elements j+1:n of j-th column.
                     */
                    ae_v_cmove(&tmp->ptr.p_complex[offs+j+1], 1, &a->ptr.pp_complex[offs+j+1][offs+j], a->stride, "N", ae_v_len(offs+j+1,offs+n-1));
                    for(i=j+1; i<=n-1; i++)
                    {
                        if( i>j+1 )
                        {
                            v = ae_v_cdotproduct(&a->ptr.pp_complex[offs+i][offs+j+1], 1, "N", &tmp->ptr.p_complex[offs+j+1], 1, "N", ae_v_len(offs+j+1,offs+i-1));
                        }
                        else
                        {
                            v = ae_complex_from_i(0);
                        }
                        if( !isunit )
                        {
                            a->ptr.pp_complex[offs+i][offs+j] = ae_c_add(v,ae_c_mul(a->ptr.pp_complex[offs+i][offs+i],tmp->ptr.p_complex[offs+i]));
                        }
                        else
                        {
                            a->ptr.pp_complex[offs+i][offs+j] = ae_c_add(v,tmp->ptr.p_complex[offs+i]);
                        }
                    }
                    ae_v_cmulc(&a->ptr.pp_complex[offs+j+1][offs+j], a->stride, ae_v_len(offs+j+1,offs+n-1), ajj);
                }
            }
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive case
     */
    tiledsplit(n, tscur, &n1, &n2, _state);
    mn = imin2(n1, n2, _state);
    touchint(&mn, _state);
    ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(rmul3((double)(mn), (double)(mn), (double)(mn), _state)*((double)4/(double)3),spawnlevel(_state)), _state);
    if( n2>0 )
    {
        if( isupper )
        {
            for(i=0; i<=n1-1; i++)
            {
                ae_v_cmuld(&a->ptr.pp_complex[offs+i][offs+n1], 1, ae_v_len(offs+n1,offs+n-1), -1.0);
            }
            cmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, isupper, isunit, 0, a, offs, offs+n1, _state);
            _spawn_matinv_cmatrixtrinverserec(a, offs+n1, n2, isupper, isunit, tmp, rep, _child_tasks, _smp_enabled, _state);
            cmatrixlefttrsm(n1, n2, a, offs, offs, isupper, isunit, 0, a, offs, offs+n1, _state);
        }
        else
        {
            for(i=0; i<=n2-1; i++)
            {
                ae_v_cmuld(&a->ptr.pp_complex[offs+n1+i][offs], 1, ae_v_len(offs,offs+n1-1), -1.0);
            }
            cmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, isupper, isunit, 0, a, offs+n1, offs, _state);
            _spawn_matinv_cmatrixtrinverserec(a, offs+n1, n2, isupper, isunit, tmp, rep, _child_tasks, _smp_enabled, _state);
            cmatrixrighttrsm(n2, n1, a, offs, offs, isupper, isunit, 0, a, offs+n1, offs, _state);
        }
    }
    matinv_cmatrixtrinverserec(a, offs, n1, isupper, isunit, tmp, rep, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_matinv_cmatrixtrinverserec(/* Complex */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    ae_bool isunit,
    /* Complex */ ae_vector* tmp,
    matinvreport* rep,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        matinv_cmatrixtrinverserec(a,offs,n,isupper,isunit,tmp,rep, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_matinv_cmatrixtrinverserec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = a;
        _task->data.parameters[1].value.ival = offs;
        _task->data.parameters[2].value.ival = n;
        _task->data.parameters[3].value.bval = isupper;
        _task->data.parameters[4].value.bval = isunit;
        _task->data.parameters[5].value.val = tmp;
        _task->data.parameters[6].value.val = rep;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_matinv_cmatrixtrinverserec(ae_task_data *_data, ae_state *_state)
{
    ae_matrix* a;
    ae_int_t offs;
    ae_int_t n;
    ae_bool isupper;
    ae_bool isunit;
    ae_vector* tmp;
    matinvreport* rep;
    a = (ae_matrix*)_data->parameters[0].value.val;
    offs = _data->parameters[1].value.ival;
    n = _data->parameters[2].value.ival;
    isupper = _data->parameters[3].value.bval;
    isunit = _data->parameters[4].value.bval;
    tmp = (ae_vector*)_data->parameters[5].value.val;
    rep = (matinvreport*)_data->parameters[6].value.val;
   ae_state_set_flags(_state, _data->flags);
   matinv_cmatrixtrinverserec(a,offs,n,isupper,isunit,tmp,rep, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_matinv_cmatrixtrinverserec(/* Complex */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    ae_bool isupper,
    ae_bool isunit,
    /* Complex */ ae_vector* tmp,
    matinvreport* rep,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_matinv_cmatrixtrinverserec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = a;
    _task->data.parameters[1].value.ival = offs;
    _task->data.parameters[2].value.ival = n;
    _task->data.parameters[3].value.bval = isupper;
    _task->data.parameters[4].value.bval = isunit;
    _task->data.parameters[5].value.val = tmp;
    _task->data.parameters[6].value.val = rep;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


static void matinv_rmatrixluinverserec(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     /* Real    */ ae_vector* work,
     matinvreport* rep,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;
    ae_int_t mn;


    ae_assert(n>=1, "MATINV: integrity check 2553 failed", _state);
    tsa = matrixtilesizea(_state);
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( n<=tsb )
    {
        tscur = tsa;
    }
    
    /*
     * Try parallelism
     */
    if( n>=2*tsb&&ae_fp_greater_eq((double)8/(double)6*rmul3((double)(n), (double)(n), (double)(n), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_matinv_rmatrixluinverserec(a,offs,n,work,rep, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Base case
     */
    if( n<=tsa )
    {
        
        /*
         * Form inv(U)
         */
        matinv_rmatrixtrinverserec(a, offs, n, ae_true, ae_false, work, rep, _state);
        
        /*
         * Solve the equation inv(A)*L = inv(U) for inv(A).
         */
        for(j=n-1; j>=0; j--)
        {
            
            /*
             * Copy current column of L to WORK and replace with zeros.
             */
            for(i=j+1; i<=n-1; i++)
            {
                work->ptr.p_double[i] = a->ptr.pp_double[offs+i][offs+j];
                a->ptr.pp_double[offs+i][offs+j] = (double)(0);
            }
            
            /*
             * Compute current column of inv(A).
             */
            if( j<n-1 )
            {
                for(i=0; i<=n-1; i++)
                {
                    v = ae_v_dotproduct(&a->ptr.pp_double[offs+i][offs+j+1], 1, &work->ptr.p_double[j+1], 1, ae_v_len(offs+j+1,offs+n-1));
                    a->ptr.pp_double[offs+i][offs+j] = a->ptr.pp_double[offs+i][offs+j]-v;
                }
            }
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive code:
     *
     *         ( L1      )   ( U1  U12 )
     * A    =  (         ) * (         )
     *         ( L12  L2 )   (     U2  )
     *
     *         ( W   X )
     * A^-1 =  (       )
     *         ( Y   Z )
     *
     * In-place calculation can be done as follows:
     * * X := inv(U1)*U12*inv(U2)
     * * Y := inv(L2)*L12*inv(L1)
     * * W := inv(L1*U1)+X*Y
     * * X := -X*inv(L2)
     * * Y := -inv(U2)*Y
     * * Z := inv(L2*U2)
     *
     * Reordering w.r.t. interdependencies gives us:
     *
     * * X := inv(U1)*U12      \ suitable for parallel execution
     * * Y := L12*inv(L1)      / 
     *
     * * X := X*inv(U2)        \
     * * Y := inv(L2)*Y        | suitable for parallel execution
     * * W := inv(L1*U1)       / 
     *
     * * W := W+X*Y
     *
     * * X := -X*inv(L2)       \ suitable for parallel execution
     * * Y := -inv(U2)*Y       /
     *
     * * Z := inv(L2*U2)
     */
    tiledsplit(n, tscur, &n1, &n2, _state);
    mn = imin2(n1, n2, _state);
    touchint(&mn, _state);
    ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq(rmul3((double)(mn), (double)(mn), (double)(mn), _state),spawnlevel(_state)), _state);
    ae_assert(n2>0, "LUInverseRec: internal error!", _state);
    
    /*
     * X := inv(U1)*U12
     * Y := L12*inv(L1)
     */
    _spawn_rmatrixlefttrsm(n1, n2, a, offs, offs, ae_true, ae_false, 0, a, offs, offs+n1, _child_tasks, _smp_enabled, _state);
    rmatrixrighttrsm(n2, n1, a, offs, offs, ae_false, ae_true, 0, a, offs+n1, offs, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    
    /*
     * X := X*inv(U2)
     * Y := inv(L2)*Y
     * W := inv(L1*U1)
     */
    _spawn_rmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, ae_true, ae_false, 0, a, offs, offs+n1, _child_tasks, _smp_enabled, _state);
    _spawn_rmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, ae_false, ae_true, 0, a, offs+n1, offs, _child_tasks, _smp_enabled, _state);
    matinv_rmatrixluinverserec(a, offs, n1, work, rep, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    if( rep->terminationtype<=0 )
    {
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * W := W+X*Y
     */
    rmatrixgemm(n1, n1, n2, 1.0, a, offs, offs+n1, 0, a, offs+n1, offs, 0, 1.0, a, offs, offs, _state);
    
    /*
     * X := -X*inv(L2)
     * Y := -inv(U2)*Y
     */
    _spawn_rmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, ae_false, ae_true, 0, a, offs, offs+n1, _child_tasks, _smp_enabled, _state);
    rmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, ae_true, ae_false, 0, a, offs+n1, offs, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    for(i=0; i<=n1-1; i++)
    {
        ae_v_muld(&a->ptr.pp_double[offs+i][offs+n1], 1, ae_v_len(offs+n1,offs+n-1), -1.0);
    }
    for(i=0; i<=n2-1; i++)
    {
        ae_v_muld(&a->ptr.pp_double[offs+n1+i][offs], 1, ae_v_len(offs,offs+n1-1), -1.0);
    }
    
    /*
     * Z := inv(L2*U2)
     */
    matinv_rmatrixluinverserec(a, offs+n1, n2, work, rep, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_matinv_rmatrixluinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    /* Real    */ ae_vector* work,
    matinvreport* rep,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        matinv_rmatrixluinverserec(a,offs,n,work,rep, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_matinv_rmatrixluinverserec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = a;
        _task->data.parameters[1].value.ival = offs;
        _task->data.parameters[2].value.ival = n;
        _task->data.parameters[3].value.val = work;
        _task->data.parameters[4].value.val = rep;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_matinv_rmatrixluinverserec(ae_task_data *_data, ae_state *_state)
{
    ae_matrix* a;
    ae_int_t offs;
    ae_int_t n;
    ae_vector* work;
    matinvreport* rep;
    a = (ae_matrix*)_data->parameters[0].value.val;
    offs = _data->parameters[1].value.ival;
    n = _data->parameters[2].value.ival;
    work = (ae_vector*)_data->parameters[3].value.val;
    rep = (matinvreport*)_data->parameters[4].value.val;
   ae_state_set_flags(_state, _data->flags);
   matinv_rmatrixluinverserec(a,offs,n,work,rep, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_matinv_rmatrixluinverserec(/* Real    */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    /* Real    */ ae_vector* work,
    matinvreport* rep,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_matinv_rmatrixluinverserec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = a;
    _task->data.parameters[1].value.ival = offs;
    _task->data.parameters[2].value.ival = n;
    _task->data.parameters[3].value.val = work;
    _task->data.parameters[4].value.val = rep;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


static void matinv_cmatrixluinverserec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     /* Complex */ ae_vector* work,
     matinvreport* rep,
     ae_state *_state)
{
    ae_task_group *_child_tasks = NULL;
    ae_bool _smp_enabled = ae_false;
    ae_int_t i;
    ae_int_t j;
    ae_complex v;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t mn;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( n<=tsb )
    {
        tscur = tsa;
    }
    
    /*
     * Try parallelism
     */
    if( n>=2*tsb&&ae_fp_greater_eq((double)32/(double)6*rmul3((double)(n), (double)(n), (double)(n), _state),smpactivationlevel(_state)) )
    {
        if( _trypexec_matinv_cmatrixluinverserec(a,offs,n,work,rep, _state) )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
    }
    
    /*
     * Base case
     */
    if( n<=tsa )
    {
        
        /*
         * Form inv(U)
         */
        matinv_cmatrixtrinverserec(a, offs, n, ae_true, ae_false, work, rep, _state);
        if( rep->terminationtype<=0 )
        {
            ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
            return;
        }
        
        /*
         * Solve the equation inv(A)*L = inv(U) for inv(A).
         */
        for(j=n-1; j>=0; j--)
        {
            
            /*
             * Copy current column of L to WORK and replace with zeros.
             */
            for(i=j+1; i<=n-1; i++)
            {
                work->ptr.p_complex[i] = a->ptr.pp_complex[offs+i][offs+j];
                a->ptr.pp_complex[offs+i][offs+j] = ae_complex_from_i(0);
            }
            
            /*
             * Compute current column of inv(A).
             */
            if( j<n-1 )
            {
                for(i=0; i<=n-1; i++)
                {
                    v = ae_v_cdotproduct(&a->ptr.pp_complex[offs+i][offs+j+1], 1, "N", &work->ptr.p_complex[j+1], 1, "N", ae_v_len(offs+j+1,offs+n-1));
                    a->ptr.pp_complex[offs+i][offs+j] = ae_c_sub(a->ptr.pp_complex[offs+i][offs+j],v);
                }
            }
        }
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * Recursive code:
     *
     *         ( L1      )   ( U1  U12 )
     * A    =  (         ) * (         )
     *         ( L12  L2 )   (     U2  )
     *
     *         ( W   X )
     * A^-1 =  (       )
     *         ( Y   Z )
     *
     * In-place calculation can be done as follows:
     * * X := inv(U1)*U12*inv(U2)
     * * Y := inv(L2)*L12*inv(L1)
     * * W := inv(L1*U1)+X*Y
     * * X := -X*inv(L2)
     * * Y := -inv(U2)*Y
     * * Z := inv(L2*U2)
     *
     * Reordering w.r.t. interdependencies gives us:
     *
     * * X := inv(U1)*U12      \ suitable for parallel execution
     * * Y := L12*inv(L1)      / 
     *
     * * X := X*inv(U2)        \
     * * Y := inv(L2)*Y        | suitable for parallel execution
     * * W := inv(L1*U1)       / 
     *
     * * W := W+X*Y
     *
     * * X := -X*inv(L2)       \ suitable for parallel execution
     * * Y := -inv(U2)*Y       /
     *
     * * Z := inv(L2*U2)
     */
    tiledsplit(n, tscur, &n1, &n2, _state);
    mn = imin2(n1, n2, _state);
    touchint(&mn, _state);
    ae_set_smp_support(&_child_tasks, &_smp_enabled, ae_fp_greater_eq((double)4*rmul3((double)(mn), (double)(mn), (double)(mn), _state),spawnlevel(_state)), _state);
    ae_assert(n2>0, "LUInverseRec: internal error!", _state);
    
    /*
     * X := inv(U1)*U12
     * Y := L12*inv(L1)
     */
    _spawn_cmatrixlefttrsm(n1, n2, a, offs, offs, ae_true, ae_false, 0, a, offs, offs+n1, _child_tasks, _smp_enabled, _state);
    cmatrixrighttrsm(n2, n1, a, offs, offs, ae_false, ae_true, 0, a, offs+n1, offs, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    
    /*
     * X := X*inv(U2)
     * Y := inv(L2)*Y
     * W := inv(L1*U1)
     */
    _spawn_cmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, ae_true, ae_false, 0, a, offs, offs+n1, _child_tasks, _smp_enabled, _state);
    _spawn_cmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, ae_false, ae_true, 0, a, offs+n1, offs, _child_tasks, _smp_enabled, _state);
    matinv_cmatrixluinverserec(a, offs, n1, work, rep, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    if( rep->terminationtype<=0 )
    {
        ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
        return;
    }
    
    /*
     * W := W+X*Y
     */
    cmatrixgemm(n1, n1, n2, ae_complex_from_d(1.0), a, offs, offs+n1, 0, a, offs+n1, offs, 0, ae_complex_from_d(1.0), a, offs, offs, _state);
    
    /*
     * X := -X*inv(L2)
     * Y := -inv(U2)*Y
     */
    _spawn_cmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, ae_false, ae_true, 0, a, offs, offs+n1, _child_tasks, _smp_enabled, _state);
    cmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, ae_true, ae_false, 0, a, offs+n1, offs, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_false, _state);
    for(i=0; i<=n1-1; i++)
    {
        ae_v_cmuld(&a->ptr.pp_complex[offs+i][offs+n1], 1, ae_v_len(offs+n1,offs+n-1), -1.0);
    }
    for(i=0; i<=n2-1; i++)
    {
        ae_v_cmuld(&a->ptr.pp_complex[offs+n1+i][offs], 1, ae_v_len(offs,offs+n1-1), -1.0);
    }
    
    /*
     * Z := inv(L2*U2)
     */
    matinv_cmatrixluinverserec(a, offs+n1, n2, work, rep, _state);
    ae_sync(_child_tasks, _smp_enabled, ae_true, _state);
}


/*************************************************************************
When called in the multithreaded mode (non-NULL _group), spawns child task.
Executed serially when called with NULL _group.
*************************************************************************/
void _spawn_matinv_cmatrixluinverserec(/* Complex */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    /* Complex */ ae_vector* work,
    matinvreport* rep,
    ae_task_group *_group, ae_bool smp_enabled, ae_state *_state)
{
    if( _group==NULL || !smp_enabled)
    {
        matinv_cmatrixluinverserec(a,offs,n,work,rep, _state);
        return;
    }
#if defined(_ALGLIB_HAS_WORKSTEALING)
    {
        ae_task_info *_task;
        _task = ae_create_task(_group, _state);
        _task->data.func = _task_matinv_cmatrixluinverserec;
        _task->data.flags = _state->flags;
        _task->data.parameters[0].value.val = a;
        _task->data.parameters[1].value.ival = offs;
        _task->data.parameters[2].value.ival = n;
        _task->data.parameters[3].value.val = work;
        _task->data.parameters[4].value.val = rep;
        ae_push_task(_task, _state, ae_false);
    }
#else
    abort(); /* critical integrity failure */
#endif
}


/*************************************************************************
This method is called when worker thread starts working on a non-root task.
*************************************************************************/
#if defined(_ALGLIB_HAS_WORKSTEALING)
void _task_matinv_cmatrixluinverserec(ae_task_data *_data, ae_state *_state)
{
    ae_matrix* a;
    ae_int_t offs;
    ae_int_t n;
    ae_vector* work;
    matinvreport* rep;
    a = (ae_matrix*)_data->parameters[0].value.val;
    offs = _data->parameters[1].value.ival;
    n = _data->parameters[2].value.ival;
    work = (ae_vector*)_data->parameters[3].value.val;
    rep = (matinvreport*)_data->parameters[4].value.val;
   ae_state_set_flags(_state, _data->flags);
   matinv_cmatrixluinverserec(a,offs,n,work,rep, _state);
}
#endif


/*************************************************************************
Inserts task as root into worker queue if called from non-worker thread, waits for completion and returns true.
Returns false if ALGLIB is configured for serial execution or we are already in the worker context.
*************************************************************************/
ae_bool _trypexec_matinv_cmatrixluinverserec(/* Complex */ ae_matrix* a,
    ae_int_t offs,
    ae_int_t n,
    /* Complex */ ae_vector* work,
    matinvreport* rep,
    ae_state *_state)
{
#if defined(_ALGLIB_HAS_WORKSTEALING)
    ae_task_info *_task;
    const char *_e;
    if( !ae_can_pexec(_state) )
        return ae_false;
    _task = ae_create_task(NULL, _state);
    _task->data.func = _task_matinv_cmatrixluinverserec;
    _task->data.flags = _state->flags;
    _task->data.parameters[0].value.val = a;
    _task->data.parameters[1].value.ival = offs;
    _task->data.parameters[2].value.ival = n;
    _task->data.parameters[3].value.val = work;
    _task->data.parameters[4].value.val = rep;
    ae_push_root_task(_task);
    ae_wait_for_event(&_task->done_event);
    _e = _task->exception;
    ae_dispose_task(_task);
    ae_assert(_e==NULL, _e, _state);
    return ae_true;
#else
    return ae_false;
#endif
}


/*************************************************************************
Recursive subroutine for HPD inversion.

  -- ALGLIB routine --
     10.02.2010
     Bochkanov Sergey
*************************************************************************/
static void matinv_hpdmatrixcholeskyinverserec(/* Complex */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* tmp,
     matinvreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_complex v;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t tsa;
    ae_int_t tsb;
    ae_int_t tscur;


    if( n<1 )
    {
        return;
    }
    tsa = matrixtilesizea(_state)/2;
    tsb = matrixtilesizeb(_state);
    tscur = tsb;
    if( n<=tsb )
    {
        tscur = tsa;
    }
    
    /*
     * Base case
     */
    if( n<=tsa )
    {
        matinv_cmatrixtrinverserec(a, offs, n, isupper, ae_false, tmp, rep, _state);
        ae_assert(rep->terminationtype>0, "HPDMatrixCholeskyInverseRec: integrity check failed", _state);
        if( isupper )
        {
            
            /*
             * Compute the product U * U'.
             * NOTE: we never assume that diagonal of U is real
             */
            for(i=0; i<=n-1; i++)
            {
                if( i==0 )
                {
                    
                    /*
                     * 1x1 matrix
                     */
                    a->ptr.pp_complex[offs+i][offs+i] = ae_complex_from_d(ae_sqr(a->ptr.pp_complex[offs+i][offs+i].x, _state)+ae_sqr(a->ptr.pp_complex[offs+i][offs+i].y, _state));
                }
                else
                {
                    
                    /*
                     * (I+1)x(I+1) matrix,
                     *
                     * ( A11  A12 )   ( A11^H        )   ( A11*A11^H+A12*A12^H  A12*A22^H )
                     * (          ) * (              ) = (                                )
                     * (      A22 )   ( A12^H  A22^H )   ( A22*A12^H            A22*A22^H )
                     *
                     * A11 is IxI, A22 is 1x1.
                     */
                    ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs][offs+i], a->stride, "Conj", ae_v_len(0,i-1));
                    for(j=0; j<=i-1; j++)
                    {
                        v = a->ptr.pp_complex[offs+j][offs+i];
                        ae_v_caddc(&a->ptr.pp_complex[offs+j][offs+j], 1, &tmp->ptr.p_complex[j], 1, "N", ae_v_len(offs+j,offs+i-1), v);
                    }
                    v = ae_c_conj(a->ptr.pp_complex[offs+i][offs+i], _state);
                    ae_v_cmulc(&a->ptr.pp_complex[offs][offs+i], a->stride, ae_v_len(offs,offs+i-1), v);
                    a->ptr.pp_complex[offs+i][offs+i] = ae_complex_from_d(ae_sqr(a->ptr.pp_complex[offs+i][offs+i].x, _state)+ae_sqr(a->ptr.pp_complex[offs+i][offs+i].y, _state));
                }
            }
        }
        else
        {
            
            /*
             * Compute the product L' * L
             * NOTE: we never assume that diagonal of L is real
             */
            for(i=0; i<=n-1; i++)
            {
                if( i==0 )
                {
                    
                    /*
                     * 1x1 matrix
                     */
                    a->ptr.pp_complex[offs+i][offs+i] = ae_complex_from_d(ae_sqr(a->ptr.pp_complex[offs+i][offs+i].x, _state)+ae_sqr(a->ptr.pp_complex[offs+i][offs+i].y, _state));
                }
                else
                {
                    
                    /*
                     * (I+1)x(I+1) matrix,
                     *
                     * ( A11^H  A21^H )   ( A11      )   ( A11^H*A11+A21^H*A21  A21^H*A22 )
                     * (              ) * (          ) = (                                )
                     * (        A22^H )   ( A21  A22 )   ( A22^H*A21            A22^H*A22 )
                     *
                     * A11 is IxI, A22 is 1x1.
                     */
                    ae_v_cmove(&tmp->ptr.p_complex[0], 1, &a->ptr.pp_complex[offs+i][offs], 1, "N", ae_v_len(0,i-1));
                    for(j=0; j<=i-1; j++)
                    {
                        v = ae_c_conj(a->ptr.pp_complex[offs+i][offs+j], _state);
                        ae_v_caddc(&a->ptr.pp_complex[offs+j][offs], 1, &tmp->ptr.p_complex[0], 1, "N", ae_v_len(offs,offs+j), v);
                    }
                    v = ae_c_conj(a->ptr.pp_complex[offs+i][offs+i], _state);
                    ae_v_cmulc(&a->ptr.pp_complex[offs+i][offs], 1, ae_v_len(offs,offs+i-1), v);
                    a->ptr.pp_complex[offs+i][offs+i] = ae_complex_from_d(ae_sqr(a->ptr.pp_complex[offs+i][offs+i].x, _state)+ae_sqr(a->ptr.pp_complex[offs+i][offs+i].y, _state));
                }
            }
        }
        return;
    }
    
    /*
     * Recursive code: triangular factor inversion merged with
     * UU' or L'L multiplication
     */
    tiledsplit(n, tscur, &n1, &n2, _state);
    
    /*
     * form off-diagonal block of trangular inverse
     */
    if( isupper )
    {
        for(i=0; i<=n1-1; i++)
        {
            ae_v_cmuld(&a->ptr.pp_complex[offs+i][offs+n1], 1, ae_v_len(offs+n1,offs+n-1), -1.0);
        }
        cmatrixlefttrsm(n1, n2, a, offs, offs, isupper, ae_false, 0, a, offs, offs+n1, _state);
        cmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, isupper, ae_false, 0, a, offs, offs+n1, _state);
    }
    else
    {
        for(i=0; i<=n2-1; i++)
        {
            ae_v_cmuld(&a->ptr.pp_complex[offs+n1+i][offs], 1, ae_v_len(offs,offs+n1-1), -1.0);
        }
        cmatrixrighttrsm(n2, n1, a, offs, offs, isupper, ae_false, 0, a, offs+n1, offs, _state);
        cmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, isupper, ae_false, 0, a, offs+n1, offs, _state);
    }
    
    /*
     * invert first diagonal block
     */
    matinv_hpdmatrixcholeskyinverserec(a, offs, n1, isupper, tmp, rep, _state);
    
    /*
     * update first diagonal block with off-diagonal block,
     * update off-diagonal block
     */
    if( isupper )
    {
        cmatrixherk(n1, n2, 1.0, a, offs, offs+n1, 0, 1.0, a, offs, offs, isupper, _state);
        cmatrixrighttrsm(n1, n2, a, offs+n1, offs+n1, isupper, ae_false, 2, a, offs, offs+n1, _state);
    }
    else
    {
        cmatrixherk(n1, n2, 1.0, a, offs+n1, offs, 2, 1.0, a, offs, offs, isupper, _state);
        cmatrixlefttrsm(n2, n1, a, offs+n1, offs+n1, isupper, ae_false, 2, a, offs+n1, offs, _state);
    }
    
    /*
     * invert second diagonal block
     */
    matinv_hpdmatrixcholeskyinverserec(a, offs+n1, n2, isupper, tmp, rep, _state);
}


void _matinvreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    matinvreport *p = (matinvreport*)_p;
    ae_touch_ptr((void*)p);
}


void _matinvreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    matinvreport       *dst = (matinvreport*)_dst;
    const matinvreport *src = (const matinvreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->r1 = src->r1;
    dst->rinf = src->rinf;
}


void _matinvreport_clear(void* _p)
{
    matinvreport *p = (matinvreport*)_p;
    ae_touch_ptr((void*)p);
}


void _matinvreport_destroy(void* _p)
{
    matinvreport *p = (matinvreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
