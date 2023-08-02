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

#ifndef _minnlc_h
#define _minnlc_h

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
#include "fbls.h"
#include "snnls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "sactivesets.h"
#include "minbleic.h"
#include "lpqppresolve.h"
#include "reviseddualsimplex.h"
#include "nlcslp.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "lpqpserv.h"
#include "vipmsolver.h"
#include "nlcsqp.h"


/*$ Declarations $*/


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinNLC subpackage to work  with  this
object
*************************************************************************/
typedef struct
{
    double stabilizingpoint;
    double initialinequalitymultiplier;
    ae_int_t solvertype;
    ae_int_t prectype;
    ae_int_t updatefreq;
    double rho;
    ae_int_t n;
    double epsx;
    ae_int_t maxits;
    ae_int_t aulitscnt;
    ae_bool xrep;
    double stpmax;
    double diffstep;
    double teststep;
    ae_vector s;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_int_t nec;
    ae_int_t nic;
    ae_matrix cleic;
    ae_vector lcsrcidx;
    ae_int_t ng;
    ae_int_t nh;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    rcommstate rstate;
    rcommstate rstateaul;
    rcommstate rstateslp;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_matrix scaledcleic;
    ae_vector xc;
    ae_vector xstart;
    ae_vector xbase;
    ae_vector fbase;
    ae_vector dfbase;
    ae_vector fm2;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fp2;
    ae_vector dfm1;
    ae_vector dfp1;
    ae_vector bufd;
    ae_vector bufc;
    ae_vector tmp0;
    ae_matrix bufw;
    ae_matrix bufz;
    ae_vector xk;
    ae_vector xk1;
    ae_vector gk;
    ae_vector gk1;
    double gammak;
    ae_bool xkpresent;
    minlbfgsstate auloptimizer;
    minlbfgsreport aulreport;
    ae_vector nubc;
    ae_vector nulc;
    ae_vector nunlc;
    ae_bool userterminationneeded;
    minslpstate slpsolverstate;
    minsqpstate sqpsolverstate;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_int_t repdbgphase0its;
} minnlcstate;


/*************************************************************************
These fields store optimization report:
* iterationscount           total number of inner iterations
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

Scaled constraint violations are reported:
* bcerr                     maximum violation of the box constraints
* bcidx                     index of the most violated box  constraint (or
                            -1, if all box constraints  are  satisfied  or
                            there is no box constraint)
* lcerr                     maximum violation of the  linear  constraints,
                            computed as maximum  scaled  distance  between
                            final point and constraint boundary.
* lcidx                     index of the most violated  linear  constraint
                            (or -1, if all constraints  are  satisfied  or
                            there is no general linear constraints)
* nlcerr                    maximum violation of the nonlinear constraints
* nlcidx                    index of the most violated nonlinear constraint
                            (or -1, if all constraints  are  satisfied  or
                            there is no nonlinear constraints)
                            
Violations of box constraints are scaled on per-component basis  according 
to  the  scale  vector s[] as specified by minnlcsetscale(). Violations of
the general linear  constraints  are  also  computed  using  user-supplied
variable scaling. Violations of nonlinear constraints are computed "as is"

TERMINATION CODES

TerminationType field contains completion code, which can be either:

=== FAILURE CODE ===
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signaled.
  -3    box  constraints  are  infeasible.  Note: infeasibility of non-box
        constraints does NOT trigger emergency  completion;  you  have  to
        examine  bcerr/lcerr/nlcerr   to  detect   possibly   inconsistent
        constraints.

=== SUCCESS CODE ===
   2    relative step is no more than EpsX.
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    user requested algorithm termination via minnlcrequesttermination(),
        last accepted point is returned
        
Other fields of this structure are not documented and should not be used!
*************************************************************************/
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    double bcerr;
    ae_int_t bcidx;
    double lcerr;
    ae_int_t lcidx;
    double nlcerr;
    ae_int_t nlcidx;
    ae_int_t dbgphase0its;
} minnlcreport;


/*$ Body $*/


/*************************************************************************
                  NONLINEARLY  CONSTRAINED  OPTIMIZATION

DESCRIPTION:
The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints
* nonlinear equality constraints Gi(x)=0
* nonlinear inequality constraints Hi(x)<=0

REQUIREMENTS:
* user must provide function value and gradient for F(), H(), G()
* starting point X0 must be feasible or not too far away from the feasible
  set
* F(), G(), H() are continuously differentiable on the  feasible  set  and
  its neighborhood
* nonlinear constraints G() and H() must have non-zero gradient at  G(x)=0
  and at H(x)=0. Say, constraint like x^2>=1 is supported, but x^2>=0   is
  NOT supported.

USAGE:

Constrained optimization if far more complex than the  unconstrained  one.
Nonlinearly constrained optimization is one of the most esoteric numerical
procedures.

Here we give very brief outline  of  the  MinNLC  optimizer.  We  strongly
recommend you to study examples in the ALGLIB Reference Manual and to read
ALGLIB User Guide on optimization, which is available at
http://www.alglib.net/optimization/

1. User initializes algorithm state with MinNLCCreate() call  and  chooses
   what NLC solver to use. There is some solver which is used by  default,
   with default settings, but you should NOT rely on  default  choice.  It
   may change in future releases of ALGLIB without notice, and no one  can
   guarantee that new solver will be  able  to  solve  your  problem  with
   default settings.
   
   From the other side, if you choose solver explicitly, you can be pretty
   sure that it will work with new ALGLIB releases.
   
   In the current release following solvers can be used:
   * SQP solver, recommended for medium-scale problems (less than thousand
     of variables) with hard-to-evaluate target functions.  Requires  less
     function  evaluations  than  other  solvers  but  each  step involves
     solution of QP subproblem, so running time may be higher than that of
     AUL (another recommended option). Activated  with  minnlcsetalgosqp()
     function.
   * AUL solver with dense  preconditioner,  recommended  for  large-scale
     problems or for problems  with  cheap  target  function.  Needs  more
     function evaluations that SQP (about  5x-10x  times  more),  but  its
     iterations  are  much  cheaper  that  that  of  SQP.  Activated  with
     minnlcsetalgoaul() function.
   * SLP solver, successive linear programming. The slowest one,  requires
     more target function evaluations that SQP and  AUL.  However,  it  is
     somewhat more robust in tricky cases, so it can be used  as  a backup
     plan. Activated with minnlcsetalgoslp() function.

2. [optional] user activates OptGuard  integrity checker  which  tries  to
   detect possible errors in the user-supplied callbacks:
   * discontinuity/nonsmoothness of the target/nonlinear constraints
   * errors in the analytic gradient provided by user
   This feature is essential for early prototyping stages because it helps
   to catch common coding and problem statement errors.
   OptGuard can be activated with following functions (one per each  check
   performed):
   * minnlcoptguardsmoothness()
   * minnlcoptguardgradient()

3. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) minnlcsetbc() for boundary constraints
   b) minnlcsetlc() for linear constraints
   c) minnlcsetnlc() for nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.
   
4. User sets scale of the variables with minnlcsetscale() function. It  is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.

5. User sets  stopping  conditions  with  minnlcsetcond(). If  NLC  solver
   uses  inner/outer  iteration  layout,  this  function   sets   stopping
   conditions for INNER iterations.
   
6. Finally, user calls minnlcoptimize()  function  which  takes  algorithm
   state and pointer (delegate, etc.) to callback function which calculates
   F/G/H.

7. User calls  minnlcresults()  to  get  solution;  additionally  you  can
   retrieve OptGuard report with minnlcoptguardresults(), and get detailed
   report about purported errors in the target function with:
   * minnlcoptguardnonc1test0results()
   * minnlcoptguardnonc1test1results()

8. Optionally user may call minnlcrestartfrom() to solve  another  problem
   with same N but another starting point. minnlcrestartfrom()  allows  to
   reuse already initialized structure.


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
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minnlcstate* state,
     ae_state *_state);


/*************************************************************************
This subroutine is a finite  difference variant of MinNLCCreate(). It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this  function
only. We recommend to read comments on MinNLCCreate() in order to get more
information about creation of NLC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinNLCSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large TRUNCATION  errors, while too small
   step will result in too large NUMERICAL  errors.  1.0E-4  can  be  good
   value to start from.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less   robust   and  precise.  Imprecise  gradient  may  slow  down
   convergence, especially on highly nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnlcstate* state,
     ae_state *_state);


/*************************************************************************
This function sets boundary constraints for NLC optimizer.

Boundary constraints are inactive by  default  (after  initial  creation).
They are preserved after algorithm restart with  MinNLCRestartFrom().

You may combine boundary constraints with  general  linear ones - and with
nonlinear ones! Boundary constraints are  handled  more  efficiently  than
other types.  Thus,  if  your  problem  has  mixed  constraints,  you  may
explicitly specify some of them as boundary and save some time/space.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1:  it is possible to specify  BndL[i]=BndU[i].  In  this  case  I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2:  when you solve your problem  with  augmented  Lagrangian  solver,
         boundary constraints are  satisfied  only  approximately!  It  is
         possible   that  algorithm  will  evaluate  function  outside  of
         feasible area!

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetbc(minnlcstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);


/*************************************************************************
This function sets linear constraints for MinNLC optimizer.

Linear constraints are inactive by default (after initial creation).  They
are preserved after algorithm restart with MinNLCRestartFrom().

You may combine linear constraints with boundary ones - and with nonlinear
ones! If your problem has mixed constraints, you  may  explicitly  specify
some of them as linear. It  may  help  optimizer   to   handle  them  more
efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
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

NOTE 1: when you solve your problem  with  augmented  Lagrangian   solver,
        linear constraints are  satisfied  only   approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        feasible area!

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetlc(minnlcstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);


/*************************************************************************
This function sets nonlinear constraints for MinNLC optimizer.

In fact, this function sets NUMBER of nonlinear  constraints.  Constraints
itself (constraint functions) are passed to MinNLCOptimize() method.  This
method requires user-defined vector function F[]  and  its  Jacobian  J[],
where:
* first component of F[] and first row  of  Jacobian  J[]  corresponds  to
  function being minimized
* next NLEC components of F[] (and rows  of  J)  correspond  to  nonlinear
  equality constraints G_i(x)=0
* next NLIC components of F[] (and rows  of  J)  correspond  to  nonlinear
  inequality constraints H_i(x)<=0

NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
      your problem has mixed constraints, you  may explicitly specify some
      of them as linear ones. It may help optimizer to  handle  them  more
      efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    NLEC    -   number of Non-Linear Equality Constraints (NLEC), >=0
    NLIC    -   number of Non-Linear Inquality Constraints (NLIC), >=0

NOTE 1: when you solve your problem  with  augmented  Lagrangian   solver,
        nonlinear constraints are satisfied only  approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        feasible area!
        
NOTE 2: algorithm scales variables  according  to   scale   specified   by
        MinNLCSetScale()  function,  so  it can handle problems with badly
        scaled variables (as long as we KNOW their scales).
           
        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints Gi(x) and Hi(x). Inappropriate scaling  of  Gi/Hi  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT same as solving it with constraint "0.001*G0(x)=0".
           
        It  means  that  YOU  are  the  one who is responsible for correct
        scaling of nonlinear constraints Gi(x) and Hi(x). We recommend you
        to scale nonlinear constraints in such way that I-th component  of
        dG/dX (or dH/dx) has approximately unit  magnitude  (for  problems
        with unit scale)  or  has  magnitude approximately equal to 1/S[i]
        (where S is a scale set by MinNLCSetScale() function).


  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetnlc(minnlcstate* state,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_state *_state);


/*************************************************************************
This function sets stopping conditions for inner iterations of  optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinNLCSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsX=0 and MaxIts=0 (simultaneously) will lead to automatic
selection of the stopping condition.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetcond(minnlcstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);


/*************************************************************************
This function sets scaling coefficients for NLC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetscale(minnlcstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);


/*************************************************************************
This function sets preconditioner to "inexact LBFGS-based" mode.

Preconditioning is very important for convergence of  Augmented Lagrangian
algorithm because presence of penalty term makes problem  ill-conditioned.
Difference between  performance  of  preconditioned  and  unpreconditioned
methods can be as large as 100x!

MinNLC optimizer may use following preconditioners,  each  with   its  own
benefits and drawbacks:
    a) inexact LBFGS-based, with O(N*K) evaluation time
    b) exact low rank one,  with O(N*K^2) evaluation time
    c) exact robust one,    with O(N^3+K*N^2) evaluation time
where K is a total number of general linear and nonlinear constraints (box
ones are not counted).

Inexact  LBFGS-based  preconditioner  uses L-BFGS  formula  combined  with
orthogonality assumption to perform very fast updates. For a N-dimensional
problem with K general linear or nonlinear constraints (boundary ones  are
not counted) it has O(N*K) cost per iteration.  This   preconditioner  has
best  quality  (less  iterations)  when   general   linear  and  nonlinear
constraints are orthogonal to each other (orthogonality  with  respect  to
boundary constraints is not required). Number of iterations increases when
constraints  are  non-orthogonal, because algorithm assumes orthogonality,
but still it is better than no preconditioner at all.

INPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 26.09.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetprecinexact(minnlcstate* state, ae_state *_state);


/*************************************************************************
This function sets preconditioner to "exact low rank" mode.

Preconditioning is very important for convergence of  Augmented Lagrangian
algorithm because presence of penalty term makes problem  ill-conditioned.
Difference between  performance  of  preconditioned  and  unpreconditioned
methods can be as large as 100x!

MinNLC optimizer may use following preconditioners,  each  with   its  own
benefits and drawbacks:
    a) inexact LBFGS-based, with O(N*K) evaluation time
    b) exact low rank one,  with O(N*K^2) evaluation time
    c) exact robust one,    with O(N^3+K*N^2) evaluation time
where K is a total number of general linear and nonlinear constraints (box
ones are not counted).

It also provides special unpreconditioned mode of operation which  can  be
used for test purposes. Comments below discuss low rank preconditioner.

Exact low-rank preconditioner  uses  Woodbury  matrix  identity  to  build
quadratic model of the penalized function. It has following features:
* no special assumptions about orthogonality of constraints
* preconditioner evaluation is optimized for K<<N. Its cost  is  O(N*K^2),
  so it may become prohibitively slow for K>=N.
* finally, stability of the process is guaranteed only for K<<N.  Woodbury
  update often fail for K>=N due to degeneracy of  intermediate  matrices.
  That's why we recommend to use "exact robust"  preconditioner  for  such
  cases.

RECOMMENDATIONS

We  recommend  to  choose  between  "exact  low  rank"  and "exact robust"
preconditioners, with "low rank" version being chosen  when  you  know  in
advance that total count of non-box constraints won't exceed N, and "robust"
version being chosen when you need bulletproof solution.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    UpdateFreq- update frequency. Preconditioner is  rebuilt  after  every
                UpdateFreq iterations. Recommended value: 10 or higher.
                Zero value means that good default value will be used.

  -- ALGLIB --
     Copyright 26.09.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetprecexactlowrank(minnlcstate* state,
     ae_int_t updatefreq,
     ae_state *_state);


/*************************************************************************
This function sets preconditioner to "exact robust" mode.

Preconditioning is very important for convergence of  Augmented Lagrangian
algorithm because presence of penalty term makes problem  ill-conditioned.
Difference between  performance  of  preconditioned  and  unpreconditioned
methods can be as large as 100x!

MinNLC optimizer may use following preconditioners,  each  with   its  own
benefits and drawbacks:
    a) inexact LBFGS-based, with O(N*K) evaluation time
    b) exact low rank one,  with O(N*K^2) evaluation time
    c) exact robust one,    with O(N^3+K*N^2) evaluation time
where K is a total number of general linear and nonlinear constraints (box
ones are not counted).

It also provides special unpreconditioned mode of operation which  can  be
used for test purposes. Comments below discuss robust preconditioner.

Exact  robust  preconditioner   uses   Cholesky  decomposition  to  invert
approximate Hessian matrix H=D+W'*C*W (where D stands for  diagonal  terms
of Hessian, combined result of initial scaling matrix and penalty from box
constraints; W stands for general linear constraints and linearization  of
nonlinear ones; C stands for diagonal matrix of penalty coefficients).

This preconditioner has following features:
* no special assumptions about constraint structure
* preconditioner is optimized  for  stability;  unlike  "exact  low  rank"
  version which fails for K>=N, this one works well for any value of K.
* the only drawback is that is takes O(N^3+K*N^2) time  to  build  it.  No
  economical  Woodbury update is applied even when it  makes  sense,  thus
  there  are  exist situations (K<<N) when "exact low rank" preconditioner
  outperforms this one.
  
RECOMMENDATIONS

We  recommend  to  choose  between  "exact  low  rank"  and "exact robust"
preconditioners, with "low rank" version being chosen  when  you  know  in
advance that total count of non-box constraints won't exceed N, and "robust"
version being chosen when you need bulletproof solution.
  
INPUT PARAMETERS:
    State   -   structure stores algorithm state
    UpdateFreq- update frequency. Preconditioner is  rebuilt  after  every
                UpdateFreq iterations. Recommended value: 10 or higher.
                Zero value means that good default value will be used.

  -- ALGLIB --
     Copyright 26.09.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetprecexactrobust(minnlcstate* state,
     ae_int_t updatefreq,
     ae_state *_state);


/*************************************************************************
This function sets preconditioner to "turned off" mode.

Preconditioning is very important for convergence of  Augmented Lagrangian
algorithm because presence of penalty term makes problem  ill-conditioned.
Difference between  performance  of  preconditioned  and  unpreconditioned
methods can be as large as 100x!

MinNLC optimizer may  utilize  two  preconditioners,  each  with  its  own
benefits and drawbacks: a) inexact LBFGS-based, and b) exact low rank one.
It also provides special unpreconditioned mode of operation which  can  be
used for test purposes.

This function activates this test mode. Do not use it in  production  code
to solve real-life problems.

INPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 26.09.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetprecnone(minnlcstate* state, ae_state *_state);


/*************************************************************************
This function sets maximum step length (after scaling of step vector  with
respect to variable scales specified by minnlcsetscale() call).

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0 (default),  if
                you don't want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: different solvers employed by MinNLC optimizer use  different  norms
      for step; AUL solver uses 2-norm, whilst SLP solver uses INF-norm.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcsetstpmax(minnlcstate* state, double stpmax, ae_state *_state);


/*************************************************************************
This  function  tells MinNLC unit to use  Augmented  Lagrangian  algorithm
for nonlinearly constrained  optimization.  This  algorithm  is  a  slight
modification of one described in "A Modified Barrier-Augmented  Lagrangian
Method for  Constrained  Minimization  (1999)"  by  D.GOLDFARB,  R.POLYAK,
K. SCHEINBERG, I.YUZEFOVICH.

AUL solver can be significantly faster than SQP on easy  problems  due  to
cheaper iterations, although it needs more function evaluations.

Augmented Lagrangian algorithm works by converting problem  of  minimizing
F(x) subject to equality/inequality constraints   to unconstrained problem
of the form

    min[ f(x) + 
        + Rho*PENALTY_EQ(x)   + SHIFT_EQ(x,Nu1) + 
        + Rho*PENALTY_INEQ(x) + SHIFT_INEQ(x,Nu2) ]
    
where:
* Rho is a fixed penalization coefficient
* PENALTY_EQ(x) is a penalty term, which is used to APPROXIMATELY  enforce
  equality constraints
* SHIFT_EQ(x) is a special "shift"  term  which  is  used  to  "fine-tune"
  equality constraints, greatly increasing precision
* PENALTY_INEQ(x) is a penalty term which is used to approximately enforce
  inequality constraints
* SHIFT_INEQ(x) is a special "shift"  term  which  is  used to "fine-tune"
  inequality constraints, greatly increasing precision
* Nu1/Nu2 are vectors of Lagrange coefficients which are fine-tuned during
  outer iterations of algorithm

This  version  of  AUL  algorithm  uses   preconditioner,  which   greatly
accelerates convergence. Because this  algorithm  is  similar  to  penalty
methods,  it  may  perform  steps  into  infeasible  area.  All  kinds  of
constraints (boundary, linear and nonlinear ones) may   be   violated   in
intermediate points - and in the solution.  However,  properly  configured
AUL method is significantly better at handling  constraints  than  barrier
and/or penalty methods.

The very basic outline of algorithm is given below:
1) first outer iteration is performed with "default"  values  of  Lagrange
   multipliers Nu1/Nu2. Solution quality is low (candidate  point  can  be
   too  far  away  from  true  solution; large violation of constraints is
   possible) and is comparable with that of penalty methods.
2) subsequent outer iterations  refine  Lagrange  multipliers  and improve
   quality of the solution.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Rho     -   penalty coefficient, Rho>0:
                * large enough  that  algorithm  converges  with   desired
                  precision. Minimum value is 10*max(S'*diag(H)*S),  where
                  S is a scale matrix (set by MinNLCSetScale) and H  is  a
                  Hessian of the function being minimized. If you can  not
                  easily estimate Hessian norm,  see  our  recommendations
                  below.
                * not TOO large to prevent ill-conditioning
                * for unit-scale problems (variables and Hessian have unit
                  magnitude), Rho=100 or Rho=1000 can be used.
                * it is important to note that Rho is internally multiplied
                  by scaling matrix, i.e. optimum value of Rho depends  on
                  scale of variables specified  by  MinNLCSetScale().
    ItsCnt  -   number of outer iterations:
                * ItsCnt=0 means that small number of outer iterations  is
                  automatically chosen (10 iterations in current version).
                * ItsCnt=1 means that AUL algorithm performs just as usual
                  barrier method.
                * ItsCnt>1 means that  AUL  algorithm  performs  specified
                  number of outer iterations
                
HOW TO CHOOSE PARAMETERS

Nonlinear optimization is a tricky area and Augmented Lagrangian algorithm
is sometimes hard to tune. Good values of  Rho  and  ItsCnt  are  problem-
specific.  In  order  to  help  you   we   prepared   following   set   of
recommendations:

* for  unit-scale  problems  (variables  and Hessian have unit magnitude),
  Rho=100 or Rho=1000 can be used.

* start from  some  small  value of Rho and solve problem  with  just  one
  outer iteration (ItcCnt=1). In this case algorithm behaves like  penalty
  method. Increase Rho in 2x or 10x steps until you  see  that  one  outer
  iteration returns point which is "rough approximation to solution".
  
  It is very important to have Rho so  large  that  penalty  term  becomes
  constraining i.e. modified function becomes highly convex in constrained
  directions.
  
  From the other side, too large Rho may prevent you  from  converging  to
  the solution. You can diagnose it by studying number of inner iterations
  performed by algorithm: too few (5-10 on  1000-dimensional  problem)  or
  too many (orders of magnitude more than  dimensionality)  usually  means
  that Rho is too large.

* with just one outer iteration you  usually  have  low-quality  solution.
  Some constraints can be violated with very  large  margin,  while  other
  ones (which are NOT violated in the true solution) can push final  point
  too far in the inner area of the feasible set.
  
  For example, if you have constraint x0>=0 and true solution  x0=1,  then
  merely a presence of "x0>=0" will introduce a bias towards larger values
  of x0. Say, algorithm may stop at x0=1.5 instead of 1.0.
  
* after you found good Rho, you may increase number of  outer  iterations.
  ItsCnt=10 is a good value. Subsequent outer iteration will refine values
  of  Lagrange  multipliers.  Constraints  which  were  violated  will  be
  enforced, inactive constraints will be dropped (corresponding multipliers
  will be decreased). Ideally, you  should  see  10-1000x  improvement  in
  constraint handling (constraint violation is reduced).
  
* if  you  see  that  algorithm  converges  to  vicinity  of solution, but
  additional outer iterations do not refine solution,  it  may  mean  that
  algorithm is unstable - it wanders around true  solution,  but  can  not
  approach it. Sometimes algorithm may be stabilized by increasing Rho one
  more time, making it 5x or 10x larger.

SCALING OF CONSTRAINTS [IMPORTANT]

AUL optimizer scales   variables   according   to   scale   specified   by
MinNLCSetScale() function, so it can handle  problems  with  badly  scaled
variables (as long as we KNOW their scales).   However,  because  function
being optimized is a mix  of  original  function and  constraint-dependent
penalty  functions, it  is   important  to   rescale  both  variables  AND
constraints.

Say,  if  you  minimize f(x)=x^2 subject to 1000000*x>=0,  then  you  have
constraint whose scale is different from that of target  function (another
example is 0.000001*x>=0). It is also possible to have constraints   whose
scales  are   misaligned:   1000000*x0>=0, 0.000001*x1<=0.   Inappropriate
scaling may ruin convergence because minimizing x^2 subject to x>=0 is NOT
same as minimizing it subject to 1000000*x>=0.

Because we  know  coefficients  of  boundary/linear  constraints,  we  can
automatically rescale and normalize them. However,  there  is  no  way  to
automatically rescale nonlinear constraints Gi(x) and  Hi(x)  -  they  are
black boxes.

It means that YOU are the one who is  responsible  for  correct scaling of
nonlinear constraints  Gi(x)  and  Hi(x).  We  recommend  you  to  rescale
nonlinear constraints in such way that I-th component of dG/dX (or  dH/dx)
has magnitude approximately equal to 1/S[i] (where S  is  a  scale  set by
MinNLCSetScale() function).

WHAT IF IT DOES NOT CONVERGE?

It is possible that AUL algorithm fails to converge to precise  values  of
Lagrange multipliers. It stops somewhere around true solution, but candidate
point is still too far from solution, and some constraints  are  violated.
Such kind of failure is specific for Lagrangian algorithms -  technically,
they stop at some point, but this point is not constrained solution.

There are exist several reasons why algorithm may fail to converge:
a) too loose stopping criteria for inner iteration
b) degenerate, redundant constraints
c) target function has unconstrained extremum exactly at the  boundary  of
   some constraint
d) numerical noise in the target function

In all these cases algorithm is unstable - each outer iteration results in
large and almost random step which improves handling of some  constraints,
but violates other ones (ideally  outer iterations should form a  sequence
of progressively decreasing steps towards solution).
   
First reason possible is  that  too  loose  stopping  criteria  for  inner
iteration were specified. Augmented Lagrangian algorithm solves a sequence
of intermediate problems, and requries each of them to be solved with high
precision. Insufficient precision results in incorrect update of  Lagrange
multipliers.

Another reason is that you may have specified degenerate constraints: say,
some constraint was repeated twice. In most cases AUL algorithm gracefully
handles such situations, but sometimes it may spend too much time figuring
out subtle degeneracies in constraint matrix.

Third reason is tricky and hard to diagnose. Consider situation  when  you
minimize  f=x^2  subject to constraint x>=0.  Unconstrained   extremum  is
located  exactly  at  the  boundary  of  constrained  area.  In  this case
algorithm will tend to oscillate between negative  and  positive  x.  Each
time it stops at x<0 it "reinforces" constraint x>=0, and each time it  is
bounced to x>0 it "relaxes" constraint (and is  attracted  to  x<0).

Such situation  sometimes  happens  in  problems  with  hidden  symetries.
Algorithm  is  got  caught  in  a  loop with  Lagrange  multipliers  being
continuously increased/decreased. Luckily, such loop forms after at  least
three iterations, so this problem can be solved by  DECREASING  number  of
outer iterations down to 1-2 and increasing  penalty  coefficient  Rho  as
much as possible.

Final reason is numerical noise. AUL algorithm is robust against  moderate
noise (more robust than, say, active set methods),  but  large  noise  may
destabilize algorithm.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgoaul(minnlcstate* state,
     double rho,
     ae_int_t itscnt,
     ae_state *_state);


/*************************************************************************
This   function  tells  MinNLC  optimizer  to  use  SLP (Successive Linear
Programming) algorithm for  nonlinearly  constrained   optimization.  This
algorithm  is  a  slight  modification  of  one  described  in  "A  Linear
programming-based optimization algorithm for solving nonlinear programming
problems" (2010) by Claus Still and Tapio Westerlund.

This solver is the slowest one in ALGLIB, it requires more target function
evaluations that SQP and AUL. However it is somewhat more robust in tricky
cases, so it can be used as a backup plan. We recommend to use  this  algo
when SQP/AUL do not work (does not return  the  solution  you  expect). If
trying different approach gives same  results,  then  MAYBE  something  is
wrong with your optimization problem.

Despite its name ("linear" = "first order method") this algorithm performs
steps similar to that of conjugate gradients method;  internally  it  uses
orthogonality/conjugacy requirement for subsequent steps  which  makes  it
closer to second order methods in terms of convergence speed.

Convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)
* extended Mangasarian–Fromovitz constraint qualification  (EMFCQ)  holds;
  in the context of this algorithm EMFCQ  means  that  one  can,  for  any
  infeasible  point,  find  a  search  direction  such that the constraint
  infeasibilities are reduced.

This algorithm has following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* initial point can be infeasible
* algorithm respects box constraints in all intermediate points  (it  does
  not even evaluate function outside of box constrained area)
* once linear constraints are enforced, algorithm will not violate them
* no such guarantees can be provided for nonlinear constraints,  but  once
  nonlinear constraints are enforced, algorithm will try  to  respect them
  as much as possible
* numerical differentiation does not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)
* from our experience, this algorithm is somewhat more  robust  in  really
  difficult cases

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

===== TRACING SLP SOLVER =================================================

SLP solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'SLP'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
                  It also prints OptGuard  integrity  checker  report when
                  nonsmoothness of target/constraints is suspected.
* 'SLP.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'SLP'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format
* 'SLP.PROBING' - to let algorithm insert additional function  evaluations
                  before line search  in  order  to  build  human-readable
                  chart of the raw  Lagrangian  (~40  additional  function
                  evaluations is performed for  each  line  search).  This
                  symbol also implicitly defines 'SLP'. Definition of this
                  symbol also automatically activates OptGuard  smoothness
                  monitor.
* 'OPTGUARD'    - for report of smoothness/continuity violations in target
                  and/or constraints. This kind of reporting is   included
                  in 'SLP', but it comes with lots of additional info.  If
                  you  need  just  smoothness  monitoring,   specify  this
                  setting.
                  
                  NOTE: this tag merely directs  OptGuard  output  to  log
                        file. Even if you specify it, you  still  have  to
                        configure OptGuard  by calling minnlcoptguard...()
                        family of functions.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related   overhead.  Specifying  'SLP.PROBING'  adds   even  larger
overhead due to additional function evaluations being performed.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("SLP,SLP.PROBING,PREC.F6", "path/to/trace.log")
> 

  -- ALGLIB --
     Copyright 02.04.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgoslp(minnlcstate* state, ae_state *_state);


/*************************************************************************
This   function  tells  MinNLC  optimizer to use SQP (Successive Quadratic
Programming) algorithm for nonlinearly constrained optimization.

This algorithm needs order of magnitude (5x-10x) less function evaluations
than AUL solver, but has higher overhead because each  iteration  involves
solution of quadratic programming problem.

Convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)

This algorithm has following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* initial point can be infeasible
* algorithm respects box constraints in all intermediate points  (it  does
  not even evaluate function outside of box constrained area)
* once linear constraints are enforced, algorithm will not violate them
* no such guarantees can be provided for nonlinear constraints,  but  once
  nonlinear constraints are enforced, algorithm will try  to  respect them
  as much as possible
* numerical differentiation does not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)

We recommend this algorithm as a default option for medium-scale  problems
(less than thousand of variables) or problems with target  function  being
hard to evaluate.

For   large-scale  problems  or  ones  with very  cheap  target   function
AUL solver can be better option.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    
===== INTERACTION WITH OPTGUARD ==========================================

OptGuard integrity  checker  allows us to catch problems  like  errors  in
gradients   and  discontinuity/nonsmoothness  of  the  target/constraints.
The latter kind of problems can be detected  by looking upon line searches
performed during optimization and searching for signs of nonsmoothness.

The problem with SQP is that it is too good for OptGuard to work - it does
not perform line searches. It typically  needs  1-2  function  evaluations
per step, and it is not enough for OptGuard to detect nonsmoothness.

So, if you suspect that your problem is  nonsmooth  and  if  you  want  to
confirm or deny it, we recommend you to either:
* use AUL or SLP solvers, which can detect nonsmoothness of the problem
* or, alternatively, activate 'SQP.PROBING' trace  tag  that  will  insert
  additional  function  evaluations (~40  per  line  step) that will  help
  OptGuard integrity checker to study properties of your problem

===== TRACING SQP SOLVER =================================================

SQP solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'SQP'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
                  It also prints OptGuard  integrity  checker  report when
                  nonsmoothness of target/constraints is suspected.
* 'SQP.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'SQP'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format
* 'SQP.PROBING' - to let algorithm insert additional function  evaluations
                  before line search  in  order  to  build  human-readable
                  chart of the raw  Lagrangian  (~40  additional  function
                  evaluations is performed for  each  line  search).  This
                  symbol  also  implicitly  defines  'SQP'  and  activates
                  OptGuard integrity checker which detects continuity  and
                  smoothness violations. An OptGuard log is printed at the
                  end of the file.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related   overhead.  Specifying  'SQP.PROBING'  adds   even  larger
overhead due to additional function evaluations being performed.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("SQP,SQP.PROBING,PREC.F6", "path/to/trace.log")
> 

  -- ALGLIB --
     Copyright 02.12.2019 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosqp(minnlcstate* state, ae_state *_state);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinNLCOptimize().

NOTE: algorithm passes two parameters to rep() callback  -  current  point
      and penalized function value at current point. Important -  function
      value which is returned is NOT function being minimized. It  is  sum
      of the value of the function being minimized - and penalty term.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcsetxrep(minnlcstate* state, ae_bool needxrep, ae_state *_state);


/*************************************************************************

NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied Jacobian, and one which uses  only  function
   vector and numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   you should choose appropriate variant of MinNLCOptimize() -  one  which
   accepts function AND Jacobian or one which accepts ONLY function.

   Be careful to choose variant of MinNLCOptimize()  which  corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinNLCOptimize()   and  specific
   function used to create optimizer.


                     |         USER PASSED TO MinNLCOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinNLCCreateF()   |     works               FAILS
   MinNLCCreate()    |     FAILS               works

   Here "FAILS" denotes inappropriate combinations  of  optimizer creation
   function  and  MinNLCOptimize()  version.   Attemps   to    use    such
   combination will lead to exception. Either  you  did  not pass gradient
   when it WAS needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
ae_bool minnlciteration(minnlcstate* state, ae_state *_state);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient/Jacobian.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target  function  (constraints)  at  the
initial point (note: future versions may also perform check  at  the final
point) and compares numerical gradient/Jacobian with analytic one provided
by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which stores both gradients/Jacobians, and specific components highlighted
as suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minnlcoptguardresults().

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
                    means of setting scale with minnlcsetscale().

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
void minnlcoptguardgradient(minnlcstate* state,
     double teststep,
     ae_state *_state);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0) and/or constraints
b) nonsmooth     target function (non-C1) and/or constraints

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
                      This kind of monitoring does not work well with  SQP
                      because SQP solver needs just 1-2 function evaluations
                      per step, which is not enough for OptGuard  to  make
                      any conclusions.
  
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
void minnlcoptguardsmoothness(minnlcstate* state,
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
* minnlcoptguardgradient() for gradient verification
* minnlcoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradfidx for specific function (Jacobian row) suspected
  * rep.badgradvidx for specific variable (Jacobian column) suspected
  * rep.badgradxbase, a point where gradient/Jacobian is tested
  * rep.badgraduser, user-provided gradient/Jacobian
  * rep.badgradnum, reference gradient/Jacobian obtained via numerical
    differentiation
* rep.nonc0suspected, and additionally:
  * rep.nonc0fidx - an index of specific function violating C0 continuity
* rep.nonc1suspected, and additionally
  * rep.nonc1fidx - an index of specific function violating C1 continuity
Here function index 0 means  target function, index 1  or  higher  denotes
nonlinear constraints.

=== ADDITIONAL REPORTS/LOGS ==============================================
    
Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minnlcoptguardnonc1test0results()
* minnlcoptguardnonc1test1results()
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
void minnlcoptguardresults(minnlcstate* state,
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
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
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
void minnlcoptguardnonc1test0results(const minnlcstate* state,
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
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
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
void minnlcoptguardnonc1test1results(minnlcstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);


/*************************************************************************
MinNLC results:  the  solution  found,  completion  codes  and  additional
information.

If you activated OptGuard integrity checking functionality and want to get
OptGuard report, it can be retrieved with:
* minnlcoptguardresults() - for a primary report about (a) suspected C0/C1
  continuity violations and (b) errors in the analytic gradient.
* minnlcoptguardnonc1test0results() - for C1 continuity violation test #0,
  detailed line search log
* minnlcoptguardnonc1test1results() - for C1 continuity violation test #1,
  detailed line search log

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report, contains information about completion
                code, constraint violation at the solution and so on.
                
                You   should   check   rep.terminationtype  in  order   to
                distinguish successful termination from unsuccessful one:
                
                === FAILURE CODES ===
                * -8    internal  integrity control  detected  infinite or
                        NAN   values    in   function/gradient.   Abnormal
                        termination signalled.
                * -3    box  constraints are infeasible.
                        Note: infeasibility of  non-box  constraints  does
                              NOT trigger emergency completion;  you  have
                              to examine rep.bcerr/rep.lcerr/rep.nlcerr to
                              detect possibly inconsistent constraints.
                              
                === SUCCESS CODES ===
                *  2   scaled step is no more than EpsX.
                *  5   MaxIts steps were taken.
                *  8   user   requested    algorithm    termination    via
                       minnlcrequesttermination(), last accepted point  is
                       returned.
                
                More information about fields of this  structure  can  be
                found in the comments on minnlcreport datatype.
   
  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcresults(const minnlcstate* state,
     /* Real    */ ae_vector* x,
     minnlcreport* rep,
     ae_state *_state);


/*************************************************************************
NLC results

Buffered implementation of MinNLCResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcresultsbuf(const minnlcstate* state,
     /* Real    */ ae_vector* x,
     minnlcreport* rep,
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
void minnlcrequesttermination(minnlcstate* state, ae_state *_state);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcrestartfrom(minnlcstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);


/*************************************************************************
Penalty function for equality constraints.
INPUT PARAMETERS:
    Alpha   -   function argument. Penalty function becomes large when
                Alpha approaches -1 or +1. It is defined for Alpha<=-1 or
                Alpha>=+1 - in this case infinite value is returned.
                
OUTPUT PARAMETERS:
    F       -   depending on Alpha:
                * for Alpha in (-1+eps,+1-eps), F=F(Alpha)
                * for Alpha outside of interval, F is some very large number
    DF      -   depending on Alpha:
                * for Alpha in (-1+eps,+1-eps), DF=dF(Alpha)/dAlpha, exact
                  numerical derivative.
                * otherwise, it is zero
    D2F     -   second derivative

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcequalitypenaltyfunction(double alpha,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state);


/*************************************************************************
"Penalty" function  for  inequality  constraints,  which  is multiplied by
penalty coefficient Rho.

"Penalty" function plays only supplementary role - it helps  to  stabilize
algorithm when solving non-convex problems. Because it  is  multiplied  by
fixed and large  Rho  -  not  Lagrange  multiplier  Nu  which  may  become
arbitrarily small! - it enforces  convexity  of  the  problem  behind  the
boundary of the feasible area.

This function is zero at the feasible area and in the close  neighborhood,
it becomes non-zero only at some distance (scaling is essential!) and grows
quadratically.

Penalty function must enter augmented Lagrangian as
    Rho*PENALTY(x-lowerbound)
with corresponding changes being made for upper bound or  other  kinds  of
constraints.

INPUT PARAMETERS:
    Alpha   -   function argument. Typically, if we have active constraint
                with precise Lagrange multiplier, we have Alpha  around 1.
                Large positive Alpha's correspond to  inner  area  of  the
                feasible set. Alpha<1 corresponds to  outer  area  of  the
                feasible set.
    StabilizingPoint- point where F becomes  non-zero.  Must  be  negative
                value, at least -1, large values (hundreds) are possible.
                
OUTPUT PARAMETERS:
    F       -   F(Alpha)
    DF      -   DF=dF(Alpha)/dAlpha, exact derivative
    D2F     -   second derivative
    
NOTE: it is important to  have  significantly  non-zero  StabilizingPoint,
      because when it  is  large,  shift  term  does  not  interfere  with
      Lagrange  multipliers  converging  to  their  final  values.   Thus,
      convergence of such modified AUL algorithm is  still  guaranteed  by
      same set of theorems.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcinequalitypenaltyfunction(double alpha,
     double stabilizingpoint,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state);


/*************************************************************************
"Shift" function  for  inequality  constraints,  which  is  multiplied  by
corresponding Lagrange multiplier.

"Shift" function is a main factor which enforces  inequality  constraints.
Inequality penalty function plays only supplementary role  -  it  prevents
accidental step deep into infeasible area  when  working  with  non-convex
problems (read comments on corresponding function for more information).

Shift function must enter augmented Lagrangian as
    Nu/Rho*SHIFT((x-lowerbound)*Rho+1)
with corresponding changes being made for upper bound or  other  kinds  of
constraints.

INPUT PARAMETERS:
    Alpha   -   function argument. Typically, if we have active constraint
                with precise Lagrange multiplier, we have Alpha  around 1.
                Large positive Alpha's correspond to  inner  area  of  the
                feasible set. Alpha<1 corresponds to  outer  area  of  the
                feasible set.
                
OUTPUT PARAMETERS:
    F       -   F(Alpha)
    DF      -   DF=dF(Alpha)/dAlpha, exact derivative
    D2F     -   second derivative

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcinequalityshiftfunction(double alpha,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state);
void _minnlcstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnlcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnlcstate_clear(void* _p);
void _minnlcstate_destroy(void* _p);
void _minnlcreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnlcreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnlcreport_clear(void* _p);
void _minnlcreport_destroy(void* _p);


/*$ End $*/
#endif

