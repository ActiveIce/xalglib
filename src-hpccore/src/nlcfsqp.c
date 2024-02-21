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
#include <stdio.h>
#include "nlcfsqp.h"


/*$ Declarations $*/
static double nlcfsqp_sqpdeltadecrease = 0.50;
static double nlcfsqp_sqpdeltaincrease = 0.99;
static double nlcfsqp_maxtrustraddecay = 0.1;
static double nlcfsqp_deftrustradgrowth = 1.41;
static double nlcfsqp_maxtrustradgrowth = 10.0;
static double nlcfsqp_momentumgrowth = 2;
static double nlcfsqp_augmentationfactor = 0.0;
static double nlcfsqp_inittrustrad = 0.1;
static double nlcfsqp_stagnationepsf = 1.0E-12;
static ae_int_t nlcfsqp_fstagnationlimit = 20;
static ae_int_t nlcfsqp_defaultbfgsresetfreq = 999999;
static double nlcfsqp_sufficientdecreasesigma = 0.1;
static ae_int_t nlcfsqp_xbfgsmemlen = 8;
static double nlcfsqp_xbfgsmaxhess = 1.0E6;
static void nlcfsqp_initqpsubsolver(const minfsqpstate* sstate,
     minfsqpsubsolver* subsolver,
     ae_state *_state);
static ae_bool nlcfsqp_qpsubproblemsolvesqp(minfsqpstate* state,
     minfsqpsubsolver* subsolver,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagbcmult,
     /* Real    */ ae_vector* lagxcmult,
     ae_bool dotrace,
     ae_bool* isinfeasible,
     ae_int_t* terminationtype,
     double* predictedchangemodel,
     double* predictedchangepenalty,
     double* d2trustradratio,
     ae_state *_state);
static ae_bool nlcfsqp_qpsubproblemsolveelastic(minfsqpstate* state,
     minfsqpsubsolver* subsolver,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagbcmult,
     /* Real    */ ae_vector* lagxcmult,
     ae_bool dotrace,
     ae_bool* isinfeasible,
     ae_int_t* terminationtype,
     double* predictedchangemodel,
     double* predictedchangepenalty,
     double* d2trustradratio,
     ae_state *_state);
static void nlcfsqp_sqpsendx(minfsqpstate* state,
     /* Real    */ const ae_vector* xs,
     ae_state *_state);
static ae_bool nlcfsqp_sqpretrievefij(const minfsqpstate* state,
     varsfuncjac* vfj,
     ae_state *_state);
static void nlcfsqp_lagrangianfg(minfsqpstate* state,
     /* Real    */ const ae_vector* x,
     double trustrad,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* j,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     ae_bool uselagrangeterms,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state);
static void nlcfsqp_targetandconstraints(minfsqpstate* state,
     const varsfuncjac* vfj,
     double* tgt,
     double* cv,
     ae_state *_state);
static double nlcfsqp_rawlagrangian(minfsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     ae_state *_state);
static void nlcfsqp_meritfunctionandrawlagrangian(minfsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     double* meritf,
     double* rawlag,
     ae_state *_state);
static double nlcfsqp_gettrustregionk(/* Real    */ const ae_vector* xcur,
     ae_int_t k,
     double trustrad,
     ae_state *_state);
static ae_bool nlcfsqp_isacceptable(const minfsqpstate* state,
     double tgt0,
     double h0,
     double tgt1,
     double h1,
     double predictedchangemodel,
     double predictedchangepenalty,
     ae_state *_state);


/*$ Body $*/


void minfsqpinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     const nlpstoppingcriteria* criteria,
     ae_bool usedensebfgs,
     minfsqpstate* state,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;
    ae_int_t nslack;


    nslack = n+2*(nec+nlec)+(nic+nlic);
    state->n = n;
    state->nec = nec;
    state->nic = nic;
    state->nlec = nlec;
    state->nlic = nlic;
    
    /*
     * Prepare RCOMM state
     */
    ae_vector_set_length(&state->rstate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstate.ba, 13+1, _state);
    ae_vector_set_length(&state->rstate.ra, 17+1, _state);
    state->rstate.stage = -1;
    state->needfij = ae_false;
    state->xupdated = ae_false;
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->fi, 1+nlec+nlic, _state);
    ae_matrix_set_length(&state->j, 1+nlec+nlic, n, _state);
    
    /*
     * Allocate memory.
     */
    rallocv(n, &state->x0, _state);
    rallocv(n, &state->xprev, _state);
    rvectorsetlengthatleast(&state->s, n, _state);
    rvectorsetlengthatleast(&state->fscales, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&state->tracegamma, 1+nlec+nlic, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    rvectorsetlengthatleast(&state->scaledbndl, n, _state);
    rvectorsetlengthatleast(&state->scaledbndu, n, _state);
    rmatrixsetlengthatleast(&state->scaledcleic, nec+nic, n+1, _state);
    ivectorsetlengthatleast(&state->lcsrcidx, nec+nic, _state);
    rvectorsetlengthatleast(&state->dtrial, nslack, _state);
    rvectorsetlengthatleast(&state->d0, nslack, _state);
    rvectorsetlengthatleast(&state->d1, nslack, _state);
    rvectorsetlengthatleast(&state->dmu, nslack, _state);
    rvectorsetlengthatleast(&state->lagbcmult, n, _state);
    rvectorsetlengthatleast(&state->dummylagbcmult, n, _state);
    rvectorsetlengthatleast(&state->lagxcmult, nec+nic+nlec+nlic, _state);
    rvectorsetlengthatleast(&state->dummylagxcmult, nec+nic+nlec+nlic, _state);
    
    /*
     * Prepare scaled problem
     */
    for(i=0; i<=n-1; i++)
    {
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state->scaledbndl.ptr.p_double[i] = bndl->ptr.p_double[i]/s->ptr.p_double[i];
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state->scaledbndu.ptr.p_double[i] = bndu->ptr.p_double[i]/s->ptr.p_double[i];
        }
        if( state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i] )
        {
            ae_assert(ae_fp_less_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]), "SQP: integrity check failed, box constraints are inconsistent", _state);
        }
        state->x0.ptr.p_double[i] = x0->ptr.p_double[i]/s->ptr.p_double[i];
        state->s.ptr.p_double[i] = s->ptr.p_double[i];
    }
    for(i=0; i<=nec+nic-1; i++)
    {
        
        /*
         * Permutation
         */
        state->lcsrcidx.ptr.p_int[i] = lcsrcidx->ptr.p_int[i];
        
        /*
         * Scale and normalize linear constraints
         */
        vv = 0.0;
        for(j=0; j<=n-1; j++)
        {
            v = cleic->ptr.pp_double[i][j]*s->ptr.p_double[j];
            state->scaledcleic.ptr.pp_double[i][j] = v;
            vv = vv+v*v;
        }
        vv = ae_sqrt(vv, _state);
        state->scaledcleic.ptr.pp_double[i][n] = cleic->ptr.pp_double[i][n];
        if( ae_fp_greater(vv,(double)(0)) )
        {
            for(j=0; j<=n; j++)
            {
                state->scaledcleic.ptr.pp_double[i][j] = state->scaledcleic.ptr.pp_double[i][j]/vv;
            }
        }
    }
    
    /*
     * Initial enforcement of box constraints
     */
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state->x0.ptr.p_double[i] = ae_maxreal(state->x0.ptr.p_double[i], state->scaledbndl.ptr.p_double[i], _state);
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state->x0.ptr.p_double[i] = ae_minreal(state->x0.ptr.p_double[i], state->scaledbndu.ptr.p_double[i], _state);
        }
    }
    
    /*
     * Stopping criteria and settings
     */
    critcopy(criteria, &state->criteria, _state);
    state->bfgsresetfreq = nlcfsqp_defaultbfgsresetfreq;
    state->usedensebfgs = usedensebfgs;
    
    /*
     * Report fields
     */
    state->repterminationtype = 0;
    state->repbcerr = (double)(0);
    state->repbcidx = -1;
    state->replcerr = (double)(0);
    state->replcidx = -1;
    state->repnlcerr = (double)(0);
    state->repnlcidx = -1;
    state->repiterationscount = 0;
    
    /*
     * Integrity checks
     */
    ae_assert(ae_fp_less(nlcfsqp_sqpdeltadecrease,nlcfsqp_sqpdeltaincrease), "MinSQP: integrity check failed", _state);
}


/*************************************************************************
This function performs actual processing for  SQP  algorithm.  It  expects
that caller redirects its reverse communication  requests NeedFiJ/XUpdated
to external user who will provide analytic derivative (or  handle  reports
about progress).

In case external user does not have analytic derivative, it is responsibility
of caller to intercept NeedFiJ request and  replace  it  with  appropriate
numerical differentiation scheme.

Results are stored:
* point - in State.StepKX

IMPORTANT: this function works with scaled problem formulation; it is
           responsibility of the caller to unscale request and scale
           Jacobian.
           
NOTE: SMonitor is expected to be correctly initialized smoothness monitor.

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
ae_bool minfsqpiteration(minfsqpstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;
    double mx;
    double deltamax;
    double multiplyby;
    double setscaleto;
    double prevtrustrad;
    ae_bool trustradstagnated;
    double relativetargetdecrease;
    ae_bool dotrace;
    ae_bool doprobingalways;
    ae_bool doprobingonfailure;
    ae_bool dodetailedtrace;
    ae_bool meritfstagnated;
    double tgt0;
    double h0;
    double tgt1;
    double h1;
    double tol;
    double stepklagval;
    double stepknlagval;
    double d2trustradratio;
    double predictedchangemodel;
    double predictedchangepenalty;
    ae_bool infinitiesdetected;
    ae_bool failedtorecoverfrominfinities;
    ae_bool isinfeasible;
    ae_bool feasibilityrestoration;
    ae_bool firstrestorationiteration;
    ae_bool stepfailed;
    ae_bool restartiteration;
    ae_bool leavefeasibilityrestoration;
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
        nec = state->rstate.ia.ptr.p_int[1];
        nic = state->rstate.ia.ptr.p_int[2];
        nlec = state->rstate.ia.ptr.p_int[3];
        nlic = state->rstate.ia.ptr.p_int[4];
        i = state->rstate.ia.ptr.p_int[5];
        j = state->rstate.ia.ptr.p_int[6];
        trustradstagnated = state->rstate.ba.ptr.p_bool[0];
        dotrace = state->rstate.ba.ptr.p_bool[1];
        doprobingalways = state->rstate.ba.ptr.p_bool[2];
        doprobingonfailure = state->rstate.ba.ptr.p_bool[3];
        dodetailedtrace = state->rstate.ba.ptr.p_bool[4];
        meritfstagnated = state->rstate.ba.ptr.p_bool[5];
        infinitiesdetected = state->rstate.ba.ptr.p_bool[6];
        failedtorecoverfrominfinities = state->rstate.ba.ptr.p_bool[7];
        isinfeasible = state->rstate.ba.ptr.p_bool[8];
        feasibilityrestoration = state->rstate.ba.ptr.p_bool[9];
        firstrestorationiteration = state->rstate.ba.ptr.p_bool[10];
        stepfailed = state->rstate.ba.ptr.p_bool[11];
        restartiteration = state->rstate.ba.ptr.p_bool[12];
        leavefeasibilityrestoration = state->rstate.ba.ptr.p_bool[13];
        v = state->rstate.ra.ptr.p_double[0];
        vv = state->rstate.ra.ptr.p_double[1];
        mx = state->rstate.ra.ptr.p_double[2];
        deltamax = state->rstate.ra.ptr.p_double[3];
        multiplyby = state->rstate.ra.ptr.p_double[4];
        setscaleto = state->rstate.ra.ptr.p_double[5];
        prevtrustrad = state->rstate.ra.ptr.p_double[6];
        relativetargetdecrease = state->rstate.ra.ptr.p_double[7];
        tgt0 = state->rstate.ra.ptr.p_double[8];
        h0 = state->rstate.ra.ptr.p_double[9];
        tgt1 = state->rstate.ra.ptr.p_double[10];
        h1 = state->rstate.ra.ptr.p_double[11];
        tol = state->rstate.ra.ptr.p_double[12];
        stepklagval = state->rstate.ra.ptr.p_double[13];
        stepknlagval = state->rstate.ra.ptr.p_double[14];
        d2trustradratio = state->rstate.ra.ptr.p_double[15];
        predictedchangemodel = state->rstate.ra.ptr.p_double[16];
        predictedchangepenalty = state->rstate.ra.ptr.p_double[17];
    }
    else
    {
        n = 359;
        nec = -58;
        nic = -919;
        nlec = -909;
        nlic = 81;
        i = 255;
        j = 74;
        trustradstagnated = ae_false;
        dotrace = ae_true;
        doprobingalways = ae_true;
        doprobingonfailure = ae_false;
        dodetailedtrace = ae_true;
        meritfstagnated = ae_false;
        infinitiesdetected = ae_true;
        failedtorecoverfrominfinities = ae_true;
        isinfeasible = ae_false;
        feasibilityrestoration = ae_false;
        firstrestorationiteration = ae_false;
        stepfailed = ae_false;
        restartiteration = ae_false;
        leavefeasibilityrestoration = ae_true;
        v = -536.0;
        vv = 487.0;
        mx = -115.0;
        deltamax = 886.0;
        multiplyby = 346.0;
        setscaleto = -722.0;
        prevtrustrad = -413.0;
        relativetargetdecrease = -461.0;
        tgt0 = 927.0;
        h0 = 201.0;
        tgt1 = 922.0;
        h1 = -154.0;
        tol = 306.0;
        stepklagval = -1011.0;
        stepknlagval = 951.0;
        d2trustradratio = -463.0;
        predictedchangemodel = 88.0;
        predictedchangepenalty = -861.0;
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
    
    /*
     * Routine body
     */
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    dotrace = ae_is_trace_enabled("SQP");
    dodetailedtrace = dotrace&&ae_is_trace_enabled("SQP.DETAILED");
    doprobingalways = dotrace&&ae_is_trace_enabled("SQP.PROBING");
    doprobingonfailure = dotrace&&ae_is_trace_enabled("SQP.PROBINGONFAILURE");
    
    /*
     * Prepare rcomm interface
     */
    state->needfij = ae_false;
    state->xupdated = ae_false;
    
    /*
     * Prepare debug timers
     */
    stimerinit(&state->timertotal, _state);
    stimerinit(&state->timerqp, _state);
    stimerinit(&state->timercallback, _state);
    stimerstart(&state->timertotal, _state);
    
    /*
     * Initialize algorithm data:
     * * Lagrangian and "Big C" estimates
     * * trust region
     * * initial function scales (vector of 1's)
     * * current approximation of the Hessian matrix H (unit matrix)
     * * initial linearized constraints
     * * initial violation of linear/nonlinear constraints
     */
    state->fstagnationcnt = 0;
    state->trustradstagnationcnt = 0;
    state->trustrad = nlcfsqp_inittrustrad;
    state->trustradgrowth = nlcfsqp_deftrustradgrowth;
    for(i=0; i<=nlec+nlic; i++)
    {
        state->fscales.ptr.p_double[i] = 1.0;
        state->tracegamma.ptr.p_double[i] = 0.0;
    }
    
    /*
     * Evaluate function vector and Jacobian at X0, send first location report.
     * Compute initial violation of constraints.
     */
    vfjallocdense(n, 1+nlec+nlic, &state->stepk, _state);
    vfjallocdense(n, 1+nlec+nlic, &state->cand, _state);
    vfjallocdense(n, 1+nlec+nlic, &state->corr, _state);
    vfjallocdense(n, 1+nlec+nlic, &state->probe, _state);
    rcopyv(n, &state->x0, &state->stepk.x, _state);
    nlcfsqp_sqpsendx(state, &state->stepk.x, _state);
    state->needfij = ae_true;
    if( dotrace )
    {
        stimerstart(&state->timercallback, _state);
    }
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    if( dotrace )
    {
        stimerstop(&state->timercallback, _state);
    }
    state->needfij = ae_false;
    if( !nlcfsqp_sqpretrievefij(state, &state->stepk, _state) )
    {
        
        /*
         * Failed to retrieve function/Jacobian, infinities detected!
         */
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    nlcfsqp_sqpsendx(state, &state->stepk.x, _state);
    state->f = state->stepk.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, &state->stepk.x, n, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlcviolation(&state->stepk.fi, &state->fscales, nlec, nlic, &state->repnlcerr, &state->repnlcidx, _state);
    nlcfsqp_targetandconstraints(state, &state->stepk, &tgt0, &h0, _state);
    nlpfinit((double)10*(h0+(double)1), &state->filter, _state);
    
    /*
     * Trace output (if needed)
     */
    if( dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  FILTER SQP SOLVER STARTED                                                                     //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
    }
    
    /*
     * Perform outer (NLC) iterations
     */
    feasibilityrestoration = ae_false;
    firstrestorationiteration = ae_false;
    infinitiesdetected = ae_false;
    failedtorecoverfrominfinities = ae_false;
    relativetargetdecrease = 1.0E50;
    if( state->usedensebfgs )
    {
        hessianinitbfgs(&state->hess, n, state->bfgsresetfreq, 0.1*nlcfsqp_sqpdeltadecrease*ae_maxreal(critgetepsx(&state->criteria, _state), ae_sqrt(ae_machineepsilon, _state), _state), _state);
    }
    else
    {
        hessianinitlowrank(&state->hess, n, ae_minint(n, nlcfsqp_xbfgsmemlen, _state), 0.1*nlcfsqp_sqpdeltadecrease*ae_maxreal(critgetepsx(&state->criteria, _state), ae_sqrt(ae_machineepsilon, _state), _state), nlcfsqp_xbfgsmaxhess, _state);
    }
    nlcfsqp_initqpsubsolver(state, &state->subsolver, _state);
lbl_5:
    if( ae_false )
    {
        goto lbl_6;
    }
    
    /*
     * Before beginning new outer iteration:
     * * check stopping criteria
     * * renormalize target function and/or constraints, if some of them have too large magnitudes
     * * save initial point for the outer iteration
     */
    if( ae_fp_less_eq(relativetargetdecrease,critgetepsf(&state->criteria, _state)) )
    {
        state->repterminationtype = 1;
        if( dotrace )
        {
            ae_trace("> stopping condition met: relative change in target is less than %0.3e\n",
                (double)(critgetepsf(&state->criteria, _state)));
        }
        goto lbl_6;
    }
    if( ae_fp_less_eq(state->trustrad,critgetepsx(&state->criteria, _state)) )
    {
        state->repterminationtype = 2;
        if( dotrace )
        {
            ae_trace("> stopping condition met: trust radius is smaller than %0.3e\n",
                (double)(critgetepsx(&state->criteria, _state)));
        }
        goto lbl_6;
    }
    v = ae_sqrt(ae_machineepsilon, _state);
    if( ae_fp_less_eq(state->trustrad,v) )
    {
        state->repterminationtype = 7;
        if( dotrace )
        {
            ae_trace("> stopping condition met: trust radius is smaller than the absolute minimum %0.3e\n",
                (double)(v));
        }
        goto lbl_6;
    }
    if( critgetmaxits(&state->criteria, _state)>0&&state->repiterationscount>=critgetmaxits(&state->criteria, _state) )
    {
        state->repterminationtype = 5;
        if( dotrace )
        {
            ae_trace("> stopping condition met: %0d iterations performed\n",
                (int)(state->repiterationscount));
        }
        goto lbl_6;
    }
    if( state->fstagnationcnt>=nlcfsqp_fstagnationlimit )
    {
        state->repterminationtype = 7;
        if( dotrace )
        {
            ae_trace("> stopping criteria are too stringent: F stagnated for %0d its, stopping\n",
                (int)(state->fstagnationcnt));
        }
        goto lbl_6;
    }
    
    /*
     * Trace output (if needed)
     */
    if( dotrace )
    {
        ae_trace("\n=== OUTER ITERATION %5d STARTED ==================================================================\n",
            (int)(state->repiterationscount));
        if( dodetailedtrace )
        {
            ae_trace("> printing raw data (prior to applying variable and function scales)\n");
            ae_trace("X (raw)       = ");
            tracevectorunscaledunshiftedautoprec(&state->stepk.x, n, &state->s, ae_true, &state->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("> printing scaled data (after applying variable and function scales)\n");
            ae_trace("X (scaled)    = ");
            tracevectorautoprec(&state->stepk.x, 0, n, _state);
            ae_trace("\n");
            ae_trace("FScales       = ");
            tracevectorautoprec(&state->fscales, 0, 1+nlec+nlic, _state);
            ae_trace("\n");
            ae_trace("GammaScl      = ");
            tracevectorautoprec(&state->tracegamma, 0, 1+nlec+nlic, _state);
            ae_trace("\n");
            ae_trace("Fi (scaled)   = ");
            tracevectorautoprec(&state->stepk.fi, 0, 1+nlec+nlic, _state);
            ae_trace("\n");
            ae_trace("|Ji| (scaled) = ");
            tracerownrm1autoprec(&state->stepk.jac, 0, 1+nlec+nlic, 0, n, _state);
            ae_trace("\n");
        }
        mx = (double)(0);
        for(i=1; i<=nlec; i++)
        {
            mx = ae_maxreal(mx, ae_fabs(state->stepk.fi.ptr.p_double[i], _state), _state);
        }
        for(i=nlec+1; i<=nlec+nlic; i++)
        {
            mx = ae_maxreal(mx, state->stepk.fi.ptr.p_double[i], _state);
        }
        ae_trace("trustRad      = %0.3e\n",
            (double)(state->trustrad));
        ae_trace("lin.violation = %0.3e    (scaled violation of linear constraints)\n",
            (double)(state->replcerr));
        ae_trace("nlc.violation = %0.3e    (scaled violation of nonlinear constraints)\n",
            (double)(mx));
        ae_trace("gamma0        = %0.3e    (Hessian 2-norm estimate for target)\n",
            (double)(state->tracegamma.ptr.p_double[0]));
        j = 0;
        for(i=0; i<=nlec+nlic; i++)
        {
            if( ae_fp_greater(state->tracegamma.ptr.p_double[i],state->tracegamma.ptr.p_double[j]) )
            {
                j = i;
            }
        }
        ae_trace("gammaMax      = %0.3e    (maximum over Hessian 2-norm estimates for target/constraints)\n",
            (double)(state->tracegamma.ptr.p_double[j]));
        ae_trace("arg(gammaMax) = %0d             (function index; 0 for target, >0 for nonlinear constraints)\n",
            (int)(j));
        ae_trace("|x|inf        = %0.3e\n",
            (double)(rmaxabsv(n, &state->stepk.x, _state)));
    }
    
    /*
     * Start of the SQP iteration
     */
    rcopyv(n, &state->stepk.x, &state->xprev, _state);
    rsetv(n, 0.0, &state->d0, _state);
    rsetv(n, 0.0, &state->d1, _state);
    if( dotrace )
    {
        if( feasibilityrestoration )
        {
            ae_trace("\n--- feasibility restoration step -------------------------------------------------------------------\n");
        }
        else
        {
            ae_trace("\n--- filter SQP step --------------------------------------------------------------------------------\n");
        }
    }
    
    /*
     * Default values of the flag variables
     */
    stepfailed = ae_false;
    restartiteration = ae_false;
    leavefeasibilityrestoration = ae_false;
    meritfstagnated = ae_false;
    relativetargetdecrease = 1.0E50;
    
    /*
     * Determine step direction
     */
    if( !feasibilityrestoration )
    {
        if( !nlcfsqp_qpsubproblemsolvesqp(state, &state->subsolver, &state->stepk.x, &state->stepk.fi, &state->stepk.jac, &state->hess, &state->d0, &state->lagbcmult, &state->lagxcmult, dotrace, &isinfeasible, &j, &predictedchangemodel, &predictedchangepenalty, &d2trustradratio, _state) )
        {
            if( dotrace )
            {
                ae_trace("> [WARNING] QP subproblem failed with TerminationType=%0d\n> decreasing trust radius\n",
                    (int)(j));
            }
            state->trustrad = 0.1*state->trustrad;
            inc(&state->repiterationscount, _state);
            goto lbl_5;
        }
        if( dotrace )
        {
            ae_trace("> the QP subproblem was solved with TerminationType=%0d, max|lagBoxMult|=%0.2e, max|lagNonBoxMult|=%0.2e\n",
                (int)(j),
                (double)(rmaxabsv(n, &state->lagbcmult, _state)),
                (double)(rmaxabsv(nec+nic+nlec+nlic, &state->lagxcmult, _state)));
        }
        if( isinfeasible )
        {
            if( dotrace )
            {
                ae_trace(">> the QP subproblem is infeasible, entering feasibility restoration phase\n>> restarting iteration\n\n");
            }
            feasibilityrestoration = ae_true;
            firstrestorationiteration = ae_true;
            inc(&state->repiterationscount, _state);
            goto lbl_5;
        }
    }
    else
    {
        if( !nlcfsqp_qpsubproblemsolveelastic(state, &state->subsolver, &state->stepk.x, &state->stepk.fi, &state->stepk.jac, &state->hess, &state->d0, &state->lagbcmult, &state->lagxcmult, dotrace, &isinfeasible, &j, &predictedchangemodel, &predictedchangepenalty, &d2trustradratio, _state) )
        {
            if( dotrace )
            {
                ae_trace("> [WARNING] QP subproblem failed with TerminationType=%0d\n> decreasing trust radius\n",
                    (int)(j));
            }
            state->trustrad = 0.1*state->trustrad;
            inc(&state->repiterationscount, _state);
            goto lbl_5;
        }
        if( dotrace )
        {
            ae_trace("> elastic QP subproblem was solved with TerminationType=%0d, max|lagBoxMult|=%0.2e, max|lagNonBoxMult|=%0.2e\n\n",
                (int)(j),
                (double)(rmaxabsv(n, &state->lagbcmult, _state)),
                (double)(rmaxabsv(nec+nic+nlec+nlic, &state->lagxcmult, _state)));
        }
    }
    rcopyv(n, &state->d0, &state->dtrial, _state);
    
    /*
     * Perform merit function line search.
     *
     * First, we try unit step. If it does not decrease merit function,
     * a second-order correction is tried (helps to combat Maratos effect).
     */
    nlcfsqp_targetandconstraints(state, &state->stepk, &tgt0, &h0, _state);
    rcopyv(n, &state->stepk.x, &state->cand.x, _state);
    raddv(n, 1.0, &state->dtrial, &state->cand.x, _state);
    nlcfsqp_sqpsendx(state, &state->cand.x, _state);
    state->needfij = ae_true;
    if( dotrace )
    {
        stimerstart(&state->timercallback, _state);
    }
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    if( dotrace )
    {
        stimerstop(&state->timercallback, _state);
    }
    state->needfij = ae_false;
    if( !nlcfsqp_sqpretrievefij(state, &state->cand, _state) )
    {
        
        /*
         * Failed to retrieve func/Jac, infinities detected
         */
        if( dotrace )
        {
            ae_trace("[WARNING] infinities in target/constraints are detected, forcing trust radius decrease and restarting iteration\n");
        }
        state->trustrad = state->trustrad*nlcfsqp_maxtrustraddecay;
        trustradresetmomentum(&state->trustradgrowth, nlcfsqp_deftrustradgrowth, _state);
        infinitiesdetected = ae_true;
        failedtorecoverfrominfinities = ae_true;
        inc(&state->repiterationscount, _state);
        goto lbl_5;
    }
    nlcfsqp_targetandconstraints(state, &state->cand, &tgt1, &h1, _state);
    if( dotrace )
    {
        ae_trace("> proposed step with relative len %0.6f (compared to trust radius)\n",
            (double)(d2trustradratio));
        ae_trace(">> change in the target function:       predicted=%0.2e, actual=%0.2e, ratio=%5.2f\n",
            (double)(predictedchangemodel),
            (double)(tgt1-tgt0),
            (double)((tgt1-tgt0)/predictedchangemodel));
        ae_trace(">> change in the constraints violation: predicted=%0.2e, actual=%0.2e, ratio=%5.2f\n",
            (double)(predictedchangepenalty),
            (double)(h1-h0),
            (double)((h1-h0)/predictedchangepenalty));
        ae_trace(">> target and constraints are:\n");
        ae_trace("targetF:        %17.9e -> %17.9e (delta=%11.3e)\n",
            (double)(state->fscales.ptr.p_double[0]*state->stepk.fi.ptr.p_double[0]),
            (double)(state->fscales.ptr.p_double[0]*state->cand.fi.ptr.p_double[0]),
            (double)(state->fscales.ptr.p_double[0]*(state->cand.fi.ptr.p_double[0]-state->stepk.fi.ptr.p_double[0])));
        ae_trace("sum-of-constr:  %17.9e -> %17.9e (delta=%11.3e)\n",
            (double)(h0),
            (double)(h1),
            (double)(h1-h0));
        ae_trace("\n");
    }
    enforceboundaryconstraints(&state->cand.x, &state->scaledbndl, &state->hasbndl, &state->scaledbndu, &state->hasbndu, n, 0, _state);
    if( userterminationneeded )
    {
        
        /*
         * User requested termination, break before we move to new point
         */
        state->repterminationtype = icase2(failedtorecoverfrominfinities, -8, 8, _state);
        if( dotrace )
        {
            ae_trace("> user requested termination\n");
        }
        goto lbl_6;
    }
    
    /*
     * Update Hessian
     */
    if( !feasibilityrestoration )
    {
        rallocv(n, &state->tmplaggrad, _state);
        rallocv(n, &state->tmpcandlaggrad, _state);
        nlcfsqp_lagrangianfg(state, &state->stepk.x, state->trustrad, &state->stepk.fi, &state->stepk.jac, &state->lagbcmult, &state->lagxcmult, ae_true, &stepklagval, &state->tmplaggrad, _state);
        nlcfsqp_lagrangianfg(state, &state->cand.x, state->trustrad, &state->cand.fi, &state->cand.jac, &state->lagbcmult, &state->lagxcmult, ae_true, &stepknlagval, &state->tmpcandlaggrad, _state);
        hessianupdatev2(&state->hess, &state->stepk.x, &state->tmplaggrad, &state->cand.x, &state->tmpcandlaggrad, 2, ae_false, dotrace, 1, _state);
        if( dotrace )
        {
            hessiangetdiagonal(&state->hess, &state->tmphdiag, _state);
            ae_trace(">> printing information about Hessian:\n");
            v = state->tmphdiag.ptr.p_double[0];
            for(i=0; i<=n-1; i++)
            {
                v = ae_minreal(v, state->tmphdiag.ptr.p_double[i], _state);
            }
            ae_trace("mindiag(Bk) = %0.3e\n",
                (double)(v));
            v = state->tmphdiag.ptr.p_double[0];
            for(i=0; i<=n-1; i++)
            {
                v = ae_maxreal(v, state->tmphdiag.ptr.p_double[i], _state);
            }
            ae_trace("maxdiag(Bk) = %0.3e\n",
                (double)(v));
            ae_trace("\n");
        }
    }
    
    /*
     * Analyze current step and decide what to do: to continue iteration or to restart it.
     *
     * The latter option has two possible choices: to restart it with trust radius decreased
     * (StepFailed=True) or with the same trust radius (StepFailed=False, performed when exiting
     * from the feasibility restoration phase).
     *
     * Depending on the outcome, we set/clear FeasibilityRestoration, RestartIteration and StepFailed.
     * Then we do some other work like printing trace logs, and finally restart iteration if needed.
     */
    if( feasibilityrestoration )
    {
        
        /*
         * Feasibility restoration phase
         */
        if( (!firstrestorationiteration&&!isinfeasible)&&nlpfisacceptable(&state->filter, tgt0, h0, _state) )
        {
            
            /*
             * Exit restoration if initial point is feasible and acceptable, and we performed at least one restoration iteration
             */
            if( dotrace )
            {
                ae_trace("> the initial point is feasible and accepted by the filter, exiting feasibility restoration phase\n");
            }
            trustradresetmomentum(&state->trustradgrowth, nlcfsqp_deftrustradgrowth, _state);
            leavefeasibilityrestoration = ae_true;
            restartiteration = ae_true;
        }
        else
        {
            
            /*
             * Check sufficient decrease condition for modified penalty
             *
             * NOTE: it is essential to have strict ">" for both conditions below in order to be able
             *       to properly handle situations with zero predicted change
             */
            if( ae_fp_greater(predictedchangepenalty,(double)(0))||ae_fp_greater(h1-h0,nlcfsqp_sufficientdecreasesigma*predictedchangepenalty) )
            {
                if( dotrace )
                {
                    ae_trace("> the sufficient decrease condition does not hold, decreasing trust radius\n");
                }
                restartiteration = ae_true;
                stepfailed = ae_true;
            }
        }
        meritfstagnated = ae_fp_less_eq(ae_fabs(h1-h0, _state),nlcfsqp_stagnationepsf*ae_maxreal(h0, h1, _state));
        firstrestorationiteration = ae_false;
    }
    else
    {
        
        /*
         * SQP phase
         */
        if( !nlcfsqp_isacceptable(state, tgt0, h0, tgt1, h1, predictedchangemodel, predictedchangepenalty, _state) )
        {
            
            /*
             * The point is not acceptable, decrease trust radius and retry
             */
            if( dotrace )
            {
                ae_trace("> the candidate point is not accepted by the filter, decreasing trust radius\n");
            }
            restartiteration = ae_true;
            stepfailed = ae_true;
        }
        meritfstagnated = ae_fp_less_eq(ae_fabs(tgt1-tgt0, _state),nlcfsqp_stagnationepsf*ae_fabs(tgt0, _state))&&ae_fp_less_eq(ae_fabs(h1-h0, _state),nlcfsqp_stagnationepsf*ae_maxreal(h0, h1, _state));
    }
    if( !dotrace )
    {
        goto lbl_7;
    }
    
    /*
     * Perform agressive probing of the search direction - additional function evaluations
     * which help us to determine possible discontinuity and nonsmoothness of the problem
     */
    if( !(doprobingalways||(doprobingonfailure&&stepfailed)) )
    {
        goto lbl_9;
    }
    smoothnessmonitorstartlagrangianprobing(smonitor, &state->stepk.x, &state->dtrial, 1.0, state->repiterationscount, -1, _state);
lbl_11:
    if( !smoothnessmonitorprobelagrangian(smonitor, _state) )
    {
        goto lbl_12;
    }
    for(j=0; j<=n-1; j++)
    {
        state->probe.x.ptr.p_double[j] = smonitor->lagprobx.ptr.p_double[j];
        if( state->hasbndl.ptr.p_bool[j] )
        {
            state->probe.x.ptr.p_double[j] = ae_maxreal(state->probe.x.ptr.p_double[j], state->scaledbndl.ptr.p_double[j], _state);
        }
        if( state->hasbndu.ptr.p_bool[j] )
        {
            state->probe.x.ptr.p_double[j] = ae_minreal(state->probe.x.ptr.p_double[j], state->scaledbndu.ptr.p_double[j], _state);
        }
    }
    nlcfsqp_sqpsendx(state, &state->probe.x, _state);
    state->needfij = ae_true;
    if( dotrace )
    {
        stimerstart(&state->timercallback, _state);
    }
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    if( dotrace )
    {
        stimerstop(&state->timercallback, _state);
    }
    state->needfij = ae_false;
    if( !nlcfsqp_sqpretrievefij(state, &state->probe, _state) )
    {
        goto lbl_12;
    }
    rcopyallocv(1+nlec+nlic, &state->probe.fi, &smonitor->lagprobfi, _state);
    rcopyallocm(1+nlec+nlic, n, &state->probe.jac, &smonitor->lagprobj, _state);
    smonitor->lagprobrawlag = nlcfsqp_rawlagrangian(state, &state->probe.x, &state->probe.fi, &state->lagbcmult, &state->lagxcmult, _state);
    goto lbl_11;
lbl_12:
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |                 probing search direction                 |\n");
    ae_trace("*** |          suggested by first-order QP subproblem          |\n");
    ae_trace("*** |                                                          |\n");
    ae_trace("*** |          note: the target/Lagrangian  may  increase      |\n");
    ae_trace("*** |                along the search  direction  due  to      |\n");
    ae_trace("*** |                nonmonotonic steps of the filter SQP      |\n");
    ae_trace("*** |                However, they  MUST  be  smooth  and      |\n");
    ae_trace("*** |                continuous.                               |\n");
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |  Step  | Lagrangian (unaugmentd)|    Target  function    |\n");
    ae_trace("*** |along  D|     must be smooth     |     must be smooth     |\n");
    ae_trace("*** |        | function   |    slope  | function   |    slope  |\n");
    smoothnessmonitortracelagrangianprobingresults(smonitor, _state);
    ae_trace("\n");
lbl_9:
    
    /*
     * Output other information
     */
lbl_7:
    if( leavefeasibilityrestoration )
    {
        feasibilityrestoration = ae_false;
    }
    if( restartiteration )
    {
        if( stepfailed )
        {
            state->trustrad = state->trustrad*0.5;
            trustradresetmomentum(&state->trustradgrowth, nlcfsqp_deftrustradgrowth, _state);
        }
        if( dotrace )
        {
            ae_trace("> restarting iteration\n\n");
        }
        inc(&state->repiterationscount, _state);
        goto lbl_5;
    }
    if( dotrace )
    {
        ae_trace("> the step was successfully performed\n");
    }
    if( !feasibilityrestoration&&(ae_fp_greater_eq(predictedchangemodel,(double)(0))||ae_fp_greater(tgt1-tgt0,nlcfsqp_sufficientdecreasesigma*predictedchangemodel)) )
    {
        nlpfappend(&state->filter, tgt0, h0, _state);
        if( dotrace )
        {
            ae_trace(">> the initial point was added to the filter (%0d entries in the filter)\n",
                (int)(state->filter.filtersize));
        }
    }
    if( dotrace )
    {
        ae_trace("\n");
    }
    
    /*
     * Analyze merit F for stagnation
     */
    relativetargetdecrease = ae_maxreal(ae_fabs(tgt1-tgt0, _state)/rmaxabs3(tgt0, tgt1, (double)(1), _state), ae_fabs(h1-h0, _state)/rmaxabs3(h0, h1, (double)(1), _state), _state);
    
    /*
     * Move to new point
     * Report one more inner iteration
     * Update constraint violations
     */
    vfjcopy(&state->cand, &state->stepk, _state);
    failedtorecoverfrominfinities = ae_false;
    nlcfsqp_sqpsendx(state, &state->stepk.x, _state);
    state->f = state->stepk.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->xupdated = ae_false;
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, &state->stepk.x, n, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlcviolation(&state->stepk.fi, &state->fscales, nlec, nlic, &state->repnlcerr, &state->repnlcidx, _state);
    
    /*
     * Update trust region.
     *
     * NOTE: when trust region radius remains fixed for a long time it may mean that we 
     *       stagnated in eternal loop. In such cases we decrease it slightly in order
     *       to break possible loop. If such decrease was unnecessary, it may be easily
     *       fixed within few iterations.
     */
    trustradstagnated = ae_false;
    prevtrustrad = state->trustrad;
    deltamax = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        deltamax = ae_maxreal(deltamax, ae_fabs(state->xprev.ptr.p_double[i]-state->stepk.x.ptr.p_double[i], _state)/nlcfsqp_gettrustregionk(&state->xprev, i, state->trustrad, _state), _state);
    }
    if( ae_fp_less_eq(deltamax,nlcfsqp_sqpdeltadecrease) )
    {
        state->trustrad = state->trustrad*ae_maxreal(deltamax/nlcfsqp_sqpdeltadecrease, nlcfsqp_maxtrustraddecay, _state);
    }
    if( ae_fp_greater_eq(deltamax,nlcfsqp_sqpdeltaincrease) )
    {
        state->trustrad = state->trustrad*state->trustradgrowth;
        trustradincreasemomentum(&state->trustradgrowth, nlcfsqp_momentumgrowth, nlcfsqp_maxtrustradgrowth, _state);
    }
    else
    {
        trustradresetmomentum(&state->trustradgrowth, nlcfsqp_deftrustradgrowth, _state);
    }
    
    /*
     * Trace
     */
    if( dotrace )
    {
        ae_trace("\n--- iteration ends ---------------------------------------------------------------------------------\n");
        ae_trace("deltaMax    = %0.3f (ratio of step length to trust radius)\n",
            (double)(deltamax));
        ae_trace("newTrustRad = %0.3e",
            (double)(state->trustrad));
        if( ae_fp_greater(state->trustrad,prevtrustrad) )
        {
            ae_trace(", trust radius increased");
        }
        if( ae_fp_less(state->trustrad,prevtrustrad) )
        {
            ae_trace(", trust radius decreased");
        }
        ae_trace("\n");
    }
    
    /*
     * Advance outer iteration counter
     */
    inc(&state->repiterationscount, _state);
    state->fstagnationcnt = icase2(meritfstagnated, state->fstagnationcnt+1, 0, _state);
    goto lbl_5;
lbl_6:
    
    /*
     * Almost done
     */
    if( infinitiesdetected )
    {
        state->repterminationtype = icase2(failedtorecoverfrominfinities, -8, state->repterminationtype+800, _state);
    }
    if( dotrace )
    {
        stimerstop(&state->timertotal, _state);
        ae_trace("\n=== STOPPED ========================================================================================\n");
        if( infinitiesdetected&&failedtorecoverfrominfinities )
        {
            ae_trace("> infinities were detected, but we failed to recover by decreasing trust radius; declaring failure with the code -8\n");
        }
        ae_trace("raw target:     %20.12e\n",
            (double)(state->stepk.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0]));
        ae_trace("total time:     %10.1f ms\n",
            (double)(stimergetms(&state->timertotal, _state)));
        ae_trace("* incl cbck:    %10.1f ms\n",
            (double)(stimergetms(&state->timercallback, _state)));
        ae_trace("* incl QP:      %10.1f ms\n",
            (double)(stimergetms(&state->timerqp, _state)));
    }
    smoothnessmonitortracestatus(smonitor, dotrace, _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = nec;
    state->rstate.ia.ptr.p_int[2] = nic;
    state->rstate.ia.ptr.p_int[3] = nlec;
    state->rstate.ia.ptr.p_int[4] = nlic;
    state->rstate.ia.ptr.p_int[5] = i;
    state->rstate.ia.ptr.p_int[6] = j;
    state->rstate.ba.ptr.p_bool[0] = trustradstagnated;
    state->rstate.ba.ptr.p_bool[1] = dotrace;
    state->rstate.ba.ptr.p_bool[2] = doprobingalways;
    state->rstate.ba.ptr.p_bool[3] = doprobingonfailure;
    state->rstate.ba.ptr.p_bool[4] = dodetailedtrace;
    state->rstate.ba.ptr.p_bool[5] = meritfstagnated;
    state->rstate.ba.ptr.p_bool[6] = infinitiesdetected;
    state->rstate.ba.ptr.p_bool[7] = failedtorecoverfrominfinities;
    state->rstate.ba.ptr.p_bool[8] = isinfeasible;
    state->rstate.ba.ptr.p_bool[9] = feasibilityrestoration;
    state->rstate.ba.ptr.p_bool[10] = firstrestorationiteration;
    state->rstate.ba.ptr.p_bool[11] = stepfailed;
    state->rstate.ba.ptr.p_bool[12] = restartiteration;
    state->rstate.ba.ptr.p_bool[13] = leavefeasibilityrestoration;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vv;
    state->rstate.ra.ptr.p_double[2] = mx;
    state->rstate.ra.ptr.p_double[3] = deltamax;
    state->rstate.ra.ptr.p_double[4] = multiplyby;
    state->rstate.ra.ptr.p_double[5] = setscaleto;
    state->rstate.ra.ptr.p_double[6] = prevtrustrad;
    state->rstate.ra.ptr.p_double[7] = relativetargetdecrease;
    state->rstate.ra.ptr.p_double[8] = tgt0;
    state->rstate.ra.ptr.p_double[9] = h0;
    state->rstate.ra.ptr.p_double[10] = tgt1;
    state->rstate.ra.ptr.p_double[11] = h1;
    state->rstate.ra.ptr.p_double[12] = tol;
    state->rstate.ra.ptr.p_double[13] = stepklagval;
    state->rstate.ra.ptr.p_double[14] = stepknlagval;
    state->rstate.ra.ptr.p_double[15] = d2trustradratio;
    state->rstate.ra.ptr.p_double[16] = predictedchangemodel;
    state->rstate.ra.ptr.p_double[17] = predictedchangepenalty;
    return result;
}


/*************************************************************************
This function initializes SQP subproblem.
Should be called once in the beginning of the optimization.

INPUT PARAMETERS:
    SState          -   solver state
    Subsolver       -   SQP subproblem to initialize
                        
                        
RETURN VALUE:
    True on success
    False on failure of the QP solver (unexpected... but possible due to numerical errors)
                        

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static void nlcfsqp_initqpsubsolver(const minfsqpstate* sstate,
     minfsqpsubsolver* subsolver,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nslack;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t lccnt;
    ae_int_t nnz;
    ae_int_t offs;
    ae_int_t i;
    ae_int_t j;


    n = sstate->n;
    nec = sstate->nec;
    nic = sstate->nic;
    nlec = sstate->nlec;
    nlic = sstate->nlic;
    nslack = n+2*(nec+nlec)+(nic+nlic);
    lccnt = nec+nic+nlec+nlic;
    
    /*
     * Allocate temporaries
     */
    rvectorsetlengthatleast(&subsolver->cural, lccnt, _state);
    rvectorsetlengthatleast(&subsolver->curau, lccnt, _state);
    rvectorsetlengthatleast(&subsolver->curbndl, nslack, _state);
    rvectorsetlengthatleast(&subsolver->curbndu, nslack, _state);
    rvectorsetlengthatleast(&subsolver->curb, nslack, _state);
    
    /*
     * Linear constraints do not change across subiterations, that's
     * why we allocate storage for them at the start of the program.
     *
     * A full set of "raw" constraints is stored; later we will filter
     * out inequality ones which are inactive anywhere in the current
     * trust region.
     *
     * NOTE: because sparserawlc object stores only linear constraint
     *       (linearizations of nonlinear ones are not stored) we
     *       allocate only minimum necessary space.
     */
    nnz = 0;
    for(i=0; i<=nec+nic-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( sstate->scaledcleic.ptr.pp_double[i][j]!=0.0 )
            {
                nnz = nnz+1;
            }
        }
    }
    ivectorsetlengthatleast(&subsolver->sparserawlc.ridx, nec+nic+1, _state);
    rvectorsetlengthatleast(&subsolver->sparserawlc.vals, nnz, _state);
    ivectorsetlengthatleast(&subsolver->sparserawlc.idx, nnz, _state);
    ivectorsetlengthatleast(&subsolver->sparserawlc.didx, nec+nic, _state);
    ivectorsetlengthatleast(&subsolver->sparserawlc.uidx, nec+nic, _state);
    offs = 0;
    subsolver->sparserawlc.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=nec+nic-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( sstate->scaledcleic.ptr.pp_double[i][j]!=0.0 )
            {
                
                /*
                 * Primary part of the matrix
                 */
                subsolver->sparserawlc.vals.ptr.p_double[offs] = sstate->scaledcleic.ptr.pp_double[i][j];
                subsolver->sparserawlc.idx.ptr.p_int[offs] = j;
                offs = offs+1;
            }
        }
        subsolver->sparserawlc.ridx.ptr.p_int[i+1] = offs;
    }
    subsolver->sparserawlc.matrixtype = 1;
    subsolver->sparserawlc.ninitialized = subsolver->sparserawlc.ridx.ptr.p_int[nec+nic];
    subsolver->sparserawlc.m = nec+nic;
    subsolver->sparserawlc.n = n;
    sparseinitduidx(&subsolver->sparserawlc, _state);
}


/*************************************************************************
This function solves QP subproblem given by initial point X, function vector Fi
and Jacobian Jac, and returns:
* search direction D[]
* estimates of Lagrangian multipliers
* predicted change in the quadratic model of the target, and in the L1-penalty
  added to the model

The QP subproblem linear and quadratic terms are multiplied by the scalar
multipliers AlphaG and AlphaH, which can be:
* (1,1) when we want to compute a step direction
* (0,0) when we want to compute best possible improvement in the linearized
   constraints over the trust region
* (1,0) when we want to compute Lagrange multipliers using linearized model
  of the target instead of the quadratic one

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcfsqp_qpsubproblemsolvesqp(minfsqpstate* state,
     minfsqpsubsolver* subsolver,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagbcmult,
     /* Real    */ ae_vector* lagxcmult,
     ae_bool dotrace,
     ae_bool* isinfeasible,
     ae_int_t* terminationtype,
     double* predictedchangemodel,
     double* predictedchangepenalty,
     double* d2trustradratio,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double vv;
    double vright;
    double vmax;
    ae_int_t lccnt;
    ae_int_t offs;
    ae_int_t nnz;
    ae_int_t j0;
    ae_int_t j1;
    double rescaleby;
    ae_bool result;

    *isinfeasible = ae_false;
    *terminationtype = 0;
    *predictedchangemodel = 0.0;
    *predictedchangepenalty = 0.0;
    *d2trustradratio = 0.0;

    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    lccnt = nec+nic+nlec+nlic;
    
    /*
     * Prepare temporary structures
     */
    rvectorgrowto(&subsolver->cural, lccnt, _state);
    rvectorgrowto(&subsolver->curau, lccnt, _state);
    rallocv(lccnt, &subsolver->rescalelag, _state);
    
    /*
     * Prepare default solution: all zeros
     */
    result = ae_true;
    *terminationtype = 0;
    *predictedchangemodel = (double)(0);
    *predictedchangepenalty = (double)(0);
    *d2trustradratio = 0.0;
    *isinfeasible = ae_false;
    rsetv(n, 0.0, d, _state);
    rsetv(n, 0.0, lagbcmult, _state);
    rsetv(lccnt, 0.0, lagxcmult, _state);
    bsetallocv(nec+nic+nlec+nlic, ae_false, &subsolver->isinactive, _state);
    
    /*
     * Linear term B
     */
    rcopyrv(n, jac, 0, &subsolver->curb, _state);
    
    /*
     * Trust radius constraints for primary variables
     */
    bsetallocv(n, ae_false, &subsolver->retainnegativebclm, _state);
    bsetallocv(n, ae_false, &subsolver->retainpositivebclm, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->curbndl.ptr.p_double[i] = -nlcfsqp_gettrustregionk(x, i, state->trustrad, _state);
        subsolver->curbndu.ptr.p_double[i] = nlcfsqp_gettrustregionk(x, i, state->trustrad, _state);
        if( state->hasbndl.ptr.p_bool[i]&&ae_fp_greater(state->scaledbndl.ptr.p_double[i]-x->ptr.p_double[i],subsolver->curbndl.ptr.p_double[i]) )
        {
            subsolver->curbndl.ptr.p_double[i] = state->scaledbndl.ptr.p_double[i]-x->ptr.p_double[i];
            subsolver->retainnegativebclm.ptr.p_bool[i] = ae_true;
        }
        if( state->hasbndu.ptr.p_bool[i]&&ae_fp_less(state->scaledbndu.ptr.p_double[i]-x->ptr.p_double[i],subsolver->curbndu.ptr.p_double[i]) )
        {
            subsolver->curbndu.ptr.p_double[i] = state->scaledbndu.ptr.p_double[i]-x->ptr.p_double[i];
            subsolver->retainpositivebclm.ptr.p_bool[i] = ae_true;
        }
    }
    
    /*
     * Prepare storage for "effective" constraining matrix
     */
    nnz = subsolver->sparserawlc.ridx.ptr.p_int[nec+nic];
    for(i=0; i<=nlec+nlic-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( jac->ptr.pp_double[1+i][j]!=0.0 )
            {
                nnz = nnz+1;
            }
        }
    }
    ivectorgrowto(&subsolver->sparseefflc.ridx, lccnt+1, _state);
    rvectorgrowto(&subsolver->sparseefflc.vals, nnz, _state);
    ivectorgrowto(&subsolver->sparseefflc.idx, nnz, _state);
    ivectorsetlengthatleast(&subsolver->sparseefflc.didx, lccnt, _state);
    ivectorsetlengthatleast(&subsolver->sparseefflc.uidx, lccnt, _state);
    subsolver->sparseefflc.ridx.ptr.p_int[0] = 0;
    subsolver->sparseefflc.m = 0;
    subsolver->sparseefflc.n = n;
    subsolver->sparseefflc.matrixtype = 1;
    
    /*
     * Append linear equality/inequality constraints
     *
     * Scan sparsified linear constraints stored in sparserawlc[], skip ones
     * which are inactive anywhere in the trust region.
     */
    rcopyallocv(n, x, &subsolver->tmp0, _state);
    for(i=0; i<=nec+nic-1; i++)
    {
        
        /*
         * Calculate:
         * * VRight - product of X[]  and AR[i] - Ith row of sparserawlc matrix.
         * * VMax   - maximum value of X*ARi computed over trust region
         */
        vright = (double)(0);
        vmax = (double)(0);
        j0 = subsolver->sparserawlc.ridx.ptr.p_int[i];
        j1 = subsolver->sparserawlc.ridx.ptr.p_int[i+1]-1;
        for(k=j0; k<=j1; k++)
        {
            j = subsolver->sparserawlc.idx.ptr.p_int[k];
            v = x->ptr.p_double[j];
            vv = subsolver->sparserawlc.vals.ptr.p_double[k];
            vright = vright+vv*v;
            if( vv>=(double)0 )
            {
                vmax = vmax+vv*(v+subsolver->curbndu.ptr.p_double[j]);
            }
            else
            {
                vmax = vmax+vv*(v+subsolver->curbndl.ptr.p_double[j]);
            }
        }
        
        /*
         * Linear constraints are passed 'as is', without additional rescaling
         */
        subsolver->rescalelag.ptr.p_double[i] = 1.0;
        
        /*
         * If constraint is an inequality one and guaranteed to be inactive
         * within trust region, it is skipped (the row itself is retained but
         * filled by zeros).
         */
        if( i>=nec&&vmax<=state->scaledcleic.ptr.pp_double[i][n] )
        {
            subsolver->sparseefflc.ridx.ptr.p_int[i+1] = subsolver->sparseefflc.ridx.ptr.p_int[i];
            subsolver->cural.ptr.p_double[i] = 0.0;
            subsolver->curau.ptr.p_double[i] = 0.0;
            subsolver->rescalelag.ptr.p_double[i] = 0.0;
            subsolver->isinactive.ptr.p_bool[i] = ae_true;
            continue;
        }
        
        /*
         * Copy constraint from sparserawlc[] to sparseefflc[]
         */
        offs = subsolver->sparseefflc.ridx.ptr.p_int[i];
        j0 = subsolver->sparserawlc.ridx.ptr.p_int[i];
        j1 = subsolver->sparserawlc.ridx.ptr.p_int[i+1]-1;
        for(k=j0; k<=j1; k++)
        {
            subsolver->sparseefflc.idx.ptr.p_int[offs] = subsolver->sparserawlc.idx.ptr.p_int[k];
            subsolver->sparseefflc.vals.ptr.p_double[offs] = subsolver->sparserawlc.vals.ptr.p_double[k];
            offs = offs+1;
        }
        subsolver->sparseefflc.ridx.ptr.p_int[i+1] = offs;
        
        /*
         * Set up bounds and slack part of D0.
         *
         * NOTE: bounds for equality and inequality constraints are
         *       handled differently
         */
        v = vright-state->scaledcleic.ptr.pp_double[i][n];
        if( i<nec )
        {
            subsolver->cural.ptr.p_double[i] = -v;
            subsolver->curau.ptr.p_double[i] = -v;
        }
        else
        {
            subsolver->cural.ptr.p_double[i] = _state->v_neginf;
            subsolver->curau.ptr.p_double[i] = -v;
        }
    }
    subsolver->sparseefflc.m = subsolver->sparseefflc.m+(nec+nic);
    
    /*
     * Append nonlinear equality/inequality constraints
     */
    for(i=0; i<=nlec+nlic-1; i++)
    {
        
        /*
         * Calculate:
         * * rescale coefficient used to normalize constraints
         * * VMax - maximum of constraint value over trust region
         */
        vmax = fi->ptr.p_double[1+i];
        vv = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            v = jac->ptr.pp_double[1+i][j];
            vv = vv+v*v;
            if( v>=(double)0 )
            {
                vmax = vmax+v*subsolver->curbndu.ptr.p_double[j];
            }
            else
            {
                vmax = vmax+v*subsolver->curbndl.ptr.p_double[j];
            }
        }
        rescaleby = (double)1/coalesce(ae_sqrt(vv, _state), (double)(1), _state);
        
        /*
         * Nonlinear constraints are passed with normalization
         */
        subsolver->rescalelag.ptr.p_double[nec+nic+i] = rescaleby;
        
        /*
         * If constraint is an inequality one and guaranteed to be inactive
         * within trust region, it is skipped (row itself is retained but
         * filled by zeros).
         */
        if( i>=nlec&&vmax<=0.0 )
        {
            subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i+1] = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i];
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = 0.0;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = 0.0;
            subsolver->rescalelag.ptr.p_double[nec+nic+i] = 0.0;
            subsolver->isinactive.ptr.p_bool[nec+nic+i] = ae_true;
            continue;
        }
        
        /*
         * Copy scaled row
         */
        offs = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i];
        for(j=0; j<=n-1; j++)
        {
            if( jac->ptr.pp_double[1+i][j]!=0.0 )
            {
                subsolver->sparseefflc.vals.ptr.p_double[offs] = rescaleby*jac->ptr.pp_double[1+i][j];
                subsolver->sparseefflc.idx.ptr.p_int[offs] = j;
                offs = offs+1;
            }
        }
        subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i+1] = offs;
        
        /*
         * Set bounds on linear equality/inequality constraints
         */
        v = rescaleby*fi->ptr.p_double[1+i];
        if( i<nlec )
        {
            
            /*
             * Equality constraint
             */
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
        }
        else
        {
            
            /*
             * Inequality constraint
             */
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = _state->v_neginf;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
        }
    }
    subsolver->sparseefflc.m = subsolver->sparseefflc.m+(nlec+nlic);
    
    /*
     * Finalize sparse matrix structure
     */
    ae_assert(subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m]<=subsolver->sparseefflc.idx.cnt, "QPSubproblemSolve: critical integrity check failed", _state);
    ae_assert(subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m]<=subsolver->sparseefflc.vals.cnt, "QPSubproblemSolve: critical integrity check failed", _state);
    subsolver->sparseefflc.ninitialized = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m];
    sparseinitduidx(&subsolver->sparseefflc, _state);
    
    /*
     * Solve quadratic program with IPM.
     *
     * We always treat its result as a valid solution, even for TerminationType<=0.
     * In case anything is wrong with solution vector, we will detect it during line
     * search phase (merit function does not increase).
     *
     * NOTE: because we cleaned up constraints that are DEFINITELY inactive within
     *       trust region, we do not have to worry about StopOnExcessiveBounds option.
     */
    rallocv(n, &subsolver->tmps, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->tmps.ptr.p_double[i] = nlcfsqp_gettrustregionk(x, i, state->trustrad, _state);
    }
    rsetallocv(n, 0.0, &subsolver->tmporigin, _state);
    if( dotrace )
    {
        stimerstart(&state->timerqp, _state);
    }
    if( state->usedensebfgs )
    {
        
        /*
         * Dense BFGS is used to generate a quasi-Newton matrix,
         * dense IPM is used to solve QP subproblems.
         */
        hessiangetmatrix(hess, ae_true, &subsolver->denseh, _state);
        vipminitdensewithslacks(&subsolver->ipmsolver, &subsolver->tmps, &subsolver->tmporigin, n, n, _state);
        vipmsetquadraticlinear(&subsolver->ipmsolver, &subsolver->denseh, &subsolver->sparsedummy, 0, ae_true, &subsolver->curb, _state);
        vipmsetconstraints(&subsolver->ipmsolver, &subsolver->curbndl, &subsolver->curbndu, &subsolver->sparseefflc, subsolver->sparseefflc.m, &subsolver->densedummy, 0, &subsolver->cural, &subsolver->curau, _state);
        vipmoptimize(&subsolver->ipmsolver, ae_false, &subsolver->tmp0, &subsolver->tmp1, &subsolver->tmp2, terminationtype, _state);
    }
    else
    {
        
        /*
         * Low-rank quasi-Newton matrix with positive-only corrections is used,
         * sparse IPM with low-rank updates is used to solve QP subproblems.
         */
        hessiangetlowrankstabilized(hess, &subsolver->tmpdiag, &subsolver->tmpcorrc, &subsolver->hesscorrd, &subsolver->hessrank, _state);
        subsolver->hesssparsediag.matrixtype = 1;
        subsolver->hesssparsediag.m = n;
        subsolver->hesssparsediag.n = n;
        iallocv(n+1, &subsolver->hesssparsediag.ridx, _state);
        iallocv(n, &subsolver->hesssparsediag.idx, _state);
        rallocv(n, &subsolver->hesssparsediag.vals, _state);
        subsolver->hesssparsediag.ridx.ptr.p_int[0] = 0;
        for(i=0; i<=n-1; i++)
        {
            subsolver->hesssparsediag.idx.ptr.p_int[i] = i;
            subsolver->hesssparsediag.vals.ptr.p_double[i] = subsolver->tmpdiag.ptr.p_double[i];
            subsolver->hesssparsediag.ridx.ptr.p_int[i+1] = i+1;
        }
        sparsecreatecrsinplace(&subsolver->hesssparsediag, _state);
        if( subsolver->hessrank>0 )
        {
            rcopyallocm(subsolver->hessrank, n, &subsolver->tmpcorrc, &subsolver->hesscorrc, _state);
        }
        ipm2init(&subsolver->ipm2, &subsolver->tmps, &subsolver->tmporigin, n, &subsolver->densedummy, &subsolver->hesssparsediag, 1, ae_true, &subsolver->hesscorrc, &subsolver->hesscorrd, subsolver->hessrank, &subsolver->curb, &subsolver->curbndl, &subsolver->curbndu, &subsolver->sparseefflc, subsolver->sparseefflc.m, &subsolver->densedummy, 0, &subsolver->cural, &subsolver->curau, _state);
        ipm2optimize(&subsolver->ipm2, ae_false, &subsolver->tmp0, &subsolver->tmp1, &subsolver->tmp2, terminationtype, _state);
    }
    if( dotrace )
    {
        stimerstop(&state->timerqp, _state);
    }
    rcopyv(n, &subsolver->tmp0, d, _state);
    
    /*
     * Analyze constraint violation
     */
    *isinfeasible = *isinfeasible||*terminationtype<=0;
    
    /*
     * Unpack results
     */
    for(i=0; i<=n-1; i++)
    {
        lagbcmult->ptr.p_double[i] = (double)(0);
        if( subsolver->retainpositivebclm.ptr.p_bool[i]&&ae_fp_greater(subsolver->tmp1.ptr.p_double[i],(double)(0)) )
        {
            lagbcmult->ptr.p_double[i] = subsolver->tmp1.ptr.p_double[i];
        }
        if( subsolver->retainnegativebclm.ptr.p_bool[i]&&ae_fp_less(subsolver->tmp1.ptr.p_double[i],(double)(0)) )
        {
            lagbcmult->ptr.p_double[i] = subsolver->tmp1.ptr.p_double[i];
        }
    }
    rcopyv(lccnt, &subsolver->tmp2, lagxcmult, _state);
    rmergemulv(lccnt, &subsolver->rescalelag, lagxcmult, _state);
    
    /*
     * Compute predicted decrease in an L1-penalized quadratic model
     */
    *predictedchangemodel = rdotvr(n, d, jac, 0, _state)+0.5*hessianvmv(hess, d, _state);
    *predictedchangepenalty = (double)(0);
    for(i=0; i<=nec+nic-1; i++)
    {
        v = rdotvr(n, x, &state->scaledcleic, i, _state)-state->scaledcleic.ptr.pp_double[i][n];
        vv = v+rdotvr(n, d, &state->scaledcleic, i, _state);
        *predictedchangepenalty = *predictedchangepenalty+rcase2(i<nec, ae_fabs(vv, _state)-ae_fabs(v, _state), ae_maxreal(vv, (double)(0), _state)-ae_maxreal(v, (double)(0), _state), _state);
    }
    for(i=0; i<=nlec+nlic-1; i++)
    {
        v = fi->ptr.p_double[1+i];
        vv = v+rdotvr(n, d, jac, 1+i, _state);
        *predictedchangepenalty = *predictedchangepenalty+rcase2(i<nlec, ae_fabs(vv, _state)-ae_fabs(v, _state), ae_maxreal(vv, (double)(0), _state)-ae_maxreal(v, (double)(0), _state), _state);
    }
    
    /*
     * Analyze length of D
     */
    ae_assert(ae_fp_eq(*d2trustradratio,0.0), "SQP: integrity check 0157 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        *d2trustradratio = ae_maxreal(*d2trustradratio, ae_fabs(d->ptr.p_double[i]/nlcfsqp_gettrustregionk(x, i, state->trustrad, _state), _state), _state);
    }
    return result;
}


/*************************************************************************
This function solves elastic  QP  subproblem  arising  during  feasibility
restoration phase.

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcfsqp_qpsubproblemsolveelastic(minfsqpstate* state,
     minfsqpsubsolver* subsolver,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagbcmult,
     /* Real    */ ae_vector* lagxcmult,
     ae_bool dotrace,
     ae_bool* isinfeasible,
     ae_int_t* terminationtype,
     double* predictedchangemodel,
     double* predictedchangepenalty,
     double* d2trustradratio,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nslack;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double v0;
    double v1;
    double v2;
    double vv;
    double vright;
    double vmax;
    ae_int_t lccnt;
    ae_int_t offsslackec;
    ae_int_t offsslacknlec;
    ae_int_t offsslackic;
    ae_int_t offsslacknlic;
    ae_int_t offs;
    ae_int_t nnz;
    ae_int_t j0;
    ae_int_t j1;
    double rescaleby;
    double tol;
    ae_bool result;

    *isinfeasible = ae_false;
    *terminationtype = 0;
    *predictedchangemodel = 0.0;
    *predictedchangepenalty = 0.0;
    *d2trustradratio = 0.0;

    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    nslack = n+2*(nec+nlec)+(nic+nlic);
    lccnt = nec+nic+nlec+nlic;
    tol = ae_maxreal(critgetepsx(&state->criteria, _state), ae_sqrt(ae_machineepsilon, _state), _state);
    
    /*
     * Locations of slack variables
     */
    offsslackec = n;
    offsslacknlec = n+2*nec;
    offsslackic = n+2*nec+2*nlec;
    offsslacknlic = n+2*(nec+nlec)+nic;
    
    /*
     * Prepare temporary structures
     */
    rvectorgrowto(&subsolver->cural, lccnt, _state);
    rvectorgrowto(&subsolver->curau, lccnt, _state);
    rallocv(lccnt, &subsolver->rescalelag, _state);
    
    /*
     * Prepare default solution: all zeros
     */
    result = ae_true;
    *terminationtype = 0;
    *predictedchangemodel = (double)(0);
    *predictedchangepenalty = (double)(0);
    *d2trustradratio = 0.0;
    *isinfeasible = ae_false;
    rsetv(n, 0.0, d, _state);
    rsetv(n, 0.0, lagbcmult, _state);
    rsetv(lccnt, 0.0, lagxcmult, _state);
    bsetallocv(nec+nic+nlec+nlic, ae_false, &subsolver->isinactive, _state);
    
    /*
     * Linear term B
     */
    rsetv(n, 0.0, &subsolver->curb, _state);
    for(i=n; i<=nslack-1; i++)
    {
        subsolver->curb.ptr.p_double[i] = 1.0;
    }
    
    /*
     * Trust radius constraints for primary variables
     */
    bsetallocv(n, ae_false, &subsolver->retainnegativebclm, _state);
    bsetallocv(n, ae_false, &subsolver->retainpositivebclm, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->curbndl.ptr.p_double[i] = -nlcfsqp_gettrustregionk(x, i, state->trustrad, _state);
        subsolver->curbndu.ptr.p_double[i] = nlcfsqp_gettrustregionk(x, i, state->trustrad, _state);
        if( state->hasbndl.ptr.p_bool[i]&&ae_fp_greater(state->scaledbndl.ptr.p_double[i]-x->ptr.p_double[i],subsolver->curbndl.ptr.p_double[i]) )
        {
            subsolver->curbndl.ptr.p_double[i] = state->scaledbndl.ptr.p_double[i]-x->ptr.p_double[i];
            subsolver->retainnegativebclm.ptr.p_bool[i] = ae_true;
        }
        if( state->hasbndu.ptr.p_bool[i]&&ae_fp_less(state->scaledbndu.ptr.p_double[i]-x->ptr.p_double[i],subsolver->curbndu.ptr.p_double[i]) )
        {
            subsolver->curbndu.ptr.p_double[i] = state->scaledbndu.ptr.p_double[i]-x->ptr.p_double[i];
            subsolver->retainpositivebclm.ptr.p_bool[i] = ae_true;
        }
    }
    
    /*
     * Prepare storage for "effective" constraining matrix
     */
    nnz = subsolver->sparserawlc.ridx.ptr.p_int[nec+nic];
    for(i=0; i<=nlec+nlic-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            if( jac->ptr.pp_double[1+i][j]!=0.0 )
            {
                nnz = nnz+1;
            }
        }
    }
    nnz = nnz+2*nec+nic;
    nnz = nnz+2*nlec+nlic;
    ivectorgrowto(&subsolver->sparseefflc.ridx, lccnt+1, _state);
    rvectorgrowto(&subsolver->sparseefflc.vals, nnz, _state);
    ivectorgrowto(&subsolver->sparseefflc.idx, nnz, _state);
    ivectorsetlengthatleast(&subsolver->sparseefflc.didx, lccnt, _state);
    ivectorsetlengthatleast(&subsolver->sparseefflc.uidx, lccnt, _state);
    subsolver->sparseefflc.ridx.ptr.p_int[0] = 0;
    subsolver->sparseefflc.m = 0;
    subsolver->sparseefflc.n = nslack;
    subsolver->sparseefflc.matrixtype = 1;
    
    /*
     * Append linear equality/inequality constraints
     *
     * Scan sparsified linear constraints stored in sparserawlc[], skip ones
     * which are inactive anywhere in the trust region.
     */
    rvectorsetlengthatleast(&subsolver->tmp0, nslack, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->tmp0.ptr.p_double[i] = x->ptr.p_double[i];
    }
    for(i=n; i<=nslack-1; i++)
    {
        subsolver->tmp0.ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=nec+nic-1; i++)
    {
        
        /*
         * Calculate:
         * * VRight - product of X[] (extended with zeros up to NSlack elements)
         *            and AR[i] - Ith row of sparserawlc matrix.
         * * VMax   - maximum value of X*ARi computed over trust region
         */
        vright = (double)(0);
        vmax = (double)(0);
        j0 = subsolver->sparserawlc.ridx.ptr.p_int[i];
        j1 = subsolver->sparserawlc.ridx.ptr.p_int[i+1]-1;
        for(k=j0; k<=j1; k++)
        {
            j = subsolver->sparserawlc.idx.ptr.p_int[k];
            v = x->ptr.p_double[j];
            vv = subsolver->sparserawlc.vals.ptr.p_double[k];
            vright = vright+vv*v;
            if( vv>=(double)0 )
            {
                vmax = vmax+vv*(v+subsolver->curbndu.ptr.p_double[j]);
            }
            else
            {
                vmax = vmax+vv*(v+subsolver->curbndl.ptr.p_double[j]);
            }
        }
        
        /*
         * Linear constraints are passed 'as is', without additional rescaling
         */
        subsolver->rescalelag.ptr.p_double[i] = 1.0;
        
        /*
         * If constraint is an inequality one and guaranteed to be inactive
         * within trust region, it is skipped (the row itself is retained but
         * filled by zeros).
         */
        if( i>=nec&&vmax<=state->scaledcleic.ptr.pp_double[i][n] )
        {
            offs = subsolver->sparseefflc.ridx.ptr.p_int[i];
            subsolver->sparseefflc.vals.ptr.p_double[offs] = (double)(-1);
            subsolver->sparseefflc.idx.ptr.p_int[offs] = offsslackic+(i-nec);
            subsolver->sparseefflc.ridx.ptr.p_int[i+1] = offs+1;
            subsolver->cural.ptr.p_double[i] = 0.0;
            subsolver->curau.ptr.p_double[i] = 0.0;
            subsolver->curbndl.ptr.p_double[offsslackic+(i-nec)] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslackic+(i-nec)] = (double)(0);
            subsolver->rescalelag.ptr.p_double[i] = 0.0;
            subsolver->isinactive.ptr.p_bool[i] = ae_true;
            continue;
        }
        
        /*
         * Start working on row I
         */
        offs = subsolver->sparseefflc.ridx.ptr.p_int[i];
        
        /*
         * Copy constraint from sparserawlc[] to sparseefflc[]
         */
        j0 = subsolver->sparserawlc.ridx.ptr.p_int[i];
        j1 = subsolver->sparserawlc.ridx.ptr.p_int[i+1]-1;
        for(k=j0; k<=j1; k++)
        {
            subsolver->sparseefflc.idx.ptr.p_int[offs] = subsolver->sparserawlc.idx.ptr.p_int[k];
            subsolver->sparseefflc.vals.ptr.p_double[offs] = subsolver->sparserawlc.vals.ptr.p_double[k];
            offs = offs+1;
        }
        
        /*
         * Set up slack variables
         */
        if( i<nec )
        {
            subsolver->sparseefflc.vals.ptr.p_double[offs+0] = (double)(-1);
            subsolver->sparseefflc.vals.ptr.p_double[offs+1] = (double)(1);
            subsolver->sparseefflc.idx.ptr.p_int[offs+0] = offsslackec+2*i+0;
            subsolver->sparseefflc.idx.ptr.p_int[offs+1] = offsslackec+2*i+1;
            offs = offs+2;
        }
        else
        {
            
            /*
             * Slack variables for inequality constraints
             */
            subsolver->sparseefflc.vals.ptr.p_double[offs] = (double)(-1);
            subsolver->sparseefflc.idx.ptr.p_int[offs] = offsslackic+(i-nec);
            offs = offs+1;
        }
        
        /*
         * Finalize row
         */
        subsolver->sparseefflc.ridx.ptr.p_int[i+1] = offs;
        
        /*
         * Set up bounds and slack part of D0.
         *
         * NOTE: bounds for equality and inequality constraints are
         *       handled differently
         */
        v = vright-state->scaledcleic.ptr.pp_double[i][n];
        if( i<nec )
        {
            subsolver->cural.ptr.p_double[i] = -v;
            subsolver->curau.ptr.p_double[i] = -v;
            subsolver->curbndl.ptr.p_double[offsslackec+2*i+0] = (double)(0);
            subsolver->curbndl.ptr.p_double[offsslackec+2*i+1] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslackec+2*i+0] = rcase2(ae_false, _state->v_posinf, ae_fabs(v, _state), _state);
            subsolver->curbndu.ptr.p_double[offsslackec+2*i+1] = rcase2(ae_false, _state->v_posinf, ae_fabs(v, _state), _state);
        }
        else
        {
            subsolver->cural.ptr.p_double[i] = _state->v_neginf;
            subsolver->curau.ptr.p_double[i] = -v;
            subsolver->curbndl.ptr.p_double[offsslackic+(i-nec)] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslackic+(i-nec)] = rcase2(ae_false, _state->v_posinf, ae_maxreal(v, 0.0, _state), _state);
        }
    }
    subsolver->sparseefflc.m = subsolver->sparseefflc.m+(nec+nic);
    
    /*
     * Append nonlinear equality/inequality constraints
     */
    for(i=0; i<=nlec+nlic-1; i++)
    {
        
        /*
         * Calculate:
         * * rescale coefficient used to normalize constraints
         * * VMax - maximum of constraint value over trust region
         */
        vmax = fi->ptr.p_double[1+i];
        vv = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            v = jac->ptr.pp_double[1+i][j];
            vv = vv+v*v;
            if( v>=(double)0 )
            {
                vmax = vmax+v*subsolver->curbndu.ptr.p_double[j];
            }
            else
            {
                vmax = vmax+v*subsolver->curbndl.ptr.p_double[j];
            }
        }
        rescaleby = (double)1/coalesce(ae_sqrt(vv, _state), (double)(1), _state);
        
        /*
         * Nonlinear constraints are passed with normalization
         */
        subsolver->rescalelag.ptr.p_double[nec+nic+i] = rescaleby;
        
        /*
         * If constraint is an inequality one and guaranteed to be inactive
         * within trust region, it is skipped (row itself is retained but
         * filled by zeros).
         */
        if( i>=nlec&&vmax<=0.0 )
        {
            offs = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i];
            subsolver->sparseefflc.vals.ptr.p_double[offs] = (double)(-1);
            subsolver->sparseefflc.idx.ptr.p_int[offs] = offsslacknlic+(i-nlec);
            subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i+1] = offs+1;
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = 0.0;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = 0.0;
            subsolver->curbndl.ptr.p_double[offsslacknlic+(i-nlec)] = 0.0;
            subsolver->curbndu.ptr.p_double[offsslacknlic+(i-nlec)] = 0.0;
            subsolver->rescalelag.ptr.p_double[nec+nic+i] = 0.0;
            subsolver->isinactive.ptr.p_bool[nec+nic+i] = ae_true;
            continue;
        }
        
        /*
         * Copy scaled row
         */
        offs = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i];
        for(j=0; j<=n-1; j++)
        {
            if( jac->ptr.pp_double[1+i][j]!=0.0 )
            {
                subsolver->sparseefflc.vals.ptr.p_double[offs] = rescaleby*jac->ptr.pp_double[1+i][j];
                subsolver->sparseefflc.idx.ptr.p_int[offs] = j;
                offs = offs+1;
            }
        }
        if( i<nlec )
        {
            
            /*
             * Add slack terms for equality constraints
             */
            subsolver->sparseefflc.vals.ptr.p_double[offs+0] = (double)(-1);
            subsolver->sparseefflc.vals.ptr.p_double[offs+1] = (double)(1);
            subsolver->sparseefflc.idx.ptr.p_int[offs+0] = offsslacknlec+2*i+0;
            subsolver->sparseefflc.idx.ptr.p_int[offs+1] = offsslacknlec+2*i+1;
            offs = offs+2;
        }
        else
        {
            
            /*
             * Add slack terms for inequality constraints
             */
            subsolver->sparseefflc.vals.ptr.p_double[offs] = (double)(-1);
            subsolver->sparseefflc.idx.ptr.p_int[offs] = offsslacknlic+(i-nlec);
            offs = offs+1;
        }
        subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i+1] = offs;
        
        /*
         * Set box constraints on slack variables and bounds on linear equality/inequality constraints
         */
        v = rescaleby*fi->ptr.p_double[1+i];
        if( i<nlec )
        {
            
            /*
             * Equality constraint
             */
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curbndl.ptr.p_double[offsslacknlec+2*i+0] = (double)(0);
            subsolver->curbndl.ptr.p_double[offsslacknlec+2*i+1] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslacknlec+2*i+0] = rcase2(ae_false, _state->v_posinf, ae_fabs(v, _state), _state);
            subsolver->curbndu.ptr.p_double[offsslacknlec+2*i+1] = rcase2(ae_false, _state->v_posinf, ae_fabs(v, _state), _state);
        }
        else
        {
            
            /*
             * Inequality constraint
             */
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = _state->v_neginf;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curbndl.ptr.p_double[offsslacknlic+(i-nlec)] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslacknlic+(i-nlec)] = rcase2(ae_false, _state->v_posinf, ae_maxreal(v, (double)(0), _state), _state);
        }
    }
    subsolver->sparseefflc.m = subsolver->sparseefflc.m+(nlec+nlic);
    
    /*
     * Finalize sparse matrix structure
     */
    ae_assert(subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m]<=subsolver->sparseefflc.idx.cnt, "QPSubproblemSolve: critical integrity check failed", _state);
    ae_assert(subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m]<=subsolver->sparseefflc.vals.cnt, "QPSubproblemSolve: critical integrity check failed", _state);
    subsolver->sparseefflc.ninitialized = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m];
    sparseinitduidx(&subsolver->sparseefflc, _state);
    
    /*
     * Solve quadratic program with IPM.
     *
     * We always treat its result as a valid solution, even for TerminationType<=0.
     * In case anything is wrong with solution vector, we will detect it during line
     * search phase (merit function does not increase).
     *
     * NOTE: because we cleaned up constraints that are DEFINITELY inactive within
     *       trust region, we do not have to worry about StopOnExcessiveBounds option.
     */
    rsetallocv(nslack, state->trustrad, &subsolver->tmps, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->tmps.ptr.p_double[i] = nlcfsqp_gettrustregionk(x, i, state->trustrad, _state);
    }
    rsetallocv(nslack, 0.0, &subsolver->tmporigin, _state);
    if( dotrace )
    {
        stimerstart(&state->timerqp, _state);
    }
    if( state->usedensebfgs )
    {
        
        /*
         * Dense BFGS is used to generate a quasi-Newton matrix,
         * dense IPM is used to solve QP subproblems.
         */
        rsetallocm(n, n, 0.0, &subsolver->denseh, _state);
        vipminitdensewithslacks(&subsolver->ipmsolver, &subsolver->tmps, &subsolver->tmporigin, n, nslack, _state);
        vipmsetquadraticlinear(&subsolver->ipmsolver, &subsolver->denseh, &subsolver->sparsedummy, 0, ae_true, &subsolver->curb, _state);
        vipmsetconstraints(&subsolver->ipmsolver, &subsolver->curbndl, &subsolver->curbndu, &subsolver->sparseefflc, subsolver->sparseefflc.m, &subsolver->densedummy, 0, &subsolver->cural, &subsolver->curau, _state);
        vipmoptimize(&subsolver->ipmsolver, ae_false, &subsolver->tmp0, &subsolver->tmp1, &subsolver->tmp2, terminationtype, _state);
    }
    else
    {
        
        /*
         * Low-rank quasi-Newton matrix with positive-only corrections is used,
         * sparse IPM with low-rank updates is used to solve QP subproblems.
         */
        subsolver->hesssparsediag.matrixtype = 1;
        subsolver->hesssparsediag.m = nslack;
        subsolver->hesssparsediag.n = nslack;
        iallocv(nslack+1, &subsolver->hesssparsediag.ridx, _state);
        iallocv(nslack, &subsolver->hesssparsediag.idx, _state);
        rallocv(nslack, &subsolver->hesssparsediag.vals, _state);
        subsolver->hesssparsediag.ridx.ptr.p_int[0] = 0;
        for(i=0; i<=nslack-1; i++)
        {
            subsolver->hesssparsediag.idx.ptr.p_int[i] = i;
            subsolver->hesssparsediag.vals.ptr.p_double[i] = ae_sqrt(ae_machineepsilon, _state);
            subsolver->hesssparsediag.ridx.ptr.p_int[i+1] = i+1;
        }
        sparsecreatecrsinplace(&subsolver->hesssparsediag, _state);
        ipm2init(&subsolver->ipm2, &subsolver->tmps, &subsolver->tmporigin, nslack, &subsolver->densedummy, &subsolver->hesssparsediag, 1, ae_true, &subsolver->hesscorrc, &subsolver->hesscorrd, 0, &subsolver->curb, &subsolver->curbndl, &subsolver->curbndu, &subsolver->sparseefflc, subsolver->sparseefflc.m, &subsolver->densedummy, 0, &subsolver->cural, &subsolver->curau, _state);
        ipm2optimize(&subsolver->ipm2, ae_false, &subsolver->tmp0, &subsolver->tmp1, &subsolver->tmp2, terminationtype, _state);
    }
    if( dotrace )
    {
        stimerstop(&state->timerqp, _state);
    }
    rcopyv(n, &subsolver->tmp0, d, _state);
    
    /*
     * Analyze decrease in constraint violation
     */
    for(i=0; i<=nec+nic-1; i++)
    {
        if( !subsolver->isinactive.ptr.p_bool[i] )
        {
            
            /*
             * Calculate product of X+D with I-th linear constraint
             */
            v0 = (double)(0);
            v1 = (double)(0);
            v2 = (double)(0);
            j0 = subsolver->sparserawlc.ridx.ptr.p_int[i];
            j1 = subsolver->sparserawlc.ridx.ptr.p_int[i+1]-1;
            for(k=j0; k<=j1; k++)
            {
                vv = subsolver->sparserawlc.vals.ptr.p_double[k];
                v0 = v0+vv*x->ptr.p_double[subsolver->sparserawlc.idx.ptr.p_int[k]];
                v1 = v1+vv*d->ptr.p_double[subsolver->sparserawlc.idx.ptr.p_int[k]];
                v2 = v2+vv*vv;
            }
            v1 = v1+v0;
            v1 = v1-state->scaledcleic.ptr.pp_double[i][n];
            if( i<nec )
            {
                v1 = ae_fabs(v1, _state);
                *isinfeasible = *isinfeasible||v1/coalesce(ae_sqrt(v2, _state), (double)(1), _state)>tol;
            }
            else
            {
                v1 = ae_maxreal(v1, 0.0, _state);
                *isinfeasible = *isinfeasible||v1/coalesce(ae_sqrt(v2, _state), (double)(1), _state)>tol;
            }
        }
    }
    for(i=0; i<=nlec+nlic-1; i++)
    {
        if( !subsolver->isinactive.ptr.p_bool[nec+nic+i] )
        {
            v1 = subsolver->rescalelag.ptr.p_double[nec+nic+i]*(fi->ptr.p_double[1+i]+rdotvr(n, d, jac, 1+i, _state));
            v2 = rdotrr(n, jac, 1+i, jac, 1+i, _state);
            if( i<nlec )
            {
                v1 = ae_fabs(v1, _state);
                *isinfeasible = *isinfeasible||v1/coalesce(ae_sqrt(v2, _state), (double)(1), _state)>tol;
            }
            else
            {
                v1 = ae_maxreal(v1, 0.0, _state);
                *isinfeasible = *isinfeasible||v1/coalesce(ae_sqrt(v2, _state), (double)(1), _state)>tol;
            }
        }
    }
    
    /*
     * Unpack results
     */
    for(i=0; i<=n-1; i++)
    {
        lagbcmult->ptr.p_double[i] = (double)(0);
        if( subsolver->retainpositivebclm.ptr.p_bool[i]&&ae_fp_greater(subsolver->tmp1.ptr.p_double[i],(double)(0)) )
        {
            lagbcmult->ptr.p_double[i] = subsolver->tmp1.ptr.p_double[i];
        }
        if( subsolver->retainnegativebclm.ptr.p_bool[i]&&ae_fp_less(subsolver->tmp1.ptr.p_double[i],(double)(0)) )
        {
            lagbcmult->ptr.p_double[i] = subsolver->tmp1.ptr.p_double[i];
        }
    }
    rcopyv(lccnt, &subsolver->tmp2, lagxcmult, _state);
    rmergemulv(lccnt, &subsolver->rescalelag, lagxcmult, _state);
    
    /*
     * Compute predicted decrease in an L1-penalized quadratic model
     */
    *predictedchangemodel = rdotvr(n, d, jac, 0, _state)+0.5*hessianvmv(hess, d, _state);
    *predictedchangepenalty = (double)(0);
    for(i=0; i<=nec+nic-1; i++)
    {
        v = rdotvr(n, x, &state->scaledcleic, i, _state)-state->scaledcleic.ptr.pp_double[i][n];
        vv = v+rdotvr(n, d, &state->scaledcleic, i, _state);
        *predictedchangepenalty = *predictedchangepenalty+rcase2(i<nec, ae_fabs(vv, _state)-ae_fabs(v, _state), ae_maxreal(vv, (double)(0), _state)-ae_maxreal(v, (double)(0), _state), _state);
    }
    for(i=0; i<=nlec+nlic-1; i++)
    {
        v = fi->ptr.p_double[1+i];
        vv = v+rdotvr(n, d, jac, 1+i, _state);
        *predictedchangepenalty = *predictedchangepenalty+rcase2(i<nlec, ae_fabs(vv, _state)-ae_fabs(v, _state), ae_maxreal(vv, (double)(0), _state)-ae_maxreal(v, (double)(0), _state), _state);
    }
    
    /*
     * Analyze length of D
     */
    ae_assert(ae_fp_eq(*d2trustradratio,0.0), "SQP: integrity check 0157 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        *d2trustradratio = ae_maxreal(*d2trustradratio, ae_fabs(d->ptr.p_double[i]/nlcfsqp_gettrustregionk(x, i, state->trustrad, _state), _state), _state);
    }
    return result;
}


/*************************************************************************
Copies X to State.X
*************************************************************************/
static void nlcfsqp_sqpsendx(minfsqpstate* state,
     /* Real    */ const ae_vector* xs,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i]&&xs->ptr.p_double[i]<=state->scaledbndl.ptr.p_double[i] )
        {
            state->x.ptr.p_double[i] = state->scaledbndl.ptr.p_double[i];
            continue;
        }
        if( state->hasbndu.ptr.p_bool[i]&&xs->ptr.p_double[i]>=state->scaledbndu.ptr.p_double[i] )
        {
            state->x.ptr.p_double[i] = state->scaledbndu.ptr.p_double[i];
            continue;
        }
        state->x.ptr.p_double[i] = xs->ptr.p_double[i];
    }
}


/*************************************************************************
Retrieves F-vector and scaled Jacobian, copies them to FiS and JS.

Returns True on success, False on failure (when F or J are not finite numbers).
*************************************************************************/
static ae_bool nlcfsqp_sqpretrievefij(const minfsqpstate* state,
     varsfuncjac* vfj,
     ae_state *_state)
{
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;
    ae_bool result;


    n = state->n;
    nlec = state->nlec;
    nlic = state->nlic;
    v = (double)(0);
    for(i=0; i<=nlec+nlic; i++)
    {
        vv = (double)1/state->fscales.ptr.p_double[i];
        vfj->fi.ptr.p_double[i] = vv*state->fi.ptr.p_double[i];
        v = v+vfj->fi.ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            vfj->jac.ptr.pp_double[i][j] = vv*state->j.ptr.pp_double[i][j];
            v = v+vfj->jac.ptr.pp_double[i][j];
        }
    }
    result = ae_isfinite(v, _state);
    return result;
}


/*************************************************************************
This function calculates Lagrangian of the problem (in scaled variables):
its value and gradient.

Additionally it also estimates violation of linear constraints at the point
as well as index of the most violated constraint
*************************************************************************/
static void nlcfsqp_lagrangianfg(minfsqpstate* state,
     /* Real    */ const ae_vector* x,
     double trustrad,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* j,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     ae_bool uselagrangeterms,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    double v;
    double vlag;
    double vact;
    double vd;
    ae_bool usesparsegemv;
    double lagalpha;

    *f = 0.0;

    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    lagalpha = rcase2(uselagrangeterms, (double)(1), (double)(0), _state);
    
    /*
     * Target function
     */
    *f = fi->ptr.p_double[0];
    for(i=0; i<=n-1; i++)
    {
        g->ptr.p_double[i] = j->ptr.pp_double[0][i];
    }
    
    /*
     * Lagrangian terms for box constraints
     */
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Lagrangian terms
         */
        *f = *f+lagalpha*lagbcmult->ptr.p_double[i]*x->ptr.p_double[i];
        g->ptr.p_double[i] = g->ptr.p_double[i]+lagalpha*lagbcmult->ptr.p_double[i];
        
        /*
         * Penalty term
         */
        if( state->hasbndl.ptr.p_bool[i]&&ae_fp_less(x->ptr.p_double[i],state->scaledbndl.ptr.p_double[i]) )
        {
            *f = *f+nlcfsqp_augmentationfactor*(x->ptr.p_double[i]-state->scaledbndl.ptr.p_double[i])*(x->ptr.p_double[i]-state->scaledbndl.ptr.p_double[i]);
            g->ptr.p_double[i] = g->ptr.p_double[i]+(double)2*nlcfsqp_augmentationfactor*(x->ptr.p_double[i]-state->scaledbndl.ptr.p_double[i]);
        }
        if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(x->ptr.p_double[i],state->scaledbndu.ptr.p_double[i]) )
        {
            *f = *f+nlcfsqp_augmentationfactor*(x->ptr.p_double[i]-state->scaledbndu.ptr.p_double[i])*(x->ptr.p_double[i]-state->scaledbndu.ptr.p_double[i]);
            g->ptr.p_double[i] = g->ptr.p_double[i]+(double)2*nlcfsqp_augmentationfactor*(x->ptr.p_double[i]-state->scaledbndu.ptr.p_double[i]);
        }
    }
    
    /*
     * Lagrangian terms for linear constraints, constraint violations
     */
    if( nec+nic>0 )
    {
        usesparsegemv = (double)state->subsolver.sparserawlc.ridx.ptr.p_int[nec+nic]<sparselevel2density(_state)*(double)n*(double)(nec+nic);
        rvectorsetlengthatleast(&state->sclagtmp0, ae_maxint(nec+nic, n, _state), _state);
        rvectorsetlengthatleast(&state->sclagtmp1, ae_maxint(nec+nic, n, _state), _state);
        if( usesparsegemv )
        {
            sparsemv(&state->subsolver.sparserawlc, x, &state->sclagtmp0, _state);
        }
        else
        {
            rmatrixgemv(nec+nic, n, 1.0, &state->scaledcleic, 0, 0, 0, x, 0, 0.0, &state->sclagtmp0, 0, _state);
        }
        for(i=0; i<=nec+nic-1; i++)
        {
            
            /*
             * Prepare
             */
            v = state->sclagtmp0.ptr.p_double[i]-state->scaledcleic.ptr.pp_double[i][n];
            vlag = lagalpha*lagxcmult->ptr.p_double[i];
            state->sclagtmp1.ptr.p_double[i] = (double)(0);
            
            /*
             * Primary Lagrangian term
             */
            vact = v;
            vd = (double)(1);
            *f = *f+vlag*vact;
            state->sclagtmp1.ptr.p_double[i] = state->sclagtmp1.ptr.p_double[i]+vlag*vd;
            
            /*
             * Quadratic augmentation term
             */
            if( i<nec||v>(double)0 )
            {
                vact = v;
            }
            else
            {
                vact = (double)(0);
            }
            *f = *f+0.5*nlcfsqp_augmentationfactor*vact*vact;
            state->sclagtmp1.ptr.p_double[i] = state->sclagtmp1.ptr.p_double[i]+nlcfsqp_augmentationfactor*vact;
        }
        if( usesparsegemv )
        {
            sparsemtv(&state->subsolver.sparserawlc, &state->sclagtmp1, &state->sclagtmp0, _state);
            for(i=0; i<=n-1; i++)
            {
                g->ptr.p_double[i] = g->ptr.p_double[i]+state->sclagtmp0.ptr.p_double[i];
            }
        }
        else
        {
            rmatrixgemv(n, nec+nic, 1.0, &state->scaledcleic, 0, 0, 1, &state->sclagtmp1, 0, 1.0, g, 0, _state);
        }
    }
    
    /*
     * Lagrangian terms for nonlinear constraints
     */
    rvectorsetlengthatleast(&state->sclagtmp1, nlec+nlic, _state);
    for(i=0; i<=nlec+nlic-1; i++)
    {
        v = fi->ptr.p_double[1+i];
        vlag = lagalpha*lagxcmult->ptr.p_double[nec+nic+i];
        state->sclagtmp1.ptr.p_double[i] = (double)(0);
        
        /*
         * Lagrangian term
         */
        vact = v;
        vd = (double)(1);
        *f = *f+vlag*vact;
        state->sclagtmp1.ptr.p_double[i] = state->sclagtmp1.ptr.p_double[i]+vlag*vd;
        
        /*
         * Augmentation term
         */
        if( i<nlec||v>(double)0 )
        {
            vact = v;
        }
        else
        {
            vact = (double)(0);
        }
        *f = *f+0.5*nlcfsqp_augmentationfactor*vact*vact;
        state->sclagtmp1.ptr.p_double[i] = state->sclagtmp1.ptr.p_double[i]+nlcfsqp_augmentationfactor*vact;
    }
    rmatrixgemv(n, nlec+nlic, 1.0, j, 1, 0, 1, &state->sclagtmp1, 0, 1.0, g, 0, _state);
}


/*************************************************************************
This function calculates target and constraint violation
*************************************************************************/
static void nlcfsqp_targetandconstraints(minfsqpstate* state,
     const varsfuncjac* vfj,
     double* tgt,
     double* cv,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    double v;

    *tgt = 0.0;
    *cv = 0.0;

    ae_assert(vfj->isdense, "FSQP: integrity check 1057 failed", _state);
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    
    /*
     * Target and constraints: initial state
     */
    *tgt = vfj->fi.ptr.p_double[0];
    *cv = (double)(0);
    
    /*
     * Constraint violation
     */
    rvectorsetlengthatleast(&state->mftmp0, nec+nic, _state);
    rmatrixgemv(nec+nic, n, 1.0, &state->scaledcleic, 0, 0, 0, &vfj->x, 0, 0.0, &state->mftmp0, 0, _state);
    for(i=0; i<=nec+nic-1; i++)
    {
        v = state->mftmp0.ptr.p_double[i]-state->scaledcleic.ptr.pp_double[i][n];
        if( i<nec )
        {
            *cv = *cv+ae_fabs(v, _state);
        }
        else
        {
            *cv = *cv+ae_maxreal(v, (double)(0), _state);
        }
    }
    for(i=0; i<=nlec+nlic-1; i++)
    {
        v = vfj->fi.ptr.p_double[1+i];
        if( i<nlec )
        {
            *cv = *cv+ae_fabs(v, _state);
        }
        else
        {
            *cv = *cv+ae_maxreal(v, (double)(0), _state);
        }
    }
}


/*************************************************************************
This function calculates raw (unaugmented and smooth) Lagrangian
*************************************************************************/
static double nlcfsqp_rawlagrangian(minfsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     ae_state *_state)
{
    double tmp0;
    double tmp1;
    double result;


    nlcfsqp_meritfunctionandrawlagrangian(state, x, fi, lagbcmult, lagxcmult, 0.0, &tmp0, &tmp1, _state);
    result = tmp1;
    return result;
}


/*************************************************************************
This function calculates L1-penalized merit function and raw  (smooth  and
un-augmented) Lagrangian
*************************************************************************/
static void nlcfsqp_meritfunctionandrawlagrangian(minfsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     double* meritf,
     double* rawlag,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    double v;

    *meritf = 0.0;
    *rawlag = 0.0;

    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    
    /*
     * Merit function and Lagrangian: primary term
     */
    *meritf = fi->ptr.p_double[0];
    *rawlag = fi->ptr.p_double[0];
    
    /*
     * Lagrangian: handle box constraints
     *
     * NOTE: we do not add box constrained term to the merit function because
     *       box constraints are handled exactly and we do not violate them.
     */
    for(i=0; i<=n-1; i++)
    {
        *rawlag = *rawlag+lagbcmult->ptr.p_double[i]*x->ptr.p_double[i];
    }
    
    /*
     * Merit function: augmentation and penalty for linear constraints
     */
    rvectorsetlengthatleast(&state->mftmp0, nec+nic, _state);
    rmatrixgemv(nec+nic, n, 1.0, &state->scaledcleic, 0, 0, 0, x, 0, 0.0, &state->mftmp0, 0, _state);
    for(i=0; i<=nec+nic-1; i++)
    {
        v = state->mftmp0.ptr.p_double[i]-state->scaledcleic.ptr.pp_double[i][n];
        if( i<nec )
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            *meritf = *meritf+meritmu*ae_fabs(v, _state);
            
            /*
             * Raw Lagrangian
             */
            *rawlag = *rawlag+lagxcmult->ptr.p_double[i]*v;
        }
        else
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            *meritf = *meritf+meritmu*ae_maxreal(v, (double)(0), _state);
            
            /*
             * Raw Lagrangian
             */
            *rawlag = *rawlag+lagxcmult->ptr.p_double[i]*v;
        }
    }
    
    /*
     * Merit function: augmentation and penalty for nonlinear constraints
     */
    for(i=0; i<=nlec+nlic-1; i++)
    {
        v = fi->ptr.p_double[1+i];
        if( i<nlec )
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            *meritf = *meritf+meritmu*ae_fabs(v, _state);
            
            /*
             * Raw Lagrangian
             */
            *rawlag = *rawlag+lagxcmult->ptr.p_double[nec+nic+i]*v;
        }
        else
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            *meritf = *meritf+meritmu*ae_maxreal(v, (double)(0), _state);
            
            /*
             * Raw Lagrangian
             */
            *rawlag = *rawlag+lagxcmult->ptr.p_double[nec+nic+i]*v;
        }
    }
}


/*************************************************************************
Returns k-th component of the trust region

  -- ALGLIB --
     Copyright 25.09.2023 by Bochkanov Sergey
*************************************************************************/
static double nlcfsqp_gettrustregionk(/* Real    */ const ae_vector* xcur,
     ae_int_t k,
     double trustrad,
     ae_state *_state)
{
    double result;


    result = trustrad*ae_maxreal(ae_fabs(xcur->ptr.p_double[k], _state), (double)(1), _state);
    return result;
}


/*************************************************************************
Checks acceptability by the filter

  -- ALGLIB --
     Copyright 25.09.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcfsqp_isacceptable(const minfsqpstate* state,
     double tgt0,
     double h0,
     double tgt1,
     double h1,
     double predictedchangemodel,
     double predictedchangepenalty,
     ae_state *_state)
{
    ae_bool result;


    if( ae_fp_less(predictedchangemodel,(double)(0))&&ae_fp_greater(tgt1-tgt0,nlcfsqp_sufficientdecreasesigma*predictedchangemodel) )
    {
        result = ae_false;
        return result;
    }
    result = nlpfisacceptable(&state->filter, tgt1, h1, _state);
    return result;
}


void _minfsqpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minfsqpsubsolver *p = (minfsqpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _vipmstate_init(&p->ipmsolver, _state, make_automatic);
    _ipm2state_init(&p->ipm2, _state, make_automatic);
    ae_vector_init(&p->curb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cural, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curau, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparserawlc, _state, make_automatic);
    _sparsematrix_init(&p->sparseefflc, _state, make_automatic);
    ae_matrix_init(&p->denseh, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummy1, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->densedummy, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsedummy, _state, make_automatic);
    ae_vector_init(&p->tmps, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmporigin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->retainnegativebclm, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->retainpositivebclm, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->rescalelag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->isinactive, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->tmpcorrc, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpdiag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hessdiag, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->hesscorrc, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hesscorrd, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->hesssparsediag, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasal, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasau, 0, DT_BOOL, _state, make_automatic);
}


void _minfsqpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minfsqpsubsolver       *dst = (minfsqpsubsolver*)_dst;
    const minfsqpsubsolver *src = (const minfsqpsubsolver*)_src;
    _vipmstate_init_copy(&dst->ipmsolver, &src->ipmsolver, _state, make_automatic);
    _ipm2state_init_copy(&dst->ipm2, &src->ipm2, _state, make_automatic);
    ae_vector_init_copy(&dst->curb, &src->curb, _state, make_automatic);
    ae_vector_init_copy(&dst->curbndl, &src->curbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->curbndu, &src->curbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->cural, &src->cural, _state, make_automatic);
    ae_vector_init_copy(&dst->curau, &src->curau, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparserawlc, &src->sparserawlc, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseefflc, &src->sparseefflc, _state, make_automatic);
    ae_matrix_init_copy(&dst->denseh, &src->denseh, _state, make_automatic);
    ae_vector_init_copy(&dst->dummy1, &src->dummy1, _state, make_automatic);
    ae_matrix_init_copy(&dst->densedummy, &src->densedummy, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsedummy, &src->sparsedummy, _state, make_automatic);
    ae_vector_init_copy(&dst->tmps, &src->tmps, _state, make_automatic);
    ae_vector_init_copy(&dst->tmporigin, &src->tmporigin, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->retainnegativebclm, &src->retainnegativebclm, _state, make_automatic);
    ae_vector_init_copy(&dst->retainpositivebclm, &src->retainpositivebclm, _state, make_automatic);
    ae_vector_init_copy(&dst->rescalelag, &src->rescalelag, _state, make_automatic);
    ae_vector_init_copy(&dst->isinactive, &src->isinactive, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpcorrc, &src->tmpcorrc, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpdiag, &src->tmpdiag, _state, make_automatic);
    ae_vector_init_copy(&dst->hessdiag, &src->hessdiag, _state, make_automatic);
    ae_matrix_init_copy(&dst->hesscorrc, &src->hesscorrc, _state, make_automatic);
    ae_vector_init_copy(&dst->hesscorrd, &src->hesscorrd, _state, make_automatic);
    dst->hessrank = src->hessrank;
    _sparsematrix_init_copy(&dst->hesssparsediag, &src->hesssparsediag, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasal, &src->hasal, _state, make_automatic);
    ae_vector_init_copy(&dst->hasau, &src->hasau, _state, make_automatic);
}


void _minfsqpsubsolver_clear(void* _p)
{
    minfsqpsubsolver *p = (minfsqpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _vipmstate_clear(&p->ipmsolver);
    _ipm2state_clear(&p->ipm2);
    ae_vector_clear(&p->curb);
    ae_vector_clear(&p->curbndl);
    ae_vector_clear(&p->curbndu);
    ae_vector_clear(&p->cural);
    ae_vector_clear(&p->curau);
    _sparsematrix_clear(&p->sparserawlc);
    _sparsematrix_clear(&p->sparseefflc);
    ae_matrix_clear(&p->denseh);
    ae_vector_clear(&p->dummy1);
    ae_matrix_clear(&p->densedummy);
    _sparsematrix_clear(&p->sparsedummy);
    ae_vector_clear(&p->tmps);
    ae_vector_clear(&p->tmporigin);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmp2);
    ae_vector_clear(&p->retainnegativebclm);
    ae_vector_clear(&p->retainpositivebclm);
    ae_vector_clear(&p->rescalelag);
    ae_vector_clear(&p->isinactive);
    ae_matrix_clear(&p->tmpcorrc);
    ae_vector_clear(&p->tmpdiag);
    ae_vector_clear(&p->hessdiag);
    ae_matrix_clear(&p->hesscorrc);
    ae_vector_clear(&p->hesscorrd);
    _sparsematrix_clear(&p->hesssparsediag);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->hasal);
    ae_vector_clear(&p->hasau);
}


void _minfsqpsubsolver_destroy(void* _p)
{
    minfsqpsubsolver *p = (minfsqpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _vipmstate_destroy(&p->ipmsolver);
    _ipm2state_destroy(&p->ipm2);
    ae_vector_destroy(&p->curb);
    ae_vector_destroy(&p->curbndl);
    ae_vector_destroy(&p->curbndu);
    ae_vector_destroy(&p->cural);
    ae_vector_destroy(&p->curau);
    _sparsematrix_destroy(&p->sparserawlc);
    _sparsematrix_destroy(&p->sparseefflc);
    ae_matrix_destroy(&p->denseh);
    ae_vector_destroy(&p->dummy1);
    ae_matrix_destroy(&p->densedummy);
    _sparsematrix_destroy(&p->sparsedummy);
    ae_vector_destroy(&p->tmps);
    ae_vector_destroy(&p->tmporigin);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmp2);
    ae_vector_destroy(&p->retainnegativebclm);
    ae_vector_destroy(&p->retainpositivebclm);
    ae_vector_destroy(&p->rescalelag);
    ae_vector_destroy(&p->isinactive);
    ae_matrix_destroy(&p->tmpcorrc);
    ae_vector_destroy(&p->tmpdiag);
    ae_vector_destroy(&p->hessdiag);
    ae_matrix_destroy(&p->hesscorrc);
    ae_vector_destroy(&p->hesscorrd);
    _sparsematrix_destroy(&p->hesssparsediag);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->hasal);
    ae_vector_destroy(&p->hasau);
}


void _minfsqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minfsqpstate *p = (minfsqpstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->scaledcleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lcsrcidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->scaledbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->scaledbndu, 0, DT_REAL, _state, make_automatic);
    _nlpstoppingcriteria_init(&p->criteria, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    _varsfuncjac_init(&p->stepk, _state, make_automatic);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xprev, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fscales, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tracegamma, 0, DT_REAL, _state, make_automatic);
    _minfsqpsubsolver_init(&p->subsolver, _state, make_automatic);
    _xbfgshessian_init(&p->hess, _state, make_automatic);
    ae_vector_init(&p->lagbcmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagxcmult, 0, DT_REAL, _state, make_automatic);
    _varsfuncjac_init(&p->cand, _state, make_automatic);
    _varsfuncjac_init(&p->corr, _state, make_automatic);
    _varsfuncjac_init(&p->probe, _state, make_automatic);
    ae_vector_init(&p->dtrial, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dmu, 0, DT_REAL, _state, make_automatic);
    _nlpfilter_init(&p->filter, _state, make_automatic);
    ae_vector_init(&p->tmppend, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmphd, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummylagbcmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummylagxcmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmplaggrad, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpcandlaggrad, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmphdiag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sclagtmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sclagtmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->mftmp0, 0, DT_REAL, _state, make_automatic);
    _stimer_init(&p->timertotal, _state, make_automatic);
    _stimer_init(&p->timerqp, _state, make_automatic);
    _stimer_init(&p->timercallback, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
}


void _minfsqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minfsqpstate       *dst = (minfsqpstate*)_dst;
    const minfsqpstate *src = (const minfsqpstate*)_src;
    dst->n = src->n;
    dst->nec = src->nec;
    dst->nic = src->nic;
    dst->nlec = src->nlec;
    dst->nlic = src->nlic;
    dst->usedensebfgs = src->usedensebfgs;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_matrix_init_copy(&dst->scaledcleic, &src->scaledcleic, _state, make_automatic);
    ae_vector_init_copy(&dst->lcsrcidx, &src->lcsrcidx, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndl, &src->scaledbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndu, &src->scaledbndu, _state, make_automatic);
    _nlpstoppingcriteria_init_copy(&dst->criteria, &src->criteria, _state, make_automatic);
    dst->bfgsresetfreq = src->bfgsresetfreq;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    dst->f = src->f;
    dst->needfij = src->needfij;
    dst->xupdated = src->xupdated;
    dst->trustrad = src->trustrad;
    dst->trustradgrowth = src->trustradgrowth;
    dst->trustradstagnationcnt = src->trustradstagnationcnt;
    dst->fstagnationcnt = src->fstagnationcnt;
    _varsfuncjac_init_copy(&dst->stepk, &src->stepk, _state, make_automatic);
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->xprev, &src->xprev, _state, make_automatic);
    ae_vector_init_copy(&dst->fscales, &src->fscales, _state, make_automatic);
    ae_vector_init_copy(&dst->tracegamma, &src->tracegamma, _state, make_automatic);
    _minfsqpsubsolver_init_copy(&dst->subsolver, &src->subsolver, _state, make_automatic);
    _xbfgshessian_init_copy(&dst->hess, &src->hess, _state, make_automatic);
    ae_vector_init_copy(&dst->lagbcmult, &src->lagbcmult, _state, make_automatic);
    ae_vector_init_copy(&dst->lagxcmult, &src->lagxcmult, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->cand, &src->cand, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->corr, &src->corr, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->probe, &src->probe, _state, make_automatic);
    ae_vector_init_copy(&dst->dtrial, &src->dtrial, _state, make_automatic);
    ae_vector_init_copy(&dst->d0, &src->d0, _state, make_automatic);
    ae_vector_init_copy(&dst->d1, &src->d1, _state, make_automatic);
    ae_vector_init_copy(&dst->dmu, &src->dmu, _state, make_automatic);
    _nlpfilter_init_copy(&dst->filter, &src->filter, _state, make_automatic);
    ae_vector_init_copy(&dst->tmppend, &src->tmppend, _state, make_automatic);
    ae_vector_init_copy(&dst->tmphd, &src->tmphd, _state, make_automatic);
    ae_vector_init_copy(&dst->dummylagbcmult, &src->dummylagbcmult, _state, make_automatic);
    ae_vector_init_copy(&dst->dummylagxcmult, &src->dummylagxcmult, _state, make_automatic);
    ae_vector_init_copy(&dst->tmplaggrad, &src->tmplaggrad, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpcandlaggrad, &src->tmpcandlaggrad, _state, make_automatic);
    ae_vector_init_copy(&dst->tmphdiag, &src->tmphdiag, _state, make_automatic);
    ae_vector_init_copy(&dst->sclagtmp0, &src->sclagtmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->sclagtmp1, &src->sclagtmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->mftmp0, &src->mftmp0, _state, make_automatic);
    dst->repiterationscount = src->repiterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repbcerr = src->repbcerr;
    dst->repbcidx = src->repbcidx;
    dst->replcerr = src->replcerr;
    dst->replcidx = src->replcidx;
    dst->repnlcerr = src->repnlcerr;
    dst->repnlcidx = src->repnlcidx;
    _stimer_init_copy(&dst->timertotal, &src->timertotal, _state, make_automatic);
    _stimer_init_copy(&dst->timerqp, &src->timerqp, _state, make_automatic);
    _stimer_init_copy(&dst->timercallback, &src->timercallback, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
}


void _minfsqpstate_clear(void* _p)
{
    minfsqpstate *p = (minfsqpstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_matrix_clear(&p->scaledcleic);
    ae_vector_clear(&p->lcsrcidx);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->scaledbndl);
    ae_vector_clear(&p->scaledbndu);
    _nlpstoppingcriteria_clear(&p->criteria);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    _varsfuncjac_clear(&p->stepk);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->xprev);
    ae_vector_clear(&p->fscales);
    ae_vector_clear(&p->tracegamma);
    _minfsqpsubsolver_clear(&p->subsolver);
    _xbfgshessian_clear(&p->hess);
    ae_vector_clear(&p->lagbcmult);
    ae_vector_clear(&p->lagxcmult);
    _varsfuncjac_clear(&p->cand);
    _varsfuncjac_clear(&p->corr);
    _varsfuncjac_clear(&p->probe);
    ae_vector_clear(&p->dtrial);
    ae_vector_clear(&p->d0);
    ae_vector_clear(&p->d1);
    ae_vector_clear(&p->dmu);
    _nlpfilter_clear(&p->filter);
    ae_vector_clear(&p->tmppend);
    ae_vector_clear(&p->tmphd);
    ae_vector_clear(&p->dummylagbcmult);
    ae_vector_clear(&p->dummylagxcmult);
    ae_vector_clear(&p->tmplaggrad);
    ae_vector_clear(&p->tmpcandlaggrad);
    ae_vector_clear(&p->tmphdiag);
    ae_vector_clear(&p->sclagtmp0);
    ae_vector_clear(&p->sclagtmp1);
    ae_vector_clear(&p->mftmp0);
    _stimer_clear(&p->timertotal);
    _stimer_clear(&p->timerqp);
    _stimer_clear(&p->timercallback);
    _rcommstate_clear(&p->rstate);
}


void _minfsqpstate_destroy(void* _p)
{
    minfsqpstate *p = (minfsqpstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_matrix_destroy(&p->scaledcleic);
    ae_vector_destroy(&p->lcsrcidx);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->scaledbndl);
    ae_vector_destroy(&p->scaledbndu);
    _nlpstoppingcriteria_destroy(&p->criteria);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    _varsfuncjac_destroy(&p->stepk);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->xprev);
    ae_vector_destroy(&p->fscales);
    ae_vector_destroy(&p->tracegamma);
    _minfsqpsubsolver_destroy(&p->subsolver);
    _xbfgshessian_destroy(&p->hess);
    ae_vector_destroy(&p->lagbcmult);
    ae_vector_destroy(&p->lagxcmult);
    _varsfuncjac_destroy(&p->cand);
    _varsfuncjac_destroy(&p->corr);
    _varsfuncjac_destroy(&p->probe);
    ae_vector_destroy(&p->dtrial);
    ae_vector_destroy(&p->d0);
    ae_vector_destroy(&p->d1);
    ae_vector_destroy(&p->dmu);
    _nlpfilter_destroy(&p->filter);
    ae_vector_destroy(&p->tmppend);
    ae_vector_destroy(&p->tmphd);
    ae_vector_destroy(&p->dummylagbcmult);
    ae_vector_destroy(&p->dummylagxcmult);
    ae_vector_destroy(&p->tmplaggrad);
    ae_vector_destroy(&p->tmpcandlaggrad);
    ae_vector_destroy(&p->tmphdiag);
    ae_vector_destroy(&p->sclagtmp0);
    ae_vector_destroy(&p->sclagtmp1);
    ae_vector_destroy(&p->mftmp0);
    _stimer_destroy(&p->timertotal);
    _stimer_destroy(&p->timerqp);
    _stimer_destroy(&p->timercallback);
    _rcommstate_destroy(&p->rstate);
}


/*$ End $*/
