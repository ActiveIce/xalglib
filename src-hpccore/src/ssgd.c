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
#include "ssgd.h"


/*$ Declarations $*/
static double ssgd_secanttoblurratio = 1.0;
static double ssgd_defaultpenalty = 100;
static double ssgd_sqjmomentum = 0.05;
static void ssgd_clearrequestfields(ssgdstate* state, ae_state *_state);
static double ssgd_penalizedtarget(ssgdstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     ae_state *_state);
static double ssgd_lcpenalty(ssgdstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
static double ssgd_scalednlcpenalty(ssgdstate* state,
     /* Real    */ const ae_vector* _fi,
     ae_int_t idx,
     double fscale,
     ae_state *_state);
static double ssgd_lcviolation(ssgdstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);


/*$ Body $*/


void ssgdinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t cntnlc,
     double rad0,
     double rad1,
     ae_int_t outerits,
     double rate0,
     double rate1,
     double momentum,
     ae_int_t maxits,
     double rho,
     ssgdstate* state,
     ae_state *_state)
{
    ae_int_t i;


    
    /*
     * Integrity checks
     */
    ae_assert(ae_isfinite(rate0, _state), "SSGD: Rate0 is not a finite number", _state);
    ae_assert(ae_isfinite(rate1, _state), "SSGD: Rate1 is not a finite number", _state);
    ae_assert(ae_isfinite(momentum, _state), "SSGD: Momentum is not a finite number", _state);
    ae_assert(ae_isfinite(rad0, _state), "SSGD: Rad0 is not a finite number", _state);
    ae_assert(ae_isfinite(rad1, _state), "SSGD: Rad1 is not a finite number", _state);
    ae_assert(ae_isfinite(rho, _state), "SSGD: Rho is not a finite number", _state);
    ae_assert(ae_fp_greater(rate0,(double)(0)), "SSGD: Rate0<=0", _state);
    ae_assert(ae_fp_greater(rate1,(double)(0)), "SSGD: Rate1<=0", _state);
    ae_assert(ae_fp_greater_eq(momentum,(double)(0)), "SSGD: Momentum<0", _state);
    ae_assert(ae_fp_less(momentum,(double)(1)), "SSGD: Momentum>=1", _state);
    ae_assert(ae_fp_greater(rad0,(double)(0)), "SSGD: Rad0<=0", _state);
    ae_assert(ae_fp_greater(rad1,(double)(0)), "SSGD: Rad1<=0", _state);
    ae_assert(ae_fp_greater_eq(rate0,rate1), "SSGD: Rate0<Rate1", _state);
    ae_assert(ae_fp_greater_eq(rad0,rad1), "SSGD: Rad0<Rad1", _state);
    ae_assert(maxits>0, "SSGD: MaxIts<=0", _state);
    ae_assert(outerits>0, "SSGD: OuterIts<=0", _state);
    ae_assert(ae_fp_greater_eq(rho,(double)(0)), "SSGD: Rho<0", _state);
    
    /*
     * Initialization
     */
    state->n = n;
    state->cntlc = cntlc;
    state->cntnlc = cntnlc;
    state->rate0 = rate0;
    state->rate1 = rate1;
    state->blur0 = rad0;
    state->blur1 = rad1;
    state->outerits = outerits;
    state->maxits = maxits;
    state->rho = coalesce(rho, ssgd_defaultpenalty, _state);
    
    /*
     * Prepare RCOMM state
     */
    ae_vector_set_length(&state->rstate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstate.ba, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 7+1, _state);
    state->rstate.stage = -1;
    state->needfi = ae_false;
    state->xupdated = ae_false;
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->fi, 1+cntnlc, _state);
    
    /*
     * Allocate memory.
     */
    rallocv(n, &state->x0, _state);
    rallocv(n, &state->s, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    rsetallocv(n, -ae_maxrealnumber, &state->finitebndl, _state);
    rsetallocv(n, ae_maxrealnumber, &state->finitebndu, _state);
    
    /*
     * Prepare scaled problem
     */
    for(i=0; i<=n-1; i++)
    {
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state->finitebndl.ptr.p_double[i] = bndl->ptr.p_double[i]/s->ptr.p_double[i];
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state->finitebndu.ptr.p_double[i] = bndu->ptr.p_double[i]/s->ptr.p_double[i];
        }
        if( state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i] )
        {
            ae_assert(ae_fp_less_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]), "SSGD: integrity check failed, box constraints are inconsistent", _state);
        }
        state->x0.ptr.p_double[i] = x0->ptr.p_double[i]/s->ptr.p_double[i];
        state->s.ptr.p_double[i] = s->ptr.p_double[i];
    }
    if( cntlc>0 )
    {
        rsetallocv(n, 0.0, &state->tmpzero, _state);
        rcopyallocm(cntlc, n, a, &state->densea, _state);
        rcopyallocv(cntlc, al, &state->al, _state);
        rcopyallocv(cntlc, au, &state->au, _state);
        scaleshiftmixedlcinplace(s, &state->tmpzero, n, &state->dummysparsea, 0, &state->densea, cntlc, &state->al, &state->au, _state);
        normalizedenselcinplace(&state->densea, cntlc, &state->al, &state->au, n, ae_true, &state->ascales, ae_true, _state);
        ballocv(cntlc, &state->hasal, _state);
        ballocv(cntlc, &state->hasau, _state);
        for(i=0; i<=cntlc-1; i++)
        {
            state->hasal.ptr.p_bool[i] = ae_isfinite(state->al.ptr.p_double[i], _state);
            state->hasau.ptr.p_bool[i] = ae_isfinite(state->au.ptr.p_double[i], _state);
        }
    }
    if( cntnlc>0 )
    {
        rcopyallocv(cntnlc, nl, &state->rawnl, _state);
        rcopyallocv(cntnlc, nu, &state->rawnu, _state);
        ballocv(cntnlc, &state->hasnl, _state);
        ballocv(cntnlc, &state->hasnu, _state);
        for(i=0; i<=cntnlc-1; i++)
        {
            state->hasnl.ptr.p_bool[i] = ae_isfinite(state->rawnl.ptr.p_double[i], _state);
            state->hasnu.ptr.p_bool[i] = ae_isfinite(state->rawnu.ptr.p_double[i], _state);
        }
    }
}


/*************************************************************************
This function performs actual processing for SSGD  algorithm.  It  expects
that the caller redirects its reverse communication  requests  NeedFiJ  or
XUpdated to the external user,  possibly  wrapping  them  in  a  numerical
differentiation code.

  -- ALGLIB --
     Copyright 26.07.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool ssgditeration(ssgdstate* state,
     ae_bool userterminationneeded,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_int_t outeridx;
    ae_int_t iteridx;
    ae_int_t i;
    ae_int_t j;
    double f0;
    double fbest;
    double fleft;
    double fright;
    double avgsqg;
    double finalscale;
    double t;
    double v;
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
        cntlc = state->rstate.ia.ptr.p_int[1];
        cntnlc = state->rstate.ia.ptr.p_int[2];
        outeridx = state->rstate.ia.ptr.p_int[3];
        iteridx = state->rstate.ia.ptr.p_int[4];
        i = state->rstate.ia.ptr.p_int[5];
        j = state->rstate.ia.ptr.p_int[6];
        dotrace = state->rstate.ba.ptr.p_bool[0];
        dodetailedtrace = state->rstate.ba.ptr.p_bool[1];
        f0 = state->rstate.ra.ptr.p_double[0];
        fbest = state->rstate.ra.ptr.p_double[1];
        fleft = state->rstate.ra.ptr.p_double[2];
        fright = state->rstate.ra.ptr.p_double[3];
        avgsqg = state->rstate.ra.ptr.p_double[4];
        finalscale = state->rstate.ra.ptr.p_double[5];
        t = state->rstate.ra.ptr.p_double[6];
        v = state->rstate.ra.ptr.p_double[7];
    }
    else
    {
        n = 359;
        cntlc = -58;
        cntnlc = -919;
        outeridx = -909;
        iteridx = 81;
        i = 255;
        j = 74;
        dotrace = ae_false;
        dodetailedtrace = ae_true;
        f0 = 205.0;
        fbest = -838.0;
        fleft = 939.0;
        fright = -526.0;
        avgsqg = 763.0;
        finalscale = -541.0;
        t = -698.0;
        v = -900.0;
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
    
    /*
     * Routine body
     */
    n = state->n;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    dotrace = ae_is_trace_enabled("SSGD");
    dodetailedtrace = dotrace&&ae_is_trace_enabled("SSGD.DETAILED");
    
    /*
     * Prepare rcomm interface
     */
    ssgd_clearrequestfields(state, _state);
    
    /*
     * Initialize algorithm state.
     */
    hqrndseed(3366544, 86335763, &state->rs, _state);
    rallocv(n, &state->xoffs0, _state);
    rallocv(n, &state->xoffs1, _state);
    rallocv(n, &state->gcur, _state);
    rcopyallocv(n, &state->x0, &state->xcur, _state);
    rmergemaxv(n, &state->finitebndl, &state->xcur, _state);
    rmergeminv(n, &state->finitebndu, &state->xcur, _state);
    rsetallocv(n, 0.0, &state->dcur, _state);
    state->repiterationscount = 0;
    state->repnfev = 0;
    avgsqg = ae_machineepsilon;
    rsetallocv(1+cntnlc, ae_machineepsilon, &state->avgsqj, _state);
    rsetallocv(1+cntnlc, 1.0, &state->fscales, _state);
    finalscale = 1.0;
    rcopyallocv(n, &state->xcur, &state->xbest, _state);
    rcopyv(n, &state->xbest, &state->x, _state);
    state->needfi = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfi = ae_false;
    rcopyallocv(1+cntnlc, &state->fi, &state->fibest, _state);
    rallocv(n, &state->xleft, _state);
    rallocv(n, &state->xright, _state);
    rallocv(1+cntnlc, &state->fileft, _state);
    rallocv(1+cntnlc, &state->firight, _state);
    
    /*
     * Trace output (if needed)
     */
    if( dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  Smoothed SGD SOLVER STARTED                                                                   //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("N             = %6d\n",
            (int)(n));
        ae_trace("cntLC         = %6d\n",
            (int)(cntlc));
        ae_trace("cntNLC        = %6d\n",
            (int)(cntnlc));
    }
    
    /*
     * Perform SGD iterations
     */
    outeridx = 0;
lbl_4:
    if( outeridx>state->outerits-1 )
    {
        goto lbl_6;
    }
    
    /*
     * Choose initial point for an outer iteration as the best point found so far
     */
    rcopyv(n, &state->xbest, &state->xcur, _state);
    
    /*
     * Inner iteration: perform MaxIts SGD steps
     */
    iteridx = 0;
lbl_7:
    if( iteridx>state->maxits-1 )
    {
        goto lbl_9;
    }
    
    /*
     * Determine iteration parameters
     */
    t = (double)iteridx/(double)ae_maxint(state->maxits-1, 1, _state);
    state->currate = state->rate0*ae_exp(ae_log(state->rate1/state->rate0, _state)*ae_maxreal((double)2*t-(double)1, (double)(0), _state), _state);
    state->curblur = state->blur0*ae_exp(ae_log(state->blur1/state->blur0, _state)*(double)outeridx/(double)ae_maxint(state->outerits-1, 1, _state), _state);
    state->zcursecantstep = ssgd_secanttoblurratio*state->curblur;
    if( dotrace )
    {
        ae_trace("\n=== INNER ITERATION %5d, OUTER ITERATION %5d ===================================================\n",
            (int)(iteridx),
            (int)(outeridx));
        ae_trace("learn.Rate    = %0.3e\n",
            (double)(state->currate));
        ae_trace("smooth.Rad    = %0.3e\n",
            (double)(state->curblur));
        ae_trace("secantStep    = %0.3e\n",
            (double)(state->zcursecantstep));
    }
    
    /*
     * Compute target value at the current point, update best value so far
     */
    rcopyv(n, &state->xcur, &state->x, _state);
    state->needfi = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfi = ae_false;
    f0 = ssgd_penalizedtarget(state, &state->x, &state->fi, _state);
    fbest = ssgd_penalizedtarget(state, &state->xbest, &state->fibest, _state);
    if( dodetailedtrace )
    {
        ae_trace("> printing current location and status:\n");
        ae_trace("X (raw)       = ");
        tracevectorunscaledunshiftedautoprec(&state->xcur, n, &state->s, ae_true, &state->s, ae_false, _state);
        ae_trace("\n");
        ae_trace("X (scaled)    = ");
        tracevectorautoprec(&state->xcur, 0, n, _state);
        ae_trace("\n");
    }
    if( ae_fp_less(f0,fbest) )
    {
        rcopyv(n, &state->xcur, &state->xbest, _state);
        rcopyallocv(1+cntnlc, &state->fi, &state->fibest, _state);
    }
    if( dotrace )
    {
        ae_trace("\n--- current target ---------------------------------------------------------------------------------\n");
        ae_trace("F             = %0.12e\n",
            (double)(state->fi.ptr.p_double[0]));
        ae_trace("F-penalized   = %0.12e\n",
            (double)(f0));
        ae_trace("lin.violation = %0.3e\n",
            (double)(ssgd_lcviolation(state, &state->xcur, _state)));
        ae_trace("\n--- best point so far ------------------------------------------------------------------------------\n");
        ae_trace("F             = %0.12e\n",
            (double)(state->fibest.ptr.p_double[0]));
        ae_trace("F-penalized   = %0.12e\n",
            (double)(fbest));
        ae_trace("lin.violation = %0.3e\n",
            (double)(ssgd_lcviolation(state, &state->xbest, _state)));
    }
    
    /*
     * Compute gradients (target, linear penalties, nonlinear penalties) using secant update
     */
    for(i=0; i<=n-1; i++)
    {
        state->xoffs0.ptr.p_double[i] = hqrnduniformr(&state->rs, _state)-0.5;
        state->xoffs1.ptr.p_double[i] = state->xoffs0.ptr.p_double[i];
    }
    rsetallocv(1+cntnlc, 0.0, &state->sqj, _state);
    i = 0;
lbl_10:
    if( i>n-1 )
    {
        goto lbl_12;
    }
    
    /*
     * Compute f(x-SecantStep) and f(x+SecantStep)
     */
    rcopyv(n, &state->xcur, &state->x, _state);
    raddv(n, state->curblur, &state->xoffs0, &state->x, _state);
    state->x.ptr.p_double[i] = state->x.ptr.p_double[i]-state->zcursecantstep;
    rmergemaxv(n, &state->finitebndl, &state->x, _state);
    rmergeminv(n, &state->finitebndu, &state->x, _state);
    state->needfi = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfi = ae_false;
    rcopyv(n, &state->x, &state->xleft, _state);
    rcopyv(1+cntnlc, &state->fi, &state->fileft, _state);
    rcopyv(n, &state->xcur, &state->x, _state);
    raddv(n, state->curblur, &state->xoffs1, &state->x, _state);
    state->x.ptr.p_double[i] = state->x.ptr.p_double[i]+state->zcursecantstep;
    rmergemaxv(n, &state->finitebndl, &state->x, _state);
    rmergeminv(n, &state->finitebndu, &state->x, _state);
    state->needfi = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needfi = ae_false;
    rcopyv(n, &state->x, &state->xright, _state);
    rcopyv(1+cntnlc, &state->fi, &state->firight, _state);
    
    /*
     * Compute squared norms of target/constraint gradients
     */
    for(j=0; j<=cntnlc; j++)
    {
        v = (state->firight.ptr.p_double[j]-state->fileft.ptr.p_double[j])/((double)2*state->zcursecantstep);
        state->sqj.ptr.p_double[j] = state->sqj.ptr.p_double[j]+v*v;
    }
    
    /*
     * Compute gradient using current target/constraint scales
     */
    state->gcur.ptr.p_double[i] = (state->firight.ptr.p_double[0]-state->fileft.ptr.p_double[0])/((double)2*state->fscales.ptr.p_double[0]*state->zcursecantstep);
    for(j=0; j<=cntnlc-1; j++)
    {
        state->gcur.ptr.p_double[i] = state->gcur.ptr.p_double[i]+(ssgd_scalednlcpenalty(state, &state->firight, j, state->fscales.ptr.p_double[1+j], _state)-ssgd_scalednlcpenalty(state, &state->fileft, j, state->fscales.ptr.p_double[1+j], _state))/((double)2*state->zcursecantstep);
    }
    state->gcur.ptr.p_double[i] = state->gcur.ptr.p_double[i]+(ssgd_lcpenalty(state, &state->xright, _state)-ssgd_lcpenalty(state, &state->xleft, _state))/((double)2*state->zcursecantstep);
    i = i+1;
    goto lbl_10;
lbl_12:
    
    /*
     * Recompute scales using data collected during gradient calculation
     */
    for(i=0; i<=cntnlc; i++)
    {
        state->avgsqj.ptr.p_double[i] = state->avgsqj.ptr.p_double[i]*((double)1-ssgd_sqjmomentum)+ssgd_sqjmomentum*ae_sqrt(state->sqj.ptr.p_double[j], _state);
        state->fscales.ptr.p_double[i] = ae_maxreal(state->avgsqj.ptr.p_double[i], 1.0, _state);
    }
    if( dotrace )
    {
        ae_trace("scale(F)      = %0.3e\n",
            (double)(state->fscales.ptr.p_double[0]));
    }
    v = ae_sqrt(rdotv2(n, &state->gcur, _state), _state);
    avgsqg = avgsqg*((double)1-ssgd_sqjmomentum)+ssgd_sqjmomentum*v;
    finalscale = rmax3(avgsqg, v, 1.0, _state);
    rmulv(n, (double)1/finalscale, &state->gcur, _state);
    
    /*
     * Perform step
     */
    rmulv(n, state->momentum, &state->dcur, _state);
    raddv(n, (double)1-state->momentum, &state->gcur, &state->dcur, _state);
    raddv(n, -state->currate, &state->dcur, &state->xcur, _state);
    rmergemaxv(n, &state->finitebndl, &state->xcur, _state);
    rmergeminv(n, &state->finitebndu, &state->xcur, _state);
    state->repiterationscount = state->repiterationscount+1;
    iteridx = iteridx+1;
    goto lbl_7;
lbl_9:
    outeridx = outeridx+1;
    goto lbl_4;
lbl_6:
    if( dotrace )
    {
        ae_trace("> reached iterations limit, stopping\n");
    }
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = cntlc;
    state->rstate.ia.ptr.p_int[2] = cntnlc;
    state->rstate.ia.ptr.p_int[3] = outeridx;
    state->rstate.ia.ptr.p_int[4] = iteridx;
    state->rstate.ia.ptr.p_int[5] = i;
    state->rstate.ia.ptr.p_int[6] = j;
    state->rstate.ba.ptr.p_bool[0] = dotrace;
    state->rstate.ba.ptr.p_bool[1] = dodetailedtrace;
    state->rstate.ra.ptr.p_double[0] = f0;
    state->rstate.ra.ptr.p_double[1] = fbest;
    state->rstate.ra.ptr.p_double[2] = fleft;
    state->rstate.ra.ptr.p_double[3] = fright;
    state->rstate.ra.ptr.p_double[4] = avgsqg;
    state->rstate.ra.ptr.p_double[5] = finalscale;
    state->rstate.ra.ptr.p_double[6] = t;
    state->rstate.ra.ptr.p_double[7] = v;
    return result;
}


/*************************************************************************
Clears request fileds (to be sure that we didn't forget to clear something)
*************************************************************************/
static void ssgd_clearrequestfields(ssgdstate* state, ae_state *_state)
{


    state->needfi = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Compute penalized target
*************************************************************************/
static double ssgd_penalizedtarget(ssgdstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     ae_state *_state)
{
    ae_int_t i;
    double result;


    result = fi->ptr.p_double[0]/state->fscales.ptr.p_double[0];
    for(i=0; i<=state->cntnlc-1; i++)
    {
        result = result+ssgd_scalednlcpenalty(state, fi, i, state->fscales.ptr.p_double[1+i], _state);
    }
    result = result+ssgd_lcpenalty(state, x, _state);
    return result;
}


/*************************************************************************
Compute penalty for linear constraints
*************************************************************************/
static double ssgd_lcpenalty(ssgdstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;


    result = 0.0;
    for(i=0; i<=state->cntlc-1; i++)
    {
        v = rdotvr(state->n, x, &state->densea, i, _state);
        if( state->hasal.ptr.p_bool[i]&&v<state->al.ptr.p_double[i] )
        {
            result = result+(double)0*(state->al.ptr.p_double[i]-v)+state->rho*(state->al.ptr.p_double[i]-v)*(state->al.ptr.p_double[i]-v);
        }
        if( state->hasau.ptr.p_bool[i]&&v>state->au.ptr.p_double[i] )
        {
            result = result+(double)0*(v-state->au.ptr.p_double[i])+state->rho*(v-state->au.ptr.p_double[i])*(v-state->au.ptr.p_double[i]);
        }
    }
    return result;
}


/*************************************************************************
Compute penalty for a single nonlinear constraint
*************************************************************************/
static double ssgd_scalednlcpenalty(ssgdstate* state,
     /* Real    */ const ae_vector* _fi,
     ae_int_t idx,
     double fscale,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector fi;
    double v;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&fi, 0, sizeof(fi));
    ae_vector_init_copy(&fi, _fi, _state, ae_true);

    result = 0.0;
    if( state->hasnl.ptr.p_bool[idx]&&fi.ptr.p_double[1+idx]<state->rawnl.ptr.p_double[idx] )
    {
        v = (state->rawnl.ptr.p_double[idx]-fi.ptr.p_double[1+idx])/fscale;
        result = result+(double)0*v+state->rho*v*v;
    }
    if( state->hasnu.ptr.p_bool[idx]&&fi.ptr.p_double[1+idx]>state->rawnu.ptr.p_double[idx] )
    {
        v = (fi.ptr.p_double[1+idx]-state->rawnu.ptr.p_double[idx])/fscale;
        result = result+(double)0*v+state->rho*v*v;
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Compute violation of linear constraints
*************************************************************************/
static double ssgd_lcviolation(ssgdstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    double result;


    result = 0.0;
    for(i=0; i<=state->cntlc-1; i++)
    {
        v = rdotvr(state->n, x, &state->densea, i, _state);
        if( state->hasal.ptr.p_bool[i] )
        {
            result = ae_maxreal(result, state->al.ptr.p_double[i]-v, _state);
        }
        if( state->hasau.ptr.p_bool[i] )
        {
            result = ae_maxreal(result, v-state->au.ptr.p_double[i], _state);
        }
    }
    return result;
}


void _ssgdstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    ssgdstate *p = (ssgdstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->finitebndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->finitebndu, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->densea, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->al, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->au, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasal, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasau, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->rawnl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawnu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasnl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasnu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->ascales, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->avgsqj, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fscales, 0, DT_REAL, _state, make_automatic);
    _hqrndstate_init(&p->rs, _state, make_automatic);
    ae_vector_init(&p->xbest, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fibest, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xcur, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gcur, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dcur, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_vector_init(&p->sqj, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xleft, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xright, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fileft, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->firight, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xoffs0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xoffs1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpzero, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->dummysparsea, _state, make_automatic);
}


void _ssgdstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    ssgdstate       *dst = (ssgdstate*)_dst;
    const ssgdstate *src = (const ssgdstate*)_src;
    dst->n = src->n;
    dst->cntlc = src->cntlc;
    dst->cntnlc = src->cntnlc;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->finitebndl, &src->finitebndl, _state, make_automatic);
    ae_vector_init_copy(&dst->finitebndu, &src->finitebndu, _state, make_automatic);
    ae_matrix_init_copy(&dst->densea, &src->densea, _state, make_automatic);
    ae_vector_init_copy(&dst->al, &src->al, _state, make_automatic);
    ae_vector_init_copy(&dst->au, &src->au, _state, make_automatic);
    ae_vector_init_copy(&dst->hasal, &src->hasal, _state, make_automatic);
    ae_vector_init_copy(&dst->hasau, &src->hasau, _state, make_automatic);
    ae_vector_init_copy(&dst->rawnl, &src->rawnl, _state, make_automatic);
    ae_vector_init_copy(&dst->rawnu, &src->rawnu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasnl, &src->hasnl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasnu, &src->hasnu, _state, make_automatic);
    ae_vector_init_copy(&dst->ascales, &src->ascales, _state, make_automatic);
    dst->maxits = src->maxits;
    dst->rate0 = src->rate0;
    dst->rate1 = src->rate1;
    dst->momentum = src->momentum;
    dst->blur0 = src->blur0;
    dst->blur1 = src->blur1;
    dst->outerits = src->outerits;
    dst->rho = src->rho;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    dst->needfi = src->needfi;
    dst->xupdated = src->xupdated;
    ae_vector_init_copy(&dst->avgsqj, &src->avgsqj, _state, make_automatic);
    ae_vector_init_copy(&dst->fscales, &src->fscales, _state, make_automatic);
    _hqrndstate_init_copy(&dst->rs, &src->rs, _state, make_automatic);
    ae_vector_init_copy(&dst->xbest, &src->xbest, _state, make_automatic);
    ae_vector_init_copy(&dst->fibest, &src->fibest, _state, make_automatic);
    ae_vector_init_copy(&dst->xcur, &src->xcur, _state, make_automatic);
    ae_vector_init_copy(&dst->gcur, &src->gcur, _state, make_automatic);
    ae_vector_init_copy(&dst->dcur, &src->dcur, _state, make_automatic);
    dst->currate = src->currate;
    dst->curblur = src->curblur;
    dst->zcursecantstep = src->zcursecantstep;
    dst->repiterationscount = src->repiterationscount;
    dst->repnfev = src->repnfev;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    ae_vector_init_copy(&dst->sqj, &src->sqj, _state, make_automatic);
    ae_vector_init_copy(&dst->xleft, &src->xleft, _state, make_automatic);
    ae_vector_init_copy(&dst->xright, &src->xright, _state, make_automatic);
    ae_vector_init_copy(&dst->fileft, &src->fileft, _state, make_automatic);
    ae_vector_init_copy(&dst->firight, &src->firight, _state, make_automatic);
    ae_vector_init_copy(&dst->xoffs0, &src->xoffs0, _state, make_automatic);
    ae_vector_init_copy(&dst->xoffs1, &src->xoffs1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpzero, &src->tmpzero, _state, make_automatic);
    _sparsematrix_init_copy(&dst->dummysparsea, &src->dummysparsea, _state, make_automatic);
}


void _ssgdstate_clear(void* _p)
{
    ssgdstate *p = (ssgdstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->finitebndl);
    ae_vector_clear(&p->finitebndu);
    ae_matrix_clear(&p->densea);
    ae_vector_clear(&p->al);
    ae_vector_clear(&p->au);
    ae_vector_clear(&p->hasal);
    ae_vector_clear(&p->hasau);
    ae_vector_clear(&p->rawnl);
    ae_vector_clear(&p->rawnu);
    ae_vector_clear(&p->hasnl);
    ae_vector_clear(&p->hasnu);
    ae_vector_clear(&p->ascales);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_vector_clear(&p->avgsqj);
    ae_vector_clear(&p->fscales);
    _hqrndstate_clear(&p->rs);
    ae_vector_clear(&p->xbest);
    ae_vector_clear(&p->fibest);
    ae_vector_clear(&p->xcur);
    ae_vector_clear(&p->gcur);
    ae_vector_clear(&p->dcur);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->sqj);
    ae_vector_clear(&p->xleft);
    ae_vector_clear(&p->xright);
    ae_vector_clear(&p->fileft);
    ae_vector_clear(&p->firight);
    ae_vector_clear(&p->xoffs0);
    ae_vector_clear(&p->xoffs1);
    ae_vector_clear(&p->tmpzero);
    _sparsematrix_clear(&p->dummysparsea);
}


void _ssgdstate_destroy(void* _p)
{
    ssgdstate *p = (ssgdstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->finitebndl);
    ae_vector_destroy(&p->finitebndu);
    ae_matrix_destroy(&p->densea);
    ae_vector_destroy(&p->al);
    ae_vector_destroy(&p->au);
    ae_vector_destroy(&p->hasal);
    ae_vector_destroy(&p->hasau);
    ae_vector_destroy(&p->rawnl);
    ae_vector_destroy(&p->rawnu);
    ae_vector_destroy(&p->hasnl);
    ae_vector_destroy(&p->hasnu);
    ae_vector_destroy(&p->ascales);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_vector_destroy(&p->avgsqj);
    ae_vector_destroy(&p->fscales);
    _hqrndstate_destroy(&p->rs);
    ae_vector_destroy(&p->xbest);
    ae_vector_destroy(&p->fibest);
    ae_vector_destroy(&p->xcur);
    ae_vector_destroy(&p->gcur);
    ae_vector_destroy(&p->dcur);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->sqj);
    ae_vector_destroy(&p->xleft);
    ae_vector_destroy(&p->xright);
    ae_vector_destroy(&p->fileft);
    ae_vector_destroy(&p->firight);
    ae_vector_destroy(&p->xoffs0);
    ae_vector_destroy(&p->xoffs1);
    ae_vector_destroy(&p->tmpzero);
    _sparsematrix_destroy(&p->dummysparsea);
}


/*$ End $*/
