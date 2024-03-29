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

#ifndef _fbls_h
#define _fbls_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "rotations.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ortfac.h"


/*$ Declarations $*/


/*************************************************************************
Structure which stores state of linear CG solver between subsequent calls
of FBLSCgIteration(). Initialized with FBLSCGCreate().

USAGE:
1. call to FBLSCGCreate()
2. F:=FBLSCgIteration(State)
3. if F is False, iterations are over
4. otherwise, fill State.AX with A*x, State.XAX with x'*A*x
5. goto 2

If you want to rerminate iterations, pass zero or negative value to XAX.

FIELDS:
    E1      -   2-norm of residual at the start
    E2      -   2-norm of residual at the end
    X       -   on return from FBLSCgIteration() it contains vector for
                matrix-vector product
    AX      -   must be filled with A*x if FBLSCgIteration() returned True
    XAX     -   must be filled with x'*A*x
    XK      -   contains result (if FBLSCgIteration() returned False)
    
Other fields are private and should not be used by outsiders.
*************************************************************************/
typedef struct
{
    double e1;
    double e2;
    ae_vector x;
    ae_vector ax;
    double xax;
    ae_int_t n;
    ae_vector rk;
    ae_vector rk1;
    ae_vector xk;
    ae_vector xk1;
    ae_vector pk;
    ae_vector pk1;
    ae_vector b;
    rcommstate rstate;
    ae_vector tmp2;
} fblslincgstate;


/*************************************************************************
Structure which stores state of basic GMRES(k)  solver  between subsequent
calls of FBLSGMRESIteration(). Initialized with FBLSGMRESCreate().

USAGE:
1. call to FBLSCGCreate()
2. F:=FBLSGMRESIteration(State)
3. if F is False, iterations are over
4. otherwise, fill State.AX with A*x
5. goto 2

RCOMM FIELDS:
    X       -   on return from FBLSCgIteration() it contains vector for
                matrix-vector product
    AX      -   must be filled with A*x if FBLSCgIteration() returned True
    
RESULT
    XS      -   contains result (if FBLSCgIteration() returned False)
    State   -   following fields can be used:
                * ItsPerformed
                * RetCode
    
Other fields are private and should not be used by outsiders:
    Qi      -   rows store orthonormal basis of the Krylov subspace
    AQi     -   rows store products A*Qi
*************************************************************************/
typedef struct
{
    ae_vector b;
    ae_vector x;
    ae_vector ax;
    ae_vector xs;
    ae_matrix qi;
    ae_matrix aqi;
    ae_matrix h;
    ae_matrix hq;
    ae_matrix hr;
    ae_vector hqb;
    ae_vector ys;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_int_t n;
    ae_int_t itscnt;
    double epsort;
    double epsres;
    double epsred;
    double epsdiag;
    ae_int_t itsperformed;
    ae_int_t retcode;
    double reprelres;
    rcommstate rstate;
} fblsgmresstate;


/*$ Body $*/


/*************************************************************************
Basic Cholesky solver for ScaleA*Cholesky(A)'*x = y.

This subroutine assumes that:
* A*ScaleA is well scaled
* A is well-conditioned, so no zero divisions or overflow may occur

INPUT PARAMETERS:
    CHA     -   Cholesky decomposition of A
    SqrtScaleA- square root of scale factor ScaleA
    N       -   matrix size, N>=0.
    IsUpper -   storage type
    XB      -   right part
    Tmp     -   buffer; function automatically allocates it, if it is  too
                small.  It  can  be  reused  if function is called several
                times.
                
OUTPUT PARAMETERS:
    XB      -   solution

NOTE 1: no assertion or tests are done during algorithm operation
NOTE 2: N=0 will force algorithm to silently return

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void fblscholeskysolve(/* Real    */ const ae_matrix* cha,
     double sqrtscalea,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* xb,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);


/*************************************************************************
Fast basic linear solver: linear SPD CG

Solves (A^T*A + alpha*I)*x = b where:
* A is MxN matrix
* alpha>0 is a scalar
* I is NxN identity matrix
* b is Nx1 vector
* X is Nx1 unknown vector.

N iterations of linear conjugate gradient are used to solve problem.

INPUT PARAMETERS:
    A   -   array[M,N], matrix
    M   -   number of rows
    N   -   number of unknowns
    B   -   array[N], right part
    X   -   initial approxumation, array[N]
    Buf -   buffer; function automatically allocates it, if it is too
            small. It can be reused if function is called several times
            with same M and N.
            
OUTPUT PARAMETERS:
    X   -   improved solution
    
NOTES:
*   solver checks quality of improved solution. If (because of problem
    condition number, numerical noise, etc.) new solution is WORSE than
    original approximation, then original approximation is returned.
*   solver assumes that both A, B, Alpha are well scaled (i.e. they are
    less than sqrt(overflow) and greater than sqrt(underflow)).
    
  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void fblssolvecgx(/* Real    */ const ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* buf,
     ae_state *_state);


/*************************************************************************
Construction of linear conjugate gradient solver.

State parameter passed using "shared" semantics (i.e. previous state is NOT
erased). When it is already initialized, we can reause prevously allocated
memory.

INPUT PARAMETERS:
    X       -   initial solution
    B       -   right part
    N       -   system size
    State   -   structure; may be preallocated, if we want to reuse memory

OUTPUT PARAMETERS:
    State   -   structure which is used by FBLSCGIteration() to store
                algorithm state between subsequent calls.

NOTE: no error checking is done; caller must check all parameters, prevent
      overflows, and so on.

  -- ALGLIB --
     Copyright 22.10.2009 by Bochkanov Sergey
*************************************************************************/
void fblscgcreate(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     fblslincgstate* state,
     ae_state *_state);


/*************************************************************************
Linear CG solver, function relying on reverse communication to calculate
matrix-vector products.

See comments for FBLSLinCGState structure for more info.

  -- ALGLIB --
     Copyright 22.10.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool fblscgiteration(fblslincgstate* state, ae_state *_state);


/*************************************************************************
Construction of GMRES(k) solver.

State parameter passed using "shared" semantics (i.e. previous state is NOT
erased). When it is already initialized, we can reause prevously allocated
memory.

After (but not before!) initialization you can tweak following fields (they
are initialized by default values, but you can change it):
* State.EpsOrt - stop if norm of new candidate for orthogonalization is below EpsOrt
* State.EpsRes - stop of residual decreased below EpsRes*|B|
* State.EpsRed - stop if relative reduction of residual |R(k+1)|/|R(k)|>EpsRed

INPUT PARAMETERS:
    B       -   right part
    N       -   system size
    K       -   iterations count, K>=1
    State   -   structure; may be preallocated, if we want to reuse memory

OUTPUT PARAMETERS:
    State   -   structure which is used by FBLSGMRESIteration() to store
                algorithm state between subsequent calls.

NOTE: no error checking is done; caller must check all parameters, prevent
      overflows, and so on.

  -- ALGLIB --
     Copyright 18.11.2020 by Bochkanov Sergey
*************************************************************************/
void fblsgmrescreate(/* Real    */ const ae_vector* b,
     ae_int_t n,
     ae_int_t k,
     fblsgmresstate* state,
     ae_state *_state);


/*************************************************************************
Linear CG solver, function relying on reverse communication to calculate
matrix-vector products.

See comments for FBLSLinCGState structure for more info.

  -- ALGLIB --
     Copyright 22.10.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool fblsgmresiteration(fblsgmresstate* state, ae_state *_state);


/*************************************************************************
Fast  least  squares  solver,  solves  well  conditioned  system   without
performing  any  checks  for  degeneracy,  and using user-provided buffers
(which are automatically reallocated if too small).

This  function  is  intended  for solution of moderately sized systems. It
uses factorization algorithms based on Level 2 BLAS  operations,  thus  it
won't work efficiently on large scale systems.

INPUT PARAMETERS:
    A       -   array[M,N], system matrix.
                Contents of A is destroyed during solution.
    B       -   array[M], right part
    M       -   number of equations
    N       -   number of variables, N<=M
    Tmp0, Tmp1, Tmp2-
                buffers; function automatically allocates them, if they are
                too  small. They can  be  reused  if  function  is   called 
                several times.
                
OUTPUT PARAMETERS:
    B       -   solution (first N components, next M-N are zero)

  -- ALGLIB --
     Copyright 20.01.2012 by Bochkanov Sergey
*************************************************************************/
void fblssolvels(/* Real    */ ae_matrix* a,
     /* Real    */ ae_vector* b,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_vector* tmp0,
     /* Real    */ ae_vector* tmp1,
     /* Real    */ ae_vector* tmp2,
     ae_state *_state);
void _fblslincgstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _fblslincgstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _fblslincgstate_clear(void* _p);
void _fblslincgstate_destroy(void* _p);
void _fblsgmresstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _fblsgmresstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _fblsgmresstate_clear(void* _p);
void _fblsgmresstate_destroy(void* _p);


/*$ End $*/
#endif

