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


#include <stdafx.h>
#include "minnlc.h"


/*$ Declarations $*/
static double minnlc_aulmaxgrowth = 10.0;
static double minnlc_maxlagmult = 1.0E7;
static ae_int_t minnlc_lbfgsfactor = 10;
static double minnlc_hessesttol = 1.0E-6;
static double minnlc_initgamma = 1.0E-6;
static double minnlc_regprec = 1.0E-6;
static void minnlc_clearrequestfields(minnlcstate* state,
     ae_state *_state);
static void minnlc_minnlcinitinternal(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnlcstate* state,
     ae_state *_state);
static void minnlc_clearpreconditioner(minlbfgsstate* auloptimizer,
     ae_state *_state);
static void minnlc_updatepreconditioner(ae_int_t prectype,
     ae_int_t updatefreq,
     ae_int_t* preccounter,
     minlbfgsstate* auloptimizer,
     /* Real    */ const ae_vector* x,
     double rho,
     double gammak,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* hasbndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ const ae_vector* nubc,
     /* Real    */ const ae_matrix* cleic,
     /* Real    */ const ae_vector* nulc,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     /* Real    */ const ae_vector* nunlc,
     /* Real    */ ae_vector* bufd,
     /* Real    */ ae_vector* bufc,
     /* Real    */ ae_matrix* bufw,
     /* Real    */ ae_matrix* bufz,
     /* Real    */ ae_vector* tmp0,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t ng,
     ae_int_t nh,
     ae_state *_state);
static void minnlc_penaltybc(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* hasbndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ const ae_vector* nubc,
     ae_int_t n,
     double rho,
     double stabilizingpoint,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state);
static void minnlc_penaltylc(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* cleic,
     /* Real    */ const ae_vector* nulc,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     double rho,
     double stabilizingpoint,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state);
static void minnlc_penaltynlc(/* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* j,
     /* Real    */ const ae_vector* nunlc,
     ae_int_t n,
     ae_int_t ng,
     ae_int_t nh,
     double rho,
     double stabilizingpoint,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state);
static ae_bool minnlc_auliteration(minnlcstate* state,
     smoothnessmonitor* smonitor,
     ae_state *_state);
static void minnlc_unscale(const minnlcstate* state,
     /* Real    */ const ae_vector* xs,
     /* Real    */ const ae_vector* scaledbndl,
     /* Real    */ const ae_vector* scaledbndu,
     /* Real    */ ae_vector* xu,
     ae_state *_state);


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
     ae_state *_state)
{

    _minnlcstate_clear(state);

    ae_assert(n>=1, "MinNLCCreate: N<1", _state);
    ae_assert(x->cnt>=n, "MinNLCCreate: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinNLCCreate: X contains infinite or NaN values", _state);
    minnlc_minnlcinitinternal(n, x, 0.0, state, _state);
}


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
     ae_state *_state)
{

    _minnlcstate_clear(state);

    ae_assert(n>=1, "MinNLCCreateF: N<1", _state);
    ae_assert(x->cnt>=n, "MinNLCCreateF: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinNLCCreateF: X contains infinite or NaN values", _state);
    ae_assert(ae_isfinite(diffstep, _state), "MinNLCCreateF: DiffStep is infinite or NaN!", _state);
    ae_assert(ae_fp_greater(diffstep,(double)(0)), "MinNLCCreateF: DiffStep is non-positive!", _state);
    minnlc_minnlcinitinternal(n, x, diffstep, state, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "MinNLCSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinNLCSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinNLCSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinNLCSetBC: BndL contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;


    n = state->n;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(k>=0, "MinNLCSetLC: K<0", _state);
    ae_assert(c->cols>=n+1||k==0, "MinNLCSetLC: Cols(C)<N+1", _state);
    ae_assert(c->rows>=k, "MinNLCSetLC: Rows(C)<K", _state);
    ae_assert(ct->cnt>=k, "MinNLCSetLC: Length(CT)<K", _state);
    ae_assert(apservisfinitematrix(c, k, n+1, _state), "MinNLCSetLC: C contains infinite or NaN values!", _state);
    
    /*
     * Handle zero K
     */
    if( k==0 )
    {
        state->nec = 0;
        state->nic = 0;
        return;
    }
    
    /*
     * Equality constraints are stored first, in the upper
     * NEC rows of State.CLEIC matrix. Inequality constraints
     * are stored in the next NIC rows.
     *
     * NOTE: we convert inequality constraints to the form
     * A*x<=b before copying them.
     */
    rmatrixsetlengthatleast(&state->cleic, k, n+1, _state);
    ivectorsetlengthatleast(&state->lcsrcidx, k, _state);
    state->nec = 0;
    state->nic = 0;
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]==0 )
        {
            ae_v_move(&state->cleic.ptr.pp_double[state->nec][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            state->lcsrcidx.ptr.p_int[state->nec] = i;
            state->nec = state->nec+1;
        }
    }
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]!=0 )
        {
            if( ct->ptr.p_int[i]>0 )
            {
                ae_v_moveneg(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            else
            {
                ae_v_move(&state->cleic.ptr.pp_double[state->nec+state->nic][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
            }
            state->lcsrcidx.ptr.p_int[state->nec+state->nic] = i;
            state->nic = state->nic+1;
        }
    }
}


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
     ae_state *_state)
{


    ae_assert(nlec>=0, "MinNLCSetNLC: NLEC<0", _state);
    ae_assert(nlic>=0, "MinNLCSetNLC: NLIC<0", _state);
    state->ng = nlec;
    state->nh = nlic;
    ae_vector_set_length(&state->fi, 1+state->ng+state->nh, _state);
    ae_matrix_set_length(&state->j, 1+state->ng+state->nh, state->n, _state);
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsx, _state), "MinNLCSetCond: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinNLCSetCond: negative EpsX", _state);
    ae_assert(maxits>=0, "MinNLCSetCond: negative MaxIts!", _state);
    if( ae_fp_eq(epsx,(double)(0))&&maxits==0 )
    {
        epsx = 1.0E-8;
    }
    state->epsx = epsx;
    state->maxits = maxits;
}


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
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinNLCSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinNLCSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "MinNLCSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


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
void minnlcsetprecinexact(minnlcstate* state, ae_state *_state)
{


    state->updatefreq = 0;
    state->prectype = 1;
}


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
     ae_state *_state)
{


    ae_assert(updatefreq>=0, "MinNLCSetPrecExactLowRank: UpdateFreq<0", _state);
    if( updatefreq==0 )
    {
        updatefreq = 10;
    }
    state->prectype = 2;
    state->updatefreq = updatefreq;
}


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
     ae_state *_state)
{


    ae_assert(updatefreq>=0, "MinNLCSetPrecExactLowRank: UpdateFreq<0", _state);
    if( updatefreq==0 )
    {
        updatefreq = 10;
    }
    state->prectype = 3;
    state->updatefreq = updatefreq;
}


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
void minnlcsetprecnone(minnlcstate* state, ae_state *_state)
{


    state->updatefreq = 0;
    state->prectype = 0;
}


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
void minnlcsetstpmax(minnlcstate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinNLCSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,(double)(0)), "MinNLCSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


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
     ae_state *_state)
{


    ae_assert(itscnt>=0, "MinNLCSetAlgoAUL: negative ItsCnt", _state);
    ae_assert(ae_isfinite(rho, _state), "MinNLCSetAlgoAUL: Rho is not finite", _state);
    ae_assert(ae_fp_greater(rho,(double)(0)), "MinNLCSetAlgoAUL: Rho<=0", _state);
    if( itscnt==0 )
    {
        itscnt = 10;
    }
    state->aulitscnt = itscnt;
    state->rho = rho;
    state->solvertype = 0;
}


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
void minnlcsetalgoslp(minnlcstate* state, ae_state *_state)
{


    state->solvertype = 1;
}


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
void minnlcsetalgosqp(minnlcstate* state, ae_state *_state)
{


    state->solvertype = 2;
}


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
void minnlcsetxrep(minnlcstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


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
ae_bool minnlciteration(minnlcstate* state, ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t n;
    ae_int_t ng;
    ae_int_t nh;
    double vleft;
    double vright;
    ae_bool b;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        i = state->rstate.ia.ptr.p_int[0];
        k = state->rstate.ia.ptr.p_int[1];
        n = state->rstate.ia.ptr.p_int[2];
        ng = state->rstate.ia.ptr.p_int[3];
        nh = state->rstate.ia.ptr.p_int[4];
        b = state->rstate.ba.ptr.p_bool[0];
        vleft = state->rstate.ra.ptr.p_double[0];
        vright = state->rstate.ra.ptr.p_double[1];
    }
    else
    {
        i = 359;
        k = -58;
        n = -919;
        ng = -909;
        nh = 81;
        b = ae_true;
        vleft = 74.0;
        vright = -788.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    if( state->rstate.stage==5 )
    {
        goto lbl_5;
    }
    if( state->rstate.stage==6 )
    {
        goto lbl_6;
    }
    if( state->rstate.stage==7 )
    {
        goto lbl_7;
    }
    if( state->rstate.stage==8 )
    {
        goto lbl_8;
    }
    if( state->rstate.stage==9 )
    {
        goto lbl_9;
    }
    if( state->rstate.stage==10 )
    {
        goto lbl_10;
    }
    if( state->rstate.stage==11 )
    {
        goto lbl_11;
    }
    if( state->rstate.stage==12 )
    {
        goto lbl_12;
    }
    if( state->rstate.stage==13 )
    {
        goto lbl_13;
    }
    if( state->rstate.stage==14 )
    {
        goto lbl_14;
    }
    if( state->rstate.stage==15 )
    {
        goto lbl_15;
    }
    if( state->rstate.stage==16 )
    {
        goto lbl_16;
    }
    if( state->rstate.stage==17 )
    {
        goto lbl_17;
    }
    if( state->rstate.stage==18 )
    {
        goto lbl_18;
    }
    if( state->rstate.stage==19 )
    {
        goto lbl_19;
    }
    if( state->rstate.stage==20 )
    {
        goto lbl_20;
    }
    if( state->rstate.stage==21 )
    {
        goto lbl_21;
    }
    if( state->rstate.stage==22 )
    {
        goto lbl_22;
    }
    if( state->rstate.stage==23 )
    {
        goto lbl_23;
    }
    if( state->rstate.stage==24 )
    {
        goto lbl_24;
    }
    
    /*
     * Routine body
     */
    
    /*
     * Init
     */
    state->userterminationneeded = ae_false;
    state->repterminationtype = 0;
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    state->repnfev = 0;
    state->repdbgphase0its = 0;
    state->repbcerr = (double)(0);
    state->repbcidx = -1;
    state->replcerr = (double)(0);
    state->replcidx = -1;
    state->repnlcerr = (double)(0);
    state->repnlcidx = -1;
    n = state->n;
    ng = state->ng;
    nh = state->nh;
    minnlc_clearrequestfields(state, _state);
    ae_assert(state->smoothnessguardlevel==0||state->smoothnessguardlevel==1, "MinNLCIteration: integrity check failed", _state);
    b = state->smoothnessguardlevel>0;
    b = b||(state->solvertype==1&&ae_is_trace_enabled("SLP.PROBING"));
    b = b||(state->solvertype==2&&ae_is_trace_enabled("SQP.PROBING"));
    smoothnessmonitorinit(&state->smonitor, &state->s, n, 1+ng+nh, b, _state);
    for(i=0; i<=n-1; i++)
    {
        state->lastscaleused.ptr.p_double[i] = state->s.ptr.p_double[i];
    }
    
    /*
     * Check correctness of box constraints
     */
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i] )
        {
            if( ae_fp_greater(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->repterminationtype = -3;
                state->repbcerr = state->bndl.ptr.p_double[i]-state->bndu.ptr.p_double[i];
                state->repbcidx = i;
                result = ae_false;
                return result;
            }
        }
    }
    
    /*
     * Test gradient
     */
    if( !(ae_fp_eq(state->diffstep,(double)(0))&&ae_fp_greater(state->teststep,(double)(0))) )
    {
        goto lbl_25;
    }
lbl_27:
    if( !smoothnessmonitorcheckgradientatx0(&state->smonitor, &state->xstart, &state->s, &state->bndl, &state->bndu, ae_true, state->teststep, _state) )
    {
        goto lbl_28;
    }
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->smonitor.x.ptr.p_double[i];
    }
    state->needfij = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    for(i=0; i<=ng+nh; i++)
    {
        state->smonitor.fi.ptr.p_double[i] = state->fi.ptr.p_double[i];
        for(k=0; k<=n-1; k++)
        {
            state->smonitor.j.ptr.pp_double[i][k] = state->j.ptr.pp_double[i][k];
        }
    }
    goto lbl_27;
lbl_28:
lbl_25:
    
    /*
     * AUL solver
     */
    if( state->solvertype!=0 )
    {
        goto lbl_29;
    }
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        rvectorsetlengthatleast(&state->xbase, n, _state);
        rvectorsetlengthatleast(&state->fbase, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fm2, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fm1, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fp1, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fp2, 1+ng+nh, _state);
    }
    ae_vector_set_length(&state->rstateaul.ia, 8+1, _state);
    ae_vector_set_length(&state->rstateaul.ra, 7+1, _state);
    state->rstateaul.stage = -1;
lbl_31:
    if( !minnlc_auliteration(state, &state->smonitor, _state) )
    {
        goto lbl_32;
    }
    
    /*
     * Numerical differentiation (if needed) - intercept NeedFiJ
     * request and replace it by sequence of NeedFi requests
     */
    if( !(ae_fp_neq(state->diffstep,(double)(0))&&state->needfij) )
    {
        goto lbl_33;
    }
    state->needfij = ae_false;
    state->needfi = ae_true;
    ae_v_move(&state->xbase.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    k = 0;
lbl_35:
    if( k>n-1 )
    {
        goto lbl_37;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    ae_v_move(&state->fm2.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    ae_v_move(&state->fm1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    ae_v_move(&state->fp1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    ae_v_move(&state->fp2.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    for(i=0; i<=ng+nh; i++)
    {
        state->j.ptr.pp_double[i][k] = ((double)8*(state->fp1.ptr.p_double[i]-state->fm1.ptr.p_double[i])-(state->fp2.ptr.p_double[i]-state->fm2.ptr.p_double[i]))/((double)6*state->diffstep*state->s.ptr.p_double[k]);
    }
    k = k+1;
    goto lbl_35;
lbl_37:
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    
    /*
     * Restore previous values of fields and continue
     */
    state->needfi = ae_false;
    state->needfij = ae_true;
    goto lbl_31;
lbl_33:
    
    /*
     * Forward request to caller
     */
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    goto lbl_31;
lbl_32:
    result = ae_false;
    return result;
lbl_29:
    
    /*
     * SLP solver
     */
    if( state->solvertype!=1 )
    {
        goto lbl_38;
    }
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        rvectorsetlengthatleast(&state->xbase, n, _state);
        rvectorsetlengthatleast(&state->fbase, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fm2, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fm1, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fp1, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fp2, 1+ng+nh, _state);
    }
    minslpinitbuf(&state->bndl, &state->bndu, &state->s, &state->xstart, n, &state->cleic, &state->lcsrcidx, state->nec, state->nic, state->ng, state->nh, state->epsx, state->maxits, &state->slpsolverstate, _state);
lbl_40:
    if( !minslpiteration(&state->slpsolverstate, &state->smonitor, state->userterminationneeded, _state) )
    {
        goto lbl_41;
    }
    
    /*
     * Forward request to caller
     */
    if( !state->slpsolverstate.needfij )
    {
        goto lbl_42;
    }
    
    /*
     * Evaluate target function/Jacobian
     */
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        goto lbl_44;
    }
    
    /*
     * Analytic Jacobian is provided
     */
    minnlc_unscale(state, &state->slpsolverstate.x, &state->slpsolverstate.scaledbndl, &state->slpsolverstate.scaledbndu, &state->x, _state);
    state->needfij = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->needfij = ae_false;
    for(i=0; i<=ng+nh; i++)
    {
        state->slpsolverstate.fi.ptr.p_double[i] = state->fi.ptr.p_double[i];
        for(k=0; k<=n-1; k++)
        {
            state->slpsolverstate.j.ptr.pp_double[i][k] = state->j.ptr.pp_double[i][k]*state->s.ptr.p_double[k];
        }
    }
    goto lbl_45;
lbl_44:
    
    /*
     * Numerical differentiation
     */
    state->needfij = ae_false;
    state->needfi = ae_true;
    minnlc_unscale(state, &state->slpsolverstate.x, &state->slpsolverstate.scaledbndl, &state->slpsolverstate.scaledbndu, &state->xbase, _state);
    k = 0;
lbl_46:
    if( k>n-1 )
    {
        goto lbl_48;
    }
    vleft = state->xbase.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    vright = state->xbase.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    if( !((state->hasbndl.ptr.p_bool[k]&&ae_fp_less(vleft,state->bndl.ptr.p_double[k]))||(state->hasbndu.ptr.p_bool[k]&&ae_fp_greater(vright,state->bndu.ptr.p_double[k]))) )
    {
        goto lbl_49;
    }
    
    /*
     * Box constraint is violated by 4-point centered formula, use 2-point uncentered one
     */
    if( state->hasbndl.ptr.p_bool[k]&&ae_fp_less(vleft,state->bndl.ptr.p_double[k]) )
    {
        vleft = state->bndl.ptr.p_double[k];
    }
    if( state->hasbndu.ptr.p_bool[k]&&ae_fp_greater(vright,state->bndu.ptr.p_double[k]) )
    {
        vright = state->bndu.ptr.p_double[k];
    }
    ae_assert(ae_fp_less_eq(vleft,vright), "MinNLC: integrity check failed", _state);
    if( ae_fp_eq(vleft,vright) )
    {
        
        /*
         * Fixed variable
         */
        for(i=0; i<=ng+nh; i++)
        {
            state->j.ptr.pp_double[i][k] = (double)(0);
        }
        goto lbl_47;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = vleft;
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    ae_v_move(&state->fm1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = vright;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    ae_v_move(&state->fp1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    for(i=0; i<=ng+nh; i++)
    {
        state->j.ptr.pp_double[i][k] = (state->fp1.ptr.p_double[i]-state->fm1.ptr.p_double[i])/(vright-vleft);
    }
    goto lbl_50;
lbl_49:
    
    /*
     * 4-point centered formula does not violate box constraints
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    ae_v_move(&state->fm2.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    ae_v_move(&state->fm1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    ae_v_move(&state->fp1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    ae_v_move(&state->fp2.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    for(i=0; i<=ng+nh; i++)
    {
        state->j.ptr.pp_double[i][k] = ((double)8*(state->fp1.ptr.p_double[i]-state->fm1.ptr.p_double[i])-(state->fp2.ptr.p_double[i]-state->fm2.ptr.p_double[i]))/((double)6*state->diffstep*state->s.ptr.p_double[k]);
    }
lbl_50:
lbl_47:
    k = k+1;
    goto lbl_46;
lbl_48:
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->needfi = ae_false;
    state->needfij = ae_true;
    for(i=0; i<=ng+nh; i++)
    {
        state->slpsolverstate.fi.ptr.p_double[i] = state->fi.ptr.p_double[i];
        for(k=0; k<=n-1; k++)
        {
            state->slpsolverstate.j.ptr.pp_double[i][k] = state->j.ptr.pp_double[i][k]*state->s.ptr.p_double[k];
        }
    }
lbl_45:
    inc(&state->repnfev, _state);
    goto lbl_40;
lbl_42:
    if( !state->slpsolverstate.xupdated )
    {
        goto lbl_51;
    }
    
    /*
     * Report current point
     */
    if( !state->xrep )
    {
        goto lbl_53;
    }
    minnlc_unscale(state, &state->slpsolverstate.x, &state->slpsolverstate.scaledbndl, &state->slpsolverstate.scaledbndu, &state->x, _state);
    state->f = state->slpsolverstate.f;
    state->xupdated = ae_true;
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->xupdated = ae_false;
lbl_53:
    goto lbl_40;
lbl_51:
    ae_assert(state->slpsolverstate.needfij, "NLC:SLP:request", _state);
    goto lbl_40;
lbl_41:
    state->repterminationtype = state->slpsolverstate.repterminationtype;
    state->repouteriterationscount = state->slpsolverstate.repouteriterationscount;
    state->repinneriterationscount = state->slpsolverstate.repinneriterationscount;
    state->repbcerr = state->slpsolverstate.repbcerr;
    state->repbcidx = state->slpsolverstate.repbcidx;
    state->replcerr = state->slpsolverstate.replcerr;
    state->replcidx = state->slpsolverstate.replcidx;
    state->repnlcerr = state->slpsolverstate.repnlcerr;
    state->repnlcidx = state->slpsolverstate.repnlcidx;
    minnlc_unscale(state, &state->slpsolverstate.stepkx, &state->slpsolverstate.scaledbndl, &state->slpsolverstate.scaledbndu, &state->xc, _state);
    result = ae_false;
    return result;
lbl_38:
    
    /*
     * SQP solver
     */
    if( state->solvertype!=2 )
    {
        goto lbl_55;
    }
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        rvectorsetlengthatleast(&state->xbase, n, _state);
        rvectorsetlengthatleast(&state->fbase, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fm2, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fm1, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fp1, 1+ng+nh, _state);
        rvectorsetlengthatleast(&state->fp2, 1+ng+nh, _state);
    }
    minsqpinitbuf(&state->bndl, &state->bndu, &state->s, &state->xstart, n, &state->cleic, &state->lcsrcidx, state->nec, state->nic, state->ng, state->nh, state->epsx, state->maxits, &state->sqpsolverstate, _state);
lbl_57:
    if( !minsqpiteration(&state->sqpsolverstate, &state->smonitor, state->userterminationneeded, _state) )
    {
        goto lbl_58;
    }
    
    /*
     * Forward request to caller
     */
    if( !state->sqpsolverstate.needfij )
    {
        goto lbl_59;
    }
    
    /*
     * Evaluate target function/Jacobian
     */
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        goto lbl_61;
    }
    
    /*
     * Analytic Jacobian is provided
     */
    minnlc_unscale(state, &state->sqpsolverstate.x, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->x, _state);
    state->needfij = ae_true;
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
    state->needfij = ae_false;
    for(i=0; i<=ng+nh; i++)
    {
        state->sqpsolverstate.fi.ptr.p_double[i] = state->fi.ptr.p_double[i];
        for(k=0; k<=n-1; k++)
        {
            state->sqpsolverstate.j.ptr.pp_double[i][k] = state->j.ptr.pp_double[i][k]*state->s.ptr.p_double[k];
        }
    }
    goto lbl_62;
lbl_61:
    
    /*
     * Numerical differentiation
     */
    state->needfij = ae_false;
    state->needfi = ae_true;
    minnlc_unscale(state, &state->sqpsolverstate.x, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->xbase, _state);
    k = 0;
lbl_63:
    if( k>n-1 )
    {
        goto lbl_65;
    }
    vleft = state->xbase.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    vright = state->xbase.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    if( !((state->hasbndl.ptr.p_bool[k]&&ae_fp_less(vleft,state->bndl.ptr.p_double[k]))||(state->hasbndu.ptr.p_bool[k]&&ae_fp_greater(vright,state->bndu.ptr.p_double[k]))) )
    {
        goto lbl_66;
    }
    
    /*
     * Box constraint is violated by 4-point centered formula, use 2-point uncentered one
     */
    if( state->hasbndl.ptr.p_bool[k]&&ae_fp_less(vleft,state->bndl.ptr.p_double[k]) )
    {
        vleft = state->bndl.ptr.p_double[k];
    }
    if( state->hasbndu.ptr.p_bool[k]&&ae_fp_greater(vright,state->bndu.ptr.p_double[k]) )
    {
        vright = state->bndu.ptr.p_double[k];
    }
    ae_assert(ae_fp_less_eq(vleft,vright), "MinNLC: integrity check failed", _state);
    if( ae_fp_eq(vleft,vright) )
    {
        
        /*
         * Fixed variable
         */
        for(i=0; i<=ng+nh; i++)
        {
            state->j.ptr.pp_double[i][k] = (double)(0);
        }
        goto lbl_64;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = vleft;
    state->rstate.stage = 17;
    goto lbl_rcomm;
lbl_17:
    ae_v_move(&state->fm1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = vright;
    state->rstate.stage = 18;
    goto lbl_rcomm;
lbl_18:
    ae_v_move(&state->fp1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    for(i=0; i<=ng+nh; i++)
    {
        state->j.ptr.pp_double[i][k] = (state->fp1.ptr.p_double[i]-state->fm1.ptr.p_double[i])/(vright-vleft);
    }
    goto lbl_67;
lbl_66:
    
    /*
     * 4-point centered formula does not violate box constraints
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 19;
    goto lbl_rcomm;
lbl_19:
    ae_v_move(&state->fm2.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 20;
    goto lbl_rcomm;
lbl_20:
    ae_v_move(&state->fm1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 21;
    goto lbl_rcomm;
lbl_21:
    ae_v_move(&state->fp1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 22;
    goto lbl_rcomm;
lbl_22:
    ae_v_move(&state->fp2.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,ng+nh));
    for(i=0; i<=ng+nh; i++)
    {
        state->j.ptr.pp_double[i][k] = ((double)8*(state->fp1.ptr.p_double[i]-state->fm1.ptr.p_double[i])-(state->fp2.ptr.p_double[i]-state->fm2.ptr.p_double[i]))/((double)6*state->diffstep*state->s.ptr.p_double[k]);
    }
lbl_67:
lbl_64:
    k = k+1;
    goto lbl_63;
lbl_65:
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 23;
    goto lbl_rcomm;
lbl_23:
    state->needfi = ae_false;
    state->needfij = ae_true;
    for(i=0; i<=ng+nh; i++)
    {
        state->sqpsolverstate.fi.ptr.p_double[i] = state->fi.ptr.p_double[i];
        for(k=0; k<=n-1; k++)
        {
            state->sqpsolverstate.j.ptr.pp_double[i][k] = state->j.ptr.pp_double[i][k]*state->s.ptr.p_double[k];
        }
    }
lbl_62:
    inc(&state->repnfev, _state);
    goto lbl_57;
lbl_59:
    if( !state->sqpsolverstate.xupdated )
    {
        goto lbl_68;
    }
    
    /*
     * Report current point
     */
    if( !state->xrep )
    {
        goto lbl_70;
    }
    minnlc_unscale(state, &state->sqpsolverstate.x, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->x, _state);
    state->f = state->sqpsolverstate.f;
    state->xupdated = ae_true;
    state->rstate.stage = 24;
    goto lbl_rcomm;
lbl_24:
    state->xupdated = ae_false;
lbl_70:
    goto lbl_57;
lbl_68:
    ae_assert(state->sqpsolverstate.needfij, "NLC:SQP:request", _state);
    goto lbl_57;
lbl_58:
    state->repterminationtype = state->sqpsolverstate.repterminationtype;
    state->repouteriterationscount = state->sqpsolverstate.repiterationscount;
    state->repinneriterationscount = state->sqpsolverstate.repiterationscount;
    state->repbcerr = state->sqpsolverstate.repbcerr;
    state->repbcidx = state->sqpsolverstate.repbcidx;
    state->replcerr = state->sqpsolverstate.replcerr;
    state->replcidx = state->sqpsolverstate.replcidx;
    state->repnlcerr = state->sqpsolverstate.repnlcerr;
    state->repnlcidx = state->sqpsolverstate.repnlcidx;
    minnlc_unscale(state, &state->sqpsolverstate.stepkx, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->xc, _state);
    result = ae_false;
    return result;
lbl_55:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = i;
    state->rstate.ia.ptr.p_int[1] = k;
    state->rstate.ia.ptr.p_int[2] = n;
    state->rstate.ia.ptr.p_int[3] = ng;
    state->rstate.ia.ptr.p_int[4] = nh;
    state->rstate.ba.ptr.p_bool[0] = b;
    state->rstate.ra.ptr.p_double[0] = vleft;
    state->rstate.ra.ptr.p_double[1] = vright;
    return result;
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "MinNLCOptGuardGradient: TestStep contains NaN or INF", _state);
    ae_assert(ae_fp_greater_eq(teststep,(double)(0)), "MinNLCOptGuardGradient: invalid argument TestStep(TestStep<0)", _state);
    state->teststep = teststep;
}


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
     ae_state *_state)
{


    ae_assert(level==0||level==1, "MinNLCOptGuardSmoothness: unexpected value of level parameter", _state);
    state->smoothnessguardlevel = level;
}


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
     ae_state *_state)
{

    _optguardreport_clear(rep);

    smoothnessmonitorexportreport(&state->smonitor, rep, _state);
}


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
     ae_state *_state)
{

    _optguardnonc1test0report_clear(strrep);
    _optguardnonc1test0report_clear(lngrep);

    smoothnessmonitorexportc1test0report(&state->smonitor.nonc1test0strrep, &state->lastscaleused, strrep, _state);
    smoothnessmonitorexportc1test0report(&state->smonitor.nonc1test0lngrep, &state->lastscaleused, lngrep, _state);
}


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
     ae_state *_state)
{

    _optguardnonc1test1report_clear(strrep);
    _optguardnonc1test1report_clear(lngrep);

    smoothnessmonitorexportc1test1report(&state->smonitor.nonc1test1strrep, &state->lastscaleused, strrep, _state);
    smoothnessmonitorexportc1test1report(&state->smonitor.nonc1test1lngrep, &state->lastscaleused, lngrep, _state);
}


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
     ae_state *_state)
{

    ae_vector_clear(x);
    _minnlcreport_clear(rep);

    minnlcresultsbuf(state, x, rep, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    rep->iterationscount = state->repinneriterationscount;
    rep->nfev = state->repnfev;
    rep->terminationtype = state->repterminationtype;
    rep->bcerr = state->repbcerr;
    rep->bcidx = state->repbcidx;
    rep->lcerr = state->replcerr;
    rep->lcidx = state->replcidx;
    rep->nlcerr = state->repnlcerr;
    rep->nlcidx = state->repnlcidx;
    rep->dbgphase0its = state->repdbgphase0its;
    if( state->repterminationtype>0 )
    {
        ae_v_move(&x->ptr.p_double[0], 1, &state->xc.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    }
    else
    {
        for(i=0; i<=state->n-1; i++)
        {
            x->ptr.p_double[i] = _state->v_nan;
        }
    }
}


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
void minnlcrequesttermination(minnlcstate* state, ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


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
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(x->cnt>=n, "MinNLCRestartFrom: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinNLCRestartFrom: X contains infinite or NaN values!", _state);
    
    /*
     * Set XC
     */
    ae_v_move(&state->xstart.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * prepare RComm facilities
     */
    ae_vector_set_length(&state->rstate.ia, 4+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 1+1, _state);
    state->rstate.stage = -1;
    minnlc_clearrequestfields(state, _state);
}


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
     ae_state *_state)
{

    *f = 0.0;
    *df = 0.0;
    *d2f = 0.0;

    *f = 0.5*alpha*alpha;
    *df = alpha;
    *d2f = 1.0;
}


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
     ae_state *_state)
{

    *f = 0.0;
    *df = 0.0;
    *d2f = 0.0;

    if( ae_fp_greater_eq(alpha,stabilizingpoint) )
    {
        *f = 0.0;
        *df = 0.0;
        *d2f = 0.0;
    }
    else
    {
        alpha = alpha-stabilizingpoint;
        *f = 0.5*alpha*alpha;
        *df = alpha;
        *d2f = 1.0;
    }
}


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
     ae_state *_state)
{

    *f = 0.0;
    *df = 0.0;
    *d2f = 0.0;

    if( ae_fp_greater_eq(alpha,0.5) )
    {
        *f = -ae_log(alpha, _state);
        *df = -(double)1/alpha;
        *d2f = (double)1/(alpha*alpha);
    }
    else
    {
        *f = (double)2*alpha*alpha-(double)4*alpha+(ae_log((double)(2), _state)+1.5);
        *df = (double)4*alpha-(double)4;
        *d2f = (double)(4);
    }
}


/*************************************************************************
Clears request fileds (to be sure that we don't forget to clear something)
*************************************************************************/
static void minnlc_clearrequestfields(minnlcstate* state,
     ae_state *_state)
{


    state->needfi = ae_false;
    state->needfij = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Internal initialization subroutine.
Sets default NLC solver with default criteria.
*************************************************************************/
static void minnlc_minnlcinitinternal(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnlcstate* state,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    memset(&c, 0, sizeof(c));
    memset(&ct, 0, sizeof(ct));
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    
    /*
     * Default params
     */
    state->stabilizingpoint = -2.0;
    state->initialinequalitymultiplier = 1.0;
    
    /*
     * Smoothness monitor, default init
     */
    state->teststep = (double)(0);
    state->smoothnessguardlevel = 0;
    smoothnessmonitorinit(&state->smonitor, &state->s, 0, 0, ae_false, _state);
    
    /*
     * Initialize other params
     */
    state->n = n;
    state->diffstep = diffstep;
    state->userterminationneeded = ae_false;
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->hasbndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->hasbndu, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->lastscaleused, n, _state);
    ae_vector_set_length(&state->xstart, n, _state);
    ae_vector_set_length(&state->xc, n, _state);
    ae_vector_set_length(&state->x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->hasbndl.ptr.p_bool[i] = ae_false;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
        state->hasbndu.ptr.p_bool[i] = ae_false;
        state->s.ptr.p_double[i] = 1.0;
        state->lastscaleused.ptr.p_double[i] = 1.0;
        state->xstart.ptr.p_double[i] = x->ptr.p_double[i];
        state->xc.ptr.p_double[i] = x->ptr.p_double[i];
    }
    minnlcsetlc(state, &c, &ct, 0, _state);
    minnlcsetnlc(state, 0, 0, _state);
    minnlcsetcond(state, 0.0, 0, _state);
    minnlcsetxrep(state, ae_false, _state);
    minnlcsetalgosqp(state, _state);
    minnlcsetprecexactrobust(state, 0, _state);
    minnlcsetstpmax(state, 0.0, _state);
    minlbfgscreate(n, ae_minint(minnlc_lbfgsfactor, n, _state), x, &state->auloptimizer, _state);
    minnlcrestartfrom(state, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function clears preconditioner for L-BFGS optimizer (sets it do default
state);

Parameters:
    AULOptimizer    -   optimizer to tune
    
  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
static void minnlc_clearpreconditioner(minlbfgsstate* auloptimizer,
     ae_state *_state)
{


    minlbfgssetprecdefault(auloptimizer, _state);
}


/*************************************************************************
This function updates preconditioner for L-BFGS optimizer.

Parameters:
    PrecType        -   preconditioner type:
                        * 0 for unpreconditioned iterations
                        * 1 for inexact LBFGS
                        * 2 for exact low rank preconditioner update after each UpdateFreq its
                        * 3 for exact robust preconditioner update after each UpdateFreq its
    UpdateFreq      -   update frequency
    PrecCounter     -   iterations counter, must be zero on the first call,
                        automatically increased  by  this  function.  This
                        counter is used to implement "update-once-in-X-iterations"
                        scheme.
    AULOptimizer    -   optimizer to tune
    X               -   current point
    Rho             -   penalty term
    GammaK          -   current  estimate  of  Hessian  norm   (used   for
                        initialization of preconditioner). Can be zero, in
                        which case Hessian is assumed to be unit.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
static void minnlc_updatepreconditioner(ae_int_t prectype,
     ae_int_t updatefreq,
     ae_int_t* preccounter,
     minlbfgsstate* auloptimizer,
     /* Real    */ const ae_vector* x,
     double rho,
     double gammak,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* hasbndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ const ae_vector* nubc,
     /* Real    */ const ae_matrix* cleic,
     /* Real    */ const ae_vector* nulc,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     /* Real    */ const ae_vector* nunlc,
     /* Real    */ ae_vector* bufd,
     /* Real    */ ae_vector* bufc,
     /* Real    */ ae_matrix* bufw,
     /* Real    */ ae_matrix* bufz,
     /* Real    */ ae_vector* tmp0,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t ng,
     ae_int_t nh,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double p;
    double dp;
    double d2p;
    ae_bool bflag;


    ae_assert(ae_fp_greater(rho,(double)(0)), "MinNLC: integrity check failed", _state);
    rvectorsetlengthatleast(bufd, n, _state);
    rvectorsetlengthatleast(bufc, nec+nic+ng+nh, _state);
    rmatrixsetlengthatleast(bufw, nec+nic+ng+nh, n, _state);
    rvectorsetlengthatleast(tmp0, n, _state);
    
    /*
     * Preconditioner before update from barrier/penalty functions
     */
    if( ae_fp_eq(gammak,(double)(0)) )
    {
        gammak = (double)(1);
    }
    for(i=0; i<=n-1; i++)
    {
        bufd->ptr.p_double[i] = gammak;
    }
    
    /*
     * Update diagonal Hessian using nonlinearity from boundary constraints:
     * * penalty term from equality constraints
     * * shift term from inequality constraints
     *
     * NOTE: penalty term for inequality constraints is ignored because it
     *       is large only in exceptional cases.
     */
    for(i=0; i<=n-1; i++)
    {
        if( (hasbndl->ptr.p_bool[i]&&hasbndu->ptr.p_bool[i])&&ae_fp_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]) )
        {
            minnlcequalitypenaltyfunction((x->ptr.p_double[i]-bndl->ptr.p_double[i])*rho, &p, &dp, &d2p, _state);
            bufd->ptr.p_double[i] = bufd->ptr.p_double[i]+d2p*rho;
            continue;
        }
        if( hasbndl->ptr.p_bool[i] )
        {
            minnlcinequalityshiftfunction((x->ptr.p_double[i]-bndl->ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
            bufd->ptr.p_double[i] = bufd->ptr.p_double[i]+nubc->ptr.p_double[2*i+0]*d2p*rho;
        }
        if( hasbndu->ptr.p_bool[i] )
        {
            minnlcinequalityshiftfunction((bndu->ptr.p_double[i]-x->ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
            bufd->ptr.p_double[i] = bufd->ptr.p_double[i]+nubc->ptr.p_double[2*i+1]*d2p*rho;
        }
    }
    
    /*
     * Process linear constraints
     */
    for(i=0; i<=nec+nic-1; i++)
    {
        ae_v_move(&bufw->ptr.pp_double[i][0], 1, &cleic->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
        v = ae_v_dotproduct(&cleic->ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = v-cleic->ptr.pp_double[i][n];
        if( i<nec )
        {
            
            /*
             * Equality constraint
             */
            minnlcequalitypenaltyfunction(v*rho, &p, &dp, &d2p, _state);
            bufc->ptr.p_double[i] = d2p*rho;
        }
        else
        {
            
            /*
             * Inequality constraint
             */
            minnlcinequalityshiftfunction(-v*rho+(double)1, &p, &dp, &d2p, _state);
            bufc->ptr.p_double[i] = nulc->ptr.p_double[i]*d2p*rho;
        }
    }
    
    /*
     * Process nonlinear constraints
     */
    for(i=0; i<=ng+nh-1; i++)
    {
        ae_v_move(&bufw->ptr.pp_double[nec+nic+i][0], 1, &jac->ptr.pp_double[1+i][0], 1, ae_v_len(0,n-1));
        v = fi->ptr.p_double[1+i];
        if( i<ng )
        {
            
            /*
             * Equality constraint
             */
            minnlcequalitypenaltyfunction(v*rho, &p, &dp, &d2p, _state);
            bufc->ptr.p_double[nec+nic+i] = d2p*rho;
        }
        else
        {
            
            /*
             * Inequality constraint
             */
            minnlcinequalityshiftfunction(-v*rho+(double)1, &p, &dp, &d2p, _state);
            bufc->ptr.p_double[nec+nic+i] = nunlc->ptr.p_double[i]*d2p*rho;
        }
    }
    
    /*
     * Add regularizer (large Rho often result in nearly-degenerate matrices;
     * sometimes Cholesky decomposition fails without regularization).
     *
     * We use RegPrec*diag(W'*W) as preconditioner.
     */
    k = nec+nic+ng+nh;
    for(j=0; j<=n-1; j++)
    {
        tmp0->ptr.p_double[j] = 0.0;
    }
    for(i=0; i<=k-1; i++)
    {
        v = bufc->ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            tmp0->ptr.p_double[j] = tmp0->ptr.p_double[j]+v*bufw->ptr.pp_double[i][j]*bufw->ptr.pp_double[i][j];
        }
    }
    for(j=0; j<=n-1; j++)
    {
        bufd->ptr.p_double[j] = bufd->ptr.p_double[j]+minnlc_regprec*tmp0->ptr.p_double[j];
    }
    
    /*
     * Apply preconditioner
     */
    if( prectype==1 )
    {
        minlbfgssetprecrankklbfgsfast(auloptimizer, bufd, bufc, bufw, nec+nic+ng+nh, _state);
    }
    if( prectype==2&&*preccounter%updatefreq==0 )
    {
        minlbfgssetpreclowrankexact(auloptimizer, bufd, bufc, bufw, nec+nic+ng+nh, _state);
    }
    if( prectype==3&&*preccounter%updatefreq==0 )
    {
        
        /*
         * Generate full NxN dense Hessian
         */
        rmatrixsetlengthatleast(bufz, n, n, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                bufz->ptr.pp_double[i][j] = (double)(0);
            }
            bufz->ptr.pp_double[i][i] = bufd->ptr.p_double[i];
        }
        if( nec+nic+ng+nh>0 )
        {
            for(i=0; i<=nec+nic+ng+nh-1; i++)
            {
                ae_assert(ae_fp_greater_eq(bufc->ptr.p_double[i],(double)(0)), "MinNLC: updatepreconditioner() integrity failure", _state);
                v = ae_sqrt(bufc->ptr.p_double[i], _state);
                for(j=0; j<=n-1; j++)
                {
                    bufw->ptr.pp_double[i][j] = bufw->ptr.pp_double[i][j]*v;
                }
            }
            rmatrixsyrk(n, nec+nic+ng+nh, 1.0, bufw, 0, 0, 2, 1.0, bufz, 0, 0, ae_true, _state);
        }
        
        /*
         * Evaluate Cholesky decomposition, set preconditioner
         */
        bflag = spdmatrixcholeskyrec(bufz, 0, n, ae_true, bufd, _state);
        ae_assert(bflag, "MinNLC: updatepreconditioner() failure, Cholesky failed", _state);
        minlbfgssetpreccholesky(auloptimizer, bufz, ae_true, _state);
    }
    inc(preccounter, _state);
}


/*************************************************************************
This subroutine adds penalty from boundary constraints to target  function
and its gradient. Penalty function is one which is used for main AUL cycle
- with Lagrange multipliers and infinite at the barrier and beyond.

Parameters:
    X[] - current point
    BndL[], BndU[] - boundary constraints
    HasBndL[], HasBndU[] - I-th element is True if corresponding constraint is present
    NuBC[] - Lagrange multipliers corresponding to constraints
    Rho - penalty term
    StabilizingPoint - branch point for inequality stabilizing term
    F - function value to modify
    G - gradient to modify

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
static void minnlc_penaltybc(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* hasbndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ const ae_vector* nubc,
     ae_int_t n,
     double rho,
     double stabilizingpoint,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t i;
    double p;
    double dp;
    double d2p;


    for(i=0; i<=n-1; i++)
    {
        if( (hasbndl->ptr.p_bool[i]&&hasbndu->ptr.p_bool[i])&&ae_fp_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]) )
        {
            
            /*
             * I-th boundary constraint is of equality-type
             */
            minnlcequalitypenaltyfunction((x->ptr.p_double[i]-bndl->ptr.p_double[i])*rho, &p, &dp, &d2p, _state);
            *f = *f+p/rho-nubc->ptr.p_double[2*i+0]*(x->ptr.p_double[i]-bndl->ptr.p_double[i]);
            g->ptr.p_double[i] = g->ptr.p_double[i]+dp-nubc->ptr.p_double[2*i+0];
            continue;
        }
        if( hasbndl->ptr.p_bool[i] )
        {
            
            /*
             * Handle lower bound
             */
            minnlcinequalitypenaltyfunction(x->ptr.p_double[i]-bndl->ptr.p_double[i], stabilizingpoint, &p, &dp, &d2p, _state);
            *f = *f+rho*p;
            g->ptr.p_double[i] = g->ptr.p_double[i]+rho*dp;
            minnlcinequalityshiftfunction((x->ptr.p_double[i]-bndl->ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
            *f = *f+p/rho*nubc->ptr.p_double[2*i+0];
            g->ptr.p_double[i] = g->ptr.p_double[i]+dp*nubc->ptr.p_double[2*i+0];
        }
        if( hasbndu->ptr.p_bool[i] )
        {
            
            /*
             * Handle upper bound
             */
            minnlcinequalitypenaltyfunction(bndu->ptr.p_double[i]-x->ptr.p_double[i], stabilizingpoint, &p, &dp, &d2p, _state);
            *f = *f+rho*p;
            g->ptr.p_double[i] = g->ptr.p_double[i]-rho*dp;
            minnlcinequalityshiftfunction((bndu->ptr.p_double[i]-x->ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
            *f = *f+p/rho*nubc->ptr.p_double[2*i+1];
            g->ptr.p_double[i] = g->ptr.p_double[i]-dp*nubc->ptr.p_double[2*i+1];
        }
    }
}


/*************************************************************************
This subroutine adds penalty from  linear  constraints to target  function
and its gradient. Penalty function is one which is used for main AUL cycle
- with Lagrange multipliers and infinite at the barrier and beyond.

Parameters:
    X[] - current point
    CLEIC[] -   constraints matrix, first NEC rows are equality ones, next
                NIC rows are inequality ones. array[NEC+NIC,N+1]
    NuLC[]  -   Lagrange multipliers corresponding to constraints,
                array[NEC+NIC]
    N       -   dimensionalty
    NEC     -   number of equality constraints
    NIC     -   number of inequality constraints.
    Rho - penalty term
    StabilizingPoint - branch point for inequality stabilizing term
    F - function value to modify
    G - gradient to modify

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
static void minnlc_penaltylc(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* cleic,
     /* Real    */ const ae_vector* nulc,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     double rho,
     double stabilizingpoint,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double p;
    double dp;
    double d2p;
    double fupd;
    double gupd;


    for(i=0; i<=nec+nic-1; i++)
    {
        v = ae_v_dotproduct(&cleic->ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = v-cleic->ptr.pp_double[i][n];
        fupd = (double)(0);
        gupd = (double)(0);
        if( i<nec )
        {
            
            /*
             * Equality constraint
             */
            minnlcequalitypenaltyfunction(v*rho, &p, &dp, &d2p, _state);
            fupd = fupd+p/rho;
            gupd = gupd+dp;
            fupd = fupd-nulc->ptr.p_double[i]*v;
            gupd = gupd-nulc->ptr.p_double[i];
        }
        else
        {
            
            /*
             * Inequality constraint
             */
            minnlcinequalitypenaltyfunction(-v, stabilizingpoint, &p, &dp, &d2p, _state);
            fupd = fupd+p*rho;
            gupd = gupd-dp*rho;
            minnlcinequalityshiftfunction(-v*rho+(double)1, &p, &dp, &d2p, _state);
            fupd = fupd+p/rho*nulc->ptr.p_double[i];
            gupd = gupd-dp*nulc->ptr.p_double[i];
        }
        *f = *f+fupd;
        ae_v_addd(&g->ptr.p_double[0], 1, &cleic->ptr.pp_double[i][0], 1, ae_v_len(0,n-1), gupd);
    }
}


/*************************************************************************
This subroutine adds penalty from nonlinear constraints to target function
and its gradient. Penalty function is one which is used for main AUL cycle
- with Lagrange multipliers and infinite at the barrier and beyond.

Parameters:
    Fi[] - function vector:
          * 1 component for function being minimized
          * NG components for equality constraints G_i(x)=0
          * NH components for inequality constraints H_i(x)<=0
    J[]  - Jacobian matrix, array[1+NG+NH,N]
    NuNLC[]  -   Lagrange multipliers corresponding to constraints,
                array[NG+NH]
    N - number of dimensions
    NG - number of equality constraints
    NH - number of inequality constraints
    Rho - penalty term
    StabilizingPoint - branch point for inequality stabilizing term
    F - function value to modify
    G - gradient to modify

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
static void minnlc_penaltynlc(/* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* j,
     /* Real    */ const ae_vector* nunlc,
     ae_int_t n,
     ae_int_t ng,
     ae_int_t nh,
     double rho,
     double stabilizingpoint,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double p;
    double dp;
    double d2p;
    double fupd;
    double gupd;


    
    /*
     * IMPORTANT: loop starts from 1, not zero!
     */
    for(i=1; i<=ng+nh; i++)
    {
        v = fi->ptr.p_double[i];
        fupd = (double)(0);
        gupd = (double)(0);
        if( i<=ng )
        {
            
            /*
             * Equality constraint
             */
            minnlcequalitypenaltyfunction(v*rho, &p, &dp, &d2p, _state);
            fupd = fupd+p/rho;
            gupd = gupd+dp;
            fupd = fupd-nunlc->ptr.p_double[i-1]*v;
            gupd = gupd-nunlc->ptr.p_double[i-1];
        }
        else
        {
            
            /*
             * Inequality constraint
             */
            minnlcinequalitypenaltyfunction(-v, stabilizingpoint, &p, &dp, &d2p, _state);
            fupd = fupd+p*rho;
            gupd = gupd-dp*rho;
            minnlcinequalityshiftfunction(-v*rho+(double)1, &p, &dp, &d2p, _state);
            fupd = fupd+p/rho*nunlc->ptr.p_double[i-1];
            gupd = gupd-dp*nunlc->ptr.p_double[i-1];
        }
        *f = *f+fupd;
        ae_v_addd(&g->ptr.p_double[0], 1, &j->ptr.pp_double[i][0], 1, ae_v_len(0,n-1), gupd);
    }
}


/*************************************************************************
This function performs actual processing for AUL algorithm. It expects that
caller redirects its reverse communication  requests  NeedFiJ/XUpdated  to
external user who will provide analytic derivative (or handle reports about
progress).

In case external user does not have analytic derivative, it is responsibility
of caller to intercept NeedFiJ request and  replace  it  with  appropriate
numerical differentiation scheme.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
static ae_bool minnlc_auliteration(minnlcstate* state,
     smoothnessmonitor* smonitor,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t ng;
    ae_int_t nh;
    ae_int_t i;
    ae_int_t j;
    ae_int_t outerit;
    ae_int_t preccounter;
    double v;
    double vv;
    double p;
    double dp;
    double d2p;
    double v0;
    double v1;
    double v2;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstateaul.stage>=0 )
    {
        n = state->rstateaul.ia.ptr.p_int[0];
        nec = state->rstateaul.ia.ptr.p_int[1];
        nic = state->rstateaul.ia.ptr.p_int[2];
        ng = state->rstateaul.ia.ptr.p_int[3];
        nh = state->rstateaul.ia.ptr.p_int[4];
        i = state->rstateaul.ia.ptr.p_int[5];
        j = state->rstateaul.ia.ptr.p_int[6];
        outerit = state->rstateaul.ia.ptr.p_int[7];
        preccounter = state->rstateaul.ia.ptr.p_int[8];
        v = state->rstateaul.ra.ptr.p_double[0];
        vv = state->rstateaul.ra.ptr.p_double[1];
        p = state->rstateaul.ra.ptr.p_double[2];
        dp = state->rstateaul.ra.ptr.p_double[3];
        d2p = state->rstateaul.ra.ptr.p_double[4];
        v0 = state->rstateaul.ra.ptr.p_double[5];
        v1 = state->rstateaul.ra.ptr.p_double[6];
        v2 = state->rstateaul.ra.ptr.p_double[7];
    }
    else
    {
        n = 809;
        nec = 205;
        nic = -838;
        ng = 939;
        nh = -526;
        i = 763;
        j = -541;
        outerit = -698;
        preccounter = -900;
        v = -318.0;
        vv = -940.0;
        p = 1016.0;
        dp = -229.0;
        d2p = -536.0;
        v0 = 487.0;
        v1 = -115.0;
        v2 = 886.0;
    }
    if( state->rstateaul.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstateaul.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstateaul.stage==2 )
    {
        goto lbl_2;
    }
    
    /*
     * Routine body
     */
    ae_assert(state->solvertype==0, "MinNLC: internal error", _state);
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    ng = state->ng;
    nh = state->nh;
    
    /*
     * Prepare scaled problem
     */
    rvectorsetlengthatleast(&state->scaledbndl, n, _state);
    rvectorsetlengthatleast(&state->scaledbndu, n, _state);
    rmatrixsetlengthatleast(&state->scaledcleic, nec+nic, n+1, _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state->scaledbndl.ptr.p_double[i] = state->bndl.ptr.p_double[i]/state->s.ptr.p_double[i];
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state->scaledbndu.ptr.p_double[i] = state->bndu.ptr.p_double[i]/state->s.ptr.p_double[i];
        }
        state->xc.ptr.p_double[i] = state->xstart.ptr.p_double[i]/state->s.ptr.p_double[i];
    }
    for(i=0; i<=nec+nic-1; i++)
    {
        
        /*
         * Scale and normalize linear constraints
         */
        vv = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = state->cleic.ptr.pp_double[i][j]*state->s.ptr.p_double[j];
            state->scaledcleic.ptr.pp_double[i][j] = v;
            vv = vv+v*v;
        }
        vv = ae_sqrt(vv, _state);
        state->scaledcleic.ptr.pp_double[i][n] = state->cleic.ptr.pp_double[i][n];
        if( ae_fp_greater(vv,(double)(0)) )
        {
            for(j=0; j<=n; j++)
            {
                state->scaledcleic.ptr.pp_double[i][j] = state->scaledcleic.ptr.pp_double[i][j]/vv;
            }
        }
    }
    
    /*
     * Prepare stopping criteria
     */
    minlbfgssetcond(&state->auloptimizer, (double)(0), (double)(0), state->epsx, state->maxits, _state);
    minlbfgssetstpmax(&state->auloptimizer, state->stpmax, _state);
    
    /*
     * Main AUL cycle:
     * * prepare Lagrange multipliers NuNB/NuLC
     * * set GammaK (current estimate of Hessian norm) to InitGamma and XKPresent to False
     */
    rvectorsetlengthatleast(&state->nubc, 2*n, _state);
    rvectorsetlengthatleast(&state->nulc, nec+nic, _state);
    rvectorsetlengthatleast(&state->nunlc, ng+nh, _state);
    rvectorsetlengthatleast(&state->xk, n, _state);
    rvectorsetlengthatleast(&state->gk, n, _state);
    rvectorsetlengthatleast(&state->xk1, n, _state);
    rvectorsetlengthatleast(&state->gk1, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->nubc.ptr.p_double[2*i+0] = 0.0;
        state->nubc.ptr.p_double[2*i+1] = 0.0;
        if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            continue;
        }
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state->nubc.ptr.p_double[2*i+0] = state->initialinequalitymultiplier;
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state->nubc.ptr.p_double[2*i+1] = state->initialinequalitymultiplier;
        }
    }
    for(i=0; i<=nec-1; i++)
    {
        state->nulc.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=nic-1; i++)
    {
        state->nulc.ptr.p_double[nec+i] = state->initialinequalitymultiplier;
    }
    for(i=0; i<=ng-1; i++)
    {
        state->nunlc.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=nh-1; i++)
    {
        state->nunlc.ptr.p_double[ng+i] = state->initialinequalitymultiplier;
    }
    state->gammak = minnlc_initgamma;
    state->xkpresent = ae_false;
    ae_assert(state->aulitscnt>0, "MinNLC: integrity check failed", _state);
    minnlc_clearpreconditioner(&state->auloptimizer, _state);
    outerit = 0;
lbl_3:
    if( outerit>state->aulitscnt-1 )
    {
        goto lbl_5;
    }
    
    /*
     * Optimize with current Lagrange multipliers
     *
     * NOTE: this code expects and checks that line search ends in the
     *       point which is used as beginning for the next search. Such
     *       guarantee is given by MCSRCH function.  L-BFGS  optimizer
     *       does not formally guarantee it, but it follows same rule.
     *       Below we a) rely on such property of the optimizer, and b)
     *       assert that it is true, in order to fail loudly if it is
     *       not true.
     *
     * NOTE: security check for NAN/INF in F/G is responsibility of
     *       LBFGS optimizer. AUL optimizer checks for NAN/INF only
     *       when we update Lagrange multipliers.
     */
    preccounter = 0;
    minlbfgssetxrep(&state->auloptimizer, ae_true, _state);
    minlbfgsrestartfrom(&state->auloptimizer, &state->xc, _state);
lbl_6:
    if( !minlbfgsiteration(&state->auloptimizer, _state) )
    {
        goto lbl_7;
    }
    if( !state->auloptimizer.needfg )
    {
        goto lbl_8;
    }
    
    /*
     * Un-scale X, evaluate F/G/H, re-scale Jacobian
     */
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->auloptimizer.x.ptr.p_double[i]*state->s.ptr.p_double[i];
    }
    state->needfij = ae_true;
    state->rstateaul.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    for(i=0; i<=ng+nh; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            state->j.ptr.pp_double[i][j] = state->j.ptr.pp_double[i][j]*state->s.ptr.p_double[j];
        }
    }
    
    /*
     * Store data for estimation of Hessian norm:
     * * current point (re-scaled)
     * * gradient of the target function (re-scaled, unmodified)
     */
    ae_v_move(&state->xk1.ptr.p_double[0], 1, &state->auloptimizer.x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->gk1.ptr.p_double[0], 1, &state->j.ptr.pp_double[0][0], 1, ae_v_len(0,n-1));
    
    /*
     * Function being optimized
     */
    state->auloptimizer.f = state->fi.ptr.p_double[0];
    for(i=0; i<=n-1; i++)
    {
        state->auloptimizer.g.ptr.p_double[i] = state->j.ptr.pp_double[0][i];
    }
    
    /*
     * Send information to OptGuard monitor
     */
    smoothnessmonitorenqueuepoint(smonitor, &state->auloptimizer.d, state->auloptimizer.stp, &state->auloptimizer.x, &state->fi, &state->j, _state);
    
    /*
     * Penalty for violation of boundary/linear/nonlinear constraints
     */
    minnlc_penaltybc(&state->auloptimizer.x, &state->scaledbndl, &state->hasbndl, &state->scaledbndu, &state->hasbndu, &state->nubc, n, state->rho, state->stabilizingpoint, &state->auloptimizer.f, &state->auloptimizer.g, _state);
    minnlc_penaltylc(&state->auloptimizer.x, &state->scaledcleic, &state->nulc, n, nec, nic, state->rho, state->stabilizingpoint, &state->auloptimizer.f, &state->auloptimizer.g, _state);
    minnlc_penaltynlc(&state->fi, &state->j, &state->nunlc, n, ng, nh, state->rho, state->stabilizingpoint, &state->auloptimizer.f, &state->auloptimizer.g, _state);
    
    /*
     * Forward termination request if needed
     */
    if( state->userterminationneeded )
    {
        minlbfgsrequesttermination(&state->auloptimizer, _state);
    }
    
    /*
     * To optimizer
     */
    goto lbl_6;
lbl_8:
    if( !state->auloptimizer.xupdated )
    {
        goto lbl_10;
    }
    
    /*
     * Report current point (if needed)
     */
    if( !state->xrep )
    {
        goto lbl_12;
    }
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->auloptimizer.x.ptr.p_double[i]*state->s.ptr.p_double[i];
    }
    state->f = state->auloptimizer.f;
    state->xupdated = ae_true;
    state->rstateaul.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
lbl_12:
    
    /*
     * Send information to OptGuard monitor
     */
    smoothnessmonitorfinalizelinesearch(smonitor, _state);
    smoothnessmonitorstartlinesearch(smonitor, &state->auloptimizer.x, &state->fi, &state->j, state->repinneriterationscount, state->repouteriterationscount, _state);
    
    /*
     * Forward termination request if needed
     */
    if( state->userterminationneeded )
    {
        minlbfgsrequesttermination(&state->auloptimizer, _state);
    }
    
    /*
     * Update constraints violation
     */
    checkbcviolation(&state->hasbndl, &state->scaledbndl, &state->hasbndu, &state->scaledbndu, &state->auloptimizer.x, n, &state->s, ae_false, &state->repbcerr, &state->repbcidx, _state);
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, &state->auloptimizer.x, n, &state->replcerr, &state->replcidx, _state);
    checknlcviolation(&state->fi, ng, nh, &state->repnlcerr, &state->repnlcidx, _state);
    
    /*
     * Update GammaK
     */
    if( state->xkpresent )
    {
        
        /*
         * XK/GK store beginning of current line search, and XK1/GK1
         * store data for the end of the line search:
         * * first, we Assert() that XK1 (last point where function
         *   was evaluated) is same as AULOptimizer.X (what is
         *   reported by RComm interface
         * * calculate step length V2.
         *
         * If V2>HessEstTol, then:
         * * calculate V0 - directional derivative at XK,
         *   and V1 - directional derivative at XK1
         * * set GammaK to Max(GammaK, |V1-V0|/V2)
         */
        for(i=0; i<=n-1; i++)
        {
            ae_assert(ae_fp_less_eq(ae_fabs(state->auloptimizer.x.ptr.p_double[i]-state->xk1.ptr.p_double[i], _state),(double)100*ae_machineepsilon)||!(ae_isfinite(state->auloptimizer.x.ptr.p_double[i], _state)&&ae_isfinite(state->xk1.ptr.p_double[i], _state)), "MinNLC: integrity check failed, unexpected behavior of LBFGS optimizer", _state);
        }
        v2 = 0.0;
        for(i=0; i<=n-1; i++)
        {
            v2 = v2+ae_sqr(state->xk.ptr.p_double[i]-state->xk1.ptr.p_double[i], _state);
        }
        v2 = ae_sqrt(v2, _state);
        if( ae_fp_greater(v2,minnlc_hessesttol) )
        {
            v0 = 0.0;
            v1 = 0.0;
            for(i=0; i<=n-1; i++)
            {
                v = (state->xk.ptr.p_double[i]-state->xk1.ptr.p_double[i])/v2;
                v0 = v0+state->gk.ptr.p_double[i]*v;
                v1 = v1+state->gk1.ptr.p_double[i]*v;
            }
            state->gammak = ae_maxreal(state->gammak, ae_fabs(v1-v0, _state)/v2, _state);
        }
    }
    else
    {
        
        /*
         * Beginning of the first line search, XK is not yet initialized.
         */
        ae_v_move(&state->xk.ptr.p_double[0], 1, &state->xk1.ptr.p_double[0], 1, ae_v_len(0,n-1));
        ae_v_move(&state->gk.ptr.p_double[0], 1, &state->gk1.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->xkpresent = ae_true;
    }
    
    /*
     * Update preconsitioner using current GammaK
     */
    minnlc_updatepreconditioner(state->prectype, state->updatefreq, &preccounter, &state->auloptimizer, &state->auloptimizer.x, state->rho, state->gammak, &state->scaledbndl, &state->hasbndl, &state->scaledbndu, &state->hasbndu, &state->nubc, &state->scaledcleic, &state->nulc, &state->fi, &state->j, &state->nunlc, &state->bufd, &state->bufc, &state->bufw, &state->bufz, &state->tmp0, n, nec, nic, ng, nh, _state);
    goto lbl_6;
lbl_10:
    ae_assert(ae_false, "MinNLC: integrity check failed", _state);
    goto lbl_6;
lbl_7:
    minlbfgsresultsbuf(&state->auloptimizer, &state->xc, &state->aulreport, _state);
    state->repinneriterationscount = state->repinneriterationscount+state->aulreport.iterationscount;
    state->repnfev = state->repnfev+state->aulreport.nfev;
    state->repterminationtype = state->aulreport.terminationtype;
    inc(&state->repouteriterationscount, _state);
    if( state->repterminationtype<=0||state->repterminationtype==8 )
    {
        goto lbl_5;
    }
    
    /*
     * 1. Evaluate F/J
     * 2. Check for NAN/INF in F/J: we just calculate sum of their
     *    components, it should be enough to reduce vector/matrix to
     *    just one value which either "normal" (all summands were "normal")
     *    or NAN/INF (at least one summand was NAN/INF).
     * 3. Update Lagrange multipliers
     */
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->xc.ptr.p_double[i]*state->s.ptr.p_double[i];
    }
    state->needfij = ae_true;
    state->rstateaul.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfij = ae_false;
    v = 0.0;
    for(i=0; i<=ng+nh; i++)
    {
        v = 0.1*v+state->fi.ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            v = 0.1*v+state->j.ptr.pp_double[i][j];
        }
    }
    if( !ae_isfinite(v, _state) )
    {
        
        /*
         * Abnormal termination - infinities in function/gradient
         */
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    for(i=0; i<=ng+nh; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            state->j.ptr.pp_double[i][j] = state->j.ptr.pp_double[i][j]*state->s.ptr.p_double[j];
        }
    }
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Process coefficients corresponding to equality-type
         * constraints.
         */
        if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            minnlcequalitypenaltyfunction((state->xc.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i])*state->rho, &p, &dp, &d2p, _state);
            state->nubc.ptr.p_double[2*i+0] = boundval(state->nubc.ptr.p_double[2*i+0]-dp, -minnlc_maxlagmult, minnlc_maxlagmult, _state);
            continue;
        }
        
        /*
         * Process coefficients corresponding to inequality-type
         * constraints. These coefficients have limited growth/decay
         * per iteration which helps to stabilize algorithm.
         */
        ae_assert(ae_fp_greater(minnlc_aulmaxgrowth,1.0), "MinNLC: integrity error", _state);
        if( state->hasbndl.ptr.p_bool[i] )
        {
            minnlcinequalityshiftfunction((state->xc.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i])*state->rho+(double)1, &p, &dp, &d2p, _state);
            v = ae_fabs(dp, _state);
            v = ae_minreal(v, minnlc_aulmaxgrowth, _state);
            v = ae_maxreal(v, (double)1/minnlc_aulmaxgrowth, _state);
            state->nubc.ptr.p_double[2*i+0] = boundval(state->nubc.ptr.p_double[2*i+0]*v, -minnlc_maxlagmult, minnlc_maxlagmult, _state);
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            minnlcinequalityshiftfunction((state->scaledbndu.ptr.p_double[i]-state->xc.ptr.p_double[i])*state->rho+(double)1, &p, &dp, &d2p, _state);
            v = ae_fabs(dp, _state);
            v = ae_minreal(v, minnlc_aulmaxgrowth, _state);
            v = ae_maxreal(v, (double)1/minnlc_aulmaxgrowth, _state);
            state->nubc.ptr.p_double[2*i+1] = boundval(state->nubc.ptr.p_double[2*i+1]*v, -minnlc_maxlagmult, minnlc_maxlagmult, _state);
        }
    }
    for(i=0; i<=nec+nic-1; i++)
    {
        v = ae_v_dotproduct(&state->scaledcleic.ptr.pp_double[i][0], 1, &state->xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = v-state->scaledcleic.ptr.pp_double[i][n];
        if( i<nec )
        {
            minnlcequalitypenaltyfunction(v*state->rho, &p, &dp, &d2p, _state);
            state->nulc.ptr.p_double[i] = boundval(state->nulc.ptr.p_double[i]-dp, -minnlc_maxlagmult, minnlc_maxlagmult, _state);
        }
        else
        {
            minnlcinequalityshiftfunction(-v*state->rho+(double)1, &p, &dp, &d2p, _state);
            v = ae_fabs(dp, _state);
            v = ae_minreal(v, minnlc_aulmaxgrowth, _state);
            v = ae_maxreal(v, (double)1/minnlc_aulmaxgrowth, _state);
            state->nulc.ptr.p_double[i] = boundval(state->nulc.ptr.p_double[i]*v, -minnlc_maxlagmult, minnlc_maxlagmult, _state);
        }
    }
    for(i=1; i<=ng+nh; i++)
    {
        
        /*
         * NOTE: loop index must start from 1, not zero!
         */
        v = state->fi.ptr.p_double[i];
        if( i<=ng )
        {
            minnlcequalitypenaltyfunction(v*state->rho, &p, &dp, &d2p, _state);
            state->nunlc.ptr.p_double[i-1] = boundval(state->nunlc.ptr.p_double[i-1]-dp, -minnlc_maxlagmult, minnlc_maxlagmult, _state);
        }
        else
        {
            minnlcinequalityshiftfunction(-v*state->rho+(double)1, &p, &dp, &d2p, _state);
            v = ae_fabs(dp, _state);
            v = ae_minreal(v, minnlc_aulmaxgrowth, _state);
            v = ae_maxreal(v, (double)1/minnlc_aulmaxgrowth, _state);
            state->nunlc.ptr.p_double[i-1] = boundval(state->nunlc.ptr.p_double[i-1]*v, -minnlc_maxlagmult, minnlc_maxlagmult, _state);
        }
    }
    outerit = outerit+1;
    goto lbl_3;
lbl_5:
    for(i=0; i<=n-1; i++)
    {
        state->xc.ptr.p_double[i] = state->xc.ptr.p_double[i]*state->s.ptr.p_double[i];
    }
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstateaul.ia.ptr.p_int[0] = n;
    state->rstateaul.ia.ptr.p_int[1] = nec;
    state->rstateaul.ia.ptr.p_int[2] = nic;
    state->rstateaul.ia.ptr.p_int[3] = ng;
    state->rstateaul.ia.ptr.p_int[4] = nh;
    state->rstateaul.ia.ptr.p_int[5] = i;
    state->rstateaul.ia.ptr.p_int[6] = j;
    state->rstateaul.ia.ptr.p_int[7] = outerit;
    state->rstateaul.ia.ptr.p_int[8] = preccounter;
    state->rstateaul.ra.ptr.p_double[0] = v;
    state->rstateaul.ra.ptr.p_double[1] = vv;
    state->rstateaul.ra.ptr.p_double[2] = p;
    state->rstateaul.ra.ptr.p_double[3] = dp;
    state->rstateaul.ra.ptr.p_double[4] = d2p;
    state->rstateaul.ra.ptr.p_double[5] = v0;
    state->rstateaul.ra.ptr.p_double[6] = v1;
    state->rstateaul.ra.ptr.p_double[7] = v2;
    return result;
}


/*************************************************************************
Unscales X (converts from scaled variables to original ones), paying special
attention to box constraints (output is always feasible; active constraints
are mapped to active ones).
*************************************************************************/
static void minnlc_unscale(const minnlcstate* state,
     /* Real    */ const ae_vector* xs,
     /* Real    */ const ae_vector* scaledbndl,
     /* Real    */ const ae_vector* scaledbndu,
     /* Real    */ ae_vector* xu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i]&&xs->ptr.p_double[i]<=scaledbndl->ptr.p_double[i] )
        {
            xu->ptr.p_double[i] = state->bndl.ptr.p_double[i];
            continue;
        }
        if( state->hasbndu.ptr.p_bool[i]&&xs->ptr.p_double[i]>=scaledbndu->ptr.p_double[i] )
        {
            xu->ptr.p_double[i] = state->bndu.ptr.p_double[i];
            continue;
        }
        xu->ptr.p_double[i] = xs->ptr.p_double[i]*state->s.ptr.p_double[i];
        if( state->hasbndl.ptr.p_bool[i]&&xu->ptr.p_double[i]<state->bndl.ptr.p_double[i] )
        {
            xu->ptr.p_double[i] = state->bndl.ptr.p_double[i];
        }
        if( state->hasbndu.ptr.p_bool[i]&&xu->ptr.p_double[i]>state->bndu.ptr.p_double[i] )
        {
            xu->ptr.p_double[i] = state->bndu.ptr.p_double[i];
        }
    }
}


void _minnlcstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minnlcstate *p = (minnlcstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->cleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lcsrcidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    _rcommstate_init(&p->rstateaul, _state, make_automatic);
    _rcommstate_init(&p->rstateslp, _state, make_automatic);
    ae_vector_init(&p->scaledbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->scaledbndu, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->scaledcleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xstart, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dfbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fm2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fm1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dfm1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dfp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bufd, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bufc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->bufw, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->bufz, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xk1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gk1, 0, DT_REAL, _state, make_automatic);
    _minlbfgsstate_init(&p->auloptimizer, _state, make_automatic);
    _minlbfgsreport_init(&p->aulreport, _state, make_automatic);
    ae_vector_init(&p->nubc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nulc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nunlc, 0, DT_REAL, _state, make_automatic);
    _minslpstate_init(&p->slpsolverstate, _state, make_automatic);
    _minsqpstate_init(&p->sqpsolverstate, _state, make_automatic);
    _smoothnessmonitor_init(&p->smonitor, _state, make_automatic);
    ae_vector_init(&p->lastscaleused, 0, DT_REAL, _state, make_automatic);
}


void _minnlcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minnlcstate       *dst = (minnlcstate*)_dst;
    const minnlcstate *src = (const minnlcstate*)_src;
    dst->stabilizingpoint = src->stabilizingpoint;
    dst->initialinequalitymultiplier = src->initialinequalitymultiplier;
    dst->solvertype = src->solvertype;
    dst->prectype = src->prectype;
    dst->updatefreq = src->updatefreq;
    dst->rho = src->rho;
    dst->n = src->n;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->aulitscnt = src->aulitscnt;
    dst->xrep = src->xrep;
    dst->stpmax = src->stpmax;
    dst->diffstep = src->diffstep;
    dst->teststep = src->teststep;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    dst->nec = src->nec;
    dst->nic = src->nic;
    ae_matrix_init_copy(&dst->cleic, &src->cleic, _state, make_automatic);
    ae_vector_init_copy(&dst->lcsrcidx, &src->lcsrcidx, _state, make_automatic);
    dst->ng = src->ng;
    dst->nh = src->nh;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    dst->needfij = src->needfij;
    dst->needfi = src->needfi;
    dst->xupdated = src->xupdated;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstateaul, &src->rstateaul, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstateslp, &src->rstateslp, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndl, &src->scaledbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndu, &src->scaledbndu, _state, make_automatic);
    ae_matrix_init_copy(&dst->scaledcleic, &src->scaledcleic, _state, make_automatic);
    ae_vector_init_copy(&dst->xc, &src->xc, _state, make_automatic);
    ae_vector_init_copy(&dst->xstart, &src->xstart, _state, make_automatic);
    ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic);
    ae_vector_init_copy(&dst->fbase, &src->fbase, _state, make_automatic);
    ae_vector_init_copy(&dst->dfbase, &src->dfbase, _state, make_automatic);
    ae_vector_init_copy(&dst->fm2, &src->fm2, _state, make_automatic);
    ae_vector_init_copy(&dst->fm1, &src->fm1, _state, make_automatic);
    ae_vector_init_copy(&dst->fp1, &src->fp1, _state, make_automatic);
    ae_vector_init_copy(&dst->fp2, &src->fp2, _state, make_automatic);
    ae_vector_init_copy(&dst->dfm1, &src->dfm1, _state, make_automatic);
    ae_vector_init_copy(&dst->dfp1, &src->dfp1, _state, make_automatic);
    ae_vector_init_copy(&dst->bufd, &src->bufd, _state, make_automatic);
    ae_vector_init_copy(&dst->bufc, &src->bufc, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_matrix_init_copy(&dst->bufw, &src->bufw, _state, make_automatic);
    ae_matrix_init_copy(&dst->bufz, &src->bufz, _state, make_automatic);
    ae_vector_init_copy(&dst->xk, &src->xk, _state, make_automatic);
    ae_vector_init_copy(&dst->xk1, &src->xk1, _state, make_automatic);
    ae_vector_init_copy(&dst->gk, &src->gk, _state, make_automatic);
    ae_vector_init_copy(&dst->gk1, &src->gk1, _state, make_automatic);
    dst->gammak = src->gammak;
    dst->xkpresent = src->xkpresent;
    _minlbfgsstate_init_copy(&dst->auloptimizer, &src->auloptimizer, _state, make_automatic);
    _minlbfgsreport_init_copy(&dst->aulreport, &src->aulreport, _state, make_automatic);
    ae_vector_init_copy(&dst->nubc, &src->nubc, _state, make_automatic);
    ae_vector_init_copy(&dst->nulc, &src->nulc, _state, make_automatic);
    ae_vector_init_copy(&dst->nunlc, &src->nunlc, _state, make_automatic);
    dst->userterminationneeded = src->userterminationneeded;
    _minslpstate_init_copy(&dst->slpsolverstate, &src->slpsolverstate, _state, make_automatic);
    _minsqpstate_init_copy(&dst->sqpsolverstate, &src->sqpsolverstate, _state, make_automatic);
    dst->smoothnessguardlevel = src->smoothnessguardlevel;
    _smoothnessmonitor_init_copy(&dst->smonitor, &src->smonitor, _state, make_automatic);
    ae_vector_init_copy(&dst->lastscaleused, &src->lastscaleused, _state, make_automatic);
    dst->repinneriterationscount = src->repinneriterationscount;
    dst->repouteriterationscount = src->repouteriterationscount;
    dst->repnfev = src->repnfev;
    dst->repterminationtype = src->repterminationtype;
    dst->repbcerr = src->repbcerr;
    dst->repbcidx = src->repbcidx;
    dst->replcerr = src->replcerr;
    dst->replcidx = src->replcidx;
    dst->repnlcerr = src->repnlcerr;
    dst->repnlcidx = src->repnlcidx;
    dst->repdbgphase0its = src->repdbgphase0its;
}


void _minnlcstate_clear(void* _p)
{
    minnlcstate *p = (minnlcstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_matrix_clear(&p->cleic);
    ae_vector_clear(&p->lcsrcidx);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    _rcommstate_clear(&p->rstate);
    _rcommstate_clear(&p->rstateaul);
    _rcommstate_clear(&p->rstateslp);
    ae_vector_clear(&p->scaledbndl);
    ae_vector_clear(&p->scaledbndu);
    ae_matrix_clear(&p->scaledcleic);
    ae_vector_clear(&p->xc);
    ae_vector_clear(&p->xstart);
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->fbase);
    ae_vector_clear(&p->dfbase);
    ae_vector_clear(&p->fm2);
    ae_vector_clear(&p->fm1);
    ae_vector_clear(&p->fp1);
    ae_vector_clear(&p->fp2);
    ae_vector_clear(&p->dfm1);
    ae_vector_clear(&p->dfp1);
    ae_vector_clear(&p->bufd);
    ae_vector_clear(&p->bufc);
    ae_vector_clear(&p->tmp0);
    ae_matrix_clear(&p->bufw);
    ae_matrix_clear(&p->bufz);
    ae_vector_clear(&p->xk);
    ae_vector_clear(&p->xk1);
    ae_vector_clear(&p->gk);
    ae_vector_clear(&p->gk1);
    _minlbfgsstate_clear(&p->auloptimizer);
    _minlbfgsreport_clear(&p->aulreport);
    ae_vector_clear(&p->nubc);
    ae_vector_clear(&p->nulc);
    ae_vector_clear(&p->nunlc);
    _minslpstate_clear(&p->slpsolverstate);
    _minsqpstate_clear(&p->sqpsolverstate);
    _smoothnessmonitor_clear(&p->smonitor);
    ae_vector_clear(&p->lastscaleused);
}


void _minnlcstate_destroy(void* _p)
{
    minnlcstate *p = (minnlcstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_matrix_destroy(&p->cleic);
    ae_vector_destroy(&p->lcsrcidx);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    _rcommstate_destroy(&p->rstate);
    _rcommstate_destroy(&p->rstateaul);
    _rcommstate_destroy(&p->rstateslp);
    ae_vector_destroy(&p->scaledbndl);
    ae_vector_destroy(&p->scaledbndu);
    ae_matrix_destroy(&p->scaledcleic);
    ae_vector_destroy(&p->xc);
    ae_vector_destroy(&p->xstart);
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->fbase);
    ae_vector_destroy(&p->dfbase);
    ae_vector_destroy(&p->fm2);
    ae_vector_destroy(&p->fm1);
    ae_vector_destroy(&p->fp1);
    ae_vector_destroy(&p->fp2);
    ae_vector_destroy(&p->dfm1);
    ae_vector_destroy(&p->dfp1);
    ae_vector_destroy(&p->bufd);
    ae_vector_destroy(&p->bufc);
    ae_vector_destroy(&p->tmp0);
    ae_matrix_destroy(&p->bufw);
    ae_matrix_destroy(&p->bufz);
    ae_vector_destroy(&p->xk);
    ae_vector_destroy(&p->xk1);
    ae_vector_destroy(&p->gk);
    ae_vector_destroy(&p->gk1);
    _minlbfgsstate_destroy(&p->auloptimizer);
    _minlbfgsreport_destroy(&p->aulreport);
    ae_vector_destroy(&p->nubc);
    ae_vector_destroy(&p->nulc);
    ae_vector_destroy(&p->nunlc);
    _minslpstate_destroy(&p->slpsolverstate);
    _minsqpstate_destroy(&p->sqpsolverstate);
    _smoothnessmonitor_destroy(&p->smonitor);
    ae_vector_destroy(&p->lastscaleused);
}


void _minnlcreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minnlcreport *p = (minnlcreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minnlcreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minnlcreport       *dst = (minnlcreport*)_dst;
    const minnlcreport *src = (const minnlcreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfev = src->nfev;
    dst->terminationtype = src->terminationtype;
    dst->bcerr = src->bcerr;
    dst->bcidx = src->bcidx;
    dst->lcerr = src->lcerr;
    dst->lcidx = src->lcidx;
    dst->nlcerr = src->nlcerr;
    dst->nlcidx = src->nlcidx;
    dst->dbgphase0its = src->dbgphase0its;
}


void _minnlcreport_clear(void* _p)
{
    minnlcreport *p = (minnlcreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minnlcreport_destroy(void* _p)
{
    minnlcreport *p = (minnlcreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
