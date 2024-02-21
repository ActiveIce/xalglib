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
#include "mincg.h"


/*$ Declarations $*/
static ae_int_t mincg_rscountdownlen = 10;
static double mincg_gtol = 0.3;
static void mincg_clearrequestfields(mincgstate* state, ae_state *_state);
static void mincg_preconditionedmultiply(mincgstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* work0,
     /* Real    */ ae_vector* work1,
     ae_state *_state);
static double mincg_preconditionedmultiply2(mincgstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* work0,
     /* Real    */ ae_vector* work1,
     ae_state *_state);
static void mincg_mincginitinternal(ae_int_t n,
     double diffstep,
     mincgstate* state,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
        NONLINEAR CONJUGATE GRADIENT METHOD

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by using one of  the
nonlinear conjugate gradient methods.

These CG methods are globally convergent (even on non-convex functions) as
long as grad(f) is Lipschitz continuous in  a  some  neighborhood  of  the
L = { x : f(x)<=f(x0) }.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinCGCreate() call
2. User tunes solver parameters with MinCGSetCond(), MinCGSetStpMax() and
   other functions
3. User calls MinCGOptimize() function which takes algorithm  state   and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinCGResults() to get solution
5. Optionally, user may call MinCGRestartFrom() to solve another  problem
   with same N but another starting point and/or another function.
   MinCGRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void mincgcreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     mincgstate* state,
     ae_state *_state)
{

    _mincgstate_clear(state);

    ae_assert(n>=1, "MinCGCreate: N too small!", _state);
    ae_assert(x->cnt>=n, "MinCGCreate: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinCGCreate: X contains infinite or NaN values!", _state);
    mincg_mincginitinternal(n, 0.0, state, _state);
    mincgrestartfrom(state, x, _state);
}


/*************************************************************************
The subroutine is finite difference variant of MinCGCreate(). It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this function
only. We recommend to read comments on MinCGCreate() in order to get more
information about creation of CG optimizer.

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
   S[] is scaling vector which can be set by MinCGSetScale() call.
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
   is  less  robust  and  precise.  L-BFGS  needs  exact  gradient values.
   Imprecise  gradient may slow down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void mincgcreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     mincgstate* state,
     ae_state *_state)
{

    _mincgstate_clear(state);

    ae_assert(n>=1, "MinCGCreateF: N too small!", _state);
    ae_assert(x->cnt>=n, "MinCGCreateF: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinCGCreateF: X contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(diffstep, _state), "MinCGCreateF: DiffStep is infinite or NaN!", _state);
    ae_assert(ae_fp_greater(diffstep,(double)(0)), "MinCGCreateF: DiffStep is non-positive!", _state);
    mincg_mincginitinternal(n, diffstep, state, _state);
    mincgrestartfrom(state, x, _state);
}


/*************************************************************************
This function sets stopping conditions for CG optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinCGSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinCGSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcond(mincgstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinCGSetCond: EpsG is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsg,(double)(0)), "MinCGSetCond: negative EpsG!", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinCGSetCond: EpsF is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsf,(double)(0)), "MinCGSetCond: negative EpsF!", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinCGSetCond: EpsX is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinCGSetCond: negative EpsX!", _state);
    ae_assert(maxits>=0, "MinCGSetCond: negative MaxIts!", _state);
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
This function sets scaling coefficients for CG optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of CG optimizer  -  step
along I-th axis is equal to DiffStep*S[I].

In   most   optimizers  (and  in  the  CG  too)  scaling is NOT a form  of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner by separate call to one of the MinCGSetPrec...() functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
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
void mincgsetscale(mincgstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinCGSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinCGSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "MinCGSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetxrep(mincgstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function turns on/off line search reports.
These reports are described in more details in developer-only  comments on
MinCGState object.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedDRep-   whether line search reports are needed or not

This function is intended for private use only. Turning it on artificially
may cause program failure.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetdrep(mincgstate* state, ae_bool needdrep, ae_state *_state)
{


    state->drep = needdrep;
}


/*************************************************************************
This function sets CG algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    CGType  -   algorithm type:
                * -1    automatic selection of the best algorithm
                * 0     DY (Dai and Yuan) algorithm
                * 1     Hybrid DY-HS algorithm

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcgtype(mincgstate* state, ae_int_t cgtype, ae_state *_state)
{


    ae_assert(cgtype>=-1&&cgtype<=1, "MinCGSetCGType: incorrect CGType!", _state);
    if( cgtype==-1 )
    {
        cgtype = 1;
    }
    state->cgtype = cgtype;
}


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

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetstpmax(mincgstate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinCGSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,(double)(0)), "MinCGSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************
This function allows to suggest initial step length to the CG algorithm.

Suggested  step  length  is used as starting point for the line search. It
can be useful when you have  badly  scaled  problem,  i.e.  when  ||grad||
(which is used as initial estimate for the first step) is many  orders  of
magnitude different from the desired step.

Line search  may  fail  on  such problems without good estimate of initial
step length. Imagine, for example, problem with ||grad||=10^50 and desired
step equal to 0.1 Line  search function will use 10^50  as  initial  step,
then  it  will  decrease step length by 2 (up to 20 attempts) and will get
10^44, which is still too large.

This function allows us to tell than line search should  be  started  from
some moderate step length, like 1.0, so algorithm will be able  to  detect
desired step length in a several searches.

Default behavior (when no step is suggested) is to use preconditioner,  if
it is available, to generate initial estimate of step length.

This function influences only first iteration of algorithm. It  should  be
called between MinCGCreate/MinCGRestartFrom() call and MinCGOptimize call.
Suggested step is ignored if you have preconditioner.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    Stp     -   initial estimate of the step length.
                Can be zero (no estimate).

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsuggeststep(mincgstate* state, double stp, ae_state *_state)
{


    ae_assert(ae_isfinite(stp, _state), "MinCGSuggestStep: Stp is infinite or NAN", _state);
    ae_assert(ae_fp_greater_eq(stp,(double)(0)), "MinCGSuggestStep: Stp<0", _state);
    state->suggestedstep = stp;
}


/*************************************************************************
This developer-only function allows to retrieve  unscaled  (!)  length  of
last good step (i.e. step which resulted in sufficient decrease of  target
function).

It can be used in for solution  of  sequential  optimization  subproblems,
where MinCGSuggestStep()  is  called  with  length  of  previous  step  as
parameter.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    
RESULT:
    length of last good step being accepted
    
NOTE:
    result of this function is undefined if you called it before

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
double mincglastgoodstep(mincgstate* state, ae_state *_state)
{
    double result;


    result = state->lastgoodstep;
    return result;
}


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdefault(mincgstate* state, ae_state *_state)
{


    state->prectype = 0;
    state->innerresetneeded = ae_true;
}


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdiag(mincgstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(d->cnt>=state->n, "MinCGSetPrecDiag: D is too short", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(d->ptr.p_double[i], _state), "MinCGSetPrecDiag: D contains infinite or NAN elements", _state);
        ae_assert(ae_fp_greater(d->ptr.p_double[i],(double)(0)), "MinCGSetPrecDiag: D contains non-positive elements", _state);
    }
    mincgsetprecdiagfast(state, d, _state);
}


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables with MinCGSetScale() call
(before or after MinCGSetPrecScale() call). Without knowledge of the scale
of your variables scale-based preconditioner will be just unit matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecscale(mincgstate* state, ae_state *_state)
{


    state->prectype = 3;
    state->innerresetneeded = ae_true;
}


/*************************************************************************
NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied  gradient, and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.
   
   Depending  on  the  specific  function  used to create optimizer object
   (either MinCGCreate()  for analytical gradient  or  MinCGCreateF()  for
   numerical differentiation) you should  choose  appropriate  variant  of
   MinCGOptimize() - one which accepts function AND gradient or one  which
   accepts function ONLY.

   Be careful to choose variant of MinCGOptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed  to  MinCGOptimize()  and  specific
   function used to create optimizer.
   

                  |         USER PASSED TO MinCGOptimize()
   CREATED WITH   |  function only   |  function and gradient
   ------------------------------------------------------------
   MinCGCreateF() |     work                FAIL
   MinCGCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function and MinCGOptimize() version. Attemps to use  such  combination
   (for  example,  to create optimizer with  MinCGCreateF()  and  to  pass
   gradient information to MinCGOptimize()) will lead to  exception  being
   thrown. Either  you  did  not  pass  gradient when it WAS needed or you
   passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool mincgiteration(mincgstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double betak;
    double v;
    double vv;
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
        i = state->rstate.ia.ptr.p_int[1];
        betak = state->rstate.ra.ptr.p_double[0];
        v = state->rstate.ra.ptr.p_double[1];
        vv = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        n = 359;
        i = -58;
        betak = -919.0;
        v = -909.0;
        vv = 81.0;
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
    
    /*
     * Routine body
     */
    
    /*
     * Prepare
     */
    n = state->n;
    state->terminationneeded = ae_false;
    state->userterminationneeded = ae_false;
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repnfev = 0;
    state->debugrestartscount = 0;
    smoothnessmonitorinit(&state->smonitor, &state->s, n, 1, state->smoothnessguardlevel>0, _state);
    rvectorsetlengthatleast(&state->invs, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->lastscaleused.ptr.p_double[i] = state->s.ptr.p_double[i];
        state->invs.ptr.p_double[i] = (double)1/state->s.ptr.p_double[i];
    }
    
    /*
     *  Check, that transferred derivative value is right
     */
    mincg_clearrequestfields(state, _state);
    if( !(ae_fp_eq(state->diffstep,(double)(0))&&ae_fp_greater(state->teststep,(double)(0))) )
    {
        goto lbl_18;
    }
lbl_20:
    if( !smoothnessmonitorcheckgradientatx0(&state->smonitor, &state->xbase, &state->s, &state->s, &state->s, ae_false, state->teststep, _state) )
    {
        goto lbl_21;
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
    goto lbl_20;
lbl_21:
lbl_18:
    
    /*
     * Preparations continue:
     * * set XK
     * * calculate F/G
     * * set DK to -G
     * * powerup algo (it may change preconditioner)
     * * apply preconditioner to DK
     * * report update of X
     * * check stopping conditions for G
     */
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->xbase.ptr.p_double[i];
    }
    ae_v_move(&state->xk.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    mincg_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        goto lbl_22;
    }
    state->needfg = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfg = ae_false;
    goto lbl_23;
lbl_22:
    state->needf = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->fbase = state->f;
    i = 0;
lbl_24:
    if( i>n-1 )
    {
        goto lbl_26;
    }
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->fp2 = state->f;
    state->x.ptr.p_double[i] = v;
    state->g.ptr.p_double[i] = ((double)8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/((double)6*state->diffstep*state->s.ptr.p_double[i]);
    i = i+1;
    goto lbl_24;
lbl_26:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_23:
    if( !state->drep )
    {
        goto lbl_27;
    }
    
    /*
     * Report algorithm powerup (if needed)
     */
    mincg_clearrequestfields(state, _state);
    state->algpowerup = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->algpowerup = ae_false;
lbl_27:
    trimprepare(state->f, &state->trimthreshold, _state);
    ae_v_moveneg(&state->dk.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    mincg_preconditionedmultiply(state, &state->dk, &state->work0, &state->work1, _state);
    if( !state->xrep )
    {
        goto lbl_29;
    }
    mincg_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->xupdated = ae_false;
lbl_29:
    if( state->terminationneeded||state->userterminationneeded )
    {
        
        /*
         * Combined termination point for "internal" termination by TerminationNeeded flag
         * and for "user" termination by MinCGRequestTermination() (UserTerminationNeeded flag).
         * In this location rules for both of methods are same, thus only one exit point is needed.
         */
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    v = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->g.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    if( ae_fp_less_eq(ae_sqrt(v, _state),state->epsg) )
    {
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    state->repnfev = 1;
    state->k = 0;
    state->fold = state->f;
    
    /*
     * Choose initial step.
     * Apply preconditioner, if we have something other than default.
     */
    if( state->prectype==2||state->prectype==3 )
    {
        
        /*
         * because we use preconditioner, step length must be equal
         * to the norm of DK
         */
        v = ae_v_dotproduct(&state->dk.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->lastgoodstep = ae_sqrt(v, _state);
    }
    else
    {
        
        /*
         * No preconditioner is used, we try to use suggested step
         */
        if( ae_fp_greater(state->suggestedstep,(double)(0)) )
        {
            state->lastgoodstep = state->suggestedstep;
        }
        else
        {
            state->lastgoodstep = 1.0;
        }
    }
    
    /*
     * Main cycle
     */
    state->rstimer = mincg_rscountdownlen;
lbl_31:
    if( ae_false )
    {
        goto lbl_32;
    }
    
    /*
     * * clear reset flag
     * * clear termination flag
     * * store G[k] for later calculation of Y[k]
     * * prepare starting point and direction and step length for line search
     */
    state->innerresetneeded = ae_false;
    state->terminationneeded = ae_false;
    ae_v_moveneg(&state->yk.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->d.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->mcstage = 0;
    state->stp = 1.0;
    linminnormalized(&state->d, &state->stp, n, _state);
    if( ae_fp_neq(state->lastgoodstep,(double)(0)) )
    {
        state->stp = state->lastgoodstep;
    }
    state->curstpmax = state->stpmax;
    
    /*
     * Report beginning of line search (if needed)
     * Terminate algorithm, if user request was detected
     */
    if( !state->drep )
    {
        goto lbl_33;
    }
    mincg_clearrequestfields(state, _state);
    state->lsstart = ae_true;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->lsstart = ae_false;
lbl_33:
    if( state->terminationneeded )
    {
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    
    /*
     * Minimization along D
     */
    smoothnessmonitorstartlinesearch1u(&state->smonitor, &state->s, &state->invs, &state->x, state->f, &state->g, state->repiterationscount, -1, _state);
    mcsrch(n, &state->x, &state->f, &state->g, &state->d, &state->stp, state->curstpmax, mincg_gtol, &state->mcinfo, &state->nfev, &state->work0, &state->lstate, &state->mcstage, _state);
lbl_35:
    if( state->mcstage==0 )
    {
        goto lbl_36;
    }
    
    /*
     * Calculate function/gradient using either
     * analytical gradient supplied by user
     * or finite difference approximation.
     *
     * "Trim" function in order to handle near-singularity points.
     */
    mincg_clearrequestfields(state, _state);
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        goto lbl_37;
    }
    state->needfg = ae_true;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->needfg = ae_false;
    goto lbl_38;
lbl_37:
    state->needf = ae_true;
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->fbase = state->f;
    i = 0;
lbl_39:
    if( i>n-1 )
    {
        goto lbl_41;
    }
    v = state->x.ptr.p_double[i];
    state->x.ptr.p_double[i] = v-state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    state->fm2 = state->f;
    state->x.ptr.p_double[i] = v-0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->fm1 = state->f;
    state->x.ptr.p_double[i] = v+0.5*state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->fp1 = state->f;
    state->x.ptr.p_double[i] = v+state->diffstep*state->s.ptr.p_double[i];
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->fp2 = state->f;
    state->x.ptr.p_double[i] = v;
    state->g.ptr.p_double[i] = ((double)8*(state->fp1-state->fm1)-(state->fp2-state->fm2))/((double)6*state->diffstep*state->s.ptr.p_double[i]);
    i = i+1;
    goto lbl_39;
lbl_41:
    state->f = state->fbase;
    state->needf = ae_false;
lbl_38:
    smoothnessmonitorenqueuepoint1u(&state->smonitor, &state->s, &state->invs, &state->d, state->stp, &state->x, state->f, &state->g, _state);
    trimfunction(&state->f, &state->g, n, state->trimthreshold, _state);
    
    /*
     * Call MCSRCH again
     */
    mcsrch(n, &state->x, &state->f, &state->g, &state->d, &state->stp, state->curstpmax, mincg_gtol, &state->mcinfo, &state->nfev, &state->work0, &state->lstate, &state->mcstage, _state);
    goto lbl_35;
lbl_36:
    smoothnessmonitorfinalizelinesearch(&state->smonitor, _state);
    
    /*
     * * terminate algorithm if "user" request for detected
     * * report end of line search
     * * store current point to XN
     * * report iteration
     * * terminate algorithm if "internal" request was detected
     */
    if( state->userterminationneeded )
    {
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->xk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    if( !state->drep )
    {
        goto lbl_42;
    }
    
    /*
     * Report end of line search (if needed)
     */
    mincg_clearrequestfields(state, _state);
    state->lsend = ae_true;
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
    state->lsend = ae_false;
lbl_42:
    ae_v_move(&state->xn.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    if( !state->xrep )
    {
        goto lbl_44;
    }
    mincg_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 17;
    goto lbl_rcomm;
lbl_17:
    state->xupdated = ae_false;
lbl_44:
    if( state->terminationneeded )
    {
        ae_v_move(&state->xn.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    
    /*
     * Line search is finished.
     * * calculate BetaK
     * * calculate DN
     * * update timers
     * * calculate step length:
     *   * LastScaledStep is ALWAYS calculated because it is used in the stopping criteria
     *   * LastGoodStep is updated only when MCINFO is equal to 1 (Wolfe conditions hold).
     *     See below for more explanation.
     */
    if( state->mcinfo==1&&!state->innerresetneeded )
    {
        
        /*
         * Standard Wolfe conditions hold
         * Calculate Y[K] and D[K]'*Y[K]
         */
        ae_v_add(&state->yk.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
        vv = ae_v_dotproduct(&state->yk.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1));
        
        /*
         * Calculate BetaK according to DY formula
         */
        v = mincg_preconditionedmultiply2(state, &state->g, &state->g, &state->work0, &state->work1, _state);
        state->betady = v/vv;
        
        /*
         * Calculate BetaK according to HS formula
         */
        v = mincg_preconditionedmultiply2(state, &state->g, &state->yk, &state->work0, &state->work1, _state);
        state->betahs = v/vv;
        
        /*
         * Choose BetaK
         */
        if( state->cgtype==0 )
        {
            betak = state->betady;
        }
        if( state->cgtype==1 )
        {
            betak = ae_maxreal((double)(0), ae_minreal(state->betady, state->betahs, _state), _state);
        }
    }
    else
    {
        
        /*
         * Something is wrong (may be function is too wild or too flat)
         * or we just have to restart algo.
         *
         * We'll set BetaK=0, which will restart CG algorithm.
         * We can stop later (during normal checks) if stopping conditions are met.
         */
        betak = (double)(0);
        state->debugrestartscount = state->debugrestartscount+1;
    }
    if( state->repiterationscount>0&&state->repiterationscount%(3+n)==0 )
    {
        
        /*
         * clear Beta every N iterations
         */
        betak = (double)(0);
    }
    if( state->mcinfo==1||state->mcinfo==5 )
    {
        state->rstimer = mincg_rscountdownlen;
    }
    else
    {
        state->rstimer = state->rstimer-1;
    }
    ae_v_moveneg(&state->dn.ptr.p_double[0], 1, &state->g.ptr.p_double[0], 1, ae_v_len(0,n-1));
    mincg_preconditionedmultiply(state, &state->dn, &state->work0, &state->work1, _state);
    ae_v_addd(&state->dn.ptr.p_double[0], 1, &state->dk.ptr.p_double[0], 1, ae_v_len(0,n-1), betak);
    state->lastscaledstep = 0.0;
    for(i=0; i<=n-1; i++)
    {
        state->lastscaledstep = state->lastscaledstep+ae_sqr(state->d.ptr.p_double[i]/state->s.ptr.p_double[i], _state);
    }
    state->lastscaledstep = state->stp*ae_sqrt(state->lastscaledstep, _state);
    if( state->mcinfo==1 )
    {
        
        /*
         * Step is good (Wolfe conditions hold), update LastGoodStep.
         *
         * This check for MCINFO=1 is essential because sometimes in the
         * constrained optimization setting we may take very short steps
         * (like 1E-15) because we were very close to boundary of the
         * feasible area. Such short step does not mean that we've converged
         * to the solution - it was so short because we were close to the
         * boundary and there was a limit on step length.
         *
         * So having such short step is quite normal situation. However, we
         * should NOT start next iteration from step whose initial length is
         * estimated as 1E-15 because it may lead to the failure of the
         * linear minimizer (step is too short, function does not changes,
         * line search stagnates).
         */
        state->lastgoodstep = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            state->lastgoodstep = state->lastgoodstep+ae_sqr(state->d.ptr.p_double[i], _state);
        }
        state->lastgoodstep = state->stp*ae_sqrt(state->lastgoodstep, _state);
    }
    
    /*
     * Update information.
     * Check stopping conditions.
     */
    v = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->g.ptr.p_double[i]*state->s.ptr.p_double[i], _state);
    }
    if( !ae_isfinite(v, _state)||!ae_isfinite(state->f, _state) )
    {
        
        /*
         * Abnormal termination - infinities in function/gradient
         */
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    state->repnfev = state->repnfev+state->nfev;
    state->repiterationscount = state->repiterationscount+1;
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        
        /*
         * Too many iterations
         */
        state->repterminationtype = 5;
        result = ae_false;
        return result;
    }
    if( ae_fp_less_eq(ae_sqrt(v, _state),state->epsg) )
    {
        
        /*
         * Gradient is small enough
         */
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    if( !state->innerresetneeded )
    {
        
        /*
         * These conditions are checked only when no inner reset was requested by user
         */
        if( ae_fp_less_eq(state->fold-state->f,state->epsf*ae_maxreal(ae_fabs(state->fold, _state), ae_maxreal(ae_fabs(state->f, _state), 1.0, _state), _state)) )
        {
            
            /*
             * F(k+1)-F(k) is small enough
             */
            state->repterminationtype = 1;
            result = ae_false;
            return result;
        }
        if( ae_fp_less_eq(state->lastscaledstep,state->epsx) )
        {
            
            /*
             * X(k+1)-X(k) is small enough
             */
            state->repterminationtype = 2;
            result = ae_false;
            return result;
        }
    }
    if( state->rstimer<=0 )
    {
        
        /*
         * Too many subsequent restarts
         */
        state->repterminationtype = 7;
        result = ae_false;
        return result;
    }
    
    /*
     * Shift Xk/Dk, update other information
     */
    ae_v_move(&state->xk.ptr.p_double[0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_move(&state->dk.ptr.p_double[0], 1, &state->dn.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fold = state->f;
    state->k = state->k+1;
    goto lbl_31;
lbl_32:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = i;
    state->rstate.ra.ptr.p_double[0] = betak;
    state->rstate.ra.ptr.p_double[1] = v;
    state->rstate.ra.ptr.p_double[2] = vv;
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

The primary OptGuard report can be retrieved with mincgoptguardresults().

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
                    means of setting scale with mincgsetscale().

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
void mincgoptguardgradient(mincgstate* state,
     double teststep,
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "MinCGOptGuardGradient: TestStep contains NaN or INF", _state);
    ae_assert(ae_fp_greater_eq(teststep,(double)(0)), "MinCGOptGuardGradient: invalid argument TestStep(TestStep<0)", _state);
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
void mincgoptguardsmoothness(mincgstate* state,
     ae_int_t level,
     ae_state *_state)
{


    ae_assert(level==0||level==1, "MinCGOptGuardSmoothness: unexpected value of level parameter", _state);
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
* mincgoptguardgradient() for gradient verification
* mincgoptguardsmoothness() for C0/C1 checks

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
* mincgoptguardnonc1test0results()
* mincgoptguardnonc1test1results()
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
void mincgoptguardresults(mincgstate* state,
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
void mincgoptguardnonc1test0results(const mincgstate* state,
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
void mincgoptguardnonc1test1results(mincgstate* state,
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
Conjugate gradient results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -8    internal integrity control  detected  infinite
                            or NAN values in  function/gradient.  Abnormal
                            termination signalled.
                    * -7    gradient verification failed.
                            See MinCGSetGradientCheck() for more information.
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible,
                            we return best X found so far
                    *  8    terminated by user
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresults(const mincgstate* state,
     /* Real    */ ae_vector* x,
     mincgreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _mincgreport_clear(rep);

    mincgresultsbuf(state, x, rep, _state);
}


/*************************************************************************
Conjugate gradient results

Buffered implementation of MinCGResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresultsbuf(const mincgstate* state,
     /* Real    */ ae_vector* x,
     mincgreport* rep,
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->xn.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nfev = state->repnfev;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
This  subroutine  restarts  CG  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgrestartfrom(mincgstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "MinCGRestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "MinCGCreate: X contains infinite or NaN values!", _state);
    ae_v_move(&state->xbase.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    mincgsuggeststep(state, 0.0, _state);
    ae_vector_set_length(&state->rstate.ia, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    mincg_clearrequestfields(state, _state);
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
void mincgrequesttermination(mincgstate* state, ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


/*************************************************************************
Faster version of MinCGSetPrecDiag(), for time-critical parts of code,
without safety checks.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdiagfast(mincgstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state)
{
    ae_int_t i;


    rvectorsetlengthatleast(&state->diagh, state->n, _state);
    rvectorsetlengthatleast(&state->diaghl2, state->n, _state);
    state->prectype = 2;
    state->vcnt = 0;
    state->innerresetneeded = ae_true;
    for(i=0; i<=state->n-1; i++)
    {
        state->diagh.ptr.p_double[i] = d->ptr.p_double[i];
        state->diaghl2.ptr.p_double[i] = 0.0;
    }
}


/*************************************************************************
This function sets low-rank preconditioner for Hessian matrix  H=D+V'*C*V,
where:
* H is a Hessian matrix, which is approximated by D/V/C
* D=D1+D2 is a diagonal matrix, which includes two positive definite terms:
  * constant term D1 (is not updated or infrequently updated)
  * variable term D2 (can be cheaply updated from iteration to iteration)
* V is a low-rank correction
* C is a diagonal factor of low-rank correction

Preconditioner P is calculated using approximate Woodburry formula:
    P  = D^(-1) - D^(-1)*V'*(C^(-1)+V*D1^(-1)*V')^(-1)*V*D^(-1)
       = D^(-1) - D^(-1)*VC'*VC*D^(-1),
where
    VC = sqrt(B)*V
    B  = (C^(-1)+V*D1^(-1)*V')^(-1)
    
Note that B is calculated using constant term (D1) only,  which  allows us
to update D2 without recalculation of B or   VC.  Such  preconditioner  is
exact when D2 is zero. When D2 is non-zero, it is only approximation,  but
very good and cheap one.

This function accepts D1, V, C.
D2 is set to zero by default.

Cost of this update is O(N*VCnt*VCnt), but D2 can be updated in just O(N)
by MinCGSetPrecVarPart.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetpreclowrankfast(mincgstate* state,
     /* Real    */ const ae_vector* d1,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* v,
     ae_int_t vcnt,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t n;
    double t;
    ae_matrix b;

    ae_frame_make(_state, &_frame_block);
    memset(&b, 0, sizeof(b));
    ae_matrix_init(&b, 0, 0, DT_REAL, _state, ae_true);

    if( vcnt==0 )
    {
        mincgsetprecdiagfast(state, d1, _state);
        ae_frame_leave(_state);
        return;
    }
    n = state->n;
    ae_matrix_set_length(&b, vcnt, vcnt, _state);
    rvectorsetlengthatleast(&state->diagh, n, _state);
    rvectorsetlengthatleast(&state->diaghl2, n, _state);
    rmatrixsetlengthatleast(&state->vcorr, vcnt, n, _state);
    state->prectype = 2;
    state->vcnt = vcnt;
    state->innerresetneeded = ae_true;
    for(i=0; i<=n-1; i++)
    {
        state->diagh.ptr.p_double[i] = d1->ptr.p_double[i];
        state->diaghl2.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=vcnt-1; i++)
    {
        for(j=i; j<=vcnt-1; j++)
        {
            t = (double)(0);
            for(k=0; k<=n-1; k++)
            {
                t = t+v->ptr.pp_double[i][k]*v->ptr.pp_double[j][k]/d1->ptr.p_double[k];
            }
            b.ptr.pp_double[i][j] = t;
        }
        b.ptr.pp_double[i][i] = b.ptr.pp_double[i][i]+1.0/c->ptr.p_double[i];
    }
    if( !spdmatrixcholeskyrec(&b, 0, vcnt, ae_true, &state->work0, _state) )
    {
        state->vcnt = 0;
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=vcnt-1; i++)
    {
        ae_v_move(&state->vcorr.ptr.pp_double[i][0], 1, &v->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
        for(j=0; j<=i-1; j++)
        {
            t = b.ptr.pp_double[j][i];
            ae_v_subd(&state->vcorr.ptr.pp_double[i][0], 1, &state->vcorr.ptr.pp_double[j][0], 1, ae_v_len(0,n-1), t);
        }
        t = (double)1/b.ptr.pp_double[i][i];
        ae_v_muld(&state->vcorr.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), t);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function updates variable part (diagonal matrix D2)
of low-rank preconditioner.

This update is very cheap and takes just O(N) time.

It has no effect with default preconditioner.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecvarpart(mincgstate* state,
     /* Real    */ const ae_vector* d2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    for(i=0; i<=n-1; i++)
    {
        state->diaghl2.ptr.p_double[i] = d2->ptr.p_double[i];
    }
}


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void mincgsetprotocolv1(mincgstate* state, ae_state *_state)
{


    state->protocolversion = 1;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void mincg_clearrequestfields(mincgstate* state, ae_state *_state)
{


    state->needf = ae_false;
    state->needfg = ae_false;
    state->xupdated = ae_false;
    state->lsstart = ae_false;
    state->lsend = ae_false;
    state->algpowerup = ae_false;
}


/*************************************************************************
This function calculates preconditioned product H^(-1)*x and stores result
back into X. Work0[] and Work1[] are used as temporaries (size must be at
least N; this function doesn't allocate arrays).

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
static void mincg_preconditionedmultiply(mincgstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* work0,
     /* Real    */ ae_vector* work1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t vcnt;
    double v;


    n = state->n;
    vcnt = state->vcnt;
    if( state->prectype==0 )
    {
        return;
    }
    if( state->prectype==3 )
    {
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = x->ptr.p_double[i]*state->s.ptr.p_double[i]*state->s.ptr.p_double[i];
        }
        return;
    }
    ae_assert(state->prectype==2, "MinCG: internal error (unexpected PrecType)", _state);
    
    /*
     * handle part common for VCnt=0 and VCnt<>0
     */
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = x->ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
    }
    
    /*
     * if VCnt>0
     */
    if( vcnt>0 )
    {
        for(i=0; i<=vcnt-1; i++)
        {
            v = ae_v_dotproduct(&state->vcorr.ptr.pp_double[i][0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
            work0->ptr.p_double[i] = v;
        }
        for(i=0; i<=n-1; i++)
        {
            work1->ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=vcnt-1; i++)
        {
            v = work0->ptr.p_double[i];
            ae_v_addd(&state->work1.ptr.p_double[0], 1, &state->vcorr.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
        }
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = x->ptr.p_double[i]-state->work1.ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
        }
    }
}


/*************************************************************************
This function calculates preconditioned product x'*H^(-1)*y. Work0[] and
Work1[] are used as temporaries (size must be at least N; this function
doesn't allocate arrays).

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
static double mincg_preconditionedmultiply2(mincgstate* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     /* Real    */ ae_vector* work0,
     /* Real    */ ae_vector* work1,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t vcnt;
    double v0;
    double v1;
    double result;


    n = state->n;
    vcnt = state->vcnt;
    
    /*
     * no preconditioning
     */
    if( state->prectype==0 )
    {
        v0 = ae_v_dotproduct(&x->ptr.p_double[0], 1, &y->ptr.p_double[0], 1, ae_v_len(0,n-1));
        result = v0;
        return result;
    }
    if( state->prectype==3 )
    {
        result = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            result = result+x->ptr.p_double[i]*state->s.ptr.p_double[i]*state->s.ptr.p_double[i]*y->ptr.p_double[i];
        }
        return result;
    }
    ae_assert(state->prectype==2, "MinCG: internal error (unexpected PrecType)", _state);
    
    /*
     * low rank preconditioning
     */
    result = 0.0;
    for(i=0; i<=n-1; i++)
    {
        result = result+x->ptr.p_double[i]*y->ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
    }
    if( vcnt>0 )
    {
        for(i=0; i<=n-1; i++)
        {
            work0->ptr.p_double[i] = x->ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
            work1->ptr.p_double[i] = y->ptr.p_double[i]/(state->diagh.ptr.p_double[i]+state->diaghl2.ptr.p_double[i]);
        }
        for(i=0; i<=vcnt-1; i++)
        {
            v0 = ae_v_dotproduct(&work0->ptr.p_double[0], 1, &state->vcorr.ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
            v1 = ae_v_dotproduct(&work1->ptr.p_double[0], 1, &state->vcorr.ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
            result = result-v0*v1;
        }
    }
    return result;
}


/*************************************************************************
Internal initialization subroutine

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
static void mincg_mincginitinternal(ae_int_t n,
     double diffstep,
     mincgstate* state,
     ae_state *_state)
{
    ae_int_t i;


    
    /*
     * Initialize
     */
    state->protocolversion = 1;
    state->teststep = (double)(0);
    state->smoothnessguardlevel = 0;
    smoothnessmonitorinit(&state->smonitor, &state->s, 0, 0, ae_false, _state);
    state->n = n;
    state->diffstep = diffstep;
    state->lastgoodstep = (double)(0);
    mincgsetcond(state, (double)(0), (double)(0), (double)(0), 0, _state);
    mincgsetxrep(state, ae_false, _state);
    mincgsetdrep(state, ae_false, _state);
    mincgsetstpmax(state, (double)(0), _state);
    mincgsetcgtype(state, -1, _state);
    mincgsetprecdefault(state, _state);
    ae_vector_set_length(&state->xk, n, _state);
    ae_vector_set_length(&state->dk, n, _state);
    ae_vector_set_length(&state->xn, n, _state);
    ae_vector_set_length(&state->dn, n, _state);
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->d, n, _state);
    ae_vector_set_length(&state->g, n, _state);
    ae_vector_set_length(&state->work0, n, _state);
    ae_vector_set_length(&state->work1, n, _state);
    ae_vector_set_length(&state->yk, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->invs, n, _state);
    ae_vector_set_length(&state->lastscaleused, n, _state);
    rvectorsetlengthatleast(&state->xbase, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
        state->invs.ptr.p_double[i] = 1.0;
        state->lastscaleused.ptr.p_double[i] = 1.0;
    }
}


void _mincgstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    mincgstate *p = (mincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->diagh, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diaghl2, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->vcorr, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->yk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    _linminstate_init(&p->lstate, _state, make_automatic);
    ae_vector_init(&p->work0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->work1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->invs, 0, DT_REAL, _state, make_automatic);
    _smoothnessmonitor_init(&p->smonitor, _state, make_automatic);
    ae_vector_init(&p->lastscaleused, 0, DT_REAL, _state, make_automatic);
}


void _mincgstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    mincgstate       *dst = (mincgstate*)_dst;
    const mincgstate *src = (const mincgstate*)_src;
    dst->n = src->n;
    dst->epsg = src->epsg;
    dst->epsf = src->epsf;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->stpmax = src->stpmax;
    dst->suggestedstep = src->suggestedstep;
    dst->xrep = src->xrep;
    dst->drep = src->drep;
    dst->cgtype = src->cgtype;
    dst->prectype = src->prectype;
    ae_vector_init_copy(&dst->diagh, &src->diagh, _state, make_automatic);
    ae_vector_init_copy(&dst->diaghl2, &src->diaghl2, _state, make_automatic);
    ae_matrix_init_copy(&dst->vcorr, &src->vcorr, _state, make_automatic);
    dst->vcnt = src->vcnt;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    dst->diffstep = src->diffstep;
    dst->nfev = src->nfev;
    dst->mcstage = src->mcstage;
    dst->k = src->k;
    ae_vector_init_copy(&dst->xk, &src->xk, _state, make_automatic);
    ae_vector_init_copy(&dst->dk, &src->dk, _state, make_automatic);
    ae_vector_init_copy(&dst->xn, &src->xn, _state, make_automatic);
    ae_vector_init_copy(&dst->dn, &src->dn, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    dst->fold = src->fold;
    dst->stp = src->stp;
    dst->curstpmax = src->curstpmax;
    ae_vector_init_copy(&dst->yk, &src->yk, _state, make_automatic);
    dst->lastgoodstep = src->lastgoodstep;
    dst->lastscaledstep = src->lastscaledstep;
    dst->mcinfo = src->mcinfo;
    dst->innerresetneeded = src->innerresetneeded;
    dst->terminationneeded = src->terminationneeded;
    dst->trimthreshold = src->trimthreshold;
    ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic);
    dst->rstimer = src->rstimer;
    dst->protocolversion = src->protocolversion;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    dst->needf = src->needf;
    dst->needfg = src->needfg;
    dst->xupdated = src->xupdated;
    dst->algpowerup = src->algpowerup;
    dst->lsstart = src->lsstart;
    dst->lsend = src->lsend;
    dst->userterminationneeded = src->userterminationneeded;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    dst->repiterationscount = src->repiterationscount;
    dst->repnfev = src->repnfev;
    dst->repterminationtype = src->repterminationtype;
    dst->debugrestartscount = src->debugrestartscount;
    _linminstate_init_copy(&dst->lstate, &src->lstate, _state, make_automatic);
    dst->fbase = src->fbase;
    dst->fm2 = src->fm2;
    dst->fm1 = src->fm1;
    dst->fp1 = src->fp1;
    dst->fp2 = src->fp2;
    dst->betahs = src->betahs;
    dst->betady = src->betady;
    ae_vector_init_copy(&dst->work0, &src->work0, _state, make_automatic);
    ae_vector_init_copy(&dst->work1, &src->work1, _state, make_automatic);
    ae_vector_init_copy(&dst->invs, &src->invs, _state, make_automatic);
    dst->teststep = src->teststep;
    dst->smoothnessguardlevel = src->smoothnessguardlevel;
    _smoothnessmonitor_init_copy(&dst->smonitor, &src->smonitor, _state, make_automatic);
    ae_vector_init_copy(&dst->lastscaleused, &src->lastscaleused, _state, make_automatic);
}


void _mincgstate_clear(void* _p)
{
    mincgstate *p = (mincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->diagh);
    ae_vector_clear(&p->diaghl2);
    ae_matrix_clear(&p->vcorr);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->xk);
    ae_vector_clear(&p->dk);
    ae_vector_clear(&p->xn);
    ae_vector_clear(&p->dn);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->yk);
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->g);
    _rcommstate_clear(&p->rstate);
    _linminstate_clear(&p->lstate);
    ae_vector_clear(&p->work0);
    ae_vector_clear(&p->work1);
    ae_vector_clear(&p->invs);
    _smoothnessmonitor_clear(&p->smonitor);
    ae_vector_clear(&p->lastscaleused);
}


void _mincgstate_destroy(void* _p)
{
    mincgstate *p = (mincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->diagh);
    ae_vector_destroy(&p->diaghl2);
    ae_matrix_destroy(&p->vcorr);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->xk);
    ae_vector_destroy(&p->dk);
    ae_vector_destroy(&p->xn);
    ae_vector_destroy(&p->dn);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->yk);
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->g);
    _rcommstate_destroy(&p->rstate);
    _linminstate_destroy(&p->lstate);
    ae_vector_destroy(&p->work0);
    ae_vector_destroy(&p->work1);
    ae_vector_destroy(&p->invs);
    _smoothnessmonitor_destroy(&p->smonitor);
    ae_vector_destroy(&p->lastscaleused);
}


void _mincgreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    mincgreport *p = (mincgreport*)_p;
    ae_touch_ptr((void*)p);
}


void _mincgreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    mincgreport       *dst = (mincgreport*)_dst;
    const mincgreport *src = (const mincgreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfev = src->nfev;
    dst->terminationtype = src->terminationtype;
}


void _mincgreport_clear(void* _p)
{
    mincgreport *p = (mincgreport*)_p;
    ae_touch_ptr((void*)p);
}


void _mincgreport_destroy(void* _p)
{
    mincgreport *p = (mincgreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
