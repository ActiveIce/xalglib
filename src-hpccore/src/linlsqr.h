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

#ifndef _linlsqr_h
#define _linlsqr_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "creflections.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "matgen.h"
#include "scodes.h"
#include "tsort.h"
#include "sparse.h"
#include "normestimator.h"
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"


/*$ Declarations $*/


/*************************************************************************
This object stores state of the LinLSQR method.

You should use ALGLIB functions to work with this object.
*************************************************************************/
typedef struct
{
    normestimatorstate nes;
    ae_vector rx;
    ae_vector b;
    ae_int_t n;
    ae_int_t m;
    ae_int_t prectype;
    ae_vector ui;
    ae_vector uip1;
    ae_vector vi;
    ae_vector vip1;
    ae_vector omegai;
    ae_vector omegaip1;
    double alphai;
    double alphaip1;
    double betai;
    double betaip1;
    double phibari;
    double phibarip1;
    double phii;
    double rhobari;
    double rhobarip1;
    double rhoi;
    double ci;
    double si;
    double theta;
    double lambdai;
    ae_vector d;
    double anorm;
    double bnorm2;
    double dnorm;
    double r2;
    ae_vector x;
    ae_vector mv;
    ae_vector mtv;
    double epsa;
    double epsb;
    double epsc;
    ae_int_t maxits;
    ae_bool xrep;
    ae_bool xupdated;
    ae_bool needmv;
    ae_bool needmtv;
    ae_bool needmv2;
    ae_bool needvmv;
    ae_bool needprec;
    ae_int_t repiterationscount;
    ae_int_t repnmv;
    ae_int_t repterminationtype;
    ae_bool running;
    ae_bool userterminationneeded;
    ae_vector tmpd;
    ae_vector tmpx;
    rcommstate rstate;
} linlsqrstate;


typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nmv;
    ae_int_t terminationtype;
} linlsqrreport;


/*$ Body $*/


/*************************************************************************
This function initializes linear LSQR Solver. This solver is used to solve
non-symmetric (and, possibly, non-square) problems. Least squares solution
is returned for non-compatible systems.

USAGE:
1. User initializes algorithm state with LinLSQRCreate() call
2. User tunes solver parameters with  LinLSQRSetCond() and other functions
3. User  calls  LinLSQRSolveSparse()  function which takes algorithm state 
   and SparseMatrix object.
4. User calls LinLSQRResults() to get solution
5. Optionally, user may call LinLSQRSolveSparse() again to  solve  another  
   problem  with different matrix and/or right part without reinitializing 
   LinLSQRState structure.
  
INPUT PARAMETERS:
    M       -   number of rows in A
    N       -   number of variables, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state
    
NOTE: see also linlsqrcreatebuf()  for  version  which  reuses  previously
      allocated place as much as possible.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrcreate(ae_int_t m,
     ae_int_t n,
     linlsqrstate* state,
     ae_state *_state);


/*************************************************************************
This function initializes linear LSQR Solver.  It  provides  exactly  same
functionality as linlsqrcreate(), but reuses  previously  allocated  space
as much as possible.
  
INPUT PARAMETERS:
    M       -   number of rows in A
    N       -   number of variables, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2018 by Bochkanov Sergey
*************************************************************************/
void linlsqrcreatebuf(ae_int_t m,
     ae_int_t n,
     linlsqrstate* state,
     ae_state *_state);


/*************************************************************************
This function sets right part. By default, right part is zero.

INPUT PARAMETERS:
    B       -   right part, array[N].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetb(linlsqrstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state);


/*************************************************************************
This  function  changes  preconditioning  settings of LinLSQQSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecunit(linlsqrstate* state, ae_state *_state);


/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecdiag(linlsqrstate* state, ae_state *_state);


/*************************************************************************
This function sets optional Tikhonov regularization coefficient.
It is zero by default.

INPUT PARAMETERS:
    LambdaI -   regularization factor, LambdaI>=0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state
    
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetlambdai(linlsqrstate* state,
     double lambdai,
     ae_state *_state);


/*************************************************************************

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
ae_bool linlsqriteration(linlsqrstate* state, ae_state *_state);


/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse M*N matrix in the CRS format (you MUST contvert  it 
                to CRS format  by  calling  SparseConvertToCRS()  function
                BEFORE you pass it to this function).
    B       -   right part, array[M]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()
    
NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinLSQRSetPrecUnit(). However, preconditioning cost is   low
      and preconditioner is very important for solution  of  badly  scaled
      problems.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsolvesparse(linlsqrstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     ae_state *_state);


/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsA    -   algorithm will be stopped if ||A^T*Rk||/(||A||*||Rk||)<=EpsA.
    EpsB    -   algorithm will be stopped if ||Rk||<=EpsB*||B||
    MaxIts  -   algorithm will be stopped if number of iterations
                more than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE: if EpsA,EpsB,EpsC and MaxIts are zero then these variables will
be setted as default values.
    
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetcond(linlsqrstate* state,
     double epsa,
     double epsb,
     ae_int_t maxits,
     ae_state *_state);


/*************************************************************************
LSQR solver: results.

This function must be called after LinLSQRSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    *  1    ||Rk||<=EpsB*||B||
                    *  4    ||A^T*Rk||/(||A||*||Rk||)<=EpsA
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            X contains best point found so far.
                            (sometimes returned on singular systems)
                    *  8    user requested termination via calling
                            linlsqrrequesttermination()
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations
                
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrresults(const linlsqrstate* state,
     /* Real    */ ae_vector* x,
     linlsqrreport* rep,
     ae_state *_state);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetxrep(linlsqrstate* state,
     ae_bool needxrep,
     ae_state *_state);


/*************************************************************************
This function restarts LinLSQRIteration

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrrestart(linlsqrstate* state, ae_state *_state);


/*************************************************************************
This function is used to peek into LSQR solver and get  current  iteration
counter. You can safely "peek" into the solver from another thread.

INPUT PARAMETERS:
    S           -   solver object

RESULT:
    iteration counter, in [0,INF)

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
ae_int_t linlsqrpeekiterationscount(const linlsqrstate* s,
     ae_state *_state);


/*************************************************************************
This subroutine submits request for termination of the running solver.  It
can be called from some other thread which wants LSQR solver to  terminate
(obviously, the  thread  running  LSQR  solver can not request termination
because it is already busy working on LSQR).

As result, solver  stops  at  point  which  was  "current  accepted"  when
termination  request  was  submitted  and returns error code 8 (successful
termination).  Such   termination   is  a smooth  process  which  properly
deallocates all temporaries.

INPUT PARAMETERS:
    State   -   solver structure

NOTE: calling this function on solver which is NOT running  will  have  no
      effect.
      
NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

NOTE: solver clears termination flag on its start, it means that  if  some
      other thread will request termination too soon, its request will went
      unnoticed.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void linlsqrrequesttermination(linlsqrstate* state, ae_state *_state);
void _linlsqrstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _linlsqrstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _linlsqrstate_clear(void* _p);
void _linlsqrstate_destroy(void* _p);
void _linlsqrreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _linlsqrreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _linlsqrreport_clear(void* _p);
void _linlsqrreport_destroy(void* _p);


/*$ End $*/
#endif

