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
#include "nlcaul.h"


/*$ Declarations $*/
static double nlcaul_defaultinequalitylag = 1.0;
static double nlcaul_maxlagmult = 1.0E7;
static double nlcaul_minlagmult = 1.0E-10;
static double nlcaul_besterrstagnationcoeff = 0.75;
static ae_int_t nlcaul_besterrstagnationlimit = 10;
static double nlcaul_stagnationepsf = 1.0E-12;
static double nlcaul_stabilizingpoint = 10.0;
static ae_int_t nlcaul_fstagnationlimit = 20;
static double nlcaul_aulmaxgrowth = 10.0;
static double nlcaul_rhomax = 10000;
static double nlcaul_gtol = 0.25;
static double nlcaul_auldeltadecrease = 0.50;
static double nlcaul_auldeltaincrease = 0.99;
static double nlcaul_aulbigscale = 5.0;
static double nlcaul_aulsmallscale = 0.2;
static ae_int_t nlcaul_xbfgsmemlen = 8;
static double nlcaul_xbfgsmaxhess = 1.0E3;
static void nlcaul_preconditionerinit(const minaulstate* sstate,
     ae_int_t refreshfreq,
     minaulpreconditioner* p,
     ae_state *_state);
static void nlcaul_preconditionersendupdates(minaulstate* state,
     minaulpreconditioner* prec,
     const varsfuncjac* current,
     double rho,
     /* Real    */ const ae_vector* lagmultbc2,
     /* Real    */ const ae_vector* lagmultxc2,
     xbfgshessian* hesstgt,
     xbfgshessian* hessaug,
     ae_state *_state);
static void nlcaul_preconditionersolve(minaulstate* state,
     minaulpreconditioner* prec,
     /* Real    */ const ae_vector* g,
     /* Real    */ ae_vector* d,
     ae_state *_state);
static void nlcaul_aulsendvirtualpoint(minaulstate* state,
     /* Real    */ const ae_vector* xvirt,
     /* Real    */ ae_vector* xtrue,
     ae_state *_state);
static void nlcaul_aulsendtruepoint(minaulstate* state,
     /* Real    */ const ae_vector* xtrue,
     ae_state *_state);
static ae_bool nlcaul_aulretrievesj(minaulstate* state,
     varsfuncjac* xvirt,
     varsfuncjac* xtrue,
     ae_state *_state);
static void nlcaul_modtargetfg(minaulstate* state,
     const varsfuncjac* s,
     double rhobc,
     double rhoxc,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state);
static void nlcaul_auglagfg(minaulstate* state,
     const varsfuncjac* s,
     double rho,
     /* Real    */ const ae_vector* lagmultbc2,
     /* Real    */ const ae_vector* lagmultxc2,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state);
static void nlcaul_rawlagfg(minaulstate* state,
     const varsfuncjac* s,
     /* Real    */ const ae_vector* lagmultbc2,
     /* Real    */ const ae_vector* lagmultxc2,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state);
static void nlcaul_clearrequestfields(minaulstate* state,
     ae_state *_state);
static void nlcaul_computeerrors(minaulstate* sstate,
     const varsfuncjac* s,
     double* errprim,
     double* errdual,
     double* errcmpl,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
This function initializes solver state
*************************************************************************/
void minaulinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t cntlc,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t cntnlc,
     const nlpstoppingcriteria* criteria,
     ae_int_t maxouterits,
     minaulstate* state,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(cntlc==0||a->m==cntlc, "AUL: rows(A)<>CntLC", _state);
    ae_assert(maxouterits>=0, "AUL: MaxOuterIts<0", _state);
    
    /*
     * Initialization
     */
    if( maxouterits==0 )
    {
        maxouterits = 20;
    }
    state->n = n;
    state->cntlc = cntlc;
    state->cntnlc = cntnlc;
    
    /*
     * Prepare RCOMM state
     */
    ae_vector_set_length(&state->rstate.ia, 9+1, _state);
    ae_vector_set_length(&state->rstate.ba, 4+1, _state);
    ae_vector_set_length(&state->rstate.ra, 23+1, _state);
    state->rstate.stage = -1;
    state->needsj = ae_false;
    state->precrefreshupcoming = ae_false;
    state->xupdated = ae_false;
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->fi, 1+cntnlc, _state);
    
    /*
     * Allocate memory.
     */
    rallocv(n, &state->x0, _state);
    rsetallocv(n, 1.0, &state->s, _state);
    rvectorsetlengthatleast(&state->fscales, 1+cntnlc, _state);
    rvectorsetlengthatleast(&state->tracegamma, 1+cntnlc, _state);
    bvectorsetlengthatleast(&state->hasbndl, n, _state);
    bvectorsetlengthatleast(&state->hasbndu, n, _state);
    rvectorsetlengthatleast(&state->scaledbndl, n, _state);
    rvectorsetlengthatleast(&state->scaledbndu, n, _state);
    rallocv(2*n, &state->lagmultbc2, _state);
    rvectorsetlengthatleast(&state->lagmultxc2, 2*(cntlc+cntnlc), _state);
    rallocv(n, &state->d, _state);
    rallocv(n, &state->du, _state);
    rallocv(n, &state->laggradcur, _state);
    rallocv(n, &state->modtgtgrad0, _state);
    rallocv(n, &state->modtgtgrad1, _state);
    
    /*
     * Prepare scaled problem
     */
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
            ae_assert(ae_fp_less_eq(bndl->ptr.p_double[i],bndu->ptr.p_double[i]), "AUL: integrity check failed, box constraints are inconsistent", _state);
        }
        state->x0.ptr.p_double[i] = x0->ptr.p_double[i]/s->ptr.p_double[i];
        state->s.ptr.p_double[i] = s->ptr.p_double[i];
    }
    if( cntlc>0 )
    {
        rsetallocv(n, 0.0, &state->tmpzero, _state);
        sparsecopytocrsbuf(a, &state->sparsea, _state);
        rcopyallocv(cntlc, al, &state->al, _state);
        rcopyallocv(cntlc, au, &state->au, _state);
        icopyallocv(cntlc, lcsrcidx, &state->lcsrcidx, _state);
        scaleshiftmixedlcinplace(s, &state->tmpzero, n, &state->sparsea, cntlc, &state->dummy2, 0, &state->al, &state->au, _state);
        normalizesparselcinplace(&state->sparsea, cntlc, &state->al, &state->au, n, ae_true, &state->ascales, ae_true, _state);
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
    enforceboundaryconstraints(&state->x0, &state->scaledbndl, &state->hasbndl, &state->scaledbndu, &state->hasbndu, n, 0, _state);
    
    /*
     * Stopping criteria and settings
     */
    critcopy(criteria, &state->criteria, _state);
    state->maxouterits = maxouterits;
    state->restartfreq = 5;
    
    /*
     * Report fields
     */
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
    ae_assert(ae_fp_less(nlcaul_auldeltadecrease,nlcaul_auldeltaincrease), "MinAUL: integrity check failed", _state);
}


/*************************************************************************
This function performs actual processing for  AUL  algorithm.  It  expects
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
ae_bool minauliteration(minaulstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_int_t nconstr;
    ae_bool dotrace;
    ae_bool doprobingalways;
    ae_bool dodetailedtrace;
    double lagcur;
    double lag0;
    double lag1;
    double laga;
    double lagb;
    double ci;
    double modtgt0;
    double modtgt1;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t jj;
    double mx;
    double setscaleto;
    double multiplyby;
    ae_int_t mcinfo;
    ae_bool inneriterationsstopped;
    double stpproposed;
    double p;
    double dp;
    double d2p;
    double steplengthtoprobe;
    double v;
    double vv;
    ae_bool stepaccepted;
    double errprim;
    double errdual;
    double errcmpl;
    double preverrprim;
    double preverrdual;
    double preverrcmpl;
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
        nconstr = state->rstate.ia.ptr.p_int[3];
        i = state->rstate.ia.ptr.p_int[4];
        j = state->rstate.ia.ptr.p_int[5];
        j0 = state->rstate.ia.ptr.p_int[6];
        j1 = state->rstate.ia.ptr.p_int[7];
        jj = state->rstate.ia.ptr.p_int[8];
        mcinfo = state->rstate.ia.ptr.p_int[9];
        dotrace = state->rstate.ba.ptr.p_bool[0];
        doprobingalways = state->rstate.ba.ptr.p_bool[1];
        dodetailedtrace = state->rstate.ba.ptr.p_bool[2];
        inneriterationsstopped = state->rstate.ba.ptr.p_bool[3];
        stepaccepted = state->rstate.ba.ptr.p_bool[4];
        lagcur = state->rstate.ra.ptr.p_double[0];
        lag0 = state->rstate.ra.ptr.p_double[1];
        lag1 = state->rstate.ra.ptr.p_double[2];
        laga = state->rstate.ra.ptr.p_double[3];
        lagb = state->rstate.ra.ptr.p_double[4];
        ci = state->rstate.ra.ptr.p_double[5];
        modtgt0 = state->rstate.ra.ptr.p_double[6];
        modtgt1 = state->rstate.ra.ptr.p_double[7];
        mx = state->rstate.ra.ptr.p_double[8];
        setscaleto = state->rstate.ra.ptr.p_double[9];
        multiplyby = state->rstate.ra.ptr.p_double[10];
        stpproposed = state->rstate.ra.ptr.p_double[11];
        p = state->rstate.ra.ptr.p_double[12];
        dp = state->rstate.ra.ptr.p_double[13];
        d2p = state->rstate.ra.ptr.p_double[14];
        steplengthtoprobe = state->rstate.ra.ptr.p_double[15];
        v = state->rstate.ra.ptr.p_double[16];
        vv = state->rstate.ra.ptr.p_double[17];
        errprim = state->rstate.ra.ptr.p_double[18];
        errdual = state->rstate.ra.ptr.p_double[19];
        errcmpl = state->rstate.ra.ptr.p_double[20];
        preverrprim = state->rstate.ra.ptr.p_double[21];
        preverrdual = state->rstate.ra.ptr.p_double[22];
        preverrcmpl = state->rstate.ra.ptr.p_double[23];
    }
    else
    {
        n = 359;
        cntlc = -58;
        cntnlc = -919;
        nconstr = -909;
        i = 81;
        j = 255;
        j0 = 74;
        j1 = -788;
        jj = 809;
        mcinfo = 205;
        dotrace = ae_false;
        doprobingalways = ae_true;
        dodetailedtrace = ae_false;
        inneriterationsstopped = ae_true;
        stepaccepted = ae_true;
        lagcur = -698.0;
        lag0 = -900.0;
        lag1 = -318.0;
        laga = -940.0;
        lagb = 1016.0;
        ci = -229.0;
        modtgt0 = -536.0;
        modtgt1 = 487.0;
        mx = -115.0;
        setscaleto = 886.0;
        multiplyby = 346.0;
        stpproposed = -722.0;
        p = -413.0;
        dp = -461.0;
        d2p = 927.0;
        steplengthtoprobe = 201.0;
        v = 922.0;
        vv = -154.0;
        errprim = 306.0;
        errdual = -1011.0;
        errcmpl = 951.0;
        preverrprim = -463.0;
        preverrdual = 88.0;
        preverrcmpl = -861.0;
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
    
    /*
     * Routine body
     */
    n = state->n;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    nconstr = state->cntlc+state->cntnlc;
    dotrace = ae_is_trace_enabled("AUL");
    dodetailedtrace = dotrace&&ae_is_trace_enabled("AUL.DETAILED");
    doprobingalways = dotrace&&ae_is_trace_enabled("AUL.PROBING");
    
    /*
     * Prepare rcomm interface
     */
    state->needsj = ae_false;
    state->precrefreshupcoming = ae_false;
    state->xupdated = ae_false;
    
    /*
     * Initialize algorithm state.
     *
     * Create two points that will be maintained during iterations:
     * * a 'virtual' point which is used by the AUL solver. This point is
     *   subject to box constraints, but they are enforced only approximately.
     *   Constraint violation in the intermediate points can be very large.
     *   However, the user does not see this point when we send a callback request.
     * * a 'true' point which actually sent to the user - obtained by clipping
     *   the virtual point to the box constrained area.
     */
    for(i=0; i<=cntnlc; i++)
    {
        state->fscales.ptr.p_double[i] = 1.0;
        state->tracegamma.ptr.p_double[i] = 0.0;
    }
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repnfev = 0;
    state->inneriterationscount = 0;
    state->outeriterationscount = 0;
    rsetv(2*n, 0.0, &state->lagmultbc2, _state);
    for(i=0; i<=n-1; i++)
    {
        if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->scaledbndl.ptr.p_double[i],state->scaledbndu.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint
             */
            state->lagmultbc2.ptr.p_double[2*i+0] = 0.0;
            state->lagmultbc2.ptr.p_double[2*i+1] = 0.0;
            continue;
        }
        if( state->hasbndl.ptr.p_bool[i] )
        {
            state->lagmultbc2.ptr.p_double[2*i+0] = nlcaul_defaultinequalitylag;
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            state->lagmultbc2.ptr.p_double[2*i+1] = nlcaul_defaultinequalitylag;
        }
    }
    rsetv(2*(cntlc+cntnlc), 0.0, &state->lagmultxc2, _state);
    for(i=0; i<=cntlc-1; i++)
    {
        if( (state->hasal.ptr.p_bool[i]&&state->hasau.ptr.p_bool[i])&&ae_fp_eq(state->al.ptr.p_double[i],state->au.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint
             */
            state->lagmultxc2.ptr.p_double[2*i+0] = 0.0;
            state->lagmultxc2.ptr.p_double[2*i+1] = 0.0;
            continue;
        }
        if( state->hasal.ptr.p_bool[i] )
        {
            state->lagmultxc2.ptr.p_double[2*i+0] = nlcaul_defaultinequalitylag;
        }
        if( state->hasau.ptr.p_bool[i] )
        {
            state->lagmultxc2.ptr.p_double[2*i+1] = nlcaul_defaultinequalitylag;
        }
    }
    for(i=0; i<=cntnlc-1; i++)
    {
        if( (state->hasnl.ptr.p_bool[i]&&state->hasnu.ptr.p_bool[i])&&ae_fp_eq(state->rawnl.ptr.p_double[i],state->rawnu.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint
             */
            state->lagmultxc2.ptr.p_double[2*cntlc+2*i+0] = 0.0;
            state->lagmultxc2.ptr.p_double[2*cntlc+2*i+1] = 0.0;
            continue;
        }
        if( state->hasnl.ptr.p_bool[i] )
        {
            state->lagmultxc2.ptr.p_double[2*cntlc+2*i+0] = nlcaul_defaultinequalitylag;
        }
        if( state->hasnu.ptr.p_bool[i] )
        {
            state->lagmultxc2.ptr.p_double[2*cntlc+2*i+1] = nlcaul_defaultinequalitylag;
        }
    }
    vfjallocsparse(n, 1+cntnlc, &state->xvirt, _state);
    vfjallocsparse(n, 1+cntnlc, &state->xtrue, _state);
    rcopyv(n, &state->x0, &state->xvirt.x, _state);
    
    /*
     * Evaluate function vector and Jacobian at X0, send first location report.
     * Compute initial violation of constraints.
     */
    nlcaul_aulsendvirtualpoint(state, &state->xvirt.x, &state->xtrue.x, _state);
    state->needsj = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needsj = ae_false;
    if( !nlcaul_aulretrievesj(state, &state->xvirt, &state->xtrue, _state) )
    {
        
        /*
         * Failed to retrieve function/Jacobian, infinities detected!
         */
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    nlcaul_aulsendtruepoint(state, &state->xtrue.x, _state);
    state->f = state->xtrue.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
    checklc2violation(&state->sparsea, &state->al, &state->au, &state->lcsrcidx, cntlc, &state->xtrue.x, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlc2violation(&state->xtrue.fi, &state->fscales, &state->rawnl, &state->rawnu, cntnlc, &state->repnlcerr, &state->repnlcidx, _state);
    
    /*
     * Trace output (if needed)
     */
    if( dotrace )
    {
        ae_trace("\n\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("//  AUL SOLVER STARTED                                                                            //\n");
        ae_trace("////////////////////////////////////////////////////////////////////////////////////////////////////\n");
        ae_trace("N             = %6d\n",
            (int)(n));
        ae_trace("cntLC         = %6d\n",
            (int)(state->cntlc));
        ae_trace("cntNLC        = %6d\n",
            (int)(state->cntnlc));
    }
    
    /*
     * Perform outer (NLC) iterations
     */
    preverrprim = 1.0E50;
    preverrdual = 1.0E50;
    preverrcmpl = 1.0E50;
    state->fstagnationcnt = 0;
    state->longeststp = 0.0;
    state->rho = 500.0;
    hessianinitlowrank(&state->hessaug, n, ae_minint(n, nlcaul_xbfgsmemlen, _state), ae_maxreal(critgetepsx(&state->criteria, _state), ae_sqrt(ae_machineepsilon, _state), _state), nlcaul_xbfgsmaxhess*((double)1+state->rho), _state);
    hessianinitlowrank(&state->hesstgt, n, 1, ae_maxreal(critgetepsx(&state->criteria, _state), ae_sqrt(ae_machineepsilon, _state), _state), nlcaul_xbfgsmaxhess, _state);
    vfjcopy(&state->xvirt, &state->xvirtbest, _state);
    vfjcopy(&state->xtrue, &state->xtruebest, _state);
    state->besterr = ae_maxrealnumber;
    state->bestiteridx = -1;
    state->besterrnegligibleupdates = 0;
    nlcaul_preconditionerinit(state, state->restartfreq, &state->preconditioner, _state);
lbl_6:
    if( ae_false )
    {
        goto lbl_7;
    }
    if( dotrace )
    {
        ae_trace("\n=== INNER ITERATION %5d, OUTER ITERATION %5d ===================================================\n",
            (int)(state->repiterationscount),
            (int)(state->outeriterationscount));
    }
    
    /*
     * Renormalize target function and/or constraints, if some of them have too large magnitudes
     */
    for(i=0; i<=cntnlc; i++)
    {
        
        /*
         * Determine (a) multiplicative coefficient applied to function value
         * and Jacobian row, and (b) new value of the function scale.
         */
        mx = (double)(0);
        j0 = state->xvirt.sj.ridx.ptr.p_int[i];
        j1 = state->xvirt.sj.ridx.ptr.p_int[i+1]-1;
        for(j=j0; j<=j1; j++)
        {
            mx = mx+ae_sqr(state->xvirt.sj.vals.ptr.p_double[j], _state);
        }
        mx = ae_sqrt(mx, _state);
        multiplyby = 1.0;
        setscaleto = state->fscales.ptr.p_double[i];
        if( ae_fp_greater_eq(mx,nlcaul_aulbigscale) )
        {
            multiplyby = (double)1/mx;
            setscaleto = state->fscales.ptr.p_double[i]*mx;
        }
        if( ae_fp_less_eq(mx,nlcaul_aulsmallscale)&&ae_fp_greater(state->fscales.ptr.p_double[i],1.0) )
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
             * * update slack variable and box constraints
             * * update FScales[] and TraceGamma[] arrays
             */
            state->xvirt.fi.ptr.p_double[i] = state->xvirt.fi.ptr.p_double[i]*multiplyby;
            state->xtrue.fi.ptr.p_double[i] = state->xtrue.fi.ptr.p_double[i]*multiplyby;
            j0 = state->xvirt.sj.ridx.ptr.p_int[i];
            j1 = state->xvirt.sj.ridx.ptr.p_int[i+1]-1;
            for(j=j0; j<=j1; j++)
            {
                state->xvirt.sj.vals.ptr.p_double[j] = state->xvirt.sj.vals.ptr.p_double[j]*multiplyby;
            }
            j0 = state->xtrue.sj.ridx.ptr.p_int[i];
            j1 = state->xtrue.sj.ridx.ptr.p_int[i+1]-1;
            for(j=j0; j<=j1; j++)
            {
                state->xtrue.sj.vals.ptr.p_double[j] = state->xtrue.sj.vals.ptr.p_double[j]*multiplyby;
            }
            if( i==0 )
            {
                
                /*
                 * Rescaling target, rescale all Lagrange multipliers
                 */
                rmulv(2*n, multiplyby, &state->lagmultbc2, _state);
                rmulv(2*(cntlc+cntnlc), multiplyby, &state->lagmultxc2, _state);
                if( dotrace )
                {
                    ae_trace("> target rescaled by %0.2e during renormalization\n",
                        (double)(multiplyby));
                }
            }
            else
            {
                
                /*
                 * Rescaling nonlinear constraint, rescale only quantities related to this constraint
                 */
                if( state->hasnl.ptr.p_bool[i-1] )
                {
                    state->rawnl.ptr.p_double[i-1] = state->rawnl.ptr.p_double[i-1]*multiplyby;
                }
                if( state->hasnu.ptr.p_bool[i-1] )
                {
                    state->rawnu.ptr.p_double[i-1] = state->rawnu.ptr.p_double[i-1]*multiplyby;
                }
                state->lagmultxc2.ptr.p_double[2*(cntlc+(i-1))+0] = state->lagmultxc2.ptr.p_double[2*(cntlc+(i-1))+0]/multiplyby;
                state->lagmultxc2.ptr.p_double[2*(cntlc+(i-1))+1] = state->lagmultxc2.ptr.p_double[2*(cntlc+(i-1))+1]/multiplyby;
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
        if( dodetailedtrace )
        {
            ae_trace("> printing raw data (prior to applying variable and function scales)\n");
            ae_trace("X (raw)       = ");
            tracevectorunscaledunshiftedautoprec(&state->xvirt.x, n, &state->s, ae_true, &state->s, ae_false, _state);
            ae_trace("\n");
            ae_trace("> printing scaled data (after applying variable and function scales)\n");
            ae_trace("X (scaled)    = ");
            tracevectorautoprec(&state->xvirt.x, 0, n, _state);
            ae_trace("\n");
            ae_trace("FScales       = ");
            tracevectorautoprec(&state->fscales, 0, 1+cntnlc, _state);
            ae_trace("\n");
            ae_trace("GammaScl      = ");
            tracevectorautoprec(&state->tracegamma, 0, 1+cntnlc, _state);
            ae_trace("\n");
            ae_trace("Fi (scaled)   = ");
            tracevectorautoprec(&state->xvirt.fi, 0, 1+cntnlc, _state);
            ae_trace("\n");
            ae_trace("lagMultBC     = ");
            tracevectorautoprec(&state->lagmultbc2, 0, 2*n, _state);
            ae_trace("\n");
            ae_trace("lagMultLC     = ");
            tracevectorautoprec(&state->lagmultxc2, 0, 2*cntlc, _state);
            ae_trace("\n");
            ae_trace("lagMultNLC    = ");
            tracevectorautoprec(&state->lagmultxc2, 2*cntlc, 2*cntlc+2*cntnlc, _state);
            ae_trace("\n");
        }
        mx = (double)(0);
        for(i=1; i<=cntnlc; i++)
        {
            mx = ae_maxreal(mx, ae_fabs(state->xvirt.fi.ptr.p_double[i], _state), _state);
        }
        ae_trace("Rho           = %0.3e    (penalty for violation of constraints)\n",
            (double)(state->rho));
        ae_trace("lin.violation = %0.3e    (scaled violation of linear constraints)\n",
            (double)(state->replcerr));
        ae_trace("nlc.violation = %0.3e    (scaled violation of nonlinear constraints)\n",
            (double)(mx));
        ae_trace("gamma0        = %0.3e    (Hessian 2-norm estimate for target)\n",
            (double)(state->tracegamma.ptr.p_double[0]));
        j = 0;
        for(i=0; i<=cntnlc; i++)
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
        ae_trace("|x|inf        = %0.3e\n",
            (double)(rmaxabsv(n, &state->xvirt.x, _state)));
        ae_trace("|LagBC|inf    = %0.3e\n",
            (double)(rmaxabsv(2*n, &state->lagmultbc2, _state)));
        ae_trace("|LagXC|inf    = %0.3e\n",
            (double)(rmaxabsv(2*(cntlc+cntnlc), &state->lagmultxc2, _state)));
    }
    
    /*
     * Save current state
     */
    vfjcopy(&state->xvirt, &state->xvirtprev, _state);
    vfjcopy(&state->xtrue, &state->xtrueprev, _state);
    
    /*
     * Send most recent copy of L-BFGS Hessians to the preconditioner
     */
    nlcaul_preconditionersendupdates(state, &state->preconditioner, &state->xvirt, state->rho, &state->lagmultbc2, &state->lagmultxc2, &state->hesstgt, &state->hessaug, _state);
    
    /*
     * Compute preconditioned step direction
     */
    if( dotrace )
    {
        ae_trace("\n--- LBFGS-preconditioned gradient descent ----------------------------------------------------------\n");
    }
    nlcaul_auglagfg(state, &state->xvirt, state->rho, &state->lagmultbc2, &state->lagmultxc2, &lagcur, &state->laggradcur, _state);
    nlcaul_modtargetfg(state, &state->xvirt, state->rho, state->rho, &modtgt0, &state->modtgtgrad0, _state);
    lag0 = lagcur;
    nlcaul_preconditionersolve(state, &state->preconditioner, &state->laggradcur, &state->d, _state);
    if( dotrace )
    {
        if( dodetailedtrace )
        {
            ae_trace("grad L(X0)    = ");
            tracevectorautoprec(&state->laggradcur, 0, n, _state);
            ae_trace("\n");
            ae_trace("D             = ");
            tracevectorautoprec(&state->d, 0, n, _state);
            ae_trace("\n");
        }
        ae_trace("(D,G)         = %0.2e (directional derivative)\n",
            (double)(rdotv(n, &state->laggradcur, &state->d, _state)));
    }
    
    /*
     * Perform optimization along step directions
     */
    rallocv(n, &state->work, _state);
    state->mcstage = 0;
    state->stp = 1.0;
    rcopyv(n, &state->d, &state->du, _state);
    linminnormalized(&state->d, &state->stp, n, _state);
    stpproposed = state->stp;
    state->longeststp = coalesce(state->longeststp, state->stp, _state);
    state->stplimit = (double)10*state->longeststp;
    rcopyallocv(n, &state->laggradcur, &state->laggrad0, _state);
    lag0 = lagcur;
    mcsrch(n, &state->xvirt.x, &lagcur, &state->laggradcur, &state->d, &state->stp, state->stplimit, nlcaul_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
lbl_8:
    if( state->mcstage==0 )
    {
        goto lbl_9;
    }
    nlcaul_clearrequestfields(state, _state);
    nlcaul_aulsendvirtualpoint(state, &state->xvirt.x, &state->xtrue.x, _state);
    state->needsj = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needsj = ae_false;
    if( !nlcaul_aulretrievesj(state, &state->xvirt, &state->xtrue, _state) )
    {
        
        /*
         * Failed to retrieve function/Jacobian, infinities detected!
         */
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    nlcaul_auglagfg(state, &state->xvirt, state->rho, &state->lagmultbc2, &state->lagmultxc2, &lagcur, &state->laggradcur, _state);
    mcsrch(n, &state->xvirt.x, &lagcur, &state->laggradcur, &state->d, &state->stp, state->stplimit, nlcaul_gtol, &mcinfo, &state->nfev, &state->work, &state->lstate, &state->mcstage, _state);
    goto lbl_8;
lbl_9:
    lag1 = lagcur;
    if( dotrace )
    {
        ae_trace("> line search finished with relative step %0.3f, absolute step %0.3e, %0d evals, MCINFO=%0d\n",
            (double)(state->stp/stpproposed),
            (double)(state->stp*rmaxabsv(n, &state->d, _state)),
            (int)(state->nfev),
            (int)(mcinfo));
        ae_trace("mod Tgt:        %17.9e -> %17.9e (delta=%11.3e)\n",
            (double)(state->xvirtprev.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0]),
            (double)(state->xvirt.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0]),
            (double)(state->xvirt.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0]-state->xvirtprev.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0]));
        ae_trace("raw Lag:        %17.9e -> %17.9e (delta=%11.3e)\n",
            (double)(lag0),
            (double)(lag1),
            (double)(lag1-lag0));
    }
    nlcaul_clearrequestfields(state, _state);
    nlcaul_aulsendtruepoint(state, &state->xtrue.x, _state);
    state->f = state->xtrue.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->xupdated = ae_false;
    state->repnfev = state->repnfev+state->nfev;
    
    /*
     * Update curvature information, statistics, errors
     */
    state->longeststp = ae_maxreal(state->longeststp, state->stp, _state);
    if( mcinfo==1 )
    {
        hessianupdate(&state->hessaug, &state->xvirtprev.x, &state->laggrad0, &state->xvirt.x, &state->laggradcur, dotrace, _state);
    }
    else
    {
        if( dotrace )
        {
            ae_trace("> L-BFGS update for an augmented Lagrangian is rejected (MCINFO<>1)\n");
        }
    }
    nlcaul_modtargetfg(state, &state->xvirt, state->rho, state->rho, &modtgt1, &state->modtgtgrad1, _state);
    hessianupdate(&state->hesstgt, &state->xvirtprev.x, &state->modtgtgrad0, &state->xvirt.x, &state->modtgtgrad1, dotrace, _state);
    checklc2violation(&state->sparsea, &state->al, &state->au, &state->lcsrcidx, cntlc, &state->xtrue.x, &state->replcerr, &state->replcidx, _state);
    unscaleandchecknlc2violation(&state->xtrue.fi, &state->fscales, &state->rawnl, &state->rawnu, cntnlc, &state->repnlcerr, &state->repnlcidx, _state);
    nlcaul_computeerrors(state, &state->xvirt, &errprim, &errdual, &errcmpl, _state);
    
    /*
     * Perform agressive probing of the search direction - additional function evaluations
     * which help us to determine possible discontinuity and nonsmoothness of the problem
     */
    if( !doprobingalways )
    {
        goto lbl_10;
    }
    vfjcopy(&state->xvirtprev, &state->xvirtprobe, _state);
    vfjcopy(&state->xtrueprev, &state->xtrueprobe, _state);
    steplengthtoprobe = ae_maxreal((double)5*(state->stp/stpproposed), 0.01, _state);
    smoothnessmonitorstartlagrangianprobing(smonitor, &state->xvirtprobe.x, &state->du, steplengthtoprobe, state->repiterationscount, state->outeriterationscount, _state);
lbl_12:
    if( !smoothnessmonitorprobelagrangian(smonitor, _state) )
    {
        goto lbl_13;
    }
    rcopyv(n, &smonitor->lagprobx, &state->xvirtprobe.x, _state);
    nlcaul_clearrequestfields(state, _state);
    nlcaul_aulsendvirtualpoint(state, &state->xvirtprobe.x, &state->xtrueprobe.x, _state);
    state->needsj = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needsj = ae_false;
    if( !nlcaul_aulretrievesj(state, &state->xvirtprobe, &state->xtrueprobe, _state) )
    {
        
        /*
         * Failed to retrieve function/Jacobian, infinities detected!
         */
        state->repterminationtype = -8;
        result = ae_false;
        return result;
    }
    rcopyallocv(1+cntnlc, &state->xvirtprobe.fi, &smonitor->lagprobfi, _state);
    rsetallocm(1+cntnlc, n, 0.0, &smonitor->lagprobj, _state);
    for(i=0; i<=cntnlc; i++)
    {
        j0 = state->xvirtprobe.sj.ridx.ptr.p_int[i];
        j1 = state->xvirtprobe.sj.ridx.ptr.p_int[i+1]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            smonitor->lagprobj.ptr.pp_double[i][state->xvirtprobe.sj.idx.ptr.p_int[jj]] = state->xvirtprobe.sj.vals.ptr.p_double[jj];
        }
    }
    rallocv(n, &state->tmpg, _state);
    nlcaul_auglagfg(state, &state->xvirtprobe, state->rho, &state->lagmultbc2, &state->lagmultxc2, &smonitor->lagprobrawlag, &state->tmpg, _state);
    goto lbl_12;
lbl_13:
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |                 probing search direction                 |\n");
    ae_trace("*** ------------------------------------------------------------\n");
    ae_trace("*** |  Step  | Lagrangian (unaugmentd)|   Projected  target    |\n");
    ae_trace("*** |along  D|     must be smooth     |     must be smooth     |\n");
    ae_trace("*** |        | function   |    slope  | function   |    slope  |\n");
    smoothnessmonitortracelagrangianprobingresults(smonitor, _state);
lbl_10:
    
    /*
     * Trace
     */
    if( dotrace )
    {
    }
    
    /*
     * Trace
     */
    if( dotrace )
    {
        ae_trace("\n--- inner iteration ends ---------------------------------------------------------------------------\n");
        ae_trace("> printing primal/dual/complementary errors:\n");
        ae_trace("prim.feas     = %0.3e    (primal feasibility error, inf-norm)\n",
            (double)(errprim));
        ae_trace("dual.feas     = %0.3e    (dual feasibility error, inf-norm)\n",
            (double)(errdual));
        ae_trace("cmpl.slckness = %0.3e    (complementary slackness, inf-norm)\n",
            (double)(errcmpl));
    }
    
    /*
     * Update iterations counts.
     * Check stopping criteria for inner iterations.
     */
    state->repiterationscount = state->repiterationscount+1;
    state->inneriterationscount = state->inneriterationscount+1;
    inneriterationsstopped = ae_false;
    state->fstagnationcnt = icase2(ae_fp_less_eq(ae_fabs(lag1-lag0, _state),nlcaul_stagnationepsf*rmax3(ae_fabs(lag1, _state), ae_fabs(lag0, _state), (double)(1), _state)), state->fstagnationcnt+1, 0, _state);
    v = ae_fabs(lag1-lag0, _state)/rmaxabs3(lag1, lag0, (double)(1), _state);
    vv = ae_fabs(modtgt1-modtgt0, _state)/rmaxabs3(modtgt1, modtgt0, (double)(1), _state);
    if( ae_fp_less_eq(ae_maxreal(v, vv, _state),critgetepsf(&state->criteria, _state)) )
    {
        inneriterationsstopped = ae_true;
        if( dotrace )
        {
            ae_trace("> stopping condition for inner iterations met: function change is smaller than %0.3e\n",
                (double)(critgetepsf(&state->criteria, _state)));
        }
    }
    if( ae_fp_less_eq(rmaxabsv(n, &state->d, _state)*state->stp,critgetepsx(&state->criteria, _state)) )
    {
        inneriterationsstopped = ae_true;
        if( dotrace )
        {
            ae_trace("> stopping condition for inner iterations met: step is smaller than %0.3e\n",
                (double)(critgetepsx(&state->criteria, _state)));
        }
    }
    if( ae_fp_less_eq(rmaxabsv(n, &state->d, _state)*state->stp,0.001*ae_sqrt(ae_machineepsilon, _state)) )
    {
        inneriterationsstopped = ae_true;
        if( dotrace )
        {
            ae_trace("> stopping condition for inner iterations met: step is smaller than the absolute minimum\n");
        }
    }
    if( critgetmaxits(&state->criteria, _state)>0&&state->inneriterationscount>=critgetmaxits(&state->criteria, _state) )
    {
        inneriterationsstopped = ae_true;
        if( dotrace )
        {
            ae_trace("> stopping condition for inner iterations met: %0d iterations performed\n",
                (int)(state->inneriterationscount));
        }
    }
    if( state->fstagnationcnt>=nlcaul_fstagnationlimit )
    {
        if( dotrace )
        {
            ae_trace("> stopping criteria are too stringent: F stagnated for %0d its, stopping\n",
                (int)(state->fstagnationcnt));
        }
        inneriterationsstopped = ae_true;
    }
    
    /*
     * Inner iterations stopped (or user requested termination)
     *
     * Update Lagrange multipliers, penalties, check stopping criteria for outer iterations.
     */
    if( inneriterationsstopped||userterminationneeded )
    {
        
        /*
         * Update best point so far
         */
        if( ae_fp_less(rmax3(errprim, errdual, errcmpl, _state),state->besterr) )
        {
            if( ae_fp_greater(rmax3(errprim, errdual, errcmpl, _state),nlcaul_besterrstagnationcoeff*state->besterr) )
            {
                state->besterrnegligibleupdates = state->besterrnegligibleupdates+1;
            }
            vfjcopy(&state->xvirt, &state->xvirtbest, _state);
            vfjcopy(&state->xtrue, &state->xtruebest, _state);
            state->besterr = rmax3(errprim, errdual, errcmpl, _state);
            state->bestiteridx = state->outeriterationscount;
        }
        
        /*
         * Advance outer iterations counter, reset inner iterations count, check stopping criteria
         */
        state->outeriterationscount = state->outeriterationscount+1;
        state->inneriterationscount = 0;
        state->fstagnationcnt = 0;
        if( (ae_fp_less_eq(errprim,(double)10*ae_sqrt(ae_machineepsilon, _state))&&ae_fp_less_eq(errdual,(double)10*ae_sqrt(ae_machineepsilon, _state)))&&ae_fp_less_eq(errcmpl,(double)10*ae_sqrt(ae_machineepsilon, _state)) )
        {
            if( dotrace )
            {
                ae_trace("> primal, dual and complementary slackness errors are small, stopping\n");
            }
            state->repterminationtype = 1;
            goto lbl_7;
        }
        if( state->besterrnegligibleupdates>=nlcaul_besterrstagnationlimit )
        {
            if( dotrace )
            {
                ae_trace("> primal/dual/complementary error had too many negligible updates, stopping\n");
            }
            state->repterminationtype = 7;
            goto lbl_7;
        }
        if( state->bestiteridx<state->outeriterationscount-nlcaul_besterrstagnationlimit )
        {
            if( dotrace )
            {
                ae_trace("> primal/dual/complementary error stagnated for too long, stopping\n");
            }
            state->repterminationtype = 7;
            goto lbl_7;
        }
        if( userterminationneeded )
        {
            if( dotrace )
            {
                ae_trace("> user requested termination, stopping\n");
            }
            state->repterminationtype = 8;
            goto lbl_7;
        }
        if( state->outeriterationscount>=state->maxouterits )
        {
            state->repterminationtype = 5;
            goto lbl_7;
        }
        
        /*
         * Save primal/dual/complementary error estimates
         */
        preverrprim = errprim;
        preverrdual = errdual;
        preverrcmpl = errcmpl;
        
        /*
         * Update Lagrange multipliers
         */
        for(i=0; i<=n-1; i++)
        {
            ae_assert(ae_fp_greater(nlcaul_aulmaxgrowth,1.0), "AUL: integrity check 2444 failed", _state);
            if( !state->hasbndl.ptr.p_bool[i]&&!state->hasbndu.ptr.p_bool[i] )
            {
                continue;
            }
            if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->scaledbndl.ptr.p_double[i],state->scaledbndu.ptr.p_double[i]) )
            {
                v = ae_minreal(nlcaul_maxlagmult, nlcaul_aulmaxgrowth*(ae_fabs(state->lagmultbc2.ptr.p_double[2*i+0], _state)+(double)1), _state);
                state->lagmultbc2.ptr.p_double[2*i+0] = boundval(state->lagmultbc2.ptr.p_double[2*i+0]+state->rho*(state->xvirt.x.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i]), -v, v, _state);
                continue;
            }
            if( state->hasbndl.ptr.p_bool[i] )
            {
                inequalityshiftedbarrierfunction((state->xvirt.x.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i])*state->rho+(double)1, &p, &dp, &d2p, _state);
                dp = ae_fabs(dp, _state);
                dp = ae_minreal(dp, nlcaul_aulmaxgrowth, _state);
                dp = ae_maxreal(dp, (double)1/nlcaul_aulmaxgrowth, _state);
                state->lagmultbc2.ptr.p_double[2*i+0] = boundval(state->lagmultbc2.ptr.p_double[2*i+0]*dp, nlcaul_minlagmult, nlcaul_maxlagmult, _state);
            }
            if( state->hasbndu.ptr.p_bool[i] )
            {
                inequalityshiftedbarrierfunction((state->scaledbndu.ptr.p_double[i]-state->xvirt.x.ptr.p_double[i])*state->rho+(double)1, &p, &dp, &d2p, _state);
                dp = ae_fabs(dp, _state);
                dp = ae_minreal(dp, nlcaul_aulmaxgrowth, _state);
                dp = ae_maxreal(dp, (double)1/nlcaul_aulmaxgrowth, _state);
                state->lagmultbc2.ptr.p_double[2*i+1] = boundval(state->lagmultbc2.ptr.p_double[2*i+1]*dp, nlcaul_minlagmult, nlcaul_maxlagmult, _state);
            }
        }
        for(i=0; i<=cntlc-1; i++)
        {
            j0 = state->sparsea.ridx.ptr.p_int[i];
            j1 = state->sparsea.ridx.ptr.p_int[i+1]-1;
            ci = (double)(0);
            for(jj=j0; jj<=j1; jj++)
            {
                ci = ci+state->xvirt.x.ptr.p_double[state->sparsea.idx.ptr.p_int[jj]]*state->sparsea.vals.ptr.p_double[jj];
            }
            if( (state->hasal.ptr.p_bool[i]&&state->hasau.ptr.p_bool[i])&&ae_fp_eq(state->al.ptr.p_double[i],state->au.ptr.p_double[i]) )
            {
                
                /*
                 * Equality constraint
                 */
                state->lagmultxc2.ptr.p_double[2*i+0] = state->lagmultxc2.ptr.p_double[2*i+0]+state->rho*(ci-state->al.ptr.p_double[i]);
            }
            else
            {
                
                /*
                 * No constraints, inequality constraint or range constraint
                 */
                if( state->hasal.ptr.p_bool[i] )
                {
                    inequalityshiftedbarrierfunction((ci-state->al.ptr.p_double[i])*state->rho+(double)1, &p, &dp, &d2p, _state);
                    dp = ae_fabs(dp, _state);
                    dp = ae_minreal(dp, nlcaul_aulmaxgrowth, _state);
                    dp = ae_maxreal(dp, (double)1/nlcaul_aulmaxgrowth, _state);
                    state->lagmultxc2.ptr.p_double[2*i+0] = boundval(state->lagmultxc2.ptr.p_double[2*i+0]*dp, nlcaul_minlagmult, nlcaul_maxlagmult, _state);
                }
                if( state->hasau.ptr.p_bool[i] )
                {
                    inequalityshiftedbarrierfunction((state->au.ptr.p_double[i]-ci)*state->rho+(double)1, &p, &dp, &d2p, _state);
                    dp = ae_fabs(dp, _state);
                    dp = ae_minreal(dp, nlcaul_aulmaxgrowth, _state);
                    dp = ae_maxreal(dp, (double)1/nlcaul_aulmaxgrowth, _state);
                    state->lagmultxc2.ptr.p_double[2*i+1] = boundval(state->lagmultxc2.ptr.p_double[2*i+1]*dp, nlcaul_minlagmult, nlcaul_maxlagmult, _state);
                }
            }
        }
        for(i=0; i<=cntnlc-1; i++)
        {
            ci = state->xvirt.fi.ptr.p_double[1+i];
            if( (state->hasnl.ptr.p_bool[i]&&state->hasnu.ptr.p_bool[i])&&ae_fp_eq(state->rawnl.ptr.p_double[i],state->rawnu.ptr.p_double[i]) )
            {
                
                /*
                 * Equality constraint
                 */
                state->lagmultxc2.ptr.p_double[2*cntlc+2*i+0] = state->lagmultxc2.ptr.p_double[2*cntlc+2*i+0]+state->rho*(ci-state->rawnl.ptr.p_double[i]);
            }
            else
            {
                
                /*
                 * No constraints, inequality constraint or range constraint
                 */
                if( state->hasnl.ptr.p_bool[i] )
                {
                    inequalityshiftedbarrierfunction((ci-state->rawnl.ptr.p_double[i])*state->rho+(double)1, &p, &dp, &d2p, _state);
                    dp = ae_fabs(dp, _state);
                    dp = ae_minreal(dp, nlcaul_aulmaxgrowth, _state);
                    dp = ae_maxreal(dp, (double)1/nlcaul_aulmaxgrowth, _state);
                    state->lagmultxc2.ptr.p_double[2*cntlc+2*i+0] = boundval(state->lagmultxc2.ptr.p_double[2*cntlc+2*i+0]*dp, nlcaul_minlagmult, nlcaul_maxlagmult, _state);
                }
                if( state->hasnu.ptr.p_bool[i] )
                {
                    inequalityshiftedbarrierfunction((state->rawnu.ptr.p_double[i]-ci)*state->rho+(double)1, &p, &dp, &d2p, _state);
                    dp = ae_fabs(dp, _state);
                    dp = ae_minreal(dp, nlcaul_aulmaxgrowth, _state);
                    dp = ae_maxreal(dp, (double)1/nlcaul_aulmaxgrowth, _state);
                    state->lagmultxc2.ptr.p_double[2*cntlc+2*i+1] = boundval(state->lagmultxc2.ptr.p_double[2*cntlc+2*i+1]*dp, nlcaul_minlagmult, nlcaul_maxlagmult, _state);
                }
            }
        }
        
        /*
         * Update upper bound on Hessian norm
         */
        hessiansetmaxhess(&state->hessaug, nlcaul_xbfgsmaxhess*((double)1+state->rho)*ae_maxreal((double)1+rmaxabsv(2*n, &state->lagmultbc2, _state), (double)1+rmaxabsv(2*cntlc+2*cntnlc, &state->lagmultxc2, _state), _state), _state);
        state->rho = ae_minreal(ae_sqrt((double)(2), _state)*state->rho, nlcaul_rhomax, _state);
    }
    goto lbl_6;
lbl_7:
    
    /*
     * Perform the final report
     */
    nlcaul_clearrequestfields(state, _state);
    nlcaul_aulsendtruepoint(state, &state->xtruebest.x, _state);
    state->f = state->xtruebest.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0];
    state->xupdated = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->xupdated = ae_false;
    
    /*
     * Almost done
     */
    if( dotrace )
    {
        ae_trace("\n=== STOPPED ========================================================================================\n");
        ae_trace("raw target:     %20.12e\n",
            (double)(state->xtruebest.fi.ptr.p_double[0]*state->fscales.ptr.p_double[0]));
    }
    smoothnessmonitortracestatus(smonitor, dotrace, _state);
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
    state->rstate.ia.ptr.p_int[3] = nconstr;
    state->rstate.ia.ptr.p_int[4] = i;
    state->rstate.ia.ptr.p_int[5] = j;
    state->rstate.ia.ptr.p_int[6] = j0;
    state->rstate.ia.ptr.p_int[7] = j1;
    state->rstate.ia.ptr.p_int[8] = jj;
    state->rstate.ia.ptr.p_int[9] = mcinfo;
    state->rstate.ba.ptr.p_bool[0] = dotrace;
    state->rstate.ba.ptr.p_bool[1] = doprobingalways;
    state->rstate.ba.ptr.p_bool[2] = dodetailedtrace;
    state->rstate.ba.ptr.p_bool[3] = inneriterationsstopped;
    state->rstate.ba.ptr.p_bool[4] = stepaccepted;
    state->rstate.ra.ptr.p_double[0] = lagcur;
    state->rstate.ra.ptr.p_double[1] = lag0;
    state->rstate.ra.ptr.p_double[2] = lag1;
    state->rstate.ra.ptr.p_double[3] = laga;
    state->rstate.ra.ptr.p_double[4] = lagb;
    state->rstate.ra.ptr.p_double[5] = ci;
    state->rstate.ra.ptr.p_double[6] = modtgt0;
    state->rstate.ra.ptr.p_double[7] = modtgt1;
    state->rstate.ra.ptr.p_double[8] = mx;
    state->rstate.ra.ptr.p_double[9] = setscaleto;
    state->rstate.ra.ptr.p_double[10] = multiplyby;
    state->rstate.ra.ptr.p_double[11] = stpproposed;
    state->rstate.ra.ptr.p_double[12] = p;
    state->rstate.ra.ptr.p_double[13] = dp;
    state->rstate.ra.ptr.p_double[14] = d2p;
    state->rstate.ra.ptr.p_double[15] = steplengthtoprobe;
    state->rstate.ra.ptr.p_double[16] = v;
    state->rstate.ra.ptr.p_double[17] = vv;
    state->rstate.ra.ptr.p_double[18] = errprim;
    state->rstate.ra.ptr.p_double[19] = errdual;
    state->rstate.ra.ptr.p_double[20] = errcmpl;
    state->rstate.ra.ptr.p_double[21] = preverrprim;
    state->rstate.ra.ptr.p_double[22] = preverrdual;
    state->rstate.ra.ptr.p_double[23] = preverrcmpl;
    return result;
}


/*************************************************************************
Shifted barrier function for inequality constraints  which  is  multiplied
by the corresponding Lagrange multiplier.

Shift function must enter augmented Lagrangian as

    LagMult/Rho*SHIFT((x-lowerbound)*Rho+1)
    
with corresponding changes being made for upper bound or  other  kinds  of
constraints.

INPUT PARAMETERS:
    Alpha   -   function argument. Typically, if we have active constraint
                with precise Lagrange multiplier, we have Alpha  around 1.
                Large positive Alpha's correspond to  inner  area  of  the
                feasible set. Alpha<1 corresponds to  outer  area  of  the
                feasible set.
                
OUTPUT PARAMETERS:
    F       -   F(Alpha)
    DF      -   DF=dF(Alpha)/dAlpha, exact derivative
    D2F     -   second derivative

  -- ALGLIB --
     Copyright 11.07.2023 by Bochkanov Sergey
*************************************************************************/
void inequalityshiftedbarrierfunction(double alpha,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state)
{

    *f = 0.0;
    *df = 0.0;
    *d2f = 0.0;

    if( alpha>=0.5 )
    {
        *f = -ae_log(alpha, _state);
        *df = -(double)1/alpha;
        *d2f = *df*(*df);
    }
    else
    {
        *f = (double)2*alpha*alpha-(double)4*alpha+(ae_log((double)(2), _state)+1.5);
        *df = (double)4*alpha-(double)4;
        *d2f = (double)(4);
    }
}


/*************************************************************************
This function prepares preconditioner for subsequent computation.

INPUT PARAMETERS:
    SState          -   solver state
    RefreshFreq     -   >=1, refresh frequency

OUTPUT PARAMETERS:
    P               -   preconditioner object, ready to be computed
                        

  -- ALGLIB --
     Copyright 13.06.2023 by Bochkanov Sergey
*************************************************************************/
static void nlcaul_preconditionerinit(const minaulstate* sstate,
     ae_int_t refreshfreq,
     minaulpreconditioner* p,
     ae_state *_state)
{


    p->refreshfreq = refreshfreq;
    p->modelage = refreshfreq;
}


/*************************************************************************
This function sends updated target and augmented Lagrangian Hessians to the
preconditioner. It is expected that this call corresponds to one update of
Hessians.

  -- ALGLIB --
     Copyright 13.06.2023 by Bochkanov Sergey
*************************************************************************/
static void nlcaul_preconditionersendupdates(minaulstate* state,
     minaulpreconditioner* prec,
     const varsfuncjac* current,
     double rho,
     /* Real    */ const ae_vector* lagmultbc2,
     /* Real    */ const ae_vector* lagmultxc2,
     xbfgshessian* hesstgt,
     xbfgshessian* hessaug,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t cntlc;
    double sigmatgt;
    double dummy;
    ae_int_t dummyupdcnt;
    double dampeps;
    ae_int_t nconstr;
    ae_int_t naug;
    ae_int_t offs;
    ae_int_t tgtrowidx;
    double vd;
    double p;
    double dp;
    double d2p;
    double vcurvature;
    double vsqrtcurvature;
    ae_int_t i;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    double ci;
    double fi;


    n = state->n;
    cntlc = state->cntlc;
    nconstr = state->cntlc+state->cntnlc;
    dampeps = (double)100*ae_sqrt(ae_machineepsilon, _state)*((double)1+rho);
    
    /*
     * Extract LBFGS memory:
     * * we extract only diagonal scaling factor Sigma from the mod-target Hessian, ignoring all updates
     * * we extract only S/Y updates from the augmented Lagrangian Hessian
     */
    hessiangetlowrankmemory(hesstgt, &sigmatgt, &prec->dummys, &prec->dummyy, &dummyupdcnt, _state);
    hessiangetlowrankmemory(hessaug, &dummy, &prec->s, &prec->y, &prec->updcnt, _state);
    ae_assert(ae_fp_greater(sigmatgt,(double)(0)), "AUL: sigma0<=0", _state);
    
    /*
     * Increase model age, skip factorization update if model age is small enough
     */
    prec->modelage = prec->modelage+1;
    if( prec->modelage<prec->refreshfreq )
    {
        return;
    }
    
    /*
     * The model needs refactorization of the initial scaling matrix
     */
    prec->modelage = 0;
    
    /*
     * Generate initial scaling matrix which is a diagonal scaling Sigma from the mod-target Hessian +
     * correction terms coming from penalized constraints.
     */
    naug = n+nconstr;
    prec->naug = naug;
    prec->augsys.matrixtype = 1;
    prec->augsys.m = naug;
    prec->augsys.n = naug;
    iallocv(naug+1, &prec->augsys.ridx, _state);
    prec->augsys.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=n-1; i++)
    {
        vd = 0.0;
        if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->scaledbndl.ptr.p_double[i],state->scaledbndu.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint, quadratic penalty
             */
            vd = rho;
        }
        else
        {
            if( state->hasbndl.ptr.p_bool[i] )
            {
                inequalityshiftedbarrierfunction((current->x.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
                vd = vd+lagmultbc2->ptr.p_double[2*i+0]*d2p*rho;
            }
            if( state->hasbndu.ptr.p_bool[i] )
            {
                inequalityshiftedbarrierfunction((state->scaledbndu.ptr.p_double[i]-current->x.ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
                vd = vd+lagmultbc2->ptr.p_double[2*i+1]*d2p*rho;
            }
        }
        
        /*
         * Write the diagonal Hessian + diagonal correction coming from the box constraints
         */
        offs = prec->augsys.ridx.ptr.p_int[i];
        igrowv(offs+naug, &prec->augsys.idx, _state);
        rgrowv(offs+naug, &prec->augsys.vals, _state);
        prec->augsys.idx.ptr.p_int[offs] = i;
        prec->augsys.vals.ptr.p_double[offs] = sigmatgt+vd+dampeps;
        offs = offs+1;
        prec->augsys.ridx.ptr.p_int[i+1] = offs;
    }
    for(i=0; i<=state->cntlc-1; i++)
    {
        
        /*
         * Compute curvature coming from I-th linear constraint
         */
        j0 = state->sparsea.ridx.ptr.p_int[i];
        j1 = state->sparsea.ridx.ptr.p_int[i+1]-1;
        ci = (double)(0);
        for(jj=j0; jj<=j1; jj++)
        {
            ci = ci+current->x.ptr.p_double[state->sparsea.idx.ptr.p_int[jj]]*state->sparsea.vals.ptr.p_double[jj];
        }
        vcurvature = 0.0;
        if( (state->hasal.ptr.p_bool[i]&&state->hasau.ptr.p_bool[i])&&ae_fp_eq(state->al.ptr.p_double[i],state->au.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint Ai*x = ALi = AUi
             */
            vcurvature = rho;
        }
        else
        {
            
            /*
             * Inequality/range constraint
             */
            if( state->hasal.ptr.p_bool[i] )
            {
                
                /*
                 * Add curvature from inequality constraint Ai*x >= ALi
                 */
                inequalityshiftedbarrierfunction((ci-state->al.ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
                vcurvature = vcurvature+lagmultxc2->ptr.p_double[2*i+0]*d2p*rho;
            }
            if( state->hasau.ptr.p_bool[i] )
            {
                
                /*
                 * Add curvature from inequality constraint Ai*x <= AUi
                 */
                inequalityshiftedbarrierfunction((state->au.ptr.p_double[i]-ci)*rho+(double)1, &p, &dp, &d2p, _state);
                vcurvature = vcurvature+lagmultxc2->ptr.p_double[2*i+1]*d2p*rho;
            }
        }
        
        /*
         * Add modification to the Hessian
         */
        vsqrtcurvature = ae_sqrt(ae_maxreal(vcurvature, 0.0, _state), _state);
        tgtrowidx = n+i;
        offs = prec->augsys.ridx.ptr.p_int[tgtrowidx];
        igrowv(offs+naug, &prec->augsys.idx, _state);
        rgrowv(offs+naug, &prec->augsys.vals, _state);
        j0 = state->sparsea.ridx.ptr.p_int[i];
        j1 = state->sparsea.ridx.ptr.p_int[i+1]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            prec->augsys.idx.ptr.p_int[offs] = state->sparsea.idx.ptr.p_int[jj];
            prec->augsys.vals.ptr.p_double[offs] = vsqrtcurvature*state->sparsea.vals.ptr.p_double[jj];
            offs = offs+1;
        }
        prec->augsys.idx.ptr.p_int[offs] = tgtrowidx;
        prec->augsys.vals.ptr.p_double[offs] = -1.0;
        offs = offs+1;
        prec->augsys.ridx.ptr.p_int[tgtrowidx+1] = offs;
    }
    for(i=0; i<=state->cntnlc-1; i++)
    {
        
        /*
         * Compute curvature coming from I-th nonlinear constraint
         */
        fi = current->fi.ptr.p_double[1+i];
        vcurvature = 0.0;
        if( (state->hasnl.ptr.p_bool[i]&&state->hasnu.ptr.p_bool[i])&&ae_fp_eq(state->rawnl.ptr.p_double[i],state->rawnu.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint Ai*x = ALi = AUi
             */
            vcurvature = rho;
        }
        else
        {
            
            /*
             * Inequality/range constraint
             */
            if( state->hasnl.ptr.p_bool[i] )
            {
                
                /*
                 * Add curvature from inequality constraint Ai*x >= ALi
                 */
                inequalityshiftedbarrierfunction((fi-state->rawnl.ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
                vcurvature = vcurvature+lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*d2p*rho;
            }
            if( state->hasnu.ptr.p_bool[i] )
            {
                
                /*
                 * Add curvature from inequality constraint Ai*x <= AUi
                 */
                inequalityshiftedbarrierfunction((state->rawnu.ptr.p_double[i]-fi)*rho+(double)1, &p, &dp, &d2p, _state);
                vcurvature = vcurvature+lagmultxc2->ptr.p_double[2*cntlc+2*i+1]*d2p*rho;
            }
        }
        
        /*
         * Add modification
         */
        vsqrtcurvature = ae_sqrt(ae_maxreal(vcurvature, 0.0, _state), _state);
        tgtrowidx = n+cntlc+i;
        offs = prec->augsys.ridx.ptr.p_int[tgtrowidx];
        igrowv(offs+naug, &prec->augsys.idx, _state);
        rgrowv(offs+naug, &prec->augsys.vals, _state);
        j0 = current->sj.ridx.ptr.p_int[1+i];
        j1 = current->sj.ridx.ptr.p_int[1+i+1]-1;
        for(jj=j0; jj<=j1; jj++)
        {
            prec->augsys.idx.ptr.p_int[offs] = current->sj.idx.ptr.p_int[jj];
            prec->augsys.vals.ptr.p_double[offs] = vsqrtcurvature*current->sj.vals.ptr.p_double[jj];
            offs = offs+1;
        }
        prec->augsys.idx.ptr.p_int[offs] = tgtrowidx;
        prec->augsys.vals.ptr.p_double[offs] = -1.0;
        offs = offs+1;
        prec->augsys.ridx.ptr.p_int[tgtrowidx+1] = offs;
    }
    sparsecreatecrsinplace(&prec->augsys, _state);
    
    /*
     * Perform analysis and factorization
     */
    isetallocv(naug, 0, &prec->priorities, _state);
    if( !spsymmanalyze(&prec->augsys, &prec->priorities, 0.0, 0, 1, 3, 1, &prec->analysis, _state) )
    {
        ae_assert(ae_false, "AUL: integrity check 8126 failed", _state);
    }
    if( !spsymmfactorize(&prec->analysis, _state) )
    {
        ae_assert(ae_false, "AUL: integrity check 8127 failed", _state);
    }
}


/*************************************************************************
This function computes gradient and applies preconditioner.

  -- ALGLIB --
     Copyright 13.06.2023 by Bochkanov Sergey
*************************************************************************/
static void nlcaul_preconditionersolve(minaulstate* state,
     minaulpreconditioner* prec,
     /* Real    */ const ae_vector* g,
     /* Real    */ ae_vector* d,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t naug;
    ae_int_t k;
    double betak;


    ae_assert(prec->modelage<prec->refreshfreq, "AUL: preconditioner needs at least one update in order to properly initialize itself", _state);
    n = state->n;
    naug = prec->naug;
    
    /*
     *
     */
    rallocv(prec->updcnt, &prec->alphak, _state);
    rallocv(prec->updcnt, &prec->rhok, _state);
    rcopymulv(n, -1.0, g, d, _state);
    for(k=prec->updcnt-1; k>=0; k--)
    {
        prec->rhok.ptr.p_double[k] = (double)1/rdotrr(n, &prec->s, k, &prec->y, k, _state);
        prec->alphak.ptr.p_double[k] = prec->rhok.ptr.p_double[k]*rdotvr(n, d, &prec->s, k, _state);
        raddrv(n, -prec->alphak.ptr.p_double[k], &prec->y, k, d, _state);
    }
    rsetallocv(naug, 0.0, &prec->dx, _state);
    rcopyv(n, d, &prec->dx, _state);
    spsymmsolve(&prec->analysis, &prec->dx, _state);
    rcopyv(n, &prec->dx, d, _state);
    for(k=0; k<=prec->updcnt-1; k++)
    {
        betak = prec->rhok.ptr.p_double[k]*rdotvr(n, d, &prec->y, k, _state);
        raddrv(n, prec->alphak.ptr.p_double[k]-betak, &prec->s, k, d, _state);
    }
}


/*************************************************************************
Sends virtual point to the user, enforcing box constraints on it and saving
box-constrained copy to XTrue
*************************************************************************/
static void nlcaul_aulsendvirtualpoint(minaulstate* state,
     /* Real    */ const ae_vector* xvirt,
     /* Real    */ ae_vector* xtrue,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    ae_assert(xvirt->cnt>=n, "AUL: integrity check 6339 failed", _state);
    ae_assert(xtrue->cnt>=n, "AUL: integrity check 6440 failed", _state);
    rcopyv(n, xvirt, &state->x, _state);
    rmergemaxv(n, &state->finitebndl, &state->x, _state);
    rmergeminv(n, &state->finitebndu, &state->x, _state);
    rcopyv(n, &state->x, xtrue, _state);
}


/*************************************************************************
Sends true point (one which is already box-constrained) to the user.
*************************************************************************/
static void nlcaul_aulsendtruepoint(minaulstate* state,
     /* Real    */ const ae_vector* xtrue,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;


    n = state->n;
    ae_assert(xtrue->cnt>=n, "AUL: integrity check 0044 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i]&&xtrue->ptr.p_double[i]<state->scaledbndl.ptr.p_double[i] )
        {
            ae_assert(ae_false, "AUL: box constrained point falls outside of the box constrained area", _state);
        }
        if( state->hasbndu.ptr.p_bool[i]&&xtrue->ptr.p_double[i]>state->scaledbndu.ptr.p_double[i] )
        {
            ae_assert(ae_false, "AUL: box constrained point falls outside of the box constrained area", _state);
        }
        state->x.ptr.p_double[i] = xtrue->ptr.p_double[i];
    }
}


/*************************************************************************
Retrieves raw functions vector and scaled Jacobian from the user data,
fills two structures, XVirt and XTrue:

* it is assumed that XVirt.X and XTrue.X are  already  initialized  -  the
  former is initialized with the virtual point (one which is  not  subject
  to box constraints) and the latter is initialized with  the  true  point
  (one which is box constrained and passed to the user)

* XTrue.Fi and XTrue.SJ are initialized with the true values at XTrue.X

* XVirt.Fi and XVirt.Sj are initialized with the  values  of  the  virtual
  model which coincides with the true function vector at  box  constrained
  points, but linearly extrapolates box constrained projection when  XVirt
  is not box-constrained.

Returns True on success, False on failure (when F or J are not finite numbers).
*************************************************************************/
static ae_bool nlcaul_aulretrievesj(minaulstate* state,
     varsfuncjac* xvirt,
     varsfuncjac* xtrue,
     ae_state *_state)
{
    ae_int_t m;
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j0;
    ae_int_t j1;
    double v;
    double vv;
    ae_int_t offs;
    ae_bool allsame;
    ae_bool result;


    ae_assert(!xvirt->isdense, "AULRetrieveSJ: dense output was specified", _state);
    ae_assert(!xtrue->isdense, "AULRetrieveSJ: dense output was specified", _state);
    ae_assert(state->sj.n==state->n, "AULRetrieveSJ: integrity check 2200 failed", _state);
    ae_assert(state->sj.m==1+state->cntnlc, "AULRetrieveSJ: integrity check 2300 failed", _state);
    ae_assert(xvirt->n==state->n, "AULRetrieveSJ: integrity check 2359 failed", _state);
    ae_assert(xvirt->m==1+state->cntnlc, "AULRetrieveSJ: integrity check 1125 failed", _state);
    ae_assert(xtrue->n==state->n, "AULRetrieveSJ: integrity check 1126 failed", _state);
    ae_assert(xtrue->m==1+state->cntnlc, "AULRetrieveSJ: integrity check 1127 failed", _state);
    n = xtrue->n;
    m = xtrue->m;
    
    /*
     * Fill true point with the raw data
     */
    v = (double)(0);
    xtrue->sj.matrixtype = -10080;
    xtrue->sj.m = m;
    xtrue->sj.n = n;
    iallocv(m+1, &xtrue->sj.ridx, _state);
    xtrue->sj.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=state->cntnlc; i++)
    {
        
        /*
         * Function value
         */
        vv = (double)1/state->fscales.ptr.p_double[i];
        xtrue->fi.ptr.p_double[i] = vv*state->fi.ptr.p_double[i];
        v = v+xtrue->fi.ptr.p_double[i];
        
        /*
         * Sparse Jacobian row (slacks are not added)
         */
        j0 = state->sj.ridx.ptr.p_int[i];
        j1 = state->sj.ridx.ptr.p_int[i+1]-1;
        offs = xtrue->sj.ridx.ptr.p_int[i];
        igrowv(offs+(j1-j0+1), &xtrue->sj.idx, _state);
        rgrowv(offs+(j1-j0+1), &xtrue->sj.vals, _state);
        for(j=j0; j<=j1; j++)
        {
            xtrue->sj.idx.ptr.p_int[offs] = state->sj.idx.ptr.p_int[j];
            xtrue->sj.vals.ptr.p_double[offs] = vv*state->sj.vals.ptr.p_double[j];
            v = v+xtrue->sj.vals.ptr.p_double[offs];
            offs = offs+1;
        }
        
        /*
         * Done
         */
        xtrue->sj.ridx.ptr.p_int[i+1] = offs;
    }
    result = ae_isfinite(v, _state);
    
    /*
     * Compute function values at the virtual point
     */
    rsetallocv(n, 0.0, &state->tmpretrdelta, _state);
    allsame = ae_true;
    for(i=0; i<=n-1; i++)
    {
        state->tmpretrdelta.ptr.p_double[i] = xvirt->x.ptr.p_double[i]-xtrue->x.ptr.p_double[i];
        allsame = allsame&&xtrue->x.ptr.p_double[i]==xvirt->x.ptr.p_double[i];
    }
    if( allsame )
    {
        vfjcopy(xtrue, xvirt, _state);
        return result;
    }
    xvirt->sj.matrixtype = -10080;
    xvirt->sj.m = m;
    xvirt->sj.n = n;
    iallocv(m+1, &xvirt->sj.ridx, _state);
    xvirt->sj.ridx.ptr.p_int[0] = 0;
    for(i=0; i<=state->cntnlc; i++)
    {
        j0 = xtrue->sj.ridx.ptr.p_int[i];
        j1 = xtrue->sj.ridx.ptr.p_int[i+1]-1;
        
        /*
         * Function value
         */
        xvirt->fi.ptr.p_double[i] = xtrue->fi.ptr.p_double[i];
        for(j=j0; j<=j1; j++)
        {
            xvirt->fi.ptr.p_double[i] = xvirt->fi.ptr.p_double[i]+xtrue->sj.vals.ptr.p_double[j]*state->tmpretrdelta.ptr.p_double[xtrue->sj.idx.ptr.p_int[j]];
        }
        
        /*
         * Sparse Jacobian row
         */
        offs = xvirt->sj.ridx.ptr.p_int[i];
        igrowv(offs+(j1-j0+1), &xvirt->sj.idx, _state);
        rgrowv(offs+(j1-j0+1), &xvirt->sj.vals, _state);
        for(j=j0; j<=j1; j++)
        {
            xvirt->sj.idx.ptr.p_int[offs] = xtrue->sj.idx.ptr.p_int[j];
            xvirt->sj.vals.ptr.p_double[offs] = xtrue->sj.vals.ptr.p_double[j];
            offs = offs+1;
        }
        
        /*
         * Done
         */
        xvirt->sj.ridx.ptr.p_int[i+1] = offs;
    }
    return result;
}


/*************************************************************************
This function calculates target and its gradient, with possibility of
applying additional stabilizing modification which does not change
constrained solution.
*************************************************************************/
static void nlcaul_modtargetfg(minaulstate* state,
     const varsfuncjac* s,
     double rhobc,
     double rhoxc,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;

    *f = 0.0;

    n = state->n;
    ae_assert(g->cnt>=n, "AUL: integrity check 5717 failed", _state);
    ae_assert(((!s->isdense&&(s->sj.matrixtype==1||s->sj.matrixtype==-10080))&&s->sj.m>=1)&&s->sj.n==n, "AUL: integrity check 5820 failed", _state);
    
    /*
     * Target function
     */
    rsetv(n, 0.0, g, _state);
    *f = s->fi.ptr.p_double[0];
    j0 = s->sj.ridx.ptr.p_int[0];
    j1 = s->sj.ridx.ptr.p_int[1]-1;
    for(jj=j0; jj<=j1; jj++)
    {
        g->ptr.p_double[s->sj.idx.ptr.p_int[jj]] = s->sj.vals.ptr.p_double[jj];
    }
}


/*************************************************************************
This function calculates augmented (penalized) Lagrangian of the problem
*************************************************************************/
static void nlcaul_auglagfg(minaulstate* state,
     const varsfuncjac* s,
     double rho,
     /* Real    */ const ae_vector* lagmultbc2,
     /* Real    */ const ae_vector* lagmultxc2,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    double ci;
    double v;
    double aix;
    double fi;
    double p;
    double dp;
    double d2p;
    double invrho;
    double stabrho;
    double rhoci;

    *f = 0.0;

    n = state->n;
    cntlc = state->cntlc;
    cntnlc = state->cntnlc;
    ae_assert(ae_fp_greater(rho,(double)(0)), "AUL: integrity check 1459 failed", _state);
    ae_assert(g->cnt>=n, "AUL: integrity check 7517 failed", _state);
    ae_assert(((!s->isdense&&(s->sj.matrixtype==1||s->sj.matrixtype==-10080))&&s->sj.m>=1)&&s->sj.n==n, "AUL: integrity check 7720 failed", _state);
    ae_assert(state->cntlc==0||((state->sparsea.matrixtype==1&&state->sparsea.m==state->cntlc)&&state->sparsea.n==n), "AUL: integrity check 3528 failed", _state);
    invrho = (double)1/rho;
    
    /*
     * Target function
     */
    nlcaul_modtargetfg(state, s, rho, rho, f, g, _state);
    
    /*
     * Lagrangian terms for box constraints
     */
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Handle equality constraint on a variable
         */
        if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&state->scaledbndl.ptr.p_double[i]==state->scaledbndu.ptr.p_double[i] )
        {
            ci = s->x.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i];
            *f = *f+0.5*rho*ci*ci+lagmultbc2->ptr.p_double[2*i+0]*ci;
            g->ptr.p_double[i] = g->ptr.p_double[i]+rho*ci+lagmultbc2->ptr.p_double[2*i+0];
            continue;
        }
        
        /*
         * Handle lower bound (inequality or range with non-zero width)
         */
        if( state->hasbndl.ptr.p_bool[i] )
        {
            inequalityshiftedbarrierfunction((s->x.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
            *f = *f+p*invrho*lagmultbc2->ptr.p_double[2*i+0];
            g->ptr.p_double[i] = g->ptr.p_double[i]+dp*lagmultbc2->ptr.p_double[2*i+0];
        }
        
        /*
         * Handle upper bound (inequality or range with non-zero width)
         */
        if( state->hasbndu.ptr.p_bool[i] )
        {
            inequalityshiftedbarrierfunction((state->scaledbndu.ptr.p_double[i]-s->x.ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
            *f = *f+p*invrho*lagmultbc2->ptr.p_double[2*i+1];
            g->ptr.p_double[i] = g->ptr.p_double[i]-dp*lagmultbc2->ptr.p_double[2*i+1];
        }
    }
    
    /*
     * Lagrangian and penalty terms for linear constraints
     */
    for(i=0; i<=state->cntlc-1; i++)
    {
        j0 = state->sparsea.ridx.ptr.p_int[i];
        j1 = state->sparsea.ridx.ptr.p_int[i+1]-1;
        
        /*
         * Compute constraint value Ai*x
         */
        ci = (double)(0);
        for(jj=j0; jj<=j1; jj++)
        {
            ci = ci+s->x.ptr.p_double[state->sparsea.idx.ptr.p_int[jj]]*state->sparsea.vals.ptr.p_double[jj];
        }
        
        /*
         * Handle Lagrangian and penalty terms
         */
        if( (state->hasal.ptr.p_bool[i]&&state->hasau.ptr.p_bool[i])&&state->al.ptr.p_double[i]==state->au.ptr.p_double[i] )
        {
            
            /*
             * Equality constraint Ai*x = ALi = AUi
             */
            ci = ci-state->al.ptr.p_double[i];
            *f = *f+lagmultxc2->ptr.p_double[2*i+0]*ci+0.5*rho*ci*ci;
            for(jj=j0; jj<=j1; jj++)
            {
                j = state->sparsea.idx.ptr.p_int[jj];
                v = state->sparsea.vals.ptr.p_double[jj];
                g->ptr.p_double[j] = g->ptr.p_double[j]+lagmultxc2->ptr.p_double[2*i+0]*v+rho*ci*v;
            }
        }
        else
        {
            
            /*
             * Inequality/range constraint
             */
            if( state->hasal.ptr.p_bool[i] )
            {
                inequalityshiftedbarrierfunction((ci-state->al.ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
                *f = *f+lagmultxc2->ptr.p_double[2*i+0]*p*invrho;
                for(jj=j0; jj<=j1; jj++)
                {
                    j = state->sparsea.idx.ptr.p_int[jj];
                    v = state->sparsea.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]+lagmultxc2->ptr.p_double[2*i+0]*dp*v;
                }
            }
            if( state->hasau.ptr.p_bool[i] )
            {
                inequalityshiftedbarrierfunction((state->au.ptr.p_double[i]-ci)*rho+(double)1, &p, &dp, &d2p, _state);
                *f = *f+lagmultxc2->ptr.p_double[2*i+1]*p*invrho;
                for(jj=j0; jj<=j1; jj++)
                {
                    j = state->sparsea.idx.ptr.p_int[jj];
                    v = state->sparsea.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]-lagmultxc2->ptr.p_double[2*i+1]*dp*v;
                }
            }
        }
    }
    
    /*
     * Lagrangian and penalty terms for nonlinear constraints
     */
    for(i=0; i<=cntnlc-1; i++)
    {
        j0 = s->sj.ridx.ptr.p_int[1+i];
        j1 = s->sj.ridx.ptr.p_int[1+i+1]-1;
        ci = s->fi.ptr.p_double[1+i];
        
        /*
         * Handle Lagrangian and penalty terms
         */
        if( (state->hasnl.ptr.p_bool[i]&&state->hasnu.ptr.p_bool[i])&&state->rawnl.ptr.p_double[i]==state->rawnu.ptr.p_double[i] )
        {
            
            /*
             * Equality constraint Fi = NLi = NUi
             */
            ci = ci-state->rawnl.ptr.p_double[i];
            *f = *f+lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*ci+0.5*rho*ci*ci;
            for(jj=j0; jj<=j1; jj++)
            {
                j = s->sj.idx.ptr.p_int[jj];
                v = s->sj.vals.ptr.p_double[jj];
                g->ptr.p_double[j] = g->ptr.p_double[j]+lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*v+rho*ci*v;
            }
        }
        else
        {
            
            /*
             * Inequality/range constraint
             */
            if( state->hasnl.ptr.p_bool[i] )
            {
                inequalityshiftedbarrierfunction((ci-state->rawnl.ptr.p_double[i])*rho+(double)1, &p, &dp, &d2p, _state);
                *f = *f+lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*p*invrho;
                for(jj=j0; jj<=j1; jj++)
                {
                    j = s->sj.idx.ptr.p_int[jj];
                    v = s->sj.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]+lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*dp*v;
                }
            }
            if( state->hasnu.ptr.p_bool[i] )
            {
                inequalityshiftedbarrierfunction((state->rawnu.ptr.p_double[i]-ci)*rho+(double)1, &p, &dp, &d2p, _state);
                *f = *f+lagmultxc2->ptr.p_double[2*cntlc+2*i+1]*p*invrho;
                for(jj=j0; jj<=j1; jj++)
                {
                    j = s->sj.idx.ptr.p_int[jj];
                    v = s->sj.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]-lagmultxc2->ptr.p_double[2*cntlc+2*i+1]*dp*v;
                }
            }
        }
    }
    
    /*
     * Additional stabilizing penalty term which is inactive within the feasible area
     * and close to its boundaries. This term is intended to prevent long steps into
     * the infeasible area.
     */
    stabrho = nlcaul_stabilizingpoint/rho;
    for(i=0; i<=n-1; i++)
    {
        if( state->hasbndl.ptr.p_bool[i] )
        {
            ci = s->x.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i]+stabrho;
            if( ci<0.0 )
            {
                rhoci = rho*ci;
                *f = *f+0.5*rhoci*ci;
                g->ptr.p_double[i] = g->ptr.p_double[i]+rhoci;
            }
        }
        if( state->hasbndu.ptr.p_bool[i] )
        {
            ci = state->scaledbndu.ptr.p_double[i]-s->x.ptr.p_double[i]+stabrho;
            if( ci<0.0 )
            {
                rhoci = rho*ci;
                *f = *f+0.5*rhoci*ci;
                g->ptr.p_double[i] = g->ptr.p_double[i]-rhoci;
            }
        }
    }
    for(i=0; i<=state->cntlc-1; i++)
    {
        j0 = state->sparsea.ridx.ptr.p_int[i];
        j1 = state->sparsea.ridx.ptr.p_int[i+1]-1;
        aix = (double)(0);
        for(jj=j0; jj<=j1; jj++)
        {
            aix = aix+s->x.ptr.p_double[state->sparsea.idx.ptr.p_int[jj]]*state->sparsea.vals.ptr.p_double[jj];
        }
        if( state->hasal.ptr.p_bool[i]&&aix-state->al.ptr.p_double[i]<-stabrho )
        {
            ci = aix-state->al.ptr.p_double[i]+stabrho;
            *f = *f+0.5*rho*ci*ci;
            for(jj=j0; jj<=j1; jj++)
            {
                j = state->sparsea.idx.ptr.p_int[jj];
                v = state->sparsea.vals.ptr.p_double[jj];
                g->ptr.p_double[j] = g->ptr.p_double[j]+rho*ci*v;
            }
        }
        if( state->hasau.ptr.p_bool[i]&&state->au.ptr.p_double[i]-aix<-stabrho )
        {
            ci = state->au.ptr.p_double[i]-aix+stabrho;
            *f = *f+0.5*rho*ci*ci;
            for(jj=j0; jj<=j1; jj++)
            {
                j = state->sparsea.idx.ptr.p_int[jj];
                v = state->sparsea.vals.ptr.p_double[jj];
                g->ptr.p_double[j] = g->ptr.p_double[j]-rho*ci*v;
            }
        }
    }
    for(i=0; i<=state->cntnlc-1; i++)
    {
        j0 = s->sj.ridx.ptr.p_int[1+i];
        j1 = s->sj.ridx.ptr.p_int[1+i+1]-1;
        fi = s->fi.ptr.p_double[1+i];
        if( state->hasnl.ptr.p_bool[i] )
        {
            ci = fi-state->rawnl.ptr.p_double[i]+stabrho;
            if( ci<0.0 )
            {
                rhoci = rho*ci;
                *f = *f+0.5*rhoci*ci;
                for(jj=j0; jj<=j1; jj++)
                {
                    j = s->sj.idx.ptr.p_int[jj];
                    v = s->sj.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]+rhoci*v;
                }
            }
        }
        if( state->hasnu.ptr.p_bool[i] )
        {
            ci = state->rawnu.ptr.p_double[i]-fi+stabrho;
            if( ci<0.0 )
            {
                rhoci = rho*ci;
                *f = *f+0.5*rhoci*ci;
                for(jj=j0; jj<=j1; jj++)
                {
                    j = s->sj.idx.ptr.p_int[jj];
                    v = s->sj.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]-rhoci*v;
                }
            }
        }
    }
}


/*************************************************************************
This function calculates raw Lagrangian of the problem
*************************************************************************/
static void nlcaul_rawlagfg(minaulstate* state,
     const varsfuncjac* s,
     /* Real    */ const ae_vector* lagmultbc2,
     /* Real    */ const ae_vector* lagmultxc2,
     double* f,
     /* Real    */ ae_vector* g,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t i;
    ae_int_t j;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    double fi;
    double ci;
    double v;

    *f = 0.0;

    n = state->n;
    cntlc = state->cntlc;
    ae_assert(g->cnt>=n, "AUL: integrity check 7517 failed", _state);
    ae_assert(((!s->isdense&&(s->sj.matrixtype==1||s->sj.matrixtype==-10080))&&s->sj.m>=1)&&s->sj.n==n, "AUL: integrity check 7720 failed", _state);
    ae_assert(state->cntlc==0||((state->sparsea.matrixtype==1&&state->sparsea.m==state->cntlc)&&state->sparsea.n==n), "AUL: integrity check 3528 failed", _state);
    
    /*
     * Target function
     */
    rsetv(n, 0.0, g, _state);
    *f = s->fi.ptr.p_double[0];
    j0 = s->sj.ridx.ptr.p_int[0];
    j1 = s->sj.ridx.ptr.p_int[1]-1;
    for(jj=j0; jj<=j1; jj++)
    {
        g->ptr.p_double[s->sj.idx.ptr.p_int[jj]] = s->sj.vals.ptr.p_double[jj];
    }
    
    /*
     * Lagrangian terms for box constraints
     */
    for(i=0; i<=n-1; i++)
    {
        
        /*
         * Handle equality constraint on a variable
         */
        if( (state->hasbndl.ptr.p_bool[i]&&state->hasbndu.ptr.p_bool[i])&&ae_fp_eq(state->scaledbndl.ptr.p_double[i],state->scaledbndu.ptr.p_double[i]) )
        {
            ci = s->x.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i];
            *f = *f+lagmultbc2->ptr.p_double[2*i+0]*ci;
            g->ptr.p_double[i] = g->ptr.p_double[i]+lagmultbc2->ptr.p_double[2*i+0];
            continue;
        }
        
        /*
         * Handle lower bound (inequality or range with non-zero width)
         */
        if( state->hasbndl.ptr.p_bool[i] )
        {
            *f = *f-lagmultbc2->ptr.p_double[2*i+0]*(s->x.ptr.p_double[i]-state->scaledbndl.ptr.p_double[i]);
            g->ptr.p_double[i] = g->ptr.p_double[i]-lagmultbc2->ptr.p_double[2*i+0];
        }
        
        /*
         * Handle upper bound (inequality or range with non-zero width)
         */
        if( state->hasbndu.ptr.p_bool[i] )
        {
            *f = *f-lagmultbc2->ptr.p_double[2*i+1]*(state->scaledbndu.ptr.p_double[i]-s->x.ptr.p_double[i]);
            g->ptr.p_double[i] = g->ptr.p_double[i]+lagmultbc2->ptr.p_double[2*i+1];
        }
    }
    
    /*
     * Lagrangian and penalty terms for linear constraints
     */
    for(i=0; i<=state->cntlc-1; i++)
    {
        j0 = state->sparsea.ridx.ptr.p_int[i];
        j1 = state->sparsea.ridx.ptr.p_int[i+1]-1;
        
        /*
         * Compute constraint value Ai*x
         */
        ci = (double)(0);
        for(jj=j0; jj<=j1; jj++)
        {
            ci = ci+s->x.ptr.p_double[state->sparsea.idx.ptr.p_int[jj]]*state->sparsea.vals.ptr.p_double[jj];
        }
        
        /*
         * Handle Lagrangian and penalty terms
         */
        if( (state->hasal.ptr.p_bool[i]&&state->hasau.ptr.p_bool[i])&&ae_fp_eq(state->al.ptr.p_double[i],state->au.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint Ai*x = ALi = AUi
             */
            ci = ci-state->al.ptr.p_double[i];
            *f = *f+lagmultxc2->ptr.p_double[2*i+0]*ci;
            for(jj=j0; jj<=j1; jj++)
            {
                j = state->sparsea.idx.ptr.p_int[jj];
                v = state->sparsea.vals.ptr.p_double[jj];
                g->ptr.p_double[j] = g->ptr.p_double[j]+lagmultxc2->ptr.p_double[2*i+0]*v;
            }
        }
        else
        {
            
            /*
             * Inequality/range constraint
             */
            if( state->hasal.ptr.p_bool[i] )
            {
                *f = *f-lagmultxc2->ptr.p_double[2*i+0]*(ci-state->al.ptr.p_double[i]);
                for(jj=j0; jj<=j1; jj++)
                {
                    j = state->sparsea.idx.ptr.p_int[jj];
                    v = state->sparsea.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]-lagmultxc2->ptr.p_double[2*i+0]*v;
                }
            }
            if( state->hasau.ptr.p_bool[i] )
            {
                
                /*
                 * Inequality constraint Ai*x <= AUi
                 */
                *f = *f-lagmultxc2->ptr.p_double[2*i+1]*(state->au.ptr.p_double[i]-ci);
                for(jj=j0; jj<=j1; jj++)
                {
                    j = state->sparsea.idx.ptr.p_int[jj];
                    v = state->sparsea.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]+lagmultxc2->ptr.p_double[2*i+1]*v;
                }
            }
        }
    }
    for(i=0; i<=state->cntnlc-1; i++)
    {
        j0 = s->sj.ridx.ptr.p_int[1+i];
        j1 = s->sj.ridx.ptr.p_int[1+i+1]-1;
        fi = s->fi.ptr.p_double[1+i];
        
        /*
         * Handle Lagrangian and penalty terms
         */
        if( (state->hasnl.ptr.p_bool[i]&&state->hasnu.ptr.p_bool[i])&&ae_fp_eq(state->rawnl.ptr.p_double[i],state->rawnu.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint Fi = NLi = NUi
             */
            ci = fi-state->rawnl.ptr.p_double[i];
            *f = *f+lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*ci;
            for(jj=j0; jj<=j1; jj++)
            {
                j = s->sj.idx.ptr.p_int[jj];
                v = s->sj.vals.ptr.p_double[jj];
                g->ptr.p_double[j] = g->ptr.p_double[j]+lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*v;
            }
        }
        else
        {
            
            /*
             * Inequality/range constraint
             */
            if( state->hasnl.ptr.p_bool[i] )
            {
                
                /*
                 * Inequality constraint Ai*x >= ALi
                 */
                *f = *f-lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*(fi-state->rawnl.ptr.p_double[i]);
                for(jj=j0; jj<=j1; jj++)
                {
                    j = s->sj.idx.ptr.p_int[jj];
                    v = s->sj.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]-lagmultxc2->ptr.p_double[2*cntlc+2*i+0]*v;
                }
            }
            if( state->hasnu.ptr.p_bool[i] )
            {
                
                /*
                 * Inequality constraint Ai*x <= AUi
                 */
                *f = *f-lagmultxc2->ptr.p_double[2*cntlc+2*i+1]*(state->rawnu.ptr.p_double[i]-fi);
                for(jj=j0; jj<=j1; jj++)
                {
                    j = s->sj.idx.ptr.p_int[jj];
                    v = s->sj.vals.ptr.p_double[jj];
                    g->ptr.p_double[j] = g->ptr.p_double[j]+lagmultxc2->ptr.p_double[2*cntlc+2*i+1]*v;
                }
            }
        }
    }
}


/*************************************************************************
Clears request fileds (to be sure that we didn't forget to clear something)
*************************************************************************/
static void nlcaul_clearrequestfields(minaulstate* state,
     ae_state *_state)
{


    state->needsj = ae_false;
    state->precrefreshupcoming = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Compute primal/dual/complementary slackness errors
*************************************************************************/
static void nlcaul_computeerrors(minaulstate* sstate,
     const varsfuncjac* s,
     double* errprim,
     double* errdual,
     double* errcmpl,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t jj;
    ae_int_t j0;
    ae_int_t j1;
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    double ci;
    double v;

    *errprim = 0.0;
    *errdual = 0.0;
    *errcmpl = 0.0;

    n = sstate->n;
    cntlc = sstate->cntlc;
    cntnlc = sstate->cntnlc;
    *errprim = 0.0;
    for(i=0; i<=n-1; i++)
    {
        if( sstate->hasbndl.ptr.p_bool[i] )
        {
            *errprim = ae_maxreal(*errprim, sstate->scaledbndl.ptr.p_double[i]-s->x.ptr.p_double[i], _state);
        }
        if( sstate->hasbndu.ptr.p_bool[i] )
        {
            *errprim = ae_maxreal(*errprim, s->x.ptr.p_double[i]-sstate->scaledbndu.ptr.p_double[i], _state);
        }
    }
    for(i=0; i<=cntlc-1; i++)
    {
        j0 = sstate->sparsea.ridx.ptr.p_int[i];
        j1 = sstate->sparsea.ridx.ptr.p_int[i+1]-1;
        ci = (double)(0);
        for(jj=j0; jj<=j1; jj++)
        {
            ci = ci+s->x.ptr.p_double[sstate->sparsea.idx.ptr.p_int[jj]]*sstate->sparsea.vals.ptr.p_double[jj];
        }
        if( sstate->hasal.ptr.p_bool[i] )
        {
            *errprim = ae_maxreal(*errprim, sstate->al.ptr.p_double[i]-ci, _state);
        }
        if( sstate->hasau.ptr.p_bool[i] )
        {
            *errprim = ae_maxreal(*errprim, ci-sstate->au.ptr.p_double[i], _state);
        }
    }
    for(i=0; i<=cntnlc-1; i++)
    {
        ci = s->fi.ptr.p_double[1+i];
        if( sstate->hasnl.ptr.p_bool[i] )
        {
            *errprim = ae_maxreal(*errprim, sstate->rawnl.ptr.p_double[i]-ci, _state);
        }
        if( sstate->hasnu.ptr.p_bool[i] )
        {
            *errprim = ae_maxreal(*errprim, ci-sstate->rawnu.ptr.p_double[i], _state);
        }
    }
    rallocv(n, &sstate->tmpg, _state);
    nlcaul_rawlagfg(sstate, s, &sstate->lagmultbc2, &sstate->lagmultxc2, &v, &sstate->tmpg, _state);
    *errdual = rmaxabsv(n, &sstate->tmpg, _state);
    *errcmpl = 0.0;
    for(i=0; i<=n-1; i++)
    {
        if( (sstate->hasbndl.ptr.p_bool[i]&&sstate->hasbndu.ptr.p_bool[i])&&ae_fp_eq(sstate->scaledbndl.ptr.p_double[i],sstate->scaledbndu.ptr.p_double[i]) )
        {
            *errcmpl = ae_maxreal(*errcmpl, ae_fabs((sstate->scaledbndl.ptr.p_double[i]-s->x.ptr.p_double[i])*sstate->lagmultbc2.ptr.p_double[2*i+0], _state), _state);
            continue;
        }
        if( sstate->hasbndl.ptr.p_bool[i] )
        {
            *errcmpl = ae_maxreal(*errcmpl, ae_fabs((sstate->scaledbndl.ptr.p_double[i]-s->x.ptr.p_double[i])*sstate->lagmultbc2.ptr.p_double[2*i+0], _state), _state);
        }
        if( sstate->hasbndu.ptr.p_bool[i] )
        {
            *errcmpl = ae_maxreal(*errcmpl, ae_fabs((s->x.ptr.p_double[i]-sstate->scaledbndu.ptr.p_double[i])*sstate->lagmultbc2.ptr.p_double[2*i+1], _state), _state);
        }
    }
    for(i=0; i<=cntlc-1; i++)
    {
        j0 = sstate->sparsea.ridx.ptr.p_int[i];
        j1 = sstate->sparsea.ridx.ptr.p_int[i+1]-1;
        ci = (double)(0);
        for(jj=j0; jj<=j1; jj++)
        {
            ci = ci+s->x.ptr.p_double[sstate->sparsea.idx.ptr.p_int[jj]]*sstate->sparsea.vals.ptr.p_double[jj];
        }
        if( (sstate->hasal.ptr.p_bool[i]&&sstate->hasau.ptr.p_bool[i])&&ae_fp_eq(sstate->al.ptr.p_double[i],sstate->au.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint
             */
            *errcmpl = ae_maxreal(*errcmpl, ae_fabs((ci-sstate->al.ptr.p_double[i])*sstate->lagmultxc2.ptr.p_double[2*i+0], _state), _state);
        }
        else
        {
            
            /*
             * No constraints, inequality constraint or range constraint
             */
            if( sstate->hasal.ptr.p_bool[i] )
            {
                *errcmpl = ae_maxreal(*errcmpl, ae_fabs((sstate->al.ptr.p_double[i]-ci)*sstate->lagmultxc2.ptr.p_double[2*i+0], _state), _state);
            }
            if( sstate->hasau.ptr.p_bool[i] )
            {
                *errcmpl = ae_maxreal(*errcmpl, ae_fabs((ci-sstate->au.ptr.p_double[i])*sstate->lagmultxc2.ptr.p_double[2*i+1], _state), _state);
            }
        }
    }
    for(i=0; i<=cntnlc-1; i++)
    {
        ci = s->fi.ptr.p_double[1+i];
        if( (sstate->hasnl.ptr.p_bool[i]&&sstate->hasnu.ptr.p_bool[i])&&ae_fp_eq(sstate->rawnl.ptr.p_double[i],sstate->rawnu.ptr.p_double[i]) )
        {
            
            /*
             * Equality constraint
             */
            *errcmpl = ae_maxreal(*errcmpl, ae_fabs((ci-sstate->rawnl.ptr.p_double[i])*sstate->lagmultxc2.ptr.p_double[2*cntlc+2*i+0], _state), _state);
        }
        else
        {
            
            /*
             * No constraints, inequality constraint or range constraint
             */
            if( sstate->hasnl.ptr.p_bool[i] )
            {
                *errcmpl = ae_maxreal(*errcmpl, ae_fabs((sstate->rawnl.ptr.p_double[i]-ci)*sstate->lagmultxc2.ptr.p_double[2*cntlc+2*i+0], _state), _state);
            }
            if( sstate->hasnu.ptr.p_bool[i] )
            {
                *errcmpl = ae_maxreal(*errcmpl, ae_fabs((ci-sstate->rawnu.ptr.p_double[i])*sstate->lagmultxc2.ptr.p_double[2*cntlc+2*i+1], _state), _state);
            }
        }
    }
}


void _minaulpreconditioner_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minaulpreconditioner *p = (minaulpreconditioner*)_p;
    ae_touch_ptr((void*)p);
    _spcholanalysis_init(&p->analysis, _state, make_automatic);
    _sparsematrix_init(&p->augsys, _state, make_automatic);
    ae_matrix_init(&p->s, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->y, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmpcorrc, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpcorrd, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpdiag, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->priorities, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->dx, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->dummys, 0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->dummyy, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->alphak, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rhok, 0, DT_REAL, _state, make_automatic);
}


void _minaulpreconditioner_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minaulpreconditioner       *dst = (minaulpreconditioner*)_dst;
    const minaulpreconditioner *src = (const minaulpreconditioner*)_src;
    dst->refreshfreq = src->refreshfreq;
    dst->modelage = src->modelage;
    _spcholanalysis_init_copy(&dst->analysis, &src->analysis, _state, make_automatic);
    _sparsematrix_init_copy(&dst->augsys, &src->augsys, _state, make_automatic);
    dst->naug = src->naug;
    ae_matrix_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_matrix_init_copy(&dst->y, &src->y, _state, make_automatic);
    dst->updcnt = src->updcnt;
    ae_matrix_init_copy(&dst->tmpcorrc, &src->tmpcorrc, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpcorrd, &src->tmpcorrd, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpdiag, &src->tmpdiag, _state, make_automatic);
    ae_vector_init_copy(&dst->priorities, &src->priorities, _state, make_automatic);
    ae_vector_init_copy(&dst->dx, &src->dx, _state, make_automatic);
    ae_matrix_init_copy(&dst->dummys, &src->dummys, _state, make_automatic);
    ae_matrix_init_copy(&dst->dummyy, &src->dummyy, _state, make_automatic);
    ae_vector_init_copy(&dst->alphak, &src->alphak, _state, make_automatic);
    ae_vector_init_copy(&dst->rhok, &src->rhok, _state, make_automatic);
}


void _minaulpreconditioner_clear(void* _p)
{
    minaulpreconditioner *p = (minaulpreconditioner*)_p;
    ae_touch_ptr((void*)p);
    _spcholanalysis_clear(&p->analysis);
    _sparsematrix_clear(&p->augsys);
    ae_matrix_clear(&p->s);
    ae_matrix_clear(&p->y);
    ae_matrix_clear(&p->tmpcorrc);
    ae_vector_clear(&p->tmpcorrd);
    ae_vector_clear(&p->tmpdiag);
    ae_vector_clear(&p->priorities);
    ae_vector_clear(&p->dx);
    ae_matrix_clear(&p->dummys);
    ae_matrix_clear(&p->dummyy);
    ae_vector_clear(&p->alphak);
    ae_vector_clear(&p->rhok);
}


void _minaulpreconditioner_destroy(void* _p)
{
    minaulpreconditioner *p = (minaulpreconditioner*)_p;
    ae_touch_ptr((void*)p);
    _spcholanalysis_destroy(&p->analysis);
    _sparsematrix_destroy(&p->augsys);
    ae_matrix_destroy(&p->s);
    ae_matrix_destroy(&p->y);
    ae_matrix_destroy(&p->tmpcorrc);
    ae_vector_destroy(&p->tmpcorrd);
    ae_vector_destroy(&p->tmpdiag);
    ae_vector_destroy(&p->priorities);
    ae_vector_destroy(&p->dx);
    ae_matrix_destroy(&p->dummys);
    ae_matrix_destroy(&p->dummyy);
    ae_vector_destroy(&p->alphak);
    ae_vector_destroy(&p->rhok);
}


void _minaulstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    minaulstate *p = (minaulstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->s, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->scaledbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->scaledbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->finitebndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->finitebndu, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sparsea, _state, make_automatic);
    ae_vector_init(&p->al, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->au, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasal, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasau, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->rawnl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rawnu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->hasnl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->hasnu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->lcsrcidx, 0, DT_INT, _state, make_automatic);
    _nlpstoppingcriteria_init(&p->criteria, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->sj, _state, make_automatic);
    _varsfuncjac_init(&p->xvirt, _state, make_automatic);
    _varsfuncjac_init(&p->xvirtprev, _state, make_automatic);
    _varsfuncjac_init(&p->xvirtbest, _state, make_automatic);
    _varsfuncjac_init(&p->xtrue, _state, make_automatic);
    _varsfuncjac_init(&p->xtrueprev, _state, make_automatic);
    _varsfuncjac_init(&p->xtruebest, _state, make_automatic);
    ae_vector_init(&p->lagmultbc2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->lagmultxc2, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->du, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fscales, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tracegamma, 0, DT_REAL, _state, make_automatic);
    _minaulpreconditioner_init(&p->preconditioner, _state, make_automatic);
    _xbfgshessian_init(&p->hessaug, _state, make_automatic);
    _xbfgshessian_init(&p->hesstgt, _state, make_automatic);
    ae_vector_init(&p->laggrad0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->laggradcur, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->modtgtgrad0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->modtgtgrad1, 0, DT_REAL, _state, make_automatic);
    _linminstate_init(&p->lstate, _state, make_automatic);
    ae_vector_init(&p->ascales, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpzero, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpy, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->dummy2, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpretrdelta, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpg, 0, DT_REAL, _state, make_automatic);
    _varsfuncjac_init(&p->xvirtprobe, _state, make_automatic);
    _varsfuncjac_init(&p->xtrueprobe, _state, make_automatic);
    ae_vector_init(&p->work, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
}


void _minaulstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    minaulstate       *dst = (minaulstate*)_dst;
    const minaulstate *src = (const minaulstate*)_src;
    dst->n = src->n;
    dst->cntlc = src->cntlc;
    dst->cntnlc = src->cntnlc;
    ae_vector_init_copy(&dst->s, &src->s, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndl, &src->hasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasbndu, &src->hasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndl, &src->scaledbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->scaledbndu, &src->scaledbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->finitebndl, &src->finitebndl, _state, make_automatic);
    ae_vector_init_copy(&dst->finitebndu, &src->finitebndu, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sparsea, &src->sparsea, _state, make_automatic);
    ae_vector_init_copy(&dst->al, &src->al, _state, make_automatic);
    ae_vector_init_copy(&dst->au, &src->au, _state, make_automatic);
    ae_vector_init_copy(&dst->hasal, &src->hasal, _state, make_automatic);
    ae_vector_init_copy(&dst->hasau, &src->hasau, _state, make_automatic);
    ae_vector_init_copy(&dst->rawnl, &src->rawnl, _state, make_automatic);
    ae_vector_init_copy(&dst->rawnu, &src->rawnu, _state, make_automatic);
    ae_vector_init_copy(&dst->hasnl, &src->hasnl, _state, make_automatic);
    ae_vector_init_copy(&dst->hasnu, &src->hasnu, _state, make_automatic);
    ae_vector_init_copy(&dst->lcsrcidx, &src->lcsrcidx, _state, make_automatic);
    _nlpstoppingcriteria_init_copy(&dst->criteria, &src->criteria, _state, make_automatic);
    dst->maxouterits = src->maxouterits;
    dst->restartfreq = src->restartfreq;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    _sparsematrix_init_copy(&dst->sj, &src->sj, _state, make_automatic);
    dst->f = src->f;
    dst->needsj = src->needsj;
    dst->precrefreshupcoming = src->precrefreshupcoming;
    dst->xupdated = src->xupdated;
    dst->rho = src->rho;
    _varsfuncjac_init_copy(&dst->xvirt, &src->xvirt, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->xvirtprev, &src->xvirtprev, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->xvirtbest, &src->xvirtbest, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->xtrue, &src->xtrue, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->xtrueprev, &src->xtrueprev, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->xtruebest, &src->xtruebest, _state, make_automatic);
    dst->besterr = src->besterr;
    dst->bestiteridx = src->bestiteridx;
    dst->besterrnegligibleupdates = src->besterrnegligibleupdates;
    ae_vector_init_copy(&dst->lagmultbc2, &src->lagmultbc2, _state, make_automatic);
    ae_vector_init_copy(&dst->lagmultxc2, &src->lagmultxc2, _state, make_automatic);
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    ae_vector_init_copy(&dst->du, &src->du, _state, make_automatic);
    ae_vector_init_copy(&dst->fscales, &src->fscales, _state, make_automatic);
    ae_vector_init_copy(&dst->tracegamma, &src->tracegamma, _state, make_automatic);
    _minaulpreconditioner_init_copy(&dst->preconditioner, &src->preconditioner, _state, make_automatic);
    _xbfgshessian_init_copy(&dst->hessaug, &src->hessaug, _state, make_automatic);
    _xbfgshessian_init_copy(&dst->hesstgt, &src->hesstgt, _state, make_automatic);
    ae_vector_init_copy(&dst->laggrad0, &src->laggrad0, _state, make_automatic);
    ae_vector_init_copy(&dst->laggradcur, &src->laggradcur, _state, make_automatic);
    ae_vector_init_copy(&dst->modtgtgrad0, &src->modtgtgrad0, _state, make_automatic);
    ae_vector_init_copy(&dst->modtgtgrad1, &src->modtgtgrad1, _state, make_automatic);
    dst->inneriterationscount = src->inneriterationscount;
    dst->outeriterationscount = src->outeriterationscount;
    dst->fstagnationcnt = src->fstagnationcnt;
    dst->longeststp = src->longeststp;
    dst->mcstage = src->mcstage;
    _linminstate_init_copy(&dst->lstate, &src->lstate, _state, make_automatic);
    dst->stp = src->stp;
    dst->stplimit = src->stplimit;
    ae_vector_init_copy(&dst->ascales, &src->ascales, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpzero, &src->tmpzero, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpy, &src->tmpy, _state, make_automatic);
    ae_matrix_init_copy(&dst->dummy2, &src->dummy2, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpretrdelta, &src->tmpretrdelta, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpg, &src->tmpg, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->xvirtprobe, &src->xvirtprobe, _state, make_automatic);
    _varsfuncjac_init_copy(&dst->xtrueprobe, &src->xtrueprobe, _state, make_automatic);
    dst->repiterationscount = src->repiterationscount;
    dst->repterminationtype = src->repterminationtype;
    dst->repnfev = src->repnfev;
    dst->repbcerr = src->repbcerr;
    dst->repbcidx = src->repbcidx;
    dst->replcerr = src->replcerr;
    dst->replcidx = src->replcidx;
    dst->repnlcerr = src->repnlcerr;
    dst->repnlcidx = src->repnlcidx;
    dst->nfev = src->nfev;
    ae_vector_init_copy(&dst->work, &src->work, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
}


void _minaulstate_clear(void* _p)
{
    minaulstate *p = (minaulstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->s);
    ae_vector_clear(&p->hasbndl);
    ae_vector_clear(&p->hasbndu);
    ae_vector_clear(&p->scaledbndl);
    ae_vector_clear(&p->scaledbndu);
    ae_vector_clear(&p->finitebndl);
    ae_vector_clear(&p->finitebndu);
    _sparsematrix_clear(&p->sparsea);
    ae_vector_clear(&p->al);
    ae_vector_clear(&p->au);
    ae_vector_clear(&p->hasal);
    ae_vector_clear(&p->hasau);
    ae_vector_clear(&p->rawnl);
    ae_vector_clear(&p->rawnu);
    ae_vector_clear(&p->hasnl);
    ae_vector_clear(&p->hasnu);
    ae_vector_clear(&p->lcsrcidx);
    _nlpstoppingcriteria_clear(&p->criteria);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    _sparsematrix_clear(&p->sj);
    _varsfuncjac_clear(&p->xvirt);
    _varsfuncjac_clear(&p->xvirtprev);
    _varsfuncjac_clear(&p->xvirtbest);
    _varsfuncjac_clear(&p->xtrue);
    _varsfuncjac_clear(&p->xtrueprev);
    _varsfuncjac_clear(&p->xtruebest);
    ae_vector_clear(&p->lagmultbc2);
    ae_vector_clear(&p->lagmultxc2);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->du);
    ae_vector_clear(&p->fscales);
    ae_vector_clear(&p->tracegamma);
    _minaulpreconditioner_clear(&p->preconditioner);
    _xbfgshessian_clear(&p->hessaug);
    _xbfgshessian_clear(&p->hesstgt);
    ae_vector_clear(&p->laggrad0);
    ae_vector_clear(&p->laggradcur);
    ae_vector_clear(&p->modtgtgrad0);
    ae_vector_clear(&p->modtgtgrad1);
    _linminstate_clear(&p->lstate);
    ae_vector_clear(&p->ascales);
    ae_vector_clear(&p->tmpzero);
    ae_vector_clear(&p->tmpy);
    ae_matrix_clear(&p->dummy2);
    ae_vector_clear(&p->tmpretrdelta);
    ae_vector_clear(&p->tmpg);
    _varsfuncjac_clear(&p->xvirtprobe);
    _varsfuncjac_clear(&p->xtrueprobe);
    ae_vector_clear(&p->work);
    _rcommstate_clear(&p->rstate);
}


void _minaulstate_destroy(void* _p)
{
    minaulstate *p = (minaulstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->s);
    ae_vector_destroy(&p->hasbndl);
    ae_vector_destroy(&p->hasbndu);
    ae_vector_destroy(&p->scaledbndl);
    ae_vector_destroy(&p->scaledbndu);
    ae_vector_destroy(&p->finitebndl);
    ae_vector_destroy(&p->finitebndu);
    _sparsematrix_destroy(&p->sparsea);
    ae_vector_destroy(&p->al);
    ae_vector_destroy(&p->au);
    ae_vector_destroy(&p->hasal);
    ae_vector_destroy(&p->hasau);
    ae_vector_destroy(&p->rawnl);
    ae_vector_destroy(&p->rawnu);
    ae_vector_destroy(&p->hasnl);
    ae_vector_destroy(&p->hasnu);
    ae_vector_destroy(&p->lcsrcidx);
    _nlpstoppingcriteria_destroy(&p->criteria);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    _sparsematrix_destroy(&p->sj);
    _varsfuncjac_destroy(&p->xvirt);
    _varsfuncjac_destroy(&p->xvirtprev);
    _varsfuncjac_destroy(&p->xvirtbest);
    _varsfuncjac_destroy(&p->xtrue);
    _varsfuncjac_destroy(&p->xtrueprev);
    _varsfuncjac_destroy(&p->xtruebest);
    ae_vector_destroy(&p->lagmultbc2);
    ae_vector_destroy(&p->lagmultxc2);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->du);
    ae_vector_destroy(&p->fscales);
    ae_vector_destroy(&p->tracegamma);
    _minaulpreconditioner_destroy(&p->preconditioner);
    _xbfgshessian_destroy(&p->hessaug);
    _xbfgshessian_destroy(&p->hesstgt);
    ae_vector_destroy(&p->laggrad0);
    ae_vector_destroy(&p->laggradcur);
    ae_vector_destroy(&p->modtgtgrad0);
    ae_vector_destroy(&p->modtgtgrad1);
    _linminstate_destroy(&p->lstate);
    ae_vector_destroy(&p->ascales);
    ae_vector_destroy(&p->tmpzero);
    ae_vector_destroy(&p->tmpy);
    ae_matrix_destroy(&p->dummy2);
    ae_vector_destroy(&p->tmpretrdelta);
    ae_vector_destroy(&p->tmpg);
    _varsfuncjac_destroy(&p->xvirtprobe);
    _varsfuncjac_destroy(&p->xtrueprobe);
    ae_vector_destroy(&p->work);
    _rcommstate_destroy(&p->rstate);
}


/*$ End $*/
