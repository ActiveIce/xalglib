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
#include <stdio.h>
#include "optserv.h"


/*$ Declarations $*/
static double optserv_ognoiselevelf = 1.0E2*ae_machineepsilon;
static double optserv_ognoiselevelg = 1.0E4*ae_machineepsilon;
static double optserv_ogminrating0 = 50.0;
static double optserv_ogminrating1 = 50.0;
static double optserv_feasibilityerror(/* Real    */ const ae_matrix* ce,
     /* Real    */ const ae_vector* x,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t k,
     /* Real    */ ae_vector* tmp0,
     ae_state *_state);
static void optserv_feasibilityerrorgrad(/* Real    */ const ae_matrix* ce,
     /* Real    */ const ae_vector* x,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t k,
     double* err,
     /* Real    */ ae_vector* grad,
     /* Real    */ ae_vector* tmp0,
     ae_state *_state);
static void optserv_testc0continuity(double f0,
     double f1,
     double f2,
     double f3,
     double noise0,
     double noise1,
     double noise2,
     double noise3,
     double delta0,
     double delta1,
     double delta2,
     ae_bool applyspecialcorrection,
     double* rating,
     double* lipschitz,
     ae_state *_state);
static void optserv_c1continuitytest0(smoothnessmonitor* monitor,
     ae_int_t funcidx,
     ae_int_t stpidx,
     ae_int_t sortedcnt,
     ae_state *_state);
static void optserv_c1continuitytest1(smoothnessmonitor* monitor,
     ae_int_t funcidx,
     ae_int_t stpidx,
     ae_int_t sortedcnt,
     ae_state *_state);
static void optserv_popfrontxy(xbfgshessian* hess, ae_state *_state);
static void optserv_hessianupdatelowlevel(xbfgshessian* hess,
     /* Real    */ ae_matrix* h,
     /* Real    */ const ae_vector* sk,
     /* Real    */ const ae_vector* yk,
     ae_int_t* status,
     ae_state *_state);
static void optserv_resetlowrankmodel(xbfgshessian* hess,
     ae_state *_state);
static void optserv_recomputelowrankmodel(xbfgshessian* hess,
     ae_state *_state);
static void optserv_recomputelowrankdiagonal(xbfgshessian* hess,
     ae_state *_state);


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
     ae_state *_state)
{
    ae_int_t i;
    double vs;
    double ve;

    *bcerr = 0.0;
    *bcidx = 0;

    *bcerr = (double)(0);
    *bcidx = -1;
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Fetch scale
         */
        if( nonunits )
        {
            vs = (double)1/s->ptr.p_double[i];
        }
        else
        {
            vs = (double)(1);
        }
        
        /*
         * Check lower bound
         */
        if( hasbndl->ptr.p_bool[i]&&x->ptr.p_double[i]<bndl->ptr.p_double[i] )
        {
            ve = (bndl->ptr.p_double[i]-x->ptr.p_double[i])*vs;
            if( ve>*bcerr )
            {
                *bcerr = ve;
                *bcidx = i;
            }
        }
        
        /*
         * Check upper bound
         */
        if( hasbndu->ptr.p_bool[i]&&x->ptr.p_double[i]>bndu->ptr.p_double[i] )
        {
            ve = (x->ptr.p_double[i]-bndu->ptr.p_double[i])*vs;
            if( ve>*bcerr )
            {
                *bcerr = ve;
                *bcidx = i;
            }
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double cx;
    double cnrm;
    double v;

    *lcerr = 0.0;
    *lcidx = 0;

    *lcerr = (double)(0);
    *lcidx = -1;
    for(i=0; i<=nec+nic-1; i++)
    {
        cx = -cleic->ptr.pp_double[i][n];
        cnrm = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            v = cleic->ptr.pp_double[i][j];
            cx = cx+v*x->ptr.p_double[j];
            cnrm = cnrm+v*v;
        }
        cnrm = ae_sqrt(cnrm, _state);
        cx = cx/coalesce(cnrm, (double)(1), _state);
        if( i<nec )
        {
            cx = ae_fabs(cx, _state);
        }
        else
        {
            cx = ae_maxreal(cx, (double)(0), _state);
        }
        if( cx>*lcerr )
        {
            *lcerr = cx;
            *lcidx = lcsrcidx->ptr.p_int[i];
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    double v;

    *nlcerr = 0.0;
    *nlcidx = 0;

    *nlcerr = (double)(0);
    *nlcidx = -1;
    for(i=0; i<=ng+nh-1; i++)
    {
        v = fi->ptr.p_double[i+1];
        if( i<ng )
        {
            v = ae_fabs(v, _state);
        }
        else
        {
            v = ae_maxreal(v, (double)(0), _state);
        }
        if( v>*nlcerr )
        {
            *nlcerr = v;
            *nlcidx = i;
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    double v;

    *nlcerr = 0.0;
    *nlcidx = 0;

    *nlcerr = (double)(0);
    *nlcidx = -1;
    for(i=0; i<=ng+nh-1; i++)
    {
        ae_assert(ae_fp_greater(fscales->ptr.p_double[i+1],(double)(0)), "UnscaleAndCheckNLCViolation: integrity check failed", _state);
        v = fi->ptr.p_double[i+1]*fscales->ptr.p_double[i+1];
        if( i<ng )
        {
            v = ae_fabs(v, _state);
        }
        else
        {
            v = ae_maxreal(v, (double)(0), _state);
        }
        if( v>*nlcerr )
        {
            *nlcerr = v;
            *nlcidx = i;
        }
    }
}


/*************************************************************************
This subroutine is used to prepare threshold value which will be used for
trimming of the target function (see comments on TrimFunction() for more
information).

This function accepts only one parameter: function value at the starting
point. It returns threshold which will be used for trimming.

  -- ALGLIB --
     Copyright 10.05.2011 by Bochkanov Sergey
*************************************************************************/
void trimprepare(double f, double* threshold, ae_state *_state)
{

    *threshold = 0.0;

    *threshold = (double)10*(ae_fabs(f, _state)+(double)1);
}


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
     ae_state *_state)
{
    ae_int_t i;


    if( ae_fp_greater_eq(*f,threshold) )
    {
        *f = threshold;
        for(i=0; i<=n-1; i++)
        {
            g->ptr.p_double[i] = 0.0;
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_bool result;


    result = ae_false;
    for(i=0; i<=nmain-1; i++)
    {
        if( (havebl->ptr.p_bool[i]&&havebu->ptr.p_bool[i])&&ae_fp_greater(bl->ptr.p_double[i],bu->ptr.p_double[i]) )
        {
            return result;
        }
        if( havebl->ptr.p_bool[i]&&ae_fp_less(x->ptr.p_double[i],bl->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = bl->ptr.p_double[i];
        }
        if( havebu->ptr.p_bool[i]&&ae_fp_greater(x->ptr.p_double[i],bu->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = bu->ptr.p_double[i];
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_less(x->ptr.p_double[nmain+i],(double)(0)) )
        {
            x->ptr.p_double[nmain+i] = (double)(0);
        }
    }
    result = ae_true;
    return result;
}


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
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=nmain-1; i++)
    {
        ae_assert((!havebl->ptr.p_bool[i]||!havebu->ptr.p_bool[i])||ae_fp_less_eq(bl->ptr.p_double[i],bu->ptr.p_double[i]), "ProjectGradientIntoBC: internal error (infeasible constraints)", _state);
        if( (havebl->ptr.p_bool[i]&&ae_fp_less_eq(x->ptr.p_double[i],bl->ptr.p_double[i]))&&ae_fp_greater(g->ptr.p_double[i],(double)(0)) )
        {
            g->ptr.p_double[i] = (double)(0);
        }
        if( (havebu->ptr.p_bool[i]&&ae_fp_greater_eq(x->ptr.p_double[i],bu->ptr.p_double[i]))&&ae_fp_less(g->ptr.p_double[i],(double)(0)) )
        {
            g->ptr.p_double[i] = (double)(0);
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_less_eq(x->ptr.p_double[nmain+i],(double)(0))&&ae_fp_greater(g->ptr.p_double[nmain+i],(double)(0)) )
        {
            g->ptr.p_double[nmain+i] = (double)(0);
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    double prevmax;
    double initval;

    *variabletofreeze = 0;
    *valuetofreeze = 0.0;
    *maxsteplen = 0.0;

    ae_assert(ae_fp_neq(alpha,(double)(0)), "CalculateStepBound: zero alpha", _state);
    *variabletofreeze = -1;
    initval = ae_maxrealnumber;
    *maxsteplen = initval;
    for(i=0; i<=nmain-1; i++)
    {
        if( havebndl->ptr.p_bool[i]&&ae_fp_less(alpha*d->ptr.p_double[i],(double)(0)) )
        {
            ae_assert(ae_fp_greater_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]), "CalculateStepBound: infeasible X", _state);
            prevmax = *maxsteplen;
            *maxsteplen = safeminposrv(x->ptr.p_double[i]-bndl->ptr.p_double[i], -alpha*d->ptr.p_double[i], *maxsteplen, _state);
            if( ae_fp_less(*maxsteplen,prevmax) )
            {
                *variabletofreeze = i;
                *valuetofreeze = bndl->ptr.p_double[i];
            }
        }
        if( havebndu->ptr.p_bool[i]&&ae_fp_greater(alpha*d->ptr.p_double[i],(double)(0)) )
        {
            ae_assert(ae_fp_less_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]), "CalculateStepBound: infeasible X", _state);
            prevmax = *maxsteplen;
            *maxsteplen = safeminposrv(bndu->ptr.p_double[i]-x->ptr.p_double[i], alpha*d->ptr.p_double[i], *maxsteplen, _state);
            if( ae_fp_less(*maxsteplen,prevmax) )
            {
                *variabletofreeze = i;
                *valuetofreeze = bndu->ptr.p_double[i];
            }
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_less(alpha*d->ptr.p_double[nmain+i],(double)(0)) )
        {
            ae_assert(ae_fp_greater_eq(x->ptr.p_double[nmain+i],(double)(0)), "CalculateStepBound: infeasible X", _state);
            prevmax = *maxsteplen;
            *maxsteplen = safeminposrv(x->ptr.p_double[nmain+i], -alpha*d->ptr.p_double[nmain+i], *maxsteplen, _state);
            if( ae_fp_less(*maxsteplen,prevmax) )
            {
                *variabletofreeze = nmain+i;
                *valuetofreeze = (double)(0);
            }
        }
    }
    if( ae_fp_eq(*maxsteplen,initval) )
    {
        *valuetofreeze = (double)(0);
        *maxsteplen = (double)(0);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_bool wasactivated;
    ae_int_t result;


    ae_assert(variabletofreeze<0||ae_fp_less_eq(steptaken,maxsteplen), "Assertion failed", _state);
    
    /*
     * Activate constraints
     */
    if( variabletofreeze>=0&&ae_fp_eq(steptaken,maxsteplen) )
    {
        x->ptr.p_double[variabletofreeze] = valuetofreeze;
    }
    for(i=0; i<=nmain-1; i++)
    {
        if( havebndl->ptr.p_bool[i]&&ae_fp_less(x->ptr.p_double[i],bndl->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = bndl->ptr.p_double[i];
        }
        if( havebndu->ptr.p_bool[i]&&ae_fp_greater(x->ptr.p_double[i],bndu->ptr.p_double[i]) )
        {
            x->ptr.p_double[i] = bndu->ptr.p_double[i];
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_less_eq(x->ptr.p_double[nmain+i],(double)(0)) )
        {
            x->ptr.p_double[nmain+i] = (double)(0);
        }
    }
    
    /*
     * Calculate number of constraints being activated
     */
    result = 0;
    for(i=0; i<=nmain-1; i++)
    {
        wasactivated = ae_fp_neq(x->ptr.p_double[i],xprev->ptr.p_double[i])&&((havebndl->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]))||(havebndu->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i])));
        wasactivated = wasactivated||variabletofreeze==i;
        if( wasactivated )
        {
            result = result+1;
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        wasactivated = ae_fp_neq(x->ptr.p_double[nmain+i],xprev->ptr.p_double[nmain+i])&&ae_fp_eq(x->ptr.p_double[nmain+i],0.0);
        wasactivated = wasactivated||variabletofreeze==nmain+i;
        if( wasactivated )
        {
            result = result+1;
        }
    }
    return result;
}


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
     ae_state *_state)
{
    ae_int_t i;
    double scalednorm;
    ae_bool isactive;


    scalednorm = 0.0;
    for(i=0; i<=nmain+nslack-1; i++)
    {
        scalednorm = scalednorm+ae_sqr(d->ptr.p_double[i]*s->ptr.p_double[i], _state);
    }
    scalednorm = ae_sqrt(scalednorm, _state);
    for(i=0; i<=nmain-1; i++)
    {
        ae_assert(!havebndl->ptr.p_bool[i]||ae_fp_greater_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]), "FilterDirection: infeasible point", _state);
        ae_assert(!havebndu->ptr.p_bool[i]||ae_fp_less_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]), "FilterDirection: infeasible point", _state);
        isactive = (havebndl->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]))||(havebndu->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]));
        if( isactive&&ae_fp_less_eq(ae_fabs(d->ptr.p_double[i]*s->ptr.p_double[i], _state),droptol*scalednorm) )
        {
            d->ptr.p_double[i] = 0.0;
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        ae_assert(ae_fp_greater_eq(x->ptr.p_double[nmain+i],(double)(0)), "FilterDirection: infeasible point", _state);
        if( ae_fp_eq(x->ptr.p_double[nmain+i],(double)(0))&&ae_fp_less_eq(ae_fabs(d->ptr.p_double[nmain+i]*s->ptr.p_double[nmain+i], _state),droptol*scalednorm) )
        {
            d->ptr.p_double[nmain+i] = 0.0;
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_bool statuschanged;
    ae_int_t result;


    result = 0;
    for(i=0; i<=nmain-1; i++)
    {
        if( ae_fp_neq(x->ptr.p_double[i],xprev->ptr.p_double[i]) )
        {
            statuschanged = ae_false;
            if( havebndl->ptr.p_bool[i]&&(ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i])||ae_fp_eq(xprev->ptr.p_double[i],bndl->ptr.p_double[i])) )
            {
                statuschanged = ae_true;
            }
            if( havebndu->ptr.p_bool[i]&&(ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i])||ae_fp_eq(xprev->ptr.p_double[i],bndu->ptr.p_double[i])) )
            {
                statuschanged = ae_true;
            }
            if( statuschanged )
            {
                result = result+1;
            }
        }
    }
    for(i=0; i<=nslack-1; i++)
    {
        if( ae_fp_neq(x->ptr.p_double[nmain+i],xprev->ptr.p_double[nmain+i])&&(ae_fp_eq(x->ptr.p_double[nmain+i],(double)(0))||ae_fp_eq(xprev->ptr.p_double[nmain+i],(double)(0))) )
        {
            result = result+1;
        }
    }
    return result;
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix ce;
    ae_int_t i;
    ae_int_t j;
    ae_int_t idx0;
    ae_int_t idx1;
    ae_vector permx;
    ae_vector xn;
    ae_vector xa;
    ae_vector newtonstep;
    ae_vector g;
    ae_vector pg;
    ae_vector tau;
    ae_vector s;
    double armijostep;
    double armijobeststep;
    double armijobestfeas;
    double v;
    double vv;
    double mx;
    double feaserr;
    double feaserr0;
    double feaserr1;
    double feasold;
    double feasnew;
    double pgnorm;
    double vn;
    double vd;
    double stp;
    ae_int_t vartofreeze;
    double valtofreeze;
    double maxsteplen;
    ae_bool werechangesinconstraints;
    ae_bool stage1isover;
    ae_bool converged;
    ae_vector activeconstraints;
    ae_vector tmpk;
    ae_vector colnorms;
    ae_int_t nactive;
    ae_int_t nfree;
    ae_vector p1;
    ae_vector p2;
    apbuffers buf;
    ae_int_t itscount;
    ae_int_t itswithintolerance;
    ae_int_t maxitswithintolerance;
    ae_int_t badits;
    ae_int_t maxbadits;
    ae_int_t gparuns;
    ae_int_t maxarmijoruns;
    double infeasibilityincreasetolerance;
    ae_matrix permce;
    ae_matrix q;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&ce, 0, sizeof(ce));
    memset(&permx, 0, sizeof(permx));
    memset(&xn, 0, sizeof(xn));
    memset(&xa, 0, sizeof(xa));
    memset(&newtonstep, 0, sizeof(newtonstep));
    memset(&g, 0, sizeof(g));
    memset(&pg, 0, sizeof(pg));
    memset(&tau, 0, sizeof(tau));
    memset(&s, 0, sizeof(s));
    memset(&activeconstraints, 0, sizeof(activeconstraints));
    memset(&tmpk, 0, sizeof(tmpk));
    memset(&colnorms, 0, sizeof(colnorms));
    memset(&p1, 0, sizeof(p1));
    memset(&p2, 0, sizeof(p2));
    memset(&buf, 0, sizeof(buf));
    memset(&permce, 0, sizeof(permce));
    memset(&q, 0, sizeof(q));
    ae_matrix_init_copy(&ce, _ce, _state, ae_true);
    *qpits = 0;
    *gpaits = 0;
    ae_vector_init(&permx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xn, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xa, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&newtonstep, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&g, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&pg, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tau, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&s, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&activeconstraints, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmpk, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&colnorms, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&p1, 0, DT_INT, _state, ae_true);
    ae_vector_init(&p2, 0, DT_INT, _state, ae_true);
    _apbuffers_init(&buf, _state, ae_true);
    ae_matrix_init(&permce, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&q, 0, 0, DT_REAL, _state, ae_true);

    maxitswithintolerance = 3;
    maxbadits = 3;
    maxarmijoruns = 5;
    *qpits = 0;
    *gpaits = 0;
    
    /*
     * Initial enforcement of the feasibility with respect to boundary constraints
     * NOTE: after this block we assume that boundary constraints are consistent.
     */
    if( !enforceboundaryconstraints(x, bndl, havebndl, bndu, havebndu, nmain, nslack, _state) )
    {
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    if( k==0 )
    {
        
        /*
         * No linear constraints, we can exit right now
         */
        result = ae_true;
        ae_frame_leave(_state);
        return result;
    }
    
    /*
     * Scale rows of CE in such way that max(CE[i,0..nmain+nslack-1])=1 for any i=0..k-1
     */
    for(i=0; i<=k-1; i++)
    {
        v = 0.0;
        for(j=0; j<=nmain+nslack-1; j++)
        {
            v = ae_maxreal(v, ae_fabs(ce.ptr.pp_double[i][j], _state), _state);
        }
        if( ae_fp_neq(v,(double)(0)) )
        {
            v = (double)1/v;
            ae_v_muld(&ce.ptr.pp_double[i][0], 1, ae_v_len(0,nmain+nslack), v);
        }
    }
    
    /*
     * Allocate temporaries
     */
    ae_vector_set_length(&xn, nmain+nslack, _state);
    ae_vector_set_length(&xa, nmain+nslack, _state);
    ae_vector_set_length(&permx, nmain+nslack, _state);
    ae_vector_set_length(&g, nmain+nslack, _state);
    ae_vector_set_length(&pg, nmain+nslack, _state);
    ae_vector_set_length(&tmpk, k, _state);
    ae_matrix_set_length(&permce, k, nmain+nslack, _state);
    ae_vector_set_length(&activeconstraints, nmain+nslack, _state);
    ae_vector_set_length(&newtonstep, nmain+nslack, _state);
    ae_vector_set_length(&s, nmain+nslack, _state);
    ae_vector_set_length(&colnorms, nmain+nslack, _state);
    for(i=0; i<=nmain+nslack-1; i++)
    {
        s.ptr.p_double[i] = 1.0;
        colnorms.ptr.p_double[i] = 0.0;
        for(j=0; j<=k-1; j++)
        {
            colnorms.ptr.p_double[i] = colnorms.ptr.p_double[i]+ae_sqr(ce.ptr.pp_double[j][i], _state);
        }
    }
    
    /*
     * K>0, we have linear equality constraints combined with bound constraints.
     *
     * Try to find feasible point as minimizer of the quadratic function
     *     F(x) = 0.5*||CE*x-b||^2 = 0.5*x'*(CE'*CE)*x - (b'*CE)*x + 0.5*b'*b
     * subject to boundary constraints given by BL, BU and non-negativity of
     * the slack variables. BTW, we drop constant term because it does not
     * actually influences on the solution.
     *
     * Below we will assume that K>0.
     */
    itswithintolerance = 0;
    badits = 0;
    itscount = 0;
    for(;;)
    {
        
        /*
         * Dynamically adjust infeasibility error tolerance
         */
        infeasibilityincreasetolerance = ae_maxreal(rmaxabsv(nmain+nslack, x, _state), (double)(1), _state)*(double)(1000+nmain)*ae_machineepsilon;
        
        /*
         * Stage 0: check for exact convergence
         */
        converged = ae_true;
        feaserr = optserv_feasibilityerror(&ce, x, nmain, nslack, k, &tmpk, _state);
        for(i=0; i<=k-1; i++)
        {
            
            /*
             * Calculate MX - maximum term in the left part
             *
             * Terminate if error in the right part is not greater than 100*Eps*MX.
             *
             * IMPORTANT: we must perform check for non-strict inequality, i.e. to use <= instead of <.
             *            it will allow us to easily handle situations with zero rows of CE.
             *
             * NOTE:      it is important to calculate feasibility error with dedicated
             *            function. Once we had a situation when usage of "inline" code
             *            resulted in different numerical values calculated at different
             *            parts of program for exactly same X. However, this value is
             *            essential for algorithm's ability to terminate before entering
             *            infinite loop, so reproducibility of numerical results is very
             *            important.
             */
            mx = (double)(0);
            v = -ce.ptr.pp_double[i][nmain+nslack];
            for(j=0; j<=nmain+nslack-1; j++)
            {
                mx = ae_maxreal(mx, ae_fabs(ce.ptr.pp_double[i][j]*x->ptr.p_double[j], _state), _state);
                v = v+ce.ptr.pp_double[i][j]*x->ptr.p_double[j];
            }
            converged = converged&&ae_fp_less_eq(ae_fabs(v, _state),(double)100*ae_machineepsilon*mx);
        }
        feaserr0 = feaserr;
        if( converged )
        {
            result = ae_fp_less_eq(feaserr,epsi);
            ae_frame_leave(_state);
            return result;
        }
        
        /*
         * Stage 1: equality constrained quadratic programming
         *
         * * treat active bound constraints as equality ones (constraint is considered 
         *   active when we are at the boundary, independently of the antigradient direction)
         * * calculate unrestricted Newton step to point XM (which may be infeasible)
         *   calculate MaxStepLen = largest step in direction of XM which retains feasibility.
         * * perform bounded step from X to XN:
         *   a) XN=XM                  (if XM is feasible)
         *   b) XN=X-MaxStepLen*(XM-X) (otherwise)
         * * X := XN
         * * if XM (Newton step subject to currently active constraints) was feasible, goto Stage 2
         * * repeat Stage 1
         *
         * NOTE 1: in order to solve constrained qudratic subproblem we will have to reorder
         *         variables in such way that ones corresponding to inactive constraints will
         *         be first, and active ones will be last in the list. CE and X are now
         *                                                       [ xi ]
         *         separated into two parts: CE = [CEi CEa], x = [    ], where CEi/Xi correspond
         *                                                       [ xa ]
         *         to INACTIVE constraints, and CEa/Xa correspond to the ACTIVE ones.
         *
         *         Now, instead of F=0.5*x'*(CE'*CE)*x - (b'*CE)*x + 0.5*b'*b, we have
         *         F(xi) = 0.5*(CEi*xi,CEi*xi) + (CEa*xa-b,CEi*xi) + (0.5*CEa*xa-b,CEa*xa).
         *         Here xa is considered constant, i.e. we optimize with respect to xi, leaving xa fixed.
         *
         *         We can solve it by performing SVD of CEi and calculating pseudoinverse of the
         *         Hessian matrix. Of course, we do NOT calculate pseudoinverse explicitly - we
         *         just use singular vectors to perform implicit multiplication by it.
         *
         */
        for(;;)
        {
            
            /*
             * Calculate G - gradient subject to equality constraints,
             * multiply it by inverse of the Hessian diagonal to obtain initial
             * step vector.
             *
             * Bound step subject to constraints which can be activated,
             * run Armijo search with increasing step size.
             * Search is terminated when feasibility error stops to decrease.
             *
             * NOTE: it is important to test for "stops to decrease" instead
             * of "starts to increase" in order to correctly handle cases with
             * zero CE.
             */
            armijobeststep = 0.0;
            optserv_feasibilityerrorgrad(&ce, x, nmain, nslack, k, &armijobestfeas, &g, &tmpk, _state);
            for(i=0; i<=nmain-1; i++)
            {
                if( havebndl->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]) )
                {
                    g.ptr.p_double[i] = 0.0;
                }
                if( havebndu->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]) )
                {
                    g.ptr.p_double[i] = 0.0;
                }
            }
            for(i=0; i<=nslack-1; i++)
            {
                if( ae_fp_eq(x->ptr.p_double[nmain+i],0.0) )
                {
                    g.ptr.p_double[nmain+i] = 0.0;
                }
            }
            v = 0.0;
            for(i=0; i<=nmain+nslack-1; i++)
            {
                if( ae_fp_neq(ae_sqr(colnorms.ptr.p_double[i], _state),(double)(0)) )
                {
                    newtonstep.ptr.p_double[i] = -g.ptr.p_double[i]/ae_sqr(colnorms.ptr.p_double[i], _state);
                }
                else
                {
                    newtonstep.ptr.p_double[i] = 0.0;
                }
                v = v+ae_sqr(newtonstep.ptr.p_double[i], _state);
            }
            if( ae_fp_eq(v,(double)(0)) )
            {
                
                /*
                 * Constrained gradient is zero, QP iterations are over
                 */
                break;
            }
            calculatestepbound(x, &newtonstep, 1.0, bndl, havebndl, bndu, havebndu, nmain, nslack, &vartofreeze, &valtofreeze, &maxsteplen, _state);
            if( vartofreeze>=0&&ae_fp_eq(maxsteplen,(double)(0)) )
            {
                
                /*
                 * Can not perform step, QP iterations are over
                 */
                break;
            }
            if( vartofreeze>=0 )
            {
                armijostep = ae_minreal(1.0, maxsteplen, _state);
            }
            else
            {
                armijostep = (double)(1);
            }
            for(;;)
            {
                ae_v_move(&xa.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                ae_v_addd(&xa.ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), armijostep);
                enforceboundaryconstraints(&xa, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
                feaserr = optserv_feasibilityerror(&ce, &xa, nmain, nslack, k, &tmpk, _state);
                if( ae_fp_greater_eq(feaserr,armijobestfeas) )
                {
                    break;
                }
                armijobestfeas = feaserr;
                armijobeststep = armijostep;
                armijostep = 2.0*armijostep;
            }
            ae_v_addd(&x->ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), armijobeststep);
            enforceboundaryconstraints(x, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
            
            /*
             * Determine number of active and free constraints
             */
            nactive = 0;
            for(i=0; i<=nmain-1; i++)
            {
                activeconstraints.ptr.p_double[i] = (double)(0);
                if( havebndl->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndl->ptr.p_double[i]) )
                {
                    activeconstraints.ptr.p_double[i] = (double)(1);
                }
                if( havebndu->ptr.p_bool[i]&&ae_fp_eq(x->ptr.p_double[i],bndu->ptr.p_double[i]) )
                {
                    activeconstraints.ptr.p_double[i] = (double)(1);
                }
                if( ae_fp_greater(activeconstraints.ptr.p_double[i],(double)(0)) )
                {
                    nactive = nactive+1;
                }
            }
            for(i=0; i<=nslack-1; i++)
            {
                activeconstraints.ptr.p_double[nmain+i] = (double)(0);
                if( ae_fp_eq(x->ptr.p_double[nmain+i],0.0) )
                {
                    activeconstraints.ptr.p_double[nmain+i] = (double)(1);
                }
                if( ae_fp_greater(activeconstraints.ptr.p_double[nmain+i],(double)(0)) )
                {
                    nactive = nactive+1;
                }
            }
            nfree = nmain+nslack-nactive;
            if( nfree==0 )
            {
                break;
            }
            *qpits = *qpits+1;
            
            /*
             * Reorder variables: CE is reordered to PermCE, X is reordered to PermX
             */
            tagsortbuf(&activeconstraints, nmain+nslack, &p1, &p2, &buf, _state);
            for(i=0; i<=k-1; i++)
            {
                for(j=0; j<=nmain+nslack-1; j++)
                {
                    permce.ptr.pp_double[i][j] = ce.ptr.pp_double[i][j];
                }
            }
            for(j=0; j<=nmain+nslack-1; j++)
            {
                permx.ptr.p_double[j] = x->ptr.p_double[j];
            }
            for(j=0; j<=nmain+nslack-1; j++)
            {
                if( p2.ptr.p_int[j]!=j )
                {
                    idx0 = p2.ptr.p_int[j];
                    idx1 = j;
                    for(i=0; i<=k-1; i++)
                    {
                        v = permce.ptr.pp_double[i][idx0];
                        permce.ptr.pp_double[i][idx0] = permce.ptr.pp_double[i][idx1];
                        permce.ptr.pp_double[i][idx1] = v;
                    }
                    v = permx.ptr.p_double[idx0];
                    permx.ptr.p_double[idx0] = permx.ptr.p_double[idx1];
                    permx.ptr.p_double[idx1] = v;
                }
            }
            
            /*
             * Calculate (unprojected) gradient:
             * G(xi) = CEi'*(CEi*xi + CEa*xa - b)
             */
            for(i=0; i<=nfree-1; i++)
            {
                g.ptr.p_double[i] = (double)(0);
            }
            for(i=0; i<=k-1; i++)
            {
                v = ae_v_dotproduct(&permce.ptr.pp_double[i][0], 1, &permx.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                tmpk.ptr.p_double[i] = v-ce.ptr.pp_double[i][nmain+nslack];
            }
            for(i=0; i<=k-1; i++)
            {
                v = tmpk.ptr.p_double[i];
                ae_v_addd(&g.ptr.p_double[0], 1, &permce.ptr.pp_double[i][0], 1, ae_v_len(0,nfree-1), v);
            }
            
            /*
             * Calculate Newton step using pseudoinverse PermCE:
             *     F(xi)  = 0.5*xi'*H*xi + g'*xi    (Taylor decomposition)
             *     XN     = -H^(-1)*g               (new point, solution of the QP subproblem)
             *     H      = CEi'*CEi                
             *     H^(-1) can be calculated via QR or LQ decomposition (see below)
             *     step   = -H^(-1)*g
             *
             * NOTE: PermCE is destroyed after this block
             */
            for(i=0; i<=nmain+nslack-1; i++)
            {
                newtonstep.ptr.p_double[i] = (double)(0);
            }
            if( k<=nfree )
            {
                
                /*
                 * CEi    = L*Q
                 * H      = Q'*L'*L*Q
                 * inv(H) = Q'*inv(L)*inv(L')*Q
                 *
                 * NOTE: we apply minor regularizing perturbation to diagonal of L,
                 *       which is equal to 10*K*Eps
                 */
                rmatrixlq(&permce, k, nfree, &tau, _state);
                rmatrixlqunpackq(&permce, k, nfree, &tau, k, &q, _state);
                v = (double)(0);
                for(i=0; i<=k-1; i++)
                {
                    v = ae_maxreal(v, ae_fabs(permce.ptr.pp_double[i][i], _state), _state);
                }
                v = coalesce(v, (double)(1), _state);
                for(i=0; i<=k-1; i++)
                {
                    permce.ptr.pp_double[i][i] = permce.ptr.pp_double[i][i]+(double)(10*k)*ae_machineepsilon*v;
                }
                rmatrixgemv(k, nfree, 1.0, &q, 0, 0, 0, &g, 0, 0.0, &tmpk, 0, _state);
                rmatrixtrsv(k, &permce, 0, 0, ae_false, ae_false, 1, &tmpk, 0, _state);
                rmatrixtrsv(k, &permce, 0, 0, ae_false, ae_false, 0, &tmpk, 0, _state);
                rmatrixgemv(nfree, k, -1.0, &q, 0, 0, 1, &tmpk, 0, 0.0, &newtonstep, 0, _state);
            }
            else
            {
                
                /*
                 * CEi    = Q*R
                 * H      = R'*R
                 * inv(H) = inv(R)*inv(R')
                 *
                 * NOTE: we apply minor regularizing perturbation to diagonal of R,
                 *       which is equal to 10*K*Eps
                 */
                rmatrixqr(&permce, k, nfree, &tau, _state);
                v = (double)(0);
                for(i=0; i<=nfree-1; i++)
                {
                    v = ae_maxreal(v, ae_fabs(permce.ptr.pp_double[i][i], _state), _state);
                }
                v = coalesce(v, (double)(1), _state);
                for(i=0; i<=nfree-1; i++)
                {
                    vv = (double)(10*nfree)*ae_machineepsilon*v;
                    if( ae_fp_less(permce.ptr.pp_double[i][i],(double)(0)) )
                    {
                        vv = -vv;
                    }
                    permce.ptr.pp_double[i][i] = permce.ptr.pp_double[i][i]+vv;
                }
                ae_v_moveneg(&newtonstep.ptr.p_double[0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,nfree-1));
                rmatrixtrsv(nfree, &permce, 0, 0, ae_true, ae_false, 1, &newtonstep, 0, _state);
                rmatrixtrsv(nfree, &permce, 0, 0, ae_true, ae_false, 0, &newtonstep, 0, _state);
            }
            
            /*
             * Post-reordering of Newton step
             */
            for(j=nmain+nslack-1; j>=0; j--)
            {
                if( p2.ptr.p_int[j]!=j )
                {
                    idx0 = p2.ptr.p_int[j];
                    idx1 = j;
                    v = newtonstep.ptr.p_double[idx0];
                    newtonstep.ptr.p_double[idx0] = newtonstep.ptr.p_double[idx1];
                    newtonstep.ptr.p_double[idx1] = v;
                }
            }
            
            /*
             * NewtonStep contains Newton step subject to active bound constraints.
             *
             * Such step leads us to the minimizer of the equality constrained F,
             * but such minimizer may be infeasible because some constraints which
             * are inactive at the initial point can be violated at the solution.
             *
             * Thus, we perform optimization in two stages:
             * a) perform bounded Newton step, i.e. step in the Newton direction
             *    until activation of the first constraint
             * b) in case (MaxStepLen>0)and(MaxStepLen<1), perform additional iteration
             *    of the Armijo line search in the rest of the Newton direction.
             */
            calculatestepbound(x, &newtonstep, 1.0, bndl, havebndl, bndu, havebndu, nmain, nslack, &vartofreeze, &valtofreeze, &maxsteplen, _state);
            if( vartofreeze>=0&&ae_fp_eq(maxsteplen,(double)(0)) )
            {
                
                /*
                 * Activation of the constraints prevent us from performing step,
                 * QP iterations are over
                 */
                break;
            }
            if( vartofreeze>=0 )
            {
                v = ae_minreal(1.0, maxsteplen, _state);
            }
            else
            {
                v = 1.0;
            }
            ae_v_moved(&xn.ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), v);
            ae_v_add(&xn.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            postprocessboundedstep(&xn, x, bndl, havebndl, bndu, havebndu, nmain, nslack, vartofreeze, valtofreeze, v, maxsteplen, _state);
            if( ae_fp_greater(maxsteplen,(double)(0))&&ae_fp_less(maxsteplen,(double)(1)) )
            {
                
                /*
                 * Newton step was restricted by activation of the constraints,
                 * perform Armijo iteration.
                 *
                 * Initial estimate for best step is zero step. We try different
                 * step sizes, from the 1-MaxStepLen (residual of the full Newton
                 * step) to progressively smaller and smaller steps.
                 */
                armijobeststep = 0.0;
                armijobestfeas = optserv_feasibilityerror(&ce, &xn, nmain, nslack, k, &tmpk, _state);
                armijostep = (double)1-maxsteplen;
                for(j=0; j<=maxarmijoruns-1; j++)
                {
                    ae_v_move(&xa.ptr.p_double[0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                    ae_v_addd(&xa.ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), armijostep);
                    enforceboundaryconstraints(&xa, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
                    feaserr = optserv_feasibilityerror(&ce, &xa, nmain, nslack, k, &tmpk, _state);
                    if( ae_fp_less(feaserr,armijobestfeas) )
                    {
                        armijobestfeas = feaserr;
                        armijobeststep = armijostep;
                    }
                    armijostep = 0.5*armijostep;
                }
                ae_v_move(&xa.ptr.p_double[0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
                ae_v_addd(&xa.ptr.p_double[0], 1, &newtonstep.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), armijobeststep);
                enforceboundaryconstraints(&xa, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
            }
            else
            {
                
                /*
                 * Armijo iteration is not performed
                 */
                ae_v_move(&xa.ptr.p_double[0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            }
            stage1isover = ae_fp_greater_eq(maxsteplen,(double)(1))||ae_fp_eq(maxsteplen,(double)(0));
            
            /*
             * Calculate feasibility errors for old and new X.
             * These quantinies are used for debugging purposes only.
             * However, we can leave them in release code because performance impact is insignificant.
             *
             * Update X. Exit if needed.
             */
            feasold = optserv_feasibilityerror(&ce, x, nmain, nslack, k, &tmpk, _state);
            feasnew = optserv_feasibilityerror(&ce, &xa, nmain, nslack, k, &tmpk, _state);
            if( ae_fp_greater_eq(feasnew,feasold+infeasibilityincreasetolerance) )
            {
                break;
            }
            ae_v_move(&x->ptr.p_double[0], 1, &xa.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            if( stage1isover )
            {
                break;
            }
        }
        
        /*
         * Stage 2: gradient projection algorithm (GPA)
         *
         * * calculate feasibility error (with respect to linear equality constraints)
         * * calculate gradient G of F, project it into feasible area (G => PG)
         * * exit if norm(PG) is exactly zero or feasibility error is smaller than EpsC
         * * let XM be exact minimum of F along -PG (XM may be infeasible).
         *   calculate MaxStepLen = largest step in direction of -PG which retains feasibility.
         * * perform bounded step from X to XN:
         *   a) XN=XM              (if XM is feasible)
         *   b) XN=X-MaxStepLen*PG (otherwise)
         * * X := XN
         * * stop after specified number of iterations or when no new constraints was activated
         *
         * NOTES:
         * * grad(F) = (CE'*CE)*x - (b'*CE)^T
         * * CE[i] denotes I-th row of CE
         * * XM = X+stp*(-PG) where stp=(grad(F(X)),PG)/(CE*PG,CE*PG).
         *   Here PG is a projected gradient, but in fact it can be arbitrary non-zero 
         *   direction vector - formula for minimum of F along PG still will be correct.
         */
        werechangesinconstraints = ae_false;
        for(gparuns=1; gparuns<=k; gparuns++)
        {
            
            /*
             * calculate feasibility error and G
             */
            optserv_feasibilityerrorgrad(&ce, x, nmain, nslack, k, &feaserr, &g, &tmpk, _state);
            
            /*
             * project G, filter it (strip numerical noise)
             */
            ae_v_move(&pg.ptr.p_double[0], 1, &g.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            projectgradientintobc(x, &pg, bndl, havebndl, bndu, havebndu, nmain, nslack, _state);
            filterdirection(&pg, x, bndl, havebndl, bndu, havebndu, &s, nmain, nslack, 1.0E-9, _state);
            for(i=0; i<=nmain+nslack-1; i++)
            {
                if( ae_fp_neq(ae_sqr(colnorms.ptr.p_double[i], _state),(double)(0)) )
                {
                    pg.ptr.p_double[i] = pg.ptr.p_double[i]/ae_sqr(colnorms.ptr.p_double[i], _state);
                }
                else
                {
                    pg.ptr.p_double[i] = 0.0;
                }
            }
            
            /*
             * Check GNorm and feasibility.
             * Exit when GNorm is exactly zero.
             */
            pgnorm = ae_v_dotproduct(&pg.ptr.p_double[0], 1, &pg.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            pgnorm = ae_sqrt(pgnorm, _state);
            if( ae_fp_eq(pgnorm,(double)(0)) )
            {
                result = ae_fp_less_eq(feaserr,epsi);
                ae_frame_leave(_state);
                return result;
            }
            
            /*
             * calculate planned step length
             */
            vn = ae_v_dotproduct(&g.ptr.p_double[0], 1, &pg.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            vd = (double)(0);
            rmatrixgemv(k, nmain+nslack, 1.0, &ce, 0, 0, 0, &pg, 0, 0.0, &tmpk, 0, _state);
            for(i=0; i<=k-1; i++)
            {
                vd = vd+ae_sqr(tmpk.ptr.p_double[i], _state);
            }
            stp = vn/vd;
            
            /*
             * Calculate step bound.
             * Perform bounded step and post-process it
             */
            calculatestepbound(x, &pg, -1.0, bndl, havebndl, bndu, havebndu, nmain, nslack, &vartofreeze, &valtofreeze, &maxsteplen, _state);
            if( vartofreeze>=0&&ae_fp_eq(maxsteplen,(double)(0)) )
            {
                result = ae_false;
                ae_frame_leave(_state);
                return result;
            }
            if( vartofreeze>=0 )
            {
                v = ae_minreal(stp, maxsteplen, _state);
            }
            else
            {
                v = stp;
            }
            ae_v_move(&xn.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            ae_v_subd(&xn.ptr.p_double[0], 1, &pg.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1), v);
            postprocessboundedstep(&xn, x, bndl, havebndl, bndu, havebndu, nmain, nslack, vartofreeze, valtofreeze, v, maxsteplen, _state);
            
            /*
             * update X
             * check stopping criteria
             */
            werechangesinconstraints = werechangesinconstraints||numberofchangedconstraints(&xn, x, bndl, havebndl, bndu, havebndu, nmain, nslack, _state)>0;
            ae_v_move(&x->ptr.p_double[0], 1, &xn.ptr.p_double[0], 1, ae_v_len(0,nmain+nslack-1));
            *gpaits = *gpaits+1;
            if( !werechangesinconstraints )
            {
                break;
            }
        }
        
        /*
         * Stage 3: decide to stop algorithm or not to stop
         *
         * 1. we can stop when last GPA run did NOT changed constraints status.
         *    It means that we've found final set of the active constraints even
         *    before GPA made its run. And it means that Newton step moved us to
         *    the minimum subject to the present constraints.
         *    Depending on feasibility error, True or False is returned.
         */
        feaserr = optserv_feasibilityerror(&ce, x, nmain, nslack, k, &tmpk, _state);
        feaserr1 = feaserr;
        if( ae_fp_greater_eq(feaserr1,feaserr0-infeasibilityincreasetolerance) )
        {
            inc(&badits, _state);
        }
        else
        {
            badits = 0;
        }
        if( ae_fp_less_eq(feaserr,epsi) )
        {
            inc(&itswithintolerance, _state);
        }
        else
        {
            itswithintolerance = 0;
        }
        if( (!werechangesinconstraints||itswithintolerance>=maxitswithintolerance)||badits>=maxbadits )
        {
            result = ae_fp_less_eq(feaserr,epsi);
            ae_frame_leave(_state);
            return result;
        }
        itscount = itscount+1;
        
        /*
         * Block below is never executed; it is necessary just to avoid
         * "unreachable code" warning about automatically generated code.
         *
         * We just need a way to transfer control to the end of the function,
         * even a fake way which is never actually traversed.
         */
        if( alwaysfalse(_state) )
        {
            result = ae_false;
            ae_assert(ae_false, "Assertion failed", _state);
            break;
        }
    }
    ae_frame_leave(_state);
    return result;
}


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
     ae_state *_state)
{
    double s;
    double h;
    double dh;
    ae_bool result;


    
    /*
     * Rescale input data to [0,1]
     */
    df = width*df;
    df0 = width*df0;
    df1 = width*df1;
    
    /*
     * Compute error scale, two sources are used:
     * * magnitudes of derivatives and secants
     * * magnitudes of input data times sqrt(machine_epsilon)
     */
    s = 0.0;
    s = ae_maxreal(s, ae_fabs(df0, _state), _state);
    s = ae_maxreal(s, ae_fabs(df1, _state), _state);
    s = ae_maxreal(s, ae_fabs(f1-f0, _state), _state);
    s = ae_maxreal(s, ae_sqrt(ae_machineepsilon, _state)*ae_fabs(f0, _state), _state);
    s = ae_maxreal(s, ae_sqrt(ae_machineepsilon, _state)*ae_fabs(f1, _state), _state);
    
    /*
     * Compute H and dH/dX at the middle of interval
     */
    h = 0.5*(f0+f1)+0.125*(df0-df1);
    dh = 1.5*(f1-f0)-0.250*(df0+df1);
    
    /*
     * Check
     */
    if( ae_fp_neq(s,(double)(0)) )
    {
        if( ae_fp_greater(ae_fabs(h-f, _state)/s,0.001)||ae_fp_greater(ae_fabs(dh-df, _state)/s,0.001) )
        {
            result = ae_false;
            return result;
        }
    }
    else
    {
        if( ae_fp_neq(h-f,0.0)||ae_fp_neq(dh-df,0.0) )
        {
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    return result;
}


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
     ae_state *_state)
{
    double d1esterror;
    double d2esterror;
    double eps;
    double e1;
    double e2;

    *d1est = 0;
    *d2est = 0;

    
    /*
     * Error estimates:
     *
     * * error in D1=d'*(A*x+b) is estimated as
     *   ED1 = eps*MAX_ABS(D)*(MAX_ABS(X)*ENORM(A)+MAX_ABS(B))
     * * error in D2=0.5*d'*A*d is estimated as
     *   ED2 = eps*MAX_ABS(D)^2*ENORM(A)
     *
     * Here ENORM(A) is some pseudo-norm which reflects the way numerical
     * error accumulates during addition. Two ways of accumulation are
     * possible - worst case (errors always increase) and mean-case (errors
     * may cancel each other). We calculate geometrical average of both:
     * * ENORM_WORST(A) = SUM(|A[i,j]|)         error in N-term sum grows as O(N)
     * * ENORM_MEAN(A)  = SQRT(SUM(A[i,j]^2))   error in N-term sum grows as O(sqrt(N))
     * * ENORM(A)       = SQRT(ENORM_WORST(A),ENORM_MEAN(A))
     */
    eps = (double)4*ae_machineepsilon;
    e1 = eps*md*(mx*absasum+mb);
    e2 = eps*md*(mx*ae_sqrt(absasum2, _state)+mb);
    d1esterror = ae_sqrt(e1*e2, _state);
    if( ae_fp_less_eq(ae_fabs(d1, _state),d1esterror) )
    {
        *d1est = 0;
    }
    else
    {
        *d1est = ae_sign(d1, _state);
    }
    e1 = eps*md*md*absasum;
    e2 = eps*md*md*ae_sqrt(absasum2, _state);
    d2esterror = ae_sqrt(e1*e2, _state);
    if( ae_fp_less_eq(ae_fabs(d2, _state),d2esterror) )
    {
        *d2est = 0;
    }
    else
    {
        *d2est = ae_sign(d2, _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t idx;
    ae_int_t i;
    ae_int_t j;
    double v;
    double v0;
    double v1;
    double vx;
    double vy;


    rvectorsetlengthatleast(&buf->norms, k, _state);
    rvectorsetlengthatleast(&buf->alpha, k, _state);
    rvectorsetlengthatleast(&buf->rho, k, _state);
    rmatrixsetlengthatleast(&buf->yk, k, n, _state);
    ivectorsetlengthatleast(&buf->idx, k, _state);
    
    /*
     * Check inputs
     */
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_fp_greater(d->ptr.p_double[i],(double)(0)), "InexactLBFGSPreconditioner: D[]<=0", _state);
    }
    for(i=0; i<=k-1; i++)
    {
        ae_assert(ae_fp_greater_eq(c->ptr.p_double[i],(double)(0)), "InexactLBFGSPreconditioner: C[]<0", _state);
    }
    
    /*
     * Reorder linear terms according to increase of second derivative.
     * Fill Norms[] array.
     */
    for(idx=0; idx<=k-1; idx++)
    {
        v = ae_v_dotproduct(&w->ptr.pp_double[idx][0], 1, &w->ptr.pp_double[idx][0], 1, ae_v_len(0,n-1));
        buf->norms.ptr.p_double[idx] = v*c->ptr.p_double[idx];
        buf->idx.ptr.p_int[idx] = idx;
    }
    tagsortfasti(&buf->norms, &buf->idx, &buf->bufa, &buf->bufb, k, _state);
    
    /*
     * Apply updates
     */
    for(idx=0; idx<=k-1; idx++)
    {
        
        /*
         * Select update to perform (ordered by ascending of second derivative)
         */
        i = buf->idx.ptr.p_int[idx];
        
        /*
         * Calculate YK and Rho
         */
        v = ae_v_dotproduct(&w->ptr.pp_double[i][0], 1, &w->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
        v = v*c->ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            buf->yk.ptr.pp_double[i][j] = (d->ptr.p_double[j]+v)*w->ptr.pp_double[i][j];
        }
        v = 0.0;
        v0 = 0.0;
        v1 = 0.0;
        for(j=0; j<=n-1; j++)
        {
            vx = w->ptr.pp_double[i][j];
            vy = buf->yk.ptr.pp_double[i][j];
            v = v+vx*vy;
            v0 = v0+vx*vx;
            v1 = v1+vy*vy;
        }
        if( (ae_fp_greater(v,(double)(0))&&ae_fp_greater(v0*v1,(double)(0)))&&ae_fp_greater(v/ae_sqrt(v0*v1, _state),(double)(n*10)*ae_machineepsilon) )
        {
            buf->rho.ptr.p_double[i] = (double)1/v;
        }
        else
        {
            buf->rho.ptr.p_double[i] = 0.0;
        }
    }
    for(idx=k-1; idx>=0; idx--)
    {
        
        /*
         * Select update to perform (ordered by ascending of second derivative)
         */
        i = buf->idx.ptr.p_int[idx];
        
        /*
         * Calculate Alpha[] according to L-BFGS algorithm
         * and update S[]
         */
        v = ae_v_dotproduct(&w->ptr.pp_double[i][0], 1, &s->ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = buf->rho.ptr.p_double[i]*v;
        buf->alpha.ptr.p_double[i] = v;
        ae_v_subd(&s->ptr.p_double[0], 1, &buf->yk.ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
    }
    for(j=0; j<=n-1; j++)
    {
        s->ptr.p_double[j] = s->ptr.p_double[j]/d->ptr.p_double[j];
    }
    for(idx=0; idx<=k-1; idx++)
    {
        
        /*
         * Select update to perform (ordered by ascending of second derivative)
         */
        i = buf->idx.ptr.p_int[idx];
        
        /*
         * Calculate Beta according to L-BFGS algorithm
         * and update S[]
         */
        v = ae_v_dotproduct(&buf->yk.ptr.pp_double[i][0], 1, &s->ptr.p_double[0], 1, ae_v_len(0,n-1));
        v = buf->alpha.ptr.p_double[i]-buf->rho.ptr.p_double[i]*v;
        ae_v_addd(&s->ptr.p_double[0], 1, &w->ptr.pp_double[i][0], 1, ae_v_len(0,n-1), v);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_bool b;


    
    /*
     * Check inputs
     */
    ae_assert(n>0, "PrepareLowRankPreconditioner: N<=0", _state);
    ae_assert(k>=0, "PrepareLowRankPreconditioner: N<=0", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_fp_greater(d->ptr.p_double[i],(double)(0)), "PrepareLowRankPreconditioner: D[]<=0", _state);
    }
    for(i=0; i<=k-1; i++)
    {
        ae_assert(ae_fp_greater_eq(c->ptr.p_double[i],(double)(0)), "PrepareLowRankPreconditioner: C[]<0", _state);
    }
    
    /*
     * Prepare buffer structure; skip zero entries of update.
     */
    rvectorsetlengthatleast(&buf->d, n, _state);
    rmatrixsetlengthatleast(&buf->v, k, n, _state);
    rvectorsetlengthatleast(&buf->bufc, k, _state);
    rmatrixsetlengthatleast(&buf->bufw, k+1, n, _state);
    buf->n = n;
    buf->k = 0;
    for(i=0; i<=k-1; i++)
    {
        
        /*
         * Estimate magnitude of update row; skip zero rows (either W or C are zero)
         */
        v = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = v+w->ptr.pp_double[i][j]*w->ptr.pp_double[i][j];
        }
        v = v*c->ptr.p_double[i];
        if( ae_fp_eq(v,(double)(0)) )
        {
            continue;
        }
        ae_assert(ae_fp_greater(v,(double)(0)), "PrepareLowRankPreconditioner: internal error", _state);
        
        /*
         * Copy non-zero update to buffer
         */
        buf->bufc.ptr.p_double[buf->k] = c->ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            buf->v.ptr.pp_double[buf->k][j] = w->ptr.pp_double[i][j];
            buf->bufw.ptr.pp_double[buf->k][j] = w->ptr.pp_double[i][j];
        }
        inc(&buf->k, _state);
    }
    
    /*
     * Reset K (for convenience)
     */
    k = buf->k;
    
    /*
     * Prepare diagonal factor; quick exit for K=0
     */
    for(i=0; i<=n-1; i++)
    {
        buf->d.ptr.p_double[i] = (double)1/d->ptr.p_double[i];
    }
    if( k==0 )
    {
        return;
    }
    
    /*
     * Use Woodbury matrix identity
     */
    rmatrixsetlengthatleast(&buf->bufz, k, k, _state);
    for(i=0; i<=k-1; i++)
    {
        for(j=0; j<=k-1; j++)
        {
            buf->bufz.ptr.pp_double[i][j] = 0.0;
        }
    }
    for(i=0; i<=k-1; i++)
    {
        buf->bufz.ptr.pp_double[i][i] = (double)1/buf->bufc.ptr.p_double[i];
    }
    for(j=0; j<=n-1; j++)
    {
        buf->bufw.ptr.pp_double[k][j] = (double)1/ae_sqrt(d->ptr.p_double[j], _state);
    }
    for(i=0; i<=k-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            buf->bufw.ptr.pp_double[i][j] = buf->bufw.ptr.pp_double[i][j]*buf->bufw.ptr.pp_double[k][j];
        }
    }
    rmatrixgemm(k, k, n, 1.0, &buf->bufw, 0, 0, 0, &buf->bufw, 0, 0, 1, 1.0, &buf->bufz, 0, 0, _state);
    b = spdmatrixcholeskyrec(&buf->bufz, 0, k, ae_true, &buf->tmp, _state);
    ae_assert(b, "PrepareLowRankPreconditioner: internal error (Cholesky failure)", _state);
    rmatrixlefttrsm(k, n, &buf->bufz, 0, 0, ae_true, ae_false, 1, &buf->v, 0, 0, _state);
    for(i=0; i<=k-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            buf->v.ptr.pp_double[i][j] = buf->v.ptr.pp_double[i][j]*buf->d.ptr.p_double[j];
        }
    }
}


/*************************************************************************
This function apply exact low-rank preconditioner prepared by
PrepareLowRankPreconditioner function (see its comments for more information).

  -- ALGLIB --
     Copyright 30.06.2014 by Bochkanov Sergey
*************************************************************************/
void applylowrankpreconditioner(/* Real    */ ae_vector* s,
     precbuflowrank* buf,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;
    ae_int_t j;
    double v;


    n = buf->n;
    k = buf->k;
    rvectorsetlengthatleast(&buf->tmp, n, _state);
    for(j=0; j<=n-1; j++)
    {
        buf->tmp.ptr.p_double[j] = buf->d.ptr.p_double[j]*s->ptr.p_double[j];
    }
    for(i=0; i<=k-1; i++)
    {
        v = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = v+buf->v.ptr.pp_double[i][j]*s->ptr.p_double[j];
        }
        for(j=0; j<=n-1; j++)
        {
            buf->tmp.ptr.p_double[j] = buf->tmp.ptr.p_double[j]-v*buf->v.ptr.pp_double[i][j];
        }
    }
    for(i=0; i<=n-1; i++)
    {
        s->ptr.p_double[i] = buf->tmp.ptr.p_double[i];
    }
}


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
     ae_state *_state)
{
    ae_int_t i;


    monitor->n = n;
    monitor->k = k;
    monitor->checksmoothness = checksmoothness;
    monitor->linesearchspoiled = ae_false;
    monitor->linesearchstarted = ae_false;
    monitor->linesearchinneridx = -1;
    monitor->linesearchouteridx = -1;
    monitor->enqueuedcnt = 0;
    monitor->sortedcnt = 0;
    rvectorsetlengthatleast(&monitor->s, n, _state);
    for(i=0; i<=n-1; i++)
    {
        monitor->s.ptr.p_double[i] = s->ptr.p_double[i];
    }
    monitor->nonc0currentrating = 0.0;
    monitor->nonc1currentrating = 0.0;
    optguardinitinternal(&monitor->rep, n, k, _state);
    monitor->nonc0strrating = 0.0;
    monitor->nonc0lngrating = -ae_maxrealnumber;
    monitor->nonc0strrep.positive = ae_false;
    monitor->nonc0lngrep.positive = ae_false;
    monitor->nonc1test0strrating = 0.0;
    monitor->nonc1test0lngrating = -ae_maxrealnumber;
    monitor->nonc1test0strrep.positive = ae_false;
    monitor->nonc1test0lngrep.positive = ae_false;
    monitor->nonc1test1strrating = 0.0;
    monitor->nonc1test1lngrating = -ae_maxrealnumber;
    monitor->nonc1test1strrep.positive = ae_false;
    monitor->nonc1test1lngrep.positive = ae_false;
    monitor->badgradhasxj = ae_false;
    ae_vector_set_length(&monitor->rstateg0.ia, 4+1, _state);
    ae_vector_set_length(&monitor->rstateg0.ra, 3+1, _state);
    monitor->rstateg0.stage = -1;
}


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
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;
    ae_int_t j;
    double v;


    n = monitor->n;
    k = monitor->k;
    
    /*
     * Skip if inactive or spoiled by NAN
     */
    if( !monitor->checksmoothness )
    {
        return;
    }
    v = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = 0.5*v+x->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        v = 0.5*v+fi->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            v = 0.5*v+jac->ptr.pp_double[i][j];
        }
    }
    if( !ae_isfinite(v, _state) )
    {
        monitor->linesearchspoiled = ae_true;
        return;
    }
    
    /*
     * Finalize previous line search
     */
    if( monitor->enqueuedcnt>0 )
    {
        smoothnessmonitorfinalizelinesearch(monitor, _state);
    }
    
    /*
     * Store initial point
     */
    monitor->linesearchstarted = ae_true;
    monitor->linesearchinneridx = inneriter;
    monitor->linesearchouteridx = outeriter;
    monitor->enqueuedcnt = 1;
    rvectorgrowto(&monitor->enqueuedstp, monitor->enqueuedcnt, _state);
    rvectorgrowto(&monitor->enqueuedx, monitor->enqueuedcnt*n, _state);
    rvectorgrowto(&monitor->enqueuedfunc, monitor->enqueuedcnt*k, _state);
    rmatrixgrowrowsto(&monitor->enqueuedjac, monitor->enqueuedcnt*k, n, _state);
    monitor->enqueuedstp.ptr.p_double[0] = 0.0;
    for(j=0; j<=n-1; j++)
    {
        monitor->enqueuedx.ptr.p_double[j] = x->ptr.p_double[j];
    }
    for(i=0; i<=k-1; i++)
    {
        monitor->enqueuedfunc.ptr.p_double[i] = fi->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            monitor->enqueuedjac.ptr.pp_double[i][j] = jac->ptr.pp_double[i][j];
        }
    }
    
    /*
     * Initialize sorted representation
     */
    rvectorgrowto(&monitor->sortedstp, 1, _state);
    ivectorgrowto(&monitor->sortedidx, 1, _state);
    monitor->sortedstp.ptr.p_double[0] = 0.0;
    monitor->sortedidx.ptr.p_int[0] = 0;
    monitor->sortedcnt = 1;
}


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
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;


    n = monitor->n;
    k = monitor->k;
    if( !monitor->checksmoothness )
    {
        return;
    }
    ae_assert(k==1, "SmoothnessMonitorStartLineSearch1: K<>1", _state);
    rvectorsetlengthatleast(&monitor->xu, n, _state);
    rvectorsetlengthatleast(&monitor->f0, 1, _state);
    rmatrixsetlengthatleast(&monitor->j0, 1, n, _state);
    monitor->f0.ptr.p_double[0] = f0;
    for(i=0; i<=n-1; i++)
    {
        monitor->xu.ptr.p_double[i] = x->ptr.p_double[i]*invs->ptr.p_double[i];
        monitor->j0.ptr.pp_double[0][i] = j0->ptr.p_double[i]*s->ptr.p_double[i];
    }
    smoothnessmonitorstartlinesearch(monitor, &monitor->xu, &monitor->f0, &monitor->j0, inneriter, outeriter, _state);
}


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
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t enqueuedcnt;
    ae_int_t sortedcnt;
    ae_bool hasduplicates;
    ae_int_t funcidx;
    ae_int_t stpidx;
    double f0;
    double f1;
    double f2;
    double f3;
    double f4;
    double noise0;
    double noise1;
    double noise2;
    double noise3;
    double rating;
    double lipschitz;
    double nrm;
    double lengthrating;


    n = monitor->n;
    k = monitor->k;
    
    /*
     * Skip if inactive or spoiled by NAN
     */
    if( (!monitor->checksmoothness||monitor->linesearchspoiled)||!monitor->linesearchstarted )
    {
        return;
    }
    v = stp;
    for(i=0; i<=n-1; i++)
    {
        v = 0.5*v+x->ptr.p_double[i];
    }
    for(i=0; i<=n-1; i++)
    {
        v = 0.5*v+d->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        v = 0.5*v+fi->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            v = 0.5*v+jac->ptr.pp_double[i][j];
        }
    }
    if( !ae_isfinite(v, _state) )
    {
        monitor->linesearchspoiled = ae_true;
        return;
    }
    
    /*
     * Enqueue
     */
    inc(&monitor->enqueuedcnt, _state);
    enqueuedcnt = monitor->enqueuedcnt;
    rvectorgrowto(&monitor->dcur, n, _state);
    rvectorgrowto(&monitor->enqueuedstp, enqueuedcnt, _state);
    rvectorgrowto(&monitor->enqueuedx, enqueuedcnt*n, _state);
    rvectorgrowto(&monitor->enqueuedfunc, enqueuedcnt*k, _state);
    rmatrixgrowrowsto(&monitor->enqueuedjac, enqueuedcnt*k, n, _state);
    monitor->enqueuedstp.ptr.p_double[enqueuedcnt-1] = stp;
    for(j=0; j<=n-1; j++)
    {
        monitor->dcur.ptr.p_double[j] = d->ptr.p_double[j];
    }
    for(j=0; j<=n-1; j++)
    {
        monitor->enqueuedx.ptr.p_double[(enqueuedcnt-1)*n+j] = x->ptr.p_double[j];
    }
    for(i=0; i<=k-1; i++)
    {
        monitor->enqueuedfunc.ptr.p_double[(enqueuedcnt-1)*k+i] = fi->ptr.p_double[i];
    }
    for(i=0; i<=k-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            monitor->enqueuedjac.ptr.pp_double[(enqueuedcnt-1)*k+i][j] = jac->ptr.pp_double[i][j];
        }
    }
    
    /*
     * Update sorted representation: insert to the end, reorder
     */
    sortedcnt = monitor->sortedcnt;
    hasduplicates = ae_false;
    for(i=0; i<=sortedcnt-1; i++)
    {
        hasduplicates = hasduplicates||monitor->sortedstp.ptr.p_double[i]==stp;
    }
    if( !hasduplicates )
    {
        inc(&monitor->sortedcnt, _state);
        sortedcnt = monitor->sortedcnt;
        rvectorgrowto(&monitor->sortedstp, sortedcnt, _state);
        ivectorgrowto(&monitor->sortedidx, sortedcnt, _state);
        monitor->sortedstp.ptr.p_double[sortedcnt-1] = stp;
        monitor->sortedidx.ptr.p_int[sortedcnt-1] = enqueuedcnt-1;
        for(i=sortedcnt-2; i>=0; i--)
        {
            if( monitor->sortedstp.ptr.p_double[i]<=monitor->sortedstp.ptr.p_double[i+1] )
            {
                break;
            }
            v = monitor->sortedstp.ptr.p_double[i];
            monitor->sortedstp.ptr.p_double[i] = monitor->sortedstp.ptr.p_double[i+1];
            monitor->sortedstp.ptr.p_double[i+1] = v;
            j = monitor->sortedidx.ptr.p_int[i];
            monitor->sortedidx.ptr.p_int[i] = monitor->sortedidx.ptr.p_int[i+1];
            monitor->sortedidx.ptr.p_int[i+1] = j;
        }
    }
    
    /*
     * Scan sorted representation, check for C0 and C1 continuity
     * violations.
     */
    rvectorsetlengthatleast(&monitor->f, sortedcnt, _state);
    rvectorsetlengthatleast(&monitor->g, sortedcnt*n, _state);
    for(funcidx=0; funcidx<=k-1; funcidx++)
    {
        
        /*
         * Fetch current function and its gradient to the contiguous storage
         */
        for(i=0; i<=sortedcnt-1; i++)
        {
            monitor->f.ptr.p_double[i] = monitor->enqueuedfunc.ptr.p_double[monitor->sortedidx.ptr.p_int[i]*k+funcidx];
            for(j=0; j<=n-1; j++)
            {
                monitor->g.ptr.p_double[i*n+j] = monitor->enqueuedjac.ptr.pp_double[monitor->sortedidx.ptr.p_int[i]*k+funcidx][j];
            }
        }
        
        /*
         * Check C0 continuity.
         *
         * The basis approach is that we find appropriate candidate point
         * (either a local minimum along the line - for target; or an interval
         * where function sign is changed - for constraints), calculate left
         * and right estimates of the Lipschitz constant (slopes between points
         * #0 and #1, #2 and #3), and then calculate slope between points #1 and
         * #2 and compare it with left/right estimates.
         *
         * The actual approach is a bit more complex to account for different
         * sources of numerical noise and different false positive scenarios.
         */
        if( funcidx==0 )
        {
            for(stpidx=0; stpidx<=sortedcnt-4; stpidx++)
            {
                f0 = monitor->f.ptr.p_double[stpidx+0];
                f1 = monitor->f.ptr.p_double[stpidx+1];
                f2 = monitor->f.ptr.p_double[stpidx+2];
                f3 = monitor->f.ptr.p_double[stpidx+3];
                noise0 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f0, _state), 1.0, _state);
                noise1 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f1, _state), 1.0, _state);
                noise2 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f2, _state), 1.0, _state);
                noise3 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f3, _state), 1.0, _state);
                if( !(f1<f0+(noise0+noise1)&&f1<f2) )
                {
                    continue;
                }
                optserv_testc0continuity(f0, f1, f2, f3, noise0, noise1, noise2, noise3, monitor->sortedstp.ptr.p_double[stpidx+1]-monitor->sortedstp.ptr.p_double[stpidx+0], monitor->sortedstp.ptr.p_double[stpidx+2]-monitor->sortedstp.ptr.p_double[stpidx+1], monitor->sortedstp.ptr.p_double[stpidx+3]-monitor->sortedstp.ptr.p_double[stpidx+2], ae_false, &rating, &lipschitz, _state);
                if( rating>optserv_ogminrating0 )
                {
                    
                    /*
                     * Store to total report
                     */
                    monitor->rep.nonc0suspected = ae_true;
                    monitor->rep.nonc0test0positive = ae_true;
                    if( rating>monitor->nonc0currentrating )
                    {
                        monitor->nonc0currentrating = rating;
                        monitor->rep.nonc0lipschitzc = lipschitz;
                        monitor->rep.nonc0fidx = funcidx;
                    }
                    
                    /*
                     * Store to "strongest" report
                     */
                    if( rating>monitor->nonc0strrating )
                    {
                        monitor->nonc0strrating = rating;
                        monitor->nonc0strrep.positive = ae_true;
                        monitor->nonc0strrep.fidx = funcidx;
                        monitor->nonc0strrep.n = n;
                        monitor->nonc0strrep.cnt = sortedcnt;
                        monitor->nonc0strrep.stpidxa = stpidx+0;
                        monitor->nonc0strrep.stpidxb = stpidx+3;
                        monitor->nonc0strrep.inneriter = monitor->linesearchinneridx;
                        monitor->nonc0strrep.outeriter = monitor->linesearchouteridx;
                        rvectorsetlengthatleast(&monitor->nonc0strrep.x0, n, _state);
                        rvectorsetlengthatleast(&monitor->nonc0strrep.d, n, _state);
                        for(i=0; i<=n-1; i++)
                        {
                            monitor->nonc0strrep.x0.ptr.p_double[i] = monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i];
                            monitor->nonc0strrep.d.ptr.p_double[i] = monitor->dcur.ptr.p_double[i];
                        }
                        rvectorsetlengthatleast(&monitor->nonc0strrep.stp, sortedcnt, _state);
                        rvectorsetlengthatleast(&monitor->nonc0strrep.f, sortedcnt, _state);
                        for(i=0; i<=sortedcnt-1; i++)
                        {
                            monitor->nonc0strrep.stp.ptr.p_double[i] = monitor->sortedstp.ptr.p_double[i];
                            monitor->nonc0strrep.f.ptr.p_double[i] = monitor->f.ptr.p_double[i];
                        }
                    }
                    
                    /*
                     * Store to "longest" report
                     */
                    nrm = (double)(0);
                    for(i=0; i<=n-1; i++)
                    {
                        nrm = nrm+ae_sqr(monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i]-monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[sortedcnt-1]*n+i], _state);
                    }
                    nrm = ae_sqrt(nrm, _state);
                    nrm = ae_minreal(nrm, 1.0, _state);
                    nrm = coalesce(nrm, ae_machineepsilon, _state);
                    lengthrating = (double)sortedcnt+ae_log(nrm, _state)/ae_log((double)(100), _state);
                    if( lengthrating>monitor->nonc0lngrating )
                    {
                        monitor->nonc0lngrating = lengthrating;
                        monitor->nonc0lngrep.positive = ae_true;
                        monitor->nonc0lngrep.fidx = funcidx;
                        monitor->nonc0lngrep.n = n;
                        monitor->nonc0lngrep.cnt = sortedcnt;
                        monitor->nonc0lngrep.stpidxa = stpidx+0;
                        monitor->nonc0lngrep.stpidxb = stpidx+3;
                        monitor->nonc0lngrep.inneriter = monitor->linesearchinneridx;
                        monitor->nonc0lngrep.outeriter = monitor->linesearchouteridx;
                        rvectorsetlengthatleast(&monitor->nonc0lngrep.x0, n, _state);
                        rvectorsetlengthatleast(&monitor->nonc0lngrep.d, n, _state);
                        for(i=0; i<=n-1; i++)
                        {
                            monitor->nonc0lngrep.x0.ptr.p_double[i] = monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i];
                            monitor->nonc0lngrep.d.ptr.p_double[i] = monitor->dcur.ptr.p_double[i];
                        }
                        rvectorsetlengthatleast(&monitor->nonc0lngrep.stp, sortedcnt, _state);
                        rvectorsetlengthatleast(&monitor->nonc0lngrep.f, sortedcnt, _state);
                        for(i=0; i<=sortedcnt-1; i++)
                        {
                            monitor->nonc0lngrep.stp.ptr.p_double[i] = monitor->sortedstp.ptr.p_double[i];
                            monitor->nonc0lngrep.f.ptr.p_double[i] = monitor->f.ptr.p_double[i];
                        }
                    }
                }
            }
        }
        
        /*
         * C1 continuity test #0
         */
        for(stpidx=0; stpidx<=sortedcnt-7; stpidx++)
        {
            
            /*
             * Fetch function values
             */
            f2 = monitor->f.ptr.p_double[stpidx+2];
            f3 = monitor->f.ptr.p_double[stpidx+3];
            f4 = monitor->f.ptr.p_double[stpidx+4];
            noise2 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f2, _state), 1.0, _state);
            noise3 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f3, _state), 1.0, _state);
            
            /*
             * Decide whether we want to test this interval or not; for target
             * function we test intervals around minimum, for constraints we
             * additionally test intervals of sign change.
             */
            if( funcidx==0 )
            {
                
                /*
                 * Target function: skip if not minimum
                 */
                if( !(f3<f2+(noise2+noise3)&&f3<f4) )
                {
                    continue;
                }
            }
            else
            {
                
                /*
                 * Constraint: skip if both (a) sign does not change, and (b) is not minumum
                 */
                if( ae_sign(f2*f4, _state)>0&&!(f3<f2+(noise2+noise3)&&f3<f4) )
                {
                    continue;
                }
            }
            optserv_c1continuitytest0(monitor, funcidx, stpidx+0, sortedcnt, _state);
            optserv_c1continuitytest0(monitor, funcidx, stpidx+1, sortedcnt, _state);
        }
        
        /*
         * C1 continuity test #1
         */
        for(stpidx=0; stpidx<=sortedcnt-4; stpidx++)
        {
            
            /*
             * Fetch function values from the interval being tested
             */
            f0 = monitor->f.ptr.p_double[stpidx+0];
            f1 = monitor->f.ptr.p_double[stpidx+1];
            f2 = monitor->f.ptr.p_double[stpidx+2];
            f3 = monitor->f.ptr.p_double[stpidx+3];
            noise0 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f0, _state), 1.0, _state);
            noise1 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f1, _state), 1.0, _state);
            noise2 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f2, _state), 1.0, _state);
            noise3 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f3, _state), 1.0, _state);
            
            /*
             * Decide whether we want to test this interval or not; for target
             * function we test intervals around minimum, for constraints we
             * additionally test intervals of sign change.
             */
            if( funcidx==0 )
            {
                
                /*
                 * Skip if not minimum
                 */
                if( !(f1<f0+(noise0+noise1)&&f2<f3+noise2+noise3) )
                {
                    continue;
                }
            }
            else
            {
                
                /*
                 * Skip if sign does not change
                 */
                if( ae_sign(f0*f3, _state)>0&&!(f1<f0+(noise0+noise1)&&f2<f3+noise2+noise3) )
                {
                    continue;
                }
            }
            optserv_c1continuitytest1(monitor, funcidx, stpidx, sortedcnt, _state);
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;


    n = monitor->n;
    k = monitor->k;
    if( !monitor->checksmoothness )
    {
        return;
    }
    ae_assert(k==1, "SmoothnessMonitorEnqueuePoint1: K<>1", _state);
    rvectorsetlengthatleast(&monitor->xu, n, _state);
    rvectorsetlengthatleast(&monitor->du, n, _state);
    rvectorsetlengthatleast(&monitor->f0, 1, _state);
    rmatrixsetlengthatleast(&monitor->j0, 1, n, _state);
    monitor->f0.ptr.p_double[0] = f0;
    for(i=0; i<=n-1; i++)
    {
        monitor->xu.ptr.p_double[i] = x->ptr.p_double[i]*invs->ptr.p_double[i];
        monitor->du.ptr.p_double[i] = d->ptr.p_double[i]*invs->ptr.p_double[i];
        monitor->j0.ptr.pp_double[0][i] = j0->ptr.p_double[i]*s->ptr.p_double[i];
    }
    smoothnessmonitorenqueuepoint(monitor, &monitor->du, stp, &monitor->xu, &monitor->f0, &monitor->j0, _state);
}


/*************************************************************************
This subroutine finalizes line search

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorfinalizelinesearch(smoothnessmonitor* monitor,
     ae_state *_state)
{


    
    /*
     * As for now - nothing to be done.
     */
}


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
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;


    n = monitor->n;
    k = monitor->k;
    ae_assert(isfinitevector(x, n, _state), "SmoothnessMonitorStartLagrangianProbing: bad X[] array", _state);
    ae_assert(isfinitevector(d, n, _state), "SmoothnessMonitorStartLagrangianProbing: bad D[] array", _state);
    ae_assert(ae_isfinite(stpmax, _state)&&ae_fp_greater(stpmax,(double)(0)), "SmoothnessMonitorStartLagrangianProbing: StpMax<=0", _state);
    ae_assert(k>=1, "SmoothnessMonitorStartLagrangianProbing: monitor object is initialized with K<=0", _state);
    monitor->lagprobnstepsstored = 0;
    monitor->lagprobstepmax = stpmax;
    monitor->lagprobinneriter = inneriter;
    monitor->lagprobouteriter = outeriter;
    rvectorsetlengthatleast(&monitor->lagprobxs, n, _state);
    rvectorsetlengthatleast(&monitor->lagprobd, n, _state);
    for(i=0; i<=n-1; i++)
    {
        monitor->lagprobxs.ptr.p_double[i] = x->ptr.p_double[i];
        monitor->lagprobd.ptr.p_double[i] = d->ptr.p_double[i];
    }
    rvectorsetlengthatleast(&monitor->lagprobx, n, _state);
    rvectorsetlengthatleast(&monitor->lagprobfi, k, _state);
    rmatrixsetlengthatleast(&monitor->lagprobj, k, n, _state);
    ae_vector_set_length(&monitor->lagrangianprobingrcomm.ia, 3+1, _state);
    ae_vector_set_length(&monitor->lagrangianprobingrcomm.ra, 4+1, _state);
    monitor->lagrangianprobingrcomm.stage = -1;
}


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
     ae_state *_state)
{
    ae_int_t stpidx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t idx;
    double stp;
    double vlargest;
    double v;
    double v0;
    double v1;
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
    if( monitor->lagrangianprobingrcomm.stage>=0 )
    {
        stpidx = monitor->lagrangianprobingrcomm.ia.ptr.p_int[0];
        i = monitor->lagrangianprobingrcomm.ia.ptr.p_int[1];
        j = monitor->lagrangianprobingrcomm.ia.ptr.p_int[2];
        idx = monitor->lagrangianprobingrcomm.ia.ptr.p_int[3];
        stp = monitor->lagrangianprobingrcomm.ra.ptr.p_double[0];
        vlargest = monitor->lagrangianprobingrcomm.ra.ptr.p_double[1];
        v = monitor->lagrangianprobingrcomm.ra.ptr.p_double[2];
        v0 = monitor->lagrangianprobingrcomm.ra.ptr.p_double[3];
        v1 = monitor->lagrangianprobingrcomm.ra.ptr.p_double[4];
    }
    else
    {
        stpidx = 359;
        i = -58;
        j = -919;
        idx = -909;
        stp = 81.0;
        vlargest = 255.0;
        v = 74.0;
        v0 = -788.0;
        v1 = 809.0;
    }
    if( monitor->lagrangianprobingrcomm.stage==0 )
    {
        goto lbl_0;
    }
    
    /*
     * Routine body
     */
    stpidx = 0;
lbl_1:
    if( stpidx>40 )
    {
        goto lbl_3;
    }
    
    /*
     * Increase storage size
     */
    rvectorgrowto(&monitor->lagprobsteps, monitor->lagprobnstepsstored+1, _state);
    rvectorgrowto(&monitor->lagproblagrangians, monitor->lagprobnstepsstored+1, _state);
    rmatrixgrowrowsto(&monitor->lagprobvalues, monitor->lagprobnstepsstored+1, monitor->k, _state);
    rmatrixgrowrowsto(&monitor->lagprobjacobians, monitor->lagprobnstepsstored+1, monitor->n*monitor->k, _state);
    
    /*
     * Determine probing step length, save step to the end of the storage
     */
    if( stpidx<=10 )
    {
        
        /*
         * First 11 steps are performed over equidistant grid
         */
        stp = (double)stpidx/(double)10*monitor->lagprobstepmax;
    }
    else
    {
        
        /*
         * Subsequent steps target interesting points
         */
        ae_assert(monitor->lagprobnstepsstored>=3, "SMonitor: critical integrity check failed", _state);
        stp = (double)(0);
        if( stpidx%3==0 )
        {
            
            /*
             * Target interval with maximum change in Lagrangian
             */
            idx = -1;
            vlargest = (double)(0);
            for(j=0; j<=monitor->lagprobnstepsstored-2; j++)
            {
                v = ae_fabs(monitor->lagproblagrangians.ptr.p_double[j+1]-monitor->lagproblagrangians.ptr.p_double[j], _state);
                if( idx<0||ae_fp_greater(v,vlargest) )
                {
                    idx = j;
                    vlargest = v;
                }
            }
            stp = 0.5*(monitor->lagprobsteps.ptr.p_double[idx]+monitor->lagprobsteps.ptr.p_double[idx+1]);
        }
        if( stpidx%3==1 )
        {
            
            /*
             * Target interval [J,J+2] with maximum change in slope of Lagrangian,
             * select subinterval [J,J+1] or [J+1,J+2] (one with maximum length).
             */
            idx = -1;
            vlargest = (double)(0);
            for(j=0; j<=monitor->lagprobnstepsstored-3; j++)
            {
                v0 = (monitor->lagproblagrangians.ptr.p_double[j+1]-monitor->lagproblagrangians.ptr.p_double[j+0])/(monitor->lagprobsteps.ptr.p_double[j+1]-monitor->lagprobsteps.ptr.p_double[j+0]+ae_machineepsilon);
                v1 = (monitor->lagproblagrangians.ptr.p_double[j+2]-monitor->lagproblagrangians.ptr.p_double[j+1])/(monitor->lagprobsteps.ptr.p_double[j+2]-monitor->lagprobsteps.ptr.p_double[j+1]+ae_machineepsilon);
                v = ae_fabs(v0-v1, _state);
                if( idx<0||ae_fp_greater(v,vlargest) )
                {
                    idx = j;
                    vlargest = v;
                }
            }
            if( ae_fp_greater(monitor->lagprobsteps.ptr.p_double[idx+2]-monitor->lagprobsteps.ptr.p_double[idx+1],monitor->lagprobsteps.ptr.p_double[idx+1]-monitor->lagprobsteps.ptr.p_double[idx+0]) )
            {
                stp = 0.5*(monitor->lagprobsteps.ptr.p_double[idx+2]+monitor->lagprobsteps.ptr.p_double[idx+1]);
            }
            else
            {
                stp = 0.5*(monitor->lagprobsteps.ptr.p_double[idx+1]+monitor->lagprobsteps.ptr.p_double[idx+0]);
            }
        }
        if( stpidx%3==2 )
        {
            
            /*
             * Target interval with maximum change in sum of squared Jacobian differences
             */
            idx = -1;
            vlargest = (double)(0);
            for(j=0; j<=monitor->lagprobnstepsstored-2; j++)
            {
                v = (double)(0);
                for(i=0; i<=monitor->k*monitor->n-1; i++)
                {
                    v = v+ae_sqr(monitor->lagprobjacobians.ptr.pp_double[j+1][i]-monitor->lagprobjacobians.ptr.pp_double[j][i], _state);
                }
                if( idx<0||ae_fp_greater(v,vlargest) )
                {
                    idx = j;
                    vlargest = v;
                }
            }
            stp = 0.5*(monitor->lagprobsteps.ptr.p_double[idx]+monitor->lagprobsteps.ptr.p_double[idx+1]);
        }
    }
    monitor->lagprobsteps.ptr.p_double[monitor->lagprobnstepsstored] = stp;
    
    /*
     * Retrieve user values
     */
    for(i=0; i<=monitor->n-1; i++)
    {
        monitor->lagprobx.ptr.p_double[i] = monitor->lagprobxs.ptr.p_double[i]+monitor->lagprobd.ptr.p_double[i]*stp;
    }
    monitor->lagprobstp = stp;
    monitor->lagrangianprobingrcomm.stage = 0;
    goto lbl_rcomm;
lbl_0:
    for(i=0; i<=monitor->k-1; i++)
    {
        monitor->lagprobvalues.ptr.pp_double[monitor->lagprobnstepsstored][i] = monitor->lagprobfi.ptr.p_double[i];
        for(j=0; j<=monitor->n-1; j++)
        {
            monitor->lagprobjacobians.ptr.pp_double[monitor->lagprobnstepsstored][i*monitor->n+j] = monitor->lagprobj.ptr.pp_double[i][j];
        }
    }
    monitor->lagproblagrangians.ptr.p_double[monitor->lagprobnstepsstored] = monitor->lagprobrawlag;
    inc(&monitor->lagprobnstepsstored, _state);
    if( stpidx==0 )
    {
        ae_assert(ae_fp_eq(stp,(double)(0)), "SmoothnessMonitorProbeLagrangian: integrity check failed", _state);
        smoothnessmonitorstartlinesearch(monitor, &monitor->lagprobx, &monitor->lagprobfi, &monitor->lagprobj, monitor->lagprobinneriter, monitor->lagprobouteriter, _state);
    }
    else
    {
        smoothnessmonitorenqueuepoint(monitor, &monitor->lagprobd, stp, &monitor->lagprobx, &monitor->lagprobfi, &monitor->lagprobj, _state);
    }
    
    /*
     * Resort
     */
    for(j=monitor->lagprobnstepsstored-1; j>=1; j--)
    {
        if( ae_fp_less_eq(monitor->lagprobsteps.ptr.p_double[j-1],monitor->lagprobsteps.ptr.p_double[j]) )
        {
            break;
        }
        swapelements(&monitor->lagprobsteps, j-1, j, _state);
        swapelements(&monitor->lagproblagrangians, j-1, j, _state);
        swaprows(&monitor->lagprobvalues, j-1, j, monitor->k, _state);
        swaprows(&monitor->lagprobjacobians, j-1, j, monitor->n*monitor->k, _state);
    }
    stpidx = stpidx+1;
    goto lbl_1;
lbl_3:
    smoothnessmonitorfinalizelinesearch(monitor, _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    monitor->lagrangianprobingrcomm.ia.ptr.p_int[0] = stpidx;
    monitor->lagrangianprobingrcomm.ia.ptr.p_int[1] = i;
    monitor->lagrangianprobingrcomm.ia.ptr.p_int[2] = j;
    monitor->lagrangianprobingrcomm.ia.ptr.p_int[3] = idx;
    monitor->lagrangianprobingrcomm.ra.ptr.p_double[0] = stp;
    monitor->lagrangianprobingrcomm.ra.ptr.p_double[1] = vlargest;
    monitor->lagrangianprobingrcomm.ra.ptr.p_double[2] = v;
    monitor->lagrangianprobingrcomm.ra.ptr.p_double[3] = v0;
    monitor->lagrangianprobingrcomm.ra.ptr.p_double[4] = v1;
    return result;
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    double steplen;
    double mxd;
    ae_vector lagrangianslopes;
    ae_vector targetslopes;

    ae_frame_make(_state, &_frame_block);
    memset(&lagrangianslopes, 0, sizeof(lagrangianslopes));
    memset(&targetslopes, 0, sizeof(targetslopes));
    ae_vector_init(&lagrangianslopes, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&targetslopes, 0, DT_REAL, _state, ae_true);

    
    /*
     * Compute slopes
     */
    ae_assert(monitor->lagprobnstepsstored>=2, "SmoothnessMonitorTraceLagrangianProbingResults: less than 2 probing steps", _state);
    ae_vector_set_length(&lagrangianslopes, monitor->lagprobnstepsstored, _state);
    ae_vector_set_length(&targetslopes, monitor->lagprobnstepsstored, _state);
    mxd = (double)(0);
    for(i=0; i<=monitor->n-1; i++)
    {
        mxd = ae_maxreal(mxd, ae_fabs(monitor->lagprobd.ptr.p_double[i], _state), _state);
    }
    for(i=0; i<=monitor->lagprobnstepsstored-2; i++)
    {
        steplen = monitor->lagprobsteps.ptr.p_double[i+1]-monitor->lagprobsteps.ptr.p_double[i]+(double)100*ae_machineepsilon;
        steplen = steplen*(mxd+(double)100*ae_machineepsilon);
        lagrangianslopes.ptr.p_double[i] = (monitor->lagproblagrangians.ptr.p_double[i+1]-monitor->lagproblagrangians.ptr.p_double[i])/steplen;
        targetslopes.ptr.p_double[i] = (monitor->lagprobvalues.ptr.pp_double[i+1][0]-monitor->lagprobvalues.ptr.pp_double[i][0])/steplen;
    }
    lagrangianslopes.ptr.p_double[monitor->lagprobnstepsstored-1] = lagrangianslopes.ptr.p_double[monitor->lagprobnstepsstored-2];
    targetslopes.ptr.p_double[monitor->lagprobnstepsstored-1] = targetslopes.ptr.p_double[monitor->lagprobnstepsstored-2];
    
    /*
     * Print to trace log
     */
    ae_trace("*** ------------------------------------------------------------\n");
    for(i=0; i<=monitor->lagprobnstepsstored-1; i++)
    {
        ae_trace("*** | %0.4f |",
            (double)(monitor->lagprobsteps.ptr.p_double[i]));
        ae_trace(" %11.3e %10.2e |",
            (double)(monitor->lagproblagrangians.ptr.p_double[i]-monitor->lagproblagrangians.ptr.p_double[0]),
            (double)(lagrangianslopes.ptr.p_double[i]));
        ae_trace(" %11.3e %10.2e |",
            (double)(monitor->lagprobvalues.ptr.pp_double[i][0]-monitor->lagprobvalues.ptr.pp_double[0][0]),
            (double)(targetslopes.ptr.p_double[i]));
        ae_trace("\n");
    }
    ae_trace("*** ------------------------------------------------------------\n");
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_bool needreport;
    ae_bool needxdreport;
    ae_bool suspicionsraised;
    ae_int_t i;
    double slope;


    
    /*
     * Do we need trace report?
     */
    suspicionsraised = (monitor->rep.nonc0suspected||monitor->rep.nonc1suspected)||monitor->rep.badgradsuspected;
    needreport = ae_false;
    needreport = needreport||callersuggeststrace;
    needreport = needreport||ae_is_trace_enabled("OPTGUARD.ALWAYS");
    needreport = needreport||(ae_is_trace_enabled("OPTGUARD")&&suspicionsraised);
    if( !needreport )
    {
        return;
    }
    needxdreport = needreport&&ae_is_trace_enabled("OPTIMIZERS.X");
    
    /*
     *
     */
    ae_trace("\n");
    ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
    ae_trace("// OPTGUARD INTEGRITY CHECKER REPORT                                                              //\n");
    ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
    if( !suspicionsraised )
    {
        ae_trace("> no discontinuity/nonsmoothness/bad-gradient suspicions were raised during optimization\n");
        return;
    }
    if( monitor->rep.nonc0suspected )
    {
        ae_trace("> [WARNING] suspected discontinuity (aka C0-discontinuity)\n");
    }
    if( monitor->rep.nonc1suspected )
    {
        ae_trace("> [WARNING] suspected nonsmoothness (aka C1-discontinuity)\n");
    }
    ae_trace("> printing out test reports...\n");
    if( monitor->rep.nonc0suspected&&monitor->rep.nonc0test0positive )
    {
        ae_trace("> printing out discontinuity test #0 report:\n");
        ae_trace("*** -------------------------------------------------------\n");
        ae_trace("*** | Test #0 for discontinuity was triggered  (this test |\n");
        ae_trace("*** | analyzes changes in function values).               |\n");
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Function information:                               |\n");
        ae_trace("*** | * function index:       %10d",
            (int)(monitor->nonc0lngrep.fidx));
        if( monitor->nonc0lngrep.fidx==0 )
        {
            ae_trace(" (target)         |\n");
        }
        else
        {
            ae_trace(" (constraint)     |\n");
        }
        ae_trace("*** | * F() Lipschitz const:  %10.2e                  |\n",
            (double)(monitor->rep.nonc0lipschitzc));
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Iteration information:                              |\n");
        if( monitor->nonc0lngrep.inneriter>=0 )
        {
            ae_trace("*** | * inner iter idx:       %10d                  |\n",
                (int)(monitor->nonc0lngrep.inneriter));
        }
        if( monitor->nonc0lngrep.outeriter>=0 )
        {
            ae_trace("*** | * outer iter idx:       %10d                  |\n",
                (int)(monitor->nonc0lngrep.outeriter));
        }
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Printing out log of suspicious line search XK+Stp*D |\n");
        ae_trace("*** | Look for abrupt changes in slope.                   |\n");
        if( !needxdreport )
        {
            ae_trace("*** | NOTE: XK and D are  not printed  by default. If you |\n");
            ae_trace("*** |       need them, add trace tag OPTIMIZERS.X         |\n");
        }
        ae_trace("*** -------------------------------------------------------\n");
        ae_trace("*** |  step along D   |     delta F     |      slope      |\n");
        ae_trace("*** ------------------------------------------------------|\n");
        for(i=0; i<=monitor->nonc0lngrep.cnt-1; i++)
        {
            slope = monitor->nonc0lngrep.f.ptr.p_double[ae_minint(i+1, monitor->nonc0lngrep.cnt-1, _state)]-monitor->nonc0lngrep.f.ptr.p_double[i];
            slope = slope/(1.0e-15+monitor->nonc0lngrep.stp.ptr.p_double[ae_minint(i+1, monitor->nonc0lngrep.cnt-1, _state)]-monitor->nonc0lngrep.stp.ptr.p_double[i]);
            ae_trace("*** |  %13.5e  |  %13.5e  |   %11.3e   |",
                (double)(monitor->nonc0lngrep.stp.ptr.p_double[i]),
                (double)(monitor->nonc0lngrep.f.ptr.p_double[i]-monitor->nonc0lngrep.f.ptr.p_double[0]),
                (double)(slope));
            if( i>=monitor->nonc0lngrep.stpidxa&&i<=monitor->nonc0lngrep.stpidxb )
            {
                ae_trace(" <---");
            }
            ae_trace("\n");
        }
        ae_trace("*** ------------------------------------------------------|\n");
        if( needxdreport )
        {
            ae_trace("*** > printing raw variables\n");
            ae_trace("*** XK = ");
            tracevectorunscaledunshiftedautoprec(&monitor->nonc0lngrep.x0, monitor->n, &monitor->s, ae_true, &monitor->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("*** D  = ");
            tracevectorunscaledunshiftedautoprec(&monitor->nonc0lngrep.d, monitor->n, &monitor->s, ae_true, &monitor->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("*** > printing scaled variables (values are divided by user-specified scales)\n");
            ae_trace("*** XK = ");
            tracevectorautoprec(&monitor->nonc0lngrep.x0, 0, monitor->n, _state);
            ae_trace("\n");
            ae_trace("*** D  = ");
            tracevectorautoprec(&monitor->nonc0lngrep.d, 0, monitor->n, _state);
            ae_trace("\n");
        }
    }
    if( monitor->rep.nonc1suspected&&monitor->rep.nonc1test0positive )
    {
        ae_trace("> printing out nonsmoothness test #0 report:\n");
        ae_trace("*** -------------------------------------------------------\n");
        ae_trace("*** | Test #0 for nonsmoothness was triggered  (this test |\n");
        ae_trace("*** | analyzes changes in  function  values  and  ignores |\n");
        ae_trace("*** | gradient info).                                     |\n");
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Function information:                               |\n");
        ae_trace("*** | * function index:         %10d",
            (int)(monitor->nonc1test0lngrep.fidx));
        if( monitor->nonc1test0lngrep.fidx==0 )
        {
            ae_trace(" (target)       |\n");
        }
        else
        {
            ae_trace(" (constraint)   |\n");
        }
        ae_trace("*** | * dF/dX Lipschitz const:  %10.2e                |\n",
            (double)(monitor->rep.nonc1lipschitzc));
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Iteration information:                              |\n");
        if( monitor->nonc1test0lngrep.inneriter>=0 )
        {
            ae_trace("*** | * inner iter idx:       %10d                  |\n",
                (int)(monitor->nonc1test0lngrep.inneriter));
        }
        if( monitor->nonc1test0lngrep.outeriter>=0 )
        {
            ae_trace("*** | * outer iter idx:       %10d                  |\n",
                (int)(monitor->nonc1test0lngrep.outeriter));
        }
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Printing out log of suspicious line search XK+Stp*D |\n");
        ae_trace("*** | Look for abrupt changes in slope.                   |\n");
        if( !needxdreport )
        {
            ae_trace("*** | NOTE: XK and D are  not printed  by default. If you |\n");
            ae_trace("*** |       need them, add trace tag OPTIMIZERS.X         |\n");
        }
        ae_trace("*** -------------------------------------------------------\n");
        ae_trace("*** |  step along D   |     delta F     |      slope      |\n");
        ae_trace("*** ------------------------------------------------------|\n");
        for(i=0; i<=monitor->nonc1test0lngrep.cnt-1; i++)
        {
            slope = monitor->nonc1test0lngrep.f.ptr.p_double[ae_minint(i+1, monitor->nonc1test0lngrep.cnt-1, _state)]-monitor->nonc1test0lngrep.f.ptr.p_double[i];
            slope = slope/(1.0e-15+monitor->nonc1test0lngrep.stp.ptr.p_double[ae_minint(i+1, monitor->nonc1test0lngrep.cnt-1, _state)]-monitor->nonc1test0lngrep.stp.ptr.p_double[i]);
            ae_trace("*** |  %13.5e  |  %13.5e  |   %11.3e   |",
                (double)(monitor->nonc1test0lngrep.stp.ptr.p_double[i]),
                (double)(monitor->nonc1test0lngrep.f.ptr.p_double[i]-monitor->nonc1test0lngrep.f.ptr.p_double[0]),
                (double)(slope));
            if( i>=monitor->nonc1test0lngrep.stpidxa&&i<=monitor->nonc1test0lngrep.stpidxb )
            {
                ae_trace(" <---");
            }
            ae_trace("\n");
        }
        ae_trace("*** ------------------------------------------------------|\n");
        if( needxdreport )
        {
            ae_trace("*** > printing raw variables\n");
            ae_trace("*** XK = ");
            tracevectorunscaledunshiftedautoprec(&monitor->nonc1test0lngrep.x0, monitor->n, &monitor->s, ae_true, &monitor->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("*** D  = ");
            tracevectorunscaledunshiftedautoprec(&monitor->nonc1test0lngrep.d, monitor->n, &monitor->s, ae_true, &monitor->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("*** > printing scaled variables (values are divided by user-specified scales)\n");
            ae_trace("*** XK = ");
            tracevectorautoprec(&monitor->nonc1test0lngrep.x0, 0, monitor->n, _state);
            ae_trace("\n");
            ae_trace("*** D  = ");
            tracevectorautoprec(&monitor->nonc1test0lngrep.d, 0, monitor->n, _state);
            ae_trace("\n");
        }
    }
    if( monitor->rep.nonc1suspected&&monitor->rep.nonc1test1positive )
    {
        ae_trace("> printing out nonsmoothness test #1 report:\n");
        ae_trace("*** -------------------------------------------------------\n");
        ae_trace("*** | Test #1 for nonsmoothness was triggered  (this test |\n");
        ae_trace("*** | analyzes changes in gradient components).           |\n");
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Function information:                               |\n");
        ae_trace("*** | * function index:         %10d",
            (int)(monitor->nonc1test1lngrep.fidx));
        if( monitor->nonc1test1lngrep.fidx==0 )
        {
            ae_trace(" (target)       |\n");
        }
        else
        {
            ae_trace(" (constraint)   |\n");
        }
        ae_trace("*** | * variable index I:       %10d                |\n",
            (int)(monitor->nonc1test1lngrep.vidx));
        ae_trace("*** | * dF/dX Lipschitz const:  %10.2e                |\n",
            (double)(monitor->rep.nonc1lipschitzc));
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Iteration information:                              |\n");
        if( monitor->nonc1test1lngrep.inneriter>=0 )
        {
            ae_trace("*** | * inner iter idx:       %10d                  |\n",
                (int)(monitor->nonc1test1lngrep.inneriter));
        }
        if( monitor->nonc1test1lngrep.outeriter>=0 )
        {
            ae_trace("*** | * outer iter idx:       %10d                  |\n",
                (int)(monitor->nonc1test1lngrep.outeriter));
        }
        ae_trace("*** |                                                     |\n");
        ae_trace("*** | Printing out log of suspicious line search XK+Stp*D |\n");
        ae_trace("*** | Look for abrupt changes in slope.                   |\n");
        if( !needxdreport )
        {
            ae_trace("*** | NOTE: XK and D are  not printed  by default. If you |\n");
            ae_trace("*** |       need them, add trace tag OPTIMIZERS.X         |\n");
        }
        ae_trace("*** -------------------------------------------------------\n");
        ae_trace("*** |  step along D   |     delta Gi    |      slope      |\n");
        ae_trace("*** ------------------------------------------------------|\n");
        for(i=0; i<=monitor->nonc1test1lngrep.cnt-1; i++)
        {
            slope = monitor->nonc1test1lngrep.g.ptr.p_double[ae_minint(i+1, monitor->nonc1test1lngrep.cnt-1, _state)]-monitor->nonc1test1lngrep.g.ptr.p_double[i];
            slope = slope/(1.0e-15+monitor->nonc1test1lngrep.stp.ptr.p_double[ae_minint(i+1, monitor->nonc1test1lngrep.cnt-1, _state)]-monitor->nonc1test1lngrep.stp.ptr.p_double[i]);
            ae_trace("*** |  %13.5e  |  %13.5e  |   %11.3e   |",
                (double)(monitor->nonc1test1lngrep.stp.ptr.p_double[i]),
                (double)(monitor->nonc1test1lngrep.g.ptr.p_double[i]-monitor->nonc1test1lngrep.g.ptr.p_double[0]),
                (double)(slope));
            if( i>=monitor->nonc1test1lngrep.stpidxa&&i<=monitor->nonc1test1lngrep.stpidxb )
            {
                ae_trace(" <---");
            }
            ae_trace("\n");
        }
        ae_trace("*** ------------------------------------------------------|\n");
        if( needxdreport )
        {
            ae_trace("*** > printing raw variables\n");
            ae_trace("*** XK = ");
            tracevectorunscaledunshiftedautoprec(&monitor->nonc1test1lngrep.x0, monitor->n, &monitor->s, ae_true, &monitor->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("*** D  = ");
            tracevectorunscaledunshiftedautoprec(&monitor->nonc1test1lngrep.d, monitor->n, &monitor->s, ae_true, &monitor->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("*** > printing scaled variables (values are divided by user-specified scales)\n");
            ae_trace("*** XK = ");
            tracevectorautoprec(&monitor->nonc1test1lngrep.x0, 0, monitor->n, _state);
            ae_trace("\n");
            ae_trace("*** D  = ");
            tracevectorautoprec(&monitor->nonc1test1lngrep.d, 0, monitor->n, _state);
            ae_trace("\n");
        }
    }
}


/*************************************************************************
This subroutine exports report to user-readable representation (all arrays
are forced to have exactly same size as needed; unused arrays are  set  to
zero length).

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
void smoothnessmonitorexportreport(smoothnessmonitor* monitor,
     optguardreport* rep,
     ae_state *_state)
{


    
    /*
     * Finalize last line search, just to be sure
     */
    if( monitor->enqueuedcnt>0 )
    {
        smoothnessmonitorfinalizelinesearch(monitor, _state);
    }
    
    /*
     * Export report
     */
    optguardexportreport(&monitor->rep, monitor->n, monitor->k, monitor->badgradhasxj, rep, _state);
}


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
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t k;
    ae_int_t i;
    ae_int_t j;
    ae_int_t varidx;
    double v;
    double vp;
    double vm;
    double vc;
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
    if( monitor->rstateg0.stage>=0 )
    {
        n = monitor->rstateg0.ia.ptr.p_int[0];
        k = monitor->rstateg0.ia.ptr.p_int[1];
        i = monitor->rstateg0.ia.ptr.p_int[2];
        j = monitor->rstateg0.ia.ptr.p_int[3];
        varidx = monitor->rstateg0.ia.ptr.p_int[4];
        v = monitor->rstateg0.ra.ptr.p_double[0];
        vp = monitor->rstateg0.ra.ptr.p_double[1];
        vm = monitor->rstateg0.ra.ptr.p_double[2];
        vc = monitor->rstateg0.ra.ptr.p_double[3];
    }
    else
    {
        n = 205;
        k = -838;
        i = 939;
        j = -526;
        varidx = 763;
        v = -541.0;
        vp = -698.0;
        vm = -900.0;
        vc = -318.0;
    }
    if( monitor->rstateg0.stage==0 )
    {
        goto lbl_0;
    }
    if( monitor->rstateg0.stage==1 )
    {
        goto lbl_1;
    }
    if( monitor->rstateg0.stage==2 )
    {
        goto lbl_2;
    }
    if( monitor->rstateg0.stage==3 )
    {
        goto lbl_3;
    }
    
    /*
     * Routine body
     */
    n = monitor->n;
    k = monitor->k;
    monitor->needfij = ae_false;
    
    /*
     * Quick exit
     */
    if( ((n<=0||k<=0)||!ae_isfinite(teststep, _state))||ae_fp_eq(teststep,(double)(0)) )
    {
        result = ae_false;
        return result;
    }
    teststep = ae_fabs(teststep, _state);
    
    /*
     * Allocate storage
     */
    rvectorsetlengthatleast(&monitor->x, n, _state);
    rvectorsetlengthatleast(&monitor->fi, k, _state);
    rmatrixsetlengthatleast(&monitor->j, k, n, _state);
    rvectorsetlengthatleast(&monitor->xbase, n, _state);
    rvectorsetlengthatleast(&monitor->fbase, k, _state);
    rvectorsetlengthatleast(&monitor->fm, k, _state);
    rvectorsetlengthatleast(&monitor->fc, k, _state);
    rvectorsetlengthatleast(&monitor->fp, k, _state);
    rvectorsetlengthatleast(&monitor->jm, k, _state);
    rvectorsetlengthatleast(&monitor->jc, k, _state);
    rvectorsetlengthatleast(&monitor->jp, k, _state);
    rmatrixsetlengthatleast(&monitor->jbaseusr, k, n, _state);
    rmatrixsetlengthatleast(&monitor->jbasenum, k, n, _state);
    rvectorsetlengthatleast(&monitor->rep.badgradxbase, n, _state);
    rmatrixsetlengthatleast(&monitor->rep.badgraduser, k, n, _state);
    rmatrixsetlengthatleast(&monitor->rep.badgradnum, k, n, _state);
    
    /*
     * Set XBase/Jacobian presence flag
     */
    monitor->badgradhasxj = ae_true;
    
    /*
     * Determine reference point, compute function vector and user-supplied Jacobian
     */
    for(i=0; i<=n-1; i++)
    {
        v = unscaledx0->ptr.p_double[i];
        if( (hasboxconstraints&&ae_isfinite(bndl->ptr.p_double[i], _state))&&ae_fp_less(v,bndl->ptr.p_double[i]) )
        {
            v = bndl->ptr.p_double[i];
        }
        if( (hasboxconstraints&&ae_isfinite(bndu->ptr.p_double[i], _state))&&ae_fp_greater(v,bndu->ptr.p_double[i]) )
        {
            v = bndu->ptr.p_double[i];
        }
        monitor->xbase.ptr.p_double[i] = v;
        monitor->rep.badgradxbase.ptr.p_double[i] = v;
        monitor->x.ptr.p_double[i] = v;
    }
    monitor->needfij = ae_true;
    monitor->rstateg0.stage = 0;
    goto lbl_rcomm;
lbl_0:
    monitor->needfij = ae_false;
    for(i=0; i<=k-1; i++)
    {
        monitor->fbase.ptr.p_double[i] = monitor->fi.ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            monitor->jbaseusr.ptr.pp_double[i][j] = monitor->j.ptr.pp_double[i][j];
            monitor->rep.badgraduser.ptr.pp_double[i][j] = monitor->j.ptr.pp_double[i][j];
        }
    }
    
    /*
     * Check Jacobian column by column
     */
    varidx = 0;
lbl_4:
    if( varidx>n-1 )
    {
        goto lbl_6;
    }
    
    /*
     * Determine test location.
     */
    v = monitor->xbase.ptr.p_double[varidx];
    vm = v-s->ptr.p_double[varidx]*teststep;
    vp = v+s->ptr.p_double[varidx]*teststep;
    if( (hasboxconstraints&&ae_isfinite(bndl->ptr.p_double[varidx], _state))&&ae_fp_less(vm,bndl->ptr.p_double[varidx]) )
    {
        vm = bndl->ptr.p_double[varidx];
    }
    if( (hasboxconstraints&&ae_isfinite(bndu->ptr.p_double[varidx], _state))&&ae_fp_greater(vp,bndu->ptr.p_double[varidx]) )
    {
        vp = bndu->ptr.p_double[varidx];
    }
    vc = vm+(vp-vm)/(double)2;
    
    /*
     * Quickly skip fixed variables
     */
    if( (ae_fp_eq(vm,vp)||ae_fp_eq(vc,vm))||ae_fp_eq(vc,vp) )
    {
        for(i=0; i<=k-1; i++)
        {
            monitor->rep.badgradnum.ptr.pp_double[i][varidx] = (double)(0);
        }
        goto lbl_5;
    }
    
    /*
     * Compute F/J at three trial points
     */
    for(i=0; i<=n-1; i++)
    {
        monitor->x.ptr.p_double[i] = monitor->xbase.ptr.p_double[i];
    }
    monitor->x.ptr.p_double[varidx] = vm;
    monitor->needfij = ae_true;
    monitor->rstateg0.stage = 1;
    goto lbl_rcomm;
lbl_1:
    monitor->needfij = ae_false;
    for(i=0; i<=k-1; i++)
    {
        monitor->fm.ptr.p_double[i] = monitor->fi.ptr.p_double[i];
        monitor->jm.ptr.p_double[i] = monitor->j.ptr.pp_double[i][varidx];
    }
    for(i=0; i<=n-1; i++)
    {
        monitor->x.ptr.p_double[i] = monitor->xbase.ptr.p_double[i];
    }
    monitor->x.ptr.p_double[varidx] = vc;
    monitor->needfij = ae_true;
    monitor->rstateg0.stage = 2;
    goto lbl_rcomm;
lbl_2:
    monitor->needfij = ae_false;
    for(i=0; i<=k-1; i++)
    {
        monitor->fc.ptr.p_double[i] = monitor->fi.ptr.p_double[i];
        monitor->jc.ptr.p_double[i] = monitor->j.ptr.pp_double[i][varidx];
    }
    for(i=0; i<=n-1; i++)
    {
        monitor->x.ptr.p_double[i] = monitor->xbase.ptr.p_double[i];
    }
    monitor->x.ptr.p_double[varidx] = vp;
    monitor->needfij = ae_true;
    monitor->rstateg0.stage = 3;
    goto lbl_rcomm;
lbl_3:
    monitor->needfij = ae_false;
    for(i=0; i<=k-1; i++)
    {
        monitor->fp.ptr.p_double[i] = monitor->fi.ptr.p_double[i];
        monitor->jp.ptr.p_double[i] = monitor->j.ptr.pp_double[i][varidx];
    }
    
    /*
     * Check derivative
     */
    for(i=0; i<=k-1; i++)
    {
        monitor->rep.badgradnum.ptr.pp_double[i][varidx] = (monitor->fp.ptr.p_double[i]-monitor->fm.ptr.p_double[i])/(vp-vm);
        if( !derivativecheck(monitor->fm.ptr.p_double[i], monitor->jm.ptr.p_double[i]*s->ptr.p_double[varidx], monitor->fp.ptr.p_double[i], monitor->jp.ptr.p_double[i]*s->ptr.p_double[varidx], monitor->fc.ptr.p_double[i], monitor->jc.ptr.p_double[i]*s->ptr.p_double[varidx], (vp-vm)/s->ptr.p_double[varidx], _state) )
        {
            monitor->rep.badgradsuspected = ae_true;
            monitor->rep.badgradfidx = i;
            monitor->rep.badgradvidx = varidx;
        }
    }
lbl_5:
    varidx = varidx+1;
    goto lbl_4;
lbl_6:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    monitor->rstateg0.ia.ptr.p_int[0] = n;
    monitor->rstateg0.ia.ptr.p_int[1] = k;
    monitor->rstateg0.ia.ptr.p_int[2] = i;
    monitor->rstateg0.ia.ptr.p_int[3] = j;
    monitor->rstateg0.ia.ptr.p_int[4] = varidx;
    monitor->rstateg0.ra.ptr.p_double[0] = v;
    monitor->rstateg0.ra.ptr.p_double[1] = vp;
    monitor->rstateg0.ra.ptr.p_double[2] = vm;
    monitor->rstateg0.ra.ptr.p_double[3] = vc;
    return result;
}


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
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(resetfreq>=0, "HessianInit: ResetFreq<0", _state);
    resetfreq = ae_minint(resetfreq, n, _state);
    hess->htype = 0;
    hess->n = n;
    hess->resetfreq = resetfreq;
    hess->stpshort = stpshort;
    hess->hage = 0;
    hess->gammasml = 0.000001;
    hess->reg = (double)100*ae_sqrt(ae_machineepsilon, _state);
    hess->smallreg = 0.01*ae_sqrt(ae_machineepsilon, _state);
    hess->microreg = ((double)1000+ae_sqrt((double)(n), _state))*ae_machineepsilon;
    hess->wolfeeps = 0.001;
    hess->maxhess = 1.0E8;
    hess->sumsy = ae_sqr(ae_machineepsilon, _state);
    hess->sumy2 = hess->sumsy*hess->gammasml;
    hess->sums2 = (double)(0);
    hess->updatestatus = 0;
    hess->sigmadecay = 1.0;
    rvectorsetlengthatleast(&hess->sk, n, _state);
    rvectorsetlengthatleast(&hess->yk, n, _state);
    rsetallocm(n, n, 0.0, &hess->hcurrent, _state);
    rsetallocm(n, n, 0.0, &hess->hincoming, _state);
    for(i=0; i<=n-1; i++)
    {
        hess->hcurrent.ptr.pp_double[i][i] = (double)(1);
        hess->hincoming.ptr.pp_double[i][i] = (double)(1);
    }
}


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
     ae_state *_state)
{


    ae_assert(n>0, "HessianInitLowRank: N<=0", _state);
    ae_assert(m>=0, "HessianInitLowRank: M<0", _state);
    m = ae_minint(m, n, _state);
    
    /*
     * Initialize generic fields
     */
    hess->htype = 3;
    hess->n = n;
    
    /*
     * Initialize mode-specific fields
     */
    hess->m = m;
    hess->memlen = 0;
    hess->sigma = (double)(1);
    hess->gamma = (double)(1);
    if( m>0 )
    {
        rallocm(m, n, &hess->s, _state);
        rallocm(m, n, &hess->y, _state);
        rallocm(m, m, &hess->lowranksst, _state);
        rallocm(m, m, &hess->lowranksyt, _state);
    }
    optserv_resetlowrankmodel(hess, _state);
    
    /*
     * Other fields
     */
    hess->resetfreq = 0;
    hess->stpshort = stpshort;
    hess->hage = 0;
    hess->gammasml = 0.000001;
    hess->reg = (double)100*ae_sqrt(ae_machineepsilon, _state);
    hess->smallreg = 0.01*ae_sqrt(ae_machineepsilon, _state);
    hess->microreg = ((double)1000+ae_sqrt((double)(n), _state))*ae_machineepsilon;
    hess->sumsy = ae_sqr(ae_machineepsilon, _state);
    hess->sumy2 = hess->sumsy*hess->gammasml;
    hess->sums2 = ae_sqr(ae_machineepsilon, _state);
    hess->wolfeeps = 0.001;
    hess->maxhess = maxhess;
    hess->updatestatus = 0;
    hess->sigmadecay = 1.0;
    rallocv(n, &hess->sk, _state);
    rallocv(n, &hess->yk, _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    double gamma;
    double sy;
    double snrm2;
    double ynrm2;
    double ski;
    double yki;
    double yk2;
    double skyk;
    double skg0;
    double skg1;
    double wolfedecay;


    n = hess->n;
    
    /*
     * Prepare Sk, Yk
     */
    sy = (double)(0);
    snrm2 = (double)(0);
    ynrm2 = (double)(0);
    skg0 = (double)(0);
    skg1 = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        ski = x1->ptr.p_double[i]-x0->ptr.p_double[i];
        yki = g1->ptr.p_double[i]-g0->ptr.p_double[i];
        hess->sk.ptr.p_double[i] = ski;
        hess->yk.ptr.p_double[i] = yki;
        sy = sy+ski*yki;
        snrm2 = snrm2+ski*ski;
        ynrm2 = ynrm2+yki*yki;
        skg0 = skg0+ski*g0->ptr.p_double[i];
        skg1 = skg1+ski*g1->ptr.p_double[i];
    }
    hess->updatestatus = 0;
    
    /*
     * Update current and incoming Hessians
     */
    ae_assert(hess->htype==0||hess->htype==3, "HessianUpdate: Hessian mode not supported", _state);
    if( hess->htype==0 )
    {
        
        /*
         * Update dense Hessian using BFGS formula for Bk
         */
        optserv_hessianupdatelowlevel(hess, &hess->hcurrent, &hess->sk, &hess->yk, &hess->updatestatus, _state);
        optserv_hessianupdatelowlevel(hess, &hess->hincoming, &hess->sk, &hess->yk, &i, _state);
        if( ae_fp_greater(sy,(double)(0)) )
        {
            hess->sumsy = hess->sumsy+sy;
            hess->sumy2 = hess->sumy2+ynrm2;
        }
        hess->sums2 = hess->sums2+snrm2;
        hess->hage = hess->hage+1;
        
        /*
         * Perform Hessian reset if needed
         */
        if( hess->resetfreq>0&&hess->hage>=hess->resetfreq )
        {
            rmatrixcopy(n, n, &hess->hincoming, 0, 0, &hess->hcurrent, 0, 0, _state);
            gamma = hess->sumy2/(hess->sumsy+hess->reg*hess->sumy2+hess->smallreg*hess->sums2);
            rsetm(n, n, 0.0, &hess->hincoming, _state);
            for(i=0; i<=n-1; i++)
            {
                hess->hincoming.ptr.pp_double[i][i] = gamma;
            }
            hess->sumsy = ae_sqr(ae_machineepsilon, _state);
            hess->sumy2 = hess->sumsy*hess->gammasml;
            hess->sums2 = (double)(0);
            hess->hage = 0;
            hess->updatestatus = 3;
        }
        return;
    }
    if( hess->htype==3 )
    {
        if( dotrace )
        {
            ae_trace("> analyzing Hessian update:\n>> (Sk,G0)=%0.15e  (Sk,G1)=%0.15e  (Yk,Yk)/(Sk.Yk)=%0.15e\n",
                (double)(skg0),
                (double)(skg1),
                (double)(ynrm2/sy));
        }
        
        /*
         * Decide whether update is good enough to be remembered
         */
        if( hess->m==0 )
        {
            
            /*
             * Zero memory was specified, update ignored
             */
            if( dotrace )
            {
                ae_trace(">> zero memory length, update rejected\n");
            }
            return;
        }
        if( ae_fp_less_eq(rmaxabsv(n, &hess->sk, _state),hess->stpshort) )
        {
            
            /*
             * Sk is too small, skip update
             */
            if( dotrace )
            {
                ae_trace(">> step is too short, update rejected\n");
            }
            return;
        }
        
        /*
         * Reject bad steps
         */
        if( ae_fp_eq(rdotv2(n, &hess->yk, _state),(double)(0)) )
        {
            
            /*
             * The function is linear in the step direction, no update applied
             */
            optserv_popfrontxy(hess, _state);
            hess->sigma = ae_maxreal(0.1*hess->sigma, 1.0E-4, _state);
            optserv_resetlowrankmodel(hess, _state);
            if( dotrace )
            {
                ae_trace(">> zero Yk (linear function?), update rejected, queue size decreased by 1, diagonal scaling sigma=%0.2e (decreased)\n",
                    (double)(hess->sigma));
            }
            return;
        }
        wolfedecay = (double)1-hess->wolfeeps*ae_minreal(ae_sqrt(snrm2, _state), 1.0, _state);
        if( !((ae_fp_less(skg0,(double)(0))&&ae_fp_greater(skg1,wolfedecay*skg0))||(ae_fp_greater(skg1,(double)(0))&&ae_fp_less(skg0,wolfedecay*skg1))) )
        {
            
            /*
             * Wolfe decay condition does not hold
             */
            optserv_popfrontxy(hess, _state);
            hess->sigma = ae_maxreal(0.1*hess->sigma, 1.0E-4, _state);
            optserv_resetlowrankmodel(hess, _state);
            if( dotrace )
            {
                ae_trace(">> Wolfe decay condition does not hold, update rejected, queue size decreased by 1, diagonal scaling sigma=%0.2e (decreased)\n",
                    (double)(hess->sigma));
            }
            return;
        }
        if( ae_fp_greater(sy,(double)(0))&&ae_fp_greater(ynrm2/sy,hess->maxhess) )
        {
            
            /*
             * Hessian norm is too high
             */
            if( dotrace )
            {
                ae_trace(">> Hessian norm is too high (%0.2e), update rejected, diagonal scaling sigma=%0.2e (unchanged)\n",
                    (double)(ynrm2/sy),
                    (double)(hess->sigma));
            }
            return;
        }
        
        /*
         * Update historical averages
         */
        hess->sumsy = hess->sumsy+sy;
        hess->sumy2 = hess->sumy2+ynrm2;
        hess->sums2 = hess->sums2+snrm2;
        
        /*
         * Apply regularization:
         * * first,  add REG*Sk to Yk in order to make model at least slightly convex (Sigma is at least REG
         * * second, add REG*Yk to Xk in order to limit model curvature (Sigma is at most 1/REG)
         *
         * Whilst specific order of these operations is not very important, we prefer to Yk+=REG*Sk be first
         * due to Sk being better guarded away from zero.
         */
        raddv(n, hess->reg, &hess->sk, &hess->yk, _state);
        raddv(n, hess->reg, &hess->yk, &hess->sk, _state);
        
        /*
         * Update low rank Hessian data
         */
        ae_assert(hess->memlen<=hess->m, "HessianUpdate: integrity check 5763 failed", _state);
        if( hess->memlen==hess->m )
        {
            optserv_popfrontxy(hess, _state);
        }
        
        /*
         * Append to S and Y
         */
        ae_assert(hess->memlen<hess->m, "HessianUpdate: integrity check 5764 failed", _state);
        hess->memlen = hess->memlen+1;
        rcopyvr(n, &hess->sk, &hess->s, hess->memlen-1, _state);
        rcopyvr(n, &hess->yk, &hess->y, hess->memlen-1, _state);
        
        /*
         * Append row/col to LowRankSST and LowRankSYT
         */
        rallocv(hess->memlen, &hess->buf, _state);
        rgemv(hess->memlen, n, 1.0, &hess->s, 0, &hess->sk, 0.0, &hess->buf, _state);
        rcopyvr(hess->memlen, &hess->buf, &hess->lowranksst, hess->memlen-1, _state);
        rcopyvc(hess->memlen, &hess->buf, &hess->lowranksst, hess->memlen-1, _state);
        rgemv(hess->memlen, n, 1.0, &hess->y, 0, &hess->sk, 0.0, &hess->buf, _state);
        rcopyvr(hess->memlen, &hess->buf, &hess->lowranksyt, hess->memlen-1, _state);
        rgemv(hess->memlen, n, 1.0, &hess->s, 0, &hess->yk, 0.0, &hess->buf, _state);
        rcopyvc(hess->memlen, &hess->buf, &hess->lowranksyt, hess->memlen-1, _state);
        
        /*
         * Recompute scaling, set SigmaDecay to 1
         */
        yk2 = rdotv2(n, &hess->yk, _state);
        skyk = rdotv(n, &hess->sk, &hess->yk, _state);
        hess->sigma = boundval(yk2/skyk, 0.1*hess->sigma, (double)10*hess->sigma, _state);
        hess->sigma = ae_minreal(hess->sigma, (double)1/(hess->reg+ae_machineepsilon), _state);
        hess->gamma = (double)1/hess->sigma;
        hess->sigmadecay = 1.0;
        if( dotrace )
        {
            ae_trace(">> diagonal scaling sigma=%0.2e\n",
                (double)(hess->sigma));
        }
        
        /*
         * Invalidate model
         */
        optserv_resetlowrankmodel(hess, _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    ae_assert(hess->htype==0||hess->htype==3, "HessianGetDiagonal: Hessian mode is not supported", _state);
    n = hess->n;
    rallocv(n, d, _state);
    if( hess->htype==0 )
    {
        
        /*
         * Explicit dense Hessian
         */
        for(i=0; i<=n-1; i++)
        {
            d->ptr.p_double[i] = hess->hcurrent.ptr.pp_double[i][i];
        }
    }
    if( hess->htype==3 )
    {
        
        /*
         * Low-rank model
         */
        optserv_recomputelowrankmodel(hess, _state);
        optserv_recomputelowrankdiagonal(hess, _state);
        rcopyv(n, &hess->lowrankeffd, d, _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    ae_assert(hess->htype==0||hess->htype==3, "HessianGetHessian: Hessian mode is not supported", _state);
    n = hess->n;
    rallocm(n, n, h, _state);
    if( hess->htype==0 )
    {
        
        /*
         * Dense direct Hessian
         */
        rcopym(n, n, &hess->hcurrent, h, _state);
    }
    if( hess->htype==3 )
    {
        
        /*
         * Low-rank model
         */
        optserv_recomputelowrankmodel(hess, _state);
        rsetm(n, n, 0.0, h, _state);
        for(i=0; i<=n-1; i++)
        {
            h->ptr.pp_double[i][i] = hess->sigma;
        }
        rmatrixgemm(n, n, hess->lowrankk, 1.0, &hess->lowrankcp, 0, 0, 1, &hess->lowrankcp, 0, 0, 0, 1.0, h, 0, 0, _state);
        rmatrixgemm(n, n, hess->lowrankk, -1.0, &hess->lowrankcm, 0, 0, 1, &hess->lowrankcm, 0, 0, 0, 1.0, h, 0, 0, _state);
    }
}


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
     ae_state *_state)
{
    ae_int_t n;


    ae_assert(hess->htype==0||hess->htype==3, "HessianMV: Hessian mode is not supported", _state);
    n = hess->n;
    rallocv(n, hx, _state);
    if( hess->htype==0 )
    {
        
        /*
         * Dense direct Hessian
         */
        rgemv(n, n, 1.0, &hess->hcurrent, 0, x, 0.0, hx, _state);
    }
    if( hess->htype==3 )
    {
        
        /*
         * Low-rank model
         */
        optserv_recomputelowrankmodel(hess, _state);
        rcopymulv(n, hess->sigma, x, hx, _state);
        if( hess->lowrankk>0 )
        {
            rallocv(hess->lowrankk, &hess->buf, _state);
            rgemv(hess->lowrankk, n, 1.0, &hess->lowrankcp, 0, x, 0.0, &hess->buf, _state);
            rgemv(n, hess->lowrankk, 1.0, &hess->lowrankcp, 1, &hess->buf, 1.0, hx, _state);
            rgemv(hess->lowrankk, n, 1.0, &hess->lowrankcm, 0, x, 0.0, &hess->buf, _state);
            rgemv(n, hess->lowrankk, -1.0, &hess->lowrankcm, 1, &hess->buf, 1.0, hx, _state);
        }
    }
}


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
     ae_state *_state)
{
    ae_int_t n;
    double result;


    ae_assert(hess->htype==0||hess->htype==3, "HessianVMV: Hessian mode is not supported", _state);
    result = (double)(0);
    n = hess->n;
    if( hess->htype==0 )
    {
        
        /*
         * Dense direct Hessian
         */
        hessianmv(hess, x, &hess->bufvmv, _state);
        result = rdotv(n, x, &hess->bufvmv, _state);
        return result;
    }
    if( hess->htype==3 )
    {
        
        /*
         * Low-rank model
         */
        optserv_recomputelowrankmodel(hess, _state);
        result = hess->sigma*rdotv2(n, x, _state);
        if( hess->lowrankk>0 )
        {
            rallocv(hess->lowrankk, &hess->bufvmv, _state);
            rgemv(hess->lowrankk, n, 1.0, &hess->lowrankcp, 0, x, 0.0, &hess->bufvmv, _state);
            result = result+rdotv2(hess->lowrankk, &hess->bufvmv, _state);
            rgemv(hess->lowrankk, n, 1.0, &hess->lowrankcm, 0, x, 0.0, &hess->bufvmv, _state);
            result = result-rdotv2(hess->lowrankk, &hess->bufvmv, _state);
        }
        return result;
    }
    return result;
}


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
     ae_state *_state)
{
    ae_int_t n;

    *xhx = 0.0;

    ae_assert(hess->htype==0||hess->htype==3, "HessianMV: Hessian mode is not supported", _state);
    n = hess->n;
    rallocv(n, hx, _state);
    *xhx = (double)(0);
    if( hess->htype==0 )
    {
        
        /*
         * Dense direct Hessian
         */
        rgemv(n, n, 1.0, &hess->hcurrent, 0, x, 0.0, hx, _state);
        *xhx = rdotv(n, x, &hess->bufvmv, _state);
        return;
    }
    if( hess->htype==3 )
    {
        
        /*
         * Low-rank model
         */
        optserv_recomputelowrankmodel(hess, _state);
        rcopymulv(n, hess->sigma, x, hx, _state);
        *xhx = hess->sigma*rdotv2(n, x, _state);
        if( hess->lowrankk>0 )
        {
            rallocv(hess->lowrankk, &hess->buf, _state);
            rgemv(hess->lowrankk, n, 1.0, &hess->lowrankcp, 0, x, 0.0, &hess->buf, _state);
            rgemv(n, hess->lowrankk, 1.0, &hess->lowrankcp, 1, &hess->buf, 1.0, hx, _state);
            *xhx = *xhx+rdotv2(hess->lowrankk, &hess->buf, _state);
            rgemv(hess->lowrankk, n, 1.0, &hess->lowrankcm, 0, x, 0.0, &hess->buf, _state);
            rgemv(n, hess->lowrankk, -1.0, &hess->lowrankcm, 1, &hess->buf, 1.0, hx, _state);
            *xhx = *xhx-rdotv2(hess->lowrankk, &hess->buf, _state);
        }
        return;
    }
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t rk;
    ae_matrix q;
    ae_vector grad;
    ae_vector x;
    ae_vector ax;
    ae_vector c;
    ae_bool issemidefinite;
    ae_bool haslinearterm;
    ae_bool hasnonlinearterms;
    ae_bool hasquadraticterm;
    ae_bool hasquarticterm;
    ae_int_t ngenerated;
    ae_int_t cidx;
    ae_int_t ctype;
    double v;
    double vmul;
    ae_vector xtrial;
    ae_int_t passcount;
    ae_int_t nboxinequality;
    ae_int_t nlinearinequality;
    ae_int_t nnonlinearinequality;

    ae_frame_make(_state, &_frame_block);
    memset(&q, 0, sizeof(q));
    memset(&grad, 0, sizeof(grad));
    memset(&x, 0, sizeof(x));
    memset(&ax, 0, sizeof(ax));
    memset(&c, 0, sizeof(c));
    memset(&xtrial, 0, sizeof(xtrial));
    _multiobjectivetestfunction_clear(problem);
    ae_matrix_init(&q, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&grad, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&x, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ax, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&c, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xtrial, 0, DT_REAL, _state, ae_true);

    haslinearterm = ae_false;
    hasnonlinearterms = ae_false;
    hasquadraticterm = ae_false;
    hasquarticterm = ae_false;
    issemidefinite = ae_false;
    touchboolean(&haslinearterm, _state);
    touchboolean(&hasnonlinearterms, _state);
    touchboolean(&hasquadraticterm, _state);
    touchboolean(&hasquarticterm, _state);
    touchboolean(&issemidefinite, _state);
    
    /*
     * Generate random test functions
     */
    problem->n = n;
    problem->m = m;
    rsetallocv(m, 0.0, &problem->tgtc, _state);
    rsetallocm(m, n, 0.0, &problem->tgtb, _state);
    rsetallocm(m*n, n, 0.0, &problem->tgta, _state);
    rsetallocm(m, n, 0.0, &problem->tgtd, _state);
    ae_assert(taskkind>=0&&taskkind<=4, "MOTFCreate: incorrect TaskKind", _state);
    if( taskkind==0 )
    {
        
        /*
         * Quartic term
         */
        for(i=0; i<=m-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                problem->tgtd.ptr.pp_double[i][j] = ae_pow((double)(2), 0.33*hqrndnormal(rs, _state), _state);
            }
        }
        hasnonlinearterms = ae_true;
        hasquarticterm = ae_true;
    }
    if( taskkind<=2 )
    {
        
        /*
         * Quadratic term
         */
        if( taskkind!=2 )
        {
            
            /*
             * well conditioned matrix
             */
            for(k=0; k<=m-1; k++)
            {
                spdmatrixrndcond(n, 100.0, &q, _state);
                rmatrixcopy(n, n, &q, 0, 0, &problem->tgta, k*n, 0, _state);
            }
        }
        else
        {
            
            /*
             * Low rank matrix
             */
            rk = ae_minint(n, 1+hqrnduniformi(rs, 5, _state), _state);
            for(k=0; k<=m-1; k++)
            {
                hqrndnormalm(rs, rk, n, &q, _state);
                rmatrixgemm(n, n, rk, 1.0, &q, 0, 0, 1, &q, 0, 0, 0, 0.0, &problem->tgta, k*n, 0, _state);
            }
        }
        hasquadraticterm = ae_true;
        hasnonlinearterms = ae_true;
    }
    if( taskkind<=3 )
    {
        
        /*
         * Linear term
         */
        hqrndnormalm(rs, m, n, &problem->tgtb, _state);
        haslinearterm = ae_true;
    }
    if( taskkind<=4 )
    {
        
        /*
         * Constant term
         */
        hqrndnormalv(rs, m, &problem->tgtc, _state);
    }
    issemidefinite = taskkind>=2;
    
    /*
     * No known solutions
     */
    ae_matrix_set_length(&problem->xsol, 0, 0, _state);
    
    /*
     * Generate random trial point which will be strictly feasible
     */
    hqrndnormalv(rs, n, &xtrial, _state);
    
    /*
     *
     * Generate empty set of constraints
     */
    rsetallocv(n, _state->v_neginf, &problem->bndl, _state);
    rsetallocv(n, _state->v_posinf, &problem->bndu, _state);
    problem->nlinear = 0;
    problem->nnonlinear = 0;
    
    /*
     * Randomly generate equality constraints using trial point
     */
    ngenerated = 0;
    passcount = 0;
    while(ngenerated<nequality&&passcount<2*n)
    {
        passcount = passcount+1;
        ctype = hqrnduniformi(rs, 3, _state);
        
        /*
         * Try adding a box constraint
         */
        if( ctype==0 )
        {
            
            /*
             * Try to find a box constraint that is not defined yet
             */
            i = hqrnduniformi(rs, n, _state);
            if( ae_isneginf(problem->bndl.ptr.p_double[i], _state)&&ae_isposinf(problem->bndu.ptr.p_double[i], _state) )
            {
                
                /*
                 * Use this box constraint.
                 * Fix a bound corresponding to the Lagrange multiplier direction,
                 * with probability 15% fix other bound too (when it is free).
                 */
                problem->bndl.ptr.p_double[i] = xtrial.ptr.p_double[i];
                problem->bndu.ptr.p_double[i] = xtrial.ptr.p_double[i];
                ngenerated = ngenerated+1;
            }
        }
        
        /*
         * Add a linear equality constraint.
         */
        if( ctype==1 )
        {
            hqrndnormalv(rs, n, &c, _state);
            v = rdotv(n, &c, &xtrial, _state);
            rmatrixgrowrowsto(&problem->densea, problem->nlinear+1, n, _state);
            rgrowv(problem->nlinear+1, &problem->al, _state);
            rgrowv(problem->nlinear+1, &problem->au, _state);
            rcopyvr(n, &c, &problem->densea, problem->nlinear, _state);
            problem->al.ptr.p_double[problem->nlinear] = v;
            problem->au.ptr.p_double[problem->nlinear] = v;
            problem->nlinear = problem->nlinear+1;
            ngenerated = ngenerated+1;
        }
        
        /*
         * Add a nonlinear equality constraint.
         */
        if( ctype==2 )
        {
            
            /*
             * Allocate place
             */
            rgrowv(problem->nnonlinear+1, &problem->nlc, _state);
            rmatrixgrowrowsto(&problem->nlb, problem->nnonlinear+1, n, _state);
            rmatrixgrowrowsto(&problem->nla, (problem->nnonlinear+1)*n, n, _state);
            rmatrixgrowrowsto(&problem->nld, problem->nnonlinear+1, n, _state);
            rgrowv(problem->nnonlinear+1, &problem->hl, _state);
            rgrowv(problem->nnonlinear+1, &problem->hu, _state);
            
            /*
             * Generate and compute nonlinear constraint
             */
            v = (double)(0);
            problem->nlc.ptr.p_double[problem->nnonlinear] = hqrndnormal(rs, _state);
            v = v+problem->nlc.ptr.p_double[problem->nnonlinear];
            hqrndnormalv(rs, n, &c, _state);
            rcopyvr(n, &c, &problem->nlb, problem->nnonlinear, _state);
            v = v+rdotv(n, &c, &xtrial, _state);
            spdmatrixrndcond(n, 10.0, &q, _state);
            vmul = nlquadratic*ae_fabs(hqrndnormal(rs, _state), _state);
            for(i=0; i<=n-1; i++)
            {
                rmulr(n, vmul, &q, i, _state);
            }
            rmatrixcopy(n, n, &q, 0, 0, &problem->nla, problem->nnonlinear*n, 0, _state);
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=n-1; j++)
                {
                    v = v+0.5*xtrial.ptr.p_double[i]*q.ptr.pp_double[i][j]*xtrial.ptr.p_double[j];
                }
            }
            for(i=0; i<=n-1; i++)
            {
                problem->nld.ptr.pp_double[problem->nnonlinear][i] = nlquartic*ae_pow((double)(2), 0.33*hqrndnormal(rs, _state), _state);
                v = v+problem->nld.ptr.pp_double[problem->nnonlinear][i]*ae_pow(xtrial.ptr.p_double[i], (double)(4), _state);
            }
            problem->hl.ptr.p_double[problem->nnonlinear] = v;
            problem->hu.ptr.p_double[problem->nnonlinear] = v;
            problem->nnonlinear = problem->nnonlinear+1;
            ngenerated = ngenerated+1;
        }
    }
    
    /*
     * Decide on desired count of inequality constraints of all types (for box constraints - only approximate count)
     */
    nboxinequality = hqrnduniformi(rs, ninequality+1, _state);
    nlinearinequality = hqrnduniformi(rs, ninequality-nboxinequality+1, _state);
    nnonlinearinequality = ninequality-nboxinequality-nlinearinequality;
    
    /*
     * Randomly generate box/linear/nonlinear inequality constraints inactive at the trial point
     */
    for(k=0; k<=nboxinequality-1; k++)
    {
        i = hqrnduniformi(rs, n, _state);
        if( ae_isneginf(problem->bndl.ptr.p_double[i], _state)&&ae_isposinf(problem->bndu.ptr.p_double[i], _state) )
        {
            ctype = hqrnduniformi(rs, 3, _state);
            problem->bndl.ptr.p_double[i] = rcase2(ctype==0||ctype==2, xtrial.ptr.p_double[i]-ae_pow((double)(2), hqrndnormal(rs, _state), _state), _state->v_neginf, _state);
            problem->bndu.ptr.p_double[i] = rcase2(ctype==1||ctype==2, xtrial.ptr.p_double[i]+ae_pow((double)(2), hqrndnormal(rs, _state), _state), _state->v_posinf, _state);
        }
    }
    for(cidx=0; cidx<=nlinearinequality-1; cidx++)
    {
        hqrndnormalv(rs, n, &c, _state);
        v = rdotv(n, &c, &xtrial, _state);
        ctype = hqrnduniformi(rs, 3, _state);
        rmatrixgrowrowsto(&problem->densea, problem->nlinear+1, n, _state);
        rgrowv(problem->nlinear+1, &problem->al, _state);
        rgrowv(problem->nlinear+1, &problem->au, _state);
        rcopyvr(n, &c, &problem->densea, problem->nlinear, _state);
        problem->al.ptr.p_double[problem->nlinear] = rcase2(ctype==0||ctype==2, v-ae_pow((double)(2), hqrndnormal(rs, _state), _state), _state->v_neginf, _state);
        problem->au.ptr.p_double[problem->nlinear] = rcase2(ctype==1||ctype==2, v+ae_pow((double)(2), hqrndnormal(rs, _state), _state), _state->v_posinf, _state);
        problem->nlinear = problem->nlinear+1;
    }
    for(cidx=0; cidx<=nnonlinearinequality-1; cidx++)
    {
        
        /*
         * Allocate place
         */
        rgrowv(problem->nnonlinear+1, &problem->nlc, _state);
        rmatrixgrowrowsto(&problem->nlb, problem->nnonlinear+1, n, _state);
        rmatrixgrowrowsto(&problem->nla, (problem->nnonlinear+1)*n, n, _state);
        rmatrixgrowrowsto(&problem->nld, problem->nnonlinear+1, n, _state);
        rgrowv(problem->nnonlinear+1, &problem->hl, _state);
        rgrowv(problem->nnonlinear+1, &problem->hu, _state);
        
        /*
         * Generate and compute nonlinear constraint
         */
        v = (double)(0);
        problem->nlc.ptr.p_double[problem->nnonlinear] = hqrndnormal(rs, _state);
        v = v+problem->nlc.ptr.p_double[problem->nnonlinear];
        hqrndnormalv(rs, n, &c, _state);
        rcopyvr(n, &c, &problem->nlb, problem->nnonlinear, _state);
        v = v+rdotv(n, &c, &xtrial, _state);
        spdmatrixrndcond(n, 10.0, &q, _state);
        vmul = nlquadratic*ae_fabs(hqrndnormal(rs, _state), _state);
        for(i=0; i<=n-1; i++)
        {
            rmulr(n, vmul, &q, i, _state);
        }
        rmatrixcopy(n, n, &q, 0, 0, &problem->nla, problem->nnonlinear*n, 0, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=n-1; j++)
            {
                v = v+0.5*xtrial.ptr.p_double[i]*q.ptr.pp_double[i][j]*xtrial.ptr.p_double[j];
            }
        }
        for(i=0; i<=n-1; i++)
        {
            problem->nld.ptr.pp_double[problem->nnonlinear][i] = nlquartic*ae_pow((double)(2), 0.33*hqrndnormal(rs, _state), _state);
            v = v+problem->nld.ptr.pp_double[problem->nnonlinear][i]*ae_pow(xtrial.ptr.p_double[i], (double)(4), _state);
        }
        ctype = hqrnduniformi(rs, 3, _state);
        problem->hl.ptr.p_double[problem->nnonlinear] = rcase2(ctype==0||ctype==2, v-ae_pow((double)(2), hqrndnormal(rs, _state), _state), _state->v_neginf, _state);
        problem->hu.ptr.p_double[problem->nnonlinear] = rcase2(ctype==1||ctype==2, v+ae_pow((double)(2), hqrndnormal(rs, _state), _state), _state->v_posinf, _state);
        problem->nnonlinear = problem->nnonlinear+1;
    }
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t k;
    ae_int_t n;
    ae_int_t m;
    ae_int_t dst;
    ae_vector ax;

    ae_frame_make(_state, &_frame_block);
    memset(&ax, 0, sizeof(ax));
    ae_vector_init(&ax, 0, DT_REAL, _state, ae_true);

    ae_assert(!needfi||fi->cnt>=problem->m+problem->nnonlinear, "MOTFEval: Fi is too short", _state);
    ae_assert(!needjac||jac->rows>=problem->m+problem->nnonlinear, "MOTFEval: Jac is too short", _state);
    ae_assert(!needjac||jac->cols>=problem->n, "MOTFEval: Jac is too short", _state);
    n = problem->n;
    m = problem->m;
    
    /*
     * Evaluate objectives
     */
    for(k=0; k<=m-1; k++)
    {
        dst = k;
        
        /*
         * Initial values (constant term)
         */
        if( needfi )
        {
            fi->ptr.p_double[dst] = problem->tgtc.ptr.p_double[k];
        }
        if( needjac )
        {
            rsetr(n, 0.0, jac, dst, _state);
        }
        
        /*
         * Linear term
         */
        if( needfi )
        {
            fi->ptr.p_double[dst] = fi->ptr.p_double[dst]+rdotvr(n, x, &problem->tgtb, k, _state);
        }
        if( needjac )
        {
            raddrr(n, 1.0, &problem->tgtb, k, jac, dst, _state);
        }
        
        /*
         * Quadratic term
         */
        rallocv(n, &ax, _state);
        rmatrixgemv(n, n, 1.0, &problem->tgta, k*n, 0, 0, x, 0, 0.0, &ax, 0, _state);
        if( needfi )
        {
            fi->ptr.p_double[dst] = fi->ptr.p_double[dst]+0.5*rdotv(n, x, &ax, _state);
        }
        if( needjac )
        {
            raddvr(n, 1.0, &ax, jac, dst, _state);
        }
        
        /*
         * Quartic term
         */
        for(i=0; i<=n-1; i++)
        {
            if( needfi )
            {
                fi->ptr.p_double[dst] = fi->ptr.p_double[dst]+problem->tgtd.ptr.pp_double[k][i]*ae_pow(x->ptr.p_double[i], (double)(4), _state);
            }
            if( needjac )
            {
                jac->ptr.pp_double[dst][i] = jac->ptr.pp_double[dst][i]+problem->tgtd.ptr.pp_double[k][i]*(double)4*ae_pow(x->ptr.p_double[i], (double)(3), _state);
            }
        }
    }
    
    /*
     * Nonlinear constraints
     */
    for(k=0; k<=problem->nnonlinear-1; k++)
    {
        dst = m+k;
        
        /*
         * Initial values (constant term)
         */
        if( needfi )
        {
            fi->ptr.p_double[dst] = problem->nlc.ptr.p_double[k];
        }
        if( needjac )
        {
            rsetr(n, 0.0, jac, dst, _state);
        }
        
        /*
         * Linear term
         */
        if( needfi )
        {
            fi->ptr.p_double[dst] = fi->ptr.p_double[dst]+rdotvr(n, x, &problem->nlb, k, _state);
        }
        if( needjac )
        {
            raddrr(n, 1.0, &problem->nlb, k, jac, dst, _state);
        }
        
        /*
         * Quadratic term
         */
        rallocv(n, &ax, _state);
        rmatrixgemv(n, n, 1.0, &problem->nla, k*n, 0, 0, x, 0, 0.0, &ax, 0, _state);
        if( needfi )
        {
            fi->ptr.p_double[dst] = fi->ptr.p_double[dst]+0.5*rdotv(n, x, &ax, _state);
        }
        if( needjac )
        {
            raddvr(n, 1.0, &ax, jac, dst, _state);
        }
        
        /*
         * Quartic term
         */
        for(i=0; i<=n-1; i++)
        {
            if( needfi )
            {
                fi->ptr.p_double[dst] = fi->ptr.p_double[dst]+problem->nld.ptr.pp_double[k][i]*ae_pow(x->ptr.p_double[i], (double)(4), _state);
            }
            if( needjac )
            {
                jac->ptr.pp_double[dst][i] = jac->ptr.pp_double[dst][i]+problem->nld.ptr.pp_double[k][i]*(double)4*ae_pow(x->ptr.p_double[i], (double)(3), _state);
            }
        }
    }
    ae_frame_leave(_state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t idxout;


    
    /*
     * Compute required amount of single-sided constraints
     */
    *olddensek = 0;
    for(i=0; i<=ksparse+kdense-1; i++)
    {
        ae_assert(ae_isfinite(cl->ptr.p_double[i], _state)||ae_isneginf(cl->ptr.p_double[i], _state), "OPTSERV: integrity check 7117 failed", _state);
        ae_assert(ae_isfinite(cu->ptr.p_double[i], _state)||ae_isposinf(cu->ptr.p_double[i], _state), "OPTSERV: integrity check 7118 failed", _state);
        if( (ae_isfinite(cl->ptr.p_double[i], _state)&&ae_isfinite(cu->ptr.p_double[i], _state))&&ae_fp_eq(cl->ptr.p_double[i],cu->ptr.p_double[i]) )
        {
            *olddensek = *olddensek+1;
            continue;
        }
        if( ae_isfinite(cl->ptr.p_double[i], _state) )
        {
            *olddensek = *olddensek+1;
        }
        if( ae_isfinite(cu->ptr.p_double[i], _state) )
        {
            *olddensek = *olddensek+1;
        }
    }
    if( *olddensek==0 )
    {
        return;
    }
    
    /*
     * Convert two-sided constraints into single-sided
     */
    idxout = 0;
    rallocm(*olddensek, n+1, olddensec, _state);
    iallocv(*olddensek, olddensect, _state);
    for(i=0; i<=ksparse+kdense-1; i++)
    {
        
        /*
         * Ignore non-binding rows
         */
        if( !ae_isfinite(cl->ptr.p_double[i], _state)&&!ae_isfinite(cu->ptr.p_double[i], _state) )
        {
            continue;
        }
        
        /*
         * Extract row
         */
        ae_assert(idxout<(*olddensek), "OPTSERV: integrity check 0324 failed", _state);
        if( i<ksparse )
        {
            
            /*
             * Extract from the sparse matrix
             */
            rsetr(n, 0.0, olddensec, idxout, _state);
            k0 = sparsec->ridx.ptr.p_int[i];
            k1 = sparsec->ridx.ptr.p_int[i+1]-1;
            for(k=k0; k<=k1; k++)
            {
                olddensec->ptr.pp_double[idxout][sparsec->idx.ptr.p_int[k]] = sparsec->vals.ptr.p_double[k];
            }
        }
        else
        {
            
            /*
             * Copy from the dense matrix
             */
            rcopyrr(n, densec, i-ksparse, olddensec, idxout, _state);
        }
        
        /*
         * Two bounds are present
         */
        if( ae_isfinite(cl->ptr.p_double[i], _state)&&ae_isfinite(cu->ptr.p_double[i], _state) )
        {
            if( ae_fp_eq(cl->ptr.p_double[i],cu->ptr.p_double[i]) )
            {
                
                /*
                 * Two bounds = equality constraints, one row is output
                 */
                olddensect->ptr.p_int[idxout] = 0;
                olddensec->ptr.pp_double[idxout][n] = cl->ptr.p_double[i];
                idxout = idxout+1;
                continue;
            }
            else
            {
                
                /*
                 * Two bounds = range constraint, two rows are output
                 */
                rcopyrr(n, olddensec, idxout, olddensec, idxout+1, _state);
                olddensect->ptr.p_int[idxout+0] = 1;
                olddensect->ptr.p_int[idxout+1] = -1;
                olddensec->ptr.pp_double[idxout+0][n] = cl->ptr.p_double[i];
                olddensec->ptr.pp_double[idxout+1][n] = cu->ptr.p_double[i];
                idxout = idxout+2;
                continue;
            }
        }
        
        /*
         * Only one bound is present
         */
        if( ae_isfinite(cl->ptr.p_double[i], _state) )
        {
            olddensect->ptr.p_int[idxout] = 1;
            olddensec->ptr.pp_double[idxout][n] = cl->ptr.p_double[i];
            idxout = idxout+1;
            continue;
        }
        if( ae_isfinite(cu->ptr.p_double[i], _state) )
        {
            olddensect->ptr.p_int[idxout] = -1;
            olddensec->ptr.pp_double[idxout][n] = cu->ptr.p_double[i];
            idxout = idxout+1;
            continue;
        }
        ae_assert(ae_false, "OPTSERV: integrity check 7025 failed", _state);
    }
    ae_assert(idxout==(*olddensek), "OPTSERV: integrity check 0214 failed", _state);
}


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
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t idxoutnlec;
    ae_int_t idxoutnlic;


    
    /*
     * Compute required amount of single-sided constraints
     */
    *cntnlec = 0;
    *cntnlic = 0;
    for(i=0; i<=nnlc-1; i++)
    {
        ae_assert(ae_isfinite(nl->ptr.p_double[i], _state)||ae_isneginf(nl->ptr.p_double[i], _state), "OPTSERV: integrity check 0017 failed", _state);
        ae_assert(ae_isfinite(nu->ptr.p_double[i], _state)||ae_isposinf(nu->ptr.p_double[i], _state), "OPTSERV: integrity check 0018 failed", _state);
        if( (ae_isfinite(nl->ptr.p_double[i], _state)&&ae_isfinite(nu->ptr.p_double[i], _state))&&ae_fp_eq(nl->ptr.p_double[i],nu->ptr.p_double[i]) )
        {
            *cntnlec = *cntnlec+1;
            continue;
        }
        if( ae_isfinite(nl->ptr.p_double[i], _state) )
        {
            *cntnlic = *cntnlic+1;
        }
        if( ae_isfinite(nu->ptr.p_double[i], _state) )
        {
            *cntnlic = *cntnlic+1;
        }
    }
    if( *cntnlec+(*cntnlic)==0 )
    {
        return;
    }
    
    /*
     * Convert two-sided constraints into single-sided ones
     */
    idxoutnlec = 0;
    idxoutnlic = *cntnlec;
    iallocv(*cntnlec+(*cntnlic), nlcidx, _state);
    rallocv(*cntnlec+(*cntnlic), nlcmul, _state);
    rallocv(*cntnlec+(*cntnlic), nlcadd, _state);
    for(i=0; i<=nnlc-1; i++)
    {
        
        /*
         * Ignore non-binding rows
         */
        if( !ae_isfinite(nl->ptr.p_double[i], _state)&&!ae_isfinite(nu->ptr.p_double[i], _state) )
        {
            continue;
        }
        
        /*
         *
         * Two bounds are present
         */
        if( ae_isfinite(nl->ptr.p_double[i], _state)&&ae_isfinite(nu->ptr.p_double[i], _state) )
        {
            if( ae_fp_eq(nl->ptr.p_double[i],nu->ptr.p_double[i]) )
            {
                
                /*
                 * Two bounds = equality constraints, one row is output
                 */
                nlcidx->ptr.p_int[idxoutnlec] = i;
                nlcmul->ptr.p_double[idxoutnlec] = 1.0;
                nlcadd->ptr.p_double[idxoutnlec] = -nl->ptr.p_double[i];
                idxoutnlec = idxoutnlec+1;
                continue;
            }
            else
            {
                
                /*
                 * Two bounds = range constraint, two rows are output
                 */
                nlcidx->ptr.p_int[idxoutnlic+0] = i;
                nlcmul->ptr.p_double[idxoutnlic+0] = -1.0;
                nlcadd->ptr.p_double[idxoutnlic+0] = nl->ptr.p_double[i];
                nlcidx->ptr.p_int[idxoutnlic+1] = i;
                nlcmul->ptr.p_double[idxoutnlic+1] = 1.0;
                nlcadd->ptr.p_double[idxoutnlic+1] = -nu->ptr.p_double[i];
                idxoutnlic = idxoutnlic+2;
                continue;
            }
        }
        
        /*
         * Only one bound is present
         */
        if( ae_isfinite(nl->ptr.p_double[i], _state) )
        {
            nlcidx->ptr.p_int[idxoutnlic+0] = i;
            nlcmul->ptr.p_double[idxoutnlic+0] = -1.0;
            nlcadd->ptr.p_double[idxoutnlic+0] = nl->ptr.p_double[i];
            idxoutnlic = idxoutnlic+1;
            continue;
        }
        if( ae_isfinite(nu->ptr.p_double[i], _state) )
        {
            nlcidx->ptr.p_int[idxoutnlic+0] = i;
            nlcmul->ptr.p_double[idxoutnlic+0] = 1.0;
            nlcadd->ptr.p_double[idxoutnlic+0] = -nu->ptr.p_double[i];
            idxoutnlic = idxoutnlic+1;
            continue;
        }
        ae_assert(ae_false, "OPTSERV: integrity check 9041 failed", _state);
    }
    ae_assert(idxoutnlec==(*cntnlec), "OPTSERV: integrity check 9242 failed", _state);
    ae_assert(idxoutnlic==*cntnlec+(*cntnlic), "OPTSERV: integrity check 9243 failed", _state);
}


/*************************************************************************
Increases trust region growth factor until maximum growth reached.
*************************************************************************/
void trustradincreasemomentum(double* growthfactor,
     double growthincrease,
     double maxgrowthfactor,
     ae_state *_state)
{


    *growthfactor = ae_minreal(*growthfactor*growthincrease, maxgrowthfactor, _state);
}


/*************************************************************************
Resets trust region growth factor to its default state.
*************************************************************************/
void trustradresetmomentum(double* growthfactor,
     double mingrowthfactor,
     ae_state *_state)
{


    *growthfactor = mingrowthfactor;
}


/*************************************************************************
This function calculates feasibility error (square root of sum of  squared
errors) for a Kx(NMain+NSlack) system of linear equalities.
    
INPUT PARAMETERS:
    CE      -   set of K equality constraints, array[K,NMain+NSlack+1]
    X       -   candidate point, array [NMain+NSlack]
    NMain   -   number of primary variables
    NSlack  -   number of slack variables
    K       -   number of constraints
    Tmp0    -   possible preallocated buffer, automatically resized

RESULT:
    Sqrt(SUM(Err^2))
    
  -- ALGLIB --
     Copyright 17.09.2015 by Bochkanov Sergey
*************************************************************************/
static double optserv_feasibilityerror(/* Real    */ const ae_matrix* ce,
     /* Real    */ const ae_vector* x,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t k,
     /* Real    */ ae_vector* tmp0,
     ae_state *_state)
{
    ae_int_t i;
    double result;


    rvectorsetlengthatleast(tmp0, k, _state);
    for(i=0; i<=k-1; i++)
    {
        tmp0->ptr.p_double[i] = -ce->ptr.pp_double[i][nmain+nslack];
    }
    rmatrixgemv(k, nmain+nslack, 1.0, ce, 0, 0, 0, x, 0, 1.0, tmp0, 0, _state);
    result = 0.0;
    for(i=0; i<=k-1; i++)
    {
        result = result+tmp0->ptr.p_double[i]*tmp0->ptr.p_double[i];
    }
    result = ae_sqrt(result, _state);
    return result;
}


/*************************************************************************
This function calculates feasibility error (square root of sum of  squared
errors) for a Kx(NMain+NSlack)  system  of  linear  equalities  and  error
gradient (with respect to x)
    
INPUT PARAMETERS:
    CE      -   set of K equality constraints, array[K,NMain+NSlack+1]
    X       -   candidate point, array [NMain+NSlack]
    NMain   -   number of primary variables
    NSlack  -   number of slack variables
    K       -   number of constraints
    Grad    -   preallocated array[NMain+NSlack]
    Tmp0    -   possible preallocated buffer, automatically resized

RESULT:
    Err     -   Sqrt(SUM(Err^2))
    Grad    -   error gradient with respect to X, array[NMain+NSlack]
    
  -- ALGLIB --
     Copyright 17.09.2015 by Bochkanov Sergey
*************************************************************************/
static void optserv_feasibilityerrorgrad(/* Real    */ const ae_matrix* ce,
     /* Real    */ const ae_vector* x,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t k,
     double* err,
     /* Real    */ ae_vector* grad,
     /* Real    */ ae_vector* tmp0,
     ae_state *_state)
{
    ae_int_t i;
    double v;

    *err = 0.0;

    ae_assert(grad->cnt>=nmain+nslack, "FeasibilityErrorGrad: integrity check failed", _state);
    rvectorsetlengthatleast(tmp0, k, _state);
    rmatrixgemv(k, nmain+nslack, 1.0, ce, 0, 0, 0, x, 0, 0.0, tmp0, 0, _state);
    *err = 0.0;
    for(i=0; i<=k-1; i++)
    {
        v = tmp0->ptr.p_double[i]-ce->ptr.pp_double[i][nmain+nslack];
        tmp0->ptr.p_double[i] = v;
        *err = *err+v*v;
    }
    *err = ae_sqrt(*err, _state);
    rmatrixgemv(nmain+nslack, k, 1.0, ce, 0, 0, 1, tmp0, 0, 0.0, grad, 0, _state);
}


/*************************************************************************
This  subroutine  checks  C0  continuity  and  returns  continuity  rating
(normalized value, with values above 50-500 being good indication  of  the
discontinuity) and Lipschitz constant.

An interval between F1 and F2 is  tested  for  (dis)continuity.  Per-point
noise estimates are provided. Delta[i] is a step from F[i] to F[i+1].

ApplySpecialCorrection parameter should be set to True  if  you  use  this
function to estimate continuity of  the  model  around  minimum;  it  adds
special correction which helps to detect "max(0,1/x)"-like discontinuities.
Without this correction algorithm will still work, but will be a bit  less
powerful. Do not use this correction  for  situations  when  you  want  to
estimate continuity around some non-extremal point  -  it  may  result  in
spurious discontinuities being reported.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
static void optserv_testc0continuity(double f0,
     double f1,
     double f2,
     double f3,
     double noise0,
     double noise1,
     double noise2,
     double noise3,
     double delta0,
     double delta1,
     double delta2,
     ae_bool applyspecialcorrection,
     double* rating,
     double* lipschitz,
     ae_state *_state)
{
    double lipschitz01;
    double lipschitz12;
    double lipschitz23;

    *rating = 0.0;
    *lipschitz = 0.0;

    
    /*
     * Compute Lipschitz constant for the interval [0,1],
     * add noise correction in order to get increased estimate (makes
     * comparison below more conservative).
     */
    lipschitz01 = (ae_fabs(f1-f0, _state)+(noise0+noise1))/delta0;
    
    /*
     * Compute Lipschitz constant for the interval [StpIdx+1,StpIdx+2],
     * SUBTRACT noise correction in order to get decreased estimate (makes
     * comparison below more conservative).
     */
    lipschitz12 = ae_maxreal(ae_fabs(f2-f1, _state)-(noise1+noise2), 0.0, _state)/delta1;
    
    /*
     * Compute Lipschitz constant for the interval [StpIdx+2,StpIdx+3]
     * using special algorithm:
     * a) if F3<F2-Noise23, Lipschitz constant is assumed to be zero
     * b) otherwise, we compute Lipschitz constant as usual,
     *    with noise correction term being added
     *
     * We need case (a) because some kinds of discontinuities
     * (like one introduced by max(1/x,0)) manifest themselves
     * in a quite special way.
     */
    if( applyspecialcorrection&&f3<f2-(noise2+noise3) )
    {
        lipschitz23 = (double)(0);
    }
    else
    {
        lipschitz23 = (ae_fabs(f3-f2, _state)+(noise2+noise3))/delta2;
    }
    
    /*
     * Compute rating (ratio of two Lipschitz constants)
     */
    ae_assert(ae_maxreal(lipschitz01, lipschitz23, _state)>(double)0, "OptGuard: integrity check failed", _state);
    *rating = lipschitz12/ae_maxreal(lipschitz01, lipschitz23, _state);
    *lipschitz = lipschitz12;
}


/*************************************************************************
This subroutine checks C1 continuity using test #0 (function  values  from
the line search log are studied, gradient is not used).

An interval between F[StpIdx+0] and F[StpIdx+5]is  tested for  continuity.
An normalized error metric (Lipschitz constant growth for the  derivative)
for the interval in question is calculated. Values above  50  are  a  good
indication of the discontinuity.

A six-point algorithm is used for testing, so we expect that Monitor.F and
Monitor.Stp have enough points for this test.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
static void optserv_c1continuitytest0(smoothnessmonitor* monitor,
     ae_int_t funcidx,
     ae_int_t stpidx,
     ae_int_t sortedcnt,
     ae_state *_state)
{
    double f0;
    double f1;
    double f2;
    double f3;
    double f4;
    double f5;
    double noise0;
    double noise1;
    double noise2;
    double noise3;
    double noise4;
    double noise5;
    double delta0;
    double delta1;
    double delta2;
    double delta3;
    double delta4;
    double d0;
    double d1;
    double d2;
    double d3;
    double newnoise0;
    double newnoise1;
    double newnoise2;
    double newnoise3;
    double newdelta0;
    double newdelta1;
    double newdelta2;
    double rating;
    double lipschitz;
    double lengthrating;
    ae_int_t i;
    ae_int_t n;
    double nrm;


    n = monitor->n;
    ae_assert(stpidx+5<sortedcnt, "C1ContinuityTest0: integrity check failed", _state);
    ae_assert(ae_fp_eq(monitor->sortedstp.ptr.p_double[0],(double)(0)), "C1ContinuityTest0: integrity check failed", _state);
    ae_assert(ae_fp_greater(monitor->sortedstp.ptr.p_double[sortedcnt-1],(double)(0)), "C1ContinuityTest0: integrity check failed", _state);
    
    /*
     * Fetch F, noise, Delta's
     */
    f0 = monitor->f.ptr.p_double[stpidx+0];
    f1 = monitor->f.ptr.p_double[stpidx+1];
    f2 = monitor->f.ptr.p_double[stpidx+2];
    f3 = monitor->f.ptr.p_double[stpidx+3];
    f4 = monitor->f.ptr.p_double[stpidx+4];
    f5 = monitor->f.ptr.p_double[stpidx+5];
    noise0 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f0, _state), 1.0, _state);
    noise1 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f1, _state), 1.0, _state);
    noise2 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f2, _state), 1.0, _state);
    noise3 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f3, _state), 1.0, _state);
    noise4 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f4, _state), 1.0, _state);
    noise5 = optserv_ognoiselevelf*ae_maxreal(ae_fabs(f5, _state), 1.0, _state);
    delta0 = monitor->sortedstp.ptr.p_double[stpidx+1]-monitor->sortedstp.ptr.p_double[stpidx+0];
    delta1 = monitor->sortedstp.ptr.p_double[stpidx+2]-monitor->sortedstp.ptr.p_double[stpidx+1];
    delta2 = monitor->sortedstp.ptr.p_double[stpidx+3]-monitor->sortedstp.ptr.p_double[stpidx+2];
    delta3 = monitor->sortedstp.ptr.p_double[stpidx+4]-monitor->sortedstp.ptr.p_double[stpidx+3];
    delta4 = monitor->sortedstp.ptr.p_double[stpidx+5]-monitor->sortedstp.ptr.p_double[stpidx+4];
    
    /*
     * Differentiate functions, get derivative values and noise
     * estimates at points (0+1)/2, (1+2)/2, (3+4)/2, (3+4)/2,
     * (4+5)/2. Compute new step values NewDelta[i] and new
     * noise estimates.
     */
    d0 = (f1-f0)/delta0;
    d1 = (f2-f1)/delta1;
    d2 = (f4-f3)/delta3;
    d3 = (f5-f4)/delta4;
    newnoise0 = (noise0+noise1)/delta0;
    newnoise1 = (noise1+noise2)/delta1;
    newnoise2 = (noise3+noise4)/delta3;
    newnoise3 = (noise4+noise5)/delta4;
    newdelta0 = 0.5*(delta0+delta1);
    newdelta1 = 0.5*delta1+delta2+0.5*delta3;
    newdelta2 = 0.5*(delta3+delta4);
    
    /*
     * Test with C0 continuity tester. "Special correction" is
     * turned off for this test.
     */
    optserv_testc0continuity(d0, d1, d2, d3, newnoise0, newnoise1, newnoise2, newnoise3, newdelta0, newdelta1, newdelta2, ae_false, &rating, &lipschitz, _state);
    
    /*
     * Store results
     */
    if( rating>optserv_ogminrating1 )
    {
        
        /*
         * Store to total report
         */
        monitor->rep.nonc1test0positive = ae_true;
        if( rating>monitor->nonc1currentrating )
        {
            monitor->nonc1currentrating = rating;
            monitor->rep.nonc1suspected = ae_true;
            monitor->rep.nonc1lipschitzc = lipschitz;
            monitor->rep.nonc1fidx = funcidx;
        }
        
        /*
         * Store to "strongest" report
         */
        if( rating>monitor->nonc1test0strrating )
        {
            monitor->nonc1test0strrating = rating;
            monitor->nonc1test0strrep.positive = ae_true;
            monitor->nonc1test0strrep.fidx = funcidx;
            monitor->nonc1test0strrep.n = n;
            monitor->nonc1test0strrep.cnt = sortedcnt;
            monitor->nonc1test0strrep.stpidxa = stpidx+1;
            monitor->nonc1test0strrep.stpidxb = stpidx+4;
            monitor->nonc1test0strrep.inneriter = monitor->linesearchinneridx;
            monitor->nonc1test0strrep.outeriter = monitor->linesearchouteridx;
            rvectorsetlengthatleast(&monitor->nonc1test0strrep.x0, n, _state);
            rvectorsetlengthatleast(&monitor->nonc1test0strrep.d, n, _state);
            for(i=0; i<=n-1; i++)
            {
                monitor->nonc1test0strrep.x0.ptr.p_double[i] = monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i];
                monitor->nonc1test0strrep.d.ptr.p_double[i] = monitor->dcur.ptr.p_double[i];
            }
            rvectorsetlengthatleast(&monitor->nonc1test0strrep.stp, sortedcnt, _state);
            rvectorsetlengthatleast(&monitor->nonc1test0strrep.f, sortedcnt, _state);
            for(i=0; i<=sortedcnt-1; i++)
            {
                monitor->nonc1test0strrep.stp.ptr.p_double[i] = monitor->sortedstp.ptr.p_double[i];
                monitor->nonc1test0strrep.f.ptr.p_double[i] = monitor->f.ptr.p_double[i];
            }
        }
        
        /*
         * Store to "longest" report
         */
        nrm = (double)(0);
        for(i=0; i<=n-1; i++)
        {
            nrm = nrm+ae_sqr(monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i]-monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[sortedcnt-1]*n+i], _state);
        }
        nrm = ae_sqrt(nrm, _state);
        nrm = ae_minreal(nrm, 1.0, _state);
        nrm = coalesce(nrm, ae_machineepsilon, _state);
        lengthrating = (double)sortedcnt+ae_log(nrm, _state)/ae_log((double)(100), _state);
        if( lengthrating>monitor->nonc1test0lngrating )
        {
            monitor->nonc1test0lngrating = lengthrating;
            monitor->nonc1test0lngrep.positive = ae_true;
            monitor->nonc1test0lngrep.fidx = funcidx;
            monitor->nonc1test0lngrep.n = n;
            monitor->nonc1test0lngrep.cnt = sortedcnt;
            monitor->nonc1test0lngrep.stpidxa = stpidx+1;
            monitor->nonc1test0lngrep.stpidxb = stpidx+4;
            monitor->nonc1test0lngrep.inneriter = monitor->linesearchinneridx;
            monitor->nonc1test0lngrep.outeriter = monitor->linesearchouteridx;
            rvectorsetlengthatleast(&monitor->nonc1test0lngrep.x0, n, _state);
            rvectorsetlengthatleast(&monitor->nonc1test0lngrep.d, n, _state);
            for(i=0; i<=n-1; i++)
            {
                monitor->nonc1test0lngrep.x0.ptr.p_double[i] = monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i];
                monitor->nonc1test0lngrep.d.ptr.p_double[i] = monitor->dcur.ptr.p_double[i];
            }
            rvectorsetlengthatleast(&monitor->nonc1test0lngrep.stp, sortedcnt, _state);
            rvectorsetlengthatleast(&monitor->nonc1test0lngrep.f, sortedcnt, _state);
            for(i=0; i<=sortedcnt-1; i++)
            {
                monitor->nonc1test0lngrep.stp.ptr.p_double[i] = monitor->sortedstp.ptr.p_double[i];
                monitor->nonc1test0lngrep.f.ptr.p_double[i] = monitor->f.ptr.p_double[i];
            }
        }
    }
}


/*************************************************************************
This  subroutine checks C1 continuity using test #1  (individual  gradient
components from the line search log are studied for continuity).

An interval between F[StpIdx+0] and F[StpIdx+3]is  tested for  continuity.
An normalized error metric (Lipschitz constant growth for the  derivative)
for the interval in question is calculated. Values above  50  are  a  good
indication of the discontinuity.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
static void optserv_c1continuitytest1(smoothnessmonitor* monitor,
     ae_int_t funcidx,
     ae_int_t stpidx,
     ae_int_t sortedcnt,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t varidx;
    ae_int_t n;
    double f0;
    double f1;
    double f2;
    double f3;
    double noise0;
    double noise1;
    double noise2;
    double noise3;
    double nrm;
    double rating;
    double lengthrating;
    double lipschitz;


    n = monitor->n;
    ae_assert(stpidx+3<sortedcnt, "C1ContinuityTest1: integrity check failed", _state);
    ae_assert(ae_fp_eq(monitor->sortedstp.ptr.p_double[0],(double)(0)), "C1ContinuityTest1: integrity check failed", _state);
    ae_assert(ae_fp_greater(monitor->sortedstp.ptr.p_double[sortedcnt-1],(double)(0)), "C1ContinuityTest1: integrity check failed", _state);
    
    /*
     * Study each component of the gradient in the interval in question
     */
    for(varidx=0; varidx<=n-1; varidx++)
    {
        f0 = monitor->g.ptr.p_double[(stpidx+0)*n+varidx];
        f1 = monitor->g.ptr.p_double[(stpidx+1)*n+varidx];
        f2 = monitor->g.ptr.p_double[(stpidx+2)*n+varidx];
        f3 = monitor->g.ptr.p_double[(stpidx+3)*n+varidx];
        noise0 = optserv_ognoiselevelg*ae_maxreal(ae_fabs(f0, _state), 1.0, _state);
        noise1 = optserv_ognoiselevelg*ae_maxreal(ae_fabs(f1, _state), 1.0, _state);
        noise2 = optserv_ognoiselevelg*ae_maxreal(ae_fabs(f2, _state), 1.0, _state);
        noise3 = optserv_ognoiselevelg*ae_maxreal(ae_fabs(f3, _state), 1.0, _state);
        optserv_testc0continuity(f0, f1, f2, f3, noise0, noise1, noise2, noise3, monitor->sortedstp.ptr.p_double[stpidx+1]-monitor->sortedstp.ptr.p_double[stpidx+0], monitor->sortedstp.ptr.p_double[stpidx+2]-monitor->sortedstp.ptr.p_double[stpidx+1], monitor->sortedstp.ptr.p_double[stpidx+3]-monitor->sortedstp.ptr.p_double[stpidx+2], ae_false, &rating, &lipschitz, _state);
        
        /*
         * Store results
         */
        if( rating>optserv_ogminrating1 )
        {
            
            /*
             * Store to total report
             */
            monitor->rep.nonc1test1positive = ae_true;
            if( rating>monitor->nonc1currentrating )
            {
                monitor->nonc1currentrating = rating;
                monitor->rep.nonc1suspected = ae_true;
                monitor->rep.nonc1lipschitzc = lipschitz;
                monitor->rep.nonc1fidx = funcidx;
            }
            
            /*
             * Store to "strongest" report
             */
            if( rating>monitor->nonc1test1strrating )
            {
                monitor->nonc1test1strrating = rating;
                monitor->nonc1test1strrep.positive = ae_true;
                monitor->nonc1test1strrep.fidx = funcidx;
                monitor->nonc1test1strrep.vidx = varidx;
                monitor->nonc1test1strrep.n = n;
                monitor->nonc1test1strrep.cnt = sortedcnt;
                monitor->nonc1test1strrep.stpidxa = stpidx+0;
                monitor->nonc1test1strrep.stpidxb = stpidx+3;
                monitor->nonc1test1strrep.inneriter = monitor->linesearchinneridx;
                monitor->nonc1test1strrep.outeriter = monitor->linesearchouteridx;
                rvectorsetlengthatleast(&monitor->nonc1test1strrep.x0, n, _state);
                rvectorsetlengthatleast(&monitor->nonc1test1strrep.d, n, _state);
                for(i=0; i<=n-1; i++)
                {
                    monitor->nonc1test1strrep.x0.ptr.p_double[i] = monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i];
                    monitor->nonc1test1strrep.d.ptr.p_double[i] = monitor->dcur.ptr.p_double[i];
                }
                rvectorsetlengthatleast(&monitor->nonc1test1strrep.stp, sortedcnt, _state);
                rvectorsetlengthatleast(&monitor->nonc1test1strrep.g, sortedcnt, _state);
                for(i=0; i<=sortedcnt-1; i++)
                {
                    monitor->nonc1test1strrep.stp.ptr.p_double[i] = monitor->sortedstp.ptr.p_double[i];
                    monitor->nonc1test1strrep.g.ptr.p_double[i] = monitor->g.ptr.p_double[i*n+varidx];
                }
            }
            
            /*
             * Store to "longest" report
             */
            nrm = (double)(0);
            for(i=0; i<=n-1; i++)
            {
                nrm = nrm+ae_sqr(monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i]-monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[sortedcnt-1]*n+i], _state);
            }
            nrm = ae_sqrt(nrm, _state);
            nrm = ae_minreal(nrm, 1.0, _state);
            nrm = coalesce(nrm, ae_machineepsilon, _state);
            lengthrating = (double)sortedcnt+ae_log(nrm, _state)/ae_log((double)(100), _state);
            if( lengthrating>monitor->nonc1test1lngrating )
            {
                monitor->nonc1test1lngrating = lengthrating;
                monitor->nonc1test1lngrep.positive = ae_true;
                monitor->nonc1test1lngrep.fidx = funcidx;
                monitor->nonc1test1lngrep.vidx = varidx;
                monitor->nonc1test1lngrep.n = n;
                monitor->nonc1test1lngrep.cnt = sortedcnt;
                monitor->nonc1test1lngrep.stpidxa = stpidx+0;
                monitor->nonc1test1lngrep.stpidxb = stpidx+3;
                monitor->nonc1test1lngrep.inneriter = monitor->linesearchinneridx;
                monitor->nonc1test1lngrep.outeriter = monitor->linesearchouteridx;
                rvectorsetlengthatleast(&monitor->nonc1test1lngrep.x0, n, _state);
                rvectorsetlengthatleast(&monitor->nonc1test1lngrep.d, n, _state);
                for(i=0; i<=n-1; i++)
                {
                    monitor->nonc1test1lngrep.x0.ptr.p_double[i] = monitor->enqueuedx.ptr.p_double[monitor->sortedidx.ptr.p_int[0]*n+i];
                    monitor->nonc1test1lngrep.d.ptr.p_double[i] = monitor->dcur.ptr.p_double[i];
                }
                rvectorsetlengthatleast(&monitor->nonc1test1lngrep.stp, sortedcnt, _state);
                rvectorsetlengthatleast(&monitor->nonc1test1lngrep.g, sortedcnt, _state);
                for(i=0; i<=sortedcnt-1; i++)
                {
                    monitor->nonc1test1lngrep.stp.ptr.p_double[i] = monitor->sortedstp.ptr.p_double[i];
                    monitor->nonc1test1lngrep.g.ptr.p_double[i] = monitor->g.ptr.p_double[i*n+varidx];
                }
            }
        }
    }
}


/*************************************************************************
Removes oldest update pair from the limited memory Hessian model and
invalidates Hessian model

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
static void optserv_popfrontxy(xbfgshessian* hess, ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    if( hess->memlen==0 )
    {
        return;
    }
    for(i=0; i<=hess->memlen-2; i++)
    {
        rcopyrr(hess->n, &hess->s, i+1, &hess->s, i, _state);
        rcopyrr(hess->n, &hess->y, i+1, &hess->y, i, _state);
    }
    for(i=0; i<=hess->memlen-2; i++)
    {
        for(j=0; j<=hess->memlen-2; j++)
        {
            hess->lowranksst.ptr.pp_double[i][j] = hess->lowranksst.ptr.pp_double[i+1][j+1];
            hess->lowranksyt.ptr.pp_double[i][j] = hess->lowranksyt.ptr.pp_double[i+1][j+1];
        }
    }
    hess->memlen = hess->memlen-1;
    optserv_resetlowrankmodel(hess, _state);
}


/*************************************************************************
Lowe-level Hessian update function, to be used by HessianUpdate()

INPUT PARAMETERS:
    Hess            -   Hessian state
    H               -   specific Hessian matrix to update, usually one
                        of the Hess fields
    X0, G0          -   point #0 and gradient at #0, array[N]
    X1, G1          -   point #1 and gradient at #1, array[N]

OUTPUT PARAMETERS:
    Status          -   sets update status (informative)

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
static void optserv_hessianupdatelowlevel(xbfgshessian* hess,
     /* Real    */ ae_matrix* h,
     /* Real    */ const ae_vector* sk,
     /* Real    */ const ae_vector* yk,
     ae_int_t* status,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    double shs;
    double sy;
    double snrm2;
    double hsnrm2;
    double ynrm2;
    double ski;
    double yki;
    double mxs;
    double mxy;
    double mxhs;
    double mxd;
    double big;

    *status = 0;

    n = hess->n;
    *status = 0;
    big = (double)1/hess->reg;
    
    /*
     * Perform preliminary analysis
     */
    rvectorsetlengthatleast(&hess->hsk, n, _state);
    rmatrixgemv(n, n, 1.0, h, 0, 0, 0, sk, 0, 0.0, &hess->hsk, 0, _state);
    shs = (double)(0);
    sy = (double)(0);
    snrm2 = (double)(0);
    ynrm2 = (double)(0);
    mxs = (double)(0);
    mxy = (double)(0);
    mxhs = (double)(0);
    hsnrm2 = (double)(0);
    mxd = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        ski = sk->ptr.p_double[i];
        yki = yk->ptr.p_double[i];
        shs = shs+ski*hess->hsk.ptr.p_double[i];
        sy = sy+ski*yki;
        snrm2 = snrm2+ski*ski;
        ynrm2 = ynrm2+yki*yki;
        mxs = ae_maxreal(mxs, ae_fabs(ski, _state), _state);
        mxy = ae_maxreal(mxy, ae_fabs(yki, _state), _state);
        mxhs = ae_maxreal(mxhs, ae_fabs(hess->hsk.ptr.p_double[i], _state), _state);
        hsnrm2 = hsnrm2+ae_sqr(hess->hsk.ptr.p_double[i], _state);
        mxd = ae_maxreal(mxd, ae_fabs(h->ptr.pp_double[i][i], _state), _state);
    }
    
    /*
     * Completely skip updates with too short steps and degenerate updates
     *
     * NOTE: may prevent us from updating Hessian near the solution
     */
    if( ae_fp_less_eq(mxs,hess->stpshort) )
    {
        
        /*
         * Sk is too small
         */
        return;
    }
    if( ae_fp_eq(hsnrm2,(double)(0)) )
    {
        
        /*
         * H*Sk is exactly zero, exit
         */
        return;
    }
    if( ae_fp_less_eq(shs,(double)(0))||ae_fp_less_eq(shs,mxs*mxd*mxs*hess->microreg) )
    {
        
        /*
         * Sk'*H*Sk is too small.
         *
         * Apply regularization to Hessian before exiting
         */
        ae_assert(ae_fp_greater(hsnrm2,(double)(0)), "UpdateHessian: integrity check failed", _state);
        rmatrixger(n, n, h, 0, 0, hess->reg/hsnrm2, &hess->hsk, 0, &hess->hsk, 0, _state);
        return;
    }
    
    /*
     * First, we discard Hessian components which give non-zero product with Sk.
     *
     * We apply some damping in order to avoid problems arising with very small
     * Sk'*H*Sk, and we apply some regularization term in order to have Sk'*Hnew*Sk
     * still slightly larger than zero.
     *
     * Traditional BFGS update adds -(Hk*Sk)*(Hk*Sk)'/(Sk'*H*Sk). We use
     * modified, more robust formula (below Z=(H*Sk)/|H*Sk|)
     *
     *     (                         (H*Sk,H*Sk)                               )
     *     ( - --------------------------------------------------------- + Reg ) * Z * Z'
     *     (    Sk'*H*Sk + Reg*(H*Sk,H*Sk) + MicroReg*(max(H)*max(S))^2        )
     */
    ae_assert(ae_fp_greater(hsnrm2,(double)(0)), "UpdateHessian: integrity check failed", _state);
    rmatrixger(n, n, h, 0, 0, -(double)1/(shs+hsnrm2*hess->reg+ae_sqr(mxd*mxs, _state)*hess->microreg)+hess->reg/hsnrm2, &hess->hsk, 0, &hess->hsk, 0, _state);
    *status = 1;
    
    /*
     * Before we update Hessian with Yk, decide whether we need this update - or
     * maybe it is better to leave Hessian as is, with small curvature along Sk
     * (in the latter case we still treat BFGS update as successful).
     *
     * Traditional BFGS update adds Yk*Yk'/(Sk,Yk) to the Hessian. Instead we
     * use modified update (below U=Yk/|Yk|)
     *
     *               (Yk,Yk)
     *     -------------------------------------- * U * U'
     *      (Sk,Yk)+Reg*(Yk,Yk)+MicroReg*(Sk,Sk)
     */
    if( ae_fp_eq(ynrm2,(double)(0)) )
    {
        return;
    }
    if( ae_fp_less_eq(sy,(double)(0)) )
    {
        return;
    }
    if( ae_fp_greater_eq(ae_sqr(mxy, _state)/sy,big) )
    {
        return;
    }
    ae_assert(ae_fp_greater(sy,(double)(0)), "UpdateHessian: integrity check failed", _state);
    rmatrixger(n, n, h, 0, 0, (double)1/(sy+hess->reg*ynrm2+hess->microreg*snrm2), yk, 0, yk, 0, _state);
    *status = 2;
}


/*************************************************************************
Invalidate low-rank model

INPUT PARAMETERS:
    Hess            -   Hessian state

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
static void optserv_resetlowrankmodel(xbfgshessian* hess,
     ae_state *_state)
{


    ae_assert(hess->htype==3, "OPTSERV: integrity check 9940 failed", _state);
    hess->lowrankmodelvalid = ae_false;
    hess->lowrankeffdvalid = ae_false;
}


/*************************************************************************
Recomputes low-rank model (DIAG and CORR fields) according to the  current
state of the LBFGS memory

INPUT PARAMETERS:
    Hess            -   Hessian state

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
static void optserv_recomputelowrankmodel(xbfgshessian* hess,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t memlen;
    ae_int_t i;
    ae_int_t j;
    ae_bool success;


    n = hess->n;
    memlen = hess->memlen;
    
    /*
     * If the model is valid, exit.
     * Otherwise, recompute it from scratch
     */
    if( hess->lowrankmodelvalid )
    {
        return;
    }
    optserv_resetlowrankmodel(hess, _state);
    
    /*
     * Quick exit for MemLen=0.
     * After this block we assume that MemLen>0
     */
    if( memlen==0 )
    {
        hess->lowrankmodelvalid = ae_true;
        hess->lowrankk = 0;
        return;
    }
    
    /*
     * Prepare RAW_CORR2, a right part of correction matrix
     *
     *     Bk = sigma*I - RAW_CORR2'*RAW_BLOCK*RAW_CORR2
     *
     * with RAW_CORR2 being 2MEMLEN*N matrix and RAW_BLOCK being 2MEMLEN*2MEMLEN matrix,
     * as defined by equations 2.17 and 3.22 in 'REPRESENTATION OF QUASI-NEWTON MATRICES
     * AND THEIR USE IN LIMITED MEMORY METHODS' by Byrd, Nocedal and Schnabel.
     *
     * The initial form for Bk is
     *
     *                    [          ]   [ -Dk     Lk' ]-1 [  Y   ]
     *     Bk = sigma*I - [ Y' B0*S' ] * [             ] * [      ]
     *                    [          ]   [ Lk  S*B0*S' ]   [ S*B0 ]
     *
     * with
     *
     *     Lk[i,j]=(Si,Yj) for i>j, 0 otherwise
     *     Dk = diag[(Si,Yj)]
     *
     */
    rallocm(2*memlen, n, &hess->corr2, _state);
    for(i=0; i<=memlen-1; i++)
    {
        rcopyrr(n, &hess->s, i, &hess->corr2, memlen+i, _state);
        rmulr(n, hess->sigma, &hess->corr2, memlen+i, _state);
        rcopyrr(n, &hess->y, i, &hess->corr2, i, _state);
    }
    
    /*
     * Start factorizing central block. Whilst it is indefinite, it has
     * pretty simple factorization
     *
     *     [ -Dk     Lk' ]   [ Dk^(0.5)          ]   [ -I      ]   [ Dk^(0.5)  -Dk^(-0.5)*Lk' ]
     *     [             ] = [                   ] * [         ] * [                          ]
     *     [ Lk  S*B0*S' ]   [ -Lk*Dk^(-0.5)  Jk ]   [      +I ]   [                      Jk' ]
     *
     * First, we compute lower triangular Jk such that Jk*Jk' = S*B0*S' + Lk*inv(Dk)*Lk'
     */
    rallocv(memlen, &hess->buf, _state);
    for(i=0; i<=memlen-1; i++)
    {
        hess->buf.ptr.p_double[i] = (double)1/ae_sqrt(hess->lowranksyt.ptr.pp_double[i][i], _state);
    }
    rsetallocm(memlen, memlen, 0.0, &hess->invsqrtdlk, _state);
    for(i=1; i<=memlen-1; i++)
    {
        rcopyrr(i, &hess->lowranksyt, i, &hess->invsqrtdlk, i, _state);
        rmergemulvr(i, &hess->buf, &hess->invsqrtdlk, i, _state);
    }
    rcopyallocm(memlen, memlen, &hess->lowranksst, &hess->jk, _state);
    rmatrixgemm(memlen, memlen, memlen, 1.0, &hess->invsqrtdlk, 0, 0, 0, &hess->invsqrtdlk, 0, 0, 1, hess->sigma, &hess->jk, 0, 0, _state);
    success = spdmatrixcholeskyrec(&hess->jk, 0, memlen, ae_false, &hess->buf, _state);
    ae_assert(success, "OPTSERV: integrity check 9828 failed", _state);
    
    /*
     * After computing Jk we proceed to form triangular factorization of the entire block matrix
     */
    rsetallocm(2*memlen, 2*memlen, 0.0, &hess->blk, _state);
    for(i=0; i<=memlen-1; i++)
    {
        hess->blk.ptr.pp_double[i][i] = ae_sqrt(hess->lowranksyt.ptr.pp_double[i][i], _state);
    }
    for(i=0; i<=memlen-1; i++)
    {
        for(j=0; j<=i-1; j++)
        {
            hess->blk.ptr.pp_double[memlen+i][j] = -hess->invsqrtdlk.ptr.pp_double[i][j];
        }
    }
    for(i=0; i<=memlen-1; i++)
    {
        for(j=0; j<=i; j++)
        {
            hess->blk.ptr.pp_double[memlen+i][memlen+j] = hess->jk.ptr.pp_double[i][j];
        }
    }
    
    /*
     * And finally we merge triangular factor into CORR2 in order to get desired two-factor
     * low rank representation without additional middle matrix. This representation is
     * computed in CORR2 and unpacked into LowRankCp and LowRankCm ('minus' and 'plus' terms).
     */
    rmatrixlefttrsm(2*memlen, n, &hess->blk, 0, 0, ae_false, ae_false, 0, &hess->corr2, 0, 0, _state);
    rallocm(memlen, n, &hess->lowrankcp, _state);
    for(i=0; i<=memlen-1; i++)
    {
        rcopyrr(n, &hess->corr2, i, &hess->lowrankcp, i, _state);
    }
    rallocm(memlen, n, &hess->lowrankcm, _state);
    for(i=0; i<=memlen-1; i++)
    {
        rcopyrr(n, &hess->corr2, memlen+i, &hess->lowrankcm, i, _state);
    }
    
    /*
     * The model was created
     */
    hess->lowrankmodelvalid = ae_true;
    hess->lowrankk = memlen;
}


/*************************************************************************
Recomputes diagonal of the low-rank model (EFFD) according to the  current
state of the LBFGS memory

INPUT PARAMETERS:
    Hess            -   Hessian state

  -- ALGLIB --
     Copyright 28.11.2022 by Bochkanov Sergey
*************************************************************************/
static void optserv_recomputelowrankdiagonal(xbfgshessian* hess,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t memlen;
    ae_int_t i;


    n = hess->n;
    memlen = hess->memlen;
    
    /*
     * If the diagonal is valid, exit.
     * Otherwise, recompute it from scratch
     */
    if( hess->lowrankeffdvalid )
    {
        return;
    }
    optserv_recomputelowrankmodel(hess, _state);
    
    /*
     * Quick exit for MemLen=0
     */
    if( memlen==0 )
    {
        hess->lowrankeffdvalid = ae_true;
        rsetallocv(n, hess->sigma, &hess->lowrankeffd, _state);
        return;
    }
    
    /*
     * Assuming MemLen>0
     */
    rsetallocv(n, hess->sigma, &hess->lowrankeffd, _state);
    rallocv(n, &hess->buf, _state);
    for(i=0; i<=hess->lowrankk-1; i++)
    {
        rcopyrv(n, &hess->lowrankcp, i, &hess->buf, _state);
        rmuladdv(n, &hess->buf, &hess->buf, &hess->lowrankeffd, _state);
        rcopyrv(n, &hess->lowrankcm, i, &hess->buf, _state);
        rnegmuladdv(n, &hess->buf, &hess->buf, &hess->lowrankeffd, _state);
    }
    hess->lowrankeffdvalid = ae_true;
}


void _precbuflbfgs_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    precbuflbfgs *p = (precbuflbfgs*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->norms, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->alpha, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rho, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->yk, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->idx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->bufa, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bufb, 0, DT_INT, _state, make_automatic);
}


void _precbuflbfgs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    precbuflbfgs       *dst = (precbuflbfgs*)_dst;
    const precbuflbfgs *src = (const precbuflbfgs*)_src;
    ae_vector_init_copy(&dst->norms, &src->norms, _state, make_automatic);
    ae_vector_init_copy(&dst->alpha, &src->alpha, _state, make_automatic);
    ae_vector_init_copy(&dst->rho, &src->rho, _state, make_automatic);
    ae_matrix_init_copy(&dst->yk, &src->yk, _state, make_automatic);
    ae_vector_init_copy(&dst->idx, &src->idx, _state, make_automatic);
    ae_vector_init_copy(&dst->bufa, &src->bufa, _state, make_automatic);
    ae_vector_init_copy(&dst->bufb, &src->bufb, _state, make_automatic);
}


void _precbuflbfgs_clear(void* _p)
{
    precbuflbfgs *p = (precbuflbfgs*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->norms);
    ae_vector_clear(&p->alpha);
    ae_vector_clear(&p->rho);
    ae_matrix_clear(&p->yk);
    ae_vector_clear(&p->idx);
    ae_vector_clear(&p->bufa);
    ae_vector_clear(&p->bufb);
}


void _precbuflbfgs_destroy(void* _p)
{
    precbuflbfgs *p = (precbuflbfgs*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->norms);
    ae_vector_destroy(&p->alpha);
    ae_vector_destroy(&p->rho);
    ae_matrix_destroy(&p->yk);
    ae_vector_destroy(&p->idx);
    ae_vector_destroy(&p->bufa);
    ae_vector_destroy(&p->bufb);
}


void _precbuflowrank_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    precbuflowrank *p = (precbuflowrank*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->v, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bufc, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->bufz, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->bufw, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp, 0, DT_REAL, _state, make_automatic);
}


void _precbuflowrank_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    precbuflowrank       *dst = (precbuflowrank*)_dst;
    const precbuflowrank *src = (const precbuflowrank*)_src;
    dst->n = src->n;
    dst->k = src->k;
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    ae_matrix_init_copy(&dst->v, &src->v, _state, make_automatic);
    ae_vector_init_copy(&dst->bufc, &src->bufc, _state, make_automatic);
    ae_matrix_init_copy(&dst->bufz, &src->bufz, _state, make_automatic);
    ae_matrix_init_copy(&dst->bufw, &src->bufw, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp, &src->tmp, _state, make_automatic);
}


void _precbuflowrank_clear(void* _p)
{
    precbuflowrank *p = (precbuflowrank*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->d);
    ae_matrix_clear(&p->v);
    ae_vector_clear(&p->bufc);
    ae_matrix_clear(&p->bufz);
    ae_matrix_clear(&p->bufw);
    ae_vector_clear(&p->tmp);
}


void _precbuflowrank_destroy(void* _p)
{
    precbuflowrank *p = (precbuflowrank*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->d);
    ae_matrix_destroy(&p->v);
    ae_vector_destroy(&p->bufc);
    ae_matrix_destroy(&p->bufz);
    ae_matrix_destroy(&p->bufw);
    ae_vector_destroy(&p->tmp);
}


void _xbfgshessian_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    xbfgshessian *p = (xbfgshessian*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->hcurrent, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->s, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->y, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->lowrankcp, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->lowrankcm, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lowrankeffd, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->lowranksst, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->lowranksyt, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->hincoming, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->yk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hsk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->buf, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->corr2, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->blk, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->jk, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->invsqrtdlk, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bufvmv, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bufupdhx, 0, DT_REAL, _state, make_automatic);
}


void _xbfgshessian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    xbfgshessian       *dst = (xbfgshessian*)_dst;
    const xbfgshessian *src = (const xbfgshessian*)_src;
    dst->htype = src->htype;
    dst->n = src->n;
    dst->resetfreq = src->resetfreq;
    dst->stpshort = src->stpshort;
    dst->gammasml = src->gammasml;
    dst->reg = src->reg;
    dst->smallreg = src->smallreg;
    dst->microreg = src->microreg;
    dst->wolfeeps = src->wolfeeps;
    dst->maxhess = src->maxhess;
    dst->m = src->m;
    ae_matrix_init_copy(&dst->hcurrent, &src->hcurrent, _state, make_automatic);
    dst->hage = src->hage;
    dst->sumy2 = src->sumy2;
    dst->sums2 = src->sums2;
    dst->sumsy = src->sumsy;
    dst->memlen = src->memlen;
    dst->sigma = src->sigma;
    dst->gamma = src->gamma;
    ae_matrix_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_matrix_init_copy(&dst->y, &src->y, _state, make_automatic);
    dst->sigmadecay = src->sigmadecay;
    dst->lowrankmodelvalid = src->lowrankmodelvalid;
    dst->lowrankk = src->lowrankk;
    ae_matrix_init_copy(&dst->lowrankcp, &src->lowrankcp, _state, make_automatic);
    ae_matrix_init_copy(&dst->lowrankcm, &src->lowrankcm, _state, make_automatic);
    dst->lowrankeffdvalid = src->lowrankeffdvalid;
    ae_vector_init_copy(&dst->lowrankeffd, &src->lowrankeffd, _state, make_automatic);
    ae_matrix_init_copy(&dst->lowranksst, &src->lowranksst, _state, make_automatic);
    ae_matrix_init_copy(&dst->lowranksyt, &src->lowranksyt, _state, make_automatic);
    dst->updatestatus = src->updatestatus;
    ae_matrix_init_copy(&dst->hincoming, &src->hincoming, _state, make_automatic);
    ae_vector_init_copy(&dst->sk, &src->sk, _state, make_automatic);
    ae_vector_init_copy(&dst->yk, &src->yk, _state, make_automatic);
    ae_vector_init_copy(&dst->hsk, &src->hsk, _state, make_automatic);
    ae_vector_init_copy(&dst->buf, &src->buf, _state, make_automatic);
    ae_matrix_init_copy(&dst->corr2, &src->corr2, _state, make_automatic);
    ae_matrix_init_copy(&dst->blk, &src->blk, _state, make_automatic);
    ae_matrix_init_copy(&dst->jk, &src->jk, _state, make_automatic);
    ae_matrix_init_copy(&dst->invsqrtdlk, &src->invsqrtdlk, _state, make_automatic);
    ae_vector_init_copy(&dst->bufvmv, &src->bufvmv, _state, make_automatic);
    ae_vector_init_copy(&dst->bufupdhx, &src->bufupdhx, _state, make_automatic);
}


void _xbfgshessian_clear(void* _p)
{
    xbfgshessian *p = (xbfgshessian*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->hcurrent);
    ae_matrix_clear(&p->s);
    ae_matrix_clear(&p->y);
    ae_matrix_clear(&p->lowrankcp);
    ae_matrix_clear(&p->lowrankcm);
    ae_vector_clear(&p->lowrankeffd);
    ae_matrix_clear(&p->lowranksst);
    ae_matrix_clear(&p->lowranksyt);
    ae_matrix_clear(&p->hincoming);
    ae_vector_clear(&p->sk);
    ae_vector_clear(&p->yk);
    ae_vector_clear(&p->hsk);
    ae_vector_clear(&p->buf);
    ae_matrix_clear(&p->corr2);
    ae_matrix_clear(&p->blk);
    ae_matrix_clear(&p->jk);
    ae_matrix_clear(&p->invsqrtdlk);
    ae_vector_clear(&p->bufvmv);
    ae_vector_clear(&p->bufupdhx);
}


void _xbfgshessian_destroy(void* _p)
{
    xbfgshessian *p = (xbfgshessian*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->hcurrent);
    ae_matrix_destroy(&p->s);
    ae_matrix_destroy(&p->y);
    ae_matrix_destroy(&p->lowrankcp);
    ae_matrix_destroy(&p->lowrankcm);
    ae_vector_destroy(&p->lowrankeffd);
    ae_matrix_destroy(&p->lowranksst);
    ae_matrix_destroy(&p->lowranksyt);
    ae_matrix_destroy(&p->hincoming);
    ae_vector_destroy(&p->sk);
    ae_vector_destroy(&p->yk);
    ae_vector_destroy(&p->hsk);
    ae_vector_destroy(&p->buf);
    ae_matrix_destroy(&p->corr2);
    ae_matrix_destroy(&p->blk);
    ae_matrix_destroy(&p->jk);
    ae_matrix_destroy(&p->invsqrtdlk);
    ae_vector_destroy(&p->bufvmv);
    ae_vector_destroy(&p->bufupdhx);
}


void _smoothnessmonitor_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    smoothnessmonitor *p = (smoothnessmonitor*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dcur, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->enqueuedstp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->enqueuedx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->enqueuedfunc, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->enqueuedjac, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sortedstp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sortedidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->lagprobxs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagprobd, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagprobx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagprobfi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->lagprobj, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->lagprobvalues, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->lagprobjacobians, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagprobsteps, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagproblagrangians, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->lagrangianprobingrcomm, _state, make_automatic);
    _optguardreport_init(&p->rep, _state, make_automatic);
    _optguardnonc0report_init(&p->nonc0strrep, _state, make_automatic);
    _optguardnonc0report_init(&p->nonc0lngrep, _state, make_automatic);
    _optguardnonc1test0report_init(&p->nonc1test0strrep, _state, make_automatic);
    _optguardnonc1test0report_init(&p->nonc1test0lngrep, _state, make_automatic);
    _optguardnonc1test1report_init(&p->nonc1test1strrep, _state, make_automatic);
    _optguardnonc1test1report_init(&p->nonc1test1lngrep, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstateg0, _state, make_automatic);
    ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fm, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->jm, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->jc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->jp, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->jbaseusr, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->jbasenum, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bufr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->f, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->deltax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->bufi, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->xu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->du, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->f0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j0, 0, 0, DT_REAL, _state, make_automatic);
}


void _smoothnessmonitor_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    smoothnessmonitor       *dst = (smoothnessmonitor*)_dst;
    const smoothnessmonitor *src = (const smoothnessmonitor*)_src;
    dst->n = src->n;
    dst->k = src->k;
    dst->checksmoothness = src->checksmoothness;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->dcur, &src->dcur, _state, make_automatic);
    dst->enqueuedcnt = src->enqueuedcnt;
    ae_vector_init_copy(&dst->enqueuedstp, &src->enqueuedstp, _state, make_automatic);
    ae_vector_init_copy(&dst->enqueuedx, &src->enqueuedx, _state, make_automatic);
    ae_vector_init_copy(&dst->enqueuedfunc, &src->enqueuedfunc, _state, make_automatic);
    ae_matrix_init_copy(&dst->enqueuedjac, &src->enqueuedjac, _state, make_automatic);
    ae_vector_init_copy(&dst->sortedstp, &src->sortedstp, _state, make_automatic);
    ae_vector_init_copy(&dst->sortedidx, &src->sortedidx, _state, make_automatic);
    dst->sortedcnt = src->sortedcnt;
    dst->lagprobinneriter = src->lagprobinneriter;
    dst->lagprobouteriter = src->lagprobouteriter;
    dst->lagprobstepmax = src->lagprobstepmax;
    dst->lagprobnstepsstored = src->lagprobnstepsstored;
    ae_vector_init_copy(&dst->lagprobxs, &src->lagprobxs, _state, make_automatic);
    ae_vector_init_copy(&dst->lagprobd, &src->lagprobd, _state, make_automatic);
    dst->lagprobstp = src->lagprobstp;
    ae_vector_init_copy(&dst->lagprobx, &src->lagprobx, _state, make_automatic);
    ae_vector_init_copy(&dst->lagprobfi, &src->lagprobfi, _state, make_automatic);
    dst->lagprobrawlag = src->lagprobrawlag;
    ae_matrix_init_copy(&dst->lagprobj, &src->lagprobj, _state, make_automatic);
    ae_matrix_init_copy(&dst->lagprobvalues, &src->lagprobvalues, _state, make_automatic);
    ae_matrix_init_copy(&dst->lagprobjacobians, &src->lagprobjacobians, _state, make_automatic);
    ae_vector_init_copy(&dst->lagprobsteps, &src->lagprobsteps, _state, make_automatic);
    ae_vector_init_copy(&dst->lagproblagrangians, &src->lagproblagrangians, _state, make_automatic);
    _rcommstate_init_copy(&dst->lagrangianprobingrcomm, &src->lagrangianprobingrcomm, _state, make_automatic);
    dst->linesearchspoiled = src->linesearchspoiled;
    dst->linesearchstarted = src->linesearchstarted;
    dst->linesearchinneridx = src->linesearchinneridx;
    dst->linesearchouteridx = src->linesearchouteridx;
    dst->nonc0currentrating = src->nonc0currentrating;
    dst->nonc1currentrating = src->nonc1currentrating;
    dst->badgradhasxj = src->badgradhasxj;
    _optguardreport_init_copy(&dst->rep, &src->rep, _state, make_automatic);
    dst->nonc0strrating = src->nonc0strrating;
    dst->nonc0lngrating = src->nonc0lngrating;
    _optguardnonc0report_init_copy(&dst->nonc0strrep, &src->nonc0strrep, _state, make_automatic);
    _optguardnonc0report_init_copy(&dst->nonc0lngrep, &src->nonc0lngrep, _state, make_automatic);
    dst->nonc1test0strrating = src->nonc1test0strrating;
    dst->nonc1test0lngrating = src->nonc1test0lngrating;
    _optguardnonc1test0report_init_copy(&dst->nonc1test0strrep, &src->nonc1test0strrep, _state, make_automatic);
    _optguardnonc1test0report_init_copy(&dst->nonc1test0lngrep, &src->nonc1test0lngrep, _state, make_automatic);
    dst->nonc1test1strrating = src->nonc1test1strrating;
    dst->nonc1test1lngrating = src->nonc1test1lngrating;
    _optguardnonc1test1report_init_copy(&dst->nonc1test1strrep, &src->nonc1test1strrep, _state, make_automatic);
    _optguardnonc1test1report_init_copy(&dst->nonc1test1lngrep, &src->nonc1test1lngrep, _state, make_automatic);
    dst->needfij = src->needfij;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstateg0, &src->rstateg0, _state, make_automatic);
    ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic);
    ae_vector_init_copy(&dst->fbase, &src->fbase, _state, make_automatic);
    ae_vector_init_copy(&dst->fm, &src->fm, _state, make_automatic);
    ae_vector_init_copy(&dst->fc, &src->fc, _state, make_automatic);
    ae_vector_init_copy(&dst->fp, &src->fp, _state, make_automatic);
    ae_vector_init_copy(&dst->jm, &src->jm, _state, make_automatic);
    ae_vector_init_copy(&dst->jc, &src->jc, _state, make_automatic);
    ae_vector_init_copy(&dst->jp, &src->jp, _state, make_automatic);
    ae_matrix_init_copy(&dst->jbaseusr, &src->jbaseusr, _state, make_automatic);
    ae_matrix_init_copy(&dst->jbasenum, &src->jbasenum, _state, make_automatic);
    ae_vector_init_copy(&dst->stp, &src->stp, _state, make_automatic);
    ae_vector_init_copy(&dst->bufr, &src->bufr, _state, make_automatic);
    ae_vector_init_copy(&dst->f, &src->f, _state, make_automatic);
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    ae_vector_init_copy(&dst->deltax, &src->deltax, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpidx, &src->tmpidx, _state, make_automatic);
    ae_vector_init_copy(&dst->bufi, &src->bufi, _state, make_automatic);
    ae_vector_init_copy(&dst->xu, &src->xu, _state, make_automatic);
    ae_vector_init_copy(&dst->du, &src->du, _state, make_automatic);
    ae_vector_init_copy(&dst->f0, &src->f0, _state, make_automatic);
    ae_matrix_init_copy(&dst->j0, &src->j0, _state, make_automatic);
}


void _smoothnessmonitor_clear(void* _p)
{
    smoothnessmonitor *p = (smoothnessmonitor*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->dcur);
    ae_vector_clear(&p->enqueuedstp);
    ae_vector_clear(&p->enqueuedx);
    ae_vector_clear(&p->enqueuedfunc);
    ae_matrix_clear(&p->enqueuedjac);
    ae_vector_clear(&p->sortedstp);
    ae_vector_clear(&p->sortedidx);
    ae_vector_clear(&p->lagprobxs);
    ae_vector_clear(&p->lagprobd);
    ae_vector_clear(&p->lagprobx);
    ae_vector_clear(&p->lagprobfi);
    ae_matrix_clear(&p->lagprobj);
    ae_matrix_clear(&p->lagprobvalues);
    ae_matrix_clear(&p->lagprobjacobians);
    ae_vector_clear(&p->lagprobsteps);
    ae_vector_clear(&p->lagproblagrangians);
    _rcommstate_clear(&p->lagrangianprobingrcomm);
    _optguardreport_clear(&p->rep);
    _optguardnonc0report_clear(&p->nonc0strrep);
    _optguardnonc0report_clear(&p->nonc0lngrep);
    _optguardnonc1test0report_clear(&p->nonc1test0strrep);
    _optguardnonc1test0report_clear(&p->nonc1test0lngrep);
    _optguardnonc1test1report_clear(&p->nonc1test1strrep);
    _optguardnonc1test1report_clear(&p->nonc1test1lngrep);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    _rcommstate_clear(&p->rstateg0);
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->fbase);
    ae_vector_clear(&p->fm);
    ae_vector_clear(&p->fc);
    ae_vector_clear(&p->fp);
    ae_vector_clear(&p->jm);
    ae_vector_clear(&p->jc);
    ae_vector_clear(&p->jp);
    ae_matrix_clear(&p->jbaseusr);
    ae_matrix_clear(&p->jbasenum);
    ae_vector_clear(&p->stp);
    ae_vector_clear(&p->bufr);
    ae_vector_clear(&p->f);
    ae_vector_clear(&p->g);
    ae_vector_clear(&p->deltax);
    ae_vector_clear(&p->tmpidx);
    ae_vector_clear(&p->bufi);
    ae_vector_clear(&p->xu);
    ae_vector_clear(&p->du);
    ae_vector_clear(&p->f0);
    ae_matrix_clear(&p->j0);
}


void _smoothnessmonitor_destroy(void* _p)
{
    smoothnessmonitor *p = (smoothnessmonitor*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->dcur);
    ae_vector_destroy(&p->enqueuedstp);
    ae_vector_destroy(&p->enqueuedx);
    ae_vector_destroy(&p->enqueuedfunc);
    ae_matrix_destroy(&p->enqueuedjac);
    ae_vector_destroy(&p->sortedstp);
    ae_vector_destroy(&p->sortedidx);
    ae_vector_destroy(&p->lagprobxs);
    ae_vector_destroy(&p->lagprobd);
    ae_vector_destroy(&p->lagprobx);
    ae_vector_destroy(&p->lagprobfi);
    ae_matrix_destroy(&p->lagprobj);
    ae_matrix_destroy(&p->lagprobvalues);
    ae_matrix_destroy(&p->lagprobjacobians);
    ae_vector_destroy(&p->lagprobsteps);
    ae_vector_destroy(&p->lagproblagrangians);
    _rcommstate_destroy(&p->lagrangianprobingrcomm);
    _optguardreport_destroy(&p->rep);
    _optguardnonc0report_destroy(&p->nonc0strrep);
    _optguardnonc0report_destroy(&p->nonc0lngrep);
    _optguardnonc1test0report_destroy(&p->nonc1test0strrep);
    _optguardnonc1test0report_destroy(&p->nonc1test0lngrep);
    _optguardnonc1test1report_destroy(&p->nonc1test1strrep);
    _optguardnonc1test1report_destroy(&p->nonc1test1lngrep);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    _rcommstate_destroy(&p->rstateg0);
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->fbase);
    ae_vector_destroy(&p->fm);
    ae_vector_destroy(&p->fc);
    ae_vector_destroy(&p->fp);
    ae_vector_destroy(&p->jm);
    ae_vector_destroy(&p->jc);
    ae_vector_destroy(&p->jp);
    ae_matrix_destroy(&p->jbaseusr);
    ae_matrix_destroy(&p->jbasenum);
    ae_vector_destroy(&p->stp);
    ae_vector_destroy(&p->bufr);
    ae_vector_destroy(&p->f);
    ae_vector_destroy(&p->g);
    ae_vector_destroy(&p->deltax);
    ae_vector_destroy(&p->tmpidx);
    ae_vector_destroy(&p->bufi);
    ae_vector_destroy(&p->xu);
    ae_vector_destroy(&p->du);
    ae_vector_destroy(&p->f0);
    ae_matrix_destroy(&p->j0);
}


void _multiobjectivetestfunction_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    multiobjectivetestfunction *p = (multiobjectivetestfunction*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->xsol, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tgtc, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tgtb, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tgta, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tgtd, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->densea, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->al, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->au, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nlc, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->nlb, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->nla, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->nld, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hu, 0, DT_REAL, _state, make_automatic);
}


void _multiobjectivetestfunction_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    multiobjectivetestfunction       *dst = (multiobjectivetestfunction*)_dst;
    const multiobjectivetestfunction *src = (const multiobjectivetestfunction*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->nactive = src->nactive;
    ae_matrix_init_copy(&dst->xsol, &src->xsol, _state, make_automatic);
    ae_vector_init_copy(&dst->tgtc, &src->tgtc, _state, make_automatic);
    ae_matrix_init_copy(&dst->tgtb, &src->tgtb, _state, make_automatic);
    ae_matrix_init_copy(&dst->tgta, &src->tgta, _state, make_automatic);
    ae_matrix_init_copy(&dst->tgtd, &src->tgtd, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_matrix_init_copy(&dst->densea, &src->densea, _state, make_automatic);
    ae_vector_init_copy(&dst->al, &src->al, _state, make_automatic);
    ae_vector_init_copy(&dst->au, &src->au, _state, make_automatic);
    dst->nlinear = src->nlinear;
    ae_vector_init_copy(&dst->nlc, &src->nlc, _state, make_automatic);
    ae_matrix_init_copy(&dst->nlb, &src->nlb, _state, make_automatic);
    ae_matrix_init_copy(&dst->nla, &src->nla, _state, make_automatic);
    ae_matrix_init_copy(&dst->nld, &src->nld, _state, make_automatic);
    ae_vector_init_copy(&dst->hl, &src->hl, _state, make_automatic);
    ae_vector_init_copy(&dst->hu, &src->hu, _state, make_automatic);
    dst->nnonlinear = src->nnonlinear;
}


void _multiobjectivetestfunction_clear(void* _p)
{
    multiobjectivetestfunction *p = (multiobjectivetestfunction*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->xsol);
    ae_vector_clear(&p->tgtc);
    ae_matrix_clear(&p->tgtb);
    ae_matrix_clear(&p->tgta);
    ae_matrix_clear(&p->tgtd);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_matrix_clear(&p->densea);
    ae_vector_clear(&p->al);
    ae_vector_clear(&p->au);
    ae_vector_clear(&p->nlc);
    ae_matrix_clear(&p->nlb);
    ae_matrix_clear(&p->nla);
    ae_matrix_clear(&p->nld);
    ae_vector_clear(&p->hl);
    ae_vector_clear(&p->hu);
}


void _multiobjectivetestfunction_destroy(void* _p)
{
    multiobjectivetestfunction *p = (multiobjectivetestfunction*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->xsol);
    ae_vector_destroy(&p->tgtc);
    ae_matrix_destroy(&p->tgtb);
    ae_matrix_destroy(&p->tgta);
    ae_matrix_destroy(&p->tgtd);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_matrix_destroy(&p->densea);
    ae_vector_destroy(&p->al);
    ae_vector_destroy(&p->au);
    ae_vector_destroy(&p->nlc);
    ae_matrix_destroy(&p->nlb);
    ae_matrix_destroy(&p->nla);
    ae_matrix_destroy(&p->nld);
    ae_vector_destroy(&p->hl);
    ae_vector_destroy(&p->hu);
}


/*$ End $*/
