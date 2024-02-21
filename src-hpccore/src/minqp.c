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
#include "minqp.h"


/*$ Declarations $*/


/*$ Body $*/


/*************************************************************************
                    CONSTRAINED QUADRATIC PROGRAMMING

The subroutine creates QP optimizer. After initial creation,  it  contains
default optimization problem with zero quadratic and linear terms  and  no
constraints.

In order to actually solve something you should:
* set cost vector with minqpsetlinearterm()
* set variable bounds with minqpsetbc() or minqpsetbcall()
* specify constraint matrix with one of the following functions:
  * modern API:
    * minqpsetlc2()       for sparse two-sided constraints AL <= A*x <= AU
    * minqpsetlc2dense()  for dense  two-sided constraints AL <= A*x <= AU
    * minqpsetlc2mixed()  for mixed  two-sided constraints AL <= A*x <= AU
    * minqpaddlc2dense()  to add one dense row to the dense constraint submatrix
    * minqpaddlc2()       to add one sparse row to the sparse constraint submatrix
    * minqpaddlc2sparsefromdense() to add one sparse row (passed as a dense array) to the sparse constraint submatrix
  * legacy API:
    * minqpsetlc()        for dense one-sided equality/inequality constraints
    * minqpsetlcsparse()  for sparse one-sided equality/inequality constraints
    * minqpsetlcmixed()   for mixed dense/sparse one-sided equality/inequality constraints
* choose appropriate QP solver and set it  and  its stopping  criteria  by
  means of minqpsetalgo??????() function
* call minqpoptimize() to run the solver and  minqpresults()  to  get  the
  solution vector and additional information.

Following solvers are recommended for convex and semidefinite problems:
* QuickQP for dense problems with box-only constraints (or no constraints
  at all)
* DENSE-IPM-QP for  convex  or  semidefinite  problems  with   medium  (up
  to several thousands) variable count, dense/sparse  quadratic  term  and
  any number  (up  to  many  thousands)  of  dense/sparse  general  linear
  constraints
* SPARSE-IPM-QP for convex  or  semidefinite  problems  with   large (many
  thousands) variable count, sparse quadratic term AND linear constraints.

If your problem happens to be nonconvex,  but  either  (a) is  effectively
convexified under constraints,  or  (b)  has  unique  solution  even  with
nonconvex target, then you can use:
* QuickQP for dense nonconvex problems with box-only constraints
* DENSE-AUL-QP  for   dense   nonconvex   problems  which  are effectively
  convexified under constraints with up to several thousands of  variables
  and any (small or large) number of general linear constraints
* QP-BLEIC for dense/sparse problems with small (up to  several  hundreds)
  number of general linear  constraints  and  arbitrarily  large  variable
  count.

INPUT PARAMETERS:
    N       -   problem size
    
OUTPUT PARAMETERS:
    State   -   optimizer with zero quadratic/linear terms
                and no constraints

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpcreate(ae_int_t n, minqpstate* state, ae_state *_state)
{
    ae_int_t i;

    _minqpstate_clear(state);

    ae_assert(n>=1, "MinQPCreate: N<1", _state);
    
    /*
     * initialize QP solver
     */
    state->n = n;
    state->mdense = 0;
    state->msparse = 0;
    state->repterminationtype = 0;
    state->absamax = (double)(1);
    state->absasum = (double)(1);
    state->absasum2 = (double)(1);
    state->akind = 0;
    state->sparseaupper = ae_false;
    cqminit(n, &state->a, _state);
    ae_vector_set_length(&state->b, n, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->havebndl, n, _state);
    ae_vector_set_length(&state->havebndu, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->startx, n, _state);
    ae_vector_set_length(&state->xorigin, n, _state);
    ae_vector_set_length(&state->xs, n, _state);
    rvectorsetlengthatleast(&state->replagbc, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
        state->havebndl.ptr.p_bool[i] = ae_false;
        state->havebndu.ptr.p_bool[i] = ae_false;
        state->b.ptr.p_double[i] = 0.0;
        state->startx.ptr.p_double[i] = 0.0;
        state->xorigin.ptr.p_double[i] = 0.0;
        state->s.ptr.p_double[i] = 1.0;
        state->replagbc.ptr.p_double[i] = 0.0;
    }
    state->stype = 0;
    state->havex = ae_false;
    minqpsetalgobleic(state, 0.0, 0.0, 0.0, 0, _state);
    qqploaddefaults(n, &state->qqpsettingsuser, _state);
    qpbleicloaddefaults(n, &state->qpbleicsettingsuser, _state);
    qpdenseaulloaddefaults(n, &state->qpdenseaulsettingsuser, _state);
    state->qpbleicfirstcall = ae_true;
    state->dbgskipconstraintnormalization = ae_false;
    state->veps = 0.0;
}


/*************************************************************************
This function sets linear term for QP solver.

By default, linear term is zero.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    B       -   linear term, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetlinearterm(minqpstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(b->cnt>=n, "MinQPSetLinearTerm: Length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "MinQPSetLinearTerm: B contains infinite or NaN elements", _state);
    minqpsetlineartermfast(state, b, _state);
}


/*************************************************************************
This  function  sets  dense  quadratic  term  for  QP solver. By  default,
quadratic term is zero.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn't used

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadraticterm(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(a->rows>=n, "MinQPSetQuadraticTerm: Rows(A)<N", _state);
    ae_assert(a->cols>=n, "MinQPSetQuadraticTerm: Cols(A)<N", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "MinQPSetQuadraticTerm: A contains infinite or NaN elements", _state);
    minqpsetquadratictermfast(state, a, isupper, 0.0, _state);
}


/*************************************************************************
This  function  sets  sparse  quadratic  term  for  QP solver. By default,
quadratic  term  is  zero.  This  function  overrides  previous  calls  to
minqpsetquadraticterm() or minqpsetquadratictermsparse().

NOTE: dense solvers like DENSE-AUL-QP or DENSE-IPM-QP  will  convert  this
      matrix to dense storage anyway.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   (optional) storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn't used
                * if not given, both lower and upper  triangles  must  be
                  filled.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadratictermsparse(minqpstate* state,
     const sparsematrix* a,
     ae_bool isupper,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t t0;
    ae_int_t t1;
    ae_int_t i;
    ae_int_t j;
    double v;


    n = state->n;
    ae_assert(sparsegetnrows(a, _state)==n, "MinQPSetQuadraticTermSparse: Rows(A)<>N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "MinQPSetQuadraticTermSparse: Cols(A)<>N", _state);
    sparsecopytocrsbuf(a, &state->sparsea, _state);
    state->sparseaupper = isupper;
    state->akind = 1;
    
    /*
     * Estimate norm of A
     * (it will be used later in the quadratic penalty function)
     */
    state->absamax = (double)(0);
    state->absasum = (double)(0);
    state->absasum2 = (double)(0);
    t0 = 0;
    t1 = 0;
    while(sparseenumerate(a, &t0, &t1, &i, &j, &v, _state))
    {
        if( i==j )
        {
            
            /*
             * Diagonal terms are counted only once
             */
            state->absamax = ae_maxreal(state->absamax, v, _state);
            state->absasum = state->absasum+v;
            state->absasum2 = state->absasum2+v*v;
        }
        if( (j>i&&isupper)||(j<i&&!isupper) )
        {
            
            /*
             * Offdiagonal terms are counted twice
             */
            state->absamax = ae_maxreal(state->absamax, v, _state);
            state->absasum = state->absasum+(double)2*v;
            state->absasum2 = state->absasum2+(double)2*v*v;
        }
    }
}


/*************************************************************************
This function sets starting point for QP solver. It is useful to have good
initial approximation to the solution, because it will increase  speed  of
convergence and identification of active constraints.

NOTE: interior point solvers ignore initial point provided by user.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    X       -   starting point, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetstartingpoint(minqpstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(x->cnt>=n, "MinQPSetStartingPoint: Length(B)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinQPSetStartingPoint: X contains infinite or NaN elements", _state);
    minqpsetstartingpointfast(state, x, _state);
}


/*************************************************************************
This  function sets origin for QP solver. By default, following QP program
is solved:

    min(0.5*x'*A*x+b'*x)
    
This function allows to solve different problem:

    min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))
    
Specification of non-zero origin affects function being minimized, but not
constraints. Box and  linear  constraints  are  still  calculated  without
origin.
    
INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    XOrigin -   origin, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetorigin(minqpstate* state,
     /* Real    */ const ae_vector* xorigin,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(xorigin->cnt>=n, "MinQPSetOrigin: Length(B)<N", _state);
    ae_assert(isfinitevector(xorigin, n, _state), "MinQPSetOrigin: B contains infinite or NaN elements", _state);
    minqpsetoriginfast(state, xorigin, _state);
}


/*************************************************************************
This function sets scaling coefficients.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison  with  tolerances)  and  as
preconditioner.

Scale of the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the
   function

If you do not know how to choose scales of your variables, you can:
* read www.alglib.net/optimization/scaling.php article
* use minqpsetscaleautodiag(), which calculates scale  using  diagonal  of
  the  quadratic  term:  S  is  set to 1/sqrt(diag(A)), which works well
  sometimes.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetscale(minqpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinQPSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinQPSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "MinQPSetScale: S contains zero elements", _state);
    }
    for(i=0; i<=state->n-1; i++)
    {
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
    state->stype = 0;
}


/*************************************************************************
This function sets automatic evaluation of variable scaling.

IMPORTANT: this function works only for  matrices  with positive  diagonal
           elements! Zero or negative elements will  result  in  -9  error
           code  being  returned.  Specify  scale  vector  manually   with
           minqpsetscale() in such cases.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison  with  tolerances)  and  as
preconditioner.

The  best  way  to  set  scaling  is  to manually specify variable scales.
However, sometimes you just need quick-and-dirty solution  -  either  when
you perform fast prototyping, or when you know your problem well  and  you
are 100% sure that this quick solution is robust enough in your case.

One such solution is to evaluate scale of I-th variable as 1/Sqrt(A[i,i]),
where A[i,i] is an I-th diagonal element of the quadratic term.

Such approach works well sometimes, but you have to be careful here.

INPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void minqpsetscaleautodiag(minqpstate* state, ae_state *_state)
{


    state->stype = 1;
}


/*************************************************************************
This function tells solver to use BLEIC-based algorithm and sets  stopping
criteria for the algorithm.

This algorithm is intended for large-scale  problems,  possibly nonconvex,
with small number of general linear constraints. Feasible initial point is
essential for good performance.

IMPORTANT: when DENSE-IPM (or DENSE-AUL for  nonconvex  problems)  solvers
           are applicable, their performance is much better than  that  of
           BLEIC-QP.
           We recommend  you to use BLEIC only when other solvers can  not
           be used.

ALGORITHM FEATURES:

* supports dense and sparse QP problems
* supports box and general linear equality/inequality constraints
* can solve all types of problems  (convex,  semidefinite,  nonconvex)  as
  long as they are bounded from below under constraints.
  Say, it is possible to solve "min{-x^2} subject to -1<=x<=+1".
  Of course, global  minimum  is found only  for  positive  definite   and
  semidefinite  problems.  As  for indefinite ones - only local minimum is
  found.

ALGORITHM OUTLINE:

* BLEIC-QP solver is just a driver function for MinBLEIC solver; it solves
  quadratic  programming   problem   as   general   linearly   constrained
  optimization problem, which is solved by means of BLEIC solver  (part of
  ALGLIB, active set method).
  
ALGORITHM LIMITATIONS:
* This algorithm is inefficient on  problems with hundreds  and  thousands
  of general inequality constraints and infeasible initial point.  Initial
  feasibility detection stage may take too long on such constraint sets.
  Consider using DENSE-IPM or DENSE-AUL instead.
* unlike QuickQP solver, this algorithm does not perform Newton steps  and
  does not use Level 3 BLAS. Being general-purpose active set  method,  it
  can activate constraints only one-by-one. Thus, its performance is lower
  than that of QuickQP.
* its precision is also a bit  inferior  to  that  of   QuickQP.  BLEIC-QP
  performs only LBFGS steps (no Newton steps), which are good at detecting
  neighborhood of the solution, buy needs many iterations to find solution
  with more than 6 digits of precision.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled constrained gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinQPSetScale()
    EpsF    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
    EpsX    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:  
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinQPSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited. NOTE: this  algorithm uses  LBFGS
                iterations,  which  are  relatively  cheap,  but   improve
                function value only a bit. So you will need many iterations
                to converge - from 0.1*N to 10*N, depending  on  problem's
                condition number.

IT IS VERY IMPORTANT TO CALL MinQPSetScale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS STOPPING CRITERIA ARE SCALE-DEPENDENT!

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection (presently it is  small    step
length, but it may change in the future versions of ALGLIB).

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgobleic(minqpstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinQPSetAlgoBLEIC: EpsG is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsg,(double)(0)), "MinQPSetAlgoBLEIC: negative EpsG", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinQPSetAlgoBLEIC: EpsF is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsf,(double)(0)), "MinQPSetAlgoBLEIC: negative EpsF", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinQPSetAlgoBLEIC: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinQPSetAlgoBLEIC: negative EpsX", _state);
    ae_assert(maxits>=0, "MinQPSetAlgoBLEIC: negative MaxIts!", _state);
    state->algokind = 2;
    if( ((ae_fp_eq(epsg,(double)(0))&&ae_fp_eq(epsf,(double)(0)))&&ae_fp_eq(epsx,(double)(0)))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->qpbleicsettingsuser.epsg = epsg;
    state->qpbleicsettingsuser.epsf = epsf;
    state->qpbleicsettingsuser.epsx = epsx;
    state->qpbleicsettingsuser.maxits = maxits;
}


/*************************************************************************
This function tells QP solver to use DENSE-AUL algorithm and sets stopping
criteria for the algorithm.

This  algorithm  is  intended  for  non-convex problems with moderate  (up
to several thousands) variable count and arbitrary number  of  constraints
which are either (a) effectively convexified under constraints or (b) have
unique solution even with nonconvex target.

IMPORTANT: when DENSE-IPM solver is applicable, its performance is usually
           much better than that of DENSE-AUL.
           We recommend  you to use DENSE-AUL only when other solvers  can
           not be used.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints
* convergence is theoretically proved for positive-definite  (convex)   QP
  problems. Semidefinite and non-convex problems can be solved as long  as
  they  are   bounded  from  below  under  constraints,  although  without
  theoretical guarantees.

ALGORITHM OUTLINE:

* this  algorithm   is   an   augmented   Lagrangian   method  with  dense
  preconditioner (hence  its  name).
* it performs several outer iterations in order to refine  values  of  the
  Lagrange multipliers. Single outer  iteration  is  a  solution  of  some
  unconstrained optimization problem: first  it  performs  dense  Cholesky
  factorization of the Hessian in order to build preconditioner  (adaptive
  regularization is applied to enforce positive  definiteness),  and  then
  it uses L-BFGS optimizer to solve optimization problem.
* typically you need about 5-10 outer iterations to converge to solution
  
ALGORITHM LIMITATIONS:

* because dense Cholesky driver is used, this algorithm has O(N^2)  memory
  requirements and O(OuterIterations*N^3) minimum running time.  From  the
  practical  point  of  view,  it  limits  its  applicability  by  several
  thousands of variables.
  From  the  other  side,  variables  count  is  the most limiting factor,
  and dependence on constraint count is  much  more  lower. Assuming  that
  constraint matrix is sparse, it may handle tens of thousands  of general
  linear constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0, stopping criteria for inner optimizer.
                Inner  iterations  are  stopped  when  step  length  (with
                variable scaling being applied) is less than EpsX.
                See  minqpsetscale()  for  more  information  on  variable
                scaling.
    Rho     -   penalty coefficient, Rho>0:
                * large enough  that  algorithm  converges  with   desired
                  precision.
                * not TOO large to prevent ill-conditioning
                * recommended values are 100, 1000 or 10000
    ItsCnt  -   number of outer iterations:
                * recommended values: 10-15 (although  in  most  cases  it
                  converges within 5 iterations, you may need a  few  more
                  to be sure).
                * ItsCnt=0 means that small number of outer iterations  is
                  automatically chosen (10 iterations in current version).
                * ItsCnt=1 means that AUL algorithm performs just as usual
                  penalty method.
                * ItsCnt>1 means that  AUL  algorithm  performs  specified
                  number of outer iterations

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing  EpsX=0  will  lead  to  automatic  step  length  selection
      (specific step length chosen may change in the future  versions  of
      ALGLIB, so it is better to specify step length explicitly).

  -- ALGLIB --
     Copyright 20.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgodenseaul(minqpstate* state,
     double epsx,
     double rho,
     ae_int_t itscnt,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsx, _state), "MinQPSetAlgoDenseAUL: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinQPSetAlgoDenseAUL: negative EpsX", _state);
    ae_assert(ae_isfinite(rho, _state), "MinQPSetAlgoDenseAUL: Rho is not finite number", _state);
    ae_assert(ae_fp_greater(rho,(double)(0)), "MinQPSetAlgoDenseAUL: non-positive Rho", _state);
    ae_assert(itscnt>=0, "MinQPSetAlgoDenseAUL: negative ItsCnt!", _state);
    state->algokind = 4;
    if( ae_fp_eq(epsx,(double)(0)) )
    {
        epsx = 1.0E-8;
    }
    if( itscnt==0 )
    {
        itscnt = 10;
    }
    state->qpdenseaulsettingsuser.epsx = epsx;
    state->qpdenseaulsettingsuser.outerits = itscnt;
    state->qpdenseaulsettingsuser.rho = rho;
}


/*************************************************************************
This function tells QP solver to  use  DENSE-IPM  QP  algorithm  and  sets
stopping criteria for the algorithm.

This  algorithm  is  intended  for convex and semidefinite  problems  with
moderate (up to several thousands) variable count and arbitrary number  of
constraints.

IMPORTANT: this algorithm won't work for nonconvex problems, use DENSE-AUL
           or BLEIC-QP instead. If you try to  run  DENSE-IPM  on  problem
           with  indefinite  matrix  (matrix having  at least one negative
           eigenvalue) then depending on circumstances it may  either  (a)
           stall at some  arbitrary  point,  or  (b)  throw  exception  on
           failure of Cholesky decomposition.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints

ALGORITHM OUTLINE:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality
  
ALGORITHM LIMITATIONS:

* because dense Cholesky driver is used, for  N-dimensional  problem  with
  M dense constaints this algorithm has O(N^2+N*M) memory requirements and
  O(N^3+N*M^2) running time.
  Having sparse constraints with Z nonzeros per row  relaxes  storage  and
  running time down to O(N^2+M*Z) and O(N^3+N*Z^2)
  From the practical  point  of  view,  it  limits  its  applicability  by
  several thousands of variables.
  From  the  other  side,  variables  count  is  the most limiting factor,
  and dependence on constraint count is  much  more  lower. Assuming  that
  constraint matrix is sparse, it may handle tens of thousands  of general
  linear constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

===== TRACING IPM SOLVER =================================================

IPM solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'IPM'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'IPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("IPM,PREC.F6", "path/to/trace.log")
> 

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgodenseipm(minqpstate* state, double eps, ae_state *_state)
{


    ae_assert(ae_isfinite(eps, _state), "MinQPSetAlgoDenseIPM: Eps is not finite number", _state);
    ae_assert(ae_fp_greater_eq(eps,(double)(0)), "MinQPSetAlgoDenseIPM: negative Eps", _state);
    state->algokind = 5;
    state->veps = eps;
}


/*************************************************************************
This function tells QP solver to  use  SPARSE-IPM  QP algorithm  and  sets
stopping criteria for the algorithm.

This  algorithm  is  intended  for convex and semidefinite  problems  with
large  variable  and  constraint  count  and  sparse  quadratic  term  and
constraints. It is possible to have  some  limited  set  of  dense  linear
constraints - they will be handled separately by dense BLAS - but the more
dense constraints you have, the more time solver needs.

IMPORTANT: internally this solver performs large  and  sparse  (N+M)x(N+M)
           triangular factorization. So it expects both quadratic term and
           constraints to be highly sparse. However, its  running  time is
           influenced by BOTH fill factor and sparsity pattern.

           Generally we expect that no more than few nonzero  elements per
           row are present. However different sparsity patterns may result
           in completely different running  times  even  given  same  fill
           factor.
           
           In many cases this algorithm outperforms DENSE-IPM by order  of
           magnitude. However, in some cases you may  get  better  results
           with DENSE-IPM even when solving sparse task.

IMPORTANT: this algorithm won't work for nonconvex problems, use DENSE-AUL
           or BLEIC-QP instead. If you try to  run  DENSE-IPM  on  problem
           with  indefinite  matrix  (matrix having  at least one negative
           eigenvalue) then depending on circumstances it may  either  (a)
           stall at some  arbitrary  point,  or  (b)  throw  exception  on
           failure of Cholesky decomposition.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints
* specializes on large-scale sparse problems

ALGORITHM OUTLINE:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality
  
ALGORITHM LIMITATIONS:

* this algorithm may handle moderate number  of dense constraints, usually
  no more than a thousand of dense ones without losing its efficiency.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

===== TRACING IPM SOLVER =================================================

IPM solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'IPM'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'IPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("IPM,PREC.F6", "path/to/trace.log")
> 

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgosparseipm(minqpstate* state,
     double eps,
     ae_state *_state)
{


    ae_assert(ae_isfinite(eps, _state), "MinQPSetAlgoSparseIPM: Eps is not finite number", _state);
    ae_assert(ae_fp_greater_eq(eps,(double)(0)), "MinQPSetAlgoSparseIPM: negative Eps", _state);
    state->algokind = 6;
    state->veps = eps;
}


/*************************************************************************
This function tells solver to use QuickQP  algorithm:  special  extra-fast
algorithm for problems with box-only constrants. It may  solve  non-convex
problems as long as they are bounded from below under constraints.

ALGORITHM FEATURES:
* several times faster than DENSE-IPM when running on box-only problem
* utilizes accelerated methods for activation of constraints.
* supports dense and sparse QP problems
* supports ONLY box constraints; general linear constraints are NOT
  supported by this solver
* can solve all types of problems  (convex,  semidefinite,  nonconvex)  as
  long as they are bounded from below under constraints.
  Say, it is possible to solve "min{-x^2} subject to -1<=x<=+1".
  In convex/semidefinite case global minimum  is  returned,  in  nonconvex
  case - algorithm returns one of the local minimums.

ALGORITHM OUTLINE:

* algorithm  performs  two kinds of iterations: constrained CG  iterations
  and constrained Newton iterations
* initially it performs small number of constrained CG  iterations,  which
  can efficiently activate/deactivate multiple constraints
* after CG phase algorithm tries to calculate Cholesky  decomposition  and
  to perform several constrained Newton steps. If  Cholesky  decomposition
  failed (matrix is indefinite even under constraints),  we  perform  more
  CG iterations until we converge to such set of constraints  that  system
  matrix becomes  positive  definite.  Constrained  Newton  steps  greatly
  increase convergence speed and precision.
* algorithm interleaves CG and Newton iterations which  allows  to  handle
  indefinite matrices (CG phase) and quickly converge after final  set  of
  constraints is found (Newton phase). Combination of CG and Newton phases
  is called "outer iteration".
* it is possible to turn off Newton  phase  (beneficial  for  semidefinite
  problems - Cholesky decomposition will fail too often)
  
ALGORITHM LIMITATIONS:

* algorithm does not support general  linear  constraints;  only  box ones
  are supported
* Cholesky decomposition for sparse problems  is  performed  with  Skyline
  Cholesky solver, which is intended for low-profile matrices. No profile-
  reducing reordering of variables is performed in this version of ALGLIB.
* problems with near-zero negative eigenvalues (or exacty zero  ones)  may
  experience about 2-3x performance penalty. The reason is  that  Cholesky
  decomposition can not be performed until we identify directions of  zero
  and negative curvature and activate corresponding boundary constraints -
  but we need a lot of trial and errors because these directions  are hard
  to notice in the matrix spectrum.
  In this case you may turn off Newton phase of algorithm.
  Large negative eigenvalues  are  not  an  issue,  so  highly  non-convex
  problems can be solved very efficiently.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled constrained gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinQPSetScale()
    EpsF    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
    EpsX    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:  
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinQPSetScale()
    MaxOuterIts-maximum number of OUTER iterations.  One  outer  iteration
                includes some amount of CG iterations (from 5 to  ~N)  and
                one or several (usually small amount) Newton steps.  Thus,
                one outer iteration has high cost, but can greatly  reduce
                funcation value.
                Use 0 if you do not want to limit number of outer iterations.
    UseNewton-  use Newton phase or not:
                * Newton phase improves performance of  positive  definite
                  dense problems (about 2 times improvement can be observed)
                * can result in some performance penalty  on  semidefinite
                  or slightly negative definite  problems  -  each  Newton
                  phase will bring no improvement (Cholesky failure),  but
                  still will require computational time.
                * if you doubt, you can turn off this  phase  -  optimizer
                  will retain its most of its high speed.

IT IS VERY IMPORTANT TO CALL MinQPSetScale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS STOPPING CRITERIA ARE SCALE-DEPENDENT!

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection (presently it is  small    step
length, but it may change in the future versions of ALGLIB).

  -- ALGLIB --
     Copyright 22.05.2014 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgoquickqp(minqpstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxouterits,
     ae_bool usenewton,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsg, _state), "MinQPSetAlgoQuickQP: EpsG is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsg,(double)(0)), "MinQPSetAlgoQuickQP: negative EpsG", _state);
    ae_assert(ae_isfinite(epsf, _state), "MinQPSetAlgoQuickQP: EpsF is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsf,(double)(0)), "MinQPSetAlgoQuickQP: negative EpsF", _state);
    ae_assert(ae_isfinite(epsx, _state), "MinQPSetAlgoQuickQP: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinQPSetAlgoQuickQP: negative EpsX", _state);
    ae_assert(maxouterits>=0, "MinQPSetAlgoQuickQP: negative MaxOuterIts!", _state);
    state->algokind = 3;
    if( ((ae_fp_eq(epsg,(double)(0))&&ae_fp_eq(epsf,(double)(0)))&&ae_fp_eq(epsx,(double)(0)))&&maxouterits==0 )
    {
        epsx = 1.0E-6;
    }
    state->qqpsettingsuser.maxouterits = maxouterits;
    state->qqpsettingsuser.epsg = epsg;
    state->qqpsettingsuser.epsf = epsf;
    state->qqpsettingsuser.epsx = epsx;
    state->qqpsettingsuser.cnphase = usenewton;
}


/*************************************************************************
This function sets box constraints for QP solver

Box constraints are inactive by default (after  initial  creation).  After
being  set,  they are  preserved until explicitly overwritten with another
minqpsetbc()  or  minqpsetbcall()  call,  or  partially  overwritten  with
minqpsetbci() call.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

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

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).
      
NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.
      
NOTE: if constraints for all variables are same you may use minqpsetbcall()
      which allows to specify constraints without using arrays.
      
NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbc(minqpstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "MinQPSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinQPSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinQPSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinQPSetBC: BndU contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->havebndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->havebndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function sets box constraints for QP solver (all variables  at  once,
same constraints for all variables)

Box constraints are inactive by default (after  initial  creation).  After
being  set,  they are  preserved until explicitly overwritten with another
minqpsetbc()  or  minqpsetbcall()  call,  or  partially  overwritten  with
minqpsetbci() call.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd                BndL=BndU
    lower bound         BndL<=x[i]              BndU=+INF
    upper bound         x[i]<=BndU              BndL=-INF
    range               BndL<=x[i]<=BndU        ...
    free variable       -                       BndL=-INF, BndU+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bound, same for all variables
    BndU    -   upper bound, same for all variables

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).
      
NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.
      
NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbcall(minqpstate* state,
     double bndl,
     double bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(ae_isfinite(bndl, _state)||ae_isneginf(bndl, _state), "MinQPSetBCAll: BndL is NAN or +INF", _state);
    ae_assert(ae_isfinite(bndu, _state)||ae_isposinf(bndu, _state), "MinQPSetBCAll: BndU is NAN or -INF", _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = bndl;
        state->bndu.ptr.p_double[i] = bndu;
        state->havebndl.ptr.p_bool[i] = ae_isfinite(bndl, _state);
        state->havebndu.ptr.p_bool[i] = ae_isfinite(bndu, _state);
    }
}


/*************************************************************************
This function sets box constraints for I-th variable (other variables are
not modified).

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd                BndL=BndU
    lower bound         BndL<=x[i]              BndU=+INF
    upper bound         x[i]<=BndU              BndL=-INF
    range               BndL<=x[i]<=BndU        ...
    free variable       -                       BndL=-INF, BndU+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bound
    BndU    -   upper bound

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).
      
NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.
      
NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbci(minqpstate* state,
     ae_int_t i,
     double bndl,
     double bndu,
     ae_state *_state)
{


    ae_assert(i>=0&&i<state->n, "MinQPSetBCi: I is outside of [0,N)", _state);
    ae_assert(ae_isfinite(bndl, _state)||ae_isneginf(bndl, _state), "MinQPSetBCi: BndL is NAN or +INF", _state);
    ae_assert(ae_isfinite(bndu, _state)||ae_isposinf(bndu, _state), "MinQPSetBCi: BndU is NAN or -INF", _state);
    state->bndl.ptr.p_double[i] = bndl;
    state->bndu.ptr.p_double[i] = bndu;
    state->havebndl.ptr.p_bool[i] = ae_isfinite(bndl, _state);
    state->havebndu.ptr.p_bool[i] = ae_isfinite(bndu, _state);
}


/*************************************************************************
This function sets dense linear constraints for QP optimizer.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().  After  call  to  this  function
all non-box constraints are dropped, and you have only  those  constraints
which were specified in the present call.

If you want  to  specify  mixed  (with  dense  and  sparse  terms)  linear
constraints, you should call minqpsetlcmixed().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
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

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations (BLEIC-QP solver is most  precise,  AUL-QP
        solver is less precise).

  -- ALGLIB --
     Copyright 19.06.2012 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc(minqpstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state)
{
    ae_frame _frame_block;
    sparsematrix dummyc;
    ae_vector dummyct;

    ae_frame_make(_state, &_frame_block);
    memset(&dummyc, 0, sizeof(dummyc));
    memset(&dummyct, 0, sizeof(dummyct));
    _sparsematrix_init(&dummyc, _state, ae_true);
    ae_vector_init(&dummyct, 0, DT_INT, _state, ae_true);

    minqpsetlcmixed(state, &dummyc, &dummyct, 0, c, ct, k, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function sets sparse linear constraints for QP optimizer.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().  After  call  to  this  function
all non-box constraints are dropped, and you have only  those  constraints
which were specified in the present call.

If you want  to  specify  mixed  (with  dense  and  sparse  terms)  linear
constraints, you should call minqpsetlcmixed().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    C       -   linear  constraints,  sparse  matrix  with  dimensions  at
                least [K,N+1]. If matrix has  larger  size,  only  leading
                Kx(N+1) rectangle is used.
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations (BLEIC-QP solver is most  precise,  AUL-QP
        solver is less precise).

  -- ALGLIB --
     Copyright 22.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcsparse(minqpstate* state,
     const sparsematrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix dummyc;
    ae_vector dummyct;

    ae_frame_make(_state, &_frame_block);
    memset(&dummyc, 0, sizeof(dummyc));
    memset(&dummyct, 0, sizeof(dummyct));
    ae_matrix_init(&dummyc, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&dummyct, 0, DT_INT, _state, ae_true);

    minqpsetlcmixed(state, c, ct, k, &dummyc, &dummyct, 0, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function sets mixed linear constraints, which include a set of  dense
rows, and a set of sparse rows.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().

This function may be useful if constraint matrix includes large number  of
both types of rows - dense and sparse. If you have just a few sparse rows,
you  may  represent  them  in  dense  format  without losing  performance.
Similarly, if you have just a few dense rows, you may store them in sparse
format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    SparseC -   linear constraints, sparse  matrix with dimensions EXACTLY
                EQUAL TO [SparseK,N+1].  Each  row  of  C  represents  one
                constraint, either equality or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    SparseCT-   type of sparse constraints, array[K]:
                * if SparseCT[i]>0, then I-th constraint is SparseC[i,*]*x >= SparseC[i,n+1]
                * if SparseCT[i]=0, then I-th constraint is SparseC[i,*]*x  = SparseC[i,n+1]
                * if SparseCT[i]<0, then I-th constraint is SparseC[i,*]*x <= SparseC[i,n+1]
    SparseK -   number of sparse equality/inequality constraints, K>=0
    DenseC  -   dense linear constraints, array[K,N+1].
                Each row of DenseC represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of DenseC (including right part) must be finite.
    DenseCT -   type of constraints, array[K]:
                * if DenseCT[i]>0, then I-th constraint is DenseC[i,*]*x >= DenseC[i,n+1]
                * if DenseCT[i]=0, then I-th constraint is DenseC[i,*]*x  = DenseC[i,n+1]
                * if DenseCT[i]<0, then I-th constraint is DenseC[i,*]*x <= DenseC[i,n+1]
    DenseK  -   number of equality/inequality constraints, DenseK>=0

NOTE 1: linear (non-box) constraints  are  satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations (BLEIC-QP solver is most  precise,  AUL-QP
        solver is less precise).

NOTE 2: due to backward compatibility reasons SparseC can be  larger  than
        [SparseK,N+1]. In this case only leading  [SparseK,N+1]  submatrix
        will be  used.  However,  the  rest  of  ALGLIB  has  more  strict
        requirements on the input size, so we recommend you to pass sparse
        term whose size exactly matches algorithm expectations.

  -- ALGLIB --
     Copyright 22.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcmixed(minqpstate* state,
     const sparsematrix* sparsec,
     /* Integer */ const ae_vector* sparsect,
     ae_int_t sparsek,
     /* Real    */ const ae_matrix* densec,
     /* Integer */ const ae_vector* densect,
     ae_int_t densek,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    double v;
    ae_vector srcidx;
    ae_vector dstidx;
    ae_vector s;
    ae_vector rs;
    ae_vector eoffs;
    ae_vector roffs;
    ae_vector v2;
    ae_vector eidx;
    ae_vector eval;
    ae_int_t t0;
    ae_int_t t1;
    ae_int_t nnz;

    ae_frame_make(_state, &_frame_block);
    memset(&srcidx, 0, sizeof(srcidx));
    memset(&dstidx, 0, sizeof(dstidx));
    memset(&s, 0, sizeof(s));
    memset(&rs, 0, sizeof(rs));
    memset(&eoffs, 0, sizeof(eoffs));
    memset(&roffs, 0, sizeof(roffs));
    memset(&v2, 0, sizeof(v2));
    memset(&eidx, 0, sizeof(eidx));
    memset(&eval, 0, sizeof(eval));
    ae_vector_init(&srcidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&dstidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&s, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&rs, 0, DT_INT, _state, ae_true);
    ae_vector_init(&eoffs, 0, DT_INT, _state, ae_true);
    ae_vector_init(&roffs, 0, DT_INT, _state, ae_true);
    ae_vector_init(&v2, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&eidx, 0, DT_INT, _state, ae_true);
    ae_vector_init(&eval, 0, DT_REAL, _state, ae_true);

    n = state->n;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(densek>=0, "MinQPSetLCMixed: K<0", _state);
    ae_assert(densek==0||densec->cols>=n+1, "MinQPSetLCMixed: Cols(C)<N+1", _state);
    ae_assert(densec->rows>=densek, "MinQPSetLCMixed: Rows(DenseC)<DenseK", _state);
    ae_assert(densect->cnt>=densek, "MinQPSetLCMixed: Length(DenseCT)<DenseK", _state);
    ae_assert(apservisfinitematrix(densec, densek, n+1, _state), "MinQPSetLCMixed: C contains infinite or NaN values!", _state);
    ae_assert(sparsek>=0, "MinQPSetLCMixed: SparseK<0", _state);
    ae_assert(sparsek==0||sparsegetncols(sparsec, _state)>=n+1, "MinQPSetLCMixed: Cols(SparseC)<N+1", _state);
    ae_assert(sparsek==0||sparsegetnrows(sparsec, _state)>=sparsek, "MinQPSetLCMixed: Rows(SparseC)<SparseK", _state);
    ae_assert(sparsect->cnt>=sparsek, "MinQPSetLCMixed: Length(SparseCT)<SparseK", _state);
    
    /*
     * Allocate place for Lagrange multipliers, fill by zero
     */
    rvectorsetlengthatleast(&state->replaglc, densek+sparsek, _state);
    for(i=0; i<=densek+sparsek-1; i++)
    {
        state->replaglc.ptr.p_double[i] = 0.0;
    }
    
    /*
     * Init
     */
    ae_vector_set_length(&state->cl, densek+sparsek, _state);
    ae_vector_set_length(&state->cu, densek+sparsek, _state);
    state->mdense = densek;
    state->msparse = sparsek;
    if( sparsek>0 )
    {
        
        /*
         * Evaluate row sizes for new storage
         */
        ae_vector_set_length(&rs, sparsek, _state);
        for(i=0; i<=sparsek-1; i++)
        {
            rs.ptr.p_int[i] = 0;
        }
        t0 = 0;
        t1 = 0;
        nnz = 0;
        while(sparseenumerate(sparsec, &t0, &t1, &i, &j, &v, _state))
        {
            if( i>sparsek-1||j>n-1 )
            {
                continue;
            }
            ae_assert(ae_isfinite(v, _state), "MinQPSetLCSparse: C contains infinite or NAN values", _state);
            nnz = nnz+1;
            rs.ptr.p_int[i] = rs.ptr.p_int[i]+1;
        }
        
        /*
         * Prepare new sparse CRS storage, copy leading SparseK*N submatrix into the storage
         */
        for(i=0; i<=sparsek-1; i++)
        {
            state->cl.ptr.p_double[i] = (double)(0);
            state->cu.ptr.p_double[i] = (double)(0);
        }
        state->sparsec.m = sparsek;
        state->sparsec.n = n;
        ivectorsetlengthatleast(&state->sparsec.ridx, sparsek+1, _state);
        ivectorsetlengthatleast(&state->sparsec.idx, nnz, _state);
        rvectorsetlengthatleast(&state->sparsec.vals, nnz, _state);
        ae_vector_set_length(&eoffs, sparsek+1, _state);
        state->sparsec.ridx.ptr.p_int[0] = 0;
        eoffs.ptr.p_int[0] = 0;
        for(i=1; i<=sparsek; i++)
        {
            state->sparsec.ridx.ptr.p_int[i] = state->sparsec.ridx.ptr.p_int[i-1]+rs.ptr.p_int[i-1];
            eoffs.ptr.p_int[i] = state->sparsec.ridx.ptr.p_int[i];
        }
        t0 = 0;
        t1 = 0;
        while(sparseenumerate(sparsec, &t0, &t1, &i, &j, &v, _state))
        {
            if( i>sparsek-1||j>n )
            {
                continue;
            }
            if( j<n )
            {
                
                /*
                 * Copy left part of constraint
                 */
                j0 = eoffs.ptr.p_int[i];
                state->sparsec.idx.ptr.p_int[j0] = j;
                state->sparsec.vals.ptr.p_double[j0] = v;
                eoffs.ptr.p_int[i] = j0+1;
            }
            else
            {
                
                /*
                 * Handle right part of the constraint
                 */
                state->cl.ptr.p_double[i] = v;
                state->cu.ptr.p_double[i] = v;
            }
        }
        for(i=0; i<=sparsek-1; i++)
        {
            ae_assert(eoffs.ptr.p_int[i]==state->sparsec.ridx.ptr.p_int[i+1], "MinQP: critical integrity check failed (sparse copying)", _state);
        }
        sparsecreatecrsinplace(&state->sparsec, _state);
        for(i=0; i<=sparsek-1; i++)
        {
            if( sparsect->ptr.p_int[i]>0 )
            {
                state->cu.ptr.p_double[i] = _state->v_posinf;
            }
            if( sparsect->ptr.p_int[i]<0 )
            {
                state->cl.ptr.p_double[i] = _state->v_neginf;
            }
        }
    }
    if( densek>0 )
    {
        
        /*
         * Copy dense constraints
         */
        rmatrixsetlengthatleast(&state->densec, densek, n, _state);
        for(i=0; i<=densek-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                state->densec.ptr.pp_double[i][j] = densec->ptr.pp_double[i][j];
            }
            if( densect->ptr.p_int[i]>0 )
            {
                state->cl.ptr.p_double[sparsek+i] = densec->ptr.pp_double[i][n];
                state->cu.ptr.p_double[sparsek+i] = _state->v_posinf;
                continue;
            }
            if( densect->ptr.p_int[i]<0 )
            {
                state->cl.ptr.p_double[sparsek+i] = _state->v_neginf;
                state->cu.ptr.p_double[sparsek+i] = densec->ptr.pp_double[i][n];
                continue;
            }
            state->cl.ptr.p_double[sparsek+i] = densec->ptr.pp_double[i][n];
            state->cu.ptr.p_double[sparsek+i] = densec->ptr.pp_double[i][n];
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This function provides legacy API for specification of mixed  dense/sparse
linear constraints.

New conventions used by ALGLIB since release  3.16.0  state  that  set  of
sparse constraints comes first,  followed  by  set  of  dense  ones.  This
convention is essential when you talk about things like order of  Lagrange
multipliers.

However, legacy API accepted mixed  constraints  in  reverse  order.  This
function is here to simplify situation with code relying on legacy API. It
simply accepts constraints in one order (old) and passes them to new  API,
now in correct order.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcmixedlegacy(minqpstate* state,
     /* Real    */ const ae_matrix* densec,
     /* Integer */ const ae_vector* densect,
     ae_int_t densek,
     const sparsematrix* sparsec,
     /* Integer */ const ae_vector* sparsect,
     ae_int_t sparsek,
     ae_state *_state)
{


    minqpsetlcmixed(state, sparsec, sparsect, sparsek, densec, densect, densek, _state);
}


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

NOTE: knowing  that  constraint  matrix  is  dense  helps  some QP solvers
      (especially modern IPM method) to utilize efficient  dense  Level  3
      BLAS for dense parts of the problem. If your problem has both  dense
      and sparse constraints, you  can  use  minqpsetlc2mixed()  function,
      which will result in dense algebra being applied to dense terms, and
      sparse sparse linear algebra applied to sparse terms.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
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
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2dense(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state)
{


    minqpsetlc2mixed(state, &state->dummysparse, 0, a, k, al, au, _state);
}


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    A       -   sparse matrix with size [K,N] (exactly!).
                Each row of A represents one general linear constraint.
                A can be stored in any sparse storage format.
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                is specified, A, AL, AU are ignored.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2(minqpstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state)
{


    minqpsetlc2mixed(state, a, k, &state->dummyr2, 0, al, au, _state);
}


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
mixed constraining matrix A including sparse part (first SparseK rows) and
dense part (last DenseK rows). Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

This function may be useful if constraint matrix includes large number  of
both types of rows - dense and sparse. If you have just a few sparse rows,
you  may  represent  them  in  dense  format  without losing  performance.
Similarly, if you have just a few dense rows, you may store them in sparse
format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    SparseA -   sparse matrix with size [K,N] (exactly!).
                Each row of A represents one general linear constraint.
                A can be stored in any sparse storage format.
    SparseK -   number of sparse constraints, SparseK>=0
    DenseA  -   linear constraints, array[K,N], set of dense constraints.
                Each row of A represents one general linear constraint.
    DenseK  -   number of dense constraints, DenseK>=0
    AL, AU  -   lower and upper bounds, array[SparseK+DenseK], with former
                SparseK elements corresponding to sparse constraints,  and
                latter DenseK elements corresponding to dense constraints;
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                is specified, A, AL, AU are ignored.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2mixed(minqpstate* state,
     const sparsematrix* sparsea,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t kdense,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;


    n = state->n;
    m = kdense+ksparse;
    
    /*
     * Check input arguments
     */
    ae_assert(ksparse>=0, "MinQPSetLC2Mixed: KSparse<0", _state);
    ae_assert(ksparse==0||sparsegetncols(sparsea, _state)==n, "MinQPSetLC2: Cols(SparseA)<>N", _state);
    ae_assert(ksparse==0||sparsegetnrows(sparsea, _state)==ksparse, "MinQPSetLC2: Rows(SparseA)<>K", _state);
    ae_assert(kdense>=0, "MinQPSetLC2Mixed: KDense<0", _state);
    ae_assert(kdense==0||densea->cols>=n, "MinQPSetLC2Mixed: Cols(DenseA)<N", _state);
    ae_assert(kdense==0||densea->rows>=kdense, "MinQPSetLC2Mixed: Rows(DenseA)<K", _state);
    ae_assert(apservisfinitematrix(densea, kdense, n, _state), "MinQPSetLC2Mixed: DenseA contains infinite or NaN values!", _state);
    ae_assert(al->cnt>=kdense+ksparse, "MinQPSetLC2Mixed: Length(AL)<K", _state);
    ae_assert(au->cnt>=kdense+ksparse, "MinQPSetLC2Mixed: Length(AU)<K", _state);
    for(i=0; i<=m-1; i++)
    {
        ae_assert(ae_isfinite(al->ptr.p_double[i], _state)||ae_isneginf(al->ptr.p_double[i], _state), "MinQPSetLC2Mixed: AL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(au->ptr.p_double[i], _state)||ae_isposinf(au->ptr.p_double[i], _state), "MinQPSetLC2Mixed: AU contains NAN or -INF", _state);
    }
    
    /*
     * Allocate place for Lagrange multipliers, fill by zero
     */
    rvectorsetlengthatleast(&state->replaglc, kdense+ksparse, _state);
    for(i=0; i<=kdense+ksparse-1; i++)
    {
        state->replaglc.ptr.p_double[i] = 0.0;
    }
    
    /*
     * Quick exit if needed
     */
    if( m==0 )
    {
        state->mdense = 0;
        state->msparse = 0;
        return;
    }
    
    /*
     * Prepare
     */
    rvectorsetlengthatleast(&state->cl, m, _state);
    rvectorsetlengthatleast(&state->cu, m, _state);
    for(i=0; i<=m-1; i++)
    {
        state->cl.ptr.p_double[i] = al->ptr.p_double[i];
        state->cu.ptr.p_double[i] = au->ptr.p_double[i];
    }
    state->mdense = kdense;
    state->msparse = ksparse;
    
    /*
     * Copy dense and sparse terms
     */
    if( ksparse>0 )
    {
        sparsecopytocrsbuf(sparsea, &state->sparsec, _state);
    }
    if( kdense>0 )
    {
        rmatrixsetlengthatleast(&state->densec, kdense, n, _state);
        rmatrixcopy(kdense, n, densea, 0, 0, &state->densec, 0, 0, _state);
    }
}


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
matrix of currently present dense constraints.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    A       -   linear constraint coefficient, array[N], right side is NOT
                included.
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint Ai*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint Ai*x<=AU
                * AU=+INF  => one-sided constraint AL<=Ai*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minqpaddlc2dense(minqpstate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(a->cnt>=n, "MinQPAddLC2Dense: Length(A)<N", _state);
    ae_assert(isfinitevector(a, n, _state), "MinQPAddLC2Dense: A contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(al, _state)||ae_isneginf(al, _state), "MinQPAddLC2Dense: AL is NAN or +INF", _state);
    ae_assert(ae_isfinite(au, _state)||ae_isposinf(au, _state), "MinQPAddLC2Dense: AU is NAN or -INF", _state);
    rvectorgrowto(&state->cl, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->cu, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->replaglc, state->msparse+state->mdense+1, _state);
    rmatrixgrowrowsto(&state->densec, state->mdense+1, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->densec.ptr.pp_double[state->mdense][i] = a->ptr.p_double[i];
    }
    state->cl.ptr.p_double[state->msparse+state->mdense] = al;
    state->cu.ptr.p_double[state->msparse+state->mdense] = au;
    state->replaglc.ptr.p_double[state->msparse+state->mdense] = 0.0;
    inc(&state->mdense, _state);
}


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint is passed in compressed format: as list of non-zero entries  of
coefficient vector A. Such approach is more efficient than  dense  storage
for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    IdxA    -   array[NNZ], indexes of non-zero elements of A:
                * can be unsorted
                * can include duplicate indexes (corresponding entries  of
                  ValA[] will be summed)
    ValA    -   array[NNZ], values of non-zero elements of A
    NNZ     -   number of non-zero coefficients in A
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint A*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint A*x<=AU
                * AU=+INF  => one-sided constraint AL<=A*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minqpaddlc2(minqpstate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t offs;
    ae_int_t offsdst;
    ae_int_t n;
    ae_int_t didx;
    ae_int_t uidx;


    n = state->n;
    
    /*
     * Check inputs
     */
    ae_assert(nnz>=0, "MinQPAddLC2: NNZ<0", _state);
    ae_assert(idxa->cnt>=nnz, "MinQPAddLC2: Length(IdxA)<NNZ", _state);
    ae_assert(vala->cnt>=nnz, "MinQPAddLC2: Length(ValA)<NNZ", _state);
    for(i=0; i<=nnz-1; i++)
    {
        ae_assert(idxa->ptr.p_int[i]>=0&&idxa->ptr.p_int[i]<n, "MinQPAddLC2: IdxA contains indexes outside of [0,N) range", _state);
    }
    ae_assert(isfinitevector(vala, nnz, _state), "MinQPAddLC2: ValA contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(al, _state)||ae_isneginf(al, _state), "MinQPAddLC2: AL is NAN or +INF", _state);
    ae_assert(ae_isfinite(au, _state)||ae_isposinf(au, _state), "MinQPAddLC2: AU is NAN or -INF", _state);
    
    /*
     * If M=0, it means that A is uninitialized.
     * Prepare sparse matrix structure
     */
    if( state->msparse==0 )
    {
        state->sparsec.matrixtype = 1;
        state->sparsec.m = 0;
        state->sparsec.n = n;
        state->sparsec.ninitialized = 0;
        ivectorsetlengthatleast(&state->sparsec.ridx, 1, _state);
        state->sparsec.ridx.ptr.p_int[0] = 0;
    }
    ae_assert(state->sparsec.matrixtype==1&&state->sparsec.m==state->msparse, "MinQPAddLC2: integrity check failed!", _state);
    
    /*
     * Reallocate inequality bounds
     */
    rvectorgrowto(&state->cl, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->cu, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->replaglc, state->msparse+state->mdense+1, _state);
    for(i=state->msparse+state->mdense; i>=state->msparse+1; i--)
    {
        state->cl.ptr.p_double[i] = state->cl.ptr.p_double[i-1];
        state->cu.ptr.p_double[i] = state->cu.ptr.p_double[i-1];
        state->replaglc.ptr.p_double[i] = state->replaglc.ptr.p_double[i-1];
    }
    state->cl.ptr.p_double[state->msparse] = al;
    state->cu.ptr.p_double[state->msparse] = au;
    state->replaglc.ptr.p_double[state->msparse] = 0.0;
    
    /*
     * Reallocate sparse storage
     */
    offs = state->sparsec.ridx.ptr.p_int[state->msparse];
    ivectorgrowto(&state->sparsec.idx, offs+nnz, _state);
    rvectorgrowto(&state->sparsec.vals, offs+nnz, _state);
    ivectorgrowto(&state->sparsec.didx, state->msparse+1, _state);
    ivectorgrowto(&state->sparsec.uidx, state->msparse+1, _state);
    ivectorgrowto(&state->sparsec.ridx, state->msparse+2, _state);
    
    /*
     * If NNZ=0, perform quick and simple row append. 
     */
    if( nnz==0 )
    {
        state->sparsec.didx.ptr.p_int[state->msparse] = state->sparsec.ridx.ptr.p_int[state->msparse];
        state->sparsec.uidx.ptr.p_int[state->msparse] = state->sparsec.ridx.ptr.p_int[state->msparse];
        state->sparsec.ridx.ptr.p_int[state->msparse+1] = state->sparsec.ridx.ptr.p_int[state->msparse];
        inc(&state->sparsec.m, _state);
        inc(&state->msparse, _state);
        return;
    }
    
    /*
     * Now we are sure that SparseC contains properly initialized sparse
     * matrix (or some appropriate dummy for M=0) and we have NNZ>0
     * (no need to care about degenerate cases).
     *
     * Append rows to SparseC:
     * * append data
     * * sort in place
     * * merge duplicate indexes
     * * compute DIdx and UIdx
     *
     */
    for(i=0; i<=nnz-1; i++)
    {
        state->sparsec.idx.ptr.p_int[offs+i] = idxa->ptr.p_int[i];
        state->sparsec.vals.ptr.p_double[offs+i] = vala->ptr.p_double[i];
    }
    tagsortmiddleir(&state->sparsec.idx, &state->sparsec.vals, offs, nnz, _state);
    offsdst = offs;
    for(i=1; i<=nnz-1; i++)
    {
        if( state->sparsec.idx.ptr.p_int[offsdst]!=state->sparsec.idx.ptr.p_int[offs+i] )
        {
            offsdst = offsdst+1;
            state->sparsec.idx.ptr.p_int[offsdst] = state->sparsec.idx.ptr.p_int[offs+i];
            state->sparsec.vals.ptr.p_double[offsdst] = state->sparsec.vals.ptr.p_double[offs+i];
        }
        else
        {
            state->sparsec.vals.ptr.p_double[offsdst] = state->sparsec.vals.ptr.p_double[offsdst]+state->sparsec.vals.ptr.p_double[offs+i];
        }
    }
    nnz = offsdst-offs+1;
    uidx = -1;
    didx = -1;
    for(j=offs; j<=offsdst; j++)
    {
        k = state->sparsec.idx.ptr.p_int[j];
        if( k==state->msparse )
        {
            didx = j;
        }
        else
        {
            if( k>state->msparse&&uidx==-1 )
            {
                uidx = j;
                break;
            }
        }
    }
    if( uidx==-1 )
    {
        uidx = offsdst+1;
    }
    if( didx==-1 )
    {
        didx = uidx;
    }
    state->sparsec.didx.ptr.p_int[state->msparse] = didx;
    state->sparsec.uidx.ptr.p_int[state->msparse] = uidx;
    state->sparsec.ridx.ptr.p_int[state->msparse+1] = offsdst+1;
    state->sparsec.ninitialized = state->sparsec.ridx.ptr.p_int[state->msparse+1];
    inc(&state->sparsec.m, _state);
    inc(&state->msparse, _state);
}


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint vector A is  passed  as  a  dense  array  which  is  internally
sparsified by this function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    DA      -   array[N], constraint vector
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint A*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint A*x<=AU
                * AU=+INF  => one-sided constraint AL<=A*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minqpaddlc2sparsefromdense(minqpstate* state,
     /* Real    */ const ae_vector* da,
     double al,
     double au,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t nzi;
    ae_int_t offs;
    ae_int_t n;
    ae_int_t nnz;
    ae_int_t didx;
    ae_int_t uidx;


    n = state->n;
    
    /*
     * Check inputs
     */
    ae_assert(da->cnt>=n, "MinQPAddLC2SparseFromDense: Length(DA)<N", _state);
    ae_assert(isfinitevector(da, n, _state), "MinQPAddLC2SparseFromDense: DA contains infinities/NANs", _state);
    ae_assert(ae_isfinite(al, _state)||ae_isneginf(al, _state), "MinQPAddLC2SparseFromDense: AL is NAN or +INF", _state);
    ae_assert(ae_isfinite(au, _state)||ae_isposinf(au, _state), "MinQPAddLC2SparseFromDense: AU is NAN or -INF", _state);
    
    /*
     * If M=0, it means that A is uninitialized.
     * Prepare sparse matrix structure
     */
    if( state->msparse==0 )
    {
        state->sparsec.matrixtype = 1;
        state->sparsec.m = 0;
        state->sparsec.n = n;
        state->sparsec.ninitialized = 0;
        ivectorsetlengthatleast(&state->sparsec.ridx, 1, _state);
        state->sparsec.ridx.ptr.p_int[0] = 0;
    }
    ae_assert(state->sparsec.matrixtype==1&&state->sparsec.m==state->msparse, "MinQPAddLC2SparseFromDense: integrity check failed!", _state);
    
    /*
     * Reallocate inequality bounds
     */
    rvectorgrowto(&state->cl, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->cu, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->replaglc, state->msparse+state->mdense+1, _state);
    for(i=state->msparse+state->mdense; i>=state->msparse+1; i--)
    {
        state->cl.ptr.p_double[i] = state->cl.ptr.p_double[i-1];
        state->cu.ptr.p_double[i] = state->cu.ptr.p_double[i-1];
        state->replaglc.ptr.p_double[i] = state->replaglc.ptr.p_double[i-1];
    }
    state->cl.ptr.p_double[state->msparse] = al;
    state->cu.ptr.p_double[state->msparse] = au;
    state->replaglc.ptr.p_double[state->msparse] = 0.0;
    
    /*
     * Determine nonzeros count.
     * Reallocate sparse storage.
     */
    nnz = 0;
    for(i=0; i<=n-1; i++)
    {
        if( !(da->ptr.p_double[i]==0.0) )
        {
            nnz = nnz+1;
        }
    }
    offs = state->sparsec.ridx.ptr.p_int[state->msparse];
    ivectorgrowto(&state->sparsec.idx, offs+nnz, _state);
    rvectorgrowto(&state->sparsec.vals, offs+nnz, _state);
    ivectorgrowto(&state->sparsec.didx, state->msparse+1, _state);
    ivectorgrowto(&state->sparsec.uidx, state->msparse+1, _state);
    ivectorgrowto(&state->sparsec.ridx, state->msparse+2, _state);
    
    /*
     * If NNZ=0, perform quick and simple row append. 
     */
    if( nnz==0 )
    {
        state->sparsec.didx.ptr.p_int[state->msparse] = state->sparsec.ridx.ptr.p_int[state->msparse];
        state->sparsec.uidx.ptr.p_int[state->msparse] = state->sparsec.ridx.ptr.p_int[state->msparse];
        state->sparsec.ridx.ptr.p_int[state->msparse+1] = state->sparsec.ridx.ptr.p_int[state->msparse];
        inc(&state->sparsec.m, _state);
        inc(&state->msparse, _state);
        return;
    }
    
    /*
     * Now we are sure that SparseC contains properly initialized sparse
     * matrix (or some appropriate dummy for M=0) and we have NNZ>0
     * (no need to care about degenerate cases).
     *
     * Append rows to SparseC:
     * * append data
     * * compute DIdx and UIdx
     *
     */
    nzi = 0;
    for(i=0; i<=n-1; i++)
    {
        if( !(da->ptr.p_double[i]==0.0) )
        {
            state->sparsec.idx.ptr.p_int[offs+nzi] = i;
            state->sparsec.vals.ptr.p_double[offs+nzi] = da->ptr.p_double[i];
            nzi = nzi+1;
        }
    }
    uidx = -1;
    didx = -1;
    for(j=offs; j<=offs+nnz-1; j++)
    {
        k = state->sparsec.idx.ptr.p_int[j];
        if( k==state->msparse )
        {
            didx = j;
        }
        else
        {
            if( k>state->msparse&&uidx==-1 )
            {
                uidx = j;
                break;
            }
        }
    }
    if( uidx==-1 )
    {
        uidx = offs+nnz;
    }
    if( didx==-1 )
    {
        didx = uidx;
    }
    state->sparsec.didx.ptr.p_int[state->msparse] = didx;
    state->sparsec.uidx.ptr.p_int[state->msparse] = uidx;
    state->sparsec.ridx.ptr.p_int[state->msparse+1] = offs+nnz;
    state->sparsec.ninitialized = state->sparsec.ridx.ptr.p_int[state->msparse+1];
    inc(&state->sparsec.m, _state);
    inc(&state->msparse, _state);
}


/*************************************************************************
This function solves quadratic programming problem.

Prior to calling this function you should choose solver by means of one of
the following functions:

* minqpsetalgoquickqp()     - for QuickQP solver
* minqpsetalgobleic()       - for BLEIC-QP solver
* minqpsetalgodenseaul()    - for Dense-AUL-QP solver
* minqpsetalgodenseipm()    - for Dense-IPM-QP solver

These functions also allow you to control stopping criteria of the solver.
If you did not set solver,  MinQP  subpackage  will  automatically  select
solver for your problem and will run it with default stopping criteria.

However, it is better to set explicitly solver and its stopping criteria.

INPUT PARAMETERS:
    State   -   algorithm state

You should use MinQPResults() function to access results after calls
to this function.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey.
     Special thanks to Elvira Illarionova  for  important  suggestions  on
     the linearly constrained QP algorithm.
*************************************************************************/
void minqpoptimize(minqpstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t nbc;
    ae_int_t neq;
    ae_int_t nineq;
    ae_int_t curecpos;
    ae_int_t curicpos;


    n = state->n;
    m = state->mdense+state->msparse;
    state->repterminationtype = -5;
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    state->repncholesky = 0;
    state->repnmv = 0;
    
    /*
     * Zero-fill Lagrange multipliers (their initial value)
     */
    for(i=0; i<=n-1; i++)
    {
        state->replagbc.ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=m-1; i++)
    {
        state->replaglc.ptr.p_double[i] = 0.0;
    }
    
    /*
     * Initial point:
     * * if we have starting point in StartX, we just have to bound it
     * * if we do not have StartX, deduce initial point from boundary constraints
     */
    if( state->havex )
    {
        for(i=0; i<=n-1; i++)
        {
            state->xs.ptr.p_double[i] = state->startx.ptr.p_double[i];
            if( state->havebndl.ptr.p_bool[i]&&ae_fp_less(state->xs.ptr.p_double[i],state->bndl.ptr.p_double[i]) )
            {
                state->xs.ptr.p_double[i] = state->bndl.ptr.p_double[i];
            }
            if( state->havebndu.ptr.p_bool[i]&&ae_fp_greater(state->xs.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->xs.ptr.p_double[i] = state->bndu.ptr.p_double[i];
            }
        }
    }
    else
    {
        for(i=0; i<=n-1; i++)
        {
            if( state->havebndl.ptr.p_bool[i]&&state->havebndu.ptr.p_bool[i] )
            {
                state->xs.ptr.p_double[i] = 0.5*(state->bndl.ptr.p_double[i]+state->bndu.ptr.p_double[i]);
                continue;
            }
            if( state->havebndl.ptr.p_bool[i] )
            {
                state->xs.ptr.p_double[i] = state->bndl.ptr.p_double[i];
                continue;
            }
            if( state->havebndu.ptr.p_bool[i] )
            {
                state->xs.ptr.p_double[i] = state->bndu.ptr.p_double[i];
                continue;
            }
            state->xs.ptr.p_double[i] = (double)(0);
        }
    }
    
    /*
     * check correctness of constraints
     */
    for(i=0; i<=n-1; i++)
    {
        if( state->havebndl.ptr.p_bool[i]&&state->havebndu.ptr.p_bool[i] )
        {
            if( ae_fp_greater(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
            {
                state->repterminationtype = -3;
                return;
            }
        }
    }
    
    /*
     * count number of bound and linear constraints
     */
    nbc = 0;
    for(i=0; i<=n-1; i++)
    {
        if( state->havebndl.ptr.p_bool[i] )
        {
            nbc = nbc+1;
        }
        if( state->havebndu.ptr.p_bool[i] )
        {
            nbc = nbc+1;
        }
    }
    
    /*
     * Effective scale
     */
    rvectorsetlengthatleast(&state->effectives, n, _state);
    if( state->stype==0 )
    {
        
        /*
         * User scale (or default one)
         */
        for(i=0; i<=n-1; i++)
        {
            state->effectives.ptr.p_double[i] = state->s.ptr.p_double[i];
        }
    }
    else
    {
        if( state->stype==1 )
        {
            
            /*
             * Diagonal is used for scaling:
             * * unpack
             * * convert to scale, return error on failure
             */
            if( state->akind==0 )
            {
                
                /*
                 * Unpack CQM structure
                 */
                cqmgetdiaga(&state->a, &state->effectives, _state);
            }
            else
            {
                if( state->akind==1 )
                {
                    for(i=0; i<=n-1; i++)
                    {
                        state->effectives.ptr.p_double[i] = sparseget(&state->sparsea, i, i, _state);
                    }
                }
                else
                {
                    ae_assert(ae_false, "MinQPOptimize: integrity check failed", _state);
                }
            }
            for(i=0; i<=n-1; i++)
            {
                if( ae_fp_less_eq(state->effectives.ptr.p_double[i],(double)(0)) )
                {
                    state->repterminationtype = -9;
                    return;
                }
                state->effectives.ptr.p_double[i] = (double)1/ae_sqrt(state->effectives.ptr.p_double[i], _state);
            }
        }
        else
        {
            ae_assert(ae_false, "MinQPOptimize: integrity check failed", _state);
        }
    }
    
    /*
     * Solvers which can not handle new two-sided constraints need them to be
     * converted into legacy equality/inequality one-sided format
     */
    if( state->algokind==2||state->algokind==4 )
    {
        
        /*
         * Scan constraint left/right sides, count equality ones and one/two-sided inequality ones
         */
        neq = 0;
        nineq = 0;
        for(i=0; i<=m-1; i++)
        {
            if( (ae_isfinite(state->cl.ptr.p_double[i], _state)&&ae_isfinite(state->cu.ptr.p_double[i], _state))&&ae_fp_eq(state->cl.ptr.p_double[i],state->cu.ptr.p_double[i]) )
            {
                inc(&neq, _state);
                continue;
            }
            if( ae_isfinite(state->cl.ptr.p_double[i], _state) )
            {
                inc(&nineq, _state);
            }
            if( ae_isfinite(state->cu.ptr.p_double[i], _state) )
            {
                inc(&nineq, _state);
            }
        }
        
        /*
         * Perform conversion
         */
        rmatrixsetlengthatleast(&state->ecleic, neq+nineq, n+1, _state);
        rvectorsetlengthatleast(&state->elagmlt, neq+nineq, _state);
        ivectorsetlengthatleast(&state->elagidx, neq+nineq, _state);
        curecpos = 0;
        curicpos = neq;
        for(i=0; i<=m-1; i++)
        {
            if( (ae_isfinite(state->cl.ptr.p_double[i], _state)&&ae_isfinite(state->cu.ptr.p_double[i], _state))&&ae_fp_eq(state->cl.ptr.p_double[i],state->cu.ptr.p_double[i]) )
            {
                
                /*
                 * Offload equality constraint
                 */
                if( i<state->msparse )
                {
                    for(j=0; j<=n-1; j++)
                    {
                        state->ecleic.ptr.pp_double[curecpos][j] = (double)(0);
                    }
                    j0 = state->sparsec.ridx.ptr.p_int[i];
                    j1 = state->sparsec.ridx.ptr.p_int[i+1]-1;
                    for(j=j0; j<=j1; j++)
                    {
                        state->ecleic.ptr.pp_double[curecpos][state->sparsec.idx.ptr.p_int[j]] = state->sparsec.vals.ptr.p_double[j];
                    }
                }
                else
                {
                    for(j=0; j<=n-1; j++)
                    {
                        state->ecleic.ptr.pp_double[curecpos][j] = state->densec.ptr.pp_double[i-state->msparse][j];
                    }
                }
                state->ecleic.ptr.pp_double[curecpos][n] = state->cu.ptr.p_double[i];
                state->elagidx.ptr.p_int[curecpos] = i;
                state->elagmlt.ptr.p_double[curecpos] = 1.0;
                inc(&curecpos, _state);
                continue;
            }
            if( ae_isfinite(state->cl.ptr.p_double[i], _state) )
            {
                
                /*
                 * Offload inequality constraint of the form CL<=C*x, convert it to -C*x<=-CL
                 */
                if( i<state->msparse )
                {
                    for(j=0; j<=n-1; j++)
                    {
                        state->ecleic.ptr.pp_double[curicpos][j] = (double)(0);
                    }
                    j0 = state->sparsec.ridx.ptr.p_int[i];
                    j1 = state->sparsec.ridx.ptr.p_int[i+1]-1;
                    for(j=j0; j<=j1; j++)
                    {
                        state->ecleic.ptr.pp_double[curicpos][state->sparsec.idx.ptr.p_int[j]] = -state->sparsec.vals.ptr.p_double[j];
                    }
                }
                else
                {
                    for(j=0; j<=n-1; j++)
                    {
                        state->ecleic.ptr.pp_double[curicpos][j] = -state->densec.ptr.pp_double[i-state->msparse][j];
                    }
                }
                state->ecleic.ptr.pp_double[curicpos][n] = -state->cl.ptr.p_double[i];
                state->elagidx.ptr.p_int[curicpos] = i;
                state->elagmlt.ptr.p_double[curicpos] = -1.0;
                inc(&curicpos, _state);
            }
            if( ae_isfinite(state->cu.ptr.p_double[i], _state) )
            {
                
                /*
                 * Offload inequality constraint of the form C*x<=CU
                 */
                if( i<state->msparse )
                {
                    for(j=0; j<=n-1; j++)
                    {
                        state->ecleic.ptr.pp_double[curicpos][j] = (double)(0);
                    }
                    j0 = state->sparsec.ridx.ptr.p_int[i];
                    j1 = state->sparsec.ridx.ptr.p_int[i+1]-1;
                    for(j=j0; j<=j1; j++)
                    {
                        state->ecleic.ptr.pp_double[curicpos][state->sparsec.idx.ptr.p_int[j]] = state->sparsec.vals.ptr.p_double[j];
                    }
                }
                else
                {
                    for(j=0; j<=n-1; j++)
                    {
                        state->ecleic.ptr.pp_double[curicpos][j] = state->densec.ptr.pp_double[i-state->msparse][j];
                    }
                }
                state->ecleic.ptr.pp_double[curicpos][n] = state->cu.ptr.p_double[i];
                state->elagidx.ptr.p_int[curicpos] = i;
                state->elagmlt.ptr.p_double[curicpos] = 1.0;
                inc(&curicpos, _state);
            }
        }
        ae_assert(curecpos==neq&&curicpos==neq+nineq, "MinQPOptimize: critical integrity check failed (ECLEIC conversion)", _state);
        
        /*
         * Run solvers
         */
        if( state->algokind==2 )
        {
            qpbleicoptimize(&state->a, &state->sparsea, state->akind, state->sparseaupper, state->absasum, state->absasum2, &state->b, &state->bndl, &state->bndu, &state->effectives, &state->xorigin, n, &state->ecleic, neq, nineq, &state->qpbleicsettingsuser, &state->qpbleicbuf, &state->qpbleicfirstcall, &state->xs, &state->repterminationtype, _state);
            state->repinneriterationscount = state->qpbleicbuf.repinneriterationscount;
            state->repouteriterationscount = state->qpbleicbuf.repouteriterationscount;
            return;
        }
        if( state->algokind==4 )
        {
            qpdenseauloptimize(&state->a, &state->sparsea, state->akind, state->sparseaupper, &state->b, &state->bndl, &state->bndu, &state->effectives, &state->xorigin, n, &state->ecleic, neq, nineq, &state->dummysparse, 0, 0, !state->dbgskipconstraintnormalization, &state->qpdenseaulsettingsuser, &state->qpdenseaulbuf, &state->xs, &state->replagbc, &state->elaglc, &state->repterminationtype, _state);
            for(i=0; i<=neq+nineq-1; i++)
            {
                state->replaglc.ptr.p_double[state->elagidx.ptr.p_int[i]] = state->replaglc.ptr.p_double[state->elagidx.ptr.p_int[i]]+state->elaglc.ptr.p_double[i]*state->elagmlt.ptr.p_double[i];
            }
            state->repinneriterationscount = state->qpdenseaulbuf.repinneriterationscount;
            state->repouteriterationscount = state->qpdenseaulbuf.repouteriterationscount;
            state->repncholesky = state->qpdenseaulbuf.repncholesky;
            return;
        }
        ae_assert(ae_false, "MinQPOptimize: integrity check failed - unknown solver", _state);
    }
    
    /*
     * QuickQP solver
     */
    if( state->algokind==3 )
    {
        if( state->mdense+state->msparse>0 )
        {
            state->repterminationtype = -5;
            return;
        }
        qqpoptimize(&state->a, &state->sparsea, &state->dummyr2, state->akind, state->sparseaupper, &state->b, &state->bndl, &state->bndu, &state->effectives, &state->xorigin, n, &state->qqpsettingsuser, &state->qqpbuf, &state->xs, &state->repterminationtype, _state);
        state->repinneriterationscount = state->qqpbuf.repinneriterationscount;
        state->repouteriterationscount = state->qqpbuf.repouteriterationscount;
        state->repncholesky = state->qqpbuf.repncholesky;
        return;
    }
    
    /*
     * QP-DENSE-IPM and QP-SPARSE-IPM solvers
     */
    if( state->algokind==5||state->algokind==6 )
    {
        
        /*
         * Prepare working versions of constraints; these versions may be modified
         * when we detect that some bounds are irrelevant.
         */
        rcopyallocv(n, &state->bndl, &state->wrkbndl, _state);
        rcopyallocv(n, &state->bndu, &state->wrkbndu, _state);
        if( state->msparse>0 )
        {
            sparsecopybuf(&state->sparsec, &state->wrksparsec, _state);
        }
        if( state->mdense>0 )
        {
            rcopyallocm(state->mdense, n, &state->densec, &state->wrkdensec, _state);
        }
        rcopyallocv(m, &state->cl, &state->wrkcl, _state);
        rcopyallocv(m, &state->cu, &state->wrkcu, _state);
        
        /*
         * Solve
         */
        ae_assert(state->akind==0||state->akind==1, "MinQPOptimize: unexpected AKind", _state);
        if( state->akind==0 )
        {
            cqmgeta(&state->a, &state->tmpr2, _state);
        }
        if( state->algokind==5 )
        {
            vipminitdense(&state->vsolver, &state->effectives, &state->xorigin, n, _state);
            vipmsetquadraticlinear(&state->vsolver, &state->tmpr2, &state->sparsea, state->akind, state->sparseaupper, &state->b, _state);
            vipmsetconstraints(&state->vsolver, &state->wrkbndl, &state->wrkbndu, &state->wrksparsec, state->msparse, &state->wrkdensec, state->mdense, &state->wrkcl, &state->wrkcu, _state);
            vipmsetcond(&state->vsolver, state->veps, state->veps, state->veps, _state);
            vipmoptimize(&state->vsolver, ae_true, &state->xs, &state->replagbc, &state->replaglc, &state->repterminationtype, _state);
            state->repinneriterationscount = state->vsolver.repiterationscount;
            state->repouteriterationscount = state->repinneriterationscount;
            state->repncholesky = state->vsolver.repncholesky;
        }
        else
        {
            ipm2init(&state->ipm2, &state->effectives, &state->xorigin, n, &state->tmpr2, &state->sparsea, state->akind, state->sparseaupper, &state->dummyr2, &state->dummyr, 0, &state->b, &state->wrkbndl, &state->wrkbndu, &state->wrksparsec, state->msparse, &state->wrkdensec, state->mdense, &state->wrkcl, &state->wrkcu, _state);
            ipm2setcond(&state->ipm2, state->veps, state->veps, state->veps, _state);
            ipm2optimize(&state->ipm2, ae_true, &state->xs, &state->replagbc, &state->replaglc, &state->repterminationtype, _state);
            state->repinneriterationscount = state->ipm2.repiterationscount;
            state->repouteriterationscount = state->ipm2.repiterationscount;
            state->repncholesky = state->ipm2.repiterationscount;
        }
        return;
    }
    
    /*
     * Integrity check failed - unknown solver
     */
    ae_assert(ae_false, "MinQPOptimize: integrity check failed - unknown solver", _state);
}


/*************************************************************************
QP solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution (on failure - the best point found
                so far).
    Rep     -   optimization report, contains:
                * completion code in Rep.TerminationType (positive  values
                  denote some kind of success, negative - failures)
                * Lagrange multipliers - for QP solvers which support them
                * other statistics
                See comments on minqpreport structure for more information
                
Following completion codes are returned in Rep.TerminationType:
* -9    failure of the automatic scale evaluation:  one  of  the  diagonal
        elements of the quadratic term is non-positive.  Specify  variable
        scales manually!
* -5    inappropriate solver was used:
        * QuickQP solver for problem with general linear constraints
* -4    the function is unbounded from below even under constraints,
        no meaningful minimum can be found.
* -3    inconsistent constraints (or, maybe, feasible point is too hard to
        find).
* -2    IPM solver has difficulty finding primal/dual feasible point.
        It is likely that the problem is either infeasible or unbounded,
        but it is difficult to determine exact reason for termination.
        X contains best point found so far.
*  >0   success
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresults(const minqpstate* state,
     /* Real    */ ae_vector* x,
     minqpreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _minqpreport_clear(rep);

    minqpresultsbuf(state, x, rep, _state);
}


/*************************************************************************
QP results

Buffered implementation of MinQPResults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresultsbuf(const minqpstate* state,
     /* Real    */ ae_vector* x,
     minqpreport* rep,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(state->xs.cnt>=state->n, "MinQPResultsBuf: integrity check failed", _state);
    ae_assert(state->replagbc.cnt>=state->n, "MinQPResultsBuf: integrity check failed", _state);
    ae_assert(state->replaglc.cnt>=state->mdense+state->msparse, "MinQPResultsBuf: integrity check failed", _state);
    rvectorsetlengthatleast(x, state->n, _state);
    rvectorsetlengthatleast(&rep->lagbc, state->n, _state);
    rvectorsetlengthatleast(&rep->laglc, state->mdense+state->msparse, _state);
    for(i=0; i<=state->n-1; i++)
    {
        x->ptr.p_double[i] = state->xs.ptr.p_double[i];
        rep->lagbc.ptr.p_double[i] = state->replagbc.ptr.p_double[i];
    }
    for(i=0; i<=state->mdense+state->msparse-1; i++)
    {
        rep->laglc.ptr.p_double[i] = state->replaglc.ptr.p_double[i];
    }
    rep->inneriterationscount = state->repinneriterationscount;
    rep->outeriterationscount = state->repouteriterationscount;
    rep->nmv = state->repnmv;
    rep->ncholesky = state->repncholesky;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
Fast version of MinQPSetLinearTerm(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetlineartermfast(minqpstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{


    ae_v_move(&state->b.ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
}


/*************************************************************************
Fast version of MinQPSetQuadraticTerm(), which doesn't check its arguments.

It accepts additional parameter - shift S, which allows to "shift"  matrix
A by adding s*I to A. S must be positive (although it is not checked).

For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadratictermfast(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     double s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    double v;
    ae_int_t j0;
    ae_int_t j1;


    n = state->n;
    state->akind = 0;
    cqmseta(&state->a, a, isupper, 1.0, _state);
    if( ae_fp_greater(s,(double)(0)) )
    {
        rvectorsetlengthatleast(&state->tmp0, n, _state);
        for(i=0; i<=n-1; i++)
        {
            state->tmp0.ptr.p_double[i] = a->ptr.pp_double[i][i]+s;
        }
        cqmrewritedensediagonal(&state->a, &state->tmp0, _state);
    }
    
    /*
     * Estimate norm of A
     * (it will be used later in the quadratic penalty function)
     */
    state->absamax = (double)(0);
    state->absasum = (double)(0);
    state->absasum2 = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j0 = i;
            j1 = n-1;
        }
        else
        {
            j0 = 0;
            j1 = i;
        }
        for(j=j0; j<=j1; j++)
        {
            v = ae_fabs(a->ptr.pp_double[i][j], _state);
            state->absamax = ae_maxreal(state->absamax, v, _state);
            state->absasum = state->absasum+v;
            state->absasum2 = state->absasum2+v*v;
        }
    }
}


/*************************************************************************
Internal function which allows to rewrite diagonal of quadratic term.
For internal use only.

This function can be used only when you have dense A and already made
MinQPSetQuadraticTerm(Fast) call.

  -- ALGLIB --
     Copyright 16.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqprewritediagonal(minqpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{


    cqmrewritedensediagonal(&state->a, s, _state);
}


/*************************************************************************
Fast version of MinQPSetStartingPoint(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetstartingpointfast(minqpstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_v_move(&state->startx.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->havex = ae_true;
}


/*************************************************************************
Fast version of MinQPSetOrigin(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetoriginfast(minqpstate* state,
     /* Real    */ const ae_vector* xorigin,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_v_move(&state->xorigin.ptr.p_double[0], 1, &xorigin->ptr.p_double[0], 1, ae_v_len(0,n-1));
}


void _minqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minqpstate *p = (minqpstate*)_p;
    ae_touch_ptr((void*)p);
    _qqpsettings_init(&p->qqpsettingsuser, _state, make_automatic);
    _qpbleicsettings_init(&p->qpbleicsettingsuser, _state, make_automatic);
    _qpdenseaulsettings_init(&p->qpdenseaulsettingsuser, _state, make_automatic);
    _convexquadraticmodel_init(&p->a, _state, make_automatic);
    _sparsematrix_init(&p->sparsea, _state, make_automatic);
    ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->havebndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->havebndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->xorigin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->startx, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->densec, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsec, _state, make_automatic);
    ae_vector_init(&p->cl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replagbc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replaglc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->effectives, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->ecleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->elaglc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->elagmlt, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->elagidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->dummyr, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->dummyr2, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->dummysparse, _state, make_automatic);
    ae_matrix_init(&p->tmpr2, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkcl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkcu, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->wrkdensec, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->wrksparsec, _state, make_automatic);
    _qpbleicbuffers_init(&p->qpbleicbuf, _state, make_automatic);
    _qqpbuffers_init(&p->qqpbuf, _state, make_automatic);
    _qpdenseaulbuffers_init(&p->qpdenseaulbuf, _state, make_automatic);
    _vipmstate_init(&p->vsolver, _state, make_automatic);
    _ipm2state_init(&p->ipm2, _state, make_automatic);
}


void _minqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minqpstate       *dst = (minqpstate*)_dst;
    const minqpstate *src = (const minqpstate*)_src;
    dst->n = src->n;
    _qqpsettings_init_copy(&dst->qqpsettingsuser, &src->qqpsettingsuser, _state, make_automatic);
    _qpbleicsettings_init_copy(&dst->qpbleicsettingsuser, &src->qpbleicsettingsuser, _state, make_automatic);
    _qpdenseaulsettings_init_copy(&dst->qpdenseaulsettingsuser, &src->qpdenseaulsettingsuser, _state, make_automatic);
    dst->veps = src->veps;
    dst->dbgskipconstraintnormalization = src->dbgskipconstraintnormalization;
    dst->algokind = src->algokind;
    dst->akind = src->akind;
    _convexquadraticmodel_init_copy(&dst->a, &src->a, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsea, &src->sparsea, _state, make_automatic);
    dst->sparseaupper = src->sparseaupper;
    dst->absamax = src->absamax;
    dst->absasum = src->absasum;
    dst->absasum2 = src->absasum2;
    ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    dst->stype = src->stype;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->havebndl, &src->havebndl, _state, make_automatic);
    ae_vector_init_copy(&dst->havebndu, &src->havebndu, _state, make_automatic);
    ae_vector_init_copy(&dst->xorigin, &src->xorigin, _state, make_automatic);
    ae_vector_init_copy(&dst->startx, &src->startx, _state, make_automatic);
    dst->havex = src->havex;
    ae_matrix_init_copy(&dst->densec, &src->densec, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsec, &src->sparsec, _state, make_automatic);
    ae_vector_init_copy(&dst->cl, &src->cl, _state, make_automatic);
    ae_vector_init_copy(&dst->cu, &src->cu, _state, make_automatic);
    dst->mdense = src->mdense;
    dst->msparse = src->msparse;
    ae_vector_init_copy(&dst->xs, &src->xs, _state, make_automatic);
    dst->repinneriterationscount = src->repinneriterationscount;
    dst->repouteriterationscount = src->repouteriterationscount;
    dst->repncholesky = src->repncholesky;
    dst->repnmv = src->repnmv;
    dst->repterminationtype = src->repterminationtype;
    ae_vector_init_copy(&dst->replagbc, &src->replagbc, _state, make_automatic);
    ae_vector_init_copy(&dst->replaglc, &src->replaglc, _state, make_automatic);
    ae_vector_init_copy(&dst->effectives, &src->effectives, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_matrix_init_copy(&dst->ecleic, &src->ecleic, _state, make_automatic);
    ae_vector_init_copy(&dst->elaglc, &src->elaglc, _state, make_automatic);
    ae_vector_init_copy(&dst->elagmlt, &src->elagmlt, _state, make_automatic);
    ae_vector_init_copy(&dst->elagidx, &src->elagidx, _state, make_automatic);
    ae_vector_init_copy(&dst->dummyr, &src->dummyr, _state, make_automatic);
    ae_matrix_init_copy(&dst->dummyr2, &src->dummyr2, _state, make_automatic);
    _sparsematrix_init_copy(&dst->dummysparse, &src->dummysparse, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpr2, &src->tmpr2, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkbndl, &src->wrkbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkbndu, &src->wrkbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkcl, &src->wrkcl, _state, make_automatic);
    ae_vector_init_copy(&dst->wrkcu, &src->wrkcu, _state, make_automatic);
    ae_matrix_init_copy(&dst->wrkdensec, &src->wrkdensec, _state, make_automatic);
    _sparsematrix_init_copy(&dst->wrksparsec, &src->wrksparsec, _state, make_automatic);
    dst->qpbleicfirstcall = src->qpbleicfirstcall;
    _qpbleicbuffers_init_copy(&dst->qpbleicbuf, &src->qpbleicbuf, _state, make_automatic);
    _qqpbuffers_init_copy(&dst->qqpbuf, &src->qqpbuf, _state, make_automatic);
    _qpdenseaulbuffers_init_copy(&dst->qpdenseaulbuf, &src->qpdenseaulbuf, _state, make_automatic);
    _vipmstate_init_copy(&dst->vsolver, &src->vsolver, _state, make_automatic);
    _ipm2state_init_copy(&dst->ipm2, &src->ipm2, _state, make_automatic);
}


void _minqpstate_clear(void* _p)
{
    minqpstate *p = (minqpstate*)_p;
    ae_touch_ptr((void*)p);
    _qqpsettings_clear(&p->qqpsettingsuser);
    _qpbleicsettings_clear(&p->qpbleicsettingsuser);
    _qpdenseaulsettings_clear(&p->qpdenseaulsettingsuser);
    _convexquadraticmodel_clear(&p->a);
    _sparsematrix_clear(&p->sparsea);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->havebndl);
    ae_vector_clear(&p->havebndu);
    ae_vector_clear(&p->xorigin);
    ae_vector_clear(&p->startx);
    ae_matrix_clear(&p->densec);
    _sparsematrix_clear(&p->sparsec);
    ae_vector_clear(&p->cl);
    ae_vector_clear(&p->cu);
    ae_vector_clear(&p->xs);
    ae_vector_clear(&p->replagbc);
    ae_vector_clear(&p->replaglc);
    ae_vector_clear(&p->effectives);
    ae_vector_clear(&p->tmp0);
    ae_matrix_clear(&p->ecleic);
    ae_vector_clear(&p->elaglc);
    ae_vector_clear(&p->elagmlt);
    ae_vector_clear(&p->elagidx);
    ae_vector_clear(&p->dummyr);
    ae_matrix_clear(&p->dummyr2);
    _sparsematrix_clear(&p->dummysparse);
    ae_matrix_clear(&p->tmpr2);
    ae_vector_clear(&p->wrkbndl);
    ae_vector_clear(&p->wrkbndu);
    ae_vector_clear(&p->wrkcl);
    ae_vector_clear(&p->wrkcu);
    ae_matrix_clear(&p->wrkdensec);
    _sparsematrix_clear(&p->wrksparsec);
    _qpbleicbuffers_clear(&p->qpbleicbuf);
    _qqpbuffers_clear(&p->qqpbuf);
    _qpdenseaulbuffers_clear(&p->qpdenseaulbuf);
    _vipmstate_clear(&p->vsolver);
    _ipm2state_clear(&p->ipm2);
}


void _minqpstate_destroy(void* _p)
{
    minqpstate *p = (minqpstate*)_p;
    ae_touch_ptr((void*)p);
    _qqpsettings_destroy(&p->qqpsettingsuser);
    _qpbleicsettings_destroy(&p->qpbleicsettingsuser);
    _qpdenseaulsettings_destroy(&p->qpdenseaulsettingsuser);
    _convexquadraticmodel_destroy(&p->a);
    _sparsematrix_destroy(&p->sparsea);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->havebndl);
    ae_vector_destroy(&p->havebndu);
    ae_vector_destroy(&p->xorigin);
    ae_vector_destroy(&p->startx);
    ae_matrix_destroy(&p->densec);
    _sparsematrix_destroy(&p->sparsec);
    ae_vector_destroy(&p->cl);
    ae_vector_destroy(&p->cu);
    ae_vector_destroy(&p->xs);
    ae_vector_destroy(&p->replagbc);
    ae_vector_destroy(&p->replaglc);
    ae_vector_destroy(&p->effectives);
    ae_vector_destroy(&p->tmp0);
    ae_matrix_destroy(&p->ecleic);
    ae_vector_destroy(&p->elaglc);
    ae_vector_destroy(&p->elagmlt);
    ae_vector_destroy(&p->elagidx);
    ae_vector_destroy(&p->dummyr);
    ae_matrix_destroy(&p->dummyr2);
    _sparsematrix_destroy(&p->dummysparse);
    ae_matrix_destroy(&p->tmpr2);
    ae_vector_destroy(&p->wrkbndl);
    ae_vector_destroy(&p->wrkbndu);
    ae_vector_destroy(&p->wrkcl);
    ae_vector_destroy(&p->wrkcu);
    ae_matrix_destroy(&p->wrkdensec);
    _sparsematrix_destroy(&p->wrksparsec);
    _qpbleicbuffers_destroy(&p->qpbleicbuf);
    _qqpbuffers_destroy(&p->qqpbuf);
    _qpdenseaulbuffers_destroy(&p->qpdenseaulbuf);
    _vipmstate_destroy(&p->vsolver);
    _ipm2state_destroy(&p->ipm2);
}


void _minqpreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minqpreport *p = (minqpreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->lagbc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->laglc, 0, DT_REAL, _state, make_automatic);
}


void _minqpreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minqpreport       *dst = (minqpreport*)_dst;
    const minqpreport *src = (const minqpreport*)_src;
    dst->inneriterationscount = src->inneriterationscount;
    dst->outeriterationscount = src->outeriterationscount;
    dst->nmv = src->nmv;
    dst->ncholesky = src->ncholesky;
    dst->terminationtype = src->terminationtype;
    ae_vector_init_copy(&dst->lagbc, &src->lagbc, _state, make_automatic);
    ae_vector_init_copy(&dst->laglc, &src->laglc, _state, make_automatic);
}


void _minqpreport_clear(void* _p)
{
    minqpreport *p = (minqpreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->lagbc);
    ae_vector_clear(&p->laglc);
}


void _minqpreport_destroy(void* _p)
{
    minqpreport *p = (minqpreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->lagbc);
    ae_vector_destroy(&p->laglc);
}


/*$ End $*/
