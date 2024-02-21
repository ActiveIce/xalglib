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

#ifndef _nlcaul_h
#define _nlcaul_h

#include "aenv.h"
#include "ialglib.h"
#include "kernels.h"
#include "smp.h"
#include "kernels_hpc.h"
#include "linmin.h"
#include "apserv.h"
#include "ablasf.h"
#include "tsort.h"
#include "optguardapi.h"
#include "ablasmkl.h"
#include "ablas.h"
#include "creflections.h"
#include "xdebug.h"
#include "hqrnd.h"
#include "matgen.h"
#include "scodes.h"
#include "sparse.h"
#include "dlu.h"
#include "sptrf.h"
#include "apstruct.h"
#include "amdordering.h"
#include "spchol.h"
#include "rotations.h"
#include "trfac.h"
#include "hblas.h"
#include "sblas.h"
#include "ortfac.h"
#include "blas.h"
#include "bdsvd.h"
#include "svd.h"
#include "hsschur.h"
#include "basicstatops.h"
#include "evd.h"
#include "trlinsolve.h"
#include "safesolve.h"
#include "rcond.h"
#include "matinv.h"
#include "optserv.h"
#include "lpqpserv.h"


/*$ Declarations $*/


/*************************************************************************
This object stores temporaries of AUL preconditioner.
*************************************************************************/
typedef struct
{
    ae_int_t refreshfreq;
    ae_int_t modelage;
    spcholanalysis analysis;
    sparsematrix augsys;
    ae_int_t naug;
    ae_matrix s;
    ae_matrix y;
    ae_int_t updcnt;
    ae_matrix tmpcorrc;
    ae_vector tmpcorrd;
    ae_vector tmpdiag;
    ae_vector priorities;
    ae_vector dx;
    ae_matrix dummys;
    ae_matrix dummyy;
    ae_vector alphak;
    ae_vector rhok;
} minaulpreconditioner;


/*************************************************************************
This object stores temporaries of AUL solver.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t cntlc;
    ae_int_t cntnlc;
    ae_vector s;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_vector finitebndl;
    ae_vector finitebndu;
    sparsematrix sparsea;
    ae_vector al;
    ae_vector au;
    ae_vector hasal;
    ae_vector hasau;
    ae_vector rawnl;
    ae_vector rawnu;
    ae_vector hasnl;
    ae_vector hasnu;
    ae_vector lcsrcidx;
    nlpstoppingcriteria criteria;
    ae_int_t maxouterits;
    ae_int_t restartfreq;
    ae_vector x;
    ae_vector fi;
    sparsematrix sj;
    double f;
    ae_bool needsj;
    ae_bool precrefreshupcoming;
    ae_bool xupdated;
    double rho;
    varsfuncjac xvirt;
    varsfuncjac xvirtprev;
    varsfuncjac xvirtbest;
    varsfuncjac xtrue;
    varsfuncjac xtrueprev;
    varsfuncjac xtruebest;
    double besterr;
    ae_int_t bestiteridx;
    ae_int_t besterrnegligibleupdates;
    ae_vector lagmultbc2;
    ae_vector lagmultxc2;
    ae_vector x0;
    ae_vector d;
    ae_vector du;
    ae_vector fscales;
    ae_vector tracegamma;
    minaulpreconditioner preconditioner;
    xbfgshessian hessaug;
    xbfgshessian hesstgt;
    ae_vector laggrad0;
    ae_vector laggradcur;
    ae_vector modtgtgrad0;
    ae_vector modtgtgrad1;
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
    ae_int_t fstagnationcnt;
    double longeststp;
    ae_int_t mcstage;
    linminstate lstate;
    double stp;
    double stplimit;
    ae_vector ascales;
    ae_vector tmpzero;
    ae_vector tmpy;
    ae_matrix dummy2;
    ae_vector tmpretrdelta;
    ae_vector tmpg;
    varsfuncjac xvirtprobe;
    varsfuncjac xtrueprobe;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repnfev;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_int_t nfev;
    ae_vector work;
    rcommstate rstate;
} minaulstate;


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
     ae_state *_state);


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
     ae_state *_state);


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
     ae_state *_state);
void _minaulpreconditioner_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minaulpreconditioner_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minaulpreconditioner_clear(void* _p);
void _minaulpreconditioner_destroy(void* _p);
void _minaulstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minaulstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minaulstate_clear(void* _p);
void _minaulstate_destroy(void* _p);


/*$ End $*/
#endif

