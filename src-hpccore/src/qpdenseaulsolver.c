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
#include "qpdenseaulsolver.h"


/*$ Declarations $*/
static double qpdenseaulsolver_evictionlevel = -0.01;
static double qpdenseaulsolver_expansionratio = 0.20;
static void qpdenseaulsolver_generateexmodel(/* Real    */ const ae_matrix* sclsfta,
     /* Real    */ const ae_vector* sclsftb,
     ae_int_t nmain,
     /* Real    */ const ae_vector* sclsftbndl,
     /* Boolean */ const ae_vector* sclsfthasbndl,
     /* Real    */ const ae_vector* sclsftbndu,
     /* Boolean */ const ae_vector* sclsfthasbndu,
     /* Real    */ const ae_matrix* sclsftcleic,
     ae_int_t sclsftnec,
     ae_int_t sclsftnic,
     /* Real    */ const ae_vector* nulc,
     double rho,
     /* Real    */ ae_matrix* exa,
     /* Real    */ ae_vector* exb,
     /* Real    */ ae_vector* exbndl,
     /* Real    */ ae_vector* exbndu,
     /* Real    */ ae_matrix* tmp2,
     ae_state *_state);
static void qpdenseaulsolver_generateexinitialpoint(/* Real    */ const ae_vector* sclsftxc,
     ae_int_t nmain,
     ae_int_t nslack,
     /* Real    */ ae_vector* exxc,
     ae_state *_state);
static void qpdenseaulsolver_updatelagrangemultipliers(/* Real    */ const ae_matrix* sclsfta,
     /* Real    */ const ae_vector* sclsftb,
     ae_int_t nmain,
     /* Real    */ const ae_vector* sclsftbndl,
     /* Boolean */ const ae_vector* sclsfthasbndl,
     /* Real    */ const ae_vector* sclsftbndu,
     /* Boolean */ const ae_vector* sclsfthasbndu,
     /* Real    */ const ae_matrix* sclsftcleic,
     ae_int_t sclsftnec,
     ae_int_t sclsftnic,
     /* Real    */ const ae_vector* exxc,
     /* Real    */ ae_vector* nulcest,
     qpdenseaulbuffers* buffers,
     ae_state *_state);
static void qpdenseaulsolver_scaleshiftoriginalproblem(const convexquadraticmodel* a,
     const sparsematrix* sparsea,
     ae_int_t akind,
     ae_bool sparseaupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nmain,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t dnec,
     ae_int_t dnic,
     const sparsematrix* scleic,
     ae_int_t snec,
     ae_int_t snic,
     ae_bool renormlc,
     qpdenseaulbuffers* state,
     /* Real    */ ae_vector* xs,
     ae_state *_state);
static double qpdenseaulsolver_normalizequadraticterm(/* Real    */ ae_matrix* a,
     /* Real    */ ae_vector* b,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     ae_bool usecleic,
     /* Real    */ ae_matrix* tmp2,
     ae_state *_state);
static void qpdenseaulsolver_selectinitialworkingset(/* Real    */ const ae_matrix* a,
     ae_int_t nmain,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     /* Real    */ ae_vector* tmp0,
     /* Real    */ ae_matrix* tmp2,
     ae_int_t* nicwork,
     ae_bool* allowwseviction,
     ae_state *_state);


/*$ Body $*/


/*************************************************************************
This function initializes QPDENSEAULSettings structure with default settings.

Newly created structure MUST be initialized by default settings  -  or  by
copy of the already initialized structure.

  -- ALGLIB --
     Copyright 14.05.2011 by Bochkanov Sergey
*************************************************************************/
void qpdenseaulloaddefaults(ae_int_t nmain,
     qpdenseaulsettings* s,
     ae_state *_state)
{


    s->epsx = 1.0E-6;
    s->outerits = 5;
    s->rho = 100.0;
}


/*************************************************************************
This function runs Dense-AUL solver; it returns after optimization process
was completed. Following QP problem is solved:

    min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))
    
subject to combination of box and general linear dense/sparse constraints.

INPUT PARAMETERS:
    DenseA      -   for dense problems (AKind=0), A-term of CQM object
                    contains system matrix. Other terms are unspecified
                    and should not be referenced.
    SparseA     -   for sparse problems (AKind=1), CRS format
    AKind       -   sparse matrix format:
                    * 0 for dense matrix
                    * 1 for sparse matrix
    SparseUpper -   which triangle of SparseAC stores matrix  -  upper  or
                    lower one (for dense matrices this  parameter  is  not
                    actual).
    B           -   linear term, array[N]
    BndL        -   lower bound, array[N]
    BndU        -   upper bound, array[N]
    S           -   scale vector, array[NC]:
                    * I-th element contains scale of I-th variable,
                    * SC[I]>0
    XOrigin     -   origin term, array[NC]. Can be zero.
    N           -   number of variables in the  original  formulation  (no
                    slack variables).
    CLEIC       -   dense linear equality/inequality constraints. Equality
                    constraints come first.
    NEC, NIC    -   number of dense equality/inequality constraints.
    SCLEIC      -   sparse linear equality/inequality constraints. Equality
                    constraints come first.
    SNEC, SNIC  -   number of sparse equality/inequality constraints.
    RenormLC    -   whether constraints should be renormalized (recommended)
                    or used "as is".
    Settings    -   QPDENSEAULSettings object initialized by one of the initialization
                    functions.
    State       -   object which stores temporaries
    XS          -   initial point, array[NC]
    
    
OUTPUT PARAMETERS:
    XS          -   last point
    TerminationType-termination type:
                    *
                    *
                    *

  -- ALGLIB --
     Copyright 2017 by Bochkanov Sergey
*************************************************************************/
void qpdenseauloptimize(const convexquadraticmodel* a,
     const sparsematrix* sparsea,
     ae_int_t akind,
     ae_bool sparseaupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nn,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t dnec,
     ae_int_t dnic,
     const sparsematrix* scleic,
     ae_int_t snec,
     ae_int_t snic,
     ae_bool renormlc,
     const qpdenseaulsettings* settings,
     qpdenseaulbuffers* state,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    double v;
    double vv;
    double rho;
    double epsx;
    ae_int_t outeridx;
    ae_int_t nmain;
    ae_int_t nslack;
    ae_int_t ntotal;
    ae_int_t ktotal;
    double maxrho;
    double feaserr;
    double feaserrprev;
    double requestedfeasdecrease;
    ae_int_t goodcounter;
    ae_int_t stagnationcounter;
    ae_int_t nectotal;
    ae_int_t nictotal;
    ae_int_t nicwork;
    ae_int_t kwork;
    ae_int_t nwork;
    ae_bool allowwseviction;
    ae_bool workingsetextended;
    double targetscale;

    *terminationtype = 0;

    nmain = nn;
    nslack = dnic+snic;
    ntotal = nmain+nslack;
    nectotal = dnec+snec;
    nictotal = dnic+snic;
    ktotal = dnec+dnic+snec+snic;
    rho = settings->rho;
    epsx = settings->epsx;
    requestedfeasdecrease = 0.33;
    maxrho = 1.0E12;
    if( ae_fp_less_eq(epsx,(double)(0)) )
    {
        epsx = 1.0E-9;
    }
    
    /*
     * Integrity checks
     */
    if( snec+snic>0 )
    {
        ae_assert(scleic->matrixtype==1, "QPDENSEAULOptimize: unexpected sparse matrix format", _state);
        ae_assert(scleic->m==snec+snic, "QPDENSEAULOptimize: unexpected sparse matrix size", _state);
        ae_assert(scleic->n==nmain+1, "QPDENSEAULOptimize: unexpected sparse matrix size", _state);
    }
    
    /*
     * Prepare
     */
    state->repinneriterationscount = 0;
    state->repouteriterationscount = 0;
    state->repncholesky = 0;
    state->repnmv = 0;
    state->repnwrkchanges = 0;
    state->repnwrk0 = 0;
    state->repnwrk1 = 0;
    state->repnwrkf = 0;
    *terminationtype = 0;
    ivectorsetlengthatleast(&state->cidx, ktotal, _state);
    rvectorsetlengthatleast(&state->nulc, ktotal, _state);
    rvectorsetlengthatleast(&state->nulcest, ktotal, _state);
    rvectorsetlengthatleast(&state->exb, ntotal, _state);
    rvectorsetlengthatleast(&state->exxc, ntotal, _state);
    rvectorsetlengthatleast(&state->exxorigin, ntotal, _state);
    rvectorsetlengthatleast(&state->exbndl, ntotal, _state);
    rvectorsetlengthatleast(&state->exbndu, ntotal, _state);
    rvectorsetlengthatleast(&state->exscale, ntotal, _state);
    rvectorsetlengthatleast(&state->tmp0, ntotal, _state);
    rvectorsetlengthatleast(&state->nicerr, nictotal, _state);
    ivectorsetlengthatleast(&state->nicnact, nictotal, _state);
    
    /*
     * Allocate Lagrange multipliers, fill by default values (zeros)
     */
    rvectorsetlengthatleast(lagbc, nmain, _state);
    rvectorsetlengthatleast(laglc, ktotal, _state);
    for(i=0; i<=nmain-1; i++)
    {
        lagbc->ptr.p_double[i] = 0.0;
    }
    for(i=0; i<=ktotal-1; i++)
    {
        laglc->ptr.p_double[i] = 0.0;
    }
    
    /*
     * Prepare scaled/shifted model in dense format - input parameters
     * are converted and stored in State.SclSftA/B/HasBndL/HasBndU/BndL/BndU/CLEIC/XC/CScales
     */
    qpdenseaulsolver_scaleshiftoriginalproblem(a, sparsea, akind, sparseaupper, b, bndl, bndu, s, xorigin, nmain, cleic, dnec, dnic, scleic, snec, snic, renormlc, state, xs, _state);
    
    /*
     * Normalize model in such way that norm(A)~1 (very roughly)
     *
     * We have two lower bounds for sigma_max(A):
     * * first estimate is provided by Frobenius norm, it is equal to ANorm/NMain
     * * second estimate is provided by max(CAC)
     *
     * We select largest one of these estimates, because using just one
     * of them is prone to different failure modes. Then, we divide A and B
     * by this estimate.
     */
    targetscale = qpdenseaulsolver_normalizequadraticterm(&state->sclsfta, &state->sclsftb, nmain, &state->sclsftcleic, nectotal, nictotal, renormlc, &state->tmp2, _state);
    
    /*
     * Select working set of inequality constraints.
     *
     * Although it is possible to process all inequality constraints
     * at once, in one large batch, some QP problems have NIC>>N constraints,
     * but only minor fraction of them is inactive in the solution.
     *
     * Because algorithm running time is O((N+NEC+NIC)^3), we can
     * save a lot of time if we process only those inequality constraints
     * which need activation. Generally, NEC<N, and only O(N) inequality
     * constraints are active in the solution.
     *
     * We can do so by solving problem first without general inequality
     * constraints at all (box and general equality constraints are added),
     * and by iteratively adding more and more inequality constraints in
     * order to get feasible solution. Such set of inequality constraints
     * is called "working set".
     *
     * NOTE: such approach works reliably only for convex QP problems; non-convex
     *       QP problem can be unbounded when some constraints are dropped.
     *
     * NOTE: we can also remove some constraints from working set, but eviction
     *       can be performed only limited amount of times (at most once); if
     *       constraint is added to working set second time, it is never removed.
     *
     * NOTE: we do not perform constraint eviction on non-convex problems
     */
    qpdenseaulsolver_selectinitialworkingset(&state->sclsfta, nmain, &state->sclsftcleic, nectotal, nictotal, &state->tmp0, &state->tmp2, &nicwork, &allowwseviction, _state);
    kwork = nectotal+nicwork;
    nwork = nmain+nicwork;
    state->repnwrk0 = nicwork;
    for(i=0; i<=nicwork-1; i++)
    {
        state->nicnact.ptr.p_int[i] = 1;
    }
    for(i=nicwork; i<=nictotal-1; i++)
    {
        state->nicnact.ptr.p_int[i] = 0;
    }
    for(i=0; i<=ktotal-1; i++)
    {
        state->cidx.ptr.p_int[i] = i;
    }
    
    /*
     * Perform outer iteration
     */
    for(i=0; i<=ktotal-1; i++)
    {
        state->nulc.ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=ntotal-1; i++)
    {
        state->exscale.ptr.p_double[i] = 1.0;
        state->exxorigin.ptr.p_double[i] = 0.0;
    }
    qpdenseaulsolver_generateexinitialpoint(&state->sclsftxc, nmain, nslack, &state->exxc, _state);
    goodcounter = 0;
    stagnationcounter = 0;
    feaserr = ae_maxrealnumber;
    for(outeridx=0; outeridx<=settings->outerits-1; outeridx++)
    {
        
        /*
         * Repeat loop until working set stabilizes.
         */
        do
        {
            
            /*
             * Preallocate space for ExA and for QQP solver; we do not allocate
             * array[NTotal,NTotal] from the start because NTotal can be much
             * larger than NMain for problems with large amount of inequality
             * constraints, and we usually need NWork=O(NMain).
             *
             * NOTE: for the sake of simplicity, 1-dimensional arrays were
             *       preallocated to the maximum size required (NTotal).
             */
            if( state->exa.rows<nwork||state->exa.cols<nwork )
            {
                i = nwork+nwork/3+1;
                rmatrixsetlengthatleast(&state->exa, i, i, _state);
            }
            qqppreallocategrowdense(&state->qqpbuf, nwork, i, _state);
            
            /*
             * Generate penalized quadratic model
             */
            qpdenseaulsolver_generateexmodel(&state->sclsfta, &state->sclsftb, nmain, &state->sclsftbndl, &state->sclsfthasbndl, &state->sclsftbndu, &state->sclsfthasbndu, &state->sclsftcleic, nectotal, nicwork, &state->nulc, rho, &state->exa, &state->exb, &state->exbndl, &state->exbndu, &state->tmp2, _state);
            
            /*
             * Solve extended QP problem subject to current working set of general
             * inequality constraints.
             */
            qqploaddefaults(nwork, &state->qqpsettingsuser, _state);
            state->qqpsettingsuser.maxouterits = 50;
            state->qqpsettingsuser.epsg = 0.0;
            state->qqpsettingsuser.epsf = 0.0;
            state->qqpsettingsuser.epsx = 0.01*epsx;
            state->qqpsettingsuser.cnphase = ae_true;
            qqpoptimize(&state->dummycqm, &state->dummysparse, &state->exa, 2, ae_true, &state->exb, &state->exbndl, &state->exbndu, &state->exscale, &state->exxorigin, nwork, &state->qqpsettingsuser, &state->qqpbuf, &state->exxc, &k, _state);
            state->repncholesky = state->repncholesky+state->qqpbuf.repncholesky;
            
            /*
             * Evaluate violation of constraints
             */
            for(i=0; i<=nictotal-1; i++)
            {
                v = ae_v_dotproduct(&state->sclsftcleic.ptr.pp_double[nectotal+i][0], 1, &state->exxc.ptr.p_double[0], 1, ae_v_len(0,nmain-1));
                v = v-state->sclsftcleic.ptr.pp_double[nectotal+i][nmain];
                state->nicerr.ptr.p_double[i] = v;
            }
            
            /*
             * Working set expansion:
             * * select limited amount of most violated constraints
             * * perform permutation of non-work constraints such that
             *   candidate constraint is first the list (update XC and NuLC)
             * * increase working set size by 1
             * * increase activation count for new constraint by 1 (this count
             *   is used later by working set eviction phase)
             * * repeat
             *
             * NOTE: we use selection sort algorithm because its O(NAdded*NWork) cost
             *       is still comparable to the cost of constraints evaluation
             */
            workingsetextended = ae_false;
            i = 0;
            while(ae_fp_less((double)(i),(double)1+qpdenseaulsolver_expansionratio*(double)nmain)&&nicwork<nictotal)
            {
                
                /*
                 * Select most violated constraint
                 */
                k = nicwork;
                for(j=nicwork; j<=nictotal-1; j++)
                {
                    if( ae_fp_greater(state->nicerr.ptr.p_double[j],state->nicerr.ptr.p_double[k]) )
                    {
                        k = j;
                    }
                }
                
                /*
                 * If violation is positive, add it
                 */
                if( ae_fp_greater(state->nicerr.ptr.p_double[k],(double)(0)) )
                {
                    swaprows(&state->sclsftcleic, nectotal+nicwork, nectotal+k, -1, _state);
                    swapelements(&state->nicerr, nicwork, k, _state);
                    swapelementsi(&state->nicnact, nicwork, k, _state);
                    swapelementsi(&state->cidx, nectotal+nicwork, nectotal+k, _state);
                    swapelements(&state->cscales, nectotal+nicwork, nectotal+k, _state);
                    state->exxc.ptr.p_double[nmain+nicwork] = 0.0;
                    state->nulc.ptr.p_double[nectotal+nicwork] = 0.0;
                    state->nicnact.ptr.p_int[nicwork] = state->nicnact.ptr.p_int[nicwork]+1;
                    inc(&nicwork, _state);
                    inc(&nwork, _state);
                    inc(&kwork, _state);
                    inc(&i, _state);
                    workingsetextended = ae_true;
                }
                else
                {
                    break;
                }
            }
            
            /*
             * Working set eviction:
             * * select constraints which are (1) far away from the
             *   boundary, AND (2) has less than two activation attempts
             *   (if constraint is regularly activated/deactivated, we keep
             *   it in the working set no matter what)
             * * remove such constraints from the working set one by one
             */
            if( allowwseviction )
            {
                for(k=nicwork-1; k>=0; k--)
                {
                    if( ae_fp_less(state->nicerr.ptr.p_double[k],qpdenseaulsolver_evictionlevel)&&state->nicnact.ptr.p_int[k]<=1 )
                    {
                        swaprows(&state->sclsftcleic, nectotal+nicwork-1, nectotal+k, -1, _state);
                        swapelementsi(&state->cidx, nectotal+nicwork-1, nectotal+k, _state);
                        swapelements(&state->cscales, nectotal+nicwork-1, nectotal+k, _state);
                        swapelements(&state->nicerr, nicwork-1, k, _state);
                        swapelementsi(&state->nicnact, nicwork-1, k, _state);
                        swapelements(&state->exxc, nmain+nicwork-1, nmain+k, _state);
                        swapelements(&state->nulc, nectotal+nicwork-1, nectotal+k, _state);
                        dec(&nicwork, _state);
                        dec(&nwork, _state);
                        dec(&kwork, _state);
                    }
                }
            }
            
            /*
             * Report working set statistics
             */
            if( state->repnwrk1==0 )
            {
                state->repnwrk1 = nicwork;
            }
            state->repnwrkf = nicwork;
            if( workingsetextended )
            {
                inc(&state->repnwrkchanges, _state);
            }
        }
        while(workingsetextended);
        
        /*
         * Estimate Lagrange multipliers using alternative algorithm
         */
        ae_v_move(&state->nulcest.ptr.p_double[0], 1, &state->nulc.ptr.p_double[0], 1, ae_v_len(0,kwork-1));
        qpdenseaulsolver_updatelagrangemultipliers(&state->sclsfta, &state->sclsftb, nmain, &state->sclsftbndl, &state->sclsfthasbndl, &state->sclsftbndu, &state->sclsfthasbndu, &state->sclsftcleic, nectotal, nicwork, &state->exxc, &state->nulcest, state, _state);
        
        /*
         * Update XC and Lagrange multipliers
         */
        feaserrprev = feaserr;
        feaserr = (double)(0);
        for(i=0; i<=kwork-1; i++)
        {
            
            /*
             * Calculate I-th feasibility error in V using formula for distance
             * between point and line (here we calculate actual distance between
             * XN and hyperplane Ci'*XN=Bi, which is different from error Ci'*XN-Bi).
             */
            v = (double)(0);
            vv = (double)(0);
            for(j=0; j<=nmain-1; j++)
            {
                v = v+state->sclsftcleic.ptr.pp_double[i][j]*state->exxc.ptr.p_double[j];
                vv = vv+ae_sqr(state->sclsftcleic.ptr.pp_double[i][j], _state);
            }
            if( i>=nectotal )
            {
                v = v+state->exxc.ptr.p_double[nmain+(i-nectotal)];
                vv = vv+ae_sqr((double)(1), _state);
            }
            v = v-state->sclsftcleic.ptr.pp_double[i][nmain];
            vv = coalesce(vv, (double)(1), _state);
            v = v/ae_sqrt(vv, _state);
            
            /*
             * Calculate magnitude of Lagrangian update (and Lagrangian parameters themselves)
             */
            feaserr = feaserr+ae_sqr(v, _state);
            state->nulc.ptr.p_double[i] = state->nulcest.ptr.p_double[i];
        }
        feaserr = ae_sqrt(feaserr, _state);
        if( ae_fp_less(feaserr,epsx) )
        {
            inc(&goodcounter, _state);
        }
        else
        {
            goodcounter = 0;
        }
        if( ae_fp_greater(feaserr,feaserrprev*requestedfeasdecrease) )
        {
            inc(&stagnationcounter, _state);
        }
        else
        {
            stagnationcounter = 0;
        }
        if( goodcounter>=2 )
        {
            break;
        }
        if( stagnationcounter>=2 )
        {
            rho = ae_minreal(rho*10.0, maxrho, _state);
        }
        else
        {
            rho = ae_minreal(rho*1.41, maxrho, _state);
        }
    }
    
    /*
     * Convert Lagrange multipliers from internal format to one expected
     * by caller:
     * * reorder multipliers for linear constraints
     * * compute residual from gradient+linearconstraints
     * * compute multipliers for box constraints from residual
     * * rescale everything
     */
    for(i=0; i<=nectotal+nicwork-1; i++)
    {
        laglc->ptr.p_double[state->cidx.ptr.p_int[i]] = -state->nulc.ptr.p_double[i]*targetscale/state->cscales.ptr.p_double[i];
    }
    rvectorsetlengthatleast(&state->tmpg, nmain, _state);
    for(i=0; i<=nmain-1; i++)
    {
        v = state->sclsftb.ptr.p_double[i];
        for(j=0; j<=nmain-1; j++)
        {
            v = v+state->sclsfta.ptr.pp_double[i][j]*state->exxc.ptr.p_double[j];
        }
        state->tmpg.ptr.p_double[i] = v;
    }
    rmatrixgemv(nmain, nectotal+nicwork, -1.0, &state->sclsftcleic, 0, 0, 1, &state->nulc, 0, 1.0, &state->tmpg, 0, _state);
    for(i=0; i<=nmain-1; i++)
    {
        if( (state->sclsfthasbndl.ptr.p_bool[i]&&ae_fp_eq(state->exxc.ptr.p_double[i],state->sclsftbndl.ptr.p_double[i]))||(state->sclsfthasbndu.ptr.p_bool[i]&&ae_fp_eq(state->exxc.ptr.p_double[i],state->sclsftbndu.ptr.p_double[i])) )
        {
            lagbc->ptr.p_double[i] = -state->tmpg.ptr.p_double[i];
        }
    }
    for(i=0; i<=nmain-1; i++)
    {
        lagbc->ptr.p_double[i] = lagbc->ptr.p_double[i]*targetscale/s->ptr.p_double[i];
    }
    
    /*
     * Unpack results.
     *
     * Add XOrigin to XC and make sure that boundary constraints are
     * satisfied.
     */
    for(i=0; i<=nmain-1; i++)
    {
        
        /*
         * Unscale/unshift
         */
        xs->ptr.p_double[i] = s->ptr.p_double[i]*state->exxc.ptr.p_double[i]+xorigin->ptr.p_double[i];
        
        /*
         * Make sure that point is feasible w.r.t. box constraints.
         * Enforce box constraints which were active in the scaled/shifted solution.
         */
        if( state->sclsfthasbndl.ptr.p_bool[i] )
        {
            if( ae_fp_less(xs->ptr.p_double[i],bndl->ptr.p_double[i]) )
            {
                xs->ptr.p_double[i] = bndl->ptr.p_double[i];
            }
            if( ae_fp_eq(state->exxc.ptr.p_double[i],state->sclsftbndl.ptr.p_double[i]) )
            {
                xs->ptr.p_double[i] = bndl->ptr.p_double[i];
            }
        }
        if( state->sclsfthasbndu.ptr.p_bool[i] )
        {
            if( ae_fp_greater(xs->ptr.p_double[i],bndu->ptr.p_double[i]) )
            {
                xs->ptr.p_double[i] = bndu->ptr.p_double[i];
            }
            if( ae_fp_eq(state->exxc.ptr.p_double[i],state->sclsftbndu.ptr.p_double[i]) )
            {
                xs->ptr.p_double[i] = bndu->ptr.p_double[i];
            }
        }
    }
    *terminationtype = 2;
}


/*************************************************************************
This function generates box-constrained QP problem, which is penalized and
augmented formulation of original linearly constrained problem

  -- ALGLIB --
     Copyright 23.02.2017 by Bochkanov Sergey
*************************************************************************/
static void qpdenseaulsolver_generateexmodel(/* Real    */ const ae_matrix* sclsfta,
     /* Real    */ const ae_vector* sclsftb,
     ae_int_t nmain,
     /* Real    */ const ae_vector* sclsftbndl,
     /* Boolean */ const ae_vector* sclsfthasbndl,
     /* Real    */ const ae_vector* sclsftbndu,
     /* Boolean */ const ae_vector* sclsfthasbndu,
     /* Real    */ const ae_matrix* sclsftcleic,
     ae_int_t sclsftnec,
     ae_int_t sclsftnic,
     /* Real    */ const ae_vector* nulc,
     double rho,
     /* Real    */ ae_matrix* exa,
     /* Real    */ ae_vector* exb,
     /* Real    */ ae_vector* exbndl,
     /* Real    */ ae_vector* exbndu,
     /* Real    */ ae_matrix* tmp2,
     ae_state *_state)
{
    ae_int_t nslack;
    ae_int_t ntotal;
    ae_int_t i;
    ae_int_t j;
    double v;


    nslack = sclsftnic;
    ntotal = nmain+nslack;
    
    /*
     * Integrity check for properly preallocated storage
     */
    ae_assert(exa->rows>=ntotal&&exa->cols>=ntotal, "QPDenseAUL.GenerateExModel - integrity check failed", _state);
    ae_assert((exb->cnt>=ntotal&&exbndl->cnt>=ntotal)&&exbndu->cnt>=ntotal, "QPDenseAUL.GenerateExModel - integrity check failed", _state);
    
    /*
     * Primary quadratic term
     */
    for(i=0; i<=ntotal-1; i++)
    {
        for(j=i; j<=ntotal-1; j++)
        {
            exa->ptr.pp_double[i][j] = (double)(0);
        }
    }
    for(i=0; i<=nmain-1; i++)
    {
        for(j=i; j<=nmain-1; j++)
        {
            exa->ptr.pp_double[i][j] = sclsfta->ptr.pp_double[i][j];
        }
    }
    
    /*
     * Primary linear term
     */
    for(i=0; i<=ntotal-1; i++)
    {
        exb->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=nmain-1; i++)
    {
        exb->ptr.p_double[i] = sclsftb->ptr.p_double[i];
    }
    
    /*
     * Box constraints - move primary, add slack
     */
    for(i=0; i<=nmain-1; i++)
    {
        if( sclsfthasbndl->ptr.p_bool[i] )
        {
            exbndl->ptr.p_double[i] = sclsftbndl->ptr.p_double[i];
        }
        else
        {
            exbndl->ptr.p_double[i] = _state->v_neginf;
        }
        if( sclsfthasbndu->ptr.p_bool[i] )
        {
            exbndu->ptr.p_double[i] = sclsftbndu->ptr.p_double[i];
        }
        else
        {
            exbndu->ptr.p_double[i] = _state->v_posinf;
        }
    }
    for(i=nmain; i<=ntotal-1; i++)
    {
        exbndl->ptr.p_double[i] = (double)(0);
        exbndu->ptr.p_double[i] = _state->v_posinf;
    }
    
    /*
     * Handle equality constraints:
     * * modify quadratic term
     * * modify linear term
     * * add Lagrangian term
     */
    rmatrixsetlengthatleast(tmp2, sclsftnec+sclsftnic, ntotal, _state);
    for(i=0; i<=sclsftnec+sclsftnic-1; i++)
    {
        
        /*
         * Given constraint row ci and right hand side ri,
         * I-th quadratic constraint adds penalty term
         *
         *     0.5*Rho*(ci'*x-ri)^2 =
         *     = 0.5*Rho*(ci'*x-ri)^T*(ci'*x-ri) =
         *     = 0.5*Rho*(x'*ci-ri')*(ci'*x-ri) =
         *     = 0.5*Rho*(x'*ci*ci'*x - ri'*ci'*x - x'*ci*ri + ri'*ri )
         *     = 0.5*Rho*(x'*(ci*ci')*x - 2*ri*(ci'*x) + ri^2 )
         *
         * Thus, quadratic term is updated by
         *
         *     0.5*Rho*(ci*ci')
         *
         * (with actual update to ExA being performed without 0.5
         * multiplier because entire matrix is post-multipliead by 0.5)
         * and linear term receives update
         *
         *     -Rho*ri*ci
         *
         * Similaryly, lagrangian term is -NUi*(ci'*x-ri),
         * so linear term is updated by
         *
         *     -NUi*ci
         *
         * Because our model does not take into account constant term,
         * we calculate just quadratic and linear terms.
         */
        ae_v_move(&tmp2->ptr.pp_double[i][0], 1, &sclsftcleic->ptr.pp_double[i][0], 1, ae_v_len(0,nmain-1));
        for(j=nmain; j<=ntotal-1; j++)
        {
            tmp2->ptr.pp_double[i][j] = (double)(0);
        }
        if( i>=sclsftnec )
        {
            tmp2->ptr.pp_double[i][nmain+i-sclsftnec] = 1.0;
        }
        v = -rho*sclsftcleic->ptr.pp_double[i][nmain];
        ae_v_addd(&exb->ptr.p_double[0], 1, &tmp2->ptr.pp_double[i][0], 1, ae_v_len(0,ntotal-1), v);
        v = -nulc->ptr.p_double[i];
        ae_v_addd(&exb->ptr.p_double[0], 1, &tmp2->ptr.pp_double[i][0], 1, ae_v_len(0,ntotal-1), v);
    }
    rmatrixsyrk(ntotal, sclsftnec+sclsftnic, rho, tmp2, 0, 0, 2, 1.0, exa, 0, 0, ae_true, _state);
}


/*************************************************************************
This function generates initial point for  "extended"  box-constrained  QP
problem.

  -- ALGLIB --
     Copyright 23.02.2017 by Bochkanov Sergey
*************************************************************************/
static void qpdenseaulsolver_generateexinitialpoint(/* Real    */ const ae_vector* sclsftxc,
     ae_int_t nmain,
     ae_int_t nslack,
     /* Real    */ ae_vector* exxc,
     ae_state *_state)
{
    ae_int_t ntotal;
    ae_int_t i;


    ntotal = nmain+nslack;
    for(i=0; i<=ntotal-1; i++)
    {
        exxc->ptr.p_double[i] = (double)(0);
    }
    for(i=0; i<=nmain-1; i++)
    {
        exxc->ptr.p_double[i] = sclsftxc->ptr.p_double[i];
    }
}


/*************************************************************************
This function estimates Lagrange multipliers for scaled-shifted QP problem
(here "scaled-shifted"  means  that we  performed  variable  scaling   and
subtracted origin) given by quadratic term A, linear term B, box constraints
and linear constraint matrix.

It is assumed that all linear constraints are equality  ones,  with  first
NEC ones being constraints without slack  variables,  and  next  NIC  ones
having slack variables. The only inequality constraints we  have  are  box
ones, with first NMain ones being "general" box constraints, and next  NIC
ones being non-negativity constraints (not specified explicitly).

We also make use of the current point XC, which is used to determine active
box constraints.

Actual QP problem size is NMain+NIC, but   some  parameters   have   lower
dimensionality.

Parameters sizes are:
* A is assumed to be array[NMain,NMain]
* B is assumed to be array[NMain]
* BndL, BndU are array[NMain]
* CLEIC is array[NEC+NIC,NMain+1] (last item in a row containts right part)
* ExXC is array[NMain+NIC], holds current point
* NuLCEst is array[NEC+NIC], holds initial values of Lagrange coeffs

On exit NuLCEst is updated with new estimate of Lagrange multipliers.

  -- ALGLIB --
     Copyright 23.02.2017 by Bochkanov Sergey
*************************************************************************/
static void qpdenseaulsolver_updatelagrangemultipliers(/* Real    */ const ae_matrix* sclsfta,
     /* Real    */ const ae_vector* sclsftb,
     ae_int_t nmain,
     /* Real    */ const ae_vector* sclsftbndl,
     /* Boolean */ const ae_vector* sclsfthasbndl,
     /* Real    */ const ae_vector* sclsftbndu,
     /* Boolean */ const ae_vector* sclsfthasbndu,
     /* Real    */ const ae_matrix* sclsftcleic,
     ae_int_t sclsftnec,
     ae_int_t sclsftnic,
     /* Real    */ const ae_vector* exxc,
     /* Real    */ ae_vector* nulcest,
     qpdenseaulbuffers* buffers,
     ae_state *_state)
{
    ae_int_t nslack;
    ae_int_t ntotal;
    ae_int_t ktotal;
    ae_int_t nqrrows;
    ae_int_t nqrcols;
    ae_int_t i;
    ae_int_t j;
    double lambdareg;
    double mxdiag;
    double v;
    ae_bool isactive;


    nslack = sclsftnic;
    ntotal = nmain+nslack;
    ktotal = sclsftnec+sclsftnic;
    
    /*
     * Given current point ExXC, we can determine active and inactive
     * constraints. After we drop inactive inequality constraints, we
     * have equality-only constrained QP problem, with mix of general
     * linear equality constraints and "simple" constraints Xi=Ci.
     *
     * Problem min(0.5*x'*A*x + b'*x) s.t. C*x=d (general linear
     * constraints) can be solved by explicitly writing out Lagrange
     * equations:
     *
     *     [ A  C' ] [ X ]   [ -b]
     *     [       ] [   ] = [   ]
     *     [ C     ] [ L ]   [ d ]
     *
     * or
     *
     *         [ X ]
     *     A1* [   ] = b1
     *         [ L ]
     *
     * where X stands for solution itself, and L stands for Lagrange
     * multipliers. It can be easily solved with direct linear solver.
     * However, such formulation does not account for "simple" equality
     * constraints on variables. It is possible to include "simple"
     * constraints into "general" ones (i.e. append (0 ... 0 -1 0 ... 0)'
     * to the constraint matrix), but it will increase problem
     * size.
     *
     * Another approach is to use initial values of X and L (X0 and L0)
     * as starting point, and to solve for "offset" from (X0, L0):
     *
     *        [ X0+X1 ]
     *     A1*[       ] = b1
     *        [ L0+L1 ]
     *
     * or 
     *
     *        [ X1 ]           [ X0 ]
     *     A1*[    ] = b1 - A1*[    ]
     *        [ L1 ]           [ L0 ]
     *
     * In such formulation components of X1 which correspond to active
     * constraints on variables are "frozen" at value 0 (because we have
     * equality constraint, offset from constrained value have to be zero).
     *
     * Thus, we can rewrite corresponding columns of A1 with zeros - and
     * use this space to store (0 ... 0 -1 0 ... 0)', which is used to
     * account for Lagrange multipliers for "simple" constraints.
     */
    nqrcols = ntotal+ktotal;
    nqrrows = nqrcols;
    rvectorsetlengthatleast(&buffers->qrsv0, nqrcols, _state);
    rvectorsetlengthatleast(&buffers->qrsvx1, nqrcols, _state);
    for(i=0; i<=ntotal-1; i++)
    {
        buffers->qrsv0.ptr.p_double[i] = exxc->ptr.p_double[i];
    }
    for(i=0; i<=ktotal-1; i++)
    {
        buffers->qrsv0.ptr.p_double[ntotal+i] = nulcest->ptr.p_double[i];
    }
    rmatrixsetlengthatleast(&buffers->qrkkt, nqrcols+nqrcols, nqrcols+1, _state);
    rvectorsetlengthatleast(&buffers->qrrightpart, nqrcols+nqrcols, _state);
    lambdareg = 1.0E-8;
    for(;;)
    {
        
        /*
         * Initialize matrix A1 and right part b1 with zeros
         */
        for(i=0; i<=buffers->qrkkt.rows-1; i++)
        {
            for(j=0; j<=buffers->qrkkt.cols-1; j++)
            {
                buffers->qrkkt.ptr.pp_double[i][j] = (double)(0);
            }
            buffers->qrrightpart.ptr.p_double[i] = (double)(0);
        }
        
        /*
         * Append quadratic term (note: we implicitly add NSlack zeros to
         * A and b).
         */
        mxdiag = (double)(0);
        for(i=0; i<=nmain-1; i++)
        {
            for(j=0; j<=nmain-1; j++)
            {
                buffers->qrkkt.ptr.pp_double[i][j] = sclsfta->ptr.pp_double[i][j];
            }
            buffers->qrrightpart.ptr.p_double[i] = -sclsftb->ptr.p_double[i];
            mxdiag = ae_maxreal(mxdiag, ae_fabs(sclsfta->ptr.pp_double[i][i], _state), _state);
        }
        mxdiag = coalesce(mxdiag, (double)(1), _state);
        
        /*
         * Append general linear constraints
         */
        for(i=0; i<=ktotal-1; i++)
        {
            for(j=0; j<=nmain-1; j++)
            {
                buffers->qrkkt.ptr.pp_double[ntotal+i][j] = -sclsftcleic->ptr.pp_double[i][j];
                buffers->qrkkt.ptr.pp_double[j][ntotal+i] = -sclsftcleic->ptr.pp_double[i][j];
            }
            if( i>=sclsftnec )
            {
                buffers->qrkkt.ptr.pp_double[ntotal+i][nmain+(i-sclsftnec)] = (double)(-1);
                buffers->qrkkt.ptr.pp_double[nmain+(i-sclsftnec)][ntotal+i] = (double)(-1);
            }
            buffers->qrrightpart.ptr.p_double[ntotal+i] = -sclsftcleic->ptr.pp_double[i][nmain];
        }
        
        /*
         * Append regularizer to the bottom of the matrix
         * (it will be factored in during QR decomposition)
         */
        if( ae_fp_greater(lambdareg,(double)(0)) )
        {
            nqrrows = nqrcols+nqrcols;
            for(i=0; i<=nqrcols-1; i++)
            {
                buffers->qrkkt.ptr.pp_double[nqrcols+i][i] = lambdareg*mxdiag;
            }
        }
        
        /*
         * Subtract reference point (X0,L0) from the system
         */
        for(i=0; i<=nqrcols-1; i++)
        {
            v = ae_v_dotproduct(&buffers->qrkkt.ptr.pp_double[i][0], 1, &buffers->qrsv0.ptr.p_double[0], 1, ae_v_len(0,nqrcols-1));
            buffers->qrrightpart.ptr.p_double[i] = buffers->qrrightpart.ptr.p_double[i]-v;
        }
        
        /*
         * Handle active "simple" equality constraints
         */
        for(i=0; i<=ntotal-1; i++)
        {
            isactive = ae_false;
            if( i<nmain&&((sclsfthasbndl->ptr.p_bool[i]&&ae_fp_eq(exxc->ptr.p_double[i],sclsftbndl->ptr.p_double[i]))||(sclsfthasbndu->ptr.p_bool[i]&&ae_fp_eq(exxc->ptr.p_double[i],sclsftbndu->ptr.p_double[i]))) )
            {
                isactive = ae_true;
            }
            if( i>=nmain&&ae_fp_eq(exxc->ptr.p_double[i],0.0) )
            {
                isactive = ae_true;
            }
            if( !isactive )
            {
                continue;
            }
            for(j=0; j<=nqrrows-1; j++)
            {
                buffers->qrkkt.ptr.pp_double[j][i] = (double)(0);
            }
            buffers->qrkkt.ptr.pp_double[i][i] = (double)(-1);
        }
        
        /*
         * Solve via QR decomposition:
         * * append right part to the system matrix
         * * perform QR decomposition of the extended matrix (right part is implicitly
         *   multiplied by Q during decomposition; believe me, it works!)
         * * check condition number, increase regularization value if necessary and retry
         * * solve triangular system, break iteration
         */
        for(i=0; i<=nqrrows-1; i++)
        {
            buffers->qrkkt.ptr.pp_double[i][nqrcols] = buffers->qrrightpart.ptr.p_double[i];
        }
        rmatrixqr(&buffers->qrkkt, nqrrows, nqrcols+1, &buffers->qrtau, _state);
        if( ae_fp_less_eq(rmatrixtrrcond1(&buffers->qrkkt, nqrcols, ae_true, ae_false, _state),(double)1000*ae_machineepsilon) )
        {
            lambdareg = coalesce((double)10*lambdareg, 1.0E-13, _state);
            continue;
        }
        for(i=nqrcols-1; i>=0; i--)
        {
            v = buffers->qrkkt.ptr.pp_double[i][nqrcols];
            for(j=i+1; j<=nqrcols-1; j++)
            {
                v = v-buffers->qrkkt.ptr.pp_double[i][j]*buffers->qrsvx1.ptr.p_double[j];
            }
            buffers->qrsvx1.ptr.p_double[i] = v/buffers->qrkkt.ptr.pp_double[i][i];
        }
        break;
    }
    
    /*
     * Update Lagrange coefficients
     */
    for(i=0; i<=ktotal-1; i++)
    {
        nulcest->ptr.p_double[i] = buffers->qrsv0.ptr.p_double[ntotal+i]+buffers->qrsvx1.ptr.p_double[ntotal+i];
    }
}


/*************************************************************************
This function generates scaled (by S) and shifted (by XC) reformulation of
the original problem.

INPUT PARAMETERS:
    DenseA      -   for dense problems (AKind=0), A-term of CQM object
                    contains system matrix. Other terms are unspecified
                    and should not be referenced.
    SparseA     -   for sparse problems (AKind=1), CRS format
    AKind       -   sparse matrix format:
                    * 0 for dense matrix
                    * 1 for sparse matrix
    SparseUpper -   which triangle of SparseAC stores matrix  -  upper  or
                    lower one (for dense matrices this  parameter  is  not
                    actual).
    B           -   linear term, array[N]
    BndL        -   lower bound, array[N]
    BndU        -   upper bound, array[N]
    S           -   scale vector, array[NC]:
                    * I-th element contains scale of I-th variable,
                    * SC[I]>0
    XOrigin     -   origin term, array[NC]. Can be zero.
    N           -   number of variables in the  original  formulation  (no
                    slack variables).
    CLEIC       -   dense linear equality/inequality constraints. Equality
                    constraints come first.
    NEC, NIC    -   number of dense equality/inequality constraints.
    SCLEIC      -   sparse linear equality/inequality constraints. Equality
                    constraints come first.
    SNEC, SNIC  -   number of sparse equality/inequality constraints.
    RenormLC    -   whether constraints should be renormalized (recommended)
                    or used "as is".
    Settings    -   QPDENSEAULSettings object initialized by one of the initialization
                    functions.
    State       -   object which stores temporaries
    XS          -   initial point, array[NC]

    
On output, following fields of the State structure are modified:
* SclSftA       -   array[NMain,NMain], quadratic term, both triangles
* SclSftB       -   array[NMain], linear term
* SclSftXC      -   array[NMain], initial point
* SclSftHasBndL,
  SclSftHasBndU,
  SclSftBndL,
  SclSftBndU    -   array[NMain], lower/upper bounds
* SclSftCLEIC   -   array[KTotal,NMain+1], general linear constraints

NOTE: State.Tmp2 is used to store temporary array[NMain,NMain]

  -- ALGLIB --
     Copyright 01.10.2017 by Bochkanov Sergey
*************************************************************************/
static void qpdenseaulsolver_scaleshiftoriginalproblem(const convexquadraticmodel* a,
     const sparsematrix* sparsea,
     ae_int_t akind,
     ae_bool sparseaupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nmain,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t dnec,
     ae_int_t dnic,
     const sparsematrix* scleic,
     ae_int_t snec,
     ae_int_t snic,
     ae_bool renormlc,
     qpdenseaulbuffers* state,
     /* Real    */ ae_vector* xs,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t j0;
    ae_int_t j1;
    double v;
    double vv;
    ae_int_t ktotal;


    ae_assert(akind==0||akind==1, "QPDENSEAULOptimize: unexpected AKind", _state);
    ktotal = dnec+dnic+snec+snic;
    rmatrixsetlengthatleast(&state->sclsfta, nmain, nmain, _state);
    rvectorsetlengthatleast(&state->sclsftb, nmain, _state);
    rvectorsetlengthatleast(&state->sclsftxc, nmain, _state);
    rvectorsetlengthatleast(&state->sclsftbndl, nmain, _state);
    rvectorsetlengthatleast(&state->sclsftbndu, nmain, _state);
    bvectorsetlengthatleast(&state->sclsfthasbndl, nmain, _state);
    bvectorsetlengthatleast(&state->sclsfthasbndu, nmain, _state);
    rmatrixsetlengthatleast(&state->sclsftcleic, ktotal, nmain+1, _state);
    rvectorsetlengthatleast(&state->cscales, ktotal, _state);
    if( akind==0 )
    {
        
        /*
         * Extract dense A and scale
         */
        cqmgeta(a, &state->tmp2, _state);
        for(i=0; i<=nmain-1; i++)
        {
            for(j=0; j<=nmain-1; j++)
            {
                state->sclsfta.ptr.pp_double[i][j] = (double)(0);
            }
        }
        for(i=0; i<=nmain-1; i++)
        {
            for(j=i; j<=nmain-1; j++)
            {
                v = state->tmp2.ptr.pp_double[i][j]*s->ptr.p_double[i]*s->ptr.p_double[j];
                state->sclsfta.ptr.pp_double[i][j] = v;
                state->sclsfta.ptr.pp_double[j][i] = v;
            }
        }
    }
    if( akind==1 )
    {
        
        /*
         * Extract sparse A and scale
         */
        ae_assert(sparsea->matrixtype==1, "QPDENSEAULOptimize: unexpected sparse matrix format", _state);
        ae_assert(sparsea->m==nmain, "QPDENSEAULOptimize: unexpected sparse matrix size", _state);
        ae_assert(sparsea->n==nmain, "QPDENSEAULOptimize: unexpected sparse matrix size", _state);
        for(i=0; i<=nmain-1; i++)
        {
            for(j=0; j<=nmain-1; j++)
            {
                state->sclsfta.ptr.pp_double[i][j] = (double)(0);
            }
        }
        if( sparseaupper )
        {
            for(i=0; i<=nmain-1; i++)
            {
                if( sparsea->didx.ptr.p_int[i]!=sparsea->uidx.ptr.p_int[i] )
                {
                    state->sclsfta.ptr.pp_double[i][i] = sparsea->vals.ptr.p_double[sparsea->didx.ptr.p_int[i]]*s->ptr.p_double[i]*s->ptr.p_double[i];
                }
                j0 = sparsea->uidx.ptr.p_int[i];
                j1 = sparsea->ridx.ptr.p_int[i+1]-1;
                for(j=j0; j<=j1; j++)
                {
                    k = sparsea->idx.ptr.p_int[j];
                    v = sparsea->vals.ptr.p_double[j]*s->ptr.p_double[i]*s->ptr.p_double[k];
                    state->sclsfta.ptr.pp_double[i][k] = v;
                    state->sclsfta.ptr.pp_double[k][i] = v;
                }
            }
        }
        else
        {
            for(i=0; i<=nmain-1; i++)
            {
                if( sparsea->didx.ptr.p_int[i]!=sparsea->uidx.ptr.p_int[i] )
                {
                    state->sclsfta.ptr.pp_double[i][i] = sparsea->vals.ptr.p_double[sparsea->didx.ptr.p_int[i]]*s->ptr.p_double[i]*s->ptr.p_double[i];
                }
                j0 = sparsea->ridx.ptr.p_int[i];
                j1 = sparsea->didx.ptr.p_int[i]-1;
                for(j=j0; j<=j1; j++)
                {
                    k = sparsea->idx.ptr.p_int[j];
                    v = sparsea->vals.ptr.p_double[j]*s->ptr.p_double[i]*s->ptr.p_double[k];
                    state->sclsfta.ptr.pp_double[i][k] = v;
                    state->sclsfta.ptr.pp_double[k][i] = v;
                }
            }
        }
    }
    for(i=0; i<=nmain-1; i++)
    {
        state->sclsftb.ptr.p_double[i] = b->ptr.p_double[i]*s->ptr.p_double[i];
        state->sclsftxc.ptr.p_double[i] = (xs->ptr.p_double[i]-xorigin->ptr.p_double[i])/s->ptr.p_double[i];
        state->sclsfthasbndl.ptr.p_bool[i] = ae_isfinite(bndl->ptr.p_double[i], _state);
        state->sclsfthasbndu.ptr.p_bool[i] = ae_isfinite(bndu->ptr.p_double[i], _state);
        state->sclsftbndl.ptr.p_double[i] = bndl->ptr.p_double[i];
        state->sclsftbndu.ptr.p_double[i] = bndu->ptr.p_double[i];
    }
    scaleshiftbcinplace(s, xorigin, &state->sclsftbndl, &state->sclsftbndu, nmain, _state);
    for(i=0; i<=ktotal-1; i++)
    {
        for(j=0; j<=nmain; j++)
        {
            state->sclsftcleic.ptr.pp_double[i][j] = (double)(0);
        }
    }
    for(i=0; i<=dnec-1; i++)
    {
        for(j=0; j<=nmain-1; j++)
        {
            v = cleic->ptr.pp_double[i][j]*s->ptr.p_double[j];
            state->sclsftcleic.ptr.pp_double[i][j] = v;
        }
        state->sclsftcleic.ptr.pp_double[i][nmain] = cleic->ptr.pp_double[i][nmain];
    }
    for(i=0; i<=dnic-1; i++)
    {
        for(j=0; j<=nmain-1; j++)
        {
            v = cleic->ptr.pp_double[dnec+i][j]*s->ptr.p_double[j];
            state->sclsftcleic.ptr.pp_double[dnec+snec+i][j] = v;
        }
        state->sclsftcleic.ptr.pp_double[dnec+snec+i][nmain] = cleic->ptr.pp_double[dnec+i][nmain];
    }
    for(i=0; i<=snec-1; i++)
    {
        
        /*
         * Because constraints are sparse, everything is a bit tricky -
         * it is possible that N-th element of the row is zero and not
         * stored; it is also possible that entire row is empty.
         */
        j0 = scleic->ridx.ptr.p_int[i];
        j1 = scleic->ridx.ptr.p_int[i+1]-1;
        if( j1>=j0&&scleic->idx.ptr.p_int[j1]==nmain )
        {
            state->sclsftcleic.ptr.pp_double[dnec+i][nmain] = scleic->vals.ptr.p_double[j1];
            j1 = j1-1;
        }
        for(j=j0; j<=j1; j++)
        {
            k = scleic->idx.ptr.p_int[j];
            v = scleic->vals.ptr.p_double[j]*s->ptr.p_double[k];
            state->sclsftcleic.ptr.pp_double[dnec+i][k] = v;
        }
    }
    for(i=0; i<=snic-1; i++)
    {
        
        /*
         * Because constraints are sparse, everything is a bit tricky -
         * it is possible that N-th element of the row is zero and not
         * stored; it is also possible that entire row is empty.
         */
        j0 = scleic->ridx.ptr.p_int[snec+i];
        j1 = scleic->ridx.ptr.p_int[snec+i+1]-1;
        if( j1>=j0&&scleic->idx.ptr.p_int[j1]==nmain )
        {
            state->sclsftcleic.ptr.pp_double[dnec+snec+dnic+i][nmain] = scleic->vals.ptr.p_double[j1];
            j1 = j1-1;
        }
        for(j=j0; j<=j1; j++)
        {
            k = scleic->idx.ptr.p_int[j];
            v = scleic->vals.ptr.p_double[j]*s->ptr.p_double[k];
            state->sclsftcleic.ptr.pp_double[dnec+snec+dnic+i][k] = v;
        }
    }
    if( renormlc&&ktotal>0 )
    {
        
        /*
         * Normalize linear constraints in such way that they have unit norm
         * (after variable scaling)
         */
        for(i=0; i<=ktotal-1; i++)
        {
            vv = 0.0;
            for(j=0; j<=nmain-1; j++)
            {
                v = state->sclsftcleic.ptr.pp_double[i][j];
                vv = vv+v*v;
            }
            vv = ae_sqrt(vv, _state);
            state->cscales.ptr.p_double[i] = vv;
            if( ae_fp_greater(vv,(double)(0)) )
            {
                vv = (double)1/vv;
                for(j=0; j<=nmain; j++)
                {
                    state->sclsftcleic.ptr.pp_double[i][j] = state->sclsftcleic.ptr.pp_double[i][j]*vv;
                }
            }
        }
    }
    else
    {
        
        /*
         * Load unit scales
         */
        for(i=0; i<=ktotal-1; i++)
        {
            state->cscales.ptr.p_double[i] = 1.0;
        }
    }
    for(i=0; i<=ktotal-1; i++)
    {
        
        /*
         * Apply XOrigin
         */
        v = 0.0;
        for(j=0; j<=nmain-1; j++)
        {
            v = v+state->sclsftcleic.ptr.pp_double[i][j]*(xorigin->ptr.p_double[j]/s->ptr.p_double[j]);
        }
        state->sclsftcleic.ptr.pp_double[i][nmain] = state->sclsftcleic.ptr.pp_double[i][nmain]-v;
    }
}


/*************************************************************************
Normalize model in such way that norm(A)~1 (very roughly)

We have two lower bounds for sigma_max(A):
* first estimate is provided by Frobenius norm, it is equal to ANorm/NMain
* second estimate is provided by max(CAC)

We select largest one of these estimates, because using just one
of them is prone to different failure modes. Then, we divide A and B
by this estimate.

INPUT PARAMETERS:
    A   -       array[N,N], quadratic term, full triangle is given
    B   -       array[N], linear term
    N   -       problem size
    CLEIC-      array[NEC+NIC,N+1], linear equality/inequality constraints
    NEC -       number of equality constraints
    NIC -       number of inequality constraints
    UseCLEIC-   additional normalization of A in such way that CLEIC*A*CLEIC'~1:
                * if False, CLEIC is ignored
                * if True, CLEIC rows MUST have unit norm (we check it)
    Tmp2-       additional buffer, possibly preallocated
    
OUTPUT PARAMETERS:
    A, B - appropriately rescaled by 1/SCL
    
RESULT:
    multiplier SCL

  -- ALGLIB --
     Copyright 01.10.2017 by Bochkanov Sergey
*************************************************************************/
static double qpdenseaulsolver_normalizequadraticterm(/* Real    */ ae_matrix* a,
     /* Real    */ ae_vector* b,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     ae_bool usecleic,
     /* Real    */ ae_matrix* tmp2,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double anorm;
    double maxcac;
    double v;
    double vv;
    ae_int_t ktotal;
    ae_int_t nmain;
    double result;


    nmain = n;
    ktotal = nec+nic;
    anorm = (double)(0);
    for(i=0; i<=nmain-1; i++)
    {
        for(j=0; j<=nmain-1; j++)
        {
            anorm = anorm+ae_sqr(a->ptr.pp_double[i][j], _state);
        }
    }
    anorm = ae_sqrt(anorm, _state);
    if( usecleic&&ktotal>0 )
    {
        
        /*
         * Calculate max(|diag(C*A*C')|), where C is constraint matrix
         */
        rmatrixsetlengthatleast(tmp2, ktotal, nmain, _state);
        rmatrixgemm(ktotal, nmain, nmain, 1.0, cleic, 0, 0, 0, a, 0, 0, 0, 0.0, tmp2, 0, 0, _state);
        maxcac = 0.0;
        for(i=0; i<=ktotal-1; i++)
        {
            v = (double)(0);
            vv = (double)(0);
            for(j=0; j<=nmain-1; j++)
            {
                v = v+tmp2->ptr.pp_double[i][j]*cleic->ptr.pp_double[i][j];
                vv = vv+ae_sqr(cleic->ptr.pp_double[i][j], _state);
            }
            ae_assert(ae_fp_less(ae_fabs(vv-(double)1, _state),1.0E-9)||ae_fp_eq(vv,(double)(0)), "DENSE-AUL: integrity check failed", _state);
            maxcac = ae_maxreal(maxcac, ae_fabs(v, _state), _state);
        }
    }
    else
    {
        maxcac = (double)(0);
    }
    result = coalesce(ae_maxreal(maxcac, anorm/(double)nmain, _state), (double)(1), _state);
    v = (double)1/result;
    for(i=0; i<=nmain-1; i++)
    {
        for(j=0; j<=nmain-1; j++)
        {
            a->ptr.pp_double[i][j] = a->ptr.pp_double[i][j]*v;
        }
    }
    for(i=0; i<=nmain-1; i++)
    {
        b->ptr.p_double[i] = b->ptr.p_double[i]*v;
    }
    return result;
}


/*************************************************************************
This function selects initial working set of general inequality constraints
for QP problem:
* for non-convex QP problems    -   NICWork=NIC is returned
* otherwise                     -   NICWork=0 is returned (we have to
                                    determine working set iteratively)

INPUT PARAMETERS:
    A           -   array[NMain], quadratic term, full matrix is stored
    NMain       -   number of variables in the "original" QP problem
    CLEIC       -   array[NEC+NIC,NMain+1], constraint matrix
    NEC         -   number of equality constraints
    NIC         -   number of inequality constraints

OUTPUT PARAMETERS:
    NICWork     -   recommended size of working set; in current version
                    either all (NICWork=NIC) or none (NICWork=0) constraints
                    are included.
    AllowWSEviction-whether problem properties allow eviction of constraints
                    from working set or not. Non-convex problems do not
                    allow eviction, convex ones do.

  -- ALGLIB --
     Copyright 02.10.2017 by Bochkanov Sergey
*************************************************************************/
static void qpdenseaulsolver_selectinitialworkingset(/* Real    */ const ae_matrix* a,
     ae_int_t nmain,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     /* Real    */ ae_vector* tmp0,
     /* Real    */ ae_matrix* tmp2,
     ae_int_t* nicwork,
     ae_bool* allowwseviction,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    *nicwork = 0;
    *allowwseviction = ae_false;

    rmatrixsetlengthatleast(tmp2, nmain, nmain, _state);
    rvectorsetlengthatleast(tmp0, nmain, _state);
    for(i=0; i<=nmain-1; i++)
    {
        for(j=i; j<=nmain-1; j++)
        {
            tmp2->ptr.pp_double[i][j] = a->ptr.pp_double[i][j];
        }
    }
    if( !spdmatrixcholeskyrec(tmp2, 0, nmain, ae_true, tmp0, _state) )
    {
        
        /*
         * Matrix is indefinite.
         *
         * We have to select full working set, otherwise algorithm may fail
         * because problem with reduced working set can be unbounded from below.
         */
        *nicwork = nic;
        *allowwseviction = ae_false;
    }
    else
    {
        
        /*
         * Positive definite matrix.
         *
         * We can select zero initial working set and expand it later.
         */
        *nicwork = 0;
        *allowwseviction = ae_true;
    }
}


void _qpdenseaulsettings_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    qpdenseaulsettings *p = (qpdenseaulsettings*)_p;
    ae_touch_ptr((void*)p);
}


void _qpdenseaulsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    qpdenseaulsettings       *dst = (qpdenseaulsettings*)_dst;
    const qpdenseaulsettings *src = (const qpdenseaulsettings*)_src;
    dst->epsx = src->epsx;
    dst->outerits = src->outerits;
    dst->rho = src->rho;
}


void _qpdenseaulsettings_clear(void* _p)
{
    qpdenseaulsettings *p = (qpdenseaulsettings*)_p;
    ae_touch_ptr((void*)p);
}


void _qpdenseaulsettings_destroy(void* _p)
{
    qpdenseaulsettings *p = (qpdenseaulsettings*)_p;
    ae_touch_ptr((void*)p);
}


void _qpdenseaulbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    qpdenseaulbuffers *p = (qpdenseaulbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->nulc, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->sclsfta, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sclsftb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sclsfthasbndl, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->sclsfthasbndu, 0, DT_BOOL, _state, make_automatic);
    ae_vector_init(&p->sclsftbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sclsftbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->sclsftxc, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->sclsftcleic, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cidx, 0, DT_INT, _state, make_automatic);
    ae_vector_init(&p->cscales, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->exa, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->exb, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->exxc, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->exbndl, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->exbndu, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->exscale, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->exxorigin, 0, DT_REAL, _state, make_automatic);
    _qqpsettings_init(&p->qqpsettingsuser, _state, make_automatic);
    _qqpbuffers_init(&p->qqpbuf, _state, make_automatic);
    ae_vector_init(&p->nulcest, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpg, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmp0, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->tmp2, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->modelg, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->deltax, 0, DT_REAL, _state, make_automatic);
    _convexquadraticmodel_init(&p->dummycqm, _state, make_automatic);
    _sparsematrix_init(&p->dummysparse, _state, make_automatic);
    ae_matrix_init(&p->qrkkt, 0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->qrrightpart, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->qrtau, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->qrsv0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->qrsvx1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nicerr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->nicnact, 0, DT_INT, _state, make_automatic);
}


void _qpdenseaulbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    qpdenseaulbuffers       *dst = (qpdenseaulbuffers*)_dst;
    const qpdenseaulbuffers *src = (const qpdenseaulbuffers*)_src;
    ae_vector_init_copy(&dst->nulc, &src->nulc, _state, make_automatic);
    ae_matrix_init_copy(&dst->sclsfta, &src->sclsfta, _state, make_automatic);
    ae_vector_init_copy(&dst->sclsftb, &src->sclsftb, _state, make_automatic);
    ae_vector_init_copy(&dst->sclsfthasbndl, &src->sclsfthasbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->sclsfthasbndu, &src->sclsfthasbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->sclsftbndl, &src->sclsftbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->sclsftbndu, &src->sclsftbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->sclsftxc, &src->sclsftxc, _state, make_automatic);
    ae_matrix_init_copy(&dst->sclsftcleic, &src->sclsftcleic, _state, make_automatic);
    ae_vector_init_copy(&dst->cidx, &src->cidx, _state, make_automatic);
    ae_vector_init_copy(&dst->cscales, &src->cscales, _state, make_automatic);
    ae_matrix_init_copy(&dst->exa, &src->exa, _state, make_automatic);
    ae_vector_init_copy(&dst->exb, &src->exb, _state, make_automatic);
    ae_vector_init_copy(&dst->exxc, &src->exxc, _state, make_automatic);
    ae_vector_init_copy(&dst->exbndl, &src->exbndl, _state, make_automatic);
    ae_vector_init_copy(&dst->exbndu, &src->exbndu, _state, make_automatic);
    ae_vector_init_copy(&dst->exscale, &src->exscale, _state, make_automatic);
    ae_vector_init_copy(&dst->exxorigin, &src->exxorigin, _state, make_automatic);
    _qqpsettings_init_copy(&dst->qqpsettingsuser, &src->qqpsettingsuser, _state, make_automatic);
    _qqpbuffers_init_copy(&dst->qqpbuf, &src->qqpbuf, _state, make_automatic);
    ae_vector_init_copy(&dst->nulcest, &src->nulcest, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpg, &src->tmpg, _state, make_automatic);
    ae_vector_init_copy(&dst->tmp0, &src->tmp0, _state, make_automatic);
    ae_matrix_init_copy(&dst->tmp2, &src->tmp2, _state, make_automatic);
    ae_vector_init_copy(&dst->modelg, &src->modelg, _state, make_automatic);
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    ae_vector_init_copy(&dst->deltax, &src->deltax, _state, make_automatic);
    _convexquadraticmodel_init_copy(&dst->dummycqm, &src->dummycqm, _state, make_automatic);
    _sparsematrix_init_copy(&dst->dummysparse, &src->dummysparse, _state, make_automatic);
    ae_matrix_init_copy(&dst->qrkkt, &src->qrkkt, _state, make_automatic);
    ae_vector_init_copy(&dst->qrrightpart, &src->qrrightpart, _state, make_automatic);
    ae_vector_init_copy(&dst->qrtau, &src->qrtau, _state, make_automatic);
    ae_vector_init_copy(&dst->qrsv0, &src->qrsv0, _state, make_automatic);
    ae_vector_init_copy(&dst->qrsvx1, &src->qrsvx1, _state, make_automatic);
    ae_vector_init_copy(&dst->nicerr, &src->nicerr, _state, make_automatic);
    ae_vector_init_copy(&dst->nicnact, &src->nicnact, _state, make_automatic);
    dst->repinneriterationscount = src->repinneriterationscount;
    dst->repouteriterationscount = src->repouteriterationscount;
    dst->repncholesky = src->repncholesky;
    dst->repnwrkchanges = src->repnwrkchanges;
    dst->repnwrk0 = src->repnwrk0;
    dst->repnwrk1 = src->repnwrk1;
    dst->repnwrkf = src->repnwrkf;
    dst->repnmv = src->repnmv;
}


void _qpdenseaulbuffers_clear(void* _p)
{
    qpdenseaulbuffers *p = (qpdenseaulbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->nulc);
    ae_matrix_clear(&p->sclsfta);
    ae_vector_clear(&p->sclsftb);
    ae_vector_clear(&p->sclsfthasbndl);
    ae_vector_clear(&p->sclsfthasbndu);
    ae_vector_clear(&p->sclsftbndl);
    ae_vector_clear(&p->sclsftbndu);
    ae_vector_clear(&p->sclsftxc);
    ae_matrix_clear(&p->sclsftcleic);
    ae_vector_clear(&p->cidx);
    ae_vector_clear(&p->cscales);
    ae_matrix_clear(&p->exa);
    ae_vector_clear(&p->exb);
    ae_vector_clear(&p->exxc);
    ae_vector_clear(&p->exbndl);
    ae_vector_clear(&p->exbndu);
    ae_vector_clear(&p->exscale);
    ae_vector_clear(&p->exxorigin);
    _qqpsettings_clear(&p->qqpsettingsuser);
    _qqpbuffers_clear(&p->qqpbuf);
    ae_vector_clear(&p->nulcest);
    ae_vector_clear(&p->tmpg);
    ae_vector_clear(&p->tmp0);
    ae_matrix_clear(&p->tmp2);
    ae_vector_clear(&p->modelg);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->deltax);
    _convexquadraticmodel_clear(&p->dummycqm);
    _sparsematrix_clear(&p->dummysparse);
    ae_matrix_clear(&p->qrkkt);
    ae_vector_clear(&p->qrrightpart);
    ae_vector_clear(&p->qrtau);
    ae_vector_clear(&p->qrsv0);
    ae_vector_clear(&p->qrsvx1);
    ae_vector_clear(&p->nicerr);
    ae_vector_clear(&p->nicnact);
}


void _qpdenseaulbuffers_destroy(void* _p)
{
    qpdenseaulbuffers *p = (qpdenseaulbuffers*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->nulc);
    ae_matrix_destroy(&p->sclsfta);
    ae_vector_destroy(&p->sclsftb);
    ae_vector_destroy(&p->sclsfthasbndl);
    ae_vector_destroy(&p->sclsfthasbndu);
    ae_vector_destroy(&p->sclsftbndl);
    ae_vector_destroy(&p->sclsftbndu);
    ae_vector_destroy(&p->sclsftxc);
    ae_matrix_destroy(&p->sclsftcleic);
    ae_vector_destroy(&p->cidx);
    ae_vector_destroy(&p->cscales);
    ae_matrix_destroy(&p->exa);
    ae_vector_destroy(&p->exb);
    ae_vector_destroy(&p->exxc);
    ae_vector_destroy(&p->exbndl);
    ae_vector_destroy(&p->exbndu);
    ae_vector_destroy(&p->exscale);
    ae_vector_destroy(&p->exxorigin);
    _qqpsettings_destroy(&p->qqpsettingsuser);
    _qqpbuffers_destroy(&p->qqpbuf);
    ae_vector_destroy(&p->nulcest);
    ae_vector_destroy(&p->tmpg);
    ae_vector_destroy(&p->tmp0);
    ae_matrix_destroy(&p->tmp2);
    ae_vector_destroy(&p->modelg);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->deltax);
    _convexquadraticmodel_destroy(&p->dummycqm);
    _sparsematrix_destroy(&p->dummysparse);
    ae_matrix_destroy(&p->qrkkt);
    ae_vector_destroy(&p->qrrightpart);
    ae_vector_destroy(&p->qrtau);
    ae_vector_destroy(&p->qrsv0);
    ae_vector_destroy(&p->qrsvx1);
    ae_vector_destroy(&p->nicerr);
    ae_vector_destroy(&p->nicnact);
}


/*$ End $*/
