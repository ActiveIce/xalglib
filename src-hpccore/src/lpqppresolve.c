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
#include "lpqppresolve.h"


/*$ Declarations $*/
static void lpqppresolve_dyncrsinitfromsparsecrs(const sparsematrix* s,
     dynamiccrs* r,
     ae_state *_state);
static void lpqppresolve_dyncrsdropzeros(dynamiccrs* a, ae_state *_state);
static void lpqppresolve_dyncrsremoverow(dynamiccrs* a,
     ae_int_t rowidx,
     ae_state *_state);
static void lpqppresolve_dyncrsremovefromrow(dynamiccrs* a,
     ae_int_t rowidx,
     ae_int_t j,
     ae_state *_state);
static void lpqppresolve_dyncrsremovesetfromrow(dynamiccrs* a,
     ae_int_t rowidx,
     const niset* s,
     ae_state *_state);
static void lpqppresolve_presolverstackinit(ae_int_t n,
     ae_int_t m,
     presolverstack* s,
     ae_state *_state);
static void lpqppresolve_presolverstreamb(presolverstack* s,
     ae_bool b,
     ae_state *_state);
static void lpqppresolve_presolverstreami(presolverstack* s,
     ae_int_t i,
     ae_state *_state);
static void lpqppresolve_presolverstreamr(presolverstack* s,
     double v,
     ae_state *_state);
static void lpqppresolve_presolverstreamir(presolverstack* s,
     ae_int_t i,
     double v,
     ae_state *_state);
static void lpqppresolve_presolverstreamcrsrow(presolverstack* s,
     const dynamiccrs* a,
     ae_int_t i,
     ae_state *_state);
static void lpqppresolve_presolverselectstreamsource(presolverstack* s,
     ae_int_t tidx,
     ae_state *_state);
static void lpqppresolve_presolverunstreamb(presolverstack* s,
     ae_bool* v,
     ae_state *_state);
static void lpqppresolve_presolverunstreami(presolverstack* s,
     ae_int_t* v,
     ae_state *_state);
static void lpqppresolve_presolverunstreamr(presolverstack* s,
     double* v,
     ae_state *_state);
static void lpqppresolve_presolverunstreamir(presolverstack* s,
     ae_int_t* vi,
     double* vr,
     ae_state *_state);
static void lpqppresolve_presolverunstreamsparsevec(presolverstack* s,
     ae_int_t* cnt,
     /* Integer */ ae_vector* idx,
     /* Real    */ ae_vector* vals,
     ae_state *_state);
static void lpqppresolve_presolverasserteos(presolverstack* s,
     ae_state *_state);
static void lpqppresolve_presolverappendtrf(presolverstack* s,
     ae_int_t tt,
     ae_state *_state);
static void lpqppresolve_presolverappendcostscaling(presolverstack* s,
     double vmul,
     ae_state *_state);
static void lpqppresolve_presolverappendcolscaling(presolverstack* s,
     ae_int_t colidx,
     double vmul,
     ae_state *_state);
static void lpqppresolve_presolverappendrowscaling(presolverstack* s,
     ae_int_t rowidx,
     double vmul,
     ae_state *_state);
static void lpqppresolve_presolverappenddropemptycol(presolverstack* s,
     ae_int_t colidx,
     double varval,
     double lagval,
     ae_int_t statval,
     ae_state *_state);
static void lpqppresolve_presolverappenddropemptyrow(presolverstack* s,
     ae_int_t rowidx,
     ae_state *_state);
static void lpqppresolve_presolverappendsingletonrow(presolverstack* s,
     ae_int_t i,
     ae_int_t j,
     double v,
     double swapsign,
     double bndl,
     ae_bool bndlisbc,
     double bndu,
     ae_bool bnduisbc,
     ae_state *_state);
static void lpqppresolve_presolverappendfixedvar(presolverstack* s,
     ae_int_t colidx,
     double fixval,
     double ci,
     const dynamiccrs* at,
     ae_state *_state);
static void lpqppresolve_presolverappendexplicitslack(presolverstack* s,
     ae_int_t i,
     ae_int_t j,
     double aij,
     double slackbndl,
     double slackbndu,
     double al,
     double au,
     const dynamiccrs* a,
     ae_state *_state);
static void lpqppresolve_presolverappendimplicitslack(presolverstack* s,
     ae_int_t i,
     ae_int_t j,
     double aij,
     double cj,
     double equalitybnd,
     const dynamiccrs* a,
     ae_state *_state);
static void lpqppresolve_presolverrestoresolution(presolverstack* s,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     /* Integer */ ae_vector* stats,
     ae_state *_state);
static void lpqppresolve_presolvebuffersinit(presolvebuffers* buf,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state);
static ae_bool lpqppresolve_dropemptycol(/* Real    */ ae_vector* c,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* isdroppedcol,
     /* Boolean */ ae_vector* lagrangefromresidual,
     ae_int_t n,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntdropped,
     ae_state *_state);
static ae_bool lpqppresolve_dropclearlynonbindingrows(ae_int_t n,
     /* Boolean */ ae_vector* isdroppedrow,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntempty,
     ae_int_t* cntnobounds,
     ae_state *_state);
static ae_bool lpqppresolve_singletonrowtobc(/* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* isdroppedcol,
     ae_int_t n,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     /* Boolean */ ae_vector* isdroppedrow,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntsingleton,
     ae_state *_state);
static ae_bool lpqppresolve_singletoncols(/* Real    */ ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ ae_vector* isdroppedcol,
     ae_int_t n,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     /* Boolean */ ae_vector* isdroppedrow,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntslackvars,
     ae_int_t* cntimplicitslacks,
     ae_int_t* cntfreecolumnsingletons,
     ae_state *_state);
static ae_bool lpqppresolve_fixvariables(/* Real    */ ae_vector* c,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* isdroppedcol,
     ae_int_t n,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntfixed,
     ae_state *_state);
static void lpqppresolve_scalecostandconstraints(/* Real    */ const ae_vector* s,
     ae_int_t n,
     /* Real    */ ae_vector* c,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     sparsematrix* sparsea,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t m,
     presolverstack* trfstack,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
No presolve, just user-supplied scaling + constraint and cost vector
normalization.

INPUT PARAMETERS:
    S           -   array[N], user-supplied scale vector, S[I]>0
    C           -   array[N], costs
    BndL        -   array[N], lower bounds (may contain -INF)
    BndU        -   array[N], upper bounds (may contain +INF)
    N           -   variable count, N>0
    SparseA     -   matrix[K,N], sparse constraints
    AL          -   array[K], lower constraint bounds (may contain -INF)
    AU          -   array[K], upper constraint bounds (may contain +INF)
    K           -   constraint count, K>=0
    Info        -   presolve info structure; temporaries allocated during
                    previous calls may be reused by this function.
                    
OUTPUT PARAMETERS:
    Info        -   contains transformed C, BndL, bndU,  SparseA,  AL,  AU
                    and   information   necessary   to   perform  backward
                    transformation.
                    Following fields can be accessed:
                    * ProblemStatus    which is:
                                       *  0 for successful transformation
                                       * -3 for infeasible problem
                                       * -4 for unbounded problem
                    
                    If Info.ProblemStatus=0, then the following fields can
                    be accessed:
                    * Info.NewN>0  for transformed problem size
                    * Info.NewM>=0 for transformed constraint count
                    * always:          Info.C, Info.BndL, Info.BndU - array[NewN]
                    * for Info.NewM>0: Info.SparseA, Info.AL, Info.AU
    
NOTE: this routine does not reallocate arrays if NNew<=NOld and/or KNew<=KOld.

  -- ALGLIB --
     Copyright 01.07.2020 by Bochkanov Sergey
*************************************************************************/
void presolvenonescaleuser(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_int_t n,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_bool dotrace,
     presolveinfo* info,
     ae_state *_state)
{
    ae_int_t i;


    
    /*
     * Integrity checks
     */
    ae_assert(bndl->cnt>=n, "PresolveNoneScaleUser: Length(BndL)<N", _state);
    ae_assert(bndu->cnt>=n, "PresolveNoneScaleUser: Length(BndU)<N", _state);
    ae_assert(s->cnt>=n, "PresolveNoneScaleUser: Length(S)<N", _state);
    ae_assert(isfinitevector(s, n, _state), "PresolveNoneScaleUser: S contains infinite or NaN elements", _state);
    ae_assert(c->cnt>=n, "PresolveNoneScaleUser: Length(C)<N", _state);
    ae_assert(isfinitevector(c, n, _state), "PresolveNoneScaleUser: C contains infinite or NaN elements", _state);
    ae_assert(k>=0, "PresolveNoneScaleUser: K<0", _state);
    ae_assert(k==0||sparseiscrs(sparsea, _state), "PresolveNoneScaleUser: A is not CRS", _state);
    ae_assert(k==0||sparsea->m==k, "PresolveNoneScaleUser: rows(A)<>K", _state);
    ae_assert(k==0||sparsea->n==n, "PresolveNoneScaleUser: cols(A)<>N", _state);
    
    /*
     * Initial check for constraint feasibility
     */
    for(i=0; i<=n-1; i++)
    {
        if( (ae_isfinite(bndl->ptr.p_double[i], _state)&&ae_isfinite(bndu->ptr.p_double[i], _state))&&ae_fp_greater(bndl->ptr.p_double[i],bndu->ptr.p_double[i]) )
        {
            if( dotrace )
            {
                ae_trace("> variable %0d is found to have infeasible box constraints, terminating\n",
                    (int)(i));
            }
            info->problemstatus = -3;
            return;
        }
    }
    for(i=0; i<=k-1; i++)
    {
        if( (ae_isfinite(al->ptr.p_double[i], _state)&&ae_isfinite(au->ptr.p_double[i], _state))&&ae_fp_greater(al->ptr.p_double[i],au->ptr.p_double[i]) )
        {
            if( dotrace )
            {
                ae_trace("> linear constraint %0d is found to have infeasible bounds, terminating\n",
                    (int)(i));
            }
            info->problemstatus = -3;
            return;
        }
    }
    
    /*
     * Reallocate storage
     */
    rvectorgrowto(&info->rawc, n, _state);
    rvectorgrowto(&info->rawbndl, n, _state);
    rvectorgrowto(&info->rawbndu, n, _state);
    
    /*
     * Save original problem formulation
     */
    lpqppresolve_presolverstackinit(n, k, &info->trfstack, _state);
    info->problemstatus = 0;
    info->newn = n;
    info->oldn = n;
    info->newm = k;
    info->oldm = k;
    bsetallocv(n, ae_false, &info->lagrangefromresidual, _state);
    iallocv(n, &info->packxperm, _state);
    iallocv(n, &info->unpackxperm, _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(s->ptr.p_double[i]>(double)0, "PresolveNoneScaleUser: S<=0", _state);
        ae_assert(ae_isfinite(bndl->ptr.p_double[i], _state)||ae_isneginf(bndl->ptr.p_double[i], _state), "PresolveNoneScaleUser: BndL contains NAN or +INF", _state);
        ae_assert(ae_isfinite(bndu->ptr.p_double[i], _state)||ae_isposinf(bndu->ptr.p_double[i], _state), "PresolveNoneScaleUser: BndU contains NAN or -INF", _state);
        info->rawc.ptr.p_double[i] = c->ptr.p_double[i];
        info->rawbndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        info->rawbndu.ptr.p_double[i] = bndu->ptr.p_double[i];
        info->packxperm.ptr.p_int[i] = i;
        info->unpackxperm.ptr.p_int[i] = i;
    }
    iallocv(k, &info->packyperm, _state);
    iallocv(k, &info->unpackyperm, _state);
    for(i=0; i<=k-1; i++)
    {
        info->packyperm.ptr.p_int[i] = i;
        info->unpackyperm.ptr.p_int[i] = i;
    }
    iallocv(n+k, &info->packstatperm, _state);
    iallocv(n+k, &info->unpackstatperm, _state);
    for(i=0; i<=n+k-1; i++)
    {
        info->packstatperm.ptr.p_int[i] = i;
        info->unpackstatperm.ptr.p_int[i] = i;
    }
    sparsecopytocrsbuf(sparsea, &info->rawa, _state);
    
    /*
     * Scale cost and box constraints
     */
    rcopyallocv(n, c, &info->c, _state);
    rcopyallocv(n, bndl, &info->bndl, _state);
    rcopyallocv(n, bndu, &info->bndu, _state);
    if( k>0 )
    {
        rcopyallocv(k, al, &info->al, _state);
        rcopyallocv(k, au, &info->au, _state);
        sparsecopybuf(sparsea, &info->sparsea, _state);
    }
    lpqppresolve_scalecostandconstraints(s, n, &info->c, &info->bndl, &info->bndu, &info->sparsea, &info->al, &info->au, k, &info->trfstack, _state);
}


/*************************************************************************
Extensive presolving for an LP problem, all techniques are used

INPUT PARAMETERS:
    S           -   array[N], user-supplied scale vector, S[I]>0
    C           -   array[N], costs
    BndL        -   array[N], lower bounds (may contain -INF)
    BndU        -   array[N], upper bounds (may contain +INF)
    N           -   variable count, N>0
    SparseA     -   matrix[K,N], sparse constraints
    AL          -   array[K], lower constraint bounds (may contain -INF)
    AU          -   array[K], upper constraint bounds (may contain +INF)
    K           -   constraint count, K>=0
    Info        -   presolve info structure; temporaries allocated during
                    previous calls may be reused by this function.
                    
OUTPUT PARAMETERS:
    Info        -   contains transformed C, BndL, bndU,  SparseA,  AL,  AU
                    and   information   necessary   to   perform  backward
                    transformation.
                    Following fields can be acessed:
                    * Info.NewN>0  for transformed problem size
                    * Info.NewM>=0 for transformed constraint count
                    * always:          Info.C, Info.BndL, Info.BndU - array[NewN]
                    * for Info.NewM>0: Info.SparseA, Info.AL, Info.AU
    
NOTE: this routine does not reallocate arrays if NNew<=NOld and/or KNew<=KOld.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
void presolvelp(/* Real    */ const ae_vector* raws,
     /* Real    */ const ae_vector* rawc,
     /* Real    */ const ae_vector* rawbndl,
     /* Real    */ const ae_vector* rawbndu,
     ae_int_t n,
     const sparsematrix* rawsparsea,
     /* Real    */ const ae_vector* rawal,
     /* Real    */ const ae_vector* rawau,
     ae_int_t m,
     ae_bool dotrace,
     presolveinfo* presolved,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_bool somethingchanged;
    ae_vector isdroppedcol;
    ae_vector isdroppedrow;
    sparsematrix normsparsea;
    sparsematrix normsparseat;
    dynamiccrs a;
    dynamiccrs at;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector al;
    ae_vector au;
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t offs;
    ae_int_t dbgemptycol;
    ae_int_t dbgemptyrow;
    ae_int_t dbgnonbindingrows;
    ae_int_t dbgfixed;
    ae_int_t dbgsingletonrow;
    ae_int_t dbgslackvars;
    ae_int_t dbgimplicitslacks;
    ae_int_t dbgfreecolumnsingletons;
    double eps;
    ae_int_t presolverounds;

    ae_frame_make(_state, &_frame_block);
    memset(&isdroppedcol, 0, sizeof(isdroppedcol));
    memset(&isdroppedrow, 0, sizeof(isdroppedrow));
    memset(&normsparsea, 0, sizeof(normsparsea));
    memset(&normsparseat, 0, sizeof(normsparseat));
    memset(&a, 0, sizeof(a));
    memset(&at, 0, sizeof(at));
    memset(&c, 0, sizeof(c));
    memset(&bndl, 0, sizeof(bndl));
    memset(&bndu, 0, sizeof(bndu));
    memset(&al, 0, sizeof(al));
    memset(&au, 0, sizeof(au));
    ae_vector_init(&isdroppedcol, 0, DT_BOOL, _state, ae_true);
    ae_vector_init(&isdroppedrow, 0, DT_BOOL, _state, ae_true);
    _sparsematrix_init(&normsparsea, _state, ae_true);
    _sparsematrix_init(&normsparseat, _state, ae_true);
    _dynamiccrs_init(&a, _state, ae_true);
    _dynamiccrs_init(&at, _state, ae_true);
    ae_vector_init(&c, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bndl, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bndu, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&al, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&au, 0, DT_REAL, _state, ae_true);

    ae_assert(m==0||sparseiscrs(rawsparsea, _state), "PRESOLVER: A is non-CRS sparse matrix", _state);
    
    /*
     * Trace
     */
    dotrace = dotrace||ae_is_trace_enabled("PRESOLVER.LP");
    if( dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("// LP PRESOLVER STARTED                                                                           //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("----- printing input problem metrics ---------------------------------------------------------------\n");
        ae_trace("N            = %10d (variables)\n",
            (int)(n));
        ae_trace("M            = %10d (constraints)\n",
            (int)(m));
        if( m!=0 )
        {
            ae_trace("nz(A)        = %10d (nonzeros in A)\n",
                (int)(rawsparsea->ridx.ptr.p_int[rawsparsea->m]));
        }
    }
    
    /*
     * Quick exit for M=0
     */
    if( m==0 )
    {
        presolvenonescaleuser(raws, rawc, rawbndl, rawbndu, n, rawsparsea, rawal, rawau, m, dotrace, presolved, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Initial check for constraint feasibility
     */
    for(i=0; i<=n-1; i++)
    {
        if( (ae_isfinite(rawbndl->ptr.p_double[i], _state)&&ae_isfinite(rawbndu->ptr.p_double[i], _state))&&ae_fp_greater(rawbndl->ptr.p_double[i],rawbndu->ptr.p_double[i]) )
        {
            if( dotrace )
            {
                ae_trace("> variable %0d is found to have infeasible box constraints, terminating\n",
                    (int)(i));
            }
            presolved->problemstatus = -3;
            ae_frame_leave(_state);
            return;
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( (ae_isfinite(rawal->ptr.p_double[i], _state)&&ae_isfinite(rawau->ptr.p_double[i], _state))&&ae_fp_greater(rawal->ptr.p_double[i],rawau->ptr.p_double[i]) )
        {
            if( dotrace )
            {
                ae_trace("> linear constraint %0d is found to have infeasible bounds, terminating\n",
                    (int)(i));
            }
            presolved->problemstatus = -3;
            ae_frame_leave(_state);
            return;
        }
    }
    
    /*
     * Trace counters
     */
    dbgemptycol = 0;
    dbgemptyrow = 0;
    dbgnonbindingrows = 0;
    dbgfixed = 0;
    dbgsingletonrow = 0;
    dbgslackvars = 0;
    dbgimplicitslacks = 0;
    dbgfreecolumnsingletons = 0;
    
    /*
     * Initial state of the presolver
     */
    eps = (double)(1000+n+m)*ae_machineepsilon;
    lpqppresolve_presolverstackinit(n, m, &presolved->trfstack, _state);
    presolved->problemstatus = 0;
    rcopyallocv(n, rawc, &c, _state);
    rcopyallocv(n, rawbndl, &bndl, _state);
    rcopyallocv(n, rawbndu, &bndu, _state);
    sparsecopybuf(rawsparsea, &normsparsea, _state);
    rcopyallocv(m, rawal, &al, _state);
    rcopyallocv(m, rawau, &au, _state);
    lpqppresolve_scalecostandconstraints(raws, n, &c, &bndl, &bndu, &normsparsea, &al, &au, m, &presolved->trfstack, _state);
    sparsecopytransposecrsbuf(&normsparsea, &normsparseat, _state);
    lpqppresolve_dyncrsinitfromsparsecrs(&normsparsea, &a, _state);
    lpqppresolve_dyncrsinitfromsparsecrs(&normsparseat, &at, _state);
    lpqppresolve_dyncrsdropzeros(&a, _state);
    lpqppresolve_dyncrsdropzeros(&at, _state);
    bsetallocv(n, ae_false, &presolved->lagrangefromresidual, _state);
    bsetallocv(n, ae_false, &isdroppedcol, _state);
    bsetallocv(m, ae_false, &isdroppedrow, _state);
    lpqppresolve_presolvebuffersinit(&presolved->buf, n, m, _state);
    
    /*
     * While something changes, keep iterating
     */
    presolverounds = 0;
    somethingchanged = ae_true;
    while(somethingchanged)
    {
        somethingchanged = ae_false;
        
        /*
         * Try the very basic reductions:
         * * empty column
         * * empty row
         * * fixed variables removal
         * * singleton rows to box constraints
         */
        if( !lpqppresolve_dropemptycol(&c, &bndl, &bndu, &isdroppedcol, &presolved->lagrangefromresidual, n, &a, &at, &al, &au, m, eps, dotrace, &presolved->buf, &presolved->trfstack, &presolved->problemstatus, &somethingchanged, &dbgemptycol, _state) )
        {
            ae_frame_leave(_state);
            return;
        }
        if( !lpqppresolve_dropclearlynonbindingrows(n, &isdroppedrow, &a, &at, &al, &au, m, eps, dotrace, &presolved->buf, &presolved->trfstack, &presolved->problemstatus, &somethingchanged, &dbgemptyrow, &dbgnonbindingrows, _state) )
        {
            ae_frame_leave(_state);
            return;
        }
        if( !lpqppresolve_singletonrowtobc(&bndl, &bndu, &isdroppedcol, n, &a, &at, &al, &au, &isdroppedrow, m, eps, dotrace, &presolved->buf, &presolved->trfstack, &presolved->problemstatus, &somethingchanged, &dbgsingletonrow, _state) )
        {
            ae_frame_leave(_state);
            return;
        }
        if( !lpqppresolve_fixvariables(&c, &bndl, &bndu, &isdroppedcol, n, &a, &at, &al, &au, m, eps, dotrace, &presolved->buf, &presolved->trfstack, &presolved->problemstatus, &somethingchanged, &dbgfixed, _state) )
        {
            ae_frame_leave(_state);
            return;
        }
        if( !lpqppresolve_singletoncols(&c, &bndl, &bndu, &isdroppedcol, n, &a, &at, &al, &au, &isdroppedrow, m, eps, dotrace, &presolved->buf, &presolved->trfstack, &presolved->problemstatus, &somethingchanged, &dbgslackvars, &dbgimplicitslacks, &dbgfreecolumnsingletons, _state) )
        {
            ae_frame_leave(_state);
            return;
        }
        presolverounds = presolverounds+1;
    }
    
    /*
     * Starting output
     */
    rcopyallocv(n, rawc, &presolved->rawc, _state);
    rcopyallocv(n, rawbndl, &presolved->rawbndl, _state);
    rcopyallocv(n, rawbndu, &presolved->rawbndu, _state);
    sparsecopytocrsbuf(rawsparsea, &presolved->rawa, _state);
    
    /*
     * Output data that are permuted by XPerm[]
     */
    presolved->oldn = n;
    presolved->newn = 0;
    isetallocv(n, -1, &presolved->packxperm, _state);
    isetallocv(n, -1, &presolved->unpackxperm, _state);
    for(i=0; i<=n-1; i++)
    {
        if( !isdroppedcol.ptr.p_bool[i] )
        {
            presolved->unpackxperm.ptr.p_int[presolved->newn] = i;
            presolved->packxperm.ptr.p_int[i] = presolved->newn;
            presolved->newn = presolved->newn+1;
        }
    }
    rallocv(presolved->newn, &presolved->bndl, _state);
    rallocv(presolved->newn, &presolved->bndu, _state);
    rallocv(presolved->newn, &presolved->c, _state);
    for(i=0; i<=presolved->newn-1; i++)
    {
        presolved->c.ptr.p_double[i] = c.ptr.p_double[presolved->unpackxperm.ptr.p_int[i]];
        presolved->bndl.ptr.p_double[i] = bndl.ptr.p_double[presolved->unpackxperm.ptr.p_int[i]];
        presolved->bndu.ptr.p_double[i] = bndu.ptr.p_double[presolved->unpackxperm.ptr.p_int[i]];
    }
    
    /*
     * Output data that are permuted by YPerm[]
     */
    presolved->oldm = m;
    presolved->newm = 0;
    isetallocv(m, -1, &presolved->packyperm, _state);
    isetallocv(m, -1, &presolved->unpackyperm, _state);
    for(i=0; i<=m-1; i++)
    {
        if( !isdroppedrow.ptr.p_bool[i] )
        {
            presolved->unpackyperm.ptr.p_int[presolved->newm] = i;
            presolved->packyperm.ptr.p_int[i] = presolved->newm;
            presolved->newm = presolved->newm+1;
        }
    }
    rallocv(presolved->newm, &presolved->al, _state);
    rallocv(presolved->newm, &presolved->au, _state);
    for(i=0; i<=presolved->newm-1; i++)
    {
        presolved->al.ptr.p_double[i] = al.ptr.p_double[presolved->unpackyperm.ptr.p_int[i]];
        presolved->au.ptr.p_double[i] = au.ptr.p_double[presolved->unpackyperm.ptr.p_int[i]];
    }
    
    /*
     * Output A which is permuted by both XPerm[] and YPerm[]
     */
    presolved->sparsea.m = 0;
    presolved->sparsea.n = presolved->newn;
    iallocv(presolved->newm+1, &presolved->sparsea.ridx, _state);
    presolved->sparsea.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=m-1; i++)
    {
        if( !isdroppedrow.ptr.p_bool[i] )
        {
            offs = presolved->sparsea.ridx.ptr.p_int[presolved->sparsea.m];
            igrowv(offs+n, &presolved->sparsea.idx, _state);
            rgrowv(offs+n, &presolved->sparsea.vals, _state);
            j0 = a.rowbegin.ptr.p_int[i];
            j1 = a.rowend.ptr.p_int[i]-1;
            for(jj=j0; jj<=j1; jj++)
            {
                j = presolved->packxperm.ptr.p_int[a.idx.ptr.p_int[jj]];
                ae_assert(j>=0, "PRESOLVE: integrity check 54fc failed", _state);
                presolved->sparsea.idx.ptr.p_int[offs] = j;
                presolved->sparsea.vals.ptr.p_double[offs] = a.vals.ptr.p_double[jj];
                offs = offs+1;
            }
            presolved->sparsea.m = presolved->sparsea.m+1;
            presolved->sparsea.ridx.ptr.p_int[presolved->sparsea.m] = offs;
        }
    }
    ae_assert(presolved->sparsea.m==presolved->newm, "PRESOLVE: integrity check ee3a failed", _state);
    sparsecreatecrsinplace(&presolved->sparsea, _state);
    
    /*
     * Prepare permutation for constraint Stats[]
     */
    isetallocv(n+m, -1, &presolved->packstatperm, _state);
    isetallocv(n+m, -1, &presolved->unpackstatperm, _state);
    offs = 0;
    for(i=0; i<=n-1; i++)
    {
        if( !isdroppedcol.ptr.p_bool[i] )
        {
            presolved->packstatperm.ptr.p_int[i] = offs;
            offs = offs+1;
        }
    }
    for(i=0; i<=m-1; i++)
    {
        if( !isdroppedrow.ptr.p_bool[i] )
        {
            presolved->packstatperm.ptr.p_int[n+i] = offs;
            offs = offs+1;
        }
    }
    ae_assert(offs==presolved->newn+presolved->newm, "PRESOLVE: integrity check 3632 failed", _state);
    for(i=0; i<=n+m-1; i++)
    {
        if( presolved->packstatperm.ptr.p_int[i]>=0 )
        {
            presolved->unpackstatperm.ptr.p_int[presolved->packstatperm.ptr.p_int[i]] = i;
        }
    }
    
    /*
     * Trace output
     */
    if( dotrace )
    {
        ae_trace("----- printing information about reductions --------------------------------------------------------\n");
        ae_trace("fixed vars           = %10d (fixed variables)\n",
            (int)(dbgfixed));
        ae_trace("empty cols           = %10d (empty constraint columns)\n",
            (int)(dbgemptycol));
        ae_trace("empty rows           = %10d (empty constraint rows)\n",
            (int)(dbgemptyrow));
        ae_trace("nonbinding rows      = %10d (irrelevant due to bounds)\n",
            (int)(dbgnonbindingrows));
        ae_trace("singleton rows       = %10d (singleton rows converted to box constraints)\n",
            (int)(dbgsingletonrow));
        ae_trace("slack variables      = %10d (singleton cols with zero cost, recognized as explicit slacks)\n",
            (int)(dbgslackvars));
        ae_trace("implicit slacks      = %10d (singleton cols at equality rows, recognized as implicit slacks)\n",
            (int)(dbgimplicitslacks));
        ae_trace("free col singletons  = %10d (free singleton columns)\n",
            (int)(dbgfreecolumnsingletons));
        ae_trace("----- printing output problem metrics --------------------------------------------------------------\n");
        ae_trace("N            = %10d (variables)\n",
            (int)(presolved->newn));
        ae_trace("M            = %10d (constraints)\n",
            (int)(presolved->newm));
        if( m!=0 )
        {
            ae_trace("nz(A)        = %10d (nonzeros in A)\n",
                (int)(presolved->sparsea.ridx.ptr.p_int[presolved->sparsea.m]));
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Backward transformation which extracts original solution from that of  the
converted problem.

Below NNew/KNew correspond to transformed problem size (as returned by the
presolve routine) and NOld/KOld correspond to original  problem  size  (as
specified by caller). We expect that caller knows  these  sizes,  so  this
routine does not report them.

INPUT PARAMETERS:
    Info        -   presolve info structure
    X           -   array[NNew], transformed solution (primal variables)
    Stats       -   array[NNew+MNew], transformed constraint status (negative -
                    at lower bound, positive -  at  upper  bound,  zero  -
                    inactive).
    LagBC       -   array[NNew], transformed Lagrange multipliers
    LagLC       -   array[KNew], transformed Lagrange multipliers
                    
OUTPUT PARAMETERS:
    X           -   array[NOld], original solution (primal variables)
    Stats       -   array[NOld+MOld], original constraint status
    LagBC       -   array[NOld], Lagrange multipliers
    LagLC       -   array[KOld], Lagrange multipliers
    
NOTE: this routine does not reallocate arrays if NOld<=NNew and/or KOld<=KNew.

  -- ALGLIB --
     Copyright 01.07.2020 by Bochkanov Sergey
*************************************************************************/
void presolvebwd(presolveinfo* info,
     /* Real    */ ae_vector* x,
     /* Integer */ ae_vector* stats,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_state *_state)
{
    ae_int_t i;


    
    /*
     * Read state of the transformed problem into storage allocated for the original problem
     */
    isetallocv(info->oldn+info->oldm, 0, &info->s1, _state);
    for(i=0; i<=info->newn+info->newm-1; i++)
    {
        info->s1.ptr.p_int[info->unpackstatperm.ptr.p_int[i]] = stats->ptr.p_int[i];
    }
    rsetallocv(info->oldn, 0.0, &info->x1, _state);
    for(i=0; i<=info->newn-1; i++)
    {
        info->x1.ptr.p_double[info->unpackxperm.ptr.p_int[i]] = x->ptr.p_double[i];
    }
    rsetallocv(info->oldn, 0.0, &info->bc1, _state);
    for(i=0; i<=info->newn-1; i++)
    {
        info->bc1.ptr.p_double[info->unpackxperm.ptr.p_int[i]] = lagbc->ptr.p_double[i];
    }
    rsetallocv(info->oldm, 0.0, &info->y1, _state);
    for(i=0; i<=info->newm-1; i++)
    {
        info->y1.ptr.p_double[info->unpackyperm.ptr.p_int[i]] = laglc->ptr.p_double[i];
    }
    
    /*
     * Apply reverse transformation
     */
    lpqppresolve_presolverrestoresolution(&info->trfstack, &info->x1, &info->bc1, &info->y1, &info->s1, _state);
    
    /*
     * Polish X according to box constraints and info from newly recovered Stats[]
     * Recompute coefficients corresponding to variables fixed during presolve using residual costs.
     */
    for(i=0; i<=info->oldn-1; i++)
    {
        if( ae_isfinite(info->rawbndl.ptr.p_double[i], _state) )
        {
            info->x1.ptr.p_double[i] = ae_maxreal(info->x1.ptr.p_double[i], info->rawbndl.ptr.p_double[i], _state);
        }
        if( ae_isfinite(info->rawbndu.ptr.p_double[i], _state) )
        {
            info->x1.ptr.p_double[i] = ae_minreal(info->x1.ptr.p_double[i], info->rawbndu.ptr.p_double[i], _state);
        }
        if( info->s1.ptr.p_int[i]<0 )
        {
            info->x1.ptr.p_double[i] = info->rawbndl.ptr.p_double[i];
        }
        if( info->s1.ptr.p_int[i]>0 )
        {
            info->x1.ptr.p_double[i] = info->rawbndu.ptr.p_double[i];
        }
    }
    rcopyallocv(info->oldn, &info->rawc, &info->d, _state);
    if( info->oldm>0 )
    {
        sparsegemv(&info->rawa, 1.0, 1, &info->y1, 0, 1.0, &info->d, 0, _state);
    }
    for(i=0; i<=info->oldn-1; i++)
    {
        if( info->lagrangefromresidual.ptr.p_bool[i] )
        {
            info->bc1.ptr.p_double[i] = -info->d.ptr.p_double[i];
        }
    }
    
    /*
     * Output
     */
    rcopyallocv(info->oldn, &info->x1, x, _state);
    rcopyallocv(info->oldm, &info->y1, laglc, _state);
    icopyallocv(info->oldn+info->oldm, &info->s1, stats, _state);
    rcopyallocv(info->oldn, &info->bc1, lagbc, _state);
}


/*************************************************************************
Initialize dynamic CRS matrix using SparseMatrix structure

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_dyncrsinitfromsparsecrs(const sparsematrix* s,
     dynamiccrs* r,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;


    m = s->m;
    n = s->n;
    ae_assert(s->matrixtype==1, "DynCRSInitFromSparseCRS: S is not CRS matrix", _state);
    r->m = m;
    r->n = n;
    icopyallocv(s->ridx.ptr.p_int[m], &s->idx, &r->idx, _state);
    rcopyallocv(s->ridx.ptr.p_int[m], &s->vals, &r->vals, _state);
    iallocv(m, &r->rowbegin, _state);
    iallocv(m, &r->rowend, _state);
    icopyvx(m, &s->ridx, 0, &r->rowbegin, 0, _state);
    icopyvx(m, &s->ridx, 1, &r->rowend, 0, _state);
}


/*************************************************************************
Drops numerical zeros from the matrix

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_dyncrsdropzeros(dynamiccrs* a, ae_state *_state)
{
    ae_int_t m;
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t offs;
    double v;


    m = a->m;
    for(i=0; i<=m-1; i++)
    {
        offs = a->rowbegin.ptr.p_int[i];
        for(jj=a->rowbegin.ptr.p_int[i]; jj<=a->rowend.ptr.p_int[i]-1; jj++)
        {
            j = a->idx.ptr.p_int[jj];
            v = a->vals.ptr.p_double[jj];
            if( v==(double)0 )
            {
                continue;
            }
            a->idx.ptr.p_int[offs] = j;
            a->vals.ptr.p_double[offs] = v;
            offs = offs+1;
        }
        a->rowend.ptr.p_int[i] = offs;
    }
}


/*************************************************************************
Removes rows

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_dyncrsremoverow(dynamiccrs* a,
     ae_int_t rowidx,
     ae_state *_state)
{


    a->rowend.ptr.p_int[rowidx] = a->rowbegin.ptr.p_int[rowidx];
}


/*************************************************************************
Removes element from the row

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_dyncrsremovefromrow(dynamiccrs* a,
     ae_int_t rowidx,
     ae_int_t j,
     ae_state *_state)
{
    ae_int_t ii;
    ae_int_t offs;


    offs = a->rowbegin.ptr.p_int[rowidx];
    for(ii=a->rowbegin.ptr.p_int[rowidx]; ii<=a->rowend.ptr.p_int[rowidx]-1; ii++)
    {
        if( a->idx.ptr.p_int[ii]!=j )
        {
            a->idx.ptr.p_int[offs] = a->idx.ptr.p_int[ii];
            a->vals.ptr.p_double[offs] = a->vals.ptr.p_double[ii];
            offs = offs+1;
        }
    }
    a->rowend.ptr.p_int[rowidx] = offs;
}


/*************************************************************************
Removes elements with indexes in the set from the row

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_dyncrsremovesetfromrow(dynamiccrs* a,
     ae_int_t rowidx,
     const niset* s,
     ae_state *_state)
{
    ae_int_t ii;
    ae_int_t offs;


    offs = a->rowbegin.ptr.p_int[rowidx];
    for(ii=a->rowbegin.ptr.p_int[rowidx]; ii<=a->rowend.ptr.p_int[rowidx]-1; ii++)
    {
        if( s->locationof.ptr.p_int[a->idx.ptr.p_int[ii]]<0 )
        {
            a->idx.ptr.p_int[offs] = a->idx.ptr.p_int[ii];
            a->vals.ptr.p_double[offs] = a->vals.ptr.p_double[ii];
            offs = offs+1;
        }
    }
    a->rowend.ptr.p_int[rowidx] = offs;
}


/*************************************************************************
Initialize presolver stack

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverstackinit(ae_int_t n,
     ae_int_t m,
     presolverstack* s,
     ae_state *_state)
{


    s->n = n;
    s->m = m;
    s->ntrf = 0;
    isetallocv(1, 0, &s->idataridx, _state);
    isetallocv(1, 0, &s->rdataridx, _state);
}


/*************************************************************************
Streams boolean value to the presolver stack data storage (the data
are appended to the last transform on top of the stack).

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverstreamb(presolverstack* s,
     ae_bool b,
     ae_state *_state)
{
    ae_int_t ilast;


    ilast = s->idataridx.ptr.p_int[s->ntrf];
    igrowv(ilast+1, &s->idata, _state);
    s->idata.ptr.p_int[ilast] = icase2(b, 1, 0, _state);
    s->idataridx.ptr.p_int[s->ntrf] = ilast+1;
}


/*************************************************************************
Streams integer value to the presolver stack data storage (the data
are appended to the last transform on top of the stack).

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverstreami(presolverstack* s,
     ae_int_t i,
     ae_state *_state)
{
    ae_int_t ilast;


    ilast = s->idataridx.ptr.p_int[s->ntrf];
    igrowv(ilast+1, &s->idata, _state);
    s->idata.ptr.p_int[ilast] = i;
    s->idataridx.ptr.p_int[s->ntrf] = ilast+1;
}


/*************************************************************************
Streams real value to the presolver stack data storage (the data
are appended to the last transform on top of the stack).

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverstreamr(presolverstack* s,
     double v,
     ae_state *_state)
{
    ae_int_t rlast;


    rlast = s->rdataridx.ptr.p_int[s->ntrf];
    rgrowv(rlast+1, &s->rdata, _state);
    s->rdata.ptr.p_double[rlast] = v;
    s->rdataridx.ptr.p_int[s->ntrf] = rlast+1;
}


/*************************************************************************
Streams a integer/real pair to the presolver stack data storage (the data
are appended to the last transform on top of the stack).

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverstreamir(presolverstack* s,
     ae_int_t i,
     double v,
     ae_state *_state)
{
    ae_int_t ilast;
    ae_int_t rlast;


    ilast = s->idataridx.ptr.p_int[s->ntrf];
    rlast = s->rdataridx.ptr.p_int[s->ntrf];
    igrowv(ilast+1, &s->idata, _state);
    rgrowv(rlast+1, &s->rdata, _state);
    s->idata.ptr.p_int[ilast] = i;
    s->rdata.ptr.p_double[rlast] = v;
    s->idataridx.ptr.p_int[s->ntrf] = ilast+1;
    s->rdataridx.ptr.p_int[s->ntrf] = rlast+1;
}


/*************************************************************************
Streams DynamicCRS row.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverstreamcrsrow(presolverstack* s,
     const dynamiccrs* a,
     ae_int_t i,
     ae_state *_state)
{
    ae_int_t k;


    lpqppresolve_presolverstreami(s, a->rowend.ptr.p_int[i]-a->rowbegin.ptr.p_int[i], _state);
    for(k=a->rowbegin.ptr.p_int[i]; k<=a->rowend.ptr.p_int[i]-1; k++)
    {
        lpqppresolve_presolverstreamir(s, a->idx.ptr.p_int[k], a->vals.ptr.p_double[k], _state);
    }
}


/*************************************************************************
Select transformation TIdx as the source for stream reads

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverselectstreamsource(presolverstack* s,
     ae_int_t tidx,
     ae_state *_state)
{


    s->sourceidx = tidx;
    s->isrc = s->idataridx.ptr.p_int[tidx];
    s->rsrc = s->rdataridx.ptr.p_int[tidx];
}


/*************************************************************************
Reads from presolver stack boolean value, advances stream pointer

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverunstreamb(presolverstack* s,
     ae_bool* v,
     ae_state *_state)
{

    *v = ae_false;

    *v = s->idata.ptr.p_int[s->isrc]!=0;
    s->isrc = s->isrc+1;
}


/*************************************************************************
Reads from presolver stack integer value, advances stream pointer

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverunstreami(presolverstack* s,
     ae_int_t* v,
     ae_state *_state)
{

    *v = 0;

    *v = s->idata.ptr.p_int[s->isrc];
    s->isrc = s->isrc+1;
}


/*************************************************************************
Reads from presolver stack real value, advances stream pointer

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverunstreamr(presolverstack* s,
     double* v,
     ae_state *_state)
{

    *v = 0.0;

    *v = s->rdata.ptr.p_double[s->rsrc];
    s->rsrc = s->rsrc+1;
}


/*************************************************************************
Reads from presolver stack int/real pair, advances stream pointers

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverunstreamir(presolverstack* s,
     ae_int_t* vi,
     double* vr,
     ae_state *_state)
{

    *vi = 0;
    *vr = 0.0;

    *vi = s->idata.ptr.p_int[s->isrc];
    s->isrc = s->isrc+1;
    *vr = s->rdata.ptr.p_double[s->rsrc];
    s->rsrc = s->rsrc+1;
}


/*************************************************************************
Reads from presolver stack compressed sparse vector, advances stream pointers

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverunstreamsparsevec(presolverstack* s,
     ae_int_t* cnt,
     /* Integer */ ae_vector* idx,
     /* Real    */ ae_vector* vals,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;


    lpqppresolve_presolverunstreami(s, cnt, _state);
    iallocv(*cnt, idx, _state);
    rallocv(*cnt, vals, _state);
    for(i=0; i<=*cnt-1; i++)
    {
        lpqppresolve_presolverunstreamir(s, &j, &v, _state);
        idx->ptr.p_int[i] = j;
        vals->ptr.p_double[i] = v;
    }
}


/*************************************************************************
Checks that we are at the end of the stream corresponding to transfrom #TIdx

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverasserteos(presolverstack* s,
     ae_state *_state)
{


    ae_assert(s->isrc==s->idataridx.ptr.p_int[s->sourceidx+1], "PresolverAssertEOS: unread integers in the stream", _state);
    ae_assert(s->rsrc==s->rdataridx.ptr.p_int[s->sourceidx+1], "PresolverAssertEOS: unread reals in the stream", _state);
}


/*************************************************************************
Appends transform placeholder

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappendtrf(presolverstack* s,
     ae_int_t tt,
     ae_state *_state)
{


    igrowv(s->ntrf+1, &s->trftype, _state);
    igrowv(s->ntrf+2, &s->idataridx, _state);
    igrowv(s->ntrf+2, &s->rdataridx, _state);
    s->trftype.ptr.p_int[s->ntrf] = tt;
    s->idataridx.ptr.p_int[s->ntrf+1] = s->idataridx.ptr.p_int[s->ntrf];
    s->rdataridx.ptr.p_int[s->ntrf+1] = s->rdataridx.ptr.p_int[s->ntrf];
    s->ntrf = s->ntrf+1;
}


/*************************************************************************
Appends cost scaling to the presolver stack.

The cost vector is MULTIPLIED by the scale coefficient.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappendcostscaling(presolverstack* s,
     double vmul,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 0, _state);
    lpqppresolve_presolverstreamr(s, vmul, _state);
}


/*************************************************************************
Appends column scaling to the presolver stack.

The variable is MULTIPLIED by the scale coefficient.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappendcolscaling(presolverstack* s,
     ae_int_t colidx,
     double vmul,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 1, _state);
    lpqppresolve_presolverstreamir(s, colidx, vmul, _state);
}


/*************************************************************************
Appends row scaling to the presolver stack.

The row is MULTIPLIED by the scale coefficient.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappendrowscaling(presolverstack* s,
     ae_int_t rowidx,
     double vmul,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 2, _state);
    lpqppresolve_presolverstreamir(s, rowidx, vmul, _state);
}


/*************************************************************************
Appends command to drop empty col and set variable and Lagrange multiplier
to prescribed values

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappenddropemptycol(presolverstack* s,
     ae_int_t colidx,
     double varval,
     double lagval,
     ae_int_t statval,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 3, _state);
    lpqppresolve_presolverstreami(s, colidx, _state);
    lpqppresolve_presolverstreamr(s, varval, _state);
    lpqppresolve_presolverstreamr(s, lagval, _state);
    lpqppresolve_presolverstreami(s, statval, _state);
}


/*************************************************************************
Appends command to drop empty row

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappenddropemptyrow(presolverstack* s,
     ae_int_t rowidx,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 4, _state);
    lpqppresolve_presolverstreami(s, rowidx, _state);
}


/*************************************************************************
Appends command to convert singleton row to box constraint

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappendsingletonrow(presolverstack* s,
     ae_int_t i,
     ae_int_t j,
     double v,
     double swapsign,
     double bndl,
     ae_bool bndlisbc,
     double bndu,
     ae_bool bnduisbc,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 5, _state);
    lpqppresolve_presolverstreami(s, i, _state);
    lpqppresolve_presolverstreami(s, j, _state);
    lpqppresolve_presolverstreamr(s, v, _state);
    lpqppresolve_presolverstreamr(s, swapsign, _state);
    lpqppresolve_presolverstreamr(s, bndl, _state);
    lpqppresolve_presolverstreamb(s, bndlisbc, _state);
    lpqppresolve_presolverstreamr(s, bndu, _state);
    lpqppresolve_presolverstreamb(s, bnduisbc, _state);
}


/*************************************************************************
Appends command to fix variable

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappendfixedvar(presolverstack* s,
     ae_int_t colidx,
     double fixval,
     double ci,
     const dynamiccrs* at,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 6, _state);
    lpqppresolve_presolverstreami(s, colidx, _state);
    lpqppresolve_presolverstreamr(s, fixval, _state);
    lpqppresolve_presolverstreamr(s, ci, _state);
    lpqppresolve_presolverstreamcrsrow(s, at, colidx, _state);
}


/*************************************************************************
Appends explicit slack transformation

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappendexplicitslack(presolverstack* s,
     ae_int_t i,
     ae_int_t j,
     double aij,
     double slackbndl,
     double slackbndu,
     double al,
     double au,
     const dynamiccrs* a,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 7, _state);
    lpqppresolve_presolverstreami(s, i, _state);
    lpqppresolve_presolverstreami(s, j, _state);
    lpqppresolve_presolverstreamr(s, aij, _state);
    lpqppresolve_presolverstreamr(s, slackbndl, _state);
    lpqppresolve_presolverstreamr(s, slackbndu, _state);
    lpqppresolve_presolverstreamr(s, al, _state);
    lpqppresolve_presolverstreamr(s, au, _state);
    lpqppresolve_presolverstreamcrsrow(s, a, i, _state);
}


/*************************************************************************
Appends implicit slack transformation

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverappendimplicitslack(presolverstack* s,
     ae_int_t i,
     ae_int_t j,
     double aij,
     double cj,
     double equalitybnd,
     const dynamiccrs* a,
     ae_state *_state)
{


    lpqppresolve_presolverappendtrf(s, 8, _state);
    lpqppresolve_presolverstreami(s, i, _state);
    lpqppresolve_presolverstreami(s, j, _state);
    lpqppresolve_presolverstreamr(s, aij, _state);
    lpqppresolve_presolverstreamr(s, cj, _state);
    lpqppresolve_presolverstreamr(s, equalitybnd, _state);
    lpqppresolve_presolverstreamcrsrow(s, a, i, _state);
}


/*************************************************************************
This function restores original solution, given the solution of the
transformed problem.

Below N and M denote column/row count for both the original problem and
transformed one PRIOR to removal of dropped columns and rows.

INPUT PARAMETERS:
    S   -           a sequence of presolve transformations
    X   -           array[N], transformed solution after permutation
                    that restores original variable order (moves all
                    fixed variables to their positions)
    LagBC-          array[N], Lagrange coeffs for box constraints
    LagLC-          array[M], Lagrange coeffs for linear constraints
                    after permutation that restores original row order
                    (moves all dropped rows to their positions)
    Stats-          array[N+M], constraint stats
    
OUTPUT PARAMETERS:
    X, LagBC, LagLC, Stats-restored solution
    

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolverrestoresolution(presolverstack* s,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     /* Integer */ ae_vector* stats,
     ae_state *_state)
{
    ae_int_t tidx;
    ae_int_t tt;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t cnt;
    ae_int_t vstat;
    double aij;
    double v;
    double vvar;
    double vlag;
    double vswap;
    double fixval;
    double ci;
    double cj;
    double equalitybnd;
    double bndl;
    double bndu;
    double al;
    double au;
    ae_bool bndlisbc;
    ae_bool bnduisbc;
    ae_bool transferlagtobc;
    double ratingl;
    double ratingu;


    for(tidx=s->ntrf-1; tidx>=0; tidx--)
    {
        tt = s->trftype.ptr.p_int[tidx];
        if( tt==0 )
        {
            
            /*
             * Reverse cost scaling
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreamr(s, &v, _state);
            lpqppresolve_presolverasserteos(s, _state);
            rmulv(s->n, (double)1/v, lagbc, _state);
            rmulv(s->m, (double)1/v, laglc, _state);
            continue;
        }
        if( tt==1 )
        {
            
            /*
             * Reverse column scaling
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreamir(s, &k, &v, _state);
            lpqppresolve_presolverasserteos(s, _state);
            x->ptr.p_double[k] = x->ptr.p_double[k]/v;
            lagbc->ptr.p_double[k] = lagbc->ptr.p_double[k]*v;
            continue;
        }
        if( tt==2 )
        {
            
            /*
             * Reverse row scaling
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreamir(s, &k, &v, _state);
            lpqppresolve_presolverasserteos(s, _state);
            laglc->ptr.p_double[k] = laglc->ptr.p_double[k]*v;
            continue;
        }
        if( tt==3 )
        {
            
            /*
             * Reverse dropping empty col
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreami(s, &k, _state);
            lpqppresolve_presolverunstreamr(s, &vvar, _state);
            lpqppresolve_presolverunstreamr(s, &vlag, _state);
            lpqppresolve_presolverunstreami(s, &vstat, _state);
            lpqppresolve_presolverasserteos(s, _state);
            x->ptr.p_double[k] = vvar;
            lagbc->ptr.p_double[k] = vlag;
            stats->ptr.p_int[k] = vstat;
            continue;
        }
        if( tt==4 )
        {
            
            /*
             * Reverse dropping empty row
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreami(s, &k, _state);
            lpqppresolve_presolverasserteos(s, _state);
            laglc->ptr.p_double[k] = (double)(0);
            stats->ptr.p_int[k] = 0;
            continue;
        }
        if( tt==5 )
        {
            
            /*
             * Handle singleton row.
             * Read data from stream.
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreami(s, &i, _state);
            lpqppresolve_presolverunstreami(s, &j, _state);
            lpqppresolve_presolverunstreamr(s, &v, _state);
            lpqppresolve_presolverunstreamr(s, &vswap, _state);
            lpqppresolve_presolverunstreamr(s, &bndl, _state);
            lpqppresolve_presolverunstreamb(s, &bndlisbc, _state);
            lpqppresolve_presolverunstreamr(s, &bndu, _state);
            lpqppresolve_presolverunstreamb(s, &bnduisbc, _state);
            lpqppresolve_presolverasserteos(s, _state);
            
            /*
             * Determine bound that is "most active" using rating that combines Lagrangian magnitude
             */
            ae_assert(ae_isfinite(bndl, _state)||ae_isfinite(bndu, _state), "PRESOLVE: singleton row with both bounds absent", _state);
            ae_assert(ae_fp_eq(ae_fabs(vswap, _state),(double)(1)), "PRESOLVE: unexpected VSwap", _state);
            ratingl = x->ptr.p_double[j]-bndl+ae_maxreal(lagbc->ptr.p_double[j], 0.0, _state);
            ratingu = bndu-x->ptr.p_double[j]+ae_maxreal(-lagbc->ptr.p_double[j], 0.0, _state);
            if( ae_fp_less(ratingl,ratingu) )
            {
                
                /*
                 * Lower bound is more likely
                 */
                transferlagtobc = bndlisbc;
            }
            else
            {
                
                /*
                 * Upper bound is more likely
                 */
                transferlagtobc = bnduisbc;
            }
            
            /*
             * Transfer constraint activity from the transformed problem to the original one
             */
            if( transferlagtobc )
            {
                
                /*
                 * Box constraint of the original problem is active, linear singleton constraint is inactive
                 */
                laglc->ptr.p_double[i] = 0.0;
                stats->ptr.p_int[s->n+i] = 0;
            }
            else
            {
                
                /*
                 * Linear constraint of the original problem is active, box constraint is inactive.
                 */
                laglc->ptr.p_double[i] = lagbc->ptr.p_double[j]/v*vswap;
                stats->ptr.p_int[s->n+i] = ae_round((double)stats->ptr.p_int[j]*vswap, _state);
                lagbc->ptr.p_double[j] = (double)(0);
                stats->ptr.p_int[j] = 0;
            }
            continue;
        }
        if( tt==6 )
        {
            
            /*
             * Fixed variable
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreami(s, &j, _state);
            lpqppresolve_presolverunstreamr(s, &fixval, _state);
            lpqppresolve_presolverunstreamr(s, &ci, _state);
            lpqppresolve_presolverunstreamsparsevec(s, &cnt, &s->sparseidx0, &s->sparseval0, _state);
            lpqppresolve_presolverasserteos(s, _state);
            x->ptr.p_double[j] = fixval;
            v = ci;
            for(i=0; i<=cnt-1; i++)
            {
                v = v+s->sparseval0.ptr.p_double[i]*laglc->ptr.p_double[s->sparseidx0.ptr.p_int[i]];
            }
            lagbc->ptr.p_double[j] = -v;
            stats->ptr.p_int[j] = ae_sign(lagbc->ptr.p_double[j], _state);
            continue;
        }
        if( tt==7 )
        {
            
            /*
             * Explicit slack variable:
             * * deduce its bounds from row activity
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreami(s, &i, _state);
            lpqppresolve_presolverunstreami(s, &j, _state);
            lpqppresolve_presolverunstreamr(s, &aij, _state);
            lpqppresolve_presolverunstreamr(s, &bndl, _state);
            lpqppresolve_presolverunstreamr(s, &bndu, _state);
            lpqppresolve_presolverunstreamr(s, &al, _state);
            lpqppresolve_presolverunstreamr(s, &au, _state);
            lpqppresolve_presolverunstreamsparsevec(s, &cnt, &s->sparseidx0, &s->sparseval0, _state);
            lpqppresolve_presolverasserteos(s, _state);
            x->ptr.p_double[j] = (double)(0);
            v = (double)(0);
            for(k=0; k<=cnt-1; k++)
            {
                v = v+s->sparseval0.ptr.p_double[k]*x->ptr.p_double[s->sparseidx0.ptr.p_int[k]];
            }
            if( ae_isfinite(al, _state) )
            {
                al = (al-v)/aij;
            }
            if( ae_isfinite(au, _state) )
            {
                au = (au-v)/aij;
            }
            if( ae_fp_less(aij,(double)(0)) )
            {
                swapr(&al, &au, _state);
                if( !ae_isfinite(al, _state) )
                {
                    al = _state->v_neginf;
                }
                if( !ae_isfinite(au, _state) )
                {
                    au = _state->v_posinf;
                }
            }
            if( ae_isfinite(al, _state)&&ae_fp_greater(al,bndl) )
            {
                bndl = al;
            }
            if( ae_isfinite(au, _state)&&ae_fp_less(au,bndu) )
            {
                bndu = au;
            }
            if( ae_isfinite(bndl, _state) )
            {
                x->ptr.p_double[j] = bndl;
            }
            else
            {
                if( ae_isfinite(bndu, _state) )
                {
                    x->ptr.p_double[j] = bndu;
                }
                else
                {
                    x->ptr.p_double[j] = (double)(0);
                }
            }
            lagbc->ptr.p_double[j] = -aij*laglc->ptr.p_double[i];
            stats->ptr.p_int[j] = -ae_sign(aij, _state)*stats->ptr.p_int[s->n+i];
            continue;
        }
        if( tt==8 )
        {
            
            /*
             * Implicit slack variable:
             * * deduce its bounds from row activity
             */
            lpqppresolve_presolverselectstreamsource(s, tidx, _state);
            lpqppresolve_presolverunstreami(s, &i, _state);
            lpqppresolve_presolverunstreami(s, &j, _state);
            lpqppresolve_presolverunstreamr(s, &aij, _state);
            lpqppresolve_presolverunstreamr(s, &cj, _state);
            lpqppresolve_presolverunstreamr(s, &equalitybnd, _state);
            lpqppresolve_presolverunstreamsparsevec(s, &cnt, &s->sparseidx0, &s->sparseval0, _state);
            lpqppresolve_presolverasserteos(s, _state);
            x->ptr.p_double[j] = (double)(0);
            v = (double)(0);
            for(k=0; k<=cnt-1; k++)
            {
                v = v+s->sparseval0.ptr.p_double[k]*x->ptr.p_double[s->sparseidx0.ptr.p_int[k]];
            }
            x->ptr.p_double[j] = (equalitybnd-v)/aij;
            lagbc->ptr.p_double[j] = -aij*laglc->ptr.p_double[i];
            laglc->ptr.p_double[i] = laglc->ptr.p_double[i]-cj/aij;
            stats->ptr.p_int[j] = -ae_sign(aij, _state)*stats->ptr.p_int[s->n+i];
            continue;
        }
        ae_assert(ae_false, "PresolverRestoreSolution: unexpected transform type", _state);
    }
}


/*************************************************************************
Prepare temporaries

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_presolvebuffersinit(presolvebuffers* buf,
     ae_int_t n,
     ae_int_t m,
     ae_state *_state)
{


    if( n>0 )
    {
        nisinitemptyslow(n, &buf->setn, _state);
    }
    if( m>0 )
    {
        nisinitemptyslow(m, &buf->setm, _state);
    }
}


/*************************************************************************
This function drops empty columns from the matrix. It may detect unbounded
problems (when unboundedness is caused by unconstrained column).

INPUT PARAMETERS:
    C, BndL, BndU  -   array[N], current cost and box constraints.
    IsDroppedCol-   array[N], column statuses; only non-dropped ones are examined.
    LagrangeFromResidual- array[N], whether we want to compute Lagrange coeffs
                    for box constraints using residual costs
    N           -   variables count (including both fixed and non-fixed)
    A, AT       -   current A and AT, dynamic CRS matrices
    AL, AU      -   array[M], lower/upper bounds for linear constraints
    M           -   linear constraints count
    Eps         -   unboundedness is checked subject to small dual feasibility
                    error tolerance
    DoTrace     -   whether tracing is needed or not
    SomethingChanged-flag variable
    CntFixed    -   debug counter, updated by the function
    trfStack    -   sequence of already applied transformations
                    
OUTPUT PARAMETERS:
    IsDroppedCol-   array[N], dropped columns are marked
    LagrangeFromResidual-
                    array[N], dropped cols are marked
    ProblemStatus-  on failure (unboundedness detected) is set to -2,
                    unchanged otherwise
    SomethingChanged-is set to True if at least one col was dropped
                    It is not changed otherwise.
    CntDropped  -   debug counter, updated by the function
    trfStack -      updated with new transforms
    
RESULT:
    if unboundedness was detected, False is returned.
    True is returned otherwise.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static ae_bool lpqppresolve_dropemptycol(/* Real    */ ae_vector* c,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* isdroppedcol,
     /* Boolean */ ae_vector* lagrangefromresidual,
     ae_int_t n,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntdropped,
     ae_state *_state)
{
    ae_int_t i;
    double v;
    ae_int_t statsval;
    ae_bool result;


    ae_assert(ae_fp_greater_eq(eps,(double)(0)), "LPPRESOLVE: Eps<0", _state);
    result = ae_true;
    
    /*
     * Scan columns, search for an empty one
     */
    for(i=0; i<=n-1; i++)
    {
        if( !isdroppedcol->ptr.p_bool[i]&&at->rowbegin.ptr.p_int[i]==at->rowend.ptr.p_int[i] )
        {
            
            /*
             * Try to detect unboundedness
             */
            if( (c->ptr.p_double[i]<-eps&&ae_isposinf(bndu->ptr.p_double[i], _state))||(c->ptr.p_double[i]>eps&&ae_isneginf(bndl->ptr.p_double[i], _state)) )
            {
                if( dotrace )
                {
                    ae_trace("> the target is unbounded (column %0d is empty, C[%0d]<>0, box constraints are insufficient)\n",
                        (int)(i),
                        (int)(i));
                }
                *problemstatus = -2;
                *somethingchanged = ae_true;
                *cntdropped = *cntdropped+1;
                result = ae_false;
                return result;
            }
            
            /*
             * Mark variable as fixed and issue transformation
             */
            isdroppedcol->ptr.p_bool[i] = ae_true;
            *somethingchanged = ae_true;
            *cntdropped = *cntdropped+1;
            v = (double)(0);
            statsval = 0;
            if( c->ptr.p_double[i]>(double)0&&ae_isfinite(bndl->ptr.p_double[i], _state) )
            {
                
                /*
                 * Variable has to be fixed at the lower bound
                 */
                v = bndl->ptr.p_double[i];
                statsval = -1;
            }
            if( c->ptr.p_double[i]<(double)0&&ae_isfinite(bndu->ptr.p_double[i], _state) )
            {
                
                /*
                 * Variable has to be fixed at the upper bound
                 */
                v = bndu->ptr.p_double[i];
                statsval = 1;
            }
            if( statsval==0 )
            {
                
                /*
                 * Variable value can be chosen arbitrarily, choose as close to zero as possible
                 */
                if( ae_isfinite(bndl->ptr.p_double[i], _state) )
                {
                    v = ae_maxreal(v, bndl->ptr.p_double[i], _state);
                }
                if( ae_isfinite(bndu->ptr.p_double[i], _state) )
                {
                    v = ae_minreal(v, bndu->ptr.p_double[i], _state);
                }
            }
            lpqppresolve_presolverappenddropemptycol(trfstack, i, v, -c->ptr.p_double[i], statsval, _state);
        }
    }
    return result;
}


/*************************************************************************
This function drops clearly nonbinding rows from the matrix: empty and ones
with infinite bounds.

It may detect infeasible problems (when infeasibility is caused by
constraint ranges  that  do  not include zero - the only value possible for
an empty row).

INPUT PARAMETERS:
    N           -   vars count
    IsDroppedRow-   array[M], column statuses; only non-dropped ones are examined.
    A, AT       -   current A and AT, dynamic CRS matrices
    AL, AU      -   array[M], lower/upper bounds for linear constraints
    M           -   linear constraints count
    Eps         -   small primal feasibility error is allowed
    DoTrace     -   whether tracing is needed or not
    SomethingChanged-flag variable
    TrfYLag, TrfYTgt, TrfStat, TrfStatTgt -
                    sequence of already applied transformations
                    
OUTPUT PARAMETERS:
    IsDroppedRow-   array[M], dropped rows are marked
    ProblemStatus-  on failure (infeasibility detected) is set to -3,
                    unchanged otherwise
    SomethingChanged-is set to True if at least one row was dropped
                    It is not changed otherwise.
    CntEmpty    -   debug counter, updated by the function
    CntNoBounds -   debug counter, updated by the function
    TrfYLag, TrfYTgt, TrfStat, TrfStatTgt -
                    sequence of already applied transformations
    
RESULT:
    if infeasibility was detected, False is returned.
    True is returned otherwise.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static ae_bool lpqppresolve_dropclearlynonbindingrows(ae_int_t n,
     /* Boolean */ ae_vector* isdroppedrow,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntempty,
     ae_int_t* cntnobounds,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t ii;
    ae_int_t jj;
    ae_bool result;


    result = ae_true;
    
    /*
     * Scan rows
     */
    nisclear(&buf->setn, _state);
    nisclear(&buf->setm, _state);
    for(i=0; i<=m-1; i++)
    {
        if( !isdroppedrow->ptr.p_bool[i] )
        {
            
            /*
             * Empty row
             */
            if( a->rowbegin.ptr.p_int[i]==a->rowend.ptr.p_int[i] )
            {
                
                /*
                 * Try to detect infeasibility
                 */
                if( (ae_isfinite(al->ptr.p_double[i], _state)&&ae_fp_greater(al->ptr.p_double[i],eps))||(ae_isfinite(au->ptr.p_double[i], _state)&&ae_fp_less(au->ptr.p_double[i],-eps)) )
                {
                    if( dotrace )
                    {
                        ae_trace("> the constraints are infeasible (row %0d is empty, initially or due to reductions, updated constraint range does not include zero)\n",
                            (int)(i));
                    }
                    *problemstatus = -3;
                    *somethingchanged = ae_true;
                    result = ae_false;
                    return result;
                }
                
                /*
                 * Mark row as dropped and issue transformation
                 */
                isdroppedrow->ptr.p_bool[i] = ae_true;
                *somethingchanged = ae_true;
                *cntempty = *cntempty+1;
                lpqppresolve_presolverappenddropemptyrow(trfstack, i, _state);
                continue;
            }
            
            /*
             * No bounds
             */
            if( ae_isneginf(al->ptr.p_double[i], _state)&&ae_isposinf(au->ptr.p_double[i], _state) )
            {
                
                /*
                 * Add row and column containing its elements to the cleanup list
                 */
                nisaddelement(&buf->setm, i, _state);
                for(jj=a->rowbegin.ptr.p_int[i]; jj<=a->rowend.ptr.p_int[i]-1; jj++)
                {
                    nisaddelement(&buf->setn, a->idx.ptr.p_int[jj], _state);
                }
                
                /*
                 * Mark row as dropped and issue transformation
                 */
                isdroppedrow->ptr.p_bool[i] = ae_true;
                *somethingchanged = ae_true;
                *cntnobounds = *cntnobounds+1;
                lpqppresolve_presolverappenddropemptyrow(trfstack, i, _state);
                continue;
            }
        }
    }
    
    /*
     * Clean up the matrix
     */
    for(ii=0; ii<=buf->setm.nstored-1; ii++)
    {
        a->rowend.ptr.p_int[buf->setm.items.ptr.p_int[ii]] = a->rowbegin.ptr.p_int[buf->setm.items.ptr.p_int[ii]];
    }
    for(jj=0; jj<=buf->setn.nstored-1; jj++)
    {
        lpqppresolve_dyncrsremovesetfromrow(at, buf->setn.items.ptr.p_int[jj], &buf->setm, _state);
    }
    return result;
}


/*************************************************************************
This function scans rows and converts singleton rows to box constraints.

INPUT PARAMETERS:
    BndL, BndU  -   array[N], box constraints.
    IsDroppedCol-   array[N], column statuses, used for integrity checks
    N           -   variables count (including both fixed and non-fixed)
    A, AT       -   current A and AT, dynamic CRS matrices
    AL, AU      -   array[M], lower/upper bounds for linear constraints
    M           -   linear constraints count
    Eps         -   tolerance used to resolve infeasibilities
    DoTrace     -   whether tracing is needed or not
    SomethingChanged-flag variable
    CntFixed    -   debug counter, updated by the function
    trfStack    -   sequence of already applied transformations
                    
OUTPUT PARAMETERS:
    IsDroppedCol-   array[N], dropped columns are marked
    LagrangeFromResidual-
                    array[N], dropped cols are marked
    ProblemStatus-  on failure (unboundedness detected) is set to -2,
                    unchanged otherwise
    SomethingChanged-is set to True if at least one col was dropped
                    It is not changed otherwise.
    CntDropped  -   debug counter, updated by the function
    trfStack -      updated with new transforms
    
RESULT:
    if unboundedness was detected, False is returned.
    True is returned otherwise.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static ae_bool lpqppresolve_singletonrowtobc(/* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* isdroppedcol,
     ae_int_t n,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     /* Boolean */ ae_vector* isdroppedrow,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntsingleton,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ii;
    ae_int_t offs;
    double v;
    double prevbndl;
    double prevbndu;
    double wrkal;
    double wrkau;
    double swapsign;
    double midpoint;
    ae_bool lowerboundisbc;
    ae_bool upperboundisbc;
    ae_bool result;


    result = ae_true;
    for(i=0; i<=m-1; i++)
    {
        if( a->rowbegin.ptr.p_int[i]+1==a->rowend.ptr.p_int[i]&&(ae_isfinite(al->ptr.p_double[i], _state)||ae_isfinite(au->ptr.p_double[i], _state)) )
        {
            
            /*
             * Read singleton row, perform integrity checks and normalization
             */
            j = a->idx.ptr.p_int[a->rowbegin.ptr.p_int[i]];
            v = a->vals.ptr.p_double[a->rowbegin.ptr.p_int[i]];
            ae_assert(!isdroppedrow->ptr.p_bool[i], "SingletonRowToBC: integrity check 6363 failed", _state);
            ae_assert(!isdroppedcol->ptr.p_bool[j], "SingletonRowToBC: integrity check 6364 failed", _state);
            ae_assert(ae_fp_neq(v,(double)(0)), "SingletonRowToBC: integrity check 6366 failed", _state);
            wrkal = al->ptr.p_double[i];
            wrkau = au->ptr.p_double[i];
            swapsign = (double)(1);
            if( ae_fp_less(v,(double)(0)) )
            {
                swapr(&wrkal, &wrkau, _state);
                wrkal = -wrkal;
                wrkau = -wrkau;
                v = -v;
                swapsign = (double)(-1);
            }
            wrkal = wrkal/v;
            wrkau = wrkau/v;
            prevbndl = bndl->ptr.p_double[j];
            prevbndu = bndu->ptr.p_double[j];
            
            /*
             * Check feasibility
             */
            if( ae_fp_greater(prevbndl,prevbndu+eps) )
            {
                if( dotrace )
                {
                    ae_trace("> the problem is infeasible (box constraint for variable %0d)\n",
                        (int)(j));
                }
                *problemstatus = -3;
                *somethingchanged = ae_true;
                result = ae_false;
                return result;
            }
            if( ae_fp_greater(wrkal,prevbndu+eps)||ae_fp_less(wrkau,prevbndl-eps) )
            {
                if( dotrace )
                {
                    ae_trace("> the problem is infeasible (singleton row %0d is incompatible with box constraints for variable %0d)\n",
                        (int)(i),
                        (int)(j));
                }
                *problemstatus = -3;
                *somethingchanged = ae_true;
                result = ae_false;
                return result;
            }
            
            /*
             * Modify problem, perform constraint harmonization for slightly infeasible box constraints
             */
            lowerboundisbc = ae_true;
            upperboundisbc = ae_true;
            if( ae_isfinite(wrkal, _state)&&ae_fp_greater(wrkal,prevbndl) )
            {
                bndl->ptr.p_double[j] = wrkal;
                lowerboundisbc = ae_false;
            }
            if( ae_isfinite(wrkau, _state)&&ae_fp_less(wrkau,prevbndu) )
            {
                bndu->ptr.p_double[j] = wrkau;
                upperboundisbc = ae_false;
            }
            if( ae_fp_less(bndu->ptr.p_double[j],bndl->ptr.p_double[j]) )
            {
                midpoint = 0.5*(bndu->ptr.p_double[j]+bndl->ptr.p_double[j]);
                bndl->ptr.p_double[j] = midpoint;
                bndu->ptr.p_double[j] = midpoint;
            }
            a->rowend.ptr.p_int[i] = a->rowbegin.ptr.p_int[i];
            offs = at->rowbegin.ptr.p_int[j];
            for(ii=at->rowbegin.ptr.p_int[j]; ii<=at->rowend.ptr.p_int[j]-1; ii++)
            {
                if( at->idx.ptr.p_int[ii]!=i )
                {
                    at->idx.ptr.p_int[offs] = at->idx.ptr.p_int[ii];
                    at->vals.ptr.p_double[offs] = at->vals.ptr.p_double[ii];
                    offs = offs+1;
                }
            }
            at->rowend.ptr.p_int[j] = offs;
            isdroppedrow->ptr.p_bool[i] = ae_true;
            *somethingchanged = ae_true;
            *cntsingleton = *cntsingleton+1;
            lpqppresolve_presolverappendsingletonrow(trfstack, i, j, v, swapsign, bndl->ptr.p_double[j], lowerboundisbc, bndu->ptr.p_double[j], upperboundisbc, _state);
        }
    }
    return result;
}


/*************************************************************************
This function tries to process singleton cols using various heuristics:
* detect explicit slacks

INPUT PARAMETERS:
    C. BndL, BndU-  array[N], cost and box constraints.
    IsDroppedCol-   array[N], column statuses, used for integrity checks
    N           -   variables count (including both fixed and non-fixed)
    A, AT       -   current A and AT, dynamic CRS matrices
    AL, AU      -   array[M], lower/upper bounds for linear constraints
    M           -   linear constraints count
    Eps         -   tolerance used to resolve infeasibilities
    DoTrace     -   whether tracing is needed or not
    SomethingChanged-flag variable
    CntFixed    -   debug counter, updated by the function
    trfStack    -   sequence of already applied transformations
                    
OUTPUT PARAMETERS:
    C, BndL, BndU-  may be modified
    IsDroppedCol-   array[N], dropped columns are marked
    LagrangeFromResidual-
                    array[N], dropped cols are marked
    ProblemStatus-  on failure (unboundedness detected) is set to -2,
                    unchanged otherwise
    SomethingChanged-is set to True if at least one col was dropped
                    It is not changed otherwise.
    CntSlackVars-   debug counter, updated by the function
    trfStack -      updated with new transforms
    
RESULT:
    if infeasibility or unboundedness was detected, False is returned.
    True is returned otherwise.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static ae_bool lpqppresolve_singletoncols(/* Real    */ ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ ae_vector* isdroppedcol,
     ae_int_t n,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     /* Boolean */ ae_vector* isdroppedrow,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntslackvars,
     ae_int_t* cntimplicitslacks,
     ae_int_t* cntfreecolumnsingletons,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ii;
    double preval;
    double prevau;
    double prevabnd;
    double aij;
    double aslk;
    double vslk;
    double v;
    double cj;
    ae_bool result;


    result = ae_true;
    for(j=0; j<=n-1; j++)
    {
        if( !isdroppedcol->ptr.p_bool[j]&&at->rowbegin.ptr.p_int[j]+1==at->rowend.ptr.p_int[j] )
        {
            i = at->idx.ptr.p_int[at->rowbegin.ptr.p_int[j]];
            aij = at->vals.ptr.p_double[at->rowbegin.ptr.p_int[j]];
            
            /*
             * Skip rows with no bounds, at least one bound is expected for the transformations below
             */
            if( !(ae_isfinite(al->ptr.p_double[i], _state)||ae_isfinite(au->ptr.p_double[i], _state)) )
            {
                continue;
            }
            
            /*
             * Is it an explicit slack?
             */
            if( ae_fp_less_eq(ae_fabs(c->ptr.p_double[j], _state),eps) )
            {
                
                /*
                 * Clear Cj
                 * Update constraint bounds
                 * Issue "explicit slack" transformation
                 */
                c->ptr.p_double[j] = (double)(0);
                preval = al->ptr.p_double[i];
                prevau = au->ptr.p_double[i];
                if( ae_fp_greater(aij,(double)(0)) )
                {
                    if( ae_isfinite(bndl->ptr.p_double[j], _state) )
                    {
                        au->ptr.p_double[i] = au->ptr.p_double[i]-aij*bndl->ptr.p_double[j];
                    }
                    else
                    {
                        au->ptr.p_double[i] = _state->v_posinf;
                    }
                    if( ae_isfinite(bndu->ptr.p_double[j], _state) )
                    {
                        al->ptr.p_double[i] = al->ptr.p_double[i]-aij*bndu->ptr.p_double[j];
                    }
                    else
                    {
                        al->ptr.p_double[i] = _state->v_neginf;
                    }
                }
                else
                {
                    if( ae_isfinite(bndu->ptr.p_double[j], _state) )
                    {
                        au->ptr.p_double[i] = au->ptr.p_double[i]-aij*bndu->ptr.p_double[j];
                    }
                    else
                    {
                        au->ptr.p_double[i] = _state->v_posinf;
                    }
                    if( ae_isfinite(bndl->ptr.p_double[j], _state) )
                    {
                        al->ptr.p_double[i] = al->ptr.p_double[i]-aij*bndl->ptr.p_double[j];
                    }
                    else
                    {
                        al->ptr.p_double[i] = _state->v_neginf;
                    }
                }
                lpqppresolve_presolverappendexplicitslack(trfstack, i, j, aij, bndl->ptr.p_double[j], bndu->ptr.p_double[j], preval, prevau, a, _state);
                
                /*
                 * Remove variable from A and AT
                 */
                lpqppresolve_dyncrsremoverow(at, j, _state);
                lpqppresolve_dyncrsremovefromrow(a, i, j, _state);
                isdroppedcol->ptr.p_bool[j] = ae_true;
                
                /*
                 * Done
                 */
                *somethingchanged = ae_true;
                *cntslackvars = *cntslackvars+1;
                continue;
            }
            
            /*
             * Is it equality row? The variable can be treated as an implicit slack
             */
            if( (ae_isfinite(al->ptr.p_double[i], _state)&&ae_isfinite(au->ptr.p_double[i], _state))&&ae_fp_less_eq(ae_fabs(al->ptr.p_double[i]-au->ptr.p_double[i], _state),eps) )
            {
                
                /*
                 * Enforce AL=AU, set Cj to zero
                 */
                prevabnd = 0.5*(al->ptr.p_double[i]+au->ptr.p_double[i]);
                al->ptr.p_double[i] = prevabnd;
                au->ptr.p_double[i] = prevabnd;
                cj = c->ptr.p_double[j];
                v = c->ptr.p_double[j]/aij;
                for(ii=a->rowbegin.ptr.p_int[i]; ii<=a->rowend.ptr.p_int[i]-1; ii++)
                {
                    c->ptr.p_double[a->idx.ptr.p_int[ii]] = c->ptr.p_double[a->idx.ptr.p_int[ii]]-v*a->vals.ptr.p_double[ii];
                }
                c->ptr.p_double[j] = (double)(0);
                
                /*
                 * Update constraint bounds
                 * Issue "implicit slack" transformation
                 */
                if( ae_fp_greater(aij,(double)(0)) )
                {
                    if( ae_isfinite(bndl->ptr.p_double[j], _state) )
                    {
                        au->ptr.p_double[i] = au->ptr.p_double[i]-aij*bndl->ptr.p_double[j];
                    }
                    else
                    {
                        au->ptr.p_double[i] = _state->v_posinf;
                    }
                    if( ae_isfinite(bndu->ptr.p_double[j], _state) )
                    {
                        al->ptr.p_double[i] = al->ptr.p_double[i]-aij*bndu->ptr.p_double[j];
                    }
                    else
                    {
                        al->ptr.p_double[i] = _state->v_neginf;
                    }
                }
                else
                {
                    if( ae_isfinite(bndu->ptr.p_double[j], _state) )
                    {
                        au->ptr.p_double[i] = au->ptr.p_double[i]-aij*bndu->ptr.p_double[j];
                    }
                    else
                    {
                        au->ptr.p_double[i] = _state->v_posinf;
                    }
                    if( ae_isfinite(bndl->ptr.p_double[j], _state) )
                    {
                        al->ptr.p_double[i] = al->ptr.p_double[i]-aij*bndl->ptr.p_double[j];
                    }
                    else
                    {
                        al->ptr.p_double[i] = _state->v_neginf;
                    }
                }
                lpqppresolve_presolverappendimplicitslack(trfstack, i, j, aij, cj, prevabnd, a, _state);
                
                /*
                 * Remove variable from A and AT, set Cj to zero
                 */
                lpqppresolve_dyncrsremoverow(at, j, _state);
                lpqppresolve_dyncrsremovefromrow(a, i, j, _state);
                isdroppedcol->ptr.p_bool[j] = ae_true;
                
                /*
                 * Done
                 */
                *somethingchanged = ae_true;
                *cntimplicitslacks = *cntimplicitslacks+1;
                continue;
            }
            
            /*
             * Is it a free column singleton?
             */
            if( !ae_isfinite(bndl->ptr.p_double[j], _state)&&!ae_isfinite(bndu->ptr.p_double[j], _state) )
            {
                
                /*
                 * Handle single-sided linear constraints or two-sided one.
                 *
                 * A general inequality/range constraint is converted to an equality one by
                 * adding temporary slack variable SLK. This variable is not actually added
                 * to the problem - we analytically remove it right after the addition.
                 *
                 * See 3.2.2 from "A modular presolve procedure for large scale linear programming"
                 * by Swietanowski for more information
                 */
                if( !ae_isfinite(al->ptr.p_double[i], _state)||!ae_isfinite(au->ptr.p_double[i], _state) )
                {
                    
                    /*
                     * Single-sided constraint
                     *
                     * Determine multiplier for the artificial temporary slack variable S.
                     * Check for unboundedness. If bounded, the slack S is zero, right-hand
                     * side of the equality constraint is not modified.
                     */
                    aslk = rcase2(ae_isfinite(au->ptr.p_double[i], _state), (double)(1), (double)(-1), _state);
                    if( ae_fp_greater(aslk*c->ptr.p_double[j]/aij,eps) )
                    {
                        if( dotrace )
                        {
                            ae_trace("> the problem is unbounded (deduced from free column singleton analysis for variable %0d, row %0d)\n",
                                (int)(j),
                                (int)(i));
                        }
                        *problemstatus = -2;
                        *somethingchanged = ae_true;
                        result = ae_false;
                        return result;
                    }
                    vslk = (double)(0);
                    if( !ae_isfinite(au->ptr.p_double[i], _state) )
                    {
                        au->ptr.p_double[i] = al->ptr.p_double[i];
                    }
                }
                else
                {
                    
                    /*
                     * Two-sided constraint
                     *
                     * Determine value of the temporary slack variable, modify right-hand side
                     * of the constraint.
                     */
                    vslk = rcase2(ae_fp_greater(c->ptr.p_double[j]/aij,(double)(0)), au->ptr.p_double[i]-al->ptr.p_double[i], 0.0, _state);
                }
                cj = c->ptr.p_double[j];
                v = cj/aij;
                for(ii=a->rowbegin.ptr.p_int[i]; ii<=a->rowend.ptr.p_int[i]-1; ii++)
                {
                    c->ptr.p_double[a->idx.ptr.p_int[ii]] = c->ptr.p_double[a->idx.ptr.p_int[ii]]-v*a->vals.ptr.p_double[ii];
                }
                c->ptr.p_double[j] = (double)(0);
                lpqppresolve_presolverappendimplicitslack(trfstack, i, j, aij, cj, au->ptr.p_double[i]-vslk, a, _state);
                al->ptr.p_double[i] = _state->v_neginf;
                au->ptr.p_double[i] = _state->v_posinf;
                lpqppresolve_dyncrsremoverow(at, j, _state);
                lpqppresolve_dyncrsremovefromrow(a, i, j, _state);
                isdroppedcol->ptr.p_bool[j] = ae_true;
                *cntfreecolumnsingletons = *cntfreecolumnsingletons+1;
                continue;
            }
        }
    }
    return result;
}


/*************************************************************************
This function scans box constraints list and tries  to  fix  variables  by
removing them from the A and AT matrices and by  recording  transformation
in trfStack.

If infeasibility is detected, returns False.

INPUT PARAMETERS:
    BndL, BndU  -   array[N], current set of box constraints. Only entries
                    with IsDroppedCol[]=False are examined
    IsDroppedCol-   array[N], column statuses; only non-dropped ones are examined.
    N           -   variables count (including both fixed and non-fixed)
    A, AT       -   current A and AT, dynamic CRS matrices
    AL, AU      -   array[M], lower/upper bounds for linear constraints
    M           -   linear constraints count
    Eps         -   tolerance to resolve slightly infeasible constraints
    DoTrace     -   whether tracing is needed or not
    SomethingChanged-flag variable
    CntFixed    -   debug counter, updated by the function
                    
OUTPUT PARAMETERS:
    IsDroppedCol-   array[N], fixed vars are marked
    LagrangeFromResidual-
                    array[N], fixed vars are marked
    A           -   columns corresponding to fixed vars are removed from A.
                    The matrix size does not change.
    AT          -   rows corresponding to fixed vars are removed from A.
                    The matrix size does not change.
    AL, AU      -   updated with fixed values
    SomethingChanged-is set to True if at least one variable was fixed.
                    It is not changed otherwise.
    CntFixed    -   debug counter, updated by the function

RESULT:
    if infeasibility is detected, returns False.
    True otherwise.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static ae_bool lpqppresolve_fixvariables(/* Real    */ ae_vector* c,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     /* Boolean */ ae_vector* isdroppedcol,
     ae_int_t n,
     dynamiccrs* a,
     dynamiccrs* at,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t m,
     double eps,
     ae_bool dotrace,
     presolvebuffers* buf,
     presolverstack* trfstack,
     ae_int_t* problemstatus,
     ae_bool* somethingchanged,
     ae_int_t* cntfixed,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t ii;
    ae_int_t jj;
    ae_int_t offs;
    double v;
    ae_bool result;


    result = ae_true;
    
    /*
     * Scan variables, determine rows to update
     */
    nisclear(&buf->setn, _state);
    nisclear(&buf->setm, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ((!isdroppedcol->ptr.p_bool[i]&&ae_isfinite(bndl->ptr.p_double[i], _state))&&ae_isfinite(bndu->ptr.p_double[i], _state))&&ae_fp_greater_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]-eps) )
        {
            
            /*
             * Detect infeasibility
             */
            if( ae_fp_greater(bndl->ptr.p_double[i],bndu->ptr.p_double[i]+eps) )
            {
                *somethingchanged = ae_true;
                *problemstatus = -3;
                result = ae_false;
                return result;
            }
            
            /*
             * Fix variable and issue transformation
             */
            isdroppedcol->ptr.p_bool[i] = ae_true;
            *somethingchanged = ae_true;
            *cntfixed = *cntfixed+1;
            lpqppresolve_presolverappendfixedvar(trfstack, i, boundval(0.5*(bndl->ptr.p_double[i]+bndu->ptr.p_double[i]), bndl->ptr.p_double[i], bndu->ptr.p_double[i], _state), c->ptr.p_double[i], at, _state);
            
            /*
             * Save dependent row indexes to SetM
             */
            nisaddelement(&buf->setn, i, _state);
            for(jj=at->rowbegin.ptr.p_int[i]; jj<=at->rowend.ptr.p_int[i]-1; jj++)
            {
                nisaddelement(&buf->setm, at->idx.ptr.p_int[jj], _state);
            }
        }
    }
    if( !*somethingchanged )
    {
        return result;
    }
    
    /*
     * Clear columns of AT corresponding to fixed variables
     */
    for(jj=0; jj<=buf->setn.nstored-1; jj++)
    {
        i = buf->setn.items.ptr.p_int[jj];
        at->rowend.ptr.p_int[i] = at->rowend.ptr.p_int[i];
    }
    
    /*
     * Scan marked rows of A and remove fixed variables
     */
    for(ii=0; ii<=buf->setm.nstored-1; ii++)
    {
        i = buf->setm.items.ptr.p_int[ii];
        offs = a->rowbegin.ptr.p_int[i];
        v = (double)(0);
        for(jj=a->rowbegin.ptr.p_int[i]; jj<=a->rowend.ptr.p_int[i]-1; jj++)
        {
            j = a->idx.ptr.p_int[jj];
            if( buf->setn.locationof.ptr.p_int[j]>=0 )
            {
                
                /*
                 * Update constraint bounds with fixed variables, remove from linear constraint
                 */
                v = v+bndl->ptr.p_double[j]*a->vals.ptr.p_double[jj];
            }
            else
            {
                
                /*
                 * Non-fixed variable, retain it
                 */
                a->idx.ptr.p_int[offs] = j;
                a->vals.ptr.p_double[offs] = a->vals.ptr.p_double[jj];
                offs = offs+1;
            }
        }
        a->rowend.ptr.p_int[i] = offs;
        if( ae_isfinite(al->ptr.p_double[i], _state) )
        {
            al->ptr.p_double[i] = al->ptr.p_double[i]-v;
        }
        if( ae_isfinite(au->ptr.p_double[i], _state) )
        {
            au->ptr.p_double[i] = au->ptr.p_double[i]-v;
        }
    }
    return result;
}


/*************************************************************************
Scale cost vector and constraints, normalize cost.

  -- ALGLIB --
     Copyright 01.07.2022 by Bochkanov Sergey
*************************************************************************/
static void lpqppresolve_scalecostandconstraints(/* Real    */ const ae_vector* s,
     ae_int_t n,
     /* Real    */ ae_vector* c,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     sparsematrix* sparsea,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_int_t m,
     presolverstack* trfstack,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    double costscale;
    double avgln;
    double v;
    double rowscale;


    
    /*
     * Apply scaling to cost, determine additional scaling due to normalization 
     */
    avgln = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        c->ptr.p_double[i] = c->ptr.p_double[i]*s->ptr.p_double[i];
        avgln = avgln+ae_log((double)1+ae_fabs(c->ptr.p_double[i], _state), _state);
    }
    costscale = ae_exp(avgln/(double)n, _state);
    rmulv(n, (double)1/costscale, c, _state);
    lpqppresolve_presolverappendcostscaling(trfstack, (double)1/costscale, _state);
    
    /*
     * Apply column scaling to BndL/BndU, output backward transformation
     */
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Transform problem
         */
        lpqppresolve_presolverappendcolscaling(trfstack, i, (double)1/s->ptr.p_double[i], _state);
        bndl->ptr.p_double[i] = bndl->ptr.p_double[i]/s->ptr.p_double[i];
        bndu->ptr.p_double[i] = bndu->ptr.p_double[i]/s->ptr.p_double[i];
    }
    for(i=0; i<=m-1; i++)
    {
        rowscale = (double)(0);
        j0 = sparsea->ridx.ptr.p_int[i];
        j1 = sparsea->ridx.ptr.p_int[i+1]-1;
        for(j=j0; j<=j1; j++)
        {
            v = s->ptr.p_double[sparsea->idx.ptr.p_int[j]]*sparsea->vals.ptr.p_double[j];
            sparsea->vals.ptr.p_double[j] = v;
            rowscale = ae_maxreal(rowscale, ae_fabs(v, _state), _state);
        }
        rowscale = ae_maxreal(rowscale, 1.0, _state);
        
        /*
         * Apply transformation to A/AL/AU
         */
        v = (double)1/rowscale;
        for(j=j0; j<=j1; j++)
        {
            sparsea->vals.ptr.p_double[j] = v*sparsea->vals.ptr.p_double[j];
        }
        al->ptr.p_double[i] = al->ptr.p_double[i]*v;
        au->ptr.p_double[i] = au->ptr.p_double[i]*v;
        lpqppresolve_presolverappendrowscaling(trfstack, i, (double)1/rowscale, _state);
    }
}


void _dynamiccrs_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    dynamiccrs *p = (dynamiccrs*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->rowbegin, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->rowend, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->idx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->vals, 0, DT_REAL, _state, make_automatic);
}


void _dynamiccrs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    dynamiccrs       *dst = (dynamiccrs*)_dst;
    const dynamiccrs *src = (const dynamiccrs*)_src;
    dst->m = src->m;
    dst->n = src->n;
    ae_vector_init_copy(&dst->rowbegin, &src->rowbegin, _state, make_automatic);
    ae_vector_init_copy(&dst->rowend, &src->rowend, _state, make_automatic);
    ae_vector_init_copy(&dst->idx, &src->idx, _state, make_automatic);
    ae_vector_init_copy(&dst->vals, &src->vals, _state, make_automatic);
}


void _dynamiccrs_clear(void* _p)
{
    dynamiccrs *p = (dynamiccrs*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->rowbegin);
    ae_vector_clear(&p->rowend);
    ae_vector_clear(&p->idx);
    ae_vector_clear(&p->vals);
}


void _dynamiccrs_destroy(void* _p)
{
    dynamiccrs *p = (dynamiccrs*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->rowbegin);
    ae_vector_destroy(&p->rowend);
    ae_vector_destroy(&p->idx);
    ae_vector_destroy(&p->vals);
}


void _presolvebuffers_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    presolvebuffers *p = (presolvebuffers*)_p;
    ae_touch_ptr((void*)p);
    _niset_init(&p->setn, _state, make_automatic);
    _niset_init(&p->setm, _state, make_automatic);
}


void _presolvebuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    presolvebuffers       *dst = (presolvebuffers*)_dst;
    const presolvebuffers *src = (const presolvebuffers*)_src;
    _niset_init_copy(&dst->setn, &src->setn, _state, make_automatic);
    _niset_init_copy(&dst->setm, &src->setm, _state, make_automatic);
}


void _presolvebuffers_clear(void* _p)
{
    presolvebuffers *p = (presolvebuffers*)_p;
    ae_touch_ptr((void*)p);
    _niset_clear(&p->setn);
    _niset_clear(&p->setm);
}


void _presolvebuffers_destroy(void* _p)
{
    presolvebuffers *p = (presolvebuffers*)_p;
    ae_touch_ptr((void*)p);
    _niset_destroy(&p->setn);
    _niset_destroy(&p->setm);
}


void _presolverstack_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    presolverstack *p = (presolverstack*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->trftype, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->idata, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->rdata, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->idataridx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->rdataridx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->sparseidx0, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->sparseval0, 0, DT_REAL, _state, make_automatic);
}


void _presolverstack_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    presolverstack       *dst = (presolverstack*)_dst;
    const presolverstack *src = (const presolverstack*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->ntrf = src->ntrf;
    ae_vector_init_copy(&dst->trftype, &src->trftype, _state, make_automatic);
    ae_vector_init_copy(&dst->idata, &src->idata, _state, make_automatic);
    ae_vector_init_copy(&dst->rdata, &src->rdata, _state, make_automatic);
    ae_vector_init_copy(&dst->idataridx, &src->idataridx, _state, make_automatic);
    ae_vector_init_copy(&dst->rdataridx, &src->rdataridx, _state, make_automatic);
    dst->sourceidx = src->sourceidx;
    dst->isrc = src->isrc;
    dst->rsrc = src->rsrc;
    ae_vector_init_copy(&dst->sparseidx0, &src->sparseidx0, _state, make_automatic);
    ae_vector_init_copy(&dst->sparseval0, &src->sparseval0, _state, make_automatic);
}


void _presolverstack_clear(void* _p)
{
    presolverstack *p = (presolverstack*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->trftype);
    ae_vector_clear(&p->idata);
    ae_vector_clear(&p->rdata);
    ae_vector_clear(&p->idataridx);
    ae_vector_clear(&p->rdataridx);
    ae_vector_clear(&p->sparseidx0);
    ae_vector_clear(&p->sparseval0);
}


void _presolverstack_destroy(void* _p)
{
    presolverstack *p = (presolverstack*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->trftype);
    ae_vector_destroy(&p->idata);
    ae_vector_destroy(&p->rdata);
    ae_vector_destroy(&p->idataridx);
    ae_vector_destroy(&p->rdataridx);
    ae_vector_destroy(&p->sparseidx0);
    ae_vector_destroy(&p->sparseval0);
}


void _presolveinfo_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    presolveinfo *p = (presolveinfo*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->rawc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawbndu, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->rawa, _state, make_automatic);
    ae_vector_init(&p->lagrangefromresidual, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->c, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->bndu, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsea, _state, make_automatic);
    ae_vector_init(&p->al, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->au, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->packxperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->packyperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->packstatperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->unpackxperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->unpackyperm, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->unpackstatperm, 0, DT_INT, _state, make_automatic);
    _presolverstack_init(&p->trfstack, _state, make_automatic);
    ae_vector_init(&p->s1, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->bc1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->y1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    _presolvebuffers_init(&p->buf, _state, make_automatic);
}


void _presolveinfo_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    presolveinfo       *dst = (presolveinfo*)_dst;
    const presolveinfo *src = (const presolveinfo*)_src;
    dst->newn = src->newn;
    dst->oldn = src->oldn;
    dst->newm = src->newm;
    dst->oldm = src->oldm;
    ae_vector_init_copy(&dst->rawc, &src->rawc, _state, make_automatic);
    ae_vector_init_copy(&dst->rawbndl, &src->rawbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->rawbndu, &src->rawbndu, _state, make_automatic);
    _sparsematrix_init_copy(&dst->rawa, &src->rawa, _state, make_automatic);
    dst->problemstatus = src->problemstatus;
    ae_vector_init_copy(&dst->lagrangefromresidual, &src->lagrangefromresidual, _state, make_automatic);
    ae_vector_init_copy(&dst->c, &src->c, _state, make_automatic);
    ae_vector_init_copy(&dst->bndl, &src->bndl, _state, make_automatic);
    ae_vector_init_copy(&dst->bndu, &src->bndu, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsea, &src->sparsea, _state, make_automatic);
    ae_vector_init_copy(&dst->al, &src->al, _state, make_automatic);
    ae_vector_init_copy(&dst->au, &src->au, _state, make_automatic);
    ae_vector_init_copy(&dst->packxperm, &src->packxperm, _state, make_automatic);
    ae_vector_init_copy(&dst->packyperm, &src->packyperm, _state, make_automatic);
    ae_vector_init_copy(&dst->packstatperm, &src->packstatperm, _state, make_automatic);
    ae_vector_init_copy(&dst->unpackxperm, &src->unpackxperm, _state, make_automatic);
    ae_vector_init_copy(&dst->unpackyperm, &src->unpackyperm, _state, make_automatic);
    ae_vector_init_copy(&dst->unpackstatperm, &src->unpackstatperm, _state, make_automatic);
    _presolverstack_init_copy(&dst->trfstack, &src->trfstack, _state, make_automatic);
    ae_vector_init_copy(&dst->s1, &src->s1, _state, make_automatic);
    ae_vector_init_copy(&dst->bc1, &src->bc1, _state, make_automatic);
    ae_vector_init_copy(&dst->x1, &src->x1, _state, make_automatic);
    ae_vector_init_copy(&dst->y1, &src->y1, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    _presolvebuffers_init_copy(&dst->buf, &src->buf, _state, make_automatic);
}


void _presolveinfo_clear(void* _p)
{
    presolveinfo *p = (presolveinfo*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->rawc);
    ae_vector_clear(&p->rawbndl);
    ae_vector_clear(&p->rawbndu);
    _sparsematrix_clear(&p->rawa);
    ae_vector_clear(&p->lagrangefromresidual);
    ae_vector_clear(&p->c);
    ae_vector_clear(&p->bndl);
    ae_vector_clear(&p->bndu);
    _sparsematrix_clear(&p->sparsea);
    ae_vector_clear(&p->al);
    ae_vector_clear(&p->au);
    ae_vector_clear(&p->packxperm);
    ae_vector_clear(&p->packyperm);
    ae_vector_clear(&p->packstatperm);
    ae_vector_clear(&p->unpackxperm);
    ae_vector_clear(&p->unpackyperm);
    ae_vector_clear(&p->unpackstatperm);
    _presolverstack_clear(&p->trfstack);
    ae_vector_clear(&p->s1);
    ae_vector_clear(&p->bc1);
    ae_vector_clear(&p->x1);
    ae_vector_clear(&p->y1);
    ae_vector_clear(&p->d);
    _presolvebuffers_clear(&p->buf);
}


void _presolveinfo_destroy(void* _p)
{
    presolveinfo *p = (presolveinfo*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->rawc);
    ae_vector_destroy(&p->rawbndl);
    ae_vector_destroy(&p->rawbndu);
    _sparsematrix_destroy(&p->rawa);
    ae_vector_destroy(&p->lagrangefromresidual);
    ae_vector_destroy(&p->c);
    ae_vector_destroy(&p->bndl);
    ae_vector_destroy(&p->bndu);
    _sparsematrix_destroy(&p->sparsea);
    ae_vector_destroy(&p->al);
    ae_vector_destroy(&p->au);
    ae_vector_destroy(&p->packxperm);
    ae_vector_destroy(&p->packyperm);
    ae_vector_destroy(&p->packstatperm);
    ae_vector_destroy(&p->unpackxperm);
    ae_vector_destroy(&p->unpackyperm);
    ae_vector_destroy(&p->unpackstatperm);
    _presolverstack_destroy(&p->trfstack);
    ae_vector_destroy(&p->s1);
    ae_vector_destroy(&p->bc1);
    ae_vector_destroy(&p->x1);
    ae_vector_destroy(&p->y1);
    ae_vector_destroy(&p->d);
    _presolvebuffers_destroy(&p->buf);
}


/*$ End $*/
