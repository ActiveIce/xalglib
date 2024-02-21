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
#include "monbi.h"


/*$ Declarations $*/
static void monbi_clearrequestfields(nbistate* state, ae_state *_state);
static void monbi_setnlcalgo(minnlcstate* s, ae_state *_state);


/*$ Body $*/


/*************************************************************************
Initialize NBI solver with the problem formulation

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void nbiscaleandinitbuf(/* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t m,
     ae_int_t frontsize,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     /* Real    */ const ae_matrix* densea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t ksparse,
     ae_int_t kdense,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     double epsx,
     ae_int_t maxits,
     ae_bool polishsolutions,
     nbistate* state,
     ae_state *_state)
{


    ae_assert(frontsize>=m, "NBIScaleAndInitBuf: FrontSize<M", _state);
    if( m==1 )
    {
        frontsize = 1;
    }
    state->n = n;
    state->m = m;
    state->epsx = epsx;
    state->maxits = maxits;
    state->xrep = ae_false;
    state->frontsize = frontsize;
    state->polishsolutions = polishsolutions;
    rsetallocv(n, 0.0, &state->tmpzero, _state);
    rsetallocv(n, 1.0, &state->tmpone, _state);
    
    /*
     * Copy and scale initial point
     */
    rcopyallocv(n, x0, &state->xstart, _state);
    rmergedivv(n, s, &state->xstart, _state);
    
    /*
     * Copy and scale box constraints
     */
    rcopyallocv(n, bndl, &state->bndl, _state);
    rcopyallocv(n, bndu, &state->bndu, _state);
    scaleshiftbcinplace(s, &state->tmpzero, &state->bndl, &state->bndu, n, _state);
    
    /*
     * Copy and scale linear constraints
     */
    state->ksparse = ksparse;
    state->kdense = kdense;
    rcopyallocv(ksparse+kdense, al, &state->cl, _state);
    rcopyallocv(ksparse+kdense, au, &state->cu, _state);
    if( ksparse>0 )
    {
        sparsecopybuf(sparsea, &state->sparsec, _state);
    }
    if( kdense>0 )
    {
        rcopyallocm(kdense, n, densea, &state->densec, _state);
    }
    scaleshiftmixedlcinplace(s, &state->tmpzero, n, &state->sparsec, ksparse, &state->densec, kdense, &state->cl, &state->cu, _state);
    
    /*
     * Copy nonlinear constraints
     */
    rcopyallocv(nnlc, nl, &state->nl, _state);
    rcopyallocv(nnlc, nu, &state->nu, _state);
    state->nnlc = nnlc;
    
    /*
     * Other fields
     */
    state->userrequestedtermination = ae_false;
    state->repterminationtype = 0;
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    state->repnfev = 0;
    state->repbcerr = (double)(0);
    state->repbcidx = -1;
    state->replcerr = (double)(0);
    state->replcidx = -1;
    state->repnlcerr = (double)(0);
    state->repnlcidx = -1;
    state->repfrontsize = 0;
    
    /*
     * prepare RComm facilities
     */
    ae_vector_set_length(&state->rstate.ia, 7+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 1+1, _state);
    state->rstate.stage = -1;
    monbi_clearrequestfields(state, _state);
    rallocv(n, &state->x, _state);
    rallocv(m+nnlc, &state->fi, _state);
    rallocm(m+nnlc, n, &state->j, _state);
}


/*************************************************************************
RCOMM function

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
ae_bool nbiiteration(nbistate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t objectiveidx;
    ae_int_t frontsize;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t frontrow;
    double v;
    double vv;
    ae_bool dotrace;
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
        objectiveidx = state->rstate.ia.ptr.p_int[2];
        frontsize = state->rstate.ia.ptr.p_int[3];
        i = state->rstate.ia.ptr.p_int[4];
        j = state->rstate.ia.ptr.p_int[5];
        k = state->rstate.ia.ptr.p_int[6];
        frontrow = state->rstate.ia.ptr.p_int[7];
        dotrace = state->rstate.ba.ptr.p_bool[0];
        v = state->rstate.ra.ptr.p_double[0];
        vv = state->rstate.ra.ptr.p_double[1];
    }
    else
    {
        n = 359;
        m = -58;
        objectiveidx = -919;
        frontsize = -909;
        i = 81;
        j = 255;
        k = 74;
        frontrow = -788;
        dotrace = ae_true;
        v = 205.0;
        vv = -838.0;
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
    if( state->rstate.stage==7 )
    {
        goto lbl_7;
    }
    if( state->rstate.stage==8 )
    {
        goto lbl_8;
    }
    if( state->rstate.stage==9 )
    {
        goto lbl_9;
    }
    if( state->rstate.stage==10 )
    {
        goto lbl_10;
    }
    if( state->rstate.stage==11 )
    {
        goto lbl_11;
    }
    if( state->rstate.stage==12 )
    {
        goto lbl_12;
    }
    if( state->rstate.stage==13 )
    {
        goto lbl_13;
    }
    if( state->rstate.stage==14 )
    {
        goto lbl_14;
    }
    if( state->rstate.stage==15 )
    {
        goto lbl_15;
    }
    
    /*
     * Routine body
     */
    n = state->n;
    m = state->m;
    frontsize = state->frontsize;
    hqrndseed(348546, 436734, &state->rs, _state);
    ae_assert(m>1||frontsize==1, "NBI: integrity check 3309 failed", _state);
    dotrace = ae_is_trace_enabled("NBI");
    
    /*
     * Trace output (if needed)
     */
    if( dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  NBI SOLVER STARTED                                                                            //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("N             = %0d (variables)\n",
            (int)(n));
        ae_trace("M             = %0d (objectives)\n",
            (int)(m));
    }
    
    /*
     * Prepare rcomm interface
     */
    monbi_clearrequestfields(state, _state);
    
    /*
     * Prepare initial state of the output
     */
    ae_assert(state->frontsize>=m, "NBI: integrity check 0503 failed", _state);
    rsetallocm(frontsize, n+m+m, _state->v_nan, &state->repparetofront, _state);
    state->repfrontsize = 0;
    state->repterminationtype = 2;
    state->userrequestedtermination = ae_false;
    
    /*
     * Convert linear and nonlinear constraints to an old format
     */
    converttwosidedlctoonesidedold(&state->sparsec, state->ksparse, &state->densec, state->kdense, n, &state->cl, &state->cu, &state->olddensec, &state->olddensect, &state->olddensek, _state);
    converttwosidednlctoonesidedold(&state->nl, &state->nu, state->nnlc, &state->nlcidx, &state->nlcmul, &state->nlcadd, &state->nlcnlec, &state->nlcnlic, _state);
    
    /*
     * Solve M initial single-objective problems that are used to find an ideal objectives vector
     */
    minnlccreate(n, &state->xstart, &state->nlcsolver, _state);
    minnlcsetcond(&state->nlcsolver, state->epsx, state->maxits, _state);
    minnlcsetbc(&state->nlcsolver, &state->bndl, &state->bndu, _state);
    minnlcsetlc(&state->nlcsolver, &state->olddensec, &state->olddensect, state->olddensek, _state);
    monbi_setnlcalgo(&state->nlcsolver, _state);
    objectiveidx = 0;
lbl_16:
    if( objectiveidx>m-1 )
    {
        goto lbl_18;
    }
    if( !dotrace )
    {
        goto lbl_19;
    }
    ae_trace("===== OPTIMIZING FOR OBJECTIVE %2d ==================================================================\n",
        (int)(objectiveidx));
    state->needfij = ae_true;
    rcopyv(n, &state->xstart, &state->x, _state);
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    ae_trace("F (initial)   = ");
    tracevectorautoprec(&state->fi, 0, m, _state);
    ae_trace("\n");
lbl_19:
    
    /*
     * Optimize with respect to one objective
     */
    minnlcsetnlc(&state->nlcsolver, state->nlcnlec, state->nlcnlic, _state);
    minnlcrestartfrom(&state->nlcsolver, &state->xstart, _state);
lbl_21:
    if( !minnlciteration(&state->nlcsolver, _state) )
    {
        goto lbl_22;
    }
    if( state->userrequestedtermination )
    {
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    if( !state->nlcsolver.needfij )
    {
        goto lbl_23;
    }
    
    /*
     * Forward report to the caller
     */
    state->needfij = ae_true;
    rcopyv(n, &state->nlcsolver.x, &state->x, _state);
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needfij = ae_false;
    
    /*
     * Convert target and constraints to the format used by the subproblem
     */
    state->nlcsolver.fi.ptr.p_double[0] = state->fi.ptr.p_double[objectiveidx];
    rcopyrr(n, &state->j, objectiveidx, &state->nlcsolver.j, 0, _state);
    for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
    {
        state->nlcsolver.fi.ptr.p_double[i+1] = state->fi.ptr.p_double[m+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
        rcopyrr(n, &state->j, m+state->nlcidx.ptr.p_int[i], &state->nlcsolver.j, i+1, _state);
        rmulr(n, state->nlcmul.ptr.p_double[i], &state->nlcsolver.j, i+1, _state);
    }
    goto lbl_21;
lbl_23:
    ae_assert(ae_false, "NBI: integrity check 6034 failed", _state);
    goto lbl_21;
lbl_22:
    minnlcresultsbuf(&state->nlcsolver, &state->x1, &state->nlcrep, _state);
    if( state->nlcrep.terminationtype==-8 )
    {
        state->repterminationtype = -8;
        state->repfrontsize = 0;
        result = ae_false;
        return result;
    }
    ae_assert(state->nlcrep.terminationtype>0, "NBI: integrity check 7144 failed", _state);
    state->repinneriterationscount = state->repinneriterationscount+state->nlcrep.iterationscount;
    state->repnfev = state->repnfev+state->nlcrep.nfev;
    if( !dotrace )
    {
        goto lbl_25;
    }
    state->needfij = ae_true;
    rcopyv(n, &state->x1, &state->x, _state);
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    ae_trace("F (unpolished)= ");
    tracevectorautoprec(&state->fi, 0, m, _state);
    ae_trace(" (%0d its, feas.err=%0.2e)\n",
        (int)(state->nlcrep.iterationscount),
        (double)(rmax3(state->nlcrep.bcerr, state->nlcrep.lcerr, state->nlcrep.nlcerr, _state)));
lbl_25:
    
    /*
     * Polish the solution: having solution X1 that minimizes f_objectiveIdx,
     * now we want to minimize a sum of f[i] subject to constraint that f_i(x)<=f_i(X1).
     *
     * This stage is essential for polishing other components of the solution
     * (even with moderate condition numbers it is possible to have f[i!=objectiveIdx]
     * underoptimized due to purely numerical issues).
     */
    if( !state->polishsolutions )
    {
        goto lbl_27;
    }
    state->needfij = ae_true;
    rcopyv(n, &state->x1, &state->x, _state);
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    rcopyallocv(m+state->nnlc, &state->fi, &state->fix1, _state);
    minnlcsetnlc(&state->nlcsolver, state->nlcnlec, state->nlcnlic+1, _state);
    minnlcrestartfrom(&state->nlcsolver, &state->x1, _state);
lbl_29:
    if( !minnlciteration(&state->nlcsolver, _state) )
    {
        goto lbl_30;
    }
    if( state->userrequestedtermination )
    {
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    if( !state->nlcsolver.needfij )
    {
        goto lbl_31;
    }
    
    /*
     * Forward report to the caller
     */
    state->needfij = ae_true;
    rcopyv(n, &state->nlcsolver.x, &state->x, _state);
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needfij = ae_false;
    
    /*
     * Target: sum of objectives
     */
    state->nlcsolver.fi.ptr.p_double[0] = (double)(0);
    rsetr(n, 0.0, &state->nlcsolver.j, 0, _state);
    for(i=0; i<=m-1; i++)
    {
        state->nlcsolver.fi.ptr.p_double[0] = state->nlcsolver.fi.ptr.p_double[0]+state->fi.ptr.p_double[i];
        raddrr(n, 1.0, &state->j, i, &state->nlcsolver.j, 0, _state);
    }
    
    /*
     * Original nonlinear constraints
     */
    for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
    {
        state->nlcsolver.fi.ptr.p_double[i+1] = state->fi.ptr.p_double[m+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
        rcopyrr(n, &state->j, m+state->nlcidx.ptr.p_int[i], &state->nlcsolver.j, i+1, _state);
        rmulr(n, state->nlcmul.ptr.p_double[i], &state->nlcsolver.j, i+1, _state);
    }
    
    /*
     * Additional nonlinear constraint: f[objectiveidx] does not increase: f(x)-f(X1)<=0
     */
    state->nlcsolver.fi.ptr.p_double[1+state->nlcnlec+state->nlcnlic] = state->fi.ptr.p_double[objectiveidx]-state->fix1.ptr.p_double[objectiveidx];
    rcopyrr(n, &state->j, objectiveidx, &state->nlcsolver.j, 1+state->nlcnlec+state->nlcnlic, _state);
    
    /*
     * Done
     */
    goto lbl_29;
lbl_31:
    ae_assert(ae_false, "NBI: integrity check 7056 failed", _state);
    goto lbl_29;
lbl_30:
    minnlcresultsbuf(&state->nlcsolver, &state->x2, &state->nlcrep, _state);
    if( state->nlcrep.terminationtype==-8 )
    {
        state->repterminationtype = -8;
        state->repfrontsize = 0;
        result = ae_false;
        return result;
    }
    ae_assert(state->nlcrep.terminationtype>0, "NBI: integrity check 7144 failed", _state);
    state->repinneriterationscount = state->repinneriterationscount+state->nlcrep.iterationscount;
    state->repnfev = state->repnfev+state->nlcrep.nfev;
    if( !dotrace )
    {
        goto lbl_33;
    }
    state->needfij = ae_true;
    rcopyv(n, &state->x2, &state->x, _state);
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    ae_trace("F (polished)  = ");
    tracevectorautoprec(&state->fi, 0, m, _state);
    ae_trace(" (%0d its, feas.err=%0.2e)\n",
        (int)(state->nlcrep.iterationscount),
        (double)(rmax3(state->nlcrep.bcerr, state->nlcrep.lcerr, state->nlcrep.nlcerr, _state)));
lbl_33:
    goto lbl_28;
lbl_27:
    rcopyallocv(n, &state->x1, &state->x2, _state);
lbl_28:
    
    /*
     * Save solution and objectives at the solution
     */
    state->needfij = ae_true;
    rcopyv(n, &state->x2, &state->x, _state);
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    rcopyvr(n, &state->x2, &state->repparetofront, state->repfrontsize, _state);
    for(i=0; i<=m-1; i++)
    {
        state->repparetofront.ptr.pp_double[state->repfrontsize][n+i] = state->fi.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        state->repparetofront.ptr.pp_double[state->repfrontsize][n+m+i] = rcase2(i==objectiveidx, (double)(1), (double)(0), _state);
    }
    state->repfrontsize = state->repfrontsize+1;
    state->repouteriterationscount = state->repouteriterationscount+1;
    if( ae_fp_greater(state->nlcrep.bcerr,state->repbcerr) )
    {
        state->repbcerr = state->nlcrep.bcerr;
        state->repbcidx = state->nlcrep.bcidx;
    }
    if( ae_fp_greater(state->nlcrep.lcerr,state->replcerr) )
    {
        state->replcerr = state->nlcrep.lcerr;
        state->replcidx = state->nlcrep.lcidx;
    }
    if( ae_fp_greater(state->nlcrep.nlcerr,state->repnlcerr) )
    {
        state->repnlcerr = state->nlcrep.nlcerr;
        state->repnlcidx = state->nlcrep.nlcidx;
    }
    
    /*
     * Report last point found and check request for termination
     */
    rcopyrv(n, &state->repparetofront, state->repfrontsize-1, &state->x, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->xupdated = ae_false;
    if( state->userrequestedtermination )
    {
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    objectiveidx = objectiveidx+1;
    goto lbl_16;
lbl_18:
    
    /*
     * Compute ideal vector and payoff matrix
     */
    rallocv(m, &state->fideal, _state);
    for(i=0; i<=m-1; i++)
    {
        state->fideal.ptr.p_double[i] = state->repparetofront.ptr.pp_double[i][n+i];
    }
    rallocm(m, m, &state->payoff, _state);
    for(i=0; i<=m-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            state->payoff.ptr.pp_double[i][j] = state->repparetofront.ptr.pp_double[j][n+i]-state->fideal.ptr.p_double[i];
        }
    }
    
    /*
     * Generate quasi-normal vector Delta[].
     *
     * NOTE: if |Delta| is less than 1, we normalize it. Without normalization
     *       nonlinear optimizer may fail when Delta is very small because
     *       an extremely small scale of Delta implies an extremely large scale
     *       of the artificial variable t.
     *
     *       Values of Delta larger than 1 are not normalized.
     *
     */
    rallocv(m, &state->delta, _state);
    for(i=0; i<=m-1; i++)
    {
        state->delta.ptr.p_double[i] = (double)(0);
        for(j=0; j<=m-1; j++)
        {
            state->delta.ptr.p_double[i] = state->delta.ptr.p_double[i]-state->payoff.ptr.pp_double[i][j];
        }
        if( ae_fp_greater_eq(state->delta.ptr.p_double[i],(double)(0)) )
        {
            
            /*
             * Something is wrong with payoff matrix, fix direction
             */
            state->delta.ptr.p_double[i] = (double)(0);
            for(j=0; j<=m-1; j++)
            {
                state->delta.ptr.p_double[i] = ae_minreal(state->delta.ptr.p_double[i], -state->payoff.ptr.pp_double[i][j], _state);
            }
            state->delta.ptr.p_double[i] = coalesce(state->delta.ptr.p_double[i], (double)(-1), _state);
        }
    }
    v = ae_sqrt(rdotv2(m, &state->delta, _state), _state);
    ae_assert(ae_fp_neq(v,(double)(0)), "NBI: integrity check 8546 failed", _state);
    rmulv(m, ae_maxreal((double)1/v, 1.0, _state), &state->delta, _state);
    
    /*
     * For each front row solve NBI subproblem
     */
    frontrow = m;
lbl_35:
    if( frontrow>frontsize-1 )
    {
        goto lbl_37;
    }
    if( dotrace )
    {
        ae_trace("===== OPTIMIZING FOR FRONT POINT %4d ==============================================================\n",
            (int)(frontrow));
    }
    
    /*
     * Generate convex hull coordinates vector Beta[] using the following
     * algorithm to obtain uniformly sampled points on a simplex:
     *
     * * generate M random uniform numbers
     * * take logarithms
     * * normalize sum
     *
     * Without latter two steps we will obtain heavily non-uniform distribution
     * with a sharp peak at the center.
     */
    rallocv(m, &state->beta, _state);
    v = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        state->beta.ptr.p_double[i] = ae_log(hqrnduniformr(&state->rs, _state)+ae_machineepsilon, _state);
        v = v+state->beta.ptr.p_double[i];
    }
    rmulv(m, (double)1/v, &state->beta, _state);
    if( dotrace )
    {
        ae_trace("Beta (normal) = ");
        tracevectorautoprec(&state->beta, 0, m, _state);
        ae_trace("\n");
    }
    
    /*
     * Prepare initial point for a subproblem:
     * * for N first variables, select point from the front with closest values of Beta[]
     * * for last variable, set T to zero (numerical experience shows that it is good enough)
     */
    rallocv(n+1, &state->subproblemstart, _state);
    k = -1;
    v = ae_maxrealnumber;
    for(i=0; i<=frontrow-1; i++)
    {
        vv = (double)(0);
        for(j=0; j<=m-1; j++)
        {
            vv = vv+ae_sqr(state->repparetofront.ptr.pp_double[i][n+m+j]-state->beta.ptr.p_double[j], _state);
        }
        if( ae_fp_less(vv,v) )
        {
            k = i;
            v = vv;
        }
    }
    ae_assert(k>=0, "NBI: integrity check 0630 failed", _state);
    rcopyrv(n, &state->repparetofront, k, &state->subproblemstart, _state);
    state->subproblemstart.ptr.p_double[n] = 0.0;
    if( dotrace )
    {
        ae_trace("StartIdx      = %0d (front point used as the initial one)\n",
            (int)(k));
    }
    
    /*
     * Reformulate box and linear constraints
     */
    rallocv(n+1, &state->bndlx, _state);
    rallocv(n+1, &state->bndux, _state);
    rcopyv(n, &state->bndl, &state->bndlx, _state);
    rcopyv(n, &state->bndu, &state->bndux, _state);
    state->bndlx.ptr.p_double[n] = _state->v_neginf;
    state->bndux.ptr.p_double[n] = _state->v_posinf;
    if( state->olddensek>0 )
    {
        rallocm(state->olddensek, n+2, &state->olddensecx, _state);
        rcopym(state->olddensek, n, &state->olddensec, &state->olddensecx, _state);
        for(i=0; i<=state->olddensek-1; i++)
        {
            state->olddensecx.ptr.pp_double[i][n] = 0.0;
            state->olddensecx.ptr.pp_double[i][n+1] = state->olddensec.ptr.pp_double[i][n];
        }
    }
    
    /*
     * Report initial F-vector
     */
    if( !dotrace )
    {
        goto lbl_38;
    }
    state->needfij = ae_true;
    rcopyv(n, &state->subproblemstart, &state->x, _state);
    state->rstate.stage = 8;
    goto lbl_rcomm;
lbl_8:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    ae_trace("F (initial)   = ");
    tracevectorautoprec(&state->fi, 0, m, _state);
    ae_trace("\n");
lbl_38:
    
    /*
     * Solve initial NBI subproblem
     */
    minnlccreate(n+1, &state->subproblemstart, &state->nlcsolver, _state);
    minnlcsetcond(&state->nlcsolver, state->epsx, state->maxits, _state);
    minnlcsetbc(&state->nlcsolver, &state->bndlx, &state->bndux, _state);
    minnlcsetlc(&state->nlcsolver, &state->olddensecx, &state->olddensect, state->olddensek, _state);
    minnlcsetnlc(&state->nlcsolver, state->nlcnlec, state->nlcnlic+m, _state);
    monbi_setnlcalgo(&state->nlcsolver, _state);
lbl_40:
    if( !minnlciteration(&state->nlcsolver, _state) )
    {
        goto lbl_41;
    }
    if( state->userrequestedtermination )
    {
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    if( !state->nlcsolver.needfij )
    {
        goto lbl_42;
    }
    
    /*
     * Forward report to the caller
     */
    state->needfij = ae_true;
    rcopyv(n, &state->nlcsolver.x, &state->x, _state);
    state->rstate.stage = 9;
    goto lbl_rcomm;
lbl_9:
    state->needfij = ae_false;
    
    /*
     * Compute target
     */
    state->nlcsolver.fi.ptr.p_double[0] = -state->nlcsolver.x.ptr.p_double[n];
    rsetr(n+1, 0.0, &state->nlcsolver.j, 0, _state);
    state->nlcsolver.j.ptr.pp_double[0][n] = (double)(-1);
    
    /*
     * Forward original nonlinear constraints
     */
    for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
    {
        state->nlcsolver.fi.ptr.p_double[1+i] = state->fi.ptr.p_double[m+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
        rcopyrr(n, &state->j, m+state->nlcidx.ptr.p_int[i], &state->nlcsolver.j, 1+i, _state);
        rmulr(n, state->nlcmul.ptr.p_double[i], &state->nlcsolver.j, 1+i, _state);
        state->nlcsolver.j.ptr.pp_double[1+i][n] = (double)(0);
    }
    
    /*
     * Append NBI constraints
     */
    for(i=0; i<=m-1; i++)
    {
        
        /*
         * PAYOFF*Beta + t*Delta >= F(x)-FIdeal
         *
         * F(x) - FIdeal - t*DELTA - PAYOFF*Beta <= 0
         */
        state->nlcsolver.fi.ptr.p_double[1+state->nlcnlec+state->nlcnlic+i] = state->fi.ptr.p_double[i]-state->fideal.ptr.p_double[i]-state->nlcsolver.x.ptr.p_double[n]*state->delta.ptr.p_double[i]-rdotvr(m, &state->beta, &state->payoff, i, _state);
        rcopyrr(n, &state->j, i, &state->nlcsolver.j, 1+state->nlcnlec+state->nlcnlic+i, _state);
        state->nlcsolver.j.ptr.pp_double[1+state->nlcnlec+state->nlcnlic+i][n] = -state->delta.ptr.p_double[i];
    }
    goto lbl_40;
lbl_42:
    ae_assert(ae_false, "NBI: integrity check 6034 failed", _state);
    goto lbl_40;
lbl_41:
    minnlcresultsbuf(&state->nlcsolver, &state->x1, &state->nlcrep, _state);
    if( state->nlcrep.terminationtype==-8 )
    {
        state->repterminationtype = -8;
        state->repfrontsize = 0;
        result = ae_false;
        return result;
    }
    ae_assert(state->nlcrep.terminationtype>0, "NBI: integrity check 7144 failed", _state);
    state->repinneriterationscount = state->repinneriterationscount+state->nlcrep.iterationscount;
    state->repnfev = state->repnfev+state->nlcrep.nfev;
    if( !dotrace )
    {
        goto lbl_44;
    }
    state->needfij = ae_true;
    rcopyv(n, &state->x1, &state->x, _state);
    state->rstate.stage = 10;
    goto lbl_rcomm;
lbl_10:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    ae_trace("F (unpolished)= ");
    tracevectorautoprec(&state->fi, 0, m, _state);
    ae_trace(" (%0d its, feas.err=%0.2e)\n",
        (int)(state->nlcrep.iterationscount),
        (double)(rmax3(state->nlcrep.bcerr, state->nlcrep.lcerr, state->nlcrep.nlcerr, _state)));
lbl_44:
    
    /*
     * Polish the solution: having solution X1 that minimizes f_objectiveIdx,
     * now we want to minimize a sum of f[i] subject to constraint that f_i(x)<=f_i(X1).
     *
     * This stage is essential for polishing other components of the solution
     * (even with moderate condition numbers it is possible to have f[i!=objectiveIdx]
     * underoptimized due to purely numerical issues).
     */
    if( !state->polishsolutions )
    {
        goto lbl_46;
    }
    state->needfij = ae_true;
    rcopyv(n, &state->x1, &state->x, _state);
    state->rstate.stage = 11;
    goto lbl_rcomm;
lbl_11:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    rcopyallocv(m+state->nnlc, &state->fi, &state->fix1, _state);
    minnlccreate(n, &state->x1, &state->nlcsolver, _state);
    minnlcsetcond(&state->nlcsolver, state->epsx, state->maxits, _state);
    minnlcsetbc(&state->nlcsolver, &state->bndlx, &state->bndux, _state);
    minnlcsetlc(&state->nlcsolver, &state->olddensec, &state->olddensect, state->olddensek, _state);
    minnlcsetnlc(&state->nlcsolver, state->nlcnlec, state->nlcnlic+m, _state);
    monbi_setnlcalgo(&state->nlcsolver, _state);
lbl_48:
    if( !minnlciteration(&state->nlcsolver, _state) )
    {
        goto lbl_49;
    }
    if( state->userrequestedtermination )
    {
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    if( !state->nlcsolver.needfij )
    {
        goto lbl_50;
    }
    
    /*
     * Forward report to the caller
     */
    state->needfij = ae_true;
    rcopyv(n, &state->nlcsolver.x, &state->x, _state);
    state->rstate.stage = 12;
    goto lbl_rcomm;
lbl_12:
    state->needfij = ae_false;
    
    /*
     * Target: sum of objectives
     */
    state->nlcsolver.fi.ptr.p_double[0] = (double)(0);
    rsetr(n, 0.0, &state->nlcsolver.j, 0, _state);
    for(i=0; i<=m-1; i++)
    {
        state->nlcsolver.fi.ptr.p_double[0] = state->nlcsolver.fi.ptr.p_double[0]+state->fi.ptr.p_double[i];
        raddrr(n, 1.0, &state->j, i, &state->nlcsolver.j, 0, _state);
    }
    
    /*
     * Original nonlinear constraints
     */
    for(i=0; i<=state->nlcnlec+state->nlcnlic-1; i++)
    {
        state->nlcsolver.fi.ptr.p_double[i+1] = state->fi.ptr.p_double[m+state->nlcidx.ptr.p_int[i]]*state->nlcmul.ptr.p_double[i]+state->nlcadd.ptr.p_double[i];
        rcopyrr(n, &state->j, m+state->nlcidx.ptr.p_int[i], &state->nlcsolver.j, i+1, _state);
        rmulr(n, state->nlcmul.ptr.p_double[i], &state->nlcsolver.j, i+1, _state);
    }
    
    /*
     * Additional nonlinear constraints: objectives do not increase: f(x)-f(X1)<=0
     */
    for(i=0; i<=m-1; i++)
    {
        state->nlcsolver.fi.ptr.p_double[1+state->nlcnlec+state->nlcnlic+i] = state->fi.ptr.p_double[i]-state->fix1.ptr.p_double[i];
        rcopyrr(n, &state->j, i, &state->nlcsolver.j, 1+state->nlcnlec+state->nlcnlic+i, _state);
    }
    
    /*
     * Done
     */
    goto lbl_48;
lbl_50:
    ae_assert(ae_false, "NBI: integrity check 6034 failed", _state);
    goto lbl_48;
lbl_49:
    minnlcresultsbuf(&state->nlcsolver, &state->x2, &state->nlcrep, _state);
    if( state->nlcrep.terminationtype==-8 )
    {
        state->repterminationtype = -8;
        state->repfrontsize = 0;
        result = ae_false;
        return result;
    }
    ae_assert(state->nlcrep.terminationtype>0, "NBI: integrity check 7144 failed", _state);
    state->repinneriterationscount = state->repinneriterationscount+state->nlcrep.iterationscount;
    state->repnfev = state->repnfev+state->nlcrep.nfev;
    if( !dotrace )
    {
        goto lbl_52;
    }
    state->needfij = ae_true;
    rcopyv(n, &state->x2, &state->x, _state);
    state->rstate.stage = 13;
    goto lbl_rcomm;
lbl_13:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    ae_trace("F (polished)  = ");
    tracevectorautoprec(&state->fi, 0, m, _state);
    ae_trace(" (%0d its, feas.err=%0.2e)\n",
        (int)(state->nlcrep.iterationscount),
        (double)(rmax3(state->nlcrep.bcerr, state->nlcrep.lcerr, state->nlcrep.nlcerr, _state)));
lbl_52:
    goto lbl_47;
lbl_46:
    rcopyallocv(n, &state->x1, &state->x2, _state);
lbl_47:
    
    /*
     * Save solution and objectives at the solution, update statistics
     */
    state->needfij = ae_true;
    rcopyv(n, &state->x2, &state->x, _state);
    state->rstate.stage = 14;
    goto lbl_rcomm;
lbl_14:
    state->needfij = ae_false;
    state->repnfev = state->repnfev+1;
    ae_assert(state->repfrontsize<frontsize, "MONBI: integrity check 1753 failed", _state);
    rcopyvr(n, &state->x2, &state->repparetofront, state->repfrontsize, _state);
    for(i=0; i<=m-1; i++)
    {
        state->repparetofront.ptr.pp_double[state->repfrontsize][n+i] = state->fi.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        state->repparetofront.ptr.pp_double[state->repfrontsize][n+m+i] = state->beta.ptr.p_double[i];
    }
    state->repfrontsize = state->repfrontsize+1;
    state->repouteriterationscount = state->repouteriterationscount+1;
    if( ae_fp_greater(state->nlcrep.bcerr,state->repbcerr) )
    {
        state->repbcerr = state->nlcrep.bcerr;
        state->repbcidx = state->nlcrep.bcidx;
    }
    if( ae_fp_greater(state->nlcrep.lcerr,state->replcerr) )
    {
        state->replcerr = state->nlcrep.lcerr;
        state->replcidx = state->nlcrep.lcidx;
    }
    if( ae_fp_greater(state->nlcrep.nlcerr,state->repnlcerr) )
    {
        state->repnlcerr = state->nlcrep.nlcerr;
        state->repnlcidx = state->nlcrep.nlcidx;
    }
    
    /*
     * Report last point found and check request for termination
     */
    rcopyrv(n, &state->repparetofront, state->repfrontsize-1, &state->x, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 15;
    goto lbl_rcomm;
lbl_15:
    state->xupdated = ae_false;
    if( state->userrequestedtermination )
    {
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    frontrow = frontrow+1;
    goto lbl_35;
lbl_37:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = objectiveidx;
    state->rstate.ia.ptr.p_int[3] = frontsize;
    state->rstate.ia.ptr.p_int[4] = i;
    state->rstate.ia.ptr.p_int[5] = j;
    state->rstate.ia.ptr.p_int[6] = k;
    state->rstate.ia.ptr.p_int[7] = frontrow;
    state->rstate.ba.ptr.p_bool[0] = dotrace;
    state->rstate.ra.ptr.p_double[0] = v;
    state->rstate.ra.ptr.p_double[1] = vv;
    return result;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forget to clear something)
*************************************************************************/
static void monbi_clearrequestfields(nbistate* state, ae_state *_state)
{


    state->needfij = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Sets NLC solver
*************************************************************************/
static void monbi_setnlcalgo(minnlcstate* s, ae_state *_state)
{


    minnlcsetalgosqp(s, _state);
}


void _nbistate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nbistate *p = (nbistate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->xstart, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->densec, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsec, _state, make_automatic);
    ae_vector_init(&p->cl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_matrix_init(&p->repparetofront, 0, 0, DT_REAL, _state, make_automatic);
    _minnlcstate_init(&p->nlcsolver, _state, make_automatic);
    _minnlcreport_init(&p->nlcrep, _state, make_automatic);
    ae_vector_init(&p->tmpzero, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpone, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->olddensec, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->olddensect, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->nlcidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->nlcmul, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nlcadd, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fideal, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->payoff, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->beta, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->delta, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->subproblemstart, 0, DT_REAL, _state, make_automatic);
    _hqrndstate_init(&p->rs, _state, make_automatic);
    ae_vector_init(&p->bndlx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndux, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->olddensecx, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fix1, 0, DT_REAL, _state, make_automatic);
}


void _nbistate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    nbistate       *dst = (nbistate*)_dst;
    const nbistate *src = (const nbistate*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->epsx = src->epsx;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    ae_vector_init_copy(&dst->xstart, &src->xstart, _state, make_automatic);
    dst->frontsize = src->frontsize;
    dst->polishsolutions = src->polishsolutions;
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    dst->ksparse = src->ksparse;
    dst->kdense = src->kdense;
    ae_matrix_init_copy(&dst->densec, &src->densec, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsec, &src->sparsec, _state, make_automatic);
    ae_vector_init_copy(&dst->cl, &src->cl, _state, make_automatic);
    ae_vector_init_copy(&dst->cu, &src->cu, _state, make_automatic);
    dst->nnlc = src->nnlc;
    ae_vector_init_copy(&dst->nl, &src->nl, _state, make_automatic);
    ae_vector_init_copy(&dst->nu, &src->nu, _state, make_automatic);
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    dst->needfij = src->needfij;
    dst->xupdated = src->xupdated;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    dst->userrequestedtermination = src->userrequestedtermination;
    ae_matrix_init_copy(&dst->repparetofront, &src->repparetofront, _state, make_automatic);
    dst->repfrontsize = src->repfrontsize;
    dst->repinneriterationscount = src->repinneriterationscount;
    dst->repouteriterationscount = src->repouteriterationscount;
    dst->repnfev = src->repnfev;
    dst->repterminationtype = src->repterminationtype;
    dst->repbcerr = src->repbcerr;
    dst->repbcidx = src->repbcidx;
    dst->replcerr = src->replcerr;
    dst->replcidx = src->replcidx;
    dst->repnlcerr = src->repnlcerr;
    dst->repnlcidx = src->repnlcidx;
    _minnlcstate_init_copy(&dst->nlcsolver, &src->nlcsolver, _state, make_automatic);
    _minnlcreport_init_copy(&dst->nlcrep, &src->nlcrep, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpzero, &src->tmpzero, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpone, &src->tmpone, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_matrix_init_copy(&dst->olddensec, &src->olddensec, _state, make_automatic);
    ae_vector_init_copy(&dst->olddensect, &src->olddensect, _state, make_automatic);
    dst->olddensek = src->olddensek;
    ae_vector_init_copy(&dst->nlcidx, &src->nlcidx, _state, make_automatic);
    ae_vector_init_copy(&dst->nlcmul, &src->nlcmul, _state, make_automatic);
    ae_vector_init_copy(&dst->nlcadd, &src->nlcadd, _state, make_automatic);
    dst->nlcnlec = src->nlcnlec;
    dst->nlcnlic = src->nlcnlic;
    ae_vector_init_copy(&dst->fideal, &src->fideal, _state, make_automatic);
    ae_matrix_init_copy(&dst->payoff, &src->payoff, _state, make_automatic);
    ae_vector_init_copy(&dst->beta, &src->beta, _state, make_automatic);
    ae_vector_init_copy(&dst->delta, &src->delta, _state, make_automatic);
    ae_vector_init_copy(&dst->subproblemstart, &src->subproblemstart, _state, make_automatic);
    _hqrndstate_init_copy(&dst->rs, &src->rs, _state, make_automatic);
    ae_vector_init_copy(&dst->bndlx, &src->bndlx, _state, make_automatic);
    ae_vector_init_copy(&dst->bndux, &src->bndux, _state, make_automatic);
    ae_matrix_init_copy(&dst->olddensecx, &src->olddensecx, _state, make_automatic);
    ae_vector_init_copy(&dst->x1, &src->x1, _state, make_automatic);
    ae_vector_init_copy(&dst->x2, &src->x2, _state, make_automatic);
    ae_vector_init_copy(&dst->fix1, &src->fix1, _state, make_automatic);
}


void _nbistate_clear(void* _p)
{
    nbistate *p = (nbistate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->xstart);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_matrix_clear(&p->densec);
    _sparsematrix_clear(&p->sparsec);
    ae_vector_clear(&p->cl);
    ae_vector_clear(&p->cu);
    ae_vector_clear(&p->nl);
    ae_vector_clear(&p->nu);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    _rcommstate_clear(&p->rstate);
    ae_matrix_clear(&p->repparetofront);
    _minnlcstate_clear(&p->nlcsolver);
    _minnlcreport_clear(&p->nlcrep);
    ae_vector_clear(&p->tmpzero);
    ae_vector_clear(&p->tmpone);
    ae_vector_clear(&p->tmp0);
    ae_matrix_clear(&p->olddensec);
    ae_vector_clear(&p->olddensect);
    ae_vector_clear(&p->nlcidx);
    ae_vector_clear(&p->nlcmul);
    ae_vector_clear(&p->nlcadd);
    ae_vector_clear(&p->fideal);
    ae_matrix_clear(&p->payoff);
    ae_vector_clear(&p->beta);
    ae_vector_clear(&p->delta);
    ae_vector_clear(&p->subproblemstart);
    _hqrndstate_clear(&p->rs);
    ae_vector_clear(&p->bndlx);
    ae_vector_clear(&p->bndux);
    ae_matrix_clear(&p->olddensecx);
    ae_vector_clear(&p->x1);
    ae_vector_clear(&p->x2);
    ae_vector_clear(&p->fix1);
}


void _nbistate_destroy(void* _p)
{
    nbistate *p = (nbistate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->xstart);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_matrix_destroy(&p->densec);
    _sparsematrix_destroy(&p->sparsec);
    ae_vector_destroy(&p->cl);
    ae_vector_destroy(&p->cu);
    ae_vector_destroy(&p->nl);
    ae_vector_destroy(&p->nu);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    _rcommstate_destroy(&p->rstate);
    ae_matrix_destroy(&p->repparetofront);
    _minnlcstate_destroy(&p->nlcsolver);
    _minnlcreport_destroy(&p->nlcrep);
    ae_vector_destroy(&p->tmpzero);
    ae_vector_destroy(&p->tmpone);
    ae_vector_destroy(&p->tmp0);
    ae_matrix_destroy(&p->olddensec);
    ae_vector_destroy(&p->olddensect);
    ae_vector_destroy(&p->nlcidx);
    ae_vector_destroy(&p->nlcmul);
    ae_vector_destroy(&p->nlcadd);
    ae_vector_destroy(&p->fideal);
    ae_matrix_destroy(&p->payoff);
    ae_vector_destroy(&p->beta);
    ae_vector_destroy(&p->delta);
    ae_vector_destroy(&p->subproblemstart);
    _hqrndstate_destroy(&p->rs);
    ae_vector_destroy(&p->bndlx);
    ae_vector_destroy(&p->bndux);
    ae_matrix_destroy(&p->olddensecx);
    ae_vector_destroy(&p->x1);
    ae_vector_destroy(&p->x2);
    ae_vector_destroy(&p->fix1);
}


/*$ End $*/
