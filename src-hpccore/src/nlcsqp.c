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
#include "nlcsqp.h"


/*$ Declarations $*/
static double nlcsqp_sqpdeltadecrease = 0.50;
static double nlcsqp_sqpdeltaincrease = 0.99;
static double nlcsqp_maxtrustraddecay = 0.1;
static double nlcsqp_deftrustradgrowth = 1.41;
static double nlcsqp_maxtrustradgrowth = 10.0;
static double nlcsqp_momentumgrowth = 2;
static double nlcsqp_maxbigc = 1.0E5;
static double nlcsqp_maxmeritmu = 1.0E5;
static double nlcsqp_augmentationfactor = 0.0;
static double nlcsqp_inittrustrad = 0.1;
static double nlcsqp_stagnationepsf = 1.0E-7;
static ae_int_t nlcsqp_fstagnationlimit = 20;
static ae_int_t nlcsqp_trustradstagnationlimit = 10;
static double nlcsqp_sqpbigscale = 5.0;
static double nlcsqp_sqpsmallscale = 0.2;
static ae_int_t nlcsqp_defaultbfgsresetfreq = 999999;
static double nlcsqp_bigceps = 0.15;
static double nlcsqp_meritmueps = 0.15;
static void nlcsqp_initqpsubsolver(const minsqpstate* sstate,
     minsqpsubsolver* subsolver,
     ae_state *_state);
static void nlcsqp_qpsubsolversetalgoipm(minsqpsubsolver* subsolver,
     ae_state *_state);
static void nlcsqp_fassolve(minsqpsubsolver* subsolver,
     /* Real    */ ae_vector* d0,
     /* Real    */ const ae_matrix* h,
     ae_int_t nq,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* a,
     ae_int_t m,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     double trustrad,
     ae_int_t* terminationtype,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagmult,
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
     ae_int_t* terminationtype,
     double* predictedchangemodel,
     double* predictedchangepenalty,
     double* sumc1,
     ae_state *_state);
static void nlcsqp_meritphaseinit(minsqpmeritphasestate* meritstate,
     /* Real    */ const ae_vector* curx,
     /* Real    */ const ae_vector* curfi,
     /* Real    */ const ae_matrix* curj,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_state *_state);
static ae_bool nlcsqp_meritphaseiteration(minsqpstate* state,
     minsqpmeritphasestate* meritstate,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state);
static void nlcsqp_meritphaseresults(const minsqpmeritphasestate* meritstate,
     /* Real    */ ae_vector* curx,
     /* Real    */ ae_vector* curfi,
     /* Real    */ ae_matrix* curj,
     ae_bool* increasebigc,
     ae_bool* increasemeritmu,
     ae_bool* meritfstagnated,
     ae_int_t* status,
     ae_state *_state);
static void nlcsqp_sqpsendx(minsqpstate* state,
     /* Real    */ const ae_vector* xs,
     ae_state *_state);
static ae_bool nlcsqp_sqpretrievefij(const minsqpstate* state,
     /* Real    */ ae_vector* fis,
     /* Real    */ ae_matrix* js,
     ae_state *_state);
static void nlcsqp_sqpcopystate(const minsqpstate* state,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* fi0,
     /* Real    */ const ae_matrix* j0,
     /* Real    */ ae_vector* x1,
     /* Real    */ ae_vector* fi1,
     /* Real    */ ae_matrix* j1,
     ae_state *_state);
static void nlcsqp_lagrangianfg(minsqpstate* state,
     /* Real    */ const ae_vector* x,
     double trustrad,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* j,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     ae_bool uselagrangeterms,
     minsqptmplagrangian* tmp,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state);
static double nlcsqp_meritfunction(minsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     minsqptmpmerit* tmp,
     ae_state *_state);
static double nlcsqp_rawlagrangian(minsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     minsqptmpmerit* tmp,
     ae_state *_state);
static void nlcsqp_meritfunctionandrawlagrangian(minsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     minsqptmpmerit* tmp,
     double* meritf,
     double* rawlag,
     ae_state *_state);
static ae_bool nlcsqp_penaltiesneedincrease(minsqpstate* state,
     minsqpmeritphasestate* meritstate,
     ae_bool dotrace,
     ae_bool* increasebigc,
     ae_bool* increasemeritmu,
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
     double epsx,
     ae_int_t maxits,
     minsqpstate* state,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;


    state->n = n;
    state->nec = nec;
    state->nic = nic;
    state->nlec = nlec;
    state->nlic = nlic;
    
    /*
     * Prepare RCOMM state
     */
    ae_vector_set_length(&state->rstate.ia, 9+1, _state);
    ae_vector_set_length(&state->rstate.ba, 5+1, _state);
    ae_vector_set_length(&state->rstate.ra, 6+1, _state);
    state->rstate.stage = -1;
    state->needfij = ae_false;
    state->xupdated = ae_false;
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->fi, 1+nlec+nlic, _state);
    ae_matrix_set_length(&state->j, 1+nlec+nlic, n, _state);
    
    /*
     * Allocate memory.
     */
    rvectorsetlengthatleast(&state->s, n, _state);
    rvectorsetlengthatleast(&state->step0x, n, _state);
    rvectorsetlengthatleast(&state->stepkx, n, _state);
    rvectorsetlengthatleast(&state->backupx, n, _state);
    rvectorsetlengthatleast(&state->step0fi, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&state->stepkfi, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&state->backupfi, 1+nlec+nlic, _state);
    rmatrixsetlengthatleast(&state->step0j, 1+nlec+nlic, n, _state);
    rmatrixsetlengthatleast(&state->stepkj, 1+nlec+nlic, n, _state);
    rvectorsetlengthatleast(&state->fscales, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&state->tracegamma, 1+nlec+nlic, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    rvectorsetlengthatleast(&state->scaledbndl, n, _state);
    rvectorsetlengthatleast(&state->scaledbndu, n, _state);
    rmatrixsetlengthatleast(&state->scaledcleic, nec+nic, n+1, _state);
    ivectorsetlengthatleast(&state->lcsrcidx, nec+nic, _state);
    
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
        state->step0x.ptr.p_double[i] = x0->ptr.p_double[i]/s->ptr.p_double[i];
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
            state->step0x.ptr.p_double[i] = ae_maxreal(state->step0x.ptr.p_double[i], state->scaledbndl.ptr.p_double[i], _state);
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state->step0x.ptr.p_double[i] = ae_minreal(state->step0x.ptr.p_double[i], state->scaledbndu.ptr.p_double[i], _state);
        }
    }
    
    /*
     * Stopping criteria and settings
     */
    state->epsx = epsx;
    state->maxits = maxits;
    state->bfgsresetfreq = nlcsqp_defaultbfgsresetfreq;
    
    /*
     * Report fields
     */
    state->repsimplexiterations = 0;
    state->repsimplexiterations1 = 0;
    state->repsimplexiterations2 = 0;
    state->repsimplexiterations3 = 0;
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
    ae_int_t status;
    double deltamax;
    double multiplyby;
    double setscaleto;
    double prevtrustrad;
    ae_int_t subiterationidx;
    ae_bool trustradstagnated;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_bool increasebigc;
    ae_bool increasemeritmu;
    ae_bool meritfstagnated;
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
        status = state->rstate.ia.ptr.p_int[8];
        subiterationidx = state->rstate.ia.ptr.p_int[9];
        trustradstagnated = state->rstate.ba.ptr.p_bool[0];
        dotrace = state->rstate.ba.ptr.p_bool[1];
        dodetailedtrace = state->rstate.ba.ptr.p_bool[2];
        increasebigc = state->rstate.ba.ptr.p_bool[3];
        increasemeritmu = state->rstate.ba.ptr.p_bool[4];
        meritfstagnated = state->rstate.ba.ptr.p_bool[5];
        v = state->rstate.ra.ptr.p_double[0];
        vv = state->rstate.ra.ptr.p_double[1];
        mx = state->rstate.ra.ptr.p_double[2];
        deltamax = state->rstate.ra.ptr.p_double[3];
        multiplyby = state->rstate.ra.ptr.p_double[4];
        setscaleto = state->rstate.ra.ptr.p_double[5];
        prevtrustrad = state->rstate.ra.ptr.p_double[6];
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
        status = 809;
        subiterationidx = 205;
        trustradstagnated = ae_false;
        dotrace = ae_true;
        dodetailedtrace = ae_false;
        increasebigc = ae_true;
        increasemeritmu = ae_true;
        meritfstagnated = ae_false;
        v = -900.0;
        vv = -318.0;
        mx = -940.0;
        deltamax = 1016.0;
        multiplyby = -229.0;
        setscaleto = -536.0;
        prevtrustrad = 487.0;
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
    
    /*
     * Prepare rcomm interface
     */
    state->needfij = ae_false;
    state->xupdated = ae_false;
    
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
    
    /*
     * Avoid spurious warnings about possibly uninitialized vars
     */
    status = 0;
    
    /*
     * Evaluate function vector and Jacobian at Step0X, send first location report.
     * Compute initial violation of constraints.
     */
    nlcsqp_sqpsendx(state, &state->step0x, _state);
    state->needfij = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    if( !nlcsqp_sqpretrievefij(state, &state->step0fi, &state->step0j, _state) )
    {
        
        /*
         * Failed to retrieve function/Jaconian, infinities detected!
         */
        for(i=0; i<=n-1; i++)
        {
            state->stepkx.ptr.p_double[i] = state->step0x.ptr.p_double[i];
        }
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    nlcsqp_sqpcopystate(state, &state->step0x, &state->step0fi, &state->step0j, &state->stepkx, &state->stepkfi, &state->stepkj, _state);
    nlcsqp_sqpsendx(state, &state->stepkx, _state);
    state->f = state->stepkfi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, &state->stepkx, n, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlcviolation(&state->stepkfi, &state->fscales, nlec, nlic, &state->repnlcerr, &state->repnlcidx, _state);
    
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
    state->bigc = (double)(500);
    state->meritmu = (double)(500);
    hessianinitlowrank(&state->hess, n, ae_minint(n, 25, _state), (double)10*coalesce(state->epsx, ae_sqrt(ae_machineepsilon, _state), _state), 1.0E2, _state);
    nlcsqp_initqpsubsolver(state, &state->subsolver, _state);
lbl_3:
    if( ae_false )
    {
        goto lbl_4;
    }
    
    /*
     * Before beginning new outer iteration:
     * * renormalize target function and/or constraints, if some of them have too large magnitudes
     * * save initial point for the outer iteration
     */
    for(i=0; i<=nlec+nlic; i++)
    {
        
        /*
         * Determine (a) multiplicative coefficient applied to function value
         * and Jacobian row, and (b) new value of the function scale.
         */
        mx = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            mx = ae_maxreal(mx, ae_fabs(state->stepkj.ptr.pp_double[i][j], _state), _state);
        }
        multiplyby = 1.0;
        setscaleto = state->fscales.ptr.p_double[i];
        if( ae_fp_greater_eq(mx,nlcsqp_sqpbigscale) )
        {
            multiplyby = (double)1/mx;
            setscaleto = state->fscales.ptr.p_double[i]*mx;
        }
        if( ae_fp_less_eq(mx,nlcsqp_sqpsmallscale)&&ae_fp_greater(state->fscales.ptr.p_double[i],1.0) )
        {
            if( ae_fp_greater(state->fscales.ptr.p_double[i]*mx,(double)(1)) )
            {
                multiplyby = (double)1/mx;
                setscaleto = state->fscales.ptr.p_double[i]*mx;
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
            state->stepkfi.ptr.p_double[i] = state->stepkfi.ptr.p_double[i]*multiplyby;
            for(j=0; j<=n-1; j++)
            {
                state->stepkj.ptr.pp_double[i][j] = state->stepkj.ptr.pp_double[i][j]*multiplyby;
            }
            state->fscales.ptr.p_double[i] = setscaleto;
            state->tracegamma.ptr.p_double[i] = state->tracegamma.ptr.p_double[i]*multiplyby;
        }
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
            tracevectorunscaledunshiftedautoprec(&state->step0x, n, &state->s, ae_true, &state->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("> printing scaled data (after applying variable and function scales)\n");
            ae_trace("X (scaled)    = ");
            tracevectorautoprec(&state->step0x, 0, n, _state);
            ae_trace("\n");
            ae_trace("FScales       = ");
            tracevectorautoprec(&state->fscales, 0, 1+nlec+nlic, _state);
            ae_trace("\n");
            ae_trace("GammaScl      = ");
            tracevectorautoprec(&state->tracegamma, 0, 1+nlec+nlic, _state);
            ae_trace("\n");
            ae_trace("Fi (scaled)   = ");
            tracevectorautoprec(&state->stepkfi, 0, 1+nlec+nlic, _state);
            ae_trace("\n");
            ae_trace("|Ji| (scaled) = ");
            tracerownrm1autoprec(&state->stepkj, 0, 1+nlec+nlic, 0, n, _state);
            ae_trace("\n");
        }
        mx = (double)(0);
        for(i=1; i<=nlec; i++)
        {
            mx = ae_maxreal(mx, ae_fabs(state->stepkfi.ptr.p_double[i], _state), _state);
        }
        for(i=nlec+1; i<=nlec+nlic; i++)
        {
            mx = ae_maxreal(mx, state->stepkfi.ptr.p_double[i], _state);
        }
        ae_trace("trustRad      = %0.3e\n",
            (double)(state->trustrad));
        ae_trace("bigC          = %0.3e    (penalty for violation of constraint linearizations)\n",
            (double)(state->bigc));
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
    }
    
    /*
     * PHASE 2
     *
     * This phase is a primary part of the algorithm which is responsible for its
     * convergence properties.
     *
     * It solves QP subproblem with possible activation and deactivation of constraints
     * and then starts backtracking (step length is bounded by 1.0) merit function search
     * (with second-order correction to deal with Maratos effect) on the direction produced
     * by QP subproblem.
     *
     * This phase is everything we need to in order to have convergence; however,
     * it has one performance-related issue: using "general" interior point QP solver
     * results in slow solution times. Fast equality-constrained phase is essential for
     * the quick convergence.
     */
    nlcsqp_qpsubsolversetalgoipm(&state->subsolver, _state);
    nlcsqp_sqpcopystate(state, &state->stepkx, &state->stepkfi, &state->stepkj, &state->step0x, &state->step0fi, &state->step0j, _state);
    nlcsqp_meritphaseinit(&state->meritstate, &state->stepkx, &state->stepkfi, &state->stepkj, n, nec, nic, nlec, nlic, _state);
lbl_5:
    if( !nlcsqp_meritphaseiteration(state, &state->meritstate, smonitor, userterminationneeded, _state) )
    {
        goto lbl_6;
    }
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    goto lbl_5;
lbl_6:
    nlcsqp_meritphaseresults(&state->meritstate, &state->stepkx, &state->stepkfi, &state->stepkj, &increasebigc, &increasemeritmu, &meritfstagnated, &status, _state);
    if( status==0 )
    {
        goto lbl_4;
    }
    ae_assert(status>0, "MinSQPIteration: integrity check failed", _state);
    
    /*
     * Update BigC/MeritMu
     */
    if( increasebigc )
    {
        state->bigc = ae_minreal((double)2*state->bigc, nlcsqp_maxbigc, _state);
    }
    if( increasemeritmu )
    {
        state->meritmu = ae_minreal((double)2*state->meritmu, nlcsqp_maxmeritmu, _state);
    }
    state->bigc = ae_maxreal(state->bigc, state->meritmu, _state);
    state->meritmu = state->bigc;
    
    /*
     * Update trust region.
     *
     * NOTE: when trust region radius remains fixed for a long time it may mean that we 
     *       stagnated in eternal loop. In such cases we decrease it slightly in order
     *       to break possible loop. If such decrease was unnecessary, it may be easily
     *       fixed within few iterations.
     */
    deltamax = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        deltamax = ae_maxreal(deltamax, ae_fabs(state->step0x.ptr.p_double[i]-state->stepkx.ptr.p_double[i], _state)/state->trustrad, _state);
    }
    trustradstagnated = ae_false;
    inc(&state->trustradstagnationcnt, _state);
    prevtrustrad = state->trustrad;
    if( ae_fp_less_eq(deltamax,nlcsqp_sqpdeltadecrease) )
    {
        state->trustrad = state->trustrad*ae_maxreal(deltamax/nlcsqp_sqpdeltadecrease, nlcsqp_maxtrustraddecay, _state);
    }
    if( ae_fp_greater_eq(deltamax,nlcsqp_sqpdeltaincrease) )
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
        if( increasebigc )
        {
            ae_trace("BigC        = %0.3e (increasing)\n",
                (double)(state->bigc));
        }
        if( increasemeritmu )
        {
            ae_trace("meritMu     = %0.3e (increasing)\n",
                (double)(state->meritmu));
        }
    }
    
    /*
     * Advance outer iteration counter, test stopping criteria
     */
    inc(&state->repiterationscount, _state);
    state->fstagnationcnt = icase2(meritfstagnated, state->fstagnationcnt+1, 0, _state);
    if( ae_fp_less_eq(state->trustrad,state->epsx) )
    {
        state->repterminationtype = 2;
        if( dotrace )
        {
            ae_trace("> stopping condition met: trust radius is smaller than %0.3e\n",
                (double)(state->epsx));
        }
        goto lbl_4;
    }
    if( state->maxits>0&&state->repiterationscount>=state->maxits )
    {
        state->repterminationtype = 5;
        if( dotrace )
        {
            ae_trace("> stopping condition met: %0d iterations performed\n",
                (int)(state->repiterationscount));
        }
        goto lbl_4;
    }
    if( state->fstagnationcnt>=nlcsqp_fstagnationlimit )
    {
        state->repterminationtype = 7;
        if( dotrace )
        {
            ae_trace("> stopping criteria are too stringent: F stagnated for %0d its, stopping\n",
                (int)(state->fstagnationcnt));
        }
        goto lbl_4;
    }
    goto lbl_3;
lbl_4:
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
    state->rstate.ia.ptr.p_int[8] = status;
    state->rstate.ia.ptr.p_int[9] = subiterationidx;
    state->rstate.ba.ptr.p_bool[0] = trustradstagnated;
    state->rstate.ba.ptr.p_bool[1] = dotrace;
    state->rstate.ba.ptr.p_bool[2] = dodetailedtrace;
    state->rstate.ba.ptr.p_bool[3] = increasebigc;
    state->rstate.ba.ptr.p_bool[4] = increasemeritmu;
    state->rstate.ba.ptr.p_bool[5] = meritfstagnated;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vv;
    state->rstate.ra.ptr.p_double[2] = mx;
    state->rstate.ra.ptr.p_double[3] = deltamax;
    state->rstate.ra.ptr.p_double[4] = multiplyby;
    state->rstate.ra.ptr.p_double[5] = setscaleto;
    state->rstate.ra.ptr.p_double[6] = prevtrustrad;
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
     * Initial state
     */
    subsolver->algokind = 0;
    
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
This function sets subsolver algorithm to interior point method (IPM)

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static void nlcsqp_qpsubsolversetalgoipm(minsqpsubsolver* subsolver,
     ae_state *_state)
{


    subsolver->algokind = 0;
}


/*************************************************************************
This function solves QP subproblem given by initial point X, function vector Fi
and Jacobian Jac, and returns estimates of Lagrangian multipliers and search direction D[].

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static void nlcsqp_fassolve(minsqpsubsolver* subsolver,
     /* Real    */ ae_vector* d0,
     /* Real    */ const ae_matrix* h,
     ae_int_t nq,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* a,
     ae_int_t m,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     double trustrad,
     ae_int_t* terminationtype,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagmult,
     ae_state *_state)
{
    ae_int_t i;


    *terminationtype = 1;
    
    /*
     * Initial point, integrity check for constraints
     */
    bvectorsetlengthatleast(&subsolver->hasbndl, n, _state);
    bvectorsetlengthatleast(&subsolver->hasbndu, n, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        subsolver->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
        ae_assert(!subsolver->hasbndl.ptr.p_bool[i]||ae_fp_less_eq(bndl->ptr.p_double[i],d0->ptr.p_double[i]), "FASSolve: integrity check failed", _state);
        ae_assert(!subsolver->hasbndu.ptr.p_bool[i]||ae_fp_greater_eq(bndu->ptr.p_double[i],d0->ptr.p_double[i]), "FASSolve: integrity check failed", _state);
        d->ptr.p_double[i] = d0->ptr.p_double[i];
    }
    bvectorsetlengthatleast(&subsolver->hasal, m, _state);
    bvectorsetlengthatleast(&subsolver->hasau, m, _state);
    for(i=0; i<=m-1; i++)
    {
        subsolver->hasal.ptr.p_bool[i] = ae_isfinite(al->ptr.p_double[i], _state);
        subsolver->hasau.ptr.p_bool[i] = ae_isfinite(au->ptr.p_double[i], _state);
        if( subsolver->hasal.ptr.p_bool[i]&&subsolver->hasau.ptr.p_bool[i] )
        {
            ae_assert(ae_fp_less_eq(al->ptr.p_double[i],au->ptr.p_double[i]), "FASSolve: integrity check failed", _state);
        }
    }
    rmatrixsetlengthatleast(&subsolver->activea, n, n, _state);
    rvectorsetlengthatleast(&subsolver->activerhs, n, _state);
    ivectorsetlengthatleast(&subsolver->activeidx, n, _state);
    subsolver->activesetsize = 0;
    
    /*
     * Activate equality constraints (at most N)
     */
    for(i=0; i<=m-1; i++)
    {
        if( (subsolver->hasal.ptr.p_bool[i]&&subsolver->hasau.ptr.p_bool[i])&&ae_fp_eq(al->ptr.p_double[i],au->ptr.p_double[i]) )
        {
            
            /*
             * Stop if full set of constraints is activated
             */
            if( subsolver->activesetsize>=n )
            {
                break;
            }
        }
    }
    rvectorsetlengthatleast(&subsolver->tmp0, n, _state);
    rvectorsetlengthatleast(&subsolver->tmp1, n, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->tmp0.ptr.p_double[i] = trustrad;
        subsolver->tmp1.ptr.p_double[i] = 0.0;
    }
    vipminitdensewithslacks(&subsolver->ipmsolver, &subsolver->tmp0, &subsolver->tmp1, nq, n, _state);
    vipmsetquadraticlinear(&subsolver->ipmsolver, h, &subsolver->sparsedummy, 0, ae_true, b, _state);
    vipmsetconstraints(&subsolver->ipmsolver, bndl, bndu, a, m, &subsolver->densedummy, 0, al, au, _state);
    vipmoptimize(&subsolver->ipmsolver, ae_false, &subsolver->tmp0, &subsolver->tmp1, &subsolver->tmp2, terminationtype, _state);
    if( *terminationtype<=0 )
    {
        return;
    }
    for(i=0; i<=n-1; i++)
    {
        d->ptr.p_double[i] = subsolver->tmp0.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        lagmult->ptr.p_double[i] = subsolver->tmp2.ptr.p_double[i];
    }
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
     ae_int_t* terminationtype,
     double* predictedchangemodel,
     double* predictedchangepenalty,
     double* sumc1,
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
     * NOTE: elements [N,NSlack) are equal to bigC + perturbation to improve numeric properties of QP problem
     */
    for(i=0; i<=n-1; i++)
    {
        subsolver->curb.ptr.p_double[i] = alphag*jac->ptr.pp_double[0][i];
    }
    for(i=n; i<=nslack-1; i++)
    {
        subsolver->curb.ptr.p_double[i] = state->bigc;
    }
    
    /*
     * Trust radius constraints for primary variables
     */
    bsetallocv(n, ae_false, &subsolver->retainnegativebclm, _state);
    bsetallocv(n, ae_false, &subsolver->retainpositivebclm, _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->curbndl.ptr.p_double[i] = -state->trustrad;
        subsolver->curbndu.ptr.p_double[i] = state->trustrad;
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
     * Solve quadratic program
     */
    if( subsolver->algokind==0 )
    {
        
        /*
         * Use dense IPM.
         *
         * We always treat its result as a valid solution, even for TerminationType<=0.
         * In case anything is wrong with solution vector, we will detect it during line
         * search phase (merit function does not increase).
         *
         * NOTE: because we cleaned up constraints that are DEFINITELY inactive within
         *       trust region, we do not have to worry about StopOnExcessiveBounds option.
         */
        rsetallocv(nslack, state->trustrad, &subsolver->tmp0, _state);
        rsetallocv(nslack, 0.0, &subsolver->tmp1, _state);
        hessiangetmatrix(hess, ae_true, &subsolver->denseh, _state);
        for(i=0; i<=n-1; i++)
        {
            rmulr(n, alphah, &subsolver->denseh, i, _state);
        }
        vipminitdensewithslacks(&subsolver->ipmsolver, &subsolver->tmp0, &subsolver->tmp1, n, nslack, _state);
        vipmsetquadraticlinear(&subsolver->ipmsolver, &subsolver->denseh, &subsolver->sparsedummy, 0, ae_true, &subsolver->curb, _state);
        vipmsetconstraints(&subsolver->ipmsolver, &subsolver->curbndl, &subsolver->curbndu, &subsolver->sparseefflc, subsolver->sparseefflc.m, &subsolver->densedummy, 0, &subsolver->cural, &subsolver->curau, _state);
        vipmoptimize(&subsolver->ipmsolver, ae_false, &subsolver->tmp0, &subsolver->tmp1, &subsolver->tmp2, terminationtype, _state);
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
        *predictedchangemodel = rdotv(n, d, &subsolver->curb, _state)+0.5*rmatrixsyvmv(n, &subsolver->denseh, 0, 0, ae_true, d, 0, &subsolver->tmp0, _state);
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
         * Done
         */
        return result;
    }
    if( subsolver->algokind==1 )
    {
        
        /*
         * Use fast active set
         */
        nlcsqp_fassolve(subsolver, &subsolver->d0, &hess->hcurrent, n, &subsolver->curb, nslack, &subsolver->curbndl, &subsolver->curbndu, &subsolver->sparseefflc, subsolver->sparseefflc.m, &subsolver->cural, &subsolver->curau, state->trustrad, terminationtype, d, lagxcmult, _state);
        if( *terminationtype<=0 )
        {
            
            /*
             * QP solver failed due to numerical errors; exit
             */
            result = ae_false;
            return result;
        }
        return result;
    }
    
    /*
     * Unexpected
     */
    ae_assert(ae_false, "SQP: unexpected subsolver type", _state);
    return result;
}


/*************************************************************************
This function initializes MeritPhase temporaries. It should be called before
beginning of each new iteration. You may call it multiple  times  for  the
same instance of MeritPhase temporaries.

INPUT PARAMETERS:
    MeritState          -   instance to be initialized.
    N                   -   problem dimensionality
    NEC, NIC            -   linear equality/inequality constraint count
    NLEC, NLIC          -   nonlinear equality/inequality constraint count

OUTPUT PARAMETERS:
    MeritState          -   instance being initialized

  -- ALGLIB --
     Copyright 05.02.2019 by Bochkanov Sergey
*************************************************************************/
static void nlcsqp_meritphaseinit(minsqpmeritphasestate* meritstate,
     /* Real    */ const ae_vector* curx,
     /* Real    */ const ae_vector* curfi,
     /* Real    */ const ae_matrix* curj,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nslack;


    nslack = n+2*(nec+nlec)+(nic+nlic);
    meritstate->n = n;
    meritstate->nec = nec;
    meritstate->nic = nic;
    meritstate->nlec = nlec;
    meritstate->nlic = nlic;
    rvectorsetlengthatleast(&meritstate->dtrial, nslack, _state);
    rvectorsetlengthatleast(&meritstate->deffective, nslack, _state);
    rvectorsetlengthatleast(&meritstate->d0, nslack, _state);
    rvectorsetlengthatleast(&meritstate->d1, nslack, _state);
    rvectorsetlengthatleast(&meritstate->dmu, nslack, _state);
    rvectorsetlengthatleast(&meritstate->stepkx, n, _state);
    rvectorsetlengthatleast(&meritstate->stepkxc, n, _state);
    rvectorsetlengthatleast(&meritstate->stepkxn, n, _state);
    rvectorsetlengthatleast(&meritstate->stepkfi, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&meritstate->stepkfic, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&meritstate->stepkfin, 1+nlec+nlic, _state);
    rmatrixsetlengthatleast(&meritstate->stepkj, 1+nlec+nlic, n, _state);
    rmatrixsetlengthatleast(&meritstate->stepkjc, 1+nlec+nlic, n, _state);
    rmatrixsetlengthatleast(&meritstate->stepkjn, 1+nlec+nlic, n, _state);
    rvectorsetlengthatleast(&meritstate->stepklaggrad, n, _state);
    rvectorsetlengthatleast(&meritstate->stepknlaggrad, n, _state);
    rvectorsetlengthatleast(&meritstate->lagbcmult, n, _state);
    rvectorsetlengthatleast(&meritstate->dummylagbcmult, n, _state);
    rvectorsetlengthatleast(&meritstate->lagxcmult, nec+nic+nlec+nlic, _state);
    rvectorsetlengthatleast(&meritstate->dummylagxcmult, nec+nic+nlec+nlic, _state);
    rvectorsetlengthatleast(&meritstate->curlinx, n, _state);
    rvectorsetlengthatleast(&meritstate->curlinfi, 1+nlec+nlic, _state);
    rmatrixsetlengthatleast(&meritstate->curlinj, 1+nlec+nlic, n, _state);
    rcopyv(n, curx, &meritstate->stepkx, _state);
    rcopyv(1+nlec+nlic, curfi, &meritstate->stepkfi, _state);
    for(i=0; i<=nlec+nlic; i++)
    {
        rcopyrr(n, curj, i, &meritstate->stepkj, i, _state);
    }
    ae_vector_set_length(&meritstate->rmeritphasestate.ia, 8+1, _state);
    ae_vector_set_length(&meritstate->rmeritphasestate.ba, 5+1, _state);
    ae_vector_set_length(&meritstate->rmeritphasestate.ra, 19+1, _state);
    meritstate->rmeritphasestate.stage = -1;
}


/*************************************************************************
This function tries to perform either phase #1 or phase #3 step.

Former corresponds to linear model step (without conjugacy constraints) with
correction for nonlinearity ("second order correction").  Such  correction
helps to overcome  Maratos  effect  (a  tendency  of  L1  penalized  merit
functions to reject nonzero steps).

Latter is a step using linear model with no second order correction.

INPUT PARAMETERS:
    State       -   SQP solver state
    SMonitor    -   smoothness monitor
    UserTerminationNeeded-True if user requested termination

OUTPUT PARAMETERS:
    State       -   RepTerminationType is set to current termination code (if Status=0).
    Status      -   when reverse communication is done, Status is set to:
                    * positive value,  if we can proceed to the next stage
                      of the outer iteration
                    * zero, if algorithm is terminated (RepTerminationType
                      is set to appropriate value)

  -- ALGLIB --
     Copyright 05.02.2019 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcsqp_meritphaseiteration(minsqpstate* state,
     minsqpmeritphasestate* meritstate,
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
    double f0;
    double f1;
    double nu;
    double localstp;
    double tol;
    double stepklagval;
    double stepknlagval;
    ae_bool dotrace;
    ae_bool doprobingalways;
    ae_bool doprobingonfailure;
    ae_bool dotracexd;
    double stp;
    double expandedrad;
    ae_bool socperformed;
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
    if( meritstate->rmeritphasestate.stage>=0 )
    {
        n = meritstate->rmeritphasestate.ia.ptr.p_int[0];
        nslack = meritstate->rmeritphasestate.ia.ptr.p_int[1];
        nec = meritstate->rmeritphasestate.ia.ptr.p_int[2];
        nic = meritstate->rmeritphasestate.ia.ptr.p_int[3];
        nlec = meritstate->rmeritphasestate.ia.ptr.p_int[4];
        nlic = meritstate->rmeritphasestate.ia.ptr.p_int[5];
        i = meritstate->rmeritphasestate.ia.ptr.p_int[6];
        j = meritstate->rmeritphasestate.ia.ptr.p_int[7];
        didx = meritstate->rmeritphasestate.ia.ptr.p_int[8];
        dotrace = meritstate->rmeritphasestate.ba.ptr.p_bool[0];
        doprobingalways = meritstate->rmeritphasestate.ba.ptr.p_bool[1];
        doprobingonfailure = meritstate->rmeritphasestate.ba.ptr.p_bool[2];
        dotracexd = meritstate->rmeritphasestate.ba.ptr.p_bool[3];
        socperformed = meritstate->rmeritphasestate.ba.ptr.p_bool[4];
        meritdecreasefailed = meritstate->rmeritphasestate.ba.ptr.p_bool[5];
        v = meritstate->rmeritphasestate.ra.ptr.p_double[0];
        vv = meritstate->rmeritphasestate.ra.ptr.p_double[1];
        mx = meritstate->rmeritphasestate.ra.ptr.p_double[2];
        f0 = meritstate->rmeritphasestate.ra.ptr.p_double[3];
        f1 = meritstate->rmeritphasestate.ra.ptr.p_double[4];
        nu = meritstate->rmeritphasestate.ra.ptr.p_double[5];
        localstp = meritstate->rmeritphasestate.ra.ptr.p_double[6];
        tol = meritstate->rmeritphasestate.ra.ptr.p_double[7];
        stepklagval = meritstate->rmeritphasestate.ra.ptr.p_double[8];
        stepknlagval = meritstate->rmeritphasestate.ra.ptr.p_double[9];
        stp = meritstate->rmeritphasestate.ra.ptr.p_double[10];
        expandedrad = meritstate->rmeritphasestate.ra.ptr.p_double[11];
        sksk = meritstate->rmeritphasestate.ra.ptr.p_double[12];
        ykyk = meritstate->rmeritphasestate.ra.ptr.p_double[13];
        skyk = meritstate->rmeritphasestate.ra.ptr.p_double[14];
        sumc1 = meritstate->rmeritphasestate.ra.ptr.p_double[15];
        sumc1soc = meritstate->rmeritphasestate.ra.ptr.p_double[16];
        predictedchange = meritstate->rmeritphasestate.ra.ptr.p_double[17];
        predictedchangemodel = meritstate->rmeritphasestate.ra.ptr.p_double[18];
        predictedchangepenalty = meritstate->rmeritphasestate.ra.ptr.p_double[19];
    }
    else
    {
        n = -115;
        nslack = 886;
        nec = 346;
        nic = -722;
        nlec = -413;
        nlic = -461;
        i = 927;
        j = 201;
        didx = 922;
        dotrace = ae_false;
        doprobingalways = ae_false;
        doprobingonfailure = ae_true;
        dotracexd = ae_true;
        socperformed = ae_true;
        meritdecreasefailed = ae_false;
        v = -861.0;
        vv = -678.0;
        mx = -731.0;
        f0 = -675.0;
        f1 = -763.0;
        nu = -233.0;
        localstp = -936.0;
        tol = -279.0;
        stepklagval = 94.0;
        stepknlagval = -812.0;
        stp = 427.0;
        expandedrad = 178.0;
        sksk = -819.0;
        ykyk = -826.0;
        skyk = 667.0;
        sumc1 = 692.0;
        sumc1soc = 84.0;
        predictedchange = 529.0;
        predictedchangemodel = 14.0;
        predictedchangepenalty = 386.0;
    }
    if( meritstate->rmeritphasestate.stage==0 )
    {
        goto lbl_0;
    }
    if( meritstate->rmeritphasestate.stage==1 )
    {
        goto lbl_1;
    }
    if( meritstate->rmeritphasestate.stage==2 )
    {
        goto lbl_2;
    }
    if( meritstate->rmeritphasestate.stage==3 )
    {
        goto lbl_3;
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
    dotracexd = dotrace&&ae_is_trace_enabled("SQP.DETAILED");
    doprobingalways = ae_is_trace_enabled("SQP.PROBING");
    doprobingonfailure = ae_is_trace_enabled("SQP.PROBINGONFAILURE");
    ae_assert(meritstate->lagbcmult.cnt>=n, "MeritPhaseIteration: integrity check failed", _state);
    ae_assert(meritstate->lagxcmult.cnt>=nec+nic+nlec+nlic, "MeritPhaseIteration: integrity check failed", _state);
    rsetv(nslack, 0.0, &meritstate->d0, _state);
    rsetv(nslack, 0.0, &meritstate->d1, _state);
    
    /*
     * Report iteration beginning
     */
    if( dotrace )
    {
        ae_trace("\n--- quadratic step ---------------------------------------------------------------------------------\n");
    }
    
    /*
     * Default decision is to continue algorithm
     */
    meritstate->status = 1;
    meritstate->increasebigc = ae_false;
    meritstate->increasemeritmu = ae_false;
    meritstate->meritfstagnated = ae_false;
    stp = (double)(0);
    
    /*
     * Determine step direction using initial quadratic model.
     */
    socperformed = ae_false;
    if( !nlcsqp_qpsubproblemsolve(state, &state->subsolver, &meritstate->stepkx, &meritstate->stepkfi, &meritstate->stepkj, &state->hess, 1.0, 1.0, &meritstate->d0, &meritstate->lagbcmult, &meritstate->lagxcmult, &j, &predictedchangemodel, &predictedchangepenalty, &sumc1, _state) )
    {
        if( dotrace )
        {
            ae_trace("> [WARNING] QP subproblem failed with TerminationType=%0d\n",
                (int)(j));
        }
        result = ae_false;
        return result;
    }
    if( dotrace )
    {
        ae_trace("> QP subproblem solved with TerminationType=%0d, max|lagBoxMult|=%0.2e, max|lagNonBoxMult|=%0.2e\n",
            (int)(j),
            (double)(rmaxabsv(n, &meritstate->lagbcmult, _state)),
            (double)(rmaxabsv(nec+nic+nlec+nlic, &meritstate->lagxcmult, _state)));
    }
    rcopyv(nslack, &meritstate->d0, &meritstate->dtrial, _state);
    rcopyv(nslack, &meritstate->d0, &meritstate->deffective, _state);
    nlcsqp_sqpcopystate(state, &meritstate->stepkx, &meritstate->stepkfi, &meritstate->stepkj, &meritstate->curlinx, &meritstate->curlinfi, &meritstate->curlinj, _state);
    
    /*
     * Perform merit function line search.
     *
     * First, we try unit step. If it does not decrease merit function,
     * a second-order correction is tried (helps to combat Maratos effect).
     */
    meritdecreasefailed = ae_false;
    localstp = 1.0;
    f0 = nlcsqp_meritfunction(state, &meritstate->stepkx, &meritstate->stepkfi, &meritstate->lagbcmult, &meritstate->lagxcmult, state->meritmu, &meritstate->tmpmerit, _state);
    for(i=0; i<=n-1; i++)
    {
        meritstate->stepkxn.ptr.p_double[i] = meritstate->stepkx.ptr.p_double[i]+localstp*meritstate->dtrial.ptr.p_double[i];
    }
    nlcsqp_sqpsendx(state, &meritstate->stepkxn, _state);
    state->needfij = ae_true;
    meritstate->rmeritphasestate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    if( !nlcsqp_sqpretrievefij(state, &meritstate->stepkfin, &meritstate->stepkjn, _state) )
    {
        
        /*
         * Failed to retrieve func/Jac, infinities detected
         */
        state->repterminationtype = -8;
        meritstate->status = 0;
        if( dotrace )
        {
            ae_trace("[ERROR] infinities in target/constraints are detected\n");
        }
        result = ae_false;
        return result;
    }
    f1 = nlcsqp_meritfunction(state, &meritstate->stepkxn, &meritstate->stepkfin, &meritstate->lagbcmult, &meritstate->lagxcmult, state->meritmu, &meritstate->tmpmerit, _state);
    predictedchange = predictedchangemodel+state->meritmu*predictedchangepenalty;
    if( dotrace )
    {
        ae_trace("> analyzing change in the merit function: predicted=%0.2e, actual=%0.2e, ratio=%5.2f\n",
            (double)(predictedchange),
            (double)(f1-f0),
            (double)((f1-f0)/predictedchange));
    }
    if( ae_fp_less(f1,f0) )
    {
        goto lbl_4;
    }
    
    /*
     * Full step increases merit function. Let's compute second order
     * correction to the constraint model and recompute trial step D:
     * * use original model of the target
     * * extrapolate model of nonlinear constraints at StepKX+D back to origin
     *
     */
    socperformed = ae_true;
    if( dotrace )
    {
        ae_trace("> step without correction does not provide sufficient decrease in the merit function, preparing second-order correction\n");
    }
    meritstate->stepkfic.ptr.p_double[0] = meritstate->stepkfi.ptr.p_double[0];
    for(j=0; j<=n-1; j++)
    {
        meritstate->stepkjc.ptr.pp_double[0][j] = meritstate->stepkj.ptr.pp_double[0][j];
    }
    for(i=1; i<=nlec+nlic; i++)
    {
        v = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            v = v+meritstate->d0.ptr.p_double[j]*meritstate->stepkj.ptr.pp_double[i][j];
            meritstate->stepkjc.ptr.pp_double[i][j] = meritstate->stepkj.ptr.pp_double[i][j];
        }
        meritstate->stepkfic.ptr.p_double[i] = meritstate->stepkfin.ptr.p_double[i]-v;
    }
    if( !nlcsqp_qpsubproblemsolve(state, &state->subsolver, &meritstate->stepkx, &meritstate->stepkfic, &meritstate->stepkjc, &state->hess, 1.0, 1.0, &meritstate->d1, &meritstate->dummylagbcmult, &meritstate->dummylagxcmult, &j, &predictedchangemodel, &predictedchangepenalty, &sumc1soc, _state) )
    {
        if( dotrace )
        {
            ae_trace("> [WARNING] second-order QP subproblem failed\n");
        }
        result = ae_false;
        return result;
    }
    if( dotrace )
    {
        ae_trace("> QP subproblem solved with TerminationType=%0d, max|lagBoxMult|=%0.2e, max|lagNonBoxMult|=%0.2e\n",
            (int)(j),
            (double)(rmaxabsv(n, &meritstate->dummylagbcmult, _state)),
            (double)(rmaxabsv(nec+nic+nlec+nlic, &meritstate->dummylagxcmult, _state)));
    }
    rcopyv(n, &meritstate->d1, &meritstate->dtrial, _state);
    rcopyv(n, &meritstate->d1, &meritstate->deffective, _state);
    nlcsqp_sqpcopystate(state, &meritstate->stepkx, &meritstate->stepkfic, &meritstate->stepkjc, &meritstate->curlinx, &meritstate->curlinfi, &meritstate->curlinj, _state);
    
    /*
     * Perform line search, we again try full step (maybe it will work after SOC)
     */
    localstp = 1.0;
    nu = 0.5;
    f1 = f0;
    smoothnessmonitorstartlinesearch(smonitor, &meritstate->stepkx, &meritstate->stepkfi, &meritstate->stepkj, state->repiterationscount, -1, _state);
lbl_6:
    if( ae_false )
    {
        goto lbl_7;
    }
    for(i=0; i<=n-1; i++)
    {
        meritstate->stepkxn.ptr.p_double[i] = meritstate->stepkx.ptr.p_double[i]+localstp*meritstate->dtrial.ptr.p_double[i];
    }
    nlcsqp_sqpsendx(state, &meritstate->stepkxn, _state);
    state->needfij = ae_true;
    meritstate->rmeritphasestate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfij = ae_false;
    if( !nlcsqp_sqpretrievefij(state, &meritstate->stepkfin, &meritstate->stepkjn, _state) )
    {
        
        /*
         * Failed to retrieve func/Jac, infinities detected
         */
        state->repterminationtype = -8;
        meritstate->status = 0;
        if( dotrace )
        {
            ae_trace("[ERROR] infinities in target/constraints are detected\n");
        }
        result = ae_false;
        return result;
    }
    smoothnessmonitorenqueuepoint(smonitor, &meritstate->dtrial, localstp, &meritstate->stepkxn, &meritstate->stepkfin, &meritstate->stepkjn, _state);
    f1 = nlcsqp_meritfunction(state, &meritstate->stepkxn, &meritstate->stepkfin, &meritstate->lagbcmult, &meritstate->lagxcmult, state->meritmu, &meritstate->tmpmerit, _state);
    predictedchange = predictedchangemodel+state->meritmu*predictedchangepenalty;
    if( dotrace )
    {
        ae_trace("> analyzing change in the merit function: predicted=%0.2e, actual=%0.2e, ratio=%5.2f\n",
            (double)(predictedchange),
            (double)(f1-f0),
            (double)((f1-f0)/predictedchange));
    }
    if( ae_fp_less(f1,f0) )
    {
        
        /*
         * Step is found!
         */
        goto lbl_7;
    }
    
    /*
     * Step is shorter than 0.001 times current search direction,
     * it means that no good step can be found.
     */
    localstp = (double)(0);
    meritdecreasefailed = ae_true;
    f1 = f0;
    nlcsqp_sqpcopystate(state, &meritstate->stepkx, &meritstate->stepkfi, &meritstate->stepkj, &meritstate->stepkxn, &meritstate->stepkfin, &meritstate->stepkjn, _state);
    rsetv(nslack, 0.0, &meritstate->deffective, _state);
    goto lbl_7;
    localstp = nu*localstp;
    nu = ae_maxreal(0.1, 0.5*nu, _state);
    goto lbl_6;
lbl_7:
    smoothnessmonitorfinalizelinesearch(smonitor, _state);
lbl_4:
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i] )
        {
            meritstate->stepkxn.ptr.p_double[i] = ae_maxreal(meritstate->stepkxn.ptr.p_double[i], state->scaledbndl.ptr.p_double[i], _state);
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            meritstate->stepkxn.ptr.p_double[i] = ae_minreal(meritstate->stepkxn.ptr.p_double[i], state->scaledbndu.ptr.p_double[i], _state);
        }
    }
    if( userterminationneeded )
    {
        
        /*
         * User requested termination, break before we move to new point
         */
        state->repterminationtype = 8;
        meritstate->status = 0;
        if( dotrace )
        {
            ae_trace("> user requested termination\n");
        }
        result = ae_false;
        return result;
    }
    nlcsqp_lagrangianfg(state, &meritstate->stepkx, state->trustrad, &meritstate->stepkfi, &meritstate->stepkj, &meritstate->lagbcmult, &meritstate->lagxcmult, ae_true, &meritstate->tmplagrangianfg, &stepklagval, &meritstate->stepklaggrad, _state);
    nlcsqp_lagrangianfg(state, &meritstate->stepkxn, state->trustrad, &meritstate->stepkfin, &meritstate->stepkjn, &meritstate->lagbcmult, &meritstate->lagxcmult, ae_true, &meritstate->tmplagrangianfg, &stepknlagval, &meritstate->stepknlaggrad, _state);
    
    /*
     * Analyze merit F for stagnation
     */
    meritstate->meritfstagnated = ae_fp_neq(localstp,(double)(0))&&ae_fp_less_eq(ae_fabs(f1-f0, _state),nlcsqp_stagnationepsf*ae_fabs(f0, _state));
    
    /*
     * Analyze linearized model - did we enforce zero violations of the constraint linearizations?
     * If we did not, we may need to increase BigC/MeritMu penalty coefficients.
     */
    if( nlcsqp_penaltiesneedincrease(state, meritstate, dotrace, &meritstate->increasebigc, &meritstate->increasemeritmu, _state) )
    {
        localstp = 0.0;
        rsetv(nslack, 0.0, &meritstate->deffective, _state);
        nlcsqp_sqpcopystate(state, &meritstate->stepkx, &meritstate->stepkfi, &meritstate->stepkj, &meritstate->stepkxn, &meritstate->stepkfin, &meritstate->stepkjn, _state);
    }
    
    /*
     * Decide whether we want to request increase BigC (a constraint enforcing multiplier for L1 penalized
     * QP subproblem) or not.
     *
     * An increase is NOT needed if at least one of the following holds:
     * * present value of BigC is already nearly maximum
     * * a long step was proposed
     * * any single constraint can be made feasible within box with radius slightly larger max|D|
     *
     * Thus, BigC is requested to be increased if a short step was made, but there are some
     * constraints that are infeasible within max|D|-sized box
     */
    if( ae_fp_less(rmaxabsv(n, &meritstate->dtrial, _state),0.9*state->trustrad)&&ae_fp_less(state->bigc,0.9*nlcsqp_maxbigc) )
    {
        expandedrad = 1.1*rmaxabsv(n, &meritstate->dtrial, _state);
        tol = ae_maxreal(ae_sqrt(ae_machineepsilon, _state)*state->trustrad, (double)1000*ae_machineepsilon, _state);
        for(i=0; i<=nec+nic-1; i++)
        {
            v = (double)(0);
            vv = (double)(0);
            for(j=0; j<=n-1; j++)
            {
                v = v+state->scaledcleic.ptr.pp_double[i][j]*state->stepkx.ptr.p_double[j];
                vv = vv+ae_fabs(state->scaledcleic.ptr.pp_double[i][j]*expandedrad, _state);
            }
            v = v-state->scaledcleic.ptr.pp_double[i][n];
            if( i>=nec )
            {
                v = ae_maxreal(v, 0.0, _state);
            }
            meritstate->increasebigc = meritstate->increasebigc||ae_fp_greater(ae_fabs(v, _state),vv+tol);
        }
        for(i=1; i<=nlec+nlic; i++)
        {
            v = state->stepkfi.ptr.p_double[i];
            vv = (double)(0);
            for(j=0; j<=n-1; j++)
            {
                vv = vv+ae_fabs(state->stepkj.ptr.pp_double[i][j]*expandedrad, _state);
            }
            if( i>=nlec+1 )
            {
                v = ae_maxreal(v, 0.0, _state);
            }
            meritstate->increasebigc = meritstate->increasebigc||ae_fp_greater(ae_fabs(v, _state),vv+tol);
        }
    }
    
    /*
     * Trace
     */
    if( !dotrace )
    {
        goto lbl_8;
    }
    
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
        v = meritstate->stepkxn.ptr.p_double[j]-meritstate->stepkx.ptr.p_double[j];
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
                v = meritstate->stepkxn.ptr.p_double[j]-meritstate->stepkx.ptr.p_double[j];
                vv = meritstate->stepkjn.ptr.pp_double[i][j]-meritstate->stepkj.ptr.pp_double[i][j];
                skyk = skyk+v*vv;
                ykyk = ykyk+vv*vv;
            }
            v = ykyk/(ae_fabs(skyk, _state)+ae_machineepsilon*ykyk+ae_machineepsilon*sksk);
            state->tracegamma.ptr.p_double[i] = ae_maxreal(state->tracegamma.ptr.p_double[i], v, _state);
        }
    }
    
    /*
     * Perform agressive probing of the search direction - additional function evaluations
     * which help us to determine possible discontinuity and nonsmoothness of the problem
     */
    if( !(doprobingalways||(doprobingonfailure&&meritdecreasefailed)) )
    {
        goto lbl_10;
    }
    didx = 0;
lbl_12:
    if( didx>1 )
    {
        goto lbl_14;
    }
    if( didx==1&&!socperformed )
    {
        goto lbl_14;
    }
    if( didx==0 )
    {
        smoothnessmonitorstartlagrangianprobing(smonitor, &meritstate->stepkx, &meritstate->d0, 1.0, state->repiterationscount, -1, _state);
    }
    else
    {
        smoothnessmonitorstartlagrangianprobing(smonitor, &meritstate->stepkx, &meritstate->d1, 1.0, state->repiterationscount, -1, _state);
    }
lbl_15:
    if( !smoothnessmonitorprobelagrangian(smonitor, _state) )
    {
        goto lbl_16;
    }
    for(j=0; j<=n-1; j++)
    {
        meritstate->stepkxc.ptr.p_double[j] = smonitor->lagprobx.ptr.p_double[j];
        if( state->hasbndl.ptr.p_bool[j] )
        {
            meritstate->stepkxc.ptr.p_double[j] = ae_maxreal(meritstate->stepkxc.ptr.p_double[j], state->scaledbndl.ptr.p_double[j], _state);
        }
        if( state->hasbndu.ptr.p_bool[j] )
        {
            meritstate->stepkxc.ptr.p_double[j] = ae_minreal(meritstate->stepkxc.ptr.p_double[j], state->scaledbndu.ptr.p_double[j], _state);
        }
    }
    nlcsqp_sqpsendx(state, &meritstate->stepkxc, _state);
    state->needfij = ae_true;
    meritstate->rmeritphasestate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfij = ae_false;
    if( !nlcsqp_sqpretrievefij(state, &smonitor->lagprobfi, &smonitor->lagprobj, _state) )
    {
        goto lbl_16;
    }
    smonitor->lagprobrawlag = nlcsqp_rawlagrangian(state, &meritstate->stepkxc, &smonitor->lagprobfi, &meritstate->lagbcmult, &meritstate->lagxcmult, state->meritmu, &meritstate->tmpmerit, _state);
    goto lbl_15;
lbl_16:
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
    goto lbl_12;
lbl_14:
lbl_10:
    
    /*
     * Output other information
     */
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(meritstate->dtrial.ptr.p_double[i], _state)/state->trustrad, _state);
    }
    if( ae_fp_greater(localstp,(double)(0)) )
    {
        ae_trace("> nonzero linear step was performed\n");
    }
    else
    {
        ae_trace("> zero linear step was performed\n");
    }
    ae_trace("max(|Di|)/TrustRad = %0.6f\n",
        (double)(mx));
    ae_trace("stp = %0.6f\n",
        (double)(localstp));
    if( dotracexd )
    {
        ae_trace("LagBoxMlt   = ");
        tracevectorautoprec(&meritstate->lagbcmult, 0, n, _state);
        ae_trace("\n");
        ae_trace("LagNonBoxMlt= ");
        tracevectorautoprec(&meritstate->lagxcmult, 0, nec+nic+nlec+nlic, _state);
        ae_trace("\n");
    }
    if( dotracexd )
    {
        ae_trace("X0 (scaled) = ");
        tracevectorautoprec(&meritstate->stepkx, 0, n, _state);
        ae_trace("\n");
        ae_trace("D  (scaled) = ");
        tracevectorautoprec(&meritstate->dtrial, 0, n, _state);
        ae_trace("\n");
        ae_trace("X1 (scaled) = ");
        tracevectorautoprec(&meritstate->stepkxn, 0, n, _state);
        ae_trace("\n");
        ae_trace("\n");
        ae_trace("grad F(X0)  = ");
        tracerowautoprec(&meritstate->stepkj, 0, 0, n, _state);
        ae_trace("\n");
        ae_trace("grad F(X1)  = ");
        tracerowautoprec(&meritstate->stepkj, 0, 0, n, _state);
        ae_trace("\n");
        ae_trace("\n");
        ae_trace("grad L(X0)  = ");
        tracevectorautoprec(&meritstate->stepklaggrad, 0, n, _state);
        ae_trace("\n");
        ae_trace("grad L(X1)  = ");
        tracevectorautoprec(&meritstate->stepknlaggrad, 0, n, _state);
        ae_trace("\n");
    }
    ae_trace("targetF:        %14.6e -> %14.6e (delta=%11.3e)\n",
        (double)(state->fscales.ptr.p_double[0]*meritstate->stepkfi.ptr.p_double[0]),
        (double)(state->fscales.ptr.p_double[0]*meritstate->stepkfin.ptr.p_double[0]),
        (double)(state->fscales.ptr.p_double[0]*(meritstate->stepkfin.ptr.p_double[0]-meritstate->stepkfi.ptr.p_double[0])));
    ae_trace("scaled-meritF:  %14.6e -> %14.6e (delta=%11.3e)\n",
        (double)(f0),
        (double)(f1),
        (double)(f1-f0));
    ae_trace("scaled-targetF: %14.6e -> %14.6e (delta=%11.3e)\n",
        (double)(meritstate->stepkfi.ptr.p_double[0]),
        (double)(meritstate->stepkfin.ptr.p_double[0]),
        (double)(meritstate->stepkfin.ptr.p_double[0]-meritstate->stepkfi.ptr.p_double[0]));
    ae_trace("max|lag-grad|:  %14.6e -> %14.6e\n",
        (double)(rmaxabsv(n, &meritstate->stepklaggrad, _state)),
        (double)(rmaxabsv(n, &meritstate->stepknlaggrad, _state)));
    ae_trace("nrm2|lag-grad|: %14.6e -> %14.6e (ratio=%0.6f)\n",
        (double)(ae_sqrt(rdotv2(n, &meritstate->stepklaggrad, _state), _state)),
        (double)(ae_sqrt(rdotv2(n, &meritstate->stepknlaggrad, _state), _state)),
        (double)(ae_sqrt(rdotv2(n, &meritstate->stepknlaggrad, _state), _state)/ae_sqrt(rdotv2(n, &meritstate->stepklaggrad, _state), _state)));
    hessiangetdiagonal(&state->hess, &meritstate->tmphdiag, _state);
    v = meritstate->tmphdiag.ptr.p_double[0];
    for(i=0; i<=n-1; i++)
    {
        v = ae_minreal(v, meritstate->tmphdiag.ptr.p_double[i], _state);
    }
    ae_trace("mindiag(Bk) = %0.3e\n",
        (double)(v));
    v = meritstate->tmphdiag.ptr.p_double[0];
    for(i=0; i<=n-1; i++)
    {
        v = ae_maxreal(v, meritstate->tmphdiag.ptr.p_double[i], _state);
    }
    ae_trace("maxdiag(Bk) = %0.3e\n",
        (double)(v));
lbl_8:
    
    /*
     * Perform Hessian update
     */
    if( ae_fp_greater(localstp,(double)(0)) )
    {
        hessianupdate(&state->hess, &meritstate->stepkx, &meritstate->stepklaggrad, &meritstate->stepkxn, &meritstate->stepknlaggrad, dotrace, _state);
    }
    if( dotrace )
    {
        if( state->hess.updatestatus>0 )
        {
            ae_trace("> Hessian updated");
            if( state->hess.updatestatus==1 )
            {
                ae_trace(" (old curvature removed, no new curvature added)\n");
            }
            if( state->hess.updatestatus==2 )
            {
                ae_trace(" (normal update)\n");
            }
            if( state->hess.updatestatus==3 )
            {
                ae_trace(" (periodic reset performed)\n");
            }
            hessiangetdiagonal(&state->hess, &meritstate->tmphdiag, _state);
            v = meritstate->tmphdiag.ptr.p_double[0];
            for(i=0; i<=n-1; i++)
            {
                v = ae_minreal(v, meritstate->tmphdiag.ptr.p_double[i], _state);
            }
            ae_trace("mindiag(Bk) = %0.3e\n",
                (double)(v));
            v = meritstate->tmphdiag.ptr.p_double[0];
            for(i=0; i<=n-1; i++)
            {
                v = ae_maxreal(v, meritstate->tmphdiag.ptr.p_double[i], _state);
            }
            ae_trace("maxdiag(Bk) = %0.3e\n",
                (double)(v));
        }
        else
        {
            ae_trace("> skipping Hessian update\n");
        }
    }
    
    /*
     * Move to new point
     */
    stp = localstp;
    if( ae_fp_less_eq(localstp,(double)(0)) )
    {
        goto lbl_17;
    }
    
    /*
     * Update current state
     */
    nlcsqp_sqpcopystate(state, &meritstate->stepkxn, &meritstate->stepkfin, &meritstate->stepkjn, &meritstate->stepkx, &meritstate->stepkfi, &meritstate->stepkj, _state);
    
    /*
     * Report one more inner iteration
     */
    nlcsqp_sqpsendx(state, &meritstate->stepkx, _state);
    state->f = meritstate->stepkfi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    meritstate->rmeritphasestate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->xupdated = ae_false;
    
    /*
     * Update constraint violations
     */
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, &meritstate->stepkx, n, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlcviolation(&meritstate->stepkfi, &state->fscales, nlec, nlic, &state->repnlcerr, &state->repnlcidx, _state);
lbl_17:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    meritstate->rmeritphasestate.ia.ptr.p_int[0] = n;
    meritstate->rmeritphasestate.ia.ptr.p_int[1] = nslack;
    meritstate->rmeritphasestate.ia.ptr.p_int[2] = nec;
    meritstate->rmeritphasestate.ia.ptr.p_int[3] = nic;
    meritstate->rmeritphasestate.ia.ptr.p_int[4] = nlec;
    meritstate->rmeritphasestate.ia.ptr.p_int[5] = nlic;
    meritstate->rmeritphasestate.ia.ptr.p_int[6] = i;
    meritstate->rmeritphasestate.ia.ptr.p_int[7] = j;
    meritstate->rmeritphasestate.ia.ptr.p_int[8] = didx;
    meritstate->rmeritphasestate.ba.ptr.p_bool[0] = dotrace;
    meritstate->rmeritphasestate.ba.ptr.p_bool[1] = doprobingalways;
    meritstate->rmeritphasestate.ba.ptr.p_bool[2] = doprobingonfailure;
    meritstate->rmeritphasestate.ba.ptr.p_bool[3] = dotracexd;
    meritstate->rmeritphasestate.ba.ptr.p_bool[4] = socperformed;
    meritstate->rmeritphasestate.ba.ptr.p_bool[5] = meritdecreasefailed;
    meritstate->rmeritphasestate.ra.ptr.p_double[0] = v;
    meritstate->rmeritphasestate.ra.ptr.p_double[1] = vv;
    meritstate->rmeritphasestate.ra.ptr.p_double[2] = mx;
    meritstate->rmeritphasestate.ra.ptr.p_double[3] = f0;
    meritstate->rmeritphasestate.ra.ptr.p_double[4] = f1;
    meritstate->rmeritphasestate.ra.ptr.p_double[5] = nu;
    meritstate->rmeritphasestate.ra.ptr.p_double[6] = localstp;
    meritstate->rmeritphasestate.ra.ptr.p_double[7] = tol;
    meritstate->rmeritphasestate.ra.ptr.p_double[8] = stepklagval;
    meritstate->rmeritphasestate.ra.ptr.p_double[9] = stepknlagval;
    meritstate->rmeritphasestate.ra.ptr.p_double[10] = stp;
    meritstate->rmeritphasestate.ra.ptr.p_double[11] = expandedrad;
    meritstate->rmeritphasestate.ra.ptr.p_double[12] = sksk;
    meritstate->rmeritphasestate.ra.ptr.p_double[13] = ykyk;
    meritstate->rmeritphasestate.ra.ptr.p_double[14] = skyk;
    meritstate->rmeritphasestate.ra.ptr.p_double[15] = sumc1;
    meritstate->rmeritphasestate.ra.ptr.p_double[16] = sumc1soc;
    meritstate->rmeritphasestate.ra.ptr.p_double[17] = predictedchange;
    meritstate->rmeritphasestate.ra.ptr.p_double[18] = predictedchangemodel;
    meritstate->rmeritphasestate.ra.ptr.p_double[19] = predictedchangepenalty;
    return result;
}


/*************************************************************************
This function initializes MeritPhase temporaries. It should be called before
beginning of each new iteration. You may call it multiple  times  for  the
same instance of MeritPhase temporaries.

INPUT PARAMETERS:
    MeritState          -   instance to be initialized.
    N                   -   problem dimensionality
    NEC, NIC            -   linear equality/inequality constraint count
    NLEC, NLIC          -   nonlinear equality/inequality constraint count

OUTPUT PARAMETERS:
    IncreaseBigC        -   whether increasing BigC is suggested (we detected
                            infeasible constraints that are NOT improved)
                            or not.
    IncreaseMeritMu     -   whether increasing MeritMu is suggested or not.
    MeritFStagnated     -   whether merit function stagnated during current
                            iteration or not

  -- ALGLIB --
     Copyright 23.03.2023 by Bochkanov Sergey
*************************************************************************/
static void nlcsqp_meritphaseresults(const minsqpmeritphasestate* meritstate,
     /* Real    */ ae_vector* curx,
     /* Real    */ ae_vector* curfi,
     /* Real    */ ae_matrix* curj,
     ae_bool* increasebigc,
     ae_bool* increasemeritmu,
     ae_bool* meritfstagnated,
     ae_int_t* status,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;


    *increasebigc = meritstate->increasebigc;
    *increasemeritmu = meritstate->increasemeritmu;
    *meritfstagnated = meritstate->meritfstagnated;
    *status = meritstate->status;
    for(i=0; i<=meritstate->n-1; i++)
    {
        curx->ptr.p_double[i] = meritstate->stepkx.ptr.p_double[i];
    }
    for(i=0; i<=meritstate->nlec+meritstate->nlic; i++)
    {
        curfi->ptr.p_double[i] = meritstate->stepkfi.ptr.p_double[i];
        for(j=0; j<=meritstate->n-1; j++)
        {
            curj->ptr.pp_double[i][j] = meritstate->stepkj.ptr.pp_double[i][j];
        }
    }
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
     /* Real    */ ae_vector* fis,
     /* Real    */ ae_matrix* js,
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
        fis->ptr.p_double[i] = vv*state->fi.ptr.p_double[i];
        v = v+fis->ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            js->ptr.pp_double[i][j] = vv*state->j.ptr.pp_double[i][j];
            v = v+js->ptr.pp_double[i][j];
        }
    }
    result = ae_isfinite(v, _state);
    return result;
}


/*************************************************************************
Copies state (X point, Fi vector, J jacobian) to preallocated storage.
*************************************************************************/
static void nlcsqp_sqpcopystate(const minsqpstate* state,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* fi0,
     /* Real    */ const ae_matrix* j0,
     /* Real    */ ae_vector* x1,
     /* Real    */ ae_vector* fi1,
     /* Real    */ ae_matrix* j1,
     ae_state *_state)
{
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;


    n = state->n;
    nlec = state->nlec;
    nlic = state->nlic;
    for(i=0; i<=n-1; i++)
    {
        x1->ptr.p_double[i] = x0->ptr.p_double[i];
    }
    for(i=0; i<=nlec+nlic; i++)
    {
        fi1->ptr.p_double[i] = fi0->ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            j1->ptr.pp_double[i][j] = j0->ptr.pp_double[i][j];
        }
    }
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
     minsqptmplagrangian* tmp,
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
        rvectorsetlengthatleast(&tmp->sclagtmp0, ae_maxint(nec+nic, n, _state), _state);
        rvectorsetlengthatleast(&tmp->sclagtmp1, ae_maxint(nec+nic, n, _state), _state);
        if( usesparsegemv )
        {
            sparsemv(&state->subsolver.sparserawlc, x, &tmp->sclagtmp0, _state);
        }
        else
        {
            rmatrixgemv(nec+nic, n, 1.0, &state->scaledcleic, 0, 0, 0, x, 0, 0.0, &tmp->sclagtmp0, 0, _state);
        }
        for(i=0; i<=nec+nic-1; i++)
        {
            
            /*
             * Prepare
             */
            v = tmp->sclagtmp0.ptr.p_double[i]-state->scaledcleic.ptr.pp_double[i][n];
            vlag = lagalpha*lagxcmult->ptr.p_double[i];
            tmp->sclagtmp1.ptr.p_double[i] = (double)(0);
            
            /*
             * Primary Lagrangian term
             */
            vact = v;
            vd = (double)(1);
            *f = *f+vlag*vact;
            tmp->sclagtmp1.ptr.p_double[i] = tmp->sclagtmp1.ptr.p_double[i]+vlag*vd;
            
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
            tmp->sclagtmp1.ptr.p_double[i] = tmp->sclagtmp1.ptr.p_double[i]+nlcsqp_augmentationfactor*vact;
        }
        if( usesparsegemv )
        {
            sparsemtv(&state->subsolver.sparserawlc, &tmp->sclagtmp1, &tmp->sclagtmp0, _state);
            for(i=0; i<=n-1; i++)
            {
                g->ptr.p_double[i] = g->ptr.p_double[i]+tmp->sclagtmp0.ptr.p_double[i];
            }
        }
        else
        {
            rmatrixgemv(n, nec+nic, 1.0, &state->scaledcleic, 0, 0, 1, &tmp->sclagtmp1, 0, 1.0, g, 0, _state);
        }
    }
    
    /*
     * Lagrangian terms for nonlinear constraints
     */
    rvectorsetlengthatleast(&tmp->sclagtmp1, nlec+nlic, _state);
    for(i=0; i<=nlec+nlic-1; i++)
    {
        v = fi->ptr.p_double[1+i];
        vlag = lagalpha*lagxcmult->ptr.p_double[nec+nic+i];
        tmp->sclagtmp1.ptr.p_double[i] = (double)(0);
        
        /*
         * Lagrangian term
         */
        vact = v;
        vd = (double)(1);
        *f = *f+vlag*vact;
        tmp->sclagtmp1.ptr.p_double[i] = tmp->sclagtmp1.ptr.p_double[i]+vlag*vd;
        
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
        tmp->sclagtmp1.ptr.p_double[i] = tmp->sclagtmp1.ptr.p_double[i]+nlcsqp_augmentationfactor*vact;
    }
    rmatrixgemv(n, nlec+nlic, 1.0, j, 1, 0, 1, &tmp->sclagtmp1, 0, 1.0, g, 0, _state);
}


/*************************************************************************
This function calculates L1-penalized merit function
*************************************************************************/
static double nlcsqp_meritfunction(minsqpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagbcmult,
     /* Real    */ const ae_vector* lagxcmult,
     double meritmu,
     minsqptmpmerit* tmp,
     ae_state *_state)
{
    double tmp0;
    double tmp1;
    double result;


    nlcsqp_meritfunctionandrawlagrangian(state, x, fi, lagbcmult, lagxcmult, meritmu, tmp, &tmp0, &tmp1, _state);
    result = tmp0;
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
     minsqptmpmerit* tmp,
     ae_state *_state)
{
    double tmp0;
    double tmp1;
    double result;


    nlcsqp_meritfunctionandrawlagrangian(state, x, fi, lagbcmult, lagxcmult, meritmu, tmp, &tmp0, &tmp1, _state);
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
     minsqptmpmerit* tmp,
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
    rvectorsetlengthatleast(&tmp->mftmp0, nec+nic, _state);
    rmatrixgemv(nec+nic, n, 1.0, &state->scaledcleic, 0, 0, 0, x, 0, 0.0, &tmp->mftmp0, 0, _state);
    for(i=0; i<=nec+nic-1; i++)
    {
        v = tmp->mftmp0.ptr.p_double[i]-state->scaledcleic.ptr.pp_double[i][n];
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
point. Depending on outcome, proposes to increase BigC, MeritMu or both.

If no update is necessary (the best outcome), False will be returned.

  -- ALGLIB --
     Copyright 13.03.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcsqp_penaltiesneedincrease(minsqpstate* state,
     minsqpmeritphasestate* meritstate,
     ae_bool dotrace,
     ae_bool* increasebigc,
     ae_bool* increasemeritmu,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t i;
    ae_int_t j;
    ae_int_t tt;
    double tol;
    double constrval;
    double allowederr;
    ae_bool failedtoenforce;
    ae_bool failedtoenforceinf;
    double dummy0;
    double dummy1;
    double dummy2;
    double penaltyatx;
    double penaltyatd;
    double penaltyatdinf;
    double modeldecrease;
    ae_bool canincreasebigc;
    ae_bool canincreasemeritmu;
    ae_bool result;

    *increasebigc = ae_false;
    *increasemeritmu = ae_false;

    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    result = ae_false;
    *increasebigc = ae_false;
    *increasemeritmu = ae_false;
    canincreasebigc = ae_fp_less(state->bigc,0.9*nlcsqp_maxbigc);
    canincreasemeritmu = ae_fp_less(state->meritmu,0.9*nlcsqp_maxmeritmu);
    tol = 1.0E-6*state->trustrad;
    
    /*
     * Already too big, skip analysis
     */
    if( !canincreasebigc&&!canincreasemeritmu )
    {
        return result;
    }
    
    /*
     * Compute penalty at X
     */
    penaltyatx = (double)(0);
    for(i=0; i<=nec+nic-1; i++)
    {
        constrval = -state->scaledcleic.ptr.pp_double[i][n];
        for(j=0; j<=n-1; j++)
        {
            constrval = constrval+state->scaledcleic.ptr.pp_double[i][j]*meritstate->curlinx.ptr.p_double[j];
        }
        if( i>=nec )
        {
            constrval = ae_maxreal(constrval, 0.0, _state);
        }
        penaltyatx = penaltyatx+ae_fabs(constrval, _state);
    }
    for(i=0; i<=nlec+nlic-1; i++)
    {
        constrval = meritstate->curlinfi.ptr.p_double[1+i];
        if( i>=nlec )
        {
            constrval = ae_maxreal(constrval, 0.0, _state);
        }
        penaltyatx = penaltyatx+ae_fabs(constrval, _state);
    }
    
    /*
     * Did we fail to enforce nearly zero linearization errors?
     */
    penaltyatd = (double)(0);
    failedtoenforce = ae_false;
    for(i=0; i<=nec+nic-1; i++)
    {
        constrval = -state->scaledcleic.ptr.pp_double[i][n];
        allowederr = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            constrval = constrval+state->scaledcleic.ptr.pp_double[i][j]*(meritstate->curlinx.ptr.p_double[j]+meritstate->dtrial.ptr.p_double[j]);
            allowederr = allowederr+ae_fabs(state->scaledcleic.ptr.pp_double[i][j]*tol, _state);
        }
        if( i>=nec )
        {
            constrval = ae_maxreal(constrval, 0.0, _state);
        }
        penaltyatd = penaltyatd+ae_fabs(constrval, _state);
        failedtoenforce = failedtoenforce||ae_fp_greater(ae_fabs(constrval, _state),allowederr);
    }
    for(i=0; i<=nlec+nlic-1; i++)
    {
        constrval = meritstate->curlinfi.ptr.p_double[1+i];
        allowederr = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            constrval = constrval+meritstate->curlinj.ptr.pp_double[1+i][j]*meritstate->dtrial.ptr.p_double[j];
            allowederr = allowederr+ae_fabs(meritstate->curlinj.ptr.pp_double[1+i][j]*tol, _state);
        }
        if( i>=nlec )
        {
            constrval = ae_maxreal(constrval, 0.0, _state);
        }
        penaltyatd = penaltyatd+ae_fabs(constrval, _state);
        failedtoenforce = failedtoenforce||ae_fp_greater(ae_fabs(constrval, _state),allowederr);
    }
    
    /*
     * We may need to increase BigC
     */
    if( canincreasebigc )
    {
        if( failedtoenforce )
        {
            
            /*
             * We failed to drive constraints linearizations to zero. But what is the best
             * theoretically possible reduction (ignoring quadratic model)?
             */
            if( !nlcsqp_qpsubproblemsolve(state, &state->subsolver, &meritstate->curlinx, &meritstate->curlinfi, &meritstate->curlinj, &state->hess, 0.0, 0.0, &meritstate->dmu, &meritstate->dummylagbcmult, &meritstate->dummylagxcmult, &tt, &dummy0, &dummy1, &dummy2, _state) )
            {
                if( dotrace )
                {
                    ae_trace("> [WARNING] QP subproblem failed with TerminationType=%0d when updating BigC\n",
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
                    constrval = constrval+state->scaledcleic.ptr.pp_double[i][j]*(state->stepkx.ptr.p_double[j]+meritstate->dmu.ptr.p_double[j]);
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
                constrval = meritstate->curlinfi.ptr.p_double[1+i];
                allowederr = (double)(0);
                for(j=0; j<=n-1; j++)
                {
                    constrval = constrval+meritstate->curlinj.ptr.pp_double[1+i][j]*meritstate->dmu.ptr.p_double[j];
                    allowederr = allowederr+ae_fabs(meritstate->curlinj.ptr.pp_double[1+i][j]*tol, _state);
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
                 * Constraints are infeasible within trust region, check that the actual penalty decrease
                 * is at least some fraction of the best possible decrease.
                 */
                if( ae_fp_less(penaltyatx-penaltyatd,nlcsqp_bigceps*ae_maxreal(penaltyatx-penaltyatdinf, 0.0, _state)) )
                {
                    
                    /*
                     * Merit function does not decrease fast enough, we must increase BigC
                     */
                    result = ae_true;
                    *increasebigc = ae_true;
                    if( dotrace )
                    {
                        ae_trace("> constraints infeasibility does not decrease fast enough, BigC increased, iteration skipped\n");
                    }
                }
            }
            else
            {
                
                /*
                 * Constraints are feasible within trust region, we must increase BigC
                 */
                result = ae_true;
                *increasebigc = ae_true;
                if( dotrace )
                {
                    ae_trace("> BigC penalty does not enforce constraints feasibility, BigC increased, iteration skipped\n");
                }
            }
        }
    }
    
    /*
     * We may need to increase MeritMu
     */
    if( canincreasemeritmu )
    {
        
        /*
         * Now we check that decrease in the quadratic model + penalty is a significant fraction of
         * the decrease in the penalty.
         */
        modeldecrease = -(rdotvr(n, &meritstate->dtrial, &meritstate->curlinj, 0, _state)+0.5*hessianvmv(&state->hess, &meritstate->dtrial, _state));
        if( ae_fp_less(modeldecrease+state->meritmu*(penaltyatx-penaltyatd),nlcsqp_meritmueps*state->meritmu*(penaltyatx-penaltyatd)) )
        {
            result = ae_true;
            *increasemeritmu = ae_true;
            if( dotrace )
            {
                ae_trace("> sufficient negativity condition does not hold, MeritMu increased, iteration skipped\n");
            }
        }
    }
    return result;
}


void _minsqpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minsqpsubsolver *p = (minsqpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _vipmstate_init(&p->ipmsolver, _state, make_automatic);
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
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->retainnegativebclm, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->retainpositivebclm, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->rescalelag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasal, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasau, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->activea, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->activerhs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->activeidx, 0, DT_INT, _state, make_automatic);
}


void _minsqpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minsqpsubsolver       *dst = (minsqpsubsolver*)_dst;
    const minsqpsubsolver *src = (const minsqpsubsolver*)_src;
    dst->algokind = src->algokind;
    _vipmstate_init_copy(&dst->ipmsolver, &src->ipmsolver, _state, make_automatic);
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
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->retainnegativebclm, &src->retainnegativebclm, _state, make_automatic);
    ae_vector_init_copy(&dst->retainpositivebclm, &src->retainpositivebclm, _state, make_automatic);
    ae_vector_init_copy(&dst->rescalelag, &src->rescalelag, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasal, &src->hasal, _state, make_automatic);
    ae_vector_init_copy(&dst->hasau, &src->hasau, _state, make_automatic);
    ae_matrix_init_copy(&dst->activea, &src->activea, _state, make_automatic);
    ae_vector_init_copy(&dst->activerhs, &src->activerhs, _state, make_automatic);
    ae_vector_init_copy(&dst->activeidx, &src->activeidx, _state, make_automatic);
    dst->activesetsize = src->activesetsize;
}


void _minsqpsubsolver_clear(void* _p)
{
    minsqpsubsolver *p = (minsqpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _vipmstate_clear(&p->ipmsolver);
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
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmp2);
    ae_vector_clear(&p->retainnegativebclm);
    ae_vector_clear(&p->retainpositivebclm);
    ae_vector_clear(&p->rescalelag);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->hasal);
    ae_vector_clear(&p->hasau);
    ae_matrix_clear(&p->activea);
    ae_vector_clear(&p->activerhs);
    ae_vector_clear(&p->activeidx);
}


void _minsqpsubsolver_destroy(void* _p)
{
    minsqpsubsolver *p = (minsqpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _vipmstate_destroy(&p->ipmsolver);
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
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmp2);
    ae_vector_destroy(&p->retainnegativebclm);
    ae_vector_destroy(&p->retainpositivebclm);
    ae_vector_destroy(&p->rescalelag);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->hasal);
    ae_vector_destroy(&p->hasau);
    ae_matrix_destroy(&p->activea);
    ae_vector_destroy(&p->activerhs);
    ae_vector_destroy(&p->activeidx);
}


void _minsqptmplagrangian_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minsqptmplagrangian *p = (minsqptmplagrangian*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->sclagtmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sclagtmp1, 0, DT_REAL, _state, make_automatic);
}


void _minsqptmplagrangian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minsqptmplagrangian       *dst = (minsqptmplagrangian*)_dst;
    const minsqptmplagrangian *src = (const minsqptmplagrangian*)_src;
    ae_vector_init_copy(&dst->sclagtmp0, &src->sclagtmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->sclagtmp1, &src->sclagtmp1, _state, make_automatic);
}


void _minsqptmplagrangian_clear(void* _p)
{
    minsqptmplagrangian *p = (minsqptmplagrangian*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->sclagtmp0);
    ae_vector_clear(&p->sclagtmp1);
}


void _minsqptmplagrangian_destroy(void* _p)
{
    minsqptmplagrangian *p = (minsqptmplagrangian*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->sclagtmp0);
    ae_vector_destroy(&p->sclagtmp1);
}


void _minsqptmpmerit_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minsqptmpmerit *p = (minsqptmpmerit*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->mftmp0, 0, DT_REAL, _state, make_automatic);
}


void _minsqptmpmerit_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minsqptmpmerit       *dst = (minsqptmpmerit*)_dst;
    const minsqptmpmerit *src = (const minsqptmpmerit*)_src;
    ae_vector_init_copy(&dst->mftmp0, &src->mftmp0, _state, make_automatic);
}


void _minsqptmpmerit_clear(void* _p)
{
    minsqptmpmerit *p = (minsqptmpmerit*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->mftmp0);
}


void _minsqptmpmerit_destroy(void* _p)
{
    minsqptmpmerit *p = (minsqptmpmerit*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->mftmp0);
}


void _minsqpmeritphasestate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minsqpmeritphasestate *p = (minsqpmeritphasestate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->dtrial, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->deffective, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dmu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkxc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkxn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfic, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfin, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkj, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkjc, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkjn, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curlinx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curlinfi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->curlinj, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagbcmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagxcmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummylagbcmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummylagxcmult, 0, DT_REAL, _state, make_automatic);
    _minsqptmpmerit_init(&p->tmpmerit, _state, make_automatic);
    _minsqptmplagrangian_init(&p->tmplagrangianfg, _state, make_automatic);
    ae_vector_init(&p->stepklaggrad, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepknlaggrad, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmphdiag, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rmeritphasestate, _state, make_automatic);
}


void _minsqpmeritphasestate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minsqpmeritphasestate       *dst = (minsqpmeritphasestate*)_dst;
    const minsqpmeritphasestate *src = (const minsqpmeritphasestate*)_src;
    dst->n = src->n;
    dst->nec = src->nec;
    dst->nic = src->nic;
    dst->nlec = src->nlec;
    dst->nlic = src->nlic;
    ae_vector_init_copy(&dst->dtrial, &src->dtrial, _state, make_automatic);
    ae_vector_init_copy(&dst->deffective, &src->deffective, _state, make_automatic);
    ae_vector_init_copy(&dst->d0, &src->d0, _state, make_automatic);
    ae_vector_init_copy(&dst->d1, &src->d1, _state, make_automatic);
    ae_vector_init_copy(&dst->dmu, &src->dmu, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkx, &src->stepkx, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkxc, &src->stepkxc, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkxn, &src->stepkxn, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfi, &src->stepkfi, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfic, &src->stepkfic, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfin, &src->stepkfin, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkj, &src->stepkj, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkjc, &src->stepkjc, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkjn, &src->stepkjn, _state, make_automatic);
    ae_vector_init_copy(&dst->curlinx, &src->curlinx, _state, make_automatic);
    ae_vector_init_copy(&dst->curlinfi, &src->curlinfi, _state, make_automatic);
    ae_matrix_init_copy(&dst->curlinj, &src->curlinj, _state, make_automatic);
    ae_vector_init_copy(&dst->lagbcmult, &src->lagbcmult, _state, make_automatic);
    ae_vector_init_copy(&dst->lagxcmult, &src->lagxcmult, _state, make_automatic);
    ae_vector_init_copy(&dst->dummylagbcmult, &src->dummylagbcmult, _state, make_automatic);
    ae_vector_init_copy(&dst->dummylagxcmult, &src->dummylagxcmult, _state, make_automatic);
    _minsqptmpmerit_init_copy(&dst->tmpmerit, &src->tmpmerit, _state, make_automatic);
    _minsqptmplagrangian_init_copy(&dst->tmplagrangianfg, &src->tmplagrangianfg, _state, make_automatic);
    ae_vector_init_copy(&dst->stepklaggrad, &src->stepklaggrad, _state, make_automatic);
    ae_vector_init_copy(&dst->stepknlaggrad, &src->stepknlaggrad, _state, make_automatic);
    dst->status = src->status;
    dst->increasebigc = src->increasebigc;
    dst->increasemeritmu = src->increasemeritmu;
    dst->meritfstagnated = src->meritfstagnated;
    ae_vector_init_copy(&dst->tmphdiag, &src->tmphdiag, _state, make_automatic);
    _rcommstate_init_copy(&dst->rmeritphasestate, &src->rmeritphasestate, _state, make_automatic);
}


void _minsqpmeritphasestate_clear(void* _p)
{
    minsqpmeritphasestate *p = (minsqpmeritphasestate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->dtrial);
    ae_vector_clear(&p->deffective);
    ae_vector_clear(&p->d0);
    ae_vector_clear(&p->d1);
    ae_vector_clear(&p->dmu);
    ae_vector_clear(&p->stepkx);
    ae_vector_clear(&p->stepkxc);
    ae_vector_clear(&p->stepkxn);
    ae_vector_clear(&p->stepkfi);
    ae_vector_clear(&p->stepkfic);
    ae_vector_clear(&p->stepkfin);
    ae_matrix_clear(&p->stepkj);
    ae_matrix_clear(&p->stepkjc);
    ae_matrix_clear(&p->stepkjn);
    ae_vector_clear(&p->curlinx);
    ae_vector_clear(&p->curlinfi);
    ae_matrix_clear(&p->curlinj);
    ae_vector_clear(&p->lagbcmult);
    ae_vector_clear(&p->lagxcmult);
    ae_vector_clear(&p->dummylagbcmult);
    ae_vector_clear(&p->dummylagxcmult);
    _minsqptmpmerit_clear(&p->tmpmerit);
    _minsqptmplagrangian_clear(&p->tmplagrangianfg);
    ae_vector_clear(&p->stepklaggrad);
    ae_vector_clear(&p->stepknlaggrad);
    ae_vector_clear(&p->tmphdiag);
    _rcommstate_clear(&p->rmeritphasestate);
}


void _minsqpmeritphasestate_destroy(void* _p)
{
    minsqpmeritphasestate *p = (minsqpmeritphasestate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->dtrial);
    ae_vector_destroy(&p->deffective);
    ae_vector_destroy(&p->d0);
    ae_vector_destroy(&p->d1);
    ae_vector_destroy(&p->dmu);
    ae_vector_destroy(&p->stepkx);
    ae_vector_destroy(&p->stepkxc);
    ae_vector_destroy(&p->stepkxn);
    ae_vector_destroy(&p->stepkfi);
    ae_vector_destroy(&p->stepkfic);
    ae_vector_destroy(&p->stepkfin);
    ae_matrix_destroy(&p->stepkj);
    ae_matrix_destroy(&p->stepkjc);
    ae_matrix_destroy(&p->stepkjn);
    ae_vector_destroy(&p->curlinx);
    ae_vector_destroy(&p->curlinfi);
    ae_matrix_destroy(&p->curlinj);
    ae_vector_destroy(&p->lagbcmult);
    ae_vector_destroy(&p->lagxcmult);
    ae_vector_destroy(&p->dummylagbcmult);
    ae_vector_destroy(&p->dummylagxcmult);
    _minsqptmpmerit_destroy(&p->tmpmerit);
    _minsqptmplagrangian_destroy(&p->tmplagrangianfg);
    ae_vector_destroy(&p->stepklaggrad);
    ae_vector_destroy(&p->stepknlaggrad);
    ae_vector_destroy(&p->tmphdiag);
    _rcommstate_destroy(&p->rmeritphasestate);
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
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    _minsqpmeritphasestate_init(&p->meritstate, _state, make_automatic);
    ae_vector_init(&p->step0x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->backupx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->step0fi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->backupfi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->step0j, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkj, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fscales, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tracegamma, 0, DT_REAL, _state, make_automatic);
    _minsqpsubsolver_init(&p->subsolver, _state, make_automatic);
    _xbfgshessian_init(&p->hess, _state, make_automatic);
    _minsqptmpmerit_init(&p->tmpmerit, _state, make_automatic);
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
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_matrix_init_copy(&dst->scaledcleic, &src->scaledcleic, _state, make_automatic);
    ae_vector_init_copy(&dst->lcsrcidx, &src->lcsrcidx, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndl, &src->scaledbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndu, &src->scaledbndu, _state, make_automatic);
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->bfgsresetfreq = src->bfgsresetfreq;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    dst->f = src->f;
    dst->needfij = src->needfij;
    dst->xupdated = src->xupdated;
    _minsqpmeritphasestate_init_copy(&dst->meritstate, &src->meritstate, _state, make_automatic);
    dst->bigc = src->bigc;
    dst->meritmu = src->meritmu;
    dst->trustrad = src->trustrad;
    dst->trustradgrowth = src->trustradgrowth;
    dst->trustradstagnationcnt = src->trustradstagnationcnt;
    dst->fstagnationcnt = src->fstagnationcnt;
    ae_vector_init_copy(&dst->step0x, &src->step0x, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkx, &src->stepkx, _state, make_automatic);
    ae_vector_init_copy(&dst->backupx, &src->backupx, _state, make_automatic);
    ae_vector_init_copy(&dst->step0fi, &src->step0fi, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfi, &src->stepkfi, _state, make_automatic);
    ae_vector_init_copy(&dst->backupfi, &src->backupfi, _state, make_automatic);
    ae_matrix_init_copy(&dst->step0j, &src->step0j, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkj, &src->stepkj, _state, make_automatic);
    ae_vector_init_copy(&dst->fscales, &src->fscales, _state, make_automatic);
    ae_vector_init_copy(&dst->tracegamma, &src->tracegamma, _state, make_automatic);
    _minsqpsubsolver_init_copy(&dst->subsolver, &src->subsolver, _state, make_automatic);
    _xbfgshessian_init_copy(&dst->hess, &src->hess, _state, make_automatic);
    _minsqptmpmerit_init_copy(&dst->tmpmerit, &src->tmpmerit, _state, make_automatic);
    dst->repsimplexiterations = src->repsimplexiterations;
    dst->repsimplexiterations1 = src->repsimplexiterations1;
    dst->repsimplexiterations2 = src->repsimplexiterations2;
    dst->repsimplexiterations3 = src->repsimplexiterations3;
    dst->repiterationscount = src->repiterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repbcerr = src->repbcerr;
    dst->repbcidx = src->repbcidx;
    dst->replcerr = src->replcerr;
    dst->replcidx = src->replcidx;
    dst->repnlcerr = src->repnlcerr;
    dst->repnlcidx = src->repnlcidx;
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
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    _minsqpmeritphasestate_clear(&p->meritstate);
    ae_vector_clear(&p->step0x);
    ae_vector_clear(&p->stepkx);
    ae_vector_clear(&p->backupx);
    ae_vector_clear(&p->step0fi);
    ae_vector_clear(&p->stepkfi);
    ae_vector_clear(&p->backupfi);
    ae_matrix_clear(&p->step0j);
    ae_matrix_clear(&p->stepkj);
    ae_vector_clear(&p->fscales);
    ae_vector_clear(&p->tracegamma);
    _minsqpsubsolver_clear(&p->subsolver);
    _xbfgshessian_clear(&p->hess);
    _minsqptmpmerit_clear(&p->tmpmerit);
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
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    _minsqpmeritphasestate_destroy(&p->meritstate);
    ae_vector_destroy(&p->step0x);
    ae_vector_destroy(&p->stepkx);
    ae_vector_destroy(&p->backupx);
    ae_vector_destroy(&p->step0fi);
    ae_vector_destroy(&p->stepkfi);
    ae_vector_destroy(&p->backupfi);
    ae_matrix_destroy(&p->step0j);
    ae_matrix_destroy(&p->stepkj);
    ae_vector_destroy(&p->fscales);
    ae_vector_destroy(&p->tracegamma);
    _minsqpsubsolver_destroy(&p->subsolver);
    _xbfgshessian_destroy(&p->hess);
    _minsqptmpmerit_destroy(&p->tmpmerit);
    _rcommstate_destroy(&p->rstate);
}


/*$ End $*/
