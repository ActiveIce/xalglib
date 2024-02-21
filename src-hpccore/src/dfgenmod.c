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
#include "dfgenmod.h"


/*$ Declarations $*/
static double dfgenmod_gammasafety = 0.5;
static double dfgenmod_omegasafety = 0.1;
static double dfgenmod_alpha1 = 0.1;
static double dfgenmod_alpha2 = 0.5;
static double dfgenmod_eta1 = 0.1;
static double dfgenmod_eta2 = 0.7;
static double dfgenmod_gammabad = 0.1;
static double dfgenmod_gammadec = 0.5;
static double dfgenmod_gammainc = 2.0;
static double dfgenmod_gammainc2 = 4.0;
static double dfgenmod_twopsstep = 0.5;
static double dfgenmod_twopssteplo = 0.15;
static double dfgenmod_twopsstephi = 1.50;
static double dfgenmod_lsaimprovementlimit = 1;
static double dfgenmod_pointunacceptablyfar = 50.0;
static double dfgenmod_pointtoofar = 2.0;
static double dfgenmod_pointtooclose = 0.02;
static ae_int_t dfgenmod_minhistory = 5;
static ae_int_t dfgenmod_maxhistory = 30;
static void dfgenmod_inithistory(dfgmstate* state, ae_state *_state);
static void dfgenmod_updatehistory(dfgmstate* state, ae_state *_state);
static void dfgenmod_updatetrustregion(dfgmstate* state,
     double trustradfactor,
     ae_state *_state);
static double dfgenmod_normrelativetotrustregion(const dfgmstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
static void dfgenmod_preparetrialrequest(dfgmstate* state,
     double* predf,
     double* predh,
     ae_state *_state);
static void dfgenmod_processtrialrequest(dfgmstate* state,
     double* f,
     double* h,
     ae_state *_state);
static ae_bool dfgenmod_testacceptancebyfilter(const dfgmstate* state,
     double fk,
     double hk,
     double fp,
     double hp,
     double fn,
     double hn,
     ae_state *_state);
static void dfgenmod_sendtofilter(const dfgmstate* state,
     double fn,
     double hn,
     ae_state *_state);
static void dfgenmod_sendstepdenialtomodel(dfgmstate* state,
     ae_state *_state);
static ae_bool dfgenmod_deltakatminimum(const dfgmstate* state,
     ae_state *_state);
static ae_bool dfgenmod_restartautodetected(const dfgmstate* state,
     ae_state *_state);
static ae_bool dfgenmod_workingsetneedsimprovement(dfgmstate* state,
     ae_state *_state);
static ae_bool dfgenmod_updateworkingsetandmodel(dfgmstate* state,
     /* Real    */ const ae_vector* xk,
     /* Real    */ const ae_vector* xn,
     /* Real    */ const ae_vector* fnvec,
     double fn,
     double hn,
     ae_bool successfulstep,
     ae_state *_state);
static ae_bool dfgenmod_improveworkingsetgeometry(dfgmstate* state,
     ae_state *_state);
static double dfgenmod_maxabslag(/* Real    */ const ae_vector* xk,
     ae_int_t n,
     /* Real    */ const ae_vector* trustregion,
     /* Real    */ const ae_vector* finitebndl,
     /* Real    */ const ae_vector* finitebndu,
     /* Real    */ const ae_vector* c,
     double c0,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* tmpx,
     ae_bool needx,
     ae_state *_state);
static void dfgenmod_mostandleastdistant(/* Real    */ const ae_vector* invtrustregion,
     ae_int_t n,
     /* Real    */ const ae_matrix* wrkset,
     ae_int_t wrksetsize,
     double* mostdistant,
     ae_int_t* mostdistantidx,
     double* leastdistant,
     ae_int_t* leastdistantidx,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Initialize derivative-free general model-based solver (DFGM)

IMPORTANT: the solver does NOT support fixed variables. The caller has  to
           preprocess the problem in order to drop all fixed vars.
*************************************************************************/
void dfgminitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     ae_int_t m,
     ae_bool isls,
     ae_int_t modeltype,
     const nlpstoppingcriteria* criteria,
     ae_int_t nnoisyrestarts,
     double rad0,
     ae_int_t maxfev,
     dfgmstate* state,
     ae_state *_state)
{
    ae_int_t i;


    
    /*
     * General initialization
     */
    ae_assert(isls||m==1, "DFGM: M<>1 for a non-least-squares problem", _state);
    state->n = n;
    state->m = m;
    state->isleastsquares = isls;
    state->cntlc = 0;
    state->cntnlc = 0;
    state->rad0 = rad0;
    state->nnoisyrestarts = nnoisyrestarts;
    
    /*
     * Model type
     */
    ae_assert(modeltype==0||modeltype==1, "DFGM: unexpected model type, check 0316 failed", _state);
    ae_assert(modeltype!=0||isls, "DFGM: ModelType=0 is possible only with least squares problems", _state);
    ae_assert(modeltype!=1||isls, "DFGM: ModelType=1 is possible only with least squares problems", _state);
    state->modeltype = modeltype;
    
    /*
     * Stopping criteria
     */
    state->epsx = critgetepsxwithdefault(criteria, 0.000001, _state);
    state->maxits = critgetmaxits(criteria, _state);
    state->maxfev = maxfev;
    state->toosmalltrustrad = coalesce(0.001*state->epsx, ae_machineepsilon, _state);
    
    /*
     * Prepare scaled problem
     */
    rallocv(n, &state->x0, _state);
    rallocv(n, &state->s, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    rvectorsetlengthatleast(&state->scaledbndl, n, _state);
    rvectorsetlengthatleast(&state->scaledbndu, n, _state);
    rsetallocv(n, -1.0E50, &state->finitebndl, _state);
    rsetallocv(n, 1.0E50, &state->finitebndu, _state);
    for(i=0; i<=n-1; i++)
    {
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state->scaledbndl.ptr.p_double[i] = bndl->ptr.p_double[i]/s->ptr.p_double[i];
            state->finitebndl.ptr.p_double[i] = state->scaledbndl.ptr.p_double[i];
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state->scaledbndu.ptr.p_double[i] = bndu->ptr.p_double[i]/s->ptr.p_double[i];
            state->finitebndu.ptr.p_double[i] = state->scaledbndu.ptr.p_double[i];
        }
        if( state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i] )
        {
            ae_assert(ae_fp_less(bndl->ptr.p_double[i],bndu->ptr.p_double[i]), "DFGM: integrity check 6007 failed", _state);
        }
        state->x0.ptr.p_double[i] = x0->ptr.p_double[i]/s->ptr.p_double[i];
        state->s.ptr.p_double[i] = s->ptr.p_double[i];
    }
    enforceboundaryconstraints(&state->x0, &state->scaledbndl, &state->hasbndl, &state->scaledbndu, &state->hasbndu, n, 0, _state);
    rcopyallocv(n, &state->x0, &state->xk, _state);
    
    /*
     * Internal RNG
     */
    hqrndseed(117564, 983549, &state->rs, _state);
    
    /*
     * Report fields
     */
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repnfev = 0;
    
    /*
     * Trace status
     */
    ae_assert(modeltype==0||modeltype==1, "DFGM: unexpected model type, check 0317 failed", _state);
    state->dotrace = ae_false;
    if( modeltype==0 )
    {
        state->dotrace = ae_is_trace_enabled("2PS");
    }
    if( modeltype==1 )
    {
        state->dotrace = ae_is_trace_enabled("DFOLSA");
    }
    state->dotrace = state->dotrace||ae_is_trace_enabled("DFGM");
    
    /*
     * RComm
     */
    ae_vector_set_length(&state->rstate.ia, 4+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 7+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool dfgmiteration(dfgmstate* state,
     ae_bool userterminationneeded,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntnlc;
    double predf;
    double predh;
    double fnew;
    double hnew;
    double skrellen;
    double v;
    double v1;
    double vv;
    ae_bool accepted;
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
        i = state->rstate.ia.ptr.p_int[0];
        j = state->rstate.ia.ptr.p_int[1];
        n = state->rstate.ia.ptr.p_int[2];
        m = state->rstate.ia.ptr.p_int[3];
        cntnlc = state->rstate.ia.ptr.p_int[4];
        accepted = state->rstate.ba.ptr.p_bool[0];
        predf = state->rstate.ra.ptr.p_double[0];
        predh = state->rstate.ra.ptr.p_double[1];
        fnew = state->rstate.ra.ptr.p_double[2];
        hnew = state->rstate.ra.ptr.p_double[3];
        skrellen = state->rstate.ra.ptr.p_double[4];
        v = state->rstate.ra.ptr.p_double[5];
        v1 = state->rstate.ra.ptr.p_double[6];
        vv = state->rstate.ra.ptr.p_double[7];
    }
    else
    {
        i = 359;
        j = -58;
        n = -919;
        m = -909;
        cntnlc = 81;
        accepted = ae_true;
        predf = 74.0;
        predh = -788.0;
        fnew = 809.0;
        hnew = 205.0;
        skrellen = -838.0;
        v = 939.0;
        v1 = -526.0;
        vv = 763.0;
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
    if( state->rstate.stage==6 )
    {
        goto lbl_6;
    }
    
    /*
     * Routine body
     */
    n = state->n;
    m = state->m;
    cntnlc = state->cntnlc;
    if( state->dotrace )
    {
        ae_trace("> derivative-free model-based solver started\n");
        if( state->modeltype==0 )
        {
            ae_trace("algorithm     = 2PS\n");
        }
        if( state->modeltype==1 )
        {
            ae_trace("algorithm     = DFO-LSA\n");
        }
    }
    
    /*
     * Initialization phase
     *
     * NOTE: TrustRadFactor is initialized by zero prior to calling UpdateTrustRegion()
     *       because this function expects some initial value to be present here.
     */
    rallocv(n, &state->xp, _state);
    rallocv(m+cntnlc, &state->fpvec, _state);
    rallocv(n, &state->sk, _state);
    rallocv(n, &state->xn, _state);
    rallocv(m+cntnlc, &state->fnvec, _state);
    state->infinitiesencountered = ae_false;
    state->recoveredfrominfinities = ae_true;
    dfgenmod_inithistory(state, _state);
    state->trustradfactor = (double)(0);
    dfgenmod_updatetrustregion(state, coalesce(state->rad0, 0.1, _state), _state);
    state->trustradbnd = state->trustradfactor;
    state->wrksetsize = 0;
    
    /*
     * Start iterations
     */
lbl_7:
    if( ae_false )
    {
        goto lbl_8;
    }
    
    /*
     * The working set is empty. It may happen during either the very first iteration of the solver,
     * or after the restart. Generate new working set from scratch, compute FkVec, Fk, Hk.
     */
    if( state->wrksetsize!=0 )
    {
        goto lbl_9;
    }
    ae_vector_set_length(&state->rstateimprove.ia, 9+1, _state);
    ae_vector_set_length(&state->rstateimprove.ba, 0+1, _state);
    ae_vector_set_length(&state->rstateimprove.ra, 8+1, _state);
    state->rstateimprove.stage = -1;
lbl_11:
    if( !dfgenmod_improveworkingsetgeometry(state, _state) )
    {
        goto lbl_12;
    }
    if( state->requesttype==4 )
    {
        state->repnfev = state->repnfev+state->querysize;
    }
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    goto lbl_11;
lbl_12:
    ae_assert(state->repterminationtype<=0, "DFGM: integrity check 4524 failed", _state);
    if( state->repterminationtype<0 )
    {
        goto lbl_8;
    }
    ae_assert(state->wrksetsize>0, "DFGM: integrity check 0227 failed", _state);
    ae_assert(state->cntlc==0, "DFGM: integrity check 8744 failed", _state);
    ae_assert(state->cntnlc==0, "DFGM: integrity check 8745 failed", _state);
    ae_assert(state->isleastsquares, "DFGM: integrity check 8746 failed", _state);
    rallocv(m+cntnlc, &state->fkvec, _state);
    for(j=0; j<=m+cntnlc-1; j++)
    {
        state->fkvec.ptr.p_double[j] = state->wrkset.ptr.pp_double[state->wrksetsize-1][n+j];
    }
    state->fk = (double)(0);
    for(j=0; j<=m-1; j++)
    {
        state->fk = state->fk+0.5*state->wrkset.ptr.pp_double[state->wrksetsize-1][n+j]*state->wrkset.ptr.pp_double[state->wrksetsize-1][n+j];
    }
    state->hk = (double)(0);
lbl_9:
    
    /*
     * Update history of changes in the trust radius and the Jacobian
     */
    dfgenmod_updatehistory(state, _state);
    
    /*
     * Report current point
     */
    state->requesttype = -1;
    rcopyallocv(n, &state->xk, &state->reportx, _state);
    state->reportf = (double)2*state->fk;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    
    /*
     * Check iteration-count based stopping criteria
     */
    if( state->maxits>0&&state->repiterationscount>=state->maxits )
    {
        if( state->dotrace )
        {
            ae_trace("> stopping condition triggered: maximum number of iterations\n");
        }
        state->repterminationtype = 5;
        goto lbl_8;
    }
    if( ae_fp_less_eq(state->trustradbnd,ae_maxreal(state->epsx, state->toosmalltrustrad, _state)) )
    {
        
        /*
         * Trust radius decreased: either perform a restart or stop
         */
        if( state->restartstoperform>0 )
        {
            dfgenmod_updatetrustregion(state, coalesce(state->rad0, 0.1, _state), _state);
            state->trustradbnd = state->trustradfactor;
            state->wrksetsize = 0;
            state->historylen = 0;
            state->historynext = 0;
            state->restartstoperform = state->restartstoperform-1;
            if( state->dotrace )
            {
                ae_trace("> restart is performed due to a small trust region size (%0d restarts left)\n",
                    (int)(state->restartstoperform));
            }
            goto lbl_7;
        }
        if( ae_fp_less_eq(state->trustradbnd,state->epsx) )
        {
            if( state->dotrace )
            {
                ae_trace("> stopping condition triggered: step less than EpsX\n");
            }
            state->repterminationtype = 2;
        }
        else
        {
            if( state->dotrace )
            {
                ae_trace("> stopping condition triggered: step is too small, emergency termination\n");
            }
            state->repterminationtype = 7;
        }
        goto lbl_8;
    }
    if( userterminationneeded )
    {
        if( state->dotrace )
        {
            ae_trace("> user requested immediate termination\n");
        }
        state->repterminationtype = 8;
        goto lbl_8;
    }
    
    /*
     * Trace output (if needed)
     */
    if( state->dotrace )
    {
        ae_trace("\n=== ITERATION %5d STARTED ========================================================================\n",
            (int)(state->repiterationscount));
        ae_trace("DeltaK        = %0.3e (current trust radius factor)\n",
            (double)(state->trustradfactor));
        ae_trace("RhoK          = %0.3e (lower bound on the trust radius)\n",
            (double)(state->trustradbnd));
        ae_assert(state->cntlc==0&&state->cntnlc==0, "DFM: integrity check 2824 failed", _state);
        ae_trace("|x|inf        = %0.3e\n",
            (double)(rmaxabsv(n, &state->xk, _state)));
        ae_trace("> computational budget used:\n");
        ae_trace("eval          = %7d   (target/constraints evaluations)\n",
            (int)(state->repnfev));
        ae_trace("splx.equiv    = %9.1f (simplex gradient equivalent = evaluations/(N+1) )\n",
            (double)((double)state->repnfev/(double)(n+1)));
        rallocv(n, &state->tmp0, _state);
        rallocv(n, &state->tmp1, _state);
        dfgenmod_mostandleastdistant(&state->invtrustregion, n, &state->wrkset, state->wrksetsize, &v, &i, &vv, &j, _state);
        ae_trace("> working set geometry:\n");
        ae_trace("furthest pt   = %6.2f (distance to furthest point divided by DeltaK)\n",
            (double)(v));
        ae_trace("nearest pt    = %6.2f (distance to nearest point divided by DeltaK)\n",
            (double)(vv));
        if( state->nnoisyrestarts>0 )
        {
            ae_trace("> restart autodetection parameters:\n");
            if( state->historylen>=dfgenmod_minhistory )
            {
                linregline(&state->iteridxhistory, &state->jacdiffhistory, state->historylen, &v, &v1, &vv, _state);
                ae_trace("slope         = %6.2f (slope of log(J[i+1]-J[i]) vs iteration index regression line)\n",
                    (double)(v1));
                ae_trace("correlation   = %6.2f (corr  of log(J[i+1]-J[i]) vs iteration index regression line)\n",
                    (double)(vv));
                v = (double)(0);
                v1 = (double)(0);
                vv = (double)(0);
                for(i=0; i<=state->historylen-1; i++)
                {
                    if( ae_fp_greater(state->trustradhistory.ptr.p_double[i],(double)(0)) )
                    {
                        v = v+(double)1;
                    }
                    if( ae_fp_less(state->trustradhistory.ptr.p_double[i],(double)(0)) )
                    {
                        v1 = v1+(double)1;
                    }
                    if( ae_fp_eq(state->trustradhistory.ptr.p_double[i],(double)(0)) )
                    {
                        vv = vv+(double)1;
                    }
                }
                ae_trace("tr-increase   = %3d    (steps with increased trust radius)\n",
                    (int)(ae_round(v, _state)));
                ae_trace("tr-decrease   = %3d    (steps with decreased trust radius)\n",
                    (int)(ae_round(v1, _state)));
                ae_trace("tr-stagnate   = %3d    (steps with stagnated trust radius)\n",
                    (int)(ae_round(vv, _state)));
            }
            else
            {
                ae_trace(">> accumulating history...\n");
            }
        }
        ae_trace("\n");
    }
    
    /*
     * Generate trial request, invoke safety phase if necessary
     */
    dfgenmod_preparetrialrequest(state, &predf, &predh, _state);
    skrellen = dfgenmod_normrelativetotrustregion(state, &state->sk, _state);
    if( state->dotrace )
    {
        ae_trace("> prepared trial step with relative length |Sk|/DeltaK=%0.1e, predicted deltaF=%0.1e, deltaH=%0.1e\n",
            (double)(skrellen),
            (double)(predf-state->fk),
            (double)(predh-state->hk));
    }
    if( ae_fp_greater(skrellen*state->trustradfactor,dfgenmod_gammasafety*state->trustradbnd) )
    {
        goto lbl_13;
    }
    
    /*
     * Decrease trust region size and send step denial signal
     */
    if( state->dotrace )
    {
        ae_trace("> proposed step is too short, decreasing trust region radius\n");
    }
    dfgenmod_updatetrustregion(state, ae_maxreal(dfgenmod_omegasafety*state->trustradfactor, state->trustradbnd, _state), _state);
    if( dfgenmod_deltakatminimum(state, _state) )
    {
        v = state->trustradbnd;
        dfgenmod_updatetrustregion(state, dfgenmod_alpha2*v, _state);
        state->trustradbnd = dfgenmod_alpha1*v;
    }
    dfgenmod_sendstepdenialtomodel(state, _state);
    
    /*
     * Improve geometry of the dataset
     */
    if( state->dotrace )
    {
        ae_trace("> improving geometry of the working set\n");
    }
    ae_vector_set_length(&state->rstateimprove.ia, 9+1, _state);
    ae_vector_set_length(&state->rstateimprove.ba, 0+1, _state);
    ae_vector_set_length(&state->rstateimprove.ra, 8+1, _state);
    state->rstateimprove.stage = -1;
lbl_15:
    if( !dfgenmod_improveworkingsetgeometry(state, _state) )
    {
        goto lbl_16;
    }
    if( state->requesttype==4 )
    {
        state->repnfev = state->repnfev+state->querysize;
    }
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    goto lbl_15;
lbl_16:
    if( state->repterminationtype!=0 )
    {
        goto lbl_8;
    }
    
    /*
     * Done
     */
    inc(&state->repiterationscount, _state);
    goto lbl_7;
lbl_13:
    if( state->requesttype==4 )
    {
        state->repnfev = state->repnfev+state->querysize;
    }
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    if( !isfinitevector(&state->replyfi, (m+cntnlc)*state->querysize, _state) )
    {
        
        /*
         * Infinities in the target/constraints, decrease trust radius and retry
         */
        if( state->dotrace )
        {
            ae_trace("[WARNING] target at the trial point is infinite, decreasing trust radius and restarting iteration\n");
        }
        state->infinitiesencountered = ae_true;
        state->recoveredfrominfinities = ae_false;
        dfgenmod_updatetrustregion(state, dfgenmod_gammabad*state->trustradfactor, _state);
        state->trustradbnd = ae_minreal(state->trustradfactor, state->trustradbnd, _state);
        dfgenmod_sendstepdenialtomodel(state, _state);
        inc(&state->repiterationscount, _state);
        goto lbl_7;
    }
    dfgenmod_processtrialrequest(state, &fnew, &hnew, _state);
    if( state->dotrace )
    {
        ae_trace("> evaluating target at Xk+Sk, actual deltaF=%0.1e, deltaH=%0.1e\n",
            (double)(fnew-state->fk),
            (double)(hnew-state->hk));
    }
    
    /*
     * Accept or deny trial step
     */
    accepted = dfgenmod_testacceptancebyfilter(state, state->fk, state->hk, predf, predh, fnew, hnew, _state);
    if( !accepted )
    {
        goto lbl_17;
    }
    
    /*
     * Move to the new point
     * Update trust radius and filter
     */
    if( state->dotrace )
    {
        ae_trace("> trial step is accepted by the filter\n");
        if( !state->recoveredfrominfinities )
        {
            ae_trace("> successfully recovered from the previous encounter with infinities/NANs in the target!\n");
        }
        ae_trace("targetF:        %17.9e -> %17.9e (delta=%11.3e)\n",
            (double)(state->fk),
            (double)(fnew),
            (double)(fnew-state->fk));
        ae_trace("targetH:        %17.9e -> %17.9e (delta=%11.3e; constraints violation)\n",
            (double)(state->hk),
            (double)(hnew),
            (double)(hnew-state->hk));
    }
    ae_assert(state->cntlc==0&&state->cntnlc==0, "DFGM: integrity check 6641 failed", _state);
    ae_assert(ae_fp_eq(state->hk,(double)(0))&&ae_fp_eq(hnew,(double)(0)), "DFGM: integrity check 6642 failed", _state);
    if( ae_fp_less(predf,state->fk)&&ae_fp_greater((state->fk-fnew)/(state->fk-predf),dfgenmod_eta2) )
    {
        if( state->dotrace )
        {
            ae_trace("> model quality is good, increasing trust radius\n");
        }
        dfgenmod_updatetrustregion(state, ae_minreal(dfgenmod_gammainc*state->trustradfactor, dfgenmod_gammainc2*(skrellen*state->trustradfactor), _state), _state);
    }
    else
    {
        if( state->dotrace )
        {
            ae_trace("> model quality is low, tightening trust radius\n");
        }
        v = ae_maxreal(dfgenmod_gammadec, skrellen, _state)*state->trustradfactor;
        if( ae_fp_less(v,1.5*state->trustradbnd) )
        {
            v = state->trustradbnd;
        }
        dfgenmod_updatetrustregion(state, v, _state);
    }
    rcopyv(n, &state->xk, &state->xp, _state);
    rcopyv(m+state->cntnlc, &state->fkvec, &state->fpvec, _state);
    state->fp = state->fk;
    state->hp = state->hk;
    rcopyv(n, &state->xn, &state->xk, _state);
    rcopyv(m+state->cntnlc, &state->fnvec, &state->fkvec, _state);
    state->fk = fnew;
    state->hk = hnew;
    state->recoveredfrominfinities = ae_true;
    dfgenmod_sendtofilter(state, fnew, hnew, _state);
    
    /*
     * Perform step and update working set and model, with possible generation of RComm requests in the process
     */
    if( state->dotrace )
    {
        ae_trace("> updating working set\n");
    }
    ae_vector_set_length(&state->rstateupdate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstateupdate.ra, 2+1, _state);
    state->rstateupdate.stage = -1;
lbl_19:
    if( !dfgenmod_updateworkingsetandmodel(state, &state->xp, &state->xn, &state->fnvec, fnew, hnew, ae_true, _state) )
    {
        goto lbl_20;
    }
    if( state->requesttype==4 )
    {
        state->repnfev = state->repnfev+state->querysize;
    }
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    goto lbl_19;
lbl_20:
    if( state->repterminationtype!=0 )
    {
        goto lbl_8;
    }
    goto lbl_18;
lbl_17:
    
    /*
     * Decrease trust radius, send step denial signal
     */
    if( state->dotrace )
    {
        ae_trace("> trial step is not accepted by the filter, decreasing trust radius\n");
    }
    v = ae_minreal(dfgenmod_gammadec, skrellen, _state)*state->trustradfactor;
    if( ae_fp_less(v,1.5*state->trustradbnd) )
    {
        v = state->trustradbnd;
    }
    dfgenmod_updatetrustregion(state, v, _state);
    dfgenmod_sendstepdenialtomodel(state, _state);
    
    /*
     * Update working set and model in order to reuse new information as much as possible.
     * Move center to the best point in the working set, as suggested by DFO-LSA authors.
     */
    if( state->dotrace )
    {
        ae_trace("> updating working set\n");
    }
    ae_vector_set_length(&state->rstateupdate.ia, 6+1, _state);
    ae_vector_set_length(&state->rstateupdate.ra, 2+1, _state);
    state->rstateupdate.stage = -1;
lbl_21:
    if( !dfgenmod_updateworkingsetandmodel(state, &state->xk, &state->xn, &state->fnvec, fnew, hnew, ae_false, _state) )
    {
        goto lbl_22;
    }
    if( state->requesttype==4 )
    {
        state->repnfev = state->repnfev+state->querysize;
    }
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    goto lbl_21;
lbl_22:
    if( state->repterminationtype!=0 )
    {
        goto lbl_8;
    }
    
    /*
     * Handle denial
     */
    if( !(dfgenmod_restartautodetected(state, _state)&&state->restartstoperform>0) )
    {
        goto lbl_23;
    }
    
    /*
     * Step not accepted, the situation suggests restart
     */
    if( state->dotrace )
    {
        ae_trace("> \n");
    }
    dfgenmod_updatetrustregion(state, coalesce(state->rad0, 0.1, _state), _state);
    state->trustradbnd = state->trustradfactor;
    state->wrksetsize = 0;
    state->historylen = 0;
    state->historynext = 0;
    state->restartstoperform = state->restartstoperform-1;
    if( state->dotrace )
    {
        ae_trace("> restart is performed due to an autodetection code signal (%0d restarts left)\n",
            (int)(state->restartstoperform));
    }
    goto lbl_24;
lbl_23:
    if( !dfgenmod_workingsetneedsimprovement(state, _state) )
    {
        goto lbl_25;
    }
    
    /*
     * Step not accepted, working set geometry needs improvement
     */
    if( state->dotrace )
    {
        ae_trace("> working set needs improvement\n");
    }
    ae_vector_set_length(&state->rstateimprove.ia, 9+1, _state);
    ae_vector_set_length(&state->rstateimprove.ba, 0+1, _state);
    ae_vector_set_length(&state->rstateimprove.ra, 8+1, _state);
    state->rstateimprove.stage = -1;
lbl_27:
    if( !dfgenmod_improveworkingsetgeometry(state, _state) )
    {
        goto lbl_28;
    }
    if( state->requesttype==4 )
    {
        state->repnfev = state->repnfev+state->querysize;
    }
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    goto lbl_27;
lbl_28:
    if( state->repterminationtype!=0 )
    {
        goto lbl_8;
    }
    goto lbl_26;
lbl_25:
    
    /*
     * Unsuccessful phase: step not accepted, restart not suggested, working set geometry does not need improvement
     */
    if( dfgenmod_deltakatminimum(state, _state) )
    {
        if( state->dotrace )
        {
            ae_trace("> trust radius at the lower bound, decreasing the bound\n");
        }
        v = state->trustradbnd;
        dfgenmod_updatetrustregion(state, dfgenmod_alpha2*v, _state);
        state->trustradbnd = dfgenmod_alpha1*v;
    }
lbl_26:
lbl_24:
lbl_18:
    
    /*
     * Update counters
     */
    inc(&state->repiterationscount, _state);
    goto lbl_7;
lbl_8:
    
    /*
     * Finalize
     */
    ae_assert(state->repterminationtype!=0, "DFGM: integrity check 0808 failed", _state);
    if( state->repterminationtype<0 )
    {
        result = ae_false;
        return result;
    }
    if( state->infinitiesencountered )
    {
        if( !state->recoveredfrominfinities )
        {
            if( state->dotrace )
            {
                ae_trace("> optimizer failed to recover from infinities, declaring unsuccessful termination\n");
            }
            state->repterminationtype = -8;
            result = ae_false;
            return result;
        }
        if( state->dotrace )
        {
            ae_trace("> optimizer encountered infinities, but successfully recovered from them, adding +800 to the completion code\n");
        }
        state->repterminationtype = state->repterminationtype+800;
    }
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = i;
    state->rstate.ia.ptr.p_int[1] = j;
    state->rstate.ia.ptr.p_int[2] = n;
    state->rstate.ia.ptr.p_int[3] = m;
    state->rstate.ia.ptr.p_int[4] = cntnlc;
    state->rstate.ba.ptr.p_bool[0] = accepted;
    state->rstate.ra.ptr.p_double[0] = predf;
    state->rstate.ra.ptr.p_double[1] = predh;
    state->rstate.ra.ptr.p_double[2] = fnew;
    state->rstate.ra.ptr.p_double[3] = hnew;
    state->rstate.ra.ptr.p_double[4] = skrellen;
    state->rstate.ra.ptr.p_double[5] = v;
    state->rstate.ra.ptr.p_double[6] = v1;
    state->rstate.ra.ptr.p_double[7] = vv;
    return result;
}


void linregline(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     double* a,
     double* b,
     double* corrxy,
     ae_state *_state)
{
    ae_int_t i;
    double ss;
    double sx;
    double sxx;
    double sy;
    double stt;
    double e1;
    double e2;
    double t;
    double varx;
    double vary;
    double meanx;
    double meany;

    *a = 0.0;
    *b = 0.0;
    *corrxy = 0.0;

    ae_assert(n>=2, "DFGM: integrity check 6541 failed", _state);
    
    /*
     * Calculate S, SX, SY, SXX
     */
    ss = (double)(0);
    sx = (double)(0);
    sy = (double)(0);
    sxx = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        ss = ss+(double)1;
        sx = sx+x->ptr.p_double[i];
        sy = sy+y->ptr.p_double[i];
        sxx = sxx+ae_sqr(x->ptr.p_double[i], _state);
    }
    
    /*
     * Test for condition number
     */
    t = ae_sqrt((double)4*ae_sqr(sx, _state)+ae_sqr(ss-sxx, _state), _state);
    e1 = 0.5*(ss+sxx+t);
    e2 = 0.5*(ss+sxx-t);
    ae_assert(ae_fp_greater(ae_minreal(e1, e2, _state),(double)1000*ae_machineepsilon*ae_maxreal(e1, e2, _state)), "DFGM: integrity check 6702 failed", _state);
    
    /*
     * Calculate A, B
     */
    *a = (double)(0);
    *b = (double)(0);
    stt = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        t = x->ptr.p_double[i]-sx/ss;
        *b = *b+t*y->ptr.p_double[i];
        stt = stt+ae_sqr(t, _state);
    }
    *b = *b/stt;
    *a = (sy-sx*(*b))/ss;
    
    /*
     * Calculate other parameters
     */
    meanx = sx/(double)n;
    meany = sy/(double)n;
    varx = (double)(0);
    vary = (double)(0);
    *corrxy = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        varx = varx+(x->ptr.p_double[i]-meanx)*(x->ptr.p_double[i]-meanx);
        vary = vary+(y->ptr.p_double[i]-meany)*(y->ptr.p_double[i]-meany);
        *corrxy = *corrxy+(x->ptr.p_double[i]-meanx)*(y->ptr.p_double[i]-meany);
    }
    *corrxy = *corrxy/ae_sqrt(coalesce(varx*vary, (double)(1), _state), _state);
}


/*************************************************************************
Initialize history of changes in the trust radius and the Jacobian.

If NNoisyRestarts is zero from the very beginning, history is not tracked.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static void dfgenmod_inithistory(dfgmstate* state, ae_state *_state)
{


    state->restartstoperform = state->nnoisyrestarts;
    state->historylen = 0;
    state->historynext = 0;
    if( state->nnoisyrestarts==0 )
    {
        return;
    }
    state->lasttrustrad = (double)(0);
    rsetallocm(state->m+state->cntnlc, state->n, 0.0, &state->lastjac, _state);
    rallocv(dfgenmod_maxhistory, &state->trustradhistory, _state);
    rallocv(dfgenmod_maxhistory, &state->jacdiffhistory, _state);
    rallocv(dfgenmod_maxhistory, &state->iteridxhistory, _state);
}


/*************************************************************************
Updates history

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static void dfgenmod_updatehistory(dfgmstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntnlc;
    ae_int_t i;
    ae_bool b;
    double jacdiff2;


    n = state->n;
    m = state->m;
    cntnlc = state->cntnlc;
    if( state->nnoisyrestarts==0 )
    {
        return;
    }
    
    /*
     * Enqueue trust radius change
     */
    state->trustradhistory.ptr.p_double[state->historynext] = (double)(0);
    if( ae_fp_less(state->trustradfactor,0.999*state->lasttrustrad) )
    {
        state->trustradhistory.ptr.p_double[state->historynext] = (double)(-1);
    }
    if( ae_fp_greater(state->trustradfactor,1.001*state->lasttrustrad) )
    {
        state->trustradhistory.ptr.p_double[state->historynext] = (double)(1);
    }
    state->lasttrustrad = state->trustradfactor;
    
    /*
     * Compute difference in Jacobians, enqueue
     */
    rallocv(n, &state->tmp0, _state);
    jacdiff2 = (double)(0);
    b = ae_false;
    if( state->modeltype==0 )
    {
        for(i=0; i<=m+cntnlc-1; i++)
        {
            rcopyrv(n, &state->tpsmodel.jac, i, &state->tmp0, _state);
            raddrv(n, -1.0, &state->lastjac, i, &state->tmp0, _state);
            jacdiff2 = jacdiff2+rdotv2(n, &state->tmp0, _state);
        }
        rcopym(m+cntnlc, n, &state->tpsmodel.jac, &state->lastjac, _state);
        b = ae_true;
    }
    if( state->modeltype==1 )
    {
        for(i=0; i<=m+cntnlc-1; i++)
        {
            rcopyrv(n, &state->lsamodel.jac, i, &state->tmp0, _state);
            raddrv(n, -1.0, &state->lastjac, i, &state->tmp0, _state);
            jacdiff2 = jacdiff2+rdotv2(n, &state->tmp0, _state);
        }
        rcopym(m+cntnlc, n, &state->lsamodel.jac, &state->lastjac, _state);
        b = ae_true;
    }
    ae_assert(b, "DFGM: integrity check 5451 failed", _state);
    state->jacdiffhistory.ptr.p_double[state->historynext] = ae_log(coalesce(jacdiff2, ae_machineepsilon*ae_machineepsilon, _state), _state);
    
    /*
     * Enqueue iterations count
     */
    state->iteridxhistory.ptr.p_double[state->historynext] = (double)(state->repiterationscount);
    
    /*
     * Update history size
     */
    state->historylen = ae_minint(state->historylen+1, dfgenmod_maxhistory, _state);
    state->historynext = (state->historynext+1)%dfgenmod_maxhistory;
}


/*************************************************************************
Compute per-variable trust region using trust region factor and a  current
location State.Xk

The trust region is stored to State.TrustRegion[], which is array[N], can
be automatically allocated by this function.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static void dfgenmod_updatetrustregion(dfgmstate* state,
     double trustradfactor,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    state->trustradfactor = trustradfactor;
    rallocv(n, &state->trustregion, _state);
    rallocv(n, &state->invtrustregion, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(ae_fp_less(state->finitebndl.ptr.p_double[i],state->finitebndu.ptr.p_double[i]), "DFGM: integrity check 4230 failed", _state);
        state->trustregion.ptr.p_double[i] = trustradfactor*ae_minreal(ae_maxreal(ae_fabs(state->xk.ptr.p_double[i], _state), (double)(1), _state), state->finitebndu.ptr.p_double[i]-state->finitebndl.ptr.p_double[i], _state);
        state->invtrustregion.ptr.p_double[i] = (double)1/state->trustregion.ptr.p_double[i];
    }
}


/*************************************************************************
Returns max(|x[i]|/State.TrustRegion[I])

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static double dfgenmod_normrelativetotrustregion(const dfgmstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    double result;


    n = state->n;
    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        result = ae_maxreal(result, ae_fabs(x->ptr.p_double[i]/state->trustregion.ptr.p_double[i], _state), _state);
    }
    return result;
}


/*************************************************************************
Prepare trial step Sk, Xn=Xk+Sk (guarded  against  box  constraints), load
RComm V2 request to  RequestType,  QuerySize,  QueryData  and  preallocate
ReplyFi.

Returns predicted value of F (target) and sum of constraints violations H

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static void dfgenmod_preparetrialrequest(dfgmstate* state,
     double* predf,
     double* predh,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_int_t ipmterminationtype;
    ae_int_t i;

    *predf = 0.0;
    *predh = 0.0;

    n = state->n;
    m = state->m;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    *predf = (double)(0);
    *predh = (double)(0);
    
    /*
     * 2PS
     */
    if( state->modeltype==0 )
    {
        ae_assert(cntlc==0, "DFGM: integrity check 4429 failed", _state);
        ae_assert(cntnlc==0, "DFGM: integrity check 4430 failed", _state);
        ae_assert(state->isleastsquares, "DFGM: integrity check 4431 failed", _state);
        
        /*
         * Generate trial step
         */
        rsetallocv(n, 0.0, &state->tmp0, _state);
        vipminitdense(&state->ipmsolver, &state->trustregion, &state->tmp0, n, _state);
        vipmsetquadraticlinear(&state->ipmsolver, &state->tpsmodel.q, &state->sparsedummy, 0, ae_true, &state->tpsmodel.g, _state);
        rallocv(n, &state->tmp1, _state);
        for(i=0; i<=n-1; i++)
        {
            state->tmp0.ptr.p_double[i] = -state->trustregion.ptr.p_double[i];
            state->tmp1.ptr.p_double[i] = state->trustregion.ptr.p_double[i];
            if( state->hasbndl.ptr.p_bool[i]&&ae_fp_greater(state->scaledbndl.ptr.p_double[i]-state->xk.ptr.p_double[i],state->tmp0.ptr.p_double[i]) )
            {
                state->tmp0.ptr.p_double[i] = state->scaledbndl.ptr.p_double[i]-state->xk.ptr.p_double[i];
            }
            if( state->hasbndu.ptr.p_bool[i]&&ae_fp_less(state->scaledbndu.ptr.p_double[i]-state->xk.ptr.p_double[i],state->tmp1.ptr.p_double[i]) )
            {
                state->tmp1.ptr.p_double[i] = state->scaledbndu.ptr.p_double[i]-state->xk.ptr.p_double[i];
            }
        }
        vipmsetconstraints(&state->ipmsolver, &state->tmp0, &state->tmp1, &state->sparsedummy, 0, &state->densedummy, 0, &state->tmp0, &state->tmp1, _state);
        vipmoptimize(&state->ipmsolver, ae_false, &state->tmp0, &state->tmp1, &state->tmp2, &ipmterminationtype, _state);
        ae_assert(ipmterminationtype>0, "DFGM: integrity check 7740 failed", _state);
        rcopyv(n, &state->tmp0, &state->sk, _state);
        rcopyv(n, &state->xk, &state->xn, _state);
        raddv(n, 1.0, &state->sk, &state->xn, _state);
        enforceboundaryconstraints(&state->xn, &state->scaledbndl, &state->hasbndl, &state->scaledbndu, &state->hasbndu, n, 0, _state);
        
        /*
         * Predicted value of target and constraints
         */
        ae_assert(cntlc==0, "DFGM: integrity check 9544 failed", _state);
        ae_assert(cntnlc==0, "DFGM: integrity check 9545 failed", _state);
        rallocv(n, &state->tmp0, _state);
        rgemv(n, n, 1.0, &state->tpsmodel.q, 0, &state->sk, 0.0, &state->tmp0, _state);
        *predf = state->fk+rdotv(n, &state->sk, &state->tpsmodel.g, _state)+0.5*rdotv(n, &state->sk, &state->tmp0, _state);
        *predh = (double)(0);
        
        /*
         * Issue RComm V2 request
         */
        state->requesttype = 4;
        state->querysize = 1;
        rcopyallocv(n, &state->xn, &state->querydata, _state);
        rallocv(m+cntnlc, &state->replyfi, _state);
        
        /*
         * Done
         */
        return;
    }
    
    /*
     * DFO-LSA
     */
    if( state->modeltype==1 )
    {
        ae_assert(cntlc==0, "DFGM: integrity check 2539 failed", _state);
        ae_assert(cntnlc==0, "DFGM: integrity check 2639 failed", _state);
        ae_assert(state->isleastsquares, "DFGM: integrity check 2739 failed", _state);
        
        /*
         * Generate trial step
         */
        rsetallocv(n, 0.0, &state->tmp0, _state);
        vipminitdense(&state->ipmsolver, &state->trustregion, &state->tmp0, n, _state);
        vipmsetquadraticlinear(&state->ipmsolver, &state->lsamodel.q, &state->sparsedummy, 0, ae_true, &state->lsamodel.g, _state);
        rallocv(n, &state->tmp1, _state);
        for(i=0; i<=n-1; i++)
        {
            state->tmp0.ptr.p_double[i] = -state->trustregion.ptr.p_double[i];
            state->tmp1.ptr.p_double[i] = state->trustregion.ptr.p_double[i];
            if( state->hasbndl.ptr.p_bool[i]&&ae_fp_greater(state->scaledbndl.ptr.p_double[i]-state->xk.ptr.p_double[i],state->tmp0.ptr.p_double[i]) )
            {
                state->tmp0.ptr.p_double[i] = state->scaledbndl.ptr.p_double[i]-state->xk.ptr.p_double[i];
            }
            if( state->hasbndu.ptr.p_bool[i]&&ae_fp_less(state->scaledbndu.ptr.p_double[i]-state->xk.ptr.p_double[i],state->tmp1.ptr.p_double[i]) )
            {
                state->tmp1.ptr.p_double[i] = state->scaledbndu.ptr.p_double[i]-state->xk.ptr.p_double[i];
            }
        }
        vipmsetconstraints(&state->ipmsolver, &state->tmp0, &state->tmp1, &state->sparsedummy, 0, &state->densedummy, 0, &state->tmp0, &state->tmp1, _state);
        vipmoptimize(&state->ipmsolver, ae_false, &state->tmp0, &state->tmp1, &state->tmp2, &ipmterminationtype, _state);
        ae_assert(ipmterminationtype>0, "DFGM: integrity check 7740 failed", _state);
        rcopyv(n, &state->tmp0, &state->sk, _state);
        rcopyv(n, &state->xk, &state->xn, _state);
        raddv(n, 1.0, &state->sk, &state->xn, _state);
        enforceboundaryconstraints(&state->xn, &state->scaledbndl, &state->hasbndl, &state->scaledbndu, &state->hasbndu, n, 0, _state);
        
        /*
         * Predicted value of target and constraints
         */
        ae_assert(cntlc==0, "DFGM: integrity check 9544 failed", _state);
        ae_assert(cntnlc==0, "DFGM: integrity check 9545 failed", _state);
        rallocv(n, &state->tmp0, _state);
        rgemv(n, n, 1.0, &state->lsamodel.q, 0, &state->sk, 0.0, &state->tmp0, _state);
        *predf = state->fk+rdotv(n, &state->sk, &state->lsamodel.g, _state)+0.5*rdotv(n, &state->sk, &state->tmp0, _state);
        *predh = (double)(0);
        
        /*
         * Issue RComm V2 request
         */
        state->requesttype = 4;
        state->querysize = 1;
        rcopyallocv(n, &state->xn, &state->querydata, _state);
        rallocv(m+cntnlc, &state->replyfi, _state);
        
        /*
         * Done
         */
        return;
    }
    ae_assert(ae_false, "DFGM: PrepareTrialRequest() is unable to handle unexpected algorithm", _state);
}


/*************************************************************************
Processes results of the RComm V2 request for a trial point.

Returns the value of F (target) and sum of constraints violations H

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static void dfgenmod_processtrialrequest(dfgmstate* state,
     double* f,
     double* h,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;

    *f = 0.0;
    *h = 0.0;

    m = state->m;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    *f = (double)(0);
    *h = (double)(0);
    
    /*
     * 2PS, LSA
     */
    if( state->modeltype==0||state->modeltype==1 )
    {
        ae_assert(cntlc==0, "DFGM: integrity check 6222 failed", _state);
        ae_assert(cntnlc==0, "DFGM: integrity check 6323 failed", _state);
        ae_assert(state->isleastsquares, "DFGM: integrity check 6424 failed", _state);
        ae_assert(state->querysize==1, "DFGM: integrity check 3540 failed", _state);
        *f = 0.5*rdotv2(m, &state->replyfi, _state);
        *h = (double)(0);
        rcopyv(m+cntnlc, &state->replyfi, &state->fnvec, _state);
        return;
    }
    ae_assert(ae_false, "DFGM: ProcessTrialRequest() is unable to handle unexpected algorithm", _state);
}


/*************************************************************************
Tests step acceptance.

Input parameters:
* Fk, Hk - current target Fk and a sum of constraint violations Hk at Xk
* Fp, Hp - predicted values
* Fn, Hn - actual values at Xn=Xk+Sk

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool dfgenmod_testacceptancebyfilter(const dfgmstate* state,
     double fk,
     double hk,
     double fp,
     double hp,
     double fn,
     double hn,
     ae_state *_state)
{
    ae_bool result;


    ae_assert((ae_fp_eq(hk,(double)(0))&&ae_fp_eq(hp,(double)(0)))&&ae_fp_eq(hn,(double)(0)), "DFGM: TestAcceptanceByFilter does not support constraints yet", _state);
    result = ae_fp_less(fp,fk)&&ae_fp_greater(fk-fn,dfgenmod_eta1*(fk-fp));
    return result;
}


/*************************************************************************
Inserts current iterate into the filter. If the iterate is not acceptable,
it is silently denied.

Input parameters:
* Fn, Hn - target Fk and a sum of constraint violations Hk at Xn=Xk+Sk

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static void dfgenmod_sendtofilter(const dfgmstate* state,
     double fn,
     double hn,
     ae_state *_state)
{


}


/*************************************************************************
Sends information about denial of the current step.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static void dfgenmod_sendstepdenialtomodel(dfgmstate* state,
     ae_state *_state)
{


    
    /*
     * 2PS
     */
    if( state->modeltype==0 )
    {
        return;
    }
    
    /*
     * LSA
     */
    if( state->modeltype==1 )
    {
        return;
    }
    ae_assert(ae_false, "DFGM: SendDenialToModel() is unable to handle unexpected algorithm", _state);
}


/*************************************************************************
Returns true if State.DeltaK is roughly State.RhoK

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool dfgenmod_deltakatminimum(const dfgmstate* state,
     ae_state *_state)
{
    ae_bool result;


    result = ae_fp_less_eq(state->trustradfactor,state->trustradbnd*1.01);
    return result;
}


/*************************************************************************
Returns true if restart is recommended right now

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool dfgenmod_restartautodetected(const dfgmstate* state,
     ae_state *_state)
{
    ae_bool result;


    result = ae_false;
    return result;
}


/*************************************************************************
Returns true if working set needs improvement

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool dfgenmod_workingsetneedsimprovement(dfgmstate* state,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    double mostdistant;
    double leastdistant;
    ae_bool result;


    n = state->n;
    result = ae_false;
    
    /*
     * 2PS, LSA
     */
    if( state->modeltype==0||state->modeltype==1 )
    {
        ae_assert(state->wrksetsize==n+1, "DFGM: integrity check 2111 failed", _state);
        dfgenmod_mostandleastdistant(&state->invtrustregion, n, &state->wrkset, state->wrksetsize, &mostdistant, &i, &leastdistant, &j, _state);
        if( state->modeltype==0 )
        {
            result = ae_fp_greater(mostdistant,dfgenmod_twopsstephi)||ae_fp_less(leastdistant,dfgenmod_twopssteplo);
        }
        else
        {
            result = ae_fp_greater(mostdistant,dfgenmod_pointtoofar)||ae_fp_less(leastdistant,dfgenmod_pointtooclose);
        }
        return result;
    }
    ae_assert(ae_false, "DFGM: WorkingSetNeedsImprovement() is unable to handle unexpected algorithm", _state);
    return result;
}


/*************************************************************************
Handle successful step.

This function updates working set and (for some solvers) issues additional
RComm V2 requests if working set needs immediate revaluation.

It also updates the model.

On exit it may set State.RepTerminationType to a negative  code  (failure)
if it encountered infinities in the target/constraints during  update  and
failed to recover from the problem.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool dfgenmod_updateworkingsetandmodel(dfgmstate* state,
     /* Real    */ const ae_vector* xk,
     /* Real    */ const ae_vector* xn,
     /* Real    */ const ae_vector* fnvec,
     double fn,
     double hn,
     ae_bool successfulstep,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_int_t i;
    ae_int_t j;
    ae_int_t replaceidx;
    double rating;
    double bestrating;
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
    if( state->rstateupdate.stage>=0 )
    {
        n = state->rstateupdate.ia.ptr.p_int[0];
        m = state->rstateupdate.ia.ptr.p_int[1];
        cntlc = state->rstateupdate.ia.ptr.p_int[2];
        cntnlc = state->rstateupdate.ia.ptr.p_int[3];
        i = state->rstateupdate.ia.ptr.p_int[4];
        j = state->rstateupdate.ia.ptr.p_int[5];
        replaceidx = state->rstateupdate.ia.ptr.p_int[6];
        rating = state->rstateupdate.ra.ptr.p_double[0];
        bestrating = state->rstateupdate.ra.ptr.p_double[1];
        v = state->rstateupdate.ra.ptr.p_double[2];
    }
    else
    {
        n = -541;
        m = -698;
        cntlc = -900;
        cntnlc = -318;
        i = -940;
        j = 1016;
        replaceidx = -229;
        rating = -536.0;
        bestrating = 487.0;
        v = -115.0;
    }
    if( state->rstateupdate.stage==0 )
    {
        goto lbl_0;
    }
    
    /*
     * Routine body
     */
    n = state->n;
    m = state->m;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    
    /*
     * 2PS
     */
    if( state->modeltype!=0 )
    {
        goto lbl_1;
    }
    
    /*
     * Do not update working set on unsuccessful steps because this solver uses fixed
     * stencil which does not allow inclusion of arbitrary points. The only way to update
     * working set is to rebuild it at a new point using the same stencil.
     */
    if( !successfulstep )
    {
        result = ae_false;
        return result;
    }
    
    /*
     * Clear working set (it will be recomputed from scratch on a new place) and call working set improvement code
     */
    state->wrksetsize = 0;
    ae_vector_set_length(&state->rstateimprove.ia, 9+1, _state);
    ae_vector_set_length(&state->rstateimprove.ba, 0+1, _state);
    ae_vector_set_length(&state->rstateimprove.ra, 8+1, _state);
    state->rstateimprove.stage = -1;
lbl_3:
    if( !dfgenmod_improveworkingsetgeometry(state, _state) )
    {
        goto lbl_4;
    }
    state->rstateupdate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    goto lbl_3;
lbl_4:
    result = ae_false;
    return result;
lbl_1:
    
    /*
     * LSA
     */
    if( state->modeltype==1 )
    {
        
        /*
         * If the step was not successful, we have to skip update if the new point
         * is too close to the center. When the step is successful, we can replace
         * the center. But it is not possible on unsuccessful steps.
         */
        if( !successfulstep )
        {
            v = (double)(0);
            for(j=0; j<=n-1; j++)
            {
                v = ae_maxreal(v, ae_fabs(xn->ptr.p_double[j]-xk->ptr.p_double[j], _state)*state->invtrustregion.ptr.p_double[j], _state);
            }
            if( ae_fp_less(v,dfgenmod_pointtooclose) )
            {
                result = ae_false;
                return result;
            }
        }
        
        /*
         * Select ReplaceIdx - index of a point to replace by the new point Xn,
         * with 0<=ReplaceIdx<N meaning replacing off-center points of the working
         * set, and ReplaceIdx=N meaning replacing (previously) central point.
         */
        rallocv(n, &state->tmp0, _state);
        rallocv(n, &state->tmp1, _state);
        replaceidx = -1;
        bestrating = (double)(0);
        for(i=0; i<=n; i++)
        {
            
            /*
             * Skip central point if the step is not successful (we remain at the old
             * place, so the central point can not be replaced)
             */
            if( i==n&&!successfulstep )
            {
                break;
            }
            
            /*
             * Compute Lagrange polynomial coefficients
             */
            if( i==n )
            {
                rsetv(n, 0.0, &state->tmp0, _state);
                for(j=0; j<=n-1; j++)
                {
                    rcopycv(n, &state->lsamodel.invw, j, &state->tmp1, _state);
                    raddv(n, -1.0, &state->tmp1, &state->tmp0, _state);
                }
            }
            else
            {
                rcopycv(n, &state->lsamodel.invw, i, &state->tmp0, _state);
            }
            
            /*
             * Compute rating as |L(Xn-Xk)|*max(|Xn-Wrk[I]|/Scales,1)^2, compare with the best
             */
            rcopyrv(n, &state->wrkset, i, &state->tmp1, _state);
            raddv(n, -1.0, xn, &state->tmp1, _state);
            rating = ae_fabs(rdotv(n, xn, &state->tmp0, _state)-rdotv(n, xk, &state->tmp0, _state)+rcase2(i==n, (double)(1), (double)(0), _state), _state)*ae_sqr(ae_maxreal(dfgenmod_normrelativetotrustregion(state, &state->tmp1, _state), (double)(1), _state), _state);
            if( replaceidx<0||ae_fp_greater(rating,bestrating) )
            {
                replaceidx = i;
                bestrating = rating;
            }
        }
        ae_assert(replaceidx>=0, "DFGM: integrity check 2850 failed", _state);
        if( state->dotrace )
        {
            if( replaceidx<n )
            {
                ae_trace(">> replacing point %0d (non-central)\n",
                    (int)(replaceidx));
            }
            else
            {
                ae_trace(">> replacing previously central point\n");
            }
        }
        
        /*
         * Modify WrkSet
         */
        if( successfulstep )
        {
            if( replaceidx<n )
            {
                
                /*
                 * Successful step, we replace off-central point by Xn.
                 *
                 * Because we always move Xn to N-th row of the working set, it means
                 * that we push the former central point from the row #N to the
                 * ReplaceIdx-th one.
                 */
                rcopyrr(n+m+cntnlc, &state->wrkset, n, &state->wrkset, replaceidx, _state);
                rcopyvr(n, xn, &state->wrkset, n, _state);
                for(i=0; i<=m+cntnlc-1; i++)
                {
                    state->wrkset.ptr.pp_double[n][n+i] = fnvec->ptr.p_double[i];
                }
            }
            else
            {
                
                /*
                 * Successful step, we replace central point by Xn.
                 */
                rcopyvr(n, xn, &state->wrkset, n, _state);
                for(i=0; i<=m+cntnlc-1; i++)
                {
                    state->wrkset.ptr.pp_double[n][n+i] = fnvec->ptr.p_double[i];
                }
            }
        }
        else
        {
            
            /*
             * Non-successful step, only off-central point can be replaced. The center
             * is not replaced or moved.
             */
            ae_assert(replaceidx<n, "DFGM: integrity check 6712 failed", _state);
            rcopyvr(n, xn, &state->wrkset, replaceidx, _state);
            for(i=0; i<=m+cntnlc-1; i++)
            {
                state->wrkset.ptr.pp_double[replaceidx][n+i] = fnvec->ptr.p_double[i];
            }
        }
        
        /*
         * Recompute matrices W and InvW
         */
        for(i=0; i<=n-1; i++)
        {
            rcopyrr(n, &state->wrkset, i, &state->lsamodel.w, i, _state);
            raddrr(n, -1.0, &state->wrkset, n, &state->lsamodel.w, i, _state);
        }
        rcopym(n, n, &state->lsamodel.w, &state->lsamodel.invw, _state);
        rmatrixinverse(&state->lsamodel.invw, n, &state->invrep, _state);
        if( state->invrep.terminationtype<=0 )
        {
            if( state->dotrace )
            {
                ae_trace(">> dataset matrix inverse failed, emergency termination with completion code +7\n");
            }
            state->repterminationtype = 7;
            result = ae_false;
            return result;
        }
        if( state->dotrace )
        {
            ae_trace(">> successfully inverted dataset matrix, cond=%0.2e\n",
                (double)((double)1/(state->invrep.r1+ae_machineepsilon)));
            rallocv(n, &state->tmp0, _state);
            rallocv(n, &state->tmp1, _state);
            v = (double)(0);
            for(i=0; i<=n; i++)
            {
                if( i==n )
                {
                    rsetv(n, 0.0, &state->tmp0, _state);
                    for(j=0; j<=n-1; j++)
                    {
                        rcopycv(n, &state->lsamodel.invw, j, &state->tmp1, _state);
                        raddv(n, -1.0, &state->tmp1, &state->tmp0, _state);
                    }
                }
                else
                {
                    rcopycv(n, &state->lsamodel.invw, i, &state->tmp0, _state);
                }
                v = ae_maxreal(v, dfgenmod_maxabslag(xn, n, &state->trustregion, &state->finitebndl, &state->finitebndu, &state->tmp0, rcase2(i==n, (double)(1), (double)(0), _state), &state->tmp1, &state->tmp2, ae_false, _state), _state);
            }
            ae_trace(">> poisedness constant is %0.2e\n",
                (double)(v));
        }
        
        /*
         * The working set is ready, rebuild a quadratic model of the target.
         *
         * Below we rely on the specific ordering of the working set
         * (the central point comes last, N additional ones come first).
         */
        ae_assert(state->cntnlc==0, "DFGM: integrity check 4811 failed", _state);
        ae_assert(state->wrksetsize==n+1, "DFGM: integrity check 0425 failed", _state);
        rallocm(n, m, &state->lsamodel.tmprhs, _state);
        rmatrixcopy(n, m, &state->wrkset, 0, n, &state->lsamodel.tmprhs, 0, 0, _state);
        rallocv(m, &state->lsamodel.tmp0, _state);
        for(i=0; i<=m-1; i++)
        {
            state->lsamodel.tmp0.ptr.p_double[i] = state->wrkset.ptr.pp_double[n][n+i];
        }
        for(i=0; i<=n-1; i++)
        {
            raddvr(m, -1.0, &state->lsamodel.tmp0, &state->lsamodel.tmprhs, i, _state);
        }
        rallocm(m, n, &state->lsamodel.jac, _state);
        rmatrixgemm(m, n, n, 1.0, &state->lsamodel.tmprhs, 0, 0, 1, &state->lsamodel.invw, 0, 0, 1, 0.0, &state->lsamodel.jac, 0, 0, _state);
        rsetallocv(n, 0.0, &state->lsamodel.g, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                state->lsamodel.g.ptr.p_double[i] = state->lsamodel.g.ptr.p_double[i]+state->wrkset.ptr.pp_double[n][n+j]*state->lsamodel.jac.ptr.pp_double[j][i];
            }
        }
        rallocm(n, n, &state->lsamodel.q, _state);
        rmatrixgemm(n, n, m, 1.0, &state->lsamodel.jac, 0, 0, 1, &state->lsamodel.jac, 0, 0, 0, 0.0, &state->lsamodel.q, 0, 0, _state);
        
        /*
         * Done
         */
        result = ae_false;
        return result;
    }
    ae_assert(ae_false, "DFGM: UpdateWorkingSetAndModel() is unable to handle unexpected algorithm", _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstateupdate.ia.ptr.p_int[0] = n;
    state->rstateupdate.ia.ptr.p_int[1] = m;
    state->rstateupdate.ia.ptr.p_int[2] = cntlc;
    state->rstateupdate.ia.ptr.p_int[3] = cntnlc;
    state->rstateupdate.ia.ptr.p_int[4] = i;
    state->rstateupdate.ia.ptr.p_int[5] = j;
    state->rstateupdate.ia.ptr.p_int[6] = replaceidx;
    state->rstateupdate.ra.ptr.p_double[0] = rating;
    state->rstateupdate.ra.ptr.p_double[1] = bestrating;
    state->rstateupdate.ra.ptr.p_double[2] = v;
    return result;
}


/*************************************************************************
For State.WrkSetSize=0 this function assumes that we actually  perform  an
initial model construction.  In  the  latter case we assume that the first
element of the working set contains some  meaningful value,  in the latter
case   the  working  set  is  built  completely  from scratch.

For State.WrkSetSize>0 this function assumes that the working set was
already initialized and we perform an update: modify working set and
(for some solvers) issues additional RComm V2 requests if working set needs
immediate revaluation.

It also updates the model:
* recomputes State.Fk and State.Hk
* updates current model by (possibly) incorporating State.Xk and (possibly)
  removing some of the previously included points.

On exit it may set State.RepTerminationType to a negative  code  (failure)
if it encountered infinities in the target/constraints during  update  and
failed to recover from the problem.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static ae_bool dfgenmod_improveworkingsetgeometry(dfgmstate* state,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_int_t i;
    ae_int_t j;
    double vlft;
    double vrgt;
    double v;
    double vmin;
    double vmax;
    double x0i;
    ae_int_t improveidx;
    ae_int_t improvedfar;
    ae_bool workingsetchanged;
    ae_int_t leastdistantidx;
    ae_int_t mostdistantidx;
    double leastdistant;
    double mostdistant;
    double mx;
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
    if( state->rstateimprove.stage>=0 )
    {
        n = state->rstateimprove.ia.ptr.p_int[0];
        m = state->rstateimprove.ia.ptr.p_int[1];
        cntlc = state->rstateimprove.ia.ptr.p_int[2];
        cntnlc = state->rstateimprove.ia.ptr.p_int[3];
        i = state->rstateimprove.ia.ptr.p_int[4];
        j = state->rstateimprove.ia.ptr.p_int[5];
        improveidx = state->rstateimprove.ia.ptr.p_int[6];
        improvedfar = state->rstateimprove.ia.ptr.p_int[7];
        leastdistantidx = state->rstateimprove.ia.ptr.p_int[8];
        mostdistantidx = state->rstateimprove.ia.ptr.p_int[9];
        workingsetchanged = state->rstateimprove.ba.ptr.p_bool[0];
        vlft = state->rstateimprove.ra.ptr.p_double[0];
        vrgt = state->rstateimprove.ra.ptr.p_double[1];
        v = state->rstateimprove.ra.ptr.p_double[2];
        vmin = state->rstateimprove.ra.ptr.p_double[3];
        vmax = state->rstateimprove.ra.ptr.p_double[4];
        x0i = state->rstateimprove.ra.ptr.p_double[5];
        leastdistant = state->rstateimprove.ra.ptr.p_double[6];
        mostdistant = state->rstateimprove.ra.ptr.p_double[7];
        mx = state->rstateimprove.ra.ptr.p_double[8];
    }
    else
    {
        n = 886;
        m = 346;
        cntlc = -722;
        cntnlc = -413;
        i = -461;
        j = 927;
        improveidx = 201;
        improvedfar = 922;
        leastdistantidx = -154;
        mostdistantidx = 306;
        workingsetchanged = ae_true;
        vlft = 951.0;
        vrgt = -463.0;
        v = 88.0;
        vmin = -861.0;
        vmax = -678.0;
        x0i = -731.0;
        leastdistant = -675.0;
        mostdistant = -763.0;
        mx = -233.0;
    }
    if( state->rstateimprove.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstateimprove.stage==1 )
    {
        goto lbl_1;
    }
    
    /*
     * Routine body
     */
    n = state->n;
    m = state->m;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    
    /*
     * If working set is empty (initialization) or algorithm rebuild
     * working set from scratch every time (like 2PS), initialize
     * working set using the best geometry possible (orthogonal distribution).
     *
     * After the working set was generated, the model is data structures
     * are initialized.
     */
    workingsetchanged = ae_false;
    if( !(state->wrksetsize==0||state->modeltype==0) )
    {
        goto lbl_2;
    }
    
    /*
     * Generate working set which satisfies box constraints
     */
    rallocv(n, &state->tmpdeltas, _state);
lbl_4:
    if( ae_false )
    {
        goto lbl_5;
    }
    
    /*
     * Initialize working set
     */
    rallocm(n+1, n+m+cntnlc, &state->wrkset, _state);
    state->wrksetsize = 0;
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i] )
        {
            ae_assert(ae_fp_greater(state->scaledbndu.ptr.p_double[i],state->scaledbndl.ptr.p_double[i]), "DFGM: integrity check 7909 failed", _state);
        }
        rcopyvr(n, &state->xk, &state->wrkset, state->wrksetsize, _state);
        
        /*
         * Evaluate two candidate points: one obtained by making a step to the left, and one obtained by making a step to the right
         */
        x0i = state->xk.ptr.p_double[i];
        vlft = boundval(x0i-dfgenmod_twopsstep*state->trustregion.ptr.p_double[i], state->finitebndl.ptr.p_double[i], state->finitebndu.ptr.p_double[i], _state);
        vrgt = boundval(x0i+dfgenmod_twopsstep*state->trustregion.ptr.p_double[i], state->finitebndl.ptr.p_double[i], state->finitebndu.ptr.p_double[i], _state);
        if( ae_fp_less_eq(x0i-vlft,0.9*(vrgt-x0i)) )
        {
            
            /*
             * The left step is shorter than the right one due to box constraints, choose the right step
             */
            state->wrkset.ptr.pp_double[state->wrksetsize][i] = vrgt;
            state->tmpdeltas.ptr.p_double[i] = vrgt-x0i;
            state->wrksetsize = state->wrksetsize+1;
            continue;
        }
        if( ae_fp_less_eq(vrgt-x0i,0.9*(x0i-vlft)) )
        {
            
            /*
             * The right step is shorter than the left one due to box constraints, choose the left step
             */
            state->wrkset.ptr.pp_double[state->wrksetsize][i] = vlft;
            state->tmpdeltas.ptr.p_double[i] = vlft-x0i;
            state->wrksetsize = state->wrksetsize+1;
            continue;
        }
        
        /*
         * Both steps are equally good, choose the random one
         */
        state->wrkset.ptr.pp_double[state->wrksetsize][i] = rcase2(hqrnduniformi(&state->rs, 2, _state)==0, vlft, vrgt, _state);
        state->tmpdeltas.ptr.p_double[i] = state->wrkset.ptr.pp_double[state->wrksetsize][i]-x0i;
        state->wrksetsize = state->wrksetsize+1;
    }
    rcopyvr(n, &state->xk, &state->wrkset, state->wrksetsize, _state);
    state->wrksetsize = state->wrksetsize+1;
    ae_assert(state->wrksetsize==n+1, "DFGM: integrity check 1309 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        state->tmpdeltas.ptr.p_double[i] = coalesce(state->tmpdeltas.ptr.p_double[i], 1.0, _state);
    }
    
    /*
     * Issue RComm V2 request, check that the reply does not contain infinities
     */
    state->requesttype = 4;
    state->querysize = state->wrksetsize;
    rallocv(n*state->querysize, &state->querydata, _state);
    rallocv((m+cntnlc)*state->querysize, &state->replyfi, _state);
    for(i=0; i<=state->wrksetsize-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            state->querydata.ptr.p_double[i*n+j] = state->wrkset.ptr.pp_double[i][j];
        }
    }
    state->rstateimprove.stage = 0;
    goto lbl_rcomm;
lbl_0:
    if( !isfinitevector(&state->replyfi, (m+cntnlc)*state->querysize, _state) )
    {
        
        /*
         * Decrease trust radius and retry
         */
        if( state->dotrace )
        {
            ae_trace("[WARNING] target at the geometry-improving point is infinite, decreasing trust radius and restarting iteration\n");
        }
        state->infinitiesencountered = ae_true;
        state->recoveredfrominfinities = ae_false;
        dfgenmod_updatetrustregion(state, dfgenmod_gammabad*state->trustradfactor, _state);
        state->trustradbnd = ae_minreal(state->trustradfactor, state->trustradbnd, _state);
        if( ae_fp_less(state->trustradfactor,coalesce(state->epsx, state->toosmalltrustrad, _state)) )
        {
            state->repterminationtype = -8;
            result = ae_false;
            return result;
        }
        goto lbl_4;
    }
    
    /*
     * Accept the reply
     */
    for(i=0; i<=state->wrksetsize-1; i++)
    {
        for(j=0; j<=m+cntnlc-1; j++)
        {
            state->wrkset.ptr.pp_double[i][n+j] = state->replyfi.ptr.p_double[i*(m+cntnlc)+j];
        }
    }
    goto lbl_5;
    goto lbl_4;
lbl_5:
    
    /*
     * Initialize model data structures
     */
    ae_assert(state->modeltype==0||state->modeltype==1, "DFGM: integrity check 0337 failed", _state);
    if( state->modeltype==0 )
    {
        rcopyallocv(n, &state->tmpdeltas, &state->tpsmodel.deltas, _state);
    }
    if( state->modeltype==1 )
    {
        rsetallocm(n, n, 0.0, &state->lsamodel.w, _state);
        rsetallocm(n, n, 0.0, &state->lsamodel.invw, _state);
        for(i=0; i<=n-1; i++)
        {
            state->lsamodel.w.ptr.pp_double[i][i] = state->tmpdeltas.ptr.p_double[i];
            state->lsamodel.invw.ptr.pp_double[i][i] = (double)1/state->tmpdeltas.ptr.p_double[i];
        }
    }
    workingsetchanged = ae_true;
lbl_2:
    
    /*
     * 2PS.
     */
    if( state->modeltype==0 )
    {
        
        /*
         * The working set was already computed, rebuild a quadratic model of the target.
         *
         * Below we rely on the specific ordering of the working set
         * (the central point comes last, N additional ones come first).
         */
        ae_assert(state->cntnlc==0, "DFGM: integrity check 7614 failed", _state);
        ae_assert(state->wrksetsize==n+1, "DFGM: integrity check 0425 failed", _state);
        rsetallocv(n, 0.0, &state->tpsmodel.g, _state);
        rallocm(n, n, &state->tpsmodel.q, _state);
        rallocm(m, n, &state->tpsmodel.jac, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                state->tpsmodel.jac.ptr.pp_double[j][i] = (state->wrkset.ptr.pp_double[i][n+j]-state->wrkset.ptr.pp_double[n][n+j])/state->tpsmodel.deltas.ptr.p_double[i];
                state->tpsmodel.g.ptr.p_double[i] = state->tpsmodel.g.ptr.p_double[i]+state->wrkset.ptr.pp_double[n][n+j]*state->tpsmodel.jac.ptr.pp_double[j][i];
            }
        }
        rmatrixgemm(n, n, m, 1.0, &state->tpsmodel.jac, 0, 0, 1, &state->tpsmodel.jac, 0, 0, 0, 0.0, &state->tpsmodel.q, 0, 0, _state);
        
        /*
         * Done
         */
        result = ae_false;
        return result;
    }
    
    /*
     * DFO-LSA.
     */
    if( state->modeltype!=1 )
    {
        goto lbl_6;
    }
    
    /*
     * Improve geometry of the dataset, if needed
     */
    ae_assert(state->wrksetsize==n+1, "DFGM: integrity check 9726 failed", _state);
    improvedfar = 0;
    
    /*
     * Repeat improvement rounds until there is no point selected for improvement.
     */
lbl_8:
    
    /*
     * Select a point to improve:
     * * improve location of all points that are too close
     * * improve location of a limited number of points that are too far
     */
    dfgenmod_mostandleastdistant(&state->invtrustregion, n, &state->wrkset, state->wrksetsize, &mostdistant, &mostdistantidx, &leastdistant, &leastdistantidx, _state);
    improveidx = -1;
    if( leastdistantidx>=0&&ae_fp_less_eq(leastdistant,dfgenmod_pointtooclose*ae_minreal(mostdistant, 1.0, _state)) )
    {
        improveidx = leastdistantidx;
    }
    if( improveidx<0&&ae_fp_greater_eq(mostdistant,dfgenmod_pointunacceptablyfar) )
    {
        improveidx = mostdistantidx;
    }
    if( (improveidx<0&&ae_fp_less((double)(improvedfar),dfgenmod_lsaimprovementlimit))&&ae_fp_greater_eq(mostdistant,dfgenmod_pointtoofar) )
    {
        improvedfar = improvedfar+1;
        improveidx = mostdistantidx;
    }
    
    /*
     * Try to find a location for an improved point, handle possible infinities in the target
     * by decreasing the trust radius and recomputing the proposal.
     *
     * The loop below has the loop condition "ImproveIdx>=0", the intention is to execute
     * it continually (until we settle on an infinity-free trust region) when there is a
     * point to improve, but to skip it if there is no points to be improved
     */
    rallocv(n, &state->tmp0, _state);
    rallocv(n, &state->tmp1, _state);
    rallocv(n, &state->tmp2, _state);
lbl_11:
    if( improveidx<0 )
    {
        goto lbl_12;
    }
    
    /*
     * Insert new point to the location where Lagrange polynomial for ImproveIdx-th point
     * reaches its maximum absolute value.
     *
     * Lagrange polynomial for this problem is a linear function with coefficients stored
     * in the ImproveIdx-th column of State.lsaModel.InvW (the constant term is zero). We
     * solve the problem by finding min L(x) and max L(x) subject to trust radius constraints,
     * and then selecting a solution with maximum absolute value.
     *
     * NOTE: C0 parameter of MaxAbsLag() is zero because we always have ImproveIdx<N, which
     *       corresponds to a Lagrange function which is zero at the origin
     */
    ae_assert(improveidx<n, "DFGM: integrity check 9820 failed", _state);
    rcopycv(n, &state->lsamodel.invw, improveidx, &state->tmp2, _state);
    dfgenmod_maxabslag(&state->xk, n, &state->trustregion, &state->finitebndl, &state->finitebndu, &state->tmp2, 0.0, &state->tmp0, &state->tmp1, ae_true, _state);
    rcopyvr(n, &state->tmp0, &state->wrkset, improveidx, _state);
    
    /*
     * Issue RComm V2 request, check that the reply does not contain infinities
     */
    state->requesttype = 4;
    state->querysize = 1;
    rallocv(n*state->querysize, &state->querydata, _state);
    rallocv((m+cntnlc)*state->querysize, &state->replyfi, _state);
    for(j=0; j<=n-1; j++)
    {
        state->querydata.ptr.p_double[j] = state->wrkset.ptr.pp_double[improveidx][j];
    }
    state->rstateimprove.stage = 1;
    goto lbl_rcomm;
lbl_1:
    if( !isfinitevector(&state->replyfi, (m+cntnlc)*state->querysize, _state) )
    {
        
        /*
         * Decrease trust radius and retry
         */
        if( state->dotrace )
        {
            ae_trace("[WARNING] target at the geometry-improving point is infinite, decreasing trust radius and restarting iteration\n");
        }
        state->infinitiesencountered = ae_true;
        state->recoveredfrominfinities = ae_false;
        dfgenmod_updatetrustregion(state, dfgenmod_gammabad*state->trustradfactor, _state);
        state->trustradbnd = ae_minreal(state->trustradfactor, state->trustradbnd, _state);
        if( ae_fp_less(state->trustradfactor,coalesce(state->epsx, state->toosmalltrustrad, _state)) )
        {
            state->repterminationtype = -8;
            result = ae_false;
            return result;
        }
        goto lbl_11;
    }
    
    /*
     * Accept the reply
     */
    for(j=0; j<=m+cntnlc-1; j++)
    {
        state->wrkset.ptr.pp_double[improveidx][n+j] = state->replyfi.ptr.p_double[j];
    }
    workingsetchanged = ae_true;
    goto lbl_12;
    goto lbl_11;
lbl_12:
    
    /*
     * Quick exit: no changes in the working set at this point means that
     *             the working set will never change. We can exit now.
     */
    if( !workingsetchanged )
    {
        result = ae_false;
        return result;
    }
    
    /*
     * Recompute matrices W and InvW, if there was an improvement in the
     * working set (this phase is ignored if WorkingSetChanged=True,
     * but ImproveIdx<0, which happens during the initialization)
     */
    if( improveidx>=0 )
    {
        rcopyrr(n, &state->wrkset, improveidx, &state->lsamodel.w, improveidx, _state);
        raddvr(n, -1.0, &state->xk, &state->lsamodel.w, improveidx, _state);
        rcopym(n, n, &state->lsamodel.w, &state->lsamodel.invw, _state);
        rmatrixinverse(&state->lsamodel.invw, n, &state->invrep, _state);
        if( state->invrep.terminationtype<=0 )
        {
            state->repterminationtype = 7;
            result = ae_false;
            return result;
        }
    }
    if( improveidx>=0 )
    {
        goto lbl_8;
    }
    
    /*
     * The working set is ready, rebuild a quadratic model of the target.
     *
     * Below we rely on the specific ordering of the working set
     * (the central point comes last, N additional ones come first).
     */
    ae_assert(state->wrksetsize==n+1, "DFGM: integrity check 0425 failed", _state);
    rallocm(n, m, &state->lsamodel.tmprhs, _state);
    rmatrixcopy(n, m, &state->wrkset, 0, n, &state->lsamodel.tmprhs, 0, 0, _state);
    rallocv(m, &state->lsamodel.tmp0, _state);
    for(i=0; i<=m-1; i++)
    {
        state->lsamodel.tmp0.ptr.p_double[i] = state->wrkset.ptr.pp_double[n][n+i];
    }
    for(i=0; i<=n-1; i++)
    {
        raddvr(m, -1.0, &state->lsamodel.tmp0, &state->lsamodel.tmprhs, i, _state);
    }
    rallocm(m, n, &state->lsamodel.jac, _state);
    rmatrixgemm(m, n, n, 1.0, &state->lsamodel.tmprhs, 0, 0, 1, &state->lsamodel.invw, 0, 0, 1, 0.0, &state->lsamodel.jac, 0, 0, _state);
    rsetallocv(n, 0.0, &state->lsamodel.g, _state);
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            state->lsamodel.g.ptr.p_double[i] = state->lsamodel.g.ptr.p_double[i]+state->wrkset.ptr.pp_double[n][n+j]*state->lsamodel.jac.ptr.pp_double[j][i];
        }
    }
    rallocm(n, n, &state->lsamodel.q, _state);
    rmatrixgemm(n, n, m, 1.0, &state->lsamodel.jac, 0, 0, 1, &state->lsamodel.jac, 0, 0, 0, 0.0, &state->lsamodel.q, 0, 0, _state);
    
    /*
     * Done
     */
    result = ae_false;
    return result;
lbl_6:
    ae_assert(ae_false, "DFGM: ImproveWorkingSetGeometry() is unable to handle unexpected algorithm", _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstateimprove.ia.ptr.p_int[0] = n;
    state->rstateimprove.ia.ptr.p_int[1] = m;
    state->rstateimprove.ia.ptr.p_int[2] = cntlc;
    state->rstateimprove.ia.ptr.p_int[3] = cntnlc;
    state->rstateimprove.ia.ptr.p_int[4] = i;
    state->rstateimprove.ia.ptr.p_int[5] = j;
    state->rstateimprove.ia.ptr.p_int[6] = improveidx;
    state->rstateimprove.ia.ptr.p_int[7] = improvedfar;
    state->rstateimprove.ia.ptr.p_int[8] = leastdistantidx;
    state->rstateimprove.ia.ptr.p_int[9] = mostdistantidx;
    state->rstateimprove.ba.ptr.p_bool[0] = workingsetchanged;
    state->rstateimprove.ra.ptr.p_double[0] = vlft;
    state->rstateimprove.ra.ptr.p_double[1] = vrgt;
    state->rstateimprove.ra.ptr.p_double[2] = v;
    state->rstateimprove.ra.ptr.p_double[3] = vmin;
    state->rstateimprove.ra.ptr.p_double[4] = vmax;
    state->rstateimprove.ra.ptr.p_double[5] = x0i;
    state->rstateimprove.ra.ptr.p_double[6] = leastdistant;
    state->rstateimprove.ra.ptr.p_double[7] = mostdistant;
    state->rstateimprove.ra.ptr.p_double[8] = mx;
    return result;
}


/*************************************************************************
Compute maximum absolute value of  a  Lagrange  polynomial  given  by  its
coefficients vector C[] and value at the trust region center C0 over trust
region.

Returns maximum absolute value and, optionally, point  where  the  maximum
is obtained.

if NeedX is True, then X must be preallocated array[N], and tmpX must also
be preallocated array[N]. If NeedX is False, X and tmpX are not referenced.

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static double dfgenmod_maxabslag(/* Real    */ const ae_vector* xk,
     ae_int_t n,
     /* Real    */ const ae_vector* trustregion,
     /* Real    */ const ae_vector* finitebndl,
     /* Real    */ const ae_vector* finitebndu,
     /* Real    */ const ae_vector* c,
     double c0,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* tmpx,
     ae_bool needx,
     ae_state *_state)
{
    double vmin;
    double vmax;
    double vlft;
    double vrgt;
    double xmin;
    double xmax;
    double xki;
    double ci;
    ae_int_t i;
    double result;


    ae_assert(!needx||x->cnt>=n, "DFGM: integrity check 5616 failed", _state);
    ae_assert(!needx||tmpx->cnt>=n, "DFGM: integrity check 5716 failed", _state);
    vmin = c0;
    vmax = c0;
    for(i=0; i<=n-1; i++)
    {
        xki = xk->ptr.p_double[i];
        ci = c->ptr.p_double[i];
        vlft = ae_maxreal(xki-trustregion->ptr.p_double[i], finitebndl->ptr.p_double[i], _state);
        vrgt = ae_minreal(xki+trustregion->ptr.p_double[i], finitebndu->ptr.p_double[i], _state);
        xmin = rcase2(ae_fp_greater_eq(ci,(double)(0)), vlft, vrgt, _state);
        xmax = rcase2(ae_fp_less_eq(ci,(double)(0)), vlft, vrgt, _state);
        vmin = vmin+(xmin-xki)*ci;
        vmax = vmax+(xmax-xki)*ci;
        if( needx )
        {
            x->ptr.p_double[i] = xmin;
            tmpx->ptr.p_double[i] = xmax;
        }
    }
    result = ae_maxreal(ae_fabs(vmin, _state), ae_fabs(vmax, _state), _state);
    if( needx&&ae_fp_less(ae_fabs(vmin, _state),ae_fabs(vmax, _state)) )
    {
        rcopyv(n, tmpx, x, _state);
    }
    return result;
}


/*************************************************************************
Compute distance to the most distant and the least distant neighbors in the
working set. The distance is computed using L-inf metric, per-variable
distances are divided by min(DeltaK,BndU[I]-BndL[I]).

  -- ALGLIB --
     Copyright 15.10.2023 by Bochkanov Sergey
*************************************************************************/
static void dfgenmod_mostandleastdistant(/* Real    */ const ae_vector* invtrustregion,
     ae_int_t n,
     /* Real    */ const ae_matrix* wrkset,
     ae_int_t wrksetsize,
     double* mostdistant,
     ae_int_t* mostdistantidx,
     double* leastdistant,
     ae_int_t* leastdistantidx,
     ae_state *_state)
{
    double mx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t centralidx;

    *mostdistant = 0.0;
    *mostdistantidx = 0;
    *leastdistant = 0.0;
    *leastdistantidx = 0;

    
    /*
     * Find most distant and least distant neighbors, 
     */
    *mostdistant = 0.0;
    *mostdistantidx = -1;
    *leastdistant = 1.0E50;
    *leastdistantidx = -1;
    centralidx = wrksetsize-1;
    for(i=0; i<=wrksetsize-2; i++)
    {
        mx = (double)(0);
        for(j=0; j<=n-1; j++)
        {
            mx = ae_maxreal(mx, ae_fabs(wrkset->ptr.pp_double[i][j]-wrkset->ptr.pp_double[centralidx][j], _state)*invtrustregion->ptr.p_double[j], _state);
        }
        if( *mostdistantidx<0||ae_fp_greater(mx,*mostdistant) )
        {
            *mostdistant = mx;
            *mostdistantidx = i;
        }
        if( *leastdistantidx<0||ae_fp_less(mx,*leastdistant) )
        {
            *leastdistant = mx;
            *leastdistantidx = i;
        }
    }
}


void _df2psmodel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    df2psmodel *p = (df2psmodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->deltas, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->jac, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->q, 0, 0, DT_REAL, _state, make_automatic);
}


void _df2psmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    df2psmodel       *dst = (df2psmodel*)_dst;
    const df2psmodel *src = (const df2psmodel*)_src;
    ae_vector_init_copy(&dst->deltas, &src->deltas, _state, make_automatic);
    ae_matrix_init_copy(&dst->jac, &src->jac, _state, make_automatic);
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    ae_matrix_init_copy(&dst->q, &src->q, _state, make_automatic);
}


void _df2psmodel_clear(void* _p)
{
    df2psmodel *p = (df2psmodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->deltas);
    ae_matrix_clear(&p->jac);
    ae_vector_clear(&p->g);
    ae_matrix_clear(&p->q);
}


void _df2psmodel_destroy(void* _p)
{
    df2psmodel *p = (df2psmodel*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->deltas);
    ae_matrix_destroy(&p->jac);
    ae_vector_destroy(&p->g);
    ae_matrix_destroy(&p->q);
}


void _dfolsamodel_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dfolsamodel *p = (dfolsamodel*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->w, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->invw, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->jac, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->q, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmprhs, 0, 0, DT_REAL, _state, make_automatic);
}


void _dfolsamodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dfolsamodel       *dst = (dfolsamodel*)_dst;
    const dfolsamodel *src = (const dfolsamodel*)_src;
    ae_matrix_init_copy(&dst->w, &src->w, _state, make_automatic);
    ae_matrix_init_copy(&dst->invw, &src->invw, _state, make_automatic);
    ae_matrix_init_copy(&dst->jac, &src->jac, _state, make_automatic);
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    ae_matrix_init_copy(&dst->q, &src->q, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmprhs, &src->tmprhs, _state, make_automatic);
}


void _dfolsamodel_clear(void* _p)
{
    dfolsamodel *p = (dfolsamodel*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->w);
    ae_matrix_clear(&p->invw);
    ae_matrix_clear(&p->jac);
    ae_vector_clear(&p->g);
    ae_matrix_clear(&p->q);
    ae_vector_clear(&p->tmp0);
    ae_matrix_clear(&p->tmprhs);
}


void _dfolsamodel_destroy(void* _p)
{
    dfolsamodel *p = (dfolsamodel*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->w);
    ae_matrix_destroy(&p->invw);
    ae_matrix_destroy(&p->jac);
    ae_vector_destroy(&p->g);
    ae_matrix_destroy(&p->q);
    ae_vector_destroy(&p->tmp0);
    ae_matrix_destroy(&p->tmprhs);
}


void _dfgmstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dfgmstate *p = (dfgmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->scaledbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->scaledbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->finitebndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->finitebndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->reportx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->querydata, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replyfi, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    _rcommstate_init(&p->rstateimprove, _state, make_automatic);
    _rcommstate_init(&p->rstateupdate, _state, make_automatic);
    ae_vector_init(&p->trustregion, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->invtrustregion, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xp, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fpvec, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sk, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xn, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fkvec, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fnvec, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->wrkset, 0, 0, DT_REAL, _state, make_automatic);
    _hqrndstate_init(&p->rs, _state, make_automatic);
    _df2psmodel_init(&p->tpsmodel, _state, make_automatic);
    _dfolsamodel_init(&p->lsamodel, _state, make_automatic);
    ae_matrix_init(&p->lastjac, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->trustradhistory, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->jacdiffhistory, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->iteridxhistory, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpdeltas, 0, DT_REAL, _state, make_automatic);
    _vipmstate_init(&p->ipmsolver, _state, make_automatic);
    ae_matrix_init(&p->densedummy, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsedummy, _state, make_automatic);
    _matinvreport_init(&p->invrep, _state, make_automatic);
}


void _dfgmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dfgmstate       *dst = (dfgmstate*)_dst;
    const dfgmstate *src = (const dfgmstate*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->cntlc = src->cntlc;
    dst->cntnlc = src->cntnlc;
    dst->isleastsquares = src->isleastsquares;
    dst->modeltype = src->modeltype;
    dst->rad0 = src->rad0;
    dst->nnoisyrestarts = src->nnoisyrestarts;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndl, &src->scaledbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndu, &src->scaledbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->finitebndl, &src->finitebndl, _state, make_automatic);
    ae_vector_init_copy(&dst->finitebndu, &src->finitebndu, _state, make_automatic);
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->maxfev = src->maxfev;
    dst->toosmalltrustrad = src->toosmalltrustrad;
    dst->requesttype = src->requesttype;
    ae_vector_init_copy(&dst->reportx, &src->reportx, _state, make_automatic);
    dst->reportf = src->reportf;
    dst->querysize = src->querysize;
    ae_vector_init_copy(&dst->querydata, &src->querydata, _state, make_automatic);
    ae_vector_init_copy(&dst->replyfi, &src->replyfi, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstateimprove, &src->rstateimprove, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstateupdate, &src->rstateupdate, _state, make_automatic);
    dst->dotrace = src->dotrace;
    ae_vector_init_copy(&dst->trustregion, &src->trustregion, _state, make_automatic);
    ae_vector_init_copy(&dst->invtrustregion, &src->invtrustregion, _state, make_automatic);
    dst->trustradfactor = src->trustradfactor;
    dst->trustradbnd = src->trustradbnd;
    ae_vector_init_copy(&dst->xp, &src->xp, _state, make_automatic);
    ae_vector_init_copy(&dst->fpvec, &src->fpvec, _state, make_automatic);
    dst->fp = src->fp;
    dst->hp = src->hp;
    ae_vector_init_copy(&dst->xk, &src->xk, _state, make_automatic);
    ae_vector_init_copy(&dst->sk, &src->sk, _state, make_automatic);
    ae_vector_init_copy(&dst->xn, &src->xn, _state, make_automatic);
    ae_vector_init_copy(&dst->fkvec, &src->fkvec, _state, make_automatic);
    ae_vector_init_copy(&dst->fnvec, &src->fnvec, _state, make_automatic);
    dst->fk = src->fk;
    dst->hk = src->hk;
    ae_matrix_init_copy(&dst->wrkset, &src->wrkset, _state, make_automatic);
    dst->wrksetsize = src->wrksetsize;
    _hqrndstate_init_copy(&dst->rs, &src->rs, _state, make_automatic);
    _df2psmodel_init_copy(&dst->tpsmodel, &src->tpsmodel, _state, make_automatic);
    _dfolsamodel_init_copy(&dst->lsamodel, &src->lsamodel, _state, make_automatic);
    dst->infinitiesencountered = src->infinitiesencountered;
    dst->recoveredfrominfinities = src->recoveredfrominfinities;
    dst->restartstoperform = src->restartstoperform;
    dst->lasttrustrad = src->lasttrustrad;
    ae_matrix_init_copy(&dst->lastjac, &src->lastjac, _state, make_automatic);
    ae_vector_init_copy(&dst->trustradhistory, &src->trustradhistory, _state, make_automatic);
    ae_vector_init_copy(&dst->jacdiffhistory, &src->jacdiffhistory, _state, make_automatic);
    ae_vector_init_copy(&dst->iteridxhistory, &src->iteridxhistory, _state, make_automatic);
    dst->historylen = src->historylen;
    dst->historynext = src->historynext;
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpdeltas, &src->tmpdeltas, _state, make_automatic);
    _vipmstate_init_copy(&dst->ipmsolver, &src->ipmsolver, _state, make_automatic);
    ae_matrix_init_copy(&dst->densedummy, &src->densedummy, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsedummy, &src->sparsedummy, _state, make_automatic);
    _matinvreport_init_copy(&dst->invrep, &src->invrep, _state, make_automatic);
    dst->repterminationtype = src->repterminationtype;
    dst->repiterationscount = src->repiterationscount;
    dst->repnfev = src->repnfev;
}


void _dfgmstate_clear(void* _p)
{
    dfgmstate *p = (dfgmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->scaledbndl);
    ae_vector_clear(&p->scaledbndu);
    ae_vector_clear(&p->finitebndl);
    ae_vector_clear(&p->finitebndu);
    ae_vector_clear(&p->reportx);
    ae_vector_clear(&p->querydata);
    ae_vector_clear(&p->replyfi);
    _rcommstate_clear(&p->rstate);
    _rcommstate_clear(&p->rstateimprove);
    _rcommstate_clear(&p->rstateupdate);
    ae_vector_clear(&p->trustregion);
    ae_vector_clear(&p->invtrustregion);
    ae_vector_clear(&p->xp);
    ae_vector_clear(&p->fpvec);
    ae_vector_clear(&p->xk);
    ae_vector_clear(&p->sk);
    ae_vector_clear(&p->xn);
    ae_vector_clear(&p->fkvec);
    ae_vector_clear(&p->fnvec);
    ae_matrix_clear(&p->wrkset);
    _hqrndstate_clear(&p->rs);
    _df2psmodel_clear(&p->tpsmodel);
    _dfolsamodel_clear(&p->lsamodel);
    ae_matrix_clear(&p->lastjac);
    ae_vector_clear(&p->trustradhistory);
    ae_vector_clear(&p->jacdiffhistory);
    ae_vector_clear(&p->iteridxhistory);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmp2);
    ae_vector_clear(&p->tmpdeltas);
    _vipmstate_clear(&p->ipmsolver);
    ae_matrix_clear(&p->densedummy);
    _sparsematrix_clear(&p->sparsedummy);
    _matinvreport_clear(&p->invrep);
}


void _dfgmstate_destroy(void* _p)
{
    dfgmstate *p = (dfgmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->scaledbndl);
    ae_vector_destroy(&p->scaledbndu);
    ae_vector_destroy(&p->finitebndl);
    ae_vector_destroy(&p->finitebndu);
    ae_vector_destroy(&p->reportx);
    ae_vector_destroy(&p->querydata);
    ae_vector_destroy(&p->replyfi);
    _rcommstate_destroy(&p->rstate);
    _rcommstate_destroy(&p->rstateimprove);
    _rcommstate_destroy(&p->rstateupdate);
    ae_vector_destroy(&p->trustregion);
    ae_vector_destroy(&p->invtrustregion);
    ae_vector_destroy(&p->xp);
    ae_vector_destroy(&p->fpvec);
    ae_vector_destroy(&p->xk);
    ae_vector_destroy(&p->sk);
    ae_vector_destroy(&p->xn);
    ae_vector_destroy(&p->fkvec);
    ae_vector_destroy(&p->fnvec);
    ae_matrix_destroy(&p->wrkset);
    _hqrndstate_destroy(&p->rs);
    _df2psmodel_destroy(&p->tpsmodel);
    _dfolsamodel_destroy(&p->lsamodel);
    ae_matrix_destroy(&p->lastjac);
    ae_vector_destroy(&p->trustradhistory);
    ae_vector_destroy(&p->jacdiffhistory);
    ae_vector_destroy(&p->iteridxhistory);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmp2);
    ae_vector_destroy(&p->tmpdeltas);
    _vipmstate_destroy(&p->ipmsolver);
    ae_matrix_destroy(&p->densedummy);
    _sparsematrix_destroy(&p->sparsedummy);
    _matinvreport_destroy(&p->invrep);
}


/*$ End $*/
