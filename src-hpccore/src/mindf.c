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
#include "mindf.h"


/*$ Declarations $*/
static double mindf_defaultmomentum = 0.9;
static void mindf_unscale(const mindfstate* state,
     /* Real    */ const ae_vector* xs,
     /* Real    */ ae_vector* xu,
     ae_state *_state);
static void mindf_unscalebatchfinitebnd(const mindfstate* state,
     /* Real    */ const ae_vector* xs,
     ae_int_t batchsize,
     ae_int_t n,
     /* Real    */ const ae_vector* finitebndl,
     /* Real    */ const ae_vector* finitebndu,
     /* Real    */ ae_vector* xu,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
                          GLOBAL OPTIMIZATION
               SUBJECT TO BOX/LINEAR/NONLINEAR CONSTRAINTS

The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints
* nonlinear generalized inequality constraints Li<=Ci(x)<=Ui, with one  of
  Li/Ui possibly being infinite

REQUIREMENTS:
* F() and C()  do  NOT  have  to be differentiable, locally Lipschitz   or
  continuous. Most solvers in this subpackage can deal with  nonsmoothness
  or minor discontinuities, although obviously smoother problems  are  the
  most easy ones.
* generally, F() and C() must be computable at any point which is feasible
  subject to box constraints

USAGE:

1. User  initializes  algorithm  state   with   mindfcreate()   call   and
   chooses specific solver to be used. There is some solver which is  used
   by default, with default settings, but  you  should  NOT  rely  on  the
   default choice. It may change in the future releases of ALGLIB  without
   notice, and no one can guarantee that the new solver will be  able   to
   solve your problem with default settings.

2. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) mindfsetbc() for boundary constraints
   b) mindfsetlc2dense() for linear constraints
   c) mindfsetnlc2() for nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.

3. User sets scale of the variables with mindfsetscale() function.  It  is
   VERY important to set  variable  scales  because  many  derivative-free
   algorithms refuse to work when variables are badly scaled.

4. Finally, user calls mindfoptimize()  function which takes algorithm
   state and pointer (delegate, etc) to callback function which calculates
   F and G.

5. User calls mindfresults() to get a solution


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from  size  of  X
    X       -   starting point, array[N]. Some solvers can utilize  a good
                initial point to seed computations.
                
                As of ALGLIB 4.01, the initial point is:
                * not used by GDEMO
                
                If the chosen solver does not need initial point, one  can
                supply zeros.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state


IMPORTANT: the  MINDF  optimizer   supports   parallel   model  evaluation
           ('callback  parallelism'). This feature, which  is  present  in
           commercial ALGLIB editions, greatly accelerates algorithms like
           differential evolution which usually issue  batch  requests  to
           user callbacks which can be efficiently parallelized.
           
           Callback parallelism  is  usually  beneficial  when  the  batch
           evalution requires more than several milliseconds.
           
           See ALGLIB Reference Manual, 'Working with commercial  version'
           section,  and  comments  on  mindfoptimize()  function for more
           information.
           
  -- ALGLIB --
     Copyright 24.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfcreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     mindfstate* state,
     ae_state *_state)
{

    _mindfstate_clear(state);

    ae_assert(n>=1, "MinDFCreate: N<1", _state);
    ae_assert(x->cnt>=n, "MinDFCreate: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinDFCreate: X contains infinite or NaN values", _state);
    
    /*
     * Default settings and state
     */
    state->protocolversion = 2;
    state->n = n;
    rsetallocv(n, _state->v_neginf, &state->bndl, _state);
    rsetallocv(n, _state->v_posinf, &state->bndu, _state);
    bsetallocv(n, ae_false, &state->hasbndl, _state);
    bsetallocv(n, ae_false, &state->hasbndu, _state);
    state->m = 0;
    state->nnlc = 0;
    rsetallocv(n, 1.0, &state->s, _state);
    state->hasx0 = ae_true;
    rcopyallocv(n, x, &state->x0, _state);
    rcopyallocv(n, x, &state->xf, _state);
    state->xrep = ae_false;
    state->ssgdmomentum = mindf_defaultmomentum;
    state->rngseed = 11732;
    state->gdemofixedparams = ae_false;
    state->gdemorho1 = (double)(0);
    state->gdemorho2 = (double)(0);
    state->stoponsmallf = ae_false;
    mindfsetalgogdemo(state, 100, 0, _state);
    
    /*
     * communication with user
     */
    state->userterminationneeded = ae_false;
    ae_vector_set_length(&state->rstate.ia, 2+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
This function sets box constraints.

Box constraints are inactive by default (after initial creation).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

  -- ALGLIB --
     Copyright 24.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetbc(mindfstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "MinDFSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinDFSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinDFSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinDFSetBC: BndL contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinDFcreate() call.
    A       -   linear constraints, array[K,N]. Each row of  A  represents
                one  constraint. One-sided  inequality   constraints, two-
                sided inequality  constraints,  equality  constraints  are
                supported (see below)
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number of equality/inequality constraints,  K>=0;  if  not
                given, inferred from sizes of A, AL, AU.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetlc2dense(mindfstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;


    n = state->n;
    
    /*
     * Check input arguments
     */
    ae_assert(k>=0, "MinDFSetLC2Dense: K<0", _state);
    ae_assert(k==0||a->cols>=n, "MinDFSetLC2Dense: Cols(A)<N", _state);
    ae_assert(k==0||a->rows>=k, "MinDFSetLC2Dense: Rows(A)<K", _state);
    ae_assert(apservisfinitematrix(a, k, n, _state), "MinDFSetLC2Dense: A contains infinite or NaN values!", _state);
    ae_assert(al->cnt>=k, "MinDFSetLC2Dense: Length(AL)<K", _state);
    ae_assert(au->cnt>=k, "MinDFSetLC2Dense: Length(AU)<K", _state);
    for(i=0; i<=k-1; i++)
    {
        ae_assert(ae_isfinite(al->ptr.p_double[i], _state)||ae_isneginf(al->ptr.p_double[i], _state), "MinDFSetLC2Dense: AL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(au->ptr.p_double[i], _state)||ae_isposinf(au->ptr.p_double[i], _state), "MinDFSetLC2Dense: AU contains NAN or -INF", _state);
    }
    
    /*
     * Quick exit if needed
     */
    if( k==0 )
    {
        state->m = 0;
        return;
    }
    
    /*
     * Copy
     */
    rcopyallocv(k, al, &state->al, _state);
    rcopyallocv(k, au, &state->au, _state);
    rcopyallocm(k, n, a, &state->densea, _state);
    state->m = k;
}


/*************************************************************************
This function sets two-sided nonlinear constraints.

In fact, this function sets only  NUMBER  of  the  nonlinear  constraints.
Constraints  themselves  (constraint  functions)   are   passed   to   the
MinDFOptimize() method.

This method accepts user-defined vector function F[] where:
* first component of F[] corresponds to the objective
* subsequent NNLC components of F[] correspond to the two-sided  nonlinear
  constraints NL<=C(x)<=NU, where
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
    State   -   structure previously allocated with MinDFCreate call.
    NL      -   array[NNLC], lower bounds, can contain -INF
    NU      -   array[NNLC], lower bounds, can contain +INF
    NNLC    -   constraints count, NNLC>=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        feasible area!
        
NOTE 2: the algorithm scales variables according to the scale specified by
        MinDFSetScale() function, so it can handle problems with badly
        scaled variables (as long as we KNOW their scales).
           
        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints. Inappropriate scaling  of nonlinear  constraints  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT the same as solving it with constraint "0.001*G0(x)=0".
           
        It means that YOU are  the  one who is responsible for the correct
        scaling of the nonlinear constraints. We recommend  you  to  scale
        nonlinear constraints in such  a  way  that  the  derivatives  (if
        constraints are differentiable) have approximately unit  magnitude
        (for problems  with  unit  variable  scales)  or  have  magnitudes
        approximately equal to 1/S[i] (where S is a variable scale set  by
        MinDFSetScale() function).

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetnlc2(mindfstate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(nnlc>=0, "MinDFSetNLC2: NNLC<0", _state);
    ae_assert(nl->cnt>=nnlc, "MinDFSetNLC2: Length(NL)<NNLC", _state);
    ae_assert(nu->cnt>=nnlc, "MinDFSetNLC2: Length(NU)<NNLC", _state);
    state->nnlc = nnlc;
    rallocv(nnlc, &state->nl, _state);
    rallocv(nnlc, &state->nu, _state);
    for(i=0; i<=nnlc-1; i++)
    {
        ae_assert(ae_isfinite(nl->ptr.p_double[i], _state)||ae_isneginf(nl->ptr.p_double[i], _state), "MinDFSetNLC2: NL[i] is +INF or NAN", _state);
        ae_assert(ae_isfinite(nu->ptr.p_double[i], _state)||ae_isposinf(nu->ptr.p_double[i], _state), "MinDFSetNLC2: NU[i] is -INF or NAN", _state);
        state->nl.ptr.p_double[i] = nl->ptr.p_double[i];
        state->nu.ptr.p_double[i] = nu->ptr.p_double[i];
    }
}


/*************************************************************************
This function sets variable scales.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison  with  tolerances)  and  to
guide algorithm steps.

The scale of a variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetscale(mindfstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinDFSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinDFSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "MinDFSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This subroutine submits request for termination of a running optimizer. It
should be called from a user-supplied callback when user decides  that  it
is time to "smoothly" terminate optimization process.  As  a  result,  the
optimizer  stops  at  the  point  which  was  "current  accepted" when the
termination request was submitted and returns  error  code  8  (successful
termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on an optimizer which is NOT running will have
      no effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfrequesttermination(mindfstate* state, ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


/*************************************************************************
This subroutine sets optimization algorithm to the differential  evolution
solver GDEMO (Generalized Differential Evolution Multiobjective)  with  an
automatic parameters selection.

A version with manually tuned parameters can be activated by  calling  the
mindfsetalgogdemofixed() function.

ALGORITHM PROPERTIES:

* the solver uses an  adaptive  parameter  tuning  strategy  'Ensemble  of
  Parameters and mutation Strategies' (EPSDE). You do not have to  specify
  crossover  probability  and  differential  weight,   the   solver   will
  automatically choose the most appropriate strategy.
* the solver can handle box, linear,  nonlinear  constraints.  Linear  and
  nonlinear constraints are  handled  by  means  of an L1/L2  penalty. The
  solver does not violate box constraints at any point,  but  may  violate
  linear and nonlinear ones. Penalty coefficient can be changed with   the
  mindfsetgdemopenalty() function.
* the solver heavily depends on variable scales being available (specified
  by means of mindfsetscale() call) and on box constraints with both lower
  and upper bounds being available which are used to determine the  search
  region. It will work without box constraints  and  without  scales,  but
  results are likely to be suboptimal.
* the solver is parallelized (in commercial ALGLIB editions), with  nearly
  linear scalability of parallel processing
* this solver is intended for finding solutions  with up to several digits
  of precision at best. Its primary purpose  is  to  find  at  least  some
  solution to an otherwise intractable problem.

IMPORTANT: derivative-free optimization is inherently less robust than the
           smooth nonlinear programming, especially when nonsmoothness and
           discontinuities are present.
           
           Derivative-free  algorithms  have  less  convergence guarantees
           than their smooth  counterparts.  It  is  considered  a  normal
           (although, obviously, undesirable) situation when a derivative-
           -free algorithm fails to converge with desired precision. Having
           2 digits of accurasy is already a  good  result,  on  difficult
           problems (high numerical noise, discontinuities) you  may  have
           even less than that.

INPUT PARAMETERS:
    State       -   solver
    EpochsCnt   -   iterations count, >0.  Usually   the  algorithm  needs
                    hundreds of iterations to converge.
    PopSize     -   population  size,  >=0.  Zero  value  means  that  the
                    default value (which is 10*N in the  current  version)
                    will be chosen.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetalgogdemo(mindfstate* state,
     ae_int_t epochscnt,
     ae_int_t popsize,
     ae_state *_state)
{


    ae_assert(popsize>=0, "MinDFSetAlgoGDEMO: PopSize<0", _state);
    ae_assert(epochscnt>0, "MinDFSetAlgoGDEMO: EpochsCnt<=0", _state);
    state->gdemoepochscnt = epochscnt;
    state->gdemopopsize = popsize;
    state->gdemofixedparams = ae_false;
    state->solvertype = 0;
}


/*************************************************************************
This subroutine tells GDEMO differential  evolution  optimizer  to  handle
linear/nonlinear constraints by an L1/L2 penalty function.

IMPORTANT: this function does NOT change the  optimization  algorithm.  If
           want to activate differential evolution solver, you still  have
           to call a proper mindfsetalgo???() function.

INPUT PARAMETERS:
    State       -   solver
    Rho1, Rho2  -   penalty parameters for constraint violations:
                    * Rho1 is a multiplier for L1 penalty
                    * Rho2 is a multiplier for L2 penalty
                    * Rho1,Rho2>=0
                    * having both of them at zero means that some  default
                      value will be chosen.
                    Ignored for problems with box-only constraints.
                    
                    L1 penalty is usually better at enforcing constraints,
                    but leads to slower convergence than L2 penalty. It is
                    possible to combine both kinds of penalties together.
                    
                    There is a compromise between constraint  satisfaction
                    and  optimality:  high  values  of   Rho   mean   that
                    constraints are satisfied with high accuracy  but that
                    the target may  be  underoptimized  due  to  numerical
                    difficulties.  Small  values  of  Rho  mean  that  the
                    solution may  grossly  violate  constraints.  Choosing
                    good Rho is usually a matter of trial and error.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetgdemopenalty(mindfstate* state,
     double rho1,
     double rho2,
     ae_state *_state)
{


    ae_assert(ae_isfinite(rho1, _state), "MinDFSetGDEMOPenalty: Rho1 is not a finite number", _state);
    ae_assert(ae_isfinite(rho2, _state), "MinDFSetGDEMOPenalty: Rho2 is not a finite number", _state);
    ae_assert(ae_fp_greater_eq(rho1,(double)(0)), "MinDFSetGDEMOPenalty: Rho1<0", _state);
    ae_assert(ae_fp_greater_eq(rho2,(double)(0)), "MinDFSetGDEMOPenalty: Rho2<0", _state);
    state->gdemorho1 = rho1;
    state->gdemorho2 = rho2;
}


/*************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetsmallf(mindfstate* state, double f, ae_state *_state)
{


    ae_assert(ae_isfinite(f, _state), "MinDFSetSmallF: F is not a finite number", _state);
    state->stoponsmallf = ae_true;
    state->smallf = f;
}


/*************************************************************************
This subroutine sets optimization algorithm to the differential  evolution
solver GDEMO (Generalized Differential Evolution Multiobjective)  with the
manual parameters selection.

Unlike DE with an automatic parameters selection  this  function  requires
user to manually specify  algorithm  parameters. In the general  case  the
full-auto GDEMO is better. However, it has to spend some time finding  out
properties of a problem being solved. Manually tuning the  solver  to  the
specific problem at hand may get 2x-3x better running time.

ALGORITHM PROPERTIES:

* the solver can handle box, linear,  nonlinear  constraints.  Linear  and
  nonlinear constraints are   handled  by  means  of  an L1  penalty.  The
  solver does not violate box constraints at any point,  but  may  violate
  linear and nonlinear ones. Penalty coefficient can be changed with   the
  mindfsetgdemopenalty() function.
* the solver heavily depends on variable scales being available (specified
  by means of mindfsetscale() call) and on box constraints with both lower
  and upper bounds being available which are used to determine the  search
  region. It will work without box constraints  and  without  scales,  but
  results are likely to be suboptimal.
* the solver is parallelized (in commercial ALGLIB editions), with  nearly
  linear scalability of parallel processing
* this solver is intended for finding solutions  with up to several digits
  of precision at best. Its primary purpose  is  to  find  at  least  some
  solution to an otherwise intractable problem.

IMPORTANT: derivative-free optimization is inherently less robust than the
           smooth nonlinear programming, especially when nonsmoothness and
           discontinuities are present.
           
           Derivative-free  algorithms  have  less  convergence guarantees
           than their smooth  counterparts.  It  is  considered  a  normal
           (although, obviously, undesirable) situation when a derivative-
           -free algorithm fails to converge with desired precision. Having
           2 digits of accurasy is already a  good  result,  on  difficult
           problems (high numerical noise, discontinuities) you  may  have
           even less than that.

INPUT PARAMETERS:
    State       -   solver
    EpochsCnt   -   iterations count, >0.  Usually   the  algorithm  needs
                    hundreds of iterations to converge.
    Strategy    -   specific DE strategy to use:
                    * 0 for DE/rand/1
                    * 1 for DE/best/2
                    * 2 for DE/rand-to-best/1
    CrossoverProb-  crossover probability, 0<CrossoverProb<1
    DifferentialWeight- weight, 0<DifferentialWeight<2
    PopSize     -   population  size,  >=0.  Zero  value  means  that  the
                    default value (which is 10*N in the  current  version)
                    will be chosen.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetalgogdemofixed(mindfstate* state,
     ae_int_t epochscnt,
     ae_int_t strategy,
     double crossoverprob,
     double differentialweight,
     ae_int_t popsize,
     ae_state *_state)
{


    ae_assert(strategy>=0&&strategy<=2, "MinDFSetAlgoGDEMOFixed: incorrect Strategy", _state);
    ae_assert((ae_isfinite(crossoverprob, _state)&&ae_fp_greater(crossoverprob,(double)(0)))&&ae_fp_less(crossoverprob,(double)(1)), "MinDFSetAlgoGDEMOFixed: CrossoverProb is infinite number or outside of (0,1)", _state);
    ae_assert((ae_isfinite(differentialweight, _state)&&ae_fp_greater(differentialweight,(double)(0)))&&ae_fp_less(differentialweight,(double)(2)), "MinDFSetAlgoGDEMOFixed: DifferentialWeight is infinite number or outside of (0,2)", _state);
    ae_assert(popsize>=0, "MinDFSetAlgoGDEMOFixed: PopSize<0", _state);
    ae_assert(epochscnt>0, "MinDFSetAlgoGDEMOFixed: EpochsCnt<=0", _state);
    state->gdemoepochscnt = epochscnt;
    state->gdemopopsize = popsize;
    state->gdemofixedparams = ae_true;
    state->gdemostrategy = strategy;
    state->gdemocrossoverprob = crossoverprob;
    state->gdemodifferentialweight = differentialweight;
    state->solvertype = 0;
}


/*************************************************************************

CALLBACK PARALLELISM

The  MINDF  optimizer  supports   parallel   model  evaluation  ('callback
parallelism').  This  feature,  which  is  present  in  commercial  ALGLIB
editions, greatly accelerates  optimization  when  using  a  solver  which
issues batch requests, i.e. multiple requests  for  target  values,  which
can be computed independently by different threads.

Callback parallelism is  usually   beneficial  when   processing  a  batch
request requires more than several  milliseconds.  It  also  requires  the
solver which issues requests in convenient batches, e.g. the  differential
evolution solver.

See ALGLIB Reference Manual, 'Working with commercial version' section for
more information.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool mindfiteration(mindfstate* state, ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
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
        n = state->rstate.ia.ptr.p_int[1];
        originalrequest = state->rstate.ia.ptr.p_int[2];
        b = state->rstate.ba.ptr.p_bool[0];
    }
    else
    {
        i = 359;
        n = -58;
        originalrequest = -919;
        b = ae_true;
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
    
    /*
     * Routine body
     */
    n = state->n;
    
    /*
     * Init
     */
    state->replcerr = 0.0;
    state->repnlcerr = 0.0;
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repnfunc = 0;
    state->repnrequests = 0;
    state->userterminationneeded = ae_false;
    
    /*
     * Allocate temporaries, as mandated by the V2 protocol
     */
    ae_assert(state->protocolversion==2, "MINDF: integrity check 5124 failed", _state);
    rallocv(n, &state->reportx, _state);
    rallocv(n, &state->tmpx1, _state);
    rallocv(1+state->nnlc, &state->tmpf1, _state);
    rallocv(n, &state->tmpg1, _state);
    
    /*
     * Various algorithms supported by the optimizer
     */
    if( state->solvertype!=0 )
    {
        goto lbl_4;
    }
    
    /*
     * Initialize the solver
     */
    b = ae_false;
    if( state->solvertype==0 )
    {
        gdemoinitbuf(&state->bndl, &state->bndu, &state->s, n, 1, &state->densea, &state->al, &state->au, state->m, &state->nl, &state->nu, state->nnlc, state->gdemopopsize, state->gdemoepochscnt, state->rngseed, &state->gdemosolver, _state);
        if( state->hasx0 )
        {
            gdemosetx0(&state->gdemosolver, &state->x0, _state);
        }
        if( state->gdemofixedparams )
        {
            gdemosetfixedparams(&state->gdemosolver, state->gdemostrategy, state->gdemocrossoverprob, state->gdemodifferentialweight, _state);
        }
        if( state->stoponsmallf )
        {
            gdemosetsmallf(&state->gdemosolver, state->smallf, _state);
        }
        b = ae_fp_neq(state->gdemorho1,(double)(0))||ae_fp_neq(state->gdemorho2,(double)(0));
        gdemosetmodepenalty(&state->gdemosolver, rcase2(b, state->gdemorho1, 50.0, _state), rcase2(b, state->gdemorho2, 50.0, _state), _state);
        b = ae_true;
    }
    ae_assert(b, "MINDF: integrity check 9536 failed", _state);
    
    /*
     * Perform iterations
     */
lbl_6:
    if( ae_false )
    {
        goto lbl_7;
    }
    
    /*
     * Run the iteration function and load request
     */
    state->requesttype = 0;
    originalrequest = 0;
    b = ae_false;
    if( state->solvertype==0 )
    {
        if( !gdemoiteration(&state->gdemosolver, state->userterminationneeded, _state) )
        {
            goto lbl_7;
        }
        if( state->gdemosolver.requesttype==4 )
        {
            rallocv(n*state->gdemosolver.querysize, &state->querydata, _state);
            state->querysize = state->gdemosolver.querysize;
            mindf_unscalebatchfinitebnd(state, &state->gdemosolver.querydata, state->gdemosolver.querysize, n, &state->gdemosolver.finitebndl, &state->gdemosolver.finitebndu, &state->querydata, _state);
            originalrequest = 4;
            b = ae_true;
        }
        if( state->gdemosolver.requesttype==-1 )
        {
            mindf_unscalebatchfinitebnd(state, &state->gdemosolver.reportx, 1, n, &state->gdemosolver.finitebndl, &state->gdemosolver.finitebndu, &state->reportx, _state);
            state->reportf = state->gdemosolver.reportf;
            originalrequest = -1;
            b = ae_true;
        }
    }
    ae_assert(b, "DFGM: integrity check 3141 failed", _state);
    
    /*
     * Forward request to user
     */
    b = ae_false;
    if( originalrequest!=-1 )
    {
        goto lbl_8;
    }
    
    /*
     * Report
     */
    ae_assert(state->protocolversion==2, "MINDF: integrity check 4642 failed", _state);
    state->requesttype = -1;
    if( !state->xrep )
    {
        goto lbl_10;
    }
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
lbl_10:
    b = ae_true;
lbl_8:
    if( originalrequest!=4 )
    {
        goto lbl_12;
    }
    state->repnfunc = state->repnfunc+state->querysize;
    state->repnrequests = state->repnrequests+1;
    state->requesttype = 4;
    state->queryfuncs = 1+state->nnlc;
    state->queryvars = n;
    state->querydim = 0;
    rallocv((1+state->nnlc)*state->querysize, &state->replyfi, _state);
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    b = ae_true;
lbl_12:
    ae_assert(b, "MINDF: integrity check 8545 failed", _state);
    
    /*
     * Send results back to the optimizer
     */
    ae_assert(originalrequest==-1||originalrequest==4, "MINDF: integrity check 9046 failed", _state);
    b = ae_false;
    if( originalrequest==-1 )
    {
        
        /*
         * Do nothing
         */
        b = ae_true;
    }
    if( originalrequest==4&&state->solvertype==0 )
    {
        
        /*
         * 2PS solver
         */
        ae_assert(state->protocolversion==2, "MINDF: integrity check 0446 failed", _state);
        rcopyv((1+state->nnlc)*state->querysize, &state->replyfi, &state->gdemosolver.replyfi, _state);
        b = ae_true;
    }
    ae_assert(b, "MINDF: integrity check 1247 failed", _state);
    goto lbl_6;
lbl_7:
    
    /*
     * Results
     */
    b = ae_false;
    if( state->solvertype==0 )
    {
        state->repterminationtype = state->gdemosolver.repterminationtype;
        state->repiterationscount = state->gdemosolver.repiterationscount;
        state->repbcerr = (double)(0);
        state->repbcidx = -1;
        state->replcerr = state->gdemosolver.replcerr;
        state->replcidx = state->gdemosolver.replcidx;
        state->repnlcerr = state->gdemosolver.repnlcerr;
        state->repnlcidx = state->gdemosolver.repnlcidx;
        rallocv(n, &state->tmp0, _state);
        rcopyrv(n, &state->gdemosolver.xbest, 0, &state->tmp0, _state);
        mindf_unscalebatchfinitebnd(state, &state->tmp0, 1, n, &state->gdemosolver.finitebndl, &state->gdemosolver.finitebndu, &state->xf, _state);
    }
    result = ae_false;
    return result;
lbl_4:
    
    /*
     * SSGD solver
     */
    if( state->solvertype!=-1 )
    {
        goto lbl_14;
    }
    ssgdinitbuf(&state->bndl, &state->bndu, &state->s, &state->x0, n, &state->densea, &state->al, &state->au, state->m, &state->nl, &state->nu, state->nnlc, state->ssgdblur0, state->ssgdblur1, state->ssgdouterits, state->ssgdrate0, state->ssgdrate1, state->ssgdmomentum, state->ssgdmaxits, state->ssgdrho, &state->ssgdsolver, _state);
lbl_16:
    if( !ssgditeration(&state->ssgdsolver, state->userterminationneeded, _state) )
    {
        goto lbl_17;
    }
    if( !state->ssgdsolver.needfi )
    {
        goto lbl_18;
    }
    state->requesttype = 4;
    state->querysize = 1;
    state->queryfuncs = 1+state->nnlc;
    state->queryvars = n;
    state->querydim = 0;
    rallocv(state->querysize*state->queryfuncs, &state->replyfi, _state);
    rallocv(n, &state->querydata, _state);
    mindf_unscale(state, &state->ssgdsolver.x, &state->querydata, _state);
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    rcopyv(1+state->nnlc, &state->replyfi, &state->ssgdsolver.fi, _state);
    goto lbl_16;
lbl_18:
    if( !state->ssgdsolver.xupdated )
    {
        goto lbl_20;
    }
    
    /*
     * Report current point
     */
    if( !state->xrep )
    {
        goto lbl_22;
    }
    state->requesttype = -1;
    mindf_unscale(state, &state->ssgdsolver.x, &state->reportx, _state);
    state->reportf = state->ssgdsolver.f;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
lbl_22:
    goto lbl_16;
lbl_20:
    ae_assert(ae_false, "MinDF: SSGD solver issued unexpected request", _state);
    goto lbl_16;
lbl_17:
    state->repterminationtype = 1;
    state->repiterationscount = state->ssgdsolver.repiterationscount;
    state->repbcerr = (double)(0);
    state->repbcidx = 0;
    state->replcerr = (double)(0);
    state->replcidx = 0;
    state->repnlcerr = (double)(0);
    state->repnlcidx = 0;
    mindf_unscale(state, &state->ssgdsolver.xcur, &state->xf, _state);
    result = ae_false;
    return result;
lbl_14:
    ae_assert(ae_false, "MinDF: unexpected solver type", _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = i;
    state->rstate.ia.ptr.p_int[1] = n;
    state->rstate.ia.ptr.p_int[2] = originalrequest;
    state->rstate.ba.ptr.p_bool[0] = b;
    return result;
}


/*************************************************************************
MinDF results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -8   internal integrity control  detected  infinite  or
                       NAN   values   in   function/gradient.    Abnormal
                       termination signalled.
                * -3   box constraints are inconsistent
                * -1   inconsistent parameters were passed:
                       * penalty parameter is zero, but we have nonlinear
                         constraints set by MinDFsetnlc2()
                *  1   successful termination according to a solver-specific
                       set of conditions
                *  8   User requested termination via minnsrequesttermination()

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfresults(const mindfstate* state,
     /* Real    */ ae_vector* x,
     mindfreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _mindfreport_clear(rep);

    mindfresultsbuf(state, x, rep, _state);
}


/*************************************************************************

Buffered implementation of MinDFresults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfresultsbuf(const mindfstate* state,
     /* Real    */ ae_vector* x,
     mindfreport* rep,
     ae_state *_state)
{


    rep->iterationscount = state->repiterationscount;
    rep->nfev = state->repnfunc;
    rep->terminationtype = state->repterminationtype;
    rep->bcerr = state->repbcerr;
    rep->lcerr = state->replcerr;
    rep->nlcerr = state->repnlcerr;
    if( state->repterminationtype>0 )
    {
        rcopyallocv(state->n, &state->xf, x, _state);
    }
    else
    {
        rsetallocv(state->n, _state->v_nan, x, _state);
    }
}


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void mindfsetprotocolv2(mindfstate* state, ae_state *_state)
{


    state->protocolversion = 2;
}


/*************************************************************************
Unscales X (converts from scaled variables to original ones)
*************************************************************************/
static void mindf_unscale(const mindfstate* state,
     /* Real    */ const ae_vector* xs,
     /* Real    */ ae_vector* xu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    for(i=0; i<=n-1; i++)
    {
        xu->ptr.p_double[i] = xs->ptr.p_double[i]*state->s.ptr.p_double[i];
    }
}


/*************************************************************************
Unscales X (converts from scaled variables to original ones), paying special
attention to box constraints (output is always feasible; active constraints
are mapped to active ones).

Can handle batch requests.

Parameters:
    State           solver
    XS              array[N*BatchSize] - variables after scaling
    FiniteBndL,
    FiniteBndU      array[N], lower/upper bounds with infinities being
                    replaced by big numbers
                    
output:
    XU              array[N*BatchSize], vars after unscaling
*************************************************************************/
static void mindf_unscalebatchfinitebnd(const mindfstate* state,
     /* Real    */ const ae_vector* xs,
     ae_int_t batchsize,
     ae_int_t n,
     /* Real    */ const ae_vector* finitebndl,
     /* Real    */ const ae_vector* finitebndu,
     /* Real    */ ae_vector* xu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double vs;


    for(k=0; k<=batchsize-1; k++)
    {
        for(j=0; j<=n-1; j++)
        {
            i = k*n+j;
            vs = xs->ptr.p_double[i];
            if( state->hasbndl.ptr.p_bool[j]&&vs<=finitebndl->ptr.p_double[j] )
            {
                xu->ptr.p_double[i] = state->bndl.ptr.p_double[j];
                continue;
            }
            if( state->hasbndu.ptr.p_bool[j]&&vs>=finitebndu->ptr.p_double[j] )
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


void _mindfstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    mindfstate *p = (mindfstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->densea, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->al, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->au, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xf, 0, DT_REAL, _state, make_automatic);
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
    _gdemostate_init(&p->gdemosolver, _state, make_automatic);
    _ssgdstate_init(&p->ssgdsolver, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
}


void _mindfstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    mindfstate       *dst = (mindfstate*)_dst;
    const mindfstate *src = (const mindfstate*)_src;
    dst->solvertype = src->solvertype;
    dst->n = src->n;
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_matrix_init_copy(&dst->densea, &src->densea, _state, make_automatic);
    ae_vector_init_copy(&dst->al, &src->al, _state, make_automatic);
    ae_vector_init_copy(&dst->au, &src->au, _state, make_automatic);
    dst->m = src->m;
    ae_vector_init_copy(&dst->nl, &src->nl, _state, make_automatic);
    ae_vector_init_copy(&dst->nu, &src->nu, _state, make_automatic);
    dst->nnlc = src->nnlc;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    dst->hasx0 = src->hasx0;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    dst->stoponsmallf = src->stoponsmallf;
    dst->smallf = src->smallf;
    dst->rngseed = src->rngseed;
    dst->xrep = src->xrep;
    ae_vector_init_copy(&dst->xf, &src->xf, _state, make_automatic);
    dst->protocolversion = src->protocolversion;
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
    dst->userterminationneeded = src->userterminationneeded;
    dst->gdemoepochscnt = src->gdemoepochscnt;
    dst->gdemopopsize = src->gdemopopsize;
    dst->gdemorho1 = src->gdemorho1;
    dst->gdemorho2 = src->gdemorho2;
    _gdemostate_init_copy(&dst->gdemosolver, &src->gdemosolver, _state, make_automatic);
    dst->gdemofixedparams = src->gdemofixedparams;
    dst->gdemostrategy = src->gdemostrategy;
    dst->gdemocrossoverprob = src->gdemocrossoverprob;
    dst->gdemodifferentialweight = src->gdemodifferentialweight;
    dst->ssgdmaxits = src->ssgdmaxits;
    dst->ssgdrate0 = src->ssgdrate0;
    dst->ssgdrate1 = src->ssgdrate1;
    dst->ssgdblur0 = src->ssgdblur0;
    dst->ssgdblur1 = src->ssgdblur1;
    dst->ssgdouterits = src->ssgdouterits;
    dst->ssgdmomentum = src->ssgdmomentum;
    dst->ssgdrho = src->ssgdrho;
    _ssgdstate_init_copy(&dst->ssgdsolver, &src->ssgdsolver, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    dst->repterminationtype = src->repterminationtype;
    dst->repiterationscount = src->repiterationscount;
    dst->repnfunc = src->repnfunc;
    dst->repnrequests = src->repnrequests;
    dst->repbcerr = src->repbcerr;
    dst->repbcidx = src->repbcidx;
    dst->replcerr = src->replcerr;
    dst->replcidx = src->replcidx;
    dst->repnlcerr = src->repnlcerr;
    dst->repnlcidx = src->repnlcidx;
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
}


void _mindfstate_clear(void* _p)
{
    mindfstate *p = (mindfstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_matrix_clear(&p->densea);
    ae_vector_clear(&p->al);
    ae_vector_clear(&p->au);
    ae_vector_clear(&p->nl);
    ae_vector_clear(&p->nu);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->xf);
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
    _gdemostate_clear(&p->gdemosolver);
    _ssgdstate_clear(&p->ssgdsolver);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->tmp0);
}


void _mindfstate_destroy(void* _p)
{
    mindfstate *p = (mindfstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_matrix_destroy(&p->densea);
    ae_vector_destroy(&p->al);
    ae_vector_destroy(&p->au);
    ae_vector_destroy(&p->nl);
    ae_vector_destroy(&p->nu);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->xf);
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
    _gdemostate_destroy(&p->gdemosolver);
    _ssgdstate_destroy(&p->ssgdsolver);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->tmp0);
}


void _mindfreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    mindfreport *p = (mindfreport*)_p;
    ae_touch_ptr((void*)p);
}


void _mindfreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    mindfreport       *dst = (mindfreport*)_dst;
    const mindfreport *src = (const mindfreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfev = src->nfev;
    dst->bcerr = src->bcerr;
    dst->lcerr = src->lcerr;
    dst->nlcerr = src->nlcerr;
    dst->terminationtype = src->terminationtype;
}


void _mindfreport_clear(void* _p)
{
    mindfreport *p = (mindfreport*)_p;
    ae_touch_ptr((void*)p);
}


void _mindfreport_destroy(void* _p)
{
    mindfreport *p = (mindfreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
