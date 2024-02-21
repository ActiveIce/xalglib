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
#include "ipm2solver.h"


/*$ Declarations $*/
static double ipm2solver_muquasidense = 5.0;
static double ipm2solver_mupromote = 3.0;
static ae_int_t ipm2solver_maxipmits = 200;
static double ipm2solver_initslackval = 100.0;
static double ipm2solver_steplengthdecay = 0.95;
static double ipm2solver_stagnationdelta = 0.99999;
static double ipm2solver_primalinfeasible1 = 1.0E-3;
static double ipm2solver_dualinfeasible1 = 1.0E-3;
static double ipm2solver_bigy = 1.0E8;
static double ipm2solver_ygrowth = 1.0E6;
static ae_int_t ipm2solver_itersfortoostringentcond = 25;
static ae_int_t ipm2solver_minitersbeforeinfeasible = 3;
static ae_int_t ipm2solver_minitersbeforestagnation = 5;
static ae_int_t ipm2solver_minitersbeforeeworststagnation = 25;
static ae_int_t ipm2solver_primalstagnationlen = 5;
static ae_int_t ipm2solver_dualstagnationlen = 7;
static double ipm2solver_minitersbeforesafeguards = 5;
static double ipm2solver_badsteplength = 1.0E-3;
static void ipm2solver_varsinitbyzero(ipm2vars* vstate,
     ae_int_t ntotal,
     ae_int_t m,
     ae_state *_state);
static void ipm2solver_varsinitfrom(ipm2vars* vstate,
     const ipm2vars* vsrc,
     ae_state *_state);
static void ipm2solver_varsaddstep(ipm2vars* vstate,
     const ipm2vars* vdir,
     double stp,
     ae_state *_state);
static double ipm2solver_varscomputecomplementaritygap(const ipm2vars* vstate,
     ae_state *_state);
static double ipm2solver_varscomputemu(const ipm2state* state,
     const ipm2vars* vstate,
     ae_state *_state);
static double ipm2solver_varscomputemutrial(const ipm2state* state,
     const ipm2vars* current,
     const ipm2vars* delta,
     double alpha,
     ae_state *_state);
static double ipm2solver_ipm2target(const ipm2state* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
static void ipm2solver_multiplygeax(const ipm2state* state,
     double alpha,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t offsax,
     ae_state *_state);
static void ipm2solver_multiplygeatx(const ipm2state* state,
     double alpha,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
static void ipm2solver_multiplyhx(const ipm2state* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     ae_state *_state);
static void ipm2solver_ipm2multiply(const ipm2state* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* hx,
     /* Real    */ ae_vector* ax,
     /* Real    */ ae_vector* aty,
     ae_state *_state);
static void ipm2solver_ipm2powerup(ipm2state* state,
     double regfree,
     ae_state *_state);
static void ipm2solver_reducedsystempowerup(ipm2reducedsystem* s,
     const ipm2state* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
static ae_bool ipm2solver_reducedsystemcomputefactorization(ipm2reducedsystem* redsys,
     ipm2state* sstate,
     const ipm2vars* current,
     double dampepsmu,
     double dampepszs,
     double dampepsp,
     double dampepsd,
     double regxy,
     ae_state *_state);
static void ipm2solver_ipm2computesteplength(const ipm2state* state,
     const ipm2vars* current,
     const ipm2vars* delta,
     double* alpha,
     ae_state *_state);
static void ipm2solver_computeerrors(const ipm2state* state,
     const ipm2righthandside* rhs,
     /* Real    */ const ae_vector* currenthx,
     /* Real    */ const ae_vector* currentax,
     /* Real    */ const ae_vector* currentaty,
     double* errp2,
     double* errd2,
     double* errpinf,
     double* errdinf,
     double* egap,
     ae_state *_state);
static void ipm2solver_traceprogress(ipm2state* state,
     double muinit,
     double alphaaff,
     double alphafin,
     ae_state *_state);
static void ipm2solver_rhscomputeprimdual(const ipm2state* state,
     const ipm2vars* current,
     /* Real    */ const ae_vector* currenthx,
     /* Real    */ const ae_vector* currentax,
     /* Real    */ const ae_vector* currentaty,
     ipm2righthandside* rhs,
     ae_state *_state);
static void ipm2solver_rhsrecomputegammapredictor(const ipm2state* state,
     const ipm2vars* current,
     double mutarget,
     ipm2righthandside* rhs,
     ae_state *_state);
static void ipm2solver_rhsrecomputegammamehrotra(const ipm2state* state,
     const ipm2vars* current,
     double mutarget,
     const ipm2vars* deltaaff,
     ipm2righthandside* rhs,
     ae_state *_state);
static ae_bool ipm2solver_reducedsystemsolve(ipm2reducedsystem* redsys,
     ipm2state* sstate,
     const ipm2vars* current,
     const ipm2righthandside* rhs,
     ipm2vars* delta,
     ae_state *_state);
static double ipm2solver_minnz(/* Real    */ const ae_vector* x,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state);
static double ipm2solver_minprodnz(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state);
static double ipm2solver_maxprodnz(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state);
static double ipm2solver_maxabsrange(/* Real    */ const ae_vector* x,
     ae_int_t r0,
     ae_int_t r1,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

INPUT PARAMETERS:
    State       -   solver  state  to  be configured; previously allocated
                    memory is reused as much as possible
    S           -   scale vector, array[N]:
                    * I-th element contains scale of I-th variable,
                    * S[I]>0
    XOrigin     -   origin term, array[N]. Can be zero. The solver solves
                    problem of the form
                    >
                    > min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))
                    >
                    The terms A and b (as well as constraints) will be
                    specified later with separate calls.
    N           -   total number of variables, N>=1

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
void ipm2init(ipm2state* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nuser,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     ae_bool isupper,
     /* Real    */ const ae_matrix* ccorr,
     /* Real    */ const ae_vector* dcorr,
     ae_int_t kcorr,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    double v;
    double vv;
    ae_int_t nnz;
    ae_int_t offs;


    ae_assert(nuser>=1, "IPM2Init: N<1", _state);
    ae_assert(isfinitevector(s, nuser, _state), "IPM2Init: S contains infinite or NaN elements", _state);
    ae_assert(isfinitevector(xorigin, nuser, _state), "IPM2Init: XOrigin contains infinite or NaN elements", _state);
    ae_assert(hkind==0||hkind==1, "IPM2Init: incorrect HKind", _state);
    ae_assert(isfinitevector(c, nuser, _state), "IPM2Init: C contains infinite or NaN elements", _state);
    ae_assert(mdense>=0, "IPM2Init: MDense<0", _state);
    ae_assert(msparse>=0, "IPM2Init: MSparse<0", _state);
    ae_assert(apservisfinitematrix(densea, mdense, nuser, _state), "IPM2Init: DenseA contains infinite or NaN values!", _state);
    ae_assert(msparse==0||sparsea->matrixtype==1, "IPM2Init: non-CRS constraint matrix!", _state);
    ae_assert(msparse==0||(sparsea->m==msparse&&sparsea->n==nuser), "IPM2Init: constraint matrix has incorrect size", _state);
    ae_assert(cl->cnt>=mdense+msparse, "IPM2Init: CL is too short!", _state);
    ae_assert(cu->cnt>=mdense+msparse, "IPM2Init: CU is too short!", _state);
    ae_assert(kcorr>=0, "IPM2Init: KCorr<0", _state);
    ae_assert(kcorr==0||ccorr->rows>=kcorr, "IPM2Init: CCorr size is not KCorr*N, rows count is too small", _state);
    ae_assert(kcorr==0||ccorr->cols>=nuser, "IPM2Init: CCorr size is not KCorr*N, cols count is too small", _state);
    ae_assert(kcorr==0||dcorr->cnt>=kcorr, "IPM2Init: DCorr size is not [KCorr]", _state);
    for(i=0; i<=kcorr-1; i++)
    {
        ae_assert(ae_fp_eq(ae_fabs(dcorr->ptr.p_double[i], _state),(double)(1)), "IPM2Init: |DCorr[I]|<>1", _state);
    }
    
    /*
     * Problem metrics, settings and type
     */
    ntotal = nuser+kcorr+msparse+mdense;
    state->nuser = nuser;
    state->naug = nuser+kcorr;
    state->ntotal = ntotal;
    state->islinear = ae_false;
    ipm2setcond(state, 0.0, 0.0, 0.0, _state);
    
    /*
     * Reports
     */
    state->repiterationscount = 0;
    state->repncholesky = 0;
    
    /*
     * Trace
     */
    state->dotrace = ae_false;
    state->dodetailedtrace = ae_false;
    
    /*
     * Scale and origin
     */
    rsetallocv(ntotal, 1.0, &state->sclx, _state);
    rsetallocv(ntotal, 1.0, &state->invsclx, _state);
    rsetallocv(ntotal, 0.0, &state->xoriginx, _state);
    for(i=0; i<=nuser-1; i++)
    {
        ae_assert(s->ptr.p_double[i]>0.0, "IPM2Init: S[i] is non-positive", _state);
        state->sclx.ptr.p_double[i] = s->ptr.p_double[i];
        state->invsclx.ptr.p_double[i] = (double)1/s->ptr.p_double[i];
        state->xoriginx.ptr.p_double[i] = xorigin->ptr.p_double[i];
    }
    
    /*
     * Extended linear term
     */
    rsetallocv(ntotal, 0.0, &state->ce, _state);
    rcopyv(nuser, c, &state->ce, _state);
    
    /*
     * Generate sparse lower triangular representation of the quadratic term H and dense
     * corrector CCorr/DCorr. Apply scaling and normalization.
     */
    state->tmplowerh.matrixtype = 1;
    state->tmplowerh.m = nuser;
    state->tmplowerh.n = nuser;
    iallocv(nuser+1, &state->tmplowerh.ridx, _state);
    if( hkind==0 )
    {
        
        /*
         * Sparsify dense term, generate leading N*N part of HX
         */
        nnz = 0;
        for(i=0; i<=nuser-1; i++)
        {
            nnz = nnz+1;
            if( isupper )
            {
                j0 = i+1;
                j1 = nuser-1;
            }
            else
            {
                j0 = 0;
                j1 = i-1;
            }
            for(j=j0; j<=j1; j++)
            {
                if( denseh->ptr.pp_double[i][j]!=(double)0 )
                {
                    nnz = nnz+1;
                }
            }
        }
        iallocv(nnz+nuser, &state->tmplowerh.idx, _state);
        rallocv(nnz+nuser, &state->tmplowerh.vals, _state);
        state->tmplowerh.ridx.ptr.p_int[0] = 0;
        offs = 0;
        vv = (double)(0);
        for(i=0; i<=state->nuser-1; i++)
        {
            
            /*
             * Off-diagonal elements are copied only when nonzero
             */
            if( !isupper )
            {
                for(j=0; j<=i-1; j++)
                {
                    if( denseh->ptr.pp_double[i][j]!=(double)0 )
                    {
                        v = denseh->ptr.pp_double[i][j];
                        state->tmplowerh.idx.ptr.p_int[offs] = j;
                        state->tmplowerh.vals.ptr.p_double[offs] = v;
                        vv = vv+v;
                        offs = offs+1;
                    }
                }
            }
            
            /*
             * Diagonal element is always copied
             */
            v = denseh->ptr.pp_double[i][i];
            state->tmplowerh.idx.ptr.p_int[offs] = i;
            state->tmplowerh.vals.ptr.p_double[offs] = v;
            vv = vv+v;
            offs = offs+1;
            
            /*
             * Off-diagonal elements are copied only when nonzero
             */
            if( isupper )
            {
                for(j=i+1; j<=nuser-1; j++)
                {
                    if( denseh->ptr.pp_double[i][j]!=(double)0 )
                    {
                        v = denseh->ptr.pp_double[i][j];
                        state->tmplowerh.idx.ptr.p_int[offs] = j;
                        state->tmplowerh.vals.ptr.p_double[offs] = v;
                        vv = vv+v;
                        offs = offs+1;
                    }
                }
            }
            
            /*
             * Finalize row
             */
            state->tmplowerh.ridx.ptr.p_int[i+1] = offs;
        }
        ae_assert(ae_isfinite(vv, _state), "VIPMSetQuadraticLinear: DenseH contains infinite or NaN values!", _state);
        ae_assert(offs==nnz, "VIPMSetQuadraticLinear: integrity check failed", _state);
        ae_assert(offs<=state->tmplowerh.vals.cnt&&offs<=state->tmplowerh.idx.cnt, "IPM2Init: integrity check failed", _state);
        sparsecreatecrsinplace(&state->tmplowerh, _state);
    }
    if( hkind==1 )
    {
        
        /*
         * Copy sparse quadratic term, but make sure that we have diagonal elements
         * present (we add diagonal if it is not present)
         */
        ae_assert(sparseh->matrixtype==1, "VIPMSetQuadraticLinear: unexpected sparse matrix format", _state);
        ae_assert(sparseh->m==nuser, "VIPMSetQuadraticLinear: unexpected sparse matrix size", _state);
        ae_assert(sparseh->n==nuser, "VIPMSetQuadraticLinear: unexpected sparse matrix size", _state);
        iallocv(sparseh->ridx.ptr.p_int[nuser]+nuser, &state->tmplowerh.idx, _state);
        rallocv(sparseh->ridx.ptr.p_int[nuser]+nuser, &state->tmplowerh.vals, _state);
        state->tmplowerh.ridx.ptr.p_int[0] = 0;
        offs = 0;
        vv = (double)(0);
        for(i=0; i<=state->nuser-1; i++)
        {
            
            /*
             * Copy subdiagonal elements (if needed)
             */
            if( !isupper )
            {
                j0 = sparseh->ridx.ptr.p_int[i];
                j1 = sparseh->didx.ptr.p_int[i]-1;
                for(k=j0; k<=j1; k++)
                {
                    v = sparseh->vals.ptr.p_double[k];
                    state->tmplowerh.idx.ptr.p_int[offs] = sparseh->idx.ptr.p_int[k];
                    state->tmplowerh.vals.ptr.p_double[offs] = v;
                    vv = vv+v;
                    offs = offs+1;
                }
            }
            
            /*
             * Diagonal element is always copied
             */
            v = (double)(0);
            if( sparseh->uidx.ptr.p_int[i]!=sparseh->didx.ptr.p_int[i] )
            {
                v = sparseh->vals.ptr.p_double[sparseh->didx.ptr.p_int[i]];
            }
            state->tmplowerh.idx.ptr.p_int[offs] = i;
            state->tmplowerh.vals.ptr.p_double[offs] = v;
            vv = vv+v;
            offs = offs+1;
            
            /*
             * Copy superdiagonal elements (if needed)
             */
            if( isupper )
            {
                j0 = sparseh->uidx.ptr.p_int[i];
                j1 = sparseh->ridx.ptr.p_int[i+1]-1;
                for(k=j0; k<=j1; k++)
                {
                    v = sparseh->vals.ptr.p_double[k];
                    state->tmplowerh.idx.ptr.p_int[offs] = sparseh->idx.ptr.p_int[k];
                    state->tmplowerh.vals.ptr.p_double[offs] = v;
                    vv = vv+v;
                    offs = offs+1;
                }
            }
            
            /*
             * Finalize row
             */
            state->tmplowerh.ridx.ptr.p_int[i+1] = offs;
        }
        ae_assert(ae_isfinite(vv, _state), "IPM2Init: SparseH contains infinite or NaN values!", _state);
        ae_assert(offs<=state->tmplowerh.vals.cnt&&offs<=state->tmplowerh.idx.cnt, "IPM2Init: integrity check failed", _state);
        sparsecreatecrsinplace(&state->tmplowerh, _state);
    }
    if( isupper )
    {
        sparsecopytransposecrsbuf(&state->tmplowerh, &state->tmpsparse0, _state);
        sparsecopybuf(&state->tmpsparse0, &state->tmplowerh, _state);
    }
    if( kcorr>0 )
    {
        for(i=0; i<=kcorr-1; i++)
        {
            ae_assert(ae_fp_eq(ae_fabs(dcorr->ptr.p_double[i], _state),1.0), "IPM2: DCorr[i] is neither 1 nor -1", _state);
        }
        rcopyallocm(kcorr, state->nuser, ccorr, &state->tmpccorr, _state);
        rcopyallocv(kcorr, dcorr, &state->tmpdcorr, _state);
    }
    scalesparseqpinplace(&state->sclx, state->nuser, &state->tmplowerh, &state->tmpccorr, &state->tmpdcorr, kcorr, &state->ce, _state);
    state->targetscale = normalizesparseqpinplace(&state->tmplowerh, ae_false, &state->tmpccorr, &state->tmpdcorr, kcorr, &state->ce, state->nuser, _state);
    
    /*
     * Extended quadratic term: copy, extend
     *
     * NOTE: we perform integrity check for inifinities/NANs by
     *       computing sum of all matrix elements and checking its
     *       value for being finite. It is a bit faster than checking
     *       each element individually.
     */
    state->sparsehe.matrixtype = 1;
    state->sparsehe.m = ntotal;
    state->sparsehe.n = ntotal;
    iallocv(ntotal+1, &state->sparsehe.ridx, _state);
    ae_assert(state->tmplowerh.matrixtype==1, "VIPMSetQuadraticLinear: unexpected sparse matrix format", _state);
    ae_assert(state->tmplowerh.m==nuser, "VIPMSetQuadraticLinear: unexpected sparse matrix size", _state);
    ae_assert(state->tmplowerh.n==nuser, "VIPMSetQuadraticLinear: unexpected sparse matrix size", _state);
    iallocv(state->tmplowerh.ridx.ptr.p_int[nuser]+kcorr*(nuser+1)+(state->ntotal-nuser), &state->sparsehe.idx, _state);
    rallocv(state->tmplowerh.ridx.ptr.p_int[nuser]+kcorr*(nuser+1)+(state->ntotal-nuser), &state->sparsehe.vals, _state);
    state->sparsehe.ridx.ptr.p_int[0] = 0;
    offs = 0;
    vv = (double)(0);
    for(i=0; i<=state->nuser-1; i++)
    {
        
        /*
         * Copy subdiagonal elements (if needed)
         */
        j0 = state->tmplowerh.ridx.ptr.p_int[i];
        j1 = state->tmplowerh.didx.ptr.p_int[i]-1;
        for(k=j0; k<=j1; k++)
        {
            v = state->tmplowerh.vals.ptr.p_double[k];
            state->sparsehe.idx.ptr.p_int[offs] = state->tmplowerh.idx.ptr.p_int[k];
            state->sparsehe.vals.ptr.p_double[offs] = v;
            vv = vv+v;
            offs = offs+1;
        }
        
        /*
         * The diagonal element is always copied
         */
        v = (double)(0);
        if( state->tmplowerh.uidx.ptr.p_int[i]!=state->tmplowerh.didx.ptr.p_int[i] )
        {
            v = state->tmplowerh.vals.ptr.p_double[state->tmplowerh.didx.ptr.p_int[i]];
        }
        state->sparsehe.idx.ptr.p_int[offs] = i;
        state->sparsehe.vals.ptr.p_double[offs] = v;
        vv = vv+v;
        offs = offs+1;
        
        /*
         * Finalize row
         */
        state->sparsehe.ridx.ptr.p_int[i+1] = offs;
    }
    ae_assert(ae_isfinite(vv, _state), "IPM2Init: SparseH contains infinite or NaN values!", _state);
    for(i=state->nuser; i<=state->naug-1; i++)
    {
        
        /*
         * Augmentation row
         */
        offs = state->sparsehe.ridx.ptr.p_int[i];
        for(j=0; j<=nuser-1; j++)
        {
            if( state->tmpccorr.ptr.pp_double[i-state->nuser][j]!=0.0 )
            {
                state->sparsehe.idx.ptr.p_int[offs] = j;
                state->sparsehe.vals.ptr.p_double[offs] = state->tmpccorr.ptr.pp_double[i-state->nuser][j];
                offs = offs+1;
            }
        }
        
        /*
         * Diagonal element
         */
        state->sparsehe.idx.ptr.p_int[offs] = i;
        state->sparsehe.vals.ptr.p_double[offs] = -state->tmpdcorr.ptr.p_double[i-state->nuser];
        offs = offs+1;
        
        /*
         * Finalize row
         */
        state->sparsehe.ridx.ptr.p_int[i+1] = offs;
    }
    for(i=state->naug; i<=state->ntotal-1; i++)
    {
        
        /*
         * Extend with zeros
         */
        state->sparsehe.idx.ptr.p_int[offs] = i;
        state->sparsehe.vals.ptr.p_double[offs] = 0.0;
        offs = offs+1;
        state->sparsehe.ridx.ptr.p_int[i+1] = offs;
    }
    ae_assert(offs<=state->sparsehe.vals.cnt&&offs<=state->sparsehe.idx.cnt, "IPM2Init: integrity check failed", _state);
    sparsecreatecrsinplace(&state->sparsehe, _state);
    state->isdiagonalh = state->sparsehe.ridx.ptr.p_int[state->ntotal]==state->ntotal;
    
    /*
     * Primary part of the box constraints
     */
    rcopyallocv(state->nuser, bndl, &state->rawbndl, _state);
    rcopyallocv(state->nuser, bndu, &state->rawbndu, _state);
    rsetallocv(state->ntotal, _state->v_neginf, &state->bndle, _state);
    rsetallocv(state->ntotal, _state->v_posinf, &state->bndue, _state);
    bsetallocv(state->ntotal, ae_false, &state->hasbndle, _state);
    bsetallocv(state->ntotal, ae_false, &state->hasbndue, _state);
    for(i=0; i<=state->nuser-1; i++)
    {
        state->hasbndle.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->hasbndue.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
        ae_assert(!((state->hasbndle.ptr.p_bool[i]&&state->hasbndue.ptr.p_bool[i])&&ae_fp_greater(bndl->ptr.p_double[i],bndu->ptr.p_double[i])), "IPM2Init: inconsistent range for box constraints", _state);
        state->bndle.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->bndue.ptr.p_double[i] = bndu->ptr.p_double[i];
    }
    scaleshiftbcinplace(&state->sclx, &state->xoriginx, &state->bndle, &state->bndue, state->ntotal, _state);
    
    /*
     * Linear constraints (sparsified):
     * * copy to TmpA, normalize here
     * * extend with slack variables, save to RawAE
     * * append constraint bounds to the extended box constraints
     */
    m = mdense+msparse;
    state->mraw = m;
    for(i=0; i<=m-1; i++)
    {
        ae_assert(ae_isfinite(cl->ptr.p_double[i], _state)||ae_isneginf(cl->ptr.p_double[i], _state), "IPM2: CL is not finite number or -INF", _state);
        ae_assert(ae_isfinite(cu->ptr.p_double[i], _state)||ae_isposinf(cu->ptr.p_double[i], _state), "IPM2: CU is not finite number or +INF", _state);
    }
    state->mraw = m;
    state->tmpa.m = m;
    state->tmpa.n = state->ntotal;
    iallocv(m+1, &state->tmpa.ridx, _state);
    state->tmpa.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=msparse-1; i++)
    {
        offs = state->tmpa.ridx.ptr.p_int[i];
        igrowv(offs+state->ntotal, &state->tmpa.idx, _state);
        rgrowv(offs+state->ntotal, &state->tmpa.vals, _state);
        j0 = sparsea->ridx.ptr.p_int[i];
        j1 = sparsea->ridx.ptr.p_int[i+1]-1;
        for(j=j0; j<=j1; j++)
        {
            state->tmpa.idx.ptr.p_int[offs] = sparsea->idx.ptr.p_int[j];
            state->tmpa.vals.ptr.p_double[offs] = sparsea->vals.ptr.p_double[j];
            offs = offs+1;
        }
        state->tmpa.ridx.ptr.p_int[i+1] = offs;
    }
    for(i=0; i<=mdense-1; i++)
    {
        offs = state->tmpa.ridx.ptr.p_int[msparse+i];
        igrowv(offs+state->ntotal, &state->tmpa.idx, _state);
        rgrowv(offs+state->ntotal, &state->tmpa.vals, _state);
        for(k=0; k<=state->nuser-1; k++)
        {
            if( densea->ptr.pp_double[i][k]!=(double)0 )
            {
                state->tmpa.idx.ptr.p_int[offs] = k;
                state->tmpa.vals.ptr.p_double[offs] = densea->ptr.pp_double[i][k];
                offs = offs+1;
            }
        }
        state->tmpa.ridx.ptr.p_int[msparse+i+1] = offs;
    }
    sparsecreatecrsinplace(&state->tmpa, _state);
    rcopyallocv(m, cl, &state->tmpal, _state);
    rcopyallocv(m, cu, &state->tmpau, _state);
    scaleshiftsparselcinplace(&state->sclx, &state->xoriginx, state->ntotal, &state->tmpa, m, &state->tmpal, &state->tmpau, _state);
    normalizesparselcinplace(&state->tmpa, m, &state->tmpal, &state->tmpau, state->ntotal, ae_true, &state->ascales, ae_true, _state);
    state->rawae.m = m;
    state->rawae.n = state->ntotal;
    iallocv(m+1, &state->rawae.ridx, _state);
    state->rawae.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=m-1; i++)
    {
        
        /*
         * Copy I-th row of TmpA to RawAE, extend with slacks
         */
        offs = state->rawae.ridx.ptr.p_int[i];
        igrowv(offs+state->ntotal, &state->rawae.idx, _state);
        rgrowv(offs+state->ntotal, &state->rawae.vals, _state);
        j0 = state->tmpa.ridx.ptr.p_int[i];
        j1 = state->tmpa.ridx.ptr.p_int[i+1]-1;
        for(j=j0; j<=j1; j++)
        {
            state->rawae.idx.ptr.p_int[offs] = state->tmpa.idx.ptr.p_int[j];
            state->rawae.vals.ptr.p_double[offs] = state->tmpa.vals.ptr.p_double[j];
            offs = offs+1;
        }
        state->rawae.idx.ptr.p_int[offs] = state->naug+i;
        state->rawae.vals.ptr.p_double[offs] = -1.0;
        offs = offs+1;
        state->rawae.ridx.ptr.p_int[i+1] = offs;
        
        /*
         * Extend box constraints
         */
        state->bndle.ptr.p_double[state->naug+i] = state->tmpal.ptr.p_double[i];
        state->bndue.ptr.p_double[state->naug+i] = state->tmpau.ptr.p_double[i];
        state->hasbndle.ptr.p_bool[state->naug+i] = ae_isfinite(state->tmpal.ptr.p_double[i], _state);
        state->hasbndue.ptr.p_bool[state->naug+i] = ae_isfinite(state->tmpau.ptr.p_double[i], _state);
    }
    sparsecreatecrsinplace(&state->rawae, _state);
    
    /*
     * Finite version of box constraints - infinities replaced by very big numbers.
     * Allows us to avoid conditional checks during KKT system generation.
     */
    rallocv(state->ntotal, &state->bndlef, _state);
    rallocv(state->ntotal, &state->bnduef, _state);
    for(i=0; i<=state->ntotal-1; i++)
    {
        state->bndlef.ptr.p_double[i] = rcase2(ae_isfinite(state->bndle.ptr.p_double[i], _state), state->bndle.ptr.p_double[i], -0.001*ae_maxrealnumber, _state);
        state->bnduef.ptr.p_double[i] = rcase2(ae_isfinite(state->bndue.ptr.p_double[i], _state), state->bndue.ptr.p_double[i], 0.001*ae_maxrealnumber, _state);
    }
}


/*************************************************************************
Sets stopping criteria for QP-IPM solver.

You can set all epsilon-values to one small value, about 1.0E-6.

INPUT PARAMETERS:
    State               -   instance initialized with one of the initialization
                            functions
    EpsP                -   maximum primal error allowed in the  solution,
                            EpsP>=0. Zero will be  automatically  replaced
                            by recommended default value,  which is  equal
                            to 10*Sqrt(Epsilon) in the current version
    EpsD                -   maximum  dual  error allowed in the  solution,
                            EpsP>=0. Zero will be  automatically  replaced
                            by recommended default value,  which is  equal
                            to 10*Sqrt(Epsilon) in the current version
    EpsGap              -   maximum  duality gap allowed in the  solution,
                            EpsP>=0. Zero will be  automatically  replaced
                            by recommended default value,  which is  equal
                            to 10*Sqrt(Epsilon) in the current version

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
void ipm2setcond(ipm2state* state,
     double epsp,
     double epsd,
     double epsgap,
     ae_state *_state)
{
    double sml;


    ae_assert(ae_isfinite(epsp, _state)&&ae_fp_greater_eq(epsp,(double)(0)), "IPM2SetCond: EpsP is infinite or negative", _state);
    ae_assert(ae_isfinite(epsd, _state)&&ae_fp_greater_eq(epsd,(double)(0)), "IPM2SetCond: EpsD is infinite or negative", _state);
    ae_assert(ae_isfinite(epsgap, _state)&&ae_fp_greater_eq(epsgap,(double)(0)), "IPM2SetCond: EpsP is infinite or negative", _state);
    sml = ae_sqrt(ae_machineepsilon, _state);
    state->epsp = coalesce(epsp, sml, _state);
    state->epsd = coalesce(epsd, sml, _state);
    state->epsgap = coalesce(epsgap, sml, _state);
}


/*************************************************************************
Solve QP problem.

INPUT PARAMETERS:
    State               -   solver instance
    DropBigBounds       -   If True, algorithm may drop box and linear constraints
                            with huge bound values that destabilize algorithm.
    
OUTPUT PARAMETERS:
    XS                  -   array[N], solution
    LagBC               -   array[N], Lagrange multipliers for box constraints
    LagLC               -   array[M], Lagrange multipliers for linear constraints
    TerminationType     -   completion code, positive values for success,
                            negative for failures (XS constrains best point
                            found so far):
                            * -2    the task is either unbounded or infeasible;
                                    the IPM solver has difficulty distinguishing between these two.
                            * +1    stopping criteria are met
                            * +7    stopping criteria are too stringent

RESULT:

This function ALWAYS returns something  meaningful in XS, LagBC, LagLC - 
either solution or the best point so far, even for negative TerminationType.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void ipm2optimize(ipm2state* state,
     ae_bool dropbigbounds,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state)
{
    ae_int_t nuser;
    ae_int_t ntotal;
    ae_int_t m;
    ae_int_t i;
    double regfree;
    double dampeps;
    double maxdampeps;
    double dampepsmu;
    double dampepszs;
    ae_int_t iteridx;
    double mu0;
    double mu1;
    double alpha;
    double alphaprev;
    double alphac;
    ae_int_t primalstagnationcnt;
    ae_int_t dualstagnationcnt;
    double errp2;
    double errd2;
    double errpinf;
    double errdinf;
    double preverrp2;
    double preverrd2;
    double errgap;
    double eprimal;
    double edual;
    double egap;
    double y0nrm;
    double bady;
    ae_int_t bestiteridx;
    double besterr;
    double bestegap;
    double besteprimal;
    double bestedual;
    ae_bool loadbest;
    double mustop;

    *terminationtype = 0;

    nuser = state->nuser;
    ntotal = state->ntotal;
    m = state->mraw;
    state->dotrace = ae_is_trace_enabled("IPM");
    state->dodetailedtrace = state->dotrace&&ae_is_trace_enabled("IPM.DETAILED");
    
    /*
     * Prepare outputs
     */
    rsetallocv(nuser, 0.0, xs, _state);
    rsetallocv(nuser, 0.0, lagbc, _state);
    rsetallocv(m, 0.0, laglc, _state);
    
    /*
     * Some integrity checks:
     * * we need PrimalStagnationLen<DualStagnationLen in order to be able to correctly
     *   detect infeasible instances (stagnated dual error is present in both infeasible
     *   and unbounded instances, so we should check for primal stagnation a few iters
     *   before checking for dual stagnation)
     */
    ae_assert(ipm2solver_primalstagnationlen<ipm2solver_dualstagnationlen, "IPM2: critical integrity failure - incorrect configuration parameters", _state);
    
    /*
     * Trace output (if needed)
     */
    if( state->dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("// IPM SOLVER STARTED                                                                             //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
    }
    
    /*
     * Prepare regularization coefficients:
     * * DampEps - damping coefficient for damped Newton step. Comes along with SafeDampEps 
     *   (threshold value when some safeguards are turned off in order to preserve convergence
     *   speed) and MaxDampEps - threshold value when we consider problem overregularized and stop.
     */
    regfree = ae_pow(ae_machineepsilon, 0.75, _state);
    dampepsmu = ae_machineepsilon;
    dampepszs = ae_machineepsilon;
    dampeps = (double)10*ae_machineepsilon;
    maxdampeps = ae_sqrt(ae_sqrt(ae_machineepsilon, _state), _state);
    
    /*
     * Set up initial state
     */
    state->repiterationscount = 0;
    state->repncholesky = 0;
    mustop = (double)100*ae_machineepsilon;
    ipm2solver_ipm2powerup(state, regfree, _state);
    ipm2solver_varsinitfrom(&state->best, &state->current, _state);
    ipm2solver_varsinitbyzero(&state->delta, ntotal, m, _state);
    ipm2solver_varsinitbyzero(&state->corr, ntotal, m, _state);
    bestiteridx = -1;
    besterr = ae_maxrealnumber;
    bestegap = ae_maxrealnumber;
    besteprimal = ae_maxrealnumber;
    bestedual = ae_maxrealnumber;
    ipm2solver_traceprogress(state, 0.0, 0.0, 0.0, _state);
    y0nrm = (double)(0);
    y0nrm = ae_maxreal(y0nrm, rmaxabsv(m, &state->current.y, _state), _state);
    y0nrm = ae_maxreal(y0nrm, rmaxabsv(ntotal, &state->current.z, _state), _state);
    y0nrm = ae_maxreal(y0nrm, rmaxabsv(ntotal, &state->current.s, _state), _state);
    
    /*
     * Start iteration
     */
    loadbest = ae_true;
    primalstagnationcnt = 0;
    dualstagnationcnt = 0;
    *terminationtype = 7;
    errp2 = ae_maxrealnumber;
    errd2 = ae_maxrealnumber;
    alphaprev = 0.0;
    ipm2solver_ipm2multiply(state, &state->current.x, &state->current.y, &state->currenthx, &state->currentax, &state->currentaty, _state);
    mu0 = ipm2solver_varscomputemu(state, &state->current, _state);
    ipm2solver_rhscomputeprimdual(state, &state->current, &state->currenthx, &state->currentax, &state->currentaty, &state->rhstarget, _state);
    for(iteridx=0; iteridx<=ipm2solver_maxipmits-1; iteridx++)
    {
        
        /*
         * Trace beginning
         */
        if( state->dotrace )
        {
            ae_trace("=== PREDICTOR-CORRECTOR STEP %2d ====================================================================\n",
                (int)(iteridx));
        }
        
        /*
         * Check regularization status, terminate if overregularized
         */
        if( ae_fp_greater_eq(dampeps,maxdampeps) )
        {
            if( state->dotrace )
            {
                ae_trace("> tried to increase regularization parameter, but it is too large\n");
                ae_trace("> it is likely that stopping conditions are too stringent, stopping at the best point found so far\n");
            }
            *terminationtype = 7;
            break;
        }
        
        /*
         * Precompute factorization
         */
        if( !ipm2solver_reducedsystemcomputefactorization(&state->reducedsystem, state, &state->current, dampepsmu, dampepszs, dampeps, dampeps, 0.0, _state) )
        {
            
            /*
             * KKT factorization failed.
             * Increase regularization parameter and skip this iteration.
             */
            dampeps = (double)10*dampeps;
            if( state->dotrace )
            {
                ae_trace("> LDLT factorization failed due to rounding errors\n");
                ae_trace("> increasing DampEps coefficient to %0.2e, skipping iteration\n",
                    (double)(dampeps));
            }
            continue;
        }
        
        /*
         * Mehrotra predictor-corrector.
         * Prepare initial predictor step
         */
        ipm2solver_rhsrecomputegammapredictor(state, &state->current, 0.0, &state->rhstarget, _state);
        if( !ipm2solver_reducedsystemsolve(&state->reducedsystem, state, &state->current, &state->rhstarget, &state->delta, _state) )
        {
            
            /*
             * Initial affine scaling step failed due to numerical errors.
             * Increase regularization parameter and skip this iteration.
             */
            dampeps = (double)10*dampeps;
            if( state->dotrace )
            {
                ae_trace("> initial affine scaling step failed to decrease residual due to rounding errors\n");
                ae_trace("> increasing DampEps coefficient to %0.2e, skipping iteration\n",
                    (double)(dampeps));
            }
            continue;
        }
        ipm2solver_ipm2computesteplength(state, &state->current, &state->delta, &alpha, _state);
        mu1 = ipm2solver_varscomputemutrial(state, &state->current, &state->delta, alpha, _state);
        
        /*
         * Perform corrector step
         */
        ipm2solver_rhsrecomputegammamehrotra(state, &state->current, ae_pow(mu1/(mu0+ae_machineepsilon), (double)(3), _state)*mu0, &state->delta, &state->rhstarget, _state);
        if( !ipm2solver_reducedsystemsolve(&state->reducedsystem, state, &state->current, &state->rhstarget, &state->corr, _state) )
        {
            dampeps = (double)10*dampeps;
            if( state->dotrace )
            {
                ae_trace("> corrector failed to decrease residual due to rounding errors\n");
                ae_trace("> increasing DampEps coefficient to %0.2e, skipping iteration\n",
                    (double)(dampeps));
            }
            continue;
        }
        
        /*
         * Finally, computing a step length and making a step
         */
        ipm2solver_ipm2computesteplength(state, &state->current, &state->corr, &alphac, _state);
        if( ae_fp_greater_eq((double)(iteridx),ipm2solver_minitersbeforesafeguards)&&(ae_fp_less_eq(alphac,ipm2solver_badsteplength)&&ae_fp_greater(alphaprev,ipm2solver_badsteplength)) )
        {
            
            /*
             * Previous step was good, but this step is too short, likely to fail due to numerical errors.
             * Increase regularization parameter and skip this iteration.
             */
            dampeps = (double)10*dampeps;
            if( state->dotrace )
            {
                ae_trace("> step length is too short (after having a good step before), suspecting rounding errors\n");
                ae_trace("> increasing DampEps coefficient to %0.2e, skipping iteration\n",
                    (double)(dampeps));
            }
            continue;
        }
        ipm2solver_varsaddstep(&state->current, &state->corr, alphac, _state);
        inc(&state->repiterationscount, _state);
        alphaprev = alphac;
        ipm2solver_traceprogress(state, mu0, alpha, alphac, _state);
        ipm2solver_ipm2multiply(state, &state->current.x, &state->current.y, &state->currenthx, &state->currentax, &state->currentaty, _state);
        ipm2solver_rhscomputeprimdual(state, &state->current, &state->currenthx, &state->currentax, &state->currentaty, &state->rhstarget, _state);
        mu0 = ipm2solver_varscomputemu(state, &state->current, _state);
        
        /*
         * Check stopping criteria
         * * primal and dual stagnation are checked only when following criteria are met:
         *   1) Mu is smaller than 1 (we already converged close enough)
         *   2) we performed more than MinItersBeforeStagnation iterations
         */
        preverrp2 = errp2;
        preverrd2 = errd2;
        ipm2solver_computeerrors(state, &state->rhstarget, &state->currenthx, &state->currentax, &state->currentaty, &errp2, &errd2, &errpinf, &errdinf, &errgap, _state);
        egap = errgap;
        eprimal = errpinf;
        edual = errdinf;
        if( ae_fp_less(rmax3(egap, eprimal, edual, _state),besterr) )
        {
            
            /*
             * Save best point found so far
             */
            ipm2solver_varsinitfrom(&state->best, &state->current, _state);
            bestiteridx = iteridx;
            besterr = rmax3(egap, eprimal, edual, _state);
            bestegap = egap;
            besteprimal = eprimal;
            bestedual = edual;
        }
        if( bestiteridx>0&&iteridx>bestiteridx+ipm2solver_minitersbeforeeworststagnation )
        {
            if( state->dotrace )
            {
                ae_trace("> worst of primal/dual/gap errors stagnated for %0d its, stopping at the best point found so far\n",
                    (int)(ipm2solver_minitersbeforeeworststagnation));
            }
            break;
        }
        if( ((ae_fp_less_eq(egap,state->epsgap)&&ae_fp_greater_eq(errp2,ipm2solver_stagnationdelta*preverrp2))&&ae_fp_greater_eq(errpinf,ipm2solver_primalinfeasible1))&&iteridx>=ipm2solver_minitersbeforestagnation )
        {
            inc(&primalstagnationcnt, _state);
            if( primalstagnationcnt>=ipm2solver_primalstagnationlen )
            {
                if( state->dotrace )
                {
                    ae_trace("> primal error stagnated for %0d its, stopping at the best point found so far\n",
                        (int)(ipm2solver_primalstagnationlen));
                }
                break;
            }
        }
        else
        {
            primalstagnationcnt = 0;
        }
        if( ((ae_fp_less_eq(egap,state->epsgap)&&ae_fp_greater_eq(errd2,ipm2solver_stagnationdelta*preverrd2))&&ae_fp_greater_eq(errdinf,ipm2solver_dualinfeasible1))&&iteridx>=ipm2solver_minitersbeforestagnation )
        {
            inc(&dualstagnationcnt, _state);
            if( dualstagnationcnt>=ipm2solver_dualstagnationlen )
            {
                if( state->dotrace )
                {
                    ae_trace("> dual error stagnated for %0d its, stopping at the best point found so far\n",
                        (int)(ipm2solver_dualstagnationlen));
                }
                break;
            }
        }
        else
        {
            dualstagnationcnt = 0;
        }
        if( ae_fp_less_eq(mu0,mustop)&&iteridx>=ipm2solver_itersfortoostringentcond )
        {
            if( state->dotrace )
            {
                ae_trace("> stopping conditions are too stringent, stopping at the best point found so far\n");
            }
            *terminationtype = 7;
            break;
        }
        if( (ae_fp_less_eq(egap,state->epsgap)&&ae_fp_less_eq(eprimal,state->epsp))&&ae_fp_less_eq(edual,state->epsd) )
        {
            if( state->dotrace )
            {
                ae_trace("> stopping criteria are met\n");
            }
            *terminationtype = 1;
            loadbest = ae_false;
            break;
        }
        bady = ipm2solver_bigy;
        bady = ae_maxreal(bady, ipm2solver_ygrowth*y0nrm, _state);
        bady = ae_maxreal(bady, ipm2solver_ygrowth*rmaxabsv(ntotal, &state->current.x, _state), _state);
        bady = ae_maxreal(bady, ipm2solver_ygrowth*rmaxabsv(ntotal, &state->current.g, _state), _state);
        bady = ae_maxreal(bady, ipm2solver_ygrowth*rmaxabsv(ntotal, &state->current.t, _state), _state);
        if( ae_fp_greater_eq(rmaxabsv(m, &state->current.y, _state),bady)&&iteridx>=ipm2solver_minitersbeforeinfeasible )
        {
            if( state->dotrace )
            {
                ae_trace("> |Y| increased beyond %0.1e, stopping at the best point found so far\n",
                    (double)(bady));
            }
            break;
        }
    }
    
    /*
     * Load best point, perform some checks
     */
    if( loadbest )
    {
        
        /*
         * Load best point
         *
         * NOTE: TouchReal() is used to avoid spurious compiler warnings about 'set but unused'
         */
        if( state->dotrace )
        {
            ae_trace("> the best point so far is one from iteration %0d\n",
                (int)(bestiteridx));
        }
        ipm2solver_varsinitfrom(&state->current, &state->best, _state);
        touchreal(&besteprimal, _state);
        touchreal(&bestedual, _state);
        touchreal(&bestegap, _state);
        
        /*
         * If no error flags were set yet, check solution quality
         */
        bady = ipm2solver_bigy;
        bady = ae_maxreal(bady, ipm2solver_ygrowth*y0nrm, _state);
        bady = ae_maxreal(bady, ipm2solver_ygrowth*rmaxabsv(ntotal, &state->current.x, _state), _state);
        bady = ae_maxreal(bady, ipm2solver_ygrowth*rmaxabsv(ntotal, &state->current.g, _state), _state);
        bady = ae_maxreal(bady, ipm2solver_ygrowth*rmaxabsv(ntotal, &state->current.t, _state), _state);
        if( *terminationtype>0&&ae_fp_greater_eq(rmaxabsv(m, &state->current.y, _state),bady) )
        {
            *terminationtype = -2;
            if( state->dotrace )
            {
                ae_trace("> |Y| increased beyond %0.1e, declaring infeasibility/unboundedness\n",
                    (double)(bady));
            }
        }
        if( *terminationtype>0&&ae_fp_greater_eq(besteprimal,ipm2solver_primalinfeasible1) )
        {
            *terminationtype = -2;
            if( state->dotrace )
            {
                ae_trace("> primal error at the best point is too high, declaring infeasibility/unboundedness\n");
            }
        }
        if( *terminationtype>0&&ae_fp_greater_eq(bestedual,ipm2solver_dualinfeasible1) )
        {
            *terminationtype = -2;
            if( state->dotrace )
            {
                ae_trace("> dual error at the best point is too high, declaring infeasibility/unboundedness\n");
            }
        }
    }
    
    /*
     * Output
     */
    for(i=0; i<=nuser-1; i++)
    {
        xs->ptr.p_double[i] = state->current.x.ptr.p_double[i];
        lagbc->ptr.p_double[i] = 0.0;
        if( state->hasgz.ptr.p_bool[i] )
        {
            lagbc->ptr.p_double[i] = lagbc->ptr.p_double[i]-state->current.z.ptr.p_double[i];
        }
        if( state->hasts.ptr.p_bool[i] )
        {
            lagbc->ptr.p_double[i] = lagbc->ptr.p_double[i]+state->current.s.ptr.p_double[i];
        }
    }
    for(i=0; i<=m-1; i++)
    {
        laglc->ptr.p_double[i] = -state->current.y.ptr.p_double[i];
    }
    
    /*
     * Unscale point and Lagrange multipliers
     */
    unscaleunshiftpointbc(&state->sclx, &state->xoriginx, &state->rawbndl, &state->rawbndu, &state->bndle, &state->bndue, &state->hasbndle, &state->hasbndue, xs, nuser, _state);
    for(i=0; i<=nuser-1; i++)
    {
        lagbc->ptr.p_double[i] = lagbc->ptr.p_double[i]*state->targetscale/state->sclx.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        laglc->ptr.p_double[i] = laglc->ptr.p_double[i]*state->targetscale/coalesce(state->ascales.ptr.p_double[i], 1.0, _state);
    }
}


/*************************************************************************
Allocates place for variables of IPM and fills by zeros.

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_varsinitbyzero(ipm2vars* vstate,
     ae_int_t ntotal,
     ae_int_t m,
     ae_state *_state)
{


    ae_assert(ntotal>=1, "VarsInitByZero: NTotal<1", _state);
    ae_assert(m>=0, "VarsInitByZero: M<0", _state);
    vstate->ntotal = ntotal;
    vstate->m = m;
    rsetallocv(ntotal, 0.0, &vstate->x, _state);
    rsetallocv(ntotal, 0.0, &vstate->g, _state);
    rsetallocv(ntotal, 0.0, &vstate->t, _state);
    rsetallocv(ntotal, 0.0, &vstate->z, _state);
    rsetallocv(ntotal, 0.0, &vstate->s, _state);
    rsetallocv(m, 0.0, &vstate->y, _state);
}


/*************************************************************************
Allocates place for variables of IPM and fills them by values of
the source

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_varsinitfrom(ipm2vars* vstate,
     const ipm2vars* vsrc,
     ae_state *_state)
{


    ae_assert(vsrc->ntotal>=1, "VarsInitFrom: NTotal<1", _state);
    ae_assert(vsrc->m>=0, "VarsInitFrom: M<0", _state);
    vstate->ntotal = vsrc->ntotal;
    vstate->m = vsrc->m;
    rcopyallocv(vsrc->ntotal, &vsrc->x, &vstate->x, _state);
    rcopyallocv(vsrc->ntotal, &vsrc->g, &vstate->g, _state);
    rcopyallocv(vsrc->ntotal, &vsrc->t, &vstate->t, _state);
    rcopyallocv(vsrc->ntotal, &vsrc->z, &vstate->z, _state);
    rcopyallocv(vsrc->ntotal, &vsrc->s, &vstate->s, _state);
    rcopyallocv(vsrc->m, &vsrc->y, &vstate->y, _state);
}


/*************************************************************************
Adds to variables direction vector times step length. Different
lengths are used for primal and dual steps.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_varsaddstep(ipm2vars* vstate,
     const ipm2vars* vdir,
     double stp,
     ae_state *_state)
{


    raddv(vstate->ntotal, stp, &vdir->x, &vstate->x, _state);
    raddv(vstate->ntotal, stp, &vdir->g, &vstate->g, _state);
    raddv(vstate->ntotal, stp, &vdir->z, &vstate->z, _state);
    raddv(vstate->ntotal, stp, &vdir->t, &vstate->t, _state);
    raddv(vstate->ntotal, stp, &vdir->s, &vstate->s, _state);
    raddv(vstate->m, stp, &vdir->y, &vstate->y, _state);
}


/*************************************************************************
Computes complementarity gap

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static double ipm2solver_varscomputecomplementaritygap(const ipm2vars* vstate,
     ae_state *_state)
{
    double result;


    result = rdotv(vstate->ntotal, &vstate->g, &vstate->z, _state)+rdotv(vstate->ntotal, &vstate->t, &vstate->s, _state);
    return result;
}


/*************************************************************************
Computes empirical value of the barrier parameter Mu

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static double ipm2solver_varscomputemu(const ipm2state* state,
     const ipm2vars* vstate,
     ae_state *_state)
{
    double result;


    result = (rdotv(state->ntotal, &vstate->z, &vstate->g, _state)+rdotv(state->ntotal, &vstate->s, &vstate->t, _state))/coalesce((double)(state->cntgz+state->cntts), (double)(1), _state);
    return result;
}


/*************************************************************************
Computes empirical value of the barrier parameter Mu at the trial point
Current+Alpha*Delta without computing the point itself.

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static double ipm2solver_varscomputemutrial(const ipm2state* state,
     const ipm2vars* current,
     const ipm2vars* delta,
     double alpha,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t nx;
    double result;


    nx = state->ntotal;
    result = (double)(0);
    for(i=0; i<=nx-1; i++)
    {
        result = result+(current->g.ptr.p_double[i]+alpha*delta->g.ptr.p_double[i])*(current->z.ptr.p_double[i]+alpha*delta->z.ptr.p_double[i]);
        result = result+(current->t.ptr.p_double[i]+alpha*delta->t.ptr.p_double[i])*(current->s.ptr.p_double[i]+alpha*delta->s.ptr.p_double[i]);
    }
    result = result/coalesce((double)(state->cntgz+state->cntts), (double)(1), _state);
    return result;
}


/*************************************************************************
Computes target function 0.5*x'*H*x+c'*x

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static double ipm2solver_ipm2target(const ipm2state* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    double v;
    double result;


    ntotal = state->ntotal;
    result = (double)(0);
    for(i=0; i<=ntotal-1; i++)
    {
        result = result+state->ce.ptr.p_double[i]*x->ptr.p_double[i];
        j0 = state->sparsehe.ridx.ptr.p_int[i];
        j1 = state->sparsehe.didx.ptr.p_int[i]-1;
        for(k=j0; k<=j1; k++)
        {
            v = state->sparsehe.vals.ptr.p_double[k];
            j = state->sparsehe.idx.ptr.p_int[k];
            result = result+v*x->ptr.p_double[i]*x->ptr.p_double[j];
        }
        ae_assert(state->sparsehe.uidx.ptr.p_int[i]!=state->sparsehe.didx.ptr.p_int[i], "VIPMTarget: sparse diagonal not found", _state);
        v = state->sparsehe.vals.ptr.p_double[state->sparsehe.didx.ptr.p_int[i]];
        result = result+0.5*v*x->ptr.p_double[i]*x->ptr.p_double[i];
    }
    return result;
}


/*************************************************************************
Computes

    Y := alpha*A*x + beta*Y

where A is constraint matrix, X is user-specified source, Y is target.

Beta can be zero (in this case original contents of Y is ignored).
If Beta is nonzero, we expect that Y contains preallocated array.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_multiplygeax(const ipm2state* state,
     double alpha,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t offsax,
     ae_state *_state)
{
    ae_int_t m;


    m = state->mraw;
    if( ae_fp_eq(beta,(double)(0)) )
    {
        rallocv(offsax+m, y, _state);
    }
    else
    {
        ae_assert(y->cnt>=offsax+m, "MultiplyGEAX: Y is too short", _state);
    }
    if( ae_fp_neq(alpha,(double)(0))&&m!=0 )
    {
        sparsegemv(&state->rawae, alpha, 0, x, offsx, beta, y, offsax, _state);
    }
}


/*************************************************************************
Computes

    Y := alpha*A'*x + beta*Y

where A is constraint matrix, X is user-specified source, Y is target.

Beta can be zero, in this case we automatically reallocate target if it is
too short (but do NOT reallocate it if its size is large enough).
If Beta is nonzero, we expect that Y contains preallocated array.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_multiplygeatx(const ipm2state* state,
     double alpha,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t m;


    ntotal = state->ntotal;
    m = state->mraw;
    if( ae_fp_eq(beta,(double)(0)) )
    {
        rallocv(offsy+ntotal, y, _state);
        rsetvx(ntotal, 0.0, y, offsy, _state);
    }
    else
    {
        ae_assert(y->cnt>=offsy+ntotal, "MultiplyGEATX: Y is too short", _state);
        rmulvx(ntotal, beta, y, offsy, _state);
    }
    if( ae_fp_neq(alpha,(double)(0))&&m!=0 )
    {
        sparsegemv(&state->rawae, alpha, 1, x, offsx, 1.0, y, offsy, _state);
    }
}


/*************************************************************************
Computes H*x, does not support advanced functionality of GEAX/GEATX

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_multiplyhx(const ipm2state* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t i;


    ntotal = state->ntotal;
    rallocv(ntotal, hx, _state);
    ae_assert(state->sparsehe.n==ntotal&&state->sparsehe.m==ntotal, "IPM2MultiplyHX: sparse H has incorrect size", _state);
    if( state->isdiagonalh )
    {
        
        /*
         * H is known to be diagonal, much faster code can be used
         */
        rcopyv(ntotal, &state->diagr, hx, _state);
        raddv(ntotal, 1.0, &state->sparsehe.vals, hx, _state);
        rmergemulv(ntotal, x, hx, _state);
    }
    else
    {
        
        /*
         * H is a general sparse matrix, use generic sparse matrix-vector multiply
         */
        sparsesmv(&state->sparsehe, ae_false, x, hx, _state);
        for(i=0; i<=ntotal-1; i++)
        {
            hx->ptr.p_double[i] = hx->ptr.p_double[i]+x->ptr.p_double[i]*state->diagr.ptr.p_double[i];
        }
    }
}


/*************************************************************************
Computes products H*x, A*x, A^T*y

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_ipm2multiply(const ipm2state* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* hx,
     /* Real    */ ae_vector* ax,
     /* Real    */ ae_vector* aty,
     ae_state *_state)
{


    ipm2solver_multiplygeax(state, 1.0, x, 0, 0.0, ax, 0, _state);
    ipm2solver_multiplygeatx(state, 1.0, y, 0, 0.0, aty, 0, _state);
    ipm2solver_multiplyhx(state, x, hx, _state);
}


/*************************************************************************
This function "powers up" factorization, i.e. prepares some important
temporaries. It should be called once prior to the first call to
VIPMInitialPoint() or VIPMFactorize().

Parameters:
    RegFree     -   regularization for free variables;
                    good value sqrt(MachineEpsilon)

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_ipm2powerup(ipm2state* state,
     double regfree,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t m;
    ae_int_t i;
    double initprimslack;
    double initdualslack;


    ntotal = state->ntotal;
    m = state->mraw;
    
    /*
     * Set up information about presence of slack variables.
     * Compute diagonal regularization matrix R.
     */
    bcopyallocv(ntotal, &state->hasbndle, &state->hasgz, _state);
    bcopyallocv(ntotal, &state->hasbndue, &state->hasts, _state);
    rsetallocv(ntotal, 0.0, &state->diagr, _state);
    ipm2solver_varsinitbyzero(&state->current, ntotal, m, _state);
    rsetallocv(ntotal, 0.0, &state->maskgz, _state);
    rsetallocv(ntotal, 0.0, &state->maskts, _state);
    state->cntgz = 0;
    state->cntts = 0;
    for(i=0; i<=ntotal-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            state->maskgz.ptr.p_double[i] = 1.0;
            state->cntgz = state->cntgz+1;
        }
        if( state->hasts.ptr.p_bool[i] )
        {
            state->maskts.ptr.p_double[i] = 1.0;
            state->cntts = state->cntts+1;
        }
        if( !state->hasgz.ptr.p_bool[i]&&!state->hasts.ptr.p_bool[i] )
        {
            state->diagr.ptr.p_double[i] = regfree;
        }
    }
    
    /*
     * Initialize sparse system
     * Set up current primal and dual slacks
     */
    ipm2solver_reducedsystempowerup(&state->reducedsystem, state, &state->current.x, &state->current.y, _state);
    initprimslack = ae_maxreal(ipm2solver_initslackval, rmaxabsv(ntotal, &state->current.x, _state), _state);
    initdualslack = ae_maxreal(ipm2solver_initslackval, rmaxabsv(m, &state->current.y, _state), _state);
    rsetv(ntotal, 0.0, &state->current.g, _state);
    rsetv(ntotal, 0.0, &state->current.z, _state);
    rsetv(ntotal, 0.0, &state->current.t, _state);
    rsetv(ntotal, 0.0, &state->current.s, _state);
    for(i=0; i<=ntotal-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            state->current.g.ptr.p_double[i] = ae_maxreal(initprimslack, state->current.x.ptr.p_double[i]-state->bndle.ptr.p_double[i], _state);
            state->current.z.ptr.p_double[i] = ae_minreal(initdualslack, initprimslack*initdualslack/state->current.g.ptr.p_double[i], _state);
        }
        if( state->hasts.ptr.p_bool[i] )
        {
            state->current.t.ptr.p_double[i] = ae_maxreal(initprimslack, state->bndue.ptr.p_double[i]-state->current.x.ptr.p_double[i], _state);
            state->current.s.ptr.p_double[i] = ae_minreal(initdualslack, initprimslack*initdualslack/state->current.t.ptr.p_double[i], _state);
        }
    }
    
    /*
     * Almost done
     */
    if( state->dotrace )
    {
        ae_trace("> initial point was generated\n");
    }
}


/*************************************************************************
This function "powers up" reduced (NX+M)*(NX+M) system, i.e. prepares data
structures to receive data necessary for the factorization and solution.

It should be called once at the beginning of the optimization.

This function also proposes initial X and Y by solving strongly damped
augmented system. It expects X and Y to be preallocated arrays long enough
to store the result.

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_reducedsystempowerup(ipm2reducedsystem* s,
     const ipm2state* state,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state)
{
    ae_int_t nuser;
    ae_int_t naug;
    ae_int_t ntotal;
    ae_int_t m;
    ae_int_t offs;
    double v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t k;
    ae_int_t colthreshold;
    ae_int_t rowthreshold;
    ae_int_t sumcoldeg;
    ae_int_t sumrowdeg;
    ae_int_t n0;
    ae_int_t n1;
    ae_int_t n2;
    ae_int_t n3;
    ae_int_t n4;
    ae_int_t groupcols;
    ae_int_t grouprows;
    ae_int_t groupequality;
    ae_int_t groupfree;
    ae_int_t densegroup;
    ae_int_t denseupdatesgroup;


    nuser = state->nuser;
    naug = state->naug;
    ntotal = state->ntotal;
    m = state->mraw;
    
    /*
     * Allocate sparse matrix
     */
    sumcoldeg = 0;
    sumrowdeg = 0;
    isetallocv(naug, 0, &s->coldegrees, _state);
    isetallocv(m, 0, &s->rowdegrees, _state);
    rallocv(ntotal+m, &s->extendedrawdiagonal, _state);
    s->compactkkt.matrixtype = 1;
    s->compactkkt.m = ntotal;
    s->compactkkt.n = ntotal;
    iallocv(s->compactkkt.m+1, &s->compactkkt.ridx, _state);
    s->compactkkt.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=naug-1; i++)
    {
        offs = s->compactkkt.ridx.ptr.p_int[i];
        igrowv(offs+naug+1, &s->compactkkt.idx, _state);
        rgrowv(offs+naug+1, &s->compactkkt.vals, _state);
        
        /*
         * Offdiagonal elements
         */
        j0 = state->sparsehe.ridx.ptr.p_int[i];
        j1 = state->sparsehe.didx.ptr.p_int[i]-1;
        for(j=j0; j<=j1; j++)
        {
            k = state->sparsehe.idx.ptr.p_int[j];
            s->compactkkt.idx.ptr.p_int[offs] = k;
            s->compactkkt.vals.ptr.p_double[offs] = -state->sparsehe.vals.ptr.p_double[j];
            s->coldegrees.ptr.p_int[i] = s->coldegrees.ptr.p_int[i]+1;
            s->coldegrees.ptr.p_int[k] = s->coldegrees.ptr.p_int[k]+1;
            sumcoldeg = sumcoldeg+2;
            offs = offs+1;
        }
        
        /*
         * Diagonal element
         */
        j1 = state->sparsehe.didx.ptr.p_int[i];
        s->extendedrawdiagonal.ptr.p_double[i] = -(state->sparsehe.vals.ptr.p_double[j1]+state->diagr.ptr.p_double[i]);
        s->compactkkt.idx.ptr.p_int[offs] = state->sparsehe.idx.ptr.p_int[j1];
        s->compactkkt.vals.ptr.p_double[offs] = s->extendedrawdiagonal.ptr.p_double[i];
        offs = offs+1;
        
        /*
         * Done
         */
        s->compactkkt.ridx.ptr.p_int[i+1] = offs;
    }
    for(i=naug; i<=ntotal-1; i++)
    {
        ae_assert(state->sparsehe.ridx.ptr.p_int[i]==state->sparsehe.didx.ptr.p_int[i], "IPM2: integrity check 0546 failed", _state);
        ae_assert(state->sparsehe.didx.ptr.p_int[i]==state->sparsehe.uidx.ptr.p_int[i]-1, "IPM2: integrity check 0547 failed", _state);
        ae_assert(ae_fp_eq(state->sparsehe.vals.ptr.p_double[state->sparsehe.didx.ptr.p_int[i]],0.0), "IPM2: integrity check 0548 failed", _state);
        s->extendedrawdiagonal.ptr.p_double[i] = -(state->sparsehe.vals.ptr.p_double[state->sparsehe.didx.ptr.p_int[i]]+state->diagr.ptr.p_double[i]);
    }
    for(i=0; i<=m-1; i++)
    {
        ae_assert(state->rawae.ridx.ptr.p_int[i+1]>state->rawae.ridx.ptr.p_int[i], "IPM2: integrity check 1251 failed", _state);
        ae_assert(state->rawae.idx.ptr.p_int[state->rawae.ridx.ptr.p_int[i+1]-1]==naug+i, "IPM2: integrity check 1252 failed", _state);
        ae_assert(ae_fp_eq(state->rawae.vals.ptr.p_double[state->rawae.ridx.ptr.p_int[i+1]-1],-1.0), "IPM2: integrity check 1253 failed", _state);
        offs = s->compactkkt.ridx.ptr.p_int[naug+i];
        igrowv(offs+naug+1, &s->compactkkt.idx, _state);
        rgrowv(offs+naug+1, &s->compactkkt.vals, _state);
        j0 = state->rawae.ridx.ptr.p_int[i];
        j1 = state->rawae.ridx.ptr.p_int[i+1]-2;
        for(j=j0; j<=j1; j++)
        {
            k = state->rawae.idx.ptr.p_int[j];
            s->compactkkt.idx.ptr.p_int[offs] = k;
            s->compactkkt.vals.ptr.p_double[offs] = state->rawae.vals.ptr.p_double[j];
            s->rowdegrees.ptr.p_int[i] = s->rowdegrees.ptr.p_int[i]+1;
            s->coldegrees.ptr.p_int[k] = s->coldegrees.ptr.p_int[k]+1;
            sumrowdeg = sumrowdeg+1;
            sumcoldeg = sumcoldeg+1;
            offs = offs+1;
        }
        s->compactkkt.idx.ptr.p_int[offs] = naug+i;
        s->compactkkt.vals.ptr.p_double[offs] = 0.0;
        offs = offs+1;
        s->extendedrawdiagonal.ptr.p_double[ntotal+i] = 0.0;
        s->compactkkt.ridx.ptr.p_int[naug+i+1] = offs;
    }
    sparsecreatecrsinplace(&s->compactkkt, _state);
    
    /*
     * Determine elimination priorities
     */
    colthreshold = ae_round(ipm2solver_muquasidense*(double)sumcoldeg/(double)naug, _state)+1;
    rowthreshold = ae_round(ipm2solver_muquasidense*(double)sumrowdeg/coalesce((double)(m), (double)(1), _state), _state)+1;
    if( ae_fp_less_eq((double)sumcoldeg/(double)naug,(double)sumrowdeg/coalesce((double)(m), (double)(1), _state)) )
    {
        groupcols = 0;
        grouprows = 1;
    }
    else
    {
        groupcols = 1;
        grouprows = 0;
    }
    groupequality = 2;
    groupfree = 3;
    densegroup = ae_maxint(groupfree, groupequality, _state);
    denseupdatesgroup = densegroup+1;
    isetallocv(ntotal, 0, &s->compactpriorities, _state);
    for(i=0; i<=nuser-1; i++)
    {
        s->compactpriorities.ptr.p_int[i] = groupcols;
        if( !state->hasbndle.ptr.p_bool[i]&&!state->hasbndue.ptr.p_bool[i] )
        {
            s->compactpriorities.ptr.p_int[i] = groupfree;
        }
        if( s->coldegrees.ptr.p_int[i]>colthreshold )
        {
            s->compactpriorities.ptr.p_int[i] = densegroup;
        }
    }
    for(i=nuser; i<=naug-1; i++)
    {
        s->compactpriorities.ptr.p_int[i] = denseupdatesgroup;
    }
    for(i=0; i<=m-1; i++)
    {
        s->compactpriorities.ptr.p_int[naug+i] = grouprows;
        if( (state->hasbndle.ptr.p_bool[naug+i]&&state->hasbndue.ptr.p_bool[naug+i])&&ae_fp_less(state->bndue.ptr.p_double[naug+i]-state->bndle.ptr.p_double[naug+i],(double)1000*ae_maxreal(state->epsp, ae_sqrt(ae_machineepsilon, _state), _state)*rmax3(ae_fabs(state->bndue.ptr.p_double[naug+i], _state), ae_fabs(state->bndle.ptr.p_double[naug+i], _state), 1.0, _state)) )
        {
            s->compactpriorities.ptr.p_int[naug+i] = groupequality;
        }
        if( s->rowdegrees.ptr.p_int[i]>rowthreshold )
        {
            s->compactpriorities.ptr.p_int[naug+i] = densegroup;
        }
    }
    if( state->dotrace )
    {
        n0 = 0;
        n1 = 0;
        n2 = 0;
        n3 = 0;
        n4 = 0;
        for(i=0; i<=ntotal-1; i++)
        {
            if( s->compactpriorities.ptr.p_int[i]==0 )
            {
                inc(&n0, _state);
            }
            if( s->compactpriorities.ptr.p_int[i]==1 )
            {
                inc(&n1, _state);
            }
            if( s->compactpriorities.ptr.p_int[i]==2 )
            {
                inc(&n2, _state);
            }
            if( s->compactpriorities.ptr.p_int[i]==3 )
            {
                inc(&n3, _state);
            }
            if( s->compactpriorities.ptr.p_int[i]==4 )
            {
                inc(&n4, _state);
            }
        }
        ae_trace("> KKT matrix formed, elimination groups determined:\n");
        if( groupcols==0 )
        {
            ae_trace(">> group 0 (variables): %7d columns\n",
                (int)(n0));
            ae_trace(">> group 1 (equations): %7d columns\n",
                (int)(n1));
        }
        else
        {
            ae_trace(">> group 0 (equations): %7d columns\n",
                (int)(n0));
            ae_trace(">> group 1 (variables): %7d columns\n",
                (int)(n1));
        }
        ae_trace(">> group 2 (equality ): %7d columns\n",
            (int)(n2));
        ae_trace(">> group 3 (free     ): %7d columns\n",
            (int)(n3));
        ae_trace(">> group 4 (dense    ): %7d columns\n",
            (int)(n4));
    }
    
    /*
     * Analyze
     */
    if( !spsymmanalyze(&s->compactkkt, &s->compactpriorities, ipm2solver_mupromote, densegroup, 1, 3, 1, &s->analysis, _state) )
    {
        ae_assert(ae_false, "ReducedSystemPowerUp: critical integrity check failed, symbolically degenerate KKT system encountered", _state);
    }
    
    /*
     * Prepare initial X and Y by solving strongly factorized system
     */
    ae_assert(x->cnt>=ntotal, "IPM2: integrity check 7400 failed", _state);
    ae_assert(y->cnt>=m, "IPM2: integrity check 7401 failed", _state);
    rallocv(ntotal+m, &s->extendedeffdiagonal, _state);
    rsetvx(ntotal, -1.0, &s->extendedeffdiagonal, 0, _state);
    rsetvx(m, 1.0, &s->extendedeffdiagonal, ntotal, _state);
    raddv(ntotal+m, 1.0, &s->extendedrawdiagonal, &s->extendedeffdiagonal, _state);
    rallocv(naug+m, &s->compacteffdiagonal, _state);
    for(i=0; i<=naug-1; i++)
    {
        s->compacteffdiagonal.ptr.p_double[i] = s->extendedeffdiagonal.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        s->compacteffdiagonal.ptr.p_double[naug+i] = -(double)1/s->extendedeffdiagonal.ptr.p_double[naug+i]+s->extendedeffdiagonal.ptr.p_double[ntotal+i];
    }
    spsymmreloaddiagonal(&s->analysis, &s->compacteffdiagonal, _state);
    if( !spsymmfactorize(&s->analysis, _state) )
    {
        ae_assert(ae_false, "ReducedSystemPowerUp: unexpected failure during decomposition of a strongly damped system", _state);
    }
    rallocv(ntotal+m, &s->extendedrhs, _state);
    rcopyv(ntotal, &state->ce, &s->extendedrhs, _state);
    for(i=0; i<=m-1; i++)
    {
        v = (double)(0);
        if( state->hasbndle.ptr.p_bool[naug+i]&&ae_fp_greater(state->bndle.ptr.p_double[naug+i],v) )
        {
            v = state->bndle.ptr.p_double[naug+i];
        }
        if( state->hasbndue.ptr.p_bool[naug+i]&&ae_fp_less(state->bndue.ptr.p_double[naug+i],v) )
        {
            v = state->bndue.ptr.p_double[naug+i];
        }
        s->extendedrhs.ptr.p_double[ntotal+i] = v;
    }
    rallocv(ntotal, &s->compactrhs, _state);
    for(i=0; i<=naug-1; i++)
    {
        s->compactrhs.ptr.p_double[i] = s->extendedrhs.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        s->compactrhs.ptr.p_double[naug+i] = s->extendedrhs.ptr.p_double[ntotal+i]+s->extendedrhs.ptr.p_double[naug+i]/s->extendedeffdiagonal.ptr.p_double[naug+i];
    }
    spsymmsolve(&s->analysis, &s->compactrhs, _state);
    for(i=0; i<=naug-1; i++)
    {
        s->extendedrhs.ptr.p_double[i] = s->compactrhs.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        s->extendedrhs.ptr.p_double[ntotal+i] = s->compactrhs.ptr.p_double[naug+i];
        s->extendedrhs.ptr.p_double[naug+i] = (s->extendedrhs.ptr.p_double[naug+i]+s->compactrhs.ptr.p_double[naug+i])/s->extendedeffdiagonal.ptr.p_double[naug+i];
    }
    rcopyvx(ntotal, &s->extendedrhs, 0, x, 0, _state);
    rcopyvx(m, &s->extendedrhs, ntotal, y, 0, _state);
}


/*************************************************************************
Generates precomputed temporary  vectors  and  KKT  factorization  at  the
beginning of the current iteration.

This function uses representation of QP problem as a box-constrained one
with linear equality constraints A*x=0.

On successful factorization returns True; on failure returns False - it is
recommended to increase regularization parameter and try one more time.

--- DESCRIPTION ----------------------------------------------------------


  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static ae_bool ipm2solver_reducedsystemcomputefactorization(ipm2reducedsystem* redsys,
     ipm2state* sstate,
     const ipm2vars* current,
     double dampepsmu,
     double dampepszs,
     double dampepsp,
     double dampepsd,
     double regxy,
     ae_state *_state)
{
    ae_int_t naug;
    ae_int_t ntotal;
    ae_int_t m;
    ae_int_t i;
    double gz;
    double st;
    double sumsq;
    double errsq;
    double badchol;
    ae_bool result;


    naug = sstate->naug;
    ntotal = sstate->ntotal;
    m = sstate->mraw;
    result = ae_true;
    badchol = 1.0E50;
    
    /*
     * Pivot out dT and dS, dG and dZ
     */
    rallocv(ntotal, &redsys->d0, _state);
    rallocv(ntotal, &redsys->d1, _state);
    rallocv(ntotal, &redsys->d2, _state);
    rallocv(ntotal, &redsys->d3, _state);
    rallocv(ntotal, &redsys->d4, _state);
    rallocv(ntotal, &redsys->d5, _state);
    rallocv(ntotal, &redsys->d6, _state);
    rallocv(ntotal, &redsys->d7, _state);
    rallocv(ntotal+m, &redsys->extendedeffdiagonal, _state);
    rsetvx(ntotal, -dampepsp-regxy, &redsys->extendedeffdiagonal, 0, _state);
    rsetvx(m, dampepsd+regxy, &redsys->extendedeffdiagonal, ntotal, _state);
    raddv(ntotal+m, 1.0, &redsys->extendedrawdiagonal, &redsys->extendedeffdiagonal, _state);
    for(i=0; i<=ntotal-1; i++)
    {
        st = current->s.ptr.p_double[i]*current->t.ptr.p_double[i];
        redsys->d0.ptr.p_double[i] = (double)1/(dampepsmu+st);
        redsys->d1.ptr.p_double[i] = redsys->d0.ptr.p_double[i]*current->t.ptr.p_double[i];
        redsys->d3.ptr.p_double[i] = sstate->maskts.ptr.p_double[i]/(dampepszs+redsys->d1.ptr.p_double[i]*current->t.ptr.p_double[i]);
        gz = current->g.ptr.p_double[i]*current->z.ptr.p_double[i];
        redsys->d4.ptr.p_double[i] = (double)1/(dampepsmu+gz);
        redsys->d5.ptr.p_double[i] = redsys->d4.ptr.p_double[i]*current->g.ptr.p_double[i];
        redsys->d7.ptr.p_double[i] = sstate->maskgz.ptr.p_double[i]/(dampepszs+redsys->d5.ptr.p_double[i]*current->g.ptr.p_double[i]);
    }
    raddv(ntotal, -1.0, &redsys->d3, &redsys->extendedeffdiagonal, _state);
    raddv(ntotal, -1.0, &redsys->d7, &redsys->extendedeffdiagonal, _state);
    rallocv(ntotal+m, &redsys->extendedregrhs, _state);
    rcopyvx(ntotal, &current->x, 0, &redsys->extendedregrhs, 0, _state);
    rcopyvx(m, &current->y, 0, &redsys->extendedregrhs, ntotal, _state);
    rmulv(ntotal+m, -regxy, &redsys->extendedregrhs, _state);
    rallocv(ntotal, &redsys->compacteffdiagonal, _state);
    for(i=0; i<=naug-1; i++)
    {
        redsys->compacteffdiagonal.ptr.p_double[i] = redsys->extendedeffdiagonal.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        redsys->compacteffdiagonal.ptr.p_double[naug+i] = -(double)1/redsys->extendedeffdiagonal.ptr.p_double[naug+i]+redsys->extendedeffdiagonal.ptr.p_double[ntotal+i];
    }
    spsymmreloaddiagonal(&redsys->analysis, &redsys->compacteffdiagonal, _state);
    spsymmsetmodificationstrategy(&redsys->analysis, 1, ae_machineepsilon, badchol, 0.0, 0.0, _state);
    if( !spsymmfactorize(&redsys->analysis, _state) )
    {
        result = ae_false;
        return result;
    }
    spsymmdiagerr(&redsys->analysis, &sumsq, &errsq, _state);
    if( sstate->dotrace )
    {
        ae_trace("LDLT-diag-err= %0.3e (diagonal reproduction error)\n",
            (double)(ae_sqrt(errsq/((double)1+sumsq), _state)));
    }
    if( ae_fp_greater(ae_sqrt(errsq/((double)1+sumsq), _state),ae_sqrt(ae_machineepsilon, _state)) )
    {
        result = ae_false;
        return result;
    }
    return result;
}


/*************************************************************************
This function estimates primal and dual step lengths (subject to step
decay parameter, which should be in [0,1] range).

Current version returns same step lengths for primal and dual steps.

INPUT PARAMETERS:
    State               -   solver state
    V0                  -   current point (we ignore one stored in State.Current)
    VS                  -   step direction
    StepDecay           -   decay parameter, the step is multiplied by this
                            coefficient. 1.0 corresponds to full step
                            length being returned. Values in (0,1] range.
    SeparateStep        -   separate step for primal and dual vars
    
OUTPUT PARAMETERS:
    AlphaP              -   primal step (after applying decay coefficient)
    AlphaD              -   dual   step (after applying decay coefficient)

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_ipm2computesteplength(const ipm2state* state,
     const ipm2vars* current,
     const ipm2vars* delta,
     double* alpha,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t i;

    *alpha = 0.0;

    ntotal = state->ntotal;
    *alpha = (double)(1);
    for(i=0; i<=ntotal-1; i++)
    {
        
        /*
         * Primal
         */
        if( delta->g.ptr.p_double[i]<0.0 )
        {
            *alpha = safeminposrv(current->g.ptr.p_double[i], -delta->g.ptr.p_double[i], *alpha, _state);
        }
        if( delta->t.ptr.p_double[i]<0.0 )
        {
            *alpha = safeminposrv(current->t.ptr.p_double[i], -delta->t.ptr.p_double[i], *alpha, _state);
        }
        
        /*
         * Dual
         */
        if( delta->z.ptr.p_double[i]<0.0 )
        {
            *alpha = safeminposrv(current->z.ptr.p_double[i], -delta->z.ptr.p_double[i], *alpha, _state);
        }
        if( delta->s.ptr.p_double[i]<0.0 )
        {
            *alpha = safeminposrv(current->s.ptr.p_double[i], -delta->s.ptr.p_double[i], *alpha, _state);
        }
    }
    *alpha = ipm2solver_steplengthdecay*(*alpha);
}


/*************************************************************************
Compute primal/dual errors and complementarity gap

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_computeerrors(const ipm2state* state,
     const ipm2righthandside* rhs,
     /* Real    */ const ae_vector* currenthx,
     /* Real    */ const ae_vector* currentax,
     /* Real    */ const ae_vector* currentaty,
     double* errp2,
     double* errd2,
     double* errpinf,
     double* errdinf,
     double* egap,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t ntotal;
    ae_int_t cntp2;
    ae_int_t cntd2;

    *errp2 = 0.0;
    *errd2 = 0.0;
    *errpinf = 0.0;
    *errdinf = 0.0;
    *egap = 0.0;

    m = state->mraw;
    ntotal = state->ntotal;
    
    /*
     * Compute primal error
     */
    cntp2 = 0;
    *errp2 = (double)(0);
    *errpinf = (double)(0);
    *errp2 = *errp2+rdotv2(m, &rhs->ea, _state);
    *errpinf = ae_maxreal(*errpinf, rmaxabsv(m, &rhs->ea, _state), _state);
    cntp2 = cntp2+m;
    *errp2 = *errp2+rdotv2(ntotal, &rhs->el, _state);
    *errpinf = ae_maxreal(*errpinf, rmaxabsv(ntotal, &rhs->el, _state), _state);
    cntp2 = cntp2+state->cntgz;
    *errp2 = *errp2+rdotv2(ntotal, &rhs->eu, _state);
    *errpinf = ae_maxreal(*errpinf, rmaxabsv(ntotal, &rhs->eu, _state), _state);
    cntp2 = cntp2+state->cntts;
    *errp2 = ae_sqrt(*errp2/coalesce((double)(cntp2), (double)(1), _state), _state);
    
    /*
     * Compute dual error
     */
    cntd2 = 0;
    *errd2 = (double)(0);
    *errdinf = (double)(0);
    *errd2 = *errd2+rdotv2(ntotal, &rhs->ed, _state);
    *errdinf = ae_maxreal(*errdinf, rmaxabsv(ntotal, &rhs->ed, _state), _state);
    cntd2 = cntd2+m;
    *errd2 = ae_sqrt(*errd2/coalesce((double)(cntd2), (double)(1), _state), _state);
    
    /*
     * Compute complementarity gap
     */
    *egap = ipm2solver_varscomputecomplementaritygap(&state->current, _state)/(1.0+ae_fabs(ipm2solver_ipm2target(state, &state->current.x, _state), _state));
}


/*************************************************************************
Evaluate progress so far, outputs trace data, if requested to do so.

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_traceprogress(ipm2state* state,
     double muinit,
     double alphaaff,
     double alphafin,
     ae_state *_state)
{
    ae_int_t naug;
    ae_int_t mraw;
    ae_int_t ntotal;
    double errp2;
    double errd2;
    double errpinf;
    double errdinf;
    double errgap;


    naug = state->naug;
    mraw = state->mraw;
    ntotal = state->ntotal;
    if( !state->dotrace )
    {
        return;
    }
    
    /*
     * Compute temporary products, RHS and errors
     */
    ipm2solver_ipm2multiply(state, &state->current.x, &state->current.y, &state->tmphx, &state->tmpax, &state->tmpaty, _state);
    ipm2solver_rhscomputeprimdual(state, &state->current, &state->tmphx, &state->tmpax, &state->tmpaty, &state->rhsprogress, _state);
    ipm2solver_computeerrors(state, &state->rhsprogress, &state->tmphx, &state->tmpax, &state->tmpaty, &errp2, &errd2, &errpinf, &errdinf, &errgap, _state);
    
    /*
     * Print high-level information
     */
    ae_trace("--- step report ------------------------------------------------------------------------------------\n");
    ae_trace("> step information\n");
    ae_trace("mu_init = %0.3e    (at the beginning)\n",
        (double)(muinit));
    if( ae_fp_neq(alphafin,(double)(0)) )
    {
        ae_trace("alphaA  = %0.3e (initial affine scaling step)\n",
            (double)(alphaaff));
        ae_trace("alphaR  = %0.3e (refined step)\n",
            (double)(alphafin));
    }
    else
    {
        ae_trace("alpha   = %0.3e\n",
            (double)(alphaaff));
    }
    ae_trace("mu_cur  = %0.3e    (after the step)\n",
        (double)(ipm2solver_varscomputemu(state, &state->current, _state)));
    ae_trace("> errors\n");
    ae_trace("errP    = %0.3e    (primal infeasibility, inf-norm)\n",
        (double)(errpinf));
    ae_trace("errD    = %0.3e    (dual infeasibility,   inf-norm)\n",
        (double)(errdinf));
    ae_trace("errGap  = %0.3e    (complementarity gap)\n",
        (double)(errgap));
    ae_trace("> current point information (inf-norm, x-suffix stands for N raw vars, a-suffix stands for M slacks)\n");
    ae_trace("|Xx|=%8.1e,  |Xa|=%8.1e,  |Gx|=%8.1e,  |Tx|=%8.1e,  |Ga|=%8.1e,  |Ta|=%8.1e\n",
        (double)(ipm2solver_maxabsrange(&state->current.x, 0, naug, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.x, naug, ntotal, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.g, 0, naug, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.t, 0, naug, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.g, naug, ntotal, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.t, naug, ntotal, _state)));
    ae_trace("                 |Y|=%8.1e,  |Zx|=%8.1e,  |Sx|=%8.1e,  |Za|=%8.1e,  |Sa|=%8.1e\n",
        (double)(ipm2solver_maxabsrange(&state->current.y, 0, mraw, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.z, 0, naug, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.s, 0, naug, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.z, naug, ntotal, _state)),
        (double)(ipm2solver_maxabsrange(&state->current.s, naug, ntotal, _state)));
    
    /*
     * Print variable stats, if required
     */
    if( state->dotrace )
    {
        ae_trace("--- variable statistics ----------------------------------------------------------------------------\n");
        ae_trace("> smallest values for nonnegative vars\n");
        ae_trace("primal:                        minGx=%8.1e  minTx=%8.1e  minGa=%8.1e  minTa=%8.1e\n",
            (double)(ipm2solver_minnz(&state->current.g, 0, naug, _state)),
            (double)(ipm2solver_minnz(&state->current.t, 0, naug, _state)),
            (double)(ipm2solver_minnz(&state->current.g, naug, ntotal, _state)),
            (double)(ipm2solver_minnz(&state->current.t, naug, ntotal, _state)));
        ae_trace("dual:                          minZx=%8.1e  minSx=%8.1e  minGa=%8.1e  minSa=%8.1e\n",
            (double)(ipm2solver_minnz(&state->current.z, 0, naug, _state)),
            (double)(ipm2solver_minnz(&state->current.s, 0, naug, _state)),
            (double)(ipm2solver_minnz(&state->current.z, naug, ntotal, _state)),
            (double)(ipm2solver_minnz(&state->current.s, naug, ntotal, _state)));
        ae_trace("> min and max complementary slackness\n");
        ae_trace("min:                             GZx=%8.1e    TSx=%8.1e    GZa=%8.1e    TSa=%8.1e\n",
            (double)(ipm2solver_minprodnz(&state->current.g, &state->current.z, 0, naug, _state)),
            (double)(ipm2solver_minprodnz(&state->current.t, &state->current.s, 0, naug, _state)),
            (double)(ipm2solver_minprodnz(&state->current.g, &state->current.z, naug, ntotal, _state)),
            (double)(ipm2solver_minprodnz(&state->current.t, &state->current.s, naug, ntotal, _state)));
        ae_trace("max:                             GZx=%8.1e    TSx=%8.1e    GZa=%8.1e    TSa=%8.1e\n",
            (double)(ipm2solver_maxprodnz(&state->current.g, &state->current.z, 0, naug, _state)),
            (double)(ipm2solver_maxprodnz(&state->current.t, &state->current.s, 0, naug, _state)),
            (double)(ipm2solver_maxprodnz(&state->current.g, &state->current.z, naug, ntotal, _state)),
            (double)(ipm2solver_maxprodnz(&state->current.t, &state->current.s, naug, ntotal, _state)));
    }
    ae_trace("\n");
}


/*************************************************************************
Computes primal-dual right-hand side for a KKT system.

GammaGZ and GammaTS are not computed by this function.

INPUT PARAMETERS:
    State           -   IPM state
    Current         -   current point (used to compute RHS)
    CurrentHX       -   precomputed H*Current.X
    CurrentAX       -   precomputed A*Current.X
    CurrentATY      -   precomputed A'*Current.Y
    
OUTPUT PARAMETERS:
    Rhs             -   RHS

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_rhscomputeprimdual(const ipm2state* state,
     const ipm2vars* current,
     /* Real    */ const ae_vector* currenthx,
     /* Real    */ const ae_vector* currentax,
     /* Real    */ const ae_vector* currentaty,
     ipm2righthandside* rhs,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t m;


    ntotal = state->ntotal;
    m = state->mraw;
    
    /*
     * ED
     */
    rcopyallocv(ntotal, &state->ce, &rhs->ed, _state);
    raddv(ntotal, -1.0, currentaty, &rhs->ed, _state);
    raddv(ntotal, 1.0, currenthx, &rhs->ed, _state);
    raddv(ntotal, -1.0, &current->z, &rhs->ed, _state);
    raddv(ntotal, 1.0, &current->s, &rhs->ed, _state);
    
    /*
     * EA
     */
    rallocv(m, &rhs->ea, _state);
    rcopymulv(m, -1.0, currentax, &rhs->ea, _state);
    
    /*
     * EL
     */
    rcopyallocv(ntotal, &state->bndlef, &rhs->el, _state);
    raddv(ntotal, -1.0, &current->x, &rhs->el, _state);
    raddv(ntotal, 1.0, &current->g, &rhs->el, _state);
    rmergemulv(ntotal, &state->maskgz, &rhs->el, _state);
    
    /*
     * EU
     */
    rcopyallocv(ntotal, &current->x, &rhs->eu, _state);
    raddv(ntotal, 1.0, &current->t, &rhs->eu, _state);
    raddv(ntotal, -1.0, &state->bnduef, &rhs->eu, _state);
    rmergemulv(ntotal, &state->maskts, &rhs->eu, _state);
}


/*************************************************************************
Recomputes GammaGZ/TS according to a predictor (affine scaling) step

INPUT PARAMETERS:
    State           -   IPM state
    Current         -   current point (used to compute RHS)
    MuTarget        -   target Mu (can be zero),
                        used to compute GammaGZ and GammaST
    
OUTPUT PARAMETERS:
    Rhs             -   RHS

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_rhsrecomputegammapredictor(const ipm2state* state,
     const ipm2vars* current,
     double mutarget,
     ipm2righthandside* rhs,
     ae_state *_state)
{
    ae_int_t ntotal;


    ntotal = state->ntotal;
    
    /*
     * GammaGZ
     */
    rallocv(ntotal, &rhs->gammagz, _state);
    rcopymulv(ntotal, mutarget, &state->maskgz, &rhs->gammagz, _state);
    rnegmuladdv(ntotal, &current->g, &current->z, &rhs->gammagz, _state);
    
    /*
     * GammaTS
     */
    rallocv(ntotal, &rhs->gammats, _state);
    rcopymulv(ntotal, mutarget, &state->maskts, &rhs->gammats, _state);
    rnegmuladdv(ntotal, &current->t, &current->s, &rhs->gammats, _state);
}


/*************************************************************************
Recomputes GammaGZ/GammaTS part of RHS according to the Mehrotra corrector
heuristic. The rest of the right-hand side is unchanged.

INPUT PARAMETERS:
    State           -   IPM state
    Current         -   current point (used to compute RHS)
    MuTarget        -   target Mu, used to compute GammaGZ and GammaST
    Delta           -   affine-scaling step produced by predictor stage
                        of the Mehrotra algorithm
    
OUTPUT PARAMETERS:
    Rhs             -   RHS.GammaGZ, Rhs.GammaTS are recomputed

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static void ipm2solver_rhsrecomputegammamehrotra(const ipm2state* state,
     const ipm2vars* current,
     double mutarget,
     const ipm2vars* deltaaff,
     ipm2righthandside* rhs,
     ae_state *_state)
{
    ae_int_t ntotal;


    ntotal = state->ntotal;
    
    /*
     * GammaGZ
     */
    rallocv(ntotal, &rhs->gammagz, _state);
    rcopymulv(ntotal, mutarget, &state->maskgz, &rhs->gammagz, _state);
    rnegmuladdv(ntotal, &current->g, &current->z, &rhs->gammagz, _state);
    rnegmuladdv(ntotal, &deltaaff->g, &deltaaff->z, &rhs->gammagz, _state);
    
    /*
     * GammaTS
     */
    rallocv(ntotal, &rhs->gammats, _state);
    rcopymulv(ntotal, mutarget, &state->maskts, &rhs->gammats, _state);
    rnegmuladdv(ntotal, &current->t, &current->s, &rhs->gammats, _state);
    rnegmuladdv(ntotal, &deltaaff->t, &deltaaff->s, &rhs->gammats, _state);
}


/*************************************************************************
Solves KKT system with given right-hand side

INPUT PARAMETERS:
    State           -   IPM state
    Current         -   current point (beginning of the iteration)
    RHS             -   right-hand side
    
OUTPUT PARAMETERS:
    Delta           -   solution

  -- ALGLIB --
     Copyright 21.12.2022 by Bochkanov Sergey
*************************************************************************/
static ae_bool ipm2solver_reducedsystemsolve(ipm2reducedsystem* redsys,
     ipm2state* sstate,
     const ipm2vars* current,
     const ipm2righthandside* rhs,
     ipm2vars* delta,
     ae_state *_state)
{
    ae_int_t naug;
    ae_int_t ntotal;
    ae_int_t m;
    ae_bool result;


    naug = sstate->naug;
    ntotal = sstate->ntotal;
    m = sstate->mraw;
    rallocv(ntotal+m, &redsys->extendedrhs, _state);
    rcopymuladdv(ntotal, &redsys->d1, &rhs->gammats, &rhs->eu, &redsys->d2, _state);
    rcopymuladdv(ntotal, &redsys->d5, &rhs->gammagz, &rhs->el, &redsys->d6, _state);
    rcopymuladdv(ntotal, &redsys->d3, &redsys->d2, &rhs->ed, &redsys->extendedrhs, _state);
    rnegmuladdv(ntotal, &redsys->d7, &redsys->d6, &redsys->extendedrhs, _state);
    rcopyvx(m, &rhs->ea, 0, &redsys->extendedrhs, ntotal, _state);
    raddv(ntotal+m, 1.0, &redsys->extendedregrhs, &redsys->extendedrhs, _state);
    
    /*
     * Reduce (NTotal+M)*(NTotal+M) extended system to a (N+M)*(N+M) compact one
     */
    rallocv(m, &redsys->vm0, _state);
    rsetallocv(m, 1.0, &redsys->um0, _state);
    rcopyvx(m, &redsys->extendedeffdiagonal, naug, &redsys->vm0, 0, _state);
    rmergedivv(m, &redsys->vm0, &redsys->um0, _state);
    rcopyvx(m, &redsys->extendedrhs, naug, &redsys->vm0, 0, _state);
    rallocv(m, &redsys->vm1, _state);
    rcopymuladdv(m, &redsys->um0, &redsys->vm0, &rhs->ea, &redsys->vm1, _state);
    rcopyvx(naug, &redsys->extendedrhs, 0, &redsys->compactrhs, 0, _state);
    rcopyvx(m, &redsys->vm1, 0, &redsys->compactrhs, naug, _state);
    spsymmsolve(&redsys->analysis, &redsys->compactrhs, _state);
    
    /*
     * Perform backsubstitution
     */
    raddvx(m, 1.0, &redsys->compactrhs, naug, &redsys->vm0, 0, _state);
    rmergemulv(m, &redsys->um0, &redsys->vm0, _state);
    rcopyvx(naug, &redsys->compactrhs, 0, &delta->x, 0, _state);
    rcopyvx(m, &redsys->vm0, 0, &delta->x, naug, _state);
    rcopyv(ntotal, &redsys->d2, &delta->s, _state);
    raddv(ntotal, 1.0, &delta->x, &delta->s, _state);
    rmergemulv(ntotal, &redsys->d3, &delta->s, _state);
    rcopyv(ntotal, &redsys->d6, &delta->z, _state);
    raddv(ntotal, -1.0, &delta->x, &delta->z, _state);
    rmergemulv(ntotal, &redsys->d7, &delta->z, _state);
    rcopynegmuladdv(ntotal, &current->t, &delta->s, &rhs->gammats, &delta->t, _state);
    rcopynegmuladdv(ntotal, &current->g, &delta->z, &rhs->gammagz, &delta->g, _state);
    rmergemulv(ntotal, &redsys->d1, &delta->t, _state);
    rmergemulv(ntotal, &redsys->d5, &delta->g, _state);
    rcopyvx(m, &redsys->compactrhs, naug, &delta->y, 0, _state);
    result = ae_true;
    return result;
}


/*************************************************************************
Computes minimum nonzero value of the vector. Returns 0 if all components
are nonpositive.

INPUT PARAMETERS:
    X               -   vector
    N               -   length

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double ipm2solver_minnz(/* Real    */ const ae_vector* x,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool nz;
    double result;


    result = (double)(0);
    nz = ae_false;
    for(i=n0; i<=n1-1; i++)
    {
        if( ae_fp_greater(x->ptr.p_double[i],(double)(0)) )
        {
            if( !nz )
            {
                result = x->ptr.p_double[i];
                nz = ae_true;
            }
            else
            {
                result = ae_minreal(result, x->ptr.p_double[i], _state);
            }
        }
    }
    return result;
}


/*************************************************************************
Computes minimum product of nonzero components.
Returns 0 if all components are nonpositive.

INPUT PARAMETERS:
    X               -   vector
    Y               -   vector
    N               -   length

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double ipm2solver_minprodnz(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool nz;
    double result;


    result = (double)(0);
    nz = ae_false;
    for(i=n0; i<=n1-1; i++)
    {
        if( ae_fp_greater(x->ptr.p_double[i],(double)(0))&&ae_fp_greater(y->ptr.p_double[i],(double)(0)) )
        {
            if( !nz )
            {
                result = x->ptr.p_double[i]*y->ptr.p_double[i];
                nz = ae_true;
            }
            else
            {
                result = ae_minreal(result, x->ptr.p_double[i]*y->ptr.p_double[i], _state);
            }
        }
    }
    return result;
}


/*************************************************************************
Computes maximum product of nonzero components.
Returns 0 if all components are nonpositive.

INPUT PARAMETERS:
    X               -   vector
    Y               -   vector
    N               -   length

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double ipm2solver_maxprodnz(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool nz;
    double result;


    result = (double)(0);
    nz = ae_false;
    for(i=n0; i<=n1-1; i++)
    {
        if( ae_fp_greater(x->ptr.p_double[i],(double)(0))&&ae_fp_greater(y->ptr.p_double[i],(double)(0)) )
        {
            if( !nz )
            {
                result = x->ptr.p_double[i]*y->ptr.p_double[i];
                nz = ae_true;
            }
            else
            {
                result = ae_maxreal(result, x->ptr.p_double[i]*y->ptr.p_double[i], _state);
            }
        }
    }
    return result;
}


/*************************************************************************
Computes maximum absolute value within a range of elements.
Debug function, not optimized.

INPUT PARAMETERS:
    X               -   vector
    R0,R1           -   range [R0,R1)

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double ipm2solver_maxabsrange(/* Real    */ const ae_vector* x,
     ae_int_t r0,
     ae_int_t r1,
     ae_state *_state)
{
    ae_int_t i;
    double result;


    result = (double)(0);
    for(i=r0; i<=r1-1; i++)
    {
        result = ae_maxreal(result, ae_fabs(x->ptr.p_double[i], _state), _state);
    }
    return result;
}


void _ipm2vars_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    ipm2vars *p = (ipm2vars*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->t, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->z, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
}


void _ipm2vars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    ipm2vars       *dst = (ipm2vars*)_dst;
    const ipm2vars *src = (const ipm2vars*)_src;
    dst->ntotal = src->ntotal;
    dst->m = src->m;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    ae_vector_init_copy(&dst->t, &src->t, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->z, &src->z, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
}


void _ipm2vars_clear(void* _p)
{
    ipm2vars *p = (ipm2vars*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->g);
    ae_vector_clear(&p->t);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->z);
    ae_vector_clear(&p->s);
}


void _ipm2vars_destroy(void* _p)
{
    ipm2vars *p = (ipm2vars*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->g);
    ae_vector_destroy(&p->t);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->z);
    ae_vector_destroy(&p->s);
}


void _ipm2reducedsystem_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    ipm2reducedsystem *p = (ipm2reducedsystem*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_init(&p->compactkkt, _state, make_automatic);
    ae_vector_init(&p->compactpriorities, 0, DT_INT, _state, make_automatic);
    _spcholanalysis_init(&p->analysis, _state, make_automatic);
    ae_vector_init(&p->extendedrawdiagonal, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->extendedeffdiagonal, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->compacteffdiagonal, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->extendedrhs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->extendedregrhs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->compactrhs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->u0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->u1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->v0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->v1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->vm0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->vm1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->um0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d3, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d4, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d5, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d6, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d7, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rowdegrees, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->coldegrees, 0, DT_INT, _state, make_automatic);
}


void _ipm2reducedsystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    ipm2reducedsystem       *dst = (ipm2reducedsystem*)_dst;
    const ipm2reducedsystem *src = (const ipm2reducedsystem*)_src;
    _sparsematrix_init_copy(&dst->compactkkt, &src->compactkkt, _state, make_automatic);
    ae_vector_init_copy(&dst->compactpriorities, &src->compactpriorities, _state, make_automatic);
    _spcholanalysis_init_copy(&dst->analysis, &src->analysis, _state, make_automatic);
    ae_vector_init_copy(&dst->extendedrawdiagonal, &src->extendedrawdiagonal, _state, make_automatic);
    ae_vector_init_copy(&dst->extendedeffdiagonal, &src->extendedeffdiagonal, _state, make_automatic);
    ae_vector_init_copy(&dst->compacteffdiagonal, &src->compacteffdiagonal, _state, make_automatic);
    ae_vector_init_copy(&dst->extendedrhs, &src->extendedrhs, _state, make_automatic);
    ae_vector_init_copy(&dst->extendedregrhs, &src->extendedregrhs, _state, make_automatic);
    ae_vector_init_copy(&dst->compactrhs, &src->compactrhs, _state, make_automatic);
    ae_vector_init_copy(&dst->u0, &src->u0, _state, make_automatic);
    ae_vector_init_copy(&dst->u1, &src->u1, _state, make_automatic);
    ae_vector_init_copy(&dst->v0, &src->v0, _state, make_automatic);
    ae_vector_init_copy(&dst->v1, &src->v1, _state, make_automatic);
    ae_vector_init_copy(&dst->vm0, &src->vm0, _state, make_automatic);
    ae_vector_init_copy(&dst->vm1, &src->vm1, _state, make_automatic);
    ae_vector_init_copy(&dst->um0, &src->um0, _state, make_automatic);
    ae_vector_init_copy(&dst->d0, &src->d0, _state, make_automatic);
    ae_vector_init_copy(&dst->d1, &src->d1, _state, make_automatic);
    ae_vector_init_copy(&dst->d2, &src->d2, _state, make_automatic);
    ae_vector_init_copy(&dst->d3, &src->d3, _state, make_automatic);
    ae_vector_init_copy(&dst->d4, &src->d4, _state, make_automatic);
    ae_vector_init_copy(&dst->d5, &src->d5, _state, make_automatic);
    ae_vector_init_copy(&dst->d6, &src->d6, _state, make_automatic);
    ae_vector_init_copy(&dst->d7, &src->d7, _state, make_automatic);
    ae_vector_init_copy(&dst->rowdegrees, &src->rowdegrees, _state, make_automatic);
    ae_vector_init_copy(&dst->coldegrees, &src->coldegrees, _state, make_automatic);
}


void _ipm2reducedsystem_clear(void* _p)
{
    ipm2reducedsystem *p = (ipm2reducedsystem*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_clear(&p->compactkkt);
    ae_vector_clear(&p->compactpriorities);
    _spcholanalysis_clear(&p->analysis);
    ae_vector_clear(&p->extendedrawdiagonal);
    ae_vector_clear(&p->extendedeffdiagonal);
    ae_vector_clear(&p->compacteffdiagonal);
    ae_vector_clear(&p->extendedrhs);
    ae_vector_clear(&p->extendedregrhs);
    ae_vector_clear(&p->compactrhs);
    ae_vector_clear(&p->u0);
    ae_vector_clear(&p->u1);
    ae_vector_clear(&p->v0);
    ae_vector_clear(&p->v1);
    ae_vector_clear(&p->vm0);
    ae_vector_clear(&p->vm1);
    ae_vector_clear(&p->um0);
    ae_vector_clear(&p->d0);
    ae_vector_clear(&p->d1);
    ae_vector_clear(&p->d2);
    ae_vector_clear(&p->d3);
    ae_vector_clear(&p->d4);
    ae_vector_clear(&p->d5);
    ae_vector_clear(&p->d6);
    ae_vector_clear(&p->d7);
    ae_vector_clear(&p->rowdegrees);
    ae_vector_clear(&p->coldegrees);
}


void _ipm2reducedsystem_destroy(void* _p)
{
    ipm2reducedsystem *p = (ipm2reducedsystem*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_destroy(&p->compactkkt);
    ae_vector_destroy(&p->compactpriorities);
    _spcholanalysis_destroy(&p->analysis);
    ae_vector_destroy(&p->extendedrawdiagonal);
    ae_vector_destroy(&p->extendedeffdiagonal);
    ae_vector_destroy(&p->compacteffdiagonal);
    ae_vector_destroy(&p->extendedrhs);
    ae_vector_destroy(&p->extendedregrhs);
    ae_vector_destroy(&p->compactrhs);
    ae_vector_destroy(&p->u0);
    ae_vector_destroy(&p->u1);
    ae_vector_destroy(&p->v0);
    ae_vector_destroy(&p->v1);
    ae_vector_destroy(&p->vm0);
    ae_vector_destroy(&p->vm1);
    ae_vector_destroy(&p->um0);
    ae_vector_destroy(&p->d0);
    ae_vector_destroy(&p->d1);
    ae_vector_destroy(&p->d2);
    ae_vector_destroy(&p->d3);
    ae_vector_destroy(&p->d4);
    ae_vector_destroy(&p->d5);
    ae_vector_destroy(&p->d6);
    ae_vector_destroy(&p->d7);
    ae_vector_destroy(&p->rowdegrees);
    ae_vector_destroy(&p->coldegrees);
}


void _ipm2righthandside_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    ipm2righthandside *p = (ipm2righthandside*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->gammagz, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gammats, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ed, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ea, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->el, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->eu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->eq, 0, DT_REAL, _state, make_automatic);
}


void _ipm2righthandside_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    ipm2righthandside       *dst = (ipm2righthandside*)_dst;
    const ipm2righthandside *src = (const ipm2righthandside*)_src;
    ae_vector_init_copy(&dst->gammagz, &src->gammagz, _state, make_automatic);
    ae_vector_init_copy(&dst->gammats, &src->gammats, _state, make_automatic);
    ae_vector_init_copy(&dst->ed, &src->ed, _state, make_automatic);
    ae_vector_init_copy(&dst->ea, &src->ea, _state, make_automatic);
    ae_vector_init_copy(&dst->el, &src->el, _state, make_automatic);
    ae_vector_init_copy(&dst->eu, &src->eu, _state, make_automatic);
    ae_vector_init_copy(&dst->eq, &src->eq, _state, make_automatic);
}


void _ipm2righthandside_clear(void* _p)
{
    ipm2righthandside *p = (ipm2righthandside*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->gammagz);
    ae_vector_clear(&p->gammats);
    ae_vector_clear(&p->ed);
    ae_vector_clear(&p->ea);
    ae_vector_clear(&p->el);
    ae_vector_clear(&p->eu);
    ae_vector_clear(&p->eq);
}


void _ipm2righthandside_destroy(void* _p)
{
    ipm2righthandside *p = (ipm2righthandside*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->gammagz);
    ae_vector_destroy(&p->gammats);
    ae_vector_destroy(&p->ed);
    ae_vector_destroy(&p->ea);
    ae_vector_destroy(&p->el);
    ae_vector_destroy(&p->eu);
    ae_vector_destroy(&p->eq);
}


void _ipm2state_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    ipm2state *p = (ipm2state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->sclx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->invsclx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xoriginx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ce, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsehe, _state, make_automatic);
    ae_vector_init(&p->diagr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndle, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndue, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndlef, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bnduef, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndle, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndue, 0, DT_BOOL, _state, make_automatic);
    _sparsematrix_init(&p->rawae, _state, make_automatic);
    ae_vector_init(&p->ascales, 0, DT_REAL, _state, make_automatic);
    _ipm2vars_init(&p->current, _state, make_automatic);
    _ipm2vars_init(&p->best, _state, make_automatic);
    _ipm2vars_init(&p->delta, _state, make_automatic);
    _ipm2vars_init(&p->corr, _state, make_automatic);
    ae_vector_init(&p->hasgz, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasts, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->maskgz, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->maskts, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->currenthx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->currentax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->currentaty, 0, DT_REAL, _state, make_automatic);
    _ipm2righthandside_init(&p->rhstarget, _state, make_automatic);
    _ipm2reducedsystem_init(&p->reducedsystem, _state, make_automatic);
    _ipm2righthandside_init(&p->rhsprogress, _state, make_automatic);
    ae_vector_init(&p->tmphx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpaty, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dummyr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpr2, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->tmpa, _state, make_automatic);
    ae_vector_init(&p->tmpal, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpau, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->tmpsparse0, _state, make_automatic);
    _sparsematrix_init(&p->tmplowerh, _state, make_automatic);
    ae_matrix_init(&p->tmpccorr, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpdcorr, 0, DT_REAL, _state, make_automatic);
}


void _ipm2state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    ipm2state       *dst = (ipm2state*)_dst;
    const ipm2state *src = (const ipm2state*)_src;
    dst->nuser = src->nuser;
    dst->naug = src->naug;
    dst->ntotal = src->ntotal;
    dst->epsp = src->epsp;
    dst->epsd = src->epsd;
    dst->epsgap = src->epsgap;
    dst->islinear = src->islinear;
    ae_vector_init_copy(&dst->sclx, &src->sclx, _state, make_automatic);
    ae_vector_init_copy(&dst->invsclx, &src->invsclx, _state, make_automatic);
    ae_vector_init_copy(&dst->xoriginx, &src->xoriginx, _state, make_automatic);
    dst->targetscale = src->targetscale;
    ae_vector_init_copy(&dst->ce, &src->ce, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsehe, &src->sparsehe, _state, make_automatic);
    ae_vector_init_copy(&dst->diagr, &src->diagr, _state, make_automatic);
    dst->isdiagonalh = src->isdiagonalh;
    ae_vector_init_copy(&dst->rawbndl, &src->rawbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->rawbndu, &src->rawbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->bndle, &src->bndle, _state, make_automatic);
    ae_vector_init_copy(&dst->bndue, &src->bndue, _state, make_automatic);
    ae_vector_init_copy(&dst->bndlef, &src->bndlef, _state, make_automatic);
    ae_vector_init_copy(&dst->bnduef, &src->bnduef, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndle, &src->hasbndle, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndue, &src->hasbndue, _state, make_automatic);
    _sparsematrix_init_copy(&dst->rawae, &src->rawae, _state, make_automatic);
    ae_vector_init_copy(&dst->ascales, &src->ascales, _state, make_automatic);
    dst->mraw = src->mraw;
    _ipm2vars_init_copy(&dst->current, &src->current, _state, make_automatic);
    _ipm2vars_init_copy(&dst->best, &src->best, _state, make_automatic);
    _ipm2vars_init_copy(&dst->delta, &src->delta, _state, make_automatic);
    _ipm2vars_init_copy(&dst->corr, &src->corr, _state, make_automatic);
    ae_vector_init_copy(&dst->hasgz, &src->hasgz, _state, make_automatic);
    ae_vector_init_copy(&dst->hasts, &src->hasts, _state, make_automatic);
    ae_vector_init_copy(&dst->maskgz, &src->maskgz, _state, make_automatic);
    ae_vector_init_copy(&dst->maskts, &src->maskts, _state, make_automatic);
    dst->cntgz = src->cntgz;
    dst->cntts = src->cntts;
    ae_vector_init_copy(&dst->currenthx, &src->currenthx, _state, make_automatic);
    ae_vector_init_copy(&dst->currentax, &src->currentax, _state, make_automatic);
    ae_vector_init_copy(&dst->currentaty, &src->currentaty, _state, make_automatic);
    _ipm2righthandside_init_copy(&dst->rhstarget, &src->rhstarget, _state, make_automatic);
    dst->repiterationscount = src->repiterationscount;
    dst->repncholesky = src->repncholesky;
    dst->dotrace = src->dotrace;
    dst->dodetailedtrace = src->dodetailedtrace;
    _ipm2reducedsystem_init_copy(&dst->reducedsystem, &src->reducedsystem, _state, make_automatic);
    _ipm2righthandside_init_copy(&dst->rhsprogress, &src->rhsprogress, _state, make_automatic);
    ae_vector_init_copy(&dst->tmphx, &src->tmphx, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpax, &src->tmpax, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpaty, &src->tmpaty, _state, make_automatic);
    ae_vector_init_copy(&dst->dummyr, &src->dummyr, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpr2, &src->tmpr2, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpa, &src->tmpa, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpal, &src->tmpal, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpau, &src->tmpau, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpsparse0, &src->tmpsparse0, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmplowerh, &src->tmplowerh, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpccorr, &src->tmpccorr, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpdcorr, &src->tmpdcorr, _state, make_automatic);
}


void _ipm2state_clear(void* _p)
{
    ipm2state *p = (ipm2state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->sclx);
    ae_vector_clear(&p->invsclx);
    ae_vector_clear(&p->xoriginx);
    ae_vector_clear(&p->ce);
    _sparsematrix_clear(&p->sparsehe);
    ae_vector_clear(&p->diagr);
    ae_vector_clear(&p->rawbndl);
    ae_vector_clear(&p->rawbndu);
    ae_vector_clear(&p->bndle);
    ae_vector_clear(&p->bndue);
    ae_vector_clear(&p->bndlef);
    ae_vector_clear(&p->bnduef);
    ae_vector_clear(&p->hasbndle);
    ae_vector_clear(&p->hasbndue);
    _sparsematrix_clear(&p->rawae);
    ae_vector_clear(&p->ascales);
    _ipm2vars_clear(&p->current);
    _ipm2vars_clear(&p->best);
    _ipm2vars_clear(&p->delta);
    _ipm2vars_clear(&p->corr);
    ae_vector_clear(&p->hasgz);
    ae_vector_clear(&p->hasts);
    ae_vector_clear(&p->maskgz);
    ae_vector_clear(&p->maskts);
    ae_vector_clear(&p->currenthx);
    ae_vector_clear(&p->currentax);
    ae_vector_clear(&p->currentaty);
    _ipm2righthandside_clear(&p->rhstarget);
    _ipm2reducedsystem_clear(&p->reducedsystem);
    _ipm2righthandside_clear(&p->rhsprogress);
    ae_vector_clear(&p->tmphx);
    ae_vector_clear(&p->tmpax);
    ae_vector_clear(&p->tmpaty);
    ae_vector_clear(&p->dummyr);
    ae_vector_clear(&p->tmp0);
    ae_matrix_clear(&p->tmpr2);
    _sparsematrix_clear(&p->tmpa);
    ae_vector_clear(&p->tmpal);
    ae_vector_clear(&p->tmpau);
    _sparsematrix_clear(&p->tmpsparse0);
    _sparsematrix_clear(&p->tmplowerh);
    ae_matrix_clear(&p->tmpccorr);
    ae_vector_clear(&p->tmpdcorr);
}


void _ipm2state_destroy(void* _p)
{
    ipm2state *p = (ipm2state*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->sclx);
    ae_vector_destroy(&p->invsclx);
    ae_vector_destroy(&p->xoriginx);
    ae_vector_destroy(&p->ce);
    _sparsematrix_destroy(&p->sparsehe);
    ae_vector_destroy(&p->diagr);
    ae_vector_destroy(&p->rawbndl);
    ae_vector_destroy(&p->rawbndu);
    ae_vector_destroy(&p->bndle);
    ae_vector_destroy(&p->bndue);
    ae_vector_destroy(&p->bndlef);
    ae_vector_destroy(&p->bnduef);
    ae_vector_destroy(&p->hasbndle);
    ae_vector_destroy(&p->hasbndue);
    _sparsematrix_destroy(&p->rawae);
    ae_vector_destroy(&p->ascales);
    _ipm2vars_destroy(&p->current);
    _ipm2vars_destroy(&p->best);
    _ipm2vars_destroy(&p->delta);
    _ipm2vars_destroy(&p->corr);
    ae_vector_destroy(&p->hasgz);
    ae_vector_destroy(&p->hasts);
    ae_vector_destroy(&p->maskgz);
    ae_vector_destroy(&p->maskts);
    ae_vector_destroy(&p->currenthx);
    ae_vector_destroy(&p->currentax);
    ae_vector_destroy(&p->currentaty);
    _ipm2righthandside_destroy(&p->rhstarget);
    _ipm2reducedsystem_destroy(&p->reducedsystem);
    _ipm2righthandside_destroy(&p->rhsprogress);
    ae_vector_destroy(&p->tmphx);
    ae_vector_destroy(&p->tmpax);
    ae_vector_destroy(&p->tmpaty);
    ae_vector_destroy(&p->dummyr);
    ae_vector_destroy(&p->tmp0);
    ae_matrix_destroy(&p->tmpr2);
    _sparsematrix_destroy(&p->tmpa);
    ae_vector_destroy(&p->tmpal);
    ae_vector_destroy(&p->tmpau);
    _sparsematrix_destroy(&p->tmpsparse0);
    _sparsematrix_destroy(&p->tmplowerh);
    ae_matrix_destroy(&p->tmpccorr);
    ae_vector_destroy(&p->tmpdcorr);
}


/*$ End $*/
