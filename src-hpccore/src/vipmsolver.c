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
#include "vipmsolver.h"


/*$ Declarations $*/
static double vipmsolver_muquasidense = 5.0;
static ae_int_t vipmsolver_maxipmits = 200;
static double vipmsolver_initslackval = 100.0;
static double vipmsolver_steplengthdecay = 0.95;
static double vipmsolver_stagnationdelta = 0.99999;
static double vipmsolver_primalinfeasible1 = 1.0E-3;
static double vipmsolver_dualinfeasible1 = 1.0E-3;
static double vipmsolver_bigy = 1.0E8;
static double vipmsolver_ygrowth = 1.0E6;
static ae_int_t vipmsolver_phase0length = 10;
static ae_int_t vipmsolver_itersfortoostringentcond = 25;
static ae_int_t vipmsolver_minitersbeforedroppingbounds = 3;
static ae_int_t vipmsolver_minitersbeforeinfeasible = 3;
static ae_int_t vipmsolver_minitersbeforestagnation = 5;
static ae_int_t vipmsolver_minitersbeforeeworststagnation = 50;
static ae_int_t vipmsolver_primalstagnationlen = 5;
static ae_int_t vipmsolver_dualstagnationlen = 7;
static double vipmsolver_bigconstrxtol = 1.0E-5;
static double vipmsolver_bigconstrmag = 1.0E3;
static double vipmsolver_minitersbeforesafeguards = 5;
static double vipmsolver_badsteplength = 1.0E-3;
static ae_int_t vipmsolver_maxrefinementits = 5;
static void vipmsolver_varsinitbyzero(vipmvars* vstate,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state);
static void vipmsolver_varsinitfrom(vipmvars* vstate,
     const vipmvars* vsrc,
     ae_state *_state);
static void vipmsolver_varsaddstep(vipmvars* vstate,
     const vipmvars* vdir,
     double stpp,
     double stpd,
     ae_state *_state);
static double vipmsolver_varscomputecomplementaritygap(const vipmvars* vstate,
     ae_state *_state);
static double vipmsolver_varscomputemu(const vipmstate* state,
     const vipmvars* vstate,
     ae_state *_state);
static void vipmsolver_reducedsysteminit(vipmreducedsparsesystem* s,
     const vipmstate* solver,
     ae_state *_state);
static ae_bool vipmsolver_reducedsystemfactorizewithaddends(vipmreducedsparsesystem* s,
     /* Real    */ const ae_vector* diag,
     /* Real    */ const ae_vector* damp,
     double modeps,
     double badchol,
     double* sumsq,
     double* errsq,
     ae_state *_state);
static void vipmsolver_reducedsystemsolve(vipmreducedsparsesystem* s,
     ae_bool dotrace,
     /* Real    */ ae_vector* b,
     ae_state *_state);
static void vipmsolver_vipminit(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_int_t nmain,
     ae_int_t ftype,
     ae_state *_state);
static double vipmsolver_vipmtarget(const vipmstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
static void vipmsolver_multiplygeax(const vipmstate* state,
     double alpha,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t offsax,
     ae_state *_state);
static void vipmsolver_multiplygeatx(const vipmstate* state,
     double alpha,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
static void vipmsolver_multiplyhx(const vipmstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     ae_state *_state);
static void vipmsolver_vipmmultiply(const vipmstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* hx,
     /* Real    */ ae_vector* ax,
     /* Real    */ ae_vector* aty,
     ae_state *_state);
static void vipmsolver_vipmpowerup(vipmstate* state,
     double regfree,
     ae_state *_state);
static ae_bool vipmsolver_vipmfactorize(vipmstate* state,
     double alpha0,
     /* Real    */ const ae_vector* d,
     double beta0,
     /* Real    */ const ae_vector* e,
     double alpha11,
     double beta11,
     double modeps,
     double dampeps,
     ae_state *_state);
static void vipmsolver_solvereducedkktsystem(vipmstate* state,
     /* Real    */ ae_vector* deltaxy,
     ae_state *_state);
static ae_bool vipmsolver_vipmprecomputenewtonfactorization(vipmstate* state,
     const vipmvars* v0,
     double regeps,
     double modeps,
     double dampeps,
     double dampfree,
     ae_state *_state);
static void vipmsolver_solvekktsystem(vipmstate* state,
     const vipmrighthandside* rhs,
     vipmvars* sol,
     ae_state *_state);
static ae_bool vipmsolver_vipmcomputestepdirection(vipmstate* state,
     const vipmvars* v0,
     double muestimate,
     const vipmvars* vdestimate,
     vipmvars* vdresult,
     double reg,
     ae_bool isdampepslarge,
     ae_state *_state);
static void vipmsolver_vipmcomputesteplength(const vipmstate* state,
     const vipmvars* v0,
     const vipmvars* vs,
     double stepdecay,
     ae_bool separatestep,
     double* alphap,
     double* alphad,
     ae_state *_state);
static void vipmsolver_vipmperformstep(vipmstate* state,
     double alphap,
     double alphad,
     ae_state *_state);
static void vipmsolver_computeerrors(vipmstate* state,
     double* errp2,
     double* errd2,
     double* errpinf,
     double* errdinf,
     double* egap,
     ae_state *_state);
static void vipmsolver_runintegritychecks(const vipmstate* state,
     const vipmvars* v0,
     const vipmvars* vd,
     double alphap,
     double alphad,
     ae_state *_state);
static void vipmsolver_traceprogress(vipmstate* state,
     double mu,
     double muaff,
     double sigma,
     double alphap,
     double alphad,
     ae_state *_state);
static void vipmsolver_rhscompute(vipmstate* state,
     const vipmvars* v0,
     double muestimate,
     const vipmvars* direstimate,
     vipmrighthandside* rhs,
     double reg,
     ae_state *_state);
static void vipmsolver_rhssubtract(vipmstate* state,
     vipmrighthandside* rhs,
     const vipmvars* v0,
     const vipmvars* vdcandidate,
     double reg,
     ae_state *_state);
static double vipmsolver_rhsprimal2(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state);
static double vipmsolver_rhsdual2(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state);
static double vipmsolver_rhsprimalinf(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state);
static double vipmsolver_rhsdualinf(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state);
static double vipmsolver_rhscompl2(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state);
static double vipmsolver_minnz(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
static double vipmsolver_minprodnz(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_state *_state);
static double vipmsolver_maxprodnz(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

The solver is configured to work internally with dense NxN  factorization,
no matter what exactly is passed - dense or sparse matrices.

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

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipminitdense(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_state *_state)
{


    ae_assert(n>=1, "VIPMInitDense: N<1", _state);
    ae_assert(isfinitevector(s, n, _state), "VIPMInitDense: S contains infinite or NaN elements", _state);
    ae_assert(isfinitevector(xorigin, n, _state), "VIPMInitDense: XOrigin contains infinite or NaN elements", _state);
    vipmsolver_vipminit(state, s, xorigin, n, n, 0, _state);
}


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

The solver is configured to work internally with dense NxN problem divided
into two distinct parts - "main" and slack one:
* dense quadratic term  is  a  NMain*NMain  matrix  (NMain<=N),  quadratic
  coefficients are zero for variables outside of [0,NMain) range)
* linear term is general vector of length N
* linear constraints have special structure for variable with  indexes  in
  [NMain,N) range: at most one element per column can be nonzero.

This mode is intended for problems arising during SL1QP nonlinear programming.

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
    NMain       -   number of "main" variables, 1<=NMain<=N
    N           -   total number of variables including slack ones

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipminitdensewithslacks(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nmain,
     ae_int_t n,
     ae_state *_state)
{


    ae_assert(nmain>=1, "VIPMInitDense: NMain<1", _state);
    ae_assert(n>=1, "VIPMInitDense: N<1", _state);
    ae_assert(nmain<=n, "VIPMInitDense: NMain>N", _state);
    ae_assert(isfinitevector(s, n, _state), "VIPMInitDense: S contains infinite or NaN elements", _state);
    ae_assert(isfinitevector(xorigin, n, _state), "VIPMInitDense: XOrigin contains infinite or NaN elements", _state);
    vipmsolver_vipminit(state, s, xorigin, n, nmain, 0, _state);
}


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

The  solver  is configured  to  work  internally  with  sparse (N+M)x(N+M)
factorization no matter what exactly is passed - dense or sparse matrices.
Dense quadratic term will be sparsified prior to storage.

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
    
This optimization mode assumes that no slack variables is present.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipminitsparse(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_state *_state)
{


    ae_assert(n>=1, "VIPMInitSparse: N<1", _state);
    ae_assert(isfinitevector(s, n, _state), "VIPMInitSparse: S contains infinite or NaN elements", _state);
    ae_assert(isfinitevector(xorigin, n, _state), "VIPMInitSparse: XOrigin contains infinite or NaN elements", _state);
    vipmsolver_vipminit(state, s, xorigin, n, n, 1, _state);
}


/*************************************************************************
Sets linear/quadratic terms for QP-IPM solver

If you initialized solver with VIMPInitDenseWithSlacks(), NMain below is a
number of non-slack variables. In other cases, NMain=N.

INPUT PARAMETERS:
    State               -   instance initialized with one of the initialization
                            functions
    DenseH              -   if HKind=0: array[NMain,NMain], dense quadratic term
                            (either upper or lower triangle)
    SparseH             -   if HKind=1: array[NMain,NMain], sparse quadratic term
                            (either upper or lower triangle)
    HKind               -   0 or 1, quadratic term format
    IsUpper             -   whether dense/sparse H contains lower or upper
                            triangle of the quadratic term
    C                   -   array[N], linear term

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipmsetquadraticlinear(vipmstate* state,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     ae_bool isupper,
     /* Real    */ const ae_vector* c,
     ae_state *_state)
{
    ae_int_t nmain;
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    double v;
    double vv;
    ae_int_t nnz;
    ae_int_t offs;


    nmain = state->nmain;
    n = state->n;
    ae_assert(hkind==0||hkind==1, "VIPMSetQuadraticLinear: incorrect HKind", _state);
    ae_assert(isfinitevector(c, n, _state), "VIPMSetQuadraticLinear: C contains infinite or NaN elements", _state);
    ae_assert(state->factorizationtype==0||state->factorizationtype==1, "VIPMSetQuadraticLinear: unexpected factorization type", _state);
    
    /*
     * Set problem info, reset factorization flag
     */
    state->islinear = ae_false;
    state->factorizationpresent = ae_false;
    state->factorizationpoweredup = ae_false;
    
    /*
     * Linear term
     */
    rvectorsetlengthatleast(&state->c, n, _state);
    rvectorcopy(n, c, 0, &state->c, 0, _state);
    
    /*
     * Quadratic term and normalization
     *
     * NOTE: we perform integrity check for inifinities/NANs by
     *       computing sum of all matrix elements and checking its
     *       value for being finite. It is a bit faster than checking
     *       each element individually.
     */
    state->hkind = -1;
    state->targetscale = 1.0;
    if( state->factorizationtype==0 )
    {
        
        /*
         * Quadratic term is stored in dense format: either copy dense
         * term of densify sparse one
         */
        state->hkind = 0;
        rmatrixsetlengthatleast(&state->denseh, nmain, nmain, _state);
        if( hkind==0 )
        {
            
            /*
             * Copy dense quadratic term
             */
            if( isupper )
            {
                rmatrixtranspose(nmain, nmain, denseh, 0, 0, &state->denseh, 0, 0, _state);
            }
            else
            {
                rmatrixcopy(nmain, nmain, denseh, 0, 0, &state->denseh, 0, 0, _state);
            }
        }
        if( hkind==1 )
        {
            
            /*
             * Extract sparse quadratic term
             */
            ae_assert(sparseh->matrixtype==1, "VIPMSetQuadraticLinear: unexpected sparse matrix format", _state);
            ae_assert(sparseh->m==nmain, "VIPMSetQuadraticLinear: unexpected sparse matrix size", _state);
            ae_assert(sparseh->n==nmain, "VIPMSetQuadraticLinear: unexpected sparse matrix size", _state);
            for(i=0; i<=nmain-1; i++)
            {
                for(j=0; j<=i; j++)
                {
                    state->denseh.ptr.pp_double[i][j] = (double)(0);
                }
            }
            for(i=0; i<=nmain-1; i++)
            {
                
                /*
                 * diagonal element
                 */
                if( sparseh->didx.ptr.p_int[i]!=sparseh->uidx.ptr.p_int[i] )
                {
                    state->denseh.ptr.pp_double[i][i] = sparseh->vals.ptr.p_double[sparseh->didx.ptr.p_int[i]];
                }
                
                /*
                 * Off-diagonal elements
                 */
                if( isupper )
                {
                    
                    /*
                     * superdiagonal elements are moved to subdiagonal part
                     */
                    j0 = sparseh->uidx.ptr.p_int[i];
                    j1 = sparseh->ridx.ptr.p_int[i+1]-1;
                    for(j=j0; j<=j1; j++)
                    {
                        state->denseh.ptr.pp_double[sparseh->idx.ptr.p_int[j]][i] = sparseh->vals.ptr.p_double[j];
                    }
                }
                else
                {
                    
                    /*
                     * subdiagonal elements are moved to subdiagonal part
                     */
                    j0 = sparseh->ridx.ptr.p_int[i];
                    j1 = sparseh->didx.ptr.p_int[i]-1;
                    for(j=j0; j<=j1; j++)
                    {
                        state->denseh.ptr.pp_double[i][sparseh->idx.ptr.p_int[j]] = sparseh->vals.ptr.p_double[j];
                    }
                }
            }
        }
        vv = (double)(0);
        for(i=0; i<=nmain-1; i++)
        {
            for(j=0; j<=i; j++)
            {
                vv = vv+state->denseh.ptr.pp_double[i][j];
            }
        }
        ae_assert(ae_isfinite(vv, _state), "VIPMSetQuadraticLinear: DenseH contains infinite or NaN values!", _state);
        scaledenseqpinplace(&state->denseh, ae_false, nmain, &state->c, n, &state->scl, _state);
        state->targetscale = normalizedenseqpinplace(&state->denseh, ae_false, nmain, &state->c, n, _state);
        state->isdiagonalh = ae_false;
    }
    if( state->factorizationtype==1 )
    {
        ae_assert(nmain==n, "VIPMSetQuadraticLinear: critical integrity check failed, NMain!=N", _state);
        
        /*
         * Quadratic term is stored in sparse format: either sparsify dense
         * term or copy the sparse one
         */
        state->hkind = 1;
        state->sparseh.matrixtype = 1;
        state->sparseh.m = n;
        state->sparseh.n = n;
        if( hkind==0 )
        {
            
            /*
             * Sparsify dense term
             */
            nnz = 0;
            for(i=0; i<=n-1; i++)
            {
                nnz = nnz+1;
                if( isupper )
                {
                    j0 = i+1;
                    j1 = n-1;
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
            ivectorsetlengthatleast(&state->sparseh.ridx, n+1, _state);
            ivectorsetlengthatleast(&state->sparseh.idx, nnz, _state);
            rvectorsetlengthatleast(&state->sparseh.vals, nnz, _state);
            state->sparseh.ridx.ptr.p_int[0] = 0;
            offs = 0;
            vv = (double)(0);
            for(i=0; i<=n-1; i++)
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
                            state->sparseh.idx.ptr.p_int[offs] = j;
                            state->sparseh.vals.ptr.p_double[offs] = v;
                            vv = vv+v;
                            offs = offs+1;
                        }
                    }
                }
                
                /*
                 * Diagonal element is always copied
                 */
                v = denseh->ptr.pp_double[i][i];
                state->sparseh.idx.ptr.p_int[offs] = i;
                state->sparseh.vals.ptr.p_double[offs] = v;
                vv = vv+v;
                offs = offs+1;
                
                /*
                 * Off-diagonal elements are copied only when nonzero
                 */
                if( isupper )
                {
                    for(j=i+1; j<=n-1; j++)
                    {
                        if( denseh->ptr.pp_double[i][j]!=(double)0 )
                        {
                            v = denseh->ptr.pp_double[i][j];
                            state->sparseh.idx.ptr.p_int[offs] = j;
                            state->sparseh.vals.ptr.p_double[offs] = v;
                            vv = vv+v;
                            offs = offs+1;
                        }
                    }
                }
                
                /*
                 * Finalize row
                 */
                state->sparseh.ridx.ptr.p_int[i+1] = offs;
            }
            ae_assert(ae_isfinite(vv, _state), "VIPMSetQuadraticLinear: DenseH contains infinite or NaN values!", _state);
            ae_assert(offs==nnz, "VIPMSetQuadraticLinear: integrity check failed", _state);
            sparsecreatecrsinplace(&state->sparseh, _state);
        }
        if( hkind==1 )
        {
            
            /*
             * Copy sparse quadratic term, but make sure that we have diagonal elements
             * present (we add diagonal if it is not present)
             */
            ae_assert(sparseh->matrixtype==1, "VIPMSetQuadraticLinear: unexpected sparse matrix format", _state);
            ae_assert(sparseh->m==n, "VIPMSetQuadraticLinear: unexpected sparse matrix size", _state);
            ae_assert(sparseh->n==n, "VIPMSetQuadraticLinear: unexpected sparse matrix size", _state);
            ivectorsetlengthatleast(&state->sparseh.ridx, n+1, _state);
            ivectorsetlengthatleast(&state->sparseh.idx, sparseh->ridx.ptr.p_int[n]+n, _state);
            rvectorsetlengthatleast(&state->sparseh.vals, sparseh->ridx.ptr.p_int[n]+n, _state);
            state->sparseh.ridx.ptr.p_int[0] = 0;
            offs = 0;
            vv = (double)(0);
            for(i=0; i<=n-1; i++)
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
                        state->sparseh.idx.ptr.p_int[offs] = sparseh->idx.ptr.p_int[k];
                        state->sparseh.vals.ptr.p_double[offs] = v;
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
                state->sparseh.idx.ptr.p_int[offs] = i;
                state->sparseh.vals.ptr.p_double[offs] = v;
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
                        state->sparseh.idx.ptr.p_int[offs] = sparseh->idx.ptr.p_int[k];
                        state->sparseh.vals.ptr.p_double[offs] = v;
                        vv = vv+v;
                        offs = offs+1;
                    }
                }
                
                /*
                 * Finalize row
                 */
                state->sparseh.ridx.ptr.p_int[i+1] = offs;
            }
            ae_assert(ae_isfinite(vv, _state), "VIPMSetQuadraticLinear: SparseH contains infinite or NaN values!", _state);
            ae_assert(offs<=state->sparseh.vals.cnt&&offs<=state->sparseh.idx.cnt, "VIPMSetQuadraticLinear: integrity check failed", _state);
            sparsecreatecrsinplace(&state->sparseh, _state);
        }
        if( isupper )
        {
            sparsecopytransposecrsbuf(&state->sparseh, &state->tmpsparse0, _state);
            sparsecopybuf(&state->tmpsparse0, &state->sparseh, _state);
        }
        
        /*
         * Finalize
         */
        scalesparseqpinplace(&state->scl, n, &state->sparseh, &state->tmpr2, &state->dummyr, 0, &state->c, _state);
        state->targetscale = normalizesparseqpinplace(&state->sparseh, ae_false, &state->tmpr2, &state->dummyr, 0, &state->c, n, _state);
        state->isdiagonalh = state->sparseh.ridx.ptr.p_int[n]==n;
    }
    ae_assert(state->hkind>=0, "VIPMSetQuadraticLinear: integrity check failed", _state);
}


/*************************************************************************
Sets constraints for QP-IPM solver

INPUT PARAMETERS:
    State               -   instance initialized with one of the initialization
                            functions
    BndL, BndU          -   lower and upper bound. BndL[] can be -INF,
                            BndU[] can be +INF.
    SparseA             -   sparse constraint matrix, CRS format
    MSparse             -   number of sparse constraints
    DenseA              -   array[MDense,N], dense part of the constraints
    MDense              -   number of dense constraints
    CL, CU              -   lower and upper bounds for constraints, first
                            MSparse are bounds for sparse part, following
                            MDense ones are bounds for dense part,
                            MSparse+MDense in total.
                            -INF <= CL[I] <= CU[I] <= +INF.
                            
This function throws exception if constraints have inconsistent bounds, i.e.
either BndL[I]>BndU[I] or CL[I]>CU[I]. In all other cases it succeeds.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipmsetconstraints(vipmstate* state,
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
    ae_int_t m;
    ae_int_t n;
    ae_int_t nmain;
    ae_int_t nslack;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t k;
    ae_int_t offsmain;
    ae_int_t offscombined;
    double vs;
    double v;


    n = state->n;
    nmain = state->nmain;
    nslack = n-nmain;
    ae_assert(mdense>=0, "VIPMSetConstraints: MDense<0", _state);
    ae_assert(msparse>=0, "VIPMSetConstraints: MSparse<0", _state);
    ae_assert(apservisfinitematrix(densea, mdense, n, _state), "VIPMSetConstraints: DenseA contains infinite or NaN values!", _state);
    ae_assert(msparse==0||sparsea->matrixtype==1, "VIPMSetConstraints: non-CRS constraint matrix!", _state);
    ae_assert(msparse==0||(sparsea->m==msparse&&sparsea->n==n), "VIPMSetConstraints: constraint matrix has incorrect size", _state);
    ae_assert(cl->cnt>=mdense+msparse, "VIPMSetConstraints: CL is too short!", _state);
    ae_assert(cu->cnt>=mdense+msparse, "VIPMSetConstraints: CU is too short!", _state);
    
    /*
     * Reset factorization flag
     */
    state->factorizationpresent = ae_false;
    state->factorizationpoweredup = ae_false;
    
    /*
     * Box constraints
     */
    rvectorsetlengthatleast(&state->bndl, n, _state);
    rvectorsetlengthatleast(&state->bndu, n, _state);
    rvectorsetlengthatleast(&state->rawbndl, n, _state);
    rvectorsetlengthatleast(&state->rawbndu, n, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->hasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->hasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
        ae_assert(!((state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_greater(bndl->ptr.p_double[i],bndu->ptr.p_double[i])), "VIPMInitDenseQuadratic: inconsistent range for box constraints", _state);
        state->bndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->bndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        state->rawbndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->rawbndu.ptr.p_double[i] = bndu->ptr.p_double[i];
    }
    scaleshiftbcinplace(&state->scl, &state->xorigin, &state->bndl, &state->bndu, n, _state);
    
    /*
     * Linear constraints (full matrices)
     */
    m = mdense+msparse;
    rvectorsetlengthatleast(&state->b, m, _state);
    rvectorsetlengthatleast(&state->r, m, _state);
    rvectorsetlengthatleast(&state->ascales, m, _state);
    bvectorsetlengthatleast(&state->aflips, m, _state);
    bvectorsetlengthatleast(&state->hasr, m, _state);
    rmatrixsetlengthatleast(&state->denseafull, mdense, n, _state);
    if( msparse>0 )
    {
        sparsecopytocrsbuf(sparsea, &state->sparseafull, _state);
    }
    if( mdense>0 )
    {
        rmatrixcopy(mdense, n, densea, 0, 0, &state->denseafull, 0, 0, _state);
    }
    for(i=0; i<=m-1; i++)
    {
        ae_assert(ae_isfinite(cl->ptr.p_double[i], _state)||ae_isneginf(cl->ptr.p_double[i], _state), "VIPMInitDenseQuadratic: CL is not finite number or -INF", _state);
        ae_assert(ae_isfinite(cu->ptr.p_double[i], _state)||ae_isposinf(cu->ptr.p_double[i], _state), "VIPMInitDenseQuadratic: CU is not finite number or +INF", _state);
        
        /*
         * Store range
         */
        if( ae_isfinite(cl->ptr.p_double[i], _state)||ae_isfinite(cu->ptr.p_double[i], _state) )
        {
            
            /*
             * Non-degenerate constraint, at least one of bounds is present
             */
            if( ae_isfinite(cl->ptr.p_double[i], _state) )
            {
                ae_assert(!ae_isfinite(cu->ptr.p_double[i], _state)||ae_fp_greater_eq(cu->ptr.p_double[i],cl->ptr.p_double[i]), "VIPMInitDenseQuadratic: inconsistent range (right-hand side) for linear constraint", _state);
                if( ae_isfinite(cu->ptr.p_double[i], _state) )
                {
                    
                    /*
                     * We have both CL and CU, i.e. CL <= A*x <= CU.
                     *
                     * It can be either equality constraint (no slacks) or range constraint
                     * (two pairs of slacks variables).
                     *
                     * Try to arrange things in such a way that |CU|>=|CL| (it can be done
                     * by multiplication by -1 and boundaries swap).
                     * 
                     * Having |CU|>=|CL| will allow us to drop huge irrelevant bound CU,
                     * if we find it irrelevant during computations. Due to limitations
                     * of our slack variable substitution, it can be done only for CU.
                     */
                    if( ae_fp_greater_eq(ae_fabs(cu->ptr.p_double[i], _state),ae_fabs(cl->ptr.p_double[i], _state)) )
                    {
                        state->b.ptr.p_double[i] = cl->ptr.p_double[i];
                        state->r.ptr.p_double[i] = cu->ptr.p_double[i]-cl->ptr.p_double[i];
                        state->hasr.ptr.p_bool[i] = ae_true;
                        state->aflips.ptr.p_bool[i] = ae_false;
                        vs = (double)(1);
                    }
                    else
                    {
                        state->b.ptr.p_double[i] = -cu->ptr.p_double[i];
                        state->r.ptr.p_double[i] = cu->ptr.p_double[i]-cl->ptr.p_double[i];
                        state->hasr.ptr.p_bool[i] = ae_true;
                        state->aflips.ptr.p_bool[i] = ae_true;
                        vs = (double)(-1);
                    }
                }
                else
                {
                    
                    /*
                     * Only lower bound: CL <= A*x.
                     *
                     * One pair of slack variables added.
                     */
                    state->b.ptr.p_double[i] = cl->ptr.p_double[i];
                    state->r.ptr.p_double[i] = _state->v_posinf;
                    state->hasr.ptr.p_bool[i] = ae_false;
                    state->aflips.ptr.p_bool[i] = ae_false;
                    vs = (double)(1);
                }
            }
            else
            {
                
                /*
                 * Only upper bound: A*x <= CU
                 *
                 * One pair of slack variables added.
                 */
                state->b.ptr.p_double[i] = -cu->ptr.p_double[i];
                state->r.ptr.p_double[i] = _state->v_posinf;
                state->hasr.ptr.p_bool[i] = ae_false;
                state->aflips.ptr.p_bool[i] = ae_true;
                vs = (double)(-1);
            }
        }
        else
        {
            
            /*
             * Degenerate constraint -inf <= Ax <= +inf.
             * Generate dummy formulation.
             */
            state->b.ptr.p_double[i] = (double)(-1);
            state->r.ptr.p_double[i] = (double)(2);
            state->hasr.ptr.p_bool[i] = ae_true;
            state->aflips.ptr.p_bool[i] = ae_false;
            vs = (double)(0);
        }
        
        /*
         * Store matrix row and its scaling coefficient
         */
        if( i<msparse )
        {
            j0 = state->sparseafull.ridx.ptr.p_int[i];
            j1 = state->sparseafull.ridx.ptr.p_int[i+1]-1;
            for(j=j0; j<=j1; j++)
            {
                state->sparseafull.vals.ptr.p_double[j] = state->sparseafull.vals.ptr.p_double[j]*vs;
            }
        }
        else
        {
            for(j=0; j<=n-1; j++)
            {
                state->denseafull.ptr.pp_double[i-msparse][j] = state->denseafull.ptr.pp_double[i-msparse][j]*vs;
            }
        }
        state->ascales.ptr.p_double[i] = vs;
    }
    scaleshiftmixedbrlcinplace(&state->scl, &state->xorigin, n, &state->sparseafull, msparse, &state->denseafull, mdense, &state->b, &state->r, _state);
    normalizemixedbrlcinplace(&state->sparseafull, msparse, &state->denseafull, mdense, &state->b, &state->r, n, ae_true, &state->tmp0, ae_true, _state);
    for(i=0; i<=m-1; i++)
    {
        state->ascales.ptr.p_double[i] = state->ascales.ptr.p_double[i]*state->tmp0.ptr.p_double[i];
    }
    state->mdense = mdense;
    state->msparse = msparse;
    
    /*
     * Separate main and slack parts of the constraint matrices
     */
    ivectorsetlengthatleast(&state->tmpi, nslack, _state);
    for(i=0; i<=nslack-1; i++)
    {
        state->tmpi.ptr.p_int[i] = 0;
    }
    state->combinedaslack.m = mdense+msparse;
    state->combinedaslack.n = nslack;
    ivectorsetlengthatleast(&state->combinedaslack.ridx, mdense+msparse+1, _state);
    ivectorsetlengthatleast(&state->combinedaslack.idx, nslack, _state);
    rvectorsetlengthatleast(&state->combinedaslack.vals, nslack, _state);
    state->combinedaslack.ridx.ptr.p_int[0] = 0;
    state->sparseamain.m = msparse;
    state->sparseamain.n = nmain;
    if( msparse>0 )
    {
        ivectorsetlengthatleast(&state->sparseamain.ridx, msparse+1, _state);
        ivectorsetlengthatleast(&state->sparseamain.idx, sparsea->ridx.ptr.p_int[msparse], _state);
        rvectorsetlengthatleast(&state->sparseamain.vals, sparsea->ridx.ptr.p_int[msparse], _state);
        state->sparseamain.ridx.ptr.p_int[0] = 0;
        for(i=0; i<=msparse-1; i++)
        {
            offsmain = state->sparseamain.ridx.ptr.p_int[i];
            offscombined = state->combinedaslack.ridx.ptr.p_int[i];
            j0 = state->sparseafull.ridx.ptr.p_int[i];
            j1 = state->sparseafull.ridx.ptr.p_int[i+1]-1;
            for(j=j0; j<=j1; j++)
            {
                v = state->sparseafull.vals.ptr.p_double[j];
                k = state->sparseafull.idx.ptr.p_int[j];
                if( k<nmain )
                {
                    state->sparseamain.idx.ptr.p_int[offsmain] = k;
                    state->sparseamain.vals.ptr.p_double[offsmain] = v;
                    offsmain = offsmain+1;
                }
                else
                {
                    ae_assert(state->tmpi.ptr.p_int[k-nmain]==0, "VIPMSetConstraints: slack column contains more than one nonzero element", _state);
                    state->combinedaslack.idx.ptr.p_int[offscombined] = k-nmain;
                    state->combinedaslack.vals.ptr.p_double[offscombined] = v;
                    state->tmpi.ptr.p_int[k-nmain] = state->tmpi.ptr.p_int[k-nmain]+1;
                    offscombined = offscombined+1;
                }
            }
            state->sparseamain.ridx.ptr.p_int[i+1] = offsmain;
            state->combinedaslack.ridx.ptr.p_int[i+1] = offscombined;
        }
    }
    sparsecreatecrsinplace(&state->sparseamain, _state);
    if( mdense>0 )
    {
        rmatrixsetlengthatleast(&state->denseamain, mdense, nmain, _state);
        rmatrixcopy(mdense, nmain, &state->denseafull, 0, 0, &state->denseamain, 0, 0, _state);
        for(i=0; i<=mdense-1; i++)
        {
            offscombined = state->combinedaslack.ridx.ptr.p_int[msparse+i];
            for(k=nmain; k<=n-1; k++)
            {
                if( state->denseafull.ptr.pp_double[i][k]!=(double)0 )
                {
                    ae_assert(state->tmpi.ptr.p_int[k-nmain]==0, "VIPMSetConstraints: slack column contains more than one nonzero element", _state);
                    state->combinedaslack.idx.ptr.p_int[offscombined] = k-nmain;
                    state->combinedaslack.vals.ptr.p_double[offscombined] = state->denseafull.ptr.pp_double[i][k];
                    state->tmpi.ptr.p_int[k-nmain] = state->tmpi.ptr.p_int[k-nmain]+1;
                    offscombined = offscombined+1;
                }
            }
            state->combinedaslack.ridx.ptr.p_int[msparse+i+1] = offscombined;
        }
    }
    sparsecreatecrsinplace(&state->combinedaslack, _state);
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
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void vipmsetcond(vipmstate* state,
     double epsp,
     double epsd,
     double epsgap,
     ae_state *_state)
{
    double sml;


    ae_assert(ae_isfinite(epsp, _state)&&ae_fp_greater_eq(epsp,(double)(0)), "VIPMSetCond: EpsP is infinite or negative", _state);
    ae_assert(ae_isfinite(epsd, _state)&&ae_fp_greater_eq(epsd,(double)(0)), "VIPMSetCond: EpsD is infinite or negative", _state);
    ae_assert(ae_isfinite(epsgap, _state)&&ae_fp_greater_eq(epsgap,(double)(0)), "VIPMSetCond: EpsP is infinite or negative", _state);
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
void vipmoptimize(vipmstate* state,
     ae_bool dropbigbounds,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    ae_int_t iteridx;
    double mu;
    double muaff;
    double sigma;
    double alphaaffp;
    double alphaaffd;
    double alphap;
    double alphad;
    ae_int_t primalstagnationcnt;
    ae_int_t dualstagnationcnt;
    double regeps;
    double regeps0;
    double regeps1;
    double dampeps;
    double safedampeps;
    double modeps;
    double maxdampeps;
    double regfree;
    double dampfree;
    ae_int_t droppedbounds;
    double primalxscale;
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
    double mumin;
    double mustop;
    double y0nrm;
    double bady;
    double mxprimal;
    double mxdeltaprimal;
    ae_int_t bestiteridx;
    double besterr;
    double bestegap;
    double besteprimal;
    double bestedual;
    ae_bool loadbest;

    *terminationtype = 0;

    n = state->n;
    m = state->mdense+state->msparse;
    state->dotrace = ae_is_trace_enabled("IPM");
    state->dodetailedtrace = state->dotrace&&ae_is_trace_enabled("IPM.DETAILED");
    
    /*
     * Prepare outputs
     */
    rsetallocv(n, 0.0, xs, _state);
    rsetallocv(n, 0.0, lagbc, _state);
    rsetallocv(m, 0.0, laglc, _state);
    
    /*
     * Some integrity checks:
     * * we need PrimalStagnationLen<DualStagnationLen in order to be able to correctly
     *   detect infeasible instances (stagnated dual error is present in both infeasible
     *   and unbounded instances, so we should check for primal stagnation a few iters
     *   before checking for dual stagnation)
     */
    ae_assert(vipmsolver_primalstagnationlen<vipmsolver_dualstagnationlen, "VIPM: critical integrity failure - incorrect configuration parameters", _state);
    
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
     * * RegEps - one that is applied to initial (5N+5M)x(5N+5M) KKT system. This one has to be
     *   small because it perturbs solution returned by the algorithm. Essential in order to
     *   avoid stalling at extremely large points.
     * * ModEps - small modification applied to LDLT decomposition in order to preserve sign
     *   of diagonal elements
     * * DampEps - damping coefficient for damped Newton step. Comes along with SafeDampEps 
     *   (threshold value when some safeguards are turned off in order to preserve convergence
     *   speed) and MaxDampEps - threshold value when we consider problem overregularized and stop.
     * * DampFree - additional damping coefficient for free variables
     */
    regfree = ae_pow(ae_machineepsilon, 0.75, _state);
    dampfree = (double)(0);
    regeps0 = ae_sqrt(ae_machineepsilon, _state);
    regeps1 = (double)10*ae_machineepsilon;
    regeps = regeps0;
    modeps = ((double)100+ae_sqrt((double)(n), _state))*ae_machineepsilon;
    dampeps = (double)10*ae_machineepsilon;
    safedampeps = ae_sqrt(ae_machineepsilon, _state);
    maxdampeps = ae_sqrt(ae_sqrt(ae_machineepsilon, _state), _state);
    
    /*
     * Set up initial state
     */
    state->repiterationscount = 0;
    state->repncholesky = 0;
    mustop = ((double)100+ae_sqrt((double)(n), _state))*ae_machineepsilon;
    mumin = 0.01*mustop;
    vipmsolver_vipmpowerup(state, regfree, _state);
    vipmsolver_varsinitfrom(&state->best, &state->current, _state);
    vipmsolver_varsinitbyzero(&state->zerovars, n, m, _state);
    vipmsolver_varsinitbyzero(&state->deltaaff, n, m, _state);
    vipmsolver_varsinitbyzero(&state->deltacorr, n, m, _state);
    bestiteridx = -1;
    besterr = ae_maxrealnumber;
    bestegap = ae_maxrealnumber;
    besteprimal = ae_maxrealnumber;
    bestedual = ae_maxrealnumber;
    vipmsolver_traceprogress(state, 0.0, 0.0, 0.0, 0.0, 0.0, _state);
    y0nrm = (double)(0);
    y0nrm = ae_maxreal(y0nrm, rmaxabsv(m, &state->current.y, _state), _state);
    y0nrm = ae_maxreal(y0nrm, rmaxabsv(m, &state->current.v, _state), _state);
    y0nrm = ae_maxreal(y0nrm, rmaxabsv(m, &state->current.q, _state), _state);
    y0nrm = ae_maxreal(y0nrm, rmaxabsv(n, &state->current.z, _state), _state);
    y0nrm = ae_maxreal(y0nrm, rmaxabsv(n, &state->current.s, _state), _state);
    
    /*
     * Start iteration
     */
    loadbest = ae_true;
    primalstagnationcnt = 0;
    dualstagnationcnt = 0;
    *terminationtype = 7;
    errp2 = ae_maxrealnumber;
    errd2 = ae_maxrealnumber;
    for(iteridx=0; iteridx<=vipmsolver_maxipmits-1; iteridx++)
    {
        regeps = rcase2(iteridx<vipmsolver_phase0length, regeps0, regeps1, _state);
        
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
         *
         * NOTE: we use "solver" regularization coefficient at this moment
         */
        if( !vipmsolver_vipmprecomputenewtonfactorization(state, &state->current, regeps, modeps, dampeps, dampfree, _state) )
        {
            
            /*
             * KKT factorization failed.
             * Increase regularization parameter and skip this iteration.
             */
            dampeps = (double)10*dampeps;
            if( state->dotrace )
            {
                ae_trace("> LDLT factorization failed due to rounding errors\n");
                ae_trace("> increasing damping coefficient to %0.2e, skipping iteration\n",
                    (double)(dampeps));
            }
            continue;
        }
        
        /*
         * Compute Mu
         */
        mu = vipmsolver_varscomputemu(state, &state->current, _state);
        
        /*
         * Compute affine scaling step for Mehrotra's predictor-corrector algorithm
         */
        if( !vipmsolver_vipmcomputestepdirection(state, &state->current, 0.0, &state->zerovars, &state->deltaaff, regeps, ae_fp_greater_eq(dampeps,safedampeps), _state) )
        {
            
            /*
             * Affine scaling step failed due to numerical errors.
             * Increase regularization parameter and skip this iteration.
             */
            dampeps = (double)10*dampeps;
            if( state->dotrace )
            {
                ae_trace("> affine scaling step failed to decrease residual due to rounding errors\n");
                ae_trace("> increasing damping coefficient to %0.2e, skipping iteration\n",
                    (double)(dampeps));
            }
            continue;
        }
        vipmsolver_vipmcomputesteplength(state, &state->current, &state->deltaaff, vipmsolver_steplengthdecay, ae_false, &alphaaffp, &alphaaffd, _state);
        
        /*
         * Compute MuAff and centering parameter
         */
        vipmsolver_varsinitfrom(&state->trial, &state->current, _state);
        vipmsolver_varsaddstep(&state->trial, &state->deltaaff, alphaaffp, alphaaffd, _state);
        muaff = vipmsolver_varscomputemu(state, &state->trial, _state);
        sigma = ae_minreal(ae_pow((muaff+mumin)/(mu+mumin), (double)(3), _state), 1.0, _state);
        ae_assert(ae_isfinite(sigma, _state)&&ae_fp_less_eq(sigma,(double)(1)), "VIPMOptimize: critical integrity check failed for Sigma (infinite or greater than 1)", _state);
        
        /*
         * Compute corrector step
         */
        if( !vipmsolver_vipmcomputestepdirection(state, &state->current, sigma*mu+mumin, &state->deltaaff, &state->deltacorr, regeps, ae_fp_greater_eq(dampeps,safedampeps), _state) )
        {
            
            /*
             * Affine scaling step failed due to numerical errors.
             * Increase regularization parameter and skip this iteration.
             */
            dampeps = (double)10*dampeps;
            if( state->dotrace )
            {
                ae_trace("> corrector step failed to decrease residual due to rounding errors\n");
                ae_trace("> increasing damping coefficient to %0.2e, skipping iteration\n",
                    (double)(dampeps));
            }
            continue;
        }
        vipmsolver_vipmcomputesteplength(state, &state->current, &state->deltacorr, vipmsolver_steplengthdecay, ae_false, &alphap, &alphad, _state);
        if( ae_fp_greater_eq((double)(iteridx),vipmsolver_minitersbeforesafeguards)&&(ae_fp_less_eq(alphap,vipmsolver_badsteplength)||ae_fp_less_eq(alphad,vipmsolver_badsteplength)) )
        {
            
            /*
             * Affine scaling step failed due to numerical errors.
             * Increase regularization parameter and skip this iteration.
             */
            dampeps = (double)10*dampeps;
            if( state->dotrace )
            {
                ae_trace("> step length is too short, suspecting rounding errors\n");
                ae_trace("> increasing damping coefficient to %0.2e, skipping iteration\n",
                    (double)(dampeps));
            }
            continue;
        }
        
        /*
         * Perform a step
         */
        vipmsolver_runintegritychecks(state, &state->current, &state->deltacorr, alphap, alphad, _state);
        vipmsolver_vipmperformstep(state, alphap, alphad, _state);
        vipmsolver_traceprogress(state, mu, muaff, sigma, alphap, alphad, _state);
        
        /*
         * Check for excessive bounds (one that are so large that they are both irrelevant
         * and destabilizing due to their magnitude)
         */
        if( dropbigbounds&&iteridx>=vipmsolver_minitersbeforedroppingbounds )
        {
            ae_assert(ae_fp_less_eq((double)10*vipmsolver_bigconstrmag,(double)1/vipmsolver_bigconstrxtol), "VIPMOptimize: integrity check failed (incorrect BigConstr settings)", _state);
            droppedbounds = 0;
            
            /*
             * Determine variable and step scales.
             * Both quantities are bounded from below by 1.0
             */
            mxprimal = 1.0;
            mxprimal = ae_maxreal(mxprimal, rmaxabsv(n, &state->current.x, _state), _state);
            mxprimal = ae_maxreal(mxprimal, rmaxabsv(n, &state->current.g, _state), _state);
            mxprimal = ae_maxreal(mxprimal, rmaxabsv(n, &state->current.t, _state), _state);
            mxprimal = ae_maxreal(mxprimal, rmaxabsv(m, &state->current.w, _state), _state);
            mxprimal = ae_maxreal(mxprimal, rmaxabsv(m, &state->current.p, _state), _state);
            mxdeltaprimal = 1.0;
            mxdeltaprimal = ae_maxreal(mxdeltaprimal, alphap*rmaxabsv(n, &state->deltacorr.x, _state), _state);
            mxdeltaprimal = ae_maxreal(mxdeltaprimal, alphap*rmaxabsv(n, &state->deltacorr.g, _state), _state);
            mxdeltaprimal = ae_maxreal(mxdeltaprimal, alphap*rmaxabsv(n, &state->deltacorr.t, _state), _state);
            mxdeltaprimal = ae_maxreal(mxdeltaprimal, alphap*rmaxabsv(m, &state->deltacorr.w, _state), _state);
            mxdeltaprimal = ae_maxreal(mxdeltaprimal, alphap*rmaxabsv(m, &state->deltacorr.p, _state), _state);
            
            /*
             * If changes in primal variables are small enough, try dropping too large bounds
             */
            if( ae_fp_less(mxdeltaprimal,mxprimal*vipmsolver_bigconstrxtol) )
            {
                
                /*
                 * Drop irrelevant box constraints
                 */
                primalxscale = 1.0;
                primalxscale = ae_maxreal(primalxscale, rmaxabsv(n, &state->current.x, _state), _state);
                for(i=0; i<=n-1; i++)
                {
                    if( (state->hasbndl.ptr.p_bool[i]&&state->hasgz.ptr.p_bool[i])&&ae_fp_greater(ae_fabs(state->bndl.ptr.p_double[i], _state),vipmsolver_bigconstrmag*primalxscale) )
                    {
                        state->hasgz.ptr.p_bool[i] = ae_false;
                        state->current.g.ptr.p_double[i] = (double)(0);
                        state->current.z.ptr.p_double[i] = (double)(0);
                        state->cntgz = state->cntgz-1;
                        inc(&droppedbounds, _state);
                    }
                    if( (state->hasbndu.ptr.p_bool[i]&&state->hasts.ptr.p_bool[i])&&ae_fp_greater(ae_fabs(state->bndu.ptr.p_double[i], _state),vipmsolver_bigconstrmag*primalxscale) )
                    {
                        state->hasts.ptr.p_bool[i] = ae_false;
                        state->current.t.ptr.p_double[i] = (double)(0);
                        state->current.s.ptr.p_double[i] = (double)(0);
                        state->cntts = state->cntts-1;
                        inc(&droppedbounds, _state);
                    }
                }
                
                /*
                 * Drop irrelevant linear constraints. Due to specifics of the solver
                 * we can drop only right part part of b<=Ax<=b+r.
                 *
                 * We can't drop b<=A from b<=A<=b+r because it impossible with our choice of
                 * slack variables. Usually we do not need to do so because we reorder constraints
                 * during initialization in such a way that |b+r|>|b| and because typical
                 * applications do not have excessively large lower AND upper bound (user may
                 * specify large value for 'absent' bound, but usually he does not mark both bounds as absent).
                 */
                vipmsolver_multiplygeax(state, 1.0, &state->current.x, 0, 0.0, &state->tmpax, 0, _state);
                primalxscale = 1.0;
                primalxscale = ae_maxreal(primalxscale, rmaxabsv(n, &state->current.x, _state), _state);
                primalxscale = ae_maxreal(primalxscale, rmaxabsv(m, &state->tmpax, _state), _state);
                for(i=0; i<=m-1; i++)
                {
                    if( ((state->hasr.ptr.p_bool[i]&&state->haspq.ptr.p_bool[i])&&ae_fp_greater(ae_fabs(state->b.ptr.p_double[i]+state->r.ptr.p_double[i], _state),vipmsolver_bigconstrmag*primalxscale))&&ae_fp_less(ae_fabs(state->b.ptr.p_double[i], _state),vipmsolver_bigconstrmag*primalxscale) )
                    {
                        ae_assert(state->haswv.ptr.p_bool[i]&&state->haspq.ptr.p_bool[i], "VIPMOptimize: unexpected integrity check failure (4y64)", _state);
                        state->haspq.ptr.p_bool[i] = ae_false;
                        state->current.p.ptr.p_double[i] = (double)(0);
                        state->current.q.ptr.p_double[i] = (double)(0);
                        state->cntpq = state->cntpq-1;
                        inc(&droppedbounds, _state);
                    }
                }
                
                /*
                 * Trace output
                 */
                if( droppedbounds>0 )
                {
                    if( state->dotrace )
                    {
                        ae_trace("[NOTICE] detected %0d irrelevant constraints with huge bounds, X converged to values well below them, dropping...\n",
                            (int)(droppedbounds));
                    }
                }
            }
        }
        
        /*
         * Check stopping criteria
         * * primal and dual stagnation are checked only when following criteria are met:
         *   1) Mu is smaller than 1 (we already converged close enough)
         *   2) we performed more than MinItersBeforeStagnation iterations
         */
        preverrp2 = errp2;
        preverrd2 = errd2;
        vipmsolver_computeerrors(state, &errp2, &errd2, &errpinf, &errdinf, &errgap, _state);
        mu = vipmsolver_varscomputemu(state, &state->current, _state);
        egap = errgap;
        eprimal = errpinf;
        edual = errdinf;
        if( ae_fp_less(rmax3(egap, eprimal, edual, _state),besterr) )
        {
            
            /*
             * Save best point found so far
             */
            vipmsolver_varsinitfrom(&state->best, &state->current, _state);
            bestiteridx = iteridx;
            besterr = rmax3(egap, eprimal, edual, _state);
            bestegap = egap;
            besteprimal = eprimal;
            bestedual = edual;
        }
        if( bestiteridx>0&&iteridx>bestiteridx+vipmsolver_minitersbeforeeworststagnation )
        {
            if( state->dotrace )
            {
                ae_trace("> worst of primal/dual/gap errors stagnated for %0d its, stopping at the best point found so far\n",
                    (int)(vipmsolver_minitersbeforeeworststagnation));
            }
            break;
        }
        if( ((ae_fp_less_eq(egap,state->epsgap)&&ae_fp_greater_eq(errp2,vipmsolver_stagnationdelta*preverrp2))&&ae_fp_greater_eq(errpinf,vipmsolver_primalinfeasible1))&&iteridx>=vipmsolver_minitersbeforestagnation )
        {
            inc(&primalstagnationcnt, _state);
            if( primalstagnationcnt>=vipmsolver_primalstagnationlen )
            {
                if( state->dotrace )
                {
                    ae_trace("> primal error stagnated for %0d its, stopping at the best point found so far\n",
                        (int)(vipmsolver_primalstagnationlen));
                }
                break;
            }
        }
        else
        {
            primalstagnationcnt = 0;
        }
        if( ((ae_fp_less_eq(egap,state->epsgap)&&ae_fp_greater_eq(errd2,vipmsolver_stagnationdelta*preverrd2))&&ae_fp_greater_eq(errdinf,vipmsolver_dualinfeasible1))&&iteridx>=vipmsolver_minitersbeforestagnation )
        {
            inc(&dualstagnationcnt, _state);
            if( dualstagnationcnt>=vipmsolver_dualstagnationlen )
            {
                if( state->dotrace )
                {
                    ae_trace("> dual error stagnated for %0d its, stopping at the best point found so far\n",
                        (int)(vipmsolver_dualstagnationlen));
                }
                break;
            }
        }
        else
        {
            dualstagnationcnt = 0;
        }
        if( ae_fp_less_eq(mu,mustop)&&iteridx>=vipmsolver_itersfortoostringentcond )
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
        bady = vipmsolver_bigy;
        bady = ae_maxreal(bady, vipmsolver_ygrowth*y0nrm, _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(n, &state->current.x, _state), _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(n, &state->current.g, _state), _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(n, &state->current.t, _state), _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(m, &state->current.w, _state), _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(m, &state->current.p, _state), _state);
        if( ae_fp_greater_eq(rmaxabsv(m, &state->current.y, _state),bady)&&iteridx>=vipmsolver_minitersbeforeinfeasible )
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
        vipmsolver_varsinitfrom(&state->current, &state->best, _state);
        touchreal(&besteprimal, _state);
        touchreal(&bestedual, _state);
        touchreal(&bestegap, _state);
        
        /*
         * If no error flags were set yet, check solution quality
         */
        bady = vipmsolver_bigy;
        bady = ae_maxreal(bady, vipmsolver_ygrowth*y0nrm, _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(n, &state->current.x, _state), _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(n, &state->current.g, _state), _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(n, &state->current.t, _state), _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(m, &state->current.w, _state), _state);
        bady = ae_maxreal(bady, vipmsolver_ygrowth*rmaxabsv(m, &state->current.p, _state), _state);
        if( *terminationtype>0&&ae_fp_greater_eq(rmaxabsv(m, &state->current.y, _state),bady) )
        {
            *terminationtype = -2;
            if( state->dotrace )
            {
                ae_trace("> |Y| increased beyond %0.1e, declaring infeasibility/unboundedness\n",
                    (double)(bady));
            }
        }
        if( *terminationtype>0&&ae_fp_greater_eq(besteprimal,vipmsolver_primalinfeasible1) )
        {
            *terminationtype = -2;
            if( state->dotrace )
            {
                ae_trace("> primal error at the best point is too high, declaring infeasibility/unboundedness\n");
            }
        }
        if( *terminationtype>0&&ae_fp_greater_eq(bestedual,vipmsolver_dualinfeasible1) )
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
    vipmsolver_multiplyhx(state, &state->current.x, &state->tmp0, _state);
    raddv(n, 1.0, &state->c, &state->tmp0, _state);
    vipmsolver_multiplygeatx(state, -1.0, &state->current.y, 0, 1.0, &state->tmp0, 0, _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->isfrozen.ptr.p_bool[i] )
        {
            
            /*
             * I-th variable is frozen, use its frozen value.
             * By the definition, I-th Lagrangian multiplier is an I-th component of Lagrangian gradient
             */
            xs->ptr.p_double[i] = state->current.x.ptr.p_double[i];
            lagbc->ptr.p_double[i] = -state->tmp0.ptr.p_double[i];
        }
        else
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
    }
    for(i=0; i<=m-1; i++)
    {
        laglc->ptr.p_double[i] = -state->current.y.ptr.p_double[i];
    }
    
    /*
     * Unscale point and Lagrange multipliers
     */
    unscaleunshiftpointbc(&state->scl, &state->xorigin, &state->rawbndl, &state->rawbndu, &state->bndl, &state->bndu, &state->hasbndl, &state->hasbndu, xs, n, _state);
    for(i=0; i<=n-1; i++)
    {
        lagbc->ptr.p_double[i] = lagbc->ptr.p_double[i]*state->targetscale/state->scl.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        laglc->ptr.p_double[i] = laglc->ptr.p_double[i]*state->targetscale/coalesce(state->ascales.ptr.p_double[i], 1.0, _state);
    }
}


/*************************************************************************
Allocates place for variables of IPM and fills by zeros.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_varsinitbyzero(vipmvars* vstate,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state)
{


    ae_assert(n>=1, "VarsInitByZero: N<1", _state);
    ae_assert(m>=0, "VarsInitByZero: M<0", _state);
    vstate->n = n;
    vstate->m = m;
    rsetallocv(n, 0.0, &vstate->x, _state);
    rsetallocv(n, 0.0, &vstate->g, _state);
    rsetallocv(n, 0.0, &vstate->t, _state);
    rsetallocv(n, 0.0, &vstate->z, _state);
    rsetallocv(n, 0.0, &vstate->s, _state);
    rsetallocv(m, 0.0, &vstate->y, _state);
    rsetallocv(m, 0.0, &vstate->w, _state);
    rsetallocv(m, 0.0, &vstate->p, _state);
    rsetallocv(m, 0.0, &vstate->v, _state);
    rsetallocv(m, 0.0, &vstate->q, _state);
}


/*************************************************************************
Allocates place for variables of IPM and fills them by values of
the source

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_varsinitfrom(vipmvars* vstate,
     const vipmvars* vsrc,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;


    n = vsrc->n;
    m = vsrc->m;
    ae_assert(n>=1, "VarsInitFrom: N<1", _state);
    ae_assert(m>=0, "VarsInitFrom: M<0", _state);
    vstate->n = n;
    vstate->m = m;
    rcopyallocv(n, &vsrc->x, &vstate->x, _state);
    rcopyallocv(n, &vsrc->g, &vstate->g, _state);
    rcopyallocv(n, &vsrc->t, &vstate->t, _state);
    rcopyallocv(n, &vsrc->z, &vstate->z, _state);
    rcopyallocv(n, &vsrc->s, &vstate->s, _state);
    rcopyallocv(m, &vsrc->y, &vstate->y, _state);
    rcopyallocv(m, &vsrc->w, &vstate->w, _state);
    rcopyallocv(m, &vsrc->p, &vstate->p, _state);
    rcopyallocv(m, &vsrc->v, &vstate->v, _state);
    rcopyallocv(m, &vsrc->q, &vstate->q, _state);
}


/*************************************************************************
Adds to variables direction vector times step length. Different
lengths are used for primal and dual steps.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_varsaddstep(vipmvars* vstate,
     const vipmvars* vdir,
     double stpp,
     double stpd,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t m;


    n = vstate->n;
    m = vstate->m;
    ae_assert(n>=1, "VarsAddStep: N<1", _state);
    ae_assert(m>=0, "VarsAddStep: M<0", _state);
    ae_assert(n==vdir->n, "VarsAddStep: sizes mismatch", _state);
    ae_assert(m==vdir->m, "VarsAddStep: sizes mismatch", _state);
    for(i=0; i<=n-1; i++)
    {
        vstate->x.ptr.p_double[i] = vstate->x.ptr.p_double[i]+stpp*vdir->x.ptr.p_double[i];
        vstate->g.ptr.p_double[i] = vstate->g.ptr.p_double[i]+stpp*vdir->g.ptr.p_double[i];
        vstate->t.ptr.p_double[i] = vstate->t.ptr.p_double[i]+stpp*vdir->t.ptr.p_double[i];
        vstate->z.ptr.p_double[i] = vstate->z.ptr.p_double[i]+stpd*vdir->z.ptr.p_double[i];
        vstate->s.ptr.p_double[i] = vstate->s.ptr.p_double[i]+stpd*vdir->s.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        vstate->w.ptr.p_double[i] = vstate->w.ptr.p_double[i]+stpp*vdir->w.ptr.p_double[i];
        vstate->p.ptr.p_double[i] = vstate->p.ptr.p_double[i]+stpp*vdir->p.ptr.p_double[i];
        vstate->y.ptr.p_double[i] = vstate->y.ptr.p_double[i]+stpd*vdir->y.ptr.p_double[i];
        vstate->v.ptr.p_double[i] = vstate->v.ptr.p_double[i]+stpd*vdir->v.ptr.p_double[i];
        vstate->q.ptr.p_double[i] = vstate->q.ptr.p_double[i]+stpd*vdir->q.ptr.p_double[i];
    }
}


/*************************************************************************
Computes complementarity gap

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static double vipmsolver_varscomputecomplementaritygap(const vipmvars* vstate,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;
    ae_int_t m;
    double result;


    n = vstate->n;
    m = vstate->m;
    result = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        result = result+vstate->z.ptr.p_double[i]*vstate->g.ptr.p_double[i]+vstate->s.ptr.p_double[i]*vstate->t.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        result = result+vstate->v.ptr.p_double[i]*vstate->w.ptr.p_double[i]+vstate->p.ptr.p_double[i]*vstate->q.ptr.p_double[i];
    }
    return result;
}


/*************************************************************************
Computes empirical value of the barrier parameter Mu

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static double vipmsolver_varscomputemu(const vipmstate* state,
     const vipmvars* vstate,
     ae_state *_state)
{
    double result;


    result = (double)(0);
    result = result+rdotv(vstate->n, &vstate->z, &vstate->g, _state)+rdotv(vstate->n, &vstate->s, &vstate->t, _state);
    result = result+rdotv(vstate->m, &vstate->v, &vstate->w, _state)+rdotv(vstate->m, &vstate->p, &vstate->q, _state);
    result = result/coalesce((double)(state->cntgz+state->cntts+state->cntwv+state->cntpq), (double)(1), _state);
    return result;
}


/*************************************************************************
Initializes reduced sparse system.

Works only for sparse IPM.

  -- ALGLIB --
     Copyright 15.11.2021 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_reducedsysteminit(vipmreducedsparsesystem* s,
     const vipmstate* solver,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t nnzmax;
    ae_int_t factldlt;
    ae_int_t permpriorityamd;
    ae_int_t memreuse;
    ae_int_t offs;
    ae_int_t rowoffs;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t sumcoldeg;
    ae_int_t sumrowdeg;
    ae_int_t colthreshold;
    ae_int_t rowthreshold;
    ae_int_t eligiblecols;
    ae_int_t eligiblerows;


    ae_assert(solver->factorizationtype==1, "ReducedSystemInit: unexpected factorization type", _state);
    ae_assert(solver->hkind==1, "ReducedSystemInit: unexpected HKind", _state);
    ntotal = solver->n+solver->mdense+solver->msparse;
    s->ntotal = ntotal;
    rallocv(ntotal, &s->effectivediag, _state);
    
    /*
     * Determine maximum amount of memory required to store sparse matrices
     */
    nnzmax = solver->sparseh.ridx.ptr.p_int[solver->n];
    if( solver->msparse>0 )
    {
        nnzmax = nnzmax+solver->sparseafull.ridx.ptr.p_int[solver->msparse];
    }
    if( solver->mdense>0 )
    {
        nnzmax = nnzmax+solver->n*solver->mdense;
    }
    nnzmax = nnzmax+ntotal;
    
    /*
     * Default DIAG and DAMP terms
     */
    rsetallocv(ntotal, 0.0, &s->diagterm, _state);
    rsetallocv(ntotal, 0.0, &s->dampterm, _state);
    
    /*
     * Prepare lower triangle of template KKT matrix (KKT system without D and E
     * terms being added to diagonals)
     */
    s->rawsystem.m = ntotal;
    s->rawsystem.n = ntotal;
    ivectorsetlengthatleast(&s->rawsystem.idx, nnzmax, _state);
    rvectorsetlengthatleast(&s->rawsystem.vals, nnzmax, _state);
    ivectorsetlengthatleast(&s->rawsystem.ridx, ntotal+1, _state);
    s->rawsystem.ridx.ptr.p_int[0] = 0;
    offs = 0;
    rowoffs = 0;
    sumcoldeg = 0;
    sumrowdeg = 0;
    isetallocv(solver->n, 0, &s->coldegrees, _state);
    isetallocv(solver->msparse+solver->mdense, 0, &s->rowdegrees, _state);
    bsetallocv(solver->n, ae_true, &s->isdiagonal, _state);
    for(i=0; i<=solver->n-1; i++)
    {
        ae_assert(solver->sparseh.didx.ptr.p_int[i]+1==solver->sparseh.uidx.ptr.p_int[i], "ReducedSystemInit: critical integrity check failed for diagonal of H", _state);
        if( !solver->isfrozen.ptr.p_bool[i] )
        {
            
            /*
             * Entire row is not frozen, but some of its entries can be.
             * Output non-frozen offdiagonal entries.
             */
            k0 = solver->sparseh.ridx.ptr.p_int[i];
            k1 = solver->sparseh.didx.ptr.p_int[i]-1;
            for(k=k0; k<=k1; k++)
            {
                j = solver->sparseh.idx.ptr.p_int[k];
                if( !solver->isfrozen.ptr.p_bool[j] )
                {
                    s->rawsystem.idx.ptr.p_int[offs] = j;
                    s->rawsystem.vals.ptr.p_double[offs] = -solver->sparseh.vals.ptr.p_double[k];
                    s->isdiagonal.ptr.p_bool[i] = ae_false;
                    s->isdiagonal.ptr.p_bool[j] = ae_false;
                    s->coldegrees.ptr.p_int[i] = s->coldegrees.ptr.p_int[i]+1;
                    s->coldegrees.ptr.p_int[j] = s->coldegrees.ptr.p_int[j]+1;
                    sumcoldeg = sumcoldeg+2;
                    offs = offs+1;
                }
            }
            
            /*
             * Output diagonal entry (it is always not frozen)
             */
            s->rawsystem.idx.ptr.p_int[offs] = i;
            s->rawsystem.vals.ptr.p_double[offs] = -solver->sparseh.vals.ptr.p_double[solver->sparseh.didx.ptr.p_int[i]];
            offs = offs+1;
        }
        else
        {
            
            /*
             * Entire row is frozen, output just -1
             */
            s->rawsystem.idx.ptr.p_int[offs] = i;
            s->rawsystem.vals.ptr.p_double[offs] = -1.0;
            offs = offs+1;
        }
        rowoffs = rowoffs+1;
        s->rawsystem.ridx.ptr.p_int[rowoffs] = offs;
    }
    for(i=0; i<=solver->msparse-1; i++)
    {
        k0 = solver->sparseafull.ridx.ptr.p_int[i];
        k1 = solver->sparseafull.ridx.ptr.p_int[i+1]-1;
        for(k=k0; k<=k1; k++)
        {
            j = solver->sparseafull.idx.ptr.p_int[k];
            if( !solver->isfrozen.ptr.p_bool[j] )
            {
                s->rawsystem.idx.ptr.p_int[offs] = j;
                s->rawsystem.vals.ptr.p_double[offs] = solver->sparseafull.vals.ptr.p_double[k];
                s->rowdegrees.ptr.p_int[i] = s->rowdegrees.ptr.p_int[i]+1;
                s->coldegrees.ptr.p_int[j] = s->coldegrees.ptr.p_int[j]+1;
                sumcoldeg = sumcoldeg+1;
                sumrowdeg = sumrowdeg+1;
                offs = offs+1;
            }
        }
        s->rawsystem.idx.ptr.p_int[offs] = rowoffs;
        s->rawsystem.vals.ptr.p_double[offs] = 0.0;
        offs = offs+1;
        rowoffs = rowoffs+1;
        s->rawsystem.ridx.ptr.p_int[rowoffs] = offs;
    }
    for(i=0; i<=solver->mdense-1; i++)
    {
        for(k=0; k<=solver->n-1; k++)
        {
            if( solver->denseafull.ptr.pp_double[i][k]!=0.0&&!solver->isfrozen.ptr.p_bool[k] )
            {
                s->rawsystem.idx.ptr.p_int[offs] = k;
                s->rawsystem.vals.ptr.p_double[offs] = solver->denseafull.ptr.pp_double[i][k];
                s->rowdegrees.ptr.p_int[solver->msparse+i] = s->rowdegrees.ptr.p_int[solver->msparse+i]+1;
                s->coldegrees.ptr.p_int[k] = s->coldegrees.ptr.p_int[k]+1;
                sumcoldeg = sumcoldeg+1;
                sumrowdeg = sumrowdeg+1;
                offs = offs+1;
            }
        }
        s->rawsystem.idx.ptr.p_int[offs] = rowoffs;
        s->rawsystem.vals.ptr.p_double[offs] = 0.0;
        offs = offs+1;
        rowoffs = rowoffs+1;
        s->rawsystem.ridx.ptr.p_int[rowoffs] = offs;
    }
    ae_assert(rowoffs==ntotal, "ReducedSystemInit: critical integrity check failed", _state);
    ae_assert(offs<=nnzmax, "ReducedSystemInit: critical integrity check failed", _state);
    sparsecreatecrsinplace(&s->rawsystem, _state);
    
    /*
     * Prepare reordering
     */
    isetallocv(ntotal, 0, &s->priorities, _state);
    colthreshold = ae_round(vipmsolver_muquasidense*(double)sumcoldeg/(double)solver->n, _state)+1;
    rowthreshold = ae_round(vipmsolver_muquasidense*(double)sumrowdeg/(double)(solver->msparse+solver->mdense+1), _state)+1;
    eligiblecols = 0;
    eligiblerows = 0;
    for(i=0; i<=solver->n-1; i++)
    {
        if( s->coldegrees.ptr.p_int[i]<=colthreshold )
        {
            eligiblecols = eligiblecols+1;
        }
    }
    for(i=0; i<=solver->mdense+solver->msparse-1; i++)
    {
        if( s->rowdegrees.ptr.p_int[i]<=rowthreshold )
        {
            eligiblerows = eligiblerows+1;
        }
    }
    if( ae_maxint(eligiblecols, eligiblerows, _state)>0&&ae_maxint(eligiblecols, eligiblerows, _state)<ntotal )
    {
        if( eligiblecols>=eligiblerows )
        {
            
            /*
             * Eliminate columns first
             */
            for(i=0; i<=solver->n-1; i++)
            {
                s->priorities.ptr.p_int[i] = icase2(s->coldegrees.ptr.p_int[i]<=colthreshold, 0, 2, _state);
            }
            for(i=solver->n; i<=ntotal-1; i++)
            {
                s->priorities.ptr.p_int[i] = icase2(s->rowdegrees.ptr.p_int[i-solver->n]<=rowthreshold, 1, 2, _state);
            }
        }
        else
        {
            
            /*
             * Eliminate rows first
             */
            for(i=0; i<=solver->n-1; i++)
            {
                s->priorities.ptr.p_int[i] = icase2(s->coldegrees.ptr.p_int[i]<=colthreshold, 1, 2, _state);
            }
            for(i=solver->n; i<=ntotal-1; i++)
            {
                s->priorities.ptr.p_int[i] = icase2(s->rowdegrees.ptr.p_int[i-solver->n]<=rowthreshold, 0, 2, _state);
            }
        }
    }
    for(i=0; i<=solver->n-1; i++)
    {
        s->priorities.ptr.p_int[i] = icase2(solver->hasbndl.ptr.p_bool[i]||solver->hasbndu.ptr.p_bool[i], s->priorities.ptr.p_int[i], 2, _state);
    }
    if( solver->dotrace )
    {
        ae_trace("> initializing KKT system; no priority ordering being applied\n");
    }
    
    /*
     * Perform factorization analysis using sparsity pattern (but not numerical values)
     */
    factldlt = 1;
    permpriorityamd = 3;
    memreuse = 1;
    if( !spsymmanalyze(&s->rawsystem, &s->priorities, (double)ntotal+1.0, 0, factldlt, permpriorityamd, memreuse, &s->analysis, _state) )
    {
        ae_assert(ae_false, "ReducedSystemInit: critical integrity check failed, symbolically degenerate KKT system encountered", _state);
    }
}


/*************************************************************************
Computes factorization of A+Diag+Damp, where A is an internally stored KKT
matrix and Diag and Damp are user-supplied diagonal terms.

The Diag term is assumed to be a "true" modification of the system, and
Damp is assumed to be a small damping factor. The difference is that the
damping factor is added during the factorization, but not accounted for
during the iterative refinement stage, i.e. we factor A+Diag+Damp, but aim
to solve A+Diag.

ModEps and BadChol are user supplied tolerances for modified Cholesky/LDLT.

Returns True on success, False on LDLT failure.

On success outputs diagonal reproduction error ErrSq, and sum of squared
diagonal elements SumSq

  -- ALGLIB --
     Copyright 15.11.2021 by Bochkanov Sergey
*************************************************************************/
static ae_bool vipmsolver_reducedsystemfactorizewithaddends(vipmreducedsparsesystem* s,
     /* Real    */ const ae_vector* diag,
     /* Real    */ const ae_vector* damp,
     double modeps,
     double badchol,
     double* sumsq,
     double* errsq,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t i;
    ae_bool result;

    *sumsq = 0.0;
    *errsq = 0.0;

    ntotal = s->ntotal;
    rcopyv(ntotal, diag, &s->diagterm, _state);
    rcopyv(ntotal, damp, &s->dampterm, _state);
    for(i=0; i<=ntotal-1; i++)
    {
        s->effectivediag.ptr.p_double[i] = s->rawsystem.vals.ptr.p_double[s->rawsystem.didx.ptr.p_int[i]]+diag->ptr.p_double[i]+damp->ptr.p_double[i];
    }
    spsymmreloaddiagonal(&s->analysis, &s->effectivediag, _state);
    result = ae_true;
    spsymmsetmodificationstrategy(&s->analysis, 1, modeps, badchol, 0.0, 0.0, _state);
    if( spsymmfactorize(&s->analysis, _state) )
    {
        
        /*
         * Compute diagonal reproduction error
         */
        spsymmdiagerr(&s->analysis, sumsq, errsq, _state);
    }
    else
    {
        *sumsq = (double)(0);
        *errsq = (double)(0);
        result = ae_false;
    }
    return result;
}


/*************************************************************************
Solve reduced KKT system, replacing right part by its solution.

  -- ALGLIB --
     Copyright 15.11.2021 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_reducedsystemsolve(vipmreducedsparsesystem* s,
     ae_bool dotrace,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t iteridx;
    double bnrm2;
    double relerr;
    double prevrelerr;


    
    /*
     * Perform initial solution
     */
    rcopyallocv(s->ntotal, b, &s->tmpb, _state);
    spsymmsolve(&s->analysis, b, _state);
    
    /*
     * Trace residual and relative error
     */
    bnrm2 = ae_maxreal(rdotv2(s->ntotal, &s->tmpb, _state), 1.0, _state);
    sparsesmv(&s->rawsystem, ae_false, b, &s->tmprhs, _state);
    rmuladdv(s->ntotal, b, &s->diagterm, &s->tmprhs, _state);
    rmulv(s->ntotal, -1.0, &s->tmprhs, _state);
    raddv(s->ntotal, 1.0, &s->tmpb, &s->tmprhs, _state);
    relerr = ae_sqrt(rdotv2(s->ntotal, &s->tmprhs, _state)/bnrm2, _state);
    if( dotrace )
    {
        ae_trace("> reduced system solved, res/rhs = %0.3e (initial)\n",
            (double)(relerr));
    }
    
    /*
     * Perform iterative refinement, if necessary
     */
    prevrelerr = 1.0E50;
    iteridx = 0;
    while((iteridx<vipmsolver_maxrefinementits&&ae_fp_greater(relerr,(double)10*ae_machineepsilon))&&ae_fp_less(relerr,0.5*prevrelerr))
    {
        
        /*
         * Compute correction and update solution
         */
        rcopyallocv(s->ntotal, &s->tmprhs, &s->tmpcorr, _state);
        spsymmsolve(&s->analysis, &s->tmpcorr, _state);
        raddv(s->ntotal, 1.0, &s->tmpcorr, b, _state);
        
        /*
         * Recompute residual
         */
        sparsesmv(&s->rawsystem, ae_false, b, &s->tmprhs, _state);
        rmuladdv(s->ntotal, b, &s->diagterm, &s->tmprhs, _state);
        rmulv(s->ntotal, -1.0, &s->tmprhs, _state);
        raddv(s->ntotal, 1.0, &s->tmpb, &s->tmprhs, _state);
        prevrelerr = relerr;
        relerr = ae_sqrt(rdotv2(s->ntotal, &s->tmprhs, _state)/bnrm2, _state);
        iteridx = iteridx+1;
    }
    if( dotrace&&iteridx>0 )
    {
        ae_trace("> reduced system solved, res/rhs = %0.3e (refined, %0d its)\n",
            (double)(relerr),
            (int)(iteridx));
    }
}


/*************************************************************************
Initializes QP-IPM state and prepares it to receive quadratic/linear terms
and constraints.

The solver is configured to work internally with factorization FType

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
    FType       -   factorization type:
                    * 0 for dense NxN factorization (normal equations)
                    * 1 for sparse (N+M)x(N+M) factorization

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_vipminit(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_int_t nmain,
     ae_int_t ftype,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t nslack;


    ae_assert(n>=1, "VIPMInit: N<1", _state);
    ae_assert(isfinitevector(s, n, _state), "VIPMInit: S contains infinite or NaN elements", _state);
    ae_assert(isfinitevector(xorigin, n, _state), "VIPMInit: XOrigin contains infinite or NaN elements", _state);
    ae_assert(ftype==0||ftype==1, "VIPMInit: unexpected FType", _state);
    ae_assert(nmain>=1, "VIPMInit: NMain<1", _state);
    ae_assert(nmain<=n, "VIPMInit: NMain>N", _state);
    nslack = n-nmain;
    
    /*
     * Problem metrics, settings and type
     */
    state->n = n;
    state->nmain = nmain;
    state->islinear = ae_true;
    state->factorizationtype = ftype;
    state->factorizationpresent = ae_false;
    state->factorizationpoweredup = ae_false;
    vipmsetcond(state, 0.0, 0.0, 0.0, _state);
    state->slacksforequalityconstraints = ae_true;
    
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
    rvectorsetlengthatleast(&state->scl, n, _state);
    rvectorsetlengthatleast(&state->invscl, n, _state);
    rvectorsetlengthatleast(&state->xorigin, n, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(s->ptr.p_double[i]>0.0, "VIPMInit: S[i] is non-positive", _state);
        state->scl.ptr.p_double[i] = s->ptr.p_double[i];
        state->invscl.ptr.p_double[i] = (double)1/s->ptr.p_double[i];
        state->xorigin.ptr.p_double[i] = xorigin->ptr.p_double[i];
    }
    state->targetscale = 1.0;
    
    /*
     * Linear and quadratic terms - default value
     */
    rvectorsetlengthatleast(&state->c, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->c.ptr.p_double[i] = (double)(0);
    }
    state->hkind = -1;
    if( ftype==0 )
    {
        
        /*
         * Dense quadratic term
         */
        rmatrixsetlengthatleast(&state->denseh, nmain, nmain, _state);
        for(i=0; i<=nmain-1; i++)
        {
            for(j=0; j<=i; j++)
            {
                state->denseh.ptr.pp_double[i][j] = (double)(0);
            }
        }
        state->hkind = 0;
        state->isdiagonalh = ae_false;
    }
    if( ftype==1 )
    {
        
        /*
         * Sparse quadratic term
         */
        state->sparseh.matrixtype = 1;
        state->sparseh.m = n;
        state->sparseh.n = n;
        state->sparseh.ninitialized = n;
        ivectorsetlengthatleast(&state->sparseh.idx, n, _state);
        rvectorsetlengthatleast(&state->sparseh.vals, n, _state);
        ivectorsetlengthatleast(&state->sparseh.ridx, n+1, _state);
        for(i=0; i<=n-1; i++)
        {
            state->sparseh.idx.ptr.p_int[i] = i;
            state->sparseh.vals.ptr.p_double[i] = 0.0;
            state->sparseh.ridx.ptr.p_int[i] = i;
        }
        state->sparseh.ridx.ptr.p_int[n] = n;
        sparsecreatecrsinplace(&state->sparseh, _state);
        state->hkind = 1;
        state->isdiagonalh = ae_true;
    }
    ae_assert(state->hkind>=0, "VIPMInit: integrity check failed", _state);
    
    /*
     * Box constraints - default values
     */
    rvectorsetlengthatleast(&state->bndl, n, _state);
    rvectorsetlengthatleast(&state->bndu, n, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    for(i=0; i<=n-1; i++)
    {
        state->hasbndl.ptr.p_bool[i] = ae_false;
        state->hasbndu.ptr.p_bool[i] = ae_false;
        state->bndl.ptr.p_double[i] = _state->v_neginf;
        state->bndu.ptr.p_double[i] = _state->v_posinf;
    }
    
    /*
     * Linear constraints - empty
     */
    state->mdense = 0;
    state->msparse = 0;
    state->combinedaslack.m = 0;
    state->combinedaslack.n = nslack;
    state->sparseamain.m = 0;
    state->sparseamain.n = nmain;
    sparsecreatecrsinplace(&state->sparseamain, _state);
    sparsecreatecrsinplace(&state->combinedaslack, _state);
}


/*************************************************************************
Computes target function 0.5*x'*H*x+c'*x

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static double vipmsolver_vipmtarget(const vipmstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nmain;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    double v;
    double result;


    n = state->n;
    nmain = state->nmain;
    ae_assert(state->hkind==0||state->hkind==1, "VIPMTarget: unexpected HKind", _state);
    result = (double)(0);
    
    /*
     * Dense
     */
    if( state->hkind==0 )
    {
        for(i=0; i<=nmain-1; i++)
        {
            for(j=0; j<=i-1; j++)
            {
                result = result+x->ptr.p_double[i]*state->denseh.ptr.pp_double[i][j]*x->ptr.p_double[j];
            }
            result = result+0.5*x->ptr.p_double[i]*x->ptr.p_double[i]*state->denseh.ptr.pp_double[i][i];
        }
        for(i=0; i<=n-1; i++)
        {
            result = result+state->c.ptr.p_double[i]*x->ptr.p_double[i];
        }
        return result;
    }
    
    /*
     * Sparse
     */
    if( state->hkind==1 )
    {
        for(i=0; i<=n-1; i++)
        {
            result = result+state->c.ptr.p_double[i]*x->ptr.p_double[i];
            j0 = state->sparseh.ridx.ptr.p_int[i];
            j1 = state->sparseh.didx.ptr.p_int[i]-1;
            for(k=j0; k<=j1; k++)
            {
                v = state->sparseh.vals.ptr.p_double[k];
                j = state->sparseh.idx.ptr.p_int[k];
                result = result+v*x->ptr.p_double[i]*x->ptr.p_double[j];
            }
            ae_assert(state->sparseh.uidx.ptr.p_int[i]!=state->sparseh.didx.ptr.p_int[i], "VIPMTarget: sparse diagonal not found", _state);
            v = state->sparseh.vals.ptr.p_double[state->sparseh.didx.ptr.p_int[i]];
            result = result+0.5*v*x->ptr.p_double[i]*x->ptr.p_double[i];
        }
        return result;
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
static void vipmsolver_multiplygeax(const vipmstate* state,
     double alpha,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t offsax,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t mdense;
    ae_int_t msparse;


    n = state->n;
    m = state->mdense+state->msparse;
    mdense = state->mdense;
    msparse = state->msparse;
    if( ae_fp_eq(beta,(double)(0)) )
    {
        rallocv(offsax+m, y, _state);
    }
    else
    {
        ae_assert(y->cnt>=offsax+m, "MultiplyGEAX: Y is too short", _state);
    }
    if( msparse>0 )
    {
        sparsegemv(&state->sparseafull, alpha, 0, x, offsx, beta, y, offsax, _state);
    }
    if( mdense>0 )
    {
        rmatrixgemv(mdense, n, alpha, &state->denseafull, 0, 0, 0, x, offsx, beta, y, offsax+msparse, _state);
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
static void vipmsolver_multiplygeatx(const vipmstate* state,
     double alpha,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t mdense;
    ae_int_t msparse;


    n = state->n;
    mdense = state->mdense;
    msparse = state->msparse;
    if( ae_fp_eq(beta,(double)(0)) )
    {
        rallocv(offsy+n, y, _state);
        rsetvx(n, 0.0, y, offsy, _state);
    }
    else
    {
        ae_assert(y->cnt>=offsy+n, "MultiplyGEATX: Y is too short", _state);
        rmulvx(n, beta, y, offsy, _state);
    }
    if( msparse>0 )
    {
        sparsegemv(&state->sparseafull, alpha, 1, x, offsx, 1.0, y, offsy, _state);
    }
    if( mdense>0 )
    {
        rmatrixgemv(n, mdense, alpha, &state->denseafull, 0, 0, 1, x, offsx+msparse, 1.0, y, offsy, _state);
    }
}


/*************************************************************************
Computes H*x, does not support advanced functionality of GEAX/GEATX

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_multiplyhx(const vipmstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nmain;
    ae_int_t i;


    n = state->n;
    nmain = state->nmain;
    rvectorsetlengthatleast(hx, n, _state);
    ae_assert(state->hkind==0||state->hkind==1, "VIPMMultiplyHX: unexpected HKind", _state);
    if( state->hkind==0 )
    {
        rmatrixsymv(nmain, 1.0, &state->denseh, 0, 0, ae_false, x, 0, 0.0, hx, 0, _state);
        for(i=nmain; i<=n-1; i++)
        {
            hx->ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=n-1; i++)
        {
            hx->ptr.p_double[i] = hx->ptr.p_double[i]+x->ptr.p_double[i]*state->diagr.ptr.p_double[i];
        }
    }
    if( state->hkind==1 )
    {
        ae_assert(state->sparseh.n==n&&state->sparseh.m==n, "VIPMMultiplyHX: sparse H has incorrect size", _state);
        if( state->isdiagonalh )
        {
            
            /*
             * H is known to be diagonal, much faster code can be used
             */
            rcopyv(n, &state->diagr, hx, _state);
            raddv(n, 1.0, &state->sparseh.vals, hx, _state);
            rmergemulv(n, x, hx, _state);
        }
        else
        {
            
            /*
             * H is a general sparse matrix, use generic sparse matrix-vector multiply
             */
            sparsesmv(&state->sparseh, ae_false, x, hx, _state);
            for(i=0; i<=n-1; i++)
            {
                hx->ptr.p_double[i] = hx->ptr.p_double[i]+x->ptr.p_double[i]*state->diagr.ptr.p_double[i];
            }
        }
    }
}


/*************************************************************************
Computes products H*x, A*x, A^T*y

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_vipmmultiply(const vipmstate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* hx,
     /* Real    */ ae_vector* ax,
     /* Real    */ ae_vector* aty,
     ae_state *_state)
{


    vipmsolver_multiplygeax(state, 1.0, x, 0, 0.0, ax, 0, _state);
    vipmsolver_multiplygeatx(state, 1.0, y, 0, 0.0, aty, 0, _state);
    vipmsolver_multiplyhx(state, x, hx, _state);
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
static void vipmsolver_vipmpowerup(vipmstate* state,
     double regfree,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    double v;
    double vrhs;
    double priorcoeff;
    double initprimslack;
    double initdualslack;
    double maxinitialnoncentrality;
    double maxinitialimbalance;
    double mu0;
    double mumin;
    ae_bool success;


    ae_assert(state->factorizationtype==0||state->factorizationtype==1, "VIPMPowerUp: unexpected factorization type", _state);
    n = state->n;
    m = state->mdense+state->msparse;
    ae_assert(!state->factorizationpoweredup, "VIPMPowerUp: repeated call", _state);
    maxinitialnoncentrality = 1.0E-6;
    maxinitialimbalance = 1.0E-6;
    
    /*
     * Set up information about presence of slack variables.
     * Decide which components of X should be frozen.
     * Compute diagonal regularization matrix R.
     */
    bcopyallocv(n, &state->hasbndl, &state->hasgz, _state);
    bcopyallocv(n, &state->hasbndu, &state->hasts, _state);
    bsetallocv(n, ae_false, &state->isfrozen, _state);
    rsetallocv(n, 0.0, &state->diagr, _state);
    vipmsolver_varsinitbyzero(&state->current, n, m, _state);
    for(i=0; i<=n-1; i++)
    {
        if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->bndl.ptr.p_double[i],state->bndu.ptr.p_double[i]) )
        {
            state->isfrozen.ptr.p_bool[i] = ae_true;
            state->hasgz.ptr.p_bool[i] = ae_false;
            state->hasts.ptr.p_bool[i] = ae_false;
            state->current.x.ptr.p_double[i] = state->bndl.ptr.p_double[i];
        }
        if( !state->hasbndl.ptr.p_bool[i]&&!state->hasbndu.ptr.p_bool[i] )
        {
            state->diagr.ptr.p_double[i] = regfree;
        }
    }
    ballocv(m, &state->haspq, _state);
    ballocv(m, &state->haswv, _state);
    for(i=0; i<=m-1; i++)
    {
        state->haswv.ptr.p_bool[i] = (state->slacksforequalityconstraints||!state->hasr.ptr.p_bool[i])||ae_fp_greater(state->r.ptr.p_double[i],(double)(0));
        state->haspq.ptr.p_bool[i] = state->hasr.ptr.p_bool[i]&&state->haswv.ptr.p_bool[i];
    }
    state->cntgz = 0;
    state->cntts = 0;
    state->cntwv = 0;
    state->cntpq = 0;
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            state->cntgz = state->cntgz+1;
        }
        if( state->hasts.ptr.p_bool[i] )
        {
            state->cntts = state->cntts+1;
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haswv.ptr.p_bool[i] )
        {
            state->cntwv = state->cntwv+1;
        }
        if( state->haspq.ptr.p_bool[i] )
        {
            state->cntpq = state->cntpq+1;
        }
    }
    
    /*
     * Special initialization for sparse version
     */
    if( state->factorizationtype==1 )
    {
        vipmsolver_reducedsysteminit(&state->reducedsparsesystem, state, _state);
    }
    state->factorizationpoweredup = ae_true;
    
    /*
     * Set up initial values of primal and dual variables X and Y by solving
     * modified KKT system which tries to enforce linear constraints (ignoring
     * box constraints for a while) subject to minimization of additional prior
     * term which moves solution towards some interior point.
     *
     * Here we expect that State.Current.X contains zeros in non-fixed variables
     * and their fixed values for fixed ones.
     */
    priorcoeff = 1.0;
    success = vipmsolver_vipmfactorize(state, 0.0, &state->diagddr, 0.0, &state->diagder, priorcoeff, priorcoeff, ae_machineepsilon, ae_machineepsilon, _state);
    ae_assert(success, "VIPMInitialPoint: impossible failure of LDLT factorization", _state);
    vipmsolver_multiplyhx(state, &state->current.x, &state->tmp0, _state);
    vipmsolver_multiplygeax(state, 1.0, &state->current.x, 0, 0.0, &state->tmp1, 0, _state);
    rallocv(n+m, &state->deltaxy, _state);
    for(i=0; i<=n-1; i++)
    {
        state->deltaxy.ptr.p_double[i] = state->c.ptr.p_double[i]+state->tmp0.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        
        /*
         * We need to specify target right-hand sides for constraints.
         *
         * Ether zero, b or b+r is used (depending on presence of r and
         * magnitudes of b and b+r, and subject to current state of frozen
         * variables).
         */
        vrhs = state->b.ptr.p_double[i]-state->tmp1.ptr.p_double[i];
        if( state->hasr.ptr.p_bool[i] )
        {
            
            /*
             * Range constraint b<=Ax<=b+r
             */
            if( ae_fp_greater_eq(vrhs,(double)(0)) )
            {
                
                /*
                 * 0<=b<=b+r, select target at lower bound
                 */
                v = vrhs;
            }
            else
            {
                
                /*
                 * b<=0, b+r can have any sign.
                 * Select zero target if possible, if not - one with smallest absolute value.
                 */
                v = ae_minreal(vrhs+state->r.ptr.p_double[i], 0.0, _state);
            }
        }
        else
        {
            
            /*
             * Single-sided constraint Ax>=b.
             * Select zero target if possible, if not - one with smallest absolute value.
             */
            v = ae_maxreal(vrhs, 0.0, _state);
        }
        state->deltaxy.ptr.p_double[n+i] = v;
    }
    vipmsolver_solvereducedkktsystem(state, &state->deltaxy, _state);
    for(i=0; i<=n-1; i++)
    {
        if( !state->isfrozen.ptr.p_bool[i] )
        {
            state->current.x.ptr.p_double[i] = state->deltaxy.ptr.p_double[i];
        }
    }
    for(i=0; i<=m-1; i++)
    {
        state->current.y.ptr.p_double[i] = state->deltaxy.ptr.p_double[n+i];
    }
    
    /*
     * Set up slacks according to our own heuristic
     */
    initprimslack = ae_maxreal(vipmsolver_initslackval, rmaxabsv(n, &state->current.x, _state), _state);
    initdualslack = ae_maxreal(vipmsolver_initslackval, rmaxabsv(m, &state->current.y, _state), _state);
    vipmsolver_multiplygeax(state, 1.0, &state->current.x, 0, 0.0, &state->tmpax, 0, _state);
    mu0 = 1.0;
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            state->current.g.ptr.p_double[i] = ae_maxreal(ae_fabs(state->current.x.ptr.p_double[i]-state->bndl.ptr.p_double[i], _state), initprimslack, _state);
            state->current.z.ptr.p_double[i] = ae_maxreal(state->current.g.ptr.p_double[i]*maxinitialimbalance, initdualslack, _state);
            mu0 = ae_maxreal(mu0, state->current.g.ptr.p_double[i]*state->current.z.ptr.p_double[i], _state);
        }
        if( state->hasts.ptr.p_bool[i] )
        {
            state->current.t.ptr.p_double[i] = ae_maxreal(ae_fabs(state->current.x.ptr.p_double[i]-state->bndu.ptr.p_double[i], _state), initprimslack, _state);
            state->current.s.ptr.p_double[i] = ae_maxreal(state->current.t.ptr.p_double[i]*maxinitialimbalance, initdualslack, _state);
            mu0 = ae_maxreal(mu0, state->current.t.ptr.p_double[i]*state->current.s.ptr.p_double[i], _state);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haswv.ptr.p_bool[i] )
        {
            state->current.w.ptr.p_double[i] = ae_maxreal(ae_fabs(state->tmpax.ptr.p_double[i]-state->b.ptr.p_double[i], _state), initprimslack, _state);
            state->current.v.ptr.p_double[i] = rmax3(state->current.w.ptr.p_double[i]*maxinitialimbalance, ae_fabs(state->current.y.ptr.p_double[i], _state), vipmsolver_initslackval, _state);
            mu0 = ae_maxreal(mu0, state->current.w.ptr.p_double[i]*state->current.v.ptr.p_double[i], _state);
        }
        if( state->haspq.ptr.p_bool[i] )
        {
            state->current.p.ptr.p_double[i] = ae_maxreal(ae_fabs(state->r.ptr.p_double[i]-state->current.w.ptr.p_double[i], _state), initprimslack, _state);
            state->current.q.ptr.p_double[i] = rmax3(state->current.p.ptr.p_double[i]*maxinitialimbalance, ae_fabs(state->current.y.ptr.p_double[i], _state), vipmsolver_initslackval, _state);
            mu0 = ae_maxreal(mu0, state->current.p.ptr.p_double[i]*state->current.q.ptr.p_double[i], _state);
        }
    }
    
    /*
     * Additional shift to ensure that initial point is not too non-centered
     */
    mumin = mu0*maxinitialnoncentrality;
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i]&&ae_fp_less(state->current.g.ptr.p_double[i]*state->current.z.ptr.p_double[i],mumin) )
        {
            v = ae_sqrt(mumin/(state->current.g.ptr.p_double[i]*state->current.z.ptr.p_double[i]), _state);
            state->current.g.ptr.p_double[i] = state->current.g.ptr.p_double[i]*v;
            state->current.z.ptr.p_double[i] = state->current.z.ptr.p_double[i]*v;
        }
        if( state->hasts.ptr.p_bool[i]&&ae_fp_less(state->current.t.ptr.p_double[i]*state->current.s.ptr.p_double[i],mumin) )
        {
            v = ae_sqrt(mumin/(state->current.t.ptr.p_double[i]*state->current.s.ptr.p_double[i]), _state);
            state->current.t.ptr.p_double[i] = state->current.t.ptr.p_double[i]*v;
            state->current.s.ptr.p_double[i] = state->current.s.ptr.p_double[i]*v;
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haswv.ptr.p_bool[i]&&ae_fp_less(state->current.w.ptr.p_double[i]*state->current.v.ptr.p_double[i],mumin) )
        {
            v = ae_sqrt(mumin/(state->current.w.ptr.p_double[i]*state->current.v.ptr.p_double[i]), _state);
            state->current.w.ptr.p_double[i] = state->current.w.ptr.p_double[i]*v;
            state->current.v.ptr.p_double[i] = state->current.v.ptr.p_double[i]*v;
        }
        if( state->haspq.ptr.p_bool[i]&&ae_fp_less(state->current.p.ptr.p_double[i]*state->current.q.ptr.p_double[i],mumin) )
        {
            v = ae_sqrt(mumin/(state->current.p.ptr.p_double[i]*state->current.q.ptr.p_double[i]), _state);
            state->current.p.ptr.p_double[i] = state->current.p.ptr.p_double[i]*v;
            state->current.q.ptr.p_double[i] = state->current.q.ptr.p_double[i]*v;
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
This function performs factorization of modified KKT system

    (                        |                 )
    ( -(H+alpha0*D+alpha1*I) |       A^T       )
    (                        |                 )
    (------------------------|-----------------)
    (                        |                 )
    (           A            | beta0*E+beta1*I )
    (                        |                 )

where:
* H is an NxN quadratic term
* A is an MxN matrix of linear constraint
* alpha0, alpha1, beta0, beta1 are nonnegative scalars
* D and E are diagonal matrices with nonnegative entries (which are ignored
  if alpha0 and beta0 are zero - arrays are not referenced at all)
* I is an NxN or MxM identity matrix

Additionally, regularizing term

    (        |        )
    ( -reg*I |        )
    (        |        )
    (--------|--------)
    (        |        )
    (        | +reg*I )
    (        |        )

is added to the entire KKT system prior to factorization in order to
improve its numerical stability.

Returns True on success, False on falure of factorization (it is recommended
to increase regularization parameter and try one more time).


  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static ae_bool vipmsolver_vipmfactorize(vipmstate* state,
     double alpha0,
     /* Real    */ const ae_vector* d,
     double beta0,
     /* Real    */ const ae_vector* e,
     double alpha11,
     double beta11,
     double modeps,
     double dampeps,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nmain;
    ae_int_t nslack;
    ae_int_t m;
    ae_int_t mdense;
    ae_int_t msparse;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t k0;
    ae_int_t k1;
    ae_int_t ka;
    ae_int_t kb;
    ae_int_t ja;
    ae_int_t jb;
    double va;
    double vb;
    double v;
    double vv;
    double badchol;
    double sumsq;
    double errsq;
    ae_int_t t0;
    ae_bool result;


    ae_assert(ae_isfinite(alpha0, _state)&&ae_fp_greater_eq(alpha0,(double)(0)), "VIPMFactorize: Alpha0 is infinite or negative", _state);
    ae_assert(ae_isfinite(alpha11, _state)&&ae_fp_greater_eq(alpha11,(double)(0)), "VIPMFactorize: Alpha1 is infinite or negative", _state);
    ae_assert(ae_isfinite(beta0, _state)&&ae_fp_greater_eq(beta0,(double)(0)), "VIPMFactorize: Beta0 is infinite or negative", _state);
    ae_assert(ae_isfinite(beta11, _state)&&ae_fp_greater_eq(beta11,(double)(0)), "VIPMFactorize: Beta1 is infinite or negative", _state);
    ae_assert(state->factorizationtype==0||state->factorizationtype==1, "VIPMFactorize: unexpected factorization type", _state);
    ae_assert(state->factorizationpoweredup, "VIPMFactorize: critical integrity check failed (no powerup stage)", _state);
    n = state->n;
    nmain = state->nmain;
    nslack = n-nmain;
    m = state->mdense+state->msparse;
    mdense = state->mdense;
    msparse = state->msparse;
    state->factorizationpresent = ae_false;
    badchol = 1.0E50;
    result = ae_true;
    
    /*
     * Dense NxN normal equations approach
     */
    if( state->factorizationtype==0 )
    {
        
        /*
         * A problem formulation with possible slacks.
         *
         * === A FORMULATION WITHOUT FROZEN VARIABLES ===
         *
         * We have to solve following system:
         *
         *     [ -(H+Dh+Rh)         Ah'  ] [ Xh ]   [ Bh ]
         *     [          -(Dz+Rz)  Az'  ] [ Xz ] = [ Bz ]
         *     [   Ah     Az         E   ] [ Y  ]   [ By ]
         *
         * with Xh being NMain-dimensional vector, Xz being NSlack-dimensional vector, constraint
         * matrix A being divided into non-slack and slack parts Ah and Az (and Ah, in turn, being
         * divided into sparse and dense parts), Rh and Rz being diagonal regularization matrix,
         * Y being M-dimensional vector.
         *
         * NOTE: due to definition of slack variables following holds: for any diagonal matrix W
         *       a product Az*W*Az' is a diagonal matrix.
         *
         * From the second line we get
         *
         *     Xz = inv(Dz+Rz)*Az'*y - inv(Dz+Rz)*Bz
         *        = inv(Dz+Rz)*Az'*y - BzWave
         *
         * Using this value for Zx, third line gives us 
         *
         *     Y  = inv(E+Az*inv(Dz+Rz)*Az')*(By+Az*BzWave-Ah*Xh)
         *        = inv(EWave)*(ByWave-Ah*Xh)
         *        with EWave = E+Az*inv(Dz+Rz)*Az' and ByWave = By+Az*BzWave
         *
         * Finally, first line gives us 
         *
         *     Xh = -inv(H+Dh+Rh+Ah'*inv(EWave)*Ah)*(Bh-Ah'*inv(EWave)*ByWave)
         *        = -inv(HWave)*BhWave
         *        with HWave = H+Dh+Rh+Ah'*inv(EWave)*Ah and BhWave = Bh-Ah'*inv(EWave)*ByWave
         *
         * In order to prepare factorization we need to compute:
         * (a) diagonal matrices Dh, Rh, Dz and Rz (and precomputed inverse of Dz+Rz)
         * (b) EWave
         * (c) HWave
         *
         * === SPECIAL HANDLING OF FROZEN VARIABLES ===
         *
         * Frozen variables result in zero steps, i.e. zero components of Xh and Xz.
         * It could be implemented by explicit modification of KKT system (zeroing out
         * columns/rows of KKT matrix, rows of right part, putting 1's to diagonal).
         *
         * However, it is possible to do without actually modifying quadratic term and
         * constraints:
         * * freezing elements of Xz can be implemented by zeroing out corresponding
         *   columns of inv(Dz+Rz) because Az always appears in computations along with diagonal Dz+Rz.
         * * freezing elements of Xh is a bit more complex - it needs:
         *   * zeroing out columns/rows of HWave and setting up unit diagonal prior to solving for Xh
         *   * explicitly zeroing out computed elements of Xh prior to computing Y and Xz
         */
        rvectorsetlengthatleast(&state->factregdhrh, nmain, _state);
        rvectorsetlengthatleast(&state->factinvregdzrz, nslack, _state);
        for(i=0; i<=n-1; i++)
        {
            v = (double)(0);
            if( alpha0>(double)0 )
            {
                v = v+alpha0*d->ptr.p_double[i];
            }
            if( alpha11>(double)0 )
            {
                v = v+alpha11;
            }
            v = v+state->diagr.ptr.p_double[i];
            v = v+dampeps;
            ae_assert(v>(double)0, "VIPMFactorize: integrity check failed, degenerate diagonal matrix", _state);
            if( i>=nmain )
            {
                if( !state->isfrozen.ptr.p_bool[i] )
                {
                    state->factinvregdzrz.ptr.p_double[i-nmain] = (double)1/v;
                }
                else
                {
                    state->factinvregdzrz.ptr.p_double[i-nmain] = 0.0;
                }
            }
            else
            {
                state->factregdhrh.ptr.p_double[i] = v;
            }
        }
        
        /*
         * Now we are ready to compute EWave
         */
        rvectorsetlengthatleast(&state->factregewave, m, _state);
        for(i=0; i<=m-1; i++)
        {
            
            /*
             * Compute diagonal element of E
             */
            v = (double)(0);
            if( beta0>(double)0 )
            {
                v = v+beta0*e->ptr.p_double[i];
            }
            if( beta11>(double)0 )
            {
                v = v+beta11;
            }
            v = v+dampeps;
            ae_assert(v>(double)0, "VIPMFactorize: integrity check failed, degenerate diagonal matrix", _state);
            
            /*
             * Compute diagonal modification Az*inv(Dz)*Az'
             */
            k0 = state->combinedaslack.ridx.ptr.p_int[i];
            k1 = state->combinedaslack.ridx.ptr.p_int[i+1]-1;
            for(k=k0; k<=k1; k++)
            {
                vv = state->combinedaslack.vals.ptr.p_double[k];
                v = v+vv*vv*state->factinvregdzrz.ptr.p_double[state->combinedaslack.idx.ptr.p_int[k]];
            }
            
            /*
             * Save EWave
             */
            state->factregewave.ptr.p_double[i] = v;
        }
        
        /*
         * Now we are ready to compute HWave:
         * * store H
         * * add Dh
         * * add Ah'*inv(EWave)*Ah
         */
        rmatrixsetlengthatleast(&state->factdensehaug, nmain, nmain, _state);
        ae_assert(state->hkind==0, "VIPMFactorize: unexpected HKind", _state);
        rmatrixcopy(nmain, nmain, &state->denseh, 0, 0, &state->factdensehaug, 0, 0, _state);
        for(i=0; i<=nmain-1; i++)
        {
            state->factdensehaug.ptr.pp_double[i][i] = state->factdensehaug.ptr.pp_double[i][i]+state->factregdhrh.ptr.p_double[i];
        }
        if( msparse>0 )
        {
            
            /*
             * Handle sparse part of Ah in Ah'*inv(EWave)*Ah
             */
            for(i=0; i<=msparse-1; i++)
            {
                v = 1.0/state->factregewave.ptr.p_double[i];
                k0 = state->sparseamain.ridx.ptr.p_int[i];
                k1 = state->sparseamain.ridx.ptr.p_int[i+1]-1;
                for(ka=k0; ka<=k1; ka++)
                {
                    ja = state->sparseamain.idx.ptr.p_int[ka];
                    va = state->sparseamain.vals.ptr.p_double[ka];
                    for(kb=k0; kb<=ka; kb++)
                    {
                        jb = state->sparseamain.idx.ptr.p_int[kb];
                        vb = state->sparseamain.vals.ptr.p_double[kb];
                        state->factdensehaug.ptr.pp_double[ja][jb] = state->factdensehaug.ptr.pp_double[ja][jb]+v*va*vb;
                    }
                }
            }
        }
        if( mdense>0 )
        {
            
            /*
             * Handle dense part of Ah in Ah'*inv(EWave)*Ah
             */
            rmatrixsetlengthatleast(&state->tmpr2, mdense, nmain, _state);
            rmatrixcopy(mdense, nmain, &state->denseamain, 0, 0, &state->tmpr2, 0, 0, _state);
            for(i=0; i<=mdense-1; i++)
            {
                v = 1.0/ae_sqrt(state->factregewave.ptr.p_double[msparse+i], _state);
                for(j=0; j<=nmain-1; j++)
                {
                    state->tmpr2.ptr.pp_double[i][j] = v*state->tmpr2.ptr.pp_double[i][j];
                }
            }
            rmatrixsyrk(nmain, mdense, 1.0, &state->tmpr2, 0, 0, 2, 1.0, &state->factdensehaug, 0, 0, ae_false, _state);
        }
        
        /*
         * Zero out rows/cols of HWave corresponding to frozen variables, set up unit diagonal
         */
        rsetallocv(nmain, 1.0, &state->tmp0, _state);
        for(i=0; i<=nmain-1; i++)
        {
            if( state->isfrozen.ptr.p_bool[i] )
            {
                state->tmp0.ptr.p_double[i] = 0.0;
            }
        }
        for(i=0; i<=nmain-1; i++)
        {
            if( state->isfrozen.ptr.p_bool[i] )
            {
                
                /*
                 * Entire row is nullified except for diagonal element
                 */
                rsetr(i+1, 0.0, &state->factdensehaug, i, _state);
                state->factdensehaug.ptr.pp_double[i][i] = 1.0;
            }
            else
            {
                
                /*
                 * Only some components are nullified
                 */
                rmergemulvr(i+1, &state->tmp0, &state->factdensehaug, i, _state);
            }
        }
        
        /*
         * Compute Cholesky factorization of HWave
         */
        if( !spdmatrixcholesky(&state->factdensehaug, nmain, ae_false, _state) )
        {
            result = ae_false;
            return result;
        }
        v = (double)(0);
        for(i=0; i<=nmain-1; i++)
        {
            v = v+state->factdensehaug.ptr.pp_double[i][i];
        }
        if( !ae_isfinite(v, _state)||ae_fp_greater(v,badchol) )
        {
            result = ae_false;
            return result;
        }
        state->factorizationpresent = ae_true;
    }
    
    /*
     * Sparse (M+N)x(M+N) factorization
     */
    if( state->factorizationtype==1 )
    {
        t0 = 0;
        
        /*
         * Generate reduced KKT matrix
         */
        rallocv(n+m, &state->facttmpdiag, _state);
        rallocv(n+m, &state->facttmpdamp, _state);
        for(i=0; i<=n-1; i++)
        {
            vv = (double)(0);
            if( alpha0>(double)0 )
            {
                vv = vv+alpha0*d->ptr.p_double[i];
            }
            if( alpha11>(double)0 )
            {
                vv = vv+alpha11;
            }
            vv = vv+state->diagr.ptr.p_double[i];
            state->facttmpdiag.ptr.p_double[i] = -vv;
            state->facttmpdamp.ptr.p_double[i] = -dampeps;
            ae_assert(vv>(double)0, "VIPMFactorize: integrity check failed, degenerate diagonal matrix", _state);
        }
        for(i=0; i<=msparse+mdense-1; i++)
        {
            vv = (double)(0);
            if( beta0>(double)0 )
            {
                vv = vv+beta0*e->ptr.p_double[i];
            }
            if( beta11>(double)0 )
            {
                vv = vv+beta11;
            }
            state->facttmpdiag.ptr.p_double[n+i] = vv;
            state->facttmpdamp.ptr.p_double[n+i] = dampeps;
            ae_assert(vv>(double)0, "VIPMFactorize: integrity check failed, degenerate diagonal matrix", _state);
        }
        
        /*
         * Perform factorization
         * Perform additional integrity check: LDLT should reproduce diagonal of initial KKT system with good precision
         */
        if( state->dotrace )
        {
            ae_trace("--- sparse KKT factorization report ----------------------------------------------------------------\n");
            t0 = ae_tickcount();
        }
        if( !vipmsolver_reducedsystemfactorizewithaddends(&state->reducedsparsesystem, &state->facttmpdiag, &state->facttmpdamp, modeps, badchol, &sumsq, &errsq, _state) )
        {
            result = ae_false;
            return result;
        }
        if( state->dotrace )
        {
            ae_trace("> factorized in %0d ms\n",
                (int)(ae_tickcount()-t0));
        }
        if( ae_fp_greater(ae_sqrt(errsq/((double)1+sumsq), _state),ae_sqrt(ae_machineepsilon, _state)) )
        {
            if( state->dotrace )
            {
                ae_trace("LDLT-diag-err= %0.3e (diagonal reproduction error)\n",
                    (double)(ae_sqrt(errsq/((double)1+sumsq), _state)));
            }
            result = ae_false;
            return result;
        }
        state->factorizationpresent = ae_true;
        
        /*
         * Trace
         */
        if( state->dotrace )
        {
            spsymmextract(&state->reducedsparsesystem.analysis, &state->tmpsparse0, &state->tmp0, &state->tmpi, _state);
            ae_trace("> diagonal terms D and E\n");
            if( ae_fp_neq(alpha0,(double)(0)) )
            {
                v = ae_fabs(d->ptr.p_double[0], _state);
                vv = ae_fabs(d->ptr.p_double[0], _state);
                for(i=1; i<=n-1; i++)
                {
                    v = ae_minreal(v, ae_fabs(d->ptr.p_double[i], _state), _state);
                    vv = ae_maxreal(vv, ae_fabs(d->ptr.p_double[i], _state), _state);
                }
                ae_trace("diagD        = %0.3e (min) ... %0.3e (max)\n",
                    (double)(v),
                    (double)(vv));
            }
            if( m>0&&ae_fp_neq(beta0,(double)(0)) )
            {
                v = ae_fabs(e->ptr.p_double[0], _state);
                vv = ae_fabs(e->ptr.p_double[0], _state);
                for(i=1; i<=m-1; i++)
                {
                    v = ae_minreal(v, ae_fabs(e->ptr.p_double[i], _state), _state);
                    vv = ae_maxreal(vv, ae_fabs(e->ptr.p_double[i], _state), _state);
                }
                ae_trace("diagE        = %0.3e (min) ... %0.3e (max)\n",
                    (double)(v),
                    (double)(vv));
            }
            ae_trace("> LDLT factorization of entire KKT matrix\n");
            v = ae_fabs(state->tmp0.ptr.p_double[0], _state);
            vv = ae_fabs(state->tmp0.ptr.p_double[0], _state);
            for(i=0; i<=state->tmpsparse0.n-1; i++)
            {
                v = ae_maxreal(v, ae_fabs(state->tmp0.ptr.p_double[i], _state), _state);
                vv = ae_minreal(vv, ae_fabs(state->tmp0.ptr.p_double[i], _state), _state);
            }
            ae_trace("|D|          = %0.3e (min) ... %0.3e (max)\n",
                (double)(vv),
                (double)(v));
            v = 0.0;
            for(i=0; i<=state->tmpsparse0.n-1; i++)
            {
                k0 = state->tmpsparse0.ridx.ptr.p_int[i];
                k1 = state->tmpsparse0.didx.ptr.p_int[i];
                for(k=k0; k<=k1; k++)
                {
                    v = ae_maxreal(v, ae_fabs(state->tmpsparse0.vals.ptr.p_double[k], _state), _state);
                }
            }
            ae_trace("max(|L|)     = %0.3e\n",
                (double)(v));
            ae_trace("diag-err     = %0.3e (diagonal reproduction error)\n",
                (double)(ae_sqrt(errsq/((double)1+sumsq), _state)));
        }
    }
    
    /*
     * Done, integrity control
     */
    ae_assert(state->factorizationpresent, "VIPMFactorize: integrity check failed", _state);
    inc(&state->repncholesky, _state);
    return result;
}


/*************************************************************************
A  low-level  function  which  solves  KKT  system  whose  regularized (!)
factorization was prepared by VIPMFactorize(). No iterative refinement  is
performed.

On input, right-hand-side is stored in DeltaXY; on output, solution replaces
DeltaXY.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_solvereducedkktsystem(vipmstate* state,
     /* Real    */ ae_vector* deltaxy,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t nmain;
    ae_int_t nslack;
    ae_int_t m;
    ae_int_t mdense;
    ae_int_t msparse;
    ae_int_t i;


    ae_assert(state->factorizationpresent, "VIPMSolve: integrity check failed - factorization is not present", _state);
    ae_assert(state->factorizationtype==0||state->factorizationtype==1, "VIPMSolve: unexpected factorization type", _state);
    n = state->n;
    nmain = state->nmain;
    nslack = n-nmain;
    m = state->mdense+state->msparse;
    mdense = state->mdense;
    msparse = state->msparse;
    
    /*
     * Dense solving
     */
    if( state->factorizationtype==0 )
    {
        
        /*
         * Compute
         *
         *     BzWave = inv(Dz+Rz)*Bz
         *     ByWave = By+Az*BzWave
         *     BhWave = Bh-Ah'*inv(EWave)*ByWave
         */
        for(i=0; i<=nslack-1; i++)
        {
            deltaxy->ptr.p_double[nmain+i] = deltaxy->ptr.p_double[nmain+i]*state->factinvregdzrz.ptr.p_double[i];
        }
        sparsegemv(&state->combinedaslack, 1.0, 0, deltaxy, nmain, 1.0, deltaxy, n, _state);
        rvectorsetlengthatleast(&state->tmp1, m, _state);
        for(i=0; i<=m-1; i++)
        {
            state->tmp1.ptr.p_double[i] = deltaxy->ptr.p_double[n+i]/state->factregewave.ptr.p_double[i];
        }
        sparsegemv(&state->sparseamain, -1.0, 1, &state->tmp1, 0, 1.0, deltaxy, 0, _state);
        rmatrixgemv(nmain, mdense, -1.0, &state->denseamain, 0, 0, 1, &state->tmp1, msparse, 1.0, deltaxy, 0, _state);
        
        /*
         * Compute Xh = -inv(HWave)*BhWave.
         * Zero out components corresponding to frozen variables.
         */
        for(i=0; i<=nmain-1; i++)
        {
            deltaxy->ptr.p_double[i] = -deltaxy->ptr.p_double[i];
        }
        rmatrixtrsv(nmain, &state->factdensehaug, 0, 0, ae_false, ae_false, 0, deltaxy, 0, _state);
        rmatrixtrsv(nmain, &state->factdensehaug, 0, 0, ae_false, ae_false, 1, deltaxy, 0, _state);
        for(i=0; i<=n-1; i++)
        {
            if( state->isfrozen.ptr.p_bool[i] )
            {
                deltaxy->ptr.p_double[i] = (double)(0);
            }
        }
        
        /*
         * Compute Y = inv(EWave)*(ByWave-Ah*Xh)
         */
        sparsegemv(&state->sparseamain, -1.0, 0, deltaxy, 0, 1.0, deltaxy, n, _state);
        rmatrixgemv(mdense, nmain, -1.0, &state->denseamain, 0, 0, 0, deltaxy, 0, 1.0, deltaxy, n+msparse, _state);
        for(i=0; i<=m-1; i++)
        {
            deltaxy->ptr.p_double[n+i] = deltaxy->ptr.p_double[n+i]/state->factregewave.ptr.p_double[i];
        }
        
        /*
         * Compute Xz = -(BzWave - inv(Dz+Rz)*Az'*y)
         */
        rvectorsetlengthatleast(&state->tmp0, nslack, _state);
        for(i=0; i<=nslack-1; i++)
        {
            state->tmp0.ptr.p_double[i] = (double)(0);
        }
        sparsegemv(&state->combinedaslack, 1.0, 1, deltaxy, n, 1.0, &state->tmp0, 0, _state);
        for(i=0; i<=nslack-1; i++)
        {
            deltaxy->ptr.p_double[nmain+i] = -(deltaxy->ptr.p_double[nmain+i]-state->factinvregdzrz.ptr.p_double[i]*state->tmp0.ptr.p_double[i]);
        }
        
        /*
         * Done
         */
        return;
    }
    
    /*
     * Sparse solving
     */
    if( state->factorizationtype==1 )
    {
        vipmsolver_reducedsystemsolve(&state->reducedsparsesystem, state->dotrace, deltaxy, _state);
        for(i=0; i<=n-1; i++)
        {
            if( state->isfrozen.ptr.p_bool[i] )
            {
                deltaxy->ptr.p_double[i] = (double)(0);
            }
        }
        return;
    }
    
    /*
     *
     */
    ae_assert(ae_false, "VIPMSolve: integrity check failed - unexpected factorization", _state);
}


/*************************************************************************
Generates precomputed temporary  vectors  and  KKT  factorization  at  the
beginning of the current iteration.

This function uses representation of  KKT  system  inspired  by  Vanderbei
slack variable approach, but with additional regularization being  applied
all along computations.

On successful factorization returns True; on failure returns False - it is
recommended to increase regularization parameter and try one more time.

--- DESCRIPTION ----------------------------------------------------------

Initial KKT system proposed by Vanderbei has following structure:

    (1) -DS*deltaT - I*deltaS                       = -mu/T + s + DELTAT*DELTAS/T = -GammaS
    (2) -DZ*deltaG - I*deltaZ                       = -mu/G + z + DELTAG*DELTAZ/G = -GammaZ
    (3) -DQ*deltaP - I*deltaQ                       = -mu/P + q + DELTAP*DELTAQ/P = -GammaQ
    (4) -DW*deltaV - I*deltaW                       = -mu/V + w + DELTAV*DELTAW/V = -GammaW
    (5)  -I*deltaY - I*deltaQ + I*deltaV            = y-q+v                       =  Beta
    (6)  -H*deltaX +A'*deltaY + I*deltaZ - I*deltaS = c-A'*y-z+s+H*x              =  Sigma
    (7)   A*deltaX - I*deltaW                       = b-A*x+w                     =  Rho
    (8)   I*deltaX - I*deltaG                       = l-x+g                       =  Nu
    (9)  -I*deltaX - I*deltaT                       = -u+x+t                      = -Tau
    (10) -I*deltaW - I*deltaP                       = -r+w+p                      = -Alpha
    
where
    
    DS = diag(S/T)
    DZ = diag(Z/G)
    DQ = diag(Q/P)
    DW = diag(W/V)

This linear system is actually  symmetric  indefinite  one,  that  can  be
regularized by modifying equations (5), (6),  (7), (8), (9), (10):

    (5)       -I*deltaY - I*deltaQ + I*deltaV -REG*deltaW= y+q-v+REG*w          =  Beta
    (6) -(H+REG)*deltaX +A'*deltaY + I*deltaZ - I*deltaS = c-A'*y-z+s+(H+REG)*x =  Sigma
    (7)        A*deltaX - I*deltaW            +REG*deltaY= b-A*x+w-REG*y        =  Rho
    (8)        I*deltaX - I*deltaG            +REG*deltaZ= l-x+g-REG*z          =  Nu
    (9)       -I*deltaX - I*deltaT            +REG*deltaS= -u+x+t-REG*s         = -Tau
    (10)      -I*deltaW - I*deltaP            +REG*deltaQ= -r+w+p-REG*q         = -Alpha
    
NOTE: regularizing equations (5)-(10) seems to be beneficial because their
      coefficients are well-normalized, usually having unit scale. Contrary
      to that, equations (1)-(4) are wildly nonnormalized, and regularization
      ruins algorithm convergence.

From (1), (2), (3) and (4) we obtain

    deltaT = (GammaS-I*deltaS)/DS
    deltaG = (GammaZ-I*deltaZ)/DZ
    deltaP = (GammaQ-I*deltaQ)/DQ
    deltaV = (GammaW-I*deltaW)/DW
    
and substitute them to equations to obtain

    (5)   -I*deltaY - I*deltaQ      - (inv(DW)+REG)*deltaW =    Beta-inv(DW)*GammaW  =  BetaCap
    (8)    I*deltaX                 + (inv(DZ)+REG)*deltaZ =      Nu+inv(DZ)*GammaZ  =  NuCap
    (9)   -I*deltaX                 + (inv(DS)+REG)*deltaS =   -(Tau-inv(DS)*GammaS) = -TauCap
    (10)  -I*deltaW                 + (inv(DQ)+REG)*deltaQ = -(Alpha-inv(DQ)*GammaQ) = -AlphaCap
    (6)   A'*deltaY + I*deltaZ - I*deltaS - (H+REG)*deltaX = c-A'*y-z+s+(H+REG)*x    =  Sigma
    (7)  REG*deltaY + A*deltaX - I*deltaW                  = b-A*x+w-REG*y           =  Rho

then, we obtain (here IRI stands for Invert-Regularize-Invert)

    DQIRI  = inv(inv(DQ)+REG)
    DZIRI  = inv(inv(DZ)+REG)
    DSIRI  = inv(inv(DS)+REG)
    
    deltaQ = (I*deltaW-AlphaCap)*DQIRI
    deltaZ =    (NuCap-I*deltaX)*DZIRI
    deltaS =   (I*deltaX-TauCap)*DSIRI
    
    DWIR   = inv(DW)+REG
    
and after substitution

    (5)   -I*deltaY        - (DQIRI+DWIR)*deltaW = BetaCap-DQIRI*AlphaCap
    (6)   A'*deltaY - (H+REG+DSIRI+DZIRI)*deltaX = Sigma-DSIRI*TauCap-DZIRI*NuCap
    (7)  REG*deltaY + A*deltaX - I*deltaW        = Rho
    
finally, we obtain
    
    DE     = inv(DQIRI+DWIR)
    DER    = DE+REG
    DDR    = DSIRI+DZIRI+REG
    deltaW = -(BetaCap-DQIRI*AlphaCap+I*deltaY)*DE

and after substitution

    (6)  -(H+DDR)*deltaX +  A'*deltaY = Sigma-DSIRI*TauCap-DZIRI*NuCap
    (7)         A*deltaX + DER*deltaY = Rho-DE*(BetaCap-DQIRI*AlphaCap)

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static ae_bool vipmsolver_vipmprecomputenewtonfactorization(vipmstate* state,
     const vipmvars* v0,
     double regeps,
     double modeps,
     double dampeps,
     double dampfree,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    ae_bool result;


    n = state->n;
    m = state->mdense+state->msparse;
    rsetallocv(n, 0.0, &state->diagdz, _state);
    rsetallocv(n, 0.0, &state->diagdzi, _state);
    rsetallocv(n, 0.0, &state->diagdziri, _state);
    rsetallocv(n, 0.0, &state->diagds, _state);
    rsetallocv(n, 0.0, &state->diagdsi, _state);
    rsetallocv(n, 0.0, &state->diagdsiri, _state);
    rsetallocv(m, 0.0, &state->diagdw, _state);
    rsetallocv(m, 0.0, &state->diagdwi, _state);
    rsetallocv(m, 0.0, &state->diagdwir, _state);
    rsetallocv(m, 0.0, &state->diagdq, _state);
    rsetallocv(m, 0.0, &state->diagdqi, _state);
    rsetallocv(m, 0.0, &state->diagdqiri, _state);
    rallocv(n, &state->diagddr, _state);
    rallocv(m, &state->diagde, _state);
    rallocv(m, &state->diagder, _state);
    
    /*
     * Handle temporary matrices arising due to box constraints
     */
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Lower bound: G*inv(Z) and Z*inv(G)
         */
        if( state->hasgz.ptr.p_bool[i] )
        {
            ae_assert(v0->g.ptr.p_double[i]>0.0&&v0->z.ptr.p_double[i]>0.0, "VIPMPrecomputeNewtonFactorization: integrity failure - G[i]<=0 or Z[i]<=0", _state);
            state->diagdz.ptr.p_double[i] = v0->z.ptr.p_double[i]/v0->g.ptr.p_double[i];
            state->diagdzi.ptr.p_double[i] = (double)1/state->diagdz.ptr.p_double[i];
            state->diagdziri.ptr.p_double[i] = (double)1/(state->diagdzi.ptr.p_double[i]+regeps);
        }
        else
        {
            ae_assert(v0->g.ptr.p_double[i]==0.0&&v0->z.ptr.p_double[i]==0.0, "VIPMPrecomputeNewtonFactorization: integrity failure - G[i]<>0 or Z[i]<>0 for absent lower bound", _state);
        }
        
        /*
         * Upper bound: T*inv(S) and S*inv(T)
         */
        if( state->hasts.ptr.p_bool[i] )
        {
            ae_assert(v0->t.ptr.p_double[i]>0.0&&v0->s.ptr.p_double[i]>0.0, "VIPMPrecomputeNewtonFactorization: integrity failure - T[i]<=0 or S[i]<=0", _state);
            state->diagds.ptr.p_double[i] = v0->s.ptr.p_double[i]/v0->t.ptr.p_double[i];
            state->diagdsi.ptr.p_double[i] = (double)1/state->diagds.ptr.p_double[i];
            state->diagdsiri.ptr.p_double[i] = (double)1/(state->diagdsi.ptr.p_double[i]+regeps);
        }
        else
        {
            ae_assert(v0->t.ptr.p_double[i]==0.0&&v0->s.ptr.p_double[i]==0.0, "VIPMPrecomputeNewtonFactorization: integrity failure - T[i]<>0 or S[i]<>0 for absent upper bound", _state);
        }
        
        /*
         * Diagonal term D
         */
        state->diagddr.ptr.p_double[i] = state->diagdziri.ptr.p_double[i]+state->diagdsiri.ptr.p_double[i]+regeps;
        if( !state->hasgz.ptr.p_bool[i]&&!state->hasts.ptr.p_bool[i] )
        {
            state->diagddr.ptr.p_double[i] = state->diagddr.ptr.p_double[i]+dampfree;
        }
    }
    
    /*
     * Handle temporary matrices arising due to linear constraints: with lower bound B[]
     * or with lower and upper bounds.
     */
    for(i=0; i<=m-1; i++)
    {
        
        /*
         * Lower bound
         */
        if( state->haswv.ptr.p_bool[i] )
        {
            ae_assert(v0->v.ptr.p_double[i]>0.0&&v0->w.ptr.p_double[i]>0.0, "VIPMPrecomputeNewtonFactorization: integrity failure - V[i]<=0 or W[i]<=0", _state);
            state->diagdw.ptr.p_double[i] = v0->w.ptr.p_double[i]/v0->v.ptr.p_double[i];
            state->diagdwi.ptr.p_double[i] = (double)1/state->diagdw.ptr.p_double[i];
            state->diagdwir.ptr.p_double[i] = state->diagdwi.ptr.p_double[i]+regeps;
        }
        else
        {
            ae_assert(v0->v.ptr.p_double[i]==0.0&&v0->w.ptr.p_double[i]==0.0, "VIPMPrecomputeNewtonFactorization: integrity failure - V[i]<>0 or W[i]<>0 for linear equality constraint", _state);
        }
        
        /*
         * Upper bound
         */
        if( state->haspq.ptr.p_bool[i] )
        {
            ae_assert(v0->p.ptr.p_double[i]>0.0&&v0->q.ptr.p_double[i]>0.0, "VIPMPrecomputeNewtonFactorization: integrity failure - P[i]<=0 or Q[i]<=0", _state);
            state->diagdq.ptr.p_double[i] = v0->q.ptr.p_double[i]/v0->p.ptr.p_double[i];
            state->diagdqi.ptr.p_double[i] = (double)1/state->diagdq.ptr.p_double[i];
            state->diagdqiri.ptr.p_double[i] = (double)1/(state->diagdqi.ptr.p_double[i]+regeps);
        }
        else
        {
            ae_assert(v0->p.ptr.p_double[i]==0.0&&v0->q.ptr.p_double[i]==0.0, "VIPMPrecomputeNewtonFactorization: integrity failure - P[i]<>0 or Q[i]<>0 for absent linear constraint", _state);
        }
        
        /*
         * Diagonal term E
         */
        if( state->haswv.ptr.p_bool[i]||state->haspq.ptr.p_bool[i] )
        {
            state->diagde.ptr.p_double[i] = (double)1/(state->diagdwir.ptr.p_double[i]+state->diagdqiri.ptr.p_double[i]);
        }
        else
        {
            state->diagde.ptr.p_double[i] = 0.0;
        }
        state->diagder.ptr.p_double[i] = state->diagde.ptr.p_double[i]+regeps;
    }
    
    /*
     * Perform factorization
     */
    result = vipmsolver_vipmfactorize(state, 1.0, &state->diagddr, 1.0, &state->diagder, 0.0, 0.0, modeps, dampeps, _state);
    return result;
}


/*************************************************************************
Solves KKT system stored in VIPMState with user-passed RHS.
Sol must be preallocated VIPMVars object whose initial values are ignored.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_solvekktsystem(vipmstate* state,
     const vipmrighthandside* rhs,
     vipmvars* sol,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;


    n = state->n;
    m = state->mdense+state->msparse;
    
    /*
     * Compute elimination temporaries
     *
     * RhsAlphaCap  = RhsAlpha - InvDQ*GammaQ
     * RhsNuCap     = RhsNu    + InvDZ*GammaZ
     * RhsTauCap    = RhsTau   - InvDS*GammaS
     * RhsBetaCap   = RhsBeta  - InvDW*GammaW
     */
    rallocv(n, &state->rhsnucap, _state);
    rallocv(n, &state->rhstaucap, _state);
    rallocv(m, &state->rhsbetacap, _state);
    rallocv(m, &state->rhsalphacap, _state);
    rcopynegmuladdv(m, &state->diagdqi, &rhs->gammaq, &rhs->alpha, &state->rhsalphacap, _state);
    rcopymuladdv(n, &state->diagdzi, &rhs->gammaz, &rhs->nu, &state->rhsnucap, _state);
    rcopynegmuladdv(n, &state->diagdsi, &rhs->gammas, &rhs->tau, &state->rhstaucap, _state);
    rcopynegmuladdv(m, &state->diagdwi, &rhs->gammaw, &rhs->beta, &state->rhsbetacap, _state);
    
    /*
     * Solve reduced KKT system
     */
    rvectorsetlengthatleast(&state->deltaxy, n+m, _state);
    for(i=0; i<=n-1; i++)
    {
        state->deltaxy.ptr.p_double[i] = rhs->sigma.ptr.p_double[i]-state->diagdziri.ptr.p_double[i]*state->rhsnucap.ptr.p_double[i]-state->diagdsiri.ptr.p_double[i]*state->rhstaucap.ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        state->deltaxy.ptr.p_double[n+i] = rhs->rho.ptr.p_double[i]-state->diagde.ptr.p_double[i]*(state->rhsbetacap.ptr.p_double[i]-state->diagdqiri.ptr.p_double[i]*state->rhsalphacap.ptr.p_double[i]);
    }
    vipmsolver_solvereducedkktsystem(state, &state->deltaxy, _state);
    
    /*
     * Perform backsubstitution
     */
    for(i=0; i<=n-1; i++)
    {
        sol->x.ptr.p_double[i] = state->deltaxy.ptr.p_double[i];
        sol->s.ptr.p_double[i] = state->diagdsiri.ptr.p_double[i]*(sol->x.ptr.p_double[i]-state->rhstaucap.ptr.p_double[i]);
        sol->z.ptr.p_double[i] = state->diagdziri.ptr.p_double[i]*(state->rhsnucap.ptr.p_double[i]-sol->x.ptr.p_double[i]);
        sol->g.ptr.p_double[i] = state->diagdzi.ptr.p_double[i]*(rhs->gammaz.ptr.p_double[i]-sol->z.ptr.p_double[i]);
        sol->t.ptr.p_double[i] = state->diagdsi.ptr.p_double[i]*(rhs->gammas.ptr.p_double[i]-sol->s.ptr.p_double[i]);
    }
    for(i=0; i<=m-1; i++)
    {
        sol->y.ptr.p_double[i] = state->deltaxy.ptr.p_double[n+i];
        sol->w.ptr.p_double[i] = -state->diagde.ptr.p_double[i]*(state->rhsbetacap.ptr.p_double[i]-state->diagdqiri.ptr.p_double[i]*state->rhsalphacap.ptr.p_double[i]+sol->y.ptr.p_double[i]);
        sol->q.ptr.p_double[i] = state->diagdqiri.ptr.p_double[i]*(sol->w.ptr.p_double[i]-state->rhsalphacap.ptr.p_double[i]);
        sol->v.ptr.p_double[i] = state->diagdwi.ptr.p_double[i]*(rhs->gammaw.ptr.p_double[i]-sol->w.ptr.p_double[i]);
        sol->p.ptr.p_double[i] = state->diagdqi.ptr.p_double[i]*(rhs->gammaq.ptr.p_double[i]-sol->q.ptr.p_double[i]);
    }
}


/*************************************************************************
Compute VIPM step by solving KKT system.

VDResult must be preallocated VIPMVars object  whose  initial  values  are
ignored.

Returns False on failure to compute step direction with reasonable accuracy
(it is advised to terminate iterations immediately).

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static ae_bool vipmsolver_vipmcomputestepdirection(vipmstate* state,
     const vipmvars* v0,
     double muestimate,
     const vipmvars* vdestimate,
     vipmvars* vdresult,
     double reg,
     ae_bool isdampepslarge,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    double vrhsprim2;
    double vrhsdual2;
    double vrhscmpl2;
    double vresprim2;
    double vresdual2;
    double vrescmpl2;
    double vrhspriminf;
    double vrhsdualinf;
    double vrespriminf;
    double vresdualinf;
    double badres;
    double verybadres;
    double residualgrowth;
    ae_bool primaldestabilized;
    ae_bool dualdestabilized;
    ae_bool result;


    n = state->n;
    m = state->mdense+state->msparse;
    badres = 1.01;
    verybadres = 1.0E3;
    result = ae_true;
    
    /*
     * Initial solver report
     */
    if( state->dotrace )
    {
        ae_trace("--- detailed KKT solver report ---------------------------------------------------------------------\n");
    }
    
    /*
     * Solve KKT system with right-hand sides coming from primal, dual 
     * and complementary slackness conditions. Analyze solution,
     * terminate immediately if primal/dual residuals are way too high.
     */
    vipmsolver_rhscompute(state, v0, muestimate, vdestimate, &state->rhs, reg, _state);
    vrhsprim2 = vipmsolver_rhsprimal2(&state->rhs, n, m, _state);
    vrhsdual2 = vipmsolver_rhsdual2(&state->rhs, n, m, _state);
    vrhscmpl2 = vipmsolver_rhscompl2(&state->rhs, n, m, _state);
    vrhspriminf = vipmsolver_rhsprimalinf(&state->rhs, n, m, _state);
    vrhsdualinf = vipmsolver_rhsdualinf(&state->rhs, n, m, _state);
    if( state->dotrace )
    {
        ae_trace("> primal/dual/complementarity right-hand-side\n");
        ae_trace("rhs-prim     = %0.3e (2-norm)\n",
            (double)(ae_sqrt(vrhsprim2, _state)));
        ae_trace("rhs-dual     = %0.3e (2-norm)\n",
            (double)(ae_sqrt(vrhsdual2, _state)));
        ae_trace("rhs-cmpl     = %0.3e (2-norm)\n",
            (double)(ae_sqrt(vrhscmpl2, _state)));
    }
    vipmsolver_solvekktsystem(state, &state->rhs, vdresult, _state);
    vipmsolver_rhssubtract(state, &state->rhs, v0, vdresult, reg, _state);
    vresprim2 = vipmsolver_rhsprimal2(&state->rhs, n, m, _state);
    vresdual2 = vipmsolver_rhsdual2(&state->rhs, n, m, _state);
    vrescmpl2 = vipmsolver_rhscompl2(&state->rhs, n, m, _state);
    vrespriminf = vipmsolver_rhsprimalinf(&state->rhs, n, m, _state);
    vresdualinf = vipmsolver_rhsdualinf(&state->rhs, n, m, _state);
    if( state->dotrace )
    {
        ae_trace("> primal/dual/complementarity residuals compared with RHS\n");
        ae_trace("res/rhs prim = %0.3e\n",
            (double)(ae_sqrt(vresprim2/coalesce(vrhsprim2, (double)(1), _state), _state)));
        ae_trace("res/rhs dual = %0.3e\n",
            (double)(ae_sqrt(vresdual2/coalesce(vrhsdual2, (double)(1), _state), _state)));
        ae_trace("res/rhs cmpl = %0.3e\n",
            (double)(ae_sqrt(vrescmpl2/coalesce(vrhscmpl2, (double)(1), _state), _state)));
        ae_trace("res/rhs all  = %0.3e\n",
            (double)(ae_sqrt((vresprim2+vresdual2+vrescmpl2)/coalesce(vrhsprim2+vrhsdual2+vrhscmpl2, (double)(1), _state), _state)));
    }
    primaldestabilized = ae_fp_less_eq(vrhspriminf,state->epsp)&&ae_fp_greater_eq(vrespriminf,ae_maxreal(verybadres*vrhspriminf, state->epsp, _state));
    dualdestabilized = ae_fp_less_eq(vrhsdualinf,state->epsd)&&ae_fp_greater_eq(vresdualinf,ae_maxreal(verybadres*vrhsdualinf, state->epsd, _state));
    residualgrowth = ae_sqrt((vresprim2+vresdual2+vrescmpl2)/coalesce(vrhsprim2+vrhsdual2+vrhscmpl2, (double)(1), _state), _state);
    if( ((primaldestabilized||dualdestabilized)&&ae_fp_greater(residualgrowth,0.01*ae_sqrt(ae_machineepsilon, _state)))&&!isdampepslarge )
    {
        if( state->dotrace )
        {
            ae_trace("> primal/dual residual growth is too high, signaling presence of numerical errors\n");
        }
        result = ae_false;
        return result;
    }
    if( ae_fp_greater(residualgrowth,badres) )
    {
        if( state->dotrace )
        {
            ae_trace("> total residual is too high, signaling presence of numerical errors\n");
        }
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
static void vipmsolver_vipmcomputesteplength(const vipmstate* state,
     const vipmvars* v0,
     const vipmvars* vs,
     double stepdecay,
     ae_bool separatestep,
     double* alphap,
     double* alphad,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    double alpha;

    *alphap = 0.0;
    *alphad = 0.0;

    n = state->n;
    m = state->mdense+state->msparse;
    ae_assert(n==v0->n&&m==v0->m, "VIPMComputeStepLength: sizes mismatch", _state);
    *alphap = (double)(1);
    *alphad = (double)(1);
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Primal
         */
        if( vs->g.ptr.p_double[i]<0.0 )
        {
            *alphap = safeminposrv(v0->g.ptr.p_double[i], -vs->g.ptr.p_double[i], *alphap, _state);
        }
        if( vs->t.ptr.p_double[i]<0.0 )
        {
            *alphap = safeminposrv(v0->t.ptr.p_double[i], -vs->t.ptr.p_double[i], *alphap, _state);
        }
        
        /*
         * Dual
         */
        if( vs->z.ptr.p_double[i]<0.0 )
        {
            *alphad = safeminposrv(v0->z.ptr.p_double[i], -vs->z.ptr.p_double[i], *alphad, _state);
        }
        if( vs->s.ptr.p_double[i]<0.0 )
        {
            *alphad = safeminposrv(v0->s.ptr.p_double[i], -vs->s.ptr.p_double[i], *alphad, _state);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        
        /*
         * Primal
         */
        if( vs->w.ptr.p_double[i]<0.0 )
        {
            *alphap = safeminposrv(v0->w.ptr.p_double[i], -vs->w.ptr.p_double[i], *alphap, _state);
        }
        if( vs->p.ptr.p_double[i]<0.0 )
        {
            *alphap = safeminposrv(v0->p.ptr.p_double[i], -vs->p.ptr.p_double[i], *alphap, _state);
        }
        
        /*
         * Dual
         */
        if( vs->v.ptr.p_double[i]<0.0 )
        {
            *alphad = safeminposrv(v0->v.ptr.p_double[i], -vs->v.ptr.p_double[i], *alphad, _state);
        }
        if( vs->q.ptr.p_double[i]<0.0 )
        {
            *alphad = safeminposrv(v0->q.ptr.p_double[i], -vs->q.ptr.p_double[i], *alphad, _state);
        }
    }
    
    /*
     * Separate step or joint step?
     */
    if( separatestep )
    {
        
        /*
         * Separate step on primal/dual
         */
        *alphap = stepdecay*(*alphap);
        *alphad = stepdecay*(*alphad);
    }
    else
    {
        
        /*
         * Because we may solve QP problem, step length has to be same for primal and dual variables
         */
        alpha = ae_minreal(*alphap, *alphad, _state);
        *alphap = stepdecay*alpha;
        *alphad = stepdecay*alpha;
    }
}


/*************************************************************************
This function performs IPM step, updates  iteration  counts  and  performs
following additional checks:
* it monitors status of box/linear constraints  and  smoothly  drops  ones
  with too large bounds (a variable or linear sum is well below constraint
  bound for several iterations)

INPUT PARAMETERS:
    State               -   solver state
    AlphaP              -   primal step to perform
    AlphaD              -   dual   step to perform
    
OUTPUT PARAMETERS:

  -- ALGLIB --
     Copyright 01.08.2020 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_vipmperformstep(vipmstate* state,
     double alphap,
     double alphad,
     ae_state *_state)
{


    
    /*
     * Perform step
     */
    vipmsolver_varsaddstep(&state->current, &state->deltacorr, alphap, alphad, _state);
    
    /*
     * Update iterations count
     */
    inc(&state->repiterationscount, _state);
}


/*************************************************************************
Compute primal/dual errors and complementarity gap

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_computeerrors(vipmstate* state,
     double* errp2,
     double* errd2,
     double* errpinf,
     double* errdinf,
     double* egap,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    ae_int_t cntp2;
    ae_int_t cntd2;
    double v;

    *errp2 = 0.0;
    *errd2 = 0.0;
    *errpinf = 0.0;
    *errdinf = 0.0;
    *egap = 0.0;

    n = state->n;
    m = state->mdense+state->msparse;
    
    /*
     * Compute primal and dual infeasibilities
     */
    vipmsolver_vipmmultiply(state, &state->current.x, &state->current.y, &state->tmphx, &state->tmpax, &state->tmpaty, _state);
    cntp2 = 0;
    *errp2 = (double)(0);
    *errpinf = (double)(0);
    for(i=0; i<=m-1; i++)
    {
        v = state->tmpax.ptr.p_double[i]-state->current.w.ptr.p_double[i]-state->b.ptr.p_double[i];
        *errp2 = *errp2+v*v;
        *errpinf = ae_maxreal(*errpinf, ae_fabs(v, _state), _state);
        inc(&cntp2, _state);
        if( state->haspq.ptr.p_bool[i] )
        {
            v = state->current.w.ptr.p_double[i]+state->current.p.ptr.p_double[i]-state->r.ptr.p_double[i];
            *errp2 = *errp2+v*v;
            *errpinf = ae_maxreal(*errpinf, ae_fabs(v, _state), _state);
            inc(&cntp2, _state);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            v = state->current.x.ptr.p_double[i]-state->current.g.ptr.p_double[i]-state->bndl.ptr.p_double[i];
            *errp2 = *errp2+v*v;
            *errpinf = ae_maxreal(*errpinf, ae_fabs(v, _state), _state);
            inc(&cntp2, _state);
        }
        if( state->hasts.ptr.p_bool[i] )
        {
            v = state->current.x.ptr.p_double[i]+state->current.t.ptr.p_double[i]-state->bndu.ptr.p_double[i];
            *errp2 = *errp2+v*v;
            *errpinf = ae_maxreal(*errpinf, ae_fabs(v, _state), _state);
            inc(&cntp2, _state);
        }
    }
    *errp2 = ae_sqrt(*errp2/coalesce((double)(cntp2), (double)(1), _state), _state);
    cntd2 = 0;
    *errd2 = (double)(0);
    *errdinf = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        if( !state->isfrozen.ptr.p_bool[i] )
        {
            v = state->tmphx.ptr.p_double[i]+state->c.ptr.p_double[i]-state->tmpaty.ptr.p_double[i];
            if( state->hasgz.ptr.p_bool[i] )
            {
                v = v-state->current.z.ptr.p_double[i];
            }
            if( state->hasts.ptr.p_bool[i] )
            {
                v = v+state->current.s.ptr.p_double[i];
            }
            *errd2 = *errd2+v*v;
            *errdinf = ae_maxreal(*errdinf, ae_fabs(v, _state), _state);
            inc(&cntd2, _state);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        v = (double)(0);
        if( state->haswv.ptr.p_bool[i] )
        {
            v = state->current.y.ptr.p_double[i]-state->current.v.ptr.p_double[i];
        }
        if( state->haspq.ptr.p_bool[i] )
        {
            v = v+state->current.q.ptr.p_double[i];
        }
        *errd2 = *errd2+v*v;
        *errdinf = ae_maxreal(*errdinf, ae_fabs(v, _state), _state);
        if( state->haswv.ptr.p_bool[i]||state->haspq.ptr.p_bool[i] )
        {
            inc(&cntd2, _state);
        }
    }
    *errd2 = ae_sqrt(*errd2/coalesce((double)(cntd2), (double)(1), _state), _state);
    *egap = vipmsolver_varscomputecomplementaritygap(&state->current, _state)/(1.0+ae_fabs(vipmsolver_vipmtarget(state, &state->current.x, _state), _state));
}


/*************************************************************************
Performs integrity checks for current point and step

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_runintegritychecks(const vipmstate* state,
     const vipmvars* v0,
     const vipmvars* vd,
     double alphap,
     double alphad,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;


    n = state->n;
    m = state->mdense+state->msparse;
    ae_assert(ae_isfinite(alphap, _state)&&ae_fp_greater_eq(alphap,(double)(0)), "[VIPM]RunIntegrityChecks: bad AlphaP", _state);
    ae_assert(ae_isfinite(alphad, _state)&&ae_fp_greater_eq(alphad,(double)(0)), "[VIPM]RunIntegrityChecks: bad AlphaD", _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            ae_assert(!state->isfrozen.ptr.p_bool[i], "[VIPM]RunIntegrityChecks: integrity failure - X[I] is frozen", _state);
            ae_assert(v0->g.ptr.p_double[i]>0.0&&v0->z.ptr.p_double[i]>0.0, "[VIPM]RunIntegrityChecks: integrity failure - G[i]<=0 or Z[i]<=0", _state);
        }
        else
        {
            ae_assert(v0->g.ptr.p_double[i]==0.0&&v0->z.ptr.p_double[i]==0.0, "[VIPM]RunIntegrityChecks: integrity failure - G[i]<>0 or Z[i]<>0 for absent lower bound", _state);
            ae_assert(vd->g.ptr.p_double[i]==0.0&&vd->z.ptr.p_double[i]==0.0, "[VIPM]RunIntegrityChecks: integrity failure - G[i]<>0 or Z[i]<>0 for absent lower bound", _state);
        }
        if( state->hasts.ptr.p_bool[i] )
        {
            ae_assert(!state->isfrozen.ptr.p_bool[i], "[VIPM]RunIntegrityChecks: integrity failure - X[I] is frozen", _state);
            ae_assert(v0->t.ptr.p_double[i]>0.0&&v0->s.ptr.p_double[i]>0.0, "[VIPM]RunIntegrityChecks: integrity failure - T[i]<=0 or S[i]<=0", _state);
        }
        else
        {
            ae_assert(v0->t.ptr.p_double[i]==0.0&&v0->s.ptr.p_double[i]==0.0, "[VIPM]RunIntegrityChecks: integrity failure - T[i]<>0 or S[i]<>0 for absent upper bound", _state);
            ae_assert(vd->t.ptr.p_double[i]==0.0&&vd->s.ptr.p_double[i]==0.0, "[VIPM]RunIntegrityChecks: integrity failure - T[i]<>0 or S[i]<>0 for absent upper bound", _state);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        ae_assert(state->haswv.ptr.p_bool[i]||!state->haspq.ptr.p_bool[i], "[VIPM]RunIntegrityChecks: inconsistent HasWV/HasPQ", _state);
        if( state->haswv.ptr.p_bool[i] )
        {
            ae_assert(v0->v.ptr.p_double[i]>0.0&&v0->w.ptr.p_double[i]>0.0, "[VIPM]RunIntegrityChecks: integrity failure - V[i]<=0 or W[i]<=0", _state);
        }
        else
        {
            ae_assert(v0->v.ptr.p_double[i]==0.0&&v0->w.ptr.p_double[i]==0.0, "[VIPM]RunIntegrityChecks: integrity failure - V[i]<>0 or W[i]<>0 for linear equality constraint", _state);
            ae_assert(vd->v.ptr.p_double[i]==0.0&&vd->w.ptr.p_double[i]==0.0, "[VIPM]RunIntegrityChecks: integrity failure - V[i]<>0 or W[i]<>0 for linear equality constraint", _state);
        }
        if( state->haspq.ptr.p_bool[i] )
        {
            ae_assert(v0->p.ptr.p_double[i]>0.0&&v0->q.ptr.p_double[i]>0.0, "[VIPM]RunIntegrityChecks: integrity failure - P[i]<=0 or Q[i]<=0", _state);
        }
        else
        {
            ae_assert(v0->p.ptr.p_double[i]==0.0&&v0->q.ptr.p_double[i]==0.0, "[VIPM]RunIntegrityChecks: integrity failure - P[i]<>0 or Q[i]<>0 for absent range of linear constraint", _state);
            ae_assert(vd->p.ptr.p_double[i]==0.0&&vd->q.ptr.p_double[i]==0.0, "[VIPM]RunIntegrityChecks: integrity failure - P[i]<>0 or Q[i]<>0 for absent range of linear constraint", _state);
        }
    }
}


/*************************************************************************
Evaluate progress so far, outputs trace data, if requested to do so.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_traceprogress(vipmstate* state,
     double mu,
     double muaff,
     double sigma,
     double alphap,
     double alphad,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    double v;
    double errp2;
    double errd2;
    double errpinf;
    double errdinf;
    double errgap;


    n = state->n;
    m = state->mdense+state->msparse;
    if( !state->dotrace )
    {
        return;
    }
    
    /*
     * Print high-level information
     */
    vipmsolver_computeerrors(state, &errp2, &errd2, &errpinf, &errdinf, &errgap, _state);
    ae_trace("--- step report ------------------------------------------------------------------------------------\n");
    ae_trace("> step information\n");
    ae_trace("mu_init = %0.3e    (at the beginning)\n",
        (double)(mu));
    ae_trace("mu_aff  = %0.3e    (by affine scaling step)\n",
        (double)(muaff));
    ae_trace("sigma   = %0.3e    (centering parameter)\n",
        (double)(sigma));
    ae_trace("alphaP  = %0.3e    (primal step)\n",
        (double)(alphap));
    ae_trace("alphaD  = %0.3e    (dual   step)\n",
        (double)(alphad));
    ae_trace("mu_cur  = %0.3e    (after the step)\n",
        (double)(vipmsolver_varscomputemu(state, &state->current, _state)));
    ae_trace("> errors\n");
    ae_trace("errP    = %0.3e    (primal infeasibility, inf-norm)\n",
        (double)(errpinf));
    ae_trace("errD    = %0.3e    (dual infeasibility,   inf-norm)\n",
        (double)(errdinf));
    ae_trace("errGap  = %0.3e    (complementarity gap)\n",
        (double)(errgap));
    ae_trace("> current point information (inf-norm)\n");
    ae_trace("|X|=%8.1e,  |G|=%8.1e,  |T|=%8.1e,  |W|=%8.1e,  |P|=%8.1e\n",
        (double)(rmaxabsv(n, &state->current.x, _state)),
        (double)(rmaxabsv(n, &state->current.g, _state)),
        (double)(rmaxabsv(n, &state->current.t, _state)),
        (double)(rmaxabsv(m, &state->current.w, _state)),
        (double)(rmaxabsv(m, &state->current.p, _state)));
    ae_trace("|Y|=%8.1e,  |Z|=%8.1e,  |S|=%8.1e,  |V|=%8.1e,  |Q|=%8.1e\n",
        (double)(rmaxabsv(m, &state->current.y, _state)),
        (double)(rmaxabsv(n, &state->current.z, _state)),
        (double)(rmaxabsv(n, &state->current.s, _state)),
        (double)(rmaxabsv(m, &state->current.v, _state)),
        (double)(rmaxabsv(m, &state->current.q, _state)));
    
    /*
     * Print variable stats, if required
     */
    if( state->dotrace )
    {
        ae_trace("--- variable statistics ----------------------------------------------------------------------------\n");
        ae_trace("> smallest values for nonnegative vars\n");
        ae_trace("primal:       minG=%8.1e  minT=%8.1e  minW=%8.1e  minP=%8.1e\n",
            (double)(vipmsolver_minnz(&state->current.g, n, _state)),
            (double)(vipmsolver_minnz(&state->current.t, n, _state)),
            (double)(vipmsolver_minnz(&state->current.w, m, _state)),
            (double)(vipmsolver_minnz(&state->current.p, m, _state)));
        ae_trace("dual:         minZ=%8.1e  minS=%8.1e  minV=%8.1e  minQ=%8.1e\n",
            (double)(vipmsolver_minnz(&state->current.z, n, _state)),
            (double)(vipmsolver_minnz(&state->current.s, n, _state)),
            (double)(vipmsolver_minnz(&state->current.v, m, _state)),
            (double)(vipmsolver_minnz(&state->current.q, m, _state)));
        ae_trace("> min and max complementary slackness\n");
        ae_trace("min:            GZ=%8.1e    TS=%8.1e    WV=%8.1e    PQ=%8.1e\n",
            (double)(vipmsolver_minprodnz(&state->current.g, &state->current.z, n, _state)),
            (double)(vipmsolver_minprodnz(&state->current.t, &state->current.s, n, _state)),
            (double)(vipmsolver_minprodnz(&state->current.w, &state->current.v, m, _state)),
            (double)(vipmsolver_minprodnz(&state->current.p, &state->current.q, m, _state)));
        ae_trace("max:            GZ=%8.1e    TS=%8.1e    WV=%8.1e    PQ=%8.1e\n",
            (double)(vipmsolver_maxprodnz(&state->current.g, &state->current.z, n, _state)),
            (double)(vipmsolver_maxprodnz(&state->current.t, &state->current.s, n, _state)),
            (double)(vipmsolver_maxprodnz(&state->current.w, &state->current.v, m, _state)),
            (double)(vipmsolver_maxprodnz(&state->current.p, &state->current.q, m, _state)));
    }
    
    /*
     * Detailed output (all variables values, not suited for high-dimensional problems)
     */
    if( state->dodetailedtrace )
    {
        vipmsolver_vipmmultiply(state, &state->current.x, &state->current.y, &state->tmphx, &state->tmpax, &state->tmpaty, _state);
        rsetallocv(n, 0.0, &state->tmplaggrad, _state);
        for(i=0; i<=n-1; i++)
        {
            if( !state->isfrozen.ptr.p_bool[i] )
            {
                v = state->tmphx.ptr.p_double[i]+state->c.ptr.p_double[i]-state->tmpaty.ptr.p_double[i];
                if( state->hasgz.ptr.p_bool[i] )
                {
                    v = v-state->current.z.ptr.p_double[i];
                }
                if( state->hasts.ptr.p_bool[i] )
                {
                    v = v+state->current.s.ptr.p_double[i];
                }
                state->tmplaggrad.ptr.p_double[i] = v;
            }
        }
        ae_trace("--- printing raw data (prior to applying variable scales and shifting by XOrigin) ------------------\n");
        ae_trace("X (raw)         = ");
        tracevectorunscaledunshiftedautoprec(&state->current.x, n, &state->scl, ae_true, &state->xorigin, ae_true, _state);
        ae_trace("\n");
        ae_trace("--- printing scaled data (after applying variable scales and shifting by XOrigin) ------------------\n");
        ae_trace("> reporting X, Lagrangian gradient\n");
        ae_trace("Xnew            = ");
        tracevectorautoprec(&state->current.x, 0, n, _state);
        ae_trace("\n");
        ae_trace("Lag-grad        = ");
        tracevectorautoprec(&state->tmplaggrad, 0, n, _state);
        ae_trace("\n");
        ae_trace("--- printing new point -----------------------------------------------------------------------------\n");
        ae_trace("> primal slacks and dual multipliers for box constraints\n");
        ae_trace("G (L prim slck) = ");
        tracevectorautoprec(&state->current.g, 0, n, _state);
        ae_trace("\n");
        ae_trace("Z (L dual mult) = ");
        tracevectorautoprec(&state->current.z, 0, n, _state);
        ae_trace("\n");
        ae_trace("T (U prim slck) = ");
        tracevectorautoprec(&state->current.t, 0, n, _state);
        ae_trace("\n");
        ae_trace("S (U dual mult) = ");
        tracevectorautoprec(&state->current.s, 0, n, _state);
        ae_trace("\n");
        ae_trace("> primal slacks and dual multipliers for linear constraints, B/R stand for B<=Ax<=B+R\n");
        ae_trace("Y (lag mult)    = ");
        tracevectorautoprec(&state->current.y, 0, m, _state);
        ae_trace("\n");
        ae_trace("W (B prim slck) = ");
        tracevectorautoprec(&state->current.w, 0, m, _state);
        ae_trace("\n");
        ae_trace("V (B dual mult) = ");
        tracevectorautoprec(&state->current.v, 0, m, _state);
        ae_trace("\n");
        ae_trace("P (R prim slck) = ");
        tracevectorautoprec(&state->current.p, 0, m, _state);
        ae_trace("\n");
        ae_trace("Q (R dual mult) = ");
        tracevectorautoprec(&state->current.q, 0, m, _state);
        ae_trace("\n");
    }
    ae_trace("\n");
}


/*************************************************************************
Compute right-hand side for KKT system.

INPUT PARAMETERS:
    State           -   IPM state
    V0              -   current point (used to compute RHS)
    MuEstimate      -   estimate of Mu (can be zero)
    DirEstimate     -   estimate of delta's (can be zero)
    
OUTPUT PARAMETERS:
    Rhs             -   RHS

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_rhscompute(vipmstate* state,
     const vipmvars* v0,
     double muestimate,
     const vipmvars* direstimate,
     vipmrighthandside* rhs,
     double reg,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;


    n = state->n;
    m = state->mdense+state->msparse;
    
    /*
     * Allocate
     */
    rvectorsetlengthatleast(&rhs->sigma, n, _state);
    rvectorsetlengthatleast(&rhs->nu, n, _state);
    rvectorsetlengthatleast(&rhs->tau, n, _state);
    rvectorsetlengthatleast(&rhs->gammaz, n, _state);
    rvectorsetlengthatleast(&rhs->gammas, n, _state);
    rvectorsetlengthatleast(&rhs->gammaw, m, _state);
    rvectorsetlengthatleast(&rhs->gammaq, m, _state);
    rsetallocv(m, 0.0, &rhs->beta, _state);
    rsetallocv(m, 0.0, &rhs->rho, _state);
    rsetallocv(m, 0.0, &rhs->alpha, _state);
    
    /*
     * Compute products H*x, A*x, A^T*y
     * We compute these products in one location for the sake of simplicity.
     */
    vipmsolver_vipmmultiply(state, &v0->x, &v0->y, &state->tmphx, &state->tmpax, &state->tmpaty, _state);
    
    /*
     * Compute right-hand side:
     * Rho      = b - A*x + w
     * Nu       = l - x + g
     * Tau      = u - x - t
     * Alpha    = r - w - p
     * Sigma    = c - A^T*y - z + s + (H+REG)*x
     * Beta     = y + q - v
     */
    for(i=0; i<=m-1; i++)
    {
        rhs->rho.ptr.p_double[i] = state->b.ptr.p_double[i]-state->tmpax.ptr.p_double[i]-reg*v0->y.ptr.p_double[i];
        if( state->haswv.ptr.p_bool[i] )
        {
            
            /*
             * Inequality/range constraint
             */
            rhs->rho.ptr.p_double[i] = rhs->rho.ptr.p_double[i]+v0->w.ptr.p_double[i];
        }
        else
        {
            
            /*
             * Equality constraint without slack variables, W[i]=0
             */
            ae_assert(v0->w.ptr.p_double[i]==0.0, "RhsCompute: W[i]<>0 for linear equality constraint", _state);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            
            /*
             * Lower bound is present
             */
            rhs->nu.ptr.p_double[i] = state->bndl.ptr.p_double[i]-v0->x.ptr.p_double[i]+v0->g.ptr.p_double[i]-reg*v0->z.ptr.p_double[i];
        }
        else
        {
            
            /*
             * Lower bound is absent, g[i] = 0
             */
            ae_assert(v0->g.ptr.p_double[i]==0.0, "RhsCompute: G[i]<>0 for absent constraint", _state);
            rhs->nu.ptr.p_double[i] = (double)(0);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( state->hasts.ptr.p_bool[i] )
        {
            
            /*
             * Upper bound is present
             */
            rhs->tau.ptr.p_double[i] = state->bndu.ptr.p_double[i]-v0->x.ptr.p_double[i]-v0->t.ptr.p_double[i]+reg*v0->s.ptr.p_double[i];
        }
        else
        {
            
            /*
             * Upper bound is absent, t[i] = 0
             */
            ae_assert(v0->t.ptr.p_double[i]==0.0, "RhsCompute: T[i]<>0 for absent constraint", _state);
            rhs->tau.ptr.p_double[i] = (double)(0);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haspq.ptr.p_bool[i] )
        {
            rhs->alpha.ptr.p_double[i] = state->r.ptr.p_double[i]-v0->w.ptr.p_double[i]-v0->p.ptr.p_double[i]+reg*v0->q.ptr.p_double[i];
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( !state->isfrozen.ptr.p_bool[i] )
        {
            rhs->sigma.ptr.p_double[i] = state->c.ptr.p_double[i]-state->tmpaty.ptr.p_double[i]+state->tmphx.ptr.p_double[i]+reg*v0->x.ptr.p_double[i];
            if( state->hasgz.ptr.p_bool[i] )
            {
                rhs->sigma.ptr.p_double[i] = rhs->sigma.ptr.p_double[i]-v0->z.ptr.p_double[i];
            }
            if( state->hasts.ptr.p_bool[i] )
            {
                rhs->sigma.ptr.p_double[i] = rhs->sigma.ptr.p_double[i]+v0->s.ptr.p_double[i];
            }
        }
        else
        {
            rhs->sigma.ptr.p_double[i] = (double)(0);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haswv.ptr.p_bool[i] )
        {
            rhs->beta.ptr.p_double[i] = rhs->beta.ptr.p_double[i]+v0->y.ptr.p_double[i]-v0->v.ptr.p_double[i]+reg*v0->w.ptr.p_double[i];
        }
        if( state->haspq.ptr.p_bool[i] )
        {
            rhs->beta.ptr.p_double[i] = rhs->beta.ptr.p_double[i]+v0->q.ptr.p_double[i];
        }
    }
    
    /*
     * Compute right-hand side:
     * GammaZ   = mu*inv(G)*e - z - inv(G)*DELTAG*deltaZ
     * GammaW   = mu*inv(V)*e - w - inv(V)*DELTAV*deltaW
     * GammaS   = mu*inv(T)*e - s - inv(T)*DELTAT*deltaS
     * GammaQ   = mu*inv(P)*e - q - inv(P)*DELTAP*deltaQ
     */
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            ae_assert(v0->g.ptr.p_double[i]>0.0, "RhsCompute: G[i]<=0", _state);
            rhs->gammaz.ptr.p_double[i] = muestimate/v0->g.ptr.p_double[i]-v0->z.ptr.p_double[i]-direstimate->g.ptr.p_double[i]*direstimate->z.ptr.p_double[i]/v0->g.ptr.p_double[i];
        }
        else
        {
            ae_assert(v0->g.ptr.p_double[i]==0.0, "RhsCompute: G[i]<>0 for absent constraint", _state);
            ae_assert(v0->z.ptr.p_double[i]==0.0, "RhsCompute: Z[i]<>0 for absent constraint", _state);
            rhs->gammaz.ptr.p_double[i] = (double)(0);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haswv.ptr.p_bool[i] )
        {
            
            /*
             * Inequality/range constraint
             */
            ae_assert(v0->v.ptr.p_double[i]>0.0, "RhsCompute: V[i]<=0", _state);
            rhs->gammaw.ptr.p_double[i] = muestimate/v0->v.ptr.p_double[i]-v0->w.ptr.p_double[i]-direstimate->v.ptr.p_double[i]*direstimate->w.ptr.p_double[i]/v0->v.ptr.p_double[i];
        }
        else
        {
            
            /*
             * Equality constraint
             */
            ae_assert(v0->v.ptr.p_double[i]==0.0, "RhsCompute: V[i]<>0 for equality constraint", _state);
            ae_assert(v0->w.ptr.p_double[i]==0.0, "RhsCompute: W[i]<>0 for equality constraint", _state);
            rhs->gammaw.ptr.p_double[i] = (double)(0);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( state->hasts.ptr.p_bool[i] )
        {
            
            /*
             * Upper bound is present
             */
            ae_assert(v0->t.ptr.p_double[i]>0.0, "RhsCompute: T[i]<=0", _state);
            rhs->gammas.ptr.p_double[i] = muestimate/v0->t.ptr.p_double[i]-v0->s.ptr.p_double[i]-direstimate->t.ptr.p_double[i]*direstimate->s.ptr.p_double[i]/v0->t.ptr.p_double[i];
        }
        else
        {
            
            /*
             * Upper bound is absent
             */
            ae_assert(v0->t.ptr.p_double[i]==0.0, "RhsCompute: T[i]<>0 for absent constraint", _state);
            ae_assert(v0->s.ptr.p_double[i]==0.0, "RhsCompute: S[i]<>0 for absent constraint", _state);
            rhs->gammas.ptr.p_double[i] = (double)(0);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haspq.ptr.p_bool[i] )
        {
            ae_assert(v0->p.ptr.p_double[i]>0.0, "RhsCompute: P[i]<=0", _state);
            rhs->gammaq.ptr.p_double[i] = muestimate/v0->p.ptr.p_double[i]-v0->q.ptr.p_double[i]-direstimate->p.ptr.p_double[i]*direstimate->q.ptr.p_double[i]/v0->p.ptr.p_double[i];
        }
        else
        {
            ae_assert(v0->p.ptr.p_double[i]==0.0, "RhsCompute: P[i]<>0 for absent range", _state);
            ae_assert(v0->q.ptr.p_double[i]==0.0, "RhsCompute: Q[i]<>0 for absent range", _state);
            rhs->gammaq.ptr.p_double[i] = (double)(0);
        }
    }
}


/*************************************************************************
Subtracts KKT*cand from already computed RHS.

A pair of RhsCompute/RhsSubtract calls results in  residual  being  loaded
into the RHS structure.

INPUT PARAMETERS:
    State           -   IPM state
    V0              -   current point (used to compute RHS)
    MuEstimate      -   estimate of Mu (can be zero)
    DirEstimate     -   estimate of delta's (can be zero)
    ResidualFrom    -   whether we want to compute RHS or residual computed
                        using VDCandidate
    VDCandidate     -   solution candidate
    
OUTPUT PARAMETERS:
    Rhs             -   either RHS or residual RHS-KKT*Cand

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
static void vipmsolver_rhssubtract(vipmstate* state,
     vipmrighthandside* rhs,
     const vipmvars* v0,
     const vipmvars* vdcandidate,
     double reg,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;


    n = state->n;
    m = state->mdense+state->msparse;
    vipmsolver_vipmmultiply(state, &vdcandidate->x, &vdcandidate->y, &state->tmphx, &state->tmpax, &state->tmpaty, _state);
    
    /*
     * Residual for Rho, Nu, Tau, Alpha, Sigma, Beta
     */
    for(i=0; i<=m-1; i++)
    {
        if( state->haswv.ptr.p_bool[i] )
        {
            rhs->rho.ptr.p_double[i] = rhs->rho.ptr.p_double[i]-(state->tmpax.ptr.p_double[i]-vdcandidate->w.ptr.p_double[i]+reg*vdcandidate->y.ptr.p_double[i]);
        }
        else
        {
            rhs->rho.ptr.p_double[i] = rhs->rho.ptr.p_double[i]-(state->tmpax.ptr.p_double[i]+reg*vdcandidate->y.ptr.p_double[i]);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            rhs->nu.ptr.p_double[i] = rhs->nu.ptr.p_double[i]-(vdcandidate->x.ptr.p_double[i]-vdcandidate->g.ptr.p_double[i]+reg*vdcandidate->z.ptr.p_double[i]);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( state->hasts.ptr.p_bool[i] )
        {
            rhs->tau.ptr.p_double[i] = rhs->tau.ptr.p_double[i]-(vdcandidate->x.ptr.p_double[i]+vdcandidate->t.ptr.p_double[i]-reg*vdcandidate->s.ptr.p_double[i]);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haspq.ptr.p_bool[i] )
        {
            rhs->alpha.ptr.p_double[i] = rhs->alpha.ptr.p_double[i]-(vdcandidate->w.ptr.p_double[i]+vdcandidate->p.ptr.p_double[i]-reg*vdcandidate->q.ptr.p_double[i]);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( !state->isfrozen.ptr.p_bool[i] )
        {
            rhs->sigma.ptr.p_double[i] = rhs->sigma.ptr.p_double[i]-(state->tmpaty.ptr.p_double[i]-state->tmphx.ptr.p_double[i]-reg*vdcandidate->x.ptr.p_double[i]);
            if( state->hasgz.ptr.p_bool[i] )
            {
                rhs->sigma.ptr.p_double[i] = rhs->sigma.ptr.p_double[i]-vdcandidate->z.ptr.p_double[i];
            }
            if( state->hasts.ptr.p_bool[i] )
            {
                rhs->sigma.ptr.p_double[i] = rhs->sigma.ptr.p_double[i]+vdcandidate->s.ptr.p_double[i];
            }
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haswv.ptr.p_bool[i] )
        {
            rhs->beta.ptr.p_double[i] = rhs->beta.ptr.p_double[i]-(-vdcandidate->y.ptr.p_double[i]+vdcandidate->v.ptr.p_double[i]-reg*vdcandidate->w.ptr.p_double[i]);
        }
        if( state->haspq.ptr.p_bool[i] )
        {
            rhs->beta.ptr.p_double[i] = rhs->beta.ptr.p_double[i]+vdcandidate->q.ptr.p_double[i];
        }
    }
    
    /*
     * Residual for GammaZ, GammaW, GammaS, GammaQ
     */
    for(i=0; i<=n-1; i++)
    {
        if( state->hasgz.ptr.p_bool[i] )
        {
            rhs->gammaz.ptr.p_double[i] = rhs->gammaz.ptr.p_double[i]-(v0->z.ptr.p_double[i]/v0->g.ptr.p_double[i]*vdcandidate->g.ptr.p_double[i]+vdcandidate->z.ptr.p_double[i]);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haswv.ptr.p_bool[i] )
        {
            rhs->gammaw.ptr.p_double[i] = rhs->gammaw.ptr.p_double[i]-(v0->w.ptr.p_double[i]/v0->v.ptr.p_double[i]*vdcandidate->v.ptr.p_double[i]+vdcandidate->w.ptr.p_double[i]);
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( state->hasts.ptr.p_bool[i] )
        {
            rhs->gammas.ptr.p_double[i] = rhs->gammas.ptr.p_double[i]-(v0->s.ptr.p_double[i]/v0->t.ptr.p_double[i]*vdcandidate->t.ptr.p_double[i]+vdcandidate->s.ptr.p_double[i]);
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( state->haspq.ptr.p_bool[i] )
        {
            rhs->gammaq.ptr.p_double[i] = rhs->gammaq.ptr.p_double[i]-(v0->q.ptr.p_double[i]/v0->p.ptr.p_double[i]*vdcandidate->p.ptr.p_double[i]+vdcandidate->q.ptr.p_double[i]);
        }
    }
}


/*************************************************************************
Computes sum of squared primal terms of RHS

INPUT PARAMETERS:
    Rhs             -   RHS structure
    N, M            -   problem metrics
    
RESULT:
    sum(sqr()) computed over primal terms (Rho, Nu, Tau, Alpha)

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double vipmsolver_rhsprimal2(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state)
{
    double result;


    result = (double)(0);
    result = result+rdotv2(m, &rhs->rho, _state);
    result = result+rdotv2(n, &rhs->nu, _state);
    result = result+rdotv2(n, &rhs->tau, _state);
    result = result+rdotv2(m, &rhs->alpha, _state);
    return result;
}


/*************************************************************************
Computes sum of squared dual terms of RHS

INPUT PARAMETERS:
    Rhs             -   RHS structure
    N, M            -   problem metrics
    
RESULT:
    sum(sqr()) computed over dual terms (Sigma, Beta)

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double vipmsolver_rhsdual2(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state)
{
    double result;


    result = (double)(0);
    result = result+rdotv2(n, &rhs->sigma, _state);
    result = result+rdotv2(m, &rhs->beta, _state);
    return result;
}


/*************************************************************************
Computes inf-norm of primal terms of RHS

INPUT PARAMETERS:
    Rhs             -   RHS structure
    N, M            -   problem metrics
    
RESULT:
    max(abs()) computed over primal terms (Rho, Nu, Tau, Alpha)

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double vipmsolver_rhsprimalinf(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state)
{
    double result;


    result = (double)(0);
    result = ae_maxreal(result, rmaxabsv(m, &rhs->rho, _state), _state);
    result = ae_maxreal(result, rmaxabsv(n, &rhs->nu, _state), _state);
    result = ae_maxreal(result, rmaxabsv(n, &rhs->tau, _state), _state);
    result = ae_maxreal(result, rmaxabsv(m, &rhs->alpha, _state), _state);
    return result;
}


/*************************************************************************
Computes inf-norm of dual terms of RHS

INPUT PARAMETERS:
    Rhs             -   RHS structure
    N, M            -   problem metrics
    
RESULT:
    max(abs()) computed over dual terms (Sigma, Beta)

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double vipmsolver_rhsdualinf(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state)
{
    double result;


    result = (double)(0);
    result = ae_maxreal(result, rmaxabsv(n, &rhs->sigma, _state), _state);
    result = ae_maxreal(result, rmaxabsv(m, &rhs->beta, _state), _state);
    return result;
}


/*************************************************************************
Computes maximum over complementarity slackness terms of RHS

INPUT PARAMETERS:
    Rhs             -   RHS structure
    N, M            -   problem metrics
    
RESULT:
    max(abs()) computed over complementarity terms (GammaZ, GammaS, GammaW, GammaQ)

  -- ALGLIB --
     Copyright 01.11.2020 by Bochkanov Sergey
*************************************************************************/
static double vipmsolver_rhscompl2(const vipmrighthandside* rhs,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state)
{
    double result;


    result = (double)(0);
    result = result+rdotv2(n, &rhs->gammaz, _state);
    result = result+rdotv2(n, &rhs->gammas, _state);
    result = result+rdotv2(m, &rhs->gammaw, _state);
    result = result+rdotv2(m, &rhs->gammaq, _state);
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
static double vipmsolver_minnz(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool nz;
    double result;


    result = (double)(0);
    nz = ae_false;
    for(i=0; i<=n-1; i++)
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
static double vipmsolver_minprodnz(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool nz;
    double result;


    result = (double)(0);
    nz = ae_false;
    for(i=0; i<=n-1; i++)
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
static double vipmsolver_maxprodnz(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_int_t n,
     ae_state *_state)
{
    ae_int_t i;
    ae_bool nz;
    double result;


    result = (double)(0);
    nz = ae_false;
    for(i=0; i<=n-1; i++)
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


void _vipmvars_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    vipmvars *p = (vipmvars*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->g, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->w, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->t, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->p, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->z, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->v, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->q, 0, DT_REAL, _state, make_automatic);
}


void _vipmvars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    vipmvars       *dst = (vipmvars*)_dst;
    const vipmvars *src = (const vipmvars*)_src;
    dst->n = src->n;
    dst->m = src->m;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->g, &src->g, _state, make_automatic);
    ae_vector_init_copy(&dst->w, &src->w, _state, make_automatic);
    ae_vector_init_copy(&dst->t, &src->t, _state, make_automatic);
    ae_vector_init_copy(&dst->p, &src->p, _state, make_automatic);
    ae_vector_init_copy(&dst->y, &src->y, _state, make_automatic);
    ae_vector_init_copy(&dst->z, &src->z, _state, make_automatic);
    ae_vector_init_copy(&dst->v, &src->v, _state, make_automatic);
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->q, &src->q, _state, make_automatic);
}


void _vipmvars_clear(void* _p)
{
    vipmvars *p = (vipmvars*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->g);
    ae_vector_clear(&p->w);
    ae_vector_clear(&p->t);
    ae_vector_clear(&p->p);
    ae_vector_clear(&p->y);
    ae_vector_clear(&p->z);
    ae_vector_clear(&p->v);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->q);
}


void _vipmvars_destroy(void* _p)
{
    vipmvars *p = (vipmvars*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->g);
    ae_vector_destroy(&p->w);
    ae_vector_destroy(&p->t);
    ae_vector_destroy(&p->p);
    ae_vector_destroy(&p->y);
    ae_vector_destroy(&p->z);
    ae_vector_destroy(&p->v);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->q);
}


void _vipmreducedsparsesystem_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    vipmreducedsparsesystem *p = (vipmreducedsparsesystem*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_init(&p->rawsystem, _state, make_automatic);
    ae_vector_init(&p->effectivediag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->isdiagonal, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->rowdegrees, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->coldegrees, 0, DT_INT, _state, make_automatic);
    _spcholanalysis_init(&p->analysis, _state, make_automatic);
    ae_vector_init(&p->priorities, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->diagterm, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->dampterm, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmprhs, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpcorr, 0, DT_REAL, _state, make_automatic);
}


void _vipmreducedsparsesystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    vipmreducedsparsesystem       *dst = (vipmreducedsparsesystem*)_dst;
    const vipmreducedsparsesystem *src = (const vipmreducedsparsesystem*)_src;
    _sparsematrix_init_copy(&dst->rawsystem, &src->rawsystem, _state, make_automatic);
    ae_vector_init_copy(&dst->effectivediag, &src->effectivediag, _state, make_automatic);
    ae_vector_init_copy(&dst->isdiagonal, &src->isdiagonal, _state, make_automatic);
    ae_vector_init_copy(&dst->rowdegrees, &src->rowdegrees, _state, make_automatic);
    ae_vector_init_copy(&dst->coldegrees, &src->coldegrees, _state, make_automatic);
    dst->ntotal = src->ntotal;
    _spcholanalysis_init_copy(&dst->analysis, &src->analysis, _state, make_automatic);
    ae_vector_init_copy(&dst->priorities, &src->priorities, _state, make_automatic);
    ae_vector_init_copy(&dst->diagterm, &src->diagterm, _state, make_automatic);
    ae_vector_init_copy(&dst->dampterm, &src->dampterm, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpb, &src->tmpb, _state, make_automatic);
    ae_vector_init_copy(&dst->tmprhs, &src->tmprhs, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpcorr, &src->tmpcorr, _state, make_automatic);
}


void _vipmreducedsparsesystem_clear(void* _p)
{
    vipmreducedsparsesystem *p = (vipmreducedsparsesystem*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_clear(&p->rawsystem);
    ae_vector_clear(&p->effectivediag);
    ae_vector_clear(&p->isdiagonal);
    ae_vector_clear(&p->rowdegrees);
    ae_vector_clear(&p->coldegrees);
    _spcholanalysis_clear(&p->analysis);
    ae_vector_clear(&p->priorities);
    ae_vector_clear(&p->diagterm);
    ae_vector_clear(&p->dampterm);
    ae_vector_clear(&p->tmpb);
    ae_vector_clear(&p->tmprhs);
    ae_vector_clear(&p->tmpcorr);
}


void _vipmreducedsparsesystem_destroy(void* _p)
{
    vipmreducedsparsesystem *p = (vipmreducedsparsesystem*)_p;
    ae_touch_ptr((void*)p);
    _sparsematrix_destroy(&p->rawsystem);
    ae_vector_destroy(&p->effectivediag);
    ae_vector_destroy(&p->isdiagonal);
    ae_vector_destroy(&p->rowdegrees);
    ae_vector_destroy(&p->coldegrees);
    _spcholanalysis_destroy(&p->analysis);
    ae_vector_destroy(&p->priorities);
    ae_vector_destroy(&p->diagterm);
    ae_vector_destroy(&p->dampterm);
    ae_vector_destroy(&p->tmpb);
    ae_vector_destroy(&p->tmprhs);
    ae_vector_destroy(&p->tmpcorr);
}


void _vipmrighthandside_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    vipmrighthandside *p = (vipmrighthandside*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->sigma, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->beta, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rho, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tau, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->alpha, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gammaz, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gammas, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gammaw, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->gammaq, 0, DT_REAL, _state, make_automatic);
}


void _vipmrighthandside_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    vipmrighthandside       *dst = (vipmrighthandside*)_dst;
    const vipmrighthandside *src = (const vipmrighthandside*)_src;
    ae_vector_init_copy(&dst->sigma, &src->sigma, _state, make_automatic);
    ae_vector_init_copy(&dst->beta, &src->beta, _state, make_automatic);
    ae_vector_init_copy(&dst->rho, &src->rho, _state, make_automatic);
    ae_vector_init_copy(&dst->nu, &src->nu, _state, make_automatic);
    ae_vector_init_copy(&dst->tau, &src->tau, _state, make_automatic);
    ae_vector_init_copy(&dst->alpha, &src->alpha, _state, make_automatic);
    ae_vector_init_copy(&dst->gammaz, &src->gammaz, _state, make_automatic);
    ae_vector_init_copy(&dst->gammas, &src->gammas, _state, make_automatic);
    ae_vector_init_copy(&dst->gammaw, &src->gammaw, _state, make_automatic);
    ae_vector_init_copy(&dst->gammaq, &src->gammaq, _state, make_automatic);
}


void _vipmrighthandside_clear(void* _p)
{
    vipmrighthandside *p = (vipmrighthandside*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->sigma);
    ae_vector_clear(&p->beta);
    ae_vector_clear(&p->rho);
    ae_vector_clear(&p->nu);
    ae_vector_clear(&p->tau);
    ae_vector_clear(&p->alpha);
    ae_vector_clear(&p->gammaz);
    ae_vector_clear(&p->gammas);
    ae_vector_clear(&p->gammaw);
    ae_vector_clear(&p->gammaq);
}


void _vipmrighthandside_destroy(void* _p)
{
    vipmrighthandside *p = (vipmrighthandside*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->sigma);
    ae_vector_destroy(&p->beta);
    ae_vector_destroy(&p->rho);
    ae_vector_destroy(&p->nu);
    ae_vector_destroy(&p->tau);
    ae_vector_destroy(&p->alpha);
    ae_vector_destroy(&p->gammaz);
    ae_vector_destroy(&p->gammas);
    ae_vector_destroy(&p->gammaw);
    ae_vector_destroy(&p->gammaq);
}


void _vipmstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    vipmstate *p = (vipmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->scl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->invscl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xorigin, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->c, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->denseh, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparseh, _state, make_automatic);
    ae_vector_init(&p->diagr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_matrix_init(&p->denseafull, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->denseamain, 0, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparseafull, _state, make_automatic);
    _sparsematrix_init(&p->sparseamain, _state, make_automatic);
    _sparsematrix_init(&p->combinedaslack, _state, make_automatic);
    ae_vector_init(&p->ascales, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->aflips, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasr, 0, DT_BOOL, _state, make_automatic);
    _vipmvars_init(&p->current, _state, make_automatic);
    _vipmvars_init(&p->best, _state, make_automatic);
    _vipmvars_init(&p->trial, _state, make_automatic);
    _vipmvars_init(&p->deltaaff, _state, make_automatic);
    _vipmvars_init(&p->deltacorr, _state, make_automatic);
    ae_vector_init(&p->isfrozen, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasgz, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasts, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->haswv, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->haspq, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->diagdz, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdzi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdziri, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagds, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdsi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdsiri, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdw, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdwi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdwir, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdq, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdqi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagdqiri, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagddr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagde, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->diagder, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->factdensehaug, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->factregdhrh, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->factinvregdzrz, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->factregewave, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->facttmpdiag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->facttmpdamp, 0, DT_REAL, _state, make_automatic);
    _vipmreducedsparsesystem_init(&p->reducedsparsesystem, _state, make_automatic);
    _vipmrighthandside_init(&p->rhs, _state, make_automatic);
    ae_vector_init(&p->rhsalphacap, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rhsbetacap, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rhsnucap, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rhstaucap, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->deltaxy, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmphx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpaty, 0, DT_REAL, _state, make_automatic);
    _vipmvars_init(&p->zerovars, _state, make_automatic);
    ae_vector_init(&p->dummyr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpy, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp2, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpr2, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmplaggrad, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpi, 0, DT_INT, _state, make_automatic);
    _sparsematrix_init(&p->tmpsparse0, _state, make_automatic);
}


void _vipmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    vipmstate       *dst = (vipmstate*)_dst;
    const vipmstate *src = (const vipmstate*)_src;
    dst->slacksforequalityconstraints = src->slacksforequalityconstraints;
    dst->n = src->n;
    dst->nmain = src->nmain;
    dst->epsp = src->epsp;
    dst->epsd = src->epsd;
    dst->epsgap = src->epsgap;
    dst->islinear = src->islinear;
    ae_vector_init_copy(&dst->scl, &src->scl, _state, make_automatic);
    ae_vector_init_copy(&dst->invscl, &src->invscl, _state, make_automatic);
    ae_vector_init_copy(&dst->xorigin, &src->xorigin, _state, make_automatic);
    dst->targetscale = src->targetscale;
    ae_vector_init_copy(&dst->c, &src->c, _state, make_automatic);
    ae_matrix_init_copy(&dst->denseh, &src->denseh, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseh, &src->sparseh, _state, make_automatic);
    ae_vector_init_copy(&dst->diagr, &src->diagr, _state, make_automatic);
    dst->hkind = src->hkind;
    dst->isdiagonalh = src->isdiagonalh;
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    ae_vector_init_copy(&dst->rawbndl, &src->rawbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->rawbndu, &src->rawbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_matrix_init_copy(&dst->denseafull, &src->denseafull, _state, make_automatic);
    ae_matrix_init_copy(&dst->denseamain, &src->denseamain, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseafull, &src->sparseafull, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparseamain, &src->sparseamain, _state, make_automatic);
    _sparsematrix_init_copy(&dst->combinedaslack, &src->combinedaslack, _state, make_automatic);
    ae_vector_init_copy(&dst->ascales, &src->ascales, _state, make_automatic);
    ae_vector_init_copy(&dst->aflips, &src->aflips, _state, make_automatic);
    ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic);
    ae_vector_init_copy(&dst->r, &src->r, _state, make_automatic);
    ae_vector_init_copy(&dst->hasr, &src->hasr, _state, make_automatic);
    dst->mdense = src->mdense;
    dst->msparse = src->msparse;
    _vipmvars_init_copy(&dst->current, &src->current, _state, make_automatic);
    _vipmvars_init_copy(&dst->best, &src->best, _state, make_automatic);
    _vipmvars_init_copy(&dst->trial, &src->trial, _state, make_automatic);
    _vipmvars_init_copy(&dst->deltaaff, &src->deltaaff, _state, make_automatic);
    _vipmvars_init_copy(&dst->deltacorr, &src->deltacorr, _state, make_automatic);
    ae_vector_init_copy(&dst->isfrozen, &src->isfrozen, _state, make_automatic);
    ae_vector_init_copy(&dst->hasgz, &src->hasgz, _state, make_automatic);
    ae_vector_init_copy(&dst->hasts, &src->hasts, _state, make_automatic);
    ae_vector_init_copy(&dst->haswv, &src->haswv, _state, make_automatic);
    ae_vector_init_copy(&dst->haspq, &src->haspq, _state, make_automatic);
    dst->cntgz = src->cntgz;
    dst->cntts = src->cntts;
    dst->cntwv = src->cntwv;
    dst->cntpq = src->cntpq;
    dst->repiterationscount = src->repiterationscount;
    dst->repncholesky = src->repncholesky;
    dst->dotrace = src->dotrace;
    dst->dodetailedtrace = src->dodetailedtrace;
    dst->factorizationtype = src->factorizationtype;
    dst->factorizationpoweredup = src->factorizationpoweredup;
    dst->factorizationpresent = src->factorizationpresent;
    ae_vector_init_copy(&dst->diagdz, &src->diagdz, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdzi, &src->diagdzi, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdziri, &src->diagdziri, _state, make_automatic);
    ae_vector_init_copy(&dst->diagds, &src->diagds, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdsi, &src->diagdsi, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdsiri, &src->diagdsiri, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdw, &src->diagdw, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdwi, &src->diagdwi, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdwir, &src->diagdwir, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdq, &src->diagdq, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdqi, &src->diagdqi, _state, make_automatic);
    ae_vector_init_copy(&dst->diagdqiri, &src->diagdqiri, _state, make_automatic);
    ae_vector_init_copy(&dst->diagddr, &src->diagddr, _state, make_automatic);
    ae_vector_init_copy(&dst->diagde, &src->diagde, _state, make_automatic);
    ae_vector_init_copy(&dst->diagder, &src->diagder, _state, make_automatic);
    ae_matrix_init_copy(&dst->factdensehaug, &src->factdensehaug, _state, make_automatic);
    ae_vector_init_copy(&dst->factregdhrh, &src->factregdhrh, _state, make_automatic);
    ae_vector_init_copy(&dst->factinvregdzrz, &src->factinvregdzrz, _state, make_automatic);
    ae_vector_init_copy(&dst->factregewave, &src->factregewave, _state, make_automatic);
    ae_vector_init_copy(&dst->facttmpdiag, &src->facttmpdiag, _state, make_automatic);
    ae_vector_init_copy(&dst->facttmpdamp, &src->facttmpdamp, _state, make_automatic);
    _vipmreducedsparsesystem_init_copy(&dst->reducedsparsesystem, &src->reducedsparsesystem, _state, make_automatic);
    _vipmrighthandside_init_copy(&dst->rhs, &src->rhs, _state, make_automatic);
    ae_vector_init_copy(&dst->rhsalphacap, &src->rhsalphacap, _state, make_automatic);
    ae_vector_init_copy(&dst->rhsbetacap, &src->rhsbetacap, _state, make_automatic);
    ae_vector_init_copy(&dst->rhsnucap, &src->rhsnucap, _state, make_automatic);
    ae_vector_init_copy(&dst->rhstaucap, &src->rhstaucap, _state, make_automatic);
    ae_vector_init_copy(&dst->deltaxy, &src->deltaxy, _state, make_automatic);
    ae_vector_init_copy(&dst->tmphx, &src->tmphx, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpax, &src->tmpax, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpaty, &src->tmpaty, _state, make_automatic);
    _vipmvars_init_copy(&dst->zerovars, &src->zerovars, _state, make_automatic);
    ae_vector_init_copy(&dst->dummyr, &src->dummyr, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpy, &src->tmpy, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp1, &src->tmp1, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmpr2, &src->tmpr2, _state, make_automatic);
    ae_vector_init_copy(&dst->tmplaggrad, &src->tmplaggrad, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpi, &src->tmpi, _state, make_automatic);
    _sparsematrix_init_copy(&dst->tmpsparse0, &src->tmpsparse0, _state, make_automatic);
}


void _vipmstate_clear(void* _p)
{
    vipmstate *p = (vipmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->scl);
    ae_vector_clear(&p->invscl);
    ae_vector_clear(&p->xorigin);
    ae_vector_clear(&p->c);
    ae_matrix_clear(&p->denseh);
    _sparsematrix_clear(&p->sparseh);
    ae_vector_clear(&p->diagr);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    ae_vector_clear(&p->rawbndl);
    ae_vector_clear(&p->rawbndu);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_matrix_clear(&p->denseafull);
    ae_matrix_clear(&p->denseamain);
    _sparsematrix_clear(&p->sparseafull);
    _sparsematrix_clear(&p->sparseamain);
    _sparsematrix_clear(&p->combinedaslack);
    ae_vector_clear(&p->ascales);
    ae_vector_clear(&p->aflips);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->r);
    ae_vector_clear(&p->hasr);
    _vipmvars_clear(&p->current);
    _vipmvars_clear(&p->best);
    _vipmvars_clear(&p->trial);
    _vipmvars_clear(&p->deltaaff);
    _vipmvars_clear(&p->deltacorr);
    ae_vector_clear(&p->isfrozen);
    ae_vector_clear(&p->hasgz);
    ae_vector_clear(&p->hasts);
    ae_vector_clear(&p->haswv);
    ae_vector_clear(&p->haspq);
    ae_vector_clear(&p->diagdz);
    ae_vector_clear(&p->diagdzi);
    ae_vector_clear(&p->diagdziri);
    ae_vector_clear(&p->diagds);
    ae_vector_clear(&p->diagdsi);
    ae_vector_clear(&p->diagdsiri);
    ae_vector_clear(&p->diagdw);
    ae_vector_clear(&p->diagdwi);
    ae_vector_clear(&p->diagdwir);
    ae_vector_clear(&p->diagdq);
    ae_vector_clear(&p->diagdqi);
    ae_vector_clear(&p->diagdqiri);
    ae_vector_clear(&p->diagddr);
    ae_vector_clear(&p->diagde);
    ae_vector_clear(&p->diagder);
    ae_matrix_clear(&p->factdensehaug);
    ae_vector_clear(&p->factregdhrh);
    ae_vector_clear(&p->factinvregdzrz);
    ae_vector_clear(&p->factregewave);
    ae_vector_clear(&p->facttmpdiag);
    ae_vector_clear(&p->facttmpdamp);
    _vipmreducedsparsesystem_clear(&p->reducedsparsesystem);
    _vipmrighthandside_clear(&p->rhs);
    ae_vector_clear(&p->rhsalphacap);
    ae_vector_clear(&p->rhsbetacap);
    ae_vector_clear(&p->rhsnucap);
    ae_vector_clear(&p->rhstaucap);
    ae_vector_clear(&p->deltaxy);
    ae_vector_clear(&p->tmphx);
    ae_vector_clear(&p->tmpax);
    ae_vector_clear(&p->tmpaty);
    _vipmvars_clear(&p->zerovars);
    ae_vector_clear(&p->dummyr);
    ae_vector_clear(&p->tmpy);
    ae_vector_clear(&p->tmp0);
    ae_vector_clear(&p->tmp1);
    ae_vector_clear(&p->tmp2);
    ae_matrix_clear(&p->tmpr2);
    ae_vector_clear(&p->tmplaggrad);
    ae_vector_clear(&p->tmpi);
    _sparsematrix_clear(&p->tmpsparse0);
}


void _vipmstate_destroy(void* _p)
{
    vipmstate *p = (vipmstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->scl);
    ae_vector_destroy(&p->invscl);
    ae_vector_destroy(&p->xorigin);
    ae_vector_destroy(&p->c);
    ae_matrix_destroy(&p->denseh);
    _sparsematrix_destroy(&p->sparseh);
    ae_vector_destroy(&p->diagr);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    ae_vector_destroy(&p->rawbndl);
    ae_vector_destroy(&p->rawbndu);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_matrix_destroy(&p->denseafull);
    ae_matrix_destroy(&p->denseamain);
    _sparsematrix_destroy(&p->sparseafull);
    _sparsematrix_destroy(&p->sparseamain);
    _sparsematrix_destroy(&p->combinedaslack);
    ae_vector_destroy(&p->ascales);
    ae_vector_destroy(&p->aflips);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->r);
    ae_vector_destroy(&p->hasr);
    _vipmvars_destroy(&p->current);
    _vipmvars_destroy(&p->best);
    _vipmvars_destroy(&p->trial);
    _vipmvars_destroy(&p->deltaaff);
    _vipmvars_destroy(&p->deltacorr);
    ae_vector_destroy(&p->isfrozen);
    ae_vector_destroy(&p->hasgz);
    ae_vector_destroy(&p->hasts);
    ae_vector_destroy(&p->haswv);
    ae_vector_destroy(&p->haspq);
    ae_vector_destroy(&p->diagdz);
    ae_vector_destroy(&p->diagdzi);
    ae_vector_destroy(&p->diagdziri);
    ae_vector_destroy(&p->diagds);
    ae_vector_destroy(&p->diagdsi);
    ae_vector_destroy(&p->diagdsiri);
    ae_vector_destroy(&p->diagdw);
    ae_vector_destroy(&p->diagdwi);
    ae_vector_destroy(&p->diagdwir);
    ae_vector_destroy(&p->diagdq);
    ae_vector_destroy(&p->diagdqi);
    ae_vector_destroy(&p->diagdqiri);
    ae_vector_destroy(&p->diagddr);
    ae_vector_destroy(&p->diagde);
    ae_vector_destroy(&p->diagder);
    ae_matrix_destroy(&p->factdensehaug);
    ae_vector_destroy(&p->factregdhrh);
    ae_vector_destroy(&p->factinvregdzrz);
    ae_vector_destroy(&p->factregewave);
    ae_vector_destroy(&p->facttmpdiag);
    ae_vector_destroy(&p->facttmpdamp);
    _vipmreducedsparsesystem_destroy(&p->reducedsparsesystem);
    _vipmrighthandside_destroy(&p->rhs);
    ae_vector_destroy(&p->rhsalphacap);
    ae_vector_destroy(&p->rhsbetacap);
    ae_vector_destroy(&p->rhsnucap);
    ae_vector_destroy(&p->rhstaucap);
    ae_vector_destroy(&p->deltaxy);
    ae_vector_destroy(&p->tmphx);
    ae_vector_destroy(&p->tmpax);
    ae_vector_destroy(&p->tmpaty);
    _vipmvars_destroy(&p->zerovars);
    ae_vector_destroy(&p->dummyr);
    ae_vector_destroy(&p->tmpy);
    ae_vector_destroy(&p->tmp0);
    ae_vector_destroy(&p->tmp1);
    ae_vector_destroy(&p->tmp2);
    ae_matrix_destroy(&p->tmpr2);
    ae_vector_destroy(&p->tmplaggrad);
    ae_vector_destroy(&p->tmpi);
    _sparsematrix_destroy(&p->tmpsparse0);
}


/*$ End $*/
