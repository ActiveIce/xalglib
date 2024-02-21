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

#ifndef _minlm_h
#define _minlm_h

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
#include "xblas.h"
#include "directdensesolvers.h"
#include "normestimator.h"
#include "linlsqr.h"
#include "linmin.h"
#include "fbls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"
#include "snnls.h"
#include "sactivesets.h"
#include "qqpsolver.h"
#include "qpdenseaulsolver.h"
#include "minbleic.h"
#include "qpbleicsolver.h"
#include "vipmsolver.h"
#include "ipm2solver.h"
#include "minqp.h"


/*$ Declarations $*/


/*************************************************************************
Step finder for Levenberg-Marquardt optimizer.
Internal object used by MinLM unit.

This structure should be initialized with MinLMStepFinderInit(). Each step
search session should start  with  MinLMStepFinderStart()  call,  followed
by a sequence of MinLMStepFinderIteration() calls.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double stpmax;
    ae_int_t modelage;
    ae_int_t maxmodelage;
    double epsx;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_bool needf;
    ae_bool needfi;
    double fbase;
    ae_vector modeldiag;
    ae_vector xbase;
    ae_vector fibase;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector s;
    rcommstate rstate;
    ae_vector xdir;
    ae_vector choleskybuf;
    ae_vector tmp0;
    ae_vector tmpct;
    double actualdecrease;
    double predicteddecrease;
    minqpstate qpstate;
    minqpreport qprep;
    sparsematrix tmpsp;
} minlmstepfinder;


/*************************************************************************
Levenberg-Marquardt optimizer.

This structure should be created using one of the MinLMCreate???()
functions. You should not access its fields directly; use ALGLIB functions
to work with it.
*************************************************************************/
typedef struct
{
    ae_int_t protocolversion;
    ae_int_t n;
    ae_int_t m;
    double diffstep;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    ae_int_t maxmodelage;
    ae_bool makeadditers;
    ae_bool userterminationneeded;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_matrix h;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool needfgh;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
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
    ae_int_t algomode;
    ae_vector xbase;
    double fbase;
    ae_vector fibase;
    ae_vector gbase;
    ae_matrix quadraticmodel;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector s;
    ae_matrix cleic;
    ae_int_t nec;
    ae_int_t nic;
    double lambdav;
    double nu;
    ae_int_t modelage;
    ae_vector xnew;
    ae_vector xdir;
    ae_vector deltax;
    ae_vector deltaf;
    ae_bool deltaxready;
    ae_bool deltafready;
    smoothnessmonitor smonitor;
    double teststep;
    ae_vector lastscaleused;
    ae_int_t fstagnationcnt;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repnfunc;
    ae_int_t repnjac;
    ae_int_t repngrad;
    ae_int_t repnhess;
    ae_int_t repncholesky;
    rcommstate rstate;
    ae_vector choleskybuf;
    ae_vector tmp0;
    double actualdecrease;
    double predicteddecrease;
    double xm1;
    double xp1;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fc1;
    ae_vector gm1;
    ae_vector gp1;
    ae_vector gc1;
    minlbfgsstate internalstate;
    minlbfgsreport internalrep;
    minqpstate qpstate;
    minqpreport qprep;
    minlmstepfinder finderstate;
} minlmstate;


/*************************************************************************
Optimization report, filled by MinLMResults() function

FIELDS:
* TerminationType, completetion code:
    * -8    optimizer detected NAN/INF values either in the function itself,
            or in its Jacobian
    * -5    inappropriate solver was used:
            * solver created with minlmcreatefgh() used  on  problem  with
              general linear constraints (set with minlmsetlc() call).
    * -3    constraints are inconsistent
    *  2    relative step is no more than EpsX.
    *  5    MaxIts steps was taken
    *  7    stopping conditions are too stringent,
            further improvement is impossible
    *  8    terminated   by  user  who  called  MinLMRequestTermination().
            X contains point which was "current accepted" when termination
            request was submitted.
* IterationsCount, contains iterations count
* NFunc, number of function calculations
* NJac, number of Jacobi matrix calculations
* NGrad, number of gradient calculations
* NHess, number of Hessian calculations
* NCholesky, number of Cholesky decomposition calculations
*************************************************************************/
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t terminationtype;
    ae_int_t nfunc;
    ae_int_t njac;
    ae_int_t ngrad;
    ae_int_t nhess;
    ae_int_t ncholesky;
} minlmreport;


/*$ Body $*/


/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] and Jacobian of f[].


REQUIREMENTS:
This algorithm will request following information during its operation:

* function vector f[] at given point X
* function vector f[] and Jacobian of f[] (simultaneously) at given point

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec()  and jac() callbacks.
First  one  is used to calculate f[] at given point, second one calculates
f[] and Jacobian df[i]/dx[j].

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works  with  general  form function and does not provide Jacobian), but it
will  lead  to  exception  being  thrown  after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateVJ() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevj(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minlmstate* state,
     ae_state *_state);


/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] only. Finite differences  are  used  to
calculate Jacobian.


REQUIREMENTS:
This algorithm will request following information during its operation:
* function vector f[] at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec() callback.

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works with general form function and does not accept function vector), but
it will  lead  to  exception being thrown after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateV() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

See also MinLMIteration, MinLMResults.

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatev(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minlmstate* state,
     ae_state *_state);


/*************************************************************************
This function sets stopping conditions for Levenberg-Marquardt optimization
algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLMSetScale()
                Recommended values: 1E-9 ... 1E-12.
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.   Only   Levenberg-Marquardt
                iterations  are  counted  (L-BFGS/CG  iterations  are  NOT
                counted because their cost is very low compared to that of
                LM).

Passing  EpsX=0  and  MaxIts=0  (simultaneously)  will  lead  to automatic
stopping criterion selection (small EpsX).

NOTE: it is not recommended to set large EpsX (say, 0.001). Because LM  is
      a second-order method, it performs very precise steps anyway.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetcond(minlmstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinLMOptimize(). Both Levenberg-Marquardt and internal  L-BFGS
iterations are reported.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetxrep(minlmstate* state, ae_bool needxrep, ae_state *_state);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: non-zero StpMax leads to moderate  performance  degradation  because
intermediate  step  of  preconditioned L-BFGS optimization is incompatible
with limits on step size.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetstpmax(minlmstate* state, double stpmax, ae_state *_state);


/*************************************************************************
This function sets scaling coefficients for LM optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Generally, scale is NOT considered to be a form of preconditioner.  But LM
optimizer is unique in that it uses scaling matrix both  in  the  stopping
condition tests and as Marquardt damping factor.

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
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetscale(minlmstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);


/*************************************************************************
This function sets boundary constraints for LM optimizer

Boundary constraints are inactive by default (after initial creation).
They are preserved until explicitly turned off with another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by bound constraints
  or at its boundary

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetbc(minlmstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);


/*************************************************************************
This function sets general linear constraints for LM optimizer

Linear constraints are inactive by default (after initial creation).  They
are preserved until explicitly turned off with another minlmsetlc() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

IMPORTANT: if you have linear constraints, it is strongly  recommended  to
           set scale of variables with minlmsetscale(). QP solver which is
           used to calculate linearly constrained steps heavily relies  on
           good scaling of input problems.

IMPORTANT: solvers created with minlmcreatefgh()  do  not  support  linear
           constraints.
           
NOTE: linear  (non-bound)  constraints are satisfied only approximately  -
      there  always  exists some violation due  to  numerical  errors  and
      algorithmic limitations.

NOTE: general linear constraints  add  significant  overhead  to  solution
      process. Although solver performs roughly same amount of  iterations
      (when compared  with  similar  box-only  constrained  problem), each
      iteration   now    involves  solution  of  linearly  constrained  QP
      subproblem, which requires ~3-5 times more Cholesky  decompositions.
      Thus, if you can reformulate your problem in such way  this  it  has
      only box constraints, it may be beneficial to do so.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetlc(minlmstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);


/*************************************************************************
This function is used to change acceleration settings

You can choose between three acceleration strategies:
* AccType=0, no acceleration.
* AccType=1, secant updates are used to update quadratic model after  each
  iteration. After fixed number of iterations (or after  model  breakdown)
  we  recalculate  quadratic  model  using  analytic  Jacobian  or  finite
  differences. Number of secant-based iterations depends  on  optimization
  settings: about 3 iterations - when we have analytic Jacobian, up to 2*N
  iterations - when we use finite differences to calculate Jacobian.

AccType=1 is recommended when Jacobian  calculation  cost is prohibitively
high (several Mx1 function vector calculations  followed  by  several  NxN
Cholesky factorizations are faster than calculation of one M*N  Jacobian).
It should also be used when we have no Jacobian, because finite difference
approximation takes too much time to compute.

Table below list  optimization  protocols  (XYZ  protocol  corresponds  to
MinLMCreateXYZ) and acceleration types they support (and use by  default).

ACCELERATION TYPES SUPPORTED BY OPTIMIZATION PROTOCOLS:

protocol    0   1   comment
V           +   +
VJ          +   +
FGH         +

DEFAULT VALUES:

protocol    0   1   comment
V               x   without acceleration it is so slooooooooow
VJ          x
FGH         x

NOTE: this  function should be called before optimization. Attempt to call
it during algorithm iterations may result in unexpected behavior.

NOTE: attempt to call this function with unsupported protocol/acceleration
combination will result in exception being thrown.

  -- ALGLIB --
     Copyright 14.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetacctype(minlmstate* state,
     ae_int_t acctype,
     ae_state *_state);


/*************************************************************************

CALLBACK PARALLELISM

The MINLM optimizer supports parallel parallel  numerical  differentiation
('callback parallelism'). This feature, which  is  present  in  commercial
ALGLIB  editions,  greatly   accelerates   optimization   with   numerical
differentiation of an expensive target functions.

Callback parallelism is usually  beneficial  when  computing  a  numerical
gradient requires more than several  milliseconds.  In this case  the  job
of computing individual gradient components can be split between  multiple
threads. Even inexpensive targets can benefit  from  parallelism,  if  you
have many variables.

If you solve a curve fitting problem, i.e. the function vector is actually
the same function computed at different points of a data points space,  it
may  be  better  to  use  an LSFIT curve fitting solver, which offers more
fine-grained parallelism due to knowledge of  the  problem  structure.  In
particular, it can accelerate both numerical differentiation  and problems
with user-supplied gradients.

ALGLIB Reference Manual, 'Working with commercial  version' section, tells
how to activate callback parallelism for your programming language.

  -- ALGLIB --
     Copyright 03.12.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool minlmiteration(minlmstate* state, ae_state *_state);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic Jacobian.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function vector  at  the  initial
point (note: future versions may also perform check  at  the final  point)
and compares numerical Jacobian with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which stores  both  Jacobians,  and  specific  components  highlighted  as
suspicious by the OptGuard.

The OptGuard report can be retrieved with minlmoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.
           
           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.
      
INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.
                    
                    You may use different step for different parameters by
                    means of setting scale with minlmsetscale().

=== EXPLANATION ==========================================================                    

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minlmoptguardgradient(minlmstate* state,
     double teststep,
     ae_state *_state);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

OptGuard checks analytic Jacobian  against  reference  value  obtained  by
numerical differentiation with user-specified step.

NOTE: other optimizers perform additional OptGuard checks for things  like
      C0/C1-continuity violations. However, LM optimizer  can  check  only
      for incorrect Jacobian.
      
      The reason is that unlike line search methods LM optimizer does  not
      perform extensive evaluations along the line. Thus, we simply do not
      have enough data to catch C0/C1-violations.

This check is activated with  minlmoptguardgradient() function.

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradfidx for specific function (Jacobian row) suspected
  * rep.badgradvidx for specific variable (Jacobian column) suspected
  * rep.badgradxbase, a point where gradient/Jacobian is tested
  * rep.badgraduser, user-provided gradient/Jacobian
  * rep.badgradnum, reference gradient/Jacobian obtained via numerical
    differentiation

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   OptGuard report
   
  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlmoptguardresults(minlmstate* state,
     optguardreport* rep,
     ae_state *_state);


/*************************************************************************
Levenberg-Marquardt algorithm results

NOTE: if you activated OptGuard integrity checking functionality and  want
      to get OptGuard report,  it  can  be  retrieved  with  the  help  of
      minlmoptguardresults() function.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization  report;  includes  termination   codes   and
                additional information. Termination codes are listed below,
                see comments for this structure for more info.
                Termination code is stored in rep.terminationtype field:
                * -8    optimizer detected NAN/INF values either in the
                        function itself, or in its Jacobian
                * -3    constraints are inconsistent
                *  2    relative step is no more than EpsX.
                *  5    MaxIts steps was taken
                *  7    stopping conditions are too stringent,
                        further improvement is impossible
                *  8    terminated by user who called minlmrequesttermination().
                        X contains point which was "current accepted" when
                        termination request was submitted.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresults(const minlmstate* state,
     /* Real    */ ae_vector* x,
     minlmreport* rep,
     ae_state *_state);


/*************************************************************************
Levenberg-Marquardt algorithm results

Buffered implementation of MinLMResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresultsbuf(const minlmstate* state,
     /* Real    */ ae_vector* x,
     minlmreport* rep,
     ae_state *_state);


/*************************************************************************
This  subroutine  restarts  LM  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinLMCreateXXX call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlmrestartfrom(minlmstate* state,
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
void minlmrequesttermination(minlmstate* state, ae_state *_state);


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void minlmsetprotocolv1(minlmstate* state, ae_state *_state);


/*************************************************************************
Set V2 reverse communication protocol
*************************************************************************/
void minlmsetprotocolv2(minlmstate* state, ae_state *_state);


/*************************************************************************
Unpacks dense Jacobian into array
*************************************************************************/
void unpackdj(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_vector* replydj,
     /* Real    */ ae_matrix* jac,
     ae_state *_state);
void _minlmstepfinder_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmstepfinder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmstepfinder_clear(void* _p);
void _minlmstepfinder_destroy(void* _p);
void _minlmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmstate_clear(void* _p);
void _minlmstate_destroy(void* _p);
void _minlmreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmreport_clear(void* _p);
void _minlmreport_destroy(void* _p);


/*$ End $*/
#endif

