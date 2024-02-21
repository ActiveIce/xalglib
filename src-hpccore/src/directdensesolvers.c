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
#include "directdensesolvers.h"


/*$ Declarations $*/
static void directdensesolvers_rmatrixlusolveinternal(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_bool havea,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state);
static void directdensesolvers_spdmatrixcholeskysolveinternal(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_matrix* a,
     ae_bool havea,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state);
static void directdensesolvers_cmatrixlusolveinternal(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_bool havea,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state);
static void directdensesolvers_hpdmatrixcholeskysolveinternal(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_matrix* a,
     ae_bool havea,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state);
static ae_int_t directdensesolvers_densesolverrfsmax(ae_int_t n,
     double r1,
     double rinf,
     ae_state *_state);
static ae_int_t directdensesolvers_densesolverrfsmaxv2(ae_int_t n,
     double r2,
     ae_state *_state);
static void directdensesolvers_rbasiclusolve(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_vector* xb,
     ae_state *_state);
static void directdensesolvers_spdbasiccholeskysolve(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* xb,
     ae_state *_state);
static void directdensesolvers_cbasiclusolve(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_vector* xb,
     ae_state *_state);
static void directdensesolvers_hpdbasiccholeskysolve(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* xb,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Dense solver for A*x=b with N*N real matrix A and N*1 real vectorx  x  and
b. This is "slow-but-feature rich" version of the  linear  solver.  Faster
version is RMatrixSolveFast() function.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations. It is also very significant on small matrices.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, RMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, the following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolve(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixSolve: N<=0", _state);
    ae_assert(a->rows>=n, "RMatrixSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "RMatrixSolve: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixSolve: length(B)<N", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixSolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    rcopyvc(n, b, &bm, 0, _state);
    rmatrixsolvem(a, n, &bm, 1, ae_true, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    rcopycv(n, &xm, 0, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix, x  and  b  are  vectors.  This is a "fast" version of  linear
solver which does NOT provide  any  additional  functions  like  condition
number estimation or iterative refinement.

Algorithm features:
* efficient algorithm O(N^3) complexity
* no performance overhead from additional functionality

If you need condition number estimation or iterative refinement, use  more
feature-rich version - RMatrixSolve().

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros
                
RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

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
     Copyright 16.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixsolvefast(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_int_t j;
    ae_vector p;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&p, 0, sizeof(p));
    ae_matrix_init_copy(&a, _a, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "RMatrixSolveFast: N<=0", _state);
    ae_assert(a.rows>=n, "RMatrixSolveFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "RMatrixSolveFast: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixSolveFast: length(B)<N", _state);
    ae_assert(apservisfinitematrix(&a, n, n, _state), "RMatrixSolveFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixSolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    rmatrixlu(&a, n, n, &p, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(a.ptr.pp_double[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.p_double[j] = 0.0;
            }
            result = ae_false;
            ae_frame_leave(_state);
            return result;
        }
    }
    directdensesolvers_rbasiclusolve(&a, &p, n, b, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices). This is  "slow-but-robust"  version  of  linear
solver with additional functionality  like  condition  number  estimation.
There also exists faster version - RMatrixSolveMFast().

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* optional iterative refinement
* O(N^3+M*N^2) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations. It also very significant on small matrices.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, RMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvem(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     ae_bool rfs,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_matrix emptya;
    ae_vector p;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&da, 0, sizeof(da));
    memset(&emptya, 0, sizeof(emptya));
    memset(&p, 0, sizeof(p));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&da, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "RMatrixSolveM: N<=0", _state);
    ae_assert(m>0, "RMatrixSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "RMatrixSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "RMatrixSolveM: cols(A)<N", _state);
    ae_assert(b->rows>=n, "RMatrixSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixSolveM: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixSolveM: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixSolveM: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * 1. factorize matrix
     * 3. solve
     */
    for(i=0; i<=n-1; i++)
    {
        ae_v_move(&da.ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
    }
    rmatrixlu(&da, n, n, &p, _state);
    if( rfs )
    {
        directdensesolvers_rmatrixlusolveinternal(&da, &p, n, a, ae_true, b, m, x, rep, _state);
    }
    else
    {
        directdensesolvers_rmatrixlusolveinternal(&da, &p, n, &emptya, ae_false, b, m, x, rep, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices). This is "fast" version of linear  solver  which
does NOT offer additional functions like condition  number  estimation  or
iterative refinement.

Algorithm features:
* O(N^3+M*N^2) complexity
* no additional functionality, highest performance

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros
                
RETURNS:
    True for well-conditioned matrix
    False for extremely badly conditioned or exactly singular problem

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixsolvemfast(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    double v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector p;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&p, 0, sizeof(p));
    ae_matrix_init_copy(&a, _a, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "RMatrixSolveMFast: N<=0", _state);
    ae_assert(m>0, "RMatrixSolveMFast: M<=0", _state);
    ae_assert(a.rows>=n, "RMatrixSolveMFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "RMatrixSolveMFast: cols(A)<N", _state);
    ae_assert(b->rows>=n, "RMatrixSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixSolveMFast: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(&a, n, n, _state), "RMatrixSolveMFast: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixSolveMFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    rmatrixlu(&a, n, n, &p, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(a.ptr.pp_double[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=m-1; k++)
                {
                    b->ptr.pp_double[j][k] = 0.0;
                }
            }
            result = ae_false;
            ae_frame_leave(_state);
            return result;
        }
    }
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        if( p.ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = b->ptr.pp_double[i][j];
                b->ptr.pp_double[i][j] = b->ptr.pp_double[p.ptr.p_int[i]][j];
                b->ptr.pp_double[p.ptr.p_int[i]][j] = v;
            }
        }
    }
    rmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_true, 0, b, 0, 0, _state);
    rmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix given by its LU decomposition, x and b are real vectors.  This
is "slow-but-robust" version of the linear LU-based solver. Faster version
is RMatrixLUSolveFast() function.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! RMatrixLUSolveFast() function.

INPUT PARAMETERS
    LUA     -   array[N,N], LU decomposition, RMatrixLU result
    P       -   array[N], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[N], right part

OUTPUT PARAMETERS
    Rep     -   additional report, the following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

    
  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolve(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixLUSolve: N<=0", _state);
    ae_assert(lua->rows>=n, "RMatrixLUSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixLUSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixLUSolve: length(P)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixLUSolve: length(B)<N", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixLUSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixLUSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    rcopyvc(n, b, &bm, 0, _state);
    rmatrixlusolvem(lua, p, n, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    rcopycv(n, &xm, 0, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix given by its LU decomposition, x and b are real vectors.  This
is "fast-without-any-checks" version of the linear LU-based solver. Slower
but more robust version is RMatrixLUSolve() function.

Algorithm features:
* O(N^2) complexity
* fast algorithm without ANY additional checks, just triangular solver

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system


  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixlusolvefast(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>0, "RMatrixLUSolveFast: N<=0", _state);
    ae_assert(lua->rows>=n, "RMatrixLUSolveFast: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixLUSolveFast: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixLUSolveFast: length(P)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixLUSolveFast: length(B)<N", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixLUSolveFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixLUSolveFast: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(lua->ptr.pp_double[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.p_double[j] = 0.0;
            }
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    directdensesolvers_rbasiclusolve(lua, p, n, b, _state);
    return result;
}


/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves  task  with  multiple  right  parts
(where b and x are NxM matrices). This  is  "robust-but-slow"  version  of
LU-based solver which performs additional  checks  for  non-degeneracy  of
inputs (condition number estimation). If you need  best  performance,  use
"fast-without-any-checks" version, RMatrixLUSolveMFast().

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! RMatrixLUSolveMFast() function.
  
INPUT PARAMETERS
    LUA     -   array[N,N], LU decomposition, RMatrixLU result
    P       -   array[N], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolvem(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&emptya, 0, sizeof(emptya));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixLUSolveM: N<=0", _state);
    ae_assert(m>0, "RMatrixLUSolveM: M<=0", _state);
    ae_assert(lua->rows>=n, "RMatrixLUSolveM: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixLUSolveM: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixLUSolveM: length(P)<N", _state);
    ae_assert(b->rows>=n, "RMatrixLUSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixLUSolveM: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixLUSolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixLUSolveM: LUA contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    directdensesolvers_rmatrixlusolveinternal(lua, p, n, &emptya, ae_false, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves  task  with  multiple  right parts,
where b and x are NxM matrices.  This is "fast-without-any-checks" version
of LU-based solver. It does not estimate  condition number  of  a  system,
so it is extremely fast. If you need better detection  of  near-degenerate
cases, use RMatrixLUSolveM() function.

Algorithm features:
* O(M*N^2) complexity
* fast algorithm without ANY additional checks, just triangular solver

INPUT PARAMETERS:
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

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
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixlusolvemfast(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    double v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;


    
    /*
     * Check for exact degeneracy
     */
    ae_assert(n>0, "RMatrixLUSolveMFast: N<=0", _state);
    ae_assert(m>0, "RMatrixLUSolveMFast: M<=0", _state);
    ae_assert(lua->rows>=n, "RMatrixLUSolveMFast: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixLUSolveMFast: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixLUSolveMFast: length(P)<N", _state);
    ae_assert(b->rows>=n, "RMatrixLUSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixLUSolveMFast: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixLUSolveMFast: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixLUSolveMFast: LUA contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(lua->ptr.pp_double[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=m-1; k++)
                {
                    b->ptr.pp_double[j][k] = 0.0;
                }
            }
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = b->ptr.pp_double[i][j];
                b->ptr.pp_double[i][j] = b->ptr.pp_double[p->ptr.p_int[i]][j];
                b->ptr.pp_double[p->ptr.p_int[i]][j] = v;
            }
        }
    }
    rmatrixlefttrsm(n, m, lua, 0, 0, ae_false, ae_true, 0, b, 0, 0, _state);
    rmatrixlefttrsm(n, m, lua, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    return result;
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where BOTH ORIGINAL A AND ITS
LU DECOMPOSITION ARE KNOWN. You can use it if for some  reasons  you  have
both A and its LU decomposition.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolve(/* Real    */ const ae_matrix* a,
     /* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixMixedSolve: N<=0", _state);
    ae_assert(a->rows>=n, "RMatrixMixedSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "RMatrixMixedSolve: cols(A)<N", _state);
    ae_assert(lua->rows>=n, "RMatrixMixedSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixMixedSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixMixedSolve: length(P)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixMixedSolve: length(B)<N", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixMixedSolve: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixMixedSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixMixedSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    rcopyvc(n, b, &bm, 0, _state);
    rmatrixmixedsolvem(a, lua, p, n, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    rcopycv(n, &xm, 0, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixMixedSolve() but  solves task with multiple right  parts
(where b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolvem(/* Real    */ const ae_matrix* a,
     /* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;

    ae_matrix_clear(x);
    _densesolverreport_clear(rep);

    ae_assert(n>0, "RMatrixMixedSolveM: N<=0", _state);
    ae_assert(m>0, "RMatrixMixedSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "RMatrixMixedSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "RMatrixMixedSolve: cols(A)<N", _state);
    ae_assert(lua->rows>=n, "RMatrixMixedSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixMixedSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixMixedSolve: length(P)<N", _state);
    ae_assert(b->rows>=n, "RMatrixMixedSolve: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixMixedSolve: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixMixedSolve: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixMixedSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixMixedSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    directdensesolvers_rmatrixlusolveinternal(lua, p, n, a, ae_true, b, m, x, rep, _state);
}


/*************************************************************************
Complex dense solver for A*X=B with N*N  complex  matrix  A,  N*M  complex
matrices  X  and  B.  "Slow-but-feature-rich"   version   which   provides
additional functions, at the cost of slower  performance.  Faster  version
may be invoked with CMatrixSolveMFast() function.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3+M*N^2) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, CMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolvem(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     ae_bool rfs,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_matrix emptya;
    ae_vector p;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&da, 0, sizeof(da));
    memset(&emptya, 0, sizeof(emptya));
    memset(&p, 0, sizeof(p));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&da, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "CMatrixSolveM: N<=0", _state);
    ae_assert(m>0, "CMatrixSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "CMatrixSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "CMatrixSolveM: cols(A)<N", _state);
    ae_assert(b->rows>=n, "CMatrixSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixSolveM: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(a, n, n, _state), "CMatrixSolveM: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixSolveM: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * factorize, solve
     */
    for(i=0; i<=n-1; i++)
    {
        ae_v_cmove(&da.ptr.pp_complex[i][0], 1, &a->ptr.pp_complex[i][0], 1, "N", ae_v_len(0,n-1));
    }
    cmatrixlu(&da, n, n, &p, _state);
    if( rfs )
    {
        directdensesolvers_cmatrixlusolveinternal(&da, &p, n, a, ae_true, b, m, x, rep, _state);
    }
    else
    {
        directdensesolvers_cmatrixlusolveinternal(&da, &p, n, &emptya, ae_false, b, m, x, rep, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Complex dense solver for A*X=B with N*N  complex  matrix  A,  N*M  complex
matrices  X  and  B.  "Fast-but-lightweight" version which  provides  just
triangular solver - and no additional functions like iterative  refinement
or condition number estimation.

Algorithm features:
* O(N^3+M*N^2) complexity
* no additional time consuming functions

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

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
     Copyright 16.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixsolvemfast(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_complex v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector p;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&p, 0, sizeof(p));
    ae_matrix_init_copy(&a, _a, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "CMatrixSolveMFast: N<=0", _state);
    ae_assert(m>0, "CMatrixSolveMFast: M<=0", _state);
    ae_assert(a.rows>=n, "CMatrixSolveMFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "CMatrixSolveMFast: cols(A)<N", _state);
    ae_assert(b->rows>=n, "CMatrixSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixSolveMFast: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(&a, n, n, _state), "CMatrixSolveMFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixSolveMFast: B contains infinite or NaN values!", _state);
    
    /*
     * Check for exact degeneracy
     */
    cmatrixlu(&a, n, n, &p, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_c_eq_d(a.ptr.pp_complex[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=m-1; k++)
                {
                    b->ptr.pp_complex[j][k] = ae_complex_from_d(0.0);
                }
            }
            result = ae_false;
            ae_frame_leave(_state);
            return result;
        }
    }
    result = ae_true;
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        if( p.ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = b->ptr.pp_complex[i][j];
                b->ptr.pp_complex[i][j] = b->ptr.pp_complex[p.ptr.p_int[i]][j];
                b->ptr.pp_complex[p.ptr.p_int[i]][j] = v;
            }
        }
    }
    cmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_true, 0, b, 0, 0, _state);
    cmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Complex dense solver for A*x=B with N*N complex matrix A and  N*1  complex
vectors x and b. "Slow-but-feature-rich" version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, CMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolve(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixSolve: N<=0", _state);
    ae_assert(a->rows>=n, "CMatrixSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "CMatrixSolve: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixSolve: length(B)<N", _state);
    ae_assert(isfinitecmatrix(a, n, n, _state), "CMatrixSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixSolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixsolvem(a, n, &bm, 1, ae_true, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Complex dense solver for A*x=B with N*N complex matrix A and  N*1  complex
vectors x and b. "Fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3) complexity
* no additional time consuming features, just triangular solver

INPUT PARAMETERS:
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS:
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixsolvefast(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     /* Complex */ ae_vector* b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_int_t j;
    ae_vector p;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&p, 0, sizeof(p));
    ae_matrix_init_copy(&a, _a, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "CMatrixSolveFast: N<=0", _state);
    ae_assert(a.rows>=n, "CMatrixSolveFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "CMatrixSolveFast: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixSolveFast: length(B)<N", _state);
    ae_assert(isfinitecmatrix(&a, n, n, _state), "CMatrixSolveFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixSolveFast: B contains infinite or NaN values!", _state);
    cmatrixlu(&a, n, n, &p, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_c_eq_d(a.ptr.pp_complex[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.p_complex[j] = ae_complex_from_d(0.0);
            }
            result = ae_false;
            ae_frame_leave(_state);
            return result;
        }
    }
    directdensesolvers_cbasiclusolve(&a, &p, n, b, _state);
    result = ae_true;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver for A*X=B with N*N complex A given by its  LU  decomposition,
and N*M matrices X and B (multiple right sides).   "Slow-but-feature-rich"
version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! CMatrixLUSolveMFast() function.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolvem(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&emptya, 0, sizeof(emptya));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixLUSolveM: N<=0", _state);
    ae_assert(m>0, "CMatrixLUSolveM: M<=0", _state);
    ae_assert(lua->rows>=n, "CMatrixLUSolveM: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixLUSolveM: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixLUSolveM: length(P)<N", _state);
    ae_assert(b->rows>=n, "CMatrixLUSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixLUSolveM: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixLUSolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixLUSolveM: LUA contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixLUSolveM: P contains values outside of [0,N)", _state);
    }
    directdensesolvers_cmatrixlusolveinternal(lua, p, n, &emptya, ae_false, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N complex A given by its  LU  decomposition,
and N*M matrices X and B (multiple  right  sides).  "Fast-but-lightweight"
version of the solver.

Algorithm features:
* O(M*N^2) complexity
* no additional time-consuming features

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixlusolvemfast(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_complex v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "CMatrixLUSolveMFast: N<=0", _state);
    ae_assert(m>0, "CMatrixLUSolveMFast: M<=0", _state);
    ae_assert(lua->rows>=n, "CMatrixLUSolveMFast: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixLUSolveMFast: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixLUSolveMFast: length(P)<N", _state);
    ae_assert(b->rows>=n, "CMatrixLUSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixLUSolveMFast: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixLUSolveMFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixLUSolveMFast: LUA contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    
    /*
     * Check for exact degeneracy
     */
    for(i=0; i<=n-1; i++)
    {
        if( ae_c_eq_d(lua->ptr.pp_complex[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=m-1; k++)
                {
                    b->ptr.pp_complex[j][k] = ae_complex_from_d(0.0);
                }
            }
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = b->ptr.pp_complex[i][j];
                b->ptr.pp_complex[i][j] = b->ptr.pp_complex[p->ptr.p_int[i]][j];
                b->ptr.pp_complex[p->ptr.p_int[i]][j] = v;
            }
        }
    }
    cmatrixlefttrsm(n, m, lua, 0, 0, ae_false, ae_true, 0, b, 0, 0, _state);
    cmatrixlefttrsm(n, m, lua, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    return result;
}


/*************************************************************************
Complex dense linear solver for A*x=b with complex N*N A  given  by its LU
decomposition and N*1 vectors x and b. This is  "slow-but-robust"  version
of  the  complex  linear  solver  with  additional  features   which   add
significant performance overhead. Faster version  is  CMatrixLUSolveFast()
function.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! CMatrixLUSolveFast() function.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolve(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixLUSolve: N<=0", _state);
    ae_assert(lua->rows>=n, "CMatrixLUSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixLUSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixLUSolve: length(P)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixLUSolve: length(B)<N", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixLUSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixLUSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixlusolvem(lua, p, n, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Complex dense linear solver for A*x=b with N*N complex A given by  its  LU
decomposition and N*1 vectors x and b. This is  fast  lightweight  version
of solver, which is significantly faster than CMatrixLUSolve(),  but  does
not provide additional information (like condition numbers).

Algorithm features:
* O(N^2) complexity
* no additional time-consuming features, just triangular solver

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros
    
NOTE: unlike  CMatrixLUSolve(),  this   function   does   NOT   check  for
      near-degeneracy of input matrix. It  checks  for  EXACT  degeneracy,
      because this check is easy to do. However,  very  badly  conditioned
      matrices may went unnoticed.


  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixlusolvefast(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>0, "CMatrixLUSolveFast: N<=0", _state);
    ae_assert(lua->rows>=n, "CMatrixLUSolveFast: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixLUSolveFast: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixLUSolveFast: length(P)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixLUSolveFast: length(B)<N", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixLUSolveFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixLUSolveFast: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixLUSolveFast: P contains values outside of [0,N)", _state);
    }
    for(i=0; i<=n-1; i++)
    {
        if( ae_c_eq_d(lua->ptr.pp_complex[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.p_complex[j] = ae_complex_from_d(0.0);
            }
            result = ae_false;
            return result;
        }
    }
    directdensesolvers_cbasiclusolve(lua, p, n, b, _state);
    result = ae_true;
    return result;
}


/*************************************************************************
Dense solver. Same as RMatrixMixedSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolvem(/* Complex */ const ae_matrix* a,
     /* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;

    ae_matrix_clear(x);
    _densesolverreport_clear(rep);

    ae_assert(n>0, "CMatrixMixedSolveM: N<=0", _state);
    ae_assert(m>0, "CMatrixMixedSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "CMatrixMixedSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "CMatrixMixedSolveM: cols(A)<N", _state);
    ae_assert(lua->rows>=n, "CMatrixMixedSolveM: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixMixedSolveM: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixMixedSolveM: length(P)<N", _state);
    ae_assert(b->rows>=n, "CMatrixMixedSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixMixedSolveM: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(a, n, n, _state), "CMatrixMixedSolveM: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixMixedSolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixMixedSolveM: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixMixedSolveM: P contains values outside of [0,N)", _state);
    }
    directdensesolvers_cmatrixlusolveinternal(lua, p, n, a, ae_true, b, m, x, rep, _state);
}


/*************************************************************************
Dense solver. Same as RMatrixMixedSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolve(/* Complex */ const ae_matrix* a,
     /* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixMixedSolve: N<=0", _state);
    ae_assert(a->rows>=n, "CMatrixMixedSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "CMatrixMixedSolve: cols(A)<N", _state);
    ae_assert(lua->rows>=n, "CMatrixMixedSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixMixedSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixMixedSolve: length(P)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixMixedSolve: length(B)<N", _state);
    ae_assert(isfinitecmatrix(a, n, n, _state), "CMatrixMixedSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixMixedSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixMixedSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixMixedSolve: P contains values outside of [0,N)", _state);
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixmixedsolvem(a, lua, p, n, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A,  and
N*M vectors X and B. It is "slow-but-feature-rich" version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, SPDMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolvem(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;

    ae_frame_make(_state, &_frame_block);
    memset(&da, 0, sizeof(da));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&da, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixSolveM: N<=0", _state);
    ae_assert(m>0, "SPDMatrixSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "SPDMatrixSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixSolveM: cols(A)<N", _state);
    ae_assert(b->rows>=n, "SPDMatrixSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "SPDMatrixSolveM: cols(B)<M", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixSolveM: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "SPDMatrixSolveM: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * factorize
     * solve
     */
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        ae_v_move(&da.ptr.pp_double[i][j1], 1, &a->ptr.pp_double[i][j1], 1, ae_v_len(j1,j2));
    }
    if( !spdmatrixcholesky(&da, n, isupper, _state) )
    {
        ae_matrix_set_length(x, n, m, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = (double)(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    rep->terminationtype = 1;
    directdensesolvers_spdmatrixcholeskysolveinternal(&da, n, isupper, a, ae_true, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A,  and
N*M vectors X and B. It is "fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N,M], it contains:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

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
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixsolvemfast(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_int_t j;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    ae_matrix_init_copy(&a, _a, _state, ae_true);

    ae_assert(n>0, "SPDMatrixSolveMFast: N<=0", _state);
    ae_assert(a.rows>=n, "SPDMatrixSolveMFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "SPDMatrixSolveMFast: cols(A)<N", _state);
    ae_assert(b->rows>=n, "SPDMatrixSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "SPDMatrixSolveMFast: cols(B)<M", _state);
    ae_assert(isfinitertrmatrix(&a, n, isupper, _state), "SPDMatrixSolveMFast: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "SPDMatrixSolveMFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    if( !spdmatrixcholesky(&a, n, isupper, _state) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                b->ptr.pp_double[i][j] = 0.0;
            }
        }
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    if( isupper )
    {
        rmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 1, b, 0, 0, _state);
        rmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    }
    else
    {
        rmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_false, 0, b, 0, 0, _state);
        rmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_false, 1, b, 0, 0, _state);
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense linear solver for A*x=b with N*N real  symmetric  positive  definite
matrix A,  N*1 vectors x and b.  "Slow-but-feature-rich"  version  of  the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, SPDMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolve(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixSolve: N<=0", _state);
    ae_assert(a->rows>=n, "SPDMatrixSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixSolve: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "SPDMatrixSolve: length(B)<N", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "SPDMatrixSolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    spdmatrixsolvem(a, n, isupper, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense linear solver for A*x=b with N*N real  symmetric  positive  definite
matrix A,  N*1 vectors x and  b.  "Fast-but-lightweight"  version  of  the
solver.

Algorithm features:
* O(N^3) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features like condition number estimation

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N], it contains:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

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
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixsolvefast(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    ae_matrix_init_copy(&a, _a, _state, ae_true);

    ae_assert(n>0, "SPDMatrixSolveFast: N<=0", _state);
    ae_assert(a.rows>=n, "SPDMatrixSolveFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "SPDMatrixSolveFast: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "SPDMatrixSolveFast: length(B)<N", _state);
    ae_assert(isfinitertrmatrix(&a, n, isupper, _state), "SPDMatrixSolveFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "SPDMatrixSolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    if( !spdmatrixcholesky(&a, n, isupper, _state) )
    {
        for(i=0; i<=n-1; i++)
        {
            b->ptr.p_double[i] = 0.0;
        }
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    directdensesolvers_spdbasiccholeskysolve(&a, n, isupper, b, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*M vectors X and B. It  is  "slow-but-
feature-rich" version of the solver which estimates  condition  number  of
the system.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver. Amount of  overhead  introduced  depends  on  M  (the
           ! larger - the more efficient).
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! SPDMatrixCholeskySolveMFast() function.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N]:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolvem(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;

    ae_frame_make(_state, &_frame_block);
    memset(&emptya, 0, sizeof(emptya));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixCholeskySolveM: N<=0", _state);
    ae_assert(m>0, "SPDMatrixCholeskySolveM: M<=0", _state);
    ae_assert(cha->rows>=n, "SPDMatrixCholeskySolveM: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "SPDMatrixCholeskySolveM: cols(CHA)<N", _state);
    ae_assert(b->rows>=n, "SPDMatrixCholeskySolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "SPDMatrixCholeskySolveM: cols(B)<M", _state);
    ae_assert(isfinitertrmatrix(cha, n, isupper, _state), "SPDMatrixCholeskySolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "SPDMatrixCholeskySolveM: LUA contains infinite or NaN values!", _state);
    directdensesolvers_spdmatrixcholeskysolveinternal(cha, n, isupper, &emptya, ae_false, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*M vectors X and B. It  is  "fast-but-
lightweight" version of  the  solver  which  just  solves  linear  system,
without any additional functions.

Algorithm features:
* O(M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional functionality

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system
    
  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixcholeskysolvemfast(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "SPDMatrixCholeskySolveMFast: N<=0", _state);
    ae_assert(m>0, "SPDMatrixCholeskySolveMFast: M<=0", _state);
    ae_assert(cha->rows>=n, "SPDMatrixCholeskySolveMFast: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "SPDMatrixCholeskySolveMFast: cols(CHA)<N", _state);
    ae_assert(b->rows>=n, "SPDMatrixCholeskySolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "SPDMatrixCholeskySolveMFast: cols(B)<M", _state);
    ae_assert(isfinitertrmatrix(cha, n, isupper, _state), "SPDMatrixCholeskySolveMFast: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "SPDMatrixCholeskySolveMFast: LUA contains infinite or NaN values!", _state);
    result = ae_true;
    for(k=0; k<=n-1; k++)
    {
        if( ae_fp_eq(cha->ptr.pp_double[k][k],0.0) )
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=m-1; j++)
                {
                    b->ptr.pp_double[i][j] = 0.0;
                }
            }
            result = ae_false;
            return result;
        }
    }
    if( isupper )
    {
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 1, b, 0, 0, _state);
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    }
    else
    {
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 0, b, 0, 0, _state);
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 1, b, 0, 0, _state);
    }
    return result;
}


/*************************************************************************
Dense solver for A*x=b with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*1 real vectors x and b. This is "slow-
but-feature-rich"  version  of  the  solver  which,  in  addition  to  the
solution, performs condition number estimation.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! SPDMatrixCholeskySolveFast() function.

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[N], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N]:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolve(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixCholeskySolve: N<=0", _state);
    ae_assert(cha->rows>=n, "SPDMatrixCholeskySolve: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "SPDMatrixCholeskySolve: cols(CHA)<N", _state);
    ae_assert(b->cnt>=n, "SPDMatrixCholeskySolve: length(B)<N", _state);
    ae_assert(isfinitertrmatrix(cha, n, isupper, _state), "SPDMatrixCholeskySolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "SPDMatrixCholeskySolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    spdmatrixcholeskysolvem(cha, n, isupper, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*x=b with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*1 real vectors x and b. This is "fast-
but-lightweight" version of the solver.

Algorithm features:
* O(N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional features

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[N], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system
    
  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixcholeskysolvefast(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "SPDMatrixCholeskySolveFast: N<=0", _state);
    ae_assert(cha->rows>=n, "SPDMatrixCholeskySolveFast: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "SPDMatrixCholeskySolveFast: cols(CHA)<N", _state);
    ae_assert(b->cnt>=n, "SPDMatrixCholeskySolveFast: length(B)<N", _state);
    ae_assert(isfinitertrmatrix(cha, n, isupper, _state), "SPDMatrixCholeskySolveFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "SPDMatrixCholeskySolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    for(k=0; k<=n-1; k++)
    {
        if( ae_fp_eq(cha->ptr.pp_double[k][k],0.0) )
        {
            for(i=0; i<=n-1; i++)
            {
                b->ptr.p_double[i] = 0.0;
            }
            result = ae_false;
            return result;
        }
    }
    directdensesolvers_spdbasiccholeskysolve(cha, n, isupper, b, _state);
    return result;
}


/*************************************************************************
Dense solver for A*X=B, with N*N Hermitian positive definite matrix A  and
N*M  complex  matrices  X  and  B.  "Slow-but-feature-rich" version of the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems (N<100).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolvem(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;

    ae_frame_make(_state, &_frame_block);
    memset(&da, 0, sizeof(da));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&da, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixSolveM: N<=0", _state);
    ae_assert(m>0, "HPDMatrixSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "HPDMatrixSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "HPDMatrixSolveM: cols(A)<N", _state);
    ae_assert(b->rows>=n, "HPDMatrixSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "HPDMatrixSolveM: cols(B)<M", _state);
    ae_assert(isfinitectrmatrix(a, n, isupper, _state), "HPDMatrixSolveM: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "HPDMatrixSolveM: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * factorize matrix, solve
     */
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        ae_v_cmove(&da.ptr.pp_complex[i][j1], 1, &a->ptr.pp_complex[i][j1], 1, "N", ae_v_len(j1,j2));
    }
    if( !hpdmatrixcholesky(&da, n, isupper, _state) )
    {
        ae_matrix_set_length(x, n, m, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_i(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    rep->terminationtype = 1;
    directdensesolvers_hpdmatrixcholeskysolveinternal(&da, n, isupper, a, ae_true, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B, with N*N Hermitian positive definite matrix A  and
N*M complex matrices X and B. "Fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features like condition number estimation

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[0..N-1]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system
    
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
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixsolvemfast(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_int_t j;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    ae_matrix_init_copy(&a, _a, _state, ae_true);

    ae_assert(n>0, "HPDMatrixSolveMFast: N<=0", _state);
    ae_assert(a.rows>=n, "HPDMatrixSolveMFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "HPDMatrixSolveMFast: cols(A)<N", _state);
    ae_assert(b->rows>=n, "HPDMatrixSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "HPDMatrixSolveMFast: cols(B)<M", _state);
    ae_assert(isfinitectrmatrix(&a, n, isupper, _state), "HPDMatrixSolveMFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "HPDMatrixSolveMFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    if( !hpdmatrixcholesky(&a, n, isupper, _state) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                b->ptr.pp_complex[i][j] = ae_complex_from_d(0.0);
            }
        }
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    if( isupper )
    {
        cmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 2, b, 0, 0, _state);
        cmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    }
    else
    {
        cmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_false, 0, b, 0, 0, _state);
        cmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_false, 2, b, 0, 0, _state);
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver for A*x=b, with N*N Hermitian positive definite matrix A, and
N*1 complex vectors  x  and  b.  "Slow-but-feature-rich"  version  of  the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, HPDMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

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
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolve(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixSolve: N<=0", _state);
    ae_assert(a->rows>=n, "HPDMatrixSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "HPDMatrixSolve: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "HPDMatrixSolve: length(B)<N", _state);
    ae_assert(isfinitectrmatrix(a, n, isupper, _state), "HPDMatrixSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "HPDMatrixSolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    hpdmatrixsolvem(a, n, isupper, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*x=b, with N*N Hermitian positive definite matrix A, and
N*1 complex vectors  x  and  b.  "Fast-but-lightweight"  version  of   the
solver without additional functions.

Algorithm features:
* O(N^3) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming functions

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[0..N-1]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

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
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixsolvefast(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    ae_matrix_init_copy(&a, _a, _state, ae_true);

    ae_assert(n>0, "HPDMatrixSolveFast: N<=0", _state);
    ae_assert(a.rows>=n, "HPDMatrixSolveFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "HPDMatrixSolveFast: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "HPDMatrixSolveFast: length(B)<N", _state);
    ae_assert(isfinitectrmatrix(&a, n, isupper, _state), "HPDMatrixSolveFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "HPDMatrixSolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    if( !hpdmatrixcholesky(&a, n, isupper, _state) )
    {
        for(i=0; i<=n-1; i++)
        {
            b->ptr.p_complex[i] = ae_complex_from_d(0.0);
        }
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    directdensesolvers_hpdbasiccholeskysolve(&a, n, isupper, b, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver for A*X=B with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition and N*M complex matrices X  and  B.  This is
"slow-but-feature-rich" version of the solver which, in  addition  to  the
solution, estimates condition number of the system.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver. Amount of  overhead  introduced  depends  on  M  (the
           ! larger - the more efficient).
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large Cholesky decomposition.  However,  if  you call
           ! this  function  many  times  for  the same  left  side,  this
           ! overhead BECOMES significant. It  also   becomes  significant
           ! for small-scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixCholeskySolveMFast() function.


INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS:
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolvem(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;

    ae_frame_make(_state, &_frame_block);
    memset(&emptya, 0, sizeof(emptya));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixCholeskySolveM: N<=0", _state);
    ae_assert(m>0, "HPDMatrixCholeskySolveM: M<=0", _state);
    ae_assert(cha->rows>=n, "HPDMatrixCholeskySolveM: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "HPDMatrixCholeskySolveM: cols(CHA)<N", _state);
    ae_assert(b->rows>=n, "HPDMatrixCholeskySolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "HPDMatrixCholeskySolveM: cols(B)<M", _state);
    ae_assert(isfinitectrmatrix(cha, n, isupper, _state), "HPDMatrixCholeskySolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "HPDMatrixCholeskySolveM: LUA contains infinite or NaN values!", _state);
    directdensesolvers_hpdmatrixcholeskysolveinternal(cha, n, isupper, &emptya, ae_false, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition and N*M complex matrices X  and  B.  This is
"fast-but-lightweight" version of the solver.

Algorithm features:
* O(M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time-consuming features

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixcholeskysolvemfast(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "HPDMatrixCholeskySolveMFast: N<=0", _state);
    ae_assert(m>0, "HPDMatrixCholeskySolveMFast: M<=0", _state);
    ae_assert(cha->rows>=n, "HPDMatrixCholeskySolveMFast: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "HPDMatrixCholeskySolveMFast: cols(CHA)<N", _state);
    ae_assert(b->rows>=n, "HPDMatrixCholeskySolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "HPDMatrixCholeskySolveMFast: cols(B)<M", _state);
    ae_assert(isfinitectrmatrix(cha, n, isupper, _state), "HPDMatrixCholeskySolveMFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "HPDMatrixCholeskySolveMFast: LUA contains infinite or NaN values!", _state);
    result = ae_true;
    for(k=0; k<=n-1; k++)
    {
        if( ae_fp_eq(cha->ptr.pp_complex[k][k].x,0.0)&&ae_fp_eq(cha->ptr.pp_complex[k][k].y,0.0) )
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=m-1; j++)
                {
                    b->ptr.pp_complex[i][j] = ae_complex_from_d(0.0);
                }
            }
            result = ae_false;
            return result;
        }
    }
    if( isupper )
    {
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 2, b, 0, 0, _state);
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    }
    else
    {
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 0, b, 0, 0, _state);
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 2, b, 0, 0, _state);
    }
    return result;
}


/*************************************************************************
Dense solver for A*x=b with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition, and N*1 complex vectors x and  b.  This  is
"slow-but-feature-rich" version of the solver  which  estimates  condition
number of the system.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixCholeskySolveFast() function.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolve(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixCholeskySolve: N<=0", _state);
    ae_assert(cha->rows>=n, "HPDMatrixCholeskySolve: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "HPDMatrixCholeskySolve: cols(CHA)<N", _state);
    ae_assert(b->cnt>=n, "HPDMatrixCholeskySolve: length(B)<N", _state);
    ae_assert(isfinitectrmatrix(cha, n, isupper, _state), "HPDMatrixCholeskySolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "HPDMatrixCholeskySolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    hpdmatrixcholeskysolvem(cha, n, isupper, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*x=b with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition, and N*1 complex vectors x and  b.  This  is
"fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time-consuming features

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixcholeskysolvefast(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "HPDMatrixCholeskySolveFast: N<=0", _state);
    ae_assert(cha->rows>=n, "HPDMatrixCholeskySolveFast: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "HPDMatrixCholeskySolveFast: cols(CHA)<N", _state);
    ae_assert(b->cnt>=n, "HPDMatrixCholeskySolveFast: length(B)<N", _state);
    ae_assert(isfinitectrmatrix(cha, n, isupper, _state), "HPDMatrixCholeskySolveFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "HPDMatrixCholeskySolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    for(k=0; k<=n-1; k++)
    {
        if( ae_fp_eq(cha->ptr.pp_complex[k][k].x,0.0)&&ae_fp_eq(cha->ptr.pp_complex[k][k].y,0.0) )
        {
            for(i=0; i<=n-1; i++)
            {
                b->ptr.p_complex[i] = ae_complex_from_d(0.0);
            }
            result = ae_false;
            return result;
        }
    }
    directdensesolvers_hpdbasiccholeskysolve(cha, n, isupper, b, _state);
    return result;
}


/*************************************************************************
Dense solver.

This subroutine finds solution of the linear system A*X=B with non-square,
possibly degenerate A.  System  is  solved in the least squares sense, and
general least squares solution  X = X0 + CX*y  which  minimizes |A*X-B| is
returned. If A is non-degenerate, solution in the usual sense is returned.

Algorithm features:
* automatic detection (and correct handling!) of degenerate cases
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..NRows-1,0..NCols-1], system matrix
    NRows   -   vertical size of A
    NCols   -   horizontal size of A
    B       -   array[0..NCols-1], right part
    Threshold-  a number in [0,1]. Singular values  beyond  Threshold*Largest are
                considered  zero.  Set  it to 0.0, if you don't understand
                what it means, so the solver will choose good value on its
                own.
                
OUTPUT PARAMETERS
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1,0..M-1], it contains:
                * solution of A*X=B (even for singular A)
                * zeros, if SVD subroutine failed

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* TerminationType is set to:
            * -4 for SVD failure
            * >0 for success
* R2        reciprocal of condition number: 1/cond(A), 2-norm.
* N         = NCols
* K         dim(Null(A))
* CX        array[0..N-1,0..K-1], kernel of A.
            Columns of CX store such vectors that A*CX[i]=0.

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
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvels(/* Real    */ const ae_matrix* a,
     ae_int_t nrows,
     ae_int_t ncols,
     /* Real    */ const ae_vector* b,
     double threshold,
     /* Real    */ ae_vector* x,
     densesolverlsreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector sv;
    ae_matrix u;
    ae_matrix vt;
    ae_vector rp;
    ae_vector utb;
    ae_vector sutb;
    ae_vector tmp;
    ae_vector ta;
    ae_vector tx;
    ae_vector buf;
    ae_vector w;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nsv;
    ae_int_t kernelidx;
    double v;
    double verr;
    ae_bool svdfailed;
    ae_bool zeroa;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_bool terminatenexttime;
    ae_bool smallerr;

    ae_frame_make(_state, &_frame_block);
    memset(&sv, 0, sizeof(sv));
    memset(&u, 0, sizeof(u));
    memset(&vt, 0, sizeof(vt));
    memset(&rp, 0, sizeof(rp));
    memset(&utb, 0, sizeof(utb));
    memset(&sutb, 0, sizeof(sutb));
    memset(&tmp, 0, sizeof(tmp));
    memset(&ta, 0, sizeof(ta));
    memset(&tx, 0, sizeof(tx));
    memset(&buf, 0, sizeof(buf));
    memset(&w, 0, sizeof(w));
    ae_vector_clear(x);
    _densesolverlsreport_clear(rep);
    ae_vector_init(&sv, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&u, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vt, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&rp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&utb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sutb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ta, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&buf, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);

    ae_assert(nrows>0, "RMatrixSolveLS: NRows<=0", _state);
    ae_assert(ncols>0, "RMatrixSolveLS: NCols<=0", _state);
    ae_assert(ae_isfinite(threshold, _state)&&ae_fp_greater_eq(threshold,(double)(0)), "RMatrixSolveLS: Threshold<0 or is infinite", _state);
    ae_assert(a->rows>=nrows, "RMatrixSolveLS: rows(A)<NRows", _state);
    ae_assert(a->cols>=ncols, "RMatrixSolveLS: cols(A)<NCols", _state);
    ae_assert(b->cnt>=nrows, "RMatrixSolveLS: length(B)<NRows", _state);
    ae_assert(apservisfinitematrix(a, nrows, ncols, _state), "RMatrixSolveLS: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, nrows, _state), "RMatrixSolveLS: B contains infinite or NaN values!", _state);
    if( ae_fp_eq(threshold,(double)(0)) )
    {
        threshold = (double)1000*ae_machineepsilon;
    }
    
    /*
     * Factorize A first
     */
    svdfailed = !rmatrixsvd(a, nrows, ncols, 1, 2, 2, &sv, &u, &vt, _state);
    zeroa = ae_fp_eq(sv.ptr.p_double[0],(double)(0));
    if( svdfailed||zeroa )
    {
        if( svdfailed )
        {
            rep->terminationtype = -4;
        }
        else
        {
            rep->terminationtype = 1;
        }
        ae_vector_set_length(x, ncols, _state);
        for(i=0; i<=ncols-1; i++)
        {
            x->ptr.p_double[i] = (double)(0);
        }
        rep->n = ncols;
        rep->k = ncols;
        ae_matrix_set_length(&rep->cx, ncols, ncols, _state);
        for(i=0; i<=ncols-1; i++)
        {
            for(j=0; j<=ncols-1; j++)
            {
                if( i==j )
                {
                    rep->cx.ptr.pp_double[i][j] = (double)(1);
                }
                else
                {
                    rep->cx.ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        rep->r2 = (double)(0);
        ae_frame_leave(_state);
        return;
    }
    nsv = ae_minint(ncols, nrows, _state);
    if( nsv==ncols )
    {
        rep->r2 = sv.ptr.p_double[nsv-1]/sv.ptr.p_double[0];
    }
    else
    {
        rep->r2 = (double)(0);
    }
    rep->n = ncols;
    rep->terminationtype = 1;
    
    /*
     * Iterative refinement of xc combined with solution:
     * 1. xc = 0
     * 2. calculate r = bc-A*xc using extra-precise dot product
     * 3. solve A*y = r
     * 4. update x:=x+r
     * 5. goto 2
     *
     * This cycle is executed until one of two things happens:
     * 1. maximum number of iterations reached
     * 2. last iteration decreased error to the lower limit
     */
    ae_vector_set_length(&utb, nsv, _state);
    ae_vector_set_length(&sutb, nsv, _state);
    ae_vector_set_length(x, ncols, _state);
    ae_vector_set_length(&tmp, ncols, _state);
    ae_vector_set_length(&ta, ncols+1, _state);
    ae_vector_set_length(&tx, ncols+1, _state);
    ae_vector_set_length(&buf, ncols+1, _state);
    for(i=0; i<=ncols-1; i++)
    {
        x->ptr.p_double[i] = (double)(0);
    }
    kernelidx = nsv;
    for(i=0; i<=nsv-1; i++)
    {
        if( ae_fp_less_eq(sv.ptr.p_double[i],threshold*sv.ptr.p_double[0]) )
        {
            kernelidx = i;
            break;
        }
    }
    rep->k = ncols-kernelidx;
    nrfs = directdensesolvers_densesolverrfsmaxv2(ncols, rep->r2, _state);
    terminatenexttime = ae_false;
    ae_vector_set_length(&rp, nrows, _state);
    for(rfs=0; rfs<=nrfs; rfs++)
    {
        if( terminatenexttime )
        {
            break;
        }
        
        /*
         * calculate right part
         */
        if( rfs==0 )
        {
            ae_v_move(&rp.ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,nrows-1));
        }
        else
        {
            smallerr = ae_true;
            for(i=0; i<=nrows-1; i++)
            {
                ae_v_move(&ta.ptr.p_double[0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,ncols-1));
                ta.ptr.p_double[ncols] = (double)(-1);
                ae_v_move(&tx.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,ncols-1));
                tx.ptr.p_double[ncols] = b->ptr.p_double[i];
                xdot(&ta, &tx, ncols+1, &buf, &v, &verr, _state);
                rp.ptr.p_double[i] = -v;
                smallerr = smallerr&&ae_fp_less(ae_fabs(v, _state),(double)4*verr);
            }
            if( smallerr )
            {
                terminatenexttime = ae_true;
            }
        }
        
        /*
         * solve A*dx = rp
         */
        for(i=0; i<=ncols-1; i++)
        {
            tmp.ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=nsv-1; i++)
        {
            utb.ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=nrows-1; i++)
        {
            v = rp.ptr.p_double[i];
            ae_v_addd(&utb.ptr.p_double[0], 1, &u.ptr.pp_double[i][0], 1, ae_v_len(0,nsv-1), v);
        }
        for(i=0; i<=nsv-1; i++)
        {
            if( i<kernelidx )
            {
                sutb.ptr.p_double[i] = utb.ptr.p_double[i]/sv.ptr.p_double[i];
            }
            else
            {
                sutb.ptr.p_double[i] = (double)(0);
            }
        }
        for(i=0; i<=nsv-1; i++)
        {
            v = sutb.ptr.p_double[i];
            ae_v_addd(&tmp.ptr.p_double[0], 1, &vt.ptr.pp_double[i][0], 1, ae_v_len(0,ncols-1), v);
        }
        
        /*
         * update x:  x:=x+dx
         */
        ae_v_add(&x->ptr.p_double[0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,ncols-1));
    }
    
    /*
     * fill CX
     */
    if( rep->k>0 )
    {
        ae_matrix_set_length(&rep->cx, ncols, rep->k, _state);
        for(i=0; i<=rep->k-1; i++)
        {
            ae_v_move(&rep->cx.ptr.pp_double[0][i], rep->cx.stride, &vt.ptr.pp_double[kernelidx+i][0], 1, ae_v_len(0,ncols-1));
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal LU solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_rmatrixlusolveinternal(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_bool havea,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;
    double v;
    double verr;
    double mxb;
    ae_bool smallerr;
    ae_bool terminatenexttime;

    ae_frame_make(_state, &_frame_block);
    memset(&xc, 0, sizeof(xc));
    memset(&y, 0, sizeof(y));
    memset(&bc, 0, sizeof(bc));
    memset(&xa, 0, sizeof(xa));
    memset(&xb, 0, sizeof(xb));
    memset(&tx, 0, sizeof(tx));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xa, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    ae_assert(n>0&&m>0, "RMatrixLUSolveInternal: integrity check 7656 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]<=n-1&&p->ptr.p_int[i]>=i, "RMatrixLUSolveInternal: incorrect pivot, P[i]<i or P[i]>=N", _state);
    }
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n+1, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = rmatrixlurcond1(lua, n, _state);
    rep->rinf = rmatrixlurcondinf(lua, n, _state);
    rep->terminationtype = 1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = (double)(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * First stage of solution: rough solution with TRSM()
     */
    mxb = 0.0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            v = b->ptr.pp_double[i][j];
            mxb = ae_maxreal(mxb, ae_fabs(v, _state), _state);
            x->ptr.pp_double[i][j] = v;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = x->ptr.pp_double[i][j];
                x->ptr.pp_double[i][j] = x->ptr.pp_double[p->ptr.p_int[i]][j];
                x->ptr.pp_double[p->ptr.p_int[i]][j] = v;
            }
        }
    }
    rmatrixlefttrsm(n, m, lua, 0, 0, ae_false, ae_true, 0, x, 0, 0, _state);
    rmatrixlefttrsm(n, m, lua, 0, 0, ae_true, ae_false, 0, x, 0, 0, _state);
    
    /*
     * Second stage: iterative refinement
     */
    if( havea )
    {
        for(k=0; k<=m-1; k++)
        {
            nrfs = directdensesolvers_densesolverrfsmax(n, rep->r1, rep->rinf, _state);
            terminatenexttime = ae_false;
            for(rfs=0; rfs<=nrfs-1; rfs++)
            {
                if( terminatenexttime )
                {
                    break;
                }
                
                /*
                 * generate right part
                 */
                smallerr = ae_true;
                ae_v_move(&xb.ptr.p_double[0], 1, &x->ptr.pp_double[0][k], x->stride, ae_v_len(0,n-1));
                for(i=0; i<=n-1; i++)
                {
                    ae_v_move(&xa.ptr.p_double[0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
                    xa.ptr.p_double[n] = (double)(-1);
                    xb.ptr.p_double[n] = b->ptr.pp_double[i][k];
                    xdot(&xa, &xb, n+1, &tx, &v, &verr, _state);
                    y.ptr.p_double[i] = -v;
                    smallerr = smallerr&&ae_fp_less(ae_fabs(v, _state),(double)4*verr);
                }
                if( smallerr )
                {
                    terminatenexttime = ae_true;
                }
                
                /*
                 * solve and update
                 */
                directdensesolvers_rbasiclusolve(lua, p, n, &y, _state);
                ae_v_add(&x->ptr.pp_double[0][k], x->stride, &y.ptr.p_double[0], 1, ae_v_len(0,n-1));
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal Cholesky solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_spdmatrixcholeskysolveinternal(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_matrix* a,
     ae_bool havea,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    ae_matrix_clear(x);
    _densesolverreport_clear(rep);

    
    /*
     * prepare: check inputs, allocate space...
     */
    ae_assert(n>0&&m>0, "SPDMatrixCholeskySolveInternal: integrity check 9858 failed", _state);
    ae_matrix_set_length(x, n, m, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = spdmatrixcholeskyrcond(cha, n, isupper, _state);
    rep->rinf = rep->r1;
    rep->terminationtype = 1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = (double)(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        return;
    }
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            x->ptr.pp_double[i][j] = b->ptr.pp_double[i][j];
        }
    }
    if( isupper )
    {
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 1, x, 0, 0, _state);
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 0, x, 0, 0, _state);
    }
    else
    {
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 0, x, 0, 0, _state);
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 1, x, 0, 0, _state);
    }
}


/*************************************************************************
Internal LU solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_cmatrixlusolveinternal(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_bool havea,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;
    ae_vector tmpbuf;
    ae_complex v;
    double verr;
    ae_bool smallerr;
    ae_bool terminatenexttime;

    ae_frame_make(_state, &_frame_block);
    memset(&xc, 0, sizeof(xc));
    memset(&y, 0, sizeof(y));
    memset(&bc, 0, sizeof(bc));
    memset(&xa, 0, sizeof(xa));
    memset(&xb, 0, sizeof(xb));
    memset(&tx, 0, sizeof(tx));
    memset(&tmpbuf, 0, sizeof(tmpbuf));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_vector_init(&xc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&y, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&bc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xa, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xb, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tx, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tmpbuf, 0, DT_REAL, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    ae_assert(n>0&&m>0, "CMatrixLUSolveInternal: integrity check 9302 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]<=n-1&&p->ptr.p_int[i]>=i, "CMatrixLUSolveInternal: incorrect pivot P[i]<i or P[I]>=N", _state);
    }
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    ae_vector_set_length(&tmpbuf, 2*n+2, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = cmatrixlurcond1(lua, n, _state);
    rep->rinf = cmatrixlurcondinf(lua, n, _state);
    rep->terminationtype = 1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_i(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * First phase: solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            x->ptr.pp_complex[i][j] = b->ptr.pp_complex[i][j];
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = x->ptr.pp_complex[i][j];
                x->ptr.pp_complex[i][j] = x->ptr.pp_complex[p->ptr.p_int[i]][j];
                x->ptr.pp_complex[p->ptr.p_int[i]][j] = v;
            }
        }
    }
    cmatrixlefttrsm(n, m, lua, 0, 0, ae_false, ae_true, 0, x, 0, 0, _state);
    cmatrixlefttrsm(n, m, lua, 0, 0, ae_true, ae_false, 0, x, 0, 0, _state);
    
    /*
     * solve
     */
    for(k=0; k<=m-1; k++)
    {
        ae_v_cmove(&bc.ptr.p_complex[0], 1, &b->ptr.pp_complex[0][k], b->stride, "N", ae_v_len(0,n-1));
        ae_v_cmove(&xc.ptr.p_complex[0], 1, &x->ptr.pp_complex[0][k], x->stride, "N", ae_v_len(0,n-1));
        
        /*
         * Iterative refinement of xc:
         * * calculate r = bc-A*xc using extra-precise dot product
         * * solve A*y = r
         * * update x:=x+r
         *
         * This cycle is executed until one of two things happens:
         * 1. maximum number of iterations reached
         * 2. last iteration decreased error to the lower limit
         */
        if( havea )
        {
            nrfs = directdensesolvers_densesolverrfsmax(n, rep->r1, rep->rinf, _state);
            terminatenexttime = ae_false;
            for(rfs=0; rfs<=nrfs-1; rfs++)
            {
                if( terminatenexttime )
                {
                    break;
                }
                
                /*
                 * generate right part
                 */
                smallerr = ae_true;
                ae_v_cmove(&xb.ptr.p_complex[0], 1, &xc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
                for(i=0; i<=n-1; i++)
                {
                    ae_v_cmove(&xa.ptr.p_complex[0], 1, &a->ptr.pp_complex[i][0], 1, "N", ae_v_len(0,n-1));
                    xa.ptr.p_complex[n] = ae_complex_from_i(-1);
                    xb.ptr.p_complex[n] = bc.ptr.p_complex[i];
                    xcdot(&xa, &xb, n+1, &tmpbuf, &v, &verr, _state);
                    y.ptr.p_complex[i] = ae_c_neg(v);
                    smallerr = smallerr&&ae_fp_less(ae_c_abs(v, _state),(double)4*verr);
                }
                if( smallerr )
                {
                    terminatenexttime = ae_true;
                }
                
                /*
                 * solve and update
                 */
                directdensesolvers_cbasiclusolve(lua, p, n, &y, _state);
                ae_v_cadd(&xc.ptr.p_complex[0], 1, &y.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
            }
        }
        
        /*
         * Store xc.
         * Post-scale result.
         */
        ae_v_cmove(&x->ptr.pp_complex[0][k], x->stride, &xc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal Cholesky solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_hpdmatrixcholeskysolveinternal(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_matrix* a,
     ae_bool havea,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;

    ae_frame_make(_state, &_frame_block);
    memset(&xc, 0, sizeof(xc));
    memset(&y, 0, sizeof(y));
    memset(&bc, 0, sizeof(bc));
    memset(&xa, 0, sizeof(xa));
    memset(&xb, 0, sizeof(xb));
    memset(&tx, 0, sizeof(tx));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_vector_init(&xc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&y, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&bc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xa, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xb, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tx, 0, DT_COMPLEX, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n+1, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = hpdmatrixcholeskyrcond(cha, n, isupper, _state);
    rep->rinf = rep->r1;
    rep->terminationtype = 1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_i(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * solve
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            x->ptr.pp_complex[i][j] = b->ptr.pp_complex[i][j];
        }
    }
    if( isupper )
    {
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 2, x, 0, 0, _state);
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 0, x, 0, 0, _state);
    }
    else
    {
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 0, x, 0, 0, _state);
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 2, x, 0, 0, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal subroutine.
Returns maximum count of RFS iterations as function of:
1. machine epsilon
2. task size.
3. condition number

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static ae_int_t directdensesolvers_densesolverrfsmax(ae_int_t n,
     double r1,
     double rinf,
     ae_state *_state)
{
    ae_int_t result;


    result = 5;
    return result;
}


/*************************************************************************
Internal subroutine.
Returns maximum count of RFS iterations as function of:
1. machine epsilon
2. task size.
3. norm-2 condition number

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static ae_int_t directdensesolvers_densesolverrfsmaxv2(ae_int_t n,
     double r2,
     ae_state *_state)
{
    ae_int_t result;


    result = directdensesolvers_densesolverrfsmax(n, (double)(0), (double)(0), _state);
    return result;
}


/*************************************************************************
Basic LU solver for PLU*x = y.

This subroutine assumes that:
* A=PLU is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_rbasiclusolve(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_vector* xb,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            v = xb->ptr.p_double[i];
            xb->ptr.p_double[i] = xb->ptr.p_double[p->ptr.p_int[i]];
            xb->ptr.p_double[p->ptr.p_int[i]] = v;
        }
    }
    for(i=1; i<=n-1; i++)
    {
        v = ae_v_dotproduct(&lua->ptr.pp_double[i][0], 1, &xb->ptr.p_double[0], 1, ae_v_len(0,i-1));
        xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
    }
    xb->ptr.p_double[n-1] = xb->ptr.p_double[n-1]/lua->ptr.pp_double[n-1][n-1];
    for(i=n-2; i>=0; i--)
    {
        v = ae_v_dotproduct(&lua->ptr.pp_double[i][i+1], 1, &xb->ptr.p_double[i+1], 1, ae_v_len(i+1,n-1));
        xb->ptr.p_double[i] = (xb->ptr.p_double[i]-v)/lua->ptr.pp_double[i][i];
    }
}


/*************************************************************************
Basic Cholesky solver for ScaleA*Cholesky(A)'*x = y.

This subroutine assumes that:
* A*ScaleA is well scaled
* A is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_spdbasiccholeskysolve(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* xb,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    
    /*
     * A = L*L' or A=U'*U
     */
    if( isupper )
    {
        
        /*
         * Solve U'*y=b first.
         */
        for(i=0; i<=n-1; i++)
        {
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/cha->ptr.pp_double[i][i];
            if( i<n-1 )
            {
                v = xb->ptr.p_double[i];
                ae_v_subd(&xb->ptr.p_double[i+1], 1, &cha->ptr.pp_double[i][i+1], 1, ae_v_len(i+1,n-1), v);
            }
        }
        
        /*
         * Solve U*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            if( i<n-1 )
            {
                v = ae_v_dotproduct(&cha->ptr.pp_double[i][i+1], 1, &xb->ptr.p_double[i+1], 1, ae_v_len(i+1,n-1));
                xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
            }
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/cha->ptr.pp_double[i][i];
        }
    }
    else
    {
        
        /*
         * Solve L*y=b first
         */
        for(i=0; i<=n-1; i++)
        {
            if( i>0 )
            {
                v = ae_v_dotproduct(&cha->ptr.pp_double[i][0], 1, &xb->ptr.p_double[0], 1, ae_v_len(0,i-1));
                xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
            }
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/cha->ptr.pp_double[i][i];
        }
        
        /*
         * Solve L'*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/cha->ptr.pp_double[i][i];
            if( i>0 )
            {
                v = xb->ptr.p_double[i];
                ae_v_subd(&xb->ptr.p_double[0], 1, &cha->ptr.pp_double[i][0], 1, ae_v_len(0,i-1), v);
            }
        }
    }
}


/*************************************************************************
Basic LU solver for ScaleA*PLU*x = y.

This subroutine assumes that:
* L is well-scaled, and it is U which needs scaling by ScaleA.
* A=PLU is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_cbasiclusolve(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_vector* xb,
     ae_state *_state)
{
    ae_int_t i;
    ae_complex v;


    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            v = xb->ptr.p_complex[i];
            xb->ptr.p_complex[i] = xb->ptr.p_complex[p->ptr.p_int[i]];
            xb->ptr.p_complex[p->ptr.p_int[i]] = v;
        }
    }
    for(i=1; i<=n-1; i++)
    {
        v = ae_v_cdotproduct(&lua->ptr.pp_complex[i][0], 1, "N", &xb->ptr.p_complex[0], 1, "N", ae_v_len(0,i-1));
        xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
    }
    xb->ptr.p_complex[n-1] = ae_c_div(xb->ptr.p_complex[n-1],lua->ptr.pp_complex[n-1][n-1]);
    for(i=n-2; i>=0; i--)
    {
        v = ae_v_cdotproduct(&lua->ptr.pp_complex[i][i+1], 1, "N", &xb->ptr.p_complex[i+1], 1, "N", ae_v_len(i+1,n-1));
        xb->ptr.p_complex[i] = ae_c_div(ae_c_sub(xb->ptr.p_complex[i],v),lua->ptr.pp_complex[i][i]);
    }
}


/*************************************************************************
Basic Cholesky solver for ScaleA*Cholesky(A)'*x = y.

This subroutine assumes that:
* A*ScaleA is well scaled
* A is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_hpdbasiccholeskysolve(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* xb,
     ae_state *_state)
{
    ae_int_t i;
    ae_complex v;


    
    /*
     * A = L*L' or A=U'*U
     */
    if( isupper )
    {
        
        /*
         * Solve U'*y=b first.
         */
        for(i=0; i<=n-1; i++)
        {
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],ae_c_conj(cha->ptr.pp_complex[i][i], _state));
            if( i<n-1 )
            {
                v = xb->ptr.p_complex[i];
                ae_v_csubc(&xb->ptr.p_complex[i+1], 1, &cha->ptr.pp_complex[i][i+1], 1, "Conj", ae_v_len(i+1,n-1), v);
            }
        }
        
        /*
         * Solve U*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            if( i<n-1 )
            {
                v = ae_v_cdotproduct(&cha->ptr.pp_complex[i][i+1], 1, "N", &xb->ptr.p_complex[i+1], 1, "N", ae_v_len(i+1,n-1));
                xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
            }
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],cha->ptr.pp_complex[i][i]);
        }
    }
    else
    {
        
        /*
         * Solve L*y=b first
         */
        for(i=0; i<=n-1; i++)
        {
            if( i>0 )
            {
                v = ae_v_cdotproduct(&cha->ptr.pp_complex[i][0], 1, "N", &xb->ptr.p_complex[0], 1, "N", ae_v_len(0,i-1));
                xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
            }
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],cha->ptr.pp_complex[i][i]);
        }
        
        /*
         * Solve L'*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],ae_c_conj(cha->ptr.pp_complex[i][i], _state));
            if( i>0 )
            {
                v = xb->ptr.p_complex[i];
                ae_v_csubc(&xb->ptr.p_complex[0], 1, &cha->ptr.pp_complex[i][0], 1, "Conj", ae_v_len(0,i-1), v);
            }
        }
    }
}


void _densesolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _densesolverreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    densesolverreport       *dst = (densesolverreport*)_dst;
    const densesolverreport *src = (const densesolverreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->r1 = src->r1;
    dst->rinf = src->rinf;
}


void _densesolverreport_clear(void* _p)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _densesolverreport_destroy(void* _p)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _densesolverlsreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->cx, 0, 0, DT_REAL, _state, make_automatic);
}


void _densesolverlsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    densesolverlsreport       *dst = (densesolverlsreport*)_dst;
    const densesolverlsreport *src = (const densesolverlsreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->r2 = src->r2;
    ae_matrix_init_copy(&dst->cx, &src->cx, _state, make_automatic);
    dst->n = src->n;
    dst->k = src->k;
}


void _densesolverlsreport_clear(void* _p)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->cx);
}


void _densesolverlsreport_destroy(void* _p)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->cx);
}


/*$ End $*/
