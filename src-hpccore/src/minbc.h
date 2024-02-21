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

#ifndef _minbc_h
#define _minbc_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "linmin.h"
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


/*$ Declarations $*/


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinBC subpackage to work with this
object
*************************************************************************/
typedef struct
{
    ae_int_t nmain;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    double diffstep;
    ae_vector s;
    ae_int_t prectype;
    ae_vector diagh;
    ae_int_t protocolversion;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    ae_bool userterminationneeded;
    rcommstate rstate;
    ae_vector xc;
    ae_vector ugc;
    ae_vector cgc;
    ae_vector xn;
    ae_vector ugn;
    ae_vector cgn;
    ae_vector xp;
    double fc;
    double fn;
    double fp;
    ae_vector d;
    double lastscaledgoodstep;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repvaridx;
    ae_int_t repterminationtype;
    ae_vector xstart;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double xm1;
    double xp1;
    double gm1;
    double gp1;
    ae_vector tmpprec;
    ae_vector tmp0;
    ae_int_t nfev;
    ae_int_t mcstage;
    double stp;
    double curstpmax;
    ae_vector work;
    linminstate lstate;
    double trimthreshold;
    ae_int_t nonmonotoniccnt;
    ae_matrix bufyk;
    ae_matrix bufsk;
    ae_vector bufrho;
    ae_vector buftheta;
    ae_int_t bufsize;
    double teststep;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
    ae_vector invs;
} minbcstate;


/*************************************************************************
This structure stores optimization report:
* iterationscount           number of iterations
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

TERMINATION CODES

terminationtype field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    inconsistent constraints.
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated by user who called minbcrequesttermination(). X contains
        point which was "current accepted" when  termination  request  was
        submitted.
*************************************************************************/
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t varidx;
    ae_int_t terminationtype;
} minbcreport;


/*$ Body $*/


/*************************************************************************
                     BOX CONSTRAINED OPTIMIZATION
          WITH FAST ACTIVATION OF MULTIPLE BOX CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x) of N arguments subject  to box
constraints (with some of box constraints actually being equality ones).

This optimizer uses algorithm similar to that of MinBLEIC (optimizer  with
general linear constraints), but presence of box-only  constraints  allows
us to use faster constraint activation strategies. On large-scale problems,
with multiple constraints active at the solution, this  optimizer  can  be
several times faster than BLEIC.

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBCCreate() call

2. USer adds box constraints by calling MinBCSetBC() function.

3. User sets stopping conditions with MinBCSetCond().

4. User calls MinBCOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBCResults() to get solution

6. Optionally user may call MinBCRestartFrom() to solve another problem
   with same N but another starting point.
   MinBCRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minbcstate* state,
     ae_state *_state);


/*************************************************************************
The subroutine is finite difference variant of MinBCCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBCCreate() in  order  to  get
more information about creation of BC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinBCSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust and precise. CG needs exact gradient values. Imprecise
   gradient may slow  down  convergence, especially  on  highly  nonlinear
   problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minbccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minbcstate* state,
     ae_state *_state);


/*************************************************************************
This function sets boundary constraints for BC optimizer.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBCRestartFrom().

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetbc(minbcstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);


/*************************************************************************
This function sets stopping conditions for the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinBCSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinBCSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection.

NOTE: when SetCond() called with non-zero MaxIts, BC solver may perform
      slightly more than MaxIts iterations. I.e., MaxIts  sets  non-strict
      limit on iterations count.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetcond(minbcstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);


/*************************************************************************
This function sets scaling coefficients for BC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  BC  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinBCSetPrec...()
functions.

There is a special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minbcsetscale(minbcstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecdefault(minbcstate* state, ae_state *_state);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE 1: D[i] should be positive. Exception will be thrown otherwise.

NOTE 2: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecdiag(minbcstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinBCSetScale()
call  (before  or after MinBCSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecscale(minbcstate* state, ae_state *_state);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinBCOptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetxrep(minbcstate* state, ae_bool needxrep, ae_state *_state);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  lead   to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetstpmax(minbcstate* state, double stpmax, ae_state *_state);


/*************************************************************************
NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either  MinBCCreate() for analytical gradient or  MinBCCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinBCOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinBCOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinBCOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinBCOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinBCCreateF()    |     works               FAILS
   MinBCCreate()     |     FAILS               works

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinBCOptimize()  version.   Attemps   to   use   such
   combination (for  example,  to  create optimizer with MinBCCreateF()
   and  to  pass  gradient  information  to  MinCGOptimize()) will lead to
   exception being thrown. Either  you  did  not pass gradient when it WAS
   needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool minbciteration(minbcstate* state, ae_state *_state);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minbcoptguardresults().

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
                    means of setting scale with minbcsetscale().

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
void minbcoptguardgradient(minbcstate* state,
     double teststep,
     ae_state *_state);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.
      
      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.
      
      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.
      
      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.
  
=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardsmoothness(minbcstate* state,
     ae_int_t level,
     ae_state *_state);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minbcoptguardgradient() for gradient verification
* minbcoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================
    
Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minbcoptguardnonc1test0results()
* minbcoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================
     
INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.
      
      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.
      
      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.
   
  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardresults(minbcstate* state,
     optguardreport* rep,
     ae_state *_state);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
  
==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================
     
INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report
    
  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardnonc1test0results(const minbcstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
  
==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================
     
INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report
    
  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardnonc1test1results(minbcstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);


/*************************************************************************
BC results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -8    internal integrity control  detected  infinite or
                        NAN   values   in   function/gradient.   Abnormal
                        termination signalled.
                * -3   inconsistent constraints.
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken
                *  8   terminated by user who called minbcrequesttermination().
                       X contains point which was "current accepted"  when
                       termination request was submitted.
                More information about fields of this  structure  can  be
                found in the comments on MinBCReport datatype.
   
  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcresults(const minbcstate* state,
     /* Real    */ ae_vector* x,
     minbcreport* rep,
     ae_state *_state);


/*************************************************************************
BC results

Buffered implementation of MinBCResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcresultsbuf(const minbcstate* state,
     /* Real    */ ae_vector* x,
     minbcreport* rep,
     ae_state *_state);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBCCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcrestartfrom(minbcstate* state,
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
void minbcrequesttermination(minbcstate* state, ae_state *_state);


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void minbcsetprotocolv1(minbcstate* state, ae_state *_state);
void _minbcstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbcstate_clear(void* _p);
void _minbcstate_destroy(void* _p);
void _minbcreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbcreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbcreport_clear(void* _p);
void _minbcreport_destroy(void* _p);


/*$ End $*/
#endif

