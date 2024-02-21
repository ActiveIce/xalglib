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

#ifndef _nlcsqp_h
#define _nlcsqp_h

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
#include "xblas.h"
#include "directdensesolvers.h"
#include "fbls.h"
#include "minlbfgs.h"
#include "cqmodels.h"
#include "lpqpserv.h"
#include "vipmsolver.h"
#include "ipm2solver.h"


/*$ Declarations $*/


/*************************************************************************
This object stores temporaries of SQP subsolver.
*************************************************************************/
typedef struct
{
    vipmstate ipmsolver;
    ipm2state ipm2;
    ae_vector curb;
    ae_vector curbndl;
    ae_vector curbndu;
    ae_vector cural;
    ae_vector curau;
    sparsematrix sparserawlc;
    sparsematrix sparseefflc;
    ae_vector d0;
    ae_matrix denseh;
    ae_vector dummy1;
    ae_matrix densedummy;
    sparsematrix sparsedummy;
    ae_vector tmps;
    ae_vector tmporigin;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector retainnegativebclm;
    ae_vector retainpositivebclm;
    ae_vector rescalelag;
    ae_matrix tmpcorrc;
    ae_vector tmpdiag;
    ae_vector hessdiag;
    ae_matrix hesscorrc;
    ae_vector hesscorrd;
    ae_int_t hessrank;
    sparsematrix hesssparsediag;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector hasal;
    ae_vector hasau;
} minsqpsubsolver;


/*************************************************************************
This object stores temporaries of SQP solver.
*************************************************************************/
typedef struct
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_bool usedensebfgs;
    ae_vector s;
    ae_matrix scaledcleic;
    ae_vector lcsrcidx;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    nlpstoppingcriteria criteria;
    ae_int_t bfgsresetfreq;
    ae_vector x;
    ae_vector fi;
    ae_matrix j;
    double f;
    ae_bool needfij;
    ae_bool xupdated;
    double meritmu;
    double trustrad;
    double trustradgrowth;
    ae_int_t trustradstagnationcnt;
    ae_int_t fstagnationcnt;
    varsfuncjac stepk;
    ae_vector x0;
    ae_vector xprev;
    ae_vector fscales;
    ae_vector tracegamma;
    minsqpsubsolver subsolver;
    xbfgshessian hess;
    ae_obj_array nonmonotonicmem;
    ae_int_t nonmonotoniccnt;
    ae_vector lagbcmult;
    ae_vector lagxcmult;
    varsfuncjac cand;
    varsfuncjac corr;
    varsfuncjac probe;
    varsfuncjac currentlinearization;
    ae_vector dtrial;
    ae_vector d0;
    ae_vector d1;
    ae_vector dmu;
    ae_vector tmppend;
    ae_vector tmphd;
    ae_vector dummylagbcmult;
    ae_vector dummylagxcmult;
    ae_vector tmplaggrad;
    ae_vector tmpcandlaggrad;
    ae_vector tmphdiag;
    ae_vector sclagtmp0;
    ae_vector sclagtmp1;
    ae_vector mftmp0;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    stimer timertotal;
    stimer timerqp;
    stimer timercallback;
    rcommstate rstate;
} minsqpstate;


/*$ Body $*/


void minsqpinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     const nlpstoppingcriteria* criteria,
     ae_bool usedensebfgs,
     minsqpstate* state,
     ae_state *_state);


/*************************************************************************
This function performs actual processing for  SQP  algorithm.  It  expects
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
ae_bool minsqpiteration(minsqpstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state);
void _minsqpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqpsubsolver_clear(void* _p);
void _minsqpsubsolver_destroy(void* _p);
void _minsqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqpstate_clear(void* _p);
void _minsqpstate_destroy(void* _p);


/*$ End $*/
#endif

