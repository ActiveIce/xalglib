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
#include "nlcslp.h"


/*$ Declarations $*/
static double nlcslp_slpstpclosetozero = 0.001;
static double nlcslp_slpdeltadecrease = 0.20;
static double nlcslp_slpdeltaincrease = 0.80;
static double nlcslp_slpstpclosetoone = 0.95;
static double nlcslp_maxtrustraddecay = 0.1;
static double nlcslp_maxtrustradgrowth = 1.333;
static double nlcslp_slpgtol = 0.4;
static double nlcslp_initbigc = 500.0;
static double nlcslp_maxbigc = 1.0E5;
static double nlcslp_bfgstol = 1.0E-5;
static double nlcslp_defaultl1penalty = 0.1;
static double nlcslp_meritfunctionbase = 0.0;
static double nlcslp_meritfunctiongain = 2.0;
static double nlcslp_inequalitydampingfactor = 10.0;
static double nlcslp_augmentationfactor = 10.0;
static double nlcslp_inittrustrad = 0.1;
static double nlcslp_stagnationepsf = 1.0E-12;
static ae_int_t nlcslp_lpfailureslimit = 20;
static ae_int_t nlcslp_fstagnationlimit = 20;
static ae_int_t nlcslp_nondescentlimit = 99999;
static ae_int_t nlcslp_nonmonotonicphase2limit = 5;
static double nlcslp_slpbigscale = 5.0;
static double nlcslp_slpsmallscale = 0.2;
static double nlcslp_defaultmaglagdecay = 0.85;
static void nlcslp_initlpsubsolver(const minslpstate* sstate,
     minslpsubsolver* subsolver,
     ae_state *_state);
static void nlcslp_lpsubproblemrestart(const minslpstate* sstate,
     minslpsubsolver* subsolver,
     ae_state *_state);
static ae_bool nlcslp_lpsubproblemsolve(minslpstate* state,
     minslpsubsolver* subsolver,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     ae_int_t innerk,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagmult,
     ae_state *_state);
static void nlcslp_lpsubproblemappendconjugacyconstraint(const minslpstate* state,
     minslpsubsolver* subsolver,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
static void nlcslp_phase13init(minslpphase13state* state13,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_bool usecorrection,
     ae_state *_state);
static ae_bool nlcslp_phase13iteration(minslpstate* state,
     minslpphase13state* state13,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     /* Real    */ ae_vector* curx,
     /* Real    */ ae_vector* curfi,
     /* Real    */ ae_matrix* curj,
     /* Real    */ ae_vector* lagmult,
     ae_int_t* status,
     double* dnrm,
     double* stp,
     ae_state *_state);
static void nlcslp_phase2init(minslpphase2state* state2,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     /* Real    */ const ae_vector* meritlagmult,
     ae_state *_state);
static ae_bool nlcslp_phase2iteration(minslpstate* state,
     minslpphase2state* state2,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     /* Real    */ ae_vector* curx,
     /* Real    */ ae_vector* curfi,
     /* Real    */ ae_matrix* curj,
     /* Real    */ ae_vector* lagmult,
     double* gammamax,
     ae_int_t* status,
     ae_state *_state);
static void nlcslp_slpsendx(minslpstate* state,
     /* Real    */ const ae_vector* xs,
     ae_state *_state);
static ae_bool nlcslp_slpretrievefij(const minslpstate* state,
     /* Real    */ ae_vector* fis,
     /* Real    */ ae_matrix* js,
     ae_state *_state);
static void nlcslp_slpcopystate(const minslpstate* state,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* fi0,
     /* Real    */ const ae_matrix* j0,
     /* Real    */ ae_vector* x1,
     /* Real    */ ae_vector* fi1,
     /* Real    */ ae_matrix* j1,
     ae_state *_state);
static void nlcslp_lagrangianfg(minslpstate* state,
     /* Real    */ const ae_vector* x,
     double trustrad,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* j,
     /* Real    */ const ae_vector* lagmult,
     minslptmplagrangian* tmp,
     double* f,
     /* Real    */ ae_vector* g,
     double* lcerr,
     ae_int_t* lcidx,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);
static double nlcslp_meritfunction(minslpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagmult,
     double mu,
     minslptmpmerit* tmp,
     ae_state *_state);
static double nlcslp_rawlagrangian(minslpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagmult,
     minslptmpmerit* tmp,
     ae_state *_state);
static void nlcslp_meritfunctionandrawlagrangian(minslpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagmult,
     double mu,
     minslptmpmerit* tmp,
     double* meritf,
     double* rawlag,
     ae_state *_state);


/*$ Body $*/


void minslpinitbuf(/* Real    */ const ae_vector* bndl,
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
     minslpstate* state,
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
    ae_vector_set_length(&state->rstate.ba, 3+1, _state);
    ae_vector_set_length(&state->rstate.ra, 16+1, _state);
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
    rmatrixsetlengthatleast(&state->backupj, 1+nlec+nlic, n, _state);
    rvectorsetlengthatleast(&state->fscales, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&state->meritlagmult, nec+nic+nlec+nlic, _state);
    rvectorsetlengthatleast(&state->dummylagmult, nec+nic+nlec+nlic, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    rvectorsetlengthatleast(&state->scaledbndl, n, _state);
    rvectorsetlengthatleast(&state->scaledbndu, n, _state);
    rmatrixsetlengthatleast(&state->scaledcleic, nec+nic, n+1, _state);
    ivectorsetlengthatleast(&state->lcsrcidx, nec+nic, _state);
    rvectorsetlengthatleast(&state->meritfunctionhistory, nlcslp_nonmonotonicphase2limit+1, _state);
    rvectorsetlengthatleast(&state->maxlaghistory, nlcslp_nonmonotonicphase2limit+1, _state);
    
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
            ae_assert(ae_fp_less_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]), "SLP: integrity check failed, box constraints are inconsistent", _state);
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
     * Stopping criteria
     */
    if( ae_fp_eq(ae_maxreal(epsx, (double)(maxits), _state),(double)(0)) )
    {
        epsx = 1.0E-8;
    }
    state->epsx = epsx;
    state->maxits = maxits;
    
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
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    
    /*
     * Integrity checks:
     * * it is important that significant step length is large enough that
     *   we do not decrease trust regiod radius; it should also be small,
     *   so we won't treat large steps as insignificant
     */
    ae_assert(ae_fp_less(nlcslp_slpstpclosetozero,nlcslp_slpdeltadecrease), "MinSLP: integrity check failed", _state);
    ae_assert(ae_fp_less(nlcslp_slpdeltadecrease,nlcslp_slpdeltaincrease), "MinSLP: integrity check failed", _state);
    ae_assert(ae_fp_less(nlcslp_slpdeltaincrease,nlcslp_slpstpclosetoone), "MinSLP: integrity check failed", _state);
}


/*************************************************************************
This function performs actual processing for  SLP  algorithm.  It  expects
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
ae_bool minslpiteration(minslpstate* state,
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
    ae_int_t innerk;
    double v;
    double vv;
    double mx;
    ae_bool lpstagesuccess;
    double gammamax;
    double f1;
    double f2;
    ae_int_t status;
    double stp;
    double deltamax;
    double multiplyby;
    double setscaleto;
    double prevtrustrad;
    ae_bool increasebigc;
    double d1nrm;
    double mu;
    double expandedrad;
    double tol;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    double maxlag;
    double maxhist;
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
        innerk = state->rstate.ia.ptr.p_int[8];
        status = state->rstate.ia.ptr.p_int[9];
        lpstagesuccess = state->rstate.ba.ptr.p_bool[0];
        increasebigc = state->rstate.ba.ptr.p_bool[1];
        dotrace = state->rstate.ba.ptr.p_bool[2];
        dodetailedtrace = state->rstate.ba.ptr.p_bool[3];
        v = state->rstate.ra.ptr.p_double[0];
        vv = state->rstate.ra.ptr.p_double[1];
        mx = state->rstate.ra.ptr.p_double[2];
        gammamax = state->rstate.ra.ptr.p_double[3];
        f1 = state->rstate.ra.ptr.p_double[4];
        f2 = state->rstate.ra.ptr.p_double[5];
        stp = state->rstate.ra.ptr.p_double[6];
        deltamax = state->rstate.ra.ptr.p_double[7];
        multiplyby = state->rstate.ra.ptr.p_double[8];
        setscaleto = state->rstate.ra.ptr.p_double[9];
        prevtrustrad = state->rstate.ra.ptr.p_double[10];
        d1nrm = state->rstate.ra.ptr.p_double[11];
        mu = state->rstate.ra.ptr.p_double[12];
        expandedrad = state->rstate.ra.ptr.p_double[13];
        tol = state->rstate.ra.ptr.p_double[14];
        maxlag = state->rstate.ra.ptr.p_double[15];
        maxhist = state->rstate.ra.ptr.p_double[16];
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
        innerk = 809;
        status = 205;
        lpstagesuccess = ae_false;
        increasebigc = ae_true;
        dotrace = ae_false;
        dodetailedtrace = ae_true;
        v = -541.0;
        vv = -698.0;
        mx = -900.0;
        gammamax = -318.0;
        f1 = -940.0;
        f2 = 1016.0;
        stp = -229.0;
        deltamax = -536.0;
        multiplyby = 487.0;
        setscaleto = -115.0;
        prevtrustrad = 886.0;
        d1nrm = 346.0;
        mu = -722.0;
        expandedrad = -413.0;
        tol = -461.0;
        maxlag = 927.0;
        maxhist = 201.0;
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
    nslack = n+2*(nec+nlec)+(nic+nlic);
    dotrace = ae_is_trace_enabled("SLP");
    dodetailedtrace = dotrace&&ae_is_trace_enabled("SLP.DETAILED");
    
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
    state->lpfailurecnt = 0;
    state->fstagnationcnt = 0;
    state->bigc = nlcslp_initbigc;
    state->trustrad = nlcslp_inittrustrad;
    for(i=0; i<=nlec+nlic; i++)
    {
        state->fscales.ptr.p_double[i] = 1.0;
    }
    for(i=0; i<=nlcslp_nonmonotonicphase2limit; i++)
    {
        state->meritfunctionhistory.ptr.p_double[i] = ae_maxrealnumber;
        state->maxlaghistory.ptr.p_double[i] = 0.0;
    }
    state->historylen = 0;
    gammamax = 0.0;
    
    /*
     * Avoid spurious warnings about possibly uninitialized vars
     */
    status = 0;
    stp = (double)(0);
    
    /*
     * Evaluate function vector and Jacobian at Step0X, send first location report.
     * Compute initial violation of constraints.
     */
    nlcslp_slpsendx(state, &state->step0x, _state);
    state->needfij = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    if( !nlcslp_slpretrievefij(state, &state->step0fi, &state->step0j, _state) )
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
    nlcslp_slpcopystate(state, &state->step0x, &state->step0fi, &state->step0j, &state->stepkx, &state->stepkfi, &state->stepkj, _state);
    nlcslp_slpsendx(state, &state->stepkx, _state);
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
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  SLP SOLVER STARTED                                                                            //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
    }
    
    /*
     * Perform outer (NLC) iterations
     */
    nlcslp_initlpsubsolver(state, &state->subsolver, _state);
lbl_5:
    if( ae_false )
    {
        goto lbl_6;
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
        if( ae_fp_greater_eq(mx,nlcslp_slpbigscale) )
        {
            multiplyby = (double)1/mx;
            setscaleto = state->fscales.ptr.p_double[i]*mx;
        }
        if( ae_fp_less_eq(mx,nlcslp_slpsmallscale)&&ae_fp_greater(state->fscales.ptr.p_double[i],1.0) )
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
             * * update FScales[] array
             */
            state->stepkfi.ptr.p_double[i] = state->stepkfi.ptr.p_double[i]*multiplyby;
            for(j=0; j<=n-1; j++)
            {
                state->stepkj.ptr.pp_double[i][j] = state->stepkj.ptr.pp_double[i][j]*multiplyby;
            }
            state->fscales.ptr.p_double[i] = setscaleto;
        }
    }
    
    /*
     * Save initial point for the outer iteration
     */
    nlcslp_slpcopystate(state, &state->stepkx, &state->stepkfi, &state->stepkj, &state->step0x, &state->step0fi, &state->step0j, _state);
    
    /*
     * Trace output (if needed)
     */
    if( dotrace )
    {
        ae_trace("\n=== OUTER ITERATION %5d STARTED ==================================================================\n",
            (int)(state->repouteriterationscount));
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
        ae_trace("lin.violation = %0.3e    (scaled violation of linear constraints)\n",
            (double)(state->replcerr));
        ae_trace("nlc.violation = %0.3e    (scaled violation of nonlinear constraints)\n",
            (double)(mx));
        ae_trace("gammaMax      = %0.3e\n",
            (double)(gammamax));
        ae_trace("max|LagMult|  = %0.3e    (maximum over %0d last iterations)\n",
            (double)(rmaxabsv(state->historylen, &state->maxlaghistory, _state)),
            (int)(state->historylen));
    }
    
    /*
     * PHASE 1:
     *
     * * perform step using linear model with second order correction
     * * compute "reference" Lagrange multipliers
     * * compute merit function at the end of the phase 1 and push it to the history queue
     *
     * NOTE: a second order correction helps to overcome Maratos effect - a tendency
     *       of L1 penalized merit function to reject nonzero steps along steepest
     *       descent direction.
     *
     *       The idea (explained in more details in the Phase13Iteration() body)
     *       is to perform one look-ahead step and use updated constraint values
     *       back at the initial point.
     */
    nlcslp_phase13init(&state->state13, n, nec, nic, nlec, nlic, ae_false, _state);
lbl_7:
    if( !nlcslp_phase13iteration(state, &state->state13, smonitor, userterminationneeded, &state->stepkx, &state->stepkfi, &state->stepkj, &state->meritlagmult, &status, &d1nrm, &stp, _state) )
    {
        goto lbl_8;
    }
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    goto lbl_7;
lbl_8:
    if( status<0 )
    {
        goto lbl_5;
    }
    if( status==0 )
    {
        goto lbl_6;
    }
    maxlag = rmaxabsv(nec+nic+nlec+nlic, &state->meritlagmult, _state);
    maxhist = rmaxabsv(state->historylen, &state->maxlaghistory, _state);
    mu = coalesce(ae_maxreal(maxhist, maxlag, _state), nlcslp_defaultl1penalty, _state);
    for(i=state->historylen; i>=1; i--)
    {
        state->meritfunctionhistory.ptr.p_double[i] = state->meritfunctionhistory.ptr.p_double[i-1];
        state->maxlaghistory.ptr.p_double[i] = state->maxlaghistory.ptr.p_double[i-1];
    }
    state->meritfunctionhistory.ptr.p_double[0] = nlcslp_meritfunction(state, &state->stepkx, &state->stepkfi, &state->meritlagmult, mu, &state->tmpmerit, _state);
    state->maxlaghistory.ptr.p_double[0] = coalesce(maxlag, nlcslp_defaultmaglagdecay*maxhist, _state);
    state->historylen = ae_minint(state->historylen+1, nlcslp_nonmonotonicphase2limit, _state);
    
    /*
     * Decide whether we need to increase BigC (penalty for the constraint violation that
     * is used by the linear subsolver) or not. BigC is increased if all of the following
     * holds true:
     * * BigC can be increased (it is below upper limit)
     * * a short step was performed (shorter than the current trust region)
     * * at least one of the constraints is infeasible within current trust region
     */
    if( ae_fp_less(d1nrm*stp,0.99*state->trustrad)&&ae_fp_less(state->bigc,0.9*nlcslp_maxbigc) )
    {
        increasebigc = ae_false;
        expandedrad = 1.1*state->trustrad;
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
            increasebigc = increasebigc||ae_fp_greater(ae_fabs(v, _state),vv+tol);
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
            increasebigc = increasebigc||ae_fp_greater(ae_fabs(v, _state),vv+tol);
        }
        if( increasebigc )
        {
            state->bigc = ae_minreal((double)10*state->bigc, nlcslp_maxbigc, _state);
            if( dotrace )
            {
                ae_trace("BigC        = %0.3e (trust radius is small, but some constraints are still infeasible - increasing constraint violation penalty)\n",
                    (double)(state->bigc));
            }
        }
    }
    
    /*
     * PHASE 2: conjugate subiterations
     *
     * If step with second order correction is shorter than 1.0, it means
     * that target is sufficiently nonlinear to use advanced iterations.
     * * perform inner LP subiterations with additional conjugacy constraints
     * * check changes in merit function, discard iteration results if merit function increased
     */
    if( ae_fp_greater_eq(stp,nlcslp_slpstpclosetoone) )
    {
        goto lbl_9;
    }
    if( dotrace )
    {
        ae_trace("> linear model produced short step, starting conjugate-gradient-like phase\n");
    }
    nlcslp_slpcopystate(state, &state->stepkx, &state->stepkfi, &state->stepkj, &state->backupx, &state->backupfi, &state->backupj, _state);
    
    /*
     * LP subiterations
     */
    nlcslp_phase2init(&state->state2, n, nec, nic, nlec, nlic, &state->meritlagmult, _state);
lbl_11:
    if( !nlcslp_phase2iteration(state, &state->state2, smonitor, userterminationneeded, &state->stepkx, &state->stepkfi, &state->stepkj, &state->dummylagmult, &gammamax, &status, _state) )
    {
        goto lbl_12;
    }
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    goto lbl_11;
lbl_12:
    if( status==0 )
    {
        
        /*
         * Save progress so far and stop
         */
        goto lbl_6;
    }
    
    /*
     * Evaluating step
     *
     * This step is essential because previous step (which minimizes Lagrangian) may fail
     * to produce descent direction for L1-penalized merit function and will increase it
     * instead of decreasing.
     *
     * During evaluation we compare merit function at new location with maximum computed
     * over last NonmonotonicPhase2Limit+1 previous ones (as suggested in 'A Sequential
     * Quadratic Programming Algorithm with Non-Monotone Line Search' by Yu-Hong Dai).
     *
     * Settings NonmonotonicPhase2Limit to 0 will result in strictly monotonic line search,
     * whilst having nonzero limits means that we perform more robust nonmonotonic search.
     */
    ae_assert(state->historylen>=1, "SLP: integrity check 6559 failed", _state);
    f1 = state->meritfunctionhistory.ptr.p_double[0];
    for(i=1; i<=state->historylen; i++)
    {
        f1 = ae_maxreal(f1, state->meritfunctionhistory.ptr.p_double[i], _state);
    }
    f2 = nlcslp_meritfunction(state, &state->stepkx, &state->stepkfi, &state->meritlagmult, mu, &state->tmpmerit, _state);
    if( dotrace )
    {
        ae_trace("> evaluating changes in merit function (max over last %0d values is used for reference):\n",
            (int)(nlcslp_nonmonotonicphase2limit+1));
        ae_trace("meritF: %14.6e -> %14.6e (delta=%11.3e)\n",
            (double)(f1),
            (double)(f2),
            (double)(f2-f1));
    }
    if( ae_fp_less(f2,f1) )
    {
        goto lbl_13;
    }
    
    /*
     * Merit function does not decrease, discard phase results and report is as one
     * more "fake" inner iteration.
     *
     * NOTE: it is important that F2=F1 is considered as "does not decrease"
     */
    if( dotrace )
    {
        ae_trace("> CG-like phase increased merit function, completely discarding phase (happens sometimes, but not too often)\n");
    }
    nlcslp_slpcopystate(state, &state->backupx, &state->backupfi, &state->backupj, &state->stepkx, &state->stepkfi, &state->stepkj, _state);
    inc(&state->repinneriterationscount, _state);
    nlcslp_slpsendx(state, &state->stepkx, _state);
    state->f = state->stepkfi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->xupdated = ae_false;
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, &state->stepkx, n, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlcviolation(&state->stepkfi, &state->fscales, nlec, nlic, &state->repnlcerr, &state->repnlcidx, _state);
    goto lbl_14;
lbl_13:
    
    /*
     * Merit function decreased, accept phase
     */
    state->meritfunctionhistory.ptr.p_double[0] = f2;
    if( dotrace )
    {
        ae_trace("> CG-like phase decreased merit function, CG-like step accepted\n");
    }
lbl_14:
    goto lbl_10;
lbl_9:
    
    /*
     * No phase #2
     */
    if( dotrace )
    {
        if( ae_fp_greater(stp,(double)(0)) )
        {
            ae_trace("> linear model produced long step, no need to start CG-like iterations\n");
        }
        else
        {
            ae_trace("> linear model produced zero step, maybe trust radius is too large\n");
        }
    }
lbl_10:
    
    /*
     * Update trust region
     */
    prevtrustrad = state->trustrad;
    deltamax = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        deltamax = ae_maxreal(deltamax, ae_fabs(state->step0x.ptr.p_double[i]-state->stepkx.ptr.p_double[i], _state)/state->trustrad, _state);
    }
    if( ae_fp_less_eq(deltamax,nlcslp_slpdeltadecrease) )
    {
        state->trustrad = state->trustrad*ae_maxreal(deltamax/nlcslp_slpdeltadecrease, nlcslp_maxtrustraddecay, _state);
    }
    if( ae_fp_greater_eq(deltamax,nlcslp_slpdeltaincrease) )
    {
        state->trustrad = state->trustrad*ae_minreal(deltamax/nlcslp_slpdeltaincrease, nlcslp_maxtrustradgrowth, _state);
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
     * Advance outer iteration counter, test stopping criteria
     */
    inc(&state->repouteriterationscount, _state);
    if( ae_fp_less_eq(ae_fabs(state->stepkfi.ptr.p_double[0]-state->step0fi.ptr.p_double[0], _state),nlcslp_stagnationepsf*ae_fabs(state->step0fi.ptr.p_double[0], _state)) )
    {
        inc(&state->fstagnationcnt, _state);
    }
    else
    {
        state->fstagnationcnt = 0;
    }
    if( ae_fp_less_eq(state->trustrad,state->epsx) )
    {
        state->repterminationtype = 2;
        if( dotrace )
        {
            ae_trace("> stopping condition met: trust radius is smaller than %0.3e\n",
                (double)(state->epsx));
        }
        goto lbl_6;
    }
    if( state->maxits>0&&state->repinneriterationscount>=state->maxits )
    {
        state->repterminationtype = 5;
        if( dotrace )
        {
            ae_trace("> stopping condition met: %0d iterations performed\n",
                (int)(state->repinneriterationscount));
        }
        goto lbl_6;
    }
    if( state->fstagnationcnt>=nlcslp_fstagnationlimit )
    {
        state->repterminationtype = 7;
        if( dotrace )
        {
            ae_trace("> stopping criteria are too stringent: F stagnated for %0d its, stopping\n",
                (int)(state->fstagnationcnt));
        }
        goto lbl_6;
    }
    goto lbl_5;
lbl_6:
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
    state->rstate.ia.ptr.p_int[8] = innerk;
    state->rstate.ia.ptr.p_int[9] = status;
    state->rstate.ba.ptr.p_bool[0] = lpstagesuccess;
    state->rstate.ba.ptr.p_bool[1] = increasebigc;
    state->rstate.ba.ptr.p_bool[2] = dotrace;
    state->rstate.ba.ptr.p_bool[3] = dodetailedtrace;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vv;
    state->rstate.ra.ptr.p_double[2] = mx;
    state->rstate.ra.ptr.p_double[3] = gammamax;
    state->rstate.ra.ptr.p_double[4] = f1;
    state->rstate.ra.ptr.p_double[5] = f2;
    state->rstate.ra.ptr.p_double[6] = stp;
    state->rstate.ra.ptr.p_double[7] = deltamax;
    state->rstate.ra.ptr.p_double[8] = multiplyby;
    state->rstate.ra.ptr.p_double[9] = setscaleto;
    state->rstate.ra.ptr.p_double[10] = prevtrustrad;
    state->rstate.ra.ptr.p_double[11] = d1nrm;
    state->rstate.ra.ptr.p_double[12] = mu;
    state->rstate.ra.ptr.p_double[13] = expandedrad;
    state->rstate.ra.ptr.p_double[14] = tol;
    state->rstate.ra.ptr.p_double[15] = maxlag;
    state->rstate.ra.ptr.p_double[16] = maxhist;
    return result;
}


/*************************************************************************
This function initializes SLP subproblem.
Should be called once in the beginning of the optimization.

INPUT PARAMETERS:
    SState          -   solver state
    Subsolver       -   SLP subproblem to initialize
                        
                        
RETURN VALUE:
    True on success
    False on failure of the LP solver (unexpected... but possible due to numerical errors)
                        

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static void nlcslp_initlpsubsolver(const minslpstate* sstate,
     minslpsubsolver* subsolver,
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
     * Create simplex solver.
     *
     * NOTE: we disable DSE pricing because it interferes with our
     *       warm-start strategy.
     */
    dsssettingsinit(&subsolver->dsssettings, _state);
    subsolver->dsssettings.pricing = 0;
    
    /*
     * Allocate temporaries
     */
    rvectorsetlengthatleast(&subsolver->cural, lccnt+n, _state);
    rvectorsetlengthatleast(&subsolver->curau, lccnt+n, _state);
    rmatrixsetlengthatleast(&subsolver->curd, n, n, _state);
    rmatrixsetlengthatleast(&subsolver->curhd, n, n, _state);
    rvectorsetlengthatleast(&subsolver->curbndl, nslack, _state);
    rvectorsetlengthatleast(&subsolver->curbndu, nslack, _state);
    rvectorsetlengthatleast(&subsolver->curb, nslack, _state);
    rvectorsetlengthatleast(&subsolver->sk, n, _state);
    rvectorsetlengthatleast(&subsolver->yk, n, _state);
    
    /*
     * Initial state
     */
    subsolver->basispresent = ae_false;
    subsolver->curdcnt = 0;
    hessianinitbfgs(&subsolver->hess, n, 0, coalesce(sstate->epsx, ae_sqrt(ae_machineepsilon, _state), _state), _state);
    
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
Restarts LP subproblem (cleans the matrix of internally stored directions)

INPUT PARAMETERS:
    SState          -   solver state
    Subsolver       -   SLP subproblem to initialize

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static void nlcslp_lpsubproblemrestart(const minslpstate* sstate,
     minslpsubsolver* subsolver,
     ae_state *_state)
{


    subsolver->curdcnt = 0;
}


/*************************************************************************
This function solves LP subproblem given by initial point X, function vector Fi
and Jacobian Jac, and returns estimates of Lagrangian multipliers and search direction D[].

This function does NOT append search direction D to conjugacy constraints,
you have to use LPSubproblemAppendConjugacyConstraint().

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcslp_lpsubproblemsolve(minslpstate* state,
     minslpsubsolver* subsolver,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     ae_int_t innerk,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* lagmult,
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
    ae_int_t basisinittype;
    ae_int_t lccnt;
    ae_int_t offsslackec;
    ae_int_t offsslacknlec;
    ae_int_t offsslackic;
    ae_int_t offsslacknlic;
    ae_int_t offs;
    ae_int_t nnz;
    ae_int_t j0;
    ae_int_t j1;
    ae_bool result;


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
    rvectorgrowto(&subsolver->cural, lccnt+subsolver->curdcnt, _state);
    rvectorgrowto(&subsolver->curau, lccnt+subsolver->curdcnt, _state);
    
    /*
     * Prepare default solution: all zeros
     */
    result = ae_true;
    for(i=0; i<=nslack-1; i++)
    {
        d->ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=lccnt-1; i++)
    {
        lagmult->ptr.p_double[i] = (double)(0);
    }
    
    /*
     * Linear term B
     *
     * NOTE: elements [N,NSlack) are equal to bigC + perturbation to improve numeric properties of LP problem
     */
    for(i=0; i<=n-1; i++)
    {
        subsolver->curb.ptr.p_double[i] = jac->ptr.pp_double[0][i];
    }
    v = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        v = v+ae_sqr(jac->ptr.pp_double[0][i], _state);
    }
    v = coalesce(ae_sqrt(v, _state), 1.0, _state);
    for(i=n; i<=nslack-1; i++)
    {
        subsolver->curb.ptr.p_double[i] = (state->bigc+1.0/(double)(1+i))*v;
    }
    
    /*
     * Trust radius constraints for primary variables
     */
    for(i=0; i<=n-1; i++)
    {
        subsolver->curbndl.ptr.p_double[i] = -state->trustrad;
        subsolver->curbndu.ptr.p_double[i] = state->trustrad;
        if( state->hasbndl.ptr.p_bool[i] )
        {
            subsolver->curbndl.ptr.p_double[i] = ae_maxreal(subsolver->curbndl.ptr.p_double[i], state->scaledbndl.ptr.p_double[i]-x->ptr.p_double[i], _state);
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            subsolver->curbndu.ptr.p_double[i] = ae_minreal(subsolver->curbndu.ptr.p_double[i], state->scaledbndu.ptr.p_double[i]-x->ptr.p_double[i], _state);
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
    nnz = nnz+subsolver->curdcnt*n;
    ivectorgrowto(&subsolver->sparseefflc.ridx, lccnt+n+1, _state);
    rvectorgrowto(&subsolver->sparseefflc.vals, nnz, _state);
    ivectorgrowto(&subsolver->sparseefflc.idx, nnz, _state);
    ivectorsetlengthatleast(&subsolver->sparseefflc.didx, lccnt+n, _state);
    ivectorsetlengthatleast(&subsolver->sparseefflc.uidx, lccnt+n, _state);
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
            v = subsolver->tmp0.ptr.p_double[j];
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
         * If constraint is an inequality one and guaranteed to be inactive
         * within trust region, it is skipped (row itself is retained but
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
         * Set up bounds.
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
            subsolver->curbndu.ptr.p_double[offsslackec+2*i+0] = ae_fabs(v, _state);
            subsolver->curbndu.ptr.p_double[offsslackec+2*i+1] = ae_fabs(v, _state);
        }
        else
        {
            subsolver->cural.ptr.p_double[i] = _state->v_neginf;
            subsolver->curau.ptr.p_double[i] = -v;
            subsolver->curbndl.ptr.p_double[offsslackic+(i-nec)] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslackic+(i-nec)] = ae_maxreal(v, (double)(0), _state);
        }
    }
    subsolver->sparseefflc.m = subsolver->sparseefflc.m+(nec+nic);
    
    /*
     * Append nonlinear equality/inequality constraints
     */
    for(i=0; i<=nlec+nlic-1; i++)
    {
        
        /*
         * Calculate scale coefficient
         */
        vv = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            v = jac->ptr.pp_double[1+i][j];
            vv = vv+v*v;
        }
        vv = (double)1/coalesce(ae_sqrt(vv, _state), (double)(1), _state);
        
        /*
         * Copy scaled row
         */
        offs = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+i];
        for(j=0; j<=n-1; j++)
        {
            if( jac->ptr.pp_double[1+i][j]!=0.0 )
            {
                subsolver->sparseefflc.vals.ptr.p_double[offs] = vv*jac->ptr.pp_double[1+i][j];
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
        v = vv*fi->ptr.p_double[1+i];
        if( i<nlec )
        {
            
            /*
             * Equality constraint
             */
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curbndl.ptr.p_double[offsslacknlec+2*i+0] = (double)(0);
            subsolver->curbndl.ptr.p_double[offsslacknlec+2*i+1] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslacknlec+2*i+0] = ae_fabs(v, _state);
            subsolver->curbndu.ptr.p_double[offsslacknlec+2*i+1] = ae_fabs(v, _state);
        }
        else
        {
            
            /*
             * Inequality constraint
             */
            subsolver->cural.ptr.p_double[subsolver->sparseefflc.m+i] = _state->v_neginf;
            subsolver->curau.ptr.p_double[subsolver->sparseefflc.m+i] = -v;
            subsolver->curbndl.ptr.p_double[offsslacknlic+(i-nlec)] = (double)(0);
            subsolver->curbndu.ptr.p_double[offsslacknlic+(i-nlec)] = ae_maxreal(v, (double)(0), _state);
        }
    }
    subsolver->sparseefflc.m = subsolver->sparseefflc.m+(nlec+nlic);
    
    /*
     * Append conjugacy constraints
     */
    for(i=0; i<=subsolver->curdcnt-1; i++)
    {
        
        /*
         * Copy N elements of CurHD
         *
         * NOTE: we expect product of D and H to be dense, so we copy all N elements
         */
        v = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            vv = subsolver->curhd.ptr.pp_double[i][j];
            v = v+vv*vv;
        }
        v = 1.0/coalesce(ae_sqrt(v, _state), 1.0, _state);
        offs = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m];
        for(j=0; j<=n-1; j++)
        {
            vv = subsolver->curhd.ptr.pp_double[i][j];
            subsolver->sparseefflc.vals.ptr.p_double[offs] = v*vv;
            subsolver->sparseefflc.idx.ptr.p_int[offs] = j;
            offs = offs+1;
        }
        subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m+1] = offs;
        
        /*
         * Set bounds on linear constraints
         */
        subsolver->cural.ptr.p_double[subsolver->sparseefflc.m] = (double)(0);
        subsolver->curau.ptr.p_double[subsolver->sparseefflc.m] = (double)(0);
        
        /*
         * Increase row count
         */
        subsolver->sparseefflc.m = subsolver->sparseefflc.m+1;
    }
    
    /*
     * Finalize sparse matrix structure
     */
    ae_assert(subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m]<=subsolver->sparseefflc.idx.cnt, "LPSubproblemSolve: critical integrity check failed", _state);
    ae_assert(subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m]<=subsolver->sparseefflc.vals.cnt, "LPSubproblemSolve: critical integrity check failed", _state);
    subsolver->sparseefflc.ninitialized = subsolver->sparseefflc.ridx.ptr.p_int[subsolver->sparseefflc.m];
    sparseinitduidx(&subsolver->sparseefflc, _state);
    
    /*
     * Choose dual simplex method basis initialization type
     */
    if( innerk==1&&subsolver->basispresent )
    {
        basisinittype = 2;
    }
    else
    {
        basisinittype = 1;
    }
    
    /*
     * Solve linear program
     */
    rvectorsetlengthatleast(&subsolver->tmp0, nslack, _state);
    for(i=0; i<=nslack-1; i++)
    {
        subsolver->tmp0.ptr.p_double[i] = state->trustrad;
    }
    presolvenonescaleuser(&subsolver->tmp0, &subsolver->curb, &subsolver->curbndl, &subsolver->curbndu, nslack, &subsolver->sparseefflc, &subsolver->cural, &subsolver->curau, subsolver->sparseefflc.m, ae_false, &subsolver->presolver, _state);
    dssinit(subsolver->presolver.newn, &subsolver->dss, _state);
    dsssetproblem(&subsolver->dss, &subsolver->presolver.c, &subsolver->presolver.bndl, &subsolver->presolver.bndu, &subsolver->densedummy, &subsolver->presolver.sparsea, 1, &subsolver->presolver.al, &subsolver->presolver.au, subsolver->presolver.newm, &subsolver->lastbasis, basisinittype, &subsolver->dsssettings, _state);
    dssoptimize(&subsolver->dss, &subsolver->dsssettings, _state);
    rcopyallocv(subsolver->presolver.newn, &subsolver->dss.repx, &subsolver->xs, _state);
    rcopyallocv(subsolver->presolver.newn, &subsolver->dss.replagbc, &subsolver->lagbc, _state);
    rcopyallocv(subsolver->presolver.newm, &subsolver->dss.replaglc, &subsolver->laglc, _state);
    icopyallocv(subsolver->presolver.newn+subsolver->presolver.newm, &subsolver->dss.repstats, &subsolver->cs, _state);
    presolvebwd(&subsolver->presolver, &subsolver->xs, &subsolver->cs, &subsolver->lagbc, &subsolver->laglc, _state);
    state->repsimplexiterations = state->repsimplexiterations+subsolver->dss.repiterationscount;
    state->repsimplexiterations1 = state->repsimplexiterations1+subsolver->dss.repiterationscount1;
    state->repsimplexiterations2 = state->repsimplexiterations2+subsolver->dss.repiterationscount2;
    state->repsimplexiterations3 = state->repsimplexiterations3+subsolver->dss.repiterationscount3;
    if( subsolver->dss.repterminationtype<=0 )
    {
        
        /*
         * LP solver failed due to numerical errors; exit
         */
        result = ae_false;
        return result;
    }
    if( innerk==1 )
    {
        
        /*
         * Store basis
         */
        dssexportbasis(&subsolver->dss, &subsolver->lastbasis, _state);
        subsolver->basispresent = ae_true;
    }
    
    /*
     * Extract direction D[] and Lagrange multipliers
     */
    for(i=0; i<=nslack-1; i++)
    {
        d->ptr.p_double[i] = subsolver->xs.ptr.p_double[i];
    }
    for(i=0; i<=lccnt-1; i++)
    {
        lagmult->ptr.p_double[i] = subsolver->laglc.ptr.p_double[i];
    }
    return result;
}


/*************************************************************************
This function appends last search direction D to conjugacy constraints  of
the LP subproblem.

  -- ALGLIB --
     Copyright 05.03.2018 by Bochkanov Sergey
*************************************************************************/
static void nlcslp_lpsubproblemappendconjugacyconstraint(const minslpstate* state,
     minslpsubsolver* subsolver,
     /* Real    */ const ae_vector* d,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;


    n = state->n;
    
    /*
     * Conjugacy constraint d*H*Dprev=0, only last row of (H*Dprev) is recomputed
     */
    ae_assert(subsolver->curdcnt<subsolver->curd.rows, "SLP: CurD is too small", _state);
    for(i=0; i<=n-1; i++)
    {
        subsolver->curd.ptr.pp_double[subsolver->curdcnt][i] = d->ptr.p_double[i];
    }
    inc(&subsolver->curdcnt, _state);
    hessianmv(&subsolver->hess, d, &subsolver->tmp0, _state);
    for(j=0; j<=n-1; j++)
    {
        subsolver->curhd.ptr.pp_double[subsolver->curdcnt-1][j] = subsolver->tmp0.ptr.p_double[j];
    }
}


/*************************************************************************
This function initializes Phase13  temporaries. It should be called before
beginning of each new iteration. You may call it multiple  times  for  the
same instance of Phase13 temporaries.

INPUT PARAMETERS:
    State13             -   instance to be initialized.
    N                   -   problem dimensionality
    NEC, NIC            -   linear equality/inequality constraint count
    NLEC, NLIC          -   nonlinear equality/inequality constraint count
    UseCorrection       -   True if we want to perform second order correction

OUTPUT PARAMETERS:
    State13     -   instance being initialized

  -- ALGLIB --
     Copyright 05.02.2019 by Bochkanov Sergey
*************************************************************************/
static void nlcslp_phase13init(minslpphase13state* state13,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_bool usecorrection,
     ae_state *_state)
{
    ae_int_t nslack;


    nslack = n+2*(nec+nlec)+(nic+nlic);
    state13->usecorrection = usecorrection;
    rvectorsetlengthatleast(&state13->d, nslack, _state);
    rvectorsetlengthatleast(&state13->dx, nslack, _state);
    rvectorsetlengthatleast(&state13->stepkxc, n, _state);
    rvectorsetlengthatleast(&state13->stepkxn, n, _state);
    rvectorsetlengthatleast(&state13->stepkfic, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&state13->stepkfin, 1+nlec+nlic, _state);
    rmatrixsetlengthatleast(&state13->stepkjc, 1+nlec+nlic, n, _state);
    rmatrixsetlengthatleast(&state13->stepkjn, 1+nlec+nlic, n, _state);
    rvectorsetlengthatleast(&state13->dummylagmult, nec+nic+nlec+nlic, _state);
    ae_vector_set_length(&state13->rphase13state.ia, 8+1, _state);
    ae_vector_set_length(&state13->rphase13state.ba, 2+1, _state);
    ae_vector_set_length(&state13->rphase13state.ra, 6+1, _state);
    state13->rphase13state.stage = -1;
}


/*************************************************************************
This function tries to perform either phase #1 or phase #3 step.

Former corresponds to linear model step (without conjugacy constraints) with
correction for nonlinearity ("second order correction").  Such  correction
helps to overcome  Maratos  effect  (a  tendency  of  L1  penalized  merit
functions to reject nonzero steps).

Latter is a step using linear model with no second order correction.

INPUT PARAMETERS:
    State       -   SLP solver state
    SMonitor    -   smoothness monitor
    UserTerminationNeeded-True if user requested termination
    CurX        -   current point, array[N]
    CurFi       -   function vector at CurX, array[1+NLEC+NLIC]
    CurJ        -   Jacobian at CurX, array[1+NLEC+NLIC,N]
    LagMult     -   array[NEC+NIC+NLEC+NLIC], contents ignored on input.

OUTPUT PARAMETERS:
    State       -   RepTerminationType is set to current termination code (if Status=0).
    CurX        -   advanced to new point
    CurFi       -   updated with function vector at CurX[]
    CurJ        -   updated with Jacobian at CurX[]
    LagMult     -   filled with current Lagrange multipliers
    Status      -   when reverse communication is done, Status is set to:
                    * negative value,  if   we  have   to  restart   outer
                      iteration
                    * positive value,  if we can proceed to the next stage
                      of the outer iteration
                    * zero, if algorithm is terminated (RepTerminationType
                      is set to appropriate value)
    DNrm        -   inf-norm of the proposed step vector D
    Stp         -   step length (multiplier for D), in [0,1]

  -- ALGLIB --
     Copyright 05.02.2019 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcslp_phase13iteration(minslpstate* state,
     minslpphase13state* state13,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     /* Real    */ ae_vector* curx,
     /* Real    */ ae_vector* curfi,
     /* Real    */ ae_matrix* curj,
     /* Real    */ ae_vector* lagmult,
     ae_int_t* status,
     double* dnrm,
     double* stp,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nslack;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_int_t innerk;
    ae_int_t i;
    ae_int_t j;
    double v;
    double mx;
    double f0;
    double f1;
    double nu;
    double localstp;
    double mu;
    ae_bool dotrace;
    ae_bool doprobing;
    ae_bool dotracexd;
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
    if( state13->rphase13state.stage>=0 )
    {
        n = state13->rphase13state.ia.ptr.p_int[0];
        nslack = state13->rphase13state.ia.ptr.p_int[1];
        nec = state13->rphase13state.ia.ptr.p_int[2];
        nic = state13->rphase13state.ia.ptr.p_int[3];
        nlec = state13->rphase13state.ia.ptr.p_int[4];
        nlic = state13->rphase13state.ia.ptr.p_int[5];
        innerk = state13->rphase13state.ia.ptr.p_int[6];
        i = state13->rphase13state.ia.ptr.p_int[7];
        j = state13->rphase13state.ia.ptr.p_int[8];
        dotrace = state13->rphase13state.ba.ptr.p_bool[0];
        doprobing = state13->rphase13state.ba.ptr.p_bool[1];
        dotracexd = state13->rphase13state.ba.ptr.p_bool[2];
        v = state13->rphase13state.ra.ptr.p_double[0];
        mx = state13->rphase13state.ra.ptr.p_double[1];
        f0 = state13->rphase13state.ra.ptr.p_double[2];
        f1 = state13->rphase13state.ra.ptr.p_double[3];
        nu = state13->rphase13state.ra.ptr.p_double[4];
        localstp = state13->rphase13state.ra.ptr.p_double[5];
        mu = state13->rphase13state.ra.ptr.p_double[6];
    }
    else
    {
        n = 922;
        nslack = -154;
        nec = 306;
        nic = -1011;
        nlec = 951;
        nlic = -463;
        innerk = 88;
        i = -861;
        j = -678;
        dotrace = ae_true;
        doprobing = ae_true;
        dotracexd = ae_true;
        v = -233.0;
        mx = -936.0;
        f0 = -279.0;
        f1 = 94.0;
        nu = -812.0;
        localstp = 427.0;
        mu = 178.0;
    }
    if( state13->rphase13state.stage==0 )
    {
        goto lbl_0;
    }
    if( state13->rphase13state.stage==1 )
    {
        goto lbl_1;
    }
    if( state13->rphase13state.stage==2 )
    {
        goto lbl_2;
    }
    if( state13->rphase13state.stage==3 )
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
    innerk = 1;
    dotrace = ae_is_trace_enabled("SLP");
    dotracexd = dotrace&&ae_is_trace_enabled("SLP.DETAILED");
    doprobing = ae_is_trace_enabled("SLP.PROBING");
    ae_assert(lagmult->cnt>=nec+nic+nlec+nlic, "Phase13Iteration: integrity check failed", _state);
    
    /*
     * Report iteration beginning
     */
    if( dotrace )
    {
        if( state13->usecorrection )
        {
            ae_trace("\n--- linear step with second-order correction -------------------------------------------------------\n");
        }
        else
        {
            ae_trace("\n--- linear step without second-order correction ----------------------------------------------------\n");
        }
    }
    
    /*
     * Default decision is to continue algorithm
     */
    *status = 1;
    *stp = (double)(0);
    *dnrm = (double)(0);
    
    /*
     * Determine step direction using linearized model with no conjugacy terms
     */
    nlcslp_lpsubproblemrestart(state, &state->subsolver, _state);
    if( !nlcslp_lpsubproblemsolve(state, &state->subsolver, curx, curfi, curj, innerk, &state13->d, lagmult, _state) )
    {
        if( dotrace )
        {
            ae_trace("> [WARNING] initial phase #1 LP subproblem failed\n");
        }
        
        /*
         * Increase failures counter.
         * Stop after too many subsequent failures
         */
        inc(&state->lpfailurecnt, _state);
        if( state->lpfailurecnt>=nlcslp_lpfailureslimit )
        {
            state->repterminationtype = 7;
            *status = 0;
            if( dotrace )
            {
                ae_trace("> stopping condition met: too many phase #1 LP failures\n");
            }
            result = ae_false;
            return result;
        }
        
        /*
         * Can not solve LP subproblem, decrease trust radius
         */
        state->trustrad = 0.5*state->trustrad;
        if( dotrace )
        {
            ae_trace("> trust radius was decreased to %0.4e\n",
                (double)(state->trustrad));
        }
        if( ae_fp_less(state->trustrad,state->epsx) )
        {
            state->repterminationtype = 2;
            *status = 0;
            if( dotrace )
            {
                ae_trace("> stopping condition met: trust radius is smaller than %0.3e\n",
                    (double)(state->epsx));
            }
        }
        else
        {
            *status = -1;
        }
        result = ae_false;
        return result;
    }
    mu = ae_maxreal(rmaxabsv(state->historylen, &state->maxlaghistory, _state), rmaxabsv(nec+nic+nlec+nlic, lagmult, _state), _state);
    mu = coalesce(mu, nlcslp_defaultl1penalty, _state);
    
    /*
     * Compute second order correction if required. The issue we address here
     * is a tendency of L1 penalized function to reject steps built using simple
     * linearized model when nonlinear constraints change faster than the target.
     *
     * The idea is that we perform trial step (stp=1) using simple linearized model,
     * compute constraint vector at the new trial point - and use these updated
     * constraint linearizations back at the initial point.
     */
    if( !state13->usecorrection )
    {
        goto lbl_4;
    }
    
    /*
     * Perform trial step using vector D to StepKXC
     */
    for(i=0; i<=n-1; i++)
    {
        state13->stepkxc.ptr.p_double[i] = curx->ptr.p_double[i]+state13->d.ptr.p_double[i];
    }
    nlcslp_slpsendx(state, &state13->stepkxc, _state);
    state->needfij = ae_true;
    state13->rphase13state.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    if( !nlcslp_slpretrievefij(state, &state13->stepkfic, &state13->stepkjc, _state) )
    {
        
        /*
         * Failed to retrieve func/Jac, infinities detected
         */
        state->repterminationtype = -8;
        *status = 0;
        if( dotrace )
        {
            ae_trace("[ERROR] infinities in target/constraints are detected\n");
        }
        result = ae_false;
        return result;
    }
    
    /*
     * Move back to point CurX[], restore original linearization of the target
     */
    state13->stepkfic.ptr.p_double[0] = curfi->ptr.p_double[0];
    for(j=0; j<=n-1; j++)
    {
        state13->stepkxc.ptr.p_double[j] = curx->ptr.p_double[j];
        state13->stepkjc.ptr.pp_double[0][j] = curj->ptr.pp_double[0][j];
    }
    
    /*
     * Extrapolate linearization of nonlinear constraints back to origin
     */
    for(i=1; i<=nlec+nlic; i++)
    {
        v = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            v = v+state13->d.ptr.p_double[j]*state13->stepkjc.ptr.pp_double[i][j];
        }
        state13->stepkfic.ptr.p_double[i] = state13->stepkfic.ptr.p_double[i]-v;
    }
    
    /*
     * Solve linearized problem one more time, now with new linearization of constraints
     * (but still old linearization of the target), obtain DX
     *
     * NOTE: because lpsubproblemrestart() call resets set of conjugate constraints, we
     *       have to re-add it after solve.
     */
    nlcslp_lpsubproblemrestart(state, &state->subsolver, _state);
    if( !nlcslp_lpsubproblemsolve(state, &state->subsolver, &state13->stepkxc, &state13->stepkfic, &state13->stepkjc, innerk, &state13->dx, &state13->dummylagmult, _state) )
    {
        
        /*
         * Second LP subproblem failed.
         * Noncritical failure, can be ignored,
         */
        if( dotrace )
        {
            ae_trace("> [WARNING] second phase #1 LP subproblem failed\n");
        }
        if( dotrace )
        {
            ae_trace("> using step without second order correction\n");
        }
    }
    else
    {
        
        /*
         * Set D to new direction
         */
        for(i=0; i<=n-1; i++)
        {
            state13->d.ptr.p_double[i] = state13->dx.ptr.p_double[i];
        }
    }
lbl_4:
    
    /*
     * Now we have search direction in D:
     * * compute DNrm
     * * append D to the list of the conjugacy constraints, so next time when we use the solver we will
     *   automatically produce conjugate direction
     */
    *dnrm = rmaxabsv(n, &state13->d, _state);
    nlcslp_lpsubproblemappendconjugacyconstraint(state, &state->subsolver, &state13->d, _state);
    
    /*
     * Perform merit function backtracking line search, with trial point being
     * computed as XN = XK + Stp*D, with Stp in [0,1]
     *
     * NOTE: we use MeritLagMult - Lagrange multipliers computed for initial,
     *       uncorrected task - for the merit function model.
     *       Using DummyLagMult can destabilize algorithm.
     */
    localstp = 1.0;
    nu = 0.5;
    f0 = nlcslp_meritfunction(state, curx, curfi, lagmult, mu, &state13->tmpmerit, _state);
    f1 = f0;
    smoothnessmonitorstartlinesearch(smonitor, curx, curfi, curj, state->repinneriterationscount, state->repouteriterationscount, _state);
lbl_6:
    if( ae_false )
    {
        goto lbl_7;
    }
    for(i=0; i<=n-1; i++)
    {
        state13->stepkxn.ptr.p_double[i] = curx->ptr.p_double[i]+localstp*state13->d.ptr.p_double[i];
    }
    nlcslp_slpsendx(state, &state13->stepkxn, _state);
    state->needfij = ae_true;
    state13->rphase13state.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfij = ae_false;
    if( !nlcslp_slpretrievefij(state, &state13->stepkfin, &state13->stepkjn, _state) )
    {
        
        /*
         * Failed to retrieve func/Jac, infinities detected
         */
        state->repterminationtype = -8;
        *status = 0;
        if( dotrace )
        {
            ae_trace("[ERROR] infinities in target/constraints are detected\n");
        }
        result = ae_false;
        return result;
    }
    smoothnessmonitorenqueuepoint(smonitor, &state13->d, localstp, &state13->stepkxn, &state13->stepkfin, &state13->stepkjn, _state);
    f1 = nlcslp_meritfunction(state, &state13->stepkxn, &state13->stepkfin, lagmult, mu, &state13->tmpmerit, _state);
    if( ae_fp_less(f1,f0) )
    {
        
        /*
         * Step is found!
         */
        goto lbl_7;
    }
    if( ae_fp_less(localstp,0.001) )
    {
        
        /*
         * Step is shorter than 0.001 times current search direction,
         * it means that no good step can be found.
         */
        localstp = (double)(0);
        nlcslp_slpcopystate(state, curx, curfi, curj, &state13->stepkxn, &state13->stepkfin, &state13->stepkjn, _state);
        goto lbl_7;
    }
    localstp = nu*localstp;
    nu = ae_maxreal(0.1, 0.5*nu, _state);
    goto lbl_6;
lbl_7:
    smoothnessmonitorfinalizelinesearch(smonitor, _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state13->stepkxn.ptr.p_double[i] = ae_maxreal(state13->stepkxn.ptr.p_double[i], state->scaledbndl.ptr.p_double[i], _state);
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state13->stepkxn.ptr.p_double[i] = ae_minreal(state13->stepkxn.ptr.p_double[i], state->scaledbndu.ptr.p_double[i], _state);
        }
    }
    if( userterminationneeded )
    {
        
        /*
         * User requested termination, break before we move to new point
         */
        state->repterminationtype = 8;
        *status = 0;
        if( dotrace )
        {
            ae_trace("> user requested termination\n");
        }
        result = ae_false;
        return result;
    }
    
    /*
     * Trace
     */
    if( !dotrace )
    {
        goto lbl_8;
    }
    if( !doprobing )
    {
        goto lbl_10;
    }
    smoothnessmonitorstartlagrangianprobing(smonitor, curx, &state13->d, 1.0, 0, state->repouteriterationscount, _state);
lbl_12:
    if( !smoothnessmonitorprobelagrangian(smonitor, _state) )
    {
        goto lbl_13;
    }
    for(j=0; j<=n-1; j++)
    {
        state13->stepkxc.ptr.p_double[j] = smonitor->lagprobx.ptr.p_double[j];
        if( state->hasbndl.ptr.p_bool[j] )
        {
            state13->stepkxc.ptr.p_double[j] = ae_maxreal(state13->stepkxc.ptr.p_double[j], state->scaledbndl.ptr.p_double[j], _state);
        }
        if( state->hasbndu.ptr.p_bool[j] )
        {
            state13->stepkxc.ptr.p_double[j] = ae_minreal(state13->stepkxc.ptr.p_double[j], state->scaledbndu.ptr.p_double[j], _state);
        }
    }
    nlcslp_slpsendx(state, &state13->stepkxc, _state);
    state->needfij = ae_true;
    state13->rphase13state.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfij = ae_false;
    if( !nlcslp_slpretrievefij(state, &smonitor->lagprobfi, &smonitor->lagprobj, _state) )
    {
        goto lbl_13;
    }
    smonitor->lagprobrawlag = nlcslp_rawlagrangian(state, &state13->stepkxc, &smonitor->lagprobfi, lagmult, &state13->tmpmerit, _state);
    goto lbl_12;
lbl_13:
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |   probing search direction suggested by LP subproblem    |\n");
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |  Step  | Lagrangian (unaugmentd)|    Target  function    |\n");
    ae_trace("*** |along  D|     must be smooth     |     must be smooth     |\n");
    ae_trace("*** |        | function   |    slope  | function   |    slope  |\n");
    smoothnessmonitortracelagrangianprobingresults(smonitor, _state);
lbl_10:
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(state13->d.ptr.p_double[i], _state)/state->trustrad, _state);
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
        ae_trace("X0 (scaled) = ");
        tracevectorautoprec(curx, 0, n, _state);
        ae_trace("\n");
        ae_trace("D  (scaled) = ");
        tracevectorautoprec(&state13->d, 0, n, _state);
        ae_trace("\n");
        ae_trace("X1 (scaled) = ");
        tracevectorautoprec(&state13->stepkxn, 0, n, _state);
        ae_trace("\n");
    }
    ae_trace("meritF:         %14.6e -> %14.6e (delta=%11.3e)\n",
        (double)(f0),
        (double)(f1),
        (double)(f1-f0));
    ae_trace("scaled-targetF: %14.6e -> %14.6e (delta=%11.3e)\n",
        (double)(curfi->ptr.p_double[0]),
        (double)(state13->stepkfin.ptr.p_double[0]),
        (double)(state13->stepkfin.ptr.p_double[0]-curfi->ptr.p_double[0]));
lbl_8:
    
    /*
     * Move to new point
     */
    *stp = localstp;
    nlcslp_slpcopystate(state, &state13->stepkxn, &state13->stepkfin, &state13->stepkjn, curx, curfi, curj, _state);
    if( ae_fp_less_eq(localstp,(double)(0)) )
    {
        goto lbl_14;
    }
    
    /*
     * Report one more inner iteration
     */
    inc(&state->repinneriterationscount, _state);
    nlcslp_slpsendx(state, curx, _state);
    state->f = curfi->ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state13->rphase13state.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->xupdated = ae_false;
    
    /*
     * Update constraint violations
     */
    checklcviolation(&state->scaledcleic, &state->lcsrcidx, nec, nic, curx, n, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlcviolation(curfi, &state->fscales, nlec, nlic, &state->repnlcerr, &state->repnlcidx, _state);
lbl_14:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state13->rphase13state.ia.ptr.p_int[0] = n;
    state13->rphase13state.ia.ptr.p_int[1] = nslack;
    state13->rphase13state.ia.ptr.p_int[2] = nec;
    state13->rphase13state.ia.ptr.p_int[3] = nic;
    state13->rphase13state.ia.ptr.p_int[4] = nlec;
    state13->rphase13state.ia.ptr.p_int[5] = nlic;
    state13->rphase13state.ia.ptr.p_int[6] = innerk;
    state13->rphase13state.ia.ptr.p_int[7] = i;
    state13->rphase13state.ia.ptr.p_int[8] = j;
    state13->rphase13state.ba.ptr.p_bool[0] = dotrace;
    state13->rphase13state.ba.ptr.p_bool[1] = doprobing;
    state13->rphase13state.ba.ptr.p_bool[2] = dotracexd;
    state13->rphase13state.ra.ptr.p_double[0] = v;
    state13->rphase13state.ra.ptr.p_double[1] = mx;
    state13->rphase13state.ra.ptr.p_double[2] = f0;
    state13->rphase13state.ra.ptr.p_double[3] = f1;
    state13->rphase13state.ra.ptr.p_double[4] = nu;
    state13->rphase13state.ra.ptr.p_double[5] = localstp;
    state13->rphase13state.ra.ptr.p_double[6] = mu;
    return result;
}


/*************************************************************************
This function initializes Phase2   temporaries. It should be called before
beginning of each new iteration. You may call it multiple  times  for  the
same instance of Phase2 temporaries.

INPUT PARAMETERS:
    State2              -   instance to be initialized.
    N                   -   problem dimensionality
    NEC, NIC            -   linear equality/inequality constraint count
    NLEC, NLIC          -   nonlinear equality/inequality constraint count
    MeritLagMult        -   Lagrange multiplier estimates used by merit function
                            (we could use ones computed during phase #2,
                            but these may differ from ones computed
                            initially at the beginning of the outer
                            iteration, so it may confuse algorithm)

OUTPUT PARAMETERS:
    State2              -   instance being initialized

  -- ALGLIB --
     Copyright 05.02.2019 by Bochkanov Sergey
*************************************************************************/
static void nlcslp_phase2init(minslpphase2state* state2,
     ae_int_t n,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     /* Real    */ const ae_vector* meritlagmult,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nslack;


    nslack = n+2*(nec+nlec)+(nic+nlic);
    rvectorsetlengthatleast(&state2->d, nslack, _state);
    rvectorsetlengthatleast(&state2->tmp0, nslack, _state);
    rvectorsetlengthatleast(&state2->stepkxn, n, _state);
    rvectorsetlengthatleast(&state2->stepkxc, n, _state);
    rvectorsetlengthatleast(&state2->stepkfin, 1+nlec+nlic, _state);
    rvectorsetlengthatleast(&state2->stepkfic, 1+nlec+nlic, _state);
    rmatrixsetlengthatleast(&state2->stepkjn, 1+nlec+nlic, n, _state);
    rmatrixsetlengthatleast(&state2->stepkjc, 1+nlec+nlic, n, _state);
    rvectorsetlengthatleast(&state2->stepklaggrad, n, _state);
    rvectorsetlengthatleast(&state2->stepknlaggrad, n, _state);
    rvectorsetlengthatleast(&state2->stepknlagmult, nec+nic+nlec+nlic, _state);
    rvectorsetlengthatleast(&state2->meritlagmult, nec+nic+nlec+nlic, _state);
    for(i=0; i<=nec+nic+nlec+nlic-1; i++)
    {
        state2->meritlagmult.ptr.p_double[i] = meritlagmult->ptr.p_double[i];
    }
    ae_vector_set_length(&state2->rphase2state.ia, 12+1, _state);
    ae_vector_set_length(&state2->rphase2state.ba, 2+1, _state);
    ae_vector_set_length(&state2->rphase2state.ra, 9+1, _state);
    state2->rphase2state.stage = -1;
}


/*************************************************************************
This function tries to perform phase #2 iterations.

Phase #2 is  a  sequence  of  linearized  steps   minimizing  L2-penalized
Lagrangian  performed  with  successively  increasing  set  of   conjugacy
constraints (which make algorithm behavior similar to that of CG).

INPUT PARAMETERS:
    State       -   SLP solver state
    SMonitor    -   smoothness monitor
    UserTerminationNeeded-True if user requested termination
    CurX        -   current point, array[N]
    CurFi       -   function vector at CurX, array[1+NLEC+NLIC]
    CurJ        -   Jacobian at CurX, array[1+NLEC+NLIC,N]
    LagMult     -   array[NEC+NIC+NLEC+NLIC], contents ignored on input.
    GammaMax    -   current estimate of the Hessian norm

OUTPUT PARAMETERS:
    State       -   RepTerminationType is set to current termination code (if Status=0).
    CurX        -   advanced to new point
    CurFi       -   updated with function vector at CurX[]
    CurJ        -   updated with Jacobian at CurX[]
    LagMult     -   filled with current Lagrange multipliers
    GammaMax    -   updated estimate of the Hessian norm    
    Status      -   when reverse communication is done, Status is set to:
                    * negative value,  if   we  have   to  restart   outer
                      iteration
                    * positive value,  if we can proceed to the next stage
                      of the outer iteration
                    * zero, if algorithm is terminated (RepTerminationType
                      is set to appropriate value)

  -- ALGLIB --
     Copyright 05.02.2019 by Bochkanov Sergey
*************************************************************************/
static ae_bool nlcslp_phase2iteration(minslpstate* state,
     minslpphase2state* state2,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     /* Real    */ ae_vector* curx,
     /* Real    */ ae_vector* curfi,
     /* Real    */ ae_matrix* curj,
     /* Real    */ ae_vector* lagmult,
     double* gammamax,
     ae_int_t* status,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nslack;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    double stp;
    ae_int_t mcinfo;
    ae_int_t mcnfev;
    ae_int_t mcstage;
    ae_int_t i;
    ae_int_t j;
    ae_int_t innerk;
    double v;
    double vv;
    double mx;
    ae_int_t nondescentcnt;
    double stepklagval;
    double stepknlagval;
    double gammaprev;
    double f0;
    double f1;
    double mu;
    ae_bool dotrace;
    ae_bool doprobing;
    ae_bool dotracexd;
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
    if( state2->rphase2state.stage>=0 )
    {
        n = state2->rphase2state.ia.ptr.p_int[0];
        nslack = state2->rphase2state.ia.ptr.p_int[1];
        nec = state2->rphase2state.ia.ptr.p_int[2];
        nic = state2->rphase2state.ia.ptr.p_int[3];
        nlec = state2->rphase2state.ia.ptr.p_int[4];
        nlic = state2->rphase2state.ia.ptr.p_int[5];
        mcinfo = state2->rphase2state.ia.ptr.p_int[6];
        mcnfev = state2->rphase2state.ia.ptr.p_int[7];
        mcstage = state2->rphase2state.ia.ptr.p_int[8];
        i = state2->rphase2state.ia.ptr.p_int[9];
        j = state2->rphase2state.ia.ptr.p_int[10];
        innerk = state2->rphase2state.ia.ptr.p_int[11];
        nondescentcnt = state2->rphase2state.ia.ptr.p_int[12];
        dotrace = state2->rphase2state.ba.ptr.p_bool[0];
        doprobing = state2->rphase2state.ba.ptr.p_bool[1];
        dotracexd = state2->rphase2state.ba.ptr.p_bool[2];
        stp = state2->rphase2state.ra.ptr.p_double[0];
        v = state2->rphase2state.ra.ptr.p_double[1];
        vv = state2->rphase2state.ra.ptr.p_double[2];
        mx = state2->rphase2state.ra.ptr.p_double[3];
        stepklagval = state2->rphase2state.ra.ptr.p_double[4];
        stepknlagval = state2->rphase2state.ra.ptr.p_double[5];
        gammaprev = state2->rphase2state.ra.ptr.p_double[6];
        f0 = state2->rphase2state.ra.ptr.p_double[7];
        f1 = state2->rphase2state.ra.ptr.p_double[8];
        mu = state2->rphase2state.ra.ptr.p_double[9];
    }
    else
    {
        n = -819;
        nslack = -826;
        nec = 667;
        nic = 692;
        nlec = 84;
        nlic = 529;
        mcinfo = 14;
        mcnfev = 386;
        mcstage = -908;
        i = 577;
        j = 289;
        innerk = 317;
        nondescentcnt = 476;
        dotrace = ae_true;
        doprobing = ae_false;
        dotracexd = ae_true;
        stp = -962.0;
        v = 161.0;
        vv = -447.0;
        mx = -799.0;
        stepklagval = 508.0;
        stepknlagval = -153.0;
        gammaprev = -450.0;
        f0 = 769.0;
        f1 = 638.0;
        mu = -361.0;
    }
    if( state2->rphase2state.stage==0 )
    {
        goto lbl_0;
    }
    if( state2->rphase2state.stage==1 )
    {
        goto lbl_1;
    }
    if( state2->rphase2state.stage==2 )
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
    dotrace = ae_is_trace_enabled("SLP");
    dotracexd = dotrace&&ae_is_trace_enabled("SLP.DETAILED");
    doprobing = ae_is_trace_enabled("SLP.PROBING");
    ae_assert(lagmult->cnt>=nec+nic+nlec+nlic, "Phase13Iteration: integrity check failed", _state);
    
    /*
     * Report iteration beginning
     */
    if( dotrace )
    {
        ae_trace("\n--- linear step with conjugate constraints (CG-like convergence) -----------------------------------\n");
    }
    
    /*
     * The default decision is to continue iterations
     */
    *status = 1;
    
    /*
     * Perform inner LP subiterations.
     *
     * During this process we maintain information about several points:
     * * point #0, initial one, with "step0" prefix
     * * point #K, last one of current LP session, with "stepk" prefix
     * * additionally we have point #KN, current candidate during line search at step K.
     *
     * For each point we store:
     * * location X (scaled coordinates)
     * * function vector Fi (target function + nonlinear constraints)
     * * scaled Jacobian J
     */
    mu = ae_maxreal(rmaxabsv(state->historylen, &state->maxlaghistory, _state), rmaxabsv(nec+nic+nlec+nlic, &state->meritlagmult, _state), _state);
    mu = coalesce(mu, nlcslp_defaultl1penalty, _state);
    nondescentcnt = 0;
    nlcslp_lpsubproblemrestart(state, &state->subsolver, _state);
    innerk = 1;
lbl_3:
    if( innerk>n )
    {
        goto lbl_5;
    }
    
    /*
     * Formulate LP subproblem and solve it
     */
    if( !nlcslp_lpsubproblemsolve(state, &state->subsolver, curx, curfi, curj, innerk, &state2->d, lagmult, _state) )
    {
        
        /*
         * LP solver failed due to numerical errors; exit.
         * It may happen when we solve problem with LOTS of conjugacy constraints.
         */
        if( innerk==1 )
        {
            
            /*
             * The very first iteration failed, really strange.
             */
            if( dotrace )
            {
                ae_trace("[WARNING] the very first LP subproblem failed to produce descent direction\n");
            }
        }
        else
        {
            
            /*
             * Quite a normal, the problem is overconstrained by conjugacy constraints now
             */
            if( dotrace )
            {
                ae_trace("> LP subproblem is overconstrained (happens after too many iterations), time to stop\n");
            }
        }
        result = ae_false;
        return result;
    }
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(state2->d.ptr.p_double[i], _state)/state->trustrad, _state);
    }
    if( ae_fp_eq(mx,(double)(0)) )
    {
        
        /*
         * Nearly-zero direction is suggested (maybe we arrived exactly to the solution), stop iterations
         */
        *status = 1;
        nlcslp_slpcopystate(state, curx, curfi, curj, &state2->stepkxn, &state2->stepkfin, &state2->stepkjn, _state);
        if( dotrace )
        {
            ae_trace("> LP subproblem suggested nearly zero step\n");
        }
        if( dotrace )
        {
            ae_trace("max(|Di|)/TrustRad = %0.6f\n",
                (double)(mx));
        }
        if( dotrace )
        {
            ae_trace("> stopping CG-like iterations\n");
        }
        result = ae_false;
        return result;
    }
    nlcslp_lpsubproblemappendconjugacyconstraint(state, &state->subsolver, &state2->d, _state);
    
    /*
     * Perform line search to minimize Lagrangian along D.
     * Post-normalize StepKXN with respect to box constraints.
     *
     * MCSRCH can fail in the following cases:
     * * rounding errors prevent optimization
     * * non-descent direction is specified (MCINFO=0 is returned)
     * In the latter case we proceed to minimization of merit function.
     *
     * NOTE: constraint violation reports are updated during Lagrangian computation
     */
    state2->lastlcerr = (double)(0);
    state2->lastlcidx = -1;
    state2->lastnlcerr = (double)(0);
    state2->lastnlcidx = -1;
    rvectorsetlengthatleast(&state2->tmp0, n, _state);
    nlcslp_lagrangianfg(state, curx, state->trustrad, curfi, curj, lagmult, &state2->tmplagrangianfg, &stepklagval, &state2->stepklaggrad, &state2->lastlcerr, &state2->lastlcidx, &state2->lastnlcerr, &state2->lastnlcidx, _state);
    nlcslp_slpcopystate(state, curx, curfi, curj, &state2->stepkxn, &state2->stepkfin, &state2->stepkjn, _state);
    v = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        state2->stepknlaggrad.ptr.p_double[i] = state2->stepklaggrad.ptr.p_double[i];
        v = v+state2->d.ptr.p_double[i]*state2->stepklaggrad.ptr.p_double[i];
    }
    if( ae_fp_greater_eq(v,(double)(0)) )
    {
        
        /*
         * Non-descent direction D was specified; it may happen because LP subproblem favors
         * directions which decrease L1 penalty and default augmentation of Lagrangian involves
         * only L2 term.
         *
         * Append direction to the conjugacy constraints and retry direction generation.
         *
         * We make several retries with conjugate directions before giving up.
         */
        if( dotrace )
        {
            ae_trace("> LP subproblem suggested nondescent step, skipping it (dLag=%0.3e)\n",
                (double)(v));
        }
        inc(&nondescentcnt, _state);
        if( nlcslp_nondescentlimit>0&&nondescentcnt>nlcslp_nondescentlimit )
        {
            if( dotrace )
            {
                ae_trace("> too many nondescent steps, stopping CG-like iterations\n");
            }
            *status = 1;
            result = ae_false;
            return result;
        }
        goto lbl_4;
    }
    smoothnessmonitorstartlinesearch(smonitor, curx, curfi, curj, state->repinneriterationscount, state->repouteriterationscount, _state);
    stepknlagval = stepklagval;
    mcnfev = 0;
    mcstage = 0;
    stp = 1.0;
    mcsrch(n, &state2->stepkxn, &stepknlagval, &state2->stepknlaggrad, &state2->d, &stp, 1.0, nlcslp_slpgtol, &mcinfo, &mcnfev, &state2->tmp0, &state2->mcstate, &mcstage, _state);
lbl_6:
    if( mcstage==0 )
    {
        goto lbl_7;
    }
    nlcslp_slpsendx(state, &state2->stepkxn, _state);
    state->needfij = ae_true;
    state2->rphase2state.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    if( !nlcslp_slpretrievefij(state, &state2->stepkfin, &state2->stepkjn, _state) )
    {
        
        /*
         * Failed to retrieve func/Jac, infinities detected
         */
        *status = 0;
        state->repterminationtype = -8;
        if( dotrace )
        {
            ae_trace("[ERROR] infinities in target/constraints are detected\n");
        }
        result = ae_false;
        return result;
    }
    smoothnessmonitorenqueuepoint(smonitor, &state2->d, stp, &state2->stepkxn, &state2->stepkfin, &state2->stepkjn, _state);
    nlcslp_lagrangianfg(state, &state2->stepkxn, state->trustrad, &state2->stepkfin, &state2->stepkjn, lagmult, &state2->tmplagrangianfg, &stepknlagval, &state2->stepknlaggrad, &state2->lastlcerr, &state2->lastlcidx, &state2->lastnlcerr, &state2->lastnlcidx, _state);
    mcsrch(n, &state2->stepkxn, &stepknlagval, &state2->stepknlaggrad, &state2->d, &stp, 1.0, nlcslp_slpgtol, &mcinfo, &mcnfev, &state2->tmp0, &state2->mcstate, &mcstage, _state);
    goto lbl_6;
lbl_7:
    smoothnessmonitorfinalizelinesearch(smonitor, _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state2->stepkxn.ptr.p_double[i] = ae_maxreal(state2->stepkxn.ptr.p_double[i], state->scaledbndl.ptr.p_double[i], _state);
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state2->stepkxn.ptr.p_double[i] = ae_minreal(state2->stepkxn.ptr.p_double[i], state->scaledbndu.ptr.p_double[i], _state);
        }
    }
    if( mcinfo<=0 )
    {
        
        /*
         * Line search failed miserably, terminate
         */
        *status = 1;
        if( innerk==1 )
        {
            
            /*
             * The very first iteration failed, really strange.
             * Let's decrease trust radius and try one more time.
             */
            state->trustrad = 0.5*state->trustrad;
            if( dotrace )
            {
                ae_trace("> line search failed miserably for unknown reason, decreasing trust radius\n");
            }
            if( ae_fp_less(state->trustrad,state->epsx) )
            {
                state->repterminationtype = 2;
                *status = 0;
                if( dotrace )
                {
                    ae_trace("> stopping condition met: trust radius is smaller than %0.3e\n",
                        (double)(state->epsx));
                }
            }
        }
        else
        {
            
            /*
             * Well, it can be normal
             */
            if( dotrace )
            {
                ae_trace("> line search failed miserably for unknown reason, proceeding further\n");
            }
        }
        result = ae_false;
        return result;
    }
    if( mcinfo==1 )
    {
        hessianupdatev2(&state->subsolver.hess, curx, &state2->stepklaggrad, &state2->stepkxn, &state2->stepknlaggrad, 2, ae_false, dotrace, 1, _state);
    }
    
    /*
     * Update GammaMax - estimate of the function Hessian norm
     */
    v = (double)(0);
    vv = (double)(0);
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(state2->stepkxn.ptr.p_double[i]-curx->ptr.p_double[i], _state), _state);
        v = v+ae_sqr(state2->stepkxn.ptr.p_double[i]-curx->ptr.p_double[i], _state);
        vv = vv+(state2->stepkjn.ptr.pp_double[0][i]-curj->ptr.pp_double[0][i])*(state2->stepkxn.ptr.p_double[i]-curx->ptr.p_double[i]);
    }
    gammaprev = *gammamax;
    if( ae_fp_greater(mx,nlcslp_bfgstol) )
    {
        *gammamax = ae_maxreal(*gammamax, ae_fabs(vv/v, _state), _state);
    }
    
    /*
     * Trace
     */
    if( !dotrace )
    {
        goto lbl_8;
    }
    if( !doprobing )
    {
        goto lbl_10;
    }
    smoothnessmonitorstartlagrangianprobing(smonitor, curx, &state2->d, 1.0, innerk, state->repouteriterationscount, _state);
lbl_12:
    if( !smoothnessmonitorprobelagrangian(smonitor, _state) )
    {
        goto lbl_13;
    }
    for(j=0; j<=n-1; j++)
    {
        state2->stepkxc.ptr.p_double[j] = smonitor->lagprobx.ptr.p_double[j];
        if( state->hasbndl.ptr.p_bool[j] )
        {
            state2->stepkxc.ptr.p_double[j] = ae_maxreal(state2->stepkxc.ptr.p_double[j], state->scaledbndl.ptr.p_double[j], _state);
        }
        if( state->hasbndu.ptr.p_bool[j] )
        {
            state2->stepkxc.ptr.p_double[j] = ae_minreal(state2->stepkxc.ptr.p_double[j], state->scaledbndu.ptr.p_double[j], _state);
        }
    }
    nlcslp_slpsendx(state, &state2->stepkxc, _state);
    state->needfij = ae_true;
    state2->rphase2state.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfij = ae_false;
    if( !nlcslp_slpretrievefij(state, &smonitor->lagprobfi, &smonitor->lagprobj, _state) )
    {
        goto lbl_13;
    }
    smonitor->lagprobrawlag = nlcslp_rawlagrangian(state, &state2->stepkxc, &smonitor->lagprobfi, lagmult, &state2->tmpmerit, _state);
    goto lbl_12;
lbl_13:
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |   probing search direction suggested by LP subproblem    |\n");
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |  Step  | Lagrangian (unaugmentd)|    Target  function    |\n");
    ae_trace("*** |along  D|     must be smooth     |     must be smooth     |\n");
    ae_trace("*** |        | function   |    slope  | function   |    slope  |\n");
    smoothnessmonitortracelagrangianprobingresults(smonitor, _state);
lbl_10:
    mx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(state2->d.ptr.p_double[i], _state)/state->trustrad, _state);
    }
    f0 = nlcslp_meritfunction(state, curx, curfi, &state2->meritlagmult, mu, &state2->tmpmerit, _state);
    f1 = nlcslp_meritfunction(state, &state2->stepkxn, &state2->stepkfin, &state2->meritlagmult, mu, &state2->tmpmerit, _state);
    ae_trace("> LP subproblem produced good direction, minimization was performed\n");
    ae_trace("max(|Di|)/TrustRad = %0.6f\n",
        (double)(mx));
    ae_trace("stp = %0.6f\n",
        (double)(stp));
    if( dotracexd )
    {
        ae_trace("X0 = ");
        tracevectorautoprec(curx, 0, n, _state);
        ae_trace("\n");
        ae_trace("D  = ");
        tracevectorautoprec(&state2->d, 0, n, _state);
        ae_trace("\n");
        ae_trace("X1 = X0 + stp*D\n");
        ae_trace("   = ");
        tracevectorautoprec(&state2->stepkxn, 0, n, _state);
        ae_trace("\n");
    }
    ae_trace("meritF:         %14.6e -> %14.6e (delta=%11.3e)\n",
        (double)(f0),
        (double)(f1),
        (double)(f1-f0));
    ae_trace("scaled-targetF: %14.6e -> %14.6e (delta=%11.3e)\n",
        (double)(curfi->ptr.p_double[0]),
        (double)(state2->stepkfin.ptr.p_double[0]),
        (double)(state2->stepkfin.ptr.p_double[0]-curfi->ptr.p_double[0]));
    ae_trace("aug.Lagrangian: %14.6e -> %14.6e (delta=%11.3e)\n",
        (double)(stepklagval),
        (double)(stepknlagval),
        (double)(stepknlagval-stepklagval));
    if( ae_fp_greater(*gammamax,gammaprev) )
    {
        ae_trace("|H| = %0.3e (Hessian norm increased)\n",
            (double)(*gammamax));
    }
lbl_8:
    
    /*
     * Check status of the termination request
     * Update current point
     * Update constraint status.
     * Report iteration.
     */
    if( userterminationneeded )
    {
        
        /*
         * User requested termination, break before we move to new point
         */
        *status = 0;
        state->repterminationtype = 8;
        if( dotrace )
        {
            ae_trace("# user requested termination\n");
        }
        result = ae_false;
        return result;
    }
    nlcslp_slpcopystate(state, &state2->stepkxn, &state2->stepkfin, &state2->stepkjn, curx, curfi, curj, _state);
    state->replcerr = state2->lastlcerr;
    state->replcidx = state2->lastlcidx;
    state->repnlcerr = state2->lastnlcerr;
    state->repnlcidx = state2->lastnlcidx;
    inc(&state->repinneriterationscount, _state);
    nlcslp_slpsendx(state, curx, _state);
    state->f = curfi->ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state2->rphase2state.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->xupdated = ae_false;
    
    /*
     * Terminate inner LP subiterations
     */
    if( state->maxits>0&&state->repinneriterationscount>=state->maxits )
    {
        
        /*
         * Iteration limit exhausted
         */
        *status = 1;
        if( dotrace )
        {
            ae_trace("# stopping criteria met (MaxIts iterations performed)\n");
        }
        result = ae_false;
        return result;
    }
    if( ae_fp_greater_eq(stp,nlcslp_slpstpclosetoone) )
    {
        
        /*
         * Step is close to 1.0, either of two is likely:
         * * we move through nearly linear region of F()
         * * we try to enforce some strongly violated constraint
         *
         * In any case, authors of the original algorithm recommend to break inner LP
         * iteration and proceed to test of sufficient decrease of merit function.
         */
        *status = 1;
        if( dotrace )
        {
            ae_trace("> step is close to 1, stopping iterations\n");
        }
        result = ae_false;
        return result;
    }
    if( (mcinfo!=1&&mcinfo!=3)&&mcinfo!=5 )
    {
        
        /*
         * Line search ended with "bad" MCINFO
         * (neither sufficient decrease, neither maximum step);
         * terminate.
         */
        *status = 1;
        if( dotrace )
        {
            ae_trace("> line search ended with bad MCINFO, no more CG-like iterations\n");
        }
        result = ae_false;
        return result;
    }
lbl_4:
    innerk = innerk+1;
    goto lbl_3;
lbl_5:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state2->rphase2state.ia.ptr.p_int[0] = n;
    state2->rphase2state.ia.ptr.p_int[1] = nslack;
    state2->rphase2state.ia.ptr.p_int[2] = nec;
    state2->rphase2state.ia.ptr.p_int[3] = nic;
    state2->rphase2state.ia.ptr.p_int[4] = nlec;
    state2->rphase2state.ia.ptr.p_int[5] = nlic;
    state2->rphase2state.ia.ptr.p_int[6] = mcinfo;
    state2->rphase2state.ia.ptr.p_int[7] = mcnfev;
    state2->rphase2state.ia.ptr.p_int[8] = mcstage;
    state2->rphase2state.ia.ptr.p_int[9] = i;
    state2->rphase2state.ia.ptr.p_int[10] = j;
    state2->rphase2state.ia.ptr.p_int[11] = innerk;
    state2->rphase2state.ia.ptr.p_int[12] = nondescentcnt;
    state2->rphase2state.ba.ptr.p_bool[0] = dotrace;
    state2->rphase2state.ba.ptr.p_bool[1] = doprobing;
    state2->rphase2state.ba.ptr.p_bool[2] = dotracexd;
    state2->rphase2state.ra.ptr.p_double[0] = stp;
    state2->rphase2state.ra.ptr.p_double[1] = v;
    state2->rphase2state.ra.ptr.p_double[2] = vv;
    state2->rphase2state.ra.ptr.p_double[3] = mx;
    state2->rphase2state.ra.ptr.p_double[4] = stepklagval;
    state2->rphase2state.ra.ptr.p_double[5] = stepknlagval;
    state2->rphase2state.ra.ptr.p_double[6] = gammaprev;
    state2->rphase2state.ra.ptr.p_double[7] = f0;
    state2->rphase2state.ra.ptr.p_double[8] = f1;
    state2->rphase2state.ra.ptr.p_double[9] = mu;
    return result;
}


/*************************************************************************
Copies X to State.X
*************************************************************************/
static void nlcslp_slpsendx(minslpstate* state,
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
static ae_bool nlcslp_slpretrievefij(const minslpstate* state,
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
        v = 0.1*v+fis->ptr.p_double[i];
        for(j=0; j<=n-1; j++)
        {
            js->ptr.pp_double[i][j] = vv*state->j.ptr.pp_double[i][j];
            v = 0.1*v+js->ptr.pp_double[i][j];
        }
    }
    result = ae_isfinite(v, _state);
    return result;
}


/*************************************************************************
Copies state (X point, Fi vector, J jacobian) to preallocated storage.
*************************************************************************/
static void nlcslp_slpcopystate(const minslpstate* state,
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
static void nlcslp_lagrangianfg(minslpstate* state,
     /* Real    */ const ae_vector* x,
     double trustrad,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* j,
     /* Real    */ const ae_vector* lagmult,
     minslptmplagrangian* tmp,
     double* f,
     /* Real    */ ae_vector* g,
     double* lcerr,
     ae_int_t* lcidx,
     double* nlcerr,
     ae_int_t* nlcidx,
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
    double vviolate;
    ae_bool usesparsegemv;
    double dampingfactor;

    *f = 0.0;
    *lcerr = 0.0;
    *lcidx = 0;
    *nlcerr = 0.0;
    *nlcidx = 0;

    n = state->n;
    nec = state->nec;
    nic = state->nic;
    nlec = state->nlec;
    nlic = state->nlic;
    dampingfactor = nlcslp_inequalitydampingfactor/trustrad;
    
    /*
     * Prepare constraint violation report
     */
    *lcerr = (double)(0);
    *lcidx = -1;
    *nlcerr = (double)(0);
    *nlcidx = -1;
    
    /*
     * Target function
     */
    *f = fi->ptr.p_double[0];
    for(i=0; i<=n-1; i++)
    {
        g->ptr.p_double[i] = j->ptr.pp_double[0][i];
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
             * Estimate constraint value at the point, update violation info
             *
             * NOTE: here we expect that scaledCLEIC[] has normalized rows
             */
            v = tmp->sclagtmp0.ptr.p_double[i]-state->scaledcleic.ptr.pp_double[i][n];
            if( i<nec||v>(double)0 )
            {
                
                /*
                 * Either equality constraint or violated inequality one.
                 * Update violation report.
                 */
                vviolate = ae_fabs(v, _state);
                if( vviolate>*lcerr )
                {
                    *lcerr = vviolate;
                    *lcidx = state->lcsrcidx.ptr.p_int[i];
                }
            }
            
            /*
             * Prepare
             */
            vlag = lagmult->ptr.p_double[i];
            tmp->sclagtmp1.ptr.p_double[i] = (double)(0);
            
            /*
             * Primary Lagrangian term
             */
            if( i<nec||v>(double)0 )
            {
                vact = v;
                vd = (double)(1);
            }
            else
            {
                vd = (double)1/((double)1-dampingfactor*v);
                vact = v*vd;
                vd = vd*vd;
            }
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
            *f = *f+0.5*nlcslp_augmentationfactor*vact*vact;
            tmp->sclagtmp1.ptr.p_double[i] = tmp->sclagtmp1.ptr.p_double[i]+nlcslp_augmentationfactor*vact;
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
        if( i<nlec||v>(double)0 )
        {
            
            /*
             * Either equality constraint or violated inequality one.
             * Update violation report.
             */
            vviolate = ae_fabs(v, _state)*state->fscales.ptr.p_double[1+i];
            if( vviolate>*nlcerr )
            {
                *nlcerr = vviolate;
                *nlcidx = i;
            }
        }
        vlag = lagmult->ptr.p_double[nec+nic+i];
        tmp->sclagtmp1.ptr.p_double[i] = (double)(0);
        
        /*
         * Lagrangian term
         */
        if( i<nlec||v>(double)0 )
        {
            vact = v;
            vd = (double)(1);
        }
        else
        {
            vd = (double)1/((double)1-dampingfactor*v);
            vact = v*vd;
            vd = vd*vd;
        }
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
        *f = *f+0.5*nlcslp_augmentationfactor*vact*vact;
        tmp->sclagtmp1.ptr.p_double[i] = tmp->sclagtmp1.ptr.p_double[i]+nlcslp_augmentationfactor*vact;
    }
    rmatrixgemv(n, nlec+nlic, 1.0, j, 1, 0, 1, &tmp->sclagtmp1, 0, 1.0, g, 0, _state);
}


/*************************************************************************
This function calculates L1-penalized merit function
*************************************************************************/
static double nlcslp_meritfunction(minslpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagmult,
     double mu,
     minslptmpmerit* tmp,
     ae_state *_state)
{
    double tmp0;
    double tmp1;
    double result;


    nlcslp_meritfunctionandrawlagrangian(state, x, fi, lagmult, mu, tmp, &tmp0, &tmp1, _state);
    result = tmp0;
    return result;
}


/*************************************************************************
This function calculates raw (unaugmented and smooth) Lagrangian
*************************************************************************/
static double nlcslp_rawlagrangian(minslpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagmult,
     minslptmpmerit* tmp,
     ae_state *_state)
{
    double tmp0;
    double tmp1;
    double result;


    nlcslp_meritfunctionandrawlagrangian(state, x, fi, lagmult, 0.0, tmp, &tmp0, &tmp1, _state);
    result = tmp1;
    return result;
}


/*************************************************************************
This function calculates L1-penalized merit function and raw  (smooth  and
un-augmented) Lagrangian
*************************************************************************/
static void nlcslp_meritfunctionandrawlagrangian(minslpstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* lagmult,
     double mu,
     minslptmpmerit* tmp,
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
            *meritf = *meritf+nlcslp_meritfunctionbase*ae_fabs(v, _state)+nlcslp_meritfunctiongain*mu*ae_fabs(v, _state);
            
            /*
             * Raw Lagrangian
             */
            *rawlag = *rawlag+lagmult->ptr.p_double[i]*v;
        }
        else
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            *meritf = *meritf+nlcslp_meritfunctionbase*ae_maxreal(v, (double)(0), _state)+nlcslp_meritfunctiongain*mu*ae_maxreal(v, (double)(0), _state);
            
            /*
             * Raw Lagrangian
             */
            *rawlag = *rawlag+lagmult->ptr.p_double[i]*v;
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
            *meritf = *meritf+nlcslp_meritfunctionbase*ae_fabs(v, _state)+nlcslp_meritfunctiongain*mu*ae_fabs(v, _state);
            
            /*
             * Raw Lagrangian
             */
            *rawlag = *rawlag+lagmult->ptr.p_double[nec+nic+i]*v;
        }
        else
        {
            
            /*
             * Merit function: augmentation term + L1 penalty term
             */
            *meritf = *meritf+nlcslp_meritfunctionbase*ae_maxreal(v, (double)(0), _state)+nlcslp_meritfunctiongain*mu*ae_maxreal(v, (double)(0), _state);
            
            /*
             * Raw Lagrangian
             */
            *rawlag = *rawlag+lagmult->ptr.p_double[nec+nic+i]*v;
        }
    }
}


void _minslpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minslpsubsolver *p = (minslpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _presolveinfo_init(&p->presolver, _state, make_automatic);
    _dualsimplexstate_init(&p->dss, _state, make_automatic);
    _dualsimplexsettings_init(&p->dsssettings, _state, make_automatic);
    _dualsimplexbasis_init(&p->lastbasis, _state, make_automatic);
    ae_matrix_init(&p->curd, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cural, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->curau, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparserawlc, _state, make_automatic);
    _sparsematrix_init(&p->sparseefflc, _state, make_automatic);
    _xbfgshessian_init(&p->hess, _state, make_automatic);
    ae_matrix_init(&p->curhd, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->densedummy, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsedummy, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->yk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->laglc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagbc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cs, 0, DT_INT, _state, make_automatic);
}


void _minslpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minslpsubsolver       *dst = (minslpsubsolver*)_dst;
    const minslpsubsolver *src = (const minslpsubsolver*)_src;
    _presolveinfo_init_copy(&dst->presolver, &src->presolver, _state, make_automatic);
    _dualsimplexstate_init_copy(&dst->dss, &src->dss, _state, make_automatic);
    _dualsimplexsettings_init_copy(&dst->dsssettings, &src->dsssettings, _state, make_automatic);
    _dualsimplexbasis_init_copy(&dst->lastbasis, &src->lastbasis, _state, make_automatic);
    dst->basispresent = src->basispresent;
    ae_matrix_init_copy(&dst->curd, &src->curd, _state, make_automatic);
    dst->curdcnt = src->curdcnt;
    ae_vector_init_copy(&dst->curb, &src->curb, _state, make_automatic);
    ae_vector_init_copy(&dst->curbndl, &src->curbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->curbndu, &src->curbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->cural, &src->cural, _state, make_automatic);
    ae_vector_init_copy(&dst->curau, &src->curau, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparserawlc, &src->sparserawlc, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseefflc, &src->sparseefflc, _state, make_automatic);
    _xbfgshessian_init_copy(&dst->hess, &src->hess, _state, make_automatic);
    ae_matrix_init_copy(&dst->curhd, &src->curhd, _state, make_automatic);
    ae_matrix_init_copy(&dst->densedummy, &src->densedummy, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsedummy, &src->sparsedummy, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->sk, &src->sk, _state, make_automatic);
    ae_vector_init_copy(&dst->yk, &src->yk, _state, make_automatic);
    ae_vector_init_copy(&dst->xs, &src->xs, _state, make_automatic);
    ae_vector_init_copy(&dst->laglc, &src->laglc, _state, make_automatic);
    ae_vector_init_copy(&dst->lagbc, &src->lagbc, _state, make_automatic);
    ae_vector_init_copy(&dst->cs, &src->cs, _state, make_automatic);
}


void _minslpsubsolver_clear(void* _p)
{
    minslpsubsolver *p = (minslpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _presolveinfo_clear(&p->presolver);
    _dualsimplexstate_clear(&p->dss);
    _dualsimplexsettings_clear(&p->dsssettings);
    _dualsimplexbasis_clear(&p->lastbasis);
    ae_matrix_clear(&p->curd);
    ae_vector_clear(&p->curb);
    ae_vector_clear(&p->curbndl);
    ae_vector_clear(&p->curbndu);
    ae_vector_clear(&p->cural);
    ae_vector_clear(&p->curau);
    _sparsematrix_clear(&p->sparserawlc);
    _sparsematrix_clear(&p->sparseefflc);
    _xbfgshessian_clear(&p->hess);
    ae_matrix_clear(&p->curhd);
    ae_matrix_clear(&p->densedummy);
    _sparsematrix_clear(&p->sparsedummy);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->sk);
    ae_vector_clear(&p->yk);
    ae_vector_clear(&p->xs);
    ae_vector_clear(&p->laglc);
    ae_vector_clear(&p->lagbc);
    ae_vector_clear(&p->cs);
}


void _minslpsubsolver_destroy(void* _p)
{
    minslpsubsolver *p = (minslpsubsolver*)_p;
    ae_touch_ptr((void*)p);
    _presolveinfo_destroy(&p->presolver);
    _dualsimplexstate_destroy(&p->dss);
    _dualsimplexsettings_destroy(&p->dsssettings);
    _dualsimplexbasis_destroy(&p->lastbasis);
    ae_matrix_destroy(&p->curd);
    ae_vector_destroy(&p->curb);
    ae_vector_destroy(&p->curbndl);
    ae_vector_destroy(&p->curbndu);
    ae_vector_destroy(&p->cural);
    ae_vector_destroy(&p->curau);
    _sparsematrix_destroy(&p->sparserawlc);
    _sparsematrix_destroy(&p->sparseefflc);
    _xbfgshessian_destroy(&p->hess);
    ae_matrix_destroy(&p->curhd);
    ae_matrix_destroy(&p->densedummy);
    _sparsematrix_destroy(&p->sparsedummy);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->sk);
    ae_vector_destroy(&p->yk);
    ae_vector_destroy(&p->xs);
    ae_vector_destroy(&p->laglc);
    ae_vector_destroy(&p->lagbc);
    ae_vector_destroy(&p->cs);
}


void _minslptmplagrangian_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minslptmplagrangian *p = (minslptmplagrangian*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->sclagtmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sclagtmp1, 0, DT_REAL, _state, make_automatic);
}


void _minslptmplagrangian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minslptmplagrangian       *dst = (minslptmplagrangian*)_dst;
    const minslptmplagrangian *src = (const minslptmplagrangian*)_src;
    ae_vector_init_copy(&dst->sclagtmp0, &src->sclagtmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->sclagtmp1, &src->sclagtmp1, _state, make_automatic);
}


void _minslptmplagrangian_clear(void* _p)
{
    minslptmplagrangian *p = (minslptmplagrangian*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->sclagtmp0);
    ae_vector_clear(&p->sclagtmp1);
}


void _minslptmplagrangian_destroy(void* _p)
{
    minslptmplagrangian *p = (minslptmplagrangian*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->sclagtmp0);
    ae_vector_destroy(&p->sclagtmp1);
}


void _minslptmpmerit_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minslptmpmerit *p = (minslptmpmerit*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->mftmp0, 0, DT_REAL, _state, make_automatic);
}


void _minslptmpmerit_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minslptmpmerit       *dst = (minslptmpmerit*)_dst;
    const minslptmpmerit *src = (const minslptmpmerit*)_src;
    ae_vector_init_copy(&dst->mftmp0, &src->mftmp0, _state, make_automatic);
}


void _minslptmpmerit_clear(void* _p)
{
    minslptmpmerit *p = (minslptmpmerit*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->mftmp0);
}


void _minslptmpmerit_destroy(void* _p)
{
    minslptmpmerit *p = (minslptmpmerit*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->mftmp0);
}


void _minslpphase13state_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minslpphase13state *p = (minslpphase13state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkxc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkxn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfic, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfin, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkjc, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkjn, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummylagmult, 0, DT_REAL, _state, make_automatic);
    _minslptmpmerit_init(&p->tmpmerit, _state, make_automatic);
    _rcommstate_init(&p->rphase13state, _state, make_automatic);
}


void _minslpphase13state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minslpphase13state       *dst = (minslpphase13state*)_dst;
    const minslpphase13state *src = (const minslpphase13state*)_src;
    dst->usecorrection = src->usecorrection;
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    ae_vector_init_copy(&dst->dx, &src->dx, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkxc, &src->stepkxc, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkxn, &src->stepkxn, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfic, &src->stepkfic, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfin, &src->stepkfin, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkjc, &src->stepkjc, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkjn, &src->stepkjn, _state, make_automatic);
    ae_vector_init_copy(&dst->dummylagmult, &src->dummylagmult, _state, make_automatic);
    _minslptmpmerit_init_copy(&dst->tmpmerit, &src->tmpmerit, _state, make_automatic);
    _rcommstate_init_copy(&dst->rphase13state, &src->rphase13state, _state, make_automatic);
}


void _minslpphase13state_clear(void* _p)
{
    minslpphase13state *p = (minslpphase13state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->dx);
    ae_vector_clear(&p->stepkxc);
    ae_vector_clear(&p->stepkxn);
    ae_vector_clear(&p->stepkfic);
    ae_vector_clear(&p->stepkfin);
    ae_matrix_clear(&p->stepkjc);
    ae_matrix_clear(&p->stepkjn);
    ae_vector_clear(&p->dummylagmult);
    _minslptmpmerit_clear(&p->tmpmerit);
    _rcommstate_clear(&p->rphase13state);
}


void _minslpphase13state_destroy(void* _p)
{
    minslpphase13state *p = (minslpphase13state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->dx);
    ae_vector_destroy(&p->stepkxc);
    ae_vector_destroy(&p->stepkxn);
    ae_vector_destroy(&p->stepkfic);
    ae_vector_destroy(&p->stepkfin);
    ae_matrix_destroy(&p->stepkjc);
    ae_matrix_destroy(&p->stepkjn);
    ae_vector_destroy(&p->dummylagmult);
    _minslptmpmerit_destroy(&p->tmpmerit);
    _rcommstate_destroy(&p->rphase13state);
}


void _minslpphase2state_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minslpphase2state *p = (minslpphase2state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->stepkxn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkxc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfic, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkjn, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkjc, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepklaggrad, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepknlaggrad, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepknlagmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->meritlagmult, 0, DT_REAL, _state, make_automatic);
    _minslptmplagrangian_init(&p->tmplagrangianfg, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    _linminstate_init(&p->mcstate, _state, make_automatic);
    _minslptmpmerit_init(&p->tmpmerit, _state, make_automatic);
    _rcommstate_init(&p->rphase2state, _state, make_automatic);
}


void _minslpphase2state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minslpphase2state       *dst = (minslpphase2state*)_dst;
    const minslpphase2state *src = (const minslpphase2state*)_src;
    ae_vector_init_copy(&dst->stepkxn, &src->stepkxn, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkxc, &src->stepkxc, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfin, &src->stepkfin, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfic, &src->stepkfic, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkjn, &src->stepkjn, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkjc, &src->stepkjc, _state, make_automatic);
    ae_vector_init_copy(&dst->stepklaggrad, &src->stepklaggrad, _state, make_automatic);
    ae_vector_init_copy(&dst->stepknlaggrad, &src->stepknlaggrad, _state, make_automatic);
    ae_vector_init_copy(&dst->stepknlagmult, &src->stepknlagmult, _state, make_automatic);
    ae_vector_init_copy(&dst->meritlagmult, &src->meritlagmult, _state, make_automatic);
    _minslptmplagrangian_init_copy(&dst->tmplagrangianfg, &src->tmplagrangianfg, _state, make_automatic);
    dst->lastlcerr = src->lastlcerr;
    dst->lastlcidx = src->lastlcidx;
    dst->lastnlcerr = src->lastnlcerr;
    dst->lastnlcidx = src->lastnlcidx;
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    _linminstate_init_copy(&dst->mcstate, &src->mcstate, _state, make_automatic);
    _minslptmpmerit_init_copy(&dst->tmpmerit, &src->tmpmerit, _state, make_automatic);
    _rcommstate_init_copy(&dst->rphase2state, &src->rphase2state, _state, make_automatic);
}


void _minslpphase2state_clear(void* _p)
{
    minslpphase2state *p = (minslpphase2state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->stepkxn);
    ae_vector_clear(&p->stepkxc);
    ae_vector_clear(&p->stepkfin);
    ae_vector_clear(&p->stepkfic);
    ae_matrix_clear(&p->stepkjn);
    ae_matrix_clear(&p->stepkjc);
    ae_vector_clear(&p->stepklaggrad);
    ae_vector_clear(&p->stepknlaggrad);
    ae_vector_clear(&p->stepknlagmult);
    ae_vector_clear(&p->meritlagmult);
    _minslptmplagrangian_clear(&p->tmplagrangianfg);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->d);
    _linminstate_clear(&p->mcstate);
    _minslptmpmerit_clear(&p->tmpmerit);
    _rcommstate_clear(&p->rphase2state);
}


void _minslpphase2state_destroy(void* _p)
{
    minslpphase2state *p = (minslpphase2state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->stepkxn);
    ae_vector_destroy(&p->stepkxc);
    ae_vector_destroy(&p->stepkfin);
    ae_vector_destroy(&p->stepkfic);
    ae_matrix_destroy(&p->stepkjn);
    ae_matrix_destroy(&p->stepkjc);
    ae_vector_destroy(&p->stepklaggrad);
    ae_vector_destroy(&p->stepknlaggrad);
    ae_vector_destroy(&p->stepknlagmult);
    ae_vector_destroy(&p->meritlagmult);
    _minslptmplagrangian_destroy(&p->tmplagrangianfg);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->d);
    _linminstate_destroy(&p->mcstate);
    _minslptmpmerit_destroy(&p->tmpmerit);
    _rcommstate_destroy(&p->rphase2state);
}


void _minslpstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minslpstate *p = (minslpstate*)_p;
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
    _minslpphase13state_init(&p->state13, _state, make_automatic);
    _minslpphase2state_init(&p->state2, _state, make_automatic);
    ae_vector_init(&p->step0x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->backupx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->step0fi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->stepkfi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->backupfi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->step0j, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->stepkj, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->backupj, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->meritlagmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummylagmult, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fscales, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->meritfunctionhistory, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->maxlaghistory, 0, DT_REAL, _state, make_automatic);
    _minslpsubsolver_init(&p->subsolver, _state, make_automatic);
    _minslptmpmerit_init(&p->tmpmerit, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
}


void _minslpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minslpstate       *dst = (minslpstate*)_dst;
    const minslpstate *src = (const minslpstate*)_src;
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
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    dst->f = src->f;
    dst->needfij = src->needfij;
    dst->xupdated = src->xupdated;
    _minslpphase13state_init_copy(&dst->state13, &src->state13, _state, make_automatic);
    _minslpphase2state_init_copy(&dst->state2, &src->state2, _state, make_automatic);
    dst->trustrad = src->trustrad;
    dst->bigc = src->bigc;
    dst->lpfailurecnt = src->lpfailurecnt;
    dst->fstagnationcnt = src->fstagnationcnt;
    ae_vector_init_copy(&dst->step0x, &src->step0x, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkx, &src->stepkx, _state, make_automatic);
    ae_vector_init_copy(&dst->backupx, &src->backupx, _state, make_automatic);
    ae_vector_init_copy(&dst->step0fi, &src->step0fi, _state, make_automatic);
    ae_vector_init_copy(&dst->stepkfi, &src->stepkfi, _state, make_automatic);
    ae_vector_init_copy(&dst->backupfi, &src->backupfi, _state, make_automatic);
    ae_matrix_init_copy(&dst->step0j, &src->step0j, _state, make_automatic);
    ae_matrix_init_copy(&dst->stepkj, &src->stepkj, _state, make_automatic);
    ae_matrix_init_copy(&dst->backupj, &src->backupj, _state, make_automatic);
    ae_vector_init_copy(&dst->meritlagmult, &src->meritlagmult, _state, make_automatic);
    ae_vector_init_copy(&dst->dummylagmult, &src->dummylagmult, _state, make_automatic);
    ae_vector_init_copy(&dst->fscales, &src->fscales, _state, make_automatic);
    ae_vector_init_copy(&dst->meritfunctionhistory, &src->meritfunctionhistory, _state, make_automatic);
    ae_vector_init_copy(&dst->maxlaghistory, &src->maxlaghistory, _state, make_automatic);
    dst->historylen = src->historylen;
    _minslpsubsolver_init_copy(&dst->subsolver, &src->subsolver, _state, make_automatic);
    _minslptmpmerit_init_copy(&dst->tmpmerit, &src->tmpmerit, _state, make_automatic);
    dst->repsimplexiterations = src->repsimplexiterations;
    dst->repsimplexiterations1 = src->repsimplexiterations1;
    dst->repsimplexiterations2 = src->repsimplexiterations2;
    dst->repsimplexiterations3 = src->repsimplexiterations3;
    dst->repinneriterationscount = src->repinneriterationscount;
    dst->repouteriterationscount = src->repouteriterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repbcerr = src->repbcerr;
    dst->repbcidx = src->repbcidx;
    dst->replcerr = src->replcerr;
    dst->replcidx = src->replcidx;
    dst->repnlcerr = src->repnlcerr;
    dst->repnlcidx = src->repnlcidx;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
}


void _minslpstate_clear(void* _p)
{
    minslpstate *p = (minslpstate*)_p;
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
    _minslpphase13state_clear(&p->state13);
    _minslpphase2state_clear(&p->state2);
    ae_vector_clear(&p->step0x);
    ae_vector_clear(&p->stepkx);
    ae_vector_clear(&p->backupx);
    ae_vector_clear(&p->step0fi);
    ae_vector_clear(&p->stepkfi);
    ae_vector_clear(&p->backupfi);
    ae_matrix_clear(&p->step0j);
    ae_matrix_clear(&p->stepkj);
    ae_matrix_clear(&p->backupj);
    ae_vector_clear(&p->meritlagmult);
    ae_vector_clear(&p->dummylagmult);
    ae_vector_clear(&p->fscales);
    ae_vector_clear(&p->meritfunctionhistory);
    ae_vector_clear(&p->maxlaghistory);
    _minslpsubsolver_clear(&p->subsolver);
    _minslptmpmerit_clear(&p->tmpmerit);
    _rcommstate_clear(&p->rstate);
}


void _minslpstate_destroy(void* _p)
{
    minslpstate *p = (minslpstate*)_p;
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
    _minslpphase13state_destroy(&p->state13);
    _minslpphase2state_destroy(&p->state2);
    ae_vector_destroy(&p->step0x);
    ae_vector_destroy(&p->stepkx);
    ae_vector_destroy(&p->backupx);
    ae_vector_destroy(&p->step0fi);
    ae_vector_destroy(&p->stepkfi);
    ae_vector_destroy(&p->backupfi);
    ae_matrix_destroy(&p->step0j);
    ae_matrix_destroy(&p->stepkj);
    ae_matrix_destroy(&p->backupj);
    ae_vector_destroy(&p->meritlagmult);
    ae_vector_destroy(&p->dummylagmult);
    ae_vector_destroy(&p->fscales);
    ae_vector_destroy(&p->meritfunctionhistory);
    ae_vector_destroy(&p->maxlaghistory);
    _minslpsubsolver_destroy(&p->subsolver);
    _minslptmpmerit_destroy(&p->tmpmerit);
    _rcommstate_destroy(&p->rstate);
}


/*$ End $*/
