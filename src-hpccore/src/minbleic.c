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
#include "minbleic.h"


/*$ Declarations $*/
static double minbleic_gtol = 0.4;
static double minbleic_maxnonmonotoniclen = 1.0E-7;
static double minbleic_initialdecay = 0.5;
static double minbleic_mindecay = 0.1;
static double minbleic_decaycorrection = 0.8;
static double minbleic_penaltyfactor = 100;
static void minbleic_clearrequestfields(minbleicstate* state,
     ae_state *_state);
static void minbleic_minbleicinitinternal(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minbleicstate* state,
     ae_state *_state);
static void minbleic_updateestimateofgoodstep(double* estimate,
     double newstep,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
                     BOUND CONSTRAINED OPTIMIZATION
       WITH ADDITIONAL LINEAR EQUALITY AND INEQUALITY CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BLEIC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBLEICCreate() call

2. USer adds boundary and/or linear constraints by calling
   MinBLEICSetBC() and MinBLEICSetLC() functions.

3. User sets stopping conditions with MinBLEICSetCond().

4. User calls MinBLEICOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBLEICResults() to get solution

6. Optionally user may call MinBLEICRestartFrom() to solve another problem
   with same N but another starting point.
   MinBLEICRestartFrom() allows to reuse already initialized structure.

NOTE: if you have box-only constraints (no  general  linear  constraints),
      then MinBC optimizer can be better option. It uses  special,  faster
      constraint activation method, which performs better on problems with
      multiple constraints active at the solution.
      
      On small-scale problems performance of MinBC is similar to  that  of
      MinBLEIC, but on large-scale ones (hundreds and thousands of  active
      constraints) it can be several times faster than MinBLEIC.

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
void minbleiccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minbleicstate* state,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    memset(&c, 0, sizeof(c));
    memset(&ct, 0, sizeof(ct));
    _minbleicstate_clear(state);
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    ae_assert(n>=1, "MinBLEICCreate: N<1", _state);
    ae_assert(x->cnt>=n, "MinBLEICCreate: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinBLEICCreate: X contains infinite or NaN values!", _state);
    minbleic_minbleicinitinternal(n, x, 0.0, state, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
The subroutine is finite difference variant of MinBLEICCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBLEICCreate() in  order  to  get
more information about creation of BLEIC optimizer.

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
   S[] is scaling vector which can be set by MinBLEICSetScale() call.
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
void minbleiccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minbleicstate* state,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix c;
    ae_vector ct;

    ae_frame_make(_state, &_frame_block);
    memset(&c, 0, sizeof(c));
    memset(&ct, 0, sizeof(ct));
    _minbleicstate_clear(state);
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ct, 0, DT_INT, _state, ae_true);

    ae_assert(n>=1, "MinBLEICCreateF: N<1", _state);
    ae_assert(x->cnt>=n, "MinBLEICCreateF: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinBLEICCreateF: X contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(diffstep, _state), "MinBLEICCreateF: DiffStep is infinite or NaN!", _state);
    ae_assert(ae_fp_greater(diffstep,(double)(0)), "MinBLEICCreateF: DiffStep is non-positive!", _state);
    minbleic_minbleicinitinternal(n, x, diffstep, state, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function sets boundary constraints for BLEIC optimizer.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

NOTE: if you have box-only constraints (no  general  linear  constraints),
      then MinBC optimizer can be better option. It uses  special,  faster
      constraint activation method, which performs better on problems with
      multiple constraints active at the solution.
      
      On small-scale problems performance of MinBC is similar to  that  of
      MinBLEIC, but on large-scale ones (hundreds and thousands of  active
      constraints) it can be several times faster than MinBLEIC.
      
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
void minbleicsetbc(minbleicstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->nmain;
    ae_assert(bndl->cnt>=n, "MinBLEICSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinBLEICSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinBLEICSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinBLEICSetBC: BndL contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
    sassetbc(&state->sas, bndl, bndu, _state);
}


/*************************************************************************
This function sets linear constraints for BLEIC optimizer.

Linear constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately:
* there always exists some minor violation (about Epsilon in magnitude)
  due to rounding errors
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.
If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetlc(minbleicstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double v;


    n = state->nmain;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(k>=0, "MinBLEICSetLC: K<0", _state);
    ae_assert(c->cols>=n+1||k==0, "MinBLEICSetLC: Cols(C)<N+1", _state);
    ae_assert(c->rows>=k, "MinBLEICSetLC: Rows(C)<K", _state);
    ae_assert(ct->cnt>=k, "MinBLEICSetLC: Length(CT)<K", _state);
    ae_assert(apservisfinitematrix(c, k, n+1, _state), "MinBLEICSetLC: C contains infinite or NaN values!", _state);
    
    /*
     * Handle zero K
     */
    if( k==0 )
    {
        state->nec = 0;
        state->nic = 0;
        sassetlc(&state->sas, c, ct, 0, _state);
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
    state->nec = 0;
    state->nic = 0;
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]==0 )
        {
            ae_v_move(&state->cleic.ptr.pp_double[state->nec][0], 1, &c->ptr.pp_double[i][0], 1, ae_v_len(0,n));
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
            state->nic = state->nic+1;
        }
    }
    
    /*
     * Normalize rows of State.CLEIC: each row must have unit norm.
     * Norm is calculated using first N elements (i.e. right part is
     * not counted when we calculate norm).
     */
    for(i=0; i<=k-1; i++)
    {
        v = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            v = v+ae_sqr(state->cleic.ptr.pp_double[i][j], _state);
        }
        if( ae_fp_eq(v,(double)(0)) )
        {
            continue;
        }
        v = (double)1/ae_sqrt(v, _state);
        ae_v_muld(&state->cleic.ptr.pp_double[i][0], 1, ae_v_len(0,n), v);
    }
    sassetlc(&state->sas, c, ct, k, _state);
}


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
                * s - scaling coefficients set by MinBLEICSetScale()
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
                * s - scaling coefficients set by MinBLEICSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection.

NOTE: when SetCond() called with non-zero MaxIts, BLEIC solver may perform
      slightly more than MaxIts iterations. I.e., MaxIts  sets  non-strict
      limit on iterations count.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetcond(minbleicstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinBLEICSetCond: EpsG is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsg,(double)(0)), "MinBLEICSetCond: negative EpsG", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinBLEICSetCond: EpsF is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsf,(double)(0)), "MinBLEICSetCond: negative EpsF", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinBLEICSetCond: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinBLEICSetCond: negative EpsX", _state);
    ae_assert(maxits>=0, "MinBLEICSetCond: negative MaxIts!", _state);
    if( ((ae_fp_eq(epsg,(double)(0))&&ae_fp_eq(epsf,(double)(0)))&&ae_fp_eq(epsx,(double)(0)))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->epsg = epsg;
    state->epsf = epsf;
    state->epsx = epsx;
    state->maxits = maxits;
}


/*************************************************************************
This function sets scaling coefficients for BLEIC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  BLEIC  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinBLEICSetPrec...()
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
void minbleicsetscale(minbleicstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->nmain, "MinBLEICSetScale: Length(S)<N", _state);
    for(i=0; i<=state->nmain-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinBLEICSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "MinBLEICSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
    sassetscale(&state->sas, s, _state);
}


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdefault(minbleicstate* state, ae_state *_state)
{


    state->prectype = 0;
}


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
void minbleicsetprecdiag(minbleicstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(d->cnt>=state->nmain, "MinBLEICSetPrecDiag: D is too short", _state);
    for(i=0; i<=state->nmain-1; i++)
    {
        ae_assert(ae_isfinite(d->ptr.p_double[i], _state), "MinBLEICSetPrecDiag: D contains infinite or NAN elements", _state);
        ae_assert(ae_fp_greater(d->ptr.p_double[i],(double)(0)), "MinBLEICSetPrecDiag: D contains non-positive elements", _state);
    }
    rvectorsetlengthatleast(&state->diagh, state->nmain, _state);
    state->prectype = 2;
    for(i=0; i<=state->nmain-1; i++)
    {
        state->diagh.ptr.p_double[i] = d->ptr.p_double[i];
    }
}


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinBLEICSetScale()
call  (before  or after MinBLEICSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecscale(minbleicstate* state, ae_state *_state)
{


    state->prectype = 3;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinBLEICOptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetxrep(minbleicstate* state,
     ae_bool needxrep,
     ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function turns on/off line search reports.
These reports are described in more details in developer-only  comments on
MinBLEICState object.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedDRep-   whether line search reports are needed or not

This function is intended for private use only. Turning it on artificially
may cause program failure.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetdrep(minbleicstate* state,
     ae_bool needdrep,
     ae_state *_state)
{


    state->drep = needdrep;
}


/*************************************************************************
This function sets maximum step length

IMPORTANT: this feature is hard to combine with preconditioning. You can't
set upper limit on step length, when you solve optimization  problem  with
linear (non-boundary) constraints AND preconditioner turned on.

When  non-boundary  constraints  are  present,  you  have to either a) use
preconditioner, or b) use upper limit on step length.  YOU CAN'T USE BOTH!
In this case algorithm will terminate with appropriate error code.

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
void minbleicsetstpmax(minbleicstate* state,
     double stpmax,
     ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinBLEICSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,(double)(0)), "MinBLEICSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************
NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either  MinBLEICCreate() for analytical gradient or  MinBLEICCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinBLEICOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinBLEICOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinBLEICOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinBLEICOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinBLEICCreateF() |     work                FAIL
   MinBLEICCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinBLEICOptimize()  version.   Attemps   to   use   such
   combination (for  example,  to  create optimizer with MinBLEICCreateF()
   and  to  pass  gradient information to MinBLEICOptimize()) will lead to
   exception being thrown. Either  you  did  not pass gradient when it WAS
   needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool minbleiciteration(minbleicstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;
    double v0;
    ae_bool b;
    ae_int_t mcinfo;
    ae_int_t actstatus;
    ae_int_t itidx;
    double penalty;
    double ginit;
    double gdecay;
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
        n = state->rstate.ia.ptr.p_int[0];
        m = state->rstate.ia.ptr.p_int[1];
        i = state->rstate.ia.ptr.p_int[2];
        j = state->rstate.ia.ptr.p_int[3];
        mcinfo = state->rstate.ia.ptr.p_int[4];
        actstatus = state->rstate.ia.ptr.p_int[5];
        itidx = state->rstate.ia.ptr.p_int[6];
        b = state->rstate.ba.ptr.p_bool[0];
        v = state->rstate.ra.ptr.p_double[0];
        vv = state->rstate.ra.ptr.p_double[1];
        v0 = state->rstate.ra.ptr.p_double[2];
        penalty = state->rstate.ra.ptr.p_double[3];
        ginit = state->rstate.ra.ptr.p_double[4];
        gdecay = state->rstate.ra.ptr.p_double[5];
    }
    else
    {
        n = 359;
        m = -58;
        i = -919;
        j = -909;
        mcinfo = 81;
        actstatus = 255;
        itidx = 74;
        b = ae_false;
        v = 809.0;
        vv = 205.0;
        v0 = -838.0;
        penalty = 939.0;
        ginit = -526.0;
        gdecay = 763.0;
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
    
    /*
     * Routine body
     */
    
    /*
     * Algorithm parameters:
     * * M          number of L-BFGS corrections.
     *              This coefficient remains fixed during iterations.
     * * GDecay     desired decrease of constrained gradient during L-BFGS iterations.
     *              This coefficient is decreased after each L-BFGS round until
     *              it reaches minimum decay.
     */
    m = ae_minint(5, state->nmain, _state);
    gdecay = minbleic_initialdecay;
    
    /*
     * Init
     */
    n = state->nmain;
    state->steepestdescentstep = ae_false;
    state->userterminationneeded = ae_false;
    state->repterminationtype = 0;
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    state->repnfev = 0;
    state->repvaridx = -1;
    state->repdebugeqerr = 0.0;
    state->repdebugfs = _state->v_nan;
    state->repdebugff = _state->v_nan;
    state->repdebugdx = _state->v_nan;
    if( ae_fp_neq(state->stpmax,(double)(0))&&state->prectype!=0 )
    {
        state->repterminationtype = -10;
        result = ae_false;
        return result;
    }
    rmatrixsetlengthatleast(&state->bufyk, m+1, n, _state);
    rmatrixsetlengthatleast(&state->bufsk, m+1, n, _state);
    rvectorsetlengthatleast(&state->bufrho, m, _state);
    rvectorsetlengthatleast(&state->buftheta, m, _state);
    rvectorsetlengthatleast(&state->tmp0, n, _state);
    smoothnessmonitorinit(&state->smonitor, &state->s, n, 1, state->smoothnessguardlevel>0, _state);
    for(i=0; i<=n-1; i++)
    {
        state->lastscaleused.ptr.p_double[i] = state->s.ptr.p_double[i];
        state->invs.ptr.p_double[i] = (double)1/state->s.ptr.p_double[i];
    }
    
    /*
     *  Check analytic derivative
     */
    minbleic_clearrequestfields(state, _state);
    if( !(ae_fp_eq(state->diffstep,(double)(0))&&ae_fp_greater(state->teststep,(double)(0))) )
    {
        goto lbl_22;
    }
lbl_24:
    if( !smoothnessmonitorcheckgradientatx0(&state->smonitor, &state->xstart, &state->s, &state->bndl, &state->bndu, ae_true, state->teststep, _state) )
    {
        goto lbl_25;
    }
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->smonitor.x.ptr.p_double[i];
    }
    state->needfg = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfg = ae_false;
    state->smonitor.fi.ptr.p_double[0] = state->f;
    for(i=0; i<=n-1; i++)
    {
        state->smonitor.j.ptr.pp_double[0][i] = state->g.ptr.p_double[i];
    }
    goto lbl_24;
lbl_25:
lbl_22:
    
    /*
     * Fill TmpPrec with current preconditioner
     */
    rvectorsetlengthatleast(&state->tmpprec, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->prectype==2 )
        {
            state->tmpprec.ptr.p_double[i] = state->diagh.ptr.p_double[i];
            continue;
        }
        if( state->prectype==3 )
        {
            state->tmpprec.ptr.p_double[i] = (double)1/ae_sqr(state->s.ptr.p_double[i], _state);
            continue;
        }
        state->tmpprec.ptr.p_double[i] = (double)(1);
    }
    sassetprecdiag(&state->sas, &state->tmpprec, _state);
    
    /*
     * Start optimization
     */
    if( !sasstartoptimization(&state->sas, &state->xstart, _state) )
    {
        state->repterminationtype = -3;
        result = ae_false;
        return result;
    }
    
    /*
     * Main cycle of BLEIC-PG algorithm
     */
    state->repterminationtype = 0;
    state->lastgoodstep = (double)(0);
    state->lastscaledgoodstep = (double)(0);
    state->maxscaledgrad = (double)(0);
    state->nonmonotoniccnt = ae_round(1.5*(double)(n+state->nic), _state)+5;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minbleic_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        goto lbl_26;
    }
    state->needfg = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfg = ae_false;
    goto lbl_27;
lbl_26:
    state->needf = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needf = ae_false;
lbl_27:
    state->fc = state->f;
    trimprepare(state->f, &state->trimthreshold, _state);
    state->repnfev = state->repnfev+1;
    if( !state->xrep )
    {
        goto lbl_28;
    }
    
    /*
     * Report current point
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fc;
    state->xupdated = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->xupdated = ae_false;
lbl_28:
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        sasstopoptimization(&state->sas, _state);
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
lbl_30:
    if( ae_false )
    {
        goto lbl_31;
    }
    
    /*
     * Preparations
     *
     * (a) calculate unconstrained gradient
     * (b) determine initial active set
     * (c) update MaxScaledGrad
     * (d) check F/G for NAN/INF, abnormally terminate algorithm if needed
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minbleic_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        goto lbl_32;
    }
    
    /*
     * Analytic gradient
     */
    state->needfg = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needfg = ae_false;
    goto lbl_33;
lbl_32:
    
    /*
     * Numerical differentiation
     */
    state->needf = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->fbase = state->f;
    i = 0;
lbl_34:
    if( i>n-1 )
    {
        goto lbl_36;
    }
    v = state->x.ptr.p_double[i];
    b = ae_false;
    if( state->hasbndl.ptr.p_bool[i] )
    {
        b = b||ae_fp_less(v-state->diffstep*state->s.ptr.p_double[i],state->bndl.ptr.p_double[i]);
    }
    if( state->hasbndu.ptr.p_bool[i] )
    {
        b = b||ae_fp_greater(v+state->diffstep*state->s.ptr.p_double[i],state->bndu.ptr.p_double[i]);
    }
    if( b )
    {
        goto lbl_37;
    }
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->fp2 = state->f;
    state->g.ptr.p_double[i] = ((double)8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/((double)6*state->diffstep*state->s.ptr.p_double[i]);
    goto lbl_38;
lbl_37:
    state->xm1 = v-state->diffstep*state->s.ptr.p_double[i];
    state->xp1 = v+state->diffstep*state->s.ptr.p_double[i];
    if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(state->xm1,state->bndl.ptr.p_double[i]) )
    {
        state->xm1 = state->bndl.ptr.p_double[i];
    }
    if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(state->xp1,state->bndu.ptr.p_double[i]) )
    {
        state->xp1 = state->bndu.ptr.p_double[i];
    }
    state->x.ptr.p_double[i] = state->xm1;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = state->xp1;
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->fp1 = state->f;
    if( ae_fp_neq(state->xm1,state->xp1) )
    {
        state->g.ptr.p_double[i] = (state->fp1-state->fm1)/(state->xp1-state->xm1);
    }
    else
    {
        state->g.ptr.p_double[i] = (double)(0);
    }
lbl_38:
    state->x.ptr.p_double[i] = v;
    i = i+1;
    goto lbl_34;
lbl_36:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_33:
    state->fc = state->f;
    ae_v_move(&state->ugc.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->cgc.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    sasreactivateconstraintsprec(&state->sas, &state->ugc, _state);
    sasconstraineddirection(&state->sas, &state->cgc, _state);
    ginit = 0.0;
    for(i=0; i<=n-1; i++)
    {
        ginit = ginit+ae_sqr(state->cgc.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    ginit = ae_sqrt(ginit, _state);
    state->maxscaledgrad = ae_maxreal(state->maxscaledgrad, ginit, _state);
    if( !ae_isfinite(ginit, _state)||!ae_isfinite(state->fc, _state) )
    {
        
        /*
         * Abnormal termination - infinities in function/gradient
         */
        sasstopoptimization(&state->sas, _state);
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        sasstopoptimization(&state->sas, _state);
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    
    /*
     * LBFGS stage:
     * * during LBFGS iterations we activate new constraints, but never
     *   deactivate already active ones.
     * * we perform at most N iterations of LBFGS before re-evaluating
     *   active set and restarting LBFGS.
     * * first iteration of LBFGS is a special - it is performed with
     *   minimum set of active constraints, algorithm termination can
     *   be performed only at this state. We call this iteration
     *  "steepest descent step".
     *
     * About termination:
     * * LBFGS iterations can be terminated because of two reasons:
     *   * "termination" - non-zero termination code in RepTerminationType,
     *     which means that optimization is done
     *   * "restart" - zero RepTerminationType, which means that we
     *     have to re-evaluate active set and resume LBFGS stage.
     * * one more option is "refresh" - to continue LBFGS iterations,
     *   but with all BFGS updates (Sk/Yk pairs) being dropped;
     *   it happens after changes in active set
     */
    state->bufsize = 0;
    state->steepestdescentstep = ae_true;
    itidx = -1;
lbl_39:
    if( itidx>=n-1 )
    {
        goto lbl_40;
    }
    
    /*
     * Increment iterations counter
     *
     * NOTE: we have strong reasons to use such complex scheme
     *       instead of just for() loop - this counter may be
     *       decreased at some occasions to perform "restart"
     *       of an iteration.
     */
    itidx = itidx+1;
    
    /*
     * At the beginning of each iteration:
     * * SAS.XC stores current point
     * * FC stores current function value
     * * UGC stores current unconstrained gradient
     * * CGC stores current constrained gradient
     * * D stores constrained step direction (calculated at this block)
     *
     *
     * Check gradient-based stopping criteria
     *
     * This stopping condition is tested only for step which is the
     * first step of LBFGS (subsequent steps may accumulate active
     * constraints thus they should NOT be used for stopping - gradient
     * may be small when constrained, but these constraints may be
     * deactivated by the subsequent steps)
     */
    if( state->steepestdescentstep&&ae_fp_less_eq(sasscaledconstrainednorm(&state->sas, &state->ugc, _state),state->epsg) )
    {
        
        /*
         * Gradient is small enough.
         * Optimization is terminated
         */
        state->repterminationtype = 4;
        goto lbl_40;
    }
    
    /*
     * 1. Calculate search direction D according to L-BFGS algorithm
     *    using constrained preconditioner to perform inner multiplication.
     * 2. Evaluate scaled length of direction D; restart LBFGS if D is zero
     *    (it may be possible that we found minimum, but it is also possible
     *    that some constraints need deactivation)
     * 3. If D is non-zero, try to use previous scaled step length as initial estimate for new step.
     */
    ae_v_move(&state->work.ptr.p_double[0], 1, &state->cgc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    for(i=state->bufsize-1; i>=0; i--)
    {
        v = ae_v_dotproduct(&state->bufsk.ptr.pp_double[i][0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->buftheta.ptr.p_double[i] = v;
        vv = v*state->bufrho.ptr.p_double[i];
        ae_v_subd(&state->work.ptr.p_double[0], 1, &state->bufyk.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), vv);
    }
    sasconstraineddirectionprec(&state->sas, &state->work, _state);
    for(i=0; i<=state->bufsize-1; i++)
    {
        v = ae_v_dotproduct(&state->bufyk.ptr.pp_double[i][0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
        vv = state->bufrho.ptr.p_double[i]*(-v+state->buftheta.ptr.p_double[i]);
        ae_v_addd(&state->work.ptr.p_double[0], 1, &state->bufsk.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), vv);
    }
    sasconstraineddirection(&state->sas, &state->work, _state);
    ae_v_moveneg(&state->d.ptr.p_double[0], 1, &state->work.ptr.p_double[0], 1, ae_v_len(0,n-1));
    v = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->d.ptr.p_double[i]/state->s.ptr.p_double[i], _state);
    }
    v = ae_sqrt(v, _state);
    if( ae_fp_eq(v,(double)(0)) )
    {
        
        /*
         * Search direction is zero.
         * If we perform "steepest descent step", algorithm is terminated.
         * Otherwise we just restart LBFGS.
         */
        if( state->steepestdescentstep )
        {
            state->repterminationtype = 4;
        }
        goto lbl_40;
    }
    ae_assert(ae_fp_greater(v,(double)(0)), "MinBLEIC: internal error", _state);
    if( ae_fp_greater(state->lastscaledgoodstep,(double)(0))&&ae_fp_greater(v,(double)(0)) )
    {
        state->stp = state->lastscaledgoodstep/v;
    }
    else
    {
        state->stp = 1.0/v;
    }
    
    /*
     * Calculate bound on step length.
     * Step direction is stored
     */
    sasexploredirection(&state->sas, &state->d, &state->curstpmax, &state->cidx, &state->cval, _state);
    state->activationstep = state->curstpmax;
    if( state->cidx>=0&&ae_fp_eq(state->activationstep,(double)(0)) )
    {
        
        /*
         * We are exactly at the boundary, immediate activation
         * of constraint is required. LBFGS stage is continued
         * with "refreshed" model.
         *
         * ! IMPORTANT: we do not clear SteepestDescent flag here,
         * !            it is very important for correct stopping
         * !            of algorithm.
         *
         * ! IMPORTANT: we decrease iteration counter in order to
         *              preserve computational budget for iterations.
         */
        sasimmediateactivation(&state->sas, state->cidx, state->cval, _state);
        state->bufsize = 0;
        itidx = itidx-1;
        goto lbl_39;
    }
    if( ae_fp_greater(state->stpmax,(double)(0)) )
    {
        v = ae_v_dotproduct(&state->d.ptr.p_double[0], 1, &state->d.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = ae_sqrt(v, _state);
        if( ae_fp_greater(v,(double)(0)) )
        {
            state->curstpmax = ae_minreal(state->curstpmax, state->stpmax/v, _state);
        }
    }
    
    /*
     * Report beginning of line search (if requested by caller).
     * See description of the MinBLEICState for more information
     * about fields accessible to caller.
     *
     * Caller may do following:
     * * change State.Stp and load better initial estimate of
     *   the step length.
     * Caller may not terminate algorithm.
     */
    if( !state->drep )
    {
        goto lbl_41;
    }
    minbleic_clearrequestfields(state, _state);
    state->lsstart = ae_true;
    state->boundedstep = state->cidx>=0;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    state->lsstart = ae_false;
lbl_41:
    
    /*
     * Minimize F(x+alpha*d)
     */
    ae_v_move(&state->xn.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->cgn.ptr.p_double[0], 1, &state->cgc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->ugn.ptr.p_double[0], 1, &state->ugc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fn = state->fc;
    state->mcstage = 0;
    smoothnessmonitorstartlinesearch1u(&state->smonitor, &state->s, &state->invs, &state->xn, state->fn, &state->ugn, state->repinneriterationscount, -1, _state);
    mcsrch(n, &state->xn, &state->fn, &state->ugn, &state->d, &state->stp, state->curstpmax, minbleic_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
lbl_43:
    if( state->mcstage==0 )
    {
        goto lbl_44;
    }
    
    /*
     * Perform correction (constraints are enforced)
     * Copy XN to X
     */
    sascorrection(&state->sas, &state->xn, &penalty, _state);
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->xn.ptr.p_double[i];
    }
    
    /*
     * Gradient, either user-provided or numerical differentiation
     */
    minbleic_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        goto lbl_45;
    }
    
    /*
     * Analytic gradient
     */
    state->needfg = ae_true;
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->needfg = ae_false;
    state->repnfev = state->repnfev+1;
    goto lbl_46;
lbl_45:
    
    /*
     * Numerical differentiation
     */
    state->needf = ae_true;
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->fbase = state->f;
    i = 0;
lbl_47:
    if( i>n-1 )
    {
        goto lbl_49;
    }
    v = state->x.ptr.p_double[i];
    b = ae_false;
    if( state->hasbndl.ptr.p_bool[i] )
    {
        b = b||ae_fp_less(v-state->diffstep*state->s.ptr.p_double[i],state->bndl.ptr.p_double[i]);
    }
    if( state->hasbndu.ptr.p_bool[i] )
    {
        b = b||ae_fp_greater(v+state->diffstep*state->s.ptr.p_double[i],state->bndu.ptr.p_double[i]);
    }
    if( b )
    {
        goto lbl_50;
    }
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 17;
    goto lbl_rcomm;
lbl_17:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 18;
    goto lbl_rcomm;
lbl_18:
    state->fp2 = state->f;
    state->g.ptr.p_double[i] = ((double)8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/((double)6*state->diffstep*state->s.ptr.p_double[i]);
    state->repnfev = state->repnfev+4;
    goto lbl_51;
lbl_50:
    state->xm1 = v-state->diffstep*state->s.ptr.p_double[i];
    state->xp1 = v+state->diffstep*state->s.ptr.p_double[i];
    if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(state->xm1,state->bndl.ptr.p_double[i]) )
    {
        state->xm1 = state->bndl.ptr.p_double[i];
    }
    if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(state->xp1,state->bndu.ptr.p_double[i]) )
    {
        state->xp1 = state->bndu.ptr.p_double[i];
    }
    state->x.ptr.p_double[i] = state->xm1;
    state->rstate.stage = 19;
    goto lbl_rcomm;
lbl_19:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = state->xp1;
    state->rstate.stage = 20;
    goto lbl_rcomm;
lbl_20:
    state->fp1 = state->f;
    if( ae_fp_neq(state->xm1,state->xp1) )
    {
        state->g.ptr.p_double[i] = (state->fp1-state->fm1)/(state->xp1-state->xm1);
    }
    else
    {
        state->g.ptr.p_double[i] = (double)(0);
    }
    state->repnfev = state->repnfev+2;
lbl_51:
    state->x.ptr.p_double[i] = v;
    i = i+1;
    goto lbl_47;
lbl_49:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_46:
    
    /*
     * Back to MCSRCH
     *
     * NOTE: penalty term from correction is added to FN in order
     *       to penalize increase in infeasibility.
     */
    smoothnessmonitorenqueuepoint1u(&state->smonitor, &state->s, &state->invs, &state->d, state->stp, &state->x, state->f, &state->g, _state);
    state->fn = state->f+minbleic_penaltyfactor*state->maxscaledgrad*penalty;
    ae_v_move(&state->cgn.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->ugn.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    sasconstraineddirection(&state->sas, &state->cgn, _state);
    trimfunction(&state->fn, &state->cgn, n, state->trimthreshold, _state);
    mcsrch(n, &state->xn, &state->fn, &state->ugn, &state->d, &state->stp, state->curstpmax, minbleic_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
    goto lbl_43;
lbl_44:
    ae_v_moveneg(&state->bufsk.ptr.pp_double[state->bufsize][0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_moveneg(&state->bufyk.ptr.pp_double[state->bufsize][0], 1, &state->cgc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_add(&state->bufsk.ptr.pp_double[state->bufsize][0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_add(&state->bufyk.ptr.pp_double[state->bufsize][0], 1, &state->cgn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    smoothnessmonitorfinalizelinesearch(&state->smonitor, _state);
    
    /*
     * Check for presence of NAN/INF in function/gradient
     */
    v = state->fn;
    for(i=0; i<=n-1; i++)
    {
        v = 0.1*v+state->ugn.ptr.p_double[i];
    }
    if( !ae_isfinite(v, _state) )
    {
        
        /*
         * Abnormal termination - infinities in function/gradient
         */
        state->repterminationtype = -8;
        goto lbl_40;
    }
    
    /*
     * Handle possible failure of the line search or request for termination
     */
    if( mcinfo!=1&&mcinfo!=5 )
    {
        
        /*
         * We can not find step which decreases function value. We have
         * two possibilities:
         * (a) numerical properties of the function do not allow us to
         *     find good step.
         * (b) we are close to activation of some constraint, and it is
         *     so close that step which activates it leads to change in
         *     target function which is smaller than numerical noise.
         *
         * Optimization algorithm must be able to handle case (b), because
         * inability to handle it will cause failure when algorithm
         * started very close to boundary of the feasible area.
         *
         * In order to correctly handle such cases we allow limited amount
         * of small steps which increase function value.
         */
        v = 0.0;
        for(i=0; i<=n-1; i++)
        {
            v = v+ae_sqr(state->d.ptr.p_double[i]*state->curstpmax/state->s.ptr.p_double[i], _state);
        }
        v = ae_sqrt(v, _state);
        b = ae_false;
        if( (state->cidx>=0&&ae_fp_less_eq(v,minbleic_maxnonmonotoniclen))&&state->nonmonotoniccnt>0 )
        {
            
            /*
             * We try to enforce non-monotonic step:
             * * Stp    := CurStpMax
             * * MCINFO := 5
             * * XN     := XC+CurStpMax*D
             * * non-monotonic counter is decreased
             *
             * NOTE: UGN/CGN are not updated because step is so short that we assume that
             *       GN is approximately equal to GC.
             *
             * NOTE: prior to enforcing such step we check that it does not increase infeasibility
             *       of constraints beyond tolerable level
             */
            v = state->curstpmax;
            ae_v_move(&state->tmp0.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
            ae_v_addd(&state->tmp0.ptr.p_double[0], 1, &state->d.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
            state->stp = state->curstpmax;
            mcinfo = 5;
            ae_v_move(&state->xn.ptr.p_double[0], 1, &state->tmp0.ptr.p_double[0], 1, ae_v_len(0,n-1));
            state->nonmonotoniccnt = state->nonmonotoniccnt-1;
            b = ae_true;
        }
        if( !b )
        {
            
            /*
             * Numerical properties of the function do not allow
             * us to solve problem. Here we have two possibilities:
             * * if it is "steepest descent" step, we can terminate
             *   algorithm because we are close to minimum
             * * if it is NOT "steepest descent" step, we should restart
             *   LBFGS iterations.
             */
            if( state->steepestdescentstep )
            {
                
                /*
                 * Algorithm is terminated
                 */
                state->repterminationtype = 7;
                goto lbl_40;
            }
            else
            {
                
                /*
                 * Re-evaluate active set and restart LBFGS
                 */
                goto lbl_40;
            }
        }
    }
    if( state->userterminationneeded )
    {
        goto lbl_40;
    }
    
    /*
     * Current point is updated:
     * * move XC/FC/GC to XP/FP/GP
     * * change current point remembered by SAS structure
     * * move XN/FN/GN to XC/FC/GC
     * * report current point and update iterations counter
     * * if MCINFO=1, push new pair SK/YK to LBFGS buffer
     */
    state->fp = state->fc;
    ae_v_move(&state->xp.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fc = state->fn;
    ae_v_move(&state->cgc.ptr.p_double[0], 1, &state->cgn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->ugc.ptr.p_double[0], 1, &state->ugn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    actstatus = sasmoveto(&state->sas, &state->xn, state->cidx>=0&&ae_fp_greater_eq(state->stp,state->activationstep), state->cidx, state->cval, _state);
    if( !state->xrep )
    {
        goto lbl_52;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minbleic_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 21;
    goto lbl_rcomm;
lbl_21:
    state->xupdated = ae_false;
lbl_52:
    state->repinneriterationscount = state->repinneriterationscount+1;
    if( mcinfo==1 )
    {
        
        /*
         * Accept new LBFGS update given by Sk,Yk
         */
        if( state->bufsize==m )
        {
            
            /*
             * Buffer is full, shift contents by one row
             */
            for(i=0; i<=state->bufsize-1; i++)
            {
                ae_v_move(&state->bufsk.ptr.pp_double[i][0], 1, &state->bufsk.ptr.pp_double[i+1][0], 1, ae_v_len(0,n-1));
                ae_v_move(&state->bufyk.ptr.pp_double[i][0], 1, &state->bufyk.ptr.pp_double[i+1][0], 1, ae_v_len(0,n-1));
            }
            for(i=0; i<=state->bufsize-2; i++)
            {
                state->bufrho.ptr.p_double[i] = state->bufrho.ptr.p_double[i+1];
                state->buftheta.ptr.p_double[i] = state->buftheta.ptr.p_double[i+1];
            }
        }
        else
        {
            
            /*
             * Buffer is not full, increase buffer size by 1
             */
            state->bufsize = state->bufsize+1;
        }
        v = ae_v_dotproduct(&state->bufyk.ptr.pp_double[state->bufsize-1][0], 1, &state->bufsk.ptr.pp_double[state->bufsize-1][0], 1, ae_v_len(0,n-1));
        vv = ae_v_dotproduct(&state->bufyk.ptr.pp_double[state->bufsize-1][0], 1, &state->bufyk.ptr.pp_double[state->bufsize-1][0], 1, ae_v_len(0,n-1));
        if( ae_fp_eq(v,(double)(0))||ae_fp_eq(vv,(double)(0)) )
        {
            
            /*
             * Strange internal error in LBFGS - either YK=0
             * (which should not have been) or (SK,YK)=0 (again,
             * unexpected). It should not take place because
             * MCINFO=1, which signals "good" step. But just
             * to be sure we have special branch of code which
             * restarts LBFGS
             */
            goto lbl_40;
        }
        state->bufrho.ptr.p_double[state->bufsize-1] = (double)1/v;
        ae_assert(state->bufsize<=m, "MinBLEIC: internal error", _state);
        
        /*
         * Update length of the good step
         */
        v = (double)(0);
        vv = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            v = v+ae_sqr((state->sas.xc.ptr.p_double[i]-state->xp.ptr.p_double[i])/state->s.ptr.p_double[i], _state);
            vv = vv+ae_sqr(state->sas.xc.ptr.p_double[i]-state->xp.ptr.p_double[i], _state);
        }
        state->lastgoodstep = ae_sqrt(vv, _state);
        minbleic_updateestimateofgoodstep(&state->lastscaledgoodstep, ae_sqrt(v, _state), _state);
    }
    
    /*
     * Check stopping criteria
     *
     * Step size and function-based stopping criteria are tested only
     * for step which satisfies Wolfe conditions and is the first step of
     * LBFGS (subsequent steps may accumulate active constraints thus
     * they should NOT be used for stopping; step size or function change
     * may be small when constrained, but these constraints may be
     * deactivated by the subsequent steps).
     *
     * MaxIts-based stopping condition is checked for all kinds of steps.
     */
    if( mcinfo==1&&state->steepestdescentstep )
    {
        
        /*
         * Step is small enough
         */
        v = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            v = v+ae_sqr((state->sas.xc.ptr.p_double[i]-state->xp.ptr.p_double[i])/state->s.ptr.p_double[i], _state);
        }
        v = ae_sqrt(v, _state);
        if( ae_fp_less_eq(v,state->epsx) )
        {
            state->repterminationtype = 2;
            goto lbl_40;
        }
        
        /*
         * Function change is small enough
         */
        if( ae_fp_less_eq(ae_fabs(state->fp-state->fc, _state),state->epsf*ae_maxreal(ae_fabs(state->fc, _state), ae_maxreal(ae_fabs(state->fp, _state), 1.0, _state), _state)) )
        {
            state->repterminationtype = 1;
            goto lbl_40;
        }
    }
    if( state->maxits>0&&state->repinneriterationscount>=state->maxits )
    {
        state->repterminationtype = 5;
        goto lbl_40;
    }
    
    /*
     * Clear "steepest descent" flag.
     */
    state->steepestdescentstep = ae_false;
    
    /*
     * Smooth reset (LBFGS memory model is refreshed) or hard restart:
     * * LBFGS model is refreshed, if line search was performed with activation of constraints
     * * algorithm is restarted if scaled gradient decreased below GDecay
     */
    if( actstatus>=0 )
    {
        state->bufsize = 0;
        goto lbl_39;
    }
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->cgc.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    if( ae_fp_less(ae_sqrt(v, _state),gdecay*ginit) )
    {
        goto lbl_40;
    }
    goto lbl_39;
lbl_40:
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        state->repterminationtype = 8;
        goto lbl_31;
    }
    if( state->repterminationtype!=0 )
    {
        
        /*
         * Algorithm terminated
         */
        goto lbl_31;
    }
    
    /*
     * Decrease decay coefficient. Subsequent L-BFGS stages will
     * have more stringent stopping criteria.
     */
    gdecay = ae_maxreal(gdecay*minbleic_decaycorrection, minbleic_mindecay, _state);
    goto lbl_30;
lbl_31:
    sasstopoptimization(&state->sas, _state);
    state->repouteriterationscount = 1;
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = i;
    state->rstate.ia.ptr.p_int[3] = j;
    state->rstate.ia.ptr.p_int[4] = mcinfo;
    state->rstate.ia.ptr.p_int[5] = actstatus;
    state->rstate.ia.ptr.p_int[6] = itidx;
    state->rstate.ba.ptr.p_bool[0] = b;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vv;
    state->rstate.ra.ptr.p_double[2] = v0;
    state->rstate.ra.ptr.p_double[3] = penalty;
    state->rstate.ra.ptr.p_double[4] = ginit;
    state->rstate.ra.ptr.p_double[5] = gdecay;
    return result;
}


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

The primary OptGuard report can be retrieved with minbleicoptguardresults().

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
                    means of setting scale with minbleicsetscale().

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
void minbleicoptguardgradient(minbleicstate* state,
     double teststep,
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "MinBLEICOptGuardGradient: TestStep contains NaN or INF", _state);
    ae_assert(ae_fp_greater_eq(teststep,(double)(0)), "MinBLEICOptGuardGradient: invalid argument TestStep(TestStep<0)", _state);
    state->teststep = teststep;
}


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
void minbleicoptguardsmoothness(minbleicstate* state,
     ae_int_t level,
     ae_state *_state)
{


    ae_assert(level==0||level==1, "MinBLEICOptGuardSmoothness: unexpected value of level parameter", _state);
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
* minbleicoptguardgradient() for gradient verification
* minbleicoptguardsmoothness() for C0/C1 checks

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
* minbleicoptguardnonc1test0results()
* minbleicoptguardnonc1test1results()
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
void minbleicoptguardresults(minbleicstate* state,
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
void minbleicoptguardnonc1test0results(const minbleicstate* state,
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
void minbleicoptguardnonc1test1results(minbleicstate* state,
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
BLEIC results

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
                * -3   inconsistent constraints. Feasible point is
                       either nonexistent or too hard to find. Try to
                       restart optimizer with better initial approximation
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken
                *  8   terminated by user who called minbleicrequesttermination().
                       X contains point which was "current accepted"  when
                       termination request was submitted.
                More information about fields of this  structure  can  be
                found in the comments on MinBLEICReport datatype.
   
  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresults(const minbleicstate* state,
     /* Real    */ ae_vector* x,
     minbleicreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _minbleicreport_clear(rep);

    minbleicresultsbuf(state, x, rep, _state);
}


/*************************************************************************
BLEIC results

Buffered implementation of MinBLEICResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresultsbuf(const minbleicstate* state,
     /* Real    */ ae_vector* x,
     minbleicreport* rep,
     ae_state *_state)
{
    ae_int_t i;


    if( x->cnt<state->nmain )
    {
        ae_vector_set_length(x, state->nmain, _state);
    }
    rep->iterationscount = state->repinneriterationscount;
    rep->inneriterationscount = state->repinneriterationscount;
    rep->outeriterationscount = state->repouteriterationscount;
    rep->nfev = state->repnfev;
    rep->varidx = state->repvaridx;
    rep->terminationtype = state->repterminationtype;
    if( state->repterminationtype>0 )
    {
        ae_v_move(&x->ptr.p_double[0], 1, &state->sas.xc.ptr.p_double[0], 1, ae_v_len(0,state->nmain-1));
    }
    else
    {
        for(i=0; i<=state->nmain-1; i++)
        {
            x->ptr.p_double[i] = _state->v_nan;
        }
    }
    rep->debugeqerr = state->repdebugeqerr;
    rep->debugfs = state->repdebugfs;
    rep->debugff = state->repdebugff;
    rep->debugdx = state->repdebugdx;
    rep->debugfeasqpits = state->repdebugfeasqpits;
    rep->debugfeasgpaits = state->repdebugfeasgpaits;
}


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicrestartfrom(minbleicstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;


    n = state->nmain;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(x->cnt>=n, "MinBLEICRestartFrom: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinBLEICRestartFrom: X contains infinite or NaN values!", _state);
    
    /*
     * Set XC
     */
    ae_v_move(&state->xstart.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * prepare RComm facilities
     */
    ae_vector_set_length(&state->rstate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 5+1, _state);
    state->rstate.stage = -1;
    minbleic_clearrequestfields(state, _state);
    sasstopoptimization(&state->sas, _state);
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
void minbleicrequesttermination(minbleicstate* state, ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


/*************************************************************************
This subroutine finalizes internal structures after emergency  termination
from State.LSStart report (see comments on MinBLEICState for more information).

INPUT PARAMETERS:
    State   -   structure after exit from LSStart report

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicemergencytermination(minbleicstate* state, ae_state *_state)
{


    sasstopoptimization(&state->sas, _state);
}


/*************************************************************************
Clears request fileds (to be sure that we don't forget to clear something)
*************************************************************************/
static void minbleic_clearrequestfields(minbleicstate* state,
     ae_state *_state)
{


    state->needf = ae_false;
    state->needfg = ae_false;
    state->xupdated = ae_false;
    state->lsstart = ae_false;
}


/*************************************************************************
Internal initialization subroutine
*************************************************************************/
static void minbleic_minbleicinitinternal(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minbleicstate* state,
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
     * Initialize
     */
    state->teststep = (double)(0);
    state->smoothnessguardlevel = 0;
    smoothnessmonitorinit(&state->smonitor, &state->s, 0, 0, ae_false, _state);
    state->nmain = n;
    state->diffstep = diffstep;
    sasinit(n, &state->sas, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->hasbndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->hasbndu, n, _state);
    ae_vector_set_length(&state->xstart, n, _state);
    ae_vector_set_length(&state->cgc, n, _state);
    ae_vector_set_length(&state->ugc, n, _state);
    ae_vector_set_length(&state->xn, n, _state);
    ae_vector_set_length(&state->cgn, n, _state);
    ae_vector_set_length(&state->ugn, n, _state);
    ae_vector_set_length(&state->xp, n, _state);
    ae_vector_set_length(&state->d, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->invs, n, _state);
    ae_vector_set_length(&state->lastscaleused, n, _state);
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->g, n, _state);
    ae_vector_set_length(&state->work, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->hasbndl.ptr.p_bool[i] = ae_false;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
        state->hasbndu.ptr.p_bool[i] = ae_false;
        state->s.ptr.p_double[i] = 1.0;
        state->invs.ptr.p_double[i] = 1.0;
        state->lastscaleused.ptr.p_double[i] = 1.0;
    }
    minbleicsetlc(state, &c, &ct, 0, _state);
    minbleicsetcond(state, 0.0, 0.0, 0.0, 0, _state);
    minbleicsetxrep(state, ae_false, _state);
    minbleicsetdrep(state, ae_false, _state);
    minbleicsetstpmax(state, 0.0, _state);
    minbleicsetprecdefault(state, _state);
    minbleicrestartfrom(state, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This subroutine updates estimate of the good step length given:
1) previous estimate
2) new length of the good step

It makes sure that estimate does not change too rapidly - ratio of new and
old estimates will be at least 0.01, at most 100.0

In case previous estimate of good step is zero (no estimate), new estimate
is used unconditionally.

  -- ALGLIB --
     Copyright 16.01.2013 by Bochkanov Sergey
*************************************************************************/
static void minbleic_updateestimateofgoodstep(double* estimate,
     double newstep,
     ae_state *_state)
{


    if( ae_fp_eq(*estimate,(double)(0)) )
    {
        *estimate = newstep;
        return;
    }
    if( ae_fp_less(newstep,*estimate*0.01) )
    {
        *estimate = *estimate*0.01;
        return;
    }
    if( ae_fp_greater(newstep,*estimate*(double)100) )
    {
        *estimate = *estimate*(double)100;
        return;
    }
    *estimate = newstep;
}


void _minbleicstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minbleicstate *p = (minbleicstate*)_p;
    ae_touch_ptr((void*)p);
    _sactiveset_init(&p->sas, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagh, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_vector_init(&p->ugc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cgc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ugn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cgn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->cleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xstart, 0, DT_REAL, _state, make_automatic);
    _snnlssolver_init(&p->solver, _state, make_automatic);
    ae_vector_init(&p->tmpprec, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->work, 0, DT_REAL, _state, make_automatic);
    _linminstate_init(&p->lstate, _state, make_automatic);
    ae_matrix_init(&p->bufyk, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->bufsk, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bufrho, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->buftheta, 0, DT_REAL, _state, make_automatic);
    _smoothnessmonitor_init(&p->smonitor, _state, make_automatic);
    ae_vector_init(&p->lastscaleused, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->invs, 0, DT_REAL, _state, make_automatic);
}


void _minbleicstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minbleicstate       *dst = (minbleicstate*)_dst;
    const minbleicstate *src = (const minbleicstate*)_src;
    dst->nmain = src->nmain;
    dst->nslack = src->nslack;
    dst->epsg = src->epsg;
    dst->epsf = src->epsf;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->drep = src->drep;
    dst->stpmax = src->stpmax;
    dst->diffstep = src->diffstep;
    _sactiveset_init_copy(&dst->sas, &src->sas, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    dst->prectype = src->prectype;
    ae_vector_init_copy(&dst->diagh, &src->diagh, _state, make_automatic);
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    dst->needf = src->needf;
    dst->needfg = src->needfg;
    dst->xupdated = src->xupdated;
    dst->lsstart = src->lsstart;
    dst->steepestdescentstep = src->steepestdescentstep;
    dst->boundedstep = src->boundedstep;
    dst->userterminationneeded = src->userterminationneeded;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    ae_vector_init_copy(&dst->ugc, &src->ugc, _state, make_automatic);
    ae_vector_init_copy(&dst->cgc, &src->cgc, _state, make_automatic);
    ae_vector_init_copy(&dst->xn, &src->xn, _state, make_automatic);
    ae_vector_init_copy(&dst->ugn, &src->ugn, _state, make_automatic);
    ae_vector_init_copy(&dst->cgn, &src->cgn, _state, make_automatic);
    ae_vector_init_copy(&dst->xp, &src->xp, _state, make_automatic);
    dst->fc = src->fc;
    dst->fn = src->fn;
    dst->fp = src->fp;
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    ae_matrix_init_copy(&dst->cleic, &src->cleic, _state, make_automatic);
    dst->nec = src->nec;
    dst->nic = src->nic;
    dst->lastgoodstep = src->lastgoodstep;
    dst->lastscaledgoodstep = src->lastscaledgoodstep;
    dst->maxscaledgrad = src->maxscaledgrad;
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    dst->repinneriterationscount = src->repinneriterationscount;
    dst->repouteriterationscount = src->repouteriterationscount;
    dst->repnfev = src->repnfev;
    dst->repvaridx = src->repvaridx;
    dst->repterminationtype = src->repterminationtype;
    dst->repdebugeqerr = src->repdebugeqerr;
    dst->repdebugfs = src->repdebugfs;
    dst->repdebugff = src->repdebugff;
    dst->repdebugdx = src->repdebugdx;
    dst->repdebugfeasqpits = src->repdebugfeasqpits;
    dst->repdebugfeasgpaits = src->repdebugfeasgpaits;
    ae_vector_init_copy(&dst->xstart, &src->xstart, _state, make_automatic);
    _snnlssolver_init_copy(&dst->solver, &src->solver, _state, make_automatic);
    dst->fbase = src->fbase;
    dst->fm2 = src->fm2;
    dst->fm1 = src->fm1;
    dst->fp1 = src->fp1;
    dst->fp2 = src->fp2;
    dst->xm1 = src->xm1;
    dst->xp1 = src->xp1;
    dst->gm1 = src->gm1;
    dst->gp1 = src->gp1;
    dst->cidx = src->cidx;
    dst->cval = src->cval;
    ae_vector_init_copy(&dst->tmpprec, &src->tmpprec, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    dst->nfev = src->nfev;
    dst->mcstage = src->mcstage;
    dst->stp = src->stp;
    dst->curstpmax = src->curstpmax;
    dst->activationstep = src->activationstep;
    ae_vector_init_copy(&dst->work, &src->work, _state, make_automatic);
    _linminstate_init_copy(&dst->lstate, &src->lstate, _state, make_automatic);
    dst->trimthreshold = src->trimthreshold;
    dst->nonmonotoniccnt = src->nonmonotoniccnt;
    ae_matrix_init_copy(&dst->bufyk, &src->bufyk, _state, make_automatic);
    ae_matrix_init_copy(&dst->bufsk, &src->bufsk, _state, make_automatic);
    ae_vector_init_copy(&dst->bufrho, &src->bufrho, _state, make_automatic);
    ae_vector_init_copy(&dst->buftheta, &src->buftheta, _state, make_automatic);
    dst->bufsize = src->bufsize;
    dst->teststep = src->teststep;
    dst->smoothnessguardlevel = src->smoothnessguardlevel;
    _smoothnessmonitor_init_copy(&dst->smonitor, &src->smonitor, _state, make_automatic);
    ae_vector_init_copy(&dst->lastscaleused, &src->lastscaleused, _state, make_automatic);
    ae_vector_init_copy(&dst->invs, &src->invs, _state, make_automatic);
}


void _minbleicstate_clear(void* _p)
{
    minbleicstate *p = (minbleicstate*)_p;
    ae_touch_ptr((void*)p);
    _sactiveset_clear(&p->sas);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->diagh);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->g);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->ugc);
    ae_vector_clear(&p->cgc);
    ae_vector_clear(&p->xn);
    ae_vector_clear(&p->ugn);
    ae_vector_clear(&p->cgn);
    ae_vector_clear(&p->xp);
    ae_vector_clear(&p->d);
    ae_matrix_clear(&p->cleic);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->xstart);
    _snnlssolver_clear(&p->solver);
    ae_vector_clear(&p->tmpprec);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->work);
    _linminstate_clear(&p->lstate);
    ae_matrix_clear(&p->bufyk);
    ae_matrix_clear(&p->bufsk);
    ae_vector_clear(&p->bufrho);
    ae_vector_clear(&p->buftheta);
    _smoothnessmonitor_clear(&p->smonitor);
    ae_vector_clear(&p->lastscaleused);
    ae_vector_clear(&p->invs);
}


void _minbleicstate_destroy(void* _p)
{
    minbleicstate *p = (minbleicstate*)_p;
    ae_touch_ptr((void*)p);
    _sactiveset_destroy(&p->sas);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->diagh);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->g);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->ugc);
    ae_vector_destroy(&p->cgc);
    ae_vector_destroy(&p->xn);
    ae_vector_destroy(&p->ugn);
    ae_vector_destroy(&p->cgn);
    ae_vector_destroy(&p->xp);
    ae_vector_destroy(&p->d);
    ae_matrix_destroy(&p->cleic);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->xstart);
    _snnlssolver_destroy(&p->solver);
    ae_vector_destroy(&p->tmpprec);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->work);
    _linminstate_destroy(&p->lstate);
    ae_matrix_destroy(&p->bufyk);
    ae_matrix_destroy(&p->bufsk);
    ae_vector_destroy(&p->bufrho);
    ae_vector_destroy(&p->buftheta);
    _smoothnessmonitor_destroy(&p->smonitor);
    ae_vector_destroy(&p->lastscaleused);
    ae_vector_destroy(&p->invs);
}


void _minbleicreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minbleicreport *p = (minbleicreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minbleicreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minbleicreport       *dst = (minbleicreport*)_dst;
    const minbleicreport *src = (const minbleicreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfev = src->nfev;
    dst->varidx = src->varidx;
    dst->terminationtype = src->terminationtype;
    dst->debugeqerr = src->debugeqerr;
    dst->debugfs = src->debugfs;
    dst->debugff = src->debugff;
    dst->debugdx = src->debugdx;
    dst->debugfeasqpits = src->debugfeasqpits;
    dst->debugfeasgpaits = src->debugfeasgpaits;
    dst->inneriterationscount = src->inneriterationscount;
    dst->outeriterationscount = src->outeriterationscount;
}


void _minbleicreport_clear(void* _p)
{
    minbleicreport *p = (minbleicreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minbleicreport_destroy(void* _p)
{
    minbleicreport *p = (minbleicreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
