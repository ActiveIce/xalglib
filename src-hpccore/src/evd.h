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

#ifndef _evd_h
#define _evd_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "ortfac.h"
#include "matgen.h"
#include "scodes.h"
#include "tsort.h"
#include "sparse.h"
#include "blas.h"
#include "rotations.h"
#include "hsschur.h"
#include "basicstatops.h"


/*$ Declarations $*/


/*************************************************************************
This object stores state of the subspace iteration algorithm.

You should use ALGLIB functions to work with this object.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t nwork;
    ae_int_t maxits;
    double eps;
    ae_int_t eigenvectorsneeded;
    ae_int_t solvermode;
    ae_bool usewarmstart;
    ae_bool firstcall;
    hqrndstate rs;
    ae_bool running;
    ae_vector tau;
    ae_matrix q0;
    ae_matrix qcur;
    ae_matrix qnew;
    ae_matrix znew;
    ae_matrix r;
    ae_matrix rz;
    ae_matrix tz;
    ae_matrix rq;
    ae_matrix dummy;
    ae_vector rw;
    ae_vector tw;
    ae_vector tmprow;
    ae_vector wcur;
    ae_vector wprev;
    ae_vector wrank;
    apbuffers buf;
    ae_matrix x;
    ae_matrix ax;
    ae_int_t requesttype;
    ae_int_t requestsize;
    ae_int_t repiterationscount;
    rcommstate rstate;
} eigsubspacestate;


/*************************************************************************
This object stores state of the subspace iteration algorithm.

You should use ALGLIB functions to work with this object.
*************************************************************************/
typedef struct
{
    ae_int_t iterationscount;
} eigsubspacereport;


/*$ Body $*/


/*************************************************************************
This function initializes subspace iteration solver. This solver  is  used
to solve symmetric real eigenproblems where just a few (top K) eigenvalues
and corresponding eigenvectors is required.

This solver can be significantly faster than  complete  EVD  decomposition
in the following case:
* when only just a small fraction  of  top  eigenpairs  of dense matrix is
  required. When K approaches N, this solver is slower than complete dense
  EVD
* when problem matrix is sparse (and/or is not known explicitly, i.e. only
  matrix-matrix product can be performed)
  
USAGE (explicit dense/sparse matrix):
1. User initializes algorithm state with eigsubspacecreate() call
2. [optional] User tunes solver parameters by calling eigsubspacesetcond()
   or other functions
3. User  calls  eigsubspacesolvedense() or eigsubspacesolvesparse() methods,
   which take algorithm state and 2D array or alglib.sparsematrix object.
  
USAGE (out-of-core mode):
1. User initializes algorithm state with eigsubspacecreate() call
2. [optional] User tunes solver parameters by calling eigsubspacesetcond()
   or other functions
3. User activates out-of-core mode of  the  solver  and  repeatedly  calls
   communication functions in a loop like below:
   > alglib.eigsubspaceoocstart(state)
   > while alglib.eigsubspaceooccontinue(state) do
   >     alglib.eigsubspaceoocgetrequestinfo(state, out RequestType, out M)
   >     alglib.eigsubspaceoocgetrequestdata(state, out X)
   >     [calculate  Y=A*X, with X=R^NxM]
   >     alglib.eigsubspaceoocsendresult(state, in Y)
   > alglib.eigsubspaceoocstop(state, out W, out Z, out Report)
   
INPUT PARAMETERS:
    N       -   problem dimensionality, N>0
    K       -   number of top eigenvector to calculate, 0<K<=N.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE: if you solve many similar EVD problems you may  find  it  useful  to
      reuse previous subspace as warm-start point for new EVD problem.  It
      can be done with eigsubspacesetwarmstart() function.

  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspacecreate(ae_int_t n,
     ae_int_t k,
     eigsubspacestate* state,
     ae_state *_state);


/*************************************************************************
Buffered version of constructor which aims to reuse  previously  allocated
memory as much as possible.

  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspacecreatebuf(ae_int_t n,
     ae_int_t k,
     eigsubspacestate* state,
     ae_state *_state);


/*************************************************************************
This function sets stopping critera for the solver:
* error in eigenvector/value allowed by solver
* maximum number of iterations to perform

INPUT PARAMETERS:
    State       -   solver structure
    Eps         -   eps>=0,  with non-zero value used to tell solver  that
                    it can  stop  after  all  eigenvalues  converged  with
                    error  roughly  proportional  to  eps*MAX(LAMBDA_MAX),
                    where LAMBDA_MAX is a maximum eigenvalue.
                    Zero  value  means  that  no  check  for  precision is
                    performed.
    MaxIts      -   maxits>=0,  with non-zero value used  to  tell  solver
                    that it can stop after maxits  steps  (no  matter  how
                    precise current estimate is)

NOTE: passing  eps=0  and  maxits=0  results  in  automatic  selection  of
      moderate eps as stopping criteria (1.0E-6 in current implementation,
      but it may change without notice).
      
NOTE: very small values of eps are possible (say, 1.0E-12),  although  the
      larger problem you solve (N and/or K), the  harder  it  is  to  find
      precise eigenvectors because rounding errors tend to accumulate.
      
NOTE: passing non-zero eps results in  some performance  penalty,  roughly
      equal to 2N*(2K)^2 FLOPs per iteration. These additional computations
      are required in order to estimate current error in  eigenvalues  via
      Rayleigh-Ritz process.
      Most of this additional time is  spent  in  construction  of  ~2Kx2K
      symmetric  subproblem  whose  eigenvalues  are  checked  with  exact
      eigensolver.
      This additional time is negligible if you search for eigenvalues  of
      the large dense matrix, but may become noticeable on  highly  sparse
      EVD problems, where cost of matrix-matrix product is low.
      If you set eps to exactly zero,  Rayleigh-Ritz  phase  is completely
      turned off.

  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspacesetcond(eigsubspacestate* state,
     double eps,
     ae_int_t maxits,
     ae_state *_state);


/*************************************************************************
This function sets warm-start mode of the solver: next call to the  solver
will reuse previous subspace as warm-start  point.  It  can  significantly
speed-up convergence when you solve many similar eigenproblems.

INPUT PARAMETERS:
    State       -   solver structure
    UseWarmStart-   either True or False

  -- ALGLIB --
     Copyright 12.11.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspacesetwarmstart(eigsubspacestate* state,
     ae_bool usewarmstart,
     ae_state *_state);


/*************************************************************************
This  function  initiates  out-of-core  mode  of  subspace eigensolver. It
should be used in conjunction with other out-of-core-related functions  of
this subspackage in a loop like below:

> alglib.eigsubspaceoocstart(state)
> while alglib.eigsubspaceooccontinue(state) do
>     alglib.eigsubspaceoocgetrequestinfo(state, out RequestType, out M)
>     alglib.eigsubspaceoocgetrequestdata(state, out X)
>     [calculate  Y=A*X, with X=R^NxM]
>     alglib.eigsubspaceoocsendresult(state, in Y)
> alglib.eigsubspaceoocstop(state, out W, out Z, out Report)

INPUT PARAMETERS:
    State       -   solver object
    MType       -   matrix type and solver mode:
    
                    * 0 =   real symmetric matrix A, products  of the form
                            A*X are computed. At every step  the  basis of
                            the  invariant  subspace  is  reorthogonalized
                            with LQ decomposition  which  makes  the  algo
                            more robust.
                            
                            The first mode introduced in ALGLIB, the  most
                            precise and robust. However, it is  suboptimal
                            for easy problems which can be solved  in  3-5
                            iterations without LQ step.
                            
                    * 1 =   real symmetric matrix A, products  of the form
                            A*X are computed. The  invariant  subspace  is
                            NOT reorthogonalized,  no  error  checks.  The
                            solver  stops  after   specified   number   of
                            iterations which should be small, 5 at most.
                            
                            This mode is intended for easy  problems  with
                            extremely fast convergence.
                    
                    Future versions of ALGLIB may  introduce  support  for
                    other  matrix   types;   for   now,   only   symmetric
                    eigenproblems are supported.


  -- ALGLIB --
     Copyright 07.06.2023 by Bochkanov Sergey
*************************************************************************/
void eigsubspaceoocstart(eigsubspacestate* state,
     ae_int_t mtype,
     ae_state *_state);


/*************************************************************************
This function performs subspace iteration  in  the  out-of-core  mode.  It
should be used in conjunction with other out-of-core-related functions  of
this subspackage in a loop like below:

> alglib.eigsubspaceoocstart(state)
> while alglib.eigsubspaceooccontinue(state) do
>     alglib.eigsubspaceoocgetrequestinfo(state, out RequestType, out M)
>     alglib.eigsubspaceoocgetrequestdata(state, out X)
>     [calculate  Y=A*X, with X=R^NxM]
>     alglib.eigsubspaceoocsendresult(state, in Y)
> alglib.eigsubspaceoocstop(state, out W, out Z, out Report)


  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
ae_bool eigsubspaceooccontinue(eigsubspacestate* state, ae_state *_state);


/*************************************************************************
This function is used to retrieve information  about  out-of-core  request
sent by solver to user code: request type (current version  of  the solver
sends only requests for matrix-matrix products) and request size (size  of
the matrices being multiplied).

This function returns just request metrics; in order  to  get contents  of
the matrices being multiplied, use eigsubspaceoocgetrequestdata().

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like below:

> alglib.eigsubspaceoocstart(state)
> while alglib.eigsubspaceooccontinue(state) do
>     alglib.eigsubspaceoocgetrequestinfo(state, out RequestType, out M)
>     alglib.eigsubspaceoocgetrequestdata(state, out X)
>     [calculate  Y=A*X, with X=R^NxM]
>     alglib.eigsubspaceoocsendresult(state, in Y)
> alglib.eigsubspaceoocstop(state, out W, out Z, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    
OUTPUT PARAMETERS:
    RequestType     -   type of the request to process:
                        * 0 - for matrix-matrix product A*X, with A  being
                          NxN matrix whose eigenvalues/vectors are needed,
                          and X being NxREQUESTSIZE one which is  returned
                          by the eigsubspaceoocgetrequestdata().
    RequestSize     -   size of the X matrix (number of columns),  usually
                        it is several times larger than number of  vectors
                        K requested by user.


  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspaceoocgetrequestinfo(eigsubspacestate* state,
     ae_int_t* requesttype,
     ae_int_t* requestsize,
     ae_state *_state);


/*************************************************************************
This function is used to retrieve information  about  out-of-core  request
sent by solver to user code: matrix X (array[N,RequestSize) which have  to
be multiplied by out-of-core matrix A in a product A*X.

This function returns just request data; in order to get size of  the data
prior to processing requestm, use eigsubspaceoocgetrequestinfo().

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like below:

> alglib.eigsubspaceoocstart(state)
> while alglib.eigsubspaceooccontinue(state) do
>     alglib.eigsubspaceoocgetrequestinfo(state, out RequestType, out M)
>     alglib.eigsubspaceoocgetrequestdata(state, out X)
>     [calculate  Y=A*X, with X=R^NxM]
>     alglib.eigsubspaceoocsendresult(state, in Y)
> alglib.eigsubspaceoocstop(state, out W, out Z, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    X               -   possibly  preallocated   storage;  reallocated  if
                        needed, left unchanged, if large enough  to  store
                        request data.
    
OUTPUT PARAMETERS:
    X               -   array[N,RequestSize] or larger, leading  rectangle
                        is filled with dense matrix X.


  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspaceoocgetrequestdata(eigsubspacestate* state,
     /* Real    */ ae_matrix* x,
     ae_state *_state);


/*************************************************************************
This function is used to send user reply to out-of-core  request  sent  by
solver. Usually it is product A*X for returned by solver matrix X.

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like below:

> alglib.eigsubspaceoocstart(state)
> while alglib.eigsubspaceooccontinue(state) do
>     alglib.eigsubspaceoocgetrequestinfo(state, out RequestType, out M)
>     alglib.eigsubspaceoocgetrequestdata(state, out X)
>     [calculate  Y=A*X, with X=R^NxM]
>     alglib.eigsubspaceoocsendresult(state, in Y)
> alglib.eigsubspaceoocstop(state, out W, out Z, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    AX              -   array[N,RequestSize] or larger, leading  rectangle
                        is filled with product A*X.


  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspaceoocsendresult(eigsubspacestate* state,
     /* Real    */ const ae_matrix* ax,
     ae_state *_state);


/*************************************************************************
This  function  finalizes out-of-core  mode  of  subspace eigensolver.  It
should be used in conjunction with other out-of-core-related functions  of
this subspackage in a loop like below:

> alglib.eigsubspaceoocstart(state)
> while alglib.eigsubspaceooccontinue(state) do
>     alglib.eigsubspaceoocgetrequestinfo(state, out RequestType, out M)
>     alglib.eigsubspaceoocgetrequestdata(state, out X)
>     [calculate  Y=A*X, with X=R^NxM]
>     alglib.eigsubspaceoocsendresult(state, in Y)
> alglib.eigsubspaceoocstop(state, out W, out Z, out Report)

INPUT PARAMETERS:
    State       -   solver state
    
OUTPUT PARAMETERS:
    W           -   array[K], depending on solver settings:
                    * top  K  eigenvalues ordered  by  descending   -   if
                      eigenvectors are returned in Z
                    * zeros - if invariant subspace is returned in Z
    Z           -   array[N,K], depending on solver settings either:
                    * matrix of eigenvectors found
                    * orthogonal basis of K-dimensional invariant subspace
    Rep         -   report with additional parameters

  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspaceoocstop(eigsubspacestate* state,
     /* Real    */ ae_vector* w,
     /* Real    */ ae_matrix* z,
     eigsubspacereport* rep,
     ae_state *_state);


/*************************************************************************
This  function runs subspace eigensolver for dense NxN symmetric matrix A,
given by its upper or lower triangle.

This function can not process nonsymmetric matrices.
  
INPUT PARAMETERS:
    State       -   solver state
    A           -   array[N,N], symmetric NxN matrix given by one  of  its
                    triangles
    IsUpper     -   whether upper or lower triangle of  A  is  given  (the
                    other one is not referenced at all).
    
OUTPUT PARAMETERS:
    W           -   array[K], top  K  eigenvalues ordered  by   descending
                    of their absolute values
    Z           -   array[N,K], matrix of eigenvectors found
    Rep         -   report with additional parameters
    
NOTE: internally this function allocates a copy of NxN dense A. You should
      take it into account when working with very large matrices occupying
      almost all RAM.

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
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspacesolvedenses(eigsubspacestate* state,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     /* Real    */ ae_vector* w,
     /* Real    */ ae_matrix* z,
     eigsubspacereport* rep,
     ae_state *_state);


/*************************************************************************
This  function runs eigensolver for dense NxN symmetric matrix A, given by
upper or lower triangle.

This function can not process nonsymmetric matrices.

INPUT PARAMETERS:
    State       -   solver state
    A           -   NxN symmetric matrix given by one of its triangles
    IsUpper     -   whether upper or lower triangle of  A  is  given  (the
                    other one is not referenced at all).
    
OUTPUT PARAMETERS:
    W           -   array[K], top  K  eigenvalues ordered  by   descending
                    of their absolute values
    Z           -   array[N,K], matrix of eigenvectors found
    Rep         -   report with additional parameters

  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
void eigsubspacesolvesparses(eigsubspacestate* state,
     const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ ae_vector* w,
     /* Real    */ ae_matrix* z,
     eigsubspacereport* rep,
     ae_state *_state);


/*************************************************************************
Internal r-comm function.

  -- ALGLIB --
     Copyright 16.01.2017 by Bochkanov Sergey
*************************************************************************/
ae_bool eigsubspaceiteration(eigsubspacestate* state, ae_state *_state);


/*************************************************************************
Finding the eigenvalues and eigenvectors of a symmetric matrix

The algorithm finds eigen pairs of a symmetric matrix by reducing it to
tridiagonal form and using the QL/QR algorithm.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

Input parameters:
    A       -   symmetric matrix which is given by its upper or lower
                triangular part.
                Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.
    IsUpper -   storage format.

Output parameters:
    D       -   eigenvalues in ascending order.
                Array whose index ranges within [0..N-1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn't changed;
                 * 1, Z contains the eigenvectors.
                Array whose indexes range within [0..N-1, 0..N-1].
                The eigenvectors are stored in the matrix columns.

Result:
    True, if the algorithm has converged.
    False, if the algorithm hasn't converged (rare case).

  -- ALGLIB --
     Copyright 2005-2008 by Bochkanov Sergey
*************************************************************************/
ae_bool smatrixevd(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_int_t zneeded,
     ae_bool isupper,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Subroutine for finding the eigenvalues (and eigenvectors) of  a  symmetric
matrix  in  a  given half open interval (A, B] by using  a  bisection  and
inverse iteration

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

Input parameters:
    A       -   symmetric matrix which is given by its upper or lower
                triangular part. Array [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.
    IsUpperA -  storage format of matrix A.
    B1, B2 -    half open interval (B1, B2] to search eigenvalues in.

Output parameters:
    M       -   number of eigenvalues found in a given half-interval (M>=0).
    W       -   array of the eigenvalues found.
                Array whose index ranges within [0..M-1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn't changed;
                 * 1, Z contains eigenvectors.
                Array whose indexes range within [0..N-1, 0..M-1].
                The eigenvectors are stored in the matrix columns.

Result:
    True, if successful. M contains the number of eigenvalues in the given
    half-interval (could be equal to 0), W contains the eigenvalues,
    Z contains the eigenvectors (if needed).

    False, if the bisection method subroutine wasn't able to find the
    eigenvalues in the given interval or if the inverse iteration subroutine
    wasn't able to find all the corresponding eigenvectors.
    In that case, the eigenvalues and eigenvectors are not returned,
    M is equal to 0.

  -- ALGLIB --
     Copyright 07.01.2006 by Bochkanov Sergey
*************************************************************************/
ae_bool smatrixevdr(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_int_t zneeded,
     ae_bool isupper,
     double b1,
     double b2,
     ae_int_t* m,
     /* Real    */ ae_vector* w,
     /* Real    */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Subroutine for finding the eigenvalues and  eigenvectors  of  a  symmetric
matrix with given indexes by using bisection and inverse iteration methods.

Input parameters:
    A       -   symmetric matrix which is given by its upper or lower
                triangular part. Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.
    IsUpperA -  storage format of matrix A.
    I1, I2 -    index interval for searching (from I1 to I2).
                0 <= I1 <= I2 <= N-1.

Output parameters:
    W       -   array of the eigenvalues found.
                Array whose index ranges within [0..I2-I1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn't changed;
                 * 1, Z contains eigenvectors.
                Array whose indexes range within [0..N-1, 0..I2-I1].
                In that case, the eigenvectors are stored in the matrix columns.

Result:
    True, if successful. W contains the eigenvalues, Z contains the
    eigenvectors (if needed).

    False, if the bisection method subroutine wasn't able to find the
    eigenvalues in the given interval or if the inverse iteration subroutine
    wasn't able to find all the corresponding eigenvectors.
    In that case, the eigenvalues and eigenvectors are not returned.

  -- ALGLIB --
     Copyright 07.01.2006 by Bochkanov Sergey
*************************************************************************/
ae_bool smatrixevdi(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_int_t zneeded,
     ae_bool isupper,
     ae_int_t i1,
     ae_int_t i2,
     /* Real    */ ae_vector* w,
     /* Real    */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Finding the eigenvalues and eigenvectors of a Hermitian matrix

The algorithm finds eigen pairs of a Hermitian matrix by  reducing  it  to
real tridiagonal form and using the QL/QR algorithm.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

Input parameters:
    A       -   Hermitian matrix which is given  by  its  upper  or  lower
                triangular part.
                Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    IsUpper -   storage format.
    ZNeeded -   flag controlling whether the eigenvectors  are  needed  or
                not. If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.

Output parameters:
    D       -   eigenvalues in ascending order.
                Array whose index ranges within [0..N-1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn't changed;
                 * 1, Z contains the eigenvectors.
                Array whose indexes range within [0..N-1, 0..N-1].
                The eigenvectors are stored in the matrix columns.

Result:
    True, if the algorithm has converged.
    False, if the algorithm hasn't converged (rare case).

Note:
    eigenvectors of Hermitian matrix are defined up to  multiplication  by
    a complex number L, such that |L|=1.

  -- ALGLIB --
     Copyright 2005, 23 March 2007 by Bochkanov Sergey
*************************************************************************/
ae_bool hmatrixevd(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     ae_int_t zneeded,
     ae_bool isupper,
     /* Real    */ ae_vector* d,
     /* Complex */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Subroutine for finding the eigenvalues (and eigenvectors) of  a  Hermitian
matrix  in  a  given half-interval (A, B] by using a bisection and inverse
iteration

Input parameters:
    A       -   Hermitian matrix which is given  by  its  upper  or  lower
                triangular  part.  Array  whose   indexes   range   within
                [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors  are  needed  or
                not. If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.
    IsUpperA -  storage format of matrix A.
    B1, B2 -    half-interval (B1, B2] to search eigenvalues in.

Output parameters:
    M       -   number of eigenvalues found in a given half-interval, M>=0
    W       -   array of the eigenvalues found.
                Array whose index ranges within [0..M-1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn't changed;
                 * 1, Z contains eigenvectors.
                Array whose indexes range within [0..N-1, 0..M-1].
                The eigenvectors are stored in the matrix columns.

Result:
    True, if successful. M contains the number of eigenvalues in the given
    half-interval (could be equal to 0), W contains the eigenvalues,
    Z contains the eigenvectors (if needed).

    False, if the bisection method subroutine  wasn't  able  to  find  the
    eigenvalues  in  the  given  interval  or  if  the  inverse  iteration
    subroutine  wasn't  able  to  find all the corresponding eigenvectors.
    In that case, the eigenvalues and eigenvectors are not returned, M  is
    equal to 0.

Note:
    eigen vectors of Hermitian matrix are defined up to multiplication  by
    a complex number L, such as |L|=1.

  -- ALGLIB --
     Copyright 07.01.2006, 24.03.2007 by Bochkanov Sergey.
*************************************************************************/
ae_bool hmatrixevdr(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     ae_int_t zneeded,
     ae_bool isupper,
     double b1,
     double b2,
     ae_int_t* m,
     /* Real    */ ae_vector* w,
     /* Complex */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Subroutine for finding the eigenvalues and  eigenvectors  of  a  Hermitian
matrix with given indexes by using bisection and inverse iteration methods

Input parameters:
    A       -   Hermitian matrix which is given  by  its  upper  or  lower
                triangular part.
                Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors  are  needed  or
                not. If ZNeeded is equal to:
                 * 0, the eigenvectors are not returned;
                 * 1, the eigenvectors are returned.
    IsUpperA -  storage format of matrix A.
    I1, I2 -    index interval for searching (from I1 to I2).
                0 <= I1 <= I2 <= N-1.

Output parameters:
    W       -   array of the eigenvalues found.
                Array whose index ranges within [0..I2-I1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn't changed;
                 * 1, Z contains eigenvectors.
                Array whose indexes range within [0..N-1, 0..I2-I1].
                In  that  case,  the eigenvectors are stored in the matrix
                columns.

Result:
    True, if successful. W contains the eigenvalues, Z contains the
    eigenvectors (if needed).

    False, if the bisection method subroutine  wasn't  able  to  find  the
    eigenvalues  in  the  given  interval  or  if  the  inverse  iteration
    subroutine wasn't able to find  all  the  corresponding  eigenvectors.
    In that case, the eigenvalues and eigenvectors are not returned.

Note:
    eigen vectors of Hermitian matrix are defined up to multiplication  by
    a complex number L, such as |L|=1.

  -- ALGLIB --
     Copyright 07.01.2006, 24.03.2007 by Bochkanov Sergey.
*************************************************************************/
ae_bool hmatrixevdi(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     ae_int_t zneeded,
     ae_bool isupper,
     ae_int_t i1,
     ae_int_t i2,
     /* Real    */ ae_vector* w,
     /* Complex */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Finding the eigenvalues and eigenvectors of a tridiagonal symmetric matrix

The algorithm finds the eigen pairs of a tridiagonal symmetric matrix by
using an QL/QR algorithm with implicit shifts.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

Input parameters:
    D       -   the main diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-1].
    E       -   the secondary diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-2].
    N       -   size of matrix A.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not needed;
                 * 1, the eigenvectors of a tridiagonal matrix
                   are multiplied by the square matrix Z. It is used if the
                   tridiagonal matrix is obtained by the similarity
                   transformation of a symmetric matrix;
                 * 2, the eigenvectors of a tridiagonal matrix replace the
                   square matrix Z;
                 * 3, matrix Z contains the first row of the eigenvectors
                   matrix.
    Z       -   if ZNeeded=1, Z contains the square matrix by which the
                eigenvectors are multiplied.
                Array whose indexes range within [0..N-1, 0..N-1].

Output parameters:
    D       -   eigenvalues in ascending order.
                Array whose index ranges within [0..N-1].
    Z       -   if ZNeeded is equal to:
                 * 0, Z hasn't changed;
                 * 1, Z contains the product of a given matrix (from the left)
                   and the eigenvectors matrix (from the right);
                 * 2, Z contains the eigenvectors.
                 * 3, Z contains the first row of the eigenvectors matrix.
                If ZNeeded<3, Z is the array whose indexes range within [0..N-1, 0..N-1].
                In that case, the eigenvectors are stored in the matrix columns.
                If ZNeeded=3, Z is the array whose indexes range within [0..0, 0..N-1].

Result:
    True, if the algorithm has converged.
    False, if the algorithm hasn't converged.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
ae_bool smatrixtdevd(/* Real    */ ae_vector* d,
     /* Real    */ const ae_vector* _e,
     ae_int_t n,
     ae_int_t zneeded,
     /* Real    */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Subroutine for finding the tridiagonal matrix eigenvalues/vectors in a
given half-interval (A, B] by using bisection and inverse iteration.

Input parameters:
    D       -   the main diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-1].
    E       -   the secondary diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-2].
    N       -   size of matrix, N>=0.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not needed;
                 * 1, the eigenvectors of a tridiagonal matrix are multiplied
                   by the square matrix Z. It is used if the tridiagonal
                   matrix is obtained by the similarity transformation
                   of a symmetric matrix.
                 * 2, the eigenvectors of a tridiagonal matrix replace matrix Z.
    A, B    -   half-interval (A, B] to search eigenvalues in.
    Z       -   if ZNeeded is equal to:
                 * 0, Z isn't used and remains unchanged;
                 * 1, Z contains the square matrix (array whose indexes range
                   within [0..N-1, 0..N-1]) which reduces the given symmetric
                   matrix to tridiagonal form;
                 * 2, Z isn't used (but changed on the exit).

Output parameters:
    D       -   array of the eigenvalues found.
                Array whose index ranges within [0..M-1].
    M       -   number of eigenvalues found in the given half-interval (M>=0).
    Z       -   if ZNeeded is equal to:
                 * 0, doesn't contain any information;
                 * 1, contains the product of a given NxN matrix Z (from the
                   left) and NxM matrix of the eigenvectors found (from the
                   right). Array whose indexes range within [0..N-1, 0..M-1].
                 * 2, contains the matrix of the eigenvectors found.
                   Array whose indexes range within [0..N-1, 0..M-1].

Result:

    True, if successful. In that case, M contains the number of eigenvalues
    in the given half-interval (could be equal to 0), D contains the eigenvalues,
    Z contains the eigenvectors (if needed).
    It should be noted that the subroutine changes the size of arrays D and Z.

    False, if the bisection method subroutine wasn't able to find the
    eigenvalues in the given interval or if the inverse iteration subroutine
    wasn't able to find all the corresponding eigenvectors. In that case,
    the eigenvalues and eigenvectors are not returned, M is equal to 0.

  -- ALGLIB --
     Copyright 31.03.2008 by Bochkanov Sergey
*************************************************************************/
ae_bool smatrixtdevdr(/* Real    */ ae_vector* d,
     /* Real    */ const ae_vector* e,
     ae_int_t n,
     ae_int_t zneeded,
     double a,
     double b,
     ae_int_t* m,
     /* Real    */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Subroutine for finding tridiagonal matrix eigenvalues/vectors with given
indexes (in ascending order) by using the bisection and inverse iteraion.

Input parameters:
    D       -   the main diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-1].
    E       -   the secondary diagonal of a tridiagonal matrix.
                Array whose index ranges within [0..N-2].
    N       -   size of matrix. N>=0.
    ZNeeded -   flag controlling whether the eigenvectors are needed or not.
                If ZNeeded is equal to:
                 * 0, the eigenvectors are not needed;
                 * 1, the eigenvectors of a tridiagonal matrix are multiplied
                   by the square matrix Z. It is used if the
                   tridiagonal matrix is obtained by the similarity transformation
                   of a symmetric matrix.
                 * 2, the eigenvectors of a tridiagonal matrix replace
                   matrix Z.
    I1, I2  -   index interval for searching (from I1 to I2).
                0 <= I1 <= I2 <= N-1.
    Z       -   if ZNeeded is equal to:
                 * 0, Z isn't used and remains unchanged;
                 * 1, Z contains the square matrix (array whose indexes range within [0..N-1, 0..N-1])
                   which reduces the given symmetric matrix to  tridiagonal form;
                 * 2, Z isn't used (but changed on the exit).

Output parameters:
    D       -   array of the eigenvalues found.
                Array whose index ranges within [0..I2-I1].
    Z       -   if ZNeeded is equal to:
                 * 0, doesn't contain any information;
                 * 1, contains the product of a given NxN matrix Z (from the left) and
                   Nx(I2-I1) matrix of the eigenvectors found (from the right).
                   Array whose indexes range within [0..N-1, 0..I2-I1].
                 * 2, contains the matrix of the eigenvalues found.
                   Array whose indexes range within [0..N-1, 0..I2-I1].


Result:

    True, if successful. In that case, D contains the eigenvalues,
    Z contains the eigenvectors (if needed).
    It should be noted that the subroutine changes the size of arrays D and Z.

    False, if the bisection method subroutine wasn't able to find the eigenvalues
    in the given interval or if the inverse iteration subroutine wasn't able
    to find all the corresponding eigenvectors. In that case, the eigenvalues
    and eigenvectors are not returned.

  -- ALGLIB --
     Copyright 25.12.2005 by Bochkanov Sergey
*************************************************************************/
ae_bool smatrixtdevdi(/* Real    */ ae_vector* d,
     /* Real    */ const ae_vector* e,
     ae_int_t n,
     ae_int_t zneeded,
     ae_int_t i1,
     ae_int_t i2,
     /* Real    */ ae_matrix* z,
     ae_state *_state);


/*************************************************************************
Finding eigenvalues and eigenvectors of a general (unsymmetric) matrix

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

The algorithm finds eigenvalues and eigenvectors of a general matrix by
using the QR algorithm with multiple shifts. The algorithm can find
eigenvalues and both left and right eigenvectors.

The right eigenvector is a vector x such that A*x = w*x, and the left
eigenvector is a vector y such that y'*A = w*y' (here y' implies a complex
conjugate transposition of vector y).

Input parameters:
    A       -   matrix. Array whose indexes range within [0..N-1, 0..N-1].
    N       -   size of matrix A.
    VNeeded -   flag controlling whether eigenvectors are needed or not.
                If VNeeded is equal to:
                 * 0, eigenvectors are not returned;
                 * 1, right eigenvectors are returned;
                 * 2, left eigenvectors are returned;
                 * 3, both left and right eigenvectors are returned.

Output parameters:
    WR      -   real parts of eigenvalues.
                Array whose index ranges within [0..N-1].
    WR      -   imaginary parts of eigenvalues.
                Array whose index ranges within [0..N-1].
    VL, VR  -   arrays of left and right eigenvectors (if they are needed).
                If WI[i]=0, the respective eigenvalue is a real number,
                and it corresponds to the column number I of matrices VL/VR.
                If WI[i]>0, we have a pair of complex conjugate numbers with
                positive and negative imaginary parts:
                    the first eigenvalue WR[i] + sqrt(-1)*WI[i];
                    the second eigenvalue WR[i+1] + sqrt(-1)*WI[i+1];
                    WI[i]>0
                    WI[i+1] = -WI[i] < 0
                In that case, the eigenvector  corresponding to the first
                eigenvalue is located in i and i+1 columns of matrices
                VL/VR (the column number i contains the real part, and the
                column number i+1 contains the imaginary part), and the vector
                corresponding to the second eigenvalue is a complex conjugate to
                the first vector.
                Arrays whose indexes range within [0..N-1, 0..N-1].

Result:
    True, if the algorithm has converged.
    False, if the algorithm has not converged.

Note 1:
    Some users may ask the following question: what if WI[N-1]>0?
    WI[N] must contain an eigenvalue which is complex conjugate to the
    N-th eigenvalue, but the array has only size N?
    The answer is as follows: such a situation cannot occur because the
    algorithm finds a pairs of eigenvalues, therefore, if WI[i]>0, I is
    strictly less than N-1.

Note 2:
    The algorithm performance depends on the value of the internal parameter
    NS of the InternalSchurDecomposition subroutine which defines the number
    of shifts in the QR algorithm (similarly to the block width in block-matrix
    algorithms of linear algebra). If you require maximum performance
    on your machine, it is recommended to adjust this parameter manually.


See also the InternalTREVC subroutine.

The algorithm is based on the LAPACK 3.0 library.
*************************************************************************/
ae_bool rmatrixevd(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_int_t vneeded,
     /* Real    */ ae_vector* wr,
     /* Real    */ ae_vector* wi,
     /* Real    */ ae_matrix* vl,
     /* Real    */ ae_matrix* vr,
     ae_state *_state);
void _eigsubspacestate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _eigsubspacestate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _eigsubspacestate_clear(void* _p);
void _eigsubspacestate_destroy(void* _p);
void _eigsubspacereport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _eigsubspacereport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _eigsubspacereport_clear(void* _p);
void _eigsubspacereport_destroy(void* _p);


/*$ End $*/
#endif

