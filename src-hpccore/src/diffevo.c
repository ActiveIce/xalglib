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
#include "diffevo.h"


/*$ Declarations $*/
static double diffevo_defaultpenalty = 100;
static ae_int_t diffevo_paramscnt = 4;
static ae_int_t diffevo_strategiescnt = 2;
static double diffevo_randomparamprob = 0.50;
static void diffevo_computefitness(const gdemostate* state,
     /* Real    */ const ae_matrix* population2rawreplies,
     ae_int_t popsize,
     /* Real    */ const ae_matrix* newpopulation,
     /* Real    */ const ae_matrix* rawreplies,
     ae_int_t batchsize,
     /* Real    */ ae_matrix* fitness,
     ae_state *_state);
static void diffevo_updatebest(gdemostate* state, ae_state *_state);
static void diffevo_generatecandidates(gdemostate* state,
     ae_state *_state);
static void diffevo_updatepopulation(gdemostate* state, ae_state *_state);
static ae_int_t diffevo_randomstrategy(gdemostate* s, ae_state *_state);
static double diffevo_randomcrossoverprob(gdemostate* s, ae_state *_state);
static double diffevo_randomdifferentialweight(gdemostate* s,
     ae_state *_state);
static void diffevo_istreaminit(gdemointstream* s,
     ae_int_t maxval,
     ae_int_t maxsize,
     ae_state *_state);
static void diffevo_rstreaminit(gdemorealstream* s,
     ae_int_t maxsize,
     ae_state *_state);
static void diffevo_rstreamrestart(gdemorealstream* s,
     hqrndstate* rs,
     ae_state *_state);
static double diffevo_rstreamfetch(gdemorealstream* s,
     hqrndstate* rs,
     ae_state *_state);


/*$ Body $*/


void gdemoinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t cntnlc,
     ae_int_t popsize,
     ae_int_t epochscnt,
     ae_int_t seed,
     gdemostate* state,
     ae_state *_state)
{
    ae_int_t i;


    
    /*
     * Integrity checks
     */
    ae_assert(m==1, "GDEMO: M<>1", _state);
    ae_assert(popsize>=0, "GDEMO: PopSize<=0", _state);
    ae_assert(epochscnt>0, "GDEMO: EpochsCnt<=0", _state);
    
    /*
     * Initialization
     */
    popsize = ae_maxint(coalescei(popsize, 10*n, _state), 10, _state);
    state->n = n;
    state->m = m;
    state->cntlc = cntlc;
    state->cntnlc = cntnlc;
    state->popsize = popsize;
    state->epochscnt = epochscnt;
    state->crossoverprob = 0.9;
    state->diffweight = 0.5;
    state->constrmode = 0;
    state->rho1 = diffevo_defaultpenalty;
    state->rho2 = diffevo_defaultpenalty;
    state->eps = 0.0;
    state->nsample = (double)(1);
    state->paramspoolsize = ae_maxint(5*popsize, 3000, _state);
    state->fixedparams = ae_false;
    state->hasx0 = ae_false;
    state->stoponsmallf = ae_false;
    hqrndseed(seed, seed+1172, &state->rs, _state);
    rsetallocm(m, n, 0.0, &state->xbest, _state);
    rsetallocv(m, _state->v_nan, &state->fbest, _state);
    
    /*
     * Prepare RCOMM state
     */
    ae_vector_set_length(&state->rstate.ia, 7+1, _state);
    ae_vector_set_length(&state->rstate.ba, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    
    /*
     * Prepare scaled problem
     */
    rallocv(n, &state->s, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    rsetallocv(n, -ae_maxrealnumber, &state->finitebndl, _state);
    rsetallocv(n, ae_maxrealnumber, &state->finitebndu, _state);
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
            ae_assert(ae_fp_less_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]), "GDEMO: integrity check failed, box constraints are inconsistent", _state);
        }
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
Set fixed algorithm params (the default is to use adaptive algorithm)

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
void gdemosetfixedparams(gdemostate* state,
     ae_int_t strategy,
     double crossoverprob,
     double differentialweight,
     ae_state *_state)
{


    state->fixedparams = ae_true;
    state->fixedstrategy = strategy;
    state->fixedcrossoverprob = crossoverprob;
    state->fixeddifferentialweight = differentialweight;
}


/*************************************************************************
Sets small F, the solver stops when the best value decreases below this level

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
void gdemosetsmallf(gdemostate* state, double f, ae_state *_state)
{


    state->stoponsmallf = ae_true;
    state->smallf = f;
}


/*************************************************************************
Sets constraint handling mode to L1/L2 penalty

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
void gdemosetmodepenalty(gdemostate* state,
     double rhol1,
     double rhol2,
     ae_state *_state)
{


    state->constrmode = 0;
    state->rho1 = rhol1;
    state->rho2 = rhol2;
}


/*************************************************************************
Sets initial point

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
void gdemosetx0(gdemostate* state,
     /* Real    */ const ae_vector* x0,
     ae_state *_state)
{


    state->hasx0 = ae_true;
    rcopyallocv(state->n, x0, &state->x0, _state);
}


/*************************************************************************
This function performs actual processing for GDEMO  algorithm.

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool gdemoiteration(gdemostate* state,
     ae_bool userterminationneeded,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vleft;
    double vright;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_int_t popsize;
    ae_int_t iteridx;
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
        m = state->rstate.ia.ptr.p_int[1];
        cntlc = state->rstate.ia.ptr.p_int[2];
        cntnlc = state->rstate.ia.ptr.p_int[3];
        i = state->rstate.ia.ptr.p_int[4];
        j = state->rstate.ia.ptr.p_int[5];
        popsize = state->rstate.ia.ptr.p_int[6];
        iteridx = state->rstate.ia.ptr.p_int[7];
        dotrace = state->rstate.ba.ptr.p_bool[0];
        dodetailedtrace = state->rstate.ba.ptr.p_bool[1];
        v = state->rstate.ra.ptr.p_double[0];
        vleft = state->rstate.ra.ptr.p_double[1];
        vright = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        n = 359;
        m = -58;
        cntlc = -919;
        cntnlc = -909;
        i = 81;
        j = 255;
        popsize = 74;
        iteridx = -788;
        dotrace = ae_true;
        dodetailedtrace = ae_true;
        v = -838.0;
        vleft = 939.0;
        vright = -526.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
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
    popsize = state->popsize;
    dotrace = ae_is_trace_enabled("GDEMO");
    dodetailedtrace = dotrace&&ae_is_trace_enabled("GDEMO.DETAILED");
    ae_assert(m==1, "GDEMO: integrity check 0732 failed", _state);
    
    /*
     * Init
     */
    state->repiterationscount = 0;
    state->repterminationtype = 5;
    state->replcerr = 0.0;
    state->replcidx = -1;
    state->repnlcerr = 0.0;
    state->repnlcidx = -1;
    rallocv(n, &state->reportx, _state);
    
    /*
     * Generate initial population and compute fitness
     */
    rallocm(2*popsize, n, &state->population2x, _state);
    rallocm(2*popsize, m+cntnlc, &state->population2rawreplies, _state);
    rallocm(2*popsize, m, &state->population2fitness, _state);
    if( !state->fixedparams )
    {
        rsetallocm(2*popsize, diffevo_paramscnt, 0.0, &state->population2params, _state);
    }
    for(i=0; i<=popsize-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            
            /*
             * Generate population individual using hints from box constraints
             */
            if( state->hasbndl.ptr.p_bool[j]&&state->hasbndu.ptr.p_bool[j] )
            {
                state->population2x.ptr.pp_double[i][j] = state->finitebndl.ptr.p_double[j]+(state->finitebndu.ptr.p_double[j]-state->finitebndl.ptr.p_double[j])*hqrnduniformr(&state->rs, _state);
                continue;
            }
            if( state->hasbndl.ptr.p_bool[j]&&!state->hasbndu.ptr.p_bool[j] )
            {
                state->population2x.ptr.pp_double[i][j] = state->finitebndl.ptr.p_double[j]+ae_pow((double)(2), hqrndnormal(&state->rs, _state), _state);
                continue;
            }
            if( state->hasbndu.ptr.p_bool[j]&&!state->hasbndl.ptr.p_bool[j] )
            {
                state->population2x.ptr.pp_double[i][j] = state->finitebndu.ptr.p_double[j]-ae_pow((double)(2), hqrndnormal(&state->rs, _state), _state);
                continue;
            }
            state->population2x.ptr.pp_double[i][j] = hqrndnormal(&state->rs, _state);
        }
        if( !state->fixedparams )
        {
            ae_assert(diffevo_paramscnt==4, "GDEMO: integrity check 6758 failed", _state);
            state->population2params.ptr.pp_double[i][0] = (double)(diffevo_randomstrategy(state, _state));
            state->population2params.ptr.pp_double[i][1] = diffevo_randomcrossoverprob(state, _state);
            state->population2params.ptr.pp_double[i][2] = diffevo_randomdifferentialweight(state, _state);
            state->population2params.ptr.pp_double[i][3] = diffevo_randomdifferentialweight(state, _state);
        }
    }
    state->requesttype = 4;
    state->querysize = popsize;
    rallocv(n*popsize, &state->querydata, _state);
    for(i=0; i<=popsize-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            state->querydata.ptr.p_double[i*n+j] = state->population2x.ptr.pp_double[i][j];
        }
    }
    rallocv(popsize*(m+cntnlc), &state->replyfi, _state);
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    for(i=0; i<=popsize-1; i++)
    {
        for(j=0; j<=m+cntnlc-1; j++)
        {
            state->population2rawreplies.ptr.pp_double[i][j] = state->replyfi.ptr.p_double[i*(m+cntnlc)+j];
        }
    }
    diffevo_computefitness(state, &state->population2rawreplies, 0, &state->population2x, &state->population2rawreplies, popsize, &state->population2fitness, _state);
    diffevo_updatebest(state, _state);
    
    /*
     * Initialize parameters pool
     */
    if( !state->fixedparams )
    {
        rallocm(state->paramspoolsize, diffevo_paramscnt, &state->paramspool, _state);
        for(i=0; i<=state->paramspoolsize-1; i++)
        {
            state->paramspool.ptr.pp_double[i][0] = (double)(diffevo_randomstrategy(state, _state));
            state->paramspool.ptr.pp_double[i][1] = diffevo_randomcrossoverprob(state, _state);
            state->paramspool.ptr.pp_double[i][2] = diffevo_randomdifferentialweight(state, _state);
        }
        state->poolinsertpos = 0;
    }
    
    /*
     * Run iterations
     */
    rallocm(popsize, n, &state->candidatesx, _state);
    rallocm(popsize, m+cntnlc, &state->candidatesrawreplies, _state);
    rallocm(popsize, m, &state->candidatesfitness, _state);
    if( !state->fixedparams )
    {
        rallocm(popsize, diffevo_paramscnt, &state->candidatesparams, _state);
    }
    iteridx = 0;
lbl_2:
    if( iteridx>state->epochscnt-1 )
    {
        goto lbl_4;
    }
    
    /*
     * Generate candidates
     */
    diffevo_generatecandidates(state, _state);
    
    /*
     * RComm request and fitness computation
     */
    state->requesttype = 4;
    state->querysize = popsize;
    rallocv(n*popsize, &state->querydata, _state);
    for(i=0; i<=popsize-1; i++)
    {
        for(j=0; j<=n-1; j++)
        {
            state->querydata.ptr.p_double[i*n+j] = state->candidatesx.ptr.pp_double[i][j];
        }
    }
    rallocv(popsize*(m+cntnlc), &state->replyfi, _state);
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    for(i=0; i<=popsize-1; i++)
    {
        for(j=0; j<=m+cntnlc-1; j++)
        {
            state->candidatesrawreplies.ptr.pp_double[i][j] = state->replyfi.ptr.p_double[i*(m+cntnlc)+j];
        }
    }
    diffevo_computefitness(state, &state->population2rawreplies, popsize, &state->candidatesx, &state->candidatesrawreplies, popsize, &state->candidatesfitness, _state);
    
    /*
     * Update population with candidates
     */
    diffevo_updatepopulation(state, _state);
    
    /*
     * Update best value so far and other counters, check stopping criteria
     */
    diffevo_updatebest(state, _state);
    state->repiterationscount = state->repiterationscount+1;
    if( (state->stoponsmallf&&m==1)&&ae_fp_less_eq(state->fbest.ptr.p_double[0],state->smallf) )
    {
        state->repterminationtype = 6;
        goto lbl_4;
    }
    iteridx = iteridx+1;
    goto lbl_2;
lbl_4:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = cntlc;
    state->rstate.ia.ptr.p_int[3] = cntnlc;
    state->rstate.ia.ptr.p_int[4] = i;
    state->rstate.ia.ptr.p_int[5] = j;
    state->rstate.ia.ptr.p_int[6] = popsize;
    state->rstate.ia.ptr.p_int[7] = iteridx;
    state->rstate.ba.ptr.p_bool[0] = dotrace;
    state->rstate.ba.ptr.p_bool[1] = dodetailedtrace;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vleft;
    state->rstate.ra.ptr.p_double[2] = vright;
    return result;
}


/*************************************************************************
Compute fitness from raw replies

INPUT PARAMETERS:
    State                   -   solver
    Population2RawReplies   -   present population, used by fitness computation
                                methods which analyze distribution of constraint
                                violations in the current population
    PopSize                 -   population size, or zero, if this
                                function is called at the initialization
                                phase when we compute fitness for the initial
                                population
    RawReplies              -   raw replies to process
    BatchSize               -   size of the set being processed
    Fitness                 -   preallocated output array
    

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static void diffevo_computefitness(const gdemostate* state,
     /* Real    */ const ae_matrix* population2rawreplies,
     ae_int_t popsize,
     /* Real    */ const ae_matrix* newpopulation,
     /* Real    */ const ae_matrix* rawreplies,
     ae_int_t batchsize,
     /* Real    */ ae_matrix* fitness,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_int_t i;
    ae_int_t j;
    double p;
    double v;
    double vv;


    n = state->n;
    m = state->m;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    ae_assert(fitness->rows>=batchsize&&fitness->cols>=m, "GDEMO: integrity check 5831 failed", _state);
    ae_assert(ae_fp_eq(state->nsample,(double)(1)), "GDEMO: integrity check 5632 failed", _state);
    
    /*
     * Quick exit
     */
    if( cntlc+cntnlc==0 )
    {
        for(i=0; i<=batchsize-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                fitness->ptr.pp_double[i][j] = rawreplies->ptr.pp_double[i][j];
            }
        }
        return;
    }
    
    /*
     * Default constraint handling strategy: L1 penalty
     */
    if( state->constrmode==0 )
    {
        for(i=0; i<=batchsize-1; i++)
        {
            p = (double)(0);
            for(j=0; j<=cntlc-1; j++)
            {
                v = rdotrr(n, newpopulation, i, &state->densea, j, _state);
                if( state->hasal.ptr.p_bool[j] )
                {
                    vv = ae_maxreal(state->al.ptr.p_double[j]-v, 0.0, _state);
                    p = p+state->rho1*vv+state->rho2*vv*vv;
                }
                if( state->hasau.ptr.p_bool[j] )
                {
                    vv = ae_maxreal(v-state->au.ptr.p_double[j], 0.0, _state);
                    p = p+state->rho1*vv+state->rho2*vv*vv;
                }
            }
            for(j=0; j<=cntnlc-1; j++)
            {
                v = rawreplies->ptr.pp_double[i][m+j];
                if( state->hasnl.ptr.p_bool[j] )
                {
                    vv = ae_maxreal(state->rawnl.ptr.p_double[j]-v, 0.0, _state);
                    p = p+state->rho1*vv+state->rho2*vv*vv;
                }
                if( state->hasnu.ptr.p_bool[j] )
                {
                    vv = ae_maxreal(v-state->rawnu.ptr.p_double[j], 0.0, _state);
                    p = p+state->rho1*vv+state->rho2*vv*vv;
                }
            }
            for(j=0; j<=m-1; j++)
            {
                fitness->ptr.pp_double[i][j] = rawreplies->ptr.pp_double[i][j]+p;
            }
        }
        return;
    }
    
    /*
     * Unknown constrmode
     */
    ae_assert(ae_false, "GDEMO: integrity check 2202 failed", _state);
}


/*************************************************************************
Updates best value so far. For multiobjective models - fails.

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static void diffevo_updatebest(gdemostate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;


    n = state->n;
    m = state->m;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    ae_assert(ae_fp_eq(state->nsample,(double)(1)), "GDEMO: integrity check 9242 failed", _state);
    ae_assert(m==1, "GDEMO: integrity check 9243 failed", _state);
    for(i=0; i<=state->popsize-1; i++)
    {
        if( ae_isnan(state->fbest.ptr.p_double[0], _state)||ae_fp_less(state->population2fitness.ptr.pp_double[i][0],state->fbest.ptr.p_double[0]) )
        {
            
            /*
             * Save best value
             */
            rcopyrr(n, &state->population2x, i, &state->xbest, 0, _state);
            state->fbest.ptr.p_double[0] = state->population2fitness.ptr.pp_double[i][0];
            
            /*
             * Update errors
             */
            state->replcerr = 0.0;
            state->replcidx = -1;
            for(j=0; j<=cntlc-1; j++)
            {
                v = rdotrr(n, &state->xbest, 0, &state->densea, j, _state);
                if( state->hasal.ptr.p_bool[j] )
                {
                    vv = state->ascales.ptr.p_double[j]*ae_maxreal(state->al.ptr.p_double[j]-v, 0.0, _state);
                    if( ae_fp_greater(vv,state->replcerr) )
                    {
                        state->replcerr = vv;
                        state->replcidx = j;
                    }
                }
                if( state->hasau.ptr.p_bool[j] )
                {
                    vv = state->ascales.ptr.p_double[j]*ae_maxreal(v-state->au.ptr.p_double[j], 0.0, _state);
                    if( ae_fp_greater(vv,state->replcerr) )
                    {
                        state->replcerr = vv;
                        state->replcidx = j;
                    }
                }
            }
            state->repnlcerr = 0.0;
            state->repnlcidx = -1;
            for(j=0; j<=cntnlc-1; j++)
            {
                v = state->population2rawreplies.ptr.pp_double[i][m+j];
                if( state->hasnl.ptr.p_bool[j] )
                {
                    vv = ae_maxreal(state->rawnl.ptr.p_double[j]-v, 0.0, _state);
                    if( ae_fp_greater(vv,state->repnlcerr) )
                    {
                        state->repnlcerr = vv;
                        state->repnlcidx = j;
                    }
                }
                if( state->hasnu.ptr.p_bool[j] )
                {
                    vv = ae_maxreal(v-state->rawnu.ptr.p_double[j], 0.0, _state);
                    if( ae_fp_greater(vv,state->repnlcerr) )
                    {
                        state->repnlcerr = vv;
                        state->repnlcidx = j;
                    }
                }
            }
        }
    }
}


/*************************************************************************
This function generates candidates

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static void diffevo_generatecandidates(gdemostate* state,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t k2;
    ae_int_t k3;
    ae_int_t r;
    ae_bool nondistinct;
    ae_bool processed;
    ae_int_t strategytype;
    double crossoverprob;
    double differentialweight;
    double differentialweight2;
    ae_int_t popsize;
    ae_vector donor;

    ae_frame_make(_state, &_frame_block);
    memset(&donor, 0, sizeof(donor));
    ae_vector_init(&donor, 0, DT_REAL, _state, ae_true);

    n = state->n;
    m = state->m;
    popsize = state->popsize;
    ae_assert(m==1, "GDEMO: integrity check 4959 failed", _state);
    
    /*
     * Generate candidates
     */
    ae_assert(popsize>=10, "GDEMO: integrity check 7156 failed", _state);
    rallocv(n, &donor, _state);
    for(i=0; i<=popsize-1; i++)
    {
        
        /*
         * Inherit candidate parameters from the target or use fixed params
         */
        if( state->fixedparams )
        {
            strategytype = state->fixedstrategy;
            crossoverprob = state->fixedcrossoverprob;
            differentialweight = state->fixeddifferentialweight;
            differentialweight2 = state->fixeddifferentialweight;
        }
        else
        {
            rcopyrr(diffevo_paramscnt, &state->population2params, i, &state->candidatesparams, i, _state);
            strategytype = ae_round(state->candidatesparams.ptr.pp_double[i][0], _state);
            crossoverprob = state->candidatesparams.ptr.pp_double[i][1];
            differentialweight = state->candidatesparams.ptr.pp_double[i][2];
            differentialweight2 = state->candidatesparams.ptr.pp_double[i][3];
        }
        
        /*
         * Generate a set of 4 mutually distinct indexes
         */
        k0 = hqrnduniformi(&state->rs, popsize, _state);
        k1 = hqrnduniformi(&state->rs, popsize, _state);
        k2 = hqrnduniformi(&state->rs, popsize, _state);
        k3 = hqrnduniformi(&state->rs, popsize, _state);
        
        /*
         * Generate donor vector
         */
        processed = ae_false;
        if( strategytype==0 )
        {
            
            /*
             * DE/rand/1
             */
            rcopyrv(n, &state->population2x, k0, &donor, _state);
            raddrv(n, -1.0, &state->population2x, k1, &donor, _state);
            rmulv(n, differentialweight, &donor, _state);
            raddrv(n, 1.0, &state->population2x, k2, &donor, _state);
            processed = ae_true;
        }
        if( strategytype==1 )
        {
            
            /*
             * DE/best/2
             */
            rsetv(n, 0.0, &donor, _state);
            raddrv(n, 1.0, &state->population2x, k0, &donor, _state);
            raddrv(n, -1.0, &state->population2x, k1, &donor, _state);
            raddrv(n, 1.0, &state->population2x, k2, &donor, _state);
            raddrv(n, -1.0, &state->population2x, k3, &donor, _state);
            rmulv(n, differentialweight, &donor, _state);
            raddrv(n, 1.0, &state->xbest, hqrnduniformi(&state->rs, m, _state), &donor, _state);
            processed = ae_true;
        }
        if( strategytype==2 )
        {
            
            /*
             * DE/rand-to-best/1
             */
            rsetv(n, 0.0, &donor, _state);
            raddrv(n, differentialweight, &state->population2x, k0, &donor, _state);
            raddrv(n, -differentialweight, &state->population2x, k1, &donor, _state);
            raddrv(n, differentialweight2, &state->xbest, hqrnduniformi(&state->rs, m, _state), &donor, _state);
            raddrv(n, -differentialweight2, &state->population2x, i, &donor, _state);
            raddrv(n, 1.0, &state->population2x, k2, &donor, _state);
            processed = ae_true;
        }
        ae_assert(processed, "GDEMO: integrity check 7131 failed", _state);
        rmergemaxv(n, &state->finitebndl, &donor, _state);
        rmergeminv(n, &state->finitebndu, &donor, _state);
        
        /*
         * Perform crossover with the target
         */
        r = hqrnduniformi(&state->rs, n, _state);
        rcopyrr(n, &state->population2x, i, &state->candidatesx, i, _state);
        for(j=0; j<=n-1; j++)
        {
            if( hqrnduniformr(&state->rs, _state)<=crossoverprob||j==r )
            {
                state->candidatesx.ptr.pp_double[i][j] = donor.ptr.p_double[j];
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Promote candidates

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static void diffevo_updatepopulation(gdemostate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t cntnlc;
    ae_int_t i;


    n = state->n;
    m = state->m;
    cntnlc = state->cntnlc;
    
    /*
     * Perform competition.
     *
     * NOTE: it is important to compare fitness as follows: "if CandFitness<=PopFitness",
     *       using <= instead of <, because it promotes explorational behavior in areas
     *       with vanishing gradient.
     */
    ae_assert(ae_fp_eq(state->nsample,(double)(1)), "GDEMO: integrity check 2649 failed", _state);
    ae_assert(m==1, "GDEMO: integrity check 2650 failed", _state);
    for(i=0; i<=state->popsize-1; i++)
    {
        if( ae_fp_less_eq(state->candidatesfitness.ptr.pp_double[i][0],state->population2fitness.ptr.pp_double[i][0]) )
        {
            
            /*
             * The candidate wins! Copy candidate data
             */
            rcopyrr(n, &state->candidatesx, i, &state->population2x, i, _state);
            rcopyrr(m+cntnlc, &state->candidatesrawreplies, i, &state->population2rawreplies, i, _state);
            rcopyrr(m, &state->candidatesfitness, i, &state->population2fitness, i, _state);
            
            /*
             * Save algorithm parameters for the individual and add them to pool
             */
            if( !state->fixedparams )
            {
                rcopyrr(diffevo_paramscnt, &state->candidatesparams, i, &state->population2params, i, _state);
                rcopyrr(diffevo_paramscnt, &state->candidatesparams, i, &state->paramspool, state->poolinsertpos, _state);
                state->poolinsertpos = (state->poolinsertpos+1)%state->paramspoolsize;
            }
        }
        else
        {
            
            /*
             * The candidate does not win.
             *
             * Re-initialize individual parameters
             */
            if( !state->fixedparams )
            {
                ae_assert(diffevo_paramscnt==4, "GDEMO: integrity check 6759 failed", _state);
                if( ae_fp_less(hqrnduniformr(&state->rs, _state),diffevo_randomparamprob) )
                {
                    state->population2params.ptr.pp_double[i][0] = (double)(diffevo_randomstrategy(state, _state));
                    state->population2params.ptr.pp_double[i][1] = diffevo_randomcrossoverprob(state, _state);
                    state->population2params.ptr.pp_double[i][2] = diffevo_randomdifferentialweight(state, _state);
                    state->population2params.ptr.pp_double[i][3] = diffevo_randomdifferentialweight(state, _state);
                }
                else
                {
                    rcopyrr(diffevo_paramscnt, &state->paramspool, hqrnduniformi(&state->rs, state->paramspoolsize, _state), &state->population2params, i, _state);
                }
            }
        }
    }
}


/*************************************************************************
Random strategy, like DE/rand/1, DE/best/1, etc

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static ae_int_t diffevo_randomstrategy(gdemostate* s, ae_state *_state)
{
    ae_int_t result;


    result = hqrnduniformi(&s->rs, diffevo_strategiescnt, _state);
    return result;
}


/*************************************************************************
Crossover probability from [0.1,0.9]

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static double diffevo_randomcrossoverprob(gdemostate* s, ae_state *_state)
{
    double result;


    result = 0.1+0.1*(double)hqrnduniformi(&s->rs, 9, _state);
    return result;
}


/*************************************************************************
Crossover differential weight from [0.4,0.9]

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static double diffevo_randomdifferentialweight(gdemostate* s,
     ae_state *_state)
{
    double result;


    result = 0.4+0.1*(double)hqrnduniformi(&s->rs, 6, _state);
    return result;
}


/*************************************************************************
Initialize integer stream

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static void diffevo_istreaminit(gdemointstream* s,
     ae_int_t maxval,
     ae_int_t maxsize,
     ae_state *_state)
{


    s->streamsize = 0;
    s->streampos = 0;
    s->maxval = maxval;
    s->maxsize = maxsize;
}


/*************************************************************************
Initialize integer stream

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static void diffevo_rstreaminit(gdemorealstream* s,
     ae_int_t maxsize,
     ae_state *_state)
{


    s->streamsize = 0;
    s->streampos = 0;
    s->maxsize = maxsize;
}


/*************************************************************************
Restarts the stream, reshuffling internally stored values

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static void diffevo_rstreamrestart(gdemorealstream* s,
     hqrndstate* rs,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t n;


    if( s->streamsize==0 )
    {
        return;
    }
    
    /*
     * Reshuffle Vals0
     */
    k = hqrnduniformi(rs, ae_minint(s->streamsize, 25, _state), _state);
    n = s->streamsize-k;
    for(i=0; i<=n-1; i++)
    {
        s->vals0.ptr.p_double[i] = s->vals0.ptr.p_double[k+i];
    }
    for(i=n; i<=s->streamsize-1; i++)
    {
        s->vals0.ptr.p_double[i] = hqrnduniformr(rs, _state);
    }
    
    /*
     * Reshuffle Vals1
     */
    k = hqrnduniformi(rs, ae_minint(s->streamsize, 25, _state), _state);
    n = s->streamsize-k;
    for(i=0; i<=n-1; i++)
    {
        s->vals1.ptr.p_double[i] = s->vals1.ptr.p_double[k+i];
    }
    for(i=n; i<=s->streamsize-1; i++)
    {
        s->vals1.ptr.p_double[i] = hqrnduniformr(rs, _state);
    }
    
    /*
     * Reset the stream
     */
    s->streampos = 0;
}


/*************************************************************************
Fetch real number from stream. If stream is used up, generate value from RS,
store it to stream's memory and fetch to user.

If more than MaxSize values were fetched from the stream without restart,
an exception is generated

  -- ALGLIB --
     Copyright 07.12.2023 by Bochkanov Sergey
*************************************************************************/
static double diffevo_rstreamfetch(gdemorealstream* s,
     hqrndstate* rs,
     ae_state *_state)
{
    double result;


    if( s->streampos==s->maxsize )
    {
        ae_assert(ae_false, "GDEMO: integrity check 9430 failed", _state);
    }
    if( s->streampos==s->streamsize )
    {
        rgrowv(s->streamsize+1, &s->vals0, _state);
        rgrowv(s->streamsize+1, &s->vals1, _state);
        s->vals0.ptr.p_double[s->streamsize] = hqrnduniformr(rs, _state);
        s->vals1.ptr.p_double[s->streamsize] = hqrnduniformr(rs, _state);
        s->streamsize = s->streamsize+1;
    }
    result = s->vals0.ptr.p_double[s->streampos]+s->vals1.ptr.p_double[s->streampos];
    if( result>=1.0 )
    {
        result = result-1.0;
    }
    s->streampos = s->streampos+1;
    return result;
}


void _gdemointstream_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    gdemointstream *p = (gdemointstream*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->vals, 0, DT_INT, _state, make_automatic);
}


void _gdemointstream_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    gdemointstream       *dst = (gdemointstream*)_dst;
    const gdemointstream *src = (const gdemointstream*)_src;
    dst->streamsize = src->streamsize;
    dst->streampos = src->streampos;
    dst->maxval = src->maxval;
    dst->maxsize = src->maxsize;
    ae_vector_init_copy(&dst->vals, &src->vals, _state, make_automatic);
}


void _gdemointstream_clear(void* _p)
{
    gdemointstream *p = (gdemointstream*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->vals);
}


void _gdemointstream_destroy(void* _p)
{
    gdemointstream *p = (gdemointstream*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->vals);
}


void _gdemorealstream_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    gdemorealstream *p = (gdemorealstream*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->vals0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->vals1, 0, DT_REAL, _state, make_automatic);
}


void _gdemorealstream_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    gdemorealstream       *dst = (gdemorealstream*)_dst;
    const gdemorealstream *src = (const gdemorealstream*)_src;
    dst->streamsize = src->streamsize;
    dst->streampos = src->streampos;
    dst->maxsize = src->maxsize;
    ae_vector_init_copy(&dst->vals0, &src->vals0, _state, make_automatic);
    ae_vector_init_copy(&dst->vals1, &src->vals1, _state, make_automatic);
}


void _gdemorealstream_clear(void* _p)
{
    gdemorealstream *p = (gdemorealstream*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->vals0);
    ae_vector_clear(&p->vals1);
}


void _gdemorealstream_destroy(void* _p)
{
    gdemorealstream *p = (gdemorealstream*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->vals0);
    ae_vector_destroy(&p->vals1);
}


void _gdemostate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    gdemostate *p = (gdemostate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
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
    ae_vector_init(&p->reportx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->querydata, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replyfi, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    _hqrndstate_init(&p->rs, _state, make_automatic);
    ae_matrix_init(&p->population2x, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->population2rawreplies, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->population2fitness, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->population2params, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->candidatesx, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->candidatesrawreplies, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->candidatesfitness, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->candidatesparams, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->paramspool, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->xbest, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fbest, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpzero, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->dummysparsea, _state, make_automatic);
}


void _gdemostate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    gdemostate       *dst = (gdemostate*)_dst;
    const gdemostate *src = (const gdemostate*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->cntlc = src->cntlc;
    dst->cntnlc = src->cntnlc;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    dst->hasx0 = src->hasx0;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
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
    dst->popsize = src->popsize;
    dst->epochscnt = src->epochscnt;
    dst->crossoverprob = src->crossoverprob;
    dst->diffweight = src->diffweight;
    dst->constrmode = src->constrmode;
    dst->rho1 = src->rho1;
    dst->rho2 = src->rho2;
    dst->eps = src->eps;
    dst->nsample = src->nsample;
    dst->fixedparams = src->fixedparams;
    dst->fixedstrategy = src->fixedstrategy;
    dst->fixedcrossoverprob = src->fixedcrossoverprob;
    dst->fixeddifferentialweight = src->fixeddifferentialweight;
    dst->stoponsmallf = src->stoponsmallf;
    dst->smallf = src->smallf;
    dst->requesttype = src->requesttype;
    ae_vector_init_copy(&dst->reportx, &src->reportx, _state, make_automatic);
    dst->reportf = src->reportf;
    dst->querysize = src->querysize;
    ae_vector_init_copy(&dst->querydata, &src->querydata, _state, make_automatic);
    ae_vector_init_copy(&dst->replyfi, &src->replyfi, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    _hqrndstate_init_copy(&dst->rs, &src->rs, _state, make_automatic);
    ae_matrix_init_copy(&dst->population2x, &src->population2x, _state, make_automatic);
    ae_matrix_init_copy(&dst->population2rawreplies, &src->population2rawreplies, _state, make_automatic);
    ae_matrix_init_copy(&dst->population2fitness, &src->population2fitness, _state, make_automatic);
    ae_matrix_init_copy(&dst->population2params, &src->population2params, _state, make_automatic);
    ae_matrix_init_copy(&dst->candidatesx, &src->candidatesx, _state, make_automatic);
    ae_matrix_init_copy(&dst->candidatesrawreplies, &src->candidatesrawreplies, _state, make_automatic);
    ae_matrix_init_copy(&dst->candidatesfitness, &src->candidatesfitness, _state, make_automatic);
    ae_matrix_init_copy(&dst->candidatesparams, &src->candidatesparams, _state, make_automatic);
    ae_matrix_init_copy(&dst->paramspool, &src->paramspool, _state, make_automatic);
    dst->paramspoolsize = src->paramspoolsize;
    dst->poolinsertpos = src->poolinsertpos;
    ae_matrix_init_copy(&dst->xbest, &src->xbest, _state, make_automatic);
    ae_vector_init_copy(&dst->fbest, &src->fbest, _state, make_automatic);
    dst->repterminationtype = src->repterminationtype;
    dst->repiterationscount = src->repiterationscount;
    dst->replcerr = src->replcerr;
    dst->replcidx = src->replcidx;
    dst->repnlcerr = src->repnlcerr;
    dst->repnlcidx = src->repnlcidx;
    ae_vector_init_copy(&dst->tmpzero, &src->tmpzero, _state, make_automatic);
    _sparsematrix_init_copy(&dst->dummysparsea, &src->dummysparsea, _state, make_automatic);
}


void _gdemostate_clear(void* _p)
{
    gdemostate *p = (gdemostate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->x0);
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
    ae_vector_clear(&p->reportx);
    ae_vector_clear(&p->querydata);
    ae_vector_clear(&p->replyfi);
    _rcommstate_clear(&p->rstate);
    _hqrndstate_clear(&p->rs);
    ae_matrix_clear(&p->population2x);
    ae_matrix_clear(&p->population2rawreplies);
    ae_matrix_clear(&p->population2fitness);
    ae_matrix_clear(&p->population2params);
    ae_matrix_clear(&p->candidatesx);
    ae_matrix_clear(&p->candidatesrawreplies);
    ae_matrix_clear(&p->candidatesfitness);
    ae_matrix_clear(&p->candidatesparams);
    ae_matrix_clear(&p->paramspool);
    ae_matrix_clear(&p->xbest);
    ae_vector_clear(&p->fbest);
    ae_vector_clear(&p->tmpzero);
    _sparsematrix_clear(&p->dummysparsea);
}


void _gdemostate_destroy(void* _p)
{
    gdemostate *p = (gdemostate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->x0);
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
    ae_vector_destroy(&p->reportx);
    ae_vector_destroy(&p->querydata);
    ae_vector_destroy(&p->replyfi);
    _rcommstate_destroy(&p->rstate);
    _hqrndstate_destroy(&p->rs);
    ae_matrix_destroy(&p->population2x);
    ae_matrix_destroy(&p->population2rawreplies);
    ae_matrix_destroy(&p->population2fitness);
    ae_matrix_destroy(&p->population2params);
    ae_matrix_destroy(&p->candidatesx);
    ae_matrix_destroy(&p->candidatesrawreplies);
    ae_matrix_destroy(&p->candidatesfitness);
    ae_matrix_destroy(&p->candidatesparams);
    ae_matrix_destroy(&p->paramspool);
    ae_matrix_destroy(&p->xbest);
    ae_vector_destroy(&p->fbest);
    ae_vector_destroy(&p->tmpzero);
    _sparsematrix_destroy(&p->dummysparsea);
}


/*$ End $*/
