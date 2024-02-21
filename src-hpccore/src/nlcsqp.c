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
#include "nlcsqp.h"


/*$ Declarations $*/
static double nlcsqp_sqpdeltadecrease = 0.50;
static double nlcsqp_sqpdeltaincrease = 0.99;
static double nlcsqp_maxtrustraddecay = 0.1;
static double nlcsqp_deftrustradgrowth = 1.41;
static double nlcsqp_maxtrustradgrowth = 10.0;
static double nlcsqp_momentumgrowth = 2;
static double nlcsqp_maxmeritmu = 1.0E5;
static double nlcsqp_augmentationfactor = 0.0;
static double nlcsqp_inittrustrad = 0.1;
static double nlcsqp_stagnationepsf = 1.0E-12;
static ae_int_t nlcsqp_fstagnationlimit = 20;
static ae_int_t nlcsqp_trustradstagnationlimit = 10;
static double nlcsqp_sqpbigscale = 5.0;
static double nlcsqp_sqpsmallscale = 0.2;
static double nlcsqp_sqpmaxrescale = 10.0;
static double nlcsqp_sqpminrescale = 0.1;
static ae_int_t nlcsqp_defaultbfgsresetfreq = 999999;
static double nlcsqp_sufficientdecreasesigma = 0.001;
static double nlcsqp_meritmueps = 0.15;
static ae_int_t nlcsqp_xbfgsmemlen = 8;
static double nlcsqp_xbfgsmaxhess = 1.0E6;
static ae_int_t nlcsqp_nonmonotoniclen = 3;
static double nlcsqp_constraintsstagnationeps = 1.0E-5;
static void nlcsqp_initqpsubsolver(const minsqpstate* sstate,
     minsqpsubsolver* subsolver,
     ae_state *_state);
static ae_bool nlcsqp_qpsubproblemsolve(minsqpstate* state,
     minsqpsubsolver* subsolver,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     xbfgshessian* hess,
     double alphag,
     double alphah,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagbcmult,
     /* Real    */ ae_vector* lagxcmult,
     ae_bool dotrace,
     ae_int_t* terminationtype,
     double* predictedchangemodel,
     double* predictedchangepenalty,
     double* sumc1,
     double* d2trustradratio,
     ae_state *_state);
static void nlcsqp_sqpsendx(minsqpstate* state,
     /* Real    */ const ae_vector* xs,
     ae_state *_state);
static ae_bool nlcsqp_sqpretrievefij(const minsqpstate* state,
     varsfuncjac* vfj,
     ae_state *_state);
static void nlcsqp_lagrangianfg(minsqpstate* state,
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
static double nlcsqp_meritfunction(minsqpstate* state,
     const varsfuncjac* vfj,
     double meritmu,
     ae_state *_state);
static double nlcsqp_rawlagrangian(minsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     ae_state *_state);
static void nlcsqp_meritfunctionandrawlagrangian(minsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     double* meritf,
     double* rawlag,
     ae_state *_state);
static ae_bool nlcsqp_penaltiesneedincrease(minsqpstate* state,
     const varsfuncjac* currentlinearization,
     /* Real    */ const ae_vector* dtrial,
     double predictedchangemodel,
     double predictedchangepenalty,
     ae_bool dotrace,
     ae_state *_state);
static double nlcsqp_gettrustregionk(/* Real    */ const ae_vector* xcur,
     ae_int_t k,
     double trustrad,
     ae_state *_state);
static double nlcsqp_nonmonotonicadjustment(minsqpstate* state,
     double fraw,
     ae_state *_state);
static void nlcsqp_nonmonotonicsave(minsqpstate* state,
     const varsfuncjac* cur,
     ae_state *_state);
static void nlcsqp_nonmonotonicmultiplyby(minsqpstate* state,
     ae_int_t jacrow,
     double v,
     ae_state *_state);


/*$ Body $*/


void minsqpinitbuf(/* Real    */ const ae_vector* bndl,
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
     minsqpstate* state,
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
    ae_vector_set_length(&state->rstate.ia, 9+1, _state);
    ae_vector_set_length(&state->rstate.ba, 14+1, _state);
    ae_vector_set_length(&state->rstate.ra, 23+1, _state);
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
    state->bfgsresetfreq = nlcsqp_defaultbfgsresetfreq;
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
    ae_assert(ae_fp_less(nlcsqp_sqpdeltadecrease,nlcsqp_sqpdeltaincrease), "MinSQP: integrity check failed", _state);
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
ae_bool minsqpiteration(minsqpstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
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
    double v;
    double vv;
    double mx;
    double deltamax;
    double multiplyby;
    double setscaleto;
    double prevtrustrad;
    ae_int_t subiterationidx;
    ae_bool trustradstagnated;
    double relativetargetdecrease;
    ae_bool dotrace;
    ae_bool doprobingalways;
    ae_bool doprobingonfailure;
    ae_bool dodetailedtrace;
    ae_bool increasemeritmu;
    ae_bool meritfstagnated;
    double f0;
    double f0raw;
    double f1;
    double tol;
    double stepklagval;
    double stepknlagval;
    double expandedrad;
    ae_bool socperformed;
    double d2trustradratio;
    ae_int_t didx;
    double sksk;
    double ykyk;
    double skyk;
    double sumc1;
    double sumc1soc;
    double predictedchange;
    double predictedchangemodel;
    double predictedchangepenalty;
    ae_bool meritdecreasefailed;
    ae_bool stepperformed;
    ae_bool goodstep;
    ae_bool firstrescale;
    ae_bool infinitiesdetected;
    ae_bool failedtorecoverfrominfinities;
    ae_bool needsoc;
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
        nslack = state->rstate.ia.ptr.p_int[1];
        nec = state->rstate.ia.ptr.p_int[2];
        nic = state->rstate.ia.ptr.p_int[3];
        nlec = state->rstate.ia.ptr.p_int[4];
        nlic = state->rstate.ia.ptr.p_int[5];
        i = state->rstate.ia.ptr.p_int[6];
        j = state->rstate.ia.ptr.p_int[7];
        subiterationidx = state->rstate.ia.ptr.p_int[8];
        didx = state->rstate.ia.ptr.p_int[9];
        trustradstagnated = state->rstate.ba.ptr.p_bool[0];
        dotrace = state->rstate.ba.ptr.p_bool[1];
        doprobingalways = state->rstate.ba.ptr.p_bool[2];
        doprobingonfailure = state->rstate.ba.ptr.p_bool[3];
        dodetailedtrace = state->rstate.ba.ptr.p_bool[4];
        increasemeritmu = state->rstate.ba.ptr.p_bool[5];
        meritfstagnated = state->rstate.ba.ptr.p_bool[6];
        socperformed = state->rstate.ba.ptr.p_bool[7];
        meritdecreasefailed = state->rstate.ba.ptr.p_bool[8];
        stepperformed = state->rstate.ba.ptr.p_bool[9];
        goodstep = state->rstate.ba.ptr.p_bool[10];
        firstrescale = state->rstate.ba.ptr.p_bool[11];
        infinitiesdetected = state->rstate.ba.ptr.p_bool[12];
        failedtorecoverfrominfinities = state->rstate.ba.ptr.p_bool[13];
        needsoc = state->rstate.ba.ptr.p_bool[14];
        v = state->rstate.ra.ptr.p_double[0];
        vv = state->rstate.ra.ptr.p_double[1];
        mx = state->rstate.ra.ptr.p_double[2];
        deltamax = state->rstate.ra.ptr.p_double[3];
        multiplyby = state->rstate.ra.ptr.p_double[4];
        setscaleto = state->rstate.ra.ptr.p_double[5];
        prevtrustrad = state->rstate.ra.ptr.p_double[6];
        relativetargetdecrease = state->rstate.ra.ptr.p_double[7];
        f0 = state->rstate.ra.ptr.p_double[8];
        f0raw = state->rstate.ra.ptr.p_double[9];
        f1 = state->rstate.ra.ptr.p_double[10];
        tol = state->rstate.ra.ptr.p_double[11];
        stepklagval = state->rstate.ra.ptr.p_double[12];
        stepknlagval = state->rstate.ra.ptr.p_double[13];
        expandedrad = state->rstate.ra.ptr.p_double[14];
        d2trustradratio = state->rstate.ra.ptr.p_double[15];
        sksk = state->rstate.ra.ptr.p_double[16];
        ykyk = state->rstate.ra.ptr.p_double[17];
        skyk = state->rstate.ra.ptr.p_double[18];
        sumc1 = state->rstate.ra.ptr.p_double[19];
        sumc1soc = state->rstate.ra.ptr.p_double[20];
        predictedchange = state->rstate.ra.ptr.p_double[21];
        predictedchangemodel = state->rstate.ra.ptr.p_double[22];
        predictedchangepenalty = state->rstate.ra.ptr.p_double[23];
    }
    else
    {
        n = 359;
        nslack = -58;
        nec = -919;
        nic = -909;
        nlec = 81;
        nlic = 255;
        i = 74;
        j = -788;
        subiterationidx = 809;
        didx = 205;
        trustradstagnated = ae_false;
        dotrace = ae_true;
        doprobingalways = ae_false;
        doprobingonfailure = ae_true;
        dodetailedtrace = ae_true;
        increasemeritmu = ae_false;
        meritfstagnated = ae_false;
        socperformed = ae_false;
        meritdecreasefailed = ae_false;
        stepperformed = ae_false;
        goodstep = ae_true;
        firstrescale = ae_false;
        infinitiesdetected = ae_true;
        failedtorecoverfrominfinities = ae_true;
        needsoc = ae_false;
        v = 346.0;
        vv = -722.0;
        mx = -413.0;
        deltamax = -461.0;
        multiplyby = 927.0;
        setscaleto = 201.0;
        prevtrustrad = 922.0;
        relativetargetdecrease = -154.0;
        f0 = 306.0;
        f0raw = -1011.0;
        f1 = 951.0;
        tol = -463.0;
        stepklagval = 88.0;
        stepknlagval = -861.0;
        expandedrad = -678.0;
        d2trustradratio = -731.0;
        sksk = -675.0;
        ykyk = -763.0;
        skyk = -233.0;
        sumc1 = -936.0;
        sumc1soc = -279.0;
        predictedchange = 94.0;
        predictedchangemodel = -812.0;
        predictedchangepenalty = 427.0;
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
    
    /*
     * Routine body
     */
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    nslack = n+2*(nec+nlec)+(nic+nlic);
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
    state->trustrad = nlcsqp_inittrustrad;
    state->trustradgrowth = nlcsqp_deftrustradgrowth;
    for(i=0; i<=nlec+nlic; i++)
    {
        state->fscales.ptr.p_double[i] = 1.0;
        state->tracegamma.ptr.p_double[i] = 0.0;
    }
    ae_obj_array_clear(&state->nonmonotonicmem);
    state->nonmonotoniccnt = 0;
    
    /*
     * Evaluate function vector and Jacobian at X0, send first location report.
     * Compute initial violation of constraints.
     */
    vfjallocdense(n, 1+nlec+nlic, &state->stepk, _state);
    vfjallocdense(n, 1+nlec+nlic, &state->cand, _state);
    vfjallocdense(n, 1+nlec+nlic, &state->corr, _state);
    vfjallocdense(n, 1+nlec+nlic, &state->probe, _state);
    vfjallocdense(n, 1+nlec+nlic, &state->currentlinearization, _state);
    rcopyv(n, &state->x0, &state->stepk.x, _state);
    nlcsqp_sqpsendx(state, &state->stepk.x, _state);
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
    if( !nlcsqp_sqpretrievefij(state, &state->stepk, _state) )
    {
        
        /*
         * Failed to retrieve function/Jacobian, infinities detected!
         */
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    nlcsqp_sqpsendx(state, &state->stepk.x, _state);
    state->f = state->stepk.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, &state->stepk.x, n, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlcviolation(&state->stepk.fi, &state->fscales, nlec, nlic, &state->repnlcerr, &state->repnlcidx, _state);
    
    /*
     * Trace output (if needed)
     */
    if( dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  SQP SOLVER STARTED                                                                            //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
    }
    
    /*
     * Perform outer (NLC) iterations
     */
    firstrescale = ae_true;
    infinitiesdetected = ae_false;
    failedtorecoverfrominfinities = ae_false;
    relativetargetdecrease = 1.0E50;
    state->meritmu = (double)(500);
    if( state->usedensebfgs )
    {
        hessianinitbfgs(&state->hess, n, state->bfgsresetfreq, (double)10*ae_maxreal(critgetepsx(&state->criteria, _state), ae_sqrt(ae_machineepsilon, _state), _state), _state);
    }
    else
    {
        hessianinitlowrank(&state->hess, n, ae_minint(n, nlcsqp_xbfgsmemlen, _state), (double)10*ae_maxreal(critgetepsx(&state->criteria, _state), ae_sqrt(ae_machineepsilon, _state), _state), nlcsqp_xbfgsmaxhess, _state);
    }
    nlcsqp_initqpsubsolver(state, &state->subsolver, _state);
lbl_6:
    if( ae_false )
    {
        goto lbl_7;
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
        goto lbl_7;
    }
    if( ae_fp_less_eq(state->trustrad,critgetepsx(&state->criteria, _state)) )
    {
        state->repterminationtype = 2;
        if( dotrace )
        {
            ae_trace("> stopping condition met: trust radius is smaller than %0.3e\n",
                (double)(critgetepsx(&state->criteria, _state)));
        }
        goto lbl_7;
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
        goto lbl_7;
    }
    if( critgetmaxits(&state->criteria, _state)>0&&state->repiterationscount>=critgetmaxits(&state->criteria, _state) )
    {
        state->repterminationtype = 5;
        if( dotrace )
        {
            ae_trace("> stopping condition met: %0d iterations performed\n",
                (int)(state->repiterationscount));
        }
        goto lbl_7;
    }
    if( state->fstagnationcnt>=nlcsqp_fstagnationlimit )
    {
        state->repterminationtype = 7;
        if( dotrace )
        {
            ae_trace("> stopping criteria are too stringent: F stagnated for %0d its, stopping\n",
                (int)(state->fstagnationcnt));
        }
        goto lbl_7;
    }
    for(i=0; i<=nlec+nlic; i++)
    {
        
        /*
         * Determine (a) multiplicative coefficient applied to function value
         * and Jacobian row, and (b) new value of the function scale.
         */
        mx = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            mx = ae_maxreal(mx, ae_fabs(state->stepk.jac.ptr.pp_double[i][j], _state), _state);
        }
        multiplyby = 1.0;
        setscaleto = state->fscales.ptr.p_double[i];
        if( ae_fp_greater_eq(mx,nlcsqp_sqpbigscale) )
        {
            v = mx;
            if( !firstrescale )
            {
                v = ae_minreal(v, nlcsqp_sqpmaxrescale, _state);
            }
            multiplyby = (double)1/v;
            setscaleto = state->fscales.ptr.p_double[i]*v;
        }
        if( ae_fp_less_eq(mx,nlcsqp_sqpsmallscale)&&ae_fp_greater(state->fscales.ptr.p_double[i],1.0) )
        {
            v = mx;
            if( !firstrescale )
            {
                v = ae_maxreal(v, nlcsqp_sqpminrescale, _state);
            }
            if( ae_fp_greater(state->fscales.ptr.p_double[i]*v,(double)(1)) )
            {
                multiplyby = (double)1/v;
                setscaleto = state->fscales.ptr.p_double[i]*v;
            }
            else
            {
                multiplyby = state->fscales.ptr.p_double[i];
                setscaleto = 1.0;
            }
        }
        if( ae_fp_neq(multiplyby,1.0) )
        {
            
            /*
             * Function #I needs renormalization:
             * * update function vector element and Jacobian matrix row
             * * update FScales[] and TraceGamma[] arrays
             */
            state->stepk.fi.ptr.p_double[i] = state->stepk.fi.ptr.p_double[i]*multiplyby;
            for(j=0; j<=n-1; j++)
            {
                state->stepk.jac.ptr.pp_double[i][j] = state->stepk.jac.ptr.pp_double[i][j]*multiplyby;
            }
            state->fscales.ptr.p_double[i] = setscaleto;
            state->tracegamma.ptr.p_double[i] = state->tracegamma.ptr.p_double[i]*multiplyby;
            nlcsqp_nonmonotonicmultiplyby(state, i, multiplyby, _state);
        }
    }
    firstrescale = ae_false;
    
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
        ae_trace("meritMu       = %0.3e    (penalty for violation of constraints)\n",
            (double)(state->meritmu));
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
    rsetv(nslack, 0.0, &state->d0, _state);
    rsetv(nslack, 0.0, &state->d1, _state);
    if( dotrace )
    {
        ae_trace("\n--- quadratic step ---------------------------------------------------------------------------------\n");
    }
    
    /*
     * Default values of the flag variables
     */
    increasemeritmu = ae_false;
    meritfstagnated = ae_false;
    meritdecreasefailed = ae_false;
    stepperformed = ae_true;
    goodstep = ae_true;
    relativetargetdecrease = 1.0E50;
    
    /*
     * Determine step direction using initial quadratic model.
     */
    socperformed = ae_false;
    if( !nlcsqp_qpsubproblemsolve(state, &state->subsolver, &state->stepk.x, &state->stepk.fi, &state->stepk.jac, &state->hess, 1.0, 1.0, &state->d0, &state->lagbcmult, &state->lagxcmult, dotrace, &j, &predictedchangemodel, &predictedchangepenalty, &sumc1, &d2trustradratio, _state) )
    {
        if( dotrace )
        {
            ae_trace("> [WARNING] QP subproblem failed with TerminationType=%0d\n> decreasing trust radius\n",
                (int)(j));
        }
        state->trustrad = 0.1*state->trustrad;
        inc(&state->repiterationscount, _state);
        goto lbl_6;
    }
    if( dotrace )
    {
        ae_trace("> QP subproblem solved with TerminationType=%0d, max|lagBoxMult|=%0.2e, max|lagNonBoxMult|=%0.2e\n",
            (int)(j),
            (double)(rmaxabsv(n, &state->lagbcmult, _state)),
            (double)(rmaxabsv(nec+nic+nlec+nlic, &state->lagxcmult, _state)));
    }
    rcopyv(nslack, &state->d0, &state->dtrial, _state);
    vfjcopy(&state->stepk, &state->currentlinearization, _state);
    
    /*
     * Perform merit function line search.
     *
     * First, we try unit step. If it does not decrease merit function,
     * a second-order correction is tried (helps to combat Maratos effect).
     */
    rallocv(n, &state->tmplaggrad, _state);
    nlcsqp_lagrangianfg(state, &state->stepk.x, state->trustrad, &state->stepk.fi, &state->stepk.jac, &state->lagbcmult, &state->lagxcmult, ae_true, &stepklagval, &state->tmplaggrad, _state);
    f0raw = nlcsqp_meritfunction(state, &state->stepk, state->meritmu, _state);
    f0 = nlcsqp_nonmonotonicadjustment(state, f0raw, _state);
    rcopyv(n, &state->stepk.x, &state->cand.x, _state);
    raddv(n, 1.0, &state->dtrial, &state->cand.x, _state);
    nlcsqp_sqpsendx(state, &state->cand.x, _state);
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
    if( !nlcsqp_sqpretrievefij(state, &state->cand, _state) )
    {
        
        /*
         * Failed to retrieve func/Jac, infinities detected
         */
        if( dotrace )
        {
            ae_trace("[WARNING] infinities in target/constraints are detected, forcing trust radius decrease and restarting iteration\n");
        }
        state->trustrad = state->trustrad*nlcsqp_maxtrustraddecay;
        trustradresetmomentum(&state->trustradgrowth, nlcsqp_deftrustradgrowth, _state);
        infinitiesdetected = ae_true;
        failedtorecoverfrominfinities = ae_true;
        inc(&state->repiterationscount, _state);
        goto lbl_6;
    }
    f1 = nlcsqp_meritfunction(state, &state->cand, state->meritmu, _state);
    predictedchange = predictedchangemodel+state->meritmu*predictedchangepenalty;
    goodstep = ae_fp_less(f1-f0raw,nlcsqp_sufficientdecreasesigma*predictedchange);
    rallocv(n, &state->tmpcandlaggrad, _state);
    nlcsqp_lagrangianfg(state, &state->cand.x, state->trustrad, &state->cand.fi, &state->cand.jac, &state->lagbcmult, &state->lagxcmult, ae_true, &stepknlagval, &state->tmpcandlaggrad, _state);
    hessianupdatev2(&state->hess, &state->stepk.x, &state->tmplaggrad, &state->cand.x, &state->tmpcandlaggrad, 2, ae_false, dotrace, 1, _state);
    if( dotrace )
    {
        ae_trace("> proposed step with relative len %0.6f, analyzing change in the merit function: predicted=%0.2e, actual=%0.2e (ignoring nonmonotonic adjustment), ratio=%5.2f\n",
            (double)(d2trustradratio),
            (double)(predictedchange),
            (double)(f1-f0raw),
            (double)((f1-f0raw)/predictedchange));
    }
    needsoc = ae_false;
    if( ae_fp_greater_eq(predictedchange,(double)(0))||ae_fp_greater_eq(f1-f0,nlcsqp_sufficientdecreasesigma*predictedchange) )
    {
        needsoc = ae_true;
        if( dotrace )
        {
            ae_trace("> step without correction does not provide sufficient decrease in the merit function, preparing second-order correction\n");
        }
    }
    if( !needsoc )
    {
        goto lbl_8;
    }
    
    /*
     * Full step increases merit function. Let's compute second order
     * correction to the constraint model and recompute trial step D:
     * * use original model of the target
     * * extrapolate model of nonlinear constraints at StepKX+D back to origin
     *
     */
    socperformed = ae_true;
    rcopyv(n, &state->stepk.x, &state->corr.x, _state);
    state->corr.fi.ptr.p_double[0] = state->stepk.fi.ptr.p_double[0];
    for(j=0; j<=n-1; j++)
    {
        state->corr.jac.ptr.pp_double[0][j] = state->stepk.jac.ptr.pp_double[0][j];
    }
    for(i=1; i<=nlec+nlic; i++)
    {
        v = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            v = v+state->d0.ptr.p_double[j]*state->stepk.jac.ptr.pp_double[i][j];
            state->corr.jac.ptr.pp_double[i][j] = state->stepk.jac.ptr.pp_double[i][j];
        }
        state->corr.fi.ptr.p_double[i] = state->cand.fi.ptr.p_double[i]-v;
    }
    if( !nlcsqp_qpsubproblemsolve(state, &state->subsolver, &state->corr.x, &state->corr.fi, &state->corr.jac, &state->hess, 1.0, 1.0, &state->d1, &state->dummylagbcmult, &state->dummylagxcmult, dotrace, &j, &predictedchangemodel, &predictedchangepenalty, &sumc1soc, &d2trustradratio, _state) )
    {
        if( dotrace )
        {
            ae_trace("> [WARNING] second-order QP subproblem failed\n> decreasing trust radius\n");
        }
        state->trustrad = 0.1*state->trustrad;
        inc(&state->repiterationscount, _state);
        goto lbl_6;
    }
    if( dotrace )
    {
        ae_trace("> QP subproblem solved with TerminationType=%0d, max|lagBoxMult|=%0.2e, max|lagNonBoxMult|=%0.2e\n",
            (int)(j),
            (double)(rmaxabsv(n, &state->dummylagbcmult, _state)),
            (double)(rmaxabsv(nec+nic+nlec+nlic, &state->dummylagxcmult, _state)));
    }
    rcopyv(n, &state->d1, &state->dtrial, _state);
    vfjcopy(&state->corr, &state->currentlinearization, _state);
    
    /*
     * Perform line search, we again try full step (maybe it will work after SOC)
     */
    smoothnessmonitorstartlinesearch(smonitor, &state->stepk.x, &state->stepk.fi, &state->stepk.jac, state->repiterationscount, -1, _state);
    rcopyv(n, &state->stepk.x, &state->cand.x, _state);
    raddv(n, 1.0, &state->dtrial, &state->cand.x, _state);
    nlcsqp_sqpsendx(state, &state->cand.x, _state);
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
    if( !nlcsqp_sqpretrievefij(state, &state->cand, _state) )
    {
        
        /*
         * Failed to retrieve func/Jac, infinities detected
         */
        if( dotrace )
        {
            ae_trace("[WARNING] infinities in target/constraints are detected, forcing trust radius decrease and restarting iteration\n");
        }
        state->trustrad = state->trustrad*nlcsqp_maxtrustraddecay;
        trustradresetmomentum(&state->trustradgrowth, nlcsqp_deftrustradgrowth, _state);
        inc(&state->repiterationscount, _state);
        infinitiesdetected = ae_true;
        failedtorecoverfrominfinities = ae_true;
        goto lbl_6;
    }
    smoothnessmonitorenqueuepoint(smonitor, &state->dtrial, 1.0, &state->cand.x, &state->cand.fi, &state->cand.jac, _state);
    f1 = nlcsqp_meritfunction(state, &state->cand, state->meritmu, _state);
    predictedchange = predictedchangemodel+state->meritmu*predictedchangepenalty;
    goodstep = ae_fp_less(f1-f0raw,nlcsqp_sufficientdecreasesigma*predictedchange);
    rallocv(n, &state->tmpcandlaggrad, _state);
    nlcsqp_lagrangianfg(state, &state->cand.x, state->trustrad, &state->cand.fi, &state->cand.jac, &state->lagbcmult, &state->lagxcmult, ae_true, &stepknlagval, &state->tmpcandlaggrad, _state);
    hessianpoplatestifpossible(&state->hess, _state);
    hessianupdatev2(&state->hess, &state->stepk.x, &state->tmplaggrad, &state->cand.x, &state->tmpcandlaggrad, 2, ae_true, dotrace, 1, _state);
    if( dotrace )
    {
        ae_trace("> proposed step with relative len %0.6f, analyzing change in the merit function: predicted=%0.2e, actual=%0.2e (ignoring nonmonotonic adjustment), ratio=%5.2f\n",
            (double)(d2trustradratio),
            (double)(predictedchange),
            (double)(f1-f0raw),
            (double)((f1-f0raw)/predictedchange));
    }
    if( ae_fp_greater_eq(predictedchange,(double)(0))||ae_fp_greater_eq(f1-f0,nlcsqp_sufficientdecreasesigma*predictedchange) )
    {
        
        /*
         * Mark step as failed
         */
        stepperformed = ae_false;
        goodstep = ae_false;
        meritdecreasefailed = ae_true;
        f1 = f0;
    }
    smoothnessmonitorfinalizelinesearch(smonitor, _state);
lbl_8:
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
        goto lbl_7;
    }
    
    /*
     * Analyze merit F for stagnation
     */
    meritfstagnated = stepperformed&&ae_fp_less_eq(ae_fabs(f1-f0, _state),nlcsqp_stagnationepsf*ae_fabs(f0, _state));
    v = ae_fabs(f1-f0, _state)/rmaxabs3(f0, f1, (double)(1), _state);
    vv = ae_fabs(state->cand.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0]-state->stepk.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0], _state)/rmaxabs3(state->cand.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0], state->stepk.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0], (double)(1), _state);
    relativetargetdecrease = rcase2(stepperformed, ae_maxreal(v, vv, _state), 1.0E50, _state);
    
    /*
     * Analyze linearized model - did we enforce zero violations of the constraint linearizations?
     * If we did not, we may need to increase MeritMu penalty coefficients.
     */
    if( nlcsqp_penaltiesneedincrease(state, &state->currentlinearization, &state->dtrial, predictedchangemodel, predictedchangepenalty, dotrace, _state) )
    {
        stepperformed = ae_false;
        goodstep = ae_false;
        increasemeritmu = ae_true;
    }
    
    /*
     * Trace
     */
    if( !dotrace )
    {
        goto lbl_10;
    }
    
    /*
     * Compute Lagrangian
     */
    rallocv(n, &state->tmplaggrad, _state);
    rallocv(n, &state->tmpcandlaggrad, _state);
    nlcsqp_lagrangianfg(state, &state->stepk.x, state->trustrad, &state->stepk.fi, &state->stepk.jac, &state->lagbcmult, &state->lagxcmult, ae_true, &stepklagval, &state->tmplaggrad, _state);
    nlcsqp_lagrangianfg(state, &state->cand.x, state->trustrad, &state->cand.fi, &state->cand.jac, &state->lagbcmult, &state->lagxcmult, ae_true, &stepknlagval, &state->tmpcandlaggrad, _state);
    
    /*
     * Update debug curvature information. Let
     *
     *     Sk = X(k+1)-X(k), Yk = G(k+1)-G(k)
     *
     * for a function Fi and store maximum over curvatures
     *
     *     gamma = (Yk,Yk)/|(Sk,Yk)|
     *
     * to TraceGamma[] array. Set MaxNewGamma to maximum of new values, set GammaIncreased
     * flag if at least one of TraceGamma[] entries was increased.
     */
    sksk = (double)(0);
    for(j=0; j<=n-1; j++)
    {
        v = state->cand.x.ptr.p_double[j]-state->stepk.x.ptr.p_double[j];
        sksk = sksk+v*v;
    }
    if( ae_fp_greater(sksk,(double)(0)) )
    {
        for(i=0; i<=nlec+nlic; i++)
        {
            ykyk = (double)(0);
            skyk = (double)(0);
            for(j=0; j<=n-1; j++)
            {
                v = state->cand.x.ptr.p_double[j]-state->stepk.x.ptr.p_double[j];
                vv = state->cand.jac.ptr.pp_double[i][j]-state->stepk.jac.ptr.pp_double[i][j];
                skyk = skyk+v*vv;
                ykyk = ykyk+vv*vv;
            }
            v = skyk/(sksk+ae_machineepsilon*ae_machineepsilon);
            state->tracegamma.ptr.p_double[i] = ae_maxreal(state->tracegamma.ptr.p_double[i], v, _state);
        }
    }
    
    /*
     * Perform agressive probing of the search direction - additional function evaluations
     * which help us to determine possible discontinuity and nonsmoothness of the problem
     */
    if( !(doprobingalways||(doprobingonfailure&&meritdecreasefailed)) )
    {
        goto lbl_12;
    }
    didx = 0;
lbl_14:
    if( didx>1 )
    {
        goto lbl_16;
    }
    if( didx==1&&!socperformed )
    {
        goto lbl_16;
    }
    if( didx==0 )
    {
        smoothnessmonitorstartlagrangianprobing(smonitor, &state->stepk.x, &state->d0, 1.0, state->repiterationscount, -1, _state);
    }
    else
    {
        smoothnessmonitorstartlagrangianprobing(smonitor, &state->stepk.x, &state->d1, 1.0, state->repiterationscount, -1, _state);
    }
lbl_17:
    if( !smoothnessmonitorprobelagrangian(smonitor, _state) )
    {
        goto lbl_18;
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
    nlcsqp_sqpsendx(state, &state->probe.x, _state);
    state->needfij = ae_true;
    if( dotrace )
    {
        stimerstart(&state->timercallback, _state);
    }
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    if( dotrace )
    {
        stimerstop(&state->timercallback, _state);
    }
    state->needfij = ae_false;
    if( !nlcsqp_sqpretrievefij(state, &state->probe, _state) )
    {
        goto lbl_18;
    }
    rcopyallocv(1+nlec+nlic, &state->probe.fi, &smonitor->lagprobfi, _state);
    rcopyallocm(1+nlec+nlic, n, &state->probe.jac, &smonitor->lagprobj, _state);
    smonitor->lagprobrawlag = nlcsqp_rawlagrangian(state, &state->probe.x, &state->probe.fi, &state->lagbcmult, &state->lagxcmult, state->meritmu, _state);
    goto lbl_17;
lbl_18:
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |                 probing search direction                 |\n");
    if( didx==0 )
    {
        ae_trace("*** |          suggested by first-order QP subproblem          |\n");
    }
    if( didx==1 )
    {
        ae_trace("*** |          suggested by second-order QP subproblem         |\n");
    }
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |  Step  | Lagrangian (unaugmentd)|    Target  function    |\n");
    ae_trace("*** |along  D|     must be smooth     |     must be smooth     |\n");
    ae_trace("*** |        | function   |    slope  | function   |    slope  |\n");
    smoothnessmonitortracelagrangianprobingresults(smonitor, _state);
    didx = didx+1;
    goto lbl_14;
lbl_16:
lbl_12:
    
    /*
     * Output other information
     */
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(state->dtrial.ptr.p_double[i], _state)/nlcsqp_gettrustregionk(&state->stepk.x, i, state->trustrad, _state), _state);
    }
    if( stepperformed&&goodstep )
    {
        ae_trace("> sufficient decrease step was performed\n");
    }
    else
    {
        if( stepperformed )
        {
            ae_trace("> nonmonotonic step was performed\n");
        }
        else
        {
            ae_trace("> no step was performed\n");
        }
    }
    ae_trace("max(|Di|)/TrustRad = %0.6f\n",
        (double)(mx));
    if( dodetailedtrace )
    {
        ae_trace("LagBoxMlt   = ");
        tracevectorautoprec(&state->lagbcmult, 0, n, _state);
        ae_trace("\n");
        ae_trace("LagNonBoxMlt= ");
        tracevectorautoprec(&state->lagxcmult, 0, nec+nic+nlec+nlic, _state);
        ae_trace("\n");
    }
    if( dodetailedtrace )
    {
        ae_trace("X0 (scaled) = ");
        tracevectorautoprec(&state->stepk.x, 0, n, _state);
        ae_trace("\n");
        ae_trace("D  (scaled) = ");
        tracevectorautoprec(&state->dtrial, 0, n, _state);
        ae_trace("\n");
        ae_trace("X1 (scaled) = ");
        tracevectorautoprec(&state->cand.x, 0, n, _state);
        ae_trace("\n");
        ae_trace("\n");
        ae_trace("grad F(X0)  = ");
        tracerowautoprec(&state->stepk.jac, 0, 0, n, _state);
        ae_trace("\n");
        ae_trace("grad F(X1)  = ");
        tracerowautoprec(&state->cand.jac, 0, 0, n, _state);
        ae_trace("\n");
        ae_trace("\n");
        ae_trace("grad L(X0)  = ");
        tracevectorautoprec(&state->tmplaggrad, 0, n, _state);
        ae_trace("\n");
        ae_trace("grad L(X1)  = ");
        tracevectorautoprec(&state->tmpcandlaggrad, 0, n, _state);
        ae_trace("\n");
    }
    ae_trace("targetF:        %17.9e -> %17.9e (delta=%11.3e)\n",
        (double)(state->fscales.ptr.p_double[0]*state->stepk.fi.ptr.p_double[0]),
        (double)(state->fscales.ptr.p_double[0]*state->cand.fi.ptr.p_double[0]),
        (double)(state->fscales.ptr.p_double[0]*(state->cand.fi.ptr.p_double[0]-state->stepk.fi.ptr.p_double[0])));
    ae_trace("scaled-meritF:  %17.9e -> %17.9e (delta=%11.3e)\n",
        (double)(f0),
        (double)(f1),
        (double)(f1-f0));
    ae_trace("scaled-targetF: %17.9e -> %17.9e (delta=%11.3e)\n",
        (double)(state->stepk.fi.ptr.p_double[0]),
        (double)(state->cand.fi.ptr.p_double[0]),
        (double)(state->cand.fi.ptr.p_double[0]-state->stepk.fi.ptr.p_double[0]));
    ae_trace("max|lag-grad|:  %17.9e -> %17.9e\n",
        (double)(rmaxabsv(n, &state->tmplaggrad, _state)),
        (double)(rmaxabsv(n, &state->tmpcandlaggrad, _state)));
    ae_trace("nrm2|lag-grad|: %17.9e -> %17.9e (ratio=%0.6f)\n",
        (double)(ae_sqrt(rdotv2(n, &state->tmplaggrad, _state), _state)),
        (double)(ae_sqrt(rdotv2(n, &state->tmpcandlaggrad, _state), _state)),
        (double)(ae_sqrt(rdotv2(n, &state->tmpcandlaggrad, _state), _state)/ae_sqrt(rdotv2(n, &state->tmplaggrad, _state), _state)));
    hessiangetdiagonal(&state->hess, &state->tmphdiag, _state);
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
lbl_10:
    
    /*
     * Move to new point
     */
    if( !stepperformed )
    {
        goto lbl_19;
    }
    
    /*
     * Save current point to the nonmonotonic memory buffer
     */
    nlcsqp_nonmonotonicsave(state, &state->cand, _state);
    
    /*
     * Update current state
     */
    vfjcopy(&state->cand, &state->stepk, _state);
    failedtorecoverfrominfinities = ae_false;
    
    /*
     * Report one more inner iteration
     */
    nlcsqp_sqpsendx(state, &state->stepk.x, _state);
    state->f = state->stepk.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->xupdated = ae_false;
    
    /*
     * Update constraint violations
     */
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, &state->stepk.x, n, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlcviolation(&state->stepk.fi, &state->fscales, nlec, nlic, &state->repnlcerr, &state->repnlcidx, _state);
lbl_19:
    
    /*
     * Update MeritMu
     */
    if( increasemeritmu )
    {
        state->meritmu = ae_minreal((double)2*state->meritmu, nlcsqp_maxmeritmu, _state);
    }
    
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
        deltamax = ae_maxreal(deltamax, ae_fabs(state->xprev.ptr.p_double[i]-state->stepk.x.ptr.p_double[i], _state)/nlcsqp_gettrustregionk(&state->xprev, i, state->trustrad, _state), _state);
    }
    inc(&state->trustradstagnationcnt, _state);
    if( ae_fp_less_eq(deltamax,nlcsqp_sqpdeltadecrease) )
    {
        state->trustrad = state->trustrad*ae_maxreal(deltamax/nlcsqp_sqpdeltadecrease, nlcsqp_maxtrustraddecay, _state);
    }
    if( ae_fp_greater_eq(deltamax,nlcsqp_sqpdeltaincrease)&&goodstep )
    {
        state->trustrad = state->trustrad*state->trustradgrowth;
        trustradincreasemomentum(&state->trustradgrowth, nlcsqp_momentumgrowth, nlcsqp_maxtrustradgrowth, _state);
    }
    else
    {
        trustradresetmomentum(&state->trustradgrowth, nlcsqp_deftrustradgrowth, _state);
    }
    if( ae_fp_less(state->trustrad,0.99*prevtrustrad)||ae_fp_greater(state->trustrad,1.01*prevtrustrad) )
    {
        state->trustradstagnationcnt = 0;
    }
    if( state->trustradstagnationcnt>=nlcsqp_trustradstagnationlimit )
    {
        state->trustrad = 0.5*state->trustrad;
        state->trustradstagnationcnt = 0;
        trustradstagnated = ae_true;
    }
    
    /*
     * Trace
     */
    if( dotrace )
    {
        ae_trace("\n--- outer iteration ends ---------------------------------------------------------------------------\n");
        ae_trace("deltaMax    = %0.3f (ratio of step length to trust radius)\n",
            (double)(deltamax));
        ae_trace("newTrustRad = %0.3e",
            (double)(state->trustrad));
        if( !trustradstagnated )
        {
            if( ae_fp_greater(state->trustrad,prevtrustrad) )
            {
                ae_trace(", trust radius increased");
            }
            if( ae_fp_less(state->trustrad,prevtrustrad) )
            {
                ae_trace(", trust radius decreased");
            }
        }
        else
        {
            ae_trace(", trust radius forcibly decreased due to stagnation for %0d iterations",
                (int)(nlcsqp_trustradstagnationlimit));
        }
        ae_trace("\n");
        if( increasemeritmu )
        {
            ae_trace("meritMu     = %0.3e (increasing)\n",
                (double)(state->meritmu));
        }
    }
    
    /*
     * Advance outer iteration counter
     */
    inc(&state->repiterationscount, _state);
    state->fstagnationcnt = icase2(meritfstagnated, state->fstagnationcnt+1, 0, _state);
    goto lbl_6;
lbl_7:
    
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
            ae_trace("> infinities were detected, but we failed to recover by decreasing trust radius; declaring failure with code -8\n");
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
    state->rstate.ia.ptr.p_int[1] = nslack;
    state->rstate.ia.ptr.p_int[2] = nec;
    state->rstate.ia.ptr.p_int[3] = nic;
    state->rstate.ia.ptr.p_int[4] = nlec;
    state->rstate.ia.ptr.p_int[5] = nlic;
    state->rstate.ia.ptr.p_int[6] = i;
    state->rstate.ia.ptr.p_int[7] = j;
    state->rstate.ia.ptr.p_int[8] = subiterationidx;
    state->rstate.ia.ptr.p_int[9] = didx;
    state->rstate.ba.ptr.p_bool[0] = trustradstagnated;
    state->rstate.ba.ptr.p_bool[1] = dotrace;
    state->rstate.ba.ptr.p_bool[2] = doprobingalways;
    state->rstate.ba.ptr.p_bool[3] = doprobingonfailure;
    state->rstate.ba.ptr.p_bool[4] = dodetailedtrace;
    state->rstate.ba.ptr.p_bool[5] = increasemeritmu;
    state->rstate.ba.ptr.p_bool[6] = meritfstagnated;
    state->rstate.ba.ptr.p_bool[7] = socperformed;
    state->rstate.ba.ptr.p_bool[8] = meritdecreasefailed;
    state->rstate.ba.ptr.p_bool[9] = stepperformed;
    state->rstate.ba.ptr.p_bool[10] = goodstep;
    state->rstate.ba.ptr.p_bool[11] = firstrescale;
    state->rstate.ba.ptr.p_bool[12] = infinitiesdetected;
    state->rstate.ba.ptr.p_bool[13] = failedtorecoverfrominfinities;
    state->rstate.ba.ptr.p_bool[14] = needsoc;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vv;
    state->rstate.ra.ptr.p_double[2] = mx;
    state->rstate.ra.ptr.p_double[3] = deltamax;
    state->rstate.ra.ptr.p_double[4] = multiplyby;
    state->rstate.ra.ptr.p_double[5] = setscaleto;
    state->rstate.ra.ptr.p_double[6] = prevtrustrad;
    state->rstate.ra.ptr.p_double[7] = relativetargetdecrease;
    state->rstate.ra.ptr.p_double[8] = f0;
    state->rstate.ra.ptr.p_double[9] = f0raw;
    state->rstate.ra.ptr.p_double[10] = f1;
    state->rstate.ra.ptr.p_double[11] = tol;
    state->rstate.ra.ptr.p_double[12] = stepklagval;
    state->rstate.ra.ptr.p_double[13] = stepknlagval;
    state->rstate.ra.ptr.p_double[14] = expandedrad;
    state->rstate.ra.ptr.p_double[15] = d2trustradratio;
    state->rstate.ra.ptr.p_double[16] = sksk;
    state->rstate.ra.ptr.p_double[17] = ykyk;
    state->rstate.ra.ptr.p_double[18] = skyk;
    state->rstate.ra.ptr.p_double[19] = sumc1;
    state->rstate.ra.ptr.p_double[20] = sumc1soc;
    state->rstate.ra.ptr.p_double[21] = predictedchange;
    state->rstate.ra.ptr.p_double[22] = predictedchangemodel;
    state->rstate.ra.ptr.p_double[23] = predictedchangepenalty;
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
static void nlcsqp_initqpsubsolver(const minsqpstate* sstate,
     minsqpsubsolver* subsolver,
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

Additionally returns SumC1 - a sum of absolute errors AT THE BEGINNING of
the step (this quantity is used to choose appropriate penalty parameter;
it corresponds to |Ck|_1 at Nocedal and Wright, 'Numerical optimization' 2006,
chapter 18, formula 18.33).

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
static ae_bool nlcsqp_qpsubproblemsolve(minsqpstate* state,
     minsqpsubsolver* subsolver,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     xbfgshessian* hess,
     double alphag,
     double alphah,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagbcmult,
     /* Real    */ ae_vector* lagxcmult,
     ae_bool dotrace,
     ae_int_t* terminationtype,
     double* predictedchangemodel,
     double* predictedchangepenalty,
     double* sumc1,
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
    ae_bool result;

    *terminationtype = 0;
    *predictedchangemodel = 0.0;
    *predictedchangepenalty = 0.0;
    *sumc1 = 0.0;
    *d2trustradratio = 0.0;

    ae_assert(ae_fp_eq(alphag,(double)(0))||ae_fp_eq(alphag,(double)(1)), "QPSubproblemSolve: AlphaG is neither 0 nor 1", _state);
    ae_assert(ae_fp_eq(alphah,(double)(0))||ae_fp_eq(alphah,(double)(1)), "QPSubproblemSolve: AlphaH is neither 0 nor 1", _state);
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    nslack = n+2*(nec+nlec)+(nic+nlic);
    lccnt = nec+nic+nlec+nlic;
    
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
    rvectorsetlengthatleast(&subsolver->d0, nslack, _state);
    rallocv(lccnt, &subsolver->rescalelag, _state);
    
    /*
     * Prepare default solution: all zeros
     */
    result = ae_true;
    *terminationtype = 0;
    *sumc1 = (double)(0);
    *predictedchangemodel = (double)(0);
    *predictedchangepenalty = (double)(0);
    *d2trustradratio = 0.0;
    for(i=0; i<=nslack-1; i++)
    {
        d->ptr.p_double[i] = 0.0;
        subsolver->d0.ptr.p_double[i] = (double)(0);
    }
    rsetv(n, 0.0, lagbcmult, _state);
    rsetv(lccnt, 0.0, lagxcmult, _state);
    
    /*
     * Linear term B
     *
     * NOTE: elements [N,NSlack) are equal to MeritMu + perturbation to improve numeric properties of QP problem
     */
    for(i=0; i<=n-1; i++)
    {
        subsolver->curb.ptr.p_double[i] = alphag*jac->ptr.pp_double[0][i];
    }
    for(i=n; i<=nslack-1; i++)
    {
        subsolver->curb.ptr.p_double[i] = state->meritmu;
    }
    
    /*
     * Trust radius constraints for primary variables
     */
    bsetallocv(n, ae_false, &subsolver->retainnegativebclm, _state);
    bsetallocv(n, ae_false, &subsolver->retainpositivebclm, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->curbndl.ptr.p_double[i] = -nlcsqp_gettrustregionk(x, i, state->trustrad, _state);
        subsolver->curbndu.ptr.p_double[i] = nlcsqp_gettrustregionk(x, i, state->trustrad, _state);
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
            *sumc1 = *sumc1+ae_fabs(v, _state);
            subsolver->cural.ptr.p_double[i] = -v;
            subsolver->curau.ptr.p_double[i] = -v;
            subsolver->curbndl.ptr.p_double[offsslackec+2*i+0] = (double)(0);
            subsolver->curbndl.ptr.p_double[offsslackec+2*i+1] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslackec+2*i+0] = ae_fabs(v, _state);
            subsolver->curbndu.ptr.p_double[offsslackec+2*i+1] = ae_fabs(v, _state);
            if( ae_fp_greater_eq(v,(double)(0)) )
            {
                subsolver->d0.ptr.p_double[offsslackec+2*i+0] = ae_fabs(v, _state);
                subsolver->d0.ptr.p_double[offsslackec+2*i+1] = (double)(0);
            }
            else
            {
                subsolver->d0.ptr.p_double[offsslackec+2*i+0] = (double)(0);
                subsolver->d0.ptr.p_double[offsslackec+2*i+1] = ae_fabs(v, _state);
            }
        }
        else
        {
            *sumc1 = *sumc1+ae_maxreal(v, 0.0, _state);
            subsolver->cural.ptr.p_double[i] = _state->v_neginf;
            subsolver->curau.ptr.p_double[i] = -v;
            subsolver->curbndl.ptr.p_double[offsslackic+(i-nec)] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslackic+(i-nec)] = ae_maxreal(v, (double)(0), _state);
            subsolver->d0.ptr.p_double[offsslackic+(i-nec)] = ae_maxreal(v, (double)(0), _state);
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
            subsolver->d0.ptr.p_double[offsslacknlic+(i-nlec)] = 0.0;
            subsolver->rescalelag.ptr.p_double[nec+nic+i] = 0.0;
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
            *sumc1 = *sumc1+ae_fabs(fi->ptr.p_double[1+i], _state);
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curbndl.ptr.p_double[offsslacknlec+2*i+0] = (double)(0);
            subsolver->curbndl.ptr.p_double[offsslacknlec+2*i+1] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslacknlec+2*i+0] = ae_fabs(v, _state);
            subsolver->curbndu.ptr.p_double[offsslacknlec+2*i+1] = ae_fabs(v, _state);
            if( ae_fp_greater_eq(v,(double)(0)) )
            {
                subsolver->d0.ptr.p_double[offsslacknlec+2*i+0] = ae_fabs(v, _state);
                subsolver->d0.ptr.p_double[offsslacknlec+2*i+1] = (double)(0);
            }
            else
            {
                subsolver->d0.ptr.p_double[offsslacknlec+2*i+0] = (double)(0);
                subsolver->d0.ptr.p_double[offsslacknlec+2*i+1] = ae_fabs(v, _state);
            }
        }
        else
        {
            
            /*
             * Inequality constraint
             */
            *sumc1 = *sumc1+ae_maxreal(fi->ptr.p_double[1+i], 0.0, _state);
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = _state->v_neginf;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curbndl.ptr.p_double[offsslacknlic+(i-nlec)] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslacknlic+(i-nlec)] = ae_maxreal(v, (double)(0), _state);
            subsolver->d0.ptr.p_double[offsslacknlic+(i-nlec)] = ae_maxreal(v, (double)(0), _state);
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
        subsolver->tmps.ptr.p_double[i] = nlcsqp_gettrustregionk(x, i, state->trustrad, _state);
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
        hessiangetmatrix(hess, ae_true, &subsolver->denseh, _state);
        for(i=0; i<=n-1; i++)
        {
            rmulr(n, alphah, &subsolver->denseh, i, _state);
        }
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
        hessiangetlowrankstabilized(hess, &subsolver->tmpdiag, &subsolver->tmpcorrc, &subsolver->hesscorrd, &subsolver->hessrank, _state);
        ae_assert(ae_fp_eq(alphah,(double)(0))||ae_fp_eq(alphah,(double)(1)), "QPSubproblemSolve: AlphaH is neither 0 nor 1", _state);
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
            subsolver->hesssparsediag.vals.ptr.p_double[i] = 0.0;
            subsolver->hesssparsediag.ridx.ptr.p_int[i+1] = i+1;
        }
        raddv(n, alphah, &subsolver->tmpdiag, &subsolver->hesssparsediag.vals, _state);
        sparsecreatecrsinplace(&subsolver->hesssparsediag, _state);
        if( subsolver->hessrank>0 )
        {
            rsetallocm(subsolver->hessrank, nslack, 0.0, &subsolver->hesscorrc, _state);
            if( ae_fp_neq(alphah,(double)(0)) )
            {
                rcopym(subsolver->hessrank, n, &subsolver->tmpcorrc, &subsolver->hesscorrc, _state);
            }
        }
        ipm2init(&subsolver->ipm2, &subsolver->tmps, &subsolver->tmporigin, nslack, &subsolver->densedummy, &subsolver->hesssparsediag, 1, ae_true, &subsolver->hesscorrc, &subsolver->hesscorrd, subsolver->hessrank, &subsolver->curb, &subsolver->curbndl, &subsolver->curbndu, &subsolver->sparseefflc, subsolver->sparseefflc.m, &subsolver->densedummy, 0, &subsolver->cural, &subsolver->curau, _state);
        ipm2optimize(&subsolver->ipm2, ae_false, &subsolver->tmp0, &subsolver->tmp1, &subsolver->tmp2, terminationtype, _state);
    }
    if( dotrace )
    {
        stimerstop(&state->timerqp, _state);
    }
    
    /*
     * Unpack results
     */
    rcopyv(nslack, &subsolver->tmp0, d, _state);
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
    *predictedchangemodel = rdotv(n, d, &subsolver->curb, _state)+0.5*hessianvmv(hess, d, _state);
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
        *d2trustradratio = ae_maxreal(*d2trustradratio, ae_fabs(d->ptr.p_double[i]/nlcsqp_gettrustregionk(x, i, state->trustrad, _state), _state), _state);
    }
    return result;
}


/*************************************************************************
Copies X to State.X
*************************************************************************/
static void nlcsqp_sqpsendx(minsqpstate* state,
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
static ae_bool nlcsqp_sqpretrievefij(const minsqpstate* state,
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
static void nlcsqp_lagrangianfg(minsqpstate* state,
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
            *f = *f+nlcsqp_augmentationfactor*(x->ptr.p_double[i]-state->scaledbndl.ptr.p_double[i])*(x->ptr.p_double[i]-state->scaledbndl.ptr.p_double[i]);
            g->ptr.p_double[i] = g->ptr.p_double[i]+(double)2*nlcsqp_augmentationfactor*(x->ptr.p_double[i]-state->scaledbndl.ptr.p_double[i]);
        }
        if( state->hasbndu.ptr.p_bool[i]&&ae_fp_greater(x->ptr.p_double[i],state->scaledbndu.ptr.p_double[i]) )
        {
            *f = *f+nlcsqp_augmentationfactor*(x->ptr.p_double[i]-state->scaledbndu.ptr.p_double[i])*(x->ptr.p_double[i]-state->scaledbndu.ptr.p_double[i]);
            g->ptr.p_double[i] = g->ptr.p_double[i]+(double)2*nlcsqp_augmentationfactor*(x->ptr.p_double[i]-state->scaledbndu.ptr.p_double[i]);
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
            *f = *f+0.5*nlcsqp_augmentationfactor*vact*vact;
            state->sclagtmp1.ptr.p_double[i] = state->sclagtmp1.ptr.p_double[i]+nlcsqp_augmentationfactor*vact;
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
        *f = *f+0.5*nlcsqp_augmentationfactor*vact*vact;
        state->sclagtmp1.ptr.p_double[i] = state->sclagtmp1.ptr.p_double[i]+nlcsqp_augmentationfactor*vact;
    }
    rmatrixgemv(n, nlec+nlic, 1.0, j, 1, 0, 1, &state->sclagtmp1, 0, 1.0, g, 0, _state);
}


/*************************************************************************
This function calculates L1-penalized merit function
*************************************************************************/
static double nlcsqp_meritfunction(minsqpstate* state,
     const varsfuncjac* vfj,
     double meritmu,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    double v;
    double result;


    ae_assert(vfj->isdense, "SQP: integrity check 1057 failed", _state);
    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    
    /*
     * Merit function and Lagrangian: primary term
     */
    result = vfj->fi.ptr.p_double[0];
    
    /*
     * Merit function: augmentation and penalty for linear constraints
     */
    rvectorsetlengthatleast(&state->mftmp0, nec+nic, _state);
    rmatrixgemv(nec+nic, n, 1.0, &state->scaledcleic, 0, 0, 0, &vfj->x, 0, 0.0, &state->mftmp0, 0, _state);
    for(i=0; i<=nec+nic-1; i++)
    {
        v = state->mftmp0.ptr.p_double[i]-state->scaledcleic.ptr.pp_double[i][n];
        if( i<nec )
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            result = result+meritmu*ae_fabs(v, _state);
        }
        else
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            result = result+meritmu*ae_maxreal(v, (double)(0), _state);
        }
    }
    
    /*
     * Merit function: augmentation and penalty for nonlinear constraints
     */
    for(i=0; i<=nlec+nlic-1; i++)
    {
        v = vfj->fi.ptr.p_double[1+i];
        if( i<nlec )
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            result = result+meritmu*ae_fabs(v, _state);
        }
        else
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            result = result+meritmu*ae_maxreal(v, (double)(0), _state);
        }
    }
    return result;
}


/*************************************************************************
This function calculates raw (unaugmented and smooth) Lagrangian
*************************************************************************/
static double nlcsqp_rawlagrangian(minsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     ae_state *_state)
{
    double tmp0;
    double tmp1;
    double result;


    nlcsqp_meritfunctionandrawlagrangian(state, x, fi, lagbcmult, lagxcmult, meritmu, &tmp0, &tmp1, _state);
    result = tmp1;
    return result;
}


/*************************************************************************
This function calculates L1-penalized merit function and raw  (smooth  and
un-augmented) Lagrangian
*************************************************************************/
static void nlcsqp_meritfunctionandrawlagrangian(minsqpstate* state,
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
This function analyzes constraint linearizations at the proposed candidate
point. Depending on outcome, proposes to increase MeritMu or not.

If no update is necessary (the best outcome), False will be returned.

  -- ALGLIB --
     Copyright 13.03.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcsqp_penaltiesneedincrease(minsqpstate* state,
     const varsfuncjac* currentlinearization,
     /* Real    */ const ae_vector* dtrial,
     double predictedchangemodel,
     double predictedchangepenalty,
     ae_bool dotrace,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t nslack;
    ae_int_t i;
    ae_int_t j;
    ae_int_t tt;
    double tol;
    double tol2;
    double constrval;
    double allowederr;
    ae_bool failedtoenforce;
    ae_bool failedtoenforceinf;
    ae_bool infeasiblewithinbox;
    double dummy0;
    double dummy1;
    double dummy2;
    double dummy3;
    double penaltyatx;
    double penaltyatd;
    double penaltyatdinf;
    double meritdecrease;
    double penaltydecrease;
    double mxd;
    double expandedrad;
    double cx;
    double cxd;
    double sumabs;
    ae_bool result;


    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    nslack = n+2*(nec+nlec)+(nic+nlic);
    result = ae_false;
    tol = 1.0E-6*state->trustrad*((double)1+rmaxabsv(n, &currentlinearization->x, _state));
    
    /*
     * Already too big, skip analysis
     */
    if( ae_fp_greater_eq(state->meritmu,0.9*nlcsqp_maxmeritmu) )
    {
        return result;
    }
    
    /*
     * Allocate storage
     */
    rallocv(nslack, &state->tmppend, _state);
    rallocv(n, &state->dummylagbcmult, _state);
    rallocv(nec+nic+nlec+nlic, &state->dummylagxcmult, _state);
    
    /*
     * Compute the following quantities:
     * * penalty at X
     * * penalty at X+D
     * * whether we failed to enforce constraints at X+D or not
     * * whether any single constraint can be made feasible within max(|D|)-sized box (ignoring other constraints) or not
     */
    penaltyatx = (double)(0);
    penaltyatd = (double)(0);
    failedtoenforce = ae_false;
    infeasiblewithinbox = ae_false;
    mxd = rmaxabsv(n, dtrial, _state);
    expandedrad = 1.1*mxd;
    tol2 = ae_maxreal(ae_sqrt(ae_machineepsilon, _state)*state->trustrad, (double)1000*ae_machineepsilon, _state);
    for(i=0; i<=nec+nic-1; i++)
    {
        cx = rdotvr(n, &currentlinearization->x, &state->scaledcleic, i, _state)-state->scaledcleic.ptr.pp_double[i][n];
        cxd = cx+rdotvr(n, dtrial, &state->scaledcleic, i, _state);
        cx = rcase2(i<nec, cx, ae_maxreal(cx, 0.0, _state), _state);
        cxd = rcase2(i<nec, cxd, ae_maxreal(cxd, 0.0, _state), _state);
        sumabs = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            sumabs = sumabs+ae_fabs(state->scaledcleic.ptr.pp_double[i][j], _state);
        }
        infeasiblewithinbox = infeasiblewithinbox||ae_fp_greater(ae_fabs(cx, _state),sumabs*expandedrad+tol2);
        penaltyatx = penaltyatx+ae_fabs(cx, _state);
        penaltyatd = penaltyatd+ae_fabs(cxd, _state);
        failedtoenforce = failedtoenforce||ae_fp_greater(ae_fabs(cxd, _state),sumabs*tol);
    }
    for(i=1; i<=nlec+nlic; i++)
    {
        cx = currentlinearization->fi.ptr.p_double[i];
        cxd = cx+rdotvr(n, dtrial, &currentlinearization->jac, i, _state);
        cx = rcase2(i<1+nlec, cx, ae_maxreal(cx, 0.0, _state), _state);
        cxd = rcase2(i<1+nlec, cxd, ae_maxreal(cxd, 0.0, _state), _state);
        sumabs = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            sumabs = sumabs+ae_fabs(currentlinearization->jac.ptr.pp_double[i][j], _state);
        }
        infeasiblewithinbox = infeasiblewithinbox||ae_fp_greater(ae_fabs(cx, _state),sumabs*expandedrad+tol2);
        penaltyatx = penaltyatx+ae_fabs(cx, _state);
        penaltyatd = penaltyatd+ae_fabs(cxd, _state);
        failedtoenforce = failedtoenforce||ae_fp_greater(ae_fabs(cxd, _state),sumabs*tol);
    }
    if( infeasiblewithinbox&&ae_fp_less(mxd,0.9*state->trustrad) )
    {
        
        /*
         * MeritMu must be increased if a short DTrial was proposed, but there are some
         * constraints that are infeasible within max|DTrial|-sized box
         */
        result = ae_true;
        if( dotrace )
        {
            ae_trace("> a short DTrial was proposed, but some constraints are infeasible within trust region; MeritMu increased, iteration skipped\n");
        }
    }
    
    /*
     * We may need to increase MeritMu
     */
    if( failedtoenforce )
    {
        
        /*
         * We failed to drive constraints linearizations to zero. But what is the best
         * theoretically possible reduction (ignoring quadratic model)?
         */
        if( !nlcsqp_qpsubproblemsolve(state, &state->subsolver, &currentlinearization->x, &currentlinearization->fi, &currentlinearization->jac, &state->hess, 0.0, 0.0, &state->tmppend, &state->dummylagbcmult, &state->dummylagxcmult, dotrace, &tt, &dummy0, &dummy1, &dummy2, &dummy3, _state) )
        {
            if( dotrace )
            {
                ae_trace("> [WARNING] QP subproblem failed with TerminationType=%0d when updating MeritMu\n",
                    (int)(tt));
            }
            return result;
        }
        penaltyatdinf = (double)(0);
        failedtoenforceinf = ae_false;
        for(i=0; i<=nec+nic-1; i++)
        {
            constrval = -state->scaledcleic.ptr.pp_double[i][n];
            allowederr = (double)(0);
            for(j=0; j<=n-1; j++)
            {
                constrval = constrval+state->scaledcleic.ptr.pp_double[i][j]*(currentlinearization->x.ptr.p_double[j]+state->tmppend.ptr.p_double[j]);
                allowederr = allowederr+ae_fabs(state->scaledcleic.ptr.pp_double[i][j]*tol, _state);
            }
            if( i>=nec )
            {
                constrval = ae_maxreal(constrval, 0.0, _state);
            }
            penaltyatdinf = penaltyatdinf+ae_fabs(constrval, _state);
            failedtoenforceinf = failedtoenforceinf||ae_fp_greater(ae_fabs(constrval, _state),allowederr);
        }
        for(i=0; i<=nlec+nlic-1; i++)
        {
            constrval = currentlinearization->fi.ptr.p_double[1+i];
            allowederr = (double)(0);
            for(j=0; j<=n-1; j++)
            {
                constrval = constrval+currentlinearization->jac.ptr.pp_double[1+i][j]*state->tmppend.ptr.p_double[j];
                allowederr = allowederr+ae_fabs(currentlinearization->jac.ptr.pp_double[1+i][j]*tol, _state);
            }
            if( i>=nlec )
            {
                constrval = ae_maxreal(constrval, 0.0, _state);
            }
            penaltyatdinf = penaltyatdinf+ae_fabs(constrval, _state);
            failedtoenforceinf = failedtoenforceinf||ae_fp_greater(ae_fabs(constrval, _state),allowederr);
        }
        
        /*
         * Branch on the constraint status within trust region
         */
        if( failedtoenforceinf )
        {
            
            /*
             * Constraints are infeasible within the trust region, check that the actual penalty decrease
             * is at least some fraction of the best possible decrease.
             */
            if( ae_fp_less(penaltyatx-penaltyatd,nlcsqp_meritmueps*ae_maxreal(penaltyatx-penaltyatdinf, 0.0, _state))&&ae_fp_less(penaltyatdinf,penaltyatx*((double)1-nlcsqp_constraintsstagnationeps)) )
            {
                
                /*
                 * Merit function does not decrease fast enough, we must increase MeritMu
                 */
                result = ae_true;
                if( dotrace )
                {
                    ae_trace("> constraints infeasibility does not decrease fast enough, MeritMu increased, iteration skipped\n");
                }
            }
        }
        else
        {
            
            /*
             * Constraints are feasible within trust region, we must increase MeritMu
             */
            result = ae_true;
            if( dotrace )
            {
                ae_trace("> MeritMu penalty does not enforce constraints feasibility, MeritMu increased, iteration skipped\n");
            }
        }
    }
    
    /*
     * Now we check that decrease in the quadratic model + penalty is a significant fraction of
     * the decrease in the penalty.
     */
    meritdecrease = -(predictedchangemodel+state->meritmu*predictedchangepenalty);
    penaltydecrease = -state->meritmu*predictedchangepenalty;
    if( ae_fp_less(meritdecrease,nlcsqp_meritmueps*penaltydecrease) )
    {
        result = ae_true;
        if( dotrace )
        {
            ae_trace("> sufficient negativity condition does not hold, MeritMu increased, iteration skipped\n");
        }
    }
    return result;
}


/*************************************************************************
Returns k-th component of the trust region

  -- ALGLIB --
     Copyright 25.09.2023 by Bochkanov Sergey
*************************************************************************/
static double nlcsqp_gettrustregionk(/* Real    */ const ae_vector* xcur,
     ae_int_t k,
     double trustrad,
     ae_state *_state)
{
    double result;


    result = trustrad*ae_maxreal(ae_fabs(xcur->ptr.p_double[k], _state), (double)(1), _state);
    return result;
}


/*************************************************************************
Adjusts merit function value using non-monotonic correction  (sets  it  to
maximum among several last values, returns adjusted value)

  -- ALGLIB --
     Copyright 25.09.2023 by Bochkanov Sergey
*************************************************************************/
static double nlcsqp_nonmonotonicadjustment(minsqpstate* state,
     double fraw,
     ae_state *_state)
{
    ae_frame _frame_block;
    varsfuncjac *vfj;
    ae_smart_ptr _vfj;
    ae_int_t i;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&_vfj, 0, sizeof(_vfj));
    ae_smart_ptr_init(&_vfj, (void**)&vfj, _state, ae_true);

    result = fraw;
    for(i=0; i<=ae_minint(state->nonmonotoniccnt, nlcsqp_nonmonotoniclen, _state)-1; i++)
    {
        ae_obj_array_get(&state->nonmonotonicmem, i, &_vfj, _state);
        result = ae_maxreal(result, nlcsqp_meritfunction(state, vfj, state->meritmu, _state), _state);
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Saves current point to the nonmonotonic memory.

  -- ALGLIB --
     Copyright 25.09.2023 by Bochkanov Sergey
*************************************************************************/
static void nlcsqp_nonmonotonicsave(minsqpstate* state,
     const varsfuncjac* cur,
     ae_state *_state)
{
    ae_frame _frame_block;
    varsfuncjac *vfj;
    ae_smart_ptr _vfj;

    ae_frame_make(_state, &_frame_block);
    memset(&_vfj, 0, sizeof(_vfj));
    ae_smart_ptr_init(&_vfj, (void**)&vfj, _state, ae_true);

    if( nlcsqp_nonmonotoniclen==0 )
    {
        ae_frame_leave(_state);
        return;
    }
    if( state->nonmonotoniccnt<nlcsqp_nonmonotoniclen )
    {
        
        /*
         * Append point to the buffer
         */
        ae_assert(ae_obj_array_get_length(&state->nonmonotonicmem)==state->nonmonotoniccnt, "SQP: integrity check 3710 failed", _state);
        vfj = (varsfuncjac*)ae_malloc(sizeof(varsfuncjac), _state); /* note: using vfj as a temporary prior to assigning its value to _vfj */
        memset(vfj, 0, sizeof(varsfuncjac));
        _varsfuncjac_init(vfj, _state, ae_false);
        ae_smart_ptr_assign(&_vfj, vfj, ae_true, ae_true, (ae_int_t)sizeof(varsfuncjac), _varsfuncjac_init_copy, _varsfuncjac_destroy);
        vfjcopy(cur, vfj, _state);
        ae_obj_array_append_transfer(&state->nonmonotonicmem, &_vfj, _state);
    }
    else
    {
        
        /*
         * Rewrite one of points in the buffer
         */
        ae_assert(ae_obj_array_get_length(&state->nonmonotonicmem)==nlcsqp_nonmonotoniclen, "SQP: integrity check 3711 failed", _state);
        ae_obj_array_get(&state->nonmonotonicmem, state->nonmonotoniccnt%nlcsqp_nonmonotoniclen, &_vfj, _state);
        vfjcopy(cur, vfj, _state);
    }
    state->nonmonotoniccnt = state->nonmonotoniccnt+1;
    ae_frame_leave(_state);
}


/*************************************************************************
Rescales target/constraints stored in the nonmonotonic memory.

  -- ALGLIB --
     Copyright 25.09.2023 by Bochkanov Sergey
*************************************************************************/
static void nlcsqp_nonmonotonicmultiplyby(minsqpstate* state,
     ae_int_t jacrow,
     double v,
     ae_state *_state)
{
    ae_frame _frame_block;
    varsfuncjac *vfj;
    ae_smart_ptr _vfj;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&_vfj, 0, sizeof(_vfj));
    ae_smart_ptr_init(&_vfj, (void**)&vfj, _state, ae_true);

    for(i=0; i<=ae_minint(state->nonmonotoniccnt, nlcsqp_nonmonotoniclen, _state)-1; i++)
    {
        ae_obj_array_get(&state->nonmonotonicmem, i, &_vfj, _state);
        ae_assert(vfj->isdense, "SQP: integrity check 7600 failed", _state);
        vfj->fi.ptr.p_double[jacrow] = vfj->fi.ptr.p_double[jacrow]*v;
        rmulr(vfj->n, v, &vfj->jac, jacrow, _state);
    }
    ae_frame_leave(_state);
}


void _minsqpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minsqpsubsolver *p = (minsqpsubsolver*)_p;
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
    ae_vector_init(&p->d0, 0, DT_REAL, _state, make_automatic);
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


void _minsqpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minsqpsubsolver       *dst = (minsqpsubsolver*)_dst;
    const minsqpsubsolver *src = (const minsqpsubsolver*)_src;
    _vipmstate_init_copy(&dst->ipmsolver, &src->ipmsolver, _state, make_automatic);
    _ipm2state_init_copy(&dst->ipm2, &src->ipm2, _state, make_automatic);
    ae_vector_init_copy(&dst->curb, &src->curb, _state, make_automatic);
    ae_vector_init_copy(&dst->curbndl, &src->curbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->curbndu, &src->curbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->cural, &src->cural, _state, make_automatic);
    ae_vector_init_copy(&dst->curau, &src->curau, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparserawlc, &src->sparserawlc, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseefflc, &src->sparseefflc, _state, make_automatic);
    ae_vector_init_copy(&dst->d0, &src->d0, _state, make_automatic);
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


void _minsqpsubsolver_clear(void* _p)
{
    minsqpsubsolver *p = (minsqpsubsolver*)_p;
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
    ae_vector_clear(&p->d0);
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


void _minsqpsubsolver_destroy(void* _p)
{
    minsqpsubsolver *p = (minsqpsubsolver*)_p;
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
    ae_vector_destroy(&p->d0);
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


void _minsqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minsqpstate *p = (minsqpstate*)_p;
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
    _minsqpsubsolver_init(&p->subsolver, _state, make_automatic);
    _xbfgshessian_init(&p->hess, _state, make_automatic);
    ae_obj_array_init(&p->nonmonotonicmem, _state, make_automatic);
    ae_vector_init(&p->lagbcmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagxcmult, 0, DT_REAL, _state, make_automatic);
    _varsfuncjac_init(&p->cand, _state, make_automatic);
    _varsfuncjac_init(&p->corr, _state, make_automatic);
    _varsfuncjac_init(&p->probe, _state, make_automatic);
    _varsfuncjac_init(&p->currentlinearization, _state, make_automatic);
    ae_vector_init(&p->dtrial, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dmu, 0, DT_REAL, _state, make_automatic);
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


void _minsqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minsqpstate       *dst = (minsqpstate*)_dst;
    const minsqpstate *src = (const minsqpstate*)_src;
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
    dst->meritmu = src->meritmu;
    dst->trustrad = src->trustrad;
    dst->trustradgrowth = src->trustradgrowth;
    dst->trustradstagnationcnt = src->trustradstagnationcnt;
    dst->fstagnationcnt = src->fstagnationcnt;
    _varsfuncjac_init_copy(&dst->stepk, &src->stepk, _state, make_automatic);
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->xprev, &src->xprev, _state, make_automatic);
    ae_vector_init_copy(&dst->fscales, &src->fscales, _state, make_automatic);
    ae_vector_init_copy(&dst->tracegamma, &src->tracegamma, _state, make_automatic);
    _minsqpsubsolver_init_copy(&dst->subsolver, &src->subsolver, _state, make_automatic);
    _xbfgshessian_init_copy(&dst->hess, &src->hess, _state, make_automatic);
    ae_obj_array_init_copy(&dst->nonmonotonicmem, &src->nonmonotonicmem, _state, make_automatic);
    dst->nonmonotoniccnt = src->nonmonotoniccnt;
    ae_vector_init_copy(&dst->lagbcmult, &src->lagbcmult, _state, make_automatic);
    ae_vector_init_copy(&dst->lagxcmult, &src->lagxcmult, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->cand, &src->cand, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->corr, &src->corr, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->probe, &src->probe, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->currentlinearization, &src->currentlinearization, _state, make_automatic);
    ae_vector_init_copy(&dst->dtrial, &src->dtrial, _state, make_automatic);
    ae_vector_init_copy(&dst->d0, &src->d0, _state, make_automatic);
    ae_vector_init_copy(&dst->d1, &src->d1, _state, make_automatic);
    ae_vector_init_copy(&dst->dmu, &src->dmu, _state, make_automatic);
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


void _minsqpstate_clear(void* _p)
{
    minsqpstate *p = (minsqpstate*)_p;
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
    _minsqpsubsolver_clear(&p->subsolver);
    _xbfgshessian_clear(&p->hess);
    ae_obj_array_clear(&p->nonmonotonicmem);
    ae_vector_clear(&p->lagbcmult);
    ae_vector_clear(&p->lagxcmult);
    _varsfuncjac_clear(&p->cand);
    _varsfuncjac_clear(&p->corr);
    _varsfuncjac_clear(&p->probe);
    _varsfuncjac_clear(&p->currentlinearization);
    ae_vector_clear(&p->dtrial);
    ae_vector_clear(&p->d0);
    ae_vector_clear(&p->d1);
    ae_vector_clear(&p->dmu);
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


void _minsqpstate_destroy(void* _p)
{
    minsqpstate *p = (minsqpstate*)_p;
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
    _minsqpsubsolver_destroy(&p->subsolver);
    _xbfgshessian_destroy(&p->hess);
    ae_obj_array_destroy(&p->nonmonotonicmem);
    ae_vector_destroy(&p->lagbcmult);
    ae_vector_destroy(&p->lagxcmult);
    _varsfuncjac_destroy(&p->cand);
    _varsfuncjac_destroy(&p->corr);
    _varsfuncjac_destroy(&p->probe);
    _varsfuncjac_destroy(&p->currentlinearization);
    ae_vector_destroy(&p->dtrial);
    ae_vector_destroy(&p->d0);
    ae_vector_destroy(&p->d1);
    ae_vector_destroy(&p->dmu);
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
