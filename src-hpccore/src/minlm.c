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
#include <stdio.h>
#include "minlm.h"


/*$ Declarations $*/
static double minlm_lambdaup = 2.0;
static double minlm_lambdadown = 0.33;
static double minlm_suspiciousnu = 16;
static ae_int_t minlm_smallmodelage = 3;
static ae_int_t minlm_additers = 5;
static double minlm_stagnationfeps = 1.0E-12;
static ae_int_t minlm_stagnationfits = 20;
static void minlm_lmprepare(ae_int_t n,
     ae_int_t m,
     ae_bool havegrad,
     minlmstate* state,
     ae_state *_state);
static void minlm_clearrequestfields(minlmstate* state, ae_state *_state);
static ae_bool minlm_increaselambda(double* lambdav,
     double* nu,
     ae_state *_state);
static void minlm_decreaselambda(double* lambdav,
     double* nu,
     ae_state *_state);
static ae_int_t minlm_checkdecrease(/* Real    */ const ae_matrix* quadraticmodel,
     /* Real    */ const ae_vector* gbase,
     double fbase,
     ae_int_t n,
     /* Real    */ const ae_vector* deltax,
     double fnew,
     double* lambdav,
     double* nu,
     ae_state *_state);
static ae_bool minlm_minlmstepfinderinit(minlmstepfinder* state,
     ae_int_t n,
     ae_int_t m,
     ae_int_t maxmodelage,
     /* Real    */ ae_vector* xbase,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     /* Real    */ const ae_vector* s,
     double stpmax,
     double epsx,
     ae_state *_state);
static void minlm_minlmstepfinderstart(minlmstepfinder* state,
     /* Real    */ const ae_matrix* quadraticmodel,
     /* Real    */ const ae_vector* gbase,
     double fbase,
     /* Real    */ const ae_vector* xbase,
     /* Real    */ const ae_vector* fibase,
     ae_int_t modelage,
     ae_state *_state);
static ae_bool minlm_minlmstepfinderiteration(minlmstepfinder* state,
     double* lambdav,
     double* nu,
     /* Real    */ ae_vector* xnew,
     /* Real    */ ae_vector* deltax,
     ae_bool* deltaxready,
     /* Real    */ ae_vector* deltaf,
     ae_bool* deltafready,
     ae_int_t* iflag,
     double* fnew,
     ae_int_t* ncholesky,
     ae_state *_state);


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
     ae_state *_state)
{

    _minlmstate_clear(state);

    ae_assert(n>=1, "MinLMCreateVJ: N<1!", _state);
    ae_assert(m>=1, "MinLMCreateVJ: M<1!", _state);
    ae_assert(x->cnt>=n, "MinLMCreateVJ: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinLMCreateVJ: X contains infinite or NaN values!", _state);
    
    /*
     * initialize, check parameters
     */
    state->protocolversion = 1;
    state->teststep = (double)(0);
    state->n = n;
    state->m = m;
    state->algomode = 1;
    
    /*
     * second stage of initialization
     */
    minlm_lmprepare(n, m, ae_false, state, _state);
    minlmsetacctype(state, 0, _state);
    minlmsetcond(state, (double)(0), 0, _state);
    minlmsetxrep(state, ae_false, _state);
    minlmsetstpmax(state, (double)(0), _state);
    minlmrestartfrom(state, x, _state);
}


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
     ae_state *_state)
{

    _minlmstate_clear(state);

    ae_assert(ae_isfinite(diffstep, _state), "MinLMCreateV: DiffStep is not finite!", _state);
    ae_assert(ae_fp_greater(diffstep,(double)(0)), "MinLMCreateV: DiffStep<=0!", _state);
    ae_assert(n>=1, "MinLMCreateV: N<1!", _state);
    ae_assert(m>=1, "MinLMCreateV: M<1!", _state);
    ae_assert(x->cnt>=n, "MinLMCreateV: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "MinLMCreateV: X contains infinite or NaN values!", _state);
    
    /*
     * Initialize
     */
    state->protocolversion = 1;
    state->teststep = (double)(0);
    state->n = n;
    state->m = m;
    state->algomode = 0;
    state->diffstep = diffstep;
    
    /*
     * Second stage of initialization
     */
    minlm_lmprepare(n, m, ae_false, state, _state);
    minlmsetacctype(state, 1, _state);
    minlmsetcond(state, (double)(0), 0, _state);
    minlmsetxrep(state, ae_false, _state);
    minlmsetstpmax(state, (double)(0), _state);
    minlmrestartfrom(state, x, _state);
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsx, _state), "MinLMSetCond: EpsX is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinLMSetCond: negative EpsX!", _state);
    ae_assert(maxits>=0, "MinLMSetCond: negative MaxIts!", _state);
    if( ae_fp_eq(epsx,(double)(0))&&maxits==0 )
    {
        epsx = 1.0E-9;
    }
    state->epsx = epsx;
    state->maxits = maxits;
}


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
void minlmsetxrep(minlmstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
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

NOTE: non-zero StpMax leads to moderate  performance  degradation  because
intermediate  step  of  preconditioned L-BFGS optimization is incompatible
with limits on step size.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetstpmax(minlmstate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "MinLMSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,(double)(0)), "MinLMSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


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
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinLMSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinLMSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "MinLMSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "MinLMSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinLMSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinLMSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinLMSetBC: BndU contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->havebndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->havebndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(k>=0, "MinLMSetLC: K<0", _state);
    ae_assert(c->cols>=n+1||k==0, "MinLMSetLC: Cols(C)<N+1", _state);
    ae_assert(c->rows>=k, "MinLMSetLC: Rows(C)<K", _state);
    ae_assert(ct->cnt>=k, "MinLMSetLC: Length(CT)<K", _state);
    ae_assert(apservisfinitematrix(c, k, n+1, _state), "MinLMSetLC: C contains infinite or NaN values!", _state);
    
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
}


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
     ae_state *_state)
{


    ae_assert((acctype==0||acctype==1)||acctype==2, "MinLMSetAccType: incorrect AccType!", _state);
    if( acctype==2 )
    {
        acctype = 0;
    }
    if( acctype==0 )
    {
        state->maxmodelage = 0;
        state->makeadditers = ae_false;
        return;
    }
    if( acctype==1 )
    {
        if( state->algomode==0 )
        {
            state->maxmodelage = 2*state->n;
        }
        else
        {
            state->maxmodelage = minlm_smallmodelage;
        }
        state->makeadditers = ae_false;
        return;
    }
}


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
ae_bool minlmiteration(minlmstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_bool bflag;
    ae_int_t iflag;
    double v;
    double xl;
    double xr;
    double s;
    double t;
    double fnew;
    ae_int_t i;
    ae_int_t k;
    ae_bool dotrace;
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
        iflag = state->rstate.ia.ptr.p_int[2];
        i = state->rstate.ia.ptr.p_int[3];
        k = state->rstate.ia.ptr.p_int[4];
        bflag = state->rstate.ba.ptr.p_bool[0];
        dotrace = state->rstate.ba.ptr.p_bool[1];
        v = state->rstate.ra.ptr.p_double[0];
        xl = state->rstate.ra.ptr.p_double[1];
        xr = state->rstate.ra.ptr.p_double[2];
        s = state->rstate.ra.ptr.p_double[3];
        t = state->rstate.ra.ptr.p_double[4];
        fnew = state->rstate.ra.ptr.p_double[5];
    }
    else
    {
        n = 359;
        m = -58;
        iflag = -919;
        i = -909;
        k = 81;
        bflag = ae_true;
        dotrace = ae_false;
        v = -788.0;
        xl = 809.0;
        xr = 205.0;
        s = -838.0;
        t = 939.0;
        fnew = -526.0;
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
    if( state->rstate.stage==25 )
    {
        goto lbl_25;
    }
    
    /*
     * Routine body
     */
    
    /*
     * prepare
     */
    dotrace = ae_is_trace_enabled("LM");
    n = state->n;
    m = state->m;
    state->repiterationscount = 0;
    state->repterminationtype = 0;
    state->repnfunc = 0;
    state->repnjac = 0;
    state->repngrad = 0;
    state->repnhess = 0;
    state->repncholesky = 0;
    state->userterminationneeded = ae_false;
    if( m>0 )
    {
        smoothnessmonitorinit(&state->smonitor, &state->s, n, m, ae_false, _state);
    }
    for(i=0; i<=n-1; i++)
    {
        state->lastscaleused.ptr.p_double[i] = state->s.ptr.p_double[i];
    }
    
    /*
     * Allocate temporaries, as mandated by the V2 protocol
     */
    if( state->protocolversion==2 )
    {
        rallocm(m, n, &state->tmpj1, _state);
        rallocv(m, &state->tmpf1, _state);
        rallocv(n, &state->tmpg1, _state);
        rallocv(n, &state->tmpx1, _state);
    }
    
    /*
     * Trace output (if needed)
     */
    if( dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  LM SOLVER STARTED                                                                             //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("N             = %6d\n",
            (int)(n));
        ae_trace("M             = %6d\n",
            (int)(m));
    }
    
    /*
     * Prepare LM step finder and enforce/check feasibility of constraints
     */
    if( !minlm_minlmstepfinderinit(&state->finderstate, n, m, state->maxmodelage, &state->xbase, &state->bndl, &state->bndu, &state->cleic, state->nec, state->nic, &state->s, state->stpmax, state->epsx, _state) )
    {
        state->repterminationtype = -3;
        result = ae_false;
        return result;
    }
    
    /*
     * set constraints for obsolete QP solver
     */
    minqpsetbc(&state->qpstate, &state->bndl, &state->bndu, _state);
    
    /*
     *  Check correctness of the analytic Jacobian
     */
    if( state->protocolversion==1 )
    {
        minlm_clearrequestfields(state, _state);
    }
    if( !(state->algomode==1&&ae_fp_greater(state->teststep,(double)(0))) )
    {
        goto lbl_26;
    }
    ae_assert(m>0, "MinLM: integrity check failed", _state);
lbl_28:
    if( !smoothnessmonitorcheckgradientatx0(&state->smonitor, &state->xbase, &state->s, &state->bndl, &state->bndu, ae_true, state->teststep, _state) )
    {
        goto lbl_29;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_30;
    }
    
    /*
     * Use V2 reverse communication protocol
     */
    state->requesttype = 2;
    state->querysize = 1;
    state->queryfuncs = m;
    state->queryvars = n;
    state->querydim = 0;
    rcopyallocv(n, &state->smonitor.x, &state->querydata, _state);
    rallocv(m, &state->replyfi, _state);
    rallocv(m*n, &state->replydj, _state);
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    rcopyv(m, &state->replyfi, &state->smonitor.fi, _state);
    unpackdj(m, n, &state->replydj, &state->smonitor.j, _state);
    goto lbl_31;
lbl_30:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 0125 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = state->smonitor.x.ptr.p_double[i];
    }
    state->needfij = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfij = ae_false;
    for(i=0; i<=m-1; i++)
    {
        state->smonitor.fi.ptr.p_double[i] = state->fi.ptr.p_double[i];
        for(k=0; k<=n-1; k++)
        {
            state->smonitor.j.ptr.pp_double[i][k] = state->j.ptr.pp_double[i][k];
        }
    }
lbl_31:
    goto lbl_28;
lbl_29:
lbl_26:
    
    /*
     * Initial evaluation of the target and report of the current point
     */
    if( !state->xrep )
    {
        goto lbl_32;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_34;
    }
    
    /*
     * Issue request and perform report using V2 protocol
     */
    rcopyv(n, &state->xbase, &state->x, _state);
    state->requesttype = 4;
    state->querysize = 1;
    state->queryfuncs = m;
    state->queryvars = n;
    state->querydim = 0;
    rcopyallocv(n, &state->x, &state->querydata, _state);
    rallocv(m, &state->replyfi, _state);
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->repnfunc = state->repnfunc+1;
    rcopyv(m, &state->replyfi, &state->fi, _state);
    state->f = rdotv2(m, &state->fi, _state);
    state->requesttype = -1;
    state->queryvars = n;
    state->reportf = state->f;
    rcopyallocv(n, &state->x, &state->reportx, _state);
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    goto lbl_35;
lbl_34:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 9600 failed", _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    minlm_clearrequestfields(state, _state);
    state->needfi = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needfi = ae_false;
    state->f = rdotv2(m, &state->fi, _state);
    state->repnfunc = state->repnfunc+1;
    rcopyv(n, &state->xbase, &state->x, _state);
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->xupdated = ae_false;
lbl_35:
lbl_32:
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        rcopyv(n, &state->xbase, &state->x, _state);
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    
    /*
     * Prepare control variables
     */
    state->fstagnationcnt = 0;
    state->nu = (double)(1);
    state->lambdav = -ae_maxrealnumber;
    state->modelage = state->maxmodelage+1;
    state->deltaxready = ae_false;
    state->deltafready = ae_false;
    
    /*
     * fvector/Jacobian-based optimization mode
     *
     * Main cycle.
     *
     * We move through it until either:
     * * one of the stopping conditions is met
     * * we decide that stopping conditions are too stringent
     *   and break from cycle
     */
lbl_36:
    if( ae_false )
    {
        goto lbl_37;
    }
    if( dotrace )
    {
        ae_trace("\n=== ITERATION %5d ==================================================================================\n",
            (int)(state->repiterationscount));
        ae_trace("ModelAge      = %9d    (model age)\n",
            (int)(state->modelage));
        ae_trace("LambdaV       = %9.3e    (damping coefficient)\n",
            (double)(state->lambdav));
    }
    
    /*
     * First, we have to prepare quadratic model for our function.
     * We use BFlag to ensure that model is prepared;
     * if it is false at the end of this block, something went wrong.
     *
     * We may either calculate brand new model or update old one.
     *
     * Before this block we have:
     * * State.XBase            - current position.
     * * State.DeltaX           - if DeltaXReady is True
     * * State.DeltaF           - if DeltaFReady is True
     *
     * After this block is over, we will have:
     * * State.XBase            - base point (unchanged)
     * * State.FBase            - F(XBase)
     * * State.GBase            - linear term
     * * State.QuadraticModel   - quadratic term
     * * State.LambdaV          - current estimate for lambda
     *
     * We also clear DeltaXReady/DeltaFReady flags
     * after initialization is done.
     */
    if( !(state->modelage>state->maxmodelage||!(state->deltaxready&&state->deltafready)) )
    {
        goto lbl_38;
    }
    
    /*
     * Refresh model (using either finite differences or analytic Jacobian)
     */
    if( state->protocolversion!=2 )
    {
        goto lbl_40;
    }
    
    /*
     * Issue request using V2 protocol
     */
    if( state->algomode!=0 )
    {
        goto lbl_42;
    }
    
    /*
     * Optimization using F values only. Use finite differences to estimate Jacobian.
     *
     * Request dense numerical Jacobian, propose 2-point finite difference formula
     */
    state->requesttype = 3;
    state->querysize = 1;
    state->queryfuncs = m;
    state->queryvars = n;
    state->querydim = 0;
    state->queryformulasize = 2;
    rallocv(n+n*2*state->queryformulasize, &state->querydata, _state);
    rcopyv(n, &state->xbase, &state->querydata, _state);
    for(k=0; k<=n-1; k++)
    {
        
        /*
         * We guard X[k] from leaving [BndL,BndU].
         * In case BndL=BndU, we assume that derivative in this direction is zero.
         */
        v = state->diffstep*state->s.ptr.p_double[k];
        xl = state->xbase.ptr.p_double[k]-v;
        if( state->havebndl.ptr.p_bool[k]&&ae_fp_less(xl,state->bndl.ptr.p_double[k]) )
        {
            xl = state->bndl.ptr.p_double[k];
        }
        xr = state->xbase.ptr.p_double[k]+v;
        if( state->havebndu.ptr.p_bool[k]&&ae_fp_greater(xr,state->bndu.ptr.p_double[k]) )
        {
            xr = state->bndu.ptr.p_double[k];
        }
        v = (double)(0);
        if( xl<xr )
        {
            v = (double)1/(xr-xl);
        }
        state->querydata.ptr.p_double[n+2*state->queryformulasize*k+0*2+0] = xl;
        state->querydata.ptr.p_double[n+2*state->queryformulasize*k+0*2+1] = -v;
        state->querydata.ptr.p_double[n+2*state->queryformulasize*k+1*2+0] = xr;
        state->querydata.ptr.p_double[n+2*state->queryformulasize*k+1*2+1] = v;
    }
    rallocv(m, &state->replyfi, _state);
    rallocv(m*n, &state->replydj, _state);
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    rcopyv(m, &state->replyfi, &state->fi, _state);
    unpackdj(m, n, &state->replydj, &state->j, _state);
    state->repnfunc = state->repnfunc+1+n*state->queryformulasize;
    state->repnjac = state->repnjac+1;
    
    /*
     * New model
     */
    state->modelage = 0;
    goto lbl_43;
lbl_42:
    
    /*
     * Request dense analytic Jacobian
     */
    state->requesttype = 2;
    state->querysize = 1;
    state->queryfuncs = m;
    state->queryvars = n;
    state->querydim = 0;
    rcopyallocv(n, &state->xbase, &state->querydata, _state);
    rallocv(m, &state->replyfi, _state);
    rallocv(m*n, &state->replydj, _state);
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    rcopyv(m, &state->replyfi, &state->fi, _state);
    unpackdj(m, n, &state->replydj, &state->j, _state);
    state->repnfunc = state->repnfunc+1;
    state->repnjac = state->repnjac+1;
    
    /*
     * New model
     */
    state->modelage = 0;
lbl_43:
    goto lbl_41;
lbl_40:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 9600 failed", _state);
    if( state->algomode!=0 )
    {
        goto lbl_44;
    }
    
    /*
     * Optimization using F values only.
     * Use finite differences to estimate Jacobian.
     */
    k = 0;
lbl_46:
    if( k>n-1 )
    {
        goto lbl_48;
    }
    
    /*
     * We guard X[k] from leaving [BndL,BndU].
     * In case BndL=BndU, we assume that derivative in this direction is zero.
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    if( state->havebndl.ptr.p_bool[k] )
    {
        state->x.ptr.p_double[k] = ae_maxreal(state->x.ptr.p_double[k], state->bndl.ptr.p_double[k], _state);
    }
    if( state->havebndu.ptr.p_bool[k] )
    {
        state->x.ptr.p_double[k] = ae_minreal(state->x.ptr.p_double[k], state->bndu.ptr.p_double[k], _state);
    }
    state->xm1 = state->x.ptr.p_double[k];
    minlm_clearrequestfields(state, _state);
    state->needfi = ae_true;
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->repnfunc = state->repnfunc+1;
    ae_v_move(&state->fm1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    if( state->havebndl.ptr.p_bool[k] )
    {
        state->x.ptr.p_double[k] = ae_maxreal(state->x.ptr.p_double[k], state->bndl.ptr.p_double[k], _state);
    }
    if( state->havebndu.ptr.p_bool[k] )
    {
        state->x.ptr.p_double[k] = ae_minreal(state->x.ptr.p_double[k], state->bndu.ptr.p_double[k], _state);
    }
    state->xp1 = state->x.ptr.p_double[k];
    minlm_clearrequestfields(state, _state);
    state->needfi = ae_true;
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->repnfunc = state->repnfunc+1;
    ae_v_move(&state->fp1.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    v = state->xp1-state->xm1;
    if( ae_fp_neq(v,(double)(0)) )
    {
        v = (double)1/v;
        ae_v_moved(&state->j.ptr.pp_double[0][k], state->j.stride, &state->fp1.ptr.p_double[0], 1, ae_v_len(0,m-1), v);
        ae_v_subd(&state->j.ptr.pp_double[0][k], state->j.stride, &state->fm1.ptr.p_double[0], 1, ae_v_len(0,m-1), v);
    }
    else
    {
        for(i=0; i<=m-1; i++)
        {
            state->j.ptr.pp_double[i][k] = (double)(0);
        }
    }
    k = k+1;
    goto lbl_46;
lbl_48:
    
    /*
     * Calculate F(XBase)
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->needfi = ae_true;
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->needfi = ae_false;
    state->repnfunc = state->repnfunc+1;
    state->repnjac = state->repnjac+1;
    
    /*
     * New model
     */
    state->modelage = 0;
    goto lbl_45;
lbl_44:
    
    /*
     * Obtain f[] and Jacobian
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->needfij = ae_true;
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->needfij = ae_false;
    state->repnfunc = state->repnfunc+1;
    state->repnjac = state->repnjac+1;
    
    /*
     * New model
     */
    state->modelage = 0;
lbl_45:
lbl_41:
    goto lbl_39;
lbl_38:
    
    /*
     * State.J contains Jacobian or its current approximation;
     * refresh it using secant updates:
     *
     * f(x0+dx) = f(x0) + J*dx,
     * J_new = J_old + u*h'
     * h = x_new-x_old
     * u = (f_new - f_old - J_old*h)/(h'h)
     *
     * We can explicitly generate h and u, but it is
     * preferential to do in-place calculations. Only
     * I-th row of J_old is needed to calculate u[I],
     * so we can update J row by row in one pass.
     *
     * NOTE: we expect that State.XBase contains new point,
     * State.FBase contains old point, State.DeltaX and
     * State.DeltaY contain updates from last step.
     */
    ae_assert(state->deltaxready&&state->deltafready, "MinLMIteration: uninitialized DeltaX/DeltaF", _state);
    t = ae_v_dotproduct(&state->deltax.ptr.p_double[0], 1, &state->deltax.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_assert(ae_fp_neq(t,(double)(0)), "MinLM: internal error (T=0)", _state);
    for(i=0; i<=m-1; i++)
    {
        v = ae_v_dotproduct(&state->j.ptr.pp_double[i][0], 1, &state->deltax.ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = (state->deltaf.ptr.p_double[i]-v)/t;
        ae_v_addd(&state->j.ptr.pp_double[i][0], 1, &state->deltax.ptr.p_double[0], 1, ae_v_len(0,n-1), v);
    }
    ae_v_move(&state->fi.ptr.p_double[0], 1, &state->fibase.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_add(&state->fi.ptr.p_double[0], 1, &state->deltaf.ptr.p_double[0], 1, ae_v_len(0,m-1));
    
    /*
     * Increase model age
     */
    state->modelage = state->modelage+1;
lbl_39:
    rmatrixgemm(n, n, m, 2.0, &state->j, 0, 0, 1, &state->j, 0, 0, 0, 0.0, &state->quadraticmodel, 0, 0, _state);
    rmatrixmv(n, m, &state->j, 0, 0, 1, &state->fi, 0, &state->gbase, 0, _state);
    ae_v_muld(&state->gbase.ptr.p_double[0], 1, ae_v_len(0,n-1), 2.0);
    v = ae_v_dotproduct(&state->fi.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    state->fbase = v;
    ae_v_move(&state->fibase.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    state->deltaxready = ae_false;
    state->deltafready = ae_false;
    
    /*
     * Perform integrity check (presense of NAN/INF)
     */
    v = state->fbase;
    for(i=0; i<=n-1; i++)
    {
        v = 0.1*v+state->gbase.ptr.p_double[i];
    }
    if( !ae_isfinite(v, _state) )
    {
        
        /*
         * Break!
         */
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    
    /*
     * If Lambda is not initialized, initialize it using quadratic model
     */
    if( ae_fp_less(state->lambdav,(double)(0)) )
    {
        state->lambdav = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            state->lambdav = ae_maxreal(state->lambdav, ae_fabs(state->quadraticmodel.ptr.pp_double[i][i], _state)*ae_sqr(state->s.ptr.p_double[i], _state), _state);
        }
        state->lambdav = 0.001*state->lambdav;
        if( ae_fp_eq(state->lambdav,(double)(0)) )
        {
            state->lambdav = (double)(1);
        }
    }
    
    /*
     * Find value of Levenberg-Marquardt damping parameter which:
     * * leads to positive definite damped model
     * * within bounds specified by StpMax
     * * generates step which decreases function value
     *
     * After this block IFlag is set to:
     * * -8, if internal integrity control detected NAN/INF in function values
     * * -3, if constraints are infeasible
     * * -2, if model update is needed (either Lambda growth is too large
     *       or step is too short, but we can't rely on model and stop iterations)
     * * -1, if model is fresh, Lambda have grown too large, termination is needed
     * *  0, if everything is OK, continue iterations
     * * >0, successful termination, step is less than EpsX
     *
     * State.Nu can have any value on enter, but after exit it is set to 1.0
     */
    iflag = -99;
    minlm_minlmstepfinderstart(&state->finderstate, &state->quadraticmodel, &state->gbase, state->fbase, &state->xbase, &state->fibase, state->modelage, _state);
lbl_49:
    if( !minlm_minlmstepfinderiteration(&state->finderstate, &state->lambdav, &state->nu, &state->xnew, &state->deltax, &state->deltaxready, &state->deltaf, &state->deltafready, &iflag, &fnew, &state->repncholesky, _state) )
    {
        goto lbl_50;
    }
    ae_assert(state->finderstate.needfi, "MinLM: internal error 2!", _state);
    state->repnfunc = state->repnfunc+1;
    if( state->protocolversion!=2 )
    {
        goto lbl_51;
    }
    
    /*
     * Issue request and perform report using V2 protocol
     */
    state->requesttype = 4;
    state->querysize = 1;
    state->queryfuncs = m;
    state->queryvars = n;
    state->querydim = 0;
    rcopyallocv(n, &state->finderstate.x, &state->querydata, _state);
    rallocv(m, &state->replyfi, _state);
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    rcopyv(m, &state->replyfi, &state->finderstate.fi, _state);
    goto lbl_52;
lbl_51:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 5138 failed", _state);
    minlm_clearrequestfields(state, _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->finderstate.x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->needfi = ae_true;
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->needfi = ae_false;
    ae_v_move(&state->finderstate.fi.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
lbl_52:
    goto lbl_49;
lbl_50:
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    state->nu = (double)(1);
    ae_assert(((iflag>=-3&&iflag<=0)||iflag==-8)||iflag>0, "MinLM: internal integrity check failed!", _state);
    if( iflag==-3 )
    {
        state->repterminationtype = -3;
        result = ae_false;
        return result;
    }
    if( iflag==-2 )
    {
        state->modelage = state->maxmodelage+1;
        goto lbl_36;
    }
    if( iflag!=-1 )
    {
        goto lbl_53;
    }
    
    /*
     * Stopping conditions are too stringent
     */
    state->repterminationtype = 7;
    if( !state->xrep )
    {
        goto lbl_55;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_57;
    }
    
    /*
     * Issue request using V2 protocol
     */
    state->requesttype = -1;
    state->queryvars = n;
    state->reportf = state->fbase;
    rcopyallocv(n, &state->xbase, &state->reportx, _state);
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    goto lbl_58;
lbl_57:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 9600 failed", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fbase;
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->xupdated = ae_false;
lbl_58:
lbl_55:
    result = ae_false;
    return result;
lbl_53:
    if( !(iflag==-8||iflag>0) )
    {
        goto lbl_59;
    }
    
    /*
     * Either:
     * * Integrity check failed - infinities or NANs
     * * successful termination (step size is small enough)
     */
    state->repterminationtype = iflag;
    if( !state->xrep )
    {
        goto lbl_61;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_63;
    }
    
    /*
     * Issue request using V2 protocol
     */
    state->requesttype = -1;
    state->queryvars = n;
    state->reportf = state->fbase;
    rcopyallocv(n, &state->xbase, &state->reportx, _state);
    state->rstate.stage = 16;
    goto lbl_rcomm;
lbl_16:
    goto lbl_64;
lbl_63:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 4042 failed", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fbase;
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 17;
    goto lbl_rcomm;
lbl_17:
    state->xupdated = ae_false;
lbl_64:
lbl_61:
    result = ae_false;
    return result;
lbl_59:
    state->f = fnew;
    
    /*
     * Levenberg-Marquardt step is ready.
     * Compare predicted vs. actual decrease and decide what to do with lambda.
     *
     * NOTE: we expect that State.DeltaX contains direction of step,
     * State.F contains function value at new point.
     */
    ae_assert(state->deltaxready, "MinLM: deltaX is not ready", _state);
    iflag = minlm_checkdecrease(&state->quadraticmodel, &state->gbase, state->fbase, n, &state->deltax, state->f, &state->lambdav, &state->nu, _state);
    if( iflag==0 )
    {
        goto lbl_65;
    }
    state->repterminationtype = iflag;
    if( !state->xrep )
    {
        goto lbl_67;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_69;
    }
    
    /*
     * Issue request using V2 protocol
     */
    state->requesttype = -1;
    state->queryvars = n;
    state->reportf = state->fbase;
    rcopyallocv(n, &state->xbase, &state->reportx, _state);
    state->rstate.stage = 18;
    goto lbl_rcomm;
lbl_18:
    goto lbl_70;
lbl_69:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 8643 failed", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->f = state->fbase;
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 19;
    goto lbl_rcomm;
lbl_19:
    state->xupdated = ae_false;
lbl_70:
lbl_67:
    result = ae_false;
    return result;
lbl_65:
    
    /*
     * Accept step, report it and
     * test stopping conditions on iterations count and function decrease.
     *
     * NOTE: we expect that State.DeltaX contains direction of step,
     * State.F contains function value at new point.
     *
     * NOTE2: we should update XBase ONLY. In the beginning of the next
     * iteration we expect that State.FIBase is NOT updated and
     * contains old value of a function vector.
     */
    ae_v_move(&state->xbase.ptr.p_double[0], 1, &state->xnew.ptr.p_double[0], 1, ae_v_len(0,n-1));
    if( !state->xrep )
    {
        goto lbl_71;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_73;
    }
    
    /*
     * Issue request using V2 protocol
     */
    state->requesttype = -1;
    state->queryvars = n;
    state->reportf = state->f;
    rcopyallocv(n, &state->xbase, &state->reportx, _state);
    state->rstate.stage = 20;
    goto lbl_rcomm;
lbl_20:
    goto lbl_74;
lbl_73:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 9600 failed", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 21;
    goto lbl_rcomm;
lbl_21:
    state->xupdated = ae_false;
lbl_74:
lbl_71:
    state->fstagnationcnt = icase2(ae_fp_less(ae_fabs(state->fbase-state->f, _state),minlm_stagnationfeps*rmaxabs3(state->fbase, state->f, (double)(1), _state)), state->fstagnationcnt+1, 0, _state);
    state->repiterationscount = state->repiterationscount+1;
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        state->repterminationtype = 5;
    }
    if( state->fstagnationcnt>=minlm_stagnationfits )
    {
        state->repterminationtype = 7;
    }
    if( state->repterminationtype<=0 )
    {
        goto lbl_75;
    }
    if( !state->xrep )
    {
        goto lbl_77;
    }
    
    /*
     * Report: XBase contains new point, F contains function value at new point
     */
    if( state->protocolversion!=2 )
    {
        goto lbl_79;
    }
    
    /*
     * Issue request using V2 protocol
     */
    state->requesttype = -1;
    state->queryvars = n;
    state->reportf = state->f;
    rcopyallocv(n, &state->xbase, &state->reportx, _state);
    state->rstate.stage = 22;
    goto lbl_rcomm;
lbl_22:
    goto lbl_80;
lbl_79:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 9600 failed", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 23;
    goto lbl_rcomm;
lbl_23:
    state->xupdated = ae_false;
lbl_80:
lbl_77:
    result = ae_false;
    return result;
lbl_75:
    state->modelage = state->modelage+1;
    goto lbl_36;
lbl_37:
    
    /*
     * Lambda is too large, we have to break iterations.
     */
    state->repterminationtype = 7;
    if( !state->xrep )
    {
        goto lbl_81;
    }
    if( state->protocolversion!=2 )
    {
        goto lbl_83;
    }
    
    /*
     * Issue request using V2 protocol
     */
    state->requesttype = -1;
    state->queryvars = n;
    state->reportf = state->f;
    rcopyallocv(n, &state->xbase, &state->reportx, _state);
    state->rstate.stage = 24;
    goto lbl_rcomm;
lbl_24:
    goto lbl_84;
lbl_83:
    
    /*
     * Use legacy V1 protocol
     */
    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol, integrity check 9600 failed", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    minlm_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 25;
    goto lbl_rcomm;
lbl_25:
    state->xupdated = ae_false;
lbl_84:
lbl_81:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = iflag;
    state->rstate.ia.ptr.p_int[3] = i;
    state->rstate.ia.ptr.p_int[4] = k;
    state->rstate.ba.ptr.p_bool[0] = bflag;
    state->rstate.ba.ptr.p_bool[1] = dotrace;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = xl;
    state->rstate.ra.ptr.p_double[2] = xr;
    state->rstate.ra.ptr.p_double[3] = s;
    state->rstate.ra.ptr.p_double[4] = t;
    state->rstate.ra.ptr.p_double[5] = fnew;
    return result;
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(teststep, _state), "MinLMOptGuardGradient: TestStep contains NaN or INF", _state);
    ae_assert(ae_fp_greater_eq(teststep,(double)(0)), "MinLMOptGuardGradient: invalid argument TestStep(TestStep<0)", _state);
    state->teststep = teststep;
}


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
     ae_state *_state)
{

    _optguardreport_clear(rep);

    smoothnessmonitorexportreport(&state->smonitor, rep, _state);
}


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
     ae_state *_state)
{

    ae_vector_clear(x);
    _minlmreport_clear(rep);

    minlmresultsbuf(state, x, rep, _state);
}


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
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->terminationtype = state->repterminationtype;
    rep->nfunc = state->repnfunc;
    rep->njac = state->repnjac;
    rep->ngrad = state->repngrad;
    rep->nhess = state->repnhess;
    rep->ncholesky = state->repncholesky;
}


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
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "MinLMRestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "MinLMRestartFrom: X contains infinite or NaN values!", _state);
    ae_v_move(&state->xbase.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_vector_set_length(&state->rstate.ia, 4+1, _state);
    ae_vector_set_length(&state->rstate.ba, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 5+1, _state);
    state->rstate.stage = -1;
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
void minlmrequesttermination(minlmstate* state, ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void minlmsetprotocolv1(minlmstate* state, ae_state *_state)
{


    state->protocolversion = 1;
}


/*************************************************************************
Set V2 reverse communication protocol
*************************************************************************/
void minlmsetprotocolv2(minlmstate* state, ae_state *_state)
{


    state->protocolversion = 2;
}


/*************************************************************************
Unpacks dense Jacobian into array
*************************************************************************/
void unpackdj(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_vector* replydj,
     /* Real    */ ae_matrix* jac,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t offs;


    if( replydj->cnt<m*n )
    {
        ae_assert(ae_false, "UnpackDJ: reply size is too small", _state);
    }
    if( jac->rows<m||jac->cols<n )
    {
        ae_assert(ae_false, "UnpackDJ: output size is too small", _state);
    }
    offs = 0;
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            jac->ptr.pp_double[i][j] = replydj->ptr.p_double[offs];
            offs = offs+1;
        }
    }
}


/*************************************************************************
Prepare internal structures (except for RComm).

Note: M must be zero for FGH mode, non-zero for V/VJ/FJ/FGJ mode.
*************************************************************************/
static void minlm_lmprepare(ae_int_t n,
     ae_int_t m,
     ae_bool havegrad,
     minlmstate* state,
     ae_state *_state)
{
    ae_int_t i;


    smoothnessmonitorinit(&state->smonitor, &state->s, 0, 0, ae_false, _state);
    if( n<=0||m<0 )
    {
        return;
    }
    if( havegrad )
    {
        ae_vector_set_length(&state->g, n, _state);
    }
    if( m!=0 )
    {
        ae_matrix_set_length(&state->j, m, n, _state);
        ae_vector_set_length(&state->fi, m, _state);
        ae_vector_set_length(&state->fibase, m, _state);
        ae_vector_set_length(&state->deltaf, m, _state);
        ae_vector_set_length(&state->fm1, m, _state);
        ae_vector_set_length(&state->fp1, m, _state);
        ae_vector_set_length(&state->fc1, m, _state);
        ae_vector_set_length(&state->gm1, m, _state);
        ae_vector_set_length(&state->gp1, m, _state);
        ae_vector_set_length(&state->gc1, m, _state);
    }
    else
    {
        ae_matrix_set_length(&state->h, n, n, _state);
    }
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->deltax, n, _state);
    ae_matrix_set_length(&state->quadraticmodel, n, n, _state);
    ae_vector_set_length(&state->xbase, n, _state);
    ae_vector_set_length(&state->gbase, n, _state);
    ae_vector_set_length(&state->xdir, n, _state);
    ae_vector_set_length(&state->tmp0, n, _state);
    
    /*
     * prepare internal L-BFGS
     */
    for(i=0; i<=n-1; i++)
    {
        state->x.ptr.p_double[i] = (double)(0);
    }
    minlbfgscreate(n, ae_minint(minlm_additers, n, _state), &state->x, &state->internalstate, _state);
    minlbfgssetcond(&state->internalstate, 0.0, 0.0, 0.0, ae_minint(minlm_additers, n, _state), _state);
    
    /*
     * Prepare internal QP solver
     */
    minqpcreate(n, &state->qpstate, _state);
    minqpsetalgoquickqp(&state->qpstate, 0.0, 0.0, coalesce(0.01*state->epsx, 1.0E-12, _state), 10, ae_true, _state);
    
    /*
     * Prepare boundary constraints
     */
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->havebndl, n, _state);
    ae_vector_set_length(&state->havebndu, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->havebndl.ptr.p_bool[i] = ae_false;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
        state->havebndu.ptr.p_bool[i] = ae_false;
    }
    
    /*
     * Prepare scaling matrix
     */
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->lastscaleused, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->s.ptr.p_double[i] = 1.0;
        state->lastscaleused.ptr.p_double[i] = 1.0;
    }
    
    /*
     * Prepare linear constraints
     */
    state->nec = 0;
    state->nic = 0;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void minlm_clearrequestfields(minlmstate* state, ae_state *_state)
{


    ae_assert(state->protocolversion==1, "MINLM: unexpected protocol", _state);
    state->needf = ae_false;
    state->needfg = ae_false;
    state->needfgh = ae_false;
    state->needfij = ae_false;
    state->needfi = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Increases lambda, returns False when there is a danger of overflow
*************************************************************************/
static ae_bool minlm_increaselambda(double* lambdav,
     double* nu,
     ae_state *_state)
{
    double lnlambda;
    double lnnu;
    double lnlambdaup;
    double lnmax;
    ae_bool result;


    result = ae_false;
    lnlambda = ae_log(*lambdav, _state);
    lnlambdaup = ae_log(minlm_lambdaup, _state);
    lnnu = ae_log(*nu, _state);
    lnmax = ae_log(ae_maxrealnumber, _state);
    if( ae_fp_greater(lnlambda+lnlambdaup+lnnu,0.25*lnmax) )
    {
        return result;
    }
    if( ae_fp_greater(lnnu+ae_log((double)(2), _state),lnmax) )
    {
        return result;
    }
    *lambdav = *lambdav*minlm_lambdaup*(*nu);
    *nu = *nu*(double)2;
    result = ae_true;
    return result;
}


/*************************************************************************
Decreases lambda, but leaves it unchanged when there is danger of underflow.
*************************************************************************/
static void minlm_decreaselambda(double* lambdav,
     double* nu,
     ae_state *_state)
{


    *nu = (double)(1);
    if( ae_fp_less(ae_log(*lambdav, _state)+ae_log(minlm_lambdadown, _state),ae_log(ae_minrealnumber, _state)) )
    {
        *lambdav = ae_minrealnumber;
    }
    else
    {
        *lambdav = *lambdav*minlm_lambdadown;
    }
}


/*************************************************************************
This function compares actual decrease vs predicted decrease  and  updates
LambdaV/Nu accordingly.

INPUT PARAMETERS:
    QuadraticModel      -   array[N,N], full Hessian matrix of quadratic
                            model at deltaX=0
    GBase               -   array[N], gradient at deltaX=0
    FBase               -   F(deltaX=0)
    N                   -   size
    DeltaX              -   step vector
    FNew                -   new function value
    LambdaV             -   lambda-value, updated on exit
    Nu                  -   Nu-multiplier, updated on exit

On exit it returns:
* Result=0  - if we have to continue iterations
* Result<>0 - if termination with completion code Result is requested

  -- ALGLIB --
     Copyright 17.02.2017 by Bochkanov Sergey
*************************************************************************/
static ae_int_t minlm_checkdecrease(/* Real    */ const ae_matrix* quadraticmodel,
     /* Real    */ const ae_vector* gbase,
     double fbase,
     ae_int_t n,
     /* Real    */ const ae_vector* deltax,
     double fnew,
     double* lambdav,
     double* nu,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double t;
    double predicteddecrease;
    double actualdecrease;
    ae_int_t result;


    result = 0;
    t = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = ae_v_dotproduct(&quadraticmodel->ptr.pp_double[i][0], 1, &deltax->ptr.p_double[0], 1, ae_v_len(0,n-1));
        t = t+deltax->ptr.p_double[i]*gbase->ptr.p_double[i]+0.5*deltax->ptr.p_double[i]*v;
    }
    predicteddecrease = -t;
    actualdecrease = -(fnew-fbase);
    if( ae_fp_less_eq(predicteddecrease,(double)(0)) )
    {
        result = 7;
        return result;
    }
    v = actualdecrease/predicteddecrease;
    if( ae_fp_less(v,0.1) )
    {
        if( !minlm_increaselambda(lambdav, nu, _state) )
        {
            
            /*
             * Lambda is too large, we have to break iterations.
             */
            result = 7;
            return result;
        }
    }
    if( ae_fp_greater(v,0.5) )
    {
        minlm_decreaselambda(lambdav, nu, _state);
    }
    return result;
}


/*************************************************************************
This  function  initializes  step finder object  with  problem  statement;
model  parameters  specified  during  this  call  should not (and can not)
change during object lifetime (although it is  possible  to  re-initialize
object with different settings).

This function reuses internally allocated objects as much as possible.

In addition to initializing step finder, this function enforces feasibility
in initial point X passed to this function. It is important that LM iteration
starts from feasible point and performs feasible steps;

RETURN VALUE:
    True for successful initialization
    False for inconsistent constraints; you should not use step finder if
    it returned False.
*************************************************************************/
static ae_bool minlm_minlmstepfinderinit(minlmstepfinder* state,
     ae_int_t n,
     ae_int_t m,
     ae_int_t maxmodelage,
     /* Real    */ ae_vector* xbase,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     /* Real    */ const ae_vector* s,
     double stpmax,
     double epsx,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool result;


    state->n = n;
    state->m = m;
    state->maxmodelage = maxmodelage;
    state->stpmax = stpmax;
    state->epsx = epsx;
    
    /*
     * Allocate temporaries, create QP solver, select QP algorithm
     */
    rvectorsetlengthatleast(&state->bndl, n, _state);
    rvectorsetlengthatleast(&state->bndu, n, _state);
    rvectorsetlengthatleast(&state->s, n, _state);
    bvectorsetlengthatleast(&state->havebndl, n, _state);
    bvectorsetlengthatleast(&state->havebndu, n, _state);
    rvectorsetlengthatleast(&state->x, n, _state);
    rvectorsetlengthatleast(&state->xbase, n, _state);
    rvectorsetlengthatleast(&state->tmp0, n, _state);
    rvectorsetlengthatleast(&state->modeldiag, n, _state);
    ivectorsetlengthatleast(&state->tmpct, nec+nic, _state);
    rvectorsetlengthatleast(&state->xdir, n, _state);
    rvectorsetlengthatleast(&state->fi, m, _state);
    rvectorsetlengthatleast(&state->fibase, m, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinLM: integrity check failed", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinLM: integrity check failed", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->havebndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->havebndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
        state->s.ptr.p_double[i] = s->ptr.p_double[i];
    }
    for(i=0; i<=nec-1; i++)
    {
        state->tmpct.ptr.p_int[i] = 0;
    }
    for(i=0; i<=nic-1; i++)
    {
        state->tmpct.ptr.p_int[nec+i] = -1;
    }
    minqpcreate(n, &state->qpstate, _state);
    if( nec+nic==0 )
    {
        minqpsetalgoquickqp(&state->qpstate, 0.0, 0.0, coalesce(0.01*epsx, 1.0E-12, _state), 10, ae_true, _state);
    }
    else
    {
        minqpsetalgodenseaul(&state->qpstate, coalesce(0.01*epsx, 1.0E-12, _state), (double)(100), 10, _state);
    }
    minqpsetbc(&state->qpstate, bndl, bndu, _state);
    minqpsetlc(&state->qpstate, cleic, &state->tmpct, nec+nic, _state);
    minqpsetscale(&state->qpstate, s, _state);
    
    /*
     * Check feasibility of constraints:
     * * check/enforce box constraints (straightforward)
     * * prepare QP subproblem which return us a feasible point
     */
    result = ae_true;
    for(i=0; i<=n-1; i++)
    {
        if( (state->havebndl.ptr.p_bool[i]&&state->havebndu.ptr.p_bool[i])&&ae_fp_greater(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            result = ae_false;
            return result;
        }
        if( state->havebndl.ptr.p_bool[i]&&ae_fp_less(xbase->ptr.p_double[i],state->bndl.ptr.p_double[i]) )
        {
            xbase->ptr.p_double[i] = state->bndl.ptr.p_double[i];
        }
        if( state->havebndu.ptr.p_bool[i]&&ae_fp_greater(xbase->ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            xbase->ptr.p_double[i] = state->bndu.ptr.p_double[i];
        }
    }
    if( nec+nic>0 )
    {
        
        /*
         * Well, we have linear constraints... let's use heavy machinery.
         *
         * We will modify QP solver state below, but everything will be
         * restored in MinLMStepFinderStart().
         */
        sparsecreate(n, n, n, &state->tmpsp, _state);
        for(i=0; i<=n-1; i++)
        {
            sparseset(&state->tmpsp, i, i, 0.5, _state);
            state->tmp0.ptr.p_double[i] = (double)(0);
        }
        minqpsetstartingpointfast(&state->qpstate, xbase, _state);
        minqpsetoriginfast(&state->qpstate, xbase, _state);
        minqpsetlineartermfast(&state->qpstate, &state->tmp0, _state);
        minqpsetquadratictermsparse(&state->qpstate, &state->tmpsp, ae_true, _state);
        minqpoptimize(&state->qpstate, _state);
        minqpresultsbuf(&state->qpstate, xbase, &state->qprep, _state);
    }
    return result;
}


/*************************************************************************
This function prepares LM step search session.
*************************************************************************/
static void minlm_minlmstepfinderstart(minlmstepfinder* state,
     /* Real    */ const ae_matrix* quadraticmodel,
     /* Real    */ const ae_vector* gbase,
     double fbase,
     /* Real    */ const ae_vector* xbase,
     /* Real    */ const ae_vector* fibase,
     ae_int_t modelage,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_vector_set_length(&state->rstate.ia, 2+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 0+1, _state);
    state->rstate.stage = -1;
    state->modelage = modelage;
    state->fbase = fbase;
    for(i=0; i<=state->m-1; i++)
    {
        state->fibase.ptr.p_double[i] = fibase->ptr.p_double[i];
    }
    for(i=0; i<=n-1; i++)
    {
        state->xbase.ptr.p_double[i] = xbase->ptr.p_double[i];
        state->modeldiag.ptr.p_double[i] = quadraticmodel->ptr.pp_double[i][i];
    }
    minqpsetstartingpointfast(&state->qpstate, xbase, _state);
    minqpsetoriginfast(&state->qpstate, xbase, _state);
    minqpsetlineartermfast(&state->qpstate, gbase, _state);
    minqpsetquadratictermfast(&state->qpstate, quadraticmodel, ae_true, 0.0, _state);
}


/*************************************************************************
This function runs LM step search session.
//
// Find value of Levenberg-Marquardt damping parameter which:
// * leads to positive definite damped model
// * within bounds specified by StpMax
// * generates step which decreases function value
//
// After this block IFlag is set to:
// * -8, if infinities/NANs were detected in function values/gradient
// * -3, if constraints are infeasible
// * -2, if model update is needed (either Lambda growth is too large
//       or step is too short, but we can't rely on model and stop iterations)
// * -1, if model is fresh, Lambda have grown too large, termination is needed
// *  0, if everything is OK, continue iterations
// * >0  - successful completion (step size is small enough)
//
// State.Nu can have any value on enter, but after exit it is set to 1.0
//
*************************************************************************/
static ae_bool minlm_minlmstepfinderiteration(minlmstepfinder* state,
     double* lambdav,
     double* nu,
     /* Real    */ ae_vector* xnew,
     /* Real    */ ae_vector* deltax,
     ae_bool* deltaxready,
     /* Real    */ ae_vector* deltaf,
     ae_bool* deltafready,
     ae_int_t* iflag,
     double* fnew,
     ae_int_t* ncholesky,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool bflag;
    double v;
    ae_int_t n;
    ae_int_t m;
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
        n = state->rstate.ia.ptr.p_int[1];
        m = state->rstate.ia.ptr.p_int[2];
        bflag = state->rstate.ba.ptr.p_bool[0];
        v = state->rstate.ra.ptr.p_double[0];
    }
    else
    {
        i = 763;
        n = -541;
        m = -698;
        bflag = ae_false;
        v = -318.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    
    /*
     * Routine body
     */
    *iflag = -99;
    n = state->n;
    m = state->m;
lbl_1:
    if( ae_false )
    {
        goto lbl_2;
    }
    *deltaxready = ae_false;
    *deltafready = ae_false;
    
    /*
     * Do we need model update?
     */
    if( state->modelage>0&&ae_fp_greater_eq(*nu,minlm_suspiciousnu) )
    {
        *iflag = -2;
        goto lbl_2;
    }
    
    /*
     * Setup quadratic solver and solve quadratic programming problem.
     * After problem is solved we'll try to bound step by StpMax
     * (Lambda will be increased if step size is too large).
     *
     * We use BFlag variable to indicate that we have to increase Lambda.
     * If it is False, we will try to increase Lambda and move to new iteration.
     */
    bflag = ae_true;
    for(i=0; i<=n-1; i++)
    {
        state->tmp0.ptr.p_double[i] = state->modeldiag.ptr.p_double[i]+*lambdav/ae_sqr(state->s.ptr.p_double[i], _state);
    }
    minqprewritediagonal(&state->qpstate, &state->tmp0, _state);
    minqpoptimize(&state->qpstate, _state);
    minqpresultsbuf(&state->qpstate, xnew, &state->qprep, _state);
    *ncholesky = *ncholesky+state->qprep.ncholesky;
    if( state->qprep.terminationtype==-3 )
    {
        
        /*
         * Infeasible constraints
         */
        *iflag = -3;
        goto lbl_2;
    }
    if( state->qprep.terminationtype==-4||state->qprep.terminationtype==-5 )
    {
        
        /*
         * Unconstrained direction of negative curvature was detected
         */
        if( !minlm_increaselambda(lambdav, nu, _state) )
        {
            *iflag = -1;
            goto lbl_2;
        }
        goto lbl_1;
    }
    ae_assert(state->qprep.terminationtype>0, "MinLM: unexpected completion code from QP solver", _state);
    ae_v_move(&state->xdir.ptr.p_double[0], 1, &xnew->ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_sub(&state->xdir.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(state->xdir.ptr.p_double[i]/state->s.ptr.p_double[i], _state);
    }
    if( ae_isfinite(v, _state) )
    {
        v = ae_sqrt(v, _state);
        if( ae_fp_greater(state->stpmax,(double)(0))&&ae_fp_greater(v,state->stpmax) )
        {
            bflag = ae_false;
        }
    }
    else
    {
        bflag = ae_false;
    }
    if( !bflag )
    {
        
        /*
         * Solution failed:
         * try to increase lambda to make matrix positive definite and continue.
         */
        if( !minlm_increaselambda(lambdav, nu, _state) )
        {
            *iflag = -1;
            goto lbl_2;
        }
        goto lbl_1;
    }
    
    /*
     * Step in State.XDir and it is bounded by StpMax.
     *
     * We should check stopping conditions on step size here.
     * DeltaX, which is used for secant updates, is initialized here.
     *
     * This code is a bit tricky because sometimes XDir<>0, but
     * it is so small that XDir+XBase==XBase (in finite precision
     * arithmetics). So we set DeltaX to XBase, then
     * add XDir, and then subtract XBase to get exact value of
     * DeltaX.
     *
     * Step length is estimated using DeltaX.
     *
     * NOTE: stopping conditions are tested
     * for fresh models only (ModelAge=0)
     */
    ae_v_move(&deltax->ptr.p_double[0], 1, &xnew->ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_sub(&deltax->ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    *deltaxready = ae_true;
    v = 0.0;
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(deltax->ptr.p_double[i]/state->s.ptr.p_double[i], _state);
    }
    v = ae_sqrt(v, _state);
    if( ae_fp_less_eq(v,state->epsx) )
    {
        if( state->modelage==0 )
        {
            
            /*
             * Step is too short, model is fresh and we can rely on it.
             * Terminating.
             */
            *iflag = 2;
            goto lbl_2;
        }
        else
        {
            
            /*
             * Step is suspiciously short, but model is not fresh
             * and we can't rely on it.
             */
            *iflag = -2;
            goto lbl_2;
        }
    }
    
    /*
     * Let's evaluate new step:
     * a) if we have Fi vector, we evaluate it using rcomm, and
     *    then we manually calculate State.F as sum of squares of Fi[]
     * b) if we have F value, we just evaluate it through rcomm interface
     *
     * We prefer (a) because we may need Fi vector for additional
     * iterations
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &xnew->ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->needf = ae_false;
    state->needfi = ae_false;
    state->needfi = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfi = ae_false;
    v = ae_v_dotproduct(&state->fi.ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    *fnew = v;
    ae_v_move(&deltaf->ptr.p_double[0], 1, &state->fi.ptr.p_double[0], 1, ae_v_len(0,m-1));
    ae_v_sub(&deltaf->ptr.p_double[0], 1, &state->fibase.ptr.p_double[0], 1, ae_v_len(0,m-1));
    *deltafready = ae_true;
    if( !ae_isfinite(*fnew, _state) )
    {
        
        /*
         * Integrity check failed, break!
         */
        *iflag = -8;
        goto lbl_2;
    }
    if( ae_fp_greater_eq(*fnew,state->fbase) )
    {
        
        /*
         * Increase lambda and continue
         */
        if( !minlm_increaselambda(lambdav, nu, _state) )
        {
            *iflag = -1;
            goto lbl_2;
        }
        goto lbl_1;
    }
    
    /*
     * We've found our step!
     */
    *iflag = 0;
    goto lbl_2;
    goto lbl_1;
lbl_2:
    *nu = (double)(1);
    ae_assert(((*iflag>=-3&&*iflag<=0)||*iflag==-8)||*iflag>0, "MinLM: internal integrity check failed!", _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = i;
    state->rstate.ia.ptr.p_int[1] = n;
    state->rstate.ia.ptr.p_int[2] = m;
    state->rstate.ba.ptr.p_bool[0] = bflag;
    state->rstate.ra.ptr.p_double[0] = v;
    return result;
}


void _minlmstepfinder_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlmstepfinder *p = (minlmstepfinder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->modeldiag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fibase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->havebndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->havebndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_vector_init(&p->xdir, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->choleskybuf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpct, 0, DT_INT, _state, make_automatic);
    _minqpstate_init(&p->qpstate, _state, make_automatic);
    _minqpreport_init(&p->qprep, _state, make_automatic);
    _sparsematrix_init(&p->tmpsp, _state, make_automatic);
}


void _minlmstepfinder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlmstepfinder       *dst = (minlmstepfinder*)_dst;
    const minlmstepfinder *src = (const minlmstepfinder*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->stpmax = src->stpmax;
    dst->modelage = src->modelage;
    dst->maxmodelage = src->maxmodelage;
    dst->epsx = src->epsx;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    dst->needf = src->needf;
    dst->needfi = src->needfi;
    dst->fbase = src->fbase;
    ae_vector_init_copy(&dst->modeldiag, &src->modeldiag, _state, make_automatic);
    ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic);
    ae_vector_init_copy(&dst->fibase, &src->fibase, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->havebndl, &src->havebndl, _state, make_automatic);
    ae_vector_init_copy(&dst->havebndu, &src->havebndu, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    ae_vector_init_copy(&dst->xdir, &src->xdir, _state, make_automatic);
    ae_vector_init_copy(&dst->choleskybuf, &src->choleskybuf, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpct, &src->tmpct, _state, make_automatic);
    dst->actualdecrease = src->actualdecrease;
    dst->predicteddecrease = src->predicteddecrease;
    _minqpstate_init_copy(&dst->qpstate, &src->qpstate, _state, make_automatic);
    _minqpreport_init_copy(&dst->qprep, &src->qprep, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpsp, &src->tmpsp, _state, make_automatic);
}


void _minlmstepfinder_clear(void* _p)
{
    minlmstepfinder *p = (minlmstepfinder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_vector_clear(&p->modeldiag);
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->fibase);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->havebndl);
    ae_vector_clear(&p->havebndu);
    ae_vector_clear(&p->s);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->xdir);
    ae_vector_clear(&p->choleskybuf);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmpct);
    _minqpstate_clear(&p->qpstate);
    _minqpreport_clear(&p->qprep);
    _sparsematrix_clear(&p->tmpsp);
}


void _minlmstepfinder_destroy(void* _p)
{
    minlmstepfinder *p = (minlmstepfinder*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_vector_destroy(&p->modeldiag);
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->fibase);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->havebndl);
    ae_vector_destroy(&p->havebndu);
    ae_vector_destroy(&p->s);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->xdir);
    ae_vector_destroy(&p->choleskybuf);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmpct);
    _minqpstate_destroy(&p->qpstate);
    _minqpreport_destroy(&p->qprep);
    _sparsematrix_destroy(&p->tmpsp);
}


void _minlmstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlmstate *p = (minlmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->h, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
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
    ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fibase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gbase, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->quadraticmodel, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->havebndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->havebndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->cleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xnew, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xdir, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->deltax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->deltaf, 0, DT_REAL, _state, make_automatic);
    _smoothnessmonitor_init(&p->smonitor, _state, make_automatic);
    ae_vector_init(&p->lastscaleused, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_vector_init(&p->choleskybuf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fm1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fc1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gm1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gc1, 0, DT_REAL, _state, make_automatic);
    _minlbfgsstate_init(&p->internalstate, _state, make_automatic);
    _minlbfgsreport_init(&p->internalrep, _state, make_automatic);
    _minqpstate_init(&p->qpstate, _state, make_automatic);
    _minqpreport_init(&p->qprep, _state, make_automatic);
    _minlmstepfinder_init(&p->finderstate, _state, make_automatic);
}


void _minlmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlmstate       *dst = (minlmstate*)_dst;
    const minlmstate *src = (const minlmstate*)_src;
    dst->protocolversion = src->protocolversion;
    dst->n = src->n;
    dst->m = src->m;
    dst->diffstep = src->diffstep;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->stpmax = src->stpmax;
    dst->maxmodelage = src->maxmodelage;
    dst->makeadditers = src->makeadditers;
    dst->userterminationneeded = src->userterminationneeded;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    ae_matrix_init_copy(&dst->h, &src->h, _state, make_automatic);
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    dst->needf = src->needf;
    dst->needfg = src->needfg;
    dst->needfgh = src->needfgh;
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
    dst->algomode = src->algomode;
    ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic);
    dst->fbase = src->fbase;
    ae_vector_init_copy(&dst->fibase, &src->fibase, _state, make_automatic);
    ae_vector_init_copy(&dst->gbase, &src->gbase, _state, make_automatic);
    ae_matrix_init_copy(&dst->quadraticmodel, &src->quadraticmodel, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->havebndl, &src->havebndl, _state, make_automatic);
    ae_vector_init_copy(&dst->havebndu, &src->havebndu, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_matrix_init_copy(&dst->cleic, &src->cleic, _state, make_automatic);
    dst->nec = src->nec;
    dst->nic = src->nic;
    dst->lambdav = src->lambdav;
    dst->nu = src->nu;
    dst->modelage = src->modelage;
    ae_vector_init_copy(&dst->xnew, &src->xnew, _state, make_automatic);
    ae_vector_init_copy(&dst->xdir, &src->xdir, _state, make_automatic);
    ae_vector_init_copy(&dst->deltax, &src->deltax, _state, make_automatic);
    ae_vector_init_copy(&dst->deltaf, &src->deltaf, _state, make_automatic);
    dst->deltaxready = src->deltaxready;
    dst->deltafready = src->deltafready;
    _smoothnessmonitor_init_copy(&dst->smonitor, &src->smonitor, _state, make_automatic);
    dst->teststep = src->teststep;
    ae_vector_init_copy(&dst->lastscaleused, &src->lastscaleused, _state, make_automatic);
    dst->fstagnationcnt = src->fstagnationcnt;
    dst->repiterationscount = src->repiterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repnfunc = src->repnfunc;
    dst->repnjac = src->repnjac;
    dst->repngrad = src->repngrad;
    dst->repnhess = src->repnhess;
    dst->repncholesky = src->repncholesky;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    ae_vector_init_copy(&dst->choleskybuf, &src->choleskybuf, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    dst->actualdecrease = src->actualdecrease;
    dst->predicteddecrease = src->predicteddecrease;
    dst->xm1 = src->xm1;
    dst->xp1 = src->xp1;
    ae_vector_init_copy(&dst->fm1, &src->fm1, _state, make_automatic);
    ae_vector_init_copy(&dst->fp1, &src->fp1, _state, make_automatic);
    ae_vector_init_copy(&dst->fc1, &src->fc1, _state, make_automatic);
    ae_vector_init_copy(&dst->gm1, &src->gm1, _state, make_automatic);
    ae_vector_init_copy(&dst->gp1, &src->gp1, _state, make_automatic);
    ae_vector_init_copy(&dst->gc1, &src->gc1, _state, make_automatic);
    _minlbfgsstate_init_copy(&dst->internalstate, &src->internalstate, _state, make_automatic);
    _minlbfgsreport_init_copy(&dst->internalrep, &src->internalrep, _state, make_automatic);
    _minqpstate_init_copy(&dst->qpstate, &src->qpstate, _state, make_automatic);
    _minqpreport_init_copy(&dst->qprep, &src->qprep, _state, make_automatic);
    _minlmstepfinder_init_copy(&dst->finderstate, &src->finderstate, _state, make_automatic);
}


void _minlmstate_clear(void* _p)
{
    minlmstate *p = (minlmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    ae_matrix_clear(&p->h);
    ae_vector_clear(&p->g);
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
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->fibase);
    ae_vector_clear(&p->gbase);
    ae_matrix_clear(&p->quadraticmodel);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->havebndl);
    ae_vector_clear(&p->havebndu);
    ae_vector_clear(&p->s);
    ae_matrix_clear(&p->cleic);
    ae_vector_clear(&p->xnew);
    ae_vector_clear(&p->xdir);
    ae_vector_clear(&p->deltax);
    ae_vector_clear(&p->deltaf);
    _smoothnessmonitor_clear(&p->smonitor);
    ae_vector_clear(&p->lastscaleused);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->choleskybuf);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->fm1);
    ae_vector_clear(&p->fp1);
    ae_vector_clear(&p->fc1);
    ae_vector_clear(&p->gm1);
    ae_vector_clear(&p->gp1);
    ae_vector_clear(&p->gc1);
    _minlbfgsstate_clear(&p->internalstate);
    _minlbfgsreport_clear(&p->internalrep);
    _minqpstate_clear(&p->qpstate);
    _minqpreport_clear(&p->qprep);
    _minlmstepfinder_clear(&p->finderstate);
}


void _minlmstate_destroy(void* _p)
{
    minlmstate *p = (minlmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    ae_matrix_destroy(&p->h);
    ae_vector_destroy(&p->g);
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
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->fibase);
    ae_vector_destroy(&p->gbase);
    ae_matrix_destroy(&p->quadraticmodel);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->havebndl);
    ae_vector_destroy(&p->havebndu);
    ae_vector_destroy(&p->s);
    ae_matrix_destroy(&p->cleic);
    ae_vector_destroy(&p->xnew);
    ae_vector_destroy(&p->xdir);
    ae_vector_destroy(&p->deltax);
    ae_vector_destroy(&p->deltaf);
    _smoothnessmonitor_destroy(&p->smonitor);
    ae_vector_destroy(&p->lastscaleused);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->choleskybuf);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->fm1);
    ae_vector_destroy(&p->fp1);
    ae_vector_destroy(&p->fc1);
    ae_vector_destroy(&p->gm1);
    ae_vector_destroy(&p->gp1);
    ae_vector_destroy(&p->gc1);
    _minlbfgsstate_destroy(&p->internalstate);
    _minlbfgsreport_destroy(&p->internalrep);
    _minqpstate_destroy(&p->qpstate);
    _minqpreport_destroy(&p->qprep);
    _minlmstepfinder_destroy(&p->finderstate);
}


void _minlmreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlmreport *p = (minlmreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minlmreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlmreport       *dst = (minlmreport*)_dst;
    const minlmreport *src = (const minlmreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->terminationtype = src->terminationtype;
    dst->nfunc = src->nfunc;
    dst->njac = src->njac;
    dst->ngrad = src->ngrad;
    dst->nhess = src->nhess;
    dst->ncholesky = src->ncholesky;
}


void _minlmreport_clear(void* _p)
{
    minlmreport *p = (minlmreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minlmreport_destroy(void* _p)
{
    minlmreport *p = (minlmreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
