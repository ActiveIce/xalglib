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

#ifndef _optserv_h
#define _optserv_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
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


/*$ Declarations $*/


/*************************************************************************
This structure is used to store temporary buffers for L-BFGS-based preconditioner.

  -- ALGLIB --
     Copyright 01.07.2014 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_vector norms;
    ae_vector alpha;
    ae_vector rho;
    ae_matrix yk;
    ae_vector idx;
    ae_vector bufa;
    ae_vector bufb;
} precbuflbfgs;


/*************************************************************************
This structure is used to store temporary buffers for LowRank preconditioner.

  -- ALGLIB --
     Copyright 21.08.2014 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_vector d;
    ae_matrix v;
    ae_vector bufc;
    ae_matrix bufz;
    ae_matrix bufw;
    ae_vector tmp;
} precbuflowrank;


/*************************************************************************
This object stores BFGS/LBFGS approximation of Hessian
*************************************************************************/
typedef struct
{
    ae_int_t htype;
    ae_int_t n;
    ae_int_t resetfreq;
    double stpshort;
    double gammasml;
    double reg;
    double smallreg;
    double microreg;
    double wolfeeps;
    double maxhess;
    ae_int_t m;
    ae_matrix hcurrent;
    ae_int_t hage;
    double sumy2;
    double sums2;
    double sumsy;
    ae_int_t memlen;
    double sigma;
    double gamma;
    ae_matrix s;
    ae_matrix y;
    double sigmadecay;
    ae_bool lowrankmodelvalid;
    ae_int_t lowrankk;
    ae_matrix lowrankcp;
    ae_matrix lowrankcm;
    ae_bool lowrankeffdvalid;
    ae_vector lowrankeffd;
    ae_matrix lowranksst;
    ae_matrix lowranksyt;
    ae_int_t updatestatus;
    ae_matrix hincoming;
    ae_vector sk;
    ae_vector yk;
    ae_vector hsk;
    ae_vector buf;
    ae_matrix corr2;
    ae_matrix blk;
    ae_matrix jk;
    ae_matrix invsqrtdlk;
    ae_vector bufvmv;
    ae_vector bufupdhx;
} xbfgshessian;


/*************************************************************************
This structure is a smoothness monitor.

  -- ALGLIB --
     Copyright 21.08.2014 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_bool checksmoothness;
    ae_vector s;
    ae_vector dcur;
    ae_int_t enqueuedcnt;
    ae_vector enqueuedstp;
    ae_vector enqueuedx;
    ae_vector enqueuedfunc;
    ae_matrix enqueuedjac;
    ae_vector sortedstp;
    ae_vector sortedidx;
    ae_int_t sortedcnt;
    ae_int_t lagprobinneriter;
    ae_int_t lagprobouteriter;
    double lagprobstepmax;
    ae_int_t lagprobnstepsstored;
    ae_vector lagprobxs;
    ae_vector lagprobd;
    double lagprobstp;
    ae_vector lagprobx;
    ae_vector lagprobfi;
    double lagprobrawlag;
    ae_matrix lagprobj;
    ae_matrix lagprobvalues;
    ae_matrix lagprobjacobians;
    ae_vector lagprobsteps;
    ae_vector lagproblagrangians;
    rcommstate lagrangianprobingrcomm;
    ae_bool linesearchspoiled;
    ae_bool linesearchstarted;
    ae_int_t linesearchinneridx;
    ae_int_t linesearchouteridx;
    double nonc0currentrating;
    double nonc1currentrating;
    ae_bool badgradhasxj;
    optguardreport rep;
    double nonc0strrating;
    double nonc0lngrating;
    optguardnonc0report nonc0strrep;
    optguardnonc0report nonc0lngrep;
    double nonc1test0strrating;
    double nonc1test0lngrating;
    optguardnonc1test0report nonc1test0strrep;
    optguardnonc1test0report nonc1test0lngrep;
    double nonc1test1strrating;
    double nonc1test1lngrating;
    optguardnonc1test1report nonc1test1strrep;
    optguardnonc1test1report nonc1test1lngrep;
    ae_bool needfij;
    ae_vector x;
    ae_vector fi;
    ae_matrix j;
    rcommstate rstateg0;
    ae_vector xbase;
    ae_vector fbase;
    ae_vector fm;
    ae_vector fc;
    ae_vector fp;
    ae_vector jm;
    ae_vector jc;
    ae_vector jp;
    ae_matrix jbaseusr;
    ae_matrix jbasenum;
    ae_vector stp;
    ae_vector bufr;
    ae_vector f;
    ae_vector g;
    ae_vector deltax;
    ae_vector tmpidx;
    ae_vector bufi;
    ae_vector xu;
    ae_vector du;
    ae_vector f0;
    ae_matrix j0;
} smoothnessmonitor;


/*************************************************************************
This structure stores a multiobjective quartic function with a mix of box,
linear and nonlinear (quartic) constraints, and known answers for each  of
its objectives.

Used for testing purposes.

  -- ALGLIB --
     Copyright 04.03.2023 by Bochkanov Sergey
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t nactive;
    ae_matrix xsol;
    ae_vector tgtc;
    ae_matrix tgtb;
    ae_matrix tgta;
    ae_matrix tgtd;
    ae_vector bndl;
    ae_vector bndu;
    ae_matrix densea;
    ae_vector al;
    ae_vector au;
    ae_int_t nlinear;
    ae_vector nlc;
    ae_matrix nlb;
    ae_matrix nla;
    ae_matrix nld;
    ae_vector hl;
    ae_vector hu;
    ae_int_t nnonlinear;
} multiobjectivetestfunction;


/*$ Body $*/


/*************************************************************************
This subroutine checks violation of the box constraints. On output it sets
bcerr to the maximum scaled violation, bcidx to the index of the violating
constraint.

if bcerr=0 (say, if no constraints are violated) then bcidx=-1.

If nonunits=false then s[] is not referenced at all (assumed unit).

  -- ALGLIB --
     Copyright 7.11.2018 by Bochkanov Sergey
*************************************************************************/
void checkbcviolation(/* Boolean */ const ae_vector* hasbndl,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* s,
     ae_bool nonunits,
     double* bcerr,
     ae_int_t* bcidx,
     ae_state *_state);


/*************************************************************************
This subroutine checks violation of the general linear constraints.

Constraints are assumed to be un-normalized and stored in the format "NEC
equality ones followed by NIC inequality ones".

On output it sets lcerr to the maximum scaled violation, lcidx to the source
index of the most violating constraint (row indexes of CLEIC are mapped to
the indexes of the "original" constraints via LCSrcIdx[] array.

if lcerr=0 (say, if no constraints are violated) then lcidx=-1.

If nonunits=false then s[] is not referenced at all (assumed unit).

  -- ALGLIB --
     Copyright 7.11.2018 by Bochkanov Sergey
*************************************************************************/
void checklcviolation(/* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     /* Real    */ const ae_vector* x,
     ae_int_t n,
     double* lcerr,
     ae_int_t* lcidx,
     ae_state *_state);


/*************************************************************************
This subroutine checks violation of the nonlinear constraints. Fi[0] is the
target value (ignored), Fi[1:NG+NH] are values of nonlinear constraints.

On output it sets nlcerr to the scaled violation, nlcidx to the index
of the most violating constraint in [0,NG+NH-1] range.

if nlcerr=0 (say, if no constraints are violated) then nlcidx=-1.

If nonunits=false then s[] is not referenced at all (assumed unit).

  -- ALGLIB --
     Copyright 7.11.2018 by Bochkanov Sergey
*************************************************************************/
void checknlcviolation(/* Real    */ const ae_vector* fi,
     ae_int_t ng,
     ae_int_t nh,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);


/*************************************************************************
This subroutine is same as CheckNLCViolation, but  is  works  with  scaled
constraints: it assumes that Fi[] were divided by FScales[] vector  BEFORE
passing them to this function.

The function checks scaled values, but reports unscaled errors.

  -- ALGLIB --
     Copyright 7.11.2018 by Bochkanov Sergey
*************************************************************************/
void unscaleandchecknlcviolation(/* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* fscales,
     ae_int_t ng,
     ae_int_t nh,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);


/*************************************************************************
This subroutine is used to prepare threshold value which will be used for
trimming of the target function (see comments on TrimFunction() for more
information).

This function accepts only one parameter: function value at the starting
point. It returns threshold which will be used for trimming.

  -- ALGLIB --
     Copyright 10.05.2011 by Bochkanov Sergey
*************************************************************************/
void trimprepare(double f, double* threshold, ae_state *_state);


/*************************************************************************
This subroutine is used to "trim" target function, i.e. to do following
transformation:

                   { {F,G}          if F<Threshold
    {F_tr, G_tr} = {
                   { {Threshold, 0} if F>=Threshold
                   
Such transformation allows us to  solve  problems  with  singularities  by
redefining function in such way that it becomes bounded from above.

  -- ALGLIB --
     Copyright 10.05.2011 by Bochkanov Sergey
*************************************************************************/
void trimfunction(double* f,
     /* Real    */ ae_vector* g,
     ae_int_t n,
     double threshold,
     ae_state *_state);


/*************************************************************************
This function enforces boundary constraints in the X.

This function correctly (although a bit inefficient) handles BL[i] which
are -INF and BU[i] which are +INF.

We have NMain+NSlack  dimensional  X,  with first NMain components bounded
by BL/BU, and next NSlack ones bounded by non-negativity constraints.

INPUT PARAMETERS
    X       -   array[NMain+NSlack], point
    BL      -   array[NMain], lower bounds
                (may contain -INF, when bound is not present)
    HaveBL  -   array[NMain], if HaveBL[i] is False,
                then i-th bound is not present
    BU      -   array[NMain], upper bounds
                (may contain +INF, when bound is not present)
    HaveBU  -   array[NMain], if HaveBU[i] is False,
                then i-th bound is not present

OUTPUT PARAMETERS
    X       -   X with all constraints being enforced

It returns True when constraints are consistent,
False - when constraints are inconsistent.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool enforceboundaryconstraints(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* bl,
     /* Boolean */ const ae_vector* havebl,
     /* Real    */ const ae_vector* bu,
     /* Boolean */ const ae_vector* havebu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);


/*************************************************************************
This function projects gradient into feasible area of boundary constrained
optimization  problem.  X  can  be  infeasible  with  respect  to boundary
constraints.  We  have  NMain+NSlack  dimensional  X,   with  first  NMain 
components bounded by BL/BU, and next NSlack ones bounded by non-negativity
constraints.

INPUT PARAMETERS
    X       -   array[NMain+NSlack], point
    G       -   array[NMain+NSlack], gradient
    BL      -   lower bounds (may contain -INF, when bound is not present)
    HaveBL  -   if HaveBL[i] is False, then i-th bound is not present
    BU      -   upper bounds (may contain +INF, when bound is not present)
    HaveBU  -   if HaveBU[i] is False, then i-th bound is not present

OUTPUT PARAMETERS
    G       -   projection of G. Components of G which satisfy one of the
                following
                    (1) (X[I]<=BndL[I]) and (G[I]>0), OR
                    (2) (X[I]>=BndU[I]) and (G[I]<0)
                are replaced by zeros.

NOTE 1: this function assumes that constraints are feasible. It throws
exception otherwise.

NOTE 2: in fact, projection of ANTI-gradient is calculated,  because  this
function trims components of -G which points outside of the feasible area.
However, working with -G is considered confusing, because all optimization
source work with G.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
void projectgradientintobc(/* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* g,
     /* Real    */ const ae_vector* bl,
     /* Boolean */ const ae_vector* havebl,
     /* Real    */ const ae_vector* bu,
     /* Boolean */ const ae_vector* havebu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);


/*************************************************************************
Given
    a) initial point X0[NMain+NSlack]
       (feasible with respect to bound constraints)
    b) step vector alpha*D[NMain+NSlack]
    c) boundary constraints BndL[NMain], BndU[NMain]
    d) implicit non-negativity constraints for slack variables
this  function  calculates  bound  on  the step length subject to boundary
constraints.

It returns:
    *  MaxStepLen - such step length that X0+MaxStepLen*alpha*D is exactly
       at the boundary given by constraints
    *  VariableToFreeze - index of the constraint to be activated,
       0 <= VariableToFreeze < NMain+NSlack
    *  ValueToFreeze - value of the corresponding constraint.

Notes:
    * it is possible that several constraints can be activated by the step
      at once. In such cases only one constraint is returned. It is caller
      responsibility to check other constraints. This function makes  sure
      that we activate at least one constraint, and everything else is the
      responsibility of the caller.
    * steps smaller than MaxStepLen still can activate constraints due  to
      numerical errors. Thus purpose of this  function  is  not  to  guard 
      against accidental activation of the constraints - quite the reverse, 
      its purpose is to activate at least constraint upon performing  step
      which is too long.
    * in case there is no constraints to activate, we return negative
      VariableToFreeze and zero MaxStepLen and ValueToFreeze.
    * this function assumes that constraints are consistent; it throws
      exception otherwise.

INPUT PARAMETERS
    X           -   array[NMain+NSlack], point. Must be feasible with respect 
                    to bound constraints (exception will be thrown otherwise)
    D           -   array[NMain+NSlack], step direction
    alpha       -   scalar multiplier before D, alpha<>0
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    
OUTPUT PARAMETERS
    VariableToFreeze:
                    * negative value     = step is unbounded, ValueToFreeze=0,
                                           MaxStepLen=0.
                    * non-negative value = at least one constraint, given by
                                           this parameter, will  be  activated
                                           upon performing maximum step.
    ValueToFreeze-  value of the variable which will be constrained
    MaxStepLen  -   maximum length of the step. Can be zero when step vector
                    looks outside of the feasible area.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
void calculatestepbound(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* d,
     double alpha,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t* variabletofreeze,
     double* valuetofreeze,
     double* maxsteplen,
     ae_state *_state);


/*************************************************************************
This function postprocesses bounded step by:
* analysing step length (whether it is equal to MaxStepLen) and activating 
  constraint given by VariableToFreeze if needed
* checking for additional bound constraints to activate

This function uses final point of the step, quantities calculated  by  the
CalculateStepBound()  function.  As  result,  it  returns  point  which is 
exactly feasible with respect to boundary constraints.

NOTE 1: this function does NOT handle and check linear equality constraints
NOTE 2: when StepTaken=MaxStepLen we always activate at least one constraint

INPUT PARAMETERS
    X           -   array[NMain+NSlack], final point to postprocess
    XPrev       -   array[NMain+NSlack], initial point
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    VariableToFreeze-result of CalculateStepBound()
    ValueToFreeze-  result of CalculateStepBound()
    StepTaken   -   actual step length (actual step is equal to the possibly 
                    non-unit step direction vector times this parameter).
                    StepTaken<=MaxStepLen.
    MaxStepLen  -   result of CalculateStepBound()
    
OUTPUT PARAMETERS
    X           -   point bounded with respect to constraints.
                    components corresponding to active constraints are exactly
                    equal to the boundary values.
                    
RESULT:
    number of constraints activated in addition to previously active ones.
    Constraints which were DEACTIVATED are ignored (do not influence
    function value).

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t postprocessboundedstep(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* xprev,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t variabletofreeze,
     double valuetofreeze,
     double steptaken,
     double maxsteplen,
     ae_state *_state);


/*************************************************************************
The  purpose  of  this  function is to prevent algorithm from "unsticking" 
from  the  active  bound  constraints  because  of  numerical noise in the
gradient or Hessian.

It is done by zeroing some components of the search direction D.  D[i]  is
zeroed when both (a) and (b) are true:
a) corresponding X[i] is exactly at the boundary
b) |D[i]*S[i]| <= DropTol*Sqrt(SUM(D[i]^2*S[I]^2))

D  can  be  step  direction , antigradient, gradient, or anything similar. 
Sign of D does not matter, nor matters step length.

NOTE 1: boundary constraints are expected to be consistent, as well as X
        is expected to be feasible. Exception will be thrown otherwise.

INPUT PARAMETERS
    D           -   array[NMain+NSlack], direction
    X           -   array[NMain+NSlack], current point
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    S           -   array[NMain+NSlack], scaling of the variables
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    DropTol     -   drop tolerance, >=0
    
OUTPUT PARAMETERS
    X           -   point bounded with respect to constraints.
                    components corresponding to active constraints are exactly
                    equal to the boundary values.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
void filterdirection(/* Real    */ ae_vector* d,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     /* Real    */ const ae_vector* s,
     ae_int_t nmain,
     ae_int_t nslack,
     double droptol,
     ae_state *_state);


/*************************************************************************
This function returns number of bound constraints whose state was  changed
(either activated or deactivated) when making step from XPrev to X.

Constraints are considered:
* active - when we are exactly at the boundary
* inactive - when we are not at the boundary

You should note that antigradient direction is NOT taken into account when
we make decions on the constraint status.

INPUT PARAMETERS
    X           -   array[NMain+NSlack], final point.
                    Must be feasible with respect to bound constraints.
    XPrev       -   array[NMain+NSlack], initial point.
                    Must be feasible with respect to bound constraints.
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    
RESULT:
    number of constraints whose state was changed.

  -- ALGLIB --
     Copyright 10.01.2012 by Bochkanov Sergey
*************************************************************************/
ae_int_t numberofchangedconstraints(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* xprev,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);


/*************************************************************************
This function finds feasible point of  (NMain+NSlack)-dimensional  problem
subject to NMain explicit boundary constraints (some  constraints  can  be
omitted), NSlack implicit non-negativity constraints,  K  linear  equality
constraints.

INPUT PARAMETERS
    X           -   array[NMain+NSlack], initial point.
    BndL        -   lower bounds, array[NMain]
                    (may contain -INF, when bound is not present)
    HaveBndL    -   array[NMain], if HaveBndL[i] is False,
                    then i-th bound is not present
    BndU        -   array[NMain], upper bounds
                    (may contain +INF, when bound is not present)
    HaveBndU    -   array[NMain], if HaveBndU[i] is False,
                    then i-th bound is not present
    NMain       -   number of main variables
    NSlack      -   number of slack variables
    CE          -   array[K,NMain+NSlack+1], equality  constraints CE*x=b.
                    Rows contain constraints, first  NMain+NSlack  columns
                    contain coefficients before X[], last  column  contain
                    right part.
    K           -   number of linear constraints
    EpsI        -   infeasibility (error in the right part) allowed in the
                    solution

OUTPUT PARAMETERS:
    X           -   feasible point or best infeasible point found before
                    algorithm termination
    QPIts       -   number of QP iterations (for debug purposes)
    GPAIts      -   number of GPA iterations (for debug purposes)
    
RESULT:
    True in case X is feasible, False - if it is infeasible.

  -- ALGLIB --
     Copyright 20.01.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool findfeasiblepoint(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     /* Real    */ const ae_matrix* _ce,
     ae_int_t k,
     double epsi,
     ae_int_t* qpits,
     ae_int_t* gpaits,
     ae_state *_state);


/*************************************************************************
    This function checks that input derivatives are right. First it scales
parameters DF0 and DF1 from segment [A;B] to [0;1]. Then it builds Hermite
spline and derivative of it in 0.5. Search scale as Max(DF0,DF1, |F0-F1|).
Right derivative has to satisfy condition:
    |H-F|/S<=0,001, |H'-F'|/S<=0,001.
    
INPUT PARAMETERS:
    F0  -   function's value in X-TestStep point;
    DF0 -   derivative's value in X-TestStep point;
    F1  -   function's value in X+TestStep point;
    DF1 -   derivative's value in X+TestStep point;
    F   -   testing function's value;
    DF  -   testing derivative's value;
   Width-   width of verification segment.

RESULT:
    If input derivatives is right then function returns true, else 
    function returns false.
    
  -- ALGLIB --
     Copyright 29.05.2012 by Bochkanov Sergey
*************************************************************************/
ae_bool derivativecheck(double f0,
     double df0,
     double f1,
     double df1,
     double f,
     double df,
     double width,
     ae_state *_state);


/*************************************************************************
Having quadratic target function

    f(x) = 0.5*x'*A*x + b'*x + penaltyfactor*0.5*(C*x-b)'*(C*x-b)
    
and its parabolic model along direction D

    F(x0+alpha*D) = D2*alpha^2 + D1*alpha
    
this function estimates numerical errors in the coefficients of the model.
    
It is important that this  function  does  NOT calculate D1/D2  -  it only
estimates numerical errors introduced during evaluation and compares their
magnitudes against magnitudes of numerical errors. As result, one of three
outcomes is returned for each coefficient:
    * "true" coefficient is almost surely positive
    * "true" coefficient is almost surely negative
    * numerical errors in coefficient are so large that it can not be
      reliably distinguished from zero

INPUT PARAMETERS:
    AbsASum -   SUM(|A[i,j]|)
    AbsASum2-   SUM(A[i,j]^2)
    MB      -   max(|B|)
    MX      -   max(|X|)
    MD      -   max(|D|)
    D1      -   linear coefficient
    D2      -   quadratic coefficient

OUTPUT PARAMETERS:
    D1Est   -   estimate of D1 sign,  accounting  for  possible  numerical
                errors:
                * >0    means "almost surely positive" (D1>0 and large)
                * <0    means "almost surely negative" (D1<0 and large)
                * =0    means "pessimistic estimate  of  numerical  errors
                        in D1 is larger than magnitude of D1 itself; it is
                        impossible to reliably distinguish D1 from zero".
    D2Est   -   estimate of D2 sign,  accounting  for  possible  numerical
                errors:
                * >0    means "almost surely positive" (D2>0 and large)
                * <0    means "almost surely negative" (D2<0 and large)
                * =0    means "pessimistic estimate  of  numerical  errors
                        in D2 is larger than magnitude of D2 itself; it is
                        impossible to reliably distinguish D2 from zero".
            
  -- ALGLIB --
     Copyright 14.05.2014 by Bochkanov Sergey
*************************************************************************/
void estimateparabolicmodel(double absasum,
     double absasum2,
     double mx,
     double mb,
     double md,
     double d1,
     double d2,
     ae_int_t* d1est,
     ae_int_t* d2est,
     ae_state *_state);


/*************************************************************************
This function calculates inexact rank-K preconditioner for Hessian  matrix
H=D+W'*C*W, where:
* H is a Hessian matrix, which is approximated by D/W/C
* D is a diagonal matrix with positive entries
* W is a rank-K correction
* C is a diagonal factor of rank-K correction

This preconditioner is inexact but fast - it requires O(N*K)  time  to  be
applied. Its main purpose - to be  used  in  barrier/penalty/AUL  methods,
where ill-conditioning is created by combination of two factors:
* simple bounds on variables => ill-conditioned D
* general barrier/penalty => correction W  with large coefficient C (makes
  problem ill-conditioned) but W itself is well conditioned.

Preconditioner P is calculated by artificially constructing a set of  BFGS
updates which tries to reproduce behavior of H:
* Sk = Wk (k-th row of W)
* Yk = (D+Wk'*Ck*Wk)*Sk
* Yk/Sk are reordered by ascending of C[k]*norm(Wk)^2

Here we assume that rows of Wk are orthogonal or nearly orthogonal,  which
allows us to have O(N*K+K^2) update instead of O(N*K^2) one. Reordering of
updates is essential for having good performance on non-orthogonal problems
(updates which do not add much of curvature are added first,  and  updates
which add very large eigenvalues are added last and override effect of the
first updates).

On input this function takes direction S and components of H.
On output it returns inv(H)*S

  -- ALGLIB --
     Copyright 30.06.2014 by Bochkanov Sergey
*************************************************************************/
void inexactlbfgspreconditioner(/* Real    */ ae_vector* s,
     ae_int_t n,
     /* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t k,
     precbuflbfgs* buf,
     ae_state *_state);


/*************************************************************************
This function prepares exact low-rank preconditioner  for  Hessian  matrix
H=D+W'*C*W, where:
* H is a Hessian matrix, which is approximated by D/W/C
* D is a diagonal matrix with positive entries
* W is a rank-K correction
* C is a diagonal factor of rank-K correction, positive semidefinite

This preconditioner is exact but relatively slow -  it  requires  O(N*K^2)
time to be prepared and O(N*K) time to be applied. It is  calculated  with
the help of Woodbury matrix identity.

It should be used as follows:
* PrepareLowRankPreconditioner() call PREPARES data structure
* subsequent calls to ApplyLowRankPreconditioner() APPLY preconditioner to
  user-specified search direction.

  -- ALGLIB --
     Copyright 30.06.2014 by Bochkanov Sergey
*************************************************************************/
void preparelowrankpreconditioner(/* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t n,
     ae_int_t k,
     precbuflowrank* buf,
     ae_state *_state);


/*************************************************************************
This function apply exact low-rank preconditioner prepared by
PrepareLowRankPreconditioner function (see its comments for more information).

  -- ALGLIB --
     Copyright 30.06.2014 by Bochkanov Sergey
*************************************************************************/
void applylowrankpreconditioner(/* Real    */ ae_vector* s,
     precbuflowrank* buf,
     ae_state *_state);


/*************************************************************************
This subroutine initializes smoothness monitor at  the  beginning  of  the
optimization session. It requires variable scales to be passed.

It is possible to perform "dummy" initialization with N=K=0.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorinit(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t k,
     ae_bool checksmoothness,
     ae_state *_state);


/*************************************************************************
This subroutine starts line search

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorstartlinesearch(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);


/*************************************************************************
This subroutine starts line search for a scalar function - convenience
wrapper for ....StartLineSearch() with unscaled variables.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorstartlinesearch1u(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* invs,
     /* Real    */ const ae_vector* x,
     double f0,
     /* Real    */ const ae_vector* j0,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);


/*************************************************************************
This subroutine enqueues one more trial point

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorenqueuepoint(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* d,
     double stp,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     ae_state *_state);


/*************************************************************************
This subroutine enqueues one more trial point for a task with scalar
function with unscaled variables - a convenience wrapper for more general
EnqueuePoint()

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorenqueuepoint1u(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* invs,
     /* Real    */ const ae_vector* d,
     double stp,
     /* Real    */ const ae_vector* x,
     double f0,
     /* Real    */ const ae_vector* j0,
     ae_state *_state);


/*************************************************************************
This subroutine finalizes line search

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorfinalizelinesearch(smoothnessmonitor* monitor,
     ae_state *_state);


/*************************************************************************
This function starts aggressive probing of the Lagrangian for a  range  of
step lengths [0,StpMax].

INPUT PARAMETERS:
    Monitor         -   monitor object
    LagMult         -   array[K-1], lagrange multipliers for nonlinear
                        constraints
    X               -   array[N], initial point for probing
    D               -   array[N], probing direction
    StpMax          -   range of steps to probe
    

  -- ALGLIB --
     Copyright 10.10.2019 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorstartlagrangianprobing(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* d,
     double stpmax,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);


/*************************************************************************
This function performs aggressive probing and sends points  to  smoothness
monitoring queue via EnqueuePoint() call.

After  each  call  it  returns  point to evaluate in  Monitor.LagProbX and
current step in Monitor.LagProbStp. Caller has to load function values and
Jacobian at X into Monitor.LagProbFi and Monitor.LagProbJ, current Lagrangian
to Monitor.LagProbRawLag and continue iteration.

NOTE: LagProbX[] is not guarded against constraint violation. Both non-box
      and box constraints are ignored.  It  is  caller  responsibility  to
      provide appropriate  X[],  D[]  and  StpMax  which  do  not  violate
      important constraints

  -- ALGLIB --
     Copyright 10.10.2019 by Bochkanov Sergey
*************************************************************************/
ae_bool smoothnessmonitorprobelagrangian(smoothnessmonitor* monitor,
     ae_state *_state);


/*************************************************************************
This function prints probing results to trace log.

Tracing is performed using fixed width for all columns, so you  may  print
a header before printing trace - and reasonably expect that its width will
match that of the trace. This function promises that it wont change  trace
output format without introducing breaking changes into its signature.

NOTE: this function ALWAYS tries to print results; it is caller's responsibility
      to decide whether he needs tracing or not.

  -- ALGLIB --
     Copyright 10.10.2019 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitortracelagrangianprobingresults(smoothnessmonitor* monitor,
     ae_state *_state);


/*************************************************************************
This subroutine tells monitor to output trace info.

If CallerSuggestsTrace=True, monitor  ALWAYS  prints  trace,  even  if  no
suspicions were raised during optimization. If  CallerSuggestsTrace=False,
the monitor will print trace only if:
* trace was requested by trace tag 'OPTGUARD' AND suspicious  points  were
  found during optimization
* trace was requested by trace tag 'OPTGUARD.ALWAYS' - always

  -- ALGLIB --
     Copyright 11.10.2019 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitortracestatus(const smoothnessmonitor* monitor,
     ae_bool callersuggeststrace,
     ae_state *_state);


/*************************************************************************
This subroutine exports report to user-readable representation (all arrays
are forced to have exactly same size as needed; unused arrays are  set  to
zero length).

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorexportreport(smoothnessmonitor* monitor,
     optguardreport* rep,
     ae_state *_state);


/*************************************************************************
Check numerical gradient at point X0 (unscaled variables!), with  optional
box constraints [BndL,BndU] (if  HasBoxConstraints=True)  and  with  scale
vector S[].

Step S[i]*TestStep is performed along I-th variable.

NeedFiJ rcomm protocol is used to request derivative information.

Box constraints BndL/BndU are expected to be feasible. It is  possible  to
have BndL=BndU.

  -- ALGLIB --
     Copyright 06.12.2018 by Bochkanov Sergey
*************************************************************************/
ae_bool smoothnessmonitorcheckgradientatx0(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* unscaledx0,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_bool hasboxconstraints,
     double teststep,
     ae_state *_state);


/*************************************************************************
This function initializes approximation of a Hessian.

Direct BFGS (Hessian matrix H is stored) with stability improvements is used.

INPUT PARAMETERS:
    Hess            -   Hessian structure, initial state is ignored, but
                        previously allocated memory is reused as much as
                        possible
    N               -   dimensions count
    ResetFreq       -   reset frequency for BFGS :
                        * ResetFreq=0 for standard BFGS
                        * ResetFreq>0 for BFGS with periodic resets (helps
                          to maintain fresh curvature information, works
                          better for highly nonquadratic problems)
    StpShort        -   short step length (INF-norm); steps shorter than that
                        are not used for Hessian updates
                        

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
void hessianinitbfgs(xbfgshessian* hess,
     ae_int_t n,
     ae_int_t resetfreq,
     double stpshort,
     ae_state *_state);


/*************************************************************************
This function initializes approximation of a Hessian.

Explicit low-rank representation of LBFGS is used.

INPUT PARAMETERS:
    Hess            -   Hessian structure, initial state is ignored, but
                        previously allocated memory is reused as much as
                        possible
    N               -   dimensions count, N>0
    M               -   memory size, M>=0 (values above N will be reduced
                        to N)
    StpShort        -   short step length (INF-norm); steps shorter than that
                        are not used for Hessian updates
    MaxHess         -   maximum Hessian curvature; steps producing curvature
                        larger than that are rejected

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
void hessianinitlowrank(xbfgshessian* hess,
     ae_int_t n,
     ae_int_t m,
     double stpshort,
     double maxhess,
     ae_state *_state);


/*************************************************************************
Updates Hessian estimate, uses regularized formula which prevents  Hessian
eigenvalues from decreasing below ~sqrt(Eps)  and  rejects  updates larger
than ~1/sqrt(Eps) in magnitude.

Either BFGS or LBFGS formula is used, depending on Hessian model settings.

INPUT PARAMETERS:
    Hess            -   Hessian state
    X0, G0          -   point #0 and gradient at #0, array[N]
    X1, G1          -   point #1 and gradient at #1, array[N]
    DoTrace         -   True if we want to print trace messages

On return sets Hess.UpdateStatus flag

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
void hessianupdate(xbfgshessian* hess,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* g0,
     /* Real    */ const ae_vector* x1,
     /* Real    */ const ae_vector* g1,
     ae_bool dotrace,
     ae_state *_state);


/*************************************************************************
Get diagonal of the Hessian.

This function works only with the following Hessian modes:
* direct BFGS
* low-rank LBFGS

Any attempt to call it for other Hessian type will result in exception.

INPUT PARAMETERS:
    Hess            -   Hessian state
    D               -   possibly preallocated array; resized if needed
    
OUTPUT PARAMETERS:
    D               -   first N elements are filled with Hessian diagonal

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
void hessiangetdiagonal(xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     ae_state *_state);


/*************************************************************************
Get Hessian matrix in a dense format.

This function works only with the following Hessian modes:
* direct BFGS
* low-rank LBFGS (needs k*N*N operations)

Any attempt to call it for other Hessian type will result in exception.

INPUT PARAMETERS:
    Hess            -   Hessian state
    IsUpper         -   whether upper or lower triangle is needed
    H               -   possibly preallocated array; resized if needed
    
OUTPUT PARAMETERS:
    H               -   either upper or lower NxN elements are filled
                        with Hessian

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
void hessiangetmatrix(xbfgshessian* hess,
     ae_bool isupper,
     /* Real    */ ae_matrix* h,
     ae_state *_state);


/*************************************************************************
Computes direct product H*x (here H is a Hessian matrix, not its inverse).
Either BFGS or LBFGS formula is used, depending on Hessian model settings.

NOTE: this function modifies internal state of Hess,  it  uses  temporary
      arrays allocated in Hess. Thus, it is not thread-safe.

INPUT PARAMETERS:
    Hess            -   Hessian state
    X               -   array[N]

OUTPUT PARAMETERS:
    HX              -   array[N], H*x

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
void hessianmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     ae_state *_state);


/*************************************************************************
Computes direct product x'*H*x (here H is a Hessian matrix, not its inverse).
Either BFGS or LBFGS formula is used, depending on Hessian model settings.

NOTE: this function modifies internal state of Hess,  it  uses  temporary
      arrays allocated in Hess. Thus, it is not thread-safe.

NOTE: for low-rank models this function is much more precise than computing
      H*x and then computing dot product with x.

INPUT PARAMETERS:
    Hess            -   Hessian state
    X               -   array[N]

RESULT:
    x'*H*x

  -- ALGLIB --
     Copyright 14.03.2023 by Bochkanov Sergey
*************************************************************************/
double hessianvmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     ae_state *_state);


/*************************************************************************
Computes direct product x'*H*x (here H is a Hessian matrix, not its inverse)
along with direct product H*x.

Either BFGS or LBFGS formula is used, depending on Hessian model settings.

NOTE: this function modifies internal state of Hess,  it  uses  temporary
      arrays allocated in Hess. Thus, it is not thread-safe.

NOTE: for low-rank models this function computes more precise x'*H*x than
      H*x multiplied by x.

INPUT PARAMETERS:
    Hess            -   Hessian state
    X               -   array[N]

OUTPUT PARAMETERS:
    HX              -   array[N], H*x
    XHX             -   x'*H*x

  -- ALGLIB --
     Copyright 14.03.2023 by Bochkanov Sergey
*************************************************************************/
void hessianxmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     double* xhx,
     ae_state *_state);


/*************************************************************************
Creates random multiobjective test problem with no known answers

INPUT PARAMETERS:
    N               -   vars cnt
    M               -   targets cnt
    NEquality       -   desired count of equality constraints of all kinds
                        (box, linear, nonlinear)
    NInequality     -   desired count of non-box inequality constraints
    TaskKind        -   task type:
                        * 0 = convex quartic
                        * 1 = convex quadratic
                        * 2 = convex low rank quadratic
                        * 3 = linear
                        * 4 = constant
    NLQuadratic     -   scale of the quadratic term of nonlinear constraints
    NLQuartic       -   scale of the quadratic term of nonlinear constraints

  -- ALGLIB --
     Copyright 04.03.2023 by Bochkanov Sergey
*************************************************************************/
void motfcreaterandomunknown(ae_int_t n,
     ae_int_t m,
     ae_int_t nequality,
     ae_int_t ninequality,
     ae_int_t taskkind,
     double nlquadratic,
     double nlquartic,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);


/*************************************************************************
Evaluate multiobjective test problem and its nonlinear constraints.

INPUT PARAMETERS:
    Problem             -   problem structure
    X                   -   trial point
    Fi                  -   preallocated array for M objectives and 
                            Problem.NNonLinear constraints
    NeedFi              -   whether Fi is needed or not
    Jac                 -   preallocated Jacobian array
    NeedJac             -   whether Jac is needed or not

  -- ALGLIB --
     Copyright 04.03.2023 by Bochkanov Sergey
*************************************************************************/
void motfeval(const multiobjectivetestfunction* problem,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* fi,
     ae_bool needfi,
     /* Real    */ ae_matrix* jac,
     ae_bool needjac,
     ae_state *_state);


/*************************************************************************
Converts modern two-sided mixed sparse/dense linear constraints to an  old
dense single-sided format.

NOTE: this function ignores inconsistent bounds, like CL>CU, assuming that
      either they were already noticed, or will be noticed later.
*************************************************************************/
void converttwosidedlctoonesidedold(const sparsematrix* sparsec,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densec,
     ae_int_t kdense,
     ae_int_t n,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     /* Real    */ ae_matrix* olddensec,
     /* Integer */ ae_vector* olddensect,
     ae_int_t* olddensek,
     ae_state *_state);


/*************************************************************************
Prepares conversion of the modern two-sided nonlinear constraints to an old
single-sided zero-bounded format.

NOTE: this function ignores inconsistent bounds, like NL>NU, assuming that
      either they were already noticed, or will be noticed later.
*************************************************************************/
void converttwosidednlctoonesidedold(/* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     /* Integer */ ae_vector* nlcidx,
     /* Real    */ ae_vector* nlcmul,
     /* Real    */ ae_vector* nlcadd,
     ae_int_t* cntnlec,
     ae_int_t* cntnlic,
     ae_state *_state);


/*************************************************************************
Increases trust region growth factor until maximum growth reached.
*************************************************************************/
void trustradincreasemomentum(double* growthfactor,
     double growthincrease,
     double maxgrowthfactor,
     ae_state *_state);


/*************************************************************************
Resets trust region growth factor to its default state.
*************************************************************************/
void trustradresetmomentum(double* growthfactor,
     double mingrowthfactor,
     ae_state *_state);
void _precbuflbfgs_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _precbuflbfgs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _precbuflbfgs_clear(void* _p);
void _precbuflbfgs_destroy(void* _p);
void _precbuflowrank_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _precbuflowrank_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _precbuflowrank_clear(void* _p);
void _precbuflowrank_destroy(void* _p);
void _xbfgshessian_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _xbfgshessian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _xbfgshessian_clear(void* _p);
void _xbfgshessian_destroy(void* _p);
void _smoothnessmonitor_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _smoothnessmonitor_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _smoothnessmonitor_clear(void* _p);
void _smoothnessmonitor_destroy(void* _p);
void _multiobjectivetestfunction_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _multiobjectivetestfunction_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _multiobjectivetestfunction_clear(void* _p);
void _multiobjectivetestfunction_destroy(void* _p);


/*$ End $*/
#endif

