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
#include "reviseddualsimplex.h"


/*$ Declarations $*/
static ae_int_t reviseddualsimplex_maxforcedrestarts = 1;
static ae_int_t reviseddualsimplex_safetrfage = 5;
static ae_int_t reviseddualsimplex_defaultmaxtrfage = 100;
static double reviseddualsimplex_minbeta = 1.0E-4;
static double reviseddualsimplex_maxudecay = 0.001;
static double reviseddualsimplex_shiftlen = 1.0E-12;
static double reviseddualsimplex_alphatrigger = 1.0E8*ae_machineepsilon;
static double reviseddualsimplex_alphatrigger2 = 0.001;
static ae_int_t reviseddualsimplex_ssinvalid = 0;
static ae_int_t reviseddualsimplex_ssvalidxn = 1;
static ae_int_t reviseddualsimplex_ssvalid = 2;
static ae_int_t reviseddualsimplex_ccfixed = 0;
static ae_int_t reviseddualsimplex_cclower = 1;
static ae_int_t reviseddualsimplex_ccupper = 2;
static ae_int_t reviseddualsimplex_ccrange = 3;
static ae_int_t reviseddualsimplex_ccfree = 4;
static ae_int_t reviseddualsimplex_ccinfeasible = 5;
static void reviseddualsimplex_subprobleminit(ae_int_t n,
     dualsimplexsubproblem* s,
     ae_state *_state);
static void reviseddualsimplex_subprobleminitphase1(const dualsimplexsubproblem* s0,
     const dualsimplexbasis* basis,
     dualsimplexsubproblem* s1,
     ae_state *_state);
static void reviseddualsimplex_subprobleminitphase3(const dualsimplexsubproblem* s0,
     dualsimplexsubproblem* s1,
     ae_state *_state);
static void reviseddualsimplex_subprobleminferinitialxn(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_state *_state);
static void reviseddualsimplex_subproblemhandlexnupdate(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_state *_state);
static double reviseddualsimplex_initialdualfeasibilitycorrection(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_shifting(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dssvector* alphar,
     double delta,
     ae_int_t q,
     double alpharpiv,
     double* thetad,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_pricingstep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_bool phase1pricing,
     ae_int_t* p,
     ae_int_t* r,
     double* delta,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_btranstep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_int_t r,
     dssvector* rhor,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_pivotrowstep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dssvector* rhor,
     dssvector* alphar,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_ftranstep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dssvector* rhor,
     ae_int_t q,
     /* Real    */ ae_vector* alphaq,
     /* Real    */ ae_vector* alphaqim,
     /* Real    */ ae_vector* tau,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_ratiotest(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dssvector* alphar,
     double delta,
     ae_int_t p,
     ae_int_t* q,
     double* alpharpiv,
     double* thetad,
     /* Integer */ ae_vector* possibleflips,
     ae_int_t* possibleflipscnt,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_updatestep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_int_t p,
     ae_int_t q,
     ae_int_t r,
     double delta,
     double alphapiv,
     double thetap,
     double thetad,
     /* Real    */ const ae_vector* alphaq,
     /* Real    */ const ae_vector* alphaqim,
     const dssvector* alphar,
     /* Real    */ const ae_vector* tau,
     /* Integer */ const ae_vector* possiblealpharflips,
     ae_int_t possiblealpharflipscnt,
     const dualsimplexsettings* settings,
     ae_state *_state);
static ae_bool reviseddualsimplex_refactorizationrequired(const dualsimplexstate* state,
     const dualsimplexsubproblem* s,
     ae_int_t q,
     double alpharpiv,
     ae_int_t r,
     double alphaqpiv,
     ae_state *_state);
static void reviseddualsimplex_cacheboundinfo(dualsimplexsubproblem* s,
     ae_int_t i,
     ae_int_t k,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_solvesubproblemdual(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_bool isphase1,
     const dualsimplexsettings* settings,
     ae_int_t* info,
     ae_state *_state);
static void reviseddualsimplex_solvesubproblemprimal(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dualsimplexsettings* settings,
     ae_int_t* info,
     ae_state *_state);
static void reviseddualsimplex_invokephase1(dualsimplexstate* state,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_dssoptimizewrk(dualsimplexstate* state,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_solveboxonly(dualsimplexstate* state,
     ae_state *_state);
static void reviseddualsimplex_setzeroxystats(dualsimplexstate* state,
     ae_state *_state);
static void reviseddualsimplex_basisinit(ae_int_t ns,
     ae_int_t m,
     dualsimplexbasis* s,
     ae_state *_state);
static void reviseddualsimplex_basisclearstats(dualsimplexbasis* s,
     ae_state *_state);
static ae_bool reviseddualsimplex_basistryresize(dualsimplexbasis* s,
     ae_int_t newm,
     const sparsematrix* at,
     const dualsimplexsettings* settings,
     ae_state *_state);
static double reviseddualsimplex_basisminimumdiagonalelement(const dualsimplexbasis* s,
     ae_state *_state);
static void reviseddualsimplex_basisexportto(const dualsimplexbasis* s0,
     dualsimplexbasis* s1,
     ae_state *_state);
static ae_bool reviseddualsimplex_basistryimportfrom(dualsimplexbasis* s0,
     const dualsimplexbasis* s1,
     const sparsematrix* at,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_basisfreshtrf(dualsimplexbasis* s,
     const sparsematrix* at,
     const dualsimplexsettings* settings,
     ae_state *_state);
static double reviseddualsimplex_basisfreshtrfunsafe(dualsimplexbasis* s,
     const sparsematrix* at,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_basisrequestweights(dualsimplexbasis* s,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_basisupdatetrf(dualsimplexbasis* s,
     const sparsematrix* at,
     ae_int_t p,
     ae_int_t q,
     /* Real    */ const ae_vector* alphaq,
     /* Real    */ const ae_vector* alphaqim,
     ae_int_t r,
     /* Real    */ const ae_vector* tau,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_basissolve(const dualsimplexbasis* s,
     /* Real    */ const ae_vector* r,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* tmpx,
     ae_state *_state);
static void reviseddualsimplex_basissolvex(const dualsimplexbasis* s,
     /* Real    */ const ae_vector* r,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* xim,
     ae_bool needintermediate,
     /* Real    */ ae_vector* tx,
     ae_state *_state);
static void reviseddualsimplex_basissolvet(const dualsimplexbasis* s,
     /* Real    */ const ae_vector* r,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* tx,
     ae_state *_state);
static void reviseddualsimplex_computeanxn(const dualsimplexstate* state,
     const dualsimplexsubproblem* subproblem,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static void reviseddualsimplex_computeantv(const dualsimplexstate* state,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* r,
     ae_state *_state);
static ae_bool reviseddualsimplex_hasbndl(const dualsimplexsubproblem* subproblem,
     ae_int_t i,
     ae_state *_state);
static ae_bool reviseddualsimplex_hasbndu(const dualsimplexsubproblem* subproblem,
     ae_int_t i,
     ae_state *_state);
static ae_bool reviseddualsimplex_isfree(const dualsimplexsubproblem* subproblem,
     ae_int_t i,
     ae_state *_state);
static void reviseddualsimplex_downgradestate(dualsimplexsubproblem* subproblem,
     ae_int_t s,
     ae_state *_state);
static double reviseddualsimplex_dualfeasibilityerror(const dualsimplexstate* state,
     const dualsimplexsubproblem* s,
     ae_state *_state);
static ae_bool reviseddualsimplex_isdualfeasible(const dualsimplexstate* state,
     const dualsimplexsubproblem* s,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_pivottobwd(/* Integer */ const ae_vector* p,
     ae_int_t m,
     /* Integer */ ae_vector* bwd,
     ae_state *_state);
static void reviseddualsimplex_inversecyclicpermutation(/* Integer */ ae_vector* bwd,
     ae_int_t m,
     ae_int_t d,
     /* Integer */ ae_vector* tmpi,
     ae_state *_state);
static void reviseddualsimplex_offloadbasiccomponents(dualsimplexsubproblem* s,
     const dualsimplexbasis* basis,
     const dualsimplexsettings* settings,
     ae_state *_state);
static void reviseddualsimplex_recombinebasicnonbasicx(dualsimplexsubproblem* s,
     const dualsimplexbasis* basis,
     ae_state *_state);
static void reviseddualsimplex_setxydstats(const dualsimplexstate* state,
     const dualsimplexsubproblem* s,
     const dualsimplexbasis* basis,
     apbuffers* buffers,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     /* Integer */ ae_vector* stats,
     ae_state *_state);
static void reviseddualsimplex_dvalloc(dssvector* x,
     ae_int_t n,
     ae_state *_state);
static void reviseddualsimplex_dvinit(dssvector* x,
     ae_int_t n,
     ae_state *_state);
static void reviseddualsimplex_dvdensetosparse(dssvector* x,
     ae_state *_state);
static void reviseddualsimplex_dvsparsetodense(dssvector* x,
     ae_state *_state);
static double reviseddualsimplex_sparsityof(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
static void reviseddualsimplex_updateavgcounter(double v,
     double* acc,
     ae_int_t* cnt,
     ae_state *_state);


/*$ Body $*/


void dsssettingsinit(dualsimplexsettings* settings, ae_state *_state)
{


    settings->xtolabs = 1.0E-6;
    settings->dtolabs = 1.0E-6;
    settings->xtolrelabs = 0.01;
    settings->pivottol = (double)10*ae_sqrt(ae_machineepsilon, _state);
    settings->perturbmag = (double)10*settings->pivottol;
    settings->maxtrfage = reviseddualsimplex_defaultmaxtrfage;
    settings->trftype = 3;
    settings->ratiotest = 1;
    settings->pricing = 1;
    settings->shifting = 2;
}


/*************************************************************************
This function initializes DSS structure. Previously  allocated  memory  is
reused as much as possible.

Default state of the problem is zero cost vector, all variables are  fixed
at zero.

  -- ALGLIB --
     Copyright 01.07.2018 by Bochkanov Sergey
*************************************************************************/
void dssinit(ae_int_t n, dualsimplexstate* s, ae_state *_state)
{
    ae_int_t i;


    ae_assert(n>0, "DSSInit: N<=0", _state);
    s->ns = n;
    s->m = 0;
    rvectorgrowto(&s->rawbndl, n, _state);
    rvectorgrowto(&s->rawbndu, n, _state);
    for(i=0; i<=n-1; i++)
    {
        s->rawbndl.ptr.p_double[i] = 0.0;
        s->rawbndu.ptr.p_double[i] = 0.0;
    }
    reviseddualsimplex_subprobleminit(n, &s->primary, _state);
    reviseddualsimplex_basisinit(n, 0, &s->basis, _state);
    rvectorgrowto(&s->repx, n, _state);
    rvectorgrowto(&s->replagbc, n, _state);
    ivectorgrowto(&s->repstats, n, _state);
    for(i=0; i<=n-1; i++)
    {
        s->repx.ptr.p_double[i] = 0.0;
        s->repstats.ptr.p_int[i] = 1;
    }
    s->dotrace = ae_false;
    s->dodetailedtrace = ae_false;
    s->dotimers = ae_false;
}


/*************************************************************************
This function specifies LP problem

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    
    BndL    -   lower bounds, array[N].
    BndU    -   upper bounds, array[N].
    
    DenseA  -   dense array[K,N], dense linear constraints (not supported
                in present version)
    SparseA -   sparse linear constraints, sparsematrix[K,N] in CRS format
    AKind   -   type of A: 0 for dense, 1 for sparse
    AL, AU  -   lower and upper bounds, array[K]
    K       -   number of equality/inequality constraints, K>=0.
    
    ProposedBasis- basis to import from (if BasisType=2)
    BasisInitType-  what to do with basis:
                * 0 - set new basis to all-logicals
                * 1 - try to reuse previous basis as much as possible
                * 2 - try to import basis from ProposedBasis
    Settings-   algorithm settings

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void dsssetproblem(dualsimplexstate* state,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_matrix* densea,
     const sparsematrix* sparsea,
     ae_int_t akind,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     const dualsimplexbasis* proposedbasis,
     ae_int_t basisinittype,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t offs;
    ae_int_t ns;
    ae_int_t j0;
    ae_int_t j1;
    ae_bool processed;
    ae_int_t oldm;
    ae_bool basisinitialized;
    double v;


    ns = state->primary.ns;
    oldm = state->primary.m;
    
    /*
     * Integrity checks
     */
    ae_assert(bndl->cnt>=ns, "DSSSetProblem: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=ns, "DSSSetProblem: Length(BndU)<N", _state);
    ae_assert(c->cnt>=ns, "SubproblemSetCost: Length(C)<N", _state);
    ae_assert(isfinitevector(c, ns, _state), "SubproblemSetCost: C contains infinite or NaN elements", _state);
    ae_assert(akind==0||akind==1, "DSSSetProblem: incorrect AKind", _state);
    ae_assert((basisinittype==0||basisinittype==1)||basisinittype==2, "DSSSetProblem: incorrect BasisInitType", _state);
    ae_assert(k>=0, "DSSSetProblem: K<0", _state);
    if( k>0&&akind==1 )
    {
        ae_assert(sparsea->m==k, "DSSSetProblem: rows(A)<>K", _state);
        ae_assert(sparsea->n==ns, "DSSSetProblem: cols(A)<>N", _state);
    }
    
    /*
     * Downgrade state
     */
    reviseddualsimplex_downgradestate(&state->primary, reviseddualsimplex_ssinvalid, _state);
    
    /*
     * Reallocate storage
     */
    rvectorgrowto(&state->primary.bndl, ns+k, _state);
    rvectorgrowto(&state->primary.bndu, ns+k, _state);
    ivectorgrowto(&state->primary.bndt, ns+k, _state);
    rvectorgrowto(&state->primary.rawc, ns+k, _state);
    rvectorgrowto(&state->primary.effc, ns+k, _state);
    rvectorgrowto(&state->primary.xa, ns+k, _state);
    rvectorgrowto(&state->primary.d, ns+k, _state);
    rvectorgrowto(&state->primary.xb, k, _state);
    rvectorgrowto(&state->primary.bndlb, k, _state);
    rvectorgrowto(&state->primary.bndub, k, _state);
    ivectorgrowto(&state->primary.bndtb, k, _state);
    rvectorgrowto(&state->primary.bndtollb, k, _state);
    rvectorgrowto(&state->primary.bndtolub, k, _state);
    
    /*
     * Save original problem formulation
     */
    state->ns = ns;
    state->m = k;
    for(i=0; i<=ns-1; i++)
    {
        state->rawbndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->rawbndu.ptr.p_double[i] = bndu->ptr.p_double[i];
    }
    
    /*
     * Setup cost, scale and box constraints
     */
    rsetv(ns+k, 0.0, &state->primary.rawc, _state);
    rsetv(ns+k, 0.0, &state->primary.effc, _state);
    for(i=0; i<=ns-1; i++)
    {
        state->primary.rawc.ptr.p_double[i] = c->ptr.p_double[i];
        state->primary.effc.ptr.p_double[i] = c->ptr.p_double[i];
    }
    for(i=0; i<=ns-1; i++)
    {
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "DSSSetProblem: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "DSSSetProblem: BndU contains NAN or -INF", _state);
        state->primary.bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->primary.bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        
        /*
         * Set bound type
         */
        if( ae_isfinite(bndl->ptr.p_double[i], _state)&&ae_isfinite(bndu->ptr.p_double[i], _state) )
        {
            if( ae_fp_greater(bndl->ptr.p_double[i],bndu->ptr.p_double[i]) )
            {
                state->primary.bndt.ptr.p_int[i] = reviseddualsimplex_ccinfeasible;
            }
            if( ae_fp_less(bndl->ptr.p_double[i],bndu->ptr.p_double[i]) )
            {
                state->primary.bndt.ptr.p_int[i] = reviseddualsimplex_ccrange;
            }
            if( ae_fp_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]) )
            {
                state->primary.bndt.ptr.p_int[i] = reviseddualsimplex_ccfixed;
            }
            continue;
        }
        if( ae_isfinite(bndl->ptr.p_double[i], _state)&&!ae_isfinite(bndu->ptr.p_double[i], _state) )
        {
            state->primary.bndt.ptr.p_int[i] = reviseddualsimplex_cclower;
            continue;
        }
        if( !ae_isfinite(bndl->ptr.p_double[i], _state)&&ae_isfinite(bndu->ptr.p_double[i], _state) )
        {
            state->primary.bndt.ptr.p_int[i] = reviseddualsimplex_ccupper;
            continue;
        }
        ae_assert(ae_isneginf(bndl->ptr.p_double[i], _state)&&ae_isposinf(bndu->ptr.p_double[i], _state), "DSSSetProblem: integrity check failed", _state);
        state->primary.bndt.ptr.p_int[i] = reviseddualsimplex_ccfree;
    }
    
    /*
     * Quick exit if no linear constraints is present
     */
    if( k==0 )
    {
        state->primary.m = 0;
        reviseddualsimplex_basisinit(state->primary.ns, state->primary.m, &state->basis, _state);
        return;
    }
    
    /*
     * Extend A with structural terms and transpose it:
     * * allocate place for A^T extended with logical part.
     * * copy with transposition
     * * perform integrity check for array sizes
     * * manually append new items
     * * update DIdx/UIdx
     */
    processed = ae_false;
    state->primary.m = k;
    if( akind==0 )
    {
        ae_assert(ae_false, "DSSSetProblem: does not support dense inputs yet", _state);
    }
    if( akind==1 )
    {
        
        /*
         * Transpose constraints matrix, apply column and row scaling.
         * Extend it with identity submatrix.
         *
         * NOTE: in order to improve stability of LU factorization we
         *       normalize rows using 2-norm, not INF-norm. Having rows
         *       normalized with 2-norm makes every element less than
         *       1.0 in magnitude, which allows us later to move logical
         *       columns to the beginning of LU factors without loosing
         *       stability.
         */
        rvectorsetlengthatleast(&state->at.vals, sparsea->ridx.ptr.p_int[k]+k, _state);
        ivectorsetlengthatleast(&state->at.idx, sparsea->ridx.ptr.p_int[k]+k, _state);
        ivectorsetlengthatleast(&state->at.ridx, ns+k+1, _state);
        ivectorsetlengthatleast(&state->at.didx, ns+k, _state);
        ivectorsetlengthatleast(&state->at.uidx, ns+k, _state);
        sparsecopytransposecrsbuf(sparsea, &state->at, _state);
        rvectorsetlengthatleast(&state->rowscales, k, _state);
        for(i=0; i<=k-1; i++)
        {
            state->rowscales.ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=ns-1; i++)
        {
            j0 = state->at.ridx.ptr.p_int[i];
            j1 = state->at.ridx.ptr.p_int[i+1]-1;
            for(j=j0; j<=j1; j++)
            {
                v = state->at.vals.ptr.p_double[j];
                jj = state->at.idx.ptr.p_int[j];
                state->at.vals.ptr.p_double[j] = v;
                state->rowscales.ptr.p_double[jj] = state->rowscales.ptr.p_double[jj]+v*v;
            }
        }
        rvectorsetlengthatleast(&state->tmp0, k, _state);
        for(i=0; i<=k-1; i++)
        {
            state->rowscales.ptr.p_double[i] = coalesce(ae_sqrt(state->rowscales.ptr.p_double[i], _state), (double)(1), _state);
            state->tmp0.ptr.p_double[i] = (double)1/state->rowscales.ptr.p_double[i];
        }
        for(i=0; i<=ns-1; i++)
        {
            j0 = state->at.ridx.ptr.p_int[i];
            j1 = state->at.ridx.ptr.p_int[i+1]-1;
            for(j=j0; j<=j1; j++)
            {
                state->at.vals.ptr.p_double[j] = state->at.vals.ptr.p_double[j]*state->tmp0.ptr.p_double[state->at.idx.ptr.p_int[j]];
            }
        }
        ae_assert(state->at.vals.cnt>=sparsea->ridx.ptr.p_int[k]+k, "DSSSetProblem: integrity check failed", _state);
        ae_assert(state->at.idx.cnt>=sparsea->ridx.ptr.p_int[k]+k, "DSSSetProblem: integrity check failed", _state);
        ae_assert(state->at.ridx.cnt>=ns+k+1, "DSSSetProblem: integrity check failed", _state);
        ae_assert(state->at.didx.cnt>=ns+k, "DSSSetProblem: integrity check failed", _state);
        ae_assert(state->at.uidx.cnt>=ns+k, "DSSSetProblem: integrity check failed", _state);
        offs = state->at.ridx.ptr.p_int[ns];
        for(i=0; i<=k-1; i++)
        {
            state->at.vals.ptr.p_double[offs+i] = -1.0;
            state->at.idx.ptr.p_int[offs+i] = i;
            state->at.ridx.ptr.p_int[ns+i+1] = state->at.ridx.ptr.p_int[ns+i]+1;
            state->at.ninitialized = state->at.ninitialized+1;
        }
        state->at.m = state->at.m+k;
        sparseinitduidx(&state->at, _state);
        sparsecopytransposecrsbuf(&state->at, &state->a, _state);
        processed = ae_true;
    }
    ae_assert(processed, "DSSSetProblem: integrity check failed (akind)", _state);
    
    /*
     * Copy AL, AU to BndL/BndT
     */
    for(i=0; i<=k-1; i++)
    {
        ae_assert(ae_isfinite(al->ptr.p_double[i], _state)||ae_isneginf(al->ptr.p_double[i], _state), "DSSSetProblem: AL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(au->ptr.p_double[i], _state)||ae_isposinf(au->ptr.p_double[i], _state), "DSSSetProblem: AU contains NAN or -INF", _state);
        state->primary.bndl.ptr.p_double[ns+i] = al->ptr.p_double[i]/state->rowscales.ptr.p_double[i];
        state->primary.bndu.ptr.p_double[ns+i] = au->ptr.p_double[i]/state->rowscales.ptr.p_double[i];
        
        /*
         * Set bound type
         */
        if( ae_isfinite(al->ptr.p_double[i], _state)&&ae_isfinite(au->ptr.p_double[i], _state) )
        {
            if( ae_fp_greater(al->ptr.p_double[i],au->ptr.p_double[i]) )
            {
                state->primary.bndt.ptr.p_int[ns+i] = reviseddualsimplex_ccinfeasible;
            }
            if( ae_fp_less(al->ptr.p_double[i],au->ptr.p_double[i]) )
            {
                state->primary.bndt.ptr.p_int[ns+i] = reviseddualsimplex_ccrange;
            }
            if( ae_fp_eq(al->ptr.p_double[i],au->ptr.p_double[i]) )
            {
                state->primary.bndt.ptr.p_int[ns+i] = reviseddualsimplex_ccfixed;
            }
            continue;
        }
        if( ae_isfinite(al->ptr.p_double[i], _state)&&!ae_isfinite(au->ptr.p_double[i], _state) )
        {
            state->primary.bndt.ptr.p_int[ns+i] = reviseddualsimplex_cclower;
            continue;
        }
        if( !ae_isfinite(al->ptr.p_double[i], _state)&&ae_isfinite(au->ptr.p_double[i], _state) )
        {
            state->primary.bndt.ptr.p_int[ns+i] = reviseddualsimplex_ccupper;
            continue;
        }
        ae_assert(ae_isneginf(al->ptr.p_double[i], _state)&&ae_isposinf(au->ptr.p_double[i], _state), "DSSSetProblem: integrity check faoled", _state);
        state->primary.bndt.ptr.p_int[ns+i] = reviseddualsimplex_ccfree;
    }
    
    /*
     * Depending on BasisInitType either start from all-logical basis
     * or try to reuse already existing basis.
     *
     * NOTE: current version does not support basis shrinkage, only
     *       growing basis can be reused.
     */
    basisinitialized = ae_false;
    if( basisinittype==2 )
    {
        
        /*
         * Import basis from one proposed by caller
         */
        ae_assert(proposedbasis->ns==state->primary.ns, "DSSSetProblemX: unable to import basis, sizes do not match", _state);
        ae_assert(proposedbasis->m==state->primary.m, "DSSSetProblemX: unable to import basis, sizes do not match", _state);
        basisinitialized = reviseddualsimplex_basistryimportfrom(&state->basis, proposedbasis, &state->at, settings, _state);
    }
    if( basisinittype==1&&state->primary.m>=oldm )
    {
        
        /*
         * New rows were added, try to reuse previous basis
         */
        for(i=oldm; i<=state->primary.m-1; i++)
        {
            state->primary.rawc.ptr.p_double[ns+i] = 0.0;
            state->primary.effc.ptr.p_double[ns+i] = 0.0;
            state->primary.xa.ptr.p_double[ns+i] = 0.0;
            state->primary.d.ptr.p_double[ns+i] = 0.0;
        }
        basisinitialized = reviseddualsimplex_basistryresize(&state->basis, state->primary.m, &state->at, settings, _state);
    }
    if( !basisinitialized )
    {
        
        /*
         * Straightforward code for all-logicals basis
         */
        for(i=0; i<=k-1; i++)
        {
            state->primary.rawc.ptr.p_double[ns+i] = 0.0;
            state->primary.effc.ptr.p_double[ns+i] = 0.0;
            state->primary.xa.ptr.p_double[ns+i] = 0.0;
            state->primary.d.ptr.p_double[ns+i] = 0.0;
        }
        reviseddualsimplex_basisinit(state->primary.ns, state->primary.m, &state->basis, _state);
        reviseddualsimplex_basisfreshtrf(&state->basis, &state->at, settings, _state);
    }
    rvectorgrowto(&state->replaglc, state->primary.m, _state);
    ivectorgrowto(&state->repstats, state->primary.ns+state->primary.m, _state);
}


/*************************************************************************
This function exports basis from the primary (phase II) subproblem.

INPUT PARAMETERS:
    State   -   structure
    
OUTPUT PARAMETERS
    Basis   -   current basis exported (no factorization, only set of
                basis/nonbasic variables)

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void dssexportbasis(const dualsimplexstate* state,
     dualsimplexbasis* basis,
     ae_state *_state)
{


    reviseddualsimplex_basisexportto(&state->basis, basis, _state);
}


/*************************************************************************
This function solves LP problem with dual simplex solver.

INPUT PARAMETERS:
    State   -   state
    
Solution results can be found in fields  of  State  which  are  explicitly
declared as accessible by external code.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void dssoptimize(dualsimplexstate* state,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nx;
    double v;
    ae_int_t cnt1;
    ae_int_t cnt2;
    ae_int_t cntfx;
    ae_int_t cntfr;
    ae_int_t cntif;
    ae_int_t ttotal;


    nx = state->primary.ns+state->primary.m;
    ttotal = 0;
    
    /*
     * Trace settings
     */
    state->dotrace = ae_is_trace_enabled("DSS");
    state->dodetailedtrace = state->dotrace&&ae_is_trace_enabled("DSS.DETAILED");
    state->dotimers = ae_is_trace_enabled("TIMERS.DSS");
    
    /*
     * Init report fields
     */
    state->repiterationscount = 0;
    state->repiterationscount1 = 0;
    state->repiterationscount2 = 0;
    state->repiterationscount3 = 0;
    state->repterminationtype = 1;
    state->repphase1time = 0;
    state->repphase2time = 0;
    state->repphase3time = 0;
    state->repdualpricingtime = 0;
    state->repdualbtrantime = 0;
    state->repdualpivotrowtime = 0;
    state->repdualratiotesttime = 0;
    state->repdualftrantime = 0;
    state->repdualupdatesteptime = 0;
    state->repfillpivotrow = (double)(0);
    state->repfillpivotrowcnt = 0;
    state->repfillrhor = (double)(0);
    state->repfillrhorcnt = 0;
    state->repfilldensemu = (double)(0);
    state->repfilldensemucnt = 0;
    reviseddualsimplex_basisclearstats(&state->basis, _state);
    
    /*
     * Setup timer (if needed)
     */
    if( state->dotimers )
    {
        ttotal = ae_tickcount();
    }
    
    /*
     * Trace output (if needed)
     */
    if( state->dotrace||state->dotimers )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  DUAL SIMPLEX SOLVER STARTED                                                                   //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("> problem size:\n");
        ae_trace("N         = %12d (variables)\n",
            (int)(state->primary.ns));
        ae_trace("M         = %12d (constraints)\n",
            (int)(state->primary.m));
    }
    if( state->dotrace )
    {
        ae_trace("> variable stats:\n");
        if( state->dodetailedtrace )
        {
        }
        cnt1 = 0;
        cnt2 = 0;
        cntfx = 0;
        cntfr = 0;
        cntif = 0;
        for(i=0; i<=state->primary.ns-1; i++)
        {
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_cclower||state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccupper )
            {
                inc(&cnt1, _state);
            }
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccrange )
            {
                inc(&cnt2, _state);
            }
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccfixed )
            {
                inc(&cntfx, _state);
            }
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccfree )
            {
                inc(&cntfr, _state);
            }
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccinfeasible )
            {
                inc(&cntif, _state);
            }
        }
        ae_trace("UBnd/LBnd   = %12d\n",
            (int)(cnt1));
        ae_trace("Range       = %12d\n",
            (int)(cnt2));
        ae_trace("Fixed       = %12d\n",
            (int)(cntfx));
        ae_trace("Free        = %12d\n",
            (int)(cntfr));
        ae_trace("Infeas      = %12d\n",
            (int)(cntif));
        ae_trace("> constraint stats:\n");
        if( state->dodetailedtrace )
        {
        }
        cnt1 = 0;
        cnt2 = 0;
        cntfx = 0;
        cntfr = 0;
        cntif = 0;
        for(i=state->primary.ns-1; i<=nx-1; i++)
        {
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_cclower||state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccupper )
            {
                inc(&cnt1, _state);
            }
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccrange )
            {
                inc(&cnt2, _state);
            }
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccfixed )
            {
                inc(&cntfx, _state);
            }
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccfree )
            {
                inc(&cntfr, _state);
            }
            if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccinfeasible )
            {
                inc(&cntif, _state);
            }
        }
        ae_trace("ubnd/lbnd   = %12d\n",
            (int)(cnt1));
        ae_trace("range       = %12d\n",
            (int)(cnt2));
        ae_trace("fixed       = %12d\n",
            (int)(cntfx));
        ae_trace("free        = %12d\n",
            (int)(cntfr));
        ae_trace("infeas      = %12d\n",
            (int)(cntif));
        v = (double)(0);
        for(i=0; i<=state->primary.ns-1; i++)
        {
            if( ae_isfinite(state->primary.bndl.ptr.p_double[i], _state) )
            {
                v = ae_maxreal(v, ae_fabs(state->primary.bndl.ptr.p_double[i], _state), _state);
            }
        }
        ae_trace("|BndL|      = %0.3e\n",
            (double)(v));
        v = (double)(0);
        for(i=0; i<=state->primary.ns-1; i++)
        {
            if( ae_isfinite(state->primary.bndu.ptr.p_double[i], _state) )
            {
                v = ae_maxreal(v, ae_fabs(state->primary.bndu.ptr.p_double[i], _state), _state);
            }
        }
        ae_trace("|BndU|      = %0.3e\n",
            (double)(v));
        v = (double)(0);
        for(i=state->primary.ns; i<=nx-1; i++)
        {
            if( ae_isfinite(state->primary.bndl.ptr.p_double[i], _state) )
            {
                v = ae_maxreal(v, ae_fabs(state->primary.bndl.ptr.p_double[i], _state), _state);
            }
        }
        ae_trace("|AL|        = %0.3e\n",
            (double)(v));
        v = (double)(0);
        for(i=state->primary.ns; i<=nx-1; i++)
        {
            if( ae_isfinite(state->primary.bndu.ptr.p_double[i], _state) )
            {
                v = ae_maxreal(v, ae_fabs(state->primary.bndu.ptr.p_double[i], _state), _state);
            }
        }
        ae_trace("|AU|        = %0.3e\n",
            (double)(v));
    }
    
    /*
     * Call actual workhorse function
     */
    reviseddualsimplex_dssoptimizewrk(state, settings, _state);
    
    /*
     * Print reports
     */
    if( state->dotrace )
    {
        ae_trace("\n");
        ae_trace("****************************************************************************************************\n");
        ae_trace("*   PRINTING ITERATION STATISTICS                                                                  *\n");
        ae_trace("****************************************************************************************************\n");
        ae_trace("> iteration counts:\n");
        ae_trace("Phase 1     = %12d\n",
            (int)(state->repiterationscount1));
        ae_trace("Phase 2     = %12d\n",
            (int)(state->repiterationscount2));
        ae_trace("Phase 3     = %12d\n",
            (int)(state->repiterationscount3));
        ae_trace("> factorization statistics:\n");
        ae_trace("FactCnt     = %12d (LU factorizations)\n",
            (int)(state->basis.statfact));
        ae_trace("UpdtCnt     = %12d (LU updates)\n",
            (int)(state->basis.statupdt));
        ae_trace("RefactPeriod= %12.1f (average refactorization interval)\n",
            (double)((double)(state->basis.statfact+state->basis.statupdt)/coalesce((double)(state->basis.statfact), (double)(1), _state)));
        ae_trace("LU-NZR      = %12.1f (average LU nonzeros per row)\n",
            (double)(state->basis.statoffdiag/(coalesce((double)(state->m), (double)(1), _state)*coalesce((double)(state->basis.statfact+state->basis.statupdt), (double)(1), _state))));
        ae_trace("> sparsity counters (average fill factors):\n");
        if( state->dodetailedtrace )
        {
            ae_trace("RhoR        = %12.4f (BTran result)\n",
                (double)(state->repfillrhor/coalesce((double)(state->repfillrhorcnt), (double)(1), _state)));
            ae_trace("AlphaR      = %12.4f (pivot row)\n",
                (double)(state->repfillpivotrow/coalesce((double)(state->repfillpivotrowcnt), (double)(1), _state)));
            if( state->basis.trftype==3 )
            {
                ae_trace("Mu          = %12.4f (Forest-Tomlin factor)\n",
                    (double)(state->repfilldensemu/coalesce((double)(state->repfilldensemucnt), (double)(1), _state)));
            }
        }
        else
        {
            ae_trace("...skipped, need DUALSIMPLEX.DETAILED trace tag\n");
        }
    }
    if( state->dotimers )
    {
        ttotal = ae_tickcount()-ttotal;
        ae_trace("\n");
        ae_trace("****************************************************************************************************\n");
        ae_trace("*   PRINTING DUAL SIMPLEX TIMERS                                                                   *\n");
        ae_trace("****************************************************************************************************\n");
        ae_trace("> total time:\n");
        ae_trace("Time        = %12d ms\n",
            (int)(ttotal));
        ae_trace("> time by phase:\n");
        ae_trace("Phase 1     = %12d ms\n",
            (int)(state->repphase1time));
        ae_trace("Phase 2     = %12d ms\n",
            (int)(state->repphase2time));
        ae_trace("Phase 3     = %12d ms\n",
            (int)(state->repphase3time));
        ae_trace("> time by step (dual phases 1 and 2):\n");
        ae_trace("Pricing     = %12d ms\n",
            (int)(state->repdualpricingtime));
        ae_trace("BTran       = %12d ms\n",
            (int)(state->repdualbtrantime));
        ae_trace("PivotRow    = %12d ms\n",
            (int)(state->repdualpivotrowtime));
        ae_trace("RatioTest   = %12d ms\n",
            (int)(state->repdualratiotesttime));
        ae_trace("FTran       = %12d ms\n",
            (int)(state->repdualftrantime));
        ae_trace("Update      = %12d ms\n",
            (int)(state->repdualupdatesteptime));
    }
}


/*************************************************************************
This function initializes subproblem structure. Previously allocated memory
is reused as much as possible.

Default state of the problem is zero cost vector, all variables are  fixed
at zero, linear constraint matrix is zero.

  -- ALGLIB --
     Copyright 01.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_subprobleminit(ae_int_t n,
     dualsimplexsubproblem* s,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(n>0, "SubproblemInit: N<=0", _state);
    s->ns = n;
    s->m = 0;
    s->state = reviseddualsimplex_ssinvalid;
    rvectorsetlengthatleast(&s->xa, n, _state);
    rvectorsetlengthatleast(&s->xb, 0, _state);
    rvectorsetlengthatleast(&s->d, n, _state);
    rvectorsetlengthatleast(&s->rawc, n, _state);
    rvectorsetlengthatleast(&s->effc, n, _state);
    rvectorsetlengthatleast(&s->bndl, n, _state);
    rvectorsetlengthatleast(&s->bndu, n, _state);
    ivectorsetlengthatleast(&s->bndt, n, _state);
    for(i=0; i<=n-1; i++)
    {
        s->rawc.ptr.p_double[i] = (double)(0);
        s->effc.ptr.p_double[i] = (double)(0);
        s->bndl.ptr.p_double[i] = (double)(0);
        s->bndu.ptr.p_double[i] = (double)(0);
        s->bndt.ptr.p_int[i] = reviseddualsimplex_ccfixed;
        s->xa.ptr.p_double[i] = 0.0;
        s->d.ptr.p_double[i] = 0.0;
    }
}


/*************************************************************************
This function initializes phase #1 subproblem which minimizes sum of  dual
infeasibilities.  It is required that total count of  non-boxed  non-fixed
variables is at least M.

It splits out basic components of XA[] to XB[]

  -- ALGLIB --
     Copyright 01.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_subprobleminitphase1(const dualsimplexsubproblem* s0,
     const dualsimplexbasis* basis,
     dualsimplexsubproblem* s1,
     ae_state *_state)
{
    ae_int_t i;


    s1->ns = s0->ns;
    s1->m = s0->m;
    copyrealarray(&s0->rawc, &s1->rawc, _state);
    copyrealarray(&s0->effc, &s1->effc, _state);
    copyrealarray(&s0->bndl, &s1->bndl, _state);
    copyrealarray(&s0->bndu, &s1->bndu, _state);
    copyintegerarray(&s0->bndt, &s1->bndt, _state);
    copyrealarray(&s0->xa, &s1->xa, _state);
    copyrealarray(&s0->xb, &s1->xb, _state);
    copyrealarray(&s0->bndlb, &s1->bndlb, _state);
    copyrealarray(&s0->bndub, &s1->bndub, _state);
    copyintegerarray(&s0->bndtb, &s1->bndtb, _state);
    copyrealarray(&s0->bndtollb, &s1->bndtollb, _state);
    copyrealarray(&s0->bndtolub, &s1->bndtolub, _state);
    copyrealarray(&s0->d, &s1->d, _state);
    for(i=0; i<=s1->ns+s1->m-1; i++)
    {
        if( s1->bndt.ptr.p_int[i]==reviseddualsimplex_cclower )
        {
            s1->bndt.ptr.p_int[i] = reviseddualsimplex_ccrange;
            s1->bndl.ptr.p_double[i] = (double)(0);
            s1->bndu.ptr.p_double[i] = (double)(1);
            s1->xa.ptr.p_double[i] = (double)(0);
            continue;
        }
        if( s1->bndt.ptr.p_int[i]==reviseddualsimplex_ccupper )
        {
            s1->bndt.ptr.p_int[i] = reviseddualsimplex_ccrange;
            s1->bndl.ptr.p_double[i] = (double)(-1);
            s1->bndu.ptr.p_double[i] = (double)(0);
            s1->xa.ptr.p_double[i] = (double)(0);
            continue;
        }
        if( s1->bndt.ptr.p_int[i]==reviseddualsimplex_ccfree )
        {
            s1->bndt.ptr.p_int[i] = reviseddualsimplex_ccrange;
            s1->bndl.ptr.p_double[i] = (double)(-1);
            s1->bndu.ptr.p_double[i] = (double)(1);
            if( ae_fp_greater_eq(s1->effc.ptr.p_double[i],(double)(0)) )
            {
                s1->xa.ptr.p_double[i] = (double)(-1);
            }
            else
            {
                s1->xa.ptr.p_double[i] = (double)(1);
            }
            continue;
        }
        s1->bndt.ptr.p_int[i] = reviseddualsimplex_ccfixed;
        s1->bndl.ptr.p_double[i] = (double)(0);
        s1->bndu.ptr.p_double[i] = (double)(0);
        s1->xa.ptr.p_double[i] = (double)(0);
    }
    s1->state = reviseddualsimplex_ssvalidxn;
}


/*************************************************************************
This function initializes phase #3 subproblem which applies primal simplex
method to the result of the phase #2.

It also performs modification of the subproblem in order to ensure that
initial point is primal feasible.

NOTE: this function expects that all components (basic and nonbasic ones)
      are stored in XA[]

  -- ALGLIB --
     Copyright 01.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_subprobleminitphase3(const dualsimplexsubproblem* s0,
     dualsimplexsubproblem* s1,
     ae_state *_state)
{


    s1->ns = s0->ns;
    s1->m = s0->m;
    copyrealarray(&s0->rawc, &s1->rawc, _state);
    copyrealarray(&s0->effc, &s1->effc, _state);
    copyrealarray(&s0->bndl, &s1->bndl, _state);
    copyrealarray(&s0->bndu, &s1->bndu, _state);
    copyintegerarray(&s0->bndt, &s1->bndt, _state);
    copyrealarray(&s0->xa, &s1->xa, _state);
    copyrealarray(&s0->xb, &s1->xb, _state);
    copyrealarray(&s0->bndlb, &s1->bndlb, _state);
    copyrealarray(&s0->bndub, &s1->bndub, _state);
    copyintegerarray(&s0->bndtb, &s1->bndtb, _state);
    copyrealarray(&s0->bndtollb, &s1->bndtollb, _state);
    copyrealarray(&s0->bndtolub, &s1->bndtolub, _state);
    copyrealarray(&s0->d, &s1->d, _state);
    s1->state = reviseddualsimplex_ssvalidxn;
}


/*************************************************************************
This function infers nonbasic variables of X using sign of effective C[].

Only non-basic components of XN are changed; everything else is NOT updated.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_subprobleminferinitialxn(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t ii;
    ae_int_t bndt;


    for(ii=0; ii<=s->ns-1; ii++)
    {
        i = state->basis.nidx.ptr.p_int[ii];
        bndt = s->bndt.ptr.p_int[i];
        if( bndt==reviseddualsimplex_ccfixed||bndt==reviseddualsimplex_ccrange )
        {
            if( s->effc.ptr.p_double[i]>=(double)0 )
            {
                s->xa.ptr.p_double[i] = s->bndl.ptr.p_double[i];
            }
            else
            {
                s->xa.ptr.p_double[i] = s->bndu.ptr.p_double[i];
            }
            continue;
        }
        if( bndt==reviseddualsimplex_cclower )
        {
            s->xa.ptr.p_double[i] = s->bndl.ptr.p_double[i];
            continue;
        }
        if( bndt==reviseddualsimplex_ccupper )
        {
            s->xa.ptr.p_double[i] = s->bndu.ptr.p_double[i];
            continue;
        }
        if( bndt==reviseddualsimplex_ccfree )
        {
            s->xa.ptr.p_double[i] = 0.0;
            continue;
        }
        ae_assert(ae_false, "SubproblemInferInitialXN: integrity check failed (infeasible constraint)", _state);
    }
    s->state = reviseddualsimplex_ssvalidxn;
}


/*************************************************************************
This function infers basic variables of X using values of non-basic vars
and updates reduced cost vector D and target function Z. Sets state age
to zero.

D[] is allocated during computations.

Temporary vectors Tmp0 and Tmp1 are used (reallocated as needed).

NOTE: this function expects that both nonbasic and basic components are
      stored in XA[]. XB[] array is not referenced.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_subproblemhandlexnupdate(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t m;
    ae_int_t nn;


    ae_assert(s->state>=reviseddualsimplex_ssvalidxn, "SubproblemHandleXNUpdate: integrity check failed (XN is not valid)", _state);
    nn = s->ns;
    m = s->m;
    
    /*
     * Compute nonbasic components
     */
    reviseddualsimplex_computeanxn(state, s, &s->xa, &state->tmp0, _state);
    reviseddualsimplex_basissolve(&state->basis, &state->tmp0, &state->tmp1, &state->tmp2, _state);
    for(i=0; i<=m-1; i++)
    {
        s->xa.ptr.p_double[state->basis.idx.ptr.p_int[i]] = -state->tmp1.ptr.p_double[i];
    }
    
    /*
     * Compute D
     */
    for(i=0; i<=m-1; i++)
    {
        state->tmp0.ptr.p_double[i] = s->effc.ptr.p_double[state->basis.idx.ptr.p_int[i]];
    }
    reviseddualsimplex_basissolvet(&state->basis, &state->tmp0, &state->tmp1, &state->tmp2, _state);
    reviseddualsimplex_computeantv(state, &state->tmp1, &s->d, _state);
    for(i=0; i<=nn-1; i++)
    {
        j = state->basis.nidx.ptr.p_int[i];
        s->d.ptr.p_double[j] = s->effc.ptr.p_double[j]-s->d.ptr.p_double[j];
    }
    
    /*
     * Update state validity/age
     */
    s->state = reviseddualsimplex_ssvalid;
}


/*************************************************************************
This function performs initial dual feasibility correction on the subproblem.
It assumes that problem state is at least ssValidXN. After call to this
function the problem state is set to ssValid.

This function returns dual feasibility error after dual feasibility correction.

NOTE: this function expects that both nonbasic and basic components are
      stored in XA[]. XB[] array is not referenced.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static double reviseddualsimplex_initialdualfeasibilitycorrection(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector dummy;
    ae_int_t nn;
    ae_int_t m;
    ae_int_t ii;
    ae_int_t i;
    ae_int_t j;
    ae_bool flipped;
    double v;
    double dj;
    double xj;
    ae_int_t bndt;
    double result;

    ae_frame_make(_state, &_frame_block);
    memset(&dummy, 0, sizeof(dummy));
    ae_vector_init(&dummy, 0, DT_REAL, _state, ae_true);

    nn = s->ns;
    m = s->m;
    ae_assert(s->state>=reviseddualsimplex_ssvalidxn, "InitialDualFeasibilityCorrection: XN is invalid", _state);
    
    /*
     * Prepare
     */
    rvectorsetlengthatleast(&state->dfctmp0, m, _state);
    rvectorsetlengthatleast(&state->dfctmp1, m, _state);
    
    /*
     * Recompute D[] using fresh factorization
     */
    reviseddualsimplex_basisfreshtrf(&state->basis, &state->at, settings, _state);
    for(i=0; i<=m-1; i++)
    {
        state->dfctmp0.ptr.p_double[i] = s->effc.ptr.p_double[state->basis.idx.ptr.p_int[i]];
    }
    reviseddualsimplex_basissolvet(&state->basis, &state->dfctmp0, &state->dfctmp1, &state->dfctmp2, _state);
    reviseddualsimplex_computeantv(state, &state->dfctmp1, &s->d, _state);
    for(i=0; i<=nn-1; i++)
    {
        j = state->basis.nidx.ptr.p_int[i];
        s->d.ptr.p_double[j] = s->effc.ptr.p_double[j]-s->d.ptr.p_double[j];
    }
    
    /*
     * Perform flips for dual-infeasible boxed variables
     */
    result = (double)(0);
    flipped = ae_false;
    for(ii=0; ii<=nn-1; ii++)
    {
        j = state->basis.nidx.ptr.p_int[ii];
        bndt = s->bndt.ptr.p_int[j];
        
        /*
         * Boxed variables, perform DFC
         */
        if( bndt==reviseddualsimplex_ccrange )
        {
            dj = s->d.ptr.p_double[j];
            xj = s->xa.ptr.p_double[j];
            if( xj==s->bndl.ptr.p_double[j]&&dj<(double)0 )
            {
                s->xa.ptr.p_double[j] = s->bndu.ptr.p_double[j];
                flipped = ae_true;
                continue;
            }
            if( xj==s->bndu.ptr.p_double[j]&&dj>(double)0 )
            {
                s->xa.ptr.p_double[j] = s->bndl.ptr.p_double[j];
                flipped = ae_true;
                continue;
            }
            continue;
        }
        
        /*
         * Non-boxed variables, compute dual feasibility error
         */
        if( bndt==reviseddualsimplex_ccfixed )
        {
            continue;
        }
        if( bndt==reviseddualsimplex_cclower )
        {
            v = -s->d.ptr.p_double[j];
            if( v>result )
            {
                result = v;
            }
            continue;
        }
        if( bndt==reviseddualsimplex_ccupper )
        {
            v = s->d.ptr.p_double[j];
            if( v>result )
            {
                result = v;
            }
            continue;
        }
        if( bndt==reviseddualsimplex_ccfree )
        {
            result = ae_maxreal(result, ae_fabs(s->d.ptr.p_double[j], _state), _state);
            continue;
        }
    }
    
    /*
     * Recompute basic components of X[]
     */
    if( flipped||s->state<reviseddualsimplex_ssvalid )
    {
        reviseddualsimplex_computeanxn(state, s, &s->xa, &state->dfctmp0, _state);
        reviseddualsimplex_basissolve(&state->basis, &state->dfctmp0, &state->dfctmp1, &state->dfctmp2, _state);
        for(i=0; i<=m-1; i++)
        {
            s->xa.ptr.p_double[state->basis.idx.ptr.p_int[i]] = -state->dfctmp1.ptr.p_double[i];
        }
    }
    
    /*
     * Update state validity/age
     */
    s->state = reviseddualsimplex_ssvalid;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
This function performs shifting using current algorithm  as  specified  by
settings.shifting.

It accepts following parameters:
* AlphaR - pivot row
* Delta - delta from pricing step
* Q - variable selected by ratio test
* AlphaRPiv - pivot element, Q-th element of  AlphaR  (because  alphaR  is
  stored in compressed format, we can't extract it easily)
* ThetaD - dual step length

If no shifts are necessary, it silently returns. If shifts are  necessary,
it modifies ThetaD, S.D, S.EffC according to shifting algorithm.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_shifting(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dssvector* alphar,
     double delta,
     ae_int_t q,
     double alpharpiv,
     double* thetad,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t dir;
    double sft;
    ae_int_t ii;
    ae_int_t j;
    ae_int_t bndt;


    
    /*
     * No shifts
     */
    if( settings->shifting==0 )
    {
        return;
    }
    if( q<0 )
    {
        return;
    }
    
    /*
     * EXPAND with ThetaD=0
     */
    if( settings->shifting==1 )
    {
        dir = ae_sign(delta, _state);
        if( ae_fp_greater_eq(*thetad*(double)dir,(double)(0)) )
        {
            return;
        }
        s->effc.ptr.p_double[q] = s->effc.ptr.p_double[q]-s->d.ptr.p_double[q];
        s->d.ptr.p_double[q] = (double)(0);
        *thetad = (double)(0);
        return;
    }
    
    /*
     * EXPAND with ThetaD=ShiftLen
     */
    if( settings->shifting==2 )
    {
        dir = ae_sign(delta, _state);
        if( ae_fp_greater(*thetad*(double)dir,(double)(0)) )
        {
            return;
        }
        
        /*
         * Ensure that non-zero step is performed
         */
        *thetad = (double)dir*reviseddualsimplex_shiftlen;
        
        /*
         * Shift Q-th coefficient
         */
        sft = *thetad*((double)dir*alpharpiv)-s->d.ptr.p_double[q];
        s->effc.ptr.p_double[q] = s->effc.ptr.p_double[q]+sft;
        s->d.ptr.p_double[q] = s->d.ptr.p_double[q]+sft;
        
        /*
         * Shift other coefficients
         */
        for(ii=0; ii<=alphar->k-1; ii++)
        {
            j = alphar->idx.ptr.p_int[ii];
            bndt = s->bndt.ptr.p_int[j];
            if( (j==q||bndt==reviseddualsimplex_ccfixed)||bndt==reviseddualsimplex_ccfree )
            {
                continue;
            }
            sft = *thetad*((double)dir*alphar->vals.ptr.p_double[ii])-s->d.ptr.p_double[j];
            
            /*
             * Handle variables at lower bound
             */
            if( bndt==reviseddualsimplex_cclower||(bndt==reviseddualsimplex_ccrange&&s->xa.ptr.p_double[j]==s->bndl.ptr.p_double[j]) )
            {
                sft = sft-settings->dtolabs;
                if( sft>(double)0 )
                {
                    s->effc.ptr.p_double[j] = s->effc.ptr.p_double[j]+sft;
                    s->d.ptr.p_double[j] = s->d.ptr.p_double[j]+sft;
                }
                continue;
            }
            if( bndt==reviseddualsimplex_ccupper||(bndt==reviseddualsimplex_ccrange&&s->xa.ptr.p_double[j]==s->bndu.ptr.p_double[j]) )
            {
                sft = sft+settings->dtolabs;
                if( sft<(double)0 )
                {
                    s->effc.ptr.p_double[j] = s->effc.ptr.p_double[j]+sft;
                    s->d.ptr.p_double[j] = s->d.ptr.p_double[j]+sft;
                }
                continue;
            }
        }
        
        /*
         * Done
         */
        return;
    }
    ae_assert(ae_false, "Shifting: unexpected shifting type", _state);
}


/*************************************************************************
This function performs pricing step

Additional parameters:
* Phase1Pricing - if True, then special Phase #1 restriction is applied to
  leaving variables: only those are eligible which will move to zero bound
  after basis change.
  
  This trick allows to accelerate and stabilize phase #1. See Robert Fourer,
  'Notes on the dual simplex method', draft report, 1994, for more info.

Returns:
* leaving variable index P
* its index R in the basis, in [0,M) range
* Delta - difference between variable value and corresponding bound

NOTE: this function expects that basic components are stored in XB[];
      corresponding entries of XA[] are ignored.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_pricingstep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_bool phase1pricing,
     ae_int_t* p,
     ae_int_t* r,
     double* delta,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t i;
    ae_int_t bi;
    double v;
    double vtarget;
    double xbi;
    double bndl;
    double bndu;
    double vdiff;
    double vtest;
    double invw;
    ae_int_t bndt;
    ae_bool hasboth;
    ae_bool hasl;
    ae_bool hasu;
    ae_int_t t0;

    *p = 0;
    *r = 0;
    *delta = 0.0;

    m = s->m;
    
    /*
     * Integrity checks
     */
    ae_assert(s->state==reviseddualsimplex_ssvalid, "PricingStep: invalid X", _state);
    ae_assert(m>0, "PricingStep: M<=0", _state);
    
    /*
     * Timers
     */
    t0 = 0;
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    
    /*
     * Pricing
     */
    if( settings->pricing==0 )
    {
        
        /*
         * "Most infeasible" pricing
         */
        *p = -1;
        *r = -1;
        *delta = (double)(0);
        vtarget = (double)(0);
        for(i=0; i<=m-1; i++)
        {
            bndt = s->bndtb.ptr.p_int[i];
            hasboth = bndt==3||bndt==0;
            hasl = hasboth||bndt==1;
            hasu = hasboth||bndt==2;
            xbi = s->xb.ptr.p_double[i];
            if( hasl )
            {
                bndl = s->bndlb.ptr.p_double[i];
                vdiff = xbi-bndl;
                v = -vdiff;
                if( v>s->bndtollb.ptr.p_double[i]&&v>vtarget )
                {
                    
                    /*
                     * Special phase 1 pricing: do not choose variables which move to non-zero bound
                     */
                    if( phase1pricing&&!(bndl==0.0) )
                    {
                        continue;
                    }
                    
                    /*
                     * Proceed as usual
                     */
                    *p = state->basis.idx.ptr.p_int[i];
                    *r = i;
                    *delta = vdiff;
                    vtarget = v;
                    continue;
                }
            }
            if( hasu )
            {
                bndu = s->bndub.ptr.p_double[i];
                vdiff = xbi-bndu;
                v = vdiff;
                if( v>s->bndtolub.ptr.p_double[i]&&v>vtarget )
                {
                    
                    /*
                     * Special phase 1 pricing: do not choose variables which move to non-zero bound
                     */
                    if( phase1pricing&&!(bndu==0.0) )
                    {
                        continue;
                    }
                    
                    /*
                     * Proceed as usual
                     */
                    *p = state->basis.idx.ptr.p_int[i];
                    *r = i;
                    *delta = vdiff;
                    vtarget = v;
                    continue;
                }
            }
        }
        
        /*
         * Trace/profile
         */
        if( state->dotrace )
        {
            ae_trace("> pricing: most infeasible variable removed\n");
            ae_trace("P           = %12d (R=%0d)\n",
                (int)(*p),
                (int)(*r));
            ae_trace("Delta       = %12.3e\n",
                (double)(*delta));
        }
        if( state->dotimers )
        {
            state->repdualpricingtime = state->repdualpricingtime+(ae_tickcount()-t0);
        }
        
        /*
         * Done
         */
        return;
    }
    if( settings->pricing==-1||settings->pricing==1 )
    {
        
        /*
         * Dual steepest edge pricing
         */
        reviseddualsimplex_basisrequestweights(&state->basis, settings, _state);
        *p = -1;
        *r = -1;
        *delta = (double)(0);
        vtarget = (double)(0);
        for(i=0; i<=m-1; i++)
        {
            bi = state->basis.idx.ptr.p_int[i];
            bndt = s->bndtb.ptr.p_int[i];
            hasboth = bndt==3||bndt==0;
            hasl = hasboth||bndt==1;
            hasu = hasboth||bndt==2;
            xbi = s->xb.ptr.p_double[i];
            invw = (double)1/state->basis.dseweights.ptr.p_double[i];
            if( hasl )
            {
                bndl = s->bndlb.ptr.p_double[i];
                vdiff = xbi-bndl;
                vtest = vdiff*vdiff*invw;
                if( vdiff<-s->bndtollb.ptr.p_double[i]&&(*p<0||vtest>vtarget) )
                {
                    
                    /*
                     * Special phase 1 pricing: do not choose variables which move to non-zero bound
                     */
                    if( phase1pricing&&!(bndl==0.0) )
                    {
                        continue;
                    }
                    
                    /*
                     * Proceed as usual
                     */
                    *p = bi;
                    *r = i;
                    *delta = vdiff;
                    vtarget = vtest;
                    continue;
                }
            }
            if( hasu )
            {
                bndu = s->bndub.ptr.p_double[i];
                vdiff = xbi-bndu;
                vtest = vdiff*vdiff*invw;
                if( vdiff>s->bndtolub.ptr.p_double[i]&&(*p<0||vtest>vtarget) )
                {
                    
                    /*
                     * Special phase 1 pricing: do not choose variables which move to non-zero bound
                     */
                    if( phase1pricing&&!(bndu==0.0) )
                    {
                        continue;
                    }
                    
                    /*
                     * Proceed as usual
                     */
                    *p = bi;
                    *r = i;
                    *delta = vdiff;
                    vtarget = vtest;
                    continue;
                }
            }
        }
        
        /*
         * Trace/profile
         */
        if( state->dotrace )
        {
            ae_trace("> dual steepest edge pricing: leaving variable found\n");
            ae_trace("P           = %12d  (variable index)\n",
                (int)(*p));
            ae_trace("R           = %12d  (variable index in basis)\n",
                (int)(*r));
            ae_trace("Delta       = %12.3e  (primal infeasibility removed)\n",
                (double)(*delta));
        }
        if( state->dotimers )
        {
            state->repdualpricingtime = state->repdualpricingtime+(ae_tickcount()-t0);
        }
        
        /*
         * Done
         */
        return;
    }
    ae_assert(ae_false, "PricingStep: unknown pricing type", _state);
}


/*************************************************************************
This function performs BTran step

Accepts:
* R, index of the leaving variable in the basis, in [0,M) range

Returns:
* RhoR, array[M], BTran result

  -- ALGLIB --
     Copyright 19.07.2020 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_btranstep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_int_t r,
     dssvector* rhor,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t i;
    ae_int_t t0;


    m = s->m;
    
    /*
     * Integrity checks
     */
    ae_assert(m>0, "BTranStep: M<=0", _state);
    
    /*
     * Timers
     */
    t0 = 0;
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    
    /*
     * BTran
     */
    rvectorsetlengthatleast(&state->btrantmp0, m, _state);
    rvectorsetlengthatleast(&state->btrantmp1, m, _state);
    rvectorsetlengthatleast(&state->btrantmp2, m, _state);
    for(i=0; i<=m-1; i++)
    {
        state->btrantmp0.ptr.p_double[i] = (double)(0);
    }
    state->btrantmp0.ptr.p_double[r] = (double)(1);
    reviseddualsimplex_dvalloc(rhor, m, _state);
    reviseddualsimplex_basissolvet(&state->basis, &state->btrantmp0, &rhor->dense, &state->btrantmp1, _state);
    reviseddualsimplex_dvdensetosparse(rhor, _state);
    
    /*
     * Timers
     */
    if( state->dotimers )
    {
        state->repdualbtrantime = state->repdualbtrantime+(ae_tickcount()-t0);
    }
}


/*************************************************************************
This function performs PivotRow step

Accepts:
* RhoR, BTRan result

Returns:
* AlphaR, array[N+M], pivot row

  -- ALGLIB --
     Copyright 19.07.2020 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_pivotrowstep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dssvector* rhor,
     dssvector* alphar,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t ns;
    ae_int_t nx;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t alphark;
    double v;
    ae_int_t t0;
    double avgcolwise;
    double avgrowwise;


    m = s->m;
    ns = s->ns;
    nx = s->ns+s->m;
    
    /*
     * Integrity checks
     */
    ae_assert(m>0, "BTranStep: M<=0", _state);
    
    /*
     * Timers
     */
    t0 = 0;
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    
    /*
     * Determine operation counts for columnwise and rowwise approaches
     */
    avgrowwise = (double)rhor->k*((double)state->at.ridx.ptr.p_int[nx]/(double)m);
    avgcolwise = (double)ns*((double)state->at.ridx.ptr.p_int[nx]/(double)nx);
    
    /*
     * Pivot row
     */
    if( ae_fp_less(avgrowwise,avgcolwise) )
    {
        
        /*
         * Use rowwise algorithm
         */
        reviseddualsimplex_dvinit(alphar, nx, _state);
        for(i=0; i<=rhor->k-1; i++)
        {
            k = rhor->idx.ptr.p_int[i];
            v = rhor->vals.ptr.p_double[i];
            j0 = state->a.ridx.ptr.p_int[k];
            j1 = state->a.ridx.ptr.p_int[k+1]-1;
            for(j=j0; j<=j1; j++)
            {
                jj = state->a.idx.ptr.p_int[j];
                alphar->dense.ptr.p_double[jj] = alphar->dense.ptr.p_double[jj]+v*state->a.vals.ptr.p_double[j];
            }
        }
        alphark = 0;
        for(i=0; i<=nx-1; i++)
        {
            if( !state->basis.isbasic.ptr.p_bool[i] )
            {
                
                /*
                 * Fetch nonbasic nonzeros to sparse part
                 */
                v = alphar->dense.ptr.p_double[i];
                if( v!=0.0 )
                {
                    alphar->idx.ptr.p_int[alphark] = i;
                    alphar->vals.ptr.p_double[alphark] = v;
                    alphark = alphark+1;
                }
            }
            else
            {
                
                /*
                 * Enforce condition that basic elements of AlphaR are exactly zero
                 */
                alphar->dense.ptr.p_double[i] = (double)(0);
            }
        }
        alphar->k = alphark;
    }
    else
    {
        
        /*
         * Use colwise algorithm
         */
        reviseddualsimplex_dvalloc(alphar, nx, _state);
        alphark = 0;
        for(i=0; i<=ns-1; i++)
        {
            k = state->basis.nidx.ptr.p_int[i];
            j0 = state->at.ridx.ptr.p_int[k];
            j1 = state->at.ridx.ptr.p_int[k+1]-1;
            v = (double)(0);
            for(j=j0; j<=j1; j++)
            {
                v = v+state->at.vals.ptr.p_double[j]*rhor->dense.ptr.p_double[state->at.idx.ptr.p_int[j]];
            }
            if( v!=0.0 )
            {
                alphar->idx.ptr.p_int[alphark] = k;
                alphar->vals.ptr.p_double[alphark] = v;
                alphark = alphark+1;
            }
        }
        alphar->k = alphark;
        reviseddualsimplex_dvsparsetodense(alphar, _state);
    }
    
    /*
     * Timers and tracing
     */
    if( state->dodetailedtrace )
    {
        reviseddualsimplex_updateavgcounter((double)rhor->k/coalesce((double)(rhor->n), (double)(1), _state), &state->repfillrhor, &state->repfillrhorcnt, _state);
        reviseddualsimplex_updateavgcounter((double)alphar->k/coalesce((double)(alphar->n), (double)(1), _state), &state->repfillpivotrow, &state->repfillpivotrowcnt, _state);
    }
    if( state->dotimers )
    {
        state->repdualpivotrowtime = state->repdualpivotrowtime+(ae_tickcount()-t0);
    }
}


/*************************************************************************
This function performs FTran step

Accepts:
* RhoR, array[M]
* Q, index of the entering variable, in [0,NX) range

Returns:
* AlphaQ,   array[M], FTran result
* AlphaQim, array[M], intermediate FTran result used by Forest-Tomlin update
* Tau,      array[M], used to compute DSE temporaries

  -- ALGLIB --
     Copyright 19.07.2020 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_ftranstep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dssvector* rhor,
     ae_int_t q,
     /* Real    */ ae_vector* alphaq,
     /* Real    */ ae_vector* alphaqim,
     /* Real    */ ae_vector* tau,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t t0;


    m = s->m;
    
    /*
     * Integrity checks
     */
    ae_assert(m>0, "BTranStep: M<=0", _state);
    
    /*
     * Timers
     */
    t0 = 0;
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    
    /*
     * FTran
     */
    rvectorsetlengthatleast(&state->ftrantmp0, m, _state);
    for(i=0; i<=m-1; i++)
    {
        state->ftrantmp0.ptr.p_double[i] = (double)(0);
    }
    j0 = state->at.ridx.ptr.p_int[q];
    j1 = state->at.ridx.ptr.p_int[q+1]-1;
    for(j=j0; j<=j1; j++)
    {
        state->ftrantmp0.ptr.p_double[state->at.idx.ptr.p_int[j]] = state->at.vals.ptr.p_double[j];
    }
    reviseddualsimplex_basissolvex(&state->basis, &state->ftrantmp0, alphaq, alphaqim, ae_true, &state->ftrantmp1, _state);
    ae_assert((settings->pricing==-1||settings->pricing==0)||settings->pricing==1, "FTran: unexpected Settings.Pricing", _state);
    if( settings->pricing==1 )
    {
        reviseddualsimplex_basissolve(&state->basis, &rhor->dense, tau, &state->ftrantmp1, _state);
    }
    
    /*
     * Timers
     */
    if( state->dotimers )
    {
        state->repdualftrantime = state->repdualftrantime+(ae_tickcount()-t0);
    }
}


/*************************************************************************
This function performs ratio test, either simple one or BFRT.

It accepts following parameters:
* AlphaR - pivot row
* Delta - delta from pricing step
* P - index of leaving variable from pricing step

It returns following results:
* Q - non-negative value for success, negative for primal infeasible problem
* AlphaRPiv - AlphaR[Q] (due to AlphaR being stored in sparse format this
  value is difficult to extract by index Q).
* ThetaD - dual step length
* PossibleFlips[PossibleFlipsCnt] - for possible flip indexes (for BFRT
  this set coincides with actual flips, but stabilizing BFRT is a bit more
  complex - some variables in PossibleFlips[] may need flipping and some not)

Internally it uses following fields of State for temporaries:
* EligibleAlphaR

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_ratiotest(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dssvector* alphar,
     double delta,
     ae_int_t p,
     ae_int_t* q,
     double* alpharpiv,
     double* thetad,
     /* Integer */ ae_vector* possibleflips,
     ae_int_t* possibleflipscnt,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t j;
    ae_int_t nj;
    ae_int_t dir;
    double vx;
    double vp;
    ae_int_t ej;
    double alpharej;
    double vtarget;
    double vtest;
    ae_int_t eligiblecnt;
    ae_int_t originaleligiblecnt;
    ae_int_t bndt;
    double alphawaver;
    double adelta;
    ae_int_t idx;
    double vtheta;
    ae_int_t t0;

    *q = 0;
    *alpharpiv = 0.0;
    *thetad = 0.0;

    nx = s->ns+s->m;
    ae_assert(ae_fp_neq(delta,(double)(0)), "RatioTest: zero delta", _state);
    ae_assert(s->state==reviseddualsimplex_ssvalid, "RatioTest: invalid X", _state);
    
    /*
     * Timers
     */
    t0 = 0;
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    
    /*
     * Clear output
     */
    *q = -1;
    *alpharpiv = (double)(0);
    *thetad = (double)(0);
    *possibleflipscnt = 0;
    
    /*
     * Prepare temporaries
     *
     * Scaled tolerances are used to test AlphaWaveR for positivity/negativity,
     * scale of I-th tolerance is calculated as ratio of ColScale[I] and ColScale[P].
     */
    dir = ae_sign(delta, _state);
    ivectorsetlengthatleast(possibleflips, nx, _state);
    
    /*
     * Prepare set of eligible variables
     *
     * NOTE: free variables are immediately chosen at this stage
     */
    ivectorsetlengthatleast(&state->eligiblealphar, alphar->k, _state);
    eligiblecnt = 0;
    for(j=0; j<=alphar->k-1; j++)
    {
        nj = alphar->idx.ptr.p_int[j];
        bndt = s->bndt.ptr.p_int[nj];
        
        /*
         * Handle fixed and free variables: fixed ones are not eligible,
         * free non-basic variables are always and immediately eligible
         */
        if( bndt==reviseddualsimplex_ccfixed )
        {
            continue;
        }
        if( bndt==reviseddualsimplex_ccfree )
        {
            *q = nj;
            *thetad = (double)(0);
            *alpharpiv = alphar->vals.ptr.p_double[j];
            if( state->dotrace )
            {
                ae_trace("> ratio test: quick exit, found free nonbasic variable\n");
                ae_trace("Q           = %12d  (variable selected)\n",
                    (int)(*q));
                ae_trace("ThetaD      = %12.3e  (dual step length)\n",
                    (double)(*thetad));
            }
            if( state->dotimers )
            {
                state->repdualratiotesttime = state->repdualratiotesttime+(ae_tickcount()-t0);
            }
            return;
        }
        
        /*
         * Handle lower/upper/range constraints
         */
        vx = s->xa.ptr.p_double[nj];
        vp = settings->pivottol;
        alphawaver = (double)dir*alphar->vals.ptr.p_double[j];
        if( bndt==reviseddualsimplex_cclower||(bndt==reviseddualsimplex_ccrange&&vx==s->bndl.ptr.p_double[nj]) )
        {
            if( alphawaver>vp )
            {
                state->eligiblealphar.ptr.p_int[eligiblecnt] = j;
                eligiblecnt = eligiblecnt+1;
                continue;
            }
        }
        if( bndt==reviseddualsimplex_ccupper||(bndt==reviseddualsimplex_ccrange&&vx==s->bndu.ptr.p_double[nj]) )
        {
            if( alphawaver<-vp )
            {
                state->eligiblealphar.ptr.p_int[eligiblecnt] = j;
                eligiblecnt = eligiblecnt+1;
                continue;
            }
        }
    }
    originaleligiblecnt = eligiblecnt;
    
    /*
     * Simple ratio test.
     */
    if( settings->ratiotest==0 )
    {
        
        /*
         * Ratio test
         */
        vtarget = (double)(0);
        for(j=0; j<=eligiblecnt-1; j++)
        {
            ej = state->eligiblealphar.ptr.p_int[j];
            nj = alphar->idx.ptr.p_int[ej];
            alpharej = alphar->vals.ptr.p_double[ej];
            
            /*
             * More general case
             */
            alphawaver = (double)dir*alpharej;
            vtest = s->d.ptr.p_double[nj]/alphawaver;
            if( *q<0||vtest<vtarget )
            {
                *q = nj;
                *alpharpiv = alpharej;
                vtarget = vtest;
                *thetad = s->d.ptr.p_double[nj]/alpharej;
            }
        }
        reviseddualsimplex_shifting(state, s, alphar, delta, *q, *alpharpiv, thetad, settings, _state);
        
        /*
         * Trace
         */
        if( state->dotrace )
        {
            ae_trace("> dual ratio test:\n");
            ae_trace("|E|         = %12d  (eligible set size)\n",
                (int)(originaleligiblecnt));
            ae_trace("Q           = %12d  (variable selected)\n",
                (int)(*q));
            ae_trace("ThetaD      = %12.3e  (dual step length)\n",
                (double)(*thetad));
        }
        if( state->dotimers )
        {
            state->repdualratiotesttime = state->repdualratiotesttime+(ae_tickcount()-t0);
        }
        
        /*
         * Done
         */
        return;
    }
    
    /*
     * Bounds flipping ratio test
     */
    if( settings->ratiotest==1 )
    {
        adelta = ae_fabs(delta, _state);
        
        /*
         * Quick exit
         */
        if( eligiblecnt==0 )
        {
            if( state->dotrace )
            {
                ae_trace("> ratio test: quick exit, no eligible variables\n");
            }
            return;
        }
        
        /*
         * BFRT
         */
        while(eligiblecnt>0)
        {
            
            /*
             * Find Q satisfying BFRT criteria
             */
            idx = -1;
            *q = -1;
            *alpharpiv = (double)(0);
            vtarget = (double)(0);
            for(j=0; j<=eligiblecnt-1; j++)
            {
                ej = state->eligiblealphar.ptr.p_int[j];
                nj = alphar->idx.ptr.p_int[ej];
                alpharej = alphar->vals.ptr.p_double[ej];
                vtheta = s->d.ptr.p_double[nj]/alpharej;
                vtest = (double)dir*vtheta;
                if( *q<0||vtest<vtarget )
                {
                    *q = nj;
                    *alpharpiv = alpharej;
                    vtarget = vtest;
                    *thetad = vtheta;
                    idx = j;
                }
            }
            ae_assert(*q>=0, "RatioTest: integrity check failed (BFRT)", _state);
            
            /*
             * BFRT mini-iterations will be terminated upon discovery
             * of non-boxed variable or upon exhausting of eligible set.
             */
            if( s->bndt.ptr.p_int[*q]!=reviseddualsimplex_ccrange )
            {
                break;
            }
            if( eligiblecnt==1 )
            {
                break;
            }
            
            /*
             * Update and test ADelta. Break BFRT mini-iterations once
             * we get negative slope.
             */
            adelta = adelta-(s->bndu.ptr.p_double[*q]-s->bndl.ptr.p_double[*q])*ae_fabs(*alpharpiv, _state);
            if( ae_fp_less_eq(adelta,(double)(0)) )
            {
                break;
            }
            
            /*
             * Update eligible set, record flip
             */
            possibleflips->ptr.p_int[*possibleflipscnt] = state->eligiblealphar.ptr.p_int[idx];
            *possibleflipscnt = *possibleflipscnt+1;
            state->eligiblealphar.ptr.p_int[idx] = state->eligiblealphar.ptr.p_int[eligiblecnt-1];
            eligiblecnt = eligiblecnt-1;
        }
        ae_assert(*q>=0, "RatioTest: unexpected failure", _state);
        *thetad = s->d.ptr.p_double[*q]/(*alpharpiv);
        reviseddualsimplex_shifting(state, s, alphar, delta, *q, *alpharpiv, thetad, settings, _state);
        
        /*
         * Trace
         */
        if( state->dotrace )
        {
            ae_trace("> dual bounds flipping ratio test:\n");
            ae_trace("|E|         = %12d  (eligible set size)\n",
                (int)(originaleligiblecnt));
            ae_trace("Q           = %12d  (variable selected)\n",
                (int)(*q));
            ae_trace("ThetaD      = %12.3e  (dual step length)\n",
                (double)(*thetad));
            ae_trace("Flips       = %12d  (possible bound flips)\n",
                (int)(state->possibleflipscnt));
        }
        if( state->dotimers )
        {
            state->repdualratiotesttime = state->repdualratiotesttime+(ae_tickcount()-t0);
        }
        
        /*
         * Done
         */
        return;
    }
    
    /*
     * Unknown test type
     */
    ae_assert(ae_false, "RatioTest: integrity check failed, unknown test type", _state);
}


/*************************************************************************
This function performs update of XB, XN, D and Z during final step of revised
dual simplex method.

It also updates basis cache of the subproblem (s.bcache field).

Depending on Settings.RatioTest, following operations are performed:
* Settings.RatioTest=0  ->  simple update is performed
* Settings.RatioTest=1  ->  bounds flipping ratio test update is performed
* Settings.RatioTest=2  ->  stabilizing bounds flipping ratio test update is performed

It accepts following parameters:
* P - index of leaving variable from pricing step
* Q - index of entering variable.
* R - index of leaving variable in AlphaQ
* Delta    - delta from pricing step
* AlphaPiv - pivot element (in absence of numerical rounding it is AlphaR[Q]=AlphaQ[R])
* ThetaP   - primal step length
* ThetaD   - dual step length
* AlphaQ   - pivot column
* AlphaQim - intermediate result from Ftran for AlphaQ, used for
             Forest-Tomlin update, not referenced when other update scheme is set
* AlphaR   - pivot row
* Tau - tau-vector for DSE pricing (ignored if simple pricing is used)
* PossibleAlphaRFlips, PossibleAlphaRFlipsCnt - outputs of the RatioTest()
  information about possible variable flips - indexes of AlphaR positions
  which are considered for flipping due to BFRT (however, we have to check
  residual costs before actually flipping variables - it is possible that some variables
  in this set actually do not need flipping)

It performs following operations:
* basis update
* update of XB/BndTB/BndLB/BndUB[] and XA[] (basic and nonbasic components), D
* update of pricing weights

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_updatestep(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_int_t p,
     ae_int_t q,
     ae_int_t r,
     double delta,
     double alphapiv,
     double thetap,
     double thetad,
     /* Real    */ const ae_vector* alphaq,
     /* Real    */ const ae_vector* alphaqim,
     const dssvector* alphar,
     /* Real    */ const ae_vector* tau,
     /* Integer */ const ae_vector* possiblealpharflips,
     ae_int_t possiblealpharflipscnt,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t m;
    ae_int_t ii;
    ae_int_t j;
    ae_int_t k;
    ae_int_t aj;
    ae_int_t k0;
    ae_int_t k1;
    double bndl;
    double bndu;
    ae_bool flipped;
    double flip;
    double dj;
    ae_int_t dir;
    ae_int_t idx;
    ae_int_t actualflipscnt;
    ae_int_t t0;
    ae_int_t alpharlen;


    nx = s->ns+s->m;
    m = s->m;
    
    /*
     * Integrity checks
     */
    ae_assert((settings->ratiotest==0||settings->ratiotest==1)||settings->ratiotest==2, "UpdateStep: invalid X", _state);
    ae_assert(s->state==reviseddualsimplex_ssvalid, "UpdateStep: invalid X", _state);
    ae_assert(p>=0&&q>=0, "UpdateStep: invalid P/Q", _state);
    ae_assert(ae_fp_neq(delta,(double)(0)), "UpdateStep: Delta=0", _state);
    ae_assert(ae_fp_neq(alphapiv,(double)(0)), "UpdateStep: AlphaPiv=0", _state);
    
    /*
     * Timers
     */
    t0 = 0;
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    
    /*
     * Prepare
     */
    dir = ae_sign(delta, _state);
    alpharlen = alphar->k;
    flip = (double)(0);
    rvectorsetlengthatleast(&state->tmp0, m, _state);
    for(k=0; k<=m-1; k++)
    {
        state->tmp0.ptr.p_double[k] = (double)(0);
    }
    ivectorsetlengthatleast(&state->ustmpi, nx, _state);
    actualflipscnt = 0;
    
    /*
     * Evaluate and update non-basic elements of D
     */
    for(ii=0; ii<=alpharlen-1; ii++)
    {
        j = alphar->idx.ptr.p_int[ii];
        s->d.ptr.p_double[j] = s->d.ptr.p_double[j]-thetad*alphar->vals.ptr.p_double[ii];
    }
    for(ii=0; ii<=possiblealpharflipscnt-1; ii++)
    {
        aj = possiblealpharflips->ptr.p_int[ii];
        j = alphar->idx.ptr.p_int[aj];
        dj = s->d.ptr.p_double[j];
        bndl = s->bndl.ptr.p_double[j];
        bndu = s->bndu.ptr.p_double[j];
        flipped = ae_false;
        if( s->xa.ptr.p_double[j]==bndl&&dj<(double)0 )
        {
            flip = bndu-bndl;
            flipped = ae_true;
        }
        else
        {
            if( s->xa.ptr.p_double[j]==bndu&&dj>(double)0 )
            {
                flip = bndl-bndu;
                flipped = ae_true;
            }
        }
        if( flipped )
        {
            delta = delta-(double)dir*(bndu-bndl)*ae_fabs(alphar->vals.ptr.p_double[aj], _state);
            state->ustmpi.ptr.p_int[actualflipscnt] = j;
            actualflipscnt = actualflipscnt+1;
            k0 = state->at.ridx.ptr.p_int[j];
            k1 = state->at.ridx.ptr.p_int[j+1]-1;
            for(k=k0; k<=k1; k++)
            {
                idx = state->at.idx.ptr.p_int[k];
                state->tmp0.ptr.p_double[idx] = state->tmp0.ptr.p_double[idx]+flip*state->at.vals.ptr.p_double[k];
            }
        }
    }
    s->d.ptr.p_double[p] = -thetad;
    s->d.ptr.p_double[q] = 0.0;
    
    /*
     * Apply BFRT update (aka long dual step) or simple ratio update
     */
    if( actualflipscnt>0 )
    {
        thetap = delta/alphapiv;
        k0 = state->at.ridx.ptr.p_int[q];
        k1 = state->at.ridx.ptr.p_int[q+1]-1;
        for(k=k0; k<=k1; k++)
        {
            idx = state->at.idx.ptr.p_int[k];
            state->tmp0.ptr.p_double[idx] = state->tmp0.ptr.p_double[idx]+thetap*state->at.vals.ptr.p_double[k];
        }
        reviseddualsimplex_basissolve(&state->basis, &state->tmp0, &state->tmp1, &state->tmp2, _state);
        for(j=0; j<=m-1; j++)
        {
            s->xb.ptr.p_double[j] = s->xb.ptr.p_double[j]-state->tmp1.ptr.p_double[j];
        }
        for(ii=0; ii<=actualflipscnt-1; ii++)
        {
            j = state->ustmpi.ptr.p_int[ii];
            if( s->xa.ptr.p_double[j]==s->bndl.ptr.p_double[j] )
            {
                s->xa.ptr.p_double[j] = s->bndu.ptr.p_double[j];
            }
            else
            {
                s->xa.ptr.p_double[j] = s->bndl.ptr.p_double[j];
            }
        }
        s->xb.ptr.p_double[r] = s->xa.ptr.p_double[q]+thetap;
        if( dir<0 )
        {
            s->xa.ptr.p_double[p] = s->bndl.ptr.p_double[p];
        }
        else
        {
            s->xa.ptr.p_double[p] = s->bndu.ptr.p_double[p];
        }
    }
    else
    {
        for(j=0; j<=m-1; j++)
        {
            s->xb.ptr.p_double[j] = s->xb.ptr.p_double[j]-thetap*alphaq->ptr.p_double[j];
        }
        s->xb.ptr.p_double[r] = s->xa.ptr.p_double[q]+thetap;
        if( dir<0 )
        {
            s->xa.ptr.p_double[p] = s->bndl.ptr.p_double[p];
        }
        else
        {
            s->xa.ptr.p_double[p] = s->bndu.ptr.p_double[p];
        }
    }
    
    /*
     * Update basis
     */
    reviseddualsimplex_basisupdatetrf(&state->basis, &state->at, p, q, alphaq, alphaqim, r, tau, settings, _state);
    
    /*
     * Update cached variables
     */
    reviseddualsimplex_cacheboundinfo(s, r, q, settings, _state);
    
    /*
     * Tracing and timers
     */
    if( state->dodetailedtrace )
    {
        if( state->basis.trftype==3 )
        {
            reviseddualsimplex_updateavgcounter(reviseddualsimplex_sparsityof(&state->basis.densemu, state->basis.trfage*m, _state), &state->repfilldensemu, &state->repfilldensemucnt, _state);
        }
    }
    if( state->dotimers )
    {
        state->repdualupdatesteptime = state->repdualupdatesteptime+(ae_tickcount()-t0);
    }
}


/*************************************************************************
This function performs several checks for accumulation of errors during
factorization update. It returns True if refactorization is advised.

  -- ALGLIB --
     Copyright 24.01.2019 by Bochkanov Sergey
*************************************************************************/
static ae_bool reviseddualsimplex_refactorizationrequired(const dualsimplexstate* state,
     const dualsimplexsubproblem* s,
     ae_int_t q,
     double alpharpiv,
     ae_int_t r,
     double alphaqpiv,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t i;
    double mx;
    double v;
    ae_bool result;


    m = s->m;
    result = ae_false;
    
    /*
     * Quick exit
     */
    if( state->basis.trfage<=reviseddualsimplex_safetrfage )
    {
        return result;
    }
    
    /*
     * Compare Q-th entry of the pivot row AlphaR with R-th entry of the AlphaQ;
     * ideally, both should match exactly. The difference is a rough estimate
     * of the magnitude of the numerical errors.
     */
    mx = 0.0;
    for(i=0; i<=m-1; i++)
    {
        v = state->alphaq.ptr.p_double[i];
        v = v*v;
        if( v>mx )
        {
            mx = v;
        }
    }
    mx = ae_sqrt(mx, _state);
    result = result||ae_fp_greater(ae_fabs(alphaqpiv-alpharpiv, _state),reviseddualsimplex_alphatrigger*(1.0+mx));
    result = result||ae_fp_greater(ae_fabs(alphaqpiv-alpharpiv, _state),reviseddualsimplex_alphatrigger2*ae_fabs(alpharpiv, _state));
    return result;
}


/*************************************************************************
This function caches information for I-th column of the  basis,  which  is
assumed to store variable K:
* lower bound in S.BndLB[I]=S.BndL[K]
* upper bound in S.BndUB[I]=S.BndU[K]
* bound type in  S.BndTB[I]=S.BndT[K]
* lower bound primal error tolerance in S.BndTolLB[I] (nonnegative)
* upper bound primal error tolerance in S.BndTolLB[I] (nonnegative).

  -- ALGLIB --
     Copyright 18.07.2020 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_cacheboundinfo(dualsimplexsubproblem* s,
     ae_int_t i,
     ae_int_t k,
     const dualsimplexsettings* settings,
     ae_state *_state)
{


    s->bndlb.ptr.p_double[i] = s->bndl.ptr.p_double[k];
    s->bndub.ptr.p_double[i] = s->bndu.ptr.p_double[k];
    s->bndtb.ptr.p_int[i] = s->bndt.ptr.p_int[k];
    s->bndtollb.ptr.p_double[i] = settings->xtolabs+settings->xtolrelabs*settings->xtolabs*ae_fabs(s->bndlb.ptr.p_double[i], _state);
    s->bndtolub.ptr.p_double[i] = settings->xtolabs+settings->xtolrelabs*settings->xtolabs*ae_fabs(s->bndub.ptr.p_double[i], _state);
}


/*************************************************************************
This function performs actual solution of dual simplex subproblem  (either
primary one or phase 1 one).

A problem with following properties is expected:
* M>0
* feasible box constraints
* dual feasible initial basis
* actual initial point XC and target value Z
* actual reduced cost vector D
* pricing weights being set to 1.0 or copied from previous problem

Returns:
    * Info = +1 for success, -3 for infeasible
    * IterationsCount is increased by amount of iterations performed

NOTE: this function internally uses separate storage of basic and nonbasic
      components; however, all inputs and outputs use single array S.XA[]
      to store both basic and nonbasic variables. It transparently splits
      variables on input and recombines them on output.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_solvesubproblemdual(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     ae_bool isphase1,
     const dualsimplexsettings* settings,
     ae_int_t* info,
     ae_state *_state)
{
    ae_int_t nx;
    ae_int_t m;
    ae_int_t i;
    ae_int_t p;
    ae_int_t r;
    ae_int_t q;
    double alpharpiv;
    double alphaqpiv;
    double thetad;
    double thetap;
    double delta;
    ae_int_t forcedrestarts;

    *info = 0;

    nx = s->ns+s->m;
    m = s->m;
    forcedrestarts = 0;
    
    /*
     * Integrity checks
     */
    ae_assert(s->state==reviseddualsimplex_ssvalid, "SolveSubproblemDual: X is not valid", _state);
    ae_assert(m>0, "SolveSubproblemDual: M<=0", _state);
    for(i=0; i<=nx-1; i++)
    {
        ae_assert(s->bndt.ptr.p_int[i]!=reviseddualsimplex_ccinfeasible, "SolveSubproblemDual: infeasible box constraints", _state);
    }
    ae_assert(reviseddualsimplex_isdualfeasible(state, s, settings, _state), "SolveSubproblemDual: dual infeasible initial basis", _state);
    
    /*
     * Actual processing
     */
    reviseddualsimplex_offloadbasiccomponents(s, &state->basis, settings, _state);
    *info = 0;
    rvectorsetlengthatleast(&state->tmp0, m, _state);
    for(;;)
    {
        
        /*
         * Iteration report
         */
        if( state->dotrace )
        {
            i = state->repiterationscount2;
            if( isphase1 )
            {
                i = state->repiterationscount1;
            }
            ae_trace("=== ITERATION %5d STARTED ========================================================================\n",
                (int)(i));
            if( state->dodetailedtrace )
            {
            }
        }
        
        /*
         * Pricing
         */
        reviseddualsimplex_pricingstep(state, s, isphase1, &p, &r, &delta, settings, _state);
        if( ae_fp_eq(delta,(double)(0)) )
        {
            
            /*
             * Solved! Feasible and bounded!
             */
            if( state->dotrace )
            {
                ae_trace("> pricing: feasible point found\n");
            }
            reviseddualsimplex_recombinebasicnonbasicx(s, &state->basis, _state);
            *info = 1;
            return;
        }
        
        /*
         * BTran
         */
        reviseddualsimplex_btranstep(state, s, r, &state->rhor, settings, _state);
        
        /*
         * Pivot row
         */
        reviseddualsimplex_pivotrowstep(state, s, &state->rhor, &state->alphar, settings, _state);
        
        /*
         * Ratio test
         */
        reviseddualsimplex_ratiotest(state, s, &state->alphar, delta, p, &q, &alpharpiv, &thetad, &state->possibleflips, &state->possibleflipscnt, settings, _state);
        if( q<0 )
        {
            
            /*
             * Do we have fresh factorization and state? If not,
             * refresh them prior to declaring that we have no solution.
             */
            if( state->basis.trfage>0&&forcedrestarts<reviseddualsimplex_maxforcedrestarts )
            {
                if( state->dotrace )
                {
                    ae_trace("> ratio test: failed, basis is old (age=%0d), forcing restart (%0d of %0d)\n",
                        (int)(state->basis.trfage),
                        (int)(forcedrestarts),
                        (int)(reviseddualsimplex_maxforcedrestarts-1));
                }
                reviseddualsimplex_basisfreshtrf(&state->basis, &state->at, settings, _state);
                reviseddualsimplex_subproblemhandlexnupdate(state, s, _state);
                reviseddualsimplex_offloadbasiccomponents(s, &state->basis, settings, _state);
                inc(&forcedrestarts, _state);
                continue;
            }
            
            /*
             * Dual unbounded, primal infeasible
             */
            if( state->dotrace )
            {
                ae_trace("> ratio test: failed, results are accepted\n");
            }
            reviseddualsimplex_recombinebasicnonbasicx(s, &state->basis, _state);
            *info = -3;
            return;
        }
        thetap = delta/alpharpiv;
        
        /*
         * FTran, including additional FTran for DSE weights (if needed)
         *
         * NOTE: AlphaQim is filled by intermediate FTran result which is useful
         *       for Forest-Tomlin update scheme. If not Forest-Tomlin update is
         *       used, then it is not set.
         */
        reviseddualsimplex_ftranstep(state, s, &state->rhor, q, &state->alphaq, &state->alphaqim, &state->tau, settings, _state);
        alphaqpiv = state->alphaq.ptr.p_double[r];
        
        /*
         * Check numerical accuracy, trigger refactorization if needed
         */
        if( reviseddualsimplex_refactorizationrequired(state, s, q, alpharpiv, r, alphaqpiv, _state) )
        {
            if( state->dotrace )
            {
                ae_trace("> refactorization test: numerical errors are too large, forcing refactorization and restart\n");
            }
            reviseddualsimplex_basisfreshtrf(&state->basis, &state->at, settings, _state);
            reviseddualsimplex_subproblemhandlexnupdate(state, s, _state);
            reviseddualsimplex_offloadbasiccomponents(s, &state->basis, settings, _state);
            continue;
        }
        
        /*
         * Basis change and update
         */
        reviseddualsimplex_updatestep(state, s, p, q, r, delta, alpharpiv, thetap, thetad, &state->alphaq, &state->alphaqim, &state->alphar, &state->tau, &state->possibleflips, state->possibleflipscnt, settings, _state);
        inc(&state->repiterationscount, _state);
        if( isphase1 )
        {
            inc(&state->repiterationscount1, _state);
        }
        else
        {
            inc(&state->repiterationscount2, _state);
        }
    }
}


/*************************************************************************
This function solves simplex subproblem using primal simplex method.

A problem with following properties is expected:
* M>0
* feasible box constraints
* primal feasible initial basis
* actual initial point XC and target value Z
* actual reduced cost vector D
* pricing weights being set to 1.0 or copied from previous problem

Returns:
    * Info = +1 for success, -3 for infeasible
    * IterationsCount is increased by amount of iterations performed

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_solvesubproblemprimal(dualsimplexstate* state,
     dualsimplexsubproblem* s,
     const dualsimplexsettings* settings,
     ae_int_t* info,
     ae_state *_state)
{
    ae_int_t nn;
    ae_int_t nx;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vmax;
    ae_int_t bi;
    double dj;
    ae_int_t bndt;
    ae_int_t q;
    ae_int_t p;
    ae_int_t r;
    ae_int_t dir;
    double lim;
    ae_bool haslim;
    double thetap;
    double xbnd;
    double flip;
    ae_int_t canddir;
    double candlim;
    double candflip;
    ae_int_t j0;
    ae_int_t j1;
    double alphawave;
    double vp;
    double vb;
    double vx;
    double vtest;
    double vv;

    *info = 0;

    nn = s->ns;
    nx = s->ns+s->m;
    m = s->m;
    
    /*
     * Integrity checks
     */
    ae_assert(s->state==reviseddualsimplex_ssvalid, "SolveSubproblemPrimal: X is not valid", _state);
    ae_assert(m>0, "SolveSubproblemPrimal: M<=0", _state);
    for(i=0; i<=nx-1; i++)
    {
        ae_assert(s->bndt.ptr.p_int[i]!=reviseddualsimplex_ccinfeasible, "SolveSubproblemPrimal: infeasible box constraints", _state);
    }
    
    /*
     * Actual processing
     */
    *info = 1;
    rvectorsetlengthatleast(&state->tmp0, m, _state);
    for(;;)
    {
        
        /*
         * Iteration report
         */
        if( state->dotrace )
        {
            i = state->repiterationscount3;
            ae_trace("=== ITERATION %5d STARTED ========================================================================\n",
                (int)(i));
            if( state->dodetailedtrace )
            {
            }
        }
        
        /*
         * Primal simplex pricing step: we implement the very basic version
         * of the pricing step because it is expected that primal simplex method
         * is used just to apply quick correction after removal of the perturbation.
         */
        q = -1;
        vmax = (double)(0);
        dir = 0;
        lim = ae_maxrealnumber;
        haslim = ae_false;
        flip = (double)(0);
        canddir = 0;
        for(i=0; i<=nn-1; i++)
        {
            j = state->basis.nidx.ptr.p_int[i];
            dj = s->d.ptr.p_double[j];
            bndt = s->bndt.ptr.p_int[j];
            if( bndt==reviseddualsimplex_ccfixed )
            {
                continue;
            }
            if( bndt==reviseddualsimplex_ccrange )
            {
                v = (double)(0);
                candlim = s->bndu.ptr.p_double[j]-s->bndl.ptr.p_double[j];
                candflip = (double)(0);
                if( s->xa.ptr.p_double[j]==s->bndl.ptr.p_double[j] )
                {
                    v = -dj;
                    canddir = 1;
                    candflip = s->bndu.ptr.p_double[j];
                }
                if( s->xa.ptr.p_double[j]==s->bndu.ptr.p_double[j] )
                {
                    v = dj;
                    canddir = -1;
                    candflip = s->bndl.ptr.p_double[j];
                }
                if( v>vmax )
                {
                    vmax = v;
                    dir = canddir;
                    lim = candlim;
                    haslim = ae_true;
                    flip = candflip;
                    q = j;
                }
                continue;
            }
            v = (double)(0);
            canddir = 0;
            if( bndt==reviseddualsimplex_cclower )
            {
                v = -dj;
                canddir = 1;
            }
            if( bndt==reviseddualsimplex_ccupper )
            {
                v = dj;
                canddir = -1;
            }
            if( bndt==reviseddualsimplex_ccfree )
            {
                v = ae_fabs(dj, _state);
                canddir = -ae_sign(dj, _state);
            }
            if( v>vmax )
            {
                vmax = v;
                dir = canddir;
                lim = ae_maxrealnumber;
                haslim = ae_false;
                q = j;
            }
            continue;
        }
        if( vmax<=settings->dtolabs )
        {
            
            /*
             * Solved: primal and dual feasible!
             */
            if( state->dotrace )
            {
                ae_trace("> primal pricing: feasible point found\n");
            }
            return;
        }
        ae_assert(q>=0, "SolveSubproblemPrimal: integrity check failed", _state);
        if( state->dotrace )
        {
            ae_trace("> primal pricing: found entering variable\n");
            ae_trace("Q           = %12d  (variable selected)\n",
                (int)(q));
            ae_trace("|D|         = %12.3e  (dual infeasibility)\n",
                (double)(vmax));
        }
        
        /*
         * FTran and textbook ratio test (again, we expect primal phase to terminate quickly)
         *
         * NOTE: AlphaQim is filled by intermediate FTran result which is useful
         *       for Forest-Tomlin update scheme. If not Forest-Tomlin update is
         *       used, then it is not set.
         */
        for(i=0; i<=m-1; i++)
        {
            state->tmp0.ptr.p_double[i] = (double)(0);
        }
        j0 = state->at.ridx.ptr.p_int[q];
        j1 = state->at.ridx.ptr.p_int[q+1]-1;
        for(j=j0; j<=j1; j++)
        {
            state->tmp0.ptr.p_double[state->at.idx.ptr.p_int[j]] = state->at.vals.ptr.p_double[j];
        }
        reviseddualsimplex_basissolvex(&state->basis, &state->tmp0, &state->alphaq, &state->alphaqim, ae_true, &state->tmp2, _state);
        vp = settings->pivottol;
        p = -1;
        r = -1;
        thetap = (double)(0);
        xbnd = (double)(0);
        for(i=0; i<=m-1; i++)
        {
            bi = state->basis.idx.ptr.p_int[i];
            alphawave = -(double)dir*state->alphaq.ptr.p_double[i];
            vx = s->xa.ptr.p_double[bi];
            if( alphawave<-vp&&reviseddualsimplex_hasbndl(s, bi, _state) )
            {
                vb = s->bndl.ptr.p_double[bi];
                if( vx<=vb )
                {
                    
                    /*
                     * X[Bi] is already out of bounds due to rounding errors, perform shifting
                     */
                    vb = vx-reviseddualsimplex_shiftlen;
                    s->bndl.ptr.p_double[bi] = vx;
                }
                vtest = (vb-vx)/alphawave;
                if( p<0||vtest<thetap )
                {
                    p = bi;
                    r = i;
                    thetap = vtest;
                    xbnd = vb;
                }
            }
            if( alphawave>vp&&reviseddualsimplex_hasbndu(s, bi, _state) )
            {
                vb = s->bndu.ptr.p_double[bi];
                if( vx>=vb )
                {
                    
                    /*
                     * X[Bi] is already out of bounds due to rounding errors, perform shifting
                     */
                    vb = vx+reviseddualsimplex_shiftlen;
                    s->bndu.ptr.p_double[bi] = vb;
                }
                vtest = (vb-vx)/alphawave;
                if( p<0||vtest<thetap )
                {
                    p = bi;
                    r = i;
                    thetap = vtest;
                    xbnd = vb;
                }
            }
        }
        if( p<0&&!haslim )
        {
            
            /*
             * Primal unbounded
             */
            *info = -4;
            if( state->dotrace )
            {
                ae_trace("> primal ratio test: dual infeasible, primal unbounded\n");
            }
            return;
        }
        if( state->dotrace )
        {
            ae_trace("> primal ratio test: found leaving variable\n");
            ae_trace("P           = %12d  (variable index)\n",
                (int)(p));
            ae_trace("R           = %12d  (variable index in basis)\n",
                (int)(r));
            ae_trace("ThetaP      = %12.3e  (primal step length)\n",
                (double)(thetap));
        }
        
        /*
         * Update step
         */
        if( p>=0&&(!haslim||thetap<lim) )
        {
            
            /*
             * One of the basic variables hit the boundary and become non-basic.
             *
             * Perform update:
             * * update basic elements of X[] (X[p] is explicitly set to the
             *   boundary value) and X[q]
             * * update target value Z
             * * update factorization
             * * update D[]
             */
            rvectorsetlengthatleast(&state->tmp0, m, _state);
            for(i=0; i<=m-1; i++)
            {
                bi = state->basis.idx.ptr.p_int[i];
                vv = thetap*((double)dir*state->alphaq.ptr.p_double[i]);
                s->xa.ptr.p_double[bi] = s->xa.ptr.p_double[bi]-vv;
            }
            s->xa.ptr.p_double[p] = xbnd;
            s->xa.ptr.p_double[q] = s->xa.ptr.p_double[q]+(double)dir*thetap;
            for(i=0; i<=m-1; i++)
            {
                state->tmp0.ptr.p_double[i] = (double)(0);
            }
            reviseddualsimplex_basisupdatetrf(&state->basis, &state->at, p, q, &state->alphaq, &state->alphaqim, r, &state->tmp0, settings, _state);
            for(i=0; i<=m-1; i++)
            {
                state->tmp0.ptr.p_double[i] = s->effc.ptr.p_double[state->basis.idx.ptr.p_int[i]];
            }
            reviseddualsimplex_basissolvet(&state->basis, &state->tmp0, &state->tmp1, &state->tmp2, _state);
            reviseddualsimplex_computeantv(state, &state->tmp1, &s->d, _state);
            for(i=0; i<=nn-1; i++)
            {
                j = state->basis.nidx.ptr.p_int[i];
                s->d.ptr.p_double[j] = s->effc.ptr.p_double[j]-s->d.ptr.p_double[j];
            }
        }
        else
        {
            
            /*
             * Basis does not change because Qth variable flips from one bound
             * to another one long before we encounter the boundary
             */
            s->xa.ptr.p_double[q] = flip;
            for(i=0; i<=m-1; i++)
            {
                bi = state->basis.idx.ptr.p_int[i];
                vv = lim*((double)dir*state->alphaq.ptr.p_double[i]);
                s->xa.ptr.p_double[bi] = s->xa.ptr.p_double[bi]-vv;
            }
        }
        inc(&state->repiterationscount, _state);
        inc(&state->repiterationscount3, _state);
    }
}


/*************************************************************************
This function estimates feasibility properties of the  current  basis  and
invokes phase 1 if necessary.

A problem with following properties is expected:
* M>0
* feasible box constraints
* some initial basis (can be dual infeasible) with actual factorization
* actual initial point XC and target value Z
* actual reduced cost vector D

It returns:
* +1 if dual feasible basis was found
* -4 if problem is dual infeasible

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_invokephase1(dualsimplexstate* state,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t m;
    double dualerr;


    m = state->primary.m;
    state->repterminationtype = 0;
    
    /*
     * Integrity checks
     */
    ae_assert(state->primary.state==reviseddualsimplex_ssvalid, "InvokePhase1: invalid primary X", _state);
    ae_assert(m>0, "InvokePhase1: M<=0", _state);
    
    /*
     * Is it dual feasible from the very beginning (or maybe after initial DFC)?
     */
    if( state->dotrace )
    {
        ae_trace("> performing initial dual feasibility correction...\n");
    }
    dualerr = reviseddualsimplex_initialdualfeasibilitycorrection(state, &state->primary, settings, _state);
    if( state->dotrace )
    {
        ae_trace("> initial dual feasibility correction done\ndualErr = %0.3e\n",
            (double)(dualerr));
    }
    if( ae_fp_less_eq(dualerr,settings->dtolabs) )
    {
        if( state->dotrace )
        {
            ae_trace("> solution is dual feasible, phase 1 is done\n");
        }
        state->repterminationtype = 1;
        return;
    }
    if( state->dotrace )
    {
        ae_trace("> solution is not dual feasible, proceeding to full-scale phase 1\n");
        ae_trace("\n");
        ae_trace("****************************************************************************************************\n");
        ae_trace("*   PHASE 1 OF DUAL SIMPLEX SOLVER                                                                 *\n");
        ae_trace("****************************************************************************************************\n");
    }
    
    /*
     * Solve phase #1 subproblem
     */
    reviseddualsimplex_subprobleminitphase1(&state->primary, &state->basis, &state->phase1, _state);
    if( state->dotrace )
    {
        ae_trace("> performing phase 1 dual feasibility correction...\n");
    }
    dualerr = reviseddualsimplex_initialdualfeasibilitycorrection(state, &state->phase1, settings, _state);
    if( state->dotrace )
    {
        ae_trace("> phase 1 dual feasibility correction done\ndualErr = %0.3e\n",
            (double)(dualerr));
    }
    reviseddualsimplex_solvesubproblemdual(state, &state->phase1, ae_true, settings, &state->repterminationtype, _state);
    ae_assert(state->repterminationtype>0, "DualSimplexSolver: unexpected failure of phase #1", _state);
    state->repterminationtype = 1;
    
    /*
     * Setup initial basis for phase #2 using solution of phase #1
     */
    if( state->dotrace )
    {
        ae_trace("> setting up phase 2 initial solution\n");
    }
    reviseddualsimplex_subprobleminferinitialxn(state, &state->primary, _state);
    dualerr = reviseddualsimplex_initialdualfeasibilitycorrection(state, &state->primary, settings, _state);
    if( ae_fp_greater(dualerr,settings->dtolabs) )
    {
        if( state->dotrace )
        {
            ae_trace("> initial dual feasibility correction failed! terminating...\n");
        }
        state->repterminationtype = -4;
        return;
    }
    state->repterminationtype = 1;
}


/*************************************************************************
This function performs actual solution.

INPUT PARAMETERS:
    State   -   state
    
Solution results can be found in fields  of  State  which  are  explicitly
declared as accessible by external code.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_dssoptimizewrk(dualsimplexstate* state,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t nx;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    double v;
    hqrndstate rs;
    ae_int_t t0;

    ae_frame_make(_state, &_frame_block);
    memset(&rs, 0, sizeof(rs));
    _hqrndstate_init(&rs, _state, ae_true);

    nx = state->primary.ns+state->primary.m;
    m = state->primary.m;
    t0 = 0;
    
    /*
     * Handle case when M=0; after this block we assume that M>0.
     */
    if( m==0 )
    {
        
        /*
         * Trace
         */
        if( state->dotrace )
        {
            ae_trace("> box-only LP problem, quick solution\n");
        }
        
        /*
         * Solve
         */
        reviseddualsimplex_solveboxonly(state, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Most basic check for correctness of box and/or linear constraints
     */
    for(j=0; j<=nx-1; j++)
    {
        if( state->primary.bndt.ptr.p_int[j]==reviseddualsimplex_ccinfeasible )
        {
            
            /*
             * Set error flag and generate some point to return
             */
            if( state->dotrace )
            {
                ae_trace("[WARNING] infeasible box constraint (or range constraint with AL>AU) found, terminating\n");
            }
            state->repterminationtype = -3;
            reviseddualsimplex_setzeroxystats(state, _state);
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Initialization:
     * * initial perturbed C[]
     */
    hqrndseed(7456, 2355, &rs, _state);
    for(i=0; i<=nx-1; i++)
    {
        if( !reviseddualsimplex_isfree(&state->primary, i, _state) )
        {
            
            /*
             * apply perturbation
             */
            v = settings->perturbmag*((double)1+ae_fabs(state->primary.rawc.ptr.p_double[i], _state))*((double)1+hqrnduniformr(&rs, _state));
            if( !reviseddualsimplex_hasbndl(&state->primary, i, _state) )
            {
                v = -v;
            }
            state->primary.effc.ptr.p_double[i] = state->primary.rawc.ptr.p_double[i]+v;
        }
    }
    
    /*
     * Solve phase 1 subproblem, then perturbed subproblem
     */
    reviseddualsimplex_basisfreshtrf(&state->basis, &state->at, settings, _state);
    if( state->primary.state==reviseddualsimplex_ssinvalid )
    {
        reviseddualsimplex_subprobleminferinitialxn(state, &state->primary, _state);
    }
    if( state->primary.state==reviseddualsimplex_ssvalidxn )
    {
        reviseddualsimplex_subproblemhandlexnupdate(state, &state->primary, _state);
    }
    ae_assert(state->primary.state==reviseddualsimplex_ssvalid, "DSS: integrity check failed (init)", _state);
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    reviseddualsimplex_invokephase1(state, settings, _state);
    if( state->dotimers )
    {
        state->repphase1time = ae_tickcount()-t0;
    }
    if( state->repterminationtype<=0 )
    {
        
        /*
         * Primal unbounded, dual infeasible
         */
        ae_assert(state->repterminationtype==-4, "DSS: integrity check for InvokePhase1() result failed", _state);
        if( state->dotrace )
        {
            ae_trace("> the problem is dual infeasible, primal unbounded\n> done\n");
        }
        reviseddualsimplex_setxydstats(state, &state->primary, &state->basis, &state->xydsbuf, &state->repx, &state->replagbc, &state->replaglc, &state->repstats, _state);
        ae_frame_leave(_state);
        return;
    }
    if( state->dotrace )
    {
        ae_trace("\n");
        ae_trace("****************************************************************************************************\n");
        ae_trace("*   PHASE 2 OF DUAL SIMPLEX SOLVER                                                                 *\n");
        ae_trace("****************************************************************************************************\n");
    }
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    reviseddualsimplex_solvesubproblemdual(state, &state->primary, ae_false, settings, &state->repterminationtype, _state);
    if( state->dotimers )
    {
        state->repphase2time = ae_tickcount()-t0;
    }
    if( state->repterminationtype<=0 )
    {
        
        /*
         * Primal infeasible
         */
        ae_assert(state->repterminationtype==-3, "DSS: integrity check for SolveSubproblemDual() result failed", _state);
        if( state->dotrace )
        {
            ae_trace("> the problem is primal infeasible\n> done\n");
        }
        reviseddualsimplex_setxydstats(state, &state->primary, &state->basis, &state->xydsbuf, &state->repx, &state->replagbc, &state->replaglc, &state->repstats, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Remove perturbation from the cost vector,
     * then use primal simplex to enforce dual feasibility
     * after removal of the perturbation (if necessary).
     */
    if( state->dotrace )
    {
        ae_trace("\n");
        ae_trace("****************************************************************************************************\n");
        ae_trace("*   PHASE 3 OF DUAL SIMPLEX SOLVER (perturbation removed from cost vector)                         *\n");
        ae_trace("****************************************************************************************************\n");
    }
    if( state->dotimers )
    {
        t0 = ae_tickcount();
    }
    reviseddualsimplex_subprobleminitphase3(&state->primary, &state->phase3, _state);
    for(i=0; i<=nx-1; i++)
    {
        state->phase3.effc.ptr.p_double[i] = state->primary.rawc.ptr.p_double[i];
    }
    ae_assert(state->phase3.state>=reviseddualsimplex_ssvalidxn, "DSS: integrity check failed (remove perturbation)", _state);
    reviseddualsimplex_subproblemhandlexnupdate(state, &state->phase3, _state);
    reviseddualsimplex_solvesubproblemprimal(state, &state->phase3, settings, &state->repterminationtype, _state);
    if( state->dotimers )
    {
        state->repphase3time = ae_tickcount()-t0;
    }
    if( state->repterminationtype<=0 )
    {
        
        /*
         * Dual infeasible, primal unbounded
         */
        ae_assert(state->repterminationtype==-4, "DSS: integrity check for SolveSubproblemPrimal() result failed", _state);
        if( state->dotrace )
        {
            ae_trace("> the problem is primal unbounded\n> done\n");
        }
        reviseddualsimplex_setxydstats(state, &state->phase3, &state->basis, &state->xydsbuf, &state->repx, &state->replagbc, &state->replaglc, &state->repstats, _state);
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=nx-1; i++)
    {
        state->primary.xa.ptr.p_double[i] = state->phase3.xa.ptr.p_double[i];
        if( reviseddualsimplex_hasbndl(&state->primary, i, _state) )
        {
            state->primary.xa.ptr.p_double[i] = ae_maxreal(state->primary.xa.ptr.p_double[i], state->primary.bndl.ptr.p_double[i], _state);
        }
        if( reviseddualsimplex_hasbndu(&state->primary, i, _state) )
        {
            state->primary.xa.ptr.p_double[i] = ae_minreal(state->primary.xa.ptr.p_double[i], state->primary.bndu.ptr.p_double[i], _state);
        }
    }
    
    /*
     * Primal and dual feasible, problem solved
     */
    state->repterminationtype = 1;
    reviseddualsimplex_setxydstats(state, &state->primary, &state->basis, &state->xydsbuf, &state->repx, &state->replagbc, &state->replaglc, &state->repstats, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Box-constrained solver; sets State.RepX, State.RepStats and State.RepTerminationType,
does not change other fields.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_solveboxonly(dualsimplexstate* state,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t ns;


    ns = state->primary.ns;
    ae_assert(state->primary.m==0, "SolveBoxOnly: integrity check failed", _state);
    rsetv(ns, 0.0, &state->replagbc, _state);
    for(i=0; i<=ns-1; i++)
    {
        
        /*
         * Handle infeasible variable
         */
        if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccinfeasible )
        {
            state->repterminationtype = -3;
            state->repx.ptr.p_double[i] = 0.5*(state->primary.bndl.ptr.p_double[i]+state->primary.bndu.ptr.p_double[i]);
            state->repstats.ptr.p_int[i] = 0;
            continue;
        }
        
        /*
         * Handle fixed variable
         */
        if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccfixed )
        {
            state->repx.ptr.p_double[i] = state->primary.bndl.ptr.p_double[i];
            state->repstats.ptr.p_int[i] = -1;
            state->replagbc.ptr.p_double[i] = -state->primary.rawc.ptr.p_double[i];
            continue;
        }
        
        /*
         * Handle non-zero cost component
         */
        if( ae_fp_greater(state->primary.rawc.ptr.p_double[i],(double)(0)) )
        {
            if( state->primary.bndt.ptr.p_int[i]!=reviseddualsimplex_ccrange&&state->primary.bndt.ptr.p_int[i]!=reviseddualsimplex_cclower )
            {
                if( state->repterminationtype>0 )
                {
                    state->repterminationtype = -4;
                }
                if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccupper )
                {
                    state->repx.ptr.p_double[i] = state->primary.bndu.ptr.p_double[i];
                    state->repstats.ptr.p_int[i] = 1;
                }
                else
                {
                    state->repx.ptr.p_double[i] = (double)(0);
                    state->repstats.ptr.p_int[i] = 0;
                }
                state->replagbc.ptr.p_double[i] = (double)(0);
            }
            else
            {
                state->repx.ptr.p_double[i] = state->primary.bndl.ptr.p_double[i];
                state->repstats.ptr.p_int[i] = -1;
                state->replagbc.ptr.p_double[i] = -state->primary.rawc.ptr.p_double[i];
            }
            continue;
        }
        if( ae_fp_less(state->primary.rawc.ptr.p_double[i],(double)(0)) )
        {
            if( state->primary.bndt.ptr.p_int[i]!=reviseddualsimplex_ccrange&&state->primary.bndt.ptr.p_int[i]!=reviseddualsimplex_ccupper )
            {
                if( state->repterminationtype>0 )
                {
                    state->repterminationtype = -4;
                }
                if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_cclower )
                {
                    state->repx.ptr.p_double[i] = state->primary.bndl.ptr.p_double[i];
                    state->repstats.ptr.p_int[i] = -1;
                }
                else
                {
                    state->repx.ptr.p_double[i] = (double)(0);
                    state->repstats.ptr.p_int[i] = 0;
                }
                state->replagbc.ptr.p_double[i] = (double)(0);
            }
            else
            {
                state->repx.ptr.p_double[i] = state->primary.bndu.ptr.p_double[i];
                state->repstats.ptr.p_int[i] = 1;
                state->replagbc.ptr.p_double[i] = -state->primary.rawc.ptr.p_double[i];
            }
            continue;
        }
        
        /*
         * Handle non-free variable with zero cost component
         */
        if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccupper||state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccrange )
        {
            state->repx.ptr.p_double[i] = state->primary.bndu.ptr.p_double[i];
            state->repstats.ptr.p_int[i] = 1;
            state->replagbc.ptr.p_double[i] = (double)(0);
            continue;
        }
        if( state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_cclower )
        {
            state->repx.ptr.p_double[i] = state->primary.bndl.ptr.p_double[i];
            state->repstats.ptr.p_int[i] = -1;
            state->replagbc.ptr.p_double[i] = (double)(0);
            continue;
        }
        
        /*
         * Free variable, zero cost component
         */
        ae_assert(state->primary.bndt.ptr.p_int[i]==reviseddualsimplex_ccfree, "DSSOptimize: integrity check failed", _state);
        state->repx.ptr.p_double[i] = (double)(0);
        state->repstats.ptr.p_int[i] = 0;
        state->replagbc.ptr.p_double[i] = (double)(0);
    }
}


/*************************************************************************
Zero-fill RepX, RepLagBC, RepLagLC, RepStats.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_setzeroxystats(dualsimplexstate* state,
     ae_state *_state)
{
    ae_int_t i;


    for(i=0; i<=state->primary.ns-1; i++)
    {
        state->repx.ptr.p_double[i] = (double)(0);
        state->replagbc.ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=state->primary.m-1; i++)
    {
        state->replaglc.ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=state->primary.ns+state->primary.m-1; i++)
    {
        state->repstats.ptr.p_int[i] = 0;
    }
}


/*************************************************************************
This function initializes basis structure; no triangular factorization is
prepared yet. Previously allocated memory is reused.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basisinit(ae_int_t ns,
     ae_int_t m,
     dualsimplexbasis* s,
     ae_state *_state)
{
    ae_int_t i;


    s->ns = ns;
    s->m = m;
    ivectorgrowto(&s->idx, m, _state);
    ivectorgrowto(&s->nidx, ns, _state);
    bvectorgrowto(&s->isbasic, ns+m, _state);
    for(i=0; i<=ns-1; i++)
    {
        s->nidx.ptr.p_int[i] = i;
        s->isbasic.ptr.p_bool[i] = ae_false;
    }
    for(i=0; i<=m-1; i++)
    {
        s->idx.ptr.p_int[i] = ns+i;
        s->isbasic.ptr.p_bool[ns+i] = ae_true;
    }
    s->trftype = 3;
    s->trfage = 0;
    s->isvalidtrf = ae_false;
    rvectorsetlengthatleast(&s->dseweights, m, _state);
    for(i=0; i<=m-1; i++)
    {
        s->dseweights.ptr.p_double[i] = 1.0;
    }
    s->dsevalid = ae_false;
    reviseddualsimplex_basisclearstats(s, _state);
}


/*************************************************************************
This function clears internal performance counters of the basis

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basisclearstats(dualsimplexbasis* s,
     ae_state *_state)
{


    s->statfact = 0;
    s->statupdt = 0;
    s->statoffdiag = (double)(0);
}


/*************************************************************************
This function resizes basis. It is assumed that constraint matrix is
completely overwritten by new one, but both matrices are similar enough
so we can reuse previous basis.

Dual steepest edge weights are invalidated by this function.

This function:
* tries to resize basis
* if possible, returns True and valid basis with valid factorization
* if resize is impossible (or abandoned due to stability reasons), it
  returns False and basis object is left in the invalid state (you have
  to reinitialize it by all-logicals basis)

Following types of resize are supported:
* new basis size is larger than previous one => logical elements are
  added to the new basis
* basis sizes match => no operation is performed
* new basis size is zero => basis is set to zero

This function:
* requires valid triangular factorization at S on entry
* replaces it by another, valid factorization
* checks that new factorization deviates from the previous one not too much
  by comparing magnitudes of min[abs(u_ii)] in both factorization (sharp
  decrease results in attempt to resize being abandoned

IMPORTANT: if smooth resize is not possible, this function throws an
           exception! It is responsibility of the caller to check that
           smooth resize is possible

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool reviseddualsimplex_basistryresize(dualsimplexbasis* s,
     ae_int_t newm,
     const sparsematrix* at,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t ns;
    ae_int_t oldm;
    ae_int_t i;
    double oldminu;
    double newminu;
    ae_bool result;


    ns = s->ns;
    oldm = s->m;
    result = ae_false;
    
    /*
     * Quick exit strategies
     */
    if( newm==0 )
    {
        reviseddualsimplex_basisinit(ns, 0, s, _state);
        result = ae_true;
        return result;
    }
    
    /*
     * Same size or larger
     */
    if( newm>=oldm )
    {
        ae_assert(s->isvalidtrf||oldm==0, "BasisTryResize: needs valid TRF in S", _state);
        
        /*
         * Save information about matrix conditioning 
         */
        oldminu = reviseddualsimplex_basisminimumdiagonalelement(s, _state);
        
        /*
         * Growth if needed
         */
        s->m = newm;
        ivectorgrowto(&s->idx, newm, _state);
        bvectorgrowto(&s->isbasic, ns+newm, _state);
        for(i=oldm; i<=newm-1; i++)
        {
            s->idx.ptr.p_int[i] = ns+i;
            s->isbasic.ptr.p_bool[ns+i] = ae_true;
        }
        
        /*
         * DSE weights are invalid and filled by 1.0
         */
        rvectorgrowto(&s->dseweights, newm, _state);
        for(i=0; i<=newm-1; i++)
        {
            s->dseweights.ptr.p_double[i] = 1.0;
        }
        s->dsevalid = ae_false;
        
        /*
         * Invalidate TRF.
         * Try to refactorize.
         */
        s->isvalidtrf = ae_false;
        newminu = reviseddualsimplex_basisfreshtrfunsafe(s, at, settings, _state);
        result = ae_fp_greater_eq(newminu,reviseddualsimplex_maxudecay*oldminu);
        return result;
    }
    ae_assert(ae_false, "BasisTryResize: unexpected branch", _state);
    return result;
}


/*************************************************************************
This function returns minimum diagonal element of S. Result=1 is  returned
for M=0.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static double reviseddualsimplex_basisminimumdiagonalelement(const dualsimplexbasis* s,
     ae_state *_state)
{
    double v;
    double vv;
    ae_int_t i;
    ae_int_t m;
    double result;


    m = s->m;
    if( m==0 )
    {
        result = (double)(1);
        return result;
    }
    ae_assert(((s->trftype==0||s->trftype==1)||s->trftype==2)||s->trftype==3, "BasisMinimumDiagonalElement: unexpected TRF type", _state);
    ae_assert(s->isvalidtrf, "BasisMinimumDiagonalElement: TRF is invalid", _state);
    v = ae_maxrealnumber;
    for(i=0; i<=m-1; i++)
    {
        vv = (double)(0);
        if( s->trftype==0||s->trftype==1 )
        {
            vv = s->denselu.ptr.pp_double[i][i];
        }
        if( s->trftype==2||s->trftype==3 )
        {
            vv = sparsegetdiagonal(&s->sparseu, i, _state);
        }
        if( vv<(double)0 )
        {
            vv = -vv;
        }
        if( vv<v )
        {
            v = vv;
        }
    }
    result = v;
    return result;
}


/*************************************************************************
This function exports division of variables into basic/nonbasic ones; only
basic/nonbasic sets are exported - triangular factorization is NOT exported;
however, valid triangular factorization IS required in order to perform
exporting.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basisexportto(const dualsimplexbasis* s0,
     dualsimplexbasis* s1,
     ae_state *_state)
{


    s1->ns = s0->ns;
    s1->m = s0->m;
    copyintegerarray(&s0->idx, &s1->idx, _state);
    copyintegerarray(&s0->nidx, &s1->nidx, _state);
    copybooleanarray(&s0->isbasic, &s1->isbasic, _state);
    s1->isvalidtrf = ae_false;
    s1->trftype = -1;
    s1->dsevalid = ae_false;
    if( s0->m>0 )
    {
        ae_assert(s0->isvalidtrf, "BasisExport: valid factorization is required for source basis", _state);
        s1->eminu = reviseddualsimplex_basisminimumdiagonalelement(s0, _state);
    }
    else
    {
        s1->eminu = (double)(1);
    }
}


/*************************************************************************
This function imports from S1 to S0 a division of variables into
basic/nonbasic ones; only basic/nonbasic sets are imported.

Triangular factorization is not imported; however,  this  function  checks
that new factorization deviates from the previous  one  not  too  much  by
comparing magnitudes of min[abs(u_ii)] in both factorization (basis being
imported stores statistics about U). Sharp decrease of diagonal elements
means that we have too unstable situation which results in import being
abandoned. In this case False is returned, and the basis S0 is left in the
indeterminate invalid state (you have to reinitialize it by all-logicals).

IMPORTANT: if metrics of S0 and S1 do not match, an exception will be generated.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool reviseddualsimplex_basistryimportfrom(dualsimplexbasis* s0,
     const dualsimplexbasis* s1,
     const sparsematrix* at,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t i;
    double newminu;
    ae_bool result;


    ae_assert(s0->ns==s1->ns, "BasisImportFrom: structural variable counts do not match", _state);
    reviseddualsimplex_basisclearstats(s0, _state);
    s0->m = s1->m;
    for(i=0; i<=s0->m-1; i++)
    {
        s0->idx.ptr.p_int[i] = s1->idx.ptr.p_int[i];
    }
    for(i=0; i<=s0->ns-1; i++)
    {
        s0->nidx.ptr.p_int[i] = s1->nidx.ptr.p_int[i];
    }
    for(i=0; i<=s0->m+s0->ns-1; i++)
    {
        s0->isbasic.ptr.p_bool[i] = s1->isbasic.ptr.p_bool[i];
    }
    s0->isvalidtrf = ae_false;
    rvectorsetlengthatleast(&s0->dseweights, s1->m, _state);
    for(i=0; i<=s1->m-1; i++)
    {
        s0->dseweights.ptr.p_double[i] = 1.0;
    }
    s0->dsevalid = ae_false;
    newminu = reviseddualsimplex_basisfreshtrfunsafe(s0, at, settings, _state);
    result = ae_fp_greater_eq(newminu,reviseddualsimplex_maxudecay*s1->eminu);
    if( !result )
    {
        s0->isvalidtrf = ae_false;
        s0->trftype = -1;
    }
    return result;
}


/*************************************************************************
This function computes fresh triangular factorization.

If TRF of age 0 (fresh) is already present, no new factorization is calculated.
If factorization has exactly zero element along diagonal, this function
generates exception.


  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basisfreshtrf(dualsimplexbasis* s,
     const sparsematrix* at,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    double v;


    v = reviseddualsimplex_basisfreshtrfunsafe(s, at, settings, _state);
    ae_assert(ae_fp_greater(v,(double)(0)), "BasisFreshTrf: degeneracy of B is detected", _state);
}


/*************************************************************************
This function computes fresh triangular factorization.

If TRF of age 0 (fresh) is already present, no new factorization is calculated.

It returns min[abs(u[i,i])] which can be used to determine whether factorization
is degenerate or not (it will factorize anything, the question is whether
it is possible to use factorization)

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static double reviseddualsimplex_basisfreshtrfunsafe(dualsimplexbasis* s,
     const sparsematrix* at,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t ns;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t k1;
    ae_int_t nzl;
    ae_int_t nzu;
    ae_int_t nlogical;
    ae_int_t nstructural;
    ae_int_t offs;
    ae_int_t offs1;
    ae_int_t offs2;
    double result;


    m = s->m;
    ns = s->ns;
    result = (double)(0);
    
    /*
     * Compare TRF type with one required by settings, invalidation and refresh otherwise
     */
    if( s->trftype!=settings->trftype )
    {
        s->trftype = settings->trftype;
        s->isvalidtrf = ae_false;
        result = reviseddualsimplex_basisfreshtrfunsafe(s, at, settings, _state);
        return result;
    }
    
    /*
     * Is it valid and fresh?
     */
    if( s->isvalidtrf&&s->trfage==0 )
    {
        result = reviseddualsimplex_basisminimumdiagonalelement(s, _state);
        return result;
    }
    
    /*
     * Dense TRF
     */
    if( s->trftype==0||s->trftype==1 )
    {
        ivectorsetlengthatleast(&s->colpermbwd, m, _state);
        for(i=0; i<=m-1; i++)
        {
            s->colpermbwd.ptr.p_int[i] = i;
        }
        rmatrixsetlengthatleast(&s->denselu, m, m, _state);
        for(i=0; i<=m-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                s->denselu.ptr.pp_double[i][j] = (double)(0);
            }
        }
        for(i=0; i<=m-1; i++)
        {
            j0 = at->ridx.ptr.p_int[s->idx.ptr.p_int[i]];
            j1 = at->ridx.ptr.p_int[s->idx.ptr.p_int[i]+1]-1;
            for(j=j0; j<=j1; j++)
            {
                s->denselu.ptr.pp_double[i][at->idx.ptr.p_int[j]] = at->vals.ptr.p_double[j];
            }
        }
        rmatrixlu(&s->denselu, m, m, &s->tmpi, _state);
        reviseddualsimplex_pivottobwd(&s->tmpi, m, &s->rowpermbwd, _state);
        s->isvalidtrf = ae_true;
        s->trfage = 0;
        s->statfact = s->statfact+1;
        s->statoffdiag = s->statoffdiag+ae_sqr((double)(m-1), _state);
        result = reviseddualsimplex_basisminimumdiagonalelement(s, _state);
        return result;
    }
    
    /*
     * Sparse TRF (with either PFI or Forest-Tomlin)
     */
    if( s->trftype==2||s->trftype==3 )
    {
        
        /*
         * Determine permutation which moves logical variables
         * to the beginning.
         *
         * NOTE: this reordering results in stable factorization
         *       because we prenormalized constraints with 2-norm,
         *       all elements in the logical columns are less than
         *       1.0 in magnitude.
         *
         * After this block is done we have following arrays:
         * * tCInvIdx[j], which is an inverse of ColPermBwf[]
         */
        ivectorsetlengthatleast(&s->tcinvidx, m, _state);
        ivectorsetlengthatleast(&s->rowpermbwd, m, _state);
        ivectorsetlengthatleast(&s->colpermbwd, m, _state);
        for(i=0; i<=m-1; i++)
        {
            s->tcinvidx.ptr.p_int[i] = i;
            s->rowpermbwd.ptr.p_int[i] = i;
            s->colpermbwd.ptr.p_int[i] = i;
        }
        nlogical = 0;
        for(i=0; i<=m-1; i++)
        {
            if( s->idx.ptr.p_int[i]>=ns )
            {
                j = s->rowpermbwd.ptr.p_int[nlogical];
                s->rowpermbwd.ptr.p_int[nlogical] = s->rowpermbwd.ptr.p_int[i];
                s->rowpermbwd.ptr.p_int[i] = j;
                j1 = s->tcinvidx.ptr.p_int[s->idx.ptr.p_int[i]-ns];
                j = s->colpermbwd.ptr.p_int[j1];
                s->colpermbwd.ptr.p_int[j1] = s->colpermbwd.ptr.p_int[nlogical];
                s->colpermbwd.ptr.p_int[nlogical] = j;
                s->tcinvidx.ptr.p_int[s->colpermbwd.ptr.p_int[nlogical]] = nlogical;
                s->tcinvidx.ptr.p_int[s->colpermbwd.ptr.p_int[j1]] = j1;
                nlogical = nlogical+1;
            }
        }
        sortmiddlei(&s->colpermbwd, nlogical, m-nlogical, _state);
        for(i=0; i<=m-1; i++)
        {
            s->tcinvidx.ptr.p_int[s->colpermbwd.ptr.p_int[i]] = i;
        }
        nstructural = m-nlogical;
        
        /*
         * Prepare SparseLU1 to receive factored out logical part of the matrix
         * and SparseLU2 to receive structural part of the matrix.
         */
        ivectorsetlengthatleast(&s->sparselu1.ridx, nstructural+1, _state);
        ivectorsetlengthatleast(&s->sparselu1.didx, nstructural, _state);
        ivectorsetlengthatleast(&s->sparselu1.uidx, nstructural, _state);
        s->sparselu1.matrixtype = 1;
        s->sparselu1.m = nstructural;
        s->sparselu1.n = nlogical;
        s->sparselu1.ridx.ptr.p_int[0] = 0;
        ivectorsetlengthatleast(&s->sparselu2.ridx, nstructural+1, _state);
        ivectorsetlengthatleast(&s->sparselu2.didx, nstructural, _state);
        ivectorsetlengthatleast(&s->sparselu2.uidx, nstructural, _state);
        s->sparselu2.matrixtype = 1;
        s->sparselu2.m = nstructural;
        s->sparselu2.n = nstructural;
        s->sparselu2.ridx.ptr.p_int[0] = 0;
        
        /*
         * Reorder array, perform LU factorization
         */
        for(k=0; k<=nstructural-1; k++)
        {
            
            /*
             * Make sure SparseLU1 and SparseLU2 have enough place.
             */
            offs1 = s->sparselu1.ridx.ptr.p_int[k];
            offs2 = s->sparselu2.ridx.ptr.p_int[k];
            ivectorgrowto(&s->sparselu1.idx, offs1+m, _state);
            rvectorgrowto(&s->sparselu1.vals, offs1+m, _state);
            ivectorgrowto(&s->sparselu2.idx, offs2+m, _state);
            rvectorgrowto(&s->sparselu2.vals, offs2+m, _state);
            
            /*
             * Extract K-th row of the SparseLU1/2 (I-th row of the original matrix)
             */
            i = s->rowpermbwd.ptr.p_int[k+nlogical];
            j0 = at->ridx.ptr.p_int[s->idx.ptr.p_int[i]];
            j1 = at->ridx.ptr.p_int[s->idx.ptr.p_int[i]+1]-1;
            for(j=j0; j<=j1; j++)
            {
                k1 = s->tcinvidx.ptr.p_int[at->idx.ptr.p_int[j]];
                if( k1<nlogical )
                {
                    
                    /*
                     * Append element to SparseLU1
                     */
                    s->sparselu1.idx.ptr.p_int[offs1] = k1;
                    s->sparselu1.vals.ptr.p_double[offs1] = at->vals.ptr.p_double[j];
                    offs1 = offs1+1;
                }
                else
                {
                    
                    /*
                     * Append element to SparseLU2
                     */
                    s->sparselu2.idx.ptr.p_int[offs2] = k1-nlogical;
                    s->sparselu2.vals.ptr.p_double[offs2] = at->vals.ptr.p_double[j];
                    offs2 = offs2+1;
                }
            }
            
            /*
             * Elements added to the last row of LU1 can be unordered,
             * so it needs resorting.
             *
             * LU2 does NOT need resorting because trailing NStructural
             * elements of permutation were post-sorted to produce
             * already sorted results.
             */
            tagsortmiddleir(&s->sparselu1.idx, &s->sparselu1.vals, s->sparselu1.ridx.ptr.p_int[k], offs1-s->sparselu1.ridx.ptr.p_int[k], _state);
            s->sparselu1.ridx.ptr.p_int[k+1] = offs1;
            s->sparselu2.ridx.ptr.p_int[k+1] = offs2;
        }
        s->sparselu1.ninitialized = s->sparselu1.ridx.ptr.p_int[nstructural];
        s->sparselu2.ninitialized = s->sparselu2.ridx.ptr.p_int[nstructural];
        sparseinitduidx(&s->sparselu1, _state);
        sparseinitduidx(&s->sparselu2, _state);
        if( nstructural>0 )
        {
            sptrflu(&s->sparselu2, 2, &s->densep2, &s->densep2c, &s->lubuf2, _state);
            for(i=0; i<=nstructural-1; i++)
            {
                j = s->rowpermbwd.ptr.p_int[i+nlogical];
                s->rowpermbwd.ptr.p_int[i+nlogical] = s->rowpermbwd.ptr.p_int[s->densep2.ptr.p_int[i]+nlogical];
                s->rowpermbwd.ptr.p_int[s->densep2.ptr.p_int[i]+nlogical] = j;
                j = s->colpermbwd.ptr.p_int[i+nlogical];
                s->colpermbwd.ptr.p_int[i+nlogical] = s->colpermbwd.ptr.p_int[s->densep2c.ptr.p_int[i]+nlogical];
                s->colpermbwd.ptr.p_int[s->densep2c.ptr.p_int[i]+nlogical] = j;
            }
            
            /*
             * Process L factor:
             *
             * 1. count number of non-zeros in the L factor,
             * 2. fill NLogical*NLogical leading block
             * 3. NStructural*M bottom block
             */
            nzl = nlogical;
            for(i=0; i<=nstructural-1; i++)
            {
                k = s->lubuf2.rowpermrawidx.ptr.p_int[i];
                nzl = nzl+(s->sparselu1.ridx.ptr.p_int[k+1]-s->sparselu1.ridx.ptr.p_int[k]);
                nzl = nzl+1+(s->sparselu2.didx.ptr.p_int[i]-s->sparselu2.ridx.ptr.p_int[i]);
            }
            rvectorsetlengthatleast(&s->sparsel.vals, nzl, _state);
            ivectorsetlengthatleast(&s->sparsel.idx, nzl, _state);
            ivectorsetlengthatleast(&s->sparsel.ridx, m+1, _state);
            ivectorsetlengthatleast(&s->sparsel.didx, m, _state);
            ivectorsetlengthatleast(&s->sparsel.uidx, m, _state);
            s->sparsel.matrixtype = 1;
            s->sparsel.m = m;
            s->sparsel.n = m;
            s->sparsel.ninitialized = nzl;
            s->sparsel.ridx.ptr.p_int[0] = 0;
            for(i=0; i<=nlogical-1; i++)
            {
                s->sparsel.idx.ptr.p_int[i] = i;
                s->sparsel.vals.ptr.p_double[i] = 1.0;
                s->sparsel.ridx.ptr.p_int[i+1] = i+1;
            }
            for(i=0; i<=nstructural-1; i++)
            {
                offs = s->sparsel.ridx.ptr.p_int[nlogical+i];
                k = s->lubuf2.rowpermrawidx.ptr.p_int[i];
                j0 = s->sparselu1.ridx.ptr.p_int[k];
                j1 = s->sparselu1.ridx.ptr.p_int[k+1]-1;
                for(j=j0; j<=j1; j++)
                {
                    s->sparsel.idx.ptr.p_int[offs] = s->sparselu1.idx.ptr.p_int[j];
                    s->sparsel.vals.ptr.p_double[offs] = -s->sparselu1.vals.ptr.p_double[j];
                    offs = offs+1;
                }
                j0 = s->sparselu2.ridx.ptr.p_int[i];
                j1 = s->sparselu2.didx.ptr.p_int[i]-1;
                for(j=j0; j<=j1; j++)
                {
                    s->sparsel.idx.ptr.p_int[offs] = nlogical+s->sparselu2.idx.ptr.p_int[j];
                    s->sparsel.vals.ptr.p_double[offs] = s->sparselu2.vals.ptr.p_double[j];
                    offs = offs+1;
                }
                s->sparsel.idx.ptr.p_int[offs] = nlogical+i;
                s->sparsel.vals.ptr.p_double[offs] = 1.0;
                offs = offs+1;
                s->sparsel.ridx.ptr.p_int[nlogical+i+1] = offs;
            }
            ae_assert(s->sparsel.ninitialized==s->sparsel.ridx.ptr.p_int[m], "BasisFreshTrf: integrity check failed", _state);
            sparseinitduidx(&s->sparsel, _state);
            
            /*
             * Process U factor:
             *
             * 1. count number of non-zeros in the U factor,
             * 2. fill NLogical*NLogical leading block
             * 3. NStructural*NStructural bottom block
             */
            nzu = nlogical;
            for(i=0; i<=nstructural-1; i++)
            {
                nzu = nzu+1+(s->sparselu2.ridx.ptr.p_int[i+1]-s->sparselu2.uidx.ptr.p_int[i]);
            }
            rvectorsetlengthatleast(&s->sparseu.vals, nzu, _state);
            ivectorsetlengthatleast(&s->sparseu.idx, nzu, _state);
            ivectorsetlengthatleast(&s->sparseu.ridx, m+1, _state);
            ivectorsetlengthatleast(&s->sparseu.didx, m, _state);
            ivectorsetlengthatleast(&s->sparseu.uidx, m, _state);
            s->sparseu.matrixtype = 1;
            s->sparseu.m = m;
            s->sparseu.n = m;
            s->sparseu.ninitialized = nzu;
            s->sparseu.ridx.ptr.p_int[0] = 0;
            for(i=0; i<=nlogical-1; i++)
            {
                s->sparseu.idx.ptr.p_int[i] = i;
                s->sparseu.vals.ptr.p_double[i] = -1.0;
                s->sparseu.ridx.ptr.p_int[i+1] = i+1;
            }
            for(i=0; i<=nstructural-1; i++)
            {
                offs = s->sparseu.ridx.ptr.p_int[nlogical+i];
                s->sparseu.idx.ptr.p_int[offs] = nlogical+i;
                j = s->sparselu2.didx.ptr.p_int[i];
                if( j<s->sparselu2.uidx.ptr.p_int[i] )
                {
                    ae_assert(s->sparselu2.idx.ptr.p_int[j]==i, "BasisFreshTrf: integrity check failed", _state);
                    s->sparseu.vals.ptr.p_double[offs] = s->sparselu2.vals.ptr.p_double[j];
                }
                else
                {
                    s->sparseu.vals.ptr.p_double[offs] = (double)(0);
                }
                offs = offs+1;
                j0 = s->sparselu2.uidx.ptr.p_int[i];
                j1 = s->sparselu2.ridx.ptr.p_int[i+1]-1;
                for(j=j0; j<=j1; j++)
                {
                    s->sparseu.idx.ptr.p_int[offs] = nlogical+s->sparselu2.idx.ptr.p_int[j];
                    s->sparseu.vals.ptr.p_double[offs] = s->sparselu2.vals.ptr.p_double[j];
                    offs = offs+1;
                }
                s->sparseu.ridx.ptr.p_int[nlogical+i+1] = offs;
            }
            ae_assert(s->sparseu.ninitialized==s->sparseu.ridx.ptr.p_int[m], "BasisFreshTrf: integrity check failed", _state);
            sparseinitduidx(&s->sparseu, _state);
        }
        else
        {
            ivectorsetlengthatleast(&s->nrs, m, _state);
            for(i=0; i<=m-1; i++)
            {
                s->nrs.ptr.p_int[i] = 1;
            }
            sparsecreatecrsbuf(m, m, &s->nrs, &s->sparsel, _state);
            for(i=0; i<=nlogical-1; i++)
            {
                sparseset(&s->sparsel, i, i, 1.0, _state);
            }
            sparsecreatecrsbuf(m, m, &s->nrs, &s->sparseu, _state);
            for(i=0; i<=nlogical-1; i++)
            {
                sparseset(&s->sparseu, i, i, -1.0, _state);
            }
        }
        sparsecopytransposecrsbuf(&s->sparseu, &s->sparseut, _state);
        s->isvalidtrf = ae_true;
        s->trfage = 0;
        s->statfact = s->statfact+1;
        s->statoffdiag = s->statoffdiag+(double)(s->sparsel.ridx.ptr.p_int[m]-m)+(double)(s->sparseu.ridx.ptr.p_int[m]-m);
        result = reviseddualsimplex_basisminimumdiagonalelement(s, _state);
        return result;
    }
    
    /*
     *
     */
    ae_assert(ae_false, "BasisFreshTrf: unexpected TRF type", _state);
    return result;
}


/*************************************************************************
This function fills S.DSEWeights by actual weights according to current
settings and sets validity flag.

Basis object MUST store valid triangular factorization, otherwise this
function throws an exception.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basisrequestweights(dualsimplexbasis* s,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t ns;
    ae_int_t i;
    ae_int_t j;
    double v;
    double vv;


    m = s->m;
    ns = s->ns;
    ae_assert((settings->pricing==-1||settings->pricing==0)||settings->pricing==1, "BasisRequestWeights: unknown pricing type", _state);
    ae_assert(s->isvalidtrf, "BasisRequestWeights: factorization is not computed prior to calling this function", _state);
    
    /*
     * If weights are valid, return immediately
     */
    if( s->dsevalid )
    {
        return;
    }
    
    /*
     * Compute weights from scratch
     */
    if( settings->pricing==-1||settings->pricing==1 )
    {
        for(i=0; i<=m-1; i++)
        {
            if( s->idx.ptr.p_int[i]<ns )
            {
                
                /*
                 * Structural variable, DSE weight is computed by definition
                 */
                rvectorsetlengthatleast(&s->wtmp0, m, _state);
                rvectorsetlengthatleast(&s->wtmp1, m, _state);
                for(j=0; j<=m-1; j++)
                {
                    s->wtmp0.ptr.p_double[j] = (double)(0);
                }
                s->wtmp0.ptr.p_double[i] = (double)(1);
                reviseddualsimplex_basissolvet(s, &s->wtmp0, &s->wtmp1, &s->wtmp2, _state);
                v = (double)(0);
                for(j=0; j<=m-1; j++)
                {
                    vv = s->wtmp1.ptr.p_double[j];
                    v = v+vv*vv;
                }
                s->dseweights.ptr.p_double[i] = v;
            }
            else
            {
                
                /*
                 * Logical variable, weight can be set to 1.0
                 */
                s->dseweights.ptr.p_double[i] = 1.0;
            }
        }
        s->dsevalid = ae_true;
        return;
    }
    
    /*
     * Compute weights from scratch
     */
    if( settings->pricing==0 )
    {
        for(i=0; i<=m-1; i++)
        {
            s->dseweights.ptr.p_double[i] = 1.0;
        }
        s->dsevalid = ae_true;
        return;
    }
    ae_assert(ae_false, "BasisRequestWeights: unexpected pricing type", _state);
}


/*************************************************************************
This function updates triangular factorization by adding Q  to  basis  and
removing P from basis. It also updates index tables IsBasic[], BasicIdx[],
Basis.NIdx[].

AlphaQim contains intermediate result from Ftran for AlphaQ, it is used
by Forest-Tomlin update scheme. If other update is used, it is not referenced
at all.

X[], D[], Z are NOT recomputed.

Tau is used if Settings.Pricing=1, ignored otherwise.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basisupdatetrf(dualsimplexbasis* s,
     const sparsematrix* at,
     ae_int_t p,
     ae_int_t q,
     /* Real    */ const ae_vector* alphaq,
     /* Real    */ const ae_vector* alphaqim,
     ae_int_t r,
     /* Real    */ const ae_vector* tau,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t nn;
    ae_int_t i;
    ae_int_t j;
    ae_bool processed;
    double invaq;
    ae_int_t dstoffs;
    ae_int_t srcoffs;
    ae_int_t srcidx;
    double srcval;
    double vcorner;
    ae_int_t idxd;
    double v;


    m = s->m;
    nn = s->ns;
    
    /*
     * Update index tables
     *
     * TODO: better code!!!!!!!!!!!!!!!!!!!!!!!
     */
    s->isbasic.ptr.p_bool[p] = ae_false;
    s->isbasic.ptr.p_bool[q] = ae_true;
    for(i=0; i<=m-1; i++)
    {
        if( s->idx.ptr.p_int[i]==p )
        {
            s->idx.ptr.p_int[i] = q;
            break;
        }
    }
    for(i=0; i<=nn-1; i++)
    {
        if( s->nidx.ptr.p_int[i]==q )
        {
            s->nidx.ptr.p_int[i] = p;
            break;
        }
    }
    
    /*
     * Update dense factorization
     */
    if( ((s->trftype!=settings->trftype||s->trftype==0)||!s->isvalidtrf)||s->trfage>=settings->maxtrfage )
    {
        
        /*
         * Complete refresh is needed for factorization
         */
        s->isvalidtrf = ae_false;
        reviseddualsimplex_basisfreshtrf(s, at, settings, _state);
    }
    else
    {
        processed = ae_false;
        if( (s->trftype==0||s->trftype==1)||s->trftype==2 )
        {
            
            /*
             * Dense/sparse factorizations with dense PFI
             */
            ae_assert(ae_fp_neq(alphaq->ptr.p_double[r],(double)(0)), "BasisUpdateTrf: integrity check failed, AlphaQ[R]=0", _state);
            rvectorgrowto(&s->densepfieta, (s->trfage+1)*m, _state);
            ivectorgrowto(&s->rk, s->trfage+1, _state);
            s->rk.ptr.p_int[s->trfage] = r;
            invaq = 1.0/alphaq->ptr.p_double[r];
            for(i=0; i<=m-1; i++)
            {
                if( i!=r )
                {
                    s->densepfieta.ptr.p_double[s->trfage*m+i] = -alphaq->ptr.p_double[i]*invaq;
                }
                else
                {
                    s->densepfieta.ptr.p_double[s->trfage*m+i] = invaq;
                }
            }
            inc(&s->trfage, _state);
            s->statupdt = s->statupdt+1;
            s->statoffdiag = s->statoffdiag+ae_sqr((double)(m-1), _state);
            processed = ae_true;
        }
        if( s->trftype==3 )
        {
            
            /*
             * Sparse factorization with Forest-Tomlin update
             */
            ae_assert(ae_fp_neq(alphaq->ptr.p_double[r],(double)(0)), "BasisUpdateTrf: integrity check failed, AlphaQ[R]=0", _state);
            rvectorgrowto(&s->densemu, (s->trfage+1)*m, _state);
            ivectorgrowto(&s->rk, s->trfage+1, _state);
            ivectorgrowto(&s->dk, s->trfage+1, _state);
            rvectorsetlengthatleast(&s->utmp0, m, _state);
            
            /*
             * Determine D - index of row being overwritten by Forest-Tomlin update
             */
            idxd = -1;
            for(i=0; i<=m-1; i++)
            {
                if( s->rowpermbwd.ptr.p_int[i]==r )
                {
                    idxd = i;
                    break;
                }
            }
            ae_assert(idxd>=0, "BasisUpdateTrf: unexpected integrity check failure", _state);
            s->rk.ptr.p_int[s->trfage] = r;
            s->dk.ptr.p_int[s->trfage] = idxd;
            
            /*
             * Modify L with permutation which moves D-th row/column to the end:
             * * rows 0...D-1 are left intact
             * * rows D+1...M-1 are moved one position up, with columns 0..D-1
             *   retained as is, and columns D+1...M-1 being moved one position left.
             * * last row is filled by permutation/modification of AlphaQim
             * Determine FT update coefficients in the process.
             */
            ivectorgrowto(&s->sparsel.idx, s->sparsel.ridx.ptr.p_int[m]+m, _state);
            rvectorgrowto(&s->sparsel.vals, s->sparsel.ridx.ptr.p_int[m]+m, _state);
            for(i=0; i<=m-1; i++)
            {
                s->utmp0.ptr.p_double[i] = (double)(0);
            }
            for(i=idxd+1; i<=m-1; i++)
            {
                j = s->sparsel.ridx.ptr.p_int[i+1]-1;
                if( s->sparsel.idx.ptr.p_int[j]!=i||s->sparsel.vals.ptr.p_double[j]!=(double)1 )
                {
                    ae_assert(ae_false, "UpdateTrf: integrity check failed for sparse L", _state);
                }
                dstoffs = s->sparsel.ridx.ptr.p_int[i-1];
                srcoffs = s->sparsel.ridx.ptr.p_int[i];
                
                /*
                 * Read first element in the row (it has at least one - unit diagonal)
                 */
                srcidx = s->sparsel.idx.ptr.p_int[srcoffs];
                srcval = s->sparsel.vals.ptr.p_double[srcoffs];
                
                /*
                 * Read/write columns 0...D-1
                 */
                while(srcidx<idxd)
                {
                    s->sparsel.idx.ptr.p_int[dstoffs] = srcidx;
                    s->sparsel.vals.ptr.p_double[dstoffs] = srcval;
                    dstoffs = dstoffs+1;
                    srcoffs = srcoffs+1;
                    srcidx = s->sparsel.idx.ptr.p_int[srcoffs];
                    srcval = s->sparsel.vals.ptr.p_double[srcoffs];
                }
                
                /*
                 * If we have non-zero element in column D, use it as
                 * right-hand side of intermediate linear system which
                 * is used to determine coefficients of update matrix.
                 */
                if( srcidx==idxd )
                {
                    s->utmp0.ptr.p_double[i-1] = srcval;
                    srcoffs = srcoffs+1;
                    srcidx = s->sparsel.idx.ptr.p_int[srcoffs];
                    srcval = s->sparsel.vals.ptr.p_double[srcoffs];
                }
                
                /*
                 * Process columns D+1...I-1
                 */
                v = s->utmp0.ptr.p_double[i-1];
                while(srcidx<i)
                {
                    s->sparsel.idx.ptr.p_int[dstoffs] = srcidx-1;
                    s->sparsel.vals.ptr.p_double[dstoffs] = srcval;
                    v = v-srcval*s->utmp0.ptr.p_double[srcidx-1];
                    dstoffs = dstoffs+1;
                    srcoffs = srcoffs+1;
                    srcidx = s->sparsel.idx.ptr.p_int[srcoffs];
                    srcval = s->sparsel.vals.ptr.p_double[srcoffs];
                }
                s->utmp0.ptr.p_double[i-1] = v;
                
                /*
                 * Write out unit diagonal, finalize row
                 */
                s->sparsel.idx.ptr.p_int[dstoffs] = i-1;
                s->sparsel.vals.ptr.p_double[dstoffs] = (double)(1);
                dstoffs = dstoffs+1;
                s->sparsel.ridx.ptr.p_int[i] = dstoffs;
            }
            s->utmp0.ptr.p_double[m-1] = (double)(1);
            dstoffs = s->sparsel.ridx.ptr.p_int[m-1];
            for(j=0; j<=idxd-1; j++)
            {
                v = alphaqim->ptr.p_double[j];
                if( v!=(double)0 )
                {
                    s->sparsel.idx.ptr.p_int[dstoffs] = j;
                    s->sparsel.vals.ptr.p_double[dstoffs] = v;
                    dstoffs = dstoffs+1;
                }
            }
            vcorner = alphaqim->ptr.p_double[idxd];
            for(j=idxd+1; j<=m-1; j++)
            {
                v = alphaqim->ptr.p_double[j];
                if( v!=(double)0 )
                {
                    s->sparsel.idx.ptr.p_int[dstoffs] = j-1;
                    s->sparsel.vals.ptr.p_double[dstoffs] = v;
                    dstoffs = dstoffs+1;
                    vcorner = vcorner-v*s->utmp0.ptr.p_double[j-1];
                }
            }
            s->sparsel.idx.ptr.p_int[dstoffs] = m-1;
            s->sparsel.vals.ptr.p_double[dstoffs] = (double)(1);
            dstoffs = dstoffs+1;
            s->sparsel.ridx.ptr.p_int[m] = dstoffs;
            s->sparsel.ninitialized = s->sparsel.ridx.ptr.p_int[m];
            for(i=0; i<=m-1; i++)
            {
                j = s->sparsel.ridx.ptr.p_int[i+1];
                s->sparsel.didx.ptr.p_int[i] = j-1;
                s->sparsel.uidx.ptr.p_int[i] = j;
            }
            ae_assert(vcorner!=(double)0, "UpdateTrf: corner element is zero, degeneracy detected", _state);
            v = (double)1/vcorner;
            for(i=0; i<=m-2; i++)
            {
                s->densemu.ptr.p_double[s->trfage*m+i] = -s->utmp0.ptr.p_double[i]*v;
            }
            s->densemu.ptr.p_double[s->trfage*m+m-1] = v;
            
            /*
             * Multiply row permutation matrix by cyclic permutation applied to L
             */
            reviseddualsimplex_inversecyclicpermutation(&s->rowpermbwd, m, idxd, &s->utmpi, _state);
            
            /*
             * Done
             */
            inc(&s->trfage, _state);
            s->statupdt = s->statupdt+1;
            s->statoffdiag = s->statoffdiag+(double)(s->sparsel.ridx.ptr.p_int[m]-m)+(double)(s->sparseu.ridx.ptr.p_int[m]-m);
            processed = ae_true;
        }
        ae_assert(processed, "BasisUpdateTrf: unexpected TRF type", _state);
    }
    
    /*
     * Update pricing weights
     */
    ae_assert((settings->pricing==-1||settings->pricing==0)||settings->pricing==1, "BasisUpdateTrf: unexpected Settings.Pricing", _state);
    processed = ae_false;
    if( settings->pricing==-1 )
    {
        
        /*
         * Weights are recomputed from scratch at every step.
         * VERY, VERY time consuming, used only for debug purposes.
         */
        s->dsevalid = ae_false;
        reviseddualsimplex_basisrequestweights(s, settings, _state);
        processed = ae_true;
    }
    if( settings->pricing==0 )
    {
        
        /*
         * Weights are filled by 1.0
         */
        if( !s->dsevalid )
        {
            for(i=0; i<=m-1; i++)
            {
                s->dseweights.ptr.p_double[i] = 1.0;
            }
            s->dsevalid = ae_true;
        }
        processed = ae_true;
    }
    if( settings->pricing==1 )
    {
        
        /*
         * Weights are computed using DSE update formula.
         */
        if( s->dsevalid )
        {
            
            /*
             * Compute using update formula
             */
            for(i=0; i<=m-1; i++)
            {
                if( i!=r )
                {
                    s->dseweights.ptr.p_double[i] = s->dseweights.ptr.p_double[i]-(double)2*(alphaq->ptr.p_double[i]/alphaq->ptr.p_double[r])*tau->ptr.p_double[i]+s->dseweights.ptr.p_double[r]*ae_sqr(alphaq->ptr.p_double[i]/alphaq->ptr.p_double[r], _state);
                    s->dseweights.ptr.p_double[i] = ae_maxreal(s->dseweights.ptr.p_double[i], reviseddualsimplex_minbeta, _state);
                }
            }
            s->dseweights.ptr.p_double[r] = s->dseweights.ptr.p_double[r]/(alphaq->ptr.p_double[r]*alphaq->ptr.p_double[r]);
        }
        else
        {
            
            /*
             * No prior values, compute from scratch (usually it is done only once)
             */
            reviseddualsimplex_basisrequestweights(s, settings, _state);
        }
        processed = ae_true;
    }
    ae_assert(processed, "BasisUpdateTrf: unexpected pricing type", _state);
}


/*************************************************************************
This function computes solution to B*x=r.

Output array is reallocated if needed. Temporary array TmpX[] is used  and
reallocated if necessary.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basissolve(const dualsimplexbasis* s,
     /* Real    */ const ae_vector* r,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* tmpx,
     ae_state *_state)
{


    reviseddualsimplex_basissolvex(s, r, x, x, ae_false, tmpx, _state);
}


/*************************************************************************
This function computes solution to B*x=r. It  also   additionally  outputs
intermediate  result  of multiplication by inv(DS)*inv(U)*inv(colPerm),  a
value essential for Forest-Tomlin update.

Output arrays are reallocated if needed. Temporary  array  TX[]   can   be
used/reallocated.

If NeedIntermediate is False or Forest-Tomlin updates are not used,
then Xim[] is not referenced at all.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basissolvex(const dualsimplexbasis* s,
     /* Real    */ const ae_vector* r,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* xim,
     ae_bool needintermediate,
     /* Real    */ ae_vector* tx,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t i;
    ae_int_t d;
    ae_int_t k;
    double v;
    double vd;
    double vv;
    ae_bool processed;


    ae_assert(s->isvalidtrf, "BasisSolve: integrity check failed", _state);
    m = s->m;
    processed = ae_false;
    rvectorsetlengthatleast(tx, m, _state);
    
    /*
     * Dense/sparse factorizations with dense PFI
     *
     * NOTE: although we solve B*x=r, internally we store factorization of B^T
     */
    if( (s->trftype==0||s->trftype==1)||s->trftype==2 )
    {
        ae_assert(s->trfage==0||s->trftype!=0, "BasisSolve: integrity check failed TrfAge vs TrfType", _state);
        rvectorsetlengthatleast(x, m, _state);
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = r->ptr.p_double[s->colpermbwd.ptr.p_int[i]];
        }
        if( s->trftype==0||s->trftype==1 )
        {
            
            /*
             * Dense TRF
             */
            rmatrixtrsv(m, &s->denselu, 0, 0, ae_true, ae_false, 1, x, 0, _state);
            rmatrixtrsv(m, &s->denselu, 0, 0, ae_false, ae_true, 1, x, 0, _state);
        }
        else
        {
            
            /*
             * Sparse TRF
             */
            sparsetrsv(&s->sparseu, ae_true, ae_false, 1, x, _state);
            sparsetrsv(&s->sparsel, ae_false, ae_false, 1, x, _state);
        }
        for(i=0; i<=m-1; i++)
        {
            tx->ptr.p_double[s->rowpermbwd.ptr.p_int[i]] = x->ptr.p_double[i];
        }
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = tx->ptr.p_double[i];
        }
        for(k=0; k<=s->trfage-1; k++)
        {
            v = x->ptr.p_double[s->rk.ptr.p_int[k]];
            for(i=0; i<=m-1; i++)
            {
                x->ptr.p_double[i] = x->ptr.p_double[i]+s->densepfieta.ptr.p_double[k*m+i]*v;
            }
            x->ptr.p_double[s->rk.ptr.p_int[k]] = x->ptr.p_double[s->rk.ptr.p_int[k]]-v;
        }
        processed = ae_true;
    }
    
    /*
     * Sparse factorization with Forest-Tomlin update
     *
     * NOTE: although we solve B*x=r, internally we store factorization of B^T
     */
    if( s->trftype==3 )
    {
        rvectorsetlengthatleast(x, m, _state);
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = r->ptr.p_double[s->colpermbwd.ptr.p_int[i]];
        }
        sparsetrsv(&s->sparseu, ae_true, ae_false, 1, x, _state);
        for(k=0; k<=s->trfage-1; k++)
        {
            
            /*
             * The code below is an amalgamation of two parts:
             *
             * cyclic permutation
             * V:=X[D];
             * for I:=D to M-2 do
             *     X[I]:=X[I+1];
             * X[M-1]:=V;
             * 
             * and triangular factor
             * V:=0;
             * for I:=D to M-1 do
             *     V:=V+X[I]*S.DenseMu[K*M+I];
             * X[M-1]:=V;
             */
            d = s->dk.ptr.p_int[k];
            vv = (double)(0);
            vd = x->ptr.p_double[d];
            for(i=d; i<=m-2; i++)
            {
                v = x->ptr.p_double[i+1];
                x->ptr.p_double[i] = v;
                vv = vv+v*s->densemu.ptr.p_double[k*m+i];
            }
            x->ptr.p_double[m-1] = vv+vd*s->densemu.ptr.p_double[k*m+m-1];
        }
        if( needintermediate )
        {
            rvectorsetlengthatleast(xim, m, _state);
            for(i=0; i<=m-1; i++)
            {
                xim->ptr.p_double[i] = x->ptr.p_double[i];
            }
        }
        sparsetrsv(&s->sparsel, ae_false, ae_false, 1, x, _state);
        for(i=0; i<=m-1; i++)
        {
            tx->ptr.p_double[s->rowpermbwd.ptr.p_int[i]] = x->ptr.p_double[i];
        }
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = tx->ptr.p_double[i];
        }
        processed = ae_true;
    }
    
    /*
     * Integrity check
     */
    ae_assert(processed, "BasisSolve: unsupported TRF type", _state);
    v = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        v = v+x->ptr.p_double[i];
    }
    ae_assert(ae_isfinite(v, _state), "BasisSolve: integrity check failed (degeneracy in B?)", _state);
}


/*************************************************************************
This function computes solution to (B^T)*x=r.

Output array is reallocated if needed. TX[] temporary is reallocated if
needed

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_basissolvet(const dualsimplexbasis* s,
     /* Real    */ const ae_vector* r,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* tx,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t i;
    ae_int_t d;
    ae_int_t k;
    double v;
    double vm;
    ae_bool processed;


    ae_assert(s->isvalidtrf, "BasisSolveT: integrity check failed", _state);
    m = s->m;
    processed = ae_false;
    rvectorsetlengthatleast(tx, m, _state);
    
    /*
     * Dense factorizations
     */
    if( (s->trftype==0||s->trftype==1)||s->trftype==2 )
    {
        ae_assert(s->trfage==0||s->trftype!=0, "BasisSolveT: integrity check failed TrfAge vs TrfType", _state);
        rvectorsetlengthatleast(x, m, _state);
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = r->ptr.p_double[i];
        }
        for(k=s->trfage-1; k>=0; k--)
        {
            v = (double)(0);
            for(i=0; i<=m-1; i++)
            {
                v = v+s->densepfieta.ptr.p_double[k*m+i]*x->ptr.p_double[i];
            }
            x->ptr.p_double[s->rk.ptr.p_int[k]] = v;
        }
        for(i=0; i<=m-1; i++)
        {
            tx->ptr.p_double[i] = x->ptr.p_double[s->rowpermbwd.ptr.p_int[i]];
        }
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = tx->ptr.p_double[i];
        }
        if( s->trftype==0||s->trftype==1 )
        {
            
            /*
             * Dense TRF
             */
            rmatrixtrsv(m, &s->denselu, 0, 0, ae_false, ae_true, 0, x, 0, _state);
            rmatrixtrsv(m, &s->denselu, 0, 0, ae_true, ae_false, 0, x, 0, _state);
        }
        else
        {
            
            /*
             * Sparse TRF
             */
            sparsetrsv(&s->sparsel, ae_false, ae_false, 0, x, _state);
            sparsetrsv(&s->sparseu, ae_true, ae_false, 0, x, _state);
        }
        for(i=0; i<=m-1; i++)
        {
            tx->ptr.p_double[s->colpermbwd.ptr.p_int[i]] = x->ptr.p_double[i];
        }
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = tx->ptr.p_double[i];
        }
        processed = ae_true;
    }
    
    /*
     * Sparse factorization with Forest-Tomlin update
     */
    if( s->trftype==3 )
    {
        rvectorsetlengthatleast(x, m, _state);
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = r->ptr.p_double[i];
        }
        for(i=0; i<=m-1; i++)
        {
            tx->ptr.p_double[i] = x->ptr.p_double[s->rowpermbwd.ptr.p_int[i]];
        }
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = tx->ptr.p_double[i];
        }
        sparsetrsv(&s->sparsel, ae_false, ae_false, 0, x, _state);
        for(k=s->trfage-1; k>=0; k--)
        {
            
            /*
             * The code below is an amalgamation of two parts:
             *
             * triangular factor
             * V:=X[M-1];
             * for I:=D to M-2 do
             *     X[I]:=X[I]+S.DenseMu[K*M+I]*V;
             * X[M-1]:=S.DenseMu[K*M+(M-1)]*V;
             * 
             * inverse of cyclic permutation
             * V:=X[M-1];
             * for I:=M-1 downto D+1 do
             *     X[I]:=X[I-1];
             * X[D]:=V;
             */
            d = s->dk.ptr.p_int[k];
            vm = x->ptr.p_double[m-1];
            v = s->densemu.ptr.p_double[k*m+(m-1)]*vm;
            if( vm!=(double)0 )
            {
                
                /*
                 * X[M-1] is non-zero, apply update
                 */
                for(i=m-2; i>=d; i--)
                {
                    x->ptr.p_double[i+1] = x->ptr.p_double[i]+s->densemu.ptr.p_double[k*m+i]*vm;
                }
            }
            else
            {
                
                /*
                 * X[M-1] is zero, just cyclic permutation
                 */
                for(i=m-2; i>=d; i--)
                {
                    x->ptr.p_double[i+1] = x->ptr.p_double[i];
                }
            }
            x->ptr.p_double[d] = v;
        }
        sparsetrsv(&s->sparseut, ae_false, ae_false, 1, x, _state);
        for(i=0; i<=m-1; i++)
        {
            tx->ptr.p_double[s->colpermbwd.ptr.p_int[i]] = x->ptr.p_double[i];
        }
        for(i=0; i<=m-1; i++)
        {
            x->ptr.p_double[i] = tx->ptr.p_double[i];
        }
        processed = ae_true;
    }
    
    /*
     * Integrity check
     */
    ae_assert(processed, "BasisSolveT: unsupported TRF type", _state);
    v = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        v = v+x->ptr.p_double[i];
    }
    ae_assert(ae_isfinite(v, _state), "BasisSolveT: integrity check failed (degeneracy in B?)", _state);
}


/*************************************************************************
This function computes product AN*XN, where AN is a  non-basic  subset  of
columns of A, and XN is a non-basic subset of columns of X.

Output array is reallocated if its size is too small.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_computeanxn(const dualsimplexstate* state,
     const dualsimplexsubproblem* subproblem,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t nn;
    ae_int_t nx;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    double v;


    nx = subproblem->ns+subproblem->m;
    m = subproblem->m;
    nn = nx-m;
    
    /*
     * Integrity check
     */
    ae_assert(subproblem->state>=reviseddualsimplex_ssvalidxn, "ComputeANXN: XN is invalid", _state);
    
    /*
     * Compute
     */
    rvectorsetlengthatleast(y, m, _state);
    for(i=0; i<=m-1; i++)
    {
        y->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=nn-1; i++)
    {
        j0 = state->at.ridx.ptr.p_int[state->basis.nidx.ptr.p_int[i]];
        j1 = state->at.ridx.ptr.p_int[state->basis.nidx.ptr.p_int[i]+1]-1;
        v = x->ptr.p_double[state->basis.nidx.ptr.p_int[i]];
        for(j=j0; j<=j1; j++)
        {
            k = state->at.idx.ptr.p_int[j];
            y->ptr.p_double[k] = y->ptr.p_double[k]+v*state->at.vals.ptr.p_double[j];
        }
    }
}


/*************************************************************************
This function computes product (AN^T)*y, where AN is a non-basic subset of
columns of A, and y is some vector.

Output array is set to full NX-sized length, with basic components of  the
output being set to zeros.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_computeantv(const dualsimplexstate* state,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* r,
     ae_state *_state)
{
    ae_int_t nn;
    ae_int_t nx;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    double v;


    nx = state->ns+state->m;
    m = state->m;
    nn = nx-m;
    
    /*
     * Allocate output, set to zero
     */
    rvectorsetlengthatleast(r, nx, _state);
    for(i=0; i<=nx-1; i++)
    {
        r->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=nn-1; i++)
    {
        j0 = state->at.ridx.ptr.p_int[state->basis.nidx.ptr.p_int[i]];
        j1 = state->at.ridx.ptr.p_int[state->basis.nidx.ptr.p_int[i]+1]-1;
        v = (double)(0);
        for(j=j0; j<=j1; j++)
        {
            v = v+state->at.vals.ptr.p_double[j]*y->ptr.p_double[state->at.idx.ptr.p_int[j]];
        }
        r->ptr.p_double[state->basis.nidx.ptr.p_int[i]] = v;
    }
}


/*************************************************************************
Returns True if I-th lower bound is present

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool reviseddualsimplex_hasbndl(const dualsimplexsubproblem* subproblem,
     ae_int_t i,
     ae_state *_state)
{
    ae_int_t k;
    ae_bool result;


    k = subproblem->bndt.ptr.p_int[i];
    result = ae_false;
    if( (k==0||k==1)||k==3 )
    {
        result = ae_true;
        return result;
    }
    if( k==2||k==4 )
    {
        result = ae_false;
        return result;
    }
    ae_assert(ae_false, "HasBndL: integrity check failed", _state);
    return result;
}


/*************************************************************************
Returns True if I-th upper bound is present

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool reviseddualsimplex_hasbndu(const dualsimplexsubproblem* subproblem,
     ae_int_t i,
     ae_state *_state)
{
    ae_int_t k;
    ae_bool result;


    k = subproblem->bndt.ptr.p_int[i];
    result = ae_false;
    if( (k==0||k==2)||k==3 )
    {
        result = ae_true;
        return result;
    }
    if( k==1||k==4 )
    {
        result = ae_false;
        return result;
    }
    ae_assert(ae_false, "HasBndL: integrity check failed", _state);
    return result;
}


/*************************************************************************
Returns True if I-th variable if free

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool reviseddualsimplex_isfree(const dualsimplexsubproblem* subproblem,
     ae_int_t i,
     ae_state *_state)
{
    ae_int_t k;
    ae_bool result;


    k = subproblem->bndt.ptr.p_int[i];
    result = ae_false;
    if( ((k==0||k==1)||k==2)||k==3 )
    {
        result = ae_false;
        return result;
    }
    if( k==4 )
    {
        result = ae_true;
        return result;
    }
    ae_assert(ae_false, "IsFree: integrity check failed", _state);
    return result;
}


/*************************************************************************
Downgrades problem state to the specified one (if status is lower than one
specified by user, nothing is changed)

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_downgradestate(dualsimplexsubproblem* subproblem,
     ae_int_t s,
     ae_state *_state)
{


    subproblem->state = ae_minint(subproblem->state, s, _state);
}


/*************************************************************************
Returns maximum dual infeasibility (only non-basic variables are  checked,
we assume that basic variables are good enough).

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static double reviseddualsimplex_dualfeasibilityerror(const dualsimplexstate* state,
     const dualsimplexsubproblem* s,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t nn;
    ae_int_t bndt;
    double result;


    nn = s->ns;
    ae_assert(s->state==reviseddualsimplex_ssvalid, "DualFeasibilityError: invalid X", _state);
    result = (double)(0);
    for(i=0; i<=nn-1; i++)
    {
        j = state->basis.nidx.ptr.p_int[i];
        bndt = s->bndt.ptr.p_int[j];
        if( bndt==reviseddualsimplex_ccfixed )
        {
            continue;
        }
        if( bndt==reviseddualsimplex_ccrange )
        {
            if( s->xa.ptr.p_double[j]==s->bndl.ptr.p_double[j] )
            {
                result = ae_maxreal(result, -s->d.ptr.p_double[j], _state);
                continue;
            }
            if( s->xa.ptr.p_double[j]==s->bndu.ptr.p_double[j] )
            {
                result = ae_maxreal(result, s->d.ptr.p_double[j], _state);
                continue;
            }
            ae_assert(ae_false, "DualFeasibilityError: integrity check failed", _state);
        }
        if( bndt==reviseddualsimplex_cclower )
        {
            ae_assert(s->xa.ptr.p_double[j]==s->bndl.ptr.p_double[j], "DualFeasibilityError: integrity check failed", _state);
            result = ae_maxreal(result, -s->d.ptr.p_double[j], _state);
            continue;
        }
        if( bndt==reviseddualsimplex_ccupper )
        {
            ae_assert(s->xa.ptr.p_double[j]==s->bndu.ptr.p_double[j], "DualFeasibilityError: integrity check failed", _state);
            result = ae_maxreal(result, s->d.ptr.p_double[j], _state);
            continue;
        }
        if( bndt==reviseddualsimplex_ccfree )
        {
            result = ae_maxreal(result, ae_fabs(s->d.ptr.p_double[j], _state), _state);
            continue;
        }
        ae_assert(ae_false, "DSSOptimize: integrity check failed (infeasible constraint)", _state);
    }
    return result;
}


/*************************************************************************
Returns True for dual feasible basis (some minor dual feasibility error is
allowed), False otherwise

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
static ae_bool reviseddualsimplex_isdualfeasible(const dualsimplexstate* state,
     const dualsimplexsubproblem* s,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_bool result;


    result = ae_fp_less_eq(reviseddualsimplex_dualfeasibilityerror(state, s, _state),settings->dtolabs);
    return result;
}


/*************************************************************************
Transforms sequence of pivot permutations P0*P1*...*Pm to forward/backward
permutation representation.

  -- ALGLIB --
     Copyright 12.09.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_pivottobwd(/* Integer */ const ae_vector* p,
     ae_int_t m,
     /* Integer */ ae_vector* bwd,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_int_t t;


    ivectorsetlengthatleast(bwd, m, _state);
    for(i=0; i<=m-1; i++)
    {
        bwd->ptr.p_int[i] = i;
    }
    for(i=0; i<=m-1; i++)
    {
        k = p->ptr.p_int[i];
        if( k!=i )
        {
            t = bwd->ptr.p_int[i];
            bwd->ptr.p_int[i] = bwd->ptr.p_int[k];
            bwd->ptr.p_int[k] = t;
        }
    }
}


/*************************************************************************
Applies inverse cyclic permutation of [D,M-1) (element D is moved to the end, the
rest of elements is shifted one position backward) to the already existing
permutation.

  -- ALGLIB --
     Copyright 12.09.2018 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_inversecyclicpermutation(/* Integer */ ae_vector* bwd,
     ae_int_t m,
     ae_int_t d,
     /* Integer */ ae_vector* tmpi,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;


    
    /*
     * update Bwd[]
     */
    k = bwd->ptr.p_int[d];
    for(i=d; i<=m-2; i++)
    {
        bwd->ptr.p_int[i] = bwd->ptr.p_int[i+1];
    }
    bwd->ptr.p_int[m-1] = k;
}


/*************************************************************************
Offloads basic components of X[], BndT[], BndL[], BndU[] to XB/BndTB/BndLB/BndUB.

  -- ALGLIB --
     Copyright 24.01.2019 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_offloadbasiccomponents(dualsimplexsubproblem* s,
     const dualsimplexbasis* basis,
     const dualsimplexsettings* settings,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t m;


    m = basis->m;
    for(i=0; i<=m-1; i++)
    {
        s->xb.ptr.p_double[i] = s->xa.ptr.p_double[basis->idx.ptr.p_int[i]];
        reviseddualsimplex_cacheboundinfo(s, i, basis->idx.ptr.p_int[i], settings, _state);
    }
}


/*************************************************************************
Recombines basic and non-basic components in X[]

  -- ALGLIB --
     Copyright 24.01.2019 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_recombinebasicnonbasicx(dualsimplexsubproblem* s,
     const dualsimplexbasis* basis,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t i;


    m = basis->m;
    for(i=0; i<=m-1; i++)
    {
        s->xa.ptr.p_double[basis->idx.ptr.p_int[i]] = s->xb.ptr.p_double[i];
    }
}


/*************************************************************************
Computes Stats array

INPUT PARAMETERS:
    S           -   problem, contains current solution at S.XA
    Basis       -   basis
    X           -   possibly preallocated output buffer
    LagBC       -   possibly preallocated output buffer
    LagLC       -   possibly preallocated output buffer
    Stats       -   possibly preallocated output buffer
    Buffers     -   temporary buffers

OUTPUT PARAMETERS:
    X           -   array[NS], solution
    LagBC       -   array[NS], Lagrange multipliers for box constraints
    LagLC       -   array[M], Lagrange multipliers for linear constraints
    Stats       -   array[NS+M], primary/slack variable stats:
                    * -1 = variable at lower bound
                    * +1 = variable at upper bound
                    *  0 = basic or free (possibly nonbasic) variable
                    fixed variables may be set to +1 or -1

  -- ALGLIB --
     Copyright 24.01.2019 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_setxydstats(const dualsimplexstate* state,
     const dualsimplexsubproblem* s,
     const dualsimplexbasis* basis,
     apbuffers* buffers,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     /* Integer */ ae_vector* stats,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t m;
    ae_int_t ns;
    ae_int_t nx;


    
    /*
     * Prepare
     */
    m = s->m;
    ns = s->ns;
    nx = s->ns+s->m;
    rvectorsetlengthatleast(x, ns, _state);
    rvectorsetlengthatleast(laglc, m, _state);
    ivectorsetlengthatleast(stats, nx, _state);
    rsetallocv(ns, 0.0, lagbc, _state);
    
    /*
     * Compute Y (in Buffers.RA1) and D (in Buffers.RA3)
     */
    rvectorsetlengthatleast(&buffers->ra0, m, _state);
    rvectorsetlengthatleast(&buffers->ra1, m, _state);
    rvectorsetlengthatleast(&buffers->ra3, nx, _state);
    for(i=0; i<=m-1; i++)
    {
        buffers->ra0.ptr.p_double[i] = s->rawc.ptr.p_double[basis->idx.ptr.p_int[i]];
    }
    reviseddualsimplex_basissolvet(basis, &buffers->ra0, &buffers->ra1, &buffers->ra2, _state);
    reviseddualsimplex_computeantv(state, &buffers->ra1, &buffers->ra3, _state);
    for(i=0; i<=ns-1; i++)
    {
        j = state->basis.nidx.ptr.p_int[i];
        buffers->ra3.ptr.p_double[j] = state->primary.rawc.ptr.p_double[j]-buffers->ra3.ptr.p_double[j];
        if( j<ns )
        {
            lagbc->ptr.p_double[j] = -buffers->ra3.ptr.p_double[j];
        }
    }
    for(i=0; i<=m-1; i++)
    {
        buffers->ra3.ptr.p_double[state->basis.idx.ptr.p_int[i]] = (double)(0);
    }
    
    /*
     * Compute X, Y, Stats
     */
    for(i=0; i<=ns-1; i++)
    {
        x->ptr.p_double[i] = s->xa.ptr.p_double[i];
        if( ae_isfinite(state->rawbndl.ptr.p_double[i], _state) )
        {
            x->ptr.p_double[i] = ae_maxreal(x->ptr.p_double[i], state->rawbndl.ptr.p_double[i], _state);
        }
        if( ae_isfinite(state->rawbndu.ptr.p_double[i], _state) )
        {
            x->ptr.p_double[i] = ae_minreal(x->ptr.p_double[i], state->rawbndu.ptr.p_double[i], _state);
        }
    }
    for(i=0; i<=ns-1; i++)
    {
        if( basis->isbasic.ptr.p_bool[i] )
        {
            lagbc->ptr.p_double[i] = (double)(0);
            continue;
        }
        if( s->bndt.ptr.p_int[i]==reviseddualsimplex_ccfixed )
        {
            continue;
        }
        if( reviseddualsimplex_hasbndl(s, i, _state)&&ae_fp_eq(s->xa.ptr.p_double[i],s->bndl.ptr.p_double[i]) )
        {
            lagbc->ptr.p_double[i] = ae_minreal(lagbc->ptr.p_double[i], 0.0, _state);
            continue;
        }
        if( reviseddualsimplex_hasbndu(s, i, _state)&&ae_fp_eq(s->xa.ptr.p_double[i],s->bndu.ptr.p_double[i]) )
        {
            lagbc->ptr.p_double[i] = ae_maxreal(lagbc->ptr.p_double[i], 0.0, _state);
            continue;
        }
        ae_assert(!reviseddualsimplex_hasbndl(s, i, _state)&&!reviseddualsimplex_hasbndu(s, i, _state), "SetStats: integrity check failed (zetta5)", _state);
        lagbc->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=m-1; i++)
    {
        laglc->ptr.p_double[i] = -buffers->ra1.ptr.p_double[i]/state->rowscales.ptr.p_double[i];
    }
    for(i=0; i<=nx-1; i++)
    {
        if( basis->isbasic.ptr.p_bool[i] )
        {
            stats->ptr.p_int[i] = 0;
            continue;
        }
        if( reviseddualsimplex_hasbndl(s, i, _state)&&ae_fp_eq(s->xa.ptr.p_double[i],s->bndl.ptr.p_double[i]) )
        {
            stats->ptr.p_int[i] = -1;
            continue;
        }
        if( reviseddualsimplex_hasbndu(s, i, _state)&&ae_fp_eq(s->xa.ptr.p_double[i],s->bndu.ptr.p_double[i]) )
        {
            stats->ptr.p_int[i] = 1;
            continue;
        }
        ae_assert(!reviseddualsimplex_hasbndl(s, i, _state)&&!reviseddualsimplex_hasbndu(s, i, _state), "SetStats: integrity check failed (zetta5)", _state);
        stats->ptr.p_int[i] = 0;
    }
}


/*************************************************************************
Initializes vector, sets all internal arrays to length N (so that  we  may
store any vector without reallocation).  Previously  allocated  memory  is
reused as much as possible.

No zero-filling is performed, X.K is undefined. Only X.N is set.

INPUT PARAMETERS:
    X           -   temporary buffers

OUTPUT PARAMETERS:
    X           -   preallocated vector, X.N=N, contents undefined

  -- ALGLIB --
     Copyright 24.07.2020 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_dvalloc(dssvector* x,
     ae_int_t n,
     ae_state *_state)
{


    ivectorsetlengthatleast(&x->idx, n, _state);
    rvectorsetlengthatleast(&x->vals, n, _state);
    rvectorsetlengthatleast(&x->dense, n, _state);
    x->n = n;
}


/*************************************************************************
Initializes vector, sets all internal arrays to length  N  and  zero-fills
them. Previously allocated memory is reused as much as possible.

INPUT PARAMETERS:
    X           -   temporary buffers

OUTPUT PARAMETERS:
    X           -   preallocated vector:
                    * X.N=N
                    * X.K=0
                    * X.Dense is zero-filled.

  -- ALGLIB --
     Copyright 24.07.2020 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_dvinit(dssvector* x,
     ae_int_t n,
     ae_state *_state)
{


    ivectorsetlengthatleast(&x->idx, n, _state);
    rvectorsetlengthatleast(&x->vals, n, _state);
    rvectorsetlengthatleast(&x->dense, n, _state);
    rsetv(n, 0.0, &x->dense, _state);
    x->n = n;
    x->k = 0;
}


/*************************************************************************
Copies dense part to sparse one.

INPUT PARAMETERS:
    X           -   allocated vector; dense part must be valid

OUTPUT PARAMETERS:
    X           -   both dense and sparse parts are valid.

  -- ALGLIB --
     Copyright 24.07.2020 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_dvdensetosparse(dssvector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t k;
    double v;


    n = x->n;
    ivectorsetlengthatleast(&x->idx, n, _state);
    rvectorsetlengthatleast(&x->vals, n, _state);
    k = 0;
    for(i=0; i<=n-1; i++)
    {
        v = x->dense.ptr.p_double[i];
        if( v!=0.0 )
        {
            x->idx.ptr.p_int[k] = i;
            x->vals.ptr.p_double[k] = v;
            k = k+1;
        }
    }
    x->k = k;
}


/*************************************************************************
Copies sparse part to dense one.

INPUT PARAMETERS:
    X           -   allocated vector; sparse part must be valid

OUTPUT PARAMETERS:
    X           -   both dense and sparse parts are valid.

  -- ALGLIB --
     Copyright 24.07.2020 by Bochkanov Sergey
*************************************************************************/
static void reviseddualsimplex_dvsparsetodense(dssvector* x,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t k;


    n = x->n;
    k = x->k;
    rsetv(n, 0.0, &x->dense, _state);
    for(i=0; i<=k-1; i++)
    {
        x->dense.ptr.p_double[x->idx.ptr.p_int[i]] = x->vals.ptr.p_double[i];
    }
}


static double reviseddualsimplex_sparsityof(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    double mx;
    double result;


    if( n<=1 )
    {
        result = (double)(0);
        return result;
    }
    mx = 1.0;
    for(i=0; i<=n-1; i++)
    {
        mx = ae_maxreal(mx, ae_fabs(x->ptr.p_double[i], _state), _state);
    }
    mx = 1.0E5*ae_machineepsilon*mx;
    k = 0;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_greater(ae_fabs(x->ptr.p_double[i], _state),mx) )
        {
            k = k+1;
        }
    }
    result = (double)k/(double)n;
    return result;
}


static void reviseddualsimplex_updateavgcounter(double v,
     double* acc,
     ae_int_t* cnt,
     ae_state *_state)
{


    *acc = *acc+v;
    *cnt = *cnt+1;
}


void _dualsimplexsettings_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dualsimplexsettings *p = (dualsimplexsettings*)_p;
    ae_touch_ptr((void*)p);
}


void _dualsimplexsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dualsimplexsettings       *dst = (dualsimplexsettings*)_dst;
    const dualsimplexsettings *src = (const dualsimplexsettings*)_src;
    dst->pivottol = src->pivottol;
    dst->perturbmag = src->perturbmag;
    dst->maxtrfage = src->maxtrfage;
    dst->trftype = src->trftype;
    dst->ratiotest = src->ratiotest;
    dst->pricing = src->pricing;
    dst->shifting = src->shifting;
    dst->xtolabs = src->xtolabs;
    dst->xtolrelabs = src->xtolrelabs;
    dst->dtolabs = src->dtolabs;
}


void _dualsimplexsettings_clear(void* _p)
{
    dualsimplexsettings *p = (dualsimplexsettings*)_p;
    ae_touch_ptr((void*)p);
}


void _dualsimplexsettings_destroy(void* _p)
{
    dualsimplexsettings *p = (dualsimplexsettings*)_p;
    ae_touch_ptr((void*)p);
}


void _dssvector_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dssvector *p = (dssvector*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->idx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->vals, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dense, 0, DT_REAL, _state, make_automatic);
}


void _dssvector_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dssvector       *dst = (dssvector*)_dst;
    const dssvector *src = (const dssvector*)_src;
    dst->n = src->n;
    dst->k = src->k;
    ae_vector_init_copy(&dst->idx, &src->idx, _state, make_automatic);
    ae_vector_init_copy(&dst->vals, &src->vals, _state, make_automatic);
    ae_vector_init_copy(&dst->dense, &src->dense, _state, make_automatic);
}


void _dssvector_clear(void* _p)
{
    dssvector *p = (dssvector*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->idx);
    ae_vector_clear(&p->vals);
    ae_vector_clear(&p->dense);
}


void _dssvector_destroy(void* _p)
{
    dssvector *p = (dssvector*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->idx);
    ae_vector_destroy(&p->vals);
    ae_vector_destroy(&p->dense);
}


void _dualsimplexbasis_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dualsimplexbasis *p = (dualsimplexbasis*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->idx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->nidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->isbasic, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->denselu, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsel, _state, make_automatic);
    _sparsematrix_init(&p->sparseu, _state, make_automatic);
    _sparsematrix_init(&p->sparseut, _state, make_automatic);
    ae_vector_init(&p->rowpermbwd, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->colpermbwd, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->densepfieta, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->densemu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rk, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->dk, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->dseweights, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wtmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wtmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wtmp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nrs, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->tcinvidx, 0, DT_INT, _state, make_automatic);
    ae_matrix_init(&p->denselu2, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->densep2, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->densep2c, 0, DT_INT, _state, make_automatic);
    _sparsematrix_init(&p->sparselu1, _state, make_automatic);
    _sparsematrix_init(&p->sparselu2, _state, make_automatic);
    _sluv2buffer_init(&p->lubuf2, _state, make_automatic);
    ae_vector_init(&p->tmpi, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->utmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->utmpi, 0, DT_INT, _state, make_automatic);
    _sparsematrix_init(&p->sparseludbg, _state, make_automatic);
}


void _dualsimplexbasis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dualsimplexbasis       *dst = (dualsimplexbasis*)_dst;
    const dualsimplexbasis *src = (const dualsimplexbasis*)_src;
    dst->ns = src->ns;
    dst->m = src->m;
    ae_vector_init_copy(&dst->idx, &src->idx, _state, make_automatic);
    ae_vector_init_copy(&dst->nidx, &src->nidx, _state, make_automatic);
    ae_vector_init_copy(&dst->isbasic, &src->isbasic, _state, make_automatic);
    dst->trftype = src->trftype;
    dst->isvalidtrf = src->isvalidtrf;
    dst->trfage = src->trfage;
    ae_matrix_init_copy(&dst->denselu, &src->denselu, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsel, &src->sparsel, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseu, &src->sparseu, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseut, &src->sparseut, _state, make_automatic);
    ae_vector_init_copy(&dst->rowpermbwd, &src->rowpermbwd, _state, make_automatic);
    ae_vector_init_copy(&dst->colpermbwd, &src->colpermbwd, _state, make_automatic);
    ae_vector_init_copy(&dst->densepfieta, &src->densepfieta, _state, make_automatic);
    ae_vector_init_copy(&dst->densemu, &src->densemu, _state, make_automatic);
    ae_vector_init_copy(&dst->rk, &src->rk, _state, make_automatic);
    ae_vector_init_copy(&dst->dk, &src->dk, _state, make_automatic);
    ae_vector_init_copy(&dst->dseweights, &src->dseweights, _state, make_automatic);
    dst->dsevalid = src->dsevalid;
    dst->eminu = src->eminu;
    dst->statfact = src->statfact;
    dst->statupdt = src->statupdt;
    dst->statoffdiag = src->statoffdiag;
    ae_vector_init_copy(&dst->wtmp0, &src->wtmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->wtmp1, &src->wtmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->wtmp2, &src->wtmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->nrs, &src->nrs, _state, make_automatic);
    ae_vector_init_copy(&dst->tcinvidx, &src->tcinvidx, _state, make_automatic);
    ae_matrix_init_copy(&dst->denselu2, &src->denselu2, _state, make_automatic);
    ae_vector_init_copy(&dst->densep2, &src->densep2, _state, make_automatic);
    ae_vector_init_copy(&dst->densep2c, &src->densep2c, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparselu1, &src->sparselu1, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparselu2, &src->sparselu2, _state, make_automatic);
    _sluv2buffer_init_copy(&dst->lubuf2, &src->lubuf2, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpi, &src->tmpi, _state, make_automatic);
    ae_vector_init_copy(&dst->utmp0, &src->utmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->utmpi, &src->utmpi, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseludbg, &src->sparseludbg, _state, make_automatic);
}


void _dualsimplexbasis_clear(void* _p)
{
    dualsimplexbasis *p = (dualsimplexbasis*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->idx);
    ae_vector_clear(&p->nidx);
    ae_vector_clear(&p->isbasic);
    ae_matrix_clear(&p->denselu);
    _sparsematrix_clear(&p->sparsel);
    _sparsematrix_clear(&p->sparseu);
    _sparsematrix_clear(&p->sparseut);
    ae_vector_clear(&p->rowpermbwd);
    ae_vector_clear(&p->colpermbwd);
    ae_vector_clear(&p->densepfieta);
    ae_vector_clear(&p->densemu);
    ae_vector_clear(&p->rk);
    ae_vector_clear(&p->dk);
    ae_vector_clear(&p->dseweights);
    ae_vector_clear(&p->wtmp0);
    ae_vector_clear(&p->wtmp1);
    ae_vector_clear(&p->wtmp2);
    ae_vector_clear(&p->nrs);
    ae_vector_clear(&p->tcinvidx);
    ae_matrix_clear(&p->denselu2);
    ae_vector_clear(&p->densep2);
    ae_vector_clear(&p->densep2c);
    _sparsematrix_clear(&p->sparselu1);
    _sparsematrix_clear(&p->sparselu2);
    _sluv2buffer_clear(&p->lubuf2);
    ae_vector_clear(&p->tmpi);
    ae_vector_clear(&p->utmp0);
    ae_vector_clear(&p->utmpi);
    _sparsematrix_clear(&p->sparseludbg);
}


void _dualsimplexbasis_destroy(void* _p)
{
    dualsimplexbasis *p = (dualsimplexbasis*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->idx);
    ae_vector_destroy(&p->nidx);
    ae_vector_destroy(&p->isbasic);
    ae_matrix_destroy(&p->denselu);
    _sparsematrix_destroy(&p->sparsel);
    _sparsematrix_destroy(&p->sparseu);
    _sparsematrix_destroy(&p->sparseut);
    ae_vector_destroy(&p->rowpermbwd);
    ae_vector_destroy(&p->colpermbwd);
    ae_vector_destroy(&p->densepfieta);
    ae_vector_destroy(&p->densemu);
    ae_vector_destroy(&p->rk);
    ae_vector_destroy(&p->dk);
    ae_vector_destroy(&p->dseweights);
    ae_vector_destroy(&p->wtmp0);
    ae_vector_destroy(&p->wtmp1);
    ae_vector_destroy(&p->wtmp2);
    ae_vector_destroy(&p->nrs);
    ae_vector_destroy(&p->tcinvidx);
    ae_matrix_destroy(&p->denselu2);
    ae_vector_destroy(&p->densep2);
    ae_vector_destroy(&p->densep2c);
    _sparsematrix_destroy(&p->sparselu1);
    _sparsematrix_destroy(&p->sparselu2);
    _sluv2buffer_destroy(&p->lubuf2);
    ae_vector_destroy(&p->tmpi);
    ae_vector_destroy(&p->utmp0);
    ae_vector_destroy(&p->utmpi);
    _sparsematrix_destroy(&p->sparseludbg);
}


void _dualsimplexsubproblem_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dualsimplexsubproblem *p = (dualsimplexsubproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->rawc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndt, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->xa, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndlb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndub, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndtb, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->bndtollb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndtolub, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->effc, 0, DT_REAL, _state, make_automatic);
}


void _dualsimplexsubproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dualsimplexsubproblem       *dst = (dualsimplexsubproblem*)_dst;
    const dualsimplexsubproblem *src = (const dualsimplexsubproblem*)_src;
    dst->ns = src->ns;
    dst->m = src->m;
    ae_vector_init_copy(&dst->rawc, &src->rawc, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->bndt, &src->bndt, _state, make_automatic);
    ae_vector_init_copy(&dst->xa, &src->xa, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    dst->state = src->state;
    ae_vector_init_copy(&dst->xb, &src->xb, _state, make_automatic);
    ae_vector_init_copy(&dst->bndlb, &src->bndlb, _state, make_automatic);
    ae_vector_init_copy(&dst->bndub, &src->bndub, _state, make_automatic);
    ae_vector_init_copy(&dst->bndtb, &src->bndtb, _state, make_automatic);
    ae_vector_init_copy(&dst->bndtollb, &src->bndtollb, _state, make_automatic);
    ae_vector_init_copy(&dst->bndtolub, &src->bndtolub, _state, make_automatic);
    ae_vector_init_copy(&dst->effc, &src->effc, _state, make_automatic);
}


void _dualsimplexsubproblem_clear(void* _p)
{
    dualsimplexsubproblem *p = (dualsimplexsubproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->rawc);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->bndt);
    ae_vector_clear(&p->xa);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->xb);
    ae_vector_clear(&p->bndlb);
    ae_vector_clear(&p->bndub);
    ae_vector_clear(&p->bndtb);
    ae_vector_clear(&p->bndtollb);
    ae_vector_clear(&p->bndtolub);
    ae_vector_clear(&p->effc);
}


void _dualsimplexsubproblem_destroy(void* _p)
{
    dualsimplexsubproblem *p = (dualsimplexsubproblem*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->rawc);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->bndt);
    ae_vector_destroy(&p->xa);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->xb);
    ae_vector_destroy(&p->bndlb);
    ae_vector_destroy(&p->bndub);
    ae_vector_destroy(&p->bndtb);
    ae_vector_destroy(&p->bndtollb);
    ae_vector_destroy(&p->bndtolub);
    ae_vector_destroy(&p->effc);
}


void _dualsimplexstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dualsimplexstate *p = (dualsimplexstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->rowscales, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawbndu, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->a, _state, make_automatic);
    _sparsematrix_init(&p->at, _state, make_automatic);
    _dualsimplexbasis_init(&p->basis, _state, make_automatic);
    _dualsimplexsubproblem_init(&p->primary, _state, make_automatic);
    _dualsimplexsubproblem_init(&p->phase1, _state, make_automatic);
    _dualsimplexsubproblem_init(&p->phase3, _state, make_automatic);
    ae_vector_init(&p->repx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replagbc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->replaglc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->repstats, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->btrantmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->btrantmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->btrantmp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ftrantmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ftrantmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->possibleflips, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->dfctmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dfctmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dfctmp2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ustmpi, 0, DT_INT, _state, make_automatic);
    _apbuffers_init(&p->xydsbuf, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp2, 0, DT_REAL, _state, make_automatic);
    _dssvector_init(&p->alphar, _state, make_automatic);
    _dssvector_init(&p->rhor, _state, make_automatic);
    ae_vector_init(&p->tau, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->alphaq, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->alphaqim, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->eligiblealphar, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->harrisset, 0, DT_INT, _state, make_automatic);
}


void _dualsimplexstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dualsimplexstate       *dst = (dualsimplexstate*)_dst;
    const dualsimplexstate *src = (const dualsimplexstate*)_src;
    ae_vector_init_copy(&dst->rowscales, &src->rowscales, _state, make_automatic);
    ae_vector_init_copy(&dst->rawbndl, &src->rawbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->rawbndu, &src->rawbndu, _state, make_automatic);
    dst->ns = src->ns;
    dst->m = src->m;
    _sparsematrix_init_copy(&dst->a, &src->a, _state, make_automatic);
    _sparsematrix_init_copy(&dst->at, &src->at, _state, make_automatic);
    _dualsimplexbasis_init_copy(&dst->basis, &src->basis, _state, make_automatic);
    _dualsimplexsubproblem_init_copy(&dst->primary, &src->primary, _state, make_automatic);
    _dualsimplexsubproblem_init_copy(&dst->phase1, &src->phase1, _state, make_automatic);
    _dualsimplexsubproblem_init_copy(&dst->phase3, &src->phase3, _state, make_automatic);
    ae_vector_init_copy(&dst->repx, &src->repx, _state, make_automatic);
    ae_vector_init_copy(&dst->replagbc, &src->replagbc, _state, make_automatic);
    ae_vector_init_copy(&dst->replaglc, &src->replaglc, _state, make_automatic);
    ae_vector_init_copy(&dst->repstats, &src->repstats, _state, make_automatic);
    dst->repterminationtype = src->repterminationtype;
    dst->repiterationscount = src->repiterationscount;
    dst->repiterationscount1 = src->repiterationscount1;
    dst->repiterationscount2 = src->repiterationscount2;
    dst->repiterationscount3 = src->repiterationscount3;
    dst->repphase1time = src->repphase1time;
    dst->repphase2time = src->repphase2time;
    dst->repphase3time = src->repphase3time;
    dst->repdualpricingtime = src->repdualpricingtime;
    dst->repdualbtrantime = src->repdualbtrantime;
    dst->repdualpivotrowtime = src->repdualpivotrowtime;
    dst->repdualratiotesttime = src->repdualratiotesttime;
    dst->repdualftrantime = src->repdualftrantime;
    dst->repdualupdatesteptime = src->repdualupdatesteptime;
    dst->repfillpivotrow = src->repfillpivotrow;
    dst->repfillpivotrowcnt = src->repfillpivotrowcnt;
    dst->repfillrhor = src->repfillrhor;
    dst->repfillrhorcnt = src->repfillrhorcnt;
    dst->repfilldensemu = src->repfilldensemu;
    dst->repfilldensemucnt = src->repfilldensemucnt;
    dst->dotrace = src->dotrace;
    dst->dodetailedtrace = src->dodetailedtrace;
    dst->dotimers = src->dotimers;
    ae_vector_init_copy(&dst->btrantmp0, &src->btrantmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->btrantmp1, &src->btrantmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->btrantmp2, &src->btrantmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->ftrantmp0, &src->ftrantmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->ftrantmp1, &src->ftrantmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->possibleflips, &src->possibleflips, _state, make_automatic);
    dst->possibleflipscnt = src->possibleflipscnt;
    ae_vector_init_copy(&dst->dfctmp0, &src->dfctmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->dfctmp1, &src->dfctmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->dfctmp2, &src->dfctmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->ustmpi, &src->ustmpi, _state, make_automatic);
    _apbuffers_init_copy(&dst->xydsbuf, &src->xydsbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    _dssvector_init_copy(&dst->alphar, &src->alphar, _state, make_automatic);
    _dssvector_init_copy(&dst->rhor, &src->rhor, _state, make_automatic);
    ae_vector_init_copy(&dst->tau, &src->tau, _state, make_automatic);
    ae_vector_init_copy(&dst->alphaq, &src->alphaq, _state, make_automatic);
    ae_vector_init_copy(&dst->alphaqim, &src->alphaqim, _state, make_automatic);
    ae_vector_init_copy(&dst->eligiblealphar, &src->eligiblealphar, _state, make_automatic);
    ae_vector_init_copy(&dst->harrisset, &src->harrisset, _state, make_automatic);
}


void _dualsimplexstate_clear(void* _p)
{
    dualsimplexstate *p = (dualsimplexstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->rowscales);
    ae_vector_clear(&p->rawbndl);
    ae_vector_clear(&p->rawbndu);
    _sparsematrix_clear(&p->a);
    _sparsematrix_clear(&p->at);
    _dualsimplexbasis_clear(&p->basis);
    _dualsimplexsubproblem_clear(&p->primary);
    _dualsimplexsubproblem_clear(&p->phase1);
    _dualsimplexsubproblem_clear(&p->phase3);
    ae_vector_clear(&p->repx);
    ae_vector_clear(&p->replagbc);
    ae_vector_clear(&p->replaglc);
    ae_vector_clear(&p->repstats);
    ae_vector_clear(&p->btrantmp0);
    ae_vector_clear(&p->btrantmp1);
    ae_vector_clear(&p->btrantmp2);
    ae_vector_clear(&p->ftrantmp0);
    ae_vector_clear(&p->ftrantmp1);
    ae_vector_clear(&p->possibleflips);
    ae_vector_clear(&p->dfctmp0);
    ae_vector_clear(&p->dfctmp1);
    ae_vector_clear(&p->dfctmp2);
    ae_vector_clear(&p->ustmpi);
    _apbuffers_clear(&p->xydsbuf);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmp2);
    _dssvector_clear(&p->alphar);
    _dssvector_clear(&p->rhor);
    ae_vector_clear(&p->tau);
    ae_vector_clear(&p->alphaq);
    ae_vector_clear(&p->alphaqim);
    ae_vector_clear(&p->eligiblealphar);
    ae_vector_clear(&p->harrisset);
}


void _dualsimplexstate_destroy(void* _p)
{
    dualsimplexstate *p = (dualsimplexstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->rowscales);
    ae_vector_destroy(&p->rawbndl);
    ae_vector_destroy(&p->rawbndu);
    _sparsematrix_destroy(&p->a);
    _sparsematrix_destroy(&p->at);
    _dualsimplexbasis_destroy(&p->basis);
    _dualsimplexsubproblem_destroy(&p->primary);
    _dualsimplexsubproblem_destroy(&p->phase1);
    _dualsimplexsubproblem_destroy(&p->phase3);
    ae_vector_destroy(&p->repx);
    ae_vector_destroy(&p->replagbc);
    ae_vector_destroy(&p->replaglc);
    ae_vector_destroy(&p->repstats);
    ae_vector_destroy(&p->btrantmp0);
    ae_vector_destroy(&p->btrantmp1);
    ae_vector_destroy(&p->btrantmp2);
    ae_vector_destroy(&p->ftrantmp0);
    ae_vector_destroy(&p->ftrantmp1);
    ae_vector_destroy(&p->possibleflips);
    ae_vector_destroy(&p->dfctmp0);
    ae_vector_destroy(&p->dfctmp1);
    ae_vector_destroy(&p->dfctmp2);
    ae_vector_destroy(&p->ustmpi);
    _apbuffers_destroy(&p->xydsbuf);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmp2);
    _dssvector_destroy(&p->alphar);
    _dssvector_destroy(&p->rhor);
    ae_vector_destroy(&p->tau);
    ae_vector_destroy(&p->alphaq);
    ae_vector_destroy(&p->alphaqim);
    ae_vector_destroy(&p->eligiblealphar);
    ae_vector_destroy(&p->harrisset);
}


/*$ End $*/
