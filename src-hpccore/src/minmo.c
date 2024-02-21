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
#include "minmo.h"


/*$ Declarations $*/
static void minmo_clearrequestfields(minmostate* state, ae_state *_state);
static void minmo_minmoinitinternal(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minmostate* state,
     ae_state *_state);
static void minmo_unscale(const minmostate* state,
     /* Real    */ const ae_vector* xs,
     /* Real    */ const ae_vector* scaledbndl,
     /* Real    */ const ae_vector* scaledbndu,
     /* Real    */ ae_vector* xu,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
                    MULTI-OBJECTIVE  OPTIMIZATION

DESCRIPTION:

The  solver  minimizes an M-dimensional vector function F(x) of N arguments
subject to any combination of:
* box constraints
* two-sided linear equality/inequality constraints AL<=A*x<=AU, where some
  of AL/AU can be infinite (i.e. missing)
* two-sided nonlinear equality/inequality constraints NL<=C(x)<=NU,  where
  some of NL/NU can be infinite (i.e. missing)

REQUIREMENTS:
* F(), C() are continuously differentiable on the feasible set and on  its
  neighborhood

USAGE:

1. User initializes algorithm state using either:
   * minmocreate()  to perform optimization with user-supplied Jacobian
   * minmocreatef() to perform optimization with numerical differentiation

2. User chooses which multi-objective solver to use. At the present moment
   only NBI (Normal Boundary Intersection) solver is implemented, which is
   activated by calling minmosetalgonbi().

3. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) minmosetbc() for boundary constraints
   b) minmosetlc2()      for two-sided sparse linear constraints;
      minmosetlc2dense() for two-sided dense  linear constraints;
      minmosetlc2mixed() for two-sided mixed sparse/dense constraints
   c) minmosetnlc2()     for two-sided nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.
   
4. User sets scale of the variables with minmosetscale() function.  It  is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.

5. User sets  stopping  conditions  with  minmosetcond().
   
6. Finally, user calls minmooptimize()   function  which  takes  algorithm
   state  and  pointers  (delegate, etc.) to the callback functions  which
   calculate F/C

7. User calls  minmoresults()  to  get the solution

8. Optionally user may call minmorestartfrom() to solve  another   problem
   with same M,N but another starting point. minmorestartfrom() allows  to
   reuse an already initialized optimizer structure.


INPUT PARAMETERS:
    N       -   variables count, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from the size of X
    M       -   objectives count, M>0.
                M=1 is possible, although makes little sense - it is better
                to use MinNLC directly.
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to reinforce feasibility during  initial  stages  of  the
                  optimization

OUTPUT PARAMETERS:
    State   -   structure that stores algorithm state

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmocreate(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minmostate* state,
     ae_state *_state)
{

    _minmostate_clear(state);

    ae_assert(n>=1, "MinMOCreate: N<1", _state);
    ae_assert(m>=1, "MinMOCreate: M<1", _state);
    ae_assert(x->cnt>=n, "MinMOCreate: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinMOCreate: X contains infinite or NaN values", _state);
    minmo_minmoinitinternal(n, m, x, 0.0, state, _state);
}


/*************************************************************************
This subroutine is a finite  difference variant of minmocreate().  It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this  function
only. We recommend to read comments on minmocreate() too.

INPUT PARAMETERS:
    N       -   variables count, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from the size of X
    M       -   objectives count, M>0.
                M=1 is possible, although makes little sense - it is better
                to use MinNLC directly.
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to reinforce feasibility during  initial  stages  of  the
                  optimization
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure that stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is a scaling vector which can be set by minmosetscale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step means too large TRUNCATION errors,  whilst  too  small  step
   means too large NUMERICAL errors.
   1.0E-4 can be good value to start from for a unit-scaled problem.
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
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmocreatef(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minmostate* state,
     ae_state *_state)
{

    _minmostate_clear(state);

    ae_assert(n>=1, "MinMOCreateF: N<1", _state);
    ae_assert(m>=1, "MinMOCreateF: M<1", _state);
    ae_assert(x->cnt>=n, "MinMOCreateF: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinMOCreateF: X contains infinite or NaN values", _state);
    ae_assert(ae_isfinite(diffstep, _state), "MinMOCreateF: DiffStep is infinite or NaN!", _state);
    ae_assert(ae_fp_greater(diffstep,(double)(0)), "MinMOCreateF: DiffStep is non-positive!", _state);
    minmo_minmoinitinternal(n, m, x, diffstep, state, _state);
}


/*************************************************************************
Use the NBI (Normal Boundary Intersection)  algorithm  for  multiobjective
optimization.

NBI is a simple yet powerful multiobjective  optimization  algorithm  that
has the following attractive properties:
* it generates nearly uniformly distributed Pareto points
* it is applicable to problems with more than 2 objectives
* it naturally supports a mix of box, linear and nonlinear constraints
* it is less sensitive to the bad scaling of the targets

The only drawback of the algorithm is that for more than 2  objectives  it
can miss some small parts of the Pareto front that are  located  near  its
boundaries.

INPUT PARAMETERS:
    State       -   structure which stores algorithm state
    FrontSize   -   desired Pareto front size, FrontSize>=M,
                    where M is an objectives count
    PolishSolutions-whether additional solution improving phase is needed
                    or not:
                    * if False, the original NBI as formulated  by Das and
                      Dennis is used. It quickly produces  good solutions,
                      but these solutions can be suboptimal (usually within
                      0.1% of the optimal values).
                      The reason is that the original NBI formulation does
                      not account for  degeneracies that allow significant
                      progress for one objective with no deterioration for
                      other objectives.
                    * if True,  the  original  NBI  is  followed  by   the
                      additional solution  polishing  phase.  This  solver
                      mode is several times slower than the original  NBI,
                      but produces better solutions.

  -- ALGLIB --
     Copyright 20.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetalgonbi(minmostate* state,
     ae_int_t frontsize,
     ae_bool polishsolutions,
     ae_state *_state)
{


    ae_assert(frontsize>=state->m, "MinMOSetAlgoNBI: FrontSize<=M", _state);
    state->solvertype = icase2(polishsolutions, 1, 0, _state);
    state->frontsize = frontsize;
}


/*************************************************************************
This function sets boundary constraints for the MO optimizer.

Boundary constraints are inactive by  default  (after  initial  creation).
They are preserved after algorithm restart with MinMORestartFrom().

You may combine boundary constraints with  general  linear ones - and with
nonlinear ones! Boundary constraints are  handled  more  efficiently  than
other types.  Thus,  if  your  problem  has  mixed  constraints,  you  may
explicitly specify some of them as boundary and save some time/space.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                a very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                a very large number or +INF.

NOTE 1:  it is possible to specify  BndL[i]=BndU[i].  In  this  case  I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetbc(minmostate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "MinMOSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinMOSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinMOSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinMOSetBC: BndL contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

NOTE: knowing  that  constraint  matrix  is dense may help some MO solvers
      to utilize efficient dense Level 3  BLAS  for  dense  parts  of  the
      problem. If your problem has both dense and sparse constraints,  you
      can use minmosetlc2mixed() function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetlc2dense(minmostate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state)
{


    minmosetlc2mixed(state, &state->dummysparse, 0, a, k, al, au, _state);
}


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
void minmosetlc2(minmostate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state)
{


    minmosetlc2mixed(state, a, k, &state->dummyr2, 0, al, au, _state);
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
Similarly, if you have just a few dense rows,  you  can  store them in the
sparse format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
void minmosetlc2mixed(minmostate* state,
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
    ae_assert(ksparse>=0, "MinMOSetLC2Mixed: KSparse<0", _state);
    ae_assert(ksparse==0||sparsegetncols(sparsea, _state)==n, "MinMOSetLC2: Cols(SparseA)<>N", _state);
    ae_assert(ksparse==0||sparsegetnrows(sparsea, _state)==ksparse, "MinMOSetLC2: Rows(SparseA)<>K", _state);
    ae_assert(kdense>=0, "MinMOSetLC2Mixed: KDense<0", _state);
    ae_assert(kdense==0||densea->cols>=n, "MinMOSetLC2Mixed: Cols(DenseA)<N", _state);
    ae_assert(kdense==0||densea->rows>=kdense, "MinMOSetLC2Mixed: Rows(DenseA)<K", _state);
    ae_assert(apservisfinitematrix(densea, kdense, n, _state), "MinMOSetLC2Mixed: DenseA contains infinite or NaN values!", _state);
    ae_assert(al->cnt>=kdense+ksparse, "MinMOSetLC2Mixed: Length(AL)<K", _state);
    ae_assert(au->cnt>=kdense+ksparse, "MinMOSetLC2Mixed: Length(AU)<K", _state);
    for(i=0; i<=m-1; i++)
    {
        ae_assert(ae_isfinite(al->ptr.p_double[i], _state)||ae_isneginf(al->ptr.p_double[i], _state), "MinMOSetLC2Mixed: AL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(au->ptr.p_double[i], _state)||ae_isposinf(au->ptr.p_double[i], _state), "MinMOSetLC2Mixed: AU contains NAN or -INF", _state);
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
This function appends two-sided linear  constraint  AL<=A*x<=AU  to  dense
constraints list.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
void minmoaddlc2dense(minmostate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(a->cnt>=n, "MinMOAddLC2Dense: Length(A)<N", _state);
    ae_assert(isfinitevector(a, n, _state), "MinMOAddLC2Dense: A contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(al, _state)||ae_isneginf(al, _state), "MinMOAddLC2Dense: AL is NAN or +INF", _state);
    ae_assert(ae_isfinite(au, _state)||ae_isposinf(au, _state), "MinMOAddLC2Dense: AU is NAN or -INF", _state);
    rvectorgrowto(&state->cl, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->cu, state->msparse+state->mdense+1, _state);
    rmatrixgrowrowsto(&state->densec, state->mdense+1, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->densec.ptr.pp_double[state->mdense][i] = a->ptr.p_double[i];
    }
    state->cl.ptr.p_double[state->msparse+state->mdense] = al;
    state->cu.ptr.p_double[state->msparse+state->mdense] = au;
    inc(&state->mdense, _state);
}


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of sparse constraints.

Constraint is passed in the compressed  format:  as  a  list  of  non-zero
entries of the coefficient vector A. Such approach is more efficient  than
the dense storage for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
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
void minmoaddlc2(minmostate* state,
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
    ae_assert(nnz>=0, "MinMOAddLC2: NNZ<0", _state);
    ae_assert(idxa->cnt>=nnz, "MinMOAddLC2: Length(IdxA)<NNZ", _state);
    ae_assert(vala->cnt>=nnz, "MinMOAddLC2: Length(ValA)<NNZ", _state);
    for(i=0; i<=nnz-1; i++)
    {
        ae_assert(idxa->ptr.p_int[i]>=0&&idxa->ptr.p_int[i]<n, "MinMOAddLC2: IdxA contains indexes outside of [0,N) range", _state);
    }
    ae_assert(isfinitevector(vala, nnz, _state), "MinMOAddLC2: ValA contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(al, _state)||ae_isneginf(al, _state), "MinMOAddLC2: AL is NAN or +INF", _state);
    ae_assert(ae_isfinite(au, _state)||ae_isposinf(au, _state), "MinMOAddLC2: AU is NAN or -INF", _state);
    
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
    ae_assert(state->sparsec.matrixtype==1&&state->sparsec.m==state->msparse, "MinMOAddLC2: integrity check failed!", _state);
    
    /*
     * Reallocate inequality bounds
     */
    rvectorgrowto(&state->cl, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->cu, state->msparse+state->mdense+1, _state);
    for(i=state->msparse+state->mdense; i>=state->msparse+1; i--)
    {
        state->cl.ptr.p_double[i] = state->cl.ptr.p_double[i-1];
        state->cu.ptr.p_double[i] = state->cu.ptr.p_double[i-1];
    }
    state->cl.ptr.p_double[state->msparse] = al;
    state->cu.ptr.p_double[state->msparse] = au;
    
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
    State   -   structure previously allocated with minmocreate() call.
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
void minmoaddlc2sparsefromdense(minmostate* state,
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
    ae_assert(da->cnt>=n, "MinMOAddLC2SparseFromDense: Length(DA)<N", _state);
    ae_assert(isfinitevector(da, n, _state), "MinMOAddLC2SparseFromDense: DA contains infinities/NANs", _state);
    ae_assert(ae_isfinite(al, _state)||ae_isneginf(al, _state), "MinMOAddLC2SparseFromDense: AL is NAN or +INF", _state);
    ae_assert(ae_isfinite(au, _state)||ae_isposinf(au, _state), "MinMOAddLC2SparseFromDense: AU is NAN or -INF", _state);
    
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
    ae_assert(state->sparsec.matrixtype==1&&state->sparsec.m==state->msparse, "MinMOAddLC2SparseFromDense: integrity check failed!", _state);
    
    /*
     * Reallocate inequality bounds
     */
    rvectorgrowto(&state->cl, state->msparse+state->mdense+1, _state);
    rvectorgrowto(&state->cu, state->msparse+state->mdense+1, _state);
    for(i=state->msparse+state->mdense; i>=state->msparse+1; i--)
    {
        state->cl.ptr.p_double[i] = state->cl.ptr.p_double[i-1];
        state->cu.ptr.p_double[i] = state->cu.ptr.p_double[i-1];
    }
    state->cl.ptr.p_double[state->msparse] = al;
    state->cu.ptr.p_double[state->msparse] = au;
    
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
This function sets two-sided nonlinear constraints for MinMO optimizer.

In fact, this function sets  only  constraints  COUNT  and  their  BOUNDS.
Constraints  themselves  (constraint  functions)   are   passed   to   the
MinMOOptimize() method as callbacks.

MinMOOptimize() method accepts a user-defined vector function F[]  and its
Jacobian J[], where:
* first M components of F[]  and  first  M rows  of  J[]   correspond   to
  multiple objectives
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
    State   -   structure previously allocated with MinMOCreate call.
    NL      -   array[NNLC], lower bounds, can contain -INF
    NU      -   array[NNLC], lower bounds, can contain +INF
    NNLC    -   constraints count, NNLC>=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible that the algorithm will evaluate the function  outside of
        the feasible area!
        
NOTE 2: algorithm scales variables  according  to the scale  specified by
        MinMOSetScale()  function,  so  it can handle problems with badly
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
        a scale set by MinMOSetScale() function).

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetnlc2(minmostate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(nnlc>=0, "MinMOSetNLC2: NNLC<0", _state);
    ae_assert(nl->cnt>=nnlc, "MinMOSetNLC2: Length(NL)<NNLC", _state);
    ae_assert(nu->cnt>=nnlc, "MinMOSetNLC2: Length(NU)<NNLC", _state);
    state->nnlc = nnlc;
    ae_vector_set_length(&state->fi, state->m+nnlc, _state);
    ae_matrix_set_length(&state->j, state->m+nnlc, state->n, _state);
    rallocv(nnlc, &state->nl, _state);
    rallocv(nnlc, &state->nu, _state);
    for(i=0; i<=nnlc-1; i++)
    {
        ae_assert(ae_isfinite(nl->ptr.p_double[i], _state)||ae_isneginf(nl->ptr.p_double[i], _state), "MinMOSetNLC2: NL[i] is +INF or NAN", _state);
        ae_assert(ae_isfinite(nu->ptr.p_double[i], _state)||ae_isposinf(nu->ptr.p_double[i], _state), "MinMOSetNLC2: NU[i] is -INF or NAN", _state);
        state->nl.ptr.p_double[i] = nl->ptr.p_double[i];
        state->nu.ptr.p_double[i] = nu->ptr.p_double[i];
    }
}


/*************************************************************************
This function sets stopping conditions for inner iterations of the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinMOSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsX=0 and MaxIts=0 (simultaneously) will lead to an automatic
selection of the stopping condition.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetcond(minmostate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsx, _state), "MinMOSetCond: EpsX is not finite number", _state);
    ae_assert(ae_fp_greater_eq(epsx,(double)(0)), "MinMOSetCond: negative EpsX", _state);
    ae_assert(maxits>=0, "MinMOSetCond: negative MaxIts!", _state);
    if( ae_fp_eq(epsx,(double)(0))&&maxits==0 )
    {
        epsx = 1.0E-6;
    }
    state->epsx = epsx;
    state->maxits = maxits;
}


/*************************************************************************
This function sets scaling coefficients for the MO optimizer.

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
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetscale(minmostate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinMOSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinMOSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "MinMOSetScale: S contains zero elements", _state);
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function turns on/off reporting of the Pareto front points.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback  function  (if  it
was provided to MinMOOptimize) every time we find a Pareto front point.

NOTE: according to the communication protocol used by ALGLIB,  the  solver
      passes two parameters to the rep() callback - a current point and  a
      target value at the current point.
      However, because  we solve a  multi-objective  problem,  the  target
      parameter is not used and set to zero.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetxrep(minmostate* state, ae_bool needxrep, ae_state *_state)
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
   you should choose appropriate variant of MinMOOptimize() -  one   which
   needs function vector AND Jacobian or one which needs ONLY function.

   Be careful to choose variant of MinMOOptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinMOOptimize()   and   specific
   function used to create optimizer.


                     |         USER PASSED TO MinMOOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinMOCreateF()    |     works               FAILS
   MinMOCreate()     |     FAILS               works

   Here "FAILS" denotes inappropriate combinations  of  optimizer creation
   function  and  MinMOOptimize()  version.   Attemps   to    use     such
   combination will lead to exception. Either  you  did  not pass gradient
   when it WAS needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool minmoiteration(minmostate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t nnlc;
    ae_int_t i;
    ae_int_t k;
    double vleft;
    double vright;
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
        nnlc = state->rstate.ia.ptr.p_int[2];
        i = state->rstate.ia.ptr.p_int[3];
        k = state->rstate.ia.ptr.p_int[4];
        vleft = state->rstate.ra.ptr.p_double[0];
        vright = state->rstate.ra.ptr.p_double[1];
    }
    else
    {
        n = 359;
        m = -58;
        nnlc = -919;
        i = -909;
        k = 81;
        vleft = 255.0;
        vright = 74.0;
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
    
    /*
     * Routine body
     */
    n = state->n;
    m = state->m;
    nnlc = state->nnlc;
    
    /*
     * Initialize
     */
    state->repterminationtype = 0;
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    state->repnfev = 0;
    state->repbcerr = (double)(0);
    state->repbcidx = -1;
    state->replcerr = (double)(0);
    state->replcidx = -1;
    state->repnlcerr = (double)(0);
    state->repnlcidx = -1;
    state->repfrontsize = 0;
    minmo_clearrequestfields(state, _state);
    
    /*
     * Check correctness of constraint bounds
     */
    for(i=0; i<=n-1; i++)
    {
        if( (ae_isfinite(state->bndl.ptr.p_double[i], _state)&&ae_isfinite(state->bndu.ptr.p_double[i], _state))&&ae_fp_greater(state->bndl.ptr.p_double[i]-state->bndu.ptr.p_double[i],state->repbcerr) )
        {
            state->repterminationtype = -3;
            state->repbcerr = state->bndl.ptr.p_double[i]-state->bndu.ptr.p_double[i];
            state->repbcidx = i;
        }
    }
    for(i=0; i<=state->mdense+state->msparse-1; i++)
    {
        if( (ae_isfinite(state->cl.ptr.p_double[i], _state)&&ae_isfinite(state->cu.ptr.p_double[i], _state))&&ae_fp_greater(state->cl.ptr.p_double[i]-state->cu.ptr.p_double[i],state->replcerr) )
        {
            state->repterminationtype = -3;
            state->replcerr = state->cl.ptr.p_double[i]-state->cu.ptr.p_double[i];
            state->replcidx = i;
        }
    }
    for(i=0; i<=state->nnlc-1; i++)
    {
        if( (ae_isfinite(state->nl.ptr.p_double[i], _state)&&ae_isfinite(state->nu.ptr.p_double[i], _state))&&ae_fp_greater(state->nl.ptr.p_double[i]-state->nu.ptr.p_double[i],state->repnlcerr) )
        {
            state->repterminationtype = -3;
            state->repnlcerr = state->nl.ptr.p_double[i]-state->nu.ptr.p_double[i];
            state->repnlcidx = i;
        }
    }
    if( state->repterminationtype<0 )
    {
        result = ae_false;
        return result;
    }
    
    /*
     * NBI solver
     */
    if( !(state->solvertype==0||state->solvertype==1) )
    {
        goto lbl_9;
    }
    
    /*
     * Prepare NBI solver
     */
    nbiscaleandinitbuf(&state->xstart, &state->s, n, m, state->frontsize, &state->bndl, &state->bndu, &state->sparsec, &state->densec, &state->cl, &state->cu, state->msparse, state->mdense, &state->nl, &state->nu, state->nnlc, state->epsx, state->maxits, state->solvertype==1, &state->nbi, _state);
    
    /*
     * Perform iterations
     */
lbl_11:
    if( !nbiiteration(&state->nbi, _state) )
    {
        goto lbl_12;
    }
    
    /*
     * Forward request to caller
     */
    if( !state->nbi.needfij )
    {
        goto lbl_13;
    }
    
    /*
     * Evaluate target function/Jacobian
     */
    if( ae_fp_neq(state->diffstep,(double)(0)) )
    {
        goto lbl_15;
    }
    
    /*
     * Analytic Jacobian is provided.
     *
     * Unscale point and forward request. We use scaled NBI.BndL/NBI.U to make sure
     * that points exactly at scaled bounds are mapped to points exactly at raw bounds.
     */
    minmo_unscale(state, &state->nbi.x, &state->nbi.bndl, &state->nbi.bndu, &state->x, _state);
    state->needfij = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    for(i=0; i<=m+nnlc-1; i++)
    {
        state->nbi.fi.ptr.p_double[i] = state->fi.ptr.p_double[i];
        rcopyrr(n, &state->j, i, &state->nbi.j, i, _state);
        rmergemulvr(n, &state->s, &state->nbi.j, i, _state);
    }
    goto lbl_16;
lbl_15:
    
    /*
     * Numerical differentiation
     */
    rallocv(n, &state->xbase, _state);
    rallocv(m+nnlc, &state->fm1, _state);
    rallocv(m+nnlc, &state->fp1, _state);
    rallocv(m+nnlc, &state->fm2, _state);
    rallocv(m+nnlc, &state->fp2, _state);
    minmo_unscale(state, &state->nbi.x, &state->nbi.bndl, &state->nbi.bndu, &state->xbase, _state);
    k = 0;
lbl_17:
    if( k>n-1 )
    {
        goto lbl_19;
    }
    vleft = state->xbase.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    vright = state->xbase.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    if( !((state->hasbndl.ptr.p_bool[k]&&ae_fp_less(vleft,state->bndl.ptr.p_double[k]))||(state->hasbndu.ptr.p_bool[k]&&ae_fp_greater(vright,state->bndu.ptr.p_double[k]))) )
    {
        goto lbl_20;
    }
    
    /*
     * Box constraint is violated by the 4-point centered formula, use 2-point uncentered one
     */
    if( state->hasbndl.ptr.p_bool[k]&&ae_fp_less(vleft,state->bndl.ptr.p_double[k]) )
    {
        vleft = state->bndl.ptr.p_double[k];
    }
    if( state->hasbndu.ptr.p_bool[k]&&ae_fp_greater(vright,state->bndu.ptr.p_double[k]) )
    {
        vright = state->bndu.ptr.p_double[k];
    }
    ae_assert(ae_fp_less_eq(vleft,vright), "MinMO: integrity check 3445 failed", _state);
    if( ae_fp_eq(vleft,vright) )
    {
        
        /*
         * Fixed variable
         */
        rsetc(m+nnlc, 0.0, &state->j, k, _state);
        goto lbl_18;
    }
    
    /*
     * Compute target at VLeft and VRight
     */
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = vleft;
    state->needfi = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfi = ae_false;
    rcopyv(m+nnlc, &state->fi, &state->fm1, _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = vright;
    state->needfi = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfi = ae_false;
    rcopyv(m+nnlc, &state->fi, &state->fp1, _state);
    
    /*
     * Compute derivative
     */
    raddv(m+nnlc, -1.0, &state->fm1, &state->fp1, _state);
    rmulv(m+nnlc, (double)1/(vright-vleft), &state->fp1, _state);
    rcopyvc(m+nnlc, &state->fp1, &state->j, k, _state);
    goto lbl_21;
lbl_20:
    
    /*
     * 4-point centered formula does not violate box constraints.
     * Compute target values at grid points.
     */
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-state->s.ptr.p_double[k]*state->diffstep;
    state->needfi = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needfi = ae_false;
    rcopyv(m+nnlc, &state->fi, &state->fm2, _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]-0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->needfi = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needfi = ae_false;
    rcopyv(m+nnlc, &state->fi, &state->fm1, _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+0.5*state->s.ptr.p_double[k]*state->diffstep;
    state->needfi = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->needfi = ae_false;
    rcopyv(m+nnlc, &state->fi, &state->fp1, _state);
    rcopyv(n, &state->xbase, &state->x, _state);
    state->x.ptr.p_double[k] = state->x.ptr.p_double[k]+state->s.ptr.p_double[k]*state->diffstep;
    state->needfi = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->needfi = ae_false;
    rcopyv(m+nnlc, &state->fi, &state->fp2, _state);
    
    /*
     * Compute derivative
     */
    raddv(m+nnlc, -1.0, &state->fm1, &state->fp1, _state);
    raddv(m+nnlc, -1.0, &state->fm2, &state->fp2, _state);
    rmulv(m+nnlc, 8.0, &state->fp1, _state);
    raddv(m+nnlc, -1.0, &state->fp2, &state->fp1, _state);
    rmulv(m+nnlc, (double)1/((double)6*state->diffstep*state->s.ptr.p_double[k]), &state->fp1, _state);
    rcopyvc(m+nnlc, &state->fp1, &state->j, k, _state);
lbl_21:
lbl_18:
    k = k+1;
    goto lbl_17;
lbl_19:
    rcopyv(n, &state->xbase, &state->x, _state);
    state->needfi = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->needfi = ae_false;
    rcopyv(m+nnlc, &state->fi, &state->nbi.fi, _state);
    rcopym(m+nnlc, n, &state->j, &state->nbi.j, _state);
lbl_16:
    inc(&state->repnfev, _state);
    goto lbl_11;
lbl_13:
    if( !state->nbi.xupdated )
    {
        goto lbl_22;
    }
    
    /*
     * Report current point
     */
    if( !state->xrep )
    {
        goto lbl_24;
    }
    minmo_unscale(state, &state->nbi.x, &state->nbi.bndl, &state->nbi.bndu, &state->x, _state);
    state->f = 0.0;
    state->xupdated = ae_true;
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->xupdated = ae_false;
lbl_24:
    goto lbl_11;
lbl_22:
    ae_assert(ae_false, "MINMO: unexpected callback request", _state);
    goto lbl_11;
lbl_12:
    
    /*
     * Done
     */
    state->repfrontsize = state->nbi.repfrontsize;
    rcopyallocm(state->repfrontsize, n+m, &state->nbi.repparetofront, &state->repparetofront, _state);
    state->repinneriterationscount = state->nbi.repinneriterationscount;
    state->repouteriterationscount = state->nbi.repouteriterationscount;
    state->repterminationtype = state->nbi.repterminationtype;
    state->repbcerr = state->nbi.repbcerr;
    state->repbcidx = state->nbi.repbcidx;
    state->replcerr = state->nbi.replcerr;
    state->replcidx = state->nbi.replcidx;
    state->repnlcerr = state->nbi.repnlcerr;
    state->repnlcidx = state->nbi.repnlcidx;
    result = ae_false;
    return result;
lbl_9:
    
    /*
     * Unexpected solver
     */
    ae_assert(ae_false, "MINBO: unexpected solver type", _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = nnlc;
    state->rstate.ia.ptr.p_int[3] = i;
    state->rstate.ia.ptr.p_int[4] = k;
    state->rstate.ra.ptr.p_double[0] = vleft;
    state->rstate.ra.ptr.p_double[1] = vright;
    return result;
}


/*************************************************************************
MinMO results:  the  solution  found,  completion  codes  and   additional
information.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    ParetoFront-array[FrontSize,N+M], approximate Pareto front.
                Its columns have the following structure:
                * first N columns are variable values
                * next  M columns are objectives at these points
                Its rows have the following structure:
                * first M rows contain solutions to single-objective tasks
                  with I-th row storing result for  I-th  objective  being
                  minimized ignoring other ones.
                  Thus, ParetoFront[I,N+I] for  0<=I<M  stores  so  called
                  'ideal objective vector'.
                * subsequent FrontSize-M rows  store  variables/objectives
                  at  various  randomly  and  nearly   uniformly   sampled
                  locations of the Pareto front.
                  
    FrontSize-  front size, >=0.
                * no larger than the number passed to setalgo()
                * for  a  single-objective  task,  FrontSize=1  is  ALWAYS
                  returned, no matter what was specified during setalgo()
                  call.
                * if  the   solver   was   prematurely   terminated   with
                  minnorequesttermination(), an  incomplete  Pareto  front
                  will be returned (it may even have less than M rows)
                * if a  failure (negative completion code) was   signaled,
                  FrontSize=0 will be returned
                  
    Rep     -   optimization report, contains information about completion
                code, constraint violation at the solution and so on.
                
                You   should   check   rep.terminationtype  in  order   to
                distinguish successful termination from unsuccessful one:
                
                === FAILURE CODES ===
                * -8    internal  integrity control  detected  infinite or
                        NAN   values    in   function/gradient.   Abnormal
                        termination signalled.
                * -3    constraint bounds are  infeasible,  i.e.  we  have
                        box/linear/nonlinear constraint  with  two  bounds
                        present, and a lower one being  greater  than  the
                        upper one.
                        Note: less obvious infeasibilities of  constraints
                              do NOT  trigger  emergency  completion;  you
                              have to examine rep.bcerr/rep.lcerr/rep.nlcerr
                              to detect possibly inconsistent constraints.
                              
                === SUCCESS CODES ===
                *  2   scaled step is no more than EpsX.
                *  5   MaxIts steps were taken.
                *  8   user   requested    algorithm    termination    via
                       minmorequesttermination(), last accepted point   is
                       returned.
                
                More information about fields of this  structure  can  be
                found in the comments on minmoreport datatype.
   
  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmoresults(const minmostate* state,
     /* Real    */ ae_matrix* paretofront,
     ae_int_t* frontsize,
     minmoreport* rep,
     ae_state *_state)
{
    ae_int_t i;

    ae_matrix_clear(paretofront);
    *frontsize = 0;
    _minmoreport_clear(rep);

    rep->inneriterationscount = state->repinneriterationscount;
    rep->outeriterationscount = state->repouteriterationscount;
    rep->nfev = state->repnfev;
    rep->terminationtype = state->repterminationtype;
    rep->bcerr = state->repbcerr;
    rep->bcidx = state->repbcidx;
    rep->lcerr = state->replcerr;
    rep->lcidx = state->replcidx;
    rep->nlcerr = state->repnlcerr;
    rep->nlcidx = state->repnlcidx;
    if( rep->terminationtype>0 )
    {
        *frontsize = state->repfrontsize;
        ae_matrix_set_length(paretofront, *frontsize, state->n+state->m, _state);
        rcopym(*frontsize, state->n+state->m, &state->repparetofront, paretofront, _state);
        for(i=0; i<=*frontsize-1; i++)
        {
            rmergemulvr(state->n, &state->s, paretofront, i, _state);
        }
    }
    else
    {
        *frontsize = 0;
        ae_matrix_set_length(paretofront, 0, 0, _state);
    }
}


/*************************************************************************
This subroutine  submits  request  for  the  termination  of  the  running
optimizer.

It should be called from the user-supplied callback when user decides that
it is time to "smoothly" terminate optimization process, or from some other
thread. As a result, optimizer stops  at  the  state  which  was  "current
accepted" when termination request was submitted and returns error code  8
(successful termination).

Usually it results in an incomplete Pareto front being returned.

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
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmorequesttermination(minmostate* state, ae_state *_state)
{


    state->nbi.userrequestedtermination = ae_true;
}


/*************************************************************************
This subroutine restarts algorithm from the new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinMOCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmorestartfrom(minmostate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    
    /*
     * First, check for errors in the inputs
     */
    ae_assert(x->cnt>=n, "MinMORestartFrom: Length(X)<N", _state);
    ae_assert(isfinitevector(x, n, _state), "MinMORestartFrom: X contains infinite or NaN values!", _state);
    
    /*
     * Set XC
     */
    ae_v_move(&state->xstart.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,n-1));
    
    /*
     * prepare RComm facilities
     */
    ae_vector_set_length(&state->rstate.ia, 4+1, _state);
    ae_vector_set_length(&state->rstate.ra, 1+1, _state);
    state->rstate.stage = -1;
    minmo_clearrequestfields(state, _state);
}


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void minmosetprotocolv1(minmostate* state, ae_state *_state)
{


    state->protocolversion = 1;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forget to clear something)
*************************************************************************/
static void minmo_clearrequestfields(minmostate* state, ae_state *_state)
{


    state->needfi = ae_false;
    state->needfij = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Internal initialization subroutine.
Sets default NLC solver with default criteria.
*************************************************************************/
static void minmo_minmoinitinternal(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minmostate* state,
     ae_state *_state)
{


    
    /*
     * Initialize
     */
    state->protocolversion = 1;
    state->n = n;
    state->m = m;
    state->diffstep = diffstep;
    rsetallocv(n, _state->v_neginf, &state->bndl, _state);
    rsetallocv(n, _state->v_posinf, &state->bndu, _state);
    bsetallocv(n, ae_false, &state->hasbndl, _state);
    bsetallocv(n, ae_false, &state->hasbndu, _state);
    rsetallocv(n, 1.0, &state->s, _state);
    rcopyallocv(n, x, &state->xstart, _state);
    minmosetlc2dense(state, &state->dummyr2, &state->dummyr1, &state->dummyr1, 0, _state);
    minmosetnlc2(state, &state->dummyr1, &state->dummyr1, 0, _state);
    minmosetcond(state, 0.0, 0, _state);
    minmosetxrep(state, ae_false, _state);
    minmosetalgonbi(state, 10, ae_true, _state);
    minmorestartfrom(state, x, _state);
    
    /*
     * Prepare RComm.X
     */
    rallocv(n, &state->x, _state);
}


/*************************************************************************
Unscales X (converts from scaled variables to original ones), paying special
attention to box constraints (output is always feasible; active constraints
are mapped to active ones).
*************************************************************************/
static void minmo_unscale(const minmostate* state,
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


void _minmostate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minmostate *p = (minmostate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xstart, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->densec, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsec, _state, make_automatic);
    ae_vector_init(&p->cl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    _nbistate_init(&p->nbi, _state, make_automatic);
    ae_matrix_init(&p->repparetofront, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummyr1, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->dummyr2, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->dummysparse, _state, make_automatic);
    ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fm2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fm1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fp2, 0, DT_REAL, _state, make_automatic);
}


void _minmostate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minmostate       *dst = (minmostate*)_dst;
    const minmostate *src = (const minmostate*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->diffstep = src->diffstep;
    dst->solvertype = src->solvertype;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    dst->xrep = src->xrep;
    ae_vector_init_copy(&dst->xstart, &src->xstart, _state, make_automatic);
    dst->frontsize = src->frontsize;
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    dst->msparse = src->msparse;
    dst->mdense = src->mdense;
    ae_matrix_init_copy(&dst->densec, &src->densec, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsec, &src->sparsec, _state, make_automatic);
    ae_vector_init_copy(&dst->cl, &src->cl, _state, make_automatic);
    ae_vector_init_copy(&dst->cu, &src->cu, _state, make_automatic);
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
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    _nbistate_init_copy(&dst->nbi, &src->nbi, _state, make_automatic);
    dst->repfrontsize = src->repfrontsize;
    ae_matrix_init_copy(&dst->repparetofront, &src->repparetofront, _state, make_automatic);
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
    ae_vector_init_copy(&dst->dummyr1, &src->dummyr1, _state, make_automatic);
    ae_matrix_init_copy(&dst->dummyr2, &src->dummyr2, _state, make_automatic);
    _sparsematrix_init_copy(&dst->dummysparse, &src->dummysparse, _state, make_automatic);
    ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic);
    ae_vector_init_copy(&dst->fm2, &src->fm2, _state, make_automatic);
    ae_vector_init_copy(&dst->fm1, &src->fm1, _state, make_automatic);
    ae_vector_init_copy(&dst->fp1, &src->fp1, _state, make_automatic);
    ae_vector_init_copy(&dst->fp2, &src->fp2, _state, make_automatic);
}


void _minmostate_clear(void* _p)
{
    minmostate *p = (minmostate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->xstart);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_matrix_clear(&p->densec);
    _sparsematrix_clear(&p->sparsec);
    ae_vector_clear(&p->cl);
    ae_vector_clear(&p->cu);
    ae_vector_clear(&p->nl);
    ae_vector_clear(&p->nu);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    _rcommstate_clear(&p->rstate);
    _nbistate_clear(&p->nbi);
    ae_matrix_clear(&p->repparetofront);
    ae_vector_clear(&p->dummyr1);
    ae_matrix_clear(&p->dummyr2);
    _sparsematrix_clear(&p->dummysparse);
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->fm2);
    ae_vector_clear(&p->fm1);
    ae_vector_clear(&p->fp1);
    ae_vector_clear(&p->fp2);
}


void _minmostate_destroy(void* _p)
{
    minmostate *p = (minmostate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->xstart);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_matrix_destroy(&p->densec);
    _sparsematrix_destroy(&p->sparsec);
    ae_vector_destroy(&p->cl);
    ae_vector_destroy(&p->cu);
    ae_vector_destroy(&p->nl);
    ae_vector_destroy(&p->nu);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    _rcommstate_destroy(&p->rstate);
    _nbistate_destroy(&p->nbi);
    ae_matrix_destroy(&p->repparetofront);
    ae_vector_destroy(&p->dummyr1);
    ae_matrix_destroy(&p->dummyr2);
    _sparsematrix_destroy(&p->dummysparse);
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->fm2);
    ae_vector_destroy(&p->fm1);
    ae_vector_destroy(&p->fp1);
    ae_vector_destroy(&p->fp2);
}


void _minmoreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minmoreport *p = (minmoreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minmoreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minmoreport       *dst = (minmoreport*)_dst;
    const minmoreport *src = (const minmoreport*)_src;
    dst->inneriterationscount = src->inneriterationscount;
    dst->outeriterationscount = src->outeriterationscount;
    dst->nfev = src->nfev;
    dst->terminationtype = src->terminationtype;
    dst->bcerr = src->bcerr;
    dst->bcidx = src->bcidx;
    dst->lcerr = src->lcerr;
    dst->lcidx = src->lcidx;
    dst->nlcerr = src->nlcerr;
    dst->nlcidx = src->nlcidx;
}


void _minmoreport_clear(void* _p)
{
    minmoreport *p = (minmoreport*)_p;
    ae_touch_ptr((void*)p);
}


void _minmoreport_destroy(void* _p)
{
    minmoreport *p = (minmoreport*)_p;
    ae_touch_ptr((void*)p);
}


/*$ End $*/
