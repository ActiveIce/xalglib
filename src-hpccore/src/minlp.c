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
#include "minlp.h"


/*$ Declarations $*/
static ae_int_t minlp_alllogicalsbasis = 0;
static void minlp_clearreportfields(minlpstate* state, ae_state *_state);


/*$ Body $*/


/*************************************************************************
                            LINEAR PROGRAMMING

The subroutine creates LP  solver.  After  initial  creation  it  contains
default optimization problem with zero cost vector and all variables being
fixed to zero values and no constraints.

In order to actually solve something you should:
* set cost vector with minlpsetcost()
* set variable bounds with minlpsetbc() or minlpsetbcall()
* specify constraint matrix with one of the following functions:
  [*] minlpsetlc()        for dense one-sided constraints
  [*] minlpsetlc2dense()  for dense two-sided constraints
  [*] minlpsetlc2()       for sparse two-sided constraints
  [*] minlpaddlc2dense()  to add one dense row to constraint matrix
  [*] minlpaddlc2()       to add one row to constraint matrix (compressed format)
* call minlpoptimize() to run the solver and  minlpresults()  to  get  the
  solution vector and additional information.
  
By  default,  LP  solver uses best algorithm available. As of ALGLIB 3.17,
sparse interior point (barrier) solver is used. Future releases of  ALGLIB
may introduce other solvers.

User may choose specific LP algorithm by calling:
* minlpsetalgodss() for revised dual simplex method with DSE  pricing  and
  bounds flipping ratio test (aka long dual step).  Large-scale  sparse LU
  solverwith  Forest-Tomlin update is used internally  as  linear  algebra
  driver.
* minlpsetalgoipm() for sparse interior point method

INPUT PARAMETERS:
    N       -   problem size
    
OUTPUT PARAMETERS:
    State   -   optimizer in the default state

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpcreate(ae_int_t n, minlpstate* state, ae_state *_state)
{
    ae_int_t i;

    _minlpstate_clear(state);

    ae_assert(n>=1, "MinLPCreate: N<1", _state);
    
    /*
     * Initialize
     */
    state->n = n;
    state->m = 0;
    minlpsetalgoipm(state, 0.0, _state);
    state->ipmlambda = (double)(0);
    ae_vector_set_length(&state->c, n, _state);
    ae_vector_set_length(&state->s, n, _state);
    ae_vector_set_length(&state->bndl, n, _state);
    ae_vector_set_length(&state->bndu, n, _state);
    ae_vector_set_length(&state->xs, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = (double)(0);
        state->bndu.ptr.p_double[i] = (double)(0);
        state->c.ptr.p_double[i] = 0.0;
        state->s.ptr.p_double[i] = 1.0;
        state->xs.ptr.p_double[i] = 1.0;
    }
    minlp_clearreportfields(state, _state);
}


/*************************************************************************
This function sets LP algorithm to revised dual simplex method.

ALGLIB implementation of dual simplex method supports advanced performance
and stability improvements like DSE pricing , bounds flipping  ratio  test
(aka long dual step), Forest-Tomlin update, shifting.

INPUT PARAMETERS:
    State   -   optimizer
    Eps     -   stopping condition, Eps>=0:
                * should be small number about 1E-6 or 1E-7.
                * zero value means that solver automatically selects good
                  value (can be different in different ALGLIB versions)
                * default value is zero
                Algorithm stops when relative error is less than Eps.

===== TRACING DSS SOLVER =================================================

DSS solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'DSS'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'DSS.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'DSS'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("DSS,PREC.F6", "path/to/trace.log")
> 

  -- ALGLIB --
     Copyright 08.11.2020 by Bochkanov Sergey
*************************************************************************/
void minlpsetalgodss(minlpstate* state, double eps, ae_state *_state)
{


    ae_assert(ae_isfinite(eps, _state), "MinLPSetAlgoDSS: Eps is not finite number", _state);
    ae_assert(ae_fp_greater_eq(eps,(double)(0)), "MinLPSetAlgoDSS: Eps<0", _state);
    state->algokind = 1;
    if( ae_fp_eq(eps,(double)(0)) )
    {
        eps = 1.0E-6;
    }
    state->dsseps = eps;
}


/*************************************************************************
This function sets LP algorithm to sparse interior point method.

ALGORITHM INFORMATION:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality

INPUT PARAMETERS:
    State   -   optimizer
    Eps     -   stopping condition, Eps>=0:
                * should be small number about 1E-6 or 1E-8.
                * zero value means that solver automatically selects good
                  value (can be different in different ALGLIB versions)
                * default value is zero
                Algorithm  stops  when  primal  error  AND  dual error AND
                duality gap are less than Eps.

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
     Copyright 08.11.2020 by Bochkanov Sergey
*************************************************************************/
void minlpsetalgoipm(minlpstate* state, double eps, ae_state *_state)
{


    ae_assert(ae_isfinite(eps, _state), "MinLPSetAlgoIPM: Eps is not finite number", _state);
    ae_assert(ae_fp_greater_eq(eps,(double)(0)), "MinLPSetAlgoIPM: Eps<0", _state);
    state->algokind = 2;
    state->ipmeps = eps;
    state->ipmlambda = 0.0;
}


/*************************************************************************
This function sets cost term for LP solver.

By default, cost term is zero.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    C       -   cost term, array[N].

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetcost(minlpstate* state,
     /* Real    */ const ae_vector* c,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;


    n = state->n;
    ae_assert(c->cnt>=n, "MinLPSetCost: Length(C)<N", _state);
    ae_assert(isfinitevector(c, n, _state), "MinLPSetCost: C contains infinite or NaN elements", _state);
    for(i=0; i<=n-1; i++)
    {
        state->c.ptr.p_double[i] = c->ptr.p_double[i];
    }
}


/*************************************************************************
This function sets scaling coefficients.

ALGLIB optimizers use scaling matrices to test stopping  conditions and as
preconditioner.

Scale of the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the
   function

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetscale(minlpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(s->cnt>=state->n, "MinLPSetScale: Length(S)<N", _state);
    for(i=0; i<=state->n-1; i++)
    {
        ae_assert(ae_isfinite(s->ptr.p_double[i], _state), "MinLPSetScale: S contains infinite or NAN elements", _state);
        ae_assert(ae_fp_neq(s->ptr.p_double[i],(double)(0)), "MinLPSetScale: S contains zero elements", _state);
    }
    for(i=0; i<=state->n-1; i++)
    {
        state->s.ptr.p_double[i] = ae_fabs(s->ptr.p_double[i], _state);
    }
}


/*************************************************************************
This function sets box constraints for LP solver (all variables  at  once,
different constraints for different variables).

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector. Constraint  status
is preserved until constraints are  explicitly  overwritten  with  another
minlpsetbc()  call,   overwritten   with  minlpsetbcall(),  or   partially
overwritten with minlmsetbci() call.

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
    BndU    -   upper bounds, array[N].

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).
      
NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.
      
NOTE: if constraints for all variables are same you may use minlpsetbcall()
      which allows to specify constraints without using arrays.
      
NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbc(minlpstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(bndl->cnt>=n, "MinLPSetBC: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "MinLPSetBC: Length(BndU)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "MinLPSetBC: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "MinLPSetBC: BndU contains NAN or -INF", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
    }
}


/*************************************************************************
This function sets box constraints for LP solver (all variables  at  once,
same constraints for all variables)

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector. Constraint  status
is preserved until constraints are  explicitly  overwritten  with  another
minlpsetbc() call or partially overwritten with minlpsetbcall().

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

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
      
NOTE: minlpsetbc() can  be  used  to  specify  different  constraints  for
      different variables.
      
NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbcall(minlpstate* state,
     double bndl,
     double bndu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(ae_isfinite(bndl, _state)||ae_isneginf(bndl, _state), "MinLPSetBCAll: BndL is NAN or +INF", _state);
    ae_assert(ae_isfinite(bndu, _state)||ae_isposinf(bndu, _state), "MinLPSetBCAll: BndU is NAN or -INF", _state);
    for(i=0; i<=n-1; i++)
    {
        state->bndl.ptr.p_double[i] = bndl;
        state->bndu.ptr.p_double[i] = bndu;
    }
}


/*************************************************************************
This function sets box constraints for I-th variable (other variables are
not modified).

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    I       -   variable index, in [0,N)
    BndL    -   lower bound for I-th variable
    BndU    -   upper bound for I-th variable

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).
      
NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.
      
NOTE: minlpsetbc() can  be  used  to  specify  different  constraints  for
      different variables.
      
NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbci(minlpstate* state,
     ae_int_t i,
     double bndl,
     double bndu,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(i>=0&&i<n, "MinLPSetBCi: I is outside of [0,N)", _state);
    ae_assert(ae_isfinite(bndl, _state)||ae_isneginf(bndl, _state), "MinLPSetBCi: BndL is NAN or +INF", _state);
    ae_assert(ae_isfinite(bndu, _state)||ae_isposinf(bndu, _state), "MinLPSetBCi: BndU is NAN or -INF", _state);
    state->bndl.ptr.p_double[i] = bndl;
    state->bndu.ptr.p_double[i] = bndu;
}


/*************************************************************************
This function sets one-sided linear constraints A*x ~ AU, where "~" can be
a mix of "<=", "=" and ">=".

IMPORTANT: this function is provided here for compatibility with the  rest
           of ALGLIB optimizers which accept constraints  in  format  like
           this one. Many real-life problems feature two-sided constraints
           like a0 <= a*x <= a1. It is really inefficient to add them as a
           pair of one-sided constraints.
           
           Use minlpsetlc2dense(), minlpsetlc2(), minlpaddlc2()  (or   its
           sparse version) wherever possible.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    A       -   linear constraints, array[K,N+1]. Each row of A represents
                one constraint, with first N elements being linear coefficients,
                and last element being right side.
    CT      -   constraint types, array[K]:
                * if CT[i]>0, then I-th constraint is A[i,*]*x >= A[i,n]
                * if CT[i]=0, then I-th constraint is A[i,*]*x  = A[i,n]
                * if CT[i]<0, then I-th constraint is A[i,*]*x <= A[i,n]
    K       -   number of equality/inequality constraints,  K>=0;  if  not
                given, inferred from sizes of A and CT.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc(minlpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector al;
    ae_vector au;
    ae_int_t n;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&al, 0, sizeof(al));
    memset(&au, 0, sizeof(au));
    ae_vector_init(&al, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&au, 0, DT_REAL, _state, ae_true);

    n = state->n;
    ae_assert(k>=0, "MinLPSetLC: K<0", _state);
    ae_assert(k==0||a->cols>=n+1, "MinLPSetLC: Cols(A)<N+1", _state);
    ae_assert(a->rows>=k, "MinLPSetLC: Rows(A)<K", _state);
    ae_assert(ct->cnt>=k, "MinLPSetLC: Length(CT)<K", _state);
    ae_assert(apservisfinitematrix(a, k, n+1, _state), "MinLPSetLC: A contains infinite or NaN values!", _state);
    
    /*
     * Handle zero K
     */
    if( k==0 )
    {
        state->m = 0;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Convert constraints to two-sided storage format, call another function
     */
    ae_vector_set_length(&al, k, _state);
    ae_vector_set_length(&au, k, _state);
    for(i=0; i<=k-1; i++)
    {
        if( ct->ptr.p_int[i]>0 )
        {
            al.ptr.p_double[i] = a->ptr.pp_double[i][n];
            au.ptr.p_double[i] = _state->v_posinf;
            continue;
        }
        if( ct->ptr.p_int[i]<0 )
        {
            al.ptr.p_double[i] = _state->v_neginf;
            au.ptr.p_double[i] = a->ptr.pp_double[i][n];
            continue;
        }
        al.ptr.p_double[i] = a->ptr.pp_double[i][n];
        au.ptr.p_double[i] = a->ptr.pp_double[i][n];
    }
    minlpsetlc2dense(state, a, &al, &au, k, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU.

This version accepts dense matrix as  input;  internally  LP  solver  uses
sparse storage  anyway  (most  LP  problems  are  sparse),  but  for  your
convenience it may accept dense inputs. This  function  overwrites  linear
constraints set by previous calls (if such calls were made).

We recommend you to use sparse version of this function unless  you  solve
small-scale LP problem (less than few hundreds of variables).

NOTE: there also exist several versions of this function:
      * one-sided dense version which  accepts  constraints  in  the  same
        format as one used by QP and  NLP solvers
      * two-sided sparse version which accepts sparse matrix
      * two-sided dense  version which allows you to add constraints row by row
      * two-sided sparse version which allows you to add constraints row by row

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
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
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc2dense(minlpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    ae_int_t nz;
    ae_vector nrs;

    ae_frame_make(_state, &_frame_block);
    memset(&nrs, 0, sizeof(nrs));
    ae_vector_init(&nrs, 0, DT_INT, _state, ae_true);

    n = state->n;
    ae_assert(k>=0, "MinLPSetLC2Dense: K<0", _state);
    ae_assert(k==0||a->cols>=n, "MinLPSetLC2Dense: Cols(A)<N", _state);
    ae_assert(a->rows>=k, "MinLPSetLC2Dense: Rows(A)<K", _state);
    ae_assert(al->cnt>=k, "MinLPSetLC2Dense: Length(AL)<K", _state);
    ae_assert(au->cnt>=k, "MinLPSetLC2Dense: Length(AU)<K", _state);
    ae_assert(apservisfinitematrix(a, k, n, _state), "MinLPSetLC2Dense: A contains infinite or NaN values!", _state);
    
    /*
     * Count actual (different from -INF<=A*x<=+INF) constraints;
     * count non-zero elements in each row.
     */
    ae_vector_set_length(&nrs, k, _state);
    state->m = k;
    if( state->m==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=k-1; i++)
    {
        ae_assert(ae_isfinite(al->ptr.p_double[i], _state)||ae_isneginf(al->ptr.p_double[i], _state), "MinLPSetLC2Dense: AL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(au->ptr.p_double[i], _state)||ae_isposinf(au->ptr.p_double[i], _state), "MinLPSetLC2Dense: AU contains NAN or -INF", _state);
        nz = 0;
        for(j=0; j<=n-1; j++)
        {
            if( ae_fp_neq(a->ptr.pp_double[i][j],(double)(0)) )
            {
                inc(&nz, _state);
            }
        }
        nrs.ptr.p_int[i] = nz;
    }
    
    /*
     * Allocate storage, copy
     */
    rvectorsetlengthatleast(&state->al, state->m, _state);
    rvectorsetlengthatleast(&state->au, state->m, _state);
    sparsecreatecrsbuf(state->m, n, &nrs, &state->a, _state);
    for(i=0; i<=k-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( ae_fp_neq(a->ptr.pp_double[i][j],(double)(0)) )
            {
                sparseset(&state->a, i, j, a->ptr.pp_double[i][j], _state);
            }
        }
        state->al.ptr.p_double[i] = al->ptr.p_double[i];
        state->au.ptr.p_double[i] = au->ptr.p_double[i];
    }
    ae_frame_leave(_state);
}


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
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
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc2(minlpstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;


    n = state->n;
    
    /*
     * Quick exit
     */
    if( k==0 )
    {
        state->m = 0;
        return;
    }
    
    /*
     * Integrity checks
     */
    ae_assert(k>0, "MinLPSetLC2: K<0", _state);
    ae_assert(sparsegetncols(a, _state)==n, "MinLPSetLC2: Cols(A)<>N", _state);
    ae_assert(sparsegetnrows(a, _state)==k, "MinLPSetLC2: Rows(A)<>K", _state);
    ae_assert(al->cnt>=k, "MinLPSetLC2: Length(AL)<K", _state);
    ae_assert(au->cnt>=k, "MinLPSetLC2: Length(AU)<K", _state);
    for(i=0; i<=k-1; i++)
    {
        ae_assert(ae_isfinite(al->ptr.p_double[i], _state)||ae_isneginf(al->ptr.p_double[i], _state), "MinLPSetLC2: AL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(au->ptr.p_double[i], _state)||ae_isposinf(au->ptr.p_double[i], _state), "MinLPSetLC2: AU contains NAN or -INF", _state);
    }
    
    /*
     * Copy
     */
    state->m = k;
    sparsecopytocrsbuf(a, &state->a, _state);
    rvectorsetlengthatleast(&state->al, k, _state);
    rvectorsetlengthatleast(&state->au, k, _state);
    for(i=0; i<=k-1; i++)
    {
        state->al.ptr.p_double[i] = al->ptr.p_double[i];
        state->au.ptr.p_double[i] = au->ptr.p_double[i];
    }
}


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present constraints.

This version accepts dense constraint vector as input, but  sparsifies  it
for internal storage and processing. Thus, time to add one  constraint  in
is O(N) - we have to scan entire array of length N. Sparse version of this
function is order of magnitude faster for  constraints  with  just  a  few
nonzeros per row.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
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
void minlpaddlc2dense(minlpstate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t nnz;


    n = state->n;
    ae_assert(a->cnt>=n, "MinLPAddLC2Dense: Length(A)<N", _state);
    ae_assert(isfinitevector(a, n, _state), "MinLPAddLC2Dense: A contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(al, _state)||ae_isneginf(al, _state), "MinLPAddLC2Dense: AL is NAN or +INF", _state);
    ae_assert(ae_isfinite(au, _state)||ae_isposinf(au, _state), "MinLPAddLC2Dense: AU is NAN or -INF", _state);
    ivectorsetlengthatleast(&state->adddtmpi, n, _state);
    rvectorsetlengthatleast(&state->adddtmpr, n, _state);
    nnz = 0;
    for(i=0; i<=n-1; i++)
    {
        if( a->ptr.p_double[i]!=0.0 )
        {
            state->adddtmpi.ptr.p_int[nnz] = i;
            state->adddtmpr.ptr.p_double[nnz] = a->ptr.p_double[i];
            nnz = nnz+1;
        }
    }
    minlpaddlc2(state, &state->adddtmpi, &state->adddtmpr, nnz, al, au, _state);
}


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present constraints.

Constraint is passed in compressed format: as list of non-zero entries  of
coefficient vector A. Such approach is more efficient than  dense  storage
for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
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
void minlpaddlc2(minlpstate* state,
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
    ae_int_t m;
    ae_int_t n;
    ae_int_t didx;
    ae_int_t uidx;


    m = state->m;
    n = state->n;
    
    /*
     * Check inputs
     */
    ae_assert(nnz>=0, "MinLPAddLC2: NNZ<0", _state);
    ae_assert(idxa->cnt>=nnz, "MinLPAddLC2: Length(IdxA)<NNZ", _state);
    ae_assert(vala->cnt>=nnz, "MinLPAddLC2: Length(ValA)<NNZ", _state);
    for(i=0; i<=nnz-1; i++)
    {
        ae_assert(idxa->ptr.p_int[i]>=0&&idxa->ptr.p_int[i]<n, "MinLPAddLC2: IdxA contains indexes outside of [0,N) range", _state);
    }
    ae_assert(isfinitevector(vala, nnz, _state), "MinLPAddLC2: ValA contains infinite or NaN values!", _state);
    ae_assert(ae_isfinite(al, _state)||ae_isneginf(al, _state), "MinLPAddLC2Dense: AL is NAN or +INF", _state);
    ae_assert(ae_isfinite(au, _state)||ae_isposinf(au, _state), "MinLPAddLC2Dense: AU is NAN or -INF", _state);
    
    /*
     * If M=0, it means that A is uninitialized.
     * Prepare sparse matrix structure
     */
    if( m==0 )
    {
        state->a.matrixtype = 1;
        state->a.m = 0;
        state->a.n = n;
        state->a.ninitialized = 0;
        ivectorsetlengthatleast(&state->a.ridx, 1, _state);
        state->a.ridx.ptr.p_int[0] = 0;
    }
    
    /*
     * Reallocate storage
     */
    offs = state->a.ridx.ptr.p_int[m];
    ivectorgrowto(&state->a.idx, offs+nnz, _state);
    rvectorgrowto(&state->a.vals, offs+nnz, _state);
    ivectorgrowto(&state->a.didx, m+1, _state);
    ivectorgrowto(&state->a.uidx, m+1, _state);
    ivectorgrowto(&state->a.ridx, m+2, _state);
    rvectorgrowto(&state->al, m+1, _state);
    rvectorgrowto(&state->au, m+1, _state);
    
    /*
     * If NNZ=0, perform quick and simple row append. 
     */
    if( nnz==0 )
    {
        state->a.didx.ptr.p_int[m] = state->a.ridx.ptr.p_int[m];
        state->a.uidx.ptr.p_int[m] = state->a.ridx.ptr.p_int[m];
        state->a.ridx.ptr.p_int[m+1] = state->a.ridx.ptr.p_int[m];
        state->al.ptr.p_double[m] = al;
        state->au.ptr.p_double[m] = au;
        state->a.m = m+1;
        state->m = m+1;
        return;
    }
    
    /*
     * Now we are sure that A contains properly initialized sparse
     * matrix (or some appropriate dummy for M=0) and we have NNZ>0
     * (no need to care about degenerate cases).
     *
     * Append rows to A:
     * * append data
     * * sort in place
     * * merge duplicate indexes
     * * compute DIdx and UIdx
     *
     */
    for(i=0; i<=nnz-1; i++)
    {
        state->a.idx.ptr.p_int[offs+i] = idxa->ptr.p_int[i];
        state->a.vals.ptr.p_double[offs+i] = vala->ptr.p_double[i];
    }
    tagsortmiddleir(&state->a.idx, &state->a.vals, offs, nnz, _state);
    offsdst = offs;
    for(i=1; i<=nnz-1; i++)
    {
        if( state->a.idx.ptr.p_int[offsdst]!=state->a.idx.ptr.p_int[offs+i] )
        {
            offsdst = offsdst+1;
            state->a.idx.ptr.p_int[offsdst] = state->a.idx.ptr.p_int[offs+i];
            state->a.vals.ptr.p_double[offsdst] = state->a.vals.ptr.p_double[offs+i];
        }
        else
        {
            state->a.vals.ptr.p_double[offsdst] = state->a.vals.ptr.p_double[offsdst]+state->a.vals.ptr.p_double[offs+i];
        }
    }
    nnz = offsdst-offs+1;
    uidx = -1;
    didx = -1;
    for(j=offs; j<=offsdst; j++)
    {
        k = state->a.idx.ptr.p_int[j];
        if( k==m )
        {
            didx = j;
        }
        else
        {
            if( k>m&&uidx==-1 )
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
    state->a.didx.ptr.p_int[m] = didx;
    state->a.uidx.ptr.p_int[m] = uidx;
    state->a.ridx.ptr.p_int[m+1] = offsdst+1;
    state->a.m = m+1;
    state->a.ninitialized = state->a.ninitialized+nnz;
    state->al.ptr.p_double[m] = al;
    state->au.ptr.p_double[m] = au;
    state->m = m+1;
}


/*************************************************************************
This function solves LP problem.

INPUT PARAMETERS:
    State   -   algorithm state

You should use minlpresults() function to access results  after  calls  to
this function.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey.
*************************************************************************/
void minlpoptimize(minlpstate* state, ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    double v;
    dualsimplexsettings settings;
    ae_vector dummy1;
    ae_matrix dummy;
    dualsimplexbasis dummybasis;
    ae_bool dotracepresolve;

    ae_frame_make(_state, &_frame_block);
    memset(&settings, 0, sizeof(settings));
    memset(&dummy1, 0, sizeof(dummy1));
    memset(&dummy, 0, sizeof(dummy));
    memset(&dummybasis, 0, sizeof(dummybasis));
    _dualsimplexsettings_init(&settings, _state, ae_true);
    ae_vector_init(&dummy1, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&dummy, 0, 0, DT_REAL, _state, ae_true);
    _dualsimplexbasis_init(&dummybasis, _state, ae_true);

    n = state->n;
    m = state->m;
    dotracepresolve = ae_is_trace_enabled("IPM")||ae_is_trace_enabled("DSS");
    minlp_clearreportfields(state, _state);
    
    /*
     * Run presolver
     */
    presolvelp(&state->s, &state->c, &state->bndl, &state->bndu, n, &state->a, &state->al, &state->au, m, dotracepresolve, &state->presolver, _state);
    if( state->presolver.problemstatus==-3||state->presolver.problemstatus==-2 )
    {
        state->repterminationtype = state->presolver.problemstatus;
        state->repn = n;
        state->repm = m;
        rsetallocv(n, 0.0, &state->xs, _state);
        rsetallocv(n, 0.0, &state->lagbc, _state);
        rsetallocv(m, 0.0, &state->laglc, _state);
        isetallocv(n+m, 0, &state->cs, _state);
        state->repf = (double)(0);
        state->repprimalerror = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            if( ae_isfinite(state->bndl.ptr.p_double[i], _state) )
            {
                state->repprimalerror = ae_maxreal(state->repprimalerror, state->bndl.ptr.p_double[i]-(double)0, _state);
            }
            if( ae_isfinite(state->bndu.ptr.p_double[i], _state) )
            {
                state->repprimalerror = ae_maxreal(state->repprimalerror, (double)0-state->bndu.ptr.p_double[i], _state);
            }
        }
        for(i=0; i<=m-1; i++)
        {
            if( ae_isfinite(state->al.ptr.p_double[i], _state) )
            {
                state->repprimalerror = ae_maxreal(state->repprimalerror, state->al.ptr.p_double[i]-(double)0, _state);
            }
            if( ae_isfinite(state->au.ptr.p_double[i], _state) )
            {
                state->repprimalerror = ae_maxreal(state->repprimalerror, (double)0-state->au.ptr.p_double[i], _state);
            }
        }
        state->repdualerror = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            state->repdualerror = ae_maxreal(state->repdualerror, ae_fabs(state->c.ptr.p_double[i], _state), _state);
        }
        state->repslackerror = (double)(0);
        ae_frame_leave(_state);
        return;
    }
    ae_assert(state->presolver.problemstatus==0, "MINLP: integrity check 4432 failed", _state);
    
    /*
     * Call current solver
     */
    if( state->algokind==1||state->algokind==2 )
    {
        
        /*
         * If presolver did NOT remove all variables (NewN>0), call the current solver
         */
        if( state->presolver.newn>0 )
        {
            if( state->algokind==1 )
            {
                
                /*
                 * Dual simplex method with presolve
                 */
                dsssettingsinit(&settings, _state);
                settings.xtolabs = state->dsseps;
                settings.dtolabs = state->dsseps;
                dssinit(state->presolver.newn, &state->dss, _state);
                dsssetproblem(&state->dss, &state->presolver.c, &state->presolver.bndl, &state->presolver.bndu, &dummy, &state->presolver.sparsea, 1, &state->presolver.al, &state->presolver.au, state->presolver.newm, &dummybasis, minlp_alllogicalsbasis, &settings, _state);
                dssoptimize(&state->dss, &settings, _state);
                
                /*
                 * Export results, convert from presolve
                 */
                rcopyallocv(state->presolver.newn, &state->dss.repx, &state->xs, _state);
                rcopyallocv(state->presolver.newn, &state->dss.replagbc, &state->lagbc, _state);
                rcopyallocv(state->presolver.newm, &state->dss.replaglc, &state->laglc, _state);
                icopyallocv(state->presolver.newn+state->presolver.newm, &state->dss.repstats, &state->cs, _state);
                state->repiterationscount = state->dss.repiterationscount;
                state->repterminationtype = state->dss.repterminationtype;
            }
            if( state->algokind==2 )
            {
                
                /*
                 * Interior point method with presolve
                 */
                rsetallocv(state->presolver.newn, 1.0, &state->units, _state);
                rsetallocv(state->presolver.newn, 0.0, &state->zeroorigin, _state);
                sparsecreatesksbandbuf(state->presolver.newn, state->presolver.newn, 0, &state->ipmquadratic, _state);
                for(i=0; i<=state->presolver.newn-1; i++)
                {
                    sparseset(&state->ipmquadratic, i, i, state->ipmlambda, _state);
                }
                sparseconverttocrs(&state->ipmquadratic, _state);
                vipminitsparse(&state->ipm, &state->units, &state->zeroorigin, state->presolver.newn, _state);
                vipmsetquadraticlinear(&state->ipm, &dummy, &state->ipmquadratic, 1, ae_false, &state->presolver.c, _state);
                vipmsetconstraints(&state->ipm, &state->presolver.bndl, &state->presolver.bndu, &state->presolver.sparsea, state->presolver.newm, &dummy, 0, &state->presolver.al, &state->presolver.au, _state);
                vipmsetcond(&state->ipm, state->ipmeps, state->ipmeps, state->ipmeps, _state);
                vipmoptimize(&state->ipm, ae_true, &state->xs, &state->lagbc, &state->laglc, &state->repterminationtype, _state);
                isetallocv(state->presolver.newn+state->presolver.newm, 0, &state->cs, _state);
                state->repiterationscount = state->ipm.repiterationscount;
            }
        }
        else
        {
            
            /*
             * Presolver removed all variables, manually set up XS and Lagrange multipliers
             */
            rsetallocv(state->presolver.newm, 0.0, &state->laglc, _state);
            isetallocv(state->presolver.newn+state->presolver.newm, 0, &state->cs, _state);
            state->repterminationtype = 1;
            state->repiterationscount = 0;
        }
        
        /*
         * Convert back from presolved format
         */
        presolvebwd(&state->presolver, &state->xs, &state->cs, &state->lagbc, &state->laglc, _state);
        state->repn = n;
        state->repm = m;
        
        /*
         * Compute F, primal and dual errors
         */
        state->repf = rdotv(n, &state->xs, &state->c, _state);
        state->repprimalerror = (double)(0);
        state->repdualerror = (double)(0);
        state->repslackerror = (double)(0);
        rcopyallocv(n, &state->c, &state->tmpg, _state);
        if( m>0 )
        {
            sparsemv(&state->a, &state->xs, &state->tmpax, _state);
            sparsegemv(&state->a, 1.0, 1, &state->laglc, 0, 1.0, &state->tmpg, 0, _state);
        }
        raddv(n, 1.0, &state->lagbc, &state->tmpg, _state);
        for(i=0; i<=n-1; i++)
        {
            if( ae_isfinite(state->bndl.ptr.p_double[i], _state) )
            {
                state->repprimalerror = ae_maxreal(state->repprimalerror, state->bndl.ptr.p_double[i]-state->xs.ptr.p_double[i], _state);
                state->repslackerror = ae_maxreal(state->repslackerror, ae_maxreal(state->xs.ptr.p_double[i]-state->bndl.ptr.p_double[i], 0.0, _state)*ae_maxreal(-state->lagbc.ptr.p_double[i], 0.0, _state), _state);
            }
            if( ae_isfinite(state->bndu.ptr.p_double[i], _state) )
            {
                state->repprimalerror = ae_maxreal(state->repprimalerror, state->xs.ptr.p_double[i]-state->bndu.ptr.p_double[i], _state);
                state->repslackerror = ae_maxreal(state->repslackerror, ae_maxreal(state->bndu.ptr.p_double[i]-state->xs.ptr.p_double[i], 0.0, _state)*ae_maxreal(state->lagbc.ptr.p_double[i], 0.0, _state), _state);
            }
            state->repdualerror = ae_maxreal(state->repdualerror, ae_fabs(state->tmpg.ptr.p_double[i], _state), _state);
        }
        for(i=0; i<=m-1; i++)
        {
            v = state->tmpax.ptr.p_double[i];
            if( ae_isfinite(state->al.ptr.p_double[i], _state) )
            {
                state->repprimalerror = ae_maxreal(state->repprimalerror, state->al.ptr.p_double[i]-v, _state);
                state->repslackerror = ae_maxreal(state->repslackerror, ae_maxreal(v-state->al.ptr.p_double[i], 0.0, _state)*ae_maxreal(-state->laglc.ptr.p_double[i], 0.0, _state), _state);
            }
            if( ae_isfinite(state->au.ptr.p_double[i], _state) )
            {
                state->repprimalerror = ae_maxreal(state->repprimalerror, v-state->au.ptr.p_double[i], _state);
                state->repslackerror = ae_maxreal(state->repslackerror, ae_maxreal(state->au.ptr.p_double[i]-v, 0.0, _state)*ae_maxreal(state->laglc.ptr.p_double[i], 0.0, _state), _state);
            }
        }
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Integrity check failed - unknown solver
     */
    ae_assert(ae_false, "MinQPOptimize: integrity check failed - unknown solver", _state);
    ae_frame_leave(_state);
}


/*************************************************************************
LP solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution (on failure: last trial point)
    Rep     -   optimization report. You should check Rep.TerminationType,
                which contains completion code, and you may check  another
                fields which contain another information  about  algorithm
                functioning.
                
                Failure codes returned by algorithm are:
                * -4    LP problem is primal unbounded (dual infeasible)
                * -3    LP problem is primal infeasible (dual unbounded)
                * -2    IPM solver detected that problem is either
                        infeasible or unbounded
                
                Success codes:
                *  1..4 successful completion
                *  5    MaxIts steps was taken

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlpresults(const minlpstate* state,
     /* Real    */ ae_vector* x,
     minlpreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _minlpreport_clear(rep);

    minlpresultsbuf(state, x, rep, _state);
}


/*************************************************************************
LP results

Buffered implementation of MinLPResults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlpresultsbuf(const minlpstate* state,
     /* Real    */ ae_vector* x,
     minlpreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t repn;
    ae_int_t repm;


    repn = state->repn;
    repm = state->repm;
    if( x->cnt<repn )
    {
        ae_vector_set_length(x, repn, _state);
    }
    ae_vector_set_length(&rep->y, repm, _state);
    ae_vector_set_length(&rep->stats, repn+repm, _state);
    rep->f = state->repf;
    rep->primalerror = state->repprimalerror;
    rep->dualerror = state->repdualerror;
    rep->slackerror = state->repslackerror;
    rep->iterationscount = state->repiterationscount;
    rep->terminationtype = state->repterminationtype;
    rcopyallocv(repm, &state->laglc, &rep->laglc, _state);
    rcopyallocv(repn, &state->lagbc, &rep->lagbc, _state);
    for(i=0; i<=repn-1; i++)
    {
        x->ptr.p_double[i] = state->xs.ptr.p_double[i];
    }
    for(i=0; i<=repm-1; i++)
    {
        rep->y.ptr.p_double[i] = -rep->laglc.ptr.p_double[i];
    }
    for(i=0; i<=repn+repm-1; i++)
    {
        rep->stats.ptr.p_int[i] = state->cs.ptr.p_int[i];
    }
}


/*************************************************************************
Clear report fields prior to the optimization.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey.
*************************************************************************/
static void minlp_clearreportfields(minlpstate* state, ae_state *_state)
{


    state->repf = 0.0;
    state->repprimalerror = 0.0;
    state->repdualerror = 0.0;
    state->repiterationscount = 0;
    state->repterminationtype = 0;
    state->repn = 0;
    state->repm = 0;
}


void _minlpstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlpstate *p = (minlpstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->c, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->a, _state, make_automatic);
    ae_vector_init(&p->al, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->au, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagbc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->laglc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cs, 0, DT_INT, _state, make_automatic);
    _dualsimplexstate_init(&p->dss, _state, make_automatic);
    _vipmstate_init(&p->ipm, _state, make_automatic);
    ae_vector_init(&p->adddtmpi, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->adddtmpr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpg, 0, DT_REAL, _state, make_automatic);
    _presolveinfo_init(&p->presolver, _state, make_automatic);
    ae_vector_init(&p->zeroorigin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->units, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->ipmquadratic, _state, make_automatic);
}


void _minlpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlpstate       *dst = (minlpstate*)_dst;
    const minlpstate *src = (const minlpstate*)_src;
    dst->n = src->n;
    dst->algokind = src->algokind;
    dst->ipmlambda = src->ipmlambda;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->c, &src->c, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    dst->m = src->m;
    _sparsematrix_init_copy(&dst->a, &src->a, _state, make_automatic);
    ae_vector_init_copy(&dst->al, &src->al, _state, make_automatic);
    ae_vector_init_copy(&dst->au, &src->au, _state, make_automatic);
    ae_vector_init_copy(&dst->xs, &src->xs, _state, make_automatic);
    ae_vector_init_copy(&dst->lagbc, &src->lagbc, _state, make_automatic);
    ae_vector_init_copy(&dst->laglc, &src->laglc, _state, make_automatic);
    ae_vector_init_copy(&dst->cs, &src->cs, _state, make_automatic);
    dst->repf = src->repf;
    dst->repprimalerror = src->repprimalerror;
    dst->repdualerror = src->repdualerror;
    dst->repslackerror = src->repslackerror;
    dst->repiterationscount = src->repiterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repn = src->repn;
    dst->repm = src->repm;
    dst->dsseps = src->dsseps;
    dst->ipmeps = src->ipmeps;
    _dualsimplexstate_init_copy(&dst->dss, &src->dss, _state, make_automatic);
    _vipmstate_init_copy(&dst->ipm, &src->ipm, _state, make_automatic);
    ae_vector_init_copy(&dst->adddtmpi, &src->adddtmpi, _state, make_automatic);
    ae_vector_init_copy(&dst->adddtmpr, &src->adddtmpr, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpax, &src->tmpax, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpg, &src->tmpg, _state, make_automatic);
    _presolveinfo_init_copy(&dst->presolver, &src->presolver, _state, make_automatic);
    ae_vector_init_copy(&dst->zeroorigin, &src->zeroorigin, _state, make_automatic);
    ae_vector_init_copy(&dst->units, &src->units, _state, make_automatic);
    _sparsematrix_init_copy(&dst->ipmquadratic, &src->ipmquadratic, _state, make_automatic);
}


void _minlpstate_clear(void* _p)
{
    minlpstate *p = (minlpstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->c);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    _sparsematrix_clear(&p->a);
    ae_vector_clear(&p->al);
    ae_vector_clear(&p->au);
    ae_vector_clear(&p->xs);
    ae_vector_clear(&p->lagbc);
    ae_vector_clear(&p->laglc);
    ae_vector_clear(&p->cs);
    _dualsimplexstate_clear(&p->dss);
    _vipmstate_clear(&p->ipm);
    ae_vector_clear(&p->adddtmpi);
    ae_vector_clear(&p->adddtmpr);
    ae_vector_clear(&p->tmpax);
    ae_vector_clear(&p->tmpg);
    _presolveinfo_clear(&p->presolver);
    ae_vector_clear(&p->zeroorigin);
    ae_vector_clear(&p->units);
    _sparsematrix_clear(&p->ipmquadratic);
}


void _minlpstate_destroy(void* _p)
{
    minlpstate *p = (minlpstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->c);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    _sparsematrix_destroy(&p->a);
    ae_vector_destroy(&p->al);
    ae_vector_destroy(&p->au);
    ae_vector_destroy(&p->xs);
    ae_vector_destroy(&p->lagbc);
    ae_vector_destroy(&p->laglc);
    ae_vector_destroy(&p->cs);
    _dualsimplexstate_destroy(&p->dss);
    _vipmstate_destroy(&p->ipm);
    ae_vector_destroy(&p->adddtmpi);
    ae_vector_destroy(&p->adddtmpr);
    ae_vector_destroy(&p->tmpax);
    ae_vector_destroy(&p->tmpg);
    _presolveinfo_destroy(&p->presolver);
    ae_vector_destroy(&p->zeroorigin);
    ae_vector_destroy(&p->units);
    _sparsematrix_destroy(&p->ipmquadratic);
}


void _minlpreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minlpreport *p = (minlpreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->lagbc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->laglc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stats, 0, DT_INT, _state, make_automatic);
}


void _minlpreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minlpreport       *dst = (minlpreport*)_dst;
    const minlpreport *src = (const minlpreport*)_src;
    dst->f = src->f;
    ae_vector_init_copy(&dst->lagbc, &src->lagbc, _state, make_automatic);
    ae_vector_init_copy(&dst->laglc, &src->laglc, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->stats, &src->stats, _state, make_automatic);
    dst->primalerror = src->primalerror;
    dst->dualerror = src->dualerror;
    dst->slackerror = src->slackerror;
    dst->iterationscount = src->iterationscount;
    dst->terminationtype = src->terminationtype;
}


void _minlpreport_clear(void* _p)
{
    minlpreport *p = (minlpreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->lagbc);
    ae_vector_clear(&p->laglc);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->stats);
}


void _minlpreport_destroy(void* _p)
{
    minlpreport *p = (minlpreport*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->lagbc);
    ae_vector_destroy(&p->laglc);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->stats);
}


/*$ End $*/
