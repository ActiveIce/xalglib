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
#include "minnlc.h"


/*$ Declarations $*/
static void minnlc_clearrequestfields(minnlcstate* state,
     ae_state *_state);
static void minnlc_minnlcinitinternal(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnlcstate* state,
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
The  subroutine  minimizes a function  F(x)  of N arguments subject to the
any combination of the:
* bound constraints
* linear inequality constraints
* linear equality constraints
* nonlinear equality constraints Gi(x)=0
* nonlinear inequality constraints Hi(x)<=0

REQUIREMENTS:
* the user must provide callback calculating F(), H(), G()  -  either both
  value and gradient, or merely a value (numerical differentiation will be
  used)
* F(), G(), H() are continuously differentiable on the  feasible  set  and
  its neighborhood
* starting point X0, which can be infeasible

USAGE:

Here we give the very brief outline  of  the MinNLC optimizer. We strongly
recommend you to study examples in the ALGLIB Reference Manual and to read
ALGLIB User Guide: https://www.alglib.net/nonlinear-programming/

1. The user initializes the solver with minnlccreate() or  minnlccreates()
   (the latter is used for numerical  differentiation)  call  and  chooses
   which NLC solver to use.
   
   In the current release the following solvers can be used:
   
   * sparse large-scale filter-based SQP solver, recommended  for problems
     of any size (from several variables to thousands  of  variables).
     Activated with minnlcsetalgosqp() function.
     
   * dense SQP-BFGS solver, recommended  for small-scale problems  (up  to
     several hundreds of variables) with a very expensive target function.
     Requires less function  evaluations than any other  solver,  but  has
     very expensive iteration.
     Activated with minnlcsetalgosqpbfgs() function.
     
   * several other solvers, including legacy ones

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
   c) minnlcsetnlc2() for nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.
   
4. User sets scale of the variables with minnlcsetscale() function. It  is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.

5. User sets stopping conditions with minnlcsetcond3() or minnlcsetcond().
   If NLC solver uses inner/outer  iteration  layout,  this  function sets
   stopping conditions for INNER iterations.
   
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

CALLBACK PARALLELISM

The MINNLC optimizer supports parallel parallel  numerical differentiation
('callback parallelism'). This feature, which  is  present  in  commercial
ALGLIB  editions,  greatly   accelerates   optimization   with   numerical
differentiation of an expensive target functions.

Callback parallelism is usually  beneficial  when  computing  a  numerical
gradient requires more than several  milliseconds.  In this case  the  job
of computing individual gradient components can be split between  multiple
threads. Even inexpensive targets can benefit  from  parallelism,  if  you
have many variables.

ALGLIB Reference Manual, 'Working with commercial  version' section, tells
how to activate callback parallelism for your programming language.

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

It sets constraints of the form

    Ci(x)=0 for i=0..NLEC-1
    Ci(x)<=0 for i=NLEC..NLEC+NLIC-1

See MinNLCSetNLC2() for a modern function which allows greater flexibility
in the constraint specification.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetnlc(minnlcstate* state,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(nlec>=0, "MinNLCSetNLC: NLEC<0", _state);
    ae_assert(nlic>=0, "MinNLCSetNLC: NLIC<0", _state);
    state->nnlc = nlec+nlic;
    rallocv(state->nnlc, &state->nl, _state);
    rallocv(state->nnlc, &state->nu, _state);
    for(i=0; i<=nlec-1; i++)
    {
        state->nl.ptr.p_double[i] = (double)(0);
        state->nu.ptr.p_double[i] = (double)(0);
    }
    for(i=nlec; i<=nlec+nlic-1; i++)
    {
        state->nl.ptr.p_double[i] = _state->v_neginf;
        state->nu.ptr.p_double[i] = (double)(0);
    }
    ae_vector_set_length(&state->fi, 1+nlec+nlic, _state);
    ae_matrix_set_length(&state->j, 1+nlec+nlic, state->n, _state);
}


/*************************************************************************
This function sets two-sided nonlinear constraints for MinNLC optimizer.

In fact, this function sets  only  constraints  COUNT  and  their  BOUNDS.
Constraints  themselves  (constraint  functions)   are   passed   to   the
MinNLCOptimize() method as callbacks.

MinNLCOptimize() method accepts a user-defined vector function F[] and its
Jacobian J[], where:
* first element of F[] and first row of J[] correspond to the target
* subsequent NNLC components of F[] (and rows of J[]) correspond  to  two-
  sided nonlinear constraints NL<=C(x)<=NU, where
  * NL[i]=NU[i] => I-th row is an equality constraint Ci(x)=NL
  * NL[i]<NU[i] => I-th tow is a  two-sided constraint NL[i]<=Ci(x)<=NU[i]
  * NL[i]=-INF  => I-th row is an one-sided constraint Ci(x)<=NU[i]
  * NU[i]=+INF  => I-th row is an one-sided constraint NL[i]<=Ci(x)
  * NL[i]=-INF, NU[i]=+INF => constraint is ignored

NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
      your problem has mixed constraints, you  may explicitly specify some
      of them as linear or box ones.
      It helps optimizer to handle them more efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    NL      -   array[NNLC], lower bounds, can contain -INF
    NU      -   array[NNLC], lower bounds, can contain +INF
    NNLC    -   constraints count, NNLC>=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible that the algorithm will evaluate the function  outside of
        the feasible area!
        
NOTE 2: algorithm scales variables  according  to the scale  specified by
        MinNLCSetScale()  function,  so it can handle problems with badly
        scaled variables (as long as we KNOW their scales).
           
        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints. Inappropriate scaling  of nonlinear  constraints  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT the same as solving it with constraint "0.001*G0(x)=0".
           
        It means that YOU are  the  one who is responsible for the correct
        scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
        you to scale nonlinear constraints in such a way that the Jacobian
        rows have approximately unit magnitude  (for  problems  with  unit
        scale) or have magnitude approximately equal to 1/S[i] (where S is
        a scale set by MinNLCSetScale() function).

  -- ALGLIB --
     Copyright 23.09.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetnlc2(minnlcstate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(nnlc>=0, "MinNLCSetNLC2: NNLC<0", _state);
    ae_assert(nl->cnt>=nnlc, "MinNLCSetNLC2: Length(NL)<NNLC", _state);
    ae_assert(nu->cnt>=nnlc, "MinNLCSetNLC2: Length(NU)<NNLC", _state);
    state->nnlc = nnlc;
    ae_vector_set_length(&state->fi, 1+nnlc, _state);
    ae_matrix_set_length(&state->j, 1+nnlc, state->n, _state);
    rallocv(nnlc, &state->nl, _state);
    rallocv(nnlc, &state->nu, _state);
    for(i=0; i<=nnlc-1; i++)
    {
        ae_assert(ae_isfinite(nl->ptr.p_double[i], _state)||ae_isneginf(nl->ptr.p_double[i], _state), "MinNLCSetNLC2: NL[i] is +INF or NAN", _state);
        ae_assert(ae_isfinite(nu->ptr.p_double[i], _state)||ae_isposinf(nu->ptr.p_double[i], _state), "MinNLCSetNLC2: NU[i] is -INF or NAN", _state);
        state->nl.ptr.p_double[i] = nl->ptr.p_double[i];
        state->nu.ptr.p_double[i] = nu->ptr.p_double[i];
    }
}


/*************************************************************************
This function sets stopping conditions for the optimizer.

This  function allows to set  iterations  limit  and  step-based  stopping
conditions. If you want the solver to stop upon having a small  change  in
the target, use minnlcsetcond3() function.

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
    critsetcondv1(&state->criteria, 0.0, epsx, maxits, _state);
}


/*************************************************************************
This function sets stopping conditions for the optimizer.

This function allows to set three types of stopping conditions:
* iterations limit
* stopping upon performing a short step (depending on the specific  solver
  being used  it may stop as soon as the first short step was made, or
  only after performing several sequential short steps)
* stopping upon having a small change in  the  target  (depending  on  the
  specific solver being used it may stop as soon as the  first  step  with
  small change in the target was made, or only  after  performing  several
  sequential steps)

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   >=0
                The optimizer will stop as soon as the following condition
                is met:
                    
                    |f_scl(k+1)-f_scl(k)| <= max(|f_scl(k+1)|,|f_scl(k)|,1)
                    
                where f_scl is an internally used by the optimizer rescaled
                target (ALGLIB optimizers usually apply rescaling in order
                to normalize target and constraints).
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinNLCSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsF, EpsX=0 and MaxIts=0 (simultaneously) will lead to the
automatic selection of the stopping condition.

  -- ALGLIB --
     Copyright 21.09.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetcond3(minnlcstate* state,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsf, _state), "MinNLCSetCond3: EpsF is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsf,(double)(0)), "MinNLCSetCond3: negative EpsF", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinNLCSetCond3: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinNLCSetCond3: negative EpsX", _state);
    ae_assert(maxits>=0, "MinNLCSetCond3: negative MaxIts!", _state);
    critsetcondv1(&state->criteria, epsf, epsx, maxits, _state);
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
This function tells MinNLC unit to use the large-scale augmented Lagrangian
algorithm for nonlinearly constrained optimization.

This  algorithm  is  a  significant  refactoring  of  one  described in "A
Modified Barrier-Augmented  Lagrangian Method for Constrained Minimization
(1999)" by D.GOLDFARB,  R.POLYAK,  K. SCHEINBERG,  I.YUZEFOVICH  with  the
following additions:
* improved sparsity support
* improved handling of large-scale problems with the low rank  LBFGS-based
  sparse preconditioner
* automatic selection of the penalty parameter Rho

AUL solver can be significantly faster than SQP on easy  problems  due  to
cheaper iterations, although it needs more function evaluations. On large-
scale sparse problems one iteration of the AUL solver usually  costs  tens
times less than one iteration of the SQP solver.

However, the SQP solver is more robust than the AUL. In particular, it  is
much better at constraint enforcement and will  never escape feasible area
after constraints were successfully enforced.  It  also  needs  much  less
target function evaluations.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    MaxOuterIts-upper limit on outer iterations count:
                * MaxOuterIts=0 means that the solver  will  automatically
                  choose an upper limit. Recommended value.
                * MaxOuterIts>1 means that the AUL solver will performs at
                  most specified number of outer iterations

  -- ALGLIB --
     Copyright 22.09.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgoaul2(minnlcstate* state,
     ae_int_t maxouterits,
     ae_state *_state)
{


    ae_assert(maxouterits>=0, "MinNLCSetAlgoAUL2: negative MaxOuterIts", _state);
    state->aulitscnt = maxouterits;
    state->solvertype = 0;
}


/*************************************************************************
This   function  tells  MinNLC  optimizer  to  use  SLP (Successive Linear
Programming) algorithm for  nonlinearly  constrained   optimization.  This
algorithm  is  a  slight  modification  of  one  described  in  "A  Linear
programming-based optimization algorithm for solving nonlinear programming
problems" (2010) by Claus Still and Tapio Westerlund.

This solver is one of the slowest  in  ALGLIB,  it  requires  more  target
function evaluations that SQP and AUL. However it is somewhat more  robust
in tricky cases, so it can be used as a backup  plan  for low  dimensional
problems.

We recommend to use this algo when SQP/AUL solvers do not work. If  trying
different approach gives the same results, then MAYBE something  is  wrong
with your optimization problem.

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
This function selects a legacy solver: an L1 merit function based SQP with
the sparse l-BFGS update.

It is recommended to use either SQP or SQP-BFGS solvers  instead  of  this
one.  These  solvers  use  filters  to  provide  much  faster  and  robust
convergence.
> 

  -- ALGLIB --
     Copyright 02.12.2019 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosl1qp(minnlcstate* state, ae_state *_state)
{


    state->solvertype = 2;
}


/*************************************************************************
This function selects a legacy solver: an L1 merit function based SQP with
the dense BFGS update.

It is recommended to use either SQP or SQP-BFGS solvers  instead  of  this
one.  These  solvers  use  filters  to  provide  much  faster  and  robust
convergence.

  -- ALGLIB --
     Copyright 02.12.2019 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosl1qpbfgs(minnlcstate* state, ae_state *_state)
{


    state->solvertype = 3;
}


/*************************************************************************
This function selects large-scale  sparse  filter-based  SQP  solver,  the
most robust solver in ALGLIB, a recommended option.

This algorithm is scalable to problems with tens of thousands of variables
and can efficiently handle sparsity of constraints.

The convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)

This algorithm has the following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* the initial point can be infeasible
* the algorithm respects box constraints in all  intermediate  points  (it
  does not even evaluate the target outside of the box constrained area)
* once linear constraints are enforced, the algorithm will not violate them
* no such guarantees can be provided for nonlinear constraints,  but  once
  nonlinear  constraints  are  enforced,  the algorithm will try to respect
  them as much as possible
* numerical differentiation does  not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

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
     Copyright 02.12.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosqp(minnlcstate* state, ae_state *_state)
{


    state->solvertype = 4;
}


/*************************************************************************
This function selects a special solver for low-dimensional  problems  with
expensive target function - the dense filer-based SQP-BFGS solver.

This algorithm uses a dense quadratic model of the  target  and  solves  a
dense QP subproblem at each step. Thus, it has difficulties scaling beyond
several hundreds of variables.  However,  it  usually  needs  the smallest
number of the target evaluations - sometimes  up  to  30%  less  than  the
sparse large-scale filter-based SQP.

The convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)

This algorithm has the following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* the initial point can be infeasible
* the algorithm respects box constraints in all  intermediate  points  (it
  does not even evaluate the target outside of the box constrained area)
* once linear constraints are enforced, the algorithm will not violate them
* no such guarantees can be provided for nonlinear constraints,  but  once
  nonlinear  constraints  are  enforced,  the algorithm will try to respect
  them as much as possible
* numerical differentiation does  not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    
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
     Copyright 02.12.2023 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosqpbfgs(minnlcstate* state, ae_state *_state)
{


    state->solvertype = 5;
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

CALLBACK PARALLELISM

The MINNLC optimizer supports parallel parallel  numerical differentiation
('callback parallelism'). This feature, which  is  present  in  commercial
ALGLIB  editions,  greatly   accelerates   optimization   with   numerical
differentiation of an expensive target functions.

Callback parallelism is usually  beneficial  when  computing  a  numerical
gradient requires more than several  milliseconds.  In this case  the  job
of computing individual gradient components can be split between  multiple
threads. Even inexpensive targets can benefit  from  parallelism,  if  you
have many variables.

ALGLIB Reference Manual, 'Working with commercial  version' section, tells
how to activate callback parallelism for your programming language.

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
    ae_int_t j;
    ae_int_t k;
    ae_int_t n;
    ae_int_t nnlc;
    ae_int_t offs;
    double vleft;
    double vright;
    ae_bool b;
    ae_int_t originalrequest;
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
        j = state->rstate.ia.ptr.p_int[1];
        k = state->rstate.ia.ptr.p_int[2];
        n = state->rstate.ia.ptr.p_int[3];
        nnlc = state->rstate.ia.ptr.p_int[4];
        offs = state->rstate.ia.ptr.p_int[5];
        originalrequest = state->rstate.ia.ptr.p_int[6];
        b = state->rstate.ba.ptr.p_bool[0];
        vleft = state->rstate.ra.ptr.p_double[0];
        vright = state->rstate.ra.ptr.p_double[1];
    }
    else
    {
        i = 359;
        j = -58;
        k = -919;
        n = -909;
        nnlc = 81;
        offs = 255;
        originalrequest = 74;
        b = ae_false;
        vleft = 809.0;
        vright = 205.0;
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
    nnlc = state->nnlc;
    minnlc_clearrequestfields(state, _state);
    ae_assert(state->smoothnessguardlevel==0||state->smoothnessguardlevel==1, "MinNLCIteration: integrity check failed", _state);
    b = state->smoothnessguardlevel>0;
    b = b||(state->solvertype==0&&(ae_is_trace_enabled("AUL.PROBING")||ae_is_trace_enabled("AUL.PROBINGONFAILURE")));
    b = b||(state->solvertype==1&&ae_is_trace_enabled("SLP.PROBING"));
    b = b||((((state->solvertype==2||state->solvertype==3)||state->solvertype==4)||state->solvertype==5)&&(ae_is_trace_enabled("SQP.PROBING")||ae_is_trace_enabled("SQP.PROBINGONFAILURE")));
    smoothnessmonitorinit(&state->smonitor, &state->s, n, 1+nnlc, b, _state);
    for(i=0; i<=n-1; i++)
    {
        state->lastscaleused.ptr.p_double[i] = state->s.ptr.p_double[i];
    }
    
    /*
     * Allocate buffers, as mandated by the V2 protocol
     */
    if( state->protocolversion==2 )
    {
        if( ae_fp_eq(state->diffstep,(double)(0)) )
        {
            rallocv(n, &state->querydata, _state);
        }
        else
        {
            rallocv(n+4*2*n, &state->querydata, _state);
        }
        rallocv(1+nnlc, &state->replyfi, _state);
        rallocv(n*(1+nnlc), &state->replydj, _state);
        rallocm(1+nnlc, n, &state->tmpj1, _state);
        rallocv(1+nnlc, &state->tmpf1, _state);
        rallocv(n, &state->tmpg1, _state);
        rallocv(n, &state->tmpx1, _state);
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
        goto lbl_14;
    }
lbl_16:
    if( !smoothnessmonitorcheckgradientatx0(&state->smonitor, &state->xstart, &state->s, &state->bndl, &state->bndu, ae_true, state->teststep, _state) )
    {
        goto lbl_17;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_18;
    }
    state->requesttype = 2;
    state->querysize = 1;
    state->queryfuncs = 1+nnlc;
    state->queryvars = n;
    state->querydim = 0;
    rcopyv(n, &state->smonitor.x, &state->querydata, _state);
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    for(i=0; i<=nnlc; i++)
    {
        state->smonitor.fi.ptr.p_double[i] = state->replyfi.ptr.p_double[i];
        for(k=0; k<=n-1; k++)
        {
            state->smonitor.j.ptr.pp_double[i][k] = state->replydj.ptr.p_double[i*n+k];
        }
    }
    goto lbl_19;
lbl_18:
    ae_assert(state->protocolversion==1, "MINNLC: integrity check 0748 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->smonitor.x.ptr.p_double[i];
    }
    state->needfij = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfij = ae_false;
    for(i=0; i<=nnlc; i++)
    {
        state->smonitor.fi.ptr.p_double[i] = state->fi.ptr.p_double[i];
        for(k=0; k<=n-1; k++)
        {
            state->smonitor.j.ptr.pp_double[i][k] = state->j.ptr.pp_double[i][k];
        }
    }
lbl_19:
    goto lbl_16;
lbl_17:
lbl_14:
    
    /*
     * Initialization phase
     */
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        rvectorsetlengthatleast(&state->xbase, n, _state);
        rvectorsetlengthatleast(&state->fbase, 1+nnlc, _state);
        rvectorsetlengthatleast(&state->fm2, 1+nnlc, _state);
        rvectorsetlengthatleast(&state->fm1, 1+nnlc, _state);
        rvectorsetlengthatleast(&state->fp1, 1+nnlc, _state);
        rvectorsetlengthatleast(&state->fp2, 1+nnlc, _state);
    }
    b = ae_false;
    if( state->solvertype==0 )
    {
        
        /*
         * Initialize AUL solver
         */
        if( state->nec+state->nic>0 )
        {
            sparsecreatecrsfromdensebuf(&state->cleic, state->nec+state->nic, n, &state->sparsea, _state);
            rallocv(state->nec+state->nic, &state->al, _state);
            rallocv(state->nec+state->nic, &state->au, _state);
            for(i=0; i<=state->nec-1; i++)
            {
                state->al.ptr.p_double[i] = state->cleic.ptr.pp_double[i][n];
                state->au.ptr.p_double[i] = state->cleic.ptr.pp_double[i][n];
            }
            for(i=state->nec; i<=state->nec+state->nic-1; i++)
            {
                state->al.ptr.p_double[i] = _state->v_neginf;
                state->au.ptr.p_double[i] = state->cleic.ptr.pp_double[i][n];
            }
        }
        minaulinitbuf(&state->bndl, &state->bndu, &state->s, &state->xstart, n, &state->sparsea, &state->al, &state->au, &state->lcsrcidx, state->nec+state->nic, &state->nl, &state->nu, nnlc, &state->criteria, state->aulitscnt, &state->aulsolverstate, _state);
        b = ae_true;
    }
    if( state->solvertype==1 )
    {
        
        /*
         * Initialize SLP solver
         */
        converttwosidednlctoonesidedold(&state->nl, &state->nu, nnlc, &state->nlcidx, &state->nlcmul, &state->nlcadd, &state->nlcnlec, &state->nlcnlic, _state);
        minslpinitbuf(&state->bndl, &state->bndu, &state->s, &state->xstart, n, &state->cleic, &state->lcsrcidx, state->nec, state->nic, state->nlcnlec, state->nlcnlic, critgetepsx(&state->criteria, _state), critgetmaxits(&state->criteria, _state), &state->slpsolverstate, _state);
        b = ae_true;
    }
    if( state->solvertype==2||state->solvertype==3 )
    {
        
        /*
         * Initialize SQP solver
         */
        converttwosidednlctoonesidedold(&state->nl, &state->nu, nnlc, &state->nlcidx, &state->nlcmul, &state->nlcadd, &state->nlcnlec, &state->nlcnlic, _state);
        minsqpinitbuf(&state->bndl, &state->bndu, &state->s, &state->xstart, n, &state->cleic, &state->lcsrcidx, state->nec, state->nic, state->nlcnlec, state->nlcnlic, &state->criteria, state->solvertype==3, &state->sqpsolverstate, _state);
        b = ae_true;
    }
    if( state->solvertype==4||state->solvertype==5 )
    {
        
        /*
         * Initialize FSQP solver
         */
        converttwosidednlctoonesidedold(&state->nl, &state->nu, nnlc, &state->nlcidx, &state->nlcmul, &state->nlcadd, &state->nlcnlec, &state->nlcnlic, _state);
        minfsqpinitbuf(&state->bndl, &state->bndu, &state->s, &state->xstart, n, &state->cleic, &state->lcsrcidx, state->nec, state->nic, state->nlcnlec, state->nlcnlic, &state->criteria, state->solvertype==5, &state->fsqpsolverstate, _state);
        b = ae_true;
    }
    ae_assert(b, "MINNLC: integrity check 7046 failed", _state);
    
    /*
     * Iteration phase
     */
lbl_20:
    if( ae_false )
    {
        goto lbl_21;
    }
    
    /*
     * Run iteration function and load evaluation point
     */
    minnlc_clearrequestfields(state, _state);
    originalrequest = 0;
    b = ae_false;
    if( state->solvertype==0 )
    {
        
        /*
         * AUL solver
         */
        if( !minauliteration(&state->aulsolverstate, &state->smonitor, state->userterminationneeded, _state) )
        {
            goto lbl_21;
        }
        if( state->aulsolverstate.needsj )
        {
            
            /*
             * Request dense analytic Jacobian
             */
            if( state->protocolversion==2 )
            {
                minnlc_unscale(state, &state->aulsolverstate.x, &state->aulsolverstate.scaledbndl, &state->aulsolverstate.scaledbndu, &state->querydata, _state);
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 0312 failed", _state);
                minnlc_unscale(state, &state->aulsolverstate.x, &state->aulsolverstate.scaledbndl, &state->aulsolverstate.scaledbndu, &state->x, _state);
            }
            originalrequest = 2;
            b = ae_true;
        }
        if( state->aulsolverstate.xupdated )
        {
            if( state->protocolversion==2 )
            {
                rallocv(n, &state->reportx, _state);
                minnlc_unscale(state, &state->aulsolverstate.x, &state->aulsolverstate.scaledbndl, &state->aulsolverstate.scaledbndu, &state->reportx, _state);
                state->reportf = state->aulsolverstate.f;
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 0941 failed", _state);
                minnlc_unscale(state, &state->aulsolverstate.x, &state->aulsolverstate.scaledbndl, &state->aulsolverstate.scaledbndu, &state->x, _state);
                state->f = state->aulsolverstate.f;
            }
            originalrequest = -1;
            b = ae_true;
        }
    }
    if( state->solvertype==1 )
    {
        
        /*
         * SLP solver
         */
        if( !minslpiteration(&state->slpsolverstate, &state->smonitor, state->userterminationneeded, _state) )
        {
            goto lbl_21;
        }
        if( state->slpsolverstate.needfij )
        {
            if( state->protocolversion==2 )
            {
                minnlc_unscale(state, &state->slpsolverstate.x, &state->slpsolverstate.scaledbndl, &state->slpsolverstate.scaledbndu, &state->querydata, _state);
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 4335 failed", _state);
                minnlc_unscale(state, &state->slpsolverstate.x, &state->slpsolverstate.scaledbndl, &state->slpsolverstate.scaledbndu, &state->x, _state);
            }
            originalrequest = 2;
            b = ae_true;
        }
        if( state->slpsolverstate.xupdated )
        {
            if( state->protocolversion==2 )
            {
                rallocv(n, &state->reportx, _state);
                minnlc_unscale(state, &state->slpsolverstate.x, &state->slpsolverstate.scaledbndl, &state->slpsolverstate.scaledbndu, &state->reportx, _state);
                state->reportf = state->slpsolverstate.f;
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 6041 failed", _state);
                minnlc_unscale(state, &state->slpsolverstate.x, &state->slpsolverstate.scaledbndl, &state->slpsolverstate.scaledbndu, &state->x, _state);
                state->f = state->slpsolverstate.f;
            }
            originalrequest = -1;
            b = ae_true;
        }
    }
    if( state->solvertype==2||state->solvertype==3 )
    {
        
        /*
         * SQP solver
         */
        if( !minsqpiteration(&state->sqpsolverstate, &state->smonitor, state->userterminationneeded, _state) )
        {
            goto lbl_21;
        }
        if( state->sqpsolverstate.needfij )
        {
            if( state->protocolversion==2 )
            {
                minnlc_unscale(state, &state->sqpsolverstate.x, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->querydata, _state);
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 7919 failed", _state);
                minnlc_unscale(state, &state->sqpsolverstate.x, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->x, _state);
            }
            originalrequest = 2;
            b = ae_true;
        }
        if( state->sqpsolverstate.xupdated )
        {
            if( state->protocolversion==2 )
            {
                rallocv(n, &state->reportx, _state);
                minnlc_unscale(state, &state->sqpsolverstate.x, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->reportx, _state);
                state->reportf = state->sqpsolverstate.f;
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 9420 failed", _state);
                minnlc_unscale(state, &state->sqpsolverstate.x, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->x, _state);
                state->f = state->sqpsolverstate.f;
            }
            originalrequest = -1;
            b = ae_true;
        }
    }
    if( state->solvertype==4||state->solvertype==5 )
    {
        
        /*
         * FSQP solver
         */
        if( !minfsqpiteration(&state->fsqpsolverstate, &state->smonitor, state->userterminationneeded, _state) )
        {
            goto lbl_21;
        }
        if( state->fsqpsolverstate.needfij )
        {
            if( state->protocolversion==2 )
            {
                minnlc_unscale(state, &state->fsqpsolverstate.x, &state->fsqpsolverstate.scaledbndl, &state->fsqpsolverstate.scaledbndu, &state->querydata, _state);
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 7919 failed", _state);
                minnlc_unscale(state, &state->fsqpsolverstate.x, &state->fsqpsolverstate.scaledbndl, &state->fsqpsolverstate.scaledbndu, &state->x, _state);
            }
            originalrequest = 2;
            b = ae_true;
        }
        if( state->fsqpsolverstate.xupdated )
        {
            if( state->protocolversion==2 )
            {
                rallocv(n, &state->reportx, _state);
                minnlc_unscale(state, &state->fsqpsolverstate.x, &state->fsqpsolverstate.scaledbndl, &state->fsqpsolverstate.scaledbndu, &state->reportx, _state);
                state->reportf = state->fsqpsolverstate.f;
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 9420 failed", _state);
                minnlc_unscale(state, &state->fsqpsolverstate.x, &state->fsqpsolverstate.scaledbndl, &state->fsqpsolverstate.scaledbndu, &state->x, _state);
                state->f = state->fsqpsolverstate.f;
            }
            originalrequest = -1;
            b = ae_true;
        }
    }
    ae_assert(b, "MINNLC: integrity check 1848 failed", _state);
    
    /*
     * Process evaluation request, perform numerical differentiation if necessary, offload results back to the optimizer
     */
    b = ae_false;
    if( originalrequest!=-1 )
    {
        goto lbl_22;
    }
    
    /*
     * Report
     */
    if( !state->xrep )
    {
        goto lbl_24;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_26;
    }
    state->requesttype = -1;
    state->queryvars = n;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    goto lbl_27;
lbl_26:
    ae_assert(state->protocolversion==1, "MINNLC: integrity check 8214 failed", _state);
    state->xupdated = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->xupdated = ae_false;
lbl_27:
lbl_24:
    b = ae_true;
lbl_22:
    if( originalrequest!=2 )
    {
        goto lbl_28;
    }
    
    /*
     * Jacobian evaluation, with numerical differentiation if needed
     */
    if( ae_fp_eq(state->diffstep,(double)(0)) )
    {
        goto lbl_30;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_32;
    }
    
    /*
     * Issue V2 request
     */
    rsetallocv(n, 0.0, &state->tmpsptolj, _state);
    state->requesttype = 3;
    state->querysize = 1;
    state->queryfuncs = 1+nnlc;
    state->queryvars = n;
    state->querydim = 0;
    state->queryformulasize = 4;
    ae_assert(state->querydata.cnt>=n+state->queryformulasize*2*n, "MINNLC: integrity check 1558 failed", _state);
    rcopyv(n, &state->querydata, &state->xbase, _state);
    rsetvx(state->queryformulasize*2*n, 0.0, &state->querydata, n, _state);
    for(k=0; k<=n-1; k++)
    {
        vleft = state->xbase.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
        vright = state->xbase.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
        if( (state->hasbndl.ptr.p_bool[k]&&ae_fp_less(vleft,state->bndl.ptr.p_double[k]))||(state->hasbndu.ptr.p_bool[k]&&ae_fp_greater(vright,state->bndu.ptr.p_double[k])) )
        {
            
            /*
             * 4-point formula violates box constraints, use 2-point uncentered one
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
                continue;
            }
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+0*2+0] = vleft;
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+0*2+1] = -(double)1/(vright-vleft);
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+1*2+0] = vright;
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+1*2+1] = (double)1/(vright-vleft);
            state->tmpsptolj.ptr.p_double[k] = (double)10*ae_machineepsilon/(vright-vleft);
            state->repnfev = state->repnfev+2;
        }
        else
        {
            
            /*
             * Use 4-point central formula
             */
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+0*2+0] = vleft;
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+0*2+1] = (double)1/((double)6*state->diffstep*state->s.ptr.p_double[k]);
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+1*2+0] = state->xbase.ptr.p_double[k]-0.5*state->s.ptr.p_double[k]*state->diffstep;
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+1*2+1] = -(double)4/((double)3*state->diffstep*state->s.ptr.p_double[k]);
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+2*2+0] = state->xbase.ptr.p_double[k]+0.5*state->s.ptr.p_double[k]*state->diffstep;
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+2*2+1] = (double)4/((double)3*state->diffstep*state->s.ptr.p_double[k]);
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+3*2+0] = vright;
            state->querydata.ptr.p_double[n+state->queryformulasize*2*k+3*2+1] = -(double)1/((double)6*state->diffstep*state->s.ptr.p_double[k]);
            state->tmpsptolj.ptr.p_double[k] = ae_fabs((double)10*ae_machineepsilon*(double)4/((double)3*state->diffstep*state->s.ptr.p_double[k]), _state);
            state->repnfev = state->repnfev+4;
        }
    }
    rcopyv(n, &state->xbase, &state->x, _state);
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needfi = ae_false;
    state->repnfev = state->repnfev+1;
    
    /*
     * Sparsify Jacobian by zeroing components whose deviation from zero can be
     * explained by numerical rounding errors.
     *
     * This part is essential for the performance of sparse optimizers which can
     * face severe performance breakdown on Jacobians which became dense due to
     * numerical rounding errors.
     */
    rallocv(1+nnlc, &state->tmpspaf, _state);
    for(i=0; i<=nnlc; i++)
    {
        state->tmpspaf.ptr.p_double[i] = ae_fabs(state->replyfi.ptr.p_double[i], _state);
    }
    for(i=0; i<=nnlc; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( ae_fabs(state->replydj.ptr.p_double[i*n+j], _state)<=state->tmpspaf.ptr.p_double[i]*state->tmpsptolj.ptr.p_double[j] )
            {
                state->replydj.ptr.p_double[i*n+j] = (double)(0);
            }
        }
    }
    goto lbl_33;
lbl_32:
    
    /*
     * Issue V1 request
     */
    ae_assert(state->protocolversion==1, "MINNLC: integrity check 0252 failed", _state);
    ae_assert(state->j.rows==1+nnlc&&state->j.cols==n, "MINNLC: integrity check 4236 failed", _state);
    rsetallocv(n, 0.0, &state->tmpsptolj, _state);
    state->needfi = ae_true;
    rcopyv(n, &state->x, &state->xbase, _state);
    k = 0;
lbl_34:
    if( k>n-1 )
    {
        goto lbl_36;
    }
    vleft = state->xbase.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    vright = state->xbase.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    if( !((state->hasbndl.ptr.p_bool[k]&&vleft<state->bndl.ptr.p_double[k])||(state->hasbndu.ptr.p_bool[k]&&vright>state->bndu.ptr.p_double[k])) )
    {
        goto lbl_37;
    }
    
    /*
     * 4-point formula violates box constraints, use 2-point uncentered one
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
        for(i=0; i<=nnlc; i++)
        {
            state->j.ptr.pp_double[i][k] = (double)(0);
        }
        goto lbl_35;
    }
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = vleft;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    rcopyv(1+nnlc, &state->fi, &state->fm1, _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = vright;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    rcopyv(1+nnlc, &state->fi, &state->fp1, _state);
    for(i=0; i<=nnlc; i++)
    {
        state->j.ptr.pp_double[i][k] = (state->fp1.ptr.p_double[i]-state->fm1.ptr.p_double[i])/(vright-vleft);
    }
    state->repnfev = state->repnfev+2;
    state->tmpsptolj.ptr.p_double[k] = (double)10*ae_machineepsilon/(vright-vleft);
    goto lbl_38;
lbl_37:
    
    /*
     * Use 4-point central formula
     */
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    rcopyv(1+nnlc, &state->fi, &state->fm2, _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    rcopyv(1+nnlc, &state->fi, &state->fm1, _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    rcopyv(1+nnlc, &state->fi, &state->fp1, _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    rcopyv(1+nnlc, &state->fi, &state->fp2, _state);
    for(i=0; i<=nnlc; i++)
    {
        state->j.ptr.pp_double[i][k] = ((double)8*(state->fp1.ptr.p_double[i]-state->fm1.ptr.p_double[i])-(state->fp2.ptr.p_double[i]-state->fm2.ptr.p_double[i]))/((double)6*state->diffstep*state->s.ptr.p_double[k]);
    }
    state->repnfev = state->repnfev+4;
    state->tmpsptolj.ptr.p_double[k] = ae_fabs((double)10*ae_machineepsilon*(double)4/((double)3*state->diffstep*state->s.ptr.p_double[k]), _state);
lbl_38:
lbl_35:
    k = k+1;
    goto lbl_34;
lbl_36:
    rcopyv(n, &state->xbase, &state->x, _state);
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->needfi = ae_false;
    state->repnfev = state->repnfev+1;
    
    /*
     * Sparsify Jacobian by zeroing components whose deviation from zero can be
     * explained by numerical rounding errors.
     *
     * This part is essential for the performance of sparse optimizers which can
     * face severe performance breakdown on Jacobians which became dense due to
     * numerical rounding errors.
     */
    rallocv(1+nnlc, &state->tmpspaf, _state);
    for(i=0; i<=nnlc; i++)
    {
        state->tmpspaf.ptr.p_double[i] = ae_fabs(state->fi.ptr.p_double[i], _state);
    }
    for(i=0; i<=nnlc; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( ae_fabs(state->j.ptr.pp_double[i][j], _state)<=state->tmpspaf.ptr.p_double[i]*state->tmpsptolj.ptr.p_double[j] )
            {
                state->j.ptr.pp_double[i][j] = (double)(0);
            }
        }
    }
lbl_33:
    goto lbl_31;
lbl_30:
    
    /*
     * User-supplied Jacobian
     */
    if( state->protocolversion!=2 )
    {
        goto lbl_39;
    }
    state->requesttype = 2;
    state->querysize = 1;
    state->queryfuncs = 1+nnlc;
    state->queryvars = n;
    state->querydim = 0;
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    goto lbl_40;
lbl_39:
    ae_assert(state->protocolversion==1, "MINNLC: integrity check 8659 failed", _state);
    ae_assert(state->j.rows==1+nnlc&&state->j.cols==n, "MINNLC: integrity check 4236 failed", _state);
    state->needfij = ae_true;
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->needfij = ae_false;
lbl_40:
    state->repnfev = state->repnfev+1;
lbl_31:
    b = ae_true;
lbl_28:
    ae_assert(b, "MINNLC: integrity check 6323 failed", _state);
    
    /*
     * Send results back to the optimizer
     */
    ae_assert(originalrequest==-1||originalrequest==2, "MINNLC: integrity check 6336 failed", _state);
    b = ae_false;
    if( originalrequest==-1 )
    {
        
        /*
         * Do nothing
         */
        b = ae_true;
    }
    if( originalrequest==2 )
    {
        if( state->solvertype==0 )
        {
            
            /*
             * AUL solver
             */
            ae_assert(state->protocolversion==1||state->protocolversion==2, "MINNLC: integrity check 1435 failed", _state);
            if( state->protocolversion==2 )
            {
                rcopyv(1+nnlc, &state->replyfi, &state->aulsolverstate.fi, _state);
            }
            else
            {
                rcopyv(1+nnlc, &state->fi, &state->aulsolverstate.fi, _state);
            }
            state->aulsolverstate.sj.matrixtype = 1;
            state->aulsolverstate.sj.m = 1+nnlc;
            state->aulsolverstate.sj.n = n;
            iallocv(state->aulsolverstate.sj.m+1, &state->aulsolverstate.sj.ridx, _state);
            state->aulsolverstate.sj.ridx.ptr.p_int[0] = 0;
            offs = 0;
            for(i=0; i<=nnlc; i++)
            {
                igrowv(offs+n, &state->aulsolverstate.sj.idx, _state);
                rgrowv(offs+n, &state->aulsolverstate.sj.vals, _state);
                if( state->protocolversion==2 )
                {
                    for(j=0; j<=n-1; j++)
                    {
                        if( state->replydj.ptr.p_double[i*n+j]!=0.0 )
                        {
                            state->aulsolverstate.sj.idx.ptr.p_int[offs] = j;
                            state->aulsolverstate.sj.vals.ptr.p_double[offs] = state->replydj.ptr.p_double[i*n+j]*state->s.ptr.p_double[j];
                            offs = offs+1;
                        }
                    }
                }
                else
                {
                    for(j=0; j<=n-1; j++)
                    {
                        if( state->j.ptr.pp_double[i][j]!=0.0 )
                        {
                            state->aulsolverstate.sj.idx.ptr.p_int[offs] = j;
                            state->aulsolverstate.sj.vals.ptr.p_double[offs] = state->j.ptr.pp_double[i][j]*state->s.ptr.p_double[j];
                            offs = offs+1;
                        }
                    }
                }
                state->aulsolverstate.sj.ridx.ptr.p_int[i+1] = offs;
            }
            state->aulsolverstate.sj.ninitialized = offs;
            sparseinitduidx(&state->aulsolverstate.sj, _state);
            b = ae_true;
        }
        if( state->solvertype==1 )
        {
            
            /*
             * SLP solver
             */
            if( state->protocolversion==2 )
            {
                state->slpsolverstate.fi.ptr.p_double[0] = state->replyfi.ptr.p_double[0];
                rcopyvr(n, &state->replydj, &state->slpsolverstate.j, 0, _state);
                rmergemulvr(n, &state->s, &state->slpsolverstate.j, 0, _state);
                for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
                {
                    state->slpsolverstate.fi.ptr.p_double[i+1] = state->replyfi.ptr.p_double[1+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
                    k = 1+state->nlcidx.ptr.p_int[i];
                    for(j=0; j<=n-1; j++)
                    {
                        state->slpsolverstate.j.ptr.pp_double[i+1][j] = state->replydj.ptr.p_double[k*n+j];
                    }
                    rmergemulvr(n, &state->s, &state->slpsolverstate.j, i+1, _state);
                    rmulr(n, state->nlcmul.ptr.p_double[i], &state->slpsolverstate.j, i+1, _state);
                }
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 4136 failed", _state);
                state->slpsolverstate.fi.ptr.p_double[0] = state->fi.ptr.p_double[0];
                rcopyrr(n, &state->j, 0, &state->slpsolverstate.j, 0, _state);
                rmergemulvr(n, &state->s, &state->slpsolverstate.j, 0, _state);
                for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
                {
                    state->slpsolverstate.fi.ptr.p_double[i+1] = state->fi.ptr.p_double[1+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
                    rcopyrr(n, &state->j, 1+state->nlcidx.ptr.p_int[i], &state->slpsolverstate.j, i+1, _state);
                    rmergemulvr(n, &state->s, &state->slpsolverstate.j, i+1, _state);
                    rmulr(n, state->nlcmul.ptr.p_double[i], &state->slpsolverstate.j, i+1, _state);
                }
            }
            b = ae_true;
        }
        if( state->solvertype==2||state->solvertype==3 )
        {
            
            /*
             * SQP solver
             */
            if( state->protocolversion==2 )
            {
                state->sqpsolverstate.fi.ptr.p_double[0] = state->replyfi.ptr.p_double[0];
                rcopyvr(n, &state->replydj, &state->sqpsolverstate.j, 0, _state);
                rmergemulvr(n, &state->s, &state->sqpsolverstate.j, 0, _state);
                for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
                {
                    state->sqpsolverstate.fi.ptr.p_double[i+1] = state->replyfi.ptr.p_double[1+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
                    k = 1+state->nlcidx.ptr.p_int[i];
                    for(j=0; j<=n-1; j++)
                    {
                        state->sqpsolverstate.j.ptr.pp_double[i+1][j] = state->replydj.ptr.p_double[k*n+j];
                    }
                    rmergemulvr(n, &state->s, &state->sqpsolverstate.j, i+1, _state);
                    rmulr(n, state->nlcmul.ptr.p_double[i], &state->sqpsolverstate.j, i+1, _state);
                }
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 2222 failed", _state);
                state->sqpsolverstate.fi.ptr.p_double[0] = state->fi.ptr.p_double[0];
                rcopyrr(n, &state->j, 0, &state->sqpsolverstate.j, 0, _state);
                rmergemulvr(n, &state->s, &state->sqpsolverstate.j, 0, _state);
                for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
                {
                    state->sqpsolverstate.fi.ptr.p_double[i+1] = state->fi.ptr.p_double[1+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
                    rcopyrr(n, &state->j, 1+state->nlcidx.ptr.p_int[i], &state->sqpsolverstate.j, i+1, _state);
                    rmergemulvr(n, &state->s, &state->sqpsolverstate.j, i+1, _state);
                    rmulr(n, state->nlcmul.ptr.p_double[i], &state->sqpsolverstate.j, i+1, _state);
                }
            }
            b = ae_true;
        }
        if( state->solvertype==4||state->solvertype==5 )
        {
            
            /*
             * FSQP solver
             */
            if( state->protocolversion==2 )
            {
                state->fsqpsolverstate.fi.ptr.p_double[0] = state->replyfi.ptr.p_double[0];
                rcopyvr(n, &state->replydj, &state->fsqpsolverstate.j, 0, _state);
                rmergemulvr(n, &state->s, &state->fsqpsolverstate.j, 0, _state);
                for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
                {
                    state->fsqpsolverstate.fi.ptr.p_double[i+1] = state->replyfi.ptr.p_double[1+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
                    k = 1+state->nlcidx.ptr.p_int[i];
                    for(j=0; j<=n-1; j++)
                    {
                        state->fsqpsolverstate.j.ptr.pp_double[i+1][j] = state->replydj.ptr.p_double[k*n+j];
                    }
                    rmergemulvr(n, &state->s, &state->fsqpsolverstate.j, i+1, _state);
                    rmulr(n, state->nlcmul.ptr.p_double[i], &state->fsqpsolverstate.j, i+1, _state);
                }
            }
            else
            {
                ae_assert(state->protocolversion==1, "MINNLC: integrity check 2222 failed", _state);
                state->fsqpsolverstate.fi.ptr.p_double[0] = state->fi.ptr.p_double[0];
                rcopyrr(n, &state->j, 0, &state->fsqpsolverstate.j, 0, _state);
                rmergemulvr(n, &state->s, &state->fsqpsolverstate.j, 0, _state);
                for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
                {
                    state->fsqpsolverstate.fi.ptr.p_double[i+1] = state->fi.ptr.p_double[1+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
                    rcopyrr(n, &state->j, 1+state->nlcidx.ptr.p_int[i], &state->fsqpsolverstate.j, i+1, _state);
                    rmergemulvr(n, &state->s, &state->fsqpsolverstate.j, i+1, _state);
                    rmulr(n, state->nlcmul.ptr.p_double[i], &state->fsqpsolverstate.j, i+1, _state);
                }
            }
            b = ae_true;
        }
    }
    ae_assert(b, "MINNLC: integrity check 0038 failed", _state);
    goto lbl_20;
lbl_21:
    
    /*
     * Results phase
     */
    b = ae_false;
    if( state->solvertype==0 )
    {
        
        /*
         * AUL results
         */
        state->repterminationtype = state->aulsolverstate.repterminationtype;
        state->repouteriterationscount = 1;
        state->repinneriterationscount = state->aulsolverstate.repiterationscount;
        state->repbcerr = state->aulsolverstate.repbcerr;
        state->repbcidx = state->aulsolverstate.repbcidx;
        state->replcerr = state->aulsolverstate.replcerr;
        state->replcidx = state->aulsolverstate.replcidx;
        state->repnlcerr = state->aulsolverstate.repnlcerr;
        state->repnlcidx = state->aulsolverstate.repnlcidx;
        minnlc_unscale(state, &state->aulsolverstate.xtruebest.x, &state->aulsolverstate.scaledbndl, &state->aulsolverstate.scaledbndu, &state->xc, _state);
        b = ae_true;
    }
    if( state->solvertype==1 )
    {
        
        /*
         * SLP results
         */
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
        b = ae_true;
    }
    if( state->solvertype==2||state->solvertype==3 )
    {
        state->repterminationtype = state->sqpsolverstate.repterminationtype;
        state->repouteriterationscount = state->sqpsolverstate.repiterationscount;
        state->repinneriterationscount = state->sqpsolverstate.repiterationscount;
        state->repbcerr = state->sqpsolverstate.repbcerr;
        state->repbcidx = state->sqpsolverstate.repbcidx;
        state->replcerr = state->sqpsolverstate.replcerr;
        state->replcidx = state->sqpsolverstate.replcidx;
        state->repnlcerr = state->sqpsolverstate.repnlcerr;
        state->repnlcidx = state->sqpsolverstate.repnlcidx;
        minnlc_unscale(state, &state->sqpsolverstate.stepk.x, &state->sqpsolverstate.scaledbndl, &state->sqpsolverstate.scaledbndu, &state->xc, _state);
        b = ae_true;
    }
    if( state->solvertype==4||state->solvertype==5 )
    {
        state->repterminationtype = state->fsqpsolverstate.repterminationtype;
        state->repouteriterationscount = state->fsqpsolverstate.repiterationscount;
        state->repinneriterationscount = state->fsqpsolverstate.repiterationscount;
        state->repbcerr = state->fsqpsolverstate.repbcerr;
        state->repbcidx = state->fsqpsolverstate.repbcidx;
        state->replcerr = state->fsqpsolverstate.replcerr;
        state->replcidx = state->fsqpsolverstate.replcidx;
        state->repnlcerr = state->fsqpsolverstate.repnlcerr;
        state->repnlcidx = state->fsqpsolverstate.repnlcidx;
        minnlc_unscale(state, &state->fsqpsolverstate.stepk.x, &state->fsqpsolverstate.scaledbndl, &state->fsqpsolverstate.scaledbndu, &state->xc, _state);
        b = ae_true;
    }
    ae_assert(b, "MINNLC: integrity check 2219 failed", _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = i;
    state->rstate.ia.ptr.p_int[1] = j;
    state->rstate.ia.ptr.p_int[2] = k;
    state->rstate.ia.ptr.p_int[3] = n;
    state->rstate.ia.ptr.p_int[4] = nnlc;
    state->rstate.ia.ptr.p_int[5] = offs;
    state->rstate.ia.ptr.p_int[6] = originalrequest;
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
                        NAN  values  in  function/gradient,  recovery  was
                        impossible. Abnormal termination signalled.
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
                       
                === ADDITIONAL CODES ===
                * +800      if   during  algorithm  execution  the  solver
                            encountered NAN/INF values in  the  target  or
                            constraints but managed to recover by reducing
                            trust region radius, the  solver  returns  one
                            of SUCCESS codes but adds +800 to the code.
                
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
    ae_vector_set_length(&state->rstate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 1+1, _state);
    state->rstate.stage = -1;
    minnlc_clearrequestfields(state, _state);
}


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void minnlcsetprotocolv1(minnlcstate* state, ae_state *_state)
{


    state->protocolversion = 1;
}


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void minnlcsetprotocolv2(minnlcstate* state, ae_state *_state)
{


    state->protocolversion = 2;
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

    state->protocolversion = 1;
    
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
    critinitdefault(&state->criteria, _state);
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
    minnlcsetxrep(state, ae_false, _state);
    minnlcsetalgosqp(state, _state);
    minnlcsetstpmax(state, 0.0, _state);
    minnlcrestartfrom(state, x, _state);
    ae_frame_leave(_state);
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
    _nlpstoppingcriteria_init(&p->criteria, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->cleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lcsrcidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->nl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->reportx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->querydata, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replyfi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replydj, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->replysj, _state, make_automatic);
    ae_vector_init(&p->tmpx1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpc1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpf1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpg1, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpj1, 0, 0, DT_REAL, _state, make_automatic);
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
    ae_vector_init(&p->tmpspaf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpsptolj, 0, DT_REAL, _state, make_automatic);
    _minaulstate_init(&p->aulsolverstate, _state, make_automatic);
    _sparsematrix_init(&p->sparsea, _state, make_automatic);
    ae_vector_init(&p->al, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->au, 0, DT_REAL, _state, make_automatic);
    _minslpstate_init(&p->slpsolverstate, _state, make_automatic);
    _minsqpstate_init(&p->sqpsolverstate, _state, make_automatic);
    _minfsqpstate_init(&p->fsqpsolverstate, _state, make_automatic);
    _smoothnessmonitor_init(&p->smonitor, _state, make_automatic);
    ae_vector_init(&p->lastscaleused, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nlcidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->nlcmul, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nlcadd, 0, DT_REAL, _state, make_automatic);
}


void _minnlcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minnlcstate       *dst = (minnlcstate*)_dst;
    const minnlcstate *src = (const minnlcstate*)_src;
    dst->stabilizingpoint = src->stabilizingpoint;
    dst->initialinequalitymultiplier = src->initialinequalitymultiplier;
    dst->solvertype = src->solvertype;
    dst->n = src->n;
    _nlpstoppingcriteria_init_copy(&dst->criteria, &src->criteria, _state, make_automatic);
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
    dst->nnlc = src->nnlc;
    ae_vector_init_copy(&dst->nl, &src->nl, _state, make_automatic);
    ae_vector_init_copy(&dst->nu, &src->nu, _state, make_automatic);
    dst->protocolversion = src->protocolversion;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    dst->needfij = src->needfij;
    dst->needfi = src->needfi;
    dst->xupdated = src->xupdated;
    dst->requesttype = src->requesttype;
    ae_vector_init_copy(&dst->reportx, &src->reportx, _state, make_automatic);
    dst->reportf = src->reportf;
    dst->querysize = src->querysize;
    dst->queryfuncs = src->queryfuncs;
    dst->queryvars = src->queryvars;
    dst->querydim = src->querydim;
    dst->queryformulasize = src->queryformulasize;
    ae_vector_init_copy(&dst->querydata, &src->querydata, _state, make_automatic);
    ae_vector_init_copy(&dst->replyfi, &src->replyfi, _state, make_automatic);
    ae_vector_init_copy(&dst->replydj, &src->replydj, _state, make_automatic);
    _sparsematrix_init_copy(&dst->replysj, &src->replysj, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpx1, &src->tmpx1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpc1, &src->tmpc1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpf1, &src->tmpf1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpg1, &src->tmpg1, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpj1, &src->tmpj1, _state, make_automatic);
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
    ae_vector_init_copy(&dst->tmpspaf, &src->tmpspaf, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpsptolj, &src->tmpsptolj, _state, make_automatic);
    _minaulstate_init_copy(&dst->aulsolverstate, &src->aulsolverstate, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsea, &src->sparsea, _state, make_automatic);
    ae_vector_init_copy(&dst->al, &src->al, _state, make_automatic);
    ae_vector_init_copy(&dst->au, &src->au, _state, make_automatic);
    dst->userterminationneeded = src->userterminationneeded;
    _minslpstate_init_copy(&dst->slpsolverstate, &src->slpsolverstate, _state, make_automatic);
    _minsqpstate_init_copy(&dst->sqpsolverstate, &src->sqpsolverstate, _state, make_automatic);
    _minfsqpstate_init_copy(&dst->fsqpsolverstate, &src->fsqpsolverstate, _state, make_automatic);
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
    ae_vector_init_copy(&dst->nlcidx, &src->nlcidx, _state, make_automatic);
    ae_vector_init_copy(&dst->nlcmul, &src->nlcmul, _state, make_automatic);
    ae_vector_init_copy(&dst->nlcadd, &src->nlcadd, _state, make_automatic);
    dst->nlcnlec = src->nlcnlec;
    dst->nlcnlic = src->nlcnlic;
}


void _minnlcstate_clear(void* _p)
{
    minnlcstate *p = (minnlcstate*)_p;
    ae_touch_ptr((void*)p);
    _nlpstoppingcriteria_clear(&p->criteria);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_matrix_clear(&p->cleic);
    ae_vector_clear(&p->lcsrcidx);
    ae_vector_clear(&p->nl);
    ae_vector_clear(&p->nu);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    ae_vector_clear(&p->reportx);
    ae_vector_clear(&p->querydata);
    ae_vector_clear(&p->replyfi);
    ae_vector_clear(&p->replydj);
    _sparsematrix_clear(&p->replysj);
    ae_vector_clear(&p->tmpx1);
    ae_vector_clear(&p->tmpc1);
    ae_vector_clear(&p->tmpf1);
    ae_vector_clear(&p->tmpg1);
    ae_matrix_clear(&p->tmpj1);
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
    ae_vector_clear(&p->tmpspaf);
    ae_vector_clear(&p->tmpsptolj);
    _minaulstate_clear(&p->aulsolverstate);
    _sparsematrix_clear(&p->sparsea);
    ae_vector_clear(&p->al);
    ae_vector_clear(&p->au);
    _minslpstate_clear(&p->slpsolverstate);
    _minsqpstate_clear(&p->sqpsolverstate);
    _minfsqpstate_clear(&p->fsqpsolverstate);
    _smoothnessmonitor_clear(&p->smonitor);
    ae_vector_clear(&p->lastscaleused);
    ae_vector_clear(&p->nlcidx);
    ae_vector_clear(&p->nlcmul);
    ae_vector_clear(&p->nlcadd);
}


void _minnlcstate_destroy(void* _p)
{
    minnlcstate *p = (minnlcstate*)_p;
    ae_touch_ptr((void*)p);
    _nlpstoppingcriteria_destroy(&p->criteria);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_matrix_destroy(&p->cleic);
    ae_vector_destroy(&p->lcsrcidx);
    ae_vector_destroy(&p->nl);
    ae_vector_destroy(&p->nu);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    ae_vector_destroy(&p->reportx);
    ae_vector_destroy(&p->querydata);
    ae_vector_destroy(&p->replyfi);
    ae_vector_destroy(&p->replydj);
    _sparsematrix_destroy(&p->replysj);
    ae_vector_destroy(&p->tmpx1);
    ae_vector_destroy(&p->tmpc1);
    ae_vector_destroy(&p->tmpf1);
    ae_vector_destroy(&p->tmpg1);
    ae_matrix_destroy(&p->tmpj1);
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
    ae_vector_destroy(&p->tmpspaf);
    ae_vector_destroy(&p->tmpsptolj);
    _minaulstate_destroy(&p->aulsolverstate);
    _sparsematrix_destroy(&p->sparsea);
    ae_vector_destroy(&p->al);
    ae_vector_destroy(&p->au);
    _minslpstate_destroy(&p->slpsolverstate);
    _minsqpstate_destroy(&p->sqpsolverstate);
    _minfsqpstate_destroy(&p->fsqpsolverstate);
    _smoothnessmonitor_destroy(&p->smonitor);
    ae_vector_destroy(&p->lastscaleused);
    ae_vector_destroy(&p->nlcidx);
    ae_vector_destroy(&p->nlcmul);
    ae_vector_destroy(&p->nlcadd);
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
