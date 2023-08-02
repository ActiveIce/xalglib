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

#ifndef _minlp_h
#define _minlp_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "scodes.h"
#include "ablasmkl.h"
#include "ablasf.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "tsort.h"
#include "sparse.h"
#include "ablas.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "creflections.h"
#include "matgen.h"
#include "rotations.h"
#include "trfac.h"
#include "lpqppresolve.h"
#include "reviseddualsimplex.h"
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "linmin.h"
#include "optguardapi.h"
#include "matinv.h"
#include "optserv.h"
#include "fbls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"
#include "vipmsolver.h"


/*$ Declarations $*/


/*************************************************************************
This object stores linear solver state.
You should use functions provided by MinLP subpackage to work with this
object
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t algokind;
    double ipmlambda;
    ae_vector s;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t m;
    sparsematrix a;
    ae_vector al;
    ae_vector au;
    ae_vector xs;
    ae_vector lagbc;
    ae_vector laglc;
    ae_vector cs;
    double repf;
    double repprimalerror;
    double repdualerror;
    double repslackerror;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repn;
    ae_int_t repm;
    double dsseps;
    double ipmeps;
    dualsimplexstate dss;
    vipmstate ipm;
    ae_vector adddtmpi;
    ae_vector adddtmpr;
    ae_vector tmpax;
    ae_vector tmpg;
    presolveinfo presolver;
    ae_vector zeroorigin;
    ae_vector units;
    sparsematrix ipmquadratic;
} minlpstate;


/*************************************************************************
This structure stores optimization report:
* f                         target function value
* lagbc                     Lagrange coefficients for box constraints
* laglc                     Lagrange coefficients for linear constraints
* y                         dual variables
* stats                     array[N+M], statuses of box (N) and linear (M)
                            constraints. This array is filled only by  DSS
                            algorithm because IPM always stops at INTERIOR
                            point:
                            * stats[i]>0  =>  constraint at upper bound
                                              (also used for free non-basic
                                              variables set to zero)
                            * stats[i]<0  =>  constraint at lower bound
                            * stats[i]=0  =>  constraint is inactive, basic
                                              variable
* primalerror               primal feasibility error
* dualerror                 dual feasibility error
* slackerror                complementary slackness error
* iterationscount           iteration count
* terminationtype           completion code (see below)

COMPLETION CODES

Completion codes:
* -4    LP problem is primal unbounded (dual infeasible)
* -3    LP problem is primal infeasible (dual unbounded)
*  1..4 successful completion
*  5    MaxIts steps was taken
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
        
LAGRANGE COEFFICIENTS

Positive Lagrange coefficient means that constraint is at its upper bound.
Negative coefficient means that constraint is at its lower  bound.  It  is
expected that at solution the dual feasibility condition holds:

    C + SUM(Ei*LagBC[i],i=0..n-1) + SUM(Ai*LagLC[i],i=0..m-1) ~ 0
    
where
* C is a cost vector (linear term)
* Ei is a vector with 1.0 at position I and 0 in other positions
* Ai is an I-th row of linear constraint matrix
*************************************************************************/
typedef struct
{
    double f;
    ae_vector lagbc;
    ae_vector laglc;
    ae_vector y;
    ae_vector stats;
    double primalerror;
    double dualerror;
    double slackerror;
    ae_int_t iterationscount;
    ae_int_t terminationtype;
} minlpreport;


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
void minlpcreate(ae_int_t n, minlpstate* state, ae_state *_state);


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
void minlpsetalgodss(minlpstate* state, double eps, ae_state *_state);


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
void minlpsetalgoipm(minlpstate* state, double eps, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
This function solves LP problem.

INPUT PARAMETERS:
    State   -   algorithm state

You should use minlpresults() function to access results  after  calls  to
this function.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey.
*************************************************************************/
void minlpoptimize(minlpstate* state, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);
void _minlpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlpstate_clear(void* _p);
void _minlpstate_destroy(void* _p);
void _minlpreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlpreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlpreport_clear(void* _p);
void _minlpreport_destroy(void* _p);


/*$ End $*/
#endif

