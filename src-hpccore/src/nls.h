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

#ifndef _nls_h
#define _nls_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "optguardapi.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "creflections.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "matgen.h"
#include "scodes.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
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
#include "matinv.h"
#include "optserv.h"
#include "linmin.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "fbls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"
#include "vipmsolver.h"
#include "dfgenmod.h"


/*$ Declarations $*/


/*************************************************************************
Nonlinear least squares solver
*************************************************************************/
typedef struct
{
    ae_int_t protocolversion;
    ae_int_t n;
    ae_int_t m;
    ae_int_t problemtype;
    ae_int_t algorithm;
    double diffstep;
    nlpstoppingcriteria criteria;
    ae_bool xrep;
    ae_vector s;
    ae_vector x0;
    double rad0;
    ae_int_t maxfev;
    ae_int_t nnoisyrestarts;
    ae_int_t nnlc;
    ae_int_t cntlc;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector idxraw2red;
    ae_vector redbl;
    ae_vector redbu;
    ae_vector reds;
    ae_vector redx0;
    ae_bool userterminationneeded;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_int_t querysize;
    ae_int_t queryfuncs;
    ae_int_t queryvars;
    ae_int_t querydim;
    ae_int_t queryformulasize;
    ae_vector querydata;
    ae_vector replyfi;
    ae_vector replydj;
    sparsematrix replysj;
    ae_vector tmpx1;
    ae_vector tmpc1;
    ae_vector tmpf1;
    ae_vector tmpg1;
    ae_matrix tmpj1;
    rcommstate rstate;
    dfgmstate dfgmsolver;
    ae_bool dotrace;
    ae_vector xc;
    ae_int_t repnfunc;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repnrequests;
} nlsstate;


/*************************************************************************
Optimization report, filled by NLSResults() function

FIELDS:
* TerminationType, completion code, which is a sum of a BASIC code and  an
ADDITIONAL code.

  The following basic codes denote failure:
    * -8    optimizer detected NAN/INF  either  in  the  function  itself,
            or its Jacobian; recovery was impossible, abnormal termination
            reported.
    * -3    box constraints are inconsistent
  
  The following basic codes denote success:
    *  2    relative step is no more than EpsX.
    *  5    MaxIts steps was taken
    *  7    stopping conditions are too stringent,
            further improvement is impossible
    *  8    terminated   by  user  who  called  NLSRequestTermination().
            X contains point which was "current accepted" when termination
            request was submitted.
  
  Additional codes can be set on success, but not on failure:
    * +800  if   during   algorithm   execution   the   solver encountered
            NAN/INF values in the target or  constraints  but  managed  to
            recover by reducing trust region radius,  the  solver  returns
            one of SUCCESS codes but adds +800 to the code.
            
* IterationsCount, contains iterations count
* NFunc, number of function calculations
*************************************************************************/
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t terminationtype;
    ae_int_t nfunc;
} nlsreport;


/*$ Body $*/


/*************************************************************************
                DERIVATIVE-FREE NONLINEAR LEAST SQUARES

DESCRIPTION:

This function creates a NLS  solver  configured  to  solve  a  constrained
nonlinear least squares problem

    min F(x) = f[0]^2 + f[1]^2 + ... + f[m-1]^2

where f[i] are available, but not their derivatives.

The  functions  f[i]  are  assumed  to  be   smooth,  but  may  have  some
amount of numerical noise (either  random  noise  or  deterministic  noise
arising from numerical simulations or other complex numerical processes).

INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i], M>=1
    X       -   initial point, array[N]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlscreatedfo(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     nlsstate* state,
     ae_state *_state);


/*************************************************************************
This function sets the derivative-free NLS optimization algorithm  to  the
2PS (2-Point Stencil) algorithm.

This solver is recommended for the following cases:
* an expensive target function is minimized  by the commercial ALGLIB with
  callback  parallelism  activated  (see  ALGLIB Reference Manual for more
  information about parallel callbacks)
* an inexpensive target function is minimized by any ALGLIB edition  (free
  or commercial)

This function works only with solvers created with nlscreatedfo(), i.e. in
the derivative-free mode.

See the end of this comment for more information about the algorithm.

INPUT PARAMETERS:
    State           -   solver; must be created with nlscreatedfo() call -
                        passing  an  object   initialized   with   another
                        constructor function will result in an exception.
    NNoisyRestarts  -   number of restarts performed to combat a noise  in
                        the target. (see below, section 'RESTARTS', for  a
                        detailed discussion):
                        * 0     means that no restarts is performed, the
                                solver stops as soon as stopping  criteria
                                are met. Recommended for noise-free tasks.
                        * >0    means  that when the stopping criteria are
                                met, the solver will  perform  a  restart:
                                increase the trust   radius  and  resample
                                points. It often helps to  solve  problems
                                with random or deterministic noise.
                
ALGORITHM DESCRIPTION AND DISCUSSION

The 2PS algorithm is a derivative-free model-based nonlinear least squares
solver which builds local models by evaluating the target at  N additional
points around the current one, with geometry similar to the 2-point finite
difference stencil.

Similarly to the Levenberg-Marquardt algorithm, the solver shows quadratic
convergence despite the fact that it builds linear models.

When compared with the DFO-LSA solver, the 2PS algorithm has the following
distinctive properties:
* the 2PS algorithm performs more target function evaluations per iteration
  (at least N+1 instead of 1-2 usually performed by the DFO-LSA)
* 2PS requires several times less iterations than the DFO-LSA because each
  iteration extracts and utilizes more information about the  target. This
  difference tends to exaggerate when N increases
* contrary to that, DFO-LSA is much better at reuse of previously computed
  points. Thus, DFO-LSA needs several times less target  evaluations  than
  2PS, usually about 3-4 times less (this ratio seems to be more  or  less
  constant independently of N).

The summary is that:
* for  expensive  targets  2PS  provides better parallelism potential than
  DFO-LSA because the former issues many  simultaneous  target  evaluation
  requests which can be easily parallelized. It  is  possible  for  2PS to
  outperform DFO-LSA by parallelism  alone,  despite  the  fact  that  the
  latter needs 3-4 times less target function evaluations.
* for inexpensive targets 2PS may win  because  it  needs many  times less
  iterations, and thus the overhead associated with the working set updates
  is also many times less.
  
RESTARTS

Restarts is a strategy used to deal with random and deterministic noise in
the target/constraints.

Noise in the objective function can be random, arising from measurement or
simulation   uncertainty,  or  deterministic,   resulting   from   complex
underlying phenomena like numerical errors or branches in the target.  Its
influence is especially high at last stages of the optimization, when  all
computations are performed with small values of a trust radius.

Restarts allow the optimization algorithm to be robust against both  types
of noise by temporarily increasing a trust radius in order  to  capture  a
global structure of the target and avoid being trapped  by  noise-produced
local features.

A restart is usually performed when the stopping criteria  are  triggered.
Instead of stopping, the solver increases  trust  radius  to  its  initial
value and tries to rebuild a model.

If you decide to optimize with restarts,  it  is  recommended  to  perform
a small amount of restarts, up to 5. Generally, restarts do not allow  one
to completely solve the problem of noise, but still   it  is  possible  to
achieve some additional progress.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetalgo2ps(nlsstate* state,
     ae_int_t nnoisyrestarts,
     ae_state *_state);


/*************************************************************************
This function sets the derivative-free NLS optimization algorithm  to  the
DFO-LSA algorithm, an ALGLIB implementation (with several modifications)of
the original DFO-LS algorithm  by  Cartis, C., Fiala, J., Marteau, B.  and
Roberts, L. ('Improving the  Flexibility  and  Robustness  of  Model-Based
Derivative-Free Optimization Solvers', 2019). The A in DFO-LSA stands  for
ALGLIB, in order to distinguish our slightly modified implementation  from
the original algorithm.

This solver is recommended for the following  case:  an  expensive  target
function is minimized without parallelism being used (either  free  ALGLIB
is used or commercial one is used but the target callback is non-reentrant
i.e. it can not be simultaneously called from multiple threads)

This function works only with solvers created with nlscreatedfo(), i.e. in
the derivative-free mode.

See the end of this comment for more information about the algorithm.

INPUT PARAMETERS:
    State           -   solver; must be created with nlscreatedfo() call -
                        passing  an  object   initialized   with   another
                        constructor function will result in an exception.
    NNoisyRestarts  -   number of restarts performed to combat a noise  in
                        the target. (see below, section 'RESTARTS', for  a
                        detailed discussion):
                        * 0     means that no restarts is performed, the
                                solver stops as soon as stopping  criteria
                                are met. Recommended for noise-free tasks.
                        * >0    means  that when the stopping criteria are
                                met, the solver will  perform  a  restart:
                                increase the trust   radius  and  resample
                                points. It often helps to  solve  problems
                                with random or deterministic noise.

ALGORITHM DESCRIPTION AND DISCUSSION

The DFO-LSA algorithm is a derivative-free model-based  NLS  solver  which
builds local models by remembering N+1 previously computed  target  values
and updating them as optimization progresses.

Similarly to the Levenberg-Marquardt algorithm, the solver shows quadratic
convergence  despite  the  fact  that  it  builds   linear   models.   Our
implementation generally follows the same lines as the  original  DFO-LSA,
with several modifications to trust radius  update  strategies,  stability
fixes (unlike original DFO-LS, our implementation can handle  and  recover
from the target breaking down due to infeasible arguments) and other minor
implementation details.

When compared with the 2PS solver, the DFO-LSA algorithm has the following
distinctive properties:
* the 2PS algorithm performs more target function evaluations per iteration
  (at least N+1 instead of 1-2 usually performed by DFO-LSA)
* 2PS requires several times less iterations  than  DFO-LSA  because  each
  iterations extracts and utilizes more information about the target. This
  difference tends to exaggerate when N increases
* contrary to that, DFO-LSA is much better at reuse of previously computed
  points. Thus, DFO-LSA needs several times less target  evaluations  than
  2PS, usually about 3-4 times less (this ratio seems to be more  or  less
  constant independently of N).

The summary is that:
* for  expensive  targets DFO-LSA is much more efficient than 2PS  because
  it reuses previously computed target values as much as possible.
* however, DFO-LSA has little parallelism potential because  (unlike  2PS)
  it  does  not  evaluate the target  in several points simultaneously and
  independently
* additionally, because DFO-LSA performs many times more  iterations  than
  2PS, iteration overhead (working set updates and matrix  inversions)  is
  an issue here. For inexpensive targets it is possible for DFO-LSA to  be
  outperformed by 2PS merely because of the linear algebra cost.
  
RESTARTS

Restarts is a strategy used to deal with random and deterministic noise in
the target/constraints.

Noise in the objective function can be random, arising from measurement or
simulation   uncertainty,  or  deterministic,   resulting   from   complex
underlying phenomena like numerical errors or branches in the target.  Its
influence is especially high at last stages of the optimization, when  all
computations are performed with small values of a trust radius.

Restarts allow the optimization algorithm to be robust against both  types
of noise by temporarily increasing a trust radius in order  to  capture  a
global structure of the target and avoid being trapped  by  noise-produced
local features.

A restart is usually performed when the stopping criteria  are  triggered.
Instead of stopping, the solver increases  trust  radius  to  its  initial
value and tries to rebuild a model.

If you decide to optimize with restarts,  it  is  recommended  to  perform
a small amount of restarts, up to 5. Generally, restarts do not allow  one
to completely solve the problem of noise, but still   it  is  possible  to
achieve some additional progress.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetalgodfolsa(nlsstate* state,
     ae_int_t nnoisyrestarts,
     ae_state *_state);


/*************************************************************************
This function sets stopping conditions

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   stop when the scaled trust region radius is  smaller  than
                EpsX.
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.

Passing  EpsX=0  and  MaxIts=0  (simultaneously)  will  lead  to automatic
stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetcond(nlsstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to NLSOptimize().

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetxrep(nlsstate* state, ae_bool needxrep, ae_state *_state);


/*************************************************************************
This function sets variable scales

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Generally, scale is NOT considered to be a  form  of  preconditioner.  But
derivative-free optimizers often use scaling matrix both  in  the stopping
condition tests and as a preconditioner.

Proper scaling is very important for the algorithm performance. It is less
important for the quality of results, but still has some influence (it  is
easier  to  converge  when  variables  are  properly  scaled, so premature
stopping is possible when very badly scalled variables are  combined  with
relaxed stopping conditions).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetscale(nlsstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);


/*************************************************************************
This function sets box constraints

Box constraints are inactive by default (after initial creation). They are
preserved until explicitly turned off with another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (the latter is recommended).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (the latter is recommended).

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: unless  explicitly  mentioned  in  the   specific   NLS  algorithm
        description, the following holds:
        * box constraints are always satisfied exactly
        * the target is NOT evaluated outside of the box-constrained area

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlssetbc(nlsstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);


/*************************************************************************

CALLBACK PARALLELISM

The  NLS  optimizer  supports   parallel   model   evaluation   ('callback
parallelism').  This  feature,  which  is  present  in  commercial  ALGLIB
editions, greatly accelerates  optimization  when  using  a  solver  which
issues batch requests, i.e. multiple requests  for  target  values,  which
can be computed independently by different threads.

Callback parallelism is  usually   beneficial  when   processing  a  batch
request requires more than several  milliseconds.  It  also  requires  the
solver which issues requests in convenient batches, e.g. 2PS solver.

See ALGLIB Reference Manual, 'Working with commercial version' section for
more information.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool nlsiteration(nlsstate* state, ae_state *_state);


/*************************************************************************
Nonlinear least squares solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization  report;  includes  termination   codes   and
                additional information. Termination codes are returned  in
                rep.terminationtype field, its possible values are  listed
                below, see comments for this structure for more info.
                
                The termination code is a sum of a basic code (success  or
                failure)  and  one/several  additional  codes.  Additional
                codes are returned only for successful termination.
                
                The following basic codes can be returned:
                * -8    optimizer detected NAN/INF values in the target or
                        nonlinear constraints and failed to recover
                * -3    box constraints are inconsistent
                *  2    relative step is no more than EpsX.
                *  5    MaxIts steps was taken
                *  7    stopping conditions are too stringent,
                        further improvement is impossible
                *  8    terminated by user who called nlsrequesttermination().
                        X contains point which was "current accepted" when
                        termination request was submitted.
                        
                The following additional codes can be returned  (added  to
                a basic code):
                * +800      if   during  algorithm  execution  the  solver
                            encountered NAN/INF values in  the  target  or
                            constraints but managed to recover by reducing
                            trust region radius, the  solver  returns  one
                            of SUCCESS codes but adds +800 to the code.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
void nlsresults(const nlsstate* state,
     /* Real    */ ae_vector* x,
     nlsreport* rep,
     ae_state *_state);


/*************************************************************************
Buffered implementation of NLSResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void nlsresultsbuf(const nlsstate* state,
     /* Real    */ ae_vector* x,
     nlsreport* rep,
     ae_state *_state);


/*************************************************************************
This  subroutine  restarts solver from the new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   optimizer
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void nlsrestartfrom(nlsstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.
      
NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void nlsrequesttermination(nlsstate* state, ae_state *_state);


/*************************************************************************
Set V2 reverse communication protocol
*************************************************************************/
void nlssetprotocolv2(nlsstate* state, ae_state *_state);
void _nlsstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlsstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlsstate_clear(void* _p);
void _nlsstate_destroy(void* _p);
void _nlsreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nlsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nlsreport_clear(void* _p);
void _nlsreport_destroy(void* _p);


/*$ End $*/
#endif

