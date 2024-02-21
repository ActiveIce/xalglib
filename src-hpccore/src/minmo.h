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

#ifndef _minmo_h
#define _minmo_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "linmin.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "optguardapi.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "creflections.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "matgen.h"
#include "scodes.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "rotations.h"
#include "trfac.h"
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
#include "matinv.h"
#include "optserv.h"
#include "fbls.h"
#include "snnls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "sactivesets.h"
#include "minbleic.h"
#include "lpqppresolve.h"
#include "reviseddualsimplex.h"
#include "nlcslp.h"
#include "xblas.h"
#include "directdensesolvers.h"
#include "lpqpserv.h"
#include "vipmsolver.h"
#include "ipm2solver.h"
#include "nlcsqp.h"
#include "nlcfsqp.h"
#include "nlcaul.h"
#include "minnlc.h"
#include "monbi.h"


/*$ Declarations $*/


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinMO subpackage to work  with  this
object
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double diffstep;
    ae_int_t solvertype;
    double epsx;
    ae_int_t maxits;
    ae_vector s;
    ae_bool xrep;
    ae_vector xstart;
    ae_int_t frontsize;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_int_t msparse;
    ae_int_t mdense;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector cl;
    ae_vector cu;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_int_t protocolversion;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    rcommstate rstate;
    nbistate nbi;
    ae_int_t repfrontsize;
    ae_matrix repparetofront;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_vector dummyr1;
    ae_matrix dummyr2;
    sparsematrix dummysparse;
    ae_vector xbase;
    ae_vector fm2;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fp2;
} minmostate;


/*************************************************************************
These fields store optimization report:
* inneriterationscount      total number of inner iterations
* outeriterationscount      number of internal optimization sessions performed
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

Scaled constraint violations (maximum over all Pareto points) are reported:
* bcerr                     maximum violation of the box constraints
* bcidx                     index of the most violated box  constraint (or
                            -1, if all box constraints  are  satisfied  or
                            there are no box constraint)
* lcerr                     maximum violation of the  linear  constraints,
                            computed as maximum  scaled  distance  between
                            final point and constraint boundary.
* lcidx                     index of the most violated  linear  constraint
                            (or -1, if all constraints  are  satisfied  or
                            there are no general linear constraints)
* nlcerr                    maximum violation of the nonlinear constraints
* nlcidx                    index of the most violated nonlinear constraint
                            (or -1, if all constraints  are  satisfied  or
                            there are no nonlinear constraints)
                            
Violations  of  the  box  constraints  are  scaled  on per-component basis
according to  the  scale  vector s[]  as specified by the minmosetscale().
Violations of the general linear  constraints  are  also   computed  using
user-supplied variable scaling. Violations of  the  nonlinear  constraints
are computed "as is"

TERMINATION CODES

TerminationType field contains completion code, which can be either:

=== FAILURE CODE ===
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signaled.
  -3    box  constraints  are  infeasible.  Note: infeasibility of non-box
        constraints does NOT trigger emergency  completion;  you  have  to
        examine  bcerr/lcerr/nlcerr   to  detect   possibly   inconsistent
        constraints.

=== SUCCESS CODE ===
   2    relative step is no more than EpsX.
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
        
NOTE: The solver internally performs many optimization sessions:  one  for
      each Pareto point, and some  amount  of  preparatory  optimizations.
      Different optimization  sessions  may  return  different  completion
      codes. If at least one of internal optimizations failed, its failure
      code is returned. If none of them failed, the most frequent code  is
      returned.
        
Other fields of this structure are not documented and should not be used!
*************************************************************************/
typedef struct
{
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    double bcerr;
    ae_int_t bcidx;
    double lcerr;
    ae_int_t lcidx;
    double nlcerr;
    ae_int_t nlcidx;
} minmoreport;


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void minmosetxrep(minmostate* state, ae_bool needxrep, ae_state *_state);


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
ae_bool minmoiteration(minmostate* state, ae_state *_state);


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
     ae_state *_state);


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
void minmorequesttermination(minmostate* state, ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void minmosetprotocolv1(minmostate* state, ae_state *_state);
void _minmostate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minmostate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minmostate_clear(void* _p);
void _minmostate_destroy(void* _p);
void _minmoreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minmoreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minmoreport_clear(void* _p);
void _minmoreport_destroy(void* _p);


/*$ End $*/
#endif

