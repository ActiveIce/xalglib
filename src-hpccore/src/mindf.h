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

#ifndef _mindf_h
#define _mindf_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "apserv.h"
#include "ablasf.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "creflections.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "matgen.h"
#include "scodes.h"
#include "tsort.h"
#include "sparse.h"
#include "linmin.h"
#include "optguardapi.h"
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
#include "lpqpserv.h"
#include "ssgd.h"
#include "diffevo.h"


/*$ Declarations $*/


/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinDF subpackage to work  with  this
object
*************************************************************************/
typedef struct
{
    ae_int_t solvertype;
    ae_int_t n;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_matrix densea;
    ae_vector al;
    ae_vector au;
    ae_int_t m;
    ae_vector nl;
    ae_vector nu;
    ae_int_t nnlc;
    ae_vector s;
    ae_bool hasx0;
    ae_vector x0;
    ae_bool stoponsmallf;
    double smallf;
    ae_int_t rngseed;
    ae_bool xrep;
    ae_vector xf;
    ae_int_t protocolversion;
    ae_int_t requesttype;
    ae_vector reportx;
    double reportf;
    ae_int_t querysize;
    ae_int_t queryfuncs;
    ae_int_t queryvars;
    ae_int_t querydim;
    ae_int_t queryformulasize;
    ae_vector querydata;
    ae_vector replyfi;
    ae_vector replydj;
    sparsematrix replysj;
    ae_vector tmpx1;
    ae_vector tmpc1;
    ae_vector tmpf1;
    ae_vector tmpg1;
    ae_matrix tmpj1;
    ae_bool userterminationneeded;
    ae_int_t gdemoepochscnt;
    ae_int_t gdemopopsize;
    double gdemorho1;
    double gdemorho2;
    gdemostate gdemosolver;
    ae_bool gdemofixedparams;
    ae_int_t gdemostrategy;
    double gdemocrossoverprob;
    double gdemodifferentialweight;
    ae_int_t ssgdmaxits;
    double ssgdrate0;
    double ssgdrate1;
    double ssgdblur0;
    double ssgdblur1;
    ae_int_t ssgdouterits;
    double ssgdmomentum;
    double ssgdrho;
    ssgdstate ssgdsolver;
    rcommstate rstate;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    ae_int_t repnfunc;
    ae_int_t repnrequests;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_vector tmp0;
} mindfstate;


/*************************************************************************
This structure stores optimization report:
* IterationsCount           total number of inner iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)
* BCErr                     maximum violation of box constraints
* LCErr                     maximum violation of linear constraints
* NLCErr                    maximum violation of nonlinear constraints

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    box constraints are inconsistent
  -1    inconsistent parameters were passed:
        * penalty parameter is zero, but we have nonlinear constraints
          set by mindfsetnlc2()
   2    sampling radius decreased below epsx
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    User requested termination via mindfrequesttermination()

Other fields of this structure are not documented and should not be used!
*************************************************************************/
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    double bcerr;
    double lcerr;
    double nlcerr;
    ae_int_t terminationtype;
} mindfreport;


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


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
void mindfrequesttermination(mindfstate* state, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  -- ALGLIB --
     Copyright 25.07.2023 by Bochkanov Sergey
*************************************************************************/
void mindfsetsmallf(mindfstate* state, double f, ae_state *_state);


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
     ae_state *_state);


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
ae_bool mindfiteration(mindfstate* state, ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);


/*************************************************************************
Set V1 reverse communication protocol
*************************************************************************/
void mindfsetprotocolv2(mindfstate* state, ae_state *_state);
void _mindfstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mindfstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mindfstate_clear(void* _p);
void _mindfstate_destroy(void* _p);
void _mindfreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mindfreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mindfreport_clear(void* _p);
void _mindfreport_destroy(void* _p);


/*$ End $*/
#endif

