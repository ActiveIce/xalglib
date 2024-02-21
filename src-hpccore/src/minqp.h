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

#ifndef _minqp_h
#define _minqp_h

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
#include "hblas.h"
#include "creflections.h"
#include "sblas.h"
#include "ablas.h"
#include "ortfac.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "matgen.h"
#include "trfac.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "normestimator.h"
#include "linlsqr.h"
#include "linmin.h"
#include "optguardapi.h"
#include "matinv.h"
#include "optserv.h"
#include "fbls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"
#include "snnls.h"
#include "sactivesets.h"
#include "qqpsolver.h"
#include "qpdenseaulsolver.h"
#include "minbleic.h"
#include "qpbleicsolver.h"
#include "vipmsolver.h"
#include "ipm2solver.h"


/*$ Declarations $*/


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinQP subpackage to work with this
object
*************************************************************************/
typedef struct
{
    ae_int_t n;
    qqpsettings qqpsettingsuser;
    qpbleicsettings qpbleicsettingsuser;
    qpdenseaulsettings qpdenseaulsettingsuser;
    double veps;
    ae_bool dbgskipconstraintnormalization;
    ae_int_t algokind;
    ae_int_t akind;
    convexquadraticmodel a;
    sparsematrix sparsea;
    ae_bool sparseaupper;
    double absamax;
    double absasum;
    double absasum2;
    ae_vector b;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t stype;
    ae_vector s;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector xorigin;
    ae_vector startx;
    ae_bool havex;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector cl;
    ae_vector cu;
    ae_int_t mdense;
    ae_int_t msparse;
    ae_vector xs;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repncholesky;
    ae_int_t repnmv;
    ae_int_t repterminationtype;
    ae_vector replagbc;
    ae_vector replaglc;
    ae_vector effectives;
    ae_vector tmp0;
    ae_matrix ecleic;
    ae_vector elaglc;
    ae_vector elagmlt;
    ae_vector elagidx;
    ae_vector dummyr;
    ae_matrix dummyr2;
    sparsematrix dummysparse;
    ae_matrix tmpr2;
    ae_vector wrkbndl;
    ae_vector wrkbndu;
    ae_vector wrkcl;
    ae_vector wrkcu;
    ae_matrix wrkdensec;
    sparsematrix wrksparsec;
    ae_bool qpbleicfirstcall;
    qpbleicbuffers qpbleicbuf;
    qqpbuffers qqpbuf;
    qpdenseaulbuffers qpdenseaulbuf;
    vipmstate vsolver;
    ipm2state ipm2;
} minqpstate;


/*************************************************************************
This structure stores optimization report:
* InnerIterationsCount      number of inner iterations
* OuterIterationsCount      number of outer iterations
* NCholesky                 number of Cholesky decomposition
* NMV                       number of matrix-vector products
                            (only products calculated as part of iterative
                            process are counted)
* TerminationType           completion code (see below)
* LagBC                     Lagrange multipliers for box constraints,
                            array[N], not filled by QP-BLEIC solver
* LagLC                     Lagrange multipliers for linear constraints,
                            array[MSparse+MDense], ignored by QP-BLEIC solver

=== COMPLETION CODES =====================================================

Completion codes:
* -9    failure of the automatic scale evaluation:  one  of  the  diagonal
        elements of the quadratic term is non-positive.  Specify  variable
        scales manually!
* -5    inappropriate solver was used:
        * QuickQP solver for problem with general linear constraints (dense/sparse)
* -4    BLEIC-QP or QuickQP solver found unconstrained direction
        of negative curvature (function is unbounded from
        below  even  under  constraints),  no  meaningful
        minimum can be found.
* -3    inconsistent constraints (or, maybe, feasible point is
        too hard to find). If you are sure that constraints are feasible,
        try to restart optimizer with better initial approximation.
* -2    IPM solver has difficulty finding primal/dual feasible point.
        It is likely that the problem is either infeasible or unbounded,
        but it is difficult to determine exact reason for termination.
        X contains best point found so far.
*  1..4 successful completion
*  5    MaxIts steps was taken
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
        
=== LAGRANGE MULTIPLIERS =================================================

Some  optimizers  report  values of  Lagrange  multipliers  on  successful
completion (positive completion code):
* DENSE-IPM-QP and SPARSE-IPM-QP return very precise Lagrange  multipliers
  as determined during solution process.
* DENSE-AUL-QP returns approximate Lagrange multipliers  (which  are  very
  close to "true"  Lagrange  multipliers  except  for  overconstrained  or
  degenerate problems)

Two arrays of multipliers are returned:
* LagBC is array[N] which is loaded with multipliers from box constraints;
  LagBC[i]>0 means that I-th constraint is at the  upper bound, LagBC[I]<0
  means that I-th constraint is at the lower bound, LagBC[I]=0 means  that
  I-th box constraint is inactive.
* LagLC is array[MSparse+MDense] which is  loaded  with  multipliers  from
  general  linear  constraints  (former  MSparse  elements  corresponds to
  sparse part of the constraint matrix, latter MDense are  for  the  dense
  constraints, as was specified by user).
  LagLC[i]>0 means that I-th constraint at  the  upper  bound,  LagLC[i]<0
  means that I-th constraint is at the lower bound, LagLC[i]=0 means  that
  I-th linear constraint is inactive.
  
On failure (or when optimizer does not support Lagrange multipliers) these
arrays are zero-filled.

It is expected that at solution the dual feasibility condition holds:

    C+H*(Xs-X0) + SUM(Ei*LagBC[i],i=0..n-1) + SUM(Ai*LagLC[i],i=0..m-1) ~ 0
    
where
* C is a linear term
* H is a quadratic term
* Xs is a solution, and X0 is an origin term (zero by default)
* Ei is a vector with 1.0 at position I and 0 in other positions
* Ai is an I-th row of linear constraint matrix

NOTE: methods  from  IPM  family  may  also  return  meaningful   Lagrange
      multipliers  on  completion   with   code   -2   (infeasibility   or
      unboundedness  detected).
*************************************************************************/
typedef struct
{
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
    ae_int_t nmv;
    ae_int_t ncholesky;
    ae_int_t terminationtype;
    ae_vector lagbc;
    ae_vector laglc;
} minqpreport;


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
void minqpcreate(ae_int_t n, minqpstate* state, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void minqpsetscaleautodiag(minqpstate* state, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void minqpsetalgodenseipm(minqpstate* state, double eps, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void minqpoptimize(minqpstate* state, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
Fast version of MinQPSetLinearTerm(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetlineartermfast(minqpstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
Fast version of MinQPSetStartingPoint(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetstartingpointfast(minqpstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);


/*************************************************************************
Fast version of MinQPSetOrigin(), which doesn't check its arguments.
For internal use only.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetoriginfast(minqpstate* state,
     /* Real    */ const ae_vector* xorigin,
     ae_state *_state);
void _minqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minqpstate_clear(void* _p);
void _minqpstate_destroy(void* _p);
void _minqpreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minqpreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minqpreport_clear(void* _p);
void _minqpreport_destroy(void* _p);


/*$ End $*/
#endif

