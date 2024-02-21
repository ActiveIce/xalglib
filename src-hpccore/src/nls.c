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
#include "nls.h"


/*$ Declarations $*/
static void nls_unscalebatch(const nlsstate* state,
     /* Real    */ const ae_vector* xs,
     ae_int_t batchsize,
     /* Integer */ const ae_vector* idxraw2red,
     ae_int_t nreduced,
     /* Real    */ const ae_vector* scaledbndl,
     /* Real    */ const ae_vector* scaledbndu,
     /* Real    */ ae_vector* xu,
     ae_state *_state);


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
     ae_state *_state)
{

    _nlsstate_clear(state);

    ae_assert(n>=1, "NLSCreateDFO: N<1!", _state);
    ae_assert(m>=1, "NLSCreateDFO: M<1!", _state);
    ae_assert(x->cnt>=n, "NLSCreateDFO: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "NLSCreateDFO: X contains infinite or NaN values!", _state);
    
    /*
     * initialize
     */
    state->protocolversion = 2;
    state->n = n;
    state->m = m;
    state->problemtype = 0;
    state->diffstep = (double)(0);
    state->xrep = ae_false;
    state->rad0 = (double)(0);
    state->maxfev = 0;
    critinitdefault(&state->criteria, _state);
    bsetallocv(n, ae_false, &state->hasbndl, _state);
    bsetallocv(n, ae_false, &state->hasbndu, _state);
    rsetallocv(n, _state->v_neginf, &state->bndl, _state);
    rsetallocv(n, _state->v_posinf, &state->bndu, _state);
    state->nnlc = 0;
    state->cntlc = 0;
    rsetallocv(n, 1.0, &state->s, _state);
    nlssetalgo2ps(state, 0, _state);
    nlsrestartfrom(state, x, _state);
}


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
     ae_state *_state)
{


    ae_assert(state->problemtype==0, "NLSSetAlgo2PS: the solver MUST be created in a derivative-free mode (i.e. with nlscreatedfo() function)", _state);
    ae_assert(nnoisyrestarts>=0, "NLSSetAlgo2PS: negative NNoisyRestarts", _state);
    state->algorithm = 0;
    state->nnoisyrestarts = nnoisyrestarts;
}


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
     ae_state *_state)
{


    ae_assert(state->problemtype==0, "NLSSetAlgoDFOLSA: the solver MUST be created in a derivative-free mode (i.e. with nlscreatedfo() function)", _state);
    ae_assert(nnoisyrestarts>=0, "NLSSetAlgo2PS: negative NNoisyRestarts", _state);
    state->algorithm = 1;
    state->nnoisyrestarts = nnoisyrestarts;
}


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
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsx, _state), "NLSSetCond: EpsX is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "NLSSetCond: negative EpsX!", _state);
    ae_assert(maxits>=0, "NLSSetCond: negative MaxIts!", _state);
    if( ae_fp_eq(epsx,(double)(0))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    critsetcondv1(&state->criteria, 0.0, epsx, maxits, _state);
}


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
void nlssetxrep(nlsstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


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
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "NLSSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "NLSSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "NLSSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "NLSSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "NLSSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "NLSSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "NLSSetBC: BndU contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
}


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
ae_bool nlsiteration(nlsstate* state, ae_state *_state)
{
    ae_int_t nraw;
    ae_int_t nreduced;
    ae_int_t m;
    ae_int_t nnlc;
    ae_int_t i;
    ae_int_t originalrequest;
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
        nraw = state->rstate.ia.ptr.p_int[0];
        nreduced = state->rstate.ia.ptr.p_int[1];
        m = state->rstate.ia.ptr.p_int[2];
        nnlc = state->rstate.ia.ptr.p_int[3];
        i = state->rstate.ia.ptr.p_int[4];
        originalrequest = state->rstate.ia.ptr.p_int[5];
        b = state->rstate.ba.ptr.p_bool[0];
    }
    else
    {
        nraw = 359;
        nreduced = -58;
        m = -919;
        nnlc = -909;
        i = 81;
        originalrequest = 255;
        b = ae_false;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    
    /*
     * Routine body
     */
    
    /*
     * prepare
     */
    nraw = state->n;
    m = state->m;
    nnlc = state->nnlc;
    state->repiterationscount = 0;
    state->repterminationtype = 0;
    state->repnrequests = 0;
    state->repnfunc = 0;
    state->userterminationneeded = ae_false;
    
    /*
     * Trace status
     */
    state->dotrace = ae_false;
    b = ae_false;
    if( state->algorithm==0 )
    {
        state->dotrace = ae_is_trace_enabled("2PS")||ae_is_trace_enabled("DFGM");
        b = ae_true;
    }
    if( state->algorithm==1 )
    {
        state->dotrace = ae_is_trace_enabled("DFOLSA")||ae_is_trace_enabled("DFGM");
        b = ae_true;
    }
    ae_assert(b, "NLS: integrity check 8718 failed", _state);
    if( state->dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  NLS SOLVER STARTED                                                                            //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("N             = %6d (original variables count)\n",
            (int)(nraw));
    }
    
    /*
     * Check correctness of box constraints, remove fixed variables (most derivative-free solvers do not allow
     * fixed variables because it overcomplicates geometry improvement code).
     */
    iallocv(nraw, &state->idxraw2red, _state);
    rallocv(nraw, &state->redbl, _state);
    rallocv(nraw, &state->redbu, _state);
    rallocv(nraw, &state->reds, _state);
    rallocv(nraw, &state->redx0, _state);
    nreduced = 0;
    for(i=0; i<=nraw-1; i++)
    {
        if( ae_isfinite(state->bndl.ptr.p_double[i], _state)&&ae_isfinite(state->bndu.ptr.p_double[i], _state) )
        {
            if( ae_fp_greater(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                if( state->dotrace )
                {
                    ae_trace("> variable %0d has inconsistent box constraints, terminating with code -3\n",
                        (int)(i));
                }
                state->repterminationtype = -3;
                result = ae_false;
                return result;
            }
            if( ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->idxraw2red.ptr.p_int[i] = -1;
                continue;
            }
        }
        state->redx0.ptr.p_double[nreduced] = state->x0.ptr.p_double[i];
        state->reds.ptr.p_double[nreduced] = state->s.ptr.p_double[i];
        state->redbl.ptr.p_double[nreduced] = state->bndl.ptr.p_double[i];
        state->redbu.ptr.p_double[nreduced] = state->bndu.ptr.p_double[i];
        state->idxraw2red.ptr.p_int[i] = nreduced;
        nreduced = nreduced+1;
    }
    if( nreduced==0 )
    {
        
        /*
         * All variables are fixed, quick exit
         */
        if( state->dotrace )
        {
            ae_trace("> all variables are fixed by their box constraints, no need to run the solver. Success!\n");
        }
        state->repterminationtype = 2;
        state->repiterationscount = 0;
        rcopyv(nraw, &state->bndl, &state->xc, _state);
        result = ae_false;
        return result;
    }
    if( state->dotrace )
    {
        ae_trace("N_reduced     = %6d (variables after removing fixed ones)\n",
            (int)(nreduced));
        ae_trace("M             = %6d (nonlinear least squares target functions)\n",
            (int)(m));
        ae_trace("cntLC         = %6d (linear constraints)\n",
            (int)(state->cntlc));
        ae_trace("cntNLC        = %6d (nonlinear constraints)\n",
            (int)(state->nnlc));
    }
    
    /*
     * Allocate temporaries, as mandated by the V2 protocol
     */
    ae_assert(state->protocolversion==2, "NLS: integrity check 5124 failed", _state);
    b = ae_false;
    if( state->problemtype==0 )
    {
        rallocv(nraw, &state->reportx, _state);
        rallocv(m+nnlc, &state->tmpf1, _state);
        rallocv(nraw, &state->tmpg1, _state);
        rallocv(nraw, &state->tmpx1, _state);
        b = ae_true;
    }
    ae_assert(b, "NLS: integrity check 6628 failed", _state);
    
    /*
     * Initialization phase
     */
    b = ae_false;
    if( state->algorithm==0||state->algorithm==1 )
    {
        
        /*
         * Initialize 2PS solver
         */
        i = -999999;
        if( state->algorithm==0 )
        {
            i = 0;
        }
        if( state->algorithm==1 )
        {
            i = 1;
        }
        ae_assert(i>=0, "NLS: integrity check 7930 failed", _state);
        dfgminitbuf(&state->redbl, &state->redbu, &state->reds, &state->redx0, nreduced, m, ae_true, i, &state->criteria, state->nnoisyrestarts, state->rad0, state->maxfev, &state->dfgmsolver, _state);
        b = ae_true;
    }
    ae_assert(b, "NLS: integrity check 1620 failed", _state);
    
    /*
     * Iteration phase
     */
lbl_2:
    if( ae_false )
    {
        goto lbl_3;
    }
    
    /*
     * Run iteration function and load evaluation point
     */
    state->requesttype = 0;
    b = ae_false;
    if( state->algorithm==0||state->algorithm==1 )
    {
        
        /*
         * 2PS solver
         */
        if( !dfgmiteration(&state->dfgmsolver, state->userterminationneeded, _state) )
        {
            goto lbl_3;
        }
        if( state->dfgmsolver.requesttype==4 )
        {
            rallocv(nraw*state->dfgmsolver.querysize, &state->querydata, _state);
            state->querysize = state->dfgmsolver.querysize;
            nls_unscalebatch(state, &state->dfgmsolver.querydata, state->dfgmsolver.querysize, &state->idxraw2red, nreduced, &state->dfgmsolver.scaledbndl, &state->dfgmsolver.scaledbndu, &state->querydata, _state);
            originalrequest = 4;
            b = ae_true;
        }
        if( state->dfgmsolver.requesttype==-1 )
        {
            nls_unscalebatch(state, &state->dfgmsolver.reportx, 1, &state->idxraw2red, nreduced, &state->dfgmsolver.scaledbndl, &state->dfgmsolver.scaledbndu, &state->reportx, _state);
            state->reportf = state->dfgmsolver.reportf;
            state->requesttype = -1;
            originalrequest = -1;
            b = ae_true;
        }
    }
    ae_assert(b, "NLS: integrity check 5510 failed", _state);
    
    /*
     * Process evaluation request, perform numerical differentiation if necessary, offload results back to the optimizer
     */
    b = ae_false;
    if( originalrequest!=-1 )
    {
        goto lbl_4;
    }
    
    /*
     * Report
     */
    ae_assert(state->protocolversion==2, "NLS: integrity check 6811 failed", _state);
    if( !state->xrep )
    {
        goto lbl_6;
    }
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
lbl_6:
    b = ae_true;
lbl_4:
    if( originalrequest!=4 )
    {
        goto lbl_8;
    }
    state->repnfunc = state->repnfunc+state->querysize;
    state->repnrequests = state->repnrequests+1;
    state->requesttype = 4;
    state->queryfuncs = m+nnlc;
    state->queryvars = nraw;
    state->querydim = 0;
    rallocv((m+nnlc)*state->querysize, &state->replyfi, _state);
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    b = ae_true;
lbl_8:
    ae_assert(b, "NLS: integrity check 7814 failed", _state);
    
    /*
     * Send results back to the optimizer
     */
    ae_assert(originalrequest==-1||originalrequest==4, "NLS: integrity check 8314 failed", _state);
    b = ae_false;
    if( originalrequest==-1 )
    {
        
        /*
         * Do nothing
         */
        b = ae_true;
    }
    if( originalrequest==4&&(state->algorithm==0||state->algorithm==1) )
    {
        
        /*
         * 2PS solver
         */
        ae_assert(state->protocolversion==2, "NLS: integrity check 9915 failed", _state);
        rcopyv((m+nnlc)*state->querysize, &state->replyfi, &state->dfgmsolver.replyfi, _state);
        b = ae_true;
    }
    ae_assert(b, "MINNLC: integrity check 0038 failed", _state);
    goto lbl_2;
lbl_3:
    
    /*
     * Results phase
     */
    b = ae_false;
    if( state->algorithm==0||state->algorithm==1 )
    {
        
        /*
         * 2PS results
         */
        state->repterminationtype = state->dfgmsolver.repterminationtype;
        state->repiterationscount = state->dfgmsolver.repiterationscount;
        nls_unscalebatch(state, &state->dfgmsolver.xk, 1, &state->idxraw2red, nreduced, &state->dfgmsolver.scaledbndl, &state->dfgmsolver.scaledbndu, &state->xc, _state);
        b = ae_true;
    }
    ae_assert(b, "NLS: integrity check 1825 failed", _state);
    if( state->dotrace )
    {
        ae_trace("> analyzing callback parallelism potential:\n");
        ae_trace("best_speedup  = %0.1f (assuming expensive target function and negligible iteration overhead)\n",
            (double)((double)state->repnfunc/(double)state->repnrequests));
    }
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = nraw;
    state->rstate.ia.ptr.p_int[1] = nreduced;
    state->rstate.ia.ptr.p_int[2] = m;
    state->rstate.ia.ptr.p_int[3] = nnlc;
    state->rstate.ia.ptr.p_int[4] = i;
    state->rstate.ia.ptr.p_int[5] = originalrequest;
    state->rstate.ba.ptr.p_bool[0] = b;
    return result;
}


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
     ae_state *_state)
{

    ae_vector_clear(x);
    _nlsreport_clear(rep);

    nlsresultsbuf(state, x, rep, _state);
}


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
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    rcopyv(state->n, &state->xc, x, _state);
    rep->iterationscount = state->repiterationscount;
    rep->terminationtype = state->repterminationtype;
    rep->nfunc = state->repnfunc;
}


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
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "NLSRestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "NLSRestartFrom: X contains infinite or NaN values!", _state);
    rcopyallocv(state->n, x, &state->x0, _state);
    rcopyallocv(state->n, x, &state->xc, _state);
    ae_vector_set_length(&state->rstate.ia, 5+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
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
void nlsrequesttermination(nlsstate* state, ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


/*************************************************************************
Set V2 reverse communication protocol
*************************************************************************/
void nlssetprotocolv2(nlsstate* state, ae_state *_state)
{


    state->protocolversion = 2;
}


/*************************************************************************
Unscales X (converts from scaled variables to original ones), paying special
attention to box constraints (output is always feasible; active constraints
are mapped to active ones).

Can handle batch requests. Can handle problems with some variables being
fixed and removed from consideration.

Parameters:
    State           solver
    XS              array[NReduced*BatchSize] - variables after scaling and
                    removal of fixed vars
    idxRaw2Red      array[NRaw], maps original variable indexes to ones
                    after removal of fixed vars. Contains -1 for fixed vars.
    scaledBndL,
    ScaledBndU      array[NReduced], lower/upper bounds after removal of
                    the fixed vars
                    
output:
    XU              array[NRaw*BatchSize], vars after unscaling and
                    substitution of the fixed vars
*************************************************************************/
static void nls_unscalebatch(const nlsstate* state,
     /* Real    */ const ae_vector* xs,
     ae_int_t batchsize,
     /* Integer */ const ae_vector* idxraw2red,
     ae_int_t nreduced,
     /* Real    */ const ae_vector* scaledbndl,
     /* Real    */ const ae_vector* scaledbndu,
     /* Real    */ ae_vector* xu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t jr;
    ae_int_t nraw;
    double vs;


    nraw = state->n;
    if( nreduced==nraw )
    {
        
        /*
         * No fixed vars, simple code
         */
        for(k=0; k<=batchsize-1; k++)
        {
            for(j=0; j<=nraw-1; j++)
            {
                i = k*nraw+j;
                vs = xs->ptr.p_double[i];
                if( state->hasbndl.ptr.p_bool[j]&&vs<=scaledbndl->ptr.p_double[j] )
                {
                    xu->ptr.p_double[i] = state->bndl.ptr.p_double[j];
                    continue;
                }
                if( state->hasbndu.ptr.p_bool[j]&&vs>=scaledbndu->ptr.p_double[j] )
                {
                    xu->ptr.p_double[i] = state->bndu.ptr.p_double[j];
                    continue;
                }
                xu->ptr.p_double[i] = vs*state->s.ptr.p_double[j];
                if( state->hasbndl.ptr.p_bool[j]&&xu->ptr.p_double[i]<state->bndl.ptr.p_double[j] )
                {
                    xu->ptr.p_double[i] = state->bndl.ptr.p_double[j];
                }
                if( state->hasbndu.ptr.p_bool[j]&&xu->ptr.p_double[i]>state->bndu.ptr.p_double[j] )
                {
                    xu->ptr.p_double[i] = state->bndu.ptr.p_double[j];
                }
            }
        }
    }
    else
    {
        
        /*
         * Some vars are fixed
         */
        for(k=0; k<=batchsize-1; k++)
        {
            for(j=0; j<=nraw-1; j++)
            {
                i = k*nraw+j;
                jr = idxraw2red->ptr.p_int[j];
                if( jr<0 )
                {
                    xu->ptr.p_double[i] = state->bndl.ptr.p_double[j];
                    continue;
                }
                vs = xs->ptr.p_double[k*nreduced+jr];
                if( state->hasbndl.ptr.p_bool[j]&&vs<=scaledbndl->ptr.p_double[jr] )
                {
                    xu->ptr.p_double[i] = state->bndl.ptr.p_double[j];
                    continue;
                }
                if( state->hasbndu.ptr.p_bool[j]&&vs>=scaledbndu->ptr.p_double[jr] )
                {
                    xu->ptr.p_double[i] = state->bndu.ptr.p_double[j];
                    continue;
                }
                xu->ptr.p_double[i] = vs*state->s.ptr.p_double[j];
                if( state->hasbndl.ptr.p_bool[j]&&xu->ptr.p_double[i]<state->bndl.ptr.p_double[j] )
                {
                    xu->ptr.p_double[i] = state->bndl.ptr.p_double[j];
                }
                if( state->hasbndu.ptr.p_bool[j]&&xu->ptr.p_double[i]>state->bndu.ptr.p_double[j] )
                {
                    xu->ptr.p_double[i] = state->bndu.ptr.p_double[j];
                }
            }
        }
    }
}


void _nlsstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nlsstate *p = (nlsstate*)_p;
    ae_touch_ptr((void*)p);
    _nlpstoppingcriteria_init(&p->criteria, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->idxraw2red, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->redbl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->redbu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->reds, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->redx0, 0, DT_REAL, _state, make_automatic);
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
    _dfgmstate_init(&p->dfgmsolver, _state, make_automatic);
    ae_vector_init(&p->xc, 0, DT_REAL, _state, make_automatic);
}


void _nlsstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    nlsstate       *dst = (nlsstate*)_dst;
    const nlsstate *src = (const nlsstate*)_src;
    dst->protocolversion = src->protocolversion;
    dst->n = src->n;
    dst->m = src->m;
    dst->problemtype = src->problemtype;
    dst->algorithm = src->algorithm;
    dst->diffstep = src->diffstep;
    _nlpstoppingcriteria_init_copy(&dst->criteria, &src->criteria, _state, make_automatic);
    dst->xrep = src->xrep;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    dst->rad0 = src->rad0;
    dst->maxfev = src->maxfev;
    dst->nnoisyrestarts = src->nnoisyrestarts;
    dst->nnlc = src->nnlc;
    dst->cntlc = src->cntlc;
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->idxraw2red, &src->idxraw2red, _state, make_automatic);
    ae_vector_init_copy(&dst->redbl, &src->redbl, _state, make_automatic);
    ae_vector_init_copy(&dst->redbu, &src->redbu, _state, make_automatic);
    ae_vector_init_copy(&dst->reds, &src->reds, _state, make_automatic);
    ae_vector_init_copy(&dst->redx0, &src->redx0, _state, make_automatic);
    dst->userterminationneeded = src->userterminationneeded;
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
    _dfgmstate_init_copy(&dst->dfgmsolver, &src->dfgmsolver, _state, make_automatic);
    dst->dotrace = src->dotrace;
    ae_vector_init_copy(&dst->xc, &src->xc, _state, make_automatic);
    dst->repnfunc = src->repnfunc;
    dst->repiterationscount = src->repiterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repnrequests = src->repnrequests;
}


void _nlsstate_clear(void* _p)
{
    nlsstate *p = (nlsstate*)_p;
    ae_touch_ptr((void*)p);
    _nlpstoppingcriteria_clear(&p->criteria);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->idxraw2red);
    ae_vector_clear(&p->redbl);
    ae_vector_clear(&p->redbu);
    ae_vector_clear(&p->reds);
    ae_vector_clear(&p->redx0);
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
    _dfgmstate_clear(&p->dfgmsolver);
    ae_vector_clear(&p->xc);
}


void _nlsstate_destroy(void* _p)
{
    nlsstate *p = (nlsstate*)_p;
    ae_touch_ptr((void*)p);
    _nlpstoppingcriteria_destroy(&p->criteria);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->idxraw2red);
    ae_vector_destroy(&p->redbl);
    ae_vector_destroy(&p->redbu);
    ae_vector_destroy(&p->reds);
    ae_vector_destroy(&p->redx0);
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
    _dfgmstate_destroy(&p->dfgmsolver);
    ae_vector_destroy(&p->xc);
}


void _nlsreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nlsreport *p = (nlsreport*)_p;
    ae_touch_ptr((void*)p);
}


void _nlsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    nlsreport       *dst = (nlsreport*)_dst;
    const nlsreport *src = (const nlsreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->terminationtype = src->terminationtype;
    dst->nfunc = src->nfunc;
}


void _nlsreport_clear(void* _p)
{
    nlsreport *p = (nlsreport*)_p;
    ae_touch_ptr((void*)p);
}


void _nlsreport_destroy(void* _p)
{
    nlsreport *p = (nlsreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
